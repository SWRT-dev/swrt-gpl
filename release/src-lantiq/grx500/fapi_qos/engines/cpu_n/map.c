/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qosal_inc.h"
#include "cpu_n_eng.h"

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

extern char *IPT;

/* =============================================================================
 *  Function Name : isQueueDefault
 *  Description   : Checks if given queues is default
 * ============================================================================*/
uint32_t isQueueDefault(qos_queue_cfg_t* q)
{
	//if (q->priority == CPU_N_MAX_Q)
	//if (!strcmp(q->name, "def_queue"))
	if (q->flags & QOS_Q_F_DEFAULT)
		return QOSAL_TRUE;
	else
		return QOSAL_FALSE;
}

int32_t ingress_ipt_config(iftype_t iftype, uint32_t order, qos_class_cfg_t *clcfg, int32_t flags)
{
	int32_t ret = 0;
	ret = iptebtAdapt(iftype, order, clcfg, flags);
	return ret;
}

int32_t def_dscp_map_init(void)
{
	uint8_t class[64];
	int32_t j=0,k=0;
	char cmd[MAX_DATA_LEN],cmd_buf[MAX_DATA_LEN];
	char policy[MAX_DATA_LEN] = "";
	struct blob_buf b = {};
	uint32_t rule_idx = 0;
	int32_t ret = LTQ_SUCCESS;

	ret = blob_buf_init(&b, 0);

	IPT = IPV4T; /* by default, use iptables. */
	ENTRY

	if (ret != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message Init Failed\n");
		return ret;
	}

	snprintf(policy, MAX_DATA_LEN, "-P %s", "RETURN");
	set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_CREAT, "DEF_DSCP_MAP", -1, policy, &b, rule_idx++);

	/* hook up this chain to beginning of FORWARD chain in filter table. */
	snprintf(cmd, sizeof(cmd), "--%s %d/%d -j %s", mark, CLASSIFICATION_NOT_DONE_MASK, CLASSIFICATION_NOT_DONE_MASK, "DEF_DSCP_MAP");
	set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, RULE_INS, EBT_QOS_STD_HOOK, -1, cmd, &b, rule_idx++);

	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_CREAT, "DEF_DSCP_MAP", -1, NULL, &b, rule_idx++);

	/* hook up this chain to beginning of FORWARD chain in filter table. */
	snprintf(cmd, sizeof(cmd), "-m %s --mark %d/%d -j %s", mark, CLASSIFICATION_NOT_DONE_MASK, CLASSIFICATION_NOT_DONE_MASK, "DEF_DSCP_MAP");
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_INS, IPT_QOS_STD_HOOK, -1, cmd, &b, rule_idx++);

	memset_s(cmd_buf, sizeof(cmd_buf), 0);
	for(j=0;j<64;j++)
	{
		if((j>-1) && (j<8))
				class[j] = 7;
		else if((j>=8) && (j<16))
			class[j] = 6;
		else if((j>=16) && (j<24))
			class[j] = 5;
		else if((j>=24) && (j<32))
			class[j] = 4;
		else if((j>=32) && (j<40))
			class[j] = 3;
		else if((j>=40) && (j<48))
			class[j] = 2;
		else if((j>=48) && (j<56))
			class[j] = 1;
		else if((j>=56) && (j<64))
			class[j] = 0;
		k = j << 2;

		snprintf(cmd_buf, sizeof(cmd_buf), "-p IPV4 --ip-tos 0x%x -j %s --set-%s 0x%x --%s-target CONTINUE",k,mark,mark,(((DEFAULT_TC - class[j]) << QUEPRIO_START_BIT_POS) | (DEFAULT_TC - class[j])),mark);
		set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, RULE_APP, "DEF_DSCP_MAP", -1, cmd_buf, &b, rule_idx++);
		memset_s(cmd_buf, sizeof(cmd_buf), 0);
		snprintf(cmd_buf, sizeof(cmd_buf), "-m dscp --dscp %d -j %s --set-mark 0x%x/0x%x",j,MARK,(((DEFAULT_TC - class[j]) << QUEPRIO_START_BIT_POS) | (DEFAULT_TC - class[j])), Q_PRIO_TC_DEF_DSCP_MASK);
		set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, "DEF_DSCP_MAP", -1, cmd_buf, &b, rule_idx++);
	}

	send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_INIT);

	EXIT
	return LTQ_SUCCESS;
}

