/* Kernel module to match the port-ranges, trigger related port-ranges,
 * and alters the destination to a local IP address.
 *
 * Copyright (C) 2003, CyberTAN Corporation
 * All Rights Reserved.
 *
 * Description:
 *   This is kernel module for port-triggering.
 *
 *   The module follows the Netfilter framework, called extended packet
 *   matching modules.
 */

#include <linux/types.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/if.h>
#include <linux/inetdevice.h>
#include <net/protocol.h>
#include <net/checksum.h>
#include <linux/proc_fs.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_nat.h>
#include <linux/netfilter_ipv4/ip_autofw.h>
#include <linux/netfilter_ipv4/lockhelp.h>
#ifdef CONFIG_NF_NAT_NEEDED
#include <net/netfilter/nf_nat.h>
#else
#include <linux/netfilter_ipv4/ip_nat_rule.h>
#endif
#include <linux/netfilter_ipv4/ipt_TRIGGER.h>

MODULE_LICENSE("GPL");

/*
This rwlock protects the main hash table, protocol/helper/expected registrations, conntrack timers
*/

#define ASSERT_READ_LOCK(x) MUST_BE_READ_LOCKED(&nf_conntrack_expect_lock)
#define ASSERT_WRITE_LOCK(x) MUST_BE_WRITE_LOCKED(&nf_conntrack_expect_lock)

#include <linux/netfilter_ipv4/listhelp.h>

#if 0
#define DEBUGP printk
#else
#define DEBUGP(format, args...)
#endif

static u_int32_t porttrigger_timeout = 120;
struct ipt_trigger
{
    struct list_head list;          /* Trigger list */
    struct timer_list timeout;      /* Timer for list destroying */
    u_int32_t srcip;                /* Outgoing source address */
    u_int32_t dstip;                /* Outgoing destination address */
    u_int16_t mproto;               /* Trigger protocol */
    u_int16_t rproto;               /* Related protocol */
    struct ipt_trigger_ports ports; /* Trigger and related ports */
    u_int8_t reply;                 /* Confirm a reply connection */
};

LIST_HEAD(trigger_list);
//DECLARE_LOCK(ip_trigger_lock);

static void trigger_refresh(struct ipt_trigger *trig, unsigned long extra_jiffies)
{
    DEBUGP("%s: \n", __FUNCTION__);
    NF_CT_ASSERT(trig);
    spin_lock_bh(&nf_conntrack_expect_lock);

    /* Need del_timer for race avoidance (may already be dying). */
    if (del_timer(&trig->timeout))
    {
        trig->timeout.expires = jiffies + extra_jiffies;
        add_timer(&trig->timeout);
    }

    spin_unlock_bh(&nf_conntrack_expect_lock);
}

static void __del_trigger(struct ipt_trigger *trig)
{
    DEBUGP("%s: \n", __FUNCTION__);
    NF_CT_ASSERT(trig);
    //MUST_BE_WRITE_LOCKED(&nf_conntrack_expect_lock);

    /* delete from 'trigger_list' */
    list_del(&trig->list);
    kfree(trig);
}

static void trigger_timeout(struct timer_list *t)
{
    struct ipt_trigger *trig = from_timer(trig, t, timeout);

    DEBUGP("trigger list %p timed out\n", trig);
    spin_lock_bh(&nf_conntrack_expect_lock);
    __del_trigger(trig);
    spin_unlock_bh(&nf_conntrack_expect_lock);
}

static unsigned int add_new_trigger(struct ipt_trigger *trig)
{
    struct ipt_trigger *new;

    DEBUGP("!!!!!!!!!!!! %s !!!!!!!!!!!\n", __FUNCTION__);
    spin_lock_bh(&nf_conntrack_expect_lock);
    new = (struct ipt_trigger *)kmalloc(sizeof(struct ipt_trigger), GFP_ATOMIC);

    if (!new)
    {
        spin_unlock_bh(&nf_conntrack_expect_lock);
        DEBUGP("%s: OOM allocating trigger list\n", __FUNCTION__);
        return -ENOMEM;
    }

    memset(new, 0, sizeof(*trig));
    INIT_LIST_HEAD(&new->list);
    memcpy(new, trig, sizeof(*trig));

    /* add to global table of trigger */
    list_prepend(&trigger_list, &new->list);

    new->timeout.expires = jiffies + (porttrigger_timeout * HZ);
    timer_setup(&new->timeout, trigger_timeout, 0);
    add_timer(&new->timeout);

    spin_unlock_bh(&nf_conntrack_expect_lock);

    return 0;
}

