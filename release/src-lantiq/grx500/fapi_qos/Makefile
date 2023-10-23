# ******************************************************************************** #
#       Copyright (c) 2015                                                         #
#       Lantiq Beteiligungs-GmbH & Co. KG                                          #
#       Lilienthalstrasse 15, 85579 Neubiberg, Germany                             #
#       For licensing information, see the file 'LICENSE' in the root folder of    #
#        this software module.                                                     #
# *******************************************************************************  #

PKG_NAME:=ltq_qosal

override LDFLAGS += -Lengines/cpu_n -Lengines/ppa -Lfapi/ -Lmisc/ -Lipc/ -Lutils/ -L$(STAGING_DIR)/usr/lib/ -lsafec-1.0
override CFLAGS += -I$(QOSAL_DIR)/share -I$(QOSAL_DIR)/include -I$(QOSAL_DIR)/misc -I$(STAGING_DIR)/usr/include/ -I$(QOSAL_DIR)/engines/cpu_n

bins:= \
	engines/cpu_n/libcpu_n.so \
	$(if $(PLATFORM_XRX500),engines/ppa/libppa.so) \
	$(if $(PLATFORM_LGM),engines/ppa/libppa.so) \
	$(if $(PLATFORM_PUMA),engines/ppa/libppa.so) \
	ipc/libqosipc.so \
	fapi/libqosfapi.so \
	misc/libsysapi.so \
	utils/libutils.so \
	qosd/qosd

libcpu_n.so_sources := $(wildcard engines/cpu_n/*.c)
ifdef PLATFORM_XRX500
  libppa.so_sources := $(wildcard engines/ppa/*.c)
endif
ifdef PLATFORM_LGM
  libppa.so_sources := $(wildcard engines/ppa/*.c)
endif
ifdef PLATFORM_PUMA
  libppa.so_sources := engines/ppa/init.c
  libppa.so_sources += engines/ppa/queue.c
endif
libqosipc.so_sources := $(wildcard ipc/ipc*.c)
libqosfapi.so_sources := $(wildcard fapi/*.c)
libsysapi.so_sources := $(wildcard misc/*.c)
libutils.so_sources := $(wildcard utils/*.c)
qosd_sources := $(wildcard qosd/*.c)

libppa.so_cflags := -DCONFIG_LTQ_PPA_IPv6_ENABLE -DCONFIG_LTQ_PPA_QOS -DCONFIG_LTQ_PPA_QOS_WFQ -DCONFIG_LTQ_PPA_QOS_RATE_SHAPING -DCONFIG_LTQ_PPA_API_SW_FASTPATH -DCONFIG_LTQ_PPA_HANDLE_CONNTRACK_SESSIONS
ifdef PLATFORM_XRX500
libppa.so_cflags += -DCONFIG_LTQ_PPA_GRX500
endif
ifdef PLATFORM_PUMA
libppa.so_cflags += -DCONFIG_LTQ_PPA_PUMA7
libppa.so_cflags += -DCONFIG_PPA_PUMA7
endif
libqosfapi.so_ldflags := -lqosipc -lhelper
qosd_ldflags := -lcpu_n $(if $(PLATFORM_XRX500),-lppa) -lqosipc -lsysapi -lutils -lezxml -ldl -lhelper $(if $(PLATFORM_PUMA),-lppa) $(if $(PLATFORM_LGM),-lppa) -lubus -lblobmsg_json -lubox

include make.inc
