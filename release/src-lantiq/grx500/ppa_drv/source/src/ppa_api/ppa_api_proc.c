/*******************************************************************************
**
** FILE NAME	: ppa_api_proc.c
** PROJECT	: PPA
** MODULES	: PPA API (Routing/Bridging Acceleration APIs)
**
** DATE		: 3 NOV 2008
** AUTHOR	: Xu Liang
** DESCRIPTION  : PPA Protocol Stack Hook API Proc Filesystem Functions
** COPYRIGHT	: Copyright (c) 2020, MaxLinear, Inc.
**                Copyright (c) 2009, Lantiq Deutschland GmbH
**                Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date		$Author		 $Comment
** 03 NOV 2008  Xu Liang		Initiate Version
*******************************************************************************/

/*
 * ####################################
 *			  Head File
 * ####################################
 */

/*
 *  Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/netdevice.h>
#include <linux/in.h>
#include <net/sock.h>
#include <asm/time.h>
#include <linux/kallsyms.h>

/*
 *  PPA Specific Head File
 */
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_drv_wrapper.h>
#include <net/ppa/ppa_hal_wrapper.h>

#include "ppa_api_misc.h"
#include "ppa_api_session.h"
#include "ppa_api_core.h"
#if defined(CONFIG_PPA_QOS) && CONFIG_PPA_QOS
#include "ppa_api_qos.h"
#endif
#include "ppa_api_netif.h"
#include "ppa_api_proc.h"
#include "ppa_api_tools.h"
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
#include "ppa_api_session_limit.h"
#include "ppa_api_debug.h"
#endif

#include "ppa_api_sw_accel.h"
/*
 * ####################################
 *			  Definition
 * ####################################
 */
#define STRBUF_LEN 64
#define STRBUF_SMALL_LEN 32

/*
 *  Compilation Switch
 */

/*
 * ####################################
 *			  Data Type
 * ####################################
 */

#ifdef PPA_API_PROC
#undef PPA_API_PROC
#endif

/*
 * ####################################
 *			 Declaration
 * ####################################
 */

void __ppa_session_put(struct uc_session_node *);
void __ppa_session_delete_item(struct uc_session_node *);
static int ppa_uc_session_display_all(void);
static int ppa_uc_session_delete_all(void);

#if IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS)
static ssize_t proc_write_classprio_seq(struct file *, const char __user *, size_t, loff_t *);
static int proc_read_classprio_seq_open(struct inode *, struct file *);
static int proc_read_classprio(struct seq_file *, void *);
#endif

static int proc_read_dbg(struct seq_file *, void *);
static ssize_t proc_write_dbg(struct file *, const char __user *, size_t, loff_t *);
static int proc_read_dbg_seq_open(struct inode *, struct file *);

static int proc_read_hook(struct seq_file *, void *);
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
static int proc_read_sess_mgmt(struct seq_file *, void *);
#endif
static ssize_t proc_write_hook(struct file *, const char __user *, size_t, loff_t *);
static int proc_read_hook_seq_open(struct inode *, struct file *);

#if IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS)
static int proc_read_filter(struct seq_file *, void *);
static ssize_t proc_write_filter(struct file *, const char __user *, size_t, loff_t *);
static int proc_read_filter_seq_open(struct inode *, struct file *);

static int proc_read_queue_per_port(struct seq_file *, void *);
static ssize_t proc_write_queue_per_port(struct file *, const char __user *, size_t, loff_t *);
static int proc_read_queue_per_port_seq_open(struct inode *, struct file *);
#endif
static int proc_read_session_filter(struct seq_file *, void *);
static ssize_t proc_write_session_filter(struct file *, const char __user *, size_t, loff_t *);
static int proc_read_session_filter_seq_open(struct inode *, struct file *);

static void *proc_read_phys_port_seq_start(struct seq_file *, loff_t *);
static void *proc_read_phys_port_seq_next(struct seq_file *, void *, loff_t *);
static void proc_read_phys_port_seq_stop(struct seq_file *, void *);
static int proc_read_phys_port_seq_show(struct seq_file *, void *);
static int proc_read_phys_port_seq_open(struct inode *, struct file *);

static void *proc_read_netif_seq_start(struct seq_file *, loff_t *);
static void *proc_read_netif_seq_next(struct seq_file *, void *, loff_t *);
static void proc_read_netif_seq_stop(struct seq_file *, void *);
static int proc_read_netif_seq_show(struct seq_file *, void *);
static int proc_read_netif_seq_open(struct inode *, struct file *);
static ssize_t proc_file_write_netif(struct file *, const char __user *, size_t, loff_t *);

static void ppa_netif_display(struct netif_info *, unsigned int);
static void ppa_netif_display_single(char *);
static void ppa_netif_display_all(void);

static void *proc_read_session_seq_start(struct seq_file *, loff_t *);
static void *proc_read_session_seq_next(struct seq_file *, void *, loff_t *);
static void proc_read_session_seq_stop(struct seq_file *, void *);
static void printk_session_ifid(char *, uint32_t);
static void printk_session_flags(char *, uint32_t);
#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
static void printk_session_debug_flags(char *, uint32_t);
#endif
static int proc_read_routing_mini_session_seq_show(struct seq_file *, void *);
static int proc_read_mc_mini_group_seq_show(struct seq_file *, void *);
static int proc_read_bridging_mini_session_seq_show(struct seq_file *, void *);
static int proc_read_mini_session_seq_show(struct seq_file *, void *);
static int proc_read_mini_session_seq_open(struct inode *, struct file *);
static ssize_t proc_file_write_mini_session(struct file *, const char __user *, size_t, loff_t *);
int ppa_session_find_unicast_routing(char *);
int ppa_session_find_unicast_routing_sw_hdr(char *);
int ppa_session_find_multicast_routing(char *);
int ppa_session_find_bridging(char *);
void ppa_unicast_routing_print_session(struct uc_session_node *p_item);
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
void ppa_unicast_routing_print_session_sw_hdr(struct uc_session_node *p_item);
#endif /*IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)*/
void ppa_multicast_routing_print_session(struct mc_session_node *p_item);
void ppa_bridging_print_session(struct br_mac_node *p_item);

#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
extern int32_t (*ppa_sw_fastpath_enable_hook)(uint32_t, uint32_t);
extern int32_t (*ppa_get_sw_fastpath_status_hook)(uint32_t *, uint32_t);
static int proc_read_swfpstatus_seq_open(struct inode *, struct file *);
static ssize_t proc_write_swfpstatus(struct file *, const char __user *, size_t , loff_t *);
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
extern int32_t (*ppa_tcp_litepath_enable_hook)(uint32_t, uint32_t);
extern int32_t (*ppa_get_tcp_litepath_status_hook)(uint32_t *, uint32_t);
static int proc_read_tcp_litepath_status_seq_open(struct inode *, struct file *);
static ssize_t proc_write_tcp_litepath_status(struct file *, const char __user *, size_t , loff_t *);
#endif
#endif
#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
static int proc_file_br_flow_open(struct inode *inode, struct file *file);
static ssize_t proc_file_br_flow_write(struct file *file, const char __user *buf, 
					size_t count, loff_t *ppos);
#endif
/* since no where using following functions, commented*/
#if 0
static unsigned char* ppa_sw_ipv4_name(void);
static unsigned char* ppa_sw_ipv6_name(void);
static unsigned char* ppa_sw_6rd_name(void);
static unsigned char* ppa_sw_dslite_name(void);
static unsigned char* ppa_sw_bridged_name(void);
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
static unsigned char* ppa_sw_ltcp_name(void);
#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
static unsigned char* ppa_sw_ltcp_lro_name(void);
#endif
#endif
#endif
typedef unsigned char* (*ppa_get_sw_hdr_type_name)(void);

/* since no where using following functions, commented*/
#if 0
static ppa_get_sw_hdr_type_name ppa_get_sw_hdr_type[SW_ACC_TYPE_MAX] = {
	ppa_sw_ipv4_name,
	ppa_sw_ipv6_name,
	ppa_sw_6rd_name,
	ppa_sw_dslite_name,
	ppa_sw_bridged_name,
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
	ppa_sw_ltcp_name,
#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
	ppa_sw_ltcp_lro_name
#endif
#endif
};
#endif

/*string process help function*/
static int stricmp(const char *, const char *);
static int strincmp(const char *, const char *, int);
static unsigned int get_number(char **p, int *len, int is_hex);
#if IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS)
static int get_token(char **, char **, int *, int *);
static void ignore_space(char **, int *);
#endif /*IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS) */
int ppa_split_buffer(char *buffer, char *array[], int max_param_num);

/*
 * ####################################
 *		   Global Variable
 * ####################################
 */

extern int g_ppa_proc_dir_flag;
extern struct proc_dir_entry *g_ppa_proc_dir;
static char session_buf[STRBUF_LEN];

int g_ppa_api_proc_dir_flag=0;
struct proc_dir_entry *g_ppa_api_proc_dir=NULL;

extern struct list_head *g_session_filter_list[MAX_HAL+1];
static name_value axhals[]={
  {"PAE",PAE_HAL},
  {"SAE",SWAC_HAL},
  {"MPE",MPE_HAL},
  {"ALL",MAX_HAL},
  {"", MAX_HAL + 1}
};

static name_value axproto[]={
  {"UDP",PPA_IPPROTO_UDP},
  {"TCP",PPA_IPPROTO_TCP},
  {"ICMPv6",58},
  {"ICMP",1},
  {"",0}
};


#if IS_ENABLED(CONFIG_SOC_GRX500)
extern ppa_tunnel_entry *g_tunnel_table[MAX_TUNNEL_ENTRIES];
extern uint32_t g_tunnel_counter[MAX_TUNNEL_ENTRIES];
#if IS_ENABLED(CONFIG_PPA_QOS)
static short ppa_port_queue_num_status;
#endif
#define MAX_CLASSPRO_BUF_SIZE 2048
extern int g_eth_class_prio_map[MAX_WLAN_DEV][MAX_TC_NUM];
#if IS_ENABLED(CONFIG_PPA_QOS)
static struct file_operations g_proc_file_classprio_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_classprio_seq_open,
	.read		= seq_read,
	.write		= proc_write_classprio_seq,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int proc_read_classprio_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_classprio, NULL);
}

#endif
#endif

static struct file_operations g_proc_file_dbg_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_dbg_seq_open,
	.read		= seq_read,
	.write		= proc_write_dbg,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int proc_read_dbg_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_dbg, NULL);
}

static struct file_operations g_proc_file_hook_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_hook_seq_open,
	.read		= seq_read,
	.write		= proc_write_hook,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int proc_read_hook_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_hook, NULL);
}

#if IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS)
static struct file_operations g_proc_file_filter_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_filter_seq_open,
	.read		= seq_read,
	.write		= proc_write_filter,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int proc_read_filter_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_filter, NULL);
}

static struct file_operations g_proc_file_queue_per_port_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_queue_per_port_seq_open,
	.read		= seq_read,
	.write		= proc_write_queue_per_port,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int proc_read_queue_per_port_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_queue_per_port, NULL);
}

static ssize_t proc_write_inggrp(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	ppa_qos_inggrp_free_list();
	return count;
}

extern int32_t qosal_get_qos_inggrp(PPA_QOS_INGGRP inggrp, PPA_IFNAME ifnames[PPA_QOS_MAX_IF_PER_INGGRP][PPA_IF_NAME_SIZE]);
static int proc_read_inggrp(struct seq_file *seq, void *v)
{
	PPA_IFNAME ifnames[PPA_QOS_MAX_IF_PER_INGGRP][PPA_IF_NAME_SIZE];
	int32_t i, j, count;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq, "Ingress Groups interfaces\n");

	memset(ifnames, 0, sizeof(ifnames));

	for (i = 0; i < PPA_QOS_INGGRP_MAX; i++) {
		count = qosal_get_qos_inggrp(i, ifnames);
		if ( count ) {
			seq_printf(seq, "Ingress Group %d has %d interfaces:\n", i, count);
			for (j = 0; j < count; j++)
				seq_printf(seq, "  %s\n", ifnames[j]);
		}
		memset(ifnames, 0, sizeof(ifnames));
	}

	return 0;
}

static int proc_read_inggrp_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_inggrp, NULL);
}

static struct file_operations g_proc_file_inggrp_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_inggrp_seq_open,
	.read		= seq_read,
	.write		= proc_write_inggrp,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif
static struct file_operations g_proc_session_filter_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_session_filter_seq_open,
	.read		= seq_read,
	.write		= proc_write_session_filter,
	.llseek		= seq_lseek,
	.release	= single_release,
};

static int proc_read_session_filter_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_session_filter, NULL);
}

#if IS_ENABLED(CONFIG_SOC_GRX500)
static ssize_t proc_write_tunnel_table(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied\n");
		return 0;
	}
	printk("Usage : cat proc/ppa/api/tunnel_table to view the g_tunnel_table\n");
	return count;
}

static int proc_read_tunnel_table(struct seq_file *seq, void *v)
{
	uint8_t i;
	const char *tunnel_type[TUNNEL_TYPE_MAX] = {
		"NULL", "6RD", "DSLITE", "L2TP", "EOGRE", "6EOGRE",
		"IPOGRE", "IP6OGRE", "VXLAN", "IPSEC"
	};

	if (!capable(CAP_NET_ADMIN)) {
		seq_printf(seq, "Read Permission denied\n");
		return 0;
	}
	seq_printf(seq, "\nGlobal Tunnel Table\n\n");

	for(i = 0; i < MAX_TUNNEL_ENTRIES; ++i) {
		if (g_tunnel_table[i]) {
			seq_printf(seq, "idx = %u ; type = %s ; session cout: %u\n",
				i, tunnel_type[g_tunnel_table[i]->tunnel_type], g_tunnel_counter[i]);
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
			if (g_tunnel_table[i]->tunnel_type == TUNNEL_TYPE_IPSEC) {
				if (g_tunnel_table[i]->tunnel_info.ipsec_hdr.inbound)
					seq_printf(seq, "inbound spi = 0x%08x\n",
						g_tunnel_table[i]->tunnel_info.ipsec_hdr.inbound->id.spi);
				if (g_tunnel_table[i]->tunnel_info.ipsec_hdr.outbound)
					seq_printf(seq, "outbound spi = 0x%08x\n",
						g_tunnel_table[i]->tunnel_info.ipsec_hdr.outbound->id.spi);
			}
#endif
		}
	}
	/*Tunnel info regarding other tunnels which can be extracted from g_tunnel_table can be added here accordingly */
	seq_printf(seq, "\n");

	return 0;
}

static int proc_read_tunnel_table_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_tunnel_table, NULL);
}

static struct file_operations g_proc_file_tunnel_table_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_tunnel_table_seq_open,
	.read		= seq_read,
	.write		= proc_write_tunnel_table,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
