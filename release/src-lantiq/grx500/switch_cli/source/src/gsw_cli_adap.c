/****************************************************************************

                               Copyright 2010
                          Lantiq Deutschland GmbH
                   Am Campeon 3; 85579 Neubiberg, Germany

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*****************************************************************************/
#include "ltq_cli_lib.h"

#ifndef IOCTL_PCTOOL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#endif

#include "adap_ops.h"
#include "mac_ops.h"
#include "gsw_cli_mac.h"

int test_cli_adap(void *fd);

struct _gswss_cfg {
	char cmdname[256];
	u32 cmdid;
	u8 args;
	int (*set_func)(void *, GSW_MAC_Cli_t *);
	int (*get_func)(void *, GSW_MAC_Cli_t *);
	char help[1024];
};

u32 gsw_gswss_reg_read(void *fd, GSW_MAC_Cli_t *param)
{
	int ret = 0;
	u32 offset = param->val[0];

	param->cmdType = GSWSS_REG_RD;
	ret = cli_ioctl(fd, GSW_GSWSS_CFG, param);

	printf("\tREG offset: 0x%04x\n\tData: %08X\n", offset, param->val[0]);
	return ret;
}

int gsw_gswss_reg_write(void *fd, GSW_MAC_Cli_t *param)
{
	param->cmdType = GSWSS_REG_WR;

	cli_ioctl(fd, GSW_GSWSS_CFG, param);

	return 0;
}




int gswss_cli_reg_rd(void *fd, GSW_MAC_Cli_t *param, u32 reg)
{
	param->cmdType = GSWSS_REG_WR;
	param->val[0] = reg;

	cli_ioctl(fd, GSW_GSWSS_CFG, param);

	return param->val[0];
}

int gswss_cli_get_switch_ss_reset(void *fd, GSW_MAC_Cli_t *param)
{
	u32 core_reset, reset;

	core_reset = gswss_cli_reg_rd(fd, param, GSWIP_CFG);
	reset = mac_get_val(core_reset, GSWIP_CFG, SS_HWRES);

	printf("Switch Subsys Reset %s\n",
	       reset ? "ENABLED" : "DISABLED");

	return 0;
}

int gswss_cli_get_macsec_reset(void *fd, GSW_MAC_Cli_t *param)
{
	u32 macsec_en, reset;

	macsec_en = gswss_cli_reg_rd(fd, param, MACSEC_EN);

	reset = mac_get_val(macsec_en, MACSEC_EN, RES);

	printf("GSWSS: MACSEC reset : %s\n",
	       reset ? "ENABLED" : "DISABLED");

	return 0;
}

int gswss_cli_get_cfg1_1588(void *fd, GSW_MAC_Cli_t *param)
{
	u32 cfg0;
	u32 trig0_sel, trig1_sel, sw_trig;

	cfg0 = gswss_cli_reg_rd(fd, param, CFG1_1588);

	trig1_sel = mac_get_val(cfg0, CFG1_1588, TRIG1SEL);
	trig0_sel = mac_get_val(cfg0, CFG1_1588, TRIG0SEL);
	sw_trig = mac_get_val(cfg0, CFG1_1588, SWTRIG);

	if (trig0_sel == 0)
		printf("\tTRIG0: PON is the master\n");
	else if (trig0_sel == 1)
		printf("\tTRIG0: PCIE0 is the master\n");
	else if (trig0_sel == 2)
		printf("\tTRIG0: PCIE1 is the master\n");
	else if (trig0_sel == 3)
		printf("\tTRIG0: XGMAC2 is the master\n");
	else if (trig0_sel == 4)
		printf("\tTRIG0: XGMAC3 is the master\n");
	else if (trig0_sel == 5)
		printf("\tTRIG0: XGMAC4 is the master\n");
	else if (trig0_sel == 6)
		printf("\tTRIG0: PON100US is the master\n");
	else if (trig0_sel == 8)
		printf("\tTRIG0: EXTPPS0 is the master\n");
	else if (trig0_sel == 9)
		printf("\tTRIG0: EXTPPS1 is the master\n");
	else if (trig0_sel == 10)
		printf("\tTRIG0: Software Trigger\n");
	else
		goto end;

	if (trig1_sel == 0)
		printf("\tTRIG1: PON is the master\n");
	else if (trig1_sel == 1)
		printf("\tTRIG1: PCIE0 is the master\n");
	else if (trig1_sel == 2)
		printf("\tTRIG1: PCIE1 is the master\n");
	else if (trig1_sel == 3)
		printf("\tTRIG1: XGMAC2 is the master\n");
	else if (trig1_sel == 4)
		printf("\tTRIG1: XGMAC3 is the master\n");
	else if (trig1_sel == 5)
		printf("\tTRIG1: XGMAC4 is the master\n");
	else if (trig1_sel == 6)
		printf("\tTRIG1: PON100US is the master\n");
	else if (trig1_sel == 8)
		printf("\tTRIG1: EXTPPS0 is the master\n");
	else if (trig1_sel == 9)
		printf("\tTRIG1: EXTPPS1 is the master\n");
	else if (trig1_sel == 10)
		printf("\tTRIG1: Software Trigger\n");
	else
		goto end;

	if (sw_trig == 0)
		printf("\tSW_TRIG: 0\n");
	else if (sw_trig == 1)
		printf("\tSW_TRIG: 1\n");

end:
	return 0;
}

