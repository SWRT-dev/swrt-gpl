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
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/tools/shared/3rd_party/iniparser/
override CFLAGS += -I$(abs_top)/wireless/shared_iwlwav-dev/
override CFLAGS += -I$(abs_top)/include/config/
override CFLAGS += -I$(abs_top)/wireless/shared_mbss_mac/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLINUX  -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT
LINK = $(LDFLAGS) $(AM_CFLAGS) $(CFLAGS) -o $@

all: drvhlpr

objs = fshlpr.o drvhlpr.o driver_api.o

# Based on generated logmacros.c file and therefore should be compiled last
logmdb-obj	:= logmacro_database.o
$(logmdb-obj)	:  $(objs) # depends on all of the above objs
objs		+= $(logmdb-obj)

deps =  $(abs_top)/tools/shared/linux/libmtlkc.a \
	$(abs_top)/wireless/libmtlkwls.a \
	$(abs_top)/tools/shared/3rd_party/iniparser/libiniparser.a

drvhlpr_LDADD = $(abs_top)/tools/shared/linux/libmtlkc.a \
		$(abs_top)/wireless/libmtlkwls.a \
		$(abs_top)/tools/shared/3rd_party/iniparser/libiniparser.a

drvhlpr: $(objs) $(deps)
	$(CC) $(LINK) $(objs) $(drvhlpr_LDADD)

clean:
	rm -f $(objs) drvhlpr 

export logger_origin_name = drvhlpr
export logger_binary_name = drvhlpr.o
export logger_origin_id = 3
export abs_builddir = $(builddir)
