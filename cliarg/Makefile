CFLAGS = -O6 -std=c99
CC     = gcc


../cliarg.a: main.o error.o print.o functions.o
	ar rcs $@ $^


main.o: main.c
	$(CC) -c $< -o $@ $(CFLAGS)

error.o: error.c
	$(CC) -c $< -o $@ $(CFLAGS)

print.o: print.c 
	$(CC) -c $< -o $@ $(CFLAGS)

functions.o: functions.c 
	$(CC) -c $< -o $@ $(CFLAGS)


clean:
	rm *.o
