CFLAGS = -O3 -std=c99 -Wextra -Wall -g
CC     = gcc
LIB    = ../cliarg.a

all: $(LIB)

$(LIB): main.o error.o print.o functions.o
	ar rc $@ $^ 
	ranlib $@

main.o: main.c
	$(CC) -c $< -o $@ $(CFLAGS)

error.o: error.c
	$(CC) -c $< -o $@ $(CFLAGS)

print.o: print.c 
	$(CC) -c $< -o $@ $(CFLAGS)

functions.o: functions.c 
	$(CC) -c $< -o $@ $(CFLAGS)


clean:
	rm -f *.o

fclean: clean
	rm -f $(LIB)

re: fclean all

