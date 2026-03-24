/******************************************************************************
 **
 ** FILE NAME  : qos_mgr_stack_al.c
 ** PROJECT    : UGW
 ** MODULES    : QoS Manager Stack Adaption Layer (Linux)
 **
 ** DATE     : 8 Jan 2020
 ** AUTHOR     : Mohammed Aarif
 ** DESCRIPTION  : QoS Manager Stack Adaption Layer (Linux)
 ** COPYRIGHT  : Copyright (c) 2019 - 2020 Intel Corporation
 **
 **   For licensing information, see the file 'LICENSE' in the root folder of
 **   this software module.
 **
 ** HISTORY
 ** $Date    $Author     $Comment
 ** 08 Jan 2020  Mohammed Aarif    Initiate Version
 *******************************************************************************/
/*
 * ####################################
 *        Head File
 * ####################################
 */
/*
 *  Common Head File
 */
#include <linux/version.h>
#include <generated/autoconf.h>

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/atmdev.h>
#include <linux/netdevice.h>
#include <linux/inetdevice.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/if_arp.h>
#include <linux/kthread.h>
#include <net/sock.h>
#include <net/ip.h>
#include <net/ipv6.h>
#include <net/addrconf.h>
#include <asm/time.h>
#include <net/netfilter/nf_conntrack.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,15,0)
#include <../net/8021q/vlan.h>
#include <net/netfilter/nf_conntrack_l3proto.h>
#include <net/netfilter/nf_conntrack_l4proto.h>
#else
#include <linux/if_bridge.h>
#endif
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_conntrack_acct.h>
#include <linux/netfilter/nf_conntrack_common.h>
#include <linux/netfilter/nf_conntrack_tuple_common.h>
#include <net/ip_tunnels.h>
#include <linux/if_tunnel.h>
#include <net/ip6_tunnel.h>
#include <linux/kallsyms.h>
#include <linux/if_macvlan.h>
/*
 *  Chip Specific Head File
 */
#include <net/qos_mgr/qos_mgr_tc_hook.h>
#include "qos_mgr_stack_al.h"
#include <net/qos_mgr/qos_mgr_hook.h>
#include "qos_mgr_ioctl.h"
#include <net/qos_mgr/qos_mgr_common.h>

/*
 * ####################################
 *			  Definition
 * ####################################
 */

#if IS_ENABLED(CONFIG_VLAN_8021Q)
#define VLAN_DEV_INFO vlan_dev_priv
#endif

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

uint32_t g_qos_mgr_dbg_enable = DBG_ENABLE_MASK_ERR;
uint32_t max_print_num=~0;

/*
 * ####################################
 *		   Global Variable
 * ####################################
 */

/*
 * ####################################
 *		   Extern Variable
 * ####################################
 */


/*
 * ####################################
 *		   Global Function
 * ####################################
 */
#if !IS_ENABLED(CONFIG_PPPOL2TP)
__u16 qos_mgr_pppol2tp_get_l2tp_session_id(struct net_device *dev)
{
	return 0;
}
__u16 qos_mgr_pppol2tp_get_l2tp_tunnel_id(struct net_device *dev)
{
	return 0;
}
int32_t qos_mgr_pppol2tp_get_base_netif(QOS_MGR_NETIF *netif, QOS_MGR_IFNAME pppol2tp_eth_ifname[QOS_MGR_IF_NAME_SIZE])
{
	return QOS_MGR_EPERM;
}
int32_t qos_mgr_pppol2tp_get_src_addr(struct net_device *dev, uint32_t *outer_srcip)
{
	return QOS_MGR_EPERM;
}
int32_t qos_mgr_pppol2tp_get_dst_addr(struct net_device *dev, uint32_t *outer_dstip)
{
	return QOS_MGR_EPERM;
}
#endif

int32_t qos_mgr_br2684_get_vcc(QOS_MGR_NETIF *netif, QOS_MGR_VCC **pvcc)
{
	if( qos_mgr_br2684_get_vcc_fn != NULL)
		return qos_mgr_br2684_get_vcc_fn( netif, pvcc);
	
	return QOS_MGR_EPERM;
}
#if !IS_ENABLED(CONFIG_PPPOATM)
int32_t qos_mgr_pppoa_get_vcc(QOS_MGR_NETIF *netif, QOS_MGR_VCC **patmvcc)
{
	return QOS_MGR_EPERM;
}

int32_t qos_mgr_if_is_pppoa(QOS_MGR_NETIF *netif, QOS_MGR_IFNAME *ifname)
{
	return 0;
}
#endif


int32_t qos_mgr_lock_init(QOS_MGR_LOCK *p_lock)
{
	if ( !p_lock )
		return QOS_MGR_EINVAL;

	spin_lock_init(&p_lock->lock);
	return QOS_MGR_SUCCESS;
}
void qos_mgr_trace_lock_get(QOS_MGR_LOCK *p_lock)
{
	ASSERT(p_lock->cnt == 0,"Lock already taken!!!, lock cnt: %d\n", p_lock->cnt);
	if(p_lock->cnt != 0){
		dump_stack();
	}
	p_lock->cnt += 1;
}

