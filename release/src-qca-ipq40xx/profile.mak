EXTRACFLAGS := -DLINUX26 -DCONFIG_QCA -DDEBUG_NOISY -DDEBUG_RCTEST -pipe -funit-at-a-time -Wno-pointer-sign -marm

ifneq ($(findstring linux-3,$(LINUXDIR)),)
EXTRACFLAGS += -DLINUX30
endif

ifneq ($(findstring linux-3.14,$(LINUXDIR)),)
EXTRACFLAGS += -DLINUX3_14
endif

export EXTRACFLAGS
