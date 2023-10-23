/******************************************************************************
 **
 ** FILE NAME	: ppa_api_hal_selector.c
 ** PROJECT	: PPA
 ** MODULES	: PPA HAL Selector APIs
 **
 ** DATE	: 28 Feb 2014
 ** AUTHOR	: Kamal Eradath
 ** DESCRIPTION : PPA HAL Selector layer APIs
 ** COPYRIGHT	: Copyright (c) 2020, MaxLinear, Inc.
 **               Copyright (c) 2017 Intel Corporation
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date		$Author				$Comment
 ** 28 Feb 2014  	Kamal Eradath		  	Initiate Version
 *******************************************************************************/
/*
 * ####################################
 *			  Head File
 * ####################################
 */
/*
 *  Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/atmdev.h>
#include <net/sock.h>

#include <net/ip_tunnels.h>
#include <linux/if_arp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <net/ipv6.h>
#include <net/ip6_tunnel.h>
/*
 *  Chip Specific Head File
 */
#include <net/ppa/ppa_api.h>
#if IS_ENABLED(CONFIG_LTQ_DATAPATH)
#include <net/datapath_api.h>
#endif
#include <net/ppa/ppa_hal_api.h>
#include <net/ppa/ppa_drv_wrapper.h>
#include <net/ppa/ppa_hal_wrapper.h>

#include "ppa_api_misc.h"
#include "ppa_api_session.h"
#include "ppa_api_core.h"
#include "ppa_api_sess_helper.h"
#include "ppa_api_qos.h"
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
#include "ppa_api_session_limit.h"
#endif
#include "ppa_api_netif.h"
#include "ppa_api_hal_selector.h"
#if defined(CONFIG_LTQ_CPU_FREQ) || defined(CONFIG_LTQ_PMCU) || defined(CONFIG_LTQ_PMCU_MODULE)
#include "ppa_api_pwm.h"
#endif
#if IS_ENABLED(CONFIG_SOC_GRX500)
#include "../platform/xrx500/ltq_pae_hal.h"
#endif
#include "ppa_stack_tnl_al.h"

/*
 * Select MPE full processing based on the condition below -
 * 1. US:GRE-LAN->{RoutedWAN,EoGRE}
 * 2. DS:GRE-LAN<-AnyTunnel
 */
#define IS_GRE_MIX_CAP_REQUIRED(p_item) \
	((ppa_is_LanSession(p_item) && ppa_is_IngressGreSession(p_item) && (!ppa_is_BrSession(p_item) || \
									ppa_is_EgressGreSession(p_item))) || \
	 (!ppa_is_LanSession(p_item) && ppa_is_EgressGreSession(p_item) && ppa_is_IngressTunneledSession(p_item)))

extern PPA_ATOMIC	g_hw_session_cnt;	/*declared in ppa_api_session.c */
extern PPA_HLIST_HEAD	g_mc_group_list_hash_table[SESSION_LIST_MC_HASH_TABLE_SIZE];

extern ppa_tunnel_entry	*g_tunnel_table[MAX_TUNNEL_ENTRIES];
extern uint32_t g_tunnel_counter[MAX_TUNNEL_ENTRIES];
extern PPE_LOCK g_tunnel_table_lock;
extern void ppa_nf_ct_refresh_acct(PPA_SESSION *ct, PPA_CTINFO ctinfo, 
		unsigned long extra_jiffies, unsigned long bytes, unsigned int pkts);

#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
static void ppa_del_tunnel_tbl_entry(PPA_TUNNEL_INFO* tInfo)
{
	uint32_t  tunnel_idx = tInfo->tunnel_idx;

	if (tunnel_idx >= MAX_TUNNEL_ENTRIES)
		return;

	ppe_lock_get(&g_tunnel_table_lock);
	if (g_tunnel_counter[tunnel_idx] && !--g_tunnel_counter[tunnel_idx]) {
		/*
		 * Mahipati:
		 * ppa_hsel_del_tunnel_entry should be called only if no sessions are
		 * active on the given tunnel.
		 * Note: This table is maintained only for PAE_HAL
		 */
		ppa_hsel_del_tunnel_entry(tInfo, 0, PAE_HAL);
		ppa_free(g_tunnel_table[tunnel_idx]);
		g_tunnel_table[tunnel_idx] = NULL;
	}
	ppe_lock_release(&g_tunnel_table_lock);
}
#endif /*!IS_ENABLED(CONFIG_X86_INTEL_LGM)*/

#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
static int compare_ip6_tunnel(PPA_IP6HDR* t1, struct ip6_tnl *t2)
{
	return ((ipv6_addr_cmp((struct in6_addr *)&(t1->saddr), &t2->parms.laddr) == 0) &&
			(ipv6_addr_cmp((struct in6_addr *)&(t1->daddr), &t2->parms.raddr) == 0));
}

static int compare_ip4_tunnel(PPA_IP4HDR* t1, struct ip_tunnel *t2)
{
	return (t1->saddr == t2->parms.iph.saddr &&
			t1->daddr == t2->parms.iph.daddr);
}

#if IS_ENABLED(CONFIG_IPV6)
static void init_ip6_tunnel(PPA_IP6HDR *ip6hdr, struct ip6_tnl* t6)
{
	ip6hdr->hop_limit = t6->parms.hop_limit;
	ipv6_addr_copy((struct in6_addr *) &ip6hdr->saddr, &t6->parms.laddr);
	ipv6_addr_copy((struct in6_addr *) &ip6hdr->daddr, &t6->parms.raddr);
}
#endif
static void init_ip4_tunnel(PPA_IP4HDR *ip4hdr, struct ip_tunnel* t)
{
	ip4hdr->saddr = t->parms.iph.saddr;
	ip4hdr->daddr = t->parms.iph.daddr;
}

static uint32_t ppa_add_tunnel_tbl_entry(PPA_TUNNEL_INFO * entry)
{
	uint32_t  index;
	uint32_t  ret = PPA_EAGAIN;
	uint32_t  empty_entry = MAX_TUNNEL_ENTRIES;
	struct ip_tunnel *tip = NULL;
	struct ip6_tnl *tip6 = NULL;
	ppa_tunnel_entry *t_entry = NULL;

	switch(entry->tunnel_type) {
		case TUNNEL_TYPE_DSLITE:
		case TUNNEL_TYPE_IP6OGRE:
		case TUNNEL_TYPE_6EOGRE:
			tip6 = (struct ip6_tnl *)netdev_priv(entry->tx_dev);
			break;
		case TUNNEL_TYPE_6RD:
		case TUNNEL_TYPE_IPOGRE:
		case TUNNEL_TYPE_EOGRE:
			tip = (struct ip_tunnel *)netdev_priv(entry->tx_dev);
			break;
		default:
			goto tunnel_err;
	}

	ppe_lock_get(&g_tunnel_table_lock);
	/*first lookup for a matching entry*/
	for (index = 0; index < MAX_TUNNEL_ENTRIES; index++) {

		t_entry = g_tunnel_table[index];
		if ( !t_entry ) {
			if (empty_entry == MAX_TUNNEL_ENTRIES)
				empty_entry = index;
		} else if( entry->tunnel_type == t_entry->tunnel_type ) {

			switch(entry->tunnel_type) {
				case TUNNEL_TYPE_DSLITE:
				case TUNNEL_TYPE_IP6OGRE:
				case TUNNEL_TYPE_6EOGRE:
					/*compere the src and dst address*/
					if( compare_ip6_tunnel(&t_entry->tunnel_info.ip6_hdr,tip6) ) {
						goto tunnel_found;
					}
					break;
				case TUNNEL_TYPE_6RD:
				case TUNNEL_TYPE_IPOGRE:
				case TUNNEL_TYPE_EOGRE:
					/*compere the src and dst address*/
					if( compare_ip4_tunnel(&t_entry->tunnel_info.ip4_hdr,tip) ) {
						goto tunnel_found;
					}
					break;
			}
		}
	}

	index = empty_entry;
	if (index >= MAX_TUNNEL_ENTRIES) {
		goto tunnel_err;
	} else {

		t_entry = (ppa_tunnel_entry *) ppa_malloc(sizeof(ppa_tunnel_entry));
		if (t_entry) {

			t_entry->tunnel_type = entry->tunnel_type;
			t_entry->hal_buffer = NULL;
			switch(entry->tunnel_type) {
				case TUNNEL_TYPE_DSLITE:
				case TUNNEL_TYPE_IP6OGRE:
				case TUNNEL_TYPE_6EOGRE:
#if IS_ENABLED(CONFIG_IPV6)
					init_ip6_tunnel(&t_entry->tunnel_info.ip6_hdr,tip6);
#endif
					break;
				case TUNNEL_TYPE_6RD:
				case TUNNEL_TYPE_IPOGRE:
				case TUNNEL_TYPE_EOGRE:
					init_ip4_tunnel(&t_entry->tunnel_info.ip4_hdr,tip);
					ASSERT(t_entry->tunnel_info.ip4_hdr.saddr != 0
							|| t_entry->tunnel_info.ip4_hdr.daddr != 0,
							"iphdr src/dst address is zero");
					break;
			}

			entry->tunnel_idx = index;
			g_tunnel_table[index] = t_entry;

#if IS_ENABLED(CONFIG_SOC_GRX500)
			if( entry->tunnel_type == TUNNEL_TYPE_DSLITE ||
					entry->tunnel_type == TUNNEL_TYPE_6RD ) {

				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
						"Adding tunnel type %d index%d\n",
						entry->tunnel_type, index);

				/* Required for DS-Lite and 6rd Tunnels */
				if (ppa_hsel_add_tunnel_entry(entry, 0, PAE_HAL) != PPA_SUCCESS) {

					ppa_debug(DBG_ENABLE_MASK_ERR,
							"Adding tunnel type %d failed\n",
							entry->tunnel_type);
					ppa_free(t_entry);
					g_tunnel_table[index] = NULL;
					goto tunnel_err;
				}
			}
#endif /*IS_ENABLED(CONFIG_SOC_GRX500)*/

		}
	}

tunnel_found:
	ret = PPA_SUCCESS;
	g_tunnel_counter[index]++;
	entry->tunnel_idx = index;

