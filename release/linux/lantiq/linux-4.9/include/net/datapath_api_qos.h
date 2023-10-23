// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 ******************************************************************************/

/*!
 * @file Datapath_api_qos.h
 *
 *  @brief Datapath QOS HAL API
 *<PRE>
 *  Purpose: try to provide common QOS HAL API and hide HW difference inside
 *           Datapath QOS HAL
 *  Note of QOS related new supports in datapath basic APIs:
 *    1) dp_alloc_port_ext:
 *         input: to reserve the required number of queues/schedulers
 *         output: cqm dequeue port base for this module
 *    2) dp_register_subif:
 *         output: cqm_deq_port/qos_deq_port/bridge port;
 *    3) dp_register_subif_ex:
 *         input: caller can specify the queue if needed. But make sure this
 *                queue is already configured properly before calling this API
 *         output: cqm_deq_port/qos_deq_port/bridge port;
 * <PRE>
 * Example
 * Queue       Sched ID: leaf/weight        Sched ID: leaf/weight    Egress Port
 *             |-----------------|    |----------------------|
 * Queue 17----|1  :0/0          |    |2 :0/0                |  Shaper CIR=100
 *             |                 |    |                      |     |
 *             |                 |    |                      |     |
 *             |                 |----|                      |-------- 7
 *             |                 |    |                      |
 *             |                 |    |                      |
 * Queue 18----|---:1/0          |    |                      |
 *             |-----------------|    |----------------------|

 * </PRE>
 * Method 1 with basic link node API dp_node_link_add from scratch (exampe1)
 *          struct dp_node_link info;
 *          int flag = 0;
 *          int dp_inst = 0;
 *          struct dp_shaper_conf shaper;
 *          struct dp_node_link_enable enable;
 *          memset(&info, 0, sizeof(info));
 *
 *          connect queue 17 to first scheduler 1
 *          info.inst = dp_inst;
 *          info.arbi = ARBITRATION_WSP_WRR;
 *          info.prio_wfq = 0;
 *          info.node_type = DP_NODE_QUEUE;
 *          info.node_id.q_id = 17;
 *          info.p_node_type = DP_NODE_SCH;
 *          info.p_node_id.sch_id = 1;
 *          if (dp_node_link_add(&info, 0) == DP_FAILURE) {
 *             return -1;
 *          }
 *
 *          connect scheduler 1 to first scheduler 2
 *          info.arbi = ARBITRATION_WSP_WRR;
 *          info.prio_wfq = 0;
 *          info.node_type = DP_NODE_SCH;
 *          info.node_id.sch_id = 1;
 *          info.p_node_type = DP_NODE_SCH;
 *          info.p_node_id.sch_id = 2;
 *          if (dp_node_link_add(&info, 0) == DP_FAILURE) {
 *             return -1;
 *          }
 *          int flag = 0;
 *          connect scheduler 2 to egress port 7
 *          info.arbi = ARBITRATION_WSP_WRR;
 *          info.prio_wfq = 0;
 *          info.node_type = DP_NODE_SCH;
 *          info.node_id.sch_id = 2;
 *          info.p_node_type = DP_NODE_PORT;
 *          info.p_node_id.cqm_deq_port = 7;
 *          if (dp_node_link_add(&info, 0) == DP_FAILURE) {
 *              return -1;
 *          }
 *
 *          connect queue 18 to first scheduler 1
 *          info.inst = dp_inst;
 *          info.arbi = ARBITRATION_WSP_WRR;
 *          info.prio_wfq = 0;
 *          info.node_type = DP_NODE_QUEUE;
 *          info.node_id.q_id = 18;
 *          info.p_node_type = DP_NODE_SCH;
 *          info.p_node_id.sch_id = 1;
 *          if (dp_node_link_add(&info, 0) == DP_FAILURE) {
 *             return -1;
 *          }
 *
 *          set port shaper
 *          struct dp_shaper_conf shaper;
 *          memset(&shaper, 0, sizeof(shaper));
 *          shaper.type = DP_NODE_SCH;
 *          shaper.id.sch_id = 2;
 *          shaper.cir = 100;
 *          shaper.pir = 100;
 *          dp_shaper_conf_set(&shaper, flag);
 *
 *          enable queue/scheduler
 *          enable.type = DP_NODE_QUEUE;
 *          enable.id.q_id = 17;
 *          enable.en = DP_NODE_EN;
 *          dp_node_link_en_set(&enable, DP_NODE_AUTO_SMART_ENABLE);
 *          enable.id.q_id = 18;
 *          dp_node_link_en_set(&enable, DP_NODE_AUTO_SMART_ENABLE);
 *
 *
 * Method 2 with basic link node API dp_link_add from scratch (example2)
 *          int flag = DP_NODE_AUTO_SMART_ENABLE;
 *          int dp_inst = 0;
 *          struct dp_shaper_conf shaper;
 *          struct dp_qos_link full_link;
 *          int dp_port = 3;
 *
 *          memset (&full_link, 0, sizeof(full_link));
 *          full_link.inst = dp_inst;
 *          full_link.cqm_deq_port = 7;
 *          full_link.q_id = 1;
 *          full_link.q_arbi = ARBITRATION_WSP_WRR;
 *          full_link.q_prio_wfq = 0;
 *          full_link.n_sch_lvl = 2;
 *          full_link.sch[0].id = 1;
 *          full_link.sch[0].arbi = ARBITRATION_WSP_WRR;
 *          full_link.sch[0].prio_wfq = 0;
 *          full_link.sch[1].id = 2;
 *          full_link.sch[1].arbi = ARBITRATION_WSP_WRR;
 *          full_link.sch[1].prio_wfq = 0;
 *          full_link.dp_port = dp_port;
 *          if (dp_link_add(&full_link,flag) == DP_FAILURE) {
 *              return -1;
 *          }
 *          full_link.q_id = 2;
 *          if (dp_link_add(&full_link,flag) == DP_FAILURE) {
 *              return -1;
 *          }
 *
 *          set port shaper
 *          struct dp_shaper_conf shaper;
 *          memset(&shaper, 0, sizeof(shaper));
 *          shaper.type = DP_NODE_SCH;
 *          shaper.id.sch_id = 2;
 *          shaper.cir = 100;
 *          shaper.pir = 100;
 *          dp_shaper_conf_set(&shaper, flag);
 *
 * Example 3 with PON OMCI and one pmapper case
 *
 *          int inst = 0;
 *          struct module *owner = NULL;
 *          u32 dev_port = 0;
 *          int32_t dp_port = 0;
 *          struct net_device *omci_dev = NULL;
 *          struct net_device *pmapper_dev = NULL;
 *          struct dp_port_data port_data = {0};
 *          uint32_t flags = 0;
 *          dp_cb_t cb;
 *          dp_subif_t subif_id;
 *          struct dp_subif_data subif_data;
 *          int32_t res;
 *
 *          flags = DP_F_GPON;
 *          port_data.flag_ops = DP_F_DATA_RESV_Q | DP_F_DATA_RESV_SCH;
 *          port_data.num_resv_q = 64;
 *          port_data.num_resv_sched = 64;
 *          dp_port = dp_alloc_port_ext(inst, owner, omci_dev, dev_port,
 *                        dp_port, NULL, &port_data, flags);
 *          if (dp_port == DP_FAILURE) {
 *              pr_err("Fail to allocate port\n");
 *              return -1;
 *          }
 *
 *          it needs to register device, ie, cb properly.
 *          At least, it needs set below callbacks:
 *          1) rx_fn
 *          2) get_subifid_fn: mainly for cpu tx path to fill in proper CTP, esp
 *             for pmapper related cases
 *          flags = 0;
 *          res = dp_register_dev_ext(inst, owner, dp_port,
 *                   &cb, NULL, flags);
 *          if (res == DP_FAILURE) {
 *               pr_err("Fail to register dev\n");
 *              dp_alloc_port_ext(inst, owner, omci_dev, dev_port,
 *              dp_port, NULL, &port_data, DP_F_DEREGISTER);
 *              return -1;
 *          }
 *
 *          Register OMCI Dev
 *          First it needs to create omci dev and store in omci_dev
 *          Once it is ready, call below dp_register_subif_ext API.
 *          After dp_register_subif_ext, below QOS structure will be created
 *          Queue       Sched ID: leaf/weight         Egress Port
 *                      |----------------------|
 *          Queue xx----|1  :0/0               |
 *                      |                      |
 *                      |                      |
 *                      |                      |---- ----- 26
 *                      |                      |
 *                      |                      |
 *                      |                      |
 *                      |----------------------|
 *          flags = 0;
 *          subif_id.inst = inst;
 *          subif_id.port_id = dp_port;
 *          subif_id.subif_num = 1;
 *          subif_id.subif = 0; OCMI CTP
 *          subif_data.deq_port_idx = 0;
 *          res = dp_register_subif_ext(inst, owner, omci_dev, omci_dev->name,
 *                           &subif_id, &subif_data, flags);
 *          if (res == DP_FAILURE) {
 *              pr_err("Fail to regisster subif port\n");
 *              dp_register_dev_ext(inst, owner, dp_port,
 *                            NULL, NULL, DP_F_DEREGISTER);
 *              dp_alloc_port_ext(inst, owner, omci_dev, dev_port,
 *                     dp_port, NULL,
 *                     &port_data, DP_F_DEREGISTER);
 *          return -1;
 *          }
 *
 *          Register pmapper device
 *          First it needs to create new Dev and store in pmapper_dev
 *          Once it is ready, call below dp_register_subif_ext API.
 *          After dp_register_subif_ext, below QOS structure will be created:
 *          Queue       Sched ID: leaf/weight         Egress Port
 *                      |----------------------|
 *          Queue xx----|x  :0/0               |
 *                      |                      |
 *                      |                      |
 *                      |                      |---- ----- 26
 *                      |                      |
 *                      |                      |
 *                      |----------------------|
 *
 *          Queue yy----|y  :0/0               |
 *                      |                      |
 *                      |                      |
 *                      |                      |---- ----- 27
 *                      |                      |
 *                      |                      |
 *                      |----------------------|
 *
 *          subif_id.subif = 1; 1st CTP for one pmapper
 *          subif_data.flag_ops = DP_SUBIF_Q_PER_CTP | DP_SUBIF_PCP;
 *          subif_data.deq_port_idx = 1;
 *          subif_data.pcp = 0;
 *          res = dp_register_subif_ext(inst, owner, pmapper_dev,
 *                     pmapper_dev->name,
 *                     &subif_id, &subif_data, flags);
 *          if (res == DP_FAILURE) {
 *               pr_err("Fail to regisster subif port\n");
 *               return -1;
 *          }

 *          Register 2nd CTP based on same Dev and same pmapper
 *          After dp_register_subif_ext,below QOS structure will be created:
 *          Queue       Sched ID: leaf/weight         Egress Port
 *                      |----------------------|
 *          Queue xx----|x  :0/0               |
 *                      |                      |
 *                      |                      |
 *                      |                      |---- ----- 26
 *                      |                      |
 *                      |                      |
 *                      |----------------------|
 *
 *          Queue yy----|y  :0/0               |
 *                      |                      |
 *                      |                      |
 *                      |                      |---- ----- 27
 *          Queue zz----|    1/0               |
 *                      |                      |
 *                      |----------------------|
 *
 *          subif_id.subif = 2; 2nd Data CTP
 *          DP_SUBIF_Q_PER_CTP bit used to create a new queue.
 *          Otherwise, it will share same queue yy
 *
 *          subif_data.flag_ops = DP_SUBIF_Q_PER_CTP | DP_SUBIF_PCP;
 *          subif_data.deq_port_idx = 1;
 *          subif_data.pcp = 1;
 *          res = dp_register_subif_ext(inst, owner, omci_dev, omci_dev->name,
 *                    &subif_id, &subif_data, flags);
 *          if (res == DP_FAILURE) {
 *               pr_err("Fail to regisster subif port\n");
 *              return -1;
 *          }
 *
 *
 * Note:
 *  1) DP QOS HAL API needs maintain below mapping:
 *     a) Physical queue id (user)  <---> Queue Node ID (ppv4)
 *     b) Physical CQM dequeue port <--> QOS dequeue port Node ID (ppv4)
 * </PRE>
 */