static int proc_read_sess_mgmt_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_sess_mgmt, NULL);
}

static struct file_operations g_proc_file_sess_mgmt_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_sess_mgmt_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

static struct seq_operations g_proc_read_phys_port_seq_ops = {
	.start	= proc_read_phys_port_seq_start,
	.next	= proc_read_phys_port_seq_next,
	.stop	= proc_read_phys_port_seq_stop,
	.show	= proc_read_phys_port_seq_show,
};
static struct file_operations g_proc_file_phys_port_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_phys_port_seq_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= seq_release,
};
static uint32_t g_proc_read_phys_port_pos = 0;

static struct seq_operations g_proc_read_netif_seq_ops = {
	.start	= proc_read_netif_seq_start,
	.next	= proc_read_netif_seq_next,
	.stop	= proc_read_netif_seq_stop,
	.show	= proc_read_netif_seq_show,
};
static struct file_operations g_proc_file_netif_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_netif_seq_open,
	.read		= seq_read,
	.write		= proc_file_write_netif,
	.llseek		= seq_lseek,
	.release	= seq_release,
};
static uint32_t g_proc_read_netif_pos = 0;

static struct seq_operations g_proc_read_mini_session_seq_ops = {
	.start      = proc_read_session_seq_start,
	.next       = proc_read_session_seq_next,
	.stop       = proc_read_session_seq_stop,
	.show       = proc_read_mini_session_seq_show,
};

static struct file_operations g_proc_file_mini_session_seq_fops = {
	.owner      = THIS_MODULE,
	.open       = proc_read_mini_session_seq_open,
	.read       = seq_read,
	.write      = proc_file_write_mini_session,
	.llseek     = seq_lseek,
	.release    = seq_release,
};

static uint32_t g_proc_read_session_pos = 0;
static uint32_t g_proc_read_session_pos_prev = ~0;

static char *g_str_dest[] = {
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};

#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
static struct file_operations g_proc_file_swfp_status = {
	.owner		= THIS_MODULE,
	.open		= proc_read_swfpstatus_seq_open,
	.read		= seq_read,
	.write		= proc_write_swfpstatus,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
static struct file_operations g_proc_file_tcp_litepath_status = {
	.owner		= THIS_MODULE,
	.open		= proc_read_tcp_litepath_status_seq_open,
	.read		= seq_read,
	.write		= proc_write_tcp_litepath_status,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif
#endif

#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
static struct file_operations g_proc_file_br_flow_learning = {
	.owner		= THIS_MODULE,
	.open		= proc_file_br_flow_open,
	.read		= seq_read,
	.write		= proc_file_br_flow_write,
	.llseek		= seq_lseek,
	.release	= single_release,
};
#endif

extern struct proc_dir_entry proc_root;
enum {
	FAMILY_DANUBE	= 1,
	FAMILY_TWINPASS	= 2,
	FAMILY_AMAZON_SE= 3,
	FAMILY_AR9	= 5,
	FAMILY_VR9	= 7,
	FAMILY_AR10	= 8,
};

enum {
	ITF_2MII	= 1,
	ITF_1MII_ATMWAN = 2,
	ITF_1MII_PTMWAN = 3,
	ITF_2MII_ATMWAN = 4,
	ITF_2MII_PTMWAN = 5,
	ITF_2MII_BONDING= 7,
};

enum {
	TYPE_A1		= 1,
	TYPE_B1		= 2,
	TYPE_E1		= 3,
	TYPE_A5		= 4,
	TYPE_D5		= 5,
	TYPE_D5v2	= 6,
	TYPE_E5		= 7,
};

/*
 * ####################################
 *		   Extern Variable
 * ####################################
 */

/*
 * ####################################
 *			Local Function
 * ####################################
 */
int Atoi(char *str)
{
	int res = 0;  /*Initialize result*/
	int sign = 1;  /*Initialize sign as positive*/
	int i = 0;  /*Initialize index of first digit*/

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

#if IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS)
static int proc_read_classprio(struct seq_file *seq, void *v)
{

	int i, j;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}

	/* skb->priority to firmware queue map (for LAN interface, QId is virtual one maitained by driver)*/
	seq_printf(seq,	"Class to Priority Map:\n");
	seq_printf(seq,	"  class	 :  0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15\n");
	for ( i = 0; i < MAX_WLAN_DEV; i++ ) {
		seq_printf(seq,	"  wlan%d	 :", i);
		for ( j = 0; j < NUM_ENTITY(g_eth_class_prio_map[i]); j++ )
			seq_printf(seq," %2d", g_eth_class_prio_map[i][j]);
		seq_printf(seq,	"\n");
	}

	return 0;
}

static ssize_t proc_write_classprio_seq(struct file *file, const char __user *buf, size_t count, loff_t *data)
{

	char *local_buf;
	int len;
	char *p1, *p2;
	int colon = 1;
	unsigned int port = ~0;
	int prio = -1;
	int class = -1;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	local_buf = (char *)ppa_malloc(MAX_CLASSPRO_BUF_SIZE);
	if(local_buf == NULL){
		ppa_debug(DBG_ENABLE_MASK_ERR, "proc_write_classprio_seq: memory creation Failed\n");
		return 0;
	}
	len = MAX_CLASSPRO_BUF_SIZE < count ? MAX_CLASSPRO_BUF_SIZE - 1 : count;
	len = len - copy_from_user(local_buf, buf, len);
	local_buf[len] = 0;

	p1 = local_buf;
	while ( get_token(&p1, &p2, &len, &colon) ) {
		if ( stricmp(p1, "help") == 0 ) {
			printk("echo <wlanX> class xx prio xx [class xx prio xx] > /proc/ppa/api/classprio\n");
			break;
		} else if ( stricmp(p1, "wlan0") == 0 ) {
			port = 0;
			prio = class = -1;
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"port = 0");
		} else if ( stricmp(p1, "wlan1") == 0 ) {
			port = 1;
			prio = class = -1;
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"port = 1");
		} else if ( stricmp(p1, "wlan2") == 0 ) {
			port = 2;
			prio = class = -1;
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"port = 2");
		} else if ( stricmp(p1, "wlan3") == 0 ) {
			port = 3;
			prio = class = -1;
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"port = 3");
		} else if ( port != ~0 ) {
			if ( stricmp(p1, "c") == 0 || stricmp(p1, "class") == 0 ) {
				ignore_space(&p2, &len);
				class = get_number(&p2, &len, 0);
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"class = %d", class);
				if ( port >= 0 && port < MAX_WLAN_DEV && class >= 0 && class < NUM_ENTITY(g_eth_class_prio_map[port])) {
					if ( prio >= 0 )
						g_eth_class_prio_map[port][class] = prio;
				} else {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"class (%d) is out of range 0 - %d", class,
						NUM_ENTITY(g_eth_class_prio_map[port]) - 1);
				}
			} else if ( stricmp(p1, "p") == 0 || stricmp(p1, "prio") == 0 ) {
				ignore_space(&p2, &len);
				prio = get_number(&p2, &len, 0);
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"prio = %d", prio);
				if ( port >= 0 && port < MAX_WLAN_DEV && prio >= 0 && prio < MAX_PRIO_NUM ) {
					if ( class >= 0 )
						g_eth_class_prio_map[port][class] = prio;
				} else {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"prio (%d) is out of range 0 - %d", prio, 8);
				}
			} else {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"unknown command (%s)", p1);
			}
		} else {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"unknown command (%s)", p1);
		}

		p1 = p2;
		colon = 1;
	}
	ppa_free(local_buf);
	return count;
}
#endif /*IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS) */

struct ppa_dgb_info
{
	char *cmd;
	char *description;
	uint32_t flag;
};

static struct ppa_dgb_info dbg_enable_mask_str[] = {
	{"err",		"error print",		DBG_ENABLE_MASK_ERR },
	{"dbg",		"debug print",		DBG_ENABLE_MASK_DEBUG_PRINT},
	{"dbg2",	"dbg2",			DBG_ENABLE_MASK_DEBUG2_PRINT | DBG_ENABLE_MASK_DEBUG_PRINT},
	{"assert",	"assert",		DBG_ENABLE_MASK_ASSERT},
	{"uc",		"dump unicast routing session",	DBG_ENABLE_MASK_DUMP_ROUTING_SESSION},
	{"mc",		"dump multicast session",	DBG_ENABLE_MASK_DUMP_MC_GROUP },
	{"br",		"dump bridging session",	DBG_ENABLE_MASK_DUMP_BRIDGING_SESSION},
	{"init",	"dump init",			DBG_ENABLE_MASK_DUMP_INIT},
	{"qos",		"dbg qos",			DBG_ENABLE_MASK_QOS},
	{"pwm",		"dbg pwm",			DBG_ENABLE_MASK_PWM},
	{"mfe",		"dbg multiple field",		DBG_ENABLE_MASK_MFE},
	{"pri",		"test qos queue via skb tos",	DBG_ENABLE_MASK_PRI_TEST},
	{"mark",	"test qos queue via skb mark",	DBG_ENABLE_MASK_MARK_TEST},
	{"ssn",		"dbg routing/bridge session",	DBG_ENABLE_MASK_SESSION},
	/*the last one */
	{"all",		"enable all debug",		-1}
};

static int proc_read_dbg(struct seq_file *seq, void *v)
{
	int i;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}

	for ( i = 0; i < NUM_ENTITY(dbg_enable_mask_str) -1; i++ ) {
		seq_printf(seq, "%-10s(%-40s):		%-5s\n", dbg_enable_mask_str[i].cmd, dbg_enable_mask_str[i].description,
			(g_ppa_dbg_enable & dbg_enable_mask_str[i].flag)  ? "enabled" : "disabled");
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
	uint32_t value=0;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
	while ( len && str[len - 1] <= ' ' )
		len--;
	str[len] = 0;
	for ( p = str; *p && *p <= ' '; p++, len-- );
	if ( !*p )
		return count;

	if ( strincmp(p, "enable", 6) == 0 ) {
		p += 6 + 1;  /*skip enable and one blank*/
		len -= 6 + 1;  /*len maybe negative now if there is no other parameters*/
		f_enable = 1;
	} else if ( strincmp(p, "disable", 7) == 0 ) {
		p += 7 + 1;  /*skip disable and one blank*/
		len -= 7 + 1; /*len maybe negative now if there is no other parameters*/
		f_enable = -1;
	} else if ( strincmp(p, "help", 4) == 0 || *p == '?' ) {
		 printk("echo <enable/disable> [");
		 for ( i = 0; i < NUM_ENTITY(dbg_enable_mask_str); i++ ) printk("%s/", dbg_enable_mask_str[i].cmd );
		 printk("] [max_print_num]> /proc/ppa/api/dbg\n");
		 printk("	Note: Default max_print_num is no limit\n");
		 ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"	Current max_print_num=%d\n", max_print_num );
	}

	if ( f_enable ) {
		if ( (len <= 0) || ( p[0] >= '0' && p[1] <= '9') ) {
			if ( f_enable > 0 )
				g_ppa_dbg_enable |= DBG_ENABLE_MASK_ALL;
			else
				g_ppa_dbg_enable &= ~DBG_ENABLE_MASK_ALL;
		} else {
			do {
				for ( i = 0; i < NUM_ENTITY(dbg_enable_mask_str); i++ )
					if ( strincmp(p, dbg_enable_mask_str[i].cmd, strlen(dbg_enable_mask_str[i].cmd) ) == 0 ) {
						if ( f_enable > 0 )
							g_ppa_dbg_enable |= dbg_enable_mask_str[i].flag;
						else
							g_ppa_dbg_enable &= ~dbg_enable_mask_str[i].flag;

						p += strlen(dbg_enable_mask_str[i].cmd) + 1; /*skip one blank*/
						len -= strlen(dbg_enable_mask_str[i].cmd) + 1;
						break;
					}
			} while ( i < NUM_ENTITY(dbg_enable_mask_str) );
		}
		
		if( len > 0 ) {
			value = get_number(&p, &len, 0);
			if( value ) {
				max_print_num=value;				
			}
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"max_print_num=%d\n", max_print_num );
		}
	}

	return count;
}

static int proc_read_hook(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	if ( !ppa_is_init() )
		seq_printf(seq, "PPA: not init\n");
	else {
		seq_printf(seq, "PPA routing\n");
#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
		seq_printf(seq, "  ppa_session_add		- %s\n", ppa_hook_session_add_fn ? "hooked" : "not hooked");
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
		seq_printf(seq, "  ppa_session_prio		- %s\n", ppa_hook_session_prio_fn ? "hooked" : "not hooked");
#endif
		seq_printf(seq, "  ppa_session_delete		- %s\n", ppa_hook_session_del_fn ? "hooked" : "not hooked");
		seq_printf(seq, "  ppa_inactivity_status	- %s\n", ppa_hook_inactivity_status_fn ? "hooked" : "not hooked");
		seq_printf(seq, "  ppa_set_session_inactivity	- %s\n", ppa_hook_set_inactivity_fn ? "hooked" : "not hooked");
#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING)
		seq_printf(seq, "PPA bridging\n");
		seq_printf(seq, "  ppa_bridge_entry_add		- %s\n", ppa_hook_bridge_entry_add_fn ?	"hooked" : "not hooked");
		seq_printf(seq, "  ppa_bridge_entry_delete	- %s\n", ppa_hook_bridge_entry_delete_fn ? "hooked" : "not hooked");
		seq_printf(seq, "  ppa_bridge_entry_hit_time	- %s\n", ppa_hook_bridge_entry_hit_time_fn ? "hooked" : "not hooked");
#endif /*CONFIG_PPA_BR_MAC_LEARNING*/
	}
	return 0;
}

#if IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS)
static int proc_read_filter(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	if (!ppa_is_init()) {
		seq_printf(seq, "PPA: not init\n");
	} else { /*
		seq_printf(seq, "------------------------------------------------\n");
		seq_printf(seq, "  PPA Filtered Packets (Not Accelerated) Stats\n");
		seq_printf(seq, "------------------------------------------------\n\n");
		seq_printf(seq, "ppa_is_pkt_host_output			: %d\n", ppa_filter.ppa_is_pkt_host_output);
		seq_printf(seq, "ppa_is_pkt_host_broadcast		: %d\n", ppa_filter.ppa_is_pkt_broadcast);
		seq_printf(seq, "ppa_is_pkt_host_multicast		: %d\n", ppa_filter.ppa_is_pkt_multicast);
		seq_printf(seq, "ppa_is_pkt_host_loopback		: %d\n", ppa_filter.ppa_is_pkt_loopback);
		seq_printf(seq, "ppa_is_pkt_protocol_invalid		: %d\n", ppa_filter.ppa_is_pkt_protocol_invalid);
		seq_printf(seq, "ppa_is_pkt_fragment			: %d\n", ppa_filter.ppa_is_pkt_fragment);
		seq_printf(seq, "ppa_is_pkt_ignore_special_session	: %d\n", ppa_filter.ppa_is_pkt_ignore_special_session);
		seq_printf(seq, "ppa_is_pkt_not_routing			: %d\n", ppa_filter.ppa_is_pkt_not_routing);
		seq_printf(seq, "ppa_is_total_filtered			: %d\n", ppa_filter.ppa_is_filtered);
		seq_printf(seq, "------------------------------------------------\n\n");
		*/
	}
	return 0;
}

