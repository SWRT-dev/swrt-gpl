// SPDX-License-Identifier: GPL-2.0
/*
 * PCIe host controller driver for Intel AXI PCIe Bridge
 *
 * Copyright (C) 2018 Intel Corporation.
 *
 * Based on the Xilinx PCIe driver
 */
#include <linux/clk.h>
#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/iopoll.h>
#ifdef CONFIG_PCIE_INTEL_MSI_PIC
#include <linux/msi.h>
#endif
#include <linux/mfd/syscon.h>
#include <linux/of_irq.h>
#include <linux/of_pci.h>
#include <linux/of_platform.h>
#include <linux/pci.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/regmap.h>
#include <linux/reset.h>

/* PCIe RC Configuration Register */
#define PCIE_VDID				0x00

/* Bit definition from pci_reg.h */
#define PCIE_PCICMDSTS				BIT(2)
#define PCIE_CCRID				BIT(3)

#define PCIE_LCAP				0x7C
#define PCIE_LCAP_MAX_LINK_SPEED		GENMASK(3, 0)
#define PCIE_LCAP_MAX_LINK_SPEED_S		0
#define PCIE_LCAP_MAX_LENGTH_WIDTH		GENMASK(9, 4)
#define PCIE_LCAP_MAX_LENGTH_WIDTH_S		4
#define PCIE_LCAP_L0S				BIT(10)

/* Link Control and Status Register */
#define PCIE_LCTLSTS				0x80
#define PCIE_LCTLSTS_ASPM_ENABLE		GENMASK(1, 0)
#define PCIE_LCTLSTS_ASPM_ENABLE_S		0
#define PCIE_LCTLSTS_RCB128			BIT(3)
#define PCIE_LCTLSTS_LINK_DISABLE		BIT(4)
#define PCIE_LCTLSTS_RETRIAN_LINK		BIT(5)
#define PCIE_LCTLSTS_COM_CLK_CFG		BIT(6)
#define PCIE_LCTLSTS_EXT_SYNC			BIT(7)

#define PCIE_LCTLSTS_CLK_PM			BIT(8)
#define PCIE_LCTLSTS_HW_AW_DIS			BIT(9)
#define PCIE_LCTLSTS_LINK_ABM_IRQ_EN		BIT(10)
#define PCIE_LCTLSTS_LINK_AB_IRQ_EN		BIT(11)

#define PCIE_LCTLSTS_LINK_SPEED			GENMASK(19, 16)
#define PCIE_LCTLSTS_LINK_SPEED_S		16
#define PCIE_LCTLSTS_NEGOTIATED_LINK_WIDTH	GENMASK(25, 20)
#define PCIE_LCTLSTS_NEGOTIATED_LINK_WIDTH_S	20
#define PCIE_LCTLSTS_RETRAIN_PENDING		BIT(27)
#define PCIE_LCTLSTS_SLOT_CLK_CFG		BIT(28)
#define PCIE_LCTLSTS_DLL_ACTIVE			BIT(29)

#define PCIE_LCTLSTS_LINK_BMS			BIT(30)
#define PCIE_LCTLSTS_LINK_ABS			BIT(31)

#define PCIE_LCTLSTS2				0xA0

#define PCIE_LCTLSTS2_TGT_LINK_SPEED		GENMASK(3, 0)
#define PCIE_LCTLSTS2_TGT_LINK_SPEED_S		0
#define PCIE_LCTLSTS2_TGT_LINK_SPEED_25GT	0x1
#define PCIE_LCTLSTS2_TGT_LINK_SPEED_5GT	0x2
#define PCIE_LCTLSTS2_TGT_LINK_SPEED_8GT	0x3
#define PCIE_LCTLSTS2_TGT_LINK_SPEED_16GT	0x4
#define PCIE_LCTLSTS2_ENTER_COMPLIACE		BIT(4)
#define PCIE_LCTLSTS2_HW_AUTO_DIS		BIT(5)

enum {
	PCIE_LINK_SPEED_AUTO = 0,
	PCIE_LINK_SPEED_GEN1,
	PCIE_LINK_SPEED_GEN2,
	PCIE_LINK_SPEED_GEN3,
	PCIE_LINK_SPEED_GEN4,
};

/* PCIe Gen3 EQ, refer to spec page 776 */
#define PCIE_XCAP2_HDR				0x158
#define PCIE_LINK_CTL3				0x15C
#define PCIE_LANE_ESR				0x160
#define PCIE_LANE_EQ_CTRL			0x164

/* Ack Frequency Register */
#define PCIE_AFR				0x70C

#define PCIE_AFR_AF				GENMASK(7, 0)
#define PCIE_AFR_AF_S				0
#define PCIE_AFR_FTS_NUM			GENMASK(15, 8)
#define PCIE_AFR_FTS_NUM_S			8
#define PCIE_AFR_COM_FTS_NUM			GENMASK(23, 16)
#define PCIE_AFR_COM_FTS_NUM_S			16
#define PCIE_AFR_GEN12_FTS_NUM_DEFAULT		127
#define PCIE_AFR_GEN3_FTS_NUM_DEFAULT		180
#define PCIE_AFR_GEN4_FTS_NUM_DEFAULT		196
#define PCIE_AFR_L0S_ENTRY_LATENCY_DEFAULT	7
#define PCIE_AFR_L1_ENTRY_LATENCY_DEFAULT	5

/* Port Link Control Register */
#define PCIE_PLCR				0x710

#define PCIE_PLCR_OTHER_MSG_REQ			BIT(0)
#define PCIE_PLCR_SCRAMBLE_DISABLE		BIT(1)
#define PCIE_PLCR_LOOPBACK_EN			BIT(2)
#define PCIE_PLCR_LTSSM_HOT_RST			BIT(3)
#define PCIE_PLCR_DLL_LINK_EN			BIT(5)
#define PCIE_PLCR_FAST_LINK_SIM_EN		BIT(7)
#define PCIE_PLCR_LINK_MODE			GENMASK(21, 16)
#define PCIE_PLCR_LINK_MODE_S			16
#define PCIE_PLCR_CORRUPTED_CRC_EN		BIT(25)

/* Multiple Lane support */
#define PCIE_PORT_LINK_MODE_1_LANES		(0x1 << 16)
#define PCIE_PORT_LINK_MODE_2_LANES		(0x3 << 16)
#define PCIE_PORT_LINK_MODE_4_LANES		(0x7 << 16)
#define PCIE_PORT_LINK_MODE_8_LANES		(0xf << 16)

/* Debug Register 1 */
#define PCIE_DBR1				0x72C
#define PCIE_DBR1_PHY_IN_TRAINING		BIT(29)
#define PCIE_DBR1_PHY_LINK_UP			BIT(4)

#define PCIE_LINK_WIDTH_SPEED_CONTROL		0x80C
#define PCIE_PORT_LOGIC_FTS			GENMASK(7, 0)
#define PCIE_PORT_LOGIC_FTS_S			0
#define PCIE_PORT_LOGIC_DEFAULT_FTS_NUM		127
#define PCIE_PORT_LOGIC_LINK_WIDTH_MASK		GENMASK(16, 8)
#define PCIE_PORT_LOGIC_LINK_WIDTH_MASK_S	8
#define PCIE_PORT_LOGIC_SPEED_CHANGE		BIT(17)

#define PCIE_PORT_LOGIC_LINK_WIDTH_1_LANES	(0x1 << 8)
#define PCIE_PORT_LOGIC_LINK_WIDTH_2_LANES	(0x2 << 8)
#define PCIE_PORT_LOGIC_LINK_WIDTH_4_LANES	(0x4 << 8)
#define PCIE_PORT_LOGIC_LINK_WIDTH_8_LANES	(0x8 << 8)

#define PCIE_GEN3_REL_CTRL			0x890
#define PCIE_RXREQ_RGRDLESS_RXTS		BIT(13)
#define PCIE_RXEQ_PH01_EN			BIT(12)
#define PCIE_RXEQ_ADAPT_EN			(PCIE_RXREQ_RGRDLESS_RXTS |\
						 PCIE_RXEQ_PH01_EN)
#define PCIE_GEN3_EQ_CTRL			0x8A8
#define PCIE_GEN3_EQ_DIR_CHG			0x8AC

#define PCIE_PIPE_LB				0x8B8
#define PCIE_PIPE_LPK_EN			BIT(31)
#define PCIE_MISC_CTRL				0x8BC
#define PCIE_MISC_CTRL_DBI_RO_WR_EN		BIT(0)

#define PCIE_MULTI_LANE_CTRL			0x8C0
#define PCIE_UPCONFIG_SUPPORT			BIT(5)
#define PCIE_DIRECT_LINK_WIDTH_CHANGE		BIT(6)
#define PCIE_TARGET_LINK_WIDTH			GENMASK(5, 0)
#define PCIE_TARGET_LINK_WIDTH_S		0
#define PCIE_TARGET_LINK_WIDTH_1_LANES		BIT(0)
#define PCIE_TARGET_LINK_WIDTH_2_LANES		BIT(1)
#define PCIE_TARGET_LINK_WIDTH_4_LANES		BIT(2)
#define PCIE_TARGET_LINK_WIDTH_8_LANES		BIT(3)

#define PCIE_IOP_CTRL				0x8C4
#define PCIE_IOP_RX_STANDBY_CTRL		GENMASK(6, 0)
#define PCIE_IOP_L1SUB_EXIT_MODE		BIT(8)
#define PCIE_IOP_L1_NOWAIT_P1			BIT(9)
#define PCIE_IOP_L1_CLK_SEL			BIT(10)

/* APP RC Core Control Register */
#define PCIE_RC_CCR				0x10

/* This should be enabled after initializing configuratin registers
 * Also should check link status retraining bit
 */
/* Enable LTSSM to continue link establishment */
#define PCIE_RC_CCR_LTSSM_ENABLE		BIT(0)
#define PCIE_DEVICE_TYPE			GENMASK(7, 4)
#define PCIE_DEVICE_TYPE_S			4
#define PCIE_RC_CCR_RC_MODE			0x04
#define PCIE_RC_CCR_EP_MODE			0

enum {
	PCIE_RC_MODE,
	PCIE_EP_MODE,
};

/* RC Core Debug Register */
#define PCIE_RC_DR				0x14

#define PCIE_RC_DR_DLL_UP			BIT(0)

/* PHY Link Status Register */
#define PCIE_PHY_SR				0x18

#define PCIE_PHY_SR_PHY_LINK_UP			BIT(0)

/* PCIe Message Control */
#define PCIE_MSG_CR				0x30

#define PCIE_XMT_PM_TURNOFF			BIT(0)
#define PCIE_XMT_PME				BIT(3)

/* PCIe Power Management Control */
#define PCIE_PMC				0x44
#define PCIE_PM_IN_L0S				BIT(18)
#define PCIE_PM_IN_L1				BIT(19)
#define PCIE_PM_IN_L2				BIT(20)
#define PCIE_PM_IN_L1SUB			BIT(26)

/* Interrupt Enalbe Register */
#define PCIE_IRNEN				0xF4

