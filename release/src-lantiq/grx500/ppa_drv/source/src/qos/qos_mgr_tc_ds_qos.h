/******************************************************************************
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 *****************************************************************************/

#ifndef _QOS_MGR_TC_DS_QOS_
#define _QOS_MGR_TC_DS_QOS_

#include <linux/if.h>

struct _tc_qos_mgr_db;

/**
 *  Configure evlan rules for traffic class assignment based on pbit.
 *  One block for single and double tagged rules is assigned to all UNI CTPS
 *  i.e., unicast, multicast and broadcast
 *
 *  \param[in] dev	qos manager database
 *  \param[in] tcid	Linux class id identified the qdisc queue
 *  \param[in] pbit	Pbit value used for the TC reassignment
 */
int32_t qos_mgr_ev_qassign(struct _tc_qos_mgr_db *dbi,
			  unsigned int tcid, uint8_t pbit);


/**
 *  Remove TC reassignment evlan rules from the UNI CTPs
 *
 *  \param[in] dev	Network device.
 *  \param[in] tcid	Pbit value used for the TC reassignment
 */

int32_t qos_mgr_ev_qunassign(struct _tc_qos_mgr_db *dbi,
			     unsigned int tcid);

#endif
