ifneq ($(CONFIG_INTEL_DATAPATH_HAL_GSWIP30),)
obj-$(CONFIG_INTEL_DATAPATH) = datapath_api.o datapath_misc.o datapath_notifier.o datapath_logical_dev.o datapath_instance.o datapath_soc.o datapath_tx.o datapath_cqm.o datapath_pce.o
obj-$(CONFIG_DEBUG_FS) += datapath_proc_api.o datapath_proc.o
obj-$(CONFIG_INTEL_DATAPATH) += gswip30/
obj-$(CONFIG_INTEL_DATAPATH) += datapath_ioctl.o
endif

ifneq ($(CONFIG_INTEL_DATAPATH_PARTIAL_FEEDS),)
obj-$(CONFIG_INTEL_DATAPATH) = datapath_api_fn.o
obj-$(CONFIG_INTEL_DATAPATH) += datapath_api_others.o
endif


