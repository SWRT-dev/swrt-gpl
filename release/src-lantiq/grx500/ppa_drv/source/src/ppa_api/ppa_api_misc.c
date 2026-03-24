/*******************************************************************************
 **
 ** FILE NAME	: ppa_api_misc.c
 ** PROJECT	: PPA
 ** MODULES	: PPA API (Routing/Bridging Acceleration APIs)
 **
 ** DATE	: 3 NOV 2008
 ** AUTHOR	: Xu Liang
 ** DESCRIPTION : PPA Protocol Stack Hook API Miscellaneous Functions
 ** COPYRIGHT	: Copyright (c) 2020, MaxLinear, Inc.
 **               Copyright (c) 2009, Lantiq Deutschland GmbH
 **               Am Campeon 3; 85579 Neubiberg, Germany
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date		$Author				$Comment
 ** 03 NOV 2008  Xu Liang			   Initiate Version
 ** 10 DEC 2012  Manamohan Shetty	   Added the support for RTP,MIB mode
 **									 Features
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

#include <linux/cdev.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/in.h>
#include <net/sock.h>
#include <asm/time.h>

/*
 *  PPA Specific Head File
 */
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_hal_api.h>
#include <net/ppa/ppa_hook.h>
#include <net/ppa/ppa_drv_wrapper.h>
#include <net/ppa/ppa_hal_wrapper.h>

#include "ppa_api_misc.h"
#include "ppa_api_session.h"
#include "ppa_api_netif.h"
#include "ppa_api_proc.h"
#include "ppa_api_tools.h"
#if IS_ENABLED(CONFIG_PPA_QOS)
#include "ppa_api_qos.h"
#endif
#include "ppa_api_mib.h"
#include "ppa_api_hal_selector.h"
#include "ppa_api_tools.h"

#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
#include "ppa_api_sw_accel.h"
#include "ppa_sae_hal.h"
#endif

#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
#include "ppa_api_session_limit.h"
#endif

/*
 * ####################################
 *			  Definition
 * ####################################
 */

/*
 *  device constant
 */
#define PPA_CHR_DEV_MAJOR		181
#define PPA_DEV_NAME			"ppa"

/*
 * ####################################
 *			  Data Type
 * ####################################
 */

/*
 * ####################################
 *			 Declaration
 * ####################################
 */
#if IS_ENABLED(CONFIG_SOC_GRX500)
extern int32_t ppa_get_fid(PPA_IFNAME *ifname, uint16_t *fid);
#endif
extern int32_t (*ppa_drv_hal_generic_hook)(PPA_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag);

long ppa_dev_ioctl(struct file *, unsigned int, unsigned long);
static int32_t ppa_get_all_vlan_filter_count(uint32_t);
/* since no where used following functions, commentd*/
#if 0
static int ppa_dev_open(struct inode *, struct file *);
static int ppa_dev_release(struct inode *, struct file *);
#ifdef PPA_API_PROC
static int print_ppa_subsystem_ver(char *, int, char *, unsigned int , unsigned int , unsigned int,
		unsigned int , unsigned int , unsigned int , unsigned int , unsigned int );
static int print_driver_ver(char *, int, char *, unsigned int, unsigned int, unsigned int,
		unsigned int, unsigned int, unsigned int, unsigned int);

static void proc_file_create(void);
static void proc_file_delete(void);
static int proc_read_ver(struct seq_file *seq, void *v);
#endif
#endif

