// SPDX-License-Identifier: GPL-2.0+
#include <linux/bitfield.h>
#include <linux/module.h>
#include <linux/nvmem-consumer.h>
#include <linux/of_address.h>
#include <linux/of_platform.h>
#include <linux/phy.h>

#define ANALOG_INTERNAL_OPERATION_MAX_US	(20)
#define ZCAL_CTRL_MIN				(0)
#define ZCAL_CTRL_MAX				(63)
#define TXRESERVE_MIN				(0)
#define TXRESERVE_MAX				(7)


#define MTK_EXT_PAGE_ACCESS		0x1f
#define MTK_PHY_PAGE_STANDARD		0x0000
#define MTK_PHY_PAGE_EXTENDED		0x0001
#define MTK_PHY_PAGE_EXTENDED_2		0x0002
#define MTK_PHY_PAGE_EXTENDED_3		0x0003
#define MTK_PHY_PAGE_EXTENDED_2A30	0x2a30
#define MTK_PHY_PAGE_EXTENDED_52B5	0x52b5

/* Registers on MDIO_MMD_VEND1 */
enum {
	MTK_PHY_MIDDLE_LEVEL_SHAPPER_0TO1 = 0,
	MTK_PHY_1st_OVERSHOOT_LEVEL_0TO1,
	MTK_PHY_2nd_OVERSHOOT_LEVEL_0TO1,
	MTK_PHY_MIDDLE_LEVEL_SHAPPER_1TO0,
	MTK_PHY_1st_OVERSHOOT_LEVEL_1TO0,
	MTK_PHY_2nd_OVERSHOOT_LEVEL_1TO0,
	MTK_PHY_MIDDLE_LEVEL_SHAPPER_0TON1, /* N means negative */
	MTK_PHY_1st_OVERSHOOT_LEVEL_0TON1,
	MTK_PHY_2nd_OVERSHOOT_LEVEL_0TON1,
	MTK_PHY_MIDDLE_LEVEL_SHAPPER_N1TO0,
	MTK_PHY_1st_OVERSHOOT_LEVEL_N1TO0,
	MTK_PHY_2nd_OVERSHOOT_LEVEL_N1TO0,
	MTK_PHY_TX_MLT3_END,
};

#define MTK_PHY_TXVLD_DA_RG				(0x12)
#define   MTK_PHY_DA_TX_I2MPB_A_GBE_MASK	GENMASK(15, 10)
#define   MTK_PHY_DA_TX_I2MPB_A_TBT_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_A2	(0x16)
#define   MTK_PHY_DA_TX_I2MPB_A_HBT_MASK	GENMASK(15, 10)
#define   MTK_PHY_DA_TX_I2MPB_A_TST_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_B1	(0x17)
#define   MTK_PHY_DA_TX_I2MPB_B_GBE_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_B_TBT_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_B2	(0x18)
#define   MTK_PHY_DA_TX_I2MPB_B_HBT_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_B_TST_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_C1	(0x19)
#define   MTK_PHY_DA_TX_I2MPB_C_GBE_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_C_TBT_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_C2	(0x20)
#define   MTK_PHY_DA_TX_I2MPB_C_HBT_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_C_TST_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_D1	(0x21)
#define   MTK_PHY_DA_TX_I2MPB_D_GBE_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_D_TBT_MASK	GENMASK(5, 0)

#define MTK_PHY_TX_I2MPB_TEST_MODE_D2	(0x22)
#define   MTK_PHY_DA_TX_I2MPB_D_HBT_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_I2MPB_D_TST_MASK	GENMASK(5, 0)


#define MTK_PHY_RESERVE_RG_0		(0x27)
#define MTK_PHY_RESERVE_RG_1		(0x28)

#define MTK_PHY_RG_ANA_TEST_POWERUP_TX	(0x3b)
#define MTK_PHY_TANA_CAL_MODE		(0xc1)
#define MTK_PHY_TANA_CAL_MODE_SHIFT	(8)

#define MTK_PHY_RXADC_CTRL_RG7		(0xc6)
#define   MTK_PHY_DA_AD_BUF_BIAS_LP_MASK	GENMASK(9, 8)

#define MTK_PHY_RXADC_CTRL_RG9		(0xc8)
#define   MTK_PHY_DA_RX_PSBN_TBT_MASK	GENMASK(14, 12)
#define   MTK_PHY_DA_RX_PSBN_HBT_MASK	GENMASK(10, 8)
#define   MTK_PHY_DA_RX_PSBN_GBE_MASK	GENMASK(6, 4)
#define   MTK_PHY_DA_RX_PSBN_LP_MASK	GENMASK(2, 0)

#define MTK_PHY_RG_ANA_CAL_RG0		(0xdb)
#define   MTK_PHY_RG_CAL_CKINV		BIT(12)
#define   MTK_PHY_RG_ANA_CALEN		BIT(8)
#define   MTK_PHY_RG_REXT_CALEN		BIT(4)
#define   MTK_PHY_RG_ZCALEN_A		BIT(0)

#define MTK_PHY_RG_ANA_CAL_RG1		(0xdc)
#define   MTK_PHY_RG_ZCALEN_B		BIT(12)
#define   MTK_PHY_RG_ZCALEN_C		BIT(8)
#define   MTK_PHY_RG_ZCALEN_D		BIT(4)
#define   MTK_PHY_RG_TXVOS_CALEN	BIT(0)

#define MTK_PHY_RG_ANA_CAL_RG2		(0xdd)
#define   MTK_PHY_RG_TXG_CALEN_A	BIT(12)
#define   MTK_PHY_RG_TXG_CALEN_B	BIT(8)
#define   MTK_PHY_RG_TXG_CALEN_C	BIT(4)
#define   MTK_PHY_RG_TXG_CALEN_D	BIT(0)

#define MTK_PHY_RG_ANA_CAL_RG5		(0xe0)
#define   MTK_PHY_RG_REXT_TRIM_MASK	GENMASK(13, 8)
#define   MTK_PHY_RG_ZCAL_CTRL_MASK	GENMASK(5, 0)

#define MTK_PHY_RG_TX_FILTER		(0xfe)

#define MTK_PHY_RG_DEV1E_REG172		(0x172)
#define   MTK_PHY_CR_TX_AMP_OFFSET_A_MASK	GENMASK(13, 8)
#define   MTK_PHY_CR_TX_AMP_OFFSET_B_MASK	GENMASK(6, 0)

#define MTK_PHY_RG_DEV1E_REG173		(0x173)
#define   MTK_PHY_CR_TX_AMP_OFFSET_C_MASK	GENMASK(13, 8)
#define   MTK_PHY_CR_TX_AMP_OFFSET_D_MASK	GENMASK(6, 0)

#define MTK_PHY_RG_DEV1E_REG174		(0x174)
#define   MTK_PHY_RSEL_TX_A_MASK	GENMASK(14, 8)
#define   MTK_PHY_RSEL_TX_B_MASK	GENMASK(6, 0)

#define MTK_PHY_RG_DEV1E_REG175		(0x175)
#define   MTK_PHY_RSEL_TX_C_MASK	GENMASK(14, 8)
#define   MTK_PHY_RSEL_TX_D_MASK	GENMASK(6, 0)

#define MTK_PHY_RG_DEV1E_REG17A		(0x17a)
#define   MTK_PHY_AD_CAL_COMP_OUT_SHIFT	(8)

