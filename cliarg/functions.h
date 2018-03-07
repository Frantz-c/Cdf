#ifndef FC_CMDARG_FUNCTIONS_H_060318
#define FC_CMDARG_FUNCTIONS_H_060318

/*
** Get first value length into an argument
*/
extern int Value_len(const char *str, int pos);

/*
** Convert ascii number in integer
*/
extern int Ascii_number_to_int(char c);

/*
** Get the number of values into an argument
*/
extern int Get_values_number(const char *v);

/*
** Convert String to int
*/
extern int Str_to_int(const char *v);

/*
** Convert string to intger array
*/
extern int *Str_to_intArray(const char *v, char *vcount);

/*
** Convert string to string array
*/
extern char **Str_to_strArray(const char *v, char *vcount);

/*
** Return the type of the argument expected
*/
extern char Get_arg_type(const char *str);

/*
** Return the value of the argument
*/
extern char *Get_arg_value(char *argument, char type, char **error[]);

/*
** Return the name of the argument
*/
extern char *Get_arg_name(char *argument, char **error[]);

/*
** return the value and set the type
*/
extern char *Get_value_n_set_type
    (
      char *argument,
      char *argList[],
      uchar arglen,
      char *type,
      char *sname,
      char *name,
      char **error[]
    );

/*
** Check if the cli argument match with the expected argument
*/
static char Match_cli_arg_n_expected_arg(char *cli_arg, char short_arg, char *long_arg);

/*
** Check if the length of the value and
*/
extern bool Check_length(char *s);


extern bool zone_match(char c, char start, char end);


#endif
