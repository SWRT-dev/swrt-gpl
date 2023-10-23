/******************************************************************************
 **
 ** FILE NAME	: ppe_drv_wrapper.c
 ** PROJECT	: PPA
 ** MODULES	: PPA Wrapper for PPE Driver API
 **
 ** DATE	: 14 Mar 2011
 ** AUTHOR	: Shao Guohua
 ** DESCRIPTION	: PPA Wrapper for PPE Driver API
 ** COPYRIGHT	: Copyright (c) 2017 Intel Corporation
 ** Copyright (c) 2011 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 ** HISTORY
 ** $Date		$Author				$Comment
 ** 14 MAR 2011	Shao Guohua			Initiate Version
 ** 10 DEC 2012	Manamohan Shetty		 Added the support for RTP,MIB mode 
 **									 Features
 ** 05 JUL 2017	Kamal Eradath			Merged ppe_drv_wrapper.c and ppa_datapath_wrapper.c
 *******************************************************************************/
/*
 * ####################################
 *				Head File
 * ####################################
 */
/*
 *	Common Head File
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
#include <linux/proc_fs.h>

/*
 *	Chip Specific Head File
 */
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_hal_api.h>
#include <net/ppa/ppa_drv_wrapper.h>

#if IS_ENABLED(CONFIG_SOC_GRX500)
#include <net/datapath_api.h>
#include <net/ppa/ltq_mpe_hal.h>
#include <net/ppa/ltq_tmu_hal_api.h>
#endif

int g_ppa_proc_dir_flag = 0;
struct proc_dir_entry *g_ppa_proc_dir = NULL;
/*Hook API for PPE Driver's Datapath layer: these hook will be set in PPE datapath driver*/
int32_t (*ppa_sw_add_session_hook)(PPA_BUF *skb, void *pitem) = NULL;
EXPORT_SYMBOL(ppa_sw_add_session_hook);

int32_t (*ppa_sw_update_session_hook)(PPA_BUF *skb, void *pitem, void *txifinfo) = NULL;
EXPORT_SYMBOL(ppa_sw_update_session_hook);

void (*ppa_sw_del_session_hook)(void *pitem) = NULL;
EXPORT_SYMBOL(ppa_sw_del_session_hook);

int32_t (*ppa_sw_fastpath_enable_hook)(uint32_t f_enable, uint32_t flags) = NULL;
EXPORT_SYMBOL(ppa_sw_fastpath_enable_hook);

int32_t (*ppa_get_sw_fastpath_status_hook)(uint32_t *f_enable, uint32_t flags) = NULL;
EXPORT_SYMBOL(ppa_get_sw_fastpath_status_hook);

int32_t (*ppa_sw_session_enable_hook)(void *pitem, uint32_t f_enable, uint32_t flags) = NULL;
EXPORT_SYMBOL(ppa_sw_session_enable_hook);

int32_t (*ppa_get_sw_session_status_hook)(void *pitem, uint32_t *f_enable, uint32_t flags) = NULL;
EXPORT_SYMBOL(ppa_get_sw_session_status_hook);

int32_t (*ppa_sw_fastpath_send_hook)(PPA_SKBUF *skb) = NULL;
EXPORT_SYMBOL(ppa_sw_fastpath_send_hook);

#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
int32_t (*ppa_sw_litepath_tcp_send_hook)(PPA_SKBUF *skb) = NULL;
EXPORT_SYMBOL(ppa_sw_litepath_tcp_send_hook);

int32_t (*ppa_tcp_litepath_enable_hook)(uint32_t f_enable, uint32_t flags) = NULL;
EXPORT_SYMBOL(ppa_tcp_litepath_enable_hook);

int32_t (*ppa_get_tcp_litepath_status_hook)(uint32_t *f_enable, uint32_t flags) = NULL;
EXPORT_SYMBOL(ppa_get_tcp_litepath_status_hook);
#endif

/*Hook API for PPE Driver's HAL layer: these hook will be set in PPE HAL driver */
int32_t (*ppa_drv_hal_generic_hook)(PPA_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag) = NULL;
EXPORT_SYMBOL(ppa_drv_hal_generic_hook);

