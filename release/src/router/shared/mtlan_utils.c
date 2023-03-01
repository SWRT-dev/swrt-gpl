#include "mtlan_utils.h"

#define OVPN_SERVER_BASE 20

/*******************************
* IP Calculate
*******************************/
static unsigned defv4mask(char *ip)
{
	struct in_addr net;

	if (inet_pton(AF_INET, ip, &net) <= 0) {
		return 24;
	}
	unsigned byte1 = (ntohl(net.s_addr) >> 24) & 0xff;

	if (byte1 < 128)
		return 8;
	if (byte1 >= 128 && byte1 < 192)
		return 16;
	if (byte1 >= 192 && byte1 < 224)
		return 24;

	return 24;
}

static int bit_count(uint32_t i)
{
	int c = 0;
	unsigned int seen_one = 0;

	while (i > 0) {
		if (i & 1) {
			seen_one = 1;
			c++;
		} else {
			if (seen_one) {
				return -1;
			}
		}
		i >>= 1;
	}

	return c;
}

/*!
  \fn uint32_t prefix2mask(int bits)
  \brief creates a netmask from a specified number of bits
  This function converts a prefix length to a netmask.  As CIDR (classless
  internet domain internet domain routing) has taken off, more an more IP
  addresses are being specified in the format address/prefix
  (i.e. 192.168.2.3/24, with a corresponding netmask 255.255.255.0).  If you
  need to see what netmask corresponds to the prefix part of the address, this
  is the function.  See also \ref mask2prefix.
  \param prefix is the number of bits to create a mask for.
  \return a network mask, in network byte order.
*/
uint32_t prefix2mask(int prefix)
{
	struct in_addr mask;
	memset(&mask, 0, sizeof(mask));
	if (prefix) {
		return htonl(~((1 << (32 - prefix)) - 1));
	} else {
		return htonl(0);
	}
}

/*!
  \fn int mask2prefix(struct in_addr mask)
  \brief calculates the number of bits masked off by a netmask.
  This function calculates the significant bits in an IP address as specified by
  a netmask.  See also \ref prefix2mask.
  \param mask is the netmask, specified as an struct in_addr in network byte order.
  \return the number of significant bits.  */
static int mask2prefix(struct in_addr mask)
{
	return bit_count(ntohl(mask.s_addr));
}

static int v4mask2int(const char *prefix)
{
	int ret;
	struct in_addr in;

	ret = inet_pton(AF_INET, prefix, &in);
	if (ret == 0)
		return -1;

	return mask2prefix(in);
}

static struct in_addr calc_network(struct in_addr addr, int prefix)
{
	struct in_addr mask;
	struct in_addr network;

	mask.s_addr = prefix2mask(prefix);

	memset(&network, 0, sizeof(network));
	network.s_addr = addr.s_addr & mask.s_addr;
	return network;
}

static char *calc_subnet(char *addr, unsigned int prefix, char *ret, size_t retsz)
{
        struct in_addr ip, network;
        char subnet[INET_ADDRSTRLEN + 1];

	if (!ret || !addr)
		return NULL;
	if (prefix > 32)
		prefix = defv4mask(addr);

	inet_pton(AF_INET, addr, &ip);
	network = calc_network(ip, prefix);
	inet_ntop(AF_INET, &network, subnet, INET_ADDRSTRLEN);
	strlcpy(ret, subnet, retsz);

	return ret;
}

/*******************************
* Get Nvram to Sturct
*******************************/
void *INIT_MTLAN(const unsigned int sz)
{
	void *p = calloc(MTLAN_MAXINUM, sz);
	if (!p)
		return NULL;
	return p;
}
void FREE_MTLAN(void *p)
{
	if(p) free(p);
}

static VLAN_T *get_mtvlan(VLAN_T *vlst, size_t *sz)
{
	char *nv = NULL, *nvp = NULL, *b;
	char *idx, *vid;
	size_t cnt = 0;

	if (sz)
		*sz = 0;

	if (!(nv = nvp = strdup(nvram_safe_get("vlan_rl"))))
		return NULL;

	while ((b = strsep(&nvp, "<")) != NULL) {
		if (vstrsep(b, ">", &idx, &vid) != VLAN_LIST_MAX_PARAM)
			continue;

		if (cnt >= MTLAN_MAXINUM)
			break;

		if (idx && *idx)
			vlst[cnt].idx = strtol(idx, NULL, 10);
		if (vid && *vid)
			vlst[cnt].vid = strtol(vid, NULL, 10);

		cnt++;
	}

	*sz = cnt;

	free(nv);
	return vlst;
}

