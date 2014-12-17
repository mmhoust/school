CC = gcc
CFLAGS= -g -Wall 
SOURCES = main.c DNSheader.c methods.c
OBJS = main.o DNSheader.o methods.o

.PHONY: depend clean

all: dnsq

dnsq: $(OBJS)
	$(CC) $(CFLAGS) -o dnsq main.o DNSheader.o methods.o

main.o: $(SOURCES)
	$(CC) $(CFLAGS) -c main.c

DNSheader.o: $(SOURCES)
	$(CC) $(CFLAGS) -c DNSheader.c

methods.o: $(SOURCES)
	$(CC) $(CFLAGS) -c methods.c

clean:
	$(RM) *.o *~ dnsq

depend: 
	makedepend -- $(CFLAGS) -- $(SOURCES)
