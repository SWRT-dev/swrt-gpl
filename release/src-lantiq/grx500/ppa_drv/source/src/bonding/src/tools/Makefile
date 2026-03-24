
###############################################################################
#         Copyright (c) 2020, MaxLinear, Inc.
#         Copyright 2016 - 2020 Intel Corporation
# 
#  For licensing information, see the file 'LICENSE' in the root folder of
#  this software module.
# 
###############################################################################

PROGRAMS:=atm_bond_cli

atm_bond_cli:=atm_bond_cli.o

%.o:%.c
	$(CC) -c -o $@ $< $(CFLAGS)

atm_bond_cli: $(atm_bond_cli)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

all: $(PROGRAMS)

clean:
	rm -f *.o $(PROGRAMS)