SUBNET_T *get_mtsubnet(SUBNET_T *sublst, size_t *lst_sz)
{
	char *nv = NULL, *nvp = NULL, *b;
	char *idx, *ifname, *addr, *netmask;
	char *dhcp_enable, *dhcp_min, *dhcp_max, *dhcp_lease;
	char *domain_name, *dns, *wins, *dhcp_res, *dhcp_res_idx;
	char subnet[INET_ADDRSTRLEN + 1];

	int dnscnt = 0;
	char *duptr, *subptr, *dnstr;

	size_t cnt = 0;

	if (lst_sz)
		*lst_sz = 0;

	if (!(nv = nvp = strdup(nvram_safe_get("subnet_rl"))))
		return NULL;

	while ((b = strsep(&nvp, "<")) != NULL) {
		if (vstrsep(b, ">", &idx, &ifname, &addr, &netmask, &dhcp_enable, &dhcp_min, &dhcp_max,
			            &dhcp_lease, &domain_name, &dns, &wins, &dhcp_res, &dhcp_res_idx) != SUBNET_LIST_MAX_PARAM)
			continue;

		if (cnt >= MTLAN_MAXINUM)
			break;

		if (idx && *idx)
			sublst[cnt].idx = strtol(idx, NULL, 10);
		if (ifname && *ifname)
			strlcpy(sublst[cnt].ifname, ifname, sizeof(sublst[cnt].ifname));
		if (addr && *addr)
			strlcpy(sublst[cnt].addr, addr, sizeof(sublst[cnt].addr));
		if (netmask && *netmask)
			strlcpy(sublst[cnt].netmask, netmask, sizeof(sublst[cnt].netmask));
		if (dhcp_enable && *dhcp_enable)
			sublst[cnt].dhcp_enable = (strtol(dhcp_enable, NULL, 10) == 0) ? 0 : 1;
		if (dhcp_min && *dhcp_min)
			strlcpy(sublst[cnt].dhcp_min, dhcp_min, sizeof(sublst[cnt].dhcp_min));
		if (dhcp_max && *dhcp_max)
			strlcpy(sublst[cnt].dhcp_max, dhcp_max, sizeof(sublst[cnt].dhcp_max));
		if (dhcp_lease && *dhcp_lease)
			sublst[cnt].dhcp_lease = strtol(dhcp_lease, NULL, 10);
		if (domain_name && *domain_name)
			strlcpy(sublst[cnt].domain_name, domain_name, sizeof(sublst[cnt].domain_name));
		if (dns && *dns)
			dnscnt = 0;
			duptr = subptr = strdup(dns);
			if (duptr) {
				while ((dnstr = strsep(&subptr, ",")) != NULL) {
					if (dnscnt > 1)
						break;
					strlcpy(sublst[cnt].dns[dnscnt], dnstr, sizeof(sublst[cnt].dns[dnscnt]));
					dnscnt++;
				}
				free(duptr);
			}
		if (wins && *wins)
			strlcpy(sublst[cnt].wins, wins, sizeof(sublst[cnt].wins));
		if (dhcp_res && *dhcp_res)
			sublst[cnt].dhcp_res = strtol(dhcp_res, NULL, 10);
		if (dhcp_res && *dhcp_res_idx)
			sublst[cnt].dhcp_res_idx = strtol(dhcp_res_idx, NULL, 10);

		strlcpy(sublst[cnt].subnet, calc_subnet(addr, v4mask2int(netmask), subnet, sizeof(subnet)), sizeof(sublst[cnt].subnet));
		sublst[cnt].prefixlen = v4mask2int(netmask);

		cnt++;
	}

	*lst_sz = cnt;

	free(nv);
	return sublst;
}

