/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#include <gswss_mac_api.h>

#if defined(PC_UTILITY) || defined(CHIPTEST)
struct adap_prv_data adap_priv_data = {
	.flags = 0,
	.ss_addr_base = GSWIP_SS_TOP_BASE,
};
#endif

int gswss_wr_reg(void *pdev, u32 reg_off, u32 reg_val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	GSWSS_MAC_RGWR(pdata, reg_off, reg_val);

	return 0;
}

int gswss_rd_reg(void *pdev, u32 reg_off)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	GSW_MAC_Cli_t *param = pdata->mac_cli;

	param->val[0] = GSWSS_MAC_RGRD(pdata, reg_off);

	return param->val[0];
}

int gswss_xgmac_reset(void *pdev, u32 reset)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_if_cfg;

	mac_if_cfg = GSWSS_MAC_RGRD(pdata, MAC_IF_CFG(pdata->mac_idx));

	MAC_SET_VAL(mac_if_cfg, MAC_IF_CFG, XGMAC_RES, reset);

	mac_dbg("GSWSS: Resetting XGMAC %d Module\n", pdata->mac_idx);

	GSWSS_MAC_RGWR(pdata, MAC_IF_CFG(pdata->mac_idx), mac_if_cfg);

	return 0;
}

int gswss_lmac_reset(void *pdev, u32 reset)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_if_cfg;

	mac_if_cfg = GSWSS_MAC_RGRD(pdata, MAC_IF_CFG(pdata->mac_idx));

	MAC_SET_VAL(mac_if_cfg, MAC_IF_CFG, LMAC_RES, reset);

	mac_dbg("GSWSS: Resetting LMAC %d Module\n", pdata->mac_idx);

	GSWSS_MAC_RGWR(pdata, MAC_IF_CFG(pdata->mac_idx), mac_if_cfg);

	return 0;
}

int gswss_adap_reset(void *pdev, u32 reset)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_if_cfg;

	mac_if_cfg = GSWSS_MAC_RGRD(pdata, MAC_IF_CFG(pdata->mac_idx));

	MAC_SET_VAL(mac_if_cfg, MAC_IF_CFG, ADAP_RES, reset);

	mac_dbg("GSWSS: Resetting ADAP %d Module\n", pdata->mac_idx);

	GSWSS_MAC_RGWR(pdata, MAC_IF_CFG(pdata->mac_idx), mac_if_cfg);

	return 0;
}

int gswss_mac_enable(void *pdev, u32 enable)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_if_cfg;

	mac_if_cfg = GSWSS_MAC_RGRD(pdata, MAC_IF_CFG(pdata->mac_idx));

	MAC_SET_VAL(mac_if_cfg, MAC_IF_CFG, MAC_EN, enable);

	mac_dbg("GSWSS: MAC %d: %s\n", pdata->mac_idx,
		enable ? "ENABLED" : "DISABLED");

	GSWSS_MAC_RGWR(pdata, MAC_IF_CFG(pdata->mac_idx), mac_if_cfg);

	return 0;
}

int gswss_set_1g_intf(void *pdev, u32 macif)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_if_cfg;
	int ret = 0;

	mac_if_cfg = GSWSS_MAC_RGRD(pdata, MAC_IF_CFG(pdata->mac_idx));

	if (macif == LMAC_GMII) {
		mac_dbg("GSWSS: MACIF Set to CFG1G with LMAC_GMII\n");
		MAC_SET_VAL(mac_if_cfg, MAC_IF_CFG, CFG1G, 0);
	} else if (macif == XGMAC_GMII) {
		mac_dbg("GSWSS: MACIF Set to CFG1G with XGMAC_GMII\n");
		MAC_SET_VAL(mac_if_cfg, MAC_IF_CFG, CFG1G, 1);
	} else {
		mac_printf("GSWSS: MACIF Set to 1G Wrong Value\n");
	}

	GSWSS_MAC_RGWR(pdata, MAC_IF_CFG(pdata->mac_idx), mac_if_cfg);

	return ret;
}

u32 gswss_get_1g_intf(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_if_cfg, macif;

	mac_if_cfg = GSWSS_MAC_RGRD(pdata, MAC_IF_CFG(pdata->mac_idx));

	macif = MAC_GET_VAL(mac_if_cfg, MAC_IF_CFG, CFG1G);

	if (macif == 0) {
		mac_dbg("GSWSS: MACIF got for CFG1G is LMAC_GMII\n");
		macif = LMAC_GMII;
	} else if (macif == 1) {
		mac_dbg("GSWSS: MACIF got for CFG1G is XGMAC_GMII\n");
		macif = XGMAC_GMII;
	} else {
		mac_printf("GSWSS: MACIF got for CFG1G is Wrong Value\n");
	}

	return macif;
}

