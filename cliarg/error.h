#ifndef FC_CMDARG_ERROR_H_060318
#define FC_CMDARG_ERROR_H_060318

/*
** Write an error into the string array "error"
*/
extern void Write_error(char c, const char *e, char **error[]);

/*
** Print the errors writed into the string array "error"
*/
extern void Cliarg_print_error(FILE *f, char *error[]);

/*
** Free the string array "error"
*/
extern void Cliarg_free_error(char **error[]);


#endif
