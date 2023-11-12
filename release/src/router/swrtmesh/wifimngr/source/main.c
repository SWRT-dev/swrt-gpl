/*
 * main.c - WiFi manager main
 *
 * Copyright (C) 2019 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: Anjan Chanda <anjan.chanda@iopsys.eu>
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
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <json-c/json.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>

#include <easy/easy.h>
#include "wifimngr.h"
#if defined(RTCONFIG_SWRTMESH)
#include <swrtmesh-utils.h>
#define TYPEDEF_BOOL
#include <shared.h>
#undef TYPEDEF_BOOL
#endif
#if defined(RTCONFIG_SWRTMESH)
#define DEFAULT_WIFI_EVMAP_JSON_FILE	"/jffs/swrtmesh/wifi.json"
#else
#define DEFAULT_WIFI_EVMAP_JSON_FILE	"/etc/wifi.json"
#endif
static struct wifimngr *this = NULL;

static int wifimngr_cmd_main(struct ubus_context *ctx)
{
	int ret;
	char radios[16][WIFI_DEV_MAX_NUM];
	struct wifimngr_iface ifs[WIFI_IF_MAX_NUM];
	int i;
	int num_radios;
	int num_ifs;
	struct wifimngr *wmngr;

	ret = wifimngr_init(&wmngr);
	if (ret != 0)
		return ret;

	this = wmngr;
	ret = wifimngr_add_object(wmngr, ctx, "wifi", add_wifi_methods);
	if (ret) {
		wifimngr_err("Failed to add 'wifi' ubus object: %s\n",
				ubus_strerror(ret));
				goto out_exit;
	}

	ret = wifimngr_add_object(wmngr, ctx, "wifi.wps", add_wps_methods);
	if (ret) {
		wifimngr_err("Failed to add 'wifi.wps' ubus object: %s\n",
				ubus_strerror(ret));
				goto out_exit;
	}

	/* enumerate radios/interfaces from wireless config */
	num_radios = wifimngr_get_wifi_devices(radios);
	if (num_radios < 0) {
		wifimngr_err("get_wifi_devices() ret = %d\n", ret);
		goto out_exit;
	}

	for (i = 0; i < num_radios; i++) {
		ret = wifimngr_add_radio_object(wmngr, ctx, radios[i]);
		if (ret) {
			wifimngr_err("Failed to add 'wifi.radio' ubus object: %s\n",
				ubus_strerror(ret));
			goto out_exit;
		}
	}

	num_ifs = wifimngr_get_wifi_interfaces(ifs);
	if (num_ifs < 0) {
		wifimngr_err("get_wifi_interfaces() ret = %d\n", ret);
		goto out_exit;
	}

	for (i = 0; i < num_ifs; i++) {
		if (ifs[i].disabled)
			continue;

		ret = wifimngr_add_interface_object(wmngr, ctx, &ifs[i]);
		if (ret) {
			wifimngr_err("Failed to add 'wifi.ap' ubus object: %s\n",
				ubus_strerror(ret));
			goto out_exit;
		}
	}

	return 0;

out_exit:
	return -1;
}

#if defined(RTCONFIG_SWRTMESH)
static void gen_evmap_file(const char *path)
{
	struct json_object *root = json_object_new_object(), *tmpobj = json_object_new_object();
	struct json_object *arrayObj = json_object_new_array(), *grpobj = json_object_new_array();
	json_object_object_add(tmpobj, "type", json_object_new_string("wifi-event"));
#if defined(RTCONFIG_RALINK)
	json_object_object_add(tmpobj, "ifname", json_object_new_string("ra*"));
#elif defined(RTCONFIG_QCA)
	json_object_object_add(tmpobj, "ifname", json_object_new_string("wifi*"));
#elif defined(RTCONFIG_LANTIQ)
	json_object_object_add(tmpobj, "ifname", json_object_new_string("wlan*"));
#elif defined(RTCONFIG_BCMARM)
	json_object_object_add(tmpobj, "ifname", json_object_new_string("wl*"));
#endif
	json_object_object_add(tmpobj, "name", json_object_new_string("nl80211"));
	json_object_object_add(tmpobj, "family", json_object_new_string("nl80211"));
	json_object_array_add(grpobj, json_object_new_string("scan"));
#if defined(RTCONFIG_BCMARM)
	json_object_array_add(grpobj, json_object_new_string("notify"));
#elif 0
	json_object_array_add(grpobj, json_object_new_string("config"));
	json_object_array_add(grpobj, json_object_new_string("mlme"));
	json_object_array_add(grpobj, json_object_new_string("vendor"));
#endif
	json_object_object_add(tmpobj, "group", grpobj);
	json_object_array_add(arrayObj, tmpobj);
	json_object_object_add(root, "events", arrayObj);
	json_object_to_file_ext(path, root, JSON_C_TO_STRING_PRETTY);
	json_object_put(root);	
}
#endif

