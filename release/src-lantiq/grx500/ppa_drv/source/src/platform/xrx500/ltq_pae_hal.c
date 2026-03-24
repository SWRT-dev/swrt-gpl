/******************************************************************************
**
** FILE NAME	: ltq_pae_hal.c
** PROJECT	: GRX500
** MODULES	: PPA PAE HAL	
**
** DATE		: 05 Feb 2014
** AUTHOR	: Kamal Eradath
** DESCRIPTION	: PAE hardware abstraction layer
 ** COPYRIGHT	: Copyright (c) 2020, MaxLinear, Inc.
 **               Copyright (c) 2014, Lantiq Deutschland GmbH
 **               Am Campeon 3; 85579 Neubiberg, Germany
**
**	 For licensing information, see the file 'LICENSE' in the root folder of
**	 this software module.
**
** HISTORY
** $Date		$Author		 	$Comment
** 05 FEB 2014		Kamal Eradath		Initiate Version
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
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <net/checksum.h>
#include <lantiq.h>
#include <lantiq_soc.h>
#include <linux/clk.h>
#include <net/ip_tunnels.h>
#include <linux/if_arp.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <asm/uaccess.h>
#include <net/ipv6.h>
#include <net/ip6_tunnel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

/*
 *	Chip Specific Head File
 */

#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_hal_api.h>
#include "../../ppa_api/ppa_api_netif.h"
#include "../../ppa_api/ppa_api_session.h"
#include "ltq_pae_hal.h"

#include <net/switch_api/lantiq_gsw_api.h>
#include <net/switch_api/lantiq_gsw_flow.h>

/*
 * ####################################
 *				Version No.
 * ####################################
 */
#define VER_FAMILY	0x10
#define VER_DRTYPE	0x04 /* 4: Stack/System Adaption Layer driver*/
#define VER_INTERFACE	0x1  /*	bit 0: MII 1 */
#define VER_ACCMODE	0x11 /*	bit 0: Routing &Bridging */

#define VER_MAJOR	0
#define VER_MID		0
#define VER_MINOR	2

#define SESSION_MIB_MULTIPLEXER	32
#define SESSION_RETRY_MAX	 8

/*
 * Maximum no of Meter's Available and
 * Reserved Meter ID's
 */

#define METER_ID_MAX 64
/* Meter Id 33 is used by QoS default configuration */
#define METER_ID_RESERVE 34

int32_t pae_hal_meter_cfg_set(PPA_QOS_METER_CFG *);

/* QoS Meter bitmaps per Interface
 */
static long unsigned int qos_meter = 0;

//The below two macros must be with DEFAULT_BRIDGING_TIMEOUT_IN_SEC and DEFAULT_BRIDGING_HIT_POLLING_TIME in ppa_api_session.h	
#define BRIDGING_TIMEOUT_IN_SEC		300	/*	5 minute*/
#define BRIDGING_HIT_POLLING_TIME	20	/*	20 seconds*/
/*
 *	Compilation Switch
 */
/*!
	\brief Turn on/off debugging message and disable inline optimization.
 */
#define ENABLE_DEBUG			1
/*!
	\brief Turn on/off ASSERT feature, print message while condition is not fulfilled.
 */
#define ENABLE_ASSERT			1
/*@}*/

#if defined(ENABLE_DEBUG) && ENABLE_DEBUG
	#define ENABLE_DEBUG_PRINT	1
	#define DISABLE_INLINE		1
#endif

#if defined(DISABLE_INLINE) && DISABLE_INLINE
	#define INLINE
#else
	#define INLINE			inline
#endif

#if defined(ENABLE_DEBUG_PRINT) && ENABLE_DEBUG_PRINT
	#undef	dbg
	static unsigned int pae_hal_dbg_enable = 0;
	#define dbg(format, arg...) do { if ( pae_hal_dbg_enable ) printk(KERN_WARNING ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
	#if !defined(dbg)
	#define dbg(format, arg...)
	#endif
#endif

#if defined(ENABLE_ASSERT) && ENABLE_ASSERT
	#define ASSERT(cond, format, arg...) do { if ( !(cond) ) printk(KERN_ERR __FILE__ ":%d:%s: " format "\n", __LINE__, __FUNCTION__, ##arg); } while ( 0 )
#else
	#define ASSERT(cond, format, arg...)
#endif

#ifndef NUM_ENTITY
#define NUM_ENTITY(x)	(sizeof(x) / sizeof(*(x)))
#endif

#define PPA_API_PROC 		1

#define PSEUDO_MC_ANY_SRC	~0

#define LINEAR_WMM	1

#define GSWR_OPS(type, cmd, args, ret)					\
{									\
	struct core_ops *ops = gsw_get_swcore_ops(1);			\
	if (ops)							\
		ret = ops->gsw_##type.cmd(ops, args);			\
	else {								\
		dbg("%s: Open SWAPI device FAILED !!\n", __func__);	\
		ret = GSW_statusErr;					\
	}								\
}

/*
 * ####################################
 *		Declaration
 * ####################################
 */

#define PAE_HW_METER 1

#if defined(PPA_API_PROC)
static unsigned int pae_hal_pce_enable = 1;
static void proc_read_pae_seq_stop(struct seq_file *, void *);

static void *proc_read_pae_route_seq_start(struct seq_file *, loff_t *);
static void *proc_read_pae_route_seq_next(struct seq_file *, void *, loff_t *);
static int proc_read_pae_route_seq_show(struct seq_file *, void *);
static int proc_read_pae_route_seq_open(struct inode *, struct file *);

static void *proc_read_pae_pce_seq_start(struct seq_file *, loff_t *);
static void *proc_read_pae_pce_seq_next(struct seq_file *, void *, loff_t *);
static int proc_read_pae_pce_seq_show(struct seq_file *, void *);
static int proc_read_pae_pce_seq_open(struct inode *, struct file *);
static ssize_t proc_set_pae_pce(struct file *, const char __user *, size_t , loff_t *);

static int proc_read_pae_rtstats_seq_open(struct inode *, struct file *);
static ssize_t proc_clear_pae_rtstats(struct file *, const char __user *, size_t , loff_t *);
static int proc_read_pae_accel_seq_open(struct inode *, struct file *);
static ssize_t proc_set_pae_accel(struct file *, const char __user *, size_t , loff_t *);
static int proc_read_pae_debug_seq_open(struct inode *, struct file *);
static ssize_t proc_set_pae_debug(struct file *, const char __user *, size_t , loff_t *);
#ifdef PAE_HW_METER
static unsigned int pae_hal_hw_meter_enable = 1;
static int proc_read_pae_meter_seq_open(struct inode *, struct file *);
static ssize_t proc_set_pae_meter(struct file *, const char __user *, size_t , loff_t *);
static PPA_LOCK		g_pae_meter_lock;

static void ppa_pae_meter_lock_init(void)
{
	ppa_lock_init(&g_pae_meter_lock);
}

static void ppa_pae_meter_lock_destroy(void)
{
	ppa_lock_destroy(&g_pae_meter_lock);
}

static void ppa_pae_meter_lock(void)
{
	ppa_lock_get(&g_pae_meter_lock);
}

static void ppa_pae_meter_unlock(void)
{
	ppa_lock_release(&g_pae_meter_lock);
}
#endif

static int proc_read_pae_lrotimeout_seq_open(struct inode *, struct file *);
static ssize_t proc_write_pae_lrotimeout(struct file *, const char __user *, size_t , loff_t *);

static int proc_read_pae_lrolen_seq_open(struct inode *, struct file *);
static ssize_t proc_write_pae_lrolen(struct file *, const char __user *, size_t , loff_t *);

static int proc_read_pae_lroflush_seq_open(struct inode *, struct file *);
static ssize_t proc_write_pae_lroflush(struct file *, const char __user *, size_t , loff_t *);
#endif

#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
extern int lro_start_flow (int *session_id, int timeout, int flags, struct cpumask cpumask);
extern int lro_stop_flow (int session_id, int timeout, int flags);
#endif

static int32_t init_pae_flows(void);
static int32_t uninit_pae_flows(void);
/*
 * ####################################
 *	 Global Variable
 * ####################################
 */

/*in GRX500 PAE is having 16 ports and this array provides the bit wise port mask for multicast handling*/
const static int dest_list_map[] = {0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 0x1000, 0x2000, 0x4000, 0x8000};

/* The Egress VLAN table of PAE needs to be allocated during the HAL init with some fixed range for each ports 
// Ther are 256 entries possible in Egress VLAN table and user can tune it based on their requirement 
// The static mapping below can be changed based on the requirement
// Port assignment is as { 0 = CPU, 1,2,3,4,5,6 = LAN, 7 = DSL WAN, 8,9 = internal/external WLAN, 10,11 = unused, 12 = USB LAN, 13 unused, 14 = CAPWAP port, 15 = ETH WAN}
*/
const static uint32_t vlan_tbl_size[] 	= {  18, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 0, 16};
const static uint32_t vlan_tbl_base[]	= { 236, 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 220};

/* global port mask for GSWIPR
// Initial configuratoion is as shown below
// when a new interface got added we have to make changes in this port map
// bit 0 -15 is mapped to port 0 to 15 of GSWIPR
// 0 = CPU 1 - 6 = LAN, 7 - 11 extended(opt) LAN/WAN, 12 - USB WAN(opt), 13-reserved, 14-capwap(opt), 15-ETHWAN 
*/
static uint32_t g_port_map = 0x9FFF; /*{ 1 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 }*/

static uint32_t g_us_accel_enabled = 1;
static uint32_t g_ds_accel_enabled = 1; 
static uint32_t g_ipv6_enabled = 1;

static uint16_t g_pce_rtrule_next = 1;

static uint32_t g_min_lrolen=150;	 /* default value set to 512byte */
static uint32_t g_lro_timeout = 200; /* 1micros = 0x126; default lro aggregation timeout value 200micro sec*/
static ppa_lro_entry g_lro_table[MAX_LRO_ENTRIES];
static PPA_LOCK	g_lro_table_lock;

#if defined(PPA_API_PROC)
static struct proc_dir_entry *g_ppa_paehal_proc_dir = NULL;
static int g_ppa_paehal_proc_dir_flag = 0;
static struct proc_dir_entry *g_ppa_paehal_lro_proc_dir = NULL;
static int g_ppa_paehal_lro_proc_dir_flag = 0;
static GSW_API_HANDLE gswr_handle;
#endif
#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
static uint8_t g_mib_mode = 0; /* 0-Byte mode,1-Packet mode */
#endif

static uint32_t nsess_add=0;
static uint32_t nsess_del=0;
static uint32_t nsess_del_fail=0;
static uint32_t nsess_add_succ=0;
static uint32_t nsess_add_fail_lock=0;
static uint32_t nsess_add_fail_rt_tbl_full=0;
static uint32_t nsess_add_fail_coll_full=0;
static uint32_t nsess_add_fail_mac_tbl_full=0;
static uint32_t nsess_add_fail_ip_tbl_full=0;
static uint32_t nsess_add_fail_rtp_full=0;
static uint32_t nsess_add_fail_pppoe_full=0;
static uint32_t nsess_add_fail_mtu_full=0;
static uint32_t nsess_add_fail_minus1=0;
static uint32_t nsess_add_fail_negindex=0;
static uint32_t nsess_add_fail_oth=0;

/* Classifiaction API variables*/
static struct switch_dev_class class_dev[2]={{0},{0}}; 
spinlock_t	g_class_lock;

const static struct pce_cat_conf g_gswr_cat_conf[] = { 
							{ 1, 0},	
							{ GSWR_CAT_FILTER_MAX, GSWR_CAT_FILTER_START } , 
							{ GSWR_CAT_INGQOS_MAX, GSWR_CAT_INGQOS_START } , 
							{ GSWR_CAT_VLAN_MAX, GSWR_CAT_VLAN_START }, 
							{ GSWR_CAT_FWD_MAX, GSWR_CAT_FWD_START}, 
							{ GSWR_CAT_USQOS_MAX, GSWR_CAT_USQOS_START }, 
							{ GSWR_CAT_DSQOS_MAX, GSWR_CAT_DSQOS_START }, 
							{ GSWR_CAT_MGMT_MAX, GSWR_CAT_MGMT_START }, 
							{ GSWR_CAT_LRO_MAX, GSWR_CAT_LRO_START },
							{ GSWR_CAT_TUN_MAX, GSWR_CAT_TUN_START } 
							};

const static struct pce_cat_conf g_gswl_cat_conf[] = {	{ 1, 0},
							{ GSWL_CAT_FILTER_MAX, GSWL_CAT_FILTER_START } ,
							{ 0, 0},
							{ GSWL_CAT_VLAN_MAX, GSWL_CAT_VLAN_START },
							{ GSWL_CAT_FWD_MAX, GSWL_CAT_FWD_START},
							{ GSWL_CAT_USQOS_MAX, GSWL_CAT_USQOS_START },
							{ GSWL_CAT_DSQOS_MAX, GSWL_CAT_DSQOS_START },
							{ GSWL_CAT_MGMT_MAX, GSWL_CAT_MGMT_START },
							{ 0, 0}, /* no LRO support*/
							{ 0, 0}	 /* no need of flow rules for tunnel support*/
							};

typedef enum {
	SHIFT_RIGHT=0,
	SHIFT_LEFT
} shift_direction_t;

int32_t pae_hal_add_class_rule(PPA_CLASS_RULE*);
int32_t pae_hal_del_class_rule(PPA_CLASS_RULE*);
int32_t pae_hal_get_class_rule(PPA_CLASS_RULE*);
int32_t pae_hal_mod_class_rule(PPA_CLASS_RULE*);
/* classification api variable ends*/
int32_t	del_lro_entry(uint8_t sessionid);

#if defined(PPA_API_PROC)
static struct seq_operations g_proc_read_pae_route_seq_ops = {
	.start		= proc_read_pae_route_seq_start,
	.next		= proc_read_pae_route_seq_next,
	.stop		= proc_read_pae_seq_stop,
	.show		= proc_read_pae_route_seq_show,
};

static struct seq_operations g_proc_read_pae_pce_seq_ops = {
	.start		= proc_read_pae_pce_seq_start,
	.next		= proc_read_pae_pce_seq_next,
	.stop		= proc_read_pae_seq_stop,
	.show		= proc_read_pae_pce_seq_show,
};

static struct file_operations g_proc_file_pae_route_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_pae_route_seq_open,
	.read		= seq_read,
	.llseek	 	= seq_lseek,
	.release	= seq_release,
};

static struct file_operations g_proc_file_pae_pce_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_pae_pce_seq_open,
	.read		= seq_read,
	.write		= proc_set_pae_pce,
	.llseek	 	= seq_lseek,
	.release	= seq_release,
};

static struct file_operations g_proc_file_pae_rtstats_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_pae_rtstats_seq_open,
	.read		= seq_read,
	.write		= proc_clear_pae_rtstats,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};

static struct file_operations g_proc_file_pae_accel_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_pae_accel_seq_open,
	.read		= seq_read,
	.write		= proc_set_pae_accel,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};

static struct file_operations g_proc_file_pae_debug_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_pae_debug_seq_open,
	.read		= seq_read,
	.write		= proc_set_pae_debug,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};

#ifdef PAE_HW_METER
static struct file_operations g_proc_file_pae_meter_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_pae_meter_seq_open,
	.read		= seq_read,
	.write		= proc_set_pae_meter,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};
#endif

static struct file_operations g_proc_file_pae_lrolen_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_pae_lrolen_seq_open,
	.read		= seq_read,
	.write		= proc_write_pae_lrolen,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};

static struct file_operations g_proc_file_pae_lroflush_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_pae_lroflush_seq_open,
	.read		= seq_read,
	.write		= proc_write_pae_lroflush,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};

static struct file_operations g_proc_file_pae_lrotimeout_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_pae_lrotimeout_seq_open,
	.read		= seq_read,
	.write		= proc_write_pae_lrotimeout,
	.llseek	 	= seq_lseek,
	.release	= single_release,
};

/*
 * ####################################
 *		Extern Variable
 * ####################################
 */
extern int g_ppa_proc_dir_flag;
extern struct proc_dir_entry *g_ppa_proc_dir;
#endif

extern ppa_tunnel_entry*	g_tunnel_table[MAX_TUNNEL_ENTRIES];

/*
 * ####################################
 *			Extern Function
 * ####################################
 */

extern uint32_t ppa_drv_generic_hal_register(uint32_t hal_id, ppa_generic_hook_t generic_hook);
extern void ppa_drv_generic_hal_deregister(uint32_t hal_id);

extern uint32_t ppa_drv_register_cap(PPA_API_CAPS cap, uint8_t wt, PPA_HAL_ID hal_id);
extern uint32_t ppa_drv_deregister_cap(PPA_API_CAPS cap, PPA_HAL_ID hal_id);

/*
 * ####################################
 *			Local Function
 * ####################################
 */

#ifdef PAE_HW_METER
#define	DEFAULT_POLLING_TIME	1		 /* 1 second */
static	PPA_TIMER		g_hit_meter_timer;
static	uint32_t		g_polling_time = DEFAULT_POLLING_TIME;

struct meter_table {
	uint8_t meter_id;
	uint8_t ref_count;
} meterTable[PPA_MAX_IFS_NUM][PPA_MAX_IFS_NUM];

struct iface_index {
	PPA_IFNAME ifname[PPA_IF_NAME_SIZE];
	unsigned long cnt;
};
struct iface_index rx_meter[PPA_MAX_IFS_NUM], tx_meter[PPA_MAX_IFS_NUM];

uint32_t put_meter_usage(uint8_t rx_id, uint8_t tx_id);

/* Zero'th meter is currently used for CoC
 * rxid_bitmap and txid_bitmap are used for
 * (rx,tx)index in meter table
 * rxid_bitmap, and txid_bitmap is set to 1, as first row and first column
 * are used for flows b/w any physical interface to logical interface and
 * vice versa.
 */
long unsigned int meter_bitmap = 1, rxid_bitmap = 1, txid_bitmap = 1;
#endif
static inline uint32_t is_lansession(uint32_t flags){
	return ((flags & SESSION_LAN_ENTRY) ? 1 : 0);
}

void show_route_entry(GSW_ROUTE_Entry_t *rt_entry)
{
	int8_t strbuf1[24]={0},strbuf2[24]={0},ij; 
	
	if(rt_entry->routeEntry.pattern.bValid) {
	
	printk(KERN_INFO "===============================================================\n");
	printk(KERN_INFO "	Index		=	 %d\n", rt_entry->nRtIndex);
	printk(KERN_INFO "	Type		=	 %d\n", rt_entry->routeEntry.pattern.eIpType);
	printk(KERN_INFO "	Src IP		=	 %8x ", rt_entry->routeEntry.pattern.nSrcIP.nIPv4);
	if(rt_entry->routeEntry.pattern.eIpType==2) {
		for(ij=2; ij<8; ij++)
		printk(KERN_INFO "%04x ",rt_entry->routeEntry.pattern.nSrcIP.nIPv6[ij]);
	}
	printk(KERN_INFO "\n");
		printk(KERN_INFO "	Dest IP		=	 %8x ", rt_entry->routeEntry.pattern.nDstIP.nIPv4);
	if(rt_entry->routeEntry.pattern.eIpType==2) {
		for(ij=2; ij<8; ij++)
		printk(KERN_INFO "%04x ",rt_entry->routeEntry.pattern.nDstIP.nIPv6[ij]);
	}
	printk(KERN_INFO "\n");
	printk(KERN_INFO "	Src Port	=	 %d\n", rt_entry->routeEntry.pattern.nSrcPort);
		printk(KERN_INFO "	Dest Port	=	 %d\n", rt_entry->routeEntry.pattern.nDstPort);
		printk(KERN_INFO "	Rt Extn Id	=	 %d\n", rt_entry->routeEntry.pattern.nRoutExtId);
		printk(KERN_INFO "	Dest PortMap	=	 %0x\n", rt_entry->routeEntry.action.nDstPortMap);
	printk(KERN_INFO "	Dest Subif	=	 %0x\n", rt_entry->routeEntry.action.nDstSubIfId);
		printk(KERN_INFO "	DstIp Type	=	 %d\n", rt_entry->routeEntry.action.eIpType);
	printk(KERN_INFO "	NAT IPaddr	=	 %x\n", rt_entry->routeEntry.action.nNATIPaddr.nIPv4);
	printk(KERN_INFO "	NAT Port	=	 %d\n", rt_entry->routeEntry.action.nTcpUdpPort);
	printk(KERN_INFO "	MTU		=	 %d\n", rt_entry->routeEntry.action.nMTUvalue);
	printk(KERN_INFO "	Src MAC		=	 %s\n", ppa_get_pkt_mac_string(rt_entry->routeEntry.action.nSrcMAC,strbuf1));
		printk(KERN_INFO "	Dst MAC		=	 %s\n", ppa_get_pkt_mac_string(rt_entry->routeEntry.action.nDstMAC,strbuf2));
		printk(KERN_INFO "	PPPoE Mode	=	 %u\n", rt_entry->routeEntry.action.bPPPoEmode);
	printk(KERN_INFO "	PPPoE SessID	=	 %u\n", rt_entry->routeEntry.action.nPPPoESessId);
	printk(KERN_INFO "	Sess Dir	=	 %u\n", rt_entry->routeEntry.action.eSessDirection);
		printk(KERN_INFO "	Routing Mode	=	 %u\n", rt_entry->routeEntry.action.eSessRoutingMode);
	printk(KERN_INFO "===============================================================\n");
	}
	
}

#if 0
static void show_pce_rule(GSW_PCE_rule_t *rule)
{	
	int8_t strbuf1[24]={0},strbuf2[24]={0}; 
	int ij;
	
	if(rule->pattern.bEnable) {
	
	printk(KERN_INFO "===============================================================\n");
		printk(KERN_INFO "	Index		=	 %d\n", rule->pattern.nIndex);
	printk(KERN_INFO "===============================================================\n");
	printk(KERN_INFO "	Pattern:\n");
	if(rule->pattern.bPortIdEnable) {
		printk(KERN_INFO "	pattern.bPortIdEnable		=	 %d\n", rule->pattern.bPortIdEnable);
		printk(KERN_INFO "	pattern.nPortId			=	 %d\n", rule->pattern.nPortId);	
		printk(KERN_INFO "	pattern.bPortId_Exclude		=	 %d\n", rule->pattern.bPortId_Exclude);	
	}
	if(rule->pattern.bSubIfIdEnable) {
		printk(KERN_INFO "	pattern.bSubIfIdEnable		=	 %d\n", rule->pattern.bSubIfIdEnable);
		printk(KERN_INFO "	pattern.nSubIfId		=	 %d\n", rule->pattern.nSubIfId);
		printk(KERN_INFO "	pattern.bSubIfId_Exclude	=	 %d\n", rule->pattern.bSubIfId_Exclude);
	}
	if(rule->pattern.bDSCP_Enable) {
		printk(KERN_INFO "	pattern.bDSCP_Enable		=	 %d\n", rule->pattern.bDSCP_Enable);
		printk(KERN_INFO "	pattern.nDSCP			=	 %d\n", rule->pattern.nDSCP);
		printk(KERN_INFO "	pattern.bDSCP_Exclude		=	 %d\n", rule->pattern.bDSCP_Exclude);
	}
	if(rule->pattern.bInner_DSCP_Enable) {
		printk(KERN_INFO "	pattern.bInner_DSCP_Enable	=	 %d\n", rule->pattern.bInner_DSCP_Enable);
		printk(KERN_INFO "	pattern.nInnerDSCP		=	 %d\n", rule->pattern.nInnerDSCP);
		printk(KERN_INFO "	pattern.bInnerDSCP_Exclude	=	 %d\n", rule->pattern.bInnerDSCP_Exclude);
	}
	if(rule->pattern.bPCP_Enable) {
		printk(KERN_INFO "	pattern.bPCP_Enable		=	 %d\n", rule->pattern.bPCP_Enable);
		printk(KERN_INFO "	pattern.nPCP			=	 %d\n", rule->pattern.nPCP);
		printk(KERN_INFO "	pattern.bCTAG_PCP_DEI_Exclude	=	 %d\n",rule->pattern.bCTAG_PCP_DEI_Exclude);
	}
	if(rule->pattern.bSTAG_PCP_DEI_Enable) {
		printk(KERN_INFO "	pattern.bSTAG_PCP_DEI_Enable	=	 %d\n", rule->pattern.bSTAG_PCP_DEI_Enable);
		printk(KERN_INFO "	pattern.nSTAG_PCP_DEI		=	 %d\n", rule->pattern.nSTAG_PCP_DEI);
		printk(KERN_INFO "	pattern.bSTAG_PCP_DEI_Exclude	=	 %d\n", rule->pattern.bSTAG_PCP_DEI_Exclude);
	}
	if(rule->pattern.bPktLngEnable) {
		printk(KERN_INFO "	pattern.bPktLngEnable		=	 %d\n", rule->pattern.bPktLngEnable);
		printk(KERN_INFO "	pattern.nPktLng			=	 %d\n", rule->pattern.nPktLng);
		printk(KERN_INFO "	pattern.nPktLngRange		=	 %d\n", rule->pattern.nPktLngRange);
		printk(KERN_INFO "	pattern.bPktLng_Exclude		=	 %d\n", rule->pattern.bPktLng_Exclude);
	}
	if(rule->pattern.bMAC_DstEnable) {
		printk(KERN_INFO "	pattern.bMAC_DstEnable		=	 %d\n", rule->pattern.bMAC_DstEnable);
		printk(KERN_INFO "	pattern.nMAC_Dst[6]		=	 %s\n", ppa_get_pkt_mac_string(rule->pattern.nMAC_Dst,strbuf1));
		printk(KERN_INFO "	pattern.nMAC_DstMask		=	 %x\n", rule->pattern.nMAC_DstMask);
		printk(KERN_INFO "	pattern.bDstMAC_Exclude		=	 %d\n", rule->pattern.bDstMAC_Exclude);
	}
	if(rule->pattern.bMAC_SrcEnable) {
		printk(KERN_INFO "	pattern.bMAC_SrcEnable		=	 %d\n", rule->pattern.bMAC_SrcEnable);
		printk(KERN_INFO "	pattern.nMAC_Src[6]		=	 %s\n", ppa_get_pkt_mac_string(rule->pattern.nMAC_Src,strbuf2));
		printk(KERN_INFO "	pattern.nMAC_SrcMask		=	 %x\n", rule->pattern.nMAC_SrcMask);
		printk(KERN_INFO "	pattern.bSrcMAC_Exclude		=	 %d\n", rule->pattern.bSrcMAC_Exclude);
	}
	if(rule->pattern.bAppDataMSB_Enable) {
		printk(KERN_INFO "	pattern.bAppDataMSB_Enable	=	 %d\n", rule->pattern.bAppDataMSB_Enable);
		printk(KERN_INFO "	pattern.nAppDataMSB		=	 %x\n", rule->pattern.nAppDataMSB);
		printk(KERN_INFO "	pattern.bAppMaskRangeMSB_Select =	 %d\n", rule->pattern.bAppMaskRangeMSB_Select);
		printk(KERN_INFO "	pattern.nAppMaskRangeMSB	=	 %x\n", rule->pattern.nAppMaskRangeMSB);
		printk(KERN_INFO "	pattern.bAppMSB_Exclude		=	 %d\n", rule->pattern.bAppMSB_Exclude);
	}
	if(rule->pattern.bAppDataLSB_Enable) {
		printk(KERN_INFO "	pattern.bAppDataLSB_Enable	=	 %d\n", rule->pattern.bAppDataLSB_Enable);
		printk(KERN_INFO "	pattern.nAppDataLSB		=	 %x\n", rule->pattern.nAppDataLSB);
		printk(KERN_INFO "	pattern.bAppMaskRangeLSB_Select =	 %d\n", rule->pattern.bAppMaskRangeLSB_Select);
		printk(KERN_INFO "	pattern.nAppMaskRangeLSB	=	 %x\n", rule->pattern.nAppMaskRangeLSB);
		printk(KERN_INFO "	pattern.bAppLSB_Exclude		=	 %d\n", rule->pattern.bAppLSB_Exclude);
		}
	if(rule->pattern.eDstIP_Select) {
		printk(KERN_INFO "	pattern.eDstIP_Select		=	 %d\n", rule->pattern.eDstIP_Select);
		printk(KERN_INFO "	pattern.nDstIP			=	 %08x ", rule->pattern.nDstIP.nIPv4);
		if(rule->pattern.eDstIP_Select==2) {
			for(ij=2; ij<8; ij++)
				printk(KERN_INFO "%04x ",rule->pattern.nDstIP.nIPv6[ij]);
		}
		printk(KERN_INFO "\n");
		printk(KERN_INFO "	pattern.nDstIP_Mask		=	 %x\n", rule->pattern.nDstIP_Mask);
		printk(KERN_INFO "	pattern.bDstIP_Exclude		=	 %d\n", rule->pattern.bDstIP_Exclude);
	}
	if(rule->pattern.eInnerDstIP_Select) {
		printk(KERN_INFO "	pattern.eInnerDstIP_Select	=	 %d\n", rule->pattern.eInnerDstIP_Select);
		printk(KERN_INFO "	pattern.nInnerDstIP		=	 %x\n", rule->pattern.nInnerDstIP.nIPv4);
		printk(KERN_INFO "	pattern.nInnerDstIP_Mask	=	 %x\n", rule->pattern.nInnerDstIP_Mask);
		printk(KERN_INFO "	pattern.bInnerDstIP_Exclude	=	 %d\n", rule->pattern.bInnerDstIP_Exclude);
	}
	if(rule->pattern.eSrcIP_Select) {
		printk(KERN_INFO "	pattern.eSrcIP_Select		=	 %d\n", rule->pattern.eSrcIP_Select);
		printk(KERN_INFO "	pattern.nSrcIP			=	 %x\n", rule->pattern.nSrcIP.nIPv4);
		printk(KERN_INFO "	pattern.nSrcIP_Mask		=	 %x\n", rule->pattern.nSrcIP_Mask);
		printk(KERN_INFO "	pattern.bSrcIP_Exclude		=	 %d\n", rule->pattern.bSrcIP_Exclude);
		}
	if(rule->pattern.eInnerSrcIP_Select) {
		printk(KERN_INFO "	pattern.eInnerSrcIP_Select	=	 %d\n", rule->pattern.eInnerSrcIP_Select);
		printk(KERN_INFO "	pattern.nInnerSrcIP		=	 %x\n", rule->pattern.nInnerSrcIP.nIPv4);
		printk(KERN_INFO "	pattern.nInnerSrcIP_Mask	=	 %x\n", rule->pattern.nInnerSrcIP_Mask);
		printk(KERN_INFO "	pattern.bInnerSrcIP_Exclude	=	 %d\n", rule->pattern.bInnerSrcIP_Exclude);
		}
	if(rule->pattern.bEtherTypeEnable) {
		printk(KERN_INFO "	pattern.bEtherTypeEnable	=	 %d\n", rule->pattern.bEtherTypeEnable);
		printk(KERN_INFO "	pattern.nEtherType		=	 %x\n", rule->pattern.nEtherType);
		printk(KERN_INFO "	pattern.nEtherTypeMask		=	 %x\n", rule->pattern.nEtherTypeMask);
		printk(KERN_INFO "	pattern.bEtherType_Exclude	=	 %d\n", rule->pattern.bEtherType_Exclude);
	}
	if(rule->pattern.bProtocolEnable) {
		printk(KERN_INFO "	pattern.bProtocolEnable		=	 %d\n", rule->pattern.bProtocolEnable);
		printk(KERN_INFO "	pattern.nProtocol		=	 %x\n", rule->pattern.nProtocol);
		printk(KERN_INFO "	pattern.nProtocolMask		=	 %x\n", rule->pattern.nProtocolMask);
		printk(KERN_INFO "	pattern.bProtocol_Exclude	=	 %d\n", rule->pattern.bProtocol_Exclude);
	}
	if(rule->pattern.bInnerProtocolEnable) {
		printk(KERN_INFO "	pattern.bInnerProtocolEnable	=	 %d\n", rule->pattern.bInnerProtocolEnable);
		printk(KERN_INFO "	pattern.nInnerProtocol		=	 %x\n", rule->pattern.nInnerProtocol);
		printk(KERN_INFO "	pattern.nInnerProtocolMask	=	 %x\n", rule->pattern.nInnerProtocolMask);
		printk(KERN_INFO "	pattern.bInnerProtocol_Exclude	=	 %d\n", rule->pattern.bInnerProtocol_Exclude);
	}
	if(rule->pattern.bSessionIdEnable) {
		printk(KERN_INFO "	pattern.bSessionIdEnable	=	 %d\n", rule->pattern.bSessionIdEnable);
		printk(KERN_INFO "	pattern.nSessionId		=	 %x\n", rule->pattern.nSessionId);
		printk(KERN_INFO "	pattern.bSessionId_Exclude	=	 %d\n", rule->pattern.bSessionId_Exclude);
	}
	if(rule->pattern.bPPP_ProtocolEnable) {
		printk(KERN_INFO "	pattern.bPPP_ProtocolEnable	=	 %d\n", rule->pattern.bPPP_ProtocolEnable);
		printk(KERN_INFO "	pattern.nPPP_Protocol		=	 %x\n", rule->pattern.nPPP_Protocol);
		printk(KERN_INFO "	pattern.nPPP_ProtocolMask	=	 %x\n", rule->pattern.nPPP_ProtocolMask);	
		printk(KERN_INFO "	pattern.bPPP_Protocol_Exclude	=	 %d\n", rule->pattern.bPPP_Protocol_Exclude);
	}
	if(rule->pattern.bVid) {
		printk(KERN_INFO "	pattern.bVid			=	 %d\n", rule->pattern.bVid);
		printk(KERN_INFO "	pattern.nVid			=	 %d\n", rule->pattern.nVid);
		printk(KERN_INFO "	pattern.bVid_Exclude		=	 %d\n", rule->pattern.bVid_Exclude);
	}
	if(rule->pattern.bSLAN_Vid) {
		printk(KERN_INFO "	pattern.bSLAN_Vid		=	%d\n", rule->pattern.bSLAN_Vid);
		printk(KERN_INFO "	pattern.nSLAN_Vid		=	%d\n", rule->pattern.nSLAN_Vid);
		printk(KERN_INFO "	pattern.bSLANVid_Exclude	=	%d\n", rule->pattern.bSLANVid_Exclude);
	}
	if(rule->pattern.bPayload1_SrcEnable) {
		printk(KERN_INFO "	pattern.bPayload1_SrcEnable	=	 %d\n", rule->pattern.bPayload1_SrcEnable);
		printk(KERN_INFO "	pattern.nPayload1		=	 %x\n", rule->pattern.nPayload1);
		printk(KERN_INFO "	pattern.nPayload1_Mask		=	 %x\n", rule->pattern.nPayload1_Mask);
		printk(KERN_INFO "	pattern.bPayload1_Exclude	=	 %d\n", rule->pattern.bPayload1_Exclude);
	}
	if(rule->pattern.bPayload2_SrcEnable) {
		printk(KERN_INFO "	pattern.bPayload2_SrcEnable	=	 %d\n", rule->pattern.bPayload2_SrcEnable);
		printk(KERN_INFO "	pattern.nPayload2		=	 %x\n", rule->pattern.nPayload2);
		printk(KERN_INFO "	pattern.nPayload2_Mask		=	 %x\n", rule->pattern.nPayload2_Mask);
		printk(KERN_INFO "	pattern.bPayload2_Exclude	=	 %d\n", rule->pattern.bPayload2_Exclude);
		}
	if(rule->pattern.bParserFlagLSB_Enable) {
		printk(KERN_INFO "	pattern.bParserFlagLSB_Enable	=	 %d\n", rule->pattern.bParserFlagLSB_Enable);
		printk(KERN_INFO "	pattern.nParserFlagLSB		=	 %x\n", rule->pattern.nParserFlagLSB);
		printk(KERN_INFO "	pattern.nParserFlagLSB_Mask	=	 %x\n", rule->pattern.nParserFlagLSB_Mask);
		printk(KERN_INFO "	pattern.bParserFlagLSB_Exclude	=	 %d\n", rule->pattern.bParserFlagLSB_Exclude);
	}
	if(rule->pattern.bParserFlagMSB_Enable) {
		printk(KERN_INFO "	pattern.bParserFlagMSB_Enable	=	 %d\n", rule->pattern.bParserFlagMSB_Enable);
		printk(KERN_INFO "	pattern.nParserFlagMSB		=	 %x\n", rule->pattern.nParserFlagMSB);
		printk(KERN_INFO "	pattern.nParserFlagMSB_Mask	=	 %x\n", rule->pattern.nParserFlagMSB_Mask);
		printk(KERN_INFO "	pattern.bParserFlagMSB_Exclude	=	 %d\n", rule->pattern.bParserFlagMSB_Exclude);
	}
	printk(KERN_INFO "	Action:\n");

	if(rule->action.eTrafficClassAction) {
		printk(KERN_INFO "	action.eTrafficClassAction	=	 %d\n", rule->action.eTrafficClassAction);
		printk(KERN_INFO "	action.nTrafficClassAlternate	=	 %d\n", rule->action.nTrafficClassAlternate);
	}
	if(rule->action.eSnoopingTypeAction) {
		printk(KERN_INFO "	action.eSnoopingTypeAction	=	 %d\n", rule->action.eSnoopingTypeAction);
	}		
	if(rule->action.eLearningAction) {
		printk(KERN_INFO "	action.eLearningAction		=	 %d\n", rule->action.eLearningAction);
	}
	if(rule->action.eIrqAction) {
		printk(KERN_INFO "	action.eIrqAction		=	 %d\n", rule->action.eIrqAction);
	}
	if(rule->action.eCrossStateAction) {
		printk(KERN_INFO "	action.eCrossStateAction	=	 %d\n", rule->action.eCrossStateAction);
	}
	if(rule->action.eCritFrameAction) {
		printk(KERN_INFO "	action.eCritFrameAction		=	 %d\n", rule->action.eCritFrameAction);
	} 
	if(rule->action.eTimestampAction) {
		printk(KERN_INFO "	action.eTimestampAction		=	 %d\n", rule->action.eTimestampAction);
	}
	if(rule->action.ePortMapAction) {
		printk(KERN_INFO "	action.ePortMapAction		=	 %d\n", rule->action.ePortMapAction);
		printk(KERN_INFO "	action.nForwardPortMap		=	 %d\n", rule->action.nForwardPortMap[0]);
		printk(KERN_INFO "	action.nForwardSubIfId		=	 %d\n", rule->action.nForwardSubIfId);
	}
	if(rule->action.bRemarkAction) {
		printk(KERN_INFO "	action.bRemarkAction		=	 %d\n", rule->action.bRemarkAction);
	}
	if(rule->action.bRemarkPCP) {
		printk(KERN_INFO "	action.bRemarkPCP		=	 %d\n", rule->action.bRemarkPCP);
	}
	if(rule->action.bRemarkSTAG_PCP) {
		printk(KERN_INFO "	action.bRemarkSTAG_PCP		=	 %d\n", rule->action.bRemarkSTAG_PCP);
	}
	if(rule->action.bRemarkSTAG_DEI) {
		printk(KERN_INFO "	action.bRemarkSTAG_DEI		=	 %d\n", rule->action.bRemarkSTAG_DEI);
	}
	if(rule->action.bRemarkDSCP) {
		printk(KERN_INFO "	action.bRemarkDSCP		=	 %d\n", rule->action.bRemarkDSCP);
	}
	if(rule->action.bRemarkClass) {
		printk(KERN_INFO "	action.bRemarkClass		=	 %d\n", rule->action.bRemarkClass);
	}
	if(rule->action.eMeterAction) {
		printk(KERN_INFO "	action.eMeterAction		=	 %d\n", rule->action.eMeterAction);
		printk(KERN_INFO "	action.nMeterId			=	 %d\n", rule->action.nMeterId);
	}
	if(rule->action.bRMON_Action) {
		printk(KERN_INFO "	action.bRMON_Action		=	 %d\n", rule->action.bRMON_Action);
		printk(KERN_INFO "	action.nRMON_Id			=	 %d\n", rule->action.nRMON_Id);
	}
	if(rule->action.eVLAN_Action) { 
		printk(KERN_INFO "	action.eVLAN_Action		=	 %d\n", rule->action.eVLAN_Action);
		printk(KERN_INFO "	action.nVLAN_Id			=	 %d\n", rule->action.nVLAN_Id);
	}
	if(rule->action.eSVLAN_Action) {
		printk(KERN_INFO "	action.eSVLAN_Action		=	 %d\n", rule->action.eSVLAN_Action);
		printk(KERN_INFO "	action.nSVLAN_Id		=	 %d\n", rule->action.nSVLAN_Id);
	}
	if(rule->action.eVLAN_CrossAction) {
		printk(KERN_INFO "	action.eVLAN_CrossAction	=	 %d\n", rule->action.eVLAN_CrossAction);
	}
	if(rule->action.nFId) {
		printk(KERN_INFO "	action.nFId			=	 %d\n", rule->action.nFId);
	}
	if(rule->action.bPortBitMapMuxControl) {
		printk(KERN_INFO "	action.bPortBitMapMuxControl	=	 %d\n", rule->action.bPortBitMapMuxControl);
	}
	if(rule->action.bPortTrunkAction) {
		printk(KERN_INFO "	action.bPortTrunkAction		=	 %d\n", rule->action.bPortTrunkAction);
	}
	if(rule->action.bPortLinkSelection) {
		printk(KERN_INFO "	action.bPortLinkSelection	=	 %d\n", rule->action.bPortLinkSelection);
	}
	if(rule->action.bCVLAN_Ignore_Control) {
		printk(KERN_INFO "	action.bCVLAN_Ignore_Control	=	 %d\n", rule->action.bCVLAN_Ignore_Control);
	}
	if(rule->action.bFlowID_Action) {
		printk(KERN_INFO "	action.bFlowID_Action		=	 %d\n", rule->action.bFlowID_Action);
		printk(KERN_INFO "	action.nFlowID			=	 %d\n", rule->action.nFlowID);
	}
	if(rule->action.bRoutExtId_Action) {
		printk(KERN_INFO "	action.bRoutExtId_Action	=	 %d\n", rule->action.bRoutExtId_Action);
		printk(KERN_INFO "	action.nRoutExtId		=	 %d\n", rule->action.nRoutExtId);
	}
	if(rule->action.bRtDstPortMaskCmp_Action) {
		printk(KERN_INFO "	action.bRtDstPortMaskCmp_Action =	 %d\n", rule->action.bRtDstPortMaskCmp_Action); 
	}
	if(rule->action.bRtSrcPortMaskCmp_Action) {
		printk(KERN_INFO "	action.bRtSrcPortMaskCmp_Action =	 %d\n", rule->action.bRtSrcPortMaskCmp_Action);
	}
	if(rule->action.bRtDstIpMaskCmp_Action) {
		printk(KERN_INFO "	action.bRtDstIpMaskCmp_Action	=	 %d\n", rule->action.bRtDstIpMaskCmp_Action); 
	}
	if(rule->action.bRtSrcIpMaskCmp_Action) {
		printk(KERN_INFO "	action.bRtSrcIpMaskCmp_Action	=	 %d\n", rule->action.bRtSrcIpMaskCmp_Action);
	}
	if(rule->action.bRtInnerIPasKey_Action) {
		printk(KERN_INFO "	action.bRtInnerIPasKey_Action	=	 %d\n", rule->action.bRtInnerIPasKey_Action);
	}
	if(rule->action.bRtAccelEna_Action) {
		printk(KERN_INFO "	action.bRtAccelEna_Action	=	 %d\n", rule->action.bRtAccelEna_Action);
	}
	if(rule->action.bRtCtrlEna_Action) {
		printk(KERN_INFO "	action.bRtCtrlEna_Action	=	 %d\n", rule->action.bRtCtrlEna_Action);
	}
	if(rule->action.eProcessPath_Action) {
		printk(KERN_INFO "	action.eProcessPath_Action	=	 %d\n", rule->action.eProcessPath_Action);
	}
	if(rule->action.ePortFilterType_Action) {
		printk(KERN_INFO "	action.ePortFilterType_Action	 =	 %d\n", rule->action.ePortFilterType_Action);
	}
	}
}
#endif

