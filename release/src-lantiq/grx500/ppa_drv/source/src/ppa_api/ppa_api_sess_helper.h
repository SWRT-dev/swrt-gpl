/*******************************************************************************
**
** FILE NAME    : ppa_api_sess_helper.h
** PROJECT      : PPA
** MODULES      : PPA API - Routing/Bridging(flow based) helper routines
**
** DATE         : 24 Feb 2015
** AUTHOR       : Mahipati Deshpande
** DESCRIPTION  : PPA Protocol Stack Hook API Session Operation Functions
** COPYRIGHT    : Copyright (c) 2020, MaxLinear, Inc.
**                Copyright (c) 2017 Intel Corporation
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author                $Comment
** 24 Feb 2015  Mahipati               The helper functions are moved from
**                                     ppa_api_session.c
*******************************************************************************/
#ifndef __PPA_API_SESS_HELPER__

#define __PPA_API_SESS_HELPER__

#define ppa_session_get_index(key) (((u64)(key) * (SESSION_LIST_HASH_TABLE_SIZE)) >> 32)

/* Session list lock init and destroy */
int ppa_session_list_lock_init(void);
void ppa_session_list_lock_destroy(void);

/* Session bucket lock init and destroy */
int ppa_session_bucket_lock_init(void);
void ppa_session_bucket_lock_destroy(void);

/* Session list init and destroy routines */
int ppa_session_list_init(void);
void ppa_session_list_free(void);

/* Session list lock and unlock routine */
void ppa_session_list_lock(void);
void ppa_session_list_unlock(void);
void ppa_session_list_read_lock(void);
void ppa_session_list_read_lock(void);

/* Session bucket lock and unlock routine */
void ppa_session_bucket_lock(uint32_t);
void ppa_session_bucket_unlock(uint32_t);

/* Session allocate, insert and delete routines */
struct uc_session_node *ppa_session_alloc_item(void);
void __ppa_session_insert_item(struct uc_session_node *);
void __ppa_session_delete_item(struct uc_session_node *p_item);
void ppa_session_list_free_item(struct uc_session_node *p_item);

void ppa_session_delete_by_netif(PPA_NETIF *);

/* TODO: Can these functions be inline */
#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
/* Bridged session flows timer handling routines */
int ppa_session_br_timer_init(struct uc_session_node *p_item);
void  ppa_session_br_timer_add(struct uc_session_node *p_item);
void  ppa_session_br_timer_del(struct uc_session_node *p_item);
#endif /*IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)*/
#if IS_ENABLED(CONFIG_PPA_API_DIRECTCONNECT)
void ppa_session_delete_by_subif(PPA_DP_SUBIF *subif);
#endif
void ppa_session_delete_by_macaddr(uint8_t *mac);

#if IS_ENABLED(CONFIG_SOC_GRX500)
void ppa_get_src_mac_pitem_pae(PPA_BUF *ppa_buf, uint8_t *src_mac);
#endif

/* Session cache related functions */
int32_t ppa_session_cache_create(void);
int32_t ppa_session_cache_shrink(void);
void ppa_session_cache_destroy(void);

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
extern int32_t __ppa_search_ipsec_rtindex(struct uc_session_node *pp_item);
#endif

void ppa_session_list_read_unlock(void);
void ppa_session_delete_by_ip(PPA_IPADDR *pIP);

struct session_action *ppa_session_mc_construct_tmplbuf(void *p_item,
							uint32_t dest_list);
void ppa_session_mc_destroy_tmplbuf(void  *sessionAction);
#if IS_ENABLED(CONFIG_PPA_IPTABLE_EVENT_HANDLING)
int ppa_iptable_event(PPA_NOTIFIER_BLOCK *, unsigned long, void *);
#endif
int32_t ppa_is_sess_accelable(struct uc_session_node *p_item);
/* Debug macro/routines */
#if 0
#define	ppa_session_print(p_item) printk("<%s> %s: proto:%d %pI4:%d <-> %pI4:%d count=%u\n", \
		__FUNCTION__,	\
		ppa_is_BrSession((p_item)?"Bridged":"Routed"), \
		p_item->pkt.ip_proto, \
		&p_item->pkt.src_ip,p_item->pkt.src_port, \
		&p_item->pkt.dst_ip,p_item->pkt.dst_port, \
		p_item->num_adds);
#else
#define  ppa_session_print
#endif

#endif
