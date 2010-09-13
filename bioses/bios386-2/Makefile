all: rombios.bin

CFLAGS=-march=i386 -m32 -g0 -O0 -fomit-frame-pointer -nostdinc -nostdlib
ASFLAGS=-march=i386 -m32

rombios.o: entry.o entry_c.o
	ld -o $@ --entry _cpu_entry -Ttext 0 --section-start .cpuentry=0xFFF00 -Map bios.map entry_c.o entry.o

rombios.bin: main.bin cpuentry.bin
	cat main.bin cpuentry.bin >$@

main.bin: rombios.o
	objcopy -j .text -j .data -j .rodata -O binary $< $@.tmp
	cat $@.tmp /dev/zero | dd count=256 bs=256 >$@.tmp2; rm $@.tmp
	dd if=$@.tmp2 of=$@ bs=256 count=255; rm $@.tmp2

cpuentry.bin: rombios.o
	objcopy -j .cpuentry --rename-section .cpuentry=.text,alloc,load,readonly,code,contents -O binary $< $@

entry_c.o: entry_c.s
	gcc -c -o $@ $<

entry_c.s: entry_c.c
	gcc $(CFLAGS) -S -o $@ $<

entry.o: entry.S
	gcc $(ASFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.bin *.bin.raw *~ *.map *.s

