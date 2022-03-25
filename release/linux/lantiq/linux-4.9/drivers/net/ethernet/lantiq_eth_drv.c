/******************************************************************************
**
** FILE NAME    : lantiq_eth_drv.c
** PROJECT      : Lantiq UEIP
** MODULES      : Lantiq CPE ethernet driver
** DATE         : 30 July  2009
** AUTHOR       : Reddy Mallikarjuna
** DESCRIPTION  : Lantiq Cross-Platform ethernet device driver
** COPYRIGHT    :       Copyright (c) 2013
**                      Lantiq Deutschland
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date                $Author                 $Comment
** 30 July 2009         Reddy Mallikarjuna  Initial UEIP release
** 28 July 2011         Kishore Kankipati 	Adapted for HN1
** 09 September 2013    Suresh Nagaraj		Adapted to Openwrt framework
*******************************************************************************/

#include <linux/version.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/kernel.h> /* printk() */
#include <linux/types.h>  /* size_t */
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/proc_fs.h>
#include <linux/etherdevice.h> /* eth_type_trans */
#include <asm/delay.h>
#include <linux/init.h>
#include <linux/clk.h>

#include <linux/of_net.h>
#include <linux/of_mdio.h>
#include <linux/of_gpio.h>

#define CONFIG_LTQMIPS_DMA
#define CONFIG_SW_ROUTING_MODE

#ifdef CONFIG_LTQMIPS_DMA
#include <xway/lantiq_dma.h>
#endif

#include "lantiq_eth_drv.h"

#ifdef CONFIG_NAPI_ENABLED
  #define CONFIG_IFX_NAPI               1
#endif

#define DRV_MODULE_NAME             "lantiq_eth_drv"
#define DRV_MODULE_VERSION          "1.1.1"
static char version[] =
        DRV_MODULE_NAME ".c:v" DRV_MODULE_VERSION " \n";

/* length of time before we decide the hardware is borked,
 * and dev->eth_tx_timeout() should be called to fix the problem
 */
#define LTQ_TX_TIMEOUT                  (10 * HZ)

#define DMA_TX_BURST_LEN                DMA_BURSTL_4DW
#define DMA_RX_BURST_LEN                DMA_BURSTL_4DW
#define ETH_PKT_BUF_SIZE                1568

#ifdef CONFIG_LTQMIPS_DMA
struct dma_device_info *g_dma_device=NULL;
#endif

static struct net_device *eth_dev[NUM_ETH_INF];
/* PMAC header structure */
cpu_egress_pkt_header_t eg_pkt_hdr;
cpu_ingress_pkt_header_t ig_pkt_hdr;
static int g_pmac_dma,g_dma_pmac;
/* /proc file to debug */
//#define SNMP_COUNTERS_DEBUG
#undef SNMP_COUNTERS_DEBUG
struct proc_dir_entry* g_eth_proc_dir;
/* Start the  network device interface queue */
static int ltq_eth_open(struct net_device *dev);
/* Stop the  network device interface queue */
static int ltq_eth_close(struct net_device *dev);
/* Transmit packet from Tx Queue to MAC */
static int ltq_start_xmit (struct sk_buff *skb, struct net_device *dev);
/* Hardware specific IOCTL's  */
static int ltq_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
/* Get the network device statistics */
static struct net_device_stats *ltq_get_stats (struct net_device *dev);
/* change MTU values */
static int ltq_change_mtu (struct net_device *dev, int new_mtu);
/*  Set mac address*/
static int ltq_set_mac_address(struct net_device *dev, void *p);
/* Transmit timeout*/
static void ltq_tx_timeout (struct net_device *dev);
/* select the tx dma channel */
static int select_tx_chan (struct sk_buff *skb, struct net_device *dev);
/*open dma rx channel*/
static void enable_dma_channel(void);
/*close the dma rx channel*/
static void disable_dma_channel(void);
/* Init of the network device */
static int ltq_switch_init(struct net_device *dev);
/* Get the ether addr from u-boot */
static unsigned char my_ethaddr[MAX_ADDR_LEN];
#ifdef  CONFIG_IFX_NAPI
static ltq_eth_fw_poll_ret_t ltq_switch_poll(struct net_device *poll_dev, int work_to_do, int *work_done);
#endif

static int rversion_open (struct inode *inode, struct file *file);
static int rpce_open (struct inode *inode, struct file *file);
static int proc_read_pce(struct seq_file *m, void *v);
static int eth_proc_version(struct seq_file *m, void *v);

struct file_operations rversion_ops = {
	.open = rversion_open,
	.read = seq_read,
	.llseek		= seq_lseek,
	.release = single_release,
};

struct file_operations rpce_ops = {
	.open = rpce_open,
	.read = seq_read,
	.llseek		= seq_lseek,
	.release = single_release,
};

static struct net_device_ops ltq_eth_drv_ops = {
        .ndo_init           = ltq_switch_init,
        .ndo_open           = ltq_eth_open,
        .ndo_stop           = ltq_eth_close,
        .ndo_start_xmit     = ltq_start_xmit,
        .ndo_set_mac_address= ltq_set_mac_address,
        .ndo_change_mtu     = ltq_change_mtu,
        .ndo_get_stats      = ltq_get_stats,
        .ndo_do_ioctl       = ltq_ioctl,
        .ndo_tx_timeout     = ltq_tx_timeout,
#ifdef  CONFIG_IFX_NAPI
        .poll           = ltq_switch_poll,
        .weight         = 25,
#endif
};

static int vr9_7port_sw_hw_init(void)
{
    unsigned int reg;
    struct clk *clk;

    /*Enable Switch Power  */
	clk = clk_get_sys("1e108000.eth", NULL);
	clk_enable(clk);

    /*Enable Switch  */
    SW_WRITE_REG32 ( (SW_READ_REG32(GLOB_CTRL_REG) | GLOB_CTRL_SE), GLOB_CTRL_REG) ;

    /* Disable MDIO auto polling mode for all ports */
    /*SW_WRITE_REG32 ( 0x0, MDC_CFG_0_REG) ; */
#if 1
    {
	int i;
	for ( i =0; i < 7; i++) {
                /*       enable counters  */
                SW_WRITE_REG32(0x1, (ETHSW_BM_PCFG_REG + (i*8)));
        }
    }
#endif
    /* Replace default IPG value from 0x85 to ox8B */
    SW_WRITE_REG32 (0x8B, PMAC_RX_IPG_REG) ;

    reg = SW_READ_REG32(PMAC_HD_CTL_REG );
    reg |= PMAC_HD_CTL_AC;
#if defined(CONFIG_PMAC_DMA_ENABLE) && CONFIG_PMAC_DMA_ENABLE
    SW_WRITE_REG32 ( (SW_READ_REG32(FDMA_PCTRL_PORT6) | FDMA_PCTRL_STEN), FDMA_PCTRL_PORT6) ;
    reg |= PMAC_HD_CTL_AS;
#else
    reg &= ~PMAC_HD_CTL_AS;
#endif
#if defined(CONFIG_DMA_PMAC_ENABLE) && CONFIG_DMA_PMAC_ENABLE
    reg |= PMAC_HD_CTL_RXSH;
#else
    reg &= ~PMAC_HD_CTL_RXSH;
#endif
    SW_WRITE_REG32( reg, PMAC_HD_CTL_REG);

    for ( reg = 0; reg < 7; reg++ ) {
        SW_WRITE_REG32((SW_READ_REG32(FDMA_PCTRL_REG(reg)) | 0x01), FDMA_PCTRL_REG(reg));
        SW_WRITE_REG32((SW_READ_REG32(SDMA_PCTRL_REG(reg)) | 0x01), SDMA_PCTRL_REG(reg));
    }
#if defined(CONFIG_DMA_PMAC_ENABLE) && CONFIG_DMA_PMAC_ENABLE
    SW_WRITE_REG32 ( 0x20, PMAC_EWAN_REG) ;
#endif
    return 0;
}

