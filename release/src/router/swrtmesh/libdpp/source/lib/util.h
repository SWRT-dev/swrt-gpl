/*
 * util.h - utility functions header.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */

#ifndef UTIL_H
#define UTIL_H


#include <libubox/list.h>
#include <arpa/inet.h>
#include <sys/time.h>


void do_daemonize(const char *pidfile);
int timeradd_msecs(struct timeval *a, unsigned long msecs, struct timeval *res);
void get_random_bytes(int num, uint8_t *buf);
//void bufprintf(uint8_t *buf, int len, const char *label);

typedef unsigned int ifstatus_t;
typedef unsigned char ifopstatus_t;

int if_brportnum(const char *ifname);

#ifndef BIT
#define BIT(x)	(1 << (x))
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof((a)[0]))
#endif

#define MACFMT		"%02x:%02x:%02x:%02x:%02x:%02x"
#define MAC2STR(_m)	(_m)[0], (_m)[1], (_m)[2], (_m)[3], (_m)[4], (_m)[5]

#define COMPACT_MACSTR "%02x%02x%02x%02x%02x%02x"

#ifndef list_flush
#define list_flush(head, type, member)					\
do {									\
	type *__p, *__tmp;						\
									\
	if (!list_empty(head))						\
		list_for_each_entry_safe(__p, __tmp, head, member) {	\
			list_del(&__p->member);				\
			free(__p);					\
		}							\
} while (0)
#endif

void bufprintf(const char *label, const uint8_t *buf, int len);
void dump(const uint8_t *buf, int len, char *label);
int has_ctrl_char(const uint8_t *data, size_t len);
int snprintf_hex(char *buf, size_t buf_size, uint8_t *data, size_t len);
const char *ssid_to_str(const uint8_t *ssid, size_t ssid_len);
int ieee80211_chan_to_freq(uint8_t op_class, uint8_t chan);

#endif /* UTIL_H */
