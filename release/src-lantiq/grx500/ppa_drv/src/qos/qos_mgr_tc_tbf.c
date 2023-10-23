/******************************************************************************
 *
 * Copyright (c) 2019 - 2020 Intel Corporation
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 *****************************************************************************/

#include "qos_mgr_tc.h"
#include "qos_mgr_tc_tbf.h"

int32_t qos_mgr_add_shaper(
		struct net_device *dev,
		uint32_t parentid,
		uint32_t q_id,
		int32_t shaper_index,
		QOS_QUEUE_LIST_ITEM *p_item,
		int32_t flags)
{
	int32_t ret = QOS_MGR_FAILURE;
	int32_t i = QOS_MGR_FAILURE;
	QOS_MGR_CMD_RATE_INFO rate_info = {0};
	QOS_SHAPER_LIST_ITEM *p_s_item = NULL;
	struct _tc_qos_mgr_db *db_handle = NULL;
	struct tc_tbf_qopt_offload *tbf = NULL;

	ret = qos_mgr_get_db_handle(dev, 1, &db_handle);
	if (ret == QOS_MGR_FAILURE)
		return QOS_MGR_FAILURE;

	for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
		if (db_handle->qos_info.q_info[i].p_handle == parentid) {
			tbf = &db_handle->qos_info.q_info[i].tbf;
			break;
		}
	}
	if (!tbf) {
		pr_err("tbf db handle not present\n");
		return QOS_MGR_FAILURE;
	}
	rate_info.shaperid = shaper_index;
	strncpy(rate_info.ifname, dev->name, strlen(dev->name));
	rate_info.shaper.cir = div_u64(tbf->set_params.rate, 1000);
	rate_info.shaper.pir = div_u64(tbf->set_params.prate, 1000);
	rate_info.shaper.enable = 1;
	rate_info.flags = flags;

	ret = qosal_add_shaper(&rate_info, &p_s_item);
	if (ret != QOS_MGR_SUCCESS) {
		pr_err("Add shaper failed\n");
		return QOS_MGR_FAILURE;
	}

	if (p_s_item != NULL) {
		if (p_s_item->p_entry < 0)
			return QOS_MGR_FAILURE;

		pr_debug("<%s> Shaper index: [%d]\n",
				__func__, p_s_item->p_entry);

		rate_info.queueid = q_id;
		rate_info.shaperid = shaper_index;

		ret = qosal_set_qos_rate(&rate_info, p_item, p_s_item);
		if (ret != QOS_MGR_SUCCESS) {
			pr_err("Set qos rate failed\n");
			return QOS_MGR_FAILURE;
		}
	}

	pr_debug("<%s> Return Shaper index: [%d]\n",
			__func__, rate_info.shaperid);

	return rate_info.shaperid;
}

int32_t qos_mgr_del_shaper(
		struct net_device *dev,
		uint32_t parentid,
		uint32_t q_id,
		int32_t shaper_id,
		QOS_QUEUE_LIST_ITEM *p_item,
		int32_t flags)
{
	int32_t ret = QOS_MGR_FAILURE;
	int32_t i = QOS_MGR_FAILURE;
	QOS_MGR_CMD_RATE_INFO rate_info = {0};
	QOS_SHAPER_LIST_ITEM *p_s_item = NULL;
	struct _tc_qos_mgr_db *db_handle = NULL;
	struct tc_tbf_qopt_offload *tbf;

	ret = qos_mgr_get_db_handle(dev, 1, &db_handle);
	if (ret == QOS_MGR_FAILURE)
		return QOS_MGR_FAILURE;

	for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
		if (db_handle->qos_info.q_info[i].p_handle == parentid) {
			tbf = &db_handle->qos_info.q_info[i].tbf;
			break;
		}
	}

	strncpy(rate_info.ifname, dev->name, strlen(dev->name));
	rate_info.shaper.enable = 1;
	rate_info.flags = flags;

	ret = qos_mgr_shaper_lookup(shaper_id, rate_info.ifname, &p_s_item);
	if (ret == QOS_MGR_ENOTAVAIL || p_s_item == NULL) {
		pr_err("Shaper not found\n");
		return QOS_MGR_FAILURE;
	}
	if (p_s_item != NULL) {
		if (p_s_item->p_entry < 0)
			return QOS_MGR_FAILURE;

		pr_debug("<%s> Shaper index: [%d]\n",
				__func__, p_s_item->p_entry);

		rate_info.queueid = q_id;
		rate_info.shaperid = shaper_id;

		ret = qosal_reset_qos_rate(&rate_info, p_item, p_s_item);
		if (ret != QOS_MGR_SUCCESS) {
			pr_err("Reset qos rate failed\n");
			return QOS_MGR_FAILURE;
		}
	}

	return ret;
}

