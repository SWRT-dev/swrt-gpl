# fapi_wlan source Makefile

PKG_NAME := dwpal
#IWLWAV_HOSTAP_DIR := ../iwlwav-hostap-2.6

#opt_no_flags := -Werror -Wcast-qual
opt_flags := -O2 -Wall -Werror

LOG_CFLAGS := -DPACKAGE_ID=\"DWPALWLAN\" -DLOGGING_ID="dwpal_6x" -DLOG_LEVEL=7 -DLOG_TYPE=1

bins := libdwpal.so libwv_core.so libwave_ipcc.a libwave_ipcs.a dwpal_cli dwpal_daemon libdwpald_client.so dwpald_client_ref test_lib_wv_ipc test_dwpal
libdwpal.so_sources := dwpal.c dwpal_ext.c $(IWLWAV_HOSTAP_DIR)/src/common/wpa_ctrl.c $(IWLWAV_HOSTAP_DIR)/src/utils/os_unix.c logs.c
libdwpal.so_cflags  := -I./include -I$(IWLWAV_HOSTAP_DIR)/src/common/ -I$(IWLWAV_HOSTAP_DIR)/src/utils/ -DCONFIG_ALLOW_SYSLOG -DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX -I$(STAGING_DIR)/usr/include/libnl3/ -I$(IWLWAV_HOSTAP_DIR)/src/drivers/
libdwpal.so_ldflags := -L./ -L$(STAGING_DIR)/opt/lantiq/lib/ -lnl-genl-3

dwpal_cli_sources := $(IWLWAV_HOSTAP_DIR)/src/common/wpa_ctrl.c $(IWLWAV_HOSTAP_DIR)/src/utils/os_unix.c dwpal_cli.c stats.c
dwpal_cli_ldflags := -L./ -lwave_ipcs -lwave_ipcc -lwv_core -ldwpal -ldl -lncurses -lreadline -lrt -L$(STAGING_DIR)/usr/sbin/ -lpthread -lnl-genl-3 -lnl-3
dwpal_cli_cflags  := -I./include -I./wv_ipc/ -I$(IWLWAV_HOSTAP_DIR)/src/common/ -I$(IWLWAV_HOSTAP_DIR)/src/utils/ -DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX -I$(STAGING_DIR)/usr/include/ -I$(IWLWAV_HOSTAP_DIR)/src/drivers/ -I$(STAGING_DIR)/usr/include/libnl3/

libwv_core.so_sources := wv_ipc/linked_list.c wv_ipc/obj_pool.c wv_ipc/work_serializer.c wv_ipc/logs.c
libwv_core.so_cflags := -DCONFIG_ALLOW_SYSLOG

# wave ipc libraries
IPC_SHARED_SRCS := wv_ipc/wave_ipc_core.c

libwave_ipcc.a_sources := wv_ipc/wave_ipc_client.c $(IPC_SHARED_SRCS)

libwave_ipcs.a_sources := wv_ipc/wave_ipc_server.c $(IPC_SHARED_SRCS)

test_lib_wv_ipc_sources := unit_tests/test_lib_wv_ipc.c unit_tests/test_list.c unit_tests/test_obj_pool.c unit_tests/test_work_serializer.c unit_tests/test_ipc_core.c unit_tests/test_ipc_client.c unit_tests/test_ipc_server.c
test_lib_wv_ipc_cflags  := -I./wv_ipc/
test_lib_wv_ipc_ldflags := -L./ -lwave_ipcc -lwave_ipcs -lwv_core

test_dwpal_sources := unit_tests/test_dwpal.c unit_tests/test_dwpal_daemon.c unit_tests/test_dwpal_ext.c
test_dwpal_cflags  := -I./daemon/ -I./include/ -I$(STAGING_DIR)/usr/include/libnl3/
test_dwpal_ldflags := -L./ -ldwpald_client -lwave_ipcs -lwv_core -ldwpal -lpthread -lnl-genl-3 -lnl-3

dwpal_daemon_sources := daemon/dwpal_daemon.c daemon/iface_manager.c daemon/hostap_iface.c daemon/nl_iface.c
dwpal_daemon_cflags  := -I./wv_ipc/ -I./include/ -I$(STAGING_DIR)/usr/include/libnl3/
dwpal_daemon_ldflags := -L./ -lwave_ipcs -lwv_core -ldwpal -lpthread -lnl-genl-3 -lnl-3

libdwpald_client.so_sources := daemon/dwpald_client.c daemon/dwpald_hostap_parse.c
libdwpald_client.so_cflags  := -I./wv_ipc/ -I./include/ -I$(STAGING_DIR)/usr/include/libnl3/
libdwpald_client.so_ldflags := -L./ -lwave_ipcc -lpthread -lnl-genl-3 -lnl-3

dwpald_client_ref_sources := daemon/dwpald_client_reference.c
dwpald_client_ref_cflags  := -I./wv_ipc/ -I./include/ -I$(STAGING_DIR)/usr/include/libnl3/
dwpald_client_ref_ldflags := -L./ -ldwpald_client -lwv_core -lpthread -lnl-genl-3 -lnl-3

include make.inc
