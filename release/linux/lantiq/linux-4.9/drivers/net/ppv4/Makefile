################################################################################
#
# Makefile
#  Description:
#  Makefile for the Packet Processor driver
#
#  SPDX-License-Identifier: GPL-2.0-only
#  Copyright (C) 2018-2020 Intel Corporation
#
################################################################################

TARGET := pp_drv

# Enable warning as errors
ccflags-y += -Wall -Werror
# Add PP include directories
# Dev
ccflags-y += -I$(src)/pp-dev
# Tools
ccflags-y += -I$(src)/pp-tools/logger
ccflags-y += -I$(src)/pp-tools/unit-test
ccflags-y += -I$(src)/pp-tools/elf-parser
ccflags-y += -I$(src)/pp-tools/debugfs
# HAL
ccflags-y += -I$(src)/pp-hal/infra
ccflags-y += -I$(src)/pp-hal/session-info
ccflags-y += -I$(src)/pp-hal/field-vector
ccflags-y += -I$(src)/pp-hal/descriptor
ccflags-y += -I$(src)/pp-hal/port-distributor
ccflags-y += -I$(src)/pp-hal/receive-packet-buffer
ccflags-y += -I$(src)/pp-hal/classifier
ccflags-y += -I$(src)/pp-hal/checker
ccflags-y += -I$(src)/pp-hal/modifier
ccflags-y += -I$(src)/pp-hal/rx-dma
ccflags-y += -I$(src)/pp-hal/buffer-manager
ccflags-y += -I$(src)/pp-hal/parser
ccflags-y += -I$(src)/pp-hal/micro-controller
# API
ccflags-y += -I$(src)/pp-api/pp-port-mgr
ccflags-y += -I$(src)/pp-api/pp-buffer-mgr
ccflags-y += -I$(src)/pp-api/pp-qos-mgr
ccflags-y += -I$(src)/pp-api/pp-misc
ccflags-y += -I$(src)/pp-api/pp-session-mgr
# Headers
ccflags-y += -I$(src)/pp-headers
ccflags-y += -I$(src)/pp-rdl-headers
ccflags-y += -I$(PKG_CONFIG_SYSROOT_DIR)/$(includedir)/pp

# Add other modules symbols
export KBUILD_EXTRA_SYMBOLS=$(PKG_CONFIG_SYSROOT_DIR)$(datadir)/modules/fpgapci.symvers  \
			    $(PKG_CONFIG_SYSROOT_DIR)$(datadir)/modules/learning.symvers

# PP DEV
$(TARGET)-y += pp-dev/pp_dev.o
$(TARGET)-y += pp-dev/pp_regs.o
$(TARGET)-$(CONFIG_OF) += pp-dev/pp_dts.o

### TOOLS ###
ifeq ($(CONFIG_PPV4_LGM),y)
# PP logger
$(TARGET)-y += pp-tools/logger/pp_logger.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-tools/logger/pp_logger_debugfs.o
endif
# PP elf parser
$(TARGET)-y += pp-tools/elf-parser/elf_parser.o
# PP common debugfs
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-tools/debugfs/pp_debugfs_common.o