int gswss_set_fe_intf(void *pdev, u32 macif)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_if_cfg;
	int ret = 0;

	mac_if_cfg = GSWSS_MAC_RGRD(pdata, MAC_IF_CFG(pdata->mac_idx));

	if (macif == LMAC_MII) {
		mac_dbg("GSWSS: MACIF Set to CFGFE with LMAC_MII\n");
		MAC_SET_VAL(mac_if_cfg, MAC_IF_CFG, CFGFE, 0);
	} else if (macif == XGMAC_GMII) {
		mac_dbg("GSWSS: MACIF Set to CFGFE with XGMAC_GMII\n");
		MAC_SET_VAL(mac_if_cfg, MAC_IF_CFG, CFGFE, 1);
	} else {
		mac_printf("GSWSS: MACIF Set to CFGFE Wrong Value\n");
	}

	GSWSS_MAC_RGWR(pdata, MAC_IF_CFG(pdata->mac_idx), mac_if_cfg);

	return ret;
}

u32 gswss_get_fe_intf(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_if_cfg, macif;

	mac_if_cfg = GSWSS_MAC_RGRD(pdata, MAC_IF_CFG(pdata->mac_idx));

	macif = MAC_GET_VAL(mac_if_cfg, MAC_IF_CFG, CFGFE);

	if (macif == 0) {
		mac_printf("GSWSS: MACIF got for CFGFE is LMAC_MII\n");
		macif = LMAC_MII;
	} else if (macif == 1) {
		mac_printf("GSWSS: MACIF got for CFGFE is XGMAC_GMII\n");
		macif = XGMAC_GMII;
	} else {
		mac_printf("GSWSS: MACIF got for CFGFE is Wrong Value\n");
	}

	return macif;
}

int gswss_set_2G5_intf(void *pdev, u32 macif)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_if_cfg;
	int ret = 0;

	mac_if_cfg = GSWSS_MAC_RGRD(pdata, MAC_IF_CFG(pdata->mac_idx));

	if (macif == XGMAC_GMII) {
		mac_dbg("GSWSS: MACIF Set to 2.5G with XGMAC_GMII\n");
		MAC_SET_VAL(mac_if_cfg, MAC_IF_CFG, CFG2G5, 0);
	} else if (macif == XGMAC_XGMII) {
		mac_dbg("GSWSS: MACIF Set to 2.5G with XGMAC_XGMII\n");
		MAC_SET_VAL(mac_if_cfg, MAC_IF_CFG, CFG2G5, 1);
	} else {
		mac_printf("GSWSS: MACIF Set to 2.5G Wrong Value\n");
	}

	GSWSS_MAC_RGWR(pdata, MAC_IF_CFG(pdata->mac_idx), mac_if_cfg);

	return ret;
}

u32 gswss_get_2G5_intf(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_if_cfg, macif;

	mac_if_cfg = GSWSS_MAC_RGRD(pdata, MAC_IF_CFG(pdata->mac_idx));

	macif = MAC_GET_VAL(mac_if_cfg, MAC_IF_CFG, CFG2G5);

	if (macif == 0) {
		mac_printf("GSWSS: MACIF Got for 2.5G with XGMAC_GMII\n");
		macif = XGMAC_GMII;
	} else if (macif == 1) {
		mac_printf("GSWSS: MACIF Got for 2.5G with XGMAC_XGMII\n");
		macif = XGMAC_XGMII;
	} else {
		mac_printf("GSWSS: MACIF Got for 2.5G Wrong Value\n");
	}

	return macif;
}

int gswss_set_mac_txfcs_ins_op(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));

	if (MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, TXFCS_INS) != val) {
		mac_dbg("GSWSS: TX FCS INS operation changing to MODE%d\n",
			val);
		MAC_SET_VAL(mac_op_cfg, MAC_OP_CFG, TXFCS_INS, val);
		GSWSS_MAC_RGWR(pdata, MAC_OP_CFG(pdata->mac_idx), mac_op_cfg);
	}

	return 0;
}

int gswss_set_mac_txfcs_rm_op(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));

	if (MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, TXFCS_RM) != val) {
		mac_dbg("GSWSS: TX FCS RM operation changing to MODE%d\n",
			val);
		MAC_SET_VAL(mac_op_cfg, MAC_OP_CFG, TXFCS_RM, val);
		GSWSS_MAC_RGWR(pdata, MAC_OP_CFG(pdata->mac_idx), mac_op_cfg);
	}

	return 0;
}

