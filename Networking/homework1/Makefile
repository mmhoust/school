CC = gcc
CFLAGS= -g -Wall 
SOURCES = passwordBreaker.c passwordServer.c methods.c
OBJS = passwordBreaker.o passwordServer.o methods.o

.PHONY: depend clean

all:	passwordServer passwordBreaker

passwordServer: $(OBJS)
	$(CC) $(CFLAGS) -o passwordServer passwordServer.o methods.o

passwordBreaker: $(OBJS)
	$(CC) $(CFLAGS) -o passwordBreaker passwordBreaker.o methods.o

passwordServer.o: $(SOURCES)
	$(CC) $(CFLAGS) -c passwordServer.c

passwordBreaker.o: $(SOURCES)
	$(CC) $(CFLAGS) -c passwordBreaker.c

methods.o: $(SOURCES)
	$(CC) $(CFLAGS) -c methods.c

clean:
	$(RM) *.o *~ passwordServer passwordBreaker

depend: $(SRCS)
	makedepend $(CFLAGS)  $(SOURCES)


