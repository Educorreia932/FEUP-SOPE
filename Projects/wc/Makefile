all: U2 Q2

U2: U2.c flagsU.c
	gcc -o U2 U2.c flagsU.c log.c utils.c -pthread -I  -Wall

Q2: Q2.c flagsQ.c
	gcc -o Q2 Q2.c flagsQ.c log.c -pthread utils.c -lpthread -lrt -I -Wall

clean:
	rm U2 Q2
