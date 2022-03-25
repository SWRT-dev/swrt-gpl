#ifndef __LEDS_XRX500_CONFIG_H
#define __LEDS_XRX500_CONFIG_H

#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/leds.h>
#include <lantiq_soc.h>
#include <linux/gpio/driver.h>

#define NUM_LEDS 32

/*
 *  Definition of Operation MASK
 */
/*!
  \def XRX500_SSO_CFG_OP_UPDATE_SHADOW
  \brief field "operation_mask" of struct "xrx500_sso_config_param" - Predefined LED ON bits.
 */
#define XRX500_SSO_CFG_OP_UPDATE_SHADOW       0x0001
/*!
  \def XRX500_SSO_CFG_OP_BLINK_MASK
  \brief field "operation_mask" of struct "xrx500_sso_config_param" - Reserve the bits needed for HW blink.
 */
#define XRX500_SSO_CFG_OP_BLINK_MASK         0x0002
/*!
  \def XRX500_SSO_CFG_OP_UPDATE_CLOCK
  \brief field "operation_mask" of struct "xrx500_sso_config_param" - Select update clock source
 */
#define XRX500_SSO_CFG_OP_UPDATE_CLOCK        0x0004
/*!
  \def XRX500_SSO_CFG_OP_UPDATE_FPID
  \brief field "operation_mask" of struct "xrx500_sso_config_param" - Select the FPID value
 */
#define XRX500_SSO_CFG_OP_UPDATE_FPID      0x0008
/*!
  \def XRX500_SSO_CFG_OP_UPDATE_FCDSC
  \brief field "operation_mask" of struct "xrx500_sso_config_param" - Select the FCDSC value
 */
#define XRX500_SSO_CFG_OP_UPDATE_FCDSC      0x0010
/*!
  \def XRX500_SSO_CFG_OP_UPDATE_GPTD
  \brief field "operation_mask" of struct "xrx500_sso_config_param" - Select the GPTD value
 */
#define XRX500_SSO_CFG_OP_UPDATE_GPTD      0x0020
/*!
  \def XRX500_SSO_CFG_OP_BLINK_RATE
  \brief field "operation_mask" of struct "xrx500_sso_config_param" - Blink rate can be different from update rate for LEDs 0-23
 */
#define XRX500_SSO_CFG_OP_BLINK_RATE      0x0040
/*!
  \def XRX500_SSO_CFG_OP_INDIVIDUAL_BLINK_SOURCE
  \brief field "operation_mask" of struct "xrx500_sso_config_param" - Blink rate can be unique for each of the LEDs from 24-31
 */
#define XRX500_SSO_CFG_OP_INDIVIDUAL_BLINK_SOURCE      0x0080
/*!
  \def XRX500_SSO_CFG_OP_BLINK_SOURCE_0_23
  \brief field "operation_mask" of struct "xrx500_sso_config_param" - Blink rate for leds 0-23, if the blink rate has to be different from update rate
 */
#define XRX500_SSO_CFG_OP_BLINK_SOURCE_0_23             0x0100
/*
* The Serial Shift Output (SSO) is found on MIPS based Lantiq xrx500 socs.
*It is a peripheral controller used to drive external shift register
*cascades. At most 4 groups of 8 bits can be driven.
*/

/* control register 0 */
#define XRX500_SSO_CON0		0x2B0
/* control register 1 */
#define XRX500_SSO_CON1		0x2B4
/* data register 0 */
#define XRX500_SSO_CPU0		0x2B8
/* data register 1 */
#define XRX500_SSO_CPU1		0x2BC
/* access register */
#define XRX500_SSO_AR		0x2C0
/* control register 2 */
#define XRX500_SSO_CON2		0x2C4
/* control register 3 */
#define XRX500_SSO_CON3		0x2C8

/*blink control*/
#define XRX500_SSO_BLINK_H8_0    0x0
#define XRX500_SSO_BLINK_H8_1    0x4
#define XRX500_SSO_BLINK_SOURCE_SHIFT 22
/*Dutycycle register*/
#define XRX500_SSO_DUTYCYCLE_BASE      0x8
#define XRX500_SSO_DUTYCYCLE_OFFSET    0x4

