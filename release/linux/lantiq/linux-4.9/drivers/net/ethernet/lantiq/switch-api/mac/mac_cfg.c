/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#include <xgmac.h>
#include <gswss_mac_api.h>
#include <lmac_api.h>
#include <xgmac_mdio.h>
#include <xgmac_ptp.h>
#ifdef __KERNEL__
#include <net/switch_api/lantiq_gsw_api.h>
#include <net/switch_api/gsw_dev.h>
#endif

int mac_set_flowctrl(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	if (val > 4) {
		mac_printf("mac_config_flowctrl invalid val %d\n", val);
		return -1;
	}

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	lmac_set_flowcon_mode(pdev, val);

	switch (val) {
	case 0: /* AUTO */
		xgmac_enable_tx_flow_ctl(pdev, pdata->pause_time);
		xgmac_enable_rx_flow_ctl(pdev);
		gswss_set_flowctrl_rx(pdev, 0);
		gswss_set_flowctrl_tx(pdev, 0);
		break;

	case 3: /* RXTX */
		xgmac_enable_tx_flow_ctl(pdev, pdata->pause_time);
		xgmac_enable_rx_flow_ctl(pdev);
		gswss_set_flowctrl_rx(pdev, 1);
		gswss_set_flowctrl_tx(pdev, 1);
		break;

	case 1: /* RX */
		/* Disable TX in XGMAC and GSWSS */
		xgmac_disable_tx_flow_ctl(pdev);
		gswss_set_flowctrl_tx(pdev, 3);
		/* Enable RX in XGMAC and GSWSS */
		xgmac_enable_rx_flow_ctl(pdev);
		gswss_set_flowctrl_rx(pdev, 1);
		break;

	case 2: /* TX */
		/* Disable RX in XGMAC and GSWSS */
		xgmac_disable_rx_flow_ctl(pdev);
		gswss_set_flowctrl_rx(pdev, 3);
		/* Enable TX in XGMAC and GSWSS */
		xgmac_enable_tx_flow_ctl(pdev, pdata->pause_time);
		gswss_set_flowctrl_tx(pdev, 1);
		break;

	case 4: /* Disabled */
		xgmac_disable_tx_flow_ctl(pdev);
		xgmac_disable_rx_flow_ctl(pdev);
		gswss_set_flowctrl_rx(pdev, 3);
		gswss_set_flowctrl_tx(pdev, 3);
		break;

	default:
		break;
	}

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_get_flowctrl(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 flow_ctrl_tx = 0, flow_ctrl_rx = 0, flow_ctrl = 0;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	flow_ctrl_tx = gswss_get_flowctrl_tx(pdev);
	flow_ctrl_rx = gswss_get_flowctrl_rx(pdev);

	if (flow_ctrl_tx == 1 && flow_ctrl_rx == 1)
		flow_ctrl = 3; /* RXTX */
	else if (flow_ctrl_tx == 1 && flow_ctrl_rx == 0)
		flow_ctrl = 2; /* TX */
	else if (flow_ctrl_tx == 0 && flow_ctrl_rx == 1)
		flow_ctrl = 1; /* RX */
	else if (flow_ctrl_tx == 3 && flow_ctrl_rx == 3)
		flow_ctrl = 4; /* Disabled */
	else if (flow_ctrl_tx == 0 && flow_ctrl_rx == 0)
		flow_ctrl = 0; /* Auto */

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return flow_ctrl;
}

int mac_reset(void *pdev, u32 reset)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	gswss_xgmac_reset(pdev, reset);
	gswss_set_xgmac_rx_disable(pdev, reset);
	gswss_set_xgmac_tx_disable(pdev, reset);
	gswss_lmac_reset(pdev, reset);

	if (reset == RESET_OFF)
		gswss_mac_enable(pdev, MAC_DIS);
	else
		gswss_mac_enable(pdev, MAC_EN);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_config_loopback(void *pdev, u32 loopback)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	xgmac_set_loopback(pdev, loopback);
	lmac_set_loopback(pdev, loopback);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_config_ipg(void *pdev, u32 ipg)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	xgmac_set_ipg(pdev, ipg);
	lmac_set_ipg(pdev, ((96 + (32 * ipg)) / 8));

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_set_duplex(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	if (val == GSW_DUPLEX_FULL) /* Full Duplex */
		val = MAC_FULL_DPLX;
	else if (val == GSW_DUPLEX_HALF) /* Half Duplex */
		val = MAC_HALF_DPLX;
	else
		val = MAC_AUTO_DPLX;

	gswss_set_duplex_mode(pdev, val);
	lmac_set_duplex_mode(pdev, val);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_get_duplex(void *pdev)
{
	int val = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	val = gswss_get_duplex_mode(pdev);

	if (val == MAC_FULL_DPLX)
		val = GSW_DUPLEX_FULL;
	else if (val == MAC_HALF_DPLX)
		val = GSW_DUPLEX_HALF;
	else
		val = GSW_DUPLEX_AUTO;

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return val;
}

int mac_set_mii_if(void *pdev, u32 mii_mode)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int ret = 0;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	/* Default values for 2.5G is XGMAC_GMII, 1G is LMAC_GMII, */
	/* FE is LMAC_MII */
	if (mii_mode == GSW_PORT_HW_XGMII) {
		gswss_set_2G5_intf(pdev, XGMAC_XGMII);
	} else if (mii_mode == GSW_PORT_HW_MII) {
		gswss_set_fe_intf(pdev, LMAC_MII);
	} else if (mii_mode == GSW_PORT_HW_GMII) {
		gswss_set_1g_intf(pdev, XGMAC_GMII);
		gswss_set_2G5_intf(pdev, XGMAC_GMII);
	} else if (mii_mode == 5) {	/* For Testing LMAC 1G only */
		gswss_set_1g_intf(pdev, LMAC_GMII);
	} else {
		mac_printf("Wrong MII Interface\n");
		ret = -1;
		goto err;
	}

err:

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return -1;
}

int mac_get_mii_if(void *pdev)
{
	int mac_speed;
	u32 intf = GSW_PORT_HW_XGMII;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	mac_speed = gswss_get_speed(pdev);

	if ((mac_speed == SPEED_10M) || (mac_speed == SPEED_100M)) {
		intf = gswss_get_fe_intf(pdev);

		if (intf == XGMAC_GMII) {
			intf = GSW_PORT_HW_GMII;
			mac_printf("Speed %s got intf XGMAC GMII\n",
				   (mac_speed == SPEED_10M ? "10M" : "100M"));
		} else {
			intf = GSW_PORT_HW_MII;
			mac_printf("Speed %s got intf LMAC MII\n",
				   (mac_speed == SPEED_10M ? "10M" : "100M"));
		}
	} else if (mac_speed == SPEED_1G) {
		intf = gswss_get_1g_intf(pdev);

		if (intf == XGMAC_GMII)
			mac_printf("Speed 1G got intf XGMAC GMII\n");
		else
			mac_printf("Speed 1G got intf LMAC GMII\n");

		intf = GSW_PORT_HW_GMII;
	} else if (mac_speed == SPEED_2G5) {
		intf = gswss_get_2G5_intf(pdev);

		if (intf == XGMAC_GMII) {
			intf = GSW_PORT_HW_GMII;
			mac_printf("Speed 2.5G got intf XGMAC GMII\n");
		} else {
			intf = GSW_PORT_HW_XGMII;
			mac_printf("Speed 2.5G got intf XGMAC XGMII\n");
		}
	} else if (mac_speed == SPEED_10G) {
		intf = GSW_PORT_HW_XGMII;
		mac_printf("Speed 10G got intf XGMAC XGMII\n");
	} else if (mac_speed == SPEED_AUTO) {
		intf = GSW_PORT_HW_XGMII;
		mac_printf("Speed 10G got intf AUTO\n");
	}

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return intf;
}

int mac_set_speed(void *pdev, u32 phy_speed)
{
	u32 mii_intf;
	int ret = 0;

	if (phy_speed == GSW_PORT_SPEED_10) {	/* 10 Mbps */
		mii_intf = gswss_get_fe_intf(pdev);

		if (mii_intf == LMAC_MII)
			phy_speed = SPEED_LMAC_10M;
		else if (mii_intf == XGMAC_GMII)
			phy_speed = SPEED_XGMAC_10M;
	} else if (phy_speed == GSW_PORT_SPEED_100) {	/* 100 Mbps */
		mii_intf = gswss_get_fe_intf(pdev);

		if (mii_intf == LMAC_MII)
			phy_speed = SPEED_LMAC_100M;
		else if (mii_intf == XGMAC_GMII)
			phy_speed = SPEED_XGMAC_100M;
	} else if (phy_speed == GSW_PORT_SPEED_1000) { /* 1 Gbps */
		mii_intf = gswss_get_1g_intf(pdev);

		if (mii_intf == XGMAC_GMII)
			phy_speed = SPEED_XGMAC_1G;
		else if (mii_intf == LMAC_GMII)
			phy_speed = SPEED_LMAC_1G;
	} else if (phy_speed == GSW_PORT_SPEED_25000) { /* 2.5Gbps */
		mii_intf = gswss_get_2G5_intf(pdev);

		if (mii_intf == XGMAC_GMII)
			phy_speed = SPEED_GMII_25G;
		else if (mii_intf == XGMAC_XGMII)
			phy_speed = SPEED_XGMII_25G;
	} else if (phy_speed == GSW_PORT_SPEED_100000) {  /* 10Gbps */
		phy_speed = SPEED_XGMAC_10G;
	} else if (phy_speed == 0) {
		phy_speed = SPEED_MAC_AUTO;
	} else {
		ret = -1;
		goto err;
	}

	mac_set_physpeed(pdev, phy_speed);

err:
	return ret;
}

int mac_set_physpeed(void *pdev, u32 phy_speed)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	xgmac_set_extcfg(pdev, 1);

	switch (phy_speed) {
	case SPEED_MAC_AUTO:
		gswss_set_speed(pdev, SPEED_AUTO);
		break;

	case SPEED_XGMAC_10G:
		xgmac_set_xgmii_speed(pdev);
		gswss_set_speed(pdev, SPEED_10G);
		break;

	case SPEED_GMII_25G:
		gswss_set_speed(pdev, SPEED_2G5);
		gswss_set_2G5_intf(pdev, XGMAC_GMII);
		xgmac_set_gmii_2500_speed(pdev);
		break;

	case SPEED_XGMII_25G:
		gswss_set_speed(pdev, SPEED_2G5);
		gswss_set_2G5_intf(pdev, XGMAC_XGMII);
		xgmac_set_xgmii_2500_speed(pdev);
		break;

	case SPEED_XGMAC_1G:
		gswss_set_speed(pdev, SPEED_1G);
		gswss_set_1g_intf(pdev, XGMAC_GMII);
		xgmac_set_gmii_speed(pdev);
		xgmac_set_extcfg(pdev, 1);
		break;

	case SPEED_XGMAC_10M:
		gswss_set_speed(pdev, SPEED_10M);

	case SPEED_XGMAC_100M:
		if (phy_speed != SPEED_XGMAC_10M)
			gswss_set_speed(pdev, SPEED_100M);

		gswss_set_fe_intf(pdev, XGMAC_GMII);
		gswss_set_1g_intf(pdev, XGMAC_GMII);
		xgmac_set_gmii_speed(pdev);
		break;

	case SPEED_LMAC_10M:
		gswss_set_speed(pdev, SPEED_10M);

	case SPEED_LMAC_100M:
		if (phy_speed != SPEED_LMAC_10M)
			gswss_set_speed(pdev, SPEED_100M);

		gswss_set_fe_intf(pdev, LMAC_MII);
		lmac_set_intf_mode(pdev, 1);
		break;

	case SPEED_LMAC_1G:
		gswss_set_speed(pdev, SPEED_1G);
		gswss_set_1g_intf(pdev, LMAC_GMII);
		lmac_set_intf_mode(pdev, 2);
		break;

	default:
		break;
	}

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_get_speed(void *pdev)
{
	int mac_speed;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	mac_speed = gswss_get_speed(pdev);

	if (mac_speed == SPEED_10M)
		mac_speed = GSW_PORT_SPEED_10;
	else if (mac_speed == SPEED_100M)
		mac_speed = GSW_PORT_SPEED_100;
	else if (mac_speed == SPEED_1G)
		mac_speed = GSW_PORT_SPEED_1000;
	else if (mac_speed == SPEED_10G)
		mac_speed = GSW_PORT_SPEED_100000;
	else if (mac_speed == SPEED_2G5)
		mac_speed = GSW_PORT_SPEED_25000;
	else if (mac_speed == SPEED_AUTO)
		mac_speed = 0;

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return mac_speed;
}

int mac_set_linksts(void *pdev, u32 linksts)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	if (linksts == GSW_PORT_LINK_UP)
		gswss_set_linkstatus(pdev, 1);
	else if (linksts == GSW_PORT_LINK_DOWN)
		gswss_set_linkstatus(pdev, 2);
	else
		gswss_set_linkstatus(pdev, 0); /* Auto */

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_get_linksts(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int linksts;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	linksts = gswss_get_linkstatus(pdev);

	if (linksts == 1)
		linksts = GSW_PORT_LINK_UP;
	else if (linksts == 2)
		linksts = GSW_PORT_LINK_DOWN;
	else
		linksts = -1;

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return linksts;
}

int mac_set_lpien(void *pdev, u32 enable, u32 lpi_waitg, u32 lpi_waitm)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

#ifdef CONFIG_SILVER_WORKAROUND
#else
	mac_printf("Configuring LPIEN\n");
	lmac_set_lpi(pdev, enable, lpi_waitg, lpi_waitm);
	xgmac_set_eee_mode(pdev, enable);
	xgmac_set_eee_timer(pdev, pdata->twt, pdata->lst);
#endif

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_get_lpien(void *pdev)
{
	int enable;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	enable = xgmac_get_eee_mode(pdev);

	mac_printf("Returning lpien = %d\n", enable);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return enable;
}

int mac_set_mtu(void *pdev, u32 mtu)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int ret = 0;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	if (gswss_get_mac_rxfcs_op(pdev) == (RX_FCS_REMOVE % 4))
		mtu -= ETHERNET_FCS_SIZE;
	if (gswss_get_mac_rxtime_op(pdev) == (RX_TIME_INSERT % 4))
		mtu += TIMESTAMP80_SIZE;
	if (gswss_get_mac_rxsptag_op(pdev) == (RX_SPTAG_INSERT % 4))
		mtu += SPTAG_SIZE;

	if (mtu > FALCON_MAX_MTU) {
		ret = -1;
		goto err;
	}

	gswss_set_mtu(pdev, mtu);

	if (mtu > XGMAC_MAX_STD_PACKET)
		xgmac_config_jumbo_pkt(pdev, mtu);
	else
		xgmac_config_std_pkt(pdev);

err:
#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return ret;
}

