.POSIX:

include config.mk

OBJ =\
	src/msg/msg.o \
	src/unistd/unistd.o \

.c.o:
	$(CC) $(CFLAGS) -I include -c $< -o $@

all: $(ANAME)

$(ANAME): $(OBJ)
	$(AR) crs $@ $^

test: $(ANAME)
	$(MAKE) -C test

install:
	mkdir -p $(INCPREFIX)
	install -m 0644 libmsg.a $(DESTDIR)$(LIBPREFIX)
	install -m 0644 include/msg/*.h $(DESTDIR)$(INCPREFIX)

uninstall:
	rm -rf $(DESTDIR)$(INCDIR)
	$(MAKE) -C src uninstall

clean:
	rm -f $(ANAME) $(OBJ)
	$(MAKE) -C test clean

.PHONY: all clean install uninstall lib test
