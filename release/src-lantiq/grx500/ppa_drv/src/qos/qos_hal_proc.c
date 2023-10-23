/*****************************************************************************
 **
 ** FILE NAME    : qos_hal_dp.c
 ** PROJECT      : QOS HAL
 ** MODULES      : (QoS Engine )
 **
 ** DATE         : 29 May 2018
 ** AUTHOR       : Purnendu Ghosh
 ** DESCRIPTION  : QOS HAL Layer
 ** COPYRIGHT : Copyright (c) 2017 - 2018 Intel Corporation
 ** Copyright (c) 2010 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 **
 *****************************************************************************/

/*
 *  Common Header File
 */
#include <linux/debugfs.h>
#include "qos_mgr_stack_al.h"
#include <net/datapath_api.h>
#include "qos_hal_debug.h"
#include "qos_mgr_api.h"
#include <net/qos_mgr_common.h>

struct qos_dgb_info
{
	const char *cmd;
	const char *desc;
	uint32_t flag;
};

static struct qos_dgb_info dbg_enable_mask_str[] = {
	{"err", "error level", QOS_DEBUG_ERR },
	{"trace", "debug level", QOS_DEBUG_TRACE },
	{"high", "high level", QOS_DEBUG_HIGH },
	{"all", "enable all debug", QOS_ENABLE_ALL_DEBUG}
};

#define NUM_ENTITY(x)	(sizeof(x) / sizeof(*(x)))
extern int g_qos_mgr_proc_dir_flag;
extern int qos_hal_get_queue_info(struct net_device *netdev);
extern int qos_hal_get_queue_map(struct net_device *netdev);
extern int qos_hal_get_detailed_queue_map(uint32_t queue_index);
extern void qos_hal_dump_qos_mgr_info(uint32_t index, char *type);
static int proc_read_queue_info_seq_open(struct inode *, struct file *);
static int proc_read_queue_info(struct seq_file *, void *);
static int proc_read_tc_map_seq_open(struct inode *, struct file *);
static int proc_read_tc_map(struct seq_file *, void *);
static int proc_read_queue_map_seq_open(struct inode *, struct file *);
static int proc_read_queue_map(struct seq_file *, void *);
static int proc_read_tc_group_seq_open(struct inode *, struct file *);
static int proc_read_tc_group(struct seq_file *, void *);
#if DP_EVENT_CB
static int proc_read_wmm_control_seq_open(struct inode *, struct file *);
static int proc_read_wmm_control(struct seq_file *, void *);
#endif
static int proc_read_spl_conn_control_seq_open(struct inode *, struct file *);
static int proc_read_spl_conn_control(struct seq_file *, void *);
static int proc_read_qos_mgr_info_seq_open(struct inode *inode, struct file *file);
static int proc_read_qos_mgr_info(struct seq_file *seq, void *v);

static int proc_read_dbg(struct seq_file *seq, void *v);
static ssize_t proc_write_queue_info(struct file *file, const char __user *buf, size_t count, loff_t *data);
static ssize_t proc_write_tc_map(struct file *file, const char __user *buf, size_t count, loff_t *data);
static ssize_t proc_write_queue_map(struct file *file, const char __user *buf, size_t count, loff_t *data);
static ssize_t proc_write_dbg(struct file *file, const char __user *buf, size_t count, loff_t *data);
static ssize_t proc_write_tc_group(struct file *file, const char __user *buf, size_t count, loff_t *data);
#if DP_EVENT_CB
static ssize_t proc_write_wmm_control(struct file *file, const char __user *buf, size_t count, loff_t *data);
#endif
static ssize_t proc_write_spl_conn_control(struct file *file, const char __user *buf, size_t count, loff_t *data);
static ssize_t proc_write_qos_mgr_info(struct file *file, const char __user *buf, size_t count, loff_t *data);

extern uint32_t high_prio_q_limit;
extern uint32_t g_qos_dbg;