/*! @defgroup Datapath_QOS Datapath QOS HAL
 *@brief All API and defines exported by Datapath QOS HAL
 */
/*! @{ */
/*!
 * @defgroup APIs_link_related QOS link subgroup
 * @brief HAL API for link related operation:
 *        Single link node operation: dp_node_link_add
 *                                    dp_node_link_get
 *                                    dp_node_unlink
 *        Smart full link path operation: dp_link_add
 * @defgroup APIs_link_node_en QOS node enable subgroup
 * @brief HAL API for link node enable:dp_node_link_en_set/get
 * @defgroup APIs_link_node_prio QOS node priority subgroup
 * @brief HAL API for link node priority setting: dp_qos_link_prio_set/get
 * @defgroup APIs_dp_queue_conf QOS queue configuration subgroup
 * @brief HAL API for link queue conf: dp_queue_conf_set/get
 * @defgroup APIs_dp_shaper_conf_set QOS shaper subgroup
 * @brief HAL API for shaper/bandwidth setting: dp_shaper_conf_set/get
 * @defgroup APIs_dp_node_alloc QOS node allocation subgroup
 * @brief HAL API for link node (queue/scheduler) allocation: dp_node_alloc/free
 * @defgroup APIs_dp_queue_map_set QOS queue mapping subgroup
 * @brief HAL API for queue mapping: dp_queue_map_set/get
 * @defgroup APIs_dp_qos_example PON registration and QOS example
 * @brief HAL API example1/example2/gpon_example3
 */
 /*! @} */
#ifndef DP_QOS_API_H
#define DP_QOS_API_H

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
#include <net/datapath_api_qos_grx500.h>
#else
#define DP_NODE_SMART_FREE 1 /*< @brief flag to free node
			      *  and its parent if no child
			      */

#define DP_MAX_SCH_LVL  3 /*!< @brief max number of hierarchy QOS layers
			   *   supported.
			   *  can change MACRO for more layers
			   */
#define DP_NODE_AUTO_ID -1  /*!< @brief auto allocate a node for queue and
			     *   scheduler
			     */
#define DP_AUTO_LEAF -1 /*!< @brief auto generate leaf in the scheduler.
			 *  For TMU QOS's scheduler, leaf need to set
			 */
#define DP_MAX_COLORS  3  /*!< @brief support max number of color */

#define DP_NO_SHAPER_LIMIT 0xFFFFFFFE /*!< @brief no limit of shaper*/
#define DP_MAX_SHAPER_LIMIT 0xFFFFFFFF /*!< @brief max limit of shaper*/

/*! @brief QOS Link Node Type: Queue, scheduler and dequeue port*/
enum dp_node_type {
	DP_NODE_UNKNOWN = 0,  /*!< Unallocated node*/
	DP_NODE_QUEUE,  /*!< queue node*/
	DP_NODE_SCH,   /*!< scheduler node*/
	DP_NODE_PORT   /*!< port node*/
};

/*! @brief QOS Link Node ID:
 *  @note for queue id or scheduler id, for add API, if its value equals to
 *        DP_NODE_AUTO_ID, add API will auto allocate a
 *        queue or scheduler for it
 */
union dp_node_id {
	int q_id;  /*!< queue physical id */
	int sch_id; /*!< scheduler id:
		     *    for pp, it is logical
		     *    for TMU, it is physical
		     */
	int cqm_deq_port; /*!< cbm/cqem dequeue port */
};

/*! @brief node flag to enable/disable/keep current setting */
enum dp_node_en {
	DP_NODE_DIS = BIT(0),     /*!< disable node:drop new incoming pkt
				   *   for ppv4, valid queue/port only
				   */
	DP_NODE_EN = BIT(1),      /*!< enable node:allow to enqueue
				   *   for ppv4, valid queue/port only
				   */
	DP_NODE_SUSPEND = BIT(2), /*!< Suspend this node no scheduling:
				   *   Not for TMU
				   *   for ppv4, valid queue/sched/port
				   */
	DP_NODE_RESUME = BIT(3),   /*!< Resume scheduling for this node:
				   *  Not for TMU
				   *  for ppv4,valid queue/sched/port
				   */
	DP_NODE_SET_CMD_MAX = BIT(4)
};

/*! @brief arbitration method used for the node in its parents scheduler/ports*/
enum dp_arbitate {
	ARBITRATION_NULL = 0, /*!< No arbitrate */
	ARBITRATION_WRR,  /*!< round robin: for ppv4 */
	ARBITRATION_SP,  /*!< strict priority: for TMU */
	ARBITRATION_WSP,  /*!< strict priority: for ppv4 */
	ARBITRATION_WSP_WRR, /*!< combination of strict priority + round robin:
			      *   ppv4 also not support at present
			      */
	ARBITRATION_WFQ,     /*!< fwq: for TMU/ppv4*/
	ARBITRATION_PARENT,  /*!< used parent's arbitration mode,
			      *   parent's node ID must be valid
			      */
};

