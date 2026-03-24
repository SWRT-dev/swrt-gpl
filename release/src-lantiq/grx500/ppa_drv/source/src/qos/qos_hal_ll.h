/***************************************************************************
 **
 ** FILE NAME    : qos_hal_ll.h
 ** PROJECT      : QOS_MGR
 ** MODULES      : QOS HAL
 **
 ** AUTHOR       : Purnendu Ghosh
 ** DESCRIPTION  : QoS LL Header File
 ** COPYRIGHT : Copyright (c) 2017 - 2018 Intel Corporation
 ** Copyright (c) 2010 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 **
 ***************************************************************************/
#define QOS_HAL_Q_PRIO_LEVEL 9
#define INVALID_SCHED_ID 0xFF
#define INVALID_SHAPER_ID 256
#define QOS_HAL_DEL_SHAPER_CFG 0x00F00000
#define QOS_HAL_QUEUE_FLUSH_TIMEOUT 100000
#define INVALID_SCHEDULER_INPUT_ID 2040
#define INVALID_SCHEDULER_BLOCK_ID 0xFF
#define EGRESS_QUEUE_ID_MAX 256
#define QOS_HAL_CPU_HIGH_PRIO_CLASS 8

extern uint32_t g_Inst;
extern uint32_t g_qos_dbg;
extern uint32_t dp_reserve;

extern uint32_t g_No_Of_QOS_Res_Queue;

/* CPU Port Resources */
extern int32_t g_CPU_PortId;
extern int32_t g_CPU_Sched;
extern int32_t g_CPU_Queue;

int32_t qos_hal_qos_init_cfg(void);
int32_t qos_hal_qos_uninit_cfg(void);

int32_t
qos_hal_get_counter_mode(
			int32_t *mode);
int32_t
qos_hal_ll_get_qos_mib(
			int32_t queueid,
			struct qos_hal_qos_mib *qos_mib,
			uint32_t flag);
int32_t
qos_hal_ll_reset_qos_mib(
			int32_t queueid,
			uint32_t flag);

int32_t qos_hal_get_free_queue(void);
int32_t qos_hal_get_free_dp_queue(void);
int32_t qos_hal_get_free_scheduler(void);
int32_t qos_hal_get_free_port(void);
int32_t qos_hal_get_free_shaper(void);
int32_t qos_hal_scheduler_free_input_get(
			const uint16_t idx,
			uint8_t *leaf);
int32_t qos_hal_port_free_input_get(
			const uint16_t idx,
			uint8_t *leaf);
int qos_hal_free_prio_scheduler_get(
			uint32_t parent_sched,
			uint32_t base_sched,
			uint32_t priority,
			uint32_t *free_prio_sched);
int qos_hal_dump_sb_info(uint32_t sb_index);
int qos_hal_dump_port_info(uint32_t port_index);
int qos_hal_dump_shaper_info(uint32_t shaper_index);
int qos_hal_dump_queue_info(uint32_t queue_index);
void qos_hal_dump_all_shaper_track(void);
void qos_hal_dump_all_queue_track(void);
void qos_hal_dump_all_port_track(void);
void qos_hal_dump_all_sched_track(void);
int32_t qos_hal_dump_all_dp_link(void);
int qos_hal_get_subitf_via_ifname(
			char *dev_name,
			dp_subif_t *dp_subif);

enum qos_hal_errorcode
qos_hal_egress_queue_create(
			const struct qos_hal_equeue_create *param);
enum qos_hal_errorcode
qos_hal_enable_disable_queue(
			int index,
			int enable);
enum qos_hal_errorcode
qos_hal_egress_queue_cfg_set(
			const struct qos_hal_equeue_cfg *param);

void
qos_hal_reset_queue_track(uint32_t queue_index);

enum qos_hal_errorcode
qos_hal_egress_queue_delete(
			uint32_t queue_index);
enum qos_hal_errorcode
qos_hal_safe_queue_delete(
			struct net_device *netdev,
			char *dev_name,
			int32_t index,
			int32_t qos_port,
			int32_t remap_to_qid,
			uint32_t flags);

int32_t qos_hal_get_qid_from_queue_index(
			uint32_t index,
			uint32_t *qid);

int32_t qos_hal_get_queue_index_from_qid(
			uint32_t qid);
int32_t qos_hal_queue_prio_get(
			uint32_t q_in,
			uint32_t *prio);

int32_t qos_hal_sched_prio_get(
			uint32_t sch_in,
			uint32_t *prio);

enum qos_hal_errorcode
qos_hal_queue_relink(
			const struct qos_hal_equeue_create *param);

enum qos_hal_errorcode
qos_hal_relink_child_to_new_parent(
			uint32_t port,
			uint32_t sched,
			uint32_t from_node_type,
			struct qos_hal_user_subif_abstract *subif_index);

int32_t qos_hal_get_sched_index_from_schedid(uint32_t schedid);
int qos_hal_free_wfq_scheduler_get(
			uint32_t port,
			uint32_t base_sched,
			uint32_t *free_wfq_sched_input);
enum qos_hal_errorcode
qos_hal_safe_queue_and_shaper_delete(
			struct net_device *netdev,
			char *dev_name,
			int32_t index,
			int32_t qos_port,
			uint32_t shaper_index,
			int32_t remap_to_qid,
			uint32_t flags);