int mac_get_mtu(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int mtu = 0;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	mtu = gswss_get_mtu(pdev);

	if (gswss_get_mac_rxfcs_op(pdev) == (RX_FCS_REMOVE % 4))
		mtu += ETHERNET_FCS_SIZE;
	if (gswss_get_mac_rxtime_op(pdev) == (RX_TIME_INSERT % 4))
		mtu -= TIMESTAMP80_SIZE;
	if (gswss_get_mac_rxsptag_op(pdev) == (RX_SPTAG_INSERT % 4))
		mtu -= SPTAG_SIZE;

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return mtu;
}

int mac_oper_cfg(void *pdev, MAC_OPER_CFG oper)
{

	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mode;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif
	mode = oper % 4;

	switch (oper) {
	case TX_FCS_NO_INSERT:
	case TX_FCS_INSERT:
		gswss_set_mac_txfcs_ins_op(pdev, mode);
		break;

	case TX_FCS_NO_REMOVE:
	case TX_FCS_REMOVE:
		gswss_set_mac_txfcs_rm_op(pdev, mode);
		break;

	case TX_SPTAG_KEEP:
	case TX_SPTAG_NOTAG:
	case TX_SPTAG_REMOVE:
	case TX_SPTAG_REPLACE:
		gswss_set_mac_txsptag_op(pdev, mode);
		break;

	case RX_FCS_NOFCS:
	case RX_FCS_NO_REMOVE:
	case RX_FCS_REMOVE:
		gswss_set_mac_rxfcs_op(pdev, mode);
		break;

	case RX_TIME_INSERT:
	case RX_TIME_NOTS:
	case RX_TIME_NO_INSERT:
		gswss_set_mac_rxtime_op(pdev, mode);
		break;

	case RX_SPTAG_INSERT:
	case RX_SPTAG_NOTAG:
	case RX_SPTAG_NO_INSERT:
		gswss_set_mac_rxsptag_op(pdev, mode);
		break;

	default:
		break;
	}

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_get_oper_cfg(void *pdev, MAC_OPER_CFG oper)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int ret = 0;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	switch (oper) {
	case TX_FCS_NO_INSERT:
	case TX_FCS_INSERT:
		ret = gswss_get_mac_txfcs_ins_op(pdev);
		break;

	case TX_FCS_NO_REMOVE:
	case TX_FCS_REMOVE:
		ret = gswss_get_mac_txfcs_rm_op(pdev);
		break;

	case TX_SPTAG_KEEP:
	case TX_SPTAG_NOTAG:
	case TX_SPTAG_REMOVE:
	case TX_SPTAG_REPLACE:
		ret = gswss_get_mac_txsptag_op(pdev);
		break;

	case RX_FCS_NOFCS:
	case RX_FCS_NO_REMOVE:
	case RX_FCS_REMOVE:
		ret = gswss_get_mac_rxfcs_op(pdev);
		break;

	case RX_TIME_INSERT:
	case RX_TIME_NOTS:
	case RX_TIME_NO_INSERT:
		ret = gswss_get_mac_rxtime_op(pdev);
		break;

	case RX_SPTAG_INSERT:
	case RX_SPTAG_NOTAG:
	case RX_SPTAG_NO_INSERT:
		ret = gswss_get_mac_rxsptag_op(pdev);
		break;

	default:
		break;
	}

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return ret;
}

