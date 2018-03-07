CFLAGS = -O6 -std=c99
CC     = gcc


all: cdf.o myregex.o cliarg.a
	$(CC) $^ -o cdf $(CFLAGS)



cdf.o: cdf.c
	$(CC) -c $< -o $@ $(CFLAGS)

myregex.o: myregex.c
	$(CC) -c $< -o $@ $(CFLAGS)


clean:
	rm *.o

Xclean: clean
	rm cdf
