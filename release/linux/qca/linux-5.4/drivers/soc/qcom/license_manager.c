/*
 * Copyright (c) 2022 Qualcomm Innovation Center, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include "license_manager.h"
#include <linux/qcom_scm.h>
#include <linux/io.h>
#include <linux/device.h>

struct qmi_handle *lm_clnt_hdl;

static struct lm_svc_ctx *lm_svc;

static struct kobject *lm_kobj;

static uint32_t license_file_count = MAX_NUM_OF_LICENSES;

void license_table_creation(uint32_t lic_file_count);
void free_license_table(void);

static unsigned int use_license_from_rootfs = 0;
static unsigned long int get_attest_key = 0;
static unsigned long int get_report = 0;
char qwes_req_buf[MAX_INPUT_FILE_LEN];
char qwes_ext_claim_buf[MAX_INPUT_FILE_LEN];
char provreq_buf_path[MAX_INPUT_FILE_LEN];

#define CFG_MAX_DIG_COUNT	2
#define QWES_ATTEST_RESP_FILE	"/tmp/attest_resp.bin"
#define QWES_M3_KEY_RESP_FILE	"/tmp/m3key.bin"
#define DEVICE_PROV_RESP_FILE	"/tmp/prov_resp.bin"

module_param(use_license_from_rootfs, uint, 0644);
MODULE_PARM_DESC(use_license_from_rootfs, "Use license files from rootfs: 0,1");

static ssize_t update_license_store(struct kobject *kobj, struct kobj_attribute *attr,
			   const char *buf, size_t count)
{
	uint32_t update, ret;
	bool *license_loaded;

	ret = kstrtouint(buf, 10, (unsigned int *)&update);

	if (ret){
		pr_err("Error while parsing the input %d\n", ret);
		return -EINVAL;
	}
	if(update != 1) {
		pr_err("Push 1 to update the licenses\n");
		return -EINVAL;
	}

	if (!lm_svc) {
		pr_err("License Service is not running\n");
		return count;
	}

	license_loaded = &lm_svc->license_loaded;

	if (*license_loaded) {
		pr_err("License files loaded already, Reloading it\n");
		free_license_table();
		*license_loaded = false;
	}

	license_table_creation(license_file_count);
	*license_loaded = true;
	pr_info("Number of license files loaded: %d\n", lm_svc->num_of_license);

	return count;
}

static struct kobj_attribute lm_update_license_attr =
	__ATTR(update_licenses, 0200, NULL, update_license_store);

void license_table_creation(uint32_t lic_file_count)
{
	int i,ret;
	struct license_info **license_list = lm_svc->license_list;
	int *num_of_license_loaded = &lm_svc->num_of_license;

	for (i=0; i < lic_file_count; i++) {
		struct license_info *license;

		license = kzalloc(sizeof(struct license_info), GFP_KERNEL);

		if (!license) {
			ret = -ENOMEM;
			pr_err("Memory allocation for license info failed %d",
								ret);
			continue;
		}

		ret = snprintf(license->path, PATH_MAX, "%s/license_%d%s",
					license_path, i+1, license_extn);
		if (ret >= PATH_MAX) {
			ret = -ENAMETOOLONG;
			pr_err("License file path too long, failed with error code %d\n", ret);
			continue;
		}
		/*Allocate memory to hold the license file*/
		license->order = get_order(QMI_LM_MAX_LICENSE_SIZE_V01);
		license->page = alloc_pages(GFP_KERNEL, license->order);
		if (!license->page) {
			ret = -ENOMEM;
			pr_err("Mem allocation for %s failed with error code %d\n",license->path, ret);
			continue;
		} else {
			/* get the mapped virtual address of the page */
			license->buffer = page_address(license->page);
		}
		memset(license->buffer, 0, QMI_LM_MAX_LICENSE_SIZE_V01);
		ret = kernel_read_file_from_path(license->path,
			&license->buffer, &license->size,
			QMI_LM_MAX_LICENSE_SIZE_V01, READING_FIRMWARE_PREALLOC_BUFFER);
		if (ret) {
			if (ret == -EFBIG){
				pr_err("Loading %s failed size bigger than MAX_LICENSE_SIZE %d\n",license->path, QMI_LM_MAX_LICENSE_SIZE_V01);
			} else if(ret != -ENOENT) {
				pr_err("Loading %s failed with error %d\n", license->path, ret);
			} else {
				pr_debug("Loading %s failed with no such file or directory\n",license->path);
			}
			free_pages((unsigned long)license->buffer, license->order);
			kfree(license);
		} else {
			pr_debug("License file %s of size %lld loaded into buffer 0x%p\n", license->path, license->size, license->buffer);

			license_list[*num_of_license_loaded] = license;
			*num_of_license_loaded = *num_of_license_loaded + 1;
		}
	}
}

