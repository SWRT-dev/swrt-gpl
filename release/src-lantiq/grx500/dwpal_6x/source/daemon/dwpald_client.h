/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __WAVE_DWPALD_CLIENT__H__
#define __WAVE_DWPALD_CLIENT__H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <libnl3/netlink/genl/genl.h>
#include <net/if.h>

typedef enum _dwpald_ret {
	DWPALD_SUCCESS,
	DWPALD_DISCONNECTED,
	DWPALD_DWPAL_FAILURE,
	DPWALD_DWPAL_IFACE_IS_DOWN,
	DWPALD_FAILED,
	DWPALD_ERROR,
} dwpald_ret;

#define NUM_OF_FREQUENCIES		32
#define SCAN_PARAM_STRING_LENGTH	64
#define NUM_OF_SSIDS			16
#define SSID_STRING_LENGTH		32
#define DEFAULT_ATTACH_ID		0

typedef struct
{
	int  freq[NUM_OF_FREQUENCIES];
	char ies[SCAN_PARAM_STRING_LENGTH];
	char meshid[SCAN_PARAM_STRING_LENGTH];
	bool lowpri;
	bool flush;
	bool ap_force;
	char ssid[NUM_OF_SSIDS][SSID_STRING_LENGTH];
	bool passive;
} scan_params;

typedef int (*hostap_event_clb)(char *ifname, char *op_code, char *msg, size_t len);
typedef int (*driver_nl_event_clb)(char *ifname, int drv_event_id,
				   void *data, size_t len);
typedef int (*nl80211_cmd_clb)(struct nl_msg *msg, void *arg);
typedef int (*nl80211_event_clb)(struct nl_msg *msg);
typedef int (*termination_cond)(void);

typedef struct _dwpald_hostap_event {
	char *op_code;
	uint8_t op_code_len;
	hostap_event_clb hapd_clb;
} dwpald_hostap_event;

typedef struct _dwpald_driver_event {
	uint32_t nl_id;
	driver_nl_event_clb drv_clb;
} dwpald_driver_nl_event;

#ifdef CONFIG_DWPALD_DEBUG_TOOLS
void dwpald_unit_test_mode(void);

dwpald_ret dwpald_term_daemon(void);

dwpald_ret dwpald_get_connected_clients(char *reply, size_t *reply_len);
#else
static inline void dwpald_unit_test_mode(void) { }
static inline dwpald_ret dwpald_term_daemon(void) { return DWPALD_ERROR; }
#endif

dwpald_ret dwpald_connect(const char *prog_name);

dwpald_ret dwpald_disconnect(void);

dwpald_ret dwpald_start_listener(void);

dwpald_ret dwpald_stop_listener(void);

dwpald_ret dwpald_start_blocked_listen(termination_cond term_cond);

dwpald_ret dwpald_hostap_attach(const char *ifname, size_t num_hap_events,
				const dwpald_hostap_event *hostap_events,
				int fail_on_iface_down);
dwpald_ret dwpald_hostap_attach_ex(const char *ifname, size_t num_hap_events,
				const dwpald_hostap_event *hostap_events,
				hostap_event_clb common_event,
				int fail_on_iface_down);
dwpald_ret dwpald_hostap_attach_with_id(const char *ifname, size_t num_hap_events,
				const dwpald_hostap_event *hostap_events,
				int fail_on_iface_down, unsigned int id);
dwpald_ret dwpald_hostap_attach_ex_with_id(const char *ifname, size_t num_hap_events,
				const dwpald_hostap_event *hostap_events,
				hostap_event_clb common_event,
				int fail_on_iface_down, unsigned int id);

dwpald_ret dwpald_hostap_detach(const char *ifname);
dwpald_ret dwpald_hostap_detach_with_id(const char *ifname, unsigned int id);

dwpald_ret dwpald_nl_drv_attach(size_t num_drv_events,
				const dwpald_driver_nl_event *driver_events,
				nl80211_event_clb nl_event_cb);

dwpald_ret dwpald_nl_drv_attach_with_id(size_t num_drv_events,
				const dwpald_driver_nl_event *driver_events,
				nl80211_event_clb nl_event_cb, unsigned int id);

dwpald_ret dwpald_hostap_cmd(const char *ifname, const char *cmd, size_t len,
			     char *reply, size_t *reply_len);

dwpald_ret dwpald_drv_get(char *ifname, unsigned int command_id, int *cmd_res,
			  void *in_data, size_t in_data_size,
			  void *out_data, size_t *out_data_size);

dwpald_ret dwpald_drv_set(char *ifname, unsigned int command_id, int *cmd_res,
			  const void *vendor_data, size_t vendor_data_size);

dwpald_ret dwpald_nl80211_id_get(int *nl80211_id);

dwpald_ret dwpald_nl80211_cmd_send(struct nl_msg *msg, nl80211_cmd_clb cmd_cb,
				   int *cmd_res, void *cb_arg);

dwpald_ret dwpald_ieee80211_scan_trigger(char *ifname, scan_params *params,
					 int *cmd_res);

dwpald_ret dwpald_ieee80211_scan_dump(char *ifname, nl80211_cmd_clb dump_cb,
				      int *cmd_res, void *cb_arg);

#endif /* __WAVE_DWPALD_CLIENT__H__ */
