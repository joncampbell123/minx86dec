#include "minx86dec/types.h"
#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"
#include "minx86dec/core8086.h"
#include "minx86dec/opcodes_str.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>

#include <isp-utils/text/sliding_window.h>

uint8_t memory[640*1024];

#define DEBUG		1

#define PREFETCH_SIZE	5

/* emulator state */
struct emu8086_segment {
	uint32_t	base;
	uint16_t	value;
};

/* general purpose register file */
union emu8086_regs {
	uint16_t		i[8];		/* [MX86_REG_AX] or such */
	struct {
		uint16_t	ax,cx,dx,bx,sp,bp,si,di;
	} __attribute__((packed)) n;
};

/* We're emulating an 8-bit datapath, else I'd add flags to say whether we
 * are fetching 8 bits or 16 bits */
struct emu8086_event {
	unsigned int		event;
	unsigned int		event_duration;	/* in clocks */
	union {
		struct {
			uint32_t	addr;
			uint32_t	reason;
		} mem;
		uint16_t	io_addr;
	} e;
	unsigned char		decoded_ins;
};

struct emu8086_memio {
	struct emu8086_state		*cpu;
	struct emu8086_event		*event;
	uint32_t			addr;
	unsigned char			data;
	unsigned char			write;
};

struct emu8086_state {
	/* register file */
	uint16_t			flags;		/* FLAGS */
	union emu8086_regs		reg;
	struct emu8086_segment		sreg[4];	/* NOTE: up to DS == MX86_SEG_DS == 3 don't use FS/GS refs! */
	/* decompiler state */
	struct {
		uint32_t		base;
		uint16_t		value;
		uint32_t		prefetch;
	} ip;
	sliding_window*			prefetch;	/* mimics the prefetch buffer. all decompilation
							   happens from here, not directly from memory.
							   just as the CPU must fetch memory to decode */
	unsigned int			reset;		/* countdown of clocks to carry out reset */
	unsigned int			reset_clock;
	uint32_t			clocks;
	struct minx86dec_state		decoder;
	struct minx86dec_instruction	last_ins;
	struct emu8086_memio		prefetch_memio;
	uint8_t				(*memio_r)(struct emu8086_memio *m);
};

enum {
	EMU86_EV_RESET=0,
	EMU86_EV_IDLE
};

enum {
	EMU86_EV_MEMREASON_NORMAL=0,
	EMU86_EV_MEMREASON_PREFETCH
};

void emu8086_segment_set(struct emu8086_segment *s,uint16_t val) {
	s->base = (uint32_t)val << 4;
	s->value = val;
}

void emu8086_state_reset(struct emu8086_state *s) {
	s->reset_clock = 400;	/* FIXME: how many cycles does it really take? */
}

void emu8086_state_assert_reset(struct emu8086_state *s) {
	if (!s->reset) {
		s->reset = 1;
		emu8086_state_reset(s);
	}
}

void emu8086_state_deassert_reset(struct emu8086_state *s) {
	s->reset = 0;
}

void emu8086_state_flush_prefetch(struct emu8086_state *s) {
	s->prefetch->data = s->prefetch->end = s->prefetch->buffer;
	s->decoder.read_ip = s->decoder.prefetch_fence = s->decoder.fence = s->prefetch->data;
	s->ip.prefetch = s->ip.base;
}

/* assuming s->ip.value and s->seg[MX86_SEG_CS].value are properly set,
 * update instruction pointer */
void emu8086_state_update_ip(struct emu8086_state *s) {
	s->ip.base = s->sreg[MX86_SEG_CS].base + s->ip.value;
	emu8086_state_flush_prefetch(s);
}

void emu8086_state_set_ip(struct emu8086_state *s,uint16_t val) {
	s->ip.value = val;
	emu8086_state_update_ip(s);
}

void emu8086_state_reset_complete(struct emu8086_state *s) {
	s->reset_clock = 0;	s->flags = 0;
	s->reg.n.ax = 1;	s->reg.n.bx = 0;
	s->reg.n.cx = 0;	s->reg.n.dx = 0;
	s->reg.n.si = 0;	s->reg.n.di = 0;
	s->reg.n.bp = 0;	s->reg.n.sp = 0;
	emu8086_segment_set(&(s->sreg[MX86_SEG_CS]),0xFFFF);
	emu8086_segment_set(&(s->sreg[MX86_SEG_DS]),0xFFFF);
	emu8086_segment_set(&(s->sreg[MX86_SEG_ES]),0xFFFF);
	emu8086_segment_set(&(s->sreg[MX86_SEG_SS]),0xFFFF);
	emu8086_state_set_ip(s,0x0000);
}

int emu8086_state_init(struct emu8086_state *s) {
	memset(s,0,sizeof(*s));

	/* prefetch buffer is 5 bytes, sliding along 256 bytes
	   helps avoid overhead of copying bytes back */
	if ((s->prefetch = sliding_window_create(256)) == NULL)
		return 1;

	s->prefetch_memio.cpu = s;
	emu8086_state_reset(s);
	return 0;
}