int32_t def_dscp_map_fini(void)
{
	char cmd[MAX_DATA_LEN];
	struct blob_buf b = {};
	uint32_t rule_idx = 0;
	int32_t ret = LTQ_SUCCESS;

	ret = blob_buf_init(&b, 0);

	IPT = IPV4T; /* by default, use iptables. */
	ENTRY

	if (ret != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message Init Failed\n");
		return ret;
	}
	snprintf(cmd, sizeof(cmd), "-j DEF_DSCP_MAP");
	set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, RULE_DEL, EBT_QOS_STD_HOOK, -1, cmd, &b, rule_idx++);
	set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_REM, "DEF_DSCP_MAP", -1, NULL, &b, rule_idx++);

	snprintf(cmd, sizeof(cmd), "-m %s --mark %d/%d -j %s", mark, CLASSIFICATION_NOT_DONE_MASK, CLASSIFICATION_NOT_DONE_MASK, "DEF_DSCP_MAP");
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_DEL, IPT_QOS_STD_HOOK, -1, cmd, &b, rule_idx++);
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_FL, "DEF_DSCP_MAP", -1, NULL, &b, rule_idx++);
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_REM, "DEF_DSCP_MAP", -1, NULL, &b, rule_idx++);

	send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_INIT);

	EXIT
	return LTQ_SUCCESS;
}

int32_t tc_q_map_init(void)
{
	char cmd[MAX_DATA_LEN];
	struct blob_buf b = {};
	uint32_t rule_idx = 0;
	int32_t ret = LTQ_SUCCESS;

	ret = blob_buf_init(&b, 0);

	IPT = IPV4T; /* by default, use iptables. */

	ENTRY
	if (ret != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message Init Failed\n");
		return ret;
	}

	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_CREAT, "TC_Q_MAP", -1, NULL, &b, rule_idx++);
	snprintf(cmd, sizeof(cmd), "-m %s --mark %d/%d -j TC_Q_MAP", mark, (1 << PPA_START_BIT_POS), (1 << PPA_START_BIT_POS));
	/* hook up this chain to beginning of PREROUTING chain in mangle table. */
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_INS, "POSTROUTING", -1, cmd, &b, rule_idx++);

	send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_INIT);

	EXIT
	return LTQ_SUCCESS;
}

int32_t tc_q_map_fini(void)
{
	char cmd[MAX_DATA_LEN];
	char rootChName[] = "TC_Q_MAP";
	struct blob_buf b = {};
	uint32_t rule_idx = 0;
	int32_t ret = LTQ_SUCCESS;

	ret = blob_buf_init(&b, 0);

	IPT = IPV4T; /* by default, use iptables. */

	ENTRY

	if (ret != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message Init Failed\n");
		return ret;
	}
	/* flush map chain. */
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_FL, rootChName, -1, NULL, &b, rule_idx++);
	/* remove map chain association with parent chain. */
	snprintf(cmd, sizeof(cmd), "-m %s --mark %d/%d -j %s", mark, (1 << PPA_START_BIT_POS), (1 << PPA_START_BIT_POS), rootChName);
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_DEL, "POSTROUTING", -1, cmd, &b, rule_idx++);
	/* remove map chain. */
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_REM, rootChName, -1, NULL, &b, rule_idx++);

	send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_INIT);

	EXIT
	return LTQ_SUCCESS;
}