int32_t ppa_ioctl_bridge_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_bridge_enable_status(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
#if IS_ENABLED(CONFIG_SOC_GRX500)
int32_t ppa_ioctl_get_bridge_fid(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
#endif
int32_t ppa_ioctl_get_hook_list(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_set_hook(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_max_entry(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t ppa_ioctl_get_portid(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t ppa_ioctl_del_session(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t ppa_ioctl_add_session(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t ppa_ioctl_modify_session(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
int32_t ppa_ioctl_get_prio_sessions(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t ppa_ioctl_manage_sessions(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t ppa_ioctl_get_sessions_count(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
#endif
int32_t ppa_ioctl_set_session_timer(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t ppa_ioctl_get_session_timer(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info);
int32_t ppa_ioctl_set_ppe_fastpath_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_ppe_fastpath_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
int32_t ppa_ioctl_set_sw_fastpath_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_sw_fastpath_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_set_sw_session_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
int32_t ppa_ioctl_get_sw_session_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA * cmd_info);
#endif
extern void ppa_session_delete_by_ip(PPA_IPADDR *ip);
extern void ppa_session_delete_by_macaddr(uint8_t *mac);

/*
 * ####################################
 *		   Global Variable
 * ####################################
 */

extern int g_ppa_proc_dir_flag;
extern struct proc_dir_entry *g_ppa_proc_dir;

extern int g_ppa_api_proc_dir_flag;
extern struct proc_dir_entry *g_ppa_api_proc_dir;

static char ZeroMAC[]={0,0,0,0,0,0};
/*
 * ####################################
 *		   Extern Variable
 * ####################################
 */
extern struct proc_dir_entry proc_root;

/*
 * ####################################
 *			Local Function
 * ####################################
 */

/*
 *  file operation functions
 */
#if IS_ENABLED(CONFIG_PPA_QOS)
static unsigned int non_checking_cmd_list[]={ PPA_CMD_INIT, PPA_CMD_EXIT, PPA_CMD_GET_STATUS, \
	PPA_CMD_GET_COUNT_PPA_HASH, PPA_CMD_GET_VERSION, PPA_CMD_GET_SIZE, \
		PPA_CMD_GET_QOS_STATUS,PPA_CMD_GET_QOS_QUEUE_MAX_NUM, PPA_CMD_GET_QOS_MIB, PPA_CMD_GET_CTRL_QOS_WFQ, \
		PPA_CMD_GET_QOS_WFQ, PPA_CMD_GET_CTRL_QOS_RATE, PPA_CMD_GET_QOS_RATE,\
		PPA_CMD_GET_HOOK_LIST, PPA_CMD_SET_HOOK,PPA_CMD_READ_MEM, PPA_CMD_SET_MEM,\
		PPA_CMD_GET_MAX_ENTRY,PPA_CMD_GET_PORTID, \
		PPA_CMD_GET_DSL_MIB,PPA_CMD_CLEAR_DSL_MIB,PPA_CMD_GET_PORT_MIB,PPA_CMD_CLEAR_PORT_MIB, \
		PPA_CMD_DBG_TOOL
};
#else
static unsigned int non_checking_cmd_list[]={ PPA_CMD_INIT, PPA_CMD_EXIT, PPA_CMD_GET_STATUS, \
	PPA_CMD_GET_COUNT_PPA_HASH, PPA_CMD_GET_VERSION, PPA_CMD_GET_SIZE, \
		PPA_CMD_GET_HOOK_LIST, PPA_CMD_SET_HOOK,PPA_CMD_READ_MEM, PPA_CMD_SET_MEM,\
		PPA_CMD_GET_MAX_ENTRY,PPA_CMD_GET_PORTID, \
		PPA_CMD_GET_DSL_MIB,PPA_CMD_CLEAR_DSL_MIB,PPA_CMD_GET_PORT_MIB,PPA_CMD_CLEAR_PORT_MIB, \
		PPA_CMD_DBG_TOOL
};
#endif

int32_t need_checking_ppa_status_cmd(unsigned int cmd )
{
	int check_flag = 1;
	int i;

	for(i=0; i< NUM_ENTITY(non_checking_cmd_list); i++ ) {
		if( non_checking_cmd_list[i] == cmd ) {
			check_flag = 0;
			break;
		}
	}

	return check_flag;
}

#if defined(FIX_MEM_SIZE)
struct uc_session_node tmp_session_buf[FIX_MEM_SIZE];
#endif

static int ppa_ioctl_get_sessions(unsigned int cmd, PPA_CMD_SESSIONS_INFO *pSessIn, unsigned long arg)
{
	struct uc_session_node *p_item_one_index=NULL;
	uint32_t   session_flag;
	uint32_t pos = 0, buff_len;
	uint32_t count=0, ret=PPA_SUCCESS;
	uint32_t i, tmp_num=0;
	void *pUserSessionList =((void*)arg) + offsetof(PPA_CMD_SESSIONS_INFO,session_list);

	if ( ppa_copy_from_user(&pSessIn->count_info, (void *)arg, sizeof(pSessIn->count_info)) != 0 )
		return -EFAULT;

	if( cmd == PPA_CMD_GET_LAN_SESSIONS )
		session_flag = SESSION_LAN_ENTRY;
	else if( cmd == PPA_CMD_GET_WAN_SESSIONS )
		session_flag = SESSION_WAN_ENTRY;
	else if( cmd == PPA_CMD_GET_LAN_WAN_SESSIONS )
		session_flag = SESSION_LAN_ENTRY | SESSION_WAN_ENTRY;
	else
		return -EIO;

	if( pSessIn->count_info.hash_index ) {
		pos = pSessIn->count_info.hash_index -1;
	}
	if( pSessIn->count_info.count == 0 )
		return -EIO;

	do {
		struct uc_session_node *pp_item;
#if defined(FIX_MEM_SIZE)
		p_item_one_index = tmp_session_buf;
		buff_len = NUM_ENTITY(tmp_session_buf);
#else

		if( p_item_one_index ) {
			ppa_free(p_item_one_index);
			p_item_one_index= NULL;
		}
		buff_len = 0;
#endif
		tmp_num = 0;

		if( (ret = ppa_session_get_items_in_hash(pos, &p_item_one_index,buff_len,
				&tmp_num, pSessIn->count_info.stamp_flag)) == PPA_INDEX_OVERFLOW ) {
			ret = PPA_SUCCESS;
			break;
		}
		pos ++;

		if( ret == PPA_ENOMEM) {
			critial_err("p_item_one_index NULL for no memory for hash index %d\n", pos);
			ret = PPA_FAILURE;
			break;
		}

		if( ret != PPA_SUCCESS ) continue;
		if( tmp_num == 0 || !p_item_one_index ) continue;

		for(i=0; i<tmp_num; i++) {

			pp_item = &p_item_one_index[i];
			if ( pp_item->flags & session_flag ) {
				if( pSessIn->count_info.flag == 0 ||/* get all LAN or WAN sessions */
					((pSessIn->count_info.flag == SESSION_ADDED_IN_HW) &&
					(pp_item->flags & SESSION_ADDED_IN_HW) )  || /*get all accelerated sessions only */
					((pSessIn->count_info.flag == SESSION_ADDED_IN_SW) &&
					(pp_item->flags & SESSION_ADDED_IN_SW) ) ||
					((pSessIn->count_info.flag == SESSION_NON_ACCE_MASK) &&
					!(pp_item->flags & SESSION_ADDED_IN_HW ) && !(pp_item->flags & SESSION_ADDED_IN_SW ) ) ) {

					if( count >= pSessIn->count_info.count ) break;  /*buffer is full. We should stop now*/
					pSessIn->session_list[0].ip_proto = pp_item->pkt.ip_proto;
					pSessIn->session_list[0].ip_tos = pp_item->pkt.ip_tos;
					ppa_memcpy( &pSessIn->session_list[0].src_ip, &pp_item->pkt.src_ip,
						sizeof(pSessIn->session_list[0].src_ip));
					pSessIn->session_list[0].src_port = pp_item->pkt.src_port;
					ppa_memcpy( &pSessIn->session_list[0].dst_ip, &pp_item->pkt.dst_ip,
						sizeof(pSessIn->session_list[0].dst_ip));
					pSessIn->session_list[0].dst_port = pp_item->pkt.dst_port;
					ppa_memcpy( &pSessIn->session_list[0].natip, &pp_item->pkt.natip,
						sizeof(pSessIn->session_list[0].natip));
					pSessIn->session_list[0].nat_port= pp_item->pkt.nat_port;
					pSessIn->session_list[0].new_dscp= pp_item->pkt.new_dscp;
					pSessIn->session_list[0].new_vci = pp_item->pkt.new_vci;
					pSessIn->session_list[0].dslwan_qid = pp_item->dslwan_qid;
					pSessIn->session_list[0].dest_ifid = pp_item->dest_ifid;
					pSessIn->session_list[0].flags= pp_item->flags;
					pSessIn->session_list[0].flag2 = pp_item->flag2;
					pSessIn->session_list[0].host_bytes = pp_item->host_bytes - pp_item->prev_clear_host_bytes;
					pSessIn->session_list[0].hw_bytes = pp_item->acc_bytes - pp_item->prev_clear_acc_bytes;
					pSessIn->session_list[0].session = pp_item->session;
					pSessIn->session_list[0].hash = (uint32_t ) pp_item->hash;
#if defined(SKB_PRIORITY_DEBUG) && SKB_PRIORITY_DEBUG
					pSessIn->session_list[0].priority = (uint32_t ) pp_item->pkt.priority;
#endif
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
					if( ppa_get_session_limit_enable(0) )
						pSessIn->session_list[0].session_priority = (uint32_t ) pp_item->session_priority;
					else
						pSessIn->session_list[0].session_priority = 0;

#endif
#if defined(SESSION_STATISTIC_DEBUG) && SESSION_STATISTIC_DEBUG
					/*below variable is used for session management debugging purpose */
					pSessIn->session_list[0].hash_index = (uint32_t ) pp_item->hash_index;
					pSessIn->session_list[0].hash_table_id = (uint32_t ) pp_item->hash_table_id;
#endif
					pSessIn->session_list[0].prev_sess_bytes = pp_item->prev_sess_bytes;

					pSessIn->session_list[0].rx_if_name[0] = '\0';
					if(pp_item->rx_if)
						ppa_strncpy( (void *)pSessIn->session_list[0].rx_if_name,
							(void *)ppa_get_netif_name(pp_item->rx_if),
							sizeof(pSessIn->session_list[0].rx_if_name) );

					pSessIn->session_list[0].tx_if_name[0] = '\0';
					if(pp_item->tx_if)
						ppa_strncpy( (void *)pSessIn->session_list[0].tx_if_name,
							(void *)ppa_get_netif_name(pp_item->tx_if),
							sizeof(pSessIn->session_list[0].tx_if_name) );

					if ( ppa_copy_to_user( pUserSessionList + count*sizeof(pSessIn->session_list[0]) ,
								pSessIn->session_list,
								sizeof(pSessIn->session_list[0] )) != 0 ) {
						if(p_item_one_index){

							ppa_free(p_item_one_index);
							p_item_one_index= NULL;
						}
						return -EFAULT;
					}

					count ++;
				}
			}
		}

		if( pSessIn->count_info.hash_index )
			break; /* retrive only from ONE hash(bucket) */

	} while(count < pSessIn->count_info.count );

#if !defined(FIX_MEM_SIZE)
	if( p_item_one_index ) {

		ppa_free(p_item_one_index);
		p_item_one_index= NULL;
	}
#endif
	pSessIn->count_info.count = count;  /*update the real session number to user space*/
	if ( ppa_copy_to_user(  (void *)arg, &pSessIn->count_info, sizeof(pSessIn->count_info)) != 0 ) {
		return -EFAULT;
	}
	if( ret == PPA_SUCCESS )
		return 0;

	return -EINVAL;
}

long ppa_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int res = 0;
	PPA_INIT_INFO info;

	PPA_CMD_DATA *cmd_info = (PPA_CMD_DATA *)ppa_malloc(sizeof(PPA_CMD_DATA));

	if ( cmd_info == NULL )
		return -EFAULT;

	if ( ppa_ioc_type(cmd) != PPA_IOC_MAGIC ) {
		printk("ppa_ioc_type(%08X - %d) != PPA_IOC_MAGIC(%d)\n", cmd, _IOC_TYPE(cmd), PPA_IOC_MAGIC);
		goto EXIT_EIO;
	} else if( ppa_ioc_nr(cmd) >= PPA_IOC_MAXNR ) {
		printk("Current cmd is %02x wrong, it should less than %02x\n", _IOC_NR(cmd), PPA_IOC_MAXNR );
		goto EXIT_EIO;
	}

	if ( ((ppa_ioc_dir(cmd) & ppa_ioc_read()) && !access_ok(VERIFY_WRITE, arg, ppa_ioc_size(cmd)))
			|| ((ppa_ioc_dir(cmd) & ppa_ioc_write()) && !access_ok(VERIFY_READ, arg, ppa_ioc_size(cmd))) ) {
		printk("access check: (%08X && %d) || (%08X && %d)\n",
				(ppa_ioc_dir(cmd) & ppa_ioc_read()),
				(int)!ppa_ioc_access_ok(ppa_ioc_verify_write(), arg, ppa_ioc_size(cmd)),
				(ppa_ioc_dir(cmd) & ppa_ioc_write()),
				(int)!ppa_ioc_access_ok(ppa_ioc_verify_read(), arg, ppa_ioc_size(cmd)));
		goto EXIT_EFAULT;
	}

	if( need_checking_ppa_status_cmd(cmd)) {
		if( !ppa_is_init()) {
			goto EXIT_EFAULT;
		}
	}

	switch ( cmd ) {
		case PPA_CMD_INIT: {
				
			PPA_IFINFO ifinfo[ 2 * PPA_MAX_IFS_NUM ];
			int i;

			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;

			if ( ppa_copy_from_user(&cmd_info->init_info, (void *)arg, sizeof(cmd_info->init_info)) != 0 )
				goto EXIT_EFAULT;

			ppa_memset(&info, 0, sizeof(info));
			ppa_memset(ifinfo, 0, sizeof(ifinfo));

			info.num_lanifs	= cmd_info->init_info.num_lanifs;
			info.p_lanifs	= ifinfo;
			info.num_wanifs	= cmd_info->init_info.num_wanifs;
			info.p_wanifs	= ifinfo + NUM_ENTITY(cmd_info->init_info.p_lanifs);
			info.max_lan_source_entries = cmd_info->init_info.max_lan_source_entries;
			info.max_wan_source_entries = cmd_info->init_info.max_wan_source_entries;
			info.max_mc_entries = cmd_info->init_info.max_mc_entries;
			info.max_bridging_entries = cmd_info->init_info.max_bridging_entries;
			if( cmd_info->init_info.add_requires_min_hits ) {
				info.add_requires_min_hits  = cmd_info->init_info.add_requires_min_hits;
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"add_requires_min_hits is set to %d\n", info.add_requires_min_hits);
			}
			for ( i = 0; i < info.num_lanifs; i++ ) {
				info.p_lanifs[i].ifname   = cmd_info->init_info.p_lanifs[i].ifname;
				info.p_lanifs[i].if_flags = cmd_info->init_info.p_lanifs[i].if_flags;
			}

			for ( i = 0; i < info.num_wanifs; i++ ) {
				info.p_wanifs[i].ifname   = cmd_info->init_info.p_wanifs[i].ifname;
				info.p_wanifs[i].if_flags = cmd_info->init_info.p_wanifs[i].if_flags;
			}

			if ( ppa_init(&info, cmd_info->init_info.flags) != PPA_SUCCESS ) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_init fail");
				goto EXIT_EIO;
			}
#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
			if ( ppa_set_mib_mode(cmd_info->init_info.mib_mode) != PPA_SUCCESS )
				goto EXIT_EIO;
#endif
			else
				goto EXIT_ZERO;

		}
		case PPA_CMD_EXIT: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
				
			ppa_enable(0, 0, 0);
			ppa_exit();
			goto EXIT_ZERO;
		}
		case PPA_CMD_ENABLE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
				
			if ( ppa_copy_from_user(&cmd_info->ena_info, (void *)arg, sizeof(cmd_info->ena_info)) != 0 )
				goto EXIT_EFAULT;

			if ( ppa_enable(cmd_info->ena_info.lan_rx_ppa_enable, cmd_info->ena_info.wan_rx_ppa_enable, cmd_info->ena_info.flags) != PPA_SUCCESS )
				goto EXIT_EIO;
			else
					goto EXIT_ZERO;
		}
#if IS_ENABLED(CONFIG_PPA_MIB_MODE)
		case PPA_CMD_SET_MIB_MODE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
				
			if ( ppa_copy_from_user(&cmd_info->mib_mode_info, (void *)arg, sizeof(cmd_info->mib_mode_info)) != 0 )
				goto EXIT_EFAULT;
			
			ppa_ioctl_clear_ports_mib(0, 0, NULL);

			if ( ppa_set_mib_mode(cmd_info->mib_mode_info.mib_mode) != PPA_SUCCESS )
				goto EXIT_EIO;
			else
				goto EXIT_ZERO;

			ppa_ioctl_clear_ports_mib(0, 0, NULL);
		}
		case PPA_CMD_GET_MIB_MODE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
				
			if ( ppa_copy_from_user(&cmd_info->mib_mode_info, (void *)arg, sizeof(cmd_info->mib_mode_info)) != 0 )
					goto EXIT_EFAULT;

			if ( ppa_get_mib_mode(&cmd_info->mib_mode_info.mib_mode) != PPA_SUCCESS )
				goto EXIT_EIO;

			if ( ppa_copy_to_user((void *)arg, &cmd_info->mib_mode_info, sizeof(cmd_info->mib_mode_info)) != 0 )
				goto EXIT_EFAULT;
			else
				goto EXIT_ZERO;
		}

#endif /* CONFIG_PPA_MIB_MODE */
		case PPA_CMD_GET_STATUS: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
				
			cmd_info->ena_info.lan_rx_ppa_enable = 0;
			cmd_info->ena_info.wan_rx_ppa_enable = 0;

			if ( ppa_copy_from_user(&cmd_info->ena_info, (void *)arg, sizeof(cmd_info->ena_info)) != 0 )
				goto EXIT_EFAULT;

			if ( ppa_get_status(&cmd_info->ena_info.lan_rx_ppa_enable, &cmd_info->ena_info.wan_rx_ppa_enable, 
				cmd_info->ena_info.flags) != PPA_SUCCESS )
				cmd_info->ena_info.flags = 0;
			else
				cmd_info->ena_info.flags = 1;
			if ( ppa_copy_to_user((void *)arg, &cmd_info->ena_info, sizeof(cmd_info->ena_info)) != 0 )
				goto EXIT_EFAULT;
			else
				goto EXIT_ZERO;
		}
		case PPA_CMD_MODIFY_MC_ENTRY: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
				
			if ( ppa_copy_from_user(&cmd_info->mc_entry, (void *)arg, sizeof(cmd_info->mc_entry)) != 0 )
				goto EXIT_EFAULT;

			if ( ppa_mc_entry_modify(cmd_info->mc_entry.mcast_addr,cmd_info->mc_entry.source_ip, NULL,
				&cmd_info->mc_entry.mc_extra, cmd_info->mc_entry.flags) != PPA_SUCCESS )
				goto EXIT_EIO;
			else
				goto EXIT_ZERO;
		}

		case PPA_CMD_GET_MC_ENTRY: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
				
			if ( ppa_copy_from_user(&cmd_info->mc_entry, (void *)arg, sizeof(cmd_info->mc_entry)) != 0 )
				goto EXIT_EFAULT;

			ppa_memset(&cmd_info->mc_entry.mc_extra, 0, sizeof(cmd_info->mc_entry.mc_extra));

			if ( ppa_mc_entry_get(cmd_info->mc_entry.mcast_addr,cmd_info->mc_entry.source_ip,
				&cmd_info->mc_entry.mc_extra, -1) != PPA_SUCCESS ) //-1: to get all flag related parameters
					goto EXIT_EIO;
				else if ( ppa_copy_to_user((void *)arg, &cmd_info->mc_entry, sizeof(cmd_info->mc_entry)) != 0 )
					goto EXIT_EFAULT;
				else
					goto EXIT_ZERO;
			}
#if defined(RTP_SAMPLING_ENABLE) && RTP_SAMPLING_ENABLE
		case PPA_CMD_SET_RTP: {
			PPA_MC_GROUP mc_group;
			int32_t res;
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;

			ppa_memset( &mc_group, 0, sizeof(mc_group));
			if ( ppa_copy_from_user(&cmd_info->mc_entry, (void *)arg, sizeof(cmd_info->mc_entry)) != 0 )
				goto EXIT_EFAULT;

			ppa_memcpy( &mc_group.ip_mc_group, &cmd_info->mc_entry.mcast_addr, sizeof(mc_group.ip_mc_group) );
			ppa_memcpy( &mc_group.source_ip, &cmd_info->mc_entry.source_ip, sizeof(mc_group.source_ip));

			mc_group.RTP_flag = cmd_info->mc_entry.RTP_flag;
			res = ppa_mc_entry_rtp_set(&mc_group);
			if ( res != PPA_SUCCESS ) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_mc_entry_rtp_set fail\n");
				goto EXIT_EIO;
			} else
				goto EXIT_ZERO;
		}
#endif
		case PPA_CMD_ADD_MAC_ENTRY: {
#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING)
			PPA_NETIF *netif, *brif;
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;

			if ( ppa_copy_from_user(&cmd_info->mac_entry, (void *)arg, sizeof(cmd_info->mac_entry)) != 0 )
				goto EXIT_EFAULT;

			netif = ppa_get_netif(cmd_info->mac_entry.ifname);
			if ( !netif )
				goto EXIT_EIO;

			brif = ppa_get_netif(cmd_info->mac_entry.brname);
			if( !brif)
				goto EXIT_EIO;

			if ( ppa_bridge_entry_add(cmd_info->mac_entry.mac_addr, brif, netif, cmd_info->mac_entry.flags) != PPA_SESSION_ADDED )
				goto EXIT_EIO;
			else
#endif /*CONFIG_PPA_BR_MAC_LEARNING*/
				goto EXIT_ZERO;
		}
		case PPA_CMD_DEL_MAC_ENTRY: {
#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING)
			PPA_NETIF *brif;
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;

			if ( ppa_copy_from_user(&cmd_info->mac_entry, (void *)arg, sizeof(cmd_info->mac_entry)) != 0 )
				goto EXIT_EFAULT;

			brif = ppa_get_netif(cmd_info->mac_entry.brname);
			if( !brif)
				goto EXIT_EIO;

			if ( ppa_bridge_entry_delete(cmd_info->mac_entry.mac_addr, brif,cmd_info->mac_entry.flags) != PPA_SUCCESS )
				goto EXIT_EIO;
			else
#endif /*CONFIG_PPA_BR_MAC_LEARNING*/
				goto EXIT_ZERO;
		}
		case PPA_CMD_SET_VLAN_IF_CFG:
		case PPA_CMD_ADD_VLAN_FILTER_CFG:
		case PPA_CMD_DEL_VLAN_FILTER_CFG:
		case PPA_CMD_DEL_ALL_VLAN_FILTER_CFG:
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			goto EXIT_ZERO;
		case PPA_CMD_GET_VLAN_IF_CFG:
		case PPA_CMD_GET_ALL_VLAN_FILTER_CFG:
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			goto EXIT_ZERO;
		case PPA_CMD_SET_IF_MAC: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			if ( ppa_copy_from_user(&cmd_info->if_mac, (void *)arg, sizeof(cmd_info->if_mac)) != 0 )
				goto EXIT_EFAULT;

			if ( ppa_set_if_mac_address(cmd_info->if_mac.ifname, cmd_info->if_mac.mac, cmd_info->if_mac.flags) != PPA_SUCCESS )
				goto EXIT_EIO;
			else
				goto EXIT_ZERO;
		}
		case PPA_CMD_GET_IF_MAC: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			if ( ppa_copy_from_user(&cmd_info->if_mac, (void *)arg, sizeof(cmd_info->if_mac)) != 0 )
				goto EXIT_EFAULT;

			ppa_memset(cmd_info->if_mac.mac, 0, sizeof(cmd_info->if_mac.mac));

			if ( ppa_get_if_mac_address(cmd_info->if_mac.ifname, cmd_info->if_mac.mac, cmd_info->if_mac.flags) != PPA_SUCCESS )
				goto EXIT_EIO;
			else if ( ppa_copy_to_user((void *)arg, &cmd_info->if_mac, sizeof(cmd_info->if_mac)) != 0 )
				goto EXIT_EFAULT;
			else
				goto EXIT_ZERO;
		}
		case PPA_CMD_ADD_LAN_IF:
		case PPA_CMD_ADD_WAN_IF: {
			PPA_IFINFO ifinfo = {0};
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;

			if ( ppa_copy_from_user(&cmd_info->if_info, (void *)arg, sizeof(cmd_info->if_info)) != 0 )
				goto EXIT_EFAULT;

			if (strnlen(cmd_info->if_info.ifname, PPA_IF_NAME_SIZE) == 0) {
				res = -EINVAL;
				goto EXIT_EIO;
			}

			ifinfo.ifname = cmd_info->if_info.ifname;
			ifinfo.ifname_lower = cmd_info->if_info.ifname_lower;
			ifinfo.force_wanitf_flag = cmd_info->if_info.force_wanitf_flag;
			if ( cmd == PPA_CMD_ADD_LAN_IF )
				ifinfo.if_flags = PPA_F_LAN_IF;
			if ( ppa_add_if(&ifinfo, 0) != PPA_SUCCESS ){
				res = -EINVAL;
				goto EXIT_EIO;
			}
			else
				goto EXIT_ZERO;
		}
		case PPA_CMD_DEL_LAN_IF:
		case PPA_CMD_DEL_WAN_IF: {
			PPA_IFINFO ifinfo = {0};
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;

			if ( ppa_copy_from_user(&cmd_info->if_info, (void *)arg, sizeof(cmd_info->if_info)) != 0 )
				goto EXIT_EFAULT;

			if (strnlen(cmd_info->if_info.ifname, PPA_IF_NAME_SIZE) == 0) {
				res = -EINVAL;
				goto EXIT_EIO;
			}

			ifinfo.ifname = cmd_info->if_info.ifname;
			ifinfo.hw_disable = cmd_info->if_info.hw_disable;
			if ( cmd == PPA_CMD_DEL_LAN_IF )
				ifinfo.if_flags = PPA_F_LAN_IF;
			if ( ppa_del_if(&ifinfo, 0) != PPA_SUCCESS ){
				res = -EINVAL;
				goto EXIT_EIO;
			}
			else
				goto EXIT_ZERO;
		}
		case PPA_CMD_GET_LAN_IF:
		case PPA_CMD_GET_WAN_IF: {
			struct netif_info *ifinfo;
			uint32_t pos = 0;
			int i = 0;
			uint32_t flag_mask = ( cmd == PPA_CMD_GET_LAN_IF)  ? NETIF_LAN_IF : NETIF_WAN_IF;
			uint32_t if_flags = (cmd == PPA_CMD_GET_LAN_IF) ? PPA_F_LAN_IF : 0;

			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			ppa_memset(&cmd_info->all_if_info, 0, sizeof(cmd_info->all_if_info));

			if ( ppa_netif_start_iteration(&pos, &ifinfo) == PPA_SUCCESS ) {
				do {
					if ( (ifinfo->flags & flag_mask) ) {
						ppa_strncpy(cmd_info->all_if_info.ifinfo[i].ifname, ifinfo->name, PPA_IF_NAME_SIZE);
						cmd_info->all_if_info.ifinfo[i].if_flags = if_flags;
#if defined(PPA_IF_MIB) && PPA_IF_MIB
						cmd_info->all_if_info.ifinfo[i].acc_rx = ifinfo->hw_accel_stats.rx_bytes - ifinfo->prev_clear_acc_rx;
						cmd_info->all_if_info.ifinfo[i].acc_tx = ifinfo->hw_accel_stats.tx_bytes - ifinfo->prev_clear_acc_tx;
#endif
						if ( ++i >= NUM_ENTITY(cmd_info->all_if_info.ifinfo) )
							break;
					}
				} while ( ppa_netif_iterate_next(&pos, &ifinfo) == PPA_SUCCESS );
			}

			ppa_netif_stop_iteration();
			cmd_info->all_if_info.num_ifinfos = i;

			if ( ppa_copy_to_user((void *)arg, &cmd_info->all_if_info, sizeof(cmd_info->all_if_info)) != 0 )
				goto EXIT_EFAULT;
			else
				goto EXIT_ZERO;
		}
		case PPA_CMD_ADD_MC: {
			PPA_MC_GROUP mc_group;
			int32_t res;
			int i, idx = 0;

			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;

			ppa_memset( &mc_group, 0, sizeof(mc_group));
			if ( ppa_copy_from_user(&cmd_info->mc_add_info, (void *)arg, sizeof(cmd_info->mc_add_info)) != 0 )
				goto EXIT_EFAULT;

			if ( cmd_info->mc_add_info.num_ifs > PPA_MAX_MC_IFS_NUM ) {
			/*  if num_ifs is zero, it means to remove the multicast session from PPE HW	*/
				printk("Error cmd_info->mc_add_info.num_ifs=%d is over max limit %d", cmd_info->mc_add_info.num_ifs, PPA_MAX_MC_IFS_NUM);
				goto EXIT_EIO;
			}

			for ( i = 0, idx = 0; i < cmd_info->mc_add_info.num_ifs; i++ ) {
				mc_group.array_mem_ifs[i].ifname = cmd_info->mc_add_info.lan_ifname[i];  /*  downstream interfce: 1*/
				mc_group.if_mask |= 1 << idx;
				idx++;
			}
			mc_group.num_ifs = idx; /*  downstream interfce number */
			/* Initialize all other member ifname to NULL string */
			for (i = mc_group.num_ifs; i < PPA_MAX_MC_IFS_NUM; i++)
				mc_group.array_mem_ifs[i].ifname = "NULL";

			if ( strnlen(cmd_info->mc_add_info.src_ifname, PPA_IF_NAME_SIZE) ) {
				/*  upstream interfce: 2*/
				mc_group.src_ifname = cmd_info->mc_add_info.src_ifname;
			}

			/*  multicast address: 3*/
			ppa_memcpy( &mc_group.ip_mc_group, &cmd_info->mc_add_info.mc.mcast_addr, sizeof(mc_group.ip_mc_group) ) ;
			ppa_memcpy( &mc_group.source_ip, &cmd_info->mc_add_info.mc.source_ip, sizeof(mc_group.source_ip));
			mc_group.SSM_flag = cmd_info->mc_add_info.mc.SSM_flag ;
			mc_group.bridging_flag = cmd_info->mc_add_info.bridging_flag;/*  multicast mode: 4*/
			if( ppa_memcmp( cmd_info->mc_add_info.mac, ZeroMAC, PPA_ETH_ALEN ) != 0 )
				ppa_memcpy(mc_group.mac, cmd_info->mc_add_info.mac, PPA_ETH_ALEN);
				/*  multicast new mac address: 5. Note, it is not used at present. only for future*/
			if ( cmd_info->mc_add_info.mc.flags & PPA_F_SESSION_VLAN ) {
				/*  flag for modifying inner vlan*/
				mc_group.vlan_insert	= cmd_info->mc_add_info.mc.mc_extra.vlan_insert;
				mc_group.vlan_remove	= cmd_info->mc_add_info.mc.mc_extra.vlan_remove;
				mc_group.vlan_prio	= cmd_info->mc_add_info.mc.mc_extra.vlan_prio;
				mc_group.vlan_cfi	= cmd_info->mc_add_info.mc.mc_extra.vlan_cfi;
				mc_group.vlan_id	= cmd_info->mc_add_info.mc.mc_extra.vlan_id;
			}
			if ( cmd_info->mc_add_info.mc.flags & PPA_F_SESSION_OUT_VLAN ) {
				/*  flag for modifying outer vlan */
				mc_group.out_vlan_insert	= cmd_info->mc_add_info.mc.mc_extra.out_vlan_insert;
				mc_group.out_vlan_remove	= cmd_info->mc_add_info.mc.mc_extra.out_vlan_remove;
				mc_group.out_vlan_tag		= cmd_info->mc_add_info.mc.mc_extra.out_vlan_tag;
			}

			if ( cmd_info->mc_add_info.mc.flags & PPA_F_SESSION_NEW_DSCP ) {
				/*  flag for modifying dscp*/
				mc_group.new_dscp_en		= cmd_info->mc_add_info.new_dscp_en;
				mc_group.new_dscp		= cmd_info->mc_add_info.mc.mc_extra.new_dscp;
			}

			mc_group.dslwan_qid_remark = cmd_info->mc_add_info.mc.mc_extra.dslwan_qid_remark;
			mc_group.dslwan_qid = cmd_info->mc_add_info.mc.mc_extra.dslwan_qid;

			res = ppa_mc_group_update( &mc_group, cmd_info->mc_add_info.mc.flags);

			if ( res != PPA_SUCCESS ) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_mc_group_update fail\n");
				goto EXIT_EIO;
			} else
				goto EXIT_ZERO;
		}
		case PPA_CMD_GET_COUNT_PPA_HASH: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			if ( ppa_copy_from_user(&cmd_info->count_info, (void *)arg, sizeof(cmd_info->count_info)) != 0 )
				goto EXIT_EFAULT;

			get_ppa_hash_count(&cmd_info->count_info) ;
			if ( ppa_copy_to_user((void *)arg, &cmd_info->count_info, sizeof(cmd_info->count_info)) != 0 ) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_copy_to_user fail\n");
					goto EXIT_EFAULT;
			}
			goto EXIT_ZERO;
		}
		case PPA_CMD_GET_PPA_HASH_SUMMARY: {
			int i;
			uint32_t offset = (void*)(&cmd_info->ppa_hash_info.hash_info) - (void*)(&cmd_info->ppa_hash_info);

			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			if ( ppa_copy_from_user(&cmd_info->ppa_hash_info, (void *)arg, sizeof(cmd_info->ppa_hash_info)) != 0 )
				goto EXIT_EFAULT;

			for(i=0; i<cmd_info->ppa_hash_info.hash_max_num; i++) {
				cmd_info->ppa_hash_info.hash_info.count[0] = ppa_sesssion_get_count_in_hash(i);
				if ( ppa_copy_to_user(  (void *)(arg + offset + i * sizeof(cmd_info->ppa_hash_info.hash_info.count[0])),
					cmd_info->ppa_hash_info.hash_info.count,
					sizeof(cmd_info->ppa_hash_info.hash_info.count[0])) != 0 ) {
					ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_copy_to_user fail for hash index %d\n", i);
					goto EXIT_EFAULT;
				}
			}
			goto EXIT_ZERO;
		}
		case PPA_CMD_GET_COUNT_LAN_SESSION:
		case PPA_CMD_GET_COUNT_WAN_SESSION:
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
		case PPA_CMD_GET_SESSIONS_CRITERIA:
		case PPA_CMD_SWAP_SESSIONS:
