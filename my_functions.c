#include "my_functions.h"

int		is_numeric(char c)
{
	return (c > 47 && c < 58);
}

int		length_of(const char *s)
{
	const char *p = s;

	if (!s)
		return (-1);
	while (*p) p++;
	return (p - s);
}

int		print(char *s)
{
	return (write(STDOUT, s, length_of(s)));
}

int		nprint(char *s, int length)
{
	return (write(STDOUT, s, length));
}

int		get_number(char **_s)
{
	char	*s;
	int		n;

	s = *_s;
	n = 0;
	while (is_numeric(*s))
	{
		n *= 10;
		n += *(s++) - 48;
	}
	*_s = s;
	return (n);
}

int		my_putc(char c)
{
	return (write(STDOUT, &c, 1));
}

void	putnbr(int n)
{
	if (n < 10)
	{
		my_putc(n + 48);
	}
	else
	{
		putnbr(n / 10);
		my_putc((n % 10) + 48);
	}
}

void	fmt_print(char *s, ...)
{
	va_list	ap;
	char	*start;
	int		param1;

	if (!s || !*s)
		return ;

	va_start(ap, s);
	start = s;
	while (*s)
	{
		if (*s == '%')
		{
			if (s != start)
				nprint(start, s - start);

			s++;
			if (*s == '%') 
				nprint("%%", 2);
			else
			{
				if (is_numeric(*s))
					param1 = get_number(&s);
				else
					param1 = -1;

				if (*s == 's')
				{
					if (param1 == -1)
						print(va_arg(ap, char*));
					else
						nprint(va_arg(ap, char*), param1);
				}
				else if (*s == 'd')
				{
					putnbr(va_arg(ap, int));
				}
			}
			start = s + 1;
		}
		s++;
	}

	va_end(ap);
	if (s != start)
		nprint(start, s - start);
}

int		my_strcmp(char *s1, char *s2)
{
	if (!s2 || !s2) return (ERROR); 
	while ((*s1 || *s2))
	{
		if (!*s1)
			return (0 - *s2);
		if (!*s2)
			return (*s1);
		if (*s1 != *s2)
			return (*s1 - *s2);
		s1++;
		s2++;
	}
	return (0);
}

int		my_strncmp(char *s1, char *s2, int size)
{
	if (!s2 || !s2 || size < 1) return (ERROR); 
	while ((*s1 || *s2) && size--)
	{
		if (!*s1 && *s2)
			return (0 - *s2);
		if (!*s2 && *s1)
			return (*s1);
		if (*s1 != *s2)
			return (*s1 - *s2);
		s1++;
		s2++;
	}
	return (0);
}

int set_nbr(char *s, int current, int n)
{
	if (n < 10)
	{
		s[current] = n + 48;
		return (current);
	}
	s[current] = (n % 10) + 48;
	return (set_nbr(s, current - 1, n / 10));
}

char *my_itoa(int n)
{
	static char number[12];

	number[11] = 0;
	return (number + set_nbr(number, 11, n));
}
