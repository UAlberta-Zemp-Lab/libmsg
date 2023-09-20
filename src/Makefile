.POSIX:

include ../config.mk

INCS = -I ../include

OBJ = msg/msg.o unistd/unistd.o

all: libmsg.a

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

libmsg.a: $(OBJ)
	$(AR) crs $@ $^

clean:
	rm -f libmsg.a $(OBJ)

install:
	mkdir -p $(DESTDIR)$(LIBDIR)
	install -m 0644 libmsg.a $(DESTDIR)$(LIBDIR)

uninstall:
	rm -f $(LIBDIR)/libmsg.a

.PHONY: all clean install uninstall
