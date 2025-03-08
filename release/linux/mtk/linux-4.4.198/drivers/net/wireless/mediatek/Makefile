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
obj-$(CONFIG_MODEL_RTAC85U) += mt7615/5.1.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_RTAC85P) += mt7615/5.1.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_R6800) += mt7615/5.1.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_RMAC2100) += mt7615/5.1.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_RMAC2100) += mt7603/4.1.2.1/mt7603_wifi_ap/
ifneq ($(wildcard drivers/net/wireless/mediatek/mt7915/7.4.0.1/mt_wifi/txpwr/single_sku.c),)
obj-$(CONFIG_MODEL_RTAX53U) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_PGBM1) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_JCGQ10PRO) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_H3CTX1801) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_XMCR660X) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_JCGQ20) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_CMCCA9) += mt7915/7.4.0.1/mt_wifi_ap/
else ifneq ($(wildcard drivers/net/wireless/mediatek/mt7915_prebuild/mt_wifi_7915.o),)
obj-$(CONFIG_MODEL_RTAX53U) += mt7915_prebuild/
obj-$(CONFIG_MODEL_PGBM1) += mt7915_prebuild/
obj-$(CONFIG_MODEL_JCGQ10PRO) += mt7915_prebuild/
obj-$(CONFIG_MODEL_H3CTX1801) += mt7915_prebuild/
obj-$(CONFIG_MODEL_XMCR660X) += mt7915_prebuild/
obj-$(CONFIG_MODEL_JCGQ20) += mt7915_prebuild/
obj-$(CONFIG_MODEL_CMCCA9) += mt7915_prebuild/
else
obj-$(CONFIG_MODEL_RTAX53U) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_PGBM1) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_JCGQ10PRO) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_H3CTX1801) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_XMCR660X) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_JCGQ20) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_CMCCA9) += mt7915/7.4.0.0/mt_wifi_ap/
endif

