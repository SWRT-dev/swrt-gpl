obj-m += dc_mode-hw.o
ifeq ($(CONFIG_DIRECTCONNECT_DP_XRX500),y)
	dc_mode-hw-objs += dc_mode0/dc_mode0_xrx500.o
else ifeq ($(CONFIG_DIRECTCONNECT_DP_LGM),y)
	dc_mode-hw-objs += dc_modex/dc_modex_ext.o
else ifeq ($(CONFIG_DIRECTCONNECT_DP_PRX300),y)
	dc_mode-hw-objs += dc_modex/dc_modex_ext.o
else ifeq ($(CONFIG_DIRECTCONNECT_DP_XRX750),y)
	dc_mode-hw-objs += dc_mode1/dc_mode1_xrx750.o
	dc_mode-hw-objs += libcppp.o
else ifeq ($(CONFIG_DIRECTCONNECT_DP_330),y)
	dc_mode-hw-objs += dc_mode1/dc_mode1_xrx330.o
endif

#ifneq ($(CONFIG_SW_DCMODE1_BUILTIN),y)
#	obj-m += dc_mode-sw.o
#	dc_mode-sw-objs += dc_mode1/dc_mode1_sw.o
#endif