static int mac_set_rxcrccheck(void *pdev, u8 disable)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	xgmac_set_rxcrc(pdev, disable);

	if (disable) {
		/* Packet dont have FCS and FCS is not removed */
		gswss_set_mac_rxfcs_op(pdev, MODE0);
	} else {
		/* Packet have FCS and FCS is removed */
		gswss_set_mac_rxfcs_op(pdev, MODE2);
	}

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

static int mac_set_sptag(void *pdev, u8 mode)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	gswss_set_mac_txsptag_op(pdev, mode);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

static int mac_set_macaddr(void *pdev, u8 *mac_addr)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	lmac_set_pauseframe_addr(pdev, mac_addr);

	/* Program MAC Address */
	xgmac_set_mac_address(pdev, mac_addr);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}


int mac_set_pfsa(void *pdev, u8 *mac_addr, u32 mode)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	lmac_set_pauseframe_samode(pdev, mode);
	lmac_set_pauseframe_addr(pdev, mac_addr);

	/* Program MAC Address */
	xgmac_set_mac_address(pdev, mac_addr);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_get_pfsa(void *pdev, u8 *mac_addr, u32 *mode)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	*mode = lmac_get_pauseframe_samode(pdev);
	lmac_get_pauseframe_addr(pdev, mac_addr);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_set_fcs_gen(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	gswss_set_xgmac_crc_ctrl(pdev, val);

	if ((val == GSW_CRC_PAD_INS_EN) || (val == GSW_CRC_EN_PAD_DIS))
		lmac_set_txfcs(pdev, 1);
	else
		lmac_set_txfcs(pdev, 0);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_get_fcs_gen(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int tx_fcs = 0;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->mac_lock);
#endif

	tx_fcs = gswss_get_xgmac_crc_ctrl(pdev);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return tx_fcs;
}

