// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2020 Intel Corporation.
 * Ayyappan Rathinam <ayyappan.rathinamX@intel.com>
 */
#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <uapi/linux/icc/drv_icc.h>

/* DEFINEs */
#define tep_reg_read(paddr)		readl((void *)KSEG1ADDR(paddr))
#define tep_reg_writel(reg, paddr)	writel(reg, (void *)KSEG1ADDR(paddr))
#define UPDATE_BITS(r, m, v)		(((r) & (~(m))) | ((v) & (m)))
#define VALIDATE_ADDR(A)\
	((((A) >= RCU_BASE) && ((A) <= RCU_BASE + RCU_OFF)) ||\
	(((A) >= CHIPTOP_BASE) && ((A) <= CHIPTOP_BASE + CHIPTOP_OFF)) ||\
	(((A) >= CGU_BASE) && ((A) <= CGU_BASE + CGU_OFF)) ||\
	(((A) >= GPHYCDB_BASE) && ((A) <= GPHYCDB_BASE + GPHYCDB_OFF)))

/* SECURE REGISTERs BASE */
#define RCU_BASE		0x16100000
#define RCU_OFF			0x80000
#define CHIPTOP_BASE		0x16180000
#define CHIPTOP_OFF		0x1000
#define CGU_BASE		0x16200000
#define CGU_OFF			0x810
#define GPHYCDB_BASE		0x16210080
#define GPHYCDB_OFF		0x90

/* RCU Register definition */
#define RST_REQ			0x10
#define RST_STAT		0x14
#define RST_STAT2		0x24
#define RST_REQ2		0x48
#define INTERAPTIV_WDT_RST_EN	0x50

/* CHIPTOP Register definition */
#define RST_BOOT_INFO		0x0
#define PAD_POWER_SUPPLY_SEL	0x4
#define TOP_LS_CTRL		0x8
#define GP_STRAP		0x1C
#define PPM			0x28
#define ENDIAN			0x4C
#define NGI_ENDIAN_IA		0x5C
#define PVT_SENSOR_0		0x100
#define PVT_SENSOR_DATA_0	0x104
#define PVT_SENSOR_TBITS_0	0x108
#define PVT_SENSOR_1		0x110
#define PVT_SENSOR_DATA_1	0x114
#define PVT_SENSOR_TBITS_1	0x118
#define ROM_EJTAG		0x124
#define OEM_C			0x128
#define SPE_DBG_OUTPUT		0x12C
#define ANTI_RLBK_VER_0		0x130
#define ANTI_RLBK_VER_1		0x134
#define SCRM_KEY_WR		0x138
#define XBAR0_BASE		0x200
#define XBAR1_BASE		0x204
#define XBAR2_BASE		0x208
#define XBAR4_BASE		0x210

/* CGU Register definition */
#define PLL0A_CFG0		0x4
#define PLL0A_CFG1		0x8
#define PLL0A_CFG2		0xC
#define PLL0B_CFG0		0x24
#define PLL0B_CFG1		0x28
#define PLL0B_CFG2		0x2C
#define PLL2_CFG0		0x64
#define PLL2_CFG1		0x68
#define PLL2_CFG2		0x6C
#define CGU_CLKGCR0_B		0x11C
#define CGU_CLKGCR1_B		0x128
#define XO_CFG			0x400
#define GPHY_FCR		0x800

enum {
	REG_ACCESS_ONCE = 1,
	REG_ACCESS_SELECTIVE = 2,
	REG_ACCESS_PROTECTED = UINT_MAX,
} REG_SECURE;

struct secure_regs {
	phys_addr_t paddr;	/* Register physical address */
	u32 mode;		/* Depends on the REG_SECURE enum */
	u32 m_once;		/* R/W selective bits only once */
	u32 mask;		/* R/W selective bits multiple times */
};

/* Read protected register list */
struct secure_regs gsec_r[] = {
	{CHIPTOP_BASE + PAD_POWER_SUPPLY_SEL, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + TOP_LS_CTRL, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + PPM, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + ENDIAN, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + NGI_ENDIAN_IA, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + PVT_SENSOR_TBITS_0, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + PVT_SENSOR_TBITS_1, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + ROM_EJTAG, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + SPE_DBG_OUTPUT, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + SCRM_KEY_WR, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + XBAR0_BASE, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + XBAR1_BASE, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + XBAR2_BASE, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + XBAR4_BASE, REG_ACCESS_PROTECTED, 0, 0},
	{CGU_BASE + CGU_CLKGCR0_B, REG_ACCESS_PROTECTED, 0, 0},
};