/*! @brief dp_node_flag */
enum dp_node_flag {
	DP_NODE_SMART_UNLINK = BIT(0),/*!< Unlink this specified node together
				       *   with all child
				       *  @note it is for unlink API only
				       */
	DP_NODE_AUTO_FREE_RES = BIT(1),/*!< auto free QOS resource after unlink.
					*  @note it is for unlink API only
					*/
	DP_NODE_AUTO_ENABLE = BIT(2),/*!< DP will auto enable this node if it is
				      *  set.
				      *  @note it is for link_add related API
				      */
	DP_NODE_AUTO_SMART_ENABLE = BIT(3),/*!< DP will auto enable all nodes
					    *   in the node's full path:
					    *  @note it is for link_add related
					    *   API only
					    */

	DP_ALLOC_RESV_ONLY = BIT(4),/*!< Only allocate node from this device's
				     *  resverved resource.
				     *  Reservation can be done via
				     *  dp_alloc_port_ext
				     *  @note it is for link_alloc/link_add
				     *  related API only
				     */
	DP_ALLOC_GLOBAL_ONLY = BIT(5),/*!< Only allocate node from the system
				       * global free resource.
				       *  @note it is for link_alloc/link_add
				       *  related API only
				       */
	DP_ALLOC_GLOBAL_FIRST  = BIT(6),/*!< allocate node follow the sequence:
					 * 1) system global free resource
					 * 2) device reserved resource
					 * @note it is for link_alloc/link_add
					 *  related API only
					 */
};

/*! @brief QOS link node atribute to setup/remove/get a link note */
struct dp_node_link {
	int inst; /*!< input: dp instance. For SOC side, it is always zero.
		   *         Anyway just call dp_get_netif_subifid to get it
		   */
	int dp_port;   /*!< input[optional]: based on it to
			*    get the free node if there is reservation
			*    during dp_alloc_port_ext
			*/
	enum dp_node_type node_type; /*!< input node type:can be queue/scheduler
				      *  /port
				      */
	union dp_node_id node_id; /*!< input node id.
				   *  if id == DP_NODE_AUTO_ID, DP will
				   *  allocate a free node from the reserved
				   *  pool or global pool and set node id
				   */

	enum dp_arbitate arbi;  /*!< <PRE>arbitration method used in its parents
				 *  for this node
				 * for dp_node_link_add: it is in  input
				 * for dp_node_link_get: it is in  output
				 * for dp_node_unlink: no use </PRE>
				 */
	int prio_wfq; /*!< <PRE>node priority
		       * for dp_node_link_add: it is in  input
		       * for dp_node_link_get: it is in  output
		       * for dp_node_unlink: no use </PRE>
		       */

	int leaf; /*!< <PRE>The leaf in the parent of this node
		   *  Only valid for TMU,valid value: 0 - 7
		   * for dp_node_link_add: it is in  input
		   * for dp_node_link_get: it is in  output
		   * for dp_node_unlink: no use</PRE>
		   */
	enum dp_node_type p_node_type;  /*!< <PRE>parent node type: scheduler/
					 *  dequeue port
					 *  for dp_node_link_add: it is input
					 *  for dp_node_link_get: it is output
					 *  for dp_node_unlink: no use</PRE>
					 */
	union dp_node_id p_node_id; /*!< <PRE>parent id
				     * for dp_node_link_add: it is in input
				     *   if type is scheduler
				     *     if id == DP_NODE_AUTO_ID
				     *        DP will allocate a free node
				     *        from the reserved or global pool
				     *        and set p_node_id
				     *     else use user specified parent id
				     *   else type is port
				     *        User must provide valid
				     *        cqm_deq_port value
				     * for dp_node_link_get: it is in output
				     * for dp_node_unlink: no use
				     * </PRE>
				     */
	union dp_node_id cqm_deq_port; /*!< <PRE>input/optional:
					*  for TMU queue link setup, it is
					*  required to specify dequeue port
					*  for dp_node_link_add: it is in input
					*  for dp_node_link_get: it is in output
					*  for dp_node_unlink: no use</PRE>
					*/
};

/*! \ingroup APIs_link_related
 * @brief Add a Link node for queue and scheduler.
 * @param [in,out] info struct dp_node_link *info
 * @param [in] flag int flag, refer to enum \ref dp_node_flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 * @note
 *  For queue: it is used to attach a queue to its parent, ie, scheduler <br />
 *  For scheduler: it is used to attach a scheduler to its parent:
 *      another scheduler or egress port <br />
 *  If it is used to move an existing scheduler to a new scheduler but still
 *     with original dequeue port, DP need to do necessary protection
 *     as dp_node_unlink API <br />
 *  If caller want to directly move an existing scheduler to a different cqm
 *     dequeue port, it is not allowed, esp for TMU case in order to make
 *     the QOS HAL API simple. <br />
 *  If caller really want to move the node to another dequeue port, it needs to
 *     unlink the node and re-link to a new dequeue port<br />
 *  If flag DP_NODE_AUTO_ENABLE/DP_NODE_AUTO_SMART_ENABLE is set, it will auto
 *     enable related node or nodes as specified, including its indirect and
 *     direct parents.<br />
 *  If flag DP_ALLOC_XXX related is set, it will try to allocate a new node
 *     as specified priority from reserved pool or global pool<br />
 */
int dp_node_link_add(struct dp_node_link *info, int flag);

/*! \ingroup APIs_link_related
 * @brief unlink a QOS node for its QOS configuration: queue or scheduler
 * @param [in,out] info struct dp_node_link *info
 * @param [in] flag int flag, refer to enum \ref dp_node_flag
 * @return [out] integer value: return DP_SUCCESS if succeed
 *                              otherwise, return DP_FAILURE
 *  @note It is used to unlink a specified node or including its full path
 *        depends on the flag set
 * <PRE>
 *  1) if it is to unlink a queue node, DP will do necessary work as below:
 *        re-map the lookup entry to a drop queue
 *        flushing the queue: not including those already dequeue by CQM
 *        disable the queue
 *        unlink the queue node as specified from its parent== Not needed?
 *  2) if it is to unlink a scheduler node, DP will do necessary work as below:
 *       a) if there is still child linked to this scheduler yet
 *             i) without flag DP_NODE_SMART_UNLINK set:
 *                   return DP_FAILURE
 *             ii) with flag DP_NODE_SMART_UNLINK set:
 *                   unlink its all child first,== Not needed?
 *                   then unlink this node as specified.== Not needed?
 *  Note:
       a) This API only unlik the node from the QOS setup by default, but the
 *        node itself is not freed.
 *     b) If the caller realy want to free the unlinked node automatically,
 *        it needs to specify flag with DP_NODE_AUTO_FREE_RES
 *     c) If DP_NODE_SMART_UNLINK set: DP will unlink all its child note also
 *     d) Normally top layer caller should not delete all nodes under one
 *        specific CQM dequeue port. But if really need to do, just set proper
 *        node_type and and node_id. -- Need take care
 * </PRE>
 */
int dp_node_unlink(struct dp_node_link *info, int flag);

/*! \ingroup APIs_link_related
 * @brief get a node's link configration
 * @param [in,out] info struct dp_node_link *info
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_node_link_get(struct dp_node_link *info, int flag);

/*! @brief struct for enable/disabling a link node */
struct dp_node_link_enable {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	enum dp_node_type type; /*!< link node type */
	union dp_node_id id; /*!< link node id */
	enum dp_node_en en;  /*!< action flag: enable/disable */
};

/*! \ingroup APIs_link_node_en
 * @brief enable/disable a link node
 * @param [in,out] en pointer to struct dp_node_link_enable *en
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 * @note if flag DP_NODE_AUTO_SMART_ENABLE is set, it will auto enable all
 * related indrect and dirct parents
 */
int dp_node_link_en_set(struct dp_node_link_enable *en, int flag);

