// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2016-2018 Linaro Ltd.
 * Copyright (C) 2014 Sony Mobile Communications AB
 * Copyright (c) 2012-2018, The Linux Foundation. All rights reserved.
 */
#include <linux/iopoll.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of_reserved_mem.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/reset.h>
#include <linux/soc/qcom/mdt_loader.h>
#include "qcom_common.h"
#include "qcom_q6v5.h"
#include <linux/rpmsg/qcom_glink.h>
#include <linux/interrupt.h>
#include <linux/qcom_scm.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <uapi/linux/major.h>

#define WCSS_CRASH_REASON		421

/* Q6SS Register Offsets */
#define Q6SS_RESET_REG		0x014
#define Q6SS_GFMUX_CTL_REG		0x020
#define Q6SS_PWR_CTL_REG		0x030
#define Q6SS_MEM_PWR_CTL		0x0B0

/* AXI Halt Register Offsets */
#define AXI_HALTREQ_REG			0x0
#define AXI_HALTACK_REG			0x4
#define AXI_IDLE_REG			0x8

#define HALT_ACK_TIMEOUT_MS		100

/* Q6SS_RESET */
#define Q6SS_STOP_CORE			BIT(0)
#define Q6SS_CORE_ARES			BIT(1)
#define Q6SS_BUS_ARES_ENABLE		BIT(2)

/* Q6SS_GFMUX_CTL */
#define Q6SS_CLK_ENABLE			BIT(1)

/* Q6SS_PWR_CTL */
#define Q6SS_L2DATA_STBY_N		BIT(18)
#define Q6SS_SLP_RET_N			BIT(19)
#define Q6SS_CLAMP_IO			BIT(20)
#define QDSS_BHS_ON			BIT(21)

/* Q6SS parameters */
#define Q6SS_LDO_BYP		BIT(25)
#define Q6SS_BHS_ON		BIT(24)
#define Q6SS_CLAMP_WL		BIT(21)
#define Q6SS_CLAMP_QMC_MEM		BIT(22)
#define HALT_CHECK_MAX_LOOPS		200
#define Q6SS_XO_CBCR		GENMASK(5, 3)

/* Q6SS config/status registers */
#define TCSR_GLOBAL_CFG0	0x0
#define TCSR_GLOBAL_CFG1	0x4
#define SSCAON_CONFIG		0x8
#define SSCAON_STATUS		0xc
#define Q6SS_BHS_STATUS		0x78
#define Q6SS_RST_EVB		0x10

#define BHS_EN_REST_ACK		BIT(0)
#define SSCAON_ENABLE		BIT(13)
#define SSCAON_BUS_EN		BIT(15)
#define SSCAON_BUS_MUX_MASK	GENMASK(18, 16)

#define MEM_BANKS		19
#define TCSR_WCSS_CLK_MASK	0x1F
#define TCSR_WCSS_CLK_ENABLE	0x14

#define WCNSS_PAS_ID		6

struct q6v5_wcss {
	struct device *dev;

	void __iomem *reg_base;
	void __iomem *rmb_base;
	void __iomem *aon_reset;

	struct regmap *halt_map;
	u32 halt_q6;
	u32 halt_wcss;
	u32 halt_nc;
	u32 reset_cmd_id;

	struct reset_control *wcss_aon_reset;
	struct reset_control *wcss_reset;
	struct reset_control *wcss_q6_reset;

	struct qcom_q6v5 q6v5;

	struct qcom_rproc_subdev smd_subdev;
	struct qcom_rproc_glink glink_subdev;
	struct qcom_rproc_ssr ssr_subdev;
	struct qcom_sysmon *sysmon;

	phys_addr_t mem_phys;
	phys_addr_t mem_reloc;
	void *mem_region;
	size_t mem_size;
};

#if defined(CONFIG_IPQ_SS_DUMP)

#define	OPEN_TIMEOUT	5000
#define	DUMP_TIMEOUT	10000

static struct timer_list dump_timeout;
static struct completion dump_complete;

static struct timer_list open_timeout;
static struct completion open_complete;
static atomic_t open_timedout;

static const struct file_operations q6_dump_ops;
static struct class *dump_class;

