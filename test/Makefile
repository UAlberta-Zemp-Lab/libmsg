CFLAGS = -O2 -ggdb -Wall -Werror
INCS = -I ../include

MSGOBJ = ../src/msg/msg.o

all: tests

.c.o:
	$(CC) $(CFLAGS) $(INCS) -o $@ -c $<

tests: unistd

unistd: $(MSGOBJ) utils.o unistd-test.o ../src/unistd/unistd.o
	$(CC) $(CFLAGS) $(INCS) $(MSGOBJ) utils.o unistd-test.o ../src/unistd/unistd.o -o $@

clean:
	rm *.o $(MSGOBJ) ../src/unistd/unistd.o