void free_license_table(void)
{
	int i;
	struct license_info **license_list = lm_svc->license_list;
	int num_of_license = lm_svc->num_of_license;

	for ( i = 0; i < num_of_license; i++) {
		if(license_list[i]->buffer) {
			free_pages((unsigned long)license_list[i]->buffer,
					license_list[i]->order);
			pr_debug("Freed 0x%p size: %lld\n",
				license_list[i]->buffer, license_list[i]->size);
			kfree(license_list[i]);
		}
	}
	lm_svc->num_of_license = 0;

}

int scm_get_device_provision_response(void *provreq_buf, u32 provreq_buf_size,
		void *provresp_buf, u32 provresp_buf_size , u32 *provresp_size)
{
	struct file *f;
	int flags = O_RDWR | O_CREAT | O_TRUNC;
	loff_t pos = 0;
	int ret;

	ret = qti_scm_get_device_provision_response(QWES_SVC_ID, QWES_DEVICE_PROVISION,
				provreq_buf, provreq_buf_size, provresp_buf,
				provresp_buf_size, provresp_size);
	if (ret) {
		pr_err("%s : device provision scm call failed\n", __func__);
		return ret;
	}

	f = filp_open(DEVICE_PROV_RESP_FILE, flags, 0600);
	if (IS_ERR(f)) {
		pr_err("filp_open(%s) for device provision response"
				" failed\n", DEVICE_PROV_RESP_FILE);
		return ret;
	}
	pr_info(" %s : attest_resp_size is %x\n", __func__, *provresp_size);
	ret = kernel_write(f, provresp_buf, *provresp_size+1, &pos);

	if (ret < 0) {
		pr_debug("Error writing device provision response file: %s\n",
				DEVICE_PROV_RESP_FILE);
	}
	filp_close(f, NULL);
	return ret;

}

int scm_get_device_attestation_response(void *req_buf, u32 req_buf_size,
		void *extclaim_buf, u32 extclaim_buf_size, void *resp_buf,
				u32 resp_buf_size, u32 *attest_resp_size)
{
	struct file *f;
	int flags = O_RDWR | O_CREAT | O_TRUNC;
	loff_t pos = 0;
	int ret;

	ret = qti_scm_get_device_attestation_response(QWES_SVC_ID,
			QWES_ATTEST_REPORT, req_buf, req_buf_size, extclaim_buf,
			extclaim_buf_size, resp_buf, resp_buf_size, attest_resp_size);
	if (ret) {
		pr_err("%s : qwes get attestation respone scm call failed\n", __func__);
		return ret;
	}

	f = filp_open(QWES_ATTEST_RESP_FILE, flags, 0600);
	if (IS_ERR(f)) {
		pr_err("filp_open(%s) for attestation respone"
				" failed\n", QWES_ATTEST_RESP_FILE);
		return ret;
	}
	pr_info(" %s : attest_resp_size is %x\n", __func__, *attest_resp_size);
	ret = kernel_write(f, resp_buf, *attest_resp_size+1, &pos);

	if (ret < 0) {
		pr_debug("Error writing attest response file: %s\n",
						QWES_ATTEST_RESP_FILE);
	}
	filp_close(f, NULL);
	return ret;

}

int scm_get_device_attestation_ephimeral_key(void *key_buf, u32 key_buf_len,
		u32 *key_len)
{
	struct file *f;
	char buf[MAX_SIZE_OF_KEY_BUF_LEN] = {0};
	int flags = O_RDWR | O_CREAT | O_TRUNC;
	loff_t pos = 0;
	u32 index;
	int rc;

	rc = qti_scm_get_device_attestation_ephimeral_key(QWES_SVC_ID,
				QWES_INIT_ATTEST, key_buf, key_buf_len, key_len);
	if (rc) {
		pr_err("%s : qwes init attestation scm failed\n", __func__);
		return rc;
	}

	pr_info(" %s : Key Length is  : %X\n", __func__, *key_len);

	for (index = 0; index < (*key_len - (*key_len % sizeof(u32)));
			index = index + sizeof(u32)) {
		snprintf(buf+strlen(buf), MAX_SIZE_OF_KEY_BUF_LEN - strlen(buf),
				"%08X ", *(unsigned int *)(key_buf + index));
	}
	if ((*key_len % sizeof(u32)) != 0) {
		snprintf(buf+strlen(buf), MAX_SIZE_OF_KEY_BUF_LEN - strlen(buf),
				"%X ", *(unsigned int *)(key_buf +
					(*key_len - (*key_len % sizeof(u32)))));
	}

	f = filp_open(QWES_M3_KEY_RESP_FILE, flags, 0600);
	if (IS_ERR(f)) {
		pr_err("filp_open(%s) for M3 key "
				" failed\n", QWES_M3_KEY_RESP_FILE);
		return rc;
	}
	rc = kernel_write(f, key_buf, *key_len+1, &pos);

	if (rc < 0) {
		pr_debug("Error writing M3 Key file: %s\n", QWES_M3_KEY_RESP_FILE);
	}
	filp_close(f, NULL);
	pr_info("%s : Key in response buffer is : %s\n", __func__, buf);
	return rc;
}

