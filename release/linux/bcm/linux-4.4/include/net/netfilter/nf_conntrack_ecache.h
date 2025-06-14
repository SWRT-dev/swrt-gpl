/*
 * connection tracking event cache.
 */

#ifndef _NF_CONNTRACK_ECACHE_H
#define _NF_CONNTRACK_ECACHE_H
#include <net/netfilter/nf_conntrack.h>

#include <net/net_namespace.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <linux/netfilter/nf_conntrack_common.h>
#include <linux/netfilter/nf_conntrack_tuple_common.h>
#include <net/netfilter/nf_conntrack_extend.h>

struct nf_conntrack_ecache {
	unsigned long cache;	/* bitops want long */
	unsigned long missed;	/* missed events */
	u16 ctmask;		/* bitmask of ct events to be delivered */
	u16 expmask;		/* bitmask of expect events to be delivered */
	u32 portid;		/* netlink portid of destroyer */
};

static inline struct nf_conntrack_ecache *
nf_ct_ecache_find(const struct nf_conn *ct)
{
#ifdef CONFIG_NF_CONNTRACK_EVENTS
	return nf_ct_ext_find(ct, NF_CT_EXT_ECACHE);
#else
	return NULL;
#endif
}

static inline struct nf_conntrack_ecache *
nf_ct_ecache_ext_add(struct nf_conn *ct, u16 ctmask, u16 expmask, gfp_t gfp)
{
#ifdef CONFIG_NF_CONNTRACK_EVENTS
	struct net *net = nf_ct_net(ct);
	struct nf_conntrack_ecache *e;

	if (!ctmask && !expmask && net->ct.sysctl_events) {
		ctmask = ~0;
		expmask = ~0;
	}
	if (!ctmask && !expmask)
		return NULL;

	e = nf_ct_ext_add(ct, NF_CT_EXT_ECACHE, gfp);
	if (e) {
		e->ctmask  = ctmask;
		e->expmask = expmask;
	}
	return e;
#else
	return NULL;
#endif
};

#ifdef CONFIG_NF_CONNTRACK_EVENTS
/* This structure is passed to event handler */
struct nf_ct_event {
	struct nf_conn *ct;
	u32 portid;
	int report;
};

struct nf_ct_event_notifier {
	int (*fcn)(unsigned int events, struct nf_ct_event *item);
};

#if defined(CONFIG_SHORTCUT_FE) || defined(CONFIG_SHORTCUT_FE_MODULE)
extern int nf_conntrack_register_notifier(struct net *net, struct notifier_block *nb);
extern int nf_conntrack_unregister_notifier(struct net *net, struct notifier_block *nb);
#else
int nf_conntrack_register_notifier(struct net *net,
				   struct nf_ct_event_notifier *nb);
void nf_conntrack_unregister_notifier(struct net *net,
				      struct nf_ct_event_notifier *nb);
#endif

void nf_ct_deliver_cached_events(struct nf_conn *ct);

static inline void
nf_conntrack_event_cache(enum ip_conntrack_events event, struct nf_conn *ct)
{
	struct nf_conntrack_ecache *e;

	e = nf_ct_ecache_find(ct);
	if (e == NULL)
		return;

	set_bit(event, &e->cache);
}

static inline int
nf_conntrack_eventmask_report(unsigned int eventmask,
			      struct nf_conn *ct,
			      u32 portid,
			      int report)
{
	struct nf_conntrack_ecache *e;

	struct net *net = nf_ct_net(ct);

	e = nf_ct_ecache_find(ct);
	if (e == NULL)
		return 0;

	if (nf_ct_is_confirmed(ct) && !nf_ct_is_dying(ct)) {
		struct nf_ct_event item = {
			.ct 	= ct,
			.portid	= e->portid ? e->portid : portid,
			.report = report
		};
		/* This is a resent of a destroy event? If so, skip missed */
		unsigned long missed = e->portid ? 0 : e->missed;

		if (!((eventmask | missed) & e->ctmask))
			return 0;

		atomic_notifier_call_chain(&net->ct.nf_conntrack_chain, eventmask | missed, &item);
	}

	return 0;
}

static inline int
nf_conntrack_event_report(enum ip_conntrack_events event, struct nf_conn *ct,
			  u32 portid, int report)
{
	return nf_conntrack_eventmask_report(1 << event, ct, portid, report);
}

