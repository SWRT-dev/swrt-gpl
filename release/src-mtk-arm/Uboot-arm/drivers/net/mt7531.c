#include <common.h>
#include <miiphy.h>
#include <errno.h>

#include "mt7531.h"

#define BIT(x)				((1 << x))

#define MT7531_NUM_PORTS		7
#define MT7531_NUM_PHYS			5

/* Access to MT7531 registers */
#define MT7531_REG_PAGE_ADDR_S		6
#define MT7531_REG_PAGE_ADDR_M		0xffc0
#define MT7531_REG_ADDR_S		2
#define MT7531_REG_ADDR_M		0x3c

#define MT7531_SMI_ADDR			0x1f
#define MT753X_SMI_ADDR_MASK		0x1f
#define MT7531_PHY_ADDR_BASE		(MT7531_SMI_ADDR + 1)

/* MDIO_CMD: MDIO commands */
#define MDIO_CMD_ADDR			0
#define MDIO_CMD_WRITE			1
#define MDIO_CMD_READ			2
#define MDIO_CMD_READ_C45		3

/* MDIO_ST: MDIO start field */
#define MDIO_ST_C45			0
#define MDIO_ST_C22			1

/* MT7531 registers */
#define PORT_MAC_CTRL_BASE		0x3000
#define PORT_MAC_CTRL_PORT_OFFSET	0x100
#define PORT_MAC_CTRL_REG(p, r)		(PORT_MAC_CTRL_BASE + \
					(p) * PORT_MAC_CTRL_PORT_OFFSET + (r))
#define PMCR(p)				PORT_MAC_CTRL_REG(p, 0x00)

#define GMACCR				(PORT_MAC_CTRL_BASE + 0xe0)

/* Fields of PMCR */
#define FORCE_MODE_EEE1G		BIT(25)
#define FORCE_MODE_EEE100		BIT(26)
#define FORCE_MODE_TX_FC		BIT(27)
#define FORCE_MODE_RX_FC		BIT(28)
#define FORCE_MODE_DPX			BIT(29)
#define FORCE_MODE_SPD			BIT(30)
#define FORCE_MODE_LNK			BIT(31)
#define IPG_CFG_S			18
#define IPG_CFG_M			0xc0000
#define EXT_PHY				BIT(17)
#define MAC_MODE			BIT(16)
#define FORCE_MODE			BIT(15)
#define MAC_TX_EN			BIT(14)
#define MAC_RX_EN			BIT(13)
#define MAC_PRE				BIT(11)
#define BKOFF_EN			BIT(9)
#define BACKPR_EN			BIT(8)
#define FORCE_EEE1G			BIT(7)
#define FORCE_EEE1000			BIT(6)
#define FORCE_RX_FC			BIT(5)
#define FORCE_TX_FC			BIT(4)
#define FORCE_SPD_S			2
#define FORCE_SPD_M			0x0c
#define FORCE_DPX			BIT(1)
#define FORCE_LINK			BIT(0)

/* Values of IPG_CFG */
#define IPG_96BIT			0
#define IPG_96BIT_WITH_SHORT_IPG	1
#define IPG_64BIT			2

/* Fields of GMACCR */
#define TXCRC_EN			BIT(19)
#define RXCRC_EN			BIT(18)
#define PRMBL_LMT_EN			BIT(17)
#define MTCC_LMT_S			9
#define MTCC_LMT_M			0x1e00
#define MAX_RX_JUMBO_S			2
#define MAX_RX_JUMBO_M			0x3c
#define MAX_RX_PKT_LEN_S		0
#define MAX_RX_PKT_LEN_M		0x3

/* Values of MAX_RX_PKT_LEN */
#define RX_PKT_LEN_1518			0
#define RX_PKT_LEN_1536			1
#define RX_PKT_LEN_1522			2
#define RX_PKT_LEN_MAX_JUMBO		3

/* Port debug count register */
#define DBG_CNT_BASE			0x3018
#define DBG_CNT_PORT_BASE		0x100
#define DBG_CNT(p)			(DBG_CNT_BASE + \
					(p) * DBG_CNT_PORT_BASE)
#define DIS_CLR				BIT(31)

#define SGMII_REG_BASE			0x5000
#define SGMII_REG_PORT_BASE		0x1000
#define SGMII_REG(p, r)			(SGMII_REG_BASE + \
					(p) * SGMII_REG_PORT_BASE + (r))
#define PCS_CONTROL_1(p)		SGMII_REG(p, 0x00)
#define SGMII_MODE(p)			SGMII_REG(p, 0x20)
#define QPHY_PWR_STATE_CTRL(p)		SGMII_REG(p, 0xe8)
#define PHYA_CTRL_SIGNAL3(p)		SGMII_REG(p, 0x128)
#define PHYA_ANA_SYSPLL(p)		SGMII_REG(p, 0x158)

/* Fields of PCS_CONTROL_1 */
#define SGMII_LINK_STATUS		BIT(18)
#define SGMII_AN_ENABLE			BIT(12)
#define SGMII_AN_RESTART		BIT(9)

/* Fields of SGMII_MODE */
#define SGMII_REMOTE_FAULT_DIS		BIT(8)
#define SGMII_IF_MODE_FORCE_DUPLEX	BIT(4)
#define SGMII_IF_MODE_FORCE_SPEED_S	0x2
#define SGMII_IF_MODE_FORCE_SPEED_M	0x0c
#define SGMII_IF_MODE_ADVERT_AN		BIT(1)

