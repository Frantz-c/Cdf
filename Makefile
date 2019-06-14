CFLAGS = -O3 -std=c99 -Wall -Wextra -g
CC     = gcc

.PHONY: cliarg_lib

all: cliarg_lib cdf.o myregex.o my_functions.o cliarg.a
	$(CC) $^ -o cdf $(CFLAGS)

cliarg_lib:
	make -C cliarg

cdf.o: cdf.c
	$(CC) -c $< -o $@ $(CFLAGS)

my_functions.o: my_functions.c
	$(CC) -c $< -o $@ $(CFLAGS)

myregex.o: myregex.c
	$(CC) -c $< -o $@ $(CFLAGS)


clean:
	rm -f *.o

fclean: clean
	rm -f cdf

re: fclean all
