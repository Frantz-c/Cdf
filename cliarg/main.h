#ifndef FC_CMDARG_MAIN_H_31102017
#define FC_CMDARG_MAIN_H_31102017

/*
** Check arguments and return a structure cliarg_t containing 
** arguments and values
*/
extern cliarg_t *Cliarg_check_arguments(char *arglist[], char *argument[],
                                        uchar cmdlen,    char **error[]);

/*
** return the type of the sliarg_t element
*/
extern char Cliarg_get_type(cliarg_t *args, char name);

/*
** return an argument of the cliarg_t struct founded by short name
*/
extern cliarg_t *Cliarg_get_argument(cliarg_t *ca, char name);

/*
** return the value of the cliarg_t struct founded by short name
*/
extern void *Cliarg_get_copy_value(cliarg_t *args, char name, char *count);

/*
** free the cliarg_t linked list from the memory
*/
extern void Cliarg_free_arguments(cliarg_t **args);

#endif
