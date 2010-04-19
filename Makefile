TOP=`pwd`

CFLAGS=-I. -I.. -g3 -O0 -std=c99

all: test-headers decode8086 decodenecv20 decode286 decode386 decode486 decode586 decode586pro decode686 decodeall decodeall_x64 test1.bin test2.bin test3.bin testnecv20.bin test64_1.bin

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

decodeall: decodeall.o coreall.o string.o
	gcc $(CFLAGS) -o $@ $^

decodeall_x64: decodeall_x64.o coreall_x64.o string.o
	gcc $(CFLAGS) -o $@ $^

.c.o:
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *~ test-headers decode8086 decodenecv20 *.bin decode8086 decode286 decode386 decode486 decode586 decodeall decodeall_x64 decodepentium decode-pentium decode686 decode-pentium2 decode586pro decode-pentiumpro random.bin.results

test1.bin: test1.asm
	nasm -O5 -o $@ -f bin $<

test2.bin: test2.asm
	nasm -O5 -o $@ -f bin $<

test3.bin: test3.asm
	nasm -O5 -o $@ -f bin $<

testnecv20.bin: testnecv20.asm
	nasm -O5 -o $@ -f bin $<

test64_1.bin: test64_1.asm
	nasm -O5 -o $@ -f bin $<