#define PCIE_IRNCR				0xF8

/* PCIe interrupt enable/control/capture register definition */
#define PCIE_IRN_AER_REPORT		BIT(0)
#define PCIE_IRN_PME			BIT(2)
#define PCIE_IRN_HOTPLUG		BIT(3)
#define PCIE_IRN_RX_VDM_MSG		BIT(4)
#define PCIE_IRN_FRS			BIT(5)
#define PCIE_IRN_CFG_DRS		BIT(6)
#define PCIE_IRN_DRS_FRS		BIT(7)
#define PCIE_IRN_RBAR_CTRL_UPDATE	BIT(8)
#define PCIE_IRN_PM_TO_ACK		BIT(9)
#define PCIE_IRN_PM_TURNOFF_ACK		BIT(10)
#define PCIE_IRN_LINK_AUTO_BW_STATUS	BIT(11)
#define PCIE_IRN_BW_MGT			BIT(12)
#define PCIE_IRN_INTA			BIT(13) /* INTA */
#define PCIE_IRN_INTB			BIT(14) /* INTB */
#define PCIE_IRN_INTC			BIT(15) /* INTC */
#define PCIE_IRN_INTD			BIT(16) /* INTD */
#define PCIE_IRN_WAKEUP			BIT(17) /* Wake up Interrupt */
#define PCIE_IRN_MSG_LTR		BIT(18)
#define PCIE_IRN_SUPRISE_DOWN		BIT(19)
#define PCIE_IRN_TRGT1_CPL_TIMEOUT	BIT(20)
#define PCIE_IRN_RADM_CPL_TIMEOUT	BIT(21)
#define PCIE_IRN_FLR_ACTIVE		BIT(22)
#define PCIE_IRN_LINK_EQ_REQ_INT	BIT(23)
#define PCIE_IRN_LINK_REQ_RST_NOT	BIT(26)
#define PCIE_IRN_SYS_INT		BIT(28)
#define PCIE_IRN_SYS_ERR_RC		BIT(29)
#define PCIE_IRN_RX_BEACON		BIT(30)

#define PCIE_INTX_OFFSET		12

#define PCIE_IRN_IR_INT  (PCIE_IRN_AER_REPORT | PCIE_IRN_PME		\
	| PCIE_IRN_RX_VDM_MSG | PCIE_IRN_SYS_ERR_RC			\
	| PCIE_IRN_PM_TO_ACK | PCIE_IRN_PM_TURNOFF_ACK			\
	| PCIE_IRN_LINK_AUTO_BW_STATUS | PCIE_IRN_BW_MGT		\
	| PCIE_IRN_WAKEUP | PCIE_IRN_MSG_LTR | PCIE_IRN_SYS_INT)

/*
 * iATU Unroll-specific register definitions
 * From 4.80 core version the address translation will be made by unroll
 */
#define PCIE_ATU_UNR_REGION_CTRL1	0x00
#define PCIE_ATU_UNR_REGION_CTRL2	0x04
#define PCIE_ATU_UNR_LOWER_BASE		0x08
#define PCIE_ATU_UNR_UPPER_BASE		0x0C
#define PCIE_ATU_UNR_LIMIT		0x10
#define PCIE_ATU_UNR_LOWER_TARGET	0x14
#define PCIE_ATU_UNR_UPPER_TARGET	0x18

/* Parameters for the waiting for iATU enabled routine */
#define LINK_WAIT_MAX_IATU_RETRIES	100
#define PCIE_ATU_REGION_INDEX2		(0x2 << 0)
#define PCIE_ATU_REGION_INDEX1		(0x1 << 0)
#define PCIE_ATU_REGION_INDEX0		(0x0 << 0)
#define PCIE_ATU_CR1			0xC000
#define PCIE_ATU_TYPE_MEM		(0x0 << 0)
#define PCIE_ATU_TYPE_IO		(0x2 << 0)
#define PCIE_ATU_TYPE_CFG0		(0x4 << 0)
#define PCIE_ATU_TYPE_CFG1		(0x5 << 0)
#define PCIE_ATU_CR2			0xC004
#define PCIE_ATU_ENABLE			BIT(31)
#define PCIE_ATU_CFG_SHIFT_MODE		BIT(28)

#define PCIE_ATU_BUS(x)			(((x) & 0xff) << 24)
#define PCIE_ATU_DEV(x)			(((x) & 0x1f) << 19)
#define PCIE_ATU_FUNC(x)		(((x) & 0x7) << 16)
/* Register address builder */
#define PCIE_ATU_OUTB_OFFS(region)	((0x3 << 18) | ((region) << 9))

/* MSI PIC */
#define MSI_PIC_BIG_ENDIAN		1
#define MSI_PIC_LITTLE_ENDIAN		0

#define MSI_PCI_INT_DISABLE		BIT(31)
#define MSI_PIC_INT_LINE		GENMASK(30, 28)
#define MSI_PIC_INT_LINE_S		28
#define MSI_MSG_ADDR_MASK		GENMASK(23, 12)
#define MSI_PIC_MSG_ADDR		GENMASK(27, 12)
#define MSI_PIC_MSG_ADDR_S		16
#define MSI_PIC_MSG_DATA		GENMASK(15, 0)
#define MSI_PIC_MSG_DATA_S		0x0

#define PCIE_MSI_MAX_IRQ_NUM_PER_RC	8

/* Keep base data lower bits as zero since MSI has maximum 32 vectors */
#define PCIE_MSI_BASE_DATA		0x4AE0

#define MSI_IRQ_PIC_TABLE_SZ		16
/* Interrupt related stuff */
enum {
	PCIE_LEGACY_DISABLE = 0,
	PCIE_LEGACY_INTA,
	PCIE_LEGACY_INTB,
	PCIE_LEGACY_INTC,
	PCIE_LEGACY_INTD,
};

#define PCIE_LEGACY_INT_MAX	PCIE_LEGACY_INTD

#ifdef CONFIG_PCIE_INTEL_MSI_PIC
struct msi_irq_idx {
	int irq;
	int idx;
};

struct msi_irq {
	u32 msi_phy_base;
	struct msi_irq_idx msi_irq_idx[MSI_IRQ_PIC_TABLE_SZ];
	spinlock_t msi_lock; /* Protected msi configuration */
	/*
	 * Each bit in msi_free_irq_bitmask represents a MSI interrupt that is
	 * in use.
	 */
	u16 msi_free_irq_bitmask;

	/*
	 * Each bit in msi_multiple_irq_bitmask tells that the device using
	 * this bit in msi_free_irq_bitmask is also using the next bit. This
	 * is used so we can disable all of the MSI interrupts when a device
	 * uses multiple.
	 */
	u16 msi_multiple_irq_bitmask;
};
#endif /* CONFIG_PCIE_INTEL_MSI_PIC */

struct intel_pcie_port {
	int id; /* Physical RC Index */
	struct list_head list;
	struct regmap	*syscfg;
	void __iomem	*app_base;
	struct device	*dev;
	struct pci_bus	*root_bus;
	spinlock_t lock; /* Per port exclusive for cfg read/write */
	bool		has_iatu;
	void __iomem	*dbi_base;
	void __iomem	*cfg0_base;
	u32		cfg0_size;
	phys_addr_t	cfg0_phy_base;
	void __iomem	*cfg1_base;
	u32		cfg1_size;
	phys_addr_t	cfg1_phy_base;
	resource_size_t	io_base;
	phys_addr_t	io_bus_addr;
	u32		io_size;
	phys_addr_t	mem_base;
	phys_addr_t	mem_bus_addr;
	u32		mem_size;
	struct resource *io;
	struct resource *mem;
	struct gpio_desc *reset_gpio;
	u32		rst_interval;
	bool		inbound_swap;
	u32		inbound_shift;
	bool		outbound_swap;
	u32		outbound_shift;
	u32		max_speed;
	u32		link_gen;
	u32		max_width;
	u32		lanes;
	int		plat_irq;
	char		irq_name[16];
#ifdef CONFIG_PCIE_INTEL_MSI_PIC
	phys_addr_t	msi_base;
	void __iomem	*msi_pic;
	struct msi_irq	msi_irqs;
	u32		msr_irq_nrs;
#endif /* CONFIG_PCIE_INTEL_MSI_PIC */
	struct clk *core_clk;
	struct reset_control *core_rst;
	struct reset_control *rcu_rst;
	struct phy *phy;
	struct list_head resources;
	u8 root_bus_nr;
};

static LIST_HEAD(intel_pcie_list);
static DEFINE_MUTEX(intel_pcie_lock);

/* Access other EP configuration space */
static inline u32 pcie_cfg_rd(void __iomem *addr, u32 reg)
{
	return readl(addr + reg);
}

static inline void pcie_cfg_wr(void __iomem *addr,
			       u32 val, unsigned int reg)
{
	writel(val, addr + reg);
}

static inline u32 pcie_app_rd(struct intel_pcie_port *lpp, u32 reg)
{
	return readl(lpp->app_base + reg);
}

static inline void pcie_app_wr(struct intel_pcie_port *lpp,
			       u32 val, unsigned int reg)
{
	writel(val, lpp->app_base + reg);
}

static inline void pcie_app_wr_mask(struct intel_pcie_port *lpp, u32 clear,
				    u32 set, unsigned int reg)
{
	pcie_app_wr(lpp, (pcie_app_rd(lpp, reg) & ~(clear)) | (set), reg);
}

static inline u32 pcie_rc_cfg_rd(struct intel_pcie_port *lpp, u32 reg)
{
	return readl(lpp->dbi_base + reg);
}

static inline void pcie_rc_cfg_wr(struct intel_pcie_port *lpp, u32 val,
				  unsigned int reg)
{
	writel(val, lpp->dbi_base + reg);
}

static inline void pcie_rc_cfg_wr_mask(struct intel_pcie_port *lpp, u32 clear,
				       u32 set, unsigned int reg)
{
	pcie_rc_cfg_wr(lpp, (pcie_rc_cfg_rd(lpp, reg) & ~(clear)) | (set), reg);
}

#ifdef CONFIG_PCIE_INTEL_MSI_PIC
static inline u32 pcie_msi_rd(struct intel_pcie_port *lpp, u32 idx)
{
	return readl(lpp->msi_pic + (idx << 2));
}

static inline void pcie_msi_wr(struct intel_pcie_port *lpp,
			       u32 val, unsigned int idx)
{
	writel(val, lpp->msi_pic + (idx << 2));
}

static inline void pcie_msi_wr_mask(struct intel_pcie_port *lpp, u32 clear,
				    u32 set, unsigned int idx)
{
	pcie_msi_wr(lpp, (pcie_msi_rd(lpp, idx) & ~(clear)) | (set), idx);
}
#endif /* CONFIG_PCIE_INTEL_MSI_PIC */

static u32 pcie_readl_unroll(struct intel_pcie_port *lpp, u32 index, u32 reg)
{
	u32 offset = PCIE_ATU_OUTB_OFFS(index);

	return pcie_rc_cfg_rd(lpp, offset + reg);
}