#define MTK_PHY_RG_DEV1E_REG17B		(0x17b)
#define   MTK_PHY_DA_CAL_CLK		BIT(0)

#define MTK_PHY_RG_DEV1E_REG17C		(0x17c)
#define   MTK_PHY_DA_CALIN_FLAG		BIT(0)

#define MTK_PHY_RG_DEV1E_REG17D		(0x17d)
#define   MTK_PHY_DASN_DAC_IN0_A_MASK	GENMASK(9, 0)

#define MTK_PHY_RG_DEV1E_REG17E		(0x17e)
#define   MTK_PHY_DASN_DAC_IN0_B_MASK	GENMASK(9, 0)

#define MTK_PHY_RG_DEV1E_REG17F		(0x17f)
#define   MTK_PHY_DASN_DAC_IN0_C_MASK	GENMASK(9, 0)

#define MTK_PHY_RG_DEV1E_REG180		(0x180)
#define   MTK_PHY_DASN_DAC_IN0_D_MASK	GENMASK(9, 0)

#define MTK_PHY_RG_DEV1E_REG181		(0x181)
#define   MTK_PHY_DASN_DAC_IN1_A_MASK	GENMASK(9, 0)

#define MTK_PHY_RG_DEV1E_REG182		(0x182)
#define   MTK_PHY_DASN_DAC_IN1_B_MASK	GENMASK(9, 0)

#define MTK_PHY_RG_DEV1E_REG183		(0x183)
#define   MTK_PHY_DASN_DAC_IN1_C_MASK	GENMASK(9, 0)

#define MTK_PHY_RG_DEV1E_REG184		(0x180)
#define   MTK_PHY_DASN_DAC_IN1_D_MASK	GENMASK(9, 0)

#define MTK_PHY_RG_LP_IIR2_K1_L		(0x22a)
#define MTK_PHY_RG_LP_IIR2_K1_U		(0x22b)
#define MTK_PHY_RG_LP_IIR2_K2_L		(0x22c)
#define MTK_PHY_RG_LP_IIR2_K2_U		(0x22d)
#define MTK_PHY_RG_LP_IIR2_K3_L		(0x22e)
#define MTK_PHY_RG_LP_IIR2_K3_U		(0x22f)
#define MTK_PHY_RG_LP_IIR2_K4_L		(0x230)
#define MTK_PHY_RG_LP_IIR2_K4_U		(0x231)
#define MTK_PHY_RG_LP_IIR2_K5_L		(0x232)
#define MTK_PHY_RG_LP_IIR2_K5_U		(0x233)

#define MTK_PHY_RG_DEV1E_REG234		(0x234)
#define   MTK_PHY_TR_OPEN_LOOP_EN_MASK	GENMASK(0, 0)
#define   MTK_PHY_LPF_X_AVERAGE_MASK	GENMASK(7, 4)

#define MTK_PHY_RG_LPF_CNT_VAL		(0x235)

#define MTK_PHY_RG_DEV1E_REG27C		(0x27c)
#define   MTK_PHY_VGASTATE_FFE_THR_ST1_MASK	GENMASK(12, 8)
#define MTK_PHY_RG_DEV1E_REG27D		(0x27d)
#define   MTK_PHY_VGASTATE_FFE_THR_ST2_MASK	GENMASK(4, 0)

#define MTK_PHY_RG_DEV1E_REG53D		(0x53d)
#define   MTK_PHY_DA_TX_R50_A_NORMAL_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_R50_A_TBT_MASK		GENMASK(5, 0)

#define MTK_PHY_RG_DEV1E_REG53E		(0x53e)
#define   MTK_PHY_DA_TX_R50_B_NORMAL_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_R50_B_TBT_MASK		GENMASK(5, 0)

#define MTK_PHY_RG_DEV1E_REG53F		(0x53f)
#define   MTK_PHY_DA_TX_R50_C_NORMAL_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_R50_C_TBT_MASK		GENMASK(5, 0)

#define MTK_PHY_RG_DEV1E_REG540		(0x540)
#define   MTK_PHY_DA_TX_R50_D_NORMAL_MASK	GENMASK(13, 8)
#define   MTK_PHY_DA_TX_R50_D_TBT_MASK		GENMASK(5, 0)


/* Registers on MDIO_MMD_VEND2 */
#define MTK_PHY_LED0_ON_CTRL		(0x24)
#define   MTK_PHY_LED0_ON_MASK			GENMASK(6, 0)
#define   MTK_PHY_LED0_ON_LINK1000	BIT(0)
#define   MTK_PHY_LED0_ON_LINK100	BIT(1)
#define   MTK_PHY_LED0_ON_LINK10	BIT(2)
#define   MTK_PHY_LED0_ON_LINKDOWN	BIT(3)
#define   MTK_PHY_LED0_ON_FDX		BIT(4) /* Full duplex */
#define   MTK_PHY_LED0_ON_HDX		BIT(5) /* Half duplex */
#define   MTK_PHY_LED0_FORCE_ON		BIT(6)
#define   MTK_PHY_LED0_POLARITY		BIT(14)
#define   MTK_PHY_LED0_ENABLE		BIT(15)

#define MTK_PHY_LED0_BLINK_CTRL		(0x25)
#define   MTK_PHY_LED0_1000TX		BIT(0)
#define   MTK_PHY_LED0_1000RX		BIT(1)
#define   MTK_PHY_LED0_100TX		BIT(2)
#define   MTK_PHY_LED0_100RX		BIT(3)
#define   MTK_PHY_LED0_10TX		BIT(4)
#define   MTK_PHY_LED0_10RX		BIT(5)
#define   MTK_PHY_LED0_COLLISION	BIT(6)
#define   MTK_PHY_LED0_RX_CRC_ERR	BIT(7)
#define   MTK_PHY_LED0_RX_IDLE_ERR	BIT(8)
#define   MTK_PHY_LED0_FORCE_BLINK	BIT(9)

#define MTK_PHY_ANA_TEST_BUS_CTRL_RG	(0x100)
#define   MTK_PHY_ANA_TEST_MODE_MASK		GENMASK(15, 8)

#define MTK_PHY_RG_DEV1F_REG110		(0x110)
#define   MTK_PHY_RG_TST_DMY2_MASK		GENMASK(5, 0)
#define   MTK_PHY_RG_TANA_RESERVE_MASK	GENMASK(13, 8)

#define MTK_PHY_RG_DEV1F_REG115		(0x115)
#define   MTK_PHY_RG_BG_RASEL_MASK	GENMASK(2, 0)

/*
 * These macro privides efuse parsing for internal phy.
 */
#define EFS_DA_TX_I2MPB_A(x)		(((x) >> 0) & GENMASK(5, 0))
#define EFS_DA_TX_I2MPB_B(x)		(((x) >> 6) & GENMASK(5, 0))
#define EFS_DA_TX_I2MPB_C(x)		(((x) >> 12) & GENMASK(5, 0))
#define EFS_DA_TX_I2MPB_D(x)		(((x) >> 18) & GENMASK(5, 0))
#define EFS_DA_TX_AMP_OFFSET_A(x)	(((x) >> 24) & GENMASK(5, 0))

#define EFS_DA_TX_AMP_OFFSET_B(x)	(((x) >> 0) & GENMASK(5, 0))
#define EFS_DA_TX_AMP_OFFSET_C(x)	(((x) >> 6) & GENMASK(5, 0))
#define EFS_DA_TX_AMP_OFFSET_D(x)	(((x) >> 12) & GENMASK(5, 0))
#define EFS_DA_TX_R50_A(x)		(((x) >> 18) & GENMASK(5, 0))
#define EFS_DA_TX_R50_B(x)		(((x) >> 24) & GENMASK(5, 0))

