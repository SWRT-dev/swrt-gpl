builddir = .

override CC := $(abs_top)/support/mtlkfront.sh $(CC)
override CFLAGS += -O2 -Wno-multichar
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(builddir)/.
override CFLAGS += -I$(builddir)/linux/
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/tools/mtidl/
override CFLAGS += -I$(abs_top)/wireless/shared/
override CFLAGS += -I$(abs_top)/include/config/
override CFLAGS += -I$(abs_top)/tools/rtlogger/shared/
override CFLAGS += -I$(abs_top)/wireless/shared_mbss_mac/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT
ARFLAGS = cru

objs = driver_api.o mtlk_cli_client.o mtlk_cli_server.o linux/mtlk_cli_port.o \
		logmacro_database.o

		
libmttools_AR = $(AR) $(ARFLAGS)

libmttools.a: $(objs)
	$(libmttools_AR) libmttools.a $(objs)
	$(RANLIB) libmttools.a

all: libmttools.a

clean:
	rm -f $(objs) libmttools.a

export logger_origin_name = libmttools
export logger_binary_name = libmttools.a
export logger_origin_id = 6
