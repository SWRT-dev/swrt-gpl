
###############################################################################
#         Copyright (c) 2020, MaxLinear, Inc.
#         Copyright 2016 - 2020 Intel Corporation
# 
#  For licensing information, see the file 'LICENSE' in the root folder of
#  this software module.
# 
###############################################################################

TEST_MODULE_NAME += tc_sample

TEST_MODULE_SOURCES += atm_tc_drv_sample.c

TEST_MODULE_OBJECTS += $(TEST_MODULE_SOURCES:.c=.o)
$(TEST_MODULE_NAME)-y += $(TEST_MODULE_OBJECTS)

obj-m += $(TEST_MODULE_NAME).o
ccflags-y += -I$(src)/../../include -Werror

-include openwrt.mk

MAKEARCH := $(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE)

all:
	$(MAKEARCH) -C $(LINUX_DIR) M=${shell pwd} modules

clean:
	$(MAKEARCH) -C $(LINUX_DIR) M=${shell pwd} clean
