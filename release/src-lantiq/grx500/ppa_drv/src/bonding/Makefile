
###############################################################################
#         Copyright (c) 2020, MaxLinear, Inc.
#         Copyright 2016 - 2020 Intel Corporation
# 
#  For licensing information, see the file 'LICENSE' in the root folder of
#  this software module.
# 
###############################################################################

MODULE_NAME += ima_bonding_driver

tasklet=n
ifeq "$(tasklet)" "y"
ccflags-y+= -DTASKLET
POLLING_ENGINE=src/common/ima_tasklet.c
else
POLLING_ENGINE=src/common/ima_workqueue.c
endif

# For debugging Rx module
#ccflags-y+= -DGETBUFFER_TEST=0
ccflags-y+= -DRESEQUENCING_TEST=0
#ccflags-y+= -DDEBUG_255
#ccflags-y+= -DBADFRAME_THEN_DUMPITSCELLS

# ASM module settings
#ccflags-y+= -DASM_PRINT
#ccflags-y+= -DASM_LOOPBACK
#ccflags-y+= -DTX_PRINT

# Tx ATM module settings
#ccflags-y+=-DTX_FRAME_PER_LINE
#ccflags-y+=-DTX_FRAME_PER_TIMESLOT
ccflags-y+= -DTXDELAY
ccflags-y+= -DTXRATIO
ccflags-y+= -DTX_FILL_LEVEL

# Rx ATM Stack usage
#ccflags-y+= -DDISABLE_RX_ATMSTACK

# For CPU load measurement
#ccflags-y+= -DTX_CPU0_RX_CPU2
#ccflags-y+= -DTX_CPU2_RX_CPU0
#ccflags-y+= -DMEASURE_JIFFIES

#ccflags-y+= -DENABLE_IOCTL_DEBUG

SOURCES += src/atm/ima_tx_module.c \
           src/atm/ima_atm_rx.c \
           src/atm/ima_rx_bonding.c \
           src/atm/ima_rx_reassembler.c \
           src/asm/ima_asm.c \
           src/common/ima_tx_queue.c \
           src/common/ima_rx_queue.c \
           src/common/ima_common.c \
           src/atm/ima_tx_cell_buffer.c \
           src/common/ima_polling.c \
           $(POLLING_ENGINE) \
           src/common/ima_vrx518_tc_interface.c \
           src/manager/ima_bonding_debugfs.c \
           src/manager/ima_bonding_ioctl_if.c \
           src/manager/ima_debug_manager.c \
           src/manager/ima_manager.c

OBJECTS += $(SOURCES:.c=.o)
obj-m += $(MODULE_NAME).o
$(MODULE_NAME)-y += $(OBJECTS)
ccflags-y += -I$(src)/include -Werror
