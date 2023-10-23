/*
 * pp_dev.c
 * Description: PP Device Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/of_device.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/uaccess.h>
#include <linux/pp_qos_api.h>
#include "pp_dev.h"
#include "pp_common.h"
#include "pp_port_mgr.h"
#include "pp_qos_fw.h"
#include "pput.h"
#include "pp_si.h"
#include "pp_desc.h"

#define DRV_NAME "PPv4_Platform_Driver"

#define CONST2STR(c) #c
#define DRV_VERSION \
	CONST2STR(PP_VER_MAJOR)"."	\
	CONST2STR(PP_VER_MAJOR_MID)"."	\
	CONST2STR(PP_VER_MID)"."	\
	CONST2STR(PP_VER_MINOR_MID)"."	\
	CONST2STR(PP_VER_MINOR)

static int __pp_dev_probe(struct platform_device *pdev);
static int __pp_dev_remove(struct platform_device *pdev);

/* PP Debug FS directory */
static struct dentry *pp_dbgfs_dir;
static struct dentry *pp_hal_dbgfs_dir;
/* PP sysfs directory */
struct kobject *pp_sysfs_dir;

/* PP clocks definitions based on SoC type */
#ifdef CONFIG_PPV4_LGM
	const char *pp_clocks[] = { "ppv4" };
#else
	const char *pp_clocks[] = { "ppv4", "freq" };
#endif

static const struct of_device_id pp_dev_match[] = {
	{ .compatible = "intel,ppv4" },
	{},
};
MODULE_DEVICE_TABLE(of, pp_dev_match);

/* Platform driver */
static struct platform_driver pp_plat_drv = {
	.probe  = __pp_dev_probe,
	.remove = __pp_dev_remove,
	.driver = {
		.name  = DRV_NAME,
		.owner = THIS_MODULE,
#if IS_ENABLED(CONFIG_OF)
		.of_match_table = pp_dev_match,
#endif
	},
};

/* Platform device */
struct platform_device *pp_pdev;
struct pp_dts_cfg *dts_cfg;
s32    probe_ret;
static bool ready; /* specify if PP driver is ready */
static bool init_done; /* specify if PP dev init is done */

/**
 * @brief Add function name and line number for all pr_* prints
 */
#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[PP_DEV]:%s:%d: " fmt, __func__, __LINE__
#endif

bool __pp_is_ready(const char *caller)
{
	if (unlikely(!ready))
		pr_err("%s: PP driver isn't ready!\n", caller);

	return ready;
}

/**
 * @brief get the PP device
 * @return struct device* pointer to device
 */
struct device *pp_dev_get(void)
{
	return pp_pdev ? &pp_pdev->dev : NULL;
}

/**
 * @brief Init pp dev driver configuration from device tree
 * @param pdev module's platform device
 */
static s32 __pp_dts_cfg_init(struct platform_device *pdev)
{
	s32 ret = 0;

	dts_cfg = devm_kzalloc(&pdev->dev, sizeof(*dts_cfg), GFP_KERNEL);
	if (unlikely(!dts_cfg)) {
		pr_err("Failed to allocate dts_cfg memory of size %u\n",
		       (u32)sizeof(*dts_cfg));
		return -ENOMEM;
	}
	dev_set_drvdata(&pdev->dev, dts_cfg);

	if (IS_ENABLED(CONFIG_OF)) {
		ret = pp_dts_cfg_get(pdev, dts_cfg);
		if (unlikely(ret)) {
			pr_err("Failed to fetch dts config, ret %d\n", ret);
			return ret;
		}
		pp_dts_cfg_dump(dts_cfg);
	}

	return ret;
}

/**
 * @brief Set the parent debug directory for all modules.
 * @return s32
 */