enum qos_hal_errorcode
qos_hal_sched_out_remap(
			const uint32_t sbid,
			const uint8_t lvl,
			const uint32_t omid,
			const uint8_t v,
			const uint16_t weight);

int qos_hal_q_align_for_priority_selector(
			struct net_device *netdev,
			char *dev_name,
			uint32_t port,
			struct qos_hal_user_subif_abstract *subif_index,
			uint32_t sb1_idx,
			uint32_t sb2_idx,
			uint32_t priority,
			uint32_t flags);

int qos_hal_shift_up_sbin_conn(
			struct net_device *netdev,
			char *dev_name,
			uint32_t port,
			struct qos_hal_user_subif_abstract *subif_index,
			uint32_t sbin,
			uint32_t flags);

int qos_hal_shift_down_sbin_conn(
			struct net_device *netdev,
			char *dev_name,
			uint32_t port,
			struct qos_hal_user_subif_abstract *subif_index,
			uint32_t sb_idx,
			uint32_t priority,
			uint32_t flags);

int qos_hal_create_port_scheduler(
			uint32_t port,
			enum qos_hal_policy policy,
			uint32_t *sched);

int32_t create_new_scheduler(
			int omid,
			int input_type,
			int policy,
			int level,
			int weight);
enum qos_hal_errorcode
qos_hal_scheduler_delete(uint32_t index);

int qos_hal_add_scheduler_level(
			uint32_t port,
			uint32_t base_sched,
			uint32_t priority_weight,
			uint32_t *new_base_sched,
			uint32_t prio_type,
			uint32_t *new_omid);
enum qos_hal_errorcode
qos_hal_scheduler_in_weight_update(
			uint32_t sbin,
			uint32_t weight);
int qos_hal_del_scheduler_level(
			struct net_device *netdev,
			uint32_t port,
			uint32_t base_sched,
			uint32_t level_sched,
			char *dev_name,
			uint32_t flags);
enum qos_hal_errorcode
qos_hal_scheduler_in_status_get(
			uint32_t sb_input,
			uint32_t *sie,
			uint32_t *sit,
			uint32_t *qsid);
enum qos_hal_errorcode
qos_hal_node_in_status_get(
			uint32_t node_id,
			uint32_t type,
			uint32_t *sie,
			uint32_t *sit,
			uint32_t *qsid);
enum qos_hal_errorcode
qos_hal_sched_blk_in_enable(
			uint32_t sbin,
			uint32_t enable);
enum qos_hal_errorcode
qos_hal_sched_blk_out_enable(
			uint32_t sbid,
			uint32_t enable);

int qos_hal_del_shaper_index(int shaper_index);
enum qos_hal_errorcode
qos_hal_token_bucket_shaper_delete(
			uint32_t index,
			uint32_t tbs_scheduler_block_input);
enum qos_hal_errorcode
qos_hal_token_bucket_shaper_assign_set_to_invalid(
			uint32_t shaper_index,
			uint32_t sbin);
enum qos_hal_errorcode
qos_hal_token_bucket_shaper_cfg_set(
			const struct qos_hal_token_bucket_shaper_cfg *param);
enum qos_hal_errorcode
qos_hal_token_bucket_shaper_create(
			uint32_t index,
			uint32_t tbs_scheduler_block_input);
enum qos_hal_errorcode
qos_hal_enable_queue_after_flush(
			struct net_device *netdev,
			char *dev_name,
			int32_t index,
			int32_t qos_port,
			int32_t remap_to_qid,
			uint32_t flags);

int32_t qos_hal_get_subif_list_item(uint32_t qos_port,
			struct net_device *dev,
			struct qos_hal_user_subif_abstract **obj,
			struct qos_hal_user_subif_abstract **obj_head);

void qos_hal_remove_subif_list_item(struct net_device *dev,
		struct qos_hal_user_subif_abstract **obj_head);

int qos_hal_get_q_res_from_netdevice(
			struct net_device *netdev,
			char *dev_name,
			uint32_t sub_if,
			dp_subif_t *dp_if,
			uint32_t sched_type,
			uint32_t *qos_ports_cnt,
			void **res,
			uint32_t flags);
int qos_hal_get_q_map(
			uint32_t inst,
			uint32_t q_index,
			uint32_t *no_entries,
			struct dp_queue_map_entry **p_qmap,
			int32_t flag);

int qos_hal_add_q_map(
			uint32_t q_index,
			uint32_t pmac_port,
			struct dp_queue_map_entry *p_qmap,
			uint32_t bit_mask);
int qos_hal_del_q_map(
			uint32_t q_index,
			uint32_t pmac_port,
			struct dp_queue_map_entry *p_qmap,
			uint32_t bit_mask);
int32_t qos_hal_handle_q_map(
			uint32_t q_new,
			uint32_t prio_level,
			uint32_t schedid,
			char *tc,
			uint8_t no_of_tc,
			uint32_t flowId,
			struct qos_hal_user_subif_abstract *subif_index,
			uint32_t flags);

int qos_hal_ll_init(void);

int32_t qos_hal_lookup_subif_list_item(uint32_t qosport,
			struct net_device *dev,
			struct qos_hal_user_subif_abstract **obj,
			struct qos_hal_user_subif_abstract **obj_head);
