/*
 * if_utils.c - helper functions for network interface
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
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netlink/route/link.h>
#include <netlink/route/addr.h>
#include <netlink/route/link/bridge.h>
#include <netlink/route/link/macvlan.h>


#include "easy.h"

static int if_openlink(const char *ifname, struct nl_sock **s, struct rtnl_link **l)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	int ret = 0;

	sk = nl_socket_alloc();
	if (sk == NULL) {
		ret = -errno;
		return ret;
	}

	nl_connect(sk, NETLINK_ROUTE);
	if (rtnl_link_get_kernel(sk, 0, ifname, &link) < 0) {
		ret = -1;
		goto out;
	}

	*l = link;
	*s = sk;
	return 0;

out:
	nl_socket_free(sk);
	return ret;
}

static int if_closelink(struct nl_sock *s, struct rtnl_link *l)
{
	rtnl_link_put(l);
	nl_socket_free(s);

	return 0;
}

int LIBEASY_API if_getoperstate(const char *ifname, ifopstatus_t *opstatus)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	int ret = 0;

	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;

	*opstatus = rtnl_link_get_operstate(link);

	if_closelink(sk, link);

	return 0;
}

int LIBEASY_API if_setoperstate(const char *ifname, ifopstatus_t opstatus)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	uint8_t opp = 0;
	int ret = 0;

	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;

	opp = rtnl_link_get_operstate(link);
	opp |= opstatus;
	rtnl_link_set_operstate(link, opp);

	if_closelink(sk, link);

	return 0;
}

int LIBEASY_API if_gethwaddr(const char *ifname, uint8_t *hwaddr)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	struct nl_addr *a;
	int ret = 0;

	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;

	a = rtnl_link_get_addr(link);
	memcpy(hwaddr, nl_addr_get_binary_addr(a), nl_addr_get_len(a));

	if_closelink(sk, link);

	return 0;
}

int LIBEASY_API if_sethwaddr(const char *ifname, uint8_t *hwaddr)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	struct nl_addr *a;
	int ret = 0;

	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;

	a = nl_addr_alloc(6);
	if (a) {
		ret = nl_addr_set_binary_addr(a, hwaddr, 6);
		if (!ret) {
			rtnl_link_set_addr(link, a);
		}
		nl_addr_put(a);
	}

	if_closelink(sk, link);

	return 0;
}

int LIBEASY_API if_getflags(const char *ifname, uint32_t *flags)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	int ret = 0;

	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;

	*flags = rtnl_link_get_flags(link);

	if_closelink(sk, link);

	return 0;
}

int LIBEASY_API if_isloopback(const char *ifname)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	unsigned int f = 0;
	int ret = 0;

	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;

	f = rtnl_link_get_flags(link);
	if_closelink(sk, link);

	return !!(f & IFF_LOOPBACK);
}

int LIBEASY_API if_setflags(const char *ifname, uint32_t flags)
{
	struct rtnl_link *link, *tmp;
	struct nl_sock *sk;
	unsigned int f = 0;
	int ret = 0;

	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;

	f = rtnl_link_get_flags(link);
	f |= flags;
	tmp = rtnl_link_alloc();
	if (tmp) {
		rtnl_link_set_flags(tmp, f);
		ret = rtnl_link_change(sk, link, tmp, 0);
		//rtnl_link_put(tmp);
	}

	if_closelink(sk, link);

	return ret;
}

int LIBEASY_API if_getaddrs(const char *ifname, struct ip_address *addrs,
			    int *num_addrs)
{
	struct rtnl_addr *naddr;
	struct nl_object *nobj;
	struct rtnl_link *link;
	struct nl_cache *res;
	struct nl_sock *sk;
	int ret = 0;
	int num;
	int i;
	unsigned int ifindex = if_nametoindex(ifname);


	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;

	ret = rtnl_addr_alloc_cache(sk, &res);
	if (ret)
		goto out;

	num = nl_cache_nitems(res);
	if (*num_addrs < num)
		num = *num_addrs;

	nobj = nl_cache_get_first(res);
	naddr = (struct rtnl_addr *)nobj;
	*num_addrs = 0;

	for (i = 0; i < num; i++) {
		struct nl_addr *local = rtnl_addr_get_local(naddr);
		struct ip_address *ip;
		int family;

		if (rtnl_addr_get_ifindex(naddr) == ifindex) {
			family = nl_addr_get_family(local);
			if (family == AF_INET6 || family == AF_INET) {
				ip = addrs + *num_addrs;
				ip->family = family;
				memcpy(&ip->addr,
				       nl_addr_get_binary_addr(local),
				       nl_addr_get_len(local));

				*num_addrs += 1;
			}
		}

		nobj = nl_cache_get_next(nobj);
		naddr = (struct rtnl_addr *)nobj;
	}

	nl_cache_free(res);
out:
	if_closelink(sk, link);

	return 0;
}

int LIBEASY_API if_setaddr(const char *ifname, const char *ip)
{
	unsigned int ifindex = if_nametoindex(ifname);
	struct rtnl_addr *rnaddr = NULL;
	struct nl_addr *naddr = NULL;
	uint8_t ipbuf[16] = {0};
	struct rtnl_link *link;
	size_t defprefix_len;
	char *prefix = NULL;
	struct nl_sock *sk;
	size_t prefix_len;
	char *ipdup;
	int ret = 0;
	int family;
	size_t len;


	if (ifindex == 0)
		return -1;

	ipdup = strdup(ip);
	if (!ipdup)
		return -1;

	prefix = strchr(ipdup, '/');
	if (prefix)
		*prefix = '\0';

	libeasy_dbg("%s: '%s'\n", __func__, ip);

	ret = inet_pton(AF_INET, ipdup, ipbuf);
	if (ret <= 0) {
		ret = inet_pton(AF_INET6, ipdup, ipbuf);
		if (ret <= 0) {
			libeasy_err("%s: Invalid ip address format!\n", __func__);
			return -1;
		} else {
			family = AF_INET6;
			len = 16;
			defprefix_len = 64;
		}
	} else {
		family = AF_INET;
		len = 4;
		defprefix_len = 24;
	}

	if (prefix) {
		prefix++;
		prefix_len = strtoul(prefix, NULL, 10);
		if (prefix_len > defprefix_len) {
			libeasy_err("%s: Invalid ip address prefix!\n", __func__);
			return -1;
		}
	} else {
		prefix_len = defprefix_len;
	}

	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;


	naddr = nl_addr_alloc(len);
	if (!naddr) {
		ret = -1;
		goto out;
	}

	nl_addr_set_family(naddr, family);
	nl_addr_set_binary_addr(naddr, ipbuf, len);
	nl_addr_set_prefixlen(naddr, (int)prefix_len);

	rnaddr = rtnl_addr_alloc();
	if (!rnaddr) {
		nl_addr_put(naddr);
		ret = -1;
		goto out;
	}

	rtnl_addr_set_ifindex(rnaddr, (int)ifindex);
	rtnl_addr_set_family(rnaddr, family);
	rtnl_addr_delete(sk, rnaddr, 0);

	rtnl_addr_set_local(rnaddr, naddr);
	ret = rtnl_addr_add(sk, rnaddr, 0);

	rtnl_addr_put(rnaddr);
	nl_addr_put(naddr);
out:
	if_closelink(sk, link);
	free(ipdup);

	return ret;
}

int LIBEASY_API if_getstats(const char *ifname, struct if_stats *s)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	int ret;

	if (!s)
		return -1;

	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;

	s->rx_packets = rtnl_link_get_stat(link, RTNL_LINK_RX_PACKETS);
	s->tx_packets = rtnl_link_get_stat(link, RTNL_LINK_TX_PACKETS);
	s->rx_bytes = rtnl_link_get_stat(link, RTNL_LINK_RX_BYTES);
	s->tx_bytes = rtnl_link_get_stat(link, RTNL_LINK_TX_BYTES);
	s->rx_errors = rtnl_link_get_stat(link, RTNL_LINK_RX_ERRORS);
	s->tx_errors = rtnl_link_get_stat(link, RTNL_LINK_TX_ERRORS);
	s->rx_dropped = rtnl_link_get_stat(link, RTNL_LINK_RX_DROPPED);
	s->tx_dropped = rtnl_link_get_stat(link, RTNL_LINK_TX_DROPPED);
	s->rx_errors_crc = rtnl_link_get_stat(link, RTNL_LINK_RX_CRC_ERR);

	if_closelink(sk, link);

	return 0;
}

int LIBEASY_API if_isbridge(const char *ifname)
{
	DIR *dir;
	char path[512] = { 0 };

	snprintf(path, 512, "/sys/class/net/%s/bridge", ifname);
	dir = opendir(path);
	if (dir) {
		closedir(dir);
		return 1;
	}

	return 0;
}

/* Returns 0 if not; else ifindex of master interface.
 * On error, returns -1.
 */