#define EFS_DA_TX_R50_C(x)		(((x) >> 0) & GENMASK(5, 0))
#define EFS_DA_TX_R50_D(x)		(((x) >> 6) & GENMASK(5, 0))
#define EFS_DA_TX_R50_A_10M(x)		(((x) >> 12) & GENMASK(5, 0))
#define EFS_DA_TX_R50_B_10M(x)		(((x) >> 18) & GENMASK(5, 0))

#define EFS_RG_BG_RASEL(x)		(((x) >> 4) & GENMASK(2, 0))
#define EFS_RG_REXT_TRIM(x)		(((x) >> 7) & GENMASK(5, 0))

typedef enum {
	PAIR_A,
	PAIR_B,
	PAIR_C,
	PAIR_D,
} phy_cal_pair_t;

enum {
	GPHY_PORT0,
	GPHY_PORT1,
	GPHY_PORT2,
	GPHY_PORT3,
};

const u8 mt798x_zcal_to_r50[64] = {
	7, 8, 9, 9, 10, 10, 11, 11,
	12, 13, 13, 14, 14, 15, 16, 16,
	17, 18, 18, 19, 20, 21, 21, 22,
	23, 24, 24, 25, 26, 27, 28, 29,
	30, 31, 32, 33, 34, 35, 36, 37,
	38, 40, 41, 42, 43, 45, 46, 48,
	49, 51, 52, 54, 55, 57, 59, 61,
	62, 63, 63, 63, 63, 63, 63, 63
};

const char pair[4] = {'A', 'B', 'C', 'D'};

#define CAL_NO_PAIR(cal_item, cal_mode, ...) \
	cal_ret = cal_item##_cal_##cal_mode(phydev, ##__VA_ARGS__);

#define CAL_PAIR_A_TO_A(cal_item, cal_mode, ...)	\
	for(i=PAIR_A; i<=PAIR_A; i++) {			\
		cal_ret = cal_item##_cal_##cal_mode(phydev, ##__VA_ARGS__, i);\
		if(cal_ret) break;			\
	}

#define CAL_PAIR_A_TO_D(cal_item, cal_mode, ...)	\
	for(i=PAIR_A; i<=PAIR_D; i++) {			\
		cal_ret = cal_item##_cal_##cal_mode(phydev, ##__VA_ARGS__, i);\
		if(cal_ret) break;			\
	}

#define SW_CAL(cal_item, cal_mode_get, pair_mode)			\
	if(ret || (!ret && strcmp("sw", cal_mode_get) == 0)) {		\
		CAL_##pair_mode(cal_item, sw)				\
	}

#define SW_EFUSE_CAL(cal_item, cal_mode_get, pair_mode,...)	\
	if ((efs_valid && ret) ||				\
	    (efs_valid && !ret && strcmp("efuse", cal_mode_get) == 0)) {	\
		CAL_##pair_mode(cal_item, efuse, ##__VA_ARGS__)	\
	} else if ((!efs_valid && ret) ||			\
		   (!ret && strcmp("sw", cal_mode_get) == 0)) {	\
		CAL_##pair_mode(cal_item, sw)			\
	}

#define EFUSE_CAL(cal_item, cal_mode_get, pair_mode, ...)	\
	if ((efs_valid && ret) ||				\
	    (efs_valid && !ret && strcmp("efuse", cal_mode_get) == 0)) {\
		CAL_##pair_mode(cal_item, efuse, ##__VA_ARGS__)	\
	}

#define CAL_FLOW(cal_item, cal_mode, cal_mode_get, pair_mode,...)	\
	ret = of_property_read_string(phydev->mdio.dev.of_node,		\
			#cal_item, &cal_mode_get);			\
	cal_mode##_CAL(cal_item, cal_mode_get, pair_mode, ##__VA_ARGS__)\
	else {								\
		dev_info(&phydev->mdio.dev, "%s cal mode %s%s,"		\
			 " use default value,"				\
			 " efs-valid: %s",				\
			 #cal_item,					\
			 ret? "" : cal_mode_get,			\
			 ret? "not specified" : " not supported",	\
			 efs_valid? "yes" : "no");			\
	}								\
	if(cal_ret) {							\
		dev_err(&phydev->mdio.dev, "%s cal failed\n", #cal_item);\
		ret = -EIO;						\
		goto out;						\
	}

static int mtk_gephy_read_page(struct phy_device *phydev)
{
	return __phy_read(phydev, MTK_EXT_PAGE_ACCESS);
}

static int mtk_gephy_write_page(struct phy_device *phydev, int page)
{
	return __phy_write(phydev, MTK_EXT_PAGE_ACCESS, page);
}

/*
 * One calibration cycle consists of:
 * 1.Set DA_CALIN_FLAG high to start calibration. Keep it high
 *   until AD_CAL_COMP is ready to output calibration result.
 * 2.Wait until DA_CAL_CLK is available.
 * 3.Fetch AD_CAL_COMP_OUT.
 */
static int cal_cycle(struct phy_device *phydev, int devad,
		u32 regnum, u16 mask, u16 cal_val)
{
	unsigned long timeout;
	int reg_val;
	int ret;

	phy_modify_mmd(phydev, devad, regnum,
			mask, cal_val);
	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG17C,
			MTK_PHY_DA_CALIN_FLAG);

	timeout = jiffies + usecs_to_jiffies(ANALOG_INTERNAL_OPERATION_MAX_US);
	do{
		reg_val = phy_read_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG17B);
	} while(time_before(jiffies, timeout) && !(reg_val & BIT(0)));

	if(!(reg_val & BIT(0))) {
		dev_err(&phydev->mdio.dev, "Calibration cycle timeout\n");
		return -ETIMEDOUT;
	}

	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG17C,
			MTK_PHY_DA_CALIN_FLAG);
	ret = phy_read_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG17A) >>
			MTK_PHY_AD_CAL_COMP_OUT_SHIFT;
	dev_dbg(&phydev->mdio.dev, "cal_val: 0x%x, ret: %d\n", cal_val, ret);

	return ret;
}

static int rext_fill_result(struct phy_device *phydev, u16 *buf)
{
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5,
			MTK_PHY_RG_REXT_TRIM_MASK, buf[0] << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_RG_DEV1F_REG115,
			MTK_PHY_RG_BG_RASEL_MASK, buf[1]);

	return 0;
}

static int rext_cal_efuse(struct phy_device *phydev, u32 *buf)
{
	u16 rext_cal_val[2];

	rext_cal_val[0] = EFS_RG_REXT_TRIM(buf[3]);
	rext_cal_val[1] = EFS_RG_BG_RASEL(buf[3]);
	rext_fill_result(phydev, rext_cal_val);

	return 0;
}

