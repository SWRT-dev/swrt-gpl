// SPDX-License-Identifier: GPL-2.0
/*
 * Sample in-kernel QMI client driver
 *
 * Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
 * Copyright (C) 2017 Linaro Ltd.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/qrtr.h>
#include <linux/net.h>
#include <linux/completion.h>
#include <linux/idr.h>
#include <linux/string.h>
#include <net/sock.h>
#include <linux/soc/qcom/qmi.h>
#include <linux/kthread.h>

#define PING_REQ1_TLV_TYPE		0x1
#define PING_RESP1_TLV_TYPE		0x2
#define PING_OPT1_TLV_TYPE		0x10
#define PING_OPT2_TLV_TYPE		0x11

#define DATA_REQ1_TLV_TYPE		0x1
#define DATA_RESP1_TLV_TYPE		0x2
#define DATA_OPT1_TLV_TYPE		0x10
#define DATA_OPT2_TLV_TYPE		0x11

#define TEST_MED_DATA_SIZE_V01		8192
#define TEST_MAX_NAME_SIZE_V01		255

#define TEST_PING_REQ_MSG_ID_V01	0x20
#define TEST_DATA_REQ_MSG_ID_V01	0x21

#define TEST_PING_REQ_MAX_MSG_LEN_V01	266
#define TEST_DATA_REQ_MAX_MSG_LEN_V01	8456

struct file *qmi_fp;	/* for getting private_data */

/* Number of iterations to run during test */
static unsigned long niterations = 5;
module_param_named(niterations, niterations, ulong, S_IRUGO | S_IWUSR | S_IWGRP);

/* Size of data during "data" command */
static unsigned long data_size = 50;
module_param_named(data_size, data_size, ulong, S_IRUGO | S_IWUSR | S_IWGRP);

/* Number of cuncurrent Threads running during test */
static unsigned long nthreads = 5;
module_param_named(nthreads, nthreads, ulong, S_IRUGO | S_IWUSR | S_IWGRP);

/* Variable to hold the test result */
static unsigned long test_res;

/* List to hold the incoming command from user-space */
static struct list_head data_list;

/* Data element to be queued during multiple commands */
struct test_qmi_data {
	struct list_head list;
	char data[64];
	atomic_t refs_count;
};

static unsigned long test_count;

/* Completion variable to avoid race between threads and fs write operation */
static struct completion qmi_complete;

/* DebugFS directory structure for QMI */
static struct qmi_dir {
	char string[16];
	unsigned long *value;
	umode_t mode;
}qdentry[] = {
	{"test", &test_res, S_IRUGO | S_IWUGO},
	{"niterations", &niterations, S_IRUGO | S_IWUGO},
	{"data_size", &data_size, S_IRUGO | S_IWUGO},
	{"nthreads", &nthreads, S_IRUGO | S_IWUGO},
};

static atomic_t cnt, async_cnt, async_rsp, async_req, pass, fail;
static struct mutex status_print_lock;

struct test_name_type_v01 {
	u32 name_len;
	char name[TEST_MAX_NAME_SIZE_V01];
};

static struct qmi_elem_info test_name_type_v01_ei[] = {
	{
		.data_type	= QMI_DATA_LEN,
		.elem_len	= 1,
		.elem_size	= sizeof(u8),
		.array_type	= NO_ARRAY,
		.tlv_type	= QMI_COMMON_TLV_TYPE,
		.offset		= offsetof(struct test_name_type_v01,
					   name_len),
	},
	{
		.data_type	= QMI_UNSIGNED_1_BYTE,
		.elem_len	= TEST_MAX_NAME_SIZE_V01,
		.elem_size	= sizeof(char),
		.array_type	= VAR_LEN_ARRAY,
		.tlv_type	= QMI_COMMON_TLV_TYPE,
		.offset		= offsetof(struct test_name_type_v01,
					   name),
	},
	{}
};

struct test_ping_req_msg_v01 {
	char ping[4];

	u8 client_name_valid;
	struct test_name_type_v01 client_name;
};

