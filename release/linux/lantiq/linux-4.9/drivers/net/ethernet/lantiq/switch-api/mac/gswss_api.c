/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#include <gswss_api.h>

int gswss_cfg0_1588(void *pdev, u32 ref_time, u32 dig_time, u32 bin_time,
		    u32 pps_sel)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 cfg0;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif

	cfg0 = GSWSS_RGRD(pdata, CFG0_1588);

	if (ref_time == 0)
		mac_dbg("\tREF_TIME: PON_PCS is the master\n");
	else if (ref_time == 1)
		mac_dbg("\tREF_TIME: PCIE0 is the master\n");
	else if (ref_time == 2)
		mac_dbg("\tREF_TIME: PCIE1 is the master\n");
	else if (ref_time == 3)
		mac_dbg("\tREF_TIME: XGMAC2 is the master\n");
	else if (ref_time == 4)
		mac_dbg("\tREF_TIME: XGMAC3 is the master\n");
	else if (ref_time == 5)
		mac_dbg("\tREF_TIME: XGMAC4 is the master\n");
	else
		goto end;

	if (dig_time == 0)
		mac_dbg("\tDIG_TIME: PON_PCS is the master\n");
	else if (dig_time == 1)
		mac_dbg("\tDIG_TIME: PCIE0 is the master\n");
	else if (dig_time == 2)
		mac_dbg("\tDIG_TIME: PCIE1 is the master\n");
	else if (dig_time == 3)
		mac_dbg("\tDIG_TIME: XGMAC2 is the master\n");
	else if (dig_time == 4)
		mac_dbg("\tDIG_TIME: XGMAC3 is the master\n");
	else if (dig_time == 5)
		mac_dbg("\tDIG_TIME: XGMAC4 is the master\n");
	else
		goto end;

	if (bin_time == 0)
		mac_dbg("\tBIN_TIME: PON_PCS is the master\n");
	else if (bin_time == 1)
		mac_dbg("\tBIN_TIME: PCIE0 is the master\n");
	else if (bin_time == 2)
		mac_dbg("\tBIN_TIME: PCIE1 is the master\n");
	else if (bin_time == 3)
		mac_dbg("\tBIN_TIME: XGMAC2 is the master\n");
	else if (bin_time == 4)
		mac_dbg("\tBIN_TIME: XGMAC3 is the master\n");
	else if (bin_time == 5)
		mac_dbg("\tBIN_TIME: XGMAC4 is the master\n");
	else
		goto end;

	if (pps_sel == 0)
		mac_dbg("\tPPS_SEL: PON_PCS is the master\n");
	else if (pps_sel == 1)
		mac_dbg("\tPPS_SEL: PCIE0 is the master\n");
	else if (pps_sel == 2)
		mac_dbg("\tPPS_SEL: PCIE1 is the master\n");
	else if (pps_sel == 3)
		mac_dbg("\tPPS_SEL: XGMAC2 is the master\n");
	else if (pps_sel == 4)
		mac_dbg("\tPPS_SEL: XGMAC3 is the master\n");
	else if (pps_sel == 5)
		mac_dbg("\tPPS_SEL: XGMAC4 is the master\n");
	else if (pps_sel == 6)
		mac_dbg("\tPPS_SEL: PON PPS100US is the master\n");
	else if (pps_sel == 7)
		mac_dbg("\tPPS_SEL: Software trigger\n");
	else
		goto end;

	MAC_SET_VAL(cfg0, CFG0_1588, REFTIME, ref_time);
	MAC_SET_VAL(cfg0, CFG0_1588, DIGTIME, dig_time);
	MAC_SET_VAL(cfg0, CFG0_1588, BINTIME, bin_time);
	MAC_SET_VAL(cfg0, CFG0_1588, PPSSEL, pps_sel);

	GSWSS_RGWR(pdata, CFG0_1588, cfg0);

end:
#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return 0;
}

int gswss_get_cfg0_1588(void *pdev, u32 *ref_time, u32 *dig_time,
			u32 *bin_time, u32 *pps_sel)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 cfg0;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif

	cfg0 = GSWSS_RGRD(pdata, CFG0_1588);

	*ref_time = MAC_GET_VAL(cfg0, CFG0_1588, REFTIME);
	*dig_time = MAC_GET_VAL(cfg0, CFG0_1588, DIGTIME);
	*bin_time = MAC_GET_VAL(cfg0, CFG0_1588, BINTIME);
	*pps_sel = MAC_GET_VAL(cfg0, CFG0_1588, PPSSEL);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return 0;
}

