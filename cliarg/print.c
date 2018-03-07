#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "print.h"


void Cliarg_show_arguments(cliarg_t *args)
{
  int   i;
  char *type;

  while (args)
  {
    type = Get_string_type(args->_type);
    printf("\e[1;36mArgument name ==> \e[0;36m-%c\n", args->_name);
    printf("\e[1;36mArgument type ==> \e[0;36m%s\n", type);
    switch (args->_type) {
      case BOOL: 
        printf("\e[1;36mValue         ==> \e[0;36m%s\n", 
        (*((int*)args->value) == 1) ? "true": "false");
      break;
      case INT:
        printf("\e[1;36mValue         ==> \e[0;36m%d\n", *((int*)args->value));
      break;
      case INT_A:
        printf("\e[1;36mValue(s)      ==> \e[0;36m");
        for (i = 0; i < args->_vcount; i++) {
          printf("%d, ", ((int*)args->value)[i]);
        }
        puts("");
      break;
      case STR:
        printf("\e[1;36mValue         ==> \e[0;36m%s\n", (char*)args->value);
      break;
      case STR_A:
        printf("\e[1;36mValue(s)      ==> \e[0;36m");
        for (i = 0; ((char**)args->value)[i]; i++) {
          printf("%s, ", ((char**)args->value)[i]);
        }
        puts("");
      break;
    }
    puts("\e[0m\n");
    free(type);
    args = args->_next;
  }
}

char *Get_string_type(char type)
{
  char *ret = NULL;

  switch(type)
  {
    case BOOL : COPY(ret, "bool");         break;
    case INT  : COPY(ret, "int");          break;
    case STR  : COPY(ret, "string");       break;
    case INT_A: COPY(ret, "int array");    break;
    case STR_A: COPY(ret, "string array"); break;
  }
  return ret;
}


