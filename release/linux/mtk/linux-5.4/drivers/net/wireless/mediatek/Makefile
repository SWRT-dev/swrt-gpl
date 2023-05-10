# SPDX-License-Identifier: GPL-2.0-only
obj-$(CONFIG_MT7601U)	+= mt7601u/
#obj-$(CONFIG_MT76_CORE)	+= mt76/
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
ifneq ($(wildcard drivers/net/wireless/mediatek/mt7915/7.4.0.1/mt_wifi_ap/Makefile),)
obj-$(CONFIG_MODEL_RTAX53U) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_PGBM1) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_JCGQ10PRO) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_H3CTX1801) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_XMCR660X) += mt7915/7.4.0.1/mt_wifi_ap/
obj-$(CONFIG_MODEL_JCGQ20) += mt7915/7.4.0.1/mt_wifi_ap/
else ifneq ($(wildcard drivers/net/wireless/mediatek/mt7915_prebuild/mt_wifi_7915.o),)
obj-$(CONFIG_MODEL_RTAX53U) += mt7915_prebuild/
obj-$(CONFIG_MODEL_PGBM1) += mt7915_prebuild/
obj-$(CONFIG_MODEL_JCGQ10PRO) += mt7915_prebuild/
obj-$(CONFIG_MODEL_H3CTX1801) += mt7915_prebuild/
obj-$(CONFIG_MODEL_XMCR660X) += mt7915_prebuild/
obj-$(CONFIG_MODEL_JCGQ20) += mt7915_prebuild/
else
obj-$(CONFIG_MODEL_RTAX53U) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_PGBM1) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_JCGQ10PRO) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_H3CTX1801) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_XMCR660X) += mt7915/7.4.0.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_JCGQ20) += mt7915/7.4.0.0/mt_wifi_ap/
endif
ifneq ($(wildcard drivers/net/wireless/mediatek/mt798x/7.6.7.0/mt_wifi/chips/mt7986.c),)
obj-$(CONFIG_MODEL_RMAX6000) += mt798x/7.6.7.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_RMAX6000) += mt798x/7.6.7.0/warp_driver/warp/
obj-$(CONFIG_MODEL_SWRT360T7) += mt798x/7.6.7.0/mt_wifi_ap/
obj-$(CONFIG_MODEL_SWRT360T7) += mt798x/7.6.7.0/warp_driver/warp/
else ifneq ($(wildcard drivers/net/wireless/mediatek/mt7986_prebuild/mt_wifi_7986.o),)
obj-$(CONFIG_MODEL_RMAX6000) += mt7986_prebuild/
else ifneq ($(wildcard drivers/net/wireless/mediatek/mt7981_prebuild/mt_wifi_7981.o),)
obj-$(CONFIG_MODEL_SWRT360T7) += mt7981_prebuild/
else
obj-$(CONFIG_MODEL_RMAX6000) += mt7986/mt_wifi_ap/
obj-$(CONFIG_MODEL_RMAX6000) += mt7986/warp_driver/warp/
obj-$(CONFIG_MODEL_SWRT360T7) += mt7986/mt_wifi_ap/
obj-$(CONFIG_MODEL_SWRT360T7) += mt7986/warp_driver/warp/
endif
ifneq ($(wildcard drivers/net/wireless/mediatek/conninfra/Makefile),)
obj-$(CONFIG_MODEL_RMAX6000) += conninfra/
obj-$(CONFIG_MODEL_SWRT360T7) += conninfra/
else
obj-$(CONFIG_MODEL_RMAX6000) += mt7986_prebuild/
obj-$(CONFIG_MODEL_SWRT360T7) += mt7981_prebuild/
endif