int LIBEASY_API if_isbridge_interface(const char *ifname)
{
	struct rtnl_link *link, *mlink;
	struct nl_sock *sk;
	int ret = 0;
	unsigned int mifindex;

	ret = if_openlink(ifname, &sk, &link);
	if (ret)
		return -1;

	mifindex = (unsigned int)rtnl_link_get_master(link);
	if (!mifindex)
		goto out;

	ret = rtnl_link_get_kernel(sk, (int)mifindex, NULL, &mlink);
	if (ret)
		goto out;

	/* if (!strncmp(rtnl_link_get_type(mlink), "bridge", 6)) */

	ret = rtnl_link_is_bridge(mlink);
	if (ret == 1)
		ret = (int)mifindex;

	rtnl_link_put(mlink);

out:
	if_closelink(sk, link);
	return ret;
}

int LIBEASY_API br_get_iflist(const char *brname, int *num, char ifs[][16])
{
	char path[512] = { 0 };
	struct dirent **namelist;
	int i = 0;
	int n;

	snprintf(path, 512, "/sys/class/net/%s/brif", brname);
	n = scandir(path, &namelist, NULL, alphasort);
	if (n < 0) {
		libeasy_err("%s: scandir error\n", __func__);
		return -1;
	} else {
		libeasy_dbg("%s: num-ports = %d\n", __func__, n);
		*num = 0;
		for (i = 0; i < n; i++) {
			if (namelist[i]->d_name[0] == '.' &&
			    (namelist[i]->d_name[1] == '\0' ||
			     (namelist[i]->d_name[1] == '.' &&
			      namelist[i]->d_name[2] == '\0'))) {
				free(namelist[i]);
				continue;
			}


			libeasy_dbg("%s\n", namelist[i]->d_name);
			strncpy(ifs[*num], namelist[i]->d_name, 15);
			ifs[*num][15] = '\0';
			*num += 1;
			free(namelist[i]);
		}
		free(namelist);
	}

	return 0;
}

