#ifndef MY_MYREGEX_H_130817
#define MY_MYREGEX_H_130817

/*
** "regex.h"
**
** Fichier contenant les fonctions 
** de manipulation des chaines via les regex.
*/

typedef struct myregex_t
{
  int start;
  int end;
  struct myregex_t *next;
}
myregex_t;


extern int preg_match(char *regex, char *string);
extern int preg_match_get(char *regex, char *string, myregex_t **pmatch);
extern int preg_match_get_all(char *regex, char *string, myregex_t **pmatch);
extern void free_myregex_t(myregex_t *regex);

#endif
