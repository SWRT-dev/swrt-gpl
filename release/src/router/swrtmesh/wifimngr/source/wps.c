/*
 * wps.c - provides "wifi.wps" ubus object
 *
 * Based on the questd's implementation of "router.wps", but updated to
 * make use libwifi.so.
 *
 * Copyright (C) 2018 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: sukru.senli@iopsys.eu
 *         Anjan Chanda <anjan.chanda@iopsys.eu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */
#include <stdio.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <uci.h>

#include <easy/easy.h>
#include <wifi.h>
#include "wifimngr.h"


enum {
	ATTR_PIN,
	__ATTR_PIN_MAX,
};

static const struct blobmsg_policy pin_policy[__ATTR_PIN_MAX] = {
	[ATTR_PIN] = { .name = "pin", .type = BLOBMSG_TYPE_STRING },
};

enum {
	WPS_ATTR_IFNAME,
	__ATTR_IFNAME_ONLY,
};

static const struct blobmsg_policy ifname_policy[__ATTR_IFNAME_ONLY] = {
	[WPS_ATTR_IFNAME] = { .name = "ifname", .type = BLOBMSG_TYPE_STRING },
};

enum {
	PIN_SET_IFNAME,
	PIN_SET_PIN,
	__PIN_SET_MAX,
};

static const struct blobmsg_policy pin_set_policy[__PIN_SET_MAX] = {
	[PIN_SET_IFNAME] = { .name = "ifname", .type = BLOBMSG_TYPE_STRING },
	[PIN_SET_PIN] = { .name = "pin", .type = BLOBMSG_TYPE_STRING },
};

enum {
	WPS_START_ATTR_IFNAME,
	WPS_START_ATTR_MODE,
	WPS_START_ATTR_ROLE,
	WPS_START_ATTR_STA_PIN,
	__WPS_START_ATTR_MAX,
};

static const struct blobmsg_policy wps_start_policy[__WPS_START_ATTR_MAX] = {
	[WPS_START_ATTR_IFNAME] = { .name = "ifname", .type = BLOBMSG_TYPE_STRING },
	[WPS_START_ATTR_MODE] = { .name = "mode", .type = BLOBMSG_TYPE_STRING },
	[WPS_START_ATTR_ROLE] = { .name = "role", .type = BLOBMSG_TYPE_STRING },
	[WPS_START_ATTR_STA_PIN] = { .name = "pin", .type = BLOBMSG_TYPE_STRING },
};

char *wifi_get_main_interface(int freq_band, char *ifmain)
{
	char wdevs[16][16];
	enum wifi_bw bw;
	int n, i;

	n = wifimngr_get_wifi_devices(wdevs);
	for (i = 0; i < n; i++) {
		uint32_t ch = 0;

		if (!wifi_get_channel(wdevs[i], &ch, &bw)) {
			if (freq_band == 5 && (ch >= 36 && ch < 200)) {
				strncpy(ifmain, wdevs[i], 15);
				return ifmain;
			} else if (freq_band == 2 && (ch > 0 && ch <= 14)) {
				strncpy(ifmain, wdevs[i], 15);
				return ifmain;
			}
		}
	}

	return NULL;
}

