/*
 * infra.h
 * Description: PP infrastructure h file
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2019 Intel Corporation
 */
#ifndef __INFRA_H__
#define __INFRA_H__
#include <linux/types.h>
#include <linux/init.h>
#include <linux/pp_api.h>
#include <linux/debugfs.h> /* struct dentry */
/**
 * \brief Infra Init Config
 * @valid params valid
 * @bootcfg_base bootconfig base address
 * @clk_ctrl_base clock ctrl base address
 * @dbgfs debugfs parent directory
 */
struct infra_init_param {
	bool valid;
	u64 bootcfg_base;
	u64 clk_ctrl_base;
	struct dentry *dbgfs;
};

#ifdef CONFIG_PPV4_LGM
/**
 * @brief Get infra HW version
 * @return u32 infra hw version
 */
u32 infra_version_get(void);

/**
 * @brief Get PP HW silicon step
 * @return enum pp_silicon_step the silicon step
 */
enum pp_silicon_step pp_silicon_step_get(void);

/**
 * @brief Module init function
 * @param init_param initial parameters
 * @return s32 0 on success
 */
s32 infra_init(struct infra_init_param *init_param);

/**
 * @brief Init dynamic hw clock gating feature
 *        This is done in a separate API cause it MUST
 *        be done after the entire PP was initialized
 */
void infra_dynamic_clk_init(void);

/**
 * @brief Module exit function
 */
void infra_exit(void);
#else
#include "pp_common.h"
#include "lantiq.h"

static inline s32 infra_init(struct infra_init_param *cfg)
{
	if (cfg->valid)
		return -EINVAL;
	else
		return 0;
}

static inline void infra_exit(void)
{
}

static inline void infra_dynamic_clk_init(void)
{
}

static inline enum pp_silicon_step pp_silicon_step_get(void)
{
	return (ltq_get_soc_rev() ==  0) ? PP_SSTEP_A : PP_SSTEP_B;
}

#endif /* CONFIG_PPV4_LGM */
#endif /* __INFRA_H__ */