static inline int trigger_out_matched(const struct ipt_trigger *i, const u_int16_t proto, const u_int16_t dport)
{
    /*
    DEBUGP("%s: i=%p, proto= %d, dport=%d.\n", __FUNCTION__, i, proto, dport);
    DEBUGP("%s: Got one, mproto= %d, mport[0..1]=%d, %d.\n", __FUNCTION__, i->mproto, i->ports.mport[0], i->ports.mport[1]);
    */

    return ((i->mproto == proto) && (i->ports.mport[0] <= dport) && (i->ports.mport[1] >= dport));
}

static unsigned int trigger_out(struct sk_buff **pskb, unsigned int hooknum, const struct net_device *in, const struct net_device *out, const void *targinfo)
{
    const struct ipt_trigger_info *info = targinfo;
    struct ipt_trigger trig, *found;
    const struct iphdr *iph = ip_hdr(*pskb);
    struct tcphdr *tcph = (void *)iph + iph->ihl * 4;   /* Might be TCP, UDP */

    DEBUGP("############# %s ############\n", __FUNCTION__);
    /* Check if the trigger range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_out_matched, struct ipt_trigger *, iph->protocol, ntohs(tcph->dest));

    if (found)
    {
        /* Yeah, it exists. We need to update(delay) the destroying timer. */
#if 0
        trigger_refresh(found, TRIGGER_TIMEOUT * HZ);
#else
        trigger_refresh(found, porttrigger_timeout * HZ);
#endif
        /* In order to allow multiple hosts use the same port range, we update
           the 'saddr' after previous trigger has a reply connection. */
        if (found->reply)
        {
            found->srcip = iph->saddr;
        }
    }
    else
    {
        /* Create new trigger */
        memset(&trig, 0, sizeof(trig));
        trig.srcip = iph->saddr;
        trig.mproto = iph->protocol;
        trig.rproto = info->proto;
        memcpy(&trig.ports, &info->ports, sizeof(struct ipt_trigger_ports));
        /* Add the new 'trig' to list 'trigger_list'. */
        add_new_trigger(&trig);
    }

    return XT_CONTINUE; /* We don't block any packet. */
}

static inline int trigger_in_matched(const struct ipt_trigger *i, const u_int16_t proto, const u_int16_t dport)
{
    /* 
    DEBUGP("%s: i=%p, proto= %d, dport=%d.\n", __FUNCTION__, i, proto, dport);
    DEBUGP("%s: Got one, rproto= %d, rport[0..1]=%d, %d.\n", __FUNCTION__, i->rproto, i->ports.rport[0], i->ports.rport[1]);
    */

    u_int16_t rproto = i->rproto;

    if (!rproto)
    {
        rproto = proto;
    }

    return ((rproto == proto) && (i->ports.rport[0] <= dport) && (i->ports.rport[1] >= dport));
}

static unsigned int trigger_in(struct sk_buff **pskb, unsigned int hooknum, const struct net_device *in, const struct net_device *out, const void *targinfo)
{
    struct ipt_trigger *found;
    const struct iphdr *iph = ip_hdr(*pskb);
    struct tcphdr *tcph = (void *)iph + iph->ihl * 4;   /* Might be TCP, UDP */
    /* Check if the trigger-ed range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_in_matched, struct ipt_trigger *, iph->protocol, ntohs(tcph->dest));

    if (found)
    {
        DEBUGP("############# %s ############\n", __FUNCTION__);
        /* Yeah, it exists. We need to update(delay) the destroying timer. */
#if 0
        trigger_refresh(found, TRIGGER_TIMEOUT * HZ);
#else
        trigger_refresh(found, porttrigger_timeout * HZ);
#endif
        DEBUGP(KERN_LEVL KERN_INFO "TRIGGER_IN: refresh trigger \n ");

        /* Accept it, or the imcoming packet could be dropped in the FORWARD chain */
        return NF_ACCEPT;
    }
    /* DROP it. */
    return NF_DROP;
}

