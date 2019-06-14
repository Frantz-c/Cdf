#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <termios.h> 
#include <fcntl.h>
#include "cliarg.h"
#include "myregex.h"
#include "cdf.h"
#include "my_functions.h"

#define ESCAPE	27

#define free_pointer(ptr)	\
	do {\
	printf("FREE %p (var = " #ptr ") :: line %d\n", (void*)ptr, __LINE__);\
	free(ptr);\
	} while (0)

int main(int argc, char *argv[])
{
	arguments *args = NULL;
	int total[2] = {0};

	if (Set_Arguments(&args, argc, argv) != 0) return -1;

	Search(total, args, args->dir, 0);

	fmt_print("\e[1;34m%d occurrence", total[CDF_MATCH]);
	if (total[CDF_MATCH] > 1 || total[CDF_MATCH] == 0) my_putc('s');
	fmt_print(" dans %d fichier", total[CDF_FILE]);
	if (total[CDF_FILE] > 1 || total[CDF_FILE] == 0) my_putc('s');
	print("\e[0m\n\n");

	Free_Arguments(args);
	return 0;
}

int Set_Arguments(arguments **main_arguments, int argc, char *argv[])
{
	cliarg_t  *args;
	arguments *argument  = malloc(sizeof(arguments));
	char     **error     = NULL;
	char      *arglist[] = 
	{
		"d:" , "directory", /* research directory */
		"r." , "recursive", /* recursive level */
		"a"  , "all",       /* search in files beginning per '.' too */
		"f:+", "filter",    /* files where to search */
		"w:", "write",		/* replacement */
		"s.", "start",		/* match start */
		"e.", "end",		/* match end */
		NULL
	};
	cliarg_t	*ptr;


	if (Arguments_number_check(argc, *argv) != 0) return -1;

	/*
	 ** Get arguments from cli
	 */
	if ((args = Cliarg_check_arguments(arglist, argv + 1, argc - 1, &error)) == NULL) 
	{
		if (**error != CLIARG_NO_ARGUMENTS) {
			Cliarg_print_error(stdout, error);
			Cliarg_free_error(&error);
			Help(argv[0]);
			return (-1);
		}
	}

	/* 
	 ** Set arguments
	 */
	if ((argument->dir = STRING(Cliarg_get_copy_value(args, 'd', NULL))) == NULL)
		CLIARG_SET_STR(argument->dir, ".");

	argument->wri = STRING(Cliarg_get_copy_value(args, 'w', NULL));
	argument->rec = ((ptr = Cliarg_get_argument(args, 'r'))) ? INT_VALUE(ptr->value) : 1;
	argument->all = ((ptr = Cliarg_get_argument(args, 'a'))) ? INT_VALUE(ptr->value) : 0;
	argument->sta = ((ptr = Cliarg_get_argument(args, 's'))) ? INT_VALUE(ptr->value) : 0;
	argument->end = ((ptr = Cliarg_get_argument(args, 'e'))) ? INT_VALUE(ptr->value) : 0;
	argument->fil = STRARRAY(Cliarg_get_copy_value(args, 'f', NULL));
	argument->exp = argv[1]; //correct_regex() -> \t bug ??

	printf("s = %d, wri = %p\n", argument->sta, (void*)argument->wri);
	/* 
	 ** Print regex and filters 
	 */
	printf("\e[0;35mRegex = \e[1;35m%s\e[0;35m\n", argument->exp);
	if (argument->fil) {
		for (int i = 0; argument->fil[i]; i++) 
			printf("fichier %d = \e[1;35m'%s'\e[0;35m\n", i, argument->fil[i]);
	}


	User_confirmation();


	*main_arguments = argument;

	Cliarg_free_arguments(&args);
	Cliarg_free_error(&error);
	return 0;
}

int Arguments_number_check(int argc, char *scriptname)
{
	if (argc < 2)
	{
		Help(scriptname);
		return -1;
	}
	else if (argc > 7)
	{
		print("\e[0;31mTrop d'arguments.\e[0m\n\n");
		Help(scriptname);
		return -1;
	}

	return 0;
}

void User_confirmation(void)
{
	print("\e[0;36mLancer la recherche ? \e[0;35m(oui = entrée)\e[0m\n");
	Raw_mode(1);

	if (getchar() != '\r')
	{
		Raw_mode(0);
		print("\e[0;31mFin du programme...\e[0m\n");
		exit(EXIT_SUCCESS);
	}
	Raw_mode(0);
	print("\e[0;32mRecherche en cours...\e[0m\n\n");
}

int	Replace_confirmation(const char *s, int start, int end, const char *replace, int line)
{
	int	c;

	printf("\e[0;33m%d: \e[0;36mVoulez-vous vraiment remplacer "
			"\e[0;33m'%s'\e[0;36m\n"
			"                           par \e[0;33m'%.*s%s%s'\e[0;36m ? \e[0;35m(oui = entrée)\e[0m\n",
			line, s, start, s, replace, s + end);
	Raw_mode(1);

	if ((c = getchar()) != '\r')
	{
		Raw_mode(0);
		if (c == ESCAPE)
			exit(1);
		puts("\e[0;31mOccurrence non-remplacée.\e[0m\n");
		return (0);
	}
	Raw_mode(0);
	puts("\e[0;32mOccurrence remplacée.\e[0m\n");
	return (1);
}

int	Backup_confirmation(const char *name)
{
	int c;

	printf("\e[0;36mVoulez-vous créer un backup du fichier \e[0;33m\"%s\""
			"\e[0;36m ? \e[0;35m(oui = entrée)\n\e[0;37mnom du backup: %s.bkup\n\e[0m",
			name, name);
	Raw_mode(1);

	if ((c = getchar()) != '\r')
	{
		Raw_mode(0);
		if (c == ESCAPE)
			exit(1);
		puts("\e[0;31mPas de backup enregistré.\e[0m\n");
		return (0);
	}
	Raw_mode(0);
	puts("\e[0;31mBackup enregistré.\e[0m\n");
	return (1);
}


void Search(int total[], arguments *arg, char *path, int rec)
{
	DIR         *directory;
	struct stat  stats;
	dirent      *dirinfo;
	char        *newpath;

	if ((directory = opendir(path)) == NULL) {
		fprintf(stderr, "Impossible d'ouvrir le dossier '%s'\n", path);
		return;
	}

	/*
	 ** Read all the files in the directory, 
	 ** open files and search into 
	 ** or open a directory and read all the files into...
	 */
	while ((dirinfo = readdir(directory)) != NULL)
	{
		if (!my_strcmp(dirinfo->d_name, ".") || !my_strcmp(dirinfo->d_name, "..")) continue;
		if (*dirinfo->d_name == '.' && arg->all == 0) continue;

		if ((newpath = calloc(strlen(path) + strlen(dirinfo->d_name) + 2, sizeof(char))) == NULL) {
			fprintf(stderr, "Erreur d'allocation de mémoire ligne %d\n", __LINE__);
			closedir(directory);
			return;
		}
		sprintf(newpath, "%s/%s", path, dirinfo->d_name);


		if (stat(newpath, &stats) == -1) continue;


		/*
		 ** If the file is a directory
		 */
		if (S_ISDIR(stats.st_mode))
		{
			if (rec + 1 == arg->rec) {
				free(newpath);
				continue;
			}
			Search(total, arg, newpath, rec + 1);
		}
		else
		{
			if (Authorized_File(arg->fil, dirinfo->d_name) && Valid_File(newpath)) {
				Search_On_File(newpath, arg, total, stats.st_size);
			}
			free(newpath);
		}
	}
	free(path);
	closedir(directory);
}

int	Search_On_File(const char *filename, arguments *arg, int total[], unsigned int fsize)
{
	static char			*buffer = NULL;
	static char			*copy = NULL;
	unsigned int		i = 0;
	unsigned int		j;
	static unsigned int	buflen = 0;
	int					line = 1;
	int					fd;
	myregex_t			*match = NULL;
	char				title = 0;
	char				*buf;
	char				*lf;
	char				*newline;
	int					n_replace = 0;

	if ((fd = open(filename, O_RDONLY)) == -1) {
		fprintf(stderr, "\e[0;31mImpossible d'ouvrir le fichier \"%s\" ligne %d.\e[0m\n", filename, __LINE__);
		return -1;
	}

	if (buflen < fsize + 1)
	{
		free(buffer);
		buffer = malloc(fsize + 1);
		if (arg->wri)
		{
			free(copy);
			copy = malloc(fsize + 1);
		}
	}
	read(fd, buffer, fsize);
	close(fd);
	buffer[fsize] = 0;
	lf = buffer - 1;

	/*
	 ** Get one line and search regex into
	 */
	do
	{
		buf = lf + 1;
		if ((lf = strchr(buf, '\n')) != NULL)
			*lf = '\0';

		/*
		 ** Print line with number of line and highlight the match
		 ** If it's the first match, print file name
		 */
		if (preg_match_get_all(arg->exp, buf, &match) == 0)
		{
			if (title == 0) 
			{
				title = 1;
				printf("\n\e[1m\e[7;34m» %s «\e[0m\n\n", filename);
				total[CDF_FILE]++;
			}
			total[CDF_MATCH]++;

			match->start += arg->sta;
			match->end -= arg->end;
			if (arg->wri)
			{
				if ((newline = Print_replacement_and_replace(buf, match, line, arg->wri)) == NULL)
				{
					free_myregex_t(match);
					match = NULL;
					goto copy_line_into_tmpfile;
				}
				n_replace++;
				//copy newline into tmpfile
				j = strlen(newline);
				strncpy(copy + i, newline, j);
				i += j;
				copy[i++] = '\n';
			}
			else
			{
				Print_match_lines(buf, match, line);
			}
			

			free_myregex_t(match);
			match = NULL;
		}
		else if (arg->wri)
		{
			copy_line_into_tmpfile:
			j = strlen(buf);
			strncpy(copy + i, buf, j);
			i += j;
			copy[i++] = '\n';
		}
		line++;
		buf = lf + 1;
	}
	while (lf);

	if (title)
		printf("\e[0m\n"); // If results are printed, close the color
	if (arg->wri && n_replace)
	{
		if (i) i--;
		if (Backup_confirmation(filename) == 1)
		{
			char	*bkupname = malloc(strlen(filename) + 6);
			strcpy(bkupname + strlen(filename), ".bkup");
			strncpy(bkupname, filename, strlen(filename));
			rename(filename, bkupname);
			free(bkupname);
		}
		fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0664);
		if (fd == -1)
		{
			fprintf(stderr, "can't create %s\n", filename);
			return (-1);
		}
		write(fd, copy, i);
		close(fd);
	}
	return (0);
}