static struct qmi_elem_info test_ping_req_msg_v01_ei[] = {
	{
		.data_type	= QMI_UNSIGNED_1_BYTE,
		.elem_len	= 4,
		.elem_size	= sizeof(char),
		.array_type	= STATIC_ARRAY,
		.tlv_type	= PING_REQ1_TLV_TYPE,
		.offset		= offsetof(struct test_ping_req_msg_v01,
					   ping),
	},
	{
		.data_type	= QMI_OPT_FLAG,
		.elem_len	= 1,
		.elem_size	= sizeof(u8),
		.array_type	= NO_ARRAY,
		.tlv_type	= PING_OPT1_TLV_TYPE,
		.offset		= offsetof(struct test_ping_req_msg_v01,
					   client_name_valid),
	},
	{
		.data_type	= QMI_STRUCT,
		.elem_len	= 1,
		.elem_size	= sizeof(struct test_name_type_v01),
		.array_type	= NO_ARRAY,
		.tlv_type	= PING_OPT1_TLV_TYPE,
		.offset		= offsetof(struct test_ping_req_msg_v01,
					   client_name),
		.ei_array	= test_name_type_v01_ei,
	},
	{}
};

struct test_ping_resp_msg_v01 {
	struct qmi_response_type_v01 resp;

	u8 pong_valid;
	char pong[4];

	u8 service_name_valid;
	struct test_name_type_v01 service_name;
};

static struct qmi_elem_info test_ping_resp_msg_v01_ei[] = {
	{
		.data_type	= QMI_STRUCT,
		.elem_len	= 1,
		.elem_size	= sizeof(struct qmi_response_type_v01),
		.array_type	= NO_ARRAY,
		.tlv_type	= PING_RESP1_TLV_TYPE,
		.offset		= offsetof(struct test_ping_resp_msg_v01,
					   resp),
		.ei_array	= qmi_response_type_v01_ei,
	},
	{
		.data_type	= QMI_OPT_FLAG,
		.elem_len	= 1,
		.elem_size	= sizeof(u8),
		.array_type	= NO_ARRAY,
		.tlv_type	= PING_OPT1_TLV_TYPE,
		.offset		= offsetof(struct test_ping_resp_msg_v01,
					   pong_valid),
	},
	{
		.data_type	= QMI_UNSIGNED_1_BYTE,
		.elem_len	= 4,
		.elem_size	= sizeof(char),
		.array_type	= STATIC_ARRAY,
		.tlv_type	= PING_OPT1_TLV_TYPE,
		.offset		= offsetof(struct test_ping_resp_msg_v01,
					   pong),
	},
	{
		.data_type	= QMI_OPT_FLAG,
		.elem_len	= 1,
		.elem_size	= sizeof(u8),
		.array_type	= NO_ARRAY,
		.tlv_type	= PING_OPT2_TLV_TYPE,
		.offset		= offsetof(struct test_ping_resp_msg_v01,
					   service_name_valid),
	},
	{
		.data_type	= QMI_STRUCT,
		.elem_len	= 1,
		.elem_size	= sizeof(struct test_name_type_v01),
		.array_type	= NO_ARRAY,
		.tlv_type	= PING_OPT2_TLV_TYPE,
		.offset		= offsetof(struct test_ping_resp_msg_v01,
					   service_name),
		.ei_array	= test_name_type_v01_ei,
	},
	{}
};

struct test_data_req_msg_v01 {
	u32 data_len;
	u8 data[TEST_MED_DATA_SIZE_V01];

	u8 client_name_valid;
	struct test_name_type_v01 client_name;
};

