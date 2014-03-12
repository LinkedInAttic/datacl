This directory is used to machine generate source code using templates

tgen.csv is a 3 column file that looks like 

tmpl_foo.c,A000,repl1.csv

This means that you create A000_foo.c by starting with tmpl_foo.c and
performing substitutions with label A000 specified in repl1.csv

