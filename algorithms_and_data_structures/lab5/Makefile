CC = gcc
CFLAGS = -D_POSIX_C_SOURCE=200809L -Wall -Wextra -Wpedantic -std=c11
SOURCES = main.c args.c messages.c logfile.c problem.c expression.c vfw.c solver.c output.c

all:
	$(CC) $(CFLAGS) -o riccati_solver $(SOURCES) -lm

debug:
	$(CC) $(CFLAGS) -D DEBUG -o riccati_solver $(SOURCES) -lm

clean:
	rm -f riccati_solver logfile *.csv
