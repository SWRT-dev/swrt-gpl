obj-y += fast_path.o
ifeq ($(wildcard net/swrt_fastpath/fast_path.c),)
ifeq ($(CONFIG_ARCH_IPQ40XX),y)
fast_path-objs = ipq4019.obj
else ifeq ($(CONFIG_ARCH_IPQ5018),y)
fast_path-objs = ipq5018.obj
else ifeq ($(CONFIG_ARCH_IPQ6018),y)
fast_path-objs = ipq6018.obj
else ifeq ($(CONFIG_ARCH_IPQ807x),y)
fast_path-objs = ipq8074.obj
else
fast_path-objs = ipq9574.obj
endif
endif