tunnel_err:
	ppe_lock_release(&g_tunnel_table_lock);

	return ret;
}
#endif /*!IS_ENABLED(CONFIG_X86_INTEL_LGM)*/

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
uint32_t ppa_add_ipsec_tunnel_tbl_entry(PPA_XFRM_STATE * entry,sa_direction dir, uint32_t *tunnel_index )
{
	uint32_t  index;
	uint32_t  ret = PPA_EAGAIN;
	uint32_t  empty_entry = MAX_TUNNEL_ENTRIES;
	ppa_tunnel_entry *t_entry = NULL;

	ppe_lock_get(&g_tunnel_table_lock);
	/*first lookup for a matching entry*/
	for (index = 0; index < MAX_TUNNEL_ENTRIES; index++) {

		t_entry = g_tunnel_table[index];
		if ( !t_entry ) {
			if (empty_entry == MAX_TUNNEL_ENTRIES)
				empty_entry = index;
		} else if (t_entry->tunnel_type == TUNNEL_TYPE_IPSEC) {
			/*compere the src and dst address*/
			if ( ( t_entry->tunnel_info.ipsec_hdr.inbound != NULL) &&
				(t_entry->tunnel_info.ipsec_hdr.inbound->aalg->alg_name != NULL) &&
				(t_entry->tunnel_info.ipsec_hdr.inbound->ealg->alg_name != NULL)  ) {
				if ( ( (t_entry->tunnel_info.ipsec_hdr.inbound->id.daddr.a4 == entry->props.saddr.a4)
					&& (t_entry->tunnel_info.ipsec_hdr.inbound->props.saddr.a4 == entry->id.daddr.a4)
					&& (ppa_str_cmp(t_entry->tunnel_info.ipsec_hdr.inbound->aalg->alg_name , entry->aalg->alg_name))
					&& (ppa_str_cmp(t_entry->tunnel_info.ipsec_hdr.inbound->ealg->alg_name , entry->ealg->alg_name))
					&& (t_entry->tunnel_info.ipsec_hdr.outbound == NULL))) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\n %s,Inbound Tunnlel found index =%d\n",__FUNCTION__, index);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\ninbound daddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.inbound->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\ninbound saddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.inbound->props.saddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry daddr=0x%x\n",entry->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry saddr=0x%x\n",entry->props.saddr.a4);
					goto tunnel_found;
				}
			} else if ( t_entry->tunnel_info.ipsec_hdr.inbound != NULL) {
				if ( ( (t_entry->tunnel_info.ipsec_hdr.inbound->id.daddr.a4 == entry->props.saddr.a4)
						&& (t_entry->tunnel_info.ipsec_hdr.inbound->props.saddr.a4 == entry->id.daddr.a4)
						&& (t_entry->tunnel_info.ipsec_hdr.outbound == NULL))) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\n %s,Inbound Tunnlel found index =%d\n",__FUNCTION__, index);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\ninbound daddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.inbound->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\ninbound saddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.inbound->props.saddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry daddr=0x%x\n",entry->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry saddr=0x%x\n",entry->props.saddr.a4);
					goto tunnel_found;
				}
			}

			if ( ( t_entry->tunnel_info.ipsec_hdr.outbound != NULL) &&
				( t_entry->tunnel_info.ipsec_hdr.outbound->aalg->alg_name != NULL) &&
				( t_entry->tunnel_info.ipsec_hdr.outbound->ealg->alg_name != NULL) ) {
				if ( (t_entry->tunnel_info.ipsec_hdr.outbound->id.daddr.a4 == entry->props.saddr.a4)
					&& (t_entry->tunnel_info.ipsec_hdr.outbound->props.saddr.a4 == entry->id.daddr.a4)
					&& (ppa_str_cmp(t_entry->tunnel_info.ipsec_hdr.outbound->aalg->alg_name , entry->aalg->alg_name))
					&& (ppa_str_cmp(t_entry->tunnel_info.ipsec_hdr.outbound->ealg->alg_name , entry->ealg->alg_name))
					&& (t_entry->tunnel_info.ipsec_hdr.inbound == NULL)) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\n %s,Outbound Tunnlel found index =%d\n",__FUNCTION__, index);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\noutbound daddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.outbound->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\noutbound saddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.outbound->props.saddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry daddr=0x%x\n",entry->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry saddr=0x%x\n",entry->props.saddr.a4);
					goto tunnel_found;
				}
			} else  if ( t_entry->tunnel_info.ipsec_hdr.outbound != NULL ) {
				if ( (t_entry->tunnel_info.ipsec_hdr.outbound->id.daddr.a4 == entry->props.saddr.a4)
					&& (t_entry->tunnel_info.ipsec_hdr.outbound->props.saddr.a4 == entry->id.daddr.a4)
					&& (t_entry->tunnel_info.ipsec_hdr.inbound == NULL)) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\n %s,Outbound Tunnlel found index =%d\n",__FUNCTION__, index);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\noutbound daddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.outbound->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\noutbound saddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.outbound->props.saddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry daddr=0x%x\n",entry->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry saddr=0x%x\n",entry->props.saddr.a4);
					goto tunnel_found;
				}
			}
		}
	}
	index = empty_entry;
	if (index >= MAX_TUNNEL_ENTRIES) {
		goto tunnel_err;
	} else {
		t_entry = (ppa_tunnel_entry *) ppa_malloc(sizeof(ppa_tunnel_entry));
		if (t_entry) {
			t_entry->tunnel_type = TUNNEL_TYPE_IPSEC;
			t_entry->hal_buffer = NULL;
			t_entry->tunnel_info.ipsec_hdr.inbound = NULL;
			t_entry->tunnel_info.ipsec_hdr.outbound = NULL;
			t_entry->tunnel_info.ipsec_hdr.routeindex = 0xFFFFFFFF;
			t_entry->tunnel_info.ipsec_hdr.inbound_pkt_cnt=0x0;
			t_entry->tunnel_info.ipsec_hdr.inbound_byte_cnt=0x0;
			t_entry->tunnel_info.ipsec_hdr.outbound_pkt_cnt=0x0;
			t_entry->tunnel_info.ipsec_hdr.outbound_byte_cnt=0x0;
			g_tunnel_table[index] = t_entry;
			g_tunnel_counter[index] = 0;

		}
	}
tunnel_found:
	ret = PPA_SUCCESS;
	if (t_entry != NULL ) {
		if(dir == INBOUND)
			t_entry->tunnel_info.ipsec_hdr.inbound = entry;
		else
			t_entry->tunnel_info.ipsec_hdr.outbound = entry;

		t_entry->tunnel_info.ipsec_hdr.dir = dir;
	}
	*tunnel_index = index;
	g_tunnel_counter[index]++;

tunnel_err:
	ppe_lock_release(&g_tunnel_table_lock);

	return ret;
}

uint32_t ppa_get_ipsec_tunnel_tbl_entry(PPA_XFRM_STATE * entry,sa_direction *dir, uint32_t *tunnel_index )
{
	uint32_t  index;
	uint32_t  ret = PPA_EAGAIN;
	uint32_t  empty_entry = MAX_TUNNEL_ENTRIES;
	ppa_tunnel_entry *t_entry = NULL;

	ppe_lock_get(&g_tunnel_table_lock);
	for (index = 0; index < MAX_TUNNEL_ENTRIES; index++) {

		t_entry = g_tunnel_table[index];
		if ( !t_entry ) {
			if (empty_entry == MAX_TUNNEL_ENTRIES)
				empty_entry = index;
		} else if (t_entry->tunnel_type == TUNNEL_TYPE_IPSEC) {
			/*compere the src and dst address*/
			if ( (t_entry->tunnel_info.ipsec_hdr.inbound != NULL) &&
				(t_entry->tunnel_info.ipsec_hdr.inbound->aalg->alg_name != NULL) &&
				(t_entry->tunnel_info.ipsec_hdr.inbound->ealg->alg_name != NULL) ) {

				if ( (t_entry->tunnel_info.ipsec_hdr.inbound->id.spi == entry->id.spi)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.inbound->id.daddr) ,
					&entry->id.daddr,entry->props.family)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.inbound->props.saddr),
					&entry->props.saddr,entry->props.family)
					&& (ppa_str_cmp(t_entry->tunnel_info.ipsec_hdr.inbound->aalg->alg_name ,
					entry->aalg->alg_name))
					&& (ppa_str_cmp(t_entry->tunnel_info.ipsec_hdr.inbound->ealg->alg_name ,
					entry->ealg->alg_name))) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\n%s,Inbound Tunnlel found index =%d\n",
							__FUNCTION__, index);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nINBOUND daddr=0x%x\n",
							t_entry->tunnel_info.ipsec_hdr.inbound->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nINBOUND saddr=0x%x\n",
							t_entry->tunnel_info.ipsec_hdr.inbound->props.saddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry daddr=0x%x\n",entry->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry saddr=0x%x\n",entry->props.saddr.a4);
					*dir = INBOUND;
					t_entry->tunnel_info.ipsec_hdr.dir = INBOUND;
					goto tunnel_found;
				}
			} else if ( t_entry->tunnel_info.ipsec_hdr.inbound != NULL) {
				if ( (t_entry->tunnel_info.ipsec_hdr.inbound->id.spi == entry->id.spi)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.inbound->id.daddr) ,
					&entry->id.daddr,entry->props.family)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.inbound->props.saddr),
					&entry->props.saddr,entry->props.family)) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\n%s,Inbound Tunnlel found index =%d\n",__FUNCTION__, index);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nINBOUND daddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.inbound->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nINBOUND saddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.inbound->props.saddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry daddr=0x%x\n",entry->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry saddr=0x%x\n",entry->props.saddr.a4);
					*dir = INBOUND;
					t_entry->tunnel_info.ipsec_hdr.dir = INBOUND;
					goto tunnel_found;
				}
			}


			if ( (t_entry->tunnel_info.ipsec_hdr.outbound != NULL) &&
				(t_entry->tunnel_info.ipsec_hdr.outbound->aalg->alg_name != NULL) &&
				(t_entry->tunnel_info.ipsec_hdr.outbound->ealg->alg_name != NULL) ) {

				if ( (t_entry->tunnel_info.ipsec_hdr.outbound->id.spi == entry->id.spi)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.outbound->id.daddr),&entry->id.daddr,entry->props.family)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.outbound->props.saddr) ,&entry->props.saddr, entry->props.family)
					&& (ppa_str_cmp(t_entry->tunnel_info.ipsec_hdr.outbound->aalg->alg_name , entry->aalg->alg_name))
					&& (ppa_str_cmp(t_entry->tunnel_info.ipsec_hdr.outbound->ealg->alg_name , entry->ealg->alg_name))) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\n%s,Outbound Tunnlel found index =%d\n",__FUNCTION__, index);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nOUTBOUND daddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.outbound->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nOUTBOUND saddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.outbound->props.saddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry daddr=0x%x\n",entry->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry saddr=0x%x\n",entry->props.saddr.a4);
					*dir = OUTBOUND;
					t_entry->tunnel_info.ipsec_hdr.dir = OUTBOUND;
					goto tunnel_found;
				}
			} else if ( t_entry->tunnel_info.ipsec_hdr.outbound != NULL) {
				if ( (t_entry->tunnel_info.ipsec_hdr.outbound->id.spi == entry->id.spi)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.outbound->id.daddr),&entry->id.daddr,entry->props.family)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.outbound->props.saddr) ,
					&entry->props.saddr, entry->props.family) ) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\n%s,Outbound Tunnlel found index =%d\n",__FUNCTION__, index);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nOUTBOUND daddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.outbound->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nOUTBOUND saddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.outbound->props.saddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry daddr=0x%x\n",entry->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nentry saddr=0x%x\n",entry->props.saddr.a4);
					*dir = OUTBOUND;
					t_entry->tunnel_info.ipsec_hdr.dir = OUTBOUND;
					goto tunnel_found;
				}
			}

		}
	}
	if (index >= MAX_TUNNEL_ENTRIES)
		goto tunnel_err;

tunnel_found:
	ret = PPA_SUCCESS;
	*tunnel_index = index;

tunnel_err:
	ppe_lock_release(&g_tunnel_table_lock);

	return ret;
}

extern int32_t ppa_session_delete_ipsec( uint32_t tunnel_index);
uint32_t ppa_add_ipsec_tunnel_tbl_update(sa_direction dir, uint32_t tunnel_index )
{
	uint32_t  ret = PPA_SUCCESS;
	ppa_tunnel_entry *t_entry = NULL;

	if (tunnel_index == MAX_TUNNEL_ENTRIES)
		return PPA_FAILURE;
	ppe_lock_get(&g_tunnel_table_lock);

	t_entry = g_tunnel_table[tunnel_index];
	if ( t_entry == NULL ) {
		ppe_lock_release(&g_tunnel_table_lock);
		return PPA_FAILURE;
	}
	else {
		if(dir == INBOUND)
			t_entry->tunnel_info.ipsec_hdr.inbound = NULL;
		else
			t_entry->tunnel_info.ipsec_hdr.outbound = NULL;
		g_tunnel_counter[tunnel_index]--;
	}

	if ((t_entry != NULL) && (t_entry->tunnel_info.ipsec_hdr.inbound == NULL) && (t_entry->tunnel_info.ipsec_hdr.outbound == NULL)) {
		ppa_free(g_tunnel_table[tunnel_index]);
		g_tunnel_table[tunnel_index] = NULL;
		ppe_lock_release(&g_tunnel_table_lock);
		ppa_session_delete_ipsec(tunnel_index);
	}
	else
		ppe_lock_release(&g_tunnel_table_lock);

	return ret;

}

