# package/firmware/wireless-regdb
#

PKG_BUILD_DIR=$(shell pwd)/source

all:
	@#Nothing to do

install:
	install -d $(INSTALLDIR)/wireless-regdb/rom/firmware
	$(PKG_BUILD_DIR)/db2fw.py $(INSTALLDIR)/wireless-regdb/rom/firmware/regulatory.db $(PKG_BUILD_DIR)/db.txt
	[ ! -e $(BUILD_NAME) ] || [ ! -f $(BUILD_NAME)/db.txt ] || ($(PKG_BUILD_DIR)/db2fw.py $(INSTALLDIR)/rom/firmware/regulatory.db $(BUILD_NAME)/db.txt)

clean:
	@#Nothing to do
