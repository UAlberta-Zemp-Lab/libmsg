.POSIX:

include ../config.mk

CFLAGS_ALL = $(CFLAGS) -ggdb

INCS = -I ../include
LIBS = -L ../src -lmsg

all: tests

.c.o:
	$(CC) $(CFLAGS_ALL) $(INCS) -o $@ -c $<

tests: unistd

unistd: utils.o unistd-test.o
	$(CC) $(CFLAGS_ALL) $(INCS) utils.o unistd-test.o $(LIBS) -o $@
	./$@

clean:
	rm -f *.o unistd

.PHONY: all clean tests