int LIBEASY_API br_add(const char *brname)
{
	struct nl_sock *sk;
	int ret = 0;

	sk = nl_socket_alloc();
	if (sk == NULL) {
		ret = -errno;
		return ret;
	}

	nl_connect(sk, NETLINK_ROUTE);
	ret = rtnl_link_bridge_add(sk, brname);
	nl_socket_free(sk);

	return ret;
}

int LIBEASY_API br_del(const char *brname)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	int ret = 0;

	sk = nl_socket_alloc();
	if (sk == NULL) {
		ret = -errno;
		return ret;
	}

	nl_connect(sk, NETLINK_ROUTE);
	ret = rtnl_link_get_kernel(sk, 0, brname, &link);
	if (ret)
		goto out;

	rtnl_link_delete(sk, link);
out:
	nl_socket_free(sk);
	return ret;
}

int LIBEASY_API br_addif(const char *brname, const char *ifname)
{
	struct rtnl_link *brlink, *link;
	struct nl_sock *sk;
	int ret = 0;


	ret = if_openlink(brname, &sk, &brlink);
	if (ret)
		return -1;

	ret = rtnl_link_get_kernel(sk, 0, ifname, &link);
	if (ret)
		goto out;

	ret = rtnl_link_enslave(sk, brlink, link);
	if (ret)
		goto out;

	rtnl_link_put(link);

out:
	if_closelink(sk, brlink);
	return ret;
}

int LIBEASY_API br_delif(const char *brname, const char *ifname)
{
	struct rtnl_link *brlink, *link;
	struct nl_sock *sk;
	int ret = 0;


	ret = if_openlink(brname, &sk, &brlink);
	if (ret)
		return -1;

	ret = rtnl_link_get_kernel(sk, 0, ifname, &link);
	if (ret)
		goto out;

	ret = rtnl_link_release(sk, link);
	if (ret)
		goto out;

	rtnl_link_put(link);

out:
	if_closelink(sk, brlink);
	return ret;
}

int LIBEASY_API macvlan_addif(const char *ifname, const char *ifnew,
			      uint8_t *macaddr, const char *mode)
{
	struct rtnl_link *link;
	struct nl_addr* addr;
	struct nl_sock *sk;
	unsigned int mifindex;
	int ret = 0;


	if (!macaddr)
		return -1;

	sk = nl_socket_alloc();
	if (sk == NULL) {
		ret = -errno;
		return ret;
	}

	nl_connect(sk, NETLINK_ROUTE);
	mifindex = if_nametoindex(ifname);
	if (!mifindex) {
		ret = -errno;
		goto out_ret;
	}

	link = rtnl_link_macvlan_alloc();
	if (!link)
		goto out_ret;

	if (ifnew)
		rtnl_link_set_name(link, ifnew);

	rtnl_link_set_link(link, (int)mifindex);
	addr = nl_addr_build(AF_LLC, macaddr, 6);
	rtnl_link_set_addr(link, addr);
	nl_addr_put(addr);

	ret = rtnl_link_macvlan_set_mode(link, mode == NULL ?
					 (uint32_t)rtnl_link_macvlan_str2mode("bridge") :
					 (uint32_t)rtnl_link_macvlan_str2mode(mode));

	if (ret)
		goto out;

	ret = rtnl_link_add(sk, link, NLM_F_CREATE);

out:
	rtnl_link_put(link);
out_ret:
	nl_socket_free(sk);

	return ret;
}

int LIBEASY_API macvlan_delif(const char *ifname)
{
	struct rtnl_link *link;
	struct nl_sock *sk;
	int ret = 0;

	sk = nl_socket_alloc();
	if (sk == NULL) {
		ret = -errno;
		return ret;
	}

	nl_connect(sk, NETLINK_ROUTE);
	ret = rtnl_link_get_kernel(sk, 0, ifname, &link);
	if (ret)
		goto out;

	rtnl_link_delete(sk, link);
out:
	nl_socket_free(sk);
	return ret;
}
