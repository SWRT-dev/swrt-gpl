#include $(TOP)/.config

ifeq ($(RALINK),y)
obj-m := sx150x-leds.o
sx150x-leds-objs := gpio-sx1503-leds.o
else ifeq ($(QCA),y)
obj-m := i2cleds.o
i2cleds-objs := rax120-leds.o
endif