int gswss_cli_get_clkmode(void *fd, GSW_MAC_Cli_t *param)
{
	int clk_mode;

	param->cmdType = GSWSS_ADAP_CLK_MD;
	clk_mode = cli_ioctl(fd, GSW_GSWSS_CFG, param);

	printf("GSWSS: Clock Mode Got :");

	if (clk_mode == 0)
		printf("%s\n", "NCO1 - 666 Mhz");
	else if (clk_mode == 1)
		printf("%s\n", "NCO2 - 450 Mhz");
	else if (clk_mode == 2)
		printf("%s\n", "Auto Mode (666/450) Mhz");
	else if (clk_mode == 3)
		printf("%s\n", "Auto Mode (666/450) Mhz");

	return 0;
}

int gswss_cli_dbg_macsec_to_mac(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_idx;

	param->cmdType = GSWSS_ADAP_MACSEC_TO_MAC;
	mac_idx = cli_ioctl(fd, GSW_GSWSS_CFG, param);

	if (mac_idx == 0)
		printf("GSWSS: MACSEC Disabled\n");
	else if (mac_idx == 2)
		printf("GSWSS: MACSEC enabled to MAC %d data traffic\n",
		       mac_idx);
	else if (mac_idx == 3)
		printf("GSWSS: MACSEC enabled to MAC %d data traffic\n",
		       mac_idx);
	else if (mac_idx == 4)
		printf("GSWSS: MACSEC enabled to MAC %d data traffic\n",
		       mac_idx);

	return 0;
}

int gswss_cli_get_nco(void *fd, GSW_MAC_Cli_t *param)
{
	int nco = 0;

	param->cmdType = GSWSS_ADAP_NCO;
	nco = cli_ioctl(fd, GSW_GSWSS_CFG, param);

	printf("GSWSS: NCO Index %d NCO Got is %x\n", param->val[0], nco);

	return 0;
}

int gswss_cli_get_cfg0_1588(void *fd, GSW_MAC_Cli_t *param)
{
	u32 ref_time, dig_time, bin_time, pps_sel;

	param->cmdType = GSWSS_ADAP_CFG_1588;
	cli_ioctl(fd, GSW_GSWSS_CFG, param);

	ref_time = param->val[0];
	dig_time = param->val[1];
	bin_time = param->val[2];
	pps_sel = param->val[3];

	if (ref_time == 0)
		printf("\tREF_TIME: PON_PCS is the master\n");
	else if (ref_time == 1)
		printf("\tREF_TIME: PCIE0 is the master\n");
	else if (ref_time == 2)
		printf("\tREF_TIME: PCIE1 is the master\n");
	else if (ref_time == 3)
		printf("\tREF_TIME: XGMAC2 is the master\n");
	else if (ref_time == 4)
		printf("\tREF_TIME: XGMAC3 is the master\n");
	else if (ref_time == 5)
		printf("\tREF_TIME: XGMAC4 is the master\n");

	if (dig_time == 0)
		printf("\tDIG_TIME: PON_PCS is the master\n");
	else if (dig_time == 1)
		printf("\tDIG_TIME: PCIE0 is the master\n");
	else if (dig_time == 2)
		printf("\tDIG_TIME: PCIE1 is the master\n");
	else if (dig_time == 3)
		printf("\tDIG_TIME: XGMAC2 is the master\n");
	else if (dig_time == 4)
		printf("\tDIG_TIME: XGMAC3 is the master\n");
	else if (dig_time == 5)
		printf("\tDIG_TIME: XGMAC4 is the master\n");

	if (bin_time == 0)
		printf("\tBIN_TIME: PON_PCS is the master\n");
	else if (bin_time == 1)
		printf("\tBIN_TIME: PCIE0 is the master\n");
	else if (bin_time == 2)
		printf("\tBIN_TIME: PCIE1 is the master\n");
	else if (bin_time == 3)
		printf("\tBIN_TIME: XGMAC2 is the master\n");
	else if (bin_time == 4)
		printf("\tBIN_TIME: XGMAC3 is the master\n");
	else if (bin_time == 5)
		printf("\tBIN_TIME: XGMAC4 is the master\n");

	if (pps_sel == 0)
		printf("\tPPS_SEL: PON_PCS is the master\n");
	else if (pps_sel == 1)
		printf("\tPPS_SEL: PCIE0 is the master\n");
	else if (pps_sel == 2)
		printf("\tPPS_SEL: PCIE1 is the master\n");
	else if (pps_sel == 3)
		printf("\tPPS_SEL: XGMAC2 is the master\n");
	else if (pps_sel == 4)
		printf("\tPPS_SEL: XGMAC3 is the master\n");
	else if (pps_sel == 5)
		printf("\tPPS_SEL: XGMAC4 is the master\n");
	else if (pps_sel == 6)
		printf("\tPPS_SEL: PON PPS100US is the master\n");
	else if (pps_sel == 7)
		printf("\tPPS_SEL: Software trigger\n");

	return 0;
}

