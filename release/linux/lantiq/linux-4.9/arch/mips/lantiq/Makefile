# Copyright (C) 2010 John Crispin <john@phrozen.org>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License version 2 as published
# by the Free Software Foundation.

obj-$(CONFIG_SOC_XWAY) += prom.o irq.o clk.o
obj-$(CONFIG_EARLY_PRINTK) += early_printk.o

obj-$(CONFIG_SOC_TYPE_XWAY) += xway/
obj-$(CONFIG_SOC_FALCON) += falcon/
obj-$(CONFIG_SOC_GRX500) += grx500/
obj-$(CONFIG_SOC_TYPE_GRX500_TEP) += grx500_bootcore/
obj-y$(CONFIG_LTQ_VMB)+=lantiq-amon.o
obj-$(CONFIG_LTQ_VMB)+=lantiq-vmb.o
obj-$(CONFIG_LTQ_ITC)+=lantiq-itc.o