int gswss_set_mac_rxfcs_op(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));

	if (MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, RXFCS) != val) {
		mac_dbg("GSWSS: RX FCS operation changing to MODE%d\n",
			val);
		MAC_SET_VAL(mac_op_cfg, MAC_OP_CFG, RXFCS, val);
		GSWSS_MAC_RGWR(pdata, MAC_OP_CFG(pdata->mac_idx), mac_op_cfg);
	}

	return 0;
}

int gswss_set_mac_rxsptag_op(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));

	if (MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, RXSPTAG) != val) {
		mac_dbg("GSWSS: RX SPTAG operation changing to MODE%d\n",
			val);
		MAC_SET_VAL(mac_op_cfg, MAC_OP_CFG, RXSPTAG, val);
		GSWSS_MAC_RGWR(pdata, MAC_OP_CFG(pdata->mac_idx), mac_op_cfg);
	}

	return 0;
}

int gswss_set_mac_txsptag_op(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));

	if (MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, TXSPTAG) != val) {
		mac_dbg("GSWSS: TX SPTAG operation changing to MODE%d\n",
			val);
		MAC_SET_VAL(mac_op_cfg, MAC_OP_CFG, TXSPTAG, val);
		GSWSS_MAC_RGWR(pdata, MAC_OP_CFG(pdata->mac_idx), mac_op_cfg);
	}

	return 0;
}

int gswss_set_mac_rxtime_op(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));

	if (MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, RXTIME) != val) {
		mac_dbg("GSWSS: RX TIME operation changing to MODE%d\n",
			val);
		MAC_SET_VAL(mac_op_cfg, MAC_OP_CFG, RXTIME, val);
		GSWSS_MAC_RGWR(pdata, MAC_OP_CFG(pdata->mac_idx), mac_op_cfg);
	}

	return 0;
}

int gswss_get_mac_txfcs_ins_op(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;
	int ret = 0;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));
	ret = MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, TXFCS_INS);

	return ret;
}

int gswss_get_mac_txfcs_rm_op(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;
	int ret = 0;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));
	ret = MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, TXFCS_RM);

	return ret;
}

int gswss_get_mac_rxfcs_op(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;
	int ret = 0;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));
	ret = MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, RXFCS);

	return ret;
}

int gswss_get_mac_rxsptag_op(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;
	int ret = 0;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));
	ret = MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, RXSPTAG);

	return ret;
}

int gswss_get_mac_txsptag_op(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;
	int ret = 0;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));
	ret = MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, TXSPTAG);

	return ret;
}

int gswss_get_mac_rxtime_op(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_op_cfg;
	int ret = 0;

	mac_op_cfg = GSWSS_MAC_RGRD(pdata, MAC_OP_CFG(pdata->mac_idx));
	ret = MAC_GET_VAL(mac_op_cfg, MAC_OP_CFG, RXTIME);

	return ret;
}

int gswss_set_mtu(void *pdev, u32 mtu)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int ret = 0;

	if (GSWSS_MAC_RGRD(pdata, MAC_MTU_CFG(pdata->mac_idx)) != mtu) {
		mac_dbg("GSWSS: MTU set to %d\n", mtu);
		GSWSS_MAC_RGWR(pdata, MAC_MTU_CFG(pdata->mac_idx), mtu);
	}

	return ret;
}

int gswss_get_mtu(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mtu = GSWSS_MAC_RGRD(pdata, MAC_MTU_CFG(pdata->mac_idx));

	mac_dbg("GSWSS: MAC%d MTU %d\n", pdata->mac_idx, mtu);

	return mtu;
}

int gswss_set_txtstamp_fifo(void *pdev,
			    struct mac_fifo_entry *f_entry)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int ret = 0;
	u16 val = 0;
	u32 mac_txtstamp;

	val = ((f_entry->ttse << 4) | (f_entry->ostc << 3) |
	       (f_entry->ostpa << 2) | f_entry->cic);
	GSWSS_MAC_RGWR(pdata, MAC_TXTS_CIC(pdata->mac_idx), val);

	mac_txtstamp = MAC_TXTS_0(pdata->mac_idx);
	val = GET_N_BITS(f_entry->ttsh, 0, 16);
	GSWSS_MAC_RGWR(pdata, mac_txtstamp, val);

	mac_txtstamp = MAC_TXTS_1(pdata->mac_idx);
	val = GET_N_BITS(f_entry->ttsh, 16, 16);
	GSWSS_MAC_RGWR(pdata, mac_txtstamp, val);

	mac_txtstamp = MAC_TXTS_2(pdata->mac_idx);
	val = GET_N_BITS(f_entry->ttsl, 0, 16);
	GSWSS_MAC_RGWR(pdata, mac_txtstamp, val);

	mac_txtstamp = MAC_TXTS_3(pdata->mac_idx);
	val = GET_N_BITS(f_entry->ttsl, 16, 16);
	GSWSS_MAC_RGWR(pdata, mac_txtstamp, val);

	mac_dbg("MAC%d: TxTstamp Fifo Record ID %d written\n",
		pdata->mac_idx, f_entry->rec_id);

	/* Write the entries into the 64 array record_id */
	gswss_set_txtstamp_access(pdev, 1, f_entry->rec_id);

	return ret;
}

