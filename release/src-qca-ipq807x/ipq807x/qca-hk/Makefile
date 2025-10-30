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
	for socdir in IPQ8074 IPQ8074A; do \
		install -d $(INSTALLFWDIR)/$${socdir} ; \
		ln -sf /tmp/caldata.bin $(INSTALLFWDIR)/$${socdir}/caldata.bin ; \
		cd $(PKG_BUILD_DIR)/$${socdir}/PIL_IMAGES && rsync -avcH . $(INSTALLFWDIR)/$${socdir} && cd - ; \
		cd $(PKG_BUILD_DIR)/$${socdir} && rsync -avcH bdwlan* $(INSTALLFWDIR)/$${socdir} && cd - ; \
		[ ! -e $(BUILD_NAME)/$${socdir} ] || install -m 644 $(BUILD_NAME)/$${socdir}/bdwlan* $(INSTALLFWDIR)/$${socdir} ; \
		[ "$(SWRT_NAME)" != "RAX120" ] || { \
			install -m 644 $(SWRT_NAME)/$${socdir}/bdwlan* $(INSTALLFWDIR)/$${socdir} ; \
			install -d $(INSTALLFWDIR)/$${socdir}/AU/ ; \
			install -m 644 $(SWRT_NAME)/$${socdir}/AU/* $(INSTALLFWDIR)/$${socdir}/AU/ ; \
			install -d $(INSTALLFWDIR)/$${socdir}/CN/ ; \
			install -m 644 $(SWRT_NAME)/$${socdir}/CN/* $(INSTALLFWDIR)/$${socdir}/CN/ ; \
			install -d $(INSTALLFWDIR)/$${socdir}/KR/ ; \
			install -m 644 $(SWRT_NAME)/$${socdir}/KR/* $(INSTALLFWDIR)/$${socdir}/KR/ ; \
			install -d $(INSTALLFWDIR)/$${socdir}/SG/ ; \
			install -m 644 $(SWRT_NAME)/$${socdir}/SG/* $(INSTALLFWDIR)/$${socdir}/SG/ ; \
			install -d $(INSTALLFWDIR)/$${socdir}/TW/ ; \
			install -m 644 $(SWRT_NAME)/$${socdir}/TW/* $(INSTALLFWDIR)/$${socdir}/TW/ ; \
			install -d $(INSTALLFWDIR)/$${socdir}/US/ ; \
			install -m 644 $(SWRT_NAME)/$${socdir}/US/* $(INSTALLFWDIR)/$${socdir}/US/ ; \
			install -d $(INSTALLFWDIR)/$${socdir}/VN/ ; \
			install -m 644 $(SWRT_NAME)/$${socdir}/VN/* $(INSTALLFWDIR)/$${socdir}/VN/ ; \
		} ; \
	done


clean:
	@#Nothing to do


