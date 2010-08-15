#include "minx86dec/types.h"
#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"
#include "minx86dec/coreall.h"
#include "minx86dec/opcodes_str.h"
#include "minx86dec/encoreall.h"
#include <string.h>
#include <stdio.h>

uint8_t buffer[16384];
uint8_t encoded[16384];

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

int main(int argc,char **argv) {
	struct minx86dec_state rst;
	struct minx86enc_state est;
	struct minx86dec_state st;
	minx86_read_ptr_t iptr;
	unsigned char mark = 1;
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
		st.ip_value = (uint32_t)(st.read_ip - buffer); rst = st;
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
		est.ip_value = st.ip_value;
		minx86enc_encodeall(&est,&i);
		{
			minx86_read_ptr_t x = (minx86_read_ptr_t)est.started_here;
			while (x < est.write_ip && (*x == 0x66 || *x == 0x67)) x++;
			if (x < est.write_ip) {
				if (*x == 0x90) /* a NOP is a NOP is a NOP */
					mark = 0;
			}
		}
		{
			unsigned char mismatch = 0;
			minx86_write_ptr_t p = est.started_here;
			minx86_read_ptr_t r = i.start;
			while (p != est.write_ip) {
				printf("%02X",*p);
				if (mark && *r != *p) printf(".");
				else printf(" ");
				r++; p++;
			}
		}
		printf("\n");

		rst.read_ip = est.started_here;
		rst.fence = encoded + sizeof(encoded);
		rst.prefetch_fence = encoded + sizeof(encoded) - 16;
		minx86dec_decodeall(&rst,&i);
		printf("    ->> ");
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
	}

	return 0;
}

