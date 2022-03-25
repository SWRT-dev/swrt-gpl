include upb.inc
-include $(TOPPREFIX)/.config
-include $(LINUXDIR)/.config

pb-y				+= hwnat
pb-$(CONFIG_RAETH)		+= raeth
pb-$(CONFIG_RALINK_RDM)		+= rt_rdm
#pb-$(CONFIG_RT2860V2_AP)	+= rt2860
#pb-$(CONFIG_MT7610_AP)		+= mt7610
#pb-$(CONFIG_RALINK_MT7612E)	+= rlt_wifi
#pb-$(CONFIG_RALINK_MT7603E)	+= rlt_wifi_7603e
#pb-$(CONFIG_RLT_AP_SUPPORT)	+= rlt_ap
#pb-$(CONFIG_RLT_STA_SUPPORT)	+= rlt_sta
#pb-$(CONFIG_CHIP_MT7615E)	+= mt_wifi_7615E
pb-$(CONFIG_PINCTRL_MT7622)	+= mt_wifi_5050
pb-$(CONFIG_MACH_LEOPARD)	+= mt_wifi
pb-$(CONFIG_SOC_MT7621)	+= mt_wifi_7915_v7400
#pb-y				+= ated_1.2.2
pb-y				+= ated_2.0.2
pb-y				+= flash
pb-y				+= reg
pb-y				+= regs
ifeq ($(RTCONFIG_PARAGON_NTFS),y)
pb-y				+= ufsd
endif

all: $(pb-y) $(pb-m)


############################################################################
# Generate short variable for destination directory.
# NOTE: Only one variable can be defined in one target.
############################################################################
$(pb-y) $(pb-m): S=$(TOPPREFIX)/$@
$(pb-y) $(pb-m): D=$(PBTOPDIR)/$@/prebuild


############################################################################
# Define special S or D variable here.
# NOTE: Only one variable can be defined in one target.
############################################################################
hwnat: S=$(LINUXDIR)/drivers/net/ethernet/mediatek
hwnat: D=$(PBLINUXDIR)/drivers/net/ethernet/mediatek/.objs
#rt2860 rt3352 mt7610 rlt_wifi rlt_wifi_7603e rlt_ap rlt_sta: S=$(LINUXDIR)/drivers/net/wireless
#mt_wifi_7615E: S=$(LINUXDIR)/drivers/net/wireless
#rt2860 rt3352 mt7610 rlt_wifi rlt_wifi_7603e rlt_ap rlt_sta: D=$(PBLINUXDIR)/drivers/net/wireless/.objs
#mt_wifi_7615E: D=$(PBLINUXDIR)/drivers/net/wireless/.objs
mt_wifi_5050: S=$(LINUXDIR)/drivers/net/wireless/mediatek/mt_wifi_5050
mt_wifi_5050: D=$(PBLINUXDIR)/drivers/net/wireless/mediatek/mt_wifi_5050/.objs
mt_wifi: S=$(LINUXDIR)/drivers/net/wireless/mediatek/mt_wifi
mt_wifi: D=$(PBLINUXDIR)/drivers/net/wireless/mediatek/mt_wifi/.objs
mt_wifi_7915_v7400: S=$(LINUXDIR)/drivers/net/wireless/mediatek/mt_wifi_7915_v7400
mt_wifi_7915_v7400: D=$(PBLINUXDIR)/drivers/net/wireless/mediatek/mt_wifi_7915_v7400/.objs
raeth rt_rdm: S=$(LINUXDIR)/drivers/net/$@
raeth rt_rdm: D=$(PBLINUXDIR)/drivers/net/.objs
#ufsd: S=$(TOPPREFIX)/$@/ralink_3.0.0/package
#ufsd: D=$(PBTOPDIR)/$@/ralink_3.0.0


############################################################################
# Copy binary
############################################################################
	#$(call inst_obj,$(S),$(D),foe_hook/foe_hook.o)

#rt2860:
#	$(call inst_obj,$(S),$(D),rt2860v2_ap/rt2860v2_ap.o)

#mt7610:
#	$(call inst_obj,$(S),$(D),MT7610_ap/MT7610_ap.o)

#rlt_wifi:
#	$(call inst_obj,$(S),$(D),rlt_wifi_ap/rlt_wifi.o)

#rlt_wifi_7603e:
#	$(call inst_obj,$(S),$(D),rlt_wifi_7603E_ap/rlt_wifi_7603e.o)

#rlt_ap:
#	$(call inst_obj,$(S),$(D),rlt_wifi_ap/rlt_wifi.o)

#rlt_sta:
#	$(call inst_obj,$(S),$(D),rlt_wif_sta/rlt_wifi.o)

#mt_wifi_7615E:
#	$(call inst_obj,$(S),$(D),mt_wifi_ap_7615E_4421/mt_wifi_7615E.o)
hwnat:
	$(call inst_obj,$(S),$(D),mtk_hnat/mtkhnat.o)
mt_wifi_5050:
	$(call inst_obj,$(S),$(D),mt_wifi_ap/mt_wifi.o)

mt_wifi:
	$(call inst_obj,$(S),$(D),mt_wifi_ap/mt_wifi.o)

mt_wifi_7915_v7400:
	$(call inst_obj,$(S),$(D),mt_wifi_ap/mt_wifi.o)

rt_rdm:
	$(call inst_obj,$(S),$(D),rt_rdm.o)

raeth:
	$(call inst_obj,$(S),$(D),raeth.o)

ated_1.2.2:
	$(call inst,$(S),$(D),ated)

ated_2.0.2:
	$(call inst,$(S),$(D),ated_ext)

flash:
	$(call inst,$(S),$(D),flash)

reg:
	$(call inst,$(S),$(D),reg)
regs:
	$(call inst,$(S),$(D),regs)

ufsd:
	$(call inst,$(S),$(D),ufsd.ko)

.PHONY: all $(pb-y) $(pb-m)
