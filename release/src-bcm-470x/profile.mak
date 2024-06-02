EXTRACFLAGS := -DLINUX26 -DCONFIG_BCMWL5 -DDEBUG_NOISY -DDEBUG_RCTEST -pipe -DBCMWPA2 -funit-at-a-time -Wno-pointer-sign -mcpu=cortex-a9 -mtune=cortex-a9 -DMUSL_LIBC -mno-unaligned-access -msoft-float -mfloat-abi=soft -fno-caller-saves -fno-plt -fcommon -fno-unwind-tables -fno-asynchronous-unwind-tables

ifeq ($(RTCONFIG_NVRAM_64K), y)
EXTRACFLAGS += -DRTCONFIG_NVRAM_64K 
endif
ifeq ($(RTCONFIG_DUAL_TRX), y)
EXTRACFLAGS += -DRTCONFIG_DUAL_TRX                    
endif
ifeq ($(RTCONFIG_NV128), y)
EXTRACFLAGS += -DCONFIG_NVSIZE_128
endif


export EXTRACFLAGS