/*! \ingroup APIs_link_node_en
 * @brief get enable/disable status for a link node
 * @param [in,out] en struct dp_node_link_enable *en
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_node_link_en_get(struct dp_node_link_enable *en, int flag);

/*! @brief structure for multiple scheduler attribute */
struct dp_sch_lvl {
	int id;     /*!< scheduler id:
		     *    for ppv4, it is scheduler node id.
		     *    for TMU, it is physical id
		     * sch_id >=0: set up the link for this queue
		     * sch_id==DP_NODE_AUTO_ID: alloc a free scheduler and
		     *                          set up the link
		     */
	int prio_wfq; /*!< scheduler output priority to its next scheduler
		       *  or egress port
		       */
	int leaf;     /*!< scheduler output leaf to the next scheduler ???
		       *  Since only valid for TMU case, shall is auto shift the
		       *  the queue according to the prio_wfq to get the leaf ??
		       */
	enum dp_arbitate arbi; /*!< arbitration method used for this link node
				* in its parents
				*/
};

/*!
 *  @brief QOS full link note's attribute in order to quickly
 *   set up a full QoS full from like below:
 *   queue -> scheduler -> .... -> dequeue port
 */
struct dp_qos_link {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	int dp_port;   /*!< input[optional]: based on it to
			*    get the free node if there is reservation
			*    during dp_alloc_port_ext and flag
			*    DP_NODE_AUTO_ID is set
			*/
	int cqm_deq_port; /*!< cbm dequeue port */
	int q_id;  /*!< physical queue id
		    *    id >=0: queue id specified by caller itself
		    *    id==DP_NODE_AUTO_ID: alloc a free queue and set up
		    */
	enum dp_arbitate q_arbi;/*!< arbitration method used in its parents for
				 *  for this queue
				 */
	int q_prio_wfq; /*!<  queue priority
			 *    input for dp_qos_link_add API
			 *    output for dp_qos_link_get API
			 */
	int q_leaf;   /*!<  The leaf to the scheduler input ???
		       *   not valid for ppv4
		       */
	int n_sch_lvl; /*!<  The number of Scheduler provided in the
			*   variable sch[DP_MAX_SCH_LVL].
			*   it should be less than DP_MAX_SCH_LVL
			*    input for dp_qos_link_add API
			*    output for dp_qos_link_get API
			*/
	struct dp_sch_lvl sch[DP_MAX_SCH_LVL]; /*!<scheduler info*/
};

/*! \ingroup APIs_link_related
 * @brief A quick way to add a full queue path with a single API call.
 * @param [in,out] cfg struct dp_qos_link *cfg
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed
 *                              otherwise, return DP_FAILURE
 * @note <PRE> It supports below example with a single API call:
 *    queue -> egress port, or
 *    queue -> scheduler -> egress port  or
 *    queue -> scheduler -> ... -> scheduler -> egress port
 *  if flag DP_NODE_AUTO_SMART_ENABLE is set, it will auto enable related nodes
 *    dp_link_add in this full path
 * </PRE>
 */
int dp_link_add(struct dp_qos_link *cfg, int flag);

/*! \ingroup APIs_link_related
 * @brief A quick way to get a full queue path configure instead of multipel
 *  calling of dp_node_link_get
 *  just based on input parameter inst, deq_port and q_id;
 * @param [in,out] cfg struct dp_qos_link *cfg
 * @param [in] flag  int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_link_get(struct dp_qos_link *cfg, int flag);

/*! @brief dp_node_prio*/
struct dp_node_prio {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	enum dp_node_type type; /*!< Link Node */
	union dp_node_id id; /*!< input node id */
	enum dp_arbitate arbi; /*!< arbitration method used for this link node
				* in its parents(scheduler/egress port)
				*/
	int prio_wfq; /*!< priority for WSP:
		       *    For TMU strict priority: 0-(highest) 1023-(lowest)
		       *    But don't know PPv4 behavior yet ????
		       *  priority for WFQ mode, it is WFQ weight.
		       */
};

/*! \addtogroup APIs_link_node_prio
 * @brief APIs_link_node_prio group
 * @param [in,out] info struct dp_node_prio *info
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 * @note ????? need to change leaf for TMU case ????.
 * Waiting Purnendu to confirm.
 *  @{
 */
/*!
 *  @brief function dp_qos_link_prio_set
 */
int dp_qos_link_prio_set(struct dp_node_prio *info, int flag);
/*!
 *  @brief function dp_qos_link_prio_get
 */
int dp_qos_link_prio_get(struct dp_node_prio *info, int flag);
/*! @} */

/*! @brief dp_q_drop_mode*/
enum dp_q_drop_mode {
	DP_QUEUE_DROP_TAIL, /*!< tail drop mode. */
	DP_QUEUE_DROP_WRED,  /*!< wred mode */
};

/*! @brief dp_color*/
enum dp_color {
	DP_COLOR_GREEN = 0, /*!< green color */
	DP_COLOR_YELLOW,   /*!< yellow color */
	DP_COLOR_RED,  /*!< red color */
};

/*! @brief dp_q_size_unit*/
enum dp_q_size_unit {
	DP_COLOR_PKT = 0, /*!< in packet*/
	DP_COLOR_BYTE,   /*!< in bytes*/
};

/*! @brief dp_q_codel */
enum dp_q_codel {
	DP_CODEL_DIS = 0, /*!< codel disable */
	DP_CODEL_EN,   /*!< codel enable */
};

/*! @brief dp_queue_conf*/
struct dp_queue_conf {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	int q_id; /*!< input: q_id */
	enum dp_node_en act;  /*!< enable/disable/suspend/resume queue */
	enum dp_q_drop_mode drop; /*!< TMU: wred/tail drop, how about PP?? */
	enum dp_q_size_unit unit; /*!< queue size unit:packet/bytes */
	u32 min_size[DP_MAX_COLORS]; /*!< queue minimal size, If QOCC less than
				      * this setting, should be no drop
				      */
	u32 max_size[DP_MAX_COLORS]; /*!< queue maximum size, If QOCC more than
				      * this setting, should be dropped.
				      * For tail drop mode, it is not valid
				      */
	u32 wred_slope[DP_MAX_COLORS];/*!< in percent, for example, 1 means 1%*/
	u32 wred_min_guaranteed; /*!< ??? from ppv4 */
	u32 wred_max_allowed; /*!< ??? from ppv4 */
	enum dp_q_codel codel;  /*!< [in] enable/disable/ queue codel param */
};

/*! \addtogroup APIs_dp_queue_conf
 * @brief Set(add/remove/disable) shaper/bandwidth based on its node
 * @param [in,out] cfg struct dp_shaper_conf *cfg
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 *  @{
 */
/*!
 *  @brief function to set queue configuration
 */
int dp_queue_conf_set(struct dp_queue_conf *cfg, int flag);
/*!
 *  @brief function to get queue configuration
 */
int dp_queue_conf_get(struct dp_queue_conf *cfg, int flag);
/*! @} */

/*! @brief enum dp_shaper_cmd */
enum dp_shaper_cmd {
	DP_SHAPER_CMD_ADD = 0, /*!< add shaper */
	DP_SHAPER_CMD_REMOVE,  /*!< remove shaper */
	DP_SHAPER_CMD_ENABLE,  /*!< ppv4 does not support enable
				* just use ADD shaper
				*/
	DP_SHAPER_CMD_DISABLE, /*!< disable the shaper: no limit
				* for ppv4 does not support disable
				* instead just remove shaper
				*/
};

/*! @brief dp_shaper_conf */
struct dp_shaper_conf {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */

	enum dp_shaper_cmd cmd; /*!< command */
	enum dp_node_type type; /*!< Shaper type */
	union dp_node_id id;  /*!< node id for queue/scheduler/port*/
	u32 cir;  /*!< bandwidth in kbps */
	u32 pir; /*!< bandwidth in kbps. PPV4 support ?? */
	u32 cbs;  /*!< cbs. Shoud be in Bytes, if need 16K burst,
		   * then need to give 16*1024,
		   * value should be multiplication of Quanta
		   * Quanta currenty in PPv4 is 4
		   * if other values there will be rounding and result
		   * not accurate
		   */
	u32 pbs;  /*!< pbs. Should be in Bytes if need 16K burst,
		   * then need to give 16*1024,
		   * value should be multiplication of Quanta
		   * Quanta currenty in PPv4 is 4
		   * if other values there will be rounding and result
		   * not accurate
		   */
};

/*! \addtogroup APIs_dp_shaper_conf_set
 * @brief Set(add/remove/disable) shaper/bandwidth based on its node
 * @param [in,out] cfg struct dp_shaper_conf *cfg
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 *  @{
 */
/*!
 *  @brief function to set shaper configuration based on its node
 */
int dp_shaper_conf_set(struct dp_shaper_conf *cfg, int flag);
/*!
 *  @brief function to get shaper configuration based on its node
 */
