// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2018 Intel Corporation.
 * Zhu Yixin <yixin.zhu@intel.com>
 */
#include <linux/clk.h>
#include <linux/debugfs.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <uapi/linux/icc/drv_icc.h>

#include "internal.h"

struct regmap_icc_context {
	void __iomem *regs;
	phys_addr_t  paddr;
	unsigned int val_bytes;
	bool         attached_clk;
	const char   *name;
	icc_devices  icc_client;
	struct clk   *clk;
	struct dentry *debugfs;
	int (*reg_write)(struct regmap_icc_context *ctx,
			 unsigned int reg, unsigned int val);
	int (*reg_read)(struct regmap_icc_context *ctx,
			unsigned int reg, unsigned int *val);
};

static struct dentry *debugfs;

static int regmap_icc_regbits_check(size_t reg_bits)
{
	switch (reg_bits) {
	case 32:
		return 0;
	case 8:
	case 16:
	case 64:
	default:
		pr_err("regbits %zu not supported by regmap icc\n", reg_bits);
		return -EINVAL;
	}
}

static int regmap_icc_get_min_stride(size_t val_bits)
{
	int min_stride;

	switch (val_bits) {
	case 32:
		min_stride = 4;
		break;
	case 8:
	case 16:
	case 64:
	default:
		return -EINVAL;
	}

	return min_stride;
}

static int
regmap_icc_write32le(struct regmap_icc_context *ctx, unsigned int reg,
		     unsigned int val)
{
	if (ctx->icc_client >= REGMAP_CGU && ctx->icc_client <= REGMAP_CT) {
		pr_debug("===%s: icc regmap write: reg: 0x%x, val: 0x%x===\n",
		 ctx->name, reg, val);
		return icc_regmap_sync_write(ctx->icc_client,
						ctx->paddr + reg, val);
	}
	return 0;
}

static int regmap_icc_write(void *context, unsigned int reg, unsigned int val)
{
	struct regmap_icc_context *ctx = context;
	int ret;

	if (!IS_ERR(ctx->clk)) {
		ret = clk_enable(ctx->clk);
		if (ret < 0)
			return ret;
	}

	ret = ctx->reg_write(ctx, reg, val);
	if (ret)
		pr_err("icc write err %d: addr: %p, val: 0x%x\n",
		       ret, ctx->regs + reg, val);

	if (!IS_ERR(ctx->clk))
		clk_disable(ctx->clk);

	return ret;
}

static int
regmap_icc_read32le(struct regmap_icc_context *ctx, unsigned int reg,
		    unsigned int *val)
{
	if (ctx->icc_client >= REGMAP_CGU && ctx->icc_client <= REGMAP_CT) {
		pr_debug("===%s: icc regmap read: reg: 0x%x===\n",
				ctx->name, reg);
		return icc_regmap_sync_read(ctx->icc_client,
						ctx->paddr + reg, val);
	}
	return 0;
}

static int regmap_icc_read(void *context, unsigned int reg, unsigned int *val)
{
	struct regmap_icc_context *ctx = context;
	int ret;

	if (!IS_ERR(ctx->clk)) {
		ret = clk_enable(ctx->clk);
		if (ret < 0)
			return ret;
	}

	ret = ctx->reg_read(ctx, reg, val);
	if (ret)
		pr_err("icc read err %d: addr: %p\n", ret, ctx->regs + reg);

	if (!IS_ERR(ctx->clk))
		clk_disable(ctx->clk);

	return 0;
}

static void regmap_icc_free_context(void *context)
{
	struct regmap_icc_context *ctx = context;

	if (!IS_ERR(ctx->clk)) {
		clk_unprepare(ctx->clk);
		if (!ctx->attached_clk)
			clk_put(ctx->clk);
	}
	kfree(context);
}

static const struct regmap_bus regmap_icc = {
	.fast_io                   = 1,
	.reg_write                 = regmap_icc_write,
	.reg_read                  = regmap_icc_read,
	.free_context              = regmap_icc_free_context,
	.val_format_endian_default = REGMAP_ENDIAN_LITTLE,
};

