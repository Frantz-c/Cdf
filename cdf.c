#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <termios.h> 
#include "cliarg.h"
#include "myregex.h"
#include "cdf.h"
#include "my_functions.h"

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

char Set_Arguments(arguments **main_arguments, int argc, char *argv[])
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
		NULL
	};


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
			return -1;
		}
	}

	/* 
	 ** Set arguments
	 */
	if ((argument->dir = STRING(Cliarg_get_copy_value(args, 'd', NULL))) == NULL)
		CLIARG_SET_STR(argument->dir, ".");

	if ((argument->rec = INTEGER(Cliarg_get_copy_value(args, 'r', NULL))) == NULL)
		CLIARG_SET_INT(argument->rec, 1);

	if ((argument->all = INTEGER(Cliarg_get_copy_value(args, 'a', NULL))) == NULL)
		CLIARG_SET_INT(argument->all, 0);

	argument->fil = STRARRAY(Cliarg_get_copy_value(args, 'f', NULL));

	argument->exp = Correct_regex(argv[1]);

	/* 
	 ** Print regex and filters 
	 */
	fmt_print("\e[0;35mRegex = \e[1;35m%s\e[0;35m\n", argument->exp);
	if (argument->fil) {
		for (int i = 0; argument->fil[i]; i++) 
			fmt_print("fichier %d = \e[1;35m'%s'\e[0;35m\n", i, argument->fil[i]);
	}


	User_confirmation();


	*main_arguments = argument;

	Cliarg_free_arguments(&args);
	Cliarg_free_error(&error);
	return 0;
}

char Arguments_number_check(int argc, char *scriptname)
{
	if (argc < 2)
	{
		Help(scriptname);
		return -1;
	}
	else if (argc > 6)
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

void Search(int total[], arguments *arg, char *path, short rec)
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
		if (*dirinfo->d_name == '.' && *arg->all == 0) continue;

		if ((newpath = calloc(length_of(path) + length_of(dirinfo->d_name) + 2, sizeof(char))) == NULL) {
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
			if (rec + 1 == *arg->rec) {
				free(newpath);
				continue;
			}
			Search(total, arg, newpath, rec + 1);
		}
		else
		{
			if (Authorized_File(arg->fil, dirinfo->d_name) && Valid_File(newpath)) {
				Search_On_File(newpath, arg, total);
			}
			free(newpath);
		}
	}
	free(path);
	closedir(directory);
}

char Search_On_File(const char *filename, arguments *arg, int total[])
{
	char		buffer[BUFSIZE];
	int			line = 1;
	FILE		*file;
	myregex_t	*match = NULL;
	char		title = 0;

	if ((file = fopen(filename, "r")) == NULL) {
		fmt_print("\e[0;31mImpossible d'ouvrir le fichier \"%s\" ligne %d.\e[0m\n", filename, __LINE__);
		return -1;
	}

	/*
	 ** Get one line and search regex into
	 */
	while (fgets(buffer, BUFSIZE, file) != NULL)
	{
		if (length_of(buffer) == 1)
			*buffer = '\0';
		else if (buffer[(length_of(buffer) - 1)] == '\n')
			buffer[(length_of(buffer) - 1)] = '\0';

		/*
		 ** Print line with number of line and highlight the match
		 ** If it's the first match, print file name
		 */
		if (preg_match_get_all(arg->exp, buffer, &match) == 0)
		{
			if (title == 0) 
			{
				title = 1;
				fmt_print("\n\e[1m\e[7;34m» %s «\e[0m\n\n", filename);
				total[CDF_FILE]++;
			}
			total[CDF_MATCH]++;

			Print_match_lines(buffer, match, line);
			free_myregex_t(match);
			match = NULL;
		}
		line++;
	}

	fclose(file);
	if (title) print("\e[0m\n"); // If results are printed, close the color
	return 0;
}

void Print_match_lines(char *buffer, myregex_t *match, int line)
{
	int i;
	const char *line_n = my_itoa(line);
	static char *color1	= "\e[0;33m";
	static char *color2	= "\e[0;37m";
	static char *color3	= "\e[1;33m";

	/*
	 ** Print line number
	 */
	nprint(color1, 7);

	for (i = 0; line_n[i]; i++) my_putc(line_n[i]);

	fmt_print(" :%7s ", color2);

	/*
	 ** Print line with colored occurrences
	 */
	for (i = 0; *buffer; i++)
	{
		if (i == match->start)
		{
			nprint(color3, 7);
		}
		else if (i == match->end)
		{
			nprint(color2, 7);
			if (match->next)
				match = match->next;
		}
		my_putc(*(buffer++));
	}
	my_putc('\n');
}