/*Hook API for PPE Driver's Datapath layer: these hook will be set in PPE datapath driver*/
/* First part is for direct path */
struct ppe_directpath_data *ppa_drv_g_ppe_directpath_data = NULL;
#if IS_ENABLED(CONFIG_SOC_GRX500)
int32_t (*ppa_drv_directpath_send_hook)(PPA_SUBIF *, PPA_SKBUF *, int32_t, uint32_t) = NULL;
#else
int32_t (*ppa_drv_directpath_send_hook)(uint32_t, PPA_SKBUF *, int32_t, uint32_t) = NULL;
#endif
int32_t (*ppa_drv_directpath_rx_stop_hook)(uint32_t, uint32_t) = NULL;
int32_t (*ppa_drv_directpath_rx_start_hook)(uint32_t, uint32_t) = NULL;

#if IS_ENABLED(CONFIG_SOC_GRX500)
int32_t (*ppa_drv_directpath_register_hook)(PPA_SUBIF *, PPA_NETIF *, PPA_DIRECTPATH_CB*, int32_t*, uint32_t) = NULL;

int32_t (*mpe_hal_feature_start_fn)(
		enum MPE_Feature_Type mpeFeature,
		uint32_t port_id,
		uint32_t * featureCfgBase,
		uint32_t flags) = NULL;
EXPORT_SYMBOL(mpe_hal_feature_start_fn);

#ifdef CONFIG_INTEL_DATAPATH_MIB_TMU_MPE_MIB
int32_t (*mpe_hal_get_netif_mib_hook_fn) (struct net_device *dev,
		dp_subif_t *subif_id, struct mpe_hal_if_stats *mpe_mib,
		uint32_t flag) = NULL;
EXPORT_SYMBOL(mpe_hal_get_netif_mib_hook_fn);

int32_t (*mpe_hal_clear_if_mib_hook_fn) (struct net_device *dev,
		dp_subif_t *subif_id, uint32_t flag) = NULL;
EXPORT_SYMBOL(mpe_hal_clear_if_mib_hook_fn);
#endif

int32_t (*ppa_construct_template_buf_hook)( PPA_BUF *skb, void *p_item, void *tx_ifinfo) = NULL;
EXPORT_SYMBOL(ppa_construct_template_buf_hook);
struct session_action * (*ppa_construct_mc_template_buf_hook)(PPA_MC_INFO *p_item, uint32_t dest_list) = NULL;
EXPORT_SYMBOL(ppa_construct_mc_template_buf_hook);
void (*ppa_destroy_template_buf_hook)(void* tmpl_buf) = NULL;
EXPORT_SYMBOL(ppa_destroy_template_buf_hook);
void (*ppa_session_mc_destroy_tmplbuf_hook)(void* sessionAction) = NULL;
EXPORT_SYMBOL(ppa_session_mc_destroy_tmplbuf_hook);
#endif

int32_t (*qos_hal_get_csum_ol_mib_hook_fn)(
		struct qos_hal_qos_stats *csum_mib,
		uint32_t flag) = NULL;
EXPORT_SYMBOL(qos_hal_get_csum_ol_mib_hook_fn);

int32_t (*qos_hal_clear_csum_ol_mib_hook_fn)(
		uint32_t flag) = NULL;
EXPORT_SYMBOL(qos_hal_clear_csum_ol_mib_hook_fn);

int32_t(*qos_hal_get_qos_mib_hook_fn)(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		struct qos_hal_qos_stats *qos_mib,
		uint32_t flag) = NULL;
EXPORT_SYMBOL(qos_hal_get_qos_mib_hook_fn);

int32_t (*qos_hal_clear_qos_mib_hook_fn)(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		uint32_t flag) = NULL;
EXPORT_SYMBOL(qos_hal_clear_qos_mib_hook_fn);

/*	others:: these hook will be set in PPE datapath driver	*/
int (*ppa_drv_get_dslwan_qid_with_vcc_hook)(struct atm_vcc *vcc) = NULL;
int (*ppa_drv_get_netif_qid_with_pkt_hook)(PPA_SKBUF *skb, void *arg, int is_atm_vcc) = NULL;
int (*ppa_drv_get_atm_qid_with_pkt_hook)(PPA_SKBUF *skb, void *arg, int is_atm_vcc) = NULL;
int (*ppa_drv_ppe_clk_change_hook)(unsigned int arg, unsigned int flags) = NULL;
int (*ppa_drv_ppe_pwm_change_hook)(unsigned int arg, unsigned int flags) = NULL;
int32_t (*ppa_drv_datapath_generic_hook)(PPA_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag) = NULL;
/*below hook will be exposed from datapath driver and called by its hal driver.*/
int32_t (*ppa_drv_datapath_mac_entry_setting)(uint8_t	*mac, uint32_t fid, uint32_t portid,
		uint32_t agetime, uint32_t st_entry , uint32_t action) = NULL;

