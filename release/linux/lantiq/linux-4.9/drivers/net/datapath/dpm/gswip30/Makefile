obj-$(CONFIG_INTEL_DATAPATH) += datapath_misc.o datapath_gswip.o
obj-$(CONFIG_INTEL_DATAPATH) += datapath_rx.o datapath_tx.o

ifneq ($(CONFIG_INTEL_DATAPATH_HAL_GSWIP30_MIB),)
obj-$(CONFIG_INTEL_DATAPATH) += datapath_mib.o
endif

ifneq ($(CONFIG_INTEL_DATAPATH_CPUFREQ),)
obj-$(CONFIG_INTEL_DATAPATH) += datapath_coc.o
endif

obj-$(CONFIG_DEBUG_FS) += datapath_proc.o datapath_lookup_proc.o
