include .config

ifeq ($(MT7622_ASIC_BOARD),y)
obj-y += mtk_nor_7622.o
else
obj-y += mtk_nor.o
obj-y += core/snfc.o
endif

ifeq ($(CONFIG_SYS_SOC),"leopard")
obj-y += spi_nor_uid.o
endif
