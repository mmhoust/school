CC = gcc
CFLAGS= -g -Wall 
SOURCES = main.c handler.c methods.c
OBJS = main.o handler.o methods.o

.PHONY: depend clean

all: simhttp

simhttp: $(OBJS)
	$(CC) $(CFLAGS) -o simhttp main.o handler.o methods.o

main.o: $(SOURCES)
	$(CC) $(CFLAGS) -c main.c

handler.o: $(SOURCES)
	$(CC) $(CFLAGS) -c handler.c

methods.o: $(SOURCES)
	$(CC) $(CFLAGS) -c methods.c

clean:
	$(RM) *.o *~ simhttp

depend: 
	makedepend -- $(CFLAGS) -- $(SOURCES)