int32_t __ppa_update_ipsec_tunnelindex(PPA_BUF *ppa_buf,uint32_t* tunnel_index)
{
	uint32_t  index;
	uint32_t  ret = PPA_EAGAIN;
	uint32_t  empty_entry = MAX_TUNNEL_ENTRIES;
	ppa_tunnel_entry *t_entry = NULL;

	uint32_t  spi_id =0x0;
	PPA_IPADDR  src_ip;
	uint16_t	src_port =0x0;
	PPA_IPADDR  dst_ip;
	uint16_t	dst_port =0x0;

	ppa_memset(&src_ip, 0, sizeof(src_ip));
	ppa_memset(&dst_ip, 0, sizeof(dst_ip));

	ppa_get_pkt_src_ip(&src_ip,ppa_buf);
	src_port	  = ppa_get_pkt_src_port(ppa_buf);
	ppa_get_pkt_dst_ip(&dst_ip,ppa_buf);
	dst_port	  = ppa_get_pkt_dst_port(ppa_buf);

	spi_id = ((src_port << 16) & 0xFFFF0000);
	spi_id |= ((dst_port) & 0xFFFF);

	ppe_lock_get(&g_tunnel_table_lock);
	for (index = 0; index < MAX_TUNNEL_ENTRIES; index++) {

		t_entry = g_tunnel_table[index];
		if (!t_entry) {
			if (empty_entry == MAX_TUNNEL_ENTRIES)
				empty_entry = index;
		} else if (t_entry->tunnel_type == TUNNEL_TYPE_IPSEC) {

			if (t_entry->tunnel_info.ipsec_hdr.inbound != NULL) {
				if ((t_entry->tunnel_info.ipsec_hdr.inbound->id.spi == spi_id)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.inbound->id.daddr) ,
					(PPA_XFRM_ADDR *) &(dst_ip),t_entry->tunnel_info.ipsec_hdr.inbound->props.family)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.inbound->props.saddr),
					(PPA_XFRM_ADDR *) &(src_ip),t_entry->tunnel_info.ipsec_hdr.inbound->props.family)) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\n%s,Inbound Tunnlel found index =%d\n",__FUNCTION__, index);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nINBOUND daddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.inbound->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nINBOUND saddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.inbound->props.saddr.a4);
					goto tunnel_found;
				}
			}
			if (t_entry->tunnel_info.ipsec_hdr.outbound != NULL) {
				if ( (t_entry->tunnel_info.ipsec_hdr.outbound->id.spi == spi_id)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.outbound->id.daddr),
					(PPA_XFRM_ADDR *)&(dst_ip),t_entry->tunnel_info.ipsec_hdr.outbound->props.family)
					&& ppa_ipsec_addr_equal(&(t_entry->tunnel_info.ipsec_hdr.outbound->props.saddr) ,
					(PPA_XFRM_ADDR *) &(src_ip),t_entry->tunnel_info.ipsec_hdr.outbound->props.family)) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\n%s,Outbound Tunnlel found index =%d\n",__FUNCTION__, index);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nOUTBOUND daddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.outbound->id.daddr.a4);
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"\nOUTBOUND saddr=0x%x\n",t_entry->tunnel_info.ipsec_hdr.outbound->props.saddr.a4);
					goto tunnel_found;
				}
			}

		}
	}
	if (index >= MAX_TUNNEL_ENTRIES)
		goto tunnel_err;

tunnel_found:
	ret = PPA_SUCCESS;
	*tunnel_index = index;
tunnel_err:
	ppe_lock_release(&g_tunnel_table_lock);

	return ret;
}
#endif

#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
#if defined(L2TP_CONFIG) && L2TP_CONFIG
static uint32_t ppa_add_tunnel_tbll2tp_entry(PPA_L2TP_INFO * entry)
{
	uint32_t index, ret = PPA_SUCCESS;
	uint32_t empty_entry = MAX_TUNNEL_ENTRIES;
	ppa_tunnel_entry *t_entry = NULL;

	ppe_lock_get(&g_tunnel_table_lock);
	for (index = 0; index < MAX_TUNNEL_ENTRIES; index++) {
		t_entry = g_tunnel_table[index];
		if (!t_entry) {
			if (empty_entry >= MAX_TUNNEL_ENTRIES)
				empty_entry = index;
		} else {
			if( t_entry->tunnel_type == entry->tunnel_type &&
					entry->tunnel_id == t_entry->tunnel_info.l2tp_hdr.tunnel_id) {
				goto ADD_PPPOL2TP_ENTRY_GOON;
			}
		}
	}

	index = empty_entry;
	if (index >= MAX_TUNNEL_ENTRIES) {
		ret = PPA_EAGAIN;
		goto ADD_TUNNEL_ENTRY_FAILURE;
	} else {
		t_entry = (ppa_tunnel_entry *) ppa_malloc(sizeof(ppa_tunnel_entry));
		if (t_entry) {
			t_entry->tunnel_type = TUNNEL_TYPE_L2TP;
			ppa_memcpy(&t_entry->tunnel_info.l2tp_hdr,entry,sizeof(PPA_L2TP_INFO));
			t_entry->hal_buffer = NULL;
			g_tunnel_table[index] = t_entry;
			g_tunnel_counter[index] = 0;
		}
	}

ADD_PPPOL2TP_ENTRY_GOON:
	g_tunnel_counter[index]++;
	entry->tunnel_idx = index;
ADD_TUNNEL_ENTRY_FAILURE:
	ppe_lock_release(&g_tunnel_table_lock);

	return ret;
}
#endif
#endif /*!IS_ENABLED(CONFIG_X86_INTEL_LGM)*/

static uint32_t ppa_form_capability_list(struct uc_session_node * p_item)
{
	uint32_t total_caps=0;

#if IS_ENABLED(CONFIG_PPA_NAT_LOOP_BACK)
	/* session is NAT LOOPBACK */ 
	if (p_item->flag2 & SESSION_FLAG2_VALID_NAT_LOOPBACK) {
		p_item->caps_list[total_caps++].cap = SESS_NAT_LOOPBACK;
		return total_caps;
	}
#endif

	if (!(p_item->flags & SESSION_IS_IPV6)) {
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
		if(!(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_SA))
#endif
#if IS_ENABLED(CONFIG_INTEL_IPQOS_MPE_DS_ACCEL)
		if(!(p_item->flag2 & SESSION_FLAG2_DS_MPE_QOS))
#endif
		/* Since map-e sessions need to be configured in mpe,
	  	 and pae has more weigtage for IPv4 session.
			 we should not mark map-e sessions as IPv4 sessions */
		if (!ppa_is_MapESession(p_item) && !ppa_is_vxlan_session(p_item))
			p_item->caps_list[total_caps++].cap = SESS_IPV4;
	} else if (!ppa_is_vxlan_session(p_item)) {
		p_item->caps_list[total_caps++].cap = SESS_IPV6;
	}

	if (IS_GRE_MIX_CAP_REQUIRED(p_item)) {
		/* Session is GRE tunnel mixed with others */
		return total_caps;
	}

	if ( ppa_is_6rdSession(p_item)) {
		p_item->caps_list[total_caps++].cap = TUNNEL_6RD;
	} else if ( ppa_is_DsLiteSession(p_item)) {
		if ( ppa_is_MapESession(p_item))
			p_item->caps_list[total_caps++].cap = TUNNEL_MAP_E;
		else
			p_item->caps_list[total_caps++].cap = TUNNEL_DSLITE;
#if defined(L2TP_CONFIG) && L2TP_CONFIG
	} else if (p_item->flags & SESSION_VALID_PPPOL2TP) {
		p_item->caps_list[total_caps++].cap =
			ppa_is_LanSession(p_item)?TUNNEL_L2TP_US:TUNNEL_L2TP_DS;
#endif
	} else if (ppa_is_GreSession(p_item)) {
		p_item->caps_list[total_caps++].cap =
			ppa_is_EgressGreSession(p_item)?TUNNEL_GRE_US:TUNNEL_GRE_DS;
	} else if (ppa_is_vxlan_session(p_item)) {
		p_item->caps_list[total_caps++].cap = TUNNEL_VXLAN;
	}

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	if ( (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND) || (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_SA) ) {
		p_item->caps_list[total_caps++].cap = TUNNEL_IPSEC_US;
	}

#if IS_ENABLED(CONFIG_INTEL_IPQOS_MPE_DS_ACCEL)
	if (p_item->flag2 & SESSION_FLAG2_DS_MPE_QOS) {
		p_item->caps_list[total_caps++].cap = MPE_DS_QOS;
	}
#endif

	if(total_caps < MAX_RT_SESS_CAPS) {
		if (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_INBOUND) {
			p_item->caps_list[total_caps++].cap = TUNNEL_IPSEC_DS;
		}
	}
#endif

	if (total_caps < MAX_RT_SESS_CAPS) {
		if (p_item->flag2 & SESSION_FLAG2_CPU_OUT) {
			p_item->caps_list[total_caps++].cap = SESS_LOCAL_OUT;
		} else if (p_item->flag2 & SESSION_FLAG2_CPU_IN) {
			p_item->caps_list[total_caps++].cap = SESS_LOCAL_IN;
		}
	}

	return total_caps;
}
/*****************************************************************************************
// ppa_api_session.c will call this to check if a session has hals registered for all
// its capabilities.
*****************************************************************************************/
int32_t ppa_is_hal_registered_for_all_caps(struct uc_session_node * p_item)
{
	uint32_t num_caps = 0, i, j;
	PPA_HAL_ID hal_id;
	int ret = PPA_SUCCESS;

	ppa_memset(p_item->caps_list, 0,
			 sizeof(PPA_HSEL_CAP_NODE) * MAX_RT_SESS_CAPS);

	/*Find the capabilities needed by the session and form a list of them*/
	num_caps = ppa_form_capability_list(p_item);

	if (ppa_select_hals_from_caplist(0, num_caps, p_item->caps_list) != PPA_SUCCESS)
		return PPA_FAILURE;

	/* for each capability find out the hal*/
	for (i = 0; i < num_caps; i+=j) {
		j = ppa_group_hals_in_capslist(i, num_caps, p_item->caps_list);

		/*check if a valid hal is registered for that capability*/
		hal_id = p_item->caps_list[i].hal_id;
		if (hal_id < PPE_HAL || hal_id >= MAX_HAL)
	  	return PPA_FAILURE;
  	}
	return ret;
}

/*****************************************************************************************
// This is to select a specified hal for all capabilities, required for a session.
*****************************************************************************************/
static int32_t ppa_select_specified_hal_for_all_caps(uint8_t start, uint8_t num_entries,
		PPA_HSEL_CAP_NODE *caps_list, PPA_HAL_ID hal_id)
{
	int ret = PPA_SUCCESS;
	uint32_t cap_idx;
	bool hal_id_found;

	/* for each capability find out the specificed hal */
	for (cap_idx = start; cap_idx < (start + num_entries); cap_idx++) {
		caps_list[cap_idx].hal_id = 0;
		caps_list[cap_idx].wt = 0;
		hal_id_found = false;

		while (ppa_select_hals_from_caplist(cap_idx, 1, caps_list) == PPA_SUCCESS) {
			if (caps_list[cap_idx].hal_id == hal_id) {
				hal_id_found = true;
				break;
			}
		}

		if (!hal_id_found) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				"<%s : %u> hal_id=%d not found for capability idx %u\n"
				,__func__, __LINE__, hal_id, cap_idx);
			ret = PPA_FAILURE;
			break;
		}
	}

	return ret;
}