static inline int
nf_conntrack_event(enum ip_conntrack_events event, struct nf_conn *ct)
{
	return nf_conntrack_eventmask_report(1 << event, ct, 0, 0);
}

struct nf_exp_event {
	struct nf_conntrack_expect *exp;
	u32 portid;
	int report;
};

struct nf_exp_event_notifier {
	int (*fcn)(unsigned int events, struct nf_exp_event *item);
};

int nf_ct_expect_register_notifier(struct net *net,
				   struct nf_exp_event_notifier *nb);
void nf_ct_expect_unregister_notifier(struct net *net,
				      struct nf_exp_event_notifier *nb);

static inline void
nf_ct_expect_event_report(enum ip_conntrack_expect_events event,
			  struct nf_conntrack_expect *exp,
			  u32 portid,
			  int report)
{
	struct net *net = nf_ct_exp_net(exp);
	struct nf_exp_event_notifier *notify;
	struct nf_conntrack_ecache *e;

	rcu_read_lock();
	notify = rcu_dereference(net->ct.nf_expect_event_cb);
	if (notify == NULL)
		goto out_unlock;

	e = nf_ct_ecache_find(exp->master);
	if (e == NULL)
		goto out_unlock;

	if (e->expmask & (1 << event)) {
		struct nf_exp_event item = {
			.exp	= exp,
			.portid	= portid,
			.report = report
		};
		notify->fcn(1 << event, &item);
	}
out_unlock:
	rcu_read_unlock();
}

static inline void
nf_ct_expect_event(enum ip_conntrack_expect_events event,
		   struct nf_conntrack_expect *exp)
{
	nf_ct_expect_event_report(event, exp, 0, 0);
}

int nf_conntrack_ecache_pernet_init(struct net *net);
void nf_conntrack_ecache_pernet_fini(struct net *net);

int nf_conntrack_ecache_init(void);
void nf_conntrack_ecache_fini(void);

static inline void nf_conntrack_ecache_delayed_work(struct net *net)
{
	if (!delayed_work_pending(&net->ct.ecache_dwork)) {
		schedule_delayed_work(&net->ct.ecache_dwork, HZ);
		net->ct.ecache_dwork_pending = true;
	}
}

static inline void nf_conntrack_ecache_work(struct net *net)
{
	if (net->ct.ecache_dwork_pending) {
		net->ct.ecache_dwork_pending = false;
		mod_delayed_work(system_wq, &net->ct.ecache_dwork, 0);
	}
}
#else /* CONFIG_NF_CONNTRACK_EVENTS */
static inline void nf_conntrack_event_cache(enum ip_conntrack_events event,
					    struct nf_conn *ct) {}
static inline int nf_conntrack_eventmask_report(unsigned int eventmask,
						struct nf_conn *ct,
						u32 portid,
						int report) { return 0; }
static inline int nf_conntrack_event(enum ip_conntrack_events event,
				     struct nf_conn *ct) { return 0; }
static inline int nf_conntrack_event_report(enum ip_conntrack_events event,
					    struct nf_conn *ct,
					    u32 portid,
					    int report) { return 0; }
static inline void nf_ct_deliver_cached_events(const struct nf_conn *ct) {}
static inline void nf_ct_expect_event(enum ip_conntrack_expect_events event,
				      struct nf_conntrack_expect *exp) {}
static inline void nf_ct_expect_event_report(enum ip_conntrack_expect_events e,
					     struct nf_conntrack_expect *exp,
 					     u32 portid,
 					     int report) {}

static inline int nf_conntrack_ecache_pernet_init(struct net *net)
{
	return 0;
}

static inline void nf_conntrack_ecache_pernet_fini(struct net *net)
{
}

static inline int nf_conntrack_ecache_init(void)
{
	return 0;
}

static inline void nf_conntrack_ecache_fini(void)
{
}

static inline void nf_conntrack_ecache_delayed_work(struct net *net)
{
}

static inline void nf_conntrack_ecache_work(struct net *net)
{
}
#endif /* CONFIG_NF_CONNTRACK_EVENTS */

#endif /*_NF_CONNTRACK_ECACHE_H*/

