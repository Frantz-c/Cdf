# Cdf
Search with regex into files. (Cdf = Chercher Dans les Fichiers)
You can also replace matches.

compilation: make
show usage: ./cdf

example:
	./cdf 'return ((-?[0-9]+)|(NULL));'
		-> match all "return x;" pattern
	./cdf 'return ((-?[0-9]+)|(NULL));' -w='return (%);'
	./cdf 'return ((-?[0-9]+)|(NULL));' -w='return (%);' --no-confirm
		-> replace all "return x;" pattern with "return (x);"