/* write protected register list */
struct secure_regs gsec_w[] = {
	{RCU_BASE + RST_REQ, REG_ACCESS_SELECTIVE, 0, 0xEFFFFFFD},
	{RCU_BASE + RST_STAT, REG_ACCESS_PROTECTED, 0, 0},
	{RCU_BASE + RST_STAT2, REG_ACCESS_PROTECTED, 0, 0},
	{RCU_BASE + RST_REQ2, REG_ACCESS_SELECTIVE, 0, 0x9E4FFFF1},
	{RCU_BASE + INTERAPTIV_WDT_RST_EN, REG_ACCESS_SELECTIVE, 0, 0xFFFFFEFF},
	{CHIPTOP_BASE + RST_BOOT_INFO, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + PAD_POWER_SUPPLY_SEL, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + TOP_LS_CTRL, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + GP_STRAP, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + PPM, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + ENDIAN, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + NGI_ENDIAN_IA, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + PVT_SENSOR_0, (REG_ACCESS_ONCE | REG_ACCESS_SELECTIVE),
					0x3FF0000, 0xFC00FFFF},
	{CHIPTOP_BASE + PVT_SENSOR_DATA_0, (REG_ACCESS_ONCE | REG_ACCESS_SELECTIVE),
					0x3FF0000, 0xFC00FFFF},
	{CHIPTOP_BASE + PVT_SENSOR_TBITS_0, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + PVT_SENSOR_1, (REG_ACCESS_ONCE | REG_ACCESS_SELECTIVE),
					0x3FF0000, 0xFC00FFFF},
	{CHIPTOP_BASE + PVT_SENSOR_DATA_1, (REG_ACCESS_ONCE | REG_ACCESS_SELECTIVE),
					0x3FF0000, 0xFC00FFFF},
	{CHIPTOP_BASE + PVT_SENSOR_TBITS_1, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + ROM_EJTAG, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + OEM_C, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + SPE_DBG_OUTPUT, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + ANTI_RLBK_VER_0, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + ANTI_RLBK_VER_1, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + SCRM_KEY_WR, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + XBAR0_BASE, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + XBAR1_BASE, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + XBAR2_BASE, REG_ACCESS_PROTECTED, 0, 0},
	{CHIPTOP_BASE + XBAR4_BASE, REG_ACCESS_PROTECTED, 0, 0},
	{CGU_BASE + PLL0A_CFG0, REG_ACCESS_ONCE, UINT_MAX, 0},
	{CGU_BASE + PLL0A_CFG1, (REG_ACCESS_ONCE | REG_ACCESS_SELECTIVE),
							0xFFFFFFF0, 0xF},
	{CGU_BASE + PLL0A_CFG2, REG_ACCESS_ONCE, UINT_MAX, 0},
	{CGU_BASE + PLL0B_CFG0, REG_ACCESS_ONCE, UINT_MAX, 0},
	{CGU_BASE + PLL0B_CFG1, REG_ACCESS_ONCE, UINT_MAX, 0},
	{CGU_BASE + PLL0B_CFG2, REG_ACCESS_ONCE, UINT_MAX, 0},
	{CGU_BASE + PLL2_CFG0, REG_ACCESS_PROTECTED, UINT_MAX, 0},
	{CGU_BASE + PLL2_CFG1, REG_ACCESS_PROTECTED, UINT_MAX, 0},
	{CGU_BASE + PLL2_CFG2, REG_ACCESS_PROTECTED, UINT_MAX, 0},
	{CGU_BASE + CGU_CLKGCR0_B, REG_ACCESS_PROTECTED, 0, 0},
	{CGU_BASE + CGU_CLKGCR1_B, REG_ACCESS_SELECTIVE, 0, 0x7F9FBFFF},
	{CGU_BASE + XO_CFG, REG_ACCESS_PROTECTED, 0, 0},
	{CGU_BASE + GPHY_FCR, REG_ACCESS_ONCE, UINT_MAX, 0},
};

/* global variable for clock divider valid list */
static u32 g_cf_table;