int gswss_cfg1_1588(void *pdev, u32 trig0_sel, u32 trig1_sel, u32 sw_trig)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 cfg0;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif

	cfg0 = GSWSS_RGRD(pdata, CFG1_1588);

	if (trig0_sel == 0)
		mac_dbg("\tTRIG0: PON is the master\n");
	else if (trig0_sel == 1)
		mac_dbg("\tTRIG0: PCIE0 is the master\n");
	else if (trig0_sel == 2)
		mac_dbg("\tTRIG0: PCIE1 is the master\n");
	else if (trig0_sel == 3)
		mac_dbg("\tTRIG0: XGMAC2 is the master\n");
	else if (trig0_sel == 4)
		mac_dbg("\tTRIG0: XGMAC3 is the master\n");
	else if (trig0_sel == 5)
		mac_dbg("\tTRIG0: XGMAC4 is the master\n");
	else if (trig0_sel == 6)
		mac_dbg("\tTRIG0: PON100US is the master\n");
	else if (trig0_sel == 8)
		mac_dbg("\tTRIG0: EXTPPS0 is the master\n");
	else if (trig0_sel == 9)
		mac_dbg("\tTRIG0: EXTPPS1 is the master\n");
	else if (trig0_sel == 10)
		mac_dbg("\tTRIG0: Software Trigger\n");
	else
		goto end;

	if (trig1_sel == 0)
		mac_dbg("\tTRIG1: PON is the master\n");
	else if (trig1_sel == 1)
		mac_dbg("\tTRIG1: PCIE0 is the master\n");
	else if (trig1_sel == 2)
		mac_dbg("\tTRIG1: PCIE1 is the master\n");
	else if (trig1_sel == 3)
		mac_dbg("\tTRIG1: XGMAC2 is the master\n");
	else if (trig1_sel == 4)
		mac_dbg("\tTRIG1: XGMAC3 is the master\n");
	else if (trig1_sel == 5)
		mac_dbg("\tTRIG1: XGMAC4 is the master\n");
	else if (trig1_sel == 6)
		mac_dbg("\tTRIG1: PON100US is the master\n");
	else if (trig1_sel == 8)
		mac_dbg("\tTRIG1: EXTPPS0 is the master\n");
	else if (trig1_sel == 9)
		mac_dbg("\tTRIG1: EXTPPS1 is the master\n");
	else if (trig1_sel == 10)
		mac_dbg("\tTRIG1: Software Trigger\n");
	else
		goto end;

	if (sw_trig == 0)
		mac_dbg("\tSW_TRIG: 0\n");
	else if (sw_trig == 1)
		mac_dbg("\tSW_TRIG: 1\n");

	MAC_SET_VAL(cfg0, CFG1_1588, TRIG1SEL, trig1_sel);
	MAC_SET_VAL(cfg0, CFG1_1588, TRIG0SEL, trig0_sel);
	MAC_SET_VAL(cfg0, CFG1_1588, SWTRIG, sw_trig);

	GSWSS_RGWR(pdata, CFG1_1588, cfg0);
end:
#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return 0;
}

int gswss_get_cfg1_1588(void *pdev, u32 *trig1_sel, u32 *trig0_sel,
			u32 *sw_trig)
{
	u32 cfg0;
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif

	cfg0 =  GSWSS_RGRD(pdata, CFG1_1588);

	*trig1_sel = MAC_GET_VAL(cfg0, CFG1_1588, TRIG1SEL);
	*trig0_sel = MAC_GET_VAL(cfg0, CFG1_1588, TRIG0SEL);
	*sw_trig = MAC_GET_VAL(cfg0, CFG1_1588, SWTRIG);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return 0;
}

int gswss_macsec_reset(void *pdev, u32 reset)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 macsec_en;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif

	macsec_en = GSWSS_RGRD(pdata, MACSEC_EN);

	MAC_SET_VAL(macsec_en, MACSEC_EN, RES, reset);

	mac_dbg("GSWSS: MACSEC reset : %s\n",
		   reset ? "ENABLED" : "DISABLED");

	GSWSS_RGWR(pdata, MACSEC_EN, macsec_en);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return 0;
}

