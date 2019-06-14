#ifndef FC_CDF_H_060318
#define FC_CDF_H_060318

typedef struct arguments
{
  char **fil;
  char  *dir;
  char	*wri;
  char  *exp;
  int   all;
  int   rec;
  int	sta;
  int	end;
}
arguments;

typedef struct dirent dirent;

#define BUFSIZE      1024
//#define DIRECTORY    16893

#define CDF_FILE     0
#define CDF_MATCH    1

#define MAX_REPLACE  20

/*
** Tell to the user if he want continue or quit the programme
** this function use exit() to quit the programme
*/
void User_confirmation(void);

/*
** Check if the arguments number given by the cli is valid
** return 1 if it's ok
*/
int Arguments_number_check(int argc, char *scriptname);
void Raw_mode(int on);

/*
** Allow to use \d and replace .*x or .+x per [^x]* or [^x]+
** return a pointer of the new memory allocation
*/
char *Correct_regex(const char *restrict s);

/*
** Print lines if regex match and highlight the pattern
*/
void Print_match_lines(char *buffer, myregex_t *match, int line);

/*
** convert a number 32 bits to ascii string
** return a constant number
*/
const char *Int32_to_ascii(int32_t n);

/*
** Get and check the arguments from the cli 
** and set the pointer of arguments structure
*/
int Set_Arguments(arguments **main_arg, int argc, char *argv[]);

//void Set_MainArg(arguments **arg);
void Free_Arguments(arguments *arg);

/*
** Print the help menu
*/
void Help(const char *script);

/*
** Return the extension of the file name given in small caps
*/
char *Get_tolower_extension(const char *filename);

/*
** ???
*/
int Valid_File(const char *filename);

/*
** Check if the file contain non ascii characters
*/
int is_readable(const char *filename);

/*
** Search all the files in the directory
*/
void Search(int total[], arguments *arg, char *path, int rec);

/*
** Check if the regex match and print the lines
*/
int Search_On_File(const char *filename, arguments *arg, int total[], unsigned int fsize);

/*
** Check if the file is authorized
*/
int Authorized_File(char **filter, char *filename);

/*
**
*/
char	*Print_replacement_and_replace(char *buffer, myregex_t *match, int line, char *write);

#endif