static void pcie_writel_unroll(struct intel_pcie_port *lpp, u32 index, u32 reg,
			       u32 val)
{
	u32 offset = PCIE_ATU_OUTB_OFFS(index);

	pcie_rc_cfg_wr(lpp, val, offset + reg);
}

static void pcie_prog_outbound_atu(struct intel_pcie_port *lpp, int index,
				   int type, u64 cpu_addr, u64 pci_addr,
				   u32 size)
{
	u32 retries, val;

	pcie_writel_unroll(lpp, index, PCIE_ATU_UNR_LOWER_BASE,
			   lower_32_bits(cpu_addr));
	pcie_writel_unroll(lpp, index, PCIE_ATU_UNR_UPPER_BASE,
			   upper_32_bits(cpu_addr));
	pcie_writel_unroll(lpp, index, PCIE_ATU_UNR_LIMIT,
			   lower_32_bits(cpu_addr + size - 1));
	pcie_writel_unroll(lpp, index, PCIE_ATU_UNR_LOWER_TARGET,
			   lower_32_bits(pci_addr));
	pcie_writel_unroll(lpp, index, PCIE_ATU_UNR_UPPER_TARGET,
			   upper_32_bits(pci_addr));
	pcie_writel_unroll(lpp, index, PCIE_ATU_UNR_REGION_CTRL1, type);
	pcie_writel_unroll(lpp, index, PCIE_ATU_UNR_REGION_CTRL2,
			   PCIE_ATU_ENABLE);

	/*
	 * Make sure ATU enable takes effect before any subsequent config
	 * and I/O accesses.
	 */
	for (retries = 0; retries < LINK_WAIT_MAX_IATU_RETRIES; retries++) {
		val = pcie_readl_unroll(lpp, index, PCIE_ATU_UNR_REGION_CTRL2);
		if (!!(val & PCIE_ATU_ENABLE))
			return;
		udelay(5);
	}
	dev_err(lpp->dev, "iATU is not being enabled\n");
}

static u32 intel_pcie_rd_other_conf(struct intel_pcie_port *lpp,
				    struct pci_bus *bus, u32 devfn, int where)
{
	int type;
	u32 busdev, cfg_size;
	u64 cpu_addr;
	void __iomem *va_cfg_base;

	busdev = PCIE_ATU_BUS(bus->number) | PCIE_ATU_DEV(PCI_SLOT(devfn)) |
		 PCIE_ATU_FUNC(PCI_FUNC(devfn));

	if (bus->parent->number == lpp->root_bus_nr) {
		type = PCIE_ATU_TYPE_CFG0;
		cpu_addr = lpp->cfg0_phy_base;
		cfg_size = lpp->cfg0_size;
		va_cfg_base = lpp->cfg0_base;
	} else {
		type = PCIE_ATU_TYPE_CFG1;
		cpu_addr = lpp->cfg1_phy_base;
		cfg_size = lpp->cfg1_size;
		va_cfg_base = lpp->cfg1_base;
	}

	pcie_prog_outbound_atu(lpp, PCIE_ATU_REGION_INDEX0, type, cpu_addr,
			       busdev, cfg_size);
	return pcie_cfg_rd(va_cfg_base, ((where & 0xFFF) & ~0x3));
}

static void intel_pcie_wr_other_conf(struct intel_pcie_port *lpp,
				     struct pci_bus *bus,
				     u32 devfn, int where, u32 val)
{
	int type;
	u32 busdev, cfg_size;
	u64 cpu_addr;
	void __iomem *va_cfg_base;

	busdev = PCIE_ATU_BUS(bus->number) | PCIE_ATU_DEV(PCI_SLOT(devfn)) |
		 PCIE_ATU_FUNC(PCI_FUNC(devfn));

	if (bus->parent->number == lpp->root_bus_nr) {
		type = PCIE_ATU_TYPE_CFG0;
		cpu_addr = lpp->cfg0_phy_base;
		cfg_size = lpp->cfg0_size;
		va_cfg_base = lpp->cfg0_base;
	} else {
		type = PCIE_ATU_TYPE_CFG1;
		cpu_addr = lpp->cfg1_phy_base;
		cfg_size = lpp->cfg1_size;
		va_cfg_base = lpp->cfg1_base;
	}

	pcie_prog_outbound_atu(lpp, PCIE_ATU_REGION_INDEX0, type, cpu_addr,
			       busdev, cfg_size);
	pcie_cfg_wr(va_cfg_base, val, ((where & 0xFFF) & ~0x3));
}

static u32 pcie_bus_addr(struct pci_bus *bus, u16 devfn, int where)
{
	u32 addr;
	u8 bus_num = bus->number;

	if (!bus_num) {
		/* type 0 */
		addr = ((PCI_SLOT(devfn) & 0x1F) << 15)
		| ((PCI_FUNC(devfn) & 0x7) << 12)
		| ((where & 0xFFF) & ~3);
	} else {
		/* type 1, only support 8 buses */
		addr = ((bus_num & 0x7) << 20)
		| ((PCI_SLOT(devfn) & 0x1F) << 15)
		| ((PCI_FUNC(devfn) & 0x7) << 12)
		| ((where & 0xFFF) & ~3);
	}
	return addr;
}

static bool intel_pcie_link_up(struct intel_pcie_port *lpp)
{
	u32 val;

	val = pcie_rc_cfg_rd(lpp, PCIE_DBR1);
	return ((val & PCIE_DBR1_PHY_LINK_UP) &&
		(!(val & PCIE_DBR1_PHY_IN_TRAINING)));
}

static bool intel_pcie_valid_device(struct intel_pcie_port *lpp,
				    struct pci_bus *bus, int dev)
{
	if (pci_is_root_bus(bus)) {
		if (dev > 0)
			return false;
	}

	/* If there is no link, then there is no device */
	if (!intel_pcie_link_up(lpp))
		return false;

	if (bus->primary == lpp->root_bus_nr && dev > 0)
		return false;
	return true;
}

static int intel_pcie_read_config(struct pci_bus *bus, u32 devfn,
				  int where, int size, u32 *value)
{
	u32 val;
	u32 data = 0;
	static const u32 mask[8] = { 0, 0xff, 0xffff, 0, 0xffffffff, 0, 0, 0 };
	int ret = PCIBIOS_SUCCESSFUL;
	struct intel_pcie_port *lpp = bus->sysdata;

	if (unlikely(size != 1 && size != 2 && size != 4)) {
		ret = PCIBIOS_BAD_REGISTER_NUMBER;
		goto out;
	}

	/* Make sure the address is aligned to natural boundary */
	if (unlikely(((size - 1) & where))) {
		ret = PCIBIOS_BAD_REGISTER_NUMBER;
		goto out;
	}

	if (!intel_pcie_valid_device(lpp, bus, PCI_SLOT(devfn))) {
		*value = 0xffffffff;
		ret = PCIBIOS_DEVICE_NOT_FOUND;
		goto out;
	}

	if (pci_is_root_bus(bus)) {/* RC itself */
		u32 tmp;

		tmp = (where & ~3);
		data = pcie_rc_cfg_rd(lpp, tmp);
		/* NB, ignore RC BAR0/BAR1 due to hardware bugs */
		if (tmp == PCI_BASE_ADDRESS_0 || tmp == PCI_BASE_ADDRESS_1)
			data = 0;
	} else {
		if (lpp->has_iatu) {
			data = intel_pcie_rd_other_conf(lpp, bus, devfn, where);
		} else {
			u32 addr = pcie_bus_addr(bus, devfn, where);

			data = pcie_cfg_rd(lpp->cfg0_base, addr);
		}
		if (lpp->outbound_swap)
			data = le32_to_cpu(data);
	}

	val = (data >> (8 * (where & 3))) & mask[size & 7];
	*value = val;
out:
	return ret;
}

static u32 pcie_size_to_value(int where, int size, u32 data, u32 value)
{
	u32 shift;
	u32 tdata = data;

	switch (size) {
	case 1:
		shift = (where & 0x3) << 3;
		tdata &= ~(0xffU << shift);
		tdata |= ((value & 0xffU) << shift);
		break;
	case 2:
		shift = (where & 3) << 3;
		tdata &= ~(0xffffU << shift);
		tdata |= ((value & 0xffffU) << shift);
		break;
	case 4:
		tdata = value;
		break;
	}
	return tdata;
}

static int intel_pcie_write_config(struct pci_bus *bus, u32 devfn,
				   int where, int size, u32 value)
{
	u32 data;
	u32 tvalue = value;
	int ret = PCIBIOS_SUCCESSFUL;
	struct intel_pcie_port *lpp = bus->sysdata;

	/* Make sure the address is aligned to natural boundary */
	if (unlikely(((size - 1) & where))) {
		ret = PCIBIOS_BAD_REGISTER_NUMBER;
		goto out;
	}

	if (!intel_pcie_valid_device(lpp, bus, PCI_SLOT(devfn))) {
		ret = PCIBIOS_DEVICE_NOT_FOUND;
		goto out;
	}

	if (pci_is_root_bus(bus)) {/* RC itself */
		u32 tmp;

		tmp = (where & ~3);
		data = pcie_rc_cfg_rd(lpp, tmp);
		data = pcie_size_to_value(where, size, data, tvalue);
		pcie_rc_cfg_wr(lpp, data, tmp);
	} else {
		if (lpp->has_iatu) {
			data = intel_pcie_rd_other_conf(lpp, bus, devfn, where);
			if (lpp->outbound_swap)
				data = le32_to_cpu(data);
			data = pcie_size_to_value(where, size, data, tvalue);
			if (lpp->outbound_swap)
				data = cpu_to_le32(data);
			intel_pcie_wr_other_conf(lpp, bus, devfn, where, data);
		} else {
			u32 addr = pcie_bus_addr(bus, devfn, where);

			data = pcie_cfg_rd(lpp->cfg0_base, addr);
			if (lpp->outbound_swap)
				data = le32_to_cpu(data);

			data = pcie_size_to_value(where, size, data, tvalue);
			if (lpp->outbound_swap)
				data = cpu_to_le32(data);
			pcie_cfg_wr(lpp->cfg0_base, data, addr);
		}
	}
out:
	return ret;
}

/* PCIe operations */
static struct pci_ops intel_pcie_ops = {
	.read	= intel_pcie_read_config,
	.write	= intel_pcie_write_config,
};