#if defined(PPA_API_PROC)
static int proc_read_pae_route_seq_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &g_proc_read_pae_route_seq_ops);
}

static void *proc_read_pae_route_seq_start(struct seq_file *seq, loff_t *ppos)
{
	static GSW_ROUTE_Entry_t rt_entry;

	memset(&rt_entry, 0, sizeof(GSW_ROUTE_Entry_t));

	if (*ppos >= MAX_ROUTING_ENTRIES)
		return NULL;

	gswr_handle = gsw_api_kopen("/dev/switch_api/1");
	if (gswr_handle == 0) {
		dbg("%s: Open SWAPI device FAILED !!\n", __func__ );
		return NULL;
	}

	rt_entry.nRtIndex = (uint32_t)(*ppos);
	if ((gsw_api_kioctl(gswr_handle, GSW_ROUTE_ENTRY_READ, &rt_entry)) < GSW_statusOk) {
		dbg("read_routing_entry returned Failure \n");
		return NULL;
	}
	return &rt_entry;
}

static void proc_read_pae_seq_stop(struct seq_file *seq, void *v)
{
	gsw_api_kclose(gswr_handle);
}

static void *proc_read_pae_route_seq_next(struct seq_file *seq, void *v, loff_t *ppos)
{
	GSW_ROUTE_Entry_t *rt_entry = (GSW_ROUTE_Entry_t*)v;

	memset(rt_entry, 0, sizeof(GSW_ROUTE_Entry_t));

	(*ppos)++;
	if (*ppos >= MAX_ROUTING_ENTRIES)
		return NULL;

	rt_entry->nRtIndex = (uint32_t)(*ppos);
	if ((gsw_api_kioctl(gswr_handle, GSW_ROUTE_ENTRY_READ, rt_entry)) < GSW_statusOk) {
		dbg("read_routing_entry returned Failure \n");
		return NULL;
	}
	return rt_entry;
}

static int proc_read_pae_route_seq_show(struct seq_file *seq, void *v)
{
	GSW_ROUTE_Entry_t *rt_entry = (GSW_ROUTE_Entry_t *)v;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}

	if (!rt_entry->routeEntry.pattern.bValid)
		return 0;

	seq_printf(seq, "===============================================================\n");
	seq_printf(seq, "	Index			=	 %d\n", rt_entry->nRtIndex);
	seq_printf(seq, "	Type			=	 %d\n", rt_entry->routeEntry.pattern.eIpType);
	if (rt_entry->routeEntry.pattern.eIpType == 2)
		seq_printf(seq, "	Src IP			=	 %pI6\n", rt_entry->routeEntry.pattern.nSrcIP.nIPv6);
	else
		seq_printf(seq, "	Src IP			=	 %pI4\n", &rt_entry->routeEntry.pattern.nSrcIP.nIPv4);

	if (rt_entry->routeEntry.pattern.eIpType == 2)
		seq_printf(seq, "	Dest IP			=	 %pI6\n", rt_entry->routeEntry.pattern.nDstIP.nIPv6);
	else
		seq_printf(seq, "	Dest IP			=	 %pI4\n", &rt_entry->routeEntry.pattern.nDstIP.nIPv4);

	seq_printf(seq, "	Src Port		=	 %d\n", rt_entry->routeEntry.pattern.nSrcPort);
	seq_printf(seq, "	Dest Port		=	 %d\n", rt_entry->routeEntry.pattern.nDstPort);
	seq_printf(seq, "	Rt Extn Id		=	 %d\n", rt_entry->routeEntry.pattern.nRoutExtId);
	seq_printf(seq, "	Dest PortMap		=	 0x%0x\n", rt_entry->routeEntry.action.nDstPortMap);
	seq_printf(seq, "	Dest Subif		=	 0x%0x\n", rt_entry->routeEntry.action.nDstSubIfId);
	seq_printf(seq, "	DstIp Type		=	 %d\n", rt_entry->routeEntry.action.eIpType);
	seq_printf(seq, "	NAT IPaddr		=	 %pI4\n", &rt_entry->routeEntry.action.nNATIPaddr.nIPv4);
	seq_printf(seq, "	NAT Port		=	 %d\n", rt_entry->routeEntry.action.nTcpUdpPort);
	seq_printf(seq, "	MTU			=	 %d\n", rt_entry->routeEntry.action.nMTUvalue);
	seq_printf(seq, "	Src MAC		 	=	 %pM\n", rt_entry->routeEntry.action.nSrcMAC);
	seq_printf(seq, "	Dst MAC		 	=	 %pM\n", rt_entry->routeEntry.action.nDstMAC);
	seq_printf(seq, "	PPPoE Mode		=	 %u\n", rt_entry->routeEntry.action.bPPPoEmode);
	seq_printf(seq, "	PPPoE SessID		=	 %u\n", rt_entry->routeEntry.action.nPPPoESessId);
	seq_printf(seq, "	Sess Dir		=	 %u\n", rt_entry->routeEntry.action.eSessDirection);
	seq_printf(seq, "	Routing Mode		=	 %u\n", rt_entry->routeEntry.action.eSessRoutingMode);
	seq_printf(seq, "	Tunnel Type	 	=	 %u\n", rt_entry->routeEntry.action.eTunType);
	seq_printf(seq, "	Hit Status		=	 %u\n", rt_entry->routeEntry.action.bHitStatus);
	seq_printf(seq, "	Session Counters	=	 %u\n", rt_entry->routeEntry.action.nSessionCtrs);

	return 0;
}

static int proc_read_pae_pce_seq_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &g_proc_read_pae_pce_seq_ops);
}

static void *proc_read_pae_pce_seq_start(struct seq_file *seq, loff_t *ppos)
{
	static GSW_PCE_rule_t rule;
	memset(&rule,0,sizeof(GSW_PCE_rule_t));

	if(*ppos == 0) {

		gswr_handle = gsw_api_kopen("/dev/switch_api/1");
		if (gswr_handle == 0) {
			dbg("%s: Open SWAPI device FAILED !!\n", __func__ );
			return NULL;
		}
		return &rule;
	} else if(*ppos < MAX_PCE_ENTRIES) { 
		(*ppos)--;
		return &rule;
	} else {
		return NULL;
	}
}

static void *proc_read_pae_pce_seq_next(struct seq_file *seq, void *v, loff_t *ppos)
{
	GSW_PCE_rule_t *rule = (GSW_PCE_rule_t*)v;
	memset(rule,0,sizeof(GSW_PCE_rule_t));
	 
	if(*ppos < MAX_PCE_ENTRIES) { 
		rule->pattern.nIndex = (uint32_t)(*ppos)++;
	 
		if((gsw_api_kioctl(gswr_handle, GSW_PCE_RULE_READ, rule)) < GSW_statusOk) {
			dbg("read_pce_entry returned Failure \n");
			return NULL;
		} else {
			return rule;
		}
	} else {
		return NULL;
	}	 
}

static int proc_read_pae_pce_seq_show(struct seq_file *seq, void *v)
{
	GSW_PCE_rule_t *rule = (GSW_PCE_rule_t *)v;
	int8_t strbuf1[24]={0},strbuf2[24]={0}; 
	int ij;
	
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	} 
	if(rule->pattern.bEnable) {
	
	seq_printf(seq,		"===============================================================\n");
	seq_printf(seq,		"	Index				=	 %d\n", rule->pattern.nIndex);
	seq_printf(seq,		"===============================================================\n");
	seq_printf(seq,		"	Pattern:\n");
	if(rule->pattern.bPortIdEnable) {
		seq_printf(seq, "	pattern.bPortIdEnable		=	 %d\n", rule->pattern.bPortIdEnable);
		seq_printf(seq, "	pattern.nPortId			=	 %d\n", rule->pattern.nPortId);	
		seq_printf(seq, "	pattern.bPortId_Exclude		=	 %d\n", rule->pattern.bPortId_Exclude);	
	}
	if(rule->pattern.bSubIfIdEnable) {
		seq_printf(seq, "	pattern.bSubIfIdEnable		=	 %d\n", rule->pattern.bSubIfIdEnable);
		seq_printf(seq, "	pattern.nSubIfId		=	 %d\n", rule->pattern.nSubIfId);
		seq_printf(seq, "	pattern.bSubIfId_Exclude	=	 %d\n", rule->pattern.bSubIfId_Exclude);
	}
	if(rule->pattern.bDSCP_Enable) {
		seq_printf(seq, "	pattern.bDSCP_Enable		=	 %d\n", rule->pattern.bDSCP_Enable);
		seq_printf(seq, "	pattern.nDSCP			=	 %d\n", rule->pattern.nDSCP);
		seq_printf(seq, "	pattern.bDSCP_Exclude		=	 %d\n", rule->pattern.bDSCP_Exclude);
	}
	if(rule->pattern.bInner_DSCP_Enable) {
		seq_printf(seq, "	pattern.bInner_DSCP_Enable	=	 %d\n", rule->pattern.bInner_DSCP_Enable);
		seq_printf(seq, "	pattern.nInnerDSCP		=	 %d\n", rule->pattern.nInnerDSCP);
		seq_printf(seq, "	pattern.bInnerDSCP_Exclude	=	 %d\n", rule->pattern.bInnerDSCP_Exclude);
	}
	if(rule->pattern.bPCP_Enable) {
		seq_printf(seq, "	pattern.bPCP_Enable		=	 %d\n", rule->pattern.bPCP_Enable);
		seq_printf(seq, "	pattern.nPCP			=	 %d\n", rule->pattern.nPCP);
		seq_printf(seq, "	pattern.bCTAG_PCP_DEI_Exclude	=	 %d\n",rule->pattern.bCTAG_PCP_DEI_Exclude);
	}
	if(rule->pattern.bSTAG_PCP_DEI_Enable) {
		seq_printf(seq, "	pattern.bSTAG_PCP_DEI_Enable	=	 %d\n", rule->pattern.bSTAG_PCP_DEI_Enable);
		seq_printf(seq, "	pattern.nSTAG_PCP_DEI		=	 %d\n", rule->pattern.nSTAG_PCP_DEI);
		seq_printf(seq, "	pattern.bSTAG_PCP_DEI_Exclude	=	 %d\n", rule->pattern.bSTAG_PCP_DEI_Exclude);
	}
	if(rule->pattern.bPktLngEnable) {
		seq_printf(seq, "	pattern.bPktLngEnable		=	 %d\n", rule->pattern.bPktLngEnable);
		seq_printf(seq, "	pattern.nPktLng			=	 %d\n", rule->pattern.nPktLng);
		seq_printf(seq, "	pattern.nPktLngRange		=	 %d\n", rule->pattern.nPktLngRange);
		seq_printf(seq, "	pattern.bPktLng_Exclude		=	 %d\n", rule->pattern.bPktLng_Exclude);
	}
	if(rule->pattern.bMAC_DstEnable) {
		seq_printf(seq, "	pattern.bMAC_DstEnable		=	 %d\n", rule->pattern.bMAC_DstEnable);
		seq_printf(seq, "	pattern.nMAC_Dst[6]		=	 %s\n", ppa_get_pkt_mac_string(rule->pattern.nMAC_Dst,strbuf1));
		seq_printf(seq, "	pattern.nMAC_DstMask		=	 %x\n", rule->pattern.nMAC_DstMask);
		seq_printf(seq, "	pattern.bDstMAC_Exclude		=	 %d\n", rule->pattern.bDstMAC_Exclude);
	}
	if(rule->pattern.bMAC_SrcEnable) {
		seq_printf(seq, "	pattern.bMAC_SrcEnable		=	 %d\n", rule->pattern.bMAC_SrcEnable);
		seq_printf(seq, "	pattern.nMAC_Src[6]		=	 %s\n", ppa_get_pkt_mac_string(rule->pattern.nMAC_Src,strbuf2));
		seq_printf(seq, "	pattern.nMAC_SrcMask		=	 %x\n", rule->pattern.nMAC_SrcMask);
		seq_printf(seq, "	pattern.bSrcMAC_Exclude		=	 %d\n", rule->pattern.bSrcMAC_Exclude);
	}
	if(rule->pattern.bAppDataMSB_Enable) {
		seq_printf(seq, "	pattern.bAppDataMSB_Enable	=	 %d\n", rule->pattern.bAppDataMSB_Enable);
		seq_printf(seq, "	pattern.nAppDataMSB		=	 %x\n", rule->pattern.nAppDataMSB);
		seq_printf(seq, "	pattern.bAppMaskRangeMSB_Select =	 %d\n", rule->pattern.bAppMaskRangeMSB_Select);
		seq_printf(seq, "	pattern.nAppMaskRangeMSB	=	 %x\n", rule->pattern.nAppMaskRangeMSB);
		seq_printf(seq, "	pattern.bAppMSB_Exclude		=	 %d\n", rule->pattern.bAppMSB_Exclude);
	}
	if(rule->pattern.bAppDataLSB_Enable) {
		seq_printf(seq, "	pattern.bAppDataLSB_Enable	=	 %d\n", rule->pattern.bAppDataLSB_Enable);
		seq_printf(seq, "	pattern.nAppDataLSB		=	 %x\n", rule->pattern.nAppDataLSB);
		seq_printf(seq, "	pattern.bAppMaskRangeLSB_Select =	 %d\n", rule->pattern.bAppMaskRangeLSB_Select);
		seq_printf(seq, "	pattern.nAppMaskRangeLSB	=	 %x\n", rule->pattern.nAppMaskRangeLSB);
		seq_printf(seq, "	pattern.bAppLSB_Exclude		=	 %d\n", rule->pattern.bAppLSB_Exclude);
	}
	if(rule->pattern.eDstIP_Select) {
		seq_printf(seq, "	pattern.eDstIP_Select		=	 %d\n", rule->pattern.eDstIP_Select);
		seq_printf(seq, "	pattern.nDstIP			=	 %08x ", rule->pattern.nDstIP.nIPv4);
		if(rule->pattern.eDstIP_Select==2) {
			for(ij=2; ij<8; ij++)
				seq_printf(seq, "%04x ",rule->pattern.nDstIP.nIPv6[ij]);
		}
		seq_printf(seq, "\n");
		seq_printf(seq, "	pattern.nDstIP_Mask		=	 %x\n", rule->pattern.nDstIP_Mask);
		seq_printf(seq, "	pattern.bDstIP_Exclude		=	 %d\n", rule->pattern.bDstIP_Exclude);
	}
	if(rule->pattern.eInnerDstIP_Select) {
		seq_printf(seq, "	pattern.eInnerDstIP_Select	=	 %d\n", rule->pattern.eInnerDstIP_Select);
		seq_printf(seq, "	pattern.nInnerDstIP		=	 %x\n", rule->pattern.nInnerDstIP.nIPv4);
		seq_printf(seq, "	pattern.nInnerDstIP_Mask	=	 %x\n", rule->pattern.nInnerDstIP_Mask);
		seq_printf(seq, "	pattern.bInnerDstIP_Exclude	=	 %d\n", rule->pattern.bInnerDstIP_Exclude);
	}
	if(rule->pattern.eSrcIP_Select) {
		seq_printf(seq, "	pattern.eSrcIP_Select		=	 %d\n", rule->pattern.eSrcIP_Select);
		seq_printf(seq, "	pattern.nSrcIP			=	 %x\n", rule->pattern.nSrcIP.nIPv4);
		seq_printf(seq, "	pattern.nSrcIP_Mask		=	 %x\n", rule->pattern.nSrcIP_Mask);
		seq_printf(seq, "	pattern.bSrcIP_Exclude		=	 %d\n", rule->pattern.bSrcIP_Exclude);
	}
	if(rule->pattern.eInnerSrcIP_Select) {
		seq_printf(seq, "	pattern.eInnerSrcIP_Select	=	 %d\n", rule->pattern.eInnerSrcIP_Select);
		seq_printf(seq, "	pattern.nInnerSrcIP		=	 %x\n", rule->pattern.nInnerSrcIP.nIPv4);
		seq_printf(seq, "	pattern.nInnerSrcIP_Mask	=	 %x\n", rule->pattern.nInnerSrcIP_Mask);
		seq_printf(seq, "	pattern.bInnerSrcIP_Exclude	=	 %d\n", rule->pattern.bInnerSrcIP_Exclude);
	}
	if(rule->pattern.bEtherTypeEnable) {
		seq_printf(seq, "	pattern.bEtherTypeEnable	=	 %d\n", rule->pattern.bEtherTypeEnable);
		seq_printf(seq, "	pattern.nEtherType		=	 %x\n", rule->pattern.nEtherType);
		seq_printf(seq, "	pattern.nEtherTypeMask		=	 %x\n", rule->pattern.nEtherTypeMask);
		seq_printf(seq, "	pattern.bEtherType_Exclude	=	 %d\n", rule->pattern.bEtherType_Exclude);
	}
	if(rule->pattern.bProtocolEnable) {
		seq_printf(seq, "	pattern.bProtocolEnable		=	 %d\n", rule->pattern.bProtocolEnable);
		seq_printf(seq, "	pattern.nProtocol		=	 %x\n", rule->pattern.nProtocol);
		seq_printf(seq, "	pattern.nProtocolMask		=	 %x\n", rule->pattern.nProtocolMask);
		seq_printf(seq, "	pattern.bProtocol_Exclude	=	 %d\n", rule->pattern.bProtocol_Exclude);
	}
	if(rule->pattern.bInnerProtocolEnable) {
		seq_printf(seq, "	pattern.bInnerProtocolEnable	=	 %d\n", rule->pattern.bInnerProtocolEnable);
		seq_printf(seq, "	pattern.nInnerProtocol		=	 %x\n", rule->pattern.nInnerProtocol);
		seq_printf(seq, "	pattern.nInnerProtocolMask	=	 %x\n", rule->pattern.nInnerProtocolMask);
		seq_printf(seq, "	pattern.bInnerProtocol_Exclude	=	 %d\n", rule->pattern.bInnerProtocol_Exclude);
	}
	if(rule->pattern.bSessionIdEnable) {
		seq_printf(seq, "	pattern.bSessionIdEnable	=	 %d\n", rule->pattern.bSessionIdEnable);
		seq_printf(seq, "	pattern.nSessionId		=	 %x\n", rule->pattern.nSessionId);
		seq_printf(seq, "	pattern.bSessionId_Exclude	=	 %d\n", rule->pattern.bSessionId_Exclude);
	}
	if(rule->pattern.bPPP_ProtocolEnable) {
		seq_printf(seq, "	pattern.bPPP_ProtocolEnable	=	 %d\n", rule->pattern.bPPP_ProtocolEnable);
		seq_printf(seq, "	pattern.nPPP_Protocol		=	 %x\n", rule->pattern.nPPP_Protocol);
		seq_printf(seq, "	pattern.nPPP_ProtocolMask	=	 %x\n", rule->pattern.nPPP_ProtocolMask);	
		seq_printf(seq, "	pattern.bPPP_Protocol_Exclude	=	 %d\n", rule->pattern.bPPP_Protocol_Exclude);
	}
	if(rule->pattern.bVid) {
		seq_printf(seq, "	pattern.bVid			=	 %d\n", rule->pattern.bVid);
		seq_printf(seq, "	pattern.nVid			=	 %d\n", rule->pattern.nVid);
		seq_printf(seq, "	pattern.bVid_Exclude		=	 %d\n", rule->pattern.bVid_Exclude);
	}
	if(rule->pattern.bSLAN_Vid) {
		seq_printf(seq, "	pattern.bSLAN_Vid		=	%d\n", rule->pattern.bSLAN_Vid);
		seq_printf(seq, "	pattern.nSLAN_Vid		=	%d\n", rule->pattern.nSLAN_Vid);
		seq_printf(seq, "	pattern.bSLANVid_Exclude	=	%d\n", rule->pattern.bSLANVid_Exclude);
	}
	if(rule->pattern.bPayload1_SrcEnable) {
		seq_printf(seq, "	pattern.bPayload1_SrcEnable	=	 %d\n", rule->pattern.bPayload1_SrcEnable);
		seq_printf(seq, "	pattern.nPayload1		=	 %x\n", rule->pattern.nPayload1);
		seq_printf(seq, "	pattern.nPayload1_Mask		=	 %x\n", rule->pattern.nPayload1_Mask);
		seq_printf(seq, "	pattern.bPayload1_Exclude	=	 %d\n", rule->pattern.bPayload1_Exclude);
	}
	if(rule->pattern.bPayload2_SrcEnable) {
		seq_printf(seq, "	pattern.bPayload2_SrcEnable	=	 %d\n", rule->pattern.bPayload2_SrcEnable);
		seq_printf(seq, "	pattern.nPayload2		=	 %x\n", rule->pattern.nPayload2);
		seq_printf(seq, "	pattern.nPayload2_Mask		=	 %x\n", rule->pattern.nPayload2_Mask);
		seq_printf(seq, "	pattern.bPayload2_Exclude	=	 %d\n", rule->pattern.bPayload2_Exclude);
	}
	if(rule->pattern.bParserFlagLSB_Enable) {
		seq_printf(seq, "	pattern.bParserFlagLSB_Enable	=	 %d\n", rule->pattern.bParserFlagLSB_Enable);
		seq_printf(seq, "	pattern.nParserFlagLSB		=	 %x\n", rule->pattern.nParserFlagLSB);
		seq_printf(seq, "	pattern.nParserFlagLSB_Mask	=	 %x\n", rule->pattern.nParserFlagLSB_Mask);
		seq_printf(seq, "	pattern.bParserFlagLSB_Exclude	=	 %d\n", rule->pattern.bParserFlagLSB_Exclude);
	}
	if(rule->pattern.bParserFlagMSB_Enable) {
		seq_printf(seq, "	pattern.bParserFlagMSB_Enable	=	 %d\n", rule->pattern.bParserFlagMSB_Enable);
		seq_printf(seq, "	pattern.nParserFlagMSB		=	 %x\n", rule->pattern.nParserFlagMSB);
		seq_printf(seq, "	pattern.nParserFlagMSB_Mask	=	 %x\n", rule->pattern.nParserFlagMSB_Mask);
		seq_printf(seq, "	pattern.bParserFlagMSB_Exclude	=	 %d\n", rule->pattern.bParserFlagMSB_Exclude);
	}
	seq_printf(seq,		"	Action:\n");
	if(rule->action.eTrafficClassAction) {
		seq_printf(seq, "	action.eTrafficClassAction	=	 %d\n", rule->action.eTrafficClassAction);
		seq_printf(seq, "	action.nTrafficClassAlternate	=	 %d\n", rule->action.nTrafficClassAlternate);
	}
	if(rule->action.eSnoopingTypeAction) {
		seq_printf(seq, "	action.eSnoopingTypeAction	=	 %d\n", rule->action.eSnoopingTypeAction);
	}		
	if(rule->action.eLearningAction) {
		seq_printf(seq, "	action.eLearningAction		=	 %d\n", rule->action.eLearningAction);
	}
	if(rule->action.eIrqAction) {
		seq_printf(seq, "	action.eIrqAction		=	 %d\n", rule->action.eIrqAction);
	}
	if(rule->action.eCrossStateAction) {
		seq_printf(seq, "	action.eCrossStateAction	=	 %d\n", rule->action.eCrossStateAction);
	}
	if(rule->action.eCritFrameAction) {
		seq_printf(seq, "	action.eCritFrameAction		=	 %d\n", rule->action.eCritFrameAction);
	} 
	if(rule->action.eTimestampAction) {
		seq_printf(seq, "	action.eTimestampAction		=	 %d\n", rule->action.eTimestampAction);
	}
	if(rule->action.ePortMapAction) {
		seq_printf(seq, "	action.ePortMapAction		=	 %d\n", rule->action.ePortMapAction);
		seq_printf(seq, "	action.nForwardPortMap		=	 %d\n", rule->action.nForwardPortMap[0]);
		seq_printf(seq, "	action.nForwardSubIfId		=	 %d\n", rule->action.nForwardSubIfId);
	}
	if(rule->action.bRemarkAction) {
		seq_printf(seq, "	action.bRemarkAction		=	 %d\n", rule->action.bRemarkAction);
	}
	if(rule->action.bRemarkPCP) {
		seq_printf(seq, "	action.bRemarkPCP		=	 %d\n", rule->action.bRemarkPCP);
	}
	if(rule->action.bRemarkSTAG_PCP) {
		seq_printf(seq, "	action.bRemarkSTAG_PCP		=	 %d\n", rule->action.bRemarkSTAG_PCP);
	}
	if(rule->action.bRemarkSTAG_DEI) {
		seq_printf(seq, "	action.bRemarkSTAG_DEI		=	 %d\n", rule->action.bRemarkSTAG_DEI);
	}
	if(rule->action.bRemarkDSCP) {
		seq_printf(seq, "	action.bRemarkDSCP		=	 %d\n", rule->action.bRemarkDSCP);
	}
	if(rule->action.bRemarkClass) {
		seq_printf(seq, "	action.bRemarkClass		=	 %d\n", rule->action.bRemarkClass);
	}
	if(rule->action.eMeterAction) {
		seq_printf(seq, "	action.eMeterAction		=	 %d\n", rule->action.eMeterAction);
		seq_printf(seq, "	action.nMeterId			=	 %d\n", rule->action.nMeterId);
	}
	if(rule->action.bRMON_Action) {
		seq_printf(seq, "	action.bRMON_Action		=	 %d\n", rule->action.bRMON_Action);
		seq_printf(seq, "	action.nRMON_Id			=	 %d\n", rule->action.nRMON_Id);
	}
	if(rule->action.eVLAN_Action) { 
		seq_printf(seq, "	action.eVLAN_Action		=	 %d\n", rule->action.eVLAN_Action);
		seq_printf(seq, "	action.nVLAN_Id			=	 %d\n", rule->action.nVLAN_Id);
	}
	if(rule->action.eSVLAN_Action) {
		seq_printf(seq, "	action.eSVLAN_Action		=	 %d\n", rule->action.eSVLAN_Action);
		seq_printf(seq, "	action.nSVLAN_Id		=	 %d\n", rule->action.nSVLAN_Id);
	}
	if(rule->action.eVLAN_CrossAction) {
		seq_printf(seq, "	action.eVLAN_CrossAction	=	 %d\n", rule->action.eVLAN_CrossAction);
	}
	if(rule->action.nFId) {
		seq_printf(seq, "	action.nFId			=	 %d\n", rule->action.nFId);
	}
	if(rule->action.bPortBitMapMuxControl) {
		seq_printf(seq, "	action.bPortBitMapMuxControl	=	 %d\n", rule->action.bPortBitMapMuxControl);
	}
	if(rule->action.bPortTrunkAction) {
		seq_printf(seq, "	action.bPortTrunkAction		=	 %d\n", rule->action.bPortTrunkAction);
	}
	if(rule->action.bPortLinkSelection) {
		seq_printf(seq, "	action.bPortLinkSelection	=	 %d\n", rule->action.bPortLinkSelection);
	}
	if(rule->action.bCVLAN_Ignore_Control) {
		seq_printf(seq, "	action.bCVLAN_Ignore_Control	=	 %d\n", rule->action.bCVLAN_Ignore_Control);
	}
	if(rule->action.bFlowID_Action) {
		seq_printf(seq, "	action.bFlowID_Action		=	 %d\n", rule->action.bFlowID_Action);
		seq_printf(seq, "	action.nFlowID			=	 %d\n", rule->action.nFlowID);
	}
	if(rule->action.bRoutExtId_Action) {
		seq_printf(seq, "	action.bRoutExtId_Action	=	 %d\n", rule->action.bRoutExtId_Action);
		seq_printf(seq, "	action.nRoutExtId		=	 %d\n", rule->action.nRoutExtId);
	}
	if(rule->action.bRtDstPortMaskCmp_Action) {
		seq_printf(seq, "	action.bRtDstPortMaskCmp_Action =	 %d\n", rule->action.bRtDstPortMaskCmp_Action); 
	}
	if(rule->action.bRtSrcPortMaskCmp_Action) {
		seq_printf(seq, "	action.bRtSrcPortMaskCmp_Action =	 %d\n", rule->action.bRtSrcPortMaskCmp_Action);
	}
	if(rule->action.bRtDstIpMaskCmp_Action) {
		seq_printf(seq, "	action.bRtDstIpMaskCmp_Action	=	 %d\n", rule->action.bRtDstIpMaskCmp_Action); 
	}
	if(rule->action.bRtSrcIpMaskCmp_Action) {
		seq_printf(seq, "	action.bRtSrcIpMaskCmp_Action	=	 %d\n", rule->action.bRtSrcIpMaskCmp_Action);
	}
	if(rule->action.bRtInnerIPasKey_Action) {
		seq_printf(seq, "	action.bRtInnerIPasKey_Action	=	 %d\n", rule->action.bRtInnerIPasKey_Action);
	}
	if(rule->action.bRtAccelEna_Action) {
		seq_printf(seq, "	action.bRtAccelEna_Action	=	 %d\n", rule->action.bRtAccelEna_Action);
	}
	if(rule->action.bRtCtrlEna_Action) {
		seq_printf(seq, "	action.bRtCtrlEna_Action	=	 %d\n", rule->action.bRtCtrlEna_Action);
	}
	if(rule->action.eProcessPath_Action) {
		seq_printf(seq, "	action.eProcessPath_Action	=	 %d\n", rule->action.eProcessPath_Action);
	}
	if(rule->action.ePortFilterType_Action) {
		seq_printf(seq,	"	action.ePortFilterType_Action	 =	 %d\n", rule->action.ePortFilterType_Action);
	}
	}
	 
	return 0;
}