/* Values of SGMII_IF_MODE_FORCE_SPEED */
#define SGMII_IF_MODE_FORCE_SPEED_10	0
#define SGMII_IF_MODE_FORCE_SPEED_100	1
#define SGMII_IF_MODE_FORCE_SPEED_1000	2

/* Fields of QPHY_PWR_STATE_CTRL */
#define PHYA_PWD			BIT(4)

/* Fields of PHYA_CTRL_SIGNAL3 */
#define RG_TPHY_SPEED_S			2
#define RG_TPHY_SPEED_M			0x0c

/* Values of RG_TPHY_SPEED */
#define RG_TPHY_SPEED_1000		0
#define RG_TPHY_SPEED_2500		1

/* Fields of PHYA_ANA_SYSPLL */
#define RG_VUSB10_ON			BIT(29)

#define SYS_CTRL			0x7000
#define SW_PHY_RST			BIT(2)
#define SW_SYS_RST			BIT(1)
#define SW_REG_RST			BIT(0)

#define PHY_IAC				0x701c
#define PHY_ACS_ST			BIT(31)
#define MDIO_REG_ADDR_S			25
#define MDIO_REG_ADDR_M			0x3e000000
#define MDIO_PHY_ADDR_S			20
#define MDIO_PHY_ADDR_M			0x1f00000
#define MDIO_CMD_S			18
#define MDIO_CMD_M			0xc0000
#define MDIO_ST_S			16
#define MDIO_ST_M			0x30000
#define MDIO_RW_DATA_S			0
#define MDIO_RW_DATA_M			0xffff

#define HWSTRAP				0x7800
#define XTAL_FSEL_S			7
#define XTAL_FSEL_M			BIT(7)

#define SWSTRAP				0x7804
#define CHG_STRAP			BIT(8)
#define PHY_EN				BIT(6)

/* Values of XTAL_FSEL_S */
#define XTAL_40MHZ			0
#define XTAL_25MHZ			1

/* TOP Signals Status Register */
#define TOP_SIG_SR			0x780c
#define PAD_MCM_SMI_EN			BIT(0)
#define PAD_DUAL_SGMII_EN		BIT(1)

#define CHIP_REV			0x781C
#define CHIP_NAME_S			16
#define CHIP_NAME_M			0xffff0000
#define CHIP_REV_S			0
#define CHIP_REV_M			0x0f
#define CHIP_REV_E1			0x0

#define PLLGP_EN			0x7820
#define EN_COREPLL			BIT(2)
#define SW_CLKSW			BIT(1)
#define SW_PLLGP			BIT(0)

#define PLLGP_CR0			0x78a8
#define RG_COREPLL_EN			BIT(22)
#define RG_COREPLL_POSDIV_S		23
#define RG_COREPLL_POSDIV_M		0x3800000
#define RG_COREPLL_SDM_PCW_S		1
#define RG_COREPLL_SDM_PCW_M		0x3ffffe
#define RG_COREPLL_SDM_PCW_CHG		BIT(0)

/* RGMII and SGMII PLL clock */
#define ANA_PLLGP_CR2			0x78b0
#define ANA_PLLGP_CR5			0x78bc

/* GPIO_PAD_0 */
#define GPIO_MODE0      		0x7c0c
#define GPIO_MODE0_S			0
#define GPIO_MODE0_M			0xf
#define GPIO_0_INTERRUPT_MODE		0x1

/* GPIO_MODE2 */
#define GPIO_MODE2      		0x7c14
#define GPIO_MODE2_MDC_S		16
#define GPIO_MODE2_MDC_M		0xf0000
#define GPIO_20_MDC_MODE		0x2
#define GPIO_MODE2_MDIO_S		20
#define GPIO_MODE2_MDIO_M		0xf00000
#define GPIO_21_MDIO_MODE		0x2

/* GPIO GROUP IOLB SMT0 Control */
#define SMT0_IOLB			0x7f04
#define SMT_IOLB_5_SMI_MDC_EN		BIT(5)

/* PHY ENABLE Register bitmap define */
#define PHY_DEV1E			0x1e
#define PHY_DEV1E_REG_13		0x13
#define PHY_DEV1E_REG_14		0x14
#define PHY_DEV1E_REG_41		0x41
#define PHY_DEV1E_REG_0C6		0x0c6
#define PHY_DEV1E_REG_0FE		0x0fe
#define PHY_DEV1E_REG_141		0x141
#define PHY_DEV1E_REG_234		0x234

/* Proprietory Control Register of Internal Phy device 0x1e */
#define RXADC_CONTROL_3			0xc2
#define RXADC_LDO_CONTROL_2		0xd3

/* Fields of PHY_DEV1E_REG_0C6 */
#define PHY_POWER_SAVING_S		8
#define PHY_POWER_SAVING_M		0x300
#define PHY_POWER_SAVING_TX		0x0

/* Fields of PHY_DEV1E_REG_234 */
#define TR_OPEN_LOOP_EN			BIT(0)