static ssize_t proc_write_filter(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[PROC_STR_LEN];
	char *cmd;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)(PROC_STR_LEN - 1));

	len -= ppa_copy_from_user(str, buf, len);

	/* skip spaces */
	while (len && str[len - 1] <= ' ')
		len--;

	str[len] = 0;
	for (cmd = str; *cmd && (*cmd) <= ' '; cmd++, len--);

	if (!(*cmd))
		return count;

	if (stricmp(cmd, "reset") == 0) {
		ppa_memset(&ppa_filter, 0, sizeof(ppa_filter));
	} else {
		printk("Usage: echo reset > /proc/ppa/api/filter\n");
		printk("About: This proc entry is to see the filtered packets that\n");
		printk("could not be accelerated\n");
	}
	return count;
}

static int proc_read_queue_per_port(struct seq_file *seq, void *v)
{
	int i;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	if (ppa_port_queue_num_status == 1) {
		if (!ppa_is_init()) {
			seq_printf(seq, "PPA: not init\n");
		} else {
			seq_printf(seq, "------------------------------------------------\n");
			seq_printf(seq, "	PPA No. of Queues per Port with status\n");
			seq_printf(seq, "------------------------------------------------\n");
			seq_printf(seq, "PORT_ID		  NUM_QUEUES	PORT_STATUS\n");
			seq_printf(seq, "------------------------------------------------\n");
			for (i = 0; i < PPA_MAX_PORT_NUM; i++) {
				seq_printf(seq, "%d	   |	  %2d	   |	%s\n", ppa_port_qnum[i].portid,
					ppa_port_qnum[i].num, ppa_port_qnum[i].status);
			}
			seq_printf(seq, "------------------------------------------------\n");
		}
	} else {
		seq_printf(seq, "This proc entry is disabled. You can enable it by doing\n");
		seq_printf(seq, "echo enable > /proc/ppa/api/queue/per/port\n");
	}

	return 0;
}

static ssize_t proc_write_queue_per_port(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[PROC_STR_LEN];
	char *cmd;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)(PROC_STR_LEN - 1));

	len -= ppa_copy_from_user(str, buf, len);

	/* skip spaces */
	while (len && str[len - 1] <= ' ')
		len--;

	str[len] = 0;
	for (cmd = str; *cmd && (*cmd) <= ' '; cmd++, len--);

	if (!(*cmd))
		return count;

	if (stricmp(cmd, "enable") == 0) {
		ppa_port_queue_num_status = 1;
	} else if (stricmp(cmd, "disable") == 0) {
		ppa_port_queue_num_status = 0;
	} else {
		printk("Usage: echo enable/disable > /proc/ppa/api/queue_per_port\n");
		printk("If enabled, this proc entry will help the user to see the number of\n");
		printk("queues associated per ppa port\n");
	}

	return count;
}
#endif /* IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS)*/


char* value_to_str(int value, name_value *pxnv)
{
  name_value *pxtmp = pxnv;
  while(pxtmp && pxtmp->name[0]){
		if (pxtmp->value == value)
    	return pxtmp->name;
    pxtmp++;
  }
  return "Invalid";
}

uint16_t str_to_value(char *pcname, name_value *pxnv)
{
  name_value *pxtmp=pxnv;

  while (pxtmp && pxtmp->name && strnlen(pxtmp->name, MAX_NAME_LEN) > 0) {
    if(strncasecmp(pxtmp->name,pcname,strnlen(pxtmp->name, MAX_NAME_LEN))==0){
      return pxtmp->value;
    }
    pxtmp++;
  }
  return 0;
}

uint16_t str_to_num(char *str)
{
	uint16_t val = 0, i = 0;
	if (!str) return 0;

	while (str[i] != ' ' && str[i] != 0) {
		val = val * 10 + str[i] - '0';
		i++;
	}
	return val;
}

static int proc_read_session_filter(struct seq_file *seq, void *v)
{
	struct list_head *list_node = NULL;
	FILTER_INFO *filter_node = NULL;
	int i = 0, j = 0;;

	if (!ppa_is_init()) {
		seq_printf(seq, "PPA: not init\n");
	} else {
		seq_printf(seq, "------------------------------------------------\n");
		seq_printf(seq, "  PPA Session Filtered List\n");
		seq_printf(seq, "------------------------------------------------\n\n");
		for (i=0; i< MAX_HAL + 1; i++) {
			if (g_session_filter_list[i]) {
			seq_printf(seq, "Session Filter List for %s HAL\n",value_to_str(i, axhals));
			j = 1;
			list_for_each(list_node, g_session_filter_list[i]) {
    		filter_node = list_entry(list_node, FILTER_INFO, list);
				seq_printf(seq, "  %2d. Proto: %s, SrcPort: %d, DestPort:%d\n", j, value_to_str(filter_node->ip_proto, axproto), filter_node->src_port, filter_node->dst_port);
				j++;
			}
			seq_printf(seq, "\n");
			}
		}
		seq_printf(seq, "------------------------------------------------\n\n");
	}
	return 0;
}

static ssize_t proc_write_session_filter(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len, i;
	char str[PROC_STR_LEN];
	char *cmd, *token;
	int f_add = -1;
	uint16_t hal;
	FILTER_INFO filter;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)(PROC_STR_LEN - 1));

	len -= ppa_copy_from_user(str, buf, len);

	/* skip spaces */
	while (len && str[len - 1] <= ' ')
		len--;

	str[len] = 0;
	for (cmd = str; *cmd && (*cmd) <= ' '; cmd++, len--);

	if (!(*cmd))
		return count;

	if ( strincmp(cmd, "add", 3) == 0 ) {
		cmd += 3 + 1;  /*skip add and one blank*/
		len -= 3 + 1;
		f_add = 1;
	} else if ( strincmp(cmd, "del", 3) == 0 ) {
		cmd += 3 + 1;  /*skip disable and one blank*/
		len -= 3 + 1;
		f_add = 0;
	} else {
		goto help;
	}

	ppa_memset(&filter, 0, sizeof(FILTER_INFO));
	hal = str_to_value(cmd, axhals);

	if (hal > MAX_HAL || hal == 0) {
		printk("Invalid HAL Id\n");
		return count;
	}

	token = strstr(cmd, "proto=");
	if (token)
		filter.ip_proto = str_to_value(token + 6, axproto);
	else goto help;
		
	token = strstr(cmd, "sport=");
	if (token)
		filter.src_port = str_to_num(token + 6);
	else goto help;
		
	token = strstr(cmd, "dport=");
	if (token)
		filter.dst_port = str_to_num(token + 6);
	else goto help;
	
	if (filter.ip_proto == 0) {
		printk("Invalid Protocol\n");
		goto help;
	}

	if (filter.ip_proto != 0)
		filter.flags |= PPA_SESSION_FILTER_PROTO;

	if (filter.src_port != 0)
		filter.flags |= PPA_SESSION_FILTER_SPORT;

	if (filter.dst_port != 0)
		filter.flags |= PPA_SESSION_FILTER_DPORT;

	if (f_add == 1)
	 add_filter_entry (hal, &filter);
	else if (f_add == 0)
	 del_filter_entry (hal, &filter);

	return count;
help:
	printk("Usage: echo add/del <HAL Name> proto=<proto> sport=<src port> dport=<dest port> > /proc/ppa/api/session_filter\n");
	printk("Example: echo add PAE proto=UDP sport=1234 dport=4321 > /proc/ppa/api/session_filter\n\n");
	printk("List of available HALs:\n");
	for (i=0; i<(sizeof(axhals)/sizeof(name_value))-1; i++){
		printk("  %d. %s\n",i+1, axhals[i].name);
	}
	return count;
}


#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
static int proc_read_sess_mgmt(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	int session_type, session_prio, engine;
	if ( !ppa_is_init() )
		seq_printf(seq, "PPA: not init\n");
	else
	{
		seq_printf(seq, "\t\t\t%-20s%-20s","PAE", "MPE");
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
		seq_printf(seq, "%-20s", "SWAC");
#endif
		for(session_type=0; session_type<MAX_SESSION_TYPE; session_type++) {
			
			switch(session_type) {
				case LAN:
					seq_printf(seq, "\n%-20s", "LAN");
					break;
				case WAN:
					seq_printf(seq, "\n%-20s", "WAN");
					break;
			}

			for(session_prio=0; session_prio<MAX_SESSION_PRIORITY; session_prio++) {
				switch(session_prio+1) {
					case HIGH:
						seq_printf(seq, "\n%-20s", "HIGH:");
						break;
					case DEFAULT:	
						seq_printf(seq, "\n%-20s", "DEFAULT:");
						break;
					case LOW:
						seq_printf(seq, "\n%-20s", "LOW:");
						break;
				}
				for(engine=0; engine<MAX_DATA_FLOW_ENGINES; engine++)
					seq_printf(seq, "\t%-18d", session_count[session_type][session_prio][engine]);
			}
			seq_printf(seq, "\n");
		}
   }
   return 0;
}
#endif

static ssize_t proc_write_hook(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[64];
	char *p;
	int i;
	PPA_VERSION ppe_fw={0};
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
	while ( len && str[len - 1] <= ' ' )
		len--;
	str[len] = 0;
	for ( p = str; *p && *p <= ' '; p++, len-- );
	if ( !*p )
		return count;

	if ( stricmp(p, "enable") == 0 )
	{
		PPA_IFINFO lanif_info[] = {
			{"eth0",	0},
			{"eth1",	0},
			{"eth0.5",	0},
			{"br0",		0},
		};
		PPA_IFINFO wanif_info[] = {
			{"eth1",	0},
			{"ppp0",	0},
			{"nas0",	0},
			{"nas1",	0},
		};
		PPA_INIT_INFO ppa_init_info = {
			num_lanifs		: NUM_ENTITY(lanif_info),
			p_lanifs		: lanif_info,
			num_wanifs		: NUM_ENTITY(wanif_info),
			p_wanifs		: wanif_info,
			max_lan_source_entries  : 64,
			max_wan_source_entries  : 64,
			max_mc_entries		: 32,
			max_bridging_entries	: 128,
			add_requires_min_hits   : 2
		};

		uint32_t f_incorrect_fw = 1;

		ppe_fw.index = 1;
		if( ppa_drv_get_firmware_id(&ppe_fw, 0) != PPA_SUCCESS ) {
			ppe_fw.index = 0;
			ppa_drv_get_firmware_id(&ppe_fw, 0);
		}

		switch ( ppe_fw.family ) {
		case FAMILY_DANUBE:
		case FAMILY_TWINPASS:
			if ( ppe_fw.itf == ITF_2MII ) {
				/* D4*/
				ppa_init_info.max_lan_source_entries = 128;
				ppa_init_info.max_wan_source_entries = 128;
				ppa_init_info.max_mc_entries = 32;
				ppa_init_info.max_bridging_entries   = 256;
				f_incorrect_fw = 0;
				for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
					if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
						lanif_info[i].ifname = NULL;
			} else if ( ppe_fw.itf == ITF_1MII_ATMWAN ) {
				/*  A4*/
				ppa_init_info.max_lan_source_entries = 64;
				ppa_init_info.max_wan_source_entries = 64;
				ppa_init_info.max_mc_entries = 32;
				ppa_init_info.max_bridging_entries   = 128;
				f_incorrect_fw = 0;
				for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
					if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
						lanif_info[i].ifname = NULL;
				for ( i = 0; i < NUM_ENTITY(wanif_info); i++ )
					if ( strcmp(wanif_info[i].ifname, "eth1") == 0 )
						wanif_info[i].ifname = NULL;
			}
			break;
		case FAMILY_AMAZON_SE:
			if ( ppe_fw.itf == ITF_1MII_ATMWAN ) {
				/*  A4*/
				ppa_init_info.max_lan_source_entries = 44;
				ppa_init_info.max_wan_source_entries = 44;
				ppa_init_info.max_mc_entries = 32;
				ppa_init_info.max_bridging_entries   = 128;
				f_incorrect_fw = 0;
				for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
					if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
						lanif_info[i].ifname = NULL;
				for ( i = 0; i < NUM_ENTITY(wanif_info); i++ )
					if ( strcmp(wanif_info[i].ifname, "eth1") == 0 )
						wanif_info[i].ifname = NULL;
			} else if ( ppe_fw.itf == ITF_1MII_PTMWAN ) {
				/*  E4*/
				ppa_init_info.max_lan_source_entries = 0;
				ppa_init_info.max_wan_source_entries = 88;
				ppa_init_info.max_mc_entries = 32;
				ppa_init_info.max_bridging_entries   = 128;
				f_incorrect_fw = 0;
				for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
					if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
						lanif_info[i].ifname = NULL;
				for ( i = 0; i < NUM_ENTITY(wanif_info); i++ )
					if ( strcmp(wanif_info[i].ifname, "eth1") == 0 )
						wanif_info[i].ifname = "ptm0";
			}
			break;
		case FAMILY_AR9:
		case FAMILY_AR10:
			/*  AR9/AR10 */
			if ( ppe_fw.itf == ITF_2MII || (ppe_fw.itf == 0 && ppe_fw.type == TYPE_D5) ) {
				/*  D5*/
				ppa_init_info.max_lan_source_entries = 576;
				ppa_init_info.max_wan_source_entries = 576;
				ppa_init_info.max_mc_entries = 64;
				ppa_init_info.max_bridging_entries = 0;/* bridging done by internal switch*/
				f_incorrect_fw = 0;
				for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
					if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
						lanif_info[i].ifname = NULL;
			} else if ( ppe_fw.itf == ITF_2MII_ATMWAN	 
				|| (ppe_fw.itf == 0 && ppe_fw.type == TYPE_A5) ) {
				/*  A5*/
				ppa_init_info.max_lan_source_entries = 192;
				ppa_init_info.max_wan_source_entries = 192;
				ppa_init_info.max_mc_entries = 64;
				ppa_init_info.max_bridging_entries = 512;
				f_incorrect_fw = 0;
				for ( i = 0; i < NUM_ENTITY(wanif_info); i++ )
					if ( strcmp(wanif_info[i].ifname, "eth1") == 0 )
						wanif_info[i].ifname = NULL;
			} else if ( ppe_fw.itf == ITF_2MII_PTMWAN
				|| (ppe_fw.itf == 0 && ppe_fw.type == TYPE_E5) ){
				/*  E5*/
				ppa_init_info.max_lan_source_entries = 192;
				ppa_init_info.max_wan_source_entries = 192;
				ppa_init_info.max_mc_entries = 64;
				ppa_init_info.max_bridging_entries = 512;
				f_incorrect_fw = 0;
				for ( i = 0; i < NUM_ENTITY(wanif_info); i++ )
					if ( strcmp(wanif_info[i].ifname, "eth1") == 0 )
						wanif_info[i].ifname = "ptm0";
			}
			break;
		case FAMILY_VR9:
			if ( ppe_fw.itf == ITF_2MII || ppe_fw.itf == ITF_2MII_BONDING
				|| (ppe_fw.itf == 0 && (ppe_fw.type == TYPE_E1 || ppe_fw.type == TYPE_D5 
				|| ppe_fw.type == TYPE_D5v2)) ) {
				/*  D5/E5/BONDING*/
				ppa_init_info.max_lan_source_entries = 192;
				ppa_init_info.max_wan_source_entries = 192;
				ppa_init_info.max_mc_entries = 64;
				ppa_init_info.max_bridging_entries = 0;
				f_incorrect_fw = 0;
				for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
					if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
						lanif_info[i].ifname = NULL;
			} else if ( ppe_fw.itf == ITF_2MII_ATMWAN
				|| (ppe_fw.itf == 0 && ppe_fw.type == TYPE_A5) ) {
				/*  A5*/
				ppa_init_info.max_lan_source_entries = 192;
				ppa_init_info.max_wan_source_entries = 192;
				ppa_init_info.max_mc_entries = 64;
				ppa_init_info.max_bridging_entries = 0;
				f_incorrect_fw = 0;
				for ( i = 0; i < NUM_ENTITY(lanif_info); i++ )
					if ( strcmp(lanif_info[i].ifname, "eth1") == 0 )
						lanif_info[i].ifname = NULL;
			}
			break;
		}
		if ( !f_incorrect_fw ) {
			PPA_MAX_ENTRY_INFO entry={0};
			ppa_drv_get_max_entries(&entry, 0);

			ppa_init_info.max_lan_source_entries = entry.max_lan_entries;
			ppa_init_info.max_wan_source_entries = entry.max_wan_entries;
			ppa_init_info.max_mc_entries = entry.max_mc_entries;
			ppa_init_info.max_bridging_entries = entry.max_bridging_entries;

			ppa_exit();
			ppa_init(&ppa_init_info, 0);
			ppa_enable(1, 1, 0);
		} else {
		   printk("wrong version:family=%d, and itf=%d\n", ppe_fw.family, ppe_fw.itf );
		}
	} else if ( stricmp(p, "disable") == 0 ) {
		ppa_enable(0, 0, 0);
		ppa_exit();
	} else
		printk("echo enable/disable > /proc/ppa/api/hook\n");

	return count;
}

