/*******************************************************************************
 **
 ** FILE NAME	: ltq_mpe_tmpl_buf.c
 ** PROJECT	: PPA
 ** MODULES	: PPA API (Routing/Bridging Acceleration APIs)
 **
 ** DATE	: April 2015
 ** AUTHOR	: Lantiq
 ** DESCRIPTION	: The session template buffer construction routines.
 ** COPYRIGHT	:		Copyright (c) 2013
 **			Lantiq Deutschland GmbH
 **		 Am Campeon 3; 85579 Neubiberg, Germany
 **
 **	 For licensing information, see the file 'LICENSE' in the root folder of
 **	 this software module.
 **
 ** HISTORY
 ** $Date		$Author				$Comment
 ** April 2015	 	Syed				Initiate Version
 *******************************************************************************/
/*
 *	Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>

#include <linux/ip.h>
#include <linux/swap.h>
#include <linux/ipv6.h>
#include <linux/if_vlan.h>
#include <net/ip.h>
#include <net/route.h>
#include <net/protocol.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/ipv6.h>
#include <net/ip6_tunnel.h>
#include <net/ip_tunnels.h>
#include <net/xfrm.h>

/*
 *	PPA Specific Head File
 */
#include <net/ppa/ppa_api_common.h>
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_hook.h>
#include <net/ppa/ppa_hal_wrapper.h>
#include "ltq_mpe_api.h"
#include "../../ppa_api/ppa_api_netif.h"
#include "../../ppa_api/ppa_api_session.h"
#include <net/ppa/ppa_stack_al.h>

#define UDP_HDR_LEN		8
#define IPV4_HDR_LEN		20	/*assuming no option fields are present*/
#define IPV6_HDR_LEN		40
#define ETH_HLEN		14	/* Total octets in header.	 */

#define L2TP_HLEN		38 /* IPv4=20 + UDP=8 + L2TP_HDR=8 + PPP_HDR=4 */
#define L2TP_HDR_LEN		6

#define PROTO_FAMILY_IP	 	2
#define PROTO_FAMILY_IPV6 	10

#define PPP_LEN			4
#define PPPOE_LEN		6
#define PPPOE_HLEN		8

#define IPSEC_HLEN		20

#define PPPOE_IPV4_TAG	0x0021
#define PPPOE_IPV6_TAG	0x0057
#define FLAG_TC_REMARK	0x40000000

#define VXLAN_HDR_LEN		8

#undef NIPQUAD
/*! \def NIPQUAD
	\brief Macro that specifies NIPQUAD definition for printing IPV4 address
 */
#define NIPQUAD(addr) \
	((unsigned char *)&addr)[0], \
((unsigned char *)&addr)[1], \
((unsigned char *)&addr)[2], \
((unsigned char *)&addr)[3]


//#define DEBUG_TMPL_BUFFER 1
#ifdef DEBUG_TMPL_BUFFER
/*
 * \brief	dump data
 * \param[in] len length of the data buffer
 * \param[in] pData Pointer to data to dump
 *
 * \return void No Value
 */
#define dbg_print printk
static inline void dump_data(uint32_t len, char *pData)
{
	uint32_t bytes = 1;

	while( len ) {

		dbg_print("%2.2X%c",(uint8_t)(*pData), (bytes%16)?' ':'\n' ) ;
		--len;
		bytes++;
		pData++;
	}
	dbg_print("\n");
}


static void print_action_info(struct session_action* s_act, uint8_t ipV4)
{
	dbg_print("Template buffer: Session Action\n");
	dbg_print("Protocol			= %d\n",s_act->protocol);
	dbg_print("dst_pmac_port_num	= %d\n",s_act->dst_pmac_port_num);
	dbg_print("dst_pmac_port_list[0]	= %d\n",s_act->dst_pmac_port_list[0]);

	dbg_print("routing_flag		= %d\n",s_act->routing_flag);

	dbg_print("new_src_ip_en		= %d\n",s_act->new_src_ip_en);
	if(ipV4)
		dbg_print("new_src_ip		= %pI4\n",&s_act->new_src_ip.ip4.ip);
	else
		dbg_print("new_src_ip		= %pI6\n",&s_act->new_src_ip.ip6.ip);

	dbg_print("new_dst_ip_en		= %d\n",s_act->new_dst_ip_en);
	if(ipV4)
		dbg_print("new_dst_ip		= %pI4\n",&s_act->new_dst_ip.ip4.ip);
	else
		dbg_print("new_dst_ip		= %pI6\n",&s_act->new_dst_ip.ip6.ip);

	dbg_print("pppoe_offset_en		= %d\n",s_act->pppoe_offset_en);
	dbg_print("pppoe_offset		= %d\n",s_act->pppoe_offset);

	dbg_print("tunnel_ip_offset_en	= %d\n",s_act->tunnel_ip_offset_en);
	dbg_print("tunnel_ip_offset		= %d\n",s_act->tunnel_ip_offset);

	dbg_print("tunnel_udp_offset_en	= %d\n",s_act->tunnel_udp_offset_en);
	dbg_print("tunnel_udp_offset	= %d\n",s_act->tunnel_udp_offset);

	dbg_print("in_eth_iphdr_offset_en	= %d\n",s_act->in_eth_iphdr_offset_en);
	dbg_print("in_eth_iphdr_offset	 = %d\n",s_act->in_eth_iphdr_offset);

	dbg_print("tunnel_type		= %d\n",s_act->tunnel_type);
	dbg_print("tunnel_rm_en		= %d\n",s_act->tunnel_rm_en);
	dbg_print("tunnel id		= %d\n",s_act->tunnel_id);

	dbg_print("pkt_len_delta		= %d\n",s_act->pkt_len_delta);
	dbg_print("templ_len		= %d\n",s_act->templ_len);

	dump_data(s_act->templ_len,s_act->templ_buf);
}
#else

#define dbg_print(fmt , ...)
#define print_action_info(s_act, ipV4)

#endif

#define IsPppoeSession(flags)		( (flags) & SESSION_VALID_PPPOE )
#define IsLanSession(flags)		( (flags) & SESSION_LAN_ENTRY )
#define IsValidVlanIns(flags)		( (flags) & SESSION_VALID_VLAN_INS )
#define IsValidVlanRm(flags)		( (flags) & SESSION_VALID_VLAN_RM )
#define IsValidOutVlanIns(flags)	( (flags) & SESSION_VALID_OUT_VLAN_INS)
#define IsValidOutVlanRm(flags)		( (flags) & SESSION_VALID_OUT_VLAN_RM)
#define IsIpv6Session(flags)		( (flags) & SESSION_IS_IPV6)
#define IsTunneledSession(flags)	( (flags) & (SESSION_TUNNEL_DSLITE | \
						SESSION_TUNNEL_6RD | \
						SESSION_VALID_PPPOL2TP))
#define IsDsliteSession(flags)		( (flags) & SESSION_TUNNEL_DSLITE )
#define Is6rdSession(flags)		( (flags) & SESSION_TUNNEL_6RD)
#define IsL2TPSession(flags)		( (flags) & SESSION_VALID_PPPOL2TP)
#define IsValidNatIP(flags)		( (flags) & SESSION_VALID_NAT_IP)
#define IsValidNatPort(flags)         ( (flags) & SESSION_VALID_NAT_PORT)
#if IS_ENABLED(CONFIG_PPA_NAT_LOOP_BACK)
#define IsValidNatLoopback(flags)     ( (flags) & SESSION_FLAG2_VALID_NAT_LOOPBACK)
#endif
 