//#define DUMP_PACKET

#ifdef DUMP_PACKET
/*
* \brief	dump skb data
* \param[in] len length of the data buffer
* \param[in] pData Pointer to data to dump
*
* \return void No Value
*/
static inline void dump_skb(u32 len, char *pData){
	int i;
	for(i=0;i<len;i++){
		printk("%2.2x ",(u8)(pData[i]));
		if (i % 16 == 15)
			printk("\n");
	}
	printk("\n");
}
#endif

/* Get the driver information, used by ethtool_ops  */
static void get_drvinfo (struct net_device *dev, struct ethtool_drvinfo *info)
{
    /* driver driver short name (Max 32 characters) */
    strcpy (info->driver, DRV_MODULE_NAME);
    /* driver version (Max 32 characters) */
    strcpy (info->version, DRV_MODULE_VERSION);
}

/* Get the network device interfcae number */
static int get_network_dev_num(struct net_device *dev)
{
    int dev_index = ( (!strcmp(dev->name, "eth0")) ? 0 : (!strcmp(dev->name, "eth1") ) ? 1 : \
    (!strcmp(dev->name, "eth2")) ? 2: (!strcmp(dev->name, "eth3"))? 3: \
    (!strcmp(dev->name, "eth4"))? 4: (!strcmp(dev->name, "eth5"))? 5: -1);
    return dev_index;
}

/* Get the network device settings  */
static int get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
    ltq_switch_priv_t *priv = netdev_priv(dev);
    unsigned int  port_status;
    int dev_index = get_network_dev_num(dev);
    if (dev_index == -1 ) {
        /* should not be here*/
        printk(KERN_ERR "%s[%d]: Dev index error(%d)!!!\n", __func__,__LINE__,dev_index);
        return -ENODEV;
    }

    cmd->supported = (SUPPORTED_Autoneg | SUPPORTED_TP);
    cmd->supported |= (SUPPORTED_100baseT_Half |    \
                    SUPPORTED_100baseT_Full |       \
                    SUPPORTED_10baseT_Half |        \
                    SUPPORTED_10baseT_Full |        \
                    SUPPORTED_1000baseT_Half |      \
                    SUPPORTED_1000baseT_Full |      \
                    SUPPORTED_MII);

    cmd->advertising = 0;
    if (priv->flags & FLAG_ADV_10HALF)
        cmd->advertising |= ADVERTISED_10baseT_Half;
    if (priv->flags & FLAG_ADV_10FULL)
        cmd->advertising |= ADVERTISED_10baseT_Full;
    if (priv->flags & FLAG_ADV_100HALF)
        cmd->advertising |= ADVERTISED_100baseT_Half;
    if (priv->flags & FLAG_ADV_100FULL)
        cmd->advertising |= ADVERTISED_100baseT_Full;

    if (priv->flags & FLAG_ADV_1000HALF)
        cmd->advertising |= ADVERTISED_1000baseT_Half;
    if (priv->flags & FLAG_ADV_1000FULL)
        cmd->advertising |= ADVERTISED_1000baseT_Full;

    cmd->advertising |= ADVERTISED_Pause | ADVERTISED_Asym_Pause;

    port_status = SW_READ_REG32(MDIO_STAT_0_REG + (dev_index * 4));

    switch ( MDIO_STAT_SPEED(port_status)) {
        case 0:
            priv->current_speed=SPEED_10;
            break;
        case 1:
            priv->current_speed=SPEED_100;
            break;
        case 2:
            priv->current_speed=SPEED_1000;
            break;
        default: /* should not be here */
            printk(KERN_ERR "%s[%d]: Port status error(%d)!!!\n", __func__,__LINE__,port_status);
    }
    cmd->speed = priv->current_speed;
    if ( port_status & MDIO_STAT_FDUP )
        priv->full_duplex=DUPLEX_FULL;
    else
        priv->full_duplex=DUPLEX_HALF;
    cmd->duplex = priv->full_duplex;

    if ( port_status & MDIO_STAT_LSTAT)
        cmd->reserved[0]    = 0x1;  /*link up bit:0*/
    else
        cmd->reserved[0]    = 0x0;  /*link down */

    if (port_status & MDIO_STAT_RXPAUEN )
        cmd->reserved[0]    |= 0x2; /*flow control enable Bit1:1*/
    else
        cmd->reserved[0]    |= 0x0; /*flow control disable*/
    /* TODO*/
    cmd->phy_address = dev_index;

    cmd->port = PORT_MII;
    cmd->transceiver = (priv->flags & FLAG_INTERNAL_PHY) ? XCVR_INTERNAL : XCVR_EXTERNAL;
    return 0;
}

/* Set the network device settings */
static int set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
    ltq_switch_priv_t *priv = netdev_priv(dev);
    unsigned int  phy_reg;
    int dev_index = get_network_dev_num(dev);
    if (dev_index == -1 ) {
        /* should not be here*/
        printk(KERN_ERR "%s[%d]: Dev index error(%d)!!!\n", __func__,__LINE__,dev_index);
        return -ENODEV;
    }
    if (cmd->autoneg == AUTONEG_ENABLE) {
        /*TODO*/
        return 0;
    } else if ((cmd->speed != SPEED_100 && cmd->speed != SPEED_10 && cmd->speed != SPEED_1000) \
            ||(cmd->duplex != DUPLEX_HALF && cmd->duplex != DUPLEX_FULL)) {
            return -EINVAL;
    }

    spin_lock_irq(&priv->lock);
    if (cmd->autoneg != AUTONEG_ENABLE) {
        phy_reg = SW_READ_REG32(PHY_ADDR_0 - (dev_index * 4));
        phy_reg &= ~(PHY_ADDR_LINKST_MASK | PHY_ADDR_SPEED_MASK | PHY_ADDR_FDUP_MASK);
        priv->flags |= FLAG_FORCE_LINK;
        priv->flags &= ~(FLAG_100_BASE_T |FLAG_1000_BASE_T | FLAG_FULL_DUPLEX);
        if (cmd->speed == SPEED_100) {
            priv->flags |= FLAG_100_BASE_T;
            phy_reg |= PHY_ADDR_SPEED_100;
        } else if(cmd->speed == SPEED_1000) {
            priv->flags |= FLAG_1000_BASE_T;
            phy_reg |= PHY_ADDR_SPEED_1000;
        }
        if (cmd->duplex == DUPLEX_FULL) {
            priv->flags |= FLAG_FULL_DUPLEX;
            phy_reg |= PHY_ADDR_FDUP_EN;
        }
        SW_WRITE_REG32(phy_reg, (PHY_ADDR_0 - (dev_index * 4)) );
    }
    spin_unlock_irq(&priv->lock);
    return 0;
}

/* Reset the device */
static int nway_reset(struct net_device *dev)
{
     /*TODO*/
    return 0;
}

/* Structure of the ether tool operation  */
static const struct ethtool_ops ethtool_ops = {
        .get_drvinfo            = get_drvinfo,
        .get_settings           = get_settings,
        .set_settings           = set_settings,
        .nway_reset             = nway_reset,
        .get_link               = ethtool_op_get_link,
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
        .get_perm_addr          = ethtool_op_get_perm_addr,
#endif
};

