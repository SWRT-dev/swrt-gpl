-include ../../include/config/auto.conf
-include ../../include/autoconf.mk

FLTYPE=$(CONFIG_FLASH_TYPE)
ifeq ($(CONFIG_MODEL),)
$(error No model!)
endif

all:
	python elftombn.py -f ../../u-boot -o ./$(CONFIG_MODEL)/openwrt-ipq6018-u-boot.mbn -v 6
	python pack.py --arch ipq6018 --fltype $(FLTYPE) --srcPath ./$(CONFIG_MODEL) --inImage ./$(CONFIG_MODEL) --outImage . && \
		rsync -avcH $(FLTYPE)-ipq6018-single.img ../../u-boot_$(FLTYPE).img

clean:
	rm -f nand-ipq6018-single.img *.pyc $(CONFIG_MODEL)/*.padded $(CONFIG_MODEL)/openwrt-ipq6018-u-boot* $(CONFIG_MODEL)/flash.*
