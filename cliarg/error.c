#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"


void Write_error(char c, const char *e, char **error[])
{
  char i;
  
  for (i = 0; (*error)[i]; i++);

  if (e) {
    (*error)[i] = malloc(sizeof(char) * (2 + strlen(e)));
    sprintf((*error)[i], "%c%s", c, e);
  }
  else {
    (*error)[i] = malloc(sizeof(char));
    *((*error)[i]) = c;
  }
}

void Cliarg_print_error(FILE *file, char *error[])
{
  while (*error)
  {
    switch ((*error)[0])
    {
      case '0': fprintf(file, "\e[1;31mSyntaxe de l'argument \e[0;33m'%s'\e[1;31m incorrecte\e[0m\n", (*error)+1);
        break;
      case '1': fprintf(file, "\e[1;31mPas de valeur attendue pour l'argument \e[0;33m'%s'\e[0m\n", (*error)+1);
        break;
      case '2': fprintf(file, "\e[1;31mValeur attendue pour l'argument \e[0;33m'%s'\e[0m\n", (*error)+1);
        break;
      case '3': fprintf(file, "\e[1;31mArgument \e[0;33m'%s' \e[1;31minconnu\e[0m\n", (*error)+1);
        break;
      case '4': fprintf(file, "\e[1;31mValeur de type \e[1;34mint \e[1;31mattendue pour l'argument \e[0;33m'%s'\e[0m\n", (*error)+1);
        break;
      case '5': fprintf(file, "\e[1;31mArgument \e[0;33m'%s' \e[1;31mtrop long\e[0m\n", (*error)+1);
        break;
      case '6': fprintf(file, "\e[1;31mErreur dans une des valeurs de l'argument \e[0;33m'%s'\e[0m\n", (*error)+1);
        break;
      case '7': fprintf(file, "\e[1;31mPas d'argument !\e[0m\n");
        break;
      default : fprintf(file, "\e[1;31mErreur inconnue !\e[0m\n");
    }
    error++;
  }
}

void Cliarg_free_error(char **error[])
{
  char i;
  for (i = 0; (*error)[i]; i++)
    free((*error)[i]);
  free(*error);
}