void qos_mgr_trace_lock_release(QOS_MGR_LOCK *p_lock)
{
	ASSERT(p_lock->cnt == 1, "Lock already released!!!, lock cnt: %d\n", p_lock->cnt);
	if(p_lock->cnt != 1){
		dump_stack();
	}
	p_lock->cnt -= 1;
}

void qos_mgr_lock_get(QOS_MGR_LOCK *p_lock)
{
	qos_mgr_spin_lock_bh(&p_lock->lock);
	qos_mgr_trace_lock_get(p_lock);

}

void qos_mgr_lock_release(QOS_MGR_LOCK *p_lock)
{
	qos_mgr_trace_lock_release(p_lock);
	qos_mgr_spin_unlock_bh(&p_lock->lock);
}

void qos_mgr_lock_destroy(QOS_MGR_LOCK *p_lock)
{
}

void *qos_mgr_malloc(uint32_t size)
{
	gfp_t flags;

	if ( in_atomic() || in_interrupt() )
		flags = GFP_ATOMIC;
	else
		flags = GFP_KERNEL;
	return kmalloc(size, flags);
}

int32_t qos_mgr_free(void *buf)
{
	kfree(buf);
	return QOS_MGR_SUCCESS;
}

void qos_mgr_memset(void *dst, uint32_t pad, uint32_t n)
{
	memset(dst, pad, n);
}

int32_t qos_mgr_atomic_read(QOS_MGR_ATOMIC *v)
{
	return atomic_read(v);
}

void qos_mgr_atomic_set(QOS_MGR_ATOMIC *v, int32_t i)
{
	atomic_set(v, i);
}
int32_t qos_mgr_atomic_inc(QOS_MGR_ATOMIC *v)
{
	return atomic_inc_return(v);
}

int32_t qos_mgr_atomic_dec(QOS_MGR_ATOMIC *v)
{
	return atomic_dec_if_positive(v);
}

uint32_t qos_mgr_copy_from_user(void *to, const void QOS_MGR_USER  *from, uint32_t  n)
{
	return copy_from_user(to, from, n);
}
uint32_t qos_mgr_copy_to_user(void QOS_MGR_USER *to, const void *from, uint32_t  n)
{
	return copy_to_user(to, from, n);
}

uint8_t *qos_mgr_strncpy(uint8_t *dest, const uint8_t *src, QOS_MGR_SIZE_T n)
{
	return strncpy(dest, src, n-1);
}

QOS_MGR_SIZE_T  qos_mgr_strlen(const uint8_t *s)
{
	return strnlen(s, QOS_MGR_IF_NAME_SIZE);
}

int32_t qos_mgr_str_cmp(char *str1,char *str2)
{
	return ( (strcmp(str1,str2) == 0) ? 1 : 0);
}

uint32_t qos_mgr_ioc_type(uint32_t nr)
{
	return  _IOC_TYPE(nr);
}

uint32_t qos_mgr_ioc_nr(uint32_t nr)
{
	return  _IOC_NR(nr);
}

uint32_t qos_mgr_ioc_dir(uint32_t nr)
{
	return  _IOC_DIR(nr);
}

uint32_t qos_mgr_ioc_read(void)
{
	return  _IOC_READ;
}

uint32_t qos_mgr_ioc_write(void)
{
	return  _IOC_WRITE;
}

uint32_t qos_mgr_ioc_size(uint32_t nr)
{
	return  _IOC_SIZE(nr);
}

uint32_t qos_mgr_ioc_access_ok(uint32_t type, uint32_t addr, uint32_t size)
{
	return  access_ok(type, addr, size);
}

uint32_t qos_mgr_ioc_verify_write(void)
{
	return VERIFY_WRITE;
}


uint32_t qos_mgr_ioc_verify_read(void)
{
	return VERIFY_READ;
}

uint32_t cal_64_div(uint64_t t1, uint64_t t2)
{ /* cal the value of t1 divided by t2 */
	if( t1 == 0 ) return 0;
	if( t2 == 0 ) return (uint32_t)-1;

	while( (t1 > WRAPROUND_32BITS) || (t2 > WRAPROUND_32BITS) )
	{
		t2 = t2 >> 1;
		t1 = t1 >> 1;
	}

	if( t1 == 0 ) return 0;
	if( t2 == 0 ) return (uint32_t)-1;

	return (uint32_t)t1/(uint32_t)t2;
}

/*!
        \brief get netdevice by name
 */
static struct net_device *qos_mgr_dev_get_by_name(const QOS_MGR_IFNAME *ifname)
{
#ifdef CONFIG_NET_NS
        struct net *net;
        struct net_device *net_dev = dev_get_by_name(&init_net, ifname);

        if (net_dev == NULL) {
                for_each_net(net) {
                        net_dev = dev_get_by_name(net, ifname);
                        if (net_dev)
                                break;
                }
        }
        return net_dev;
#else
        return dev_get_by_name(&init_net, ifname);
#endif
}

QOS_MGR_NETIF *qos_mgr_get_netif(const QOS_MGR_IFNAME *ifname)
{
	/*This implementation has risk that the device is destroyed after we get and free it*/

	QOS_MGR_NETIF *netif;

	if ( ifname && (netif = qos_mgr_dev_get_by_name(ifname)) )
	{
		dev_put(netif);
		return netif;
	}
	else
		return NULL;
}