static void *proc_read_phys_port_seq_start(struct seq_file *seq, loff_t *ppos)
{
	struct phys_port_info *ifinfo;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}

	if ( *ppos != 0 )
		(*ppos)--;

	g_proc_read_phys_port_pos = (uint32_t)*ppos;
	if ( ppa_phys_port_start_iteration(&g_proc_read_phys_port_pos, &ifinfo) == PPA_SUCCESS ) {
		*ppos = g_proc_read_phys_port_pos;
		return ifinfo;
	} else
		return NULL;
}

static void *proc_read_phys_port_seq_next(struct seq_file *seq, void *v, loff_t *ppos)
{
	struct phys_port_info *ifinfo = (struct phys_port_info *)v;

	g_proc_read_phys_port_pos = (uint32_t)*ppos;
	if ( ppa_phys_port_iterate_next(&g_proc_read_phys_port_pos, &ifinfo) == PPA_SUCCESS ) {
		*ppos = g_proc_read_phys_port_pos;
		return ifinfo;
	} else
		return NULL;
}

static void proc_read_phys_port_seq_stop(struct seq_file *seq, void *v)
{
	ppa_phys_port_stop_iteration();
}

static int proc_read_phys_port_seq_show(struct seq_file *seq, void *v)
{
	static const char *str_mode[] = {
		"CPU",
		"LAN",
		"WAN",
		"MIX (LAN/WAN)"
	};
	static const char *str_type[] = {
		"CPU",
		"ATM",
		"ETH",
		"EXT"
	};
	static const char *str_vlan[] = {
		"no VLAN",
		"inner VLAN",
		"outer VLAN",
		"N/A"
	};

	struct phys_port_info *ifinfo = (struct phys_port_info *)v;
	static int extra_itf = 0;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	if ( g_proc_read_phys_port_pos == 1 ) {
		seq_printf(seq, "Physical Port List\n");
		if ( PPA_IS_PORT_CPU0_AVAILABLE() ) {
			seq_printf(seq, "  no. %u\n", ++extra_itf);
			seq_printf(seq, "	mode		= %s\n", str_mode[0]);
			seq_printf(seq, "	type		= %s\n", str_type[0]);
			seq_printf(seq, "	vlan		= %s\n", str_vlan[0]);
			seq_printf(seq, "	port		= %d\n", PPA_PORT_CPU0);
		}
		if ( PPA_IS_PORT_ATM_AVAILABLE() ) {
			seq_printf(seq, "  no. %u\n", ++extra_itf);
			seq_printf(seq, "	mode		= %s\n", str_mode[2]);
			seq_printf(seq, "	type		= %s\n", str_type[1]);
			seq_printf(seq, "	vlan		= %s\n", str_vlan[PPA_PORT_ATM_VLAN_FLAGS]);
			seq_printf(seq, "	port		= %d\n", PPA_PORT_ATM);
		}
	}

	seq_printf(seq,	 "  no. %u\n", g_proc_read_phys_port_pos + extra_itf);
	seq_printf(seq,	 "	next		= %px\n", ifinfo->next);
	seq_printf(seq,	 "	mode		= %s\n", str_mode[ifinfo->mode]);
	seq_printf(seq,	 "	type		= %s\n", str_type[ifinfo->type]);
	seq_printf(seq,	 "	vlan		= %s\n", str_vlan[ifinfo->vlan]);
	seq_printf(seq,	 "	port		= %d\n", ifinfo->port);
	seq_printf(seq,	 "	ifname		= %s\n", ifinfo->ifname);

	return 0;
}

static int proc_read_phys_port_seq_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &g_proc_read_phys_port_seq_ops);
}

static void *proc_read_netif_seq_start(struct seq_file *seq, loff_t *ppos)
{
	struct netif_info *ifinfo;

	g_proc_read_netif_pos = (uint32_t)*ppos;
	if ( ppa_netif_start_iteration(&g_proc_read_netif_pos, &ifinfo) == PPA_SUCCESS ) {
		return ifinfo;
	} else
		return NULL;
}

static void *proc_read_netif_seq_next(struct seq_file *seq, void *v, loff_t *ppos)
{
	struct netif_info *ifinfo = (struct netif_info *)v;

	++*ppos; /* workaround for wrong display*/
	g_proc_read_netif_pos = (uint32_t)*ppos;
	if ( ppa_netif_iterate_next(&g_proc_read_netif_pos, &ifinfo) == PPA_SUCCESS ) {
		return ifinfo;
	} else
		return NULL;
}

static void proc_read_netif_seq_stop(struct seq_file *seq, void *v)
{
	ppa_netif_stop_iteration();
}

static int proc_read_netif_seq_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &g_proc_read_netif_seq_ops);
}

static int proc_read_netif_seq_show(struct seq_file *seq, void *v)
{
	static const char *str_flag[] = {
		"VLAN",           /* 0x00000001*/
		"BRIDGE",
		"VANI",
		"UPDATED",
		"PHY_ETH",        /*  0x00000010*/
		"PHY_ATM",
		"PHY_TUNNEL",
		"VXLAN_TUNNEL",
		"BR2684",         /*  0x00000100 */
		"EOA",
		"IPOA",
		"PPPOATM",
		"PPPOE",          /*  0x00001000 */
		"VLAN_INNER",
		"VLAN_OUTER",
		"VLAN_CANT_SUPPORT",
		"LAN_IF",         /*  0x00010000 */
		"WAN_IF",
		"PHY_IF_GOT",
		"PHYS_PORT_GOT",
		"MAC_AVAILABLE",  /*  0x00100000 */
		"MAC_ENTRY_CREATED",
		"PPPOL2TP",
		"DIRECTPATH",
		"GRE_TUNNEL",     /*  0x01000000 */
		"DIRECTCONNECT_WIFI",
		"L2NAT",
		"VETH",
		"Not_Used",
		"Not_Used",
		"Not_Used",
		"Not_Used"
	};

	struct netif_info *ifinfo = (struct netif_info *)v;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
#if defined(PPA_IF_MIB) && PPA_IF_MIB

	if ( g_proc_read_netif_pos == 1 )
	{
		seq_printf(seq,"================================================================================================\n");
		seq_printf(seq,"%26s| %18s| %5s| %11s| %11s| %11s|\n","name (users/status)","lower-if","port","flags","HW MIB rx","HW MIB tx" );
		seq_printf(seq,"================================================================================================\n");
	}
	seq_printf(seq,"%18s (%3d/%1u)| %18s| %5d|  0x%08x| %11u| %11u|\n",ifinfo->name,ppa_atomic_read(&ifinfo->count),ifinfo->enable,
							ifinfo->lower_ifname,ifinfo->phys_port,ifinfo->flags,
							ifinfo->hw_accel_stats.rx_bytes,ifinfo->hw_accel_stats.tx_bytes);

#else

	if ( g_proc_read_netif_pos == 1 )
	{
		seq_printf(seq,"===================================================================\n");
		seq_printf(seq, "%26s| %18s| %5s| %11s|\n","name (users/status)","lower-if","port","flags");
		seq_printf(seq,"===================================================================\n");
	}
	seq_printf(seq,"%18s (%3d/%1u)| %18s| %5d|  0x%08x|\n",ifinfo->name,ppa_atomic_read(&ifinfo->count),
						ifinfo->enable,ifinfo->lower_ifname,ifinfo->phys_port);

#endif
	if(ifinfo->next == NULL)
	{
		int i,j,len=0;
		len = sizeof(str_flag)/sizeof(*str_flag);

		seq_printf(seq,"=======================================================================================================\n");
		seq_printf(seq,"\t\t\t\tFlag bits (MSB to LSB)\n");
		seq_printf(seq,"=======================================================================================================\n");
		for( i=1,j=0; i <= (len / 8); i++)
		{
			seq_printf(seq,"Bits [%02d-%02d] : ",(i*8)-1,(i-1)*8);
			for(j=(i*8)-1; j>=(i-1)*8; j--){
				if(j != (i*8)-1 )
					seq_printf(seq,"| ");
				seq_printf(seq,"%18s",str_flag[j]);
		    }
			seq_printf(seq,"\n");
		}
		seq_printf(seq,"\nNote: In above flags table \"NETIF_\" is stripped from flags\n");
	}
	return 0;
}

void ppa_netif_display_all(void)
{
	struct netif_info *ifinfo;
	unsigned int pos=0;
	if ( ppa_netif_start_iteration(&pos, &ifinfo) == PPA_SUCCESS ){
		do{
			ppa_netif_display(ifinfo, pos);
			printk("------------------------------------------------------------------------------------------------------------------------\n\n");
		}while(ppa_netif_iterate_next(&pos, &ifinfo) == PPA_SUCCESS );
	}
	ppa_netif_stop_iteration();
}

void ppa_netif_display_single(char *if_name)
{
	struct netif_info *ifinfo;
	int found = 0;
	unsigned int pos=0;
	if ( ppa_netif_start_iteration(&pos, &ifinfo) == PPA_SUCCESS ){
		do{
			if ( stricmp(ifinfo->name, if_name) == 0 ){
				ppa_netif_display(ifinfo, pos);
				found = 1;
				break;
			}
		}while(ppa_netif_iterate_next(&pos, &ifinfo) == PPA_SUCCESS );

		if(!found)
			printk("!!!Invalid interface name!!!\n");
	}
	ppa_netif_stop_iteration();
}