struct dentry *g_qos_mgr_proc_dir = NULL;
int g_qos_mgr_proc_dir_flag = 0;

static int strincmp(const char *p1, const char *p2, int n)
{
	int c1 = 0, c2;

	while (n && *p1 && *p2) {
		c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
		c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
		c1 -= c2;
		if (c1)
			return c1;
		p1++;
		p2++;
		n--;
	}

	return n ? *p1 - *p2 : c1;
}

int Atoi(char *str)
{
	int res = 0;
	int sign = 1;
	int i = 0;

	/*If number is negative, then update sign*/
	if (str[0] == '-') {
		sign = -1;
		i++;  /*Also update index of first digit*/
	}

	/*Iterate through all digits and update the result*/
	for (; str[i] != '\0'; ++i)
		res = res*10 + str[i] - '0';

	/*Return result with sign*/
	return sign*res;
}

int return_val(char *p, char *str)
{
	char *temp;
	char buf[30];
	snprintf(buf, sizeof(buf), "%s", p);

	temp = strstr(buf, str);
	if (temp != NULL) {
		while (*temp != ' ' && *temp)
			temp++;
		str = ++temp;
		while (*temp != ' ' && *temp)
			temp++;

		*temp = '\0';
	}
	return Atoi(str);
}

char *return_string(char *buf, char *str)
{
	char *temp;

	temp = strstr(buf, str);
	if (temp != NULL) {
		while (*temp != ' ' && *temp)
			temp++;

		str = ++temp;
		while (*temp != ' ' && *temp)
			temp++;

		*temp = '\0';
	}
	return str;
}


static struct file_operations g_proc_file_queue_info_seq_fops = {
	.owner      = THIS_MODULE,
	.open       = proc_read_queue_info_seq_open,
	.read       = seq_read,
	.write	= proc_write_queue_info,
	.llseek     = seq_lseek,
	.release    = single_release,
};

static int proc_read_queue_info_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_queue_info, NULL);
}

static int proc_read_queue_info(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk("Read Permission denied");
		return 0;
	}
	seq_printf(seq, "echo <interface> > /sys/kernel/debug/qos_mgr/queue_info\n");
	return 0;
}

static ssize_t proc_write_queue_info(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[64];
	char *p;
	struct net_device *netdev;
	uint32_t l_qos_dbg;

	if (!capable(CAP_NET_ADMIN)) {
		printk("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)sizeof(str) - 1);
	len -= qos_mgr_copy_from_user(str, buf, len);
	while (len && str[len - 1] <= ' ')
		len--;
	str[len] = 0;
	for (p = str; *p && *p <= ' '; p++)
		len--;
	if (!*p)
		return count;

	if (strstr(p, "help")) {
		printk("echo <interface>  > /sys/kernel/debug/qos_mgr/queue_info\n");
		return count;
	}
	netdev = dev_get_by_name(&init_net, p);
	if (netdev == NULL) {
		printk("Invalid interface name\n");
		return QOS_MGR_FAILURE;
	}
	l_qos_dbg = g_qos_dbg;
	g_qos_dbg = QOS_DEBUG_TRACE;
	qos_hal_get_queue_info(netdev);
	g_qos_dbg = l_qos_dbg;

	if (netdev)
		dev_put(netdev);

	return count;
}

static int proc_read_dbg_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_dbg, NULL);
}


static struct file_operations g_proc_file_dbg_seq_fops = {
	.owner      = THIS_MODULE,
	.open       = proc_read_dbg_seq_open,
	.read       = seq_read,
	.write      = proc_write_dbg,
	.llseek     = seq_lseek,
	.release    = single_release,
};