#define PHY_DEV1F			0x1f
#define PHY_DEV1F_REG_44		0x44
#define PHY_DEV1F_REG_104		0x104
#define PHY_DEV1F_REG_10A		0x10a
#define PHY_DEV1F_REG_10B		0x10b
#define PHY_DEV1F_REG_10C		0x10c
#define PHY_DEV1F_REG_10D		0x10d
#define PHY_DEV1F_REG_268		0x268
#define PHY_DEV1F_REG_269		0x269
#define PHY_DEV1F_REG_26A		0x26A
#define PHY_DEV1F_REG_403		0x403

/* Proprietory Control Register of Internal Phy device 0x1f */
#define TXVLD_DA_271			0x271
#define TXVLD_DA_272			0x272
#define TXVLD_DA_273			0x273

/* Fields of PHY_DEV1F_REG_403 */
#define GBE_EFUSE_SETTING		BIT(3)
#define BYPASS_MODE 			BIT(4)
#define POWER_ON_OFF 			BIT(5)
#define PHY_PLL_M			(BIT(8) | BIT(9))
#define PHY_PLL_SEL(x)			((x << 8) & PHY_PLL_M)

/* PHY Extend Register 0x14 bitmap of define */
#define PHY_EXT_REG_14			0x14

/* Fields of PHY_EXT_REG_14 */
#define PHY_EN_DOWN_SHFIT		BIT(4)

/* PHY Extend Register 0x17 bitmap of define */
#define PHY_EXT_REG_17			0x17

/* Fields of PHY_EXT_REG_17 */
#define PHY_LINKDOWN_POWER_SAVING_EN	BIT(4)

/* PHY Token Ring Register 0x10 bitmap of define */
#define PHY_TR_REG_10			0x10

/* PHY Token Ring Register 0x11 bitmap of define */
#define PHY_TR_REG_11			0x11

/* PHY Token Ring Register 0x12 bitmap of define */
#define PHY_TR_REG_12			0x12

/* PHY PMA Register 0x18 bitmap of define */
#define ENABLE_RANDOM_UPDATE_TRIGGER	BIT(8)

int __mii_mgr_read(u32 phy_addr, u32 phy_register, u32 *read_data);
int __mii_mgr_write(u32 phy_addr, u32 phy_register, u32 write_data);


u32 mt7531_reg_read(u32 reg)
{
	u32 high, low;

	__mii_mgr_write(MT7531_SMI_ADDR, 0x1f,
		(reg & MT7531_REG_PAGE_ADDR_M) >> MT7531_REG_PAGE_ADDR_S);

	__mii_mgr_read(MT7531_SMI_ADDR,
		(reg & MT7531_REG_ADDR_M) >> MT7531_REG_ADDR_S, &low);

	__mii_mgr_read(MT7531_SMI_ADDR, 0x10, &high);

	return (high << 16) | (low & 0xffff);
}

void mt7531_reg_write(u32 reg, u32 val)
{
	__mii_mgr_write(MT7531_SMI_ADDR, 0x1f,
		(reg & MT7531_REG_PAGE_ADDR_M) >> MT7531_REG_PAGE_ADDR_S);

	__mii_mgr_write(MT7531_SMI_ADDR,
		(reg & MT7531_REG_ADDR_M) >> MT7531_REG_ADDR_S, val & 0xffff);

	__mii_mgr_write(MT7531_SMI_ADDR, 0x10, val >> 16);
}

/* Indirect MDIO clause 22/45 access */
static int mt7531_mii_rw(int phy, int reg, u16 data, u32 cmd, u32 st)
{
	ulong timeout;
	u32 val, timeout_ms;
	int ret = 0;

	val = (st << MDIO_ST_S) |
	      ((cmd << MDIO_CMD_S) & MDIO_CMD_M) |
	      ((phy << MDIO_PHY_ADDR_S) & MDIO_PHY_ADDR_M) |
	      ((reg << MDIO_REG_ADDR_S) & MDIO_REG_ADDR_M);

	if (cmd == MDIO_CMD_WRITE || cmd == MDIO_CMD_ADDR)
		val |= data & MDIO_RW_DATA_M;

	mt7531_reg_write(PHY_IAC, val | PHY_ACS_ST);

	timeout_ms = 100;
	timeout = get_timer(0);
	while (1) {
		val = mt7531_reg_read(PHY_IAC);

		if ((val & PHY_ACS_ST) == 0)
			break;

		if (get_timer(timeout) > timeout_ms)
			return -ETIMEDOUT;
	}

	if (cmd == MDIO_CMD_READ || cmd == MDIO_CMD_READ_C45) {
		val = mt7531_reg_read(PHY_IAC);
		ret = val & MDIO_RW_DATA_M;
	}

	return ret;
}

int mt7531_mii_read(int phy, int reg)
{
	if (phy < MT7531_NUM_PHYS)
		phy = (MT7531_PHY_ADDR_BASE + phy) & MT753X_SMI_ADDR_MASK;

	return mt7531_mii_rw(phy, reg, 0, MDIO_CMD_READ, MDIO_ST_C22);
}

void mt7531_mii_write(int phy, int reg, u16 val)
{
	if (phy < MT7531_NUM_PHYS)
		phy = (MT7531_PHY_ADDR_BASE + phy) & MT753X_SMI_ADDR_MASK;

	mt7531_mii_rw(phy, reg, val, MDIO_CMD_WRITE, MDIO_ST_C22);
}