int gswss_cli_get_int_en_sts(void *fd, GSW_MAC_Cli_t *param)
{

	u32 reg_val, reg_val1;
	int ret = 0, i = 0;
	u32 mac_int_isr0, mac_int_isr1;
	u32 max_mac = 0;


	if (gswip_version == GSWIP_3_1)
		max_mac = 3;
	else if (gswip_version == GSWIP_3_2)
		max_mac = 9;

	mac_int_isr0 = gswss_cli_reg_rd(fd, param, GSWIPSS_ISR0);
	mac_int_isr1 = gswss_cli_reg_rd(fd, param, GSWIPSS_ISR1);
	reg_val = gswss_cli_reg_rd(fd, param, GSWIPSS_IER0);
	reg_val1 = gswss_cli_reg_rd(fd, param, GSWIPSS_IER1);

	printf("GSWIPSS_IER0 %08x\n", reg_val);
	printf("GSWIPSS_IER1 %08x\n", reg_val1);
	printf("GSWIPSS_ISR0 %08x\n", mac_int_isr0);
	printf("GSWIPSS_ISR1 %08x\n", mac_int_isr1);

	for (i = 0; i < max_mac; i++) {
		printf("\tXGMAC %d INT EN:  %s\n",
		       i,
		       GET_N_BITS(reg_val,
				  GSWIPSS_IER0_XGMAC2_POS + i, 1) ?
		       "ENABLED" :
		       "DISABLED");
		printf("\tXGMAC %d INT STS: %s\n",
		       i,
		       GET_N_BITS(mac_int_isr0,
				  GSWIPSS_ISR0_XGMAC2_POS + i, 1) ?
		       "ENABLED" :
		       "DISABLED");

		printf("\tLINK %d INT EN:  %s\n",
		       i,
		       GET_N_BITS(reg_val1,
				  GSWIPSS_IER1_LINK2_POS + i, 1) ?
		       "ENABLED" :
		       "DISABLED");
		printf("\tLINK %d INT STS: %s\n",
		       i,
		       GET_N_BITS(mac_int_isr1,
				  GSWIPSS_ISR1_LINK2_POS + i, 1) ?
		       "ENABLED" :
		       "DISABLED");
	}

	return ret;
}

int gswss_cli_get_phy2mode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 phy_mode;
	u8 speed_msb, speed_lsb, speed;
	u8 linkst, fdup, flow_ctrl_tx, flow_ctrl_rx;
	int i = param->mac_idx;

	phy_mode = gswss_cli_reg_rd(fd, param, PHY_MODE(param->mac_idx));
	printf("PHY_MODE_%d %x\n", i, phy_mode);
	speed_msb = GET_N_BITS(phy_mode, 15, 1);
	speed_lsb = GET_N_BITS(phy_mode, 11, 2);
	speed = (speed_msb << 2) | speed_lsb;

	if (speed == 0)
		printf("\tGSWSS: SPEED    10 Mbps\n");
	else if (speed == 1)
		printf("\tGSWSS: SPEED    100 Mbps\n");
	else if (speed == 2)
		printf("\tGSWSS: SPEED    1 Gbps\n");
	else if (speed == 3)
		printf("\tGSWSS: SPEED    10 Gbps\n");
	else if (speed == 4)
		printf("\tGSWSS: SPEED    2.5 Gbps\n");
	else if (speed == 5)
		printf("\tGSWSS: SPEED    5 Gbps\n");
	else if (speed == 6)
		printf("\tGSWSS: SPEED    RESERVED\n");
	else if (speed == 7)
		printf("\tGSWSS: SPEED    Auto Mode\n");

	linkst = GET_N_BITS(phy_mode, 13, 2);

	if (linkst == 0)
		printf("\tGSWSS: LINK STS: Auto Mode\n");
	else if (linkst == 1)
		printf("\tGSWSS: LINK STS: Forced up\n");
	else if (linkst == 2)
		printf("\tGSWSS: LINK STS: Forced down\n");
	else if (linkst == 3)
		printf("\tGSWSS: LINK STS: Reserved\n");

	fdup = GET_N_BITS(phy_mode, 9, 2);

	if (fdup == 0)
		printf("\tGSWSS: Duplex mode set: Auto Mode\n");
	else if (fdup == 1)
		printf("\tGSWSS: Duplex mode set: Full Duplex Mode\n");
	else if (fdup == 2)
		printf("\tGSWSS: Duplex mode set: Reserved\n");
	else if (fdup == 3)
		printf("\tGSWSS: Duplex mode set: Half Duplex Mode\n");

	flow_ctrl_tx = GET_N_BITS(phy_mode, 7, 2);

	if (flow_ctrl_tx == 0)
		printf("\tGSWSS: Flow Ctrl Mode TX: Auto Mode\n");
	else if (flow_ctrl_tx == 1)
		printf("\tGSWSS: Flow Ctrl Mode TX: ENABLED\n");
	else if (flow_ctrl_tx == 2)
		printf("\tGSWSS: Flow Ctrl Mode TX: Reserved\n");
	else if (flow_ctrl_tx == 3)
		printf("\tGSWSS: Flow Ctrl Mode TX: DISABLED\n");

	flow_ctrl_rx = GET_N_BITS(phy_mode, 7, 2);

	if (flow_ctrl_rx == 0)
		printf("\tGSWSS: Flow Ctrl Mode RX: Auto Mode\n");
	else if (flow_ctrl_rx == 1)
		printf("\tGSWSS: Flow Ctrl Mode RX: ENABLED\n");
	else if (flow_ctrl_rx == 2)
		printf("\tGSWSS: Flow Ctrl Mode RX: Reserved\n");
	else if (flow_ctrl_rx == 3)
		printf("\tGSWSS: Flow Ctrl Mode RX: DISABLED\n");

	return 0;
}

