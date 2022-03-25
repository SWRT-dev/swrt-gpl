#ifndef _CQM_DEV_H
#define _CQM_DEV_H
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/export.h>
#include <linux/err.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/clk.h>

#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/reset.h>
#include <lantiq.h>
#include <linux/platform_device.h>
#include <linux/mfd/syscon.h>
#include <lantiq_soc.h>
#include "cqm_common.h"

#define PRX300_DEV_NAME "prx300-cqm"
#define GRX500_DEV_NAME "grx500-cbm"
#define MAX_NUM_POOLS 16
#define MAX_CPU_DQ_PORT_ARGS		2
#define MAX_CPU_DQ_PORT_N_TYPE	8

struct dt_node_inst {
	char *dev_name;
	char *node_name;
	int instance_id;
};

struct cqm_data {
	int num_resources;
	int num_intrs;
	unsigned int *intrs;
	struct clk *cqm_clk[2];
	struct reset_control *rcu_reset;
	struct regmap *syscfg;
	bool force_xpcs;
	int num_pools;
	int num_pools_a1;
	unsigned int pool_ptrs[MAX_NUM_POOLS];
	unsigned int pool_size[MAX_NUM_POOLS];
	unsigned int pool_ptrs_a1[MAX_NUM_POOLS];
	unsigned int pool_size_a1[MAX_NUM_POOLS];
	u32  dq_port[MAX_CPU_DQ_PORT_N_TYPE];
	u32 num_dq_port;
	u32 gsw_mode;
	int gint_mode;
};
#endif
