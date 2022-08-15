
CC=gcc
CFLAGS=-std=c99 -Wall -pedantic -g3
LDFLAGS=`./libs/xml2-config --cflags`


all: csscheck

csscheck: csscheck.o storepaths.o corefuns.o 
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ csscheck.o storepaths.o corefuns.o `./libs/xml2-config --libs`

csscheck.o: src/csscheck.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c src/csscheck.c  `./libs/xml2-config --libs`
storepaths.o: src/storepaths.c
	$(CC) $(CFLAGS) -o $@ -c src/storepaths.c
corefuns.o: src/corefuns.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ -c src/corefuns.c `./libs/xml2-config --libs`


clean:
	rm -f csscheck *.o
