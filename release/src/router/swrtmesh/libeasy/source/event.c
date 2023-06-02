/*
 * event.c - implements event subscription and handling APIs
 *
 * Copyright (C) 2020 iopsys Software Solutions AB. All rights reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netlink/genl/ctrl.h>
#include <netlink/genl/genl.h>
#include <netlink/route/link.h>
#include <netlink/attr.h>

#include "easy.h"

#define MAX_MSG_LEN	512

struct default_event {
	struct nl_sock *sock;
	struct event_struct req;
	int (*filter_event)(struct event_struct *req, char *resp_data);
};

/* default_event attributes */
enum default_event_attrs {
	DEFAULT_EVENT_ATTR_UNSPEC,
	DEFAULT_EVENT_ATTR_MSG,
	__DEFAULT_EVENT_ATTR_AFTER_LAST,
	NUM_DEFAULT_EVENT_ATTR = __DEFAULT_EVENT_ATTR_AFTER_LAST,
	DEFAULT_EVENT_ATTR_MAX = __DEFAULT_EVENT_ATTR_AFTER_LAST - 1,
};

/* default_event policy */
static struct nla_policy default_event_policy[NUM_DEFAULT_EVENT_ATTR] = {
	[DEFAULT_EVENT_ATTR_MSG] = { .type = NLA_STRING },
};

int easy_default_event_handler(struct nl_msg *msg, void *arg)
{
	struct default_event *ev = (struct default_event *)arg;
	struct event_response *resp = &ev->req.resp;
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	struct nlattr *attrs[NUM_DEFAULT_EVENT_ATTR];
	int ret;

	if (!genlmsg_valid_hdr(nlh, 0)) {
		libeasy_err("received invalid message\n");
		return 0;
	}

	ret = genlmsg_parse(nlh, 0, attrs, DEFAULT_EVENT_ATTR_MSG,
						default_event_policy);
	if (ret)
		return ret;

	/* TODO: match 'ev->req.ifname' with received event's ifname and
	 * discard events that are not ours.
	 * This will need change in the default_event_attrs to include an
	 * attribute for 'ifname'. And, all event senders MUST fill that
	 * attribute with appropriate 'ifname'.
	 *
	 * Until that is done, we find "vif" string's value in the event
	 * message string.
	 */

	if (attrs[DEFAULT_EVENT_ATTR_MSG]) {
		size_t len = (size_t)nla_len(attrs[DEFAULT_EVENT_ATTR_MSG]);

		if (len > MAX_MSG_LEN) {
			/* event will be discarded by caller anyway
			 * if ill-formed.
			 */
			len = MAX_MSG_LEN;
		}

		resp->type = EASY_EVENT_DEFAULT;
		resp->len = (uint32_t)len;
		if (resp->data) {
			memcpy(resp->data,
				nla_get_string(attrs[DEFAULT_EVENT_ATTR_MSG]),
				len);

		}

		/* Allow caller to filter events before its callback
		 * gets called.
		 * Discard event if filter_event() returned non-zero.
		 */
		if (ev->filter_event &&
			ev->filter_event(&ev->req, (char *)resp->data)) {

			return 0;
		}

		if (ev->req.cb)
			ev->req.cb(&ev->req);
	}

	return 0;
}

int LIBEASY_API easy_register_event(struct event_struct *req, void **handle)
{
	struct default_event *ev;
	struct nl_sock *sock;
	int grp;
	int err;

	ev = calloc(1, sizeof(struct default_event));
	if (!ev) {
		libeasy_err("%s: malloc failed!\n", __func__);
		return -ENOMEM;
	}

	memcpy(&ev->req, req, sizeof(struct event_struct));
	sock = nl_socket_alloc();
	if (!sock) {
		libeasy_err("Error: nl_socket_alloc\n");
		goto free_handle;
	}
	ev->sock = sock;
	ev->filter_event = req->filter_event;
	nl_socket_disable_seq_check(sock);
	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM,
			    easy_default_event_handler, ev);

	if ((err = genl_connect(sock)) < 0) {
		libeasy_err("Error: %s\n", nl_geterror(err));
		goto free_sock;
	}

	if ((grp = genl_ctrl_resolve_grp(sock, req->family, req->group)) < 0) {
		libeasy_err("Error: %s ('%s', '%s')\n",
					nl_geterror(grp),
					req->family,
					req->group);
		goto free_sock;
	}

	nl_socket_add_membership(sock, grp);
	req->fd_monitor = easy_get_event_fd(ev);
	*handle = ev;

	return 0;

free_sock:
	nl_socket_free(sock);
free_handle:
	free(ev);
	*handle = NULL;
	return -1;
}

int LIBEASY_API easy_unregister_event(void *handle)
{
	struct default_event *ev = (struct default_event *)handle;
	struct event_struct *req;
	int ret = -1;
	int grp;

	if (!ev)
		return ret;

	req = &ev->req;
	grp = genl_ctrl_resolve_grp(ev->sock, req->family, req->group);
	if (grp < 0) {
		libeasy_err("Error: %s ('%s', '%s')\n",
					nl_geterror(grp),
					req->family,
					req->group);
		goto free_sock;
	}

	ret = 0;
	nl_socket_drop_membership(ev->sock, grp);

free_sock:
	nl_socket_free(ev->sock);
	free(ev);
	return ret;
}

int LIBEASY_API easy_recv_event(void *handle)
{
	struct default_event *ev = (struct default_event *)handle;
	int err;

	err = nl_recvmsgs_default(ev->sock);
	if (err < 0) {
		libeasy_err("Error: %s\n", nl_geterror(err));
		return err;
	}

	return 0;
}

int LIBEASY_API easy_get_event_fd(void *handle)
{
	if (!handle)
		return -1;

	return nl_socket_get_fd(((struct default_event *)handle)->sock);
}