#ifndef MODULE
/* Get the ether addr from u-boot */
static int __init ethaddr_setup(char *line)
{
    char *ep;
    int i;
    memset(my_ethaddr, 0, MAX_ADDR_LEN);
    /* there should really be routines to do this stuff */
    for (i = 0; i < 6; i++)	{
        my_ethaddr[i] = line ? simple_strtoul(line, &ep, 16) : 0;
        if (line)
            line = (*ep) ? ep+1 : ep;
    }
    printk("mac address %2x-%2x-%2x-%2x-%2x-%2x \n" \
        ,my_ethaddr[0]  \
        ,my_ethaddr[1]  \
        ,my_ethaddr[2]  \
        ,my_ethaddr[3]  \
        ,my_ethaddr[4]  \
        ,my_ethaddr[5]);
    return 0;
}
__setup("ethaddr=", ethaddr_setup);
#endif

/* Turn On RX DMA channels */
static void enable_dma_channel(void)
{
#ifdef CONFIG_LTQMIPS_DMA
    struct dma_device_info* dma_dev=g_dma_device;
    int i;

    for(i=0; i<dma_dev->max_rx_chan_num; i++) {
        if ( (dma_dev->rx_chan[i])->control==IFX_DMA_CH_ON )
            dma_dev->rx_chan[i]->open(dma_dev->rx_chan[i]);
    }
#endif
}

/* Turn Off RX DMA channels */
static void disable_dma_channel()
{
#ifdef CONFIG_LTQMIPS_DMA
    struct dma_device_info* dma_dev=g_dma_device;
    int i;

    for (i=0; i<dma_dev->max_rx_chan_num; i++)
        dma_dev->rx_chan[i]->close(dma_dev->rx_chan[i]);
#endif
}
static int index=0 ;
/* open the network device interface*/
static int ltq_eth_open(struct net_device *dev)
{
    if(index == 0) {
        enable_dma_channel();
    }
    index++;
//    netif_start_queue(dev);
    return 0;
}

/* Close the network device interface*/
static int ltq_eth_close(struct net_device *dev)
{
    if(index) index--;
	if(index == 0){
	    disable_dma_channel();
	}
//    netif_stop_queue(dev);
    return 0;
}

/* Send the packet to netwrok rx queue, used by  switch_hw_receive function */
static void eth_rx(struct net_device *dev, int len,struct sk_buff* skb)
{
    ltq_switch_priv_t *priv             = netdev_priv(dev);

    skb->dev = dev;
    skb->protocol = eth_type_trans(skb, dev);
#ifdef  CONFIG_IFX_NAPI
    netif_receive_skb(skb);
#else
    netif_rx(skb);
#endif
    priv->stats.rx_packets++;
    priv->stats.rx_bytes+=len;
}

/*
* This function is called in dma intr handler (DMA RCV_INT interrupt).
* This function get the data from the DMA device.
*   if the packet is valid then it will send to upper layer based on  criteria.
*       The switch CPU port PMAC status header is enabled, then remove the header and
*           look from which port the packet comes and send to relative network device.
        if PMAC status header is not enabled, then send the packets eth0 interafce
*/
static void switch_hw_receive(struct dma_device_info* dma_dev)
{
    unsigned char* buf=NULL;
    int len=0 ;
    struct sk_buff *skb=NULL;
    struct net_device *dev;

#ifdef CONFIG_LTQMIPS_DMA
    len = dma_device_read(dma_dev,&buf,(void**)&skb);
#endif
    if ((len >= 0x600) || (len < 64) ) {
        printk(KERN_ERR "%s[%d]: Packet is too large/small (%d)!!!\n", __func__,__LINE__,len);
        goto rx_err_exit;
    }
    if (skb == NULL  ) {
        printk(KERN_ERR "%s[%d]: Can't restore the Packet !!!\n", __func__,__LINE__);
        goto rx_err_exit;
    }
    /* remove CRC */
    len -= 4;
    if (len > (skb->end -skb->tail)) {
        printk(KERN_ERR "%s[%d]: len:%d end:%p tail:%p Err!!!\n", __func__,__LINE__,(len+4), skb->end, skb->tail);
        goto rx_err_exit;
    }
    if (buf) {
#ifdef DUMP_PACKET
        printk("rx len:%d\n",len);
        dump_skb(len, (char *)buf);
#endif
    }
    skb_put(skb,len);
    if(g_pmac_dma){
        int sourcePortId;;
        len -= 8;  /*Remove PMAC to DMA header */
        skb_pull(skb,8);
        eg_pkt_hdr = * ((cpu_egress_pkt_header_t *) (buf+2));
        sourcePortId = (eg_pkt_hdr.SLPID) & 0x7 ;
        /*sourcePortId = (eg_pkt_hdr.SPPID) & 0x7 ; */
        if(sourcePortId < NUM_ETH_INF) {
            switch(sourcePortId) {
                case 0:
                    dev = eth_dev[0];
                    break;
                case 1:
                    dev = eth_dev[1];
                    break;
                case 2:
                    dev = eth_dev[2];
                    break;
                case 3:
                    dev = eth_dev[3];
                    break;
                case 4:
                    dev = eth_dev[4];
                    break;
                case 5:
                    dev = eth_dev[5];
                    break;
                default:
                /*printk("%s[%d], SLPID:%d ERROR!!! \n", __FUNCTION__, __LINE__,(eg_pkt_hdr.SLPID) & 0x7);*/
                /*printk("%s[%d], SPPID:%d ERROR!!! \n", __FUNCTION__, __LINE__,(eg_pkt_hdr.SPPID) & 0x7);  */
                goto rx_err_exit;
            }
        } else {
            /*printk("%s[%d], SLPID:%d Packet dropped!!! \n", __FUNCTION__, __LINE__,(eg_pkt_hdr.SLPID) & 0x7);*/
            /*printk("%s[%d], SPPID:%d Packet dropped!!! \n", __FUNCTION__, __LINE__,(eg_pkt_hdr.SPPID) & 0x7); */
            goto rx_err_exit;
        }
    } else
        dev= eth_dev[0];

    skb->dev = dev;
    eth_rx(dev,len,skb);
    return ;

rx_err_exit:
    if (skb)
        dev_kfree_skb_any(skb);
    return ;
}

/* Get the network device stats information */
static struct net_device_stats *ltq_get_stats (struct net_device *dev)
{
    ltq_switch_priv_t *priv = netdev_priv(dev);
    return &priv->stats;
}

/* Trasmit timeout */
static void ltq_tx_timeout(struct net_device *dev)
{
    ltq_switch_priv_t *priv		= netdev_priv(dev);
#ifdef CONFIG_LTQMIPS_DMA
    struct dma_device_info* dma_dev = g_dma_device;
    int i;

    printk(KERN_ERR "%s: transmit timed out, disable the dma channel irq\n", dev->name);

    priv->stats.tx_errors++;

    for (i=0; i<dma_dev->max_tx_chan_num; i++) {
        dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);
    }
    netif_wake_queue(dev);
#endif
}

/* Set the MAC address */
static int ltq_set_mac_address (struct net_device *dev, void *p)
{
#if 0
    ltq_switch_priv_t *priv		= netdev_priv(dev);
#endif
    struct sockaddr *addr = p;

    if (netif_running(dev))
        return -EBUSY;

    if (!is_valid_ether_addr(addr->sa_data))
        return -EINVAL;

    memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);
#if 0
    /* TODO: is it required to set MAC address in the HW registers?*/
    spin_lock_irq(&priv->lock);
    if (!(dev->flags & IFF_PROMISC)) {
        __set_mac_addr(dev->dev_addr);
    }
    spin_unlock_irq(&priv->lock);
#endif
    return 0;
}

/* Change the MTU value of the netwrok device interfaces */
static int ltq_change_mtu (struct net_device *dev, int new_mtu)
{
    if(new_mtu < ETH_ZLEN || new_mtu > ETH_DATA_LEN)
        return -EINVAL;
    dev->mtu = new_mtu;
    return 0;
}