int32_t tc_q_map_add(char *ifname, iftype_t iftype, qos_queue_cfg_t *q)
{
	int32_t i = 0, tc = 0, q_prio = 0, q_type = 0;
	int32_t ds_mark = 0;
	char rootChName[MAX_NAME_LEN], q_chName[MAX_NAME_LEN], cmd[MAX_DATA_LEN];
	char *substr = NULL;
	struct blob_buf b = {};
	uint32_t rule_idx = 0;
	iptebt_ops_t act;
	int32_t ret = LTQ_SUCCESS;

	ret = blob_buf_init(&b, 0);

	ENTRY
	UNUSED(iftype);
	UNUSED(q_type);
	if (ret != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message Init Failed\n");
		return ret;
	}
	if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS))
		snprintf(rootChName, sizeof(rootChName), "%s_ING_MAP", ifname);
	else
		snprintf(rootChName, sizeof(rootChName), "%s_MAP", ifname);
	if(strstr_s(q->name, MAX_NAME_LEN, ifname, strnlen_s(ifname, MAX_NAME_LEN), &substr) == EOK) {
		if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS))
			snprintf(q_chName, sizeof(q_chName) - 1, "ing_%s", q->name);
		else
			snprintf(q_chName, sizeof(q_chName) - 1, "%s", q->name);
	}else {
		if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS))
			snprintf(q_chName, sizeof(q_chName) - 1, "%s_ing_%s", ifname, q->name);
		else
			snprintf(q_chName, sizeof(q_chName) - 1, "%s_%s", ifname, q->name);

	}

	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_CREAT, rootChName, -1, NULL, &b, rule_idx++);

	if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS)) {
		ds_mark |= 1 << INGRESS_EGRESS_QOS_BIT;
		snprintf(cmd, sizeof(cmd), "-o %s -m %s --mark %d/%d -j %s", ifname, mark, ds_mark, ds_mark, rootChName);
		set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_INS, "TC_Q_MAP", -1, cmd, &b, rule_idx++);
	} else {
		snprintf(cmd, sizeof(cmd), "-o %s -j %s", ifname, rootChName);
		set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, "TC_Q_MAP", -1, cmd, &b, rule_idx++);
	}

	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_CREAT, q_chName, -1, NULL, &b, rule_idx++);
	if (isQueueDefault(q)) act = RULE_APP;
	else act = RULE_INS;
	snprintf(cmd, sizeof(cmd), "-j %s", q_chName);
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, act, rootChName, -1, cmd, &b, rule_idx++);

	q_prio = 0; /*mark tc in both tc and prio bits in extmark*/
	if (isQueueDefault(q)) {
		while ((i < MAX_TC_NUM) && (q->tc_map[i])) {
			if(q->tc_map[i] <= DEFAULT_TC)
				tc = (DEFAULT_TC + 1 - q->tc_map[i]) << TC_START_BIT_POS;
			else
				tc = (MAX_TC_NUM + DEFAULT_TC + 1 - q->tc_map[i]) << TC_START_BIT_POS;
			q_prio = tc << QUEPRIO_START_BIT_POS; /*copy TC to Prio and mark both in extmark*/
			if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS)) {
				ds_mark = 0;
				ds_mark |= 1 << INGRESS_DSQOS_BIT; 
				snprintf(cmd, sizeof(cmd), "-j %s --set-mark %d/%d", MARK, ds_mark, ds_mark);
			} else if (q->flags & QOS_Q_F_DS_QOS) {
				snprintf(cmd, sizeof(cmd), "-j %s --set-mark %d/%d", MARK, q_prio | tc | DS_QOS_MPE_MARK, Q_PRIO_TC_MASK | DS_QOS_MPE_MARK | VLAN_PRIO_BIT_MASK);
			} else
				snprintf(cmd, sizeof(cmd), "-j %s --set-mark %d/%d", MARK, q_prio | tc, Q_PRIO_TC_MASK | VLAN_PRIO_BIT_MASK | (1 << PPA_START_BIT_POS));
			set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, q_chName, -1, cmd, &b, rule_idx++);

			i++;
		}
	}
	else {
		while ((i < MAX_TC_NUM) && (q->tc_map[i])) {
			if(q->tc_map[i] <= DEFAULT_TC)
				tc = (DEFAULT_TC + 1 - q->tc_map[i]) << TC_START_BIT_POS;
			else
				tc = (MAX_TC_NUM + DEFAULT_TC + 1 - q->tc_map[i]) << TC_START_BIT_POS;
			q_prio = tc << QUEPRIO_START_BIT_POS; /*copy TC to Prio and mark both in extmark*/
			if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS)) {
				ds_mark = 0;
				ds_mark |= 1 << INGRESS_DSQOS_BIT;
				snprintf(cmd, sizeof(cmd), "-m %s --mark %d/%d -j %s --set-mark %d/%d", mark, (tc << INGRESS_TC_BIT), INGRESS_TC_MASK, MARK, ds_mark, ds_mark);
			} else if (q->flags & QOS_Q_F_DS_QOS) {
				PPA_BIT_ENABLE(tc)
				snprintf(cmd, sizeof(cmd), "-m %s --mark %d/%d -j %s --set-mark %d/%d", mark, tc, TC_MASK | (1 << PPA_START_BIT_POS), MARK, q_prio | tc | DS_QOS_MPE_MARK, Q_PRIO_TC_MASK | DS_QOS_MPE_MARK | VLAN_PRIO_BIT_MASK);
			} else {
				PPA_BIT_ENABLE(tc)
				snprintf(cmd, sizeof(cmd), "-m %s --mark %d/%d -j %s --set-mark %d/%d", mark, tc, TC_MASK | (1 << PPA_START_BIT_POS), MARK, q_prio | tc, Q_PRIO_TC_MASK | VLAN_PRIO_BIT_MASK);
			}
			set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, q_chName, -1, cmd, &b, rule_idx++);
			i++;
		}
	}
	if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS)) {
		snprintf(cmd, sizeof(cmd), "-m %s --mark %d/%d -j ACCEPT", mark, ds_mark, ds_mark);
	} else {
		snprintf(cmd, sizeof(cmd), "-m %s --mark %d/%d -j ACCEPT", mark, q_prio | tc, (Q_PRIO_TC_MASK | (1 << PPA_START_BIT_POS)));
	}
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, q_chName, -1, cmd, &b, rule_idx++);

	send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_MODIFY);

	EXIT
	return LTQ_SUCCESS;
}