static struct qmi_elem_info test_data_req_msg_v01_ei[] = {
	{
		.data_type	= QMI_DATA_LEN,
		.elem_len	= 1,
		.elem_size	= sizeof(u32),
		.array_type	= NO_ARRAY,
		.tlv_type	= DATA_REQ1_TLV_TYPE,
		.offset		= offsetof(struct test_data_req_msg_v01,
					   data_len),
	},
	{
		.data_type	= QMI_UNSIGNED_1_BYTE,
		.elem_len	= TEST_MED_DATA_SIZE_V01,
		.elem_size	= sizeof(u8),
		.array_type	= VAR_LEN_ARRAY,
		.tlv_type	= DATA_REQ1_TLV_TYPE,
		.offset		= offsetof(struct test_data_req_msg_v01,
					   data),
	},
	{
		.data_type	= QMI_OPT_FLAG,
		.elem_len	= 1,
		.elem_size	= sizeof(u8),
		.array_type	= NO_ARRAY,
		.tlv_type	= DATA_OPT1_TLV_TYPE,
		.offset		= offsetof(struct test_data_req_msg_v01,
					   client_name_valid),
	},
	{
		.data_type	= QMI_STRUCT,
		.elem_len	= 1,
		.elem_size	= sizeof(struct test_name_type_v01),
		.array_type	= NO_ARRAY,
		.tlv_type	= DATA_OPT1_TLV_TYPE,
		.offset		= offsetof(struct test_data_req_msg_v01,
					   client_name),
		.ei_array	= test_name_type_v01_ei,
	},
	{}
};

struct test_data_resp_msg_v01 {
	struct qmi_response_type_v01 resp;

	u8 data_valid;
	u32 data_len;
	u8 data[TEST_MED_DATA_SIZE_V01];

	u8 service_name_valid;
	struct test_name_type_v01 service_name;
};

static struct qmi_elem_info test_data_resp_msg_v01_ei[] = {
	{
		.data_type	= QMI_STRUCT,
		.elem_len	= 1,
		.elem_size	= sizeof(struct qmi_response_type_v01),
		.array_type	= NO_ARRAY,
		.tlv_type	= DATA_RESP1_TLV_TYPE,
		.offset		= offsetof(struct test_data_resp_msg_v01,
					   resp),
		.ei_array	= qmi_response_type_v01_ei,
	},
	{
		.data_type	= QMI_OPT_FLAG,
		.elem_len	= 1,
		.elem_size	= sizeof(u8),
		.array_type	= NO_ARRAY,
		.tlv_type	= DATA_OPT1_TLV_TYPE,
		.offset		= offsetof(struct test_data_resp_msg_v01,
					   data_valid),
	},
	{
		.data_type	= QMI_DATA_LEN,
		.elem_len	= 1,
		.elem_size	= sizeof(u32),
		.array_type	= NO_ARRAY,
		.tlv_type	= DATA_OPT1_TLV_TYPE,
		.offset		= offsetof(struct test_data_resp_msg_v01,
					   data_len),
	},
	{
		.data_type	= QMI_UNSIGNED_1_BYTE,
		.elem_len	= TEST_MED_DATA_SIZE_V01,
		.elem_size	= sizeof(u8),
		.array_type	= VAR_LEN_ARRAY,
		.tlv_type	= DATA_OPT1_TLV_TYPE,
		.offset		= offsetof(struct test_data_resp_msg_v01,
					   data),
	},
	{
		.data_type	= QMI_OPT_FLAG,
		.elem_len	= 1,
		.elem_size	= sizeof(u8),
		.array_type	= NO_ARRAY,
		.tlv_type	= DATA_OPT2_TLV_TYPE,
		.offset		= offsetof(struct test_data_resp_msg_v01,
					   service_name_valid),
	},
	{
		.data_type	= QMI_STRUCT,
		.elem_len	= 1,
		.elem_size	= sizeof(struct test_name_type_v01),
		.array_type	= NO_ARRAY,
		.tlv_type	= DATA_OPT2_TLV_TYPE,
		.offset		= offsetof(struct test_data_resp_msg_v01,
					   service_name),
		.ei_array	= test_name_type_v01_ei,
	},
	{}
};

