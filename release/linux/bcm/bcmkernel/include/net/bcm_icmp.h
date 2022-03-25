/*
*    Copyright (c) 2003-2019 Broadcom
*    All Rights Reserved
*
<:label-BRCM:2019:DUAL/GPL:standard

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation (the "GPL").

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

:>
*/
#ifndef _BCM_ICMP_H
#define	_BCM_ICMP_H

struct icmp_bxm {
	struct sk_buff *skb;
	int offset;
	int data_len;

	struct {
		struct icmphdr icmph;
		__be32	       times[3];
	} data;
	int head_len;
	struct ip_options_data replyopts;
};

struct icmp_control {
	bool (*handler)(struct sk_buff *skb);
	short   error;
};

extern const struct icmp_control icmp_pointers[];

void icmp_push_reply(struct icmp_bxm *icmp_param,
		 struct flowi4 *fl4,
		 struct ipcm_cookie *ipc, struct rtable **rt);
struct rtable *icmp_route_lookup(struct net *net,
		 struct flowi4 *fl4,
		 struct sk_buff *skb_in,
		 const struct iphdr *iph,
		 __be32 saddr, u8 tos, u32 mark,
		 int type, int code,
		 struct icmp_bxm *param);
void send_icmp_frag(struct sk_buff *skb_in, int type, int code, __be32 info);

#endif	/* _BCM_ICMP_H */
