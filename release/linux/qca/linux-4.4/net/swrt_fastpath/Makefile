obj-y += fast_path.o
ifeq ($(wildcard net/swrt_fastpath/fast_path.c),)
ifeq ($(CONFIG_ARCH_IPQ40XX),y)
fast_path-objs = fast_path_ipq4019.o
else ifeq ($(CONFIG_ARCH_IPQ5018),y)
fast_path-objs = fast_path_ipq5018.o
else ifeq ($(CONFIG_ARCH_IPQ6018),y)
fast_path-objs = fast_path_ipq6018.o
else ifeq ($(CONFIG_ARCH_IPQ807x),y)
fast_path-objs = fast_path_ipq8074.o
else
fast_path-objs = fast_path_ipq9574.o
endif
endif