static void xt_nat_convert_range(struct nf_nat_range2 *dst, const struct nf_nat_ipv4_range *src)
{
    memset(&dst->min_addr, 0, sizeof(dst->min_addr));
    memset(&dst->max_addr, 0, sizeof(dst->max_addr));
    memset(&dst->base_proto, 0, sizeof(dst->base_proto));

    dst->flags       = src->flags;
    dst->min_addr.ip = src->min_ip;
    dst->max_addr.ip = src->max_ip;
    dst->min_proto   = src->min;
    dst->max_proto   = src->max;
}

static unsigned int trigger_dnat(struct sk_buff **pskb, unsigned int hooknum, const struct net_device *in, const struct net_device *out, const void *targinfo)
{
    struct ipt_trigger *found;
    const struct iphdr *iph = ip_hdr(*pskb);
    struct tcphdr *tcph = (void *)iph + iph->ihl * 4;    /* Might be TCP, UDP */
    struct nf_conn *ct;
    enum ip_conntrack_info ctinfo;
    struct nf_nat_ipv4_multi_range_compat newrange;
    struct nf_nat_range2 range;

    NF_CT_ASSERT(hooknum == NF_INET_PRE_ROUTING);
    /* Check if the trigger-ed range has already existed in 'trigger_list'. */
    found = LIST_FIND(&trigger_list, trigger_in_matched, struct ipt_trigger *, iph->protocol, ntohs(tcph->dest));

    if (!found || !found->srcip)
    {
        /* DROP it. */
        return NF_DROP;
    }

    DEBUGP("############# %s ############\n", __FUNCTION__);

    found->reply = 1;   /* Confirm there has been a reply connection. */
    ct = nf_ct_get(*pskb, &ctinfo);
    NF_CT_ASSERT(ct && (ctinfo == IP_CT_NEW));

    DEBUGP("%s: got ", __FUNCTION__);

    nf_ct_dump_tuple(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);
    /* Alter the destination of imcoming packet. */
    newrange = ((struct nf_nat_ipv4_multi_range_compat)
        {
            1,
            {
                {
                    NF_NAT_RANGE_MAP_IPS, found->srcip, found->srcip, { 0 }, { 0 }
                }
            }
        });

    xt_nat_convert_range(&range, &newrange.range[0]);

    /* Hand modified range to generic setup. */
    return nf_nat_setup_info(ct, &range, HOOK2MANIP(hooknum));
}

static unsigned int
trigger_target(struct sk_buff *skb, const struct xt_action_param *par)
{
    const struct ipt_trigger_info *info = par->targinfo;
    const struct iphdr *iph = ip_hdr(skb);

    /*
    DEBUGP("%s: type = %s\n", __FUNCTION__,
            (info->type == IPT_TRIGGER_DNAT) ? "dnat" :
            (info->type == IPT_TRIGGER_IN) ? "in" : "out");
    */

    /* The Port-trigger only supports TCP and UDP. */
    if ((iph->protocol != IPPROTO_TCP) && (iph->protocol != IPPROTO_UDP))
    {
        return XT_CONTINUE;
    }

    if (info->type == IPT_TRIGGER_OUT)
    {
        return trigger_out(&skb, par->state->hook, par->state->in, par->state->out, par->targinfo);
    }
    else if (info->type == IPT_TRIGGER_IN)
    {
        return trigger_in(&skb, par->state->hook, par->state->in, par->state->out, par->targinfo);
    }
    else if (info->type == IPT_TRIGGER_DNAT)
    {
        return trigger_dnat(&skb, par->state->hook, par->state->in, par->state->out, par->targinfo);
    }

    return XT_CONTINUE;
}

