# ark - graphical text editor
# See LICENSE file for copyright and license details.

.POSIX:

include config.mk

HEADERS = bindings.def.h config.def.h llist.h ui.h ark.h
SRC = llist.c ark.c ui.c main.c
OBJ = ${SRC:.c=.o}

all: options ark 

options:
	@echo ark build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -g -c ${CFLAGS} $<

${OBJ}: bindings.h config.h config.mk

bindings.h:
	@echo creating $@ from bindings.def.h
	@cp bindings.def.h $@

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

ark: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${LDFLAGS} ${OBJ}

test_llist: llist.o
	@echo CC -o tests/test_llist.c
	@${CC} -o tests/test_llist tests/test_llist.c llist.o -lcheck

test: test_llist
	@echo running test_llist
	./tests/test_llist

clean:
	@echo cleaning
	@rm -f ark ${OBJ} ark-${VERSION}.tar.gz
	@rm -f tests/*.o tests/test_llist

dist: clean
	@echo creating dist tarball
	@mkdir -p ark-${VERSION}
	@cp -R LICENSE Makefile config.mk ${HEADERS} \
		${SRC} ark-${VERSION}
	@tar -cf ark-${VERSION}.tar ark-${VERSION}
	@gzip ark-${VERSION}.tar
	@rm -rf ark-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f ark ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/ark
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < ark.1 > ${DESTDIR}${MANPREFIX}/man1/ark.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/ark.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/ark
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/ark.1

.PHONY: all options clean dist install uninstall test_llist test
