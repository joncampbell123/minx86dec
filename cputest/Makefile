all: test8086.com

test8086.com: test8086.asm
	nasm -o $@ -f bin $^

clean:
	rm -f stdout.txt results.txt *~ *.com *.LOG *.log