#endif
		case PPA_CMD_GET_COUNT_NONE_LAN_WAN_SESSION:
		case PPA_CMD_GET_COUNT_LAN_WAN_SESSION:
		case PPA_CMD_GET_COUNT_MC_GROUP:
		case PPA_CMD_GET_COUNT_VLAN_FILTER:
		case PPA_CMD_GET_COUNT_MAC:
		case PPA_CMD_GET_HOOK_COUNT:
		{
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			if ( ppa_copy_from_user(&cmd_info->count_info, (void *)arg, sizeof(cmd_info->count_info)) != 0 )
				goto EXIT_EFAULT;

			if( cmd == PPA_CMD_GET_COUNT_LAN_SESSION )
				cmd_info->count_info.count = ppa_session_get_routing_count( 1,  cmd_info->count_info.flag, cmd_info->count_info.hash_index) ;
			else if( cmd == PPA_CMD_GET_COUNT_WAN_SESSION )
				cmd_info->count_info.count = ppa_session_get_routing_count(0,  cmd_info->count_info.flag, cmd_info->count_info.hash_index ) ;
			else if( cmd == PPA_CMD_GET_COUNT_NONE_LAN_WAN_SESSION )
				cmd_info->count_info.count = ppa_session_get_routing_count(2,  cmd_info->count_info.flag, cmd_info->count_info.hash_index ) ;
			else if( cmd == PPA_CMD_GET_COUNT_LAN_WAN_SESSION )
				cmd_info->count_info.count = ppa_session_get_routing_count( 1,  cmd_info->count_info.flag, cmd_info->count_info.hash_index)
					+ ppa_session_get_routing_count(0,  cmd_info->count_info.flag, cmd_info->count_info.hash_index );
			else if( cmd == PPA_CMD_GET_COUNT_MC_GROUP )
				cmd_info->count_info.count = ppa_get_mc_group_count(  cmd_info->count_info.flag ) ;
			else if( cmd == PPA_CMD_GET_COUNT_VLAN_FILTER)
				cmd_info->count_info.count = ppa_get_all_vlan_filter_count(  cmd_info->count_info.flag ) ;
			else if( cmd == PPA_CMD_GET_COUNT_MAC)
					cmd_info->count_info.count = ppa_get_br_count( cmd_info->count_info.flag ) ;
			else if( cmd == PPA_CMD_GET_HOOK_COUNT )
				cmd_info->count_info.count = get_ppa_hook_list_count() ;
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
			else if ( cmd == PPA_CMD_GET_SESSIONS_CRITERIA ){
				if ( ppa_get_session_limit_enable(0) ) {
					cmd_info->session_criteria_info.ppa_low_prio_data_rate = ppa_get_low_prio_data_rate(0);
					cmd_info->session_criteria_info.ppa_def_prio_data_rate = ppa_get_def_prio_data_rate(0);
					cmd_info->session_criteria_info.ppa_low_prio_thresh = ppa_get_low_prio_thresh(0);
					cmd_info->session_criteria_info.ppa_def_prio_thresh = ppa_get_def_prio_thresh(0);
				} else {
					cmd_info->session_criteria_info.ppa_low_prio_data_rate = 0;
					cmd_info->session_criteria_info.ppa_def_prio_data_rate = 0;
					cmd_info->session_criteria_info.ppa_low_prio_thresh = 0;
					cmd_info->session_criteria_info.ppa_def_prio_thresh = 0;
				}
			}
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
			else if ( cmd == PPA_CMD_SWAP_SESSIONS ) {
				ppa_swap_sessions(0);
			}
#endif
#endif
			if ( ppa_copy_to_user((void *)arg, &cmd_info->count_info, sizeof(cmd_info->count_info)) != 0 ) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_copy_to_user fail\n");
				goto EXIT_EFAULT;
			}
			goto EXIT_ZERO;
		}
		case PPA_CMD_GET_MC_GROUPS: {
			struct mc_session_node *pp_item;
			uint32_t pos = 0;
			int32_t count=0, i, index;

			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			if ( ppa_copy_from_user(&cmd_info->mc_groups.count_info, (void *)arg, sizeof(cmd_info->mc_groups.count_info)) != 0 )
				goto EXIT_EFAULT;

			if( cmd_info->mc_groups.count_info.count == 0 )
				goto EXIT_ZERO;

			if ( ppa_mc_group_start_iteration(&pos, &pp_item) == PPA_SUCCESS ) {
				count = 0;
				do {
					/*copy session information one by one to user space */
					/*pp_item->flags & SESSION_ADDED_IN_HW */
					ppa_memset(&cmd_info->mc_groups.mc_group_list, 0, sizeof(cmd_info->mc_groups.mc_group_list) );
#if defined(RTP_SAMPLING_ENABLE) && RTP_SAMPLING_ENABLE
					if(pp_item->RTP_flag == 1) {
						if ( ppa_hw_get_mc_rtp_sampling_cnt( pp_item ) != PPA_SUCCESS ) {
							ppa_mc_group_stop_iteration();
							goto EXIT_EIO;
						}
					} else{
						cmd_info->mc_groups.mc_group_list[0].mc.rtp_pkt_cnt=0;
						cmd_info->mc_groups.mc_group_list[0].mc.rtp_seq_num=0;
					}
					ppa_memcpy(&cmd_info->mc_groups.mc_group_list[0].mc.RTP_flag , &pp_item->RTP_flag,
						sizeof(cmd_info->mc_groups.mc_group_list[0].mc.RTP_flag));
					ppa_memcpy(&cmd_info->mc_groups.mc_group_list[0].mc.rtp_pkt_cnt , &pp_item->rtp_pkt_cnt,
						sizeof(cmd_info->mc_groups.mc_group_list[0].mc.rtp_pkt_cnt));
					ppa_memcpy(&cmd_info->mc_groups.mc_group_list[0].mc.rtp_seq_num , &pp_item->rtp_seq_num,
						sizeof(cmd_info->mc_groups.mc_group_list[0].mc.rtp_seq_num));

#endif
					ppa_memcpy( &cmd_info->mc_groups.mc_group_list[0].mc.mcast_addr, &pp_item->grp.ip_mc_group,
						sizeof(cmd_info->mc_groups.mc_group_list[0].mc.mcast_addr));
					ppa_memcpy( &cmd_info->mc_groups.mc_group_list[0].mc.source_ip, &pp_item->grp.source_ip,
						sizeof(cmd_info->mc_groups.mc_group_list[0].mc.source_ip));
					cmd_info->mc_groups.mc_group_list[0].mc.SSM_flag = pp_item->SSM_flag;
					cmd_info->mc_groups.mc_group_list[0].num_ifs = pp_item->grp.num_ifs;


					for (i = 0; i < PPA_MAX_MC_IFS_NUM; i++) {
						if (pp_item->grp.txif[i].netif)
							ppa_strncpy(cmd_info->mc_groups.mc_group_list[0].lan_ifname[i],
								(void *)ppa_get_netif_name(pp_item->grp.txif[i].netif),
								sizeof(cmd_info->mc_groups.mc_group_list[0].lan_ifname[i]) - 1);
					}

					if( pp_item->grp.src_netif ) {
						ppa_strncpy( cmd_info->mc_groups.mc_group_list[0].src_ifname,
						(void *)ppa_get_netif_name(pp_item->grp.src_netif),
						sizeof(cmd_info->mc_groups.mc_group_list[0].src_ifname) -1 );
					}

					cmd_info->mc_groups.mc_group_list[0].mc.mc_extra.new_dscp = pp_item->grp.new_dscp;

					cmd_info->mc_groups.mc_group_list[0].mc.mc_extra.vlan_id = pp_item->grp.new_vci &0xFFF;
					cmd_info->mc_groups.mc_group_list[0].mc.mc_extra.vlan_prio = ( pp_item->grp.new_vci >>13 )& 0x7;
					cmd_info->mc_groups.mc_group_list[0].mc.mc_extra.vlan_cfi =( pp_item->grp.new_vci >>12 )& 0x1;
					cmd_info->mc_groups.mc_group_list[0].mc.mc_extra.out_vlan_tag = pp_item->grp.out_vlan_tag;
					cmd_info->mc_groups.mc_group_list[0].mc.mc_extra.dslwan_qid= pp_item->dslwan_qid;
					cmd_info->mc_groups.mc_group_list[0].mc.flags= pp_item->flags;
					cmd_info->mc_groups.mc_group_list[0].mc.hw_bytes = pp_item->acc_bytes - pp_item->prev_clear_acc_bytes;
					cmd_info->mc_groups.mc_group_list[0].bridging_flag= pp_item->bridging_flag;

					index = (void *)cmd_info->mc_groups.mc_group_list - (void *)&cmd_info->mc_groups.count_info +
							count * sizeof(cmd_info->mc_groups.mc_group_list[0]);

					if ( ppa_copy_to_user(  (void *)(arg + index) , cmd_info->mc_groups.mc_group_list,
						sizeof(cmd_info->mc_groups.mc_group_list[0])) != 0 ) {
						ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
							"ppa_copy_to_user fail during copy_to_user for mc: count=%d index=%d\n", count, index);
						ppa_mc_group_stop_iteration();
						goto EXIT_EFAULT;
					}

					count ++;
				} while ( (ppa_mc_group_iterate_next(&pos, &pp_item) == PPA_SUCCESS ) && (count < cmd_info->mc_groups.count_info.count) );
			}

			cmd_info->mc_groups.count_info.count = count;  /*update the real session number to user space */
			ppa_mc_group_stop_iteration();

			if ( ppa_copy_to_user(  (void *)arg, &cmd_info->mc_groups.count_info, sizeof(cmd_info->mc_groups.count_info)) != 0 ) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_copy_to_user fail during copy_to_user for mc counter_info\n");
				goto EXIT_EFAULT;
			}

			goto EXIT_ZERO;
		}
		case PPA_CMD_GET_LAN_SESSIONS:
		case PPA_CMD_GET_WAN_SESSIONS:
		case PPA_CMD_GET_LAN_WAN_SESSIONS: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_sessions(cmd, &cmd_info->session_info, arg);

			goto EXIT_LAST;
		}
		case PPA_CMD_GET_ALL_MAC: {
			struct br_mac_node *pp_item;
			int32_t count = 0;
			uint32_t pos = 0;
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;

			if ( ppa_copy_from_user(&cmd_info->all_br_info.count_info, (void *)arg, sizeof(cmd_info->all_br_info.count_info)) != 0 )
				goto EXIT_EFAULT;


			if ( ppa_bridging_session_start_iteration(&pos, &pp_item) == PPA_SUCCESS && cmd_info->all_br_info.count_info.count ) {
				do {
					ppa_memcpy( cmd_info->all_br_info.session_list[0].mac_addr, pp_item->mac, PPA_ETH_ALEN);
					ppa_strncpy( cmd_info->all_br_info.session_list[0].ifname, ppa_get_netif_name(pp_item->netif),
						sizeof(cmd_info->all_br_info.session_list[0].ifname) );
					cmd_info->all_br_info.session_list[0].flags = pp_item->flags;

					if ( ppa_copy_to_user( ( (void *)arg) + ( (void *)cmd_info->all_br_info.session_list  -
						(void *)&cmd_info->all_br_info.count_info) + count * sizeof(cmd_info->all_br_info.session_list[0]) ,
						cmd_info->all_br_info.session_list, sizeof(cmd_info->all_br_info.session_list[0])) != 0 ) {
							ppa_bridging_session_stop_iteration();
							goto EXIT_EFAULT;
						}
					count ++;

					if( count == cmd_info->all_br_info.count_info.count ) break;  /*buffer is full. We should stop now */
				} while ( ppa_bridging_session_iterate_next(&pos, &pp_item) == PPA_SUCCESS );
			}

			ppa_bridging_session_stop_iteration();
			cmd_info->all_br_info.count_info.count = count;  /*update the real session number to user space */
			if ( ppa_copy_to_user(  (void *)arg, &cmd_info->all_br_info.count_info, sizeof(cmd_info->all_br_info.count_info)) != 0 ) {
				goto EXIT_EFAULT;
			}

			goto EXIT_ZERO;
		}
		case PPA_CMD_GET_VERSION: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			ppa_memset(&cmd_info->ver, 0, sizeof(cmd_info->ver) );
			ppa_get_api_id(&cmd_info->ver.ppa_api_ver.family, &cmd_info->ver.ppa_api_ver.type,
				&cmd_info->ver.ppa_api_ver.itf, &cmd_info->ver.ppa_api_ver.mode, &cmd_info->ver.ppa_api_ver.major,
				&cmd_info->ver.ppa_api_ver.mid, &cmd_info->ver.ppa_api_ver.minor);
			ppa_get_stack_al_id(&cmd_info->ver.ppa_stack_al_ver.family, &cmd_info->ver.ppa_stack_al_ver.type,
				&cmd_info->ver.ppa_stack_al_ver.itf, &cmd_info->ver.ppa_stack_al_ver.mode,
				&cmd_info->ver.ppa_stack_al_ver.major, &cmd_info->ver.ppa_stack_al_ver.mid, &cmd_info->ver.ppa_stack_al_ver.minor);

			if( ppa_drv_get_hal_id(&cmd_info->ver.ppe_hal_ver,0) == PPA_SUCCESS) {
				cmd_info->ver.ppe_fw_ver[0].index = 0;
				cmd_info->ver.ppe_fw_ver[1].index = 1;
				ppa_drv_get_firmware_id(&cmd_info->ver.ppe_fw_ver[0], 0);
				ppa_drv_get_firmware_id(&cmd_info->ver.ppe_fw_ver[1], 0);
			}
			ppa_subsystem_id(&cmd_info->ver.ppa_subsys_ver.family, &cmd_info->ver.ppa_subsys_ver.type, &cmd_info->ver.ppa_subsys_ver.itf,
				&cmd_info->ver.ppa_subsys_ver.mode, &cmd_info->ver.ppa_subsys_ver.major, &cmd_info->ver.ppa_subsys_ver.mid,
				&cmd_info->ver.ppa_subsys_ver.minor, &cmd_info->ver.ppa_subsys_ver.tag);