/* select the DMA channel numbers, refer dma_setup_init function */
static int select_tx_chan (struct sk_buff *skb, struct net_device *dev)
{
     int chan_no;
/*TODO: select the channel based on  criteria*/
    int dev_index = (!strcmp(dev->name, "eth0") ? 0 : 1);
    if (dev_index)
        chan_no = 1;
    else
        chan_no = 0;
    return chan_no;
}

/*
* Transmit packet over DMA, which comes from the Tx Queue
* Note: Tx packet based on the interface queue.
*       if packet comes from eth0, then sendout the packet over Tx DMA channel 0
*       if packet comes from eth1, then sendout the packet over Tx DMA channel 1
* refer the function "select_tx_chan" selection of dma channels
* if switch CPU port PMAC status header is enabled, then set the status header
*   based on criteria and push the status header infront of header.
* if head room is not availabe for status header(4-bytes), reallocate the head room
*   and push status header  infront of the header
*/
static int ltq_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    ltq_switch_priv_t *priv		= netdev_priv(dev);
    int len , rc = NETDEV_TX_OK;
    char *data;
#ifdef CONFIG_LTQMIPS_DMA
    struct dma_device_info* dma_dev=g_dma_device;
#endif

    len = skb->len < ETH_ZLEN ? ETH_ZLEN : skb->len;

    if(g_dma_pmac) {
        memset((void *) &ig_pkt_hdr, 0, sizeof (ig_pkt_hdr));
        /*if DIRECT is set 1, then packet is directly forward to the physical port based on DPID (no learning),
		if DIRECT is set 0, then switch will learn and decide forward the packet to which physical port*/

#ifdef CONFIG_SW_ROUTING_MODE
        ig_pkt_hdr.DPID_EN = 1;
#else
        ig_pkt_hdr.DPID_EN = 0;
#endif
        if(!(strcmp(dev->name, "eth0"))) {
            ig_pkt_hdr.SPID= 2;  /*CPU port */
            ig_pkt_hdr.DPID= 0;  /*Packet send through MII0 interface, valid only when DIRECT is set 1 */
        } else if(!(strcmp(dev->name, "eth1"))) {
            ig_pkt_hdr.SPID= 2;  /*CPU port */
            ig_pkt_hdr.DPID= 1; /* Send packet through  MII1 interface, valid only when DIRECT is set 1 */
            /*printk("%s[%d], eth1 \n",__FUNCTION__, __LINE__); */
        } else {
            /*if eth1 interface is not there, then send through eth0 */
            printk("%s[%d], Default eth0 \n",__FUNCTION__, __LINE__);
            ig_pkt_hdr.SPID= 2;
            ig_pkt_hdr.DPID= 0;  /*Packet send through MII0 interface, valid only when DIRECT is set 1 */
        }
        if(skb_headroom(skb)>=4) {
            /*printk("%s[%d]: [%d] \n",__FUNCTION__,__LINE__,skb_headroom(skb));   */
            skb_push(skb,4);
            memcpy(skb->data, (void*)&ig_pkt_hdr, 4);
            len+=4;
        } else {
            struct sk_buff *tmp = skb;
            skb = skb_realloc_headroom(tmp, 4);
            if(tmp)
                dev_kfree_skb_any(tmp);
            if (skb ==  NULL) {
                printk("%s skb_realloc_headroom failed\n", __func__);
                return -ENOMEM;
            }
            skb_push(skb, 4);
            memcpy(skb->data, &ig_pkt_hdr, 4);
            len += 4;
        }
    }

    data = skb->data;
    priv->skb = skb;
    dev->trans_start = jiffies;

#ifdef CONFIG_LTQMIPS_DMA
    /*select the tx channel*/
    dma_dev->current_tx_chan = select_tx_chan (skb, dev);

    if (dma_device_write(dma_dev, data, len, skb) != len ) {
        if (skb)
            dev_kfree_skb_any(skb);
        priv->stats.tx_errors++;
        priv->stats.tx_dropped++;
  /*      rc = NETDEV_TX_BUSY;  */
    } else {
        priv->stats.tx_packets++;
        priv->stats.tx_bytes+=len;
    }
#endif
    return rc;
}

/* Platform specific IOCTL's handler */
static int ltq_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
  /* TODO*/

    return -EOPNOTSUPP;
}

#ifdef CONFIG_LTQMIPS_DMA
/*
* DMA Pseudo Interrupt handler.
* This function handle the DMA pseudo interrupts to handle the data packets Rx/Tx over DMA channels
* It will handle the following interrupts
*   RCV_INT: DMA receive the packet interrupt,So get from the PPE peripheral
*   TX_BUF_FULL_INT: TX channel descriptors are not availabe, so, stop the transmission
        and enable the Tx channel interrupt.
*   TRANSMIT_CPT_INT: Tx channel descriptors are availabe and resume the transmission and
        disable the Tx channel interrupt.
*/
int dma_intr_handler(struct dma_device_info* dma_dev,int status)
{
    struct net_device* dev;
    int i;

    switch(status) {
        case RCV_INT:
            switch_hw_receive(dma_dev);
            break;
        case TX_BUF_FULL_INT:
            for(i=0; i < NUM_ETH_INF ; i++){
                dev = eth_dev[i];
                netif_stop_queue(dev);
            }
            for(i=0;i<dma_dev->max_tx_chan_num;i++) {
                if((dma_dev->tx_chan[i])->control==IFX_DMA_CH_ON)
                    dma_dev->tx_chan[i]->enable_irq(dma_dev->tx_chan[i]);
                }
            break;
        case TRANSMIT_CPT_INT:
            for(i=0;i<dma_dev->max_tx_chan_num;i++) {
                dma_dev->tx_chan[i]->disable_irq(dma_dev->tx_chan[i]);
            }
            for(i=0; i < NUM_ETH_INF ; i++){
                dev = eth_dev[i];
                netif_wake_queue(dev);
            }
            break;
    }
    return 0;
}
#endif

/*
* Allocates the buffer for ethernet packet.
* This function is invoke when DMA callback function to be called
*   to allocate a new buffer for Rx packets.*/
unsigned char* sw_dma_buffer_alloc(int len, int* byte_offset,void** opt)
{
    unsigned char *buffer=NULL;
    struct sk_buff *skb=NULL;
    int offset = 0;
#ifdef CONFIG_LTQMIPS_DMA
    int dma_burst_len = g_dma_device->rx_burst_len <<2;
#else
    int dma_burst_len = 4;
#endif
    /* for reserving 2 bytes in skb buffer, so, set offset 2 bytes infront of data pointer */
    *byte_offset=2;
    skb = dev_alloc_skb(ETH_PKT_BUF_SIZE+dma_burst_len);
    if (skb == NULL) {
        printk(KERN_ERR "%s[%d]: Buffer allocation failed!!!\n", __func__,__LINE__);
        return NULL;
    }
    if(likely(skb)) {
        if(((u32)skb->data & (dma_burst_len -1)) != 0) {
            offset = ~((u32)skb->data+(dma_burst_len -1)) & (dma_burst_len -1);
        }
        if(offset != 0 ) {
            buffer = (unsigned char *)(skb->data+offset);
            skb_reserve(skb, offset);
        } else {
            buffer = (unsigned char*)(skb->data);
        }
        skb_reserve(skb, 2);
        *(int*)opt=(int)skb;
    }
    return buffer;
}

/* Free skb buffer
* This function frees a buffer previously allocated by the DMA buffer
*   alloc callback function. */
int sw_dma_buffer_free(unsigned char* dataptr,void* opt)
{
    struct sk_buff *skb=NULL;

    if(opt==NULL){
        if(dataptr)
            kfree(dataptr);
    }else {
        skb=(struct sk_buff*)opt;
        if(skb)
            dev_kfree_skb_any(skb);
    }

    return 0;
}

