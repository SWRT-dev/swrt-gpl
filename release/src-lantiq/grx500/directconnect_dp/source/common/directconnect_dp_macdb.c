/*
 * DirectConnect provides a common interface for the network devices to achieve the full or partial 
   acceleration services from the underlying packet acceleration engine
 * Copyright (c) 2017, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

/* Includes */
#include <linux/module.h>
#include <linux/version.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#include <linux/kthread.h>
#include <linux/if_vlan.h>
#include <linux/jhash.h>

#include <directconnect_dp_api.h>
#include <directconnect_dp_dcmode_api.h>
#include "directconnect_dp_device.h"
#include "directconnect_dp_dcmode_wrapper.h"
#include "directconnect_dp_macdb.h"

#define MACDB_HASH_BITS 8
#define MACDB_HASH_SIZE (1 << MACDB_HASH_BITS)

struct macdb_entry {
    struct hlist_node hlist;
    unsigned char addr[ETH_ALEN];
    struct net_device *dev;
    struct net_device *real_dev;
    struct rcu_head rcu;
};

static struct socket *g_nl_sock = NULL;
static struct task_struct *evt_rx_thread = NULL;

struct hlist_head macdb_hash[MACDB_HASH_SIZE];
static struct kmem_cache *macdb_cache __read_mostly;
static u32 macdb_salt __read_mostly;

static inline int
mac_hash(const unsigned char *mac)
{
    /* use 1 byte of OUI and 3 bytes of NIC */
    u32 key = get_unaligned((u32 *)(mac + 2));
    return jhash_1word(key, macdb_salt) & (MACDB_HASH_SIZE - 1);
}

static struct
macdb_entry *macdb_find(struct hlist_head *head,
                            const unsigned char *addr)
{
    struct macdb_entry *m;

    hlist_for_each_entry(m, head, hlist) {
        if (ether_addr_equal(m->addr, addr))
            return m;
    }
    return NULL;
}

static struct
macdb_entry *macdb_add(struct hlist_head *head,
                       const unsigned char *addr,
                       struct net_device *dev,
                       struct net_device *real_dev)
{
    struct macdb_entry *m;

    m = kmem_cache_alloc(macdb_cache, GFP_ATOMIC);
    if (m) {
        memcpy(m->addr, addr, ETH_ALEN);
        m->dev = dev;
        m->real_dev = real_dev;
        hlist_add_head_rcu(&m->hlist, head);
    }
    return m;
}

static void
macdb_free_rcu(struct rcu_head *head)
{
    struct macdb_entry *m =
        container_of(head, struct macdb_entry, rcu);
    kmem_cache_free(macdb_cache, m);
}

static inline void
macdb_delete(struct macdb_entry *m)
{
    hlist_del_rcu(&m->hlist);
    call_rcu(&m->rcu, macdb_free_rcu);
}

static inline void
macdb_delete_sync(struct macdb_entry *m)
{
    hlist_del(&m->hlist);
    kmem_cache_free(macdb_cache, m);
}

static void
macdb_flush(void)
{
    int i;

    for (i = 0; i < MACDB_HASH_SIZE; i++) {
        struct macdb_entry *m;
        struct hlist_node *n;

        hlist_for_each_entry_safe(m, n, &macdb_hash[i], hlist)
            macdb_delete_sync(m);
    }
}

