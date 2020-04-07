simpledu: simpledu.c flags.c queue.c utils.c log.c
	@gcc -o simpledu simpledu.c flags.c queue.c utils.c log.c -I.
clean:
	@rm simpledu