static SUBNET_T *get_default_lan(SUBNET_T *ptr)
{
	char subnet[INET_ADDRSTRLEN + 1];

	if (!ptr)
		return NULL;

	memset(ptr, 0, sizeof(SUBNET_T));

	strlcpy(ptr->ifname, nvram_safe_get("lan_ifname"), sizeof(ptr->ifname));
	strlcpy(ptr->addr, nvram_safe_get("lan_ipaddr"), sizeof(ptr->addr));
	strlcpy(ptr->netmask, nvram_safe_get("lan_netmask"), sizeof(ptr->netmask));
	strlcpy(ptr->subnet, calc_subnet(ptr->addr, v4mask2int(ptr->netmask), subnet, sizeof(subnet)), sizeof(ptr->subnet));
	ptr->prefixlen = v4mask2int(nvram_safe_get("lan_netmask"));
	ptr->dhcp_enable = nvram_get_int("dhcp_enable_x");
	strlcpy(ptr->dhcp_min, nvram_safe_get("dhcp_start"), sizeof(ptr->dhcp_min));
	strlcpy(ptr->dhcp_max, nvram_safe_get("dhcp_end"), sizeof(ptr->dhcp_max));
	ptr->dhcp_lease = nvram_get_int("dhcp_lease");
	strlcpy(ptr->domain_name, nvram_safe_get("lan_domain"), sizeof(ptr->domain_name));
	strlcpy(ptr->dns[0], nvram_safe_get("dhcp_dns1_x"), sizeof(ptr->dns[0]));
	strlcpy(ptr->dns[1], nvram_safe_get("dhcp_dns2_x"), sizeof(ptr->dns[1]));
	strlcpy(ptr->wins, nvram_safe_get("dhcp_wins_x"), sizeof(ptr->wins));
	ptr->dhcp_res = nvram_get_int("dhcp_static_x");
	ptr->dhcp_res_idx = 0; /* no reference */

	return ptr;
}

static SUBNET_T *get_subnet_by_idx(int idx, SUBNET_T *lst, size_t lst_sz)
{
	int i;
	SUBNET_T *ptr = NULL;

	if (!lst)
		return NULL;
	for (i = 0; i < lst_sz; i++) {
		if (lst[i].idx == idx) {
			ptr = &lst[i];
			return ptr;
		}
	}
	return NULL;
}

static VLAN_T *get_vlan_by_idx(int idx, VLAN_T *lst, size_t lst_sz)
{
	int i;
	VLAN_T *ptr = NULL;

	if (!lst)
		return NULL;
	for (i = 0; i < lst_sz; i++) {
		if (lst[i].idx == idx) {
			ptr = &lst[i];
			return ptr;
		}
	}
	return NULL;
}