int gswss_switch_ss_reset(void *pdev)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 core_reset;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif

	core_reset = GSWSS_RGRD(pdata, GSWIP_CFG);

	mac_dbg("Switch Subsys Resetting\n");

	MAC_SET_VAL(core_reset, GSWIP_CFG, SS_HWRES, 1);

	GSWSS_RGWR(pdata, GSWIP_CFG, core_reset);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return 0;
}


int gswss_set_clkmode(void *pdev, u32 clk_mode)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 clk_mode_cfg;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif

	clk_mode_cfg = GSWSS_RGRD(pdata, GSWIP_CFG);

	mac_dbg("GSWSS: Clock Mode Changing to :");

	if (clk_mode == 0)
		mac_dbg("%s\n", "NCO1 - 666 Mhz");
	else if (clk_mode == 1)
		mac_dbg("%s\n", "NCO2 - 450 Mhz");
	else if (clk_mode == 2)
		mac_dbg("%s\n", "Auto Mode (666/450) Mhz");
	else if (clk_mode == 3)
		mac_dbg("%s\n", "Auto Mode (666/450) Mhz");
	else
		goto end;

	MAC_SET_VAL(clk_mode_cfg, GSWIP_CFG, CLK_MD, clk_mode);

	GSWSS_RGWR(pdata, GSWIP_CFG, clk_mode_cfg);

end:
#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return 0;
}

int gswss_get_clkmode(void *pdev)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 clk_mode_cfg;
	int clk_mode;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif

	clk_mode_cfg = GSWSS_RGRD(pdata, GSWIP_CFG);
	clk_mode = MAC_GET_VAL(clk_mode_cfg, GSWIP_CFG, CLK_MD);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return clk_mode;
}

int gswss_set_corese(void *pdev, u32 val)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 core_en;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif
	core_en = GSWSS_RGRD(pdata, GSWIP_CFG);

	if (val == FREEZE) {
		pdata->core_en_cnt++;
	} else {
		if (pdata->core_en_cnt)
			pdata->core_en_cnt--;
	}

	/* Unfreeze only when Count is 0 & val is UNFREEZE
	 * Freeze when val is FREEZE
	 */
	if ((val == UNFREEZE && pdata->core_en_cnt == 0) ||
	    (val == FREEZE)) {
		MAC_SET_VAL(core_en, GSWIP_CFG, CORE_SE, val);
		GSWSS_RGWR(pdata, GSWIP_CFG, core_en);
	}

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif
	return 0;
}

int gswss_get_corese(void *pdev)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 core_en;
	int enable;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif
	core_en = GSWSS_RGRD(pdata, GSWIP_CFG);

	enable = MAC_GET_VAL(core_en, GSWIP_CFG, CORE_SE);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif
	return enable;
}

int gswss_set_macsec_to_mac(void *pdev, u32 mac_idx, u32 enable)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 macsec_en;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif
	macsec_en = GSWSS_RGRD(pdata, MACSEC_EN);

	mac_idx += 2;

	if (enable) {
		mac_dbg("GSWSS: MACSEC enabled to MAC %d data traffic\n",
			   mac_idx);
		MAC_SET_VAL(macsec_en, MACSEC_EN, SEL, mac_idx);
	} else {
		mac_dbg("GSWSS: MACSEC to MAC mapping : DISABLED\n");
		MAC_SET_VAL(macsec_en, MACSEC_EN, SEL, 0);
	}

	GSWSS_RGWR(pdata, MACSEC_EN, macsec_en);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return 0;
}

int gswss_get_macsec_to_mac(void *pdev)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 macsec_en, mac_idx;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif
	macsec_en = GSWSS_RGRD(pdata, MACSEC_EN);

	mac_idx = MAC_GET_VAL(macsec_en, MACSEC_EN, SEL);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif
	return mac_idx;
}

/* Calling from Interrupt Context, so no lock required. */
int gswss_set_interrupt(void *pdev, u32 mod, u32 idx, u32 enable)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 mac_int_en0, mac_int_en1;
	int ret = 0;

	switch (mod) {
	case XGMAC:
		//mac_dbg("XGMAC %d: Interrupt %s\n", idx,
		//	   enable ? "ENABLED" : "DISABLED");

		mac_int_en0 = GSWSS_RGRD(pdata, GSWIPSS_IER0);
		SET_N_BITS(mac_int_en0, idx, GSWIPSS_IER0_XGMAC2_WIDTH, enable);
		GSWSS_RGWR(pdata, GSWIPSS_IER0, mac_int_en0);
		break;

	case LINK:
		mac_dbg("LINK %d: Interrupt %s\n", idx,
			   enable ? "ENABLED" : "DISABLED");

		mac_int_en1 = GSWSS_RGRD(pdata, GSWIPSS_IER1);
		SET_N_BITS(mac_int_en1, idx, GSWIPSS_IER1_LINK2_WIDTH, enable);
		GSWSS_RGWR(pdata, GSWIPSS_IER1, mac_int_en1);
		break;

	default:
		ret = -1;
		break;
	}

	return ret;
}

