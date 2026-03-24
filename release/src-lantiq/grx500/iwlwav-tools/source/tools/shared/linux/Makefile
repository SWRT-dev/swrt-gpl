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
override CFLAGS += -Wall -Wno-multichar -fno-strict-aliasing -Werror -Wno-error=undef -O2
override CFLAGS += -I$(builddir)/.
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/include/config/
override CFLAGS += -I$(abs_top)/wireless/shared_iwlwav-dev/
override CFLAGS += -I$(abs_top)/tools/shared/3rd_party/iniparser/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT

ARFLAGS = cru

all: libmtlkc.a

objs = mtlkirba.o mtlk_assert.o mtlknlink.o osal_osdep.o utils.o mtlksighandler.o \
		mtlkirbhash.o \
		$(abs_top)/tools/shared/mtlk_pathutils.o \
		$(abs_top)/tools/shared/mtlkcontainer.o \
		$(abs_top)/tools/shared/argv_parser.o \
		log_osdep.o mtlk_rtlog_app.o \

# Based on generated logmacros.c file and therefore should be compiled last
logmdb-obj	:= logmacro_database.o
$(logmdb-obj)	:  $(objs) # depends on all of the above objs
objs		+= $(logmdb-obj)

libmtlkc_AR = $(AR) $(ARFLAGS)

libmtlkc.a: $(objs)
	$(libmtlkc_AR) libmtlkc.a $(objs)
	$(RANLIB) libmtlkc.a

clean:
	rm -f $(objs) libmtlkc.a

export abs_builddir = $(builddir)
export logger_origin_name = libmtlkc
export logger_binary_name = libmtlkc.a
export logger_origin_id = 4
