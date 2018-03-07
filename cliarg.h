#ifndef FC_CLIARG_H_31102017
#define FC_CLIARG_H_31102017


/* DEFINES ===================================================*/


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


/* STRUCTURES ================================================*/


typedef struct cliarg_t
{
  char  _name;
  char  _type;     /* STR, INT, STR_P, INT_P, BOOL */
  char  _vcount;   /* array length if it's an array */

  void  *value;   

  struct cliarg_t *_next;
}
cliarg_t;


/* PROTOTYPES =================================================*/


/*
** Check arguments and return a structure cliarg_t containing 
** arguments and values
*/
extern cliarg_t *Cliarg_check_arguments(char *arglist[], char *cli_arg[], unsigned char cli_len, char **error[]);

/*
** return an argument of the cliarg_t struct founded by short name
*/
extern cliarg_t *Cliarg_get_argument(cliarg_t *ca, char name);

/*
** Print arguments, values and types from cmdarg_t linked list
*/
extern void Cliarg_show_arguments(cliarg_t *args);

/*
** free the cliarg_t linked list from the memory
*/
extern void Cliarg_free_arguments(cliarg_t **args);

/*
** return the type of the sliarg_t element
*/
extern char Cliarg_get_type(cliarg_t *args, char name);

/*
** return the value of the cliarg_t struct founded by short name
*/
extern void *Cliarg_get_copy_value(cliarg_t *args, char name, char *count);

/*
** Print the errors writed into the string array "error"
*/
extern void Cliarg_print_error(FILE *f, char *error[]);

/*
** Free the string array "error" from the memory
*/
extern void Cliarg_free_error(char **error[]);

#endif
