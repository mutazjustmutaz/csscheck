
CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g3
LDFLAGS=-I./myxml/xmlinst/include/libxml2 -L./myxml/xmlinst/lib

all: csscheck

csscheck: csscheck.o storepaths.o corefuns.o 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ csscheck.o storepaths.o corefuns.o -lxml2

csscheck.o: src/csscheck.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c src/csscheck.c
storepaths.o: src/storepaths.c
	$(CC) $(CFLAGS) -o $@ -c src/storepaths.c
corefuns.o: src/corefuns.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c src/corefuns.c 


clean:
	rm -f csscheck *.o