/*****************************************************************************************
// ppa_api_session.c will call this when a session is getting added
*****************************************************************************************/
int32_t ppa_hsel_add_routing_session(struct uc_session_node * p_item, PPA_BUF *skb,
		uint32_t flags)
{
	/* while adding a new session do the following steps
	// check all the flags and find the capabilities needed and form an list of capabilities needed by this session
	// group the capabilities based on first regisrted HAL*/

	uint32_t num_caps = 0, i, j, k, l;
	uint8_t f_more_hals = 0;
	PPA_ROUTING_INFO route;
	FILTER_INFO *filter_node = NULL;
	FILTER_INFO entry;
#if defined(L2TP_CONFIG) && L2TP_CONFIG
	PPA_L2TP_INFO l2tpinfo = { 0 };
#endif

#if IS_ENABLED(CONFIG_SOC_GRX500)
	uint32_t pret = -1;
	struct uc_session_node *old_item;
#ifdef CONFIG_PPA_VAP_QOS_SUPPORT
	QOS_LOGICAL_IF_LIST_ITEM *p_logical_if_item;
#endif
#endif
	struct netif_info *tx_ifinfo = NULL;
	uint32_t hw_litepath_dest_ifid = 0;
	uint8_t hw_litepath_available = false;
	struct netif_info* hw_lp_ifinfo = NULL;

	int ret = PPA_SUCCESS;

	ppa_memset(&route, 0, sizeof(route));
#if defined(L2TP_CONFIG) && L2TP_CONFIG
	ppa_memset(&l2tpinfo, 0, sizeof(l2tpinfo));
#endif

	if (p_item->flags & SESSION_ADDED_IN_HW) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "Session %px already in HW\n",
				p_item->session);
		return PPA_SUCCESS;
	}

	/* Locally terminated/generated sessions that cannot be accelerated are filtered out here */
	if ((p_item->flag2 & SESSION_FLAG2_CPU_BOUND)) {
		if (ppa_is_sess_accelable(p_item) != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				"This locally terminated session cannot be accelerated!\n");
			return PPA_FAILURE;
		}
	}
	ppa_memset(&entry, 0, sizeof(FILTER_INFO));
	if (skb) {
		entry.ip_proto = ppa_get_pkt_ip_proto(skb);
		entry.src_port = ppa_get_pkt_src_port(skb);
		entry.dst_port = ppa_get_pkt_dst_port(skb);
	}else {
		entry.ip_proto = p_item->pkt.ip_proto;
		entry.src_port = p_item->pkt.src_port;
		entry.dst_port = p_item->pkt.dst_port;
	}

	/* //////////////////////////////////////////////////////////////
	//  find all the capabilities needed by this session
	////////////////////////////////////////////////////////////// */

	ppa_memset(p_item->caps_list, 0,
			sizeof(PPA_HSEL_CAP_NODE) * MAX_RT_SESS_CAPS);

	num_caps = ppa_form_capability_list(p_item);

	if (num_caps >= MAX_RT_SESS_CAPS)
		return PPA_FAILURE;

	/* //////////////////////////////////////////////////////////////
	// for each capability find out the HAL
	/////////////////////////////////////////////////////////////// */

	if (flags & PPA_HSEL_F_SESSION_SW_ONLY ) { /* HW Acc disabled using setppefp -f 0 */
		ret = ppa_select_specified_hal_for_all_caps(0, num_caps, p_item->caps_list, SWAC_HAL);
		if (ret != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				"<%s : %u> Only SWAC_HAL cannot accelerate session %px!\n",
				__func__, __LINE__, p_item);
			return PPA_FAILURE;
		}
	} else if (ppa_select_hals_from_caplist(0, num_caps, p_item->caps_list) !=
			PPA_SUCCESS) {
		return PPA_FAILURE;
	}

	/*extract the tx_ifinfo for sess_meta*/
	if (p_item->br_tx_if)
		ret = ppa_get_netif_info(p_item->br_tx_if->name, &tx_ifinfo);
	else if (p_item->tx_if)
		ret = ppa_get_netif_info(p_item->tx_if->name, &tx_ifinfo);

	if (ret != PPA_SUCCESS) {
		ret = PPA_SUCCESS;
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %d failed to get the tx_ifinfo!!!\n", __FUNCTION__, __LINE__);
	}

	if (!(flags & PPA_HSEL_F_SESSION_SW_ONLY) && (p_item->flag2 & SESSION_FLAG2_CPU_BOUND) &&
		!ppa_is_TunneledSession(p_item) && ppa_netif_lookup("lite0", &hw_lp_ifinfo) == PPA_SUCCESS) {

		if (p_item->flag2 & SESSION_FLAG2_CPU_OUT) {
			hw_litepath_available = true;
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "<%s> hwlitepath available for CPU OUT!!\n", __func__);
		} else if ((p_item->flag2 & SESSION_FLAG2_CPU_IN) && (p_item->flags &
				(SESSION_VALID_PPPOE | SESSION_VALID_VLAN_RM))) {
			hw_litepath_dest_ifid = hw_lp_ifinfo->phys_port;
			hw_litepath_available = true;
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "<%s> hwlitepath available for CPU IN!!\n", __func__);
		}

		ppa_netif_put(hw_lp_ifinfo);
	}

	route.p_item = p_item;
	
	/*printk(KERN_INFO"%s %d num_caps=%d\n",__FUNCTION__,__LINE__,num_caps);*/

	for (i = 0; i < num_caps;) {
		f_more_hals = 0;
		j = ppa_group_hals_in_capslist(i, num_caps, p_item->caps_list);

		filter_node = get_matched_entry(p_item->caps_list[i].hal_id, &entry);
		if (filter_node) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "Session can not be learnt with hal:%d\n",p_item->caps_list[i].hal_id);
			if (ppa_select_hals_from_caplist(i, j, p_item->caps_list) != PPA_SUCCESS) {
				return PPA_FAILURE;
			} else {
				continue;
			}
		}
		/* Based on the capability of first entry in the list we can decide the action*/
		switch (p_item->caps_list[i].cap) {
			case SESS_IPV4:
			case SESS_IPV6:
			case SESS_LOCAL_IN:
			case SESS_LOCAL_OUT:
#if IS_ENABLED(CONFIG_PPA_NAT_LOOP_BACK)
		        case SESS_NAT_LOOPBACK:
#endif
#if IS_ENABLED(CONFIG_INTEL_IPQOS_MPE_DS_ACCEL)
			case MPE_DS_QOS:
#endif
			case TUNNEL_MAP_E:
			case TUNNEL_VXLAN:
#if IS_ENABLED(CONFIG_SOC_GRX500)
				/* Check if session is GRE tunnel mixed with others */
				if (IS_GRE_MIX_CAP_REQUIRED(p_item)) {
					/* Only MPE full processing supported */
					if (p_item->caps_list[i].hal_id != MPE_HAL) {
						ret = PPA_FAILURE;
						break;
					}
				}

				/*Get the packet info and metadata info */
				if(!(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_LAN)) {
					route.pppoe_mode = ((p_item->flags & SESSION_VALID_PPPOE) ? 1 : 0);
				}
#else
				route.pppoe_mode = ((p_item->flags & SESSION_VALID_PPPOE) ? 1 : 0);
#endif /*IS_ENABLED(CONFIG_SOC_GRX500)*/

				/*Extract the tunnel information */
				route.tnnl_info.tunnel_type = TUNNEL_TYPE_MAX;
				if ( ppa_is_6rdSession(p_item) ) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
							"add 6RD entry to acceleration, tx dev: %s\n",
							ppa_get_netif_name(p_item->tx_if));
					route.tnnl_info.tunnel_type = TUNNEL_TYPE_6RD;
				} else if ( ppa_is_DsLiteSession(p_item)) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
							"add Dslite entry to acceleration, tx dev: %s\n",
							ppa_get_netif_name(p_item->tx_if));
					route.tnnl_info.tunnel_type = TUNNEL_TYPE_DSLITE;
				} else if ( ppa_is_GreSession(p_item) &&
					    !(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND) ) {

					struct netif_info* pnetinfo;
					char* ifName;

					if( ppa_is_gre_netif(p_item->tx_if))
						ifName = ppa_get_netif_name(p_item->tx_if);
					else
						ifName = ppa_get_netif_name(p_item->rx_if);

					if( PPA_SUCCESS != ppa_netif_lookup(ifName, &pnetinfo)) {
						goto MTU_ERROR;
					}

					switch( pnetinfo->greInfo.flowId) {
						case FLOWID_IPV4GRE:
							route.tnnl_info.tunnel_type = TUNNEL_TYPE_IPOGRE; break;
						case FLOWID_IPV4_EoGRE:
							route.tnnl_info.tunnel_type = TUNNEL_TYPE_EOGRE; break;
						case FLOWID_IPV6GRE:
							route.tnnl_info.tunnel_type = TUNNEL_TYPE_IP6OGRE; break;
						case FLOWID_IPV6_EoGRE:
							route.tnnl_info.tunnel_type = TUNNEL_TYPE_6EOGRE; break;
						default:
							break;
					}
					ppa_netif_put(pnetinfo);
				} else if (ppa_is_vxlan_session(p_item)) {
					route.tnnl_info.tunnel_type = TUNNEL_TYPE_VXLAN;
				}
#if defined(L2TP_CONFIG) && L2TP_CONFIG
				else if (p_item->flags & SESSION_VALID_PPPOL2TP) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
							"add L2TP entry to acceleration, tx dev: %s\n",
							ppa_get_netif_name(p_item->tx_if));
					route.tnnl_info.tunnel_type = TUNNEL_TYPE_L2TP;
				}
#endif

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
				else if (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND) {
					route.tnnl_info.tunnel_type = TUNNEL_TYPE_IPSEC;
					route.tnnl_info.tunnel_idx = p_item->tunnel_idx;
				}
				else if (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_INBOUND) {
					route.tnnl_info.tunnel_type = TUNNEL_TYPE_IPSEC;
					route.tnnl_info.tunnel_idx = p_item->tunnel_idx;
				}
				/* If the tunnel type is not IPSec we need to allocate the tunnel table entry */
				if(route.tnnl_info.tunnel_type != TUNNEL_TYPE_IPSEC && !(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_TRANS))
					p_item->tunnel_idx = ~0;
#endif /* CONFIG_PPA_MPE_IP97 */

#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
				if ( route.tnnl_info.tunnel_type != TUNNEL_TYPE_MAX ) {
					/*upstream*/
					switch( route.tnnl_info.tunnel_type ) {
#if !IS_ENABLED(CONFIG_PRX300_CQM)
					/* ppa_add_tunnel_tbl_entry has hard code to add entry in pae,
							skip it PRX as it doesnt have PAE */
						case TUNNEL_TYPE_DSLITE:
						case TUNNEL_TYPE_6RD: {
							if (!ppa_is_LanSession(p_item))
								break;

							route.tnnl_info.tx_dev = p_item->tx_if;

							if (!ppa_is_MapESession(p_item) && ppa_add_tunnel_tbl_entry(&route.tnnl_info) != PPA_SUCCESS) {
								goto MTU_ERROR;
							}
							p_item->tunnel_idx = route.tnnl_info.tunnel_idx;
							break;
						}
#endif
						case TUNNEL_TYPE_EOGRE:
						case TUNNEL_TYPE_6EOGRE:
						case TUNNEL_TYPE_IPOGRE:
						case TUNNEL_TYPE_IP6OGRE: {
							if ( !ppa_is_EgressGreSession(p_item) )
								break;

							/*
							 * Note: For PAE complimentary processing PPPoE mode should
							 * be reset for these tunnel
							 */
							route.pppoe_mode = 0;
							route.tnnl_info.tx_dev = p_item->tx_if;
							if (ppa_add_tunnel_tbl_entry(&route.tnnl_info) != PPA_SUCCESS) {
								goto MTU_ERROR;
							}
							p_item->tunnel_idx = route.tnnl_info.tunnel_idx;
							break;
						}
						case TUNNEL_TYPE_L2TP: {
							if (!ppa_is_LanSession(p_item))
								break;

							/*
							 * Note: For PAE complimentary processing PPPoE mode should
							 * be reset for these tunnel
							 */
							route.pppoe_mode = 0;
							route.tnnl_info.tunnel_type = TUNNEL_TYPE_L2TP;
							route.tnnl_info.tx_dev = p_item->tx_if;	
							l2tpinfo.tunnel_id = p_item->pkt.pppol2tp_tunnel_id;
							l2tpinfo.session_id = p_item->pkt.pppol2tp_session_id;
							l2tpinfo.tunnel_type = TUNNEL_TYPE_L2TP;

							ppa_pppol2tp_get_l2tpinfo(p_item->tx_if, &l2tpinfo);
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
							if (!(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_TRANS)) { /* skip tunnel entry add for L2TP over IPSec Transport mode*/
#endif
								if (ppa_add_tunnel_tbll2tp_entry(&l2tpinfo) == PPA_SUCCESS)
									p_item->tunnel_idx = l2tpinfo.tunnel_idx;
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
							}
#endif
							break;
						}
						case TUNNEL_TYPE_VXLAN: {
							if (!ppa_is_LanSession(p_item))
								break;

							route.tnnl_info.tx_dev = p_item->tx_if;
							break;
						}
					}
				} /*Up stream tunnel handling*/
#endif /*!IS_ENABLED(CONFIG_X86_INTEL_LGM)*/

				/* source mac */
				if (!(p_item->flags & SESSION_TX_ITF_IPOA_PPPOA_MASK)) {
					if ( !(p_item->flags & SESSION_VALID_NEW_SRC_MAC) ) {
						p_item->flags |= SESSION_VALID_NEW_SRC_MAC;
#if IS_ENABLED(CONFIG_SOC_GRX500)
						if (p_item->br_rx_if) {
							ppa_bridge_entry_add(p_item->s_mac, NULL, p_item->br_rx_if,
								PPA_F_STATIC_ENTRY);
						} else {
							if (p_item->rx_if
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
							    && !((p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_INBOUND) ||
								 (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND))
#endif
							   ) {
								ppa_bridge_entry_add(p_item->s_mac, NULL, p_item->rx_if,
									PPA_F_STATIC_ENTRY);
							}
						}
#endif /*IS_ENABLED(CONFIG_SOC_GRX500)*/
					}
				}

#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
				if(p_item->pkt.extmark & SESSION_FLAG_TC_REMARK)
					route.f_tc_remark_enable = 1;
				else
					route.f_tc_remark_enable = 0;
				/* skb->priority 0 - 7 refers to Low->High priority Queue & TC 0-7 also refers to Low->High priority Queue*/
				route.tc_remark = ((p_item->pkt.extmark & TC_MASK) >> TC_START_BIT_POS) - 1;
#else /* CONFIG_NETWORK_EXTMARK */
				if(p_item->pkt.mark & SESSION_FLAG_TC_REMARK)
					route.f_tc_remark_enable = 1;
				else
					route.f_tc_remark_enable = 0;
				/* skb->priority 0 - 7 refers to Low->High priority Queue & TC 0-7 also refers to Low->High priority Queue*/
				route.tc_remark = ((p_item->pkt.mark & TC_MASK) >> TC_START_BIT_POS) - 1;
