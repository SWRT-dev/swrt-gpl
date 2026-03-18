# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

# Define variables for qca-nss-fw-hk
INSTALLFWDIR:=$(INSTALLDIR)/lib/firmware

all:
	@#Nothing to do

install:
	@echo Install NSS firmware.
	install -d $(INSTALLFWDIR)
	install -m 644 $(PKG_BUILD_DIR)/retail_router0.bin $(INSTALLFWDIR)/qca-nss0-retail.bin
	ln -sf qca-nss0-retail.bin $(INSTALLFWDIR)/qca-nss0.bin
ifneq ($(CONFIG_KERNEL_IPQ_MEM_PROFILE),256)
	install -m 644 $(PKG_BUILD_DIR)/retail_router1.bin $(INSTALLFWDIR)/qca-nss1-retail.bin
	ln -sf qca-nss1-retail.bin $(INSTALLFWDIR)/qca-nss1.bin
endif

clean:
	@#Nothing to do