/*
 * replacement syntax:
	./cdf 'funcname\([^)]+\)' -w='newname\(%\)'
	./cdf '

*/


static char	*get_replacement(char *s, int len, char *write)
{
	/* % joker */
	char			*start;
	char			*end;
	char			*joker;
	/* */
	char			*tmp;
	unsigned int	new_len = 0;
	char			*replace;

	printf("\e[1;34mWRITE = \"%s\"\e[0m\n", write);
	joker = strchr(write, '%');
	if (joker)
	{
		for (tmp = s; *tmp && *tmp != *(joker - 1); tmp++);
		start = (!tmp) ? s : tmp;
		for (tmp = s + len - 1; tmp != s && *tmp != *(joker + 1); tmp--);
		end = (tmp == s) ? s + len - 1 : tmp;
		if (end < start)
			end = s + len - 1;
		new_len = end - start;
		new_len += (joker - 1) - write;
		new_len += strlen(joker + 1);
	}
	else
	{
		new_len = strlen(write);
	}

	replace = malloc(new_len + 1);
	if (!joker) {
		strncpy(replace, write, new_len);
		return (replace);
	}
	strncpy(replace, write, (joker - 1) - write);
	strncpy(replace + (unsigned long)((joker - 1) - write), start, end - start);
	strcpy(replace + (unsigned long)((joker - 1) - write) + (end - start), joker + 1);
	return (replace);
}