static int proc_read_dbg(struct seq_file *seq, void *v)
{
	int i;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq, "\t====================================================\n");
	seq_printf(seq, "\t|     CMD        |    Discription    |     Status    |\n");
	seq_printf(seq, "\t----------------------------------------------------\n");
	for (i = 0;  i < NUM_ENTITY(dbg_enable_mask_str) - 1; i++) {
		seq_printf(seq, "\t|%9s       |", dbg_enable_mask_str[i].cmd);
		seq_printf(seq, "%9s       |", dbg_enable_mask_str[i].desc);
		seq_printf(seq, "%9s       |\n", (g_qos_dbg & dbg_enable_mask_str[i].flag)  ? "enabled" : "disabled");
		seq_printf(seq, "\t---------------------------------------------------- \n");
	}
	return 0;
}


static ssize_t proc_write_dbg(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[64];
	char *p;

	int f_enable = 0;
	int i;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)(sizeof(str) - 1));
	len -= qos_mgr_copy_from_user(str, buf, len);
	while (len && str[len - 1] <= ' ')
		len--;
	str[len] = 0;
	for (p = str; *p && *p <= ' '; p++)
		len--;
	if (!*p)
		return count;
	if (strincmp(p, "enable", 6) == 0) {
		p += 6 + 1; /*skip enable and one blank*/
		len -= 6 + 1; /*len maybe negative now if no other params*/
		f_enable = 1;

	} else if (strincmp(p, "disable", 7) == 0) {
		p += 7 + 1;  /*skip disable and one blank*/
		len -= 7 + 1; /*len maybe negative now if no other parameters*/
		f_enable = -1;

	} else if (strincmp(p, "help", 4) == 0 || *p == '?') {
		printk("echo <enable/disable> [");
		for (i = 0; i < NUM_ENTITY(dbg_enable_mask_str); i++)
			printk("%s/", dbg_enable_mask_str[i].cmd);
		printk("] > /sys/kernel/debug/qos_mgr/dbg\n");
	}
	if (f_enable) {
		if ((len <= 0) || (p[0] >= '0' && p[1] <= '9')) {
			if (f_enable > 0)
				g_qos_dbg |= QOS_ENABLE_ALL_DEBUG;
			else
				g_qos_dbg &= ~QOS_ENABLE_ALL_DEBUG;
		} else {
			do {
				for (i = 0; i < NUM_ENTITY(dbg_enable_mask_str); i++)
					if (strincmp(p, dbg_enable_mask_str[i].cmd,
						strlen(dbg_enable_mask_str[i].cmd)) == 0) {
						if (f_enable > 0) {
							int32_t j = 0;
							do {
								g_qos_dbg |= 1 << j;
							} while (!(dbg_enable_mask_str[i].flag & 1 << j++));
						} else {
							g_qos_dbg &= ~dbg_enable_mask_str[i].flag;
						}
						p += strlen(dbg_enable_mask_str[i].cmd) + 1; //skip one blank
						len -= strlen(dbg_enable_mask_str[i].cmd) + 1; //skip one blank. len maybe negative now if there is no other parameters

						break;
					}
			} while (i < NUM_ENTITY(dbg_enable_mask_str));
		}
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "QOS HAL Err Debug is enable\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "QOS HAL HIGH Debug is enable\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "QOS HAL TRACE Debug is enable\n");

	return count;
}

static struct file_operations g_proc_file_tc_map_seq_fops = {
	.owner      = THIS_MODULE,
	.open       = proc_read_tc_map_seq_open,
	.read       = seq_read,
	.write	= proc_write_tc_map,
	.llseek     = seq_lseek,
	.release    = single_release,
};

static int proc_read_tc_map_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_tc_map, NULL);
}

static int proc_read_tc_map(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq, "echo <interface> > /sys/kernel/debug/qos_mgr/tc_map\n");
	return 0;
}

