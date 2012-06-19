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
	if (argc > 2 && (!strcmp(argv[2],"/32") || !strcmp(argv[2],"-32"))) st.data32 = st.addr32 = est.data32 = est.addr32 = 1;
	else if (argc > 2 && (!strcmp(argv[2],"/16x32") || !strcmp(argv[2],"-16x32"))) { st.data32 = st.addr32 = 0; est.data32 = est.addr32 = 1; }
	else if (argc > 2 && (!strcmp(argv[2],"/32x16") || !strcmp(argv[2],"-32x16"))) { st.data32 = st.addr32 = 1; est.data32 = est.addr32 = 0; }

	while (st.read_ip < st.fence) {
		struct minx86dec_instruction i;
		minx86dec_init_instruction(&i);
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

		est.ip_value = st.ip_value;
		minx86enc_encodeall(&est,&i);
		if (est.started_here < est.write_ip) {
			rst.read_ip = est.started_here;
			rst.fence = encoded + sizeof(encoded);
			rst.prefetch_fence = encoded + sizeof(encoded) - 16;
			rst.data32 = est.data32;
			rst.addr32 = est.addr32;
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
		else {
			printf("    ->> \n");
		}
	}

	return 0;
}