/* Enables the Attachment of timestamp in Rx Direction and
 * Removal of Special tag in Tx Direction
 */
int mac_enable_ts(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
#ifdef __KERNEL__
	struct core_ops *ops = gsw_get_swcore_ops(0);
	GSW_CPU_PortCfg_t cpu_port_cfg;
#if IS_ENABLED(CONFIG_LTQ_DATAPATH_PTP1588_SW_WORKAROUND)
	int pce_rule_id = 0;
	GSW_PCE_rule_t pce;
#endif

	if (!ops) {
		mac_printf("Open SWAPI device FAILED!\n");
		return -1;
	}

	memset((void *)&cpu_port_cfg, 0x00, sizeof(cpu_port_cfg));

	/* Enable the Egress Special Tag */
	ops->gsw_common_ops.CPU_PortCfgGet(ops, &cpu_port_cfg);
	cpu_port_cfg.nPortId = pdata->mac_idx;
	cpu_port_cfg.bSpecialTagIngress = 1;
	cpu_port_cfg.bSpecialTagEgress = 1;
	ops->gsw_common_ops.CPU_PortCfgSet(ops, &cpu_port_cfg);

#if IS_ENABLED(CONFIG_LTQ_DATAPATH_PTP1588_SW_WORKAROUND)
	/* All RX packets in GSWIP currently have PTP=1 */
	memset(&pce, 0, sizeof(pce));
	pce.pattern.nIndex = pce_rule_id;
	pce.pattern.bEnable = 1;
	pce.action.eTimestampAction = 2;
	ops->gsw_tflow_ops.TFLOW_PceRuleWrite(ops, &pce);
#endif
	spin_lock_bh(&pdata->mac_lock);
#endif

	//mac_printf("MAC %d: Enable Timestamp operations\n", pdata->mac_idx);

	/* Adjust MTU */
	if (gswss_get_mac_rxtime_op(pdev) != MODE1)
		gswss_set_mtu(pdev, gswss_get_mtu(pdev) + TIMESTAMP80_SIZE);

	/* Tell adaption layer to attach Timestamp */
	gswss_set_mac_rxtime_op(pdev, MODE1);

	/* Tell adaption layer to remove Special Tag in Tx Directon */
	gswss_set_mac_txsptag_op(pdev, MODE3);

	mac_int_enable(pdev);

	xgmac_set_mac_int(pdev, XGMAC_TSTAMP_EVNT, 1);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_disable_ts(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#ifdef __KERNEL__

	GSW_CPU_PortCfg_t cpu_port_cfg;
	struct core_ops *ops = gsw_get_swcore_ops(0);

	if (!ops) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EIO;
	}

	memset((void *)&cpu_port_cfg, 0x00, sizeof(cpu_port_cfg));

	/* Disable the Egress Special Tag */
	ops->gsw_common_ops.CPU_PortCfgGet(ops, &cpu_port_cfg);
	cpu_port_cfg.nPortId = pdata->mac_idx;
	cpu_port_cfg.bSpecialTagEgress = 0;
	ops->gsw_common_ops.CPU_PortCfgSet(ops, &cpu_port_cfg);

	spin_lock_bh(&pdata->mac_lock);
#endif
	mac_printf("MAC %d: Disable Timestamp operations\n", pdata->mac_idx);

	/* Adjust MTU */
	if (gswss_get_mac_rxtime_op(pdev) == MODE1)
		gswss_set_mtu(pdev, gswss_get_mtu(pdev) - TIMESTAMP80_SIZE);

	gswss_set_mac_rxtime_op(pdev, MODE0);
	gswss_set_mac_txsptag_op(pdev, MODE0);

	xgmac_disable_tstamp(pdev);

	xgmac_set_mac_int(pdev, XGMAC_TSTAMP_EVNT, 0);

	pdata->systime_initialized = 0;

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->mac_lock);
#endif

	return 0;
}