int dp_shaper_conf_get(struct dp_shaper_conf *cfg, int flag);
/*! @} */

/*! @brief dp_node_alloc */
struct dp_node_alloc {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	int dp_port;   /*!< input[optional]: based on it to
			*    get the free node if there is reservation
			*    during dp_alloc_port_ext
			*/

	enum dp_node_type type; /*!< node type: queue or scheduler */
	union dp_node_id id;  /*!< <PRE>node id: if id == DP_NODE_AUTO_ID,
			       *  allocate a free node from global pool or from
			       *  its reserved pool
			       *  otherwise provided by the caller itself.</PRE>
			       */
	//int cqm_dq_port; /*Added for qos slim driver only*/
};

/*! \ingroup APIs_dp_node_alloc
 * @brief allocate a node(queue or scheduler)
 * @param [in,out] node struct dp_node_info *node
 * @param [in] flag int, refer to enum \ref dp_node_flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_node_alloc(struct dp_node_alloc *node, int flag);

/*! \ingroup APIs_dp_node_alloc
 * @brief free a node: queue or scheduler
 * @param [in,out] node struct dp_node_alloc *node
 * @param [in] flag int flag
 *             if flag DP_NODE_SMART_FREE set try to free parent if no child
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_node_free(struct dp_node_alloc *node, int flag);

/*! \ingroup dp_node_children_free
 * @brief free all children based on specified parent node
 * @param [in,out] node struct dp_node_alloc *node
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *				otherwise, return DP_FAILURE
 */
int dp_node_children_free(struct dp_node_alloc *node, int flag);

/*! @brief dp_queue_res */
struct dp_queue_res {
	int q_id; /*!< queue id */
	int q_node; /*!< queue logica node id. For debugging only */
	int sch_lvl; /*!< number of scheduler layers configured for this queue*/
	int sch_id[DP_MAX_SCH_LVL]; /*!< Scheduler information.
				     *  @note the scheduler of sch_id[0] is the
				     *   one which the queue attached if there
				     *   scheduler used for this queue
				     */
	int leaf[DP_MAX_SCH_LVL]; /*!< leaf information. Valid only for TMU HW
				   *
				   */
	int cqm_deq_port; /*!< cqm dequeue port: absolute port id */
	int qos_deq_port; /*!< qos dequeue port: Normally user no need to know*/
	u32 cqm_deq_port_type; /*!< output: specify DQ port type, like CPU,
				* DPDK, MPE and so on.
				*/
	u32 cpu_id; /*!< valid only for CPU port */
	u16 cpu_gpid; /*!< gpid value */
};

#define DEQ_PORT_OFFSET_ALL -1 /*!< @brief Port offset all */

/*! @brief dp_dequeue_res */
struct dp_dequeue_res {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	struct net_device *dev; /*!< <PRE>input: get resource for this dev
				 *         NULL Allowed, for example  ATM/CPU
				 *         If valid, then dp_port no use</PRE>
				 */
	int dp_port;  /*!< input: if DEV NULL, dp_port must be valid,
		       *  otherwise no use
		       */
	int cqm_deq_idx; /*!< <PRE>get resource as specified dequeue port
			  *  offset (relative)
			  *  If it is DEQ_PORT_OFFSET_ALL, it means
			  *  get all resource under that dev/dep_port
			  *  related device.
			  *  DEQ_PORT_OFFSET_ALL is mainly for PON/CPU case,
			  *  since multiple dequeue port applied
			  *  For pon case, cqm_deq_port is like tcont idx
			  * </PRE>
			  */
	int cqm_deq_port; /*!< <PRE>get resource as specified dequeue port
			   *  absolution dequeue port
			   *  output only:
			   *    for cqm_deq_idx, cqm_deq_port is
			   *           matched absolute cqm dequeue port
			   *     if cqm_deq_idx == DEQ_PORT_OFFSET_ALL,
			   *            it is the base of cqm dequeue port
			   * </PRE>
			   */

	int num_deq_ports; /*!< <PRE>output: The number of dequeue port this
			    *    dev have.
			    *    Normally this value should be 1.
			    *    For GPON case, if cqm_deq_port ==
			    *        DEQ_PORT_OFFSET_ALL, then it will be the
			    *    max number of dequeue port. In PRX300, it is
			    *    64.</PRE>
			    */
	int num_q; /*!< output: the number of queues*/
	int q_res_size; /*!< input: to indicate q num can be stored in q_res*/
	struct dp_queue_res *q_res;/*!< output: resource output.
				    *  @note caller should allocate the memory.
				    *  <PRE>Procedure:
				    *  1st call with res NULL to get the num_q;
				    *  then allocate memory: sizeof(*res)*num_q
				    *  2nd call with valid res pointer to get
				    *  real queue information</PRE>
				    */
};

/*! @brief Function deque dp_deq_port_res_get
 * @param [in,out] res struct dp_dequeue_res *res
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_deq_port_res_get(struct dp_dequeue_res *res, int flag);

/*! @brief dp_counter_type */
enum dp_counter_type {
	DP_ENQUEUE_MIB_MODE = 0 /*!< For TMU Enqueue mib mode setting:pkt/bytes
				 *  Not sure about PPv4 yet
				 */
};

/*! @brief dp_counter_conf */
struct dp_counter_conf {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	enum dp_counter_type counter_type; /*!< counter_type to get/set */
	int id;  /*!< depends on mode, it can be queue id/scheduler id/.... */
	int mode; /*!< value to set */
};

/*! @brief Function dp_counter_mode_set
 * @param [in,out] cfg struct dp_counter_conf *cfg
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_counter_mode_set(struct dp_counter_conf *cfg, int flag);

/*! @brief Function dp_counter_mode_get
 * @param [in,out] cfg struct dp_counter_conf *cfg
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed
 *                              otherwise, return DP_FAILURE
 */
int dp_counter_mode_get(struct dp_counter_conf *cfg, int flag);

/*! @brief dp_q_map*/
struct dp_q_map {
	u32	mpe1; /*!< MPE1 Flag: 1 bit*/
	u32	mpe2; /*!< MPE2 Flag:1 bit */
	u32	dp_port; /*!< logical port id: 4 bits*/
	u32	flowid; /*!< FlowId (Bits 7:6): 2 bit */
	u32	subif; /*!< subif*/
	u32	dec; /*!< VPN Decrypt flag: 1 bit*/
	u32	enc; /*!< VPN Encrypt flag: 1 bit*/
	u32	class; /*!< Traffic Class: 4 bits*/
	u32	egflag; /*!< egflag: 1 bits */
};

/*! @brief dp_q_map_mask*/
struct dp_q_map_mask {
	u32	flowid:1; /*!< FlowId don't care */
	u32	dec:1; /*!< DEC Decrypt flag don't care */
	u32	enc:1; /*!< ENC Encrypt flag don't care */
	u32	mpe1:1; /*!< MPE1 Flag don't care */
	u32	mpe2:1; /*!< MPE2 Flag don't care */
	u32	subif:1; /*!< subif don't care */
	u32	subif_id_mask; /*!< subifid mask for
				* CQM qmap lookup setting
				* if the value of one particular bit is 1, it
				* means don't care for this bit,this field is
				* valid only if subif field bit is set
				*/
	u32	dp_port:1; /*!< logical port don't care */
	u32	class:1; /*!< Traffic Class don't care */
	u32	egflag :1; /*!< egflag don't care */
};

/*! @brief queue_map_set*/
struct dp_queue_map_set {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	int q_id; /*!< queue id */
	struct dp_q_map map;  /*!< lookup map value */
	struct dp_q_map_mask mask; /*!< lookup map don't care flag setting:
				    *  1 - means don't care this bit setting
				    */
};

/*! @brief queue_map_entry*/
struct dp_queue_map_entry {
	struct dp_q_map qmap;    /*!< map setting */
};

/*! @brief queue_map_get*/
struct dp_queue_map_get {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	int q_id; /*!<  queue id */
	u32 egflag; /*!< eglag map to different table: for LGM only */
	int num_entry; /*!< output: the number of entries mapped to specified
			*           qid
			*/
	int qmap_size; /*!< number of qmap_entry buffer*/
	struct dp_queue_map_entry *qmap_entry; /*!< caller need to provide
						*   buffer.otherwise, only
						*   return num_entries.
						*/
};