static ssize_t proc_write_tc_map(struct file *file, const char __user *buf, size_t count, loff_t *data)
{

	uint32_t len;
	char str[50];
	char *p ;
	struct net_device *netdev ;
	uint32_t l_qos_dbg;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)sizeof(str) - 1);
	len -= qos_mgr_copy_from_user(str, buf, len);
	while (len && str[len - 1] <= ' ')
		len--;
	str[len] = 0;
	for (p = str; *p && *p <= ' '; p++)
		len--;
	if (!*p)
		return count;

	if (strstr(p, "help")) {
		printk("echo <interface> > /sys/kernel/debug/qos_mgr/tc_map\n");
		return count;

	} else {
		netdev = dev_get_by_name(&init_net, p);
		if (netdev == NULL) {
			printk("Invalid interface name\n");
			return QOS_MGR_FAILURE;
		}

		l_qos_dbg = g_qos_dbg;
		g_qos_dbg = QOS_DEBUG_TRACE;
		qos_hal_get_queue_map(netdev);
		g_qos_dbg = l_qos_dbg;

		if (netdev)
			dev_put(netdev);

		return count;
	}
}

static struct file_operations g_proc_file_queue_map_seq_fops = {
	.owner      = THIS_MODULE,
	.open       = proc_read_queue_map_seq_open,
	.read       = seq_read,
	.write	= proc_write_queue_map,
	.llseek     = seq_lseek,
	.release    = single_release,
};

static int proc_read_queue_map_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_queue_map, NULL);
}

static int proc_read_queue_map(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq, "echo <QId> > /sys/kernel/debug/qos_mgr/queue_map\n");
	return 0;
}

static ssize_t proc_write_queue_map(struct file *file, const char __user *buf, size_t count, loff_t *data)
{

	uint32_t len;
	uint32_t l_qos_dbg;
	char str[50];
	char *p;
	int32_t queue_index;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)sizeof(str) - 1);
	len -= qos_mgr_copy_from_user(str, buf, len);
	while (len && str[len - 1] <= ' ')
		len--;
	str[len] = 0;
	for (p = str; *p && *p <= ' '; p++)
		len--;
	if (!*p)
		return count;

	if (strstr(p, "help")) {
		printk("echo <QId> > /sys/kernel/debug/qos_mgr/queue_map\n");
		return count;

	} else {
		queue_index = Atoi(p);
		l_qos_dbg = g_qos_dbg;
		g_qos_dbg = QOS_DEBUG_TRACE;
		qos_hal_get_detailed_queue_map(queue_index);
		g_qos_dbg = l_qos_dbg;
		return count;
	}

}

static struct file_operations g_proc_file_tc_group_seq_fops = {
	.owner      = THIS_MODULE,
	.open       = proc_read_tc_group_seq_open,
	.read       = seq_read,
	.write	= proc_write_tc_group,
	.llseek     = seq_lseek,
	.release    = single_release,
};

static int proc_read_tc_group_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_tc_group, NULL);
}

static int proc_read_tc_group(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq, "echo <tc group split Idx> > /sys/kernel/debug/qos_mgr/tc_group\n");
	return 0;
}

static ssize_t proc_write_tc_group(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	uint32_t len;
	char str[50];
	char *p ;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)sizeof(str) - 1);
	len -= qos_mgr_copy_from_user(str, buf, len);
	while (len && str[len - 1] <= ' ')
		len--;
	str[len] = 0;
	for (p = str; *p && *p <= ' '; p++)
		len--;
	if (!*p)
		return count;

	if (strstr(p, "help")) {
		printk("echo <tc group split Idx> > /sys/kernel/debug/qos_mgr/tc_group\n");
		return count;

	} else {
		high_prio_q_limit = Atoi(p);
		printk("high_prio_q_limit = %d\n", high_prio_q_limit);
		return count;
	}

}

#if DP_EVENT_CB
static struct file_operations g_proc_file_wmm_control_seq_fops = {
	.owner      = THIS_MODULE,
	.open       = proc_read_wmm_control_seq_open,
	.read       = seq_read,
	.write	    = proc_write_wmm_control,
	.llseek     = seq_lseek,
	.release    = single_release,
};

static int proc_read_wmm_control_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_wmm_control, NULL);
}

static int proc_read_wmm_control(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}

	qos_hal_global_wmm_status();
	return 0;
}

