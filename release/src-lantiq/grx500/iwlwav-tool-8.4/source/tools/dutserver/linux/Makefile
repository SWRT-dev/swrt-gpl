builddir = .

override CC := $(abs_top)/support/mtlkfront.sh $(CC)
override AM_CFLAGS = -Wall -Wno-multichar -fno-strict-aliasing -Werror -Wno-error=undef
override CFLAGS += -O2 -Wno-multichar
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(builddir)/.
override CFLAGS += -I$(abs_top)/tools/mttools/shared
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/wireless/shared/
override CFLAGS += -I$(abs_top)/include/config/
override CFLAGS += -I$(abs_top)/tools/rtlogger/shared/
override CFLAGS += -I$(abs_top)/tools/shared/3rd_party/iniparser/
override CFLAGS += -I$(abs_top)/tools/mtidl/
override CFLAGS += -I$(abs_top)/wireless/shared_mbss_mac/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT

LD_LIBS =	-L$(abs_top)/tools/mttools/shared/ -lmttools \
		-L$(abs_top)/tools/shared/linux/ -lmtlkc \
		-L$(abs_top)/wireless/shared/ -lmtlkwls \
		-L$(abs_top)/tools/shared/3rd_party/iniparser/ -liniparser

LINK = $(LDFLAGS) $(LD_LIBS) $(AM_CFLAGS) $(CFLAGS) -o $@

objs = dut_host_if.o dut_msg_clbk.o driver_api.o dut_utest_nv.o dutserver.o sockets.o \
		logmacro_database.o

deps =  $(abs_top)/tools/mttools/shared/libmttools.a \
	$(abs_top)/tools/shared/linux/libmtlkc.a \
	$(abs_top)/wireless/shared/libmtlkwls.a \
	$(abs_top)/tools/shared/3rd_party/iniparser/libiniparser.a

dutserver_LDADD =  $(abs_top)/tools/mttools/shared/libmttools.a \
		$(abs_top)/tools/shared/linux/libmtlkc.a \
		$(abs_top)/wireless/shared/libmtlkwls.a \
		$(abs_top)/tools/shared/3rd_party/iniparser/libiniparser.a

dutserver: $(objs) $(deps)
	$(CC) $(LINK) $(objs) $(dutserver_LDADD)

all: dutserver

clean:
	rm -f $(objs) dutserver 

export logger_origin_name = dutserver
export logger_binary_name = dutserver.o
export logger_origin_id = 6
