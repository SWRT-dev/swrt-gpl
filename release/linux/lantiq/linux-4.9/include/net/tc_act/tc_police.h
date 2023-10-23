/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NET_TC_POLICE_H
#define __NET_TC_POLICE_H

#include <net/act_api.h>

struct tcf_police {
	struct tc_action	common;
	int			tcfp_result;
	u32			tcfp_ewma_rate;
	s64			tcfp_burst;
	u32			tcfp_mtu;
	s64			tcfp_toks;
	s64			tcfp_ptoks;
	s64			tcfp_mtu_ptoks;
	s64			tcfp_t_c;
	struct psched_ratecfg	rate;
	bool			rate_present;
	struct psched_ratecfg	peak;
	bool			peak_present;
};

#define to_police(pc) ((struct tcf_police *)pc)

#define POL_TAB_MASK     15

/* old policer structure from before tc actions */
struct tc_police_compat {
	u32			index;
	int			action;
	u32			limit;
	u32			burst;
	u32			mtu;
	struct tc_ratespec	rate;
	struct tc_ratespec	peakrate;
};

static inline bool is_tcf_police(const struct tc_action *act)
{
#ifdef CONFIG_NET_CLS_ACT
	if (act->ops && act->ops->type == TCA_ID_POLICE)
		return true;
#endif
	return false;
}

static inline u64 tcf_police_rate_bytes_ps(const struct tc_action *act)
{
	return to_police(act)->rate.rate_bytes_ps;
}

static inline s64 tcf_police_tcfp_burst(const struct tc_action *act)
{
	return to_police(act)->tcfp_burst;
}

static inline u64 tcf_police_peak_bytes_ps(const struct tc_action *act)
{
	return to_police(act)->peak.rate_bytes_ps;
}

static inline u32 tcf_police_tcfp_mtu(const struct tc_action *act)
{
	return to_police(act)->tcfp_mtu;
}

#endif /* __NET_TC_POLICE_H */