struct dump_file_private {
	int remaining_bytes;
	int rel_addr_off;
	int ehdr_remaining_bytes;
	char *ehdr;
	struct task_struct *pdesc;
};

struct dumpdev {
	const char *name;
	const struct file_operations *fops;
	fmode_t fmode;
	char ss_name[8];
	phys_addr_t dump_phy_addr;
	size_t dump_size;
} q6dump = {"q6mem", &q6_dump_ops, FMODE_UNSIGNED_OFFSET | FMODE_EXCL, "wcnss"};

static void open_timeout_func(unsigned long data)
{
	atomic_set(&open_timedout, 1);
	complete(&open_complete);
	pr_err("open time Out: Q6 crash dump collection failed\n");
}

static void dump_timeout_func(unsigned long data)
{
	struct dump_file_private *dfp = (struct dump_file_private *)data;

	pr_err("Time Out: Q6 crash dump collection failed\n");

	dump_timeout.data = -ETIMEDOUT;
	send_sig(SIGKILL, dfp->pdesc, 0);
}

static int q6_dump_open(struct inode *inode, struct file *file)
{
	struct dump_file_private *dfp = NULL;

	del_timer_sync(&open_timeout);

	if (atomic_read(&open_timedout) == 1)
		return -ENODEV;

	file->f_mode |= q6dump.fmode;

	dfp = kzalloc(sizeof(struct dump_file_private), GFP_KERNEL);
	if (dfp == NULL) {
		pr_err("%s:\tCan not allocate memory for private structure\n",
				__func__);
		return -ENOMEM;
	}

	dfp->remaining_bytes = q6dump.dump_size;
	dfp->rel_addr_off = 0;
	dfp->pdesc = current;

	file->private_data = dfp;

	dump_timeout.data = (unsigned long)dfp;

	/* This takes care of the user space app stalls during delayed read. */
	init_completion(&dump_complete);

	setup_timer(&dump_timeout, dump_timeout_func, (unsigned long)dfp);
	mod_timer(&dump_timeout, jiffies + msecs_to_jiffies(DUMP_TIMEOUT));

	complete(&open_complete);

	return 0;
}

static int q6_dump_release(struct inode *inode, struct file *file)
{
	int dump_minor =  iminor(inode);
	int dump_major = imajor(inode);

	kfree(file->private_data);

	device_destroy(dump_class, MKDEV(dump_major, dump_minor));

	class_destroy(dump_class);

	complete(&dump_complete);

	return 0;
}