int gswss_get_txtstamp_fifo(void *pdev, u32 record_id, struct mac_fifo_entry *f_entry)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_txtstamp;

	gswss_set_txtstamp_access(pdev, 0, record_id);

	mac_dbg("\nMAC%d: TxTstamp Fifo Record ID %d:\n",
		pdata->mac_idx,
		record_id);

	mac_txtstamp = GSWSS_MAC_RGRD(pdata, MAC_TXTS_CIC(pdata->mac_idx));

	f_entry->ttse = GET_N_BITS(mac_txtstamp, 4, 1);
	f_entry->ostc = GET_N_BITS(mac_txtstamp, 3, 1);
	f_entry->ostpa = GET_N_BITS(mac_txtstamp, 2, 1);
	f_entry->cic = GET_N_BITS(mac_txtstamp, 0, 2);

	f_entry->ttsl =
		((GSWSS_MAC_RGRD(pdata, MAC_TXTS_1(pdata->mac_idx)) << 16) |
		 GSWSS_MAC_RGRD(pdata, MAC_TXTS_0(pdata->mac_idx)));
	f_entry->ttsh =
		((GSWSS_MAC_RGRD(pdata, MAC_TXTS_3(pdata->mac_idx)) << 16) |
		 GSWSS_MAC_RGRD(pdata, MAC_TXTS_2(pdata->mac_idx)));

	f_entry->rec_id = record_id;

	return 0;
}

int gswss_set_txtstamp_access(void *pdev, u32 op_mode, u32 addr)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int ret = 0;
	u16 tstamp_acc = 0;
	u32 busy_bit;

	MAC_SET_VAL(tstamp_acc, MAC_TXTS_ACC, BAS, 1);
	MAC_SET_VAL(tstamp_acc, MAC_TXTS_ACC, OPMOD, op_mode);
	MAC_SET_VAL(tstamp_acc, MAC_TXTS_ACC, ADDR, addr);

	GSWSS_MAC_RGWR(pdata, MAC_TXTS_ACC(pdata->mac_idx), tstamp_acc);

	while (1) {
		tstamp_acc = GSWSS_MAC_RGRD(pdata,
					    MAC_TXTS_ACC(pdata->mac_idx));
		busy_bit = MAC_GET_VAL(tstamp_acc, MAC_TXTS_ACC, BAS);

		if (busy_bit == 0)
			break;
	}

	return ret;
}

int gswss_set_duplex_mode(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u16 phy_mode = 0;

	phy_mode = GSWSS_MAC_RGRD(pdata, PHY_MODE(pdata->mac_idx));

	if (MAC_GET_VAL(phy_mode, PHY_MODE, FDUP) != val) {
		if (val == MAC_AUTO_DPLX)
			mac_dbg("\tGSWSS: Duplex mode set: Auto Mode\n");
		else if (val == MAC_FULL_DPLX)
			mac_dbg("\tGSWSS: Duplex mode set: Full Duplex\n");
		else if (val == MAC_RES_DPLX)
			mac_dbg("\tGSWSS: Duplex mode set: Reserved\n");
		else if (val == MAC_HALF_DPLX)
			mac_dbg("\tGSWSS: Duplex mode set: Half Duplex\n");

		MAC_SET_VAL(phy_mode, PHY_MODE, FDUP, val);
		GSWSS_MAC_RGWR(pdata, PHY_MODE(pdata->mac_idx), phy_mode);
	}

	return 0;
}

int gswss_get_duplex_mode(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u16 phy_mode = 0;
	u32 val;

	phy_mode = GSWSS_MAC_RGRD(pdata, PHY_MODE(pdata->mac_idx));
	val = MAC_GET_VAL(phy_mode, PHY_MODE, FDUP);

	if (val == MAC_AUTO_DPLX)
		mac_dbg("\tGSWSS: Duplex mode got: Auto Mode\n");
	else if (val == MAC_FULL_DPLX)
		mac_dbg("\tGSWSS: Duplex mode got: Full Duplex\n");
	else if (val == MAC_RES_DPLX)
		mac_dbg("\tGSWSS: Duplex mode got: Reserved\n");
	else if (val == MAC_HALF_DPLX)
		mac_dbg("\tGSWSS: Duplex mode got: Half Duplex\n");

	return val;
}

