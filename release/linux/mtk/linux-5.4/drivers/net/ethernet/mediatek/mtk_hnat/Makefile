ccflags-y=-Werror

obj-$(CONFIG_NET_MEDIATEK_HNAT)         += mtkhnat.o
mtkhnat-objs := hnat.o hnat_nf_hook.o hnat_debugfs.o hnat_mcast.o \
		external/hnat_entry.o
mtkhnat-$(CONFIG_MEDIATEK_NETSYS_V3) += external/v3/hnat_ipv4_hnapt.o
ifeq ($(CONFIG_NET_DSA_AN8855), y)
mtkhnat-y	+= hnat_stag.o
else
mtkhnat-$(CONFIG_NET_DSA_MT7530)	+= hnat_stag.o
endif