#ifdef  CONFIG_IFX_NAPI
/* This function scheduled from upper layer when the NAPI is enabled*/
//static int eth_switch_poll(struct net_device *poll_dev, int *budget)
//{
//    int work_to_do, work_done, ret;
//    struct dma_device_info* dma_dev=g_dma_device;
//
//    work_to_do = min(*budget, poll_dev->quota);
//    work_done = 0;
//    ret = dma_device_poll(dma_dev, work_to_do, &work_done);
//    *budget -= work_done;
//    poll_dev->quota -= work_done;
//    return ret;
//}
static eth_fw_poll_ret_t ltq_switch_poll(struct net_device *poll_dev, int work_to_do, int *work_done)
{
    int ret;

#ifdef CONFIG_LTQMIPS_DMA
    ret = dma_device_poll(g_dma_device, work_to_do, work_done);
    return ret == 0 ? IFX_ETH_FW_POLL_COMPLETE : IFX_ETH_FW_POLL_CONTINUE;
#endif
}

static void switch_activate_poll(struct dma_device_info* dma_dev)
{
    struct net_device *dev;
    int i;

    for(i=0; i < NUM_ETH_INF ; i++) {
        dev = eth_dev[i];
//        if ( netif_rx_schedule_prep(dev) )
//        __netif_rx_schedule(dev);
        ltq_eth_fw_poll_schedule(dev);
    }
}

static void switch_inactivate_poll(struct dma_device_info* dma_dev)
{
    struct net_device *dev;
    int i;

    for(i=0; i < NUM_ETH_INF ; i++) {
        dev = eth_dev[i];
//        if(netif_running(dev) )
//            netif_rx_complete(dev);
        ltq_eth_fw_poll_complete(dev);
    }
}
#endif

/* Unregister with DMA device core driver */
static void dma_setup_uninit(void)
{
#ifdef CONFIG_LTQMIPS_DMA
    struct dma_device_info* dma_dev=g_dma_device;
    if( dma_dev ) {
        dma_device_unregister(dma_dev);
        dma_device_release(dma_dev);
    }
#endif
}

#ifdef CONFIG_LTQMIPS_DMA
/* Register with DMA device core driver */
static int ltqcpe_dma_setup_init(void)
{
    int i, ret = 0;

    g_dma_device=dma_device_reserve("PPE");
    if(!g_dma_device) {
        printk(KERN_ERR "%s[%d]: Reserved with DMA core driver failed!!!\n", __func__,__LINE__);
        return -ENODEV;
    }
    g_dma_device->buffer_alloc              =&sw_dma_buffer_alloc;
    g_dma_device->buffer_free               =&sw_dma_buffer_free;
    g_dma_device->intr_handler              =&dma_intr_handler;
    g_dma_device->num_rx_chan               = 4;
    g_dma_device->num_tx_chan               = 2;
    g_dma_device->tx_burst_len              = DMA_TX_BURST_LEN;
    g_dma_device->rx_burst_len              = DMA_RX_BURST_LEN;
    g_dma_device->tx_endianness_mode        = IFX_DMA_ENDIAN_TYPE3;
    g_dma_device->rx_endianness_mode        = IFX_DMA_ENDIAN_TYPE3;
    g_dma_device->port_packet_drop_enable   = 0;

    for (i = 0; i < g_dma_device->num_rx_chan; i++) {
        g_dma_device->rx_chan[i]->packet_size       =  ETH_PKT_BUF_SIZE;
        g_dma_device->rx_chan[i]->control           = IFX_DMA_CH_ON;
    }
    for (i = 0; i < g_dma_device->num_tx_chan; i++) {
        if ( (i == 0) || (i == 1)) /* eth0 --> DMA Tx0 channel, eth1--> DMA Tx1 channel*/
            g_dma_device->tx_chan[i]->control       = IFX_DMA_CH_ON;
        else
            g_dma_device->tx_chan[i]->control       = IFX_DMA_CH_OFF;
    }
#ifdef  CONFIG_IFX_NAPI
    g_dma_device->activate_poll     = switch_activate_poll;
    g_dma_device->inactivate_poll   = switch_inactivate_poll;
#endif
    ret = dma_device_register (g_dma_device);
    if ( ret != 0)
        printk(KERN_ERR "%s[%d]: Register with DMA core driver Failed!!!\n", __func__,__LINE__);

    return ret;
}
#endif

/* init of the network device */
static int ltq_switch_init(struct net_device *dev)
{
    u64 retval;
    static int macVal=0;
    int i;

    /*printk("%s up\n",dev->name); */

    SET_ETHTOOL_OPS(dev, &ethtool_ops);

    for ( i = 0, retval = 0; i < 6; i++ )
        retval += dev->dev_addr[i];
    if ( retval == 0 ) {
        /*read the mac address from the mac table and put them into the mac table.*/
      	for (i = 0; i < 6; i++) {
    		retval +=my_ethaddr[i];
    	}
    	/* if ethaddr not set in u-boot, then use default one */
    	if (retval == 0) {
    	    dev->dev_addr[0] = 0x00;
    		dev->dev_addr[1] = 0x20;
    		dev->dev_addr[2] = 0xda;
    		dev->dev_addr[3] = 0x86;
    		dev->dev_addr[4] = 0x23;
    		dev->dev_addr[5] = 0x74 + macVal;
    	} else {
    	    for (i = 0; i < 6; i++) {
    	        dev->dev_addr[i] = my_ethaddr[i];
    	    }
    	    dev->dev_addr[5] += + macVal ;
    	}
    	macVal++;
    }
    return 0;
}

/* Driver version info */
static inline int eth_drv_ver(char *buf)
{
    return sprintf(buf, "Lantiq ethernet driver for XWAY, version %s,(c)2009 Infineon Technologies AG\n", version);
}

#if defined (SNMP_COUNTERS_DEBUG)
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

#define RMON_COUNT_SIZE         64

static unsigned int g_rmon_counter[7][0x30] = {{0}};

void clear_rmon_counter(int port)
{
    int i;
    unsigned int  val0=0, val1=0, val2=0, val3=0, data=0;

    for(i=0;i<RMON_COUNT_SIZE;i++) {
        SW_WRITE_REG32(i, ETHSW_BM_RAM_ADDR_REG);
        SW_WRITE_REG32((0x8000| port), ETHSW_BM_RAM_CTRL_REG);
        while(  (SW_READ_REG32(ETHSW_BM_RAM_CTRL_REG) & 0x8000) == 1  ) {
            ;
        }
        val3 = SW_READ_REG32(ETHSW_BM_RAM_VAL_3_REG);
        val2 = SW_READ_REG32(ETHSW_BM_RAM_VAL_2_REG);
        val0 = SW_READ_REG32(ETHSW_BM_RAM_VAL_0_REG);
        val1 = SW_READ_REG32(ETHSW_BM_RAM_VAL_1_REG);
        data = (val1 << 16) | (val0);
        g_rmon_counter[port][i] = data;
    }
}

