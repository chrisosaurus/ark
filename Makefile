# ark - graphical text editor
# See LICENSE file for copyright and license details.

.POSIX:

include config.mk

SRC = llist.c ui.c ark.c
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

${OBJ}: config.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

ark: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${LDFLAGS} ${OBJ}

clean:
	@echo cleaning
	@rm -f ark ${OBJ} ark-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p ark-${VERSION}
	@cp -R LICENSE Makefile config.mk config.def.h \
		README TODO ark.1 codes.h ${SRC} ark-${VERSION}
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

.PHONY: all options clean dist install uninstall
