.POSIX:

include config.mk

all: lib

lib:
	$(MAKE) -C src

test: lib
	$(MAKE) -C test

install:
	mkdir -p $(INCDIR)
	install -m 0644 include/msg/*.h $(DESTDIR)$(INCDIR)/
	$(MAKE) -C src install

uninstall:
	rm -rf $(DESTDIR)$(INCDIR)
	$(MAKE) -C src uninstall

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean

.PHONY: all clean install uninstall lib test
