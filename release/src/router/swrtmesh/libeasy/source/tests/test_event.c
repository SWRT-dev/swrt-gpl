/*
 * test_event.c - read test event definition file and subscribe to events.
 *
 * Copyright (C) 2020 iopsys Software Solutions AB. All rights reserved.
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
#include <signal.h>
#include <sys/types.h>
#include <sys/select.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <json-c/json.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>

#include "easy.h"

/* Test events' definition file contains upto 4 valid events block */
#define MAX_EVENTS	4

/* Test app private data */
struct private {
	int maxfds;
	int numevs;
	fd_set rfds;
	char ifname[16];
	int fd[MAX_EVENTS];
	void *handle[MAX_EVENTS];
};

/* used only for freeing struct private during program exit */
struct private *g_private = NULL;

/* Here event response is assumed to be of max 512 bytes.
 * It can be more. Allocate event response buffer accordingly during
 * registration.
 */
#define MAX_EVENT_RESPONSE_LEN		512

int test_event_cb(struct event_struct *e)
{
	/* struct private *priv = (struct private *)e->priv; */
	struct event_response *resp = &e->resp;
	char evtbuf[512] = {0};

	switch (resp->type) {
	case EASY_EVENT_DEFAULT:
	default:
		strncpy(evtbuf, (char *)resp->data, resp->len);
		libeasy_dbg("%s\n", evtbuf);
		break;
	}

	return 0;
}

int test_recv_event(struct private *priv)
{
	int err;
	int i;

	for (;;) {
		err = select(priv->maxfds, &priv->rfds, NULL, NULL, NULL);
		if (err >= 0) {
			for (i = 0; i < MAX_EVENTS && priv->fd[i]; i++) {
				if (FD_ISSET(priv->fd[i], &priv->rfds)) {
					easy_recv_event(priv->handle[i]);
				}
			}
		}
	}

	return 0;
}

int test_setup_event(struct private *priv, const char *ifname,
					const char *family, const char *group)
{
	struct event_struct event;
	void *handle;
	int ret = 0;
	int fd;

	/* prepare event struct */
	memset(&event, 0, sizeof(struct event_struct));
	if (ifname)
		strncpy(event.ifname, ifname, 16);

	strncpy(event.family, family, 32);
	strncpy(event.group, group, 32);
	event.priv = priv;
	event.cb = test_event_cb;

	/* allocate response buffer */
	event.resp.data = calloc(MAX_EVENT_RESPONSE_LEN, sizeof(uint8_t));
	if (event.resp.data == NULL)
		goto free_priv;

	event.resp.len = MAX_EVENT_RESPONSE_LEN;

	/* register for event notification */
	ret = easy_register_event(&event, &handle);
	if (ret < 0) {
		libeasy_err("Failed to register netlink event (%s, %s)!\n",
				family, group);
		goto free_priv;
	}

	fd = easy_get_event_fd(handle);
	FD_SET(fd, &priv->rfds);
	priv->handle[priv->numevs] = handle;
	priv->fd[priv->numevs++] = fd;
	priv->maxfds = fd + 1;

free_priv:
	if (event.resp.len)
		free(event.resp.data);

	return ret;
}

void test_exit(void)
{
	struct private *priv = g_private;
	int i;
	int ret = 0;

	for (i = 0; i < priv->numevs; i++) {
		if (priv->fd[i])
			ret |= easy_unregister_event(priv->handle[i]);
	}

	if (ret)
		printf("Failed to unregister event!\n");

	free(priv);
	exit(0);
}

static void test_sighandler(int sig)
{
	switch (sig) {
	case SIGTERM:
	case SIGINT:
		test_exit();
		break;
	default:
		break;
	}
}

static int test_setup_events_from_file(struct private *priv, const char *file)
{
	struct json_object *jevs, *jev_array;
	int ret=0, i, j;
	int len = 0;

	jevs = json_object_from_file(file);
	if (!jevs) {
		libeasy_err("Failed to open '%s'\n", file);
		return -1;
	}

	if (!json_object_is_type(jevs, json_type_object)) {
		json_object_put(jevs);
		return -1;
	}

	json_object_object_get_ex(jevs, "events", &jev_array);
	len = json_object_array_length(jev_array);
	for (i = 0; i < len; i++) {
		struct json_object *jev;
		struct json_object *jev_family, *jev_ifname;
		struct json_object *jev_grp_array, *jev_grp;
		const char *ifname, *family, *group;
		int grplen = 0;

		jev = json_object_array_get_idx(jev_array, i);

		json_object_object_get_ex(jev, "ifname", &jev_ifname);
		ifname = json_object_get_string(jev_ifname);

		json_object_object_get_ex(jev, "family", &jev_family);
		family = json_object_get_string(jev_family);

		json_object_object_get_ex(jev, "group", &jev_grp_array);
		grplen = json_object_array_length(jev_grp_array);

		for (j = 0; j < grplen; j++) {
			jev_grp = json_object_array_get_idx(jev_grp_array, j);
			group = json_object_get_string(jev_grp);
			libeasy_dbg("Setup event (%s, %s, %s)\n",
						ifname, family, group);
			ret |= test_setup_event(priv, ifname, family, group);
		}
	}

	return ret;
}

void test_usage(const char *progname)
{
	libeasy_dbg("Usage: test app for events' subscription\n");
	libeasy_dbg("       %s -f <event-defs.json>\n", progname);
}

int main(int argc, char **argv)
{
	struct private *priv;
	const char *jsonfile = NULL;
	int ch;

	while ((ch = getopt(argc, argv, "hf:")) != -1) {
		switch (ch) {
		case 'h':
			test_usage(argv[0]);
			exit(0);
		case 'f':
			jsonfile = optarg;
			break;
		default:
			break;
		}
	}

	if (!jsonfile) {
		libeasy_err("A valid events definition file needed!\n");
		return -1;
	}

	set_sighandler(SIGPIPE, SIG_DFL);
	set_sighandler(SIGINT, test_sighandler);
	set_sighandler(SIGTERM, test_sighandler);

	priv = calloc(1, sizeof(struct private));
	if (!priv) {
		libeasy_err("memory alloc failed!\n");
		return -1;
	}
	g_private = priv;

	/* read events' map file and register events */
	test_setup_events_from_file(priv, jsonfile);

	/* receive for events forever */
	test_recv_event(priv);

	/* should not reach here... */
	test_exit();

	return 0;
}