static int rext_cal_sw(struct phy_device *phydev)
{
	u8 rg_zcal_ctrl_def;
	u8 zcal_lower, zcal_upper, rg_zcal_ctrl;
	u8 lower_ret, upper_ret;
	u16 rext_cal_val[2];
	int ret;

	phy_modify_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_ANA_TEST_BUS_CTRL_RG,
		MTK_PHY_ANA_TEST_MODE_MASK, MTK_PHY_TANA_CAL_MODE << 8);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
		MTK_PHY_RG_TXVOS_CALEN);
	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
		MTK_PHY_RG_CAL_CKINV | MTK_PHY_RG_ANA_CALEN | MTK_PHY_RG_REXT_CALEN);
	phy_modify_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_RG_DEV1F_REG110,
		MTK_PHY_RG_TST_DMY2_MASK, 0x1);

	rg_zcal_ctrl_def = phy_read_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5) &
					MTK_PHY_RG_ZCAL_CTRL_MASK;
	zcal_lower = ZCAL_CTRL_MIN;
	zcal_upper = ZCAL_CTRL_MAX;

	dev_dbg(&phydev->mdio.dev, "Start REXT SW cal.\n");
	while((zcal_upper-zcal_lower) > 1) {
		rg_zcal_ctrl = DIV_ROUND_CLOSEST(zcal_lower+zcal_upper, 2);
		ret = cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5,
				MTK_PHY_RG_ZCAL_CTRL_MASK, rg_zcal_ctrl);
		if(ret == 1) {
			zcal_upper = rg_zcal_ctrl;
			upper_ret = ret;
		} else if(ret == 0) {
			zcal_lower = rg_zcal_ctrl;
			lower_ret = ret;
		} else
			goto restore;
	}

	if(zcal_lower == ZCAL_CTRL_MIN) {
		ret = lower_ret = cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5,
				MTK_PHY_RG_ZCAL_CTRL_MASK, zcal_lower);
	} else if(zcal_upper == ZCAL_CTRL_MAX) {
		ret = upper_ret = cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5,
				MTK_PHY_RG_ZCAL_CTRL_MASK, zcal_upper);
	}
	if (ret < 0)
		goto restore;

	ret = upper_ret-lower_ret;
	if (ret == 1) {
		rext_cal_val[0] = zcal_upper;
		rext_cal_val[1] = zcal_upper >> 3;
		rext_fill_result(phydev, rext_cal_val);
		dev_info(&phydev->mdio.dev, "REXT SW cal result: 0x%x\n", zcal_upper);
		ret = 0;
	} else
		ret = -EINVAL;

restore:
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_ANA_TEST_BUS_CTRL_RG,
			   MTK_PHY_ANA_TEST_MODE_MASK);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
			   MTK_PHY_RG_CAL_CKINV | MTK_PHY_RG_ANA_CALEN | MTK_PHY_RG_REXT_CALEN);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_RG_DEV1F_REG110,
			   MTK_PHY_RG_TST_DMY2_MASK);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5,
		       MTK_PHY_RG_ZCAL_CTRL_MASK, rg_zcal_ctrl_def);

	return ret;
}

static int tx_offset_fill_result(struct phy_device *phydev, u16 *buf)
{
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG172,
		       MTK_PHY_CR_TX_AMP_OFFSET_A_MASK, buf[0] << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG172,
		       MTK_PHY_CR_TX_AMP_OFFSET_B_MASK, buf[1]);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG173,
		       MTK_PHY_CR_TX_AMP_OFFSET_C_MASK, buf[2] << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG173,
		       MTK_PHY_CR_TX_AMP_OFFSET_D_MASK, buf[3]);

	return 0;
}

static int tx_offset_cal_efuse(struct phy_device *phydev, u32 *buf)
{
	u16 tx_offset_cal_val[4];

	tx_offset_cal_val[0] = EFS_DA_TX_AMP_OFFSET_A(buf[0]);
	tx_offset_cal_val[1] = EFS_DA_TX_AMP_OFFSET_B(buf[1]);
	tx_offset_cal_val[2] = EFS_DA_TX_AMP_OFFSET_C(buf[1]);
	tx_offset_cal_val[3] = EFS_DA_TX_AMP_OFFSET_D(buf[1]);

	tx_offset_fill_result(phydev, tx_offset_cal_val);

	return 0;
}

static int tx_amp_fill_result(struct phy_device *phydev, u16 *buf)
{
	int i;
	int bias[16] = {0};
	switch(phydev->drv->phy_id) {
		case 0x03a29461:
		{
			/* We add some calibration to efuse values
			 * due to board level influence.
			 * GBE: +7, TBT: +1, HBT: +4, TST: +7
			 */
			int tmp[16] = { 7, 1, 4, 7,
					7, 1, 4, 7,
					7, 1, 4, 7,
					7, 1, 4, 7 };
			memcpy(bias, (const void *)tmp, sizeof(bias));
			break;
		}
		case 0x03a29481:
		{
			int tmp[16] = { 10, 6, 6, 10,
					10, 6, 6, 10,
					10, 6, 6, 10,
					10, 6, 6, 10 };
			memcpy(bias, (const void *)tmp, sizeof(bias));
			break;
		}
		default:
			break;
	}

	for (i = 0; i < 12; i += 4) {
		if (likely(buf[i>>2] + bias[i] >= 32)) {
			bias[i] -= 13;
		} else {
			phy_modify_mmd(phydev, MDIO_MMD_VEND1, 0x5c,
				0xf << (12-i), 0x6 << (12-i));
			bias[i+1] += 13;
			bias[i+2] += 13;
			bias[i+3] += 13;
		}
	}

	/* Prevent overflow */
	for (i = 0; i < 12; i++) {
		if (buf[i>>2] + bias[i] > 63) {
			buf[i>>2] = 63;
			bias[i] = 0;
		} else if (buf[i>>2] + bias[i] < 0) {
			/* Bias caused by board design may change in the future.
			 * So check negative cases, too.
			 */
			buf[i>>2] = 0;
			bias[i] = 0;
		}
	}

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TXVLD_DA_RG,
		       MTK_PHY_DA_TX_I2MPB_A_GBE_MASK, (buf[0] + bias[0]) << 10);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TXVLD_DA_RG,
		       MTK_PHY_DA_TX_I2MPB_A_TBT_MASK, buf[0] + bias[1]);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_A2,
		       MTK_PHY_DA_TX_I2MPB_A_HBT_MASK, (buf[0] + bias[2]) << 10);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_A2,
		       MTK_PHY_DA_TX_I2MPB_A_TST_MASK, buf[0] + bias[3]);

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_B1,
		       MTK_PHY_DA_TX_I2MPB_B_GBE_MASK, (buf[1] + bias[4]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_B1,
		       MTK_PHY_DA_TX_I2MPB_B_TBT_MASK, buf[1] + bias[5]);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_B2,
		       MTK_PHY_DA_TX_I2MPB_B_HBT_MASK, (buf[1] + bias[6]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_B2,
		       MTK_PHY_DA_TX_I2MPB_B_TST_MASK, buf[1] + bias[7]);

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_C1,
		       MTK_PHY_DA_TX_I2MPB_C_GBE_MASK, (buf[2] + bias[8]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_C1,
		       MTK_PHY_DA_TX_I2MPB_C_TBT_MASK, buf[2] + bias[9]);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_C2,
		       MTK_PHY_DA_TX_I2MPB_C_HBT_MASK, (buf[2] + bias[10]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_C2,
		       MTK_PHY_DA_TX_I2MPB_C_TST_MASK, buf[2] + bias[11]);

	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_D1,
		       MTK_PHY_DA_TX_I2MPB_D_GBE_MASK, (buf[3] + bias[12]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_D1,
		       MTK_PHY_DA_TX_I2MPB_D_TBT_MASK, buf[3] + bias[13]);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_D2,
		       MTK_PHY_DA_TX_I2MPB_D_HBT_MASK, (buf[3] + bias[14]) << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_D2,
		       MTK_PHY_DA_TX_I2MPB_D_TST_MASK, buf[3] + bias[15]);

	return 0;
}