void ppa_netif_display(struct netif_info *ifinfo, unsigned int pos)
{
	unsigned long bit;
	int i;
	int flag=0;
	static const char str_flag[][MAX_FLAG_SIZE] = {
		"NETIF_VLAN",			/* 0x00000001*/
		"NETIF_BRIDGE",
		"NETIF_VANI",
		"NETIF_UPDATED",
		"NETIF_PHY_ETH",		/*  0x00000010*/
		"NETIF_PHY_ATM",
		"NETIF_PHY_TUNNEL",
		"NETIF_VXLAN_TUNNEL",
		"NETIF_BR2684",			/*  0x00000100 */
		"NETIF_EOA",
		"NETIF_IPOA",
		"NETIF_PPPOATM",
		"NETIF_PPPOE",			/*  0x00001000 */
		"NETIF_VLAN_INNER",
		"NETIF_VLAN_OUTER",
		"NETIF_VLAN_CANT_SUPPORT",
		"NETIF_LAN_IF",			/*  0x00010000 */
		"NETIF_WAN_IF",
		"NETIF_PHY_IF_GOT",
		"NETIF_PHYS_PORT_GOT",
		"NETIF_MAC_AVAILABLE",		/*  0x00100000 */
		"NETIF_MAC_ENTRY_CREATED",
		"NETIF_PPPOL2TP",
		"NETIF_DIRECTPATH",
		"NETIF_GRE_TUNNEL",		/*  0x01000000 */
		"NETIF_DIRECTCONNECT_WIFI",
		"NETIF_L2NAT",
		"NETIF_VETH",
		"Not_used",
		"Not_used",
		"Not_used",
		"Not_used"
	};
	char mac_addr[25]={0};
	char phy_port[5]={0};
	int buf_size = sizeof(str_flag);
	char *buf = kzalloc(buf_size,GFP_KERNEL);

	if(!buf){
		printk("!!allocating buffer failed!!");
		return ;
	}

	printk("entry no: %u\n",pos);
	/*
	 * printout these info
	 * name(netif_ptr) :mac_addr - status - users= <user count> - phy_port= <port no>
	*/
	if ( (ifinfo->flags & NETIF_MAC_AVAILABLE) )
		snprintf(mac_addr,sizeof(mac_addr),"%02x:%02x:%02x:%02x:%02x:%02x",
						(u32)ifinfo->mac[0], (u32)ifinfo->mac[1], (u32)ifinfo->mac[2],
						(u32)ifinfo->mac[3], (u32)ifinfo->mac[4], (u32)ifinfo->mac[5]);
	else
		snprintf(mac_addr,sizeof(mac_addr),"N/A");

	if ( (ifinfo->flags & NETIF_PHYS_PORT_GOT) )
		snprintf(phy_port,sizeof(phy_port),"%u\n",ifinfo->phys_port);
	else
		snprintf(phy_port,sizeof(phy_port),"N/A\n");

	printk("%s (%p) : %s - %s - users= %d - phys_port=%s",ifinfo->name, ifinfo->netif,
					mac_addr, (ifinfo->enable ? "enabled" : "disabled"),
					ppa_atomic_read(&ifinfo->count), phy_port);

	/*
	 * Printing the VLAN informations
	*/
	printk("    vlan_info: \n");
	printk("        %-17s = %u\n", "vlan_layer", ifinfo->vlan_layer);
	if ( (ifinfo->flags & NETIF_VLAN_INNER) )
		printk("        %-17s = 0x%x\n","inner_vid", ifinfo->inner_vid);
	if ( (ifinfo->flags & NETIF_VLAN_OUTER) )
		printk("        %-17s = 0x%x\n", "outer_vid", ifinfo->outer_vid);
	if( ifinfo->out_vlan_netif )
		printk("        %-17s = %s\n", "out_vlan_if", ppa_get_netif_name(ifinfo->out_vlan_netif) );
	if( ifinfo->in_vlan_netif )
		printk("        %-17s = %s\n", "in_vlan_if", ppa_get_netif_name(ifinfo->in_vlan_netif) );

	/*
	 * Priting the mac entry
	*/
	if ( ifinfo->mac_entry == ~0 )
		printk("    mac_entry: N/A\n");
	else
		printk("    mac_entry: %u\n",ifinfo->mac_entry);

	/*
	 * Printing misc info
	*/
	printk("    misc info:\n");
	if ( (ifinfo->flags & NETIF_PPPOE) )
		printk("        %-17s = %u\n", "pppoe_session_id", ifinfo->pppoe_session_id);
	if( ifinfo->brif )
		printk("        %-17s = %s\n", "Bridge name", ppa_get_netif_name(ifinfo->brif) );
#if defined(PPA_IF_MIB) && PPA_IF_MIB
	for(i=0; i < ifinfo->sub_if_index; i++) {
		strncat(buf, ifinfo->sub_if_name[i],PPA_IF_NAME_SIZE);
		strncat(buf," ",1);
	}
	if (ifinfo->sub_if_index)
		printk("        %-17s = %s\n", "upper_interfaces", buf);

	/*
	 * Printing counter values
	*/
	printk("    HW MIB rx/tx : %llu/%llu (%u:%llu/%u:%llu)\n",
		ifinfo->hw_accel_stats.rx_bytes - ifinfo->prev_clear_acc_rx,
		ifinfo->hw_accel_stats.tx_bytes - ifinfo->prev_clear_acc_tx,
		ifinfo->hw_accel_stats.rx_bytes, ifinfo->prev_clear_acc_rx,
		ifinfo->hw_accel_stats.tx_bytes, ifinfo->prev_clear_acc_tx);
#endif /* PPA_IF_MIB */

	/*
	 * Printing the flags in strings
	*/
	memset(buf,0,sizeof(str_flag));
	for ( bit = 1, i = 0; i < sizeof(str_flag) / sizeof(*str_flag); bit <<= 1, i++ )
		if ( (ifinfo->flags & bit) ){
			if ( flag++ )
				strncat(buf," | ",3);
			strncat(buf,str_flag[i],MAX_FLAG_SIZE);
		}

	if ( flag )
		strncat(buf,"\n",1);
	else
		snprintf(buf,buf_size,"NULL\n");

	printk("    flags: %s",buf);
	kfree(buf);
}

static ssize_t proc_file_write_netif(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	char *p;

	PPA_IFINFO ifinfo = {0};
	int f_is_lanif = -1;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}
	len = min(count, (size_t)sizeof(str) - 1);
	len -= ppa_copy_from_user(str, buf, len);
	while ( len && str[len - 1] <= ' ' )
		len--;
	str[len] = 0;
	for ( p = str; *p && *p <= ' '; p++, len-- );
	if ( !*p )
		return count;

	if ( strincmp(p, "add ", 4) == 0 ) {
		p += 4;

		while ( *p && *p <= ' ' )
			p++;

		if ( strincmp(p, "lan ", 4) == 0 )
			f_is_lanif = 1;
		else if ( strincmp(p, "wan ",4) == 0 )
			f_is_lanif = 0;

		if ( f_is_lanif >= 0 ) {
			p += 4;

			while ( *p && *p <= ' ' )
				p++;

			ifinfo.ifname = p;
			ifinfo.if_flags = f_is_lanif ? PPA_F_LAN_IF : 0;
			ppa_add_if(&ifinfo, 0);
		}
	} else if ( strincmp(p, "del ", 4) == 0 ) {
		p += 4;

		while ( *p && *p <= ' ' )
			p++;

		if ( strincmp(p, "lan ", 4) == 0 )
			f_is_lanif = 1;
		else if ( strincmp(p, "wan ",4) == 0 )
			f_is_lanif = 0;

		if ( f_is_lanif >= 0 ) {
			p += 4;

			while ( *p && *p <= ' ' )
				p++;

			ifinfo.ifname = p;
			ifinfo.if_flags = f_is_lanif ? PPA_F_LAN_IF : 0;
			ppa_del_if(&ifinfo, 0);
		} else {
			ifinfo.ifname = p;
			ifinfo.if_flags = 0;
			ppa_del_if(&ifinfo, 0);
			ifinfo.if_flags = PPA_F_LAN_IF;
			ppa_del_if(&ifinfo, 0);
		}
	} else if ( strincmp(p, "update ", 7) == 0 ) {
		p += 7;

		while ( *p && *p <= ' ' )
			p++;

		if ( *p ) {
			int32_t ret = ppa_netif_update(NULL, p);

			if ( ret == PPA_SUCCESS )
				printk("Successfully\n");
			else
				pr_err("Failed: %d\n", ret);
		}
	} else if ( strincmp(p, "show ", 5) == 0) {
		p+=5;

		while ( *p && *p <= ' ' )
			p++;

		if (*p) {
			if ( strncmp(p,"all", 3) == 0)
				ppa_netif_display_all();
			else
				ppa_netif_display_single(p);
		}
		else
			goto PRINT_USAGE;

	}
	else
		goto PRINT_USAGE;

	return count;

PRINT_USAGE:
	printk("Usage:\n");
	printk("Add LAN/WAN interface: echo add lan/wan if-name  > /proc/ppa/api/netif\n");
	printk("Delete LAN/WAN interface: echo del lan/wan if-name  > /proc/ppa/api/netif\n");
	printk("Update interface : echo update if-name > /proc/ppa/api/netif\n");
	printk("Display single interface information:  echo show if-name > /proc/ppa/api/netif\n");
	printk("Display all interface information: echo show all > /proc/ppa/api/netif\n");

	return count;

}

static void *proc_read_session_seq_start(struct seq_file *seq, loff_t *ppos)
{
	struct uc_session_node *p_rt_item;
	struct mc_session_node *p_mc_item;
	struct br_mac_node *p_br_item;
	uint32_t type;

	if ( *ppos == 0 ) {
		g_proc_read_session_pos = 0;
		g_proc_read_session_pos_prev = ~0;
	} else
		g_proc_read_session_pos = g_proc_read_session_pos_prev;
	type = g_proc_read_session_pos & 0x30000000;
	g_proc_read_session_pos &= 0x0FFFFFFF;
	switch ( type ) {
	case 0x00000000:
		if ( ppa_session_start_iteration(&g_proc_read_session_pos, &p_rt_item) == PPA_SUCCESS ) {
			*ppos = g_proc_read_session_pos;
			return p_rt_item;
		} else {
			ppa_session_stop_iteration();
			g_proc_read_session_pos = 0;
		}
	case 0x10000000:
		if ( ppa_mc_group_start_iteration(&g_proc_read_session_pos, &p_mc_item) == PPA_SUCCESS ) {
			g_proc_read_session_pos |= 0x10000000;
			*ppos = g_proc_read_session_pos;
			return p_mc_item;
		} else {
			ppa_mc_group_stop_iteration();
			g_proc_read_session_pos = 0;
		}
	case 0x20000000:
		if ( ppa_bridging_session_start_iteration(&g_proc_read_session_pos, &p_br_item) == PPA_SUCCESS ) {
			g_proc_read_session_pos |= 0x20000000;
			*ppos = g_proc_read_session_pos;
			return p_br_item;
		} else {
			g_proc_read_session_pos |= 0x20000000;
			*ppos = g_proc_read_session_pos;
		}
	}

	return NULL;
}

static void *proc_read_session_seq_next(struct seq_file *seq, void *v, loff_t *ppos)
{
	uint32_t type;

	g_proc_read_session_pos_prev = (uint32_t)*ppos;
	g_proc_read_session_pos = (uint32_t)*ppos;
	type = g_proc_read_session_pos & 0x30000000;
	g_proc_read_session_pos &= 0x0FFFFFFF;
	switch ( type )
	{
	case 0x00000000:
		if ( ppa_session_iterate_next(&g_proc_read_session_pos,
			(struct uc_session_node **)&v) == PPA_SUCCESS ) {
			*ppos = g_proc_read_session_pos;
			return v;
		} else {
			ppa_session_stop_iteration();
			g_proc_read_session_pos = 0;
			if ( ppa_mc_group_start_iteration(&g_proc_read_session_pos, (
				struct mc_session_node **)&v) == PPA_SUCCESS ) {
				g_proc_read_session_pos |= 0x10000000;
				*ppos = g_proc_read_session_pos;
				return v;
			}
			ppa_mc_group_stop_iteration();
			g_proc_read_session_pos = 0;
			if ( ppa_bridging_session_start_iteration(&g_proc_read_session_pos,
					(struct br_mac_node **)&v) == PPA_SUCCESS ) {
				g_proc_read_session_pos |= 0x20000000;
				*ppos = g_proc_read_session_pos;
				return v;
			}
			g_proc_read_session_pos |= 0x20000000;
			*ppos = g_proc_read_session_pos;
			return NULL;
		}
		break;
	case 0x10000000:
		if ( ppa_mc_group_iterate_next(&g_proc_read_session_pos,
				(struct mc_session_node **)&v) == PPA_SUCCESS ) {
			g_proc_read_session_pos |= 0x10000000;
			*ppos = g_proc_read_session_pos;
			return v;
		} else {
			ppa_mc_group_stop_iteration();
			g_proc_read_session_pos = 0;
			if ( ppa_bridging_session_start_iteration(&g_proc_read_session_pos,
				(struct br_mac_node **)&v) == PPA_SUCCESS ) {
				g_proc_read_session_pos |= 0x20000000;
				*ppos = g_proc_read_session_pos;
				return v;
			}
			g_proc_read_session_pos |= 0x20000000;
			*ppos = g_proc_read_session_pos;
			return NULL;
		}
		break;
	case 0x20000000:
		if ( ppa_bridging_session_iterate_next(&g_proc_read_session_pos,
			(struct br_mac_node **)&v) == PPA_SUCCESS ) {
			g_proc_read_session_pos |= 0x20000000;
			*ppos = g_proc_read_session_pos;
			return v;
		} else {
			g_proc_read_session_pos |= 0x20000000;
			*ppos = g_proc_read_session_pos;
			return NULL;
		}
		break;
	default:
		return NULL;
	}
}

static void proc_read_session_seq_stop(struct seq_file *seq, void *v)
{
	switch ( g_proc_read_session_pos & 0x30000000 ) {
	case 0x00000000: ppa_session_stop_iteration(); break;
	case 0x10000000: ppa_mc_group_stop_iteration(); break;					 
	case 0x20000000: ppa_bridging_session_stop_iteration();
	}
}

static void printk_session_ifid(char *str, uint32_t ifid)
{
	const char *pstr =  ifid < NUM_ENTITY(g_str_dest) ? g_str_dest[ifid] : "INVALID";

	printk(str);
	printk("%u - %s\n", ifid, pstr);
}

static void printk_session_flags(char *str, uint32_t flags)
{
	static const char *str_flag[] = {
		"IS_REPLY",                 /*  0x00000001*/
		"Reserved",
		"SESSION_IS_TCP",
		"STAMPING",
		"ADDED_IN_HW",              /*  0x00000010 */
		"NOT_ACCEL_FOR_MGM",
		"STATIC",
		"DROP",
		"VALID_NAT_IP",             /*  0x00000100 */
		"VALID_NAT_PORT",
		"VALID_NAT_SNAT",
		"NOT_ACCELABLE",
		"VALID_VLAN_INS",           /*  0x00001000 */
		"VALID_VLAN_RM",
		"SESSION_VALID_OUT_VLAN_INS",
		"SESSION_VALID_OUT_VLAN_RM",
		"VALID_PPPOE",              /*  0x00010000 */
		"VALID_NEW_SRC_MAC",
		"VALID_MTU",
		"VALID_NEW_DSCP",
		"SESSION_VALID_DSLWAN_QID", /*  0x00100000 */
		"SESSION_TX_ITF_IPOA",
		"SESSION_TX_ITF_PPPOA",
		"Reserved",
		"SRC_MAC_DROP_EN",          /*  0x01000000 */
		"SESSION_TUNNEL_6RD",
		"SESSION_TUNNEL_DSLITE",
		"Reserved",
		"LAN_ENTRY",                /*  0x10000000 */
		"WAN_ENTRY",
		"IPV6",
		"ADDED_IN_SW",
	};

	int flag;
	unsigned long bit;
	int i;

	printk(str);

	flag = 0;
	for (bit = 1, i = 0; bit; bit <<= 1, i++)
		if ((flags & bit)) {
			if (flag++)
				printk("| ");
			printk(str_flag[i]);
		}
	if (flag)
		printk("\n");
	else
		printk("NULL\n");
}

