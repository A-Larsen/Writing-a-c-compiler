assemble: return_2.c
	gcc -S -O -fno-asynchronous-unwind-tables -fcf-protection=none return_2.c

compile: return_2.s
	gcc return_2.s -o return_2