static ssize_t q6_dump_read(struct file *file, char __user *buf, size_t count,
		loff_t *ppos)
{
	void *buffer = NULL;
	size_t elfcore_hdrsize;
	Elf32_Phdr *phdr;
	Elf32_Ehdr *ehdr;
	int nsegments = 1;
	size_t count2 = 0;
	struct dump_file_private *dfp = (struct dump_file_private *)
		file->private_data;

	if (dump_timeout.data == -ETIMEDOUT)
		return 0;

	mod_timer(&dump_timeout, jiffies + msecs_to_jiffies(DUMP_TIMEOUT));

	if (dfp->ehdr == NULL) {
		elfcore_hdrsize = sizeof(*ehdr) + sizeof(*phdr) * nsegments;

		ehdr = kzalloc(elfcore_hdrsize, GFP_KERNEL);
		if (ehdr == NULL)
			return -ENOMEM;

		dfp->ehdr = (char *)ehdr;
		phdr = (Elf32_Phdr *)(ehdr + 1);

		memcpy(ehdr->e_ident, ELFMAG, SELFMAG);
		ehdr->e_ident[EI_CLASS] = ELFCLASS32;
		ehdr->e_ident[EI_DATA] = ELFDATA2LSB;
		ehdr->e_ident[EI_VERSION] = EV_CURRENT;
		ehdr->e_ident[EI_OSABI] = ELFOSABI_NONE;
		ehdr->e_type = ET_CORE;
		ehdr->e_machine = EM_QDSP6;
		ehdr->e_version = EV_CURRENT;
		ehdr->e_phoff = sizeof(*ehdr);
		ehdr->e_ehsize = sizeof(*ehdr);
		ehdr->e_phentsize = sizeof(*phdr);
		ehdr->e_phnum = nsegments;

		phdr->p_type = PT_LOAD;
		phdr->p_offset = elfcore_hdrsize;
		phdr->p_vaddr = phdr->p_paddr = q6dump.dump_phy_addr;
		phdr->p_filesz = phdr->p_memsz = q6dump.dump_size;
		phdr->p_flags = PF_R | PF_W | PF_X;

		dfp->ehdr_remaining_bytes = elfcore_hdrsize;
	}

	if (dfp->ehdr_remaining_bytes) {
		if (count > dfp->ehdr_remaining_bytes) {
			count2 = dfp->ehdr_remaining_bytes;
			copy_to_user(buf, dfp->ehdr + *ppos, count2);
			buf += count2;
			dfp->ehdr_remaining_bytes -= count2;
			count -= count2;
			kfree(dfp->ehdr);
		} else {
			copy_to_user(buf, dfp->ehdr + *ppos, count);
			dfp->ehdr_remaining_bytes -= count;
			if (!dfp->ehdr_remaining_bytes)
				kfree(dfp->ehdr);
			*ppos = *ppos + count;
			return count;
		}
	}

	if (count > dfp->remaining_bytes)
		count = dfp->remaining_bytes;

	if (dfp->rel_addr_off < q6dump.dump_size) {
		buffer = ioremap(q6dump.dump_phy_addr + dfp->rel_addr_off,
				count);
		if (!buffer) {
			pr_err("can not map physical address %x : %d\n",
					(unsigned int)q6dump.dump_phy_addr +
					dfp->rel_addr_off, (int)count);
			return -ENOMEM;
		}
		dfp->rel_addr_off = dfp->rel_addr_off + count;
		copy_to_user(buf, buffer, count);
	} else
		return 0;

	dfp->remaining_bytes = dfp->remaining_bytes - count;

	iounmap(buffer);

	*ppos = *ppos + count + count2;
	return count + count2;
}

static ssize_t q6_dump_write(struct file *file, const char __user *buf,
		size_t count, loff_t *ppos)
{
	return 0;
}

static const struct file_operations q6_dump_ops = {
	.open		=	q6_dump_open,
	.read		=	q6_dump_read,
	.write		=	q6_dump_write,
	.release	=       q6_dump_release,
};

static void crashdump_init(struct rproc *rproc, struct rproc_dump_segment *segment, void *dest)
{
	int ret = 0;
	int dump_major = 0;
	struct device *dump_dev = NULL;
	struct device_node *node = NULL;

	init_completion(&open_complete);
	atomic_set(&open_timedout, 0);

	dump_major = register_chrdev(UNNAMED_MAJOR, "dump", &q6_dump_ops);
	if (dump_major < 0) {
		ret = dump_major;
		pr_err("Unable to allocate a major number err = %d", ret);
		goto reg_failed;
	}

	dump_class = class_create(THIS_MODULE, "dump");
	if (IS_ERR(dump_class)) {
		ret = PTR_ERR(dump_class);
		goto class_failed;
	}

	dump_dev = device_create(dump_class, NULL, MKDEV(dump_major, 0), NULL,
			q6dump.name);
	if (IS_ERR(dump_dev)) {
		ret = PTR_ERR(dump_dev);
		pr_err("Unable to create a device err = %d", ret);
		goto device_failed;
	}

	node = of_find_node_by_name(NULL, q6dump.ss_name);
	if (node == NULL) {
		ret = -ENODEV;
		goto dump_dev_failed;
	}

	ret = of_property_read_u32_index(node, "reg", 1, (u32 *)&q6dump.dump_phy_addr);
	if (ret) {
		pr_err("could not retrieve reg property: %d\n", ret);
		goto dump_dev_failed;
	}

	ret = of_property_read_u32_index(node, "reg", 3, (u32 *)&q6dump.dump_size);
	if (ret) {
		pr_err("could not retrieve reg property: %d\n",
				ret);
		goto dump_dev_failed;
	}

	/* This avoids race condition between the scheduled timer and the opened
	 * file discriptor during delay in user space app execution.
	 */
	setup_timer(&open_timeout, open_timeout_func, 0);

	mod_timer(&open_timeout, jiffies + msecs_to_jiffies(OPEN_TIMEOUT));

	wait_for_completion(&open_complete);

	if (atomic_read(&open_timedout) == 1) {
		ret = -ETIMEDOUT;
		goto dump_dev_failed;
	}

	wait_for_completion(&dump_complete);

	if (dump_timeout.data == -ETIMEDOUT) {
		ret = dump_timeout.data;
		dump_timeout.data = 0;
	}

	del_timer_sync(&dump_timeout);
	return;

dump_dev_failed:
	device_destroy(dump_class, MKDEV(dump_major, 0));
device_failed:
	class_destroy(dump_class);
class_failed:
	unregister_chrdev(dump_major, "dump");
reg_failed:
	return;
}
#else
static void crashdump_init(struct rproc *rproc, struct rproc_dump_segment *segment, void *dest)
{
	return;
}
#endif /* CONFIG_IPQ_SS_DUMP */