int gswss_cli_get_txtstamp_fifo(void *fd, GSW_MAC_Cli_t *param)
{
	struct mac_fifo_entry *f_entry = &param->f_entry;

	param->cmdType = GSWSS_MAC_TXTSTAMP_FIFO;
	cli_ioctl(fd, GSW_GSWSS_CFG, param);

	printf("GSWSS MAC%d\n", param->mac_idx);
	printf("\tTTSE: \t%s\n",
	       f_entry->ttse ? "ENABLED" : "DISABLED");
	printf("\tOSTC: \t%s\n",
	       f_entry->ostc ? "ENABLED" : "DISABLED");
	printf("\tOSTPA: \t%s\n",
	       f_entry->ostpa ? "ENABLED" : "DISABLED");

	if (f_entry->cic == 0)
		printf("\tCIC: \t"
		       "DISABLED\n");

	if (f_entry->cic == 1)
		printf("\tCIC: \tTime stamp IP Checksum update\n");

	if (f_entry->cic == 2)
		printf("\tCIC: \tTime stamp IP and "
		       "Payload Checksum update\n");

	if (f_entry->cic == 3)
		printf("\tCIC: \tTime stamp IP, Payload checksum and "
		       "Pseudo header update\n");

	printf("\tTTSL: \t%d\n", f_entry->ttsl);
	printf("\tTTSH: \t%d\n", f_entry->ttsh);

	return 0;
}

int gswss_cli_get_macop(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_op_cfg, val;
	int i = param->mac_idx;

	mac_op_cfg = gswss_cli_reg_rd(fd, param, MAC_OP_CFG(param->mac_idx));
	printf("GSWSS: MAC%d OP_CFG %d\n", i, mac_op_cfg);

	val = mac_get_val(mac_op_cfg, MAC_OP_CFG, TXFCS_INS);
	printf("TX direction FCS\n");

	if (val == MODE0)
		printf("\tPacket does not have FCS and FCS "
		       "is not inserted\n");
	else if (val == MODE1)
		printf("\tPacket does not have FCS and FCS "
		       "is inserted\n");
	else if (val == MODE2)
		printf("\tPacket has FCS and FCS is not inserted\n");
	else if (val == MODE3)
		printf("\tReserved\n");

	val = mac_get_val(mac_op_cfg, MAC_OP_CFG, RXFCS);
	printf("RX direction FCS\n");

	if (val == MODE0)
		printf("\tPacket does not have FCS and FCS "
		       "is not removed\n");
	else if (val == MODE1)
		printf("\tReserved\n");
	else if (val == MODE2)
		printf("\tPacket has FCS and FCS is not removed\n");
	else if (val == MODE3)
		printf("\tPacket has FCS and FCS is removed\n");

	val = mac_get_val(mac_op_cfg, MAC_OP_CFG, TXSPTAG);
	printf("TX Special Tag\n");

	if (val == MODE0)
		printf("\tPacket does not have Special Tag and "
		       "Special Tag is not removed\n");
	else if (val == MODE1)
		printf("\tPacket has Special Tag and "
		       "Special Tag is replaced\n");
	else if (val == MODE2)
		printf("\tPacket has Special Tag and "
		       "Special Tag is not removed\n");
	else if (val == MODE3)
		printf("\tPacket has Special Tag and "
		       "Special Tag is removed\n");

	val = mac_get_val(mac_op_cfg, MAC_OP_CFG, RXSPTAG);
	printf("RX Special Tag\n");

	if (val == MODE0)
		printf("\tPacket does not have Special Tag and "
		       "Special Tag is not inserted\n");
	else if (val == MODE1)
		printf("\tPacket does not have Special Tag and "
		       "Special Tag is inserted\n");
	else if (val == MODE2)
		printf("\tPacket has Special Tag and "
		       "Special Tag is not inserted\n");
	else if (val == MODE3)
		printf("\tReserved\n");

	val = mac_get_val(mac_op_cfg, MAC_OP_CFG, RXTIME);
	printf("RX Direction Timestamp\n");

	if (val == MODE0)
		printf("\tPacket does not have time stamp and "
		       "time stamp is not inserted.\n");
	else if (val == MODE1)
		printf("\tPacket doe not have time stamp and "
		       "time stamp is inserted\n");
	else if (val == MODE2)
		printf("\tPacket has time stamp and "
		       "time stamp is not inserted\n");
	else if (val == MODE3)
		printf("\tReserved\n");

	return 0;
}

int gswss_cli_get_mtu(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mtu = 0;

	param->cmdType = GSWSS_MAC_MTU;
	mtu = cli_ioctl(fd, GSW_GSWSS_CFG, param);

	printf("GSWSS: MAC%d MTU: %d\n", param->mac_idx, mtu);

	return 0;
}

int gswss_cli_get_macif(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_if_cfg, val;
	int i = param->mac_idx;

	mac_if_cfg = gswss_cli_reg_rd(fd, param, MAC_IF_CFG(param->mac_idx));
	printf("GSWSS: MAC%d IF_CFG %d\n", i,
	       mac_if_cfg);
	val = mac_get_val(mac_if_cfg, MAC_IF_CFG, CFG1G);
	printf("\t1G:            %s\n",
	       val ?
	       "XGMAC GMII interface mode is used" :
	       "Legacy GMAC GMII interface mode is used");
	val = mac_get_val(mac_if_cfg, MAC_IF_CFG, CFGFE);
	printf("\tFast Ethernet: %s\n",
	       val ?
	       "XGMAC GMII interface mode is used" :
	       "Legacy GMAC MII interface mode is used");
	val = mac_get_val(mac_if_cfg, MAC_IF_CFG, CFG2G5);
	printf("\t2.5G:          %s\n",
	       val ?
	       "XGMAC XGMII interface mode is used" :
	       "XGMAC GMII interface mode is used");

	return 0;
}

