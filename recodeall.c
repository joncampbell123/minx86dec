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
	uint8_t			data32:1;	/* 386+ 32-bit encoding */
	uint8_t			addr32:1;	/* 386+ 32-bit encoding */
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

/* code will call this to generate the appropriate 386 data override prefix.
 * if 16-bit mode and the arg provided is 32 bits wide, OR
 * if 32-bit mode and the arg provided is 16 bits wide */
static inline minx86_write_ptr_t minx86enc_32_overrides(struct minx86dec_argv *a,struct minx86enc_state *est,minx86_write_ptr_t o) {
	if ((a->size>>2)^(est->data32)) *o++ = 0x66;
	if ((a->memregsz>>2)^(est->addr32)) *o++ = 0x67;
	return o;
}

int minx86enc_match_memreg(struct minx86dec_argv *a) {
	int ret = -1,mod = 0;

	if (a->memref_base != 0) {
		if (	(a->memregsz == 4 && (int32_t)a->memref_base >= -0x80 && (int32_t)a->memref_base < 0x80) ||
			(a->memregsz == 2 && (int16_t)a->memref_base >= -0x80 && (int16_t)a->memref_base < 0x80))
			mod = 1;
		else
			mod = 2;
	}

	if (a->memregs == 1) {
		if (a->memregsz == 4) {/* 32-bit */
		}
		else {/* 16-bit */
			switch (a->memreg[0]) {
				case MX86_REG_SI:	return 4 | (mod<<6);
				case MX86_REG_DI:	return 5 | (mod<<6);
				case MX86_REG_BP: {
					if (mod == 0) mod = 1;	/* No such [BP] must encode as [BP+0] */
					return 6 | (mod<<6); }
				case MX86_REG_BX:	return 7 | (mod<<6);
			}
		}
	}
	else if (a->memregs == 2) {
		if (a->memregsz == 4) {/* 32-bit */
		}
		else {/* 16-bit */
			/* BX or BP */
			switch (a->memreg[0]) {
				case MX86_REG_BX:	ret = 0; break;
				case MX86_REG_BP:	ret = 2; break;
				default:		return -1;
			}
			/* second one may be SI or DI */
			switch (a->memreg[1]) {
				case MX86_REG_SI:	break;
				case MX86_REG_DI:	ret |= 1; break;
				default:		return -1;
			}

			ret |= (mod<<6);
		}
	}

	return ret;
}

void minx86enc_encodeall(struct minx86enc_state *est,struct minx86dec_instruction *ins) {
	minx86_write_ptr_t o = est->write_ip;
	est->started_here = o;

	switch (ins->opcode) {
		case MXOP_JMP: {
			struct minx86dec_argv *a = &ins->argv[0];
			o = minx86enc_32_overrides(a,est,o);
			if (a->regtype == MX86_RT_REG) {
				*o++ = 0xFF;
				*o++ = (3<<6) | (4<<3) | a->reg;	/* mod=3 reg=4 rm=reg */
			}
			else if (a->regtype == MX86_RT_NONE) {
				int match = minx86enc_match_memreg(a);
				if (match >= 0) {
					*o++ = 0xFF;
					*o++ = (4<<3) | match;		/* the match value includes "MOD" bits */
					switch (match>>6) { /* encode according to mod bits */
						case 1:	*o++ = (uint8_t)(a->memref_base); break;
						case 2:	if (a->memregsz == 4)	{ *((uint32_t*)o) = (uint32_t)(a->memref_base);       o += 4; }
							else			{ *((uint16_t*)o) = (uint16_t)(a->memref_base);       o += 2; };
							break;
					}
				}
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

		printf("     -> ");
		minx86enc_encodeall(&est,&i);
		{
			unsigned char mismatch = 0;
			minx86_write_ptr_t p = est.started_here;
			while (p != est.write_ip) printf("%02X ",*p++);
		}
		printf("\n");
	}

	return 0;
}