static int intel_pcie_bios_map_irq(const struct pci_dev *dev, u8 slot, u8 pin)
{
	u32 irq_bit = 0;
	int irq = 0;
	struct intel_pcie_port *lpp = dev->bus->sysdata;
	struct device *pdev = lpp->dev;

	dev_dbg(pdev, "%s port %d dev %s slot %d pin %d enter\n", __func__,
		lpp->id, pci_name(dev), slot, pin);

	if (pin == PCIE_LEGACY_DISABLE || pin > PCIE_LEGACY_INT_MAX) {
		dev_warn(pdev, "WARNING: dev %s: invalid interrupt pin %d\n",
			 pci_name(dev), pin);
		return -1;
	}

	irq = of_irq_parse_and_map_pci(dev, slot, pin);
	if (!irq) {
		dev_err(pdev, "trying to map irq for unknown slot:%d pin:%d\n",
			slot, pin);
		return -1;
	}

	/* Pin to irq offset bit position */
	irq_bit = BIT((pin + PCIE_INTX_OFFSET));

	/* Clear possible pending interrupts first */
	pcie_app_wr_mask(lpp, 0, irq_bit, PCIE_IRNCR);
	dev_dbg(pdev, "%s PCIE_IRNCR: 0x%08x\n", __func__,
		pcie_app_rd(lpp, PCIE_IRNCR));

	pcie_app_wr_mask(lpp, 0, irq_bit, PCIE_IRNEN);
	dev_dbg(pdev,  "%s PCIE_IRNEN: 0x%08x\n", __func__,
		pcie_app_rd(lpp, PCIE_IRNEN));
	dev_dbg(pdev, "%s dev %s irq %d assigned\n", __func__,
		pci_name(dev), irq);
	dev_dbg(pdev, "%s dev %s: exit\n", __func__, pci_name(dev));
	return irq;
}

static int intel_pcie_wait_phy_link_up(struct intel_pcie_port *lpp)
{
	int err;
	u32 val;
	struct device *dev = lpp->dev;

	/* Wait for PHY link is up, 1 second should be sufficient */
	err = readl_poll_timeout(lpp->dbi_base + PCIE_DBR1,
				 val, ((val & PCIE_DBR1_PHY_LINK_UP) &&
				 (!(val & PCIE_DBR1_PHY_IN_TRAINING))),
				 50, 1000 * USEC_PER_MSEC);
	if (err)
		dev_err(dev, "%s port %d timeout\n", __func__, lpp->id);

	return err;
}

static void intel_pcie_bridge_class_code_setup(struct intel_pcie_port *lpp)
{
	pcie_rc_cfg_wr_mask(lpp, 0, PCIE_MISC_CTRL_DBI_RO_WR_EN,
			    PCIE_MISC_CTRL);
	pcie_rc_cfg_wr_mask(lpp, 0xffffff00, PCI_CLASS_BRIDGE_PCI << 16,
			    PCIE_CCRID);
	pcie_rc_cfg_wr_mask(lpp, PCIE_MISC_CTRL_DBI_RO_WR_EN, 0,
			    PCIE_MISC_CTRL);
}

static void intel_pcie_lane_and_width_setup(struct intel_pcie_port *lpp)
{
	u32 val;
	u32 lanes = lpp->lanes;
	struct device *dev = lpp->dev;

	val = pcie_rc_cfg_rd(lpp, PCIE_PLCR);
	val &= ~PCIE_PLCR_LINK_MODE;
	switch (lanes) {
	case 1:
		val |= PCIE_PORT_LINK_MODE_1_LANES;
		break;
	case 2:
		val |= PCIE_PORT_LINK_MODE_2_LANES;
		break;
	case 4:
		val |= PCIE_PORT_LINK_MODE_4_LANES;
		break;
	case 8:
		val |= PCIE_PORT_LINK_MODE_8_LANES;
		break;
	default:
		dev_err(dev, "PCIe not valid lane value\n");
		return;
	}
	pcie_rc_cfg_wr(lpp, val, PCIE_PLCR);

	/* Link width speed control register */
	val = pcie_rc_cfg_rd(lpp, PCIE_LINK_WIDTH_SPEED_CONTROL);
	val &= ~PCIE_PORT_LOGIC_LINK_WIDTH_MASK;
	switch (lanes) {
	case 1:
		val |= PCIE_PORT_LOGIC_LINK_WIDTH_1_LANES;
		break;
	case 2:
		val |= PCIE_PORT_LOGIC_LINK_WIDTH_2_LANES;
		break;
	case 4:
		val |= PCIE_PORT_LOGIC_LINK_WIDTH_4_LANES;
		break;
	case 8:
		val |= PCIE_PORT_LOGIC_LINK_WIDTH_8_LANES;
		break;
	}
	pcie_rc_cfg_wr(lpp, val, PCIE_LINK_WIDTH_SPEED_CONTROL);

	dev_dbg(dev, "PCIE_PLCR: 0x%08x\n", pcie_rc_cfg_rd(lpp, PCIE_PLCR));
	dev_dbg(dev, "PCIE_LINK_WIDTH_SPEED_CONTROL: 0x%08x\n",
		pcie_rc_cfg_rd(lpp, PCIE_LINK_WIDTH_SPEED_CONTROL));
}

static void intel_pcie_rc_mode_setup(struct intel_pcie_port *lpp)
{
	pcie_app_wr_mask(lpp, PCIE_DEVICE_TYPE,
			 PCIE_RC_CCR_RC_MODE << PCIE_DEVICE_TYPE_S,
			 PCIE_RC_CCR);

	dev_dbg(lpp->dev, "%s PCIE_RC_CCR 0x%08x\n",
		__func__, pcie_app_rd(lpp, PCIE_RC_CCR));
}

static void intel_pcie_ltssm_enable(struct intel_pcie_port *lpp)
{
	/* Enable LTSSM */
	pcie_app_wr_mask(lpp, 0, PCIE_RC_CCR_LTSSM_ENABLE, PCIE_RC_CCR);
	dev_dbg(lpp->dev, "%s PCIE_RC_CCR 0x%08x\n",
		__func__, pcie_app_rd(lpp, PCIE_RC_CCR));
}

static void intel_pcie_ltssm_disable(struct intel_pcie_port *lpp)
{
	pcie_app_wr_mask(lpp, PCIE_RC_CCR_LTSSM_ENABLE, 0, PCIE_RC_CCR);
	dev_dbg(lpp->dev, "%s PCIE_RC_CCR 0x%08x\n",
		__func__, pcie_app_rd(lpp, PCIE_RC_CCR));
}

static const char *pcie_link_gen_to_str(int gen)
{
	switch (gen) {
	case PCIE_LINK_SPEED_GEN1:
		return "2.5";
	case PCIE_LINK_SPEED_GEN2:
		return "5.0";
	case PCIE_LINK_SPEED_GEN3:
		return "8.0";
	case PCIE_LINK_SPEED_GEN4:
		return "16.0";
	default:
		break;
	}
	return "???";
}

static void intel_pcie_link_setup(struct intel_pcie_port *lpp)
{
	u32 val;

	val = pcie_rc_cfg_rd(lpp, PCIE_LCAP);
	lpp->max_speed = (val & PCIE_LCAP_MAX_LINK_SPEED);
	lpp->max_width = (val & PCIE_LCAP_MAX_LENGTH_WIDTH) >>
			 PCIE_LCAP_MAX_LENGTH_WIDTH_S;
	dev_dbg(lpp->dev, "%s:max speed %s width x%d\n", __func__,
		pcie_link_gen_to_str(lpp->max_speed), lpp->max_width);

	pcie_rc_cfg_wr_mask(lpp, PCIE_LCTLSTS_LINK_DISABLE |
			    PCIE_LCTLSTS_ASPM_ENABLE,
			    PCIE_LCTLSTS_SLOT_CLK_CFG |
			    PCIE_LCTLSTS_COM_CLK_CFG | PCIE_LCTLSTS_RCB128,
			    PCIE_LCTLSTS);

	dev_dbg(lpp->dev, "%s PCIE_LCTLSTS: 0x%08x\n",
		__func__, pcie_rc_cfg_rd(lpp, PCIE_LCTLSTS));
}

static void intel_pcie_max_speed_setup(struct intel_pcie_port *lpp)
{
	u32 reg, val;
	struct device *dev = lpp->dev;

	reg = pcie_rc_cfg_rd(lpp, PCIE_LCTLSTS2);
	switch (lpp->link_gen) {
	case PCIE_LINK_SPEED_GEN1:
		reg &= ~PCIE_LCTLSTS2_TGT_LINK_SPEED;
		reg |= PCIE_LCTLSTS2_HW_AUTO_DIS |
			PCIE_LCTLSTS2_TGT_LINK_SPEED_25GT;
		break;
	case PCIE_LINK_SPEED_GEN2:
		reg &= ~PCIE_LCTLSTS2_TGT_LINK_SPEED;
		reg |= PCIE_LCTLSTS2_HW_AUTO_DIS |
			PCIE_LCTLSTS2_TGT_LINK_SPEED_5GT;
		break;
	case PCIE_LINK_SPEED_GEN3:
		reg &= ~PCIE_LCTLSTS2_TGT_LINK_SPEED;
		reg |= PCIE_LCTLSTS2_HW_AUTO_DIS |
			PCIE_LCTLSTS2_TGT_LINK_SPEED_8GT;
		break;
	case PCIE_LINK_SPEED_GEN4:
		reg &= ~PCIE_LCTLSTS2_TGT_LINK_SPEED;
		reg |= PCIE_LCTLSTS2_HW_AUTO_DIS |
			PCIE_LCTLSTS2_TGT_LINK_SPEED_16GT;
		break;
	default:
		/* Use hardware capability */
		val = pcie_rc_cfg_rd(lpp, PCIE_LCAP);
		val &= PCIE_LCAP_MAX_LINK_SPEED;
		reg &= ~PCIE_LCTLSTS2_HW_AUTO_DIS;
		reg |= val;
		break;
	}
	pcie_rc_cfg_wr(lpp, reg, PCIE_LCTLSTS2);
	dev_dbg(dev, "%s PCIE_LCTLSTS2 0x%08x 0x%08x\n",
		__func__, pcie_rc_cfg_rd(lpp, PCIE_LCTLSTS2), reg);
}

static void intel_pcie_speed_change_enable(struct intel_pcie_port *lpp)
{
	pcie_rc_cfg_wr_mask(lpp, PCIE_PORT_LOGIC_FTS,
			    PCIE_PORT_LOGIC_SPEED_CHANGE |
			    PCIE_PORT_LOGIC_DEFAULT_FTS_NUM,
			    PCIE_LINK_WIDTH_SPEED_CONTROL);
	dev_dbg(lpp->dev, "%s PCIE_LINK_WIDTH_SPEED_CONTROL: 0x%08x\n",
		__func__, pcie_rc_cfg_rd(lpp, PCIE_LINK_WIDTH_SPEED_CONTROL));
}

static void intel_pcie_speed_change_disable(struct intel_pcie_port *lpp)
{
	pcie_rc_cfg_wr_mask(lpp, PCIE_PORT_LOGIC_SPEED_CHANGE, 0,
			    PCIE_LINK_WIDTH_SPEED_CONTROL);
	dev_dbg(lpp->dev, "%s PCIE_LINK_WIDTH_SPEED_CONTROL: 0x%08x\n",
		__func__, pcie_rc_cfg_rd(lpp, PCIE_LINK_WIDTH_SPEED_CONTROL));
}