static int q6v5_wcss_reset(struct q6v5_wcss *wcss)
{
	int ret;
	u32 val;
	int i;

	/* Assert resets, stop core */
	val = readl(wcss->reg_base + Q6SS_RESET_REG);
	val |= Q6SS_CORE_ARES | Q6SS_BUS_ARES_ENABLE | Q6SS_STOP_CORE;
	writel(val, wcss->reg_base + Q6SS_RESET_REG);
	/* BHS require xo cbcr to be enabled */
	val = readl(wcss->reg_base + Q6SS_XO_CBCR);
	val |= 0x1;
	writel(val, wcss->reg_base + Q6SS_XO_CBCR);

	/* Read CLKOFF bit to go low indicating CLK is enabled */
	ret = readl_poll_timeout(wcss->reg_base + Q6SS_XO_CBCR,
				 val, !(val & BIT(31)), 1,
				 HALT_CHECK_MAX_LOOPS);
	if (ret) {
		dev_err(wcss->dev,
			"xo cbcr enabling timed out (rc:%d)\n", ret);
		return ret;
	}
	/* Enable power block headswitch and wait for it to stabilize */
	val = readl(wcss->reg_base + Q6SS_PWR_CTL_REG);
	val |= Q6SS_BHS_ON;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);
	udelay(1);

	/* Put LDO in bypass mode */
	val |= Q6SS_LDO_BYP;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);

	/* Deassert Q6 compiler memory clamp */
	val = readl(wcss->reg_base + Q6SS_PWR_CTL_REG);
	val &= ~Q6SS_CLAMP_QMC_MEM;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);

	/* Deassert memory peripheral sleep and L2 memory standby */
	val |= Q6SS_L2DATA_STBY_N | Q6SS_SLP_RET_N;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);

	/* Turn on L1, L2, ETB and JU memories 1 at a time */
	val = readl(wcss->reg_base + Q6SS_MEM_PWR_CTL);
	for (i = MEM_BANKS; i >= 0; i--) {
		val |= BIT(i);
		writel(val, wcss->reg_base + Q6SS_MEM_PWR_CTL);
		/*
		 * Read back value to ensure the write is done then
		 * wait for 1us for both memory peripheral and data
		 * array to turn on.
		 */
		val |= readl(wcss->reg_base + Q6SS_MEM_PWR_CTL);
		udelay(1);
	}
	/* Remove word line clamp */
	val = readl(wcss->reg_base + Q6SS_PWR_CTL_REG);
	val &= ~Q6SS_CLAMP_WL;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);

	/* Remove IO clamp */
	val &= ~Q6SS_CLAMP_IO;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);

	/* Bring core out of reset */
	val = readl(wcss->reg_base + Q6SS_RESET_REG);
	val &= ~Q6SS_CORE_ARES;
	writel(val, wcss->reg_base + Q6SS_RESET_REG);

	/* Turn on core clock */
	val = readl(wcss->reg_base + Q6SS_GFMUX_CTL_REG);
	val |= Q6SS_CLK_ENABLE;
	writel(val, wcss->reg_base + Q6SS_GFMUX_CTL_REG);

	/* Start core execution */
	val = readl(wcss->reg_base + Q6SS_RESET_REG);
	val &= ~Q6SS_STOP_CORE;
	writel(val, wcss->reg_base + Q6SS_RESET_REG);

	return 0;
}