MTLAN_T *get_mtlan(MTLAN_T *nwlst, size_t *lst_sz)
{
	char *nv = NULL, *nvp = NULL, *b;

	/* basic necessary parameters */
	char *sdn_idx, *sdn_name, *sdn_enable;
	char *vlan_idx, *subnet_idx, *apg_idx;

	/* continue to add parameters */
	char *vpnc_idx = NULL, *vpns_idx = NULL, *dnsf_idx = NULL, *urlf_idx = NULL, *nwf_idx = NULL;
	char *cp_idx = NULL, *gre_idx = NULL, *fw_idx = NULL, *killsw_sw = NULL, *ahs_sw = NULL;
	char *wan_idx = NULL;

	char idx[4], *next;

	size_t vlan_sz = 0, subnet_sz = 0;
	size_t cnt = 0;

	int __cnt = 0;

	if (lst_sz)
		*lst_sz = 0;

	if (!nwlst)
		return NULL;

	VLAN_T   *pvlan_rl = (VLAN_T *)INIT_MTLAN(sizeof(VLAN_T));
	SUBNET_T *psubnet_rl = (SUBNET_T *)INIT_MTLAN(sizeof(SUBNET_T));

	if (!pvlan_rl || !psubnet_rl) {
		nwlst = NULL;
		goto MTEND;
	}

	if (!get_mtvlan(pvlan_rl, &vlan_sz)) {
		nwlst = NULL;
		goto MTEND;
	}
	if (!get_mtsubnet(psubnet_rl, &subnet_sz)) {
		nwlst = NULL;
		goto MTEND;
	}
	if (!(nvp = nv = strdup(nvram_safe_get("sdn_rl")))) {
		nwlst = NULL;
		goto MTEND;
	}


	while ((b = strsep(&nvp, "<")) != NULL) {

		/* init */
		vpnc_idx = vpns_idx = dnsf_idx = urlf_idx = nwf_idx = NULL;
		cp_idx = gre_idx = fw_idx = killsw_sw = ahs_sw = NULL;
		wan_idx = NULL;

		if (vstrsep(b, ">", &sdn_idx, &sdn_name, &sdn_enable, &vlan_idx, &subnet_idx,
				    &apg_idx, &vpnc_idx, &vpns_idx, &dnsf_idx,
				    &urlf_idx, &nwf_idx, &cp_idx, &gre_idx, &fw_idx,
				    &killsw_sw, &ahs_sw, &wan_idx) < SDN_LIST_BASIC_PARAM)
			continue;

		if (cnt >= MTLAN_MAXINUM)
			break;

		if (!((vlan_idx && *vlan_idx) && (subnet_idx && *subnet_idx)))
			continue;

		if (strtol(vlan_idx, NULL, 10) > 0 && strtol(subnet_idx, NULL, 10) > 0) {

			VLAN_T   *pvlan_rl_idx = get_vlan_by_idx(strtol(vlan_idx, NULL, 10), pvlan_rl, vlan_sz);
			SUBNET_T *psubnet_rl_idx = get_subnet_by_idx(strtol(subnet_idx, NULL, 10), psubnet_rl, subnet_sz);

			if (!pvlan_rl_idx || !psubnet_rl_idx)
				continue;

			memcpy(&nwlst[cnt].vid, &pvlan_rl_idx->vid, sizeof(nwlst[cnt].vid));
			memcpy(&nwlst[cnt].nw_t, psubnet_rl_idx, sizeof(nwlst[cnt].nw_t));

		} else { /* Get Default LAN Info */

			if (!get_default_lan(&nwlst[cnt].nw_t))
				continue;
			nwlst[cnt].vid = 0;
		}

		if (sdn_enable && *sdn_enable)
			nwlst[cnt].enable = strtol(sdn_enable, NULL, 10);
		if (sdn_idx && *sdn_idx)
			nwlst[cnt].sdn_t.sdn_idx = strtol(sdn_idx, NULL, 10);
		if (apg_idx && *apg_idx)
			nwlst[cnt].sdn_t.apg_idx = strtol(apg_idx, NULL, 10);
		if (vpnc_idx && *vpnc_idx)
			nwlst[cnt].sdn_t.vpnc_idx = strtol(vpnc_idx, NULL, 10);
		if (vpns_idx && *vpns_idx) {
			__cnt = 0;
			__foreach (idx, vpns_idx, next, ",") {
				if (__cnt < MTLAN_VPNS_MAXINUM) {
					nwlst[cnt].sdn_t.vpns_idx_rl[__cnt] = strtol(idx, NULL, 10);
					__cnt++;
				}
			}
		}
		if (dnsf_idx && *dnsf_idx)
			nwlst[cnt].sdn_t.dnsf_idx = strtol(dnsf_idx, NULL, 10);
		if (urlf_idx && *urlf_idx)
			nwlst[cnt].sdn_t.urlf_idx = strtol(urlf_idx, NULL, 10);
		if (nwf_idx && *nwf_idx)
			nwlst[cnt].sdn_t.nwf_idx = strtol(nwf_idx, NULL, 10);
		if (cp_idx && *cp_idx)
			nwlst[cnt].sdn_t.cp_idx = strtol(cp_idx, NULL, 10);
		if (gre_idx && *gre_idx) {
			__cnt = 0;
			__foreach (idx, gre_idx, next, ",") {
				if (__cnt < MTLAN_GRE_MAXINUM) {
					nwlst[cnt].sdn_t.gre_idx_rl[__cnt] = strtol(idx, NULL, 10);
					__cnt++;
				}
			}
		}
		if (fw_idx && *fw_idx)
			nwlst[cnt].sdn_t.fw_idx = strtol(fw_idx, NULL, 10);
		if (killsw_sw && *killsw_sw)
			nwlst[cnt].sdn_t.killsw_sw = strtol(killsw_sw, NULL, 10);
		if (ahs_sw && *ahs_sw)
			nwlst[cnt].sdn_t.ahs_sw = strtol(ahs_sw, NULL, 10);
		if (wan_idx && *wan_idx)
			nwlst[cnt].sdn_t.wan_idx = strtol(wan_idx, NULL, 10);

		cnt++;
	}

	*lst_sz = cnt;

	free(nv);

MTEND:
	FREE_MTLAN((void *)pvlan_rl);
	FREE_MTLAN((void *)psubnet_rl);
	return nwlst;
}