int mt7531_mmd_read(int phy, int devad, u16 reg)
{
	int val;

	if (phy < MT7531_NUM_PHYS)
		phy = (MT7531_PHY_ADDR_BASE + phy) & MT753X_SMI_ADDR_MASK;

	mt7531_mii_rw(phy, devad, reg, MDIO_CMD_ADDR, MDIO_ST_C45);
	val = mt7531_mii_rw(phy, devad, 0, MDIO_CMD_READ_C45, MDIO_ST_C45);

	return val;
}

void mt7531_mmd_write(int phy, int devad, u16 reg, u16 val)
{
	if (phy < MT7531_NUM_PHYS)
		phy = (MT7531_PHY_ADDR_BASE + phy) & MT753X_SMI_ADDR_MASK;

	mt7531_mii_rw(phy, devad, reg, MDIO_CMD_ADDR, MDIO_ST_C45);
	mt7531_mii_rw(phy, devad, val, MDIO_CMD_WRITE, MDIO_ST_C45);
}



static int mt7531_set_port_sgmii_force_mode(u32 port,
					    struct mt7531_port_cfg *port_cfg)
{
	u32 speed, port_base, val;
	ulong timeout;
	u32 timeout_ms;

	printf("mt7531: mt7531_set_port_sgmii_force_mode, port = %d\n", port);

	if (port < 5 || port >= MT7531_NUM_PORTS) {
		printf("mt7531: port %d is not a SGMII port\n", port);
		return -EINVAL;
	}

	port_base = port - 5;

	switch (port_cfg->speed) {
	case MAC_SPD_1000:
		speed = RG_TPHY_SPEED_1000;
		break;
	case MAC_SPD_2500:
		speed = RG_TPHY_SPEED_2500;
		break;
	default:
		printf("mt7531: invalid SGMII speed idx %d for port %d\n",
			port_cfg->speed, port);

		speed = RG_TPHY_SPEED_1000;
	}

	/* Step 1: Speed select register setting */
	val = mt7531_reg_read(PHYA_CTRL_SIGNAL3(port_base));
	val &= ~RG_TPHY_SPEED_M;
	val |= speed << RG_TPHY_SPEED_S;
	mt7531_reg_write(PHYA_CTRL_SIGNAL3(port_base), val);

	/* Step 2 : Disable AN  */
	val = mt7531_reg_read(PCS_CONTROL_1(port_base));
	val &= ~SGMII_AN_ENABLE;
	mt7531_reg_write(PCS_CONTROL_1(port_base), val);

	/* Step 3: SGMII force mode setting */
	val = mt7531_reg_read(SGMII_MODE(port_base));
	val &= ~SGMII_IF_MODE_ADVERT_AN;
	val &= ~SGMII_IF_MODE_FORCE_SPEED_M;
	val |= SGMII_IF_MODE_FORCE_SPEED_1000 << SGMII_IF_MODE_FORCE_SPEED_S;
	val |= SGMII_IF_MODE_FORCE_DUPLEX;
	/* For sgmii force mode, 0 is full duplex and 1 is half duplex */
	if (port_cfg->duplex)
		val &= ~SGMII_IF_MODE_FORCE_DUPLEX;
	mt7531_reg_write(SGMII_MODE(port_base), val);

	/* Step 4: XXX: Disable Link partner's AN and set force mode */

	/* Step 5: XXX: Special setting for PHYA ==> reserved for flexible */

	/* Step 6 : Release PHYA power down state */
	val = mt7531_reg_read(QPHY_PWR_STATE_CTRL(port_base));
	val &= ~PHYA_PWD;
	mt7531_reg_write(QPHY_PWR_STATE_CTRL(port_base), val);

	/* Step 7 : Polling SGMII_LINK_STATUS */
	timeout_ms = 2000;
	timeout = get_timer(0);
	while (1) {
		val = mt7531_reg_read(PCS_CONTROL_1(port_base));
		val &= SGMII_LINK_STATUS;

		if (val)
			break;

		if (get_timer(timeout) > timeout_ms) {
			printf("mt7531: timeout waiting for SGMII_LINK\n");
			return -ETIMEDOUT;
		}
	}
	printf("mt7531 port = %d SGMII_LINK SUCESS \n", port);
	return 0;
}

static int mt7531_set_port_sgmii_an_mode(u32 port, struct mt7531_port_cfg *port_cfg)
{
	return 0;
}

static int mt7531_set_port_rgmii(u32 port)
{
	return 0;
}

