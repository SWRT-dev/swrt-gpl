obj-$(CONFIG_ASUS_NVRAM_API)  += nvram_linux.o
ifeq ($(CONFIG_SOC_MT7621),y)
nvram_linux-objs = nvram_linux_mips.o
else
nvram_linux-objs = nvram_linux_arm.o
endif

