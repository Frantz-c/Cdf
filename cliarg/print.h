#ifndef FC_CMDARG_PRINT_H_060318
#define FC_CMDARG_PRINT_H_060318


#define STR   1
#define STR_A 2
#define INT   3
#define INT_A 4
#define BOOL  5

/*
** Print arguments, values and types from cmdarg_t linked list
*/
extern void Cliarg_show_arguments(cliarg_t *args);

/*
** convert the type to string and return it
*/
extern char *Get_string_type(char type);

#endif