static int wps_status(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	char status[16];
	enum wps_status code = 0;
	struct blob_attr *tb[__ATTR_IFNAME_ONLY];
	char ifname[16] = {0};
	struct blob_buf bb;

	memset(&bb, 0, sizeof(bb));
	blobmsg_parse(ifname_policy, __ATTR_IFNAME_ONLY, tb, blob_data(msg),
								blob_len(msg));

	if (!(tb[WPS_ATTR_IFNAME])) {
		if (wifi_get_main_interface(5, ifname) == NULL)
			return UBUS_STATUS_UNKNOWN_ERROR;
	} else {
		strncpy(ifname, blobmsg_data(tb[WPS_ATTR_IFNAME]), 15);
	}

	if (wifi_get_wps_status(ifname, &code))
		return UBUS_STATUS_UNKNOWN_ERROR;

	switch (code) {
		case WPS_STATUS_INIT:
			strcpy(status, "init");
			break;
		case WPS_STATUS_PROCESSING:
			strcpy(status, "processing");
			break;
		case WPS_STATUS_SUCCESS:
			strcpy(status, "success");
			break;
		case WPS_STATUS_FAIL:
			strcpy(status, "fail");
			break;
		case WPS_STATUS_TIMEOUT:
			strcpy(status, "timeout");
			break;
		default:
			strcpy(status, "unknown");
			break;
	}

	blob_buf_init(&bb, 0);
	blobmsg_add_u32(&bb, "code", code);
	blobmsg_add_string(&bb, "status", status);
	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

int wps_start(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	struct blob_attr *tb[__WPS_START_ATTR_MAX];
	struct wps_param wps = {
		.role = WPS_REGISTRAR,
		.method = WPS_METHOD_PBC
	};
	char ifname[16] = {0};
	char role[16] = {0};
	char mode[8] = {0};
	unsigned long pin;

	blobmsg_parse(wps_start_policy, __WPS_START_ATTR_MAX,
					tb, blob_data(msg), blob_len(msg));

	if (!(tb[WPS_START_ATTR_IFNAME])) {
		/* if interface not provided, assume 5Ghz main interface */
		if (wifi_get_main_interface(5, ifname) == NULL)
			return UBUS_STATUS_UNKNOWN_ERROR;
	} else {
		strncpy(ifname, blobmsg_data(tb[WPS_START_ATTR_IFNAME]), 15);
	}

	if (!(tb[WPS_START_ATTR_MODE])) {
		wps.method = WPS_METHOD_PBC;
	} else {
		strncpy(mode, blobmsg_data(tb[WPS_START_ATTR_MODE]), 7);
		if (!strcasecmp(mode, "pin"))
			wps.method = WPS_METHOD_PIN;
		else if (!strcasecmp(mode, "pbc"))
			wps.method = WPS_METHOD_PBC;
		else
			return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (!(tb[WPS_START_ATTR_ROLE])) {
		wps.role = WPS_REGISTRAR;
	} else {
		strncpy(role, blobmsg_data(tb[WPS_START_ATTR_ROLE]), 10);
		if (!strcasecmp(role, "registrar"))
			wps.role = WPS_REGISTRAR;
		else if (!strcasecmp(role, "enrollee"))
			wps.role = WPS_ENROLLEE;
		else if (!strcasecmp(role, "bsta"))
			wps.role = WPS_ENROLLEE_BSTA;
		else
			return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (wps.method == WPS_METHOD_PIN) {
		if (!(tb[WPS_START_ATTR_STA_PIN])) {
			return UBUS_STATUS_INVALID_ARGUMENT;
		} else {
			pin = strtoul(blobmsg_data(tb[WPS_START_ATTR_STA_PIN]), NULL, 10);
			if (!wifi_is_wps_pin_valid(pin))
				return UBUS_STATUS_INVALID_ARGUMENT;
			wps.pin = pin;
		}
	}

	if ((wifi_start_wps(ifname, wps) == 0))
		return UBUS_STATUS_OK;

	return UBUS_STATUS_UNKNOWN_ERROR;
}

static int wps_genpin(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	unsigned long PIN;
	char local_devPwd[32];
	struct blob_buf bb;

	memset(&bb, 0, sizeof(bb));

	wifi_generate_wps_pin(&PIN);
	sprintf(local_devPwd, "%08u", (unsigned int)PIN);
	local_devPwd[8] = '\0';

	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb, "pin", local_devPwd);
	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return UBUS_STATUS_OK;
}

static int wps_checkpin(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	unsigned long pin;
	bool valid = false;
	struct blob_attr *tb[__ATTR_PIN_MAX];
	struct blob_buf bb;

	memset(&bb, 0, sizeof(bb));
	blobmsg_parse(pin_policy, __ATTR_PIN_MAX, tb, blob_data(msg),
								blob_len(msg));

	if (!(tb[ATTR_PIN]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	pin = strtoul(blobmsg_get_string(tb[ATTR_PIN]), NULL, 10);
	valid = wifi_is_wps_pin_valid(pin);

	blob_buf_init(&bb, 0);
	blobmsg_add_u8(&bb, "valid", valid);
	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return UBUS_STATUS_OK;
}

int wps_set_ap_pin(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	struct blob_attr *tb[__PIN_SET_MAX];
	unsigned long pin;
	char ifname[16] = {0};

	blobmsg_parse(pin_set_policy, __PIN_SET_MAX, tb, blob_data(msg),
								blob_len(msg));

	if (!(tb[PIN_SET_IFNAME])) {
		if (wifi_get_main_interface(5, ifname) == NULL)
			return UBUS_STATUS_UNKNOWN_ERROR;
	} else {
		strncpy(ifname, blobmsg_data(tb[PIN_SET_IFNAME]), 15);
	}

	if (!(tb[PIN_SET_PIN]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	pin = strtoul(blobmsg_data(tb[PIN_SET_PIN]), NULL, 10);
	if (!wifi_is_wps_pin_valid(pin))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (wifi_set_wps_pin(ifname, pin))
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

static int wps_show_ap_pin(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	char pinstr[9] = {0};
	unsigned long pin;
	struct blob_attr *tb[__ATTR_IFNAME_ONLY];
	char ifname[16] = {0};
	struct blob_buf bb;

	memset(&bb, 0, sizeof(bb));
	blobmsg_parse(ifname_policy, __ATTR_IFNAME_ONLY, tb, blob_data(msg),
								blob_len(msg));

	if (!(tb[WPS_ATTR_IFNAME])) {
		if (wifi_get_main_interface(5, ifname) == NULL)
			return UBUS_STATUS_UNKNOWN_ERROR;
	} else {
		strncpy(ifname, blobmsg_data(tb[WPS_ATTR_IFNAME]), 15);
	}

	if (wifi_get_wps_pin(ifname, &pin))
		return UBUS_STATUS_UNKNOWN_ERROR;

	blob_buf_init(&bb, 0);
	sprintf(pinstr, "%08lu", pin);
	blobmsg_add_string(&bb, "pin", pinstr);
	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return UBUS_STATUS_OK;
}

int wps_stop(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	char ifname[16] = {0};
	struct blob_attr *tb[__ATTR_IFNAME_ONLY];
	struct wifimngr_iface ifs[WIFI_IF_MAX_NUM];
	int i, num_ifs = 0;

	blobmsg_parse(ifname_policy, __ATTR_IFNAME_ONLY, tb, blob_data(msg),
								blob_len(msg));

	if (tb[WPS_ATTR_IFNAME]) {
		strncpy(ifname, blobmsg_data(tb[WPS_ATTR_IFNAME]), 15);
		wifi_stop_wps(ifname);
	} else {
		num_ifs = wifimngr_get_wifi_interfaces(ifs);
		for (i = 0; i < num_ifs; i++) {
			wifi_stop_wps(ifs[i].iface);
		}
	}

	return 0;
}

#define MAX_WPS_METHODS		10
int add_wps_methods(struct ubus_object *wifix)
{
	int n_methods = 0;
	struct ubus_method *wps_methods;

	wps_methods = calloc(MAX_WPS_METHODS, sizeof(struct ubus_method));
	if (!wps_methods)
		return -ENOMEM;

	/* Usage -
	 * start {"mode":"pbc|pin", "role":"enrollee|registrar", "pin":"pin"}
	 * where default mode = pbc and default role = registrar.
	 * If role = registrar, and mode = pin, then enrollee's pin
	 * should be provided through the 'pin' attribute.
	 */
	UBUS_METHOD_ADD(wps_methods, n_methods,
		UBUS_METHOD("start", wps_start, wps_start_policy));

	UBUS_METHOD_ADD(wps_methods, n_methods,
		UBUS_METHOD_NOARG("stop", wps_stop));


	UBUS_METHOD_ADD(wps_methods, n_methods,
			UBUS_METHOD("status", wps_status, ifname_policy));

	UBUS_METHOD_ADD(wps_methods, n_methods,
			UBUS_METHOD_NOARG("generate_pin", wps_genpin));

	UBUS_METHOD_ADD(wps_methods, n_methods,
			UBUS_METHOD("validate_pin", wps_checkpin, pin_policy));

	UBUS_METHOD_ADD(wps_methods, n_methods,
		UBUS_METHOD("showpin", wps_show_ap_pin, ifname_policy));

	UBUS_METHOD_ADD(wps_methods, n_methods,
		UBUS_METHOD("setpin", wps_set_ap_pin, pin_set_policy));


	wifix->methods = wps_methods;
	wifix->n_methods = n_methods;

	return 0;
}