static int q6v5_wcss_start(struct rproc *rproc)
{
	struct q6v5_wcss *wcss = rproc->priv;
	int ret;

	ret = qcom_scm_pas_auth_and_reset(WCNSS_PAS_ID, 0, wcss->reset_cmd_id);
	if (ret) {
		dev_err(wcss->dev, "q6-wcss reset failed\n");
		return ret;
	} else {

		/* q6-wcss reset done. wait for ready interrupt */
		goto skip_reset;
	}


	qcom_q6v5_prepare(&wcss->q6v5);

	/* Release Q6 and WCSS reset */
	ret = reset_control_deassert(wcss->wcss_reset);
	if (ret) {
		dev_err(wcss->dev, "wcss_reset failed\n");
		return ret;
	}

	ret = reset_control_deassert(wcss->wcss_q6_reset);
	if (ret) {
		dev_err(wcss->dev, "wcss_q6_reset failed\n");
		goto wcss_reset;
	}

	/* Lithium configuration - clock gating and bus arbitration */
	ret = regmap_update_bits(wcss->halt_map,
				 wcss->halt_nc + TCSR_GLOBAL_CFG0,
				 TCSR_WCSS_CLK_MASK,
				 TCSR_WCSS_CLK_ENABLE);
	if (ret)
		goto wcss_q6_reset;

	ret = regmap_update_bits(wcss->halt_map,
				 wcss->halt_nc + TCSR_GLOBAL_CFG1,
				 1, 0);
	if (ret)
		goto wcss_q6_reset;

	/* Write bootaddr to EVB so that Q6WCSS will jump there after reset */
	writel(rproc->bootaddr >> 4, wcss->reg_base + Q6SS_RST_EVB);

	ret = q6v5_wcss_reset(wcss);
	if (ret)
		goto wcss_q6_reset;

skip_reset:
	ret = qcom_q6v5_wait_for_start(&wcss->q6v5, 5 * HZ);
	if (ret == -ETIMEDOUT)
		dev_err(wcss->dev, "start timed out\n");

	return ret;

wcss_q6_reset:
	reset_control_assert(wcss->wcss_q6_reset);

wcss_reset:
	reset_control_assert(wcss->wcss_reset);

	return ret;
}

static void q6v5_wcss_halt_axi_port(struct q6v5_wcss *wcss,
				    struct regmap *halt_map,
				    u32 offset)
{
	unsigned long timeout;
	unsigned int val;
	int ret;

	/* Check if we're already idle */
	ret = regmap_read(halt_map, offset + AXI_IDLE_REG, &val);
	if (!ret && val)
		return;

	/* Assert halt request */
	regmap_write(halt_map, offset + AXI_HALTREQ_REG, 1);

	/* Wait for halt */
	timeout = jiffies + msecs_to_jiffies(HALT_ACK_TIMEOUT_MS);
	for (;;) {
		ret = regmap_read(halt_map, offset + AXI_HALTACK_REG, &val);
		if (ret || val || time_after(jiffies, timeout))
			break;

		msleep(1);
	}

	ret = regmap_read(halt_map, offset + AXI_IDLE_REG, &val);
	if (ret || !val)
		dev_err(wcss->dev, "port failed halt\n");

	/* Clear halt request (port will remain halted until reset) */
	regmap_write(halt_map, offset + AXI_HALTREQ_REG, 0);
}

