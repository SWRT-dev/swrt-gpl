include ../common.mak
include $(SRCBASE)/.config
ifeq ($(RTCONFIG_RALINK_MT7621),y)
PREBUILT_BIN=$(shell if [ ! -f "$(SRCBASE)/../sdk/mtk/mt7621/bndstrg_plus/Makefile" ]; then echo 1; else echo 0; fi)
else ifeq ($(RTCONFIG_MT798X),y)
PREBUILT_BIN=$(shell if [ ! -f "$(SRCBASE)/../sdk/mtk/mt7986/bndstrg_plus/Makefile" ]; then echo 1; else echo 0; fi)
endif

all:
ifneq ($(PREBUILT_BIN),1)
ifeq ($(RTCONFIG_RALINK_MT7621),y)
	ln -sf ../../../sdk/mtk/mt7621/bndstrg_plus bndstrg_plus
else ifeq ($(RTCONFIG_MT798X),y)
	ln -sf ../../../sdk/mtk/mt7986/bndstrg_plus bndstrg_plus
endif
	$(MAKE) -C bndstrg_plus all
else
	mkdir -p bndstrg_plus
	cp prebuilt/$(BUILD_NAME)/bndstrg2 bndstrg_plus/bndstrg2
endif

install:
	install -D bndstrg_plus/bndstrg2 $(INSTALLDIR)/bndstrg_plus/usr/sbin/bndstrg2
	$(STRIP) $(INSTALLDIR)/bndstrg_plus/usr/sbin/bndstrg2

