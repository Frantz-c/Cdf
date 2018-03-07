#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "error.h"
#include "functions.h"


int Value_len(const char *str, int pos)
{
  int len = pos;

  while (*str != ',' && *str != '\0')
  {
    str++;
    pos++;
  }
  pos++;
  return pos - 1 - len;
}

int Ascii_number_to_int(char c)
{
  return (c >= 48 && c <= 57) ? c - 48: 0;
}

bool Check_length(char *s)
{
  uchar i;

  for (i = 0; s[i] != '='; i++) {
    if (i > DEFAULT_LENGTH_NAME) return false;
  }
  for (; s[i] != '\0'; i++) {
    if (i > DEFAULT_LENGTH_NAME + DEFAULT_LENGTH_VALUE) return false;
  }
  return true;
}

char Match_cli_arg_n_expected_arg(char *cli_arg, char short_arg, char *long_arg)
{
  uchar i;

  for (i = 0; cli_arg[i] == '-'; i++);
  if (i == 1) {
    return (cli_arg[i] == short_arg) ? 1: false;
  }
  else if (i == 2)
  {
    for (; cli_arg[i] != '=' && long_arg[i - 2] != '\0'; i++)
    {
      if (cli_arg[i] != long_arg[i - 2]) {
        return false;
      }
    }
    if (cli_arg[i] != '\0' && cli_arg[i] != '=') {
      return false;
    }
    return 2;
  }
  return false;
}

char *Get_value_n_set_type
    (
      char *cli_arg,
      char *arglist[],
      uchar arglen,
      char *type,
      char *sname,
      char *name,
      char **error[]
    )
{
  char *value = NULL;
  uchar i;
  char long_or_short;
  bool e = false;

  if (!name) return NULL;

  for (i = 0; i < arglen - 1; i += 2)
  {
    if ((long_or_short = Match_cli_arg_n_expected_arg(cli_arg, arglist[i][0], arglist[i + 1])))
    {
      *type = Get_arg_type(arglist[i]);
      if (*type == NOTYPE) continue;
      value = Get_arg_value(cli_arg, *type, error);
      *sname = arglist[i][0];
      return value;
    }
    else if (!e) {
      e = true;
      continue;
    }
  }

  if (e) {
    Write_error('3', cli_arg, error);
    return NULL;
  }
  return value;
}

char Get_arg_type(const char *str)
{
  int len = strlen(str);

  if (len == 1) return BOOL;
  if (len == 2)
  {
    if (str[1] == ':') return STR;
    if (str[1] == '.') return INT;
  }
  if (len == 3)
  {
    if (str[1] == ':' && str[2] == '+') return STR_A;
    if (str[1] == '.' && str[2] == '+') return INT_A;
  }
  return NOTYPE;
}

char *Get_arg_name(char *argument, char **error[])
{
  char *tmp = calloc(DEFAULT_LENGTH_NAME, sizeof(char));
  char *s = NULL;
  int len = strlen(argument);
  uchar i = 0;
  uchar j;

  if (len >= 2) {
    if (argument[0] == '-') {
      if (argument[1] == '-' && len > 3) i++;
      i++;
    }
  }else {
    Write_error('0', argument, error);
    free(tmp);
    return NULL;
  }

  j = i;
  for (; i < 40 && argument[i] != '\0' && argument[i] != '='; i++)
  {
    if (j == 1 && i - j > 1) {
      Write_error('0', argument, error);
      free(tmp);
      return NULL;
    }
    if (zone_match(argument[i], 'a', 'z') || zone_match(argument[i], 'A', 'Z')) {
      tmp[i - j] = argument[i];
    }
    else {
      Write_error('0', argument, error);
      free(tmp);
      return NULL;
    }
  }
  i++;
  tmp[i - j] = '\0';
  s = malloc(sizeof(char) * (i - j));
  strcpy(s, tmp);
  free(tmp);
  return s;
}

