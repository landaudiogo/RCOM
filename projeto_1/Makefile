all: writer reader

writer: src/writer.c shared.o
	gcc -o exec/writer src/writer.c shared.o funcs.o

reader: src/reader.c shared.o
	gcc -o exec/reader src/reader.c shared.o funcs.o

shared.o: src/shared.c src/funcs.c
	gcc -c src/shared.c src/funcs.c