/*
 * ping_write() - ping_pong debugfs file write handler
 * @file:	debugfs file context
 * @user_buf:	reference to the user data (ignored)
 * @count:	number of bytes in @user_buf
 * @ppos:	offset in @file to write
 *
 * This function allows user space to send out a ping_pong QMI encoded message
 * to the associated remote test service and will return with the result of the
 * transaction. It serves as an example of how to provide a custom response
 * handler.
 *
 * Return: @count, or negative errno on failure.
 */
static ssize_t ping_write(struct file *file, const char __user *user_buf,
			  size_t count, loff_t *ppos)
{
	struct qmi_handle *qmi = file->private_data;
	struct test_ping_req_msg_v01 req = {};
	struct qmi_txn txn;
	int ret;


	memcpy(req.ping, "ping", sizeof(req.ping));

	ret = qmi_txn_init(qmi, &txn, NULL, NULL);
	if (ret < 0)
		return ret;

	ret = qmi_send_request(qmi, NULL, &txn,
			       TEST_PING_REQ_MSG_ID_V01,
			       TEST_PING_REQ_MAX_MSG_LEN_V01,
			       test_ping_req_msg_v01_ei, &req);
	if (ret < 0) {
		qmi_txn_cancel(&txn);
		return ret;
	}

	ret = qmi_txn_wait(&txn, 5 * HZ);
	if (ret < 0)
		count = ret;

	return count;
}

static const struct file_operations ping_fops = {
	.open = simple_open,
	.write = ping_write,
};

static void ping_pong_cb(struct qmi_handle *qmi, struct sockaddr_qrtr *sq,
			 struct qmi_txn *txn, const void *data)
{
	const struct test_ping_resp_msg_v01 *resp = data;

	if (!txn) {
		pr_err("spurious ping response\n");
		return;
	}

	if (resp->resp.result == QMI_RESULT_FAILURE_V01)
		txn->result = -ENXIO;
	else if (!resp->pong_valid || memcmp(resp->pong, "pong", 4))
		txn->result = -EINVAL;

	pr_info("Response for ping is %s\n", resp->pong);
	complete(&txn->completion);
}

/*
 * data_write() - data debugfs file write handler
 * @file:	debugfs file context
 * @user_buf:	reference to the user data
 * @count:	number of bytes in @user_buf
 * @ppos:	offset in @file to write
 *
 * This function allows user space to send out a data QMI encoded message to
 * the associated remote test service and will return with the result of the
 * transaction. It serves as an example of how to have the QMI helpers decode a
 * transaction response into a provided object automatically.
 *
 * Return: @count, or negative errno on failure.
 */
static ssize_t data_write(struct file *file, const char __user *user_buf,
			  size_t count, loff_t *ppos)

{
	struct qmi_handle *qmi = file->private_data;
	struct test_data_resp_msg_v01 *resp;
	struct test_data_req_msg_v01 *req;
	struct qmi_txn txn;
	int ret;

	req = kzalloc(sizeof(*req), GFP_KERNEL);
	if (!req)
		return -ENOMEM;

	resp = kzalloc(sizeof(*resp), GFP_KERNEL);
	if (!resp) {
		kfree(req);
		return -ENOMEM;
	}

	req->data_len = min_t(size_t, sizeof(req->data), count);
	if (copy_from_user(req->data, user_buf, req->data_len)) {
		ret = -EFAULT;
		goto out;
	}

	ret = qmi_txn_init(qmi, &txn, test_data_resp_msg_v01_ei, resp);
	if (ret < 0)
		goto out;

	ret = qmi_send_request(qmi, NULL, &txn,
			       TEST_DATA_REQ_MSG_ID_V01,
			       TEST_DATA_REQ_MAX_MSG_LEN_V01,
			       test_data_req_msg_v01_ei, req);
	if (ret < 0) {
		qmi_txn_cancel(&txn);
		goto out;
	}

	ret = qmi_txn_wait(&txn, 5 * HZ);
	if (ret < 0) {
		goto out;
	} else if (!resp->data_valid ||
		   resp->data_len != req->data_len ||
		   memcmp(resp->data, req->data, req->data_len)) {
		pr_err("response data doesn't match expectation\n");
		ret = -EINVAL;
		goto out;
	}

	pr_info("Response data is %s\n", resp->data);
	ret = count;

out:
	kfree(resp);
	kfree(req);

	return ret;
}