/*! \ingroup APIs_dp_queue_map_set
 * @brief function dp_queue_map_set
 * @param [in,out] cfg struct dp_queue_map_set *cfg
 * @param [in] flag unsigned int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_queue_map_set(struct dp_queue_map_set *cfg, int flag);

/*! \ingroup APIs_dp_queue_map_set
 * @brief function dp_queue_map_get
 * @param [in,out] cfg struct dp_queue_map_get *cfg
 * @param [in] flag unsigned int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_queue_map_get(struct dp_queue_map_get *cfg, int flag);

#define DP_MAX_CHILD_PER_NODE 8 /*!< Maximum child number per node */

/*! @brief dp_node*/
struct dp_node {
	union dp_node_id id; /*!< node ID */
	enum dp_node_type type; /*!< node type */
};

/*! @brief queue_map_get*/
struct dp_node_child {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	union dp_node_id id; /*!< input: node ID */
	enum dp_node_type type; /*!< input: node type: only for sched/port */
	int num; /*!< output: number of child provided in child[] array */
	struct dp_node child[DP_MAX_CHILD_PER_NODE]; /*!< output: child arary*/
};

/*! \ingroup dp_children_get
 * @brief function dp_children_get
 * @param [in,out] cfg struct dp_node_child *cfg
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_children_get(struct dp_node_child *cfg, int flag);

/*! @brief dp_qos_level */
struct dp_qos_level {
	int inst; /* input: dp instance. For SOC side, it is always zero */
	int max_sch_lvl; /* output: max scheduler level */
};

/*! \ingroup dp_qos_level_get
 * @brief function dp_qos_level_get
 * @param [in,out] struct dp_qos_level *dp
 * @param [in] flag int flag
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 */
int dp_qos_level_get(struct dp_qos_level *dp, int flag);

/*! Enumeration for color marking mode for drop precedence selection */
enum dp_col_marking {
	/*!< Mark all to green */
	DP_NO_MARKING,
	/*!< Internal marking derives the color of the packet from internal
	 *   data flow instead of using VLAN tag or DSCP
	 */
	DP_INTERNAL,
	/*!< Drop eligible bit color marking */
	DP_DEI,
	/*!< No drop precedence in the PCP */
	DP_PCP_8P0D,
	/*!< PCP = 4 drop eligible */
	DP_PCP_7P1D,
	/*!< PCP = 4, 2 drop eligible */
	DP_PCP_6P2D,
	/*!< PCP = 4, 2, 0 drop eligible */
	DP_PCP_5P3D,
	/*!< Drop precedence according to RFC 2597 */
	DP_DSCP_AF,
};

/*! Enumeration for selection of single or dual rate color marker */
enum dp_meter_type {
	/*!< RFC2697 color marker */
	srTCM,
	/*!< RFC4115 color marker */
	trTCM,
};

/*! Enumeration for traffic types needed for flow and bridge meters config */
enum dp_meter_traffic_type {
	/*!< All traffic */
	DP_OTHERS,
	/*!< Unicast traffic with no destination address */
	DP_UKNOWN_UNICAST,
	/*!< Upstream multicast traffic */
	DP_MULTICAST,
	/*!< Upstream broadcast traffic */
	DP_BROADCAST,
	/*!< All traffic flows */
	DP_TRAFFIC_TYPE_MAX,
};

/*!
 * @struct dp_meter_cfg
 *
 * @brief This structure defines the exact meter configuration
 *
 */
struct dp_meter_cfg {
	/*!< meter for ingressing traffic */
#define DP_DIR_INGRESS 0
	/*!< meter for egressing traffic */
#define DP_DIR_EGRESS  1
	/*!< Configure color marking only, no meter setup */
#define DP_COL_MARKING			BIT(0)
	/*!< Attach meter to PCE rule e.g., CPU traffic limiting */
#define DP_METER_ATTACH_PCE		BIT(1)
	/*!< Attach meter to CTP port */
#define DP_METER_ATTACH_CTP		BIT(2)
	/*!< Attach meter to bridge port */
#define DP_METER_ATTACH_BRPORT		BIT(3)
	/*!< Attach meter to bridge */
#define DP_METER_ATTACH_BRIDGE		BIT(4)
	/*!< meter ID */
	int meter_id;
	/*!< meter type single/dual rate */
	enum dp_meter_type type;
	/*!< Committed information rate in bit/s */
	u64 cir;
	/*!< Peak information rate in bit/s */
	u64 pir;
	/*!< committed burst size in bytes */
	u32 cbs;
	/*!< peak burst size in bytes */
	u32 pbs;
	/*!< color blind/aware */
	bool col_mode;
	/*!< DP_DIR_INGRESS(0) and DP_DIR_EGRESS(1) */
	int dir;
	 /*!< color marking mode */
	enum dp_col_marking mode;
	 /*!< traffic flow type for bridge/PCE rule mode only */
	union dp_pce {
		enum dp_meter_traffic_type flow;
	} dp_pce;
};

/*!< API dp_meter_alloc: allocate a meter resource
 *
 * @param: [out] meter ID
 * @param: [in]  flag:
 *	   DP_F_DEREGISTER - free an already allocated meter
 *
 */
int dp_meter_alloc(int inst, int *meterid, int flag);

/*!< API dp_meter_add: setup meter/color marking and apply it to CTP/BP/Bridge
 * accordingly
 * This API is used to configure meters and color marking on flows, bridges,
 * CTP and bridge port at ingress/egress direction. The meter configuration
 * requires four traffic parameters CIR/PIR/CBS/PBS. For single rate metering
 * only CIR and CBS are required.
 *
 * @param: [in] dev pointer to netdevice CTP/BP/Bridge. In case of PCE rule
 *		meter this netdevice is used to get the correct device and
 *		PCE table instance.
 * @param: [in] meter meter parameters: rates, color marking
 * @param: [in] flag:
 *	   DP_COL_MARKING		- setup only color marking
 *	   DP_METER_ATTACH_PCE		- setup PCE rule meter e.g., CPU traffic
 *	   DP_METER_ATTACH_CTP		- setup CTP port metering
 *	   DP_METER_ATTACH_BPORT	- setup bridge port metering
 *	   DP_METER_ATTACH_BRIDGE	- setup bridge metering
 *
 */

int dp_meter_add(struct net_device *dev, struct dp_meter_cfg *meter, int flag);

/*!< API dp_meter_del: delete meter
 * This API deletes meter attached to CTP/BP/Bridge.
 * The meter id in the struct dp_meter_cfg has to be a valid one.
 *
 * @param: [in] dev pointer to netdevice CTP/BP/Bridge
 * @param: [in] meter meter parameters: rates, color marking
 * @param: [in] flag:
 *	   DP_METER_ATTACH_PCE		- setup PCE rule meter e.g., CPU traffic
 *	   DP_METER_ATTACH_CTP		- setup CTP port metering
 *	   DP_METER_ATTACH_BPORT	- setup bridge port metering
 *	   DP_METER_ATTACH_BRIDGE	- setup bridge metering
 *
 */
int dp_meter_del(struct net_device *dev, struct dp_meter_cfg *meter, int flag);

/*!
 * @struct dp_tc_cfg
 *
 * Structure defining the mapping of traffic class to CTP.
 *
 */
struct dp_tc_cfg {
	/*!< CTP port netdev */
	struct net_device *dev;
	/*!< Traffic class */
	u8 tc;
	/*!< Force CTP to traffic class */
	bool force;
};

/*!< API dp_ingress_ctp_tc_map_set: force to one traffic class if received
 *  packet from one specified setup CTP.
 *  This used e.g., when CTP port ingress traffic has to be assigned to a
 *  downstream queue.
 *
 * @param [in] mapping of CTP to traffic class
 * @param [in] flag: reserved
 *
 */
int dp_ingress_ctp_tc_map_set(struct dp_tc_cfg *tc, int flag);

/*!
 * @struct dp_qos_cfg_info
 *
 * Structure defining the global QoS config info
 *
 */
struct dp_qos_cfg_info {
	int inst; /*!< input: dp instance. For SOC side, it is always zero */
	u32 quanta; /*!< QoS quanta for scheduler */
	u32 reinsert_deq_port; /*!< dequeue port allocated for reinsertion flow.
							* -1 if invalid dequeue port
							*/
	u32 reinsert_qid; /*!< queue ID allocated for reinsertion flow
					   * -1 if invalid queue ID
					   */
};

/*!< API dp_qos_global_info_get: Helps to retrieve global QoS
 *  Config information.
 *
 * @param [in,out] qos info struct dp_qos_info *info
 * @param [in] flag: reserved
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 *
 */
int dp_qos_global_info_get(struct dp_qos_cfg_info *info, int flag);