int mac_get_int_sts(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int val = 0;
	u32 mac_isr = XGMAC_RGRD(pdata, MAC_ISR);
	u32 mtl_q_isr = XGMAC_RGRD(pdata, MTL_Q_ISR);
	u32 lmac_pisr = LMAC_RGRD(pdata, MAC_PISR(pdata->mac_idx));

	//mac_printf("MAC ISR %x MTL ISR %x LMAC Pisr %x\n", mac_isr, mtl_q_isr,
	//	   lmac_pisr);

	/* XGMAC Mac ISR Status */
	if (mac_isr) {
		/* Timestamp interrupt */
		if (MAC_GET_VAL(mac_isr, MAC_ISR, TSIS))
			val |= (1 << XGMAC_TSTAMP_EVNT);

		/* LPI interrupt (EEE) */
		if (MAC_GET_VAL(mac_isr, MAC_ISR, LPIIS))
			val |= (1 << XGMAC_LPI_EVNT);

		/* transmit error status interrupt */
		if (MAC_GET_VAL(mac_isr, MAC_ISR, TXESIS))
			val |= (1 << XGMAC_TXERR_STS_EVNT);

		/* Receive error status interrupt */
		if (MAC_GET_VAL(mac_isr, MAC_ISR, RXESIS))
			val |= (1 << XGMAC_RXERR_STS_EVNT);

		/* Power Management interrupt */
		if (MAC_GET_VAL(mac_isr, MAC_ISR, PMTIS))
			val |= (1 << XGMAC_PMT_EVNT);
	}

	/* XGMAC MTL ISR Status */
	if (mtl_q_isr) {
		/* Tx Q Overflow Interrupt Enable */
		if (MAC_GET_VAL(mtl_q_isr, MTL_Q_ISR, TXUNFIS))
			val |= (1 << XGMAC_TXQ_OVFW_EVNT);

		/* Rx Q Overflow Interrupt Enable */
		if (MAC_GET_VAL(mtl_q_isr, MTL_Q_ISR, ABPSIS))
			val |= (1 << XGMAC_RXQ_OVFW_EVNT);

		/* Average bits per slot interrupt enable */
		if (MAC_GET_VAL(mtl_q_isr, MTL_Q_ISR, RXOVFIS))
			val |= (1 << XGMAC_AVG_BPS_EVNT);
	}

	/* LMAC ISR Status */
	if (lmac_pisr) {
		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, PHYERR))
			val |= (1 << LMAC_PHYERR_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, ALIGN))
			val |= (1 << LMAC_ALIGN_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, SPEED))
			val |= (1 << LMAC_SPEED_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, FDUP))
			val |= (1 << LMAC_FDUP_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, RXPAUEN))
			val |= (1 << LMAC_RXPAUEN_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, TXPAUEN))
			val |= (1 << LMAC_TXPAUEN_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, LPIOFF))
			val |= (1 << LMAC_LPIOFF_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, LPION))
			val |= (1 << LMAC_LPION_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, JAM))
			val |= (1 << LMAC_JAM_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, FCSERR))
			val |= (1 << LMAC_FCSERR_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, TXPAUSE))
			val |= (1 << LMAC_TXPAU_EVNT);

		if (MAC_GET_VAL(lmac_pisr, MAC_PISR, RXPAUSE))
			val |= (1 << LMAC_RXPAU_EVNT);
	}

	return val;
}

