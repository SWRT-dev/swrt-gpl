EXTRACFLAGS := -DLINUX26 -DCONFIG_QCA -DDEBUG_NOISY -D_GNU_SOURCE -DDEBUG_RCTEST -pipe -funit-at-a-time -Wno-pointer-sign -marm -D__UAPI_DEF_ETHHDR -DMUSL_LIBC

ifneq ($(findstring linux-3,$(LINUXDIR)),)
EXTRACFLAGS += -DLINUX30
endif

export EXTRACFLAGS
