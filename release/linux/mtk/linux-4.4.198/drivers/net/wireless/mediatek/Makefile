obj-$(CONFIG_MT7601U)	+= mt7601u/
#ifneq ($(CONFIG_MODEL_RTAX53U),)
#obj-$(CONFIG_MODEL_RTAX53U) += mt_wifi_7915_v7400/mt_wifi_ap/
#endif
ifneq ($(CONFIG_MODEL_RTAX54),)
obj-$(CONFIG_MODEL_RTAX54) += mt_wifi_7915_v7400/mt_wifi_ap/
endif
ifneq ($(CONFIG_MODEL_RT4GAX56),)
obj-$(CONFIG_MODEL_RT4GAX56) += mt_wifi_7915_v7400/mt_wifi_ap/
endif
obj-$(CONFIG_CHIP_MT7615E) += mt7615/5.1.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_RMAC2100) += mt7603/4.1.2.1/mt7603_wifi_ap/
ifneq ($(wildcard drivers/net/wireless/mediatek/mt7915/7.4.0.1/mt_wifi/txpwr/single_sku.c),)
obj-$(CONFIG_CHIP_MT7915) += mt7915/7.4.0.1/mt_wifi_ap/
else ifneq ($(wildcard drivers/net/wireless/mediatek/mt7915_prebuild/mt_wifi_mt7915.o),)
obj-$(CONFIG_CHIP_MT7915) += mt7915_prebuild/
else
obj-$(CONFIG_CHIP_MT7915) += mt7915/7.4.0.0/mt_wifi_ap/
endif

