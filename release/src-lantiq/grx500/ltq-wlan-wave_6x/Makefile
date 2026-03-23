# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

all:
	@#Nothing to do

install:
	install -d $(INSTALLDIR)/opt/intel/wave/images
	install -d $(INSTALLDIR)/opt/intel/bin
	install -d $(INSTALLDIR)/opt/intel/sbin
	install -d $(INSTALLDIR)/opt/intel/etc
	install -d $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/fw/bin/* $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/fw/bin/fw_scd_file.scd $(INSTALLDIR)/opt/intel/bin
	install -m 755 $(PKG_BUILD_DIR)/genrisc/bin/* $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/progmodel/bin/* $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/psd/bin/* $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/regulatory/bin/* $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/wave_components.ve $(INSTALLDIR)/opt/intel/etc/

clean:
	@#Nothing to do



