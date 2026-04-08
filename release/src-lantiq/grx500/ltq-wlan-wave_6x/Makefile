# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

all:
	@#Nothing to do

install:
	install -d $(INSTALLDIR)/opt/intel/etc
	install -d $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/fw/bin/* $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/genrisc/bin/* $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/progmodel/bin/* $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/psd/bin/* $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/regulatory/bin/* $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/wave_components.ver $(INSTALLDIR)/opt/intel/etc/
	cd $(INSTALLDIR)/lib/firmware && ln -sf ../../tmp/cal_wlan0.bin
	cd $(INSTALLDIR)/lib/firmware && ln -sf ../../tmp/cal_wlan2.bin
	cd $(INSTALLDIR)/lib/firmware && ln -sf ../../tmp/cal_wlan4.bin

clean:
	@#Nothing to do

stage:
	@#Nothing to do

