#
# SPDX-License-Identifier:	GPL-2.0+
#

ifdef CONFIG_SPL_BUILD
obj-y += pll2.o
obj-y += spm_mtcmos.o
obj-y += dramc/
else
obj-y += gpio.o
obj-y += leopard_evb.o
endif
obj-y += uart.o
obj-y += timer.o
obj-y += wdt.o

include $(COMMON_DIR_MK)

#$(warning ====================================================================)
#$(warning $(SRCTREE))
#$(warning $(OBJTREE))
#$(warning $(UBOOTINCLUDE))
#$(warning $(ARCH))
#$(warning $(BOARD))
#$(warning ====================================================================)