static int mt7531_mac_port_setup(u32 port, struct mt7531_port_cfg *port_cfg)
{
	u32 pmcr;
	u32 speed;

	printf("mt7531: mt7531_mac_port_setup, port = %d\n", port);

	if (port < 5 || port >= MT7531_NUM_PORTS) {
		printf("mt7531: port %d is not a MAC port\n", port);
		return -EINVAL;
	}

	if (port_cfg->enabled) {
		pmcr = (IPG_96BIT_WITH_SHORT_IPG << IPG_CFG_S) |
		       MAC_MODE | MAC_TX_EN | MAC_RX_EN |
		       BKOFF_EN | BACKPR_EN;

		if (port_cfg->force_link) {
			/* pmcr's speed field 0x11 is reserved, sw should set 0x10 */
			speed = port_cfg->speed;
			if(port_cfg->speed == MAC_SPD_2500)
				speed = MAC_SPD_1000;

			pmcr |= FORCE_MODE_LNK | FORCE_LINK |
				FORCE_MODE_SPD | FORCE_MODE_DPX |
				FORCE_MODE_RX_FC | FORCE_MODE_TX_FC |
				FORCE_RX_FC | FORCE_TX_FC |
				(speed << FORCE_SPD_S);

			if (port_cfg->duplex)
				pmcr |= FORCE_DPX;
		}
	} else {
		pmcr = FORCE_MODE_LNK;
	}

	switch (port_cfg->phy_mode) {
	case PHY_INTERFACE_MODE_RGMII:
		mt7531_set_port_rgmii(port);
		break;
	case PHY_INTERFACE_MODE_SGMII:
		if (port_cfg->force_link)
			mt7531_set_port_sgmii_force_mode(port, port_cfg);
		else
			mt7531_set_port_sgmii_an_mode(port, port_cfg);
		break;
	default:
		printf("PHY mode %d is not supported by port %d\n",
			 port_cfg->phy_mode, port);

		pmcr = FORCE_MODE_LNK;
	}

	printf("mt7531: mt7531_mac_port_setup, PMCR%d = %08x\n", port, pmcr);

	mt7531_reg_write(PMCR(port), pmcr);

	return 0;
}


static void mt7531_core_pll_setup(void)
{
	u32 top_sig;
	u32 hwstrap;
	u32 xtal;
	u32 val;

	val = mt7531_reg_read(CHIP_REV);
	top_sig = mt7531_reg_read(TOP_SIG_SR);
	hwstrap = mt7531_reg_read(HWSTRAP);
	if ((val & CHIP_REV_M) > 0)
		xtal = (top_sig & PAD_MCM_SMI_EN) ? XTAL_40MHZ : XTAL_25MHZ;
	else
		xtal = (hwstrap & XTAL_FSEL_M) >> XTAL_FSEL_S;

	printf("mt7531: mt7531_core_pll_setup, hwstrap = %08x, xtal=%dMHz\n",
	       hwstrap, (xtal == XTAL_25MHZ) ? 25 : 40);

	/* Only BE needs additional setting */
	if (top_sig & PAD_DUAL_SGMII_EN)
		return;

	/* Disable Port5 SGMII clearly */
	val = mt7531_reg_read(PHYA_ANA_SYSPLL(0));
	val &= ~RG_VUSB10_ON;
	mt7531_reg_write(PHYA_ANA_SYSPLL(0), val);

	switch ((hwstrap & XTAL_FSEL_M) >> XTAL_FSEL_S) {
	case XTAL_25MHZ:
		/* Step 1 : Disable MT7531 COREPLL */
		val = mt7531_reg_read(PLLGP_EN);
		val &= ~EN_COREPLL;
		mt7531_reg_write(PLLGP_EN, val);

		/* Step 2: switch to XTAL output */
		val = mt7531_reg_read(PLLGP_EN);
		val |= SW_CLKSW;
		mt7531_reg_write(PLLGP_EN, val);

		val = mt7531_reg_read(PLLGP_CR0);
		val &= ~RG_COREPLL_EN;
		mt7531_reg_write(PLLGP_CR0, val);

		/* Step 3: disable PLLGP and enable program PLLGP */
		val = mt7531_reg_read(PLLGP_EN);
		val |= SW_PLLGP;
		mt7531_reg_write(PLLGP_EN, val);

		/* Step 4: program COREPLL output frequency to 500MHz */
		val = mt7531_reg_read(PLLGP_CR0);
		val &= ~RG_COREPLL_POSDIV_M;
		val |= 2 << RG_COREPLL_POSDIV_S;  /* 4 division: 010 */
		mt7531_reg_write(PLLGP_CR0, val);
		udelay(25);

		val = mt7531_reg_read(PLLGP_CR0);
		val &= ~RG_COREPLL_SDM_PCW_M;
		val |= 0x140000 << RG_COREPLL_SDM_PCW_S;
		mt7531_reg_write(PLLGP_CR0, val);

		/* Set feedback divide ratio update signal to high */
		val = mt7531_reg_read(PLLGP_CR0);
		val |= RG_COREPLL_SDM_PCW_CHG;
		mt7531_reg_write(PLLGP_CR0, val);
		/* Wait for at least 16 XTAL clocks */
		udelay(10);

		/* Step 5: set feedback divide ratio update signal to low */
		val = mt7531_reg_read(PLLGP_CR0);
		val &= ~RG_COREPLL_SDM_PCW_CHG;
		mt7531_reg_write(PLLGP_CR0, val);

		/* add enable 325M clock for SGMII */
		mt7531_reg_write(ANA_PLLGP_CR5 , 0xad0000);

		/* add enable 250SSC clock for RGMII */
		mt7531_reg_write(ANA_PLLGP_CR2 , 0x4f40000);

		/*Step 6: Enable MT7531 PLL */
		val = mt7531_reg_read(PLLGP_CR0);
		val |= RG_COREPLL_EN;
		mt7531_reg_write(PLLGP_CR0, val);

		val = mt7531_reg_read(PLLGP_EN);
		val |= EN_COREPLL;
		mt7531_reg_write(PLLGP_EN, val);
		udelay(25);

		break;
	case XTAL_40MHZ:
		/* Step 1 : Disable MT7531 COREPLL */
		val = mt7531_reg_read(PLLGP_EN);
		val &= ~EN_COREPLL;
		mt7531_reg_write(PLLGP_EN, val);

		/* Step 2: switch to XTAL output */
		val = mt7531_reg_read(PLLGP_EN);
		val |= SW_CLKSW;
		mt7531_reg_write(PLLGP_EN, val);

		val = mt7531_reg_read(PLLGP_CR0);
		val &= ~RG_COREPLL_EN;
		mt7531_reg_write(PLLGP_CR0, val);

		/* Step 3: disable PLLGP and enable program PLLGP */
		val = mt7531_reg_read(PLLGP_EN);
		val |= SW_PLLGP;
		mt7531_reg_write(PLLGP_EN, val);

		/* Step 4: program COREPLL output frequency to 500MHz */
		val = mt7531_reg_read(PLLGP_CR0);
		val &= ~RG_COREPLL_POSDIV_M;
		val |= 2 << RG_COREPLL_POSDIV_S;
		mt7531_reg_write(PLLGP_CR0, val);
		udelay(25);

		val = mt7531_reg_read(PLLGP_CR0);
		val &= ~RG_COREPLL_SDM_PCW_M;
		val |= 0x190000 << RG_COREPLL_SDM_PCW_S;
		mt7531_reg_write(PLLGP_CR0, val);

		/* Set feedback divide ratio update signal to high */
		val = mt7531_reg_read(PLLGP_CR0);
		val |= RG_COREPLL_SDM_PCW_CHG;
		mt7531_reg_write(PLLGP_CR0, val);
		/* Wait for at least 16 XTAL clocks */
		udelay(10);

		/* Step 5: set feedback divide ratio update signal to low */
		val = mt7531_reg_read(PLLGP_CR0);
		val &= ~RG_COREPLL_SDM_PCW_CHG;
		mt7531_reg_write(PLLGP_CR0, val);

		/* Enable 325M clock for SGMII */
		mt7531_reg_write(ANA_PLLGP_CR5, 0xad0000);

		/* Enable 250SSC clock for RGMII */
		mt7531_reg_write(ANA_PLLGP_CR2, 0x4f40000);

		/* Step 6: Enable MT7531 PLL */
		val = mt7531_reg_read(PLLGP_CR0);
		val |= RG_COREPLL_EN;
		mt7531_reg_write(PLLGP_CR0, val);

		val = mt7531_reg_read(PLLGP_EN);
		val |= EN_COREPLL;
		mt7531_reg_write(PLLGP_EN, val);
		udelay(25);
		break;
	}
}