static void qmi_handle_license_termination_mode_req(struct qmi_handle *handle,
			struct sockaddr_qrtr *sq,
			struct qmi_txn *txn,
			const void *decoded_msg)
{
	struct qmi_lm_get_termination_mode_req_msg_v01 *req;
	struct qmi_lm_get_termination_mode_resp_msg_v01 *resp;
	struct client_info *client;
	int ret;
	bool *license_loaded = &lm_svc->license_loaded;
	struct feature_info *itr, *tmp;

	if(*license_loaded == false) {
		license_table_creation(license_file_count);
		*license_loaded = true;
	}

	req = (struct qmi_lm_get_termination_mode_req_msg_v01 *)decoded_msg;

	resp = kzalloc(sizeof(*resp), GFP_KERNEL);
	if (!resp) {
		pr_err("%s: Memory allocation failed for resp buffer\n",
							__func__);
		return;
	}

	client = kzalloc(sizeof(*client), GFP_KERNEL);
	if (!client) {
		pr_err("%s: Memory allocation failed for client ctx\n",
							__func__);
		goto free_resp_mem;
	}

	pr_debug("License termination: Request rcvd: node_id: 0x%x, timestamp: "
			"0x%llx,chip_id: 0x%x,chip_name :%s,serialno:0x%x\n",
			sq->sq_node, req->timestamp, req->id,
			req->name, req->serialnum);

	client->sq_node = sq->sq_node;
	client->sq_port = sq->sq_port;
	list_add_tail(&client->node, &lm_svc->clients_connected);

	if (!list_empty(&lm_svc->clients_feature_list)) {
		list_for_each_entry_safe(itr, tmp, &lm_svc->clients_feature_list,
								node) {
			if (itr->sq_node == sq->sq_node && itr->sq_port == sq->sq_port) {
				list_del(&itr->node);
				kfree(itr);
			}
		}
	}

	resp->termination_mode =  lm_svc->termination_mode;
	resp->reserved = 77;

	ret = qmi_send_response(handle, sq, txn,
			QMI_LM_GET_TERMINATION_MODE_RESP_V01,
			QMI_LM_GET_TERMINATION_MODE_RESP_MSG_V01_MAX_MSG_LEN,
			qmi_lm_get_termination_mode_resp_msg_v01_ei, resp);
	if (ret < 0)
		pr_err("%s: Sending license termination response failed"
					"with error_code:%d\n", __func__, ret);
	else
		pr_debug("License termination: Response sent, license"
			" termination mode 0x%x\n", resp->termination_mode);

free_resp_mem:
	kfree(resp);
}

static ssize_t device_attestation_ephimeral_key_show(struct device_driver *driver,
		char *buff)
{
	return snprintf(buff, CFG_MAX_DIG_COUNT, "%ld", get_attest_key);
}

static ssize_t device_attestation_ephimeral_key_store(struct device_driver *driver,
		const char *buff, size_t count)
{
	void *key_buf = NULL;
	u32 *key_len = NULL;
	int ret = 0;

	if (kstrtoul(buff, 0, &get_attest_key))
		return -EINVAL;

	if (get_attest_key == 1) {
		key_buf = kzalloc(QWES_M3_KEY_BUFF_MAX_SIZE, GFP_KERNEL);
		if (!key_buf) {
			pr_err("%s: Memory allocation failed for key buffer\n",
					__func__);
			return -EINVAL;
		}
		key_len = kzalloc(sizeof(u32), GFP_KERNEL);
		if (!key_len) {
			pr_err("%s: Memory allocation failed for key length\n",
					__func__);
			ret = -EINVAL;
			goto key_buf_alloc_err;
		}

		ret = scm_get_device_attestation_ephimeral_key(key_buf,
				QWES_M3_KEY_BUFF_MAX_SIZE, key_len);
		kfree(key_len);
		ret = count;
	}
	else {
		pr_err("%s : Invalid sysfs entry input\n", __func__);
		return -EINVAL;
	}


key_buf_alloc_err:
	kfree(key_buf);
	return ret;
}
static DRIVER_ATTR_RW(device_attestation_ephimeral_key);

