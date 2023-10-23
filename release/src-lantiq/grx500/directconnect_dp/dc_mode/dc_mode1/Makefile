ifeq ($(CONFIG_DIRECTCONNECT_DP_XRX750),y)
	obj-m += dc_mode1-xrx750.o
	dc_mode1-xrx750-objs += dc_mode1_xrx750.o
	dc_mode1-xrx750-objs += libcppp.o
endif

ifeq ($(CONFIG_DIRECTCONNECT_DP_330),y)
	obj-m += dc_mode1-xrx330.o
	dc_mode1-xrx330-objs += dc_mode1_xrx330.o
endif

ifneq ($(CONFIG_SW_DCMODE1_BUILTIN),y)
	obj-m += dc_mode1-sw.o
	dc_mode1-sw-objs += dc_mode1_sw.o
endif
