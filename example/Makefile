


CC = gcc
CFLAGS  = -g -Wall -I../include
IDIR =../include
SRCDIR =..
ODIR=obj

_DEPS = callbacks.h commands.h parser.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = commands.o parser.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


default: main

main: main.c $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	rm $(ODIR)/*.o