int gswss_set_speed(void *pdev, u8 speed)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u16 phy_mode = 0;
	u8 speed_msb = 0, speed_lsb = 0;

	phy_mode = GSWSS_MAC_RGRD(pdata, PHY_MODE(pdata->mac_idx));

	speed_msb = GET_N_BITS(speed, 2, 1);
	speed_lsb = GET_N_BITS(speed, 0, 2);

	if (speed == SPEED_10M)
		mac_dbg("\tGSWSS: SPEED    10 Mbps\n");
	else if (speed == SPEED_100M)
		mac_dbg("\tGSWSS: SPEED    100 Mbps\n");
	else if (speed == SPEED_1G)
		mac_dbg("\tGSWSS: SPEED    1 Gbps\n");
	else if (speed == SPEED_10G)
		mac_dbg("\tGSWSS: SPEED    10 Gbps\n");
	else if (speed == SPEED_2G5)
		mac_dbg("\tGSWSS: SPEED    2.5 Gbps\n");
	else if (speed == SPEED_5G)
		mac_dbg("\tGSWSS: SPEED    5 Gbps\n");
	else if (speed == SPEED_FLEX)
		mac_dbg("\tGSWSS: SPEED    RESERVED\n");
	else if (speed == SPEED_AUTO)
		mac_dbg("\tGSWSS: SPEED    Auto Mode\n");

	MAC_SET_VAL(phy_mode, PHY_MODE, SPEEDMSB, speed_msb);
	MAC_SET_VAL(phy_mode, PHY_MODE, SPEEDLSB, speed_lsb);

	GSWSS_MAC_RGWR(pdata, PHY_MODE(pdata->mac_idx), phy_mode);

	return 0;
}

u8 gswss_get_speed(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u16 phy_mode = 0;
	u8 speed_msb, speed_lsb, speed;

	phy_mode = GSWSS_MAC_RGRD(pdata, PHY_MODE(pdata->mac_idx));

	speed_msb = MAC_GET_VAL(phy_mode, PHY_MODE, SPEEDMSB);
	speed_lsb = MAC_GET_VAL(phy_mode, PHY_MODE, SPEEDLSB);

	speed = ((speed_msb << 2) | speed_lsb);

	if (speed == SPEED_10M)
		mac_dbg("\tGSWSS: SPEED Got   10 Mbps\n");
	else if (speed == SPEED_100M)
		mac_dbg("\tGSWSS: SPEED Got   100 Mbps\n");
	else if (speed == SPEED_1G)
		mac_dbg("\tGSWSS: SPEED Got   1 Gbps\n");
	else if (speed == SPEED_10G)
		mac_dbg("\tGSWSS: SPEED Got   10 Gbps\n");
	else if (speed == SPEED_2G5)
		mac_dbg("\tGSWSS: SPEED Got   2.5 Gbps\n");
	else if (speed == SPEED_5G)
		mac_dbg("\tGSWSS: SPEED Got   5 Gbps\n");
	else if (speed == SPEED_FLEX)
		mac_dbg("\tGSWSS: SPEED Got   RESERVED\n");
	else if (speed == SPEED_AUTO)
		mac_dbg("\tGSWSS: SPEED Got   Auto Mode\n");

	return speed;
}

int gswss_set_linkstatus(void *pdev, u8 linkst)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u16 phy_mode = 0;

	phy_mode = GSWSS_MAC_RGRD(pdata, PHY_MODE(pdata->mac_idx));

	if (MAC_GET_VAL(phy_mode, PHY_MODE, LINKST) != linkst) {
		if (linkst == 0)
			mac_dbg("\tGSWSS: LINK STS: Auto Mode\n");
		else if (linkst == 1)
			mac_dbg("\tGSWSS: LINK STS: Forced UP\n");
		else if (linkst == 2)
			mac_dbg("\tGSWSS: LINK STS: Forced DOWN\n");
		else if (linkst == 3)
			mac_dbg("\tGSWSS: LINK STS: Reserved\n");

		MAC_SET_VAL(phy_mode, PHY_MODE, LINKST, linkst);
		GSWSS_MAC_RGWR(pdata, PHY_MODE(pdata->mac_idx), phy_mode);
	}

	return 0;
}

