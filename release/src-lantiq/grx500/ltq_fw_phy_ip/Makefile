# Convert swrt build environment variable to sdk
include ../sdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

all:
	touch stamp-h1

install:
	install -d $(INSTALLDIR)/lib/firmware/
	install -m 755 $(PKG_BUILD_DIR)/xRx5xx/ltq_fw_PHY11G_IP_1v1_xRx5xx_A21_R8549.bin \
		$(INSTALLDIR)/lib/firmware/ltq_fw_PHY11G_IP_xRx5xx_A21.bin

clean:
	@#Nothing to do

stage:
	@#Nothing to do