#if IS_ENABLED(CONFIG_IPV6)
			cmd_info->ver.ppa_feature.ipv6_en = 1;
#endif
#if IS_ENABLED(CONFIG_PPA_QOS)
			cmd_info->ver.ppa_feature.qos_en = 1;
#endif

			if ( ppa_copy_to_user(  (void *)arg, &cmd_info->ver, sizeof(cmd_info->ver)) != 0 )
				goto EXIT_EFAULT;
			goto EXIT_ZERO;
		}
		case PPA_CMD_GET_SIZE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			ppa_memset(&cmd_info->size_info, 0, sizeof(cmd_info->size_info) );
			cmd_info->size_info.rout_session_size = sizeof(struct uc_session_node);
			cmd_info->size_info.mc_session_size= sizeof(struct mc_session_node);
			cmd_info->size_info.br_session_size= sizeof(struct br_mac_node);
			cmd_info->size_info.netif_size= sizeof(struct netif_info);

			if ( ppa_copy_to_user(  (void *)arg, &cmd_info->size_info, sizeof(cmd_info->size_info)) != 0 )
				goto EXIT_EFAULT;
			goto EXIT_ZERO;
		}
		case PPA_CMD_BRIDGE_ENABLE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_bridge_enable(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_BRIDGE_STATUS: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_bridge_enable_status(cmd, arg, cmd_info );
			break;
		}
