all: test8086.com

test8086.com: test8086.asm 8086/*.inc
	nasm -o test8086.com -f bin test8086.asm

clean:
	rm -f stdout.txt results.txt *~ *.com *.LOG *.log stderr.txt