static int q6v5_wcss_powerdown(struct q6v5_wcss *wcss)
{
	int ret;
	u32 val;

	/* 1 - Assert WCSS/Q6 HALTREQ */
	q6v5_wcss_halt_axi_port(wcss, wcss->halt_map, wcss->halt_wcss);

	/* 2 - Enable WCSSAON_CONFIG */
	val = readl(wcss->rmb_base + SSCAON_CONFIG);
	val |= SSCAON_ENABLE;
	writel(val, wcss->rmb_base + SSCAON_CONFIG);

	/* 3 - Set SSCAON_CONFIG */
	val |= SSCAON_BUS_EN;
	val &= ~SSCAON_BUS_MUX_MASK;
	writel(val, wcss->rmb_base + SSCAON_CONFIG);

	/* 4 - SSCAON_CONFIG 1 */
	val |= BIT(1);
	writel(val, wcss->rmb_base + SSCAON_CONFIG);

	/* 5 - wait for SSCAON_STATUS */
	ret = readl_poll_timeout(wcss->rmb_base + SSCAON_STATUS,
				 val, (val & 0xffff) == 0x400, 1000,
				 HALT_CHECK_MAX_LOOPS);
	if (ret) {
		dev_err(wcss->dev,
			"can't get SSCAON_STATUS rc:%d)\n", ret);
		return ret;
	}

	/* 6 - De-assert WCSS_AON reset */
	reset_control_assert(wcss->wcss_aon_reset);

	/* 7 - Disable WCSSAON_CONFIG 13 */
	val = readl(wcss->rmb_base + SSCAON_CONFIG);
	val &= ~SSCAON_ENABLE;
	writel(val, wcss->rmb_base + SSCAON_CONFIG);

	/* 8 - De-assert WCSS/Q6 HALTREQ */
	reset_control_assert(wcss->wcss_reset);

	return 0;
}

static int q6v5_q6_powerdown(struct q6v5_wcss *wcss)
{
	int ret;
	u32 val;
	int i;

	/* 1 - Halt Q6 bus interface */
	q6v5_wcss_halt_axi_port(wcss, wcss->halt_map, wcss->halt_q6);

	/* 2 - Disable Q6 Core clock */
	val = readl(wcss->reg_base + Q6SS_GFMUX_CTL_REG);
	val &= ~Q6SS_CLK_ENABLE;
	writel(val, wcss->reg_base + Q6SS_GFMUX_CTL_REG);

	/* 3 - Clamp I/O */
	val = readl(wcss->reg_base + Q6SS_PWR_CTL_REG);
	val |= Q6SS_CLAMP_IO;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);

	/* 4 - Clamp WL */
	val |= QDSS_BHS_ON;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);

	/* 5 - Clear Erase standby */
	val &= ~Q6SS_L2DATA_STBY_N;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);

	/* 6 - Clear Sleep RTN */
	val &= ~Q6SS_SLP_RET_N;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);

	/* 7 - turn off Q6 memory foot/head switch one bank at a time */
	for (i = 0; i < 20; i++) {
		val = readl(wcss->reg_base + Q6SS_MEM_PWR_CTL);
		val &= ~BIT(i);
		writel(val, wcss->reg_base + Q6SS_MEM_PWR_CTL);
		mdelay(1);
	}

	/* 8 - Assert QMC memory RTN */
	val = readl(wcss->reg_base + Q6SS_PWR_CTL_REG);
	val |= Q6SS_CLAMP_QMC_MEM;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);

	/* 9 - Turn off BHS */
	val &= ~Q6SS_BHS_ON;
	writel(val, wcss->reg_base + Q6SS_PWR_CTL_REG);
	udelay(1);

	/* 10 - Wait till BHS Reset is done */
	ret = readl_poll_timeout(wcss->reg_base + Q6SS_BHS_STATUS,
				 val, !(val & BHS_EN_REST_ACK), 1000,
				 HALT_CHECK_MAX_LOOPS);
	if (ret) {
		dev_err(wcss->dev, "BHS_STATUS not OFF (rc:%d)\n", ret);
		return ret;
	}

	/* 11 -  Assert WCSS reset */
	reset_control_assert(wcss->wcss_reset);

	/* 12 - Assert Q6 reset */
	reset_control_assert(wcss->wcss_q6_reset);

	return 0;
}

static int q6v5_wcss_stop(struct rproc *rproc)
{
	struct q6v5_wcss *wcss = rproc->priv;
	int ret;

	ret = qcom_scm_pas_shutdown(WCNSS_PAS_ID);
	if (ret) {
		dev_err(wcss->dev, "not able to shutdown\n");
		return ret;
	} else {
		qcom_q6v5_unprepare(&wcss->q6v5);
		return ret;
	}

	/* WCSS powerdown */
	ret = qcom_q6v5_request_stop(&wcss->q6v5);
	if (ret == -ETIMEDOUT) {
		dev_err(wcss->dev, "timed out on wait\n");
		return ret;
	}

	ret = q6v5_wcss_powerdown(wcss);
	if (ret)
		return ret;

	/* Q6 Power down */
	ret = q6v5_q6_powerdown(wcss);
	if (ret)
		return ret;

	qcom_q6v5_unprepare(&wcss->q6v5);

	return 0;
}