static void __pp_dbg_init(void)
{
	/* main pp sysfs folder */
	dts_cfg->pmgr_params.sysfs = pp_sysfs_dir;
	dts_cfg->chk_param.sysfs   = pp_sysfs_dir;
	dts_cfg->misc_params.sysfs = pp_sysfs_dir;
	dts_cfg->qos_params.sysfs  = pp_sysfs_dir;

	if (IS_ENABLED(CONFIG_DEBUG_FS)) {
		/* main pp debugfs folder */
		dts_cfg->smgr_params.dbgfs      = pp_dbgfs_dir;
		dts_cfg->bmgr_params.dbgfs      = pp_dbgfs_dir;
		dts_cfg->pmgr_params.dbgfs      = pp_dbgfs_dir;
		dts_cfg->misc_params.root_dbgfs = pp_dbgfs_dir;
		dts_cfg->qos_params.dbgfs       = pp_dbgfs_dir;
		dts_cfg->chk_param.root_dbgfs   = pp_dbgfs_dir;

		/* pp hal debugfs folder */
		dts_cfg->misc_params.hal_dbgfs  = pp_hal_dbgfs_dir;
		dts_cfg->infra_params.dbgfs     = pp_hal_dbgfs_dir;
		dts_cfg->parser_params.dbgfs    = pp_hal_dbgfs_dir;
		dts_cfg->rpb_params.dbgfs       = pp_hal_dbgfs_dir;
		dts_cfg->port_dist_params.dbgfs = pp_hal_dbgfs_dir;
		dts_cfg->cls_param.dbgfs        = pp_hal_dbgfs_dir;
		dts_cfg->chk_param.dbgfs        = pp_hal_dbgfs_dir;
		dts_cfg->mod_param.dbgfs        = pp_hal_dbgfs_dir;
		dts_cfg->rx_dma_params.dbgfs    = pp_hal_dbgfs_dir;
		dts_cfg->uc_params.dbgfs        = pp_hal_dbgfs_dir;
	}
}

static int init_clocks(struct platform_device *pdev)
{
	const char *clock_name;
	struct clk *clk;
	u32 i;

	for (i = 0; i < ARRAY_SIZE(pp_clocks); i++) {
		clock_name = pp_clocks[i];
		clk = devm_clk_get(&pdev->dev, clock_name);
		if (IS_ERR(clk)) {
			pr_err("failed to get clock '%s', error %ld\n",
			       clock_name, PTR_ERR(clk));
			return -ENODEV;
		}

		pr_info("enabling pp clock '%s'\n", clock_name);
		clk_prepare_enable(clk);
	}

	return 0;
}

static void disable_clocks(struct platform_device *pdev)
{
	const char *clock_name;
	struct clk *clk;
	u32 i;

	for (i = 0; i < ARRAY_SIZE(pp_clocks); i++) {
		clock_name = pp_clocks[i];
		clk = devm_clk_get(&pdev->dev, clock_name);
		if (IS_ERR(clk)) {
			pr_err("failed to get clock '%s', error %ld\n",
			       clock_name, PTR_ERR(clk));
			continue;
		}

		pr_info("disabling pp clock '%s'\n", clock_name);
		clk_disable_unprepare(clk);
	}
}

/**
 * @brief Exit pp hal modules
 */
static void __pp_hal_exit(struct platform_device *pdev)
{
	port_dist_exit();
	rpb_exit();
	prsr_exit();
	cls_exit();
	chk_exit();
	mod_exit();
	uc_exit();
	rx_dma_exit();
	bm_exit();
	infra_exit();
	disable_clocks(pdev);
}

#if IS_ENABLED(PP_POWER_ON_REGS_TEST)
#include "pp_dev_test.h"
extern s32 uc_dbg_clk_set(struct uc_init_params *init_param, bool en);
extern s32 infra_clck_ctrl_set(struct infra_init_param *init_param, bool en);

static s32 __reg_access_test(struct test_reg *reg, bool r_only)
{
	u32 val;
	void *virt;

	virt = ioremap(reg->addr, 4);
	if (unlikely(!virt)) {
		pr_err("Failed to map reg %#llx\n", reg->addr);
		return -ENOMEM;
	}

	if (!r_only)
		PP_REG_WR32(virt, reg->val);

	val = PP_REG_RD32(virt);
	/* unmap the memory */
	iounmap(virt);

	if (reg->val == val)
		return 0;

	pr_err("PP reg %#llx value is %#x, expected is %#x\n",
	       reg->addr, val, reg->val);
	return -EIO;
}

/**
 * @brief Test all PP register and memories
 */