static void intel_pcie_max_link_width_setup(struct intel_pcie_port *lpp)
{
	u32 reg;
	struct device *dev = lpp->dev;

	reg = pcie_rc_cfg_rd(lpp, PCIE_LCTLSTS);
	/* HW auto bandwidth negotiation must be enabled */
	reg &= ~PCIE_LCTLSTS_HW_AW_DIS;
	pcie_rc_cfg_wr(lpp, reg, PCIE_LCTLSTS);

	reg = pcie_rc_cfg_rd(lpp, PCIE_MULTI_LANE_CTRL);
	reg &= ~PCIE_TARGET_LINK_WIDTH;
	reg |= lpp->lanes;
	reg |= PCIE_DIRECT_LINK_WIDTH_CHANGE;
	pcie_rc_cfg_wr(lpp, reg, PCIE_MULTI_LANE_CTRL);
	dev_dbg(dev, "%s PCIE_MULTI_LANE_CTRL 0x%08x 0x%08x\n",
		__func__, pcie_rc_cfg_rd(lpp, PCIE_MULTI_LANE_CTRL), reg);
}

static void intel_pcie_port_logic_setup(struct intel_pcie_port *lpp)
{
	u32 reg, fts;
	struct device *dev = lpp->dev;

	switch (lpp->max_speed) {
	case PCIE_LINK_SPEED_GEN1:
	case PCIE_LINK_SPEED_GEN2:
		fts = PCIE_AFR_GEN12_FTS_NUM_DEFAULT;
		break;
	case PCIE_LINK_SPEED_GEN3:
		fts = PCIE_AFR_GEN3_FTS_NUM_DEFAULT;
		break;
	case PCIE_LINK_SPEED_GEN4:
		fts = PCIE_AFR_GEN4_FTS_NUM_DEFAULT;
		break;
	default:
		fts = PCIE_AFR_GEN12_FTS_NUM_DEFAULT;
		break;
	}
	reg = pcie_rc_cfg_rd(lpp, PCIE_AFR);
	reg &= ~(PCIE_AFR_FTS_NUM | PCIE_AFR_COM_FTS_NUM);
	reg |= (fts << PCIE_AFR_FTS_NUM_S) | (fts << PCIE_AFR_COM_FTS_NUM_S);
	pcie_rc_cfg_wr(lpp, reg, PCIE_AFR);

	dev_dbg(dev, "%s PCIE_AFR: 0x%08x\n",
		__func__, pcie_rc_cfg_rd(lpp, PCIE_AFR));

	/* Port Link Control Register */
	pcie_rc_cfg_wr_mask(lpp, 0, PCIE_PLCR_DLL_LINK_EN, PCIE_PLCR);
	dev_dbg(dev, "%s PCIE_PLCR: 0x%08x\n",
		__func__, pcie_rc_cfg_rd(lpp, PCIE_PLCR));
}

static void intel_pcie_upconfig_setup(struct intel_pcie_port *lpp)
{
	u32 reg;

	reg = pcie_rc_cfg_rd(lpp, PCIE_MULTI_LANE_CTRL);
	reg |= PCIE_UPCONFIG_SUPPORT;
	pcie_rc_cfg_wr(lpp, reg, PCIE_MULTI_LANE_CTRL);

	reg = pcie_rc_cfg_rd(lpp, PCIE_IOP_CTRL);
	reg &= ~PCIE_IOP_RX_STANDBY_CTRL;
	pcie_rc_cfg_wr(lpp, reg, PCIE_IOP_CTRL);
}

static void intel_pcie_rc_setup(struct intel_pcie_port *lpp)
{
	intel_pcie_ltssm_disable(lpp);
	intel_pcie_link_setup(lpp);
	if (lpp->max_speed >= PCIE_LINK_SPEED_GEN3) {
		intel_pcie_rc_mode_setup(lpp);
		intel_pcie_lane_and_width_setup(lpp);
	}
	intel_pcie_upconfig_setup(lpp);
	intel_pcie_bridge_class_code_setup(lpp);
	intel_pcie_max_speed_setup(lpp);
	intel_pcie_speed_change_enable(lpp);
	intel_pcie_port_logic_setup(lpp);
}

static int intel_pcie_ep_rst_init(struct intel_pcie_port *lpp)
{
	int err;
	struct device *dev = lpp->dev;

	lpp->reset_gpio = devm_gpiod_get_optional(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(lpp->reset_gpio)) {
		err = PTR_ERR(lpp->reset_gpio);
		dev_err(dev, "failed to request PCIe GPIO: %d\n", err);
		return err;
	}

	lpp->rcu_rst = devm_reset_control_get_optional(lpp->dev, "rcu_rst");
	if (IS_ERR(lpp->rcu_rst)) {
		err = PTR_ERR(lpp->rcu_rst);
		if (err == -ENOENT) {
			lpp->rcu_rst = NULL;
		} else {
			dev_err(dev, "failed to get rcu reset: %d\n", err);
			return err;
		}
	}

	/* Only one of them is needed */
	if ((lpp->reset_gpio && lpp->rcu_rst) ||
	    (!lpp->reset_gpio && !lpp->rcu_rst))
		return -EINVAL;

	if (lpp->rcu_rst)
		reset_control_deassert(lpp->rcu_rst);

	msleep(100);
	return 0;
}

static void intel_pcie_syscfg_clr(struct intel_pcie_port *lpp, u32 off,
				  int bitn)
{
	regmap_update_bits(lpp->syscfg, off, BIT(bitn), 0 << bitn);
}

#ifdef CONFIG_CPU_BIG_ENDIAN
static void intel_pcie_syscfg_set(struct intel_pcie_port *lpp, u32 off,
				  int bitn)
{
	regmap_update_bits(lpp->syscfg, off, BIT(bitn), BIT(bitn));
}

static void intel_pcie_endian_set(struct intel_pcie_port *lpp, u32 off,
				  int bitn)
{
	intel_pcie_syscfg_set(lpp, off, bitn);
}
#endif /* CONFIG_CPU_BIG_ENDIAN */

static void intel_pcie_endian_clr(struct intel_pcie_port *lpp, u32 off,
				  int bitn)
{
	intel_pcie_syscfg_clr(lpp, off, bitn);
}

static void intel_pcie_endian_setup(struct intel_pcie_port *lpp)
{
	const u32 endian_reg_offset = 0x4c;
#ifdef CONFIG_CPU_BIG_ENDIAN
	if (lpp->outbound_swap)
		intel_pcie_endian_set(lpp, endian_reg_offset,
				      lpp->outbound_shift);
	else
		intel_pcie_endian_clr(lpp, endian_reg_offset,
				      lpp->outbound_shift);
	if (lpp->inbound_swap)
		intel_pcie_endian_set(lpp, endian_reg_offset,
				      lpp->inbound_shift);
	else
		intel_pcie_endian_clr(lpp, endian_reg_offset,
				      lpp->inbound_shift);
#else
	intel_pcie_endian_clr(lpp, endian_reg_offset,
			      lpp->outbound_shift);
	intel_pcie_endian_clr(lpp, endian_reg_offset,
			      lpp->inbound_shift);
#endif /* CONFIG_CPU_BIG_ENDIAN */
}

static void intel_pcie_core_rst_assert(struct intel_pcie_port *lpp)
{
	if (lpp->core_rst)
		reset_control_assert(lpp->core_rst);
}

static void intel_pcie_core_rst_deassert(struct intel_pcie_port *lpp)
{
	if (lpp->core_rst) {
		/* Make sure one micro-second delay */
		udelay(1);
		reset_control_deassert(lpp->core_rst);

		/* Some SoC core reset also reset PHY, more delay needed */
		mdelay(1);
	}
}

static void intel_pcie_device_rst_assert(struct intel_pcie_port *lpp)
{
	if (lpp->reset_gpio)
		gpiod_set_value_cansleep(lpp->reset_gpio, 1);

	if (lpp->rcu_rst)
		reset_control_assert(lpp->rcu_rst);
}

static void intel_pcie_device_rst_deassert(struct intel_pcie_port *lpp)
{
	msleep(lpp->rst_interval);

	if (lpp->reset_gpio)
		gpiod_set_value_cansleep(lpp->reset_gpio, 0);

	if (lpp->rcu_rst)
		reset_control_deassert(lpp->rcu_rst);
}

static int intel_pcie_enable_clks(struct intel_pcie_port *lpp)
{
	int err;

	err = clk_prepare_enable(lpp->core_clk);
	if (err < 0) {
		dev_err(lpp->dev, "failed to enable core clock: %d\n", err);
		return err;
	}
	return 0;
}

static int intel_pcie_app_loigc_setup(struct intel_pcie_port *lpp)
{
	intel_pcie_device_rst_deassert(lpp);

	intel_pcie_ltssm_enable(lpp);
	/* Check link up status after enabling LTSSM */
	if (intel_pcie_wait_phy_link_up(lpp) != 0)
		return -ETIMEDOUT;
	return 0;
}

#ifdef CONFIG_PCIE_INTEL_MSI_PIC
static void intel_pcie_msi_pic_init(struct intel_pcie_port *lpp)
{
	int i;
	int irq_num;
	struct platform_device *pdev = to_platform_device(lpp->dev);
	struct msi_irq *lmsi = &lpp->msi_irqs;

	spin_lock_init(&lmsi->msi_lock);
	lmsi->msi_phy_base = lpp->msi_base;
	lmsi->msi_free_irq_bitmask = 0;
	lmsi->msi_multiple_irq_bitmask = 0;
	irq_num = platform_irq_count(pdev);
	irq_num--; /* last one is ir interrupt */
	lpp->msr_irq_nrs = irq_num;
	for (i = 0; i < lpp->msr_irq_nrs; i++) {
		lmsi->msi_irq_idx[i].irq = platform_get_irq(pdev, i);
		lmsi->msi_irq_idx[i].idx = i;
	}

	spin_lock(&lmsi->msi_lock);
	if (lpp->inbound_swap)
		pcie_msi_wr(lpp, MSI_PIC_BIG_ENDIAN, MSI_IRQ_PIC_TABLE_SZ);
	else
		pcie_msi_wr(lpp, MSI_PIC_LITTLE_ENDIAN, MSI_IRQ_PIC_TABLE_SZ);

	spin_unlock(&lmsi->msi_lock);
}

static void intel_pcie_msi_init(struct intel_pcie_port *lpp)
{
	intel_pcie_msi_pic_init(lpp);
}

static void intel_msi_unmask_irq(struct msi_desc *desc, u32 mask)
{
	u32 mask_bits = desc->masked;

	if (!desc->msi_attrib.maskbit)
		return;
	mask_bits &= ~mask;
	pci_write_config_dword(to_pci_dev(desc->dev), desc->mask_pos,
			       mask_bits);
}

static int intel_setup_msi_irq(struct pci_dev *pdev,
			       struct msi_desc *desc, int nvec)
{
	int i, irq_base, pos;
	u32 val;
	u16 control;
	int irq_idx;
	int irq_step;
	int configured_private_bits;
	int request_private_bits;
	struct msi_msg msg;
	u16 search_mask;
	u16 msi_base_data;
	struct msi_irq *lmsi;
	u32 msi_mask;
	struct intel_pcie_port *lpp = pdev->bus->sysdata;
	struct device *dev = lpp->dev;