int gswss_get_linkstatus(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u16 phy_mode = 0;
	int linkst;

	phy_mode = GSWSS_MAC_RGRD(pdata, PHY_MODE(pdata->mac_idx));

	linkst = MAC_GET_VAL(phy_mode, PHY_MODE, LINKST);

	if (linkst == 0)
		mac_dbg("\tGSWSS: LINK STS Got: Auto Mode\n");
	else if (linkst == 1)
		mac_dbg("\tGSWSS: LINK STS Got: Forced UP\n");
	else if (linkst == 2)
		mac_dbg("\tGSWSS: LINK STS Got: Forced DOWN\n");
	else if (linkst == 3)
		mac_dbg("\tGSWSS: LINK STS Got: Reserved\n");

	return linkst;
}

int gswss_set_flowctrl_tx(void *pdev, u8 flow_ctrl_tx)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int ret = 0;
	u16 phy_mode = 0;

	phy_mode = GSWSS_MAC_RGRD(pdata, PHY_MODE(pdata->mac_idx));

	if (MAC_GET_VAL(phy_mode, PHY_MODE, FCONTX) != flow_ctrl_tx) {
		if (flow_ctrl_tx == 0)
			mac_dbg("\tGSWSS: Flow Ctrl Mode TX: Auto Mode\n");
		else if (flow_ctrl_tx == 1)
			mac_dbg("\tGSWSS: Flow Ctrl Mode TX: ENABLED\n");
		else if (flow_ctrl_tx == 2)
			mac_dbg("\tGSWSS: Flow Ctrl Mode TX: Reserved\n");
		else if (flow_ctrl_tx == 3)
			mac_dbg("\tGSWSS: Flow Ctrl Mode TX: DISABLED\n");

		MAC_SET_VAL(phy_mode, PHY_MODE, FCONTX, flow_ctrl_tx);
		GSWSS_MAC_RGWR(pdata, PHY_MODE(pdata->mac_idx), phy_mode);
	}

	return ret;
}

u32 gswss_get_flowctrl_tx(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u16 phy_mode = 0;
	u32 flow_ctrl_tx;

	phy_mode = GSWSS_MAC_RGRD(pdata, PHY_MODE(pdata->mac_idx));

	flow_ctrl_tx = MAC_GET_VAL(phy_mode, PHY_MODE, FCONTX);

	if (flow_ctrl_tx == 0)
		mac_printf("\tGSWSS: Flow Ctrl Mode TX: Auto Mode\n");
	else if (flow_ctrl_tx == 1)
		mac_printf("\tGSWSS: Flow Ctrl Mode TX: ENABLED\n");
	else if (flow_ctrl_tx == 2)
		mac_printf("\tGSWSS: Flow Ctrl Mode TX: Reserved\n");
	else if (flow_ctrl_tx == 3)
		mac_printf("\tGSWSS: Flow Ctrl Mode TX: DISABLED\n");

	return flow_ctrl_tx;
}

int gswss_set_flowctrl_rx(void *pdev, u8 flow_ctrl_rx)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int ret = 0;
	u16 phy_mode = 0;

	phy_mode = GSWSS_MAC_RGRD(pdata, PHY_MODE(pdata->mac_idx));

	if (MAC_GET_VAL(phy_mode, PHY_MODE, FCONRX) != flow_ctrl_rx) {
		if (flow_ctrl_rx == 0)
			mac_dbg("\tGSWSS: Flow Ctrl Mode RX: Auto Mode\n");
		else if (flow_ctrl_rx == 1)
			mac_dbg("\tGSWSS: Flow Ctrl Mode RX: ENABLED\n");
		else if (flow_ctrl_rx == 2)
			mac_dbg("\tGSWSS: Flow Ctrl Mode RX: Reserved\n");
		else if (flow_ctrl_rx == 3)
			mac_dbg("\tGSWSS: Flow Ctrl Mode RX: DISABLED\n");

		MAC_SET_VAL(phy_mode, PHY_MODE, FCONRX, flow_ctrl_rx);
		GSWSS_MAC_RGWR(pdata, PHY_MODE(pdata->mac_idx), phy_mode);
	}

	return ret;
}

u32 gswss_get_flowctrl_rx(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u16 phy_mode = 0;
	u32 flow_ctrl_rx;

	phy_mode = GSWSS_MAC_RGRD(pdata, PHY_MODE(pdata->mac_idx));

	flow_ctrl_rx = MAC_GET_VAL(phy_mode, PHY_MODE, FCONRX);

	if (flow_ctrl_rx == 0)
		mac_printf("\tGSWSS: Flow Ctrl Mode RX: Auto Mode\n");
	else if (flow_ctrl_rx == 1)
		mac_printf("\tGSWSS: Flow Ctrl Mode RX:  ENABLED\n");
	else if (flow_ctrl_rx == 2)
		mac_printf("\tGSWSS: Flow Ctrl Mode RX:  Reserved\n");
	else if (flow_ctrl_rx == 3)
		mac_printf("\tGSWSS: Flow Ctrl Mode RX:  DISABLED\n");

	return flow_ctrl_rx;
}