#if IS_ENABLED(CONFIG_SOC_GRX500)
		case PPA_CMD_GET_BRIDGE_FID: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_bridge_fid(cmd, arg, cmd_info);
			break;
		}
#endif
#if IS_ENABLED(CONFIG_PPA_QOS)
		case PPA_CMD_GET_QOS_STATUS: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_qos_status(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_QOS_QUEUE_MAX_NUM: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_qos_qnum(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_QOS_MIB: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_qos_mib(cmd, arg, cmd_info );
			break;
		}
#if !defined(CONFIG_PPA_PUMA7)
		case PPA_CMD_SET_CTRL_QOS_WFQ: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_ctrl_qos_wfq(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_CTRL_QOS_WFQ: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_ctrl_qos_wfq(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_SET_QOS_WFQ: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_qos_wfq(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_RESET_QOS_WFQ: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_reset_qos_wfq(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_QOS_WFQ:{
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_qos_wfq(cmd, arg, cmd_info );
			break;
		}
#endif //end of !PUMA7
		case PPA_CMD_SET_CTRL_QOS_RATE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_ctrl_qos_rate(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_CTRL_QOS_RATE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_ctrl_qos_rate(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_SET_QOS_RATE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_qos_rate(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_RESET_QOS_RATE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_reset_qos_rate(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_QOS_RATE: {
			res = ppa_ioctl_get_qos_rate(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_ADD_QOS_QUEUE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_add_qos_queue(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_MOD_QOS_QUEUE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_modify_qos_queue(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_DEL_QOS_QUEUE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_delete_qos_queue(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_ENG_QUEUE_INIT: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_qos_init_cfg(cmd);
			break;
		}
		case PPA_CMD_ENG_QUEUE_UNINIT: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_qos_init_cfg(cmd);
			break;
		}
#if IS_ENABLED(CONFIG_SOC_GRX500)
		case PPA_CMD_QOS_DSCP_CLASS_SET: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_qos_init_cfg(cmd);
			break;
		}
		case PPA_CMD_QOS_DSCP_CLASS_RESET: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_qos_init_cfg(cmd);
			break;
		}
#endif
		case PPA_CMD_MOD_SUBIF_PORT: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_mod_subif_port_config(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_ADD_WMM_QOS_QUEUE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_add_wmm_qos_queue(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_DEL_WMM_QOS_QUEUE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_delete_wmm_qos_queue(cmd, arg, cmd_info );
			break;
		}
#if IS_ENABLED(CONFIG_SOC_GRX500)
		case PPA_CMD_SET_QOS_INGGRP: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_qos_ingress_group(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_QOS_INGGRP: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_qos_ingress_group(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_ADD_CLASSIFIER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_add_class_rule(cmd , arg, cmd_info);
			break;
		}
		case PPA_CMD_MOD_CLASSIFIER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_mod_class_rule(cmd , arg, cmd_info);
			break;
		}
		case PPA_CMD_GET_CLASSIFIER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_class_rule(cmd , arg, cmd_info);
			break;
		}
		case PPA_CMD_DEL_CLASSIFIER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_del_class_rule(cmd , arg, cmd_info);
			break;
		}
#endif
		case PPA_CMD_SET_QOS_SHAPER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_qos_shaper(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_QOS_SHAPER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_qos_shaper(cmd, arg, cmd_info );
			break;
		}
#endif //end of CONFIG_PPA_QOS
		case  PPA_CMD_GET_HOOK_LIST: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_hook_list(cmd, arg, cmd_info );
			break;
		}
		case  PPA_CMD_SET_HOOK: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_hook(cmd, arg, cmd_info );
			break;
		}
		case  PPA_CMD_READ_MEM: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_read_mem(cmd, arg, cmd_info );
			break;
		}
		case  PPA_CMD_SET_MEM: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_mem(cmd, arg, cmd_info );
			break;
		}
		case  PPA_CMD_GET_MAX_ENTRY:{
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_max_entry(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_PORTID: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_portid(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_DSL_MIB: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_dsl_mib(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_CLEAR_DSL_MIB: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_clear_dsl_mib(cmd, arg, cmd_info );
			break;
		}
#if IS_ENABLED(CONFIG_IPV4_IPV6_COUNTER_SUPPORT)
		case PPA_CMD_GET_IFACE_MIB: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_iface_mib(cmd, arg, cmd_info );
			break;
		}
#endif /* CONFIG_IPV4_IPV6_COUNTER_SUPPORT */
		case PPA_CMD_DEL_SESSION: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_del_session(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_ADD_SESSION: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_add_session(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_MODIFY_SESSION: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_modify_session(cmd, arg, cmd_info );
			break;
		}
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
		case PPA_CMD_MANAGE_SESSIONS: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_manage_sessions(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_SESSIONS: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_prio_sessions(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_SESSIONS_COUNT: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_sessions_count(cmd, arg, cmd_info );
			break;
		}
#endif
		case PPA_CMD_SET_SESSION_TIMER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_session_timer(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_SESSION_TIMER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_session_timer(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_PORT_MIB: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_ports_mib(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_CLEAR_PORT_MIB: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			if ( ppa_copy_from_user(&cmd_info->port_mib_info, (void *)arg, sizeof(cmd_info->port_mib_info)) != 0 )
				goto EXIT_EFAULT;
			res = ppa_ioctl_clear_ports_mib(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_SET_HAL_DBG_FLAG: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			if ( ppa_copy_from_user(&cmd_info->genernal_enable_info, (void *)arg, sizeof(cmd_info->genernal_enable_info)) != 0 )
				goto EXIT_EFAULT;
			ppa_drv_set_hal_dbg(&cmd_info->genernal_enable_info,0);
			break;
		}
		case PPA_CMD_DBG_TOOL: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_dbg_tool_test(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_SET_VALUE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_value(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_VALUE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_value(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_SET_PPA_FASTPATH_ENABLE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_ppe_fastpath_enable(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_PPA_FASTPATH_ENABLE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_ppe_fastpath_enable(cmd, arg, cmd_info );
			break;
		}
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
		case PPA_CMD_SET_SW_FASTPATH_ENABLE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_sw_fastpath_enable(cmd, arg, cmd_info );
				break;
		}
		case PPA_CMD_GET_SW_FASTPATH_ENABLE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_sw_fastpath_enable(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_SET_SW_SESSION_ENABLE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_sw_session_enable(cmd, arg, cmd_info );
			break;
		}
		case PPA_CMD_GET_SW_SESSION_ENABLE: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_get_sw_session_enable(cmd, arg, cmd_info );
			break;
		}
#endif
#if IS_ENABLED(CONFIG_SOC_GRX500)
#if IS_ENABLED(CONFIG_PPA_QOS)
		case PPA_CMD_SET_QOS_METER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_set_qos_meter(cmd, arg, cmd_info);
			break;
		}
		case PPA_CMD_ADD_QOS_METER: {
			if (!capable(CAP_NET_ADMIN))
				goto EXIT_EFAULT;
			res = ppa_ioctl_add_qos_meter(cmd, arg, cmd_info);
			break;
		}
#endif
#endif
		default:
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"cmd not supported:0x%0x(its nr value=0x%0x)\n", cmd, ppa_ioc_nr(cmd) );
			goto EXIT_ENOIOCTLCMD;
	}
	if ( res != PPA_SUCCESS ) {
		goto EXIT_EIO;
	} else
		goto EXIT_ZERO;

EXIT_EIO:
	if (res != -EINVAL)
		res = -EIO;
	goto EXIT_LAST;

EXIT_EFAULT:
	res = -EFAULT;
	goto EXIT_LAST;
EXIT_ZERO:
	res = 0;
	goto EXIT_LAST;
EXIT_ENOIOCTLCMD:
	res = ENOIOCTLCMD;
	goto EXIT_LAST;
EXIT_LAST:
	if( cmd_info )
		ppa_free(cmd_info);
	return res;
}

static int32_t ppa_get_all_vlan_filter_count(uint32_t count_flag)
{
	uint32_t  i, j;
	int32_t count = 0;
	uint32_t vlan_filter_type[] = {PPA_F_VLAN_FILTER_IFNAME, PPA_F_VLAN_FILTER_IP_SRC,
		PPA_F_VLAN_FILTER_ETH_PROTO, PPA_F_VLAN_FILTER_VLAN_TAG};
	PPA_VFILTER_COUNT_CFG vfilter_count={0};
	PPA_BRDG_VLAN_FILTER_MAP vlan_map={0};

	count = 0;
	for ( i = 0; i < NUM_ENTITY(vlan_filter_type); i++ ) {
		vfilter_count.vfitler_type = vlan_filter_type[i];
		ppa_drv_get_max_vfilter_entries(&vfilter_count, 0);

		for ( j = 0; j < vfilter_count.num; j++ ) {
			vlan_map.ig_criteria_type = vlan_filter_type[i];
			vlan_map.entry = j;

			if ( ppa_drv_get_vlan_map( &vlan_map , 0)  == PPA_SUCCESS ) {
				count++;
			}
		}
	}
	return  count;
}

/* since from no where calling following functions, commented*/
#if 0
#ifdef PPA_API_PROC
static int print_ppa_subsystem_ver(char *buf, int buf_len, char *title, unsigned int family, unsigned int type,
	unsigned int itf, unsigned int mode, unsigned int major, unsigned int mid, 
	unsigned int minor, unsigned int tag)
{
	int len = 0;
	len += ppa_snprintf(buf + len, buf_len - len, "%s:\n", title);
	len += ppa_snprintf(buf + len, buf_len - len, "  Version ID: %d.%d.%d-%d\n", major, mid, minor, tag);
	return len;
}
static int print_driver_ver(char *buf, int buf_len, char *title, unsigned int family, unsigned int type,
	unsigned int itf, unsigned int mode, unsigned int major, unsigned int mid, unsigned int minor)
{
	static char * dr_ver_family_str[] = {
		NULL,
		"Danube",
		"Twinpass",
		"Amazon-SE",
		NULL,
		"AR9",
		"GR9",
		"VR9"
		"GRX500"
	};
	static char * dr_ver_type_str[] = {
		"Normal Data Path",
		"Indirect-Fast Path",
		"HAL",
		"Hook",
		"OS Adatpion Layer",
		"PPA API"
	};
	static char * dr_ver_interface_str[] = {
		"MII0",
		"MII1",
		"ATM",
		"PTM"
	};
	static char * dr_ver_accmode_str[] = {
		"Routing",
		"Bridging",
	};

	int len = 0;
	unsigned int bit;
	int i, j;

	len += ppa_snprintf(buf + len, buf_len - len, "%s:\n", title);
	len += ppa_snprintf(buf + len, buf_len - len, "  Version ID: %d.%d.%d.%d.%d.%d.%d\n", family, type, itf, mode, major, mid, minor);
	len += ppa_snprintf(buf + len, buf_len - len, "  Family	: ");
	for ( bit = family, i = j = 0; bit != 0 && i < NUM_ENTITY(dr_ver_family_str); bit >>= 1, i++ )
		if ( (bit & 0x01) && dr_ver_family_str[i] != NULL ) {
			if ( j )
				len += ppa_snprintf(buf + len, buf_len - len, " | %s", dr_ver_family_str[i]);
			else
				len += ppa_snprintf(buf + len, buf_len - len, dr_ver_family_str[i]);
			j++;
		}
	if ( j )
		len += ppa_snprintf(buf + len, buf_len - len, "\n");
	else
		len += ppa_snprintf(buf + len, buf_len - len, "N/A\n");
	len += ppa_snprintf(buf + len, buf_len - len, "  DR Type   : ");
	for ( bit = type, i = j = 0; bit != 0 && i < NUM_ENTITY(dr_ver_type_str); bit >>= 1, i++ )
		if ( (bit & 0x01) && dr_ver_type_str[i] != NULL ) {
			if ( j )
				len += ppa_snprintf(buf + len, buf_len - len, " | %s", dr_ver_type_str[i]);
			else
				len += ppa_snprintf(buf + len, buf_len - len, dr_ver_type_str[i]);
			j++;
		}
	if ( j )
		len += ppa_snprintf(buf + len, buf_len - len, "\n");
	else
		len += ppa_snprintf(buf + len, buf_len - len, "N/A\n");
	len += ppa_snprintf(buf + len, buf_len - len, "  Interface : ");
	for ( bit = itf, i = j = 0; bit != 0 && i < NUM_ENTITY(dr_ver_interface_str); bit >>= 1, i++ )
		if ( (bit & 0x01) && dr_ver_interface_str[i] != NULL ) {
			if ( j )
				len += ppa_snprintf(buf + len, buf_len - len, " | %s", dr_ver_interface_str[i]);
			else
				len += ppa_snprintf(buf + len, buf_len - len, dr_ver_interface_str[i]);
			j++;
		}
	if ( j )
		len += ppa_snprintf(buf + len, buf_len - len, "\n");
	else
		len += ppa_snprintf(buf + len, buf_len - len, "N/A\n");
	len += ppa_snprintf(buf + len, buf_len - len, "  Mode	  : ");
	for ( bit = mode, i = j = 0; bit != 0 && i < NUM_ENTITY(dr_ver_accmode_str); bit >>= 1, i++ )
		if ( (bit & 0x01) && dr_ver_accmode_str[i] != NULL ) {
			if ( j )
				len += ppa_snprintf(buf + len, buf_len - len, " | %s", dr_ver_accmode_str[i]);
			else
				len += ppa_snprintf(buf + len, buf_len - len, dr_ver_accmode_str[i]);
			j++;
		}
	if ( j )
		len += ppa_snprintf(buf + len, buf_len - len, "\n");
	else
		len += ppa_snprintf(buf + len, buf_len - len, "N/A\n");
	len += ppa_snprintf(buf + len, buf_len - len, "  Release   : %d.%d.%d\n", major, mid, minor);

	return len;
}

static int proc_read_ppa_ver_open(struct inode *inode, struct file *file)
{
	return single_open(file, &proc_read_ver, NULL);
}

static struct file_operations g_proc_file_ppa_ver_seq_fops = {
	.owner		= THIS_MODULE,
	.open		= proc_read_ppa_ver_open,
	.read		= seq_read,
	.llseek		= seq_lseek,
	.release	= single_release,
};
static void proc_file_create(void)
{
	struct proc_dir_entry *res;

	if ( !g_ppa_proc_dir_flag ) {
		g_ppa_proc_dir = proc_mkdir("ppa", NULL);
		g_ppa_proc_dir_flag = 1;
	}

	if ( !g_ppa_api_proc_dir_flag ) {
		g_ppa_api_proc_dir = proc_mkdir("api", g_ppa_proc_dir);
		g_ppa_api_proc_dir_flag = 1;
	}
	res = proc_create("ver", 0600,
			g_ppa_api_proc_dir,
			&g_proc_file_ppa_ver_seq_fops);
}

static void proc_file_delete(void)
{

	remove_proc_entry("ver",
			g_ppa_api_proc_dir);

	if ( g_ppa_api_proc_dir_flag ) {
		remove_proc_entry("api",
				g_ppa_proc_dir);
		g_ppa_api_proc_dir = NULL;
		g_ppa_api_proc_dir_flag = 0;
	}

	if ( g_ppa_proc_dir_flag ) {
		remove_proc_entry("ppa", NULL);
		g_ppa_proc_dir = NULL;
		g_ppa_proc_dir_flag = 0;
	}
}
static int proc_read_ver(struct seq_file *seq, void *v)
{
	PPA_VERSION ver={0};
	int len = 0, count = 1024, off = 0;
	char buf[1024]={0};
	char *page = buf;

	if (!capable(CAP_NET_ADMIN)) {
		printk ("Read Permission denied");
		return 0;
	}
	ppa_subsystem_id(&ver.family, &ver.type, &ver.itf, &ver.mode, &ver.major, &ver.mid, &ver.minor, &ver.tag);
	len += print_ppa_subsystem_ver(page + off + len, count - len, "PPA Sub System info",
		ver.family, ver.type, ver.itf, ver.mode, ver.major, ver.mid, ver.minor, ver.tag);
	if (len) { seq_printf(seq, " %s\n", buf); ppa_memset(page,0,1024); len=0; }
	ppa_get_api_id(&ver.family, &ver.type, &ver.itf, &ver.mode, &ver.major, &ver.mid, &ver.minor);
	len += print_driver_ver(page + off + len, count - len, "PPA API driver info", ver.family, 
		ver.type, ver.itf, ver.mode, ver.major, ver.mid, ver.minor);
	if (len) { seq_printf(seq, " %s\n", buf); ppa_memset(page,0,1024); len=0; }
	ppa_get_stack_al_id(&ver.family, &ver.type, &ver.itf, &ver.mode, &ver.major, &ver.mid, &ver.minor);
	len += print_driver_ver(page + off + len, count - len, "PPA Stack Adaption Layer driver info",
		ver.family, ver.type, ver.itf, ver.mode, ver.major, ver.mid, ver.minor);
	if (len) { seq_printf(seq, " %s\n", buf); ppa_memset(page,0,1024); len=0; }
	if( ppa_drv_get_hal_id(&ver,0) == PPA_SUCCESS) {
		len += print_driver_ver(page + off + len, count - len, "PPE HAL driver info", ver.family,
			ver.type, ver.itf, ver.mode, ver.major, ver.mid, ver.minor);
		if (len) { seq_printf(seq, " %s\n", buf); ppa_memset(page,0,1024); len=0; }
		ppa_memset(&ver,0,sizeof(ver));
		ver.index = 0;
		ppa_drv_get_firmware_id(&ver, 0);
		len += print_driver_ver(page + off + len, count - len, "PPE HAL driver info", ver.family,
			ver.type, ver.itf, ver.mode, ver.major, ver.mid, ver.minor);
		if (len) { seq_printf(seq, " %s\n", buf); ppa_memset(page,0,1024); len=0; }
		ppa_memset(&ver,0,sizeof(ver));
		ver.index = 1;
		ppa_drv_get_firmware_id(&ver, 0);
		len += print_driver_ver(page + off + len, count - len, "PPE HAL driver info", ver.family,
			ver.type, ver.itf, ver.mode, ver.major, ver.mid, ver.minor);
		if (len) { seq_printf(seq, " %s\n", buf); ppa_memset(page,0,1024); len=0; }
	}
	return len;
}
#endif
#endif

int32_t ppa_ioctl_bridge_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING)
	int res=PPA_FAILURE;

	res = copy_from_user( &cmd_info->br_enable_info, (void *)arg, sizeof(cmd_info->br_enable_info));
	if ( res != PPA_SUCCESS )
		return PPA_FAILURE;

	res = ppa_hook_bridge_enable( cmd_info->br_enable_info.bridge_enable, cmd_info->br_enable_info.flags);

	return res;
#endif /*CONFIG_PPA_BR_MAC_LEARNING*/
	return PPA_SUCCESS;
}

int32_t ppa_ioctl_get_bridge_enable_status(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING)
	int res=PPA_FAILURE;

	ppa_memset(&cmd_info->br_enable_info, 0, sizeof(cmd_info->br_enable_info) );

	res = copy_from_user( &cmd_info->br_enable_info, (void *)arg, sizeof(cmd_info->br_enable_info));
	if ( res != PPA_SUCCESS )
		return PPA_FAILURE;

	res = ppa_hook_get_bridge_status( &cmd_info->br_enable_info.bridge_enable, cmd_info->br_enable_info.flags);

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->br_enable_info, sizeof(cmd_info->br_enable_info)) != 0 )
		return PPA_FAILURE;

	return res;
#endif /*CONFIG_PPA_BR_MAC_LEARNING*/
	return PPA_SUCCESS;
}

#if IS_ENABLED(CONFIG_SOC_GRX500)
/* Returns PPA_SUCCESS only if interface is a bridge */
static int ppa_is_bridge_valid(char *ifname)
{
	PPA_NETIF *dev = ppa_dev_get_by_name(ifname);

	if (!dev)
		return PPA_FAILURE; /* Interface doesnt exists */

	if (dev->priv_flags & IFF_EBRIDGE) {
		ppa_put_netif(dev);
		return PPA_SUCCESS; /* Interface is a bridge */
	}
	ppa_put_netif(dev);
	return PPA_FAILURE; /* Interface is not a bridge */
}

int32_t ppa_ioctl_get_bridge_fid(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int res = PPA_FAILURE;
	uint16_t fid = 0xFFFF;

	ppa_memset(&cmd_info->br_fid_info, 0, sizeof(PPA_CMD_BRIDGE_FID_INFO));

	res = copy_from_user(&cmd_info->br_fid_info, (void *)arg, sizeof(PPA_CMD_BRIDGE_FID_INFO));
	if(res != PPA_SUCCESS) {
		return PPA_FAILURE;
	}
	
	res = ppa_is_bridge_valid(cmd_info->br_fid_info.ifname);
	if (res != PPA_SUCCESS) {
		cmd_info->br_fid_info.fid = fid; //error
		goto COPY_TO_USER;
	}

	res = ppa_get_fid(cmd_info->br_fid_info.ifname, &fid);
	if (res != PPA_SUCCESS) {
		return PPA_FAILURE;
	}

	cmd_info->br_fid_info.fid = fid;

COPY_TO_USER:
	res = ppa_copy_to_user((void *)arg, &cmd_info->br_fid_info, sizeof(PPA_CMD_BRIDGE_FID_INFO));
	if (res != PPA_SUCCESS) {
		return PPA_FAILURE;
	}
	return res;
}
#endif /* CONFIG_SOC_GRX500 */

int32_t ppa_ioctl_get_max_entry(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int res=PPA_FAILURE;

	ppa_memset(&cmd_info->acc_entry_info, 0, sizeof(cmd_info->acc_entry_info) );

	res = copy_from_user( &cmd_info->acc_entry_info, (void *)arg, sizeof(cmd_info->acc_entry_info));
	if ( res != PPA_SUCCESS )
		return PPA_FAILURE;

	res = ppa_get_max_entries( &cmd_info->acc_entry_info.entries, cmd_info->acc_entry_info.flags);

	if ( res != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_ioctl_get_max_entry fail\n");
		res = PPA_FAILURE;
	}
	if ( ppa_copy_to_user( (void *)arg, &cmd_info->acc_entry_info, sizeof(cmd_info->acc_entry_info)) != 0 )
		return PPA_FAILURE;

	return res;
}

int32_t ppa_ioctl_get_portid(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int res=PPA_FAILURE;
	struct netif_info *rx_ifinfo=NULL;

	ppa_memset(&cmd_info->portid_info, 0, sizeof(cmd_info->portid_info) );

	res = copy_from_user( &cmd_info->portid_info, (void *)arg, sizeof(cmd_info->portid_info));
	if ( res != PPA_SUCCESS )
		return PPA_FAILURE;

	if (ppa_get_netif_info(cmd_info->portid_info.ifname, &rx_ifinfo) != PPA_SUCCESS ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"failed in getting info structure of ifname (%s)\n", cmd_info->portid_info.ifname );

		return PPA_FAILURE;
	}
	cmd_info->portid_info.portid = rx_ifinfo->phys_port;

	ppa_netif_put(rx_ifinfo);

	res = PPA_SUCCESS;

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->portid_info, sizeof(cmd_info->portid_info)) != 0 )
		return PPA_FAILURE;

	return res;
}

int32_t ppa_ioctl_del_session(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	/*only IP address based session delete is supported*/
	PPA_CMD_DEL_SESSION_INFO *pSessInfo;

	pSessInfo = &cmd_info->del_session;

	if ( ppa_copy_from_user(pSessInfo, (void *)arg, sizeof(cmd_info->del_session)) != 0 ) {
		return PPA_FAILURE;
	}
	if(pSessInfo->type == PPA_SESS_DEL_USING_MAC) {
		ppa_session_delete_by_macaddr(pSessInfo->u.mac_addr);
	} else {
		ppa_session_delete_by_ip(&(pSessInfo->u.ip));
	}

	return PPA_SUCCESS;
}


int32_t ppa_ioctl_add_session(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	printk("This ioctl is no more supported. No Session will be added/deleted from user space\n");
	return PPA_FAILURE;
}

static void __ppa_ioctl_modify_session(
	PPA_CMD_SESSION_EXTRA_ENTRY *pSessExtraInfo,
	struct uc_session_node *p_item)
{
	/*below code copy from ppa_session_modify ---start*/
	ppa_update_session_extra(&pSessExtraInfo->session_extra, p_item,
			pSessExtraInfo->flags);

	if( pSessExtraInfo->flags & PPA_F_ACCEL_MODE) {
		/* Only need to remove or add to accleration path and no other
		   information need to update */

		if (p_item->flags & SESSION_ADDED_IN_HW && pSessExtraInfo->session_extra.accel_enable == 0) {
			/* just remove the accelerated session from PPE FW.*/
			ppa_hsel_del_routing_session(p_item);
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH) && defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
			update_session_mgmt_stats(p_item, ADD);
#endif
		}
	} else if ( pSessExtraInfo->flags != 0) {
		/* update parameters to PPE FW SESSION TABLE */
		if( ppa_hw_update_session_extra(p_item, pSessExtraInfo->flags) != PPA_SUCCESS ) {

			/*  update failed*/
			ppa_hsel_del_routing_session(p_item);
#if defined(CONFIG_PPA_API_SW_FASTPATH) && defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
			update_session_mgmt_stats(p_item, ADD);
#endif
		}
	}
	/*below code copy from ppa_session_modify ---end*/
}