#endif

#if defined(CONFIG_PPA_VAP_QOS_SUPPORT) && IS_ENABLED(CONFIG_SOC_GRX500)
				if(!ppa_is_TunneledSession(p_item)) {
					if (ppa_qos_logical_if_lookup(p_item->dest_subifid, NULL, &p_logical_if_item) == PPA_SUCCESS) {
						if (p_logical_if_item->flowId > 3) {
							route.nFlowId = ((p_logical_if_item->flowId & 0x3) << 6);
#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
							if (p_item->pkt.extmark & SESSION_FLAG_TC_REMARK)
								route.tc_remark |= 8;
#else /* CONFIG_NETWORK_EXTMARK */
							if (p_item->pkt.mark & SESSION_FLAG_TC_REMARK)
								route.tc_remark |= 8;
#endif /* CONFIG_NETWORK_EXTMARK */
						} else
							route.nFlowId = (p_logical_if_item->flowId << 6);
					}
				}
#endif
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
				if(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_LAN)
					route.tnnl_info.tunnel_type = TUNNEL_TYPE_IPSEC;
#endif
				/*check whether more HALs needs to be configured to complete this session configuration*/
				if (i + j != num_caps) {  /*more HALs are there in the caps_list*/
					f_more_hals = 1;
					/*TBD: setup flow between the Acceleration blocks*/
				}

				if ((p_item->caps_list[i].hal_id != SWAC_HAL) && (p_item->caps_list[i].hal_id != PPV4_HAL)) {
					/* TCP litepath supported only in swaccel & LGM*/
					if( (p_item->flag2 & SESSION_FLAG2_CPU_BOUND) ) {
						/*printk(KERN_INFO"%s %d tcp litepath session rejected by HAL_ID=%d\n",
							__FUNCTION__, __LINE__, p_item->caps_list[i].hal_id);*/
						ret = PPA_FAILURE;
						break;	
					}
				}
				if (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_INBOUND) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "Valid IPSec inbound SA, adding metadata needs to be skipped!\n");
						goto SKIP_ADD_SESS_META;
				}

#if IS_ENABLED(CONFIG_SOC_GRX500)
				if ((p_item->caps_list[i].hal_id == SWAC_HAL) && hw_litepath_available
					&& (p_item->flag2 & SESSION_FLAG2_CPU_OUT)) {
					/* Try PAE Engine explicity for litepath hw acceleration,
					but calling route before tmpl buff creation will not work
					for MPE or complemntary processing! */
					pret = ppa_hsel_add_routing_entry(&route, 0, PAE_HAL);
					if (pret == PPA_SUCCESS) {
						p_item->sess_hash = route.entry;
						p_item->routing_entry = route.entry;
						ppa_atomic_inc(&g_hw_session_cnt);
					} else {
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
							"%s %d PAE route failed for TSO (HW Litepath)!\n",
							__FUNCTION__, __LINE__);
						/* If PAE route fails for TSO, data will take
						 * default TCP SW Litepath route
						 */
					}
				}
#endif /*IS_ENABLED(CONFIG_SOC_GRX500)*/
				/*Create the session metadata (eg:template buffer) */
				ret = ppa_hsel_add_sess_meta(&route, skb, tx_ifinfo, p_item->caps_list[i].hal_id);
				if (ret == PPA_SUCCESS) {
SKIP_ADD_SESS_META:
					if ((ret = ppa_hsel_add_routing_entry(&route, 0,
								p_item->caps_list[i].hal_id)) == PPA_SUCCESS) {
#if IS_ENABLED(CONFIG_SOC_GRX500)
						if ((p_item->flags & SESSION_ADDED_IN_SW) && (p_item->caps_list[i].hal_id == SWAC_HAL) &&
							(p_item->flag2 & SESSION_FLAG2_CPU_IN) && (p_item->flag2 & SESSION_FLAG2_LRO) &&
							hw_litepath_available) {

							p_item->dest_ifid = hw_litepath_dest_ifid;
							p_item->dest_subifid = 0;

							/* Try PAE Engine explicity for litepath hw acceleration */
							pret = ppa_hsel_add_routing_entry(&route, 0, PAE_HAL);
							if (pret == PPA_SUCCESS) {
								p_item->sess_hash = route.entry;
								p_item->routing_entry = route.entry;
								ppa_atomic_inc(&g_hw_session_cnt);
							} else {
								ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
									"%s %d PAE route failed for LRO (HW Litepath)!\n",
									__FUNCTION__, __LINE__);
								/* If PAE route fails for LRO, data will take
								 * default TCP SW Litepath route.
								 */
#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
								if (ppa_del_lro_entry(p_item) != PPA_SUCCESS)
									ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
										"lro entry delete failed\n");
								else
									p_item->flag2 &= ~SESSION_FLAG2_LRO;
#endif
							}
						}
						if (p_item->caps_list[i].hal_id == PAE_HAL) {
							/*in this case we have to check the flags to see whether the PAE has done any swap*/
							if (route.update_flags & FLAG_SESSION_SWAPPED) {
							/*
							 * Search in the session list to find out the session with
							 * routing_entry == route.entry (same index)
							 * clear the flag SESSION_ADDED_IN_HW
							 * make session->routing_entry = ~0;
							 */
								ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "Session swapped!!!\n");
								old_item = __ppa_session_find_by_routing_entry(route.entry);
								if (old_item) {
									old_item->routing_entry = ~0;
									/*
								 	* Mahipati : //TODO
								 	* For session management reseting of this flag will create issues.
								 	* This need to be taken care.
								 	*/
									old_item->flags &= ~SESSION_ADDED_IN_HW;

									ppa_atomic_dec(&g_hw_session_cnt);
									__ppa_session_put(old_item);
								}
							}
						}

						p_item->sess_hash = route.entry;
#endif /*IS_ENABLED(CONFIG_SOC_GRX500)*/
						p_item->routing_entry = route.entry;

						ppa_atomic_inc(&g_hw_session_cnt);
						/* Session add succeeded*/
						break;
					} else {
						/*fail in add_routing_entry*/
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %d fail in add_routing_entry in HAL_ID=%d\n",
							__FUNCTION__, __LINE__, p_item->caps_list[i].hal_id);
						ppa_hsel_del_sess_meta(&route, p_item->caps_list[i].hal_id);
						p_item->session_meta = NULL;
#if IS_ENABLED(CONFIG_SOC_GRX500)
						if (route.update_flags & FLAG_SESSION_LOCK_FAIL) {
							p_item->flag2 |= SESSION_FLAG2_HW_LOCK_FAIL;
						}
#endif

					}
				} else {
					p_item->session_meta = NULL;
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %d add session meta failed\n",__FUNCTION__, __LINE__);
				}

				if (ret != PPA_SUCCESS) {
					/* below condition will be true incase of HW Litepath
					 * sessions where sw-acc related route addition or
					 * sess-meta fails, hence PAE route needs to be deleted.
					 */
					if ((p_item->flags & SESSION_ADDED_IN_HW) &&
					    (p_item->flag2 & SESSION_FLAG2_CPU_OUT)) {
#if IS_ENABLED(CONFIG_SOC_GRX500)
						if (p_item->sess_hash)
							p_item->sess_hash = ~0;
#endif
						ppa_hsel_del_routing_entry(&route, 0, PAE_HAL);
						p_item->routing_entry = ~0;
						p_item->flags &= ~SESSION_ADDED_IN_HW;
						ppa_atomic_dec(&g_hw_session_cnt);
					}
				}
MTU_ERROR:
				ret = PPA_FAILURE;
				break;
			case TUNNEL_L2TP_US:
			case TUNNEL_GRE_US:

				/* Create the session metadata (eg: Template buffer) */
				ret = ppa_hsel_add_sess_meta(&route, skb, tx_ifinfo, p_item->caps_list[i].hal_id);
				if (ret != PPA_SUCCESS) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %d add complement session meta failed\n", __FUNCTION__, __LINE__);
					ret = PPA_FAILURE;
					break;
				}

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
			case TUNNEL_IPSEC_DS:
#endif
				{
					/* Session_meta addition not neded for Security Association, but route needs to be added */
					if ((ret = ppa_hsel_add_complement(&route, 0, p_item->caps_list[i].hal_id)) != PPA_SUCCESS) {
						ppa_hsel_del_sess_meta(&route, p_item->caps_list[i].hal_id);
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %d add complement session failed\n", __FUNCTION__, __LINE__);
						ret = PPA_FAILURE;
					}
				}
				break;
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
			case TUNNEL_IPSEC_US:

				p_item->flags |= SESSION_LAN_ENTRY;
				route.tnnl_info.tunnel_type = TUNNEL_TYPE_IPSEC;
				route.tnnl_info.tunnel_idx = p_item->tunnel_idx;
				route.entry = p_item->routing_entry;

				/* Create the session metadata (eg:template buffer) */

				if (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_LAN) {
					ppa_hsel_add_sess_meta(&route, skb, tx_ifinfo, p_item->caps_list[i].hal_id);
				}
				/* Session metadata not needed for Security Association, but for Data packet its needed */

				if ((ret = ppa_hsel_add_complement(&route, 0, p_item->caps_list[i].hal_id)) != PPA_SUCCESS) {
					ppa_hsel_del_sess_meta(&route, p_item->caps_list[i].hal_id);
					ret = PPA_FAILURE;
				} else
					p_item->flags |= SESSION_ADDED_IN_HW;

				break;
#endif

			case TUNNEL_ENCRYPT:
			case TUNNEL_DECRYPT:
				break;
			default:
				break;
		}
		/*if it returns failure*/
		if (ret != PPA_SUCCESS) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %d Deleting partial configurations\n", __FUNCTION__, __LINE__);
			/*revert all the settings done from caps_list[0] to caps_list[i-1]*/
			for (k = 0; k < i; k++) {
				l = ppa_group_hals_in_capslist(k, i, p_item->caps_list);
				if (p_item->caps_list[k].cap == SESS_IPV4
						|| p_item->caps_list[k].cap == SESS_IPV6) {
					ppa_hsel_del_routing_entry(&route, 0,
							p_item->caps_list[k].hal_id);
					p_item->routing_entry = ~0;
					p_item->flags &= ~SESSION_ADDED_IN_HW;
					ppa_atomic_dec(&g_hw_session_cnt);
				} else if (p_item->caps_list[k].cap == TUNNEL_L2TP_US) {
					/*TBD: call the fn to delete the tunnel configuration*/
				}
				k += l;
			}
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %d Trying to find next eligible HAL candidate..\n",__FUNCTION__, __LINE__);
			/*find the next HAL registred for this capability*/
			if (ppa_select_hals_from_caplist(i, j, p_item->caps_list) != PPA_SUCCESS) {
				/* Workaround : try for L2TP full processing in SAE */
				if ((p_item->caps_list[i].cap == TUNNEL_L2TP_US) && (i > 0)) {
					if (ppa_select_specified_hal_for_all_caps(0, num_caps, p_item->caps_list, SWAC_HAL) == PPA_SUCCESS) {
						i = 0;
						continue;
					}
				}

				/*this session cannot be added to the HW*/
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "%s %d No more HALS registered for this capability\n", __FUNCTION__, __LINE__);
				if (tx_ifinfo)
					ppa_netif_put(tx_ifinfo);

#if IS_ENABLED(CONFIG_SOC_GRX500)
				/* source mac */
				if ( (p_item->flags & SESSION_VALID_NEW_SRC_MAC) && !is_zero_mac(p_item->s_mac) ) {
					ppa_bridge_entry_delete(p_item->s_mac, NULL, PPA_F_STATIC_ENTRY);
				}
#endif
				return PPA_FAILURE;
			} else {
				i = 0;
				continue;
			}
		}
		/*returned success in step 1 proceed*/
		i += j;
	}

	if (tx_ifinfo)
		ppa_netif_put(tx_ifinfo);

	p_item->num_caps = num_caps;
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
	update_session_mgmt_stats(p_item, ADD);
#endif

#if IS_ENABLED(CONFIG_SOC_GRX500)
	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"\n%s %s %d add entry success\n sip=%u\n dip=%u\n sp=%u\n dp=%u\n protocol_tcp=%d\n src_port=%0x\n dest_port=%0x\n\
			dest_subifid=%0x\n p_item->sess_hash=%u\n session=%px\n mtu=%d num_caps=%d \ntimeout=%d \n", __FILE__,
			__FUNCTION__, __LINE__, p_item->pkt.src_ip.ip, p_item->pkt.dst_ip.ip, p_item->pkt.src_port, 
			p_item->pkt.dst_port, p_item->flags & SESSION_IS_TCP, p_item->src_ifid, p_item->dest_ifid, p_item->dest_subifid, 
			p_item->sess_hash, p_item->session, p_item->mtu, p_item->num_caps, p_item->timeout);