static const struct file_operations data_fops = {
	.open = simple_open,
	.write = data_write,
};

static struct qmi_msg_handler qmi_sample_handlers[] = {
	{
		.type = QMI_RESPONSE,
		.msg_id = TEST_PING_REQ_MSG_ID_V01,
		.ei = test_ping_resp_msg_v01_ei,
		.decoded_size = sizeof(struct test_ping_req_msg_v01),
		.fn = ping_pong_cb
	},
	{}
};

struct qmi_sample {
	struct qmi_handle qmi;

	struct dentry *de_dir;
	struct dentry *de_data;
	struct dentry *de_ping;
	struct dentry *de_test;
	struct dentry *de_nthreads;
	struct dentry *de_niterations;
	struct dentry *de_data_size;
};

static struct dentry *qmi_debug_dir;

static void update_status(void)
{
	unsigned int max = nthreads * niterations;
	unsigned int count = atomic_read(&cnt);
	unsigned int percent;
	static unsigned int pre_percent;

	percent = (count * 100)/max;

	if (percent > pre_percent)
		pr_info("Completed(%d%%)...\n", percent);

	pre_percent = percent;
}

static int test_qmi_ping_pong_send_msg(void)
{
	struct qmi_handle *qmi = qmi_fp->private_data;
	struct test_ping_req_msg_v01 req = {};
	struct qmi_txn txn;
	int ret;

	atomic_inc(&cnt);

	memcpy(req.ping, "ping", sizeof(req.ping));

	ret = qmi_txn_init(qmi, &txn, NULL, NULL);
	if (ret < 0)
		return ret;

	ret = qmi_send_request(qmi, NULL, &txn,
			TEST_PING_REQ_MSG_ID_V01,
			TEST_PING_REQ_MAX_MSG_LEN_V01,
			test_ping_req_msg_v01_ei, &req);
	if (ret < 0) {
		atomic_inc(&fail);
		qmi_txn_cancel(&txn);
		return ret;
	}

	ret = qmi_txn_wait(&txn, 5 * HZ);
	if (ret < 0)
		test_count = ret;

	atomic_inc(&pass);
	mutex_lock(&status_print_lock);
	update_status();
	mutex_unlock(&status_print_lock);
	return test_count;

}

static int test_qmi_data_send_msg(unsigned int data_len)
{
	struct qmi_handle *qmi = qmi_fp->private_data;
	struct test_data_resp_msg_v01 *resp;
	struct test_data_req_msg_v01 *req;
	struct qmi_txn txn;
	int ret, i;

	atomic_inc(&cnt);

	req = kzalloc(sizeof(*req), GFP_KERNEL);
	if (!req) {
		atomic_inc(&fail);
		return -ENOMEM;
	}

	resp = kzalloc(sizeof(*resp), GFP_KERNEL);
	if (!resp) {
		kfree(req);
		atomic_inc(&fail);
		return -ENOMEM;
	}

	req->data_len = data_len;
	for (i = 0; i < req->data_len; i = i + sizeof(int))
		memcpy(req->data + i, (uint8_t *)&i, sizeof(int));

	ret = qmi_txn_init(qmi, &txn, test_data_resp_msg_v01_ei, resp);
	if (ret < 0) {
		atomic_inc(&fail);
		goto out;
	}

	ret = qmi_send_request(qmi, NULL, &txn,
			TEST_DATA_REQ_MSG_ID_V01,
			TEST_DATA_REQ_MAX_MSG_LEN_V01,
			test_data_req_msg_v01_ei, req);
	if (ret < 0) {
		qmi_txn_cancel(&txn);
		atomic_inc(&fail);
		goto out;
	}

	ret = qmi_txn_wait(&txn, 5 * HZ);

	if (ret < 0) {
		goto out;
	} else if (!resp->data_valid ||
			resp->data_len != req->data_len ||
			memcmp(resp->data, req->data, req->data_len)) {
		pr_err("response data doesn't match expectation\n");
		atomic_inc(&fail);
		ret = -EINVAL;
		goto out;
	} else {
		pr_info("Data valid\n");
		atomic_inc(&pass);
	}

	ret = test_count;

	mutex_lock(&status_print_lock);
	update_status();
	mutex_unlock(&status_print_lock);

out:
	kfree(resp);
	kfree(req);

	return ret;
}