#define IsBridgedSession(flags)		( (flags) & SESSION_FLAG2_BRIDGED_SESSION)
#define IsGreSession(flags)		( (flags) & SESSION_FLAG2_GRE)
#define IsIpsecSession(flags)		( (flags) & SESSION_FLAG2_VALID_IPSEC_OUTBOUND)
#define IsIpsecTransSession(flags)	( (flags) & SESSION_FLAG2_VALID_IPSEC_TRANS)

#define IsVxlanSession(flags)		((flags) & SESSION_FLAG2_VXLAN)
#if IS_ENABLED(CONFIG_INTEL_IPQOS_MPE_DS_ACCEL)
#define IsDsQosSession(p_item)		( ((p_item)->flag2) & SESSION_FLAG2_DS_MPE_QOS)
#endif

static inline void ppa_htons(uint16_t *ptr, uint16_t val )
{
	*ptr = val;
}

static inline void ppa_htonl(uint32_t *ptr, uint32_t val )
{
	*ptr = val;
}

#if IS_ENABLED(CONFIG_INTEL_IPQOS_MPE_DS_ACCEL)
static inline
uint32_t set_qmap_from_ing_grp(PKT_INFO pkt) {
	uint32_t qmap = 0;	// DEC | ENC | MPE2 | MPE1 | FLOWID(2 bits)
#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
	uint32_t ing_grp = ((pkt.extmark & SESSION_ING_GROUP) >> ING_GROUP_POS);
#else
	uint32_t ing_grp = ((pkt.mark & SESSION_ING_GROUP) >> ING_GROUP_POS);
#endif
	switch(ing_grp) {
		case 0: qmap = 4;	// 0|0|0|1|0|0
				break;
		case 1: qmap = 12;	// 0|0|1|1|0|0
				break;
		case 2: qmap = 5;	// 0|0|0|1|0|1
				break;
		case 3: qmap = 13;	// 0|0|1|1|0|1
				break;
	}

	return qmap;
}
#endif /* CONFIG_INTEL_IPQOS_MPE_DS_ACCEL */
int32_t ppa_tmpl_get_underlying_vlan_interface_if_eogre(PPA_NETIF *netif, PPA_NETIF **base_netif, uint8_t *isEoGre)
{
	uint8_t isIPv4Gre = 0;
	PPA_IFNAME underlying_intname[PPA_IF_NAME_SIZE];

	*base_netif = NULL;

	if( ppa_if_is_vlan_if(netif, NULL)) {
		if(ppa_vlan_get_underlying_if(netif, NULL, underlying_intname) == PPA_SUCCESS) {
			netif = ppa_get_netif(underlying_intname); //Get underling layer net dev
		} else {
			return PPA_FAILURE;
		}
	}
	if (ppa_is_gre_netif_type(netif, &isIPv4Gre, isEoGre)) {
		*base_netif = netif;
		return PPA_SUCCESS;
	}

	return PPA_FAILURE;

}

static uint16_t ppa_out_header_len( struct uc_session_node *p_item, uint32_t *delta)
{
	uint16_t headerlength=0;

	uint8_t is_lan = IsLanSession(p_item->flags)?1:0;

	if( ! IsBridgedSession(p_item->flag2) ) {

		if( IsDsliteSession(p_item->flags) ) {
			/* Handle DS-Lite Tunneled sessions */
			if( is_lan ) {
				headerlength += IPV6_HDR_LEN;
				*delta += IPV6_HDR_LEN;
			} else if (ppa_is_EgressGreSession(p_item)) {
				*delta -= IPV6_HDR_LEN;
			}
		} else if( Is6rdSession(p_item->flags) ) {

			/* Handle 6rd Tunneled sessions */
			if( is_lan ) {
				headerlength += IPV4_HDR_LEN;
				*delta += IPV4_HDR_LEN;
			} else if (ppa_is_EgressGreSession(p_item)) {
				*delta -= IPV4_HDR_LEN;
			}
		} else if ( IsL2TPSession(p_item->flags)) {
			/* handle for l2tp Tunneled session */
			if( is_lan ) {
				headerlength += L2TP_HLEN;
				*delta += L2TP_HLEN;
			}
			/*
			 * L2TP ingress session :
			 * To handle optional L2TP header (say, Length) - use tunnel_rm_en=1 instead
			 */
		} else if(IsIpsecSession(p_item->flag2)) {

			if( is_lan ) {
				headerlength += IPSEC_HLEN ;
				*delta += IPSEC_HLEN;
			} else {
				*delta -= IPSEC_HLEN;
			}
		}
	}
	if (IsVxlanSession(p_item->flag2)) {
		if (is_lan) {
			uint16_t hdrlen = 0;
			PPA_NETIF *base_netif = NULL;
			PPA_NETIF *temp_netif = NULL;
			uint8_t is_vxlan;

			temp_netif = p_item->tx_if;
			if (ppa_get_underlying_vlan_interface_if_vxlan(temp_netif, &base_netif, &is_vxlan) == PPA_SUCCESS) {
				ppa_get_vxlan_hdrlen(base_netif, &hdrlen);
			}

			headerlength += hdrlen;
			*delta += hdrlen;
			if (is_vxlan)
				*delta += ETH_HLEN;
		} else {
			*delta -= ETH_HLEN;
		}
	}
	if( IsGreSession(p_item->flag2)) {
		/*Both IPoGRE and EoGRE get tunnel hdr length*/
		if( ppa_is_EgressGreSession(p_item) ) {
			uint16_t hdrlen=0;
			PPA_NETIF *base_netif=NULL;
			PPA_NETIF *temp_netif=NULL;
			uint8_t is_eogre;
			if ( !IsBridgedSession(p_item->flag2) && !is_lan ) {
				temp_netif = p_item->br_tx_if;
			} else {
				temp_netif = p_item->tx_if;
			}
			if(ppa_tmpl_get_underlying_vlan_interface_if_eogre(temp_netif, &base_netif, &is_eogre) == PPA_SUCCESS) {
				ppa_get_gre_hdrlen(base_netif, &hdrlen);
			}
			headerlength += hdrlen;
			*delta += hdrlen;
			if (is_eogre) {
				/* headerlength adjusted later */
				*delta += ETH_HLEN;
			}
		}

		if ( ppa_is_IngressGreSession(p_item) ) {
			uint16_t hdrlen=0;
			PPA_NETIF *base_netif=NULL;
			PPA_NETIF *temp_netif=NULL;
			uint8_t is_eogre;
			if ( !IsBridgedSession(p_item->flag2) && is_lan ) {
				temp_netif = p_item->br_rx_if;
				/* bridged flow learning enabled */
				if (!temp_netif)
					temp_netif = p_item->rx_if;
			} else {
				temp_netif = p_item->rx_if;
			}
			if(ppa_tmpl_get_underlying_vlan_interface_if_eogre(temp_netif, &base_netif, &is_eogre) == PPA_SUCCESS) {
				ppa_get_gre_hdrlen(base_netif, &hdrlen);
			}
			if (!IsBridgedSession(p_item->flag2) || ppa_is_EgressGreSession(p_item))
				*delta -= hdrlen;
			if (is_eogre) {
				*delta -= ETH_HLEN;
			}
		}
	}

	if( IsValidVlanIns(p_item->flags) ) {
		*delta += VLAN_HLEN;
		headerlength += VLAN_HLEN;
	}
	if( IsValidVlanRm(p_item->flags) ) {
		*delta += -VLAN_HLEN;
	}
	if (IsValidOutVlanRm(p_item->flags))
		*delta -= VLAN_HLEN;
	if( IsPppoeSession(p_item->flags) ) {
		if( is_lan ) {
			headerlength += PPPOE_HLEN;
			*delta += PPPOE_HLEN;
		} else {
			*delta += -PPPOE_HLEN;
		}
	}

	if (IsValidOutVlanIns(p_item->flags)) {
		*delta += VLAN_HLEN;
		headerlength += VLAN_HLEN;
	}

	/* No special handling required for PPPOA */
	headerlength += ETH_HLEN;	/* mac header offset */
	return headerlength;
}