void emu8086_state_free(struct emu8086_state *s) {
	if (s->prefetch) sliding_window_destroy(s->prefetch);
	s->prefetch = NULL;
}

void print_state(struct emu8086_state *c) {
	fprintf(stderr,"[st] %04X:%04X A=%04X B=%04X C=%04X D=%04X S=%04X D=%04X BP=%04X\n",
		c->sreg[MX86_SEG_CS].value,
		c->ip.value,
		c->reg.n.ax,
		c->reg.n.bx,
		c->reg.n.cx,
		c->reg.n.dx,
		c->reg.n.si,
		c->reg.n.di,
		c->reg.n.bp);
	fprintf(stderr,"     FL%04X %s %s %s %s %s %s %s %s %s DS=%04X ES=%04X SS:SP=%04X:%04X\n",
		c->flags,
		(c->flags & (1<<11)) ? "OF" : "  ",
		(c->flags & (1<<10)) ? "DF" : "  ",
		(c->flags &  (1<<9)) ? "IF" : "  ",
		(c->flags &  (1<<8)) ? "TF" : "  ",
		(c->flags &  (1<<7)) ? "SF" : "  ",
		(c->flags &  (1<<6)) ? "ZF" : "  ",
		(c->flags &  (1<<4)) ? "AF" : "  ",
		(c->flags &  (1<<2)) ? "PF" : "  ",
		(c->flags &  (1<<0)) ? "CF" : "  ",
		c->sreg[MX86_SEG_DS].value,
		c->sreg[MX86_SEG_ES].value,
		c->sreg[MX86_SEG_SS].value,
		c->reg.n.sp);
}

void print_ins(struct minx86dec_instruction *i) {
	minx86_read_ptr_t iptr;
	char arg_c[101];
	int c;

	fprintf(stderr,"INS: ");
	for (c=0,iptr=i->start;iptr != i->end;c++)
		fprintf(stderr,"%02X ",*iptr++);
	for (;c < 8;c++)
		fprintf(stderr,"   ");
	fprintf(stderr,"%-8s ",opcode_string[i->opcode]);
	for (c=0;c < i->argc;) {
		minx86dec_regprint(&i->argv[c],arg_c);
		fprintf(stderr,"%s",arg_c);
		if (++c < i->argc) fprintf(stderr,",");
	}
	fprintf(stderr,"\n");
}

/* NOTE: mem I/O on this CPU is always 8-bit, no flags needed for word/dword reading */
void emu8086_state_poke_prefetch(struct emu8086_state *s,unsigned char c) {
	*(s->prefetch->end++) = c;
	s->ip.prefetch++;
#ifdef DEBUG
	assert(s->ip.prefetch == (s->ip.base+s->prefetch->end-s->prefetch->data));
#endif
	s->decoder.fence = s->prefetch->end;
}

void emu8086_state_prefetch_cycle(struct emu8086_state *s,struct emu8086_event *e) {
	unsigned char c;

	s->prefetch_memio.event = e;
	do {
		if ((s->prefetch->data+PREFETCH_SIZE+16) > s->prefetch->fence)
			sliding_window_flush(s->prefetch);

		/* issue memory read */
		e->event_duration += 2;
		s->prefetch_memio.addr = s->ip.prefetch;
		emu8086_state_poke_prefetch(s,c=s->memio_r(&(s->prefetch_memio)));
#ifdef DEBUG
		fprintf(stderr,"memread(0x%05X) = 0x%02X\n",s->prefetch_memio.addr,c);
#endif
	} while ((s->prefetch->data+PREFETCH_SIZE) > s->prefetch->end);

}

int emu8086_state_event(struct emu8086_state *s,struct emu8086_event *e) {
	size_t inslen;

	e->decoded_ins = 0;
	if (s->reset_clock != 0) {
		e->event = EMU86_EV_RESET;
		e->event_duration = s->reset_clock;
		return 1;
	}
	/* help keep the prefetch full */
	else if ((s->prefetch->data+PREFETCH_SIZE) > s->prefetch->end) {
		e->event = EMU86_EV_IDLE;
		e->event_duration = 0;
		emu8086_state_prefetch_cycle(s,e);
		return 1;
	}

	e->event = EMU86_EV_IDLE;
	e->event_duration = 2;

#ifdef DEBUG
	assert(s->prefetch->data == s->decoder.read_ip);
	assert(s->prefetch->end == s->decoder.fence);
#endif

	s->decoder.ip_value = (uint32_t)(s->ip.value);
	minx86dec_decode8086(&s->decoder,&s->last_ins);
	inslen = (size_t)(s->last_ins.end - s->last_ins.start);
	s->ip.base += inslen;
	s->ip.value += inslen;
	s->prefetch->data += inslen;
	e->decoded_ins = 1;

	return 1;
}