#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
static void printk_session_debug_flags(char *str, uint32_t flags)
{
	static const char *str_flag[] = {
		"NOT_REACH_MIN_HITS",           /*  0x00000001 */
		"ALG",
		"ZERO_DST_MAC",
		"TCP_NOT_ESTABLISHED",
		"RX_IF_NOT_IN_IF_LIST",         /*  0x00000010 */
		"TX_IF_NOT_IN_IF_LIST",
		"RX_IF_UPDATE_FAIL",
		"TX_IF_UPDATE_FAIL",
		"SRC_BRG_IF_NOT_IN_BRG_TBL",    /*  0x00000100 */
		"SRC_IF_NOT_IN_IF_LIST",
		"DST_BRG_IF_NOT_IN_BRG_TBL",
		"DST_IF_NOT_IN_IF_LIST",
		"ADD_PPPOE_ENTRY_FAIL",         /*  0x00001000 */
		"ADD_MTU_ENTRY_FAIL",
		"ADD_MAC_ENTRY_FAIL",
		"ADD_OUT_VLAN_ENTRY_FAIL",
		"RX_PPPOE",                     /*  0x00010000 */
		"TX_PPPOE",
		"TX_BR2684_EOA",
		"TX_BR2684_IPOA",
		"TX_PPPOA",                     /*  0x00100000 */
		"GET_DST_MAC_FAIL",
		"RX_INNER_VLAN",
		"RX_OUTER_VLAN",
		"TX_INNER_VLAN",                /*  0x01000000 */
		"TX_OUTER_VLAN",
		"RX_VLAN_CANT_SUPPORT",
		"TX_VLAN_CANT_SUPPORT",
		"UPDATE_HASH_FAIL",             /*  0x10000000 */
		"PPE Limitation",
		"INVALID",
		"INVALID",
	};

	int flag;
	unsigned long bit;
	int i;

	printk(str);

	flag = 0;
	for (bit = 1, i = 0; bit; bit <<= 1, i++)
		if ((flags & bit)) {
			if (flag++)
				printk("| ");
			printk(str_flag[i]);
			printk(" ");
		}
	if (flag)
		printk("\n");
	else
		printk("NULL\n");

}
#endif

static int proc_read_routing_mini_session_seq_show(struct seq_file *seq, void *v)
{
	struct uc_session_node *p_item = v;
	uint32_t pos = g_proc_read_session_pos & 0x0FFFFFFF;
	int8_t strbuf1[64], strbuf2[64], strbuf3[64];

	if (pos == 1) {
		seq_printf(seq, "===================================================================================================================================================\n");
		seq_printf(seq, "\t\t\t\t\t\tSession List (Unicast Routing)\n");
		seq_printf(seq, "===================================================================================================================================================\n");
		seq_printf(seq, "   No.|         Session|          Src_IP|          Dst_IP|          Nat_IP|  Src_Port| Dst_Port| Proto| HW Flg| SW Flg|   Rx_If|   Tx_If| HW_Idx|\n");
		seq_printf(seq, "===================================================================================================================================================\n");
	}
	seq_printf(seq,
	" %5d| %16px| %15s| %15s| %15s| %8u| %8u| %6s| %7s| %7s| %8s| %8s| %8u|\n",
	pos,
	p_item->session,
	ppa_get_pkt_ip_string(p_item->pkt.src_ip, p_item->flags & SESSION_IS_IPV6, strbuf1),
	ppa_get_pkt_ip_string(p_item->pkt.dst_ip, p_item->flags & SESSION_IS_IPV6, strbuf2),
	ppa_get_pkt_ip_string(p_item->pkt.nat_ip, p_item->flags & SESSION_IS_IPV6, strbuf3),
	(uint32_t)p_item->pkt.src_port,
	(uint32_t)p_item->pkt.dst_port,
	((p_item->pkt.ip_proto == PPA_IPPROTO_TCP) ?" TCP" : ((p_item->pkt.ip_proto == PPA_IPPROTO_UDP)? " UDP" : " ESP" )),
	(p_item->flags & (SESSION_ADDED_IN_HW) ? "Yes" : " No"),
	(p_item->flags & (SESSION_ADDED_IN_SW) ? "Yes" : " No"),
	(p_item->rx_if == NULL ? "N/A" : ppa_get_netif_name(p_item->rx_if)),
	(p_item->tx_if == NULL ? "N/A" : ppa_get_netif_name(p_item->tx_if)),
#if IS_ENABLED(CONFIG_SOC_GRX500)
	(p_item->flags & (SESSION_ADDED_IN_HW) ? p_item->sess_hash : 0)
#else
	(p_item->flags & (SESSION_ADDED_IN_HW) ? p_item->routing_entry : 0)
#endif /*IS_ENABLED(CONFIG_SOC_GRX500)*/
	);
	return 0;
}

static int proc_read_mc_mini_group_seq_show(struct seq_file *seq, void *v)
{
	struct mc_session_node *p_item = v;
	uint32_t pos = g_proc_read_session_pos & 0x0FFFFFFF, i;
	char mc_entry[STRBUF_SMALL_LEN], src_mac_entry[STRBUF_SMALL_LEN];
	int8_t strbuf[64];
	char mc_client[STRBUF_LEN] = {0};

	if (pos == 1) {
		seq_printf(seq, "===================================================================================================================================================\n");
		seq_printf(seq, "\t\t\t\t\t\tSession List (Multicast Routing)\n");
		seq_printf(seq, "===================================================================================================================================================\n");
		seq_printf(seq, "      No.| Mc_hlist.next|     Mode|     IP_mc_group|    Src_Interface| Mc_entry| Src_mac_entry| HW Flag| SW Flag | EG netdevices |\n");
		seq_printf(seq, "===================================================================================================================================================\n");
	}

        if (p_item->mc_entry == ~0)
                snprintf(mc_entry, sizeof(mc_entry), "N/A");
        else
                snprintf(mc_entry, sizeof(mc_entry), "%d", p_item->mc_entry);

        snprintf(src_mac_entry, sizeof(src_mac_entry), "N/A");

	for (i = 0; i < PPA_MAX_MC_IFS_NUM && ((strlen(mc_client) + PPA_IF_NAME_SIZE +1) < STRBUF_LEN); i++) {
		if ((p_item->grp.if_mask & (1 << i)) && p_item->grp.txif[i].netif != NULL) {
			strncat( mc_client, ppa_get_netif_name(p_item->grp.txif[i].netif), PPA_IF_NAME_SIZE);
			strcat(mc_client," ");
		}
	}

	seq_printf(seq,
	" %8d| %13p| %8s| %3s | %16s| %8s| %13s| %7s| %7s | %s |\n",
	pos,
	p_item->mc_hlist.next,
	p_item->bridging_flag ? "bridging" : "routing",
	ppa_get_pkt_ip_string(p_item->grp.ip_mc_group.ip, p_item->grp.ip_mc_group.f_ipv6, strbuf),
	p_item->grp.src_netif ? ppa_get_netif_name(p_item->grp.src_netif) : "N/A",
	mc_entry,
	src_mac_entry,
	(p_item->flags & (SESSION_ADDED_IN_HW) ? "Yes" : " No"),
	(p_item->flags & (SESSION_ADDED_IN_SW) ? "Yes" : " No"),
	mc_client);

	return 0;
}

static int proc_read_bridging_mini_session_seq_show(struct seq_file *seq,
						    void *v)
{
	struct br_mac_node *p_item = v;
	uint32_t pos = g_proc_read_session_pos & 0x0FFFFFFF;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}

	if (pos == 1) {
		seq_printf(seq, "===================================================================================================================================================\n");
		seq_printf(seq, "\t\t\t\t\t\tSession List (Bridging)\n");
		seq_printf(seq, "===================================================================================================================================================\n");
		seq_printf(seq, "      No.| Br_hlist|            Mac[6]|            Netif|        dest_ifid|  dslwan_qid| HW Flag| SW Flag| Timeout\n");
		seq_printf(seq, "===================================================================================================================================================\n");
	}
	seq_printf(seq,
	" %8d| %px| %02x:%02x:%02x:%02x:%02x:%02x| %16s| %16d| %11u| %7s| %7s| %7d\n",
	pos,
	&p_item->br_hlist,
	(uint32_t)p_item->mac[0],
	(uint32_t)p_item->mac[1],
	(uint32_t)p_item->mac[2],
	(uint32_t)p_item->mac[3],
	(uint32_t)p_item->mac[4],
	(uint32_t)p_item->mac[5],
	(p_item->netif == NULL ? "N/A" : ppa_get_netif_name(p_item->netif)),
	(uint32_t)p_item->dest_ifid,
	(uint32_t)p_item->dslwan_qid,
	(p_item->flags & (SESSION_ADDED_IN_HW) ? "Yes" : " No"),
	(p_item->flags & (SESSION_ADDED_IN_SW) ? "Yes" : " No"),
	p_item->timeout);

	return 0;

}

static int proc_read_mini_session_seq_show(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	switch (g_proc_read_session_pos & 0x30000000) {
		case 0x00000000: return proc_read_routing_mini_session_seq_show(seq, v);
		case 0x10000000: return proc_read_mc_mini_group_seq_show(seq, v);
		case 0x20000000: return proc_read_bridging_mini_session_seq_show(seq, v);
		default:         return 0;
	}
}
 
static int proc_read_mini_session_seq_open(struct inode *inode, struct file *file)
{
    return seq_open(file, &g_proc_read_mini_session_seq_ops);
}

int ppa_split_buffer(char *buffer, char *array[], int max_param_num)
{
	int i, set_copy = 0;
	int res = 0;
	int len;

	for (i = 0; i < max_param_num; i++)
		array[i] = NULL;
	if (!buffer)
		return 0;
	len = strlen(buffer);
	for (i = 0; i < max_param_num;) {
		while (len && ((*buffer == ' ') || (*buffer == '\r') || (*buffer == '\r'))) {
			buffer++;
			len--;
		}
		for (;
				*buffer != ' ' && *buffer != '\0' && *buffer != '\r' &&
				*buffer != '\n' && *buffer != '\t'; buffer++, len--) {
			/*Find first valid charactor */
			set_copy = 1;
			if (!array[i])
				array[i] = buffer;
		}

		if (set_copy == 1) {
			i++;
			if (*buffer == '\0' || *buffer == '\r' ||
					*buffer == '\n') {
				*buffer = 0;
				break;
			}
			*buffer = 0;
			buffer++;
			len--;
			set_copy = 0;

		} else {
			if (*buffer == '\0' || *buffer == '\r' ||
					*buffer == '\n')
				break;
			buffer++;
			len--;
		}
	}
	res = i;

	return res;
}

static ssize_t proc_file_write_mini_session(struct file *file, const char __user *buf, size_t count,
                            loff_t *ppos)
{
	int len, num;
	char str[64];
	char *param_list[5];
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

        if (!capable(CAP_NET_ADMIN)) {
                printk ("Write Permission denied\n");
                return 0;
        }

	if (!len)
		return count;

	num = ppa_split_buffer(str, param_list, ARRAY_SIZE(param_list));

	if (num == 1) {
		if (stricmp(param_list[0], "u") == 0) {
			ppa_uc_session_display_all();
			return count;
		} else if (stricmp(param_list[0], "c") == 0) {
			ppa_uc_session_delete_all();
			printk("All ppa unicast sessions just got cleared !\n");
			memset(session_buf, 0, sizeof(session_buf));
			return count;
		} else {
			goto PRINT_USAGE;
		}
	} else if (num != 2) {
		goto PRINT_USAGE;
	}

	if (stricmp(param_list[0], "u") == 0) {
		snprintf(session_buf, sizeof(session_buf), "%s", param_list[1]);
		if(!ppa_session_find_unicast_routing(session_buf))
			printk("No Unicast Routing Session with session-id = %s is present\n", session_buf);
		memset(session_buf, 0, sizeof(session_buf));
	} else if (stricmp(param_list[0], "m") == 0) {
		snprintf(session_buf, sizeof(session_buf), "%s", param_list[1]);
		if(!ppa_session_find_multicast_routing(session_buf))
			printk("No Multicast Routing Session with mc_hlist.next = %s is present\n", session_buf);
		memset(session_buf, 0, sizeof(session_buf));
	} else if (stricmp(param_list[0], "b") == 0) {
		snprintf(session_buf, sizeof(session_buf), "%s", param_list[1]);
		if(!ppa_session_find_bridging(session_buf))
			printk("No Bridging Session with session-id = %s is present\n", session_buf);
		memset(session_buf, 0, sizeof(session_buf));
	}
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
	else if (stricmp(param_list[0], "s") == 0 && stricmp(param_list[1], "u") == 0) {
		snprintf(session_buf, sizeof(session_buf), "%s", param_list[2]);
		if (!ppa_session_find_unicast_routing_sw_hdr(session_buf))
			printk("No Unicast Routing Session with session-id = %s is present\n", session_buf);
		memset(session_buf, 0, sizeof(session_buf));
	}
#endif
	else
		goto PRINT_USAGE;

	return count;

PRINT_USAGE:
	printk("Usage:\n");
	printk("Clear all the unicast sessions: echo c > /proc/ppa/api/session\n");
	printk("Display unicast routing session information:  echo u session-id > /proc/ppa/api/session\n");
	printk("Display all the unicast sessions information: echo u > /proc/ppa/api/session\n");
	printk("Display multicast routing session information:  echo m mc_hlist.next > /proc/ppa/api/session\n");
	printk("Display bridging session information:  echo b br_hlist > /proc/ppa/api/session\n");
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
	printk("Display software accelerated header information for unicast routing session:  echo s u session-id > /proc/ppa/api/session\n");
#endif
	return count;

}

int ppa_uc_session_display_all(void)
{
	struct uc_session_node *pp_item = NULL;
	uint32_t pos = 0, match = 0;

	if (ppa_session_start_iteration(&pos, &pp_item) == PPA_SUCCESS) {
		printk("=================================================================================================================\n");
		printk("                                     Unicast Routing Session(s)\n");
		printk("=================================================================================================================\n");
		do {
			printk("    No. %d\n", ++match);
			printk("=================================================================================================================\n");
			ppa_unicast_routing_print_session(pp_item);
			printk("=================================================================================================================\n");
		} while(ppa_session_iterate_next(&pos, &pp_item) == PPA_SUCCESS);
	}

	ppa_session_stop_iteration();
	return match;
}

static int ppa_uc_session_delete_all(void)
{
	struct uc_session_node *pp_item = NULL;
	uint32_t pos = 0;

	if (ppa_session_start_iteration(&pos, &pp_item) == PPA_SUCCESS) {
		do {
			__ppa_session_put(pp_item);
			__ppa_session_delete_item(pp_item);
		} while(ppa_session_iterate_next(&pos, &pp_item) == PPA_SUCCESS);
	}
	ppa_session_stop_iteration();
	return 0;
}

int ppa_session_find_unicast_routing(char *session_buf_unicast)
{
	struct uc_session_node *pp_item = NULL;
	uint32_t pos = 0, match = 0;
	long i = 0;

	if (ppa_session_start_iteration(&pos, &pp_item) == PPA_SUCCESS) {
		printk("=================================================================================================================\n");
		printk("                                     Unicast Routing Session(s)\n");
		printk("=================================================================================================================\n");
		do {
			if (!kstrtol(session_buf_unicast, 16, &i) && (void*)pp_item->session == (void*)i) {
				printk("    No. %d\n", ++match);
				printk("=================================================================================================================\n");
				ppa_unicast_routing_print_session(pp_item);
				printk("=================================================================================================================\n");
			}
		} while(ppa_session_iterate_next(&pos, &pp_item) == PPA_SUCCESS);
	}

	ppa_session_stop_iteration();
	return match;
}

