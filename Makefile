TOP=`pwd`

HOSTA=$(shell uname -m)

CFLAGS=-I. -I.. -std=c99
ifeq ($(DEBUG),1)
CFLAGS += -g3 -O0
else
CFLAGS += -g0 -Os -fomit-frame-pointer -fexpensive-optimizations -Wall -Wno-long-long -std=gnu99
endif

ifeq ($(HOSTA),x86_64)
CFLAGS += -march=x86-64
else
CFLAGS += -march=i686
endif

DESTDIR =
PREFIX = /usr/local

all: test-headers libminx86dec.a decode8086 sdecode8086 decodenecv20 decode286 decode386 decode386iit387 decode386am decode386amiit387 decode386ibmslc decode386ibmslciit387 decode486 decode486a decode486am decode486amiit387 decode486ibmslc decode486ibmslciit387 decode486ibmslc2 decode486ibmslc2iit387 decode586 decode586pro decode586mmx decode686 decode5x86_cyrix decode6x86_cyrix decode486cyrix decode486cyrixiit387 decodeall decodeall_x64 test1.bin test2.bin test3.bin test3_32.bin testnecv20.bin test64_1.bin test6x86_cyrix.bin test386.bin test_evolution.bin test_all_amd64.bin recode8086 recodenecv20 recode286 recode386 recode386am recode486 recode486a recode486am recode586 recodeall recode1.bin DUMMY

install: all core286.h core386am.h core386.h core386ibmslc.h core486a.h core486am.h core486cyrix.h core486.h core486ibmslc2.h core486ibmslc.h core586.h core586mmx.h core586pro.h core5x86_cyrix.h core686.h core6x86_cyrix.h core8086.h coreall.h coreall_x64.h corenecv20.h encoreall.h iit387.h opcodes.h opcodes_str.h score8086.h state.h types.h x64_core_macros.h x86_core.h x86_core_macros.h DUMMY
	mkdir -p -m 755 $(DESTDIR)$(PREFIX)/lib $(DESTDIR)$(PREFIX)/include/minx86dec
	install -c -m 644 libminx86dec.a $(DESTDIR)$(PREFIX)/lib/
	install -c -m 644 core286.h core386am.h core386.h core386ibmslc.h core486a.h core486am.h core486cyrix.h core486.h core486ibmslc2.h core486ibmslc.h core586.h core586mmx.h core586pro.h core5x86_cyrix.h core686.h core6x86_cyrix.h core8086.h coreall.h coreall_x64.h corenecv20.h encoreall.h iit387.h opcodes.h opcodes_str.h score8086.h state.h types.h x64_core_macros.h x86_core.h x86_core_macros.h $(DESTDIR)$(PREFIX)/include/minx86dec/

DUMMY:
	@true

test-headers: test-headers.o
	gcc -o $@ $<
	./$@

libminx86dec.a: string.o core8086.o score8086.o corenecv20.o core286.o core386.o iit387.o core386am.o core386ibmslc.o core486.o core486a.o core486am.o core486ibmslc.o core486ibmslc2.o core586.o core586pro.o core586mmx.o core686.o core5x86_cyrix.o core6x86_cyrix.o core486cyrix.o coreall.o encoreall.o coreall_x64.o
	rm -f $@
	ar cqs $@ $^

core8086.o: x86_core.h x86_core_macros.h

decode8086: decode8086.o core8086.o string.o
	gcc $(CFLAGS) -o $@ $^

score8086.o: x86_core.h x86_core_macros.h

sdecode8086: sdecode8086.o score8086.o string.o
	gcc $(CFLAGS) -o $@ $^

corenecv20.o: x86_core.h x86_core_macros.h

decodenecv20: decodenecv20.o corenecv20.o string.o
	gcc $(CFLAGS) -o $@ $^

core286.o: x86_core.h x86_core_macros.h

decode286: decode286.o core286.o string.o
	gcc $(CFLAGS) -o $@ $^

core386.o: x86_core.h x86_core_macros.h

decode386: decode386.o core386.o string.o
	gcc $(CFLAGS) -o $@ $^

decode386iit387: decode386iit387.o core386.o string.o iit387.o
	gcc $(CFLAGS) -o $@ $^

decode386iit387.o: decode386.c
	gcc $(CFLAGS) -DIIT_FPU -c -o $@ $<

core386am.o: x86_core.h x86_core_macros.h

decode386am: decode386am.o core386am.o string.o
	gcc $(CFLAGS) -o $@ $^

decode386amiit387: decode386amiit387.o core386am.o string.o iit387.o
	gcc $(CFLAGS) -o $@ $^

decode386amiit387.o: decode386am.c
	gcc $(CFLAGS) -DIIT_FPU -c -o $@ $<

core386ibmslc.o: x86_core.h x86_core_macros.h

decode386ibmslc: decode386ibmslc.o core386ibmslc.o string.o
	gcc $(CFLAGS) -o $@ $^

decode386ibmslciit387: decode386ibmslciit387.o core386ibmslc.o string.o iit387.o
	gcc $(CFLAGS) -o $@ $^

decode386ibmslciit387.o: decode386ibmslc.c
	gcc $(CFLAGS) -DIIT_FPU -c -o $@ $<

core486.o: x86_core.h x86_core_macros.h

decode486: decode486.o core486.o string.o
	gcc $(CFLAGS) -o $@ $^

core486a.o: x86_core.h x86_core_macros.h

decode486a: decode486a.o core486a.o string.o
	gcc $(CFLAGS) -o $@ $^

core486am.o: x86_core.h x86_core_macros.h

decode486am: decode486am.o core486am.o string.o
	gcc $(CFLAGS) -o $@ $^

