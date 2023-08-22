# SPDX-License-Identifier: GPL-2.0-only
obj-m := mt_wifi.o
ifeq ($(CONFIG_CFG80211_SUPPORT),y)
mt_wifi-objs := mt_wifi_hostapd_mt7915.o
else
mt_wifi-objs := mt_wifi_mt7915.o
endif


