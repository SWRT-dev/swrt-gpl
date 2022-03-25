obj-$(CONFIG_INTEL_DATAPATH) += datapath_misc.o datapath_gswip.o datapath_proc.o
obj-$(CONFIG_INTEL_DATAPATH) += datapath_rx.o datapath_tx.o datapath_lookup_proc.o 

ifneq ($(CONFIG_INTEL_DATAPATH_QOS_HAL),)
obj-$(CONFIG_INTEL_DATAPATH) += datapath_ppv4_api.o datapath_ppv4.o datapath_ppv4_session.o
endif

ifneq ($(CONFIG_INTEL_DATAPATH_HAL_GSWIP32_MIB),)
obj-$(CONFIG_INTEL_DATAPATH) += datapath_mib.o
endif

ifneq ($(CONFIG_INTEL_DATAPATH_CPUFREQ),)
obj-$(CONFIG_INTEL_DATAPATH) += datapath_coc.o
endif

ifneq ($(CONFIG_INTEL_DATAPATH_SWITCHDEV),)
obj-$(CONFIG_INTEL_DATAPATH) += datapath_switchdev.o datapath_ext_vlan.o datapath_tc_asym_vlan.o
endif
