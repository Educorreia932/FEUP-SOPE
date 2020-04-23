all: U1 Q1

U1: U1.c flagsU.c
	@gcc -o U1 U1.c flagsU.c -I.

Q1: Q1.c flagsQ.c
	@gcc -o Q1 Q1.c flagsQ.c -I.

clean:
	@rm U1 Q1
