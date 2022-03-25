/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2017 Intel Corporation.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of version 2 of the GNU General Public License as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *  The full GNU General Public License is included in this distribution
 *  in the file called LICENSE.GPL.
 *
 *  Contact Information:
 *  Intel Corporation
 *  2200 Mission College Blvd.
 *  Santa Clara, CA  97052
 */

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/debugfs.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/dma-mapping.h>
#include <linux/clk.h>

#include "pp_bm_drv.h"
#include "pp_bm_drv_internal.h"
#include "pp_bm_regs.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Intel Corporation");
MODULE_AUTHOR("obakshe");
MODULE_DESCRIPTION("Intel(R) PPv4 buffer_manager Driver");
MODULE_VERSION(BMGR_DRIVER_VERSION);

////////////////////
//// PROTOTYPES ////
////////////////////

// Static function only. External function are declared in the h file
static int buffer_manager_probe(struct platform_device *pdev);
static int buffer_manager_remove(struct platform_device *pdev);

//////////////////////////
//// Global variables ////
//////////////////////////

#define RC_SUCCESS	(0)

static const struct of_device_id bm_match[] = {
	{ .compatible = "intel,prx300-bm" },
	{},
};
MODULE_DEVICE_TABLE(of, bm_match);

//!< Platform driver
static struct platform_driver	g_bm_platform_driver = {
	.probe = buffer_manager_probe,
	.remove = buffer_manager_remove,
	.driver = {
			.owner = THIS_MODULE,
			.name = "buffer_manager",
#ifdef CONFIG_OF
			.of_match_table = bm_match,
#endif
		  },
};

//!< global pointer for private database
static struct bmgr_driver_private	*this;

void __iomem	*bm_config_addr_base;
void __iomem	*bm_policy_mngr_addr_base;
void __iomem	*uc_mcdma0_config_addr_base;

#define BM_BASE		(bm_config_addr_base)		// (0x18B00000)
#define BM_RAM_BASE	(bm_policy_mngr_addr_base)	// (0x18B10000)

#define IO_VIRT(phy_addr)	((phy_addr) + this->debugfs_info.virt2phyoff)
#define DDR_VIRT(phy_addr)	((phy_addr) + this->debugfs_info.ddrvirt2phyoff)

//#define RD_DDR32(addr)	(*(volatile u32 *)(DDR_VIRT(addr)))
#define WR_DDR32(addr, var)	((*(volatile u32 *)(DDR_VIRT(addr))) = var)

#define RD_REG_32(addr)	__raw_readl((volatile u32 *)(addr))
#define WR_REG_32(addr, val)	__raw_writel(val, (volatile u32 *)(addr))
//#define WR_REG_32(addr, var)	((*(volatile u32 *)(IO_VIRT(addr))) = var)
//#define RD_REG_32(addr)	(*(volatile u32 *)(IO_VIRT(addr)))

//////////////
//// APIs ////
//////////////

/**************************************************************************
 *! \fn	bmgr_db_lock
 **************************************************************************
 *
 *  \brief	This function locks the DB in a recursion-save manner
 *
 *  \return	void
 *
 **************************************************************************/
static inline void bmgr_db_lock(void)
{
	spin_lock_bh(&this->driver_db.db_lock);
}

/**************************************************************************
 *! \fn	bmgr_db_unlock
 **************************************************************************
 *
 *  \brief	This function unlocks the DB in a recursion-save manner
 *
 *  \return	void
 *
 **************************************************************************/
static inline void bmgr_db_unlock(void)
{
	spin_unlock_bh(&this->driver_db.db_lock);
}

/**************************************************************************
 *! \fn	buffer_manager_db_init
 **************************************************************************
 *
 *  \brief	Initialize the DB
 *
 *  \param	priv:	buffer_manager private data pointer
 *
 *  \return	0 on success, other error code on failure
 *
 **************************************************************************/
static s32 buffer_manager_db_init(struct bmgr_driver_private *priv)
{
	struct bmgr_driver_db	*db = &priv->driver_db;

	if (!db) {
		pr_err("buffer_manager_db_init(): db was not initialized successfuly!\n");
		return -EINVAL;
	}

	memset(db, 0, sizeof(struct bmgr_driver_db));

	spin_lock_init(&db->db_lock);

	pr_info("buffer_manager_db_init(): db was initialized successfuly\n");

	return 0;
}

static struct resource *get_resource(struct platform_device *pdev,
				     const char *name,
				     unsigned int type,
				     size_t size)
{
	struct resource *r;
	size_t sz;
	struct device *dev;

	dev = &pdev->dev;

	r = platform_get_resource_byname(pdev, type, name);
	if (!r) {
		dev_err(dev, "Could not get %s resource\n", name);
		return NULL;
	}

	sz = resource_size(r);
	if (size && sz != size) {
		dev_err(dev, "Illegal size %zu for %s resource expected %zu\n",
			sz, name, size);
		return NULL;
	}

	return r;
}

static void print_resource(struct device *dev,
			   const char *name,
			   struct resource *r)
{

	dev_dbg(dev, "%s memory resource: start(0x%08zX), size(%zu)\n",
		 name,
		 (size_t)(uintptr_t)r->start,
		 (size_t)(uintptr_t)resource_size(r));
}

static void __iomem *get_resource_mapped_addr(struct platform_device *pdev,
					      const char *name,
					      unsigned int size)
{
	struct resource	*res;

	res = get_resource(pdev, name, IORESOURCE_MEM, size);
	if (!res)
		return NULL;

	print_resource(&pdev->dev, name, res);

	return devm_ioremap_resource(&pdev->dev, res);
}

/**************************************************************************
 *! \fn	buffer_manager_probe
 **************************************************************************
 *
 *  \brief	probe platform device hook
 *
 *  \param	pdev:	platform device pointer
 *
 *  \return	0 on success, other error code on failure
 *
 **************************************************************************/
static int buffer_manager_probe(struct platform_device *pdev)
{
	int				ret;
	struct bmgr_driver_private	*priv;
	u32				val;
	struct device_node		*node;
	int				err;
	struct clk			*ppv4_freq_clk;
	struct clk			*ppv4_gate_clk;

	dev_dbg(&pdev->dev, "BM probe...\n");

	node = pdev->dev.of_node;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	platform_set_drvdata(pdev, priv);
	priv->pdev = pdev;
	priv->enabled = true;
	this = priv;

	ret = buffer_manager_db_init(priv);
	if (ret) {
		kfree(priv);
		return -ENOMEM;
	}

	bm_config_addr_base = get_resource_mapped_addr(pdev,
						       "reg-config",
						       0x10000);
	bm_policy_mngr_addr_base = get_resource_mapped_addr(pdev,
							    "ram-config",
							    0x10000);
	uc_mcdma0_config_addr_base = get_resource_mapped_addr(pdev,
							      "uc-mcdma0",
							      0x200);

	if (!bm_config_addr_base ||
	    !bm_policy_mngr_addr_base ||
	    !uc_mcdma0_config_addr_base) {
		dev_err(&pdev->dev, "failed to request and remap io ranges\n");
		return -ENOMEM;
	}

	err = of_property_read_u32(node, "intel,max-policies", &val);
	if (err) {
		dev_err(&pdev->dev,
			"Could not get max policies from DT, error is %d\n",
			err);
		return -ENODEV;
	}

	dev_info(&pdev->dev, "max-policies = %d\n", val);
	this->driver_db.max_policies = val;
	if (this->driver_db.max_policies > PP_BMGR_MAX_POLICIES) {
		dev_err(&pdev->dev, "max policies %d is %d\n",
			this->driver_db.max_policies, PP_BMGR_MAX_POLICIES);
		return -ENODEV;
	}

	err = of_property_read_u32(node, "intel,max-groups", &val);
	if (err) {
		dev_err(&pdev->dev,
			"Could not get max groups from DT, error is %d\n",
			err);
		return -ENODEV;
	}

	dev_info(&pdev->dev, "max-groups = %d\n", val);
	this->driver_db.max_groups = val;
	if (this->driver_db.max_groups > PP_BMGR_MAX_GROUPS) {
		dev_err(&pdev->dev, "max groups %d is %d\n",
			this->driver_db.max_groups, PP_BMGR_MAX_GROUPS);
		return -ENODEV;
	}

	err = of_property_read_u32(node, "intel,max-pools", &val);
	if (err) {
		dev_err(&pdev->dev,
			"Could not get max pools from DT, error is %d\n",
			err);
		return -ENODEV;
	}

	dev_info(&pdev->dev, "max-pools = %d\n", val);
	this->driver_db.max_pools = val;
	if (this->driver_db.max_pools > PP_BMGR_MAX_POOLS) {
		dev_err(&pdev->dev, "max pools %d is %d\n",
			this->driver_db.max_pools, PP_BMGR_MAX_POOLS);
		return -ENODEV;
	}

	ppv4_freq_clk = devm_clk_get(&pdev->dev, "freq");
	if (IS_ERR(ppv4_freq_clk)) {
		ret = PTR_ERR(ppv4_freq_clk);
		dev_err(&pdev->dev, "failed to get ppv4_freq_clk:%d\n", ret);
		return ret;
	}

	ppv4_gate_clk = devm_clk_get(&pdev->dev, "ppv4");
	if (IS_ERR(ppv4_gate_clk)) {
		ret = PTR_ERR(ppv4_gate_clk);
		dev_err(&pdev->dev, "failed to get ppv4_gate_clk:%d\n", ret);
		return ret;
	}

	clk_prepare_enable(ppv4_gate_clk);
	clk_prepare_enable(ppv4_freq_clk);

	bm_dbg_dev_init(pdev);

	return 0;
}