int32_t ppa_ioctl_modify_session(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	struct uc_session_node *p_item=NULL;
	struct uc_session_node *p_r_item=NULL;
	PPA_CMD_SESSION_EXTRA_ENTRY *pSessExtraInfo;
	uint32_t idx;

	pSessExtraInfo = &cmd_info->session_extra_info;

	if ( ppa_copy_from_user(pSessExtraInfo, (void *)arg, sizeof(cmd_info->session_extra_info)) != 0 ) {
		return PPA_FAILURE;
	}

	if( pSessExtraInfo->flags == 0 || pSessExtraInfo->session == 0)
		return PPA_SUCCESS; /*no information need to update */

	if( PPA_SESSION_EXISTS ==
			__ppa_session_find_ct_hash((PPA_SESSION*)(pSessExtraInfo->session),
				pSessExtraInfo->hash, &p_item) ) {
		int8_t r_direction = -1;

		ppa_debug( DBG_ENABLE_MASK_DEBUG_PRINT,
				"ConnTrack matched %px match with %px\n",
				(void *)pSessExtraInfo->session, (void *)p_item->session );

		/* Check if other direction need to be modified */
		if( p_item ) {

			if( p_item->flags & SESSION_WAN_ENTRY &&
					(pSessExtraInfo->lan_wan_flags & SESSION_LAN_ENTRY )) {
				r_direction = 1;
			} else if( p_item->flags & SESSION_LAN_ENTRY &&
					pSessExtraInfo->lan_wan_flags & SESSION_WAN_ENTRY ) {
				r_direction = 1;
			}
		}

		if( r_direction != -1 ) {
			__ppa_session_find_by_ct(p_item->session,
					(p_item->flags & SESSION_IS_REPLY)?0:1,
					&p_r_item);
		}

		if( p_item ) {
			/*  Modify and put session */
			idx = ppa_session_get_index(p_item->hash);
			ppa_session_bucket_lock(idx);
			__ppa_ioctl_modify_session(pSessExtraInfo,p_item);
			__ppa_session_put(p_item);
			ppa_session_bucket_unlock(idx);
		}

		if( p_r_item ) {
			/*  Modify and put session */
			idx = ppa_session_get_index(p_r_item->hash);
			ppa_session_bucket_lock(idx);
			__ppa_ioctl_modify_session(pSessExtraInfo,p_r_item);
			__ppa_session_put(p_r_item);
			ppa_session_bucket_unlock(idx);
		}
	}
	else {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
			"ppa_ioctl_modify_session: Invalid session\n");
		return PPA_FAILURE;
	}
	return PPA_SUCCESS;
}

