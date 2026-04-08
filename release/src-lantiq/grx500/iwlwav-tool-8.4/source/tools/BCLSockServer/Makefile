builddir = .

override CC := $(abs_top)/support/mtlkfront.sh $(CC)
override AM_CFLAGS = -Wall -Wno-multichar -fno-strict-aliasing -Werror -Wno-error=undef -fno-common -fno-builtin
override CFLAGS += -O2

override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(builddir)/.
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/wireless/shared/
override CFLAGS += -I$(abs_top)/include/config/
override CFLAGS += -I$(abs_top)/tools/rtlogger/shared/
override CFLAGS += -I$(abs_top)/tools/BCLSockServer/BCL
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLINUX -DHAVE_CONFIG_H -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT
override CFLAGS += -include $(abs_top)/include/config/config.h
LINK = $(LDFLAGS) $(AM_CFLAGS) $(CFLAGS) -o $@

objs = BCLSockServer.o BCL/mt_bcl.o BCL/mt_bcl_funcs.o\
	BCL/mt_lchacc.o mt_tftp.o BCL/mt_util.o BCL/mt_wapi.o\
	$(abs_top)/wireless/shared/mtlk_algorithms.o\
	logmacro_database.o

deps = $(abs_top)/tools/shared/linux/libmtlkc.a

BclSockServer_LDADD = $(abs_top)/tools/shared/linux/libmtlkc.a

BCLSockServer: $(objs) $(deps)
	$(CC) $(LINK) $(objs) $(BclSockServer_LDADD)

all: BCLSockServer

clean:
	rm -f $(objs) BclSockServer

export logger_origin_name = BclSockServer
export logger_binary_name = mtlk.o
export logger_origin_id = 6