static ssize_t proc_write_wmm_control(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[64];
	char *p;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("write permission denied");
		return 0;
	}
	len = min(count, (size_t)(sizeof(str) - 1));
	len -= qos_mgr_copy_from_user(str, buf, len);
	while (len && str[len - 1] <= ' ')
		len--;
	str[len] = 0;
	for (p = str; *p && *p <= ' '; p++)
		len--;
	if (!*p)
		return count;
	if (strincmp(p, "enable", 6) == 0) {
		qos_hal_global_wmm_enable_disable(1);
	} else if (strincmp(p, "disable", 7) == 0) {
		qos_hal_global_wmm_enable_disable(0);
	} else {
		printk("echo <enable/disable> > /sys/kernel/debug/qos_mgr/wmm_control\n");
	}

	return count;
}
#endif

static struct file_operations g_proc_file_spl_conn_control_seq_fops = {
	.owner      = THIS_MODULE,
	.open       = proc_read_spl_conn_control_seq_open,
	.read       = seq_read,
	.write	    = proc_write_spl_conn_control,
	.llseek     = seq_lseek,
	.release    = single_release,
};

static int proc_read_spl_conn_control_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_spl_conn_control, NULL);
}

static int proc_read_spl_conn_control(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}

	return 0;
}

static ssize_t proc_write_spl_conn_control(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	int f_spl_conn, device_type, queue_len;
	char str[64];
	char *p;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("write permission denied");
		return 0;
	}
	memset(str, 0x0, sizeof(str));
	len = min(count, (size_t)(sizeof(str) - 1));
	len -= qos_mgr_copy_from_user(str, buf, len);

	while (len && str[len - 1] <= ' ')
		len--;
	str[len] = 0;
	for (p = str; *p && *p <= ' '; p++)
		len--;
	if (!*p) {
		printk("echo <lro> <queue size> > /sys/kernel/debug/qos_mgr/spl_conn_control\n");
		return count;
	}
	if (strincmp(p, "lro", 3) == 0) {
		device_type = DP_SPL_TOE;
		p += 3 + 1; /*skip enable and one blank*/
		len -= 3 + 1; /*len maybe negative now if no other params*/
		f_spl_conn = 1;
	} else {
		printk("echo <lro> <queue size> > /sys/kernel/debug/qos_mgr/spl_conn_control\n");
		return count;
	}

	if (f_spl_conn) {
		if (len <= 0) {
			printk("echo <lro> <queue size> > /sys/kernel/debug/qos_mgr/spl_conn_control\n");
		} else {
			queue_len = Atoi(p);
			if (device_type == DP_SPL_TOE) {
				qos_hal_spl_conn_mod(DP_SPL_TOE, queue_len);
			}
		}
	}

	return count;
}


static struct file_operations g_proc_file_qos_mgr_info_seq_fops = {
	.owner      = THIS_MODULE,
	.open       = proc_read_qos_mgr_info_seq_open,
	.read       = seq_read,
	.write	    = proc_write_qos_mgr_info,
	.llseek     = seq_lseek,
	.release    = single_release,
};

static int proc_read_qos_mgr_info_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_qos_mgr_info, NULL);
}

static int proc_read_qos_mgr_info(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}

	printk("echo <queue/shaper/port/sched> <index> > /sys/kernel/debug/qos_mgr/qos_mgr_info\n");
	printk("To print all use:\n echo <queue/shaper/port/sched> -1 > /sys/kernel/debug/qos_mgr/qos_mgr_info\n");

	return 0;
}

