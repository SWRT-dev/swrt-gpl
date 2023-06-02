/*
 * if_utils.h - network interface helper functions header.
 *
 * Copyright (C) 2020 iopsys Software Solutions AB. All rights reserved.
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

#ifndef EASY_IF_UTILS_H
#define EASY_IF_UTILS_H

#include <stdint.h>
#include <arpa/inet.h>


#ifdef __cplusplus
extern "C" {
#endif



/*
 * unsigned int if_nametoindex(const char *ifname);
 * char *if_indextoname(unsigned int ifindex, char *ifname);
 *
 * The following functions similar to the above are defined for convenience -
 *
 */
int if_gethwaddr(const char *ifname, uint8_t *hwaddr);
int if_sethwaddr(const char *ifname, uint8_t *hwaddr);
int if_getflags(const char *ifname, uint32_t *flags);
int if_setflags(const char *ifname, uint32_t flags);
int if_getoperstate(const char *ifname, ifopstatus_t *opstatus);
int if_setoperstate(const char *ifname, ifopstatus_t opstatus);
int if_isloopback(const char *ifname);
int if_isbridge(const char *ifname);


struct ip_address {
	int family;	/* AF_INET or AF_INET6 */
	union {
		uint8_t raw[16];
		struct in_addr ip4;
		struct in6_addr ip6;
	} addr;
};

/** Get ipv4 and ipv6 address(es) of an interface.
 *
 * Usage:
 *
 * struct ip_address ips[10] = {0};
 * int n = 10;
 *
 * if_getaddrs("wlp3s0", ips, &n);
 * for (i = 0; i < n; i++) {
 *	char buf[256] = {0};
 *	size_t sz = 256;
 *	if (ips[i].family == AF_INET)
 *		inet_ntop(AF_INET, &ips[i].addr.ip4, buf, sz);
 *	else
 *		inet_ntop(AF_INET6, &ips[i].addr.ip6, buf, sz);
 *
 *	printf("Address: %s\n", buf);
 * }
 *
 *
 * @param[in] ifname interface name.
 * @param[in/out] addrs array of 'struct ip_address'.
 * @param[in/out] num_addrs number of array elements passed and updated by output.
 * @return 0 on Success, -1 or any other value on error.
 */
int if_getaddrs(const char *ifname, struct ip_address *addrs, int *num_addrs);


/** Set/add ipv4 or ipv6 address to an interface.
 *
 * Usage:
 *
 * if_setaddr("enx18d6c71f5b64", "fe80::3f5c:ba36:f9f2:8000/64");
 * if_setaddr("enx18d6c71f5b64", "192.168.1.200/24");
 * if_setaddr("enx18d6c71f5b64", "10.10.10.101");
 *
 * @param[in] ifname interface name.
 * @param[in] ip ipv4/ipv6 address string with optional prefix.
 * @return 0 on Success, -1 on error.
 */
int if_setaddr(const char *ifname, const char *ip);

/** struct if_stats - interface statistics */
struct if_stats {
	uint64_t rx_packets;
	uint64_t tx_packets;
	uint64_t rx_bytes;
	uint64_t tx_bytes;
	uint64_t rx_errors;
	uint64_t tx_errors;
	uint64_t rx_dropped;
	uint64_t tx_dropped;
	uint64_t rx_errors_crc;
};

int if_getstats(const char *ifname, struct if_stats *s);

/* Returns 'ifindex' of master bridge, else 0. On error, returns -1. */
int if_isbridge_interface(const char *ifname);


int br_add(const char *brname);
int br_del(const char *brname);
int br_addif(const char *brname, const char *ifname);
int br_delif(const char *brname, const char *ifname);
int br_get_iflist(const char *brname, int *num, char ifs[][16]);


int macvlan_addif(const char *ifname, const char *ifnew, uint8_t *macaddr,
		  const char *mode);
int macvlan_delif(const char *ifname);

#ifdef __cplusplus
}
#endif

#endif /* EASY_IF_UTILS_H */
