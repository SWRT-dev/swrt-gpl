# ******************************************************************************** #
#       Copyright (C) 2018 Intel Corporation					   #
#       Copyright (C) 2017-2018 Intel Corporation                                  #
#       Lantiq Beteiligungs-GmbH & Co. KG                                          #
#       Lilienthalstrasse 15, 85579 Neubiberg, Germany                             #
#       For licensing information, see the file 'LICENSE' in the root folder of    #
#        this software module.                                                     #
# *******************************************************************************  #

ifeq ($(wildcard $(SRCBASE)/router/libhelper-2.3.0.16/*.c),)
all:
	-cp -f prebuilt/libhelper.so ./

install:

clean:
else
PKG_NAME:= libhelper

CFLAGS := $(strip $(subst -DPACKAGE_ID=\"libhelper\",,$(CFLAGS))) -DPACKAGE_ID=\"libhelper\"
CFLAGS := $(strip $(subst -DLOGGING_ID="libhelper",,$(CFLAGS))) -DLOGGING_ID="libhelper"

opt_no_flags := -Wcast-qual

bins := libhelper.so 

libhelper.so_sources := $(wildcard *.c)
libhelper.so_cflags := -I./include/ 
libhelper.so_ldflags := -lsafec-3.3 
	
include make.inc
endif
