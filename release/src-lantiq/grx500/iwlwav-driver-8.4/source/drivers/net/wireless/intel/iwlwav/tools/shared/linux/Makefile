builddir = .

override CC := $(abs_top)/support/mtlkfront.sh $(CC)
override CFLAGS += -Wall -Wno-multichar -fno-strict-aliasing -Werror -Wno-error=undef -O2
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(builddir)/.
override CFLAGS += -I$(builddir)/../
override CFLAGS += -I$(abs_top)/tools/mttools/shared
override CFLAGS += -I$(abs_top)/tools/mttools/shared/linux/
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/rtlogger/shared/
override CFLAGS += -I$(abs_top)/include/config/
override CFLAGS += -I$(abs_top)/wireless/shared/
override CFLAGS += -I$(abs_top)/wireless/shared/linux/
override CFLAGS += -I$(abs_top)/wireless/shared/shared_mbss_mac/
override CFLAGS += -I$(abs_top)/tools/shared/3rd_party/iniparser/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT 

ARFLAGS = cru

objs = mtlkirba.o mtlk_assert.o mtlknlink.o osal_osdep.o utils.o mtlksighandler.o \
		$(abs_top)/tools/shared/mtlkirbhash.o \
		$(abs_top)/tools/shared/mtlk_pathutils.o \
		$(abs_top)/tools/shared/mtlkcontainer.o \
		$(abs_top)/tools/shared/mtlk_socket.o \
		$(abs_top)/tools/shared/argv_parser.o \
		log_osdep.o mtlk_rtlog_app.o \
		logmacro_database.o

libmtlkc_AR = $(AR) $(ARFLAGS)

libmtlkc.a: $(objs)
	$(libmtlkc_AR) libmtlkc.a $(objs)
	$(RANLIB) libmtlkc.a

all: libmtlkc.a

clean:
	rm -f $(objs) libmtlkc.a

export logger_origin_name = libmtlkc
export logger_binary_name = libmtlkc.a
export logger_origin_id = 4