int32_t tc_q_map_delete(char *ifname, iftype_t iftype, qos_queue_cfg_t *q)
{
	int32_t ds_mark = 0;
	char rootChName[MAX_NAME_LEN], q_chName[MAX_NAME_LEN], cmd[MAX_DATA_LEN];
	char *substr = NULL;
	qos_queue_cfg_t *tmp_q = NULL;
	uint32_t ing_q_num = 0, egr_q_num = 0;
	struct blob_buf b = {};
	uint32_t rule_idx = 0;
	int32_t ret = LTQ_SUCCESS;

	ret = blob_buf_init(&b, 0);

	ENTRY

	if (ret != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message Init Failed\n");
		return ret;
	}

	UNUSED(iftype);
	UNUSED(ds_mark);
	
	if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS)) 
		snprintf(rootChName, sizeof(rootChName), "%s_ING_MAP", ifname);
	else
		snprintf(rootChName, sizeof(rootChName), "%s_MAP", ifname);
	
	if(strstr_s(q->name, MAX_NAME_LEN, ifname, strnlen_s(ifname, MAX_NAME_LEN), &substr) == EOK) {
		if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS))
			snprintf(q_chName, sizeof(q_chName) - 1, "ing_%s", q->name);
		else
			snprintf(q_chName, sizeof(q_chName) - 1, "%s", q->name);
	}else {
		if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS))
			snprintf(q_chName, sizeof(q_chName) - 1, "%s_ing_%s", ifname, q->name);
		else
			snprintf(q_chName, sizeof(q_chName) - 1, "%s_%s", ifname, q->name);
	}

	/* flush map chain. */
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_FL, q_chName, -1, NULL, &b, rule_idx++);
	/* remove map chain association with parent chain. */
	snprintf(cmd, sizeof(cmd), "-j %s", q_chName);
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_DEL, rootChName, -1, cmd, &b, rule_idx++);
	/* remove map chain. */
	set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_REM, q_chName, -1, NULL, &b, rule_idx++);

	qosd_queue_get(ifname, NULL, &egr_q_num, &tmp_q, 0);
	qosd_queue_get(ifname, NULL, &ing_q_num, &tmp_q, QOS_OP_F_INGRESS);
	LOGF_LOG_DEBUG("Egress Queue Num:[%d] Ingress Queue Num:[%d]\n", egr_q_num,ing_q_num);
	if (egr_q_num + ing_q_num <= 1) {
		/* current delete is for the last queue which is still in es tables. */
		/* flush map chain. */
		set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_FL, rootChName, -1, NULL, &b, rule_idx++);
		/* remove map chain association with parent chain. */
		if ((q->flags & QOS_Q_F_INGRESS) && (q->flags & QOS_Q_F_DS_QOS)) {
			ds_mark |= 1 << INGRESS_EGRESS_QOS_BIT;
			snprintf(cmd, sizeof(cmd), "-m %s --mark %d/%d -o %s -j %s", mark, ds_mark, ds_mark, ifname, rootChName);
		} else	
			snprintf(cmd, sizeof(cmd), "-o %s -j %s", ifname, rootChName);
	
		set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_DEL, "TC_Q_MAP", -1, cmd, &b, rule_idx++);
		/* remove map chain. */
		set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_REM, rootChName, -1, NULL, &b, rule_idx++);
	}

	send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_MODIFY);

	EXIT
	return LTQ_SUCCESS;
}
