with_clang:
	clang -Wall -lz -o ericstract ericstract.c

with_gcc:
	gcc -Wall -lz -o ericstract ericstract.c

debug:
	clang -g -O0 -Weverything -lz -o ericstract ericstract.c
