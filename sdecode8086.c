#include "minx86dec/types.h"
#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"
#include "minx86dec/score8086.h"
#include "minx86dec/opcodes_str.h"
#include <string.h>
#include <stdio.h>

uint8_t buffer[16384],*bufr;
uint8_t tmp[4096],*tmpw;

static void minx86dec_init_state(struct minx86dec_state *st) {
	memset(st,0,sizeof(*st));
}

/* streaming decode fetch: our job is to extend the prefetch fence, or if that is not possible, to leave the pointer as-is */
static void my_fetch8(struct minx86dec_state *ctx) {
	while (ctx->prefetch_fence < (tmp+sizeof(tmp)) && bufr < (buffer+sizeof(buffer)))
		*(ctx->prefetch_fence++) = *bufr++;
}

int main(int argc,char **argv) {
	struct minx86dec_state st;
	minx86_read_ptr_t iptr;
	char arg_c[101];
	int sz=0,ip=0;
	FILE *fp;
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
	if (argc > 2 && (!strcmp(argv[2],"/32") || !strcmp(argv[2],"-32"))) {
		st.data32 = st.addr32 = 1;
		fprintf(stderr,"Warning: This core does not support 32-bit encoding.\n");
	}

	st.fence = tmp + sizeof(tmp);
	while (ip < sz) {
		struct minx86dec_instruction i;
		minx86dec_init_instruction(&i);

		/* simulate (somewhat) the 8086 prefetch by copying 4 bytes, then letting the fetch8 code read more if needed */
		/* if our fetch code doesn't do it's job it's pretty obvious */
		memset(tmp,0xFF,64);
		bufr = buffer+ip;
		st.prefetch_fence = tmp;
		*((uint32_t*)st.prefetch_fence) = *((uint32_t*)(bufr)); st.prefetch_fence += 4; bufr += 4;

		st.ip_value = (uint32_t)ip;
		st.fetch_overruns = 0;
		st.read_ip = tmp;
		minx86dec_sdecode8086(&st,&i,my_fetch8);
		if (st.fetch_overruns) printf("*WARNING: Fetch overrun (%u) and failure to fill\n",st.fetch_overruns);
		printf("0x%04X  ",(unsigned int)ip);
		for (c=0,iptr=i.start;iptr != i.end;c++,ip++)
			printf("%02X ",*iptr++);
		for (;c < 8;c++)
			printf("   ");
		printf("%-8s ",opcode_string[i.opcode]);
		for (c=0;c < i.argc;) {
			minx86dec_regprint(&i.argv[c],arg_c);
			printf("%s",arg_c);
			if (++c < i.argc) printf(",");
		}
		printf("\n");
	}

	return 0;
}