	lmsi = &lpp->msi_irqs;
	WARN_ON(nvec <= 0);
	dev_dbg(dev, "%s %s port %d enter\n",
		__func__, pci_name(pdev), lpp->id);
	/*
	 * Skip RC and switch ports since we have limited
	 * interrupt resource available
	 */
	if (pci_pcie_type(pdev) != PCI_EXP_TYPE_ENDPOINT) {
		dev_dbg(dev,
			"%s RC %d or Switch Port doesn't use MSI interrupt\n",
			__func__, lpp->id);
		return -EINVAL;
	}
	/*
	 * Read the MSI config to figure out how many IRQs this device
	 * wants.  Most devices only want 1, which will give
	 * configured_private_bits and request_private_bits equal 0.
	 */
	pci_read_config_word(pdev, pdev->msi_cap + PCI_MSI_FLAGS, &control);

	/*
	 * If the number of private bits has been configured then use
	 * that value instead of the requested number. This gives the
	 * driver the chance to override the number of interrupts
	 * before calling pci_enable_msi().
	 */
	configured_private_bits = (control & PCI_MSI_FLAGS_QSIZE) >> 4;

	if (configured_private_bits == 0) {
		/* Nothing is configured, so use the hardware requested size */
		request_private_bits = (control & PCI_MSI_FLAGS_QMASK) >> 1;
	} else {
		/*
		 * Use the number of configured bits, assuming the
		 * driver wanted to override the hardware request
		 * value.
		 */
		request_private_bits = configured_private_bits;
	}

	/*
	 * The PCI 2.3 spec mandates that there are at most 32
	 * interrupts. If this device asks for more, only give it one.
	 */
	if (request_private_bits > 5)
		request_private_bits = 0;
again:
	/*
	 * The IRQs have to be aligned on a power of two based on the
	 * number being requested.
	 */
	irq_step = (1 << request_private_bits);

	/* NB: Pick up the minimun one for usage */
	irq_step = min(irq_step, nvec);

	msi_mask = (1 << (1 << request_private_bits)) - 1;
	/* If device supports more than RC supported, fall back to single MSI */
	if (irq_step > lpp->msr_irq_nrs) {
		irq_step = 1;
		request_private_bits = 0;
		dev_info(dev, "Requested interrupts more than RC supported, fall back to single MSI!!!\n");
	}

	/* Mask with one bit for each IRQ */
	search_mask = (1 << irq_step) - 1;

	/*
	 * We're going to search msi_free_irq_bitmask_lock for zero
	 * bits. This represents an MSI interrupt number that isn't in
	 * use.
	 */
	spin_lock(&lmsi->msi_lock);
	for (pos = 0; pos < MSI_IRQ_PIC_TABLE_SZ; pos += irq_step) {
		if ((lmsi->msi_free_irq_bitmask & (search_mask << pos)) == 0) {
			lmsi->msi_free_irq_bitmask |= search_mask << pos;
			lmsi->msi_multiple_irq_bitmask |=
				(search_mask >> 1) << pos;
			break;
		}
	}
	spin_unlock(&lmsi->msi_lock);

	/* Make sure the search for available interrupts didn't fail */
	if (pos >= MSI_IRQ_PIC_TABLE_SZ) {
		if (request_private_bits) {
			dev_dbg(dev, "%s: Unable to find %d free interrupts, trying just one",
				__func__,
				1 << request_private_bits);
			request_private_bits = 0;
			goto again;
		} else {
			dev_err(dev, "%s: Unable to find a free MSI interrupt\n",
				__func__);
			return -EINVAL;
		}
	}

	/* Only assign the base irq to msi entry */
	irq_base = lmsi->msi_irq_idx[pos].irq;
	irq_idx = lmsi->msi_irq_idx[pos].idx;

	dev_dbg(dev, "pos %d, irq_base %d irq_setup %d irq_idx %d\n",
		pos, irq_base, irq_step, irq_idx);

	/*
	 * Initialize MSI. This has to match the memory-write endianness
	 * from the device Address bits [23:12]
	 * For multiple MSI, we have to assign and enable sequence MSI data
	 * Make sure that base data lower bits as zero since multiple MSI
	 * just modify lower several bits to generate different interrupts
	 * pos is used for multiple MSI with multiple devices on the same
	 * PCIe port.
	 */
	msi_base_data = ((1 << pos) + PCIE_MSI_BASE_DATA) & (~search_mask);
	spin_lock(&lmsi->msi_lock);
	for (i = 0; i < irq_step; i++) {
		val = (((irq_idx + i) % lpp->msr_irq_nrs) << MSI_PIC_INT_LINE_S)
			| (((lmsi->msi_phy_base & MSI_MSG_ADDR_MASK) >> 12)
			<< MSI_PIC_MSG_ADDR_S)
			| (((msi_base_data + i) & MSI_PIC_MSG_DATA)
			<< MSI_PIC_MSG_DATA_S);
		pcie_msi_wr(lpp, val, pos + i);
		/* Enable this entry */
		pcie_msi_wr_mask(lpp, MSI_PCI_INT_DISABLE, 0, pos + i);
		dev_dbg(dev, "pic_table[%d]: 0x%08x\n",
			(pos + i), pcie_msi_rd(lpp, pos + i));
	}
	spin_unlock(&lmsi->msi_lock);

	/* Update multiple MSI property */
	desc->msi_attrib.multiple = request_private_bits;
	/* Assign base irq and base data to the first MSI entry */
	irq_set_msi_desc(irq_base, desc);
	msg.address_hi = 0x0;
	msg.address_lo = lmsi->msi_phy_base;
	msg.data = (msi_base_data << MSI_PIC_MSG_DATA_S);
	dev_dbg(dev,
		"base msi_data: pos %d data 0x%08x irq %d-%d\n",
		pos, msg.data, irq_base, irq_base + irq_step - 1);

	pci_write_msi_msg(irq_base, &msg);

	/* Unmask optional per-vector MSI, some advanced card supported */
	intel_msi_unmask_irq(desc, msi_mask);

	dev_dbg(dev, "%s port %d exit\n", __func__, lpp->id);
	return 0;
}

static int intel_setup_msi_irqs(struct msi_controller *ctr, struct pci_dev *dev,
				int nvec, int type)
{
	struct msi_desc *entry;
	int ret;

	/* MSI-X is not supported */
	if (type == PCI_CAP_ID_MSIX)
		return -EINVAL;

	for_each_pci_msi_entry(entry, dev) {
		ret = intel_setup_msi_irq(dev, entry, nvec);
		if (ret < 0)
			return ret;
		if (ret > 0)
			return -ENOSPC;
	}
	return 0;
}

static void intel_teardown_msi_irqs(struct msi_controller *ctr,
				    struct pci_dev *pdev)
{
	int i;
	int pos;
	int irq;
	int number_irqs;
	u16 bitmask;
	struct msi_desc *desc;
	struct intel_pcie_port *lpp;
	struct msi_irq *lmsi;
	struct device *dev;

	lpp = pdev->bus->sysdata;
	lmsi = &lpp->msi_irqs;
	dev = lpp->dev;
	/*
	 * Skip RC and switch ports since we have limited
	 * interrupt resource available
	 */
	if (pci_pcie_type(pdev) != PCI_EXP_TYPE_ENDPOINT) {
		dev_dbg(dev, "%s RC %d or Switch Port doesn't use MSI interrupt\n",
			__func__, lpp->id);
		return;
	}

	WARN_ON(list_empty(dev_to_msi_list(&pdev->dev)));
	desc = first_msi_entry(&pdev->dev);

	/* NB pdev->irq has been restored, retrieve msi irq number */
	irq = desc->irq;

	dev_dbg(dev, "%s port %d irq %d enter\n", __func__, lpp->id, irq);

	if (irq == 0)
		return;
	/*
	 * Shift the mask to the correct bit location, not always correct
	 * Probally, the first match will be chosen.
	 */
	for (pos = 0; pos < MSI_IRQ_PIC_TABLE_SZ; pos++) {
		if (lmsi->msi_irq_idx[pos].irq == irq &&
		    (lmsi->msi_free_irq_bitmask & (1 << pos)))
			break;
	}

	if (pos >= MSI_IRQ_PIC_TABLE_SZ) {
		dev_err(dev, "%s: Unable to find a matched MSI interrupt %d\n",
			__func__, irq);
		return;
	}
	/*
	 * Count the number of IRQs we need to free by looking at the
	 * msi_multiple_irq_bitmask. Each bit set means that the next
	 * IRQ is also owned by this device.
	 */
	number_irqs = 0;
	while (((pos + number_irqs) < MSI_IRQ_PIC_TABLE_SZ) &&
	       (lmsi->msi_multiple_irq_bitmask
		& (1 << (pos + number_irqs))))
		number_irqs++;

	number_irqs++;

	/* Disable entries if multiple MSI  */
	spin_lock(&lmsi->msi_lock);
	for (i = 0; i < number_irqs; i++) {
		pcie_msi_wr_mask(lpp, (MSI_PIC_INT_LINE | MSI_MSG_ADDR_MASK |
			MSI_PIC_MSG_DATA), MSI_PCI_INT_DISABLE, pos + i);
	}
	spin_unlock(&lmsi->msi_lock);

	/* Mask with one bit for each IRQ */
	bitmask = (1 << number_irqs) - 1;

	bitmask <<= pos;
	if ((lmsi->msi_free_irq_bitmask & bitmask) != bitmask) {
		dev_err(dev, "%s: Attempted to teardown MSI interrupt %d not in use\n",
			__func__, irq);
		return;
	}

	/* Checks are done, update the in use bitmask */
	spin_lock(&lmsi->msi_lock);
	lmsi->msi_free_irq_bitmask &= ~bitmask;
	lmsi->msi_multiple_irq_bitmask &= ~(bitmask >> 1);
	spin_unlock(&lmsi->msi_lock);

	dev_dbg(dev, "%s port %d exit\n", __func__, lpp->id);
}

struct msi_controller intel_msi_chip = {
	.setup_irqs = intel_setup_msi_irqs,
};

void arch_teardown_msi_irqs(struct pci_dev *dev)
{
	struct msi_controller *chip = dev->bus->msi;

	if (chip)
		return intel_teardown_msi_irqs(chip, dev);
}
#endif /* CONFIG_PCIE_INTEL_MSI_PIC */