#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
int32_t ppa_ioctl_get_sessions_count(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	uint16_t session_type,session_prio,engine;

	if ( ! ppa_get_session_limit_enable(0))
		return PPA_SUCCESS;

	for(session_type=0; session_type<MAX_SESSION_TYPE; session_type++)
		for(session_prio=0; session_prio<MAX_SESSION_PRIORITY; session_prio++)
			for(engine=0; engine<MAX_DATA_FLOW_ENGINES; engine++)
				cmd_info->session_count_info.count_info[session_type][session_prio][engine].count =
								session_count[session_type][session_prio][engine];

	if ( ppa_copy_to_user((void *)arg, &cmd_info->session_count_info,
				sizeof(PPA_CMD_SESSION_COUNT_INFO)) != 0 )
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_copy_to_user fail\n");
	return PPA_SUCCESS;
}

int32_t ppa_ioctl_get_prio_sessions(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	PPA_CMD_GET_SESSIONS_INFO *pSessInfo;
	PPA_CMD_SESSION_COUNT_INFO *session_count;

	uint16_t session_type,session_prio,engine;
	uint32_t size=0, count=0;

	struct uc_session_node *p_item, *start;
	void *pUserSessionList =((void*)arg) +
		offsetof(PPA_CMD_GET_SESSIONS_INFO, session_list);

	extern PPA_LIST_HEAD session_list_head[MAX_SESSION_TYPE][MAX_SESSION_PRIORITY][MAX_DATA_FLOW_ENGINES];

	if ( ! ppa_get_session_limit_enable(0))
		return PPA_SUCCESS;

	session_count = &cmd_info->sessions_info.session_count;
	if ( ppa_copy_from_user(session_count,
				(void *)arg, sizeof(PPA_CMD_SESSION_COUNT_INFO)) != 0 )
		return PPA_FAILURE;

	pSessInfo = &cmd_info->sessions_info;

	for(session_type=0; session_type<MAX_SESSION_TYPE; session_type++)
		for(session_prio=0; session_prio<MAX_SESSION_PRIORITY; session_prio++)
			for(engine=0; engine<MAX_DATA_FLOW_ENGINES; engine++) {
				count = 0;
				ppa_session_list_lock();
				ppa_list_for_each_entry_safe_reverse(p_item, start,
						&session_list_head[session_type][session_prio][engine], priority_list) {

					if( count++ >= session_count->count_info[session_type][session_prio][engine].count)
						break;

					if(p_item->prev_sess_bytes) {
						pSessInfo->session_list[0].flags = p_item->flags;
						pSessInfo->session_list[0].host_bytes = p_item->host_bytes -
							p_item->prev_clear_host_bytes;
						pSessInfo->session_list[0].hw_bytes = p_item->acc_bytes -
							p_item->prev_clear_acc_bytes;
						pSessInfo->session_list[0].prev_sess_bytes = p_item->prev_sess_bytes;
						pSessInfo->session_list[0].session = (uint32_t ) p_item->session;
						pSessInfo->session_list[0].hash = (uint32_t ) p_item->hash;
						pSessInfo->session_list[0].collision_flag = 
							(uint32_t ) p_item->collision_flag;

						ppa_copy_to_user( pUserSessionList + size*sizeof(pSessInfo->session_list[0]),
								pSessInfo->session_list,sizeof(pSessInfo->session_list[0]));
						size++;
					}

					if(session_count->count_info[session_type][session_prio][engine].stamp_flag
						& SESSION_BYTE_STAMPING) {
						if(p_item->flags & SESSION_ADDED_IN_HW)
							p_item->prev_sess_bytes = p_item->acc_bytes -
								p_item->prev_clear_acc_bytes;
						else
							p_item->prev_sess_bytes = p_item->host_bytes;
					}
				}
				ppa_session_list_unlock();
				session_count->count_info[session_type][session_prio][engine].count = count;
			}

	if( ppa_copy_to_user((void *)arg, &pSessInfo->session_count,
				sizeof(PPA_CMD_SESSION_COUNT_INFO)) != 0 ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_copy_to_user fail\n");
		return -EFAULT;
	}
	return PPA_SUCCESS;
}

int32_t ppa_ioctl_manage_sessions(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	struct uc_session_node *p_item=NULL;
	PPA_CMD_MANAGE_SESSION *psess_buffer;
	PPA_CMD_COUNT_INFO	 *count_info;
	int size = 0, index = 0;

	if ( ! ppa_get_session_limit_enable(0))
		return PPA_SUCCESS;

	count_info = &cmd_info->manage_sessions.count_info;
	if ( ppa_copy_from_user(count_info, (void *)arg,
				sizeof(PPA_CMD_COUNT_INFO)) != 0 ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_copy_from_user fail\n");
		return PPA_FAILURE;
	}

	size = sizeof(PPA_CMD_MANAGE_SESSION) +
		(sizeof(PPA_CMD_SESSION_EXTRA_ENTRY) * ( count_info->count));

	psess_buffer = (PPA_CMD_MANAGE_SESSION *)ppa_malloc(size);
	if ( psess_buffer == NULL )
		return -EFAULT;

	if ( ppa_copy_from_user(psess_buffer, (void *)arg, size) != 0 ) {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_copy_from_user fail\n");
		ppa_free(psess_buffer);
		return PPA_FAILURE;
	}

	if ( psess_buffer->count_info.count > 0 ) {
		ppa_session_list_lock();
		for ( index = 0; index < psess_buffer->count_info.count; index++ ) {
			if( PPA_SESSION_EXISTS ==
				__ppa_session_find_ct_hash((PPA_SESSION*)psess_buffer->session_extra_info[index].session,
						psess_buffer->session_extra_info[index].hash, &p_item) ) {
				if(p_item->flags & SESSION_ADDED_IN_HW) {
					ppa_hsel_del_routing_session(p_item);
					/* software session management is handled by SAE */
					ppa_sw_session_enable(p_item, 1, p_item->flags);
					p_item->prev_sess_bytes = 0x00;

				} else if(p_item->flags & SESSION_ADDED_IN_SW) {
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
					/* software session management is handled by SAE */
					ppa_sw_session_enable(p_item, 0, p_item->flags);
					if(ppa_hsel_add_routing_session(p_item, 0) != PPA_SUCCESS ){
						p_item->flag2 |= SESSION_FLAG2_ADD_HW_FAIL;
					} else {
						p_item->prev_sess_bytes = 0x00;
					}
#endif
				}
				__ppa_session_put(p_item);
			}
		}
		ppa_session_list_unlock();
	}
	ppa_free(psess_buffer);
	return PPA_SUCCESS;
}
#endif

int32_t ppa_ioctl_set_session_timer(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	if ( ppa_copy_from_user(&cmd_info->session_timer_info, (void *)arg,
		sizeof(cmd_info->session_timer_info)) != 0 ) {
		return PPA_FAILURE;
	}

	if( ppa_hook_set_inactivity_fn )
		return ppa_hook_set_inactivity_fn( (PPA_U_SESSION *)cmd_info->session_timer_info.session,
			cmd_info->session_timer_info.timer_in_sec);
	else return PPA_FAILURE;
}

int32_t ppa_ioctl_get_session_timer(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	ppa_memset(&cmd_info->session_timer_info, 0, sizeof(cmd_info->session_timer_info) );
	cmd_info->session_timer_info.timer_in_sec = ppa_api_get_session_poll_timer();

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->session_timer_info,
			sizeof(cmd_info->session_timer_info)) != 0 )
		return PPA_FAILURE;
	else
		return PPA_SUCCESS;
}

int32_t ppa_ioctl_set_ppe_fastpath_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int res=PPA_FAILURE;

	res = copy_from_user( &cmd_info->ppe_fastpath_enable_info, (void *)arg,
			sizeof(cmd_info->ppe_fastpath_enable_info));
	if ( res != PPA_SUCCESS )
		return PPA_FAILURE;

	res = ppa_hook_set_ppe_fastpath_enable(
		cmd_info->ppe_fastpath_enable_info.ppe_fastpath_enable,
		cmd_info->ppe_fastpath_enable_info.flags);

	return res;
}

int32_t ppa_ioctl_get_ppe_fastpath_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int res=PPA_FAILURE;

	ppa_memset(&cmd_info->ppe_fastpath_enable_info, 0, sizeof(cmd_info->ppe_fastpath_enable_info) );

	res = copy_from_user( &cmd_info->ppe_fastpath_enable_info, (void *)arg,
			sizeof(cmd_info->ppe_fastpath_enable_info));
	if ( res != PPA_SUCCESS )
		return PPA_FAILURE;

	res = ppa_hook_get_ppe_fastpath_enable(
			&cmd_info->ppe_fastpath_enable_info.ppe_fastpath_enable,
			cmd_info->ppe_fastpath_enable_info.flags);

	if ( ppa_copy_to_user( (void *)arg, &cmd_info->ppe_fastpath_enable_info,
			sizeof(cmd_info->ppe_fastpath_enable_info)) != 0 )
		return PPA_FAILURE;

	return res;
}

#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
int32_t ppa_ioctl_set_sw_fastpath_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int res=PPA_FAILURE;

	res = copy_from_user( &cmd_info->sw_fastpath_enable_info, (void *)arg,
			sizeof(cmd_info->sw_fastpath_enable_info));
	if ( res != PPA_SUCCESS )
		return PPA_FAILURE;

	res = ppa_sw_fastpath_enable( cmd_info->sw_fastpath_enable_info.sw_fastpath_enable);

	if ( res == PPA_SUCCESS ) {
		if( cmd_info->sw_fastpath_enable_info.sw_fastpath_enable ) {
			/* Re-register the hooks */
			ppa_reg_export_fn(PPA_SW_FASTPATH_SEND_FN,
					ppa_sw_fastpath_send,
					"ppa_sw_fastpath_send",
					(void**)&ppa_hook_sw_fastpath_send_fn,
					ppa_hook_sw_fastpath_send_rpfn);
		} else {
			/* Un-register the hooks */
			ppa_unreg_export_fn(PPA_SW_FASTPATH_SEND_FN,
					(void**)&ppa_hook_sw_fastpath_send_fn);
		}

	} else {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_ioctl_set_sw_fastpath_enable fail\n");
	}

	return res;
}

