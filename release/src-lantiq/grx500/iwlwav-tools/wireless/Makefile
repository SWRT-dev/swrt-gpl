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
override CFLAGS += -O2 -Wno-multichar
override CFLAGS += -I$(builddir)
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(builddir)/shared/
override CFLAGS += -I$(builddir)/shared_iwlwav-dev/
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/wireless/shared_mbss_mac/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLINUX -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT
ARFLAGS = cru

all: libmtlkwls.a

objs =  $(builddir)/shared_iwlwav-dev/mem_leak.o \
	$(builddir)/shared_iwlwav-dev/mtlk_algorithms.o \
	$(builddir)/shared_iwlwav-dev/mtlk_objpool.o \
	$(builddir)/shared_iwlwav-dev/mtlkstartup.o \
	$(builddir)/shared_iwlwav-dev/mtlkerr.o \
	$(builddir)/shared_iwlwav-dev/mtlkirbhash.o \
	$(builddir)/shared_iwlwav-dev/wave_dataex.o \
	$(abs_top)/tools/shared/linux/osal_osdep.o

# Based on generated logmacros.c file and therefore should be compiled last
logmdb-obj	:= $(builddir)/shared_iwlwav-dev/logmacro_database.o
$(logmdb-obj)	:  $(objs) # depends on all of the above objs
objs		+= $(logmdb-obj)

libmtlkwls_AR = $(AR) $(ARFLAGS)

libmtlkwls.a: $(objs)
	$(libmtlkwls_AR) libmtlkwls.a $(objs)
	$(RANLIB) libmtlkwls.a

clean:
	rm -f $(objs) libmtlkwls.a

export logger_origin_name = libmtlkwls
export logger_binary_name = libmtlkwls.a
export logger_origin_id = 5
export abs_builddir = $(builddir)