decode486amiit387: decode486amiit387.o core486am.o string.o iit387.o
	gcc $(CFLAGS) -o $@ $^

decode486amiit387.o: decode486am.c
	gcc $(CFLAGS) -DIIT_FPU -c -o $@ $<

core486ibmslc.o: x86_core.h x86_core_macros.h

decode486ibmslc: decode486ibmslc.o core486ibmslc.o string.o
	gcc $(CFLAGS) -o $@ $^

decode486ibmslciit387: decode486ibmslciit387.o core486ibmslc.o string.o iit387.o
	gcc $(CFLAGS) -o $@ $^

decode486ibmslciit387.o: decode486ibmslc.c
	gcc $(CFLAGS) -DIIT_FPU -c -o $@ $<

core486ibmslc2.o: x86_core.h x86_core_macros.h

decode486ibmslc2: decode486ibmslc2.o core486ibmslc2.o string.o
	gcc $(CFLAGS) -o $@ $^

decode486ibmslc2iit387: decode486ibmslc2iit387.o core486ibmslc2.o string.o iit387.o
	gcc $(CFLAGS) -o $@ $^

decode486ibmslc2iit387.o: decode486ibmslc2.c
	gcc $(CFLAGS) -DIIT_FPU -c -o $@ $<

core586.o: x86_core.h x86_core_macros.h

decode586: decode586.o core586.o string.o
	gcc $(CFLAGS) -o $@ $^
	rm -f decode-pentium
	ln -s decode586 decode-pentium

core586pro.o: x86_core.h x86_core_macros.h

decode586pro: decode586pro.o core586pro.o string.o
	gcc $(CFLAGS) -o $@ $^
	rm -f decode-pentiumpro
	ln -s decode586pro decode-pentiumpro

core586mmx.o: x86_core.h x86_core_macros.h

decode586mmx: decode586mmx.o core586mmx.o string.o
	gcc $(CFLAGS) -o $@ $^
	rm -f decode-pentiummmx
	ln -s decode586mmx decode-pentiummmx

core686.o: x86_core.h x86_core_macros.h

decode686: decode686.o core686.o string.o
	gcc $(CFLAGS) -o $@ $^
	rm -f decode-pentium2
	ln -s decode686 decode-pentium2

core5x86_cyrix.o: x86_core.h x86_core_macros.h

decode5x86_cyrix: decode5x86_cyrix.o core5x86_cyrix.o string.o
	gcc $(CFLAGS) -o $@ $^

core6x86_cyrix.o: x86_core.h x86_core_macros.h

decode6x86_cyrix: decode6x86_cyrix.o core6x86_cyrix.o string.o
	gcc $(CFLAGS) -o $@ $^

core486cyrix.o: x86_core.h x86_core_macros.h

decode486cyrix: decode486cyrix.o core486cyrix.o string.o
	gcc $(CFLAGS) -o $@ $^

decode486cyrixiit387: decode486cyrixiit387.o core486cyrix.o string.o iit387.o
	gcc $(CFLAGS) -o $@ $^

decode486cyrixiit387.o: decode486cyrix.c
	gcc $(CFLAGS) -DIIT_FPU -c -o $@ $<

coreall.o: x86_core.h x86_core_macros.h

decodeall: decodeall.o coreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recode8086: recode8086.o core8086.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recodenecv20: recodenecv20.o corenecv20.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recode286: recode286.o core286.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recode386: recode386.o core386.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recode386am: recode386am.o core386am.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recode486: recode486.o core486.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recode486a: recode486a.o core486a.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recode486am: recode486am.o core486am.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recode586: recode586.o core586.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recodeall: recodeall.o coreall.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

coreall_x64.o: x86_core.h x86_core_macros.h

decodeall_x64: decodeall_x64.o coreall_x64.o string.o
	gcc $(CFLAGS) -o $@ $^

.c.o:
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.[oa] *~ test-headers decode8086 sdecode8086 decodenecv20 *.bin decode286 decode386 decode386iit387 decode386am decode386amiit387 decode386ibmslc decode386ibmslciit387 decode486 decode486a decode486am decode486amiit387 decode486ibmslc decode486ibmslc2 decode586 decodeall decodeall_x64 decodepentium decode-pentium decode686 decode5x86_cyrix decode6x86_cyrix decode486ibmslciit387 decode486ibmslc2iit387 decode486cyrix decode486cyrixiit387 decode-pentium2 decode586pro decode586mmx decode-pentiumpro decode-pentiummmx random.bin.results recodeall recode8086 recode286 recode386 recode486 recode486a recode486am recode586 recodenecv20 recode386am
	find -name \*~ -delete

test1.bin: test1.asm
	nasm -O5 -o $@ -f bin $<

test2.bin: test2.asm
	nasm -O5 -o $@ -f bin $<

test3.bin: test3.asm
	nasm -O5 -o $@ -f bin $<

test3_32.bin: test3.asm
	nasm -O5 -DB32 -o $@ -f bin $<

test386.bin: test386.asm
	nasm -O5 -o $@ -f bin $<

testnecv20.bin: testnecv20.asm
	nasm -O5 -o $@ -f bin $<

test64_1.bin: test64_1.asm
	nasm -O5 -o $@ -f bin $<

test6x86_cyrix.bin: test6x86_cyrix.asm
	nasm -O5 -o $@ -f bin $<

test_evolution.bin: test_evolution.asm
	nasm -O5 -o $@ -f bin $<

test_all_amd64.bin: test_all_amd64.asm
	nasm -O5 -o $@ -f bin $<

recode1.bin: recode1.asm
	nasm -O5 -o $@ -f bin $<

