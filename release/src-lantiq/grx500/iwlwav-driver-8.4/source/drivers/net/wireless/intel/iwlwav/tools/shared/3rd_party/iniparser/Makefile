builddir = .

override CC := $(abs_top)/support/mtlkfront.sh $(CC)
override CFLAGS += -O2 -Wno-multichar
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(builddir)/.

override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/tools/mttools/shared
override CFLAGS += -I$(abs_top)/tools/mttools/shared/linux/
override CFLAGS += -I$(abs_top)/wireless/shared/
override CFLAGS += -I$(abs_top)/tools/rtlogger/shared/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT
ARFLAGS = cru

objs = dictionary.o iniparser.o \
		logmacro_database.o


libiniparser_AR = $(AR) $(ARFLAGS)

libiniparser.a: $(objs)
	$(libiniparser_AR) libiniparser.a $(objs)
	$(RANLIB) libiniparser.a

all: libiniparser.a

clean:
	rm -f $(objs) libiniparser.a

export logger_origin_name = libiniparser
export logger_binary_name = libiniparser.a
export logger_origin_id = 4
