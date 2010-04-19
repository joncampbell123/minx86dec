#include "minx86dec/types.h"
#include "minx86dec/state.h"
#include "minx86dec/opcodes.h"
#include "minx86dec/coreall.h"
#include "minx86dec/opcodes_str.h"
#include "minx86dec/coreall_x64.h"
#include <string.h>
#include <stdio.h>

uint8_t buffer[16384];

static void minx86dec_init_state_x64(struct minx86dec_state_x64 *st) {
	memset(st,0,sizeof(*st));
}

static void minx86dec_set_buffer_x64(struct minx86dec_state_x64 *st,uint8_t *buf,int sz) {
	st->fence = buf + sz;
	st->prefetch_fence = st->fence - 16;
	st->read_ip = buf;
}

int main(int argc,char **argv) {
	struct minx86dec_state_x64 st;
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

	minx86dec_init_state_x64(&st);
	minx86dec_set_buffer_x64(&st,buffer,sz);

	while (st.read_ip < st.fence) {
		struct minx86dec_instruction_x64 i;
		st.ip_value = (uint32_t)(st.read_ip - buffer);
		minx86dec_decodeall_x64(&st,&i);
		printf("0x%04X  ",(unsigned int)(i.start - buffer));
		for (c=0,iptr=i.start;iptr != i.end;c++)
			printf("%02X ",*iptr++);
		for (;c < 8;c++)
			printf("   ");
		printf("%-8s ",opcode_string[i.opcode]);
		for (c=0;c < i.argc;) {
			minx86dec_regprint_x64(&i.argv[c],arg_c);
			printf("%s",arg_c);
			if (++c < i.argc) printf(",");
		}
		if (i.lock) printf("  ; LOCK#");
		printf("\n");
	}

	return 0;
}