static ssize_t proc_set_pae_pce(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[40];
	char *p;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Write Permission denied");
		return 0;
	}

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);

	while ( len && str[len - 1] <= ' ' )
		len--;
	str[len] = 0;

	for( p = str; *p && *p <= ' '; p++, len-- );
	
	if ( !*p )
		return count;

	if(strncmp(p,"enable",6)==0) {
		if(!pae_hal_pce_enable) {
			init_pae_flows();
			pae_hal_pce_enable=1;
			printk(KERN_INFO "PCE RULES Enabled!!!\n"); 
		} else {
			printk(KERN_INFO "PCE RULES already Enabled!!!\n"); 
		}
	} else if (strncmp(p,"disable",7)==0){
		if(pae_hal_pce_enable) {
			uninit_pae_flows();
			pae_hal_pce_enable=0;	
			printk(KERN_INFO "PCE RULES disabled!!!\n"); 
		} else {
			printk(KERN_INFO "PCE RULES already disabled!!!\n"); 
		}
	} else {
		printk(KERN_INFO "usage : echo <enable/disable> > /proc/ppa/pae/pce\n"); 
	}
	
	return len; 
}

void pae_proc_file_create(void)
{
	struct proc_dir_entry *res;
	
	if(!g_ppa_proc_dir_flag) {
	g_ppa_proc_dir = proc_mkdir("ppa", NULL);
	g_ppa_proc_dir_flag = 1;
	}

	g_ppa_paehal_proc_dir = proc_mkdir("pae", g_ppa_proc_dir);
	g_ppa_paehal_proc_dir_flag = 1;

	g_ppa_paehal_lro_proc_dir = proc_mkdir("lro", g_ppa_paehal_proc_dir);
	g_ppa_paehal_lro_proc_dir_flag = 1;

	res = proc_create("route", 0600,
			g_ppa_paehal_proc_dir,
			&g_proc_file_pae_route_seq_fops);

	res = proc_create("pce", 0600,
			g_ppa_paehal_proc_dir,
			&g_proc_file_pae_pce_seq_fops);

	res = proc_create("rtstats", 0600,
			g_ppa_paehal_proc_dir,
			&g_proc_file_pae_rtstats_seq_fops);
	
	res = proc_create("accel", 0600,
			g_ppa_paehal_proc_dir,
			&g_proc_file_pae_accel_seq_fops);
	
	res = proc_create("dbg", 0600,
			g_ppa_paehal_proc_dir,
			&g_proc_file_pae_debug_seq_fops);

#ifdef PAE_HW_METER
	res = proc_create("hw_meter", 0600, 
			g_ppa_paehal_proc_dir,
			&g_proc_file_pae_meter_seq_fops);
#endif
	
	res = proc_create("pkt_len", 0600,
			g_ppa_paehal_lro_proc_dir,
			&g_proc_file_pae_lrolen_seq_fops);
	
	 res = proc_create("flush", 0600,
			g_ppa_paehal_lro_proc_dir,
			&g_proc_file_pae_lroflush_seq_fops);

	 res = proc_create("timeout", 0600,
			g_ppa_paehal_lro_proc_dir,
			&g_proc_file_pae_lrotimeout_seq_fops);

}

void pae_proc_file_remove(void)
{
	remove_proc_entry("route", g_ppa_paehal_proc_dir);
	remove_proc_entry("pce", g_ppa_paehal_proc_dir);
	remove_proc_entry("rtstats", g_ppa_paehal_proc_dir);
	remove_proc_entry("accel", g_ppa_paehal_proc_dir);
	remove_proc_entry("dbg", g_ppa_paehal_proc_dir);

#ifdef PAE_HW_METER
	remove_proc_entry("hw_meter", g_ppa_paehal_proc_dir);
#endif
	
	if(g_ppa_paehal_proc_dir_flag) {
		remove_proc_entry("pae", NULL);	
		g_ppa_paehal_proc_dir_flag = 0;
	}
	
	remove_proc_entry("pkt_len", g_ppa_paehal_lro_proc_dir);
	remove_proc_entry("flush", g_ppa_paehal_lro_proc_dir);
	
	if(g_ppa_paehal_lro_proc_dir_flag) {
		remove_proc_entry("lro", NULL);	
		g_ppa_paehal_lro_proc_dir_flag = 0;
	}
}

static int proc_read_pae_rtstats(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq,	"=====================================================================\n");
	seq_printf(seq,	"Total Number of Routing session entrie			: %u\n", nsess_add_succ - nsess_del + nsess_del_fail);
	seq_printf(seq,	"Total Number of Routing session add requests		: %u\n", nsess_add);
	seq_printf(seq,	"Total Number of Routing session delete			: %u\n", nsess_del);
	seq_printf(seq,	"Total Number of Routing session delete fail		: %u\n", nsess_del_fail);
	seq_printf(seq,	"Total Number of Routing session add fails		: %u\n", nsess_add_fail_lock + nsess_add_fail_rt_tbl_full + 
			nsess_add_fail_coll_full + nsess_add_fail_mac_tbl_full + nsess_add_fail_ip_tbl_full + nsess_add_fail_rtp_full + 
			nsess_add_fail_pppoe_full + nsess_add_fail_mtu_full + nsess_add_fail_oth +nsess_add_fail_minus1 + nsess_add_fail_negindex);
	seq_printf(seq,	"Total Number of Routing session add fail lock		: %u\n", nsess_add_fail_lock);
	seq_printf(seq,	"Total Number of Routing session add fail rt tbl full	: %u\n", nsess_add_fail_rt_tbl_full);
	seq_printf(seq,	"Total Number of Routing session add fail coll full	: %u\n", nsess_add_fail_coll_full);
	seq_printf(seq,	"Total Number of Routing session add fail mac tbl full	: %u\n", nsess_add_fail_mac_tbl_full);
	seq_printf(seq,	"Total Number of Routing session add fail ip tbl full	: %u\n", nsess_add_fail_ip_tbl_full);
	seq_printf(seq,	"Total Number of Routing session add fail rtp full	: %u\n", nsess_add_fail_rtp_full);
	seq_printf(seq,	"Total Number of Routing session add fail pppoe full	: %u\n", nsess_add_fail_pppoe_full);
	seq_printf(seq,	"Total Number of Routing session add fail mtu full	: %u\n", nsess_add_fail_mtu_full);
	seq_printf(seq,	"Total Number of Routing session add fail retun -1	: %u\n", nsess_add_fail_minus1);
	seq_printf(seq,	"Total Number of Routing session add fail index < 0	: %u\n", nsess_add_fail_negindex);
	seq_printf(seq,	"Total Number of Routing session add fail others	: %u\n", nsess_add_fail_oth);
	seq_printf(seq,	"=====================================================================\n");
	return 0;
}

static int proc_read_pae_rtstats_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_pae_rtstats, NULL);
}

static ssize_t proc_clear_pae_rtstats(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[40];
	char *p;

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

	if(strncmp(p,"clear",5)==0) {
		nsess_add=0;
		nsess_del=0;
		nsess_del_fail=0;
		nsess_add_succ=0;
		nsess_add_fail_lock=0;
		nsess_add_fail_rt_tbl_full=0;
		nsess_add_fail_coll_full=0;
		nsess_add_fail_mac_tbl_full=0;
		nsess_add_fail_ip_tbl_full=0;
		nsess_add_fail_rtp_full=0;
		nsess_add_fail_pppoe_full=0;
		nsess_add_fail_mtu_full=0;
		nsess_add_fail_minus1=0;
		nsess_add_fail_negindex=0;
		nsess_add_fail_oth=0;
		printk(KERN_INFO "PAE stats cleared!!!\n"); 
	} else {
		printk(KERN_INFO "usage : echo clear > /proc/ppa/pae/rtstats\n"); 
	}	
	
	return len; 
}

static int proc_read_pae_accel(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq,	"PAE Upstream Acceleration		: %s\n", g_us_accel_enabled ? "enabled" : "disabled");
	seq_printf(seq,	"PAE Downstream Acceleration	: %s\n", g_ds_accel_enabled ? "enabled" : "disabled");
	return 0;
}
 
static int proc_read_pae_accel_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_pae_accel, NULL);
}

static ssize_t proc_set_pae_accel(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[40];
	char *p;
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

	if(strncmp(p,"enable",6)==0) {
		if(len > 6) { 
			if(strncmp(p+7,"us",2)==0) {
				g_us_accel_enabled=3;
			} else if(strncmp(p+7,"ds",2)==0) { 
				g_ds_accel_enabled=3;
			}
		} else {
			g_us_accel_enabled=3;
			g_ds_accel_enabled=3;
		} 
		printk(KERN_INFO "Acceleration Enabled!!!\n"); 
	} else if (strncmp(p,"disable",7)==0){
		if(len > 7) { 
			if(strncmp(p+8,"us",2)==0) {
				g_us_accel_enabled=0;
			} else if(strncmp(p+8,"ds",2)==0) { 
				g_ds_accel_enabled=0;
			}
		} else {
			g_us_accel_enabled=0;
			g_ds_accel_enabled=0;
		} 
		printk(KERN_INFO "Acceleration Disabled!!!\n"); 
	} else {
		printk(KERN_INFO "usage : echo <enable/disable> [us/ds] > /proc/ppa/pae/accel\n"); 
	}
	
	return len; 
}

static int proc_read_pae_debug(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq,	"PAE Debug	: %s\n", pae_hal_dbg_enable ? "enabled" : "disabled");
	return 0;
}

static int proc_read_pae_debug_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_pae_debug, NULL);
}

static ssize_t proc_set_pae_debug(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len;
	char str[40];
	char *p;
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

	if(strncmp(p,"enable",6)==0) {
		pae_hal_dbg_enable=1;
		printk(KERN_INFO "Debug Enabled!!!\n"); 
	} else if (strncmp(p,"disable",7)==0){
		pae_hal_dbg_enable=0;
		printk(KERN_INFO "Debug Disbled!!!\n"); 
	} else {
		printk(KERN_INFO "usage : echo <enable/disable> > /proc/ppa/pae/dbg\n"); 
	}
	
	return len; 
}

#ifdef PAE_HW_METER
/* proc_read_pae_meter to dump hardware meter usage
 * cat /proc/ppa/pae/hw_meter
 * PAE H/W meters status	: enabled
 *	(RX,TX)[ref_count:meter_id]
 *				eth1_wan4
 *			0:0	10:1
 *	eth1_wan4	10:2	0:0
 */
static int proc_read_pae_meter(struct seq_file *seq, void *v)
{
	uint8_t rx_id = 0, tx_id = 0;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq,	"PAE H/W meters status	: %s\n",
			 pae_hal_hw_meter_enable ? "enabled" : "disabled");

	if (txid_bitmap == 1 && rxid_bitmap == 1)
		return 0;

	seq_printf(seq, "(RX,TX)[ref_count:meter_id]\n%17s", "");
	for_each_set_bit(tx_id, &txid_bitmap, PPA_MAX_IFS_NUM)
		seq_printf(seq,	"%16s:%-8lu", tx_meter[tx_id].ifname,
						tx_meter[tx_id].cnt);
	seq_printf(seq, "\n");
	for_each_set_bit(rx_id, &rxid_bitmap, PPA_MAX_IFS_NUM) {
		seq_printf(seq, "%16s:%-8lu", rx_meter[rx_id].ifname,
						rx_meter[rx_id].cnt);
		for_each_set_bit(tx_id, &txid_bitmap, PPA_MAX_IFS_NUM)
			seq_printf(seq,	"%8u:%-8u",
					 meterTable[rx_id][tx_id].ref_count,
					 meterTable[rx_id][tx_id].meter_id);
		seq_printf(seq, "\n");
	}
	return 0;
}

static int proc_read_pae_meter_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_pae_meter, NULL);
}

static ssize_t proc_set_pae_meter(struct file *file,
					const char __user *buf,
					size_t count,
					loff_t *data)
{
	int len;
	char str[40];
	char *p;
	uint8_t tx_id=0, rx_id=0;

	len = min(count, (size_t)(sizeof(str) - 1));
	len -= ppa_copy_from_user(str, buf, len);
	while ( len && str[len - 1] <= ' ' )
		len--;
	str[len] = 0;
	for ( p = str; *p && *p <= ' '; p++, len-- );
	if ( !*p )
		return count;

	if(strncmp(p,"enable",6) == 0) {
		if (!pae_hal_hw_meter_enable) {
			ppa_timer_add(&g_hit_meter_timer, g_polling_time);
			pae_hal_hw_meter_enable = 1;
		}
	} else if (strncmp(p,"disable",7) == 0){
		if (pae_hal_hw_meter_enable) {
			ppa_pae_meter_lock();
			ppa_timer_del(&g_hit_meter_timer);
			for_each_set_bit(rx_id, &rxid_bitmap, PPA_MAX_IFS_NUM) {
				for_each_set_bit(tx_id, &txid_bitmap, PPA_MAX_IFS_NUM) {
					put_meter_usage(rx_id,tx_id);
				}
			}
			ppa_pae_meter_unlock();
			memset(tx_meter, 0 ,sizeof(struct iface_index)*PPA_MAX_IFS_NUM );
			memset(rx_meter, 0 ,sizeof(struct iface_index)*PPA_MAX_IFS_NUM );
			meter_bitmap = 1;
			rxid_bitmap = 1;
			txid_bitmap = 1;
			pae_hal_hw_meter_enable = 0;
		}
	} else {
		printk(KERN_INFO "usage : echo <enable/disable> > /proc/ppa/pae/hw_meter\n");
	}
	return len;
}
#endif

static int proc_read_pae_lrotimeout(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq, "LRO engine aggregation timeout: %dmicros\n", g_lro_timeout);
	return 0;
}

static int proc_read_pae_lrotimeout_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_pae_lrotimeout, NULL);
}

static ssize_t proc_write_pae_lrotimeout(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len, tmp=0;
	char str[64];
	char *p;
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
	
	kstrtol(p, 10, (long*)&tmp);
	if(tmp) {
		if(tmp >= 0 && tmp <= 100000) { 
			g_lro_timeout = tmp;
		} else {
			printk(KERN_INFO "LRO aggregation timeout cannot be set to %d\n",tmp);
			return len;
		}
	}
	/* change the length on all the existing lro rules*/

	printk(KERN_INFO "LRO aggregation timeout changed to %d micro s\n", g_lro_timeout); 
	return len; 
}

static int proc_read_pae_lrolen(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq, "Minimum packet length forwarded to LRO engine: %d\n", g_min_lrolen);
	return 0;
}

static int proc_read_pae_lrolen_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_pae_lrolen, NULL);
}

static ssize_t proc_write_pae_lrolen(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len, tmp=0;
	char str[64];
	char *p;

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
	
	kstrtol(p, 10, (long*)&tmp);
	if(tmp) {
		if(tmp >= 0 && tmp <= 10000) { 
			g_min_lrolen = tmp;
		} else {
			printk(KERN_INFO "Minimum packet length cannot be set to %d\n",tmp);
			return len;
		}
	}

	printk(KERN_INFO "Minimum packet length forwarded to LRO changed to %d\n", g_min_lrolen); 
	return len; 
}
static int proc_read_pae_lroflush(struct seq_file *seq, void *v)
{
	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	seq_printf(seq, "usage : echo [1-8] > /proc/ppa/pae/lro/flush\n");
	return 0;
}
static int proc_read_pae_lroflush_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_pae_lroflush, NULL);
}

static ssize_t proc_write_pae_lroflush(struct file *file, const char __user *buf, size_t count, loff_t *data)
{
	int len, tmp=0;
	char str[64];
	char *p;
	int index= -1;

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
	
	kstrtol(p, 10, (long*)&tmp);
	if(tmp) {
		index = tmp;
	}
	
	if( index <= 0 || index > MAX_LRO_ENTRIES ) {
		printk(KERN_INFO "usage : echo [1-8] > /proc/ppa/pae/lro/flush\n"); 
	} else {
		del_lro_entry((uint8_t)index);
	}
	return len; 
}
#endif
/* wrapper function to call the ltq_ethsw_api_kioctl and check the return value
 * if the return value is GSW_statusLock_Failed then we need to retry SESSION_RETRY_MAX
   times before returning error to the caller
 */
static int32_t ltq_try_gswapi_kioctl(uint32_t command, uint32_t arg)
{
	uint16_t retry_conf=0;	 
	int32_t ret=0;
	GSW_API_HANDLE gswr = 0;

	 gswr = gsw_api_kopen("/dev/switch_api/1");
	
	if (gswr == 0) {
		dbg( "%s: Open SWAPI device FAILED !!\n", __func__ );
		return PPA_FAILURE;
	}

retry_config:

	if((ret = gsw_api_kioctl(gswr, command, (void *)arg)) < GSW_statusOk) {
		if( ret == GSW_statusLock_Failed) {
			retry_conf++;
			if(retry_conf < SESSION_RETRY_MAX) {
				goto retry_config;
			}
		}
	}
	
	gsw_api_kclose(gswr);
	
	return ret;
}

/* gswl_api_kioctl is wrapper function to call the ltq_ethsw_api_kioctl
 * for GSWIP-L.
 * return value is GSW_statusLock_Failed then we need to
 * retry SESSION_RETRY_MAX times before returning error to the caller.
 */
static int32_t gswl_api_kioctl(uint32_t command, uint32_t arg)
{
	uint16_t retry_conf = 0;
	int32_t ret = 0;
	GSW_API_HANDLE gswr = 0;

	gswr = gsw_api_kopen("/dev/switch_api/0");
	if (gswr == 0) {
		dbg("%s: Open SWAPI device FAILED !!\n", __func__);
		return PPA_FAILURE;
	}

retry_config:
	ret = gsw_api_kioctl(gswr, command, (void *)arg);
	if (ret < GSW_statusOk) {
		if (ret == GSW_statusLock_Failed) {
			retry_conf++;
			if (retry_conf < SESSION_RETRY_MAX)
				goto retry_config;
		}
	}
	gsw_api_kclose(gswr);
	return ret;
}

/*
* vlan table initialization for PAE egress vlan handling
*/
static	int32_t init_egress_vlan_table(void)
{
	int i, ret=0;
	GSW_cfg_t gswCfg={0};	
	GSW_PCE_EgVLAN_Cfg_t egVcfg={0};
	GSW_PCE_EgVLAN_Entry_t egV={0};

	ppa_memset (&gswCfg,0x00, sizeof(gswCfg));
	if((ret=ltq_try_gswapi_kioctl(GSW_CFG_GET,(unsigned int)&gswCfg)) < GSW_statusOk) {
			dbg( "GSW_CFG_GET returned faulure%d\n",ret);
			return ret;
	}
	

	gswCfg.bVLAN_Aware = 1;
	
	if((ret=ltq_try_gswapi_kioctl(GSW_CFG_SET,(unsigned int)&gswCfg)) < GSW_statusOk) {
			dbg( "GSW_CFG_SET returned faulure%d\n",ret);
			return ret;
	}

	for(i=0; i<MAX_PAE_PORTS; i++) {
		ppa_memset (&egVcfg, 0x00, sizeof(egVcfg));
		ppa_memset (&egV, 0x00, sizeof(egV));

		/* set the egress vlan treatment on port i as sub interface id based
		// set the base index for each port*/
		egVcfg.nPortId = i;
		egVcfg.bEgVidEna=1;
		egVcfg.eEgVLANmode=GSW_PCE_EG_VLAN_SUBIFID_BASED;
		egVcfg.nEgStartVLANIdx = vlan_tbl_base[i];
		if((ret=ltq_try_gswapi_kioctl( GSW_PCE_EG_VLAN_CFG_SET,(unsigned int)&egVcfg)) < GSW_statusOk) {
			dbg( "GSW_PCE_EG_VLAN_CFG_SET	returned faulure%d\n",ret);
			return ret; 
		}
		if(i > 0) {
			/* set the egress vlan tratment for port i index 0 as untag all*/
			egV.nPortId = i;
			egV.nIndex = vlan_tbl_base[i];
			egV.bEgVLAN_Action = 1;
			egV.bEgSVidRem_Action = 1;
			egV.bEgCVidRem_Action = 1;
			if((ret=ltq_try_gswapi_kioctl( GSW_PCE_EG_VLAN_ENTRY_WRITE,(unsigned int)&egV)) < GSW_statusOk) {
				dbg( "GSW_PCE_EG_VLAN_ENTRY_WRITE	returned faulure%d\n",ret);
				return ret; 
			}

		} else {
			/* no vlan action on port 0*/
			egV.nPortId = i;
			egV.nIndex = vlan_tbl_base[i];
			egV.bEgVLAN_Action = 1;

			if((ret=ltq_try_gswapi_kioctl( GSW_PCE_EG_VLAN_ENTRY_WRITE,(unsigned int)&egV)) < GSW_statusOk) {
				dbg( "GSW_PCE_EG_VLAN_ENTRY_WRITE	returned faulure%d\n",ret);
				return ret; 
			}
	
			/* set the egress vlan tratment for port 0 index 16 as no vlan action (for multicast and all unknown packets that gets forwarded to CPU)*/
			ppa_memset (&egV, 0x00, sizeof(egV));
			egV.nPortId = i;
			egV.nIndex = vlan_tbl_base[i] + vlan_tbl_size[i] - 2;
			egV.bEgVLAN_Action = 1;
			if((ret=ltq_try_gswapi_kioctl( GSW_PCE_EG_VLAN_ENTRY_WRITE,(unsigned int)&egV)) < GSW_statusOk) {
				dbg( "GSW_PCE_EG_VLAN_ENTRY_WRITE	returned faulure%d\n",ret);
				return ret; 
			}

			/* set the egress vlan tratment for port i index 17 as untag all (for multicast) in case of MPE complementary processing*/
			ppa_memset (&egV, 0x00, sizeof(egV));
			egV.nPortId = i;
			egV.nIndex = vlan_tbl_base[i] + vlan_tbl_size[i] - 1;
			egV.bEgVLAN_Action = 1;
			egV.bEgSVidRem_Action = 1;
			egV.bEgCVidRem_Action = 1;
			if((ret=ltq_try_gswapi_kioctl( GSW_PCE_EG_VLAN_ENTRY_WRITE,(unsigned int)&egV)) < GSW_statusOk) {
				dbg( "GSW_PCE_EG_VLAN_ENTRY_WRITE	returned faulure%d\n",ret);
				return ret; 
			}
		}
	}
	
	return PPA_SUCCESS;
}

static int32_t uninit_egress_vlan_table(void)
{
	int ret = 0, i;
	GSW_PCE_EgVLAN_Cfg_t egVcfg={0};
	GSW_cfg_t gswCfg={0};	

	for(i=0; i<MAX_PAE_PORTS; i++) {
		ppa_memset (&egVcfg, 0x00, sizeof(egVcfg));
		egVcfg.nPortId = i;
		egVcfg.bEgVidEna = 0;
		if((ret=ltq_try_gswapi_kioctl( GSW_PCE_EG_VLAN_CFG_SET,(unsigned int)&egVcfg)) < GSW_statusOk) {
			dbg( "GSW_PCE_EG_VLAN_CFG_SET	returned faulure\n");
			return ret; 
		}
	}

	ppa_memset (&gswCfg,0x00, sizeof(gswCfg));
	if((ret=ltq_try_gswapi_kioctl(GSW_CFG_GET,(unsigned int)&gswCfg)) < GSW_statusOk) {
		dbg( "GSW_CFG_GET returned faulure%d\n",ret);
		return ret;
	}

	gswCfg.bVLAN_Aware = 0;
	if((ret=ltq_try_gswapi_kioctl(GSW_CFG_SET,(unsigned int)&gswCfg)) < GSW_statusOk) {
		dbg( "GSW_CFG_SET returned faulure%d\n",ret);
		return ret;
	}

	return PPA_SUCCESS;
}

static int32_t init_pae_flows(void)
{
	PPA_CLASS_RULE rule;	 
	int ret = 0;
	const char *end;

#if IS_ENABLED(CONFIG_CBM_LS_ENABLE) && CONFIG_CBM_LS_ENABLE 
	/* pce rule for tcp load spreader*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));
	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_MGMT; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_TCP; 
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_TCP); 

	rule.pattern.bPortIdEnable=1;
	rule.pattern.nPortId=0;
	rule.pattern.bPortId_Exclude=1;

	rule.action.qos_action.flowid_enabled = 1;
	rule.action.qos_action.flowid=0x40;

	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}
#endif
	/* port 0 exclude*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_MGMT;

	rule.pattern.bEnable=1;	
	rule.pattern.bPortIdEnable=1;
	rule.pattern.nPortId=0;

	rule.action.fwd_action.rtaccelenable = 0;
	rule.action.fwd_action.rtctrlenable = 1;
	
	rule.action.fwd_action.processpath = 4; /* CPU	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_CPU_ING_CNTR;

	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
	dbg( "add pce rule returned failure %d\n",ret);
	return PPA_FAILURE;
	}

	/* Forward padded packet (class bit 3) to CPU */
	ppa_memset(&rule, 0, sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_FWD;

	rule.pattern.bEnable=1;

	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = PCE_PARSER_LSB_4TH_VLAN;
	rule.pattern.nParserFlagLSB_Mask = (uint16_t)(~PCE_PARSER_LSB_4TH_VLAN);

	rule.action.fwd_action.portmap = 4;
	rule.action.fwd_action.forward_portmap = 0x1;

	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_CPU_ING_CNTR; 
	
	if (pae_hal_add_class_rule(&rule) != PPA_SUCCESS) {
		dbg("add pce rule returned failure %d\n", ret);
		return PPA_FAILURE;
	}

	/* tcp unicast*/
	ppa_memset(&rule, 0, sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_FWD; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_TCP; 
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS | 
					PCE_PARSER_MSB_TCP | PCE_PARSER_MSB_RT_EXCEP |	PCE_PARSER_MSB_INNR_IPV6 | PCE_PARSER_MSB_INNR_IPV4); 

	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB_Mask = ~(PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP);

	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_TCP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_TCP_CNTR; 

	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
	dbg( "add pce rule returned failure %d\n",ret);
	return PPA_FAILURE;
	}

	/* multicast v4 */
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_FWD; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_UDP_HDR_AFTR_FST_IP_HDR;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS |
					PCE_PARSER_MSB_UDP_HDR_AFTR_FST_IP_HDR | PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_INNR_IPV6 | PCE_PARSER_MSB_INNR_IPV4);

	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = PCE_PARSER_LSB_OUTR_IPV4;
	rule.pattern.nParserFlagLSB_Mask = (uint16_t)(~(PCE_PARSER_LSB_OUTR_IPV4 | PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP | PCE_PARSER_LSB_WOL));

	rule.pattern.eDstIP_Select = 1;
	rule.pattern.nDstIP.nIPv4 = in_aton("224.0.0.0");
	rule.pattern.nDstIP_Mask= PCE_IPV4_MCAST_MASK;	 /* 8 bit mask for ipv4 each bit used to mask a nibble*/

	rule.action.fwd_action.rtdestportmaskcmp = 0;
	rule.action.fwd_action.rtsrcportmaskcmp = 0;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1 */	
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_MCAST_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
	dbg( "add pce rule returned failure %d\n",ret);
	return PPA_FAILURE;
	}

	/*multicast v6*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_FWD; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_UDP_HDR_AFTR_FST_IP_HDR | PCE_PARSER_MSB_OUTR_IPV6;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS |
				PCE_PARSER_MSB_UDP_HDR_AFTR_FST_IP_HDR | PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_INNR_IPV6 | 
				PCE_PARSER_MSB_INNR_IPV4 | PCE_PARSER_MSB_OUTR_IPV6);

	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = 0x0000;
	rule.pattern.nParserFlagLSB_Mask = ~( PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP | PCE_PARSER_LSB_WOL);

	rule.pattern.eDstIP_Select = 2;
	in6_pton("ff00:0:0:0:0:0:0:0",INET6_ADDRSTRLEN,(void*)&rule.pattern.nDstIP.nIPv6,-1,&end);
	rule.pattern.nDstIP_Mask= PCE_IPV6_MCAST_MASK;	/*32 bit mask for 128 bit ipv6 address one bit per nibble*/

	rule.action.fwd_action.rtdestportmaskcmp = 0;
	rule.action.fwd_action.rtsrcportmaskcmp = 0;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1 */	
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_MCAST_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
	dbg( "add pce rule returned failure %d\n",ret);
	return PPA_FAILURE;
	}

	/* udp unicast v4*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_FWD; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_UDP_HDR_AFTR_FST_IP_HDR;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS |
				PCE_PARSER_MSB_UDP_HDR_AFTR_FST_IP_HDR | PCE_PARSER_MSB_RT_EXCEP |	PCE_PARSER_MSB_INNR_IPV6 | PCE_PARSER_MSB_INNR_IPV4);

	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = PCE_PARSER_LSB_OUTR_IPV4;
	rule.pattern.nParserFlagLSB_Mask = (uint16_t)(~(PCE_PARSER_LSB_OUTR_IPV4 | PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP | PCE_PARSER_LSB_WOL));

	rule.pattern.eDstIP_Select = 1;
	rule.pattern.nDstIP.nIPv4 = in_aton("224.0.0.0");
	rule.pattern.nDstIP_Mask= PCE_IPV4_MCAST_MASK;	 /* 8 bit mask for ipv4 each bit used to mask a nibble*/
	rule.pattern.bDstIP_Exclude= 1;

	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_UDP_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
	dbg( "add pce rule returned failure %d\n",ret);
	return PPA_FAILURE;
	}
 
	/* udp unicast v6 */
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_FWD; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_UDP_HDR_AFTR_FST_IP_HDR | PCE_PARSER_MSB_OUTR_IPV6;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS |
					PCE_PARSER_MSB_UDP_HDR_AFTR_FST_IP_HDR | PCE_PARSER_MSB_RT_EXCEP |	PCE_PARSER_MSB_INNR_IPV6 | 
					PCE_PARSER_MSB_INNR_IPV4 | PCE_PARSER_MSB_OUTR_IPV6);
	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = 0x0000;
	rule.pattern.nParserFlagLSB_Mask = ~( PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP | PCE_PARSER_LSB_WOL);

	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1 */	
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_UDP_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}
 
	/* 6rd Downstream TCP*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_TCP | PCE_PARSER_MSB_INNR_IPV6;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_INNR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IP_FRAGMT |
					PCE_PARSER_MSB_IPV4_OPTNS | PCE_PARSER_MSB_TCP | PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_INNR_IPV6);
	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = PCE_PARSER_LSB_OUTR_IPV4;
	rule.pattern.nParserFlagLSB_Mask = (uint16_t)(~(PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_OUTR_IPV4));

	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_TCP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_6RD_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

	/* 6rd Downstream UDP Multicast*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV6;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV6_WITH_EXTN_HDR |
					PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_IPV4_OPTNS | PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_INNR_IPV6);
	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = PCE_PARSER_LSB_OUTR_IPV4;
	rule.pattern.nParserFlagLSB_Mask = (uint16_t)(~(PCE_PARSER_LSB_OUTR_IPV4 | PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP));

	rule.pattern.eInnerDstIP_Select= 2;
	in6_pton("ff00:0:0:0:0:0:0:0",INET6_ADDRSTRLEN,(void*)&rule.pattern.nInnerDstIP.nIPv6,-1,&end);
	rule.pattern.nInnerDstIP_Mask= PCE_IPV6_MCAST_MASK;
	
	rule.action.fwd_action.rtdestportmaskcmp = 0;
	rule.action.fwd_action.rtsrcportmaskcmp = 0;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_6RD_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

	/* 6rd Downstream UDP*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV6;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV6_WITH_EXTN_HDR |
					PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_IPV4_OPTNS | PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_INNR_IPV6);

	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = PCE_PARSER_LSB_OUTR_IPV4;
	rule.pattern.nParserFlagLSB_Mask = (uint16_t)(~(PCE_PARSER_LSB_OUTR_IPV4 | PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP));
	
	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_6RD_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

	/* dslite Downstream TCP */
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_TCP | PCE_PARSER_MSB_INNR_IPV4 | PCE_PARSER_MSB_OUTR_IPV6;
	rule.pattern.nParserFlagMSB_Mask = ~( PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_INNR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IP_FRAGMT |
					PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS | PCE_PARSER_MSB_TCP |
					PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_INNR_IPV4 | PCE_PARSER_MSB_OUTR_IPV6);

	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_TCP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_DSLITE_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

	/* dslite Downstream UDP Multicast*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV4 | PCE_PARSER_MSB_OUTR_IPV6;
	rule.pattern.nParserFlagMSB_Mask = ~( PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV6_WITH_EXTN_HDR | 
					PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_RT_EXCEP | 
					PCE_PARSER_MSB_INNR_IPV4 | PCE_PARSER_MSB_OUTR_IPV6);

	rule.pattern.eInnerDstIP_Select= 1;
	rule.pattern.nInnerDstIP.nIPv4 = in_aton("224.0.0.0");
	rule.pattern.nInnerDstIP_Mask= PCE_IPV4_MCAST_MASK; 
	
	rule.action.fwd_action.rtdestportmaskcmp = 0;
	rule.action.fwd_action.rtsrcportmaskcmp = 0;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_DSLITE_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

	/* dslite Downstream UDP*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV4 | PCE_PARSER_MSB_OUTR_IPV6;
	rule.pattern.nParserFlagMSB_Mask = ~( PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV6_WITH_EXTN_HDR | 
					PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_RT_EXCEP | 
					PCE_PARSER_MSB_INNR_IPV4 | PCE_PARSER_MSB_OUTR_IPV6);
	 
	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1 */	
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_DSLITE_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

	/* L2TP Downstream TCP */
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_TCP;
	rule.pattern.nParserFlagMSB_Mask = ~( PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_INNR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IP_FRAGMT | 
					PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS | PCE_PARSER_MSB_TCP | 
					PCE_PARSER_MSB_RT_EXCEP);

	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB_Mask = ~(PCE_PARSER_LSB_GRE);
	
	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_TCP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_L2TP_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

	/* L2TP Downstream UDP Multicast inner ipv6*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV6;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV6_WITH_EXTN_HDR |
					PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS | 
					PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_RT_EXCEP);

	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB_Mask = ~(PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP);
	
	rule.pattern.eInnerDstIP_Select= 2;
	in6_pton("ff00:0:0:0:0:0:0:0",INET6_ADDRSTRLEN,(void*)&rule.pattern.nInnerDstIP.nIPv6,-1,&end);
	rule.pattern.nInnerDstIP_Mask= PCE_IPV6_MCAST_MASK;

	rule.action.fwd_action.rtdestportmaskcmp = 0;
	rule.action.fwd_action.rtsrcportmaskcmp = 0;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_L2TP_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

 	/* L2TP Downstream UDP Multicast inner ipv4*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV4;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_IP_FRAGMT | 
					PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS | PCE_PARSER_MSB_RT_EXCEP | 
					PCE_PARSER_MSB_INNR_IPV4);


	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB_Mask = ~(PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP);
	
	rule.pattern.eInnerDstIP_Select= 1;
	rule.pattern.nInnerDstIP.nIPv4 = in_aton("224.0.0.0");
	rule.pattern.nInnerDstIP_Mask= PCE_IPV4_MCAST_MASK; 
		
	rule.action.fwd_action.rtdestportmaskcmp = 0;
	rule.action.fwd_action.rtsrcportmaskcmp = 0;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1 */	
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_L2TP_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
	dbg( "add pce rule returned failure %d\n",ret);
	return PPA_FAILURE;
	}

	/* L2TP downstream UDP inner ipv6*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV6;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV6_WITH_EXTN_HDR |
					PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS | 
					PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_RT_EXCEP);

	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB_Mask = ~(PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP);
	
	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1 */	
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_L2TP_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add pce rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

	/* L2TP downstream UDP inner ipv4*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_INNR_IPV4;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_L2TP_DATA | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR | PCE_PARSER_MSB_IP_FRAGMT | 
					PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS | PCE_PARSER_MSB_RT_EXCEP | 
					PCE_PARSER_MSB_INNR_IPV4);

	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB_Mask = ~(PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP);
	
	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	 
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1 */	
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_L2TP_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
	dbg( "add pce rule returned failure %d\n",ret);
	return PPA_FAILURE;
	}

	/* CAPWAP downstream traffic */
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = PCE_PARSER_LSB_CAPWAP;
	rule.pattern.nParserFlagLSB_Mask = ~(PCE_PARSER_LSB_GRE | PCE_PARSER_LSB_CAPWAP);

	rule.action.fwd_action.portmap = 4;
	rule.action.fwd_action.forward_portmap = 0x2000;
	rule.action.fwd_action.rtaccelenable = 0;
	rule.action.fwd_action.rtctrlenable = 1;
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_CAPWAP_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add capwap rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}
 
	/* GRE traffic - TCP*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_TCP; 
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_IP_FRAGMT | 
				PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS |
				PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_TCP );
	
	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = PCE_PARSER_LSB_GRE;
	rule.pattern.nParserFlagLSB_Mask = ~(PCE_PARSER_LSB_GRE);

	rule.action.fwd_action.rtinneripaskey = 1;
	
	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_TCP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;

	rule.action.fwd_action.processpath = 1; /* MPE1 */

	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_GRE_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add tcp gre rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

	/* GRE Downstream Multicast*/
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_IP_FRAGMT | 
				PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS |
				PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR );
	
	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = PCE_PARSER_LSB_GRE;
	rule.pattern.nParserFlagLSB_Mask = ~(PCE_PARSER_LSB_GRE|PCE_PARSER_LSB_CAPWAP);

	rule.pattern.eInnerDstIP_Select= 1;
	rule.pattern.nInnerDstIP.nIPv4 = in_aton("224.0.0.0");
	rule.pattern.nInnerDstIP_Mask= PCE_IPV4_MCAST_MASK; 
	 
	rule.action.fwd_action.rtinneripaskey = 1;
 
	rule.action.fwd_action.rtdestportmaskcmp = 0;
	rule.action.fwd_action.rtsrcportmaskcmp = 0;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;

	rule.action.fwd_action.processpath = 1; /* MPE1 */

	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_GRE_CNTR; 

	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add multicast gre rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

	/* GRE traffic - UDP */
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_IP_FRAGMT | 
				PCE_PARSER_MSB_OUTR_IPV6_WITH_EXTN_HDR | PCE_PARSER_MSB_IPV4_OPTNS |
				PCE_PARSER_MSB_RT_EXCEP | PCE_PARSER_MSB_UDP_HDR_AFTR_SEC_IP_HDR );
	
	rule.pattern.bParserFlagLSB_Enable = 1;
	rule.pattern.nParserFlagLSB = PCE_PARSER_LSB_GRE;
	rule.pattern.nParserFlagLSB_Mask = ~(PCE_PARSER_LSB_GRE|PCE_PARSER_LSB_CAPWAP);

	rule.action.fwd_action.rtinneripaskey = 1;

	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_UDP;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.rtctrlenable = 1;

	rule.action.fwd_action.processpath = 1; /* MPE1 */

	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_GRE_CNTR; 

	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
		dbg( "add udp gre rule returned failure %d\n",ret);
		return PPA_FAILURE;
	}

