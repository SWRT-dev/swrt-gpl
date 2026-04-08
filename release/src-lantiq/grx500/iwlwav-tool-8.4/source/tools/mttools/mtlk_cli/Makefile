builddir = .

override CC := $(abs_top)/support/mtlkfront.sh $(CC)
override AM_CFLAGS = -Wall -Wno-multichar -fno-strict-aliasing -Werror -Wno-error=undef
override CFLAGS += -O2 -Wno-multichar
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(builddir)/.
override CFLAGS += -I$(abs_top)/tools/mttools/shared
override CFLAGS += -I$(abs_top)/tools/mttools/shared/linux/
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/wireless/shared/
override CFLAGS += -I$(abs_top)/include/config/
override CFLAGS += -I$(abs_top)/tools/rtlogger/shared/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLINUX  -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT

LD_LIBS =	-L$(abs_top)/tools/mttools/shared/ -lmttools \
		-L$(abs_top)/tools/shared/linux/ -lmtlkc \
		-L$(abs_top)/wireless/shared/ -lmtlkwls

LINK = $(LDFLAGS) $(LD_LIBS) $(AM_CFLAGS) $(CFLAGS) -o $@

objs =  main.o \
		logmacro_database.o

deps =  $(abs_top)/tools/mttools/shared/libmttools.a \
	$(abs_top)/tools/shared/linux/libmtlkc.a \
	$(abs_top)/wireless/shared/libmtlkwls.a

mtlk_cli_LDADD =  $(abs_top)/tools/mttools/shared/libmttools.a \
		$(abs_top)/tools/shared/linux/libmtlkc.a \
		$(abs_top)/wireless/shared/libmtlkwls.a


mtlk_cli: $(objs) $(deps)
	$(CC) $(LINK) $(objs) $(mtlk_cli_LDADD)

all: mtlk_cli

clean:
	rm -f $(objs) mtlk_cli 

export logger_origin_name = mtlk_cli
export logger_binary_name = mtlk_cli.o
export logger_origin_id = 6
