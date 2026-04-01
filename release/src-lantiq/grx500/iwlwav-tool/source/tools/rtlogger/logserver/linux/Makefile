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
override AM_CFLAGS = -Wall -Wno-multichar -fno-strict-aliasing -Werror -Wno-error=undef
override CFLAGS += -O2 -Wno-multichar
override CFLAGS += -I$(builddir)/.
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(abs_top)/tools/shared
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/wireless/shared_iwlwav-dev/
override CFLAGS += -I$(abs_top)/include/config/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT

all: logserver

LD_LIBS =	-L$(abs_top)/tools/shared/linux/ -lmtlkc \
		-L$(abs_top)/wireless/ -lmtlkwls

LINK = $(LDFLAGS) $(LD_LIBS) $(AM_CFLAGS) $(CFLAGS) -o $@

objs =  logserver.o db.o net.o cqueue.o proto_drv.o proto_lg.o logsrv_utils.o \

# Based on generated logmacros.c file and therefore should be compiled last
logmdb-obj	:= logmacro_database.o
$(logmdb-obj)	:  $(objs) # depends on all of the above objs
objs		+= $(logmdb-obj)

deps = $(abs_top)/tools/shared/linux/libmtlkc.a \
	$(abs_top)/wireless/libmtlkwls.a

logserver_LDADD = $(abs_top)/tools/shared/linux/libmtlkc.a \
		$(abs_top)/wireless/libmtlkwls.a

logserver: $(objs) $(deps)
	$(CC) $(LINK) $(objs) $(logserver_LDADD)

clean:
	rm -f $(objs) logserver 

export logger_origin_name = logserver
export logger_binary_name = $(logger_origin_name)
export logger_origin_id = 11
export abs_builddir = $(builddir)
