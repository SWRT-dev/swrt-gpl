# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

# Define variables for qca-nss-fw-eip-hk
INSTALLFWDIR:=$(INSTALLDIR)/lib/firmware

all:
	@#Nothing to do

install:
	@echo Install NSS EIP firmware.
	install -d $(INSTALLFWDIR)
	rsync -avcH $(PKG_BUILD_DIR)/ $(INSTALLFWDIR)

clean:
	@#Nothing to do
