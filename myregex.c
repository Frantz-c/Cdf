#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <regex.h>
#include "myregex.h"

static void Add_Myregex_t(myregex_t **zmatch, unsigned int start, unsigned int end);

int preg_match(char *regex, char *string)
{
	int err;
	regex_t preg;

	err = regcomp(&preg, regex, REG_NOSUB | REG_EXTENDED);

	if (err == 0)
	{
		int match;

		match = regexec(&preg, string, 0, NULL, 0);
		regfree(&preg);

		if (match == 0) return 0;
		else if (match == REG_NOMATCH) return -3;
		else return -1;
	}
	else return -2;
}

int preg_match_get(char *regex, char *string, myregex_t **pmatch)
{
	int		err;
	regex_t	preg;

	*pmatch = NULL;
	err = regcomp(&preg, regex, REG_EXTENDED);

	if (err == 0)
	{
		int			match;
		regmatch_t	pregmatch;

//		pregmatch = malloc(sizeof(regmatch_t *) * 1U);
		match = regexec(&preg, string, 1U, &pregmatch, 0);
		regfree(&preg);

		if (match == 0)
		{
			Add_Myregex_t(pmatch, pregmatch.rm_so, pregmatch.rm_eo);
//			free(pregmatch);
			return (1);
		}
		else if (match == REG_NOMATCH)
			return (0);
	}
	else {
		fprintf(stderr, "regex \"%s\": erreur de compression\n", regex);
		abort();
	}
	return (-1);
}

int preg_match_get_all(char *regex, char *string, myregex_t **pmatch)
{
	int err;
	regex_t preg;
	int match;
	unsigned short i;
	size_t nmatch = 1;
	int offset = 0;

	if ((err = regcomp(&preg, regex, REG_EXTENDED)) == 0)
	{
		while (1)
		{
			regmatch_t pregmatch[1] = {{0, 0}};

			match = regexec(&preg, string + offset, nmatch, pregmatch, 0);

			if (match == 0)
			{
				for (i = 0; i < nmatch; i++)
				{
					if (pregmatch[i].rm_so == pregmatch[i].rm_eo) break;
					Add_Myregex_t(pmatch, pregmatch[i].rm_so + offset, pregmatch[i].rm_eo + offset);
					offset += pregmatch[i].rm_so + 1;
				}
			}
			else if (match == REG_NOMATCH) {
				regfree(&preg);
				if (*pmatch) return 0;
				return -3;
			}
			else {
				regfree(&preg);
				return -1;
			}
		}
	}
	return -2;
}

void free_myregex_t(myregex_t *regex)
{
	myregex_t *tmp;

	while (regex)
	{
		tmp = regex;
		regex = regex->next;
		free(tmp);
	}
}

void Add_Myregex_t(myregex_t **zmatch, unsigned int start, unsigned int end)
{
	myregex_t *zm = malloc(sizeof(myregex_t));
	zm->start = start;
	zm->end = end;
	zm->next = NULL;

	if (*zmatch == NULL) *zmatch = zm;
	else {
		myregex_t *tmp;
		tmp = *zmatch;
		while (tmp->next) tmp = tmp->next;
		tmp->next = zm;
	}
}