/*!
 * @struct dp_port_cfg_info
 *
 * Structure defining the QoS port config info
 *
 */
struct dp_port_cfg_info {
	int inst; /*!< [in] dp instance. For SOC side, it is always zero */
	int pid; /*!< [in] physical qos port id */
	u32 green_threshold; /*!< [in] QoS port Egress green bytes threshold*/
	u32 yellow_threshold; /*!< [in] QoS port Egress yellow bytes threshold*/
	u32 ewsp; /*!< [in] Enhanced EWSP Operation */
#define DP_PORT_CFG_GREEN_THRESHOLD	BIT(0) /*!< [in] config Green thresh */
#define DP_PORT_CFG_YELLOW_THRESHOLD	BIT(1) /*!< [in] config Yellow thresh */
#define DP_PORT_CFG_EWSP		BIT(2) /*!< [in] config EWSP */
};

/*!< API dp_qos_port_conf_set: Helps to retrieve global QoS
 *  Config information.
 *
 * @param [in] port info struct dp_port_cfg_info *info
 * @param [in] flag: reserved
 * @return [out] integer value: return DP_SUCCESS if succeed,
 *                              otherwise, return DP_FAILURE
 *
 */
int dp_qos_port_conf_set(struct dp_port_cfg_info *info, int flag);

/*!
 * @struct dp_qos_q_logic
 *
 * Structure defining the conversion from physical to logical queue ID
 *
 */
struct dp_qos_q_logic {
	int inst;  /*!< [in] DP instance */
	int q_id;  /*!< [in] physical queue id */
	u32 q_logic_id; /*!< [out] q_logical id */
};

/*!< dp_qos_get_q_logic: convert physical queue ID to logical queue ID
 *   @param [in/out] cfg
 *   @param [in] flag: reserved only
 *   @return failure DP_FAILURE
 *           succeed DP_SUCCESS
 */
int dp_qos_get_q_logic(struct dp_qos_q_logic *cfg, int flag);

/*!
 * @struct dp_qos_blk_flush_port
 *
 * Structure to flush Q based on Deq Port
 *
 */
struct dp_qos_blk_flush_port {
	int inst;	 /*!< [in] DP instance */
	u32 dp_port;	 /*!< [in] Logical port id */
	int deq_port_idx;/*!< [in] Flushes Q based  on deq port idx specified */
};

/*!< dp_block_flush_port: Flush all the Q's associated with the deq port
 *   Flushes all the Q in this port and restore Q back to original deq port,
 *   Qmap will be pointing to Drop Q
 *   @param [in/out] cfg
 *   @param [in] flag: reserved only
 *   @return failure DP_FAILURE
 *           succeed DP_SUCCESS
 */
int dp_block_flush_port(struct dp_qos_blk_flush_port *cfg, int flag);

/*!
 * @struct dp_qos_q_logic
 *
 * Structure to flush Q based on specified physical qid
 *
 */
struct dp_qos_blk_flush_queue {
	int inst;  /*!< [in] DP instance */
	int q_id;  /*!< [in] physical queue id */
};

/*!< dp_block_flush_queue: Flush the packets specified in QiD
 *   Flushes the Q and restore Q back to original deq port,
 *   Qmap will be pointing to Drop Q
 *   @param [in/out] cfg
 *   @param [in] flag: reserved only
 *   @return failure DP_FAILURE
 *           succeed DP_SUCCESS
 */
int dp_block_flush_queue(struct dp_qos_blk_flush_queue *cfg, int flag);

/*!
 * @struct dp_qos_queue_info
 *
 * Structure defining the QoS queue info
 *
 */
struct dp_qos_queue_info {
	int inst; /*!< [in] dp instance */
	u32 nodeid; /*!< [in] QoS logical qid */
	u32 qocc; /*!< [out] queue occupancy value */
};

/*!< API dp_qos_get_q_qocc: Helps to retrieve QoS queue occupancy  value
 *
 * This API is only for CQM driver to use during queue flush 
 * There is no any DPM level lock used
 *
 * @param [in,out] qos queue info struct dp_qos_queue_info *info
 * @param [in] flag: reserved
 * @return [out] failure DP_FAILURE
 *		 succeed DP_SUCCESS
 *
 */
int dp_qos_get_q_qocc(struct dp_qos_queue_info *info, int flag);

#ifdef ENABLE_QOS_EXAMPLE
/*! \ingroup APIs_dp_qos_example
 * @brief example1: use basic node_link API to create complex QOS structure
 * @note
 * <PRE> <PRE>
 * Queue       Sched ID: leaf/weight        Sched ID: leaf/weight    Egress Port
 *             |-----------------|    |----------------------|
 * Queue 17----|1  :0/0          |    |2 :0/0                |  Shaper CIR=100
 *             |                 |    |                      |     |
 *             |                 |    |                      |     |
 *             |                 |----|                      |-------- 7
 *             |                 |    |                      |
 *             |                 |    |                      |
 * Queue 18----|---:1/0          |    |                      |
 *             |-----------------|    |----------------------|
 *</PRE></PRE>
 */
static inline int example1(void)
{
	struct dp_node_link info;
	int flag = 0;
	int dp_inst = 0;
	struct dp_shaper_conf shaper;
	struct dp_node_link_enable enable;

	memset(&info, 0, sizeof(info));

	/*connect queue 17 to first scheduler 1*/
	info.inst = dp_inst;
	info.arbi = ARBITRATION_WSP_WRR;
	info.prio_wfq = 0;
	info.node_type = DP_NODE_QUEUE;
	info.node_id.q_id = 17;
	info.p_node_type = DP_NODE_SCH;
	info.p_node_id.sch_id = 1;
	if (dp_node_link_add(&info, 0) == DP_FAILURE)
		return -1;

	/*connect scheduler 1 to first scheduler 2*/
	info.arbi = ARBITRATION_WSP_WRR;
	info.prio_wfq = 0;
	info.node_type = DP_NODE_SCH;
	info.node_id.sch_id = 1;
	info.p_node_type = DP_NODE_SCH;
	info.p_node_id.sch_id = 2;
	if (dp_node_link_add(&info, 0) == DP_FAILURE) {
		/* Note: need to unlink some already added node.
		 * For demo purpose, here skip it
		 */
		return -1;
	}

	/*connect scheduler 2 to egress port 7*/
	info.arbi = ARBITRATION_WSP_WRR;
	info.prio_wfq = 0;
	info.node_type = DP_NODE_SCH;
	info.node_id.sch_id = 2;
	info.p_node_type = DP_NODE_PORT;
	info.p_node_id.cqm_deq_port = 7;
	if (dp_node_link_add(&info, 0) == DP_FAILURE) {
		/* Note: need to unlink some already added node.
		 * For demo purpose, here skip it
		 */
		return -1;
	}

	/*connect queue 18 to first scheduler 1*/
	info.inst = dp_inst;
	info.arbi = ARBITRATION_WSP_WRR;
	info.prio_wfq = 0;
	info.node_type = DP_NODE_QUEUE;
	info.node_id.q_id = 18;
	info.p_node_type = DP_NODE_SCH;
	info.p_node_id.sch_id = 1;
	if (dp_node_link_add(&info, 0) == DP_FAILURE) {
		/* Note: need to unlink some already added node.
		 * For demo purpose, here skip it
		 */
		return -1;
	}

	/* set port shaper*/
	memset(&shaper, 0, sizeof(shaper));
	shaper.type = DP_NODE_SCH;
	shaper.id.sch_id = 2;
	shaper.cir = 100;
	shaper.pir = 100;
	dp_shaper_conf_set(&shaper, flag);

	/*Enable queue/scheduler */
	enable.type = DP_NODE_QUEUE;
	enable.id.q_id = 17;
	enable.en = DP_NODE_EN;
	dp_node_link_en_set(&enable, DP_NODE_AUTO_SMART_ENABLE);
	enable.id.q_id = 18;
	enable.en = DP_NODE_EN;
	dp_node_link_en_set(&enable, DP_NODE_AUTO_SMART_ENABLE);

	return 0;
}

/*! \ingroup APIs_dp_qos_example
 * @brief example2: use smart link API to create complex QOS structure
 * @note
 * <PRE> <PRE>
 * Queue       Sched ID: leaf/weight        Sched ID: leaf/weight    Egress Port
 *             |-----------------|    |----------------------|
 * Queue 17----|1  :0/0          |    |2 :0/0                |  Shaper CIR=100
 *             |                 |    |                      |     |
 *             |                 |    |                      |     |
 *             |                 |----|                      |-------- 7
 *             |                 |    |                      |
 *             |                 |    |                      |
 * Queue 18----|---:1/0          |    |                      |
 *             |-----------------|    |----------------------|
 *</PRE></PRE>
 */
