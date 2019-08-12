


CC = gcc
CFLAGS  = -g -Wall

default: main

main: main.c parser.o commands.o callbacks.o
	$(CC) $(CFLAGS) -o main main.c commands.o parser.o callbacks.o

parser.o: parser.c parser.h commands.h callbacks.h
	$(CC) $(CFLAGS) -c parser.c

commands.o: commands.c parser.h commands.h callbacks.h
	$(CC) $(CFLAGS) -c commands.c

callbacks.o: callbacks.c callbacks.h
	$(CC) $(CFLAGS) -c callbacks.c


clean: rm *.o main