MTLAN_T *get_mtlan_by_vid(const unsigned int vid, MTLAN_T *nwlst, size_t *lst_sz)
{
	int i;
	size_t r = 0, mtl_sz = 0;

	MTLAN_T *pmtl = (MTLAN_T *)INIT_MTLAN(sizeof(MTLAN_T));

	if (lst_sz)
		*lst_sz = 0;

	if (!get_mtlan(pmtl, &mtl_sz)) {
		FREE_MTLAN((void *)pmtl);
		return NULL;
	}

	for (i = 0; i < mtl_sz; i++) {
		if (pmtl[i].vid == vid) {
			memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
			r++;
		}
	}

	*lst_sz = r;
	FREE_MTLAN((void *)pmtl);
	return nwlst;
}

MTLAN_T *get_mtlan_by_idx(SDNFT_TYPE type, const unsigned int idx, MTLAN_T *nwlst, size_t *lst_sz)
{
	int i, j;
	size_t r = 0;
	size_t mtl_sz = 0;

	if (lst_sz)
		*lst_sz = 0;

	if (type >= SDNFT_TYPE_MAX)
		return NULL;

	MTLAN_T *pmtl = (MTLAN_T *)INIT_MTLAN(sizeof(MTLAN_T));

	if (!get_mtlan(pmtl, &mtl_sz)) {
		FREE_MTLAN((void *)pmtl);
		return NULL;
	}

	for (i = 0; i < mtl_sz; i++) {
		if (type == SDNFT_TYPE_SDN) {
			if (pmtl[i].sdn_t.sdn_idx == idx) {
				memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
				r++;
			}
		} else if (type == SDNFT_TYPE_APG) {
			if (pmtl[i].sdn_t.apg_idx == idx) {
				memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
				r++;
			}
		} else if (type == SDNFT_TYPE_VPNC) {
			if (pmtl[i].sdn_t.vpnc_idx == idx) {
				memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
				r++;
			}
		} else if (type == SDNFT_TYPE_VPNS) {
			for (j = 0; j < MTLAN_VPNS_MAXINUM; j++) {
				if (pmtl[i].sdn_t.vpns_idx_rl[j] == idx) {
					memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
					r++;
					break;
				}
			}
		} else if (type == SDNFT_TYPE_URLF) {
			if (pmtl[i].sdn_t.urlf_idx == idx) {
				memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
				r++;
			}
		} else if (type == SDNFT_TYPE_NWF) {
			if (pmtl[i].sdn_t.nwf_idx == idx) {
				memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
				r++;
			}
		} else if (type == SDNFT_TYPE_GRE) {
			for (j = 0; j < MTLAN_GRE_MAXINUM; j++) {
				if (pmtl[i].sdn_t.gre_idx_rl[j] == idx) {
					memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
					r++;
					break;
				}
			}
		} else if (type == SDNFT_TYPE_FW) {
			if (pmtl[i].sdn_t.fw_idx == idx) {
				memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
				r++;
			}
		} else if (type == SDNFT_TYPE_KILLSW) {
			if (pmtl[i].sdn_t.killsw_sw == idx) {
				memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
				r++;
			}
		} else if (type == SDNFT_TYPE_AHS) {
			if (pmtl[i].sdn_t.ahs_sw == idx) {
				memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
				r++;
			}
		} else if (type == SDNFT_TYPE_WAN) {
			if (pmtl[i].sdn_t.wan_idx == idx) {
				memcpy(&nwlst[r], &pmtl[i], sizeof(MTLAN_T));
				r++;
			}
		}
	}

	*lst_sz = r;
	FREE_MTLAN((void *)pmtl);

	return nwlst;
}