static void *q6v5_wcss_da_to_va(struct rproc *rproc, u64 da, int len)
{
	struct q6v5_wcss *wcss = rproc->priv;
	int offset;

	offset = da - wcss->mem_reloc;
	if (offset < 0 || offset + len > wcss->mem_size)
		return NULL;

	return wcss->mem_region + offset;
}

static int q6v5_wcss_load(struct rproc *rproc, const struct firmware *fw)
{
	struct q6v5_wcss *wcss = rproc->priv;
	const struct firmware *m3_fw;
	int ret;

	ret = request_firmware(&m3_fw, "IPQ8074/m3_fw.mdt", wcss->dev);
	if (ret) {
		dev_info(wcss->dev, "skipping firmware %s\n", "m3_fw.mdt");
		goto skip_m3;
	}

	ret = qcom_mdt_load_no_init(wcss->dev, m3_fw, "IPQ8074/m3_fw.mdt", 0,
				    wcss->mem_region, wcss->mem_phys,
				    wcss->mem_size, &wcss->mem_reloc);
	if (ret) {
		dev_err(wcss->dev, "can't load %s\n", "m3_fw.bXX");
		return ret;
	}

skip_m3:
	return qcom_mdt_load(wcss->dev, fw, rproc->firmware,
			     WCNSS_PAS_ID, wcss->mem_region, wcss->mem_phys,
			     wcss->mem_size, &wcss->mem_reloc);
}

static const struct rproc_ops q6v5_wcss_ops = {
	.start = q6v5_wcss_start,
	.stop = q6v5_wcss_stop,
	.da_to_va = q6v5_wcss_da_to_va,
	.load = q6v5_wcss_load,
	.get_boot_addr = rproc_elf_get_boot_addr,
};

static int q6v5_wcss_init_reset(struct q6v5_wcss *wcss)
{
	struct device *dev = wcss->dev;

	wcss->wcss_aon_reset = devm_reset_control_get(dev, "wcss_aon_reset");
	if (IS_ERR(wcss->wcss_aon_reset)) {
		dev_err(wcss->dev, "unable to acquire wcss_aon_reset\n");
		return PTR_ERR(wcss->wcss_aon_reset);
	}

	wcss->wcss_reset = devm_reset_control_get(dev, "wcss_reset");
	if (IS_ERR(wcss->wcss_reset)) {
		dev_err(wcss->dev, "unable to acquire wcss_reset\n");
		return PTR_ERR(wcss->wcss_reset);
	}

	wcss->wcss_q6_reset = devm_reset_control_get(dev, "wcss_q6_reset");
	if (IS_ERR(wcss->wcss_q6_reset)) {
		dev_err(wcss->dev, "unable to acquire wcss_q6_reset\n");
		return PTR_ERR(wcss->wcss_q6_reset);
	}

	return 0;
}

static int q6v5_wcss_init_mmio(struct q6v5_wcss *wcss,
			       struct platform_device *pdev)
{
	struct of_phandle_args args;
	struct resource *res;
	int ret;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "qdsp6");
	wcss->reg_base = ioremap(res->start, resource_size(res));
	if (IS_ERR(wcss->reg_base))
		return PTR_ERR(wcss->reg_base);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "rmb");
	wcss->rmb_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(wcss->rmb_base))
		return PTR_ERR(wcss->rmb_base);

	ret = of_parse_phandle_with_fixed_args(pdev->dev.of_node,
					       "qcom,halt-regs", 3, 0, &args);
	if (ret < 0) {
		dev_err(&pdev->dev, "failed to parse qcom,halt-regs\n");
		return -EINVAL;
	}

	wcss->halt_map = syscon_node_to_regmap(args.np);
	of_node_put(args.np);
	if (IS_ERR(wcss->halt_map))
		return PTR_ERR(wcss->halt_map);

	wcss->halt_q6 = args.args[0];
	wcss->halt_wcss = args.args[1];
	wcss->halt_nc = args.args[2];

	return 0;
}

