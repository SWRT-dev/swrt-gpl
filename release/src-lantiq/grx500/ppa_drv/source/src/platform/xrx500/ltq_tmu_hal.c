/*******************************************************************************
 **
 ** FILE NAME	: ltq_tmu_hal.c
 ** PROJECT	: TMU HAL
 ** MODULES	: TMU (QoS Engine )
 **
 ** DATE	: 8 Aug 2014
 ** AUTHOR	: Purnendu Ghosh
 ** DESCRIPTION	: TMU HAL Layer
 ** COPYRIGHT	: Copyright (c) 2020, MaxLinear, Inc.
 **               Copyright (c) 2009, Lantiq Deutschland GmbH
 **               Am Campeon 3; 85579 Neubiberg, Germany
 **
 **	 For licensing information, see the file 'LICENSE' in the root folder of
 **	 this software module.
 **
 ** HISTORY
 ** $Date		$Author				$Comment
 ** 8 Aug 2014		Purnendu Ghosh		 Initiate Version
 *******************************************************************************/
/*
 *	Common Header File
 */
#include <linux/version.h>
#include <generated/autoconf.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <net/checksum.h>
#include <lantiq.h>
#include <lantiq_soc.h>
#include <linux/clk.h>
#include <net/ip_tunnels.h>
#include <linux/if_arp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <net/ip6_tunnel.h>
#include <net/ipv6.h>

#include <linux/if_link.h>

/*
 * Driver Header files 
 */
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_hal_api.h>
#include <net/lantiq_cbm_api.h>
#include <net/drv_tmu_ll.h>
#include <net/datapath_api.h>
#include "ltq_tmu_hal.h"
#include "ltq_tmu_hal_api.h"
#include "ltq_tmu_hal_debug.h"
#include <net/switch_api/lantiq_gsw_api.h>

#define dbg printk
#define TMU_HAL_Q_PRIO_LEVEL 9
#define INVALID_SCHED_ID 0xFF
#define INVALID_SHAPER_ID 256
#define CPU_PORT_ID 2
#define PMAC_CPU_PORT_ID 0
#define TMU_HAL_QUEUE_SHAPER_INGRESS_MPE	 0x0F000000
#define TMU_HAL_DEL_SHAPER_CFG 0x00F00000
#define TMU_HAL_ADD_QUEUE_EXPLICIT	 0x00000000
#define TMU_HAL_QUEUE_FLUSH_TIMEOUT 100000

#define CBM_QUEUE_FLUSH_SUPPORT
#define PPPOA_SUPPORT
#define WRAPAROUND_32_BITS 0xFFFFFFFF
#define LRO_QID_DEF 37

#define TMU_ALLOC(type, var) \
	do { \
		var = kzalloc(sizeof(type), GFP_ATOMIC); \
		if (!var) { \
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR, \
				"[%s:%d] TMU_ALLOC() failed\n", \
				__func__, __LINE__); \
			return TMU_HAL_STATUS_ERR; \
		}; \
	} while (0)

int8_t LAN_PORT[] = {2,3,4,5,6};
int8_t LAN_PORT_Q[][4] = { {8,9,10,11}, {12,13,14,15}, {16,17,18,19},{20,21,22,23},{24,25,26,27} };
int32_t WT_Q[] = {20480,20480,65535,65535};
int32_t SCH_Q[] = {1,1,0,0};
#define MAX_NUMBER_OF_LAN_PORTS 4

uint32_t g_tmu_dbg = TMU_DEBUG_ERR;
static uint32_t dp_reserve;
uint32_t high_prio_q_limit = 16;
module_param(dp_reserve, int, 0);
module_param(high_prio_q_limit, int, 0);

uint32_t g_Inst;

/* Global Resources*/
uint32_t g_No_Of_TMU_Res_Port;
uint32_t g_No_Of_TMU_Res_Scheduler;
uint32_t g_No_Of_TMU_Res_Queue;
uint32_t g_No_Of_Ingress_Interface;

/* CPU Port Resources*/
int32_t g_CPU_PortId;
int32_t g_CPU_Sched;
int32_t g_CPU_Queue;

/* MPE Port Resources*/
int32_t g_MPE_PortId = -1;
int32_t g_MPE_Sched = -1;
int32_t g_MPE_Queue = -1;
/* Directpath Egress */
uint32_t g_DpEgressQueueScheduler=0;
uint32_t g_Root_sched_id_DpEgress = 0xFF;

/* Directpath Ingress */
uint32_t g_IngressQueueScheduler;
uint32_t g_Root_sched_id_Ingress = 0xFF;

/* Directpath Ingress TMU Resources*/
uint32_t g_Port_id_DpIngress;
uint32_t g_Queue_id_DpIngress;
uint32_t g_DpIngressQueueScheduler = 0xFF;

extern uint32_t ppa_drv_generic_hal_register(uint32_t hal_id, ppa_generic_hook_t generic_hook);
extern void ppa_drv_generic_hal_deregister(uint32_t hal_id);

extern uint32_t ppa_drv_register_cap(PPA_API_CAPS cap, uint8_t wt, PPA_HAL_ID hal_id);
extern uint32_t ppa_drv_deregister_cap(PPA_API_CAPS cap, PPA_HAL_ID hal_id);

extern int32_t (*tmu_hal_get_csum_ol_mib_hook_fn)(struct tmu_hal_qos_stats *csum_mib, uint32_t flag);
extern int32_t (*tmu_hal_clear_csum_ol_mib_hook_fn)(uint32_t flag);
extern int32_t(*tmu_hal_get_qos_mib_hook_fn)(struct net_device *netdev,	dp_subif_t *subif_id, int32_t queueid, struct tmu_hal_qos_stats *qos_mib, uint32_t flag);
extern int32_t (*tmu_hal_clear_qos_mib_hook_fn)(struct net_device *netdev, dp_subif_t *subif_id, int32_t queueid, uint32_t flag);
extern int32_t (*mpe_hal_set_checksum_queue_map_hook_fn)(uint32_t pmac_port, uint32_t flags);

enum tmu_hal_errorcode tmu_hal_flush_queue_if_qocc(struct net_device *netdev, char *dev_name, int32_t index, int32_t tmu_port, int32_t remap_to_qid, uint32_t flags);
static int tmu_hal_add_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags);
static int tmu_hal_del_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags);
static int tmu_hal_get_tmu_res_from_netdevice(struct net_device *netdev, char *dev_name, uint32_t sub_if, dp_subif_t *dp_if, uint32_t *tmu_ports_cnt, cbm_tmu_res_t **res, uint32_t flags);

static int	
tmu_hal_add_queue(struct net_device *netdev, char *dev_name, char tc[MAX_TC_NUM], uint8_t no_of_tc, uint32_t schedid, uint32_t prio_type, uint32_t prio_level, uint32_t weight, uint32_t flowId, QOS_Q_ADD_CFG *param, uint32_t flags);
static int tmu_hal_delete_queue(struct net_device *netdev, char *dev_name, uint32_t index, uint32_t priority, uint32_t scheduler_id, uint32_t flowId, uint32_t flags);
static int tmu_hal_modify_queue(struct net_device *netdev, QOS_Q_MOD_CFG *param);
int tmu_hal_add_shaper_index(QOS_RATE_SHAPING_CFG *cfg);
int tmu_hal_add_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg,int32_t tmu_q_idx, uint32_t flags);
int tmu_hal_del_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, int32_t tmu_q_idx, uint32_t flags);
int tmu_hal_get_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags);
int tmu_hal_modify_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg);
int tmu_hal_get_user_index_from_qid(struct tmu_hal_user_subif_abstract *subif_index, uint32_t qid);
enum tmu_hal_errorcode tmu_hal_scheduler_cfg_get(uint32_t index);
int tmu_hal_del_shaper_index(int shaper_index);
int tmu_hal_update_queue(struct net_device *netdev, char *dev_name, uint32_t q_id, uint32_t priority, uint32_t weight, uint32_t flags, QOS_Q_ADD_CFG *cfg);
int tmu_hal_update_q_weight(struct net_device *netdev, char *dev_name, uint32_t q_id, uint32_t priority, uint32_t weight, uint32_t flags, uint32_t mpe_flag, QOS_Q_ADD_CFG *cfg);
enum tmu_hal_errorcode tmu_hal_queue_param_update(const struct tmu_hal_equeue_cfg *param, uint32_t flags);
enum tmu_hal_errorcode tmu_hal_update_base_wfq_sched_weight(uint16_t sched_id);
enum tmu_hal_errorcode tmu_hal_update_ingress_sched_weight(uint16_t sched_id);

int tmu_hal_delete_mpe_ingress_queue(
		struct net_device *netdev,
		char *dev_name, 
		uint32_t q_id, 
		uint32_t priority,
		uint32_t scheduler_id, 
		uint32_t flags);

static int tmu_hal_add_q_map(uint32_t q_index, uint32_t pmac_port, cbm_queue_map_entry_t *p_qmap, uint32_t bit_mask);
static int tmu_hal_dp_egress_root_create(struct net_device *netdev);
static int tmu_hal_dp_port_resources_get(uint32_t dp_port, struct tmu_hal_dp_res_info *res_p);
static int tmu_hal_get_queue_num(struct net_device *netdev, int32_t portid, int32_t *q_num);

static int32_t tmu_hal_qos_init_cfg(void);
static int32_t tmu_hal_qos_uninit_cfg(void);
static int32_t tmu_hal_qos_set_gswr(uint32_t cmd,void *cfg);
static int32_t tmu_hal_qos_set_gswl(uint32_t cmd,void *cfg);

static int32_t tmu_hal_add_meter(PPA_QOS_METER_CFG *);
static void tmu_hal_vap_qmap_update(cbm_queue_map_entry_t *, int32_t *, dp_subif_t *);

enum tmu_hal_errorcode tmu_hal_egress_queue_cfg_set(const struct tmu_hal_equeue_cfg *param);
enum tmu_hal_errorcode tmu_hal_qoct_read(const uint32_t qid, uint32_t *wq, uint32_t *qrth, uint32_t *qocc, uint32_t *qavg);

struct tmu_hal_qos_mib g_csum_ol_mib = {0};
int32_t tmu_hal_get_qos_mib1(struct net_device *netdev, uint32_t portid, uint32_t queueid, PPA_QOS_MIB *mib, uint32_t flag);
int32_t
tmu_hal_get_qos_mib(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		struct tmu_hal_qos_stats *qos_mib,
		uint32_t flag);
int32_t
tmu_hal_modify_subif_to_port(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port_id,
		uint32_t priority,
		uint32_t weight,
		uint32_t flags);

int32_t tmu_hal_set_checksum_queue_map(uint32_t pmac_port);
int32_t tmu_hal_set_lro_queue_map(uint32_t pmac_port);
uint32_t tmu_hal_get_base_sched_id(uint32_t sbin, uint32_t sched_id);

int32_t tmu_hal_qos_set_gswr(uint32_t cmd,void *cfg)
{
	GSW_API_HANDLE gswr_handle = 0;
	GSW_QoS_queuePort_t *qos_queueport_set;
	GSW_QoS_SVLAN_ClassPCP_PortCfg_t *qos_classpcp;
	GSW_QoS_portRemarkingCfg_t *qos_remark_cfg;
	GSW_QoS_WRED_Cfg_t *qos_wred_set;
	GSW_QoS_WRED_QueueCfg_t *qos_wred_q_set;
	GSW_QoS_WRED_PortCfg_t *qos_wred_port_set;
	GSW_QoS_DSCP_ClassCfg_t *qos_dscp_class_set;
	GSW_QoS_portCfg_t *qos_portcfg_set;
	int32_t ret = 0;
	
	gswr_handle = gsw_api_kopen("/dev/switch_api/1");
	
	switch(cmd) {
		case GSW_QOS_QUEUE_PORT_SET: {	
			qos_queueport_set = (GSW_QoS_queuePort_t *)cfg;
			ret =	gsw_api_kioctl(gswr_handle, GSW_QOS_QUEUE_PORT_SET, qos_queueport_set);
			break;
		}
		case GSW_QOS_PORT_CFG_SET: {
			qos_portcfg_set = (GSW_QoS_portCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswr_handle, GSW_QOS_PORT_CFG_SET, qos_portcfg_set);
			break;
		}
		case GSW_QOS_DSCP_CLASS_SET: {
			qos_dscp_class_set = (GSW_QoS_DSCP_ClassCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswr_handle, GSW_QOS_DSCP_CLASS_SET, qos_dscp_class_set);
			break;
		}
		case GSW_QOS_PORT_REMARKING_CFG_SET: {
			qos_remark_cfg = (GSW_QoS_portRemarkingCfg_t *)cfg;
			ret = gsw_api_kioctl(gswr_handle, GSW_QOS_PORT_REMARKING_CFG_SET, qos_remark_cfg);
			break;
		}
		case GSW_QOS_SVLAN_CLASS_PCP_PORT_SET: {
			qos_classpcp = (GSW_QoS_SVLAN_ClassPCP_PortCfg_t *)cfg;
			ret = gsw_api_kioctl(gswr_handle, GSW_QOS_SVLAN_CLASS_PCP_PORT_SET, qos_classpcp);
			break;
		}
		case GSW_QOS_SVLAN_CLASS_PCP_PORT_GET: {
			qos_classpcp = (GSW_QoS_SVLAN_ClassPCP_PortCfg_t *)cfg;
			ret = gsw_api_kioctl(gswr_handle, GSW_QOS_SVLAN_CLASS_PCP_PORT_GET, qos_classpcp);
			break;
		}
		case GSW_QOS_WRED_CFG_SET: {
			qos_wred_set = (GSW_QoS_WRED_Cfg_t *)cfg;
			ret = gsw_api_kioctl(gswr_handle, GSW_QOS_WRED_CFG_SET, qos_wred_set);
			break;
		}
		case GSW_QOS_WRED_QUEUE_CFG_SET: {
			qos_wred_q_set = (GSW_QoS_WRED_QueueCfg_t *)cfg;
			ret = gsw_api_kioctl(gswr_handle, GSW_QOS_WRED_QUEUE_CFG_SET, qos_wred_q_set);
			break;
		}
		case GSW_QOS_WRED_PORT_CFG_SET: {
			qos_wred_port_set = (GSW_QoS_WRED_PortCfg_t *)cfg;
			ret = gsw_api_kioctl(gswr_handle, GSW_QOS_WRED_PORT_CFG_SET, qos_wred_port_set);
			break;
		}
		default:
			break;
	}
	if(ret < GSW_statusOk)
	pr_err(" gsw_api_kioctl returned failure \n");
	gsw_api_kclose(gswr_handle);

	return ret;
}

int32_t tmu_hal_qos_set_gswl(uint32_t cmd,void *cfg)
{
	GSW_API_HANDLE gswl_handle = 0;
	GSW_QoS_schedulerCfg_t *qos_schcfg_set;
	GSW_QoS_DSCP_ClassCfg_t *qos_dscp_class_set;
	GSW_QoS_portCfg_t *qos_portcfg_set;
	GSW_portCfg_t *portcfg_set;
	GSW_QoS_queuePort_t *qos_queueport_set;
	GSW_QoS_WRED_QueueCfg_t *qos_wred_set;
	GSW_QoS_FlowCtrlPortCfg_t *qos_flowctrl;
	GSW_register_t *regCfg;
	int32_t ret = 0;

	gswl_handle = gsw_api_kopen("/dev/switch_api/0");

	switch(cmd) {
		case GSW_QOS_SCHEDULER_CFG_SET:{
			qos_schcfg_set = (GSW_QoS_schedulerCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_SCHEDULER_CFG_SET, qos_schcfg_set);
			break;
		}
		
		case GSW_QOS_DSCP_CLASS_SET: {	
			qos_dscp_class_set = (GSW_QoS_DSCP_ClassCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_DSCP_CLASS_SET, qos_dscp_class_set);
			break;
		}
		
		case GSW_QOS_PORT_CFG_SET: {	
			qos_portcfg_set = (GSW_QoS_portCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_PORT_CFG_SET, qos_portcfg_set);
			break;
		}
		
		case GSW_QOS_QUEUE_PORT_SET: {	
			qos_queueport_set = (GSW_QoS_queuePort_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_QUEUE_PORT_SET, qos_queueport_set);
			break;
		}
		case GSW_PORT_CFG_SET: {
			portcfg_set = (GSW_portCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_PORT_CFG_SET, portcfg_set);
			break;
		}
		case GSW_QOS_WRED_QUEUE_CFG_SET: {
			qos_wred_set = (GSW_QoS_WRED_QueueCfg_t *)cfg; 
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_WRED_QUEUE_CFG_SET, qos_wred_set);
			break;
		}
		case GSW_QOS_FLOWCTRL_PORT_CFG_SET: {
			qos_flowctrl = (GSW_QoS_FlowCtrlPortCfg_t *)cfg;
			ret =	gsw_api_kioctl(gswl_handle, GSW_QOS_FLOWCTRL_PORT_CFG_SET, qos_flowctrl);
			break;
		}
		case GSW_REGISTER_SET: {
			regCfg = (GSW_register_t *)cfg;
			ret = gsw_api_kioctl(gswl_handle, GSW_REGISTER_SET, regCfg);
			break;
		}
		default:
			break;
	}

	if(ret < GSW_statusOk)
		pr_err(" gsw_api_kioctl returned failure \n");
	gsw_api_kclose(gswl_handle);
	return ret;
}

int32_t tmu_hal_qos_init_cfg(void)
{
	int32_t ret = PPA_SUCCESS,i,j,k,cnt,l,m,n,o;
	GSW_QoS_portCfg_t qos_portcfg_set;
	GSW_portCfg_t portcfg_set;
	GSW_QoS_queuePort_t qos_queueport_set;
	GSW_QoS_queuePort_t qos_queueport_set_pae;
	GSW_QoS_schedulerCfg_t qos_schcfg_set;
	GSW_QoS_WRED_Cfg_t qos_wred_set;
	GSW_QoS_WRED_QueueCfg_t qos_wred_q_set;
	GSW_QoS_WRED_PortCfg_t qos_wred_port_set;
	GSW_QoS_FlowCtrlPortCfg_t qos_flowctrl;
	GSW_register_t regCfg;

	memset(&qos_portcfg_set, 0, sizeof(GSW_QoS_portCfg_t));
	memset(&portcfg_set, 0, sizeof(GSW_portCfg_t));
	memset(&qos_queueport_set, 0, sizeof(GSW_QoS_queuePort_t));
	memset(&qos_queueport_set_pae, 0, sizeof(GSW_QoS_queuePort_t));
	memset(&qos_schcfg_set, 0, sizeof(GSW_QoS_schedulerCfg_t));
	memset(&qos_wred_set, 0, sizeof(GSW_QoS_WRED_Cfg_t));
	memset(&qos_wred_q_set, 0, sizeof(GSW_QoS_WRED_QueueCfg_t));
	memset(&qos_wred_port_set, 0, sizeof(GSW_QoS_WRED_PortCfg_t));
	memset(&qos_flowctrl, 0, sizeof(GSW_QoS_FlowCtrlPortCfg_t));
	memset(&regCfg, 0, sizeof(GSW_register_t));

	/* Port CFG Set */
	for(i=0; i<sizeof(LAN_PORT)/sizeof(int8_t); i++) {
		qos_portcfg_set.nPortId = LAN_PORT[i];
		qos_portcfg_set.eClassMode = 0;
		qos_portcfg_set.nTrafficClass = 0;
		ret = tmu_hal_qos_set_gswl(GSW_QOS_PORT_CFG_SET,&qos_portcfg_set);
	}
	
	/* QoS Port Cfg Set */
	/* For LAN Ports */
	for (i = 2; i < 6; i++) {
		qos_portcfg_set.nPortId = i;
		qos_portcfg_set.eClassMode = 1;
		qos_portcfg_set.nTrafficClass = 0;
		ret = tmu_hal_qos_set_gswr(GSW_QOS_PORT_CFG_SET, &qos_portcfg_set);
	}
	/* For DSL Ports */
	for (i = 7; i < 10; i++) {
		qos_portcfg_set.nPortId = i;
		qos_portcfg_set.eClassMode = 1;
		qos_portcfg_set.nTrafficClass = 0;
		ret = tmu_hal_qos_set_gswr(GSW_QOS_PORT_CFG_SET, &qos_portcfg_set);
	}
	/* For EthWAN Port */
	for (i = 15; i < 16; i++) {
		qos_portcfg_set.nPortId = i;
		qos_portcfg_set.eClassMode = 1;
		qos_portcfg_set.nTrafficClass = 0;
		ret = tmu_hal_qos_set_gswr(GSW_QOS_PORT_CFG_SET, &qos_portcfg_set);
	}

	k=i=cnt=j=0;
	
	/* Setting Scheduler on Lan Queues for simultaneous Lan->Lan & Wan->Lan QoS */
	for(k=0; k<5; k++) {
		for(j=0; j<4; j++) {
			qos_schcfg_set.nQueueId = (4*LAN_PORT[k] + j);
			qos_schcfg_set.eType = SCH_Q[j];
			qos_schcfg_set.nWeight = WT_Q[j];
			ret = tmu_hal_qos_set_gswl(GSW_QOS_SCHEDULER_CFG_SET,&qos_schcfg_set);
		}
	}
	/* End of Setting Wred on Lan Queues for simultaneous Lan->Lan & Wan->Lan QoS */
	for (l=0; l<=15; l++) {
		qos_queueport_set_pae.nPortId = 15; 
		qos_queueport_set_pae.nTrafficClassId = l; 
		qos_queueport_set_pae.nQueueId = 31; 
		qos_queueport_set_pae.bRedirectionBypass = 1; 
		qos_queueport_set_pae.nRedirectPortId = 15; 
		ret = tmu_hal_qos_set_gswr(GSW_QOS_QUEUE_PORT_SET,&qos_queueport_set_pae);
	}
	for (m=0; m<=15; m++) {
		qos_queueport_set_pae.nPortId = 15; 
		qos_queueport_set_pae.nTrafficClassId = m; 
		qos_queueport_set_pae.nQueueId = 30; 
		qos_queueport_set_pae.bRedirectionBypass = 0; 
		qos_queueport_set_pae.nRedirectPortId = 0; 
		ret = tmu_hal_qos_set_gswr(GSW_QOS_QUEUE_PORT_SET,&qos_queueport_set_pae);
	}

	/* Setting Flow control for Lan-> Lan QoS*/	
	qos_flowctrl.nPortId = 0;
	qos_flowctrl.nFlowCtrl_Min = 0xFF;
	qos_flowctrl.nFlowCtrl_Max = 0xFF;
	ret = tmu_hal_qos_set_gswl(GSW_QOS_FLOWCTRL_PORT_CFG_SET,&qos_flowctrl);

	qos_wred_set.eProfile = 0;
	qos_wred_set.eMode = 0;
	qos_wred_set.eThreshMode = 0;
	qos_wred_set.nRed_Min = 0x3FF;
	qos_wred_set.nRed_Max = 0x3FF;
	qos_wred_set.nYellow_Min = 0x3FF;
	qos_wred_set.nYellow_Max = 0x3FF;
	qos_wred_set.nGreen_Min = 0x3FF;
	qos_wred_set.nGreen_Max = 0x3FF;
	ret = tmu_hal_qos_set_gswr(GSW_QOS_WRED_CFG_SET, &qos_wred_set);

	for (n=0; n<32; n++) {
		qos_wred_q_set.nQueueId = n;
		qos_wred_q_set.nRed_Min = 0xFF;
		qos_wred_q_set.nRed_Max = 0xFF;
		qos_wred_q_set.nYellow_Min = 0xFF;
		qos_wred_q_set.nYellow_Max = 0xFF;
		qos_wred_q_set.nGreen_Min = 0xFF;
		qos_wred_q_set.nGreen_Max = 0xFF;
		ret = tmu_hal_qos_set_gswr(GSW_QOS_WRED_QUEUE_CFG_SET, &qos_wred_q_set);
	}
			
	for (o=0; o<16; o++) {
		qos_wred_port_set.nPortId = o;
		qos_wred_port_set.nRed_Min = 0x3FF;
		qos_wred_port_set.nRed_Max = 0x3FF;
		qos_wred_port_set.nYellow_Min = 0x3FF;
		qos_wred_port_set.nYellow_Max = 0x3FF;
		qos_wred_port_set.nGreen_Min = 0x3FF;
		qos_wred_port_set.nGreen_Max = 0x3FF;
		ret = tmu_hal_qos_set_gswr(GSW_QOS_WRED_PORT_CFG_SET, &qos_wred_port_set);
	}

	return ret;
}

int32_t tmu_hal_qos_uninit_cfg(void)
{
	int32_t ret = PPA_SUCCESS,j,k,l,m;
	GSW_QoS_schedulerCfg_t qos_schcfg_set;
	GSW_QoS_queuePort_t qos_queueport_set_pae;

	memset(&qos_schcfg_set, 0, sizeof(GSW_QoS_schedulerCfg_t));
	memset(&qos_queueport_set_pae, 0, sizeof(GSW_QoS_queuePort_t));

	/* Queue Scheduler cfg Set */
	for (j=0; j<MAX_NUMBER_OF_LAN_PORTS; j++) {
		for(k=0; k<4; k++) {
			qos_schcfg_set.nQueueId = LAN_PORT_Q[j][k];
			qos_schcfg_set.eType = 0;
			qos_schcfg_set.nWeight = 0xffff;
			ret = tmu_hal_qos_set_gswl(GSW_QOS_SCHEDULER_CFG_SET,&qos_schcfg_set);
		}
	}
	qos_queueport_set_pae.nPortId = 15; 
	qos_queueport_set_pae.nTrafficClassId = 0; 
	qos_queueport_set_pae.nQueueId = 30; 
	qos_queueport_set_pae.bRedirectionBypass = 0; 
	qos_queueport_set_pae.nRedirectPortId = 15; 
	ret = tmu_hal_qos_set_gswr(GSW_QOS_QUEUE_PORT_SET,&qos_queueport_set_pae);
		
	qos_queueport_set_pae.nPortId = 15; 
	qos_queueport_set_pae.nTrafficClassId = 0; 
	qos_queueport_set_pae.nQueueId = 30; 
	qos_queueport_set_pae.bRedirectionBypass = 1; 
	qos_queueport_set_pae.nRedirectPortId = 15; 
	ret = tmu_hal_qos_set_gswr(GSW_QOS_QUEUE_PORT_SET,&qos_queueport_set_pae);
	
	for (m=1; m<=15; m++) {
		qos_queueport_set_pae.nPortId = 15; 
		qos_queueport_set_pae.nTrafficClassId = m; 
		qos_queueport_set_pae.nQueueId = 31; 
		qos_queueport_set_pae.bRedirectionBypass = 0; 
		qos_queueport_set_pae.nRedirectPortId = 15; 
		ret = tmu_hal_qos_set_gswr(GSW_QOS_QUEUE_PORT_SET,&qos_queueport_set_pae);
	}
	for (l=1; l<=15; l++) {
		qos_queueport_set_pae.nPortId = 15; 
		qos_queueport_set_pae.nTrafficClassId = l; 
		qos_queueport_set_pae.nQueueId = 31; 
		qos_queueport_set_pae.bRedirectionBypass = 1; 
		qos_queueport_set_pae.nRedirectPortId = 15; 
		ret = tmu_hal_qos_set_gswr(GSW_QOS_QUEUE_PORT_SET,&qos_queueport_set_pae);
	}

	return ret;
}

int32_t get_qos_status( PPA_QOS_STATUS *stat)
{
	return PPA_SUCCESS;
}

enum tmu_hal_errorcode tmu_hal_wfq_threshold_update(struct tmu_hal_equeue_cfg *q_param, QOS_Q_ADD_CFG *cfg)
{
	int ret = TMU_HAL_STATUS_ERR;
	q_param->drop_threshold_green_min = cfg->drop.wred.min_th0;
	q_param->drop_threshold_green_max = cfg->drop.wred.max_th0;
	q_param->drop_threshold_yellow_min = cfg->drop.wred.min_th1;
	q_param->drop_threshold_yellow_max = cfg->drop.wred.max_th1;
	q_param->drop_probability_green = cfg->drop.wred.max_p0;
	q_param->drop_probability_yellow = cfg->drop.wred.max_p1;
        if (cfg->drop.mode == PPA_QOS_DROP_WRED)
                q_param->wred_enable = 1;
        else if (cfg->drop.mode == PPA_QOS_DROP_TAIL)
                q_param->wred_enable = 0;
	q_param->enable = 1;
	q_param->drop_threshold_red = cfg->drop.wred.drop_th1;
	ret = tmu_hal_egress_queue_cfg_set(q_param);
	return ret;
}

/*================================================================================================ */

static int32_t tmu_hal_generic_hook(PPA_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag)
{
	int32_t i=0;
	int32_t q_count = 0;	
	struct net_device *if_dev = NULL;
	uint32_t res = PPA_SUCCESS;	
	PPA_QOS_MIB_INFO *mib_info;
	char *dev_name;
 	QOS_Q_DEL_CFG *cfg;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_HIGH,"tmu_hal_generic_hook cmd 0x%x\n", cmd );
	switch (cmd)	{
	case PPA_GENERIC_HAL_INIT: {

		if((res = ppa_drv_register_cap(QOS_QUEUE, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability QOS_QUEUE,!!!\n");	
		}
		if((res = ppa_drv_register_cap(QOS_INGGRP, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability QOS_QUEUE,!!!\n");	
		}
		if((res = ppa_drv_register_cap(Q_SCH_WFQ, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability Q_SCH_WFQ,!!!\n");	
		}
		if((res = ppa_drv_register_cap(Q_SCH_SP, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability Q_SCH_SP,!!!\n");	
		}
		if((res = ppa_drv_register_cap(Q_DROP_DT, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability Q_DROP_DT,!!!\n");	
		}
		if((res = ppa_drv_register_cap(Q_DROP_WRED, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability Q_DROP_WRED,!!!\n");	
		}
		if((res = ppa_drv_register_cap(Q_DROP_RED, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability Q_DROP_RED,!!!\n");	
		}
		if((res = ppa_drv_register_cap(Q_SHAPER, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability Q_SHAPER,!!!\n");	
		}
		if((res = ppa_drv_register_cap(PORT_SHAPER, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for PORT_SHAPER,!!!\n");	
		}
		if((res = ppa_drv_register_cap(QOS_INIT, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability QOS_INIT!!!\n");	
		}
		if((res = ppa_drv_register_cap(QOS_UNINIT, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability QOS_UNINIT!!!\n");	
		}
		if((res = ppa_drv_register_cap(QOS_WMM_INIT, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability QOS_WMM_INIT!!!\n");	
		}
		if((res = ppa_drv_register_cap(QOS_WMM_UNINIT, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability QOS_WMM_UNINIT!!!\n");	
		}
		if((res = ppa_drv_register_cap(QOS_INGGRP, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability QOS_INGGRP!!!\n");	
		}
		if ((res = ppa_drv_register_cap(QOS_METER, 1, TMU_HAL)) != PPA_SUCCESS) {
			pr_err("ppa_drv_register_cap returned failure for capability QOS_METER!!!\n");
		}
		return PPA_SUCCESS;
	}
	case PPA_GENERIC_HAL_EXIT: {
		return PPA_SUCCESS;
	} 
	case PPA_GENERIC_HAL_QOS_INIT_CFG: {
		res = tmu_hal_qos_init_cfg(); 
		return res;
	} 
	case PPA_GENERIC_HAL_QOS_UNINIT_CFG: {
		res = tmu_hal_qos_uninit_cfg(); 
		return res;
	} 
	case PPA_GENERIC_HAL_GET_HAL_VERSION: {
		PPA_VERSION *v = (PPA_VERSION *)buffer;
		v->major = 0;
		v->mid = 0;
		v->minor = 1;
		return PPA_SUCCESS;
	}
	case PPA_GENERIC_HAL_GET_PPE_FW_VERSION: {
		PPA_VERSION *v=(PPA_VERSION *)buffer;
		v->major = 1;
		v->mid = 1;
		v->minor =	0 ;

		return PPA_SUCCESS;
	}	 
	case PPA_GENERIC_HAL_GET_QOS_STATUS: {
		return get_qos_status((PPA_QOS_STATUS *)buffer);
	}
	case PPA_GENERIC_HAL_GET_QOS_QUEUE_NUM: {
		PPA_QOS_COUNT_CFG *cfg = NULL;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_HIGH," ************ PPA_GENERIC_HAL_GET_QOS_QUEUE_NUM *************** \n");
		cfg = (PPA_QOS_COUNT_CFG *) buffer;
		/*printk("<%s> inf =%s\n", __FUNCTION__, cfg->ifname);
		if ( (if_dev = dev_get_by_name(&init_net, cfg->ifname)) == NULL ) {
			printk("Invalid interface name\n");
			return PPA_FAILURE;
		 }*/
		res = tmu_hal_get_queue_num(if_dev, cfg->portid, &q_count);
		cfg->num = q_count;
		return res;
	}
	case PPA_GENERIC_HAL_GET_QOS_MIB: {
		res = TMU_HAL_STATUS_OK;
		mib_info=(PPA_QOS_MIB_INFO *)buffer;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," ********* PPA_GENERIC_HAL_GET_QOS_MIB *********** \n");
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> inf =%s\n", __FUNCTION__, mib_info->ifname);
		if ( (if_dev = dev_get_by_name(&init_net, mib_info->ifname)) == NULL ) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Invalid interface name\n");
			return PPA_FAILURE;
		}

		res = tmu_hal_get_qos_mib1(if_dev, mib_info->portid, mib_info->queueid, &mib_info->mib, mib_info->flag );
		dev_put(if_dev);
		return res;
	}
	case PPA_GENERIC_HAL_QOS_ADDQUE_CFG: {
		QOS_Q_ADD_CFG *cfg;
		cfg = (QOS_Q_ADD_CFG *) buffer;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," ********* PPA_GENERIC_HAL_QOS_ADDQUE_CFG *********** \n");
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> inf =%s dev_name = %s\n", __FUNCTION__, cfg->ifname,cfg->dev_name);
		if ( (if_dev = dev_get_by_name(&init_net, cfg->ifname)) == NULL ) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Invalid interface name\n");
			pr_err("<%s> inf= %s is not up\n",__FUNCTION__, cfg->ifname);
			return PPA_FAILURE;
		 }

		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"name=%s index=%d mtu=%u\n", if_dev->name, if_dev->ifindex, if_dev->mtu);
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"InterfaceId: %d intfId_en: %d \n", cfg->intfId, cfg->intfId_en);
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," No Of Tc: %d\n", cfg->tc_no);
		for(i=0; i< cfg->tc_no; i++) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," tc[%d]: %d\n", i, cfg->tc_map[i]);
		}

		if((cfg->flags & PPA_QOS_Q_F_WFQ_UPDATE) == PPA_QOS_Q_F_WFQ_UPDATE)
		{
			tmu_hal_update_queue(
						if_dev,
						cfg->dev_name,
						cfg->q_id,
						cfg->priority,
						cfg->weight,
						cfg->flags,
						cfg);
			if(if_dev)
				dev_put(if_dev);

			return PPA_SUCCESS;
		}
		cfg->q_id = tmu_hal_add_queue(if_dev,
						cfg->dev_name, 
						cfg->tc_map, 
						cfg->tc_no, 
						cfg->intfId_en, 
						cfg->q_type +1 , 
						cfg->priority, 
						cfg->weight, 
						cfg->intfId, 
						cfg, cfg->flags);
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," <%s>: q_id returned by tmu_hal_add_queue : %d\n",__FUNCTION__, cfg->q_id);

		if(if_dev)
		dev_put(if_dev);
		return PPA_SUCCESS;
	}
	case PPA_GENERIC_HAL_QOS_MODQUE_CFG: {	/*modify queue*/
		QOS_Q_MOD_CFG *cfg;

		cfg=(QOS_Q_MOD_CFG *)buffer;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," ********* PPA_GENERIC_HAL_QOS_MODQUE_CFG *********** \n");
		/*printk("******new modification: type=%s level=%d weight=%d flag=%d\n",cfg->qtype,cfg->qlevel,cfg->weight, cfg->flags);*/
		if ( (if_dev = dev_get_by_name(&init_net, cfg->ifname)) == NULL ) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Invalid interface name\n");
			return PPA_FAILURE;
		}

		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"name=%s index=%d mtu=%u\n", if_dev->name, if_dev->ifindex, if_dev->mtu);
		 
		if(tmu_hal_modify_queue(if_dev, cfg) != TMU_HAL_STATUS_OK)
			res=PPA_FAILURE;

		if(if_dev)
			dev_put(if_dev);
		return res;

		}
	 case PPA_GENERIC_HAL_QOS_DELQUE_CFG: {
		 res = TMU_HAL_STATUS_OK;
		 cfg=(QOS_Q_DEL_CFG *)buffer;
		 TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," ********* PPA_GENERIC_HAL_QOS_DELQUE_CFG: *********** \n");
		 TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"interface =%s dev_name=%s queue id =%d flags = %d \n", cfg->ifname, cfg->dev_name, cfg->q_id, cfg->flags);
		 if((cfg->flags & PPA_F_PPPOATM) != PPA_F_PPPOATM) {
		 	if ( (if_dev = dev_get_by_name(&init_net, cfg->ifname)) == NULL ) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Invalid interface name\n");
		 	}
		 }  else
			 TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> PPPOATM Intf : %s \n", __FUNCTION__, cfg->dev_name);
		 if(if_dev != NULL) {
			 if((cfg->flags & PPA_F_PPPOATM) != PPA_F_PPPOATM)
				 dev_name = if_dev->name;
			 else
		 			 dev_name = cfg->dev_name;
		 }  else
			 dev_name = cfg->dev_name;
		 
		 TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> Priority=%d\n", __FUNCTION__, cfg->priority);

		 if((cfg->flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) {
			if(g_MPE_PortId && (mpe_hal_set_checksum_queue_map_hook_fn != NULL))
			 	tmu_hal_delete_mpe_ingress_queue( if_dev, dev_name, cfg->q_id, cfg->priority, 0, cfg->flags);
		 }
		 res = tmu_hal_delete_queue( if_dev, dev_name, cfg->q_id, cfg->priority, 0, cfg->intfId, cfg->flags);
		 if(if_dev)
			 dev_put(if_dev);
		 return res;
	}
	case PPA_GENERIC_HAL_MOD_SUBIF_PORT_CFG: {
		 QOS_MOD_SUBIF_PORT_CFG *cfg;
		 cfg= (QOS_MOD_SUBIF_PORT_CFG*)buffer; 
		 printk("mod_subif: interface=%s priority=%d weight=%d\n", cfg->ifname, cfg->priority_level, cfg->weight);
		 if ( (if_dev = dev_get_by_name(&init_net, cfg->ifname)) == NULL ) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Invalid interface name\n");
			return PPA_FAILURE;
		 }

		 res = tmu_hal_modify_subif_to_port(if_dev, if_dev->name, cfg->port_id, cfg->priority_level, cfg->weight, cfg->flags);
		 if(if_dev)
			 dev_put(if_dev);
		 return res;
	}
	case PPA_GENERIC_HAL_SET_QOS_SHAPER_CFG: {
		QOS_RATE_SHAPING_CFG *cfg ;
		cfg = (QOS_RATE_SHAPING_CFG *)buffer;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"----------- PPA_GENERIC_HAL_SET_QOS_SHAPER_CFG ------------\n");
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," pir =%d cir=%d pbs=%d cbs=%d \n",cfg->shaper.pir,cfg->shaper.cir,cfg->shaper.pbs,cfg->shaper.cbs);	
		if(cfg->flag & PPA_QOS_Q_F_WFQ_UPDATE)
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> : PPA_QOS_Q_F_WFQ_UPDATE is set: flags = %d \n",__FUNCTION__,cfg->flag);

		cfg->phys_shaperid = tmu_hal_add_shaper_index(cfg);
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Returned physical Shaper = %d\n",cfg->phys_shaperid);
		return PPA_SUCCESS;
	}
	case PPA_GENERIC_HAL_SET_QOS_RATE_SHAPING_CFG: {
		QOS_RATE_SHAPING_CFG *cfg;
		
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"-----------PPA_GENERIC_HAL_SET_QOS_RATE_SHAPING_CFG------------\n");
		cfg=(QOS_RATE_SHAPING_CFG *)buffer;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> inf =%s dev_name=%s \n", __FUNCTION__, cfg->ifname, cfg->dev_name);
		if(cfg->flag & PPA_QOS_Q_F_WFQ_UPDATE)
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"---- PPA_GENERIC_HAL_SET_QOS_RATE_SHAPING_CFG --- : PPA_QOS_Q_F_WFQ_UPDATE is set: flags = %d \n",cfg->flag);

		if ( (if_dev = dev_get_by_name(&init_net, cfg->ifname)) == NULL ) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Invalid interface name\n");
			return PPA_FAILURE;
		 }

		if((cfg->queueid == -1) && (cfg->flag & PPA_QOS_OP_F_MODIFY))
			tmu_hal_modify_port_rate_shaper(if_dev, cfg);
		else if(cfg->queueid == -1)
			tmu_hal_add_port_rate_shaper(if_dev, cfg, cfg->flag);
		else {
			tmu_hal_add_queue_rate_shaper_ex(if_dev, cfg, -1, cfg->flag);

			if(((cfg->flag & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) && (g_MPE_PortId != -1) && (mpe_hal_set_checksum_queue_map_hook_fn != NULL)) {
				tmu_hal_add_queue_rate_shaper_ex(if_dev, cfg, -1, (cfg->flag | TMU_HAL_QUEUE_SHAPER_INGRESS_MPE));
			}
		}
		if(if_dev)	
			dev_put(if_dev);
		return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_RESET_QOS_RATE_SHAPING_CFG: {
		QOS_RATE_SHAPING_CFG *cfg;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"-----------PPA_GENERIC_HAL_RESET_QOS_RATE_SHAPING_CFG------------\n");
		cfg=(QOS_RATE_SHAPING_CFG *)buffer;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> inf =%s dev_name = %s \n", __FUNCTION__, cfg->ifname, cfg->dev_name);
		if((cfg->flag & PPA_F_PPPOATM) != PPA_F_PPPOATM) {
			if ( (if_dev = dev_get_by_name(&init_net, cfg->ifname)) == NULL ) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Invalid interface name\n");
		 		}
		} else
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> PPPOATM Intf : %s \n", __FUNCTION__, cfg->dev_name);

		if(cfg->queueid == -1)	
			tmu_hal_del_port_rate_shaper(if_dev, cfg, cfg->flag);
		else {
			tmu_hal_del_queue_rate_shaper_ex(if_dev, cfg, -1, (cfg->flag | TMU_HAL_DEL_SHAPER_CFG));
			if(((cfg->flag & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) && (g_MPE_PortId != -1) && (mpe_hal_set_checksum_queue_map_hook_fn != NULL)) {
				tmu_hal_del_queue_rate_shaper_ex(if_dev, cfg, -1, (cfg->flag | TMU_HAL_DEL_SHAPER_CFG | TMU_HAL_QUEUE_SHAPER_INGRESS_MPE));
			}
		}
		
		if(if_dev)	
			dev_put(if_dev);
		return PPA_SUCCESS;

	}
	case PPA_GENERIC_HAL_GET_QOS_RATE_SHAPING_CFG: {
		QOS_RATE_SHAPING_CFG *cfg;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"-----------PPA_GENERIC_HAL_GET_QOS_RATE_SHAPING_CFG------------\n");
		cfg=(QOS_RATE_SHAPING_CFG *)buffer;
		if ( (if_dev = dev_get_by_name(&init_net, cfg->ifname)) == NULL ) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Invalid interface name\n");
			return PPA_FAILURE;
		 }
		tmu_hal_get_queue_rate_shaper_ex(if_dev, cfg, cfg->flag);
		if(if_dev)
			dev_put(if_dev);

	}
	case PPA_GENERIC_HAL_SET_QOS_METER_CFG: {
		PPA_QOS_METER_CFG *meter_cfg;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "-----------PPA_GENERIC_HAL_SET_QOS_METER_CFG------------\n");
		meter_cfg = (PPA_QOS_METER_CFG *)buffer;

		return tmu_hal_add_meter(meter_cfg);
	}
	default:
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"ppa_hal_generic_hook not support cmd 0x%x\n", cmd );
		return PPA_FAILURE;
	}

	return PPA_FAILURE;
}

int tmu_hal_get_subitf_via_ifname(char *dev_name, dp_subif_t *dp_subif)
{
	uint32_t m=0,n=0;
	int32_t ret = TMU_HAL_STATUS_ERR;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;

	for(m=0;m<=25;m++) {
		for(n=0;n< TMU_HAL_MAX_SUB_IFID_PER_PORT;n++) {
			subif_index = tmu_hal_user_sub_interface_info + (m * TMU_HAL_MAX_SUB_IFID_PER_PORT) + n;
			if(strncmp(subif_index->dp_subif.ifname,dev_name,16) == 0) {
				dp_subif->subif	= subif_index->dp_subif.subif << 8; 
				dp_subif->port_id	= subif_index->dp_subif.port_id; 
				return TMU_HAL_STATUS_OK;
			}
		}
	}
	return ret;
}

/** =================================	STATS API's ====================================== */
static uint32_t update_queue_mib(uint32_t *last, uint32_t *curr, uint32_t *accumulated)
{
	uint32_t delta;
	
	if( *curr >= *last) {
		delta = (*curr - *last);
		*accumulated += delta;
	} else {
		delta = (*curr + WRAPAROUND_32_BITS - *last);
		*accumulated += delta;
	}
	*last = *curr;
	return delta;

}



/*!
	\brief This is to get the mib couter for specified port and queue
	\param[in] portid the physical port id
	\param[in] queueid the queueid for the mib counter to get
	\param[out] mib the buffer for mib counter
	\param[in] flag reserved for future
	\return returns the queue number supported on this port.
*/
int32_t tmu_hal_get_qos_mib1(
		struct net_device *netdev,
		uint32_t portid, 
		uint32_t queueid, 
		PPA_QOS_MIB *mib, 
		uint32_t flag)
{

	uint32_t qdc[4];
	int32_t ret = TMU_HAL_STATUS_OK;
	int32_t nof_of_tmu_ports, index, mode = 0;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_dp_res_info res = {0};

	TMU_ALLOC(dp_subif_t, dp_subif);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Get MIB for QueueId:[%d] Flag:[%d]\n",
			__FUNCTION__, queueid, flag);
	if (tmu_hal_get_tmu_res_from_netdevice(netdev, (netdev ? netdev->name: NULL), 0, dp_subif, &nof_of_tmu_ports, &tmu_res, flag) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"%s:%d Error: Failed to get resources from Netdevice\n",
				__FUNCTION__, __LINE__);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}

#ifdef TMU_HAL_TEST
	dp_subif->port_id = 15;
	dp_subif->subif = 0;
#endif

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Get Queue Stats for Datapath Port [%d] and TMU Port:[%d]\n",
			__FUNCTION__,dp_subif->port_id, tmu_res->tmu_port);

	subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;

	if ((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) {
		TMU_HAL_DEBUG_MSG (TMU_DEBUG_TRACE, "<%s> For CPU Port\n", __FUNCTION__);
		tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
		
		subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
	}
	kfree(dp_subif);

	if ( (index = tmu_hal_get_user_index_from_qid(subif_index, queueid) ) == TMU_HAL_STATUS_ERR) {
		ret = TMU_HAL_STATUS_INVALID_QID;
		goto CBM_RESOURCES_CLEANUP;
	}

	cbm_counter_mode_get(0, &mode);
	if (mode == 0) {
		mib->total_rx_pkt = get_enq_counter(subif_index->user_queue[index].queue_index);
		mib->total_tx_pkt = get_deq_counter(subif_index->user_queue[index].queue_index);
	} else {
		mib->total_rx_bytes = get_enq_counter(subif_index->user_queue[index].queue_index);
		mib->total_tx_bytes = get_deq_counter(subif_index->user_queue[index].queue_index);
	}
	tmu_qdct_read(subif_index->user_queue[index].queue_index, qdc);

	mib->fast_path_total_pkt_drop_cnt = (qdc[0] + qdc[1] + qdc[2] + qdc[3]);

	tmu_hal_qoct_read(subif_index->user_queue[index].queue_index, NULL, NULL, &mib->q_occ, NULL);

CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return ret;
}

int32_t tmu_hal_get_csum_ol_mib(
		struct tmu_hal_qos_stats *csum_mib, 
		uint32_t flag)
{
	int32_t nof_of_tmu_ports = 0;
	cbm_tmu_res_t *tmu_res = NULL;
	int32_t ret = TMU_HAL_STATUS_OK;
	int32_t mode = 0;
	uint32_t delta = 0 , enq_temp = 0, deq_temp = 0, port_id = 0;

	if (cbm_dp_port_resources_get(&port_id, &nof_of_tmu_ports, &tmu_res, DP_F_CHECKSUM) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		return TMU_HAL_STATUS_ERR;
	}

/*	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> TMU Port: %d SB: %d Q: %d\n",
//			__FUNCTION__, tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);*/
	cbm_counter_mode_get(0, &mode);
/*	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enq/Deq Counter Mode is =%s \n",__FUNCTION__, (mode==1)?"Bytes":"Packet");*/

	enq_temp= get_enq_counter(tmu_res->tmu_q);
	deq_temp= get_deq_counter(tmu_res->tmu_q);
	if(mode == 0){
	
		delta = update_queue_mib(&g_csum_ol_mib.enqPkts, &enq_temp, &g_csum_ol_mib.enqPkts);
		csum_mib->enqPkts += delta;
		delta = update_queue_mib(&g_csum_ol_mib.deqPkts, &deq_temp, &g_csum_ol_mib.deqPkts);
		csum_mib->deqPkts += delta;
	} else {
		delta = update_queue_mib(&g_csum_ol_mib.enqBytes, &enq_temp, &g_csum_ol_mib.enqBytes);
		csum_mib->enqBytes += delta;
		delta = update_queue_mib(&g_csum_ol_mib.deqBytes, &deq_temp, &g_csum_ol_mib.deqBytes);
		csum_mib->deqBytes += delta;
	}

	kfree(tmu_res);
	
/*	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	*/
	return ret;

}

int32_t tmu_hal_clear_csum_ol_mib(
		uint32_t flag)
{
	int32_t nof_of_tmu_ports = 0;
	cbm_tmu_res_t *tmu_res = NULL;
	uint32_t port_id = 0;

	if (cbm_dp_port_resources_get(&port_id, &nof_of_tmu_ports, &tmu_res, DP_F_CHECKSUM) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		return TMU_HAL_STATUS_ERR;
	}
	reset_enq_counter(tmu_res->tmu_q);
	reset_deq_counter(tmu_res->tmu_q);

	memset(&g_csum_ol_mib, 0, sizeof(struct tmu_hal_qos_mib));
	kfree(tmu_res);
	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	

	return PPA_SUCCESS;
}

int32_t 
tmu_hal_get_qos_mib(
		struct net_device *netdev, 
		dp_subif_t *subif_id, 
		int32_t queueid, 
		struct tmu_hal_qos_stats *qos_mib, 
		uint32_t flag)
{
	uint32_t qdc[4];
	int32_t ret = TMU_HAL_STATUS_OK;
	int32_t index;
	uint32_t qocc, wq, qrth, qavg;
	uint32_t nof_of_tmu_ports=0;
	cbm_tmu_res_t *tmu_res=NULL;
	dp_subif_t *dp_subif = NULL;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_user_subif_abstract *port_subif_index = NULL;

	if (!netdev && !subif_id) {
/*		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR, "netdev=%p subif_id=%p\n", netdev,subif_id);*/
		return PPA_FAILURE;
	}


	if (!netdev && (subif_id->port_id >= 16)) {
/*		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR, "netdev=%p port_id=%d\n", netdev,subif_id->port_id);*/
		return PPA_FAILURE;
	}

	TMU_ALLOC(dp_subif_t, dp_subif);
	dp_subif->port_id = subif_id->port_id;
	dp_subif->subif = subif_id->subif;

	if (netdev) {
		if (dp_get_netif_subifid(netdev, NULL, NULL, 0, dp_subif, 0) != PPA_SUCCESS) {
/*			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get Subif Id\n", __FUNCTION__, __LINE__); */
			kfree(dp_subif);
			return TMU_HAL_STATUS_ERR;
		}
	} else {
		if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != 0) {
/*			_TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__); */

			kfree(dp_subif);
			return TMU_HAL_STATUS_ERR;
		}
	}

/*	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
			__FUNCTION__,dp_subif.port_id,dp_subif.subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);*/
	dp_subif->subif	= dp_subif->subif >> 8;
	if (tmu_res == NULL){
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
/*	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," Sub interface index %p\n",subif_index);*/

	if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) {
		struct tmu_hal_dp_res_info res = {0};
/*		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Adding the shaper for Directpath interface!!!\n",__FUNCTION__);*/
		tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
/*		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);*/

		subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
/*		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);*/

	}
	kfree(dp_subif);
	if((queueid != -1) && (queueid < EGRESS_QUEUE_ID_MAX)) { 
/* for individual queue
//		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Get MIB for QueueId %d\n",__FUNCTION__, queueid); */
		if( (index = tmu_hal_get_user_index_from_qid(subif_index, queueid) ) == TMU_HAL_STATUS_ERR) {
			ret = TMU_HAL_STATUS_INVALID_QID;
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Invalid Queue Id\n", __FUNCTION__, __LINE__);
			goto CBM_RESOURCES_CLEANUP;
		}

/*		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> User Queue Index =%d \n",__FUNCTION__, index);
//		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Actual Queue Index %d\n",__FUNCTION__,subif_index->user_queue[index].queue_index);*/

		qos_mib->enqPkts = get_enq_counter(subif_index->user_queue[index].queue_index);
		qos_mib->deqPkts = get_deq_counter(subif_index->user_queue[index].queue_index);
/*		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Rx: %lld TX:%lld\n",qos_mib->enqPkts,qos_mib->deqPkts);*/
		tmu_qdct_read(subif_index->user_queue[index].queue_index, qdc);
		qos_mib->dropPkts = (qdc[0] + qdc[1] + qdc[2] + qdc[3]);
		
		tmu_qoct_read(subif_index->user_queue[index].queue_index, &wq, &qrth, &qocc, &qavg);
		qos_mib->qOccPkts = qocc;
/*		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> Dropped Pkts: %lld	QoCC:%d\n",__FUNCTION__,qos_mib->dropPkts, qos_mib->qOccPkts);*/
	} else if (queueid == -1) {
/* for that netdevice*/
		int32_t i, q_count, mode;
		uint32_t enq_temp = 0, deq_temp = 0, drop_temp = 0, delta = 0;

		cbm_counter_mode_get(0, &mode);
/*		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enq/Deq Counter Mode is =%s \n",__FUNCTION__, (mode==1)?"Bytes":"Packet");
		//printk("<%s> Enq/Deq Counter Mode is =%s \n",__FUNCTION__, (mode==1)?"Bytes":"Packet");*/
		tmu_hal_get_queue_num(netdev, subif_id->port_id, &q_count);
		for(i=0; i< q_count; i++) {
			if (subif_index->user_queue[i].queue_index >= TMU_HAL_MAX_EGRESS_QUEUES) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> ERROR Wrong Queue %d\n", __FUNCTION__, subif_index->user_queue[i].queue_index);
				continue;
			}

			enq_temp= get_enq_counter(subif_index->user_queue[i].queue_index);
			if(mode == 0){
				delta = update_queue_mib(&subif_index->user_queue[i].q_mib.enqPkts, &enq_temp, &subif_index->user_queue[i].q_mib.enqPkts);
				subif_index->subif_mib.enqPkts += delta;
			} else {
				delta = update_queue_mib(&subif_index->user_queue[i].q_mib.enqBytes, &enq_temp, &subif_index->user_queue[i].q_mib.enqBytes);
				subif_index->subif_mib.enqBytes += delta;

			}

			deq_temp= get_deq_counter(subif_index->user_queue[i].queue_index);
			if(mode == 0){
				delta = update_queue_mib(&subif_index->user_queue[i].q_mib.deqPkts, &deq_temp, &subif_index->user_queue[i].q_mib.deqPkts);
				subif_index->subif_mib.deqPkts += delta;
			} else {
				delta = update_queue_mib(&subif_index->user_queue[i].q_mib.deqBytes, &deq_temp, &subif_index->user_queue[i].q_mib.deqBytes);
				subif_index->subif_mib.deqPkts += delta;
			}

			tmu_qdct_read(subif_index->user_queue[i].queue_index, qdc);
			drop_temp = (qdc[0] + qdc[1] + qdc[2] + qdc[3]);
			delta = update_queue_mib(&subif_index->user_queue[i].q_mib.dropPkts, &drop_temp, &subif_index->user_queue[i].q_mib.dropPkts);
			subif_index->subif_mib.dropPkts += delta;
			//qos_mib->dropPkts += (qdc[0] + qdc[1] + qdc[2] + qdc[3]);
		}
		if(mode == 0) {
			qos_mib->enqPkts = subif_index->subif_mib.enqPkts;
			qos_mib->deqPkts = subif_index->subif_mib.deqPkts;
/*			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Subif Pkts ===> Rx: %lld TX:%lld\n",qos_mib->enqPkts,qos_mib->deqPkts);*/
		} else {
			qos_mib->enqBytes = subif_index->subif_mib.enqBytes;
			qos_mib->deqBytes = subif_index->subif_mib.deqBytes;
/*			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Subif Bytes ====> Rx: %lld TX:%lld\n",qos_mib->enqBytes, qos_mib->deqBytes);
			//printk("Subif Bytes ====> Rx: %d TX:%d\n",qos_mib->enqBytes, qos_mib->deqBytes);*/
		}
		qos_mib->dropPkts = subif_index->subif_mib.dropPkts;
	}
/*	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> Wrong Queue Id %d\n", __FUNCTION__, queueid);*/

CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
	
/*	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	*/
	return ret;
}

int32_t 
tmu_hal_reset_qos_mib(
		struct net_device *netdev, 
		dp_subif_t *subif_id, 
		int32_t queueid, 
		uint32_t flag)
{
	uint32_t qdc[4];
	int32_t ret = TMU_HAL_STATUS_OK;
	int32_t nof_of_tmu_ports, index;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_user_subif_abstract *port_subif_index = NULL;

	if (!netdev && !subif_id) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR, "netdev=%p subif_id=%p\n", netdev,subif_id);
		return PPA_FAILURE;
	}


	if (!netdev && (subif_id->port_id >= 16)) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR, "netdev=%p port_id=%d\n", netdev,subif_id->port_id);
		return PPA_FAILURE;
	}
	TMU_ALLOC(dp_subif_t, dp_subif);
	dp_subif->port_id = subif_id->port_id;
	dp_subif->subif = subif_id->subif;
	if(tmu_hal_get_tmu_res_from_netdevice(netdev, netdev->name, 0, dp_subif, &nof_of_tmu_ports, &tmu_res, flag) != TMU_HAL_STATUS_OK){
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"%s:%d Error: Failed to get resources from Netdevice\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
			__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);
	dp_subif->subif	= dp_subif->subif >> 8;
	subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," Sub interface index %p\n",subif_index);

	if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) {
		struct tmu_hal_dp_res_info res = {0};
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Adding the shaper for Directpath interface!!!\n",__FUNCTION__);
		tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

		subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);

	}
	kfree(dp_subif);
	if((queueid != -1) && (queueid < EGRESS_QUEUE_ID_MAX)) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Get MIB for QueueId %d\n",__FUNCTION__, queueid); 
		if( (index = tmu_hal_get_user_index_from_qid(subif_index, queueid) ) == TMU_HAL_STATUS_ERR) {
			ret = TMU_HAL_STATUS_INVALID_QID;
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Invalid Queue Id\n", __FUNCTION__, __LINE__);
			goto CBM_RESOURCES_CLEANUP;
		}

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> User Queue Index =%d \n",__FUNCTION__, index);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Actual Queue Index %d\n",__FUNCTION__,subif_index->user_queue[index].queue_index);

		reset_enq_counter(subif_index->user_queue[index].queue_index);
		reset_deq_counter(subif_index->user_queue[index].queue_index);
		memset(qdc, 0, sizeof(qdc));
		tmu_qdct_read(subif_index->user_queue[index].queue_index, qdc);
		
	} else if (queueid == -1) {
		int32_t i, q_count;

		tmu_hal_get_queue_num(netdev, subif_id->port_id, &q_count);
		for(i=0; i< q_count; i++)
		{
			reset_enq_counter(subif_index->user_queue[i].queue_index);
			reset_deq_counter(subif_index->user_queue[i].queue_index);
			memset(qdc, 0, sizeof(qdc));
			tmu_qdct_read(subif_index->user_queue[i].queue_index, qdc);
		}
		memset(&subif_index->subif_mib, 0, sizeof(struct tmu_hal_subif_mib));
		for(i=0; i< TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE; i++)
			memset(&subif_index->user_queue[i].q_mib, 0, sizeof(struct tmu_hal_qos_mib));
	} else 
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> Wrong Queue Id %d\n", __FUNCTION__, queueid);
CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return ret;
}


/* ======================================================================================================= */

static int32_t tmu_hal_get_free_queue(void)
{
	int i;
	for(i=g_No_Of_TMU_Res_Queue; i<EGRESS_QUEUE_ID_MAX; i++) {
	if(tmu_hal_queue_track[i].is_enabled == 0)
		break;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Free Queue ID is %d\n",__FUNCTION__,i);
	return i;
}

static int32_t tmu_hal_get_free_dp_queue(void)
{
	int i;
	for(i= 0; i< g_No_Of_TMU_Res_Queue; i++) {
	if(tmu_hal_queue_track[i].is_enabled == 0)
		break;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Free Queue ID is %d\n",__FUNCTION__,i);
	return i;
}

STATIC int32_t tmu_hal_get_free_scheduler(void)
{
	int i;
	for(i=g_No_Of_TMU_Res_Scheduler; i<TMU_HAL_MAX_SCHEDULER; i++) {
	if(tmu_hal_sched_track[i].in_use == 0)
		break;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Free Scheduler ID is %d\n",__FUNCTION__,i);
	return i;
}

STATIC int32_t tmu_hal_get_free_port(void)
{
	int i;
	for(i=0; i<TMU_HAL_MAX_EGRESS_PORT; i++) {
	if(tmu_hal_port_track[i].is_enabled == 0)
		break;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Free Port ID is %d\n",i);
	return i;
}

STATIC int32_t tmu_hal_get_free_shaper(void)
{
	int i;
	for(i=1; i<TMU_HAL_MAX_SHAPER; i++) {
		if(tmu_hal_shaper_track[i].is_enabled == 0)
		break;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Free Shaper Index is %d\n",__FUNCTION__,i);
	return i;
}


STATIC int32_t 
tmu_hal_scheduler_free_input_get(const uint16_t idx, uint8_t *leaf)
{
	int i;
	uint8_t leaf_mask;
	if(idx >= TMU_HAL_MAX_SCHEDULER)
		return TMU_HAL_STATUS_MAX_SCHEDULERS_REACHED;

	leaf_mask = tmu_hal_sched_track[idx].leaf_mask;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Scheduler %d has leaf mask %x\n",__FUNCTION__,idx,leaf_mask);
	/* get any free leaf out of 0 to 7 */
	for (i = 0; i < 8; i++) {
		if (!((leaf_mask >> i) & 0x1)) {
			leaf_mask |= (1 << i);
			break;
		}
	}
	if (i >= 8)
		return -1;

	*leaf = i;

	return 0;
}

char *get_sb_policy(int idx)
{
	if(tmu_hal_sched_track[idx].policy == TMU_HAL_POLICY_WFQ)
		return ("WFQ");
	else if(tmu_hal_sched_track[idx].policy == TMU_HAL_POLICY_STRICT)
		return ("SP");
	else
		return ("Unknown");
}


int tmu_hal_dump_sb_info(uint32_t sb_index)
{
	struct tmu_hal_sched_track_info Sindex;


	TMU_HAL_ASSERT(sb_index >= TMU_HAL_MAX_SCHEDULER);

	Sindex = tmu_hal_sched_track[sb_index];

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\n =================\n");
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Used: %d\n", Sindex.in_use);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SBID: %d\n", sb_index);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"LVL: %d\n", Sindex.level);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"LEAF-MASK: %x\n", Sindex.leaf_mask);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Policy: %s\n", get_sb_policy(sb_index));
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SB Out Conn Type: %s\n", (Sindex.sched_out_conn == 0)?"Port":"SB");
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"OMID: %d(SB:%d)\n", Sindex.omid,(Sindex.omid >> 3));
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Next Sched ID: %d\n", Sindex.next_sched_id);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Peer Sched Id: %d\n", Sindex.peer_sched_id);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Selector: %d\n", Sindex.priority_selector);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\n =================\n");

	return TMU_HAL_STATUS_OK;

}

int32_t tmu_hal_get_subif_index_from_netdev(struct net_device *netdev, struct tmu_hal_user_subif_abstract *port_subif_index)
{
		int32_t i = 0;
	struct tmu_hal_user_subif_abstract *temp_index = NULL;

	temp_index = port_subif_index +1;
	for(i=0; i < TMU_HAL_MAX_SUB_IFID_PER_PORT; i++) {
		if(temp_index->netdev == netdev)
			break;
		temp_index = port_subif_index +1;
	}
	if(i < TMU_HAL_MAX_SUB_IFID_PER_PORT) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"subif index for net device is %d\n", i);
		return i;
	} 
	return TMU_HAL_STATUS_ERR;

}
/** ======================================================================================*/
enum tmu_hal_errorcode
tmu_hal_qoct_read(const uint32_t qid, uint32_t *wq, uint32_t *qrth,
					 uint32_t *qocc, uint32_t *qavg)
{
	uint32_t wq1, qrth1, qocc1, qavg1;

	tmu_qoct_read(qid, &wq1, &qrth1, &qocc1, &qavg1);

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"qid: %3d qocc: %4u", qid, qocc1);
	*qocc = qocc1;

	return TMU_HAL_STATUS_OK;
}


/** The tmu_hal_egress_port_enable function is used to enable or disable any
		egress port.
*/
enum tmu_hal_errorcode
tmu_hal_egress_port_enable(const uint32_t epn, bool ena)
{
	TMU_HAL_ASSERT(epn >= TMU_HAL_MAX_EGRESS_PORT);

	tmu_egress_port_enable(epn, ena);

	return TMU_HAL_STATUS_OK;

}

static void
tmu_hal_vap_qmap_update(cbm_queue_map_entry_t *q_map, int32_t *q_map_mask, dp_subif_t *dp_subif)
{
	/*
	 * Subif[0:15] has to be copied to flowid (2bits), dec (1 bit) and
	 * enc (1 bit) bits in the same order with LSB at enc bit.
	 */
	*q_map_mask = 0;
	q_map->flowid = (dp_subif->subif & 0xc) >> 2;
	q_map->dec = (dp_subif->subif & 0x2) >> 1;
	q_map->enc = dp_subif->subif & 0x1;
	q_map->mpe1 = 0;
	*q_map_mask |= CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> qmap for vap flowid:[0x%x] dec:[0x%x] enc:[0x%x]\n", __FUNCTION__, q_map->flowid, q_map->dec, q_map->enc);
	return;
}

/* The tmu_hal_update_base_wfq_sched_weight function is used to update the
 * weights in the base WFQ scheduler.
 * Inputs:
 * base_sched = Scheduler ID of the base WFQ scheduler that is connected to
 * main SP scheduler directly.
 *
 * 1. Iterate through the SBIN of base_sched
 * 2. If any SB (child SB) is connected to the base_sched, read the weight of
 *    Qs attached to those SB from tmu_hal_queue_track
 * 3. Iterate again and recalculate the weights of the SBINs on the base_sched
 *    based on weights of the queues attached to child SBs.
 * 4. Also set the weight of the queues of child SBs based on the SBIN weights.
 */
enum tmu_hal_errorcode
tmu_hal_update_base_wfq_sched_weight(uint16_t base_sched)
{
	uint16_t sched1;
	uint32_t sched_in, sched1_in;
	struct tmu_sched_blk_in_link ilink, ilink1;
	uint8_t leaf_index = 0;
	uint32_t leaf_weights[8] = {0};
	uint32_t total_weight = 0, sched_weight = 0, q_weight;
	int i, j;

	if (tmu_hal_sched_track[base_sched].in_use == 0) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d  Scheduler is not in use \n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_NOT_AVAILABLE;
	}

	for (i = 0; i < 8 ; i++) {
		sched_in = (base_sched << 3) + i;
		tmu_sched_blk_in_link_get(sched_in, &ilink);

		if (ilink.sie != 0) { // input is enabled
			if (ilink.sit == 0) { // input is QID
				total_weight += ((ilink.iwgt == 0) ? 0 : (1000 /ilink.iwgt));
				leaf_weights[leaf_index++] = ((ilink.iwgt == 0 ) ? 0:(1000 /ilink.iwgt));
			} else if (ilink.sit == 1) { //input is SBID
				sched_weight = 0;
				sched1 = ilink.qsid;
				for (j = 0; j < 8; j++) {
					sched1_in = (sched1 << 3) + j;
					tmu_sched_blk_in_link_get(sched1_in, &ilink1);
					if (ilink1.sie != 0) { //Checking directly the weights. Ignoring if there is SB connected
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> sched1_in:[%d] has weight:[%d]\n", __FUNCTION__, sched1_in, tmu_hal_queue_track[ilink1.qsid].prio_weight);
						/* Sum up weights only for WFQ queues. SP queue has prio_weight -1. */
						if (tmu_hal_queue_track[ilink1.qsid].prio_weight != -1)
							sched_weight += tmu_hal_queue_track[ilink1.qsid].prio_weight;
					}
				}
				total_weight += sched_weight;
				leaf_weights[leaf_index++] = sched_weight;
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> sched1:[%d] has total weight:[%d]\n", __FUNCTION__, sched1, sched_weight);
			}
		}
	}

	leaf_index = 0;
	for ( i = 0; i < 8; i++) {
		sched_in = (base_sched << 3) + i;
		tmu_sched_blk_in_link_get(sched_in, &ilink);

		if (ilink.sie != 0) { // input is enabled
			leaf_weights[leaf_index] = ((total_weight == 0) ? 0 : ((leaf_weights[leaf_index] * 100) / total_weight));
			if (ilink.sit == 0) {
				if (tmu_hal_queue_track[ilink.qsid].q_type == TMU_HAL_POLICY_WFQ && leaf_weights[leaf_index] == 0)
					leaf_weights[leaf_index] = 1;
			}
			ilink.iwgt = ((leaf_weights[leaf_index]==0) ? 0:(1000 / leaf_weights[leaf_index]));
			tmu_sched_blk_in_link_set(sched_in, &ilink);

			sched1 = ilink.qsid;
			if (ilink.sit == 1) {
				for (j = 0; j < 8; j++) {
					sched1_in = (sched1 << 3) + j;
					tmu_sched_blk_in_link_get(sched1_in, &ilink1);
					if (ilink1.sie != 0) { //Checking directly the weights. Ignoring if there is SB connected
						q_weight = tmu_hal_queue_track[ilink1.qsid].prio_weight;
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> WFQ sbin:[%d] ilink1.qsid:[%d] q_weight:[%d]\n", __FUNCTION__, sched1_in, ilink1.qsid, q_weight);
						/* Recalculate weights only for WFQ queues. SP queue has prio_weight -1. */
						if(q_weight != -1) {
							/* Weight calculation when the next level scheduler has all SP/WFQ Qs */
							q_weight = ((leaf_weights[leaf_index] == 0) ? 0: ((q_weight * 100) / leaf_weights[leaf_index]));
							TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> WFQ sbin:[%d] New Weight:[%d]\n", __FUNCTION__, sched1_in, q_weight);
							ilink1.iwgt = ((q_weight == 0) ? 0 : (1000 / q_weight));
							tmu_sched_blk_in_link_set(sched1_in, &ilink1);
						}
					}
				}
			}
			leaf_index++;
		}
	}

	return TMU_HAL_STATUS_OK;
}

/* The tmu_hal_update_ingress_sched_weight function is used to update the
 * weights in the base ingress scheduler for CPU and MPE port. It will allocate
 * equal weights to all the active leafs in the scheduler which is passed.
 *
 * Inputs:
 * sched_id = Scheduler ID of the base ingress scheduler
 *
 * 1. Get the leaf mask of the given sched_id
 * 2. Calcultae number of active leafs from leaf mask
 * 3. Calculate weight percentage such that every leaf has equal weights (100/number of leafs)
 * 4. Iterate through active leafs and set the new weight
 */
enum tmu_hal_errorcode tmu_hal_update_ingress_sched_weight(uint16_t sched_id)
{
	uint8_t sched_leaf_mask;
	uint8_t leaf_count;
	uint32_t sched_weight, sched_in;
	struct tmu_sched_blk_in_link ilink;
	int leaf_idx;

	if (sched_id >= TMU_HAL_MAX_SCHEDULER) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"<%s:%d> Scheduler index is not valid\n",
				__FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}

	if (tmu_hal_sched_track[sched_id].in_use == 0) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"<%s:%d> Scheduler is not in use\n",
				__FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_NOT_AVAILABLE;
	}

	leaf_count = 0;
	sched_leaf_mask = tmu_hal_sched_track[sched_id].leaf_mask;
	if (sched_leaf_mask == 0) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"<%s:%d> No active leafs on the scheduler\n",
				__FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}

	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s:%d> Scheduler:[%d] has leafmask:[%u]\n",
			__FUNCTION__, __LINE__, sched_id, sched_leaf_mask);
	/* Get number of active leafs */
	while (sched_leaf_mask != 0) {
		sched_leaf_mask = sched_leaf_mask & (sched_leaf_mask - 1);
		leaf_count++;
	}
	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s:%d> Leaf Count:[%d]\n",
			__FUNCTION__, __LINE__, leaf_count);

	if (leaf_count == 0) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"<%s:%d> No active leafs on the scheduler\n",
				__FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}

	sched_weight = 100 / leaf_count;
	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s:%d> New Scheduler Weight:[%d]\n",
			__FUNCTION__, __LINE__, sched_weight);

	for (leaf_idx = 0; leaf_idx < 8 ; leaf_idx++) {
		sched_in = (sched_id << 3) + leaf_idx;
		tmu_sched_blk_in_link_get(sched_in, &ilink);

		/* If SBIN is enabled */
		if (ilink.sie != 0) {
			ilink.iwgt = (sched_weight == 0) ? 1 : 1000/sched_weight;
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s:%d> Scheduler:[%d] with SBIN:[%u] has Weight:[%u]\n",
					__FUNCTION__, __LINE__, ilink.qsid, sched_in, sched_weight);
			tmu_sched_blk_in_link_set(sched_in, &ilink);
		}
	}

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_egress_queue_create function is used to assign the scheduler and
		egress port to an egress queue.
*/
enum tmu_hal_errorcode
tmu_hal_egress_queue_create(const struct tmu_hal_equeue_create *param)
{
	uint16_t idx, scheduler_input;
	uint8_t leaf;
	int ret;
	struct tmu_sched_blk_in_link	ilink;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"tmu_hal_egress_queue_create %d %d %d\n",
				param->index,
				param->scheduler_input,
				param->egress_port_number);

	TMU_HAL_ASSERT(param->index >= EGRESS_QUEUE_ID_MAX ||
			 param->egress_port_number >= TMU_HAL_MAX_EGRESS_PORT ||
			 param->scheduler_input >= TMU_HAL_MAX_SCHEDULER * 8);


	scheduler_input = param->scheduler_input;
	idx = param->scheduler_input >> 3;
	TMU_HAL_ASSERT(idx >= TMU_HAL_MAX_SCHEDULER);

	if (tmu_hal_sched_track[idx].in_use == 0) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d	Scheduler is not in use \n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_NOT_AVAILABLE;
	}
	leaf = scheduler_input & 7;
	if (tmu_hal_sched_track[idx].policy == TMU_HAL_POLICY_WFQ) {
		scheduler_input &= ~7;
		ret = tmu_hal_scheduler_free_input_get(idx, &leaf);
		if (ret < 0) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get Free Input\n", __FUNCTION__, __LINE__);
			return TMU_HAL_STATUS_NOT_AVAILABLE;
		}
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"got free input\n");
		scheduler_input |= (uint32_t )leaf;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>LL tmu_egress_queue_create Q:%d SB:%d P:%d Wt:%d\n",
			__FUNCTION__,
			param->index,
			param->scheduler_input,
			param->egress_port_number,
			param->iwgt);

	tmu_egress_queue_create(param->index, scheduler_input,
				param->egress_port_number);
	
	/* Configure the weight of Scheduler Input */
	tmu_sched_blk_in_link_get(param->scheduler_input, &ilink);
	ilink.iwgt = param->iwgt;
	tmu_sched_blk_in_link_set(param->scheduler_input, &ilink);

	/** Remember total number of queues for that port	*/
	tmu_hal_port_track[param->egress_port_number].no_of_queues++;

	/** Remember egress port number	*/
	tmu_hal_queue_track[param->index].epn = param->egress_port_number;
	/** Remember scheduler id to which it is connected to of that queue	*/
	tmu_hal_queue_track[param->index].connected_to_sched_id = idx;
	/** Remember scheduler id input to which it is connected to of that queue	*/
	tmu_hal_queue_track[param->index].sched_input = param->scheduler_input;
	/** mark the queue as enabled */
	tmu_hal_queue_track[param->index].is_enabled = true;
	/** Remember the queue weight  */
	tmu_hal_queue_track[param->index].prio_weight = (param->iwgt == 0) ? 0 : (1000 / param->iwgt);
	/** Remember the leaf mask of the Scheduler */
	tmu_hal_sched_track[idx].leaf_mask |= (1 << leaf) ;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Connected SB:leaf_mask = %d:%d \n",__FUNCTION__,
			tmu_hal_queue_track[param->index].connected_to_sched_id, 
			tmu_hal_sched_track[idx].leaf_mask);
	
	if (tmu_hal_sched_track[idx].policy == TMU_HAL_POLICY_WFQ) {
		ret = tmu_hal_update_base_wfq_sched_weight(tmu_hal_sched_track[idx].omid>>3);
		if(ret != TMU_HAL_STATUS_OK)
			return TMU_HAL_STATUS_ERR;
	}

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_egress_queue_get function is used to read back structural attributes
	of a queue.
*/
enum tmu_hal_errorcode 
tmu_hal_egress_queue_get(uint32_t queue_index,
		struct tmu_hal_equeue_create *out)
{
	uint32_t sbin, epn;
	struct tmu_sched_blk_in_link	ilink;

	TMU_HAL_ASSERT(queue_index >= EGRESS_QUEUE_ID_MAX);

	tmu_qsmt_read(queue_index, &sbin);
	tmu_qemt_read(queue_index, &epn);

	tmu_sched_blk_in_link_get(sbin, &ilink);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Queue %d --> SBIN: %4u weight: %4u\n", queue_index, sbin, ilink.iwgt);
	out->index = queue_index;
	out->egress_port_number = epn;
	out->scheduler_input = sbin;

	return TMU_HAL_STATUS_OK;
}


/** The tmu_hal_enable_disable_queue function is used to enable or disable any queue
	*/
enum tmu_hal_errorcode 
tmu_hal_enable_disable_queue(int index, int enable)
{
	TMU_HAL_ASSERT(index >= EGRESS_QUEUE_ID_MAX);

	tmu_equeue_enable(index, enable);
	return TMU_HAL_STATUS_OK;
}

#ifdef CBM_QUEUE_FLUSH_SUPPORT
/** The tmu_hal_enable_disable_queue function is used to enable or disable any queue
	*/
enum tmu_hal_errorcode 
tmu_hal_enable_queue_after_flush(struct net_device *netdev, char *dev_name, int32_t index, int32_t tmu_port, int32_t remap_to_qid, uint32_t flags)
{
	dp_subif_t *dp_subif = NULL;
	PPA_VCC *vcc = NULL;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enter --> \n",__FUNCTION__);

	TMU_ALLOC(dp_subif_t, dp_subif);
	ppa_br2684_get_vcc(netdev,&vcc);
	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
		if(dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
			if(tmu_hal_get_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
				kfree(dp_subif);
				return TMU_HAL_STATUS_ERR;
			}
		}
	}

	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap to Queue %d for netdev %s\n",__FUNCTION__, remap_to_qid, netdev->name);
	if(cbm_dp_q_enable(g_Inst, dp_subif->port_id, index, tmu_port, remap_to_qid, TMU_HAL_QUEUE_FLUSH_TIMEOUT, 1, 0) == PPA_FAILURE)
	{
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Flush Queue\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	udelay(20000);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <----- Exit \n",__FUNCTION__);

	kfree(dp_subif);
	return TMU_HAL_STATUS_OK;
}

enum tmu_hal_errorcode 
tmu_hal_flush_queue_if_qocc(struct net_device *netdev, char *dev_name, int32_t index,int32_t tmu_port, int32_t remap_to_qid, uint32_t flags)
{
	dp_subif_t *dp_subif = NULL;
	PPA_VCC *vcc = NULL;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enter --> \n",__FUNCTION__);

	TMU_ALLOC(dp_subif_t, dp_subif);
	/* Not checking the netdevice for ingress QoS */
	if (!(flags & PPA_QOS_Q_F_INGRESS)) {
		ppa_br2684_get_vcc(netdev,&vcc);
		if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
			if(dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
				if(tmu_hal_get_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
					kfree(dp_subif);
					return TMU_HAL_STATUS_ERR;
				}
			}
		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap to Queue %d for netdev %s\n",__FUNCTION__, remap_to_qid, netdev->name);
	}

#ifdef FLUSH_COUNT_SUPPORT
	int32_t flush_count=0;
	while(cbm_dp_q_enable(
		g_Inst,
		dp_subif->port_id,
		index, 
		tmu_port, 
		remap_to_qid, 
		TMU_HAL_QUEUE_FLUSH_TIMEOUT, 
		0, 
		(CBM_Q_F_DISABLE | CBM_Q_F_NODEQUEUE)) == PPA_FAILURE)
#else
	if(cbm_dp_q_enable(
		g_Inst,
		dp_subif->port_id,
		index, 
		tmu_port, 
		remap_to_qid, 
		TMU_HAL_QUEUE_FLUSH_TIMEOUT, 
		0, 
		(CBM_Q_F_DISABLE | CBM_Q_F_FLUSH )) == PPA_FAILURE)
#endif
	{
#ifdef FLUSH_COUNT_SUPPORT
		flush_count++;
		if (flush_count == 5 ) {
#endif
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Flush Queue\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
#ifdef FLUSH_COUNT_SUPPORT
		}
#endif
	}
	udelay(20000);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <----- Exit \n",__FUNCTION__);
	kfree(dp_subif);
	return TMU_HAL_STATUS_OK;
}

enum tmu_hal_errorcode 
tmu_hal_queue_enable_disable_if_qocc(
		struct net_device *netdev, 
		char* dev_name,
		int32_t index,
		int32_t tmu_port, 
		int32_t remap_to_qid, 
		int32_t enable,
		uint32_t flags)
{
	dp_subif_t *dp_subif = NULL;
	PPA_VCC *vcc = NULL;
	uint32_t qocc, wq, qrth, qavg;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enter --> \n",__FUNCTION__);
	
	TMU_ALLOC(dp_subif_t, dp_subif);
	ppa_br2684_get_vcc(netdev,&vcc);
	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
		if(dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
			if(tmu_hal_get_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
				kfree(dp_subif);
				return TMU_HAL_STATUS_ERR;
			}
		}
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap to Queue %d for netdev %s\n",
			__FUNCTION__, remap_to_qid, netdev->name);

	if(cbm_dp_q_enable(
		g_Inst,
		dp_subif->port_id,
		index, 
		tmu_port, 
		remap_to_qid, 
		((enable == 0)?TMU_HAL_QUEUE_FLUSH_TIMEOUT:0), 
		((enable == 0)? 0:1), 
		((enable == 0)?CBM_Q_F_DISABLE | CBM_Q_F_NODEQUEUE: CBM_Q_F_NODEQUEUE)) == PPA_FAILURE)
	{
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Flush Queue for disabling\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}

	kfree(dp_subif);
	udelay(20000);

	qocc = 0;
	tmu_qoct_read(index, &wq, &qrth, &qocc, &qavg);

	if (qocc > 0)
		msleep(300); /*sleep for 100 millisecond*/

	qocc = 0;
	tmu_qoct_read(index, &wq, &qrth, &qocc, &qavg);
	if (qocc > 0) {

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Qocc %d\n",
			__FUNCTION__, qocc);
		return TMU_HAL_STATUS_ERR;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <----- Exit \n",__FUNCTION__);
	return TMU_HAL_STATUS_OK;
}
#endif


/** The tmu_hal_egress_queue_delete function is used to delete the queue from 
	scheduler input 
	index - TMU queue index
*/
enum tmu_hal_errorcode 
tmu_hal_egress_queue_delete(uint32_t queue_index)
{
	uint8_t leaf;
	TMU_HAL_ASSERT(queue_index >= EGRESS_QUEUE_ID_MAX);


	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Queue %d\n",__FUNCTION__,queue_index);
	tmu_egress_queue_delete(queue_index);
	leaf = tmu_hal_queue_track[queue_index].sched_input & 7;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Queue sched input:leaf = %d:%d \n",
			__FUNCTION__,tmu_hal_queue_track[queue_index].sched_input, leaf);
	
	tmu_hal_sched_track[tmu_hal_queue_track[queue_index].connected_to_sched_id].leaf_mask &= ~(1<<leaf);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Connected SB:leaf_mask = %d:%d \n",
			__FUNCTION__, tmu_hal_queue_track[queue_index].connected_to_sched_id, 
			tmu_hal_sched_track[tmu_hal_queue_track[queue_index].connected_to_sched_id].leaf_mask);
	tmu_hal_port_track[tmu_hal_queue_track[queue_index].epn].no_of_queues --;

	/** Reset egress port number	*/
	tmu_hal_queue_track[queue_index].epn = EPNNULL_EGRESS_PORT_ID;
	/** Reset scheduler id to which it is connected to of that queue	*/
	tmu_hal_queue_track[queue_index].connected_to_sched_id = 0xFF;
	/** Reset scheduler input to which it is connected to of that queue	*/
	tmu_hal_queue_track[queue_index].sched_input = 1023;
	/** mark the queue as disabled */
	tmu_hal_queue_track[queue_index].is_enabled = false;


	return TMU_HAL_STATUS_OK;
}

enum tmu_hal_errorcode 
tmu_hal_safe_queue_delete(struct net_device *netdev, char *dev_name, int32_t index, int32_t tmu_port, int32_t remap_to_qid, uint32_t flags)
{

#ifdef CBM_QUEUE_FLUSH_SUPPORT
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Queue %d for netdev %s\n",__FUNCTION__, index, netdev->name);
	if(tmu_hal_flush_queue_if_qocc(netdev, dev_name, index, tmu_port, remap_to_qid, flags) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to flush qocc\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}
#endif

	if(tmu_hal_egress_queue_delete(index) == PPA_FAILURE) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d Failed to delete Queue\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}

	return TMU_HAL_STATUS_OK;
}

enum tmu_hal_errorcode 
tmu_hal_safe_queue_disable(struct net_device *netdev, char* dev_name, int32_t index, int32_t tmu_port, int32_t remap_to_qid, uint32_t flags)
{

#ifdef CBM_QUEUE_FLUSH_SUPPORT
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Queue %d for netdev %s\n",__FUNCTION__, index, netdev->name);
	if(tmu_hal_queue_enable_disable_if_qocc(netdev, dev_name, index, -1, -1, 0, flags) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR to flush qocc by disabling \n", __FUNCTION__, __LINE__);

		if(tmu_hal_flush_queue_if_qocc(netdev, dev_name, index, -1, -1, flags) != TMU_HAL_STATUS_OK) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to flush qocc\n", __FUNCTION__, __LINE__);
			return TMU_HAL_STATUS_ERR;
		}
	}
#endif

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disabling the scheduler input	%d \n",__FUNCTION__, tmu_hal_queue_track[index].sched_input);
	if(tmu_hal_egress_queue_delete(index) == PPA_FAILURE) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d Failed to delete Queue\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}


	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_scheduler_in_weight_update function is used to configure 
	the weight of a queue.
*/
enum tmu_hal_errorcode 
tmu_hal_scheduler_in_weight_update(uint32_t sbin, uint32_t weight)
{
	struct tmu_sched_blk_in_link	ilink;

	tmu_sched_blk_in_link_get(sbin, &ilink);
	ilink.iwgt = (1000 / weight);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Update weight of SBIN: %u to weight: %u\n",__FUNCTION__, sbin, ilink.iwgt);
	tmu_sched_blk_in_link_set(sbin, &ilink);
	tmu_hal_queue_track[ilink.qsid].prio_weight = weight;
	return TMU_HAL_STATUS_OK;
}

void tmu_hal_dump_egress_queue_drop_params(struct tmu_equeue_drop_params thx)
{

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Parameter:%s\n",__FUNCTION__,(thx.qe == true)?"enabled":"disabled");
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Drop Mode:%s\n",__FUNCTION__,(thx.dmod == 0)?"DT":"WRED");
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Threshold0 Max:%d Min:%d Drop probability:%d\n",__FUNCTION__,thx.math0,thx.mith0,thx.maxp0 );
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Threshold1 Max:%d Min:%d Drop probability:%d\n",__FUNCTION__,thx.math1,thx.mith1,thx.maxp1 );
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Avg Wt:%d Reservation Threshold:%d Max Threshold0:%d Max Threshold1:%d\n",
				__FUNCTION__,thx.wq,thx.qrth,thx.qtth0, thx.qtth1 );

}

enum tmu_hal_errorcode
tmu_hal_queue_param_update(const struct tmu_hal_equeue_cfg *param, uint32_t flags)
{
	struct tmu_equeue_link equeue_link = {0};
	struct tmu_equeue_drop_params thx = {0};
	uint16_t idx;
	int ret;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Get the Drop params for Q %d : Weight = %d\n",__FUNCTION__, param->index, param->weight);

	tmu_equeue_drop_params_get(param->index, &thx);
	tmu_hal_dump_egress_queue_drop_params(thx);

	tmu_equeue_link_get(param->index, &equeue_link);
	if(param->weight != 0) {
		tmu_hal_scheduler_in_weight_update(tmu_hal_queue_track[param->index].sched_input, param->weight);
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> tmu_hal_queue_track[param->index].prio_weight:[%d]\n", __FUNCTION__, tmu_hal_queue_track[param->index].prio_weight);
		idx = tmu_hal_queue_track[param->index].connected_to_sched_id;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> Scheduler Connected:[%d] Update weight on scheduler:[%d]\n", __FUNCTION__, idx, tmu_hal_sched_track[idx].omid>>3);
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> flags:[%d] Sched Policy:[%d]\n", __FUNCTION__, flags, tmu_hal_sched_track[idx].policy);
		if (flags & PPA_QOS_Q_F_INGRESS || flags & PPA_QOS_Q_F_VAP_QOS) {
			if (tmu_hal_sched_track[idx].policy == TMU_HAL_POLICY_WFQ) {
				ret = tmu_hal_update_base_wfq_sched_weight(tmu_hal_sched_track[idx].omid>>3);
				if (ret != TMU_HAL_STATUS_OK)
					return TMU_HAL_STATUS_ERR;
			}
		} else {
			ret = tmu_hal_update_base_wfq_sched_weight(tmu_hal_sched_track[idx].omid>>3);
			if (ret != TMU_HAL_STATUS_OK)
				return TMU_HAL_STATUS_ERR;
		}
	}

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_egress_queue_cfg_set function is used to configure a priority queue.
*/
enum tmu_hal_errorcode 
tmu_hal_egress_queue_cfg_set(const struct tmu_hal_equeue_cfg *param)
{
	struct tmu_equeue_link equeue_link = {0};
	struct tmu_equeue_drop_params thx = {0};

	TMU_HAL_ASSERT(param->index > EGRESS_QUEUE_ID_MAX);

	TMU_HAL_ASSERT(param->index >= EGRESS_QUEUE_ID_MAX ||
						 param->enable > 1 ||
						 param->sbin_enable >1 ||
						 param->weight > 1023 ||
						 param->wred_enable > 1 ||
						 param->avg_weight > 15 ||
						 param->drop_threshold_unassigned > 2304 ||
						 param->reservation_threshold > 2304 ||
						 param->drop_threshold_red > 2304 ||
						 param->drop_threshold_green_max > 2304 ||
						 param->drop_threshold_green_min > 2304 ||
						 param->drop_threshold_yellow_max > 2304 ||
						 param->drop_threshold_yellow_min > 2304 ||
						 param->drop_probability_green > 4095 ||
						 param->drop_probability_yellow > 4095 ||
						 param->drop_threshold_red > 2304);

	TMU_HAL_ASSERT(param->drop_threshold_green_max < param->drop_threshold_green_min);
	TMU_HAL_ASSERT(param->drop_threshold_yellow_max < param->drop_threshold_yellow_min);

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Get the Drop params for Q %d \n",__FUNCTION__, param->index);
	tmu_equeue_drop_params_get(param->index, &thx);
	tmu_hal_dump_egress_queue_drop_params(thx);

	if(param->wred_enable == 1){ 
		thx.mith0 = param->drop_threshold_green_min;
		thx.math0 = param->drop_threshold_green_max;
		thx.mith1 = param->drop_threshold_yellow_min;
		thx.math1 = param->drop_threshold_yellow_max;
		thx.maxp0 = param->drop_probability_green >> 4;
		thx.maxp1 = param->drop_probability_yellow >> 4;
		thx.dmod	= param->wred_enable;
		thx.qe	= param->enable;
		thx.wq	= param->avg_weight;
		thx.qrth	= param->reservation_threshold;
		thx.qtth0 = param->drop_threshold_unassigned;
		thx.qtth1 = param->drop_threshold_red;
	} else if(param->wred_enable == 0) {
		thx.dmod  = param->wred_enable;
		thx.qe    = param->enable;
		thx.math0 = param->drop_threshold_green_max;
		thx.math1 = param->drop_threshold_yellow_max;
		thx.qtth1 = param->drop_threshold_red;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Set the Drop Mode to %d \n",__FUNCTION__, thx.dmod);
	}
	else
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Drop Mode not supported \n",__FUNCTION__);

	tmu_equeue_drop_params_set(param->index, &thx);

	if(param->wred_enable == 1){ 
		tmu_wred_curve_max_px_set(0, param->index,
				param->drop_probability_green);
		tmu_wred_curve_max_px_set(1, param->index,
				param->drop_probability_yellow);
	}

	tmu_equeue_link_get(param->index, &equeue_link);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SBIN: %4u\n", equeue_link.sbin);

	return TMU_HAL_STATUS_OK;

}

/** The tmu_hal_egress_queue_status_get function is used to read the current status
	of an egress queue in the Packet Engine hardware module.

	- index: selects the egress queue

	- fill: number of segments, currently in the egress queue

	- fill_avg: number of bytes, average value

	- frames: number of frames, currently in the egress queue 

	 Queue configuration option: shared maximum queue size to be reported
*/
enum tmu_hal_errorcode 
tmu_hal_egress_queue_status_get(uint32_t index,
			struct tmu_hal_equeue_status *out)
{
	struct tmu_equeue_link equeue_link;
	struct tmu_sched_blk_in_link sblink;
	uint32_t wq, qrth, qocc, qavg;
	uint32_t qfm[3];


	TMU_HAL_ASSERT(index > EGRESS_QUEUE_ID_MAX);

	tmu_qoct_read(index, &wq, &qrth, &qocc, &qavg);
	tmu_qfmt_read(index, &qfm[0]);

	out->index = index;

	out->config_opt = 1;

	out->fill = qocc;

	out->fill_avg = qavg;

	out->frames = qfm[0];

	out->fill_max = out->fill;

	tmu_equeue_link_get(index, &equeue_link);
	tmu_sched_blk_in_link_get(equeue_link.sbin, &sblink);
	out->iwgt = sblink.iwgt;

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_egress_queue_path_get function is used to read back structural
	attributes along the path from a queue up to the egress port.
*/
enum tmu_hal_errorcode 
tmu_hal_egress_queue_path_get(uint32_t index,
			struct tmu_hal_equeue_path *out)
{
	struct tmu_equeue_link		qmt;
	struct tmu_sched_blk_in_link	sbit;
	struct tmu_sched_blk_out_link sbot;
	struct tmu_eport_link		 epmt;
	uint8_t lvl;


	TMU_HAL_ASSERT(index > EGRESS_QUEUE_ID_MAX);

	tmu_equeue_link_get(index, &qmt);
	tmu_sched_blk_in_link_get(qmt.sbin, &sbit);
	tmu_sched_blk_out_link_get(qmt.sbin>>3, &sbot);
	lvl = 0;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"QID		 %4u", index);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"QE		%4u", qmt.qe);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"EPN		 %4u\n", qmt.epn);
	out->qe = qmt.qe;
	out->epn = qmt.epn;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SBIN[%d] %4u", lvl, qmt.sbin);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SBID[%d] %4u", lvl, qmt.sbin >> 3);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"LEAF[%d] %4u\n", lvl, qmt.sbin &	7);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SIE [%d] %4u", lvl, sbit.sie);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SIT [%d] %4u", lvl, sbit.sit);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"IWGT[%d] %4u\n", lvl, sbit.iwgt);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"QSID[%d] %4u", lvl, sbit.qsid);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SOE [%d] %4u", lvl, sbot.soe);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"LVL [%d] %4u", lvl, sbot.lvl);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\n==============\n");

	out->info[lvl].sbid = qmt.sbin >> 3;
	out->info[lvl].leaf = qmt.sbin &	7;
	out->info[lvl].sie = sbit.sie;
	out->info[lvl].sit = sbit.sit;
	out->info[lvl].iwgt = sbit.iwgt;
	out->info[lvl].qsid = sbit.qsid;
	out->info[lvl].soe = sbot.soe;
	out->info[lvl].lvl = sbot.lvl;

	while ((sbot.v==1) && (lvl<8)) {
		lvl++;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SBID[%d] %4u", lvl, sbot.omid >> 3);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"LEAF[%d] %4u\n", lvl, sbot.omid &	7);
		out->info[lvl].sbid = sbot.omid >> 3;
		out->info[lvl].leaf = sbot.omid &	7;
		tmu_sched_blk_in_link_get(sbot.omid, &sbit);
		tmu_sched_blk_out_link_get(sbot.omid>>3, &sbot);
		out->info[lvl].sie = sbit.sie;
		out->info[lvl].sit = sbit.sit;
		out->info[lvl].iwgt = sbit.iwgt;
		out->info[lvl].qsid = sbit.qsid;
		out->info[lvl].soe = sbot.soe;
		out->info[lvl].lvl = sbot.lvl;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SIE [%d] %4u", lvl, sbit.sie);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SIT [%d] %4u", lvl, sbit.sit);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"IWGT[%d] %4u", lvl, sbit.iwgt);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"QSID[%d] %4u", lvl, sbit.qsid);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SOE [%d] %4u", lvl, sbot.soe);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"LVL [%d] %4u", lvl, sbot.lvl);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"==============\n");
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"EPN		 %4u", sbot.omid);
	tmu_egress_port_link_get(sbot.omid, &epmt);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"EPE		 %4u", epmt.epe);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SBID		%4u", epmt.sbid);

	out->lvl = lvl;
	out->omid = sbot.omid;
	out->epe = epmt.epe;
	out->sbid = epmt.sbid;
	return TMU_HAL_STATUS_OK;
}


/** The tmu_hal_scheduler_cfg_set function is used to configure a scheduler's
	parameters and scheduling policy.

	- index: selects the scheduler to be configured

		An error code is returned(TMU_HAL_STATUS_VALUE_RANGE_ERR), if
		index > TMU_HAL_MAX_SCHEDULER - 1

	 - priority_weight

		The weight parameter is provided for the _connected_ scheduler or T-CONT

		An error code is returned if priority_weight > 1023
		(TMU_HAL_STATUS_VALUE_RANGE_ERR).

	 - scheduler_policy
		Scheduler policy	
*/
enum tmu_hal_errorcode 
tmu_hal_scheduler_cfg_set(const struct tmu_hal_scheduler_cfg *param)
{
	uint32_t sbot[2];
	struct tmu_sched_blk_out_link sb;
	struct tmu_sched_blk_in_weights weights;


	TMU_HAL_ASSERT(param->index >= TMU_HAL_MAX_SCHEDULER);
	TMU_HAL_ASSERT(param->weight > 1023);

	tmu_sched_blk_out_enable(param->index, param->output_enable);

	tmu_sched_blk_out_link_get(param->index, &sb);

	/* set the WRR in the connected scheduler if applicable */
	tmu_sbot_read(param->index, &sbot[0]);
	if (sb.v == 1) {
		tmu_sched_blk_in_weights_get(sb.omid>>3, &weights);
		weights.iwgt[sb.omid&0x7] = param->weight;
		tmu_sched_blk_in_weights_set(sb.omid>>3, &weights);
	}

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_scheduler_cfg_get function is used to read back the scheduling
	policy of a scheduler in the Packet Engine hardware module.
*/
enum tmu_hal_errorcode 
tmu_hal_scheduler_cfg_get(uint32_t index)
{
	uint8_t leaf;
	uint32_t sbin;
	struct tmu_sched_blk_out_link sb;
	struct tmu_sched_blk_in_weights weights;
	struct tmu_sched_blk_in_link ilink;
	struct tmu_hal_scheduler_cfg out;

	TMU_HAL_ASSERT(index >= TMU_HAL_MAX_SCHEDULER);

	out.output_enable = tmu_is_sched_blk_out_enabled(index);
	out.index = index;

	tmu_sched_blk_out_link_get(index, &sb);

	/* get the WRR in the connected scheduler if applicable */
	if (sb.v == 1) {
		tmu_sched_blk_in_weights_get(sb.omid>>3, &weights);
		out.weight = weights.iwgt[sb.omid&0x7];
	} else {
		out.weight = 0;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler: %d SBOT Status:%d \n",__FUNCTION__, index, out.output_enable);

	for (leaf = 0; leaf < 8; leaf++) {
		sbin = (index << 3) + leaf;
		tmu_sched_blk_in_link_get(sbin, &ilink);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> SBIN: %d Status:%d \n",__FUNCTION__, sbin, ilink.sie);
	}

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_scheduler_create function is used to create a scheduler ID in the
	scheduler table.
*/
/** 
	The following parameters must be configured:
	- scheduler_id			: Managed Entity ID of the scheduler
								Return an error if this number is already in use
								(TMU_HAL_STATUS_NOT_AVAILABLE).
	- index					 : Identifies one of the available schedulers.
								Return an error, if no more scheduler is
			available (TMU_HAL_STATUS_NOT_AVAILABLE).
								Else return the selected value.
								The relationship between scheduler_id and
								index is stored in a software variable.
	- use_regular			 : This parameter selects
								if the scheduler is attached to the egress port
			identified by port_idx (use_regular==1) or to
			the input of another scheduler identified by
			connected_scheduler_index (use_regular==0).
	- port_idx				: egress port, if use_regular==1, else ignore
	- connected_scheduler_index
								: if use_regular==0
				 Scheduler index (range 0..127),
								Return an error code
			(TMU_HAL_STATUS_VALUE_RANGE_ERR),
								else continue with priority_weight
	- priority_weight		 : Priority/Weight value is used to determine both
								the leaf of the connected_scheduler_index and
								the weight of that leaf, to be set in IWGT.
								The scheduler_leaf	will then be treated as a
								hidden resource from the pool of 8 inputs per
								scheduler. SW shall keep track of already
								assigned inputs, taking into consideration that
								each input will receive a weight (0 in case of
								strict priority) and the leaf number indicates
								 the priority in case of strict priority.

	- scheduler_policy		: returns the scheduler's policy as
								TMU_HAL_POLICY_NULL: if nothing is connected to the
								scheduler's input
								TMU_HAL_POLICY_STRICT : if a single
								queue or a single scheduler is connected to the
								scheduler's input
								TMU_HAL_POLICY_WFQ : if more than one
			queue/scheduler are connected to the scheduler's
								input
*/
enum tmu_hal_errorcode 
tmu_hal_scheduler_create(const struct tmu_hal_sched_create *param)
{
	enum tmu_hal_errorcode ret = TMU_HAL_STATUS_OK;
	uint32_t omid;
	uint8_t	v;
	uint8_t leaf=0;
	uint16_t weight;
	uint8_t idx = (uint8_t)param->connected_scheduler_index;

	omid	 = param->port_idx;
	v		= 0;
	weight = 0;

	TMU_HAL_ASSERT(param->index >= TMU_HAL_MAX_SCHEDULER);
	TMU_HAL_ASSERT(param->level > TMU_HAL_MAX_SCHEDULER_LEVEL);

	/*printk("param->index:%d tmu_hal_sched_track[param->index].in_use: %d\n",param->index, tmu_hal_sched_track[param->index].in_use);*/
	if (tmu_hal_sched_track[param->index].in_use)
		return TMU_HAL_STATUS_NOT_AVAILABLE;

	if (param->use_regular == 0) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Connected Scheduler %d Policy %d\n",idx, tmu_hal_sched_track[idx].policy);
		/* hierarchical scheduling */
		TMU_HAL_ASSERT(idx >= TMU_HAL_MAX_SCHEDULER);

		if (tmu_hal_sched_track[idx].in_use == 0)
			return TMU_HAL_STATUS_NOT_AVAILABLE;
		/* check for the correct level configuration
		//TMU_HAL_ASSERT(tmu_hal_sched_track[idx].level >=
		//		 param->level);
		//TMU_HAL_ASSERT(tmu_hal_sched_track[idx].level !=
		//		 (param->level - 1));*/

		switch (param->scheduler_policy) {
		/*printk("Connected Scheduler Policy %d\n",tmu_hal_sched_track[idx].policy);*/
		case TMU_HAL_POLICY_NULL:
			/* connected scheduler is a head */
			return TMU_HAL_STATUS_NOT_AVAILABLE;
		case TMU_HAL_POLICY_STRICT:
			TMU_HAL_ASSERT(param->priority_weight > 7);

			leaf = (uint8_t)param->priority_weight;
			weight = 0;
			break;
		case TMU_HAL_POLICY_WFQ:
			TMU_HAL_ASSERT(param->priority_weight > 1023);

			if (tmu_hal_scheduler_free_input_get(
					param->connected_scheduler_index,
					&leaf) < 0)
				return TMU_HAL_STATUS_NOT_AVAILABLE;
			weight = param->priority_weight;
			break;
		default:
			TMU_HAL_ASSERT(1);
		}
		omid = (uint32_t) (
			(param->connected_scheduler_index << 3) |
			(leaf & 0x7));
		v = 1;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\n =================\n");
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SBID: %d\n", param->index);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"LVL: %d\n", param->level);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"OMID: %d\n", omid);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Policy: %d\n", param->scheduler_policy);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"v: %d\n", v);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"weight: %d\n", weight);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\n =================\n");

	tmu_sched_blk_create(param->index, param->level, omid, v, weight);

	/* mark scheduler as used*/
	tmu_hal_sched_track[param->index].in_use = true;
	/* set scheduler id*/
	tmu_hal_sched_track[param->index].id = param->index;
	/* remember the policy */
	tmu_hal_sched_track[param->index].policy = param->scheduler_policy;
	/* remember scheduler level*/
	tmu_hal_sched_track[param->index].level = param->level;
	/* remember scheduler output connection type*/
	tmu_hal_sched_track[param->index].sched_out_conn = v;
	/* remember scheduler output connection */
	tmu_hal_sched_track[param->index].omid = omid;

	/* update the leaf mask of the connected scheduler	*/
	tmu_hal_sched_track[param->connected_scheduler_index].leaf_mask |= (1 << leaf) ;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Exit -------------> \n");
	return ret;
}

/** The tmu_hal_scheduler_delete function is used to remove a scheduler from the
	scheduler hierarchy.
	- update the leaf mask of the omid of the scheduler
	- disable scheduler output
	- disable scheduler input
	- update the scheduler tracking information
*/
enum tmu_hal_errorcode 
tmu_hal_scheduler_delete(uint32_t index)
{
	uint32_t leaf, connected_scheduler_index;
	uint32_t sbin;
	struct tmu_sched_blk_in_link ilink;
	struct tmu_sched_blk_out_link olink;

	TMU_HAL_ASSERT(index >= TMU_HAL_MAX_SCHEDULER);

	if (!tmu_hal_sched_track[index].in_use)
		return TMU_HAL_STATUS_NOT_AVAILABLE;

	/*Update the leaf_mask of OMID if it is a Scheduler*/
	if(tmu_hal_sched_track[index].sched_out_conn == 1) {
		leaf = tmu_hal_sched_track[index].omid & 7;
		connected_scheduler_index = tmu_hal_sched_track[index].omid >> 3;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Connected Scheduler index %d leaf %d\n",__FUNCTION__,connected_scheduler_index, leaf);
		tmu_hal_sched_track[connected_scheduler_index].leaf_mask &=	~(1<<leaf);
	}


	/* disable scheduler output */
	olink.soe	= 0;
	olink.lvl	= 0;
	olink.omid = 0xFF;
	olink.v	= 0;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> SB omid %d V: %d\n",__FUNCTION__,olink.omid, olink.v);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disable scheduler %d output link\n",__FUNCTION__,index);
	tmu_sched_blk_out_link_set(index, &olink);

	for (leaf = 0; leaf < 8; leaf++) {
		/* disable the input */
		sbin = (index << 3) + leaf;
		ilink.sie	= 0;
		ilink.sit	= 0;
		ilink.qsid = 0xFF;
		ilink.iwgt = 0;
		/*TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disable the input link of : %d \n",__FUNCTION__, sbin);*/
		tmu_sched_blk_in_link_set(sbin, &ilink);
	}

	/* mark scheduler as unused */
	tmu_hal_sched_track[index].in_use = false;
	/* reset scheduler output connection type*/
	tmu_hal_sched_track[index].sched_out_conn = 0xFF;
	/* reset scheduler output connection */
	tmu_hal_sched_track[index].omid = 0xFF;
	/* reset the policy */
	tmu_hal_sched_track[index].policy = TMU_HAL_POLICY_NULL;
	/* reset the priority selecteor */
	tmu_hal_sched_track[index].priority_selector = 0;
	tmu_hal_sched_track[index].next_sched_id = 0;
	tmu_hal_sched_track[index].peer_sched_id = 0;
	/* reset the leaf mask */
	tmu_hal_sched_track[index].leaf_mask = 0;

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_scheduler_get function is used to read back structural attributes
	of a scheduler.
*/
enum tmu_hal_errorcode
tmu_hal_scheduler_get(uint32_t index,
		struct tmu_hal_sched_create *out)
{
	struct tmu_sched_blk_in_weights weights;
	
	TMU_HAL_ASSERT(index >= TMU_HAL_MAX_SCHEDULER);

	out->index = index;
	out->level = tmu_hal_sched_track[index].level;
	out->scheduler_id = tmu_hal_sched_track[index].id;
	out->scheduler_policy = tmu_hal_sched_track[index].policy;
	out->use_regular = tmu_hal_sched_track[index].sched_out_conn;

	out->port_idx = tmu_hal_sched_track[index].sched_out_conn == 0 ? tmu_hal_sched_track[index].omid : 0xFF;
	out->connected_scheduler_index	= tmu_hal_sched_track[index].sched_out_conn == 0 ? 0xFF : tmu_hal_sched_track[index].omid >> 3;

	tmu_sched_blk_in_weights_get(out->connected_scheduler_index, &weights);
	out->priority_weight = weights.iwgt[tmu_hal_sched_track[index].omid & 0x7];

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"POLICY		 %4u", out->scheduler_policy);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"LVL		%4u", out->level);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"OMID		%4u", tmu_hal_sched_track[index].omid);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"weight		%4u", out->priority_weight);

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_scheduler_status_get function is used to read back status variables
	of a scheduler.
*/
enum tmu_hal_errorcode 
tmu_hal_scheduler_status_get(uint32_t index,
				struct tmu_hal_scheduler_status *out)
{
	struct tmu_sched_blk_out_status stat;

	TMU_HAL_ASSERT(index >= TMU_HAL_MAX_SCHEDULER);

	tmu_sched_blk_out_status_get(index, &stat);

	out->index = index;
	out->sof	 = stat.sof;
	out->wl	= stat.wl;
	out->wqid	= stat.wqid;

	return TMU_HAL_STATUS_OK;

}


/** The tmu_hal_scheduler_in_enable_disable function is used to enable or disable
	any scheduler input.
*/
enum tmu_hal_errorcode 
tmu_hal_scheduler_in_enable_disable(uint32_t sbin, uint32_t enable)
{
	struct tmu_sched_blk_in_link ilink;

	TMU_HAL_ASSERT(sbin >= SCHEDULER_BLOCK_INPUT_ID_MAX);

	tmu_sched_blk_in_link_get(sbin, &ilink);
	ilink.sie = enable;
	tmu_sched_blk_in_link_set(sbin, &ilink);

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_scheduler_in_cfg function is used to enable a Scheduler
	Block input and enable it.
*/
enum tmu_hal_errorcode
tmu_hal_scheduler_in_cfg(uint32_t sb_input, uint32_t omid)
{
	struct tmu_sched_blk_in_link ilink;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Enable the input link of : %d \n", sb_input);
	/* enable next level input */
	ilink.sie	= 1;
	ilink.sit	= 1;
	ilink.qsid = omid;
	ilink.iwgt = 0;
	tmu_sched_blk_in_link_set(sb_input, &ilink);
	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_update_level_recursive function is used to change the scheduler	
	level recursively to the last level of connected scheduler.
	It is called from tmu_hal_sched_out_remap function.
*/
void tmu_hal_update_level_recursive(uint32_t sbid, uint32_t sbid_start, uint32_t curr_lvl, uint32_t start_level)
{
	int i,k=sbid<<3,level;
	struct tmu_sched_blk_in_link sbit;
	struct tmu_sched_blk_out_link qsid_link;
	level = curr_lvl;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Current level for sbid %u is %u\n",__FUNCTION__, sbid, curr_lvl);
	for(i=0;i<8;i++,k++) {
		if(sbid == sbid_start)
			level = start_level;
		/*traversal each leaf based on current sbid */
		tmu_sched_blk_in_link_get(k, &sbit);
		/*TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"leaf(%d) is %s. Its queue is %u\n", i, sbit.sie ? "enabled":"disabled", sbit.qsid);*/
		/*If scheduler Input is disabled, then check next sbin,ie, k+1 */
		if(sbit.sie == 0)
			continue;
		if(sbit.sit == 1) {
			/* means the input is SBID, not QID, then continue search its parent SB's queue */
			memset(&qsid_link, 0, sizeof(struct tmu_sched_blk_out_link));
			tmu_sched_blk_out_link_get(sbit.qsid,&qsid_link);
			qsid_link.lvl = level+1;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Setting the level of QSID %u to %u\n",__FUNCTION__, sbit.qsid, qsid_link.lvl);
			tmu_sched_blk_out_link_set(sbit.qsid, &qsid_link);
			/** Remeber the level of the scheduler */
			tmu_hal_sched_track[sbit.qsid].level = qsid_link.lvl;
			/*iterate all its parent sbid and update */
			tmu_hal_update_level_recursive(sbit.qsid, sbid_start, qsid_link.lvl,start_level);
			continue;
		}

	}	
}


/** The tmu_hal_sched_out_remap function is used to map the scheduler to 
	the input of any other scheduler and recursively updated the level of
	all the higher level scheduler connected to it in hierarchy.
*/
enum tmu_hal_errorcode
tmu_hal_sched_out_remap(const uint32_t sbid, const uint8_t lvl,
							const uint32_t omid, const uint8_t v,
							const uint16_t weight)
{
	struct tmu_sched_blk_out_link olink;
	struct tmu_sched_blk_in_link ilink;
	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," Remap SBID %u --> OMID %u\n ", sbid, omid);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t LVL: %u	 v: %d	weight:	%d\n", lvl, v, weight);	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\n=================\n");

	/* Klocwork fix */
	if ((omid>>3) > TMU_HAL_MAX_SCHEDULER-1)
		return -EINVAL;
	
	/* enable scheduler output */
	olink.soe	= 1;
	olink.lvl	= lvl;
	olink.omid = omid;
	olink.v	= v;
	tmu_sched_blk_out_link_set(sbid, &olink);

	/* enable next level input */
	ilink.sie	= 1;
	ilink.sit	= 1;
	ilink.qsid = sbid;
	ilink.iwgt = weight;
		
	if (v == 0) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Port Link Set: Omid:%d	 sbid: %d	\n", __FUNCTION__, omid, sbid);	
				tmu_egress_port_link_set(omid, sbid);
	
	} else {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>SB Link Set: Omid:%d	 sbid: %d	\n", __FUNCTION__, omid, sbid);	
		tmu_sched_blk_in_link_set(omid, &ilink);
		/** Remeber the next_sched_id of the omid */
		tmu_hal_sched_track[omid>>3].next_sched_id = sbid;
		//tmu_hal_sched_track[omid>>3].leaf_mask |= (1 << 0) ;
		tmu_hal_sched_track[omid>>3].leaf_mask |= (1 << (omid & 7)) ;

	}

	
	/** Remeber the level of the scheduler */
	tmu_hal_sched_track[sbid].level = lvl;
	/** Remeber the scheduler out connection of the scheduler */
	tmu_hal_sched_track[sbid].sched_out_conn = v;
	/** Remeber the omid of the scheduler */
	tmu_hal_sched_track[sbid].omid = omid ;

	/** Update the level of all the higher level schedulers */
	tmu_hal_update_level_recursive(sbid,sbid,lvl,lvl);

	return TMU_HAL_STATUS_OK;
}



/** The tmu_hal_token_bucket_shaper_create function is used to attach a Token
	Bucket Shaper (TBS) to a scheduler input and enable it.
*/
enum tmu_hal_errorcode
tmu_hal_token_bucket_shaper_create(uint32_t index, uint32_t tbs_scheduler_block_input)
{
	struct tmu_token_bucket_shaper_params tbs;

	TMU_HAL_ASSERT(index >= TMU_HAL_MAX_SHAPER);
	TMU_HAL_ASSERT(tbs_scheduler_block_input >=
						 SCHEDULER_BLOCK_INPUT_ID_MAX);

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Attach shaper %d to SBIN: %d	\n",__FUNCTION__, index, tbs_scheduler_block_input);
	tmu_token_bucket_shaper_create(index, tbs_scheduler_block_input);

	tbs.tbe0 = 1;
	tbs.tbe1 = 1;
	tbs.mod	= 0;

	tbs.pir	= TMU_TBS_PIR_DEF;
	tbs.pbs	= TMU_TBS_PBS_DEF;
	tbs.cir	= TMU_TBS_CIR_DEF;
	tbs.cbs	= TMU_TBS_CBS_DEF;

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_token_bucket_shaper_delete function is used to remove a Token
	Bucket Shaper (TBS) from a scheduler input and disable it.
*/
enum tmu_hal_errorcode
tmu_hal_token_bucket_shaper_delete(uint32_t index, uint32_t tbs_scheduler_block_input)
{

	TMU_HAL_ASSERT(index >= TMU_HAL_MAX_SHAPER);
	TMU_HAL_ASSERT(tbs_scheduler_block_input >=
						 SCHEDULER_BLOCK_INPUT_ID_MAX);

	tmu_token_bucket_shaper_delete(index, tbs_scheduler_block_input);

	return TMU_HAL_STATUS_OK;
}


/** The tmu_hal_token_bucket_shaper_get function is used to read back the assignment
	of a Token Bucket Shaper (TBS) to a scheduler input.
*/
enum tmu_hal_errorcode
tmu_hal_token_bucket_shaper_get(uint32_t index, uint32_t *tbs_scheduler_block_input)
{
	uint32_t sbin;

	TMU_HAL_ASSERT(index >= TMU_HAL_MAX_SHAPER);

	tmu_token_bucket_shaper_link_get(index, &sbin);

	*tbs_scheduler_block_input = sbin;

	return TMU_HAL_STATUS_OK;
}


/** The tmu_hal_token_bucket_shaper_cfg_set function is used to configure a Token
	Bucket Scheduler (TBS) for CIR/CBS or PIR/PBS, respectively. If only a
	single shaper shall be configured, both indexes are set to the same value.
*/
enum tmu_hal_errorcode
tmu_hal_token_bucket_shaper_cfg_set(const struct tmu_hal_token_bucket_shaper_cfg *param)
{
	struct tmu_token_bucket_shaper_params tbs;
	uint32_t pir=0, cir=0, pbs=0, cbs=0;

	TMU_HAL_ASSERT(param->index >= TMU_HAL_MAX_SHAPER);

	/* The default value 0 accepts the TMU's factory default policy */
	pir = (param->pir == 0) ? TMU_TBS_PIR_DEF : param->pir;
	/* The default value 0 accepts the TMU's factory default policy */
	pbs = (param->pbs == 0) ? TMU_TBS_PBS_DEF : param->pbs;
	/* The default value 0 accepts the TMU's factory default policy */
	cbs = (param->cbs == 0) ? TMU_TBS_CBS_DEF : param->cbs;

	/* Treat values < TMU_HAL_TBS_PIR_MIN as TMU_HAL_TBS_PIR_MIN */
	pir = pir < TMU_TBS_PIR_MIN ? TMU_TBS_PIR_MIN : pir;
	/* Treat values < TMU_HAL_TBS_CIR_MIN as TMU_HAL_TBS_CIR_MIN */
	cir = param->cir < TMU_TBS_CIR_MIN ? TMU_TBS_CIR_MIN : param->cir;

	TMU_HAL_ASSERT(pir > TMU_TBS_PIR_MAX);
	TMU_HAL_ASSERT(pbs > TMU_TBS_PBS_MAX);
	TMU_HAL_ASSERT(cir > TMU_TBS_CIR_MAX);
	TMU_HAL_ASSERT(cbs > TMU_TBS_CBS_MAX);

	tbs.tbe0 = param->enable;
	tbs.tbe1 = param->enable;
	tbs.mod	= param->mode;
	tbs.pir	= pir;
	tbs.pbs	= pbs;
	tbs.cir	= cir;
	tbs.cbs	= cbs;

	/** If pir and cir are equal then TMU should be in color blind mode */
	if(pir == cir)
		tbs.mod =0;
	/*printk("<%s> PIR and CIR is equal\n", __FUNCTION__);*/

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>PIR=%d, PBS=%d, CIR=%d, CBS=%d, MODE=%d for TBID %u\n",__FUNCTION__, tbs.pir, tbs.pbs, tbs.cir, tbs.cbs, tbs.mod, param->index);

	tmu_token_bucket_shaper_cfg_set(param->index, &tbs);

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_token_bucket_shaper_cfg_get function is used to read back the
	configuration of a Token Bucket Shaper (TBS).
*/
enum tmu_hal_errorcode
tmu_hal_token_bucket_shaper_cfg_get(uint32_t index,
				struct tmu_hal_token_bucket_shaper_cfg *out)
{
	struct tmu_token_bucket_shaper_params tbs;

	TMU_HAL_ASSERT(index >= TMU_HAL_MAX_SHAPER);

	tmu_token_bucket_shaper_cfg_get(index, &tbs);

	out->index	= index;
	out->enable	= tbs.tbe0 & tbs.tbe1;
	out->mode	= tbs.mod;
	out->pir	= tbs.pir;
	out->pbs	= tbs.pbs;
	out->cir	= tbs.cir;
	out->cbs	= tbs.cbs;

	return TMU_HAL_STATUS_OK;
}

/** ================================ HIGH LEVEL API,s ===============================	*/
int 
create_new_scheduler(int omid, int input_type, int policy, int level, int weight)
{
	uint32_t sched_id;
	struct tmu_hal_sched_create sched_new;

	sched_id = tmu_hal_get_free_scheduler();
	if(sched_id == INVALID_SCHED_ID) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"Invalid schedular Id=%d !!!!!\n", sched_id);
		return TMU_HAL_STATUS_ERR;
	}
 
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Create Scheduler for OMID %d\n",omid);
	memset(&sched_new, 0, sizeof( struct tmu_hal_sched_create));
	sched_new.index = sched_id;
	sched_new.level = level;
	
	if(input_type == 0)
		sched_new.connected_scheduler_index = omid;
	else
		sched_new.port_idx = omid;
		
	sched_new.use_regular = input_type; /*port_idx will be selected*/
	sched_new.scheduler_policy = policy;
	sched_new.priority_weight = weight;

	if(TMU_HAL_STATUS_NOT_AVAILABLE == tmu_hal_scheduler_create(&sched_new)) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"Failed to create Scheduler Block!!!!!\n");
		return TMU_HAL_STATUS_ERR;
	}
	return sched_id;

}

int tmu_hal_free_prio_scheduler_get(uint32_t parent_sched, uint32_t base_sched, uint32_t priority, uint32_t *free_prio_sched)
{
	uint32_t omid=0, i, j, k=0;
	struct tmu_hal_sched_track_info Snext;
	Snext = tmu_hal_sched_track[base_sched];
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<tmu_hal_free_prio_scheduler_get> Parent Scheduler:%d base_sched:%d \n",parent_sched,base_sched);
	if(priority > TMU_HAL_MAX_PRIORITY_LEVEL) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Priority can not be more then Max Prio Limit\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}

	i = TMU_HAL_MAX_PRIORITY_LEVEL/TMU_HAL_Q_PRIO_LEVEL; 
	j = priority/TMU_HAL_Q_PRIO_LEVEL;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"j = %d \n",j);
	do
	{
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Snext.id = %d Snext.peer_sched.id = %d\n",Snext.id, Snext.peer_sched_id);
		if(j == k) {
			*free_prio_sched = Snext.id;
			return TMU_HAL_STATUS_OK;
		}
		else {
			Snext = tmu_hal_sched_track[Snext.peer_sched_id]; 
			k++;
		}
	} while(Snext.id !=0xFF);

	if(k < j)
		omid = create_new_scheduler(parent_sched, 0, TMU_HAL_POLICY_WFQ, Snext.level, 0);
	Snext.peer_sched_id = omid;

	*free_prio_sched = omid;
		return TMU_HAL_STATUS_OK;
}

int tmu_hal_add_scheduler_level(
			uint32_t port,
			uint32_t base_sched,
			uint32_t priority_weight,
			uint32_t *new_base_sched,
			uint32_t sched_type,
			uint32_t *new_omid)
{
	uint32_t omid=0, omid1=0, old_omid;
	struct tmu_hal_sched_track_info Snext;

	Snext = tmu_hal_sched_track[base_sched];
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base Scheduler: %d \n",__FUNCTION__,Snext.id);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Create the Priority Selector level \n",__FUNCTION__);
	old_omid = Snext.omid;
	if(Snext.sched_out_conn == 1 ) {
		omid = create_new_scheduler(old_omid >> 3, 0, TMU_HAL_POLICY_STRICT, Snext.level, old_omid & 7);
		tmu_hal_sched_track[old_omid >> 3].next_sched_id = omid;
		tmu_hal_sched_track[omid].next_sched_id = Snext.id;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Configuring	Scheduler id of SB %d -->SB %d -->SB %d \n",__FUNCTION__,Snext.id, omid, Snext.omid);
	} else if(Snext.sched_out_conn == 0 ) { 
		omid = create_new_scheduler(old_omid, 1, TMU_HAL_POLICY_STRICT, Snext.level, 0);
		tmu_hal_port_track[port].input_sched_id = omid;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Configuring	Scheduler id of SB %d -->SB %d -->Port %d \n",__FUNCTION__,Snext.id, omid, Snext.omid);
	}

					
	tmu_hal_sched_track[omid].policy = Snext.policy; 
	if((Snext.policy == TMU_HAL_POLICY_WFQ) && (sched_type == TMU_HAL_POLICY_WFQ))
	{
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base Scheduler Policy : %d New Queue Policy %d \n",__FUNCTION__, Snext.policy, sched_type);
		tmu_hal_sched_out_remap(Snext.id, Snext.level+1, omid<<3, 1, 1);
	}
	else
	{
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base Scheduler Policy : %d New Queue Policy %d \n",__FUNCTION__, Snext.policy, sched_type);
		tmu_hal_sched_out_remap(Snext.id, Snext.level+1, omid<<3, 1, 0);
	}
	tmu_hal_dump_sb_info(Snext.id);

	/*create a new scheduler and connect it to the second highest priority leaf.
	/ this scheduler input will be treated as priority 8-15 for that port.*/
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Create the Priority Selector Scheduler 8-15 \n",__FUNCTION__);
	omid1 = create_new_scheduler(omid, 0, sched_type, Snext.level+1, 1);	
	tmu_hal_sched_track[omid1].policy = Snext.policy;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Priority Selector Scheduler 8-15 is : %d \n",__FUNCTION__, omid1);
	tmu_hal_sched_track[base_sched].peer_sched_id = omid1;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Configuring	Peer Scheduler config of SB %d -->SB %d \n",__FUNCTION__, Snext.id, tmu_hal_sched_track[base_sched].peer_sched_id);

	tmu_hal_dump_sb_info(omid);
	tmu_hal_dump_sb_info(omid1);
	*new_base_sched = omid;
	*new_omid = omid1;
	return TMU_HAL_STATUS_OK;

}

int tmu_hal_del_scheduler_level(
			struct net_device *netdev,
			uint32_t port, 
			uint32_t base_sched , 
			uint32_t level_sched,
			char *dev_name,
			uint32_t flags)
{
	uint32_t old_omid,sched_del, new_base_sched;
	struct tmu_hal_sched_track_info Snext;

	Snext = tmu_hal_sched_track[base_sched];
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete the Priority Selector level for port %d \n",__FUNCTION__, port);
	old_omid = Snext.omid;
	if(tmu_hal_sched_track[level_sched].priority_selector == 2) {
		new_base_sched = tmu_hal_sched_track[tmu_hal_sched_track[base_sched].next_sched_id].peer_sched_id;	
		sched_del = tmu_hal_sched_track[base_sched].next_sched_id;
	} else if(tmu_hal_sched_track[level_sched].priority_selector == 3) {
		sched_del = tmu_hal_sched_track[tmu_hal_sched_track[base_sched].next_sched_id].peer_sched_id;	
		new_base_sched = tmu_hal_sched_track[base_sched].next_sched_id;
	} else {
		return TMU_HAL_STATUS_ERR;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Disable omid %d and then Delete the Level Scheduler %d\n",__FUNCTION__,tmu_hal_sched_track[sched_del].omid, sched_del);
	tmu_sched_blk_in_enable(tmu_hal_sched_track[sched_del].omid, 0);
	tmu_hal_scheduler_delete(sched_del);

#ifdef CBM_QUEUE_FLUSH_SUPPORT
	if(tmu_hal_flush_queue_if_qocc(netdev, dev_name, -1, port, -1, flags) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to flush qocc\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}
#else
	if(Snext.sched_out_conn == 1 )
	{
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disbale OMID %d \n",__FUNCTION__,tmu_hal_sched_track[base_sched].omid);
		tmu_sched_blk_in_enable(tmu_hal_sched_track[base_sched].omid, 0);
	}

#endif
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete the base Scheduler %d\n",__FUNCTION__,base_sched);
	tmu_hal_scheduler_delete(base_sched);

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>New Base Scheduler %d\n",__FUNCTION__,new_base_sched);
	
	if(Snext.sched_out_conn == 1 ) {
		tmu_hal_sched_out_remap(new_base_sched, tmu_hal_sched_track[new_base_sched].level- 1, old_omid , 1, old_omid & 7);
		tmu_hal_sched_track[old_omid >> 3].next_sched_id = new_base_sched;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap Scheduler id of SB %d -->SB %d	\n",__FUNCTION__,new_base_sched, old_omid);
	} else if(Snext.sched_out_conn == 0 ) { 
		tmu_hal_sched_out_remap(new_base_sched, tmu_hal_sched_track[new_base_sched].level- 1, old_omid , 0, old_omid & 7);
		tmu_hal_port_track[port].input_sched_id = new_base_sched;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap Scheduler id of SB %d --> Port %d \n",__FUNCTION__,new_base_sched, old_omid);
	}
	tmu_hal_sched_track[new_base_sched].priority_selector = 0;

#ifdef	CBM_QUEUE_FLUSH_SUPPORT
	if(tmu_hal_enable_queue_after_flush(netdev, dev_name, -1, port, -1, flags) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to enable queue after queue flush\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}
#endif
	tmu_hal_dump_sb_info(new_base_sched);
	tmu_hal_dump_sb_info(base_sched);
	tmu_hal_dump_sb_info(sched_del);
	return TMU_HAL_STATUS_OK;
}

int tmu_hal_free_wfq_leaf_get(uint32_t base_sched, uint32_t *free_wfq_leaf)
{
	uint32_t i;
	struct tmu_hal_sched_track_info Snext;
	struct tmu_sched_blk_in_link ilink;

	Snext = tmu_hal_sched_track[base_sched];
	for(i=0; i<8; i++) {
		/* Get the input details of scheduler leaf */
		tmu_sched_blk_in_link_get(Snext.id << 3 | i, &ilink);
		if(ilink.sie == 0 ) /*queue/sbid is connected to this leaf*/{	
			*free_wfq_leaf = (Snext.id << 3 | i);
			return TMU_HAL_STATUS_OK;
				
		} else {
			if(ilink.sit == 1) /* input is SBID */{
				tmu_hal_free_wfq_leaf_get(ilink.qsid, free_wfq_leaf);
			}
		}
	}
	return TMU_HAL_STATUS_ERR;
}

int tmu_hal_free_wfq_scheduler_get(uint32_t port, uint32_t base_sched, uint32_t *free_wfq_sched_input)
{
	uint32_t omid, i, peer_id;
	struct tmu_sched_blk_in_link ilink;
	struct tmu_hal_sched_track_info Snext;

	Snext = tmu_hal_sched_track[base_sched];
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base_sched:%d \n",__FUNCTION__,base_sched);
	for(i=0; i<8; i++) {
		/* Get the input details of scheduler leaf */
		tmu_sched_blk_in_link_get(Snext.id << 3 | i, &ilink);
		if(ilink.sie == 0 ) /*no queue connected to this leaf */{
			*free_wfq_sched_input = (Snext.id << 3 | i);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Free WFQ Sched input = %d\n",__FUNCTION__, *free_wfq_sched_input);
			return TMU_HAL_STATUS_OK;
		} 
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Snext.id = %d	leaf = %d\n",Snext.id, i);
	}
	omid = tmu_hal_sched_track[Snext.id].omid;
	peer_id = tmu_hal_sched_track[Snext.id].peer_sched_id;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Omid = %d Peer SCH ID %d\n",__FUNCTION__, omid, tmu_hal_sched_track[Snext.id].peer_sched_id);
	if (peer_id == 0) {
		uint32_t new_s = 0, lev = 0;
		new_s = create_new_scheduler(omid >> 3, 0, TMU_HAL_POLICY_WFQ,
			(tmu_hal_sched_track[omid >> 3].level + 1), 1);
		lev = tmu_hal_sched_track[omid >> 3].level + 1;
		tmu_hal_sched_track[Snext.id].peer_sched_id = new_s;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "New peer schdular id is %d\n", new_s);
		*free_wfq_sched_input = (new_s << 3);
		return TMU_HAL_STATUS_OK;
	} else {
		for (i = 0; i < 8; i++)
		{
			/* Get the input details of scheduler leaf */
			tmu_sched_blk_in_link_get(peer_id << 3 | i, &ilink);
			if (ilink.sie == 0) //no queue connected to this leaf
			{
				*free_wfq_sched_input = (peer_id << 3 | i);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Free WFQ Sched input = %d\n",__FUNCTION__, *free_wfq_sched_input);
				return TMU_HAL_STATUS_OK;
			}
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Snext.id = %d  leaf = %d\n", peer_id, i);
		}
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Free WFQ Sched input = %d\n",__FUNCTION__, *free_wfq_sched_input);
	return TMU_HAL_STATUS_OK;
}

enum tmu_hal_errorcode 
tmu_hal_safe_queue_and_shaper_delete(struct net_device *netdev, char* dev_name, int32_t index, int32_t tmu_port, uint32_t shaper_index, int32_t remap_to_qid, uint32_t flags)
{
#ifdef CBM_QUEUE_FLUSH_SUPPORT
	if(tmu_hal_queue_enable_disable_if_qocc(netdev, dev_name, index, tmu_port, remap_to_qid, 0, flags) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR to flush qocc by disabling \n", __FUNCTION__, __LINE__);

		if(tmu_hal_flush_queue_if_qocc(netdev, dev_name, index, -1, -1, flags) != TMU_HAL_STATUS_OK) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to flush qocc\n", __FUNCTION__, __LINE__);
			return TMU_HAL_STATUS_ERR;
		}
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Queue %d for netdev %s\n",__FUNCTION__, index, netdev->name);
#endif

	if(shaper_index != 0xFF) {

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disabling the shaper index %d from scheduler input	%d \n",
				__FUNCTION__, shaper_index, tmu_hal_queue_track[index].sched_input);
		/*printk("<%s> Disabling the shaper index %d from scheduler input	%d \n",__FUNCTION__, shaper_index, tmu_hal_queue_track[index].sched_input);*/
		tmu_hal_token_bucket_shaper_delete(shaper_index, tmu_hal_queue_track[index].sched_input );

		if((flags & TMU_HAL_DEL_SHAPER_CFG) == TMU_HAL_DEL_SHAPER_CFG) {
			/* Delete the shaper instance */
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete the shaper instance %d	\n",__FUNCTION__, shaper_index);
			/*printk("<%s> Delete the shaper instance %d	\n",__FUNCTION__, shaper_index);*/
			tmu_hal_del_shaper_index(shaper_index);
			tmu_hal_queue_track[index].tb_index = 0xFF;
		}
	}
	if(tmu_hal_egress_queue_delete(index) == PPA_FAILURE) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d Failed to delete Queue\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}

	return TMU_HAL_STATUS_OK;
}
int32_t tmu_hal_shift_up_down_q(
		struct net_device *netdev,
		char *dev_name, 
		int32_t port, 
		uint32_t sched_input, 
		int32_t q_in, 
		uint32_t shaper_index,
		struct tmu_hal_user_subif_abstract *subif_index, 
		uint32_t leaf,
		uint32_t flags)
{


	cbm_queue_map_entry_t *q_map_get = NULL;
	int32_t no_entries =0;
	struct tmu_hal_equeue_create q_reconnect;
	struct tmu_equeue_link equeue_link = {0};
	struct tmu_hal_token_bucket_shaper_cfg cfg_shaper = {0};
	struct tmu_hal_equeue_cfg q_param = {0};
	int ret = TMU_HAL_STATUS_OK;
	uint32_t user_q_index=0;

	/*printk("<%s> Enter -----> Q: %d \n",__FUNCTION__,q_in);*/
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is attached to this Queue\n",__FUNCTION__,tmu_hal_queue_track[q_in].tb_index);

	if (cbm_queue_map_get(g_Inst, q_in, &no_entries, &q_map_get, 0) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_ERR(TMU_DEBUG_ERR, "CBM FAILURE: Queue %d could not be mapped properly !!\n", q_in);
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> %d No of entries is %d for queue=%d\n", __FUNCTION__,__LINE__, no_entries, q_in);

	tmu_equeue_link_get(q_in, &equeue_link);
	tmu_hal_safe_queue_and_shaper_delete(netdev, dev_name, q_in, -1, shaper_index,	-1, flags);

	memset(&q_reconnect, 0, sizeof(struct tmu_hal_equeue_create));
	q_reconnect.index = q_in;				
	q_reconnect.egress_port_number = port;
	q_reconnect.scheduler_input =	sched_input << 3 | (leaf); /*highest priority leaf */
	if((subif_index->user_queue[tmu_hal_queue_track[q_in].user_q_idx].queue_type) == TMU_HAL_POLICY_STRICT) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Strict Priority Queue\n");
		q_reconnect.iwgt = 0;
	} else if ((subif_index->user_queue[tmu_hal_queue_track[q_in].user_q_idx].queue_type) == TMU_HAL_POLICY_WFQ) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Weighted Fair Queue\n");
		q_reconnect.iwgt = 1000/(subif_index->user_queue[tmu_hal_queue_track[q_in].user_q_idx].weight);
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Reconnecting the queue %d of weight %d from scheduler input %d to scheduler input %d\n",
			__FUNCTION__, q_in, q_reconnect.iwgt, equeue_link.sbin, q_reconnect.scheduler_input );
	ret = tmu_hal_egress_queue_create(&q_reconnect);

	user_q_index = tmu_hal_queue_track[q_in].user_q_idx;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Configure Drop params for mode %s \n",
			__FUNCTION__, (subif_index->user_queue[user_q_index].ecfg.wred_enable == 0)?"DT":"WRED");
	q_param.index = q_in;
	q_param.enable = 1;
	q_param.wred_enable = subif_index->user_queue[user_q_index].ecfg.wred_enable;
	q_param.drop_threshold_green_min = subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_min;

	/* When System default Queue is moved up/down default thresholds are set to 0x24 */
	if (user_q_index == 0 && subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_max == 0 && (!(flags & PPA_QOS_Q_F_INGRESS)))
		q_param.drop_threshold_green_max = 3 * TMU_GREEN_DEFAULT_THRESHOLD;
	else
		q_param.drop_threshold_green_max = subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_max;
	q_param.drop_threshold_yellow_min = subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_min;
	if (user_q_index == 0 && subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_max == 0 && (!(flags & PPA_QOS_Q_F_INGRESS)))
		q_param.drop_threshold_yellow_max = TMU_GREEN_DEFAULT_THRESHOLD;
	else
		q_param.drop_threshold_yellow_max = subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_max;
	q_param.drop_probability_green = subif_index->user_queue[user_q_index].ecfg.drop_probability_green;
	q_param.drop_probability_yellow = subif_index->user_queue[user_q_index].ecfg.drop_probability_yellow;
	if (user_q_index == 0 && subif_index->user_queue[user_q_index].ecfg.drop_threshold_red == 0 && (!(flags & PPA_QOS_Q_F_INGRESS)))
		q_param.drop_threshold_red = 0x0;
	else
		q_param.drop_threshold_red = subif_index->user_queue[user_q_index].ecfg.drop_threshold_red;
	q_param.avg_weight = subif_index->user_queue[user_q_index].ecfg.weight;

	tmu_hal_egress_queue_cfg_set(&q_param);

	if(shaper_index != 0xFF) {
		cfg_shaper.enable = true;
		cfg_shaper.mode = tmu_hal_shaper_track[shaper_index].tb_cfg.mode;
		cfg_shaper.cir = (tmu_hal_shaper_track[shaper_index].tb_cfg.cir * 1000) / 8;
		cfg_shaper.pir = (tmu_hal_shaper_track[shaper_index].tb_cfg.pir * 1000 ) / 8;
		cfg_shaper.cbs = tmu_hal_shaper_track[shaper_index].tb_cfg.cbs; 
		cfg_shaper.pbs = tmu_hal_shaper_track[shaper_index].tb_cfg.pbs;
		cfg_shaper.index = shaper_index;

		tmu_hal_token_bucket_shaper_cfg_set(&cfg_shaper);
		/*Add the token to the scheduler input*/
		tmu_hal_token_bucket_shaper_create(shaper_index, q_reconnect.scheduler_input);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is created for scheduler input %d \n",
				__FUNCTION__,shaper_index, q_reconnect.scheduler_input);
	}

	/*tmu_hal_shaper_track[shaper_index].sb_input = q_reconnect.scheduler_input;*/
	udelay(20000);
	if (no_entries > 0) {
		int32_t j;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Q map pointer =%p\n", q_map_get);
		for (j=0; j<no_entries;j++)
			cbm_queue_map_set(g_Inst, q_in, &q_map_get[j], 0);
		no_entries = 0;
	}
	kfree(q_map_get);
	q_map_get = NULL;

	subif_index->user_queue[tmu_hal_queue_track[q_in].user_q_idx].sbin = q_reconnect.scheduler_input;
	/*printk("<%s> Exit -----> Q: %d \n",__FUNCTION__,q_in);*/
	return ret;
}


int
tmu_hal_add_new_queue(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port, 
		uint32_t policy_type, 
		uint32_t priority,	
		uint32_t prio_weight, 
		uint32_t sched_id, 
		struct tmu_hal_user_subif_abstract *subif_index,
		enum tmu_hal_errorcode *eErrCode,
		uint32_t flags )
{
	uint32_t q_index, q_new, priority_weight;
	struct tmu_hal_sched_track_info Snext;
	struct tmu_hal_equeue_create q_add;
	int ret = TMU_HAL_STATUS_OK;
	if(sched_id >= TMU_HAL_MAX_SCHEDULER)
		return TMU_HAL_STATUS_MAX_SCHEDULERS_REACHED;
	Snext = tmu_hal_sched_track[sched_id];
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> port: %d scheduler: %d policy_type: %d priority:%d priority_weight: %d\n",
			__FUNCTION__, port, sched_id, policy_type, priority, prio_weight); 

	/* Get the free queue from the pool */
	if(dp_reserve)
		q_new = tmu_hal_get_free_dp_queue();	
	else
		q_new = tmu_hal_get_free_queue();

	if(q_new == 0xFF ) {
		*eErrCode =	TMU_HAL_STATUS_MAX_QUEUES_FOR_PORT;
		ret = TMU_HAL_STATUS_ERR;
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Exceed Max queue for Port\n", __FUNCTION__, __LINE__);
		goto ADD_Q_ERR_HANDLER;
	}
	priority_weight = priority;
	if (Snext.priority_selector > 0){
		if(priority > 7)
			priority_weight = priority-8;
		else
			priority_weight = priority;
	}

	memset(&q_add, 0, sizeof(struct tmu_hal_equeue_create));
	q_add.index = q_new;				
	q_add.egress_port_number = port;

	if(policy_type == TMU_HAL_POLICY_STRICT) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Strict Priority Queue\n",__FUNCTION__);
		q_add.iwgt = 0;
	} else if (policy_type == TMU_HAL_POLICY_WFQ) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Weighted Fair Queue\n",__FUNCTION__);
		q_add.iwgt = (1000/prio_weight);
	}

	if(tmu_hal_sched_track[sched_id].policy == TMU_HAL_POLICY_WFQ) {
		uint32_t free_wfq_leaf;
		tmu_hal_free_wfq_scheduler_get(port, sched_id, &free_wfq_leaf);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Free leaf is %d\n",__FUNCTION__, free_wfq_leaf);
		q_add.scheduler_input =	free_wfq_leaf;
		q_add.iwgt = (q_add.iwgt == 0)?1:q_add.iwgt ;
		ret = tmu_hal_egress_queue_create(&q_add);
	} else {
		uint32_t new_sched;
		struct tmu_sched_blk_in_link ilink;

		/* Get the input details of scheduler leaf */
		tmu_sched_blk_in_link_get(Snext.id << 3 | priority_weight, &ilink);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>SBIN: %d is %s\n",__FUNCTION__,Snext.id << 3 | priority_weight, ilink.sie ? "enabled":"disabled");

		/* If scheduler Input is disabled	and no entry in leaf mask of Scheduler */
		if(ilink.sie == 0 ) /*no queue connected to this leaf*/ {							
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"No queue is attached to this leaf\n");

			q_add.scheduler_input =	Snext.id << 3 | priority_weight;
			ret = tmu_hal_egress_queue_create(&q_add);

		} else { /*already queue/scheduler is connected*/
			if(ilink.sit == 0) /*input is QID */ {	
				struct tmu_hal_equeue_create q_reconnect;
				uint32_t cfg_shaper, user_q_index;
				struct tmu_hal_equeue_cfg q_param = {0};
				cbm_queue_map_entry_t *q_map_get = NULL;
				int32_t no_entries =0;
				struct tmu_hal_token_bucket_shaper_cfg cfgShaper = {0};

				q_index = ilink.qsid; /* this is the QID */						

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is attached to this Queue\n",__FUNCTION__,tmu_hal_queue_track[q_index].tb_index);
				cfg_shaper = tmu_hal_queue_track[q_index].tb_index;

				/* Delete the Queue */
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Already queue %d is attached to this leaf\n",q_index);

				if (cbm_queue_map_get(g_Inst, q_index, &no_entries, &q_map_get, 0) == TMU_HAL_STATUS_ERR) {
					TMU_HAL_DEBUG_ERR(TMU_DEBUG_ERR, "CBM FAILURE: Queue %d could not be mapped properly !!\n", q_index);
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> %d No of entries is %d for queue=%d\n", 
						__FUNCTION__,__LINE__, no_entries, q_index);
				tmu_hal_safe_queue_and_shaper_delete(netdev, dev_name, q_index, -1, cfg_shaper,	-1, flags);

				/* Create a WFQ scheduler and attach to the leaf of Scheduler */
				new_sched = create_new_scheduler(Snext.id, 0, TMU_HAL_POLICY_STRICT, Snext.level+1, priority_weight);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"New Scheduler ID is %d\n",new_sched);
				tmu_hal_sched_track[new_sched].policy = TMU_HAL_POLICY_WFQ;

				/* Add the deleted queue to the WFQ scheduler */
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Reconnecting the queue %d to scheduler %d\n",q_index, new_sched);
				memset(&q_reconnect, 0, sizeof(struct tmu_hal_equeue_create));
				q_reconnect.index = q_index;				
				q_reconnect.egress_port_number = port;
				q_reconnect.scheduler_input =	new_sched << 3 | 0; /* highest priority leaf */
				q_reconnect.iwgt = (tmu_hal_queue_track[q_index].prio_weight == 0) ? 0 : 1000/tmu_hal_queue_track[q_index].prio_weight;
				ret = tmu_hal_egress_queue_create(&q_reconnect);
				if(cfg_shaper != 0xFF) {
					cfgShaper.enable = true;
					cfgShaper.mode = tmu_hal_shaper_track[cfg_shaper].tb_cfg.mode;
					cfgShaper.cir = (tmu_hal_shaper_track[cfg_shaper].tb_cfg.cir * 1000) / 8;
					cfgShaper.pir = (tmu_hal_shaper_track[cfg_shaper].tb_cfg.pir * 1000 ) / 8;
					cfgShaper.cbs = tmu_hal_shaper_track[cfg_shaper].tb_cfg.cbs;
					cfgShaper.pbs = tmu_hal_shaper_track[cfg_shaper].tb_cfg.pbs;
					cfgShaper.index = cfg_shaper;

					tmu_hal_token_bucket_shaper_cfg_set(&cfgShaper);
					//* Add the token to the scheduler input*/
					tmu_hal_token_bucket_shaper_create(cfg_shaper, q_reconnect.scheduler_input);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is created for scheduler input %d \n",
							__FUNCTION__, cfg_shaper, q_reconnect.scheduler_input);
				}

				udelay(20000);

				if(subif_index != NULL) {
					user_q_index = tmu_hal_queue_track[q_index].user_q_idx;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Configure Drop params for mode %s \n",
							__FUNCTION__, (subif_index->user_queue[user_q_index].ecfg.wred_enable == 0)?"DT":"WRED");
					q_param.index = q_index;
					q_param.enable = 1;
					q_param.wred_enable = subif_index->user_queue[user_q_index].ecfg.wred_enable;
					q_param.drop_threshold_green_min = subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_min;

					/* When System default Queue is moved up/down default thresholds are set to 0x24 */
					if(user_q_index == 0 && subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_max == 0)
						q_param.drop_threshold_green_max = 3 * TMU_GREEN_DEFAULT_THRESHOLD;
					else
						q_param.drop_threshold_green_max = subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_max;
					q_param.drop_threshold_yellow_min = subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_min;
					if(user_q_index == 0 && subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_max == 0)
						q_param.drop_threshold_yellow_max = TMU_GREEN_DEFAULT_THRESHOLD;
					else
						q_param.drop_threshold_yellow_max = subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_max;
					q_param.drop_probability_green = subif_index->user_queue[user_q_index].ecfg.drop_probability_green;
					q_param.drop_probability_yellow = subif_index->user_queue[user_q_index].ecfg.drop_probability_yellow;
					q_param.avg_weight = subif_index->user_queue[user_q_index].ecfg.weight;
					if(user_q_index == 0 && subif_index->user_queue[user_q_index].ecfg.drop_threshold_red == 0)
						q_param.drop_threshold_red = 0x0;
					else
						q_param.drop_threshold_red = subif_index->user_queue[user_q_index].ecfg.drop_threshold_red;

					tmu_hal_egress_queue_cfg_set(&q_param);
				}

				if (no_entries > 0) {
					int32_t j;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Q map pointer =%p\n", q_map_get);
					for (j=0; j<no_entries;j++)
						cbm_queue_map_set(g_Inst, q_index, &q_map_get[j], 0);
					no_entries = 0;
				}
				kfree(q_map_get);
				q_map_get = NULL;

				/* Add the new queue to the WFQ scheduler */
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Adding new queue %d to scheduler %d\n",q_new, new_sched);
				q_add.scheduler_input =  new_sched << 3 | 1; //next priority leaf
				q_add.iwgt = (prio_weight == 0)? 0 :(1000/prio_weight) ;
				ret = tmu_hal_egress_queue_create(&q_add);

			} else if(ilink.sit == 1){
				/* input is SBID
				// Already a scheduler is connected. Find the free leaf.
				// Then create a new queue to that leaf.*/
				uint8_t leaf=0;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Attached Scheduler ID is %d\n",ilink.qsid);
				if (tmu_hal_scheduler_free_input_get(
							ilink.qsid,
							&leaf) < 0) {
					*eErrCode =	TMU_HAL_STATUS_NOT_AVAILABLE;
					ret = TMU_HAL_STATUS_ERR;
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Free Schedular Not Available\n", __FUNCTION__, __LINE__);
					goto ADD_Q_ERR_HANDLER;
				}

				q_add.scheduler_input =	ilink.qsid << 3 | leaf;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Free leaf of scheduler %d is %d\n",ilink.qsid, leaf);
				ret = tmu_hal_egress_queue_create(&q_add);
			}
		}
	}

ADD_Q_ERR_HANDLER:
	if(ret == TMU_HAL_STATUS_OK){	
		tmu_hal_queue_track[q_new].q_type = policy_type;	
		return q_new;
	} else {
		q_new = 0xFF;
	}
	return ret;
}

/** This function is used to delete and then add all the scheduler block
	input from sbin to all the lower priority input of that scheduler.
	In the caller function the queue is getting deleted at this sbin.
	So this function will be called to shift all the scheduler input
	up to this scheduler input (sbin).

	netdev: net device for which the operation is performed
	tmu_port: TMU port
	subif_index: pointer to sub interface database information
	sbin: shift up all the scheduler input to this sbin

	Step 1: Run a loop for j = sbin to 6 :
	Step 2: Check if whether the ( sbin + j )input is enabled
	Step 3: If input is disabled then do nothing 
	Step 4: If Yes, then check if the scheduler input is a queue
		or another scheduler
	Step 5: If Queue then delete the queue from that input and 
		connect to sbin 
	Step 6: If the input is a scheduler then check the scheduler 
		policy.
	Step 7: If scheduler policy is Strict and sub interface index
		for physical port (is_logical == 0) then that means one 
		logical interface VLAN/VAP is connected to this. SO
		update the port scheduler input egress of the subif index
		of that logical interface.
	Step 8: If scheduler policy is a WFQ then loop for all the scheduler
		input. If the input is also a scheduler input, then it could 
		only be a logical interface that is connected to this priority
		level. So whichever logical interface that is connected to this
		priority level, update the port_sched_in_egress. 
	Step 9: Remap the scheduler with correct omid and level. 
	Step 10: Loop is completed. Goto Step1.
*/
int tmu_hal_shift_up_sbin_conn(
		struct net_device *netdev,
		char *dev_name, 
		uint32_t port, 
		struct tmu_hal_user_subif_abstract *subif_index, 
		uint32_t sbin,
		uint32_t flags)
{
	int i, j;
	uint32_t q_in;
	uint32_t sched_input = sbin >> 3;
	struct tmu_sched_blk_in_link ilink;
	int ret = TMU_HAL_STATUS_OK;

	i = sbin & 7; 
	if (i >=8)
		return TMU_HAL_STATUS_ERR;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shift up to the index = %d \n",__FUNCTION__, i);

	for(j= i; j < 7; j++) {

		tmu_sched_blk_in_link_get(sched_input << 3 | (j+1), &ilink);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>SBIN: %d is %s\n",__FUNCTION__, sched_input << 3 | (j+1), ilink.sie ? "enabled":"disabled");
		if(ilink.sie != 0) { //* input is enabled*/
			if(ilink.sit == 0) /* input is QID */ {
				uint32_t cfg_shaper;

				q_in = ilink.qsid;

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is attached to this Queue\n",__FUNCTION__,tmu_hal_queue_track[q_in].tb_index);
				cfg_shaper = tmu_hal_queue_track[q_in].tb_index;

				tmu_hal_shift_up_down_q(netdev, dev_name, port, sched_input, q_in, cfg_shaper, subif_index, j, flags);

			} else if(ilink.sit == 1){ /* input is SBID */
				/* must be one sub-interface is connected to this leaf*/
				uint32_t wt=0;	
				if(subif_index->is_logical == 0 && 
						tmu_hal_sched_track[ilink.qsid].policy == TMU_HAL_POLICY_STRICT) {
					int t;
					struct tmu_hal_user_subif_abstract *temp_index = NULL;

					temp_index = subif_index +1;
					for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id: %d port_sched_in %d port_shaper_sched %d ilink.qsid = %d\n",
								__FUNCTION__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, 
								temp_index->port_shaper_sched, ilink.qsid);
						if((temp_index->base_sched_id_egress == ilink.qsid) || temp_index->port_shaper_sched == ilink.qsid){
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id: %d ilink.qsid = %d\n",
									__FUNCTION__, temp_index->base_sched_id_egress, ilink.qsid);
							break;
						}
						temp_index += 1;
					}
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif_index->base_sched_id_egress:%d shift up scheduler: %d\n",
							__FUNCTION__,subif_index->base_sched_id_egress,sched_input);
					if(subif_index->base_sched_id_egress == sched_input)
					{
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub interface port scheduler input is changed from %d -> %d\n",
								__FUNCTION__,temp_index->port_sched_in_egress, sched_input << 3 | j); 
						temp_index->port_sched_in_egress = sched_input << 3 | j;
					}
				} else {
					/* must be many queues or one or more logical interface are 
					//	also connected to this leaf */
					struct tmu_sched_blk_in_link ilink_wfq;
					struct tmu_hal_user_subif_abstract *temp_index = NULL;
					int32_t w=0, t=0;

					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler %d has Policy : %d \n",
							__FUNCTION__, ilink.qsid, tmu_hal_sched_track[ilink.qsid].policy);

					for(w=0;w<8;w++) {
						tmu_sched_blk_in_link_get(ilink.qsid << 3 | w, &ilink_wfq);
						if(ilink_wfq.sit == 1) { /* input is SBID */
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler input %d : leaf %d \n",
									__FUNCTION__, ilink_wfq.qsid, w);
							if(tmu_hal_sched_track[ilink_wfq.qsid].policy == TMU_HAL_POLICY_STRICT) {
								temp_index = subif_index +1;
								for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
									TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id: %d port_sched_in %d port_shaper_sched %d ilink.qsid = %d\n",
											__FUNCTION__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, 
											temp_index->port_shaper_sched, ilink_wfq.qsid);
									if((temp_index->base_sched_id_egress == ilink_wfq.qsid) || 
											temp_index->port_shaper_sched == ilink_wfq.qsid) {
										TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id: %d port_sched_in %d ilink.qsid = %d\n",
												__FUNCTION__, temp_index->base_sched_id_egress, 
												temp_index->port_sched_in_egress, ilink_wfq.qsid);
										wt = (subif_index->subif_weight==0)?1:subif_index->subif_weight;
										break;
									}
									temp_index += 1;
								}
								TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif_index->base_sched_id_egress:%d sched_input: %d",
										__FUNCTION__,subif_index->base_sched_id_egress,sched_input);
								if(subif_index->base_sched_id_egress == sched_input) {
									TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub if %d port scheduler input is changed from %d -> %d\n",
											__FUNCTION__, t, temp_index->port_sched_in_egress, sched_input << 3 | (j)); 
									temp_index->port_sched_in_egress = sched_input << 3 | j;
								}
							}
						}
					}
				}

				wt = (subif_index->subif_weight==0)?1:subif_index->subif_weight;
				tmu_hal_sched_out_remap(ilink.qsid, tmu_hal_sched_track[sched_input].level +1,sched_input << 3 | j, 1, wt);

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Disable the input link of : %d \n", sched_input << 3 | (j+1) );
				tmu_hal_scheduler_in_enable_disable(sched_input << 3 | (j+1) , 0);
				tmu_hal_sched_track[sched_input].leaf_mask &= ~(1<< (j+1) );
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Updated leaf mask of SB: %d : %d \n", sched_input,tmu_hal_sched_track[sched_input].leaf_mask);

			} // input is SBID */
		} /* input is enabled */
	} /* for loop */
	return ret;

}

/** This function is used to delete and then add all the scheduler block
	input from sbin to all the lower priority input of that scheduler.
	In the caller function the queue is getting deleted at this sbin.
	So this function will be called to shift all the scheduler input
	down from this scheduler input (sbin).

	netdev: net device for which the operation is performed
	tmu_port: TMU port
	subif_index: pointer to sub interface database information
	sb_idx: shift down all the scheduler input from this sbin

	Step 1: Run a loop for j = sbin to 6 :
	Step 2: Check if whether the ( sbin + j )input is enabled
	Step 3: If input is disabled then do nothing 
	Step 4: If Yes, then check if the scheduler input is a queue
		or another scheduler
	Step 5: If Queue then delete the queue from that input and 
		connect to sbin 
	Step 6: If the input is a scheduler then check the scheduler 
		policy.
	Step 7: If scheduler policy is Strict and sub interface index
		for physical port (is_logical == 0) then that means one 
		logical interface VLAN/VAP is connected to this. SO
		update the port scheduler input egress of the subif index
		of that logical interface.
	Step 8: If scheduler policy is a WFQ then loop for all the scheduler
		input. If the input is also a scheduler input, then it could 
		only be a logical interface that is connected to this priority
		level. So whichever logical interface that is connected to this
		priority level, update the port_sched_in_egress. 
	Step 9: Remap the scheduler with correct omid and level. 
	Step 10: Loop is completed. Goto Step1.
*/
int tmu_hal_shift_down_sbin_conn(
		struct net_device *netdev,
		char *dev_name, 
		uint32_t port, 
		struct tmu_hal_user_subif_abstract *subif_index, 
		uint32_t sb_idx, 
		uint32_t priority,
		uint32_t flags)
{
	int i, j;
	uint8_t leaf_mask;
	uint32_t q_in;
	uint32_t sched_input = sb_idx;
	struct tmu_sched_blk_in_link ilink;
	int ret = TMU_HAL_STATUS_OK;

	leaf_mask = tmu_hal_sched_track[sb_idx].leaf_mask;

	/* get any free leaf out of 0 to 7 */
	for (i = 0; i < 8; i++) {
		if (!((leaf_mask >> i) & 0x1)) {
			leaf_mask |= (1 << i);
			break;
		}
	}
	if (i >=8) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR No Free leaf\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> free leaf = %d shift index %d \n",__FUNCTION__, i, priority);

	if(i == priority)
		return TMU_HAL_STATUS_OK;

	for(j= i-1; j >= priority; j--) {
		if(j < 0)
			return PPA_FAILURE;

		tmu_sched_blk_in_link_get(sched_input << 3 | j, &ilink);
		/*TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Index=%d SBIN: %d is %s\n",__FUNCTION__,j, (sched_input << 3 | j), ilink.sie ? "enabled":"disabled");*/
		if(ilink.sit == 0) /* input is QID */{
			uint32_t cfg_shaper;

			q_in = ilink.qsid;

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is attached to this Queue\n",__FUNCTION__,tmu_hal_queue_track[q_in].tb_index);
			cfg_shaper = tmu_hal_queue_track[q_in].tb_index;

			tmu_hal_shift_up_down_q(netdev, dev_name, port, sched_input, q_in, cfg_shaper, subif_index, (j+1), flags);

		} else if(ilink.sit == 1){ /* input is SBID*/
			if(subif_index->is_logical == 0 && 
					tmu_hal_sched_track[ilink.qsid].policy == TMU_HAL_POLICY_STRICT)  {
				/* must be sub-interface is connected to this leaf */
				int t;
				struct tmu_hal_user_subif_abstract *temp_index = NULL;

				temp_index = subif_index +1;
				for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id: %d port_sched_in %d port_shaper_sched %d ilink.qsid = %d\n",
							__FUNCTION__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, 
							temp_index->port_shaper_sched, ilink.qsid);
					if((temp_index->base_sched_id_egress == ilink.qsid) || temp_index->port_shaper_sched == ilink.qsid){
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id: %d port_sched_in %d ilink.qsid = %d\n",
								__FUNCTION__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, ilink.qsid);
						break;
					}
					temp_index += 1;
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub interface %d port scheduler input is changed from %d -> %d\n",
								__FUNCTION__, t, temp_index->port_sched_in_egress, sched_input << 3 | (j+1)); 
				temp_index->port_sched_in_egress = sched_input << 3 | (j+1);
			} else {
				struct tmu_sched_blk_in_link ilink_wfq;
				struct tmu_hal_user_subif_abstract *temp_index = NULL;
				int32_t w=0, t=0;

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler %d has Policy : %d \n",__FUNCTION__, ilink.qsid, tmu_hal_sched_track[ilink.qsid].policy);
				
				for(w=0;w<8;w++) {
					tmu_sched_blk_in_link_get(ilink.qsid << 3 | w, &ilink_wfq);
					if(ilink_wfq.sit == 1) { /* input is SBID*/
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler input %d : leaf %d \n",
								__FUNCTION__, ilink_wfq.qsid, w);
						if(tmu_hal_sched_track[ilink_wfq.qsid].policy == TMU_HAL_POLICY_STRICT) {
							temp_index = subif_index +1;
							for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
								TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id: %d port_sched_in %d port_shaper_sched %d ilink.qsid = %d\n",
								__FUNCTION__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, 
								temp_index->port_shaper_sched, ilink_wfq.qsid);
								if((temp_index->base_sched_id_egress == ilink_wfq.qsid) || temp_index->port_shaper_sched == ilink_wfq.qsid){
									TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id: %d port_sched_in %d ilink.qsid = %d\n",
										__FUNCTION__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, ilink_wfq.qsid);
									break;
								}
								temp_index += 1;
							}
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub interface %d port scheduler input is changed from %d -> %d\n",
									__FUNCTION__, t, temp_index->port_sched_in_egress, sched_input << 3 | (j+1)); 
							temp_index->port_sched_in_egress = sched_input << 3 | (j+1);

						}
					}
				}
			}
			tmu_hal_sched_out_remap(ilink.qsid, tmu_hal_sched_track[sb_idx].level +1,sched_input << 3 | (j+1), 1, ilink.iwgt);

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disable the input link of : %d \n",__FUNCTION__, sched_input << 3 | j);
			tmu_hal_scheduler_in_enable_disable(sched_input << 3 | j , 0);
			tmu_hal_sched_track[sched_input].leaf_mask &= ~(1<< j );
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Updated leaf mask of SB: %d : %d \n", sched_input,tmu_hal_sched_track[sched_input].leaf_mask);
		} /* ilink.sit is enabled*/
	}
	return ret;
}


int tmu_hal_q_align_for_priority_selector(
			struct net_device *netdev,
			char *dev_name, 
			uint32_t port, 
			struct tmu_hal_user_subif_abstract *subif_index, 
			uint32_t sb1_idx, 
			uint32_t sb2_idx, 
			uint32_t priority,
			uint32_t flags)
{
	int i, j, k;
		uint8_t leaf_mask;
	uint32_t q_in;
	uint32_t sched_in;
	uint32_t sched_input = sb1_idx;
	uint32_t sched_input_new = sb2_idx;
	struct tmu_sched_blk_in_link ilink;
	int ret = TMU_HAL_STATUS_OK;

		leaf_mask = tmu_hal_sched_track[sb1_idx].leaf_mask;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Find the index for priority %d scheduler %d\n",__FUNCTION__,priority,sched_input);
	i =0;
	for(j=7; j >=0; j--) {
		sched_in = sb1_idx << 3 | j ;
		tmu_sched_blk_in_link_get(sched_in, &ilink);
		/*TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>SBIN: %d is %s\n",__FUNCTION__,sched_in, ilink.sie ? "enabled":"disabled");*/
		i++;
		if(ilink.sit == 0) /* input is QID */{	
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Prio level %d for Q index %d\n", 
			subif_index->user_queue[tmu_hal_queue_track[ilink.qsid].user_q_idx].prio_level, ilink.qsid);
			if(priority > subif_index->user_queue[tmu_hal_queue_track[ilink.qsid].user_q_idx].prio_level)
				break;

		} else if(ilink.sit == 1) {/* input is SBID*/
			for(k=0; k<subif_index->queue_cnt; k++) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"k %d sbin %d\n",k, subif_index->user_queue[k].sbin);
				if((subif_index->user_queue[k].sbin >> 3) == ilink.qsid) 
					break;
			}
			if(k == subif_index->queue_cnt) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Queue Count on main interface=%d Queue Count on port =%d\n", subif_index->queue_cnt, 
							tmu_hal_port_track[subif_index->tmu_port_idx].no_of_queues);
 				if(subif_index->queue_cnt < tmu_hal_port_track[subif_index->tmu_port_idx].no_of_queues ) {
					if(subif_index->is_logical == 0) {
						int t;
						struct tmu_hal_user_subif_abstract *temp_index = NULL;
				
						temp_index = subif_index +1;
						for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Sub interface %d User level %d\n",t,temp_index->port_sched_in_user_lvl);
							if(temp_index->port_sched_in_user_lvl < priority){
								goto Q_ALIGN_INDEX;
							}
							temp_index += 1;
						} 
					}
				}
			} else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"user Queue Index %d is connected to SBIN %d\n ",k, subif_index->user_queue[k].sbin);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Prio level %d\n",subif_index->user_queue[k].prio_level);
				if(subif_index->user_queue[k].prio_level < priority)
					break;
			}
		}
	}

Q_ALIGN_INDEX:	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> i =%d j = %d \n",__FUNCTION__,i,j);
	
	if(j == 7)
		return TMU_HAL_STATUS_OK;

	k =0; 
	j +=1;	
	for( ; j < 8; j++) {
		tmu_sched_blk_in_link_get(sched_input << 3 | j, &ilink);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"SBIN: %d is %s\n",sched_input << 3 | j, ilink.sie ? "enabled":"disabled");
		if(ilink.sit == 0) /* input is QID*/
				{
			uint32_t cfg_shaper;

			q_in = ilink.qsid;

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is attached to this Queue\n",__FUNCTION__,tmu_hal_queue_track[q_in].tb_index);
			cfg_shaper = tmu_hal_queue_track[q_in].tb_index;

			tmu_hal_shift_up_down_q(netdev, dev_name, port, sched_input_new, q_in, cfg_shaper, subif_index, k, flags);

		} else if(ilink.sit == 1){ /* input is SBID */
			uint32_t omid;
			uint32_t sb_in = (sched_input << 3 | j);
			omid = sched_input_new << 3 | k;
			if(subif_index->is_logical == 0 && 
				tmu_hal_sched_track[ilink.qsid].policy == TMU_HAL_POLICY_STRICT) {/* must be sub-interface is connected to this leaf */
				int t;
				struct tmu_hal_user_subif_abstract *temp_index = NULL;
				
				temp_index = subif_index +1;
				for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
					if((temp_index->base_sched_id_egress == ilink.qsid) || temp_index->port_shaper_sched == ilink.qsid){
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"base_sched_id: %d ilink.qsid = %d\n", temp_index->base_sched_id_egress, ilink.qsid);
						break;
					}
					temp_index += 1;
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub interface port scheduler input is changed from %d -> %d\n",
							__FUNCTION__,temp_index->port_sched_in_egress, omid); 
				temp_index->port_sched_in_egress = omid;
			} else {
				struct tmu_sched_blk_in_link ilink_wfq;
				struct tmu_hal_user_subif_abstract *temp_index = NULL;
				int32_t w=0, t=0;

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler %d has Policy : %d \n",__FUNCTION__, ilink.qsid, tmu_hal_sched_track[ilink.qsid].policy);
				
				for(w=0;w<8;w++) {
					tmu_sched_blk_in_link_get(ilink.qsid << 3 | w, &ilink_wfq);
					if(ilink_wfq.sit == 1) { /* input is SBID*/
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler input %d : leaf %d \n",
								__FUNCTION__, ilink_wfq.qsid, w);
						if(tmu_hal_sched_track[ilink_wfq.qsid].policy == TMU_HAL_POLICY_STRICT) {
							temp_index = subif_index +1;
							for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
								TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id: %d port_sched_in %d port_shaper_sched %d ilink.qsid = %d\n",
								__FUNCTION__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, 
								temp_index->port_shaper_sched, ilink_wfq.qsid);
								if((temp_index->base_sched_id_egress == ilink_wfq.qsid) || temp_index->port_shaper_sched == ilink_wfq.qsid){
									TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id: %d port_sched_in %d ilink.qsid = %d\n",
										__FUNCTION__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, ilink_wfq.qsid);
									break;
								}
								temp_index += 1;
							}
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub interface port scheduler input is changed from %d -> %d\n",
									__FUNCTION__,temp_index->port_sched_in_egress, omid); 
							temp_index->port_sched_in_egress = omid;
						}
					}
				}
			}

			tmu_hal_sched_out_remap(ilink.qsid, tmu_hal_sched_track[sched_input].level+1, omid, 1, 0);

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disable the input link of : %d \n",__FUNCTION__, sb_in);
			tmu_hal_scheduler_in_enable_disable(sb_in , 0);
			tmu_hal_sched_track[sched_input].leaf_mask &= ~(1<< j );
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Updated leaf mask of SB: %d : %d \n", __FUNCTION__, sb_in, tmu_hal_sched_track[sched_input].leaf_mask);

		}
		k++;
	}
	return ret;
}

int tmu_hal_get_q_idx_of_same_prio(
		struct tmu_hal_user_subif_abstract *subif_index, 
		uint32_t sched, 
		uint32_t priority, 
		uint32_t *q_index, 
		uint8_t *user_index, 
		uint32_t *sb_to_connect,
		uint32_t flags)
{
	int i=0;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Queue Count: %d\n",subif_index->queue_cnt);
	for(i=0; i<subif_index->queue_cnt ; i++) {
		
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> UserQ[%d].prio=%d priority: %d \n",__FUNCTION__,i, subif_index->user_queue[i].prio_level,priority);			
		
		if(subif_index->user_queue[i].prio_level == priority) 
			break;
	}
	if(i == subif_index->queue_cnt ) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<tmu_hal_get_q_idx_of_same_prio> Queue Count on main interface=%d Queue Count on port =%d\n", 
				subif_index->queue_cnt, tmu_hal_port_track[subif_index->tmu_port_idx].no_of_queues);
 		if(subif_index->queue_cnt < tmu_hal_port_track[subif_index->tmu_port_idx].no_of_queues ) {
			/**	For sure Queue is connected for sub interface also*/
			if(subif_index->is_logical == 0 || flags & PPA_QOS_Q_F_VAP_QOS) {
				uint32_t sched_in, base_sched;
				int t;
				struct tmu_hal_user_subif_abstract *temp_index = NULL;
				struct tmu_sched_blk_in_link ilink;

				if(tmu_hal_sched_track[sched].priority_selector == 1) {
					if(priority > 7)
						sched_in = tmu_hal_sched_track[tmu_hal_sched_track[sched].next_sched_id].peer_sched_id;
					else
						sched_in = tmu_hal_sched_track[sched].next_sched_id;
				} else {
					sched_in = sched;
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> scheduler input = %d\n",__FUNCTION__, sched_in);			
				temp_index = subif_index +1;
				for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++)  {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub interface %d User level %d connected to port input %d\n",
							__FUNCTION__,t,temp_index->port_sched_in_user_lvl, temp_index->port_sched_in_egress);
					if (temp_index->port_sched_in_user_lvl == priority) {
						if (flags & PPA_QOS_Q_F_VAP_QOS) {
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Same priority Sub-interface input match found for index\n",
									__FUNCTION__);
							*user_index = temp_index->port_sched_in_egress; //& 7;
							*sb_to_connect = sched_in;
							return TMU_HAL_STATUS_OK;
						}
						*user_index = temp_index->port_sched_in_egress & 7;
						*sb_to_connect = sched_in;
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Same priority Sub-interface input match found for index %d\n",
									__FUNCTION__, *user_index);

						tmu_sched_blk_in_link_get(temp_index->port_sched_in_egress, &ilink);
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> ilink.qsid = %d\n",__FUNCTION__, ilink.qsid);

						if(tmu_hal_sched_track[ilink.qsid].policy == TMU_HAL_POLICY_STRICT) {
							/* must be sub-interface is connected to this leaf	*/
							tmu_sched_blk_in_enable(temp_index->port_sched_in_egress, 0); /* Disable the scheduler input */
							tmu_hal_sched_track[sched_in].leaf_mask &= ~(1<< *user_index);
							base_sched = create_new_scheduler(sched_in, 0, TMU_HAL_POLICY_STRICT, 
										tmu_hal_sched_track[ilink.qsid].level, 
										*user_index);
							tmu_hal_sched_track[base_sched].policy = TMU_HAL_POLICY_WFQ;
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New Schedular =%d\n",__FUNCTION__, base_sched); 

							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,">>>> Remap SB = %d\n", ilink.qsid);
							tmu_hal_sched_out_remap(ilink.qsid, 
										tmu_hal_sched_track[base_sched].level + 1, 
										base_sched << 3, 1, 
										(subif_index->subif_weight==0)?1:subif_index->subif_weight);
						}
						*q_index = 0xFF;
						return TMU_HAL_STATUS_OK;
					}
					temp_index += 1;
				} 
				*q_index = 0xFF;
				*sb_to_connect = 0xFF;
				return TMU_HAL_STATUS_ERR;
			} 
			goto NO_MATCH_FOUND;				
		} 
		goto NO_MATCH_FOUND;				
	} else {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Same priority Queue match found for index %d\n",__FUNCTION__, i);
		*q_index =	subif_index->user_queue[i].queue_index;
		*sb_to_connect =	subif_index->user_queue[i].sbin >> 3;
		*user_index = subif_index->user_queue[i].sbin & 7;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> User Index %d SB to connect: %d\n",__FUNCTION__,*user_index, *sb_to_connect);
	}

	return TMU_HAL_STATUS_OK;
NO_MATCH_FOUND:
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> No Match Found \n",__FUNCTION__);
	*q_index = 0xFF;
	*sb_to_connect = 0xFF;
	return TMU_HAL_STATUS_ERR;
}

uint32_t tmu_hal_get_base_sched_id(uint32_t sbin, uint32_t sched_id) {
	uint32_t sbin_temp = sbin;
	while((sbin >> 3 != sched_id) && ((sbin >> 3) > 0 && (sbin != INVALID_SCHED_ID))) {
		sbin_temp = sbin;
		sbin = tmu_hal_sched_track[sbin_temp >> 3].omid;
	}
	return (sbin_temp >> 3);
}

int 
tmu_hal_find_sched_in_for_q_add(
			struct tmu_hal_user_subif_abstract *subif_index, 
			uint32_t sched_id, 
			uint32_t priority, 
			uint32_t *index)
{
	int i,j,k;
	uint32_t sched_in;
	struct tmu_sched_blk_in_link ilink;
	uint8_t leaf;
	uint32_t base_sched_id;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enter\n",__FUNCTION__);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>sched_id %d\n",__FUNCTION__,sched_id);
	if (tmu_hal_scheduler_free_input_get(
				sched_id,
				&leaf) < 0)
	{
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get Free leaf\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Queue Count: %d free_leaf %d\n",__FUNCTION__,subif_index->queue_cnt, leaf);
	j=0;
	for(i= leaf; i>0 ; i--) {
		sched_in = sched_id << 3 | (i-1) ;
		tmu_sched_blk_in_link_get(sched_in, &ilink);
		/*TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> SBIN: %d is %s\n",__FUNCTION__,sched_in, ilink.sie ? "enabled":"disabled");*/
		j++;
		if(ilink.sie != 0) { /* input is enabled*/
			if(ilink.sit == 0) /* input is QID */
			{	
				/*TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>User Q index %d\n",__FUNCTION__, tmu_hal_queue_track[ilink.qsid].user_q_idx);*/
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Prio level %d for Q index %d\n",
						__FUNCTION__, subif_index->user_queue[tmu_hal_queue_track[ilink.qsid].user_q_idx].prio_level, ilink.qsid);
				if(priority > subif_index->user_queue[tmu_hal_queue_track[ilink.qsid].user_q_idx].prio_level)
					break;

			} else if(ilink.sit == 1) {/* input is SBID */
				for(k=0; k<subif_index->queue_cnt && subif_index->queue_cnt<=TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE; k++) {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"k %d sbin %d\n",k, subif_index->user_queue[k].sbin);
					base_sched_id = tmu_hal_get_base_sched_id(subif_index->user_queue[k].sbin, sched_id);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> base_sched_id :[%d]\n", __FUNCTION__, base_sched_id);
					if(base_sched_id == ilink.qsid)
						break;
				}
				if(k < TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE) {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> User Queue Index %d is connected to SBIN %d\n ",__FUNCTION__,k, subif_index->user_queue[k].sbin);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Prio level %d\n",__FUNCTION__, subif_index->user_queue[k].prio_level);

					if(k == subif_index->queue_cnt) {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Queue Count on main interface=%d Queue Count on port =%d\n", subif_index->queue_cnt,
								tmu_hal_port_track[subif_index->tmu_port_idx].no_of_queues);
						if(subif_index->queue_cnt < tmu_hal_port_track[subif_index->tmu_port_idx].no_of_queues ) {
							if(subif_index->is_logical == 0) {
								int t;
								struct tmu_hal_user_subif_abstract *temp_index = NULL;

								temp_index = subif_index +1;
								for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
									TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub interface %d User level %d\n",
											__FUNCTION__,t,temp_index->port_sched_in_user_lvl);
									if(temp_index->port_sched_in_user_lvl < priority){
										goto QUEUE_ADD_INPUT_INDEX;
									}
									temp_index += 1;
								}
							}
						}
					} else {
						if(subif_index->user_queue[k].prio_level < priority)
							break;
					}
				}
			} /* ilink.sit == 1 */
		}
	}
QUEUE_ADD_INPUT_INDEX:
	/*TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"i %d j:%d leaf:%d\n",i, j , leaf);*/
	if(i >1 && j == leaf ) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Queue add input index %d\n",leaf);
		*index = leaf;
	} else {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Queue add input index %d\n",i);
		*index = i;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Exit\n",__FUNCTION__);
	return TMU_HAL_STATUS_OK;
}

int tmu_hal_queue_add(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port, 
		uint32_t base_sched, 
		uint32_t prio_type, 
		uint32_t priority, 
		uint32_t weight, 
		struct tmu_hal_user_subif_abstract *subif_index,
		uint32_t flags)
{
	uint32_t q_index, q_new=0, sched_id, shift_idx;
	uint8_t uIndex;
	struct tmu_hal_sched_track_info Snext;
	enum tmu_hal_errorcode eErr;
	int ret = TMU_HAL_STATUS_OK;
	if(base_sched >= TMU_HAL_MAX_SCHEDULER)
		return TMU_HAL_STATUS_MAX_SCHEDULERS_REACHED;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> port: %d base_sched: %d prio_type: %d priority: %d weight: %d\n",
			__FUNCTION__, port, base_sched, prio_type, priority, weight); 
	/* Initialize Snext to the base scheduler id */
	Snext = tmu_hal_sched_track[base_sched];
	tmu_hal_dump_sb_info(Snext.id);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Base Scheduler Id: %d\n",__FUNCTION__, Snext.id);

	if(tmu_hal_get_q_idx_of_same_prio(subif_index, Snext.id, priority, &q_index, &uIndex, &sched_id, flags) != TMU_HAL_STATUS_OK) {

		if(tmu_hal_sched_track[base_sched].policy == TMU_HAL_POLICY_STRICT) { 
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Priority Scheduler and Priority Queue\n",__FUNCTION__);
			if(Snext.leaf_mask < 0xFF && Snext.priority_selector != 1) {
				tmu_hal_find_sched_in_for_q_add(subif_index, Snext.id, priority, &shift_idx );
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>shift_idx:%d\n",__FUNCTION__,shift_idx);
				tmu_hal_shift_down_sbin_conn(netdev, dev_name, port, subif_index, Snext.id, shift_idx, flags);
				q_new = tmu_hal_add_new_queue(netdev, dev_name, port, prio_type, shift_idx, weight, Snext.id, subif_index, &eErr, flags );
			} else {
				uint32_t omid, parent;
				uint32_t free_sched;
				struct tmu_hal_sched_track_info Stemp;
				parent = Snext.id;
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"parent %d next_sched_id %d\n",parent, Snext.next_sched_id);
				if(Snext.leaf_mask == 0xFF && tmu_hal_sched_track[Snext.next_sched_id].priority_selector != 2)  {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Adding a new level\n",__FUNCTION__);
					tmu_hal_add_scheduler_level(port, base_sched, weight, &parent, prio_type, &omid);
					tmu_hal_sched_track[parent].priority_selector = 1;
					tmu_hal_sched_track[Snext.id].priority_selector = 2;
					tmu_hal_sched_track[omid].priority_selector = 3;
					subif_index->base_sched_id_egress = parent;
					Stemp = tmu_hal_sched_track[parent];
					tmu_hal_q_align_for_priority_selector(netdev, dev_name, port, subif_index, 
							tmu_hal_sched_track[Stemp.next_sched_id].id, 
							tmu_hal_sched_track[Stemp.next_sched_id].peer_sched_id, TMU_HAL_Q_PRIO_LEVEL, flags);
				} 
				Stemp = tmu_hal_sched_track[parent];
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base sched %d Peer sched %d\n",
						__FUNCTION__,
						tmu_hal_sched_track[Stemp.next_sched_id].id, 
						tmu_hal_sched_track[Stemp.next_sched_id].peer_sched_id);	

				tmu_hal_free_prio_scheduler_get(parent, tmu_hal_sched_track[Stemp.next_sched_id].id, priority, &free_sched);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Free Priority scheduler %d\n",free_sched);	
				tmu_hal_find_sched_in_for_q_add(subif_index, free_sched, priority, &shift_idx );
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"shift_idx:%d\n",shift_idx);
				tmu_hal_shift_down_sbin_conn(netdev, dev_name, port, subif_index, free_sched, shift_idx, flags);
				q_new = tmu_hal_add_new_queue(netdev, dev_name, port, prio_type, shift_idx, weight, free_sched, subif_index, &eErr , flags);
			}
		} else if(tmu_hal_sched_track[base_sched].policy == TMU_HAL_POLICY_WFQ && prio_type == TMU_HAL_POLICY_WFQ) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," WFQ Scheduler and WFQ Queue\n");
			if(Snext.leaf_mask < 0xFF && Snext.priority_selector != 1) {
				q_new = tmu_hal_add_new_queue(netdev, dev_name, port, prio_type, priority, weight, Snext.id, subif_index, &eErr, flags);
			} else {
				uint32_t omid, parent;
				uint32_t free_wfq_sched;
				struct tmu_hal_sched_track_info Stemp;
				parent = Snext.id;
				Stemp = tmu_hal_sched_track[Snext.next_sched_id];
				if(Snext.next_sched_id == 0xFF) {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Adding a new level\n");
					tmu_hal_add_scheduler_level(port, base_sched, weight, &parent, prio_type, &omid);
					subif_index->base_sched_id_egress = parent;
					free_wfq_sched = omid;
				} else {

					tmu_hal_free_wfq_scheduler_get(parent, tmu_hal_sched_track[Snext.next_sched_id].id, &free_wfq_sched);
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Free WFQ scheduler %d\n",free_wfq_sched);	
				q_new = tmu_hal_add_new_queue(netdev, dev_name, port, prio_type, priority, weight, free_wfq_sched, subif_index, &eErr, flags);

			}
		} else if(tmu_hal_sched_track[base_sched].policy == TMU_HAL_POLICY_WFQ && prio_type == TMU_HAL_POLICY_STRICT) {
			uint8_t leaf;
			uint32_t old_omid, omid;
			struct tmu_hal_sched_track_info Snext;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," WFQ Scheduler and Priority Queue\n");
			Snext = tmu_hal_sched_track[base_sched];
			if(Snext.sched_out_conn == 1 ) {
				old_omid = Snext.omid >> 3;
				omid = create_new_scheduler(old_omid, 0, TMU_HAL_POLICY_STRICT, Snext.level, 0);
				tmu_hal_sched_track[old_omid].next_sched_id = omid;
				tmu_hal_sched_track[omid].next_sched_id = Snext.id;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Configuring	Scheduler id of SB %d -->SB %d -->SB %d \n",Snext.id, omid, Snext.omid);
			} else if(Snext.sched_out_conn == 0 ) { 
				old_omid = Snext.omid ;
				omid = create_new_scheduler(old_omid, 1, TMU_HAL_POLICY_STRICT, Snext.level, 0);
				tmu_hal_port_track[port].input_sched_id = omid;
			} else {

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR," Wrong Scheduler Out connection !!!!\n");
				return TMU_HAL_STATUS_ERR; 
			}
			subif_index->base_sched_id_egress = omid;
			tmu_hal_sched_track[omid].policy = TMU_HAL_POLICY_STRICT;
			q_new = tmu_hal_add_new_queue(netdev, dev_name, port, prio_type, priority, weight, omid, subif_index, &eErr, flags);

			/*Queue is added. So the output of the WFQ scheduler can now be
			//connected to the any leaf (mostly the lowest priority leaf) of the	 
			//priority scheduler with weight.*/
			tmu_hal_scheduler_free_input_get(omid, &leaf);
			tmu_hal_sched_out_remap(Snext.id, Snext.level+ 1, omid << 3 | leaf, 1, 1);
		}
	} else {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"User Index:%d Scheduler Block: %d\n",uIndex, sched_id);
		if(q_index == 0xFF) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Connected SB->SB Input : at the same priority %d: %d\n", 
					__FUNCTION__, sched_id , subif_index->user_queue[uIndex].sbin & 7);
			q_new = tmu_hal_add_new_queue(netdev , dev_name, port, prio_type, uIndex, weight, sched_id, subif_index, &eErr, flags);
		}
		else {
			uint32_t conn_schd = 0, peer_schd = 0;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Connected Q->SB->SB Input : at the same priority %d:%d: %d\n", 
					__FUNCTION__, q_index, tmu_hal_queue_track[q_index].connected_to_sched_id , subif_index->user_queue[uIndex].sbin & 7);
			conn_schd = tmu_hal_queue_track[q_index].connected_to_sched_id;
			peer_schd = tmu_hal_sched_track[conn_schd].peer_sched_id;
			if (tmu_hal_sched_track[conn_schd].policy == TMU_HAL_POLICY_WFQ && tmu_hal_sched_track[conn_schd].leaf_mask == 0xFF && peer_schd == 0 ) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Connected and peer shcedular id %d %d\n", conn_schd, peer_schd);
				tmu_hal_shift_down_sbin_conn(netdev, dev_name, port, subif_index, Snext.id, uIndex + 1, flags);
			}
			q_new = tmu_hal_add_new_queue(netdev,
							dev_name, 
							port, 
							prio_type, 
							uIndex, 
							weight, 
							tmu_hal_queue_track[q_index].connected_to_sched_id, 
							subif_index, &eErr, flags);
		}
	}

	if(ret == TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<tmu_hal_queue_add>New Queue is %d\n",q_new);
		return q_new;
	} else
		ret = TMU_HAL_STATUS_ERR;
	return ret;
}

/** The tmu_hal_create_port_scheduler function is used to add a scheduler
	when a queue will be added for any logical interface of that port.
*/
int tmu_hal_create_port_scheduler(uint32_t port)
{
	uint32_t sched_prio, omid;
	uint32_t remap_sched_id;
	int input_type, level;
	
		
	if(tmu_hal_port_track[port].shaper_sched_id == 0xFF) {
		omid = port;
		input_type = 1;
		level = 0;
		remap_sched_id = tmu_hal_port_track[port].input_sched_id;
	} else {/*Port Rate shaper is already there*/
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Shaper scheduler is there\n");
		omid = tmu_hal_port_track[port].shaper_sched_id;
		input_type = 0;
		level = 1;
		remap_sched_id = tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].next_sched_id ;
	}
	
	sched_prio = create_new_scheduler(omid, input_type, TMU_HAL_POLICY_STRICT, level, 0);
	tmu_hal_port_track[port].port_sched_id = sched_prio;	
	tmu_hal_sched_track[sched_prio].policy = TMU_HAL_POLICY_WFQ;
	tmu_hal_sched_track[sched_prio].next_sched_id = remap_sched_id;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Scheduler that needs to Remap is %d\n",remap_sched_id);
	tmu_hal_sched_out_remap(remap_sched_id, level+ 1, sched_prio << 3, 1, 0);
	
	return TMU_HAL_STATUS_OK;
}

int tmu_hal_is_priority_scheduler_for_port(uint32_t port)
{

	if(tmu_hal_port_track[port].port_sched_id == 0xFF) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR No schedular is Attached to Port\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}
	return TMU_HAL_STATUS_OK;

}

int tmu_hal_dump_subif_queue_info(struct tmu_hal_user_subif_abstract *subif_index) {

	int i =0;
	int32_t shaper_idx=0;
	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "\tQueue Count = %5d \t\t\t\tInterface = %7s\n",subif_index->queue_cnt, subif_index->netdev->name);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "\t==================================================================================================================\n");
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "\t|	QID	|	TMU QID		|	Priority	|	Type	|	Weight	|	Shaper	|\n");
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "\t------------------------------------------------------------------------------------------------------------------\n");

	for(i=0; i < subif_index->queue_cnt && subif_index->queue_cnt<=TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE; i++) {
		shaper_idx = tmu_hal_queue_track[subif_index->user_queue[i].queue_index].tb_index;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t|	%3d	|",subif_index->user_queue[i].qid);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"	%6d	|",subif_index->user_queue[i].queue_index);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"	%6d	|",subif_index->user_queue[i].prio_level);
		if (subif_index->user_queue[i].queue_type ==	TMU_HAL_POLICY_STRICT )
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"	  SP  	|");
		else 
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"	  WFQ	|");
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"	%6d	|",subif_index->user_queue[i].weight);
		if ( shaper_idx != 0xFF ) {
		
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"	idx=%3d	|\n", tmu_hal_shaper_track[shaper_idx].tb_cfg.index);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t|\t\t\t\t\t\t\t\t\t\t	 | mode=%9d	 |\n", tmu_hal_shaper_track[shaper_idx].tb_cfg.mode);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t|\t\t\t\t\t\t\t\t\t\t	 | cir=%9d	|\n", tmu_hal_shaper_track[shaper_idx].tb_cfg.cir);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t|\t\t\t\t\t\t\t\t\t\t	 | pir=%9d	|\n", tmu_hal_shaper_track[shaper_idx].tb_cfg.pir);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t|\t\t\t\t\t\t\t\t\t\t	 | pbs=%9d	|\n", tmu_hal_shaper_track[shaper_idx].tb_cfg.pbs);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t|\t\t\t\t\t\t\t\t\t\t	 | cbs=%9d	|\n", tmu_hal_shaper_track[shaper_idx].tb_cfg.cbs);
		} else {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"	X	|\n");

		}	
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "\t------------------------------------------------------------------------------------------------------------------\n");
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "\t==================================================================================================================\n");
	return TMU_HAL_STATUS_OK;
}

int tmu_hal_dump_subif_queue_map(struct tmu_hal_user_subif_abstract *subif_index, uint32_t sys_q) {

	int32_t i =0, j=0;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\tQueue Count = %d Interface = %s\n",subif_index->queue_cnt, subif_index->netdev->name);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t ==================================================================\n");
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t|		User QId	|	TMU QId	|	Tc		|\n");
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t ------------------------------------------------------------------\n");
	for(i=0; i< subif_index->queue_cnt; i++) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t|		%6d	",subif_index->user_queue[i].qid);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"|	%8d	",subif_index->user_queue[i].queue_index);
		if ( sys_q == subif_index->user_queue[i].queue_index ) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"|	System Default	|\n");	
		} else if (subif_index->default_q == subif_index->user_queue[i].queue_index) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"|	Qos Default	|\n");	
		} else {	
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"| ");
			for(j=0; j< MAX_TC_NUM; j++) {	
				if (subif_index->user_queue[i].qmap[j]) {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"%2d,",subif_index->user_queue[i].qmap[j]);
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"	 ");	
				}
			}
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," |\n");
		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t ------------------------------------------------------------------\n");
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"\t ==================================================================\n");

	return TMU_HAL_STATUS_OK;
}

int tmu_hal_get_pppoa_if(struct net_device *netif, PPA_IFNAME base_ifname[PPA_IF_NAME_SIZE], uint32_t *flags)
{

	PPA_VCC *vcc = NULL;
	short vpi = 0;
	int vci = 0;

	if ( ppa_if_is_pppoa(netif, NULL) && ppa_pppoa_get_vcc(netif, &vcc) == PPA_SUCCESS) {
		*flags |= PPA_F_PPPOATM;
		vpi = vcc->vpi;
		vci = vcc->vci;
		snprintf(base_ifname, PPA_IF_NAME_SIZE, "atm_%d%d",vpi,vci);
		return PPA_SUCCESS;
	}
	else
	return PPA_FAILURE;
}

int tmu_hal_get_queue_info(struct net_device *netdev) {

	uint32_t nof_of_tmu_ports, flags=0;
	dp_subif_t *dp_subif = NULL;
	cbm_tmu_res_t *tmu_res = NULL;
	struct tmu_hal_user_subif_abstract *subif_index;
	PPA_VCC *vcc = NULL;
	char dev_name[PPA_IF_NAME_SIZE];

	TMU_ALLOC(dp_subif_t, dp_subif);
	ppa_br2684_get_vcc(netdev,&vcc);
	if(tmu_hal_get_pppoa_if(netdev,dev_name,&flags) == PPA_SUCCESS) {
		if((flags & PPA_F_PPPOATM) == PPA_F_PPPOATM) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Interface : %s , Base PPPOA Interface : %s \n",netdev->name,dev_name);	
			if (dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get Subif Id when VCC NULL \n", __FUNCTION__, __LINE__);
				kfree(dp_subif);
				return TMU_HAL_STATUS_ERR;
			}
		}
	} else {
		if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, 0) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get Subif Id\n", __FUNCTION__, __LINE__);
			kfree(dp_subif);
			return TMU_HAL_STATUS_ERR;
		}
	}
	/* printk("dp_subif.port_id =%d dp_subif.subif=%d\n", dp_subif.port_id, dp_subif.subif);*/
	dp_subif->subif	= dp_subif->subif >> 8;
	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, 0) != PPA_SUCCESS) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get Subif Id\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}

	/*printk("dp_subif.port_id =%d dp_subif.subif=%d\n", dp_subif.port_id, dp_subif.subif);*/
	if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res,0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	/*printk("tmu_res->tmu_port =%d\n", tmu_res->tmu_port);*/
	subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
	tmu_hal_dump_subif_queue_info(subif_index);
	kfree(dp_subif);
	kfree(tmu_res);

	return TMU_HAL_STATUS_OK;
}
EXPORT_SYMBOL(tmu_hal_get_queue_info);


int tmu_hal_get_queue_map(struct net_device *netdev) {

	uint32_t nof_of_tmu_ports, sys_q, flags=0;
	dp_subif_t *dp_subif = NULL;
	cbm_tmu_res_t *tmu_res = NULL;
	struct tmu_hal_user_subif_abstract *subif_index;
	PPA_VCC *vcc = NULL;
	char dev_name[PPA_IF_NAME_SIZE];

	TMU_ALLOC(dp_subif_t, dp_subif);
	ppa_br2684_get_vcc(netdev,&vcc);
	if(tmu_hal_get_pppoa_if(netdev,dev_name,&flags) == PPA_SUCCESS) {
		if((flags & PPA_F_PPPOATM) == PPA_F_PPPOATM) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:Interface of type PPPOA : %s \n", __FUNCTION__,dev_name);	
			if (dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get Subif Id when VCC NULL \n", __FUNCTION__, __LINE__);
				kfree(dp_subif);
				return TMU_HAL_STATUS_ERR;
			}
		}
	} else {
		if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, 0) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get Subif Id\n", __FUNCTION__, __LINE__);
			kfree(dp_subif);
			return TMU_HAL_STATUS_ERR;
		}
	}

	/*printk("dp_subif.port_id =%d dp_subif.subif=%d\n", dp_subif.port_id, dp_subif.subif);*/
	if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	
	/*printk("tmu_res->tmu_port =%d\n", tmu_res->tmu_port);*/
	if(nof_of_tmu_ports == 2) {
		subif_index = tmu_hal_user_sub_interface_info + ((tmu_res + 1)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + (dp_subif->subif >> 8);
		sys_q = (tmu_res + 1)->tmu_q;
	} else {
		subif_index = tmu_hal_user_sub_interface_info + ((tmu_res)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + (dp_subif->subif >> 8);
		sys_q = tmu_res->tmu_q;
	}
	tmu_hal_dump_subif_queue_map(subif_index, sys_q);

	kfree(dp_subif);
	kfree(tmu_res);
	return TMU_HAL_STATUS_OK;
}
EXPORT_SYMBOL(tmu_hal_get_queue_map);

int tmu_hal_get_detailed_queue_map(uint32_t queue_index)
{
	cbm_queue_map_entry_t *q_map_get = NULL;
	int32_t num_entries = 0, i = 0;
	if (cbm_queue_map_get(
			g_Inst,
			queue_index,
			&num_entries,
			&q_map_get, 0) == -1){
		TMU_HAL_DEBUG_ERR(TMU_DEBUG_ERR, "CBM FAILURE: Queue %d could not be mapped properly !!\n", queue_index);
		return TMU_HAL_STATUS_ERR;
	}
	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "Queue %d has mapped entries %d \n", queue_index, num_entries);

	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "[FlowId]	[DEC]	[ENC]	[MPE1]	[MPE2]	[TC]	\n");
	for(i=0; i< num_entries; i++)
	{
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "|	%4d	|	%4d	|	%4d	|	%4d	|	%4d	|	%4d	|\n",
				(q_map_get+i)->flowid, 
				(q_map_get+i)->dec, 
				(q_map_get+i)->enc, 
				(q_map_get+i)->mpe1, 
				(q_map_get+i)->mpe2, 
				(q_map_get+i)->tc);
	}

	kfree(q_map_get);

	return TMU_HAL_STATUS_OK;
}
EXPORT_SYMBOL(tmu_hal_get_detailed_queue_map);

int 
tmu_hal_find_min_prio_level_of_port(
			struct tmu_hal_user_subif_abstract *subif_index, 
			uint32_t *sched, 
			uint32_t *prio_lvl)
{
	uint32_t base_sched;

	base_sched = subif_index->base_sched_id_egress;
	if(base_sched >= TMU_HAL_MAX_SCHEDULER)
		return TMU_HAL_STATUS_MAX_SCHEDULERS_REACHED;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler Id: %d Priority selector: %d\n",__FUNCTION__, base_sched, tmu_hal_sched_track[base_sched].priority_selector);
	
	if(tmu_hal_sched_track[base_sched].priority_selector == 0) {
		*sched = base_sched; 
		*prio_lvl = 7;
	} else {
		*sched = tmu_hal_sched_track[tmu_hal_sched_track[base_sched].next_sched_id].peer_sched_id;
		*prio_lvl = TMU_HAL_MAX_PRIORITY_LEVEL-1;
				
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> PEER SCHED Id: %d Priority Level: %d\n",__FUNCTION__, tmu_hal_sched_track[tmu_hal_sched_track[base_sched].next_sched_id].peer_sched_id, *prio_lvl);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler Id: %d Priority Level: %d\n",__FUNCTION__,*sched,*prio_lvl);
	return TMU_HAL_STATUS_OK;
}

/**
	subif_index -- sub interface index of the logical interface
	port_index -- index for the port interface details
	port_sched_id -- scheduler id of the Egress Port
	prio_level -- scheduler input where the logical interface should be connected to
	weight -- weightage among all in that prio_level
*/
int 
tmu_hal_connect_subif_to_port(
			struct net_device *netdev,
			char * dev_name,
			struct tmu_hal_user_subif_abstract *subif_index, 
			struct tmu_hal_user_subif_abstract *port_index, 
			uint32_t port_sched_id, 
			uint32_t prio_level, 
			uint32_t weight,
			uint32_t flags)
{
	uint32_t base_sched = 0xFF, omid = 0xFF;
	uint32_t q_index;
	struct tmu_sched_blk_in_link ilink;
	struct tmu_hal_equeue_create q_reconnect;

	tmu_sched_blk_in_link_get(port_sched_id << 3 | prio_level, &ilink);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> SBIN: %d is %s\n",
			__FUNCTION__, port_sched_id << 3 | prio_level, ilink.sie ? "enabled":"disabled");

	if(ilink.sie != 0 ) /* either queue/sbid is connected to this prio_level */ {
		if(ilink.sit == 0) /* input is QID */ {
			omid = ilink.qsid;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Already Queue %d is added for the to the Port Scheduler In %d\n",
					__FUNCTION__, ilink.qsid, port_sched_id << 3 | prio_level);
			q_index = ilink.qsid; /* this is the QID */
			tmu_hal_safe_queue_delete(netdev, dev_name, q_index, -1, -1, flags);

			omid = create_new_scheduler(port_sched_id, 0, TMU_HAL_POLICY_STRICT, tmu_hal_sched_track[port_sched_id].level + 1, prio_level); 
			tmu_hal_sched_track[omid].policy = TMU_HAL_POLICY_WFQ; 
							
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Reconnecting the queue %d to scheduler %d\n",__FUNCTION__, q_index, omid);
			memset(&q_reconnect, 0, sizeof(struct tmu_hal_equeue_create));
			q_reconnect.index = q_index;				
			q_reconnect.egress_port_number = subif_index->tmu_port_idx;
			q_reconnect.scheduler_input =	omid << 3 | 0; /*highest priority leaf */
			q_reconnect.iwgt = (tmu_hal_queue_track[q_index].prio_weight == 0)?1:tmu_hal_queue_track[q_index].prio_weight ;
			tmu_hal_egress_queue_create(&q_reconnect);

			base_sched = create_new_scheduler(omid, 0, TMU_HAL_POLICY_WFQ, tmu_hal_sched_track[omid].level + 1, 1 ); 
			tmu_hal_sched_track[base_sched].policy = TMU_HAL_POLICY_STRICT; 
					
		} else if(ilink.sit == 1) { /*input is SBID */

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Already Scheduler %d is added for the to the Port Scheduler In %d\n",
					__FUNCTION__, ilink.qsid, port_sched_id << 3 | prio_level);
			if(tmu_hal_sched_track[ilink.qsid].policy == TMU_HAL_POLICY_STRICT) {
				/*Some other sub-interface is already connected to this priority level.*/
				uint32_t sched_in;
				int t;
				struct tmu_hal_user_subif_abstract *temp_index = NULL;

				temp_index = port_index +1;
				for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub interface %d User level %d port_sched_in_egress %d\n",
							__FUNCTION__,t,temp_index->port_sched_in_user_lvl, temp_index->port_sched_in_egress);
					/*TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Sub interface index %x\n",temp_index);*/
					if((temp_index->port_sched_in_user_lvl == prio_level) && (subif_index->port_sched_in_egress >> 3 != NULL_SCHEDULER_BLOCK_ID)){
						break;
					}
					temp_index += 1;
				} 
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub interface %d matched priority %d\n",
						__FUNCTION__, t, temp_index->port_sched_in_user_lvl);
					tmu_sched_blk_in_enable(port_sched_id << 3 | prio_level, 0); /* Disable the scheduler input */
				
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Sched level %d\n",__FUNCTION__,tmu_hal_sched_track[port_sched_id].level);	
				sched_in = create_new_scheduler(port_sched_id, 0, TMU_HAL_POLICY_STRICT, tmu_hal_sched_track[port_sched_id].level + 1, prio_level); 
				tmu_hal_sched_track[sched_in].policy = TMU_HAL_POLICY_WFQ; 
				
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> >>>> Remap SB = %d\n", __FUNCTION__, ilink.qsid);
				tmu_hal_sched_out_remap(ilink.qsid, tmu_hal_sched_track[sched_in].level + 1, sched_in << 3, 1, 1);
				if(temp_index->port_shaper_sched != INVALID_SCHED_ID) {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Sub interface Port Shaper scheduler input is changed from %d -> %d\n",
							__FUNCTION__, tmu_hal_sched_track[temp_index->port_shaper_sched].omid, sched_in << 3); 
					tmu_hal_sched_track[temp_index->port_shaper_sched].omid = sched_in << 3;
				}

				base_sched = create_new_scheduler(sched_in, 0, TMU_HAL_POLICY_WFQ, tmu_hal_sched_track[sched_in].level + 1, 1 /*weight*/); 
				tmu_hal_sched_track[base_sched].policy = TMU_HAL_POLICY_STRICT; 
				
			} else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Already scheduler level is created for same priority %d\n", __FUNCTION__, prio_level);
				base_sched = create_new_scheduler(ilink.qsid, 0, TMU_HAL_POLICY_WFQ, tmu_hal_sched_track[ilink.qsid].level + 1, 1 /*weight*/); 
				tmu_hal_sched_track[base_sched].policy = TMU_HAL_POLICY_STRICT; 
			}
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New Scheduler is %d\n",__FUNCTION__, base_sched);
			subif_index->port_sched_in_egress = port_sched_id << 3 | prio_level;
		}
	} else {

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> priority Level %d is free. Add Scheduler to the Port Scheduler %d\n",
				__FUNCTION__, prio_level, port_sched_id );
		base_sched = create_new_scheduler(port_sched_id, 0, TMU_HAL_POLICY_STRICT, tmu_hal_sched_track[port_sched_id].level + 1, prio_level); 
		tmu_hal_sched_track[base_sched].policy = TMU_HAL_POLICY_STRICT; 
		subif_index->port_sched_in_egress = port_sched_id << 3 | prio_level;
	}

	subif_index->base_sched_id_egress = base_sched;
	subif_index->subif_weight = 0;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base Scheduler %d Port Scheduler input %d\n",
				__FUNCTION__, subif_index->base_sched_id_egress, subif_index->port_sched_in_egress);

	return TMU_HAL_STATUS_OK;

}

/**
	netdev -- netdevice interface
	port_id -- if no netdevice then for this port id
	priority -- scheduler input where the logical interface should be connected to
	weight -- weightage among all in that prio_level
	flags -- flags
*/
int32_t
tmu_hal_modify_subif_to_port(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port_id, 
		uint32_t priority, 
		uint32_t weight,
		uint32_t flags)
{
	uint32_t nof_of_tmu_ports;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	PPA_VCC *vcc = NULL; 
	int32_t ret = TMU_HAL_STATUS_OK;
	int32_t tmuport;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_user_subif_abstract *port_subif_index = NULL;

	TMU_ALLOC(dp_subif_t, dp_subif);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify sub interface %s to priority level %d weight %d\n",__FUNCTION__, netdev->name, priority, weight);
	ppa_br2684_get_vcc(netdev,&vcc);
	if(vcc == NULL)
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"VCC is NULL\n");	
	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get Subif Id\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	dp_subif->subif	= dp_subif->subif >> 8;
	if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
			__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);

	tmuport = tmu_res->tmu_port;
	if(flags & PPA_QOS_Q_F_INGRESS) {
		subif_index = tmu_hal_user_sub_interface_ingress_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_ingress_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	} else {
		subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	}

	/*For Directpath interfacei */
	if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) {
		struct tmu_hal_dp_res_info res = {0};
		tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

		subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);

	}

	if(nof_of_tmu_ports > 1) { /* For LAN interface*/
		if(tmu_hal_get_subif_index_from_netdev(netdev, port_subif_index) == TMU_HAL_STATUS_ERR) {
			port_subif_index = tmu_hal_user_sub_interface_info + ((tmu_res+1)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
			if(tmu_hal_get_subif_index_from_netdev(netdev, port_subif_index) == TMU_HAL_STATUS_ERR) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> No valid port index found for the subif \n",__FUNCTION__);
				ret = TMU_HAL_STATUS_ERR;
				goto CBM_RESOURCES_CLEANUP;
			}
			tmuport = (tmu_res+1)->tmu_port;
		}
	}

	if(dp_subif->subif != 0) {
		uint8_t uIndex;
		uint32_t port_prio_level;
		uint32_t user_prio;
		uint32_t port_sched;
		uint32_t sched_id;
		uint32_t q_index =0, shift_idx;
		uint32_t omid, new_omid = 0xFF, new_port_omid;
		uint32_t base_sched, new_sched;
		struct tmu_sched_blk_in_link ilink;

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> base_sched_id_egress=%d port_sched_in_egress=%d\n",
				__FUNCTION__, subif_index->base_sched_id_egress, subif_index->port_sched_in_egress );

		port_sched = (subif_index->port_sched_in_egress >> 3);
		port_prio_level = user_prio = priority;

		if((subif_index->port_sched_in_user_lvl == priority) || (priority == 0) ) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>No change in priority of Logical interface \n",	__FUNCTION__);
			omid = tmu_hal_sched_track[subif_index->base_sched_id_egress].omid;
			tmu_hal_scheduler_in_weight_update(omid, weight);
			subif_index->subif_weight = weight;
			goto CBM_RESOURCES_CLEANUP;
		}
		
		if(tmu_hal_get_q_idx_of_same_prio(port_subif_index, port_sched, port_prio_level, &q_index, &uIndex, &sched_id, flags) != TMU_HAL_STATUS_OK) {
			tmu_hal_find_sched_in_for_q_add(port_subif_index, port_sched, port_prio_level, &shift_idx );
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>shift_idx:%d\n",__FUNCTION__,shift_idx);
			tmu_hal_shift_down_sbin_conn(netdev, dev_name, tmuport, port_subif_index, port_sched, shift_idx, flags);
			port_prio_level = shift_idx;
			new_port_omid = (subif_index->port_sched_in_egress & ~7) | port_prio_level;
			new_omid = new_port_omid;
		} else {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Connect the logical interface to User Index:%d Scheduler Block: %d\n",
					__FUNCTION__, uIndex, sched_id);
			port_prio_level = uIndex;
			new_port_omid = (subif_index->port_sched_in_egress & ~7) | port_prio_level;

			tmu_sched_blk_in_link_get(new_port_omid, &ilink);
			if(ilink.sie != 0 ) { /*no queue connected to this leaf */
				if(ilink.sit == 0) /* input is QID*/ {
					struct tmu_hal_equeue_create q_reconnect;
					QOS_RATE_SHAPING_CFG cfg;
					uint32_t cfg_shaper;

					/* Check whether the same type of queue is connected to this leaf */	
					/* If not then return TMU_HAL_STATUS_ERR */
					q_index = ilink.qsid; /* this is the QID */

					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is attached to this Queue\n",__FUNCTION__,tmu_hal_queue_track[q_index].tb_index);
					cfg_shaper = tmu_hal_queue_track[q_index].tb_index;
					if(cfg_shaper != 0xFF) {
						memset(&cfg, 0, sizeof(QOS_RATE_SHAPING_CFG));
						cfg.shaperid = cfg_shaper;
						cfg.dev_name = dev_name;
						cfg.flag = flags;
						tmu_hal_del_queue_rate_shaper_ex(netdev, &cfg, q_index, flags);
					}

					/* Delete the Queue */
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Already queue %d is attached to this leaf\n",q_index);
					tmu_hal_safe_queue_delete(netdev, dev_name, q_index, -1, -1, flags);

					/* Create a WFQ scheduler and attach to the leaf of Scheduler */
					new_sched = create_new_scheduler(sched_id, 0, TMU_HAL_POLICY_STRICT, tmu_hal_sched_track[sched_id].level+1, port_prio_level);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"New Scheduler ID is %d\n",new_sched);
					tmu_hal_sched_track[new_sched].policy = TMU_HAL_POLICY_WFQ;

					/* Add the deleted queue to the WFQ scheduler */
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Reconnecting the queue %d to scheduler %d\n",q_index, new_sched);
					memset(&q_reconnect, 0, sizeof(struct tmu_hal_equeue_create));
					q_reconnect.index = q_index;				
					q_reconnect.egress_port_number = tmuport;
					q_reconnect.scheduler_input =	new_sched << 3 | 0; /*highest priority leaf */
					q_reconnect.iwgt = (tmu_hal_queue_track[q_index].prio_weight == 0)?1:tmu_hal_queue_track[q_index].prio_weight ;
					tmu_hal_egress_queue_create(&q_reconnect);
					if(cfg_shaper != 0xFF) {
						memset(&cfg, 0, sizeof(QOS_RATE_SHAPING_CFG));
						cfg.shaperid = cfg_shaper;
						cfg.dev_name = dev_name;
						cfg.flag = flags;
						tmu_hal_add_queue_rate_shaper_ex(netdev, &cfg, q_index, flags);
					}
					new_omid = (new_sched << 3 | 1);

				} else {
					uint32_t free_wfq_leaf;
					tmu_hal_free_wfq_scheduler_get(tmuport, ilink.qsid, &free_wfq_leaf);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Free leaf is %d\n",__FUNCTION__, free_wfq_leaf);
					new_omid =	free_wfq_leaf;
				}
			}
		}

		subif_index->port_sched_in_user_lvl = user_prio;
		base_sched = subif_index->base_sched_id_egress;
		omid = tmu_hal_sched_track[base_sched].omid;
		if((omid >> 3 )== subif_index->port_shaper_sched) {
			base_sched = subif_index->port_shaper_sched;
			omid = tmu_hal_sched_track[base_sched].omid;
		} 

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Subif_index: base_sched_id_egress: %d port_sched_in_egress: %d OMID: %d\n",
				__FUNCTION__,base_sched, subif_index->port_sched_in_egress, omid);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Adding the Sub interface to the leaf %d\n",__FUNCTION__,port_prio_level);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New OMID %d\n",__FUNCTION__, new_omid);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New Port OMID %d\n",__FUNCTION__, new_port_omid);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base Scheduler omid %d port_sched_in_egress= %d\n",__FUNCTION__, omid, subif_index->port_sched_in_egress );


		tmu_sched_blk_in_enable(omid, 0);
		tmu_hal_sched_track[omid >> 3].leaf_mask &= ~(1<< (omid & 7) );
		tmu_hal_sched_out_remap(base_sched, tmu_hal_sched_track[new_omid >> 3].level+1, new_omid, 1, (weight == 0)?1:weight);
		subif_index->subif_weight = weight;
		if(omid == subif_index->port_sched_in_egress)
		{
			tmu_hal_shift_up_sbin_conn(netdev, dev_name, tmuport, port_subif_index, subif_index->port_sched_in_egress, flags);
		} else {
			tmu_hal_shift_up_sbin_conn(netdev, dev_name, tmuport, port_subif_index, omid, flags);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler leaf mask: %d\n",__FUNCTION__, tmu_hal_sched_track[omid >> 3].leaf_mask);
			if(tmu_hal_sched_track[omid >> 3].leaf_mask == 0) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Nothing connected to any leaf. Delete scheduler %d\n",__FUNCTION__, (omid >> 3));
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Omid: %d\n",__FUNCTION__, tmu_hal_sched_track[omid >> 3].omid);				
				tmu_sched_blk_in_enable(tmu_hal_sched_track[omid >> 3].omid, 0);
				tmu_hal_scheduler_delete(omid >> 3);
				tmu_hal_shift_up_sbin_conn(netdev, dev_name, tmuport, port_subif_index, subif_index->port_sched_in_egress, flags);
			}
		}
		subif_index->port_sched_in_egress = new_port_omid;
	}

CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
	kfree(dp_subif);
	return TMU_HAL_STATUS_OK;
}

int tmu_hal_get_tmu_res_from_netdevice(
		struct net_device *netdev,
		char *dev_name, 
		uint32_t sub_if, 
		dp_subif_t *dp_if, 
		uint32_t *tmu_ports_cnt, 
		cbm_tmu_res_t **res, 
		uint32_t flags)
{
	uint32_t i=0;
	uint32_t nof_of_tmu_ports;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	PPA_VCC *vcc = NULL; 
	uint32_t flag = 0;

	TMU_ALLOC(dp_subif_t, dp_subif);

#ifdef TMU_HAL_TEST
	uint32_t port;
	port = dp_subif->port_id = 15;
	dp_subif->subif = sub_if;
#endif
	if((flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) {
		if((flags & DP_F_MPE_ACCEL) == DP_F_MPE_ACCEL)
			flag = DP_F_MPE_ACCEL;
		
	} else {
		if(netdev) {

			ppa_br2684_get_vcc(netdev,&vcc);
			if(vcc == NULL)
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"VCC is NULL\n");	
			if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
				/*TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);*/
				if(dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
					/* TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n",
						__FUNCTION__, __LINE__);*/
					if(tmu_hal_get_subitf_via_ifname(dev_name,dp_subif) != PPA_SUCCESS) {
						/* TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n",
						__FUNCTION__, __LINE__); */
						kfree(dp_subif);
						return TMU_HAL_STATUS_ERR;
					}
				}	
			}

			dp_if->port_id = dp_subif->port_id;
			dp_if->subif = dp_subif->subif >> 8;
		} else {
			dp_subif->port_id = dp_if->port_id;
			dp_subif->subif = dp_if->subif;
		}
	}
	dp_subif->subif	= dp_subif->subif >> 8;

	if(cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, flag) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}

/*	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> For Datapath Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
			__FUNCTION__,dp_subif.port_id,dp_subif.subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);*/

	*tmu_ports_cnt = nof_of_tmu_ports;
	*res = tmu_res;

	for(i=0; i<nof_of_tmu_ports; i++) {
		if(tmu_hal_port_track[(tmu_res+i)->tmu_port].default_sched_id == 0xFF) {
/*			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
					__FUNCTION__, dp_subif.port_id,dp_subif.subif,(tmu_res + i)->tmu_port,(tmu_res+i)->tmu_sched,(tmu_res+i)->tmu_q);*/
			if((flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) {
				(tmu_hal_user_sub_interface_ingress_info + 
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->is_logical = (dp_subif->subif == 0) ? 0 : 1;
				(tmu_hal_user_sub_interface_ingress_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->tmu_port_idx = tmu_res->tmu_port;
				(tmu_hal_user_sub_interface_ingress_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->base_sched_id_egress = tmu_res->tmu_sched;
				(tmu_hal_user_sub_interface_ingress_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->user_queue[0].queue_index = tmu_res->tmu_q;
				(tmu_hal_user_sub_interface_ingress_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->user_queue[0].queue_type = TMU_HAL_POLICY_STRICT;
				(tmu_hal_user_sub_interface_ingress_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->user_queue[0].qid = 0;
				(tmu_hal_user_sub_interface_ingress_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->user_queue[0].prio_level = 0;
				(tmu_hal_user_sub_interface_ingress_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->user_queue[0].sbin = tmu_res->tmu_sched << 3;
				(tmu_hal_user_sub_interface_ingress_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->user_queue[0].qmap[MAX_TC_NUM-1] = 0;
				(tmu_hal_user_sub_interface_ingress_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->qid_last_assigned = 0;
				(tmu_hal_user_sub_interface_ingress_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->queue_cnt++;

			} else {
				uint32_t j;	
				/** For the physical port */
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->is_logical = 0 ; 
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->tmu_port_idx = (tmu_res+i)->tmu_port; 
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->base_sched_id_egress = (tmu_res+i)->tmu_sched; 
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->user_queue[0].queue_index = (tmu_res+i)->tmu_q; 
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->user_queue[0].queue_type = TMU_HAL_POLICY_STRICT; 
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->user_queue[0].prio_level = 16; 
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->user_queue[0].qid = 0; 
				(tmu_hal_user_sub_interface_info + 
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->user_queue[0].sbin = (tmu_res+i)->tmu_sched << 3; 
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->user_queue[0].qmap[MAX_TC_NUM-1] = 0; 
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->qid_last_assigned = 0; 
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->default_q = (tmu_res+i)->tmu_q; 
				(tmu_hal_user_sub_interface_info +
					((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) )->queue_cnt++; 

				/** For all the sub-interface*/
				for(j=1; j< TMU_HAL_MAX_SUB_IFID_PER_PORT; j++) {
					(tmu_hal_user_sub_interface_info +
						((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + j)->is_logical = 1; 
					(tmu_hal_user_sub_interface_info +
						((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + j)->tmu_port_idx = (tmu_res+i)->tmu_port; 
					(tmu_hal_user_sub_interface_info +
						((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + j)->base_sched_id_egress = 0xFF; 
					(tmu_hal_user_sub_interface_info +
						((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + j)->port_sched_in_user_lvl = TMU_HAL_MAX_PRIORITY_LEVEL; 
					(tmu_hal_user_sub_interface_info +
						((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + j)->port_sched_in_egress = NULL_SCHEDULER_INPUT_ID; 
					(tmu_hal_user_sub_interface_info +
						((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + j)->port_shaper_sched = INVALID_SCHED_ID; 
					(tmu_hal_user_sub_interface_info +
						((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + j)->shaper_idx = INVALID_SHAPER_ID; 
					(tmu_hal_user_sub_interface_info +
						((tmu_res + i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + j)->default_q = 0; 

				}

			}
			/** Update the Port info*/
			tmu_hal_port_track[(tmu_res+i)->tmu_port].is_enabled = 1;
			tmu_hal_port_track[(tmu_res+i)->tmu_port].input_sched_id = (tmu_res+i)->tmu_sched;
			tmu_hal_port_track[(tmu_res+i)->tmu_port].default_sched_id = (tmu_res+i)->tmu_sched;
			tmu_hal_port_track[(tmu_res+i)->tmu_port].shaper_sched_id = 0xFF;
			tmu_hal_port_track[(tmu_res+i)->tmu_port].port_sched_id = 0xFF;
			tmu_hal_port_track[(tmu_res+i)->tmu_port].no_of_queues = 1;

			/** Update the Scheduler info*/
			tmu_hal_sched_track[(tmu_res+i)->tmu_sched].in_use = 1;
			tmu_hal_sched_track[(tmu_res+i)->tmu_sched].id = (tmu_res+i)->tmu_sched;
			tmu_hal_sched_track[(tmu_res+i)->tmu_sched].level = 0;
			tmu_hal_sched_track[(tmu_res+i)->tmu_sched].policy = TMU_HAL_POLICY_STRICT;
			tmu_hal_sched_track[(tmu_res+i)->tmu_sched].leaf_mask = 0x01;
			tmu_hal_sched_track[(tmu_res+i)->tmu_sched].next_sched_id = 0xFF;
			tmu_hal_sched_track[(tmu_res+i)->tmu_sched].peer_sched_id = 0xFF;
			tmu_hal_sched_track[(tmu_res+i)->tmu_sched].sched_out_conn = 0; /* default connected to the port */
			tmu_hal_sched_track[(tmu_res+i)->tmu_sched].omid = (tmu_res+i)->tmu_port;
			tmu_hal_sched_track[(tmu_res+i)->tmu_sched].tbs = 0xFF;


			if(flag == DP_F_MPE_ACCEL) {
				tmu_hal_sched_track[(tmu_res+i)->tmu_sched].policy = TMU_HAL_POLICY_WFQ;
			}

			/** Update the Queue info*/
			tmu_hal_queue_track[(tmu_res+i)->tmu_q].is_enabled = 1;
			tmu_hal_queue_track[(tmu_res+i)->tmu_q].connected_to_sched_id = (tmu_res+i)->tmu_sched;
			tmu_hal_queue_track[(tmu_res+i)->tmu_q].epn = (tmu_res+i)->tmu_port;
			tmu_hal_queue_track[(tmu_res+i)->tmu_q].q_type = TMU_HAL_POLICY_STRICT;
			tmu_hal_queue_track[(tmu_res+i)->tmu_q].prio_weight = 0;
			tmu_hal_queue_track[(tmu_res+i)->tmu_q].sched_input = ((tmu_res+i)->tmu_sched << 3);
		} /* loop for .default_sched_id=0xFF */
	} /* loop for no_of_tmu_ports*/
	kfree(dp_subif);
	return TMU_HAL_STATUS_OK;
}

int32_t tmu_hal_get_ingress_index(uint32_t flags)
{
	int32_t index = TMU_HAL_STATUS_ERR;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> flags %x\n ",__FUNCTION__,flags);
	if((flags & PPA_QOS_Q_F_INGGRP1) == PPA_QOS_Q_F_INGGRP1)
		index = 5; /*PPA_QOS_Q_F_INGGRP1 >> 7;*/
	if((flags & PPA_QOS_Q_F_INGGRP2) == PPA_QOS_Q_F_INGGRP2)
		index = 6; /*PPA_QOS_Q_F_INGGRP2 >> 7;*/
	if((flags & PPA_QOS_Q_F_INGGRP3) == PPA_QOS_Q_F_INGGRP3)
		index = 7; /*PPA_QOS_Q_F_INGGRP3 >> 7;*/
	if((flags & PPA_QOS_Q_F_INGGRP4) == PPA_QOS_Q_F_INGGRP4)
		index = 8; /*PPA_QOS_Q_F_INGGRP4 >> 7;*/
	if((flags & PPA_QOS_Q_F_INGGRP5) == PPA_QOS_Q_F_INGGRP5)
		index = 9; /*PPA_QOS_Q_F_INGGRP5 >> 7;*/
	if((flags & PPA_QOS_Q_F_INGGRP6) == PPA_QOS_Q_F_INGGRP6)
		index = 10; /*PPA_QOS_Q_F_INGGRP6 >> 7;*/

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Group index is %d\n ",__FUNCTION__,index);
	return index;
}

int32_t tmu_hal_get_mpe_ingress_index(uint32_t flags)
{
	int32_t index = TMU_HAL_STATUS_ERR;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> flags %x\n ",__FUNCTION__,flags);
	if((flags & PPA_QOS_Q_F_INGGRP1) == PPA_QOS_Q_F_INGGRP1)
		index = 10; /*PPA_QOS_Q_F_INGGRP1 >> 7;*/
	if((flags & PPA_QOS_Q_F_INGGRP2) == PPA_QOS_Q_F_INGGRP2)
		index = 11; /*PPA_QOS_Q_F_INGGRP2 >> 7;*/
	if((flags & PPA_QOS_Q_F_INGGRP3) == PPA_QOS_Q_F_INGGRP3)
		index = 12; /*PPA_QOS_Q_F_INGGRP3 >> 7;*/
	if((flags & PPA_QOS_Q_F_INGGRP4) == PPA_QOS_Q_F_INGGRP4)
		index = 13; /*PPA_QOS_Q_F_INGGRP4 >> 7;*/
	if((flags & PPA_QOS_Q_F_INGGRP5) == PPA_QOS_Q_F_INGGRP5)
		index = 14; /*PPA_QOS_Q_F_INGGRP5 >> 7;*/
	if((flags & PPA_QOS_Q_F_INGGRP6) == PPA_QOS_Q_F_INGGRP6)
		index = 15; /*PPA_QOS_Q_F_INGGRP6 >> 7;*/

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Group index is %d\n ",__FUNCTION__,index);
	return index;
}

int32_t tmu_hal_set_mpe_ingress_grp_qmap(struct tmu_hal_user_subif_abstract *subif_index, uint32_t prio_level, uint32_t flags, uint32_t q_index, char *tc , uint8_t no_of_tc)
{
	cbm_queue_map_entry_t q_map;
	int32_t q_map_mask = 0, x=0, i=0;

	/** Configure the QMAP table to connect to the Egress queue*/
	memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));

	q_map.ep = 0;
	q_map.tc = *tc;

	if((flags & PPA_QOS_Q_F_INGGRP1) == PPA_QOS_Q_F_INGGRP1) {
		q_map.flowid = 0;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 1;
		q_map.mpe2 = 0;
	}

	if((flags & PPA_QOS_Q_F_INGGRP2) == PPA_QOS_Q_F_INGGRP2) {
		q_map.flowid = 0;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 1;
		q_map.mpe2 = 1;
	}

	if((flags & PPA_QOS_Q_F_INGGRP3) == PPA_QOS_Q_F_INGGRP3) {
		q_map.flowid = 1;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 1;
		q_map.mpe2 = 0;
	}

	if((flags & PPA_QOS_Q_F_INGGRP4) == PPA_QOS_Q_F_INGGRP4) {
		q_map.flowid = 1;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 1;
		q_map.mpe2 = 1;
	}

	if((flags & PPA_QOS_Q_F_INGGRP5) == PPA_QOS_Q_F_INGGRP5) {
		q_map.flowid = 1;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 1;
		q_map.mpe2 = 1;
	}

	if((flags & PPA_QOS_Q_F_INGGRP6) == PPA_QOS_Q_F_INGGRP6) {
		q_map.flowid = 0;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 1;
		q_map.mpe2 = 1;
	}

	if(prio_level == subif_index->default_prio ) { 
		/*default Q of Physical interface */

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Map Default Q: %d\n",__FUNCTION__,q_index);
		q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE ;
		cbm_queue_map_set(g_Inst, q_index, &q_map, q_map_mask);
		subif_index->default_q = q_index;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Queue count: %d\n",__FUNCTION__, subif_index->queue_cnt);

		/* Updating qmap and no_of_tc of the default Q */
		if(subif_index->queue_cnt < TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE) {
			for(i=0; i< no_of_tc; i++)
			{
				q_map.tc = *(tc+i);
				subif_index->user_queue[subif_index->queue_cnt].qmap[i] =*(tc+i); //tc[i];
			}
			subif_index->user_queue[subif_index->queue_cnt].no_of_tc = no_of_tc;
		}
		for(x=0; x < subif_index->queue_cnt && subif_index->queue_cnt<=TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE; x++)
		{
			q_map_mask = 0;
			/** Configure the QMAP table to connect to the Ingress queue*/

			q_map.ep = 0;
			for(i=0; i< subif_index->user_queue[x].no_of_tc; i++) {
				q_map.tc = subif_index->user_queue[x].qmap[i];
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Deafult Q --> Add QMAP for Port %d and TC: %d\n",
						__FUNCTION__,q_map.ep,q_map.tc);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Default Q --> User Index %d Q Index: %d\n",
						__FUNCTION__, x, subif_index->user_queue[x].queue_index);
				cbm_queue_map_set(g_Inst, subif_index->user_queue[x].queue_index, &q_map, q_map_mask);
			}

		}
	} else { /* not for default q*/
		/** Configure the QMAP table to connect to the Ingress queue*/
		q_map.ep = 0;
		if(subif_index->queue_cnt < TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE) {
			for(i=0; i< no_of_tc; i++) {
				q_map.tc = *(tc+i);

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add QMAP for Port %d and TC: %d\n",__FUNCTION__,q_map.ep,q_map.tc);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Index of New Q: %d\n",__FUNCTION__, q_index);
				cbm_queue_map_set(g_Inst, q_index, &q_map, q_map_mask);
				subif_index->user_queue[subif_index->queue_cnt].qmap[i] =*(tc+i);
			}	
			subif_index->user_queue[subif_index->queue_cnt].no_of_tc = no_of_tc;
		}
	}

	return TMU_HAL_STATUS_OK;

}

int32_t tmu_hal_set_ingress_grp_qmap(struct tmu_hal_user_subif_abstract *subif_index, uint32_t prio_level, uint32_t flags, uint32_t q_index, char *tc , uint8_t no_of_tc)
{
	cbm_queue_map_entry_t q_map;
		int32_t q_map_mask = 0, x=0, i=0;

	/** Configure the QMAP table to connect to the Egress queue*/
	memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));

	q_map.ep = 0;
	q_map.tc = *tc;

	if((flags & PPA_QOS_Q_F_INGGRP1) == PPA_QOS_Q_F_INGGRP1) {
		q_map.flowid = 0;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 0;
		q_map.mpe2 = 0;
	}

	if((flags & PPA_QOS_Q_F_INGGRP2) == PPA_QOS_Q_F_INGGRP2) {
		q_map.flowid = 0;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 0;
		q_map.mpe2 = 1;
	}

	if((flags & PPA_QOS_Q_F_INGGRP3) == PPA_QOS_Q_F_INGGRP3) {
		q_map.flowid = 1;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 0;
		q_map.mpe2 = 0;
	}

	if((flags & PPA_QOS_Q_F_INGGRP4) == PPA_QOS_Q_F_INGGRP4) {
		q_map.flowid = 1;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 0;
		q_map.mpe2 = 1;
	}

	if((flags & PPA_QOS_Q_F_INGGRP5) == PPA_QOS_Q_F_INGGRP5) {
		q_map.flowid = 1;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 1;
		q_map.mpe2 = 1;
	}

	if((flags & PPA_QOS_Q_F_INGGRP6) == PPA_QOS_Q_F_INGGRP6) {
		q_map.flowid = 0;
		q_map.enc = 0;
		q_map.dec = 0;
		q_map.mpe1 = 1;
		q_map.mpe2 = 1;
	}

	if(prio_level == subif_index->default_prio )  { 
		/*default Q of Physical interface */

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Map Default Q: %d\n",__FUNCTION__,q_index);
		q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE ;
		cbm_queue_map_set(g_Inst, q_index, &q_map, q_map_mask);
		subif_index->default_q = q_index;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Queue count: %d\n",__FUNCTION__, subif_index->queue_cnt);

		/* Updating qmap and no_of_tc of the default Q */
		if(subif_index->queue_cnt < TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE) {
			for(i=0; i< no_of_tc; i++)
			{
				q_map.tc = *(tc+i);
				subif_index->user_queue[subif_index->queue_cnt].qmap[i] =*(tc+i); //tc[i];
			}
			subif_index->user_queue[subif_index->queue_cnt].no_of_tc = no_of_tc;
		}
		for(x=0; x < subif_index->queue_cnt && subif_index->queue_cnt<=TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE; x++)
		{
			q_map_mask = 0;
			/** Configure the QMAP table to connect to the Ingress queue*/

			q_map.ep = 0;
			for(i=0; i< subif_index->user_queue[x].no_of_tc; i++) {
				q_map.tc = subif_index->user_queue[x].qmap[i];
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Deafult Q --> Add QMAP for Port %d and TC: %d\n",
						__FUNCTION__,q_map.ep,q_map.tc);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Default Q --> User Index %d Q Index: %d\n",
						__FUNCTION__, x, subif_index->user_queue[x].queue_index);
				cbm_queue_map_set(g_Inst, subif_index->user_queue[x].queue_index, &q_map, q_map_mask);
			}

		}
	} else {
		/** Configure the QMAP table to connect to the Ingress queue*/

		q_map.ep = 0;
		if(subif_index->queue_cnt < TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE) {
			for(i=0; i< no_of_tc; i++) {
				q_map.tc = *(tc+i);

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add QMAP for Port %d and TC: %d\n",__FUNCTION__,q_map.ep,q_map.tc);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Index of New Q: %d\n",__FUNCTION__, q_index);
				cbm_queue_map_set(g_Inst, q_index, &q_map, q_map_mask);
				subif_index->user_queue[subif_index->queue_cnt].qmap[i] =*(tc+i); //tc[i];
			}
			subif_index->user_queue[subif_index->queue_cnt].no_of_tc = no_of_tc;
		}
	}

	return TMU_HAL_STATUS_OK;

}
int tmu_hal_add_ingress_queue(
		struct net_device *netdev, 
		char *dev_name, 
		char *tc,
		uint8_t no_of_tc,
		uint32_t schedid, 
		uint32_t prio_type, 
		uint32_t prio_level, 
		uint32_t weight, 
		uint32_t flowId,
		QOS_Q_ADD_CFG *params, 
		uint32_t flags)
{
	uint32_t i=0, base_sched, qid_last_assigned = 0;
	uint32_t nof_of_tmu_ports;
	cbm_tmu_res_t *tmu_res=NULL;
	dp_subif_t *dp_subif = NULL;
	int32_t tmuport;
	int32_t ret = TMU_HAL_STATUS_OK;
	struct tmu_hal_equeue_cfg q_param;
	struct tmu_equeue_drop_params thx;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);
	
	TMU_ALLOC(dp_subif_t, dp_subif);
	if((flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) {
#ifdef TMU_HAL_TEST
		port = dp_subif->port_id = 15;
		dp_subif->subif = schedid;
#endif 
		if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
			kfree(dp_subif);
			kfree(tmu_res);
			return TMU_HAL_STATUS_ERR;
		}
		dp_subif->subif	= dp_subif->subif >> 8;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> CPU Ingress --> For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d No Of TMU ports: %d\n",
				__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q,nof_of_tmu_ports);
		tmuport = tmu_res->tmu_port;
		kfree(tmu_res);
	}

	if(( tmuport = (tmu_hal_get_ingress_index(flags))) == TMU_HAL_STATUS_ERR) {
		ret = PPA_FAILURE;
		goto CBM_RESOURCES_CLEANUP;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Group index is %d\n ",__FUNCTION__,tmuport);

	if((flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) {
		struct tmu_hal_user_subif_abstract *ingress_subif_index = NULL;
		struct tmu_hal_user_subif_abstract *subif_index = NULL;
		struct tmu_hal_user_subif_abstract *port_index = NULL;
		struct tmu_hal_user_subif_abstract *subif_index_port = NULL;
		cbm_tmu_res_t *cpu_tmu_res = NULL;
		uint32_t nof_of_cpu_tmu_ports;

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add Ingress Queue for Datapath Port %d\n",__FUNCTION__, dp_subif->port_id);

		for(i=0; i<nof_of_tmu_ports; i++) {
			(tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->is_logical =
				(dp_subif->subif == 0) ? 0 : 1;
			(tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->tmu_port_idx = tmuport;
		}

		ingress_subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Ingress Sub interface index %p\n",__FUNCTION__,ingress_subif_index);

		if(!g_IngressQueueScheduler) {
			uint32_t old_omid, root_omid, sched_port, q_new;
			uint32_t q_index = 0xFF;
			struct tmu_hal_equeue_create q_reconnect;
			struct tmu_hal_sched_track_info Snext;
			cbm_queue_map_entry_t *q_map_get = NULL;
			int32_t no_entries =0;

#ifdef TMU_HAL_TEST
			dp_subif->port_id = 0;
			dp_subif->subif = 0;
#endif
			if(tmu_hal_get_tmu_res_from_netdevice(netdev, netdev->name, schedid, dp_subif,
					&nof_of_cpu_tmu_ports, &cpu_tmu_res, (flags & (~DP_F_MPE_ACCEL | PPA_QOS_Q_F_INGRESS))) == TMU_HAL_STATUS_ERR) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"%s:%d Error: Failed to get resources from Netdevice\n", __FUNCTION__, __LINE__);
				kfree(dp_subif);
				kfree(cpu_tmu_res);
				return TMU_HAL_STATUS_ERR;
			}

			/*Create the WFQ Root Scheduler*/
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," Create WFQ Scheduler for CPU Ingress Queue\n");

			old_omid = tmu_hal_port_track[g_CPU_PortId].input_sched_id;
			Snext = tmu_hal_sched_track[old_omid];

			q_index = cpu_tmu_res->tmu_q; /* this is the QID */
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Already queue %d is attached to this leaf\n",q_index);

			if (cbm_queue_map_get(g_Inst, q_index, &no_entries, &q_map_get, 0) == TMU_HAL_STATUS_ERR) {
				TMU_HAL_DEBUG_ERR(TMU_DEBUG_ERR, "CBM FAILURE: Queue %d could not be mapped properly !!\n", q_index);
			}
			tmu_equeue_drop_params_get(q_index, &thx);
			tmu_hal_dump_egress_queue_drop_params(thx);
			tmu_hal_safe_queue_delete(netdev, netdev->name, q_index, -1, -1, flags);

			root_omid = create_new_scheduler(old_omid, 0, TMU_HAL_POLICY_WFQ, Snext.level+1, 1);
			tmu_hal_sched_track[old_omid].next_sched_id = root_omid;
			tmu_hal_sched_track[root_omid].policy = TMU_HAL_POLICY_WFQ;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Configuring	Scheduler id of SB %d -->SB %d \n",root_omid, Snext.id);

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Reconnecting the queue %d to scheduler %d\n",q_index, root_omid);
			memset(&q_reconnect, 0, sizeof(struct tmu_hal_equeue_create));
			q_reconnect.index = q_index;				
			q_reconnect.egress_port_number = cpu_tmu_res->tmu_port;
			q_reconnect.scheduler_input =	root_omid << 3 | 0; /*highest priority leaf */
			q_reconnect.iwgt = (tmu_hal_queue_track[q_index].prio_weight == 0)?1:tmu_hal_queue_track[q_index].prio_weight ;
			tmu_hal_egress_queue_create(&q_reconnect);

			tmu_equeue_drop_params_set(q_index, &thx);

			g_Root_sched_id_Ingress = root_omid;

			udelay(20000);
			if (no_entries > 0) {
				int32_t j;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Q map pointer =%p\n", q_map_get);
				for (j=0; j<no_entries;j++)
					cbm_queue_map_set(g_Inst, q_index, &q_map_get[j], 0);
				no_entries = 0;
			}
			kfree(q_map_get);
			q_map_get = NULL;

			/* Now create the Port scheduler for the interface */
			sched_port = create_new_scheduler(g_Root_sched_id_Ingress, 0, TMU_HAL_POLICY_WFQ, tmu_hal_sched_track[root_omid].level + 1, 1);
			tmu_hal_sched_track[sched_port].policy = TMU_HAL_POLICY_STRICT;
			q_new = tmu_hal_queue_add(netdev, netdev->name, cpu_tmu_res->tmu_port,sched_port, prio_type, prio_level, weight, ingress_subif_index, flags);
			/* Set the threshold values for the default ingress queue, values are obtained from the queue configuration */
			memset(&q_param, 0, sizeof(struct tmu_hal_equeue_cfg));
			q_param.index = q_new;
			q_param.enable = 1;
			if(params->drop.mode == PPA_QOS_DROP_WRED) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify WRED params \n",__FUNCTION__);
				q_param.drop_threshold_green_min = params->drop.wred.min_th0;
				q_param.drop_threshold_green_max = params->drop.wred.max_th0;
				q_param.drop_threshold_yellow_min = params->drop.wred.min_th1;
				q_param.drop_threshold_yellow_max = params->drop.wred.max_th1;
				q_param.drop_probability_green = params->drop.wred.max_p0;
				q_param.drop_probability_yellow = params->drop.wred.max_p1;
				q_param.wred_enable = 1;
				q_param.avg_weight = params->drop.wred.weight;
			} else if (params->drop.mode == PPA_QOS_DROP_TAIL) {
				q_param.wred_enable = 0;
				q_param.drop_threshold_green_max = params->drop.wred.max_th0;
				q_param.drop_threshold_yellow_max = params->drop.wred.max_th1;
				q_param.drop_threshold_red = params->drop.wred.drop_th1;
			}
			tmu_hal_egress_queue_cfg_set(&q_param);

			port_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT); 
			port_index->base_sched_id_egress = sched_port;

			g_IngressQueueScheduler++;

			qid_last_assigned = ++(ingress_subif_index->qid_last_assigned);
			tmu_hal_queue_track[q_new].user_q_idx = ingress_subif_index->queue_cnt;

			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].queue_index = q_new;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].queue_type = prio_type;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].prio_level = prio_level;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].sbin = tmu_hal_queue_track[q_new].sched_input;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].qid = qid_last_assigned;

			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.drop_threshold_green_min = params->drop.wred.min_th0;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.drop_threshold_green_max = params->drop.wred.max_th0;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.drop_threshold_yellow_min = params->drop.wred.min_th1;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.drop_threshold_yellow_max = params->drop.wred.max_th1;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.drop_threshold_red = params->drop.wred.drop_th1;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.drop_probability_green = params->drop.wred.max_p0;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.drop_probability_yellow = params->drop.wred.max_p1;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.weight = params->drop.wred.weight;;
			ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.enable = 1;
			if(params->drop.mode == PPA_QOS_DROP_WRED) {
				ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.wred_enable = 1;
			} else if (params->drop.mode == PPA_QOS_DROP_TAIL) {
				ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].ecfg.wred_enable = 0;
			}

			ingress_subif_index->dp_subif.port_id = dp_subif->port_id;
			ingress_subif_index->dp_subif.subif = dp_subif->subif;
			if((flags & PPA_F_PPPOATM) == PPA_F_PPPOATM)
			{
				snprintf(ingress_subif_index->dp_subif.ifname, sizeof(ingress_subif_index->dp_subif.ifname),"%s", dev_name);
			}
			else
				snprintf(ingress_subif_index->dp_subif.ifname, sizeof(ingress_subif_index->dp_subif.ifname),"%s", netdev->name);

			if((flags & PPA_QOS_Q_F_DEFAULT) == PPA_QOS_Q_F_DEFAULT) {	
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Priority Level for Default Q : %d\n",prio_level);
				ingress_subif_index->default_prio = prio_level;
			}
			
			tmu_hal_set_ingress_grp_qmap(ingress_subif_index, prio_level, flags, q_new, tc, no_of_tc);
			ingress_subif_index->queue_cnt++ ;
			tmu_hal_dump_subif_queue_info(ingress_subif_index);

			/* Update SBIN weights of base scheduler for ingress */
			tmu_hal_update_ingress_sched_weight(g_Root_sched_id_Ingress);

			kfree(cpu_tmu_res);
		} else {
			/*Root Scheduler is already connected*/
			uint32_t q_new, sched_port;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Root Scheduler %d is already created\n",g_Root_sched_id_Ingress);
			port_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT); 
			if(port_index->base_sched_id_egress == 0xFF) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Create Port Scheduler for the port \n");
				sched_port = create_new_scheduler(g_Root_sched_id_Ingress, 0, TMU_HAL_POLICY_WFQ, tmu_hal_sched_track[g_Root_sched_id_Ingress].level + 1, 1);
				tmu_hal_sched_track[sched_port].policy = TMU_HAL_POLICY_STRICT;

				port_index->base_sched_id_egress = sched_port;
				for(i=0; i<TMU_HAL_MAX_SUB_IFID_PER_PORT; i++) {
					(port_index + i)->port_sched_in_egress = sched_port; 
				}
			}
			else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Port Scheduler for the port %d is %d\n",dp_subif->port_id, ingress_subif_index->port_sched_in_egress);
			}
			subif_index = ingress_subif_index;
			subif_index_port = port_index;
			if(dp_subif->subif != 0) {
				uint32_t port_prio_level;
				if(subif_index->queue_cnt == 0 && subif_index->base_sched_id_egress == 0xFF) {
					uint8_t leaf;
					uint32_t port_sched;
					tmu_hal_find_min_prio_level_of_port(subif_index_port, &port_sched, &port_prio_level);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Add First Queue of the subif %d to the Port Scheduler %d\n",dp_subif->subif,port_sched);
					subif_index->port_sched_in_user_lvl = port_prio_level;

					if (tmu_hal_scheduler_free_input_get(
								port_sched,
								&leaf) >= 0) 
						port_prio_level = leaf;

					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Adding the Sub interface to the leaf %d\n",port_prio_level);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"port_sched_in_egress= %d\n", subif_index->port_sched_in_egress );
					if(subif_index->port_sched_in_egress >> 3 == NULL_SCHEDULER_BLOCK_ID){
						tmu_hal_connect_subif_to_port(netdev, netdev->name, subif_index, subif_index_port, port_sched, port_prio_level, weight, flags);
					} else {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Base Scheduler is already there\n");	
					}
					base_sched = subif_index->base_sched_id_egress;

				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Minimum 1 Queue is already added for the subif %d to the Port Scheduler\n",
						dp_subif->subif);
					base_sched = subif_index->base_sched_id_egress;
				}
			} else {

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," For the main physical interface of the Port \n");
				base_sched = subif_index->base_sched_id_egress;
			}

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Base scheduler for the interface is %d \n",base_sched);

			q_new = tmu_hal_queue_add(netdev, netdev->name, g_CPU_PortId, base_sched, prio_type, prio_level, weight, subif_index, flags);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"User Queue/Prio Level/Weight --> %d/%d/%d is mapped to Queue index-->%d \n",
						subif_index->queue_cnt, prio_level, weight, q_new);

			/* Set the threshold values for the ingress queue, values are obtained from the queue configuration */
			memset(&q_param, 0, sizeof(struct tmu_hal_equeue_cfg));
			q_param.index = q_new;
			q_param.enable = 1;
			if(params->drop.mode == PPA_QOS_DROP_WRED) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify WRED params \n",__FUNCTION__);
				q_param.drop_threshold_green_min = params->drop.wred.min_th0;
				q_param.drop_threshold_green_max = params->drop.wred.max_th0;
				q_param.drop_threshold_yellow_min = params->drop.wred.min_th1;
				q_param.drop_threshold_yellow_max = params->drop.wred.max_th1;
				q_param.drop_probability_green = params->drop.wred.max_p0;
				q_param.drop_probability_yellow = params->drop.wred.max_p1;
				q_param.wred_enable = 1;
				q_param.avg_weight = params->drop.wred.weight;
			} else if (params->drop.mode == PPA_QOS_DROP_TAIL) {
				q_param.wred_enable = 0;
				q_param.drop_threshold_green_max = params->drop.wred.max_th0;
				q_param.drop_threshold_yellow_max = params->drop.wred.max_th1;
				q_param.drop_threshold_red = params->drop.wred.drop_th1;
			}
			tmu_hal_egress_queue_cfg_set(&q_param);

			qid_last_assigned = ++(subif_index->qid_last_assigned);
			tmu_hal_queue_track[q_new].user_q_idx = subif_index->queue_cnt;

			subif_index->user_queue[subif_index->queue_cnt].queue_index = q_new;
			subif_index->user_queue[subif_index->queue_cnt].queue_type = prio_type;
			subif_index->user_queue[subif_index->queue_cnt].prio_level = prio_level;
			subif_index->user_queue[subif_index->queue_cnt].weight = weight;
			subif_index->user_queue[subif_index->queue_cnt].sbin = tmu_hal_queue_track[q_new].sched_input;
			subif_index->user_queue[subif_index->queue_cnt].qid = qid_last_assigned;

			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_green_min = params->drop.wred.min_th0;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_green_max = params->drop.wred.max_th0;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_yellow_min = params->drop.wred.min_th1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_yellow_max = params->drop.wred.max_th1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_red = params->drop.wred.drop_th1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_probability_green = params->drop.wred.max_p0;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_probability_yellow = params->drop.wred.max_p1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.weight = params->drop.wred.weight;;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.enable = 1;
			if(params->drop.mode == PPA_QOS_DROP_WRED) {
				subif_index->user_queue[subif_index->queue_cnt].ecfg.wred_enable = 1;
			} else if (params->drop.mode == PPA_QOS_DROP_TAIL) {
				subif_index->user_queue[subif_index->queue_cnt].ecfg.wred_enable = 0;
			}

			subif_index->netdev = netdev ;
			subif_index->dp_subif.port_id = dp_subif->port_id;
			subif_index->dp_subif.subif = dp_subif->subif;
			if((flags & PPA_F_PPPOATM) == PPA_F_PPPOATM) {
				snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname),"%s", dev_name);
			} else
				snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname),"%s", netdev->name);

			if((flags & PPA_QOS_Q_F_DEFAULT) == PPA_QOS_Q_F_DEFAULT) {	
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Priority Level for Default Q : %d\n",prio_level);
				subif_index->default_prio = prio_level;
			}
			tmu_hal_set_ingress_grp_qmap(subif_index, prio_level, flags, q_new, tc, no_of_tc);
			subif_index->queue_cnt++ ;
			tmu_hal_dump_subif_queue_info(subif_index);

			/* Update SBIN weights of base scheduler for ingress */
			tmu_hal_update_ingress_sched_weight(g_Root_sched_id_Ingress);
		}
		
	}
CBM_RESOURCES_CLEANUP:
	kfree(dp_subif);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return qid_last_assigned ;

}

int tmu_hal_add_mpe_ingress_queue(
		struct net_device *netdev, 
		char *dev_name, 
		char *tc,
		uint8_t no_of_tc,
		uint32_t schedid, 
		uint32_t prio_type, 
		uint32_t prio_level, 
		uint32_t weight, 
		uint32_t flowId,
		QOS_Q_ADD_CFG *params, 
		uint32_t flags)
{
	uint32_t i=0, base_sched, qid_last_assigned = 0;
	uint32_t nof_of_tmu_ports=0;
	cbm_tmu_res_t *cpu_tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	int32_t tmuport;
	struct tmu_egress_port_thr epth;
	int32_t ret = TMU_HAL_STATUS_OK;
	struct tmu_hal_equeue_cfg q_param;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);
	
	TMU_ALLOC(dp_subif_t, dp_subif);
	if(( tmuport = (tmu_hal_get_mpe_ingress_index(flags))) == TMU_HAL_STATUS_ERR) {
		ret = PPA_FAILURE;
		goto CBM_RESOURCES_CLEANUP;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Group index is %d\n ",__FUNCTION__,tmuport);

	if((flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS)
	{
		struct tmu_hal_user_subif_abstract *ingress_subif_index = NULL;
		struct tmu_hal_user_subif_abstract *subif_index = NULL;
		struct tmu_hal_user_subif_abstract *port_index = NULL;
		struct tmu_hal_user_subif_abstract *subif_index_port = NULL;
		uint32_t nof_of_cpu_tmu_ports;
		uint32_t q_new, sched_port;

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add Ingress Queue for Datapath Port %d\n",__FUNCTION__, dp_subif->port_id);

		if(tmu_hal_get_tmu_res_from_netdevice(netdev, dev_name, schedid, dp_subif, &nof_of_cpu_tmu_ports, &cpu_tmu_res, (flags | PPA_QOS_Q_F_INGRESS | DP_F_MPE_ACCEL)) == TMU_HAL_STATUS_ERR) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"%s:%d Error: Failed to get resources from Netdevice\n", __FUNCTION__, __LINE__);
			kfree(cpu_tmu_res);
			kfree(dp_subif);
			return TMU_HAL_STATUS_ERR;
		}
		for(i=0; i<nof_of_tmu_ports; i++) {
			(tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->is_logical =
				(dp_subif->subif == 0) ? 0 : 1;
			(tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->tmu_port_idx = tmuport;
		}

		ingress_subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Ingress Sub interface index %p\n",__FUNCTION__,ingress_subif_index);

		port_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT); 
		if(port_index->base_sched_id_egress == 0xFF) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Create Port Scheduler for the port \n");
			sched_port = create_new_scheduler(cpu_tmu_res->tmu_sched, 0, TMU_HAL_POLICY_WFQ, tmu_hal_sched_track[cpu_tmu_res->tmu_sched].level + 1, 1);
			tmu_hal_sched_track[sched_port].policy = TMU_HAL_POLICY_STRICT;

			port_index->base_sched_id_egress = sched_port;
			for(i=0; i<TMU_HAL_MAX_SUB_IFID_PER_PORT; i++) {
				(port_index + i)->port_sched_in_egress = sched_port; 
			}
		}
		else {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Port Scheduler for the port %d is %d\n",dp_subif->port_id, ingress_subif_index->port_sched_in_egress);
		}
		subif_index = ingress_subif_index;
		subif_index_port = port_index;
		if(dp_subif->subif != 0) {
			uint32_t port_prio_level;
			if(subif_index->queue_cnt == 0 && subif_index->base_sched_id_egress == 0xFF) {
				uint8_t leaf;
				uint32_t port_sched;
				tmu_hal_find_min_prio_level_of_port(subif_index_port, &port_sched, &port_prio_level);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Add First Queue of the subif %d to the Port Scheduler %d\n",dp_subif->subif,port_sched);
				subif_index->port_sched_in_user_lvl = port_prio_level;

				if (tmu_hal_scheduler_free_input_get(
							port_sched,
							&leaf) >= 0)
					port_prio_level = leaf;

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Adding the Sub interface to the leaf %d\n",port_prio_level);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"port_sched_in_egress= %d\n", subif_index->port_sched_in_egress );
				if(subif_index->port_sched_in_egress >> 3 == NULL_SCHEDULER_BLOCK_ID){
					tmu_hal_connect_subif_to_port(netdev, netdev->name, subif_index, subif_index_port, port_sched, port_prio_level, weight, flags);
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Base Scheduler is already there\n");	
				}
				base_sched = subif_index->base_sched_id_egress;

			} else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Minimum 1 Queue is already added for the subif %d to the Port Scheduler\n",
					dp_subif->subif);
				base_sched = subif_index->base_sched_id_egress;
			}
		} else {

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," For the main physical interface of the Port \n");
			base_sched = subif_index->base_sched_id_egress;
		}

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Base scheduler for the interface is %d \n",base_sched);

		q_new = tmu_hal_queue_add(netdev, netdev->name, g_MPE_PortId, base_sched, prio_type, prio_level, weight, subif_index, flags);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"User Queue/Prio Level/Weight --> %d/%d/%d is mapped to Queue index-->%d \n",
				subif_index->queue_cnt, prio_level, weight, q_new);

			/* Set the threshold values for the ingress queue, values are obtained from the queue configuration */
		memset(&q_param, 0, sizeof(struct tmu_hal_equeue_cfg));
		q_param.index = q_new;
		q_param.enable = 1;
		if(params->drop.mode == PPA_QOS_DROP_WRED) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify WRED params \n",__FUNCTION__);
			q_param.drop_threshold_green_min = params->drop.wred.min_th0;
			q_param.drop_threshold_green_max = params->drop.wred.max_th0;
			q_param.drop_threshold_yellow_min = params->drop.wred.min_th1;
			q_param.drop_threshold_yellow_max = params->drop.wred.max_th1;
			q_param.drop_probability_green = params->drop.wred.max_p0;
			q_param.drop_probability_yellow = params->drop.wred.max_p1;
			q_param.wred_enable = 1;
			q_param.avg_weight = params->drop.wred.weight;
			//q_param->reservation_threshold;
			//q_param->drop_threshold_unassigned;
			//q_param->drop_threshold_red;
		} else if (params->drop.mode == PPA_QOS_DROP_TAIL) {
			q_param.wred_enable = 0;
			q_param.drop_threshold_green_max = params->drop.wred.max_th0;
			q_param.drop_threshold_yellow_max = params->drop.wred.max_th1;
			q_param.drop_threshold_red = params->drop.wred.drop_th1;
		}

		tmu_hal_egress_queue_cfg_set(&q_param);

		qid_last_assigned = ++(subif_index->qid_last_assigned);
		tmu_hal_queue_track[q_new].user_q_idx = subif_index->queue_cnt;

		subif_index->user_queue[subif_index->queue_cnt].queue_index = q_new;
		subif_index->user_queue[subif_index->queue_cnt].queue_type = prio_type;
		subif_index->user_queue[subif_index->queue_cnt].prio_level = prio_level;
		subif_index->user_queue[subif_index->queue_cnt].weight = weight;
		subif_index->user_queue[subif_index->queue_cnt].sbin = tmu_hal_queue_track[q_new].sched_input;
		subif_index->user_queue[subif_index->queue_cnt].qid = qid_last_assigned;

		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_green_min = params->drop.wred.min_th0;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_green_max = params->drop.wred.max_th0;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_yellow_min = params->drop.wred.min_th1;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_yellow_max = params->drop.wred.max_th1;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_red = params->drop.wred.drop_th1;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_probability_green = params->drop.wred.max_p0;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_probability_yellow = params->drop.wred.max_p1;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.weight = params->drop.wred.weight;;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.enable = 1;
		if(params->drop.mode == PPA_QOS_DROP_WRED) {
			subif_index->user_queue[subif_index->queue_cnt].ecfg.wred_enable = 1;
		} else if (params->drop.mode == PPA_QOS_DROP_TAIL) {
			subif_index->user_queue[subif_index->queue_cnt].ecfg.wred_enable = 0;
		}

		subif_index->netdev = netdev ;
		
		subif_index->dp_subif.port_id = dp_subif->port_id;
		subif_index->dp_subif.subif = dp_subif->subif;
		if((flags & PPA_F_PPPOATM) == PPA_F_PPPOATM) {
			snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname),"%s", dev_name);
		}
		else
			snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname),"%s", netdev->name);
		if((flags & PPA_QOS_Q_F_DEFAULT) == PPA_QOS_Q_F_DEFAULT) {	
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Priority Level for Default Q : %d\n",prio_level);
			subif_index->default_prio = prio_level;
		}
		tmu_hal_set_mpe_ingress_grp_qmap(subif_index, prio_level, flags, q_new, tc, no_of_tc);
		subif_index->queue_cnt++ ;
		tmu_hal_dump_subif_queue_info(subif_index);

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New Q:[%d] TMU Port:[%d] has number of queues:[%d]\n", __FUNCTION__, q_new, tmu_hal_queue_track[q_new].epn, tmu_hal_port_track[tmu_hal_queue_track[q_new].epn].no_of_queues);

		/* Update SBIN weights of base scheduler for ingress */
		tmu_hal_update_ingress_sched_weight(cpu_tmu_res->tmu_sched);
		/*
		 * Increasing the threshold of TMU port for MPE for all colors to 0x240.
		 */
		if(tmu_hal_port_track[tmu_hal_queue_track[q_new].epn].no_of_queues > 1)	{
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Increasing Port Threshold of TMU Port %d\n", __FUNCTION__, tmu_hal_queue_track[q_new].epn);
			tmu_egress_port_tail_drop_thr_get(tmu_hal_queue_track[q_new].epn, &epth);
			epth.epth[0] = TMU_HAL_PORT_THRESHOLD_16TC;
			epth.epth[1] = TMU_HAL_PORT_THRESHOLD_16TC;
			epth.epth[2] = TMU_HAL_PORT_THRESHOLD_16TC;
			epth.epth[3] = TMU_HAL_PORT_THRESHOLD_16TC;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Threshold of TMU Port %d Increased to %x\n", __FUNCTION__, tmu_hal_queue_track[q_new].epn, TMU_HAL_PORT_THRESHOLD_16TC);
			tmu_egress_port_tail_drop_thr_set(tmu_hal_queue_track[q_new].epn, &epth);
		}
	}
CBM_RESOURCES_CLEANUP:
	kfree(cpu_tmu_res);
	kfree(dp_subif);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return qid_last_assigned ;

}

/** this function will check if any wfq schedular is attached to the
 ** input shced_id and return the wfq schedular id
 **/
uint32_t check_sched_input(uint32_t sched_id)
{
	struct tmu_sched_blk_in_link ilink = {0};
	struct tmu_sched_blk_in_link wfq_ilink = {0};

	tmu_sched_blk_in_link_get(sched_id << 3, &ilink);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enter %d\n" ,__FUNCTION__,sched_id);
	if (ilink.sie != 0) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> sie is not zero qsid %d sit %d\n",
				__FUNCTION__,ilink.qsid, ilink.sit);
		if (ilink.sit == 1) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Already Scheduler %d is added for the port schedular %d\n",
					__FUNCTION__, ilink.qsid, sched_id);
			if (tmu_hal_sched_track[ilink.qsid].policy == TMU_HAL_POLICY_WFQ) {
				tmu_sched_blk_in_link_get(ilink.qsid << 3, &wfq_ilink);
				if (wfq_ilink.sie != 0) {
					if (wfq_ilink.sit == 1) {
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> Scheduler %d is connected to  %d\n",
								__FUNCTION__, wfq_ilink.qsid, ilink.qsid);
						return ilink.qsid;
					} else {
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> Queus are connected to sched %d\n",
								__FUNCTION__, wfq_ilink.qsid);
						return TMU_HAL_STATUS_OK;
					}
				}
			} else
				return TMU_HAL_STATUS_OK;
		}
	}
	return TMU_HAL_STATUS_OK;
}

int tmu_hal_add_egress_queue(
		struct net_device *netdev, 
		char *dev_name, 
		char *tc , /*tc[MAX_TC_NUM],*/
		uint8_t no_of_tc,
		uint32_t schedid, 
		uint32_t prio_type, 
		uint32_t prio_level, 
		uint32_t weight, 
		uint32_t flowId, 
		QOS_Q_ADD_CFG *param,
		uint32_t flags)
{
	uint32_t i=0, base_sched, q_new, qid_last_assigned = 0;
	uint32_t nof_of_tmu_ports;
	uint32_t sbin_sched = 0;
	cbm_tmu_res_t *tmu_res=NULL;
	dp_subif_t *dp_subif = NULL;
	uint32_t port;
	struct tmu_hal_user_subif_abstract *subif_index;
	struct tmu_hal_user_subif_abstract *subif_index_port;
	struct tmu_hal_equeue_cfg q_param;
	int32_t tmuport;
	struct tmu_egress_port_thr epth;
	int add_checksum = 0;
	cbm_tmu_res_t *tmu_res_checksum = NULL;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);	

	TMU_ALLOC(dp_subif_t, dp_subif);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add Egress Queue for netdevice %s\n",__FUNCTION__,netdev->name);
	if(tmu_hal_get_tmu_res_from_netdevice(netdev, dev_name, schedid, dp_subif, &nof_of_tmu_ports, &tmu_res, flags) != TMU_HAL_STATUS_OK){
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"%s:%d Error: Failed to get resources from Netdevice\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}

	port = dp_subif->port_id;
	tmuport = tmu_res->tmu_port;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add Egress Queue for Datapath Port %d\n",__FUNCTION__,dp_subif->port_id);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of queues for TMU Port %d is %d\n",
			__FUNCTION__,tmu_res->tmu_port, tmu_hal_port_track[tmu_res->tmu_port].no_of_queues);

	if(tmu_hal_port_track[tmu_res->tmu_port].no_of_queues == TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Max number of queues reached for Datapath Port %d\n",__FUNCTION__,dp_subif->port_id);
		kfree(tmu_res);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "flags=%d\n", flags);
	if((dp_subif->port_id !=0) && (tmu_res->tmu_port != g_CPU_PortId) ) { /* Egress handling except CPU port */

		subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		subif_index_port = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of Queus for sub interface is %d\n",__FUNCTION__,subif_index->queue_cnt);

		/** Selection of Scheduler is only applicable for the physical 
			interface and for the LAN port only.
			CBM resources for LAN ports always returns 2 TMU ports. So
			needs to decide the tmu port and scheduler based on that.
		 */	
		if(dp_subif->subif == 0) /* for the physical interface*/ {
			if(prio_level <= high_prio_q_limit) {
				if(nof_of_tmu_ports > 1) {
					base_sched = (tmu_res)->tmu_sched;
					tmuport = (tmu_res)->tmu_port;
					subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Low Priority Port:%d SB: %d \n",__FUNCTION__,tmuport,base_sched);
				}
			} else {
				if(nof_of_tmu_ports > 1) {
					base_sched = (tmu_res+1)->tmu_sched;
					tmuport = (tmu_res+1)->tmu_port;
					subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> High	Priority Port:%d SB: %d \n",__FUNCTION__,tmuport,base_sched);
				}
			} 

		}

		if((dp_subif->subif != 0 )) { /* for the logical interface */
			uint32_t port_prio_level;
			uint32_t user_prio;
			uint32_t port_sched;

			/** By default the logical interface will be connected to the port priority
				level of (TMU_HAL_MAX_PRIORITY_LEVEL - 1).	*/
			tmu_hal_find_min_prio_level_of_port(subif_index_port, &port_sched, &port_prio_level);
			port_prio_level = user_prio = (TMU_HAL_MAX_PRIORITY_LEVEL - 1);
			sbin_sched = port_sched;
			/** Depends on the value of high priority queue limit, the selection of port
				schedler and tmu port is required. 
				This selection is required for LAN port. 
			 */
			if(port_prio_level <= high_prio_q_limit) {
				if(nof_of_tmu_ports > 1) {
					port_sched = base_sched = (tmu_res)->tmu_sched;
					tmuport = (tmu_res)->tmu_port;
					subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Connect logical interface to Low Priority Port:%d SB: %d \n",
							__FUNCTION__,tmuport,base_sched);
				}
			} else {
				if(nof_of_tmu_ports > 1) {
					port_sched = base_sched = (tmu_res+1)->tmu_sched;
					tmuport = (tmu_res+1)->tmu_port;
					subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Connect logical interface to High	Priority Port:%d SB: %d \n",
							__FUNCTION__,tmuport,base_sched);
				}
			}

#ifdef OPTION_1
			if(tmu_hal_is_priority_scheduler_for_port(tmuport) == TMU_HAL_STATUS_OK)
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Scheduler for the port %d is %d\n",__FUNCTION__,port,tmu_hal_port_track[port].port_sched_id);
			else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add Port Scheduler for the port %d\n",__FUNCTION__,tmuport);
				tmu_hal_create_port_scheduler(tmuport);
			}
#else
			if(tmu_hal_port_track[port].shaper_sched_id == 0xFF) {
				tmu_hal_port_track[tmuport].port_sched_id	= tmu_hal_port_track[tmuport].input_sched_id;
			} else {/*Port Rate shaper is already there*/
				tmu_hal_port_track[tmuport].port_sched_id	= tmu_hal_sched_track[tmu_hal_port_track[tmuport].shaper_sched_id].next_sched_id ;
			}

#endif
			if(subif_index->queue_cnt == 0 && subif_index->base_sched_id_egress == 0xFF)
			{
				uint8_t leaf, uIndex;
				uint32_t sched_id;
				uint32_t q_index =0, shift_idx;
				uint32_t vaps = 0,ret;
				/* check if we already have a wfq schedular for vap qos in case of
				   base vap creation */
				if (flags & PPA_QOS_Q_F_VAP_QOS) {
					vaps = tmu_hal_sched_track[port_sched].next_sched_id;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> vap sched is %d \n",__FUNCTION__, vaps);
					if (vaps != 0 && vaps < 255) {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> vap sched %d and omid is %d\n",
							__FUNCTION__, vaps,tmu_hal_sched_track[vaps].omid);
					if (tmu_hal_sched_track[vaps].policy == TMU_HAL_POLICY_WFQ)
						port_sched = tmu_hal_sched_track[vaps].omid >> 3;
					}
					sbin_sched = port_sched;
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add First Queue of the subif %d to the Port Scheduler %d\n",
						__FUNCTION__,dp_subif->subif, port_sched);

				/** In case already 8 queues are already created for the base interface
					then, there is no free leaf. We need to create the scheduler level. 
				 */
				if (tmu_hal_scheduler_free_input_get(
							port_sched,
							&leaf) < 0) {
					uint32_t omid, parent;
					struct tmu_hal_sched_track_info Snext;
					struct tmu_hal_sched_track_info Stemp;

					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port:%d has leaf mask:%d .. No Free leaf\n",
							__FUNCTION__,port_sched,tmu_hal_sched_track[port_sched].leaf_mask);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Creating a new level\n",__FUNCTION__);
					Snext = tmu_hal_sched_track[port_sched];
					parent = Snext.id;
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"parent %d next_sched_id %d\n",parent, Snext.next_sched_id);
					if(Snext.leaf_mask == 0xFF && tmu_hal_sched_track[Snext.next_sched_id].priority_selector != 2) {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Adding a new level for SB:%d\n",__FUNCTION__, port_sched);
						tmu_hal_add_scheduler_level(tmuport, port_sched, 0, &parent, TMU_HAL_POLICY_STRICT, &omid);
						tmu_hal_sched_track[parent].priority_selector = 1;
						tmu_hal_sched_track[Snext.id].priority_selector = 2;
						tmu_hal_sched_track[omid].priority_selector = 3;
						subif_index_port->base_sched_id_egress = parent;
						Stemp = tmu_hal_sched_track[parent];
						tmu_hal_q_align_for_priority_selector(netdev, dev_name, tmuport, subif_index_port, 
								tmu_hal_sched_track[Stemp.next_sched_id].id, 
								tmu_hal_sched_track[Stemp.next_sched_id].peer_sched_id, TMU_HAL_Q_PRIO_LEVEL, flags);
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New Sched: %d\n",__FUNCTION__, omid);
						port_sched = omid;
					}
				}
				ret = tmu_hal_get_q_idx_of_same_prio(subif_index_port, port_sched,
							port_prio_level, &q_index, &uIndex, &sched_id, flags);
				/* if we have vap level qos for both vap and base vap, no need to create common
				   wfq schedular as it is created during base vap creation */
				if ((vaps == 0 || vaps == 0xFF) && (ret != TMU_HAL_STATUS_OK)) {
					uint32_t new_sched, level;

					tmu_hal_find_sched_in_for_q_add(subif_index_port, port_sched, port_prio_level, &shift_idx );
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>shift_idx:%d\n",__FUNCTION__,shift_idx);
					tmu_hal_shift_down_sbin_conn(netdev, dev_name, tmuport, subif_index_port, port_sched, shift_idx, flags);
					port_prio_level = shift_idx;
					if (flags & PPA_QOS_Q_F_VAP_QOS) {
						level = tmu_hal_sched_track[port_sched].level;
						new_sched = create_new_scheduler(port_sched, 0, TMU_HAL_POLICY_WFQ, 0, 0);
						tmu_hal_sched_track[new_sched].next_sched_id = port_sched;
						tmu_hal_sched_track[new_sched].policy = TMU_HAL_POLICY_WFQ;
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s>New Common WFQ Schedular for VAP - %d level - %d \n",
							__FUNCTION__,new_sched, level+1);
						tmu_hal_sched_out_remap(new_sched, level+1, port_sched << 3, 1, 0);
						sched_id = new_sched;
						port_sched = new_sched;
					}
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Connect the logical interface to User Index:%d Scheduler Block: %d\n",
							__FUNCTION__, uIndex, sched_id);
					if (flags & PPA_QOS_Q_F_VAP_QOS) {
						if (vaps)
							port_prio_level = 0;
						else
							port_prio_level = sched_id << 3;
					} else
						port_prio_level = uIndex;
				}

				subif_index->port_sched_in_user_lvl = user_prio;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Adding the Sub interface to the leaf %d\n",__FUNCTION__,port_prio_level);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> port_sched_in_egress= %d\n",__FUNCTION__, subif_index->port_sched_in_egress );
				if(subif_index->port_sched_in_egress >> 3 == NULL_SCHEDULER_BLOCK_ID) {
					tmu_hal_connect_subif_to_port(netdev, dev_name, subif_index, subif_index_port, port_sched, port_prio_level, weight, flags);
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base Scheduler is already there\n",__FUNCTION__);	
				}
				base_sched = subif_index->base_sched_id_egress;

			} else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Minimum 1 Queue is already added for the subif %d to the Port Scheduler \n",
						__FUNCTION__, dp_subif->subif);
				base_sched = subif_index->base_sched_id_egress;
			}
		} else {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> For the main physical interface of the Port \n",__FUNCTION__);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Prio level %d High Prio Limit %d \n",__FUNCTION__,prio_level, high_prio_q_limit);
			base_sched = subif_index->base_sched_id_egress;
			/* For Base vap default queue addition, check if vap wfq shcedular is present or not
			   based on that either create base wfq schedular or only vap schedualr */
			if ((flags & PPA_QOS_Q_F_VAP_QOS) && ((flags & PPA_QOS_Q_F_DEFAULT) == PPA_QOS_Q_F_DEFAULT)) {
				uint32_t new_sched, level = 0, vap_sched;
				uint32_t shift_idx = 0;
				uint8_t leaf;
				sbin_sched = base_sched;
				subif_index_port->base_sched_id_egress = base_sched;

				vap_sched = check_sched_input(base_sched);
				if (vap_sched == 0) {
					tmu_hal_find_sched_in_for_q_add(subif_index_port, base_sched, prio_level, &shift_idx );
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>shift_idx:%d\n",__FUNCTION__,shift_idx);
					tmu_hal_shift_down_sbin_conn(netdev, dev_name, tmuport, subif_index_port, base_sched, shift_idx, flags);
					level = tmu_hal_sched_track[base_sched].level;
					new_sched = create_new_scheduler(base_sched, 0, TMU_HAL_POLICY_WFQ, 0, 0);

					tmu_hal_sched_track[new_sched].next_sched_id = base_sched;
					tmu_hal_sched_track[new_sched].policy = TMU_HAL_POLICY_WFQ;
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s>New Common WFQ Schedualr for VAP - %d level - %d\n",
							__FUNCTION__,new_sched, level+1);
					tmu_hal_sched_out_remap(new_sched, level+1, base_sched << 3, 1, 0);
					base_sched = new_sched;

					new_sched = create_new_scheduler(base_sched, 0, TMU_HAL_POLICY_STRICT, level+2, 0);
					tmu_hal_sched_track[new_sched].next_sched_id = base_sched;
					tmu_hal_sched_track[new_sched].policy = TMU_HAL_POLICY_STRICT;
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s>New VAP Sched for base VAP - %d level - %d \n",
							__FUNCTION__,new_sched, level+2);
					tmu_hal_sched_out_remap(new_sched, level+2, base_sched << 3, 1, 0);
					base_sched = new_sched;
				} else {
					base_sched = vap_sched;
					tmu_hal_shift_down_sbin_conn(netdev, dev_name, tmuport,
								subif_index_port, base_sched, shift_idx, flags);
					new_sched = create_new_scheduler(base_sched, 0, TMU_HAL_POLICY_STRICT, level+1, 0);
					tmu_hal_sched_track[new_sched].next_sched_id = base_sched;
					tmu_hal_sched_track[new_sched].policy = TMU_HAL_POLICY_STRICT;
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s>New VAP Sched for base vap - %d level - %d \n",
							__FUNCTION__,new_sched, level+1);
					tmu_hal_scheduler_free_input_get(base_sched, &leaf);
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s>leaf %d\n", __FUNCTION__, leaf);
					tmu_hal_shift_down_sbin_conn(netdev, dev_name, tmuport,
								subif_index_port, base_sched, leaf, flags);
					tmu_hal_sched_out_remap(new_sched, level+2, base_sched << 3, 1, 0);
					base_sched = new_sched;
				}
				subif_index->base_sched_id_egress = base_sched;
			}
		}

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base scheduler for the interface is %d \n",__FUNCTION__,base_sched);

		q_new = tmu_hal_queue_add(netdev, dev_name, tmuport, base_sched, prio_type, prio_level, weight, subif_index, flags);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"User Queue/Prio Level/Weight --> %d/%d/%d is mapped to Queue index-->%d \n",
				subif_index->queue_cnt, prio_level, weight, q_new);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_HIGH, "flags=%d\n", flags);

		memset(&q_param, 0, sizeof(struct tmu_hal_equeue_cfg));
		q_param.index = q_new;
		q_param.enable = 1;
		if(param->drop.mode == PPA_QOS_DROP_WRED) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify WRED params \n",__FUNCTION__);
			q_param.drop_threshold_green_min = param->drop.wred.min_th0;
			q_param.drop_threshold_green_max = param->drop.wred.max_th0;
			q_param.drop_threshold_yellow_min = param->drop.wred.min_th1;
			q_param.drop_threshold_yellow_max = param->drop.wred.max_th1;
			q_param.drop_probability_green = param->drop.wred.max_p0;
			q_param.drop_probability_yellow = param->drop.wred.max_p1;
			q_param.wred_enable = 1;
			q_param.avg_weight = param->drop.wred.weight;
		} else if (param->drop.mode == PPA_QOS_DROP_TAIL) {
			q_param.wred_enable = 0;
			q_param.drop_threshold_green_max = param->drop.wred.max_th0;
			q_param.drop_threshold_yellow_max = param->drop.wred.max_th1;
			q_param.drop_threshold_red = param->drop.wred.drop_th1;
		}

		tmu_hal_egress_queue_cfg_set(&q_param);

		if((flags & PPA_QOS_Q_F_DEFAULT) == PPA_QOS_Q_F_DEFAULT) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Priority Level for Default Q : %d\n",prio_level);
			subif_index->default_prio = prio_level;
		}

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Default queue prio=%d prio_level=%d \n",__FUNCTION__,subif_index->default_prio, prio_level);
		if((prio_level == subif_index->default_prio ) && ((flags & PPA_QOS_Q_F_DEFAULT) == PPA_QOS_Q_F_DEFAULT)) {
			cbm_queue_map_entry_t q_map;
			int32_t q_map_mask = 0, x=0;
			struct tmu_sched_blk_in_link ilink;
			
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Prio Level/No Of TMU ports --> %d/%d/ \n",__FUNCTION__,prio_level, nof_of_tmu_ports);
			/** Configure the QMAP table to connect to the Egress queue*/
			memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));

			q_map.ep = dp_subif->port_id;

			if(dp_subif->subif != 0) /* for the logical interface */ {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Deafult Q --> Flow Id %d for VLAN interface subif: %d\n",
						__FUNCTION__, flowId, dp_subif->subif);
				/** This is a hack right now. I am passing the cfg->intfId_en. This is required 
					for ATM as the interface with subif>0 could be the default */
				if(schedid == 0) {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Hack for VLAN with cfg->intfId_en: %d\n",__FUNCTION__, schedid);
					q_map_mask  |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
						CBM_QUEUE_MAP_F_DE_DONTCARE |
						CBM_QUEUE_MAP_F_EN_DONTCARE |
						CBM_QUEUE_MAP_F_TC_DONTCARE |
						CBM_QUEUE_MAP_F_MPE1_DONTCARE |
						CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
				} else {
					q_map.flowid = flowId;
					q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
						CBM_QUEUE_MAP_F_DE_DONTCARE |
						CBM_QUEUE_MAP_F_MPE1_DONTCARE |
						CBM_QUEUE_MAP_F_MPE2_DONTCARE ;

					if(flowId > 3) {
						q_map.tc |= 8;
						q_map.flowid = (flowId & 0x3);
					} else
						q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE;
				}
				if (flags & PPA_QOS_Q_F_VAP_QOS) {
					tmu_hal_vap_qmap_update(&q_map, &q_map_mask, dp_subif);
					q_map_mask  |= CBM_QUEUE_MAP_F_TC_DONTCARE;
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Map Default Q for logical interface: %d\n",__FUNCTION__,q_new);
				tmu_hal_add_q_map(q_new, CPU_PORT_ID, &q_map, q_map_mask);
				subif_index->default_q = q_new;
				for(x=1; x<subif_index->queue_cnt; x++) {
					q_map_mask = 0;
					/** Configure the QMAP table to connect to the Egress queue*/
					memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));

					q_map.ep = dp_subif->port_id;
					q_map.flowid = flowId;
					q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
						CBM_QUEUE_MAP_F_DE_DONTCARE |
						CBM_QUEUE_MAP_F_MPE1_DONTCARE |
						CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					for(i=0; i< subif_index->user_queue[x].no_of_tc; i++)
					{
						q_map.tc = subif_index->user_queue[x].qmap[i];
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Default Q --> Add QMAP for Port %d and TC: %d\n",
								__FUNCTION__,q_map.ep,q_map.tc);
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Default Q --> User Index %d Q Index: %d\n",
								__FUNCTION__, x, subif_index->user_queue[x].queue_index);
						tmu_hal_add_q_map(subif_index->user_queue[x].queue_index, CPU_PORT_ID, &q_map, q_map_mask);
					}
				}
				if (flags & PPA_QOS_Q_F_VAP_QOS) {
					tmu_sched_blk_in_link_get(sbin_sched << 3, &ilink);
					tmu_hal_update_ingress_sched_weight(ilink.qsid);
				}
			} else { /*default Q of Physical interface*/
				/* Check and configure the QMAP table to connect to the checksum queue
				 * FLOW ID: 00 DEC:1 ENC:1 MPE1:0 MPE2:1 EP: 7-14 CLASS: XX
				 */
				struct tmu_sched_blk_in_link ilink;

				add_checksum = 0;
				nof_of_tmu_ports = 0;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> vvvv physical interface\n",
						__FUNCTION__);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Checking for checksum queue on port:[%d]\n", 
						__FUNCTION__, dp_subif->port_id);

				if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res_checksum, DP_F_CHECKSUM) == TMU_HAL_STATUS_OK) {
					add_checksum = 1;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Checksum Queue present! Qid:[%d]\n",
							__FUNCTION__, tmu_res_checksum->tmu_q);
				} else {
					kfree(tmu_res_checksum);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Checksum Queue NOT present!\n",
							__FUNCTION__);
				}
				q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
					CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
					CBM_QUEUE_MAP_F_EN_DONTCARE |
					CBM_QUEUE_MAP_F_DE_DONTCARE |
					CBM_QUEUE_MAP_F_TC_DONTCARE |
					CBM_QUEUE_MAP_F_MPE1_DONTCARE |
					CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Map Default Q: %d\n",__FUNCTION__,q_new);
				tmu_hal_add_q_map(q_new, CPU_PORT_ID, &q_map, q_map_mask);
				subif_index->default_q = q_new;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Queue count: %d\n",__FUNCTION__, subif_index->queue_cnt);
				for(x=1; x<subif_index->queue_cnt; x++) {
					q_map_mask = 0;
					/** Configure the QMAP table to connect to the Egress queue*/
					memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));

					q_map.ep = dp_subif->port_id;
					q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
						CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
						CBM_QUEUE_MAP_F_EN_DONTCARE |
						CBM_QUEUE_MAP_F_DE_DONTCARE |
						CBM_QUEUE_MAP_F_MPE1_DONTCARE |
						CBM_QUEUE_MAP_F_MPE2_DONTCARE ;

					for(i=0; i< subif_index->user_queue[x].no_of_tc; i++) {
						q_map.tc = subif_index->user_queue[x].qmap[i];
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Deafult Q --> Add QMAP for Port %d and TC: %d\n",
								__FUNCTION__,q_map.ep,q_map.tc);
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Default Q --> User Index %d Q Index: %d\n",
								__FUNCTION__, x, subif_index->user_queue[x].queue_index);
						tmu_hal_add_q_map(subif_index->user_queue[x].queue_index, CPU_PORT_ID, &q_map, q_map_mask);
					}

				}
				if (flags & PPA_QOS_Q_F_VAP_QOS) {
					tmu_sched_blk_in_link_get(sbin_sched << 3, &ilink);
					tmu_hal_update_ingress_sched_weight(ilink.qsid);
				}
			}
		} else { /* not for default q */
			cbm_queue_map_entry_t q_map;
			uint32_t q_map_mask = 0;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Not for default Q\n", __FUNCTION__);
			/** Configure the QMAP table to connect to the Egress queue*/
			memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
			q_map.ep = dp_subif->port_id;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> No Of TC: %d\n",__FUNCTION__, no_of_tc);
			for(i=0; i< no_of_tc; i++) {
				q_map.tc = *(tc+i);
				if(dp_subif->subif != 0) /* for the logical interface */{
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Flow Id %d for VLAN interface subif: %d\n",__FUNCTION__, flowId, dp_subif->subif);
					/** This is a hack right now. I am passing the cfg->intfId_en. This is required 
						for ATM as the interface with subif>0 could be the default */
					if(schedid == 0) {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Hack for VLAN with cfg->intfId_en: %d\n",__FUNCTION__, schedid);
						q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
							CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
							CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					} else {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Sched id is not zero with flowid:[%d]\n", __FUNCTION__, flowId);
						q_map.flowid = flowId;
						if(flowId > 3) {
							q_map.tc |= 8;
							q_map.flowid = (flowId & 0x3);
						}
						q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					}
					if (flags & PPA_QOS_Q_F_VAP_QOS) {
						tmu_hal_vap_qmap_update(&q_map, &q_map_mask, dp_subif);
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> Qmap: flowid:[0x%x] dec:[0x%x] enc:[0x%x]\n", __FUNCTION__, q_map.flowid, q_map.dec, q_map.enc);
					}
				} else {
					/* Check and configure the QMAP table to connect to the checksum queue
					 * FLOW ID: 00 DEC:1 ENC:1 MPE1:0 MPE2:1 EP: 7-14 CLASS: XX
					 */
					add_checksum = 0;
					nof_of_tmu_ports = 0;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Checking for checksum queue on port:[%d]\n",
							__FUNCTION__, dp_subif->port_id);

					if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res_checksum, DP_F_CHECKSUM) == TMU_HAL_STATUS_OK) {
						add_checksum = 1;
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Checksum Queue present! Qid:[%d]\n",
								__FUNCTION__, tmu_res_checksum->tmu_q);
					} else {
						kfree(tmu_res_checksum);
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Checksum Queue NOT present!\n",
								__FUNCTION__);
					}

					q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
						CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
						CBM_QUEUE_MAP_F_EN_DONTCARE |
						CBM_QUEUE_MAP_F_DE_DONTCARE |
						CBM_QUEUE_MAP_F_MPE1_DONTCARE |
						CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add QMAP for Port %d and TC: %d\n",__FUNCTION__,q_map.ep,q_map.tc);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Index of New Q: %d\n",__FUNCTION__,q_new);
				cbm_queue_map_set(g_Inst, q_new, &q_map, q_map_mask);
				subif_index->user_queue[subif_index->queue_cnt].qmap[i] =*(tc+i);
			}	
			subif_index->user_queue[subif_index->queue_cnt].no_of_tc = no_of_tc;
		}
		/*Special handling for DL port */
		if((flags & PPA_QOS_Q_F_WLANDP) == PPA_QOS_Q_F_WLANDP) {
			struct tmu_hal_dp_res_info res = {0};
			cbm_queue_map_entry_t q_map;
			uint32_t q_map_mask = 0;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> DL Interface ... Add Ingress QMAP for Port %d and TC: %d\n",__FUNCTION__,q_map.ep,q_map.tc);
			tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
			memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
			q_map_mask =0;

			q_map.ep = dp_subif->port_id;
			q_map.flowid = 0;
			q_map.enc = 1;
			q_map.dec =1;
			q_map.mpe1 =0;
			q_map.mpe2 =0;
			q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE; 
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Set QMAP for the Ingress Q: %d\n",__FUNCTION__,res.q_ingress);
			tmu_hal_add_q_map(res.q_ingress, CPU_PORT_ID, &q_map, q_map_mask);

		}

		if (add_checksum) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Adding q_map for checksum queue:[%d]\n",
					__FUNCTION__, tmu_res_checksum->tmu_q);
			if (tmu_hal_set_checksum_queue_map(dp_subif->port_id) != TMU_HAL_STATUS_OK) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR, "<%s> Adding q_map for checksum queue:[%d] failed!\n",
						__FUNCTION__, tmu_res_checksum->tmu_q);
			}
			kfree(tmu_res_checksum);
		}

		qid_last_assigned = ++(subif_index->qid_last_assigned);
		tmu_hal_queue_track[q_new].user_q_idx = subif_index->queue_cnt;

		subif_index->user_queue[subif_index->queue_cnt].queue_index = q_new;
		subif_index->user_queue[subif_index->queue_cnt].queue_type = prio_type;
		subif_index->user_queue[subif_index->queue_cnt].prio_level = prio_level;
		subif_index->user_queue[subif_index->queue_cnt].weight = weight;
		subif_index->user_queue[subif_index->queue_cnt].sbin = tmu_hal_queue_track[q_new].sched_input;
		subif_index->user_queue[subif_index->queue_cnt].qid = qid_last_assigned;

		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_green_min = param->drop.wred.min_th0;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_green_max = param->drop.wred.max_th0;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_yellow_min = param->drop.wred.min_th1;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_yellow_max = param->drop.wred.max_th1;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_red = param->drop.wred.drop_th1;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_probability_green = param->drop.wred.max_p0;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_probability_yellow = param->drop.wred.max_p1;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.weight = param->drop.wred.weight;;
		subif_index->user_queue[subif_index->queue_cnt].ecfg.enable = 1;	
		if(param->drop.mode == PPA_QOS_DROP_WRED) {
			subif_index->user_queue[subif_index->queue_cnt].ecfg.wred_enable = 1;
		} else if (param->drop.mode == PPA_QOS_DROP_TAIL) {
			subif_index->user_queue[subif_index->queue_cnt].ecfg.wred_enable = 0;
		}
		subif_index->netdev = netdev ;
		subif_index->queue_cnt++ ;
		subif_index->dp_subif.port_id = dp_subif->port_id;
		subif_index->dp_subif.subif = dp_subif->subif;
		if((flags & PPA_F_PPPOATM) == PPA_F_PPPOATM) {
			snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname),"%s", dev_name);
		}
		else
			snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname),"%s", netdev->name);

		tmu_hal_dump_subif_queue_info(subif_index);

	} else { /*Egress handling for CPU Port = DP Egress */
		/*Directpath Egress*/
		if(!g_DpEgressQueueScheduler) {
			/*Create the WFQ Scheduler*/
			g_DpEgressQueueScheduler++;
		}
		else {
			/*Root Scheduler is already connected*/
			uint32_t sched_port;
			struct tmu_hal_dp_res_info res = {0};
			struct tmu_hal_user_subif_abstract *ingress_subif_index = NULL;
			struct tmu_hal_user_subif_abstract *port_index = NULL;
			cbm_queue_map_entry_t q_map;
			int32_t q_map_mask = 0;

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Root Scheduler for directpath %d is already created\n",__FUNCTION__,g_Root_sched_id_DpEgress);
			if(dp_get_netif_subifid(netdev, NULL, NULL, 0, dp_subif, flags) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
			if(dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,
						"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
					if(tmu_hal_get_subitf_via_ifname(dev_name,dp_subif) != PPA_SUCCESS) {
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,
							"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
						kfree(dp_subif);
						return TMU_HAL_STATUS_ERR;
					}
				}
			}

			tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> TMU Port is %d \n",__FUNCTION__,res.dp_egress_res.dp_port);
			port_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT); 
			ingress_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) +
									(dp_subif->subif >> 8);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"subif index is %p\n",ingress_subif_index);

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Base scheduler Id %d\n",port_index->base_sched_id_egress);
			if(port_index->base_sched_id_egress == 0xFF) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Create Port Scheduler for the port \n");
				sched_port = create_new_scheduler(g_Root_sched_id_DpEgress, 0, TMU_HAL_POLICY_WFQ, tmu_hal_sched_track[g_Root_sched_id_DpEgress].level + 1, 1);

				port_index->base_sched_id_egress = sched_port;
				for(i=0; i<TMU_HAL_MAX_SUB_IFID_PER_PORT; i++) {
					(port_index + i)->port_sched_in_egress = sched_port; 
				}
			} else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Port Scheduler for the port %d is %d\n",
						__FUNCTION__,dp_subif->port_id, ingress_subif_index->port_sched_in_egress);
			}
			subif_index = ingress_subif_index;
			subif_index_port = port_index;
			if(dp_subif->subif != 0) {
				uint32_t port_prio_level;
				if(subif_index->queue_cnt == 0 && subif_index->base_sched_id_egress == 0xFF) {

					uint8_t leaf, uIndex;
					uint32_t port_sched, sched_id;
					uint32_t q_index =0, shift_idx;
					uint32_t user_prio;
					tmu_hal_find_min_prio_level_of_port(subif_index_port, &port_sched, &port_prio_level);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Add First Queue of the subif %d to the Port Scheduler %d\n",
							__FUNCTION__,(dp_subif->subif >> 8),port_sched);
					subif_index->port_sched_in_user_lvl = port_prio_level;
					port_prio_level = user_prio = TMU_HAL_MAX_PRIORITY_LEVEL - 1;

					/** In case already 8 queues are already created for the base interface
						then, there is no free leaf. We need to create the scheduler level. 
					 */
					if (tmu_hal_scheduler_free_input_get(
								port_sched,
								&leaf) < 0) {
						uint32_t omid, parent;
						struct tmu_hal_sched_track_info Snext;
						struct tmu_hal_sched_track_info Stemp;

						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port:%d has leaf mask:%d .. No Free leaf\n",
								__FUNCTION__,port_sched,tmu_hal_sched_track[port_sched].leaf_mask);
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Creating a new level\n",__FUNCTION__);
						Snext = tmu_hal_sched_track[port_sched];
						parent = Snext.id;
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"parent %d next_sched_id %d\n",parent, Snext.next_sched_id);
						if(Snext.leaf_mask == 0xFF && tmu_hal_sched_track[Snext.next_sched_id].priority_selector != 2)  {
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Adding a new level for SB:%d\n",__FUNCTION__, port_sched);
							tmu_hal_add_scheduler_level(port, port_sched, 0, &parent, TMU_HAL_POLICY_STRICT, &omid);
							tmu_hal_sched_track[parent].priority_selector = 1;
							tmu_hal_sched_track[Snext.id].priority_selector = 2;
							tmu_hal_sched_track[omid].priority_selector = 3;
							subif_index_port->base_sched_id_egress = parent;
							Stemp = tmu_hal_sched_track[parent];
							tmu_hal_q_align_for_priority_selector(netdev, dev_name, port, subif_index_port, 
									tmu_hal_sched_track[Stemp.next_sched_id].id, 
									tmu_hal_sched_track[Stemp.next_sched_id].peer_sched_id, TMU_HAL_Q_PRIO_LEVEL, flags);
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New Sched: %d\n",__FUNCTION__, omid);
							port_sched = omid;

						}
					} 					

					if(tmu_hal_get_q_idx_of_same_prio(subif_index_port, port_sched, port_prio_level, &q_index, &uIndex, &sched_id, flags) != TMU_HAL_STATUS_OK) {
						tmu_hal_find_sched_in_for_q_add(subif_index_port, port_sched, port_prio_level, &shift_idx );
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>shift_idx:%d\n",__FUNCTION__,shift_idx);
						tmu_hal_shift_down_sbin_conn(netdev, dev_name, tmuport, subif_index_port, port_sched, shift_idx, flags);
						port_prio_level = shift_idx;
					} else {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"User Index:%d Scheduler Block: %d\n",uIndex, sched_id);
						port_prio_level = uIndex;
					}
					subif_index->port_sched_in_user_lvl = user_prio;

					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Adding the Sub interface to the leaf %d\n",__FUNCTION__,port_prio_level);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> port_sched_in_egress= %d\n",__FUNCTION__, subif_index->port_sched_in_egress );
					if(subif_index->port_sched_in_egress >> 3 == NULL_SCHEDULER_BLOCK_ID){
						tmu_hal_connect_subif_to_port(netdev, dev_name, subif_index, subif_index_port, port_sched, port_prio_level, weight, flags);
					} else {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base Scheduler is already there\n",__FUNCTION__);	
					}

					base_sched = subif_index->base_sched_id_egress;

				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Minimum 1 Queue is already added for the subif %d to the Port Scheduler\n",
						dp_subif->subif);
					base_sched = subif_index->base_sched_id_egress;
				}
			} else {

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> For the Physical interface of the Port \n",__FUNCTION__);
				base_sched = subif_index->base_sched_id_egress;
			}

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Base scheduler for the interface is %d \n",__FUNCTION__,base_sched);

			q_new = tmu_hal_queue_add(netdev, dev_name, g_CPU_PortId, base_sched, prio_type, prio_level, weight, subif_index, flags);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>User Queue/Prio Level/Weight --> %d/%d/%d is mapped to Queue index-->%d \n",
					__FUNCTION__, subif_index->queue_cnt, prio_level, weight, q_new);

			memset(&q_param, 0, sizeof(struct tmu_hal_equeue_cfg));
			q_param.index = q_new;
			q_param.enable = 1;
			if(param->drop.mode == PPA_QOS_DROP_WRED) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify WRED params \n",__FUNCTION__);
				q_param.drop_threshold_green_min = param->drop.wred.min_th0;
				q_param.drop_threshold_green_max = param->drop.wred.max_th0;
				q_param.drop_threshold_yellow_min = param->drop.wred.min_th1;
				q_param.drop_threshold_yellow_max = param->drop.wred.max_th1;
				q_param.drop_probability_green = param->drop.wred.max_p0;
				q_param.drop_probability_yellow = param->drop.wred.max_p1;
				q_param.wred_enable = 1;
				q_param.avg_weight = param->drop.wred.weight;
			} else if (param->drop.mode == PPA_QOS_DROP_TAIL) {
				q_param.wred_enable = 0;
				q_param.drop_threshold_green_max = param->drop.wred.max_th0;
				q_param.drop_threshold_yellow_max = param->drop.wred.max_th1;
				q_param.drop_threshold_red = param->drop.wred.drop_th1;
			}

			tmu_hal_egress_queue_cfg_set(&q_param);

			qid_last_assigned = ++(subif_index->qid_last_assigned);
			tmu_hal_queue_track[q_new].user_q_idx = subif_index->queue_cnt;

			subif_index->user_queue[subif_index->queue_cnt].queue_index = q_new;
			subif_index->user_queue[subif_index->queue_cnt].queue_type = prio_type;
			subif_index->user_queue[subif_index->queue_cnt].prio_level = prio_level;
			subif_index->user_queue[subif_index->queue_cnt].weight = weight;
			subif_index->user_queue[subif_index->queue_cnt].sbin = tmu_hal_queue_track[q_new].sched_input;
			subif_index->user_queue[subif_index->queue_cnt].qid = qid_last_assigned;

			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_green_min = param->drop.wred.min_th0;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_green_max = param->drop.wred.max_th0;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_yellow_min = param->drop.wred.min_th1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_yellow_max = param->drop.wred.max_th1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_red = param->drop.wred.drop_th1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_probability_green = param->drop.wred.max_p0;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_probability_yellow = param->drop.wred.max_p1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.weight = param->drop.wred.weight;;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.enable = 1;	
			if(param->drop.mode == PPA_QOS_DROP_WRED) {
				subif_index->user_queue[subif_index->queue_cnt].ecfg.wred_enable = 1;
			} else if (param->drop.mode == PPA_QOS_DROP_TAIL) {
				subif_index->user_queue[subif_index->queue_cnt].ecfg.wred_enable = 0;
			}
			subif_index->netdev = netdev ;
			subif_index->queue_cnt++ ;

			subif_index->dp_subif.port_id = dp_subif->port_id;
			subif_index->dp_subif.subif = dp_subif->subif;
			if((flags & PPA_F_PPPOATM) == PPA_F_PPPOATM) {
				snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname),"%s", dev_name);
			}
			else
				snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname),"%s", netdev->name);

			tmu_hal_dump_subif_queue_info(subif_index);

			if((flags & PPA_QOS_Q_F_DEFAULT) == PPA_QOS_Q_F_DEFAULT) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Priority Level for Default Q : %d\n",prio_level);
				subif_index->default_prio = prio_level;
			}
			if((prio_level == subif_index->default_prio ) && ((flags & PPA_QOS_Q_F_DEFAULT) == PPA_QOS_Q_F_DEFAULT)) {
				cbm_queue_map_entry_t q_map;
				int32_t q_map_mask = 0, x=0;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Prio Level/No Of TMU ports --> %d/%d/ \n",__FUNCTION__,prio_level, nof_of_tmu_ports);
				/** Configure the QMAP table to connect to the Egress queue*/
				memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));

				q_map.ep = dp_subif->port_id;

				if(dp_subif->subif != 0) /* for the logical interface */ {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Deafult Q --> Flow Id %d for VLAN interface subif: %d\n",
							__FUNCTION__, flowId, dp_subif->subif);
					q_map.flowid = flowId;
					if(flowId > 3) {
						q_map.tc |= 8;
						q_map.flowid = (flowId & 0x3);
					}
					q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
						CBM_QUEUE_MAP_F_DE_DONTCARE |
						CBM_QUEUE_MAP_F_TC_DONTCARE |
						CBM_QUEUE_MAP_F_MPE1_DONTCARE |
						CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Map Default Q: %d\n",__FUNCTION__,q_new);
					tmu_hal_add_q_map(q_new, CPU_PORT_ID, &q_map, q_map_mask);
					subif_index->default_q = q_new;
					for(x=1; x<subif_index->queue_cnt; x++) {
						q_map_mask = 0;
						/** Configure the QMAP table to connect to the Egress queue*/
						memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));

						q_map.ep = dp_subif->port_id;
						q_map.flowid = flowId;
						q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
						for(i=0; i< MAX_TC_NUM; i++) {
							q_map.tc = subif_index->user_queue[x].qmap[i];
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Default Q --> Add QMAP for Port %d and TC: %d\n",
									__FUNCTION__,q_map.ep,q_map.tc);
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Default Q --> User Index %d Q Index: %d\n",
									__FUNCTION__, x, subif_index->user_queue[x].queue_index);
							tmu_hal_add_q_map(subif_index->user_queue[x].queue_index, CPU_PORT_ID, &q_map, q_map_mask);
						}
					}
				} else { /*default Q of Physical interface*/
					q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
						CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
						CBM_QUEUE_MAP_F_EN_DONTCARE |
						CBM_QUEUE_MAP_F_DE_DONTCARE |
						CBM_QUEUE_MAP_F_TC_DONTCARE |
						CBM_QUEUE_MAP_F_MPE1_DONTCARE |
						CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Map Default Q: %d\n",__FUNCTION__,q_new);
					tmu_hal_add_q_map(q_new, CPU_PORT_ID, &q_map, q_map_mask);
					subif_index->default_q = q_new;
					for(x=1; x<subif_index->queue_cnt; x++) {
						q_map_mask = 0;
						/** Configure the QMAP table to connect to the Egress queue*/
						memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));

						q_map.ep = dp_subif->port_id;
						q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
							CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
							CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;

						for(i=0; i< MAX_TC_NUM; i++) {
							q_map.tc = subif_index->user_queue[x].qmap[i];
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Deafult Q --> Add QMAP for Port %d and TC: %d\n",
									__FUNCTION__,q_map.ep,q_map.tc);
							TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Default Q --> User Index %d Q Index: %d\n",
									__FUNCTION__, x, subif_index->user_queue[x].queue_index);
							tmu_hal_add_q_map(subif_index->user_queue[x].queue_index, CPU_PORT_ID, &q_map, q_map_mask);
						}
					}	
				}
			} else { /* not for default queue*/
				cbm_queue_map_entry_t q_map;
				uint32_t q_map_mask = 0;
				/** Configure the QMAP table to connect to the Egress queue*/
				memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
				q_map.ep = dp_subif->port_id;
				for(i=0; i< MAX_TC_NUM; i++)
				{
					if(tc[i] != 0)
						q_map.tc = *(tc+i);
					else
						break;

					if(dp_subif->subif != 0) {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Flow Id %d for VLAN interface subif: %d\n",__FUNCTION__, flowId,
							dp_subif->subif);
						q_map.flowid = flowId;
						if(flowId > 3) {
							q_map.tc |= 8;
							q_map.flowid = (flowId & 0x3);
						}
						q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					} else {

						q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
							CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
							CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					}
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add QMAP for Port %d and TC: %d\n",__FUNCTION__,q_map.ep,q_map.tc);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Index of New Q: %d\n",__FUNCTION__,q_new);
					cbm_queue_map_set(g_Inst, q_new, &q_map, q_map_mask);
					subif_index->user_queue[subif_index->queue_cnt].qmap[i] = *(tc+i);
				}	

			}

			/** We need to make sure that the ingress queue mapping is retained
				after changing the queue map for adding any queue. 
			 */
			memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
			q_map_mask =0;

			q_map.ep = dp_subif->port_id;
			q_map.flowid = 0;
			q_map.enc = 1;
			q_map.dec =1;
			q_map.mpe1 =0;
			q_map.mpe2 =0;
			q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE; 
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Set QMAP for the Ingress Q: %d\n",__FUNCTION__,res.q_ingress);
			tmu_hal_add_q_map(res.q_ingress, CPU_PORT_ID, &q_map, q_map_mask);

		}
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> TMU Port:[%d] has number of queues:[%d]\n", __FUNCTION__, tmu_hal_queue_track[q_new].epn, tmu_hal_port_track[tmu_hal_queue_track[q_new].epn].no_of_queues);

	/* If the number of queues on TMU port is greater than 8, increase all color
	 * threshold of the port to 0x240.
	 */
	if(tmu_hal_port_track[tmu_hal_queue_track[q_new].epn].no_of_queues > 8)	{
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Increasing Port Threshold of TMU Port %d\n", __FUNCTION__, tmu_hal_queue_track[q_new].epn);
		tmu_egress_port_tail_drop_thr_get(tmu_hal_queue_track[q_new].epn, &epth);
		epth.epth[0] = TMU_HAL_PORT_THRESHOLD_16TC;
		epth.epth[1] = TMU_HAL_PORT_THRESHOLD_16TC;
		epth.epth[2] = TMU_HAL_PORT_THRESHOLD_16TC;
		epth.epth[3] = TMU_HAL_PORT_THRESHOLD_16TC;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Threshold of TMU Port %d Increased to %x\n", __FUNCTION__, tmu_hal_queue_track[q_new].epn, TMU_HAL_PORT_THRESHOLD_16TC);
		tmu_egress_port_tail_drop_thr_set(tmu_hal_queue_track[q_new].epn, &epth);
	}

	kfree(dp_subif);
	kfree(tmu_res);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return qid_last_assigned ;
}

int tmu_hal_add_queue(
		struct net_device *netdev, 
		char *dev_name, 
		char *tc,
		uint8_t no_of_tc,
		uint32_t schedid, 
		uint32_t prio_type, 
		uint32_t prio_level, 
		uint32_t weight, 
		uint32_t flowId, 
		QOS_Q_ADD_CFG *params,
		uint32_t flags)
{

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);	

	if((flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) {
		if(g_MPE_PortId != -1 && (mpe_hal_set_checksum_queue_map_hook_fn != NULL)) {
			tmu_hal_add_mpe_ingress_queue(
						netdev,
						dev_name,
						tc,
						no_of_tc,
						schedid,
						prio_type,
						prio_level,
						weight,
						flowId,
						params, 
						flags);
		}
		return tmu_hal_add_ingress_queue(
						netdev,
						dev_name,
						tc,
						no_of_tc,
						schedid,
						prio_type,
						prio_level,
						weight,
						flowId,
						params, 
						flags);
	} else { /* Handling for Egress interface */
		return tmu_hal_add_egress_queue(
						netdev,
						dev_name,
						tc,
						no_of_tc,
						schedid,
						prio_type,
						prio_level,
						weight,
						flowId,
						params, 
						flags);
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return TMU_HAL_STATUS_OK;
}

int tmu_hal_get_no_of_q_for_prio_lvl(struct tmu_hal_user_subif_abstract *subif_index, uint32_t prio,
					uint32_t *qindex, struct tmu_hal_user_subif_abstract *sub_index, uint32_t flags)
{
	int32_t count=0 ,i, q_match=0;
	struct tmu_hal_user_subif_abstract *sub_match = NULL;
	for(i = 0; i < subif_index->queue_cnt && subif_index->queue_cnt <= TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE; i++) {
		if(subif_index->user_queue[i].prio_level == prio) {
			count++;
			q_match = i;
		}
	}

	if(subif_index->is_logical == 0) {
		int t;
		struct tmu_hal_user_subif_abstract *temp_index = NULL;
				
		temp_index = subif_index +1;
		for(t=1; t < TMU_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_HIGH,"<%s> Sub interface %d User level %d\n",__FUNCTION__,t,temp_index->port_sched_in_user_lvl);
			
			if((temp_index->port_sched_in_egress & 7 ) == prio){
				count++;
				sub_match = temp_index;
			
			}
			temp_index += 1;
		} 
	}

	if(count == 1) {
		if(subif_index->is_logical == 0 || flags & PPA_QOS_Q_F_VAP_QOS)
		{
			*qindex = subif_index->user_queue[q_match].queue_index;
			sub_index = sub_match;
		}
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of Queue of priority %d is %d\n",__FUNCTION__,prio,count);
	return count;
}

int tmu_hal_get_user_index_from_q_index(struct tmu_hal_user_subif_abstract *subif_index, uint32_t qid)
{
	int i=0;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif Queue count %d\n",__FUNCTION__,subif_index->queue_cnt);
	for(i = 0; i <subif_index->queue_cnt; i++) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_HIGH,"<%s> qid = %d user req qid =%d\n",__FUNCTION__, subif_index->user_queue[i].queue_index, qid);
		if(subif_index->user_queue[i].queue_index == qid) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Matched qid =%d\n",__FUNCTION__, i);
			return i;
		}
	}
	return TMU_HAL_STATUS_ERR;
}

int tmu_hal_get_user_index_from_qid(struct tmu_hal_user_subif_abstract *subif_index, uint32_t qid)
{
	int i=0;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif Queue count %d\n",__FUNCTION__,subif_index->queue_cnt);
	for(i = 0; i <subif_index->queue_cnt; i++) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_HIGH,"<%s> qid = %d user req qid =%d\n",__FUNCTION__, subif_index->user_queue[i].qid, qid);
		if(subif_index->user_queue[i].qid == qid) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Matched qid =%d\n",__FUNCTION__, i);
			return i;
		}
	}
	return TMU_HAL_STATUS_ERR;
}

/**
	This function is called when there is only 1 queue left for that priority level. 
	So the WFQ scheduler which is used to connect all the same priority level queue, 
	can be deleted and the only remaining queue can be connected to base scheduler leaf.
	
	This is function is called from tmu_hal_delete_queue().
*/
int tmu_hal_reconnect_q_to_high_sched_lvl(
		struct net_device *netdev, 
		char *dev_name, 
		struct tmu_hal_user_subif_abstract *subif_index,
		uint32_t q_index, 
		uint32_t port, 
		uint32_t sched_connect_to, 
		uint32_t* reconn_sched_in,
		uint32_t flags)
{
	struct tmu_hal_equeue_create q_reconnect;
	uint32_t schedin, reconn_sched, index;
	uint32_t cfg_shaper,user_q_index;
	struct tmu_hal_equeue_cfg q_param = {0};
	cbm_queue_map_entry_t *q_map_get = NULL;							
	int32_t no_entries =0;
	struct tmu_hal_token_bucket_shaper_cfg cfgShaper = {0}; 

	schedin = tmu_hal_queue_track[q_index].connected_to_sched_id;
	reconn_sched = tmu_hal_sched_track[sched_connect_to].omid ;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is attached to this Queue\n",__FUNCTION__,tmu_hal_queue_track[q_index].tb_index);
	cfg_shaper = tmu_hal_queue_track[q_index].tb_index;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Queue %d is connected to the Scheduler %d\n",__FUNCTION__,q_index, schedin);

	if (cbm_queue_map_get(g_Inst, q_index, &no_entries, &q_map_get, 0) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_ERR(TMU_DEBUG_ERR, "CBM FAILURE: Queue %d could not be mapped properly !!\n", q_index);
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> %d No of entries is %d for queue=%d\n", __FUNCTION__,__LINE__, no_entries, q_index);
	tmu_hal_safe_queue_and_shaper_delete(netdev, dev_name, q_index, -1, cfg_shaper,	-1, flags);

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Scheduler %d\n",__FUNCTION__, sched_connect_to);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disabling Scheduler input %d\n",__FUNCTION__,reconn_sched);
	tmu_sched_blk_in_enable(reconn_sched, 0);
	tmu_hal_scheduler_delete(sched_connect_to);

	if( (index = tmu_hal_get_user_index_from_q_index(subif_index, q_index) ) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d Invalid Queue Id\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_INVALID_QID;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Reconnecting the queue %d to scheduler %d\n",q_index, sched_connect_to>>3);
	memset(&q_reconnect, 0, sizeof(struct tmu_hal_equeue_create));
	q_reconnect.index = q_index;				
	q_reconnect.egress_port_number = port;
	q_reconnect.scheduler_input =	reconn_sched; 
	if((subif_index->user_queue[tmu_hal_queue_track[q_index].user_q_idx].queue_type) == TMU_HAL_POLICY_STRICT)
	{
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Strict Priority Queue\n");
		q_reconnect.iwgt = 0;
	} else if ((subif_index->user_queue[tmu_hal_queue_track[q_index].user_q_idx].queue_type) == TMU_HAL_POLICY_WFQ) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Weighted Fair Queue\n");
		q_reconnect.iwgt = 1000/(subif_index->user_queue[tmu_hal_queue_track[q_index].user_q_idx].weight);
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Reconnecting the queue %d of weight %d to scheduler input %d\n",
			q_index, q_reconnect.iwgt, q_reconnect.scheduler_input );

	tmu_hal_egress_queue_create(&q_reconnect);

	if(cfg_shaper != 0xFF) {
		cfgShaper.enable = true;
		cfgShaper.mode = tmu_hal_shaper_track[cfg_shaper].tb_cfg.mode;
		cfgShaper.cir = (tmu_hal_shaper_track[cfg_shaper].tb_cfg.cir * 1000) / 8;
		cfgShaper.pir = (tmu_hal_shaper_track[cfg_shaper].tb_cfg.pir * 1000 ) / 8;
		cfgShaper.cbs = tmu_hal_shaper_track[cfg_shaper].tb_cfg.cbs;
		cfgShaper.pbs = tmu_hal_shaper_track[cfg_shaper].tb_cfg.pbs;
		cfgShaper.index = cfg_shaper;

		tmu_hal_token_bucket_shaper_cfg_set(&cfgShaper);
		/*Add the token to the scheduler input */
		tmu_hal_token_bucket_shaper_create(cfg_shaper, q_reconnect.scheduler_input);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is created for scheduler input %d \n",
				__FUNCTION__, cfg_shaper, q_reconnect.scheduler_input);
	}
	user_q_index = tmu_hal_queue_track[q_index].user_q_idx;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Configure Drop params for mode %s \n",
			__FUNCTION__, (subif_index->user_queue[user_q_index].ecfg.wred_enable == 0)?"DT":"WRED");
	q_param.index = q_index;
	q_param.enable = 1;
	q_param.wred_enable = subif_index->user_queue[user_q_index].ecfg.wred_enable;
	q_param.drop_threshold_green_min = subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_min;

	/* When System default Queue is moved up/down default thresholds are set to 0x24 */
	if (user_q_index == 0 && subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_max == 0)
		q_param.drop_threshold_green_max = 3 * TMU_GREEN_DEFAULT_THRESHOLD;
	else
		q_param.drop_threshold_green_max = subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_max;
	q_param.drop_threshold_yellow_min = subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_min;
	if (user_q_index == 0 && subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_max == 0)
		q_param.drop_threshold_yellow_max = TMU_GREEN_DEFAULT_THRESHOLD;
	else
		q_param.drop_threshold_yellow_max = subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_max;
	q_param.drop_probability_green = subif_index->user_queue[user_q_index].ecfg.drop_probability_green;
	q_param.drop_probability_yellow = subif_index->user_queue[user_q_index].ecfg.drop_probability_yellow;
	q_param.avg_weight = subif_index->user_queue[user_q_index].ecfg.weight;
	if (user_q_index == 0 && subif_index->user_queue[user_q_index].ecfg.drop_threshold_red == 0)
		q_param.drop_threshold_red = 0x0;
	else
		q_param.drop_threshold_red = subif_index->user_queue[user_q_index].ecfg.drop_threshold_red;

	tmu_hal_egress_queue_cfg_set(&q_param);

	if (no_entries > 0) {
		int32_t j;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Q map pointer =%p\n", q_map_get);
		for (j=0; j<no_entries;j++)
			cbm_queue_map_set(g_Inst, q_index, &q_map_get[j], 0);
		no_entries = 0;
	}
	kfree(q_map_get);
	q_map_get = NULL;

	tmu_hal_queue_track[q_index].connected_to_sched_id = reconn_sched >> 3;
	tmu_hal_queue_track[q_index].sched_input = reconn_sched;

	subif_index->user_queue[index].sbin = reconn_sched;
	*reconn_sched_in = reconn_sched;

	return TMU_HAL_STATUS_OK;
}

int tmu_hal_delete_mpe_ingress_queue(
		struct net_device *netdev, 
		char *dev_name, 
		uint32_t q_id, 
		uint32_t priority,
		uint32_t scheduler_id, 
		uint32_t flags)
{
	uint32_t i=0, connected_sched, index;
	uint16_t base_sched;
	uint32_t nof_of_tmu_ports;
	int32_t prio;
	int32_t ret = TMU_HAL_STATUS_OK;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
	int32_t tmuport, tmu_q_sys;
	int32_t recnct=255;
	char *devname;

	cbm_queue_map_entry_t *q_map_get = NULL;
	int32_t num_entries=0;
	struct tmu_egress_port_thr epth;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);	

	TMU_ALLOC(dp_subif_t, dp_subif);
	devname = dev_name;
	dp_subif->subif	= dp_subif->subif >> 8;
	if(cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		kfree(tmu_res);
		return TMU_HAL_STATUS_ERR;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
			__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Delete Qid: %d\n",__FUNCTION__,q_id);	

	tmuport = tmu_res->tmu_port;
	if(flags & PPA_QOS_Q_F_INGRESS) {
		if(( tmuport = (tmu_hal_get_mpe_ingress_index(flags))) == TMU_HAL_STATUS_ERR) {
			kfree(dp_subif);
			ret = TMU_HAL_STATUS_ERR;
			goto CBM_RESOURCES_CLEANUP;
		}

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Group index is %d\n ",__FUNCTION__,tmuport);

		subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) +	dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	} else {
		subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	}

	if(flags & PPA_QOS_Q_F_INGRESS) {
		tmu_q_sys = g_MPE_Queue;
		tmuport = g_MPE_PortId;
	}
	else
		tmu_q_sys = tmu_res->tmu_q;

	if( (index = tmu_hal_get_user_index_from_qid(subif_index, q_id) ) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d Invalid Queue Id\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		ret = TMU_HAL_STATUS_INVALID_QID;
		goto CBM_RESOURCES_CLEANUP;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete User Queue Index =%d \n",__FUNCTION__, index);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Actual Queue Index %d\n",__FUNCTION__,subif_index->user_queue[index].queue_index);

	prio = subif_index->user_queue[index].prio_level; 
	
	/** If the delete is for Egress queue and for the physical interface 
		--> then don't allow to delete the reserved queue
	 */
	if((! (flags & PPA_QOS_Q_F_INGRESS)) && subif_index->is_logical != 1) {
		if(subif_index->user_queue[index].queue_index < g_No_Of_TMU_Res_Queue) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Reserved Queue %d can't be deleted \n",__FUNCTION__,subif_index->user_queue[index].queue_index);
			kfree(dp_subif);
			ret = TMU_HAL_STATUS_ERR;
			goto CBM_RESOURCES_CLEANUP;
		}
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> User Queue sbin %d	Actual Queue SBIN:%d\n",
			__FUNCTION__,subif_index->user_queue[index].sbin, tmu_hal_queue_track[subif_index->user_queue[index].queue_index].sched_input);

	subif_index->user_queue[index].sbin = tmu_hal_queue_track[subif_index->user_queue[index].queue_index].sched_input;

	if(subif_index->default_q != 0) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Deafult Queue %d \n",__FUNCTION__, subif_index->default_q);
		recnct = subif_index->default_q;	
	} else	{
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>QoS Deafult Queue %d \n",__FUNCTION__, tmu_q_sys);
		recnct = tmu_q_sys;
	}

	if (cbm_queue_map_get(
		g_Inst,
		subif_index->user_queue[index].queue_index,
			&num_entries, &q_map_get, 0) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_ERR(TMU_DEBUG_ERR, "CBM FAILURE: Queue %d could not be mapped properly !!\n", subif_index->user_queue[index].queue_index);
	}
	
	if(prio == subif_index->default_prio ) {

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Prio Level/No Of TMU ports --> %d/%d/ \n",prio,nof_of_tmu_ports);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Default Queue %d has mapped entries %d \n",__FUNCTION__,subif_index->user_queue[index].queue_index,num_entries);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap to System Default Q: %d\n",__FUNCTION__,tmu_q_sys);

		if (num_entries > 0) {
			for(i = 0; i < num_entries; i++) {
				tmu_hal_add_q_map(tmu_q_sys, CPU_PORT_ID, &q_map_get[i], 0);
			}
		}

		subif_index->default_q = 0;
	} else {
		cbm_queue_map_entry_t q_map;
		uint32_t q_map_mask =0;
		for(i = 0; i < subif_index->user_queue[index].no_of_tc; i++) {
			/** Configure the QMAP table to connect to the DP egress queue*/
			memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));

			if (flags & PPA_QOS_Q_F_INGRESS) {
				q_map.ep = PMAC_CPU_PORT_ID;
			} else {
				q_map.ep = dp_subif->port_id;
			}

			q_map.tc = subif_index->user_queue[index].qmap[i];
			q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
				CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
				CBM_QUEUE_MAP_F_EN_DONTCARE |
				CBM_QUEUE_MAP_F_DE_DONTCARE |
				CBM_QUEUE_MAP_F_MPE1_DONTCARE |
				CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete QMAP for Port %d and TC: %d\n",__FUNCTION__,q_map.ep,q_map.tc);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap to Q: %d\n",__FUNCTION__,tmu_q_sys);
			if(subif_index->default_q != 0) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap to default Q: %d\n",__FUNCTION__,subif_index->default_q);
				tmu_hal_add_q_map(subif_index->default_q, CPU_PORT_ID, &q_map, q_map_mask);
			} else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap to Q: %d\n",__FUNCTION__,tmu_q_sys);
				tmu_hal_add_q_map(tmu_q_sys, CPU_PORT_ID, &q_map, q_map_mask);
			}
		} /* for loop no_of_tc */
	}
	kfree(dp_subif);

	if (num_entries > 0) {
		num_entries =0;
	}
	kfree(q_map_get);
	q_map_get = NULL;

	tmu_hal_safe_queue_and_shaper_delete(netdev, devname, subif_index->user_queue[index].queue_index, -1, 
		tmu_hal_queue_track[subif_index->user_queue[index].queue_index].tb_index,	-1, (flags | TMU_HAL_DEL_SHAPER_CFG));

	connected_sched = subif_index->user_queue[index].sbin >> 3;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Queue sbin %d sb:%d\n",__FUNCTION__,subif_index->user_queue[index].sbin, connected_sched);
	base_sched = tmu_hal_sched_track[connected_sched].omid >> 3;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base_sched:[%d]\n",__FUNCTION__, base_sched);

	tmu_hal_shift_up_sbin_conn(netdev, devname, tmuport, subif_index, subif_index->user_queue[index].sbin, flags);
	subif_index->queue_cnt--;

	/** For all the logical interface if the last queue is getting deleted, then
		1> Delete then scheduler 
		2> Disable the scheduler input of the connected scheduler
		3> If the connected scheduler is Port scheduler then shift up the all the connected index of port scheduler to that index
		4> Update the subif_index 
	 */		
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s:%d> Queue count:[%d]\n",
			__FUNCTION__, __LINE__, subif_index->queue_cnt);
	if ((subif_index->is_logical == 1 && subif_index->queue_cnt == 0)
			|| ((flags & PPA_QOS_Q_F_INGRESS) && (subif_index->queue_cnt == 0))) {
		uint32_t omid, port_omid;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> No more queue for the sub interface\n",__FUNCTION__);
		/*Delete the sub interface base scheduler */
		omid = tmu_hal_sched_track[subif_index->base_sched_id_egress].omid;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Scheduler subif_index: base_sched_id_egress: %d port_sched_in_egress: %d OMID: %d\n",
				__FUNCTION__,subif_index->base_sched_id_egress, subif_index->port_sched_in_egress, omid);
		/*tmu_sched_blk_in_enable(subif_index->port_sched_in_egress, 0);*/
		tmu_sched_blk_in_enable(omid, 0);
		tmu_hal_scheduler_delete(subif_index->base_sched_id_egress);

		if(omid == subif_index->port_sched_in_egress)
			tmu_hal_shift_up_sbin_conn(netdev, devname, tmuport, port_subif_index, subif_index->port_sched_in_egress, flags);
		else if((omid >> 3 )== subif_index->port_shaper_sched) {
			QOS_RATE_SHAPING_CFG cfg;
			cfg.dev_name = devname;
			cfg.flag = flags;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Need to Delete Port rate shaper %d\n",__FUNCTION__, (omid >> 3));
			/** Base scheduler is already deleted. So update the next_sched_id of the
				port rate shaper scheduler even before deleting it. 
			 */
			port_omid = tmu_hal_sched_track[omid >> 3].omid;
			tmu_hal_sched_track[omid >> 3].next_sched_id = INVALID_SCHED_ID;
			if(tmu_hal_del_port_rate_shaper(netdev, &cfg, flags) == TMU_HAL_STATUS_OK) {
				tmu_sched_blk_in_enable(port_omid, 0);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shift the Port egress input up %d\n",__FUNCTION__, subif_index->port_sched_in_egress);
				tmu_hal_shift_up_sbin_conn(netdev, devname, tmuport, port_subif_index, subif_index->port_sched_in_egress, flags);
			}
		} else {
			if(tmu_hal_sched_track[omid >> 3].leaf_mask == 0) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Nothing connected to any leaf. Delete scheduler %d\n",__FUNCTION__, (omid >> 3));
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Omid: %d\n",__FUNCTION__, tmu_hal_sched_track[omid >> 3].omid);				
				tmu_sched_blk_in_enable(tmu_hal_sched_track[omid >> 3].omid, 0);
				tmu_hal_scheduler_delete(omid >> 3);
				tmu_hal_shift_up_sbin_conn(netdev, devname, tmuport, port_subif_index, subif_index->port_sched_in_egress, flags);
			}
		}
		subif_index->base_sched_id_egress = INVALID_SCHED_ID;
		subif_index->port_sched_in_egress = NULL_SCHEDULER_INPUT_ID;

		/* Update SBIN weights of base scheduler for ingress */
		if (flags & PPA_QOS_Q_F_INGRESS) {
			tmu_hal_update_ingress_sched_weight(base_sched);
		}

		ret = TMU_HAL_STATUS_OK;
		goto CBM_RESOURCES_CLEANUP;
	}

	/** Now after deleting the queue, we have to rearrange the user queue info buffer
		1> Compaction --> move up all the queue index information till the deleted index.
		2> If all the queues of that scheduler are deleted, then delete the scheduler and 
		adjust the priority level. 
	 */	
	if(subif_index->queue_cnt != 0) {
		uint32_t temp;
		uint32_t qindex = 0xFF;
		struct tmu_hal_user_subif_abstract *sub_index = NULL;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Index %d Queue Count is %d\n",index,subif_index->queue_cnt);
		for(i = index; i <subif_index->queue_cnt; i++) {
			temp = subif_index->user_queue[index].sbin;
			memcpy(&subif_index->user_queue[i], &subif_index->user_queue[i+1], sizeof(struct tmu_hal_user_queue_info));
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"i=%d temp sbin %d user sbin %d\n",i,temp , subif_index->user_queue[i].sbin);
			tmu_hal_queue_track[subif_index->user_queue[i+1].queue_index].user_q_idx = i;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Q %d -> User Index %d\n",
					subif_index->user_queue[i+1].queue_index , 
					tmu_hal_queue_track[subif_index->user_queue[i+1].queue_index].user_q_idx);
		}
		subif_index->user_queue[i].queue_index = 0xFF;
		subif_index->user_queue[i].sbin = NULL_SCHEDULER_INPUT_ID;
		subif_index->user_queue[i].prio_level = TMU_HAL_MAX_PRIORITY_LEVEL;
		subif_index->user_queue[i].qid = 0;

		if(tmu_hal_sched_track[connected_sched].leaf_mask == 0) {
			uint32_t next_sched, peer_sched;
			next_sched = tmu_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id;
			peer_sched = tmu_hal_sched_track[tmu_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id].peer_sched_id;

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler %d has no queue connected\n",__FUNCTION__, connected_sched);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Priority Selector %d\n",tmu_hal_sched_track[connected_sched].priority_selector);
			if(tmu_hal_sched_track[connected_sched].priority_selector == 3) {
				if(tmu_hal_sched_track[tmu_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id].peer_sched_id == connected_sched) {
					tmu_hal_del_scheduler_level(netdev, tmuport, subif_index->base_sched_id_egress , connected_sched, devname, flags);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New Base Scheduler %d \n",__FUNCTION__, next_sched);
					subif_index->base_sched_id_egress = next_sched;
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Something is wrong\n");
				}
			} else if(tmu_hal_sched_track[connected_sched].priority_selector ==2 ) {
				if(tmu_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id == connected_sched) {
					tmu_hal_del_scheduler_level(netdev, tmuport, subif_index->base_sched_id_egress , connected_sched, devname, flags);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New Base Scheduler %d \n",__FUNCTION__, peer_sched);
					subif_index->base_sched_id_egress = peer_sched;
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Something is wrong\n");
				}
			} else if(tmu_hal_sched_track[connected_sched].policy == TMU_HAL_POLICY_WFQ ) {

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Must be the WFQ scheduler %d of same level \n", __FUNCTION__, connected_sched);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disableing the omid %d of the scheduler \n", 
							__FUNCTION__, tmu_hal_sched_track[connected_sched].omid);
				tmu_sched_blk_in_enable(tmu_hal_sched_track[connected_sched].omid, 0);
				tmu_hal_scheduler_delete(tmu_hal_sched_track[connected_sched].id);
			}
		}

		if(tmu_hal_get_no_of_q_for_prio_lvl(subif_index, prio, &qindex, sub_index, flags) == 1)
		{
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Only 1 queue is connected this priority level of scheduler/Policy %d:%d\n",
					connected_sched, tmu_hal_sched_track[connected_sched].policy);
			if(tmu_hal_sched_track[connected_sched].policy == TMU_HAL_POLICY_WFQ) {
				uint32_t new_sched_in;
				index = 0 ;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," Connect the Queue %d to the higher scheduler level \n", qindex);
				tmu_hal_reconnect_q_to_high_sched_lvl(netdev, devname, subif_index, qindex, tmuport, connected_sched, &new_sched_in, flags);
			}
		}
		tmu_hal_dump_subif_queue_info(subif_index);
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> TMU Port:[%d] has number of queues:[%d]\n", __FUNCTION__, g_MPE_PortId, tmu_hal_port_track[g_MPE_PortId].no_of_queues);

	if(tmu_hal_port_track[g_MPE_PortId].no_of_queues == 1) {

		/* If the number of queues on TMU port for MPE is equal to 1, i,e., only
		 * system default Q is there, then set threshold default value 0x120.
		 * This threshold is increased when number of queues is greater than 1.
		 */
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Decreasing Port Threshold of TMU Port %d\n", __FUNCTION__, g_MPE_PortId);
		tmu_egress_port_tail_drop_thr_get(g_MPE_PortId, &epth);
		epth.epth[0] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		epth.epth[1] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		epth.epth[2] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		epth.epth[3] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Threshold of TMU Port %d Decreased to %x\n", __FUNCTION__, g_MPE_PortId, TMU_HAL_PORT_THRESHOLD_DEFAULT);
		tmu_egress_port_tail_drop_thr_set(g_MPE_PortId, &epth);
	}

CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return ret;
}

int tmu_hal_update_queue(
		struct net_device *netdev,
		char *dev_name,
		uint32_t q_id,
		uint32_t priority,
		uint32_t weight,
		uint32_t flags,
		QOS_Q_ADD_CFG *cfg)
{
	uint32_t mpe_flag = 0;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);


	if((flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) {
		if(g_MPE_PortId != -1 && (mpe_hal_set_checksum_queue_map_hook_fn != NULL)) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," <%s>: Update MPE Queue weights\n",__FUNCTION__);
			mpe_flag = 1;
			tmu_hal_update_q_weight(
						netdev,
						dev_name,
						q_id,
						priority,
						weight,
						flags,
						mpe_flag,
						cfg);

		}
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," <%s>: Update Ingress Queue weights\n",__FUNCTION__);
		mpe_flag = 0;
		tmu_hal_update_q_weight(
					netdev,
					dev_name,
					q_id,
					priority,
					weight,
					flags,
					mpe_flag,
					cfg);
	} else {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," <%s>: Update Egress Queue weights\n",__FUNCTION__);
		mpe_flag = 0;
		tmu_hal_update_q_weight(
					netdev,
					dev_name,
					q_id,
					priority,
					weight,
					flags,
					mpe_flag,
					cfg);
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);
	return TMU_HAL_STATUS_OK;
}

/* This function updates the new weights of the required queue */
int tmu_hal_update_q_weight(
		struct net_device *netdev,
		char *dev_name,
		uint32_t q_id,
		uint32_t priority,
		uint32_t weight,
		uint32_t flags,
		uint32_t mpe_flag,
		QOS_Q_ADD_CFG *cfg)
{

	uint32_t index;
	uint32_t nof_of_tmu_ports;
	int32_t ret = TMU_HAL_STATUS_OK;
	cbm_tmu_res_t *tmu_res;
	dp_subif_t *dp_subif = NULL;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
	PPA_VCC *vcc = NULL;
	int32_t tmuport, tmu_q_sys;
	char *devname;
	struct tmu_hal_equeue_cfg q_param;


	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);

	TMU_ALLOC(dp_subif_t, dp_subif);
	devname = dev_name;
	ppa_br2684_get_vcc(netdev,&vcc);
	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
		if(dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
			if(tmu_hal_get_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
				kfree(dp_subif);
				return -TMU_HAL_STATUS_NOT_AVAILABLE;
			}
		}
	}

	dp_subif->subif  = dp_subif->subif >> 8;
	if(cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != 0) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
			__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);

	tmuport = tmu_res->tmu_port;
	if(flags & PPA_QOS_Q_F_INGRESS)
	{
		if(mpe_flag) {
			if(( tmuport = (tmu_hal_get_mpe_ingress_index(flags))) == TMU_HAL_STATUS_ERR) {
				kfree(dp_subif);
				ret = TMU_HAL_STATUS_ERR;
				goto CBM_RESOURCES_CLEANUP;
			}
		} else {
			if(( tmuport = (tmu_hal_get_ingress_index(flags))) == TMU_HAL_STATUS_ERR) {
				kfree(dp_subif);
				ret = TMU_HAL_STATUS_ERR;
				goto CBM_RESOURCES_CLEANUP;
			}
		}

        TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Group index is %d\n ",__FUNCTION__,tmuport);

		subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	} else {
		subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	}

	//For Directpath interface
	if(!(mpe_flag)) {
		if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) {
			struct tmu_hal_dp_res_info res = {0};
			tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

			subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + 
							dp_subif->subif;
			port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);

		}
	}

	if(flags & PPA_QOS_Q_F_INGRESS) {
		if(mpe_flag) {
			tmu_q_sys = g_MPE_Queue;
			tmuport = g_MPE_PortId;
		} else {
			tmu_q_sys = g_CPU_Queue;
			tmuport = g_CPU_PortId;
		}
	}
	else
		tmu_q_sys = tmu_res->tmu_q;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> priority=%d high_prio_q_limit=%d\n", __FUNCTION__, priority, high_prio_q_limit);
	if(!(flags & PPA_QOS_Q_F_INGRESS)) {
		if(priority <= high_prio_q_limit) {
			int32_t base_sched;
			if(nof_of_tmu_ports > 1) {
				base_sched = (tmu_res)->tmu_sched;
				tmuport = (tmu_res)->tmu_port;
				tmu_q_sys = (tmu_res)->tmu_q;
				subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
				port_subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Low Priority Port:%d SB: %d \n",__FUNCTION__,tmuport,base_sched);
			}
		} else {
			int32_t base_sched;
			if(nof_of_tmu_ports > 1) {
				base_sched = (tmu_res+1)->tmu_sched;
				tmuport = (tmu_res+1)->tmu_port;
				tmu_q_sys = (tmu_res+1)->tmu_q;
				subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
				port_subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> High Priority Port:%d SB: %d \n",__FUNCTION__,tmuport,base_sched);
			}

		}
	}
	kfree(dp_subif);

	if( (index = tmu_hal_get_user_index_from_qid(subif_index, q_id) ) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d Invalid Queue Id\n", __FUNCTION__, __LINE__);
		ret = TMU_HAL_STATUS_INVALID_QID;
		goto CBM_RESOURCES_CLEANUP;
	}

	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> Qindex=%d QWeight=%d\n", __FUNCTION__, subif_index->user_queue[index].queue_index,tmu_hal_queue_track[subif_index->user_queue[index].queue_index].prio_weight);

	memset(&q_param, 0, sizeof(struct tmu_hal_equeue_cfg));
	q_param.index = subif_index->user_queue[index].queue_index;
	q_param.weight = weight;
	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE," <%s>: Weight= %d : qid = %d\n",__FUNCTION__, q_param.weight, q_param.index);
	tmu_hal_queue_param_update(&q_param, flags);

	subif_index->user_queue[index].weight = weight;
	tmu_hal_queue_track[subif_index->user_queue[index].queue_index].prio_weight = weight;
	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"<%s> Qindex:[%d] Prio_weight:[%d] Qweight:[%d]\n",
			__FUNCTION__, subif_index->user_queue[index].queue_index,
			tmu_hal_queue_track[subif_index->user_queue[index].queue_index].prio_weight,
			subif_index->user_queue[index].weight);

	tmu_hal_wfq_threshold_update(&q_param, cfg);

	subif_index->user_queue[index].ecfg.drop_threshold_green_min = cfg->drop.wred.min_th0;
	subif_index->user_queue[index].ecfg.drop_threshold_green_max = cfg->drop.wred.max_th0;
	subif_index->user_queue[index].ecfg.drop_threshold_yellow_min = cfg->drop.wred.min_th1;
	subif_index->user_queue[index].ecfg.drop_threshold_yellow_max = cfg->drop.wred.max_th1;
	subif_index->user_queue[index].ecfg.drop_threshold_red = cfg->drop.wred.drop_th1;
	subif_index->user_queue[index].ecfg.drop_probability_green = cfg->drop.wred.max_p0;
	subif_index->user_queue[index].ecfg.drop_probability_yellow = cfg->drop.wred.max_p1;
	subif_index->user_queue[index].ecfg.weight = cfg->drop.wred.weight;
	subif_index->user_queue[index].ecfg.enable = 1;
	if(cfg->drop.mode == PPA_QOS_DROP_WRED) {
		subif_index->user_queue[index].ecfg.wred_enable = 1;
	} else if (cfg->drop.mode == PPA_QOS_DROP_TAIL) {
		subif_index->user_queue[index].ecfg.wred_enable = 0;
	}

CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);
	return TMU_HAL_STATUS_OK;

}

int tmu_hal_delete_queue(
		struct net_device *netdev,
		char *dev_name, 
		uint32_t q_id, 
		uint32_t priority,
		uint32_t scheduler_id, 
		uint32_t flowId,
		uint32_t flags)
{
	uint32_t i=0, connected_sched, index;
	uint16_t base_sched;
	uint32_t nof_of_tmu_ports, pr_id = 0, pr_omid = 0;
	int32_t prio;
	int32_t ret = TMU_HAL_STATUS_OK;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
	PPA_VCC *vcc = NULL;
	int32_t tmuport, tmu_q_sys;
	int32_t recnct=255;
	char *devname;

	cbm_queue_map_entry_t q_map;
	uint32_t q_map_mask = 0;
	cbm_queue_map_entry_t *q_map_get = NULL;
	int32_t num_entries=0;
	struct tmu_egress_port_thr epth;

	int add_checksum = 0;
	cbm_tmu_res_t *tmu_res_checksum = NULL;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);	
	
	TMU_ALLOC(dp_subif_t, dp_subif);
	devname = dev_name;
	if (!(flags & PPA_QOS_Q_F_INGRESS)) {
		ppa_br2684_get_vcc(netdev,&vcc);
		if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
			if(dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
				if(tmu_hal_get_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
					kfree(dp_subif);
					return -TMU_HAL_STATUS_NOT_AVAILABLE;
				}
			}
		}
	}

#ifdef TMU_HAL_TEST
	dp_subif->port_id = 15;
	dp_subif->subif = scheduler_id;
#endif

	dp_subif->subif	= dp_subif->subif >> 8;
	if(cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
			__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Delete Qid: %d\n",__FUNCTION__,q_id);	

	tmuport = tmu_res->tmu_port;
	if(flags & PPA_QOS_Q_F_INGRESS) {
		if(( tmuport = (tmu_hal_get_ingress_index(flags))) == TMU_HAL_STATUS_ERR) {
			kfree(dp_subif);
			ret = TMU_HAL_STATUS_ERR;
			goto CBM_RESOURCES_CLEANUP;
		}

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Group index is %d\n ",__FUNCTION__,tmuport);

		subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	} else {
		subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		add_checksum = 0;
		nof_of_tmu_ports = 0;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Checking for checksum queue on port:[%d]\n", 
			__FUNCTION__, dp_subif->port_id);

		if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res_checksum, DP_F_CHECKSUM) == TMU_HAL_STATUS_OK) {
			add_checksum = 1;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Checksum Queue present! Qid:[%d]\n",
					__FUNCTION__, tmu_res_checksum->tmu_q);
		} else {
			kfree(tmu_res_checksum);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Checksum Queue NOT present!\n",
							__FUNCTION__);
		}
	}

	/*For Directpath interface */
	if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) {
		struct tmu_hal_dp_res_info res = {0};
		tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

		subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);

	}

	if(flags & PPA_QOS_Q_F_INGRESS) {
		tmu_q_sys = g_CPU_Queue;
		tmuport = g_CPU_PortId;
	}
	else
		tmu_q_sys = tmu_res->tmu_q;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> priority=%d high_prio_q_limit=%d\n", __FUNCTION__, priority, high_prio_q_limit);
	if(!(flags & PPA_QOS_Q_F_INGRESS)) {
		if(priority <= high_prio_q_limit) {
			int32_t base_sched;
			if(nof_of_tmu_ports > 1) {
				base_sched = (tmu_res)->tmu_sched;
				tmuport = (tmu_res)->tmu_port;
				tmu_q_sys = (tmu_res)->tmu_q;
				subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
				port_subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Low Priority Port:%d SB: %d \n",__FUNCTION__,tmuport,base_sched);
			}	
		} else {
			int32_t base_sched;
			if(nof_of_tmu_ports > 1) {
				base_sched = (tmu_res+1)->tmu_sched;
				tmuport = (tmu_res+1)->tmu_port;
				tmu_q_sys = (tmu_res+1)->tmu_q;
				subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
				port_subif_index = tmu_hal_user_sub_interface_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> High Priority Port:%d SB: %d \n",__FUNCTION__,tmuport,base_sched);
			}

		}
	}

	if( (index = tmu_hal_get_user_index_from_qid(subif_index, q_id) ) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d Invalid Queue Id\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		ret = TMU_HAL_STATUS_INVALID_QID;
		goto CBM_RESOURCES_CLEANUP;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete User Queue Index =%d \n",__FUNCTION__, index);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Actual Queue Index %d\n",__FUNCTION__,subif_index->user_queue[index].queue_index);

	prio = subif_index->user_queue[index].prio_level; 
	/** If the delete is for Egress queue and for the physical interface 
		--> then don't allow to delete the reserved queue
	 */
	if((! (flags & PPA_QOS_Q_F_INGRESS)) && subif_index->is_logical != 1) {
		if(subif_index->user_queue[index].queue_index < g_No_Of_TMU_Res_Queue) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Reserved Queue %d can't be deleted \n",__FUNCTION__,subif_index->user_queue[index].queue_index);
			kfree(dp_subif);
			ret = TMU_HAL_STATUS_ERR;
			goto CBM_RESOURCES_CLEANUP;
		}
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> User Queue sbin %d	Actual Queue SBIN:%d\n",
			__FUNCTION__,subif_index->user_queue[index].sbin, tmu_hal_queue_track[subif_index->user_queue[index].queue_index].sched_input);

	subif_index->user_queue[index].sbin = tmu_hal_queue_track[subif_index->user_queue[index].queue_index].sched_input;

	if(subif_index->default_q != 0) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Deafult Queue %d \n",__FUNCTION__, subif_index->default_q);
		recnct = subif_index->default_q;	
	} else	{
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>QoS Deafult Queue %d \n",__FUNCTION__, tmu_q_sys);
		recnct = tmu_q_sys;
	}

	if (cbm_queue_map_get(
		g_Inst,
		subif_index->user_queue[index].queue_index,
			&num_entries, &q_map_get, 0) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_ERR(TMU_DEBUG_ERR, "CBM FAILURE: Queue %d could not be mapped properly !!\n", subif_index->user_queue[index].queue_index);
	}

	if(prio == subif_index->default_prio ) {

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Prio Level/No Of TMU ports --> %d/%d/ \n",prio,nof_of_tmu_ports);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Default Queue %d has mapped entries %d \n",__FUNCTION__,subif_index->user_queue[index].queue_index,num_entries);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap to System Default Q: %d\n",__FUNCTION__,tmu_q_sys);

		if (flags & PPA_QOS_Q_F_VAP_QOS) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Deleting queue map for VAP Default queue\n",__FUNCTION__);
			memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
			q_map.ep = dp_subif->port_id;
			tmu_hal_vap_qmap_update(&q_map, &q_map_mask, dp_subif);
			q_map_mask  |= CBM_QUEUE_MAP_F_TC_DONTCARE;
			tmu_hal_add_q_map(tmu_q_sys, CPU_PORT_ID, &q_map, q_map_mask);
		} else {
			if (num_entries > 0) {
				for(i = 0; i < num_entries; i++) {
					tmu_hal_add_q_map(tmu_q_sys, CPU_PORT_ID, &q_map_get[i], 0);
				}
			}
		}
		subif_index->default_q = 0;
	} else {
		for(i = 0; i < subif_index->user_queue[index].no_of_tc; i++) {
			/** Configure the QMAP table to connect to the DP egress queue*/
			q_map_mask = 0;
			memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));

			if (flags & PPA_QOS_Q_F_INGRESS) {
				q_map.ep = PMAC_CPU_PORT_ID;
			} else {
				q_map.ep = dp_subif->port_id;
			}
 
			q_map.tc = subif_index->user_queue[index].qmap[i];
			q_map.flowid = flowId;
			if (flags & PPA_QOS_Q_F_VAP_QOS) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Deleting queue map for VAP queue\n",__FUNCTION__);
				tmu_hal_vap_qmap_update(&q_map, &q_map_mask, dp_subif);
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE, "<%s> Qmap: flowid:[0x%x] dec:[0x%x] enc:[0x%x]\n", __FUNCTION__, q_map.flowid, q_map.dec, q_map.enc);
			}
			else if (subif_index->is_logical == 1 && scheduler_id != 0) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Flowid dont care not set\n", __FUNCTION__);
				q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
					CBM_QUEUE_MAP_F_DE_DONTCARE |
					CBM_QUEUE_MAP_F_MPE1_DONTCARE |
					CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
			}
			else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> flowid dont care IS set\n", __FUNCTION__);
				q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
					CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
					CBM_QUEUE_MAP_F_EN_DONTCARE |
					CBM_QUEUE_MAP_F_DE_DONTCARE |
					CBM_QUEUE_MAP_F_MPE1_DONTCARE |
					CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
			}

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete QMAP for Port %d and TC: %d\n",__FUNCTION__,q_map.ep,q_map.tc);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> System Default Q: %d\n",__FUNCTION__,tmu_q_sys);
			if(subif_index->default_q != 0) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap to default Q: %d\n",__FUNCTION__,subif_index->default_q);
				tmu_hal_add_q_map(subif_index->default_q, CPU_PORT_ID, &q_map, q_map_mask);
			} else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap to Q: %d\n",__FUNCTION__,tmu_q_sys);
				tmu_hal_add_q_map(tmu_q_sys, CPU_PORT_ID, &q_map, q_map_mask);
			}
		} /* for loop no_of_tc */

		if (add_checksum) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> Adding q_map for checksum queue:[%d]\n",
					__FUNCTION__, tmu_res_checksum->tmu_q);
			if (tmu_hal_set_checksum_queue_map(dp_subif->port_id) != TMU_HAL_STATUS_OK) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR, "<%s> Adding q_map for checksum queue:[%d] failed!\n",
						__FUNCTION__, tmu_res_checksum->tmu_q);
			}
			kfree(tmu_res_checksum);
		}
	}
	kfree(dp_subif);

	if (num_entries > 0) {
		num_entries =0;
	}
	kfree(q_map_get);
	q_map_get = NULL;

	tmu_hal_safe_queue_and_shaper_delete(netdev, devname, subif_index->user_queue[index].queue_index, -1, tmu_hal_queue_track[subif_index->user_queue[index].queue_index].tb_index,	-1, (flags | TMU_HAL_DEL_SHAPER_CFG));

	connected_sched = subif_index->user_queue[index].sbin >> 3;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Queue sbin %d sb:%d\n",__FUNCTION__,subif_index->user_queue[index].sbin, connected_sched);
	base_sched = tmu_hal_sched_track[connected_sched].omid >> 3;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Base_sched:[%d]\n",__FUNCTION__, base_sched);

	tmu_hal_shift_up_sbin_conn(netdev, devname, tmuport, subif_index, subif_index->user_queue[index].sbin, flags);
	subif_index->queue_cnt--;

	/** For all the logical interface if the last queue is getting deleted, then
		1> Delete then scheduler 
		2> Disable the scheduler input of the connected scheduler
		3> If the connected scheduler is Port scheduler then shift up the all the connected index of port scheduler to that index
		4> Update the subif_index 
	 */		
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s:%d> Queue count:[%d]\n",
			__FUNCTION__, __LINE__, subif_index->queue_cnt);
	if ((subif_index->is_logical == 1 && subif_index->queue_cnt == 0)
			|| ((flags & PPA_QOS_Q_F_INGRESS) && (subif_index->queue_cnt == 0))) {
		uint32_t omid, port_omid, leaf = 0, vap_sched = 0;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> No more queue for the sub interface\n",__FUNCTION__);
		/* Delete the sub interface base scheduler */
		omid = tmu_hal_sched_track[subif_index->base_sched_id_egress].omid;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Scheduler subif_index: base_sched_id_egress: %d port_sched_in_egress: %d OMID: %d\n",
				__FUNCTION__,subif_index->base_sched_id_egress, subif_index->port_sched_in_egress, omid);
		tmu_sched_blk_in_enable(omid, 0);
		tmu_hal_scheduler_delete(subif_index->base_sched_id_egress);
		leaf = tmu_hal_sched_track[omid >> 3].leaf_mask;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Leaf Mask %d\n",__FUNCTION__, leaf);

		if ((omid == subif_index->port_sched_in_egress) && (leaf != 0))
			tmu_hal_shift_up_sbin_conn(netdev, devname, tmuport, port_subif_index, subif_index->port_sched_in_egress, flags);
		else if((omid >> 3 )== subif_index->port_shaper_sched && (leaf == 0)) {
			QOS_RATE_SHAPING_CFG cfg;
			memset(&cfg, 0, sizeof(QOS_RATE_SHAPING_CFG));
			cfg.dev_name = devname;
			cfg.flag = flags;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Need to Delete Port rate shaper %d\n",__FUNCTION__, (omid >> 3));
			/** Base scheduler is already deleted. So update the next_sched_id of the
				port rate shaper scheduler even before deleting it. 
			 */
			port_omid = tmu_hal_sched_track[omid >> 3].omid;
			tmu_hal_sched_track[omid >> 3].next_sched_id = INVALID_SCHED_ID;
			if(tmu_hal_del_port_rate_shaper(netdev, &cfg, flags) == TMU_HAL_STATUS_OK) {
				tmu_sched_blk_in_enable(port_omid, 0);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shift the Port egress input up %d\n",__FUNCTION__, subif_index->port_sched_in_egress);
				tmu_hal_shift_up_sbin_conn(netdev, devname, tmuport, port_subif_index, subif_index->port_sched_in_egress, flags);
			}
		} else {
			if(tmu_hal_sched_track[omid >> 3].leaf_mask == 0) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Nothing connected to any leaf. Delete scheduler %d\n",__FUNCTION__, (omid >> 3));
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Omid: %d\n",__FUNCTION__, tmu_hal_sched_track[omid >> 3].omid);				
				vap_sched = (tmu_hal_sched_track[subif_index->port_sched_in_egress >> 3].omid >> 3);
				tmu_sched_blk_in_enable(tmu_hal_sched_track[omid >> 3].omid, 0);
				tmu_hal_scheduler_delete(omid >> 3);
				tmu_hal_shift_up_sbin_conn(netdev, devname, tmuport, port_subif_index, subif_index->port_sched_in_egress, flags);
				if (flags & PPA_QOS_Q_F_VAP_QOS) {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> vap sched: %d\n",__FUNCTION__, vap_sched);
					tmu_hal_sched_track[vap_sched].next_sched_id = INVALID_SCHED_ID;
				}
			}
		}
		subif_index->base_sched_id_egress = INVALID_SCHED_ID;
		subif_index->port_sched_in_egress = NULL_SCHEDULER_INPUT_ID;

		/* Update SBIN weights of base scheduler for ingress */
		if (flags & PPA_QOS_Q_F_INGRESS || flags & PPA_QOS_Q_F_VAP_QOS) {
			tmu_hal_update_ingress_sched_weight(base_sched);
		}

		ret = TMU_HAL_STATUS_OK;
		goto CBM_RESOURCES_CLEANUP;
	}

	/** Now after deleting the queue, we have to rearrange the user queue info buffer
		1> Compaction --> move up all the queue index information till the deleted index.
		2> If all the queues of that scheduler are deleted, then delete the scheduler and 
		adjust the priority level. 
	 */	
	if(subif_index->queue_cnt != 0) {
		uint32_t temp;
		uint32_t qindex = 0xFF;
		struct tmu_hal_user_subif_abstract *sub_index = NULL;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Index %d Queue Count is %d\n",index,subif_index->queue_cnt);
		for(i = index; i <subif_index->queue_cnt; i++) {
			temp = subif_index->user_queue[index].sbin;
			memcpy(&subif_index->user_queue[i], &subif_index->user_queue[i+1], sizeof(struct tmu_hal_user_queue_info));
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"i=%d temp sbin %d user sbin %d\n",i,temp , subif_index->user_queue[i].sbin);
			tmu_hal_queue_track[subif_index->user_queue[i+1].queue_index].user_q_idx = i;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Q %d -> User Index %d\n",
					subif_index->user_queue[i+1].queue_index , 
					tmu_hal_queue_track[subif_index->user_queue[i+1].queue_index].user_q_idx);
		}
		subif_index->user_queue[i].queue_index = 0xFF;
		subif_index->user_queue[i].sbin = NULL_SCHEDULER_INPUT_ID;
		subif_index->user_queue[i].prio_level = TMU_HAL_MAX_PRIORITY_LEVEL;
		subif_index->user_queue[i].qid = 0;

		if(tmu_hal_sched_track[connected_sched].leaf_mask == 0) {
			uint32_t next_sched, peer_sched;
			next_sched = tmu_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id;
			peer_sched = tmu_hal_sched_track[tmu_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id].peer_sched_id;

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Scheduler %d has no queue connected\n",__FUNCTION__, connected_sched);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Priority Selector %d\n",tmu_hal_sched_track[connected_sched].priority_selector);
			if(tmu_hal_sched_track[connected_sched].priority_selector == 3) {
				if(tmu_hal_sched_track[tmu_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id].peer_sched_id == connected_sched) {
					tmu_hal_del_scheduler_level(netdev, tmuport, subif_index->base_sched_id_egress , connected_sched, devname, flags);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New Base Scheduler %d \n",__FUNCTION__, next_sched);
					subif_index->base_sched_id_egress = next_sched;
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Something is wrong\n");
				}
			} else if(tmu_hal_sched_track[connected_sched].priority_selector ==2 ) {
				if(tmu_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id == connected_sched) {
					tmu_hal_del_scheduler_level(netdev, tmuport, subif_index->base_sched_id_egress , connected_sched, devname, flags);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> New Base Scheduler %d \n",__FUNCTION__, peer_sched);
					subif_index->base_sched_id_egress = peer_sched;
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Something is wrong\n");
				}
			} else if(tmu_hal_sched_track[connected_sched].policy == TMU_HAL_POLICY_WFQ ) {

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Must be the WFQ scheduler %d of same level \n", __FUNCTION__, connected_sched);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disableing the omid %d of the scheduler \n", 
							__FUNCTION__, tmu_hal_sched_track[connected_sched].omid);
				pr_omid = tmu_hal_sched_track[connected_sched].omid;
				/* Peer WFQ Schedular of same priority is der, Move UP the SBIN of Peer Schedular*/
				if (tmu_hal_sched_track[connected_sched].peer_sched_id > 0)
					pr_id = 1;
				tmu_sched_blk_in_enable(tmu_hal_sched_track[connected_sched].omid, 0);
				tmu_hal_scheduler_delete(tmu_hal_sched_track[connected_sched].id);
				if (pr_id == 1)
					tmu_hal_shift_up_sbin_conn(netdev, devname, tmuport, subif_index, pr_omid, flags);
			}
		}

		if(tmu_hal_get_no_of_q_for_prio_lvl(subif_index, prio, &qindex, sub_index, flags) == 1) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Only 1 queue is connected this priority level of scheduler/Policy %d:%d\n",
					connected_sched, tmu_hal_sched_track[connected_sched].policy);
			if(tmu_hal_sched_track[connected_sched].policy == TMU_HAL_POLICY_WFQ) {
				uint32_t new_sched_in;
				index = 0 ;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," Connect the Queue %d to the higher scheduler level \n", qindex);
				tmu_hal_reconnect_q_to_high_sched_lvl(netdev, devname, subif_index, qindex, tmuport, connected_sched, &new_sched_in, flags);
			}
		}
		tmu_hal_dump_subif_queue_info(subif_index);
	}

	if (flags & PPA_QOS_Q_F_INGRESS) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> TMU Port:[%d] has number of queues:[%d]\n", __FUNCTION__, g_CPU_PortId, tmu_hal_port_track[g_CPU_PortId].no_of_queues);
	} else {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> TMU Port:[%d] has number of queues:[%d]\n", __FUNCTION__, subif_index->tmu_port_idx, tmu_hal_port_track[subif_index->tmu_port_idx].no_of_queues);
	}

	/* If the number of queues on TMU port is less than 9, reset all colors
	 * threshold of the port to default value 0x120.
	 * This threshold is increased when number of queues is greater than 9.
	 * For CPU queues: if number of queues is equal to 1, i.e, only system
	 * default Q is there, then set threshold default value 0x120.
	 */
	if (!(flags & PPA_QOS_Q_F_INGRESS) && tmu_hal_port_track[subif_index->tmu_port_idx].no_of_queues <= 8) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Decreasing Port Threshold of TMU Port %d\n", __FUNCTION__, subif_index->tmu_port_idx);
		tmu_egress_port_tail_drop_thr_get(subif_index->tmu_port_idx, &epth);
		epth.epth[0] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		epth.epth[1] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		epth.epth[2] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		epth.epth[3] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Threshold of TMU Port %d Decreased to %x\n", __FUNCTION__, subif_index->tmu_port_idx, TMU_HAL_PORT_THRESHOLD_DEFAULT);
		tmu_egress_port_tail_drop_thr_set(subif_index->tmu_port_idx, &epth);
	}
	else if ((flags & PPA_QOS_Q_F_INGRESS) && tmu_hal_port_track[g_CPU_PortId].no_of_queues == 1) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Decreasing Port Threshold of TMU Port %d\n", __FUNCTION__, g_CPU_PortId);
		tmu_egress_port_tail_drop_thr_get(g_CPU_PortId, &epth);
		epth.epth[0] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		epth.epth[1] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		epth.epth[2] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		epth.epth[3] = TMU_HAL_PORT_THRESHOLD_DEFAULT;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Threshold of TMU Port %d Decreased to %x\n", __FUNCTION__, g_CPU_PortId, TMU_HAL_PORT_THRESHOLD_DEFAULT);
		tmu_egress_port_tail_drop_thr_set(g_CPU_PortId, &epth);
	}

CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return ret;
}

int tmu_hal_modify_queue(struct net_device *netdev, QOS_Q_MOD_CFG *param)
{
	int32_t ret = TMU_HAL_STATUS_OK;
	int32_t nof_of_tmu_ports, index;
	cbm_tmu_res_t *tmu_res  = NULL;
	dp_subif_t *dp_subif = NULL;
	struct tmu_hal_equeue_cfg q_param;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
	PPA_VCC *vcc = NULL;

	TMU_ALLOC(dp_subif_t, dp_subif);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify QueueId %d\n",__FUNCTION__, param->q_id); 
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Priority %d Weight %d \n",__FUNCTION__, param->priority, param->weight); 
	ppa_br2684_get_vcc(netdev,&vcc);
	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, 0) != PPA_SUCCESS) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get Subif Id\n", __FUNCTION__, __LINE__);
		return TMU_HAL_STATUS_ERR;
	}

#ifdef TMU_HAL_TEST
	dp_subif->port_id = 15;
	dp_subif->subif = 0;
#endif

	dp_subif->subif	= dp_subif->subif >> 8;
	if(cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
				__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);
	subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE," Sub interface index %p\n",subif_index);

	if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) {
		struct tmu_hal_dp_res_info res = {0};
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Adding the shaper for Directpath interface!!!\n",__FUNCTION__);
		tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);
		
		subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);
		
	}
	kfree(dp_subif);

	if( (index = tmu_hal_get_user_index_from_qid(subif_index, param->q_id) ) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Invalid Queue Id\n", __FUNCTION__, __LINE__);
		ret = TMU_HAL_STATUS_INVALID_QID;
		goto CBM_RESOURCES_CLEANUP;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify User Queue Index =%d \n",__FUNCTION__, index);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify Actual Queue Index %d\n",__FUNCTION__,subif_index->user_queue[index].queue_index);

	if(subif_index->user_queue[index].prio_level != param->priority) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify Priority level to %d\n",__FUNCTION__,param->priority);
		if(tmu_hal_delete_queue(netdev,netdev->name, param->q_id,subif_index->user_queue[index].prio_level, 0, 0, param->flags) == TMU_HAL_STATUS_OK)
		{
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Add a new Queue \n",__FUNCTION__);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> After add queue : Q_ID = %d\n",__FUNCTION__,param->q_id);
		}
		else {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Queue Deletion Failed\n", __FUNCTION__, __LINE__);
			ret = TMU_HAL_STATUS_ERR;
			goto CBM_RESOURCES_CLEANUP;
		}

	}

	memset(&q_param, 0, sizeof(struct tmu_hal_equeue_cfg));

	if(param->drop.mode == PPA_QOS_DROP_WRED) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify WRED params \n",__FUNCTION__);
		q_param.drop_threshold_green_min = param->drop.wred.min_th0;
		q_param.drop_threshold_green_max = param->drop.wred.max_th0;
		q_param.drop_threshold_yellow_min = param->drop.wred.min_th1;
		q_param.drop_threshold_yellow_max = param->drop.wred.max_th1;
		q_param.drop_probability_green = param->drop.wred.max_p0;
		q_param.drop_probability_yellow = param->drop.wred.max_p1;
		q_param.wred_enable = 1;
		q_param.enable = 1;
		q_param.avg_weight = param->drop.wred.weight;
	} else if (param->drop.mode == PPA_QOS_DROP_TAIL) {
		q_param.wred_enable = 0;
		q_param.drop_threshold_green_max = param->drop.wred.max_th0;
		q_param.drop_threshold_yellow_max = param->drop.wred.max_th1;
		q_param.drop_threshold_red = param->drop.wred.drop_th1;
	}
	q_param.weight = 1000/param->weight;

	tmu_hal_egress_queue_cfg_set(&q_param);

CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
	return ret;
}

int tmu_hal_get_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	int32_t ret = TMU_HAL_STATUS_OK;
	uint32_t index, q_index;
	uint32_t nof_of_tmu_ports;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
	PPA_VCC *vcc = NULL;


	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Add Shaper for the Queue ID %d\n", cfg->queueid);

	TMU_ALLOC(dp_subif_t, dp_subif);
	ppa_br2684_get_vcc(netdev,&vcc);
	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
		if(dp_get_port_subitf_via_ifname(cfg->dev_name, dp_subif) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
			if(tmu_hal_get_subitf_via_ifname(cfg->dev_name, dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
				kfree(dp_subif);
				return TMU_HAL_STATUS_ERR;
			}
		}
	}

#ifdef TMU_HAL_TEST
	dp_subif->port_id = 15;
	dp_subif->subif = scheduler_id;
#endif

	dp_subif->subif	= dp_subif->subif >> 8;
	if(cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
				__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);

	if(flags & PPA_QOS_Q_F_INGRESS)
	{
		subif_index = tmu_hal_user_sub_interface_ingress_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_ingress_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	} else {
		subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	}
	kfree(dp_subif);

	if( (index = tmu_hal_get_user_index_from_qid(subif_index, cfg->queueid) ) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Invalid Queue Id\n", __FUNCTION__, __LINE__);
		ret = TMU_HAL_STATUS_INVALID_QID;
		goto CBM_RESOURCES_CLEANUP;
	}

	q_index = subif_index->user_queue[index].queue_index;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Actual Queue Index %d\n",__FUNCTION__,subif_index->user_queue[index].queue_index);

	if(tmu_hal_queue_track[q_index].sched_input == 1023) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"%s:%d ERROR Wrong Queue %d\n",__FUNCTION__,__LINE__, q_index);
		ret = TMU_HAL_STATUS_ERR;
		goto CBM_RESOURCES_CLEANUP;
	} else {
		if(cfg->shaperid <= 0) {

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"%s:%d No shaper instanec is created for Queue index %d\n",__FUNCTION__,__LINE__, q_index);
			ret = TMU_HAL_STATUS_ERR;
			goto CBM_RESOURCES_CLEANUP;
		} else {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Shaper %d is already created \n",cfg->shaperid);
			cfg->shaper.phys_shaperid = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.index;
			cfg->shaper.enable = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.enable;
			cfg->shaper.mode = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.mode;
			cfg->shaper.cir = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.cir;
			cfg->shaper.pir = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.pir;
			cfg->shaper.cbs = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.cbs;
			cfg->shaper.pbs = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.pbs;
		}
	}

CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);

	return ret;
}

/** Fill shaper cfg from shaper track and Create a new shaper instance or update it.
	** 1> Fill shaper cfg structure from shaper tarck corresponding to input shaperid
	** 2> Get New free shaper and Update the shaper tracking information.
	** 3> Return the shaper index.
 */
int tmu_hal_set_cfg_shaper_from_shaper_track(QOS_RATE_SHAPING_CFG *cfg)
{

	int shaper_index;
	cfg->shaper.mode = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.mode;
	cfg->shaper.pir = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.pir;
	cfg->shaper.cir = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.cir;
	cfg->shaper.pbs = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.pbs;
	cfg->shaper.cbs = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.cbs;


	shaper_index = tmu_hal_get_free_shaper();
	TMU_HAL_ASSERT(shaper_index >= TMU_HAL_MAX_SHAPER);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>New Shaper	%d\n",__FUNCTION__, shaper_index);
	
	tmu_hal_shaper_track[shaper_index].is_enabled = true;
	tmu_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
	tmu_hal_shaper_track[shaper_index].tb_cfg.mode = cfg->shaper.mode;
	tmu_hal_shaper_track[shaper_index].tb_cfg.pir = cfg->shaper.pir;
	tmu_hal_shaper_track[shaper_index].tb_cfg.cir = cfg->shaper.cir;
	tmu_hal_shaper_track[shaper_index].tb_cfg.pbs = cfg->shaper.pbs;
	tmu_hal_shaper_track[shaper_index].tb_cfg.cbs = cfg->shaper.cbs;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return shaper_index;
}

/** Create a new shaper instance or update it.
	** 1> if shaper id <=0, then get free shaper instance
	** 2> Update the shaper tracking information for the new or given shaper index.
	** 3> Return the shaper index.
 */
int tmu_hal_add_shaper_index(QOS_RATE_SHAPING_CFG *cfg)
{
	int shaper_index;
	if(cfg->shaperid <= 0) {
		shaper_index = tmu_hal_get_free_shaper();
	} else {
		shaper_index = cfg->shaperid;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Modify Shaper %d\n",__FUNCTION__, shaper_index);
	}
	TMU_HAL_ASSERT(shaper_index >= TMU_HAL_MAX_SHAPER);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>New Shaper	%d\n",__FUNCTION__, shaper_index);
	tmu_hal_shaper_track[shaper_index].is_enabled = true;
	tmu_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
	tmu_hal_shaper_track[shaper_index].tb_cfg.mode = cfg->shaper.mode;
	tmu_hal_shaper_track[shaper_index].tb_cfg.pir = cfg->shaper.pir;
	tmu_hal_shaper_track[shaper_index].tb_cfg.cir = cfg->shaper.cir;
	tmu_hal_shaper_track[shaper_index].tb_cfg.pbs = cfg->shaper.pbs;
	tmu_hal_shaper_track[shaper_index].tb_cfg.cbs = cfg->shaper.cbs;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return shaper_index;
}


/** Add the shaper for the queue.
	** 1> Check if the tmu queue index is provided. if not then get it from the user queue index.			 
	** 2> Check if the shaper instance is provided. 
	** 3> If not then --> Get free shaper index.
	** 4> Create the shaper and configure it.
	** 5> Update the shaper tracking information.
	** 6> Configure the shaper for the queue scheduler input.
*/
int tmu_hal_add_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, int32_t tmu_q_idx, uint32_t flags)
{
	int32_t i=0, shaper_index = 0;
	int32_t ret = TMU_HAL_STATUS_OK;
	int32_t index = 0, tmuport=0; 
	uint32_t q_index = 0;
	uint32_t nof_of_tmu_ports;
	uint32_t fail_cnt = 0;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	struct tmu_hal_token_bucket_shaper_cfg cfg_shaper;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
	PPA_VCC *vcc = NULL;
	char *dev_name;

#ifdef	CBM_QUEUE_FLUSH_SUPPORT
	cbm_queue_map_entry_t *q_map_get = NULL;
	int32_t num_entries =0;
#endif

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Add Shaper for the Queue ID %d of Port Id:%d\n", cfg->queueid, cfg->portid);
	
	TMU_ALLOC(dp_subif_t, dp_subif);
	dev_name = cfg->dev_name;
	if(tmu_q_idx == -1) {
		if(netdev) {
			if(netdev != NULL)
			ppa_br2684_get_vcc(netdev,&vcc);
			if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n",
					__FUNCTION__, __LINE__);
				if(dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n",
						__FUNCTION__, __LINE__);
					if(tmu_hal_get_subitf_via_ifname(dev_name,dp_subif) != PPA_SUCCESS) {
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n",
							__FUNCTION__, __LINE__);
						kfree(dp_subif);
						return TMU_HAL_STATUS_ERR;
					}
				}
			}
		} else if(!netdev && (cfg->portid > 0)) {
			dp_subif->port_id = cfg->portid;
		} else {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR netdev and port id both are invalid \n", __FUNCTION__, __LINE__);
			kfree(dp_subif);
			return TMU_HAL_STATUS_ERR;
		}

#ifdef TMU_HAL_TEST
		dp_subif->port_id = 15;
		dp_subif->subif = scheduler_id;
#endif

		dp_subif->subif	= dp_subif->subif >> 8;
		if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
			kfree(tmu_res);
			kfree(dp_subif);
			return TMU_HAL_STATUS_ERR;
		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
				__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);

		if(flags & PPA_QOS_Q_F_INGRESS)
		{
			if((flags & TMU_HAL_QUEUE_SHAPER_INGRESS_MPE) == TMU_HAL_QUEUE_SHAPER_INGRESS_MPE) { /*for MPE Ingress Queue */
				if(( tmuport = (tmu_hal_get_mpe_ingress_index(flags))) == TMU_HAL_STATUS_ERR) {
					kfree(dp_subif);
					ret = TMU_HAL_STATUS_ERR;
					goto CBM_RESOURCES_CLEANUP;
				} 
			} else {/*for CPU Ingress queue */
				if(( tmuport = (tmu_hal_get_ingress_index(flags))) == TMU_HAL_STATUS_ERR) {
					kfree(dp_subif);
					ret = TMU_HAL_STATUS_ERR;
					goto CBM_RESOURCES_CLEANUP;
				}

			}
			subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
			port_subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		} else {
			subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
			port_subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		}

		/* Find the user qid from the user queue array*/
		for(i=0; i < nof_of_tmu_ports; i++) {
			if((flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) {
				if( (index = tmu_hal_get_user_index_from_qid(subif_index, cfg->queueid) ) == TMU_HAL_STATUS_ERR) {
					fail_cnt++;
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Qid Matched QId=%d\n",__FUNCTION__, cfg->queueid);
					break;
				}
			} else {
				if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) {
					struct tmu_hal_dp_res_info res = {0};
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Adding the shaper for Directpath interface!!!\n",__FUNCTION__);
					tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

					subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) +
									dp_subif->subif;
					port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);

				} else {
					subif_index = tmu_hal_user_sub_interface_info + ((tmu_res+i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
					port_subif_index = tmu_hal_user_sub_interface_info + ((tmu_res+i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
				}
				if( (index = tmu_hal_get_user_index_from_qid(subif_index, cfg->queueid) ) == TMU_HAL_STATUS_ERR) {
					fail_cnt++;
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Qid Matched QId=%d\n",__FUNCTION__, cfg->queueid);
					break;
				}
			}
		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Count=%d nof_of_tmu_ports=%d\n",__FUNCTION__, fail_cnt, nof_of_tmu_ports);
		if(fail_cnt == nof_of_tmu_ports) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Queue Id %d\n",__FUNCTION__, cfg->queueid);
			kfree(dp_subif);
			ret = TMU_HAL_STATUS_INVALID_QID;
			goto CBM_RESOURCES_CLEANUP;
		}
		
		/*TMU Queue Index*/
		if(index >= 0)
			q_index = subif_index->user_queue[index].queue_index;
	} else
		q_index = tmu_q_idx;

	kfree(dp_subif);
	if ( q_index > TMU_HAL_MAX_EGRESS_QUEUES-1) {
		ret = TMU_HAL_STATUS_INVALID_QID;
		goto CBM_RESOURCES_CLEANUP;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Actual Queue Index %d\n",__FUNCTION__, q_index);

#ifdef	CBM_QUEUE_FLUSH_SUPPORT
	if(cbm_queue_map_get(g_Inst, q_index, &num_entries, &q_map_get, 0) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_ERR(TMU_DEBUG_ERR, "CBM FAILURE: Queue %d could not be mapped properly !!\n", q_index);
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> %d No of entries is %d for queue=%d\n", __FUNCTION__,__LINE__, num_entries, q_index);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Q map pointer =%p\n",__FUNCTION__, q_map_get);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Queue %d for netdev %s\n",__FUNCTION__, q_index, netdev->name);
	if(tmu_hal_flush_queue_if_qocc(netdev, dev_name, q_index, -1, -1, flags) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to flush qocc\n", __FUNCTION__, __LINE__);
		ret = TMU_HAL_STATUS_ERR;
		goto CBM_RESOURCES_CLEANUP;
	}
#endif

	if(flags & PPA_QOS_Q_F_INGRESS) {
		if((flags & TMU_HAL_QUEUE_SHAPER_INGRESS_MPE) == TMU_HAL_QUEUE_SHAPER_INGRESS_MPE) { /*for MPE Ingress Queue */
			shaper_index = tmu_hal_set_cfg_shaper_from_shaper_track(cfg);
			cfg->shaperid = shaper_index;
		}
	}
	memset(&cfg_shaper, 0, sizeof(struct tmu_hal_token_bucket_shaper_cfg));
	if(tmu_hal_queue_track[q_index].sched_input == 1023) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Wrong Queue %d\n",__FUNCTION__, q_index);
		ret = TMU_HAL_STATUS_ERR;
		goto CBM_RESOURCES_CLEANUP;
	} else {
		if(cfg->shaperid <= 0) {
			/* Get free Shaper */
			if((shaper_index = tmu_hal_get_free_shaper()) >= TMU_HAL_MAX_SHAPER ) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> No shaper available !!!\n",__FUNCTION__);
				ret = TMU_HAL_STATUS_ERR;
				goto CBM_RESOURCES_CLEANUP;
			}

			tmu_hal_shaper_track[shaper_index].is_enabled = true;
			tmu_hal_shaper_track[shaper_index].sb_input = tmu_hal_queue_track[q_index].sched_input;
			tmu_hal_queue_track[q_index].tb_index = shaper_index;
			cfg_shaper.index = shaper_index;
			cfg_shaper.cir = (cfg->rate_in_kbps * 1000) / 8 ;

			tmu_hal_shaper_track[shaper_index].tb_cfg.enable = true;
			tmu_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
			tmu_hal_shaper_track[shaper_index].tb_cfg.cir = cfg->rate_in_kbps ;
			tmu_hal_shaper_track[shaper_index].tb_cfg.cbs = cfg->burst;
			tmu_hal_token_bucket_shaper_cfg_set(&cfg_shaper);
			/*Add the token to the scheduler input*/
			tmu_hal_token_bucket_shaper_create(shaper_index, tmu_hal_queue_track[q_index].sched_input);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper %d is created for scheduler input %d \n",
					__FUNCTION__,shaper_index, tmu_hal_queue_track[q_index].sched_input);
		} else {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper instance %d is already created \n",__FUNCTION__,cfg->shaperid);
			cfg_shaper.index = cfg->shaperid;
			tmu_hal_shaper_track[cfg->shaperid].tb_cfg.enable = true;
			tmu_hal_shaper_track[shaper_index].tb_cfg.index = cfg->shaperid;
			tmu_hal_queue_track[q_index].tb_index = cfg->shaperid;
			cfg_shaper.enable = true;
			cfg_shaper.mode = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.mode;
			cfg_shaper.cir = (tmu_hal_shaper_track[cfg->shaperid].tb_cfg.cir * 1000) / 8;
			cfg_shaper.pir = (tmu_hal_shaper_track[cfg->shaperid].tb_cfg.pir * 1000 ) / 8;
			cfg_shaper.cbs = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.cbs; ;
			cfg_shaper.pbs = tmu_hal_shaper_track[cfg->shaperid].tb_cfg.pbs; ;
			shaper_index = cfg->shaperid;
			tmu_hal_token_bucket_shaper_cfg_set(&cfg_shaper);
			/*Add the token to the scheduler input*/
			tmu_hal_token_bucket_shaper_create(cfg_shaper.index, tmu_hal_queue_track[q_index].sched_input);
		}
	}
	
#ifdef CBM_QUEUE_FLUSH_SUPPORT
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enable Queue %d for netdev %s\n",__FUNCTION__, q_index, netdev->name);
	if(tmu_hal_enable_queue_after_flush(netdev, dev_name, q_index, -1, q_index, flags) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to enable queue after queue flush\n", __FUNCTION__, __LINE__);
		ret = TMU_HAL_STATUS_ERR;
		goto CBM_RESOURCES_CLEANUP;
	}

	if (num_entries > 0) {
		int32_t j;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Q map pointer =%p no_entries=%d\n", q_map_get, num_entries);
		for (j=0; j<num_entries;j++)
			cbm_queue_map_set(g_Inst, q_index, &q_map_get[j], 0);
		num_entries = 0;
	}
#endif
CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
#ifdef CBM_QUEUE_FLUSH_SUPPORT
	kfree(q_map_get);
	q_map_get = NULL;
#endif

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	if((ret == TMU_HAL_STATUS_INVALID_QID) || (ret == TMU_HAL_STATUS_ERR))
		return PPA_FAILURE;
	return shaper_index;

}



int tmu_hal_del_shaper_index(int shaper_index)
{
	TMU_HAL_ASSERT(shaper_index >= TMU_HAL_MAX_SHAPER);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Shaper instance	%d\n",__FUNCTION__, shaper_index);
	tmu_hal_shaper_track[shaper_index].is_enabled = false;
	tmu_hal_shaper_track[shaper_index].tb_cfg.index = 0xFF;
	tmu_hal_shaper_track[shaper_index].tb_cfg.enable = false;
	tmu_hal_shaper_track[shaper_index].tb_cfg.mode = 0;
	tmu_hal_shaper_track[shaper_index].tb_cfg.pir = 0;
	tmu_hal_shaper_track[shaper_index].tb_cfg.cir = 0;
	tmu_hal_shaper_track[shaper_index].tb_cfg.pbs = 0;
	tmu_hal_shaper_track[shaper_index].tb_cfg.cbs = 0;
	return TMU_HAL_STATUS_OK;
}

int tmu_hal_del_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, int32_t tmu_q_idx, uint32_t flags)
{
	int32_t i=0, shaper_index,tmuport=0;
	int32_t ret = TMU_HAL_STATUS_OK;
	uint32_t index, q_index , fail_cnt=0;
	uint32_t nof_of_tmu_ports;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
	PPA_VCC *vcc = NULL;
	char *dev_name;

#ifdef CBM_QUEUE_FLUSH_SUPPORT
	cbm_queue_map_entry_t *q_map_get = NULL;
		int32_t num_entries =0;
#endif

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Delete Shaper for the Queue ID %d\n",__FUNCTION__, cfg->queueid);

	TMU_HAL_ASSERT(cfg->shaperid >= TMU_HAL_MAX_SHAPER);
	dev_name = cfg->dev_name;

	TMU_ALLOC(dp_subif_t, dp_subif);
	if(tmu_q_idx == -1) {
		if(cfg->portid <= 0) {
			if(netdev != NULL)
			ppa_br2684_get_vcc(netdev,&vcc);

			if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
				if(dp_get_port_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,
						"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
					if(tmu_hal_get_subitf_via_ifname(dev_name, dp_subif) != PPA_SUCCESS) {
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,
							"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
						kfree(dp_subif);
						return TMU_HAL_STATUS_ERR;
					}
				}
			}
		} else
			dp_subif->port_id = cfg->portid;

#ifdef TMU_HAL_TEST
		dp_subif->port_id = 15;
		dp_subif->subif = scheduler_id;

#endif

		dp_subif->subif	= dp_subif->subif >> 8;
		if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
			kfree(tmu_res);
			kfree(dp_subif);
			return TMU_HAL_STATUS_ERR;
		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
				__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);

		if(flags & PPA_QOS_Q_F_INGRESS) {
			if((flags & TMU_HAL_QUEUE_SHAPER_INGRESS_MPE) == TMU_HAL_QUEUE_SHAPER_INGRESS_MPE) { /*for MPE Ingress Queue */
				if(( tmuport = (tmu_hal_get_mpe_ingress_index(flags))) == TMU_HAL_STATUS_ERR) {
					kfree(dp_subif);
					ret = TMU_HAL_STATUS_ERR;
					goto CBM_RESOURCES_CLEANUP;
				}
			} else {/*for CPU Ingress queue*/
				if(( tmuport = (tmu_hal_get_ingress_index(flags))) == TMU_HAL_STATUS_ERR) {
					kfree(dp_subif);
					ret = TMU_HAL_STATUS_ERR;
					goto CBM_RESOURCES_CLEANUP;
				}

			}
			subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
			port_subif_index = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		} else {
			subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
			port_subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		}

		for(i=0; i < nof_of_tmu_ports; i++) {
			if((flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) {
				if( (index = tmu_hal_get_user_index_from_qid(subif_index, cfg->queueid) ) == TMU_HAL_STATUS_ERR) {
					fail_cnt++;
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Qid Matched QId=%d\n",__FUNCTION__, cfg->queueid);
					break;
				}
			} else {
				if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) {
					struct tmu_hal_dp_res_info res = {0};
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Deleting the shaper for Directpath interface!!!\n",__FUNCTION__);
					tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);
	
					subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) +
									dp_subif->subif;
					port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);

				} else {
					subif_index = tmu_hal_user_sub_interface_info + ((tmu_res+i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
					port_subif_index = tmu_hal_user_sub_interface_info + ((tmu_res+i)->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
				}	
				if( (index = tmu_hal_get_user_index_from_qid(subif_index, cfg->queueid) ) == TMU_HAL_STATUS_ERR) {
					fail_cnt++;
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Qid Matched QId=%d\n",__FUNCTION__, cfg->queueid);
					break;
				}
			}
		}	
		if(fail_cnt == nof_of_tmu_ports) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Queue Id %d\n",__FUNCTION__, cfg->queueid);
			kfree(dp_subif);
			ret = TMU_HAL_STATUS_INVALID_QID;
			goto CBM_RESOURCES_CLEANUP;
		}
		/* Find the user queue index for user qid from the user queue array*/
		if( (index = tmu_hal_get_user_index_from_qid(subif_index, cfg->queueid) ) == TMU_HAL_STATUS_ERR) {
			kfree(dp_subif);
			ret = TMU_HAL_STATUS_INVALID_QID;
			goto CBM_RESOURCES_CLEANUP;
		}

		/* TMU Queue Index */
		q_index = subif_index->user_queue[index].queue_index;
	} else {
		q_index = tmu_q_idx;
		kfree(dp_subif);
		ret = TMU_HAL_STATUS_INVALID_QID;
		goto CBM_RESOURCES_CLEANUP;
	}

	kfree(dp_subif);
	if (q_index > TMU_HAL_MAX_EGRESS_QUEUES-1) {
		ret = TMU_HAL_STATUS_INVALID_QID;
		goto CBM_RESOURCES_CLEANUP;
	}

	if(((flags & PPA_QOS_Q_F_INGRESS) == PPA_QOS_Q_F_INGRESS) && ((flags & TMU_HAL_QUEUE_SHAPER_INGRESS_MPE) == TMU_HAL_QUEUE_SHAPER_INGRESS_MPE ))
		shaper_index = tmu_hal_queue_track[q_index].tb_index;
	else
		shaper_index = cfg->shaperid;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Delete Shaper %d of Queue %d\n",__FUNCTION__,shaper_index, q_index);

#ifdef CBM_QUEUE_FLUSH_SUPPORT
	if(cbm_queue_map_get(g_Inst, q_index, &num_entries, &q_map_get, 0) == TMU_HAL_STATUS_ERR) {
		TMU_HAL_DEBUG_ERR(TMU_DEBUG_ERR, "CBM FAILURE: Queue %d could not be mapped properly !!\n", q_index);
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> %d No of entries is %d for queue=%d\n", __FUNCTION__,__LINE__, num_entries, q_index);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Q map pointer =%p\n",__FUNCTION__, q_map_get);
	if(tmu_hal_flush_queue_if_qocc(netdev, dev_name, q_index,-1, -1, flags) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to flush qocc\n", __FUNCTION__, __LINE__);
		ret = TMU_HAL_STATUS_ERR;
		goto CBM_RESOURCES_CLEANUP;
	}	
#endif

	/*Delete the Tocken Bucket*/
	tmu_hal_token_bucket_shaper_delete(shaper_index, tmu_hal_queue_track[q_index].sched_input );

	tmu_sched_blk_in_enable(tmu_hal_queue_track[q_index].sched_input, 1);

	if((flags & TMU_HAL_DEL_SHAPER_CFG) == TMU_HAL_DEL_SHAPER_CFG) {
		/* Delete the shaper instance */
		tmu_hal_del_shaper_index(shaper_index);	
		/* Enable the scheduler block input */
		tmu_sched_blk_in_enable(tmu_hal_queue_track[q_index].sched_input, 1);
	}

#ifdef CBM_QUEUE_FLUSH_SUPPORT
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enable Queue %d for netdev %s\n",__FUNCTION__, q_index, netdev->name);
	if(tmu_hal_enable_queue_after_flush(netdev, dev_name, q_index, -1, q_index,flags) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to enable queue after queue flush\n", __FUNCTION__, __LINE__);
		ret = TMU_HAL_STATUS_ERR;
		goto CBM_RESOURCES_CLEANUP;
	}

	if (num_entries > 0) {
		int32_t j;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Q map pointer =%p no_entries=%d\n", q_map_get, num_entries);
		for (j=0; j<num_entries;j++)
			cbm_queue_map_set(g_Inst, q_index, &q_map_get[j], 0);
		num_entries = 0;
	}
#endif
	tmu_hal_queue_track[q_index].tb_index = 0xFF;

CBM_RESOURCES_CLEANUP:
	kfree(q_map_get);
	q_map_get = NULL;
	kfree(tmu_res);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return ret;
}

typedef struct {

	cbm_queue_map_entry_t *q_map;
	int32_t no_of_entries;

} tmu_hal_queue_qmap;

/** The tmu_hal_add_ingress_port_rate_shaper adds the port rate shaper.
	** 1> Get free Shaper index
	** 2> Add a Shaper Scheduler for that logical port.			 
	** 3> Remap the scheduler which is connected to the logical Port(scheduler) previously.
	** 4> Update the logical Port(scheduler) tracking information.
	** 5> Create the shaper and configure it.
	** 6> Update the scheduler/shaper tracking information.
	** 7> Configure the shaper.
*/
int tmu_hal_add_cpu_ingress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, struct tmu_hal_user_subif_abstract *subif_index_logical, uint32_t flags)
{
	uint8_t leaf=0;
	uint32_t p_sched;
	int32_t sched_old, shaper_index, sched_new;
	struct tmu_hal_token_bucket_shaper_cfg cfg_shaper;
	uint16_t base_sched;

	sched_old = subif_index_logical->base_sched_id_egress;

	if(subif_index_logical->port_shaper_sched == 0xFF) {
		if(cfg->shaperid <= 0) {
			shaper_index = tmu_hal_get_free_shaper();
			if(shaper_index >= TMU_HAL_MAX_SHAPER)
			{
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper index. Shaper not available.!!!! \n",__FUNCTION__);
				shaper_index = TMU_HAL_STATUS_ERR;
				return PPA_FAILURE;
			}
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>New Shaper %d is created \n",__FUNCTION__,shaper_index);
		} else	{
			shaper_index = cfg->shaperid;
		}

		leaf = tmu_hal_sched_track[sched_old].omid & 0x7;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Port Scheduler Id %d connected to leaf %d\n",
				__FUNCTION__, tmu_hal_sched_track[sched_old].omid,leaf);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Port Scheduler level is %d \n",__FUNCTION__, tmu_hal_sched_track[sched_old].level);

		/* Get the base sched id */
		base_sched = tmu_hal_sched_track[sched_old].omid >> 3;

		if ((sched_new = 
			create_new_scheduler(tmu_hal_sched_track[sched_old].omid >> 3, 0, 
						TMU_HAL_POLICY_STRICT, 
						tmu_hal_sched_track[sched_old].level, leaf)) < 0 )  {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR, "ERRoR schedular Not available/ busy status=%d\n", sched_new);
			return TMU_HAL_STATUS_ERR;
		}
		if (sched_new == TMU_HAL_MAX_SCHEDULER)
			return -EINVAL;
 
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>New Port Shaper Scheduler %d\n",__FUNCTION__,sched_new);

		tmu_hal_sched_track[sched_new].policy = TMU_HAL_POLICY_STRICT;

		subif_index_logical->port_shaper_sched = sched_new;
		tmu_hal_sched_track[sched_new].next_sched_id = sched_old;

		/** Remap the already connected scheduler */
		p_sched = sched_new << 3;
		tmu_hal_sched_out_remap(sched_old, tmu_hal_sched_track[sched_old].level+1, p_sched, 1, 0);		

		/* Add the token to the scheduler input*/
		tmu_hal_token_bucket_shaper_create(shaper_index, p_sched);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Shaper %d is created for scheduler input %d \n",shaper_index,sched_new);


		tmu_hal_shaper_track[shaper_index].is_enabled = true;
		tmu_hal_shaper_track[shaper_index].sb_input = p_sched;

		subif_index_logical->shaper_idx = shaper_index;
		tmu_hal_sched_track[sched_new].tbs = shaper_index;

		/* Update SBIN weights of base scheduler for ingress */
		tmu_hal_update_ingress_sched_weight(base_sched);

	} else {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Shaper %d is already created \n",__FUNCTION__, subif_index_logical->shaper_idx);
		shaper_index = subif_index_logical->shaper_idx;
	}
	tmu_hal_shaper_track[shaper_index].tb_cfg.enable = true;
	tmu_hal_shaper_track[shaper_index].tb_cfg.index = cfg->shaperid;
	cfg_shaper.index = shaper_index;
	cfg_shaper.enable = true;
	cfg_shaper.mode = tmu_hal_shaper_track[shaper_index].tb_cfg.mode;
	cfg_shaper.cir = (tmu_hal_shaper_track[shaper_index].tb_cfg.cir * 1000) / 8;
	cfg_shaper.pir = (tmu_hal_shaper_track[shaper_index].tb_cfg.pir * 1000 ) / 8;
	cfg_shaper.cbs = tmu_hal_shaper_track[shaper_index].tb_cfg.cbs;
	cfg_shaper.pbs = tmu_hal_shaper_track[shaper_index].tb_cfg.pbs;

	tmu_hal_token_bucket_shaper_cfg_set(&cfg_shaper);

	return TMU_HAL_STATUS_OK;
}

int tmu_hal_add_ingress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	uint32_t tmuport;
	int32_t cpu_shaper_index;
	struct tmu_hal_user_subif_abstract *subif_index_logical = NULL;

	if(( tmuport = (tmu_hal_get_ingress_index(flags))) == TMU_HAL_STATUS_ERR)
		return PPA_FAILURE;

	subif_index_logical = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	tmu_hal_add_cpu_ingress_port_rate_shaper(netdev, cfg, subif_index_logical, flags);
	cpu_shaper_index =	cfg->shaperid;

	if(g_MPE_PortId != -1 && (mpe_hal_set_checksum_queue_map_hook_fn != NULL))
	{
		int32_t shaper_index;
		if(( tmuport = (tmu_hal_get_mpe_ingress_index(flags))) == TMU_HAL_STATUS_ERR)
			return PPA_FAILURE;
		subif_index_logical = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);

		shaper_index = tmu_hal_get_free_shaper();
		if(shaper_index >= TMU_HAL_MAX_SHAPER)
		{
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper index. Shaper not available.!!!! \n",__FUNCTION__);
			shaper_index = TMU_HAL_STATUS_ERR;
			return PPA_FAILURE;
		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>New Shaper %d is created \n",__FUNCTION__,shaper_index);
		tmu_hal_shaper_track[shaper_index].is_enabled = true;
			tmu_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
			tmu_hal_shaper_track[shaper_index].tb_cfg.enable = false;
			tmu_hal_shaper_track[shaper_index].tb_cfg.mode = tmu_hal_shaper_track[cpu_shaper_index].tb_cfg.mode;
			tmu_hal_shaper_track[shaper_index].tb_cfg.pir = tmu_hal_shaper_track[cpu_shaper_index].tb_cfg.pir;
			tmu_hal_shaper_track[shaper_index].tb_cfg.cir = tmu_hal_shaper_track[cpu_shaper_index].tb_cfg.cir;
			tmu_hal_shaper_track[shaper_index].tb_cfg.pbs = tmu_hal_shaper_track[cpu_shaper_index].tb_cfg.pbs;
			tmu_hal_shaper_track[shaper_index].tb_cfg.cbs = tmu_hal_shaper_track[cpu_shaper_index].tb_cfg.cbs;

		cfg->shaperid = shaper_index;
		tmu_hal_add_cpu_ingress_port_rate_shaper(netdev, cfg, subif_index_logical, flags);
	}

	return TMU_HAL_STATUS_OK;
}


/** The tmu_hal_add_egress_port_rate_shaper adds the port rate shaper.
	** 1> Get free Shaper index
	** 2> Add a Shaper Scheduler for that port.			 
	** 3> Remap the scheduler which is connected to the Port previously.
	** 4> Update the Port tracking information.
	** 5> Create the shaper and configure it.
	** 6> Update the scheduler/shaper tracking information.
	** 7> Configure the shaper.
*/
int tmu_hal_add_egress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	int32_t shaper_index, port=0, count=0;
	struct tmu_hal_token_bucket_shaper_cfg cfg_shaper;
	uint32_t i=0, j=0,k=0, num_q=0;
	uint32_t nof_of_tmu_ports;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	PPA_VCC *vcc = NULL;
	struct tmu_hal_user_subif_abstract *subif_index_logical = NULL;
	int32_t sched_new, sched_old, prev_shaper_id = 0;
	uint32_t p_sched;		
	char *dev_name;

#ifdef CBM_QUEUE_FLUSH_SUPPORT
	tmu_hal_queue_qmap qmap[16];
#endif

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter - Flags %d --> \n",__FUNCTION__, flags);

	TMU_ALLOC(dp_subif_t, dp_subif);
	dev_name = cfg->dev_name;
	if(netdev != NULL)
	ppa_br2684_get_vcc(netdev,&vcc);

	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
		if(dp_get_port_subitf_via_ifname(cfg->dev_name, dp_subif) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
			if(tmu_hal_get_subitf_via_ifname(cfg->dev_name,dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
				kfree(dp_subif);
				return TMU_HAL_STATUS_ERR;
			}
		}
	}

#ifdef TMU_HAL_TEST
	dp_subif->port_id = 15;
	dp_subif->subif = scheduler_id;

#endif

	dp_subif->subif	= dp_subif->subif >> 8;
	if(cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
			__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);

	if(high_prio_q_limit < 17){
		if(nof_of_tmu_ports > 1)
			count = 1;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Nof Of TMU Ports %d Count %d\n",__FUNCTION__,nof_of_tmu_ports,count);

	if((dp_subif->subif == 0) && ((flags & PPA_QOS_Q_F_VAP_QOS) != PPA_QOS_Q_F_VAP_QOS)) { /*for the physical interface */
		for(j=0;j <= count; j++) { /* 2 ports for LAN interface*/
			int32_t temp_shaper_id=0;
			struct tmu_hal_user_subif_abstract *subif_index = NULL;

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Count=%d loop=%d\n", count,j);
			port = (tmu_res+j)->tmu_port;
			subif_index = tmu_hal_user_sub_interface_info + (port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + (dp_subif->subif >> 8);

			if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) { /* for directpath interface */
				struct tmu_hal_dp_res_info res = {0};
				struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Adding the shaper for Directpath interface!!!\n",__FUNCTION__);
				tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

				subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) +
								(dp_subif->subif >> 8);
				port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);
				port = res.dp_egress_res.dp_port;

			}
			if(j==0)
				prev_shaper_id = temp_shaper_id = cfg->shaperid;

			if(tmu_hal_port_track[port].shaper_sched_id == 0xFF){
				sched_old = tmu_hal_port_track[port].input_sched_id;		
				/*Get free Shaper */
				if(temp_shaper_id <= 0) {
					shaper_index = tmu_hal_get_free_shaper();
					if(shaper_index >= TMU_HAL_MAX_SHAPER)
					{
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper index. Shaper not available.!!!! \n",__FUNCTION__);
						shaper_index = TMU_HAL_STATUS_ERR;
						goto CBM_RESOURCES_CLEANUP;
					}
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>New Shaper %d is created \n",__FUNCTION__,shaper_index);
					tmu_hal_shaper_track[shaper_index].is_enabled = true;
					tmu_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
					tmu_hal_shaper_track[shaper_index].tb_cfg.enable = false;
					tmu_hal_shaper_track[shaper_index].tb_cfg.mode = tmu_hal_shaper_track[prev_shaper_id].tb_cfg.mode;
					tmu_hal_shaper_track[shaper_index].tb_cfg.pir = tmu_hal_shaper_track[prev_shaper_id].tb_cfg.pir;
					tmu_hal_shaper_track[shaper_index].tb_cfg.cir = tmu_hal_shaper_track[prev_shaper_id].tb_cfg.cir;
					tmu_hal_shaper_track[shaper_index].tb_cfg.pbs = tmu_hal_shaper_track[prev_shaper_id].tb_cfg.pbs;
					tmu_hal_shaper_track[shaper_index].tb_cfg.cbs = tmu_hal_shaper_track[prev_shaper_id].tb_cfg.cbs;

				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Shaper %d is already created \n",__FUNCTION__,cfg->shaperid);
					shaper_index = cfg->shaperid;
					prev_shaper_id = cfg->shaperid;
				}

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Shaper Scheduler Id of Port %d is %d\n",__FUNCTION__,port, tmu_hal_port_track[port].shaper_sched_id);	

				num_q = subif_index->queue_cnt;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of Queues %d\n",__FUNCTION__, num_q);

				if(num_q == 0)
					sched_old = (tmu_res+j)->tmu_sched;
				if ((sched_old < 0) || (sched_old >= TMU_HAL_MAX_SCHEDULER)) {
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"<%s>Invalid Schedular Id of the port %d\n", __FUNCTION__, sched_old);
					shaper_index = TMU_HAL_STATUS_ERR;
					goto CBM_RESOURCES_CLEANUP;
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Input Scheduler Id of the port %d\n",__FUNCTION__, sched_old);

#ifdef CBM_QUEUE_FLUSH_SUPPORT

				tmu_hal_scheduler_cfg_get(sched_old);
				if(tmu_hal_flush_queue_if_qocc(netdev, dev_name, -1, port, -1, flags) != TMU_HAL_STATUS_OK) {
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to flush qocc\n", __FUNCTION__, __LINE__);
					shaper_index = TMU_HAL_STATUS_ERR;
					goto CBM_RESOURCES_CLEANUP;
				}
				udelay(20000);
#endif
				/** Create the Shaper scheduler */
				if((dp_subif->port_id !=0) && ((tmu_res+j)->tmu_port == g_CPU_PortId) ) { /* for directpath interface */
					uint8_t leaf=0;
					leaf = tmu_hal_sched_track[sched_old].omid & 0x7;
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Root Scheduler Id %d connected to leaf %d\n",__FUNCTION__, tmu_hal_sched_track[sched_old].omid,leaf);
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Port Scheduler level is %d \n",__FUNCTION__, tmu_hal_sched_track[sched_old].level);
					sched_new = create_new_scheduler(tmu_hal_sched_track[sched_old].omid >> 3, 0, TMU_HAL_POLICY_STRICT, tmu_hal_sched_track[sched_old].level, leaf);
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Add Shaper Scheduler for the port %d\n",__FUNCTION__,port);
					sched_new = create_new_scheduler(port, 1, TMU_HAL_POLICY_STRICT, 0, 0);
					if (sched_new < 0) {
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR, "ERROR schedular Not available/ busy status=%d\n", sched_new);
						shaper_index = TMU_HAL_STATUS_ERR;
						goto CBM_RESOURCES_CLEANUP;
					}
					tmu_hal_sched_track[sched_new].sched_out_conn = 0; /*connected to the Port*/
					tmu_hal_sched_track[sched_new].omid = port; /*Port*/
					tmu_hal_sched_track[sched_old].sched_out_conn = 1;
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>New Port Shaper Scheduler %d\n",__FUNCTION__,sched_new);

				tmu_hal_sched_track[sched_new].policy = TMU_HAL_POLICY_STRICT;

				tmu_hal_port_track[port].shaper_sched_id = sched_new;
				tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].next_sched_id = sched_old;
				tmu_hal_port_track[port].input_sched_id = sched_new;

				/** Remap the already connected scheduler */
				p_sched = sched_new << 3;
				if((dp_subif->port_id !=0) && ((tmu_res+j)->tmu_port == g_CPU_PortId) ) /* for directpath interface */
					tmu_hal_sched_out_remap(sched_old, tmu_hal_sched_track[sched_old].level+1, p_sched, 1, 0);		
				else 
					tmu_hal_sched_out_remap(sched_old, 1, p_sched, 1, 0);		

				/*Add the token to the scheduler input */
				tmu_hal_token_bucket_shaper_create(shaper_index, p_sched);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Shaper %d is created for scheduler input %d -->Port %d\n",shaper_index,sched_new,port);

				tmu_sched_blk_out_enable(sched_old, 1);
				tmu_sched_blk_out_enable(sched_new, 1);

				tmu_hal_dump_sb_info(sched_old);

				tmu_hal_shaper_track[shaper_index].is_enabled = true;
				tmu_hal_shaper_track[shaper_index].sb_input = p_sched;

				tmu_hal_port_track[port].shaper_sched_id = sched_new;
				tmu_hal_sched_track[sched_new].tbs = shaper_index;

			} 
			else {
				shaper_index = tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].tbs;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Shaper %d is already there for the port %d\n",__FUNCTION__,shaper_index, port);
			}
			if(temp_shaper_id <= 0) {

				cfg_shaper.index = shaper_index;
				cfg_shaper.cir = (cfg->rate_in_kbps * 1000) / 8 ;
				cfg_shaper.pir = (cfg->rate_in_kbps * 1000) / 8 ;

				cfg_shaper.enable = true;
				tmu_hal_shaper_track[shaper_index].tb_cfg.enable = true;
				tmu_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;

				cfg_shaper.mode = tmu_hal_shaper_track[shaper_index].tb_cfg.mode;
				cfg_shaper.cir = (tmu_hal_shaper_track[shaper_index].tb_cfg.cir * 1000) / 8;
				cfg_shaper.pir = (tmu_hal_shaper_track[shaper_index].tb_cfg.pir * 1000 ) / 8;
				cfg_shaper.cbs = tmu_hal_shaper_track[shaper_index].tb_cfg.cbs;
				cfg_shaper.pbs = tmu_hal_shaper_track[shaper_index].tb_cfg.pbs;
			} else {
				tmu_hal_shaper_track[shaper_index].tb_cfg.enable = true;
				tmu_hal_shaper_track[shaper_index].tb_cfg.index = cfg->shaperid;
				cfg_shaper.index = shaper_index;
				cfg_shaper.enable = true;
				cfg_shaper.mode = tmu_hal_shaper_track[shaper_index].tb_cfg.mode;
				cfg_shaper.cir = (tmu_hal_shaper_track[shaper_index].tb_cfg.cir * 1000) / 8;
				cfg_shaper.pir = (tmu_hal_shaper_track[shaper_index].tb_cfg.pir * 1000 ) / 8;
				cfg_shaper.cbs = tmu_hal_shaper_track[shaper_index].tb_cfg.cbs;
				cfg_shaper.pbs = tmu_hal_shaper_track[shaper_index].tb_cfg.pbs;
			}
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Set the configuration for TBID %d\n",__FUNCTION__, cfg_shaper.index);
			tmu_hal_token_bucket_shaper_cfg_set(&cfg_shaper);
#ifdef CBM_QUEUE_FLUSH_SUPPORT
				if(tmu_hal_enable_queue_after_flush(netdev, dev_name, -1, port, -1, flags) != TMU_HAL_STATUS_OK) {
					TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to enable queue after queue flush\n", __FUNCTION__, __LINE__);
					shaper_index = TMU_HAL_STATUS_ERR;
					goto CBM_RESOURCES_CLEANUP;
					}
				udelay(20000);

#endif


		} 
	} else { /* add port rate shaper for logical interface VLAN/VAP's*/
		uint8_t leaf=0;
		/* Add shaper only on subifid port : 0 (irrespective of nb TMU ports) */
		subif_index_logical = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) { /* for directpath interface */
			struct tmu_hal_dp_res_info res = {0};
			struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Adding the shaper for Directpath interface!!!\n",__FUNCTION__);
			tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

			subif_index_logical = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) +
									(dp_subif->subif);
			port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index logical is %p\n",__FUNCTION__,subif_index_logical);
			port = res.dp_egress_res.dp_port;

		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> netdev:%s port_sched_in_egress : %d base_sched_id_egress: %d\n",
				__FUNCTION__, netdev->name, subif_index_logical->port_sched_in_egress, subif_index_logical->base_sched_id_egress);	
		sched_old = subif_index_logical->base_sched_id_egress;

		if((subif_index_logical->port_shaper_sched == 0xFF) || (flags & PPA_QOS_Q_F_VAP_QOS))
		{
			if(cfg->shaperid <= 0) {
				shaper_index = tmu_hal_get_free_shaper();
				if(shaper_index >= TMU_HAL_MAX_SHAPER)
				{
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper index. Shaper not available.!!!! \n",__FUNCTION__);
					shaper_index = TMU_HAL_STATUS_ERR;
					goto CBM_RESOURCES_CLEANUP;
				}
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>New Shaper %d is created \n",__FUNCTION__,shaper_index);
			} else	{
				shaper_index = cfg->shaperid;
			}
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Shaper Scheduler Id of logical interface %d is %d\n",
					__FUNCTION__, dp_subif->subif, subif_index_logical->port_shaper_sched);
			leaf = tmu_hal_sched_track[sched_old].omid & 0x7;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Port Scheduler Id %d connected to leaf %d\n",__FUNCTION__, tmu_hal_sched_track[sched_old].omid,leaf);

#ifdef CBM_QUEUE_FLUSH_SUPPORT
			num_q = subif_index_logical->queue_cnt;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of Queues %d\n",__FUNCTION__, num_q);
			
				tmu_hal_scheduler_cfg_get(sched_old);
				for(i=0; i < num_q; i++ )
				{
					cbm_queue_map_entry_t *q_map_get = NULL;
					int32_t num_entries;
					if ( cbm_queue_map_get(
						g_Inst,
						subif_index_logical->user_queue[i].queue_index,
						&num_entries, &q_map_get, 0) == -1 ) {
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> cbm_queue_map_get failed \n",__FUNCTION__);
						qmap[i].q_map = NULL;
						qmap[i].no_of_entries = 0;
					} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Queue %d has mapped entries %d \n",
							__FUNCTION__,subif_index_logical->user_queue[i].queue_index,num_entries);
					qmap[i].q_map = q_map_get;
					qmap[i].no_of_entries = num_entries;
					}
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disable Queue %d\n",__FUNCTION__, subif_index_logical->user_queue[i].queue_index);
					if(tmu_hal_flush_queue_if_qocc(netdev,dev_name, 
								subif_index_logical->user_queue[i].queue_index, 
								-1,-1, flags) == PPA_FAILURE)
					{
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Flush Queue\n", __FUNCTION__, __LINE__);
						shaper_index = TMU_HAL_STATUS_ERR;
						goto CBM_RESOURCES_CLEANUP;
					}
				}
#endif



			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Port Scheduler level is %d \n",__FUNCTION__, tmu_hal_sched_track[sched_old].level);

			sched_new = tmu_hal_sched_track[sched_old].omid >> 3;
			subif_index_logical->port_shaper_sched = sched_new;

			/** Remap the already connected scheduler */
			p_sched = tmu_hal_sched_track[sched_old].omid;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Shaper Scheduler %d and p sched %d\n",__FUNCTION__, sched_new, p_sched);

#ifdef CBM_QUEUE_FLUSH_SUPPORT
				for(i=0; i < num_q; i++ )
				{
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enable Queue %d\n",__FUNCTION__, subif_index_logical->user_queue[i].queue_index);
					tmu_hal_enable_disable_queue( subif_index_logical->user_queue[i].queue_index , 1);
					if(qmap[i].no_of_entries != 0)
					{
						TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of entries to be mapped %d\n",
								__FUNCTION__, qmap[i].no_of_entries);
						for (k=0; k<qmap[i].no_of_entries;k++) {
							cbm_queue_map_set(g_Inst, subif_index_logical->user_queue[i].queue_index, &qmap[i].q_map[k], 0);
						}
						kfree(qmap[i].q_map);
					}
				}
#endif


			/*Add the token to the scheduler input */
			tmu_hal_token_bucket_shaper_create(shaper_index, p_sched);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Shaper %d is created for scheduler input %d -->Port %d\n",shaper_index,sched_new,port);


			tmu_hal_shaper_track[shaper_index].is_enabled = true;
			tmu_hal_shaper_track[shaper_index].sb_input = p_sched;

			subif_index_logical->shaper_idx = shaper_index;
			tmu_hal_sched_track[sched_new].tbs = shaper_index;

			} else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Shaper %d is already created \n",__FUNCTION__, subif_index_logical->shaper_idx);
				shaper_index = subif_index_logical->shaper_idx;
			}
			tmu_hal_shaper_track[shaper_index].tb_cfg.enable = true;
			tmu_hal_shaper_track[shaper_index].tb_cfg.index = cfg->shaperid;
			cfg_shaper.index = shaper_index;
			cfg_shaper.enable = true;
			cfg_shaper.mode = tmu_hal_shaper_track[shaper_index].tb_cfg.mode;
			cfg_shaper.cir = (tmu_hal_shaper_track[shaper_index].tb_cfg.cir * 1000) / 8;
			cfg_shaper.pir = (tmu_hal_shaper_track[shaper_index].tb_cfg.pir * 1000 ) / 8;
			cfg_shaper.cbs = tmu_hal_shaper_track[shaper_index].tb_cfg.cbs;
			cfg_shaper.pbs = tmu_hal_shaper_track[shaper_index].tb_cfg.pbs;

			tmu_hal_token_bucket_shaper_cfg_set(&cfg_shaper);
		}

CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
	kfree(dp_subif);
	return shaper_index;
}

int tmu_hal_add_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);	

	if((flags & PPA_QOS_OP_F_INGRESS) == PPA_QOS_OP_F_INGRESS) {
		return tmu_hal_add_ingress_port_rate_shaper(
						netdev,
						cfg, 
				flags);
	} else { /* Handling for Egress interface */
		return tmu_hal_add_egress_port_rate_shaper(
						netdev,
						cfg, 
				flags);
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return TMU_HAL_STATUS_OK;

}


int tmu_hal_modify_cpu_ingress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, struct tmu_hal_user_subif_abstract *subif_index_logical)
{
	int32_t shaper_index;
	int32_t ret = TMU_HAL_STATUS_OK;
	struct tmu_hal_token_bucket_shaper_cfg cfg_shaper;

	if(subif_index_logical->port_shaper_sched == INVALID_SCHED_ID)
	{
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper Scheduler Id: %d \n",
				__FUNCTION__, subif_index_logical->port_shaper_sched);
		ret = TMU_HAL_STATUS_ERR;
		return PPA_FAILURE;
	} else	{
		if(subif_index_logical->shaper_idx == INVALID_SHAPER_ID)
		{
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper Id \n",__FUNCTION__);
			ret = TMU_HAL_STATUS_ERR;
			return PPA_FAILURE;
		} else
			shaper_index = subif_index_logical->shaper_idx;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify Shaper Index %d\n", __FUNCTION__, subif_index_logical->shaper_idx);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify Port Shaper Scheduler %d\n", __FUNCTION__, subif_index_logical->port_shaper_sched);

	tmu_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
	tmu_hal_shaper_track[shaper_index].tb_cfg.enable = true;
	//tmu_hal_shaper_track[shaper_index].tb_cfg.mode = cfg->shaper.mode;
	tmu_hal_shaper_track[shaper_index].tb_cfg.pir = cfg->rate_in_kbps;
	tmu_hal_shaper_track[shaper_index].tb_cfg.cir = cfg->rate_in_kbps;
	tmu_hal_shaper_track[shaper_index].tb_cfg.pbs = cfg->burst;
	tmu_hal_shaper_track[shaper_index].tb_cfg.cbs = cfg->burst;

	cfg_shaper.index = shaper_index;
	cfg_shaper.enable = true;
	cfg_shaper.mode = tmu_hal_shaper_track[shaper_index].tb_cfg.mode;
	cfg_shaper.cir = (tmu_hal_shaper_track[shaper_index].tb_cfg.cir * 1000) / 8;
	cfg_shaper.pir = (tmu_hal_shaper_track[shaper_index].tb_cfg.pir * 1000 ) / 8;
	cfg_shaper.cbs = tmu_hal_shaper_track[shaper_index].tb_cfg.cbs; //4000;
	cfg_shaper.pbs = tmu_hal_shaper_track[shaper_index].tb_cfg.pbs; //4000;

	tmu_hal_token_bucket_shaper_cfg_set(&cfg_shaper);
	return PPA_SUCCESS;
}

int tmu_hal_modify_ingress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg)
{
	uint32_t tmuport;
	struct tmu_hal_user_subif_abstract *subif_index_logical = NULL;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);
	if(( tmuport = (tmu_hal_get_ingress_index(cfg->flag))) == TMU_HAL_STATUS_ERR)
		return PPA_FAILURE;

	subif_index_logical = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	tmu_hal_modify_cpu_ingress_port_rate_shaper(netdev, cfg, subif_index_logical);

	if(g_MPE_PortId != -1 && (mpe_hal_set_checksum_queue_map_hook_fn != NULL))
	{
		if(( tmuport = (tmu_hal_get_mpe_ingress_index(cfg->flag))) == TMU_HAL_STATUS_ERR)
			return PPA_FAILURE;
		subif_index_logical = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		tmu_hal_modify_cpu_ingress_port_rate_shaper(netdev, cfg, subif_index_logical);
	}

	return TMU_HAL_STATUS_OK;
}


int tmu_hal_modify_egress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg)
{
	int32_t shaper_index = 0, port = 0, count = 0;
	struct tmu_hal_token_bucket_shaper_cfg cfg_shaper;
	uint32_t j = 0, num_q = 0, ret = 0;
	uint32_t nof_of_tmu_ports;
	cbm_tmu_res_t *tmu_res;
	dp_subif_t *dp_subif = NULL;
	PPA_VCC *vcc = NULL;
	struct tmu_hal_user_subif_abstract *subif_index_logical = NULL;
	char *dev_name;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);

	TMU_ALLOC(dp_subif_t, dp_subif);
	dev_name = cfg->dev_name;
	if(netdev != NULL)
	ppa_br2684_get_vcc(netdev,&vcc);

	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, cfg->flag) != PPA_SUCCESS) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
		if(dp_get_port_subitf_via_ifname(cfg->dev_name, dp_subif) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
			if(tmu_hal_get_subitf_via_ifname(cfg->dev_name, dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
				kfree(dp_subif);
				return TMU_HAL_STATUS_ERR;
			}
		}
	}
	dp_subif->subif  = dp_subif->subif >> 8;

	if(cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != 0) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}

	if(high_prio_q_limit < 17){
		if(nof_of_tmu_ports > 1)
			count = 1;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Nof Of TMU Ports %d Count %d\n",__FUNCTION__,nof_of_tmu_ports,count);

	if(dp_subif->subif == 0) { //for the physical interface
		for(j=0;j <= count; j++) { // 2 ports for LAN interface
			struct tmu_hal_user_subif_abstract *subif_index = NULL;

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Count=%d loop=%d\n", count,j);
			port = (tmu_res+j)->tmu_port;
			subif_index = tmu_hal_user_sub_interface_info + (port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + (dp_subif->subif >> 8);

			if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) { // for directpath interface
				struct tmu_hal_dp_res_info res = {0};
				struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Adding the shaper for Directpath interface!!!\n",__FUNCTION__);
				tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

				subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) +
								(dp_subif->subif >> 8);
				port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);
				port = res.dp_egress_res.dp_port;

			}
			if(tmu_hal_port_track[port].shaper_sched_id == INVALID_SCHED_ID)
			{
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Invalid Shaper Scheduler Id: %d for the port %d\n",
						__FUNCTION__,tmu_hal_port_track[port].shaper_sched_id, port);
				ret = TMU_HAL_STATUS_ERR;
				goto CBM_RESOURCES_CLEANUP;
			} else  {
				if(tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].tbs == INVALID_SHAPER_ID)
				{
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper Id \n",__FUNCTION__);
					ret = TMU_HAL_STATUS_ERR;
					goto CBM_RESOURCES_CLEANUP;
				} else
					shaper_index = tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].tbs;
			}

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Modify Shaper %d pir %d cir %d pbs %d\n",shaper_index,cfg->shaper.pir,cfg->shaper.cir,cfg->shaper.pbs);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Modify Scheduler %d shaper\n",tmu_hal_port_track[port].shaper_sched_id);
			tmu_hal_shaper_track[shaper_index].is_enabled = true;
			tmu_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
			tmu_hal_shaper_track[shaper_index].tb_cfg.enable = true;
			//tmu_hal_shaper_track[shaper_index].tb_cfg.mode = cfg->shaper.mode;
			tmu_hal_shaper_track[shaper_index].tb_cfg.pir = cfg->rate_in_kbps;
			tmu_hal_shaper_track[shaper_index].tb_cfg.cir = cfg->rate_in_kbps;
			tmu_hal_shaper_track[shaper_index].tb_cfg.pbs = cfg->burst;
			tmu_hal_shaper_track[shaper_index].tb_cfg.cbs = cfg->burst;

			num_q = subif_index->queue_cnt;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of Queues %d\n",__FUNCTION__, num_q);

			cfg_shaper.index = shaper_index;
			cfg_shaper.enable = true;
			cfg_shaper.mode = tmu_hal_shaper_track[shaper_index].tb_cfg.mode;
			cfg_shaper.cir = (tmu_hal_shaper_track[shaper_index].tb_cfg.cir * 1000) / 8;
			cfg_shaper.pir = (tmu_hal_shaper_track[shaper_index].tb_cfg.pir * 1000 ) / 8;
			cfg_shaper.cbs = tmu_hal_shaper_track[shaper_index].tb_cfg.cbs; //4000;
			cfg_shaper.pbs = tmu_hal_shaper_track[shaper_index].tb_cfg.pbs; //4000;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Modify the configuration for TBID %d\n",__FUNCTION__, cfg_shaper.index);
			tmu_hal_token_bucket_shaper_cfg_set(&cfg_shaper);
		}
	} else { // add port rate shaper for logical interface VLAN/VAP's
		/* Add shaper only on subifid port : 0 (irrespective of nb TMU ports) */
		subif_index_logical = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) { // for directpath interface
			struct tmu_hal_dp_res_info res = {0};
			//struct tmu_hal_user_subif_abstract *subif_index = NULL;
			struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Adding the shaper for Directpath interface!!!\n",__FUNCTION__);
			tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

			subif_index_logical = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) +
									(dp_subif->subif);
			port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index logical is %p\n",__FUNCTION__,subif_index_logical);
			port = res.dp_egress_res.dp_port;

		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> netdev:%s port_sched_in_egress : %d base_sched_id_egress: %d\n",
				__FUNCTION__, netdev->name, subif_index_logical->port_sched_in_egress, subif_index_logical->base_sched_id_egress);

		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> netdev:%s port_sched_in_egress : %d \n",__FUNCTION__, netdev->name, subif_index_logical->port_sched_in_egress);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Port Shaper Scheduler %d\n", __FUNCTION__, subif_index_logical->port_shaper_sched);

		if(subif_index_logical->port_shaper_sched == INVALID_SCHED_ID)
		{
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper Scheduler Id: %d for the logical interface %d\n",
					__FUNCTION__, subif_index_logical->port_shaper_sched, dp_subif->subif);
			ret = TMU_HAL_STATUS_ERR;
			goto CBM_RESOURCES_CLEANUP;
		} else  {
			if(subif_index_logical->shaper_idx == INVALID_SHAPER_ID)
			{
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper Id \n",__FUNCTION__);
				ret = TMU_HAL_STATUS_ERR;
				goto CBM_RESOURCES_CLEANUP;
			} else
				shaper_index = subif_index_logical->shaper_idx;
		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify Shaper Index %d\n", __FUNCTION__, subif_index_logical->shaper_idx);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Modify Port Shaper Scheduler %d\n", __FUNCTION__, subif_index_logical->port_shaper_sched);

		num_q = subif_index_logical->queue_cnt;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of Queues %d\n",__FUNCTION__, num_q);

		tmu_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
		tmu_hal_shaper_track[shaper_index].tb_cfg.enable = true;
		//tmu_hal_shaper_track[shaper_index].tb_cfg.mode = cfg->shaper.mode;
		tmu_hal_shaper_track[shaper_index].tb_cfg.pir = cfg->rate_in_kbps;
		tmu_hal_shaper_track[shaper_index].tb_cfg.cir = cfg->rate_in_kbps;
		tmu_hal_shaper_track[shaper_index].tb_cfg.pbs = cfg->burst;
		tmu_hal_shaper_track[shaper_index].tb_cfg.cbs = cfg->burst;

		cfg_shaper.index = shaper_index;
		cfg_shaper.enable = true;
		cfg_shaper.mode = tmu_hal_shaper_track[shaper_index].tb_cfg.mode;
		cfg_shaper.cir = (tmu_hal_shaper_track[shaper_index].tb_cfg.cir * 1000) / 8;
		cfg_shaper.pir = (tmu_hal_shaper_track[shaper_index].tb_cfg.pir * 1000 ) / 8;
		cfg_shaper.cbs = tmu_hal_shaper_track[shaper_index].tb_cfg.cbs; //4000;
		cfg_shaper.pbs = tmu_hal_shaper_track[shaper_index].tb_cfg.pbs; //4000;

		tmu_hal_token_bucket_shaper_cfg_set(&cfg_shaper);
	}

CBM_RESOURCES_CLEANUP:
	kfree(tmu_res);
	kfree(dp_subif);
	return shaper_index;
}

int tmu_hal_modify_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg)
{
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);

	if((cfg->flag & PPA_QOS_OP_F_INGRESS) == PPA_QOS_OP_F_INGRESS) {
		return tmu_hal_modify_ingress_port_rate_shaper(
						netdev,
						cfg);
	} else { // Handling for Egress interface
		return tmu_hal_modify_egress_port_rate_shaper(
						netdev,
						cfg);
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);
	return TMU_HAL_STATUS_OK;

}

int tmu_hal_del_cpu_ingress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, struct tmu_hal_user_subif_abstract *subif_index_logical, uint32_t flags)
{
	struct tmu_sched_blk_in_tbs tbs;
	int32_t shaper_index;
	int32_t ret = TMU_HAL_STATUS_OK;
	uint32_t tmuport;
	uint32_t sb_conn=0, next_sched_to_shaper;
	int32_t omid =0;
	uint32_t subif_lvl =0;
	uint16_t base_sched;

	if(( tmuport = (tmu_hal_get_ingress_index(flags))) == TMU_HAL_STATUS_ERR)
		return PPA_FAILURE;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> netdev:%s port_sched_in_egress : %d \n",
			__FUNCTION__, netdev->name, subif_index_logical->port_sched_in_egress);	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Port Shaper Scheduler %d\n", 
			__FUNCTION__, subif_index_logical->port_shaper_sched);

	if(subif_index_logical->port_shaper_sched == INVALID_SCHED_ID) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper Scheduler Id: %d \n",
				__FUNCTION__, subif_index_logical->port_shaper_sched);
		ret = TMU_HAL_STATUS_ERR;
		return PPA_FAILURE;
	} else	{
		if(subif_index_logical->shaper_idx == INVALID_SHAPER_ID) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper Id \n",__FUNCTION__);
			ret = TMU_HAL_STATUS_ERR;
			return PPA_FAILURE;
		} else 
			shaper_index = subif_index_logical->shaper_idx;
	}

	omid = tmu_hal_sched_track[subif_index_logical->port_shaper_sched].omid;
	/* Get the base scheduler ID */
	base_sched = omid >> 3;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Shaper Index %d\n", __FUNCTION__, subif_index_logical->shaper_idx);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Port Shaper Scheduler %d\n", __FUNCTION__, subif_index_logical->port_shaper_sched);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> OMID for Port Shaper Scheduler %d\n", __FUNCTION__, omid);

	tmu_sched_blk_in_enable(subif_index_logical->port_shaper_sched << 3, 0);

	tbs.tbe	= 0;
	tbs.tbid = shaper_index;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disabling the shaper %d\n",__FUNCTION__, shaper_index);
	tmu_sched_blk_in_shaper_assign_set(subif_index_logical->port_shaper_sched<< 3, &tbs);

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Mapping the shaper to NULL_SCHEDULER_INPUT_ID \n",__FUNCTION__);
	tmu_token_bucket_shaper_link_set(shaper_index, NULL_SCHEDULER_INPUT_ID);

	/* Delete the Tocken Bucket */
	tmu_hal_token_bucket_shaper_delete(shaper_index, subif_index_logical->port_shaper_sched << 3 );
	next_sched_to_shaper = tmu_hal_sched_track[subif_index_logical->port_shaper_sched].next_sched_id;
	subif_lvl = tmu_hal_sched_track[subif_index_logical->port_shaper_sched].level;

	/* Delete the Scheduler */
	tmu_hal_scheduler_delete(subif_index_logical->port_shaper_sched);
	if(next_sched_to_shaper != INVALID_SCHED_ID) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Remap SB %d --> OMID %d\n",
				__FUNCTION__, next_sched_to_shaper, sb_conn);
		tmu_hal_sched_out_remap(next_sched_to_shaper, subif_lvl, omid, 1, 0);
	}

	/* Update the shaper tracking*/
	tmu_hal_shaper_track[shaper_index].is_enabled = false;
	tmu_hal_shaper_track[shaper_index].sb_input = 0xFF;

	/* Update the scheduler tracking */ 
	tmu_hal_sched_track[subif_index_logical->port_shaper_sched].in_use = 0;
	tmu_hal_sched_track[subif_index_logical->port_shaper_sched].tbs = INVALID_SHAPER_ID;
	tmu_hal_sched_track[subif_index_logical->port_shaper_sched].leaf_mask = 0;
	tmu_hal_sched_track[subif_index_logical->port_shaper_sched].policy = TMU_HAL_POLICY_NULL;

	subif_index_logical->shaper_idx = INVALID_SHAPER_ID;
	subif_index_logical->port_shaper_sched = INVALID_SCHED_ID;

	/* Update SBIN weights of base scheduler for ingress */
	tmu_hal_update_ingress_sched_weight(base_sched);

	return shaper_index;
}

int tmu_hal_del_ingress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	uint32_t tmuport;
	struct tmu_hal_user_subif_abstract *subif_index_logical = NULL;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);	
	if(( tmuport = (tmu_hal_get_ingress_index(flags))) == TMU_HAL_STATUS_ERR)
		return PPA_FAILURE;

	subif_index_logical = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
	tmu_hal_del_cpu_ingress_port_rate_shaper(netdev, cfg, subif_index_logical, flags);

	if(g_MPE_PortId != -1 && (mpe_hal_set_checksum_queue_map_hook_fn != NULL)) {
		if(( tmuport = (tmu_hal_get_mpe_ingress_index(flags))) == TMU_HAL_STATUS_ERR)
			return PPA_FAILURE;
		subif_index_logical = tmu_hal_user_sub_interface_ingress_info + (tmuport * TMU_HAL_MAX_SUB_IFID_PER_PORT);
		tmu_hal_del_cpu_ingress_port_rate_shaper(netdev, cfg, subif_index_logical, flags);
	}

	return TMU_HAL_STATUS_OK;
}

/** The tmu_hal_del_egress_port_rate_shaper deletes the port rate shaper.
 ** 1> Find the shaper scheduler id from the shaper index
 ** 2> Before deleting the token bucket make sure that all the packets 
 are dequeued properly.
 ** 3> Remap the scheduler which is connected to the shaper scheduler previously.
 ** 4> Update the port tracking information.
 ** 5> Update the shaper tracking information.
 ** 6> Update the scheduler tracking information.
 */
int tmu_hal_del_egress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	struct tmu_sched_blk_in_tbs tbs;
	int32_t shaper_index, port, count=0, sys_q=0;
	int32_t ret = TMU_HAL_STATUS_OK;
	uint32_t k=0;
	uint32_t nof_of_tmu_ports;
	uint32_t sb_conn=0, next_sched_to_shaper;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	PPA_VCC *vcc = NULL;
	struct tmu_hal_user_subif_abstract *subif_index_logical = NULL;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;
	char *dev_name;

#ifdef CBM_QUEUE_FLUSH_SUPPORT
	tmu_hal_queue_qmap qmap[16];
	uint32_t num_q = 0;
	int32_t i, j;
#endif

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Delete Port rate Shaper \n");
	
	TMU_ALLOC(dp_subif_t, dp_subif);
	dev_name = cfg->dev_name;
	if(netdev != NULL)
	ppa_br2684_get_vcc(netdev,&vcc);

	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __FUNCTION__, __LINE__);
		if(dp_get_port_subitf_via_ifname(cfg->dev_name, dp_subif) != PPA_SUCCESS) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __FUNCTION__, __LINE__);
			if(tmu_hal_get_subitf_via_ifname(cfg->dev_name,dp_subif) != PPA_SUCCESS) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"%s:%d ERROR Failed to Get Subif Id when VCC NULL from tmu_hal_get_subitf_via_ifname \n", __FUNCTION__, __LINE__);
				kfree(dp_subif);
				return TMU_HAL_STATUS_ERR;
			}
		}
	}

#ifdef TMU_HAL_TEST
	dp_subif->port_id = 15;
	dp_subif->subif = scheduler_id;

#endif
	dp_subif->subif	= dp_subif->subif >> 8;
	if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
			__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);

	if(high_prio_q_limit < 17){
		if(nof_of_tmu_ports > 1)
			count = 1;
	}

	if(dp_subif->subif == 0) { /*for the physical interface */
		for(k=0;k <= count; k++) {
			port = (tmu_res+k)->tmu_port;
			sys_q = (tmu_res+k)->tmu_q;

			subif_index = tmu_hal_user_sub_interface_info + (port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + (dp_subif->subif >> 8);
			if((dp_subif->port_id !=0) && ((tmu_res+k)->tmu_port == g_CPU_PortId) ) { /* for directpath interface */
				struct tmu_hal_dp_res_info res = {0};
				struct tmu_hal_user_subif_abstract *subif_index = NULL;
				struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Deleting the shaper for Directpath interface!!!\n",__FUNCTION__);
				tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

				subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) +
								(dp_subif->subif >> 8);
				port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index is %p\n",__FUNCTION__,subif_index);
				port = res.dp_egress_res.dp_port;
				sb_conn = tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].omid;

			}
			if(tmu_hal_port_track[port].shaper_sched_id == INVALID_SCHED_ID)
			{
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Invalid Shaper Scheduler Id: %d for the port %d\n",
						__FUNCTION__,tmu_hal_port_track[port].shaper_sched_id, port);
				ret = TMU_HAL_STATUS_ERR;
				goto CBM_RESOURCES_CLEANUP;
			} else	{
				if(tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].tbs == INVALID_SHAPER_ID)
				{
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper Id \n",__FUNCTION__);
					ret = TMU_HAL_STATUS_ERR;
					goto CBM_RESOURCES_CLEANUP;
				} else 
					shaper_index = tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].tbs;
			}

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Delete Shaper %d\n",tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].tbs);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Delete Scheduler %d\n",tmu_hal_port_track[port].shaper_sched_id);
			/* Make sure that all the packets are dequeued */

#ifdef CBM_QUEUE_FLUSH_SUPPORT
			num_q = subif_index->queue_cnt;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of Queues %d\n",__FUNCTION__, num_q);
			if(tmu_hal_flush_queue_if_qocc(netdev, dev_name, -1, port, -1,flags) != TMU_HAL_STATUS_OK) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to flush qocc\n", __FUNCTION__, __LINE__);
				ret = TMU_HAL_STATUS_ERR;
				goto CBM_RESOURCES_CLEANUP;
			}
			udelay(20000);
#endif
#if 1

			tbs.tbe	= 0;
			tbs.tbid = shaper_index;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Disabling the shaper %d\n",shaper_index);


			/*Delete the Tocken Bucket */
			tmu_hal_token_bucket_shaper_delete(shaper_index, tmu_hal_port_track[port].shaper_sched_id << 3 );
			next_sched_to_shaper = tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].next_sched_id;
			/*Delete the Scheduler*/
			tmu_hal_scheduler_delete(tmu_hal_port_track[port].shaper_sched_id);
#endif
			/** Remap the next scheduler id that is connected to the shaper scheduler */
			if((dp_subif->port_id !=0) && ((tmu_res+k)->tmu_port == g_CPU_PortId) ) { // for directpath interface
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Remap SB %d --> OMID %d\n",__FUNCTION__, next_sched_to_shaper, sb_conn);
				tmu_hal_sched_out_remap(next_sched_to_shaper, 
						tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].level, 
						sb_conn, 1, 0);
			} else {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"Remap SB %d --> OMID %d\n",tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].next_sched_id, port);
				tmu_hal_sched_out_remap(next_sched_to_shaper, 0, port, 0, 0);
			}
			tmu_hal_dump_sb_info(next_sched_to_shaper);
			/* Update the port tracking */
			tmu_hal_port_track[port].input_sched_id = next_sched_to_shaper;

#ifdef CBM_QUEUE_FLUSH_SUPPORT
			if(tmu_hal_enable_queue_after_flush(netdev, dev_name, -1, port, -1, flags) != TMU_HAL_STATUS_OK) {
				TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR to enable queue after queue flush\n", __FUNCTION__, __LINE__);
				ret = TMU_HAL_STATUS_ERR;
				goto CBM_RESOURCES_CLEANUP;
			}
			udelay(20000);
#endif
			/* Update the shaper tracking */
			tmu_hal_shaper_track[shaper_index].is_enabled = false;
			tmu_hal_shaper_track[shaper_index].sb_input = 0xFF;

			/* Update the scheduler tracking */
			tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].in_use = 0;
			tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].tbs = INVALID_SHAPER_ID;
			tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].leaf_mask = 0;
			tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].policy = TMU_HAL_POLICY_NULL;

			tmu_hal_port_track[port].shaper_sched_id = INVALID_SCHED_ID;

		} 
	} else { /* for logical interface VLAN/VAP's */
		int32_t omid =0;
		uint32_t sb_input = 0;
		/* Add shaper only on subifid port : 0 (irrespective of nb TMU ports) */
		subif_index_logical = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		if((dp_subif->port_id !=0) && (tmu_res->tmu_port == g_CPU_PortId) ) { /* for directpath interface */
			struct tmu_hal_dp_res_info res = {0};
			struct tmu_hal_user_subif_abstract *port_subif_index = NULL;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Deleting the shaper for Directpath interface!!!\n",__FUNCTION__);
			tmu_hal_dp_port_resources_get(dp_subif->port_id, &res);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Port Id for Directpath interface is %d\n",__FUNCTION__,res.dp_egress_res.dp_port);

			subif_index_logical = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) +
									(dp_subif->subif);
			port_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT);
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> subif index logical is %p\n",__FUNCTION__,subif_index_logical);
			port = res.dp_egress_res.dp_port;
			sb_conn = tmu_hal_sched_track[tmu_hal_port_track[port].shaper_sched_id].omid;

		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> netdev:%s port_sched_in_egress : %d \n",__FUNCTION__, netdev->name, subif_index_logical->port_sched_in_egress);	
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Port Shaper Scheduler %d\n", __FUNCTION__, subif_index_logical->port_shaper_sched);

		if(subif_index_logical->port_shaper_sched == INVALID_SCHED_ID) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper Scheduler Id: %d for the logical interface %d\n",
					__FUNCTION__, subif_index_logical->port_shaper_sched, dp_subif->subif);
			ret = TMU_HAL_STATUS_ERR;
			goto CBM_RESOURCES_CLEANUP;
		} else	{
			if(subif_index_logical->shaper_idx == INVALID_SHAPER_ID) {
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR,"<%s> Invalid Shaper Id \n",__FUNCTION__);
				ret = TMU_HAL_STATUS_ERR;
				goto CBM_RESOURCES_CLEANUP;
			} else 
				shaper_index = subif_index_logical->shaper_idx;
		}

		omid = tmu_hal_sched_track[subif_index_logical->port_shaper_sched].omid;
		sb_input = tmu_hal_shaper_track[subif_index_logical->shaper_idx].sb_input;
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Shaper Index %d\n", __FUNCTION__, subif_index_logical->shaper_idx);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Delete Port Shaper Scheduler %d\n", __FUNCTION__, subif_index_logical->port_shaper_sched);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> OMID for Port Shaper Scheduler %d\n", __FUNCTION__, omid);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> shaper is connected to sbin  %d\n", __FUNCTION__, sb_input);
#ifdef CBM_QUEUE_FLUSH_SUPPORT
			num_q = subif_index_logical->queue_cnt;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of Queues %d\n",__FUNCTION__, num_q);
			for(i=0; i < num_q; i++ ) {
				cbm_queue_map_entry_t *q_map_get = NULL;
				int32_t num_entries;
				if ( cbm_queue_map_get(
							g_Inst,
							subif_index_logical->user_queue[i].queue_index,
							&num_entries, &q_map_get, 0) == -1 ) {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s> No Queue Map entry available \n", __FUNCTION__);
					qmap[i].q_map = NULL;
					qmap[i].no_of_entries = 0;
				} else {
					TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Queue %d has mapped entries %d \n",
							__FUNCTION__,subif_index_logical->user_queue[i].queue_index,num_entries);
					qmap[i].q_map = q_map_get;
					qmap[i].no_of_entries = num_entries;
				}

				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disable Queue %d\n",__FUNCTION__, subif_index_logical->user_queue[i].queue_index);

				if(tmu_hal_flush_queue_if_qocc(netdev, dev_name, 
							subif_index_logical->user_queue[i].queue_index, 
							-1,-1, flags) == PPA_FAILURE) {
						TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Flush Queue\n", __FUNCTION__, __LINE__);
						ret = TMU_HAL_STATUS_ERR;
						goto CBM_RESOURCES_CLEANUP;
					}
			}
#endif

		if (flags & PPA_QOS_Q_F_VAP_QOS) {
			/*Delete the Tocken Bucket*/
			tmu_hal_token_bucket_shaper_delete(shaper_index, sb_input);
			tmu_sched_blk_in_enable(sb_input, 1);
			tmu_hal_del_shaper_index(shaper_index);
		}
		else {
			tmu_sched_blk_in_enable(sb_input, 0);

			tbs.tbe	= 0;
			tbs.tbid = shaper_index;
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Disabling the shaper %d\n",__FUNCTION__, shaper_index);
			tmu_sched_blk_in_shaper_assign_set(sb_input, &tbs);

			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Mapping the shaper to NULL_SCHEDULER_INPUT_ID \n",__FUNCTION__);
			tmu_token_bucket_shaper_link_set(shaper_index, NULL_SCHEDULER_INPUT_ID);

			/*Delete the Tocken Bucket*/
			tmu_hal_token_bucket_shaper_delete(shaper_index, sb_input);

			/* Delete the Scheduler */
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper Scheduler:[%d] leafmask:[%d]\n",
					__FUNCTION__, subif_index_logical->port_shaper_sched,
					tmu_hal_sched_track[subif_index_logical->port_shaper_sched].leaf_mask);
			if (tmu_hal_sched_track[subif_index_logical->port_shaper_sched].leaf_mask == 0) {
					tmu_hal_scheduler_delete(subif_index_logical->port_shaper_sched);
			}
		}
#ifdef CBM_QUEUE_FLUSH_SUPPORT
		for(i=0; i < num_q; i++ )
		{
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Enable Queue %d\n",__FUNCTION__, subif_index_logical->user_queue[i].queue_index);
			tmu_hal_enable_disable_queue( subif_index_logical->user_queue[i].queue_index , 1);
			if(qmap[i].no_of_entries != 0)
			{
				TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Number of entries to be mapped %d\n",
						__FUNCTION__, qmap[i].no_of_entries);
				for (j=0; j<qmap[i].no_of_entries;j++)
					cbm_queue_map_set(g_Inst, subif_index_logical->user_queue[i].queue_index, &qmap[i].q_map[j], 0);
				kfree(qmap[i].q_map);
			}
		}
#endif
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Shaper Scheduler:[%d] leafmask:[%d]\n",
				__FUNCTION__, subif_index_logical->port_shaper_sched,
				tmu_hal_sched_track[subif_index_logical->port_shaper_sched].leaf_mask);
		/* Update the scheduler tracking */
		if (tmu_hal_sched_track[subif_index_logical->port_shaper_sched].leaf_mask == 0) {
			tmu_hal_sched_track[subif_index_logical->port_shaper_sched].in_use = 0;
			tmu_hal_sched_track[subif_index_logical->port_shaper_sched].tbs = INVALID_SHAPER_ID;
			tmu_hal_sched_track[subif_index_logical->port_shaper_sched].leaf_mask = 0;
			tmu_hal_sched_track[subif_index_logical->port_shaper_sched].policy = TMU_HAL_POLICY_NULL;
		}
		tmu_hal_sched_track[subif_index_logical->port_shaper_sched].tbs = INVALID_SHAPER_ID;
		/* Update the shaper tracking */
		tmu_hal_shaper_track[shaper_index].is_enabled = false;
		tmu_hal_shaper_track[shaper_index].sb_input = 0xFF;

		subif_index_logical->shaper_idx = INVALID_SHAPER_ID;
		subif_index_logical->port_shaper_sched = INVALID_SCHED_ID;

	}

CBM_RESOURCES_CLEANUP:	
	kfree(tmu_res);
	kfree(dp_subif);
	return ret;
}

/** The tmu_hal_del_port_rate_shaper deletes the port rate shaper.
 ** 1> Find the shaper scheduler id from the shaper index
 ** 2> Before deleting the token bucket make sure that all the packets 
 are dequeued properly.
 ** 3> Remap the scheduler which is connected to the shaper scheduler previously.
 ** 4> Update the port tracking information.
 ** 5> Update the shaper tracking information.
 ** 6> Update the scheduler tracking information.
 */
int tmu_hal_del_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Enter --> \n",__FUNCTION__);	

	if((flags & PPA_QOS_OP_F_INGRESS) == PPA_QOS_OP_F_INGRESS) {
		return tmu_hal_del_ingress_port_rate_shaper(
						netdev,
						cfg, 
				flags);
	} else { /* Handling for Egress interface */
		return tmu_hal_del_egress_port_rate_shaper(
						netdev,
						cfg, 
				flags);
	}
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> <-- Exit --> \n",__FUNCTION__);	
	return TMU_HAL_STATUS_OK;
}


int tmu_hal_get_subif_from_netdev(
		struct net_device *netdev,
		uint32_t *port_id, 
		uint32_t *subif_id)
{
	dp_subif_t *dp_subif = NULL;
	PPA_VCC *vcc = NULL;
	uint32_t flags = 0;

	TMU_ALLOC(dp_subif_t, dp_subif);
	ppa_br2684_get_vcc(netdev,&vcc);
	if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != PPA_SUCCESS) {
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}
	*port_id = dp_subif->port_id;
	*subif_id = dp_subif->subif;

	kfree(dp_subif);
	return TMU_HAL_STATUS_OK;
}

int tmu_hal_get_queue_num(struct net_device *netdev, int32_t portid, int32_t *q_num)
{
	int32_t nof_of_tmu_ports;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	PPA_VCC *vcc = NULL;
	struct tmu_hal_user_subif_abstract *subif_index = NULL;

	TMU_ALLOC(dp_subif_t, dp_subif);
	if(netdev != NULL) {
		ppa_br2684_get_vcc(netdev,&vcc);
		if(dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, 0) != PPA_SUCCESS) {
			kfree(dp_subif);
			return TMU_HAL_STATUS_ERR;
		}
	} else {
		dp_subif->port_id = portid;
		dp_subif->subif = 0;
	}

#ifdef TMU_HAL_TEST
	dp_subif->port_id = 15;
	dp_subif->subif = 0;
#endif

	dp_subif->subif	= dp_subif->subif >> 8;
	if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_HIGH,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(dp_subif);
		kfree(tmu_res);
		return TMU_HAL_STATUS_ERR;
	}

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_HIGH,"<%s>For Port: %d subif: %d --> TMU Port: %d SB: %d Q: %d\n",
				__FUNCTION__,dp_subif->port_id,dp_subif->subif,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);
	subif_index = tmu_hal_user_sub_interface_info + (tmu_res->tmu_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_HIGH," Sub interface index %p\n",subif_index);

	*q_num = subif_index->queue_cnt; 
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_HIGH," Number of Queues %d \n",*q_num );
	kfree(tmu_res);

	kfree(dp_subif);
	return TMU_HAL_STATUS_OK;
}

static int32_t tmu_hal_add_meter(PPA_QOS_METER_CFG *meter_cfg) {
	int32_t ret;
	GSW_API_HANDLE gswr;
	GSW_QoS_meterCfg_t param;

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s:%d> ENTRY\n", __FUNCTION__, __LINE__);

	ret = 0;
	gswr = 0;
	param.nMeterId = meter_cfg->phys_meterid;
	param.bEnable = meter_cfg->enable;
	param.eMtrType = meter_cfg->type;
	param.nRate = meter_cfg->cir;
	param.nCbs = meter_cfg->cbs;
	param.nPiRate = meter_cfg->pir;
	param.nEbs = meter_cfg->pbs;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s:%d> nMeterId=%d bEnable=%d type=%d rate=%d cbs=%d pir=%d pbs=%d port_id=%d",
		 __FUNCTION__, __LINE__,param.nMeterId,param.bEnable,param.eMtrType,param.nRate,param.nCbs,param.nPiRate,param.nEbs, meter_cfg->port_id);

	gswr = gsw_api_kopen("/dev/switch_api/1");

	if (gswr == 0) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_ERR, "<%s:%d> Open SWAPI device FAILED !!\n", __FUNCTION__ , __LINE__);
		return PPA_FAILURE;
	}

	ret = gsw_api_kioctl(gswr, GSW_QOS_METER_CFG_SET, &param);
	gsw_api_kclose(gswr);

	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE, "<%s:%d> EXIT with ret:[%d]\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

/** ====================================== QMAP Handling ======== ===============================	*/
int tmu_hal_add_q_map(uint32_t q_index, uint32_t pmac_port, cbm_queue_map_entry_t *p_qmap, uint32_t bit_mask)
{
	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_HIGH,"<%s> Add QMAP for Qindex %d TC:%d \n",__FUNCTION__,q_index, p_qmap->tc);

	cbm_queue_map_set(g_Inst, q_index, p_qmap, bit_mask);

	return TMU_HAL_STATUS_OK;
}

int tmu_hal_del_q_map(uint32_t q_index, uint32_t pmac_port, cbm_queue_map_entry_t *p_qmap, uint32_t bit_mask)
{
	uint32_t flags = 0;

	cbm_queue_map_set(g_Inst, 255, p_qmap, flags);
	return TMU_HAL_STATUS_OK;
}
/** ====================================== Directpath Connectivity ===============================	*/
int tmu_hal_dp_port_resources_get(
		uint32_t pmac_port,
		struct tmu_hal_dp_res_info *res_p)
{
	*res_p = tmu_hal_dp_res[pmac_port];
	return TMU_HAL_STATUS_OK;
} 

int tmu_hal_dp_egress_root_create(struct net_device *netdev)
{
	uint32_t old_omid, root_omid ;
	struct tmu_hal_sched_track_info Snext;
	uint32_t nof_of_tmu_ports;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;

	TMU_ALLOC(dp_subif_t, dp_subif);

#ifdef TMU_HAL_TEST
	dp_subif->port_id = 0;
	dp_subif->subif = 0;
#endif
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Get Resources for Directpath Egress....\n",__FUNCTION__);
	if(tmu_hal_get_tmu_res_from_netdevice(netdev, netdev->name, 0, dp_subif, &nof_of_tmu_ports, &tmu_res, 0) == TMU_HAL_STATUS_ERR){
		kfree(dp_subif);
		return TMU_HAL_STATUS_ERR;
	}

	kfree(dp_subif);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> CPU TMU Port %d and Scheduler %d\n",__FUNCTION__,tmu_res->tmu_port, tmu_hal_port_track[tmu_res->tmu_port].input_sched_id);
	
	/*Create the WFQ Root Scheduler*/
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Create WFQ Scheduler for Directpath Egress\n",__FUNCTION__);

	old_omid = tmu_hal_port_track[g_CPU_PortId].input_sched_id;
	Snext = tmu_hal_sched_track[old_omid];
	root_omid = create_new_scheduler(old_omid, 0, TMU_HAL_POLICY_WFQ, Snext.level+1, 1);
	tmu_hal_sched_track[old_omid].next_sched_id = root_omid;
	tmu_hal_sched_track[root_omid].policy = TMU_HAL_POLICY_WFQ;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>Configuring	Scheduler id of SB %d -->SB %d \n",__FUNCTION__,root_omid, Snext.id);

	g_Root_sched_id_DpEgress = root_omid;
	g_DpEgressQueueScheduler++;
	kfree(tmu_res);
	return TMU_HAL_STATUS_OK;

}

int tmu_hal_setup_dp_egress_connectivity(
			struct net_device *netdev, 
			uint32_t pmac_port)
{
	uint32_t port_sched,q_new, q_map_mask=0;
	uint32_t dp_port, j=0;
	enum tmu_hal_errorcode eErr;
	cbm_queue_map_entry_t q_map;
	struct tmu_hal_user_subif_abstract *ingress_subif_index = NULL;

	if (pmac_port >= TMU_HAL_MAX_DP_RES)
		return -EINVAL;
	
	if(!g_DpEgressQueueScheduler) {
		if(tmu_hal_dp_egress_root_create(netdev) == TMU_HAL_STATUS_ERR) {
			TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Failed to create DP Egress scheduler for netdev %s\n",__FUNCTION__,netdev->name);
			return TMU_HAL_STATUS_ERR;	
		}
	}
	
	dp_port = tmu_hal_get_free_port();
	TMU_HAL_ASSERT(dp_port >= TMU_HAL_MAX_EGRESS_PORT);
	ingress_subif_index = tmu_hal_user_sub_interface_ingress_info + (dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) ;

	/* Now create the Port scheduler for the interface */
	port_sched = create_new_scheduler(g_Root_sched_id_DpEgress, 0, TMU_HAL_POLICY_WFQ, tmu_hal_sched_track[g_Root_sched_id_DpEgress].level + 1, 1);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> DP Port scheduler is %d\n",__FUNCTION__,port_sched);
	tmu_hal_sched_track[port_sched].policy = TMU_HAL_POLICY_STRICT;

	/* Add the Queue to the scheduler */	
	q_new = tmu_hal_add_new_queue(netdev, netdev->name, g_CPU_PortId, 1, 0, 0, port_sched, ingress_subif_index, &eErr, 0 );
	
	/** Update the Egress Queue resource info */
	tmu_hal_dp_res[pmac_port].netdev = netdev;
	tmu_hal_dp_res[pmac_port].dp_egress_res.qidx = q_new;
	tmu_hal_dp_res[pmac_port].dp_egress_res.dp_sched = port_sched;

	/** Configure the QMAP table to connect to the DP egress queue*/
		memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
	
	q_map.ep = pmac_port;
	q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
			CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
			CBM_QUEUE_MAP_F_EN_DONTCARE |
			CBM_QUEUE_MAP_F_DE_DONTCARE |
			CBM_QUEUE_MAP_F_MPE1_DONTCARE |
			CBM_QUEUE_MAP_F_MPE2_DONTCARE |
			CBM_QUEUE_MAP_F_TC_DONTCARE;
	tmu_hal_add_q_map(q_new, CPU_PORT_ID, &q_map, q_map_mask);

	/** This is a dummy TMU port. Only required for ingress sub interface index */
	tmu_hal_dp_res[pmac_port].dp_egress_res.dp_port = dp_port;
	tmu_hal_port_track[dp_port].is_enabled = 1;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> DP Port for pmac port %d is %d\n",__FUNCTION__,pmac_port, tmu_hal_dp_res[pmac_port].dp_egress_res.dp_port);

	tmu_hal_port_track[dp_port].input_sched_id = port_sched;
	tmu_hal_port_track[dp_port].shaper_sched_id = 0xFF;
	tmu_hal_sched_track[port_sched].tbs = 0xFF;

	ingress_subif_index->is_logical = 0 ; 
	ingress_subif_index->tmu_port_idx = g_CPU_PortId; 
	ingress_subif_index->base_sched_id_egress = port_sched; 
	ingress_subif_index->user_queue[0].queue_index = q_new; 
	ingress_subif_index->user_queue[0].queue_type = TMU_HAL_POLICY_STRICT;
	ingress_subif_index->user_queue[0].qid = 0; 				
	ingress_subif_index->user_queue[0].prio_level = 9 ;  
	ingress_subif_index->user_queue[0].sbin = port_sched << 3; 
	ingress_subif_index->user_queue[0].qmap[MAX_TC_NUM-1] = 0; 
	ingress_subif_index->qid_last_assigned = 0; 
	ingress_subif_index->default_q = q_new;
	ingress_subif_index->queue_cnt++; 
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> ingress_subif_index is %p Queue Count %d\n",__FUNCTION__,ingress_subif_index,ingress_subif_index->queue_cnt);
	for(j=1; j< TMU_HAL_MAX_SUB_IFID_PER_PORT; j++) {
		(ingress_subif_index + j)->is_logical = 1; 
		(ingress_subif_index + j)->tmu_port_idx = dp_port; 
		(ingress_subif_index + j)->base_sched_id_egress = 0xFF; 
		(ingress_subif_index + j)->port_sched_in_user_lvl = TMU_HAL_MAX_PRIORITY_LEVEL; 
		(ingress_subif_index + j)->port_sched_in_egress = NULL_SCHEDULER_INPUT_ID; 
		(ingress_subif_index + j)->port_shaper_sched = INVALID_SCHED_ID; 
		(ingress_subif_index + j)->shaper_idx = INVALID_SHAPER_ID; 
		(ingress_subif_index + j)->default_q = 0; 
			
	}


	return TMU_HAL_STATUS_OK;
}

int tmu_hal_setup_dp_ingress_connectivity(
			struct net_device *netdev, 
			uint32_t pmac_port)
{
	uint32_t q_new, q_map_mask=0;
	uint32_t nof_of_tmu_ports;
	enum tmu_hal_errorcode eErr;
	cbm_queue_map_entry_t q_map;
	cbm_tmu_res_t *tmu_res = NULL;
	dp_subif_t *dp_subif = NULL;
	uint32_t port_id = 0;

	if (pmac_port >= TMU_HAL_MAX_DP_RES)
		return TMU_HAL_STATUS_ERR;

	if(g_DpIngressQueueScheduler == 0xFF) {

		TMU_ALLOC(dp_subif_t, dp_subif);
		dp_get_netif_subifid(netdev, NULL, NULL, 0, dp_subif, 0);
		if (cbm_dp_port_resources_get(&dp_subif->port_id, &nof_of_tmu_ports, &tmu_res, DP_F_DIRECTPATH_RX) != TMU_HAL_STATUS_OK) {
			TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
			kfree(tmu_res);
			kfree(dp_subif);
			return TMU_HAL_STATUS_ERR;
		}
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> TMU resources for Directpath Rx is Port:%d ,SB:%d, Q:%d \n",
				__FUNCTION__,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);
		g_DpIngressQueueScheduler = tmu_res->tmu_sched; 
		g_Port_id_DpIngress = tmu_res->tmu_port; 
		g_Queue_id_DpIngress = tmu_res->tmu_q; 
		q_new = tmu_res->tmu_q; 
		tmu_hal_sched_track[g_DpIngressQueueScheduler].policy = TMU_HAL_POLICY_WFQ;
		tmu_hal_sched_track[g_DpIngressQueueScheduler].leaf_mask = 0x01;
		kfree(tmu_res);
		kfree(dp_subif);
	} else {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> Create Ingress Queue for Directpath Rx Port:%d ,SB:%d\n",
				__FUNCTION__,g_Port_id_DpIngress,g_DpIngressQueueScheduler);
		q_new = tmu_hal_add_new_queue(netdev, netdev->name, g_Port_id_DpIngress, 1, 0, 1, g_DpIngressQueueScheduler, NULL, &eErr, 0);
	}
	/** Update the Ingress Queue resource info */
	tmu_hal_dp_res[pmac_port].q_ingress = q_new; 
 
	/** Configure the QMAP table to connect to the ingress queue 
		FLOW ID: 00 DEC:1 ENC:1 MPE1:0 MPE2:0 EP: 7-14 CLASS: XX 
	*/
	memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
	
	q_map.ep = pmac_port;
	q_map.flowid = 0;
	q_map.enc = 1;
	q_map.dec =1;
	q_map.mpe1 =0;
	q_map.mpe2 =0;
	q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE; 
	TMU_HAL_DEBUG_MSG(TMU_DEBUG_TRACE,"Q: %d\n",q_new);
	tmu_hal_add_q_map(q_new, CPU_PORT_ID, &q_map, q_map_mask);

	/** Configure the QMAP table to connect to the checksum queue 
		FLOW ID: 00 DEC:1 ENC:1 MPE1:0 MPE2:1 EP: 7-14 CLASS: XX 
	*/
	port_id = pmac_port;
	if (cbm_dp_port_resources_get(&port_id, &nof_of_tmu_ports, &tmu_res, DP_F_CHECKSUM) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		return TMU_HAL_STATUS_ERR;
	}
	printk( "<%s> TMU resources for Checksum is Port:%d ,SB:%d, Q:%d \n",
			__FUNCTION__,tmu_res->tmu_port,tmu_res->tmu_sched,tmu_res->tmu_q);

	memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
	
	q_map.ep = pmac_port;
	q_map.flowid = 0;
	q_map.enc = 1;
	q_map.dec =1;
	q_map.mpe2 =1;
	q_map_mask |= CBM_QUEUE_MAP_F_MPE1_DONTCARE | 
			CBM_QUEUE_MAP_F_TC_DONTCARE; 

	cbm_queue_map_set(g_Inst, tmu_res->tmu_q, &q_map, q_map_mask);


	/** Added for Loopback port which is required for IPsec */
	if(!strcmp(netdev->name,"loopdev0") ) {	
		printk("For Loopdev device Q:%d\n",q_new);
			memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
		q_map.ep = pmac_port;

			q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
				CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
				CBM_QUEUE_MAP_F_EN_DONTCARE |
				CBM_QUEUE_MAP_F_DE_DONTCARE |
				CBM_QUEUE_MAP_F_MPE1_DONTCARE |
				CBM_QUEUE_MAP_F_MPE2_DONTCARE |
				CBM_QUEUE_MAP_F_TC_DONTCARE;
		tmu_hal_add_q_map(q_new, CPU_PORT_ID, &q_map, q_map_mask);
	} 

	kfree(tmu_res);		
	return TMU_HAL_STATUS_OK;
}
EXPORT_SYMBOL(tmu_hal_setup_dp_ingress_connectivity);

int tmu_hal_set_checksum_queue_map( uint32_t pmac_port )
{
	uint32_t q_map_mask=0;
	uint32_t nof_of_tmu_ports=0;
	cbm_queue_map_entry_t q_map;
	cbm_tmu_res_t *tmu_res = NULL;
	uint32_t port_id = 0;

	if (pmac_port >= TMU_HAL_MAX_DP_RES)
		return TMU_HAL_STATUS_ERR;
	
	/** Configure the QMAP table to connect to the checksum queue 
		FLOW ID: 00 DEC:1 ENC:1 MPE1:0 MPE2:1 EP: 7-14 CLASS: XX 
	*/
	port_id = pmac_port;
	if (cbm_dp_port_resources_get(&port_id, &nof_of_tmu_ports, &tmu_res, DP_F_CHECKSUM) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		return TMU_HAL_STATUS_ERR;
	}

	memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
	
	q_map.ep = pmac_port;
	q_map.flowid = 0;
	q_map.enc = 1;
	q_map.dec =1;
	q_map.mpe2 =1;
	q_map_mask |= CBM_QUEUE_MAP_F_MPE1_DONTCARE | 
			CBM_QUEUE_MAP_F_TC_DONTCARE; 

	cbm_queue_map_set(g_Inst, tmu_res->tmu_q, &q_map, q_map_mask); 

	kfree(tmu_res);		
	return TMU_HAL_STATUS_OK;
}

int tmu_hal_set_lro_queue_map( uint32_t pmac_port )
{
	uint32_t q_map_mask = 0;
	cbm_queue_map_entry_t q_map;

	if (pmac_port >= TMU_HAL_MAX_DP_RES)
		return TMU_HAL_STATUS_ERR;
	
	memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
	
	q_map.ep = pmac_port;
	q_map.flowid = 2;
	q_map_mask |= CBM_QUEUE_MAP_F_MPE1_DONTCARE |
			CBM_QUEUE_MAP_F_MPE2_DONTCARE |
			CBM_QUEUE_MAP_F_EN_DONTCARE |
			CBM_QUEUE_MAP_F_DE_DONTCARE |
			CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
			CBM_QUEUE_MAP_F_TC_DONTCARE;

	cbm_queue_map_set(g_Inst, LRO_QID_DEF, &q_map, q_map_mask);

	return TMU_HAL_STATUS_OK;
}


int tmu_hal_remove_dp_egress_connectivity(struct net_device *netdev, uint32_t pmac_port)
{
	int32_t q_map_mask = 0;
	cbm_queue_map_entry_t q_map;
	struct tmu_hal_dp_res_info res = {0};
	struct tmu_hal_user_subif_abstract *ingress_subif_index = NULL;

	tmu_hal_dp_port_resources_get(pmac_port, &res);

	/* Delete the Queue */
	tmu_hal_safe_queue_delete(netdev, netdev->name, res.dp_egress_res.qidx, -1, -1,0);

	/* Delete the Scheduler */
	tmu_hal_scheduler_delete(res.dp_egress_res.dp_sched);


	/** Configure the QMAP table to connect to the ingress queue*/
		memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
	
	q_map.ep = pmac_port;
	q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
			CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
			CBM_QUEUE_MAP_F_EN_DONTCARE |
			CBM_QUEUE_MAP_F_DE_DONTCARE |
			CBM_QUEUE_MAP_F_MPE1_DONTCARE |
			CBM_QUEUE_MAP_F_MPE2_DONTCARE |
			CBM_QUEUE_MAP_F_TC_DONTCARE;

	/** Unmap the QMAP table for this egress queue*/
	tmu_hal_del_q_map(res.dp_egress_res.qidx, CPU_PORT_ID, &q_map, q_map_mask);
	
	tmu_hal_add_q_map(g_CPU_Queue, CPU_PORT_ID, &q_map, q_map_mask);

	ingress_subif_index = tmu_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * TMU_HAL_MAX_SUB_IFID_PER_PORT) ;
	ingress_subif_index->is_logical = 0 ; 
	ingress_subif_index->tmu_port_idx = 0xFF; 
	ingress_subif_index->base_sched_id_egress = 0xFF; 
	ingress_subif_index->user_queue[0].queue_index = 0xFF; 
	ingress_subif_index->user_queue[0].queue_type = TMU_HAL_POLICY_NULL;
	ingress_subif_index->user_queue[0].qid = 0; 				
	ingress_subif_index->user_queue[0].prio_level = 0; 
	ingress_subif_index->user_queue[0].sbin = NULL_SCHEDULER_INPUT_ID; 
	ingress_subif_index->default_q = 0;
	ingress_subif_index->qid_last_assigned = 0; 
	ingress_subif_index->queue_cnt = 0; 

	tmu_hal_port_track[res.dp_egress_res.dp_port].is_enabled = 0;
	/** Update the Egress Queue resource info */
	tmu_hal_dp_res[pmac_port].dp_egress_res.qidx = 0xFF; 
	tmu_hal_dp_res[pmac_port].dp_egress_res.dp_sched = 0xFF; 
	tmu_hal_dp_res[pmac_port].dp_egress_res.dp_port = 0xFF; 

	return TMU_HAL_STATUS_OK;
}

int tmu_hal_remove_dp_ingress_connectivity(struct net_device *netdev, uint32_t pmac_port)
{
	int32_t	q_map_mask = 0;
	struct tmu_hal_dp_res_info res;
	cbm_queue_map_entry_t q_map;

	tmu_hal_dp_port_resources_get(pmac_port, &res);


	/** Update the Ingress Queue resource info */
	tmu_hal_dp_res[pmac_port].q_ingress = 0xFF; 
	memset(&q_map, 0, sizeof(cbm_queue_map_entry_t));
	
	q_map.ep = pmac_port;
	q_map.flowid = 0;
	q_map.enc = 1;
	q_map.dec =1;
	q_map.mpe1 =0;
	q_map.mpe2 =0;
	q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE; 

	/** Configure the QMAP table to connect to the ingress queue*/
	tmu_hal_add_q_map(res.q_ingress, CPU_PORT_ID, &q_map, q_map_mask);
	if(res.q_ingress != g_Queue_id_DpIngress) {
		tmu_hal_egress_queue_delete(res.q_ingress);
	} else
		g_DpIngressQueueScheduler = 0xFF;

	return TMU_HAL_STATUS_OK;
}
EXPORT_SYMBOL(tmu_hal_remove_dp_ingress_connectivity);

/** =========================	Initialize all the list ================= */

/** This function updates all the list with the CBM reserved resources
*/
int hal_init(void)
{
	uint32_t port, queue, sched;
	int i,j,k;
	uint32_t nof_of_tmu_ports;
	cbm_tmu_res_t *tmu_res = NULL;
	GSW_PMAC_Eg_Cfg_t egCfg;
	GSW_PMAC_Ig_Cfg_t igCfg;
	GSW_CPU_PortCfg_t cpuPortCfg;
	GSW_PMAC_BM_Cfg_t bmCfg;
	GSW_QoS_WRED_QueueCfg_t wredQueueCfg;
	GSW_API_HANDLE gswl;
	GSW_API_HANDLE gswr;
	GSW_QoS_queuePort_t qos_queueport_set; 
	uint32_t port_id = 0;
	cbm_tmu_res_t *tmu_res_checksum = NULL;
	struct tmu_equeue_drop_params thx = {0};

#ifdef TMU_HAL_TEST
	sched = 24;
	port = 24;
	queue = 37;
#endif
	cbm_reserved_dp_resources_get(&port, &sched, &queue);
	if(port == 0) 
		return TMU_HAL_STATUS_ERR;
	
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>CBM RESERVED RESOURCES Port: %d Scheduler: %d	Queue: %d\n",__FUNCTION__,port,sched,queue); 

	g_No_Of_TMU_Res_Port = port;
	g_No_Of_TMU_Res_Scheduler = sched;
	g_No_Of_TMU_Res_Queue = queue;
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"************************dp_reserve=%d*****************\n", dp_reserve);
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"************************high_prio_q_limit=%d*****************\n", high_prio_q_limit);
	if(dp_reserve)
	{
		g_No_Of_TMU_Res_Queue = queue + (TMU_HAL_MAX_DIRECTPATH_PORT * 2);
		g_No_Of_TMU_Res_Scheduler = sched + (TMU_HAL_MAX_DIRECTPATH_PORT + 2);
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s> g_No_Of_TMU_Res_Queue =%d g_No_Of_TMU_Res_Scheduler=%d\n", __FUNCTION__, g_No_Of_TMU_Res_Queue, 
									g_No_Of_TMU_Res_Scheduler);
		g_No_Of_TMU_Res_Port = port + TMU_HAL_MAX_DIRECTPATH_PORT;
	}
	g_No_Of_TMU_Res_Port = port + TMU_HAL_MAX_DIRECTPATH_PORT;
	g_No_Of_Ingress_Interface = port + TMU_HAL_MAX_DIRECTPATH_PORT; 
	
	printk("Total node : %d	and size of each node: %d\n", g_No_Of_TMU_Res_Port * TMU_HAL_MAX_SUB_IFID_PER_PORT , sizeof(struct tmu_hal_user_subif_abstract));
	printk("Total allocated size=%d\n", g_No_Of_TMU_Res_Port * TMU_HAL_MAX_SUB_IFID_PER_PORT * sizeof(struct tmu_hal_user_subif_abstract));
	/* User Abstraction list for Egress Queues */
	tmu_hal_user_sub_interface_info = (struct tmu_hal_user_subif_abstract *) 
	alloc_pages_exact(((g_No_Of_TMU_Res_Port+1) * TMU_HAL_MAX_SUB_IFID_PER_PORT * sizeof(struct tmu_hal_user_subif_abstract)) , GFP_KERNEL);
	if (!tmu_hal_user_sub_interface_info) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"Failed to allocate memory for Egress Queue Management Table\n");
		return TMU_HAL_STATUS_ERR;
	}
	/* printk("tmu_hal_user_sub_interface_info: %x\n", tmu_hal_user_sub_interface_info); */
	memset(tmu_hal_user_sub_interface_info, 0, (g_No_Of_TMU_Res_Port+1) * TMU_HAL_MAX_SUB_IFID_PER_PORT * sizeof(struct tmu_hal_user_subif_abstract));

	/* User Abstraction list for Ingress Queues */
	tmu_hal_user_sub_interface_ingress_info = (struct tmu_hal_user_subif_abstract *) 
	alloc_pages_exact((g_No_Of_Ingress_Interface+1) * TMU_HAL_MAX_SUB_IFID_PER_PORT * sizeof( struct tmu_hal_user_subif_abstract) , GFP_KERNEL);
	if (!tmu_hal_user_sub_interface_ingress_info) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"Failed to allocate memory for Ingress Queue Management Table\n");
		return TMU_HAL_STATUS_ERR;
	}
	/* printk("tmu_hal_user_sub_interface_ingress_info: %p\n", tmu_hal_user_sub_interface_ingress_info);*/
	printk("Total node : %d	and size of each node: %d\n", g_No_Of_Ingress_Interface * TMU_HAL_MAX_SUB_IFID_PER_PORT , sizeof(struct tmu_hal_user_subif_abstract));
	memset(tmu_hal_user_sub_interface_ingress_info, 0, (g_No_Of_Ingress_Interface+1) * TMU_HAL_MAX_SUB_IFID_PER_PORT * sizeof(struct tmu_hal_user_subif_abstract));


	for (i=0; i<= port+1; i++) {
		tmu_hal_port_track[i].is_enabled = 1;
		tmu_hal_port_track[i].input_sched_id = 0xFF;
		tmu_hal_port_track[i].default_sched_id = 0xFF;
		tmu_hal_port_track[i].shaper_sched_id = 0xFF;
		tmu_hal_port_track[i].port_sched_id = 0xFF;

		(tmu_hal_user_sub_interface_info + i* TMU_HAL_MAX_SUB_IFID_PER_PORT)->base_sched_id_egress = i;
		for(j=1; j< TMU_HAL_MAX_SUB_IFID_PER_PORT-1; j++) {
			(tmu_hal_user_sub_interface_info + i* TMU_HAL_MAX_SUB_IFID_PER_PORT + j)->base_sched_id_egress = 0xFF;
			for(k=0; k< TMU_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE; k++) {
				(tmu_hal_user_sub_interface_info + i* TMU_HAL_MAX_SUB_IFID_PER_PORT + j)->user_queue[k].queue_index = 0xFF;
				(tmu_hal_user_sub_interface_info + i* TMU_HAL_MAX_SUB_IFID_PER_PORT + j)->user_queue[k].prio_level = 0xFF;
			}
			
		}
		for(j=0; j< g_No_Of_Ingress_Interface * TMU_HAL_MAX_SUB_IFID_PER_PORT; j++) {
			(tmu_hal_user_sub_interface_ingress_info + j)->base_sched_id_egress = 0xFF;
			(tmu_hal_user_sub_interface_ingress_info + j)->port_sched_in_egress = NULL_SCHEDULER_INPUT_ID ; 
			(tmu_hal_user_sub_interface_ingress_info + j)->port_shaper_sched = 0xFF ; 

		}
	}
	for (i=0; i<= g_No_Of_TMU_Res_Scheduler; i++) {
		tmu_hal_sched_track[i].in_use = 1;
		tmu_hal_sched_track[i].id = i;
		tmu_hal_sched_track[i].level = 0;
		tmu_hal_sched_track[i].policy = TMU_HAL_POLICY_STRICT;
		tmu_hal_sched_track[i].next_sched_id = 0xFF;
		tmu_hal_sched_track[i].peer_sched_id = 0xFF;
		tmu_hal_sched_track[i].sched_out_conn = 0; /* default connected to the port */
		tmu_hal_sched_track[i].omid = i;
		tmu_hal_sched_track[i].tbs = 0xFF;
		tmu_hal_sched_track[i].priority_selector = 0;
	}
	for (i=0; i<= g_No_Of_TMU_Res_Queue; i++) {
		tmu_hal_queue_track[i].is_enabled = 1;
		tmu_hal_queue_track[i].connected_to_sched_id = sched;
		tmu_hal_queue_track[i].sched_input = sched;
		tmu_hal_queue_track[i].q_type = TMU_HAL_POLICY_WFQ;	
		tmu_hal_queue_track[i].prio_weight = 0;	
		tmu_hal_queue_track[i].tb_index = 0xFF;	
	}

	/** Get the TMU resources for CPU */
	if (cbm_dp_port_resources_get(&port_id, &nof_of_tmu_ports, &tmu_res, 0) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
		return TMU_HAL_STATUS_ERR;
	}

	g_CPU_PortId = tmu_res->tmu_port;
	g_CPU_Sched = tmu_res->tmu_sched;
	g_CPU_Queue = tmu_res->tmu_q;
		
	TMU_HAL_DEBUG_MSG( TMU_ENABLE_ALL_DEBUG,"<%s> Number of CPU TMU Ports are:%d \n",__FUNCTION__, nof_of_tmu_ports);
	TMU_HAL_DEBUG_MSG( TMU_ENABLE_ALL_DEBUG,"<%s> CPU TMU resources are Port:%d ,SB:%d, Q:%d \n",__FUNCTION__,g_CPU_PortId, g_CPU_Sched, g_CPU_Queue);
	kfree(tmu_res);

	/** Get the TMU resources for MPE */
	nof_of_tmu_ports =0;
	tmu_res = NULL;
	if (cbm_dp_port_resources_get(&port_id, &nof_of_tmu_ports, &tmu_res, DP_F_MPE_ACCEL) != TMU_HAL_STATUS_OK) {
		TMU_HAL_DEBUG_MSG(TMU_DEBUG_ERR,"%s:%d ERROR Failed to Get TMU Resources\n", __FUNCTION__, __LINE__);
		kfree(tmu_res);
 		return TMU_HAL_STATUS_ERR;
	}

	g_MPE_PortId = tmu_res->tmu_port;
	g_MPE_Sched = tmu_res->tmu_sched;
	g_MPE_Queue = tmu_res->tmu_q;
		
	TMU_HAL_DEBUG_MSG( TMU_ENABLE_ALL_DEBUG,"<%s> Number of MPE TMU Ports are:%d \n",__FUNCTION__, nof_of_tmu_ports);
	TMU_HAL_DEBUG_MSG( TMU_ENABLE_ALL_DEBUG,"<%s> MPE TMU resources are Port:%d ,SB:%d, Q:%d \n",__FUNCTION__,g_MPE_PortId, g_MPE_Sched, g_MPE_Queue);
	kfree(tmu_res);

	/* Do the GSWIP-L configuration */
	gswl = gsw_api_kopen("/dev/switch_api/0");
	if (gswl == 0) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"%s: Open SWAPI device FAILED !!\n", __FUNCTION__ );
		return TMU_HAL_STATUS_ERR;
	}

	memset((void *)&egCfg, 0x00, sizeof(egCfg));
	memset((void *)&igCfg, 0x00, sizeof(igCfg));
	memset((void *)&cpuPortCfg, 0x00, sizeof(cpuPortCfg));
	memset((void *)&bmCfg, 0x00, sizeof(bmCfg));
	memset((void *)&wredQueueCfg, 0x00, sizeof(wredQueueCfg));
	memset((void *)&qos_queueport_set, 0x00, sizeof(GSW_QoS_queuePort_t));

	/*
	 * 2. GSWIP-L PMAC Ingress Configuration Table
	 * Entry:
	 * Address: (i from 0 to 15)
	 * DMA Channel = i
	 * Value:
	 * PMAC_Flag = 1
	 * SPPID_Mode = 1
	 * SUBID_Mode = 1
	 * CLASSEN_Mode = 1
	 * CLASS_Mode = 1 
	 * PMAPEN_Mode = 1
	 * PMAP_Mode = 1
	 * TCPERR_DP = 1
	 * DF_PMAC_HD.PMAP_EN = 1, DF_PMAC_HD.PMAP = 1 << i[2:0], DF_PMAC_HD.CLASS_EN = 1, DF_PMAC_HD.CLASS = i[3]*2 + 1
	 * Other fields = 0
	 */
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"<%s>PMAC_IG_CFG_SET for GSW-L\n",__FUNCTION__);

	for (i = 0; i <= 15; i++) {
		igCfg.nTxDmaChanId	= i;
		igCfg.bErrPktsDisc	= 1;
		igCfg.bPmapDefault	= 1;
		igCfg.bPmapEna		= 1;
		igCfg.bClassDefault = 1;
		igCfg.bClassEna	 = 1;
		igCfg.eSubId = GSW_PMAC_IG_CFG_SRC_PMAC;
		igCfg.bSpIdDefault	= 1;
		igCfg.bPmacPresent	= 1;
		igCfg.defPmacHdr[0] = 0;
		igCfg.defPmacHdr[1] = 0;
		if(i>=8)
			igCfg.defPmacHdr[2] = 15;
		else
			igCfg.defPmacHdr[2] = 7;
		igCfg.defPmacHdr[3] = 0x90; /*(1 << 7) | (1 << 4);*/
		igCfg.defPmacHdr[4] = 0;
		igCfg.defPmacHdr[5] = 0;
		igCfg.defPmacHdr[7] = 1 << (i & 0x7);
		gsw_api_kioctl(gswl, GSW_PMAC_IG_CFG_SET, &igCfg);
	}

		
	/* GSWIP-L PMAC Backpressure config	*/
	for (i = 1; i <= 6; i++) {
		bmCfg.nTxDmaChanId = i;
		bmCfg.txQMask = 1 << (4*i+1);
		bmCfg.rxPortMask = 0x01;
		gsw_api_kioctl(gswl, GSW_PMAC_BM_CFG_SET, &bmCfg);
	}
		
	for (i = 9; i <= 14; i++) {
		bmCfg.nTxDmaChanId = i;
		bmCfg.txQMask = 1 << (4*(i-8)+3);
		bmCfg.rxPortMask = 0x01;
		gsw_api_kioctl(gswl, GSW_PMAC_BM_CFG_SET, &bmCfg);
	}

	/* Do the GSWIP-R configuration for Back Pressure */
	gswr = gsw_api_kopen("/dev/switch_api/1");
	if (gswr == 0) {
		TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"%s: Open SWAPI device FAILED !!\n", __FUNCTION__ );
		return TMU_HAL_STATUS_ERR;
	}
	memset((void *)&bmCfg, 0x00, sizeof(bmCfg));
		
	bmCfg.nTxDmaChanId = 0;
	bmCfg.txQMask = 0x7fffeaab;
	bmCfg.rxPortMask = 0x007e;
	gsw_api_kioctl(gswr, GSW_PMAC_BM_CFG_SET, &bmCfg);

	memset((void *)&bmCfg, 0x00, sizeof(bmCfg));
	bmCfg.nTxDmaChanId = 6;
	bmCfg.txQMask = 0x6aaaaaa2;
	bmCfg.rxPortMask = 0x0;
	gsw_api_kioctl(gswr, GSW_PMAC_BM_CFG_SET, &bmCfg);

	memset((void *)&bmCfg, 0x00, sizeof(bmCfg));
	bmCfg.nTxDmaChanId = 5;
	bmCfg.txQMask = 0;
	bmCfg.rxPortMask = 0x0801;
	gsw_api_kioctl(gswr, GSW_PMAC_BM_CFG_SET, &bmCfg);

	memset((void *)&bmCfg, 0x00, sizeof(bmCfg));
	bmCfg.nTxDmaChanId = 7;
	bmCfg.txQMask = 0;
	bmCfg.rxPortMask = 0x0080;
	gsw_api_kioctl(gswr, GSW_PMAC_BM_CFG_SET, &bmCfg);

	memset((void *)&bmCfg, 0x00, sizeof(bmCfg));
	bmCfg.nTxDmaChanId = 8;
	bmCfg.txQMask = 0;
	bmCfg.rxPortMask = 0x100;
	gsw_api_kioctl(gswr, GSW_PMAC_BM_CFG_SET, &bmCfg);

	memset((void *)&bmCfg, 0x00, sizeof(bmCfg));
	bmCfg.nTxDmaChanId = 13;
	bmCfg.txQMask = 0;
	bmCfg.rxPortMask = 0x2000;
	gsw_api_kioctl(gswr, GSW_PMAC_BM_CFG_SET, &bmCfg);

	memset((void *)&bmCfg, 0x00, sizeof(bmCfg));
	bmCfg.nTxDmaChanId = 15;
	bmCfg.txQMask = 0x80000000;
	bmCfg.rxPortMask = 0x0;
	gsw_api_kioctl(gswr, GSW_PMAC_BM_CFG_SET, &bmCfg);

	gsw_api_kclose(gswr);

	for (i = 0; i <= 3; i++) {
		wredQueueCfg.nQueueId = i;
		wredQueueCfg.nRed_Min = 0xFF;
		wredQueueCfg.nRed_Max = 0xFF;
		wredQueueCfg.nYellow_Min = 0xFF;
		wredQueueCfg.nYellow_Max = 0xFF;
		wredQueueCfg.nGreen_Min = 0xFF;
		wredQueueCfg.nGreen_Max = 0xFF;
		gsw_api_kioctl(gswl, GSW_QOS_WRED_QUEUE_CFG_SET, &wredQueueCfg);
	}

	for (i = 4; i <= 31; i++) {
		wredQueueCfg.nQueueId = i;
		wredQueueCfg.nRed_Min = 0x40;
		wredQueueCfg.nRed_Max = 0x40;
		wredQueueCfg.nYellow_Min = 0x40;
		wredQueueCfg.nYellow_Max = 0x40;
		wredQueueCfg.nGreen_Min = 0x40;
		wredQueueCfg.nGreen_Max = 0x40;
		gsw_api_kioctl(gswl, GSW_QOS_WRED_QUEUE_CFG_SET, &wredQueueCfg);
	}

	/* Setting Config on Lan port Queues for Lan->lan QoS */
	i=j=0;
	for(i=0;i<4;i++) {
		for(j=0;j<=6;j++) {
			qos_queueport_set.nPortId = j;
			qos_queueport_set.nTrafficClassId = i;
			qos_queueport_set.nQueueId = 4*j;
			qos_queueport_set.bRedirectionBypass = 0;
			qos_queueport_set.nRedirectPortId = j;
			gsw_api_kioctl(gswl, GSW_QOS_QUEUE_PORT_SET, &qos_queueport_set);
		}
	}

	i=j=0;
	for(i=5;i<15;i++) {
		for(j=0;j<=6;j++) {
			qos_queueport_set.nPortId = j;
			qos_queueport_set.nTrafficClassId = i;
			qos_queueport_set.nQueueId = (4*j + 2);
			qos_queueport_set.bRedirectionBypass = 0;
			qos_queueport_set.nRedirectPortId = j;
			gsw_api_kioctl(gswl, GSW_QOS_QUEUE_PORT_SET, &qos_queueport_set);
		}
	}

	i=j=0;
	for(j=0;j<=6;j++) {
		qos_queueport_set.nPortId = j;
		qos_queueport_set.nTrafficClassId = 7;
		qos_queueport_set.nQueueId = (4*j + 1);
		qos_queueport_set.bRedirectionBypass = 0;
		qos_queueport_set.nRedirectPortId = j;
		gsw_api_kioctl(gswl, GSW_QOS_QUEUE_PORT_SET, &qos_queueport_set);

		qos_queueport_set.nPortId = j;
		qos_queueport_set.nTrafficClassId = 15;
		qos_queueport_set.nQueueId = (4*j + 3);
		qos_queueport_set.bRedirectionBypass = 0;
		qos_queueport_set.nRedirectPortId = j;
		gsw_api_kioctl(gswl, GSW_QOS_QUEUE_PORT_SET, &qos_queueport_set);
	}

	gsw_api_kclose(gswl);

	/* Set default queue length for HW checksum queue */
	if (cbm_dp_port_resources_get(&port_id, &nof_of_tmu_ports,
		&tmu_res_checksum, DP_F_CHECKSUM) == TMU_HAL_STATUS_OK) {
		tmu_equeue_drop_params_get(tmu_res_checksum->tmu_q, &thx);
		thx.qtth0 = 0x24;
		tmu_equeue_drop_params_set(tmu_res_checksum->tmu_q, &thx);
	}
	kfree(tmu_res_checksum);

	return TMU_HAL_STATUS_OK;
}

void tmu_hal_buffers_init(void)
{
	int i;

	for (i=0; i<TMU_HAL_MAX_EGRESS_PORT; i++) {
		tmu_hal_port_track[i].is_enabled = 0;
		tmu_hal_port_track[i].port_sched_id = 0xFF;
	}

	for (i=0; i<TMU_HAL_MAX_SCHEDULER; i++) {
		tmu_hal_sched_track[i].in_use = 0;
		tmu_hal_sched_track[i].omid = INVALID_SCHED_ID;
		tmu_hal_sched_track[i].tbs = 0xFF;
	}

	for (i=0; i<TMU_HAL_MAX_EGRESS_QUEUES; i++) {
		tmu_hal_queue_track[i].is_enabled = 0;
		tmu_hal_queue_track[i].tb_index = 0xFF;
	}

}



static void tmu_hal_register(void)
{
	TMU_HAL_DEBUG_MSG( TMU_DEBUG_TRACE,"tmu_hal_register.\n");
	ppa_drv_generic_hal_register(TMU_HAL, tmu_hal_generic_hook);
}



static int __init tmu_hal_init(void)
{
	TMU_HAL_DEBUG_MSG( TMU_ENABLE_ALL_DEBUG,"TMU HAL Init.\n");
	tmu_hal_buffers_init();
	hal_init();
	tmu_hal_register();
	tmu_hal_proc_create();
	tmu_hal_set_checksum_queue_map_hook_fn = tmu_hal_set_checksum_queue_map;
	tmu_hal_set_lro_queue_map_hook_fn = tmu_hal_set_lro_queue_map;
	tmu_hal_get_qos_mib_hook_fn = tmu_hal_get_qos_mib;
	tmu_hal_get_csum_ol_mib_hook_fn = tmu_hal_get_csum_ol_mib;
	tmu_hal_clear_csum_ol_mib_hook_fn = tmu_hal_clear_csum_ol_mib;
	tmu_hal_clear_qos_mib_hook_fn = tmu_hal_reset_qos_mib;
	tmu_hal_remove_dp_egress_connectivity_hook_fn = tmu_hal_remove_dp_egress_connectivity;
	tmu_hal_setup_dp_egress_connectivity_hook_fn = tmu_hal_setup_dp_egress_connectivity;
	tmu_hal_remove_dp_ingress_connectivity_hook_fn = tmu_hal_remove_dp_ingress_connectivity;
	tmu_hal_setup_dp_ingress_connectivity_hook_fn = tmu_hal_setup_dp_ingress_connectivity;

#ifdef TMU_HAL_TEST
#define TMU_ETHWAN_PORT 19
	PPA_QOS_RATE_SHAPING_CFG cfg;

	cfg.portid = 19;
	cfg.queueid = 300;
	cfg.rate_in_kbps = 1000;
	cfg.burst = 0;
	cfg.flag = 0;
#endif

	printk("Done!!\n");
	return 0;
}

static void __exit tmu_hal_exit(void)
{
	tmu_hal_proc_destroy();
	ppa_drv_generic_hal_deregister(TMU_HAL);
	free_pages_exact(tmu_hal_user_sub_interface_info, sizeof(struct tmu_hal_user_subif_abstract));
	free_pages_exact(tmu_hal_user_sub_interface_ingress_info, sizeof(struct tmu_hal_user_subif_abstract));

	tmu_hal_get_qos_mib_hook_fn = NULL;
	tmu_hal_set_checksum_queue_map_hook_fn = NULL;
	tmu_hal_set_lro_queue_map_hook_fn = NULL;
	tmu_hal_get_csum_ol_mib_hook_fn = NULL;
	tmu_hal_clear_csum_ol_mib_hook_fn = NULL;
	tmu_hal_clear_qos_mib_hook_fn = NULL;
	tmu_hal_remove_dp_egress_connectivity_hook_fn = NULL;
	tmu_hal_setup_dp_egress_connectivity_hook_fn = NULL;
	tmu_hal_remove_dp_ingress_connectivity_hook_fn = NULL;
	tmu_hal_setup_dp_ingress_connectivity_hook_fn = NULL;
}

module_init(tmu_hal_init);
module_exit(tmu_hal_exit);

MODULE_LICENSE("GPL");


