all: U1 Q1

U1: U1.c flagsU.c
	@gcc -o U1 U1.c flagsU.c log.c -pthread -I  -Wall

Q1: Q1.c flagsQ.c
	@gcc -o Q1 Q1.c flagsQ.c -pthread -I. -Wall

clean:
	@rm U1 Q1