/* validate cpu freq divider values */
static bool validate_freq_divider(u32 val)
{
	u32 table = g_cf_table;

	if (g_cf_table == 0)
		return true;

	while (table) {
		if ((table & 0xF) == val)
			return true;
		table = table >> 4;
	}
	return false;
}

static int tep_sec_readl(phys_addr_t paddr, u32 *val)
{
	int idx;
	size_t size = ARRAY_SIZE(gsec_r);
	u32 reg;

	*val = 0;

	if (!VALIDATE_ADDR(paddr)) {
		pr_err("!!! ===%s: paddr 0x%x out of range !!!\n",
		       __func__, paddr);
		return -EINVAL;
	}

	reg = tep_reg_read(paddr);

	/* Find the secured reg index */
	for (idx = 0; idx < size; idx++) {
		if (gsec_r[idx].paddr == paddr)
			break;
	}

	if ((idx >= 0) && (idx < size)) {
		if (gsec_r[idx].mode == REG_ACCESS_PROTECTED) {
			pr_err("!!! ===%s:reg 0x%x is secured !!!\n",
			       __func__, gsec_r[idx].paddr);
			return -EPERM;
		} else if (gsec_r[idx].mode & REG_ACCESS_ONCE) {
			if (gsec_r[idx].m_once) {
				*val = reg & gsec_r[idx].m_once;
				gsec_r[idx].mode &= (~REG_ACCESS_ONCE);
				gsec_r[idx].mask &= (~gsec_r[idx].m_once);
			} else {
				pr_err("===%s:reg 0x%x ONCE mask %u ?\n",
				       __func__, gsec_r[idx].paddr,
				       gsec_r[idx].m_once);
			}

			if ((gsec_r[idx].mode & REG_ACCESS_SELECTIVE) &&
			    (gsec_r[idx].mask)) {
				*val |= reg & gsec_r[idx].mask;
			} else {
				gsec_r[idx].mode = REG_ACCESS_PROTECTED;
			}
		} else if (gsec_r[idx].mode & REG_ACCESS_SELECTIVE) {
			if (gsec_r[idx].mask) {
				*val = reg & gsec_r[idx].mask;
			} else {
				pr_err("===%s:reg 0x%x SELECTIVE mask %u ?\n",
				       __func__, gsec_r[idx].paddr,
				       gsec_r[idx].mask);
			}
		}
	} else {
		*val = reg;
	}

	return 0;
}

static int tep_sec_writel(phys_addr_t paddr, u32 val)
{
	int idx;
	size_t size = ARRAY_SIZE(gsec_w);
	u32 reg;

	if (!VALIDATE_ADDR(paddr)) {
		pr_err("!!! ===%s: paddr 0x%x out of range !!!\n",
		       __func__, paddr);
		return -EINVAL;
	}

	/* Find the secured reg index */
	for (idx = 0; idx < size; idx++) {
		if (gsec_w[idx].paddr == paddr)
			break;
	}

	/* Validate the values if REG is PLL0A_CFG1 */
	if (gsec_w[idx].paddr == (CGU_BASE + PLL0A_CFG1)) {
		if (validate_freq_divider(val & 0xF) == false) {
			pr_err("!!! ===%s: paddr 0x%x out of range !!!\n",
			       __func__, paddr);
			return -EINVAL;
		}
	}

	if ((idx >= 0) && (idx < size)) {
		reg = tep_reg_read(paddr);
		if (gsec_w[idx].mode == REG_ACCESS_PROTECTED) {
			pr_err("!!! ===%s:reg 0x%x is secured !!!\n",
			       __func__, gsec_w[idx].paddr);
			return -EPERM;
		} else if (gsec_w[idx].mode & REG_ACCESS_ONCE) {
			if (gsec_w[idx].m_once) {
				reg = UPDATE_BITS(reg, gsec_w[idx].m_once, val);
				gsec_w[idx].mode &= (~REG_ACCESS_ONCE);
				gsec_w[idx].mask &= (~gsec_w[idx].m_once);
				gsec_w[idx].m_once = 0;
			} else {
				pr_err("===%s:reg 0x%x ONCE mask %u ?\n",
				       __func__, gsec_r[idx].paddr,
				       gsec_r[idx].m_once);
			}

			if ((gsec_w[idx].mode & REG_ACCESS_SELECTIVE) &&
			    (gsec_w[idx].mask)) {
				reg |= UPDATE_BITS(reg, gsec_w[idx].mask, val);
			} else {
				gsec_w[idx].mode = REG_ACCESS_PROTECTED;
			}
		} else if (gsec_w[idx].mode & REG_ACCESS_SELECTIVE) {
			if (gsec_w[idx].mask) {
				reg = UPDATE_BITS(reg, gsec_w[idx].mask, val);
			} else {
				pr_err("===%s:reg 0x%x SELECTIVE mask %u ?\n",
				       __func__, gsec_w[idx].paddr,
				       gsec_w[idx].mask);
			}
		}
	} else {
		reg = val;
	}
	tep_reg_writel(reg, paddr);
	return 0;
}

