#include $(TOP)/.config
obj-m := i2cleds.o
ifeq ($(R6800),y)
i2cleds-objs := gpio-sx1503-leds.o
else ifeq ($(RAX120),y)
i2cleds-objs := rax120-leds.o
else ifeq ($(RMAX6000),y)
i2cleds-objs := rmax6000-leds.o
endif