/* Hook API for datapath A1 to get MPoA type */
int32_t (*ppa_drv_hal_get_mpoa_type_hook)(uint32_t dslwan_qid, uint32_t *mpoa_type) = NULL;

#if IS_ENABLED(CONFIG_SOC_GRX500)
int ppa_drv_directpath_send(PPA_SUBIF *subif, PPA_SKBUF *skb, int32_t len, uint32_t flags)
{
	if (!ppa_drv_directpath_send_hook) {
		PPA_SKB_FREE(skb);
		return PPA_EINVAL;
	}
	return ppa_drv_directpath_send_hook(subif, skb, len, flags);
}
#else
int ppa_drv_directpath_send(uint32_t if_id, PPA_SKBUF *skb, int32_t len, uint32_t flags)
{
	if (!ppa_drv_directpath_send_hook) {
		PPA_SKB_FREE(skb);
		return PPA_EINVAL;
	}
	return ppa_drv_directpath_send_hook(if_id, skb, len, flags);
}
#endif

int ppa_drv_directpath_rx_stop(uint32_t if_id, uint32_t flags)
{
	if (!ppa_drv_directpath_rx_stop_hook)
		return PPA_EINVAL;
	return ppa_drv_directpath_rx_stop_hook(if_id, flags);
}

int ppa_drv_directpath_rx_start(uint32_t if_id, uint32_t flags)
{
	if (!ppa_drv_directpath_rx_start_hook)
		return PPA_EINVAL;
	return ppa_drv_directpath_rx_start_hook(if_id, flags);
}

#if IS_ENABLED(CONFIG_SOC_GRX500)
int ppa_drv_directpath_register(PPA_SUBIF *subif, PPA_NETIF *netif,
	PPA_DIRECTPATH_CB *pDirectpathCb, int32_t *index, uint32_t flags)
{
	if (!ppa_drv_directpath_register_hook)
		return PPA_EINVAL;
	return ppa_drv_directpath_register_hook(subif, netif, pDirectpathCb, index, flags);
}
int (*tmu_hal_remove_dp_egress_connectivity_hook_fn)(struct net_device *netdev, uint32_t pmac_port) = NULL;
EXPORT_SYMBOL(tmu_hal_remove_dp_egress_connectivity_hook_fn);
int (*tmu_hal_setup_dp_egress_connectivity_hook_fn)(struct net_device *netdev, uint32_t pmac_port) = NULL;
EXPORT_SYMBOL(tmu_hal_setup_dp_egress_connectivity_hook_fn);

int (*tmu_hal_remove_dp_ingress_connectivity_hook_fn)(struct net_device *netdev, uint32_t pmac_port) = NULL;
EXPORT_SYMBOL(tmu_hal_remove_dp_ingress_connectivity_hook_fn);
int (*tmu_hal_setup_dp_ingress_connectivity_hook_fn)(struct net_device *netdev, uint32_t pmac_port) = NULL;
EXPORT_SYMBOL(tmu_hal_setup_dp_ingress_connectivity_hook_fn);

int32_t (*tmu_hal_set_checksum_queue_map_hook_fn)(uint32_t pmac_port) = NULL;
EXPORT_SYMBOL(tmu_hal_set_checksum_queue_map_hook_fn);

int32_t (*tmu_hal_set_lro_queue_map_hook_fn)(uint32_t pmac_port) = NULL;
EXPORT_SYMBOL(tmu_hal_set_lro_queue_map_hook_fn);

int32_t (*mpe_hal_set_checksum_queue_map_hook_fn)(
		uint32_t pmac_port, uint32_t flags) = NULL;
EXPORT_SYMBOL(mpe_hal_set_checksum_queue_map_hook_fn);

#if IS_ENABLED(CONFIG_PPA_TMU_MIB_SUPPORT)
int32_t (*tmu_hal_get_csum_ol_mib_hook_fn)(
		struct tmu_hal_qos_stats *csum_mib,
		uint32_t flag) = NULL;
EXPORT_SYMBOL(tmu_hal_get_csum_ol_mib_hook_fn);
int32_t (*tmu_hal_clear_csum_ol_mib_hook_fn)(
		uint32_t flag) = NULL;
