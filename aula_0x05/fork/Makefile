.PHONY=all clean

CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c11
LDFLAGS=

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
EXES=$(SRCS:%.c=%)

all: $(EXES)

clean:
	@rm -f $(OBJS)
	@rm -f $(EXES)

%: %.o
	$(CC) $(LDFLAGS) $< -o $@ 

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

# by Heredia, Eduardo (2020)