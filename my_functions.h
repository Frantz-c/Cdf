#include <stdarg.h>
#include <unistd.h>
//#include <stdio.h>

#define STDOUT	1
#define ERROR	-1000

int		is_numeric(char c);
int		length_of(const char *s);
int		print(char *s);
int		nprint(char *s, int length);
int		get_number(char **_s);
int		my_putc(char c);
void	putnbr(int n);
void	fmt_print(char *s, ...);
int		my_strcmp(char *s1, char *s2);
int		my_strncmp(char *s1, char *s2, int size);
char	*my_itoa(int n);
