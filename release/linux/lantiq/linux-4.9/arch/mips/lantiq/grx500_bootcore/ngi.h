#ifndef NGI_H
#define NGI_H
#include <linux/types.h>

#define LTQ_NGI_MAX_PR_REGIONS 8
#define LTQ_NGI_MAX_RT_PR_REGIONS 8
#define LTQ_NUM_TARGET_AGENTS 64
#define LTQ_NUM_INIT_AGENTS 39

#define LTQ_NUM_NGI_INSTANCES 9

/* Protection Region structure */
struct ngi_acl_pr {
	char *pr_name;
	u32 pr_id;
	void *base;
	u32 size;
	u32 level;	/* Evaluation level of PR (0-lowest, 3-highest) */
	u32 group_read_pr;
	u32 group_write_pr;
};

struct ngi_ta_core {
	u32 core_code;
	char *core_name;
	u32 num_pr;
	struct ngi_acl_pr acl_pr[LTQ_NGI_MAX_PR_REGIONS];
};

struct ngi_ia_core {
	u32 core_code;
	char *core_name;
};

struct ngi_rt_core {
	u32 inst_id;
	char *name;
	u32 num_pr;
	struct ngi_acl_pr rt_pr[LTQ_NGI_MAX_RT_PR_REGIONS];
};
#endif
