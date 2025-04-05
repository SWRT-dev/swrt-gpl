/* SPDX-License-Identifier: GPL-2.0
 * HWMON driver for Aquantia PHY
 *
 * Author: Nikita Yushchenko <nikita.yoush@cogentembedded.com>
 * Author: Andrew Lunn <andrew@lunn.ch>
 * Author: Heiner Kallweit <hkallweit1@gmail.com>
 */

#include <linux/device.h>
#include <linux/phy.h>

#define PHY_ID_AQ1202	0x03a1b445
#define PHY_ID_AQ2104	0x03a1b460
#define PHY_ID_AQR105	0x03a1b4a2
#define PHY_ID_AQR106	0x03a1b4d0
#define PHY_ID_AQR107	0x03a1b4e0
#define PHY_ID_AQCS109	0x03a1b5c2
#define PHY_ID_AQR405	0x03a1b4b0
#define PHY_ID_AQR113C	0x31c31c12
#define PHY_ID_CUX3410	0x31c31dd3

#define MDIO_AN_VEND_PROV_DOWNSHIFT_DFLT		4

#define PMAPMD_RSVD_VEND_PROV				0xe400
#define PMAPMD_RSVD_VEND_PROV_MDI_CONF			BIT(0)

/* MDIO_MMD_C22EXT */
#define MDIO_C22EXT_STAT_SGMII_RX_GOOD_FRAMES		0xd292
#define MDIO_C22EXT_STAT_SGMII_RX_BAD_FRAMES		0xd294
#define MDIO_C22EXT_STAT_SGMII_RX_FALSE_CARRIER		0xd297
#define MDIO_C22EXT_STAT_SGMII_TX_GOOD_FRAMES		0xd313
#define MDIO_C22EXT_STAT_SGMII_TX_BAD_FRAMES		0xd315
#define MDIO_C22EXT_STAT_SGMII_TX_FALSE_CARRIER		0xd317
#define MDIO_C22EXT_STAT_SGMII_TX_COLLISIONS		0xd318
#define MDIO_C22EXT_STAT_SGMII_TX_LINE_COLLISIONS	0xd319
#define MDIO_C22EXT_STAT_SGMII_TX_FRAME_ALIGN_ERR	0xd31a
#define MDIO_C22EXT_STAT_SGMII_TX_RUNT_FRAMES		0xd31b

struct aqr107_hw_stat {
	const char *name;
	int reg;
	int size;
};

#define SGMII_STAT(n, r, s) { n, MDIO_C22EXT_STAT_SGMII_ ## r, s }
static const struct aqr107_hw_stat aqr107_hw_stats[] = {
	SGMII_STAT("sgmii_rx_good_frames",	    RX_GOOD_FRAMES,	26),
	SGMII_STAT("sgmii_rx_bad_frames",	    RX_BAD_FRAMES,	26),
	SGMII_STAT("sgmii_rx_false_carrier_events", RX_FALSE_CARRIER,	 8),
	SGMII_STAT("sgmii_tx_good_frames",	    TX_GOOD_FRAMES,	26),
	SGMII_STAT("sgmii_tx_bad_frames",	    TX_BAD_FRAMES,	26),
	SGMII_STAT("sgmii_tx_false_carrier_events", TX_FALSE_CARRIER,	 8),
	SGMII_STAT("sgmii_tx_collisions",	    TX_COLLISIONS,	 8),
	SGMII_STAT("sgmii_tx_line_collisions",	    TX_LINE_COLLISIONS,	 8),
	SGMII_STAT("sgmii_tx_frame_alignment_err",  TX_FRAME_ALIGN_ERR,	16),
	SGMII_STAT("sgmii_tx_runt_frames",	    TX_RUNT_FRAMES,	22),
};
#define AQR107_SGMII_STAT_SZ ARRAY_SIZE(aqr107_hw_stats)

#ifdef CONFIG_AQUANTIA_PHY_MIB
struct aqr107_mib_stat {
	u64 crc8_error_packets;
	u64 ldpc_error_packets;
	u64 ls_tx_good_packets;
	u64 ls_tx_bad_packets;
	u64 ls_rx_good_packets;
	u64 ls_rx_bad_packets;
	u64 ss_tx_good_packets;
	u64 ss_tx_bad_packets;
	u64 ss_rx_good_packets;
	u64 ss_rx_bad_packets;
};
#endif

struct aqr107_priv {
	u64 sgmii_stats[AQR107_SGMII_STAT_SZ];
#ifdef CONFIG_AQUANTIA_PHY_FW_DOWNLOAD
	struct phy_device *phydevs[1];
	struct task_struct *heartbeat_thread;
	spinlock_t lock;
	bool fw_initialized;
	int fw_dl_mode;
	u16 heartbeat;
#endif
#ifdef CONFIG_AQUANTIA_PHY_MIB
	struct aqr107_mib_stat mib;
	struct task_struct *mib_thread;
#endif
};

int aqr107_set_downshift(struct phy_device *phydev, u8 cnt);
void aqr107_chip_info(struct phy_device *phydev);
int aqr107_config_mdi(struct phy_device *phydev);
int aqr107_config_usx_aneg_en(struct phy_device *phydev);
int aqr107_config_led(struct phy_device *phydev);

#if IS_REACHABLE(CONFIG_HWMON)
int aqr_hwmon_probe(struct phy_device *phydev);
#else
static inline int aqr_hwmon_probe(struct phy_device *phydev) { return 0; }
#endif

#ifdef CONFIG_AQUANTIA_PHY_FW_DOWNLOAD
enum aqr_fw_dl_mode {
	FW_DL_SINGLE = 0,
	FW_DL_GNAGLOAD,
};

int aqr_firmware_heartbeat_thread(void *data);
int aqr_firmware_download(struct phy_device *phydev);
#endif
#ifdef CONFIG_AQUANTIA_PHY_MIB
int aqr107_config_mib(struct phy_device *phydev);
#endif