char *Get_arg_value(char *argument, char type, char **error[])
{
  char *tmp = calloc(DEFAULT_LENGTH_VALUE, sizeof(char));
  char *s = NULL;
  uchar i, j, k;

  // if an value is given to bool type
  if (type == BOOL)
  {
    for (i = 0; argument[i] != '\0'; i++)
    {
      if (argument[i] == '=') {
        Write_error('1', argument, error);
        free(tmp);
        return NULL;
      }
    }
    s = malloc(sizeof(char) * 2);
    *s = 't'; s[1] = '\0';
    free(tmp);
    return s;
  }

  else 
  {
    // no value given ???
    for (i = 0, j = 0; argument[i] != '=' && argument[i] != '\0'; i++);
    if (argument[i] == '\0' ) {
      Write_error('2', argument, error);
      free(tmp);
      return NULL;
    }
    i++;
    for (; argument[i] != '\0'; i++, j++)
    {
      if (type == INT) {
        if (!zone_match(argument[i], '0', '9')) {
          Write_error('4', argument, error);
          free(tmp);
          return NULL;
        }
      }
      else if (type == INT_A) {
        if (!zone_match(argument[i], '0', '9')) {
          if (k || argument[i] != ',') {
            Write_error('4', argument, error);
            free(tmp);
            return NULL;
          }
          k++;
        }else {
          if (k) k = 0;
        }
      }
      else if (type == STR_A) {
        if (argument[i] == ',' && argument[i - 1] == ',') {
          Write_error('6', argument, error);
          free(tmp);
          return NULL;
        }
      }
      tmp[j] = argument[i];
    }
  }

  j++;
  s = malloc(sizeof(char) * j);
  strcpy(s, tmp);

  free(tmp);
  return s;
}

int Get_values_number(const char *v)
{
  int nb_v = 1;
  while (*(v++) != '\0')
  {
    if (*v == ',') {
      nb_v++;
      if (nb_v == MAX_VALUES) break;
    }
  }
  return nb_v;
}
bool zone_match(char c, char start, char end)
{
  return (c >= start && c <= end) ? true: false;
}

int Str_to_int(const char *v)
{
  int len = strlen(v);
  char i = 0;
  int v_int = 0;
  int mod = 1;


  if (len < 5 || (len == 5 && *v < '6') )
  {
    for (i = len - 1; i >= 0; i--)
    {
      v_int += mod * Ascii_number_to_int(v[i]);
      mod *= 10;
    }
  }
  return v_int;
}

int *Str_to_intArray(const char *v, char *vcount)
{
  int len;
  int i, j, k;
  int *intArray = NULL;
  int v_int = 0;
  int mod = 1;
  *vcount = Get_values_number(v);

  if ( !(intArray = malloc(sizeof(int) * *vcount)) ) return NULL;

  for (j = 0, k = 0; j < *vcount; j++)
  {
    len = Value_len(v+k, k);
    if (len < 5 || (len == 5 && *v < '6') )
    {
      for (i = (k + len) - 1; i >= k; i--)
      {
        v_int += mod * Ascii_number_to_int(v[i]);
        mod *= 10;
      }
    }else {
      free(intArray);
      return NULL;
    }
    
    intArray[j] = v_int;
    mod = 1;
    v_int = 0;
    k += len + 1;
  }
  return intArray;
}

char **Str_to_strArray(const char *v, char *vcount)
{
  int len;
  int j, k;
  char **strArray = NULL;
  *vcount = Get_values_number(v);

  if ( !(strArray = calloc((*vcount + 1), sizeof(char *))) ) return NULL;

  for (j = 0, k = 0; j < *vcount; j++)
  {
    len = Value_len(v+k, k);

    if ( !(strArray[j] = calloc((len + 1), sizeof(char)) )) return NULL;
    strncpy(strArray[j], v+k, len);
    k += len + 1; 
  }

  return strArray;
}


