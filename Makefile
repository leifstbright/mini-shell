
CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lreadline

all: ssi

ssi: ssi.c
	$(CC) $(CFLAGS) ssi.c -o ssi $(LIBS)

test: ssi
	chmod +x marking-linux-amd64
	./marking-linux-amd64

clean:
	rm -f ssi *.o self-marking.log