static int tx_amp_cal_efuse(struct phy_device *phydev, u32 *buf)
{
	u16 tx_amp_cal_val[4];

	tx_amp_cal_val[0] = EFS_DA_TX_I2MPB_A(buf[0]);
	tx_amp_cal_val[1] = EFS_DA_TX_I2MPB_B(buf[0]);
	tx_amp_cal_val[2] = EFS_DA_TX_I2MPB_C(buf[0]);
	tx_amp_cal_val[3] = EFS_DA_TX_I2MPB_D(buf[0]);
	tx_amp_fill_result(phydev, tx_amp_cal_val);

	return 0;
}

static int tx_r50_fill_result(struct phy_device *phydev, u16 *buf,
			      phy_cal_pair_t txg_calen_x)
{
	int bias[4] = {0};
	int i;
	switch(phydev->drv->phy_id) {
		case 0x03a29481:
		{
			int tmp[16] = { -1, -1, -1, -1 };
			memcpy(bias, (const void *)tmp, sizeof(bias));
			break;
		}
		/* 0x03a29461 enters default case */
		default:
			break;
	}

	for (i = 0; i < 4; i++) {
		if (buf[i>>2] + bias[i] > 63) {
			buf[i>>2] = 63;
			bias[i] = 0;
		} else if (buf[i>>2] + bias[i] < 0) {
			buf[i>>2] = 0;
			bias[i] = 0;
		}
	}
	switch(txg_calen_x) {
		case PAIR_A:
			phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG53D,
				       MTK_PHY_DA_TX_R50_A_NORMAL_MASK, (buf[0] + bias[0]) << 8);
			phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG53D,
				       MTK_PHY_DA_TX_R50_A_TBT_MASK, (buf[0]) + bias[0]);
			break;
		case PAIR_B:
			phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG53E,
				       MTK_PHY_DA_TX_R50_B_NORMAL_MASK, (buf[0] + bias[1])<< 8);
			phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG53E,
				       MTK_PHY_DA_TX_R50_B_TBT_MASK, (buf[0] + bias[1]));
			break;
		case PAIR_C:
			phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG53F,
				       MTK_PHY_DA_TX_R50_C_NORMAL_MASK, (buf[0] + bias[2])<< 8);
			phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG53F,
				       MTK_PHY_DA_TX_R50_C_TBT_MASK, (buf[0] + bias[2]));
			break;
		case PAIR_D:
			phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG540,
				       MTK_PHY_DA_TX_R50_D_NORMAL_MASK, (buf[0] + bias[3])<< 8);
			phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG540,
				       MTK_PHY_DA_TX_R50_D_TBT_MASK, (buf[0] + bias[3]));
			break;
	}
	return 0;
}

static int tx_r50_cal_efuse(struct phy_device *phydev, u32 *buf,
			    phy_cal_pair_t txg_calen_x)
{
	u16 tx_r50_cal_val[1];

	switch(txg_calen_x) {
		case PAIR_A:
			tx_r50_cal_val[0] = EFS_DA_TX_R50_A(buf[1]);
			break;
		case PAIR_B:
			tx_r50_cal_val[0] = EFS_DA_TX_R50_B(buf[1]);
			break;
		case PAIR_C:
			tx_r50_cal_val[0] = EFS_DA_TX_R50_C(buf[2]);
			break;
		case PAIR_D:
			tx_r50_cal_val[0] = EFS_DA_TX_R50_D(buf[2]);
			break;
	}
	tx_r50_fill_result(phydev, tx_r50_cal_val, txg_calen_x);

	return 0;
}

static int tx_r50_cal_sw(struct phy_device *phydev, phy_cal_pair_t txg_calen_x)
{
	u8 rg_zcal_ctrl_def;
	u8 zcal_lower, zcal_upper, rg_zcal_ctrl;
	u8 lower_ret, upper_ret;
	u16 tx_r50_cal_val[1];
	int ret;

	phy_modify_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_ANA_TEST_BUS_CTRL_RG,
		MTK_PHY_ANA_TEST_MODE_MASK, MTK_PHY_TANA_CAL_MODE << 8);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
		MTK_PHY_RG_TXVOS_CALEN);
	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
		MTK_PHY_RG_CAL_CKINV | MTK_PHY_RG_ANA_CALEN);
	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG2,
		BIT(txg_calen_x * 4));
	phy_modify_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_RG_DEV1F_REG110,
		MTK_PHY_RG_TST_DMY2_MASK, 0x1);

	rg_zcal_ctrl_def = phy_read_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5) &
				MTK_PHY_RG_ZCAL_CTRL_MASK;
	zcal_lower = ZCAL_CTRL_MIN;
	zcal_upper = ZCAL_CTRL_MAX;

	dev_dbg(&phydev->mdio.dev, "Start TX-R50 Pair%c SW cal.\n", pair[txg_calen_x]);
	while((zcal_upper-zcal_lower) > 1) {
		rg_zcal_ctrl = DIV_ROUND_CLOSEST(zcal_lower+zcal_upper, 2);
		ret = cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5,
				MTK_PHY_RG_ZCAL_CTRL_MASK, rg_zcal_ctrl);
		if(ret==1) {
			zcal_upper = rg_zcal_ctrl;
			upper_ret = ret;
		} else if(ret==0) {
			zcal_lower = rg_zcal_ctrl;
			lower_ret = ret;
		} else
			goto restore;
	}

	if(zcal_lower == ZCAL_CTRL_MIN) {
		ret = lower_ret = cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5,
				MTK_PHY_RG_ZCAL_CTRL_MASK, zcal_lower);
	} else if(zcal_upper == ZCAL_CTRL_MAX) {
		ret = upper_ret = cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5,
				MTK_PHY_RG_ZCAL_CTRL_MASK, zcal_upper);
	}
	if (ret < 0)
		goto restore;

	ret = upper_ret-lower_ret;
	if (ret == 1) {
		tx_r50_cal_val[0] = mt798x_zcal_to_r50[zcal_upper];
		tx_r50_fill_result(phydev, tx_r50_cal_val, txg_calen_x);
		dev_info(&phydev->mdio.dev, "TX-R50 Pair%c SW cal result: 0x%x\n",
			pair[txg_calen_x], zcal_lower);
		ret = 0;
	} else
		ret = -EINVAL;

restore:
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_ANA_TEST_BUS_CTRL_RG,
		MTK_PHY_ANA_TEST_MODE_MASK);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
		MTK_PHY_RG_CAL_CKINV | MTK_PHY_RG_ANA_CALEN);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG2,
		BIT(txg_calen_x * 4));
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_RG_DEV1F_REG110,
		MTK_PHY_RG_TST_DMY2_MASK);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG5,
		MTK_PHY_RG_ZCAL_CTRL_MASK, rg_zcal_ctrl_def);

	return ret;
}