EXPORT_SYMBOL(tmu_hal_clear_csum_ol_mib_hook_fn);
int32_t(*tmu_hal_get_qos_mib_hook_fn)(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		struct tmu_hal_qos_stats *qos_mib,
		uint32_t flag) = NULL;
EXPORT_SYMBOL(tmu_hal_get_qos_mib_hook_fn);
int32_t (*tmu_hal_clear_qos_mib_hook_fn)(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		uint32_t flag) = NULL;
EXPORT_SYMBOL(tmu_hal_clear_qos_mib_hook_fn);

int32_t ppa_drv_get_csum_ol_mib(
		struct tmu_hal_qos_stats *csum_mib,
		uint32_t flag)
{
	if (!tmu_hal_get_csum_ol_mib_hook_fn)
		return PPA_EINVAL;
	return tmu_hal_get_csum_ol_mib_hook_fn (csum_mib, flag);
}
EXPORT_SYMBOL(ppa_drv_get_csum_ol_mib);

int32_t ppa_drv_clear_csum_ol_mib(
		uint32_t flag)
{
	if (!tmu_hal_clear_csum_ol_mib_hook_fn)
		return PPA_EINVAL;
	return tmu_hal_clear_csum_ol_mib_hook_fn (flag);
}
EXPORT_SYMBOL(ppa_drv_clear_csum_ol_mib);

int32_t ppa_drv_get_tmu_qos_mib (
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		struct tmu_hal_qos_stats *qos_mib,
		uint32_t flag)
{
	if (!tmu_hal_get_qos_mib_hook_fn)
		return PPA_EINVAL;
	return tmu_hal_get_qos_mib_hook_fn (netdev, subif_id, queueid, qos_mib, flag);
}
EXPORT_SYMBOL(ppa_drv_get_tmu_qos_mib);
int32_t ppa_drv_reset_tmu_qos_mib (
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		uint32_t flag)
{
	if (!tmu_hal_clear_qos_mib_hook_fn)
		return PPA_EINVAL;
	return tmu_hal_clear_qos_mib_hook_fn (netdev, subif_id, queueid, flag);
}
EXPORT_SYMBOL(ppa_drv_reset_tmu_qos_mib);
#endif

int32_t ppa_drv_tmu_set_checksum_queue_map(uint32_t pmac_port)
{
	if(!tmu_hal_set_checksum_queue_map_hook_fn)
		return PPA_EINVAL;
	return tmu_hal_set_checksum_queue_map_hook_fn (pmac_port);
}
EXPORT_SYMBOL(ppa_drv_tmu_set_checksum_queue_map);

int32_t ppa_drv_tmu_set_lro_queue_map(uint32_t pmac_port)
{
	if(!tmu_hal_set_lro_queue_map_hook_fn)
		return PPA_EINVAL;
	return tmu_hal_set_lro_queue_map_hook_fn (pmac_port);
}
EXPORT_SYMBOL(ppa_drv_tmu_set_lro_queue_map);

int32_t ppa_drv_mpe_set_checksum_queue_map(uint32_t pmac_port, uint32_t flags)
{
	if(!mpe_hal_set_checksum_queue_map_hook_fn)
		return PPA_EINVAL;
	return mpe_hal_set_checksum_queue_map_hook_fn(pmac_port, flags);
}
EXPORT_SYMBOL(ppa_drv_mpe_set_checksum_queue_map);
#endif

int ppa_drv_get_dslwan_qid_with_vcc(struct atm_vcc *vcc)
{
	if (!ppa_drv_get_dslwan_qid_with_vcc_hook)
		return 0;
	else
		return ppa_drv_get_dslwan_qid_with_vcc_hook(vcc);
}

int ppa_drv_get_netif_qid_with_pkt(PPA_SKBUF *skb, void *arg, int is_atm_vcc)
{
	if (!ppa_drv_get_netif_qid_with_pkt_hook)
		return 0;
	else
		return ppa_drv_get_netif_qid_with_pkt_hook(skb, arg, is_atm_vcc);
}

int ppa_drv_ppe_clk_change(unsigned int arg, unsigned int flags)
{
	if (!ppa_drv_ppe_clk_change_hook)
		return PPA_FAILURE;
	else return ppa_drv_ppe_clk_change_hook(arg, flags);
}

int ppa_drv_ppe_pwm_change(unsigned int arg, unsigned int flags)
{
	if (!ppa_drv_ppe_pwm_change_hook)
		return PPA_FAILURE;
	else return ppa_drv_ppe_pwm_change_hook(arg, flags);
}