static inline int example2(void)
{
	/* Method 2 with basic link node API dp_link_add from scratch*/
	int flag = DP_NODE_AUTO_SMART_ENABLE;
	int dp_inst = 0;
	struct dp_shaper_conf shaper;
	struct dp_qos_link full_link;
	int dp_port = 3;

	memset(&full_link, 0, sizeof(full_link));
	full_link.inst = dp_inst;
	full_link.cqm_deq_port = 7;
	full_link.q_id = 1;
	full_link.q_arbi = ARBITRATION_WSP_WRR;
	full_link.q_prio_wfq = 0;
	full_link.n_sch_lvl = 2;
	full_link.sch[0].id = 1;
	full_link.sch[0].arbi = ARBITRATION_WSP_WRR;
	full_link.sch[0].prio_wfq = 0;
	full_link.sch[1].id = 2;
	full_link.sch[1].arbi = ARBITRATION_WSP_WRR;
	full_link.sch[1].prio_wfq = 0;
	full_link.dp_port = dp_port;
	if (dp_link_add(&full_link, flag) == DP_FAILURE) {
		pr_err("dp_link_add fail\n");
		return -1;
	}
	full_link.q_id = 2;
	if (dp_link_add(&full_link, flag) == DP_FAILURE) {
		pr_err("dp_link_add fail\n");
		return -1;
	}

	/*set port shaper*/
	memset(&shaper, 0, sizeof(shaper));
	shaper.type = DP_NODE_SCH;
	shaper.id.sch_id = 2;
	shaper.cir = 100;
	shaper.pir = 100;
	dp_shaper_conf_set(&shaper, flag);

	return 0;
}

/*! \ingroup APIs_dp_qos_example
 * @brief gpon_example3:OMCI GEM port and one PMAPPER with basic QOS
 * <PRE><PRE>
 * Queue       Sched ID: leaf/weight         Egress Port
 *             |----------------------|
 * Queue xx----|x  :0/0               |
 *             |                      |
 *             |                      |
 *             |                      |---- ----- 26
 *             |                      |
 *             |                      |
 *             |----------------------|
 *
 * Queue yy----|y  :0/0               |
 *             |                      |
 *             |                      |
 *             |                      |---- ----- 27
 *             |                      |
 *             |                      |
 *             |----------------------|
 * </PRE></PRE>
 */
static inline int gpon_example3(void)
{
	int inst = 0;
	struct module *owner = NULL;
	u32 dev_port = 0;
	s32 dp_port = 0;
	struct net_device *omci_dev = NULL;
	struct net_device *pmapper_dev = NULL;
	struct dp_port_data port_data = {0};
	u32 flags = 0;
	dp_cb_t cb;
	dp_subif_t subif_id;
	struct dp_subif_data subif_data;
	s32 res;

	/*Allocate dp_port and reserve QOS resource if needed.
	 *Note: must set moudle owner properly.
	 */
	flags = DP_F_GPON;
	port_data.flag_ops = DP_F_DATA_RESV_Q | DP_F_DATA_RESV_SCH;
	port_data.num_resv_q = 64;
	port_data.num_resv_sched = 64;
	dp_port = dp_alloc_port_ext(inst, owner, omci_dev, dev_port,
				    dp_port, NULL, &port_data, flags);
	if (dp_port == DP_FAILURE) {
		pr_err("Fail to allocate port\n");
		return -1;
	}

	/*it needs to register device, ie, cb properly.
	 * At least, it needs set below callbacks:
	 * 1) rx_fn
	 * 2) get_subifid_fn: mainly for cpu tx path to fill in proper CTP, esp
	 *    for pmapper related cases
	 */
	flags = 0;
	res = dp_register_dev_ext(inst, owner, dp_port,
				  &cb, NULL, flags);
	if (res == DP_FAILURE) {
		pr_err("Fail to register dev\n");
		dp_alloc_port_ext(inst, owner, omci_dev, dev_port,
				  dp_port, NULL, &port_data, DP_F_DEREGISTER);
		return -1;
	}

	/* Register OMCI Dev
	 * First it needs to create omci dev and store in omci_dev
	 * Once it is ready, call below dp_register_subif_ext API.
	 * After dp_register_subif_ext, below QOS structure will be created
	 * Queue       Sched ID: leaf/weight         Egress Port
	 *             |----------------------|
	 * Queue xx----|1  :0/0               |
	 *             |                      |
	 *             |                      |
	 *             |                      |---- ----- 26
	 *             |                      |
	 *             |                      |
	 *             |                      |
	 *             |----------------------|
	 */
	flags = 0;
	subif_id.inst = inst;
	subif_id.port_id = dp_port;
	subif_id.subif_num = 1;
	subif_id.subif = 0; /*OCMI CTP */
	subif_data.deq_port_idx = 0;
	res = dp_register_subif_ext(inst, owner, omci_dev, omci_dev->name,
				    &subif_id, &subif_data, flags);
	if (res == DP_FAILURE) {
		pr_err("Fail to regisster subif port\n");
		dp_register_dev_ext(inst, owner, dp_port,
				    NULL, NULL, DP_F_DEREGISTER);
		dp_alloc_port_ext(inst, owner, omci_dev, dev_port,
				  dp_port, NULL,
				  &port_data, DP_F_DEREGISTER);
		return -1;
	}

	/* Register pmapper device
	 * First it needs to create new Dev and store in pmapper_dev
	 * Once it is ready, call below dp_register_subif_ext API.
	 * After dp_register_subif_ext, below QOS structure will be created:
	 * Queue       Sched ID: leaf/weight         Egress Port
	 *             |----------------------|
	 * Queue xx----|x  :0/0               |
	 *             |                      |
	 *             |                      |
	 *             |                      |---- ----- 26
	 *             |                      |
	 *             |                      |
	 *             |----------------------|

	 * Queue yy----|y  :0/0               |
	 *             |                      |
	 *             |                      |
	 *             |                      |---- ----- 27
	 *             |                      |
	 *             |                      |
	 *             |----------------------|
	 *
	 */
	subif_id.subif = 1; /*2nd CTP for one pmapper*/
	subif_data.flag_ops = DP_SUBIF_Q_PER_CTP | DP_SUBIF_PCP;
	subif_data.deq_port_idx = 1;
	subif_data.pcp = 0;
	res = dp_register_subif_ext(inst, owner, pmapper_dev, pmapper_dev->name,
				    &subif_id, &subif_data, flags);
	if (res == DP_FAILURE) {
		pr_err("Fail to regisster subif port\n");
		return -1;
	}

	/* Register 2nd CTP based on same Dev and same pmapper
	 * After dp_register_subif_ext, below QOS structure will be created:
	 * Queue       Sched ID: leaf/weight         Egress Port
	 *             |----------------------|
	 * Queue xx----|x  :0/0               |
	 *             |                      |
	 *             |                      |
	 *             |                      |---- ----- 26
	 *             |                      |
	 *             |                      |
	 *             |----------------------|

	 * Queue yy----|y  :0/0               |
	 *             |                      |
	 *             |                      |
	 *             |                      |---- ----- 27
	 * Queue zz----|    1/0               |
	 *             |                      |
	 *             |----------------------|
	 */

	subif_id.subif = 2; /*1st Data CTP */
	/* DP_SUBIF_Q_PER_CTP bit used to create a new queue.
	 * Otherwise, it will share same queue yy
	 */
	subif_data.flag_ops = DP_SUBIF_Q_PER_CTP | DP_SUBIF_PCP;
	subif_data.deq_port_idx = 1;
	subif_data.pcp = 1;
	res = dp_register_subif_ext(inst, owner, omci_dev, omci_dev->name,
				    &subif_id, &subif_data, flags);
	if (res == DP_FAILURE) {
		pr_err("Fail to regisster subif port\n");
		return -1;
	}

	return 0;
}

/*! @note below API not need to provide:
 * 1) dp_get_dc_umt_pid:only valid for GRX500, will be merged into existing API
 *    dp_get_netif_subifid
 * 2) dp_queue_enable(flush): no need to export since will be integraded inside
 *    DP API, like dp_node_del
 * 3) dp_reserved_resource_get: no hardcoded reservation will be take. No need
 */
#endif
#endif /* CONFIG_INTEL_DATAPATH_HAL_GSWIP30 */
#endif