size_t get_mtlan_cnt()
{
	char *nv = NULL, *nvp = NULL, *b;
	size_t cnt = 0;
	if (!(nvp = nv = strdup(nvram_safe_get("sdn_rl"))))
		return 0;
	while ((b = strsep(&nvp, "<")) != NULL)
		cnt++;
	free(nv);
	return (cnt - 1);
}

static VPN_PROTO_T _get_real_vpn_proto(const char* proto)
{
	if (!proto)
		return VPN_PROTO_UNKNOWN;
	else if (!strcmp(proto, VPN_PROTO_OVPN_STR))
		return VPN_PROTO_OVPN;
	else if (!strcmp(proto, VPN_PROTO_PPTP_STR))
		return VPN_PROTO_PPTP;
	else if (!strcmp(proto, VPN_PROTO_L2TP_STR))
		return VPN_PROTO_L2TP;
	else if (!strcmp(proto, VPN_PROTO_WG_STR))
		return VPN_PROTO_WG;
	else if (!strcmp(proto, VPN_PROTO_HMA_STR))
		return VPN_PROTO_OVPN;
	else if (!strcmp(proto, VPN_PROTO_NORDVPN_STR))
		return VPN_PROTO_WG;
	else if (!strcmp(proto, VPN_PROTO_IPSEC_STR))
		return VPN_PROTO_IPSEC;
	else
		return VPN_PROTO_UNKNOWN;
}

static char* _get_vpn_proto_str(VPN_PROTO_T proto)
{
	switch(proto) {
		case VPN_PROTO_PPTP:
			return VPN_PROTO_PPTP_STR;
		case VPN_PROTO_L2TP:
			return VPN_PROTO_L2TP_STR;
		case VPN_PROTO_OVPN:
			return VPN_PROTO_OVPN_STR;
		case VPN_PROTO_IPSEC:
			return VPN_PROTO_IPSEC_STR;
		case VPN_PROTO_WG:
			return VPN_PROTO_WG_STR;
		case VPN_PROTO_L2GRE:
			return VPN_PROTO_L2GRE_STR;
		case VPN_PROTO_L3GRE:
			return VPN_PROTO_L3GRE_STR;
		default:
			return "UNKNOWN";
	}
}

int get_vpnc_idx_by_proto_unit(VPN_PROTO_T proto, int unit)
{
	char *nv = NULL, *nvp = NULL, *b = NULL;
	char *desc, *nv_proto, *nv_unit, *username, *passwd, *active, *vpnc_idx, *region, *conntype;
	int ret = 0;
	VPN_PROTO_T nv_real_proto;

	//pptp l2tp handle by vpn fusion
	if (proto == VPN_PROTO_PPTP || proto == VPN_PROTO_L2TP)
		return 0;

	nv = nvp = strdup(nvram_safe_get("vpnc_clientlist"));
	if (nv) {
		while ((b = strsep(&nvp, "<"))) {
			if (vstrsep(b, ">", &desc, &nv_proto, &nv_unit, &username, &passwd, &active, &vpnc_idx, &region, &conntype) < 4)
				continue;
			if (!nv_proto || !nv_unit || !vpnc_idx)
				continue;
			nv_real_proto = _get_real_vpn_proto(nv_proto);
			if (atoi(nv_unit) == unit && nv_real_proto == proto) {
				ret = atoi(vpnc_idx);
				break;
			}
		}
		free(nv);
	}
	return (ret);
}

int get_vpns_idx_by_proto_unit(VPN_PROTO_T proto, int unit)
{
	char *nv = NULL, *nvp = NULL, *b = NULL;
	char *desc, *nv_proto, *nv_unit, *active, *vpns_idx;
	int ret = 0;
	int nounit = 0;

	// no unit design
	if (proto == VPN_PROTO_PPTP || proto == VPN_PROTO_IPSEC)
		nounit = 1;

	nv = nvp = strdup(nvram_safe_get("vpns_rl"));
	if (nv) {
		while ((b = strsep(&nvp, "<"))) {
			if (vstrsep(b, ">", &desc, &nv_proto, &nv_unit, &active, &vpns_idx) < 2)
				continue;
			if (!nv_proto || !vpns_idx)
				continue;
			if (!strcmp(nv_proto, _get_vpn_proto_str(proto))) {
				if (nounit) {
					ret = atoi(vpns_idx);
					break;
				}
				else if (nv_unit && atoi(nv_unit) == unit) {
					ret = atoi(vpns_idx);
					break;
				}
			}
		}
		free(nv);
	}
	return (ret);
}

