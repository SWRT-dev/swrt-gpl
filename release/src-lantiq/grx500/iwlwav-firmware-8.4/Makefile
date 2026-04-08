# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

all:
	@#Nothing to do

install:
	install -d $(INSTALLDIR)/opt/intel/etc
	install -d $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/* $(INSTALLDIR)/lib/firmware/
	echo -e "iwlwav_firmware_tag=\"0e00b32e\"" > $(INSTALLDIR)/opt/intel/etc/iwlwav_firmware.ver

clean:
	@#Nothing to do

stage:
	@#Nothing to do