int print_rmon_counter(int  port)
{
    int i;
    unsigned int  val0=0, val1=0, val2=0, val3=0, data=0;
    printk("RMON counter for Port: %d\n",port);
    for(i=0;i<RMON_COUNT_SIZE;i++) {
        SW_WRITE_REG32(i, ETHSW_BM_RAM_ADDR_REG);
        SW_WRITE_REG32((0x8000| port), ETHSW_BM_RAM_CTRL_REG);
        while(  (SW_READ_REG32(ETHSW_BM_RAM_CTRL_REG) & 0x8000) == 1  ) {
            ;
        }
        val3 = SW_READ_REG32(ETHSW_BM_RAM_VAL_3_REG);
        val2 = SW_READ_REG32(ETHSW_BM_RAM_VAL_2_REG);
        val0 = SW_READ_REG32(ETHSW_BM_RAM_VAL_0_REG);
        val1 = SW_READ_REG32(ETHSW_BM_RAM_VAL_1_REG);
        data = (val1 << 16) | (val0);
        data -= g_rmon_counter[port][i];
        switch(i) {
            case 0x1F :
                printk("Receive Frame Count                    :0x%08x\n", data);
                break;
            case 0x23 :
                printk("Receive Unicast Frame Count            :0x%08x\n",data);
                break;
            case 0x22 :
                printk("Receive Multicast Frame Count          :0x%08x\n",data);
                break;
            case 0x21 :
                printk("Receive CRC errors Count               :0x%08x\n",data);
                break;
            case 0x1D :
                printk("Receive Undersize good Count           :0x%08x\n",data);
                break;
            case 0x1E :
                printk("Receive Undersize bad Count            :0x%08x\n",data);
                break;
            case 0x1B :
                printk("Receive Oversize good Count            :0x%08x\n",data);
                break;
            case 0x1C :
                printk("Receive Oversize bad Count             :0x%08x\n",data);
                break;
            case 0x20 :
                printk("Receive Pause good Count               :0x%08x\n",data);
                break;
            case 0x1A :
                printk("Receive Alignment errors Count         :0x%08x\n",data);
                break;
            case 0x12 :
                printk("Receive size 64Bytes Frame Count       :0x%08x\n",data);
                break;
            case 0x13 :
                printk("Receive size 65-127Bytes Frame Count   :0x%08x\n",data);
                break;
            case 0x14 :
                printk("Receive size 128-255Bytes Frame Count  :0x%08x\n",data);
                break;
            case 0x15 :
                printk("Receive size 256-511Bytes Frame Count  :0x%08x\n",data);
                break;
            case 0x16 :
                printk("Receive size 512-1023Bytes Frame Count :%08x\n",data);
                break;
            case 0x17 :
                printk("Receive size >1024Bytes Frame Count    :%08x\n",data);
                break;
            case 0x18 :
                printk("Receive Discard (Tail-Drop) frame Count:0x%08x\n",data);
                break;
            case 0x19 :
                printk("Receive Drop ( Filter) frame Count     :0x%08x\n",data);
                break;
            case 0x24 :
                printk("Receive Good Byte Count (Low)          :%08x\n",data);
                break;
            case 0x25 :
                printk("Receive Good Byte Count(High)          :%08x\n",data);
                break;
            case 0x26 :
                printk("Receive Bad Byte Count (Low)           :%08x\n",data);
                break;
            case 0x27 :
                printk("Receive Bad Byte Count(High)           :%08x\n",data);
                break;
            case 0x11 :
                printk("Receive Discard (Acive Congestion Management) frame Count:%08x\n",data);
                break;
            case 0x0C :
                printk("Transmit Frame Count                        :%08x\n",data);
                break;
            case 0x06 :
                printk("Transmit Unicast Frame Count                :%08x\n",data);
                break;
            case 0x07 :
                printk("Transmit Multicast Frame Count              :%08x\n",data);
                break;
            case 0x00 :
                printk("Transmit size 64Bytes Frame Count           :%08x\n",data);
                break;
            case 0x01 :
                printk("Transmit size 65-127Bytes Frame Count       :%08x\n",data);
                break;
            case 0x02 :
                printk("Transmit size 128-255Bytes Frame Count      :%08x\n",data);
                break;
            case 0x03 :
                printk("Transmit size 256-511Bytes Frame Count      :%08x\n",data);
                break;
            case 0x04 :
                printk("Transmit size 512-1023Bytes Frame Count     :%08x\n",data);
                break;
            case 0x05 :
                printk("Transmit size >1024Bytes Frame Count        :%08x\n",data);
                break;
            case 0x08 :
                printk("Transmit Single Collision Count             :%08x\n",data);
                break;
            case 0x09 :
                printk("Transmit Multiple Collision Count           :%08x\n",data);
                break;
            case 0x0A :
                printk("Transmit Late Collision Count               :%08x\n",data);
                break;
            case 0x0B :
                printk("Transmit Excessive Collision Count          :%08x\n",data);
                break;
            case 0x0D :
                printk("Transmit Pause Frame Count                  :%08x\n",data);
                break;
            case 0x10 :
                printk("Transmit Dropped Frame Count                :%08x\n",data);
                break;
            case 0x0E :
                printk("Transmit Good Byte Count (Low)              :%08x\n",data);
                break;
            case 0x0F :
                printk("Transmit Good Byte Count(High)              :%08x\n",data);
                break;
            }
        }
        return 0;
}