static
int32_t qos_mgr_find_valid_parent_handle_store_tbf(
				struct _tc_qos_mgr_db *dev_handle,
				struct tc_tbf_qopt_offload *tbf)
{
	int32_t i = QOS_MGR_FAILURE;

	if (dev_handle == NULL)
		return QOS_MGR_FAILURE;
	if (tbf->parent) {
		for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
			if (tbf->parent == dev_handle->qos_info.q_info[i].p_handle) {
				pr_debug("Found Q handler for index %d\n", i);
				memcpy(&dev_handle->qos_info.q_info[i].tbf,
					tbf,
					sizeof(struct tc_tbf_qopt_offload));
				break;
			}
		}
	}
	if (i == QOS_MGR_MAX_QUEUE_IFACE) {
		pr_debug("<%s> Valid Queue handler not found.\n", __func__);
		return QOS_MGR_FAILURE;
	}
	return i;
}

static
int32_t qos_mgr_find_valid_parent_handle_delete_tbf(
				struct _tc_qos_mgr_db *dev_handle,
				struct tc_tbf_qopt_offload *tbf)
{
	int32_t i = QOS_MGR_FAILURE;

	if (tbf->parent) {
		for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
			pr_debug("tbf->parent:[%x] dev_handle->qos_info.q_info[i].p_handle:[%x]\n",
					tbf->parent, dev_handle->qos_info.q_info[i].p_handle);
			if (tbf->parent == dev_handle->qos_info.q_info[i].p_handle) {
				pr_debug("Found Q handler for index %d\n", i);
				memset(&dev_handle->qos_info.q_info[i].tbf, 0, sizeof(struct tc_tbf_qopt_offload));
				dev_handle->qos_info.q_info[i].shaper_id = -1;
				dev_handle->qos_info.q_info[i].shaper_index = -1;
				break;
			}
		}
	}
	if (i == QOS_MGR_MAX_QUEUE_IFACE) {
		pr_debug("<%s> Valid Queue handler not found.\n", __func__);
		return QOS_MGR_FAILURE;
	}
	return i;
}

int32_t qos_mgr_tbf_offload(
			struct net_device *dev,
			struct tc_tbf_qopt_offload *sch_tbf_off)
{
	int32_t err = QOS_MGR_FAILURE;
	struct _tc_qos_mgr_db *db_handle = NULL;
	struct tc_tbf_qopt_offload *sch_tbf = sch_tbf_off;

	if (sch_tbf == NULL)
		return err;

	switch (sch_tbf->command) {
	case TC_TBF_REPLACE:
		pr_debug("%s: parent %#x handle %#x "
				" rate %llu prate %llu burst: %u pburst: %u\n",
				__func__,
				sch_tbf->parent,
				sch_tbf->handle,
				sch_tbf->set_params.rate,
				sch_tbf->set_params.prate,
				sch_tbf->set_params.burst,
				sch_tbf->set_params.pburst);
		err = qos_mgr_get_db_handle(dev, 1, &db_handle);
		if (err == QOS_MGR_FAILURE)
			return QOS_MGR_FAILURE;

		err = qos_mgr_find_valid_parent_handle_store_tbf(
				db_handle,
				sch_tbf);
		break;
	case TC_TBF_DESTROY:
		pr_debug("%s: destroy parent %#x handle %#x\n",
				__func__,
				sch_tbf->parent, sch_tbf->handle);
		err = qos_mgr_get_db_handle(dev, 1, &db_handle);
		if (err == QOS_MGR_FAILURE)
			return QOS_MGR_FAILURE;

		err = qos_mgr_find_valid_parent_handle_delete_tbf(
				db_handle,
				sch_tbf);
		break;
	default:
		break;
	}

	return err;
}