int gswss_set_xgmac_tx_disable(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 xgmac_ctrl = 0;

	xgmac_ctrl = GSWSS_MAC_RGRD(pdata, XGMAC_CTRL(pdata->mac_idx));

	if (MAC_GET_VAL(xgmac_ctrl, XGMAC_CTRL, DISTX) != val) {
		mac_dbg("\tGSWSS: XGMAC %d TX %s\n", pdata->mac_idx,
			val ? "DISABLED" : "NOT DISABLED");
		MAC_SET_VAL(xgmac_ctrl, XGMAC_CTRL, DISTX, val);
		GSWSS_MAC_RGWR(pdata, XGMAC_CTRL(pdata->mac_idx), xgmac_ctrl);
	}

	return 0;
}

int gswss_set_xgmac_rx_disable(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 xgmac_ctrl = 0;

	xgmac_ctrl = GSWSS_MAC_RGRD(pdata, XGMAC_CTRL(pdata->mac_idx));

	if (MAC_GET_VAL(xgmac_ctrl, XGMAC_CTRL, DISRX) != val) {
		mac_dbg("\tGSWSS: XGMAC %d RX %s\n", pdata->mac_idx,
			val ? "DISABLED" : "NOT DISABLED");
		MAC_SET_VAL(xgmac_ctrl, XGMAC_CTRL, DISRX, val);
		GSWSS_MAC_RGWR(pdata, XGMAC_CTRL(pdata->mac_idx), xgmac_ctrl);
	}

	return 0;
}

int gswss_set_xgmac_crc_ctrl(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 xgmac_ctrl = 0;

	xgmac_ctrl = GSWSS_MAC_RGRD(pdata, XGMAC_CTRL(pdata->mac_idx));

	if (MAC_GET_VAL(xgmac_ctrl, XGMAC_CTRL, CPC) != val) {
		if (val == 0)
			mac_printf("GSWSS: CRC and PAD insertion are "
				   "enabled.\n");
		else if (val == 1)
			mac_printf("GSWSS: CRC insert enable,PAD insert "
				   "disable\n");
		else if (val == 2)
			mac_printf("GSWSS: CRC,PAD not inserted not "
				   "replaced.\n");

		MAC_SET_VAL(xgmac_ctrl, XGMAC_CTRL, CPC, val);
		GSWSS_MAC_RGWR(pdata, XGMAC_CTRL(pdata->mac_idx), xgmac_ctrl);
	}

	return 0;
}

int gswss_set_eee_cap(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 aneg_eee = 0;

	aneg_eee = GSWSS_MAC_RGRD(pdata, ANEG_EEE(pdata->mac_idx));
	MAC_SET_VAL(aneg_eee, ANEG_EEE, EEE_CAPABLE, val);
	GSWSS_MAC_RGWR(pdata, ANEG_EEE(pdata->mac_idx), aneg_eee);

	return 0;
}

int gswss_get_xgmac_crc_ctrl(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 xgmac_ctrl = 0, val = 0;

	xgmac_ctrl = GSWSS_MAC_RGRD(pdata, XGMAC_CTRL(pdata->mac_idx));

	val = MAC_GET_VAL(xgmac_ctrl, XGMAC_CTRL, CPC);

	if (val == 0)
		mac_printf("GSWSS: CRC and PAD insertion are enabled.\n");
	else if (val == 1)
		mac_printf("GSWSS: CRC insert enable,PAD insert disable\n");
	else if (val == 2)
		mac_printf("GSWSS: CRC,PAD not inserted not replaced.\n");

	return val;
}

void gswss_get_xgmac_ctrl(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 xgmac_ctrl = 0, distx, disrx, crc_ctrl;

	xgmac_ctrl = GSWSS_MAC_RGRD(pdata, XGMAC_CTRL(pdata->mac_idx));
	mac_printf("GSWSS: XGMAC CTRL %d %x\n", pdata->mac_idx, xgmac_ctrl);
	distx = MAC_GET_VAL(xgmac_ctrl, XGMAC_CTRL, DISTX);
	disrx = MAC_GET_VAL(xgmac_ctrl, XGMAC_CTRL, DISRX);
	crc_ctrl = MAC_GET_VAL(xgmac_ctrl, XGMAC_CTRL, CPC);

	mac_printf("\tGSWSS: XGMAC %d TX %s\n", pdata->mac_idx,
		   distx ? "DISABLED" : "NOT DISABLED");
	mac_printf("\tGSWSS: XGMAC %d RX %s\n", pdata->mac_idx,
		   disrx ? "DISABLED" : "NOT DISABLED");

	if (crc_ctrl == 0)
		mac_printf("\tGSWSS: CRC and PAD insertion are enabled.\n");
	else if (crc_ctrl == 1)
		mac_printf("\tGSWSS: CRC insert enable,PAD insert disable\n");
	else if (crc_ctrl == 2)
		mac_printf("\tGSWSS: CRC,PAD not inserted not replaced.\n");
}

