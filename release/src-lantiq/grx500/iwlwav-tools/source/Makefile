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

export abs_top = $(shell pwd)
export abs_top_srcdir = $(abs_top)
export abs_top_builddir = $(abs_top_srcdir)
export abs_logprep_srcdir = $(abs_top_srcdir)/wireless/shared_iwlwav-dev
export STAGING_DIR = 

CFLAGS += -Werror=format-security -Wall -Wl,-strip-debug
CFLAGS += `pkg-config --cflags libnl-3.0` -Wno-unused-result
LDFLAGS += `pkg-config libnl-3.0 libnl-genl-3.0 --libs` -lpthread

all: dutserver dump_handler logserver drvhlpr BclSockServer whm_handler;

clean:
	$(MAKE) -C tools/shared/linux/               clean
	$(MAKE) -C wireless/                         clean
	$(MAKE) -C tools/shared/3rd_party/iniparser/ clean
	$(MAKE) -C tools/rtlogger/logserver/linux/   clean
	$(MAKE) -C tools/dutserver/linux/            clean
	$(MAKE) -C tools/drvhlpr/                    clean
	$(MAKE) -C tools/dump_handler/               clean
	$(MAKE) -C tools/whm_handler/                clean
	$(MAKE) -C tools/BCLSockServer/              clean
	find $(abs_top) -name "logmacros.*"   | xargs rm -f
	find $(abs_top) -name "Makefile.am"   | xargs rm -f
	find $(abs_top) -name "Makefile.in"   | xargs rm -f
	find $(abs_top) -name "*.scd"         | xargs rm -f
	find $(abs_top) -name "*.o.cmd"       | xargs rm -f 
	find $(abs_top) -name "*.stderr"      | xargs rm -f
	find $(abs_top) -name "*.pipe.result" | xargs rm -f
	find $(abs_top) -name ".logprep_ts"   | xargs rm -rf

libmtlkc:
	$(MAKE) -C tools/shared/linux/

libmtlkwls:
	$(MAKE) -C wireless/

libiniparser:
	$(MAKE) -C tools/shared/3rd_party/iniparser/

dutserver: libmtlkc libmtlkwls libiniparser
	$(MAKE) -C tools/dutserver/linux/

logserver: libmtlkc libmtlkwls
	$(MAKE) -C tools/rtlogger/logserver/linux/

drvhlpr: libmtlkc libmtlkwls libiniparser
	$(MAKE) -C tools/drvhlpr/

dump_handler: libmtlkc libmtlkwls
	$(MAKE) -C tools/dump_handler/

whm_handler: libmtlkc libmtlkwls
	$(MAKE) -C tools/whm_handler/

BclSockServer: libmtlkc
	$(MAKE) -C tools/BCLSockServer

release: dutserver dump_handler drvhlpr whm_handler

debug: all
