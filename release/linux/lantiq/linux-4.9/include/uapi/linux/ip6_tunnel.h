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

struct map_rule_parm {
	struct in6_addr ipv6_prefix;
	__u8            ipv6_prefix_length;
	__be32          ipv4_prefix;
	__u8            ipv4_prefix_length;
	__u16           psid_prefix;
	__u8            psid_prefix_length;
	__u8            ea_length;
	__u8            psid_offset;
	__u8            forwarding_mode;
	__u8            forwarding_rule;
	struct in6_addr raddr;
	struct in6_addr laddr;
};

struct map_pool_parm {
	__be32          pool_prefix;
	__u8            pool_prefix_length;
};

struct map_parm {
	char                 name[IFNAMSIZ];
	int                  tunnel_source;
	struct in6_addr      br_address;
	__u8                 br_address_length;
	__u8                 role;
	__u8                 default_forwarding_mode;
	__u8                 default_forwarding_rule;
	int                  ipv6_fragment_size;
	__u8                 ipv4_fragment_inner;
	__u8                 napt_always;
	__u8                 napt_force_recycle;
	unsigned long        rule_num;
	unsigned long        pool_num;
	struct map_rule_parm rule[0];
	struct map_pool_parm pool[0];
};

#endif
