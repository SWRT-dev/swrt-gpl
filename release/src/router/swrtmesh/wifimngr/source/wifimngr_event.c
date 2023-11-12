/*
 * wifimngr_event.c - handle wifi events
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
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <net/if.h>
#include <sys/types.h>
#include <dirent.h>

#include <libubox/blobmsg_json.h>
#include "libubus.h"

#include <easy/easy.h>
#include <wifi.h>
#include "wifimngr.h"

#define MAX_EVENT_RESPONSE_LEN		2400

static struct list_head event_list;

struct wifimngr_event {
	struct uloop_fd uloop_fd;
	struct list_head list;
	struct event_struct event;
	void *handle;
};

static int wifimngr_ubus_event(struct ubus_context *ctx, char *message)
{
	char event[32] = {0};
	char data[MAX_EVENT_RESPONSE_LEN] = {0};
	struct blob_buf b;

	if (WARN_ON(strlen(message) > sizeof(data)))
		return 0;

	if (WARN_ON(sscanf(message, "%31s '%2399[^\n]'", event, data) != 2))
		return 0;

	/* ignore non-wifi events */
	if (!strstr(event, "wifi."))
		return 0;

	memset(&b, 0, sizeof(b));
	blob_buf_init(&b, 0);

	if (!blobmsg_add_json_from_string(&b, data)) {
		wifimngr_err("Failed to parse message data: %s\n", data);
		return -1;
	}

	ubus_send_event(ctx, event, b.head);
	blob_buf_free(&b);

	return 0;
}

int wifimngr_event_cb(struct event_struct *e)
{
	struct ubus_context *pctx = (struct ubus_context *)e->priv;
	struct event_response *resp = &e->resp;
	char evtbuf[MAX_EVENT_RESPONSE_LEN] = {0};

	switch (resp->type) {
	case WIFI_EVENT_SCAN_START:
		snprintf(evtbuf, MAX_EVENT_RESPONSE_LEN - 1,
			 "wifi.radio '{\"ifname\":\"%s\", \"event\":\"scan_started\"}'",
			 e->ifname);
		wifimngr_ubus_event(pctx, evtbuf);
		break;
	case WIFI_EVENT_SCAN_END:
		snprintf(evtbuf, MAX_EVENT_RESPONSE_LEN - 1,
			 "wifi.radio '{\"ifname\":\"%s\", \"event\":\"scan_finished\"}'",
			 e->ifname);
		wifimngr_ubus_event(pctx, evtbuf);
		break;
	case WIFI_EVENT_SCAN_ABORT:
		snprintf(evtbuf, MAX_EVENT_RESPONSE_LEN - 1,
			 "wifi.radio '{\"ifname\":\"%s\", \"event\":\"scan_aborted\"}'",
			 e->ifname);
		wifimngr_ubus_event(pctx, evtbuf);
		break;
	default:
		/* last resort - the default event handler */
		if (WARN_ON(resp->len >= sizeof(evtbuf)))
			break;

		strncpy(evtbuf, (char *)resp->data, resp->len);
		wifimngr_ubus_event(pctx, evtbuf);
		break;
	}

	return 0;
}

static void wifimngr_event_uloop_cb(struct uloop_fd *fd, unsigned int events)
{
	struct wifimngr_event *ev = (void *) fd;
	int rv;

	for (;;) {
		rv = wifi_recv_event(ev->event.ifname, ev->handle);
		if (rv < 0)
			break;
	}

	if (rv == -ENETDOWN) {
		wifimngr_dbg("%s read error ENETDOWN - rearm uloop fd monitor\n", ev->event.ifname);
		uloop_fd_delete(&ev->uloop_fd);
		uloop_fd_add(&ev->uloop_fd, ULOOP_READ | ULOOP_EDGE_TRIGGER);
	}
}

void wifimngr_event_init(void)
{
	INIT_LIST_HEAD(&event_list);
}

void wifimngr_event_exit(void)
{
	struct wifimngr_event *ev, *tmp;

	list_for_each_entry_safe(ev, tmp, &event_list, list) {
		wifimngr_dbg("remove %s %s %s\n", ev->event.ifname, ev->event.family, ev->event.group);
		uloop_fd_delete(&ev->uloop_fd);
		wifi_unregister_event(ev->event.ifname, ev->handle);
		list_del(&ev->list);
		free(ev->event.resp.data);
		free(ev);
	}

	list_del_init(&event_list);
}