void gswss_test_all_reg(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	gswss_check_reg(pdev, GSWIPSS_IER0, "GSWIPSS_IER0", 0, 0x8C8C, 0);
	gswss_check_reg(pdev, GSWIPSS_ISR0, "GSWIPSS_ISR0", 0, 0x8C8C, 0);
	gswss_check_reg(pdev, GSWIPSS_IER1, "GSWIPSS_IER1", 0, 0x8C, 0);
	gswss_check_reg(pdev, GSWIPSS_ISR1, "GSWIPSS_ISR1", 0, 0x8C, 0);
	gswss_check_reg(pdev, CFG0_1588, "CFG0_1588", 0, 0x7777, 0);
	gswss_check_reg(pdev, CFG1_1588, "CFG1_1588", 0, 0xFF80, 0);

	gswss_check_reg(pdev, MAC_IF_CFG(pdata->mac_idx),
			"MAC_IF_CFG", pdata->mac_idx, 0x7, 0);
	gswss_check_reg(pdev, MAC_OP_CFG(pdata->mac_idx),
			"MAC_OP_CFG", pdata->mac_idx, 0x1FF, 0);
	gswss_check_reg(pdev, MAC_MTU_CFG(pdata->mac_idx),
			"MAC_MTU_CFG", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_GINT_CFG(pdata->mac_idx),
			"MAC_GINT_CFG", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_GINT_HD0_CFG(pdata->mac_idx),
			"MAC_GINT_HD0_CFG", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_GINT_HD1_CFG(pdata->mac_idx),
			"MAC_GINT_HD1_CFG", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_GINT_HD2_CFG(pdata->mac_idx),
			"MAC_GINT_HD2_CFG", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_GINT_HD3_CFG(pdata->mac_idx),
			"MAC_GINT_HD3_CFG", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_GINT_HD4_CFG(pdata->mac_idx),
			"MAC_GINT_HD4_CFG", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_GINT_HD5_CFG(pdata->mac_idx),
			"MAC_GINT_HD5_CFG", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_GINT_HD6_CFG(pdata->mac_idx),
			"MAC_GINT_HD6_CFG", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_TXTS_0(pdata->mac_idx),
			"MAC_TXTS_0", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_TXTS_1(pdata->mac_idx),
			"MAC_TXTS_1", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_TXTS_2(pdata->mac_idx),
			"MAC_TXTS_2", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_TXTS_3(pdata->mac_idx),
			"MAC_TXTS_3", pdata->mac_idx, 0xFFFF, 0);
	gswss_check_reg(pdev, MAC_TXTS_CIC(pdata->mac_idx),
			"MAC_TXTS_CIC", pdata->mac_idx, 0x1F, 0);
	gswss_check_reg(pdev, MAC_TXTS_ACC(pdata->mac_idx),
			"MAC_TXTS_ACC", pdata->mac_idx, 0x403F, 0);
	gswss_check_reg(pdev, PHY_MODE(pdata->mac_idx),
			"PHY_MODE", pdata->mac_idx, 0xFFE0, 0);
	gswss_check_reg(pdev, ANEG_EEE(pdata->mac_idx),
			"ANEG_EEE", pdata->mac_idx, 0xF, 0);
	gswss_check_reg(pdev, XGMAC_CTRL(pdata->mac_idx),
			"XGMAC_CTRL", pdata->mac_idx, 0xF, 0);
}

void gswss_check_reg(void *pdev, u32 reg, char *name, int idx,
		     u16 set_val, u16 clr_val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 val;

	GSWSS_MAC_RGWR(pdata, reg, set_val);
	val = GSWSS_MAC_RGRD(pdata, reg);

	if (val != set_val)
		mac_printf("Setting reg %s: %d with %x FAILED\n",
			   name, idx, set_val);

	GSWSS_MAC_RGWR(pdata, reg, clr_val);

	if (val != clr_val)
		mac_printf("Setting reg %s: %d with %x FAILED\n",
			   name, idx, clr_val);
}

#if defined(PC_UTILITY) || defined(CHIPTEST)
struct adap_ops *gsw_get_adap_ops(u32 devid)
{
	return &adap_priv_data.ops;
}
#endif