static ssize_t device_attestation_request_buff_show(struct device_driver *driver,
		char *buff)
{
	return snprintf(buff, MAX_INPUT_FILE_LEN, "%s", qwes_req_buf);
}

static ssize_t device_attestation_request_buff_store(struct device_driver *driver,
		const char *buff, size_t count)
{
	if (buff) {
		snprintf(qwes_req_buf, strlen(buff), "%s", buff);
	}
	return count;

}
static DRIVER_ATTR_RW(device_attestation_request_buff);

static ssize_t device_external_claim_buff_show(struct device_driver *driver,
                char *buff)
{
	return  snprintf(buff, MAX_INPUT_FILE_LEN, "%s", qwes_ext_claim_buf);
}

static ssize_t device_external_claim_buff_store(struct device_driver *driver,
                const char *buff, size_t count)
{
	if (buff) {
		snprintf(qwes_ext_claim_buf, strlen(buff), "%s", buff);
	}
	return count;

}
static DRIVER_ATTR_RW(device_external_claim_buff);

static ssize_t device_get_attestation_response_show(struct device_driver *driver,
		char *buff)
{
	return snprintf(buff, CFG_MAX_DIG_COUNT, "%ld", get_report);
}

static ssize_t device_get_attestation_response_store(struct device_driver *driver,
		const char *buff, size_t count)
{
	void *req_buf = NULL;
	void *resp_buf = NULL;
	void *extclaim_buf = NULL;
	void *data = NULL;
	u32 req_buf_size = 0;
	u32 extclaim_buf_size = 0;
	u32 *attest_resp_size = NULL;
	loff_t size = 0;
	int ret = 0;

	if (kstrtoul(buff, 0, &get_report)) {
		return -EINVAL;
	}
	if (get_report == 1) {

		if (strlen(qwes_req_buf) <= 0) {
			return -EINVAL;
		}
		resp_buf = kzalloc(QWES_RESP_BUFF_MAX_SIZE, GFP_KERNEL);
		if (!resp_buf) {
			pr_err("%s: Memory allocation failed for resp buffer\n",
					__func__);
			return -EINVAL;
		}

		attest_resp_size = kzalloc(sizeof(u32), GFP_KERNEL);
		if (!attest_resp_size) {
			pr_err("%s: Memory allocation failed for attest resp"
							"length\n", __func__);
			goto resp_buf_alloc_err;
		}
		if (strlen(qwes_req_buf) > 0) {
			ret = kernel_read_file_from_path(qwes_req_buf, &data,
							&size, 0, READING_POLICY);
			if (ret < 0) {
				pr_err("%s File open failed\n", __func__);
				goto attest_resp_size_alloc_err;
			}
			req_buf_size = size;
			req_buf = kzalloc(req_buf_size, GFP_KERNEL);
			if (!req_buf) {
				pr_err("%s: Memory allocation failed for attest"
						"request buffer\n", __func__);
				vfree(data);
				goto attest_resp_size_alloc_err;
			}
			if (data != NULL) {
				memcpy(req_buf, data, req_buf_size);
			}
			pr_info("%s : req_buf size is %x\n", __func__,
							(unsigned int)size);
			vfree(data);
		}
		if (strlen(qwes_ext_claim_buf) > 0) {
			data = NULL;
			ret = kernel_read_file_from_path(qwes_ext_claim_buf,
						&data, &size, 0, READING_POLICY);
			if (ret < 0) {
				pr_err("%s File open failed\n", __func__);
				goto req_buf_alloc_err;
			}
			extclaim_buf_size = size;
			extclaim_buf = kzalloc(extclaim_buf_size, GFP_KERNEL);
			if (!extclaim_buf) {
				pr_err("%s: Memory allocation failed for"
						"extclaim buffer\n", __func__);
				vfree(data);
				goto req_buf_alloc_err;
			}
			if (data != NULL) {
				memcpy(extclaim_buf, data, extclaim_buf_size);
			}
			pr_info("%s : extclaim_buf size is %x\n", __func__,
							(unsigned int)size);
			vfree(data);
		}
		ret = scm_get_device_attestation_response(req_buf, req_buf_size,
				extclaim_buf, extclaim_buf_size, resp_buf,
				QWES_RESP_BUFF_MAX_SIZE, attest_resp_size);
		if (extclaim_buf != NULL) {
			kfree(extclaim_buf);
		}
		ret = count;
	}
	else {
		pr_err("%s : Invalid sysfs entry input\n", __func__);
		return -EINVAL;
	}

req_buf_alloc_err:
	kfree(req_buf);
attest_resp_size_alloc_err:
	kfree(attest_resp_size);
resp_buf_alloc_err:
	kfree(resp_buf);
	return ret;
}
static DRIVER_ATTR_RW(device_get_attestation_response);