int get_gre_idx_by_proto_unit(VPN_PROTO_T proto, int unit)
{
	char *nv = NULL, *nvp = NULL, *b = NULL;
	char *desc, *nv_proto, *nv_unit, *active, *gre_idx;
	int ret = 0;

	nv = nvp = strdup(nvram_safe_get("gre_rl"));
	if (nv) {
		while ((b = strsep(&nvp, "<"))) {
			if (vstrsep(b, ">", &desc, &nv_proto, &nv_unit, &active, &gre_idx) < 3)
				continue;
			if (!nv_proto || !gre_idx)
				continue;
			if (!strcmp(nv_proto, _get_vpn_proto_str(proto))
			 && nv_unit && atoi(nv_unit) == unit
			) {
				ret = atoi(gre_idx);
				break;
			}
		}
		free(nv);
	}
	return (ret);
}

char* get_vpns_ifname_by_vpns_idx(int vpns_idx, char* buf, size_t len)
{
	char *nv = NULL, *nvp = NULL, *b = NULL;
	char *desc, *nv_proto, *nv_unit, *active, *nv_vpns_idx;
	int ret = 0;

	nv = nvp = strdup(nvram_safe_get("vpns_rl"));
	if (nv) {
		while ((b = strsep(&nvp, "<"))) {
			if (vstrsep(b, ">", &desc, &nv_proto, &nv_unit, &active, &nv_vpns_idx) < 2)
				continue;
			else if (!nv_proto || !nv_vpns_idx)
				continue;
			else if (vpns_idx != atoi(nv_vpns_idx))
				continue;
			else if (!strcmp(nv_proto, VPN_PROTO_WG_STR)) {
				if (nv_unit) {
					snprintf(buf, len, "wgs%d", atoi(nv_unit));
					ret = 1;
				}
			}
			else if (!strcmp(nv_proto, VPN_PROTO_OVPN_STR)) {
				if (nv_unit) {
					snprintf(buf, len, "tun%d", OVPN_SERVER_BASE + atoi(nv_unit));
					ret = 1;
				}
			}
			else if (!strcmp(nv_proto, VPN_PROTO_PPTP_STR)) {
				snprintf(buf, len, "pptp+");
				ret = 1;
			}
		}
		free(nv);
	}
	return (ret) ? buf : NULL;
}

char* get_vpns_iprange_by_vpns_idx(int vpns_idx, char* buf, size_t len)
{
	char *nv = NULL, *nvp = NULL, *b = NULL;
	char *desc, *nv_proto, *nv_unit, *active, *nv_vpns_idx;
	int ret = 0;

	nv = nvp = strdup(nvram_safe_get("vpns_rl"));
	if (nv) {
		while ((b = strsep(&nvp, "<"))) {
			if (vstrsep(b, ">", &desc, &nv_proto, &nv_unit, &active, &nv_vpns_idx) < 2)
				continue;
			else if (!nv_proto || !nv_vpns_idx)
				continue;
			else if (vpns_idx != atoi(nv_vpns_idx))
				continue;
			else if (!strcmp(nv_proto, VPN_PROTO_IPSEC_STR)) {
				//ipsec_profile_1=4>Host-to-Net>null>null>wan>>1>12345678>null>null>null>null>null>1>10.10.10> ...
				int ipsec_vsubnet_idx = 13;
				char ipsec_prof[256] = {0};
				char *p, *p_end;
				nvram_safe_get_r("ipsec_profile_1", ipsec_prof, sizeof(ipsec_prof));
				p = strpbrk(ipsec_prof, ">");
				while (ipsec_vsubnet_idx--)
					p = strpbrk(p+1, ">");
				p_end = strpbrk(p+1, ">");
				*p_end = '\0';
				snprintf(buf, len, "%s.1-%s.254", p+1, p+1);
				ret = 1;
			}
		}
		free(nv);
	}
	return (ret) ? buf : NULL;
}

