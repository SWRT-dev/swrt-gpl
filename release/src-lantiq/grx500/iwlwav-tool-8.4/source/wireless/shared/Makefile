builddir = .

override CC := $(abs_top)/support/mtlkfront.sh $(CC)
override CFLAGS += -O2 -Wno-multichar
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(builddir)/.
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/tools/shared/linux/3rd_party/
override CFLAGS += -I$(abs_top)/tools/mttools/shared
override CFLAGS += -I$(abs_top)/tools/mttools/shared/linux/
override CFLAGS += -I$(abs_top)/tools/rtlogger/shared/
override CFLAGS += -I$(abs_top)/tools/mtidl/
override CFLAGS += -I$(abs_top)/wireless/shared_mbss_mac/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLINUX -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT
ARFLAGS = cru

objs = mem_leak.o mtlk_algorithms.o mtlk_objpool.o mtlkstartup.o mtlkerr.o mtlkstrtok.o \
		$(abs_top)/tools/shared/mtlkirbhash.o pattern.o \
		logmacro_database.o $(abs_top)/tools/shared/linux/osal_osdep.o

libmtlkwls_AR = $(AR) $(ARFLAGS)

libmtlkwls.a: $(objs)
	$(libmtlkwls_AR) libmtlkwls.a $(objs)
	$(RANLIB) libmtlkwls.a

all: libmtlkwls.a

clean:
	rm -f $(objs) libmtlkwls.a

export logger_origin_name = libmtlkwls
export logger_binary_name = libmtlkwls.a
export logger_origin_id = 5