### HAL ###
# PP infra
ifeq ($(CONFIG_PPV4_LGM),y)
$(TARGET)-y += pp-hal/infra/infra.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-hal/infra/infra_debugfs.o
# Session info
$(TARGET)-y += pp-hal/session-info/pp_si.o
# Session info
$(TARGET)-y += pp-hal/field-vector/pp_fv.o
# Descriptor
$(TARGET)-y += pp-hal/descriptor/pp_desc.o
# PP port distributor
$(TARGET)-y += pp-hal/port-distributor/port_dist.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-hal/port-distributor/port_dist_debugfs.o
# PP RPB
$(TARGET)-y += pp-hal/receive-packet-buffer/rpb.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-hal/receive-packet-buffer/rpb_debugfs.o
# PP CLASSIFIER
$(TARGET)-y += pp-hal/classifier/classifier.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-hal/classifier/classifier_debugfs.o
# PP CHECKER
$(TARGET)-y += pp-hal/checker/checker.o
$(TARGET)-y += pp-hal/checker/checker_sgc.o
$(TARGET)-y += pp-hal/checker/checker_tbm.o
$(TARGET)-y += pp-hal/checker/checker_sysfs.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-hal/checker/checker_debugfs.o
# PP MODIFIER
$(TARGET)-y += pp-hal/modifier/modifier_core.o
$(TARGET)-y += pp-hal/modifier/modifier_recipes.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-hal/modifier/modifier_debugfs.o
# PP RX-DMA
$(TARGET)-y += pp-hal/rx-dma/rx_dma.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-hal/rx-dma/rx_dma_debugfs.o
# PP Parser
$(TARGET)-y += pp-hal/parser/parser.o
$(TARGET)-y += pp-hal/parser/parser_protocols.o
$(TARGET)-y += pp-hal/parser/parser_tbl.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-hal/parser/parser_debugfs.o
# PP Micro-Controller
$(TARGET)-y += pp-hal/micro-controller/uc.o
$(TARGET)-y += pp-hal/micro-controller/uc_egr_mbox.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-hal/micro-controller/uc_debugfs.o
endif

# PP Buffer Manager
$(TARGET)-y += pp-hal/buffer-manager/bm.o

### API ###
ifeq ($(CONFIG_PPV4_LGM),y)
# PP port manager
$(TARGET)-y += pp-api/pp-port-mgr/pp_port_mgr.o
$(TARGET)-y += pp-api/pp-port-mgr/pp_port_mgr_sysfs.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-api/pp-port-mgr/pp_port_mgr_debugfs.o
# PP Session Manager
$(TARGET)-y += pp-api/pp-session-mgr/pp_session_mgr.o
$(TARGET)-y += pp-api/pp-session-mgr/pp_session_mgr_mcast.o
$(TARGET)-y += pp-api/pp-session-mgr/pp_session_mgr_syncq.o
$(TARGET)-y += pp-api/pp-session-mgr/pp_session_mgr_tdox.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-api/pp-session-mgr/pp_session_mgr_debugfs.o
endif

# PP Misc
$(TARGET)-y += pp-api/pp-misc/pp_misc.o
$(TARGET)-$(CONFIG_PPV4_LGM) += pp-api/pp-misc/pp_misc_sysfs.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-api/pp-misc/pp_misc_debugfs.o

# PP Buffer Manager
$(TARGET)-y += pp-api/pp-buffer-mgr/pp_buffer_mgr.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-api/pp-buffer-mgr/pp_buffer_mgr_debugfs.o
# PP QoS Manager
$(TARGET)-y += pp-api/pp-qos-mgr/pp_qos_main.o
$(TARGET)-y += pp-api/pp-qos-mgr/pp_qos_utils.o
$(TARGET)-y += pp-api/pp-qos-mgr/pp_qos_fw.o
$(TARGET)-$(CONFIG_PPV4_LGM) += pp-api/pp-qos-mgr/pp_qos_sysfs.o
$(TARGET)-$(CONFIG_DEBUG_FS) += pp-api/pp-qos-mgr/pp_qos_debugfs.o

# PP unit-test
ifeq ($(CONFIG_PPV4_UT),y)
ifeq ($(CONFIG_DEBUG_FS),y)
$(TARGET)-y += pp-tools/unit-test/pput_drv.o

ifeq ($(CONFIG_PPV4_LGM),y)
$(TARGET)-y += pp-tools/unit-test/tests/pp_infra_tests.o
$(TARGET)-y += pp-tools/unit-test/tests/pp_port_tests.o
$(TARGET)-y += pp-tools/unit-test/tests/pp_session_tests.o
$(TARGET)-y += pp-tools/unit-test/tests/pp_parser_tests.o
endif

$(TARGET)-y += pp-tools/unit-test/tests/pp_qos_tests.o
$(TARGET)-y += pp-tools/unit-test/tests/pp_bm_tests.o
endif
endif

ifneq ($(KERNELRELEASE),)

obj-$(CONFIG_PPV4) += $(TARGET).o

else

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

endif

