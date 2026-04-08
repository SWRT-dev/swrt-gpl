# fapi_wlan source Makefile

PKG_NAME := dwpal
#IWLWAV_HOSTAP_DIR := ../iwlwav-hostap-2.6

opt_no_flags := -Werror -Wcast-qual

LOG_CFLAGS := -DPACKAGE_ID=\"DWPALWLAN\" -DLOGGING_ID="dwpal_6x" -DLOG_LEVEL=7 -DLOG_TYPE=1

bins := libdwpal.so dwpal_cli
libdwpal.so_sources := dwpal.c dwpal_ext.c $(IWLWAV_HOSTAP_DIR)/src/common/wpa_ctrl.c $(IWLWAV_HOSTAP_DIR)/src/utils/os_unix.c
libdwpal.so_cflags  := -I./include -I$(IWLWAV_HOSTAP_DIR)/src/common/ -I$(IWLWAV_HOSTAP_DIR)/src/utils/ -DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX -I$(STAGING_DIR)/usr/include/libnl3/ -I$(IWLWAV_HOSTAP_DIR)/src/drivers/
libdwpal.so_ldflags := -L./ -L$(STAGING_DIR)/opt/lantiq/lib/ -lnl-genl-3

dwpal_cli_sources := $(IWLWAV_HOSTAP_DIR)/src/common/wpa_ctrl.c $(IWLWAV_HOSTAP_DIR)/src/utils/os_unix.c dwpal_cli.c stats.c
dwpal_cli_ldflags := -L./ -ldwpal -ldl -lncurses -lreadline -lrt -L$(STAGING_DIR)/usr/sbin/ -lpthread -lnl-genl-3 -lnl-3
dwpal_cli_cflags  := -I./include -I$(IWLWAV_HOSTAP_DIR)/src/common/ -I$(IWLWAV_HOSTAP_DIR)/src/utils/ -DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX -I$(STAGING_DIR)/usr/include/ -I$(IWLWAV_HOSTAP_DIR)/src/drivers/ -I$(STAGING_DIR)/usr/include/libnl3/

include make.inc
