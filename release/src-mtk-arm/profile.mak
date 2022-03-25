#export EXTRACFLAGS := -DLINUX26 -DDEBUG_NOISY -DDEBUG_RCTEST -pipe -funit-at-a-time -Wno-pointer-sign -mtune=mips32 -mips32
ifeq ($(RTAX53U)$(RTAX54)$(RT4GAX56),y)
EXTRACFLAGS := -DLINUX26 -DCONFIG_RALINK -DMUSL_LIBC -DDEBUG_NOISY -DDEBUG_RCTEST -D_GNU_SOURCE -D__UAPI_DEF_ETHHDR -DRALINK_DBDC_MODE -pipe -funit-at-a-time -Wno-pointer-sign -mtune=mips32 -mips32
else ifeq ($(RT4GAC86U),y)
EXTRACFLAGS := -DLINUX26 -DCONFIG_RALINK -DDEBUG_NOISY -DDEBUG_RCTEST -D__UAPI_DEF_ETHHDR -pipe -funit-at-a-time -Wno-pointer-sign
else
EXTRACFLAGS := -DLINUX26 -DCONFIG_RALINK -DMUSL_LIBC -DDEBUG_NOISY -DDEBUG_RCTEST -D_GNU_SOURCE -D__UAPI_DEF_ETHHDR -DRALINK_DBDC_MODE -pipe -funit-at-a-time -Wno-pointer-sign -marm
endif
ifneq ($(findstring linux-3,$(LINUXDIR)),)
EXTRACFLAGS += -DLINUX30
endif

export EXTRACFLAGS