int ppa_session_find_unicast_routing_sw_hdr(char *session_buf_unicast)
{
	struct uc_session_node *pp_item = NULL;
	uint32_t pos = 0, present = 0;
	long i = 0;
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
	uint32_t match = 0;
#endif
	if (ppa_session_start_iteration(&pos, &pp_item) == PPA_SUCCESS) {
		printk("=================================================================================================================\n");
		printk("                                     Unicast Routing Session(s) Sw Accel header\n");
		printk("=================================================================================================================\n");
		do {
			if (!kstrtol(session_buf_unicast, 16, &i) && (void*)pp_item->session == (void*)i) {
				present = 1;
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
				if (pp_item->flags & (SESSION_ADDED_IN_SW)) {
					printk("    No. %d\n", ++match);
					printk("=================================================================================================================\n");
					ppa_unicast_routing_print_session_sw_hdr(pp_item);
					printk("=================================================================================================================\n");
				} else {
					printk("Unicast Routing Session with Session ID %px is not Added in Software\n", pp_item->session);
				}
#endif /*IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)*/
			}
		} while (ppa_session_iterate_next(&pos, &pp_item) == PPA_SUCCESS);
	}

	ppa_session_stop_iteration();
	return present;
}

int ppa_session_find_multicast_routing(char *session_buf_multicast)
{
	struct mc_session_node *pp_item = NULL;
	uint32_t pos = 0, match = 0;
	long i = 0;

	if (ppa_mc_group_start_iteration(&pos, &pp_item) == PPA_SUCCESS) {
		printk("=================================================================================================================\n");
		printk("                                       Multicast Routing Session(s)\n");
		printk("=================================================================================================================\n");
		do {
			if (!kstrtol(session_buf_multicast, 16, &i) && (void*)pp_item->mc_hlist.next == (void*)i) {
				printk("    No. %d\n", ++match);
				printk("=================================================================================================================\n");
				ppa_multicast_routing_print_session(pp_item);
				printk("=================================================================================================================\n");
			}
		} while (ppa_mc_group_iterate_next(&pos, &pp_item) == PPA_SUCCESS);
	}

	ppa_mc_group_stop_iteration();
	return match;
}

int ppa_session_find_bridging(char *session_buf_bridge)
{
	struct br_mac_node *pp_item = NULL;
	uint32_t pos = 0, match = 0;
	long i = 0;

	if (ppa_bridging_session_start_iteration(&pos, &pp_item) == PPA_SUCCESS) {
		printk("=================================================================================================================\n");
		printk("                                           Bridging Session(s)\n");
		printk("=================================================================================================================\n");
		do {
			if (!kstrtol(session_buf_bridge, 16, &i) && (void*)&pp_item->br_hlist == (void*)i) {
				printk("    No. %d\n", ++match);
				printk("=================================================================================================================\n");
				ppa_bridging_print_session(pp_item);
				printk("=================================================================================================================\n");
			}
		} while(ppa_bridging_session_iterate_next(&pos, &pp_item) == PPA_SUCCESS);
	}

	ppa_bridging_session_stop_iteration();
	return match;
}

void ppa_unicast_routing_print_session(struct uc_session_node *p_item)
{
	int8_t strbuf[64];
#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
	PPA_MIB_MODE_ENABLE mib_cfg;
#endif
	printk("    next             = %px\n", &p_item->hlist);
	printk("    session          = %px\n", p_item->session);
	printk("    ip_proto         = %u\n",   (uint32_t)p_item->pkt.ip_proto);
#ifdef CONFIG_PPA_PP_LEARNING
	printk("    eth_type         = %u\n",   (uint32_t)p_item->pkt.protocol);
#endif
	printk("    ip_tos           = %u\n",   (uint32_t)p_item->pkt.ip_tos);
	printk("    src_ip           = %s\n",   ppa_get_pkt_ip_string(p_item->pkt.src_ip, p_item->flags & SESSION_IS_IPV6, strbuf));
	printk("    src_port         = %u\n",   (uint32_t)p_item->pkt.src_port);
	printk("    src_mac[6]       = %s\n",	 ppa_get_pkt_mac_string(p_item->pkt.src_mac, strbuf));
	printk("    dst_ip           = %s\n",   ppa_get_pkt_ip_string(p_item->pkt.dst_ip, p_item->flags & SESSION_IS_IPV6, strbuf));
	printk("    dst_port         = %u\n",   (uint32_t)p_item->pkt.dst_port);
	printk("    dst_mac[6]       = %s\n",   ppa_get_pkt_mac_string(p_item->pkt.dst_mac, strbuf));
	printk("    nat_ip           = %s\n",   ppa_get_pkt_ip_string(p_item->pkt.natip.natsrcip.ip, p_item->flags & SESSION_IS_IPV6, strbuf));
	printk("    nat_port         = %u\n",   (uint32_t)p_item->pkt.nat_port);
	printk("    nat_src_mac[6]   = %s\n",   ppa_get_pkt_mac_string(p_item->pkt.nat_src_mac, strbuf));
	printk("    num_adds         = %u( minimum required hit is %d)\n",   (uint32_t)p_item->num_adds, g_ppa_min_hits);
#ifdef CONFIG_MIPS
	if ((uint32_t)p_item->rx_if < KSEG0 || (uint32_t)p_item->rx_if >= KSEG1)
		printk("    rx_if            = %s (%px)\n", "N/A", p_item->rx_if);
	else
#endif
		printk("    rx_if            = %s (%px)\n", p_item->rx_if == NULL ? "N/A" : ppa_get_netif_name(p_item->rx_if), p_item->rx_if);
#ifdef CONFIG_MIPS
	if ((uint32_t)p_item->tx_if < KSEG0 || (uint32_t)p_item->tx_if >= KSEG1)
		printk("    tx_if            = %s (%px)\n", "N/A(may no ip output hook or not meet hit count)", p_item->tx_if);
	else
#endif
		printk("    tx_if            = %s (%px)\n", p_item->tx_if == NULL ? "N/A" : ppa_get_netif_name(p_item->tx_if), p_item->tx_if);
	printk("    timeout          = %u\n",   p_item->timeout);
	printk("    last_hit_time    = %u (now %u)\n",   p_item->last_hit_time, ppa_get_time_in_sec());
	printk("    new_dscp         = %u\n",   (uint32_t)p_item->pkt.new_dscp);
	printk("    pppoe_session_id = %u\n",   (uint32_t)p_item->pkt.pppoe_session_id);
	printk("    new_vci          = 0x%04X\n", (uint32_t)p_item->pkt.new_vci);
	printk("    out_vlan_tag     = 0x%08X\n", p_item->pkt.out_vlan_tag);
	printk("    mtu              = %u\n",   p_item->mtu);
	printk("    dslwan_qid       = %u (RX), %u (TX)\n",   ((uint32_t)p_item->dslwan_qid >> 8) & 0xFF, (uint32_t)p_item->dslwan_qid & 0xFF);
	printk("    skb priority     = %02u\n",   p_item->pkt.priority);
	printk("    skb mark         = %02u\n",   p_item->pkt.mark);
	printk("    reference count  = %d\n",   ppa_atomic_read(&p_item->used));
	printk_session_ifid("    dest_ifid        = ",   p_item->dest_ifid);

	printk_session_flags("    flags            = ", p_item->flags);
#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
	printk_session_debug_flags("    debug_flags      = ", p_item->debug_flags);
#endif

	if (p_item->routing_entry == ~0)
		printk("    routing_entry    = N/A\n");
	else
		printk("    routing_entry    = %u (%s)\n", p_item->routing_entry & 0x7FFFFFFF, (p_item->routing_entry & 0x80000000) ? "LAN" : "WAN");

	printk("    mips bytes       = %llu\n",   p_item->host_bytes - p_item->prev_clear_host_bytes);

#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
	ppa_drv_get_mib_mode(&mib_cfg);
	
	if (mib_cfg.session_mib_unit == 1)
		printk("    hw accel packets   = %llu(%llu:%llu)\n",   p_item->acc_bytes - p_item->prev_clear_acc_bytes, p_item->acc_bytes, p_item->prev_clear_acc_bytes);
	else
		printk("    hw accel bytes   = %llu(%llu:%llu)\n",   p_item->acc_bytes - p_item->prev_clear_acc_bytes, p_item->acc_bytes, p_item->prev_clear_acc_bytes);
	printk("    accel last/poll  = %llu/%u\n",   p_item->last_bytes, ppa_get_hit_polling_time());

#else
	printk("    hw accel bytes   = %llu(%llu:%llu)\n",   p_item->acc_bytes - p_item->prev_clear_acc_bytes, p_item->acc_bytes, p_item->prev_clear_acc_bytes);
#endif

#if defined(SESSION_STATISTIC_DEBUG) && SESSION_STATISTIC_DEBUG
	if (p_item->flag2 & SESSION_FLAG2_HASH_INDEX_DONE)
		printk("    hash table/index = %u/%u\n",   p_item->hash_table_id, p_item->hash_index);
#endif
}

#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
void ppa_unicast_routing_print_session_sw_hdr(struct uc_session_node *p_item)
{
	t_sw_hdr *swh = NULL;
	if ( p_item->session_meta != NULL) {
		swh = (t_sw_hdr *)p_item->session_meta;
		printk("    session          		= %px\n", p_item->session);
		printk("    tot_hdr_len      		= %u\n", swh->tot_hdr_len);
		printk("    transport hdr offset  	= %u\n", swh->transport_offset); 
		printk("    network hdr offset		= %u\n", swh->network_offset);
#if IS_ENABLED(CONFIG_NETWORK_EXTMARK)
		printk("    extmark          		= %u\n", swh->extmark);
#else /* CONFIG_NETWORK_EXTMARK */
		printk("    mark          		= %u\n", swh->mark);
#endif /* CONFIG_NETWORK_EXTMARK */
		printk("    type     			= %d\n", swh->type);
		printk("    tx_if		        = %s\n", (swh->tx_if == NULL ? "N/A" : ppa_get_netif_name(swh->tx_if)));
		printk("    Hexdump of Header      	=\n\n");
		print_hex_dump(KERN_INFO, "", DUMP_PREFIX_NONE, 16, 1, swh->hdr, swh->tot_hdr_len, true);
	} else {
		printk(" No Sw Accel header for %px session is present\n", p_item->session);
	}
}
#endif /*IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)*/

/* since no where using following functions, commented*/
#if 0
static unsigned char *ppa_sw_ipv4_name()
{
	return "SW_ACC_TYPE_IPV4";
}
static unsigned char *ppa_sw_ipv6_name()
{
	return "SW_ACC_TYPE_IPV6";
}
static unsigned char *ppa_sw_6rd_name()
{
	return "SW_ACC_TYPE_6RD";
}
static unsigned char *ppa_sw_dslite_name()
{
	return "SW_ACC_TYPE_DSLITE";
}
static unsigned char *ppa_sw_bridged_name()
{
	return "SW_ACC_TYPE_BRIDGED";
}
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
static unsigned char *ppa_sw_ltcp_name()
{
	return "SW_ACC_TYPE_LTCP";
}
#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
static unsigned char *ppa_sw_ltcp_lro_name()
{
	return "SW_ACC_TYPE_LTCP_LRO";
}
#endif
#endif
#endif

void ppa_multicast_routing_print_session(struct mc_session_node *p_item)
{
	int i, j;
	uint32_t bit;
	int8_t strbuf[64];
	printk("    next             = %px\n", p_item->mc_hlist.next);
	printk("    mode             = %s\n", p_item->bridging_flag ?  "bridging" : "routing");
	printk("    ip_mc_group      = %s\n", ppa_get_pkt_ip_string((PPA_IPADDR)p_item->grp.ip_mc_group.ip.ip, p_item->grp.ip_mc_group.f_ipv6, strbuf));
	printk("    interfaces       = %d\n", p_item->grp.num_ifs);
	for (i = 0; i < PPA_MAX_MC_IFS_NUM; i++)
		if ((p_item->grp.if_mask & (1 << i)) && p_item->grp.txif[i].netif != NULL)
		printk("      %d. %16s (TTL %u)\n", i, ppa_get_netif_name(p_item->grp.txif[i].netif), p_item->grp.txif[i].ttl);
		else
			printk("      %d. N/A              (mask %d, netif %s)\n", i, 
				(p_item->grp.if_mask & (1 << i)) ? 1 : 0, p_item->grp.txif[i].netif ?
					ppa_get_netif_name(p_item->grp.txif[i].netif) : "NULL");
	printk("    src_interface    = %s\n",   p_item->grp.src_netif ? ppa_get_netif_name(p_item->grp.src_netif) : "N/A");
#if IS_ENABLED(CONFIG_SOC_GRX500)
	printk("    src_mac          = %s\n", ppa_get_pkt_mac_string(p_item->s_mac, strbuf));
#endif
	printk("    new_dscp         = %04X\n", (uint32_t)p_item->grp.new_dscp);
	printk("    new_vci          = %04X\n", (uint32_t)p_item->grp.new_vci);
	printk("    out_vlan_tag     = %08X\n", p_item->grp.out_vlan_tag);
	printk("    dslwan_qid       = %u\n",   (uint32_t)p_item->dslwan_qid);

	for (i = j = 0, bit = 1; i < sizeof(p_item->dest_ifid) * 8; i++, bit <<= 1)
		if ((p_item->dest_ifid & bit)) {
			printk_session_ifid("    dest_ifid        = ", i);
		}

	printk_session_flags("    flags            = ", p_item->flags);

#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
	printk_session_debug_flags("    debug_flags      = ", p_item->debug_flags);
#endif

	if (p_item->mc_entry == ~0)
		printk("    mc_entry         = N/A\n");
	else
		printk("    mc_entry         = %d\n", p_item->mc_entry);
}

void ppa_bridging_print_session(struct br_mac_node *p_item)
{

	printk("    next             = %px\n", &p_item->br_hlist);
	printk("    mac[6]           = %02x:%02x:%02x:%02x:%02x:%02x\n", (uint32_t)p_item->mac[0], (uint32_t)p_item->mac[1], (uint32_t)p_item->mac[2], (uint32_t)p_item->mac[3], (uint32_t)p_item->mac[4], (uint32_t)p_item->mac[5]);
	printk("    netif            = %s (%px)\n", p_item->netif == NULL ? "N/A" : ppa_get_netif_name(p_item->netif), p_item->netif);
	printk("    vci              = %04X\n", (uint32_t)p_item->vci);
	printk("    new_vci          = %04X\n", (uint32_t)p_item->new_vci);
	printk("    timeout          = %d\n",   p_item->timeout);
	printk("    last_hit_time    = %d\n",   p_item->last_hit_time);
	printk("    dslwan_qid       = %d\n",   (uint32_t)p_item->dslwan_qid);

	printk_session_ifid("    dest_ifid        = ",   p_item->dest_ifid);

	printk_session_flags("    flags            = ", p_item->flags);

#if defined(ENABLE_SESSION_DEBUG_FLAGS) && ENABLE_SESSION_DEBUG_FLAGS
	printk_session_debug_flags("    debug_flags      = ", p_item->debug_flags);
#endif
	if (p_item->bridging_entry == ~0)
		printk("    bridging_entry   = N/A\n");
	else if ((p_item->bridging_entry & 0x80000000))
		printk("    bridging_entry   = %08X\n", p_item->bridging_entry);
	else
		printk("    bridging_entry   = %d\n", p_item->bridging_entry);
}