static irqreturn_t intel_pcie_core_isr(int irq, void *arg)
{
	u32 reg;
	struct intel_pcie_port *lpp = arg;
	struct device *dev = lpp->dev;

	reg = pcie_app_rd(lpp, PCIE_IRNCR);

	if (reg & PCIE_IRN_AER_REPORT)
		dev_dbg(dev, "AER interrupt received\n");

	if (reg & PCIE_IRN_PME)
		dev_dbg(dev, "Power Management Event received\n");

	if (reg & PCIE_IRN_HOTPLUG)
		dev_dbg(dev, "Hotplug interrupt received\n");

	if (reg & PCIE_IRN_RX_VDM_MSG)
		dev_dbg(dev, "Vendor Message received\n");

	if (reg & PCIE_IRN_PM_TO_ACK)
		dev_dbg(dev, "PM ack interrupt recevied\n");

	if (reg & PCIE_IRN_PM_TURNOFF_ACK)
		dev_dbg(dev, "PM turn off ack interrupt received\n");

	if (reg & PCIE_IRN_LINK_AUTO_BW_STATUS)
		dev_dbg(dev, "Link auto bandwidth interrupt received\n");

	if (reg & PCIE_IRN_BW_MGT)
		dev_dbg(dev, "Bandwidth management interrupt received\n");

	if (reg & PCIE_IRN_WAKEUP)
		dev_dbg(dev, "Wake up interrupt received\n");

	if (reg & PCIE_IRN_MSG_LTR)
		dev_dbg(dev, "Long tolerance interrupt received\n");

	if (reg & PCIE_IRN_SYS_ERR_RC)
		dev_dbg(dev, "System err rc interrupt received\n");

	if (reg & PCIE_IRN_SYS_INT)
		dev_dbg(dev, "System err system received\n");

	pcie_app_wr(lpp, reg & PCIE_IRN_IR_INT, PCIE_IRNCR);

	return IRQ_HANDLED;
}

static int intel_pcie_setup_irq(struct intel_pcie_port *lpp)
{
	int irq, err;
	struct device *dev = lpp->dev;
	struct platform_device *pdev;

	pdev = to_platform_device(dev);
	irq = platform_get_irq_byname(pdev, "ir");
	if (irq < 0) {
		dev_err(dev, "missing sys integrated irq resource\n");
		return irq;
	}
	snprintf(&lpp->irq_name[0], sizeof(lpp->irq_name) - 1,
		 "pcie_ir%d", lpp->id);
	lpp->plat_irq = irq;
	err = devm_request_irq(dev, irq, intel_pcie_core_isr,
			       IRQF_SHARED, lpp->irq_name, lpp);
	if (err) {
		dev_err(dev, "%s request irq %d failed\n",
			__func__, irq);
		return err;
	}
	/* Enable integrated interrupts */
	pcie_app_wr_mask(lpp, 0, PCIE_IRN_IR_INT, PCIE_IRNEN);
	return 0;
}

static void intel_pcie_core_irq_disable(struct intel_pcie_port *lpp)
{
	pcie_app_wr(lpp, 0, PCIE_IRNEN);

	/* Clear it first */
	pcie_app_wr_mask(lpp, PCIE_IRN_IR_INT, PCIE_IRN_IR_INT,  PCIE_IRNCR);
	dev_dbg(lpp->dev, "%s PCIE_IRNCR: 0x%08x\n",
		__func__, pcie_app_rd(lpp, PCIE_IRNCR));
}

static int intel_pcie_clks_get(struct intel_pcie_port *lpp)
{
	lpp->core_clk = devm_clk_get(lpp->dev, "ctl");
	if (IS_ERR(lpp->core_clk))
		return PTR_ERR(lpp->core_clk);

	return 0;
}

static int intel_pcie_resets_get(struct intel_pcie_port *lpp)
{
	int ret;

	lpp->core_rst = devm_reset_control_get_optional(lpp->dev, "core");
	if (IS_ERR(lpp->core_rst)) {
		ret = PTR_ERR(lpp->core_rst);
		if (ret == -ENOENT) {
			lpp->core_rst = NULL;
		} else {
			dev_err(lpp->dev, "Failed to get reset control %d\n",
				ret);
			return ret;
		}
	}
	return 0;
}

static int intel_pcie_parse_dt(struct intel_pcie_port *lpp)
{
	int ret;
	u32 prop;
	struct resource *res;
	struct device *dev = lpp->dev;
	struct device_node *np = dev->of_node;
	struct platform_device *pdev;
	const char *type;

	type = of_get_property(np, "device_type", NULL);
	if (!type || strcmp(type, "pci")) {
		dev_err(dev, "invalid \"device_type\" %s\n", type);
		return -EINVAL;
	}
	pdev = to_platform_device(lpp->dev);

	if (!device_property_read_u32(dev, "intel,rst-interval", &prop))
		lpp->rst_interval = prop;

	if (!device_property_read_u32(dev, "intel,inbound-swap", &prop))
		lpp->inbound_swap = prop;

	if (!device_property_read_u32(dev, "intel,outbound-swap", &prop))
		lpp->outbound_swap = prop;

	dev_dbg(dev,
		"rst_interval %d in_swap %d, out_swap %d\n",
		lpp->rst_interval, lpp->inbound_swap, lpp->outbound_swap);

	if (!device_property_read_u32(dev, "intel,inbound-shift", &prop))
		lpp->inbound_shift = prop;

	if (!device_property_read_u32(dev, "intel,outbound-shift", &prop))
		lpp->outbound_shift = prop;

	if (!device_property_read_u32(dev, "intel,iatu", &prop))
		lpp->has_iatu = prop ? true : false;

	ret = device_property_read_u32(dev, "max-link-speed", &prop);
	if (prop > 4)
		prop = 4;
	lpp->link_gen = ret < 0 ? 0 : prop;

	lpp->lanes = 1;
	ret = device_property_read_u32(dev, "num-lanes", &lpp->lanes);
	if (!ret && (lpp->lanes == 0 ||
		     lpp->lanes > 2)) {
		dev_warn(dev, "invalid num-lanes, default to use one lane\n");
		lpp->lanes = 1;
	}

	dev_dbg(dev,
		"in-shift %d out-shift %d speed %d lanes %d iatu %s\n",
		lpp->inbound_shift, lpp->outbound_shift, lpp->link_gen,
		lpp->lanes, lpp->has_iatu ? "enabled" : "disabled");

	lpp->syscfg = syscon_regmap_lookup_by_phandle(np, "intel,syscon");
	if (IS_ERR(lpp->syscfg)) {
		dev_err(dev, "No syscon phandle specified for pcie syscon\n");
		lpp->syscfg = NULL;
		return -EINVAL;
	}

	/* Controller Cfg Register */
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "csr");
	if (!res)
		return -ENXIO;

	lpp->dbi_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(lpp->dbi_base))
		return PTR_ERR(lpp->dbi_base);

	dev_dbg(dev, "%s rc %d csr %p\n", __func__, lpp->id, lpp->dbi_base);

	/* Device cfg space */
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "cfg");
	if (!res)
		return -ENXIO;

	if (lpp->has_iatu) {
		lpp->cfg0_size = resource_size(res) / 2;
		lpp->cfg1_size = resource_size(res) / 2;
		lpp->cfg0_phy_base = res->start;
		lpp->cfg1_phy_base = res->start + lpp->cfg0_size;
		lpp->cfg0_base = devm_ioremap(lpp->dev, lpp->cfg0_phy_base,
					      lpp->cfg0_size);
		if (IS_ERR(lpp->cfg0_base)) {
			dev_err(lpp->dev, "cfg0 base ioremap error\n");
			return -ENOMEM;
		}
		dev_dbg(dev, "%s cfg0 %p size %x\n", __func__,
			lpp->cfg0_base, lpp->cfg0_size);
		lpp->cfg1_base = devm_ioremap(lpp->dev, lpp->cfg1_phy_base,
					      lpp->cfg1_size);
		if (IS_ERR(lpp->cfg1_base)) {
			dev_err(lpp->dev, "cfg1 base ioremap error\n");
			return -ENOMEM;
		}
		dev_dbg(dev, "%s cfg1 %p size %x\n", __func__,
			lpp->cfg1_base, lpp->cfg1_size);
	} else {
		lpp->cfg0_size = resource_size(res);
		lpp->cfg0_phy_base = res->start;
		lpp->cfg0_base = devm_ioremap_resource(dev, res);
		if (IS_ERR(lpp->cfg0_base))
			return -ENOMEM;
		dev_dbg(dev, "%s cfg %p size %x\n", __func__,
			lpp->cfg0_base, lpp->cfg0_size);
	}
	/* Applicaton logic */
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "app");
	if (!res)
		return -ENXIO;

	lpp->app_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(lpp->app_base))
		return PTR_ERR(lpp->app_base);

	dev_dbg(dev, "%s rc app_base %p\n", __func__, lpp->app_base);

#ifdef CONFIG_PCIE_INTEL_MSI_PIC
	/* msi physical space */
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "msi");
	if (!res)
		return -ENXIO;

	lpp->msi_base = res->start;
	dev_dbg(dev, "%s rc msi_base 0x%08x\n", __func__, (u32)lpp->msi_base);

	/* msi pic base */
	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "pic");
	if (!res)
		return -ENXIO;

	lpp->msi_pic = devm_ioremap_resource(dev, res);
	if (IS_ERR(lpp->msi_pic))
		return PTR_ERR(lpp->msi_pic);
	dev_dbg(dev, "%s rc msi_pic %p\n", __func__, lpp->msi_pic);
#endif /* CONFIG_PCIE_INTEL_MSI_PIC */

	lpp->phy = devm_phy_get(dev, "phy");
	if (IS_ERR(lpp->phy)) {
		ret = PTR_ERR(lpp->phy);
		if (ret == -EPROBE_DEFER)
			dev_info(dev, "probe deferred\n");
		else
			dev_err(dev, "couldn't get pcie-phy!\n");
		return ret;
	}

	return 0;
}

static int intel_pcie_get_resources(struct platform_device *pdev)
{
	int err;
	struct device *dev;
	struct intel_pcie_port *lpp;

	lpp = platform_get_drvdata(pdev);
	dev = lpp->dev;

	err = intel_pcie_clks_get(lpp);
	if (err) {
		dev_err(dev, "failed to get clks: %d\n", err);
		return err;
	}

	err = intel_pcie_resets_get(lpp);
	if (err) {
		dev_err(dev, "failed to get resets: %d\n", err);
		return err;
	}

	return intel_pcie_parse_dt(lpp);
}

static int intel_pcie_host_setup(struct intel_pcie_port *lpp)
{
	int ret;

	pm_runtime_enable(lpp->dev);
	pm_runtime_get_sync(lpp->dev);

	intel_pcie_core_rst_assert(lpp);

	intel_pcie_device_rst_assert(lpp);

	intel_pcie_core_rst_deassert(lpp);

	intel_pcie_enable_clks(lpp);

	if (lpp->phy) {
		if (phy_init(lpp->phy))
			return -ENODEV;
	}

#ifdef CONFIG_PCIE_INTEL_MSI_PIC
	intel_pcie_msi_init(lpp);
#endif
	intel_pcie_rc_setup(lpp);
	ret = intel_pcie_app_loigc_setup(lpp);
	if (ret)
		return ret;

	return intel_pcie_setup_irq(lpp);
}

static ssize_t
pcie_link_status_show(struct device *dev, struct device_attribute *attr,
		      char *buf)
{
	u32 reg, width, gen;
	struct intel_pcie_port *lpp;

	lpp = dev_get_drvdata(dev);

	reg = pcie_rc_cfg_rd(lpp, PCIE_LCTLSTS);
	width = (reg & PCIE_LCTLSTS_NEGOTIATED_LINK_WIDTH) >>
		PCIE_LCTLSTS_NEGOTIATED_LINK_WIDTH_S;
	gen = (reg & PCIE_LCTLSTS_LINK_SPEED) >> PCIE_LCTLSTS_LINK_SPEED_S;
	if (gen > lpp->max_speed)
		return -EINVAL;
	return sprintf(buf, "Port %2u Width x%u Speed %s GT/s\n", lpp->id,
		       width, pcie_link_gen_to_str(gen));
}
static DEVICE_ATTR_RO(pcie_link_status);