#endif

	return PPA_SUCCESS;
}

/*****************************************************************************************/
// this function will be called from ppa_api_session.c when a session is getting deleted
/*****************************************************************************************/
void ppa_hsel_del_routing_session(struct uc_session_node *p_item)
{
	PPA_ROUTING_INFO route;
	uint32_t i, j;
	uint64_t tmp = 0, pkts = 0;
	PPA_CTINFO ctinfo;   

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	uint32_t num_caps = 0;

	if(!(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_SA)) {
#endif
		if ( !((p_item->flags & (SESSION_ADDED_IN_HW | SESSION_ADDED_IN_SW)) && p_item->num_caps) &&
		     !(p_item->flag2 & SESSION_FLAG2_CONTAINER)) {
			return;
		}
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	}
#endif

	ppa_memset(&route,0,sizeof(route));

	route.p_item = p_item;

#if IS_ENABLED(CONFIG_SOC_GRX500)
	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"\ndel entry \nsip=%u \ndip=%u \nsp=%u \ndp=%u \nprotocol_tcp=%d \ndest_port=%0x \ndest_subifid=%0x \nsess_hash=%u \nsession=%px num_caps=%d\n",
			p_item->pkt.src_ip.ip, p_item->pkt.dst_ip.ip, p_item->pkt.src_port,
			p_item->pkt.dst_port, p_item->flags & SESSION_IS_TCP,
			p_item->dest_ifid, p_item->dest_subifid, p_item->sess_hash,
			p_item->session, p_item->num_caps);
#endif

#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
	update_session_mgmt_stats(p_item, DELETE);
#endif

	/*when init, these entry values are ~0, the max the number which can be detected by these functions*/
	route.bytes = 0;
	route.f_hit = 0;
	/* read the hit status from the HAL which handles the routing for this session*/

	if (p_item->caps_list[0].hal_id == PAE_HAL) {
#if IS_ENABLED(CONFIG_SOC_GRX500)
		route.entry = p_item->sess_hash;
#else
		route.entry = p_item->routing_entry;
#endif
	} else {
		route.entry = p_item->routing_entry;
	}

#if IS_ENABLED(CONFIG_SOC_GRX500)
	/* TCP Litepath - HW and SW */
	if ((p_item->flags & SESSION_ADDED_IN_HW) && (p_item->flags & SESSION_ADDED_IN_SW)) {

		ppa_hsel_get_routing_entry_bytes(&route, 1, PAE_HAL);
		ppa_hsel_del_routing_entry(&route, 0, PAE_HAL);
		p_item->flags &= ~SESSION_ADDED_IN_HW;
		ppa_atomic_dec(&g_hw_session_cnt);
	} else {
		ppa_hsel_get_routing_entry_bytes(&route, 1, p_item->caps_list[0].hal_id);
	}
#else /*IS_ENABLED(CONFIG_SOC_GRX500)*/
	ppa_hsel_get_routing_entry_bytes(&route, 1, p_item->caps_list[0].hal_id);
#endif
	if (route.f_hit) {

		/* read the statistics from hal*/
		tmp = route.bytes;
		pkts = route.packets;

		/*refresh the ct timeout with g_hit_polling_time*/
		ctinfo = (p_item->flags & SESSION_IS_REPLY) ? IP_CT_IS_REPLY : 0;
		ppa_nf_ct_refresh_acct( p_item->session, ctinfo, 2*DEFAULT_HIT_POLLING_TIME*HZ, tmp, 0);

		/* reset the accelerated counters, as it is going to be deleted */
		p_item->acc_bytes = 0;
		p_item->last_bytes = 0;

#if defined(PPA_IF_MIB) && PPA_IF_MIB
		/*update mib interfaces*/
		update_netif_hw_mib(ppa_get_netif_name(p_item->rx_if), tmp, 1);
		if (p_item->br_rx_if)
			update_netif_hw_mib(ppa_get_netif_name(p_item->br_rx_if), tmp, 1);
		update_netif_hw_mib(ppa_get_netif_name(p_item->tx_if), tmp, 0);
		if (p_item->br_tx_if)
			update_netif_hw_mib(ppa_get_netif_name(p_item->br_tx_if), tmp, 0);
#endif
	}

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	if((p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_INBOUND) || (p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND))
		route.tnnl_info.tunnel_type = TUNNEL_TYPE_IPSEC;

	if((p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_SA))
	{
		ppa_memset(p_item->caps_list, 0,
				sizeof(PPA_HSEL_CAP_NODE) * MAX_RT_SESS_CAPS);

		num_caps = ppa_form_capability_list(p_item);

		/* ///////////////////////////////////////////////////////////////
		// for each capability find out the HAL
		/////////////////////////////////////////////////////////////// */
		if (ppa_select_hals_from_caplist(0, num_caps, p_item->caps_list) !=
				PPA_SUCCESS) {
			return;
		}
		p_item->num_caps = num_caps;
	}
#endif

	for (i = 0; i < p_item->num_caps;) {

		j = ppa_group_hals_in_capslist(i, p_item->num_caps, p_item->caps_list);

		switch(p_item->caps_list[i].cap)
		{
			case SESS_IPV4:
			case SESS_IPV6:
#if IS_ENABLED(CONFIG_PPA_NAT_LOOP_BACK)
			case SESS_NAT_LOOPBACK:
#endif
#if IS_ENABLED(CONFIG_INTEL_IPQOS_MPE_DS_ACCEL)
			case MPE_DS_QOS:
#endif
			case TUNNEL_MAP_E:
			case TUNNEL_VXLAN:
				{
					/* Check if session is GRE tunnel mixed with others */
					if (IS_GRE_MIX_CAP_REQUIRED(p_item)) {
						/* Only MPE full processing supported */
						if (p_item->caps_list[i].hal_id != MPE_HAL) {
							break;
						}
					}
					if(ppa_hsel_del_routing_entry(&route, 0, p_item->caps_list[i].hal_id) != PPA_SUCCESS){
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_hsel_del_routing_entry returned failure !!\n");
					}
					if(ppa_hsel_del_sess_meta(&route, p_item->caps_list[i].hal_id) != PPA_SUCCESS) {
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_hsel_del_sess_meta returned failure !!\n");
					}
#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
					route.tnnl_info.tunnel_idx = p_item->tunnel_idx;
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
					if(route.tnnl_info.tunnel_type != TUNNEL_TYPE_IPSEC && !(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_TRANS))
						p_item->tunnel_idx =  ~0;
#endif
					if ( ppa_is_6rdSession(p_item) ) {
						route.tnnl_info.tunnel_type = TUNNEL_TYPE_6RD;
					} else if (ppa_is_DsLiteSession(p_item)) {
						route.tnnl_info.tunnel_type = TUNNEL_TYPE_DSLITE;
					}


#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
					if ((route.tnnl_info.tunnel_idx != ~0) && (route.tnnl_info.tunnel_type != TUNNEL_TYPE_IPSEC)
						&& (route.tnnl_info.tunnel_type != TUNNEL_TYPE_VXLAN) &&
							!(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_TRANS))
#else
						if(route.tnnl_info.tunnel_idx != ~0)
#endif
						{
							ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
									"\n%s,%d, ppa_del_tunnel_tbl_entry called, pitem: %px, flag: %x, flag2: %x\n",
									__func__,__LINE__, p_item, p_item->flags, p_item->flag2);
							ppa_del_tunnel_tbl_entry(&route.tnnl_info);

						}

#if IS_ENABLED(CONFIG_SOC_GRX500)
					/* source mac */
					if ( (p_item->flags & SESSION_VALID_NEW_SRC_MAC) && !is_zero_mac(p_item->s_mac) ) {
						ppa_bridge_entry_delete(p_item->s_mac, NULL, PPA_F_STATIC_ENTRY);
					}
#endif
#endif /*!IS_ENABLED(CONFIG_X86_INTEL_LGM)*/

					p_item->flags &= ~SESSION_ADDED_IN_HW;

					ppa_atomic_dec(&g_hw_session_cnt);
					break;
				}
			case TUNNEL_L2TP_US:
			case TUNNEL_GRE_US:
				{
					/*TBD: call the fn to delete the tunnel configuration*/
					if (ppa_hsel_del_complement(&route, 0, p_item->caps_list[i].hal_id) != PPA_SUCCESS) {
						pr_err("Failed to delete Complementary entry!!!\n");
					}
					if (ppa_hsel_del_sess_meta(&route, p_item->caps_list[i].hal_id) != PPA_SUCCESS ) {
						pr_err("Failed to remove sess metadata\n");
					}

					break;
				}

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
			case TUNNEL_IPSEC_DS:
				{
					p_item->flags &= ~SESSION_LAN_ENTRY;
					route.tnnl_info.tunnel_type = TUNNEL_TYPE_IPSEC;
					route.tnnl_info.tunnel_idx = p_item->tunnel_idx;
					if ((ppa_hsel_del_complement(&route, 0, p_item->caps_list[i].hal_id)) != PPA_SUCCESS) {
						pr_err("Failed to delete Complementary entry!!!\n");
					}
					break;
				}

			case TUNNEL_IPSEC_US:
				{
					p_item->flags |= SESSION_LAN_ENTRY;
					route.tnnl_info.tunnel_type = TUNNEL_TYPE_IPSEC;
					route.tnnl_info.tunnel_idx = p_item->tunnel_idx;
					route.entry = p_item->routing_entry;
					if ((ppa_hsel_del_complement(&route, 0, p_item->caps_list[i].hal_id)) != PPA_SUCCESS) {
						pr_err("Failed to delete Complementary entry!!!\n");
					}
					if (!(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_SA)) {
						/* For IPSec data outbound traffic, not for Security Association */
						if (ppa_hsel_del_sess_meta(&route, p_item->caps_list[i].hal_id) != PPA_SUCCESS ) {
							pr_err("Failed to remove sess metadata\n");
						}
					}
					break;
				}
#endif
			default:
				break;
		}
		i += j;
	}
	p_item->num_caps = 0;
}
/*****************************************************************************************
// this function will be called from ppa_modify_session fn
// only the following feilds can be modified on an on going session
// DSCP
// mtu
// out VLAN handling
// inner vlan handling
// pppoe header
// This needs to be supported by PAE and MPE which has the capability SESS_IPV4 or SESS_IPV6
*****************************************************************************************/
int32_t ppa_hsel_update_routing_session(struct uc_session_node *p_item,
		uint32_t flags)
{
	PPA_ROUTING_INFO route = { 0 };

	route.p_item = p_item;

	if ((flags & PPA_F_SESSION_NEW_DSCP))
		route.update_flags |=
			PPA_UPDATE_ROUTING_ENTRY_NEW_DSCP_EN |
			PPA_UPDATE_ROUTING_ENTRY_NEW_DSCP;

	if ((flags & PPA_F_SESSION_VLAN))
		route.update_flags |=
			PPA_UPDATE_ROUTING_ENTRY_VLAN_INS_EN |
			PPA_UPDATE_ROUTING_ENTRY_NEW_VCI |
			PPA_UPDATE_ROUTING_ENTRY_VLAN_RM_EN;

	if ((flags & PPA_F_PPPOE)) {
		if (p_item->pkt.pppoe_session_id == 0) {
			/*need to disable pppoe flag, ie, change to PPPOE transparent*/
			route.pppoe_mode = 0;
			route.update_flags |= PPA_UPDATE_ROUTING_ENTRY_PPPOE_MODE;
			route.update_flags |= PPA_UPDATE_ROUTING_ENTRY_PPPOE_IX;
		} else { 
			route.pppoe_mode = 1;
			route.update_flags |= PPA_UPDATE_ROUTING_ENTRY_PPPOE_MODE;
			route.update_flags |= PPA_UPDATE_ROUTING_ENTRY_PPPOE_IX;
		}
	}

	if ((flags & PPA_F_MTU)) {
		route.update_flags |= PPA_UPDATE_ROUTING_ENTRY_MTU_IX;
	}
	if ((flags & PPA_F_SESSION_OUT_VLAN)) {
		route.update_flags |=
			PPA_UPDATE_ROUTING_ENTRY_OUT_VLAN_INS_EN |
			PPA_UPDATE_ROUTING_ENTRY_OUT_VLAN_RM_EN;
	}

	route.entry = p_item->routing_entry;

	return ppa_hsel_update_routing_entry(&route, 0,
			p_item->caps_list[0].hal_id);
}

