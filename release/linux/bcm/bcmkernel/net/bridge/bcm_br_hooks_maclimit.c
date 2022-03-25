/*
*    Copyright (c) 2003-2020 Broadcom
*    All Rights Reserved
*
<:label-BRCM:2020:DUAL/GPL:standard

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation (the "GPL").

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

:>
*/
#include "bcm_br_hooks_maclimit.h"

#define NORMAL_DROP_PACKET(m) ((m)->max && (((m)->learning_count+(m)->reserve) >= (m)->max))
#define ZERO_DROP_PACKET(m)   ((!(m)->max) && (m)->max_zero_drop)
#define NEED_DROP_PACKET(m)   (NORMAL_DROP_PACKET(m) || ZERO_DROP_PACKET(m))
#define HAVE_RESERVE_ENTRY(m) ((m)->learning_count < (m)->min)

mac_limit_rcv_hook_t _rcv_ex = NULL;
mac_learning_notify_hook_t  _notify_ex = NULL;
mac_learning_update_hook_t _update_ex = NULL;
extern spinlock_t mac_limit_spinlock;

int bcm_mac_limit_en = 0;       /* mac limit default disabled */
EXPORT_SYMBOL(bcm_mac_limit_en);

void bcm_mac_limit_learning_notify(struct net_bridge *br, const struct net_bridge_fdb_entry *fdb, int type)
{
    struct net_device *dev;
    struct mac_limit *dev_mac_limit;
    struct mac_limit *br_mac_limit;
    int br_update = 0;
    
    if (!fdb || !fdb->dst || fdb->is_local)
        return;
    
    dev = fdb->dst->dev;
    dev_mac_limit = &dev->bcm_nd_ext.mac_limit;
    br_mac_limit = &br->dev->bcm_nd_ext.mac_limit;

    spin_lock(&mac_limit_spinlock);
    br_update = dev_mac_limit->enable;
    if (type == RTM_DELNEIGH)
    {
       dev_mac_limit->learning_count--;
       if (br_update)
       {
          if (HAVE_RESERVE_ENTRY(dev_mac_limit))
          {
              br_mac_limit->reserve++;
          }
          br_mac_limit->learning_count--;
       }
    }
    else if (type == RTM_NEWNEIGH)
    {
        if (br_update)
        {
            if (HAVE_RESERVE_ENTRY(dev_mac_limit))
            {
                br_mac_limit->reserve--;
            }
            br_mac_limit->learning_count++;
        }
        dev_mac_limit->learning_count++;
    }
    
    if (_notify_ex)
        _notify_ex(dev, type);
    
    spin_unlock(&mac_limit_spinlock);
}

void bcm_mac_limit_learning_update(struct net_bridge_fdb_entry *fdb,struct net_bridge_port *source)
{
    struct net_device *dev, *br_dev;
    struct mac_limit *dev_mac_limit;
    struct mac_limit *br_mac_limit;
    int br_update = 0;
    
    if (!fdb || !fdb->dst || !source)
        return;
    
    dev = fdb->dst->dev;
    br_dev = source->br->dev;
    dev_mac_limit = &dev->bcm_nd_ext.mac_limit;
    br_mac_limit = &br_dev->bcm_nd_ext.mac_limit;

    spin_lock(&mac_limit_spinlock);
    br_update = dev_mac_limit->enable;
    /* mac move case, delete obselete then notify new */
    if (unlikely(source != fdb->dst))
    {
       dev_mac_limit->learning_count--;
       if (br_update)
       {
          if (HAVE_RESERVE_ENTRY(dev_mac_limit))
          {
              br_mac_limit->reserve++;
          }
          br_mac_limit->learning_count--;
       }
           
       if (_update_ex)
           _update_ex(dev, 0);
    }
    spin_unlock(&mac_limit_spinlock);
}

static unsigned int _bcm_mac_limit_rcv(struct sk_buff *skb)
{
    struct net_device *dev = skb->dev;
    struct net_bridge_port *p = br_port_get_rcu(skb->dev);
    struct mac_limit *dev_mac_limit = &dev->bcm_nd_ext.mac_limit;
    struct net_device *br_dev;
    struct mac_limit *br_mac_limit;
    struct net_bridge_fdb_entry *fdb;
    u16 vid = 0;
        
    if (!br_port_exists(dev) || !dev_mac_limit->enable)
        return NF_ACCEPT;
    
    br_vlan_get_tag(skb, &vid);  
    fdb = br_fdb_find_rcu(p->br, eth_hdr(skb)->h_source, vid);
    if (unlikely(!fdb || (dev != fdb->dst->dev)))
    {
        br_dev = p->br->dev;
        br_mac_limit = &br_dev->bcm_nd_ext.mac_limit;
        if (NEED_DROP_PACKET(dev_mac_limit))
        {
            dev_mac_limit->drop_count++;
            return NF_DROP;
        }

        if (NEED_DROP_PACKET(br_mac_limit) && !HAVE_RESERVE_ENTRY(dev_mac_limit))
        {
            br_mac_limit->drop_count++;
            return NF_DROP;
        }
        
        if (_rcv_ex)
            return _rcv_ex(dev);
        else
            return NF_ACCEPT;
    }
    else
        return NF_ACCEPT;
}

unsigned int bcm_br_fdb_mac_limit(struct sk_buff *skb)
{
    if (!bcm_mac_limit_en)
        return 0;
    else
        return (_bcm_mac_limit_rcv(skb) == NF_DROP);
}