/**************************************************************************
 *! \fn	buffer_manager_remove
 **************************************************************************
 *
 *  \brief	probe platform device hook
 *
 *  \param	pdev:	platform device pointer
 *
 *  \return	0 on success, other error code on failure
 *
 **************************************************************************/
static int buffer_manager_remove(struct platform_device *pdev)
{
	struct bmgr_driver_private	*priv = platform_get_drvdata(pdev);

	priv->enabled = 0;

	kfree(priv);

	platform_set_drvdata(pdev, NULL);
	this = NULL;

	bm_dbg_dev_clean(pdev);

	dev_dbg(&pdev->dev, "buffer_manager_remove(): remove done\n");

	return 0;
}

/**************************************************************************
 *! \fn	buffer_manager_driver_init
 **************************************************************************
 *
 *  \brief	Init platform device driver
 *
 *  \return	0 on success, other error code on failure
 *
 **************************************************************************/
static int __init buffer_manager_driver_init(void)
{
	int ret;

	bm_dbg_module_init();

	ret = platform_driver_register(&g_bm_platform_driver);
	if (ret < 0) {
		pr_err("platform_driver_register failed (%d)\n", ret);
		return ret;
	}

	pr_info("buffer manager driver init done\n");

	return 0;
}

/**************************************************************************
 *! \fn	buffer_manager_driver_exit
 **************************************************************************
 *
 *  \brief	Exit platform device driver
 *
 *  \return	0 on success, other error code on failure
 *
 **************************************************************************/
static void __exit buffer_manager_driver_exit(void)
{
	platform_driver_unregister(&g_bm_platform_driver);

	bm_dbg_module_clean();

	pr_debug("buffer manager driver exit done\n");
}

/*************************************************/
/**		Module Declarations		**/
/*************************************************/
//module_init(buffer_manager_driver_init);
arch_initcall(buffer_manager_driver_init);
module_exit(buffer_manager_driver_exit);

void copy_dma(u32 src, u32 dst, u32 flags)
{
	// base-0x18850000
	u32 MCDMA0_BASE_ADDR = (u32)uc_mcdma0_config_addr_base;
	u32 MCDMA_SRC_OFFSET = 0;
	u32 MCDMA_DST_OFFSET = 0x4;
	u32 MCDMA_CONTROL_OFFSET = 0x8;
	u32 mcdma_channel = 0;
	u32 active_bit = (1 << 30);
	u32 pxp_offset = 0;
	struct timespec start_ts;
	struct timespec end_ts;
	u32 DMA0_SRC_ADDR = MCDMA0_BASE_ADDR +
		MCDMA_SRC_OFFSET + 0x40 * mcdma_channel;
	u32 DMA0_DST_ADDR = MCDMA0_BASE_ADDR +
		MCDMA_DST_OFFSET + 0x40 * mcdma_channel;
	u32 DMA0_CTRL_ADDR = MCDMA0_BASE_ADDR +
		MCDMA_CONTROL_OFFSET + 0x40 * mcdma_channel;

	WR_REG_32(DMA0_SRC_ADDR, src - pxp_offset); //source address
	WR_REG_32(DMA0_DST_ADDR, dst - pxp_offset); //destination address
	pr_info("Copying from ADDR 0x%x to ADDR 0x%x\n",
		src - pxp_offset, dst - pxp_offset);
	// 8 bytes burst, total size is 8 byte which is 64 bits
	WR_REG_32(DMA0_CTRL_ADDR, flags);
	pr_info("SLEEP\n");
	msleep(100);

	getnstimeofday(&start_ts);

	// wait for Active bit 30 will be 0
	while ((RD_REG_32(DMA0_CTRL_ADDR) & active_bit) != 0) {
		getnstimeofday(&end_ts);
		if ((timespec_sub(end_ts, start_ts).tv_sec) > 5) {
			pr_err("!!!!!!! DMA TIMEOUT !!!!!!!\n");
			return;
		}
	}

	msleep(100);
	pr_info("Copying DONE (control 0x%x)\n", RD_REG_32(DMA0_CTRL_ADDR));
}

/**************************************************************************
 *! \fn	bmgr_wait_for_init_completion
 **************************************************************************
 *
 *  \brief	This function reads the init bit and waits for completion
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_wait_for_init_completion(void)
{
	u32	st;
	struct timespec start_ts;
	struct timespec end_ts;

	pr_debug("Waiting for operation complete....");

	getnstimeofday(&start_ts);

	do {
		getnstimeofday(&end_ts);
		if ((timespec_sub(end_ts, start_ts).tv_sec) > 5) {
			pr_err("!!!!!!! WAIT COMPLETETION TIMEOUT !!!!!!!\n");
			return -EINVAL;
		}
		st = (RD_REG_32(BMGR_STATUS_REG_ADDR(BM_BASE)) & (1));
	} while (st);

	pr_debug("Done\n");

	return RC_SUCCESS;
}

static int is_pow_2(int num)
{
	int pow;

	if (num == 0)
		return 0;

	for (pow = 1; pow > 0; pow <<= 1)
	{
		if (pow == num)
			return 1;
		if (pow > num)
			return 0;
	}

	return 0;
}

/**************************************************************************
 *! \fn	bmgr_is_pool_params_valid
 **************************************************************************
 *
 *  \brief	Check wheather pool parameters are valid
 *
 *  \param	pool_params:	Pool param from user
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_is_pool_params_valid(
		const struct bmgr_pool_params * const pool_params)
{
	// Validity check
	if (!pool_params) {
		pr_err("bmgr_is_pool_params_valid: pool_params is NULL\n");
		return -EINVAL;
	}

	if (pool_params->size_of_buffer < BMGR_MIN_POOL_BUFFER_SIZE) {
		pr_err("bmgr_is_pool_params_valid: size_of_buffer %d should be smaller than %d\n",
		       pool_params->size_of_buffer,
		       BMGR_MIN_POOL_BUFFER_SIZE);
		return -EINVAL;
	}

	if (pool_params->size_of_buffer % BMGR_MIN_POOL_BUFFER_SIZE) {
		pr_err("bmgr_is_pool_params_valid: size_of_buffer %d must be aligned to %d bytes\n",
		       pool_params->size_of_buffer,
		       BMGR_MIN_POOL_BUFFER_SIZE);
		return -EINVAL;
	}

	if (pool_params->base_addr_low & 0x3F) {
		pr_err("bmgr_is_pool_params_valid: base_addr_low %d must be aligned to %d bytes\n",
		       pool_params->base_addr_low,
		       BMGR_MIN_POOL_BUFFER_SIZE);
		return -EINVAL;
	}

	if (!IS_ALIGNED(pool_params->num_buffers, 64)) {
		pr_err("Number of buffers must be aligned to 64\n");
		return -EINVAL;
	}

	/* Num_buffers can be up to 2^24 */
	if (pool_params->num_buffers >= 0x1000000) {
		pr_err("Number of buffers can be up to 0x1000000\n");
		return -EINVAL;
	}

	if (pool_params->group_id >= this->driver_db.max_groups) {
		pr_err("bmgr_is_pool_params_valid: group_id %d must be smaller than %d\n",
		       pool_params->group_id, this->driver_db.max_groups);
		return -EINVAL;
	}

	if (pool_params->num_pools > PP_BMGR_MAX_POOLS ||
	    !is_pow_2(pool_params->num_pools)) {
		pr_err("bmgr_is_pool_params_valid: max_pools %d is not valid\n",
		       pool_params->num_pools);
		return -EINVAL;
	}

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_is_policy_params_valid
 **************************************************************************
 *
 *  \brief	Check wheather policy parameters are valid
 *
 *  \param	policy_params:	Policy param from user
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_is_policy_params_valid(
		const struct bmgr_policy_params * const policy_params)
{
	if (!policy_params) {
		pr_err("bmgr_is_policy_params_valid: policy_params is NULL\n");
		return -EINVAL;
	}

	if (policy_params->group_id >= this->driver_db.max_groups) {
		pr_err("group_id %d >= %d\n",
		       policy_params->group_id, this->driver_db.max_groups);
		return -EINVAL;
	}

	if (policy_params->num_pools_in_policy > PP_BMGR_MAX_POOLS_IN_POLICY) {
		pr_err("num_pools_in_policy %d should be up to %d\n",
		       policy_params->num_pools_in_policy,
		       PP_BMGR_MAX_POOLS_IN_POLICY);
		return -EINVAL;
	}

	return RC_SUCCESS;
}