static void __pp_initial_hw_test(void)
{
	struct test_reg *reg;
	s32 test_ret = 0;

	/* enable necessary clocks */
	if (unlikely(infra_clck_ctrl_set(&dts_cfg->infra_params, true))) {
		pr_err("Failed to enable infra clocks\n");
		return;
	}

	if (unlikely(uc_dbg_clk_set(&dts_cfg->uc_params, true))) {
		pr_err("Failed to enable uc clocks\n");
		goto disable_infra;
	}

	/* Test the registers default values */
	for_each_arr_entry(reg, reg_dflt, ARRAY_SIZE(reg_dflt))
		test_ret |= __reg_access_test(reg, true);

	/* Test the registers random values */
	for_each_arr_entry(reg, reg_rand, ARRAY_SIZE(reg_rand))
		test_ret |= __reg_access_test(reg, false);

	/* Test the registers all '1' values */
	for_each_arr_entry(reg, reg_mask, ARRAY_SIZE(reg_mask))
		test_ret |= __reg_access_test(reg, false);

	/* Test the memories all '0' values */
	for_each_arr_entry(reg, mem_zero, ARRAY_SIZE(mem_zero))
		test_ret |= __reg_access_test(reg, false);

	/* Test the memories all '1' values */
	for_each_arr_entry(reg, mem_mask, ARRAY_SIZE(mem_mask))
		test_ret |= __reg_access_test(reg, false);

	/* Test the memories random values */
	for_each_arr_entry(reg, mem_rand, ARRAY_SIZE(mem_rand))
		test_ret |= __reg_access_test(reg, false);

	/* Test the memories random (reversed) values */
	for_each_arr_entry(reg, mem_rand_rev, ARRAY_SIZE(mem_rand_rev))
		test_ret |= __reg_access_test(reg, false);

	pr_err("+======================================+\n");
	pr_err("| PP HW registers test %-4s            |\n",
	       test_ret ? "fail" : "pass");
	pr_err("+======================================+\n");

	/* disable back the clocks */
	if (unlikely(uc_dbg_clk_set(&dts_cfg->uc_params, false)))
		pr_err("Failed to disable infra and uc clocks\n");
disable_infra:
	if (unlikely(infra_clck_ctrl_set(&dts_cfg->infra_params, false)))
		pr_err("Failed to disable infra and uc clocks\n");
}
#endif

/**
 * @brief Init pp hal modules
 * @return s32 0 on success
 */
static s32 __pp_hal_init(struct platform_device *pdev)
{
	s32 ret = 0;

#if IS_ENABLED(PP_POWER_ON_REGS_TEST)
	__pp_initial_hw_test();
#endif

	ret = init_clocks(pdev);
	if (unlikely(ret)) {
		pr_err("Failed to init the clocks\n");
		goto out;
	}

	ret = pp_regs_init();
	if (unlikely(ret)) {
		pr_err("Failed to init infra module\n");
		goto out;
	}

	ret = infra_init(&dts_cfg->infra_params);
	if (unlikely(ret)) {
		pr_err("Failed to init infra module\n");
		goto out;
	}

	/* init bm */
	ret = bm_init(&dts_cfg->bm_params);
	if (unlikely(ret)) {
		pr_err("Failed to init bm module\n");
		goto out;
	}
	/* init rxdma */
	ret = rx_dma_init(&dts_cfg->rx_dma_params);
	if (unlikely(ret)) {
		pr_err("Failed to init rxdma module\n");
		goto out;
	}
	/* init modifier */
	ret = mod_init(&dts_cfg->mod_param);
	if (unlikely(ret)) {
		pr_err("Failed to init modifier module\n");
		goto out;
	}
	/* init checker */
	ret = chk_init(&dts_cfg->chk_param);
	if (unlikely(ret)) {
		pr_err("Failed to init checker module\n");
		goto out;
	}
	/* init classifier */
	ret = cls_init(&dts_cfg->cls_param);
	if (unlikely(ret)) {
		pr_err("Failed to init classifier module\n");
		goto out;
	}
	/* init parser */
	ret = prsr_init(&dts_cfg->parser_params);
	if (unlikely(ret)) {
		pr_err("Failed to init parser module\n");
		goto out;
	}
	/* init rpb */
	ret = rpb_init(&dts_cfg->rpb_params);
	if (unlikely(ret)) {
		pr_err("Failed to init rpb module\n");
		goto out;
	}
	/* init port dist */
	ret = port_dist_init(&dts_cfg->port_dist_params);
	if (unlikely(ret)) {
		pr_err("Failed to init port dist module\n");
		goto out;
	}

	/* init HW dynamic clock gating */
	if (dts_cfg->infra_params.valid)
		infra_dynamic_clk_init();

out:
	return ret;
}

/**
 * @brief Exit pp api modules
 */
