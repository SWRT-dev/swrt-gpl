#export EXTRACFLAGS := -DLINUX26 -DDEBUG_NOISY -DDEBUG_RCTEST -pipe -funit-at-a-time -Wno-pointer-sign -mtune=mips32 -mips32
ifeq ($(EN7561)),y)
EXTRACFLAGS := -DLINUX26 -DCONFIG_RALINK -DMUSL_LIBC -DDEBUG_NOISY -DDEBUG_RCTEST -D_GNU_SOURCE -D__UAPI_DEF_ETHHDR -pipe -funit-at-a-time -Wno-pointer-sign -mtune=mips32 -mips32
ifeq ($(DBDC_MODE)),y)
EXTRACFLAGS += -DRALINK_DBDC_MODE
endif
endif
ifneq ($(findstring linux-3,$(LINUXDIR)),)
EXTRACFLAGS += -DLINUX30
endif

export EXTRACFLAGS

