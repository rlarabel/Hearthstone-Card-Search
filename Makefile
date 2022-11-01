all: parser search

parser: parser.c
	gcc -g -std=gnu11 -Werror -Wall parser.c -o parser

search: search.c
	gcc -g -std=gnu11 -Werror -Wall search.c -o search