int wifimngr_event_main(void *ctx, const char *evmap_file)
{
	struct json_object *jevs, *jev_array;
	int len = 0;
	int ret = 0;
	int i, j;
#if defined(RTCONFIG_SWRTMESH)
	if(!f_exists(evmap_file))
		gen_evmap_file(evmap_file);
#endif
	wifimngr_event_init();

	/* read from json file about events to listen for */
	jevs = json_object_from_file(evmap_file);
	if (!jevs) {
		wifimngr_err("Failed to open '%s'\n", evmap_file);
		return -1;
	}

	if (!json_object_is_type(jevs, json_type_object))
		goto out_json;

	json_object_object_get_ex(jevs, "events", &jev_array);
	len = json_object_array_length(jev_array);
	for (i = 0; i < len; i++) {
		struct json_object *jev;
		struct json_object *jev_family, *jev_ifname;
		struct json_object *jev_group_array, *jev_grp;
		const char *ifname, *family, *group;
		int grplen = 0;

		jev = json_object_array_get_idx(jev_array, i);

		json_object_object_get_ex(jev, "ifname", &jev_ifname);
		ifname = json_object_get_string(jev_ifname);

		json_object_object_get_ex(jev, "family", &jev_family);
		family = json_object_get_string(jev_family);

		json_object_object_get_ex(jev, "group", &jev_group_array);
		grplen = json_object_array_length(jev_group_array);
		for (j = 0; j < grplen; j++) {
			jev_grp = json_object_array_get_idx(jev_group_array, j);
			group = json_object_get_string(jev_grp);
			wifimngr_dbg("Setup event (%s, %s, %s)\n",
						ifname, family, group);
			ret = wifimngr_events_register(ctx, ifname, family, group);
			if (ret < 0)
				wifimngr_err("event_register failed %d\n", ret);
		}
	}

out_json:
	json_object_put(jevs);
	return ret;
}

void wifimngr_clean_exit(void)
{
	wifimngr_event_exit();
	wifimngr_exit(this);
	exit(0);
}

static void wifimngr_sighandler(int sig)
{
	switch (sig) {
	case SIGTERM:
	case SIGINT:
		wifimngr_clean_exit();
		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	const char *evmap_file = DEFAULT_WIFI_EVMAP_JSON_FILE;
	struct ubus_context *ctx = NULL;
	const char *ubus_socket = NULL;
	int ret;
	int ch;

	while ((ch = getopt(argc, argv, "vhs:e:")) != -1) {
		switch (ch) {
		case 'v':
			wifimngr_version();
			exit(0);
		case 's':
			ubus_socket = optarg;
			break;
		case 'e':
			evmap_file = optarg;
			break;
		default:
			break;
		}
	}

	uloop_init();
	ctx = ubus_connect(ubus_socket);
	if (!ctx) {
		wifimngr_err("Failed to connect to ubus\n");
		return -1;
	}

	ubus_add_uloop(ctx);

	wifimngr_event_main(ctx, evmap_file);

	ret = wifimngr_cmd_main(ctx);
	if (ret) {
		wifimngr_err("Failed to add wifimngr ubus objects! aborting..\n");
		wifimngr_exit(this);
		return 0;
	}

	set_sighandler(SIGPIPE, SIG_DFL);
	set_sighandler(SIGINT, wifimngr_sighandler);
	set_sighandler(SIGTERM, wifimngr_sighandler);

	uloop_run();
	ubus_free(ctx);
	uloop_done();

	return 0;
}