int gswss_cli_get_mac_reset(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_if_cfg, reset;
	int ret = 0, i = param->mac_idx;

	mac_if_cfg = gswss_cli_reg_rd(fd, param, MAC_IF_CFG(param->mac_idx));
	reset = mac_get_val(mac_if_cfg, MAC_IF_CFG, XGMAC_RES);
	printf("\tXGMAC %d Reset Bit: %s\n", i,
	       reset ? "ENABLED" : "DISABLED");
	reset = mac_get_val(mac_if_cfg, MAC_IF_CFG, LMAC_RES);
	printf("\tXGMAC %d Reset Bit: %s\n", i,
	       reset ? "ENABLED" : "DISABLED");
	reset = mac_get_val(mac_if_cfg, MAC_IF_CFG, ADAP_RES);
	printf("\tXGMAC %d Reset Bit: %s\n", i,
	       reset ? "ENABLED" : "DISABLED");

	return ret;
}

int gswss_cli_get_mac_en(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_if_cfg, enable;
	int ret = 0, i = param->mac_idx;

	mac_if_cfg = gswss_cli_reg_rd(fd, param, MAC_IF_CFG(param->mac_idx));
	enable = mac_get_val(mac_if_cfg, MAC_IF_CFG, MAC_EN);
	printf("GSWSS: MAC %d: %s\n", i, enable ? "ENABLED" : "DISABLED");

	return ret;
}

int gswss_cli_get_corese(void *fd, GSW_MAC_Cli_t *param)
{
	int ret;

	param->cmdType = GSWSS_ADAP_CORESE;
	ret = cli_ioctl(fd, GSW_GSWSS_CFG, param);
	printf("CORESE  %s\n", ret ? "ENABLED" : "DISABLED");

	return ret;
}

struct _gswss_cfg gswss_cfg[] = {
	{
		"reset",
		GSWSS_MAC_RESET,
		3,
		0,
		gswss_cli_get_mac_reset,
		" <2/3/4: MacIdx> "
		" <0/1/2/3/4 XGMAC/LMAC/ADAP> <1/0: Reset/No reset>"
	},
	{
		"macen",
		GSWSS_MAC_EN,
		2,
		0,
		gswss_cli_get_mac_en,
		" <2/3/4: MacIdx> "
		" <1/0: Enable/Disable>"
	},
	{
		"macif",
		GSWSS_MAC_IF,
		3,
		0,
		gswss_cli_get_macif,
		" <2/3/4: MacIdx> <0/1/2: 1G/FE/2.5G> "
		" <0/1/2/3 MII/GMII/XGMAC_GMII/XGMAC_XGMII>"
	},
	{
		"macop",
		GSWSS_MAC_OP,
		4,
		0,
		gswss_cli_get_macop,
		" <2/3/4: MacIdx> <0/1 RX/TX> "
		" <0/1/2 FCS/SPTAG/TIME> <0/1/2/3 Mode0/Mode1/Mode2/Mode3>"
	},
	{
		"mtu",
		GSWSS_MAC_MTU,
		2,
		0,
		gswss_cli_get_mtu,
		" <2/3/4: MacIdx> "
		" <mtu size>"
	},
	{
		"txtstamp_fifo",
		GSWSS_MAC_TXTSTAMP_FIFO,
		8,
		0,
		gswss_cli_get_txtstamp_fifo,
		" <2/3/4: MacIdx> "
		" <ttse ostc ost_avail cic sec nsec rec_id>"
	},
	{
		"w",
		GSWSS_REG_WR,
		3,
		0,
		0,
		" <reg_off> <reg_val>"
	},
	{
		"r",
		GSWSS_REG_RD,
		2,
		0,
		0,
		" <reg_off>"
	},
	{
		"int_en",
		GSWSS_ADAP_INT,
		3,
		0,
		gswss_cli_get_int_en_sts,
		" <2/3/4: MacIdx> "
		" <0/1/2/3 XGMAC/LMAC/ADAP/MACSEC> <1/0: Enable/Disable>"
	},
	{
		"macsec",
		GSWSS_ADAP_MACSEC_TO_MAC,
		2,
		0,
		gswss_cli_dbg_macsec_to_mac,
		" <2/3/4: MacIdx> "
		" <1/0: Enable/Disable>"
	},
	{
		"cfg_1588",
		GSWSS_ADAP_CFG_1588,
		4,
		0,
		gswss_cli_get_cfg0_1588,
		" <ref_time <0/1/2/3/4/5 PON/PCIE0/PCIE1/XGMAC2/XGMAC3/XGMAC4>"
		" <dig_time <0/1/2/3/4/5 PON/PCIE0/PCIE1/XGMAC2/XGMAC3/XGMAC4>"
		" <bin_time <0/1/2/3/4/5 PON/PCIE0/PCIE1/XGMAC2/XGMAC3/XGMAC4>"
		" <pps_sel <0/1/2/3/4/5/6/7 PON/PCIE0/PCIE1/XGMAC2/XGMAC3/XGMAC4/PON100US/SW>"
	},
	{
		"aux_trig",
		GSWSS_ADAP_1588_CFG1,
		3,
		0,
		gswss_cli_get_cfg1_1588,
		" <trig0_sel <0/1/2/3/4/5/6/8/9/10 PON/PCIE0/PCIE1/XGMAC2/XGMAC3/XGMAC4/PON100US/EXTPPS0/EXTPPS1/SW>"
		" <trig1_sel <0/1/2/3/4/5/6/8/9/10 PON/PCIE0/PCIE1/XGMAC2/XGMAC3/XGMAC4/PON100US/EXTPPS0/EXTPPS1/SW>"
		" <sw_trig <0/1>"
	},
	{
		"clk_md",
		GSWSS_ADAP_CLK_MD,
		1,
		0,
		gswss_cli_get_clkmode,
		" <0/1/2/3 666/450/Auto/Auto Mhz >"
	},
	{
		"nco",
		GSWSS_ADAP_NCO,
		2,
		0,
		gswss_cli_get_nco,
		" <args:2 1/2/3/4:Nco Index,  Value>",
	},
	{
		"macsec_rst",
		GSWSS_ADAP_MACSEC_RST,
		1,
		0,
		gswss_cli_get_macsec_reset,
		" <1/0: Reset/No-Reset>"
	},
	{
		"ss_rst",
		GSWSS_ADAP_SS_RST,
		0,
		0,
		gswss_cli_get_switch_ss_reset,
		" <Reset Switch Core>"
	},
	{
		"corese",
		GSWSS_ADAP_CORESE,
		1,
		0,
		gswss_cli_get_corese,
		" <1/0: enable/disable>"
	},
	{
		"phymode",
		GSWSS_MAC_PHY2MODE,
		0,
		0,
		gswss_cli_get_phy2mode,
		" <2/3/4: MacIdx>",
	},
};