char	*Print_replacement_and_replace(char *buffer, myregex_t *match, int line, char *write)
{
	static unsigned int	length = 0;
	static char			*newline;
	char				*replace;

	replace = get_replacement(buffer + match->start, match->end - match->start, write);
	if (Replace_confirmation(buffer, match->start, match->end, replace, line) == 0)
	{
		free(replace);
		return (NULL);
	}
	if (length <= strlen(buffer) + (strlen(replace) - (match->end - match->start)))
	{
		free(newline);
		length = (strlen(buffer) + (strlen(replace) - (match->end - match->start)));
		newline = malloc(length + 1);
	}
	if (match->start)
		strncpy(newline, buffer, match->start);
	strcpy(newline + match->start, replace);
	if (buffer[match->end])
		strcpy(newline + match->start + strlen(replace), buffer + match->end);
	puts("OKOK");
	free(replace);
	return (newline);
}

void	Print_match_lines(char *buffer, myregex_t *match, int line)
{
	int i;
	const char *line_n = my_itoa(line);
	char		*start = buffer;

	/*
	 ** Print line number
	 */
	printf("\e[0;33m%s :\e[0;37m ", line_n);

	/*
	 ** Print line with colored occurrences
	 */
	for (i = 0; *buffer; i++, buffer++)
	{
		if (i == match->start)
		{
			printf("%.*s\e[1;33m", (int)(buffer - start), start);
			start = buffer;
		}
		else if (i == match->end)
		{
			printf("%.*s\e[0;37m", (int)(buffer - start), start);
			if (match->next)
				match = match->next;
			start = buffer;
		}
	}
	printf("%.*s\n", (int)(buffer - start), start);
}

