#/******************************************************************************
#
#         Copyright (c) 2020, MaxLinear, Inc.
#         Copyright 2016 - 2020 Intel Corporation
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
override CFLAGS += -I$(abs_top)/wireless/shared_iwlwav-dev/
override CFLAGS += -I$(abs_top)/include/config/
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -include $(abs_top)/include/config/config.h
override CFLAGS += -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT

all: whm_handler

LD_LIBS = -L$(abs_top)/tools/shared/linux/ -lmtlkc \
		-L$(abs_top)/wireless/ -lmtlkwls

USE_DWPAL_DAEMON_APIS=1
ifeq ($(USE_DWPAL_DAEMON_APIS),1)
override CFLAGS += -DCONFIG_USE_DWPAL_DAEMON=1
LD_LIBS += -ldwpald_client -lwv_core
endif

LINK = $(LDFLAGS) $(LD_LIBS) $(LD_LIBS) $(AM_CFLAGS) $(CFLAGS) -o $@

deps =  $(abs_top)/tools/shared/linux/libmtlkc.a \
	$(abs_top)/wireless/libmtlkwls.a

whm_handler_LDADD  = $(abs_top)/tools/shared/linux/libmtlkc.a \
		$(abs_top)/wireless/libmtlkwls.a

objs =  $(abs_top)/tools/shared/argv_parser.o \
	whm_handler.o \

# Based on generated logmacros.c file and therefore should be compiled last
logmdb-obj	:= logmacro_database.o
$(logmdb-obj)	:  $(objs) # depends on all of the above objs
objs		+= $(logmdb-obj)

whm_handler: $(objs)
	$(CC) $(LINK) $(objs) $(whm_handler_LDADD)

clean:
	rm -f $(objs) whm_handler 

export logger_origin_name = whm_handler
export logger_binary_name = $(logger_origin_name)
export logger_origin_id = 17
export abs_builddir = $(builddir)
