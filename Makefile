all:
	gcc -W -Wall -Werror sort.c main.c -o sort -lm

test:
	./sort