/*****************************************************************************************
The statistics needs to be updated to p_item periodically by a timer thread
This function is called from the timer thread periodically
for a routed session statics needs to be read from the HAL that does the routing fn
The for each session p_item->caps_list[0] will always give us the HAL id which does the routing
*****************************************************************************************/
int32_t update_subif_port_info(struct mc_session_node *p_item,
				   uint32_t *dest_list, uint32_t *dest_list_vap,
				   uint16_t *subif)
{
	int32_t i;
	struct netif_info *p_netif_info = NULL;

	for (i = 0; i < PPA_MAX_MC_IFS_NUM; i++) {
		if (ppa_netif_lookup(p_item->grp.txif[i].netif->name,
					 &p_netif_info) != PPA_SUCCESS)
			continue;

		if (!(p_netif_info->flags & NETIF_PHYS_PORT_GOT)) {
			ppa_netif_put(p_netif_info);
			return PPA_FAILURE;
		}

		*dest_list_vap |= (1 << p_netif_info->phys_port);
#if IS_ENABLED(CONFIG_SOC_GRX500)
		subif[p_netif_info->phys_port] |= (1 << (p_netif_info->subif_id >> 8));
#endif
		ppa_netif_put(p_netif_info);
	}

	return PPA_SUCCESS;
}

static bool ppa_is_mc_sess_accelable(struct mc_session_node *p_item)
{
	int8_t i;
	struct netif_info *p_netif_info = NULL;
	bool is_accelable = true;

	for (i = 0; i < PPA_MAX_MC_IFS_NUM; i++) {
		if (ppa_netif_lookup(p_item->grp.txif[i].netif->name,
					 &p_netif_info) != PPA_SUCCESS)
			continue;

		/*
		 * Multicast seesion is not accelerable,
		 * if any of the members has the following netif type
		 * - GRE type
		 * - VLAN type
		 * - VxLAN type
		 */
		if ((p_netif_info->flags & NETIF_GRE_TUNNEL) ||
		    (p_netif_info->flags & NETIF_VXLAN_TUNNEL)
		    || (p_netif_info->flags & NETIF_VLAN)) {
			    is_accelable = false;
			    ppa_netif_put(p_netif_info);
			    break;
		}
		ppa_netif_put(p_netif_info);
	}

	return is_accelable;
}

/* ****************************************************************************************
// This function will be called when a new group is added
// replaces ppa_hw_add_mc_group
**************************************************************************************** */
int32_t ppa_hsel_add_wan_mc_group(struct mc_session_node *p_item, PPA_C_OP *cop)
{
	/*find the capabilities needed for this session*/
	/*call the HAL apis as needed*/
	PPA_MC_INFO mc = { 0 };
	uint32_t num_caps = 0, i = 0, j = 0;
	int ret = PPA_SUCCESS;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	uint32_t idx;
	uint32_t l_dest_list = 0, l_dest_list_vap = 0;
	struct mc_session_node *old_item;
#endif

	if (!ppa_is_mc_sess_accelable(p_item)) {
		ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP,
			  "[%s:%d] not accelerable mcast session 0x%px!\n",
			  __func__, __LINE__, p_item);
		return PPA_FAILURE;
	}

	ppa_memset(&mc, 0, sizeof(mc));
	
	mc.p_item = p_item;
	mc.cop = cop;
	mc.dest_list = p_item->dest_ifid;

	ppa_memset(p_item->caps_list, 0, MAX_MC_SESS_CAPS * sizeof(PPA_HSEL_CAP_NODE));

	/* ///////////////////////////////////////////////////////////////
	//  find all the capabilities needed by this session
	/////////////////////////////////////////////////////////////// */

	/*must be multicast downstream session*/
	p_item->caps_list[num_caps++].cap = SESS_MC_DS;

#if IS_ENABLED(CONFIG_SOC_GRX500)
	/* for additional capability requirement*/
	if (p_item->num_vap > 1)
		p_item->caps_list[num_caps++].cap = SESS_MC_DS_VAP;

	update_subif_port_info(p_item, &l_dest_list, &l_dest_list_vap, mc.dest_subif);
#endif
	/* //////////////////////////////////////////////////////////////
	// for each capability find out the HAL
	/////////////////////////////////////////////////////////////// */
	if (ppa_select_hals_from_caplist(0, num_caps, p_item->caps_list) !=
			PPA_SUCCESS) {
		return PPA_FAILURE;
	}

	for (i = 0; i < num_caps;) {
		j = ppa_group_hals_in_capslist(i, num_caps, p_item->caps_list);

		/*Based on the capability of first entry in the list we can decide the action*/
		switch (p_item->caps_list[i].cap) {
			case SESS_MC_DS:
#if IS_ENABLED(CONFIG_SOC_GRX500)
				if (p_item->caps_list[i].hal_id == PAE_HAL) {
					if (p_item->num_vap > 1) {
						mc.dest_list = 1; /* cpu port for MPE processing */
						mc.dest_list |= l_dest_list;
					}

					/*
					 * In case of PAE HAL we need to add source mac of the packet to MAC table
					 * for the packet to be forwarded in aceelreation
					 */
					if (p_item->grp.src_netif && !is_zero_mac(p_item->s_mac))
						ppa_bridge_entry_add(p_item->s_mac, NULL, p_item->grp.src_netif, PPA_F_STATIC_ENTRY);
				} else if (p_item->caps_list[i].hal_id == MPE_HAL) {
					p_item->session_action = (void *)ppa_session_mc_construct_tmplbuf(&mc, l_dest_list_vap);
					if (!p_item->session_action) {
						ret = PPA_FAILURE;
						break;
					}
					mc.dest_list = l_dest_list_vap;
				}

	  			mc.tnnl_info.tunnel_type = TUNNEL_TYPE_NULL;
	  			if ( ((p_item->flags) & SESSION_TUNNEL_6RD) == SESSION_TUNNEL_6RD) {
					mc.tnnl_info.tunnel_type = TUNNEL_TYPE_6RD;
	  			} else if ( ((p_item->flags) &  SESSION_TUNNEL_DSLITE) == SESSION_TUNNEL_DSLITE ) {
					mc.tnnl_info.tunnel_type = TUNNEL_TYPE_DSLITE;
	  			} else if( ((p_item->flag2) & SESSION_FLAG2_GRE)  == SESSION_FLAG2_GRE ) {
					mc.tnnl_info.tunnel_type = TUNNEL_TYPE_IPOGRE;
	  			}
#if defined(L2TP_CONFIG) && L2TP_CONFIG
	  			else if ( ((p_item->flags) & SESSION_VALID_PPPOL2TP) == SESSION_VALID_PPPOL2TP) {
					mc.tnnl_info.tunnel_type = TUNNEL_TYPE_L2TP;
	  			}
#endif
				mc.update_flags |= FLAG_SESSION_HI_PRIO;
#endif
				/*check whether more HALs needs to be configured to complete this session configuration*/
				if ((ret = ppa_hsel_add_wan_mc_entry(&mc, 0, p_item->caps_list[i].hal_id)) ==
						PPA_SUCCESS) {
#if IS_ENABLED(CONFIG_SOC_GRX500)
					if (p_item->caps_list[i].hal_id == PAE_HAL) {
						/* in this case we have to check the flags to see whether the PAE has done any swap*/
						if (mc.update_flags & FLAG_SESSION_SWAPPED) {
							/* TBD: search in the session list to find out the session with routing_entry == route.entry (same index)
							// clear the flag SESSION_ADDED_IN_HW
							// make session->routing_entry = ~0;
							*/
							for (idx = 0; idx < NUM_ENTITY(g_mc_group_list_hash_table);
									idx++) {
								ppa_hlist_for_each_entry(old_item,
										&g_mc_group_list_hash_table[idx],
										mc_hlist) {
									if (old_item->mc_entry == mc.p_entry) {
										old_item->mc_entry = 0;
										old_item->flags &= ~SESSION_ADDED_IN_HW;
										ppa_atomic_dec(&g_hw_session_cnt);
										goto SESSION_SWAP_CONTINUE;
									}
								}
							}
						}
					}
SESSION_SWAP_CONTINUE:
					p_item->sess_hash = mc.p_entry;
#endif /*IS_ENABLED(CONFIG_SOC_GRX500)*/
					p_item->mc_entry = mc.p_entry;
					p_item->flags |= SESSION_ADDED_IN_HW;
					ppa_atomic_inc(&g_hw_session_cnt);
					break;
				}
#if IS_ENABLED(CONFIG_SOC_GRX500)
				else if (p_item->caps_list[i].hal_id == MPE_HAL) {
					ppa_session_mc_destroy_tmplbuf((void *)p_item->session_action);
					p_item->session_action = NULL;
				}
#endif
				ret = PPA_EAGAIN;
				break;
#if IS_ENABLED(CONFIG_SOC_GRX500)
			case SESS_MC_DS_VAP:
				/* TBD: complementary processing needs to be added*/
				p_item->session_action = (void *)ppa_session_mc_construct_tmplbuf(&mc, l_dest_list_vap);
				if (!p_item->session_action) {
					ret = PPA_FAILURE;
					break;
				}
				mc.dest_list = l_dest_list_vap;

				ret = ppa_hsel_add_wan_mc_entry(&mc, 0, p_item->caps_list[i].hal_id);
				if (ret != PPA_SUCCESS) {
					ppa_session_mc_destroy_tmplbuf((void *)p_item->session_action);
					p_item->session_action = NULL;

					/*revert all the settings done for caps_list[0] */
					ppa_hsel_del_wan_mc_entry(&mc, 0, p_item->caps_list[0].hal_id);
					p_item->mc_entry = ~0;
					p_item->flags &= ~SESSION_ADDED_IN_HW;
					ppa_atomic_dec(&g_hw_session_cnt);
					if (p_item->caps_list[0].hal_id == PAE_HAL) {
						if (!is_zero_mac(p_item->s_mac))
							ppa_bridge_entry_delete(p_item->s_mac, NULL, PPA_F_STATIC_ENTRY);
					}
				}

				break;
#endif
			default:

				break;
		}
		if (ret != PPA_SUCCESS) {
			/*if the capability is SESS_MC_DS then we can check for any more HALs registred
			// TBD: if the capability needed is SESS_MC_DS_VAP we have to call a different 
			// API to forward the packet to CPU port / clear the MPE flag in the PAE configuration
			//find the next HAL registred for this capability */
			if (ppa_select_hals_from_caplist(i, j, p_item->caps_list) !=
					PPA_SUCCESS) {
				/* this session cannot be added to the HW*/
				return PPA_FAILURE;
			} else {
				continue;
			}
		}
		/*returned success in step 1 proceed*/
		i += j;
	}

	p_item->num_caps = num_caps;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"\n%s %s %d add mc entry success\n sip=%u\n dip=%u\n sp=%u\n dp=%u\n src_netif=%s\n dest_ifid=%0x\n\
			dest_subifid=%0x\n group_id=%u\n p_item->sess_hash=%u\n num_caps=%d \nlast_hit_time=%d \n", __FILE__,
			__FUNCTION__, __LINE__, p_item->grp.source_ip.ip.ip, p_item->grp.ip_mc_group.ip.ip, 
			p_item->grp.src_port, p_item->grp.dst_port, p_item->grp.src_netif->name, p_item->dest_ifid, 
			p_item->dest_subifid, p_item->grp.group_id, p_item->sess_hash, p_item->num_caps, p_item->last_hit_time);
#endif
	return ret;
}