void emu8086_state_advance(struct emu8086_state *s,struct emu8086_event *e,unsigned int clocks) {
	if (clocks == 0) return;

	if (s->reset_clock != 0 && s->reset == 0) {
		if (clocks > e->event_duration) clocks = e->event_duration;
		s->reset_clock -= clocks;
		if (s->reset_clock == 0) emu8086_state_reset_complete(s);
	}

	s->clocks += clocks;
}

int emu8086_state_in_reset(struct emu8086_state *s) {
	return (s->reset_clock != 0);
}

void emu8086_state_event_print(struct emu8086_state *s,struct emu8086_event *e) {
#ifdef DEBUG
	fprintf(stderr,"[event at clk=%u len=%u]\n",s->clocks,e->event_duration);
	switch (e->event) {
		case EMU86_EV_RESET:
			fprintf(stderr,"  reset\n");
			break;
		case EMU86_EV_IDLE:
			fprintf(stderr,"  idle\n");
			break;
	};
#endif
}

void my_event_handler(struct emu8086_state *s,struct emu8086_event *e) {
}

uint8_t my_memio_r(struct emu8086_memio *m) {
	/* let's emulate a system bus that's SLOW :) */
	m->event->event_duration += 10;

	if (m->addr < sizeof(memory))
		return memory[m->addr];

	return 0xFF;
}

int main(int argc,char **argv) {
	char arg_c[101];
	int sz=0;
	int c;

	/* init random */
	srand(time(NULL)^getpid());

	/* init memory */
	memset(memory,0x00,sizeof(memory));

	/* decide where the program goes */
	unsigned int psp_seg = 0xA00 + (((unsigned int)(rand()*rand())) % 0x4000);
	unsigned char *PSP = memory + (psp_seg << 4);

	/* init PSP segment */
	memset(PSP,0x90,256);	/* TODO */

	/* program code */
	unsigned int code_seg = psp_seg + 0x10;
	unsigned char *code = memory + (code_seg << 4);
	unsigned char *code_fence;
	unsigned long code_size;

	/* load program */
	{
		int fd = open("cputest/test8086.com",O_RDONLY);
		if (fd < 0) {
			perror("Cannot load test 8086 program");
			return 1;
		}

		code_size = read(fd,code,(size_t)(memory+sizeof(memory)-code_seg));
		close(fd);
	}
	code_fence = code + code_size;

	fprintf(stderr,"Memory size:          %luKB\n",(unsigned long)(sizeof(memory)>>10));
	fprintf(stderr,"PSP segment:          0x%04X (0x%05X)\n",psp_seg,psp_seg<<4);
	fprintf(stderr,"Code segment:         0x%04X (0x%05X)\n",code_seg,code_seg<<4);
	fprintf(stderr,"Code range:           0x%05X-0x%05X\n",(unsigned int)(code - memory),
		(unsigned int)(code_fence - memory));

	/* setup processor */
	struct emu8086_event event;
	struct emu8086_state cpu;

	if (emu8086_state_init(&cpu)) {
		fprintf(stderr,"Failed to init CPU\n");
		return 1;
	}
	cpu.memio_r = my_memio_r;

	/* wait for processor to finish reset */
	{
		int patience = 100;
		do {
			if (--patience <= 0) {
				fprintf(stderr,"Ran out of patience during reset process\n");
				return 1;
			}
			if (!emu8086_state_event(&cpu,&event)) {
				fprintf(stderr,"Event engine had nothing to report\n");
				return 1;
			}
			emu8086_state_event_print(&cpu,&event);
			emu8086_state_advance(&cpu,&event,event.event_duration);
		} while (emu8086_state_in_reset(&cpu));
		fprintf(stderr,"Reset completed in %u clocks\n",cpu.clocks);
	}

	/* setup instruction ptr to the program */
	{
		struct emu8086_state *s = &cpu;
		s->reg.n.ax = 0;	s->reg.n.bx = 0;
		s->reg.n.cx = 0;	s->reg.n.dx = 0;
		s->reg.n.si = 0;	s->reg.n.di = 0;
		s->reg.n.bp = 0;	s->reg.n.sp = 0xFFF8;
		emu8086_segment_set(&(s->sreg[MX86_SEG_CS]),psp_seg);
		emu8086_segment_set(&(s->sreg[MX86_SEG_DS]),psp_seg);
		emu8086_segment_set(&(s->sreg[MX86_SEG_ES]),psp_seg);
		emu8086_segment_set(&(s->sreg[MX86_SEG_SS]),psp_seg);
		emu8086_state_set_ip(s,0x0100);
	}

	/* run the program */
	{
		int count = 0;
		while (count < 100) {
			if (!emu8086_state_event(&cpu,&event)) {
				fprintf(stderr,"Event engine had nothing to report\n");
				return 1;
			}

			emu8086_state_event_print(&cpu,&event);
#ifdef DEBUG
			if (event.decoded_ins) {
				print_ins(&cpu.last_ins);
				print_state(&cpu);
			}
#endif

			my_event_handler(&cpu,&event);
			emu8086_state_advance(&cpu,&event,event.event_duration);

			count++;
		}
	}

	/* done */
	emu8086_state_free(&cpu);

	return 0;
}

