#!/usr/bin/make -f

PROG=		ztest
SRCS=		ztest.c
OBJS=		ztest.o

CC?=		cc

CPPFLAGS_STD?=	-D_POSIX_SOURCE=200809L -D_XOPEN_SOURCE=700

CPPFLAGS+=	${CPPFLAGS_STD}

CFLAGS_STD?=	-std=c99 -pedantic
CFLAGS_OPT?=	-g -O -pipe
CFLAGS_WARN?=	-Wall -W -Wextra
CFLAGS_ZSTD?=	$(shell pkg-config --cflags libzstd)

CFLAGS?=	${CFLAGS_OPT}
CFLAGS+=	${CFLAGS_STD}

LIBS_ZSTD?=	$(shell pkg-config --libs libzstd)

LDFLAGS?=

LIBS?=
LIBS+=		${LIBS_ZSTD}

RM?=		rm -f --

all:		${PROG}

${PROG}:	${OBJS}
		${CC} ${LDFLAGS} -o ${PROG} ${OBJS} ${LIBS}

.c.o:
		${CC} -c ${CPPFLAGS} ${CFLAGS} -o $@ $<

clean:
		${RM} ${PROG} ${OBJS}

test:		all
		./${PROG}

.PHONY:		all test clean