static int
dc_dp_rx_event(void *data)
{
    int32_t ret = 0;
    struct sock *sk = (struct sock *)data;
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    struct ndmsg *ndm;
    struct nlattr *tb[NDA_MAX+1];
    uint8_t *addr;
    struct net_device *dev, *old_dev;
    struct net_device *real_dev, *old_real_dev;
    bool updated;
    struct dc_dp_priv_dev_info *p_devinfo;
    struct hlist_head *macdb_head;
    struct macdb_entry *m;

    do {
        /* Receive the data packet (blocking) */
        skb = skb_recv_datagram(sk, 0, 0, &ret);
        if (!skb) {
            pr_debug("DC DP: Invalid netlink data!\n");
            continue;
        }

        /* Validate netlink message */
        nlh = (struct nlmsghdr *)skb->data;
        if (!nlh || !nlmsg_ok(nlh, skb->len)) {
            pr_debug("DC DP: Invalid netlink header data!\n");
            goto drop;
        }

        /*
         * FIXME:
         * For the routed interface, ndm_family=AF_INET/AF_INET6 can be used
         */
        ndm = nlmsg_data(nlh);
        if (((nlh->nlmsg_type != RTM_NEWNEIGH) &&
             (nlh->nlmsg_type != RTM_DELNEIGH)) ||
            (ndm->ndm_family != AF_BRIDGE) ||
            (ndm->ndm_state & NUD_PERMANENT)) {
            pr_debug("DC DP: nlmsg with other nlmsg_type=%d ndm_family=%d"
                     " ndm_state=%d!\n", nlh->nlmsg_type,
                     ndm->ndm_family, ndm->ndm_state);
            goto drop;
        }

        /* Extract associated MAC */
#if LINUX_VERSION_CODE > KERNEL_VERSION(4,11,0)
        ret = nlmsg_parse(nlh, sizeof(*ndm), tb, NDA_MAX, NULL, NULL);
#else
        ret = nlmsg_parse(nlh, sizeof(*ndm), tb, NDA_MAX, NULL);
#endif
        if (ret < 0) {
            pr_debug("DC DP: nlmsg with invalid data\n");
            goto drop;
        }

        if (!tb[NDA_LLADDR] || nla_len(tb[NDA_LLADDR]) != ETH_ALEN) {
            pr_debug("DC DP: nlmsg with invalid address!\n");
            goto drop;
        }

        addr = nla_data(tb[NDA_LLADDR]);

        /* Extract associated interface */
        dev = __dev_get_by_index(&init_net, ndm->ndm_ifindex);
        if (dev == NULL) {
            pr_debug("DC DP: nlmsg with unknown ifindex!\n");
            goto drop;
        }

        /* Extract underlying real interface (only VLAN supported, FIXME) */
        real_dev = dev;
        if (is_vlan_dev(dev))
            real_dev = vlan_dev_real_dev(dev);

        updated = false;
        macdb_head = &macdb_hash[mac_hash(addr)];
        m = macdb_find(macdb_head, addr);
        switch (nlh->nlmsg_type) {
        case RTM_NEWNEIGH:
            pr_debug("DC DP: RTM_NEWNEIGH event for mac=%pM on dev=%s\n",
                     addr, dev->name);

            /*
             * For a new <mac,iface> association, create a new entry in macdb.
             * For a updated <mac,iface> association, update the macdb entry
             *   and for old <mac,iface> association,
             *     notify through peripheral callback (if any) and
             *     platform api call to flush acceleration entries.
             */
            if (likely(!m))
                macdb_add(macdb_head, addr, dev, real_dev);
            else if (likely(dev != m->dev)) {
                old_dev = m->dev;
                old_real_dev = m->real_dev;
                m->dev = dev;
                m->real_dev = real_dev;

                updated = true;
            }
            break;

        case RTM_DELNEIGH:
            pr_debug("DC DP: RTM_DELNEIGH event for mac=%pM on dev=%s\n",
                     addr, dev->name);

            /*
             * For the existing <mac,iface> association, delete macdb entry,
             *   notify through peripheral callback (if any) and
             *   platform api call to flush acceleration entries.
             */
            if (likely(!m || (dev == m->dev))) {
                old_dev = dev;
                old_real_dev = real_dev;
                updated = true;

                if (likely(m))
                    macdb_delete(m);
            }

            break;
        }

        if (updated) {
            pr_debug("DC DP: Notify for mac=%pM on dev=%s (phys=%s)\n",
                     addr, old_dev->name, old_real_dev->name);

            p_devinfo = NULL;
            dc_dp_get_device_by_subif_netif(old_real_dev, &p_devinfo);

            /* Notify MAC update to the peripheral driver, if any */
            if (p_devinfo && p_devinfo->cb.notify_mac_fn)
                p_devinfo->cb.notify_mac_fn(old_real_dev, addr, 0);

            /* Flush acceleration entries for the MAC, except platform/GRX500 */
            if (!IS_ENABLED(CONFIG_PPA_BR_MAC_LEARNING))
                dc_dp_dcmode_wrapper_disconn_if(p_devinfo, old_dev, NULL, addr, 0);
        }

drop:
        skb_free_datagram(sk, skb);
    } while (!kthread_should_stop());

    return 0;
}

void
dc_dp_macdb_seq_dump(struct seq_file *seq)
{
    int i;

    seq_printf(seq, "%-17s\t%-16s\n", "mac addr", "iface (base)");

    rcu_read_lock();
    for (i = 0; i < MACDB_HASH_SIZE; i++) {
        struct macdb_entry *m;

        hlist_for_each_entry_rcu(m, &macdb_hash[i], hlist) {
            seq_printf(seq, "%pM\t%s (%s)\n",
                m->addr, m->dev->name, m->real_dev->name);
        }
    }
    rcu_read_unlock();
}

int32_t
dc_dp_notify_init(void)
{
    int32_t ret;
    struct sockaddr_nl addr = {0};

    /* Create a macdb */
    macdb_cache = kmem_cache_create("dc_dp_macdb_cache",
                                     sizeof(struct macdb_entry),
                                     0,
                                     SLAB_HWCACHE_ALIGN, NULL);
    if (!macdb_cache) {
        ret = -ENOMEM;
        goto err_out;
    }

    get_random_bytes(&macdb_salt, sizeof(macdb_salt));

    /* Create a netlink socket */
    ret = sock_create_kern(&init_net, AF_NETLINK, SOCK_RAW, NETLINK_ROUTE, &g_nl_sock);
    if (ret) {
        pr_err("sock_create_kern() failed (ret=%d)!\n", ret);
        goto err_out_db;
    }

    addr.nl_family  = AF_NETLINK;
    addr.nl_pid     = 0;
    addr.nl_groups  = 1 << (RTNLGRP_NEIGH - 1);

    ret = kernel_bind(g_nl_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (ret) {
        pr_err("kernel_bind() failed (ret=%d)!\n", ret);
        goto err_out_sock;
    }

    /* Start a event receiver thread */
    evt_rx_thread = kthread_run(dc_dp_rx_event, g_nl_sock->sk, "evt_rx");
    if (IS_ERR(evt_rx_thread)) {
        pr_err("failed to start event rx thread!");
        ret = -1;
        goto err_out_sock;
    }

    return 0;

err_out_sock:
    sock_release(g_nl_sock);
    g_nl_sock = NULL;

err_out_db:
    kmem_cache_destroy(macdb_cache);
    macdb_cache = NULL;

err_out:
    return ret;
}

void
dc_dp_notify_exit(void)
{
    if (evt_rx_thread) {
        kthread_stop(evt_rx_thread);
        evt_rx_thread = NULL;
    }

    sock_release(g_nl_sock);
    g_nl_sock = NULL;

    macdb_flush();
    /* Wait for completion of call_rcu()'s (macdb_free_rcu) */
    rcu_barrier();
    kmem_cache_destroy(macdb_cache);
}
