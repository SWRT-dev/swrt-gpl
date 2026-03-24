#/******************************************************************************
#
#         Copyright (c) 2020, MaxLinear, Inc.
#         Copyright 2016 - 2020 Intel Corporation
#         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
#         Copyright 2009 - 2014 Lantiq Deutschland GmbH
#         Copyright 2007 - 2008 Infineon Technologies AG
#
#  For licensing information, see the file 'LICENSE' in the root folder of
#  this software module.
#
#*******************************************************************************/

builddir = .

override CC := $(abs_top)/support/mtlkfront.sh $(CC)
override AM_CFLAGS = -Wall -Wno-multichar -fno-strict-aliasing -Werror -Wno-error=undef -fno-common -fno-builtin
override CFLAGS += -O2

override CFLAGS += -I$(builddir)/.
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/wireless/shared_iwlwav-dev/
override CFLAGS += -I$(abs_top)/include/config/
override CFLAGS += -I$(abs_top)/tools/BCLSockServer/BCL
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLINUX -DHAVE_CONFIG_H -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT
override CFLAGS += -include $(abs_top)/include/config/config.h
LINK = $(LDFLAGS) $(AM_CFLAGS) $(CFLAGS) -o $@

all: BCLSockServer

objs = BCLSockServer.o BCL/mt_bcl.o BCL/mt_bcl_funcs.o \
	BCL/mt_lchacc.o mt_tftp.o BCL/mt_util.o BCL/mt_wapi.o \
	mtlk_algorithms.o

# Based on generated logmacros.c file and therefore should be compiled last
logmdb-obj	:= logmacro_database.o
$(logmdb-obj)	:  $(objs) # depends on all of the above objs
objs		+= $(logmdb-obj)

deps = $(abs_top)/tools/shared/linux/libmtlkc.a

BclSockServer_LDADD = $(abs_top)/tools/shared/linux/libmtlkc.a

BCLSockServer: $(objs) $(deps)
	$(CC) $(LINK) $(objs) $(BclSockServer_LDADD)

clean:
	rm -f $(objs) BCLSockServer

export logger_origin_name = BclSockServer
export logger_binary_name = $(logger_origin_name)
export logger_origin_id = 15
export abs_builddir = $(builddir)