static ssize_t get_device_provision_response_show(struct device_driver *driver,
                char *buff)
{
        return snprintf(buff, MAX_INPUT_FILE_LEN, "%s", provreq_buf_path);
}

static ssize_t get_device_provision_response_store(struct device_driver *driver,
		const char *buff, size_t count)
{
	void *provreq_buf = NULL;
	void *provresp_buf = NULL;
	void *data = NULL;
	u32 *provresp_size = NULL;
	uint32_t provreq_buf_size = 0;
	loff_t size = 0;
	int ret;

	if (buff) {
		snprintf(provreq_buf_path, strlen(buff), "%s", buff);
	}
	if (strlen(provreq_buf_path) <= 0) {
		return -EINVAL;
	}
	else {
		ret = kernel_read_file_from_path(provreq_buf_path, &data,
				&size, 0, READING_POLICY);
		if (ret < 0) {
			pr_err("%s File open failed\n", __func__);
			return -EINVAL;
		}
		provresp_buf = kzalloc(QWES_RESP_BUFF_MAX_SIZE, GFP_KERNEL);
		if (!provresp_buf) {
			pr_err("%s: Memory allocation failed for provresp_buf\n",
					__func__);
			vfree(data);
			return -EINVAL;
		}

		provresp_size = kzalloc(sizeof(u32), GFP_KERNEL);
		if (!provresp_size) {
			pr_err("%s: Memory allocation failed for provresp_buf_"
					"size\n", __func__);
			vfree(data);
			goto provresp_buf_alloc_err;
		}

		provreq_buf_size = size;
		provreq_buf = kzalloc(provreq_buf_size, GFP_KERNEL);
		if (!provreq_buf) {
			pr_err("%s: Memory allocation failed for attest"
					"request buffer\n", __func__);
			vfree(data);
			goto provresp_size_alloc_err;
		}
		if (data != NULL) {
			memcpy(provreq_buf, data, provreq_buf_size);
		}
		pr_info("%s : provreq_buf_size is %x\n", __func__,
				(unsigned int)size);
		vfree(data);

		ret = scm_get_device_provision_response(provreq_buf, provreq_buf_size,
				provresp_buf, QWES_RESP_BUFF_MAX_SIZE, provresp_size);

		kfree(provreq_buf);
	}

provresp_size_alloc_err:
	kfree(provresp_size);

provresp_buf_alloc_err:
	kfree(provresp_buf);
	return count;
}
static DRIVER_ATTR_RW(get_device_provision_response);

static void qmi_handle_license_download_req(struct qmi_handle *handle,
			struct sockaddr_qrtr *sq,
			struct qmi_txn *txn,
			const void *decoded_msg)
{

	struct qmi_lm_download_license_req_msg_v01 *req;
	struct qmi_lm_download_license_resp_msg_v01 *resp;
	struct license_info **license_list = lm_svc->license_list;
	int num_of_license = lm_svc->num_of_license;
	int ret, req_lic_index;

	req = (struct qmi_lm_download_license_req_msg_v01 *)decoded_msg;

	resp = kzalloc(sizeof(*resp), GFP_KERNEL);
	if (!resp) {
		pr_err("%s: Memory allocation failed for resp buffer\n",
							__func__);
		return;
	}

	pr_debug("License download: Request rcvd, node_id: 0x%x, ReservedValue :0x%x,"
			"LicenseIndex %d\n", sq->sq_node, req->reserved,
			req->license_index);

	if (num_of_license <= 0) {
		resp->next_lic_index = 0;
		resp->data_valid = 0;
		resp->reserved = 0xdead;

		goto send_resp;
	}

	if (req->license_index < 0 || (req->license_index >= num_of_license)) {
		pr_err("%s: unexpected license_index in request: %d\n",
					__func__, req->license_index);
		goto free_resp_buf;
	}
	req_lic_index = req->license_index;

	memcpy(resp->data, license_list[req_lic_index]->buffer,
				license_list[req_lic_index]->size);
	resp->data_len = license_list[req_lic_index]->size;
	resp->data_valid = 1;
	resp->reserved = 0xacac;

	if ((req_lic_index+1) < num_of_license)
		resp->next_lic_index = req_lic_index+1;
	else
		resp->next_lic_index = 0;

send_resp:
	ret = qmi_send_response(handle, sq, txn,
			QMI_LM_DOWNLOAD_RESP_V01,
			QMI_LM_DOWNLOAD_LICENSE_RESP_MSG_V01_MAX_MSG_LEN,
			qmi_lm_download_license_resp_msg_v01_ei, resp);
	if (ret < 0)
		pr_err("%s: Sending license termination response failed"
					"with error_code:%d\n",__func__,ret);
	else
		pr_debug("License download: Response sent, license buffer "
			"size :0x%x, valid: %d next_lic_index %d\n",
			resp->data_len, resp->data_valid, resp->next_lic_index);
free_resp_buf:
	kfree(resp);

}

