CC = gcc
CFLAGS = -Wall -Wextra
SOURCES = main.c args.c cqueue.c messages.c read_data.c introsort.c aho_corasick.c

all:
	$(CC) $(CFLAGS) -o main $(SOURCES) -lm

debug:
	$(CC) $(CFLAGS) -DDEBUG -o main $(SOURCES) logfile.c -lm

clean:
	rm -f main logfile
