/*
 * wifimngr.c - provides "wifi" ubus object
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

#ifndef WIFIMNGR_H
#define WIFIMNGR_H

 #define wifimngr_err(...)  pr_error("wifimngr: " __VA_ARGS__)
 #define wifimngr_warn(...) pr_warn("wifimngr: " __VA_ARGS__)
 #define wifimngr_info(...) pr_info("wifimngr: " __VA_ARGS__)
 #define wifimngr_dbg(...)  pr_debug("wifimngr: " __VA_ARGS__)


/* wifi ubus objects */
struct wifi_ubus_object {
	struct ubus_object obj;
	struct ubus_object_type obj_type;
	struct list_head list;
	struct list_head sobjlist;   /* list of sub objects */
};

struct wifimngr {
	struct ubus_object *wifi;
	struct list_head radiolist;  /* list of wifi_ubus_objects */
	struct list_head iflist;     /* list of wifi_ubus_objects */
};

#define WIFI_DEV_MAX_NUM        16
#define WIFI_IF_MAX_NUM         16

struct wifimngr_device {
	char device[16];
	char phy[16];
};

enum iface_mode {
	WIFI_AP,
	WIFI_STA,
};

struct wifimngr_iface {
	char iface[16];
	char device[16];
	char phy[16];
	int mode;
	int disabled;
};

int wifimngr_get_wifi_devices(char devs[][WIFI_DEV_MAX_NUM]);
int wifimngr_get_wifi_interfaces(struct wifimngr_iface ifs[]);


//#define wifimngr_add_object(w, u, n, type)  ubus_add_## type ##_object(w, u, n)

#define UBUS_METHOD_ADD(_tab, iter, __m)				\
do {									\
	struct ubus_method ___m = __m;					\
	memcpy(&_tab[iter++], &___m, sizeof(struct ubus_method));	\
} while(0)


extern int add_wifi_methods(struct ubus_object *wifi_obj);
extern int add_wps_methods(struct ubus_object *wifi_obj);

extern int wifimngr_add_object(struct wifimngr *w,
				struct ubus_context *ctx,
				const char *objname,
				int (*add_methods)(struct ubus_object *o));

extern int wifimngr_add_radio_object(struct wifimngr *w,
					struct ubus_context *ctx,
					const char *objname);

extern int wifimngr_add_interface_object(struct wifimngr *w,
					struct ubus_context *ctx,
					struct wifimngr_iface *iface);

extern int wifimngr_init(struct wifimngr **w);
extern int wifimngr_exit(struct wifimngr *w);

extern void wifimngr_version(void);

extern int wifimngr_nl_msgs_handler(struct ubus_context *ctx);

extern void wifimngr_event_init(void);
extern void wifimngr_event_exit(void);
extern int wifimngr_events_register(void *ctx, const char *ifname,
				    const char *family, const char *group);
extern int wifimngr_event_register(void *ctx, const char *ifname,
				   const char *family, const char *group);
extern void wifimngr_event_unregister(const char *ifname);

#endif /* WIFIMNGR_H */