static int tx_vcm_cal_sw(struct phy_device *phydev, phy_cal_pair_t rg_txreserve_x)
{
	u8 lower_idx, upper_idx, txreserve_val;
	u8 lower_ret, upper_ret;
	int ret;

	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
		MTK_PHY_RG_ANA_CALEN);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
		MTK_PHY_RG_CAL_CKINV);
	phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
		MTK_PHY_RG_TXVOS_CALEN);

	switch(rg_txreserve_x) {
		case PAIR_A:
			phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG17D,
				MTK_PHY_DASN_DAC_IN0_A_MASK);
			phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG181,
				MTK_PHY_DASN_DAC_IN1_A_MASK);
			phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
				MTK_PHY_RG_ZCALEN_A);
			break;
		case PAIR_B:
			phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG17E,
				MTK_PHY_DASN_DAC_IN0_B_MASK);
			phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG182,
				MTK_PHY_DASN_DAC_IN1_B_MASK);
			phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
				MTK_PHY_RG_ZCALEN_B);
			break;
		case PAIR_C:
			phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG17F,
				MTK_PHY_DASN_DAC_IN0_C_MASK);
			phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG183,
				MTK_PHY_DASN_DAC_IN1_C_MASK);
			phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
				MTK_PHY_RG_ZCALEN_C);
			break;
		case PAIR_D:
			phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG180,
				MTK_PHY_DASN_DAC_IN0_D_MASK);
			phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG184,
				MTK_PHY_DASN_DAC_IN1_D_MASK);
			phy_set_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
				MTK_PHY_RG_ZCALEN_D);
			break;
		default:
			ret = -EINVAL;
			goto restore;
	}

	lower_idx = TXRESERVE_MIN;
	upper_idx = TXRESERVE_MAX;

	dev_dbg(&phydev->mdio.dev, "Start TX-VCM SW cal.\n");
	while((upper_idx-lower_idx) > 1) {
		txreserve_val = DIV_ROUND_CLOSEST(lower_idx+upper_idx, 2);
		ret = cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RXADC_CTRL_RG9,
				MTK_PHY_DA_RX_PSBN_TBT_MASK | MTK_PHY_DA_RX_PSBN_HBT_MASK |
				MTK_PHY_DA_RX_PSBN_GBE_MASK | MTK_PHY_DA_RX_PSBN_LP_MASK,
				txreserve_val << 12 | txreserve_val << 8 |
				txreserve_val << 4 | txreserve_val);
		if(ret==1) {
			upper_idx = txreserve_val;
			upper_ret = ret;
		} else if(ret==0) {
			lower_idx = txreserve_val;
			lower_ret = ret;
		} else
			goto restore;
	}

	if(lower_idx == TXRESERVE_MIN) {
		ret = lower_ret = cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RXADC_CTRL_RG9,
				MTK_PHY_DA_RX_PSBN_TBT_MASK | MTK_PHY_DA_RX_PSBN_HBT_MASK |
				MTK_PHY_DA_RX_PSBN_GBE_MASK | MTK_PHY_DA_RX_PSBN_LP_MASK,
				lower_idx << 12 | lower_idx << 8 | lower_idx << 4 | lower_idx);
	} else if(upper_idx == TXRESERVE_MAX) {
		ret = upper_ret = cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RXADC_CTRL_RG9,
				MTK_PHY_DA_RX_PSBN_TBT_MASK | MTK_PHY_DA_RX_PSBN_HBT_MASK |
				MTK_PHY_DA_RX_PSBN_GBE_MASK | MTK_PHY_DA_RX_PSBN_LP_MASK,
				upper_idx << 12 | upper_idx << 8 | upper_idx << 4 | upper_idx);
	}
	if (ret < 0)
		goto restore;

	/* We calibrate TX-VCM in different logic. Check upper index and then
	 * lower index. If this calibration is valid, apply lower index's result.
	 */
	ret = upper_ret-lower_ret;
	if (ret == 1) {
		ret = 0;
		/* Make sure we use upper_idx in our calibration system */
		cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RXADC_CTRL_RG9,
			MTK_PHY_DA_RX_PSBN_TBT_MASK | MTK_PHY_DA_RX_PSBN_HBT_MASK |
			MTK_PHY_DA_RX_PSBN_GBE_MASK | MTK_PHY_DA_RX_PSBN_LP_MASK,
			upper_idx << 12 | upper_idx << 8 | upper_idx << 4 | upper_idx);
		dev_info(&phydev->mdio.dev, "TX-VCM SW cal result: 0x%x\n", upper_idx);
	} else if (lower_idx == TXRESERVE_MIN && upper_ret == 1 && lower_ret == 1) {
		ret = 0;
		cal_cycle(phydev, MDIO_MMD_VEND1, MTK_PHY_RXADC_CTRL_RG9,
			MTK_PHY_DA_RX_PSBN_TBT_MASK | MTK_PHY_DA_RX_PSBN_HBT_MASK |
			MTK_PHY_DA_RX_PSBN_GBE_MASK | MTK_PHY_DA_RX_PSBN_LP_MASK,
			lower_idx << 12 | lower_idx << 8 | lower_idx << 4 | lower_idx);
		dev_warn(&phydev->mdio.dev, "TX-VCM SW cal result at low margin 0x%x\n", lower_idx);
	} else if (upper_idx == TXRESERVE_MAX && upper_ret == 0 && lower_ret == 0) {
		ret = 0;
		dev_warn(&phydev->mdio.dev, "TX-VCM SW cal result at high margin 0x%x\n", upper_idx);
	} else
		ret = -EINVAL;

restore:
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
		MTK_PHY_RG_ANA_CALEN);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
		MTK_PHY_RG_TXVOS_CALEN);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG0,
		MTK_PHY_RG_ZCALEN_A);
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_ANA_CAL_RG1,
		MTK_PHY_RG_ZCALEN_B | MTK_PHY_RG_ZCALEN_C | MTK_PHY_RG_ZCALEN_D);

	return ret;
}

static void mtk_gephy_config_init(struct phy_device *phydev)
{
	/* Disable EEE */
	phy_write_mmd(phydev, MDIO_MMD_AN, MDIO_AN_EEE_ADV, 0);

	/* Enable HW auto downshift */
	phy_modify_paged(phydev, MTK_PHY_PAGE_EXTENDED, 0x14, 0, BIT(4));

	/* Increase SlvDPSready time */
	phy_select_page(phydev, MTK_PHY_PAGE_EXTENDED_52B5);
	__phy_write(phydev, 0x10, 0xafae);
	__phy_write(phydev, 0x12, 0x2f);
	__phy_write(phydev, 0x10, 0x8fae);
	phy_restore_page(phydev, MTK_PHY_PAGE_STANDARD, 0);

	/* Adjust 100_mse_threshold */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x123, 0xffff);

	/* Disable mcc */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0xa6, 0x300);
}

static int mt7530_phy_config_init(struct phy_device *phydev)
{
	mtk_gephy_config_init(phydev);

	/* Increase post_update_timer */
	phy_write_paged(phydev, MTK_PHY_PAGE_EXTENDED_3, 0x11, 0x4b);

	return 0;
}

static int mt7531_phy_config_init(struct phy_device *phydev)
{
	if (phydev->interface != PHY_INTERFACE_MODE_INTERNAL)
		return -EINVAL;

	mtk_gephy_config_init(phydev);

	/* PHY link down power saving enable */
	phy_set_bits(phydev, 0x17, BIT(4));
	phy_clear_bits_mmd(phydev, MDIO_MMD_VEND1, 0xc6, 0x300);

	/* Set TX Pair delay selection */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x13, 0x404);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, 0x14, 0x404);

	return 0;
}

