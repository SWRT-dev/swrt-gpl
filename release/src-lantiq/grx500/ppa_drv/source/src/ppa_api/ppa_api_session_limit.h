/*******************************************************************************
**
** FILE NAME    : ppa_api_session_limit.h
** PROJECT      : PPA
** MODULES      : PPA API (Session Limiting APIs)
**
** DATE         : 20 May 2014
** AUTHOR       : Lantiq
** DESCRIPTION  : PPA Session Limiting Header
** COPYRIGHT    : Copyright (c) 2017 Intel Corporation
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author                $Comment
**                                     Features
*******************************************************************************/

#include <net/netfilter/nf_conntrack_session_limit.h>
#define EWMA_WINDOW_PACKETS 10
#define EWMA_IGNORE_SEED_PACKETS 30
#define LAN_RESERVE_COLLISIONS                  0
#define WAN_RESERVE_COLLISIONS                  0

/*session management macros*/
#define  MAX_DATA_FLOW_ENGINES 3 /* will be changed to runtime value*/

#define  MAX_SESSION_PRIORITY 3
#define  MAX_SESSION_TYPE 2

enum ppa_engine_type{
PAE,
MPE,
#if defined(CONFIG_PPA_API_SW_FASTPATH)
SWAC
#endif
};

enum sess_type{
LAN,
WAN};

extern uint32_t g_ppa_lan_reserve_collisions;
extern uint32_t g_ppa_wan_reserve_collisions;
extern uint32_t g_ppa_lan_current_collisions;
extern uint32_t g_ppa_wan_current_collisions;
extern uint32_t g_ppa_low_prio_data_rate;
extern uint32_t g_ppa_def_prio_data_rate;
extern uint32_t g_ppa_low_prio_thresh;
extern uint32_t g_ppa_def_prio_thresh;

extern int32_t ppa_session_prio(PPA_SESSION *, uint32_t);
extern unsigned int session_count[MAX_SESSION_TYPE][MAX_SESSION_PRIORITY][MAX_DATA_FLOW_ENGINES];

extern uint32_t ppa_reset_ewma_stats(struct uc_session_node *p_item, uint32_t flags);
extern uint32_t ppa_session_pass_criteria(PPA_BUF *ppa_buf, struct uc_session_node *p_item, uint32_t flags);
extern uint32_t ppa_swap_sessions(uint32_t flags);
extern uint32_t ppa_update_ewma(PPA_BUF *ppa_buf, struct uc_session_node *p_item, uint32_t flags);
extern uint32_t ppa_session_update_collisions(uint32_t flags);
extern uint32_t ppa_session_store_ewma(PPA_BUF *ppa_buf, struct uc_session_node *p_item, uint32_t flags);
extern uint32_t ppa_decide_collision(struct uc_session_node *p_item);
extern uint32_t ppa_num_collision_sessions(uint32_t flag);
extern uint32_t ppa_session_limit_update_flags(struct uc_session_node *p_item, uint32_t flags);
extern uint32_t ppa_session_record_time(PPA_BUF *ppa_buf, struct uc_session_node *p_item, uint32_t flags);
extern uint32_t ppa_init_session_limit_params(PPA_INIT_INFO *p_info, uint32_t flags);
extern void ppa_session_mgmt_init(void);
extern void ppa_session_mgmt_exit(void);
extern void update_session_mgmt_stats(struct uc_session_node *p_item, enum ppa_oper_type oper);