int32_t ppa_ioctl_get_sw_fastpath_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int res=PPA_FAILURE;

	ppa_memset(&cmd_info->sw_fastpath_enable_info, 0, sizeof(cmd_info->sw_fastpath_enable_info) );

	res = copy_from_user( &cmd_info->sw_fastpath_enable_info, (void *)arg,
			sizeof(cmd_info->sw_fastpath_enable_info));
	if ( res != PPA_SUCCESS )
		return PPA_FAILURE;

	res = ppa_get_sw_fastpath_status( &cmd_info->sw_fastpath_enable_info.sw_fastpath_enable);

	if ( res != PPA_SUCCESS )
	{
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"ppa_ioctl_get_sw_fastpath_enable fail\n");
		res = PPA_FAILURE;
	}
	if ( ppa_copy_to_user( (void *)arg, &cmd_info->sw_fastpath_enable_info,
			sizeof(cmd_info->sw_fastpath_enable_info)) != 0 )
		return PPA_FAILURE;

	return res;
}

int32_t ppa_ioctl_set_sw_session_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int res=PPA_FAILURE;
	PPA_CMD_SW_SESSION_ENABLE_INFO *pSwSessEnableInfo;
	struct uc_session_node *p_item=NULL;

	pSwSessEnableInfo = &cmd_info->sw_session_enable_info;

	if ( ppa_copy_from_user(pSwSessEnableInfo, (void *)arg,
		sizeof(cmd_info->sw_session_enable_info)) != 0 ) {
		return PPA_FAILURE;
	}

	if( pSwSessEnableInfo->session == 0 ) {
		ppa_debug( DBG_ENABLE_MASK_DEBUG_PRINT,
				"ppa_ioctl_set_sw_session_enable failed as session %px\n",
				(void *)pSwSessEnableInfo->session);
		return PPA_FAILURE; //*sw session can not be enabled*/
	}

	ppa_session_list_lock();
	if( PPA_SESSION_EXISTS ==
			__ppa_session_find_by_ct((PPA_SESSION*)(pSwSessEnableInfo->session),
				0, &p_item) ) {

		ppa_debug( DBG_ENABLE_MASK_DEBUG_PRINT,
				"ConnTrack matched %px match with %px\n",
				(void *)pSwSessEnableInfo->session,(void *)p_item->session);

		/* Check if other direction need to be modified */
		if ( (p_item->flags & SESSION_ADDED_IN_SW) ) {
			res = ppa_sw_session_enable( p_item, pSwSessEnableInfo->sw_session_enable,
				pSwSessEnableInfo->flags );

			if ( res != PPA_SUCCESS ) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
					"ppa_ioctl_set_sw_session_enable fail\n");
				res = PPA_FAILURE;
			}
		} else {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				"No point to enable or disable session in sw accel.\n");
			res = PPA_SUCCESS; // No point to enable/disable
		}
		__ppa_session_put(p_item);
	} else {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Session %px does not exist!!!\n",
			(void *)pSwSessEnableInfo->session);
	}

	ppa_session_list_unlock();

	return res;
}

int32_t ppa_ioctl_get_sw_session_enable(unsigned int cmd, unsigned long arg, PPA_CMD_DATA *cmd_info)
{
	int res=PPA_FAILURE;
	PPA_CMD_SW_SESSION_ENABLE_INFO *pSwSessEnableInfo;
	struct uc_session_node *p_item=NULL;

	pSwSessEnableInfo = &cmd_info->sw_session_enable_info;
	pSwSessEnableInfo->sw_session_enable = 0;

	if ( ppa_copy_from_user(pSwSessEnableInfo, (void *)arg,
			sizeof(cmd_info->sw_session_enable_info)) != 0 ) {
		return PPA_FAILURE;
	}

	if( pSwSessEnableInfo->session == 0 ) {
		ppa_debug( DBG_ENABLE_MASK_DEBUG_PRINT,
				"ppa_ioctl_get_sw_session_enable failed as session=%px\n",
				(void *)pSwSessEnableInfo->session );
		return PPA_FAILURE; /*sw session can not be enabled*/
	}

	ppa_session_list_lock();
	if( PPA_SESSION_EXISTS ==
			__ppa_session_find_by_ct((PPA_SESSION*)(pSwSessEnableInfo->session),
				0, &p_item) ) {

		ppa_debug( DBG_ENABLE_MASK_DEBUG_PRINT,
				"ConnTrack matched %px match with %px\n",
				(void *)pSwSessEnableInfo->session, (void *)p_item->session );

		/* Check if other direction need to be modified */
		if ( (p_item->flags & SESSION_ADDED_IN_SW) ) {
			res = ppa_get_sw_session_status( p_item,
				&pSwSessEnableInfo->sw_session_enable, pSwSessEnableInfo->flags );

			if ( res != PPA_SUCCESS ) {
				ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
					"ppa_ioctl_get_sw_session_enable fail\n");
				res = PPA_FAILURE;
			}
		} else {
			ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,
				"No point to enable or disable session in sw accel.\n");
			res = PPA_SUCCESS; /* No point to enable/disable*/
		}
	} else {
		ppa_debug(DBG_ENABLE_MASK_DEBUG_PRINT,"Session %px does not exist!!!\n",
			(void *)pSwSessEnableInfo->session);
	}

	ppa_session_list_unlock();

	if ( ppa_copy_to_user( (void *)arg, pSwSessEnableInfo,
			sizeof(cmd_info->sw_session_enable_info)) != 0 )
		return PPA_FAILURE;

	return res;
}
#endif
int32_t ppa_session_delete(PPA_SESSION *p_session, uint32_t flags);
/*
 * ####################################
 *		   Init/Cleanup API
 * ####################################
 */
void ppa_reg_expfn(void)
{
#if IS_ENABLED(CONFIG_PPA_RT_SESS_LEARNING)
	ppa_reg_export_fn(PPA_SESSION_ADD_FN, ppa_session_add, "ppa_session_add",
		(void**)&ppa_hook_session_add_fn, ppa_session_add_rpfn );
#if IS_ENABLED(CONFIG_PPA_BR_SESS_LEARNING)
	ppa_reg_export_fn(PPA_SESSION_BRADD_FN, ppa_session_add, "ppa_session_bradd",
		(void**)&ppa_hook_session_bradd_fn, ppa_session_bradd_rpfn );
#endif /*CONFIG_PPA_BR_SESS_LEARNING*/
	ppa_reg_export_fn(PPA_SESSION_REFCNT_INC_FN, ppa_pitem_refcnt_inc, "ppa_hook_pitem_refcnt_inc",
		(void**)&ppa_hook_pitem_refcnt_inc_fn, ppa_session_refcnt_inc_fn);
	ppa_reg_export_fn(PPA_SESSION_REFCNT_DEC_FN, ppa_pitem_refcnt_dec, "ppa_hook_pitem_refcnt_dec",
		(void**)&ppa_hook_pitem_refcnt_dec_fn, ppa_session_refcnt_dec_fn);
#if IS_ENABLED(CONFIG_PPA_MPE_IP97)
	ppa_reg_export_fn(PPA_SESSION_IPSEC_ADD_FN, ppa_session_ipsec_add, "ppa_session_ipsec_add",
		(void**)&ppa_hook_session_ipsec_add_fn, ppa_session_ipsec_add_rpfn);
	ppa_reg_export_fn(PPA_SESSION_IPSEC_DEL_FN, ppa_session_ipsec_delete, "ppa_session_ipsec_delete",
		(void**)&ppa_hook_session_ipsec_del_fn, ppa_session_ipsec_del_rpfn);
#endif
#endif /*CONFIG_PPA_RT_SESS_LEARNING*/
	ppa_reg_export_fn(PPA_SESSION_DEL_FN, ppa_session_delete, "ppa_session_delete",
		(void**)&ppa_hook_session_del_fn, ppa_session_del_rpfn);
	ppa_reg_export_fn(PPA_GET_CT_STAT_FN, ppa_get_ct_stats, "ppa_get_ct_stats",
		(void**)&ppa_hook_get_ct_stats_fn, ppa_get_ct_stats_rpfn );
#if defined(CONFIG_PPA_HANDLE_CONNTRACK_SESSIONS)
	ppa_reg_export_fn(PPA_SESSION_PRIO_FN, ppa_session_prio, "ppa_session_prio",
		(void**)&ppa_hook_session_prio_fn, ppa_session_prio_rpfn);
#endif
	ppa_reg_export_fn(PPA_INACTIVITY_STATUS_FN, ppa_inactivity_status, "ppa_inactivity_status",
		(void**)&ppa_hook_inactivity_status_fn, ppa_inactivity_status_rpfn );
	ppa_reg_export_fn(PPA_SET_INACTIVITY_FN, ppa_set_session_inactivity, "ppa_set_session_inactivity",
		(void**)&ppa_hook_set_inactivity_fn, ppa_set_inactivity_rpfn );
#if IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING)
	ppa_reg_export_fn(PPA_BRIDGE_ENTRY_ADD_FN, ppa_bridge_entry_add, "ppa_bridge_entry_add",
		(void**)&ppa_hook_bridge_entry_add_fn, ppa_bridge_entry_add_rpfn );
	ppa_reg_export_fn(PPA_BRIDGE_ENTRY_DELETE_FN, ppa_bridge_entry_delete, "ppa_bridge_entry_delete",
		(void**)&ppa_hook_bridge_entry_delete_fn, ppa_bridge_entry_delete_rpfn );
	ppa_reg_export_fn(PPA_BRIDGE_ENTRY_HIT_TIME_FN, ppa_bridge_entry_hit_time, "ppa_bridge_entry_hit_time",
		(void**)&ppa_hook_bridge_entry_hit_time_fn, ppa_bridge_entry_hit_time_rpfn  );
#endif /*CONFIG_PPA_BR_MAC_LEARNING*/
#if defined(PPA_IF_MIB) && PPA_IF_MIB
	ppa_reg_export_fn(PPA_GET_NETIF_ACCEL_STATS_FN, ppa_get_netif_accel_stats, "ppa_get_netif_accel_stats",
		(void**)&ppa_hook_get_netif_accel_stats_fn, ppa_get_netif_accel_stats_rpfn);
#endif
#if IS_ENABLED(CONFIG_PPA_API_DIRECTCONNECT)
	ppa_reg_export_fn(PPA_DISCONN_IF_FN, ppa_disconn_if, "ppa_disconn_if",
		(void**)&ppa_hook_disconn_if_fn, ppa_disconn_if_rpfn );
#endif
#if IS_ENABLED(CONFIG_PPA_API_DIRECTPATH)
	ppa_reg_export_fn(PPA_PHYS_PORT_ADD_FN, ppa_phys_port_add, "ppa_phys_port_add",
		(void**)&ppa_phys_port_add_hook_fn, ppa_phys_port_add_hook_rpfn  );
	ppa_reg_export_fn(PPA_PHYS_PORT_REMOVE_FN, ppa_phys_port_remove, "ppa_phys_port_remove",
		(void**)&ppa_phys_port_remove_hook_fn, ppa_phys_port_remove_hook_rpfn  );
#endif /* CONFIG_PPA_API_DIRECTPATH */
#if defined(QOS_HOOK_EXPORT_SUPPORT) /* qos functions not used by kernel by now. So not export yet*/
#if IS_ENABLED(CONFIG_PPA_QOS)
	ppa_reg_export_fn(PPA_GET_DSL_MIB_FN, ppa_get_dsl_mib, "ppa_get_dsl_mib",
		(void**)&ppa_hook_get_qos_qnum, ppa_get_qos_qnum_rpfn );
	ppa_reg_export_fn(PPA_GET_PORT_MIB_FN, ppa_get_ports_mib, "ppa_get_ports_mib",(
		(void**)&ppa_hook_get_qos_mib, ppa_get_qos_mib_rpfn  );
#endif /* CONFIG_PPA_QOS_RATE_SHAPING */
#endif /* CONFIG_PPA_QOS */
#if IS_ENABLED(CONFIG_PPA_API_SW_FASTPATH)
	ppa_reg_export_fn(PPA_ENABLE_SW_FASTPATH_FN, ppa_sw_fastpath_enable, "ppa_sw_fastpath_enable",
		(void**)&ppa_hook_set_sw_fastpath_enable_fn, ppa_hook_set_sw_fastpath_enable_rpfn );
	ppa_reg_export_fn(PPA_GET_SW_FASTPATH_STATUS_FN, ppa_get_sw_fastpath_status, "ppa_get_sw_fastpath_status",
		(void**)&ppa_hook_get_sw_fastpath_status_fn, ppa_hook_get_sw_fastpath_status_rpfn );
	ppa_reg_export_fn(PPA_SW_FASTPATH_SEND_FN, ppa_sw_fastpath_send, "ppa_sw_fastpath_send",
		(void**)&ppa_hook_sw_fastpath_send_fn, ppa_hook_sw_fastpath_send_rpfn);
#endif
}

int32_t ppa_api_init(void)
{

	if ( ppa_api_session_manager_create() != PPA_SUCCESS )
		goto PPA_API_SESSION_MANAGER_CREATE_FAIL;

	ppa_api_proc_file_create();

	ppa_reg_expfn();

	printk("PPA API --- init successfully\n");
	return 0;

PPA_API_SESSION_MANAGER_CREATE_FAIL:
	ppa_drv_hal_exit(0);
	return -1;
}

void __exit ppa_driver_exit(void)
{
	ppa_exit();
	ppa_export_fn_manager_exit();

	ppa_api_proc_file_destroy();

	ppa_api_session_manager_destroy();

}
