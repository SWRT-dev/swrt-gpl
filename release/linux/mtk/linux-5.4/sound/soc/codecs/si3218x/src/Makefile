#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#

# platform driver
subdir-ccflags-y += -I../inc
subdir-ccflags-y += -DSI3218X
subdir-ccflags-y += -DPROSLIC_LINUX_KERNEL
snd-soc-procslic-common-objs := proslic.o proslic_tstin.o si_voice.o si_voice_version.o si3218x_intf.o vdaa.o si3217x_intf.o si3217x_revb_intf.o si3217x_revc_intf.o si3219x_intf.o si3226x_intf.o si3228x_intf.o

obj-$(CONFIG_SND_SOC_SI3218X) += snd-soc-proslic-common.o