/* software or hardware update select bit */
#define XRX500_SSO_CON_SWU	BIT(31)

/* Blink rate select bit */
#define XRX500_SSO_BLINK_RATE_SHIFT	30
#define XRX500_SSO_BLINK_RATE_MASK	BIT(30)

/* clock source for update */
#define XRX500_SSO_UPD_SHIFT	30
#define XRX500_SSO_UPD_MASK	0xc0000000


/* FPID */
#define XRX500_SSO_FPID_SHIFT	23
#define XRX500_SSO_FPID_MASK	0x1800000

/* FCDSE */
#define XRX500_SSO_FCDSC_SHIFT	20
#define XRX500_SSO_FCDSC_MASK	0x300000

/* GPTD */
#define XRX500_SSO_GPTD_SHIFT	25
#define XRX500_SSO_GPTD_MASK	0x6000000

/*PHY control*/
#define XRX500_SSO_PHY_MASK     0x7
#define XRX500_SSO_LD0_MASK 	0x1F
#define XRX500_SSO_LD1_MASK     0x1F00
#define XRX500_SSO_LD2_MASK     0x1F0000

#define XRX500_SSO_LD0_SHIFT	0
#define XRX500_SSO_LD1_SHIFT	8
#define XRX500_SSO_LD2_SHIFT	16

#define XRX500_SSO_PHY6_SHIFT_FACTOR	0
#define XRX500_SSO_PHY2_SHIFT_FACTOR	1
#define XRX500_SSO_PHY3_SHIFT_FACTOR	2
#define XRX500_SSO_PHY4_SHIFT_FACTOR    3
#define XRX500_SSO_PHY5_SHIFT_FACTOR    4

#define XRX500_SSO_LD0_PATTERN(phy6, phy2, phy3, phy4, phy5) \
						(((phy6 & BIT(0))\
						| ((phy2 & BIT(0)) << 1)\
						| ((phy3 & BIT(0)) << 2)\
						|((phy4 & BIT(0)) << 3)\
						|((phy5 & BIT(0)) << 4)\
						) << XRX500_SSO_LD0_SHIFT)
#define XRX500_SSO_LD1_PATTERN(phy6, phy2, phy3, phy4, phy5) \
					((((phy6 & BIT(1)) >> 1)\
					| (((phy2 & BIT(1)) >> 1) << 1)\
					| (((phy3 & BIT(1)) >> 1) << 2)\
					| (((phy4 & BIT(1)) >> 1) << 3)\
					| (((phy5 & BIT(1)) >> 1) << 4)\
					) << XRX500_SSO_LD1_SHIFT)
#define XRX500_SSO_LD2_PATTERN(phy6, phy2, phy3, phy4, phy5) \
					((((phy6 & BIT(2)) >> 2)\
					| (((phy2 & BIT(2)) >> 2) << 1)\
					| (((phy3 & BIT(2)) >> 2) << 2)\
					| (((phy4 & BIT(2)) >> 2) << 3)\
					| (((phy5 & BIT(2)) >> 2) << 4)\
					) << XRX500_SSO_LD2_SHIFT)

/*WLAN control*/
#define XRX500_SSO_WLAN_MASK	0x7
#define XRX500_SSO_W1_W2_W3_MASK	0xE0
#define XRX500_SSO_W4_W5_W6_MASK	0xE000

#define XRX500_SSO_W1_W2_W3_SHIFT	5
#define XRX500_SSO_W4_W5_W6_SHIFT	13

#define XRX500_SSO_W1_W2_W3_PATTERN(wlan)	\
	((wlan & 0x7) << XRX500_SSO_W1_W2_W3_SHIFT)
#define XRX500_SSO_W4_W5_W6_PATTERN(wlan)	\
	(((wlan & 0x38) >> 7) << XRX500_SSO_W4_W5_W6_SHIFT)

/*SSO has 3 groups of 8 bits */
#define XRX500_SSO_GROUP0		BIT(0)
#define XRX500_SSO_GROUP1		BIT(1)
#define XRX500_SSO_GROUP2		BIT(2)
#define XRX500_SSO_GROUP3		BIT(3)
#define XRX500_SSO_GROUP_MASK		(0xF)