/*****************************************************/
// called when the multicast group is deleted
// replaces ppa_hw_del_mc_group
/*****************************************************/
void ppa_hsel_del_wan_mc_group(struct mc_session_node *p_item)
{
	PPA_MC_INFO mc = { 0 };
	uint64_t tmp = 0, pkts = 0;
	uint32_t i, j;
#if IS_ENABLED(CONFIG_SOC_GRX500)
	uint32_t l_dest_list=0, l_dest_list_vap=0;
#endif

	if (!(p_item->flags & SESSION_ADDED_IN_HW))
		return;

	if (!p_item->num_caps)
		return;

	mc.p_item = p_item;
	mc.p_entry = p_item->mc_entry;

	/*only first HAL in the cap_list is needded tpo get the stats*/
	ppa_hsel_get_mc_entry_bytes(&mc, 0, p_item->caps_list[0].hal_id);
	if (mc.f_hit) {

		tmp = mc.bytes;
		pkts = mc.packets;

		/* reset the accelerated counters, as it is going to be deleted */
		p_item->acc_bytes = 0;
		p_item->last_bytes = 0;

		if (p_item->grp.src_netif)
			update_netif_hw_mib(ppa_get_netif_name(p_item->grp.src_netif), tmp,
					1);

		for (i = 0; i < PPA_MAX_MC_IFS_NUM; i++)
			if (p_item->grp.txif[i].netif)
				update_netif_hw_mib(ppa_get_netif_name(p_item->grp.txif[i].netif), tmp,
						0);
	}

#if IS_ENABLED(CONFIG_SOC_GRX500)
	update_subif_port_info(p_item, &l_dest_list, &l_dest_list_vap, mc.dest_subif);
#endif

	/*when init, these entry values are ~0, the max the number which can be detected by these functions*/
	ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP,
		  "ppa_hw_del_mc_group:  remove %d from HW\n",
		  p_item->mc_entry);
	for (i = 0; i < p_item->num_caps;) {

		j = ppa_group_hals_in_capslist(i, p_item->num_caps,
						   p_item->caps_list);

		if (p_item->caps_list[i].cap == SESS_MC_DS) {

#if IS_ENABLED(CONFIG_SOC_GRX500)
			if (p_item->caps_list[i].hal_id == MPE_HAL)
				mc.dest_list = l_dest_list_vap;
#endif

			/*Delete the multicast sessionfrom HW */
			ppa_hsel_del_wan_mc_entry(&mc, 0, p_item->caps_list[i].hal_id);

			p_item->mc_entry = ~0;

#if IS_ENABLED(CONFIG_SOC_GRX500)
			if (p_item->caps_list[i].hal_id == PAE_HAL) {
				if (!is_zero_mac(p_item->s_mac))
					ppa_bridge_entry_delete(p_item->s_mac, NULL, PPA_F_STATIC_ENTRY);
			} else if (p_item->caps_list[i].hal_id == MPE_HAL) {
				ppa_session_mc_destroy_tmplbuf((void *)p_item->session_action);
				p_item->session_action = NULL;
			}
#endif
			p_item->flags &= ~SESSION_ADDED_IN_HW;

			ppa_atomic_dec(&g_hw_session_cnt);
		}
#if IS_ENABLED(CONFIG_SOC_GRX500)
		else if (p_item->caps_list[i].cap == SESS_MC_DS_VAP) {
			/*TBD: undo the mpe  config*/

			/*mc.p_entry is expected to be filled beforehand*/
			mc.dest_list = l_dest_list_vap;

			ppa_hsel_del_wan_mc_entry(&mc, 0, p_item->caps_list[i].hal_id);
			ppa_session_mc_destroy_tmplbuf((void *)p_item->session_action);
			p_item->session_action = NULL;

		}
#endif
		i += j;
	}
}

/****************************************************************************************/
// Called for each join/leave messages.
// NB:This function will not do the hal selection again.(done during ADD)
// It just update the entries in the already selected HAL.
// If the current operation requires re-selection of HALs.. A DEL and ADD is MUST.
/****************************************************************************************/
int32_t ppa_hsel_update_wan_mc_group(struct mc_session_node *p_item, PPA_C_OP *cop,
		uint32_t flags)
{
	PPA_MC_INFO mc = { 0 };
	uint32_t i = 0, j = 0;
	int ret = PPA_SUCCESS;

	ppa_memset(&mc, 0, sizeof(mc));

	mc.p_item = p_item;
	mc.cop = cop;
	mc.dest_list = p_item->dest_ifid;
	
	if ((p_item->flags & SESSION_ADDED_IN_HW)) {

		mc.p_entry = p_item->mc_entry;

		for (i = 0; i < p_item->num_caps;) {
			j = ppa_group_hals_in_capslist(i, p_item->num_caps, p_item->caps_list);

			/* Based on the capability of first entry in the list we can decide the action*/
			switch (p_item->caps_list[i].cap) {
			case SESS_MC_DS:

				if ((ret = ppa_hsel_update_wan_mc_entry(&mc, 0, p_item->caps_list[i].hal_id))
					== PPA_SUCCESS) {
					break;
				}
				p_item->mc_entry = mc.p_entry;
				ret = PPA_EAGAIN;

				break;
			case SESS_MC_DS_VAP:
				/*Complementary processing not supported*/
				ppa_debug(DBG_ENABLE_MASK_DUMP_MC_GROUP, "ppa_hw_update_mc_group:  doesnot support this capability\n");
				break;
			default:
				break;
			}
			if (ret != PPA_SUCCESS) {
			/* if the capability is SESS_MC_DS then we can check for any more HALs registred
			// TBD: if the capability needed is SESS_MC_DS_VAP we have to call a different 
			// API to forward the packet to CPU port / clear the MPE flag in the PAE configuration
			//find the next HAL registred for this capability */
				if (ppa_select_hals_from_caplist(i, j, p_item->caps_list) !=
					PPA_SUCCESS) {
					return PPA_FAILURE;
				} else {
					continue;
				}
			}
			/* returned success in step 1 proceed*/
			i += j;
		}
	}
	return PPA_SUCCESS;
}

#if IS_ENABLED(CONFIG_SOC_GRX500)
int32_t ppa_add_class_rule(PPA_CLASS_RULE *rule)
{
	int32_t ret=PPA_SUCCESS;
	uint32_t flag=0;
	if ( ( ret=ppa_hsel_add_class_rule(rule, flag, PAE_HAL) ) == PPA_SUCCESS )
	{
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add success!!!\n",__FILE__,__FUNCTION__,__LINE__);
	}
	else {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_add_class_rule : class rule add Fail\n");
		return PPA_FAILURE;
	}

	return ret;
}

int32_t ppa_mod_class_rule(PPA_CLASS_RULE *rule)
{
	int32_t ret=PPA_SUCCESS;
	uint32_t flag=0;
	if ( ( ret=ppa_hsel_mod_class_rule(rule, flag, PAE_HAL) ) == PPA_SUCCESS )
	{
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add success!!!\n",__FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}

int32_t ppa_del_class_rule(PPA_CLASS_RULE *rule)
{
	int32_t ret=PPA_SUCCESS;
	uint32_t flag=0;
	if ( ( ret=ppa_hsel_del_class_rule(rule, flag, PAE_HAL) ) == PPA_SUCCESS )
	{
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add success!!!\n",__FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}

int32_t ppa_get_class_rule(PPA_CLASS_RULE *rule)
{
	int32_t ret=PPA_SUCCESS;
	uint32_t flag=0;
	if ( ( ret=ppa_hsel_get_class_rule (rule, flag, PAE_HAL) ) == PPA_SUCCESS )
	{
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d add success!!!\n",__FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}

int32_t ppa_set_qos_meter(PPA_QOS_METER_CFG *meter_cfg, uint32_t hal_id)
{
	int32_t ret;
	uint32_t flag;

	flag = 0;
	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT, "ppa_set_qos_meter called for hal:[%d]\n", hal_id);
	ret = ppa_hsel_set_qos_meter_entry(meter_cfg, flag, hal_id);
	if (ret == PPA_SUCCESS) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"%s %s %d meter add success!!!\n",__FILE__,__FUNCTION__,__LINE__);
	}
	return ret;
}
#endif /*IS_ENABLED(CONFIG_SOC_GRX500)*/

#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
static bool is_dynamic_port (struct netif_info *ifinfo)
{
	return (ifinfo->phys_port > 6 && ifinfo->phys_port < 15);
}

int32_t ppa_add_lro_entry(struct uc_session_node * p_item, uint32_t f_test)
{
	int32_t ret = PPA_SUCCESS;
	PPA_LRO_INFO lro_entry = {0};
	PPA_HSEL_CAP_NODE cap_node = {0};

#if IS_ENABLED(CONFIG_SOC_GRX500)
	/***********************************************/
	/*LRO not currently working with directpath interfaces
	  remove this section once this issue is fixed*/
	struct netif_info *rx_ifinfo;

	if (p_item->rx_if) {
		if (PPA_SUCCESS !=
			ppa_netif_lookup(ppa_get_netif_name(p_item->rx_if),
					 &rx_ifinfo)) {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				  "failed in getting info structure of rx_if (%s)\n",
				  ppa_get_netif_name(p_item->rx_if));
			return PPA_FAILURE;
		} else {
			if (rx_ifinfo->flags & NETIF_BRIDGE) {
				ppa_netif_put(rx_ifinfo);
				if (p_item->br_rx_if) {
					if (PPA_SUCCESS !=
						ppa_netif_lookup(ppa_get_netif_name(p_item->br_rx_if),
								 &rx_ifinfo)) {
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
							  "failed in getting info structure of rx_if (%s)\n",
							  ppa_get_netif_name(p_item->br_rx_if));
						return PPA_FAILURE;
					} else {
						if ((rx_ifinfo->flags & NETIF_DIRECTPATH)
							|| (rx_ifinfo->flags & NETIF_DIRECTCONNECT_WIFI)
							|| (is_dynamic_port(rx_ifinfo))) {
								ppa_netif_put(rx_ifinfo);
								return PPA_FAILURE;
						}
						ppa_netif_put(rx_ifinfo);
						ret = PPA_SUCCESS;
					}
				} else {
					return PPA_FAILURE;
				}
			} else if ((rx_ifinfo->flags & NETIF_DIRECTPATH)
					|| (rx_ifinfo->flags & NETIF_DIRECTCONNECT_WIFI)
					|| (is_dynamic_port(rx_ifinfo))) {
						ppa_netif_put(rx_ifinfo);
						return PPA_FAILURE;
					/* On enabling bridge flow learning, p_item rxif is wlan0/wlan2 not br-lan*/
					/* Skip LRO learning for this scenario */
			} else {
				ppa_netif_put(rx_ifinfo);
			}
		}
	} else {
		return PPA_FAILURE;
	}
#endif /*CONFIG_SOC_GRX500*/

	lro_entry.p_item = p_item;
	if (p_item->flags & SESSION_IS_IPV6) {
		lro_entry.f_ipv6 = 1;
		ppa_memcpy(lro_entry.src_ip.ip6, p_item->pkt.src_ip.ip6,
			   sizeof(lro_entry.src_ip.ip6));
		ppa_memcpy(lro_entry.dst_ip.ip6, p_item->pkt.dst_ip.ip6,
			   sizeof(lro_entry.dst_ip.ip6));
	} else {
		lro_entry.src_ip.ip = p_item->pkt.src_ip.ip;
		lro_entry.dst_ip.ip = p_item->pkt.dst_ip.ip;
	}

	lro_entry.src_port = p_item->pkt.src_port;
	lro_entry.dst_port = p_item->pkt.dst_port;

	cap_node.cap = LRO_ENG;
	ppa_select_hals_from_caplist(0,1,&cap_node); 

	ret = ppa_hsel_add_lro_entry(&lro_entry, 0, cap_node.hal_id);
	if(ret == PPA_SUCCESS) {
		p_item->lro_sessid = lro_entry.session_id;
		p_item->flag2 |= SESSION_FLAG2_LRO;
	}
	return ret;
}

int32_t ppa_del_lro_entry(struct uc_session_node * p_item)
{
	PPA_LRO_INFO lro_entry={0};
	PPA_HSEL_CAP_NODE cap_node= {0};
	lro_entry.session_id = p_item->lro_sessid;
	cap_node.cap = LRO_ENG;
	ppa_select_hals_from_caplist(0,1,&cap_node); 
	return ppa_hsel_del_lro_entry(&lro_entry, 0, cap_node.hal_id);
}
#endif /*IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)*/
int32_t ppa_lro_entry_criteria(struct uc_session_node *p_item, PPA_BUF *ppa_buf, uint32_t flags)
{
	/* 1. Session gets added to lro only after 64k bytes are transmitted (TCP segment size)
	// 2. Session gets added to lro only if packet size is large (to avoid tcp ack)
	// have to add proper session entry criteria based on data rate here.
	//	ppa_get_pkt_rx_src_mac_addr(ppa_buf, p_item->src_mac);
	//	if(ppa_check_is_lro_enabled_netif(p_item->rx_if, p_item->src_mac)!=PPA_SUCCESS)
	//	return PPA_FAILURE; */

#if !IS_ENABLED(CONFIG_SOC_GRX500)
	if (p_item->flags & SESSION_WAN_ENTRY)
		return PPA_FAILURE;
#endif

	if ( p_item->host_bytes > 64000 && ppa_skb_len(ppa_buf) > (p_item->mtu - 128) ) {
		return PPA_SUCCESS;
	}
	return PPA_FAILURE;
}
EXPORT_SYMBOL(ppa_lro_entry_criteria);

