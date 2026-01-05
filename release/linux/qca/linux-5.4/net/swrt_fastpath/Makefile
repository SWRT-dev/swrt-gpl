obj-y += fast_path.o
ifeq ($(wildcard net/swrt_fastpath/fast_path.c),)
ifeq ($(CONFIG_ARCH_MEDIATEK),y)
fast_path-objs = fast_path_mt798x.o
else ifeq ($(CONFIG_SOC_MT7621),y)
fast_path-objs = fast_path_mt7621.o
else ifeq ($(CONFIG_PINCTRL_IPQ8074),y)
fast_path-objs = fast_path_ipq8074.o
else ifeq ($(CONFIG_PINCTRL_IPQ5332),y)
fast_path-objs = fast_path_ipq5332.o
endif
endif