static u32 bmgr_get_control(void)
{
	return RD_REG_32(BMGR_CTRL_REG_ADDR(BM_BASE));
}

static u32 enable_pop(void)
{
	u32 reg = RD_REG_32(BMGR_CTRL_REG_ADDR(BM_BASE));

	reg &= (~0x2);
	WR_REG_32(BMGR_CTRL_REG_ADDR(BM_BASE), reg);

	return RC_SUCCESS;
}

static u32 disable_pop(void)
{
	u32 reg = RD_REG_32(BMGR_CTRL_REG_ADDR(BM_BASE));

	reg |= 0x2;
	WR_REG_32(BMGR_CTRL_REG_ADDR(BM_BASE), reg);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_enable_min_guarantee_per_pool
 **************************************************************************
 *
 *  \brief	Enables/Disables minimum guarantee per pool
 *
 *  \param	pool_id:	Pool ID
 *  \param	enable:		True to enable, false to disable
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_enable_min_guarantee_per_pool(u8 pool_id, u8 enable)
{
	u32	value;
	u32	mask;

	// Validity check
	if (pool_id >= this->driver_db.max_pools) {
		pr_err("pool_id %d out of range %d\n",
		       pool_id, this->driver_db.max_pools);
		return -EINVAL;
	}

	// Read value
	value = RD_REG_32(BMGR_POOL_MIN_GRNT_MASK_REG_ADDR(BM_BASE));
	mask = BIT(pool_id);

	if (enable) {
		value |= mask;
	} else {
		mask = ~mask;
		value &= mask;
	}

	WR_REG_32(BMGR_POOL_MIN_GRNT_MASK_REG_ADDR(BM_BASE), value);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_pool_enable
 **************************************************************************
 *
 *  \brief	Enable pool
 *
 *  \param	pool_id:	Pool ID
 *  \param	enable:		True to enable, false to disable
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_pool_enable(u8 pool_id, bool enable)
{
	u32	value;
	u32	mask;

	// Validity check
	if (pool_id >= this->driver_db.max_pools) {
		pr_err("pool_id %d out of range %d\n",
		       pool_id, this->driver_db.max_pools);
		return -EINVAL;
	}

	// Read value
	value = RD_REG_32(BMGR_POOL_ENABLE_REG_ADDR(BM_BASE));
	mask = (BIT(pool_id) << 16) | BIT(pool_id);

	if (enable) {
		// Pool Enable
		value |= mask;
	} else {
		// Pool disable
		mask = ~mask;
		value &= mask;
	}

	WR_REG_32(BMGR_POOL_ENABLE_REG_ADDR(BM_BASE), value);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_pool_reset_fifo
 **************************************************************************
 *
 *  \brief	Resets the pool fifo
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_pool_reset_fifo(u8 pool_id)
{
	u32	reset_reg;

	// Validity check
	if (pool_id >= this->driver_db.max_pools) {
		pr_err("pool_id %d out of range %d\n",
		       pool_id, this->driver_db.max_pools);
		return -EINVAL;
	}

	reset_reg = RD_REG_32(BMGR_POOL_FIFO_RESET_REG_ADDR(BM_BASE));

	// Set the reset bit to 0
	reset_reg &= ~BIT(pool_id);

	// Pool FIFO Reset
	WR_REG_32(BMGR_POOL_FIFO_RESET_REG_ADDR(BM_BASE), reset_reg);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_configure_ocp_master
 **************************************************************************
 *
 *  \brief	Configures the burst size and number of bursts
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_configure_ocp_master(void)
{
	// OCP Master burst size
	// 256B burst for all pools
	WR_REG_32(BMGR_OCPM_BURST_SIZE_REG_ADDR(BM_BASE), 0xAA);

	// OCP Master number of bursts
	// 2 bursts for all pools
	WR_REG_32(BMGR_OCPM_NUM_OF_BURSTS_REG_ADDR(BM_BASE), 0x55);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_ocp_burst_size
 **************************************************************************
 *
 *  \brief	Gets burst size
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static u32 bmgr_get_ocp_burst_size(void)
{
	return RD_REG_32(BMGR_OCPM_BURST_SIZE_REG_ADDR(BM_BASE));
}

/**************************************************************************
 *! \fn	bmgr_set_pool_size
 **************************************************************************
 *
 *  \brief	Sets pool size
 *
 *  \param	pool_id:	Pool ID
 *  \param	num_buffers:	Number of buffers in pool
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_pool_size(u8 pool_id, u32 num_buffers)
{
	// Sets number of buffers in pools
	WR_REG_32(BMGR_POOL_SIZE_REG_ADDR(BM_BASE, pool_id), num_buffers);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_pool_size
 **************************************************************************
 *
 *  \brief	Returns the number of buffers in pool
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	Number of buffers in pool
 *
 **************************************************************************/
static u32 bmgr_get_pool_size(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_SIZE_REG_ADDR(BM_BASE, pool_id));
}

/**************************************************************************
 *! \fn	bmgr_get_group_available_buffers
 **************************************************************************
 *
 *  \brief	Returns the available buffers in group
 *
 *  \param	group_id:	Group ID
 *
 *  \return	Available buffers in group
 *
 **************************************************************************/
static u32 bmgr_get_group_available_buffers(u8 group_id)
{
	return RD_REG_32(BMGR_GROUP_AVAILABLE_BUFF_REG_ADDR(BM_BASE,
			 group_id));
}

/**************************************************************************
 *! \fn	bmgr_add_group_available_buffers
 **************************************************************************
 *
 *  \brief  Adds the available buffers in group.
 *
 *  \param	group_id:	Group ID
 *  \param	num_buffers:	Available buffres to add
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_add_group_available_buffers(u8 group_id,
					    u32 num_buffers)
{
	u32 reg = bmgr_get_group_available_buffers(group_id);

	reg += num_buffers;
	WR_REG_32(BMGR_GROUP_AVAILABLE_BUFF_REG_ADDR(BM_BASE, group_id), reg);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_set_group_reserved_buffers
 **************************************************************************
 *
 *  \brief	Sets the number of reserved buffers in group
 *
 *  \param	group_id:		Group ID
 *  \param	reserved_buffers:	reserved buffers in group
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_group_reserved_buffers(u8 group_id, u32 reserved_buffers)
{
	WR_REG_32(BMGR_GROUP_RESERVED_BUFF_REG_ADDR(BM_BASE, group_id),
		  reserved_buffers);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_group_reserved_buffers
 **************************************************************************
 *
 *  \brief	Returns the number of reserved buffers in group
 *
 *  \param	group_id:	Group ID
 *
 *  \return	Number of reserved buffers in group
 *
 **************************************************************************/
static u32 bmgr_get_group_reserved_buffers(u8 group_id)
{
	return RD_REG_32(BMGR_GROUP_RESERVED_BUFF_REG_ADDR(BM_BASE,
			 group_id));
}

/**************************************************************************
 *! \fn	bmgr_set_pcu_fifo_base_address
 **************************************************************************
 *
 *  \brief	Sets the pcu fifo base address (In SRAM)
 *
 *  \param	pool_id:	Pool ID
 *  \param	base_address:	PCU Fifo base address
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_pcu_fifo_base_address(u8 pool_id, u32 base_address)
{
	WR_REG_32(BMGR_POOL_PCU_FIFO_BASE_ADDR_REG_ADDR(BM_BASE, pool_id),
		  base_address);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_pcu_fifo_base_address
 **************************************************************************
 *
 *  \brief	Returns the pcu fifo base address (In SRAM)
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	PCU Fifo base address
 *
 **************************************************************************/
static u32 bmgr_get_pcu_fifo_base_address(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_PCU_FIFO_BASE_ADDR_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_set_pcu_fifo_size
 **************************************************************************
 *
 *  \brief	Sets the PCU fifo size
 *
 *  \param	pool_id:	Pool ID
 *  \param	fifo_size:	PCU Fifo size
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_pcu_fifo_size(u8 pool_id, u32 fifo_size)
{
	WR_REG_32(BMGR_POOL_PCU_FIFO_SIZE_REG_ADDR(BM_BASE, pool_id),
		  fifo_size);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_pcu_fifo_size
 **************************************************************************
 *
 *  \brief	Returns the PCU fifo size
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	PCU Fifo size
 *
 **************************************************************************/
static u32 bmgr_get_pcu_fifo_size(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_PCU_FIFO_SIZE_REG_ADDR(BM_BASE, pool_id));
}

/**************************************************************************
 *! \fn	bmgr_set_pcu_fifo_occupancy
 **************************************************************************
 *
 *  \brief	Sets the PCU fifo occupancy
 *
 *  \param	pool_id:	Pool ID
 *  \param	fifo_occupancy:	Occupancy of the pool
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_pcu_fifo_occupancy(u8 pool_id, u32 fifo_occupancy)
{
	WR_REG_32(BMGR_POOL_PCU_FIFO_OCCUPANCY_REG_ADDR(BM_BASE, pool_id),
		  fifo_occupancy);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_pcu_fifo_occupancy
 **************************************************************************
 *
 *  \brief	Returns the PCU fifo occupancy
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	Occupancy of the pool
 *
 **************************************************************************/
static u32 bmgr_get_pcu_fifo_occupancy(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_PCU_FIFO_OCCUPANCY_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_set_pcu_fifo_prog_empty
 **************************************************************************
 *
 *  \brief	Sets the PCU fifo empty threshold
 *
 *  \param	pool_id:	Pool ID
 *  \param	threshold:	PCU fifo empty threshold
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_pcu_fifo_prog_empty(u8 pool_id, u32 threshold)
{
	WR_REG_32(BMGR_POOL_PCU_FIFO_PROG_EMPTY_REG_ADDR(BM_BASE, pool_id),
		  threshold);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_pcu_fifo_prog_empty
 **************************************************************************
 *
 *  \brief	Returns the PCU fifo empty threshold
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	PCU fifo empty threshold
 *
 **************************************************************************/
static u32 bmgr_get_pcu_fifo_prog_empty(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_PCU_FIFO_PROG_EMPTY_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_set_pcu_fifo_prog_full
 **************************************************************************
 *
 *  \brief	Sets the PCU fifo full threshold
 *
 *  \param	pool_id:	Pool ID
 *  \param	threshold:	PCU fifo full threshold
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_pcu_fifo_prog_full(u8 pool_id, u32 threshold)
{
	WR_REG_32(BMGR_POOL_PCU_FIFO_PROG_FULL_REG_ADDR(BM_BASE, pool_id),
		  threshold);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_pcu_fifo_prog_full
 **************************************************************************
 *
 *  \brief	Returns the PCU fifo prog full threshold
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static u32 bmgr_get_pcu_fifo_prog_full(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_PCU_FIFO_PROG_FULL_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_set_ext_fifo_base_addr_low
 **************************************************************************
 *
 *  \brief	Sets the external fifo base low address
 *
 *  \param	pool_id:	Pool ID
 *  \param	low_address:	External fifo base low address
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_ext_fifo_base_addr_low(u8 pool_id, u32 low_address)
{
	WR_REG_32(BMGR_POOL_EXT_FIFO_BASE_ADDR_LOW_REG_ADDR(BM_BASE, pool_id),
		  low_address);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_ext_fifo_base_addr_low
 **************************************************************************
 *
 *  \brief	Returns the external fifo base low address
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	External fifo base low address
 *
 **************************************************************************/
static u32 bmgr_get_ext_fifo_base_addr_low(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_EXT_FIFO_BASE_ADDR_LOW_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_set_ext_fifo_base_addr_high
 **************************************************************************
 *
 *  \brief	Sets the external fifo base high address
 *
 *  \param	pool_id:	Pool ID
 *  \param	high_address:	External fifo base high address
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_ext_fifo_base_addr_high(u8 pool_id, u32 high_address)
{
	WR_REG_32(BMGR_POOL_EXT_FIFO_BASE_ADDR_HIGH_REG_ADDR(BM_BASE, pool_id),
		  high_address);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_ext_fifo_base_addr_high
 **************************************************************************
 *
 *  \brief	Returns the external fifo base high address
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	External fifo base high address
 *
 **************************************************************************/
static u32 bmgr_get_ext_fifo_base_addr_high(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_EXT_FIFO_BASE_ADDR_HIGH_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_set_ext_fifo_occupancy
 **************************************************************************
 *
 *  \brief	Sets external fifo occupancy
 *
 *  \param	pool_id:	Pool ID
 *  \param	occupancy:	External fifo occupancy
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_ext_fifo_occupancy(u8 pool_id, u32 occupancy)
{
	WR_REG_32(BMGR_POOL_EXT_FIFO_OCC_REG_ADDR(BM_BASE, pool_id),
		  occupancy);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_ext_fifo_occupancy
 **************************************************************************
 *
 *  \brief	Returns the external fifo occupancy
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	External fifo occupancy
 *
 **************************************************************************/
static u32 bmgr_get_ext_fifo_occupancy(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_EXT_FIFO_OCC_REG_ADDR(BM_BASE, pool_id));
}

/**************************************************************************
 *! \fn	bmgr_get_pool_allocated_counter
 **************************************************************************
 *
 *  \brief	Returns the number of allocated buffers in pool
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	Number of allocated buffers
 *
 **************************************************************************/
static u32 bmgr_get_pool_allocated_counter(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_ALLOCATED_COUNTER_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_get_pool_pop_counter
 **************************************************************************
 *
 *  \brief	Returns the number of pop operations on pool
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	Number of pop operations
 *
 **************************************************************************/
static u32 bmgr_get_pool_pop_counter(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_POP_COUNTER_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_get_pool_push_counter
 **************************************************************************
 *
 *  \brief	Returns the number of push operations on pool
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	Number of push operations
 *
 **************************************************************************/
static u32 bmgr_get_pool_push_counter(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_PUSH_COUNTER_REG_ADDR(BM_BASE, pool_id));
}

/**************************************************************************
 *! \fn	bmgr_get_pool_burst_write_counter
 **************************************************************************
 *
 *  \brief	Returns the burst write counter
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	Burst write counter
 *
 **************************************************************************/
static u32 bmgr_get_pool_burst_write_counter(u8 pool_id)
{
	return RD_REG_32(BMGR_DDR_BURST_WRITE_COUNTER_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_get_pool_burst_read_counter
 **************************************************************************
 *
 *  \brief	Returns the burst read counter
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	Burst read counter
 *
 **************************************************************************/
static u32 bmgr_get_pool_burst_read_counter(u8 pool_id)
{
	return RD_REG_32(BMGR_DDR_BURST_READ_COUNTER_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_set_pool_watermark_low_threshold
 **************************************************************************
 *
 *  \brief	Sets the watermark low threshold
 *
 *  \param	pool_id:	Pool ID
 *  \param	threshold:	low threshold
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_pool_watermark_low_threshold(u8 pool_id, u32 threshold)
{
	WR_REG_32(BMGR_POOL_WATERMARK_LOW_THRESHOLD_REG_ADDR(BM_BASE, pool_id),
		  threshold);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_pool_watermark_low_threshold
 **************************************************************************
 *
 *  \brief	Returns the watermark low threshold
 *
 *  \param	pool_id:	Pool ID
 *
 *  \return	Low threshold
 *
 **************************************************************************/
static u32 bmgr_get_pool_watermark_low_threshold(u8 pool_id)
{
	return RD_REG_32(BMGR_POOL_WATERMARK_LOW_THRESHOLD_REG_ADDR(BM_BASE,
			 pool_id));
}

/**************************************************************************
 *! \fn	bmgr_get_policy_null_counter
 **************************************************************************
 *
 *  \brief	Returns the policy null counter
 *
 *  \param	policy_id:	Policy ID
 *
 *  \return	Policy null counter
 *
 **************************************************************************/
static u32 bmgr_get_policy_null_counter(u8 policy_id)
{
	return RD_REG_32(BMGR_POLICY_NULL_COUNTER_REG_ADDR(BM_BASE,
			 policy_id));
}

/**************************************************************************
 *! \fn	bmgr_set_policy_max_allowed_per_policy
 **************************************************************************
 *
 *  \brief	Sets policy max allowed buffers per policy
 *
 *  \param	policy_id:	Policy ID
 *  \param	max_allowed:	Max allowed per this policy
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_policy_max_allowed_per_policy(u8 policy_id,
						  u32 max_allowed)
{
	WR_REG_32(BMGR_POLICY_MAX_ALLOWED_ADDR(BM_RAM_BASE, policy_id),
		  max_allowed);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_policy_max_allowed_per_policy
 **************************************************************************
 *
 *  \brief	Returns the policy max allowed buffers per policy
 *
 *  \param	policy_id:	Policy ID
 *
 *  \return	Max allowed per this policy
 *
 **************************************************************************/
static u32 bmgr_get_policy_max_allowed_per_policy(u8 policy_id)
{
	return RD_REG_32(BMGR_POLICY_MAX_ALLOWED_ADDR(BM_RAM_BASE,
			 policy_id));
}

/**************************************************************************
 *! \fn	bmgr_set_policy_min_guaranteed_per_policy
 **************************************************************************
 *
 *  \brief	Sets minimum guaranteed per policy
 *
 *  \param	min_guaranteed:	Minimum guaranteed per policy
 *  \param	policy_id:	Policy ID
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_policy_min_guaranteed_per_policy(u8 policy_id,
						     u32 min_guaranteed)
{
	WR_REG_32(BMGR_POLICY_MIN_GUARANTEED_ADDR(BM_RAM_BASE, policy_id),
		  min_guaranteed);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_policy_min_guaranteed_per_policy
 **************************************************************************
 *
 *  \brief	Returns minimum guaranteed per policy
 *
 *  \param	policy_id:	Policy ID
 *
 *  \return	Minimum guaranteed per policy
 *
 **************************************************************************/
static u32 bmgr_get_policy_min_guaranteed_per_policy(u8 policy_id)
{
	return RD_REG_32(BMGR_POLICY_MIN_GUARANTEED_ADDR(BM_RAM_BASE,
			 policy_id));
}

/**************************************************************************
 *! \fn	bmgr_set_policy_group_association
 **************************************************************************
 *
 *  \brief	Maps group to policy
 *
 *  \param	policy_id:	Policy ID
 *  \param	group_id:	Group ID
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_policy_group_association(u8 policy_id, u8 group_id)
{
	WR_REG_32(BMGR_POLICY_GROUP_ASSOCIATED_ADDR(BM_RAM_BASE, policy_id),
		  (u32)group_id);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_policy_group_association
 **************************************************************************
 *
 *  \brief	Returns the mapped group to this policy
 *
 *  \param	policy_id:	Policy ID
 *
 *  \return	The mapped group ID
 *
 **************************************************************************/
static u32 bmgr_get_policy_group_association(u8 policy_id)
{
	return RD_REG_32(BMGR_POLICY_GROUP_ASSOCIATED_ADDR(BM_RAM_BASE,
			 policy_id));
}

/**************************************************************************
 *! \fn	bmgr_set_policy_pool_mapping
 **************************************************************************
 *
 *  \brief	Maps pool to policy (With priority)
 *
 *  \param	policy_id:	Policy ID
 *  \param	pool_id:	Pool ID
 *  \param	priority:	0 (highest priority) - 3 scale
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_policy_pool_mapping(u8 policy_id, u8 pool_id, u8 priority)
{
	u32	value;
	u32	mask;

	if (priority >= bmgr_policy_pool_priority_max)
		pr_err("highest priority %d allowed is %d\n",
		       priority, bmgr_policy_pool_priority_max - 1);

	value = RD_REG_32(BMGR_POLICY_POOLS_MAPPING_ADDR(BM_RAM_BASE,
							 policy_id));

	// Clear relevant byte
	mask = 0xFF << (8 * priority);
	value &= ~mask;

	// Prepare the value to be wriiten
	value |= (pool_id << (8 * priority));

	WR_REG_32(BMGR_POLICY_POOLS_MAPPING_ADDR(BM_RAM_BASE, policy_id),
		  value);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_policy_pools_mapping
 **************************************************************************
 *
 *  \brief	Returns the mapped pools to this policy
 *
 *  \param	policy_id:	Policy ID
 *
 *  \return	The mapped pool ID
 *
 **************************************************************************/
static u32 bmgr_get_policy_pools_mapping(u8 policy_id)
{
	return RD_REG_32(BMGR_POLICY_POOLS_MAPPING_ADDR(BM_RAM_BASE,
			 policy_id));
}

/**************************************************************************
 *! \fn	bmgr_set_policy_max_allowed_per_pool
 **************************************************************************
 *
 *  \brief	Sets policy max allowed per pool
 *
 *  \param	policy_id:	Policy ID
 *  \param	pool_id:	Pool ID
 *  \param	max_allowed:	Max allowed for this pool
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
static s32 bmgr_set_policy_max_allowed_per_pool(u8 policy_id,
						u8 pool_id,
						u32 max_allowed)
{
	WR_REG_32(BMGR_POLICY_MAX_ALLOWED_PER_POOL_ADDR(BM_RAM_BASE,
							policy_id,
							pool_id),
							max_allowed);

	return RC_SUCCESS;
}

/**************************************************************************
 *! \fn	bmgr_get_policy_max_allowed_per_pool
 **************************************************************************
 *
 *  \brief	Returns the max allowed per pool
 *
 *  \param	policy_id:	Policy ID
 *  \param	pool_id:	Pool ID
 *
 *  \return	Max allowed for this pool
 *
 **************************************************************************/
static u32 bmgr_get_policy_max_allowed_per_pool(u8 policy_id, u8 pool_id)
{
	return RD_REG_32(BMGR_POLICY_MAX_ALLOWED_PER_POOL_ADDR(BM_RAM_BASE,
							       policy_id,
							       pool_id));
}

/**************************************************************************
 *! \fn	bmgr_get_policy_number_of_allocated_buffers
 **************************************************************************
 *
 *  \brief	Returns the number of allocated buffer in this policy
 *
 *  \param	policy_id:	Policy ID
 *
 *  \return	Number of allocated buffer in this policy
 *
 **************************************************************************/
static u32 bmgr_get_policy_number_of_allocated_buffers(u8 policy_id)
{
	return RD_REG_32(BMGR_POLICY_ALLOC_BUFF_COUNTER_ADDR(BM_RAM_BASE,
			 policy_id));
}

/**************************************************************************
 *! \fn	bmgr_get_policy_num_allocated_per_pool
 **************************************************************************
 *
 *  \brief	Returns the number of allocated buffers per pool in this policy
 *
 *  \param	policy_id:	Policy ID
 *  \param	pool_id:	Pool ID
 *
 *  \return	Number of allocated buffers per pool in this policy
 *
 **************************************************************************/
static u32 bmgr_get_policy_num_allocated_per_pool(u8 policy_id,
						  u8 pool_id)
{
	return RD_REG_32(BMGR_POLICY_ALLOC_BUFF_PER_POOL_COUNTER_ADDR(
			 BM_RAM_BASE, pool_id, policy_id));
}

s32 bmgr_test_dma(u32 num_bytes)
{
	void		*addr = NULL;
	void		*addr1 = NULL;
	dma_addr_t	dma;
	dma_addr_t	dma1;

	addr = dma_alloc_coherent(&this->pdev->dev,
				   2 * sizeof(u32),
				   &dma,
				   GFP_KERNEL | GFP_DMA);
	if (!addr) {
		dev_err(&this->pdev->dev, "Could not allocate addr using dmam_alloc_coherent\n");
		return -ENOMEM;
	}

	addr1 = dma_alloc_coherent(&this->pdev->dev,
				    2 * sizeof(u32),
				    &dma1,
				    GFP_KERNEL | GFP_DMA);
	if (!addr1) {
		dev_err(&this->pdev->dev, "Could not allocate addr1 using dmam_alloc_coherent\n");
		dma_free_coherent(&this->pdev->dev, 2 * sizeof(u32), addr, dma);
		return -ENOMEM;
	}

	WR_REG_32(addr, 0);
	WR_REG_32((addr + 4), 0);
	WR_REG_32(addr1, 0xcafecafe);
	WR_REG_32((addr1 + 4), 0x12345678);

	pr_info("ADDRESSES ======> 0x%x[0x%x] ; 0x%x[0x%x] ; 0x%x[0x%x] ; 0x%x[0x%x]\n",
		(u32)addr, (u32)dma, (u32)(addr + 4), (u32)(dma + 4),
		(u32)(addr1), (u32)(dma1), (u32)(addr1 + 4), (u32)(dma1 + 4));
	pr_info("Before TEST ======> 0x%x ; 0x%x ; 0x%x ; 0x%x\n",
		RD_REG_32(addr), RD_REG_32(addr + 4),
		RD_REG_32(addr1), RD_REG_32(addr1 + 4));
	copy_dma((u32)dma1, (u32)dma, (0x80100000 | (8 * num_bytes)));
	pr_info("After TEST ======> 0x%x ; 0x%x ; 0x%x ; 0x%x\n",
		RD_REG_32(addr), RD_REG_32(addr + 4),
		RD_REG_32(addr1), RD_REG_32(addr1 + 4));

	if ((RD_REG_32(addr) == RD_REG_32(addr1)) &&
	    (RD_REG_32(addr + 4) == RD_REG_32(addr1 + 4))) {
		pr_info("\nDMA Test OK\n");
	} else {
		pr_info("\nDMA Test Failed\n");
	}

	dma_free_coherent(&this->pdev->dev, 2 * sizeof(u32), addr1, dma1);
	dma_free_coherent(&this->pdev->dev, 2 * sizeof(u32), addr, dma);

	return 0;
}

/**************************************************************************
 *! \fn	bmgr_pop_buffer
 **************************************************************************
 *
 *  \brief	Pops a buffer from the buffer manager
 *
 *  \param	buff_info:	Buffer information
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
s32 bmgr_pop_buffer(struct bmgr_buff_info * const buff_info)
{
	u32	address = BMGR_DATAPATH_BASE;
	u32	index = 0;
	u32	low = 0;
	u32	high = 0;
	void	*addr;
	dma_addr_t	dma;
	u16	offset = 0;

	if (!buff_info) {
		pr_err("bmgr_pop_buffer: buff_info is NULL\n");
		return -EINVAL;
	}

	if (buff_info->num_allocs > PP_BMGR_MAX_BURST_IN_POP) {
		pr_err("bmgr_pop_buffer: num_allocs %d is limited with %d\n",
		       buff_info->num_allocs, PP_BMGR_MAX_BURST_IN_POP);
		return -EINVAL;
	}

	// Write the Single/Burst bit
	if (buff_info->num_allocs > 1)
		address |= BIT(16);

	// Write the Policy
	address |= (buff_info->policy_id << 8);

	addr = dmam_alloc_coherent(&this->pdev->dev,
				   2 * sizeof(u32) * buff_info->num_allocs,
				   &dma,
				   GFP_KERNEL | GFP_DMA);
	if (!addr) {
		dev_err(&this->pdev->dev, "Could not allocate using dmam_alloc_coherent\n");
		return -ENOMEM;
	}

	copy_dma(address, (u32)dma,
		 (0x80100000 | (8 * buff_info->num_allocs)));

	for (index = 0; index < buff_info->num_allocs; index++) {
		low = RD_REG_32(addr + offset);
		high = RD_REG_32(addr + offset + 4);
		offset += 8;

		pr_info("POP ======> 0x%x ; 0x%x\n", low, high);

		buff_info->addr_low[index] = low;
		buff_info->addr_high[index] = high & 0xF;
		buff_info->pool_id[index] = (high & 0xFF000000) >> 24;

		pr_info("bmgr_pop_buffer: ---> pop buffer from address %p (pool %d, addr low %p, addr high %p)\n",
			(void *)address, buff_info->pool_id[index],
			(void *)buff_info->addr_low[index],
			(void *)buff_info->addr_high[index]);

		if (buff_info->pool_id[index] >= this->driver_db.max_pools) {
			this->driver_db.policies[buff_info->policy_id].
				num_allocated_buffers += index;
			pr_info("Can't pop from policy %d\n",
				buff_info->policy_id);
			return -ENOSPC;
		}

		this->driver_db.pools[buff_info->pool_id[index]].
			num_allocated_buffers++;
	}

	this->driver_db.policies[buff_info->policy_id].
		num_allocated_buffers += buff_info->num_allocs;

	return RC_SUCCESS;
}
EXPORT_SYMBOL(bmgr_pop_buffer);

/**************************************************************************
 *! \fn	bmgr_push_buffer
 **************************************************************************
 *
 *  \brief	Pushes a buffer back to the buffer manager
 *
 *  \param	buff_info:	Buffer information
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
s32 bmgr_push_buffer(struct bmgr_buff_info * const buff_info)
{
	u32	address = BMGR_DATAPATH_BASE;
	u32	value = 0;
	u32	index = 0;
	dma_addr_t	dma;
	void	*addr;
	u32	*ptr2push; // 64 bit per allocation

	if (!buff_info) {
		pr_err("bmgr_push_buffer: buff_info is NULL\n");
		return -EINVAL;
	}

	if (buff_info->num_allocs > PP_BMGR_MAX_BURST_IN_POP) {
		pr_err("bmgr_push_buffer: num_allocs %d is limited with %d\n",
		       buff_info->num_allocs, PP_BMGR_MAX_BURST_IN_POP);
		return -EINVAL;
	}

	// Write the Single/Burst bit
	if (buff_info->num_allocs > 1)
		address |= BIT(16);

	// Write the Policy
	address |= (buff_info->policy_id << 8);

	addr = dmam_alloc_coherent(&this->pdev->dev,
                                    2 * PP_BMGR_MAX_BURST_IN_POP,
                                    &dma,
                                    GFP_KERNEL | GFP_DMA);
	if (!addr) {
		dev_err(&this->pdev->dev, "Could not allocate using dmam_alloc_coherent\n");
		return -ENOMEM;
	}
	ptr2push = (u32 *)addr;

	// write to ddr
	for (index = 0; index < buff_info->num_allocs; index++) {
		ptr2push[2*index] = buff_info->addr_low[index];
		value = (buff_info->addr_high[index] & 0xF) |
			 ((buff_info->pool_id[index] & 0xFF) << 24);
		ptr2push[2*index + 1] = value;

		pr_info("addr 0x%x addr1 0x%x %p %p\n", ptr2push[2*index], ptr2push[2*index+1], &ptr2push[2*index], &ptr2push[2*index+1]);
		pr_info("bmgr_push_buffer: <--- push buffer to address %p (pool %d, addr low %p, addr high %p, value %d)\n",
			(void *)address, buff_info->pool_id[index],
			(void *)buff_info->addr_low[index],
			(void *)buff_info->addr_high[index], value);

		this->driver_db.pools[buff_info->pool_id[index]].
			num_deallocated_buffers++;
	}

	copy_dma((u32)dma, address,
		 (0x80040000 | (8 * buff_info->num_allocs))/*0x80100008*/);

	this->driver_db.policies[buff_info->policy_id].
		num_deallocated_buffers += buff_info->num_allocs;

	return RC_SUCCESS;
}
EXPORT_SYMBOL(bmgr_push_buffer);

/**************************************************************************
 *! \fn	bmgr_driver_init
 **************************************************************************
 *
 *  \brief  Initializes the buffer manager driver.
 *          Must be the first driver's function to be called
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
s32 bmgr_driver_init(void)
{
	u16 index;
	u16 idx2;

	pr_info("Buffer Manager driver is initializing....");

	// @lock
	bmgr_db_lock();

	disable_pop();

	bmgr_configure_ocp_master();

	// Reset group reserved buffers
	for (index = 0; index < this->driver_db.max_groups; index++)
		bmgr_set_group_reserved_buffers(index, 0);

	// Init RAM
	for (index = 0; index < this->driver_db.max_policies; index++) {
		bmgr_set_policy_max_allowed_per_policy(index, 0);
		bmgr_set_policy_min_guaranteed_per_policy(index, 0);
		bmgr_set_policy_group_association(index, 0);
		WR_REG_32(BMGR_POLICY_POOLS_MAPPING_ADDR(BM_RAM_BASE, index),
			  0);
		WR_REG_32(BMGR_POLICY_ALLOC_BUFF_COUNTER_ADDR(BM_RAM_BASE,
							      index), 0);
		for (idx2 = 0; idx2 < this->driver_db.max_pools; idx2++) {
			bmgr_set_policy_max_allowed_per_pool(index, idx2, 0);
			WR_REG_32(BMGR_POLICY_ALLOC_BUFF_PER_POOL_COUNTER_ADDR(
					BM_RAM_BASE, idx2, index), 0);
		}
	}

	// @unlock
	bmgr_db_unlock();

	pr_info("Done\n");

	return RC_SUCCESS;
}
EXPORT_SYMBOL(bmgr_driver_init);

/**************************************************************************
 *! \fn	bmgr_pool_configure
 **************************************************************************
 *
 *  \brief	Configure a Buffer Manager pool
 *
 *  \param	pool_params:	Pool param from user
 *  \param	pool_id[OUT]:	Pool ID
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
s32 bmgr_pool_configure(const struct bmgr_pool_params * const pool_params,
			u8 * const pool_id)
{
	s32		status = RC_SUCCESS;
	void		*pointers_table = NULL;
	u32		index = 0;
	u32		*temp_pointers_table_ptr = NULL;
	u64		user_array_ptr;
	u32		val = 0;
	dma_addr_t	dma;

	pr_info("Configuring buffer manager pool...");

	// @lock
	bmgr_db_lock();

	disable_pop();

	// Validity check
	status = bmgr_is_pool_params_valid(pool_params);
	if (status != RC_SUCCESS)
		goto unlock;

	*pool_id = 0;

	// Find next available slot in pools db
	for (index = 0; index < this->driver_db.max_pools; index++) {
		if (this->driver_db.pools[index].is_busy == 0) {
			*pool_id = index;
			break;
		}
	}

	// If not found (Can be done also using num_pools)
	if (index == this->driver_db.max_pools) {
		pr_err("bmgr_pool_configure: pools DB is full!");
		status = -EIO;

		goto unlock;
	}

	// Allocate pool_param->pool_num_of_buff * POINTER_SIZE bytes array
	pointers_table = dmam_alloc_coherent(&this->pdev->dev,
				sizeof(u32) * pool_params->num_buffers,
				&dma,
				GFP_KERNEL | GFP_DMA);
	if (!pointers_table) {
		pr_err("bmgr_pool_configure: Failed to allocate pointers_table, num_buffers %d",
		       pool_params->num_buffers);
		status = -ENOMEM;
		goto unlock;
	}

	temp_pointers_table_ptr = (u32 *)pointers_table;

	user_array_ptr = (pool_params->base_addr_low) |
			((u64)pool_params->base_addr_high << 32);

	for (index = 0; index < pool_params->num_buffers; index++) {
		u32 temp = user_array_ptr >> 6;
		// for debugging ...
		if (index == 0 || index == pool_params->num_buffers - 1)
			pr_debug("bmgr_pool_configure: index %d) writing 0x%x to 0x%x\n",
				index, temp, (u32)temp_pointers_table_ptr);

		*temp_pointers_table_ptr = user_array_ptr >> 6;
		temp_pointers_table_ptr++;
		user_array_ptr += pool_params->size_of_buffer;
	}

	status = bmgr_set_pool_size(*pool_id, pool_params->num_buffers);
	if (status != RC_SUCCESS)
		goto free_memory;

	status = bmgr_set_ext_fifo_base_addr_low(*pool_id, (u32)dma);
	if (status != RC_SUCCESS)
		goto free_memory;

	status = bmgr_set_ext_fifo_base_addr_high(*pool_id, 0);
	if (status != RC_SUCCESS)
		goto free_memory;

	status = bmgr_set_ext_fifo_occupancy(*pool_id,
					     pool_params->num_buffers);
	if (status != RC_SUCCESS)
		goto free_memory;

	// Verify group is not full
	val = pool_params->group_id;
	index = this->driver_db.groups[val].num_pools_in_group;
	if (index >= PP_BMGR_MAX_POOLS_IN_GROUP) {
		pr_err("bmgr_pool_configure: Group %d is full. num_pools_in_group %d",
		       pool_params->group_id, index);
		status = -EIO;

		goto free_memory;
	}

	// The group was unused
	if (index == 0)
		this->driver_db.num_groups++;

	this->driver_db.groups[pool_params->group_id].pools[index] = *pool_id;
	this->driver_db.groups[pool_params->group_id].available_buffers +=
		pool_params->num_buffers;
	this->driver_db.groups[pool_params->group_id].num_pools_in_group++;
	// Group's reserved buffers will be updated when configuring the policy

	status = bmgr_add_group_available_buffers(pool_params->group_id,
						  pool_params->num_buffers);
	if (status != RC_SUCCESS)
		goto free_memory;

	val = BMGR_DEFAULT_PCU_FIFO_SIZE;
	val /= pool_params->num_pools;
	status = bmgr_set_pcu_fifo_base_address(*pool_id,
						BMGR_START_PCU_FIFO_SRAM_ADDR +
						(*pool_id * val));
	if (status != RC_SUCCESS)
		goto free_memory;

	status = bmgr_set_pcu_fifo_size(*pool_id, val);
	if (status != RC_SUCCESS)
		goto free_memory;

	status = bmgr_set_pcu_fifo_occupancy(*pool_id, 0);
	if (status != RC_SUCCESS)
		goto free_memory;

	val = BMGR_DEFAULT_PCU_FIFO_LOW_THRESHOLD;
	val /= pool_params->num_pools;
	status = bmgr_set_pcu_fifo_prog_empty(*pool_id, val);
	if (status != RC_SUCCESS)
		goto free_memory;

	val = BMGR_DEFAULT_PCU_FIFO_HIGH_THRESHOLD;
	val /= pool_params->num_pools;
	status = bmgr_set_pcu_fifo_prog_full(*pool_id, val);
	if (status != RC_SUCCESS)
		goto free_memory;

	val = BMGR_DEFAULT_WATERMARK_LOW_THRESHOLD;
	status = bmgr_set_pool_watermark_low_threshold(*pool_id, val);
	if (status != RC_SUCCESS)
		goto free_memory;

	if (pool_params->flags & POOL_ENABLE_FOR_MIN_GRNT_POLICY_CALC) {
		status = bmgr_enable_min_guarantee_per_pool(*pool_id, 1);
		if (status != RC_SUCCESS)
			goto free_memory;
	}

	status = bmgr_pool_reset_fifo(*pool_id);
	if (status != RC_SUCCESS)
		goto free_memory;

	status = bmgr_pool_enable(*pool_id, 1);
	if (status != RC_SUCCESS)
		goto free_memory;

	// Update pool's DB
	memcpy(&this->driver_db.pools[*pool_id].pool_params,
	       pool_params, sizeof(struct bmgr_pool_params));
	this->driver_db.pools[*pool_id].is_busy = 1;
	this->driver_db.pools[*pool_id].internal_pointers_tables =
			temp_pointers_table_ptr;
	this->driver_db.num_pools++;

	bmgr_wait_for_init_completion();

	enable_pop();

	// @unlock
	bmgr_db_unlock();

	pr_info("Done");

	// OK
	return status;

free_memory:
	// free pointers_table
		kfree(pointers_table);
unlock:
	enable_pop();

	// @unlock
	bmgr_db_unlock();

	return status;
}
EXPORT_SYMBOL(bmgr_pool_configure);

/**************************************************************************
 *! \fn	bmgr_policy_configure
 **************************************************************************
 *
 *  \brief	Configure a Buffer Manager policy
 *
 *  \param	policy_params:	Policy param from user
 *  \param	policy_id[OUT]:	Policy ID
 *
 *  \return	RC_SUCCESS on success, other error code on failure
 *
 **************************************************************************/
s32 bmgr_policy_configure(const struct bmgr_policy_params * const policy_params,
			  u8 * const policy_id)
{
	s32	status = RC_SUCCESS;
	u16	index = 0;
	u32	id = 0;
	u32	min = 0;
	u32	max = 0;

	pr_info("Configuring buffer manager policy...");

	// @lock
	bmgr_db_lock();

	disable_pop();

	// Validity check
	status = bmgr_is_policy_params_valid(policy_params);
	if (status != RC_SUCCESS)
		goto unlock;

	*policy_id = 0;

	// Find next available slot in policy db
	for (index = 0; index < this->driver_db.max_policies; index++) {
		if (this->driver_db.policies[index].is_busy == 0) {
			*policy_id = index;
			break;
		}
	}

	// If not found (Can be done also using num_policies)
	if (index == this->driver_db.max_policies) {
		pr_err("No free policy!");
		status = -EIO;

		goto unlock;
	}

	status = bmgr_set_policy_group_association(*policy_id,
						   policy_params->group_id);
	if (status != RC_SUCCESS)
		goto unlock;

	max = policy_params->max_allowed;
	status = bmgr_set_policy_max_allowed_per_policy(*policy_id,
							max);
	if (status != RC_SUCCESS)
		goto unlock;

	min = policy_params->min_guaranteed;
	status = bmgr_set_policy_min_guaranteed_per_policy(*policy_id,
							   min);
	if (status != RC_SUCCESS)
		goto unlock;

	// Set the group's reserved buffers
	this->driver_db.groups[policy_params->group_id].reserved_buffers =
		bmgr_get_group_reserved_buffers(policy_params->group_id);
	this->driver_db.groups[policy_params->group_id].reserved_buffers +=
			policy_params->min_guaranteed;

	id = this->driver_db.groups[policy_params->group_id].reserved_buffers;
	status = bmgr_set_group_reserved_buffers(policy_params->group_id, id);
	if (status != RC_SUCCESS)
		goto unlock;

	for (index = 0; index < policy_params->num_pools_in_policy; index++) {
		max = policy_params->pools_in_policy[index].max_allowed;
		status = bmgr_set_policy_max_allowed_per_pool(*policy_id,
							      index,
							      max);
		if (status != RC_SUCCESS)
			goto unlock;

		id = policy_params->pools_in_policy[index].pool_id;
		status = bmgr_set_policy_pool_mapping(*policy_id,
						      id,
						      index);
		if (status != RC_SUCCESS)
			goto unlock;
	}

	// Update Policy DB
	this->driver_db.num_policies++;
	this->driver_db.policies[*policy_id].is_busy = 1;
	memcpy(&this->driver_db.policies[*policy_id].policy_params,
	       policy_params,
	       sizeof(struct bmgr_policy_params));

	bmgr_wait_for_init_completion();

	enable_pop();

	// @unlock
	bmgr_db_unlock();

	pr_info("Done");

	// OK
	return status;

unlock:
	enable_pop();

	// @unlock
	bmgr_db_unlock();

	// Failure
	return status;
}
EXPORT_SYMBOL(bmgr_policy_configure);

void print_hw_stats(void)
{
	u32	counter;
	u16	max_pools = this->driver_db.num_pools;
	u16	max_groups = this->driver_db.num_groups;
	u16	max_policies = this->driver_db.num_policies;
	u16	idx;
	u16	idx1;

	counter = bmgr_get_control();
	pr_info("Control = 0x%x\n", counter);

	counter = RD_REG_32(BMGR_POOL_MIN_GRNT_MASK_REG_ADDR(BM_BASE));
	pr_info("Pool Min Grant Bit Mask = 0x%x\n", counter);

	counter = RD_REG_32(BMGR_POOL_ENABLE_REG_ADDR(BM_BASE));
	pr_info("Pool Enable = 0x%x\n", counter);

	counter = RD_REG_32(BMGR_POOL_FIFO_RESET_REG_ADDR(BM_BASE));
	pr_info("Pool FIFO Reset = 0x%x\n", counter);

	counter = bmgr_get_ocp_burst_size();
	pr_info("OCP Master Burst Size = 0x%x\n", counter);

	counter = RD_REG_32(BMGR_OCPM_NUM_OF_BURSTS_REG_ADDR(BM_BASE));
	pr_info("OCP Master Number Of Bursts = 0x%x\n", counter);

	counter = RD_REG_32(BMGR_STATUS_REG_ADDR(BM_BASE));
	pr_info("Status = 0x%x\n", counter);

	pr_info("Pool size:\n");
	pr_info("==========\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pool_size(idx);
		pr_info("Pool %d size = 0x%x\n", idx, counter);
	}

	pr_info("Group available buffers:\n");
	pr_info("========================\n");
	for (idx = 0; idx < max_groups; idx++) {
		counter = bmgr_get_group_available_buffers(idx);
		pr_info("Group %d available buffers = 0x%x\n", idx, counter);
	}

	pr_info("Group reserved buffers:\n");
	pr_info("=======================\n");
	for (idx = 0; idx < max_groups; idx++) {
		counter = bmgr_get_group_reserved_buffers(idx);
		pr_info("Group %d reserved buffers = 0x%x\n", idx, counter);
	}

	pr_info("PCU FIFO base address:\n");
	pr_info("======================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pcu_fifo_base_address(idx);
		pr_info("Pool %d PCU FIFO base address = 0x%x\n",
			idx, counter);
	}

	pr_info("PCU FIFO size:\n");
	pr_info("==============\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pcu_fifo_size(idx);
		pr_info("Pool %d PCU FIFO size = 0x%x\n", idx, counter);
	}

	pr_info("PCU FIFO occupancy:\n");
	pr_info("===================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pcu_fifo_occupancy(idx);
		pr_info("Pool %d PCU FIFO occupancy = 0x%x\n", idx, counter);
	}

	pr_info("PCU FIFO Prog empty:\n");
	pr_info("====================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pcu_fifo_prog_empty(idx);
		pr_info("Pool %d PCU FIFO Prog empty = 0x%x\n", idx, counter);
	}

	pr_info("PCU FIFO Prog full:\n");
	pr_info("===================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pcu_fifo_prog_full(idx);
		pr_info("Pool %d PCU FIFO Prog full = 0x%x\n", idx, counter);
	}

	pr_info("EXT FIFO Base Addr Low:\n");
	pr_info("=======================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_ext_fifo_base_addr_low(idx);
		pr_info("Pool %d EXT FIFO Base Addr Low = 0x%x\n",
			idx, counter);
	}

	pr_info("EXT FIFO Base Addr High:\n");
	pr_info("========================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_ext_fifo_base_addr_high(idx);
		pr_info("Pool %d EXT FIFO Base Addr High = 0x%x\n",
			idx, counter);
	}

	pr_info("EXT FIFO occupancy:\n");
	pr_info("===================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_ext_fifo_occupancy(idx);
		pr_info("Pool %d EXT FIFO occupancy = 0x%x\n", idx, counter);
	}

	pr_info("Pool allocated counter:\n");
	pr_info("=======================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pool_allocated_counter(idx);
		pr_info("Pool %d allocated counter = 0x%x\n", idx, counter);
	}

	pr_info("Pool pop counter:\n");
	pr_info("=================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pool_pop_counter(idx);
		pr_info("Pool %d pop counter = 0x%x\n", idx, counter);
	}

	pr_info("Pool push counter:\n");
	pr_info("==================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pool_push_counter(idx);
		pr_info("Pool %d push counter = 0x%x\n", idx, counter);
	}

	pr_info("Pool DDR Burst write counter:\n");
	pr_info("=============================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pool_burst_write_counter(idx);
		pr_info("Pool %d Burst write counter = 0x%x\n", idx, counter);
	}

	pr_info("Pool DDR Burst read counter:\n");
	pr_info("============================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pool_burst_read_counter(idx);
		pr_info("Pool %d Burst read counter = 0x%x\n", idx, counter);
	}

	pr_info("Pool watermark low threshold:\n");
	pr_info("=============================\n");
	for (idx = 0; idx < max_pools; idx++) {
		counter = bmgr_get_pool_watermark_low_threshold(idx);
		pr_info("Pool %d watermark low threshold = 0x%x\n",
			idx, counter);
	}

	pr_info("Policy NULL counter:\n");
	pr_info("====================\n");
	for (idx = 0; idx < max_policies; idx++) {
		counter = bmgr_get_policy_null_counter(idx);
		pr_info("Policy %d NULL counter = 0x%x\n", idx, counter);
	}

	pr_info("Policy Max allowed per policy:\n");
	pr_info("==============================\n");
	for (idx = 0; idx < max_policies; idx++) {
		counter = bmgr_get_policy_max_allowed_per_policy(idx);
		pr_info("Policy %d Max allowed = 0x%x\n", idx, counter);
	}

	pr_info("Policy Min guaranteed per policy:\n");
	pr_info("=================================\n");
	for (idx = 0; idx < max_policies; idx++) {
		counter = bmgr_get_policy_min_guaranteed_per_policy(idx);
		pr_info("Policy %d Min guaranteed = 0x%x\n", idx, counter);
	}

	pr_info("Policy group association:\n");
	pr_info("=========================\n");
	for (idx = 0; idx < max_policies; idx++) {
		counter = bmgr_get_policy_group_association(idx);
		pr_info("Policy %d group association = 0x%x\n", idx, counter);
	}

	pr_info("Policy pools mapping:\n");
	pr_info("=====================\n");
	for (idx = 0; idx < max_policies; idx++) {
		counter = bmgr_get_policy_pools_mapping(idx);
		pr_info("Policy %d pools mapping = 0x%x\n", idx, counter);
	}

	pr_info("Policy max allowed per pool:\n");
	pr_info("============================\n");
	for (idx = 0; idx < max_policies; idx++) {
		pr_info("Policy %d:\n", idx);
		for (idx1 = 0; idx1 < max_pools; idx1++) {
			counter = bmgr_get_policy_max_allowed_per_pool(idx,
								       idx1);
			pr_info("    max allowed per pool with priority %d = 0x%x\n",
				idx1, counter);
		}
	}

	pr_info("Policy num allocated buffers:\n");
	pr_info("=============================\n");
	for (idx = 0; idx < max_policies; idx++) {
		counter = bmgr_get_policy_number_of_allocated_buffers(idx);
		pr_info("Policy %d num allocated buffers = 0x%x\n",
			idx, counter);
	}

	pr_info("Policy num allocated buffers per pool:\n");
	pr_info("======================================\n");
	for (idx = 0; idx < max_policies; idx++) {
		pr_info("Policy %d:\n", idx);
		for (idx1 = 0; idx1 < max_pools; idx1++) {
			counter = bmgr_get_policy_num_allocated_per_pool(idx,
									 idx1);
			pr_info("    num allocated per pool with priority %d = 0x%x\n",
				idx1, counter);
		}
	}
}
