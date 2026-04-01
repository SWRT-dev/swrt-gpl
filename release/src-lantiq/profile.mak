ifeq ($(RTCONFIG_NVRAM_64K), y)
EXTRACFLAGS += -DRTCONFIG_NVRAM_64K
endif
ifeq ($(RTCONFIG_NV128), y)
EXTRACFLAGS += -DCONFIG_NVSIZE_128
endif

ifeq ($(RTCONFIG_LANTIQ),y)
EXTRACFLAGS := -DCONFIG_LANTIQ -DDEBUG_NOISY -DDEBUG_RCTEST -pipe -funit-at-a-time -Wno-pointer-sign -mno-branch-likely -mtune=24kc -march=mips32r2 -fno-caller-saves -fno-plt -fPIC -mabicalls -DMUSL_LIBC -D_GNU_SOURCE
endif

EXTRACFLAGS += -DLINUX30

export EXTRACFLAGS