uint32_t ppa_drv_dp_sb_addr_to_fpi_addr_convert(PPA_FPI_ADDR*a, uint32_t flag)
{
	if (!ppa_drv_datapath_generic_hook)
		return PPA_FAILURE;
	return ppa_drv_datapath_generic_hook(PPA_GENERIC_DATAPATH_ADDR_TO_FPI_ADDR, (void *)a, flag);
}

int32_t ppa_hook_set_lan_seperate_flag(uint32_t flag)
{
	if (!ppa_drv_datapath_generic_hook)
		return PPA_FAILURE;
	return ppa_drv_datapath_generic_hook(PPA_GENERIC_DATAPATH_SET_LAN_SEPARATE_FLAG, NULL, flag);
}
int32_t ppa_hook_get_lan_seperate_flag(uint32_t flag)
{
	if (!ppa_drv_datapath_generic_hook)
		return PPA_FAILURE;
	return ppa_drv_datapath_generic_hook(PPA_GENERIC_DATAPATH_GET_LAN_SEPARATE_FLAG, NULL, flag);
}
uint32_t ppa_hook_set_wan_seperate_flag(uint32_t flag)
{
	if (!ppa_drv_datapath_generic_hook)
		return PPA_FAILURE;
	return ppa_drv_datapath_generic_hook(PPA_GENERIC_DATAPATH_SET_WAN_SEPARATE_FLAG, NULL, flag);
}
uint32_t ppa_hook_get_wan_seperate_flag(uint32_t flag)
{
	if (!ppa_drv_datapath_generic_hook)
		return PPA_FAILURE;
	return ppa_drv_datapath_generic_hook(PPA_GENERIC_DATAPATH_GET_WAN_SEPARATE_FLAG, NULL, flag);
}

/*for PPE driver's datapath APIs*/
EXPORT_SYMBOL(ppa_drv_g_ppe_directpath_data);
EXPORT_SYMBOL(ppa_drv_directpath_send_hook);
#if IS_ENABLED(CONFIG_SOC_GRX500)
EXPORT_SYMBOL(ppa_drv_directpath_register);
EXPORT_SYMBOL(ppa_drv_directpath_register_hook);
#endif
EXPORT_SYMBOL(ppa_drv_directpath_rx_stop_hook);
EXPORT_SYMBOL(ppa_drv_directpath_rx_start_hook);
EXPORT_SYMBOL(ppa_drv_directpath_send);
EXPORT_SYMBOL(ppa_drv_directpath_rx_stop);
EXPORT_SYMBOL(ppa_drv_directpath_rx_start);

EXPORT_SYMBOL(ppa_drv_get_dslwan_qid_with_vcc_hook);
EXPORT_SYMBOL(ppa_drv_get_netif_qid_with_pkt_hook);
EXPORT_SYMBOL(ppa_drv_get_atm_qid_with_pkt_hook);
EXPORT_SYMBOL(ppa_drv_hal_get_mpoa_type_hook);
EXPORT_SYMBOL(ppa_drv_ppe_clk_change_hook);
EXPORT_SYMBOL(ppa_drv_ppe_pwm_change_hook);
EXPORT_SYMBOL(ppa_drv_get_dslwan_qid_with_vcc);
EXPORT_SYMBOL(ppa_drv_get_netif_qid_with_pkt);
EXPORT_SYMBOL(ppa_drv_ppe_clk_change);
EXPORT_SYMBOL(ppa_drv_ppe_pwm_change);
EXPORT_SYMBOL(ppa_drv_datapath_generic_hook);
EXPORT_SYMBOL(ppa_drv_datapath_mac_entry_setting);
EXPORT_SYMBOL(ppa_drv_dp_sb_addr_to_fpi_addr_convert);
EXPORT_SYMBOL(ppa_hook_get_lan_seperate_flag);
EXPORT_SYMBOL(ppa_hook_set_lan_seperate_flag);
EXPORT_SYMBOL(ppa_hook_get_wan_seperate_flag);
EXPORT_SYMBOL(ppa_hook_set_wan_seperate_flag);

/*Below wrapper is for PPE driver datapath..---- Not sure whether need to add synchronization or not ---end*/
EXPORT_SYMBOL(g_ppa_proc_dir_flag);
EXPORT_SYMBOL(g_ppa_proc_dir);