static void __pp_api_exit(void)
{
	/* TODO: pp_session_mgr_exit();*/
	pp_misc_exit();
	pp_smgr_exit();
	pmgr_exit();
	pp_bmgr_exit();
	pp_qos_mgr_exit();
}

/**
 * @brief Init pp api modules
 * @return s32 0 on success
 */
static s32 __pp_api_init(void)
{
	s32 ret = 0;
	struct pp_qos_dev *qdev;

	/* init qos */
	if (dts_cfg->qos_params.valid) {
		pp_qos_mgr_init();
		qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
		if (unlikely(ptr_is_null(qdev)))
			return -EINVAL;

		ret = pp_qos_dev_init(qdev, &dts_cfg->qos_params);
		if (unlikely(ret)) {
			pr_err("pp_qos_dev_init failed\n");
			goto out;
		}
	}

	/* init bm */
	ret = pp_bmgr_init(&dts_cfg->bmgr_params);
	if (unlikely(ret)) {
		pr_err("Failed to init buffer mgr module\n");
		goto out;
	}

	/* init port mgr */
	if (dts_cfg->port_dist_params.valid) {
		ret = pmgr_init(&dts_cfg->pmgr_params);
		if (unlikely(ret)) {
			pr_err("Failed to init port mgr module\n");
			goto out;
		}
	}

	/* init session mgr */
	ret = pp_smgr_init(&dts_cfg->smgr_params);
	if (unlikely(ret)) {
		pr_err("Failed to init session mgr module\n");
		goto out;
	}

	/* init misc */
	ret = pp_misc_init(&dts_cfg->misc_params);
	if (unlikely(ret)) {
		pr_err("Failed to init misc module\n");
		goto out;
	}

out:
	return ret;
}

/**
 * @brief Platform driver probe function, do as follow:<br>
 *         1. Initialized driver database <br>
 *         2. Initialized PP drivers <br>
 *         3. Create dbgfs stuff <br>
 * @param pdev platform device
 * @return int 0 on success, non-zero value otherwise
 */
static int __pp_dev_probe(struct platform_device *pdev)
{
	pr_debug("start (pdev: %s)\n", pdev->name);
	pp_pdev = pdev;

	/* Create PP sysfs directory */
	pp_sysfs_dir = kobject_create_and_add("pp", kernel_kobj);
	if (unlikely(!pp_sysfs_dir)) {
		pr_err("Failed to create PP sysfs dir\n");
		probe_ret = -ENOMEM;
		return probe_ret;
	}
	pr_debug("pp sysfs directory created\n");
	/* Create PP debugfs directory */
	if (IS_ENABLED(CONFIG_DEBUG_FS)) {
		/* debug - init pp base debugfs dir */
		pp_dbgfs_dir = debugfs_create_dir("pp", NULL);
		if (unlikely(!pp_dbgfs_dir)) {
			pr_err("Failed to create PP debugfs dir\n");
			probe_ret = -ENOMEM;
			return probe_ret;
		}
		pr_debug("pp debugfs directory created\n");

		pp_hal_dbgfs_dir = debugfs_create_dir("hal", pp_dbgfs_dir);
		if (unlikely(!pp_hal_dbgfs_dir)) {
			pr_err("Failed to create PP hal debugfs dir\n");
			probe_ret = -ENOMEM;
			return probe_ret;
		}
		pr_debug("pp/hal debugfs directory created\n");

	}

	probe_ret = __pp_dts_cfg_init(pdev);
	if (unlikely(probe_ret)) {
		pr_err("Failed to fetch dts config, ret %d\n", probe_ret);
		return probe_ret;
	}

	__pp_dbg_init();

	probe_ret = __pp_hal_init(pdev);
	if (unlikely(probe_ret)) {
		pr_err("Failed to init pp hal drivers\n");
		return probe_ret;
	}

	probe_ret = __pp_api_init();
	if (unlikely(probe_ret)) {
		pr_err("Failed to init pp api drivers\n");
		return probe_ret;
	}

#ifdef CONFIG_PPV4_LGM
	if (pp_silicon_step_get() == PP_SSTEP_B) {
		probe_ret = dma_set_mask_and_coherent(&pp_pdev->dev,
						      DMA_BIT_MASK(64));
		if (probe_ret) {
			pr_err("Set dma bitmask to 64 failed!\n");
			return probe_ret;
		}
	}
#endif

	/* init unit tests */
	if (IS_ENABLED(CONFIG_PPV4_UT) && IS_ENABLED(CONFIG_DEBUG_FS)) {
		probe_ret = pput_init(pp_dbgfs_dir);
		if (unlikely(probe_ret)) {
			pr_err("Failed to init unit tests, ret %d\n",
			       probe_ret);
		}
	}

	if (likely(!probe_ret))
		init_done = true;

	return probe_ret;
}