#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	/* IPSec Downstream */
	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_TUN; 

	rule.pattern.bEnable=1;	
	rule.pattern.bProtocolEnable=1;	
	rule.pattern.nProtocol = PCE_PROTO_ESP;
	rule.pattern.nProtocolMask =(u8) ~(0xFF);

	rule.pattern.bParserFlagMSB_Enable = 1;
	rule.pattern.nParserFlagMSB_Mask = ~(PCE_PARSER_MSB_IP_FRAGMT | PCE_PARSER_MSB_RT_EXCEP);

	rule.action.fwd_action.rtctrlenable = 1;
	rule.action.fwd_action.rtaccelenable = 1;
	rule.action.fwd_action.routextid_enable = 1;
	rule.action.fwd_action.routextid = RT_EXTID_IPSEC;
	rule.action.fwd_action.rtdstipmaskcmp = 1;
	rule.action.fwd_action.rtsrcipmaskcmp = 1; 
	rule.action.fwd_action.rtdestportmaskcmp = 1;
	rule.action.fwd_action.rtsrcportmaskcmp = 1;
	
	rule.action.fwd_action.processpath = 1; /* MPE1	*/
		 
	rule.action.rmon_action = 1;
	rule.action.rmon_id = RMON_IPSEC_CNTR; 
	
	if(pae_hal_add_class_rule(&rule)!=PPA_SUCCESS) {
	dbg( "add pce rule returned failure %d\n",ret);
	return PPA_FAILURE;
	}
#endif

	return PPA_SUCCESS;
}

static int32_t uninit_pae_flows(void) 
{	 
	int i, ret=PPA_SUCCESS;
	PPA_CLASS_RULE rule;	 

	ppa_memset(&rule,0,sizeof(PPA_CLASS_RULE));
	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_MGMT; 
	for(i=class_dev[GSWR_INGRESS].cat_map[CAT_MGMT].cat_last_ordr; i >= 0; i--) {
		rule.order = i+1;
		pae_hal_del_class_rule(&rule);		
	}
	rule.category = CAT_FWD; 
	for(i=class_dev[GSWR_INGRESS].cat_map[CAT_FWD].cat_last_ordr; i >= 0; i--) {
		rule.order = i+1;
		pae_hal_del_class_rule(&rule);		
	}
	rule.category = CAT_TUN; 
	for(i=class_dev[GSWR_INGRESS].cat_map[CAT_TUN].cat_last_ordr; i >= 0; i--) {
		rule.order = i+1;
		pae_hal_del_class_rule(&rule);		
	}
	dbg( "PCE rule disable returned success\n");	
	return ret;
}

static uint32_t get_routing_extension_id(uint32_t proto)
{
	int ret = 0;
/* By default there will be two flow rules one for TCP and one for UDP
// so the routing extension ids by default is 0x100 for udp and 0x200 for tcp*/
	switch(proto) {
	case IP_PROTO_TCP:
		ret = RT_EXTID_TCP;
		break;
	case IP_PROTO_UDP:
		ret = RT_EXTID_UDP;
		break;
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	case IP_PROTO_ESP:
		ret = RT_EXTID_IPSEC;
		break;
#endif
	default:
		break;
	}
	return ret;
}

/*
 * ####################################
 *			 Global Function
 * ####################################
 */

void get_pae_hal_id(uint32_t *p_family,
					uint32_t *p_type,
					uint32_t *p_if,
					uint32_t *p_mode,
					uint32_t *p_major,
					uint32_t *p_mid,
					uint32_t *p_minor)
{
	if ( p_family )
		*p_family = VER_FAMILY;

	if ( p_type )
		*p_type = VER_DRTYPE;

	if ( p_if )
		*p_if = VER_INTERFACE;

	if ( p_mode )
		*p_mode = VER_ACCMODE;

	if ( p_major )
		*p_major = VER_MAJOR;

	if ( p_mid )
		*p_mid = VER_MID;

	if ( p_minor )
		*p_minor = VER_MINOR;
}

/*!
	\fn uint32_t get_firmware_id(uint32_t *id,
					 char *name,
					 char *version)
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief read firmware ID
	\param p_family	 get family code
	\param p_type	 get firmware type
	\return no return value
 */
int32_t get_firmware_id(uint32_t *id,
					 char *p_name,
					 char *p_version)
{
	GSW_version_t sw_version;
	int ret=0;
	
	ppa_memset(&sw_version,0,sizeof(sw_version));

	if((ret=ltq_try_gswapi_kioctl( GSW_VERSION_GET, (unsigned int)&sw_version)) >= GSW_statusOk) {
	*id=sw_version.nId;
	ppa_memcpy(p_name,sw_version.cName, PPA_VERSION_LEN);
	ppa_memcpy(p_version,sw_version.cVersion, PPA_VERSION_LEN);
	} else {
	return ret;
	}

	return PPA_SUCCESS;
}

uint32_t get_number_of_phys_port(void)
{
	return MAX_PAE_PORTS;	
}


/*TBD: modify this fn to read this information from dp driver*/
void get_phys_port_info(uint32_t port, uint32_t *p_flags,
				PPA_IFNAME ifname[PPA_IF_NAME_SIZE])
{
	char *str_ifname[] = {
		"",
		"eth0_0",
		"eth0_1",
		"eth0_2",
		"eth0_3",
		"eth0_4",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"eth1"
	};

	if ( port >= sizeof(str_ifname) / sizeof(*str_ifname) ) {
		if ( p_flags )
			*p_flags = 0;
		if ( ifname )
			*ifname = 0;
	return;
	}

	if(p_flags) {
		*p_flags = 0;
		switch(port) {
			case 0: /*CPU port */
				*p_flags = PPA_PHYS_PORT_FLAGS_MODE_CPU_VALID;
				*ifname = 0;
				break;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				if( g_port_map & ( 1 << port)) { 	
					*p_flags = PPA_PHYS_PORT_FLAGS_MODE_ETH_LAN_VALID;
					*p_flags |= PPA_PHYS_PORT_FLAGS_OUTER_VLAN;
				}
				break;
			case 7: 
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				if( g_port_map & ( 1 << port)) { 	
					*p_flags = PPA_PHYS_PORT_FLAGS_MODE_ETH_MIX_VALID;
					*p_flags |= PPA_PHYS_PORT_FLAGS_OUTER_VLAN;
				}
				break;
			case 15: /*eth1*/
				if( g_port_map & ( 1 << port)) { 	
					*p_flags = PPA_PHYS_PORT_FLAGS_MODE_ETH_WAN_VALID;
					*p_flags |= PPA_PHYS_PORT_FLAGS_OUTER_VLAN;
				}
				break;
			default:
				*p_flags = 0;
				break;
		}
	}
	if ( ifname )
		snprintf(ifname, PPA_IF_NAME_SIZE, "%s", str_ifname[port]);
}

/*!
	\fn void get_max_route_entries(uint32_t *p_entry,
							 uint32_t *p_mc_entry)
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief get maximum number of routing entries
	\param p_entry	get maximum number of uni-cast routing entries. 
	\param p_mc_entry get maximum number of multicast routing entries. In Amazon-S (AR9) D5, there are 64 entries.
	\return no return value
 */
void get_max_route_entries(uint32_t *p_entry,
				uint32_t *p_mc_entry)
{
	if ( p_entry )
		*p_entry = MAX_ROUTING_ENTRIES;

	if ( p_mc_entry )
		*p_mc_entry = MAX_WAN_MC_ENTRIES;
}

void get_max_bridging_entries(uint32_t *p_entry)
{
	if ( p_entry )
		*p_entry = MAX_BRIDGING_ENTRIES;	
}

/*!
Not supported
*/
void set_route_cfg(uint32_t f_is_lan,
			uint32_t entry_num,
			uint32_t mc_entry_num,
			uint32_t flags,
			uint8_t	f_mpe_route,
			uint8_t	f_l2tp_ds,
			uint8_t	f_mc_vaps)
{
}

void set_bridging_cfg(uint32_t entry_num,
			uint32_t br_to_src_port_mask, 
			uint32_t br_to_src_port_en,
			uint32_t f_dest_vlan_en,
			uint32_t f_src_vlan_en,
			uint32_t f_mac_change_drop,
			uint32_t flags)
{
}

/*!
	\fn void get_acc_mode(uint32_t f_is_lan, uint32_t *p_acc_mode)
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief get acceleration mode for interfaces (LAN/WAN)
	\param f_is_lan		 0: WAN interface, 1: LAN interface
	\param p_acc_mode	 a u32 data pointer to get acceleration mode (PPA_ACC_MODE_ROUTING / PPA_ACC_MODE_NONE)
	\return no return value
 */
void get_acc_mode(uint32_t f_is_lan, uint32_t *p_acc_mode)
{
	if(f_is_lan)
		*p_acc_mode = g_us_accel_enabled;
	else
		*p_acc_mode = g_ds_accel_enabled;
}

/*!
	\fn void set_acc_mode(uint32_t f_is_lan,
					uint32_t acc_mode)
	\brief set acceleration mode for interfaces (LAN/WAN)
	\param f_is_lan		 0: WAN interface, 1: LAN interface
	\param p_acc_mode	 acceleration mode (PPA_ACC_MODE_ROUTING / PPA_ACC_MODE_NONE/ PPA_ACC_MODE_BRIDGING/ PPA_ACC_MODE_HYBRID)
	\return no return value
//	acc_mode:
//	0: no acceleration
//	1: Bridge learning
//	2: routing acceleration
//	3: routing acceleration + bridge Learinig enabled
*/
void set_acc_mode(uint32_t f_is_lan, uint32_t acc_mode)
{

if(f_is_lan)
	g_us_accel_enabled = acc_mode;
else
	g_ds_accel_enabled = acc_mode;
}

int32_t is_ipv6_enabled(void)
{
	return g_ipv6_enabled;
}

#ifdef PAE_HW_METER
/* allocate_meter(rx,tx)
 * Used to allocate and return meter if not exists
 * else increase and the usage count and return same meter
 * for given (rx,tx) index.
 */
uint8_t allocate_meter(uint8_t rx_id, uint8_t tx_id)
{
	GSW_QoS_meterCfg_t param;
	GSW_RMON_clear_t clear_rmon;

	if (meterTable[rx_id][tx_id].ref_count > 0) {
		meterTable[rx_id][tx_id].ref_count++;
		return meterTable[rx_id][tx_id].meter_id;
	}

	param.nMeterId = find_first_zero_bit(&meter_bitmap, METER_ID_MAX);
	param.bEnable = true;
	param.eMtrType = GSW_QOS_Meter_srTCM;
        /* Committed Burst Size (CBS [Bytes]). */
	param.nCbs = 0x9300;
 	/** Excess Burst Size (EBS [Bytes]). */
	param.nEbs = 0x8700;
        /* Committed Information Rate (CIR [kbit/s]) */
        param.nRate = 0x124F800; /* 1GBps*/

	if (ltq_try_gswapi_kioctl(GSW_QOS_METER_CFG_SET,
					(unsigned int)&param) < GSW_statusOk) {
		meterTable[rx_id][tx_id].meter_id = 0x00;
	} else {
		clear_rmon.eRmonType = 3; /* eRmonType H/w meter type */
		clear_rmon.nRmonId = param.nMeterId;
		ltq_try_gswapi_kioctl(GSW_RMON_CLEAR,
						(unsigned int)&clear_rmon);
		meterTable[rx_id][tx_id].meter_id = param.nMeterId;
		meterTable[rx_id][tx_id].ref_count++;
		set_bit(meterTable[rx_id][tx_id].meter_id, &meter_bitmap);
	}
	return meterTable[rx_id][tx_id].meter_id;
}

/* put_meter_usage(rx,tx)
 * Used to put/release the meter for given (rx,tx) index
 */
uint32_t put_meter_usage(uint8_t rx_id, uint8_t tx_id)
{
	GSW_QoS_meterCfg_t param;

	if (meterTable[rx_id][tx_id].ref_count == 0)
		return PPA_SUCCESS;

	meterTable[rx_id][tx_id].ref_count--;

	if (meterTable[rx_id][tx_id].ref_count == 0) {
		param.bEnable = false;
		param.nMeterId = meterTable[rx_id][tx_id].meter_id;
		meterTable[rx_id][tx_id].meter_id = 0;
		if (ltq_try_gswapi_kioctl(GSW_QOS_METER_CFG_SET,
						(unsigned int)&param) < GSW_statusOk) {
			dbg("\n Unable to free meter[%u]", param.nMeterId);
		}
		clear_bit(param.nMeterId, &meter_bitmap);
	}
	return PPA_SUCCESS;
}

/* put_iface_idx(iface, is_tx)
 * iface -> for a givene iface, is_tx-> 0/1=>rx/tx
 * Used to put/release interface row/column for interface
 * if not used further.
 */
uint32_t put_iface_idx(PPA_IFNAME *pifname ,bool is_tx)
{
	struct iface_index *meter_t;
	long unsigned int *index_map;
	bool found = false;
	int midx, indx;

	meter_t = (is_tx) ? tx_meter : rx_meter;
	index_map = (is_tx) ? &txid_bitmap : &rxid_bitmap;

	for_each_set_bit(indx, index_map, PPA_MAX_IFS_NUM)
		if(strncmp(meter_t[indx].ifname, pifname,
				 PPA_IF_NAME_SIZE) == 0) {
			found = true;
			break;
		}
	if (!found) {
		pr_err("No iface[%s] found in is_tx(%d) table\n",
				 pifname, is_tx);
		return 0;
	}

	if (is_tx) {
		for (midx=0; midx < PPA_MAX_IFS_NUM; midx++)
			if (meterTable[midx][indx].ref_count > 0)
				return PPA_SUCCESS;
	} else {
		for (midx=0; midx < PPA_MAX_IFS_NUM; midx++)
			if (meterTable[indx][midx].ref_count > 0)
				return PPA_SUCCESS;
	}
	clear_bit(indx, index_map);
	memset(&meter_t[indx], 0x00, sizeof(struct iface_index));
	return PPA_SUCCESS;
}

/* get_iface_idx(iface, is_tx)
 * iface -> for a givene iface, is_tx-> 0/1=>rx/tx
 * Used to allocate interface row/column for interface
 * if already present return the index.
 */
uint8_t get_iface_idx(PPA_IFNAME *pifname ,bool is_tx)
{
	int indx;
	long unsigned int *index_map;
	struct iface_index *meter_t;

	if ((!pifname) || (pifname && !pifname[0])) {
		pr_err("No iface[%s] specified for is_tx(%d) table\n",
				 pifname, is_tx);
		return 0;
	}

	meter_t = (is_tx) ? tx_meter : rx_meter;
	index_map = (is_tx) ? &txid_bitmap : &rxid_bitmap;

	for_each_set_bit(indx, index_map, PPA_MAX_IFS_NUM)
		if (strncmp(meter_t[indx].ifname, pifname,
				PPA_IF_NAME_SIZE) == 0)
			return indx;

	indx = find_first_zero_bit(index_map, PPA_MAX_IFS_NUM);
	if (indx >= PPA_MAX_IFS_NUM)
		return 0;

	memset(&meter_t[indx], 0x00, sizeof(struct iface_index));
	ppa_strncpy(meter_t[indx].ifname, pifname, PPA_IF_NAME_SIZE);
	set_bit(indx, index_map);
	return indx;
}

/* get_session_nMeterId(route, rt_entry)
 * for a given (rx_iface, tx_iface) pair
 * find the appropraite meter and attach to the session
 * Same is updated and attached in the rt_entry.
 */
int32_t get_session_nMeterId(struct uc_session_node *p_item,
				 GSW_ROUTE_Entry_t *rt_entry)
{
	uint8_t rx_id = 0, tx_id = 0, meter_id = 0;
	PPA_IFNAME rxifname[PPA_IF_NAME_SIZE] = {0};
	PPA_IFNAME txifname[PPA_IF_NAME_SIZE] = {0};

	if (!pae_hal_hw_meter_enable)
		return PPA_SUCCESS;
	if (!p_item->is_rx_logical && !p_item->is_tx_logical)
		return PPA_SUCCESS;

	if (p_item->is_rx_logical) {
		if (p_item->br_rx_if)
			ppa_strncpy(rxifname, ppa_get_netif_name(p_item->br_rx_if), PPA_IF_NAME_SIZE);
		else if (p_item->rx_if)
			ppa_strncpy(rxifname, ppa_get_netif_name(p_item->rx_if), PPA_IF_NAME_SIZE);

		if (rxifname[0])
			rx_id = get_iface_idx(rxifname, 0);
	}
	if (p_item->is_tx_logical) {
		if (p_item->br_tx_if)
			ppa_strncpy(txifname, ppa_get_netif_name(p_item->br_tx_if), PPA_IF_NAME_SIZE);
		else if (p_item->tx_if)
			ppa_strncpy(txifname, ppa_get_netif_name(p_item->tx_if), PPA_IF_NAME_SIZE);

		if (txifname[0])
			tx_id = get_iface_idx(txifname, 1);
	}

	if ((rx_id == 0) && (tx_id == 0)) {
		dbg("\nrx_id and tx_id are NULL\n");
		return PPA_SUCCESS;
	}

	meter_id = allocate_meter(rx_id, tx_id);
	if (meter_id > 0) {
		rt_entry->routeEntry.action.nMeterId = meter_id;
		rt_entry->routeEntry.action.bMeterAssign = true;
	}
	return PPA_SUCCESS;
}

/* put_session_nMeterId(route, rt_entry)
 * for a given (rx_iface, tx_iface) pair release the meter used
 */
int32_t put_session_nMeterId(struct uc_session_node *p_item)
{
	uint8_t rx_id = 0, tx_id = 0;
	struct iface_index *meter_tx, *meter_rx;
	PPA_IFNAME rxifname[PPA_IF_NAME_SIZE] = {0};
	PPA_IFNAME txifname[PPA_IF_NAME_SIZE] = {0};

	if (!p_item->is_rx_logical && !p_item->is_tx_logical)
		return PPA_SUCCESS;

	if (p_item->is_rx_logical) {
		if (p_item->br_rx_if)
			ppa_strncpy(rxifname, ppa_get_netif_name(p_item->br_rx_if), PPA_IF_NAME_SIZE);
		else if (p_item->rx_if)
			ppa_strncpy(rxifname, ppa_get_netif_name(p_item->rx_if), PPA_IF_NAME_SIZE);

		if (rxifname[0]) {
			meter_rx = rx_meter;
			for_each_set_bit(rx_id, &rxid_bitmap, PPA_MAX_IFS_NUM)
				if (strncmp(meter_rx[rx_id].ifname, rxifname,
						 PPA_IF_NAME_SIZE) == 0)
					break;

			if (rx_id >= PPA_MAX_IFS_NUM)
				rx_id = 0;
		}
	}

	if (p_item->is_tx_logical) {
		if (p_item->br_tx_if)
			ppa_strncpy(txifname, ppa_get_netif_name(p_item->br_tx_if), PPA_IF_NAME_SIZE);
		else if (p_item->tx_if)
			ppa_strncpy(txifname, ppa_get_netif_name(p_item->tx_if), PPA_IF_NAME_SIZE);

		if (txifname[0]) {
			meter_tx = tx_meter;
			for_each_set_bit(tx_id, &txid_bitmap, PPA_MAX_IFS_NUM)
				if (strncmp(meter_tx[tx_id].ifname, txifname,
						 PPA_IF_NAME_SIZE) == 0)
					break;

			if (tx_id >= PPA_MAX_IFS_NUM)
				tx_id = 0;
		}
	}

	if ((rx_id == 0) && (tx_id == 0)) {
		dbg("\nrx_id and tx_id are NULL\n");
		return PPA_SUCCESS;
	}

	put_meter_usage(rx_id, tx_id);

	if (rx_id)
		put_iface_idx(meter_rx[rx_id].ifname, 0);
	if (tx_id)
		put_iface_idx(meter_tx[tx_id].ifname, 1);
	return PPA_SUCCESS;
}

/* ppa_poll_meter()
 * timer handler funtions to frequently poll and
 * update the meter couters
 */
static void ppa_poll_meter(unsigned long dummy)
{
	uint8_t rx_id, tx_id;
	unsigned long long totalCnt;

	GSW_RMON_Meter_cnt_t meterCnt;

	ppa_pae_meter_lock();
	for_each_set_bit(rx_id, &rxid_bitmap, PPA_MAX_IFS_NUM) {
		totalCnt = 0;
		for_each_set_bit(tx_id, &txid_bitmap, PPA_MAX_IFS_NUM) {
			memset(&meterCnt, 0x00, sizeof(GSW_RMON_Meter_cnt_t));
			meterCnt.nMeterId = meterTable[rx_id][tx_id].meter_id;
			if (ltq_try_gswapi_kioctl(GSW_RMON_METER_GET,
						(unsigned int)&meterCnt) == GSW_statusOk) {
				totalCnt += (meterCnt.nGreenCount
						+ meterCnt.nYellowCount
						+ meterCnt.nRedCount);
			}
		}
		rx_meter[rx_id].cnt = totalCnt;
	}
	rx_meter[0].cnt = 0;
	for_each_set_bit(tx_id, &txid_bitmap, PPA_MAX_IFS_NUM) {
		totalCnt = 0;
		for_each_set_bit(rx_id, &rxid_bitmap, PPA_MAX_IFS_NUM) {
			memset(&meterCnt, 0x00, sizeof(GSW_RMON_Meter_cnt_t));
			meterCnt.nMeterId = meterTable[rx_id][tx_id].meter_id;
			if (ltq_try_gswapi_kioctl(GSW_RMON_METER_GET,
						(unsigned int)&meterCnt) == GSW_statusOk) {
				totalCnt += (meterCnt.nGreenCount
						+ meterCnt.nYellowCount
						+ meterCnt.nRedCount);
			}
		}
		tx_meter[tx_id].cnt = totalCnt;
	}
	ppa_pae_meter_unlock();
	tx_meter[0].cnt = 0;
	ppa_timer_add(&g_hit_meter_timer, g_polling_time);
	return;
}
#endif

/*!
	\fn int32_t add_routing_entry(PPA_ROUTING_INFO *route_info) 
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief add one routing entry
 */