void Help(const char *script)
{
	printf
	(
		"\e[7;31m                                Usage :                                   \e[0m\n\n"
		"\e[1;31m%s \e[0;33m\"regex\"\e[0m"
		" [\e[1;31m-d\e[0m=\e[0;33m\"directory\"\e[0m]\e[1;32m?\e[0m"
		" [\e[1;31m-r\e[0m=\e[0;33m\"recursive\"\e[0m]\e[1;32m?\e[0m"
		" [\e[1;31m-f\e[0m=\e[0;33m\"files\"\e[0m]\e[1;32m?\e[0m"
		" [\e[1;31m-a\e[0m]\e[1;32m?\e[0m\n\n"
		"\e[0;32mArguments : \n"
		"  \e[0;36m-d / --directory : \e[0;37mDirectory where to look \e[0m(\e[1;34mstring\e[0m)\n"
		"  \e[0;36m-f / --filter    : \e[0;37mFiles where to look \e[0m(\e[1;34mregex(,regex)*\e[0m)\n"
		"  \e[0;36m-r / --recursive : \e[0;37mRecursive level \e[0m(\e[1;34minteger\e[0m)\n"
		"  \e[0;36m-a / --all       : \e[0;37mSearch in all directories and all files \e[0m(\e[1;34mboolean\e[0m)\n"
		"  \e[0;36m-w / --write     : \e[0;37mReplacement (%% = joker) \e[0m(\e[1;34mstring\e[0m)\n"
		"  \e[0;36m-s / --start     : \e[0;37mHilighting/replacement start \e[0m(\e[1;34mint\e[0m)\n"
		"  \e[0;36m-e / --end       : \e[0;37mHilighting/replacement end \e[0m(\e[1;34mint\e[0m)\n\n",
		script
	);
}

void Free_Arguments(arguments *arg)
{
	if (!arg) return;

	//if (arg->exp) free(arg->exp);
	free(arg->wri);
	if (arg->fil) {
		for (int i = 0; arg->fil[i]; i++) free(arg->fil[i]);
		free(arg->fil);
	}
	free(arg);
}

int Valid_File(const char *filename)
{
	char			*ext = NULL;
	char			**ptr;
	static char		*ext_list[] = 
	{
		"jpg","jpeg","bmp","gif","png","tiff","tif",
		"mp3", "wma", "wave", "riff", "aiff", "raw", "ogg",
		"mp4", "avi", "wmv", "mov", "flv", "mpg", "rmvb",
		"odt", "pdf"
		"iso",
		"o", "a", "dll", "so", "exe", "app",
		"tar", "gz",
		NULL
	};

	if (!is_readable(filename)) return 0;
	if ((ext = Get_tolower_extension(filename)) != NULL)
	{
		ptr = ext_list;
		while (*ptr)
		{
			if (my_strcmp(*ptr, ext) == 0) goto false_label;
			ptr++;
		}

		free(ext);
		return (1);
	}
	return (1);

false_label:
	free(ext);
	return (0);
}

/*
** fonction à recoder pour éliminer uniquement les fichiers binaires
*/
int is_readable(const char *filename)
{
	FILE *file;
	char tmp[21];
	short i, len;

	if ((file = fopen(filename, "r")) == NULL) return 0;
	if ((len = fread(tmp, sizeof(char), 21, file)) < 1) {
		fclose(file);
		return 0;
	}
	fclose(file);

	for (i = 0; i < (len - 1); i++) 
		if ((tmp[i] != '\n' && tmp[i] != '\t') && tmp[i] < 32 && tmp[i + 1] < 32) return 0;

	return 1;
}

char *Get_tolower_extension(const char *filename)
{
	char *ret;
	short lastpoint, i;

	for (i = 0, lastpoint = 0; filename[i]; i++)
	{
		if (filename[i] == '.') lastpoint = i;
	}
	if (lastpoint == 0) return NULL;

	lastpoint++;
	ret = calloc(strlen(filename + lastpoint) + 1, sizeof(char));
	for (i = 0, filename += lastpoint; *filename; filename++, i++)
	{
		ret[i] = (*filename >= 65 && *filename <= 90) ? (*filename) + 32: *filename;
	}
	ret[i] = '\0';

	return ret;
}

int Authorized_File(char **filter, char *filename)
{
	if (filter == NULL) return 1;

	while (*filter) {
		if (preg_match(*filter, filename) == 0) return 1;
		filter++;
	}
	return 0;
}

void Raw_mode(int on) 
{ 
	static struct termios cooked; 
	static int raw_active = 0; 

	if (raw_active == on) 
		return; 

	if (on) 
	{ 
		struct termios raw; 

		tcgetattr(STDIN_FILENO, &cooked); 

		raw = cooked; 
		cfmakeraw(&raw); 
		tcsetattr(STDIN_FILENO, TCSANOW, &raw); 
	} 
	else 
		tcsetattr(STDIN_FILENO, TCSANOW, &cooked); 

	raw_active = on; 
}
