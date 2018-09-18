CFLAGS = -O6 -std=c99 -W -Wall -Wextra
CC     = gcc


all: cdf.o myregex.o my_functions.o cliarg.a
	$(CC) $^ -o cdf $(CFLAGS)



cdf.o: cdf.c
	$(CC) -c $< -o $@ $(CFLAGS)

my_functions.o: my_functions.c
	$(CC) -c $< -o $@ $(CFLAGS)

myregex.o: myregex.c
	$(CC) -c $< -o $@ $(CFLAGS)


clean:
	rm *.o

Xclean: clean
	rm cdf
