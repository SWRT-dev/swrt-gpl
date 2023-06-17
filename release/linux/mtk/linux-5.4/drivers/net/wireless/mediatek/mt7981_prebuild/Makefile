# SPDX-License-Identifier: GPL-2.0-only
obj-m := mt_wifi.o mtk_warp.o mtk_warp_proxy.o conninfra.o
#obj-m := conninfra.o
ifeq ($(CONFIG_CFG80211_SUPPORT),y)
mt_wifi-objs := mt_wifi_hostapd_mt7981.o
mtk_warp-objs := mtk_warp_hostapd_mt7981.o
mtk_warp_proxy-objs := mtk_warp_proxy_hostapd_mt7981.o
else
mt_wifi-objs := mt_wifi_mt7981.o
mtk_warp-objs := mtk_warp_mt7981.o
mtk_warp_proxy-objs := mtk_warp_proxy_mt7981.o
endif

conninfra-objs := conninfra_mt7981.o