int mac_init(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

#if defined(PC_UTILITY) || defined(CHIPTEST)
	int i = 0;

	xgmac_init_pdata(pdata, -1);

	mac_printf("XGMAC INIT for Module %d\n", pdata->mac_idx);
#endif

	/* Get all hw capability */
	xgmac_get_hw_capability(pdev);

	/* Initialize MAC related features */
	/* Program MAC Address */
	xgmac_set_mac_address(pdev, pdata->mac_addr);

	/* Program for Jumbo/Std settings */
	mac_set_mtu(pdev, pdata->mtu);

	/* TODO: Interrupt enable change to Irq register */
	/* Configure RWK filter frames */

	populate_filter_frames(pdev);

	/* Program Promisc mode and All multicast mode */
	xgmac_set_promiscuous_mode(pdev, pdata->promisc_mode);

	/* Program All multicast mode */
	xgmac_set_all_multicast_mode(pdev, pdata->all_mcast_mode);


#ifdef __KERNEL__
	/* Default enable flow control Rx and TX */
	mac_set_flowctrl(pdev, 3);
#endif
	/* Configure MII for 1G and 2.5G to XGMAC by default
     * default value is LMAC
     */
	mac_set_mii_if(pdev, GSW_PORT_HW_GMII);

	if (pdata->haps) {
		/* Set the link status as UP */
		mac_set_linksts(pdev, GSW_PORT_LINK_UP);

		/* Set the Duplex as Full Duplex */
		mac_set_duplex(pdev, GSW_DUPLEX_FULL);
	} else {
		/* Set the link status as AUTO */
		mac_set_linksts(pdev, GSW_PORT_LINK_AUTO);

		/* Set the Duplex as AUTO */
		mac_set_duplex(pdev, GSW_DUPLEX_AUTO);
	}

	/* Set LPI TX Automate and LPI Auto Timer Enable to
	 * overcome packet-drop issue
	 */
	xgmac_set_mac_lpitx(pdev, 1);

#if defined(UPTIME) && UPTIME

	if (pdata->mac_idx == MAC_2)
		xgmac_set_hwtstamp_settings(pdev, 1, 1);

#endif

	/* Configure MAC Speed to 10/2.5/1 G */
	if (pdata->haps)
		mac_set_physpeed(pdev, pdata->phy_speed);
	else
		mac_set_physpeed(pdev, SPEED_MAC_AUTO);

	/* POWER ON MAC Tx/Rx */
	xgmac_powerup(pdev);

#ifdef __KERNEL__
	/* Filter pause frames from XGMAC */
	xgmac_pause_frame_filtering(pdev, 1);
#endif
	fifo_init(pdev);

	/* EEE Capability Turn OFF as Aquania OnBoard Phy and SFP+ will go down
	 * with EEE Capability ON/Auto.
	 */
	gswss_set_eee_cap(pdev, EEE_CAP_OFF);

	/* Tell adaption layer to remove FCS in Rx Direction */
	gswss_set_mac_rxfcs_op(pdev, MODE3);

	/* Insert Dummy Special tag from GSWIP Subsys
	 * Bug Fix for Insertion was not getting hit in PCE rule
	 * Default Insert a dummy Special Tag to all packets entering from MAC
         * MAC -> GSWIP
	 */
	gswss_set_mac_rxsptag_op(pdev, (RX_SPTAG_INSERT % 4));

	/* By default setting FDMA Don't Remove FCS @ Tx
	 * PMAC is now set as no dummy FCS addition @ Tx,
	 * Otherwise Insertion case will have 4 Extra Byte 0's
	 */
	gswss_set_mac_txfcs_rm_op(pdev, (TX_FCS_NO_REMOVE % 4));

	/* Set XGMAC Port to MDIO Clause 22 */
	mdio_set_clause(pdev, 1, (pdata->mac_idx - MAC_2));

#ifdef __KERNEL__
	xgmac_mdio_register(pdev);
#endif

#ifdef CONFIG_PTP_1588_CLOCK
	xgmac_ptp_init(pdev);
#endif

	return 0;
}

/* TRCSTS: MTL Tx Queue Read Controller Status
 *	This field indicates the state of the Tx Queue Read Controller:
 *	00 Idle state
 *	01 Read state (transferring data to the MAC transmitter)
 *	10 Waiting for pending Tx Status from the MAC transmitter
 *	11 Flushing the Tx queue because of the Packet Abort request from
 *	the MAC
 * TXQSTS: MTL Tx Queue Not Empty Status
 *	if 1, it indicates that the MTL Tx Queue is not empty
 *	and some data is left for transmission.
 * PRXQ: Number of Packets in Receive Queue
 *	This field indicates the current number of packets in the Rx Queue. The
 *	theoretical maximum value for this field is 256KB/16B = 16K Packets,
 *	that is, Max_Queue_Size/Min_Packet_Size.
 * RXQSTS: MTL Rx Queue Fill-Level Status
 *	This field gives the status of the fill-level of the Rx Queue:
 *	00 Rx Queue empty
 *	01 Rx Queue fill-level below flow-control deactivate threshold
 *	10 Rx Queue fill-level above flow-control activate threshold
 *	11 Rx Queue full
 */