int qmi_process_user_input(void *data)
{
	struct test_qmi_data *qmi_data, *temp_qmi_data;
	unsigned short index = 0;

	wait_for_completion_timeout(&qmi_complete, msecs_to_jiffies(1000));

	list_for_each_entry_safe(qmi_data, temp_qmi_data, &data_list, list) {
		atomic_inc(&qmi_data->refs_count);

		if (!strncmp(qmi_data->data, "ping_pong", sizeof(qmi_data->data))) {
			for (index = 0; index < niterations; index++) {
				test_res = test_qmi_ping_pong_send_msg();
			}
		} else if (!strncmp(qmi_data->data, "data", sizeof(qmi_data->data))) {
			for (index = 0; index < niterations; index++) {
				test_res = test_qmi_data_send_msg(data_size);
			}
		} else {
			test_res = 0;
			pr_err("Invalid Test.\n");
			list_del(&qmi_data->list);
			break;
		}

		if (atomic_dec_and_test(&qmi_data->refs_count)) {
			pr_info("Test Completed. Pass: %d Fail: %d\n",
					atomic_read(&pass), atomic_read(&fail));
			list_del(&qmi_data->list);
			kfree(qmi_data);
			qmi_data = NULL;
		}
	}
	return 0;
}

static int test_qmi_open(struct inode *ip, struct file *fp)
{
	char thread_name[32];
	struct task_struct *qmi_task;
	int index = 0;

	if (ip->i_private)
		fp->private_data = ip->i_private;

	atomic_set(&cnt, 0);
	atomic_set(&pass, 0);
	atomic_set(&fail, 0);
	atomic_set(&async_cnt, 0);
	atomic_set(&async_req, 0);
	atomic_set(&async_rsp, 0);
	for (index = 1; index < sizeof(qdentry)/sizeof(struct qmi_dir); index++) {
		if (!strncmp(fp->f_path.dentry->d_iname, qdentry[index].string, \
					sizeof(fp->f_path.dentry->d_iname)))
			return 0;
	}
	pr_info("Total commands: %lu (Threads: %lu Iteration: %lu)\n",
			nthreads * niterations, nthreads, niterations);
	init_completion(&qmi_complete);
	for (index = 0; index < nthreads; index++) {
		snprintf(thread_name, sizeof(thread_name), "qmi_test_""%d", index);
		qmi_task = kthread_run(qmi_process_user_input, &data_list, thread_name);
	}
	return 0;

}

static ssize_t test_qmi_read(struct file *fp, char __user *buf,
		size_t count, loff_t *pos)
{
	char _buf[16] = {0};
	int index = 0;

	for (index = 0; index < sizeof(qdentry)/sizeof(struct qmi_dir); index++) {
		if (!strncmp(fp->f_path.dentry->d_iname, qdentry[index].string, \
					sizeof(fp->f_path.dentry->d_iname)))
			snprintf(_buf, sizeof(_buf), "%lu\n", *qdentry[index].value);
	}

	return simple_read_from_buffer(buf, count, pos, _buf, strnlen(_buf, 16));

}

static int test_qmi_release(struct inode *ip, struct file *fp)
{
	return 0;
}