/* Calling from Interrupt Context, so no lock required. */
int gswss_get_int_stat(void *pdev, u32 mod)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	int ret = 0;

	switch (mod) {
	case XGMAC:
		ret = GSWSS_RGRD(pdata, GSWIPSS_ISR0);
		break;

	case LINK:
		ret = GSWSS_RGRD(pdata, GSWIPSS_ISR1);
		break;

	default:
		ret = -1;
		break;
	}

	return ret;
}

int gswss_set_nco(void *pdev, u32 val, u32 nco_idx)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 nco_lsb, nco_msb;
	u32 lsb_off, msb_off;
	int ret = 0;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif

	if (nco_idx) {
		lsb_off = (NCO1_LSB + ((nco_idx - 1) * 0x8));
		msb_off = lsb_off + 4;
	} else {
		lsb_off = NCO_LSB;
		msb_off = lsb_off + 4;
	}

	nco_lsb = GSWSS_RGRD(pdata, lsb_off);
	nco_msb = GSWSS_RGRD(pdata, msb_off);

	nco_lsb = (val & 0xFFFF);
	nco_msb = ((val & 0xFFFF0000) >> 16);

	mac_dbg("GSWSS: NCO_LSB Configured to %x\n", nco_lsb);
	mac_dbg("GSWSS: NCO_MSB Configured to %x\n", nco_msb);

	GSWSS_RGWR(pdata, lsb_off, nco_lsb);
	GSWSS_RGWR(pdata, msb_off, nco_msb);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return ret;
}

int gswss_get_nco(void *pdev, u32 nco_idx)
{
	struct adap_prv_data *pdata = GET_ADAP_PDATA(pdev);
	u32 nco_lsb, nco_msb;
	u32 lsb_off, msb_off;
	int nco;

#ifdef __KERNEL__
	spin_lock_bh(&pdata->adap_lock);
#endif

	if (nco_idx) {
		lsb_off = (NCO1_LSB + ((nco_idx - 1) * 0x8));
		msb_off = lsb_off + 4;
	} else {
		lsb_off = NCO_LSB;
		msb_off = lsb_off + 4;
	}

	nco_lsb = GSWSS_RGRD(pdata, lsb_off);
	nco_msb = GSWSS_RGRD(pdata, msb_off);

	mac_dbg("GSWSS: NCO_LSB Got is %x\n", nco_lsb);
	mac_dbg("GSWSS: NCO_MSB Got is %x\n", nco_msb);

	nco = ((nco_msb << 16) | nco_lsb);

#ifdef __KERNEL__
	spin_unlock_bh(&pdata->adap_lock);
#endif

	return nco;
}

void gswss_init_fn_ptrs(struct adap_ops *adap_ops)
{
	adap_ops->ss_hwreset = gswss_switch_ss_reset;

	adap_ops->ss_set_cfg0_1588 = gswss_cfg0_1588;
	adap_ops->ss_get_cfg0_1588 = gswss_get_cfg0_1588;

	adap_ops->ss_set_clkmode = gswss_set_clkmode;
	adap_ops->ss_get_clkmode = gswss_get_clkmode;

	adap_ops->ss_core_en = gswss_set_corese;
	adap_ops->ss_get_core_en = gswss_get_corese;

	adap_ops->ss_set_macsec_mac = gswss_set_macsec_to_mac;
	adap_ops->ss_get_macsec_mac = gswss_get_macsec_to_mac;

	adap_ops->ss_reset_macsec = gswss_macsec_reset;

	adap_ops->ss_set_nco = gswss_set_nco;
	adap_ops->ss_get_nco = gswss_get_nco;

	adap_ops->ss_set_inten = gswss_set_interrupt;
	adap_ops->ss_get_intstat = gswss_get_int_stat;
	adap_ops->ss_cli = gswss_cfg_main;
	adap_ops->ss_rg_rd = gswss_rd_reg;
	adap_ops->ss_rg_wr = gswss_wr_reg;
}