int mac_exit(void *pdev)
{
	u32 mtl_tx_debugq, trcsts, txqsts;
	u32 mtl_rx_debugq, prxq, rxqsts;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	mtl_tx_debugq = XGMAC_RGRD(pdata, MTL_Q_TQDG);
	trcsts = MAC_GET_VAL(mtl_tx_debugq, MTL_Q_TQDG, TRCSTS);
	txqsts = MAC_GET_VAL(mtl_tx_debugq, MTL_Q_TQDG, TXQSTS);

	if ((trcsts != 1) && (txqsts == 0)) {
		mac_printf("XGMAC EXIT for Module %d\n", pdata->mac_idx);
		/* POWER OFF MAC Tx/Rx */
		xgmac_powerdown(pdev);
	} else
		mac_printf("Can't Exit XGMAC %d now,data is left for txing\n",
			   pdata->mac_idx);

	mtl_rx_debugq = XGMAC_RGRD(pdata, MTL_Q_RQDG);
	prxq = MAC_GET_VAL(mtl_tx_debugq, MTL_Q_RQDG, PRXQ);
	rxqsts = MAC_GET_VAL(mtl_tx_debugq, MTL_Q_RQDG, RXQSTS);

	if ((prxq == 0) && (rxqsts == 0))
		mac_printf("All Data is transferred to system memory\n");
	else
		mac_printf("ERROR: Data is still txing to system memory\n");

#ifdef __KERNEL__
	xgmac_mdio_unregister(pdev);
#endif

	return 0;
}

int mac_int_enable(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct adap_ops *adap_ops = gsw_get_adap_ops(0);

	/* Enable LMAC 2/3/4 Interrupt */
	lmac_set_int(pdev, 1);
	/* Enable XGMAC 2/3/4 Interrupt */
	gswss_set_interrupt(adap_ops, XGMAC, pdata->mac_idx, 1);

	return 0;
}

int mac_int_disable(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct adap_ops *adap_ops = gsw_get_adap_ops(0);

	/* Enable LMAC 2/3/4 Interrupt */
	lmac_set_int(pdev, 0);
	/* Enable XGMAC 2/3/4 Interrupt */
	gswss_set_interrupt(adap_ops, XGMAC, pdata->mac_idx, 0);

	return 0;
}

int mac_irq_event_enable(void *pdev, GSW_Irq_Op_t *irq)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	/* Enable LMAC and XGMAC Interrupt */
	mac_int_enable(pdev);

	if (irq->blk == LMAC_BLK) {
		mac_printf("LMAC %d: mac_irq_enable Event %x\n",
			   pdata->mac_idx, irq->event);
		/* Enable LMAC Event Interrupt */
		lmac_set_event_int(pdev, irq->event, 1);
	} else if (irq->blk == XGMAC_BLK) {
		mac_printf("XGMAC %d: mac_irq_enable Event %x\n",
			   pdata->mac_idx, irq->event);
		/* Enable XGMAC Event Interrupt */
		xgmac_set_mac_int(pdev, irq->event, 1);
	} else {
		mac_printf("Unsupported MAC Block %d\n", irq->blk);
		return GSW_statusErr;
	}

	return 0;
}

int mac_irq_event_disable(void *pdev, GSW_Irq_Op_t *irq)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	if (irq->blk == LMAC_BLK) {
		mac_printf("LMAC %d: mac_irq_disable Event %x\n",
			   pdata->mac_idx, irq->event);
		/* Disable LMAC Event Interrupt */
		lmac_set_event_int(pdev, irq->event, 0);
	} else if (irq->blk == XGMAC_BLK) {
		mac_printf("XGMAC %d: mac_irq_disable Event %x\n",
			   pdata->mac_idx, irq->event);
		/* Disable XGMAC Event Interrupt */
		xgmac_set_mac_int(pdev, irq->event, 0);
	} else {
		mac_printf("Unsupported MAC Block %d\n", irq->blk);
		return GSW_statusErr;
	}

	return 0;
}

int mac_irq_register(void *pdev, GSW_Irq_Op_t *irq)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	if (irq->event >= MAX_IRQ_EVNT) {
		mac_printf("Wrong Irq event\n");
		return -1;
	}

	mac_printf("MAC %d: mac_irq_register Event %x\n",
		   pdata->mac_idx, irq->event);

	pdata->irq_hdl[irq->event].cb = irq->call_back;
	pdata->irq_hdl[irq->event].param = irq->param;

	return 0;
}

int mac_irq_unregister(void *pdev, GSW_Irq_Op_t *irq)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	if (irq->event >= MAX_IRQ_EVNT) {
		mac_printf("Wrong Irq event\n");
		return -1;
	}

	mac_printf("MAC %d: mac_irq_unregister Event %x\n",
		   pdata->mac_idx, irq->event);

	pdata->irq_hdl[irq->event].cb = NULL;
	pdata->irq_hdl[irq->event].param = NULL;

	return 0;
}