static uint32_t ppa_form_dslite_tunnel(PPA_NETIF *dev, uint8_t *hdr)
{
	struct ipv6hdr ip6h;
	struct ip6_tnl *t;

	if (dev->type != ARPHRD_TUNNEL6) {
		return 0;
	}
	t = (struct ip6_tnl *)netdev_priv(dev);

	ppa_memset(&ip6h, 0, sizeof(struct ipv6hdr));
	ip6h.version = 6;
	ip6h.hop_limit = t->parms.hop_limit;
	ip6h.nexthdr = IPPROTO_IPIP;
	ipv6_addr_copy(&ip6h.saddr, &t->parms.laddr);
	ipv6_addr_copy(&ip6h.daddr, &t->parms.raddr);

	ppa_memcpy(hdr, &ip6h, sizeof(struct ipv6hdr));
	return (sizeof(struct ipv6hdr));
}

static uint32_t ppa_form_6rd_tunnel(struct uc_session_node *p_item, uint8_t* hdr)
{
	struct iphdr iph;
	struct ip_tunnel *t;
	PPA_NETIF *dev = p_item->tx_if;

	if (dev->type != ARPHRD_SIT) {
		return 0;
	}
	t = (struct ip_tunnel *)netdev_priv(dev);

	ppa_memset(&iph, 0, sizeof(struct iphdr));
	iph.version = 4;
	iph.ihl	= 5;
	iph.tot_len = 20;
	iph.ttl	= ppa_get_ip_ttl_default();
	iph.protocol = IPPROTO_IPV6;
	iph.saddr = t->parms.iph.saddr;
	iph.daddr = p_item->pkt.sixrd_daddr;
	iph.check = ip_fast_csum((unsigned char *)&iph, iph.ihl);

	ppa_memcpy(hdr, &iph, sizeof(struct iphdr));
	return (sizeof(struct iphdr));
}

void ppa_update_grehdr_inherited(void *s_pkt, PPA_NETIF *gre_netif,
		bool isIPv4Gre, void *grehdr, struct session_action *pSwaHdr)
{
	struct iphdr *iph;
	struct ipv6hdr *ip6h;
	struct ip6_tnl *t;
	uint8_t dsfield_outer = 0;
	bool is_recal_csum = false;

	/* IPv4 fields to be inherited from inner IP header */
	if (isIPv4Gre) {
		iph = (struct iphdr *)grehdr;

		/* ToS (DSCP+ECN) inheritence */
		if ((iph->tos & 0x1) &&
		    ((pSwaHdr->outer_dscp_mode == 1) || (pSwaHdr->outer_dscp_mode == 0))) {

			/* (DSCP+ECN) to be inherited by MPE fw */
			pSwaHdr->outer_dscp_mode = 1;
			iph->tos &= ~0x1;
			is_recal_csum = true;
		} else {

			/* Only ECN is inherited here, no DSCP inheritence */
			if (pSwaHdr->outer_dscp_mode == 1)
				pSwaHdr->outer_dscp_mode = 0;

			iph->tos &= ~0x1;
			iph->tos = INET_ECN_encapsulate(0, ppa_get_pkt_ip_tos(s_pkt));
			is_recal_csum = true;
		}

		/* TTL is inherited here, when not configured explicitly */
		if (iph->ttl == 0) {
			iph->ttl = ppa_get_pkt_ip_ttl(s_pkt);
			is_recal_csum = true;
		}

		/* Re-calculate IPv4 header checksum */
		if (is_recal_csum) {
			iph->check = 0;
			iph->check = ppa_ip_fast_csum((unsigned char *)iph, iph->ihl);
		}
	} else {
		ip6h = (struct ipv6hdr *)grehdr;
		t =  netdev_priv(gre_netif);
		dsfield_outer = 0;

		/* ToS (DSCP+ECN) inheritence */
		if ((t->parms.flags & IP6_TNL_F_USE_ORIG_TCLASS) &&
		    (pSwaHdr->outer_dscp_mode == 0)) {

			/* (DSCP+ECN) to be inherited by MPE fw */
			pSwaHdr->outer_dscp_mode = 1;
		} else if (!((t->parms.flags & IP6_TNL_F_USE_ORIG_TCLASS) &&
			     (pSwaHdr->outer_dscp_mode == 1))) {

			/* Only ECN is inherited here, no DSCP inheritence */
			if (pSwaHdr->outer_dscp_mode == 1)
				pSwaHdr->outer_dscp_mode = 0;

			dsfield_outer |= INET_ECN_encapsulate(0, ppa_get_pkt_ip_tos(s_pkt));
			ipv6_change_dsfield(ip6h, 0, dsfield_outer);
		}
	}
}

static uint32_t ppa_form_ipsec_tunnel_hdr(void *s_pkt, uint8_t* hdr, unsigned isIPv6)
{

	struct iphdr iph;

	PPA_IPADDR	src_ip;
	PPA_IPADDR	dst_ip;

	ppa_get_pkt_src_ip(&src_ip,s_pkt);
	ppa_get_pkt_dst_ip(&dst_ip,s_pkt);

	form_IPv4_header(&iph,src_ip.ip,dst_ip.ip, 50,20);
	ppa_memcpy(hdr , &iph, sizeof(struct iphdr));
	hdr += IPV4_HDR_LEN;
	return PPA_SUCCESS;
}

static uint32_t ppa_form_l2tp_tunnel(struct uc_session_node *p_item, uint8_t* hdr, unsigned isIPv6)
{
	struct iphdr iph;
	struct udphdr udph;
	uint32_t outer_srcip, outer_dstip;

	/* adding IP header to templet buffer */
	ppa_pppol2tp_get_src_addr(p_item->tx_if,&outer_srcip);
	ppa_pppol2tp_get_dst_addr(p_item->tx_if,&outer_dstip);
	form_IPv4_header(&iph, outer_srcip, outer_dstip, 17,38);
	ppa_memcpy(hdr , &iph, sizeof(struct iphdr));
	hdr += IPV4_HDR_LEN;

	/* adding UDP header to templet buffer */
	form_UDP_header(&udph, 0x06a5, 0x06a5,18);
	udph.check = 0x0;
	ppa_memcpy(hdr , &udph, sizeof(struct udphdr));
	hdr += UDP_HDR_LEN;

	/* adding L2TP header to templet buffer*/
	ppa_htons((uint16_t*)hdr, 0x0002);
	ppa_htons((uint16_t*)(hdr + 2), p_item->pkt.pppol2tp_tunnel_id); /* copying l2tp tunnel_id @ appropriate offset */
	ppa_htons((uint16_t*)(hdr + 4), p_item->pkt.pppol2tp_session_id) ; /* copying l2tp session_id @ appropriate offset */
	hdr += L2TP_HDR_LEN ;
	/* adding ppp header to templet buffer */
	*(hdr) = 0xff;
	*(hdr + 1) = 0x03;
	if(isIPv6)
		ppa_htons((uint16_t*)(hdr + 2),	0x0057);
	else
		ppa_htons((uint16_t*)(hdr + 2),	0x0021);

	return 38;
}

