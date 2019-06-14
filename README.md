# Cdf
Search with regex into files. (Cdf = Chercher Dans les Fichiers)
You can replace a pattern in all files.


search example:  ./cdf 'printf\(' -e=1

replace example: ./cdf 'printf\(' -e=1 -w='my_printf'

compilation:  make

show usage: ./cdf

/!\ replacement system is not completely finished /!\
