/******************************************************************************
**
** FILE NAME    : ifx_eth_framework.h
** PROJECT      : UEIP
** MODULES      : ETH
**
** DATE         : 2 Nov 2010
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global ETH driver framework header file
** COPYRIGHT    :              Copyright (c) 2009
**                          Lantiq Deutschland GmbH
**                   Am Campeon 3; 85579 Neubiberg, Germany
**
**   For licensing information, see the file 'LICENSE' in the root folder of
**   this software module.
**
** HISTORY
** $Date        $Author         $Comment
** 02 NOV 2010  Xu Liang        Init Version
*******************************************************************************/



#ifndef IFX_ETH_FRAMEWORK_H
#define IFX_ETH_FRAMEWORK_H



/*
 * ####################################
 *              Data Type
 * ####################################
 */

typedef enum {
    IFX_ETH_FW_POLL_COMPLETE = 0,
    IFX_ETH_FW_POLL_CONTINUE = 1,
} ifx_eth_fw_poll_ret_t;

struct ifx_eth_fw_netdev_ops {
    //  routines usually implemented by IFX ETH/PPE drivers
    //  not all routines defined in net_device/net_device_ops are covered
    int             (*init)(struct net_device *dev);
    void            (*uninit)(struct net_device *dev);

    int             (*open)(struct net_device *dev);
    int	            (*stop)(struct net_device *dev);

    int             (*start_xmit)(struct sk_buff *skb, struct net_device *dev);

    void            (*set_multicast_list)(struct net_device *dev);
    int             (*set_mac_address)(struct net_device *dev, void *addr);
    int             (*do_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
    int             (*set_config)(struct net_device *dev, struct ifmap *map);
    int             (*change_mtu)(struct net_device *dev, int new_mtu);
    int	            (*neigh_setup)(struct net_device *dev, struct neigh_parms *);

    struct net_device_stats*
                    (*get_stats)(struct net_device *dev);

#ifdef CONFIG_NET_POLL_CONTROLLER
    void            (*poll_controller)(struct net_device *dev);
#endif
    ifx_eth_fw_poll_ret_t
                    (*poll)(struct net_device *dev, int work_to_do, int *work_done);
    int             weight;

    void            (*tx_timeout)(struct net_device *dev);
    int             watchdog_timeo;
};



/*
 * ####################################
 *             Declaration
 * ####################################
 */

extern void *ifx_eth_fw_netdev_priv(struct net_device *dev);
extern struct net_device *ifx_eth_fw_alloc_netdev(int sizeof_priv, const char *name, struct ifx_eth_fw_netdev_ops *ops);
extern void ifx_eth_fw_free_netdev(struct net_device *dev, int force);
extern int ifx_eth_fw_register_netdev(struct net_device *dev);
void ifx_eth_fw_unregister_netdev(struct net_device *dev, int force);
extern int ifx_eth_fw_poll_schedule(struct net_device *dev);
extern int ifx_eth_fw_poll_complete(struct net_device *dev);



#endif  //  IFX_ETH_FRAMEWORK_H
