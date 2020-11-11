SHELL=/bin/bash


all: writer reader

writer: src/writer.c lib
	gcc src/writer.c -o exec/writer  shared.o funcs.o

reader: src/reader.c lib
	gcc -ggdb3 src/reader.c -o exec/reader shared.o funcs.o

lib: src/shared.c src/funcs.c
	gcc -c src/{shared.c,funcs.c}