/* Edge configuration bits */
#define XRX500_SSO_FALLING	BIT(26)
#define XRX500_SSO_EDGE_MASK	BIT(26)
#define XRX500_SSO_EDGE_SHIFT 26

/*Dutycycle configuration bit*/
#define XRX500_SSO_DUTYCYCLE(led)	\
	(XRX500_SSO_DUTYCYCLE_BASE + (led * XRX500_SSO_DUTYCYCLE_OFFSET))

/*Auto Dimming function*/
#define XRX500_SSO_DIMM_ENABLE_MASK 0x8000000
#define XRX500_SSO_DIMM_ENABLE_SHIFT 27
#define XRX500_SSO_LUX_INTERVAL_MASK 0xF00
#define XRX500_SSO_LUX_INTERVAL_SHIFT 8
#define XRX500_SSO_LUX_DISCHARGE_MASK 0x3F000
#define XRX500_SSO_LUX_DISCHARGE_SHIFT 12
#define XRX500_SSO_LUX_LEVEL_MASK 0xFF
#define XRX500_SSO_LUX_LEVEL_SHIFT 0

/*Data offset*/
#define XRX500_SSO_DATA_OFFSET_MASK 0xc0000
#define XRX500_SSO_DATA_OFFSET_SHIFT 18

/*Store Mode*/
#define XRX500_SSO_STORE_MODE_MASK 0x10000000
#define XRX500_SSO_STORE_MODE_SHIFT 28

#define xrx500_sso_r32(m, reg)		__raw_readl(m + reg)
#define xrx500_sso_w32(m, val, reg)	__raw_writel(val, m + reg)
#define xrx500_sso_w32_mask(m, clear, set, reg) \
		ltq_w32((ltq_r32(m + reg) & ~(clear)) | (set), \
		m + reg)
		
enum xrx500_sso_CON1_us
{
	LED_CON1_UPDATE_SRC_SW = 0,
	LED_CON1_UPDATE_SRC_GPTD2,
	LED_CON1_UPDATE_SRC_FPID,
};

enum xrx500_sso_H8_us
{
	LED_H8_UPDATE_SRC_FPID = 0,
	LED_H8_UPDATE_SRC_GPTD,
	LED_H8_UPDATE_SRC_VIM,
};

enum xrx500_sso_fsc_divider 
{
	LED_FSC_2HZ = 0,
	LED_FSC_4HZ,
	LED_FSC_8HZ,
	LED_FSC_10HZ,
};

enum xrx500_sso_gptd_divider
{
	LED_GPTD_50HZ = 0,
	LED_GPTD_100HZ,
	LED_GPTD_200HZ,
	LED_GPTD_250HZ,
};


struct xrx500_sso_freq_table {
	u32 value;
	u32 range_start;
	u32 range_end;
};

struct xrx500_sso {
	struct gpio_chip gc;
	void __iomem *virt;
	u32 edge;	/* rising or falling edge triggered shift register */
	u32 shadow;	/* shadow the shift registers state */
	u8 groups;	/* we can drive 1 groups of 8bit each */
	u8 phy2;	/* 3 bits can be driven by phy2 */
	u8 phy3;	/* 3 bits can be driven by phy3 */
	u8 phy4;	/* 3 bits can be driven by phy4 */
	u8 phy5;	/* 3 bits can be driven by phy5 */
	u8 phy6;	/* 3 bits can be driven by phy6 */
	u8 upd;	/* 2 bits for update source*/
	u32 blink_mask;
	u32 blink_rate;
	u32 blink_source_24_31[8];
	u32 blink_source_0_23;
	u8 wlan;	/*6 bits for WLAN*/
	u8 reserved;	/* mask out the hw driven bits in gpio_request */
};

struct xrx500_sso_led {
	struct led_classdev     device;             /*!< LED device */
	const char *name;
	const char *default_trigger;
	unsigned	phys_id;
	unsigned	active_low:1;
	unsigned	retain_state_suspended:1;
	unsigned	default_state:2;
	struct work_struct work;
	u8 new_level;
	u8 can_sleep;
	u8 blinking;
};

int get_cpu_select(void);
int grx500_register_sysfs(struct platform_device *pdev);
#endif
