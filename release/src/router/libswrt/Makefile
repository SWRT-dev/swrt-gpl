include ../common.mak

OBJ = libswrt.o shared.o
ifeq ($(RTCONFIG_MULTILAN_CFG), y)
OBJ += swrt_mtlan.o
endif

CFLAGS += $(if $(RTCONFIG_DEBUG),-g,-Os) -idirafter$(LINUXDIR)/include -I$(TOP)/shared  -I$(SRCBASE)/include
CFLAGS += -I$(TOP)/curl/include
CFLAGS += -I$(TOP)/openssl/include
CFLAGS += -I$(TOP)/libcodb -I$(TOP)/sqlite -I$(TOP)/libdisk
CFLAGS += -shared -I$(SRCBASE)/include -I$(STAGEDIR)/usr/include/json-c

ifeq ($(RTCONFIG_BCMARM),y)
CFLAGS += -I$(SRCBASE)/shared/bcmwifi/include -DTYPEDEF_FLOAT_T
CFLAGS += -I$(SRCBASE)/common/include
endif

ifeq ($(RTCONFIG_REALTEK),y)
LDFLAGS = -L$(TOP)/nvram${BCMEX} -lnvram -L$(TOP)/shared -lshared -lcrypt -ldl
else
LDFLAGS = -L$(TOP)/nvram${BCMEX}${EX7} -lnvram -L$(TOP)/shared -lshared -lcrypt -ldl
endif

ifeq ($(HND_ROUTER),y)
LDFLAGS += -L$(TOP)/wlcsm -lwlcsm
endif
ifeq ($(HND_ROUTER_AX),y)
LDFLAGS += -L$(TOP_PLATFORM)/nvram -lnvram
LDFLAGS += -L$(TOP_PLATFORM)/wlcsm -lwlcsm
endif

LDFLAGS += -lpthread -lm
LDFLAGS += $(LIBS) -L$(TOP)/shared -lshared -L$(TOP)/nvram -lnvram
LDFLAGS += -L$(STAGEDIR)/usr/lib -ljson-c
LDFLAGS += -L$(TOP)/libcodb -lcodb
ifeq ($(RTCONFIG_BCM_502L07P2),y)
LDFLAGS += -L$(TOP_PLATFORM)/ethctl_lib -lethctl
endif
LDFLAGS += -L$(TOP)/curl/lib/.libs -lcurl -L$(TOP)/
LDFLAGS += -L$(TOP)/openssl -lcrypto -lssl
LDFLAGS += -L$(TOP)/sqlite/.libs -lsqlite3
LDFLAGS += -L$(TOP)/libdisk -ldisk

TARGET = libswrt.so


ifneq ($(wildcard $(SRCBASE)/router/libswrt/$(BUILD_NAME)/libswrt.o),)
all:
	-cp -f prebuild/$(BUILD_NAME)/*.o ./
else
all: $(TARGET)
endif

%.o: %.h
%.o: %.c
	$(CC) $(CFLAGS) -fPIC -c $<

$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(CFLAGS) $(LIBS) $(LDFLAGS)

install:
	$(if $(RTCONFIG_DEBUG),,$(STRIP) $(TARGET))
	install -D $(TARGET) $(INSTALLDIR)/libasc/usr/lib/$(TARGET)

clean:
	-rm *.o $(TARGET)

.PHONY: all clean