static void qmi_handle_feature_list_req(struct qmi_handle *handle,
			struct sockaddr_qrtr *sq,
			struct qmi_txn *txn,
			const void *decoded_msg)
{
	struct qmi_lm_feature_list_req_msg_v01 *req;
	struct qmi_lm_feature_list_resp_msg_v01 *resp;
	struct feature_info *licensed_features, *itr, *tmp;
	int i, ret;

	req = (struct qmi_lm_feature_list_req_msg_v01 *)decoded_msg;

	pr_debug("Licensed Features: Request rcvd, node_id: 0x%x", sq->sq_node);

	resp = kzalloc(sizeof(*resp), GFP_KERNEL);
	if (!resp) {
		pr_err("%s: Memory allocation failed for resp buffer\n",
							__func__);
		goto free_resp_buf;
	}
	resp->resp.result = QMI_RESULT_FAILURE_V01;

	if(!req->feature_list_valid) {
		pr_err("No Features are licensed in node 0x%x\n",sq->sq_node);
	}

	licensed_features = kzalloc(sizeof(*licensed_features), GFP_KERNEL);
	if (!licensed_features) {
		pr_err("%s: Memory allocation failed for feature list\n",
							__func__);
		goto send_resp;
	}

	licensed_features->sq_node = sq->sq_node;
	licensed_features->sq_port = sq->sq_port;
	licensed_features->reserved = req->reserved;
	if(!req->feature_list_valid) {
		licensed_features->len = 0;
	} else {
		licensed_features->len = req->feature_list_len;
		if(licensed_features->len > QMI_LM_MAX_FEATURE_LIST_V01) {
			pr_err("Feature_list larger than QMI_MAX_FEATURE_LIST_V01"
					"%d, so assiging it to max \n",
					QMI_LM_MAX_FEATURE_LIST_V01);
			licensed_features->len = QMI_LM_MAX_FEATURE_LIST_V01;
		}

		for(i =0; i<licensed_features->len; i++)
			licensed_features->list[i] = req->feature_list[i];

	}

	if (!list_empty(&lm_svc->clients_feature_list)) {
		list_for_each_entry_safe(itr, tmp, &lm_svc->clients_feature_list,
								node) {
			if (itr->sq_node == sq->sq_node && itr->sq_port == sq->sq_port) {
				list_del(&itr->node);
				kfree(itr);
			}
		}
	}

	list_add_tail(&licensed_features->node, &lm_svc->clients_feature_list);
	resp->resp.result = QMI_RESULT_SUCCESS_V01;

send_resp:
	ret = qmi_send_response(handle, sq, txn,
			QMI_LM_FEATURE_LIST_RESP_V01,
			QMI_LM_FEATURE_LIST_RESP_MSG_V01_MAX_MSG_LEN,
			qmi_lm_feature_list_resp_msg_v01_ei, resp);
	if (ret < 0)
		pr_err("%s: Sending license termination response failed"
					"with error_code:%d\n",__func__,ret);
	else
		pr_debug("Licensed Features: Response sent, Result code "
			"%d\n", resp->resp.result);
free_resp_buf:
	kfree(resp);

}

static ssize_t show_licensed_features(struct kobject *k,
				struct kobj_attribute *attr, char *buf)
{
	uint32_t i, len = 0, max_buf_len = PAGE_SIZE;
	struct feature_info *itr, *tmp;

