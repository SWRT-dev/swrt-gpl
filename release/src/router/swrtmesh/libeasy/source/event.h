/*
 * event.h - event handler interfaces definition (API header file).
 *
 * Copyright (C) 2019 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: anjan.chanda@iopsys.eu
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

#ifndef EASYEVENT_H
#define EASYEVENT_H

#include <stdint.h>
#include <stdbool.h>
#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* struct event_response - event response data */
struct event_response {
	uint32_t type;
	uint32_t len;
	uint8_t *data;
};

/* every event type knows what to do with returned data */
struct event_struct {
	char ifname[16];
	char family[32];
	char group[32];
	int (*cb)(struct event_struct *ev);
	int (*filter_event)(struct event_struct *req, char *resp_data);
	void *priv;
	struct event_response resp;

	/* User of this library should not touch
	 * the fields below.
	 *
	 * They can be set by a subsystem's implementation
	 * to process further an event before it reaches
	 * the user.
	 */
	int (*override_cb)(struct event_struct *ev);

	/* File descriptor read monitor. If upper layer
	 * will detect read operation ready, will run
	 * wifi_recv_event()
	 */
	int fd_monitor;
};

enum easy_event_type {
	EASY_EVENT_UNDEFINED,
	EASY_EVENT_DEFAULT = 99,   /* default handler */
};

extern int easy_register_event(struct event_struct *req, void **handle);
extern int easy_unregister_event(void *handle);
extern int easy_recv_event(void *handle);
extern int easy_get_event_fd(void *handle);

#ifdef __cplusplus
}
#endif

#endif /* EASYEVENT_H */
