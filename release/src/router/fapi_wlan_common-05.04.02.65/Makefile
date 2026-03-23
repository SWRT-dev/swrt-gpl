# fapi_wlan source Makefile
ifeq ($(wildcard $(SRCBASE)/router/fapi_wlan_common-05.04.02.65/*.c),)
all:
	-cp -f prebuilt/libfapiwlancommon.so ./
	-cp -f prebuilt/fapi_wlan_beerock_cli ./

install:

clean:
else
PKG_NAME := libfapiwlancommon

opt_no_flags := -Werror -Wcast-qual

ifeq ($(COMMON_WAVE_BUILD),)
	bins := libfapiwlancommon.so
	libfapiwlancommon.so_sources := fapi_wlan.c
	libfapiwlancommon.so_cflags  := -I./include -I$(TOP)/libhelper-2.3.0.16/include -DFILE_EXISTS
	libfapiwlancommon.so_ldflags := -L./ -L$(TOP)/libhelper-2.3.0.16
else
	bins := libfapiwlancommon.so fapi_wlan_cli fapi_wlan_debug_cli fapi_wlan_dbXml fapi_wlan_daemon
	libfapiwlancommon.so_sources := ../hostapd-2.6/src/common/wpa_ctrl.c ../hostapd-2.6/src/utils/os_unix.c fapi_wlan.c fapi_wlan_hostapd_ctrl.c fapi_wlan_aml.c wlan_config_server_api.c fapi_wlan_tools.c fapi_wlan_daemon.c
	libfapiwlancommon.so_cflags  := -I./include -I$(TOP)/libhelper-2.3.0.16/include -I../hostapd-2.6/src/common/ -I../hostapd-2.6/src/utils/ -DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX -I$(STAGINGDIR)/usr/include/
	libfapiwlancommon.so_ldflags := -L./ -L$(TOP)/libhelper-2.3.0.16 -L$(STAGINGDIR)/usr/lib/ -lsafec-1.0

	fapi_wlan_daemon_sources := ../hostapd-2.6/src/common/wpa_ctrl.c ../hostapd-2.6/src/utils/os_unix.c fapi_wlan_daemon.c
	fapi_wlan_daemon_ldflags := -L./ -L$(TOP)/libhelper-2.3.0.16 -lfapiwlancommon -ldl -lhelper -lncurses -lreadline -lrt -L$(STAGINGDIR)/usr/lib/ -lsafec-1.0
	fapi_wlan_daemon_cflags  := -I./include -I$(TOP)/libhelper-2.3.0.16/include -I../hostapd-2.6/src/common/ -I../hostapd-2.6/src/utils/ -DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX -I$(STAGINGDIR)/usr/include/

	fapi_wlan_cli_sources := fapi_wlan_cli.c
	fapi_wlan_cli_ldflags := -L./ -L$(TOP)/libhelper-2.3.0.16 -lfapiwlancommon -ldl -lhelper -lrt -L$(STAGINGDIR)/usr/lib/ -lsafec-1.0
	fapi_wlan_cli_cflags  := -I./include -I$(TOP)/libhelper-2.3.0.16/include -I$(STAGINGDIR)/usr/include/

	fapi_wlan_debug_cli_sources := ../hostapd-2.6/src/common/wpa_ctrl.c ../hostapd-2.6/src/utils/os_unix.c fapi_wlan_debug_cli.c fapi_wlan_band_steering.c
	fapi_wlan_debug_cli_ldflags := -L./ -L$(TOP)/libhelper-2.3.0.16 -lfapiwlancommon -ldl -lhelper -lncurses -lreadline -lrt -L$(STAGINGDIR)/usr/lib/ -lsafec-1.0
	fapi_wlan_debug_cli_cflags  := -I./include -I$(TOP)/libhelper-2.3.0.16/include -I../hostapd-2.6/src/common/ -I../hostapd-2.6/src/utils/ -DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX -I$(STAGINGDIR)/usr/include/

	fapi_wlan_dbXml_sources := fapi_wlan_dbXml.c fapi_wlan_ezxml.c
	fapi_wlan_dbXml_ldflags := -L./ -L$(TOP)/libhelper-2.3.0.16  -lhelper -L$(TOP)/usr/lib/ -lsafec-3.3
	fapi_wlan_dbXml_cflags  := -I./include -I$(TOP)/libhelper-2.3.0.16/include -I$(TOP)/usr/include/
endif

include make.inc
endif