static int mt7531_set_gpio_pinmux(void)
{
	u32 val;

	/* set GPIO 0 interrupt mode */
	val = mt7531_reg_read(GPIO_MODE0);
	val &= ~GPIO_MODE0_M;
	val |= GPIO_0_INTERRUPT_MODE << GPIO_MODE0_S ;
	mt7531_reg_write(GPIO_MODE0, val);

	/* set GPIO 20/21 DMC/MDIO mode 
	EXT SMI did not use in WAX206 */
#if 0
	val = mt7531_reg_read(GPIO_MODE2);
	val &= ~GPIO_MODE2_MDC_M;
	val |= GPIO_20_MDC_MODE << GPIO_MODE2_MDC_S ;
	mt7531_reg_write(GPIO_MODE2, val);

	val = mt7531_reg_read(GPIO_MODE2);
	val &= ~GPIO_MODE2_MDIO_M;
	val |= GPIO_21_MDIO_MODE << GPIO_MODE2_MDIO_S ;
	mt7531_reg_write(GPIO_MODE2, val);
#endif

	return 0;
}

static void mt7531_afifo_reset(int enable)
{
	int p;
	u32 val;

	for (p = 0; p < MT7531_NUM_PORTS; p++) {
		val = mt7531_reg_read(DBG_CNT(p));
		if (enable) {
			val &= ~DIS_CLR;
		} else {
			val |= DIS_CLR;
		}
		mt7531_reg_write(DBG_CNT(p), val);
	}
}

static void mt7531_phy_pll_setup(void)
{
	u32 hwstrap;
	u32 val;

	val = mt7531_reg_read(CHIP_REV);
	if ((val & CHIP_REV_M) > 0)
		return;

	hwstrap = mt7531_reg_read(HWSTRAP);

	switch ((hwstrap & XTAL_FSEL_M) >> XTAL_FSEL_S) {
	case XTAL_25MHZ:
		/* disable pll auto calibration */
		mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_104, 0x608);

		/* change pll sel */
		val = mt7531_mmd_read(0, PHY_DEV1F,
				     PHY_DEV1F_REG_403);
		val &= ~(PHY_PLL_M);
		val |= PHY_PLL_SEL(3);
		mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_403, val);

		/* set divider ratio */
		mt7531_mmd_write(0, PHY_DEV1F,
			       PHY_DEV1F_REG_10A, 0x1009);

		/* set divider ratio */
		mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_10B, 0x7c6);

		/* capacitance and resistance adjustment */
		mt7531_mmd_write(0, PHY_DEV1F,
			       PHY_DEV1F_REG_10C, 0xa8be);

		break;
	case XTAL_40MHZ:
		/* disable pll auto calibration */
		mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_104, 0x608);

		/* change pll sel */
		val = mt7531_mmd_read(0, PHY_DEV1F,
				     PHY_DEV1F_REG_403);
		val &= ~(PHY_PLL_M);
		val |= PHY_PLL_SEL(3);
		mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_403, val);

		/* set divider ratio */
		mt7531_mmd_write(0, PHY_DEV1F,
			       PHY_DEV1F_REG_10A, 0x1018);

		/* set divider ratio */
		mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_10B, 0xc676);

		/* capacitance and resistance adjustment */
		mt7531_mmd_write(0, PHY_DEV1F,
			       PHY_DEV1F_REG_10C, 0xd8be);
		break;
	}

	/* power down pll. additional delay is not required via mdio access */
	mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_10D, 0x10);

	/* power up pll */
	mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_10D, 0x14);
}