static int q6v5_alloc_memory_region(struct q6v5_wcss *wcss)
{
	struct reserved_mem *rmem = NULL;
	struct device_node *node;
	struct device *dev = wcss->dev;

	node = of_parse_phandle(dev->of_node, "memory-region", 0);
	if (node)
		rmem = of_reserved_mem_lookup(node);
	of_node_put(node);

	if (!rmem) {
		dev_err(dev, "unable to acquire memory-region\n");
		return -EINVAL;
	}

	wcss->mem_phys = rmem->base;
	wcss->mem_reloc = rmem->base;
	wcss->mem_size = rmem->size;
	wcss->mem_region = devm_ioremap_wc(dev, wcss->mem_phys, wcss->mem_size);
	if (!wcss->mem_region) {
		dev_err(dev, "unable to map memory region: %pa+%pa\n",
			&rmem->base, &rmem->size);
		return -EBUSY;
	}

	return 0;
}

static int q6v5_wcss_probe(struct platform_device *pdev)
{
	struct q6v5_wcss *wcss;
	struct rproc *rproc;
	int ret;

	rproc = rproc_alloc(&pdev->dev, pdev->name, &q6v5_wcss_ops,
			    "IPQ8074/q6_fw.mdt", sizeof(*wcss));
	if (!rproc) {
		dev_err(&pdev->dev, "failed to allocate rproc\n");
		return -ENOMEM;
	}

	wcss = rproc->priv;
	wcss->dev = &pdev->dev;

	ret = q6v5_wcss_init_mmio(wcss, pdev);
	if (ret)
		goto free_rproc;

	ret = q6v5_alloc_memory_region(wcss);
	if (ret)
		goto free_rproc;

	ret = q6v5_wcss_init_reset(wcss);
	if (ret)
		goto free_rproc;

	ret = qcom_q6v5_init(&wcss->q6v5, pdev, rproc, WCSS_CRASH_REASON, NULL);
	if (ret)
		goto free_rproc;

	rproc->auto_boot = false;
	ret = rproc_add(rproc);
	if (ret)
		goto free_rproc;

	/*
	 * Registering custom coredump function with a dummy dump segment as the
	 * dump regions are taken care by the dump function itself
	 */
	ret = rproc_coredump_add_custom_segment(rproc, 0, 0, crashdump_init, NULL);
	if (ret)
		goto free_rproc;

	ret = of_property_read_u32(pdev->dev.of_node, "qca,sec-reset-cmd",
				   &wcss->reset_cmd_id);
	if (ret)
		wcss->reset_cmd_id = QCOM_SCM_PAS_AUTH_DEBUG_RESET_CMD;

	qcom_add_glink_subdev(rproc, &wcss->glink_subdev);
	qcom_add_ssr_subdev(rproc, &wcss->ssr_subdev, "rproc");
	platform_set_drvdata(pdev, rproc);

	return 0;

free_rproc:
	rproc_free(rproc);

	return ret;
}

static int q6v5_wcss_remove(struct platform_device *pdev)
{
	struct rproc *rproc = platform_get_drvdata(pdev);
	struct q6v5_wcss *wcss;

	wcss = rproc->priv;
	wcss->dev = &pdev->dev;

	rproc_del(rproc);
	qcom_remove_glink_subdev(rproc, &wcss->glink_subdev);
	qcom_remove_ssr_subdev(rproc, &wcss->ssr_subdev);
	rproc_free(rproc);

	return 0;
}

static const struct of_device_id q6v5_wcss_of_match[] = {
	{ .compatible = "qcom,ipq8074-wcss-pil" },
	{ },
};
MODULE_DEVICE_TABLE(of, q6v5_wcss_of_match);

static struct platform_driver q6v5_wcss_driver = {
	.probe = q6v5_wcss_probe,
	.remove = q6v5_wcss_remove,
	.driver = {
		.name = "qcom-q6v5-wcss-pil",
		.of_match_table = q6v5_wcss_of_match,
	},
};
module_platform_driver(q6v5_wcss_driver);

MODULE_DESCRIPTION("Hexagon WCSS Peripheral Image Loader");
MODULE_LICENSE("GPL v2");
