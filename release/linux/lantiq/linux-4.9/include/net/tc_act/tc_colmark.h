/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __NET_TC_COLMARK_H
#define __NET_TC_COLMARK_H

#include <net/act_api.h>
#include <linux/tc_act/tc_colmark.h>

struct tcf_colmark {
	struct tc_action	common;
	enum tc_drop_precedence	precedence; /* packet drop precedence marker */
	enum tc_meter_type	type;  /* color marker type: srTCM, trTCM */
	bool			blind;
	u32			flags;
};

#define to_colmark(a) ((struct tcf_colmark *)a)

static inline bool is_tcf_colmark(const struct tc_action *a)
{
#ifdef CONFIG_NET_CLS_ACT
	if (a->ops && a->ops->type == TCA_ACT_COLMARK)
		return true;
#endif
	return false;
}

static inline bool tcf_colmark_mode(const struct tc_action *a)
{
	return to_colmark(a)->blind;
}

static inline u8 tcf_colmark_precedence(const struct tc_action *a)
{
	return to_colmark(a)->precedence;
}

static inline u8 tcf_colmark_mtype(const struct tc_action *a)
{
	return to_colmark(a)->type;
}

static inline u8 tcf_colmark_flags(const struct tc_action *a)
{
	return to_colmark(a)->flags;
}

#endif /* __NET_TC_COLMARK_H */
