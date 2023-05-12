include $(TOP)/.config

TARGET_CFLAGS = -I$(LINUX_DIR)/include
ifeq ($(RTCONFIG_SOC_IPQ40XX),y)
	TARGET_CFLAGS += -DCONFIG_IPQ40XX_SUPPORT
else ifeq ($(RTCONFIG_SOC_IPQ50XX),y)
	TARGET_CFLAGS += -DCONFIG_IPQ50XX_SUPPORT
else ifeq ($(RTCONFIG_SOC_IPQ60XX),y)
	TARGET_CFLAGS += -DCONFIG_IPQ60XX_SUPPORT
else ifeq ($(RTCONFIG_SOC_IPQ8064),y)
	TARGET_CFLAGS += -DCONFIG_IPQ806X_SUPPORT
else ifeq ($(RTCONFIG_SOC_IPQ8074),y)
	TARGET_CFLAGS += -DCONFIG_IPQ807X_SUPPORT
else ifeq ($(RTCONFIG_SOC_IPQ95XX),y)
	TARGET_CFLAGS += -DCONFIG_IPQ95XX_SUPPORT
else ifeq ($(RTCONFIG_RALINK_MT7621),y)
	TARGET_CFLAGS += -DCONFIG_MT7621_SUPPORT
else ifeq ($(RTCONFIG_RALINK_MT798X),y)
	TARGET_CFLAGS += -DCONFIG_MT798X_SUPPORT
else ifeq ($(RTCONFIG_AMD64),y)
	TARGET_CFLAGS += -DCONFIG_X86_SUPPORT
endif


ifneq ($(wildcard liboptimizer.c),)
all: pbr-optimizer

pbr-optimizer: main.c liboptimizer.c
	$(CC) $(CFLAGS) $(TARGET_CFLAGS) $(LDFLAGS) -o pbr-optimizer liboptimizer.c main.c

else
all: 
ifeq ($(RTCONFIG_SOC_IPQ40XX),y)
	cp prebuilt/pbr-optimizer-ipq40xx pbr-optimizer
else ifeq ($(RTCONFIG_SOC_IPQ50XX),y)
	cp prebuilt/pbr-optimizer-ipq50xx pbr-optimizer
else ifeq ($(RTCONFIG_SOC_IPQ60XX),y)
	cp prebuilt/pbr-optimizer-ipq60xx pbr-optimizer
else ifeq ($(RTCONFIG_SOC_IPQ8064),y)
	cp prebuilt/pbr-optimizer-ipq8064 pbr-optimizer
else ifeq ($(RTCONFIG_SOC_IPQ8074),y)
	cp prebuilt/pbr-optimizer-ipq8074 pbr-optimizer
else ifeq ($(RTCONFIG_SOC_IPQ95XX),y)
	cp prebuilt/pbr-optimizer-ipq95xx pbr-optimizer
else ifeq ($(RTCONFIG_RALINK_MT7621),y)
	cp prebuilt/pbr-optimizer-mt7621 pbr-optimizer
else ifeq ($(RTCONFIG_RALINK_MT798X),y)
	cp prebuilt/pbr-optimizer-mt798x pbr-optimizer
else ifeq ($(RTCONFIG_AMD64),y)
	cp prebuilt/pbr-optimizer-x86 pbr-optimizer
endif
endif

install:
	install -d $(INSTALLDIR)/usr/sbin
	install -m 0755 pbr-optimizer $(INSTALLDIR)/usr/sbin
	$(STRIP) $(INSTALLDIR)/usr/sbin/pbr-optimizer

clean:
	rm -f pbr-optimizer  *.elf *.gdb *.o
