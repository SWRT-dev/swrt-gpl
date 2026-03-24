builddir = .

override CFLAGS += -O2 -Wno-multichar
override CFLAGS += -I$(builddir)/.
override CFLAGS += -I$(abs_top)
override CFLAGS += -I$(abs_top)/tools/shared/
override CFLAGS += -I$(abs_top)/tools/shared/linux/
override CFLAGS += -I$(abs_top)/wireless/shared_iwlwav-dev/
override CFLAGS += -DHAVE_CONFIG_H -include $(abs_top)/include/config/config.h
override CFLAGS += -DMTLK_SOURCE_VERSION="\"1.0\""
override CFLAGS += -DLOG_LOCAL_OID=MTLKFRONT_WILL_FILL_IT
ARFLAGS = cru

objs = dictionary.o iniparser.o

libiniparser_AR = $(AR) $(ARFLAGS)

libiniparser.a: $(objs)
	$(libiniparser_AR) libiniparser.a $(objs)
	$(RANLIB) libiniparser.a

all: libiniparser.a

clean:
	rm -f $(objs) libiniparser.a