void Help(const char *script)
{
	fmt_print
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
		"  \e[0;36m-a / --all       : \e[0;37mSearch in all directories and all files \e[0m(\e[1;34mboolean\e[0m)\n\n",
		script
	);
}

void Free_Arguments(arguments *arg)
{
	int i;
	if (!arg) return;

	if (arg->exp) free(arg->exp);
	if (arg->rec) free(arg->rec);
	if (arg->all) free(arg->all);
	if (arg->fil) {
		for (i = 0; arg->fil[i]; i++) free(arg->fil[i]);
		free(arg->fil);
	}
	free(arg);
}

char Valid_File(const char *filename)
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
char is_readable(const char *filename)
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
	ret = calloc(length_of(filename + lastpoint) + 1, sizeof(char));
	for (i = 0, filename += lastpoint; *filename; filename++, i++)
	{
		ret[i] = (*filename >= 65 && *filename <= 90) ? (*filename) + 32: *filename;
	}
	ret[i] = '\0';

	return ret;
}

char Authorized_File(char **filter, char *filename)
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

char *Correct_regex(const char *restrict s)
{
	int   ps_count = 0; // .* "PointStar"
	int   ps_pos[20];
	int   pp_count = 0; // .+ "PointPlus"
	int   pp_pos[20];
	char *ret       = NULL;
	int   i, j;
	int   bracket = 0;
	int   parent = 0;
	int   acco = 0;

	if (*s == '\0') {
		ret = calloc(3, sizeof(char));
		ret[0] = '.';
		ret[1] = '*';
		return ret;
	}

	for (i = 0; s[i]; i++)
	{
		if (ps_count == 30 || pp_count == 30) exit(EXIT_FAILURE);

		// décompte des crochets ouverts
		if (s[i] == '[' && (i == 0 || (i == 1 && s[i-1] != '\\') || (i > 1 && (s[i-1] != '\\' || s[i-2] == '\\'))))
			bracket++;
		else if (s[i] == ']' && (i == 0 || (s[i-1] != '\\' || (i > 1 && s[i-2] == '\\'))))
			bracket--;
		// décompte des parenthèses
		if (s[i] == '(' && (i == 0 || (i == 1 && s[i-1] != '\\') || (i > 1 && (s[i-1] != '\\' || s[i-2] == '\\'))))
			parent++;
		else if (s[i] == ')' && (i == 0 || (s[i-1] != '\\' || (i > 1 && s[i-2] == '\\'))))
			parent--;
		// décompte des accolades
		if (s[i] == '{' && (i == 0 || (i == 1 && s[i-1] != '\\') || (i > 1 && (s[i-1] != '\\' || s[i-2] == '\\'))))
			acco++;
		else if (s[i] == '}' && (i == 0 || (s[i-1] != '\\' || (i > 1 && s[i-2] == '\\'))))
			acco--;

		// Récupération position des ".*" & ".+"
		else if (s[i] == '.')
		{
			if ((i > 0 && (s[i-1] == '\\' || s[i-1] == '[')) || s[i+1] == '\0') continue;
			if (s[i+1] == '+' )
				ps_pos[ps_count++] = i++;
			else if (s[i+1] == '*')
				ps_pos[ps_count++] = i++;
		}
	}

	//fmt_print("\n<<< %d ; %d ; %d >>>\n\n", bracket, parent, acco);
	if (bracket != 0 || parent != 0 || acco != 0)
	{
		fprintf(stderr, "\e[1;31mRegex \"\e[0;36m%s\e[1;31m\" invalide !\n", s);
		exit(EXIT_FAILURE);
	}

	ps_pos[ps_count] = -1;
	pp_pos[pp_count] = -1;

	ret = malloc((i + 1 + (ps_count * 2) + (pp_count * 2)) * sizeof(char));

	if (ps_count == 0 && pp_count == 0) {
		strcpy(ret, s);
		ret[i] = '\0';
		return ret;
	}

	for (i = j = ps_count = pp_count = 0; *s; i++, j++, s++)
	{
		// Remplacement ".+" & ".*"
		if (i == ps_pos[ps_count] || i == pp_pos[pp_count]) {
			ret[j++] = '[';
			ret[j++] = '^';
			s++;
			ret[j + 2] = *s;
			(i++ == ps_pos[ps_count]) ? ps_count++: pp_count++;
			s++;
			while (*s == '\\' && s[-1] != '\\') s++, i++;
			ret[j++] = *s;
			ret[j++] = ']';
		}
		else
			ret[j] = *s;
	}
	ret[j] = '\0';

	return ret;
}