int wifimngr_events_register(void *ctx, const char *ifname,
			     const char *family, const char *group)
{
	char prefix[64] = {};
	struct dirent *p;
	char *ptr;
	DIR *d;


	if (!ifname)
		return wifimngr_event_register(ctx, NULL, family, group);

	ptr = strstr(ifname, "*");
	if (!ptr)
		return wifimngr_event_register(ctx, ifname, family, group);

	strncpy(prefix, ifname, sizeof(prefix));
	prefix[ptr - ifname] = '\0';

	wifimngr_dbg("events_register prefix %s\n", prefix);

	d = opendir("/sys/class/net");
	if (WARN_ON(!d))
		return -1;

	while ((p = readdir(d))) {
		if (!strcmp(p->d_name, ""))
			continue;
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;
		if (strncmp(p->d_name, prefix, strlen(prefix)))
			continue;

		WARN_ON(wifimngr_event_register(ctx, p->d_name, family, group));
	}

	closedir(d);
	return 0;
}

int wifimngr_event_register(void *ctx, const char *ifname,
			    const char *family, const char *group)
{
	//struct ubus_context *pctx = (struct ubus_context *)ctx;
	struct wifimngr_event *ev;

	list_for_each_entry(ev, &event_list, list) {
		if (strcmp(family, ev->event.family))
			continue;
		if (strcmp(group, ev->event.group))
			continue;
		if (ifname && strcmp(ifname, ev->event.ifname))
			continue;

		/* Already registered */
		wifimngr_dbg("%s %s %s already registered\n",
			ev->event.ifname, ev->event.family, ev->event.group);
		return 0;
	}

	ev = calloc(1, sizeof(*ev));
	if (!ev) {
		wifimngr_err("%s %s %s calloc(ev) failed\n", ifname, family, group);
		return -1;
	}

	if (ifname)
		strncpy(ev->event.ifname, ifname, 16);
	strncpy(ev->event.family, family, 32);
	strncpy(ev->event.group, group, 32);
	ev->event.priv = ctx;
	ev->event.cb = wifimngr_event_cb;
	/* setup response buffer */
	ev->event.resp.data = calloc(MAX_EVENT_RESPONSE_LEN, sizeof(uint8_t));
	if (ev->event.resp.data == NULL) {
		free(ev);
		wifimngr_err("%s %s %s calloc(resp) failed\n",
			ev->event.ifname, ev->event.family, ev->event.group);
		return -1;
	}

	ev->event.resp.len = ev->event.resp.data ? MAX_EVENT_RESPONSE_LEN : 0;

	if (wifi_register_event((char *)ifname, &ev->event, &ev->handle)) {
		free(ev->event.resp.data);
		free(ev);
		wifimngr_err("%s %s %s wifi_register_revent() failed\n",
			ev->event.ifname, ev->event.family, ev->event.group);
		return -1;
	}

	if (ev->event.fd_monitor) {
		ev->uloop_fd.fd = ev->event.fd_monitor;
		ev->uloop_fd.cb = wifimngr_event_uloop_cb;

		if (uloop_fd_add(&ev->uloop_fd, ULOOP_READ | ULOOP_EDGE_TRIGGER)) {
			wifimngr_err("uloop_fd_add(%d) failed\n", ev->uloop_fd.fd);
			wifi_unregister_event(ev->event.ifname, ev->handle);
			free(ev->event.resp.data);
			free(ev);
			return -1;
		}
	}

	list_add_tail(&ev->list, &event_list);
	return 0;
}

void wifimngr_event_unregister(const char *ifname)
{
	struct wifimngr_event *ev, *tmp;

	if (!ifname)
		return;

	list_for_each_entry_safe(ev, tmp, &event_list, list) {
		if (strcmp(ifname, ev->event.ifname))
			continue;
		wifimngr_dbg("remove %s %s %s\n", ev->event.ifname, ev->event.family, ev->event.group);
		uloop_fd_delete(&ev->uloop_fd);
		wifi_unregister_event(ifname, ev->handle);
		list_del(&ev->list);
		free(ev->event.resp.data);
		free(ev);
	}
}
