# Release Version 1.0.4
# Add config define "CONFIG_SND_SOC_PROSLIC" to "y"
# Add to Makefile of prented folder: obj-$(CONFIG_SND_SOC_PROSLIC) += proslic/

# Platform
subdir-ccflags-y += -DSI3218X
subdir-ccflags-y += -DPROSLIC_LINUX_KERNEL
subdir-ccflags-y += -Iinc
subdir-ccflags-y += -Iconfig_inc


# Basic
snd-soc-si3218x-objs := proslic_sys_main.o proslic_sys_spi.o proslic_sys_timer.o si3218x.o proslic_spi_api.o proslic_timer_intf_linux.o

obj-$(CONFIG_SND_SOC_SI3218X) += src/proslic.o src/proslic_tstin.o src/si_voice.o src/si_voice_version.o src/si3218x_intf.o src/vdaa.o src/vdaa_constants.o src/si3218x_LCCB_constants.o
obj-$(CONFIG_SND_SOC_SI3218X) += patch_files/si3218x_patch_A_2017MAY25.o
obj-$(CONFIG_SND_SOC_SI3218X) += snd-soc-si3218x.o
