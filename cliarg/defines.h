#ifndef FC_CLIARG_DEFINES_H_31102017
#define FC_CLIARG_DEFINES_H_31102017

/* DEFINES ===================================================*/

#define NOTYPE -1
#define ERROR -1
#define bool char
#define uchar unsigned char
#define false 0
#define true 1

#define STR   1
#define STR_A 2
#define INT   3
#define INT_A 4
#define BOOL  5

#define CLIARG_NO_ARGUMENTS '7'

#define INTEGER(var)        ((int*)(var))
#define INTARRAY(var)       ((int*)(var))
#define STRING(var)         ((char*)(var))
#define STRARRAY(var)       ((char**)(var))

#define CLIARG_SET_INT(var, val) \
  do{\
  (var) = malloc(sizeof(int));\
  *(var) = val;\
  }while(0);

#define CLIARG_SET_STR(var, str) \
  do{\
  (var) = malloc(sizeof(char) * (strlen(str) + 1));\
  strcpy(var, str);\
  (var)[strlen(str)] = '\0';\
  }while(0);

#define COPY(a, b)\
  (a) = calloc(strlen(b) + 1, sizeof(char));\
  strcpy(a, b);

#define MAX_VALUES 10
#define DEFAULT_LENGTH_NAME 40
#define DEFAULT_LENGTH_VALUE 80


/* STRUCTURES ================================================*/

typedef struct cliarg_t
{
  char  _name;
  char  _type;     /* 'STR', 'INT', 'STR_P', 'INT_P', 'BOOL' */
  char  _vcount;   /* longueur du tableau si tableau de valeurs */

  /* valeurs */
  void  *value;

  struct cliarg_t *_next;
}
cliarg_t;

#endif