static inline int mac_nstrlen(char *s)
{
	int cnt = 0;

	if (s == (void *)0)
		return 0;

	while (s[cnt])
		cnt++;

	return cnt;
}

static inline unsigned long mac_nstrtoul(const char *s, int len,
		u32 *next_idx)
{
	unsigned long acc;
	int cnt = 0;
	char c;
	int base = 0;

	acc = 0;

	c = s[cnt];

	if (s[cnt] == '0') {
		cnt++;

		if ((s[cnt] == 'x') || (s[cnt] == 'X')) {
			base = 16;
			cnt++;
		}
	}

	if (!base)
		base = 10;

	for (; cnt < len; cnt++) {
		c = s[cnt];

		if ((c >= '0') && (c <= '9'))
			c -= '0';
		else if ((c >= 'a') && (c <= 'f'))
			c -= 'a' - 10;
		else if ((c >= 'A') && (c <= 'F'))
			c -= 'A' - 10;
		else
			c = 0;

		acc *= base;
		acc += c;
	}

	(*next_idx)++;

	return acc;
}

void gswss_help(void)
{
	int i = 0;
	int num_of_elem = (sizeof(gswss_cfg) / sizeof(struct _gswss_cfg));

	printf("\n----GSWSS Commands----\n\n");

	for (i = 0; i < num_of_elem; i++) {
		if (gswss_cfg[i].help) {
#if defined(CHIPTEST) && CHIPTEST

			if (gswss_cfg[i].cmdid < GSWSS_MAX_ADAP) {
				printf("gsw gswss    %15s \t %s\n",
				       gswss_cfg[i].cmdname,
				       gswss_cfg[i].help);
			}

#else

			if (gswss_cfg[i].cmdid < GSWSS_MAX_ADAP) {
				printf("switch_cli gswss    %15s \t %s\n",
				       gswss_cfg[i].cmdname,
				       gswss_cfg[i].help);
			}

#endif
		}
	}

	printf("\n\n");

	for (i = 0; i < num_of_elem; i++) {
		if (gswss_cfg[i].help) {
#if defined(CHIPTEST) && CHIPTEST

			if (gswss_cfg[i].cmdid < GSWSS_MAX_ADAP) {
				if (gswss_cfg[i].cmdid < GSWSS_MAX_MAC) {
					if ((!strcmp(gswss_cfg[i].cmdname, "r")) ||
					    (!strcmp(gswss_cfg[i].cmdname, "w"))) {
						continue;
					} else {
						printf("gsw gswss    get %10s \t %s\n",
						       gswss_cfg[i].cmdname, "<2/3/4... Mac Idx>");
					}
				} else {
					if ((!strcmp(gswss_cfg[i].cmdname, "r")) ||
					    (!strcmp(gswss_cfg[i].cmdname, "w"))) {
						continue;
					} else {
						printf("gsw gswss    get %10s \t\n",
						       gswss_cfg[i].cmdname);
					}
				}
			}

#else

			if (gswss_cfg[i].cmdid < GSWSS_MAX_ADAP) {
				if (gswss_cfg[i].cmdid < GSWSS_MAX_MAC) {
					if ((!strcmp(gswss_cfg[i].cmdname, "r")) ||
					    (!strcmp(gswss_cfg[i].cmdname, "w"))) {
						continue;
					} else {
						printf("switch_cli gswss    get %10s \t %s\n",
						       gswss_cfg[i].cmdname, "<2/3/4... Mac Idx>");
					}

				} else {
					if ((!strcmp(gswss_cfg[i].cmdname, "r")) ||
					    (!strcmp(gswss_cfg[i].cmdname, "w"))) {
						continue;
					} else {
						if (!strcmp(gswss_cfg[i].cmdname, "nco")) {
							printf("switch_cli gswss    get %10s %s\t\n", gswss_cfg[i].cmdname, "NCO Index <1,2,3,4>");
						} else {
							printf("switch_cli gswss    get %10s \t\n",
							       gswss_cfg[i].cmdname);
						}
					}
				}
			}

#endif
		}
	}

	printf("\n");
}

int get_mac_idx(char *argv[], u32 idx, GSW_MAC_Cli_t *param)
{
	u32 max_mac = 0;

	if (gswip_version == GSWIP_3_1)
		max_mac = 3;
	else if (gswip_version == GSWIP_3_2)
		max_mac = 9;

	if ((idx < MAC_2) || (idx > ((max_mac + MAC_2) - 1))) {
		printf("Give valid mac index 2/3/4/...\n");
		return -1;
	}

	param->mac_idx = idx;

	return idx;
}

