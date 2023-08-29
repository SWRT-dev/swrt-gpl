include ../qsdk.config

# Convert asuswrt build environment variable to qca-nss-fw-eip-hk
PKG_BUILD_DIR=$(shell pwd)/source

# Define variables for qca-nss-fw-eip-hk
INSTALLFWDIR:=$(INSTALLDIR)/lib/firmware

all:
	@#Nothing to do

install:
	@echo Install $(TYPE) NSS firmware.
	install -d $(INSTALLFWDIR)
	rsync -avcH $(PKG_BUILD_DIR)/ $(INSTALLFWDIR)

clean:
	@#Nothing to do
