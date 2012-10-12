VERSION = 0.1

PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

INCS = 
LIBS = -lX11

CFLAGS = -std=c99 -pedantic -Wall ${INCS}
LDFLAGS = ${LIBS}

CC = cc
