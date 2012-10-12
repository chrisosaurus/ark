VERSION = 0.1

PREFIX = /usr/local
#MANPREFIX = ${PREFIX}/share/man
MANPREFIX = /usr/man

INCS = 
LIBS = -lX11

CFLAGS = -std=c99 -pedantic -Wall ${INCS}
LDFLAGS = ${LIBS}

CC = cc