static int trigger_check(const struct xt_tgchk_param *par)
{
    //const struct ipt_entry *e = (struct ipt_entry*)par->entryinfo;
    const struct ipt_trigger_info *info = par->targinfo;
    struct list_head *cur_item, *tmp_item;

    if ((strcmp(par->table, "mangle") == 0))
    {
        DEBUGP("trigger_check: bad table `%s'.\n", par->table);
        return -EINVAL;
    }

    if (par->hook_mask & ~((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD)))
    {
        DEBUGP("trigger_check: bad hooks %x.\n", par->hook_mask);
        return -EINVAL;
    }

    if (info->proto)
    {
        if (info->proto != IPPROTO_TCP && info->proto != IPPROTO_UDP)
        {
            DEBUGP("trigger_check: bad proto %d.\n", info->proto);
            return -EINVAL;
        }
    }

    if (info->type == IPT_TRIGGER_OUT)
    {
        if (!info->ports.mport[0] || !info->ports.rport[0])
        {
            DEBUGP("trigger_check: Try 'iptbles -j TRIGGER -h' for help.\n");
            return -EINVAL;
        }
    }

    /* Empty the 'trigger_list' */
    list_for_each_safe(cur_item, tmp_item, &trigger_list)
    {
        struct ipt_trigger *trig = (void *)cur_item;

        DEBUGP("%s: list_for_each_safe(): %p.\n", __FUNCTION__, trig);
        del_timer(&trig->timeout);
        __del_trigger(trig);
    }

    return 0;
}

static struct xt_target redirect_reg = {
    .name           = "TRIGGER",
    .family         = NFPROTO_IPV4,
    .target         = trigger_target,
    .targetsize     = sizeof(struct ipt_trigger_info),
    .hooks          = (1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD),
    .checkentry     = trigger_check,
    .me             = THIS_MODULE,
};

#if defined(CONFIG_PROC_FS)
static int get_triiger_timeout_info(struct seq_file *file, void *v)
{
	char* buf;

	buf = kasprintf(GFP_KERNEL, "%d", porttrigger_timeout);
	
	if (!buf)
    {
		return -ENOMEM;
    }
		
	seq_write(file, buf, sizeof(buf));
	kfree(buf);

    return 0;
}

static int trigger_timeout_open(struct inode *inode, struct file *file)
{
	return single_open(file, get_triiger_timeout_info, NULL);
}

static ssize_t trigger_timeout_read(struct file *file, char __user *userbuf, size_t count, loff_t *ppos)
{
	char* buf;
	int ret;

	buf = kasprintf(GFP_KERNEL, "%d", porttrigger_timeout);
	
	if (!buf)
    {
		return -ENOMEM;
    }

	ret = simple_read_from_buffer(userbuf, count, ppos, buf, strlen(buf));
	kfree(buf);

	return ret;
}

static ssize_t trigger_timeout_write(struct file *file, const char __user *user, size_t count, loff_t *data)
{
    unsigned char local_buf[32];
    int ret_val = 0;
    if (count > 32)
    {
        printk(KERN_ERR "\n%s: Buffer Overflow\n", __FUNCTION__);
        return -EFAULT;
    }

    if (copy_from_user(local_buf,user,count))
    {
        return -EFAULT;;
    }
    local_buf[count - 1] = '\0';
    ret_val = count;
    sscanf(local_buf, "%d", &porttrigger_timeout);

    return ret_val;
}

static const struct file_operations proc_trigger_operations = {
    .open           = trigger_timeout_open,
    .read           = trigger_timeout_read,
    .write          = trigger_timeout_write,
    .llseek         = seq_lseek,
    .release        = single_release,
};

static struct proc_dir_entry *trigger_timeout_proc = NULL;

static int create_porttrigger_proc(void)
{
    trigger_timeout_proc = proc_create("porttrigger_timeout", 0644, NULL, &proc_trigger_operations);

    if (trigger_timeout_proc)
    {
        //trigger_timeout_proc->read_proc = trigger_timeout_read;
        //trigger_timeout_proc->write_proc = trigger_timeout_write;
    }
    else
    {
        printk("\n%s: create proc file for trigger timeout failed! \n", __FUNCTION__);
        return 1;
    }

    return 0;
}

static int destroy_porttrigger_proc(void)
{
    remove_proc_entry("porttrigger_timeout", NULL);
    return 0;
}
#endif /* CONFIG_PROC_FS */

static int __init init(void)
{
#if defined(CONFIG_PROC_FS)
    if (create_porttrigger_proc())
    {
        return 1;
    }
#endif /* CONFIG_PROC_FS */

    int ret = xt_register_target(&redirect_reg);

    return ret;
}

static void __exit fini(void)
{
#if defined(CONFIG_PROC_FS)
    destroy_porttrigger_proc();
#endif /* CONFIG_PROC_FS */

    xt_unregister_target(&redirect_reg);
}

module_init(init);
module_exit(fini);