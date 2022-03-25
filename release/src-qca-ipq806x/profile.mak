ifeq ($(MUSL64),y)
#MARCH := -march=aarch64
else
MARCH := -marm
endif

EXTRACFLAGS := -DLINUX26 -DCONFIG_QCA -DDEBUG_NOISY -DDEBUG_RCTEST -pipe -funit-at-a-time -Wno-pointer-sign $(MARCH)

ifneq ($(findstring linux-3,$(LINUXDIR)),)
EXTRACFLAGS += -DLINUX30
endif

export EXTRACFLAGS
