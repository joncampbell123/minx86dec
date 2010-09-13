all: rombios.bin

rombios.o: entry.o entry_c.o
	ld -o $@ --entry _cpu_entry -Ttext 0xE0000 --section-start .cpuentry=0xFFF80 -Map bios.map entry_c.o entry.o

rombios.bin: main.bin cpuentry.bin
	cat main.bin cpuentry.bin >$@

main.bin: rombios.o
	objcopy -j .text -O binary $< $@.tmp
	cat $@.tmp /dev/zero | dd count=1 bs=$$((0x1FF80)) >$@
	rm $@.tmp

cpuentry.bin: rombios.o
	objcopy -j .cpuentry --rename-section .cpuentry=.text,alloc,load,readonly,code,contents -O binary $< $@

entry_c.o: entry_c.c
	gcc -march=i386 -m32 -nostdinc -nostdlib -c -o $@ $<

entry.o: entry.S
	gcc -march=i386 -m32 -c -o $@ $<

clean:
	rm -f *.o *.bin *.bin.raw *~ *.map