static int get_token(char **p1, char **p2, int *len, int *colon)
{
    int tlen = 0;
    while ( *len && !((**p1 >= 'A' && **p1 <= 'Z')
            || (**p1 >= 'a' && **p1<= 'z')
            || (**p1 >= '0' && **p1<= '9')) ) {
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

static int proc_read_rmon(struct file *file, const char *buf, unsigned long count, void *data)
{
    char local_buf[2048];
    int len, colon = 0, port = -1;
    char *p1, *p2;

    len = sizeof(local_buf) < count ? sizeof(local_buf) - 1 : count;
    len = len - copy_from_user(local_buf, buf, len);
    local_buf[len] = 0;
    p1 = local_buf;
    while ( get_token(&p1, &p2, &len, &colon) ) {
        if ( stricmp(p1, "p0") == 0 ) {
            port= 0;
        } else if ( stricmp(p1, "p1") == 0 ) {
            port= 1;
        } else if ( stricmp(p1, "p2") == 0 ) {
            port= 2;
        } else if ( stricmp(p1, "p3") == 0 ) {
            port= 3;
        } else if ( stricmp(p1, "p4") == 0 ) {
            port= 4;
        } else if ( stricmp(p1, "p5") == 0 ) {
            port= 5;
        } else if ( stricmp(p1, "p6") == 0 ) {
            port= 6;
        } else if ( stricmp(p1, "clear") == 0 || stricmp(p1, "clean") == 0 ) {
            int i;

            for ( i = 0; i < 7; i++ )
                clear_rmon_counter(i);
            break;
        } else if ( stricmp(p1, "help") == 0 || strcmp(p1, "?") == 0 ) {
            printk("echo px[x:0~6] > /proc/driver/7port_sw/read_rmon_counters\n");
            printk("echo clean > /proc/driver/7port_sw/read_rmon_counters\n");
            break;
        }
        p1 = p2;
    }

    if (port >= 0 && port <= 6)
        print_rmon_counter(port);
    return count;
}
#endif /*SNMP_COUNTERS_DEBUG*/

/* Displays the version of ETH module via proc file */
static int eth_proc_version(struct seq_file *m, void *v)
{
    /* No sanity check cos length is smaller than one page */
    seq_printf (m, "Lantiq ethernet driver for XWAY, version %s,(c)2009 Infineon Technologies AG\n", version);
    return 0;
}

static int print_pce_entry(char *buf, int index)
{
#define VR9_SWIP_MACRO                          (KSEG1 | 0x1E108000)
#define VR9_SWIP_MACRO_REG(off)                 ((volatile u32*)(VR9_SWIP_MACRO + (off) * 4))
#define VR9_SWIP_TOP                            (VR9_SWIP_MACRO | (0x0C40 * 4))
#define VR9_SWIP_TOP_REG(off)                   ((volatile u32*)(VR9_SWIP_TOP + (off) * 4))
//  Parser & Classification Engine
#define PCE_TBL_KEY(n)                          VR9_SWIP_MACRO_REG(0x440 + 7 - (n))                 //  n < 7
#define PCE_TBL_MASK                            VR9_SWIP_MACRO_REG(0x448)
#define PCE_TBL_VAL(n)                          VR9_SWIP_MACRO_REG(0x449 + 4 - (n))                 //  n < 4;
#define PCE_TBL_ADDR                            VR9_SWIP_MACRO_REG(0x44E)
#define PCE_TBL_CTRL                            VR9_SWIP_MACRO_REG(0x44F)
#define PCE_TBL_STAT                            VR9_SWIP_MACRO_REG(0x450)
#define PCE_GCTRL_REG(reg)                      VR9_SWIP_MACRO_REG(0x456 + (reg))
#define PCE_PCTRL_REG(port, reg)                VR9_SWIP_MACRO_REG(0x480 + (port) * 0xA + (reg))    //  port < 12, reg < 4

    static char *out_fields[] = {
        "OUT_MAC0  ",   //  0
        "OUT_MAC1  ",   //  1
        "OUT_MAC2  ",   //  2
        "OUT_MAC3  ",   //  3
        "OUT_MAC4  ",   //  4
        "OUT_MAC5  ",   //  5
        "OUT_ETHTYP",   //  6
        "OUT_VTAG0 ",   //  7
        "OUT_VTAG1 ",   //  8
        "OUT_ITAG0 ",   //  9
        "OUT_ITAG1 ",   //  10
        "OUT_ITAG2 ",   //  11
        "OUT_ITAG3 ",   //  12
        "OUT_IP0   ",   //  13
        "OUT_IP1   ",   //  14
        "OUT_IP2   ",   //  15
        "OUT_IP3   ",   //  16
        "OUT_SIP0  ",   //  17
        "OUT_SIP1  ",   //  18
        "OUT_SIP2  ",   //  19
        "OUT_SIP3  ",   //  20
        "OUT_SIP4  ",   //  21
        "OUT_SIP5  ",   //  22
        "OUT_SIP6  ",   //  23
        "OUT_SIP7  ",   //  24
        "OUT_DIP0  ",   //  25
        "OUT_DIP1  ",   //  26
        "OUT_DIP2  ",   //  27
        "OUT_DIP3  ",   //  28
        "OUT_DIP4  ",   //  29
        "OUT_DIP5  ",   //  30
        "OUT_DIP6  ",   //  31
        "OUT_DIP7  ",   //  32
        "OUT_SESID ",   //  33
        "OUT_PROT  ",   //  34
        "OUT_APP0  ",   //  35
        "OUT_APP1  ",   //  36
        "OUT_IGMP0 ",   //  37
        "OUT_IGMP1 ",   //  38
        "OUT_IPOFF ",   //  39
        "OUT_NONE  ",   //  63
    };
    static char *types[] = {
        "INSTR  ",  //  0
        "IPV6   ",  //  1
        "LENACCU",  //  2
    };
    static char *flags[] = {
        "FLAG_ITAG ",  //  0
        "FLAG_VLAN ",  //  1
        "FLAG_SNAP ",  //  2
        "FLAG_PPPOE",  //  3
        "FLAG_IPV6 ",  //  4
        "FLAG_IPV6F",  //  5
        "FLAG_IPV4 ",  //  6
        "FLAG_IGMP ",  //  7
        "FLAG_TU   ",  //  8
        "FLAG_HOP  ",  //  9
        "FLAG_NN1  ",  //  10
        "FLAG_NN2  ",  //  11
        "FLAG_END  ",  //  12
        "FLAG_NO   ",  //  13
    };
    static char *titles[] = {
        "0 : IFXTAG    ",
        "1 : C_VTAG    ",
        "2 : ET_IPV4   ",
        "3 : ET_IPV6   ",
        "4 : ET_PPPOE_S",
        "5 : ET_PPPOE_D",
        "6 : S_VTAG    ",
        "7 : C_VTAG2   ",
        "8 : EL_LES_800",
        "9 : ET_OTHER  ",
        "10: EL_GRE_600",
        "11: EL_LES_600",
        "12: SNAP1     ",
        "13: NO_SNAP1  ",
        "14: SNAP2     ",
        "15: NO_SNAP2  ",
        "16: SESID_IGN ",
        "17: SESID     ",
        "18: PPPOE_IP  ",
        "19: PPPOE_NOIP",
        "20: IPV4_VER  ",
        "21: IPV6_VER  ",
        "22: NO_IP     ",
        "23: IPV4_UDP1 ",
        "24: IPV4_TCP  ",
        "25: IPV4_IGMP1",
        "26: IPV4_OTH1 ",
        "27: IPV4_UDP2 ",
        "28: IPV4_UDP3 ",
        "29: IPV4_OTH2 ",
        "30: IPV4_OTH3 ",
        "31: IPV4_IGMP2",
        "32: IPV4_IGMP3",
        "33: IPV4_IGMP4",
        "34: IPV6_UDP  ",
        "35: IPV6_TCP  ",
        "36: IPV6_HOP  ",
        "37: IPV6_ROU  ",
        "38: IPV6_DES  ",
        "39: IPV6_OTH  ",
        "40: NXT_HD_UDP",
        "41: NXT_HD_TCP",
        "42: NXT_HD_HOP",
        "43: NXT_HD_ROU",
        "44: NXT_HD_DES",
        "45: NXT_HD_OTH",
        "46: TU_IP     ",
        "47: TU_PORTS  ",
        "48: IPV6_IP   ",
        "49: END       ",
        "50: END       ",
        "51: END       ",
        "52: END       ",
        "53: END       ",
        "54: END       ",
        "55: END       ",
        "56: END       ",
        "57: END       ",
        "58: END       ",
        "59: END       ",
        "60: END       ",
        "61: END       ",
        "62: END       ",
        "63: END       ",
    };

    int len = 0;
    unsigned int val[4];
    unsigned int value;
    unsigned int mask;
    unsigned int ns;
    char *p_out_field;
    unsigned int L;
    char *p_type;
    char *p_flag;
    unsigned int ipv4_len;
    char *p_title;
    unsigned int tmp;

    if ( index < 0 )
    {
        len += sprintf(buf + len, "//------------------------------------------------------------------------------------------\n");
        len += sprintf(buf + len, "//                value    mask   ns  out_fields   L  type     flags       ipv4_len\n");
        len += sprintf(buf + len, "//------------------------------------------------------------------------------------------\n");
    }
    else
    {
        while ( (*PCE_TBL_CTRL & (1 << 15)) );
        *PCE_TBL_ADDR   = index;
        *PCE_TBL_CTRL   = 0x8000;   //  read micro code
        while ( (*PCE_TBL_CTRL & (1 << 15)) );
        val[3] = *PCE_TBL_VAL(3);
        val[2] = *PCE_TBL_VAL(2);
        val[1] = *PCE_TBL_VAL(1);
        val[0] = *PCE_TBL_VAL(0);

        value = val[3] & 0xFFFF;
        mask = val[2] & 0xFFFF;
        ns = (val[1] >> 10) & ((1 << 6) - 1);
        tmp = (val[1] >> 4) & ((1 << 6) - 1);
        if ( tmp <= 39 )
            p_out_field = out_fields[tmp];
        else if ( tmp == 63 )
            p_out_field = out_fields[40];
        else
            p_out_field = "reserved";
        L = ((val[1] & ((1 << 4) - 1)) << 1) | ((val[0] >> 15) & 0x01);
        tmp = (val[0] >> 13) & ((1 << 2) - 1);
        if ( tmp <= sizeof(types) / sizeof(*types) )
            p_type = types[tmp];
        else
            p_type = "reserved";
        tmp = (val[0] >> 9) & ((1 << 4) - 1);
        if ( tmp <= sizeof(flags) / sizeof(*flags) )
            p_flag = flags[tmp];
        else
            p_flag = "reserved";
        ipv4_len = (val[0] >> 8) & 0x01;
        tmp = index;
        if ( tmp <= sizeof(titles) / sizeof(*titles) )
            p_title = titles[tmp];
        else
            p_title = "reserved";
        len += sprintf(buf + len, "LANTIQ_FLOW_PCE_MC_M(0x%04X, 0x%04X, %-2d, %s, %2d, %s, %s, %d),   // %s\n",
                       value, mask, ns, p_out_field, L, p_type, p_flag, ipv4_len, p_title);
    }

    return len;
}

static int proc_read_pce(struct seq_file *m, void *v)
{

#if 0
    int len = 0;
    int len_max = off + count;
    char *pstr;
    char str[1024];
    int llen;

    int i;

    pstr = *start = page;

    for ( i = -1; i < 64; i++ )
    {
        llen = print_pce_entry(str, i);
        if ( len <= off && len + llen > off )
        {
            memcpy(pstr, str + off - len, len + llen - off);
            pstr += len + llen - off;
        }
        else if ( len > off )
        {
            memcpy(pstr, str, llen);
            pstr += llen;
        }
        len += llen;
        if ( len >= len_max )
            goto PROC_READ_PCE_OVERRUN_END;
    }

    *eof = 1;

    return len - off;

PROC_READ_PCE_OVERRUN_END:
    return len - llen - off;
#endif
	return 0;
}

static int rversion_open (struct inode *inode, struct file *file)
{
	return single_open(file, eth_proc_version, NULL);
}

static int rpce_open (struct inode *inode, struct file *file)
{
	return single_open(file, proc_read_pce, NULL);
}


/* create proc for debug  info, eth_module_init */
static int eth_proc_create(void)
{
    /* procfs */
    g_eth_proc_dir = proc_mkdir("driver/7port_sw",NULL);
    if (g_eth_proc_dir == NULL) {
        printk(KERN_ERR "%s: Create proc directory (/driver/7port_sw) failed!!!\n", __func__);
        return -EIO;
    }
    //create_proc_read_entry("version", 0, g_eth_proc_dir, eth_proc_version,  NULL);
    proc_create ("version", S_IRUGO, g_eth_proc_dir, &rversion_ops);
#if defined (SNMP_COUNTERS_DEBUG)
    {
        struct proc_dir_entry *res;
        res = create_proc_entry("read_rmon_counters", 0, g_eth_proc_dir );
        if ( res ) {
            res->read_proc  = NULL;
            res->write_proc = proc_read_rmon;
        }
    }
#endif /* SNMP_COUNTERS_DEBUG */
    //create_proc_read_entry("pce", 0, g_eth_proc_dir, proc_read_pce,  NULL);
    proc_create ("pce", S_IRUGO, g_eth_proc_dir, &rpce_ops);
    return 0;
}

/* remove of the proc entries, eth_module_exit */
static void eth_proc_delete(void)
{

    remove_proc_entry("version", g_eth_proc_dir);
#if defined (SNMP_COUNTERS_DEBUG)
    remove_proc_entry("read_rmon_counters", g_eth_proc_dir);
#endif
    remove_proc_entry("pce", g_eth_proc_dir);
    remove_proc_entry("driver/7port_sw",  NULL);
}

/* Initilization  Ethernet module */
static int ltq_eth_drv_init (void)
{
    int  i,  err;
    unsigned int reg;
    char ver_str[128] = {0};
    ltq_switch_priv_t* priv;
    g_pmac_dma = 0;
    g_dma_pmac = 0;

#ifdef CONFIG_LTQMIPS_DMA
     /* Register with DM core driver */
   err = ltqcpe_dma_setup_init();
#endif

     /* HW init of the Switch */
    vr9_7port_sw_hw_init();

    for (i = 0; i < NUM_ETH_INF ; i++) {
        char name[16];
        sprintf(name, "eth%d", i);
        eth_dev[i] = alloc_etherdev(sizeof(ltq_switch_priv_t));
        if (!eth_dev[i]) {
            printk(KERN_ERR "%s[%d]: no memory for eth_dev!!!\n", __func__,__LINE__);
            err = -ENOMEM;
            goto err_out_free_res;
        }

		/* setup the network device */
		strcpy(eth_dev[i]->name, name);
		eth_dev[i]->netdev_ops = &ltq_eth_drv_ops;
		eth_dev[i]->watchdog_timeo = LTQ_TX_TIMEOUT;

		/* setup the private data */
        priv = netdev_priv(eth_dev[i]);
        priv->phy_addr = i;

        /* By default, advertise supported  speed/duplex settings. */
        priv->flags |= (FLAG_ADV_10HALF         \
                        | FLAG_ADV_10FULL       \
                        | FLAG_ADV_100HALF      \
                        | FLAG_ADV_100FULL      \
                        | FLAG_ADV_1000HALF     \
                        | FLAG_ADV_1000FULL);

    	/* By default, auto-negotiate PAUSE. */
        priv->flags |= FLAG_PAUSE_AUTO;
        spin_lock_init(&priv->lock);
        err = register_netdev(eth_dev[i]);
        if ( err ) {
            printk(KERN_ERR "%s[%d]: Register with network device failed!!!\n", __func__,__LINE__);
            goto err_out_free_res;
        }
    }

    reg = SW_READ_REG32(PMAC_HD_CTL_REG );
    if (reg  & PMAC_HD_CTL_AS )
        g_pmac_dma = 1;
    if (reg  & PMAC_HD_CTL_RXSH )
        g_dma_pmac = 1;

    if (eth_proc_create() != 0 )
        goto err_out_free_res;
 /* Print the driver version info */
    eth_drv_ver(ver_str);
    printk(KERN_INFO "%s", ver_str);
    printk("g_dma_pmac:%d, g_pmac_dma:%d, reg:0x%08x !!!\n",g_dma_pmac, g_pmac_dma,reg);
    return  0;

err_out_free_res:
/*Unregister with DMA core driver */
    dma_setup_uninit();
    /* unregister the network devices */
    for (i=0; i< NUM_ETH_INF ; i++) {
        if(eth_dev[i])
            free_netdev(eth_dev[i]);
    }
    return err;
}

static void  ltq_eth_drv_exit (void)
{
    int i;

    /* unregister the network devices */
    for (i=0; i< NUM_ETH_INF ; i++) {
        unregister_netdev(eth_dev[i]);
        free_netdev(eth_dev[i]);
    }
    /*Unregister with DMA core driver */
    dma_setup_uninit();
    /* remove of the proc entries */
    eth_proc_delete();
}

static int ltq_eth_drv_probe(struct platform_device *pdev)
{

	/* Just do the init */
	ltq_eth_drv_init ();

	return 0;
}

static int ltq_eth_drv_remove(struct platform_device *pdev)
{
	/* Just do the exit */
	ltq_eth_drv_exit ();
	return 0;
}

static const struct of_device_id ltq_eth_drv_match[] = {
	{ .compatible = "lantiq,xway-net" },
	{},
};
MODULE_DEVICE_TABLE(of, ltq_eth_drv_match);

static struct platform_driver ltq_eth_driver = {
	.probe = ltq_eth_drv_probe,
	.remove = ltq_eth_drv_remove,
	.driver = {
		.name = "xway-net",
		.of_match_table = ltq_eth_drv_match,
		.owner = THIS_MODULE,
	},
};

module_platform_driver(ltq_eth_driver);

MODULE_AUTHOR("Reddy Mallikarjuna");
MODULE_DESCRIPTION("Lantiq ethernet driver (Supported XRX200/XRX3XX)");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_MODULE_VERSION);