int32_t add_routing_entry(PPA_ROUTING_INFO *route)
{
	GSW_ROUTE_Entry_t rt_entry ={0};
	int32_t ret=0;
	int8_t strbuf1[24]={0},strbuf2[24]={0};
	
	struct uc_session_node *p_item = (struct uc_session_node *)route->p_item;
	if(!p_item) return PPA_FAILURE;

	ppa_memset(&rt_entry,0,sizeof(GSW_ROUTE_Entry_t));

	nsess_add++;
	/* Session Hash oroginally calculated by PAE on ingress*/
	rt_entry.nHashVal = -1;
	/* kamal TBD: need to check whether there is any additional criteria to set the rt_entry.bPrio */
	 
	rt_entry.nRtIndex = -1;
	rt_entry.routeEntry.pattern.bValid = 1;
	/* fill the route pattern*/
	if(p_item->flags & SESSION_IS_IPV6) {
		rt_entry.routeEntry.pattern.eIpType = GSW_RT_IP_V6;
	} else {
		rt_entry.routeEntry.pattern.eIpType = GSW_RT_IP_V4;
	}
	
	
	if(p_item->flags & SESSION_IS_TCP) {
		rt_entry.routeEntry.pattern.nRoutExtId = get_routing_extension_id(IP_PROTO_TCP); 
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	} else if(( route->tnnl_info.tunnel_type == TUNNEL_TYPE_IPSEC) && !(is_lansession(p_item->flags))) { /*for upstream route ext id should be tcp/udp*/
		rt_entry.routeEntry.pattern.nRoutExtId = get_routing_extension_id(IP_PROTO_ESP);
#endif
	} else {
		rt_entry.routeEntry.pattern.nRoutExtId = get_routing_extension_id(IP_PROTO_UDP);
	}

	if(p_item->flags & SESSION_IS_IPV6) {
		ppa_memcpy(rt_entry.routeEntry.pattern.nSrcIP.nIPv6,p_item->pkt.src_ip.ip6,sizeof(uint16_t)*8);
		ppa_memcpy(rt_entry.routeEntry.pattern.nDstIP.nIPv6,p_item->pkt.dst_ip.ip6,sizeof(uint16_t)*8);
	} else {
		rt_entry.routeEntry.pattern.nSrcIP.nIPv4 = p_item->pkt.src_ip.ip;
		rt_entry.routeEntry.pattern.nDstIP.nIPv4 = p_item->pkt.dst_ip.ip;
	}
	
	rt_entry.routeEntry.pattern.nSrcPort = p_item->pkt.src_port;
	rt_entry.routeEntry.pattern.nDstPort = p_item->pkt.dst_port;
	
	/* fill the route action*/
	if( ( (is_lansession(p_item->flags)) && (( route->tnnl_info.tunnel_type == TUNNEL_TYPE_L2TP)
						|| ( route->tnnl_info.tunnel_type == TUNNEL_TYPE_IPOGRE)
						|| ( route->tnnl_info.tunnel_type == TUNNEL_TYPE_IP6OGRE)) )
#if IS_ENABLED(CONFIG_PPA_MPE_IP97) 
		|| ( route->tnnl_info.tunnel_type == TUNNEL_TYPE_IPSEC )
#endif
		 ) {

		rt_entry.routeEntry.action.nDstPortMap = 1;
	} else if ( ppa_is_EgressGreSession(p_item) && ((route->tnnl_info.tunnel_type == TUNNEL_TYPE_EOGRE) ||
							(route->tnnl_info.tunnel_type == TUNNEL_TYPE_6EOGRE)) ) {
		rt_entry.routeEntry.action.nDstPortMap = 1;
	} else {
		rt_entry.routeEntry.action.nDstPortMap = 1 << p_item->dest_ifid;
	}


	/* route->dest_subifid will have vap/vlan table index/ATM VCC 4bits 
	// nDstSubIfId = 000 | dest_subifid (4bits) | sta id (8bits)	
	// VLAN handling
	// vlans will be converted into subifids and will be passed to this function in the route->dest_subifid as shown below 
	//	route.dest_subifid read from dp driver has it in the correct format*/
	rt_entry.routeEntry.action.nDstSubIfId = p_item->dest_subifid;

	if(p_item->flags & (SESSION_VALID_NAT_PORT | SESSION_VALID_NAT_IP)) {
		if(p_item->flags & SESSION_IS_IPV6) {
			rt_entry.routeEntry.action.eIpType = GSW_RT_IP_V6;
			ppa_memcpy(rt_entry.routeEntry.action.nNATIPaddr.nIPv6,p_item->pkt.nat_ip.ip6, sizeof(uint16_t)*8);
		} else {
			rt_entry.routeEntry.action.eIpType = GSW_RT_IP_V4;
			rt_entry.routeEntry.action.nNATIPaddr.nIPv4 = p_item->pkt.nat_ip.ip;
		}	

		if(p_item->pkt.nat_port){
			rt_entry.routeEntry.action.nTcpUdpPort = p_item->pkt.nat_port;
		}
	}

	rt_entry.routeEntry.action.nMTUvalue = (p_item->mtu > 0 ? p_item->mtu : 1500 );

#if IS_ENABLED(CONFIG_PPA_MPE_IP97) 
	if(( route->tnnl_info.tunnel_type == TUNNEL_TYPE_IPSEC) && !(is_lansession(p_item->flags))) /* for DS direction, no need to modify src mac */
		rt_entry.routeEntry.action.bMAC_SrcEnable=0;
	else
#endif
	if (p_item->flag2 & SESSION_FLAG2_CPU_IN)
		rt_entry.routeEntry.action.bMAC_SrcEnable=0;
	else
		rt_entry.routeEntry.action.bMAC_SrcEnable=1;
	if( route->tnnl_info.tunnel_type == TUNNEL_TYPE_EOGRE	||
		route->tnnl_info.tunnel_type == TUNNEL_TYPE_6EOGRE ||
		p_item->flag2 & SESSION_FLAG2_BRIDGED_SESSION) {
								
		if(p_item->flag2 & SESSION_FLAG2_VALID_L2_SNAT)
			ppa_memcpy(rt_entry.routeEntry.action.nSrcMAC,p_item->pkt.nat_src_mac, sizeof(uint8_t)*PPA_ETH_ALEN);
		else
			ppa_memcpy(rt_entry.routeEntry.action.nSrcMAC,p_item->pkt.src_mac, sizeof(uint8_t)*PPA_ETH_ALEN);
	} else {
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
		if ( (!(route->tnnl_info.tunnel_type == TUNNEL_TYPE_IPSEC) &&
		     !(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_INBOUND)) ) {
			if(!(p_item->flag2 & SESSION_FLAG2_VALID_IPSEC_OUTBOUND_LAN))
#endif
				ppa_get_netif_hwaddr(p_item->tx_if, rt_entry.routeEntry.action.nSrcMAC, 1);
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
		}
#endif
	}

#if IS_ENABLED(CONFIG_PPA_MPE_IP97) 
	if(( route->tnnl_info.tunnel_type == TUNNEL_TYPE_IPSEC) && !(is_lansession(p_item->flags))) /* for DS direction, no need to modify dst mac*/
		rt_entry.routeEntry.action.bMAC_DstEnable=0;
	else
#endif
	if (p_item->flag2 & SESSION_FLAG2_CPU_IN)
		rt_entry.routeEntry.action.bMAC_DstEnable=0;
	else
		rt_entry.routeEntry.action.bMAC_DstEnable=1;
	ppa_memcpy(rt_entry.routeEntry.action.nDstMAC,p_item->pkt.dst_mac, sizeof(uint8_t)*PPA_ETH_ALEN); 

	rt_entry.routeEntry.action.bPPPoEmode = route->pppoe_mode; 
	if(rt_entry.routeEntry.action.bPPPoEmode) {
		rt_entry.routeEntry.action.bTunnel_Enable=1;
		rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_NULL;
		rt_entry.routeEntry.action.nPPPoESessId = p_item->pkt.pppoe_session_id;
	}

	rt_entry.routeEntry.action.bTTLDecrement = 1; // ENABLED

	/*Tunneling parameters*/
	if( route->tnnl_info.tunnel_type != TUNNEL_TYPE_NULL) {
		rt_entry.routeEntry.action.bTunnel_Enable=1;
		switch (route->tnnl_info.tunnel_type) {
#if IS_ENABLED(CONFIG_PPA_MPE_IP97) 
		case TUNNEL_TYPE_IPSEC:
			rt_entry.routeEntry.action.nTunnelIndex = route->tnnl_info.tunnel_idx;
			rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_IPSEC;
			break;
#endif
		case TUNNEL_TYPE_6RD:
			if(is_lansession(p_item->flags)) { 
				rt_entry.routeEntry.action.eIpType = GSW_RT_IP_V4;
				rt_entry.routeEntry.action.nTunnelIndex = route->tnnl_info.tunnel_idx;
				/* for 6rd destination ip address is taken from this field*/
				rt_entry.routeEntry.action.nNATIPaddr.nIPv4	= p_item->pkt.sixrd_daddr; 
			} else {
				rt_entry.routeEntry.action.eIpType = GSW_RT_IP_V6;
			}		
			rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_6RD;
			break;
		case TUNNEL_TYPE_DSLITE:
			if(is_lansession(p_item->flags)) { 
				rt_entry.routeEntry.action.eIpType = GSW_RT_IP_V6;
				rt_entry.routeEntry.action.nTunnelIndex = route->tnnl_info.tunnel_idx;
			} else {
				rt_entry.routeEntry.action.eIpType = GSW_RT_IP_V4;
			}
			rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_DSLITE;
			break;
#if defined(L2TP_CONFIG) && L2TP_CONFIG
		case TUNNEL_TYPE_L2TP:
			if(!(is_lansession(p_item->flags))) { 
			rt_entry.routeEntry.action.bTunnel_Enable=1;
				rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_L2TP; 
			}
			else {
				rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_NULL;

				/* for US direction, no need to modify src or dst mac */
				rt_entry.routeEntry.action.bMAC_SrcEnable = 0;
				rt_entry.routeEntry.action.bMAC_DstEnable = 0;
			}
			break;
#endif
		case TUNNEL_TYPE_EOGRE:
		case TUNNEL_TYPE_6EOGRE:
			/* for EoGRE, no TTL decrements */
			rt_entry.routeEntry.action.bTTLDecrement = 0;

		case TUNNEL_TYPE_IPOGRE:
		case TUNNEL_TYPE_IP6OGRE:
			if ( ppa_is_EgressGreSession(p_item) ) {
				rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_NULL;
				rt_entry.routeEntry.action.bTunnel_Enable = 0;

				/* for US direction, no need to modify src or dst mac */
				rt_entry.routeEntry.action.bMAC_SrcEnable = 0;
				rt_entry.routeEntry.action.bMAC_DstEnable = 0;

				/* for US direction, MPE decrements TTL as complementary action */
				rt_entry.routeEntry.action.bTTLDecrement = 0;
			} else {
				if(p_item->flags & SESSION_IS_IPV6)
					rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_6RD;
				else
					rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_DSLITE;
			}
			break;

		default:
			break;
		}
	}
#ifdef PAE_HW_METER
	ppa_pae_meter_lock();
	get_session_nMeterId(p_item, &rt_entry);
	ppa_pae_meter_unlock();
#endif
 
	/* FLOW id based handling*/
	if (p_item->flag2 & SESSION_FLAG2_LRO)
		rt_entry.routeEntry.action.nFlowId = 0x80 + (p_item->lro_sessid - 1);
	else
		rt_entry.routeEntry.action.nFlowId = route->nFlowId;

	/* DSCP handling*/
	if(p_item->flags & SESSION_VALID_NEW_DSCP) {
	rt_entry.routeEntry.action.bInnerDSCPRemark = 1;
		rt_entry.routeEntry.action.nDSCP = p_item->pkt.new_dscp;
		rt_entry.routeEntry.action.eOutDSCPAction = 2; 
	}

	/* Traffic class remarking*/
	dbg("\n f_tc_remark_enable = %d , tc_remark = %d \n",route->f_tc_remark_enable,route->tc_remark);
	rt_entry.routeEntry.action.bTCremarking = route->f_tc_remark_enable;
	rt_entry.routeEntry.action.nTrafficClass = route->tc_remark;

	/* eSessDirection=0 for GRE DS PAE full processing; Otherwise eSessDirection=<Session Dir> */
	rt_entry.routeEntry.action.eSessDirection = (ppa_is_IngressGreSession(p_item) ? 0 : is_lansession(p_item->flags));

	rt_entry.routeEntry.action.eSessRoutingMode = (p_item->flags & SESSION_VALID_NAT_IP) ? 
			((p_item->flags & SESSION_VALID_NAT_PORT) ?	PPA_ROUTE_TYPE_NAPT : PPA_ROUTE_TYPE_NAT) 
			: PPA_ROUTE_TYPE_IPV4;

#if IS_ENABLED(CONFIG_PPA_MPE_IP97) 
	if(( route->tnnl_info.tunnel_type == TUNNEL_TYPE_IPSEC) && (is_lansession(p_item->flags))) 
		rt_entry.routeEntry.action.eSessRoutingMode = 0x0;
#endif


	dbg("\nAdd_routing_entry\n src_mac[6]=%s\n dst_mac[6]=%s\n sip=%u\n dip=%u\n sp=%u\n dp=%u\n routing extension id=%d\n portmap=%0x\n subifid=%0x\n direction=%d\n pppoe_mode=%d\n pppoe_session_id=%d tunnel_type=%d tunnel_index=%d\n",
	/*printk(KERN_INFO "\nAdd_routing_entry\n src_mac[6]=%s\n dst_mac[6]=%s\n sip=%u\n dip=%u\n \
		sp=%u\n dp=%u\n routing extension id=%d\n portmap=%0x\n subifid=%0x\n direction=%d\n \
		pppoe_mode=%d\n pppoe_session_id=%d\n",*/
 		ppa_get_pkt_mac_string(rt_entry.routeEntry.action.nSrcMAC, strbuf1),
 		ppa_get_pkt_mac_string(rt_entry.routeEntry.action.nDstMAC, strbuf2),
 		rt_entry.routeEntry.pattern.nSrcIP.nIPv4, 
 		rt_entry.routeEntry.pattern.nDstIP.nIPv4, 
		rt_entry.routeEntry.pattern.nSrcPort, 
		rt_entry.routeEntry.pattern.nDstPort, 
		rt_entry.routeEntry.pattern.nRoutExtId, 
		rt_entry.routeEntry.action.nDstPortMap, 
		rt_entry.routeEntry.action.nDstSubIfId,
		rt_entry.routeEntry.action.eSessDirection,
		rt_entry.routeEntry.action.bPPPoEmode,
		rt_entry.routeEntry.action.nPPPoESessId,
		rt_entry.routeEntry.action.eTunType,
		rt_entry.routeEntry.action.nTunnelIndex);
 
	GSWR_OPS(pae_ops, ROUTE_SessionEntryAdd, &rt_entry, ret);
	if (ret < GSW_statusOk) {
		dbg("add_routing_entry returned failure\n");
		switch(ret) {
		case (int)GSW_statusLock_Failed:
			route->update_flags |= FLAG_SESSION_LOCK_FAIL;	
			nsess_add_fail_lock++;
			break;
		case GSW_ROUTE_ERROR_MTU_FULL:
			nsess_add_fail_mtu_full++;
			break;
		case GSW_ROUTE_ERROR_PPPOE_FULL:
			nsess_add_fail_pppoe_full++;
			break;
		case GSW_ROUTE_ERROR_RTP_FULL:
			nsess_add_fail_rtp_full++;
			break;
		case GSW_ROUTE_ERROR_IP_FULL:
			nsess_add_fail_ip_tbl_full++;
			break;
		case GSW_ROUTE_ERROR_MAC_FULL:
			nsess_add_fail_mac_tbl_full++;
			break;
		case GSW_ROUTE_ERROR_RT_SESS_FULL:
			nsess_add_fail_rt_tbl_full++;
			break;
		case GSW_ROUTE_ERROR_RT_COLL_FULL:
			nsess_add_fail_coll_full++;
			break;
		case PPA_FAILURE:
			nsess_add_fail_minus1++;
		default:
			nsess_add_fail_oth++;
			break;
		}	
#ifdef PAE_HW_METER
		ppa_pae_meter_lock();
		put_session_nMeterId(p_item);
		ppa_pae_meter_unlock();
#endif
		return PPA_FAILURE;
	} else {
		if(rt_entry.nRtIndex >=0 ) {	
			route->entry = rt_entry.nRtIndex;
			nsess_add_succ++;
		} else {
#ifdef PAE_HW_METER
			ppa_pae_meter_lock();
			put_session_nMeterId(p_item);
			ppa_pae_meter_unlock();
#endif
			nsess_add_fail_negindex++;
			return PPA_FAILURE;
		}
	
		if( rt_entry.nFlags & 0x01) /* swap done */
			route->update_flags |= FLAG_SESSION_SWAPPED;	 
	}

	p_item->flags |= SESSION_ADDED_IN_HW;
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(add_routing_entry);

int32_t del_routing_entry(PPA_ROUTING_INFO *route)
{
	GSW_ROUTE_Entry_t	 rt_entry={0};
	int32_t ret=0;

	struct uc_session_node *p_item = (struct uc_session_node *)route->p_item;
	if(!p_item) return PPA_FAILURE;
	ppa_memset(&rt_entry,0,sizeof(GSW_ROUTE_Entry_t));

	/*Session Hash oroginally calculated by PAE on ingress*/
	rt_entry.nRtIndex = route->entry;	 
 
	// fill the route pattern 
	if(p_item->flags & SESSION_IS_IPV6) { 
		rt_entry.routeEntry.action.eIpType = GSW_RT_IP_V6;
		ppa_memcpy(rt_entry.routeEntry.pattern.nSrcIP.nIPv6,p_item->pkt.src_ip.ip6,sizeof(uint16_t)*8);
		ppa_memcpy(rt_entry.routeEntry.pattern.nDstIP.nIPv6,p_item->pkt.dst_ip.ip6,sizeof(uint16_t)*8);
	} else {
		rt_entry.routeEntry.action.eIpType = GSW_RT_IP_V4;
		rt_entry.routeEntry.pattern.nSrcIP.nIPv4 = p_item->pkt.src_ip.ip;
		rt_entry.routeEntry.pattern.nDstIP.nIPv4 = p_item->pkt.dst_ip.ip;
	}
	
	if(p_item->flags & SESSION_IS_TCP) { 
		rt_entry.routeEntry.pattern.nRoutExtId = get_routing_extension_id(IP_PROTO_TCP); 
	} else {
		rt_entry.routeEntry.pattern.nRoutExtId = get_routing_extension_id(IP_PROTO_UDP);
	}
	
	rt_entry.routeEntry.pattern.nSrcPort = p_item->pkt.src_port;
	rt_entry.routeEntry.pattern.nDstPort = p_item->pkt.dst_port;
 
	GSWR_OPS(pae_ops, ROUTE_SessionEntryDel, &rt_entry, ret);
	if (ret < GSW_statusOk) {
		if( ret == GSW_statusLock_Failed) {
			dbg("del_routing_entry returned failure\n");	
			route->update_flags |= FLAG_SESSION_LOCK_FAIL;
			nsess_add_fail_lock++;
		}
		nsess_del_fail++;	
	} else {
		nsess_del++;
#ifdef PAE_HW_METER
		ppa_pae_meter_lock();
		put_session_nMeterId(p_item);
		ppa_pae_meter_unlock();
#endif
	}
	return ret;	
}

int32_t update_routing_entry( PPA_ROUTING_INFO *route)
{
	dbg("update routing entry is not supported in PAE!!!\n");
	return PPA_SUCCESS;
}

/*!
	\fn int32_t add_wan_mc_entry(PPA_MC_INFO *mc_route)
	\param mc_route				 pointer to multicast route info
	\return 0: OK, otherwise: fail
 */
int32_t add_wan_mc_entry(PPA_MC_INFO *mc_route)
{
	GSW_ROUTE_Entry_t	 rt_entry={0};
	int32_t ret=0;
	int8_t strbuf1[24]={0},strbuf2[24]= {0};
	uint8_t mc_dst_mac[PPA_ETH_ALEN] = {0};

	struct mc_session_node *p_item = (struct mc_session_node *)mc_route->p_item;
	ppa_memset(&rt_entry,0,sizeof(GSW_ROUTE_Entry_t));

	nsess_add++;
	/* Session Hash originally calculated by PAE on ingress*/
	rt_entry.nHashVal = -1;

	rt_entry.routeEntry.pattern.bValid = 1;
	if( mc_route->update_flags & FLAG_SESSION_HI_PRIO)
		rt_entry.bPrio	= 1;
	/*Fill the route pattern */
	if(p_item->grp.ip_mc_group.f_ipv6) {
		rt_entry.routeEntry.pattern.eIpType =	GSW_RT_IP_V6;
		rt_entry.routeEntry.action.eIpType =	GSW_RT_IP_V6;
		ppa_memcpy(rt_entry.routeEntry.pattern.nSrcIP.nIPv6,p_item->grp.source_ip.ip.ip6,sizeof(uint16_t)*8);
		ppa_memcpy(rt_entry.routeEntry.pattern.nDstIP.nIPv6,p_item->grp.ip_mc_group.ip.ip6,sizeof(uint16_t)*8);
		/* Derive multicast MAC from the multicast IP */
		ipv6_eth_mc_map((struct in6_addr *)p_item->grp.ip_mc_group.ip.ip6, mc_dst_mac);
	} else {
		rt_entry.routeEntry.pattern.eIpType =	GSW_RT_IP_V4;	
		rt_entry.routeEntry.action.eIpType =	GSW_RT_IP_V4;
		if (is_ip_zero(&p_item->grp.source_ip)) {
			rt_entry.routeEntry.pattern.nSrcIP.nIPv4 = PSEUDO_MC_ANY_SRC;
		} else {
			rt_entry.routeEntry.pattern.nSrcIP.nIPv4 = p_item->grp.source_ip.ip.ip;
		}
		rt_entry.routeEntry.pattern.nDstIP.nIPv4 = p_item->grp.ip_mc_group.ip.ip;
		/* Derive multicast MAC from the multicast IP */
		ip_eth_mc_map(p_item->grp.ip_mc_group.ip.ip, mc_dst_mac);
	}
	/* Multicast data is always UDP*/
	rt_entry.routeEntry.pattern.nRoutExtId = get_routing_extension_id(IP_PROTO_UDP);
	
	/*Fill the route action
	// TBD this inforamation needs to be filled by the HAL selector before callin the HAL*/
	rt_entry.routeEntry.action.nDstPortMap = mc_route->dest_list;

	/*	destination subifid for multicast is 
	//	bits 12:0
	//	12 multicast flag = 1
	//	11:8	VAP ID 0-15 or sub interface id in case of lan ports which is used for egress vlan handling
	//	7 group flag 0 = single VAP , 1 = more than one VAP
	//	6:0 GroupID*/

	/* multicast flag needed for wave wireless interfaces and for complementary processing. */
	if( mc_route->dest_list == 0 || mc_route->dest_list > 0x7F ) {	/* detPort=00 in case of MPE and > 6 */
		rt_entry.routeEntry.action.nDstSubIfId |= 0x1000; /*12 multicast flag = 1 13:12 reserved*/
	}

	rt_entry.routeEntry.action.nDstSubIfId |= p_item->dest_subifid; /*11:8	VAP ID 0-15*/
	rt_entry.routeEntry.action.nDstSubIfId |= (p_item->num_vap > 1 ? 1 : 0) << 7; /* 7 group flag 0 = single VAP , 1 = more than one VAP*/
	rt_entry.routeEntry.action.nDstSubIfId |= (p_item->grp.group_id & 0x7F); /*6:0 GroupID */

	/* Only if no MPE complementary multicast processing */
	if (p_item->num_vap <= 1) {
		if (p_item->flags & SESSION_VALID_SRC_MAC) {
			rt_entry.routeEntry.action.bMAC_SrcEnable = 1;
			ppa_memcpy(rt_entry.routeEntry.action.nSrcMAC, p_item->src_mac, sizeof(uint8_t)*PPA_ETH_ALEN);
		}

		rt_entry.routeEntry.action.bMAC_DstEnable=1;
		ppa_memcpy(rt_entry.routeEntry.action.nDstMAC, mc_dst_mac, sizeof(uint8_t)*PPA_ETH_ALEN); 

		/*Assumption is that if the pppoe mode is termination and the session is DS pppoe header will be removed by PAE*/
		rt_entry.routeEntry.action.bPPPoEmode = (p_item->flags & SESSION_VALID_PPPOE) ? 1 : 0;

		if (rt_entry.routeEntry.action.bPPPoEmode) {
			rt_entry.routeEntry.action.bTunnel_Enable=1;
			rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_NULL;
		}
	}

	/* Metering parameters
	// TBD: need to add support
	//rt_entry.routeEntry.action.bMeterAssign = 0;
	//rt_entry.routeEntry.action.nMeterId = 0;*/

	/* RTP paramters
	//rt_entry.routeEntry.action.bRTPMeasEna = p_item->sample_en;*/
	rt_entry.routeEntry.action.bRTPMeasEna = 1; /*always enable*/
	rt_entry.routeEntry.action.nRTPSeqNumber = p_item->grp.group_id; /*PAE RTP counter table index*/

	/* TUNNEL handling
	// tunnel information needs to be passed down from the hal selector*/
	if( mc_route->tnnl_info.tunnel_type != TUNNEL_TYPE_NULL) {

		rt_entry.routeEntry.action.bTunnel_Enable=1;
		switch (mc_route->tnnl_info.tunnel_type) {

		case TUNNEL_TYPE_6RD:
			rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_6RD;
			break;
		case TUNNEL_TYPE_DSLITE:
			rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_DSLITE;
			break;
#if defined(L2TP_CONFIG) && L2TP_CONFIG
		case TUNNEL_TYPE_L2TP:
			rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_L2TP; 
			break;
#endif
		case TUNNEL_TYPE_EOGRE:
		case TUNNEL_TYPE_IPOGRE:
		case TUNNEL_TYPE_IP6OGRE:
		case TUNNEL_TYPE_6EOGRE:
			if(p_item->grp.ip_mc_group.f_ipv6)  {
				rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_6RD;
			} else {
				rt_entry.routeEntry.action.eTunType = GSW_ROUTE_TUNL_DSLITE;
			}
			break;
		default:
			break;
		}
	}
	
	rt_entry.routeEntry.action.eSessDirection = 0; /*only mc downstream traffic is supported by PPA*/
	rt_entry.routeEntry.action.eSessRoutingMode = 1; /*no NAT for multicast GSW_ROUTE_MODE_ROUTING */
	rt_entry.routeEntry.action.bTTLDecrement = 1; /*enabled */
	rt_entry.routeEntry.action.nMTUvalue = (mc_route->mtu > 0 ? mc_route->mtu : 1500 );
	
	/*printk(KERN_INFO "\nAdd_routing_entry\n src_mac[6]=%s\n dst_mac[6]=%s\n sip=%u\n dip=%u\n sp=%u\n dp=%u\n \
		routing extension id=%d\n portmap=%0x\n subifid=%0x\n direction=%d\n pppoe_mode=%d\n pppoe_session_id=%d\n",*/
	 dbg("\nAdd_routing_entry\n src_mac[6]=%s\n dst_mac[6]=%s\n sip=%u\n dip=%u\n sp=%u\n dp=%u\n routing \
		extension id=%d\n portmap=%0x\n subifid=%0x\n direction=%d\n pppoe_mode=%d\n pppoe_session_id=%d\n",
 		ppa_get_pkt_mac_string(rt_entry.routeEntry.action.nSrcMAC, strbuf1),
		ppa_get_pkt_mac_string(rt_entry.routeEntry.action.nDstMAC, strbuf2),
		rt_entry.routeEntry.pattern.nSrcIP.nIPv4, 
		rt_entry.routeEntry.pattern.nDstIP.nIPv4, 
		rt_entry.routeEntry.pattern.nSrcPort, 
		rt_entry.routeEntry.pattern.nDstPort, 
	 	rt_entry.routeEntry.pattern.nRoutExtId, 
 		rt_entry.routeEntry.action.nDstPortMap, 
		rt_entry.routeEntry.action.nDstSubIfId,
		rt_entry.routeEntry.action.eSessDirection,
		rt_entry.routeEntry.action.bPPPoEmode,
		rt_entry.routeEntry.action.nPPPoESessId);

	GSWR_OPS(pae_ops, ROUTE_SessionEntryAdd, &rt_entry, ret);
	if (ret < GSW_statusOk) {
		if( ret == GSW_statusLock_Failed) {
			dbg("add_mc_routing_entry returned failure\n");	
			mc_route->update_flags |= FLAG_SESSION_LOCK_FAIL;	
			nsess_add_fail_lock++;
		} else
			nsess_add_fail_oth++;
		return ret;
	}
	
	mc_route->p_entry = rt_entry.nRtIndex; 	
	if( rt_entry.nFlags & 0x01) /* swap done */
	mc_route->update_flags |= FLAG_SESSION_SWAPPED;	 
	nsess_add_succ++;
	 
	return PPA_SUCCESS;
}
EXPORT_SYMBOL(add_wan_mc_entry);
/*!
	\fn void del_wan_mc_entry(uint32_t entry)
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief delete one multicast routing entry
	\param entry	entry number got from function call "add_wan_mc_entry"
	\return no return value
 */
int32_t del_wan_mc_entry(PPA_MC_INFO *mc_route)
{
	GSW_ROUTE_Entry_t	 rt_entry={0};
	int32_t ret=0;

	struct mc_session_node *p_item = (struct mc_session_node *)mc_route->p_item;
	ppa_memset(&rt_entry,0,sizeof(GSW_ROUTE_Entry_t));

	/* Session Hash oroginally calculated by PAE on ingress*/
	rt_entry.nRtIndex = mc_route->p_entry;	 
 
	/* fill the route pattern */
	if(p_item->grp.ip_mc_group.f_ipv6) {
		rt_entry.routeEntry.pattern.eIpType =	GSW_RT_IP_V6;
		ppa_memcpy(rt_entry.routeEntry.pattern.nSrcIP.nIPv6,p_item->grp.source_ip.ip.ip6,sizeof(uint16_t)*8);
		ppa_memcpy(rt_entry.routeEntry.pattern.nDstIP.nIPv6,p_item->grp.ip_mc_group.ip.ip6,sizeof(uint16_t)*8);
	} else {
		rt_entry.routeEntry.pattern.eIpType =	GSW_RT_IP_V4;	
		if (is_ip_zero(&p_item->grp.source_ip)) {
			rt_entry.routeEntry.pattern.nSrcIP.nIPv4 = PSEUDO_MC_ANY_SRC;
		} else {
			rt_entry.routeEntry.pattern.nSrcIP.nIPv4 = p_item->grp.source_ip.ip.ip;
		}
		rt_entry.routeEntry.pattern.nDstIP.nIPv4 = p_item->grp.ip_mc_group.ip.ip;
	}

	/*TBD: get_routing_extension_id() needs to take appropriate input parameters differentiate the flow rules configured*/
	rt_entry.routeEntry.pattern.nRoutExtId = get_routing_extension_id(IP_PROTO_UDP); //always UDP 

	rt_entry.routeEntry.action.bRTPMeasEna = 1; /*always enabled*/
	rt_entry.routeEntry.action.nRTPSeqNumber = p_item->grp.group_id; /*PAE RTP counter table index*/

	GSWR_OPS(pae_ops, ROUTE_SessionEntryDel, &rt_entry, ret);
	if (ret < GSW_statusOk) {
		if( ret == GSW_statusLock_Failed) {
			mc_route->update_flags |= FLAG_SESSION_LOCK_FAIL;	
		}
		dbg("del_wan_mc_entry returned failure\n");
		nsess_del_fail++;
	} else
		nsess_del++;
	return ret;	
}

/*!
	\fn int32_t update_wan_mc_entry(PPA_MC_INFO mc_route)
*/
int32_t update_wan_mc_entry(PPA_MC_INFO *mc_route)
{
	/*PAE supports very limited modify operation
	in case of multicast sessions; it allows modification of 
	1. dest_port_map
	2. dest sub if_id ??
	*/

	GSW_ROUTE_Session_Dest_t mod_entry={0};
	int32_t ret=0;

	struct mc_session_node *p_item = (struct mc_session_node *)mc_route->p_item;
	/* Session Hash originally calculated by PAE on ingress*/
	mod_entry.nRtIdx = mc_route->p_entry;	 
	
	/* TBD this inforamation needs to be filled by the HAL selector before callin the HAL*/
	mod_entry.nDstPortMap = mc_route->dest_list;

	/*	destination subifid for multicast is 
	//	bits 14:0
	//	14 multicast flag = 1
	//	13:12 reserved
	//	11:8	VAP ID 0-15 or destination sub interface oid which denotes vlan s
	//	7 group flag 0 = single VAP , 1 = more than one VAP
	//	6:0 GroupID*/
	mod_entry.nDstSubIfId |= 0x1000; /*14 multicast flag = 1 13:12 reserved*/
	mod_entry.nDstSubIfId |= p_item->dest_subifid; /*11:8	VAP ID 0-15*/
	mod_entry.nDstSubIfId |= (p_item->num_vap > 1 ? 1 : 0) << 7; /*	7 group flag 0 = single VAP , 1 = more than one VAP*/
	mod_entry.nDstSubIfId |= (p_item->grp.group_id & 0x7F); /*	6:0 GroupID */

	if((ret=ltq_try_gswapi_kioctl( GSW_ROUTE_SESSION_DEST_MOD, (unsigned int)&mod_entry)) < GSW_statusOk) {
		if( ret == GSW_statusLock_Failed) {
			mc_route->update_flags |= FLAG_SESSION_LOCK_FAIL;	
		}
		dbg("modify multicast routing failed\n");	
	}
	
	return ret;
}
EXPORT_SYMBOL(update_wan_mc_entry);

int32_t add_bridging_entry(uint32_t port, uint8_t mac[PPA_ETH_ALEN],
			uint32_t fid, int32_t age_timer,
			uint16_t static_entry, uint16_t sub_ifid)
{
	GSW_MAC_tableAdd_t mac_entry={0};
	int8_t strbuf1[24]={0};
	int ret= 0;	 
 
	ppa_memset(&mac_entry, 0, sizeof(mac_entry));

	mac_entry.nFId = fid;
	mac_entry.nPortId = port;
	ppa_memcpy(mac_entry.nMAC, mac, sizeof(uint8_t)*PPA_ETH_ALEN);
	mac_entry.nAgeTimer = 15; /*1111 binary (4bits) x global multiplier 20 (default) = 300*/
	if(static_entry == SESSION_STATIC) {
		mac_entry.bStaticEntry = 1;
	}
	/*sub interface id read from datapath driver will already be in 11:8. */
	mac_entry.nSubIfId = sub_ifid;
	
	if((ret=ltq_try_gswapi_kioctl( GSW_MAC_TABLE_ENTRY_ADD, (unsigned int)&mac_entry)) < GSW_statusOk) {
		dbg("add_bridging_entry returned failure\n");	
	}	

	dbg("\nAdd_bridging_entry\n src_mac[6]=%s\n port=%0x\n subifid=%0x fid=%x\n static=%d\n",
	ppa_get_pkt_mac_string(mac_entry.nMAC, strbuf1), mac_entry.nPortId, mac_entry.nSubIfId, mac_entry.nFId, mac_entry.bStaticEntry);
	return ret;
}
EXPORT_SYMBOL(add_bridging_entry);

int32_t del_bridging_entry(uint8_t mac[PPA_ETH_ALEN], uint32_t fid)
{
	GSW_MAC_tableRemove_t mac_entry = {0};
	int ret = 0;

	ppa_memset(&mac_entry, 0, sizeof(mac_entry));

	mac_entry.nFId = fid;
	ppa_memcpy(mac_entry.nMAC, mac, sizeof(uint8_t)*PPA_ETH_ALEN);

	dbg("\nDelete_bridging_entry\n src_mac[6] = %pM\n fid = %x\n",
		mac, fid);

	/* When a client moves from IRE to the gateway the client is unable to
	 * communicate as GSWIP-L is programmed in auto learning mode.
	 * So GSWIP-L entry is deleted when entry getting deleted from Linux
	 * bridge
	 */
	ret = gswl_api_kioctl(GSW_MAC_TABLE_ENTRY_REMOVE,
					 (unsigned int)&mac_entry);
	if (ret < GSW_statusOk)
		dbg("PAE_HAL GSW_MAC_TABLE_ENTRY_REMOVE %pM failed for GSWIP-L\n",
			mac);

	ret = ltq_try_gswapi_kioctl(GSW_MAC_TABLE_ENTRY_REMOVE,
					(unsigned int)&mac_entry);
	if (ret < GSW_statusOk)
		dbg("ppa pae HAL GSW_MAC_TABLE_ENTRY_REMOVE failed for GSWIP-R\n");

	return ret;
}
EXPORT_SYMBOL(del_bridging_entry);

uint32_t add_br_port(PPA_BR_PORT_INFO *br_info)
{

	PPA_CLASS_RULE class_info = {0};
	GSW_VLAN_IdCreate_t vlan_info = {0}; 
	GSW_VLAN_portMemberAdd_t vlan_member = {0};
	
	 ppa_memset(&class_info, 0, sizeof(PPA_CLASS_RULE));	 

	class_info.in_dev = GSWR_INGRESS;
	class_info.category = CAT_VLAN;
	class_info.pattern.bEnable=1;
	class_info.pattern.bPortIdEnable=1;
	class_info.pattern.nPortId = br_info->port;

	if(br_info->subif_en) {
		class_info.pattern.bSubIfIdEnable = 1;
		class_info.pattern.nSubIfId = ((br_info->subif) & 0xF00) >> 8;
	}

	if(br_info->vid) {	
		class_info.pattern.bVid = 1;	/* BUG in switchapi this overwrite the port information.*/
		class_info.pattern.nVid = br_info->vid; 	

		/*create vlan in switch*/
		vlan_info.nVId = br_info->vid;
		vlan_info.nFId = br_info->brid;	 
		if((ltq_try_gswapi_kioctl( GSW_VLAN_ID_CREATE, (unsigned int)&vlan_info)) < GSW_statusOk) {
			dbg("ppa add vlan entry exists\n");	
		}
 
		/*make CPU port as member of the vlan with tag egress enabled*/
		vlan_member.nVId = br_info->vid;
		vlan_member.nPortId = 0; /*CPU port*/
		vlan_member.bVLAN_TagEgress = 1; /*egress tagging enabled*/
		if((ltq_try_gswapi_kioctl( GSW_VLAN_PORT_MEMBER_ADD, (unsigned int)&vlan_member)) < GSW_statusOk) {
			dbg("ppa adding vlan membership entry exists\n");	
		}

	}

	class_info.action.vlan_action.cvlan = VLAN_ALTERNATIVE;	
	class_info.action.vlan_action.vlan_id = br_info->vid;	
	class_info.action.vlan_action.fid = br_info->brid;
	
	class_info.action.rmon_action = 1;
	class_info.action.rmon_id = RMON_BRIDGE_CNTR-br_info->brid;
	
	if(pae_hal_add_class_rule(&class_info)!=PPA_SUCCESS) {
		dbg( "add_br_port: add class rule failed\n");
		return PPA_FAILURE;
	}
	br_info->index = class_info.uidx;
	return PPA_SUCCESS;
}

uint32_t del_br_port(PPA_BR_PORT_INFO *br_info)
{
	PPA_CLASS_RULE class_info = {0};	 
	class_info.in_dev = GSWR_INGRESS;
	class_info.category = CAT_VLAN;

	class_info.uidx = br_info->index;
	
	pae_hal_del_class_rule(&class_info);		

	return PPA_SUCCESS;
}

/*!
	\fn del_tunnel_entry(struct net_device *dev,
										uint32_t *tunnel_idx)
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief delete tunnel entry
	\param tunnel_idx			a data pointer to get tunnel id
	\return 0: OK, otherwise: fail
 */
int32_t del_tunnel_entry(uint32_t tunnel_idx)
{
	GSW_ROUTE_Tunnel_Entry_t t_entry={0};
	int32_t ret = PPA_FAILURE;
	 
	ppa_memset(&t_entry,0,sizeof(t_entry));
	
	t_entry.nTunIndex = tunnel_idx;

	if((ret = ltq_try_gswapi_kioctl( GSW_ROUTE_TUNNEL_ENTRY_DELETE, (unsigned int)&t_entry)) < GSW_statusOk) {
		dbg("ppa delete tunnel entry failed\n");	
	}
	return ret;
}

/*!
	\fn add_tunnel_entry(struct net_device *dev, uint32_t *tunnel_idx)
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief add tunnel entry
	\param tunnel_type		 type of tunnel
	\param tunnel_idx			a data pointer to get tunnel id
	\return 0: OK, otherwise: fail
 */
int32_t add_tunnel_entry(uint32_t tunnel_type, uint32_t tunnel_idx)
{
	GSW_ROUTE_Tunnel_Entry_t t_entry={0};
	uint32_t ret = PPA_SUCCESS;
	 
	ppa_memset(&t_entry,0,sizeof(t_entry));
	
	t_entry.nTunIndex = tunnel_idx;

	if(g_tunnel_table[tunnel_idx]) {
		if(tunnel_type == TUNNEL_TYPE_6RD) {
			t_entry.tunnelEntry.eTunnelType = GSW_ROUTE_TUNL_6RD;
			t_entry.tunnelEntry.t.tun6RD.nSrcIP4Addr.nIPv4 = g_tunnel_table[tunnel_idx]->tunnel_info.ip4_hdr.saddr;
			t_entry.tunnelEntry.t.tun6RD.nDstIP4Addr.nIPv4 = g_tunnel_table[tunnel_idx]->tunnel_info.ip4_hdr.daddr;
		} else if (tunnel_type == TUNNEL_TYPE_DSLITE) {
			t_entry.tunnelEntry.eTunnelType = GSW_ROUTE_TUNL_DSLITE;
			ppa_memcpy(t_entry.tunnelEntry.t.tunDSlite.nSrcIP6Addr.nIPv6, g_tunnel_table[tunnel_idx]->tunnel_info.ip6_hdr.saddr, sizeof(uint16_t)*8);
			ppa_memcpy(t_entry.tunnelEntry.t.tunDSlite.nDstIP6Addr.nIPv6, g_tunnel_table[tunnel_idx]->tunnel_info.ip6_hdr.daddr, sizeof(uint16_t)*8);
		}
	} else {
		dbg("ppa add tunnel entry failed at index %d\n", tunnel_idx);
		return PPA_FAILURE;
	}

	if((ret = ltq_try_gswapi_kioctl( GSW_ROUTE_TUNNEL_ENTRY_ADD, (unsigned int)&t_entry) < GSW_statusOk)) {
	dbg("ppa add tunnel entry failed\n");
	}

	return ret;

}
EXPORT_SYMBOL(add_tunnel_entry);

/*
initialize the lro_table
*/
void init_lro_table(void)
{
	ppa_memset(g_lro_table, 0, sizeof(ppa_lro_entry)*MAX_LRO_ENTRIES);
	ppa_lock_init(&g_lro_table_lock);
}
/*!
	\fn int32_t add_lro_entry( )
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief add a LRO HW session
	\param PPA_LRO_INFO * lro_entry
	\return >= 0: lro session id, otherwise: fail
 */
int32_t add_lro_entry(PPA_LRO_INFO *lro_entry)
{
	int32_t i = MAX_LRO_ENTRIES, ret = PPA_SUCCESS;
	struct uc_session_node *p_item = (struct uc_session_node *)lro_entry->p_item;
	PPA_CLASS_RULE rule;
#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
	struct cpumask cpumask;
#endif

	ppa_lock_get(&g_lro_table_lock);

#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
	/* call the lro driver, default timeout value of 200micros */
	cpumask.bits[0] = 0x01;
	ret = lro_start_flow(&i, (g_lro_timeout * 0x126), 0, cpumask);
	if (ret < 0) {
		ppa_lock_release(&g_lro_table_lock);
		dbg("LRO start flow returned failure %d\n", ret);
		return ret;
	}
#endif
	if (unlikely((i < 0) || (i >= MAX_LRO_ENTRIES))) {
		dbg("idx is invalid!\n");
		goto STOP_FLOW;
	}
	/* flow empty entry is i*/
	ppa_memset(&(g_lro_table[i]), 0, sizeof(ppa_lro_entry));

	if ((p_item->flags & SESSION_VALID_VLAN_RM) ||
	    (p_item->flags & SESSION_VALID_PPPOE) ||
	    ppa_is_TunneledSession(p_item)) {
		g_lro_table[i].enabled = 1;
		lro_entry->session_id = i + 1;
	} else {
		/* copy the values*/
		g_lro_table[i].f_ipv6 = lro_entry->f_ipv6;
		ppa_memcpy(&(g_lro_table[i].srcip), &lro_entry->src_ip,
			   sizeof(IP_ADDR));
		ppa_memcpy(&(g_lro_table[i].dstip), &lro_entry->dst_ip,
			   sizeof(IP_ADDR));
		g_lro_table[i].srcport = lro_entry->src_port;
		g_lro_table[i].dstport = lro_entry->dst_port;
		g_lro_table[i].enabled = 1;

		lro_entry->session_id = i+1;

		ppa_memset(&rule, 0, sizeof(PPA_CLASS_RULE));

		rule.in_dev = GSWR_INGRESS;
		rule.category = CAT_LRO;
		rule.pattern.bEnable = 1;

		rule.pattern.bParserFlagMSB_Enable = 1;
		rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_TCP;
		rule.pattern.nParserFlagMSB_Mask =
			(uint16_t)(~(PCE_PARSER_MSB_LRO_EXCEP | PCE_PARSER_MSB_TCP |
				     PCE_PARSER_MSB_INNR_IPV6 | PCE_PARSER_MSB_INNR_IPV4));
		if (g_lro_table[i].f_ipv6) {
			rule.pattern.eDstIP_Select = GSW_PCE_IP_V6;
			ppa_memcpy(rule.pattern.nDstIP.nIPv6,
				   g_lro_table[i].dstip.ip6,
				   sizeof(uint16_t)*8);
			rule.pattern.nDstIP_Mask = 0x0000;
			rule.pattern.eSrcIP_Select = GSW_PCE_IP_V6;
			ppa_memcpy(rule.pattern.nSrcIP.nIPv6,
				   g_lro_table[i].srcip.ip6,
				   (sizeof(uint16_t) * 8));
			rule.pattern.nSrcIP_Mask = 0x0000;
		} else {
			rule.pattern.eDstIP_Select = GSW_PCE_IP_V4;
			rule.pattern.nDstIP.nIPv4 = g_lro_table[i].dstip.ip;
			rule.pattern.nDstIP_Mask = 0xFF00;
			rule.pattern.eSrcIP_Select = GSW_PCE_IP_V4;
			rule.pattern.nSrcIP.nIPv4 = g_lro_table[i].srcip.ip;
			rule.pattern.nSrcIP_Mask = 0xFF00;
		}
		//TCP source port and destination port
		rule.pattern.bAppDataMSB_Enable = 1;
		rule.pattern.nAppDataMSB = g_lro_table[i].srcport;
		rule.pattern.nAppMaskRangeMSB = 0xF0;

		rule.pattern.bAppDataLSB_Enable = 1;
		rule.pattern.nAppDataLSB = g_lro_table[i].dstport;
		rule.pattern.nAppMaskRangeLSB = 0xF0;

		rule.pattern.bEnable = 1;
		rule.pattern.bPktLngEnable = 1;
		rule.pattern.nPktLng = g_min_lrolen;
		rule.pattern.nPktLngRange = 1500;

		rule.action.fwd_action.rtctrlenable = 1;
		rule.action.qos_action.flowid_enabled = 1;
		rule.action.qos_action.flowid = 0x80 + i;
		rule.action.fwd_action.processpath = 2;

		if (pae_hal_add_class_rule(&rule) != PPA_SUCCESS) {
			dbg("add lro rule add returned failure %d\n", ret);
			g_lro_table[i].enabled = 0;
			goto STOP_FLOW;
		}
		g_lro_table[i].session_uid[0] = rule.uidx;

		ppa_memset(&rule, 0, sizeof(PPA_CLASS_RULE));

		rule.in_dev = GSWR_INGRESS;
		rule.category = CAT_LRO;

		rule.pattern.bEnable = 1;

		rule.pattern.bParserFlagMSB_Enable = 1;
		rule.pattern.nParserFlagMSB = PCE_PARSER_MSB_LRO_EXCEP | PCE_PARSER_MSB_TCP;
		rule.pattern.nParserFlagMSB_Mask =
			(uint16_t)(~(PCE_PARSER_MSB_LRO_EXCEP | PCE_PARSER_MSB_TCP |
				     PCE_PARSER_MSB_INNR_IPV6 | PCE_PARSER_MSB_INNR_IPV4));
		if (g_lro_table[i].f_ipv6) {
			rule.pattern.eDstIP_Select = GSW_PCE_IP_V6;
			ppa_memcpy(rule.pattern.nDstIP.nIPv6,
				   g_lro_table[i].dstip.ip6,
				   (sizeof(uint16_t) * 8));
			rule.pattern.nDstIP_Mask = 0x0000;
			rule.pattern.eSrcIP_Select = GSW_PCE_IP_V6;
			ppa_memcpy(rule.pattern.nSrcIP.nIPv6,
				   g_lro_table[i].srcip.ip6,
				   (sizeof(uint16_t) * 8));
			rule.pattern.nSrcIP_Mask = 0x0000;
		} else {
			rule.pattern.eDstIP_Select = GSW_PCE_IP_V4;
			rule.pattern.nDstIP.nIPv4 = g_lro_table[i].dstip.ip;
			rule.pattern.nDstIP_Mask = 0xFF00;
			rule.pattern.eSrcIP_Select = GSW_PCE_IP_V4;
			rule.pattern.nSrcIP.nIPv4 = g_lro_table[i].srcip.ip;
			rule.pattern.nSrcIP_Mask = 0xFF00;
		}
		//*TCP source port and destination port*/
		rule.pattern.bAppDataMSB_Enable = 1;
		rule.pattern.nAppDataMSB = g_lro_table[i].srcport;
		rule.pattern.nAppMaskRangeMSB = 0xF0;

		rule.pattern.bAppDataLSB_Enable = 1;
		rule.pattern.nAppDataLSB = g_lro_table[i].dstport;
		rule.pattern.nAppMaskRangeLSB = 0xF0;

		rule.pattern.bEnable = 1;
		rule.pattern.bPktLngEnable = 1;
		rule.pattern.nPktLng = g_min_lrolen;
		rule.pattern.nPktLngRange = 1500;
		rule.action.fwd_action.rtctrlenable = 1;
		rule.action.qos_action.flowid_enabled = 1;
		rule.action.qos_action.flowid = 0xC0 + i;
		rule.action.fwd_action.processpath = 2;

		if (pae_hal_add_class_rule(&rule) != PPA_SUCCESS) {
			dbg("add lro rule2 returned failure %d\n", ret);
			g_lro_table[i].enabled = 0;
			goto DEL_RULE;
		}
		g_lro_table[i].session_uid[1] = rule.uidx;
		g_lro_table[i].session_uid_enabled = 1;
	}

	ppa_lock_release(&g_lro_table_lock);

	dbg("add_lro_entry succeeded...\n");
	return PPA_SUCCESS;

DEL_RULE:
	ppa_memset(&rule, 0, sizeof(PPA_CLASS_RULE));
	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_LRO;
	rule.uidx = g_lro_table[i].session_uid[0];
	pae_hal_del_class_rule(&rule);

STOP_FLOW:
#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
	lro_stop_flow(i, 0, 0);
#endif
	ppa_lock_release(&g_lro_table_lock);
	return PPA_FAILURE;
}

/*!
	\fn int32_t del_lro_entry( )
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief delete a LRO HW session
	\param uint8_t sessionid
	\return >= 0: success, otherwise: fail
 */

int32_t del_lro_entry(uint8_t sessionid)
{
	uint32_t ret = PPA_SUCCESS;
	PPA_CLASS_RULE rule;

	sessionid -= 1;
	if (sessionid >= MAX_LRO_ENTRIES) {
		dbg("Invalid LRO rule index\n");
		return PPA_FAILURE;
	}

	ppa_lock_get(&g_lro_table_lock);

	if (!g_lro_table[sessionid].enabled) {
		ppa_lock_release(&g_lro_table_lock);
		dbg("No LRO entry at the index %d\n", (sessionid + 1));
		return PPA_FAILURE;
	}

	if (g_lro_table[sessionid].session_uid_enabled) {
		ppa_memset(&rule, 0, sizeof(PPA_CLASS_RULE));

		rule.in_dev = GSWR_INGRESS;
		rule.category = CAT_LRO;
		rule.uidx = g_lro_table[sessionid].session_uid[0];
		pae_hal_del_class_rule(&rule);

		ppa_memset(&rule, 0, sizeof(PPA_CLASS_RULE));

		rule.in_dev = GSWR_INGRESS;
		rule.category = CAT_LRO;
		rule.uidx = g_lro_table[sessionid].session_uid[1];
		pae_hal_del_class_rule(&rule);
	}

	/*call the to delete the flow*/
#if IS_ENABLED(CONFIG_LTQ_TOE_DRIVER)
	lro_stop_flow(sessionid, 0, 0);
#endif

	/* free the flow table */
	ppa_memset(&(g_lro_table[sessionid]), 0, sizeof(ppa_lro_entry));

	ppa_lock_release(&g_lro_table_lock);

	dbg("del_lro_entry succeeded...\n");
	return ret;
}

/*!
	\fn int32_t add_vlan_entry(uint32_t new_tag,
							 uint32_t *p_entry)
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief add outer VLAN tag
	\param PPA_OUT_VLAN_INFO * vlan_entry
	\return 0: OK, otherwise: fail
 */
int32_t add_vlan_entry(PPA_OUT_VLAN_INFO *vlan_entry)
{
	int ret=0;
	GSW_PCE_EgVLAN_Entry_t egV={0};
	GSW_SVLAN_cfg_t svlancfg={0};
	GSW_SVLAN_portCfg_t svlan_portcfg = {0};

	if(vlan_entry->port_id < MAX_PAE_PORTS) {
		/* The sub interface id 16 is always used for no vlan acion behavior on the interface
		//will be set during the initialization of PAE HAL*/
		if( ((vlan_entry->subif_id > (vlan_tbl_size[vlan_entry->port_id] - 1)) || (vlan_entry->subif_id > (MAX_SUBIF_IDS - 1))) ) {
			/* sub interfaces range <1 -15> can be used */
			dbg("subif_if out of range\n");
			return PPA_FAILURE;
		}
		/*
		 * PPA_VLAN_PROTO_8021AD type will be set with subif_id zero in order
		 * to reset the cvid_rem bit. So we should not return if 8021AD type
		 * is set.
		 */
		else if (vlan_entry->subif_id == 0 && vlan_entry->vlan_type != PPA_VLAN_PROTO_8021AD) {
			/*The sub interface id 0 is used for un tagged behavior on the base interface. */
			/* will be set during the initialization of PAE HAL */
			vlan_entry->vlan_entry = vlan_tbl_base[vlan_entry->port_id]; 
			return PPA_SUCCESS;	
		}

		if(vlan_entry->vlan_type == PPA_VLAN_PROTO_8021AD){
			svlancfg.nEthertype = PPA_VLAN_PROTO_8021AD;		
			if((ret=ltq_try_gswapi_kioctl( GSW_SVLAN_CFG_SET,(unsigned int)&svlancfg)) < GSW_statusOk) {
				dbg("GSW_SVLAN_CFG_SET returned faulure%d\n",ret);
			} else {
				dbg("SVLAN type set to %x\n",PPA_VLAN_PROTO_8021AD);
			} 
		}

		ppa_memset (&egV, 0x00, sizeof(egV));

		egV.nPortId = vlan_entry->port_id;
		egV.nIndex = vlan_tbl_base[vlan_entry->port_id] + vlan_entry->subif_id;
		egV.bEgVLAN_Action = 1;
		egV.bEgSVidRem_Action = vlan_entry->stag_rem;
		egV.bEgCVidRem_Action = vlan_entry->ctag_rem;
		egV.nEgSVid = vlan_entry->stag_vlan_id;
		egV.bEgCVidIns_Action = vlan_entry->ctag_ins;	
		egV.bEgSVidIns_Action = vlan_entry->stag_ins;
		egV.nEgCVid = vlan_entry->vlan_id;

		if((ret=ltq_try_gswapi_kioctl( GSW_PCE_EG_VLAN_ENTRY_WRITE,(unsigned int)&egV)) < GSW_statusOk) {
			dbg("GSW_PCE_EG_VLAN_ENTRY_WRITE returned faulure%d\n",ret);
		} else {
			vlan_entry->vlan_entry = egV.nIndex;
			dbg("\nadd vlan entry success \nnPortId =%d \nnIndex =%d \nbEgVLAN_Action =%d \nbEgSVidRem_Action =%d \nbEgCVidRem_Action =%d \
			\nEgSVid =%d \nbEgCVidIns_Action =%d \nbEgSVidIns_Action =%d \nnEgCVid =%d \n",
			egV.nPortId,egV.nIndex, egV.bEgVLAN_Action, egV.bEgSVidRem_Action, egV.bEgCVidRem_Action, 
 			egV.nEgSVid,egV.bEgCVidIns_Action,egV.bEgSVidIns_Action,egV.nEgCVid);
		}

		/*
		 * Enable STAG VLAN Support for the port, where:
		 * 1. Both STAG and CTAG VLAN insert action applied.
		 * 2. STAG insert action applied with 802.1AD VLAN type.
		 */
		if ((vlan_entry->stag_ins && vlan_entry->ctag_ins) ||
				(vlan_entry->stag_ins && vlan_entry->vlan_type == PPA_VLAN_PROTO_8021AD)) {
			svlan_portcfg.nPortId = vlan_entry->port_id;
			if (ltq_try_gswapi_kioctl(GSW_SVLAN_PORT_CFG_GET, (unsigned int)&svlan_portcfg) >= GSW_statusOk) {
				svlan_portcfg.bSVLAN_TagSupport = 1;
				if (ltq_try_gswapi_kioctl(GSW_SVLAN_PORT_CFG_SET, (unsigned int)&svlan_portcfg) < GSW_statusOk) {
					dbg("GSW_SVLAN_PORT_CFG_SET returned faulure\n");
				}
			} else
				dbg("GSW_SVLAN_PORT_CFG_GET returned faulure\n");
		}
	}
	return ret;
}
EXPORT_SYMBOL(add_vlan_entry);

/*!
	\fn void del_vlan_entry(uint32_t entry)
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief delete outer VLAN tag
	\param entry	entry number got from function call "add_vlan_entry"
	\return no return value
 */
int32_t del_vlan_entry(PPA_OUT_VLAN_INFO *vlan_entry)
{
	GSW_PCE_EgVLAN_Entry_t egV={0};
	int32_t ret=0;

	if(vlan_entry->port_id < MAX_PAE_PORTS) {
	/*The sub interface id 0 is used for un tagged behavior on the base interface. 
	and the sub interface id 16 is always used for no vlan acion behavior on the interface
	will be set during the initialization of PAE HAL*/
	if( ((vlan_entry->subif_id > (vlan_tbl_size[vlan_entry->port_id] - 1)) || (vlan_entry->subif_id > (MAX_SUBIF_IDS - 1))) ) {
		dbg("%s: Invalid Subifid !!\n", __func__ );
		return PPA_FAILURE;	
	} else if ( vlan_entry->subif_id == 0) {
		if( vlan_entry->vlan_entry == 0 || vlan_entry->vlan_entry == vlan_tbl_base[vlan_entry->port_id])
			return PPA_SUCCESS;
		else {
			egV.nIndex = vlan_entry->vlan_entry;	
		}
	} else {
		egV.nIndex = vlan_tbl_base[vlan_entry->port_id] + vlan_entry->subif_id;
	}	

	ppa_memset (&egV, 0x00, sizeof(egV));

	/* set the egress vlan tratment for port i index 0 as untag all*/
	egV.nPortId = vlan_entry->port_id;
	egV.nIndex = vlan_tbl_base[vlan_entry->port_id] + vlan_entry->subif_id;
	egV.bEgVLAN_Action = 0;
	if((ltq_try_gswapi_kioctl( GSW_PCE_EG_VLAN_ENTRY_WRITE,(unsigned int)&egV)) < GSW_statusOk) {
		dbg("GSW_PCE_EG_VLAN_CFG_SET	returned faulure\n");
	}
	
	}
	return ret;
}

/*!
	\fn int32_t get_vlan_entry(uint32_t entry,
							 uint32_t *p_outer_vlan_tag)
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief get outer VLAN tag
	\param entry				entry number got from function call "add_vlan_entry"
	\param p_outer_vlan_tag	 a data pointer to get outer VLAN tag
	\return 0: OK, otherwise: fail
 */
int32_t get_vlan_entry(PPA_OUT_VLAN_INFO *vlan_entry)
{
	int ret=0;
	GSW_PCE_EgVLAN_Entry_t egV={0};
	
	if(vlan_entry->port_id < MAX_PAE_PORTS) {

		/* The sub interface id 0 is used for un tagged behavior on the base interface. 
		//and the sub interface id 16 is always used for no vlan acion behavior on the interface
		//will be set during the initialization of PAE HAL */
		if( (vlan_entry->subif_id < 1) && ((vlan_entry->subif_id > (vlan_tbl_size[vlan_entry->port_id] - 1)) ||
				(vlan_entry->subif_id > (MAX_SUBIF_IDS - 1))) ) {
			/* sub interfaces range <1 -15> can be used */
			return PPA_FAILURE;	
		}

		ppa_memset (&egV, 0x00, sizeof(egV));

		/* set the egress vlan tratment for port i index 0 as untag all*/
		egV.nPortId = vlan_entry->port_id;
		egV.nIndex = vlan_tbl_base[vlan_entry->port_id] + vlan_entry->subif_id;
		if((ret=ltq_try_gswapi_kioctl( GSW_PCE_EG_VLAN_ENTRY_READ,(unsigned int)&egV)) < GSW_statusOk) {
			dbg( "GSW_PCE_EG_VLAN_CFG_SET	returned faulure%d\n",ret);
			return ret; 
		}	 

		vlan_entry->stag_rem = egV.bEgSVidRem_Action;
		vlan_entry->ctag_rem = egV.bEgCVidRem_Action;
		vlan_entry->stag_vlan_id = egV.nEgSVid;
		vlan_entry->ctag_ins = egV.bEgCVidIns_Action;	
		vlan_entry->stag_ins = egV.bEgSVidIns_Action;
		vlan_entry->vlan_id = egV.nEgCVid;
	
	}
	return PPA_SUCCESS;
}

void get_itf_mib(uint32_t itf, struct ppe_itf_mib *p)
{
	GSW_RMON_Route_cnt_t pae_port_mib={0};

	if ( p != NULL && itf < MAX_PAE_PORTS ) {
		ppa_memset(&pae_port_mib, 0, sizeof(GSW_RMON_Route_cnt_t));
		
		pae_port_mib.nRoutedPortId = itf;
	
		if((ltq_try_gswapi_kioctl( GSW_RMON_ROUTE_GET, (unsigned int)&pae_port_mib)) >= GSW_statusOk) {

			p->ig_fast_rt_ipv4_udp_pkts = pae_port_mib.nRxUCv4UDPPktsCount;
			p->ig_fast_rt_ipv4_tcp_pkts = pae_port_mib.nRxUCv4TCPPktsCount;
			p->ig_fast_rt_ipv4_mc_pkts = pae_port_mib.nRxMCv4PktsCount;
			p->ig_fast_rt_ipv4_bytes = pae_port_mib.nRxIPv4BytesCount;
			p->ig_fast_rt_ipv6_udp_pkts = pae_port_mib.nRxUCv6UDPPktsCount;
			p->ig_fast_rt_ipv6_tcp_pkts = pae_port_mib.nRxUCv6TCPPktsCount;
			p->ig_fast_rt_ipv6_mc_pkts = pae_port_mib.nRxMCv6PktsCount;
			p->ig_fast_rt_ipv6_bytes = pae_port_mib.nRxIPv6BytesCount;
			p->ig_cpu_pkts = pae_port_mib.nRxCpuPktsCount;
			p->ig_cpu_bytes = pae_port_mib.nRxCpuBytesCount;
			p->ig_drop_pkts = pae_port_mib.nRxPktsDropCount;
			p->ig_drop_bytes = pae_port_mib.nRxBytesDropCount;
			p->eg_fast_pkts = pae_port_mib.nTxPktsCount;
			p->eg_fast_bytes = pae_port_mib.nTxBytesCount;
	
		}
	}
}

/*!
	\fn get_itf_mib_generic(
	\ingroup GRX500_PPA_PAE_GLOBAL_FUNCTIONS
	\brief Get the interface MIB for physical interface from Switch and for logical interface from Meter 
	\param MIN Info data structure	
	\return no return value
 */
int32_t get_itf_mib_generic(PPA_ITF_MIB_INFO *mib_tmp)
{
	int ret;
	GSW_RMON_Route_cnt_t pae_port_mib = {0};
	struct intf_mib *p = NULL;
	struct netif_info *ifinfo_tmp = NULL;
	uint32_t itf = 0;
#ifdef PAE_HW_METER
	int indx;
#endif
	if ((mib_tmp == NULL) || (mib_tmp->ifinfo == NULL))
		return PPA_FAILURE;

	ifinfo_tmp = mib_tmp->ifinfo;
	p = &(mib_tmp->mib);

	if (p == NULL)
		return PPA_FAILURE;

	if ((strcmp(ifinfo_tmp->name, ifinfo_tmp->phys_netif_name) == 0) &&
			!(ifinfo_tmp->flags & NETIF_BRIDGE)) {
		itf = ifinfo_tmp->phys_port;

		if (itf < MAX_PAE_PORTS) {
			ppa_memset(&pae_port_mib, 0,
					sizeof(GSW_RMON_Route_cnt_t));
			pae_port_mib.nRoutedPortId = itf;
			GSWR_OPS(rmon_ops, RMON_Route_Get, &pae_port_mib, ret);
			if (ret >= GSW_statusOk) {
				p->rx_packets = pae_port_mib.nRxUCv4UDPPktsCount;
				p->rx_packets += pae_port_mib.nRxUCv4TCPPktsCount;
				p->rx_bytes = pae_port_mib.nRxIPv4BytesCount;
				p->rx_packets += pae_port_mib.nRxUCv6UDPPktsCount;
				p->rx_packets += pae_port_mib.nRxUCv6TCPPktsCount;
				p->rx_bytes += pae_port_mib.nRxIPv6BytesCount;
				p->rx_dropped = pae_port_mib.nRxPktsDropCount;
				p->tx_packets = pae_port_mib.nTxPktsCount;
				p->tx_bytes = pae_port_mib.nTxBytesCount;
				/*dbg("\n%s: Interface name=%s,Rx Pkt count=%llu,Tx Pkt count=%llu\n",__FUNCTION__,ifinfo_tmp->name,p->rx_packets,p->tx_packets);
				dbg("\nTx Pkt count=%u\n",pae_port_mib.nTxPktsCount);*/
			}
		} else {
			return PPA_FAILURE;
		}
	} else {
		itf = 0;
		/* Read from the meter for the logical interface */
#ifdef PAE_HW_METER
		for_each_set_bit(indx, &rxid_bitmap, PPA_MAX_IFS_NUM)
			if(strncmp(rx_meter[indx].ifname, ifinfo_tmp->name,
						PPA_IF_NAME_SIZE) == 0) {
				p->rx_packets = rx_meter[indx].cnt;
				break;
			}
		for_each_set_bit(indx, &txid_bitmap, PPA_MAX_IFS_NUM)
			if(strncmp(tx_meter[indx].ifname, ifinfo_tmp->name,
						PPA_IF_NAME_SIZE) == 0) {
				p->tx_packets = tx_meter[indx].cnt;
				break;
			}
#endif
	}
	return PPA_SUCCESS;
}

/*!
	\fn uint32_t get_routing_entry_bytes(uint32_t session_index, uint32_t *f_hit, uint32_t reset_flag)
	\brief get one routing entry's byte counter
	\param entry	entry number got from function call "add_routing_entry"
	\param f_hit hit status
	\param count counter value
	\return error code from switch API
 */
int32_t get_routing_entry_bytes(uint32_t session_index, uint32_t *f_hit, uint32_t *count, uint8_t reset_flag)
{
	int ret;
	GSW_ROUTE_Session_Hit_t sess_stat = {0};

	sess_stat.nRtIndex = session_index;
	sess_stat.eHitOper = reset_flag;

	GSWR_OPS(pae_ops, ROUTE_SessHitOp, &sess_stat, ret);
	if (ret >= GSW_statusOk) {
		*f_hit = sess_stat.bHitStatus;
		*count = sess_stat.nSessCntr;
	}

	return ret;
}
/*!
	\fn uint32_t get_rtp_sampling_cnt(uint32_t session_index, uint32_t *seq_num, uint32_t *pkt_cnt)
	\brief get one routing entry's byte counter
	\param entry	entry number got from function call "add_routing_entry"
	\param seq_num sequence number
	\param pkt_count packet counter
	\return error code from switch API
 */
int32_t get_rtp_sampling_cnt(uint32_t session_index, uint32_t *seq_num, uint32_t *pkt_cnt)
{
	int ret=0;
	GSW_ROUTE_Entry_t rt_entry= {0};

	rt_entry.nRtIndex = session_index;

	if((ret = ltq_try_gswapi_kioctl( GSW_ROUTE_ENTRY_READ, (unsigned int)&rt_entry)) >= GSW_statusOk) {
		*seq_num = rt_entry.routeEntry.action.nRTPSeqNumber;
		*pkt_cnt = rt_entry.routeEntry.action.nRTPSessionPktCnt;	
	}

	return ret;

}

int32_t test_and_clear_hit_stat(uint32_t session_index, uint8_t *f_hit)
{
	GSW_ROUTE_Session_Hit_t sess_stat = {0};
	int ret=0;

	sess_stat.eHitOper = GSW_ROUTE_HIT_CLEAR;
	sess_stat.nRtIndex = session_index;
	
	if((ret = ltq_try_gswapi_kioctl( GSW_ROUTE_SESSION_HIT_OP, (unsigned int)&sess_stat)) >= GSW_statusOk) {
		*f_hit = sess_stat.bHitStatus;
	}


	return ret;
}

int32_t test_and_clear_bridging_hit_stat(uint32_t fid, uint8_t	mac[PPA_ETH_ALEN], uint32_t *f_hit, uint32_t *age_time)
{
	
	int ret=PPA_SUCCESS;
	int8_t strbuf1[24]={0};
	 
	GSW_MAC_tableQuery_t mac_entry;

	ppa_memset(&mac_entry,0,sizeof(GSW_MAC_tableQuery_t));

	mac_entry.nFId = fid;
	ppa_memcpy(mac_entry.nMAC, mac, PPA_ETH_ALEN);
	
	if((ret=ltq_try_gswapi_kioctl( GSW_MAC_TABLE_ENTRY_QUERY, (unsigned int)&mac_entry)) >= GSW_statusOk) {
		*age_time = mac_entry.nAgeTimer;
		*f_hit = mac_entry.bFound;	
	}
	dbg("\nbridging_entry hit\n src_mac[6]=%s\n fid=%x\n age_time=%d f_hit=%d", ppa_get_pkt_mac_string(mac, strbuf1), fid, mac_entry.nAgeTimer, mac_entry.bFound);

	return ret; 
}

static int32_t uninit_class_mgmt(void)
{
	int32_t ret= PPA_SUCCESS, i;
	
	spin_lock_bh(&g_class_lock );
	for(i=0; i< GSWR_PCE_MAX; i++) {
		if( class_dev[GSWR_INGRESS].cat_ordr_vect[i].usage_flg) {
		/* call the pae flow disable function in index*/
		}
	}	
	
	ppa_free(class_dev[GSWR_INGRESS].cat_ordr_vect);
	ppa_free(class_dev[GSWR_INGRESS].pce_tbl_bitmap);

	for(i=0; i< GSWL_PCE_MAX; i++) {
		if( class_dev[GSWL_INGRESS].cat_ordr_vect[i].usage_flg) {
			/* call the pae flow disable function in index*/
		}
	}
	
	ppa_free(class_dev[GSWL_INGRESS].cat_ordr_vect);
	ppa_free(class_dev[GSWL_INGRESS].pce_tbl_bitmap);

	for( i=0; i < CAT_MAX; i++) {
		ppa_free(class_dev[GSWL_INGRESS].cat_map[i].cat_idx_vect);
		ppa_free(class_dev[GSWR_INGRESS].cat_map[i].cat_idx_vect);
	}
	
	spin_unlock_bh(&g_class_lock );
	 
	return ret;
}

static int32_t init_class_mgmt(void)
{
	int32_t ret= PPA_SUCCESS, i;

	spin_lock_init(&g_class_lock);

	ppa_memset(class_dev,0,sizeof(struct switch_dev_class)*2);

	class_dev[GSWL_INGRESS].tot_max = GSWL_PCE_MAX;
	class_dev[GSWL_INGRESS].tot_used = 0;
	
	class_dev[GSWR_INGRESS].tot_max = GSWR_PCE_MAX;
	class_dev[GSWR_INGRESS].tot_used = 0;
 
	for( i=0; i < CAT_MAX; i++) {

		class_dev[GSWL_INGRESS].cat_map[i].cat_id = i;
		class_dev[GSWL_INGRESS].cat_map[i].cat_max = g_gswl_cat_conf[i].cat_max;
		class_dev[GSWL_INGRESS].cat_map[i].cat_start_idx = g_gswl_cat_conf[i].cat_start_idx;
		if( g_gswl_cat_conf[i].cat_max) {	
			class_dev[GSWL_INGRESS].cat_map[i].cat_idx_vect = (cat_idx_map_t *) kmalloc ( sizeof(cat_idx_map_t) * g_gswl_cat_conf[i].cat_max, GFP_KERNEL); 	 
			ppa_memset(class_dev[GSWL_INGRESS].cat_map[i].cat_idx_vect,0x0000FFFF,sizeof(cat_idx_map_t) * g_gswl_cat_conf[i].cat_max);
		}
	
/*	printk(KERN_INFO"class_dev[GSWL_INGRESS].cat_map[%d].cat_id = %d cat_max=%d cat_start_idx=%d \n", i, 
		class_dev[GSWL_INGRESS].cat_map[i].cat_id, class_dev[GSWL_INGRESS].cat_map[i].cat_max, class_dev[GSWL_INGRESS].cat_map[i].cat_start_idx); */

		class_dev[GSWL_INGRESS].cat_map[i].cat_used = 0; 
		class_dev[GSWL_INGRESS].cat_map[i].cat_last_ordr = -1;
	
		class_dev[GSWR_INGRESS].cat_map[i].cat_id = i; 
		class_dev[GSWR_INGRESS].cat_map[i].cat_max = g_gswr_cat_conf[i].cat_max;
		class_dev[GSWR_INGRESS].cat_map[i].cat_start_idx = g_gswr_cat_conf[i].cat_start_idx;
		if(g_gswr_cat_conf[i].cat_max) {
			class_dev[GSWR_INGRESS].cat_map[i].cat_idx_vect = (cat_idx_map_t *) kmalloc ( sizeof(cat_idx_map_t) * g_gswr_cat_conf[i].cat_max, GFP_KERNEL); 	 
			ppa_memset(class_dev[GSWR_INGRESS].cat_map[i].cat_idx_vect,0x0000FFFF,sizeof(cat_idx_map_t) * g_gswr_cat_conf[i].cat_max);
		}

/*	printk(KERN_INFO"class_dev[GSWR_INGRESS].cat_map[%d].cat_id = %d cat_max=%d cat_start_idx=%d \n", i, class_dev[GSWR_INGRESS].cat_map[i].cat_id, 
		class_dev[GSWR_INGRESS].cat_map[i].cat_max, class_dev[GSWR_INGRESS].cat_map[i].cat_start_idx); */
	
		class_dev[GSWR_INGRESS].cat_map[i].cat_used = 0; 
		class_dev[GSWR_INGRESS].cat_map[i].cat_last_ordr = -1;

	}
	
	/*Kamal hardcoded for the time being with subcat GSWR_CAT_FILTER under CAT_FILTER*/
	/*subcat setup PAE*/
	class_dev[GSWR_INGRESS].subcat_map[0].cat_id = CAT_NONE;
	class_dev[GSWR_INGRESS].subcat_map[0].subcat_id = SUBCAT_NONE;
	class_dev[GSWR_INGRESS].subcat_map[0].subcat_max = GSWR_PCE_MAX;
	class_dev[GSWR_INGRESS].subcat_map[0].subcat_used = 0;

	class_dev[GSWR_INGRESS].subcat_map[1].cat_id = CAT_FILTER;
	class_dev[GSWR_INGRESS].subcat_map[1].subcat_id = SUBCAT_WLAN_FILTER;
	class_dev[GSWR_INGRESS].subcat_map[1].subcat_max = GSWR_CAT_FILTER_MAX/2;	 
	class_dev[GSWR_INGRESS].subcat_map[1].subcat_used = 0;

	/*subcat setup GSWL*/
	class_dev[GSWL_INGRESS].subcat_map[0].cat_id = 0;
	class_dev[GSWL_INGRESS].subcat_map[0].subcat_id = SUBCAT_NONE;
	class_dev[GSWL_INGRESS].subcat_map[0].subcat_max = GSWL_PCE_MAX;
	class_dev[GSWL_INGRESS].subcat_map[0].subcat_used = 0;

	class_dev[GSWL_INGRESS].subcat_map[1].cat_id = CAT_FILTER;
	class_dev[GSWL_INGRESS].subcat_map[1].subcat_id = SUBCAT_WLAN_FILTER;
	class_dev[GSWL_INGRESS].subcat_map[1].subcat_max = GSWL_CAT_FILTER_MAX/2;	 
	class_dev[GSWL_INGRESS].subcat_map[1].subcat_used = 0;

	class_dev[GSWL_INGRESS].pce_tbl_bitmap = (uint32_t*) kmalloc(sizeof(uint32_t)*(GSWL_PCE_MAX/32), GFP_KERNEL);
	ppa_memset(class_dev[GSWL_INGRESS].pce_tbl_bitmap,0,sizeof(uint32_t)*(GSWL_PCE_MAX/32));
	class_dev[GSWL_INGRESS].cat_ordr_vect = (cat_ordr_vect_t*) kmalloc(sizeof(cat_ordr_vect_t)*GSWL_PCE_MAX, GFP_KERNEL);
	ppa_memset(class_dev[GSWL_INGRESS].cat_ordr_vect, 0, sizeof(cat_ordr_vect_t)*GSWL_PCE_MAX);

	class_dev[GSWR_INGRESS].pce_tbl_bitmap = (uint32_t*) kmalloc(sizeof(uint32_t)*(GSWR_PCE_MAX/32), GFP_KERNEL);
	ppa_memset(class_dev[GSWR_INGRESS].pce_tbl_bitmap,0,sizeof(uint32_t)*(GSWR_PCE_MAX/32));
	class_dev[GSWR_INGRESS].cat_ordr_vect = (cat_ordr_vect_t*) kmalloc(sizeof(cat_ordr_vect_t)*GSWR_PCE_MAX, GFP_KERNEL);
	ppa_memset(class_dev[GSWR_INGRESS].cat_ordr_vect, 0, sizeof(cat_ordr_vect_t)*GSWR_PCE_MAX);

	/*printk(KERN_INFO"class_dev[GSWL_INGRESS].pce_tbl_bitmap = %d class_dev[GSWR_INGRESS].pce_tbl_bitmap=%d \n",
		sizeof(uint32_t)*(GSWL_PCE_MAX/32), sizeof(uint32_t)*(GSWR_PCE_MAX/32));
	printk(KERN_INFO"class_dev[GSWL_INGRESS].tot_max = %d, class_dev[GSWL_INGRESS].tot_used = %d\n", 
		class_dev[GSWL_INGRESS].tot_max, class_dev[GSWL_INGRESS].tot_used);
	printk(KERN_INFO"class_dev[GSWR_INGRESS].tot_max = %d, class_dev[GSWR_INGRESS].tot_used = %d\n",
		class_dev[GSWR_INGRESS].tot_max, class_dev[GSWR_INGRESS].tot_used);*/

	return ret;
}

static uint8_t bitmap_isset( ppa_class_devingress_t indev, uint16_t index) 
{
	int idx = index/32, offset=index%32;

	if(!offset) { idx-=1; offset = 31;} else { offset-=1;}
	if(indev <= GSWL_INGRESS) {
		/*printk(KERN_INFO" idx =%d offset =%d class_dev[indev].pce_tbl_bitmap[idx] = %u, val=%u\n", 
			idx, offset, class_dev[indev].pce_tbl_bitmap[idx], ((class_dev[indev].pce_tbl_bitmap[idx] >> offset) & 0x1));*/
		return ((class_dev[indev].pce_tbl_bitmap[idx] >> offset) & 0x1);
	}
	return 1;
} 

static int32_t set_bitmap(ppa_class_devingress_t indev, uint16_t index)
{
	int idx = index/32, offset=index%32;

	if(!offset) { idx-=1; offset = 31;} else { offset-=1;}
	if(indev <= GSWL_INGRESS) {
		if(!((class_dev[indev].pce_tbl_bitmap[idx] >> offset) & 0x1)) {
			class_dev[indev].pce_tbl_bitmap[idx] |= (1 << offset);
			return PPA_SUCCESS;
		}
	}
	return PPA_FAILURE;
}

static int32_t clear_bitmap(ppa_class_devingress_t indev, uint16_t index)
{
	int idx = index/32, offset=index%32;

	if(!offset) { idx-=1; offset = 31;} else { offset-=1;}
	if(indev <= GSWL_INGRESS) {
		if((class_dev[indev].pce_tbl_bitmap[idx] >> offset) & 0x1) {
			/*printk(KERN_INFO" bitmap before reset = %u\n", class_dev[indev].pce_tbl_bitmap[idx]);*/
			class_dev[indev].pce_tbl_bitmap[idx] &= ~(1 << offset);
			/*printk(KERN_INFO" bitmap after reset = %u\n", class_dev[indev].pce_tbl_bitmap[idx]);*/
			return PPA_SUCCESS;
		}
	}
	return PPA_FAILURE;
}

static int32_t pce_rule_read(GSW_API_HANDLE* gsw_handle, GSW_PCE_rule_t* pcerule, uint16_t idx)
{
	int32_t ret = PPA_SUCCESS;

	pcerule->pattern.nIndex = idx;

	if((gsw_api_kioctl(*gsw_handle, GSW_PCE_RULE_READ, pcerule)) < GSW_statusOk) {
		dbg("read_routing_entry returned Failure \n");
		ret=PPA_FAILURE;
	}

	return ret;
}


static int32_t pce_rule_write(GSW_API_HANDLE* gsw_handle, GSW_PCE_rule_t* pcerule, uint16_t idx)
{
	int32_t ret = PPA_SUCCESS;

	pcerule->pattern.nIndex = idx;
	/*printk(KERN_INFO "writing pattern.nIndex = %d\n", pcerule->pattern.nIndex); */

	if((gsw_api_kioctl(*gsw_handle, GSW_PCE_RULE_WRITE, pcerule)) < GSW_statusOk) {
		dbg("read_routing_entry returned Failure \n");
	ret=PPA_FAILURE;
	}

	return ret;
}

static int32_t open_switch_dev(ppa_class_devingress_t ing_dev, GSW_API_HANDLE* gsw_handle) 
{	
	if(ing_dev==GSWL_INGRESS) {
		*gsw_handle = gsw_api_kopen("/dev/switch_api/0");
	} else {
		*gsw_handle = gsw_api_kopen("/dev/switch_api/1");
	}
	if (*gsw_handle == 0) {
		dbg( "%s: Open SWAPI device FAILED !!\n", __func__ );
		return PPA_FAILURE;
	}
	return PPA_SUCCESS;
}

void copy_class_to_pce(GSW_PCE_rule_t* pcerule, PPA_CLASS_RULE* classrule) 
{
	/*copy the pattern */
	ppa_memcpy(&pcerule->pattern,&classrule->pattern, sizeof(GSW_PCE_pattern_t));

	/*copy the actions*/
	/*filter actions*/
	pcerule->action.ePortFilterType_Action = classrule->action.filter_action.portfilter;
	pcerule->action.eCrossStateAction = classrule->action.filter_action.crossstate;
	/*vlan actions*/
	pcerule->action.nFId = classrule->action.vlan_action.fid;
	pcerule->action.eVLAN_Action = classrule->action.vlan_action.cvlan;
	pcerule->action.nVLAN_Id = classrule->action.vlan_action.vlan_id;
	pcerule->action.eSVLAN_Action = classrule->action.vlan_action.svlan;
	pcerule->action.nSVLAN_Id = classrule->action.vlan_action.svlan_id;	 
	pcerule->action.eVLAN_CrossAction = classrule->action.vlan_action.cross_vlan;
	pcerule->action.bCVLAN_Ignore_Control = classrule->action.vlan_action.cvlan_ignore;
	/*fwd actions*/
	pcerule->action.eLearningAction = classrule->action.fwd_action.learning;
	pcerule->action.bPortTrunkAction = classrule->action.fwd_action.port_trunk;
	pcerule->action.ePortMapAction = classrule->action.fwd_action.portmap;
	pcerule->action.nForwardPortMap[0] = classrule->action.fwd_action.forward_portmap;
	pcerule->action.nForwardSubIfId = classrule->action.fwd_action.forward_subifid;
	pcerule->action.bRoutExtId_Action = classrule->action.fwd_action.routextid_enable;
	pcerule->action.nRoutExtId = classrule->action.fwd_action.routextid;
	pcerule->action.bRtDstPortMaskCmp_Action = classrule->action.fwd_action.rtdestportmaskcmp;
	pcerule->action.bRtSrcPortMaskCmp_Action = classrule->action.fwd_action.rtsrcportmaskcmp;
	pcerule->action.bRtDstIpMaskCmp_Action = classrule->action.fwd_action.rtdstipmaskcmp;
	pcerule->action.bRtSrcIpMaskCmp_Action = classrule->action.fwd_action.rtsrcipmaskcmp;
	pcerule->action.bRtInnerIPasKey_Action = classrule->action.fwd_action.rtinneripaskey;
	pcerule->action.bRtAccelEna_Action = classrule->action.fwd_action.rtaccelenable;
	pcerule->action.bRtCtrlEna_Action = classrule->action.fwd_action.rtctrlenable;
	pcerule->action.eProcessPath_Action = classrule->action.fwd_action.processpath;
	/*QoS actions*/
	pcerule->action.eTrafficClassAction = classrule->action.qos_action.trafficclass;
	pcerule->action.nTrafficClassAlternate = classrule->action.qos_action.alt_trafficclass;
	pcerule->action.eMeterAction = classrule->action.qos_action.meter;
	pcerule->action.nMeterId = classrule->action.qos_action.meterid;
	pcerule->action.eCritFrameAction = classrule->action.qos_action.criticalframe;
	pcerule->action.bRemarkAction = classrule->action.qos_action.remark;
	pcerule->action.bRemarkPCP = classrule->action.qos_action.remarkpcp;
	pcerule->action.bRemarkSTAG_PCP = classrule->action.qos_action.remark_stagpcp;
	pcerule->action.bRemarkSTAG_DEI = classrule->action.qos_action.remark_stagdei;
	pcerule->action.bRemarkDSCP = classrule->action.qos_action.remark_dscp;
	pcerule->action.bRemarkClass = classrule->action.qos_action.remark_class;
	pcerule->action.bFlowID_Action = classrule->action.qos_action.flowid_enabled;
	pcerule->action.nFlowID = classrule->action.qos_action.flowid;
	/*Mgmt actions*/
	pcerule->action.eIrqAction = classrule->action.mgmt_action.irq; 
	pcerule->action.eTimestampAction = classrule->action.mgmt_action.timestamp;
	/*Rmon Action*/
	pcerule->action.bRMON_Action = classrule->action.rmon_action;
	pcerule->action.nRMON_Id = classrule->action.rmon_id; 
}

void copy_pce_to_class(PPA_CLASS_RULE* classrule, GSW_PCE_rule_t* pcerule)
{
	/*copy the pattern */
	ppa_memcpy(&classrule->pattern,&pcerule->pattern, sizeof(GSW_PCE_pattern_t));
	
	/*copy the actions
	filter actions*/
	classrule->action.filter_action.portfilter = pcerule->action.ePortFilterType_Action;
	classrule->action.filter_action.crossstate = pcerule->action.eCrossStateAction;
	/*vlan actions*/
	classrule->action.vlan_action.fid = pcerule->action.nFId;
	classrule->action.vlan_action.cvlan = pcerule->action.eVLAN_Action;
	classrule->action.vlan_action.vlan_id = pcerule->action.nVLAN_Id;
	classrule->action.vlan_action.svlan = pcerule->action.eSVLAN_Action;
	classrule->action.vlan_action.svlan_id = pcerule->action.nSVLAN_Id;	 
	classrule->action.vlan_action.cross_vlan = pcerule->action.eVLAN_CrossAction;
	classrule->action.vlan_action.cvlan_ignore = pcerule->action.bCVLAN_Ignore_Control;
	/*fwd actions*/
	classrule->action.fwd_action.learning = pcerule->action.eLearningAction;
	classrule->action.fwd_action.port_trunk = pcerule->action.bPortTrunkAction;
	classrule->action.fwd_action.portmap = pcerule->action.ePortMapAction;
	classrule->action.fwd_action.forward_portmap = pcerule->action.nForwardPortMap[0];
	classrule->action.fwd_action.forward_subifid = pcerule->action.nForwardSubIfId;
	classrule->action.fwd_action.routextid_enable = pcerule->action.bRoutExtId_Action;
	classrule->action.fwd_action.routextid = pcerule->action.nRoutExtId;
	classrule->action.fwd_action.rtdestportmaskcmp = pcerule->action.bRtDstPortMaskCmp_Action;
	classrule->action.fwd_action.rtsrcportmaskcmp = pcerule->action.bRtSrcPortMaskCmp_Action;
	classrule->action.fwd_action.rtdstipmaskcmp = pcerule->action.bRtDstIpMaskCmp_Action;
	classrule->action.fwd_action.rtsrcipmaskcmp = pcerule->action.bRtSrcIpMaskCmp_Action;
	classrule->action.fwd_action.rtinneripaskey = pcerule->action.bRtInnerIPasKey_Action;
	classrule->action.fwd_action.rtaccelenable = pcerule->action.bRtAccelEna_Action;
	classrule->action.fwd_action.rtctrlenable = pcerule->action.bRtCtrlEna_Action;
	classrule->action.fwd_action.processpath = pcerule->action.eProcessPath_Action;
	/*QoS actions*/
	classrule->action.qos_action.trafficclass = pcerule->action.eTrafficClassAction;
	classrule->action.qos_action.alt_trafficclass = pcerule->action.nTrafficClassAlternate;
	classrule->action.qos_action.meter = pcerule->action.eMeterAction;
	classrule->action.qos_action.meterid = pcerule->action.nMeterId;
	classrule->action.qos_action.criticalframe = pcerule->action.eCritFrameAction;
	classrule->action.qos_action.remark = pcerule->action.bRemarkAction;
	classrule->action.qos_action.remarkpcp = pcerule->action.bRemarkPCP;
	classrule->action.qos_action.remark_stagpcp = pcerule->action.bRemarkSTAG_PCP;
	classrule->action.qos_action.remark_stagdei = pcerule->action.bRemarkSTAG_DEI;
	classrule->action.qos_action.remark_dscp = pcerule->action.bRemarkDSCP;
	classrule->action.qos_action.remark_class = pcerule->action.bRemarkClass;
	classrule->action.qos_action.flowid_enabled = pcerule->action.bFlowID_Action;
	classrule->action.qos_action.flowid = pcerule->action.nFlowID;
	/*Mgmt actions*/
	classrule->action.mgmt_action.irq = pcerule->action.eIrqAction; 
	classrule->action.mgmt_action.timestamp = pcerule->action.eTimestampAction;
	/*Rmon Action*/
	classrule->action.rmon_action = pcerule->action.bRMON_Action;
	classrule->action.rmon_id = pcerule->action.nRMON_Id; 
} 

static uint16_t find_ordr_from_uid(ppa_class_devingress_t ing_dev, ppa_class_category_t cat, uint16_t uid)
{
	uint16_t ordr=0xFFFF, i;
		
	/*printk(KERN_INFO" %s %s %d ing_dev=%u cat=%u cat_last_ordr=%d\n", __FILE__, __FUNCTION__, __LINE__, ing_dev, cat, class_dev[ing_dev].cat_map[cat].cat_last_ordr);*/
	for(i=0; i<=class_dev[ing_dev].cat_map[cat].cat_last_ordr; i++) {
		if(class_dev[ing_dev].cat_map[cat].cat_idx_vect[i].uid == uid) {
			ordr = i;
			break;
		}	
	} 
	return ordr;
} 

uint16_t find_free_index(ppa_class_devingress_t ing_dev, ppa_class_category_t cat)
{
	uint16_t index = 0xFFFF;

	/*printk(KERN_INFO" class_dev[%d].cat_map[%d].cat_start_idx=%d\n", ing_dev, cat, class_dev[ing_dev].cat_map[cat].cat_start_idx);*/

	for(index = class_dev[ing_dev].cat_map[cat].cat_start_idx; index < class_dev[ing_dev].tot_max; index++) {
		if(!bitmap_isset(ing_dev, index)) {
			/*set the occupancy bitmap*/
			/*printk(KERN_INFO" %s %s %d index=%u \n", __FILE__, __FUNCTION__, __LINE__, index);*/
			if(set_bitmap(ing_dev, index) == PPA_SUCCESS) {
				break;
			}
		}	
	}
	return index;
}

int32_t	swap_pce_rules(ppa_class_devingress_t ing_dev, ppa_class_category_t cat, uint16_t ordr, uint16_t* index)
{
	int32_t ret = PPA_SUCCESS;
	uint16_t t_idx=0xFFFF;
	GSW_PCE_rule_t pcerule;
	GSW_API_HANDLE gsw_handle=0;
	
	if(open_switch_dev(ing_dev,&gsw_handle)!=PPA_SUCCESS)
		return PPA_FAILURE;

	/*printk(KERN_INFO" %s %s %d ordr=%d index=%d \n", __FILE__, __FUNCTION__, __LINE__, ordr, *index);*/
	ppa_memset(&pcerule,0,sizeof(GSW_PCE_rule_t));

	if(ordr < class_dev[ing_dev].cat_map[cat].cat_max) {
		t_idx = class_dev[ing_dev].cat_map[cat].cat_idx_vect[ordr].index;
		/*printk(KERN_INFO" %s %s %d t_idx=%d\n", __FILE__, __FUNCTION__, __LINE__, t_idx);*/
		pce_rule_read(&gsw_handle, &pcerule, t_idx);
		/*printk(KERN_INFO" %s %s %d calling pce_rule_write\n", __FILE__, __FUNCTION__, __LINE__);*/
		if(pcerule.action.nFId){
			pcerule.action.eVLAN_Action = 2; //VLAN_ALTERNATIVE 
		}
		ret=pce_rule_write(&gsw_handle, &pcerule, *index);
		class_dev[ing_dev].cat_map[cat].cat_idx_vect[ordr].index=*index;
		class_dev[ing_dev].cat_ordr_vect[*index] = class_dev[ing_dev].cat_ordr_vect[t_idx];
		class_dev[ing_dev].cat_ordr_vect[t_idx].usage_flg=0;
		*index = t_idx;
	}

	gsw_api_kclose(gsw_handle);
	return ret;
}

int32_t update_class_tables( PPA_CLASS_RULE* rule, uint16_t index)
{
	int32_t ret=PPA_SUCCESS;
	GSW_PCE_rule_t pcerule;
	GSW_API_HANDLE gsw_handle=0;
	
	if(open_switch_dev(rule->in_dev,&gsw_handle)!=PPA_SUCCESS)
	return PPA_FAILURE;
 
	ppa_memset(&pcerule,0,sizeof(GSW_PCE_rule_t));
	/* copy values to the switch datastructure */
	copy_class_to_pce(&pcerule, rule);

	/*printk(KERN_INFO"before writing the rule \n"); */
	/*show_pce_rule(&pcerule);*/

	ret=pce_rule_write(&gsw_handle, &pcerule, index);
	class_dev[rule->in_dev].cat_map[rule->category].cat_idx_vect[rule->order-1].index = index;
	class_dev[rule->in_dev].cat_map[rule->category].cat_idx_vect[rule->order-1].uid = g_pce_rtrule_next++; 
	
	class_dev[rule->in_dev].cat_map[rule->category].cat_used++; 
	class_dev[rule->in_dev].subcat_map[rule->subcategory].subcat_used++; 
	class_dev[rule->in_dev].tot_used++;

	class_dev[rule->in_dev].cat_ordr_vect[index].cat_id = rule->category; 
	class_dev[rule->in_dev].cat_ordr_vect[index].subcat_id = rule->subcategory; 
	class_dev[rule->in_dev].cat_ordr_vect[index].cat_ordr = rule->order-1;
	class_dev[rule->in_dev].cat_ordr_vect[index].usage_flg = 1; 

	/* return the unique index */
	rule->uidx = class_dev[rule->in_dev].cat_map[rule->category].cat_idx_vect[rule->order-1].uid;
	rule->pattern.nIndex = pcerule.pattern.nIndex;
	
	/*printk(KERN_INFO" %s %s %d pce rule added rule->order=%d index=%d, uid=%d tot_used=%d\n",
		__FILE__, __FUNCTION__, __LINE__, rule->order, index, rule->uidx, class_dev[rule->in_dev].tot_used);*/

	gsw_api_kclose(gsw_handle);
	return ret;
}

int32_t shift_cat_idx_vect(ppa_class_devingress_t ing_dev, ppa_class_category_t cat, uint16_t ordr, shift_direction_t dir)
{
	int i;
	if(dir == SHIFT_RIGHT) {
		class_dev[ing_dev].cat_map[cat].cat_last_ordr++;	
		for(i=class_dev[ing_dev].cat_map[cat].cat_last_ordr; i>ordr; i--) {
			class_dev[ing_dev].cat_map[cat].cat_idx_vect[i]=class_dev[ing_dev].cat_map[cat].cat_idx_vect[i-1];
		}
		class_dev[ing_dev].cat_map[cat].cat_idx_vect[ordr].index = 0xFFFF;
		class_dev[ing_dev].cat_map[cat].cat_idx_vect[ordr].uid = 0;
	} else {
		if(ordr < class_dev[ing_dev].cat_map[cat].cat_last_ordr) {
			for(i=ordr; i<class_dev[ing_dev].cat_map[cat].cat_last_ordr; i++) {
				class_dev[ing_dev].cat_map[cat].cat_idx_vect[i]=class_dev[ing_dev].cat_map[cat].cat_idx_vect[i+1];
			}
		}
		class_dev[ing_dev].cat_map[cat].cat_idx_vect[class_dev[ing_dev].cat_map[cat].cat_last_ordr].index = 0xFFFF;
		class_dev[ing_dev].cat_map[cat].cat_idx_vect[class_dev[ing_dev].cat_map[cat].cat_last_ordr].uid = 0;
		class_dev[ing_dev].cat_map[cat].cat_last_ordr--;	
	}	
	/*printk(KERN_INFO" %s %s %d .cat_last_ordr=%d\n", __FILE__, __FUNCTION__, __LINE__, class_dev[ing_dev].cat_map[cat].cat_last_ordr);*/
	return PPA_SUCCESS;
}

/* pae_hal_meter_cfg_set function add, delete and configures meter with
*  given configs and add and delete pce rule with the meter id and
*  port ID.
*/
int32_t pae_hal_meter_cfg_set(PPA_QOS_METER_CFG *meter_cfg)
{
	int32_t ret = PPA_SUCCESS;
	GSW_QoS_meterCfg_t param;
	PPA_CLASS_RULE rule;
	int32_t setBit = 0;

	ppa_memset(&rule, 0, sizeof(PPA_CLASS_RULE));

	rule.in_dev = GSWR_INGRESS;
	rule.category = CAT_DSQOS;

	rule.pattern.bEnable = 1;
	rule.pattern.bPortIdEnable = 1;
	rule.pattern.nPortId = meter_cfg->port_id;

	param.nMeterId = meter_cfg->phys_meterid;
	if (meter_cfg->flags & PPA_QOS_OP_F_ADD) {
		ret = find_first_zero_bit(&qos_meter, METER_ID_MAX);
		if ((ret + METER_ID_RESERVE) >= METER_ID_MAX) {
			dbg("Error meter bits exhausted!!! %d\n", ret);
			return PPA_FAILURE;
		}
		param.nMeterId = ret + METER_ID_RESERVE;
		dbg("New Meter ID is %d\n", param.nMeterId);
		set_bit(ret, &qos_meter);
	}
	dbg(" pae_hal_meter_cfg_set Meter ID is %d PortID = %d\n", param.nMeterId, meter_cfg->port_id);

	param.bEnable = meter_cfg->enable;
	param.eMtrType =  meter_cfg->type;
	param.nCbs = meter_cfg->cbs;
	param.nEbs = meter_cfg->pbs;
	param.nRate = meter_cfg->pir;

	ret = ltq_try_gswapi_kioctl(GSW_QOS_METER_CFG_SET,
			(unsigned int)&param);
	if (ret < GSW_statusOk) {
		dbg("Meter config set Failed\n");
		return ret;
	}

	if (meter_cfg->flags & PPA_QOS_OP_F_MODIFY)
		return PPA_SUCCESS;

	rule.action.qos_action.meterid = param.nMeterId;
	rule.action.qos_action.meter = 2;
	if (meter_cfg->flags & PPA_QOS_OP_F_DELETE) {
		setBit = param.nMeterId - METER_ID_RESERVE;
		clear_bit(setBit, &qos_meter);
		dbg("Deleted Meter ID is %d and bit = %d\n", param.nMeterId, setBit);
		rule.uidx = meter_cfg->pce_id;
		if (pae_hal_del_class_rule(&rule) != PPA_SUCCESS) {
			dbg("Meter del pce rule returned failure\n");
			return PPA_FAILURE;
		}
	} else {
		if (pae_hal_add_class_rule(&rule) != PPA_SUCCESS) {
			dbg("Meter add pce rule returned failure\n");
			return PPA_FAILURE;
		}
	}
	meter_cfg->phys_meterid = param.nMeterId;
	meter_cfg->pce_id = rule.uidx;
	dbg("Meter pce rule UIDX IS %d\n", rule.uidx);

	return ret;
}

int32_t pae_hal_add_class_rule(PPA_CLASS_RULE* rule)
{
	int32_t ret= PPA_SUCCESS;
	uint16_t idx=0xFFFF, ordr=0;
/*
	printk(KERN_INFO" %s %s %d class_dev=%u\n", __FILE__, __FUNCTION__, __LINE__,class_dev);
	printk(KERN_INFO"&class_dev[GSWL_INGRESS] = %u\n", &class_dev[GSWL_INGRESS]);
	printk(KERN_INFO"class_dev[GSWL_INGRESS].tot_max = %d, class_dev[GSWL_INGRESS].tot_used = %d\n",
		class_dev[GSWL_INGRESS].tot_max, class_dev[GSWL_INGRESS].tot_used);
	printk(KERN_INFO"&class_dev[GSWL_INGRESS].tot_max = %u, &class_dev[GSWL_INGRESS].tot_used = %u\n",
		&class_dev[GSWL_INGRESS].tot_max, &class_dev[GSWL_INGRESS].tot_used);
	
	printk(KERN_INFO"&class_dev[GSWR_INGRESS] = %u\n", &class_dev[GSWR_INGRESS]);
	printk(KERN_INFO"class_dev[GSWR_INGRESS].tot_max = %d, class_dev[GSWR_INGRESS].tot_used = %d\n",
		class_dev[GSWR_INGRESS].tot_max, class_dev[GSWR_INGRESS].tot_used);
	printk(KERN_INFO"&class_dev[GSWR_INGRESS].tot_max = %u, &class_dev[GSWR_INGRESS].tot_used = %u\n",
		&class_dev[GSWR_INGRESS].tot_max, &class_dev[GSWR_INGRESS].tot_used);
*/
 
	//validate the inputs
	if (rule->in_dev > GSWL_INGRESS || rule->category >= CAT_MAX || rule->subcategory >= SUBCAT_MAX ) {
		return PPA_FAILURE;
	}

	spin_lock_bh(&g_class_lock );

	if(class_dev[rule->in_dev].tot_used < class_dev[rule->in_dev].tot_max) {
		if( (rule->category < CAT_MAX) && class_dev[rule->in_dev].cat_map[rule->category].cat_used < class_dev[rule->in_dev].cat_map[rule->category].cat_max) {
			if( (rule->subcategory < SUBCAT_MAX) && 
				class_dev[rule->in_dev].subcat_map[rule->subcategory].subcat_used < class_dev[rule->in_dev].subcat_map[rule->subcategory].subcat_max) {

				/*if rule->order=0 then we assign the cat_last_ordr + 1*/
				if(!rule->order) {
					rule->order = class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr + 2; 
				}

				if(rule->order-1 < class_dev[rule->in_dev].cat_map[rule->category].cat_max) {
			
				/*if order passed is far beyond the cat_last_order reset the order to cat_last_ordr + 1*/
				if((class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr + 1) < (rule->order - 1)) {
					rule->order = class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr + 2;
				}
				
				/*find a free 'idx'
				//printk(KERN_INFO" %s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
				//printk(KERN_INFO" rule->in_dev = %d\n rule->category= %d\n rule->subcategory = %d\n rule->order=%d\n", 
				//		rule->in_dev, rule->category, rule->subcategory, rule->order);*/
			
				/*printk(KERN_INFO"class_dev[%d].tot_max = %d, class_dev[%d].tot_used = %d\n", 
				//		rule->in_dev, class_dev[rule->in_dev].tot_max, rule->in_dev, class_dev[rule->in_dev].tot_used);*/

				idx = find_free_index(rule->in_dev, rule->category);

				/*printk(KERN_INFO"index allocated	= %d\n", idx);*/

				if( idx < class_dev[rule->in_dev].tot_max) { 		
			
					/*find 'ordr' where cat_idx_vect[ordr] < idx*/
					for(ordr=0; (ordr <= class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr) && 
						(class_dev[rule->in_dev].cat_map[rule->category].cat_idx_vect[ordr].index < idx); ordr ++);

			
					/*printk(KERN_INFO" %s %s %d ordr=%d cat_last_ordr=%d idx=%d order=%d\n", __FILE__, __FUNCTION__,
					__LINE__, ordr, class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr, idx, rule->order);*/
			
					/*while ordr < rule->order-1 swap*/
					while(ordr < rule->order-1) {
						if(swap_pce_rules(rule->in_dev, rule->category, ordr, &idx)!=PPA_SUCCESS) {
							dbg("PCE rule swap failed in dev %d index=%d...\n", rule->in_dev, idx);
							clear_bitmap(rule->in_dev,idx);
							ret = PPA_FAILURE;
							goto ADD_FINISH;
						}
						ordr++;
					}
		
					/*while ordr > rule->order-1 swap*/
					while(ordr > rule->order-1) {
						ordr--;
						if(swap_pce_rules(rule->in_dev, rule->category, ordr, &idx)!=PPA_SUCCESS) {
							dbg("PCE rule swap failed in dev %d index=%d...\n", rule->in_dev, idx);
							clear_bitmap(rule->in_dev,idx);
							ret = PPA_FAILURE;
							goto ADD_FINISH;
						}
					}			

					/*printk(KERN_INFO" %s %s %d ordr=%d\n", __FILE__, __FUNCTION__, __LINE__, ordr);*/
					/*now ordr == order -1 */
					if( ordr > class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr) {
						class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr = ordr;
						/*printk(KERN_INFO" %s %s %d .cat_last_ordr=%d\n", __FILE__, __FUNCTION__, __LINE__,
							class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr);*/
					} else {
						ret = shift_cat_idx_vect(rule->in_dev, rule->category, ordr, SHIFT_RIGHT);
					}
					/*Write the new rule in idx and update the tables*/
					ret = update_class_tables(rule, idx);
				} else {
					dbg("No free index found ... \n");
					ret = PPA_FAILURE;
				}
				} else {
					dbg("Order cannot be more than category max %d...\n", class_dev[rule->in_dev].cat_map[rule->category].cat_max);
					ret = PPA_FAILURE;
				}
			} else {
				dbg("MAX limit %d reached in for subcategory %d...\n", class_dev[rule->in_dev].subcat_map[rule->subcategory].subcat_max, rule->subcategory);
				ret = PPA_FAILURE;
			} 
		} else {
			dbg("MAX limit %d reached in for category %d...\n", class_dev[rule->in_dev].cat_map[rule->category].cat_max, rule->category);
			ret = PPA_FAILURE;
		}
	} else {
		dbg("MAX PCE rule limit reached in dev %d...\n", rule->in_dev);
		ret = PPA_FAILURE;
	}	
ADD_FINISH:
	spin_unlock_bh(&g_class_lock );
	return ret;
}
EXPORT_SYMBOL(pae_hal_add_class_rule);

int32_t pae_hal_del_class_rule(PPA_CLASS_RULE* rule)
{
	int32_t ret= PPA_SUCCESS;
	uint16_t idx=0xFFFF, ordr=0;
	GSW_PCE_rule_t pcerule;
	GSW_API_HANDLE gsw_handle=0;
	
	if (open_switch_dev(rule->in_dev,&gsw_handle)!=PPA_SUCCESS || rule->subcategory >= SUBCAT_MAX || rule->category >= CAT_MAX)
		return PPA_FAILURE;

	ppa_memset(&pcerule,0,sizeof(GSW_PCE_rule_t));

	spin_lock_bh(&g_class_lock );
	/*if the caller has passed the order to delete.. it is assumed that the caller has maintained the relative order properly*/
	if(rule->order) {
		ordr = rule->order-1;
	} else {
		ordr = find_ordr_from_uid(rule->in_dev, rule->category, rule->uidx);
	}

	/*printk(KERN_INFO" %s %s %d calling pce_rule_delete ordr=%d\n", __FILE__, __FUNCTION__, __LINE__,ordr);*/

	if( ordr <= class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr) {
		idx = class_dev[rule->in_dev].cat_map[rule->category].cat_idx_vect[ordr].index;
		if(class_dev[rule->in_dev].cat_ordr_vect[idx].usage_flg) {
			/*disable the rule at idx
			printk(KERN_INFO" %s %s %d calling pce_rule_write at idx=%d\n", __FILE__, __FUNCTION__, __LINE__,idx);*/
			ret = pce_rule_write(&gsw_handle, &pcerule, idx); 
			class_dev[rule->in_dev].cat_ordr_vect[idx].usage_flg=0;
			clear_bitmap(rule->in_dev,idx);		
		
			ret = shift_cat_idx_vect(rule->in_dev, rule->category, ordr, SHIFT_LEFT);
	
			class_dev[rule->in_dev].cat_map[rule->category].cat_used--; 
			class_dev[rule->in_dev].subcat_map[rule->subcategory].subcat_used--; 
			class_dev[rule->in_dev].tot_used--;
		} else {
			dbg("Device=%d Category=%d Order=%d PCE table Index=%d is empty...\n",rule->in_dev, rule->category, rule->order, idx);
			ret = PPA_FAILURE;
		}	
	} else {
		dbg("Invalid order %d for categoty %d...\n", rule->order, rule->category);
		ret = PPA_FAILURE;
	}
	spin_unlock_bh(&g_class_lock );
	gsw_api_kclose(gsw_handle);
	return ret;
}
EXPORT_SYMBOL(pae_hal_del_class_rule);

int32_t pae_hal_mod_class_rule(PPA_CLASS_RULE* rule)
{
	int32_t ret= PPA_SUCCESS;
	uint16_t idx=0xFFFF, ordr=0;
	GSW_PCE_rule_t pcerule;
	GSW_API_HANDLE gsw_handle=0;
 
	if (open_switch_dev(rule->in_dev,&gsw_handle)!=PPA_SUCCESS || rule->category >= CAT_MAX)
		return PPA_FAILURE;

	ppa_memset(&pcerule,0,sizeof(GSW_PCE_rule_t));

	spin_lock_bh(&g_class_lock );
/*if the caller has passed the order to delete.. it is assumed that the caller has maintained the relative order properly*/
	if(rule->order) {
		ordr = rule->order-1;
	} else {
		ordr = find_ordr_from_uid(rule->in_dev, rule->category, rule->uidx);
	}

	if( ordr <= class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr) {
		idx = class_dev[rule->in_dev].cat_map[rule->category].cat_idx_vect[ordr].index;
		if(class_dev[rule->in_dev].cat_ordr_vect[idx].usage_flg) {
			pce_rule_read(&gsw_handle, &pcerule, idx); 
			/*modify the pce rule*/
			copy_class_to_pce(&pcerule, rule);
			/*printk(KERN_INFO" %s %s %d calling pce_rule_write\n", __FILE__, __FUNCTION__, __LINE__);*/
			ret=pce_rule_write(&gsw_handle, &pcerule, idx); 
		} else {
			dbg("Device=%d Category=%d Order=%d PCE table Index=%d is empty...\n",rule->in_dev, rule->category, rule->order, idx);
			ret = PPA_FAILURE;
		}	
	} else {
		dbg("Invalid order %d for categoty %d...\n", rule->order, rule->category);
		ret = PPA_FAILURE;
	}
	spin_unlock_bh(&g_class_lock );
	gsw_api_kclose(gsw_handle);
	return ret;
}
EXPORT_SYMBOL(pae_hal_mod_class_rule);

int32_t pae_hal_get_class_rule(PPA_CLASS_RULE* rule)
{
	int32_t ret= PPA_SUCCESS;
	uint16_t idx=0xFFFF, ordr=0;
	GSW_PCE_rule_t pcerule;
	GSW_API_HANDLE gsw_handle=0;
	
	if (open_switch_dev(rule->in_dev,&gsw_handle)!=PPA_SUCCESS || rule->category >= CAT_MAX)
		return PPA_FAILURE;
	
	ppa_memset(&pcerule,0,sizeof(GSW_PCE_rule_t));

	spin_lock_bh(&g_class_lock );
	/*if the caller has passed the order to delete.. it is assumed that the caller has maintained the relative order properly*/
	if(rule->order) {
		ordr = rule->order-1;
	} else {
		ordr = find_ordr_from_uid(rule->in_dev, rule->category, rule->uidx);
	}

	if( ordr <= class_dev[rule->in_dev].cat_map[rule->category].cat_last_ordr) {
		idx = class_dev[rule->in_dev].cat_map[rule->category].cat_idx_vect[ordr].index;
		if(class_dev[rule->in_dev].cat_ordr_vect[idx].usage_flg) {
			pce_rule_read(&gsw_handle, &pcerule, idx);
			copy_pce_to_class(rule, &pcerule); 
		} else {
			dbg("Device=%d Category=%d Order=%d PCE table Index=%d is empty...\n",rule->in_dev, rule->category, rule->order, idx);
			ret = PPA_FAILURE;
		}	
	} else {
		dbg("Invalid order %d for categoty %d...\n", rule->order, rule->category);
		ret = PPA_FAILURE;
	}
	/*show_pce_rule(&pcerule);	*/

	spin_unlock_bh(&g_class_lock );
	gsw_api_kclose(gsw_handle);
	return ret;
}
EXPORT_SYMBOL(pae_hal_get_class_rule);

static inline uint32_t ppa_drv_get_phys_port_num(void)
{
	return 16;
}

static inline uint32_t set_wanitf(PPA_WANITF_CFG *wanitf_cfg, uint32_t flag)
{
	if( !wanitf_cfg ) return PPA_FAILURE;

	return PPA_SUCCESS;
}

void ltq_pae_set_mac_violation_ports(uint32_t port)
{
	GSW_register_t dataCfg = {0};
	int ibase = REG_PCE_CTRL3_BASE;

	dataCfg.nRegAddr = ibase + (10*(port-1));

	ltq_try_gswapi_kioctl( GSW_REGISTER_GET, (unsigned int)&dataCfg);

	dataCfg.nData |= (1 << 10); /*set the 10th bit	of PCE_CTRL_3 */
	dataCfg.nData |= (1 << 7); /* PCE_PCTRL_3 Bit VIO_7 (bit 7) = 1 */

	ltq_try_gswapi_kioctl( GSW_REGISTER_SET, (unsigned int)&dataCfg);
}
EXPORT_SYMBOL(ltq_pae_set_mac_violation_ports);

void ltq_pae_reset_mac_violation_ports(uint32_t port)
{
	GSW_register_t dataCfg = {0};
	int ibase = REG_PCE_CTRL3_BASE;

	dataCfg.nRegAddr = ibase + (10*(port-1));

	ltq_try_gswapi_kioctl( GSW_REGISTER_GET, (unsigned int)&dataCfg);

	dataCfg.nData &= ~(1 << 10); /*set the 10th bit of PCE_CTRL_3	*/
	dataCfg.nData &= ~(1 << 7); /* PCE_PCTRL_3 Bit VIO_7 (bit 7) = 1 */

	ltq_try_gswapi_kioctl( GSW_REGISTER_SET, (unsigned int)&dataCfg);
}
EXPORT_SYMBOL(ltq_pae_reset_mac_violation_ports);

void ltq_pae_enable_monitor_port(uint32_t port)
{
	GSW_monitorPortCfg_t monPortCfg={0};
	monPortCfg.nPortId = port;   /*CPU Port for mirroring.*/
	monPortCfg.bMonitorPort = 1;  /* Enable Mirroring.*/

	ltq_try_gswapi_kioctl ( GSW_MONITOR_PORT_CFG_SET, (unsigned int)&monPortCfg); 

}
EXPORT_SYMBOL(ltq_pae_enable_monitor_port);

void ltq_pae_disable_monitor_port(uint32_t port)
{
	GSW_monitorPortCfg_t monPortCfg={0};
	monPortCfg.nPortId = port;   /*CPU Port for mirroring.*/
	monPortCfg.bMonitorPort = 0;  // Enable Mirroring.

	ltq_try_gswapi_kioctl ( GSW_MONITOR_PORT_CFG_SET, (unsigned int)&monPortCfg); 

}
EXPORT_SYMBOL(ltq_pae_disable_monitor_port);

/* PAE is supposed to have learning disabled on all the ports
// we need to forward all the packets with unknown source mac to CPU to manage the learning of MAC address in PAE
// but PAE cannot forward packet to CPU based on source MAC
// The below function is a workaround until the Hardware bug is fixed
// This will have a side effect that port monitoring cannot be enabled in PAE for any other purpose
*/
static void init_pae_ports(void)
{
	GSW_portCfg_t portCfg={0};
	GSW_portCfg_t portCfgGswl={0};
	int i;
	int ibase=0x48D; /*PCE_PCTRL3 register address for port 1*/
#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING) 
	GSW_register_t dataCfg={0};
#endif /*IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING) */
	GSW_RMON_mode_t rmonMode={0};

	dbg("Enabling managed switch in PAE...\n");
	for (i = 1; i < MAX_PAE_PORTS; i++, ibase += 10) {
		portCfg.nPortId = i;
		ltq_try_gswapi_kioctl(GSW_PORT_CFG_GET, (unsigned int)&portCfg);

		portCfg.bLearning = 0;	/* 0 = learning enabled */
		portCfg.nLearningLimit = 0; /* learning limit	= 0 */
		/*MAC learning port lock enabled to avoid overwriting of learned MAC*/
		portCfg.bLearningMAC_PortLock = 1;

		ltq_try_gswapi_kioctl(GSW_PORT_CFG_SET, (unsigned int)&portCfg);

#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING)
		dataCfg.nRegAddr = ibase;
		ltq_try_gswapi_kioctl(GSW_REGISTER_GET, (unsigned int)&dataCfg);

		dataCfg.nData |= ( 1 << 13); /*Set the 13th bit	of PCE_CTRL_3 "New MAC-Port Association CPU Port Forwarding Enable"*/
		ltq_try_gswapi_kioctl(GSW_REGISTER_SET, (unsigned int)&dataCfg);
#endif /*IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING) */
	}
	/* clear the mac table*/
	ltq_try_gswapi_kioctl( GSW_MAC_TABLE_CLEAR, 0);

#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING) 
	ppa_memset(&dataCfg,0x00, sizeof(GSW_register_t));
	dataCfg.nRegAddr = 0x456; /*PCE_GCTRL_0*/
	ltq_try_gswapi_kioctl( GSW_REGISTER_GET, (unsigned int)&dataCfg);

	dataCfg.nData |= ( 1 << 2); /* Enable forward packets with port lock violation (global): PCE_GCTRL_0 Bit PLCKMOD (bit 2) = 1*/
	ltq_try_gswapi_kioctl( GSW_REGISTER_SET, (unsigned int)&dataCfg);
#endif /*IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING) */

	/*setting the rmon route counters to byte counters*/
	rmonMode.eRmonType = GSW_RMON_ROUTE_TYPE;
	rmonMode.eCountMode = GSW_RMON_COUNT_BYTES;

	ltq_try_gswapi_kioctl( GSW_RMON_MODE_SET, (unsigned int)&rmonMode);
	
	/*Disable Mutlicast,Broadcast, unknown unicast flood on gsw-l port 2,3,4,5 , and forward to cpu port   */
	for (i = 2; i < MAX_GSWL_PORTS; i++) {
		portCfgGswl.nPortId = i;
		gswl_api_kioctl( GSW_PORT_CFG_GET, (unsigned int)&portCfgGswl);

		portCfgGswl.bUnicastUnknownDrop = 1;	/* UnicastUnknownDrop = 1 */
		portCfgGswl.bMulticastUnknownDrop = 1; /* MulticastUnknownDrop	= 1 */
		portCfgGswl.bBroadcastDrop = 1; /* BroadcastDrop	= 1 */
		gswl_api_kioctl( GSW_PORT_CFG_SET, (unsigned int)&portCfgGswl);
	}
}

static void uninit_pae_ports(void)
{
	GSW_portCfg_t portCfg={0};
	int i;

	dbg("Disabling managed switch in PAE...\n");
	for (i = 1; i < MAX_PAE_PORTS; i++) {
		portCfg.nPortId = i;
		ltq_try_gswapi_kioctl( GSW_PORT_CFG_GET, (unsigned int)&portCfg);

		portCfg.bLearning = 0;	/*0 = learning enabled 1 = disabled*/
		portCfg.nLearningLimit = 255;
		portCfg.ePortMonitor &= ~GSW_PORT_MONITOR_LEARNING_LIMIT; 

		ltq_try_gswapi_kioctl( GSW_PORT_CFG_SET, (unsigned int)&portCfg);
	}
}
/* All the capabilities currently supported	are hardcoded 
// register all the capabilities supported by PAE HAL*/
static int32_t pae_hal_register_caps(void)
{
	int32_t res = PPA_SUCCESS;	

	if((res = ppa_drv_register_cap(SESS_BRIDG, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability SESS_BRIDG!!!\n");	
		goto PAE_HAL_FAIL;
	}	
	
	if((res = ppa_drv_register_cap(SESS_IPV4, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability SESS_IPV4!!!\n");	
		ppa_drv_deregister_cap(SESS_BRIDG,PAE_HAL);
		goto PAE_HAL_FAIL;
	}	
	
	if((res = ppa_drv_register_cap(SESS_IPV6, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability SESS_IPV6!!!\n");	
		ppa_drv_deregister_cap(SESS_BRIDG,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV4,PAE_HAL);
		goto PAE_HAL_FAIL;
	}	
	
	if((res = ppa_drv_register_cap(SESS_MC_DS, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability SESS_MC_DS!!!\n");	
		ppa_drv_deregister_cap(SESS_BRIDG,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV4,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV6,PAE_HAL);
		goto PAE_HAL_FAIL;
	}	
			
	if((res = ppa_drv_register_cap(TUNNEL_6RD, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_6RD!!!\n");	
		ppa_drv_deregister_cap(SESS_BRIDG,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV4,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV6,PAE_HAL);
		ppa_drv_deregister_cap(SESS_MC_DS,PAE_HAL);
		goto PAE_HAL_FAIL;
	}	
	
	if((res = ppa_drv_register_cap(TUNNEL_DSLITE, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_DSLITE!!!\n");	
		ppa_drv_deregister_cap(SESS_BRIDG,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV4,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV6,PAE_HAL);
		ppa_drv_deregister_cap(SESS_MC_DS,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_6RD,PAE_HAL);
		goto PAE_HAL_FAIL;
	}	
		
	if((res = ppa_drv_register_cap(TUNNEL_L2TP_DS, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_L2TP_DS!!!\n");	
		ppa_drv_deregister_cap(SESS_BRIDG,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV4,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV6,PAE_HAL);
		ppa_drv_deregister_cap(SESS_MC_DS,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_6RD,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_DSLITE,PAE_HAL);
		goto PAE_HAL_FAIL;
	}	
	
	if((res = ppa_drv_register_cap(QOS_CLASSIFY, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability QOS_CLASSIFY!!!\n");	
		ppa_drv_deregister_cap(SESS_BRIDG,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV4,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV6,PAE_HAL);
		ppa_drv_deregister_cap(SESS_MC_DS,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_6RD,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_DSLITE,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_L2TP_DS,PAE_HAL);
	}	

	if((res = ppa_drv_register_cap(TUNNEL_GRE_DS, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_GRE_DS!!\n");	
		ppa_drv_deregister_cap(SESS_BRIDG,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV4,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV6,PAE_HAL);
		ppa_drv_deregister_cap(SESS_MC_DS,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_6RD,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_DSLITE,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_L2TP_DS,PAE_HAL);
		ppa_drv_deregister_cap(QOS_CLASSIFY, PAE_HAL);
	}	

	if((res = ppa_drv_register_cap(LRO_ENG, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability TUNNEL_GRE_DS!!\n");	
		ppa_drv_deregister_cap(SESS_BRIDG,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV4,PAE_HAL);
		ppa_drv_deregister_cap(SESS_IPV6,PAE_HAL);
		ppa_drv_deregister_cap(SESS_MC_DS,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_6RD,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_DSLITE,PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_L2TP_DS,PAE_HAL);
		ppa_drv_deregister_cap(QOS_CLASSIFY, PAE_HAL);
		ppa_drv_deregister_cap(TUNNEL_GRE_DS,PAE_HAL);
	}
	if((res = ppa_drv_register_cap(WMM_QOS_DSCP_MAP, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability WMM_QOS_DSCP_MAP!!\n");
	}
	if ((res = ppa_drv_register_cap(PORT_METER, 1, PAE_HAL)) != PPA_SUCCESS) {
		dbg("ppa_drv_register_cap returned failure for capability PORT_METER\n");
	}
PAE_HAL_FAIL:
	return res;
}


static int32_t pae_hal_deregister_caps(void)
{
	ppa_drv_deregister_cap(SESS_BRIDG,PAE_HAL);
	ppa_drv_deregister_cap(SESS_IPV4,PAE_HAL);
	ppa_drv_deregister_cap(SESS_IPV6,PAE_HAL);
	ppa_drv_deregister_cap(SESS_MC_DS,PAE_HAL);
	ppa_drv_deregister_cap(TUNNEL_6RD,PAE_HAL);
	ppa_drv_deregister_cap(TUNNEL_DSLITE,PAE_HAL);
	ppa_drv_deregister_cap(TUNNEL_L2TP_DS,PAE_HAL);
	ppa_drv_deregister_cap(QOS_CLASSIFY,PAE_HAL); 
	ppa_drv_deregister_cap(TUNNEL_GRE_DS,PAE_HAL); 
	ppa_drv_deregister_cap(LRO_ENG,PAE_HAL); 
	ppa_drv_deregister_cap(WMM_QOS_DSCP_MAP, PAE_HAL);
	ppa_drv_deregister_cap(PORT_METER, PAE_HAL);
	return PPA_SUCCESS;
}

static int32_t pae_hal_dscp_class_cfg(uint32_t flags)
{
	uint32_t dscp = 0;
	int32_t ret = 0;
	GSW_QoS_DSCP_ClassCfg_t qos_dscp_class_set;
	memset(&qos_dscp_class_set, 0, sizeof(GSW_QoS_DSCP_ClassCfg_t));

	if (flags) {
		/* DSCP Class Set */
		for (dscp = 0; dscp < 64; dscp++) {
#if LINEAR_WMM
			qos_dscp_class_set.nTrafficClass[dscp] =  dscp / 8;
#else
			/* Background */
			if (dscp >= 8 && dscp < 24)
				qos_dscp_class_set.nTrafficClass[dscp] = 4;

			/* Best Effort */
			else if ((dscp >= 0 && dscp < 8) || (dscp >= 24 && dscp < 32))
				qos_dscp_class_set.nTrafficClass[dscp] = 5;

			/* Video */
			else if (dscp >= 32 && dscp < 48)
				qos_dscp_class_set.nTrafficClass[dscp] = 6;

			/* Voice */
			else if (dscp >= 48 && dscp < 64)
				qos_dscp_class_set.nTrafficClass[dscp] = 7;
#endif /* LINEAR_WMM */
		}
	}

	ret = ltq_try_gswapi_kioctl(GSW_QOS_DSCP_CLASS_SET, (unsigned int)&qos_dscp_class_set);
	if (ret < GSW_statusOk)
	{
		printk("GSW_QOS_DSCP_CLASS_SET returned failure : [%d]\n", ret);
	}

	return PPA_SUCCESS;
}

#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
static int32_t pae_hal_mib_mode_cfg(PPA_CMD_MIB_MODE_INFO *mib_info)
{
	GSW_RMON_mode_t rmon_mib_mode_set;
	int32_t ret = 0;

	memset(&rmon_mib_mode_set, 0, sizeof(GSW_RMON_mode_t));

	if (mib_info->mib_mode == 1) {
		/* Packet Mode */
		rmon_mib_mode_set.eRmonType = GSW_RMON_ROUTE_TYPE;
		rmon_mib_mode_set.eCountMode = GSW_RMON_COUNT_PKTS;
		g_mib_mode = mib_info->mib_mode;
	} else if (mib_info->mib_mode == 0) {
		/* Byte Mode */
		rmon_mib_mode_set.eRmonType = GSW_RMON_ROUTE_TYPE;
		rmon_mib_mode_set.eCountMode = GSW_RMON_COUNT_BYTES;
		g_mib_mode = mib_info->mib_mode;
	} else {
		return PPA_FAILURE;
	}

	ret = ltq_try_gswapi_kioctl(GSW_RMON_MODE_SET, (unsigned int)&rmon_mib_mode_set);
	if (ret < GSW_statusOk)
	{
		printk("GSW_RMON_MODE_SET returned failure : [%d]\n", ret);
	}

	return PPA_SUCCESS;
}

static int32_t pae_hal_get_mib_mode(PPA_CMD_MIB_MODE_INFO *mib_info)
{
	mib_info->mib_mode = g_mib_mode;
	return PPA_SUCCESS;
}
#endif

static int32_t pae_hal_generic_hook(PPA_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag)
{
	/*dbg("pae_hal_generic_hook cmd 0x%x_%s\n", cmd, ENUM_STRING(cmd) );*/
	switch (cmd)	{
		case PPA_GENERIC_HAL_GET_PORT_MIB: {
			int i=0;
			int num;
			int ret;
			GSW_RMON_Route_cnt_t pae_port_mib;
			PPA_PORT_MIB *mib = (PPA_PORT_MIB*) buffer;
			num = NUM_ENTITY(mib->mib_info) > ppa_drv_get_phys_port_num() ? ppa_drv_get_phys_port_num():NUM_ENTITY(mib->mib_info) ;
			for(i=0; i<num; i++)
			{
				/*assuming pae is /dev/switch_api/1*/
				ppa_memset(&pae_port_mib, 0, sizeof(GSW_RMON_Route_cnt_t));
				pae_port_mib.nRoutedPortId = i;

				GSWR_OPS(rmon_ops, RMON_Route_Get, &pae_port_mib, ret);
					
				mib->mib_info[i].ig_fast_rt_ipv4_udp_pkts = pae_port_mib.nRxUCv4UDPPktsCount;
				mib->mib_info[i].ig_fast_rt_ipv4_tcp_pkts = pae_port_mib.nRxUCv4TCPPktsCount;
				mib->mib_info[i].ig_fast_rt_ipv4_mc_pkts = pae_port_mib.nRxMCv4PktsCount;
				mib->mib_info[i].ig_fast_rt_ipv4_bytes = pae_port_mib.nRxIPv4BytesCount;
				mib->mib_info[i].ig_fast_rt_ipv6_udp_pkts = pae_port_mib.nRxUCv6UDPPktsCount;
				mib->mib_info[i].ig_fast_rt_ipv6_tcp_pkts = pae_port_mib.nRxUCv6TCPPktsCount;
				mib->mib_info[i].ig_fast_rt_ipv6_mc_pkts = pae_port_mib.nRxMCv6PktsCount;
				mib->mib_info[i].ig_fast_rt_ipv6_bytes = pae_port_mib.nRxIPv6BytesCount;
				mib->mib_info[i].ig_cpu_pkts = pae_port_mib.nRxCpuPktsCount;
				mib->mib_info[i].ig_cpu_bytes = pae_port_mib.nRxCpuBytesCount;
				mib->mib_info[i].ig_drop_pkts = pae_port_mib.nRxPktsDropCount;
				mib->mib_info[i].ig_drop_bytes = pae_port_mib.nRxBytesDropCount;
				mib->mib_info[i].eg_fast_pkts = pae_port_mib.nTxPktsCount;
				mib->mib_info[i].eg_fast_bytes = pae_port_mib.nTxBytesCount;
				if( i>=1 && i<=6	)
					mib->mib_info[i].port_flag = PPA_PORT_MODE_ETH;
				else if( i == 13 )
					mib->mib_info[i].port_flag = PPA_PORT_MODE_DSL;
				else if( i == 0 )	/* 0 is CPU port*/
					mib->mib_info[i].port_flag = PPA_PORT_MODE_CPU;
				else
					mib->mib_info[i].port_flag = PPA_PORT_MODE_EXT;
			}
			mib->port_num = num;
			/*dbg("port_num=%d\n", mib->port_num);*/
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_SET_DEBUG:	{
			pae_hal_dbg_enable = ((PPA_CMD_GENERAL_ENABLE_INFO*)buffer)->enable;
			dbg("Set pae_hal_dbg_enable to 0x%x\n", pae_hal_dbg_enable );
			return PPA_SUCCESS;
		}
	 case PPA_GENERIC_HAL_GET_MAX_ENTRIES: {

			PPA_MAX_ENTRY_INFO *entry=(PPA_MAX_ENTRY_INFO *)buffer;

			entry->max_lan_entries = MAX_ROUTING_ENTRIES/2;
			entry->max_wan_entries = MAX_ROUTING_ENTRIES/2; 
			entry->max_mc_entries = MAX_WAN_MC_ENTRIES;
			entry->max_bridging_entries = MAX_BRIDGING_ENTRIES;
			entry->max_ipv6_addr_entries =	MAX_IP_ENTRIES/4;
			entry->max_pae_routing_entries = MAX_ROUTING_ENTRIES;
			entry->max_tunnel_entries = MAX_TUNNEL_ENTRIES;

			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_GET_HAL_VERSION: {
			PPA_VERSION *v=(PPA_VERSION *)buffer;
			get_pae_hal_id( &v->family, &v->type,&v->itf, &v->mode, &v->major, &v->mid, &v->minor );
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_GET_PPE_FW_VERSION: {
			PPA_VERSION *v=(PPA_VERSION *)buffer;
			get_pae_hal_id( &v->family, &v->type,&v->itf, &v->mode, &v->major, &v->mid, &v->minor );
			return get_firmware_id(&v->id, v->name, v->version);
		}
	case PPA_GENERIC_HAL_GET_PHYS_PORT_NUM: {
			PPA_COUNT_CFG *count=(PPA_COUNT_CFG *)buffer;
			count->num = get_number_of_phys_port();
			return PPA_SUCCESS;
		 }
	case PPA_GENERIC_HAL_GET_PHYS_PORT_INFO: {
			PPE_IFINFO *info = (PPE_IFINFO *) buffer;
			get_phys_port_info(info->port, &info->if_flags, info->ifname);
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_SET_ROUT_CFG: {
			PPA_ROUTING_CFG *cfg=(PPA_ROUTING_CFG *)buffer;
			set_route_cfg(cfg->f_is_lan, cfg->entry_num, cfg->mc_entry_num, 
							cfg->flags, cfg->f_mpe_route, cfg->f_l2tp_ds, cfg->f_mc_vaps);
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_SET_BRDG_CFG: {
			PPA_BRDG_CFG *cfg=(PPA_BRDG_CFG *)buffer;
			set_bridging_cfg( cfg->entry_num, cfg->br_to_src_port_mask,	
								cfg->br_to_src_port_en,
								cfg->f_dest_vlan_en,
								cfg->f_src_vlan_en,
								cfg->f_mac_change_drop,
								cfg->flags);

			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_SET_ACC_ENABLE: {
		/*kamal: enable/disable upstream/downstream acceleration */
			PPA_ACC_ENABLE *cfg=(PPA_ACC_ENABLE *)buffer;
			set_acc_mode( cfg->f_is_lan, cfg->f_enable);

			return PPA_SUCCESS;
		}
	 case PPA_GENERIC_HAL_GET_ACC_ENABLE: {
			PPA_ACC_ENABLE *cfg=(PPA_ACC_ENABLE *)buffer;
			get_acc_mode( cfg->f_is_lan, &cfg->f_enable);

			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_GET_IPV6_FLAG: {
		/*kamal : Always returns enabled*/
			return is_ipv6_enabled();
		}
	case PPA_GENERIC_HAL_UPDATE_SESS_META: {
		/*no special metadate needed for PAE HAL*/
		if (g_us_accel_enabled || g_ds_accel_enabled) {
			PPA_SESSMETA_INFO *metainfo = (PPA_SESSMETA_INFO *)buffer;
			struct netif_info *txif_info;
			struct uc_session_node *p_item;

			if (metainfo) {
				txif_info = (struct netif_info *)metainfo->txif_info;
				p_item = (struct uc_session_node *)metainfo->p_item;

				/* Generally, PAE does not support WiFi/VLAN egress acceleration */
				if (txif_info &&
				    ((txif_info->flags & NETIF_DIRECTCONNECT_WIFI) &&
				     (txif_info->flags & NETIF_VLAN))) {

					if (!(p_item && ppa_is_EgressGreSession(p_item)))
						return PPA_FAILURE;
				/* NOTE: MPE complementary still be allowed, for example EoGRE over WiFi/VLAN */
				}
			}

			return PPA_SUCCESS;
		} else
			return PPA_FAILURE;
		} 
	case PPA_GENERIC_HAL_CLEAR_SESS_META: {
		/*no special metadate needed for PAE HAL*/
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_ADD_ROUTE_ENTRY: {
			PPA_ROUTING_INFO *route=(PPA_ROUTING_INFO *)buffer;
			struct uc_session_node *p_item = (struct uc_session_node *)route->p_item;
			if(!p_item) return PPA_FAILURE;
			/*call the switch api for adding route entry
			checking the global acceleration enable/disable settings*/
			if(is_lansession(p_item->flags)) {
				if(!g_us_accel_enabled) {
					dbg("\n PAE US Acceleration is disabled!!! \n"); 
					return PPA_FAILURE; 
				} 
			} else {
				if(!g_ds_accel_enabled) {
					dbg("\n PAE DS Acceleration is disabled!!! \n"); 
					return PPA_FAILURE;	
				}	
			}
			return add_routing_entry(route);
		}
	case PPA_GENERIC_HAL_DEL_ROUTE_ENTRY: {
			PPA_ROUTING_INFO *route=(PPA_ROUTING_INFO *)buffer;
			int retry_del=0;
			while((del_routing_entry( route )) < 0) {
				retry_del++;
				if(retry_del >=10) { /* retry limit 10 reached*/
					return PPA_FAILURE;	
				}
			}
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_ADD_MC_ENTRY: {
			PPA_MC_INFO *mc = (PPA_MC_INFO *)buffer;
			if (!g_ds_accel_enabled) {
				dbg("\n PAE DS Acceleration is disabled!!! \n");
				return PPA_FAILURE;
			}
			return add_wan_mc_entry(mc);
		}
	case PPA_GENERIC_HAL_DEL_MC_ENTRY: {
			PPA_MC_INFO *mc = (PPA_MC_INFO *)buffer;
			del_wan_mc_entry(mc);
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_UPDATE_MC_ENTRY: {
			PPA_MC_INFO *mc = (PPA_MC_INFO *)buffer;
			return update_wan_mc_entry(mc);
		}
	case PPA_GENERIC_HAL_ADD_BR_MAC_BRIDGING_ENTRY: {
			PPA_BR_MAC_INFO *br_mac = (PPA_BR_MAC_INFO *)buffer;
			return add_bridging_entry(br_mac->port, br_mac->mac, br_mac->fid, br_mac->age_timer, br_mac->static_entry, br_mac->sub_ifid);
		}
	case PPA_GENERIC_HAL_DEL_BR_MAC_BRIDGING_ENTRY: {
			PPA_BR_MAC_INFO *br_mac = (PPA_BR_MAC_INFO *)buffer;
			del_bridging_entry(br_mac->mac, br_mac->fid);

			return PPA_SUCCESS;
		}
	 case PPA_GENERIC_HAL_ADD_BR_PORT: {
			PPA_BR_PORT_INFO *br_info = (PPA_BR_PORT_INFO *)buffer;
			return add_br_port(br_info);	
		}
	 case PPA_GENERIC_HAL_DEL_BR_PORT: {
			PPA_BR_PORT_INFO *br_info = (PPA_BR_PORT_INFO *)buffer;
			return del_br_port(br_info);		
		}
	 case PPA_GENERIC_HAL_GET_ROUTE_ACC_BYTES: {
			PPA_ROUTING_INFO *route=(PPA_ROUTING_INFO *)buffer;
			uint32_t f_hit=0;
			uint32_t cnt=0;

			get_routing_entry_bytes(route->entry, &f_hit, &cnt, GSW_ROUTE_HIT_N_CNTR_CLEAR);
			route->f_hit = f_hit;
			route->bytes = cnt;
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_GET_MC_ACC_BYTES: {
			PPA_MC_INFO *mc =(PPA_MC_INFO *)buffer;
			uint32_t f_hit=0;
			uint32_t cnt=0;

			get_routing_entry_bytes(mc->p_entry, &f_hit, &cnt, GSW_ROUTE_HIT_N_CNTR_CLEAR);
			mc->f_hit = f_hit;
			mc->bytes = cnt;
		
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_SET_MC_RTP: {
			return PPA_SUCCESS;	
		}
	case PPA_GENERIC_HAL_GET_MC_RTP_SAMPLING_CNT: {
			PPA_MC_INFO *mc =(PPA_MC_INFO *)buffer;
			struct mc_session_node *p_item = (struct mc_session_node *)mc->p_item;
			uint32_t seq_num=0;
			uint32_t pkt_cnt=0;
			get_rtp_sampling_cnt(mc->p_entry, &seq_num, &pkt_cnt);
			if(p_item) {
				p_item->rtp_seq_num = seq_num;
				p_item->rtp_pkt_cnt = pkt_cnt;
			}
		}
	case PPA_GENERIC_HAL_ADD_OUT_VLAN_ENTRY: {
			PPA_OUT_VLAN_INFO *vlan=(PPA_OUT_VLAN_INFO *)buffer;
			return add_vlan_entry( vlan);
		}
	case PPA_GENERIC_HAL_DEL_OUT_VLAN_ENTRY: {
				PPA_OUT_VLAN_INFO *vlan=(PPA_OUT_VLAN_INFO *)buffer;
				del_vlan_entry( vlan);
				return PPA_SUCCESS;
			}
	case PPA_GENERIC_HAL_GET_OUT_VLAN_ENTRY: {
				PPA_OUT_VLAN_INFO *vlan=(PPA_OUT_VLAN_INFO *)buffer;
				return get_vlan_entry( vlan);
			}
	case PPA_GENERIC_HAL_ADD_6RD_TUNNEL_ENTRY: {
				PPA_TUNNEL_INFO *tnnl_info = (PPA_TUNNEL_INFO *)buffer;
				return add_tunnel_entry(tnnl_info->tunnel_type, tnnl_info->tunnel_idx);
			}
	case PPA_GENERIC_HAL_DEL_6RD_TUNNEL_ENTRY: {
				PPA_TUNNEL_INFO *tnnl_info = (PPA_TUNNEL_INFO *)buffer;
				del_tunnel_entry(tnnl_info->tunnel_idx);
				return PPA_SUCCESS;
			}
	case PPA_GENERIC_HAL_ADD_DSLITE_TUNNEL_ENTRY: {
				PPA_TUNNEL_INFO *tnnl_info = (PPA_TUNNEL_INFO *)buffer;
				return add_tunnel_entry(tnnl_info->tunnel_type, tnnl_info->tunnel_idx);
			}
	case PPA_GENERIC_HAL_DEL_DSLITE_TUNNEL_ENTRY: {
				PPA_TUNNEL_INFO *tnnl_info = (PPA_TUNNEL_INFO *)buffer;
				del_tunnel_entry(tnnl_info->tunnel_idx);
				return PPA_SUCCESS;
			}
	case PPA_GENERIC_HAL_ADD_LRO_ENTRY:{
			PPA_LRO_INFO *lro_info = (PPA_LRO_INFO *) buffer;
			return add_lro_entry(lro_info);
		}
	case PPA_GENERIC_HAL_DEL_LRO_ENTRY: {
			PPA_LRO_INFO *lro_info = (PPA_LRO_INFO *) buffer;
			del_lro_entry(lro_info->session_id);
			return PPA_SUCCESS;	
		}
	case PPA_GENERIC_HAL_GET_ITF_MIB: {
			PPE_ITF_MIB_INFO *mib=(PPE_ITF_MIB_INFO *)buffer;
			get_itf_mib( mib->itf, &mib->mib);
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_GET_NEW_ITF_MIB: {
			PPA_ITF_MIB_INFO *mib_tmp=(PPA_ITF_MIB_INFO *)buffer;
			return (get_itf_mib_generic(mib_tmp));
		}
	case PPA_GENERIC_HAL_TEST_CLEAR_ROUTE_HIT_STAT: {
		/*check whether a routing entry is hit or not*/
			PPA_ROUTING_INFO *route=(PPA_ROUTING_INFO *)buffer;
			test_and_clear_hit_stat( route->entry, &route->f_hit);
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_TEST_CLEAR_BR_HIT_STAT: {
		/*check whether a bridge mac entry is hit or not*/
			PPA_BR_MAC_INFO *br_mac=(PPA_BR_MAC_INFO *)buffer;
			test_and_clear_bridging_hit_stat( br_mac->fid, br_mac->mac, &br_mac->f_hit, &br_mac->age_timer);
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_TEST_CLEAR_MC_HIT_STAT: {
		/*check whether a multicast entry is hit or not*/
			PPA_MC_INFO *mc =(PPA_MC_INFO *)buffer;
			test_and_clear_hit_stat( mc->p_entry, &mc->f_hit);
			return PPA_SUCCESS;
		}
	case PPA_GENERIC_HAL_ADD_CLASS_RULE: {
			PPA_CLASS_RULE *class_rule = (PPA_CLASS_RULE*)buffer;
			return pae_hal_add_class_rule(class_rule);
		}
	case PPA_GENERIC_HAL_DEL_CLASS_RULE: {
			PPA_CLASS_RULE *class_rule = (PPA_CLASS_RULE*)buffer;
			return pae_hal_del_class_rule(class_rule);
		}
	case PPA_GENERIC_HAL_MOD_CLASS_RULE: {
			PPA_CLASS_RULE *class_rule = (PPA_CLASS_RULE*)buffer;
			return pae_hal_mod_class_rule(class_rule);
		}
	case PPA_GENERIC_HAL_GET_CLASS_RULE: {
			PPA_CLASS_RULE *class_rule = (PPA_CLASS_RULE*)buffer;
			return pae_hal_get_class_rule(class_rule);
		}
	case PPA_GENERIC_HAL_INIT: {
			return pae_hal_register_caps();;
		}
	case PPA_GENERIC_HAL_EXIT: {
			return pae_hal_deregister_caps();
		}
	case PPA_GENERIC_HAL_QOS_DSCP_CLASS_MAP_SET: {
			return pae_hal_dscp_class_cfg(1);
		}
	case PPA_GENERIC_HAL_QOS_DSCP_CLASS_MAP_RESET: {
			return pae_hal_dscp_class_cfg(0);
		}
#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
	case PPA_GENERIC_HAL_SET_MIB_MODE_ENABLE: {
			PPA_CMD_MIB_MODE_INFO *mib_mode = (PPA_CMD_MIB_MODE_INFO *)buffer;
			return pae_hal_mib_mode_cfg(mib_mode);
		}
	case PPA_GENERIC_HAL_GET_MIB_MODE_ENABLE: {
			PPA_CMD_MIB_MODE_INFO *mib_mode = (PPA_CMD_MIB_MODE_INFO *)buffer;
			return pae_hal_get_mib_mode(mib_mode);
		}
#endif
	case PPA_GENERIC_HAL_SET_QOS_METER_CFG:
	{
		PPA_QOS_METER_CFG *meter_cfg = (PPA_QOS_METER_CFG *)buffer;
		return pae_hal_meter_cfg_set(meter_cfg);
	}
	default:
		return PPA_FAILURE;
	}
	return PPA_FAILURE;
}

/*
 * ####################################
 *			 Init/Cleanup API
 * ####################################
 */

static inline void hal_init(void)
{
	ppa_drv_generic_hal_register(PAE_HAL, pae_hal_generic_hook);

	/* initialize the egress vlan table*/
	init_egress_vlan_table();
	init_class_mgmt();
	init_pae_flows();
	init_pae_ports();
	init_lro_table();
#ifdef PAE_HW_METER
	ppa_timer_init(&g_hit_meter_timer, ppa_poll_meter);
	ppa_timer_add(&g_hit_meter_timer, g_polling_time);
#endif
}

static inline void hal_exit(void)
{
	ppa_drv_generic_hal_deregister( PAE_HAL);
#ifdef PAE_HW_METER
	ppa_timer_del(&g_hit_meter_timer);
	ppa_pae_meter_lock_destroy();
#endif
	uninit_egress_vlan_table();
	uninit_pae_flows();
	uninit_class_mgmt();
	uninit_pae_ports(); 
}

static int __init pae_hal_init(void)
{
	hal_init();
#if defined(PPA_API_PROC)
	pae_proc_file_create();
#endif
#ifdef PAE_HW_METER
	ppa_pae_meter_lock_init();
#endif
	return 0;
}

static void __exit pae_hal_exit(void)
{
#if defined(PPA_API_PROC)
	pae_proc_file_remove();
#endif
	hal_exit();
}

module_init(pae_hal_init);
module_exit(pae_hal_exit);
MODULE_LICENSE("GPL");
