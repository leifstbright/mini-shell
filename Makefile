
CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lreadline

all: ssi

ssi: ssi.c
	$(CC) $(CFLAGS) ssi.c -o ssi $(LIBS)


clean:
	rm -f ssi *.o self-marking.log