	if (!list_empty(&lm_svc->clients_feature_list)) {
		list_for_each_entry_safe(itr, tmp,
					&lm_svc->clients_feature_list, node) {
			if(itr->len == 0) {
				len += scnprintf(buf+len, max_buf_len-len,
					"\nClient Node:0x%x Port:%d,"
					" No feature licensed\n",itr->sq_node,
					itr->sq_port);
			} else {
				len += scnprintf(buf+len, max_buf_len-len,
					"\nClient Node:0x%x Port:%d,"
					" %d features licensed\n"
					" Feature List:\n", itr->sq_node,
					itr->sq_port, itr->len);
				for(i=0;i<itr->len;i++)
					 len += scnprintf(buf+len,
						max_buf_len-len,
						" %d\n",itr->list[i]);
			}
			len += scnprintf(buf+len, max_buf_len-len,
					"\nAdditional Info: 0x%08x\n",
					itr->reserved);
		}
	} else
		len += scnprintf(buf+len, max_buf_len-len,
			"Client's licensed feature list not available\n");

	return len;
}

static struct kobj_attribute lm_licensed_features_attr =
	__ATTR(licensed_features, 0400, show_licensed_features, NULL);

static void lm_qmi_svc_disconnect_cb(struct qmi_handle *qmi,
	unsigned int node, unsigned int port)
{
	struct client_info *itr, *tmp;

	if (!list_empty(&lm_svc->clients_connected)) {
		list_for_each_entry_safe(itr, tmp, &lm_svc->clients_connected,
								node) {
			if (itr->sq_node == node && itr->sq_port == port) {
				pr_info("Received LM QMI client disconnect "
					"from node:0x%x port:%d\n",
					node, port);
				list_del(&itr->node);
				kfree(itr);
			}
		}
	}
}

static struct qmi_ops lm_server_ops = {
	.del_client = lm_qmi_svc_disconnect_cb,
};
static struct qmi_msg_handler lm_req_handlers[] = {
	{
		.type = QMI_REQUEST,
		.msg_id = QMI_LM_GET_TERMINATION_MODE_REQ_V01,
		.ei = qmi_lm_get_termination_mode_req_msg_v01_ei,
		.decoded_size = QMI_LM_GET_TERMINATION_MODE_REQ_MSG_V01_MAX_MSG_LEN,
		.fn = qmi_handle_license_termination_mode_req,
	},
	{
		.type = QMI_REQUEST,
		.msg_id = QMI_LM_DOWNLOAD_REQ_V01,
		.ei = qmi_lm_download_license_req_msg_v01_ei,
		.decoded_size = QMI_LM_DOWNLOAD_LICENSE_REQ_MSG_V01_MAX_MSG_LEN,
		.fn = qmi_handle_license_download_req,
	},
	{
		.type = QMI_REQUEST,
		.msg_id = QMI_LM_FEATURE_LIST_REQ_V01,
		.ei = qmi_lm_feature_list_req_msg_v01_ei,
		.decoded_size = QMI_LM_FEATURE_LIST_REQ_MSG_V01_MAX_MSG_LEN,
		.fn = qmi_handle_feature_list_req,
	},
	{}
};

