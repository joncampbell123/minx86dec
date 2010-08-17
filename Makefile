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

all: test-headers decode8086 decodenecv20 decode286 decode386 decode486 decode586 decode586pro decode686 decode6x86_cyrix decodeall decodeall_x64 test1.bin test2.bin test3.bin testnecv20.bin test64_1.bin test6x86_cyrix.bin test386.bin test_evolution.bin test_all_amd64.bin recodeall recodeall32 recode1.bin all-cputest

all-cputest:
	make -C cputest

test-headers: test-headers.o
	gcc -o $@ $<
	./$@

decode8086: decode8086.o core8086.o string.o
	gcc $(CFLAGS) -o $@ $^

decodenecv20: decodenecv20.o corenecv20.o string.o
	gcc $(CFLAGS) -o $@ $^

decode286: decode286.o core286.o string.o
	gcc $(CFLAGS) -o $@ $^

decode386: decode386.o core386.o string.o
	gcc $(CFLAGS) -o $@ $^

decode486: decode486.o core486.o string.o
	gcc $(CFLAGS) -o $@ $^

decode586: decode586.o core586.o string.o
	gcc $(CFLAGS) -o $@ $^
	rm -f decode-pentium
	ln -s decode586 decode-pentium

decode586pro: decode586pro.o core586pro.o string.o
	gcc $(CFLAGS) -o $@ $^
	rm -f decode-pentiumpro
	ln -s decode586pro decode-pentiumpro

decode686: decode686.o core686.o string.o
	gcc $(CFLAGS) -o $@ $^
	rm -f decode-pentium2
	ln -s decode686 decode-pentium2

decode6x86_cyrix: decode6x86_cyrix.o core6x86_cyrix.o string.o
	gcc $(CFLAGS) -o $@ $^

decodeall: decodeall.o coreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recodeall: recodeall.o coreall.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

recodeall32: recodeall32.o coreall.o encoreall.o string.o
	gcc $(CFLAGS) -o $@ $^

decodeall_x64: decodeall_x64.o coreall_x64.o string.o
	gcc $(CFLAGS) -o $@ $^

.c.o:
	gcc $(CFLAGS) -c -o $@ $<

cputest-clean:
	make -C cputest clean

clean: cputest-clean
	rm -f *.o *~ test-headers decode8086 decodenecv20 *.bin decode8086 decode286 decode386 decode486 decode586 decodeall decodeall_x64 decodepentium decode-pentium decode686 decode6x86_cyrix decode-pentium2 decode586pro decode-pentiumpro random.bin.results recodeall recodeall32
	find -name \*~ -delete

test1.bin: test1.asm
	nasm -O5 -o $@ -f bin $<

test2.bin: test2.asm
	nasm -O5 -o $@ -f bin $<

test3.bin: test3.asm
	nasm -O5 -o $@ -f bin $<

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