#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
static int proc_read_swfpstatus(struct seq_file *seq, void *v)
{
	uint32_t status=0;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	if(ppa_get_sw_fastpath_status_hook)
		ppa_get_sw_fastpath_status_hook(&status,0);

	seq_printf(seq, "software fastpath is = %s\n", status?"Enabled":"Disabled");
	return 0;
}

static int proc_read_swfpstatus_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_swfpstatus, NULL);
}

static ssize_t proc_write_swfpstatus(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	char *cmd;
	uint32_t status=0;

        if (!capable(CAP_NET_ADMIN)) {
                printk ("Write Permission denied");
                return 0;
        }

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
 
	/* skip spaces */
	while ( len && str[len - 1] <= ' ' )
		len--;

	str[len] = 0;
	for ( cmd = str; *cmd && (*cmd) <= ' '; cmd++, len-- );

	if( !(*cmd) )
		return count;

	if ( stricmp(cmd, "enable") == 0 ) {
		status=1;
	} else if ( stricmp(cmd, "disable") == 0 ) {
		status=0;
	} else {
		printk("echo enable/disable > /proc/ppa/api/sw_fastpath\n");
	}

	if(ppa_sw_fastpath_enable_hook) 
		ppa_sw_fastpath_enable_hook(status,0);

	return count;
}

#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
static int proc_read_tcplp_status(struct seq_file *seq, void *v)
{
	uint32_t status=0;
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	if(ppa_get_tcp_litepath_status_hook)
		ppa_get_tcp_litepath_status_hook(&status,0);

	seq_printf(seq, "TCP litpath is = %s\n", status?"Enabled":"Disabled");
	return 0;
}

static int proc_read_tcp_litepath_status_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_tcplp_status, NULL);
}

static ssize_t proc_write_tcp_litepath_status(struct file *file, const char __user *buf, 
					size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	char *cmd;
	uint32_t status=0;
        if (!capable(CAP_NET_ADMIN)) {
                printk ("Write Permission denied");
                return 0;
        }

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);

	/* skip spaces */  
	while ( len && str[len - 1] <= ' ' )
		len--;

	str[len] = 0;
	for ( cmd = str; *cmd && (*cmd) <= ' '; cmd++, len-- );

	if( !(*cmd) )
		return count;

	if ( stricmp(cmd, "enable") == 0 ) {
		status=1;
  	} else if ( stricmp(cmd, "disable") == 0 ) {
		status=0;
	} else {
		printk("echo enable/disable > /proc/ppa/api/tcp_litepath\n");
	}

	if(ppa_tcp_litepath_enable_hook)
		ppa_tcp_litepath_enable_hook(status,0);

	return count;
}
#endif
#endif

#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
static int proc_read_br_flow_learning(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq, "bridgef_flow_learning = %s\n",
		ppa_session_bridged_flow_status()?"Enabled":"Disabled");
	return 0;
}

static int proc_file_br_flow_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_br_flow_learning, NULL);
}

static ssize_t proc_file_br_flow_write(struct file *file, const char __user *buf, 
					size_t count, loff_t *ppos)
{
	int len;
	char str[64];
	char *cmd;
        if (!capable(CAP_NET_ADMIN)) {
                printk ("Write Permission denied");
                return 0;
        }

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);

	/* skip spaces */
	while ( len && str[len - 1] <= ' ' )
		len--;

	str[len] = 0;
	for ( cmd = str; *cmd && (*cmd) <= ' '; cmd++, len-- );

	if( !(*cmd) )
		return count;

	if ( stricmp(cmd, "enable") == 0 ) {
		ppa_session_bridged_flow_set_status(1);
	} else if ( stricmp(cmd, "disable") == 0 ) {
		ppa_session_bridged_flow_set_status(0);
	} else {
		printk("echo enable/disable > /proc/ppa/api/bridged_flow_learning\n");
	}
	return count;
}
#endif /*CONFIG_PPA_BR_SESS_LEARNING*/

/*
 *  string process help function
 */
static int stricmp(const char *p1, const char *p2)
{
	int c1, c2;

	while ( *p1 && *p2 ) {
		c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
		c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
		if ( (c1 -= c2) )
			return c1;
		p1++;
		p2++;
	}

	return *p1 - *p2;
}

static int strincmp(const char *p1, const char *p2, int n)
{
	int c1 = 0, c2;

	while ( n && *p1 && *p2 ) {
		c1 = *p1 >= 'A' && *p1 <= 'Z' ? *p1 + 'a' - 'A' : *p1;
		c2 = *p2 >= 'A' && *p2 <= 'Z' ? *p2 + 'a' - 'A' : *p2;
		if ( (c1 -= c2) )
			return c1;
		p1++;
		p2++;
		n--;
	}
	return n ? *p1 - *p2 : c1;
}

static unsigned int get_number(char **p, int *len, int is_hex)
{
	unsigned int ret = 0;
	int n = 0;	

	ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"enter get_number\n");
	if ( (*p)[0] == '0' && (*p)[1] == 'x' ) {
		is_hex = 1;
		(*p) += 2;
		(*len) -= 2;
	}

	if ( is_hex ) {
		while ( *len && ((**p >= '0' && **p <= '9') || (**p >= 'a' && **p <= 'f') || (**p >= 'A' && **p <= 'F')) ) {
			if ( **p >= '0' && **p <= '9' )
				n = **p - '0';
			else if ( **p >= 'a' && **p <= 'f' )
			   n = **p - 'a' + 10;
			else if ( **p >= 'A' && **p <= 'F' )
				n = **p - 'A' + 10;
			ret = (ret << 4) | n;
			(*p)++;
			(*len)--;
		}
	} else {
		while ( *len && **p >= '0' && **p <= '9' ) {
			n = **p - '0';
			ret = ret * 10 + n;
			(*p)++;
			(*len)--;
		}
	}
	
	return ret;
}

#if IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS)
static int get_token(char **p1, char **p2, int *len, int *colon)
{
	int tlen = 0;

	while ( *len && !((**p1 >= 'A' && **p1 <= 'Z') || (**p1 >= 'a' && **p1<= 'z') || (**p1 >= '0' && **p1<= '9')) ) {
		(*p1)++;
		(*len)--;
	}
	if ( !*len )
		return 0;

	if ( *colon ) {
		*colon = 0;
		*p2 = *p1;
		while ( *len && **p2 > ' ' && **p2 != ',' ) {
			if ( **p2 == ':' ) {
				*colon = 1;
				break;
			}
			(*p2)++;
			(*len)--;
			tlen++;
		}
		**p2 = 0;
	} else {
		*p2 = *p1;
		while ( *len && **p2 > ' ' && **p2 != ',' ) {
			(*p2)++;
			(*len)--;
			tlen++;
		}
		**p2 = 0;
	}

	return tlen;
}

static void ignore_space(char **p, int *len)
{
	while ( *len && (**p <= ' ' || **p == ':' || **p == '.' || **p == ',') ) {
		(*p)++;
		(*len)--;
	}
}
#endif /* IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS)*/

/*
 * ####################################
 *		   Global Function
 * ####################################
 */

/*
 * ####################################
 *		   Init/Cleanup API
 * ####################################
 */

void ppa_api_proc_file_create(void)
{
	struct proc_dir_entry *res;
	uint32_t f_incorrect_fw = 1;
	int i;
	PPA_VERSION ver={0};

	ver.index = 1;
	if( ppa_drv_get_firmware_id(&ver, 0) != PPA_SUCCESS) {
		ver.index = 0;
		ppa_drv_get_firmware_id(&ver, 0);
	}
	switch ( ver.family ) {
	case 1: /*  Danube */
	case 2: /*  Twinpass */
	case 3: /*  Amazon-SE */
		g_str_dest[0] = "ETH0";
		g_str_dest[2] = "CPU0";
		g_str_dest[3] = "EXT_INT1";
		g_str_dest[4] = "EXT_INT2";
		g_str_dest[5] = "EXT_INT3";
		g_str_dest[6] = "EXT_INT4";
		g_str_dest[7] = "EXT_INT5";
		if ( ver.itf == 1 && ver.family != 3) {
			/*  D4*/
			g_str_dest[1] = "ETH1";
			f_incorrect_fw = 0;
		} else if ( ver.itf == 2 ) {
			/*  A4*/
			g_str_dest[1] = "ATM";
			f_incorrect_fw = 0;
		}
		break;
	case 5: /* AR9  */
	case 6: /*  GR9 */
		g_str_dest[0] = "ETH0";
		g_str_dest[1] = "ETH1";
		g_str_dest[2] = "CPU0";
		g_str_dest[3] = "EXT_INT1";
		g_str_dest[4] = "EXT_INT2";
		g_str_dest[5] = "EXT_INT3";
		if ( ver.itf == 1 ) {
			/*  D5 */
			g_str_dest[6] = "EXT_INT4";
			g_str_dest[7] = "EXT_INT5";
			f_incorrect_fw = 0;
		} else if ( ver.itf == 4 ) {
			/*  A5 */
			g_str_dest[6] = "ATM_res";
			g_str_dest[7] = "ATM";
			f_incorrect_fw = 0;
		}
	case 7: /*  VR9 */
		g_str_dest[0] = "ETH0";
		g_str_dest[1] = "ETH1";
		g_str_dest[2] = "CPU0";
		g_str_dest[3] = "EXT_INT1";
		g_str_dest[4] = "EXT_INT2";
		g_str_dest[5] = "EXT_INT3";
		if ( ver.itf == 1 ) {
			/*  D5 */
			g_str_dest[6] = "EXT_INT4";
			g_str_dest[7] = "EXT_INT5";
			f_incorrect_fw = 0;
		} else if ( ver.itf == 4 ) {
			/*  A5 */
			g_str_dest[6] = "ATM_res";
			g_str_dest[7] = "ATM";
			f_incorrect_fw = 0;
		} else if ( ver.itf == 5 ) {
			/*  E5 */
			g_str_dest[6] = "PTM_res";
			g_str_dest[7] = "PTM";
			f_incorrect_fw = 0;
		}
	}
	if ( f_incorrect_fw ) {
		for ( i = 0; i < NUM_ENTITY(g_str_dest); i++ )
			g_str_dest[i] = "INVALID";
	}

	if(!g_ppa_proc_dir_flag) {
		g_ppa_proc_dir = proc_mkdir("ppa", NULL);
		g_ppa_proc_dir_flag = 1;
	}

	g_ppa_api_proc_dir = proc_mkdir("api", g_ppa_proc_dir);
	g_ppa_api_proc_dir_flag = 1;
	
#if IS_ENABLED(CONFIG_SOC_GRX500) && IS_ENABLED(CONFIG_PPA_QOS)
	res = proc_create("class2prio", 0600,
			g_ppa_api_proc_dir, &g_proc_file_classprio_seq_fops);
	res = proc_create("inggrp", 0600,
			g_ppa_api_proc_dir, &g_proc_file_inggrp_seq_fops);

	res = proc_create("filter", 0600, g_ppa_api_proc_dir, &g_proc_file_filter_seq_fops);

	res = proc_create("queue_per_port", 0600, g_ppa_api_proc_dir, &g_proc_file_queue_per_port_seq_fops);
#endif
	res = proc_create("session_filter", 0600, g_ppa_api_proc_dir, &g_proc_session_filter_seq_fops);
	res = proc_create("session", 0600, g_ppa_api_proc_dir, &g_proc_file_mini_session_seq_fops);

	res = proc_create("dbg", 0600,
			g_ppa_api_proc_dir, &g_proc_file_dbg_seq_fops);
	res = proc_create("hook", 0600,
			g_ppa_api_proc_dir, &g_proc_file_hook_seq_fops);
	res = proc_create("phys_port", 0600,
			g_ppa_api_proc_dir, &g_proc_file_phys_port_seq_fops);
	res = proc_create("netif", 0600,
			g_ppa_api_proc_dir, &g_proc_file_netif_seq_fops);
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
	res = proc_create("sess-mgmt", 0600,
			g_ppa_api_proc_dir, &g_proc_file_sess_mgmt_seq_fops);
#endif

#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
	res = proc_create("sw_fastpath", 0600,
			g_ppa_api_proc_dir, &g_proc_file_swfp_status);
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
	res = proc_create("tcp_litepath", 0600,
			g_ppa_api_proc_dir, &g_proc_file_tcp_litepath_status);
#endif
#endif

#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
	res = proc_create("bridged_flow_learning", 0600,
			g_ppa_api_proc_dir, &g_proc_file_br_flow_learning);
#endif
#if IS_ENABLED(CONFIG_SOC_GRX500)
	res = proc_create("tunnel_table", 0600,
			g_ppa_api_proc_dir, &g_proc_file_tunnel_table_seq_fops);
#endif
}

void ppa_api_proc_file_destroy(void)
{
	remove_proc_entry("session", g_ppa_api_proc_dir);
	remove_proc_entry("netif", g_ppa_api_proc_dir);
	remove_proc_entry("phys_port", g_ppa_api_proc_dir);
	remove_proc_entry("hook", g_ppa_api_proc_dir);
 
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
	remove_proc_entry("sw_fastpath", g_ppa_api_proc_dir);
#if IS_ENABLED(CONFIG_PPA_TCP_LITEPATH)
	remove_proc_entry("tcp_litepath", g_ppa_api_proc_dir);
#endif
#endif

#if IS_ENABLED(CONFIG_SOC_GRX500)
	remove_proc_entry("inggrp", g_ppa_api_proc_dir);
	remove_proc_entry("class2prio", g_ppa_api_proc_dir);
	remove_proc_entry("filter", g_ppa_api_proc_dir);
	remove_proc_entry("queue_per_port", g_ppa_api_proc_dir);
	remove_proc_entry("tunnel_table", g_ppa_api_proc_dir);
#endif
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
	remove_proc_entry("sess-mgmt", g_ppa_api_proc_dir);
#endif
#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
	remove_proc_entry("bridged_flow_learning", g_ppa_api_proc_dir); 
#endif
	remove_proc_entry("dbg", g_ppa_api_proc_dir);
	if ( g_ppa_api_proc_dir_flag ) {
		remove_proc_entry("api", g_ppa_proc_dir);
		g_ppa_api_proc_dir = NULL;
		g_ppa_api_proc_dir_flag = 0;
	}
}
