TOP=`pwd`

CFLAGS=-I. -I.. -g3 -O0

all: test-headers decode8086 decode286 decode386 test1.bin test2.bin

test-headers: test-headers.o
	gcc -o $@ $<
	./$@

decode8086: decode8086.o core8086.o string.o
	gcc $(CFLAGS) -o $@ $^

decode286: decode286.o core286.o string.o
	gcc $(CFLAGS) -o $@ $^

decode386: decode386.o core386.o string.o
	gcc $(CFLAGS) -o $@ $^

.c.o:
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o test-headers decode8086 *.bin decode8086 decode286 decode386

test1.bin: test1.asm
	nasm -O5 -o $@ -f bin $<

test2.bin: test2.asm
	nasm -O5 -o $@ -f bin $<

