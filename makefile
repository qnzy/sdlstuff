CC = gcc
CFLAS = -og
LDFLAGS = -lm -lSDL

SRCS = $(wildcard *.c)
PROGS = $(patsubst %.c,%,$(SRCS))
all: $(PROGS)

%: %.c
	$(CC) $(CFLAGS) $@.c $(LDFLAGS) -o $@

clean:
	rm -f $(PROGS)
