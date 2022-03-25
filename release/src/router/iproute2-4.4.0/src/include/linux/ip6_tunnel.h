#ifndef _IP6_TUNNEL_H
#define _IP6_TUNNEL_H

#include <linux/types.h>

#define IPV6_TLV_TNL_ENCAP_LIMIT 4
#define IPV6_DEFAULT_TNL_ENCAP_LIMIT 4

/* don't add encapsulation limit if one isn't present in inner packet */
#define IP6_TNL_F_IGN_ENCAP_LIMIT 0x1
/* copy the traffic class field from the inner packet */
#define IP6_TNL_F_USE_ORIG_TCLASS 0x2
/* copy the flowlabel from the inner packet */
#define IP6_TNL_F_USE_ORIG_FLOWLABEL 0x4
/* being used for Mobile IPv6 */
#define IP6_TNL_F_MIP6_DEV 0x8
/* copy DSCP from the outer packet */
#define IP6_TNL_F_RCV_DSCP_COPY 0x10
/* copy fwmark from inner packet */
#define IP6_TNL_F_USE_ORIG_FWMARK 0x20

struct ip6_tnl_parm {
	char name[IFNAMSIZ];	/* name of tunnel device */
	int link;		/* ifindex of underlying L2 interface */
	__u8 proto;		/* tunnel protocol */
	__u8 encap_limit;	/* encapsulation limit for tunnel */
	__u8 hop_limit;		/* hop limit for tunnel */
	__be32 flowinfo;	/* traffic class and flowlabel for tunnel */
	__u32 flags;		/* tunnel flags */
	struct in6_addr laddr;	/* local tunnel end-point address */
	struct in6_addr raddr;	/* remote tunnel end-point address */
};

/* IPv6 tunnel FMR */
struct ip6_tnl_fmr_rule{
	struct in6_addr ip6_prefix;
	struct in_addr ip4_prefix;

	__u8 ip6_prefix_len;
	__u8 ip4_prefix_len;
	__u8 ea_len;
	__u8 offset;
};

struct ipv6_tnl_fmr{
	struct in6_addr laddr;
	struct in6_addr raddr;
	struct ip6_tnl_fmr_rule rule;
};

struct __ip6_tnl_fmr {
	struct __ip6_tnl_fmr *next; /* next fmr in list */
	struct ip6_tnl_fmr_rule rule;
};

struct ip6_tnl_parm2 {
	char name[IFNAMSIZ];	/* name of tunnel device */
	int link;		/* ifindex of underlying L2 interface */
	__u8 proto;		/* tunnel protocol */
	__u8 encap_limit;	/* encapsulation limit for tunnel */
	__u8 hop_limit;		/* hop limit for tunnel */
	__be32 flowinfo;	/* traffic class and flowlabel for tunnel */
	__u32 flags;		/* tunnel flags */
	struct in6_addr laddr;	/* local tunnel end-point address */
	struct in6_addr raddr;	/* remote tunnel end-point address */

	__be16			i_flags;
	__be16			o_flags;
	__be32			i_key;
	__be32			o_key;
};

#endif
