EXTRACFLAGS := -DLINUX26 -DCONFIG_QCA -DDEBUG_NOISY -DDEBUG_RCTEST -pipe -funit-at-a-time -Wno-pointer-sign -march=armv8-a -mcpu=cortex-a53+crypto -DMUSL_LIBC -fno-caller-saves -fno-plt -Wa,--noexecstack -fhonour-copts -fstack-protector-strong -D_FORTIFY_SOURCE=2 -Wl,-z,now -Wl,-z,relro

ifneq ($(findstring linux-3,$(LINUXDIR)),)
EXTRACFLAGS += -DLINUX30
endif

export EXTRACFLAGS