static void mt7531_phy_setting(void)
{
	int i;
	u32 val;

	for (i = 0; i < MT7531_NUM_PHYS; i++) {
		/* Enable HW auto downshift */
		mt7531_mii_write(i, 0x1f, 0x1);
		val = mt7531_mii_read(i, PHY_EXT_REG_14);
		val |= PHY_EN_DOWN_SHFIT;
		mt7531_mii_write(i, PHY_EXT_REG_14, val);

		/* Decrease SlvDPSready time */
		mt7531_mii_write(i, 0x1f, 0x52b5);
		mt7531_mii_write(i, PHY_TR_REG_10, 0xafae);
		mt7531_mii_write(i, PHY_TR_REG_12, 0x6);
		mt7531_mii_write(i, PHY_TR_REG_10, 0x8fae);
		mt7531_mii_write(i, 0x1f, 0);

		/* Disable Random Update Mechanism */
		mt7531_mii_write(i, 0x1f, 0x52b5);
		mt7531_mii_write(i, PHY_TR_REG_10, 0xafb0);
		val = mt7531_mii_read(i, PHY_TR_REG_11);
		val |= ENABLE_RANDOM_UPDATE_TRIGGER;
		mt7531_mii_write(i, PHY_TR_REG_11, val);
		mt7531_mii_write(i, PHY_TR_REG_10, 0x8fb0);
		mt7531_mii_write(i, 0x1f, 0);

		/* PHY link down power saving enable */
		val = mt7531_mii_read(i, PHY_EXT_REG_17);
		val |= PHY_LINKDOWN_POWER_SAVING_EN;
		mt7531_mii_write(i, PHY_EXT_REG_17, val);

		val = mt7531_mmd_read(i, PHY_DEV1E, PHY_DEV1E_REG_0C6);
		val &= ~PHY_POWER_SAVING_M;
		val |= PHY_POWER_SAVING_TX << PHY_POWER_SAVING_S;
		mt7531_mmd_write(i, PHY_DEV1E, PHY_DEV1E_REG_0C6, val);

		/* Timing Recovery for GbE slave mode */
		mt7531_mii_write(i, 0x1f, 0x52b5);
		mt7531_mii_write(i, PHY_TR_REG_12, 0x6f);
		mt7531_mii_write(i, PHY_TR_REG_11, 0xb90a);
		mt7531_mii_write(i, PHY_TR_REG_10, 0x8f82);
		mt7531_mii_write(i, PHY_TR_REG_12, 0x2e);
		mt7531_mii_write(i, PHY_TR_REG_11, 0xbaef);
		mt7531_mii_write(i, PHY_TR_REG_10, 0x968c);
		mt7531_mii_write(i, 0x1f, 0);
		val = mt7531_mmd_read(i, PHY_DEV1E, PHY_DEV1E_REG_234);
		val |= TR_OPEN_LOOP_EN;
		mt7531_mmd_write(i, PHY_DEV1E, PHY_DEV1E_REG_234, val);

		/* Enable Asymmetric Pause Capability */
		val = mt7531_mii_read(i, MII_ADVERTISE);
		val |= ADVERTISE_PAUSE_ASYM;
		mt7531_mii_write(i, MII_ADVERTISE, val);
	}
}

static int mt7531_internal_phy_calibration(void)
{
	u32 i, val;
	int ret;

	printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>> START CALIBRATION:\n");

	/* gphy value from sw path */
	val = mt7531_mmd_read(0, PHY_DEV1F, PHY_DEV1F_REG_403);
	val |= GBE_EFUSE_SETTING;
	mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_403, val);

	for (i = 0; i < MT7531_NUM_PHYS; i++) {
		printf("-------- gephy-calbration (port:%d) --------\n", i);
		ret = 0;
		ret = mt7531_phy_calibration(i);

		/* set Auto-negotiation with giga extension. */
		val = mt7531_mii_read(i, MII_BMCR);
		val |= BMCR_ANENABLE | BMCR_ANRESTART;
		mt7531_mii_write(i, MII_BMCR, val);
		if (ret)
			return ret;
	}

	return 0;
}

