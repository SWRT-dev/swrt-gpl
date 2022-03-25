#include $(srctree)/drivers/misc/mediatek/Makefile.custom
ifdef CONFIG_SPL_BUILD
obj-$(CONFIG_MTK_MTD_NAND) += mtk_snand_spl.o
else
obj-$(CONFIG_MTK_MTD_NAND) += mtk_snand_k.o
endif
