TOP=`pwd`

HOSTA=$(shell uname -m)

CFLAGS=-I. -I.. -std=c99
ifeq ($(DEBUG),1)
CFLAGS += -g3 -O0
else
CFLAGS += -g0 -Os -fomit-frame-pointer -fexpensive-optimizations
endif

ifeq ($(HOSTA),x86_64)
CFLAGS += -march=x86-64
else
CFLAGS += -march=i686
endif

all: test-headers decode8086 decodenecv20 decode286 decode386 decode486 decode486a decode586 decode586pro decode586mmx decode686 decode6x86_cyrix decodeall decodeall_x64 test1.bin test2.bin test3.bin test3_32.bin testnecv20.bin test64_1.bin test6x86_cyrix.bin test386.bin test_evolution.bin test_all_amd64.bin recode8086 recodenecv20 recode286 recode386 recode486 recode486a recode586 recodeall recode1.bin all-cputest testemu8086
	+$(MAKE) -C bioses

all-cputest:
	+$(MAKE) -C cputest

test-headers: test-headers.o
	gcc -o $@ $<
	./$@

core8086.o: x86_core.h x86_core_macros.h

decode8086: decode8086.o core8086.o string.o
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

core486.o: x86_core.h x86_core_macros.h

decode486: decode486.o core486.o string.o
	gcc $(CFLAGS) -o $@ $^

core486a.o: x86_core.h x86_core_macros.h

decode486a: decode486a.o core486a.o string.o
	gcc $(CFLAGS) -o $@ $^

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

core6x86_cyrix.o: x86_core.h x86_core_macros.h

decode6x86_cyrix: decode6x86_cyrix.o core6x86_cyrix.o string.o
	gcc $(CFLAGS) -o $@ $^

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

recode486: recode486.o core486.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recode486a: recode486a.o core486a.o encoreall.o string.o
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

testemu8086: testemu8086.o core8086.o string.o
	gcc $(CFLAGS) -o $@ $^ -lisp-utils-text

cputest-clean:
	+$(MAKE) -C cputest clean

clean: cputest-clean
	rm -f *.o *~ test-headers decode8086 decodenecv20 *.bin decode8086 decode286 decode386 decode486 decode486a decode586 decodeall decodeall_x64 decodepentium decode-pentium decode686 decode6x86_cyrix decode-pentium2 decode586pro decode586mmx decode-pentiumpro decode-pentiummmx random.bin.results recodeall testemu8086 recode8086 recode286 recode386 recode486 recode486a recode586 recodenecv20
	find -name \*~ -delete
	+$(MAKE) -C bioses clean

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