static struct regmap_icc_context
*regmap_icc_gen_context(struct device *dev, const char *clk_id,
			void __iomem *regs, phys_addr_t paddr,
			const struct regmap_config *config)
{
	struct regmap_icc_context *ctx;
	int min_stride;
	int ret;
	icc_devices client_id;

	if (strstr(config->name, "reset"))
		client_id = REGMAP_RCU;
	else if (strstr(config->name, "cgu") ||
		 (strstr(config->name, "gphy_cdb_pdi")) ||
		 (strstr(config->name, "gphy_cdb_fcsi")))
		client_id = REGMAP_CGU;
	else if (strstr(config->name, "chiptop"))
		client_id = REGMAP_CT;
	else
		return ERR_PTR(-EINVAL);

	ret = regmap_icc_regbits_check(config->reg_bits);
	if (ret)
		return ERR_PTR(ret);

	if (config->pad_bits)
		return ERR_PTR(-EINVAL);

	min_stride = regmap_icc_get_min_stride(config->val_bits);
	if (min_stride < 0)
		return ERR_PTR(min_stride);

	if (config->reg_stride < min_stride)
		return ERR_PTR(-EINVAL);

	ctx = kzalloc(sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return ERR_PTR(-ENOMEM);

	ctx->regs = regs;
	ctx->paddr = paddr;
	ctx->val_bytes = config->val_bits / 8;
	ctx->clk = ERR_PTR(-ENODEV);
	ctx->name = config->name;
	ctx->icc_client = client_id;

	switch (regmap_get_val_endian(dev, &regmap_icc, config)) {
	case REGMAP_ENDIAN_DEFAULT:
	case REGMAP_ENDIAN_LITTLE:
	case REGMAP_ENDIAN_BIG:
#ifdef __LITTLE_ENDIAN
	case REGMAP_ENDIAN_NATIVE:
#endif
		ctx->reg_read = regmap_icc_read32le;
		ctx->reg_write = regmap_icc_write32le;
		break;
	default:
		ret = -EINVAL;
		goto err_free;
	}

	if (!clk_id)
		return ctx;

	ctx->clk = clk_get(dev, clk_id);
	if (IS_ERR(ctx->clk)) {
		ret = PTR_ERR(ctx->clk);
		goto err_free;
	}

	ret = clk_prepare(ctx->clk);
	if (ret < 0) {
		clk_put(ctx->clk);
		goto err_free;
	}

	return ctx;

err_free:
	kfree(ctx);

	return ERR_PTR(ret);
}

struct regmap *__regmap_init_icc_clk(struct device *dev, const char *clk_id,
				     void __iomem *regs, phys_addr_t paddr,
				     const struct regmap_config *config,
				     struct lock_class_key *lock_key,
				     const char *lock_name)
{
	struct regmap_icc_context *ctx;

	ctx = regmap_icc_gen_context(dev, clk_id, regs, paddr, config);
	if (IS_ERR(ctx))
		return ERR_CAST(ctx);

	return __regmap_init(dev, &regmap_icc, ctx, config,
			     lock_key, lock_name);
}
EXPORT_SYMBOL_GPL(__regmap_init_icc_clk);

struct regmap *__devm_regmap_init_icc_clk(struct device *dev,
					  const char *clk_id,
					  void __iomem *regs, phys_addr_t paddr,
					  const struct regmap_config *config,
					  struct lock_class_key *lock_key,
					  const char *lock_name)
{
	struct regmap_icc_context *ctx;

	ctx = regmap_icc_gen_context(dev, clk_id, regs, paddr, config);
	if (IS_ERR(ctx))
		return ERR_CAST(ctx);

	return __devm_regmap_init(dev, &regmap_icc, ctx, config,
				  lock_key, lock_name);
}
EXPORT_SYMBOL_GPL(__devm_regmap_init_icc_clk);

int regmap_icc_attach_clk(struct regmap *map, struct clk *clk)
{
	struct regmap_icc_context *ctx = map->bus_context;

	ctx->clk = clk;
	ctx->attached_clk = true;

	return clk_prepare(ctx->clk);
}
EXPORT_SYMBOL_GPL(regmap_icc_attach_clk);

void regmap_icc_detach_clk(struct regmap *map)
{
	struct regmap_icc_context *ctx = map->bus_context;

	clk_unprepare(ctx->clk);

	ctx->attached_clk = false;
	ctx->clk = NULL;
}
EXPORT_SYMBOL_GPL(regmap_icc_detach_clk);

static ssize_t
regmap_reg_dbg(struct file *s, const char __user *buffer,
	       size_t count, loff_t *pos)
{
	char buf[64] = {0};
	char action;
	u32 icc_id, reg, val = 0;
	size_t buf_size;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (count >= sizeof(buf))
		return -EINVAL;

	buf_size = min(count, sizeof(buf) - 1);
	if (copy_from_user(buf, buffer, buf_size))
		return -EFAULT;

	if (strcmp(buf, "help") == 0)
		goto __err_help;

	if (sscanf(buf, "%c %u 0x%x 0x%x", &action, &icc_id, &reg, &val) > 4)
		goto __err_help;

	/*its not allowed client */
	if (!(icc_id >= REGMAP_CGU && icc_id <= REGMAP_CT)) {
		pr_err("Possible values for client id is [%u] to [%u]!\n",
				REGMAP_CGU, REGMAP_CT);
		goto __err_help;
	}

	switch (action) {
	case 'w':
		ret = icc_regmap_sync_write(icc_id, reg, val);
		if (ret != 0)
			pr_info("icc_regmap_sync_write failed %d\n", ret);
	break;
	case 'r':
		ret = icc_regmap_sync_read(icc_id, reg, &val);
		if (ret != 0)
			pr_info("icc_regmap_sync_read failed %d\n", ret);
		else
			pr_info("read: addr 0x%x val 0x%x\n", reg, val);
	break;
	default:
		goto __err_help;
	}

	return count;

__err_help:
	pr_info("icc client ids: CGU %u RCU %u CHIPTOP %u\n",
		REGMAP_CGU, REGMAP_RCU, REGMAP_CT);
	pr_info("echo r <icc client> <addr> > dbg\n");
	pr_info("echo w <icc client> <addr> <value> > dbg\n");
	return count;
}

static const struct file_operations regmap_icc_dbg_fops = {
	.write = regmap_reg_dbg,
	.llseek = noop_llseek,
};

static int __init regmap_icc_debugfs_init(void)
{
	debugfs = debugfs_create_dir("regmap-icc", NULL);
	if (IS_ERR_OR_NULL(debugfs))
		return -ENOMEM;

	if (!debugfs_create_file("dbg", 0200, debugfs, NULL,
				 &regmap_icc_dbg_fops))
		return -ENOMEM;

	return 0;
}
device_initcall(regmap_icc_debugfs_init);

MODULE_LICENSE("GPL v2");
