# Convert swrt build environment variable to qsdk
include ../qsdk.config
include ../rules.mk
PKG_BUILD_DIR=$(shell pwd)/source

# Define variables for qca-hk
INSTALLFWDIR:=$(INSTALLDIR)/lib/firmware

all:
	@#Nothing to do

install:
	install -d $(INSTALLDIR)/vendor/firmware
	[ ! -e $(PKG_BUILD_DIR)/Data.msc ] || install -m 644 $(PKG_BUILD_DIR)/Data.msc $(INSTALLDIR)/vendor/firmware
	for socdir in IPQ5018 QCN6122; do \
		install -d $(INSTALLFWDIR)/$${socdir} ; \
		ln -sf /tmp/caldata.bin $(INSTALLFWDIR)/$${socdir}/caldata.bin ; \
		cd $(PKG_BUILD_DIR)/$${socdir}/PIL_IMAGES && rsync -avcH . $(INSTALLFWDIR)/$${socdir} && cd - ; \
		cd $(PKG_BUILD_DIR)/$${socdir} && rsync -avcH bdwlan* $(INSTALLFWDIR)/$${socdir} && cd - ; \
		[ ! -e $(BUILD_NAME)/$${socdir} ] || install -m 644 $(BUILD_NAME)/$${socdir}/bdwlan* $(INSTALLFWDIR)/$${socdir} ; \
	done


clean:
	@#Nothing to do


