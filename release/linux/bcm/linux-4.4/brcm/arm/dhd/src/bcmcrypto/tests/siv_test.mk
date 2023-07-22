#
# Assorted cryptographic algorithms
#
# Copyright (C) 2012 Broadcom Corporation
#
# $Id $
#

SRCBASE	:= ../..
VPATH=.:$(SRCBASE)/bcmcrypto:$(SRCBASE)/shared

CC	:= gcc
CFLAGS	+= -I. -I$(SRCBASE)/include -I$(SRCBASE)/../components/shared -Wall
CFLAGS	+= -g -DDEBUG -ffunction-sections
CFLAGS	+= -Wall -Werror

PROGS	:= siv_test

all: $(PROGS)

clean:
	rm -f *.o *.obj $(PROGS) *.exe

CRYPTO_OBJS = aes.o rijndael-alg-fst.o sha256.o
UTILS_OBJS = bcmutils.o

OBJS = siv_test.o siv.o aessiv.o $(CRYPTO_OBJS) $(UTILS_OBJS)

$(PROGS):	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): $(@:.o=.c)
