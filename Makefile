CC = gcc
CFLAG = -Wall -Wextra -std=c17 -pedantic -g -Og -ggdb -fsanitize=address,leak,undefined
OBJ = *.o

allocator: allocator.c
	$(CC) $(CFLAG) $< -c

test: test.c
	$(CC) $(CFLAG) $(OBJ) $< -o $@

all:
	make allocator test

clean:
	rm *.o test