static void mt7531_adjust_line_driving(u32 port)
{
	/* For ADC timing margin window for LDO calibration */
	mt7531_mmd_write(port, PHY_DEV1E, RXADC_LDO_CONTROL_2, 0x2222);

	/* Adjust AD sample timing */
	mt7531_mmd_write(port, PHY_DEV1E, RXADC_CONTROL_3, 0x4444);

	/* Adjust Line driver current for different mode */
	mt7531_mmd_write(port, PHY_DEV1F, TXVLD_DA_271, 0x2ca5);

	/* Adjust Line driver current for different mode */
	mt7531_mmd_write(port, PHY_DEV1F, TXVLD_DA_272, 0xc6b);

	/* Adjust Line driver gain for 10BT from 1000BT calibration result */
	mt7531_mmd_write(port, PHY_DEV1F, TXVLD_DA_273, 0x3000);

	/* Adjust RX Echo path filter */
	mt7531_mmd_write(port, PHY_DEV1E, PHY_DEV1E_REG_0FE, 0x2);

	/* Adjust RX HVGA bias current */
	mt7531_mmd_write(port, PHY_DEV1E, PHY_DEV1E_REG_41, 0x3333);

	/* Adjust TX class AB driver 1 */
	mt7531_mmd_write(port, PHY_DEV1F, PHY_DEV1F_REG_268, 0x384);

	/* Adjust TX class AB driver 2 */
	mt7531_mmd_write(port, PHY_DEV1F, PHY_DEV1F_REG_269, 0x1114);

	/* Adjust DAC delay for TX Pairs */
	mt7531_mmd_write(port, PHY_DEV1E, PHY_DEV1E_REG_13, 0x404);
	mt7531_mmd_write(port, PHY_DEV1E, PHY_DEV1E_REG_14, 0x404);

	/* Adjust DAC digital delay for TX Delay */
	mt7531_mmd_write(port, PHY_DEV1F, PHY_DEV1F_REG_44, 0xc0);

	/* Adjust Line driver compensation cap for stability concern due to
	 * increase current.
	 */
	mt7531_mmd_write(port, PHY_DEV1F, PHY_DEV1F_REG_26A, 0x3333);
}

void mt7531_sw_init(struct mt7531_port_cfg *p5, struct mt7531_port_cfg *p6)
{
	int i;
	u32 val;

	printf("mt7531: mt7531_sw_init\n");

	for (i = 0; i < MT7531_NUM_PHYS; i++) {
		val = mt7531_mii_read(i, MII_BMCR);
		val |= BMCR_ISOLATE;
		mt7531_mii_write(i, MII_BMCR, val);
	}

	/* Force MAC link down before reset */
	mt7531_reg_write(PMCR(5), FORCE_MODE_LNK);
	mt7531_reg_write(PMCR(6), FORCE_MODE_LNK);

	/* Switch soft reset */
	mt7531_reg_write(SYS_CTRL, SW_SYS_RST | SW_REG_RST);
	udelay(100);

	/* Enable MDC input Schmitt Trigger */
	val = mt7531_reg_read(SMT0_IOLB);
	mt7531_reg_write(SMT0_IOLB, val | SMT_IOLB_5_SMI_MDC_EN);

	/* Set 7531 gpio pinmux */
	mt7531_set_gpio_pinmux();

	/* global mac control settings configuration */
	mt7531_reg_write(GMACCR,
			 (15 << MTCC_LMT_S) | (2 << MAX_RX_JUMBO_S) |
			 RX_PKT_LEN_MAX_JUMBO);

	mt7531_core_pll_setup();

	if (p5)
		mt7531_mac_port_setup(5, p5);

	if (p6)
		mt7531_mac_port_setup(6, p6);

	/* Disable AFIFO reset for extra short IPG */
	mt7531_afifo_reset(0);

	/* Let internal PHYs only Tx constant data in configure stage to
	 * reduce link down/up in configure stage.
	 */
	for (i = 0; i < MT7531_NUM_PHYS; i++)
		mt7531_mmd_write(i, PHY_DEV1E, PHY_DEV1E_REG_141, 0x200);

	/* Internal PHYs might be enabled by HW Bootstrapping. Disable
	 * internal PHYs before setup PHY PLL.
	 */
	val = mt7531_mmd_read(0, PHY_DEV1F, PHY_DEV1F_REG_403);
	val |= BYPASS_MODE;
	val |= POWER_ON_OFF;
	mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_403, val);

	mt7531_phy_pll_setup();

	/* Enable Internal PHYs */
	val = mt7531_mmd_read(0 ,PHY_DEV1F, PHY_DEV1F_REG_403);
	val |= BYPASS_MODE;
	val &= ~POWER_ON_OFF;
	mt7531_mmd_write(0, PHY_DEV1F, PHY_DEV1F_REG_403, val);

	mt7531_phy_setting();

	for (i = 0; i < MT7531_NUM_PHYS; i++) {
		val = mt7531_mii_read(i, MII_BMCR);
		val &= ~BMCR_ISOLATE;
		mt7531_mii_write(i, MII_BMCR, val);
	}

	for (i = 0; i < MT7531_NUM_PHYS; i++)
		mt7531_adjust_line_driving(i);

	/* Restore normal Tx function of internal PHYs after configure stage. */
	for (i = 0; i < MT7531_NUM_PHYS; i++)
		mt7531_mmd_write(i, PHY_DEV1E, PHY_DEV1E_REG_141, 0x0);

	mt7531_internal_phy_calibration();

}