static void process_regmap_message(icc_devices client_id)
{
	int ret;
	icc_msg_t rw;
	icc_msg_t icc_reply_msg;
	u32 addr, val;

	memset(&rw, 0, sizeof(icc_msg_t));
	ret = icc_read(client_id, &rw);

	if (ret > 0) {
		memset(&icc_reply_msg, 0, sizeof(icc_msg_t));
		icc_reply_msg.src_client_id = rw.dst_client_id;
		icc_reply_msg.dst_client_id = rw.src_client_id;
		icc_reply_msg.param[0] = rw.param[0];
		icc_reply_msg.param[1] = rw.param[1];
		addr = icc_reply_msg.param[0];
		val = icc_reply_msg.param[1];

		switch (rw.msg_id) {
		case REGMAP_WR_MSGID:
			tep_sec_writel((phys_addr_t)addr, val);
			break;

		case REGMAP_RD_MSGID:
			val = 0;
			tep_sec_readl((phys_addr_t)addr, &val);
			icc_reply_msg.param[1] = val;
			break;

		case REGMAP_CF_MSGID:
			/* For clock frequency table from IAP */
			/* No response required , so return */
			/* store the bitfield in global variable */
			g_cf_table = rw.param[0];
			return;

		default:
			icc_reply_msg.param[2] = 1;
			break;
		}
		icc_write(client_id, &icc_reply_msg);
	} else {
		pr_err("Read from ICC REGMAP failed\n");
	}
}

static void pfn_icc_regmap_callback_cgu(icc_wake_type wake_type)
{
	if ((wake_type & ICC_READ))
		process_regmap_message(REGMAP_CGU);
}

static void pfn_icc_regmap_callback_rcu(icc_wake_type wake_type)
{
	if ((wake_type & ICC_READ))
		process_regmap_message(REGMAP_RCU);
}

static void pfn_icc_regmap_callback_ct(icc_wake_type wake_type)
{
	if ((wake_type & ICC_READ))
		process_regmap_message(REGMAP_CT);
}

int __init init_icc_regmap(void)
{
	int result_cgu, result_rcu, result_ct;

	result_cgu = icc_open((struct inode *)REGMAP_CGU, NULL);
	result_rcu = icc_open((struct inode *)REGMAP_RCU, NULL);
	result_ct = icc_open((struct inode *)REGMAP_CT, NULL);

	if (result_cgu == 0)
		result_cgu = icc_register_callback(REGMAP_CGU,
						&pfn_icc_regmap_callback_cgu);
	if (result_rcu == 0)
		result_rcu = icc_register_callback(REGMAP_RCU,
						&pfn_icc_regmap_callback_rcu);
	if (result_ct == 0)
		result_ct = icc_register_callback(REGMAP_CT,
						&pfn_icc_regmap_callback_ct);
	if (result_cgu < 0) {
		pr_err("%s: icc_register_callback failed for cgu\n",
		       __func__);
		icc_unregister_callback(REGMAP_CGU);
		icc_close((struct inode *)REGMAP_CGU, NULL);
		return -EBUSY;
	}
	if (result_rcu < 0) {
		pr_err("%s: icc_register_callback failed for rcu\n", __func__);
		icc_unregister_callback(REGMAP_RCU);
		icc_close((struct inode *)REGMAP_RCU, NULL);
		return -EBUSY;
	}
	if (result_ct < 0) {
		pr_err("%s: icc_register_callback failed for chip top\n",
		       __func__);
		icc_unregister_callback(REGMAP_CT);
		icc_close((struct inode *)REGMAP_CT, NULL);
		return -EBUSY;
	}
	return 0;
}
module_init(init_icc_regmap);
MODULE_LICENSE("GPL v2");
