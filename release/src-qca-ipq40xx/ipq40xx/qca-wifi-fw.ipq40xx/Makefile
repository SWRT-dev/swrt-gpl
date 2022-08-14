# Convert asuswrt build environment variable to qca-wifi-fw
PKG_BUILD_DIR=$(shell pwd)/source

# Define variables for qca-wifi-fw
INSTALLFWDIR:=$(INSTALLDIR)/lib/firmware

all:
	@#Nothing to do

install:
	install -d $(INSTALLFWDIR)/IPQ4019/hw.1
	install -m 644 $(PKG_BUILD_DIR)/* $(INSTALLFWDIR)/IPQ4019/hw.1
	[ ! -e $(BUILD_NAME) ] || install -m 644 $(BUILD_NAME)/*.bin $(INSTALLFWDIR)/IPQ4019/hw.1

clean:
	@#Nothing to do
