obj-$(CONFIG_INTEL_DATAPATH) += datapath_misc.o datapath_gswip.o datapath_proc.o
obj-$(CONFIG_INTEL_DATAPATH) += datapath_ppv4.o
obj-$(CONFIG_INTEL_DATAPATH) += datapath_lookup_proc.o datapath_ppv4_api.o
obj-$(CONFIG_INTEL_DATAPATH) += datapath_rx.o datapath_tx.o

ifneq ($(CONFIG_INTEL_DATAPATH_HAL_GSWIP31_MIB),)
obj-$(CONFIG_INTEL_DATAPATH) += datapath_mib.o
endif

ifneq ($(CONFIG_INTEL_DATAPATH_CPUFREQ),)
obj-$(CONFIG_INTEL_DATAPATH) += datapath_coc.o
endif

ifneq ($(CONFIG_INTEL_DATAPATH_SWITCHDEV),)
obj-$(CONFIG_INTEL_DATAPATH) += datapath_switchdev.o datapath_ext_vlan.o datapath_tc_asym_vlan.o
endif