static ssize_t proc_write_qos_mgr_info(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[64];
	char *p, *ptr = NULL;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("write permission denied");
		return 0;
	}
	len = min(count, (size_t)(sizeof(str) - 1));
	len -= qos_mgr_copy_from_user(str, buf, len);
	while (len && str[len - 1] <= ' ')
		len--;
	str[len] = 0;
	for (p = str; *p && *p <= ' '; p++)
		len--;
	if (!*p)
		return count;

	if (strincmp(p, "queue ", 5) == 0) {

		ptr = strsep(&p, " ");
		if (!ptr)
			goto usage;

		ptr = strsep(&p, " ");
		if (!ptr)
			goto usage;

		qos_hal_dump_qos_mgr_info(Atoi(ptr), "queue");

		goto end;

	} else if (strincmp(p, "shaper", 6) == 0) {

		ptr = strsep(&p, " ");
		if (!ptr)
			goto usage;

		ptr = strsep(&p, " ");
		if (!ptr)
			goto usage;

		qos_hal_dump_qos_mgr_info(Atoi(ptr), "shaper");

		goto end;

	} else if (strincmp(p, "port", 4) == 0) {

		ptr = strsep(&p, " ");
		if (!ptr)
			goto usage;

		ptr = strsep(&p, " ");
		if (!ptr)
			goto usage;

		qos_hal_dump_qos_mgr_info(Atoi(ptr), "port");

		goto end;

	} else if (strincmp(p, "sched", 5) == 0) {

		ptr = strsep(&p, " ");
		if (!ptr)
			goto usage;

		ptr = strsep(&p, " ");
		if (!ptr)
			goto usage;

		qos_hal_dump_qos_mgr_info(Atoi(ptr), "sched");

		goto end;
	}

usage:
	printk("echo <queue/shaper/port/sched> <index> > /sys/kernel/debug/qos_mgr/qos_mgr_info\n");
	printk("To print all use:\n echo <queue/shaper/port/sched> -1 > /sys/kernel/debug/qos_mgr/qos_mgr_info\n");

end:
	return count;
}

int qos_hal_proc_create(void)
{
	struct dentry *file = NULL;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "QOS HAL Create Proc entries flag:[%d]\n", g_qos_mgr_proc_dir_flag);
	printk("QOS HAL Create Proc entries flag:[%d]\n", g_qos_mgr_proc_dir_flag);

	if (!g_qos_mgr_proc_dir_flag) {
		g_qos_mgr_proc_dir = debugfs_create_dir("qos_mgr", NULL);
		if (g_qos_mgr_proc_dir == NULL)
			printk("debugfs create dir NULL\n");

		g_qos_mgr_proc_dir_flag = 1;
	}

	file = debugfs_create_file("queue_info", 0600,
			g_qos_mgr_proc_dir, NULL, &g_proc_file_queue_info_seq_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("dbg", 0600,
			g_qos_mgr_proc_dir, NULL, &g_proc_file_dbg_seq_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("tc_map", 0600,
			g_qos_mgr_proc_dir, NULL, &g_proc_file_tc_map_seq_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("queue_map", 0600,
			g_qos_mgr_proc_dir, NULL, &g_proc_file_queue_map_seq_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("tc_group", 0600,
			g_qos_mgr_proc_dir, NULL, &g_proc_file_tc_group_seq_fops);
	if (!file)
		goto err;

#if DP_EVENT_CB
	file = debugfs_create_file("wmm_control", 0600,
			g_qos_mgr_proc_dir, NULL, &g_proc_file_wmm_control_seq_fops);
	if (!file)
		goto err;
#endif
	file = debugfs_create_file("spl_conn_control", 0600,
			g_qos_mgr_proc_dir, NULL, &g_proc_file_spl_conn_control_seq_fops);
	if (!file)
		goto err;

	file = debugfs_create_file("qos_mgr_info", 0600,
			g_qos_mgr_proc_dir, NULL, &g_proc_file_qos_mgr_info_seq_fops);
	if (!file)
		goto err;

	return QOS_MGR_SUCCESS;

err:
	debugfs_remove_recursive(g_qos_mgr_proc_dir);
	g_qos_mgr_proc_dir_flag = 0;
	return QOS_MGR_FAILURE;
}

void qos_hal_proc_destroy(void)
{
	if (g_qos_mgr_proc_dir_flag) {
		debugfs_remove_recursive(g_qos_mgr_proc_dir);
		g_qos_mgr_proc_dir_flag = 0;
	}
}
