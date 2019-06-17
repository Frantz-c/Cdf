CFLAGS = -O3 -std=c99 -Wall -Wextra -g
CC     ?= gcc
LIB    = cliarg.a
FILES  = cdf.o myregex.o my_functions.o 

.PHONY: cliarg_lib

all: cliarg_lib $(FILES)
	$(CC) $(FILES) $(LIB) -o cdf $(CFLAGS)

cliarg_lib:
	make -C cliarg

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)


clean:
	rm -f *.o

fclean: clean
	rm -f cdf

re: fclean all
