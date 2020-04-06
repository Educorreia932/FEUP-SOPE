simpledu: simpledu.c flags.c queue.c utils.c
	@gcc -o simpledu simpledu.c flags.c queue.c utils.c -I.
clean:
	@rm simpledu