int gswss_main(u32 argc, char *argv[], void *fd)
{
	u32 start_arg = 0;
	u32 idx;
	GSW_MAC_Cli_t param;
	int i = 0, num_of_elem, found = 0, j = 0, ret = 0;

	start_arg++;
	start_arg++;

	if (argc <= 2) {
		gswss_help();
		goto end;
	}

	if (!strcmp(argv[start_arg], "-help")) {
		gswss_help();
		goto end;
	}

	memset(&param, 0x00, sizeof(param));

	if (!strcmp(argv[start_arg], "test")) {
		test_cli_adap(fd);
		goto end;
	}

	if (!strcmp(argv[start_arg], "r")) {
		start_arg++;
		found = 1;

		if ((strstr(argv[start_arg], "0x")) ||
		    (strstr(argv[start_arg], "0X")))
			printf("\n");
		else
			printf("Please give the address with "
			       "0x firmat\n");

		param.val[0] = mac_nstrtoul(argv[start_arg],
					    mac_nstrlen(argv[start_arg]),
					    &start_arg);
		gsw_gswss_reg_read(fd, &param);

		goto end;
	}

	if (!strcmp(argv[start_arg], "w")) {
		start_arg++;
		found = 1;

		if ((strstr(argv[start_arg], "0x")) ||
		    (strstr(argv[start_arg], "0X")))
			printf("\n");
		else
			printf("Please give the address with "
			       "0x format\n");

		param.val[0] = mac_nstrtoul(argv[start_arg],
					    mac_nstrlen(argv[start_arg]),
					    &start_arg);
		param.val[1] = mac_nstrtoul(argv[start_arg],
					    mac_nstrlen(argv[start_arg]),
					    &start_arg);
		gsw_gswss_reg_write(fd, &param);

		goto end;
	}

	if (!strcmp(argv[start_arg], "get")) {
		start_arg++;

		if (!argv[start_arg]) {
			gswss_help();
			goto end;
		}

		num_of_elem = (sizeof(gswss_cfg) / sizeof(struct _gswss_cfg));

		for (i = 0; i < num_of_elem; i++) {

			if (gswss_cfg[i].cmdid < GSWSS_MAX_MAC) {

				if (!strcmp(argv[start_arg], gswss_cfg[i].cmdname)) {

					start_arg++;
					found = 1;

					if (!argv[start_arg]) {
						printf("switch_cli gswss get %s <2/3/4..: MacIdx>\n", gswss_cfg[i].cmdname);
						goto end;
					}

					idx = mac_nstrtoul(argv[start_arg],
							   mac_nstrlen(argv[start_arg]),
							   &start_arg);

					if (get_mac_idx(argv, idx, &param) < 0)
						return -1;

					for (j = 0; j < gswss_cfg[i].args - 1; j++) {
						if (argv[start_arg]) {
							param.val[j] = mac_nstrtoul(argv[start_arg],
										    mac_nstrlen(argv[start_arg]),
										    &start_arg);
						}
					}

					param.cmdType = gswss_cfg[i].cmdid;
					param.get = 1;

					if (gswss_cfg[i].get_func)
						gswss_cfg[i].get_func(fd, &param);
					else
						ret = cli_ioctl(fd, GSW_GSWSS_CFG, &param);

					goto end;
				}
			}

			if (gswss_cfg[i].cmdid > GSWSS_MAX_MAC &&
			    gswss_cfg[i].cmdid < GSWSS_MAX_ADAP) {

				if (!strcmp(argv[start_arg], gswss_cfg[i].cmdname)) {
					start_arg++;
					found = 1;

					if (!strcmp(gswss_cfg[i].cmdname, "nco")) {
						if (!argv[start_arg]) {
							printf("switch_cli gswss get nco <NCO Index 1,2,3,4>\n");
							goto end;
						}
					}

					for (j = 0; j < gswss_cfg[i].args; j++) {

						if (argv[start_arg]) {
							param.val[j] = mac_nstrtoul(argv[start_arg],
										    mac_nstrlen(argv[start_arg]),
										    &start_arg);
						}


					}

					if (!strcmp(gswss_cfg[i].cmdname, "nco")) {
						if (param.val[0] < 1 || param.val[0] > 4) {
							printf("switch_cli gswss get nco <NCO Index 1,2,3,4>\n");
							goto end;
						}
					}

					param.cmdType = gswss_cfg[i].cmdid;
					param.get = 1;

					if (gswss_cfg[i].get_func)
						gswss_cfg[i].get_func(fd, &param);
					else
						ret = cli_ioctl(fd, GSW_GSWSS_CFG, &param);

					goto end;
				}
			}
		}

		if (found == 0) {
			gswss_help();
			goto end;
		}
	} else {

		num_of_elem = (sizeof(gswss_cfg) / sizeof(struct _gswss_cfg));

		if (!argv[start_arg]) {
			gswss_help();
			goto end;
		}

		for (i = 0; i < num_of_elem; i++) {

			if (gswss_cfg[i].cmdid < GSWSS_MAX_MAC) {

				if (!strcmp(argv[start_arg], gswss_cfg[i].cmdname)) {
					start_arg++;
					found = 1;

					if (!argv[start_arg]) {
						printf("switch_cli gswss %s %s\n", gswss_cfg[i].cmdname, gswss_cfg[i].help);
						goto end;
					}

					if (gswss_cfg[i].cmdid != GSWSS_REG_RD && gswss_cfg[i].cmdid != GSWSS_REG_WR) {

						idx = mac_nstrtoul(argv[start_arg],
								   mac_nstrlen(argv[start_arg]),
								   &start_arg);

						if (get_mac_idx(argv, idx, &param) < 0)
							return -1;
					} else {
						param.mac_idx = MAC_2;
					}

					for (j = 0; j < gswss_cfg[i].args - 1; j++) {

						if (!argv[start_arg]) {
							printf("switch_cli gswss %s %s\n", gswss_cfg[i].cmdname, gswss_cfg[i].help);
							goto end;
						}

						param.val[j] = mac_nstrtoul(argv[start_arg],
									    mac_nstrlen(argv[start_arg]),
									    &start_arg);
					}

					param.cmdType = gswss_cfg[i].cmdid;
					param.get = 0;

					if (gswss_cfg[i].set_func)
						gswss_cfg[i].set_func(fd, &param);
					else
						ret = cli_ioctl(fd, GSW_GSWSS_CFG, &param);

					if (gswss_cfg[i].cmdid == GSWSS_REG_RD) {
						printf("\tREG offset: 0x%04x\n\tData: %08X\n", param.val[0],
						       ret);
					}

					if (gswss_cfg[i].cmdid == GSWSS_REG_WR) {
						printf("\tREG offset: 0x%04x\n\tData: %08X\n", param.val[0], param.val[1]);
					}

					goto end;
				}
			}

			if (gswss_cfg[i].cmdid > GSWSS_MAX_MAC &&
			    gswss_cfg[i].cmdid < GSWSS_MAX_ADAP) {

				if (!strcmp(argv[start_arg], gswss_cfg[i].cmdname)) {
					start_arg++;
					found = 1;

					if (!argv[start_arg]) {
						printf("switch_cli gswss %s %s\n", gswss_cfg[i].cmdname, gswss_cfg[i].help);
						goto end;
					}


					for (j = 0; j < gswss_cfg[i].args; j++) {

						if (!argv[start_arg]) {
							printf("switch_cli gswss %s %s\n", gswss_cfg[i].cmdname, gswss_cfg[i].help);
							goto end;
						}

						param.val[j] = mac_nstrtoul(argv[start_arg],
									    mac_nstrlen(argv[start_arg]),
									    &start_arg);
					}

					param.cmdType = gswss_cfg[i].cmdid;
					param.get = 0;

					if (gswss_cfg[i].set_func)
						gswss_cfg[i].set_func(fd, &param);
					else
						ret = cli_ioctl(fd, GSW_GSWSS_CFG, &param);

					goto end;
				}
			}
		}

		if (found == 0) {
			gswss_help();
			goto end;
		}
	}

end:
	return 0;
}


