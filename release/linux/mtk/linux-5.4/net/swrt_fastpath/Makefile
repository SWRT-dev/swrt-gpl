obj-y += fast_path.o
ifeq ($(wildcard net/swrt_fastpath/fast_path.c),)
ifeq ($(CONFIG_ARCH_MEDIATEK),y)
fast_path-objs = fast_path_798x.obj
else ifeq ($(CONFIG_SOC_MT7621),y)
fast_path-objs = fast_path_7621.obj
endif
endif
