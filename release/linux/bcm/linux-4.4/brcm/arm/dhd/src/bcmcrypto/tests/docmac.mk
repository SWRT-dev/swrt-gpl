#
# Assorted cryptographic algorithms
#
# Copyright (C) 2012 Broadcom Corporation
#
# $Id: $
#

SRCBASE	:= ../..
VPATH=.:$(SRCBASE)/bcmcrypto:$(SRCBASE)/shared

CC	:= gcc
CFLAGS	+= -I. -I$(SRCBASE)/include -I$(SRCBASE)/../components/shared -Wall
CFLAGS	+= -g -DDEBUG -ffunction-sections
CFLAGS	+= -Wall -Werror

PROGS	:= docmac

all: $(PROGS)

clean:
	rm -f *.o *.obj $(PROGS) *.exe

CRYPTO_OBJS = prf.o hmac.o sha1.o md5.o sha256.o hmac_sha256.o
UTILS_OBJS = bcmutils.o
OBJS = docmac.o $(CRYPTO_OBJS) $(UTILS_OBJS)
docmac:	$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS) -lssl -lcrypto