static inline void mt7981_phy_finetune(struct phy_device *phydev)
{
	u32 i;
	/* 100M eye finetune:
	 * Keep middle level of TX MLT3 shapper as default.
	 * Only change TX MLT3 overshoot level here.
	 */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_1st_OVERSHOOT_LEVEL_0TO1, 0x1ce);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_2nd_OVERSHOOT_LEVEL_0TO1, 0x1c1);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_1st_OVERSHOOT_LEVEL_1TO0, 0x20f);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_2nd_OVERSHOOT_LEVEL_1TO0, 0x202);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_1st_OVERSHOOT_LEVEL_0TON1, 0x3d0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_2nd_OVERSHOOT_LEVEL_0TON1, 0x3c0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_1st_OVERSHOOT_LEVEL_N1TO0, 0x13);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_2nd_OVERSHOOT_LEVEL_N1TO0, 0x5);

	/* TX-AMP finetune:
	 * 100M +4, 1000M +6 to default value.
	 * If efuse values aren't valid, TX-AMP uses the below values.
	 */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TXVLD_DA_RG, 0x9824);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_A2, 0x9026);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_B1, 0x2624);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_B2, 0x2426);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_C1, 0x2624);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_C2, 0x2426);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_D1, 0x2624);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_TX_I2MPB_TEST_MODE_D2, 0x2426);

	phy_select_page(phydev, MTK_PHY_PAGE_EXTENDED_52B5);
	/* EnabRandUpdTrig = 1 */
	__phy_write(phydev, 0x11, 0x2f00);
	__phy_write(phydev, 0x12, 0xe);
	__phy_write(phydev, 0x10, 0x8fb0);

	/* SlvDSPreadyTime = 0xc */
	__phy_write(phydev, 0x11, 0x671);
	__phy_write(phydev, 0x12, 0xc);
	__phy_write(phydev, 0x10, 0x8fae);

	/* NormMseLoThresh = 85 */
	__phy_write(phydev, 0x11, 0x55a0);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x83aa);

	/* InhibitDisableDfeTail1000 = 1 */
	__phy_write(phydev, 0x11, 0x2b);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x8f80);

	/* SSTr related */
	__phy_write(phydev, 0x11, 0xbaef);
	__phy_write(phydev, 0x12, 0x2e);
	__phy_write(phydev, 0x10, 0x968c);

	/* VcoSlicerThreshBitsHigh */
	__phy_write(phydev, 0x11, 0x5555);
	__phy_write(phydev, 0x12, 0x55);
	__phy_write(phydev, 0x10, 0x8ec0);

	/* ResetSyncOffset = 6 */
	__phy_write(phydev, 0x11, 0x600);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x8fc0);

	/* VgaDecRate = 1 */
	__phy_write(phydev, 0x11, 0x4c2a);
	__phy_write(phydev, 0x12, 0x3e);
	__phy_write(phydev, 0x10, 0x8fa4);

	phy_restore_page(phydev, MTK_PHY_PAGE_STANDARD, 0);

	/* TR_OPEN_LOOP_EN = 1, lpf_x_average = 9*/
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG234,
		MTK_PHY_TR_OPEN_LOOP_EN_MASK & MTK_PHY_LPF_X_AVERAGE_MASK,
		BIT(0) & (0x9 << 4));

	/* rg_tr_lpf_cnt_val = 512 */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LPF_CNT_VAL, 0x200);

	/* IIR2 related */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K1_L, 0x82);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K1_U, 0x0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K2_L, 0x103);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K2_U, 0x0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K3_L, 0x82);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K3_U, 0x0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K4_L, 0xd177);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K4_U, 0x3);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K5_L, 0x2c82);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K5_U, 0xe);

	/* FFE peaking */
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG27C,
		MTK_PHY_VGASTATE_FFE_THR_ST1_MASK, 0x1b << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG27D,
		MTK_PHY_VGASTATE_FFE_THR_ST2_MASK, 0x1e);

	/* TX shape */
	/* 10/100/1000 TX shaper is enabled by default */
	for (i = 0x202; i < 0x230; i += 2) {
		if (i == 0x20c || i == 0x218 || i == 0x224)
			continue;
		phy_write_mmd(phydev, MDIO_MMD_VEND2, i, 0x2219);
		phy_write_mmd(phydev, MDIO_MMD_VEND2, i+1, 0x23);
	}
}

static inline void mt7988_phy_finetune(struct phy_device *phydev)
{
	int i;
	u16 val[12] = {0x0187, 0x01cd, 0x01c8, 0x0182,
		0x020d, 0x0206, 0x0384, 0x03d0,
		0x03c6, 0x030a, 0x0011, 0x0005};

	for(i=0; i<MTK_PHY_TX_MLT3_END; i++) {
		phy_write_mmd(phydev, MDIO_MMD_VEND1, i, val[i]);
	}

	/* TCT finetune */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_TX_FILTER, 0x5);

	/* Disable TX power saving */
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RXADC_CTRL_RG7,
			MTK_PHY_DA_AD_BUF_BIAS_LP_MASK, 0x3 << 8);

	phy_select_page(phydev, MTK_PHY_PAGE_EXTENDED_52B5);
	/* EnabRandUpdTrig = 1 */
	__phy_write(phydev, 0x11, 0x2f00);
	__phy_write(phydev, 0x12, 0xe);
	__phy_write(phydev, 0x10, 0x8fb0);

	/* SlvDSPreadyTime = 0xc */
	__phy_write(phydev, 0x11, 0x671);
	__phy_write(phydev, 0x12, 0xc);
	__phy_write(phydev, 0x10, 0x8fae);

	/* NormMseLoThresh = 85 */
	__phy_write(phydev, 0x11, 0x55a0);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x83aa);

	/* InhibitDisableDfeTail1000 = 1 */
	__phy_write(phydev, 0x11, 0x2b);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x8f80);

	/* SSTr related */
	__phy_write(phydev, 0x11, 0xbaef);
	__phy_write(phydev, 0x12, 0x2e);
	__phy_write(phydev, 0x10, 0x968c);

	/* MrvlTrFix100Kp = 3, MrvlTrFix100Kf = 2,
	 * MrvlTrFix1000Kp = 3, MrvlTrFix1000Kf = 2
	 */
	__phy_write(phydev, 0x11, 0xd10a);
	__phy_write(phydev, 0x12, 0x34);
	__phy_write(phydev, 0x10, 0x8f82);

	/* VcoSlicerThreshBitsHigh */
	__phy_write(phydev, 0x11, 0x5555);
	__phy_write(phydev, 0x12, 0x55);
	__phy_write(phydev, 0x10, 0x8ec0);

	/* ResetSyncOffset = 6 */
	__phy_write(phydev, 0x11, 0x600);
	__phy_write(phydev, 0x12, 0x0);
	__phy_write(phydev, 0x10, 0x8fc0);

	/* VgaDecRate = 1 */
	__phy_write(phydev, 0x11, 0x4c2a);
	__phy_write(phydev, 0x12, 0x3e);
	__phy_write(phydev, 0x10, 0x8fa4);

	phy_restore_page(phydev, MTK_PHY_PAGE_STANDARD, 0);

	/* TR_OPEN_LOOP_EN = 1, lpf_x_average = 9*/
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG234,
		MTK_PHY_TR_OPEN_LOOP_EN_MASK & MTK_PHY_LPF_X_AVERAGE_MASK,
		BIT(0) & (0x9 << 4));

	/* rg_tr_lpf_cnt_val = 512 */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LPF_CNT_VAL, 0x200);

	/* IIR2 related */
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K1_L, 0x82);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K1_U, 0x0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K2_L, 0x103);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K2_U, 0x0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K3_L, 0x82);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K3_U, 0x0);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K4_L, 0xd177);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K4_U, 0x3);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K5_L, 0x2c82);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_LP_IIR2_K5_U, 0xe);

	/* FFE peaking */
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG27C,
		MTK_PHY_VGASTATE_FFE_THR_ST1_MASK, 0x1b << 8);
	phy_modify_mmd(phydev, MDIO_MMD_VEND1, MTK_PHY_RG_DEV1E_REG27D,
		MTK_PHY_VGASTATE_FFE_THR_ST2_MASK, 0x1e);

	/* TX shape */
	/* 10/100/1000 TX shaper is enabled by default */
	for (i = 0x202; i < 0x230; i += 2) {
		if (i == 0x20c || i == 0x218 || i == 0x224)
			continue;
		phy_write_mmd(phydev, MDIO_MMD_VEND2, i, 0x2219);
		phy_write_mmd(phydev, MDIO_MMD_VEND2, i+1, 0x23);
	}
}

