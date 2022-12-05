#ifndef QSFP_DD_H__
#define QSFP_DD_H__

/* Identifier and revision compliance (Page 0) */
#define QSFP_DD_ID_OFFSET			0x00
#define QSFP_DD_REV_COMPLIANCE_OFFSET		0x01

#define QSFP_DD_MODULE_TYPE_OFFSET		0x55
#define QSFP_DD_MT_MMF				0x01
#define QSFP_DD_MT_SMF				0x02

/* Module-Level Monitors (Page 0) */
#define QSFP_DD_CURR_TEMP_OFFSET		0x0E
#define QSFP_DD_CURR_CURR_OFFSET		0x10

#define QSFP_DD_CTOR_OFFSET			0xCB

/* Vendor related information (Page 0) */
#define QSFP_DD_VENDOR_NAME_START_OFFSET	0x81
#define QSFP_DD_VENDOR_NAME_END_OFFSET		0x90

#define QSFP_DD_VENDOR_OUI_OFFSET		0x91

#define QSFP_DD_VENDOR_PN_START_OFFSET		0x94
#define QSFP_DD_VENDOR_PN_END_OFFSET		0xA3

#define QSFP_DD_VENDOR_REV_START_OFFSET		0xA4
#define QSFP_DD_VENDOR_REV_END_OFFSET		0xA5

#define QSFP_DD_VENDOR_SN_START_OFFSET		0xA6
#define QSFP_DD_VENDOR_SN_END_OFFSET		0xB5

#define QSFP_DD_DATE_YEAR_OFFSET		0xB6
#define QSFP_DD_DATE_VENDOR_LOT_OFFSET		0xBC

/* CLEI Code (Page 0) */
#define QSFP_DD_CLEI_PRESENT_BYTE		0x02
#define QSFP_DD_CLEI_PRESENT_MASK		0x20
#define QSFP_DD_CLEI_START_OFFSET		0xBE
#define QSFP_DD_CLEI_END_OFFSET			0xC7

/* Cable assembly length */
#define QSFP_DD_CBL_ASM_LEN_OFFSET		0xCA
#define QSFP_DD_6300M_MAX_LEN			0xFF

/* Cable length with multiplier */
#define QSFP_DD_MULTIPLIER_00			0x00
#define QSFP_DD_MULTIPLIER_01			0x40
#define QSFP_DD_MULTIPLIER_10			0x80
#define QSFP_DD_MULTIPLIER_11			0xC0
#define QSFP_DD_LEN_MUL_MASK			0xC0
#define QSFP_DD_LEN_VAL_MASK			0x3F

/* Module power characteristics */
#define QSFP_DD_PWR_CLASS_OFFSET		0xC8
#define QSFP_DD_PWR_MAX_POWER_OFFSET		0xC9
#define QSFP_DD_PWR_CLASS_MASK			0xE0
#define QSFP_DD_PWR_CLASS_1			0x00
#define QSFP_DD_PWR_CLASS_2			0x01
#define QSFP_DD_PWR_CLASS_3			0x02
#define QSFP_DD_PWR_CLASS_4			0x03
#define QSFP_DD_PWR_CLASS_5			0x04
#define QSFP_DD_PWR_CLASS_6			0x05
#define QSFP_DD_PWR_CLASS_7			0x06
#define QSFP_DD_PWR_CLASS_8			0x07

/* Copper cable attenuation */
#define QSFP_DD_COPPER_ATT_5GHZ			0xCC
#define QSFP_DD_COPPER_ATT_7GHZ			0xCD
#define QSFP_DD_COPPER_ATT_12P9GHZ		0xCE
#define QSFP_DD_COPPER_ATT_25P8GHZ		0xCF

/* Cable assembly lane */
#define QSFP_DD_CABLE_ASM_NEAR_END_OFFSET	0xD2
#define QSFP_DD_CABLE_ASM_FAR_END_OFFSET	0xD3

/* Media interface technology */
#define QSFP_DD_MEDIA_INTF_TECH_OFFSET		0xD4
#define QSFP_DD_850_VCSEL			0x00
#define QSFP_DD_1310_VCSEL			0x01
#define QSFP_DD_1550_VCSEL			0x02
#define QSFP_DD_1310_FP				0x03
#define QSFP_DD_1310_DFB			0x04
#define QSFP_DD_1550_DFB			0x05
#define QSFP_DD_1310_EML			0x06
#define QSFP_DD_1550_EML			0x07
#define QSFP_DD_OTHERS				0x08
#define QSFP_DD_1490_DFB			0x09
#define QSFP_DD_COPPER_UNEQUAL			0x0A
#define QSFP_DD_COPPER_PASS_EQUAL		0x0B
#define QSFP_DD_COPPER_NF_EQUAL			0x0C
#define QSFP_DD_COPPER_F_EQUAL			0x0D
#define QSFP_DD_COPPER_N_EQUAL			0x0E
#define QSFP_DD_COPPER_LINEAR_EQUAL		0x0F

/*-----------------------------------------------------------------------
 * Upper Memory Page 0x01: contains advertising fields that define properties
 * that are unique to active modules and cable assemblies.
 * RealOffset = 1 * 0x80 + LocalOffset
 */
#define PAG01H_UPPER_OFFSET			(0x01 * 0x80)

/* Supported Link Length (Page 1) */
#define QSFP_DD_SMF_LEN_OFFSET			(PAG01H_UPPER_OFFSET + 0x84)
#define QSFP_DD_OM5_LEN_OFFSET			(PAG01H_UPPER_OFFSET + 0x85)
#define QSFP_DD_OM4_LEN_OFFSET			(PAG01H_UPPER_OFFSET + 0x86)
#define QSFP_DD_OM3_LEN_OFFSET			(PAG01H_UPPER_OFFSET + 0x87)
#define QSFP_DD_OM2_LEN_OFFSET			(PAG01H_UPPER_OFFSET + 0x88)

/* Wavelength (Page 1) */
#define QSFP_DD_NOM_WAVELENGTH_MSB		(PAG01H_UPPER_OFFSET + 0x8A)
#define QSFP_DD_NOM_WAVELENGTH_LSB		(PAG01H_UPPER_OFFSET + 0x8B)
#define QSFP_DD_WAVELENGTH_TOL_MSB		(PAG01H_UPPER_OFFSET + 0x8C)
#define QSFP_DD_WAVELENGTH_TOL_LSB		(PAG01H_UPPER_OFFSET + 0x8D)

/* Signal integrity controls */
#define QSFP_DD_SIG_INTEG_TX_OFFSET		(PAG01H_UPPER_OFFSET + 0xA1)
#define QSFP_DD_SIG_INTEG_RX_OFFSET		(PAG01H_UPPER_OFFSET + 0xA2)

#define YESNO(x) (((x) != 0) ? "Yes" : "No")
#define ONOFF(x) (((x) != 0) ? "On" : "Off")

void qsfp_dd_show_all(const __u8 *id);

#endif /* QSFP_DD_H__ */