VPN_VPNX_T* get_vpnx_by_vpnc_idx(VPN_VPNX_T* vpnx, int vpnc_idx)
{
	char *nv = NULL, *nvp = NULL, *b = NULL;
	char *desc, *nv_proto, *nv_unit, *username, *passwd, *active, *nv_vpnc_idx, *region, *conntype;
	int ret = 0;

	if (!vpnx)
		return NULL;

	nv = nvp = strdup(nvram_safe_get("vpnc_clientlist"));
	if (nv) {
		while ((b = strsep(&nvp, "<"))) {
			if (vstrsep(b, ">", &desc, &nv_proto, &nv_unit, &username, &passwd, &active, &nv_vpnc_idx, &region, &conntype) < 4)
				continue;
			if (!nv_proto || !nv_vpnc_idx)
				continue;
			else if (vpnc_idx != atoi(nv_vpnc_idx))
				continue;
			vpnx->proto = _get_real_vpn_proto(nv_proto);
			if (nv_unit &&
				(  vpnx->proto == VPN_PROTO_OVPN
				|| vpnx->proto == VPN_PROTO_WG
			)) {
				vpnx->unit = atoi(nv_unit);
			}
			ret = 1;
			break;
		}
		free(nv);
	}

	return (ret) ? vpnx : NULL;
}

VPN_VPNX_T* get_vpnx_by_vpns_idx(VPN_VPNX_T* vpnx, int vpns_idx)
{
	char *nv = NULL, *nvp = NULL, *b = NULL;
	char *desc, *nv_proto, *nv_unit, *active, *nv_vpns_idx;
	int ret = 0;

	if (!vpnx)
		return NULL;

	nv = nvp = strdup(nvram_safe_get("vpns_rl"));
	if (nv) {
		while ((b = strsep(&nvp, "<"))) {
			if (vstrsep(b, ">", &desc, &nv_proto, &nv_unit, &active, &nv_vpns_idx) < 2)
				continue;
			else if (!nv_proto || !nv_vpns_idx)
				continue;
			else if (vpns_idx != atoi(nv_vpns_idx))
				continue;
			else if (!strcmp(nv_proto, VPN_PROTO_WG_STR)) {
				vpnx->proto = VPN_PROTO_WG;
				if (nv_unit)
					vpnx->unit = atoi(nv_unit);
				ret = 1;
			}
			else if (!strcmp(nv_proto, VPN_PROTO_OVPN_STR)) {
				vpnx->proto = VPN_PROTO_OVPN;
				if (nv_unit)
					vpnx->unit = atoi(nv_unit);
				ret = 1;
			}
			else if (!strcmp(nv_proto, VPN_PROTO_IPSEC_STR)) {
				vpnx->proto = VPN_PROTO_IPSEC;
				ret = 1;
			}
			else if (!strcmp(nv_proto, VPN_PROTO_PPTP_STR)) {
				vpnx->proto = VPN_PROTO_PPTP;
				ret = 1;
			}
			break;
		}
		free(nv);
	}

	return (ret) ? vpnx : NULL;
}

VPN_VPNX_T* get_vpnx_by_gre_idx(VPN_VPNX_T* vpnx, int gre_idx)
{
	char *nv = NULL, *nvp = NULL, *b = NULL;
	char *desc, *nv_proto, *nv_unit, *active, *nv_gre_idx;
	int ret = 0;

	if (!vpnx)
		return NULL;

	nv = nvp = strdup(nvram_safe_get("gre_rl"));
	if (nv) {
		while ((b = strsep(&nvp, "<"))) {
			if (vstrsep(b, ">", &desc, &nv_proto, &nv_unit, &active, &nv_gre_idx) < 2)
				continue;
			else if (!nv_proto || !nv_gre_idx)
				continue;
			else if (gre_idx != atoi(nv_gre_idx))
				continue;
			else if (!strcmp(nv_proto, VPN_PROTO_L2GRE_STR)) {
				vpnx->proto = VPN_PROTO_L2GRE;
				if (nv_unit)
					vpnx->unit = atoi(nv_unit);
				ret = 1;
			}
			else if (!strcmp(nv_proto, VPN_PROTO_L3GRE_STR)) {
				vpnx->proto = VPN_PROTO_L3GRE;
				if (nv_unit)
					vpnx->unit = atoi(nv_unit);
				ret = 1;
			}
			break;
		}
		free(nv);
	}

	return (ret) ? vpnx : NULL;
}