int mac_irq_clr(void *pdev, u32 event)
{
	switch (event) {
	case XGMAC_PMT_EVNT:
		xgmac_pmt_int_clr(pdev);
		break;

	case XGMAC_TSTAMP_EVNT:
#ifdef __KERNEL__
		xgmac_ptp_isr_hdlr(pdev);
#endif
		break;

	case XGMAC_LPI_EVNT:
		xgmac_lpi_int_clr(pdev);
		break;

	/* These 2 interrupts are cleared by a read RXESIS or TXESIS
	 * Which will be done while reading Interrupt Status
	 */
	case XGMAC_TXERR_STS_EVNT:
	case XGMAC_RXERR_STS_EVNT:
		break;

	case XGMAC_TXQ_OVFW_EVNT:
	case XGMAC_RXQ_OVFW_EVNT:
	case XGMAC_AVG_BPS_EVNT:
		xgmac_clear_mtl_int(pdev, event);
		break;

	case LMAC_PHYERR_EVNT:
	case LMAC_ALIGN_EVNT:
	case LMAC_SPEED_EVNT:
	case LMAC_FDUP_EVNT:
	case LMAC_RXPAUEN_EVNT:
	case LMAC_TXPAUEN_EVNT:
	case LMAC_LPIOFF_EVNT:
	case LMAC_LPION_EVNT:
	case LMAC_JAM_EVNT:
	case LMAC_FCSERR_EVNT:
	case LMAC_TXPAU_EVNT:
	case LMAC_RXPAU_EVNT:
		lmac_clear_int(pdev, event);
		break;

	default:
		break;
	}

	return 0;
}

static void mac_soft_restart(void *pdev)
{
	/* Bring Down and up Xgmac Tx and Rx, Not all register reset */
	xgmac_powerdown(pdev);
	xgmac_powerup(pdev);
}

void mac_init_fn_ptrs(struct mac_ops *mac_op)
{
	mac_op->set_flow_ctl = mac_set_flowctrl;
	mac_op->get_flow_ctl = mac_get_flowctrl;

	mac_op->mac_reset = mac_reset;

	mac_op->set_speed = mac_set_speed;
	mac_op->get_speed = mac_get_speed;

	mac_op->set_duplex = mac_set_duplex;
	mac_op->get_duplex = mac_get_duplex;

	mac_op->mac_config_ipg = mac_config_ipg;
	mac_op->mac_config_loopback = mac_config_loopback;

	mac_op->set_lpi = mac_set_lpien;
	mac_op->get_lpi = mac_get_lpien;

	mac_op->set_mii_if = mac_set_mii_if;
	mac_op->get_mii_if = mac_get_mii_if;

	mac_op->set_link_sts = mac_set_linksts;
	mac_op->get_link_sts = mac_get_linksts;

	mac_op->set_mtu = mac_set_mtu;
	mac_op->get_mtu = mac_get_mtu;

	mac_op->set_pfsa = mac_set_pfsa;
	mac_op->get_pfsa = mac_get_pfsa;

	mac_op->set_fcsgen = mac_set_fcs_gen;
	mac_op->get_fcsgen = mac_get_fcs_gen;

	mac_op->get_int_sts = mac_get_int_sts;
	mac_op->clr_int_sts = mac_irq_clr;

	mac_op->init_systime = xgmac_init_systime;
	mac_op->config_addend = xgmac_set_tstamp_addend;
	mac_op->adjust_systime = xgmac_adjust_systime;
	mac_op->get_systime = xgmac_get_systime;
	mac_op->get_tx_tstamp = xgmac_get_tx_tstamp;
	mac_op->config_hw_time_stamping = xgmac_set_hwtstamp_settings;
	mac_op->config_subsec_inc = xgmac_config_subsec_inc;
	mac_op->get_txtstamp_cap_cnt = xgmac_get_txtstamp_cnt;

	mac_op->init = mac_init;
	mac_op->exit = mac_exit;
	mac_op->xgmac_cli = xgmac_cfg_main;
	mac_op->lmac_cli = lmac_cfg_main;
	mac_op->xgmac_reg_rd = xgmac_rd_reg;
	mac_op->xgmac_reg_wr = xgmac_wr_reg;
	mac_op->lmac_reg_rd = lmac_rd_reg;
	mac_op->lmac_reg_wr = lmac_wr_reg;

#ifdef __KERNEL__
	mac_op->set_hwts = xgmac_set_hwts;
	mac_op->get_hwts = xgmac_get_hwts;
	mac_op->do_rx_hwts = xgmac_rx_hwts;
	mac_op->do_tx_hwts = xgmac_tx_hwts;
	mac_op->mac_get_ts_info = xgmac_get_ts_info;
#endif
	mac_op->soft_restart = mac_soft_restart;

	mac_op->set_macaddr = mac_set_macaddr;
	mac_op->set_rx_crccheck = mac_set_rxcrccheck;
	mac_op->set_sptag = mac_set_sptag;

	mac_op->mac_int_en = mac_int_enable;
	mac_op->mac_int_dis = mac_int_disable;

	mac_op->IRQ_Disable = mac_irq_event_disable;
	mac_op->IRQ_Enable = mac_irq_event_enable;
	mac_op->IRQ_Register = mac_irq_register;
	mac_op->IRQ_UnRegister = mac_irq_unregister;

	mac_op->mac_op_cfg = mac_oper_cfg;
}
