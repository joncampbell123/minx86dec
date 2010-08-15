#include "minx86dec/types.h"
#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"
#include "minx86dec/coreall.h"
#include "minx86dec/opcodes_str.h"
#include <string.h>
#include <stdio.h>

uint8_t buffer[16384];
uint8_t encoded[16384];

/* instruction encoding */
struct minx86enc_state {
	uint32_t		ip_value;	/* IP instruction pointer value */
	minx86_write_ptr_t	started_here;	/* after encoding: where the first byte was */
	minx86_write_ptr_t	write_ip;	/* decoding reads from here */
	minx86_write_ptr_t	fence;
	uint8_t			data32:1;	/* 386+ 32-bit opcode data operand decoding */
	uint8_t			addr32:1;	/* 386+ 32-bit opcode address operand decoding */
};

static void minx86enc_init_state(struct minx86enc_state *st) {
	memset(st,0,sizeof(*st));
}

static void minx86dec_init_state(struct minx86dec_state *st) {
	memset(st,0,sizeof(*st));
}

static void minx86dec_set_buffer(struct minx86dec_state *st,uint8_t *buf,int sz) {
	st->fence = buf + sz;
	st->prefetch_fence = st->fence - 16;
	st->read_ip = buf;
}

static void minx86enc_set_buffer(struct minx86enc_state *st,uint8_t *buf,int sz) {
	st->fence = buf + sz;
	st->write_ip = buf;
}

void minx86enc_encodeall(struct minx86enc_state *est,struct minx86dec_instruction *ins) {
	minx86_write_ptr_t o = est->write_ip;
	est->started_here = o;

	switch (ins->opcode) {
		case MXOP_JMP: {
			struct minx86dec_argv *a = &ins->argv[0];
			if (a->regtype == MX86_RT_REG) {
				if ((a->size>>1)^(ins->data32)) *o++ = 0x66; /* 32-bit op + 16-bit mode/16-bit op + 32-bit mode: need data override */
				*o++ = 0xFF;
				*o++ = (3<<6) | (4<<3) | a->reg;	/* mod=3 reg=4 rm=reg */
			}
		} break;
	}

	est->write_ip = o;
}

int main(int argc,char **argv) {
	struct minx86enc_state est;
	struct minx86dec_state st;
	minx86_read_ptr_t iptr;
	char arg_c[101];
	FILE *fp;
	int sz=0;
	int c;

	if ((fp = fopen(argv[1],"rb")) == NULL) {
		fprintf(stderr,"Cannot open %s\n",argv[1]);
		return 1;
	}
	sz = fread(buffer,1,sizeof(buffer),fp);
	fclose(fp);
	if (sz < 1) {
		fprintf(stderr,"File too small\n");
		return 1;
	}

	minx86dec_init_state(&st);
	minx86dec_set_buffer(&st,buffer,sz);
	minx86enc_init_state(&est);
	minx86enc_set_buffer(&est,encoded,sizeof(encoded));

	while (st.read_ip < st.fence) {
		struct minx86dec_instruction i;
		st.ip_value = (uint32_t)(st.read_ip - buffer);
		minx86dec_decodeall(&st,&i);
		printf("0x%04X  ",(unsigned int)(i.start - buffer));
		for (c=0,iptr=i.start;iptr != i.end;c++)
			printf("%02X ",*iptr++);
		for (;c < 8;c++)
			printf("   ");
		printf("%-8s ",opcode_string[i.opcode]);
		for (c=0;c < i.argc;) {
			minx86dec_regprint(&i.argv[c],arg_c);
			printf("%s",arg_c);
			if (++c < i.argc) printf(",");
		}
		if (i.lock) printf("  ; LOCK#");
		printf("\n");

		printf("      -> ");
		minx86enc_encodeall(&est,&i);
		{
			minx86_write_ptr_t p = est.started_here;
			while (p != est.write_ip) {
				printf("%02X ",*p++);
			}
		}
		printf("\n");
	}

	return 0;
}