static int license_manager_probe(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	bool host_license_termination = false;
	bool device_license_termination = false;
	int ret = 0;
	struct lm_svc_ctx *svc;

	svc = kzalloc(sizeof(struct lm_svc_ctx), GFP_KERNEL);
	if (!svc)
		return -ENOMEM;

	ret = driver_create_file(pdev->dev.driver,
			&driver_attr_device_attestation_ephimeral_key);
	if (ret) {
		dev_err(&pdev->dev, "failed to create sysfs entry\n");
		return ret;
	}
	ret = driver_create_file(pdev->dev.driver,
			&driver_attr_device_attestation_request_buff);
	if (ret) {
		dev_err(&pdev->dev, "failed to create sysfs entry\n");
		return ret;
	}
	ret = driver_create_file(pdev->dev.driver,
			&driver_attr_device_external_claim_buff);
	if (ret) {
		dev_err(&pdev->dev, "failed to create sysfs entry\n");
		return ret;
	}
	ret = driver_create_file(pdev->dev.driver,
			&driver_attr_device_get_attestation_response);
	if (ret) {
		dev_err(&pdev->dev, "failed to create sysfs entry\n");
		return ret;
	}

	ret = driver_create_file(pdev->dev.driver,
			&driver_attr_get_device_provision_response);
	if (ret) {
		dev_err(&pdev->dev, "failed to create sysfs entry\n");
		return ret;
	}

	device_license_termination = of_property_read_bool(node,
						"device-license-termination");
	pr_debug("device-license-termination : %s \n",
				device_license_termination ? "true" : "false");

	if (device_license_termination)
		goto skip_device_mode;

	host_license_termination = of_property_read_bool(node,
						"host-license-termination");
	pr_debug("host-license-termination : %s \n",
				host_license_termination ? "true" : "false");

	if (!host_license_termination) {
		pr_debug("License Termination mode not given in DT,"
			"Assuming Device license termination by default \n");
		device_license_termination = true;
	}

skip_device_mode:
	svc->termination_mode =  host_license_termination ?
			QMI_LICENSE_TERMINATION_AT_HOST_V01 :
				QMI_LICENSE_TERMINATION_AT_DEVICE_V01;

	svc->license_list = (struct license_info**)
				kzalloc(sizeof(struct license_info*) *
				MAX_NUM_OF_LICENSES, GFP_KERNEL);

	if (!svc->license_list) {
		ret = -ENOMEM;
		pr_err("%s:Mem allocation failed for license_list %d\n",
							__func__, ret);
		goto free_lm_svc;
	}

	svc->license_loaded = false;

	svc->lm_svc_hdl = kzalloc(sizeof(struct qmi_handle), GFP_KERNEL);
	if (!svc->lm_svc_hdl) {
		ret = -ENOMEM;
		pr_err("%s:Mem allocation failed for LM svc handle %d\n",
							__func__, ret);
		goto free_lm_license_list;
	}
	ret = qmi_handle_init(svc->lm_svc_hdl,
				QMI_LICENSE_MANAGER_SERVICE_MAX_MSG_LEN,
				&lm_server_ops,
				lm_req_handlers);
	if (ret < 0) {
		pr_err("%s:Error registering license manager svc %d\n",
							__func__, ret);
		goto free_lm_svc_handle;
	}
	ret = qmi_add_server(svc->lm_svc_hdl, QMI_LM_SERVICE_ID_V01,
					QMI_LM_SERVICE_VERS_V01,
					0);
	if (ret < 0) {
		pr_err("%s: failed to add license manager svc server :%d\n",
							__func__, ret);
		goto release_lm_svc_handle;
	}

	INIT_LIST_HEAD(&svc->clients_connected);
	INIT_LIST_HEAD(&svc->clients_feature_list);

	lm_svc = svc;

	/* Creating a directory in /sys/kernel/ */
	lm_kobj = kobject_create_and_add("license_manager", kernel_kobj);
	if (lm_kobj) {
		if (sysfs_create_file(lm_kobj, &lm_update_license_attr.attr)) {
			pr_err("Cannot create update_license sysfs file for lm\n");
			kobject_put(lm_kobj);
		}
		if (sysfs_create_file(lm_kobj, &lm_licensed_features_attr.attr)) {
			pr_err("Cannot create licensed_features sysfs file for lm\n");
			kobject_put(lm_kobj);
		}
	} else {
		pr_err("Unable to create license manager sysfs entry\n");
	}
	pr_info("License Manager driver registered\n");

	return 0;

release_lm_svc_handle:
	qmi_handle_release(svc->lm_svc_hdl);
free_lm_svc_handle:
	kfree(svc->lm_svc_hdl);
free_lm_license_list:
	kfree(svc->license_list);
free_lm_svc:
	kfree(svc);

	return ret;
}

static int license_manager_remove(struct platform_device *pdev)
{
	struct lm_svc_ctx *svc = lm_svc;
	struct client_info *itr, *tmp;
	struct feature_info *iter, *temp;

	qmi_handle_release(svc->lm_svc_hdl);

	if (!list_empty(&svc->clients_connected)) {
		list_for_each_entry_safe(itr, tmp, &svc->clients_connected,
								node) {
			list_del(&itr->node);
			kfree(itr);
		}
	}
	if (!list_empty(&svc->clients_feature_list)) {
		list_for_each_entry_safe(iter, temp, &svc->clients_feature_list,
								node) {
			list_del(&iter->node);
			kfree(iter);
		}
	}

	free_license_table();
	kfree(svc->license_list);
	kfree(svc->lm_svc_hdl);
	kfree(svc);

	lm_svc = NULL;

	return 0;
}

static const struct of_device_id of_license_manager_match[] = {
	{.compatible = "qti,license-manager-service"},
	{  /* sentinel value */ },
};

static struct platform_driver license_manager_driver = {
	.probe		= license_manager_probe,
	.remove		= license_manager_remove,
	.driver		= {
		.name	= "license_manager",
		.of_match_table	= of_license_manager_match,
	},
};

static int __init license_manager_init(void)
{
	return platform_driver_register(&license_manager_driver);
}
module_init(license_manager_init);

static void __exit license_manager_exit(void)
{
	platform_driver_unregister(&license_manager_driver);
}
module_exit(license_manager_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("License manager driver");