static ssize_t test_qmi_write(struct file *fp, const char __user *buf,
		size_t count, loff_t *pos)
{
	unsigned char cmd[64];
	int len;
	int index = 0;
	struct test_qmi_data *qmi_data;

	qmi_fp = fp;
	if (count < 1)
		return 0;

	len = min(count, (sizeof(cmd) - 1));

	if (copy_from_user(cmd, buf, len))
		return -EFAULT;

	cmd[len] = 0;
	if (cmd[len-1] == '\n') {
		cmd[len-1] = 0;
		len--;
	}

	for (index = 1; index < sizeof(qdentry)/sizeof(struct qmi_dir); index++) {
		if (!strncmp(fp->f_path.dentry->d_iname, qdentry[index].string, \
					sizeof(fp->f_path.dentry->d_iname))) {
			kstrtoul(cmd, 0, qdentry[index].value);
			return count;
		}
	}

	qmi_data = kmalloc(sizeof(struct test_qmi_data), GFP_KERNEL);
	if (!qmi_data) {
		pr_err("Unable to allocate memory for qmi_data\n");
		return -ENOMEM;
	}

	memcpy(qmi_data->data, cmd, sizeof(cmd));

	atomic_set(&qmi_data->refs_count, 0);
	list_add_tail(&qmi_data->list, &data_list);
	test_count = count;
	complete_all(&qmi_complete);


	return count;

}

static const struct file_operations debug_ops = {
	.open = test_qmi_open,
	.read = test_qmi_read,
	.write = test_qmi_write,
	.release = test_qmi_release,
};

static int qmi_sample_probe(struct platform_device *pdev)
{
	struct sockaddr_qrtr *sq;
	struct qmi_sample *sample;
	char path[20];
	int ret;

	sample = devm_kzalloc(&pdev->dev, sizeof(*sample), GFP_KERNEL);
	if (!sample)
		return -ENOMEM;

	ret = qmi_handle_init(&sample->qmi, TEST_DATA_REQ_MAX_MSG_LEN_V01,
			      NULL,
			      qmi_sample_handlers);
	if (ret < 0)
		return ret;

	sq = dev_get_platdata(&pdev->dev);
	ret = kernel_connect(sample->qmi.sock, (struct sockaddr *)sq,
			     sizeof(*sq), 0);
	if (ret < 0) {
		pr_err("failed to connect to remote service port\n");
		goto err_release_qmi_handle;
	}

	snprintf(path, sizeof(path), "%d:%d", sq->sq_node, sq->sq_port);

	sample->de_dir = debugfs_create_dir(path, qmi_debug_dir);
	if (IS_ERR(sample->de_dir)) {
		ret = PTR_ERR(sample->de_dir);
		goto err_release_qmi_handle;
	}

	sample->de_data = debugfs_create_file("data", 0600, sample->de_dir,
					      sample, &data_fops);
	if (IS_ERR(sample->de_data)) {
		ret = PTR_ERR(sample->de_data);
		goto err_remove_de_dir;
	}

	sample->de_ping = debugfs_create_file("ping", 0600, sample->de_dir,
					      sample, &ping_fops);
	if (IS_ERR(sample->de_ping)) {
		ret = PTR_ERR(sample->de_ping);
		goto err_remove_de_data;
	}

	sample->de_test = debugfs_create_file("test", 0600, sample->de_dir,
					      sample, &debug_ops);

	if (IS_ERR(sample->de_test)) {
		pr_err("Failed to create debugfs entry for test\n");
		goto err_remove_de_data;
	}

	sample->de_nthreads = debugfs_create_file("nthreads", 0600,
						  sample->de_dir, NULL,
						  &debug_ops);
	if (IS_ERR(sample->de_nthreads)) {
		pr_err("Failed to create debugfs entry for nthreads\n");
		goto err_remove_de_test;
	}

	sample->de_niterations = debugfs_create_file("niterations", 0600,
						     sample->de_dir, NULL,
						     &debug_ops);

	if (IS_ERR(sample->de_niterations)) {
		pr_err("Failed to create debugfs entry for niterations\n");
		goto err_remove_de_nthreads;
	}

	sample->de_data_size = debugfs_create_file("data_size", 0600,
						   sample->de_dir, NULL,
						   &debug_ops);
	if (IS_ERR(sample->de_data_size)) {
		pr_err("Failed to create debugfs entry for data size\n");
		goto err_remove_de_niterations;
	}

	platform_set_drvdata(pdev, sample);

	return 0;

err_remove_de_niterations:
	debugfs_remove(sample->de_niterations);
err_remove_de_nthreads:
	debugfs_remove(sample->de_nthreads);
err_remove_de_test:
	debugfs_remove(sample->de_test);
err_remove_de_data:
	debugfs_remove(sample->de_data);
err_remove_de_dir:
	debugfs_remove(sample->de_dir);
err_release_qmi_handle:
	qmi_handle_release(&sample->qmi);

	return ret;
}

