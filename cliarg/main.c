#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "functions.h"
#include "error.h"
#include "main.h"


static bool Set_arguments(char *arglist[], uchar arglen, char *argument, char **error[], cliarg_t **args);
static void Add_argument(cliarg_t **args, char name, char *value, char type);
static void Set_value(cliarg_t *arg, char *val, char type);


cliarg_t *Cliarg_check_arguments(char *arglist[], char *argument[], uchar cmdlen, char **error[])
{
  cliarg_t *args = NULL;
  uchar i, arglen;
  char e = true;

  /*
  ** Mémory allocation of errors
  */
  for (arglen = 0; arglist[arglen] != NULL; arglen++);
  *error = malloc(sizeof(char *) * (arglen + 1));
  for (i = 0; i < arglen + 1; i++) (*error)[i] = NULL;

  /*
  ** If no arguments, set the error
  */
  if (cmdlen == 1) {
    Write_error('7', NULL, error);
    return NULL;
  }
  cmdlen--;
  argument++;

  /*
  ** Check arguments (type of value if value is expected) and return it in cliarg_t linked list
  */
  for (i = 0; i < cmdlen; i++)
  {
    e = true;
    if (!Check_length(argument[i])) {
      Write_error('5', argument[i], error);
    }
    else if (Set_arguments(arglist, arglen, argument[i], error, &args)) {
      e = false;
    }
  }
  if (e) return NULL;
  if (**error) return NULL;
  return args;
}

cliarg_t *Cliarg_get_argument(cliarg_t *ca, char name)
{
  while (ca)
  {
    if (ca->_name == name) return ca;
    ca = ca->_next;
  }
  return NULL;
}

bool Set_arguments(char *arglist[], uchar arglen, char *argument, char **error[], cliarg_t **args)
{
  char *name   = NULL;
  char  argpos = 0;
  char *value  = NULL;
  char  type;
  char  e       = false;

  /* Vérification de la validité de argument */
  if (!(name = Get_arg_name(argument, error))) {
    if (!e) e = true;
  }
  if (!(value = Get_value_n_set_type(argument, arglist, arglen, &type, &argpos, name, error))) {
    if (!e) e = true;
  }
  if (!e) {
    Add_argument(args, argpos, value, type);
    free(value);
  }
  if (name) free(name);
  return (e) ? false: true;
}

void Add_argument(cliarg_t **args, char name, char *value, char type)
{
  cliarg_t *next = *args;
  cliarg_t *tmpArg = malloc(sizeof(cliarg_t));

  if (tmpArg) {
    tmpArg->_name = name;
    tmpArg->_type = type;
    tmpArg->_next = NULL;
    Set_value(tmpArg, value, type);

    if (!*args)
    {
      *args = tmpArg;
      return;
    }
    while (next->_next) next = next->_next;
    next->_next = tmpArg;
  }
}

void Cliarg_free_arguments(cliarg_t **args)
{
  int i;
  cliarg_t *tmpArgs = *args;
  cliarg_t *tmp2;
  
  while (tmpArgs)
  {
    if (tmpArgs->_type == INT_A) free((int*)tmpArgs->value);
    else if (tmpArgs->_type == STR) free((char*)tmpArgs->value);
    else if (tmpArgs->_type == STR_A) {
      for (i = 0; ((char**)tmpArgs->value)[i]; i++) 
        free(((char**)tmpArgs->value)[i]);
      free((char**)tmpArgs->value);
    }
    tmp2 = tmpArgs;
    tmpArgs = tmpArgs->_next;
    free(tmp2);
  }
  *args = NULL;
}

void Set_value(cliarg_t *arg, char *val, char type)
{
  int   *v_int;
  int   *t_int = NULL;
  char  *v_char;
  char **t_char = NULL;
  arg->value = NULL;

  if (type == BOOL) {
    v_int = malloc(sizeof(int));
    *v_int = (*val == 't') ? true: false;
    arg->value = (void*)v_int;
  }
  else if (type == INT) {
    v_int = malloc(sizeof(int));
    *v_int = Str_to_int(val);
    arg->value = (void*)v_int;
  }
  else if (type == INT_A) {
    t_int = Str_to_intArray(val, &(arg->_vcount));
    arg->value = (void*)t_int;
  }
  else if (type == STR) {
    v_char = calloc(strlen(val) + 1, sizeof(char));
    strcpy(v_char, val);
    arg->value = (void*)v_char;
  }
  else if (type == STR_A) {
    t_char = Str_to_strArray(val, &(arg->_vcount));
    arg->value = (void*)t_char;
  }
}

char Cliarg_get_type(cliarg_t *args, char name)
{
  cliarg_t *tmp = NULL;

  if ((tmp = Cliarg_get_argument(args, name)) != NULL)
    return tmp->_type;
  return 0;
}

void *Cliarg_get_copy_value(cliarg_t *args, char name, char *count)
{
  cliarg_t *tmp   = NULL;
  void     *value = NULL;

  if ((tmp = Cliarg_get_argument(args, name)) != NULL)
  {
    // malloc memory and copy the argument in pointed address
    switch(tmp->_type)
    {
      case STR:
        value = (void*)calloc(strlen(STRING(tmp->value)) + 1, sizeof(char));
        strcpy(STRING(value), STRING(tmp->value));
        return value;

      case STR_A:
        value = (void*)calloc(tmp->_vcount + 1, sizeof(char*));
        for (int i = 0; i < tmp->_vcount; i++)
        {
          STRARRAY(value)[i] = calloc(strlen(STRARRAY(tmp->value)[i]) + 1, sizeof(char));
          strcpy(STRARRAY(value)[i], STRARRAY(tmp->value)[i]);
        }
        if (count) *count = tmp->_vcount;
        return value;

      case BOOL:
      case INT:
        value = (void*)malloc(sizeof(int));
        *INTEGER(value) = *INTEGER(tmp->value);
        return value;

      case INT_A:
        value = (void*)calloc(tmp->_vcount, sizeof(int));
        for (int i = 0; i < tmp->_vcount; i++)
        {
          INTARRAY(value)[i] = INTARRAY(tmp->value)[i];
        }
        if (count) *count = tmp->_vcount;
        return value;
    }
  }
  return NULL;
}