/*
 * This function creates the template buffer for mpe
 */
int32_t ppa_form_session_tmpl(PPA_SESSMETA_INFO *metainfo)
{
	int ret = PPA_SUCCESS;
	uint32_t delta = 0;
	uint16_t tlen = 0;
	uint16_t proto_type = ETH_P_IP;
	unsigned isIPv6 = 0;
	uint8_t* hdr;
	uint16_t vtag;

	struct session_action	swaHdr;
	struct session_action	*p_swaHdr;

	uint8_t	isIPv4Gre = 1; /* Valid if session is GRE */
	uint8_t	isEgGreTap = 0;  /* Valid if sessions is GRE */
	uint8_t	isIPv4VxLAN = 1; /* Valid if session is VxLAN IPv4 */
	uint8_t	isvtep = 0;  /* Valid if sessions is VXLAN */
				
	uint16_t hdrlen = 0;
	PPA_NETIF *base_netif=NULL;
	PPA_NETIF *tx_netif=NULL;
	PPA_NETIF *temp_netif=NULL;
	uint8_t is_eogre, is_vxlan;

	bool need_nat = false;
	uint32_t mtu = 0;
	struct uc_session_node *p_item = (struct uc_session_node *)metainfo->p_item;
	struct netif_info *txif_info = (struct netif_info *)metainfo->txif_info;

	if(!p_item || !metainfo->skb) return PPA_FAILURE;
	/** comment to support full processing */
	/* if the header is already allocated return*/
	if(p_item->session_meta) {
		dbg_print("<%s> already allocated\n", __func__);
		return PPA_SUCCESS;
	}
	mtu = p_item->mtu;

	ppa_memset(&swaHdr,0,sizeof(struct session_action));

	if( IsIpv6Session(p_item->flags) ) {
		isIPv6 = 1;
		proto_type = ETH_P_IPV6;
	}

	/*get the actual txif */
	swaHdr.dst_pmac_port_num=1;
	swaHdr.dst_pmac_port_list[0] = p_item->dest_ifid;

	if(swaHdr.dst_pmac_port_list[0] < MAX_PMAC_PORT)
		swaHdr.uc_vap_list[0] = p_item->dest_subifid ;

	/* Set fcs_bd when FCS required; DSL Bonding is one example. */
	if (txif_info && (txif_info->flags & NETIF_FCS))
		swaHdr.fcs_bd = 1;

	/*
	 * Find the length of template buffer
	 */
	tlen = ppa_out_header_len(p_item, &delta);

	if( ppa_is_EgressGreSession(p_item) ) { /* EoGRE/IPoGRE */
		if ( IsBridgedSession(p_item->flag2) )
			tx_netif = p_item->tx_if;
		else
			tx_netif = p_item->br_tx_if;

		ppa_is_gre_netif_type(tx_netif, &isIPv4Gre, &isEgGreTap);
	}

	if (IsVxlanSession(p_item->flag2)) { /* VxLAN */
		isvtep = 1;
		tx_netif = p_item->tx_if;
		ppa_is_vxlan_netif_type(tx_netif, &isIPv4VxLAN);
	}

	/*Bridged session handling currently supports only EoGRE and VxLAN */
	if ((IsBridgedSession(p_item->flag2) && !isvtep) ||
		isEgGreTap || (IsLanSession(p_item->flags) && isvtep)) {
		/* EoGRE Egress handling */
		if (ppa_is_EgressGreSession(p_item) || isvtep) {
			if (IsBridgedSession(p_item->flag2)) {
				if (isvtep)
					swaHdr.tunnel_type = TUNL_VXLAN;
				else
					swaHdr.tunnel_type = TUNL_EOGRE;
			} else {
				/* RoutedWAN->EoGRE session full processing */
				if ( !IsLanSession(p_item->flags) ) {
					if ( Is6rdSession(p_item->flags) ) {
						swaHdr.tunnel_type = TUNL_6RD;
					} else if ( IsDsliteSession(p_item->flags) ) {
						swaHdr.tunnel_type = TUNL_DSLITE;
					} else if ( IsL2TPSession(p_item->flags) && !IsIpsecTransSession(p_item->flag2) ) {
						swaHdr.tunnel_type = TUNL_L2TP;
						swaHdr.tunnel_rm_en = 1;
					} else if ( ppa_is_IngressGreSession(p_item) ) {
						swaHdr.tunnel_type = TUNL_IPOGRE;
					} else if ( IsIpsecTransSession(p_item->flag2) ) {
						swaHdr.tunnel_type = TRAN_ESP;
					} else if ( IsIpsecSession(p_item->flag2) ) {
						swaHdr.tunnel_type = TUNL_ESP;
					} else {
						/* Non-tunnel WAN like Static/DHCP/PPPoE */
						swaHdr.tunnel_type = TUNL_EOGRE;
						need_nat = true;
					}
				}
			}

			hdrlen=is_eogre=is_vxlan=0;

			temp_netif = tx_netif;
			if (isvtep && ppa_get_underlying_vlan_interface_if_vxlan
				(temp_netif, &base_netif, &is_vxlan) == PPA_SUCCESS) {
					ppa_get_vxlan_hdrlen(base_netif, &hdrlen);
					swaHdr.tunnel_type = TUNL_VXLAN;
					swaHdr.vni = p_item->pkt.vni;
					swaHdr.udp_vxlan_port = p_item->pkt.udp_vxlan_port;
			} else if (ppa_tmpl_get_underlying_vlan_interface_if_eogre
				(temp_netif, &base_netif, &is_eogre) == PPA_SUCCESS) {
					ppa_get_gre_hdrlen(base_netif, &hdrlen);
			}

			swaHdr.tunnel_ip_offset_en = 1;
			swaHdr.tunnel_ip_offset = tlen - hdrlen;

			temp_netif = tx_netif;
			if(ppa_if_is_vlan_if(temp_netif, temp_netif->name)) {
				tlen += VLAN_HLEN;
			}

			if (!isvtep) {
				tlen += ETH_HLEN;
				swaHdr.in_eth_iphdr_offset_en = 1;
				swaHdr.in_eth_iphdr_offset = tlen;
			} else {
				swaHdr.tunnel_udp_offset_en = 1;
				swaHdr.tunnel_udp_offset = ((swaHdr.tunnel_ip_offset +
						(isIPv4VxLAN ? IPV4_HDR_LEN : IPV6_HDR_LEN)));
				swaHdr.in_eth_iphdr_offset_en = 1;
				swaHdr.in_eth_iphdr_offset = swaHdr.tunnel_udp_offset +
						UDP_HDR_LEN + ETH_HLEN + VXLAN_HDR_LEN;
				tlen += ETH_HLEN;
			}

			if (isIPv4Gre || isIPv4VxLAN)
				proto_type = ETH_P_IP;
			else
				proto_type = ETH_P_IPV6;

			if ( ppa_is_EgressPppoeSession(p_item) ) { /* upstream tunnel header */
				swaHdr.pppoe_offset_en = 1;
				swaHdr.pppoe_offset = swaHdr.tunnel_ip_offset - PPPOE_HLEN;
			}
		} else if (ppa_is_IngressGreSession(p_item)) {
			swaHdr.tunnel_rm_en = 1;
			swaHdr.tunnel_type = TUNL_EOGRE;
		}
	} else if (IsTunneledSession(p_item->flags) | IsGreSession(p_item->flag2) |
			IsIpsecSession(p_item->flag2) | IsVxlanSession(p_item->flag2)) {
		/* All tunnel US encapsulation complementary processing except EoGre */
		if( IsLanSession(p_item->flags) ) {
			if( ppa_is_MapESession(p_item)) {
				swaHdr.tunnel_type = TUNL_DSLITE;
				swaHdr.tunnel_ip_offset_en = 1;
				swaHdr.tunnel_ip_offset = tlen - IPV6_HDR_LEN;
				swaHdr.new_src_ip_en = 1;
				swaHdr.new_src_ip.ip4.ip = p_item->pkt.nat_ip.ip;
				if (IsValidNatPort(p_item->flags))
					swaHdr.new_src_port = p_item->pkt.nat_port;
				proto_type = ETH_P_IPV6;
			} else if ( IsDsliteSession(p_item->flags) ) {

				swaHdr.tunnel_type = TUNL_DSLITE;
				swaHdr.tunnel_ip_offset_en = 1;
				swaHdr.tunnel_ip_offset	= tlen - IPV6_HDR_LEN;
				swaHdr.in_eth_iphdr_offset_en = 1;
				swaHdr.in_eth_iphdr_offset = tlen;
				proto_type = ETH_P_IPV6;

			} else if ( Is6rdSession(p_item->flags) ) {

				swaHdr.tunnel_type = TUNL_6RD;
				swaHdr.tunnel_ip_offset_en = 1;
				swaHdr.tunnel_ip_offset	= tlen - IPV4_HDR_LEN;
				swaHdr.in_eth_iphdr_offset_en = 1;
				swaHdr.in_eth_iphdr_offset = tlen;
				proto_type = ETH_P_IP;

			} else if (IsL2TPSession(p_item->flags) && !IsIpsecTransSession(p_item->flag2)) { /* Only L2TP not with IPSec Transport mode */

				swaHdr.tunnel_type = TUNL_L2TP;
				swaHdr.tunnel_ip_offset_en = 1;
				swaHdr.tunnel_ip_offset	= tlen - L2TP_HLEN;
				swaHdr.in_eth_iphdr_offset_en = 1;
				swaHdr.in_eth_iphdr_offset = tlen;
				swaHdr.tunnel_udp_offset_en = 1;
				swaHdr.tunnel_udp_offset = ((swaHdr.tunnel_ip_offset + IPV4_HDR_LEN));
				proto_type = ETH_P_IP;

				if (IsPppoeSession( p_item->flags))
					mtu += 48;
				else
					mtu += 40;

			} else if (IsIpsecTransSession(p_item->flag2)) { /* L2TP over IPSec transport mode */

				swaHdr.tunnel_type = TRAN_ESP;
				swaHdr.tunnel_ip_offset_en = 1;
				swaHdr.tunnel_ip_offset = tlen - L2TP_HLEN;
				swaHdr.in_eth_iphdr_offset_en = 1;
				swaHdr.in_eth_iphdr_offset = tlen;
				swaHdr.tunnel_udp_offset_en = 1;
				swaHdr.tunnel_udp_offset = ((swaHdr.tunnel_ip_offset + IPV4_HDR_LEN));
				proto_type = ETH_P_IP;

				if (IsPppoeSession( p_item->flags))
					mtu += 48;
				else
					mtu += 40;
				swaHdr.tunnel_id = p_item->tunnel_idx;

			} else if( IsIpsecSession(p_item->flag2)) {

				swaHdr.tunnel_type = TUNL_ESP;
				swaHdr.tunnel_id = p_item->tunnel_idx;
				swaHdr.tunnel_ip_offset_en = 0;
				swaHdr.tunnel_ip_offset = tlen - IPSEC_HLEN;
				swaHdr.esp_ip_offset_en = 1;
				swaHdr.esp_ip_offset = tlen - IPSEC_HLEN;

			} else if ( ppa_is_EgressGreSession(p_item) ) { /* IPoGRE */
				/*
				 * Required fields for gre tunnel -
				 * tunnel_type, in_eth_iphdr_offset,tunnel_ip_offset_en & corresponding
				 * flags
				 */
				hdrlen = 0;

				swaHdr.tunnel_type = TUNL_IPOGRE ;

				swaHdr.in_eth_iphdr_offset_en = 1;
				swaHdr.in_eth_iphdr_offset = tlen;

				ppa_get_gre_hdrlen(p_item->tx_if, &hdrlen);
				swaHdr.tunnel_ip_offset_en = 1;
				swaHdr.tunnel_ip_offset = tlen - hdrlen;

				if( isIPv4Gre )
					proto_type = ETH_P_IP;
				else
					proto_type = ETH_P_IPV6;
			} else {
				need_nat = true;
			}

			/* EoGRE Ingress handling */
			if ( ppa_is_IngressGreSession(p_item) ) { /* EoGRE */
				if ( !IsIpsecTransSession(p_item->flag2) && !IsIpsecSession(p_item->flag2) )
					swaHdr.tunnel_type = TUNL_EOGRE;
			}

			/* In case of tunnels on the WAN side, to get the pppoe offset, tunnel ip offset
			must be a non zero number and >= PPPOE_HLEN (i.e 8), where as for tunnels on the LAN side
			for example: having EoGRE on Ingress and PPPoE on Egress, pppoe offset is derived by
			subtracting PPPoE HLEN from tmpl buff length */

			if( IsPppoeSession(p_item->flags) ) {
				swaHdr.pppoe_offset_en = 1;
				if (swaHdr.tunnel_ip_offset >= PPPOE_HLEN)
					swaHdr.pppoe_offset = swaHdr.tunnel_ip_offset - PPPOE_HLEN;
				else
					swaHdr.pppoe_offset = tlen - PPPOE_HLEN;
			}
		} else {
			/* DS tunnels not handled in mpe*/
			swaHdr.tunnel_rm_en = 1;
			if (Is6rdSession(p_item->flags)) {
				swaHdr.tunnel_type = TUNL_6RD;
			} else if( ppa_is_MapESession(p_item)) {
				swaHdr.tunnel_type = TUNL_DSLITE;
				swaHdr.new_dst_ip_en = 1;
#if IS_ENABLED(CONFIG_PPA_NAT_LOOP_BACK)
				swaHdr.new_dst_ip.ip4.ip = p_item->pkt.nat_dst_ip.ip;
#endif
				if (IsValidNatPort(p_item->flags))
					swaHdr.new_dst_port = p_item->pkt.nat_port;
			} else if (IsDsliteSession(p_item->flags)) {
				swaHdr.tunnel_type = TUNL_DSLITE;
			} else if (IsL2TPSession(p_item->flags)) {
				swaHdr.tunnel_type = TUNL_L2TP;
			} else if (IsGreSession(p_item->flag2)) {
				swaHdr.tunnel_type = TUNL_IPOGRE ;
			} else if (IsVxlanSession(p_item->flag2)) {
				swaHdr.tunnel_type = TUNL_VXLAN;
				swaHdr.vni = p_item->pkt.vni;
				swaHdr.udp_vxlan_port = p_item->pkt.udp_vxlan_port;
				swaHdr.in_eth_iphdr_offset_en = 0;
				swaHdr.in_eth_iphdr_offset = 0;
				swaHdr.outer_dscp_mode = 1;
			}
		}
	} else {
		/* IPV4/IPV6 session full processing*/
		need_nat = true;
	}

	if (need_nat) {
		swaHdr.new_dst_ip_en = 1;
		swaHdr.new_src_ip_en = 1;

		if( IsLanSession(p_item->flags) ) {
			if( IsPppoeSession(p_item->flags) ) {
				swaHdr.pppoe_offset_en = 1;
				if (swaHdr.tunnel_ip_offset >= PPPOE_HLEN)
					swaHdr.pppoe_offset = swaHdr.tunnel_ip_offset - PPPOE_HLEN;
				else
					swaHdr.pppoe_offset = tlen - PPPOE_HLEN;
			}
			if( isIPv6 ) {
				/** Right now MPE requires the DW in this format */
				swaHdr.new_dst_ip.ip6.ip[3] = p_item->pkt.dst_ip.ip6[0];
				swaHdr.new_dst_ip.ip6.ip[2] = p_item->pkt.dst_ip.ip6[1];
				swaHdr.new_dst_ip.ip6.ip[1] = p_item->pkt.dst_ip.ip6[2];
				swaHdr.new_dst_ip.ip6.ip[0] = p_item->pkt.dst_ip.ip6[3];

				swaHdr.new_src_ip.ip6.ip[3] = p_item->pkt.src_ip.ip6[0];
				swaHdr.new_src_ip.ip6.ip[2] = p_item->pkt.src_ip.ip6[1];
				swaHdr.new_src_ip.ip6.ip[1] = p_item->pkt.src_ip.ip6[2];
				swaHdr.new_src_ip.ip6.ip[0] = p_item->pkt.src_ip.ip6[3];

			} else {
				if (IsValidNatIP(p_item->flags))
					swaHdr.new_src_ip.ip4.ip = p_item->pkt.nat_ip.ip;
				else
					swaHdr.new_src_ip.ip4.ip = p_item->pkt.src_ip.ip;
				swaHdr.new_dst_ip.ip4.ip = p_item->pkt.dst_ip.ip;
				
				if (IsValidNatPort(p_item->flags))
					swaHdr.new_src_port = p_item->pkt.nat_port;

			}
		} else {

			if( isIPv6 ) {
				/** Right now MPE requires the DW in this format */
				swaHdr.new_dst_ip.ip6.ip[3] = p_item->pkt.dst_ip.ip6[0];
				swaHdr.new_dst_ip.ip6.ip[2] = p_item->pkt.dst_ip.ip6[1];
				swaHdr.new_dst_ip.ip6.ip[1] = p_item->pkt.dst_ip.ip6[2];
				swaHdr.new_dst_ip.ip6.ip[0] = p_item->pkt.dst_ip.ip6[3];

				swaHdr.new_src_ip.ip6.ip[3] = p_item->pkt.src_ip.ip6[0];
				swaHdr.new_src_ip.ip6.ip[2] = p_item->pkt.src_ip.ip6[1];
				swaHdr.new_src_ip.ip6.ip[1] = p_item->pkt.src_ip.ip6[2];
				swaHdr.new_src_ip.ip6.ip[0] = p_item->pkt.src_ip.ip6[3];
			} else {
				swaHdr.new_src_ip.ip4.ip = p_item->pkt.src_ip.ip;
				if (IsValidNatIP(p_item->flags))
					swaHdr.new_dst_ip.ip4.ip = p_item->pkt.nat_ip.ip;
				else
					swaHdr.new_dst_ip.ip4.ip = p_item->pkt.dst_ip.ip;
				
				if (IsValidNatPort(p_item->flags))
					swaHdr.new_dst_port = p_item->pkt.nat_port;

#if IS_ENABLED(CONFIG_INTEL_IPQOS_MPE_DS_ACCEL)
				if (IsDsQosSession(p_item)) {
#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
					if (!(p_item->pkt.extmark & SESSION_PCE_CLASSIFICATION)) {
						/* Classication done in SW */
						swaHdr.ig_traffic_class_en = (p_item->pkt.extmark & SESSION_ING_TC_SET) >> ING_TC_SET_POS;

						swaHdr.new_traffic_class_en = (p_item->pkt.extmark & SESSION_EGR_TC_SET) >> EGR_TC_SET_POS;

						if (swaHdr.ig_traffic_class_en) {
							swaHdr.qmap = set_qmap_from_ing_grp(p_item->pkt);
							swaHdr.pkt_loop = 1;
						}

						if (!(swaHdr.new_traffic_class_en) && (p_item->pkt.extmark & SESSION_FLAG_TC_REMARK)) {
							swaHdr.new_traffic_class_en = 1;
						}
					}
#else /* CONFIG_NETWORK_EXTMARK */
					if (!(p_item->pkt.mark & SESSION_PCE_CLASSIFICATION)) {
						/* Classication done in SW */
						swaHdr.ig_traffic_class_en = (p_item->pkt.mark & SESSION_ING_TC_SET) >> ING_TC_SET_POS;

						swaHdr.new_traffic_class_en = (p_item->pkt.mark & SESSION_EGR_TC_SET) >> EGR_TC_SET_POS;

						if (swaHdr.ig_traffic_class_en) {
							swaHdr.qmap = set_qmap_from_ing_grp(p_item->pkt);
							swaHdr.pkt_loop = 1;
						}

						if (!(swaHdr.new_traffic_class_en) && (p_item->pkt.mark & SESSION_FLAG_TC_REMARK)) {
							swaHdr.new_traffic_class_en = 1;
						}
					}
#endif /* CONFIG_NETWORK_EXTMARK */
					else {
						/* Classification done in HW */
						swaHdr.pkt_loop = 0;
						swaHdr.ig_traffic_class_en = 0;
						swaHdr.new_traffic_class_en = 0;
					}

#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
					swaHdr.ig_traffic_class = ((p_item->pkt.extmark & SESSION_INGRESS_TC) >> INGRESS_TC_BIT_POS) - 1;
					swaHdr.traffic_class = ((p_item->pkt.extmark & TC_MASK) >> TC_START_BIT_POS) - 1;
#else /* CONFIG_NETWORK_EXTMARK */
					swaHdr.ig_traffic_class = ((p_item->pkt.mark & SESSION_INGRESS_TC) >> INGRESS_TC_BIT_POS) - 1;
					swaHdr.traffic_class = ((p_item->pkt.mark & TC_MASK) >> TC_START_BIT_POS) - 1;
#endif /* CONFIG_NETWORK_EXTMARK */

				}
#endif /* CONFIG_INTEL_IPQOS_MPE_DS_ACCEL */
			}
		}

#if IS_ENABLED(CONFIG_PPA_NAT_LOOP_BACK)
		if (IsValidNatLoopback(p_item->flag2)) {
			swaHdr.new_src_ip.ip4.ip = p_item->pkt.nat_src_ip.ip;
			swaHdr.new_dst_ip.ip4.ip = p_item->pkt.nat_dst_ip.ip;
		}
#endif

	}

	swaHdr.entry_vld = 1;
	swaHdr.routing_flag = 1;
	swaHdr.templ_len = tlen;
	swaHdr.mtu = mtu;
	swaHdr.pkt_len_delta = delta;

#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
	if(p_item->pkt.extmark & FLAG_TC_REMARK) {
#else /* CONFIG_NETWORK_EXTMARK */
	if(p_item->pkt.mark & FLAG_TC_REMARK) {
#endif /* CONFIG_NETWORK_EXTMARK */
		swaHdr.new_traffic_class_en = 1;
		swaHdr.traffic_class = (p_item->pkt.priority >= 15) ? 15 : p_item->pkt.priority;
	}

	if (p_item->flags & SESSION_VALID_NEW_DSCP) {
		swaHdr.new_inner_dscp = p_item->pkt.new_dscp;
		swaHdr.new_inner_dscp_en = 1;
	}

        if (swaHdr.tunnel_type != TUNL_NULL)  {
		if (p_item->flags & SESSION_VALID_NEW_DSCP)
			swaHdr.outer_dscp_mode = 2;
		else
			swaHdr.outer_dscp_mode = 1;
	}

	p_swaHdr = ( struct session_action *)ppa_alloc_dma(sizeof(struct session_action));
	if(p_swaHdr == NULL) {
		return PPA_ENOMEM;
	}
	ppa_memset(p_swaHdr, 0, sizeof(struct session_action));
	ppa_memcpy(p_swaHdr, &swaHdr,sizeof(struct session_action));

	if( swaHdr.templ_len ) {
		p_swaHdr->templ_buf = ppa_alloc_dma(swaHdr.templ_len);
		if (!p_swaHdr->templ_buf) {
			ppa_free(p_swaHdr);
			return PPA_ENOMEM;
		}
		ppa_memset(p_swaHdr->templ_buf, 0, swaHdr.templ_len);
	}
	/* else	TODO: Handle when no buffer need to be inserted.
		 Required mainly for downstream */
	hdr = p_swaHdr->templ_buf;

	/*construct the datalink header */
	if(!(p_item->flags & SESSION_TX_ITF_IPOA_PPPOA_MASK)) { /* put ethernet header */ 
		if (IsBridgedSession(p_item->flag2) || isEgGreTap || isvtep) {
			if (isEgGreTap || isvtep) {
				is_eogre = 0, is_vxlan = 0;

				ppa_memcpy(hdr, p_item->pkt.dst_mac, ETH_ALEN);
				temp_netif = tx_netif;
				if (ppa_tmpl_get_underlying_vlan_interface_if_eogre(temp_netif, &base_netif, &is_eogre) == PPA_SUCCESS ||
					ppa_get_underlying_vlan_interface_if_vxlan(temp_netif, &base_netif, &is_vxlan) == PPA_SUCCESS)
				{
					ppa_get_netif_hwaddr(base_netif, hdr + ETH_ALEN, 1);
				}
			} else
				ppa_memcpy(hdr, eth_hdr(metainfo->skb), ETH_ALEN*2);
		} else {
			/*get the MAC address of txif*/
			ret = ppa_get_netif_hwaddr(p_item->tx_if, hdr + ETH_ALEN, 1);
			if (ret == PPA_FAILURE){
				ppa_free(p_swaHdr->templ_buf);
				ppa_free(p_swaHdr);
				return ret;
			}
			ppa_memcpy(hdr, p_item->pkt.dst_mac, ETH_ALEN);
		}
		hdr += ETH_ALEN*2;

		if (IsValidOutVlanIns(p_item->flags)) {
			ppa_htonl((uint32_t*)hdr, p_item->pkt.out_vlan_tag);
			hdr +=VLAN_HLEN;
		}

		if (IsValidVlanIns(p_item->flags)) {
			ppa_htons((uint16_t*)hdr, ETH_P_8021Q);
			ppa_htons((uint16_t*)(hdr+2), p_item->pkt.new_vci);
			hdr +=VLAN_HLEN;
		}

		/* construct pppoe header */
		if( IsLanSession(p_item->flags) && IsPppoeSession(p_item->flags) ) {

			ppa_htons((uint16_t*)hdr, ETH_P_PPP_SES);
			hdr += 2; /* Two bytes for ETH protocol field */

			ppa_htons((uint16_t*)hdr, 0x1100);
			ppa_htons((uint16_t*)(hdr+2), p_item->pkt.pppoe_session_id); //sid

			/* payload length: Length of PPPoE payload in template buffer */
			if (swaHdr.tunnel_type == TRAN_ESP)
				ppa_htons((uint16_t*)(hdr+4), 0x16);
			else
				ppa_htons((uint16_t*)(hdr+4),
					(swaHdr.templ_len - (swaHdr.pppoe_offset + PPPOE_LEN)));

			if( ETH_P_IPV6 == proto_type ) { // ppp type ipv6
				ppa_htons((uint16_t*)(hdr+6), PPPOE_IPV6_TAG);
			} else {
				ppa_htons((uint16_t*)(hdr+6), PPPOE_IPV4_TAG);
			}
			hdr += PPPOE_HLEN;
		} else {
			ppa_htons((uint16_t*)hdr, proto_type);
			hdr += 2; /* Two bytes for ETH protocol field */
		}
	} else if (p_item->flags & SESSION_TX_ITF_IPOA_PPPOA_MASK) {
		/* Set dummy Ethernet header */
		ppa_memset(hdr, 0, ETH_ALEN*2);
		hdr += ETH_ALEN*2;

		if (IsValidOutVlanIns(p_item->flags)) {
			ppa_htonl((uint32_t*)hdr, p_item->pkt.out_vlan_tag);
			hdr +=VLAN_HLEN;
		}

		if (IsValidVlanIns(p_item->flags)) {
			ppa_htons((uint16_t*)hdr, ETH_P_8021Q);
			ppa_htons((uint16_t*)(hdr+2), p_item->pkt.new_vci);
			hdr +=VLAN_HLEN;
		}
		ppa_htons((uint16_t*)hdr, proto_type);
		hdr += 2; /* Two bytes for ETH protocol field */
	}

	if( IsBridgedSession(p_item->flag2) || isEgGreTap || isvtep) {
		if (ppa_is_EgressGreSession(p_item) || (isvtep && IsLanSession(p_item->flags))) {
			/* Add GRE Header to buffer for EoGre */
			is_eogre = 0, is_vxlan = 0;
			tlen=100;
			temp_netif = tx_netif;
			if (ppa_tmpl_get_underlying_vlan_interface_if_eogre(temp_netif, &base_netif, &is_eogre) == PPA_SUCCESS) {
				if (ppa_if_is_vlan_if(temp_netif, NULL))
					ppa_form_gre_hdr(base_netif, isIPv6, ETH_HLEN+4, hdr, &tlen);
				else
					ppa_form_gre_hdr(base_netif, isIPv6, ETH_HLEN, hdr, &tlen);

				ppa_update_grehdr_inherited(metainfo->skb, base_netif, isIPv4Gre, hdr, &swaHdr);

			} else if (ppa_get_underlying_vlan_interface_if_vxlan(temp_netif, &base_netif, &is_vxlan) == PPA_SUCCESS) {
				if (ppa_if_is_vlan_if(temp_netif, NULL))
					ppa_form_vxlan_hdr(metainfo->skb, base_netif, isIPv6, ETH_HLEN + 4, hdr, &tlen);
				else
					ppa_form_vxlan_hdr(metainfo->skb, base_netif, isIPv6, ETH_HLEN, hdr, &tlen);
			}

			hdr += tlen;
			/* Inner MAC hdr. In future CVLAN handle CVLANs here */
			if (IsBridgedSession(p_item->flag2)) {
				memcpy(hdr, (metainfo->skb->data - ETH_HLEN), ETH_HLEN);
			} else {
				ppa_get_dmac_from_dst_entry(hdr, metainfo->skb, ppa_dst(metainfo->skb));
				ppa_get_netif_hwaddr(p_item->tx_if, hdr + ETH_ALEN, 1);
				if ( IsIpv6Session(p_item->flags) ) {
					ppa_htons((uint16_t *)&hdr[12], ETH_P_IPV6);
				} else {
					ppa_htons((uint16_t *)&hdr[12], ETH_P_IP);
				}
			}
			temp_netif = tx_netif;
			if(ppa_if_is_vlan_if(temp_netif, temp_netif->name)) {
				ppa_htons(&vtag, *((uint16_t*)(hdr+12)));

				ppa_htons((uint16_t*)(hdr+12), ETH_P_8021Q);
				ppa_htons((uint16_t*)(hdr+14), ppa_get_vlan_id(p_item->tx_if));
				ppa_htons((uint16_t*)(hdr+16), vtag);
			}
		}
		goto hdr_done;
	}

	if( IsLanSession(p_item->flags) ) {
		/* Add Tunnel header here */
		if( ppa_is_MapESession(p_item)) {
			ppa_get_ipv6_tnl_iph((struct ipv6hdr*)hdr, metainfo->skb->dev, 0);
		} else if ( IsDsliteSession(p_item->flags) ) {
			ppa_form_dslite_tunnel(p_item->tx_if, hdr);
		} else if ( Is6rdSession(p_item->flags) ) {
			ppa_form_6rd_tunnel(p_item, hdr);
		} else if ( IsL2TPSession(p_item->flags)) {
			ppa_form_l2tp_tunnel(p_item, hdr,isIPv6);
		} else if ( ppa_is_EgressGreSession(p_item) ) { /* IPoGRE */
			/*Add GRE Header to templet buffer*/
			is_eogre = 0;
			tlen=100;
			temp_netif = p_item->tx_if;
			if(ppa_tmpl_get_underlying_vlan_interface_if_eogre(temp_netif, &base_netif, &is_eogre) == PPA_SUCCESS)
			{
				if(ppa_if_is_vlan_if(temp_netif, NULL))
					ppa_form_gre_hdr(base_netif, isIPv6, 4, hdr, &tlen);
				else
					ppa_form_gre_hdr(base_netif, isIPv6, 0, hdr, &tlen);

				ppa_update_grehdr_inherited(metainfo->skb, base_netif, isIPv4Gre, hdr, &swaHdr);
			}

		} else if(IsIpsecSession(p_item->flag2)) {
			ppa_form_ipsec_tunnel_hdr(metainfo->skb, hdr,isIPv6);
		}
	}

hdr_done:
	p_item->session_meta = (void *)p_swaHdr;
	print_action_info(p_swaHdr,isIPv6==0);
	dbg_print("Template buffer creation succeeded p_item->session_meta =%px\n",p_item->session_meta);
	return ret;
}

void ppa_remove_session_tmpl(struct uc_session_node *p_item)
{
	struct session_action *s_act;

	if (!p_item) {
		dbg_print("p_item is NULL\n");
		return;
	}

	s_act =	(struct session_action*)p_item->session_meta;

	if(s_act) {
		dbg_print("Template buffer freeing template buffer\n");
		ppa_free(s_act->templ_buf); /* free templ_buf */
		ppa_free(s_act); /* free complete session_action */
		p_item->session_meta = NULL;
	}
}

void ppa_remove_session_mc_tmplbuf(void *sess_act)
{
	struct session_action *s_act =
		(struct session_action*)sess_act;
	if(s_act) {
		ppa_free(s_act->templ_buf); /* free templ_buf */
		ppa_free(s_act); /* free complete session_action */
	}
}

struct session_action * ppa_form_mc_tmplbuf(PPA_MC_INFO *mc, uint32_t dest_list)
{
	struct session_action *sessionAction;
	uint32_t i, k = 0;
	uint8_t* Hdr;
	uint8_t mc_dst_mac[ETH_ALEN] = {0};

	struct mc_session_node *p_item = (struct mc_session_node*) mc->p_item;
	if (!p_item){
		dbg_print("pitem = NULL\n");
		return NULL;
	}

	if (p_item->session_action) {
		pr_err("<%s> ERROR: sessionAction should be NULL while forming tmplbuff\n", __func__);
		return p_item->session_action;
	}

	/* Derive multicast MAC from the multicast IP */
	if ( IsIpv6Session(p_item->flags) ) {
		ipv6_eth_mc_map((struct in6_addr *)p_item->grp.ip_mc_group.ip.ip6, mc_dst_mac);
	} else {
		ip_eth_mc_map(p_item->grp.ip_mc_group.ip.ip, mc_dst_mac);
	}

	/* filling of session Actioin */
	sessionAction = (struct session_action *) ppa_alloc_dma(sizeof(struct session_action));
	if (sessionAction == NULL) {
		dbg_print("sesionAction is NULL\n");
		return NULL;
	}
	ppa_memset(sessionAction, 0, sizeof(struct session_action));
	for (i = 0; i < MAX_PMAC_PORT; i++)
	{
		if (dest_list & (1 << i)) {
			sessionAction->dst_pmac_port_num++;
			sessionAction->dst_pmac_port_list[k++] = i;
		}

	}
	sessionAction->templ_len = ETH_HLEN;
	sessionAction->pkt_len_delta = 0 ;
	sessionAction->mtu = 1500;
	sessionAction->entry_vld = 1;
	sessionAction->mc_index = p_item->grp.group_id;
	sessionAction->mc_way = 1; /* Need to enable with new firmware for workaround for VAP level multicast. */

	if (p_item->flags & SESSION_VALID_PPPOE)
		sessionAction->pkt_len_delta -= PPPOE_HLEN;

	if (p_item->flags & SESSION_VALID_VLAN_RM)
		sessionAction->pkt_len_delta -= VLAN_HLEN;
	/* tunnel handling part needs to added*/

	sessionAction->templ_buf = ppa_alloc_dma(sessionAction->templ_len);
	if (sessionAction->templ_buf == NULL) {
		dbg_print("alloc caps_list item failed \n");
		ppa_free(sessionAction);
		return NULL;
	}

	ppa_memset(sessionAction->templ_buf, 0, sessionAction->templ_len);
	/* filling of templet buffer */
	Hdr = sessionAction->templ_buf;
	ppa_memcpy(Hdr, mc_dst_mac, ETH_ALEN);
	ppa_memcpy(Hdr + ETH_ALEN, p_item->src_mac, ETH_ALEN);
	Hdr += ETH_ALEN*2;

	if (p_item->flags & SESSION_IS_IPV6)
		ppa_htons((uint16_t*)Hdr, ETH_P_IPV6);
	else
		ppa_htons((uint16_t*)Hdr, ETH_P_IP);

	p_item->session_action = (void *)sessionAction;

	dbg_print("Ifid=%d group_id=%d dest_subifid=%d src_port=%d dst_port=%d	dst_ip=%d.%d.%d.%d src_ip=%d.%d.%d.%d\n", p_item->dest_ifid,
			p_item->grp.group_id, p_item->dest_subifid, p_item->grp.src_port, p_item->grp.dst_port, NIPQUAD(p_item->grp.ip_mc_group.ip), NIPQUAD(p_item->grp.source_ip.ip));

	return sessionAction;
}

extern struct session_action * (*ppa_construct_mc_template_buf_hook)(PPA_MC_INFO *p_item, uint32_t dest_list);
extern void (*ppa_session_mc_destroy_tmplbuf_hook)(void* sessionAction);

int32_t ppa_tmplbuf_register_hooks(void)
{
	/* Template buffer creation for unicast sessions moved to MPE HAL on need basis*/
	ppa_construct_mc_template_buf_hook = ppa_form_mc_tmplbuf;
	ppa_session_mc_destroy_tmplbuf_hook = ppa_remove_session_mc_tmplbuf;
	return PPA_SUCCESS;
}

void ppa_tmplbuf_unregister_hooks(void)
{
	ppa_construct_mc_template_buf_hook = NULL;
	ppa_session_mc_destroy_tmplbuf_hook = NULL;
}