/**
 * @brief Platform driver remove function, free all resources
 *        that was used by the driver
 * @note Doesn't change any HW configuration or registers
 *
 * @return int 0 on success
 */
static int __pp_dev_remove(struct platform_device *pdev)
{
	pr_info("start\n");

	ready = false;

	/* clean other modules first */
	if (IS_ENABLED(CONFIG_PPV4_UT) && IS_ENABLED(CONFIG_DEBUG_FS))
		pput_exit();

	__pp_api_exit();
	__pp_hal_exit(pdev);

	dev_set_drvdata(&pdev->dev, NULL);
	if (dts_cfg)
		devm_kfree(&pdev->dev, dts_cfg);
	dts_cfg = NULL;

	kobject_put(pp_sysfs_dir);

	if (IS_ENABLED(CONFIG_DEBUG_FS))
		debugfs_remove_recursive(pp_dbgfs_dir);

	pr_info("done\n");

	return 0;
}

/**
 * @brief Module exit function
 * @return int 0 on success
 */
static void __pp_dev_exit(void)
{
	pr_info("start\n");

	platform_device_unregister(pp_pdev);
	platform_driver_unregister(&pp_plat_drv);
	pp_pdev = NULL;

	pr_info("done\n");
}

/**
 * @brief PP driver late init call, mainly for initializing
 *        the UCs FWs
 * @note this is a late init routine of the PP driver and should
 *       be called by any driver!!!
 * @return s32 0 on success, error code otherwise
 */
s32 pp_dev_late_init(void)
{
	void *qdev;
	s32 ret;

	if (unlikely(!init_done)) {
		pr_err("PP init is not done\n");
		return -EPERM;
	}

	if (unlikely(ready)) {
		pr_err("PP init was already completed\n");
		return -EPERM;
	}

	if (unlikely(ptr_is_null(dts_cfg)))
		return -EPERM;

	/* init pp uc */
	ret = uc_init(&dts_cfg->uc_params);
	if (unlikely(ret)) {
		pr_err("Failed to init uc module\n");
		return ret;
	}

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(!qdev)) {
		pr_err("failed to open qos dev %u\n", PP_QOS_INSTANCE_ID);
		return -EINVAL;
	}

	ret = qos_fw_init(qdev);
	if (unlikely(ret)) {
		pr_err("failed to init qos fw, ret %d\n", ret);
		return ret;
	}

	/* open the datapath */
	ret = rpb_start(&dts_cfg->rpb_params);
	if (unlikely(ret)) {
		pr_err("Failed to start rpb\n");
		return ret;
	}

	/* Init synch queues */
	ret = smgr_sq_init(&dts_cfg->smgr_params);
	if (unlikely(ret)) {
		pr_err("Failed to start synchronization queues\n");
		return ret;
	}

	/* finally, pp driver is ready */
	ready = true;
	pr_info("PP is ready!\n");
	return 0;
}
EXPORT_SYMBOL(pp_dev_late_init);

/**
 * @brief Module init function
 * @return int 0 on success
 */
static int __pp_dev_init(void)
{
	int ret = 0;

	pr_debug("start\n");

	probe_ret = -EIO;

	ret = platform_driver_register(&pp_plat_drv);
	if (unlikely(ret < 0)) {
		pr_err("Failed to register platform driver, ret = %u\n", ret);
		return ret;
	}

	if (unlikely(probe_ret)) {
		pr_err("probe failed (%d)\n", probe_ret);
		__pp_dev_exit();
		return probe_ret;
	}

	pr_debug("done\n");
	return 0;
}

/**
 * @brief Modules attributes
 */
MODULE_AUTHOR("Intel Corporation");
MODULE_DESCRIPTION(DRV_NAME);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRV_VERSION);
#ifdef CONFIG_PPV4_LGM
device_initcall(__pp_dev_init);
#else
arch_initcall(__pp_dev_init);
#endif
module_exit(__pp_dev_exit);