int test_cli_adap(void *fd)
{
	int i = 0, j = 0, k = 0, m = 0, setget;
	int num_of_elem =
		(sizeof(gswss_cfg) / sizeof(struct _gswss_cfg));
	int argc = 0;
	char **argv;

	argv = malloc(15 * sizeof(char *));     // allocate the array to hold the pointer

	if (argv == NULL) {
		printf("%s memory allocation failed !! %d\n", __func__, __LINE__);
		return -1;
	}

	for (i = 0; i < 15; i++) {
		argv[i] = malloc(255 * sizeof(char));  // allocate each array to hold the strings

		if (argv[i] == NULL) {
			printf("%s memory allocation failed !! %d\n", __func__, __LINE__);

			while (--i >= 0)
				free(argv[i]);              // Free Previous

			free(argv);              		// Free Array
			return -1;
		}
	}

	for (setget = 0; setget < 2; setget++) {
		for (m = 0; m < 3; m++) {
			for (i = 0; i < num_of_elem; i++) {

				k = 0;

				argv[k] = "switch_cli";
				k++;
				argv[k] = "gswss";
				k++;


				for (j = 2; j < 15; j++)
					memset(argv[j], '\0', 255);

				if (gswss_cfg[i].cmdid < GSWSS_MAX_MAC) {

					if (setget == 1) {
						strcpy(argv[k], "get");
						k++;
						strcpy(argv[k], gswss_cfg[i].cmdname);
						k++;

						if (m == 0)
							strcpy(argv[k], "2");
						else if (m == 1)
							strcpy(argv[k], "3");
						else if (m == 2)
							strcpy(argv[k], "4");

						k++;

						printf("%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);
					} else {
						strcpy(argv[k], gswss_cfg[i].cmdname);
						k++;

						if (gswss_cfg[i].cmdid != GSWSS_REG_RD && gswss_cfg[i].cmdid != GSWSS_REG_WR) {
							if (m == 0)
								strcpy(argv[k], "2");
							else if (m == 1)
								strcpy(argv[k], "3");
							else if (m == 2)
								strcpy(argv[k], "4");

							k++;
						}

						printf("%s %s %s %s", argv[0], argv[1], argv[2], argv[3]);

						for (j = 0; j < gswss_cfg[i].args - 1; j++) {
							strcpy(argv[k], "1");
							printf(" %s", argv[k]);
							k++;
						}

						printf("\n");
					}
				} else if (gswss_cfg[i].cmdid > GSWSS_MAX_MAC) {

					if (setget == 1) {
						strcpy(argv[k], "get");
						k++;
						strcpy(argv[k], gswss_cfg[i].cmdname);
						k++;

						printf("%s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);
						system("sleep 1");
					} else {
						strcpy(argv[k], gswss_cfg[i].cmdname);
						k++;

						printf("%s %s %s", argv[0], argv[1], argv[2]);
						system("sleep 1");

						for (j = 0; j < gswss_cfg[i].args; j++) {
							strcpy(argv[k], "1");
							printf(" %s", argv[k]);
							k++;
						}

						printf("\n");
					}
				}

				argc = k;
				system("sleep 1");

				gswss_main(argc, argv, fd);
			}
		}
	}

	for (i = 0; i < 15; i++) {
		free(argv[i]);
	}

	free(argv);

	return 0;
}