static ssize_t pcie_speed_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t len)
{
	unsigned long val;
	struct intel_pcie_port *lpp;

	lpp = dev_get_drvdata(dev);

	if (kstrtoul(buf, 10, &val))
		return -EINVAL;

	if (val > lpp->max_speed)
		return -EINVAL;

	lpp->link_gen = val;
	intel_pcie_max_speed_setup(lpp);
	intel_pcie_speed_change_disable(lpp);
	intel_pcie_speed_change_enable(lpp);
	return len;
}
static DEVICE_ATTR_WO(pcie_speed);

/*
 * Link width change on the fly is not always successful.
 * It also depends on your partner.
 */
static ssize_t pcie_width_store(struct device *dev,
				struct device_attribute *attr,
				const char *buf, size_t len)
{
	unsigned long val;
	struct intel_pcie_port *lpp;

	lpp = dev_get_drvdata(dev);

	if (kstrtoul(buf, 10, &val))
		return -EINVAL;

	if (val > lpp->max_width)
		return -EINVAL;
	lpp->lanes = val;
	intel_pcie_max_link_width_setup(lpp);
	return len;
}
static DEVICE_ATTR_WO(pcie_width);

static struct attribute *pcie_cfg_attrs[] = {
	&dev_attr_pcie_link_status.attr,
	&dev_attr_pcie_speed.attr,
	&dev_attr_pcie_width.attr,
	NULL,
};
ATTRIBUTE_GROUPS(pcie_cfg);

static int intel_pcie_sysfs_init(struct intel_pcie_port *lpp)
{
	return sysfs_create_groups(&lpp->dev->kobj, pcie_cfg_groups);
}

static void intel_pcie_sysfs_deinit(struct intel_pcie_port *lpp)
{
	sysfs_remove_groups(&lpp->dev->kobj, pcie_cfg_groups);
}

static int intel_pcie_add_controller(struct intel_pcie_port *lpp)
{
	mutex_lock(&intel_pcie_lock);
	list_add_tail(&lpp->list, &intel_pcie_list);
	mutex_unlock(&intel_pcie_lock);
	return 0;
}

static void intel_pcie_remove_controller(struct intel_pcie_port *lpp)
{
	struct list_head *pos, *q;
	struct intel_pcie_port *tmp;

	mutex_lock(&intel_pcie_lock);
	list_for_each_safe(pos, q, &intel_pcie_list) {
		tmp = list_entry(pos, struct intel_pcie_port, list);
		if (tmp == lpp) {
			list_del(pos);
			break;
		}
	}
	mutex_unlock(&intel_pcie_lock);
}

static void intel_pcie_deinit_phy(struct intel_pcie_port *lpp)
{
	if (lpp->phy)
		phy_exit(lpp->phy);
}

static void intel_pcie_disable_clks(struct intel_pcie_port *lpp)
{
	clk_disable_unprepare(lpp->core_clk);
}

static int intel_pcie_wait_l2(struct intel_pcie_port *lpp)
{
	int err;
	u32 value;

	if (lpp->max_speed < PCIE_LINK_SPEED_GEN3)
		return 0;

	/* Send PME_TURN_OFF message */
	pcie_app_wr_mask(lpp, 0, PCIE_XMT_PM_TURNOFF, PCIE_MSG_CR);

	/* Read PMC status and wait for falling into L2 link state */
	err = readl_poll_timeout(lpp->app_base + PCIE_PMC, value,
				 (value & PCIE_PM_IN_L2), 20,
				 jiffies_to_usecs(5 * HZ));
	if (err) {
		dev_err(lpp->dev, "PCIe link enter L2 timeout!\n");
		return err;
	}

	return 0;
}

static void intel_pcie_turn_off(struct intel_pcie_port *lpp)
{
	if (intel_pcie_link_up(lpp))
		intel_pcie_wait_l2(lpp);

	/* Put EP in reset state */
	intel_pcie_device_rst_assert(lpp);
}

static void __intel_pcie_remove(struct intel_pcie_port *lpp)
{
	intel_pcie_core_irq_disable(lpp);
	intel_pcie_turn_off(lpp);
	intel_pcie_disable_clks(lpp);
	intel_pcie_core_rst_assert(lpp);
	intel_pcie_sysfs_deinit(lpp);

	pm_runtime_put_sync(lpp->dev);
	pm_runtime_disable(lpp->dev);

	intel_pcie_deinit_phy(lpp);
	intel_pcie_remove_controller(lpp);
}

static int intel_pcie_remove(struct platform_device *pdev)
{
	struct intel_pcie_port *lpp = platform_get_drvdata(pdev);

	pci_stop_root_bus(lpp->root_bus);
	pci_remove_root_bus(lpp->root_bus);
	__intel_pcie_remove(lpp);

	return 0;
}

static void intel_pcie_shutdown(struct platform_device *pdev)
{
	struct intel_pcie_port *lpp = platform_get_drvdata(pdev);

	__intel_pcie_remove(lpp);
}

#ifdef CONFIG_PM_SLEEP
static int intel_pcie_suspend(struct device *dev)
{
	struct intel_pcie_port *lpp = dev_get_drvdata(dev);

	pcie_rc_cfg_wr_mask(lpp, PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
			    PCI_COMMAND_MASTER, 0, PCI_COMMAND);
	return 0;
}

static int intel_pcie_resume(struct device *dev)
{
	struct intel_pcie_port *lpp = dev_get_drvdata(dev);

	pcie_rc_cfg_wr_mask(lpp, 0, PCI_COMMAND_IO | PCI_COMMAND_MEMORY |
			    PCI_COMMAND_MASTER, PCI_COMMAND);
	return 0;
}

static int __maybe_unused intel_pcie_suspend_noirq(struct device *dev)
{
	int ret;
	struct intel_pcie_port *lpp = dev_get_drvdata(dev);

	intel_pcie_core_irq_disable(lpp);

	ret = intel_pcie_wait_l2(lpp);
	if (ret)
		return ret;

	intel_pcie_deinit_phy(lpp);

	intel_pcie_disable_clks(lpp);

	return ret;
}

static int __maybe_unused intel_pcie_resume_noirq(struct device *dev)
{
	int err;
	struct intel_pcie_port *lpp = dev_get_drvdata(dev);

	err = intel_pcie_host_setup(lpp);
	if (err)
		return err;
	return 0;
}
#endif /* CONFIG_PM_SLEEP */

static const struct of_device_id of_intel_pcie_match[] = {
	{ .compatible = "intel,xrx500-pcie" },
	{ .compatible = "intel,prx300-pcie" },
	{}
};

static int intel_pcie_probe(struct platform_device *pdev)
{
	int ret;
	int id;
	struct device *dev = &pdev->dev;
	struct intel_pcie_port *lpp;
	struct pci_bus *bus, *child;
	struct device_node *np = dev->of_node;
	struct resource_entry *win, *tmp;
	LIST_HEAD(res);

	if (!np)
		return -ENODEV;

	lpp = devm_kzalloc(dev, sizeof(*lpp), GFP_KERNEL);
	if (!lpp)
		return -ENOMEM;

	id = of_alias_get_id(np, "pcie");
	if (id < 0) {
		dev_err(dev, "failed to get domain id, errno %d\n", id);
		return id;
	}

	lpp->id = id;

	lpp->dev = dev;

	spin_lock_init(&lpp->lock);

	platform_set_drvdata(pdev, lpp);

	ret = intel_pcie_get_resources(pdev);
	if (ret)
		return ret;

	/* Inputs from dts for endian and gpio reset */
	intel_pcie_endian_setup(lpp);

	ret = intel_pcie_ep_rst_init(lpp);
	if (ret)
		return ret;

	ret = of_pci_get_host_bridge_resources(dev->of_node, 0, 0xff, &res,
					       &lpp->io_base);
	if (ret) {
		dev_err(dev, "Getting bridge resources failed\n");
		return ret;
	}

	ret = devm_request_pci_bus_resources(dev, &res);
	if (ret)
		goto err_res;

	/* Get the memory ranges from DT */
	resource_list_for_each_entry_safe(win, tmp, &res) {
		switch (resource_type(win->res)) {
		case IORESOURCE_IO:
			lpp->io = win->res;
			lpp->io->name = "I/O";
			lpp->io_size = resource_size(lpp->io);
			lpp->io_bus_addr = lpp->io->start - win->offset;
			break;
		case IORESOURCE_MEM:
			lpp->mem = win->res;
			lpp->mem->name = "MEM";
			lpp->mem_size = resource_size(lpp->mem);
			lpp->mem_bus_addr = lpp->mem->start - win->offset;
			lpp->mem_base = lpp->mem->start;
			break;
		case IORESOURCE_BUS:
			lpp->root_bus_nr = win->res->start;
			break;
		}
	}

	intel_pcie_add_controller(lpp);

	/* RC/host related initialization */
	ret = intel_pcie_host_setup(lpp);
	if (ret)
		goto err_host;

	intel_pcie_sysfs_init(lpp);
#ifdef CONFIG_PCIE_INTEL_MSI_PIC
	bus = pci_scan_root_bus_msi(dev, lpp->root_bus_nr,
				    &intel_pcie_ops, lpp, &res,
				    &intel_msi_chip);
	intel_msi_chip.dev = dev;
#else
	bus = pci_scan_root_bus(dev, lpp->root_bus_nr, &intel_pcie_ops,
				lpp, &res);
#endif /* CONFIG_PCIE_INTEL_MSI_PIC */
	if (!bus) {
		ret = -ENOMEM;
		goto err_host;
	}

	/* Legacy interrupt */
	pci_fixup_irqs(pci_common_swizzle, intel_pcie_bios_map_irq);

	pci_assign_unassigned_bus_resources(bus);

	list_for_each_entry(child, &bus->children, node)
		pcie_bus_configure_settings(child);
	pci_bus_add_devices(bus);
	lpp->root_bus = bus;
	dev_info(dev,
		 "Intel AXI PCIe Root Complex Port %d Init Done\n", lpp->id);
	return 0;
err_host:
	__intel_pcie_remove(lpp);
err_res:
	pci_free_resource_list(&res);
	return ret;
}

static const struct dev_pm_ops intel_pcie_pm_ops = {
	SET_NOIRQ_SYSTEM_SLEEP_PM_OPS(intel_pcie_suspend, intel_pcie_resume)
};

static struct platform_driver intel_pcie_driver = {
	.probe = intel_pcie_probe,
	.remove = intel_pcie_remove,
	.shutdown = intel_pcie_shutdown,
	.driver = {
		.name = "intel-pcie",
		.of_match_table = of_match_ptr(of_intel_pcie_match),
		.pm = &intel_pcie_pm_ops,
	},
};

builtin_platform_driver(intel_pcie_driver);
