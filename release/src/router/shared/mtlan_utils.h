#ifndef MTLAN_UTILS_H
#define MTLAN_UTILS_H
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <ctype.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <bcmnvram.h>
#include <shared.h>
#include "shutils.h"

#define MTLAN_MAXINUM             9 /* 1 (Default) + 8 */
#define MTLAN_VPNS_MAXINUM        8
#define MTLAN_GRE_MAXINUM         8
#define SDN_LIST_BASIC_PARAM      6
#define SDN_LIST_MAX_PARAM        17
#define VLAN_LIST_MAX_PARAM       2
#define SUBNET_LIST_MAX_PARAM     13

enum {
	MTLAN_IFUNIT_BASE=50,
	MTLAN_IFUNIT_START=51,
	MTLAN_IFUNIT_END=58,
	MTLAN_IFUNIT_MAX
};

typedef enum {
	SDNFT_TYPE_SDN,
	SDNFT_TYPE_APG,
	SDNFT_TYPE_VPNC,
	SDNFT_TYPE_VPNS,
	SDNFT_TYPE_URLF,
	SDNFT_TYPE_NWF,
	SDNFT_TYPE_GRE,
	SDNFT_TYPE_FW,
	SDNFT_TYPE_KILLSW,
	SDNFT_TYPE_AHS,
	SDNFT_TYPE_WAN,
	SDNFT_TYPE_MAX
} SDNFT_TYPE;

typedef struct __vlan_rl_t__
{
	int idx;
	int vid;
} VLAN_T;

typedef struct __subnet_rl_t__
{
	int idx;
	char ifname[10];
	char addr[INET_ADDRSTRLEN + 1];
	char netmask[INET_ADDRSTRLEN + 1];
	char subnet[INET_ADDRSTRLEN + 1];
	unsigned int prefixlen;
	unsigned int dhcp_enable;
	char dhcp_min[INET_ADDRSTRLEN + 1];
	char dhcp_max[INET_ADDRSTRLEN + 1];
	unsigned int dhcp_lease;
	char domain_name[64];
	char dns[2][32];
	char wins[64];
	unsigned int dhcp_res;
	unsigned int dhcp_res_idx;
} SUBNET_T;

typedef struct __sdn_feature_t__
{
	int sdn_idx;
	int apg_idx;
	int vpnc_idx;
	int vpns_idx_rl[MTLAN_VPNS_MAXINUM];
	int dnsf_idx; /* DNS Filter */
	int urlf_idx; /* URL Filter */
	int nwf_idx;  /* Netwrok Service Filter */
	int cp_idx;   /* Captive Portal */
	int gre_idx_rl[MTLAN_GRE_MAXINUM];   /* GRE Keepalive */
	int fw_idx;  /* Firewall */
	int killsw_sw; /* Kill Switch */
	int ahs_sw;  /* Access Host Service Switch */
	int wan_idx;
} SDNFT_T;


typedef struct __multiple_lan_t__
{
	int enable;
	int vid;
	SUBNET_T nw_t;
	SDNFT_T sdn_t;
} MTLAN_T;

typedef enum {
	VPN_TYPE_SERVER = 0,
	VPN_TYPE_CLIENT
} VPN_TYPE_T;

typedef enum{
	VPN_PROTO_PPTP,
	VPN_PROTO_L2TP,
	VPN_PROTO_OVPN,
	VPN_PROTO_IPSEC,
	VPN_PROTO_WG,
	VPN_PROTO_L2GRE,
	VPN_PROTO_L3GRE,
	VPN_PROTO_UNKNOWN
}VPN_PROTO_T;

typedef struct __vpn_vpnx__
{
	// VPN_TYPE_T type;
	VPN_PROTO_T proto;
	int unit;
} VPN_VPNX_T;

#define VPN_PROTO_PPTP_STR "PPTP"
#define VPN_PROTO_L2TP_STR "L2TP"
#define VPN_PROTO_OVPN_STR "OpenVPN"
#define VPN_PROTO_IPSEC_STR "IPSec"
#define VPN_PROTO_WG_STR "WireGuard"
#define VPN_PROTO_HMA_STR "HMA"
#define VPN_PROTO_NORDVPN_STR "NordVPN"
#define VPN_PROTO_L2GRE_STR "L2GRE"
#define VPN_PROTO_L3GRE_STR "L3GRE"

extern void *INIT_MTLAN(const unsigned int sz);
extern void FREE_MTLAN(void *p);

extern SUBNET_T *get_mtsubnet(SUBNET_T *sublst, size_t *lst_sz);
extern MTLAN_T  *get_mtlan(MTLAN_T *nwlst, size_t *lst_sz);
extern MTLAN_T  *get_mtlan_by_vid(const unsigned int vid, MTLAN_T *nwlst, size_t *lst_ze);
extern MTLAN_T  *get_mtlan_by_idx(SDNFT_TYPE type, const unsigned int idx, MTLAN_T *nwlst, size_t *lst_sz);

extern size_t get_mtlan_cnt();
extern int get_vpns_idx_by_proto_unit(VPN_PROTO_T proto, int unit);
extern int get_vpnc_idx_by_proto_unit(VPN_PROTO_T proto, int unit);
extern int get_gre_idx_by_proto_unit(VPN_PROTO_T proto, int unit);
extern char* get_vpns_ifname_by_vpns_idx(int vpns_idx, char* buf, size_t len);
extern char* get_vpns_iprange_by_vpns_idx(int vpns_idx, char* buf, size_t len);
extern VPN_VPNX_T* get_vpnx_by_vpnc_idx(VPN_VPNX_T* vpnx, int vpnc_idx);
extern VPN_VPNX_T* get_vpnx_by_vpns_idx(VPN_VPNX_T* vpnx, int vpns_idx);
extern VPN_VPNX_T* get_vpnx_by_gre_idx(VPN_VPNX_T* vpnx, int gre_idx);
#endif