static int qmi_sample_remove(struct platform_device *pdev)
{
	struct qmi_sample *sample = platform_get_drvdata(pdev);

	debugfs_remove(sample->de_data_size);
	debugfs_remove(sample->de_niterations);
	debugfs_remove(sample->de_nthreads);
	debugfs_remove(sample->de_test);
	debugfs_remove(sample->de_ping);
	debugfs_remove(sample->de_data);
	debugfs_remove(sample->de_dir);

	qmi_handle_release(&sample->qmi);

	return 0;
}

static struct platform_driver qmi_sample_driver = {
	.probe = qmi_sample_probe,
	.remove = qmi_sample_remove,
	.driver = {
		.name = "qmi_sample_client",
	},
};

static int qmi_sample_new_server(struct qmi_handle *qmi,
				 struct qmi_service *service)
{
	struct platform_device *pdev;
	struct sockaddr_qrtr sq = { AF_QIPCRTR, service->node, service->port };
	int ret;

	pdev = platform_device_alloc("qmi_sample_client", PLATFORM_DEVID_AUTO);
	if (!pdev)
		return -ENOMEM;

	ret = platform_device_add_data(pdev, &sq, sizeof(sq));
	if (ret)
		goto err_put_device;

	ret = platform_device_add(pdev);
	if (ret)
		goto err_put_device;

	service->priv = pdev;

	return 0;

err_put_device:
	platform_device_put(pdev);

	return ret;
}

static void qmi_sample_del_server(struct qmi_handle *qmi,
				  struct qmi_service *service)
{

	struct platform_device *pdev = service->priv;

	platform_device_unregister(pdev);
}

static struct qmi_handle lookup_client;

static struct qmi_ops lookup_ops = {
	.new_server = qmi_sample_new_server,
	.del_server = qmi_sample_del_server,
};

static int qmi_sample_init(void)
{
	int ret;

	qmi_debug_dir = debugfs_create_dir("qmi_sample", NULL);
	if (IS_ERR(qmi_debug_dir)) {
		pr_err("failed to create qmi_sample dir\n");
		return PTR_ERR(qmi_debug_dir);
	}

	ret = platform_driver_register(&qmi_sample_driver);
	if (ret)
		goto err_remove_debug_dir;

	ret = qmi_handle_init(&lookup_client, 0, &lookup_ops, NULL);
	if (ret < 0)
		goto err_unregister_driver;

	qmi_add_lookup(&lookup_client, 15, 0, 0);

	INIT_LIST_HEAD(&data_list);
	mutex_init(&status_print_lock);

	return 0;

err_unregister_driver:
	platform_driver_unregister(&qmi_sample_driver);
err_remove_debug_dir:
	debugfs_remove(qmi_debug_dir);

	return ret;
}

static void qmi_sample_exit(void)
{
	struct test_qmi_data *qmi_data, *temp_qmi_data;

	qmi_handle_release(&lookup_client);

	platform_driver_unregister(&qmi_sample_driver);

	debugfs_remove(qmi_debug_dir);

	list_for_each_entry_safe(qmi_data, temp_qmi_data, &data_list, list) {
		list_del(&qmi_data->list);
		kfree(qmi_data);
	}
	list_del(&data_list);
}

module_init(qmi_sample_init);
module_exit(qmi_sample_exit);

MODULE_DESCRIPTION("Sample QMI client driver");
MODULE_LICENSE("GPL v2");