static int mt798x_phy_calibration(struct phy_device *phydev)
{
	const char *cal_mode_from_dts;
	int i, ret;
	int cal_ret = 0;
	u32 *buf;
	bool efs_valid = true;
	size_t len;
	struct nvmem_cell *cell;

	if (phydev->interface != PHY_INTERFACE_MODE_GMII)
		return -EINVAL;

	cell = nvmem_cell_get(&phydev->mdio.dev, "phy-cal-data");
	if (IS_ERR(cell)) {
		if (PTR_ERR(cell) == -EPROBE_DEFER)
			return PTR_ERR(cell);
		return 0;
	}

	buf = (u32 *)nvmem_cell_read(cell, &len);
	if (IS_ERR(buf))
		return PTR_ERR(buf);
	nvmem_cell_put(cell);

	if(!buf[0] && !buf[1] && !buf[2] && !buf[3])
		efs_valid = false;

	if (len < 4 * sizeof(u32)) {
		dev_err(&phydev->mdio.dev, "invalid calibration data\n");
		ret = -EINVAL;
		goto out;
	}

	CAL_FLOW(rext, SW_EFUSE, cal_mode_from_dts, NO_PAIR, buf)
	CAL_FLOW(tx_offset, EFUSE, cal_mode_from_dts, NO_PAIR, buf)
	CAL_FLOW(tx_amp, EFUSE, cal_mode_from_dts, NO_PAIR, buf)
	CAL_FLOW(tx_r50, SW_EFUSE, cal_mode_from_dts, PAIR_A_TO_D, buf)
	CAL_FLOW(tx_vcm, SW, cal_mode_from_dts, PAIR_A_TO_A)
	ret = 0;

out:
	kfree(buf);
	return ret;
}

static int mt7981_phy_probe(struct phy_device *phydev)
{
	mt7981_phy_finetune(phydev);

	return mt798x_phy_calibration(phydev);
}

static int mt7988_phy_probe(struct phy_device *phydev)
{
	struct device_node *np;
	void __iomem *boottrap;
	u32 reg;
	int port;

	/* Setup LED polarity according to boottrap's polarity */
	np = of_find_compatible_node(NULL, NULL, "mediatek,boottrap");
	if (!np)
		return -ENOENT;
	boottrap = of_iomap(np, 0);
	if (!boottrap)
		return -ENOMEM;
	reg = readl(boottrap);
	port = phydev->mdio.addr;
	if ((port == GPHY_PORT0 && reg & BIT(8)) ||
	    (port == GPHY_PORT1 && reg & BIT(9)) ||
	    (port == GPHY_PORT2 && reg & BIT(10)) ||
	    (port == GPHY_PORT3 && reg & BIT(11))) {
		phy_write_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_LED0_ON_CTRL,
			MTK_PHY_LED0_ENABLE | MTK_PHY_LED0_ON_LINK10 |
			MTK_PHY_LED0_ON_LINK100 | MTK_PHY_LED0_ON_LINK1000);
	} else {
		phy_write_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_LED0_ON_CTRL,
			MTK_PHY_LED0_ENABLE | MTK_PHY_LED0_POLARITY |
			MTK_PHY_LED0_ON_LINK10 | MTK_PHY_LED0_ON_LINK100 |
			MTK_PHY_LED0_ON_LINK1000);
	}
	phy_write_mmd(phydev, MDIO_MMD_VEND2, MTK_PHY_LED0_BLINK_CTRL,
			MTK_PHY_LED0_1000TX | MTK_PHY_LED0_1000RX |
			MTK_PHY_LED0_100TX  | MTK_PHY_LED0_100RX  |
			MTK_PHY_LED0_10TX   | MTK_PHY_LED0_10RX);

	mt7988_phy_finetune(phydev);

	return mt798x_phy_calibration(phydev);
}

static struct phy_driver mtk_gephy_driver[] = {
#if 0
	{
		PHY_ID_MATCH_EXACT(0x03a29412),
		.name		= "MediaTek MT7530 PHY",
		.config_init	= mt7530_phy_config_init,
		/* Interrupts are handled by the switch, not the PHY
		 * itself.
		 */
		.config_intr	= genphy_no_config_intr,
		.handle_interrupt = genphy_no_ack_interrupt,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.read_page	= mtk_gephy_read_page,
		.write_page	= mtk_gephy_write_page,
	},
	{
		PHY_ID_MATCH_EXACT(0x03a29441),
		.name		= "MediaTek MT7531 PHY",
		.config_init	= mt7531_phy_config_init,
		/* Interrupts are handled by the switch, not the PHY
		 * itself.
		 */
		.config_intr	= genphy_no_config_intr,
		.handle_interrupt = genphy_no_ack_interrupt,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.read_page	= mtk_gephy_read_page,
		.write_page	= mtk_gephy_write_page,
	},
#endif
	{
		PHY_ID_MATCH_EXACT(0x03a29461),
		.name		= "MediaTek MT7981 PHY",
		.probe		= mt7981_phy_probe,
		/* Interrupts are handled by the switch, not the PHY
		 * itself.
		 */
		.config_intr	= genphy_no_config_intr,
		.handle_interrupt = genphy_no_ack_interrupt,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.read_page	= mtk_gephy_read_page,
		.write_page	= mtk_gephy_write_page,
	},
	{
		PHY_ID_MATCH_EXACT(0x03a29481),
		.name		= "MediaTek MT7988 PHY",
		.probe		= mt7988_phy_probe,
		/* Interrupts are handled by the switch, not the PHY
		 * itself.
		 */
		.config_intr	= genphy_no_config_intr,
		.handle_interrupt = genphy_no_ack_interrupt,
		.suspend	= genphy_suspend,
		.resume		= genphy_resume,
		.read_page	= mtk_gephy_read_page,
		.write_page	= mtk_gephy_write_page,
	},
};

module_phy_driver(mtk_gephy_driver);

static struct mdio_device_id __maybe_unused mtk_gephy_tbl[] = {
	{ PHY_ID_MATCH_VENDOR(0x03a29400) },
	{ }
};

MODULE_DESCRIPTION("MediaTek Gigabit Ethernet PHY driver");
MODULE_AUTHOR("DENG, Qingfang <dqfext@gmail.com>");
MODULE_LICENSE("GPL");

MODULE_DEVICE_TABLE(mdio, mtk_gephy_tbl);
