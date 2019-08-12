


CC = gcc
CFLAGS  = -g -Wall

default: main

main: main.c parser.o commands.o
	$(CC) $(CFLAGS) -o main main.c commands.o parser.o 

parser.o: parser.c parser.h commands.h callbacks.h
	$(CC) $(CFLAGS) -c parser.c

commands.o: commands.c parser.h commands.h callbacks.h
	$(CC) $(CFLAGS) -c commands.c

clean: rm *.o main
