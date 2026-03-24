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

#include <mac_ops.h>
#include "gsw_cli_mac.h"

int test_lmac_cli(void *fd);

struct _lmac_cfg {
	char cmdname[256];
	u32 cmdType;
	u8 args;
	int (*set_func)(void *, GSW_MAC_Cli_t *);
	int (*get_func)(void *, GSW_MAC_Cli_t *);
	char help[1024];
};

u32 lmac_reg_read(void *fd, GSW_MAC_Cli_t *param)
{
	int ret = 0;
	u32 offset = param->val[0];

	param->cmdType = LMAC_REG_RD;
	ret = cli_ioctl(fd, GSW_LMAC_CFG, param);

	printf("\tREG offset: 0x%04x\n\tData: %08X\n", offset, param->val[0]);
	return ret;
}

int lmac_reg_write(void *fd, GSW_MAC_Cli_t *param)
{
	param->cmdType = LMAC_REG_WR;

	cli_ioctl(fd, GSW_LMAC_CFG, param);


	return 0;
}

u32 gsw_lmac_reg_read(void *fd, GSW_MAC_Cli_t *param, u32 reg)
{
	param->cmdType = LMAC_REG_RD;
	param->val[0] = reg;
	cli_ioctl(fd, GSW_LMAC_CFG, param);

	return param->val[0];
}

u32 gsw_lmac_reg_write(void *fd, GSW_MAC_Cli_t *param, u32 reg, u32 data)
{
	int ret = 0;

	param->cmdType = LMAC_REG_RD;
	param->val[0] = reg;
	param->val[1] = data;
	ret = cli_ioctl(fd, GSW_LMAC_CFG, param);

	return ret;
}

int lmac_cli_get_intf_mode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_ctrl0 = gsw_lmac_reg_read(fd, param, MAC_CTRL0(param->mac_idx));
	u32 val = 0;

	printf("LMAC %d INTF MODE %08x\n", param->mac_idx, mac_ctrl0);
	val = mac_get_val(mac_ctrl0, MAC_CTRL0, GMII);

	if (val == 0)
		printf("\tIntf mode set to : AUTO\n");
	else if (val == 1)
		printf("\tIntf mode set to : MII (10/100/200 Mbps)\n");
	else if (val == 2)
		printf("\tIntf mode set to : GMII (1000 Mbps)\n");
	else if (val == 3)
		printf("\tIntf mode set to : GMII_2G (2000 Mbps)\n");

	return 0;
}

int lmac_cli_get_duplex_mode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_ctrl0 = gsw_lmac_reg_read(fd, param, MAC_CTRL0(param->mac_idx));
	u32 val;

	printf("LMAC %d DUPLEX MODE %08x\n", param->mac_idx, mac_ctrl0);

	val = mac_get_val(mac_ctrl0, MAC_CTRL0, FDUP);

	if (val == 0)
		printf("\tFDUP mode set to : AUTO\n");
	else if (val == 1)
		printf("\tFDUP mode set to : Full Duplex Mode\n");
	else if (val == 2)
		printf("\tFDUP mode set to : Reserved\n");
	else if (val == 3)
		printf("\tFDUP mode set to : Half Duplex Mode\n");

	return val;
}

int lmac_cli_get_flowcon_mode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_ctrl0 = gsw_lmac_reg_read(fd, param, MAC_CTRL0(param->mac_idx));
	int val;

	printf("LMAC %d FLOWCONTROL MODE %08x\n",
	       param->mac_idx, mac_ctrl0);

	val = mac_get_val(mac_ctrl0, MAC_CTRL0, FCON);

	if (val == 0)
		printf("\tFCON mode set to : AUTO\n");
	else if (val == 1)
		printf("\tFCON mode set to : Receive only\n");
	else if (val == 2)
		printf("\tFCON mode set to : transmit only\n");
	else if (val == 3)
		printf("\tFCON mode set to : RXTX\n");
	else if (val == 4)
		printf("\tFCON mode set to : DISABLED\n");

	return val;
}

int lmac_cli_get_txfcs(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_ctrl0 = gsw_lmac_reg_read(fd, param, MAC_CTRL0(param->mac_idx));
	u32 val;

	printf("LMAC %d FCS %08x\n", param->mac_idx, mac_ctrl0);

	val = mac_get_val(mac_ctrl0, MAC_CTRL0, FCS);
	printf("\tFCS generation : %s\n", val ? "ENABLED" : "DISABLED");

	return val;
}

int lmac_cli_get_int(void *fd, GSW_MAC_Cli_t *param)
{
	u32 lmac_isr = gsw_lmac_reg_read(fd, param, LMAC_ISR);
	u32 val = 0, mac_idx = 0;

	mac_idx = param->mac_idx;
	val = GET_N_BITS(lmac_isr, mac_idx, LMAC_ISR_MAC2_WIDTH);

	return val;
}

int lmac_cli_get_ipg(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_ctrl1 = gsw_lmac_reg_read(fd, param, MAC_CTRL1(param->mac_idx));
	u32 val;

	printf("LMAC %d IPG %08x\n", param->mac_idx, mac_ctrl1);

	val = mac_get_val(mac_ctrl1, MAC_CTRL1, IPG);
	printf("\tIPG set to %d : bytes\n", val);

	return val;
}

int lmac_cli_get_preamble(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_ctrl1 = gsw_lmac_reg_read(fd, param, MAC_CTRL1(param->mac_idx));
	u32 val;

	printf("LMAC %d PREAMBLE %08x\n", param->mac_idx, mac_ctrl1);

	val = mac_get_val(mac_ctrl1, MAC_CTRL1, SHORTPRE);
	printf("\tPreamble is : %s\n", val ? "0 byte" : "7 byte");

	return val;
}

int lmac_cli_get_defermode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_ctrl1 = gsw_lmac_reg_read(fd, param, MAC_CTRL1(param->mac_idx));
	u32 val;

	printf("LMAC %d DEFERMODE %08x\n", param->mac_idx, mac_ctrl1);

	val = mac_get_val(mac_ctrl1, MAC_CTRL1, DEFERMODE);
	printf("\tCRS backpressure : %s\n",
	       val ?
	       "Enabled in Full Duplex mode" :
	       "Enabled in Half Duplex mode");

	return val;
}

int lmac_cli_get_lpi(void *fd, GSW_MAC_Cli_t *param, u32 *mode_en, u32 *lpi_waitg, u32 *lpi_waitm)
{
	u32 mac_ctrl4 = gsw_lmac_reg_read(fd, param, MAC_CTRL4(param->mac_idx));

	printf("LMAC %d LPI %08x\n", param->mac_idx, mac_ctrl4);

	*mode_en = mac_get_val(mac_ctrl4, MAC_CTRL4, LPIEN);
	printf("\tLPI Mode : %s\n", *mode_en ? "ENABLED" : "DISABLED");

	*lpi_waitm = mac_get_val(mac_ctrl4, MAC_CTRL4, WAIT);
	printf("\tLPI Wait time for 100M : %d usec\n", *lpi_waitm);

	*lpi_waitg = mac_get_val(mac_ctrl4, MAC_CTRL4, GWAIT);
	printf("\tLPI Wait time for 1G : %d usec\n", *lpi_waitg);

	return 0;
}

int lmac_cli_get_jps(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_ctrl5 = gsw_lmac_reg_read(fd, param, MAC_CTRL5(param->mac_idx));
	u32 pjps_bp, pjps_nobp;

	printf("LMAC %d JPS %08x\n", param->mac_idx, mac_ctrl5);

	pjps_bp = mac_get_val(mac_ctrl5, MAC_CTRL5, PJPS_BP);
	printf("\tProlong Jam Pattern Size during backpressure : %s\n",
	       pjps_bp ?
	       "64 bit jam pattern" :
	       "32 bit jam pattern");

	pjps_nobp = mac_get_val(mac_ctrl5, MAC_CTRL5, PJPS_NOBP);
	printf("\tProlong Jam Pattern Size during no-backpressure : %s\n",
	       pjps_nobp ?
	       "64 bit jam pattern" :
	       "32 bit jam pattern");

	return 0;
}

int lmac_cli_get_loopback(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_testen = gsw_lmac_reg_read(fd, param, MAC_TESTEN(param->mac_idx));
	u32 val;

	printf("LMAC %d Loopback: %08x\n", param->mac_idx, mac_testen);

	val = mac_get_val(mac_testen, MAC_TESTEN, LOOP);
	printf("\tLMAC: Loopback : %s\n", val ? "ENABLED" : "DISABLED");

	return val;
}

int lmac_cli_get_txer(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_testen = gsw_lmac_reg_read(fd, param, MAC_TESTEN(param->mac_idx));
	u32 val;

	printf("LMAC %d TXER %08x\n", param->mac_idx, mac_testen);

	val = mac_get_val(mac_testen, MAC_TESTEN, TXER);
	printf("\tInject transmit error : %s\n",
	       val ? "ENABLED" : "DISABLED");

	return val;
}

int lmac_cli_get_lpimonitor_mode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_testen = gsw_lmac_reg_read(fd, param, MAC_TESTEN(param->mac_idx));
	u32 val;

	printf("LMAC %d LPI MONITORING MODE %08x\n",
	       param->mac_idx, mac_testen);

	val = mac_get_val(mac_testen, MAC_TESTEN, LPITM);

	if (val == 0)
		printf("\tLPI to be monitored in time recording : TX\n");
	else if (val == 1)
		printf("\tLPI to be monitored in time recording : RX\n");
	else if (val == 2)
		printf("\tLPI to be monitored in time recording : TXRX\n");

	return val;
}

int lmac_cli_get_pauseframe_samode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_pfad = gsw_lmac_reg_read(fd, param, MAC_PFADCFG);
	u32 val;

	printf("LMAC: PAUSE FRAME SAMODE %08x\n", mac_pfad);

	val = mac_get_val(mac_pfad, MAC_PFADCFG, SAMOD);
	printf("\tPause frame use : %s\n",
	       val ?
	       "PORT specific MAC source address" :
	       "COMMON MAC source address");

	return val;
}

int lmac_cli_get_pauseframe_addr(void *fd, GSW_MAC_Cli_t *param, u8 *mac_addr)
{
	u16 mac_addr_0 = 0, mac_addr_1 = 0, mac_addr_2;

	mac_addr_0 = gsw_lmac_reg_read(fd, param, MAC_PFSA_0);
	mac_addr[1] = ((mac_addr_0 & 0xFF00) >> 8);
	mac_addr[0] = (mac_addr_0 & 0x00FF);

	mac_addr_1 = gsw_lmac_reg_read(fd, param, MAC_PFSA_1);
	mac_addr[3] = ((mac_addr_1 & 0xFF00) >> 8);
	mac_addr[2] = (mac_addr_1 & 0x00FF);

	mac_addr_2 = gsw_lmac_reg_read(fd, param, MAC_PFSA_2);
	mac_addr[5] = ((mac_addr_2 & 0xFF00) >> 8);
	mac_addr[4] = (mac_addr_2 & 0x00FF);

	return 0;
}

int lmac_cli_get_mac_pstat(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_pstat = gsw_lmac_reg_read(fd, param, MAC_PSTAT(param->mac_idx));

	printf("LMAC %d PORT STAT: %08x\n", param->mac_idx, mac_pstat);

	if (mac_get_val(mac_pstat, MAC_PSTAT, RXLPI))
		printf("\tReceive Low Power Idle Status : "
		       "LPI Low power idle state\n");
	else
		printf("\tReceive Low Power Idle Status : "
		       "Normal Power state\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, TXLPI))
		printf("\tTransmit Low Power Idle Status : "
		       "LPI Low power idle state\n");
	else
		printf("\tTransmit Low Power Idle Status : "
		       "Normal Power state\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, CRS))
		printf("\tCarrier Detected\n");
	else
		printf("\tNo Carrier Detected\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, LSTAT))
		printf("\tLink is : UP\n");
	else
		printf("\tLink is : DOWN\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, TXPAUEN))
		printf("\tLink Partner accepts Pause frames\n");
	else
		printf("\tLink Partner doesnot accepts Pause frames\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, RXPAUEN))
		printf("\tLink Partner sends Pause frames\n");
	else
		printf("\tLink Partner doesnot sends Pause frames\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, TXPAU))
		printf("\tTransmit Pause status is active\n");
	else
		printf("\tNormal transmit operation\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, RXPAU))
		printf("\tReceive Pause status is active\n");
	else
		printf("\tNormal Receive operation\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, FDUP))
		printf("\tFull duplex Mode\n");
	else
		printf("\thalf Duplex mode\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, MBIT))
		printf("\tAttached PHY runs at a data rate of "
		       "100 Mbps\n");
	else
		printf("\tAttached PHY runs at a data rate of "
		       "10 Mbps\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, GBIT))
		printf("\tAttached PHY runs at a data rate of "
		       "1000 or 2000 Mbps\n");
	else
		printf("\tAttached PHY runs at a data rate of "
		       "10 or 100 Mbps\n");

	if (mac_get_val(mac_pstat, MAC_PSTAT, PACT))
		printf("\tPHY is active and responds to MDIO accesses\n");
	else
		printf("\tPHY is inactive or not present\n");

	return 0;
}

int lmac_cli_get_mac_pisr(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_pisr = gsw_lmac_reg_read(fd, param, MAC_PISR(param->mac_idx));

	printf("LMAC %d PORT INTERRUPT STATUS: %08x\n",
	       param->mac_idx, mac_pisr);

	if (mac_get_val(mac_pisr, MAC_PISR, RXPAUSE))
		printf("\tAtleast 1 pause frame has been Received\n");

	if (mac_get_val(mac_pisr, MAC_PISR, TXPAUSE))
		printf("\tAtleast 1 pause frame has been transmitted\n");

	if (mac_get_val(mac_pisr, MAC_PISR, FCSERR))
		printf("\tFrame checksum Error Detected\n");

	if (mac_get_val(mac_pisr, MAC_PISR, LENERR))
		printf("\tLength mismatch Error Detected\n");

	if (mac_get_val(mac_pisr, MAC_PISR, TOOLONG))
		printf("\tToo Long frame Error Detected\n");

	if (mac_get_val(mac_pisr, MAC_PISR, TOOSHORT))
		printf("\tToo Short frame Error Detected\n");

	if (mac_get_val(mac_pisr, MAC_PISR, JAM))
		printf("\tJam status detected\n");

	if (mac_get_val(mac_pisr, MAC_PISR, LPION))
		printf("\tReceive low power idle mode is entered\n");

	if (mac_get_val(mac_pisr, MAC_PISR, LPIOFF))
		printf("\tReceive low power idle mode is left\n");

	if (mac_get_val(mac_pisr, MAC_PISR, TXPAUEN))
		printf("\tA change of Transmit Pause Enable Status\n");

	if (mac_get_val(mac_pisr, MAC_PISR, RXPAUEN))
		printf("\tA change of Receive Pause Enable Status\n");

	if (mac_get_val(mac_pisr, MAC_PISR, FDUP))
		printf("\tA change of half- or full-duplex mode\n");

	if (mac_get_val(mac_pisr, MAC_PISR, SPEED))
		printf("\tA change of speed mode\n");

	if (mac_get_val(mac_pisr, MAC_PISR, PACT))
		printf("\tA change of link activity\n");

	if (mac_get_val(mac_pisr, MAC_PISR, ALIGN))
		printf("\tA frame has been received which an "
		       "alignment error\n");

	if (mac_get_val(mac_pisr, MAC_PISR, PHYERR))
		printf("\tA frame has been received which has an "
		       "active rx_err signal\n");

	return 0;
}

int lmac_cli_get_rmon(void *fd, GSW_MAC_Cli_t *param)
{
	struct lmac_rmon_cnt lmac_cnt[10];
	int i = 0, max_mac = 0;
	static char buf[256] = {'\0'};

	if (param->set_all) {
		if (gswip_version == GSWIP_3_1)
			max_mac = 3;
		else if (gswip_version == GSWIP_3_2)
			max_mac = 9;

		memset((char *)buf, '\0', 256);

		param->set_all = 0;

		for (i = 0; i < max_mac; i++) {
			param->mac_idx = MAC_2 + i;

			memset(&lmac_cnt[i], 0, sizeof(struct lmac_rmon_cnt));
			memset(&param->lpstats, 0, sizeof(struct lmac_rmon_cnt));
			param->cmdType = LMAC_RMON;

			cli_ioctl(fd, GSW_LMAC_CFG, param);
			memcpy(&lmac_cnt[i], &param->lpstats, sizeof(struct lmac_rmon_cnt));
		}

		printf("\nTYPE                        ");

		for (i = 0; i < max_mac; i++)
			sprintf(buf + strlen(buf), "%18s %d\t", "LMAC", (i + MAC_2));

		printf("%s", buf);
		printf("\n");
	} else {

		memset((char *)buf, '\0', 256);
		memset(&lmac_cnt[i], 0, sizeof(struct lmac_rmon_cnt));
		memset(&param->lpstats, 0, sizeof(struct lmac_rmon_cnt));

		param->cmdType = LMAC_RMON;
		cli_ioctl(fd, GSW_LMAC_CFG, param);
		memcpy(&lmac_cnt[i], &param->lpstats, sizeof(struct lmac_rmon_cnt));

		printf("\nTYPE                        ");
		sprintf(buf + strlen(buf), "%18s %d\t", "LMAC", param->mac_idx);
		printf("%s", buf);
		printf("\n");
		max_mac = 1;
	}

	memset((char *)buf, '\0', 256);

	printf("Single Collision Cnt      = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20d\t",
			lmac_cnt[i].sing_coln_cnt);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', 256);

	printf("Multiple Collision Cnt    = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20d\t",
			lmac_cnt[i].mple_coln_cnt);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', 256);

	printf("Late Collision Cnt        = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20d\t",
			lmac_cnt[i].late_coln_cnt);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', 256);

	printf("Excess Collision Cnt      = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20d\t",
			lmac_cnt[i].excs_coln_cnt);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', 256);

	printf("Rx Pause Cnt              = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20d\t",
			lmac_cnt[i].rx_pause_cnt);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', 256);

	printf("Tx Pause Cnt              = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20d\t",
			lmac_cnt[i].tx_pause_cnt);

	printf("%s", buf);
	printf("\n");

	return 0;
}

struct _lmac_cfg lmac_cfg[] = {
	{
		"r",
		LMAC_REG_RD,
		1,
		0,
		0,
		"<arg: reg_off>"
	},
	{
		"w",
		LMAC_REG_WR,
		2,
		0,
		0,
		"<arg: reg_off, reg_val>"
	},
	{
		"rmon",
		LMAC_RMON,
		0,
		0,
		lmac_cli_get_rmon,
		"<RMON>"
	},
	{
		"clear_rmon",
		LMAC_CLR_RMON,
		0,
		0,
		0,
		"<RMON Clear>"
	},
	{
		"ifmode",
		LMAC_IF_MODE,
		1,
		0,
		lmac_cli_get_intf_mode,
		"<arg: <0/1/2/3: AUTO/MII/GMII/GMII_2G>>"
	},
	{
		"duplex",
		LMAC_DPX,
		1,
		0,
		lmac_cli_get_duplex_mode,
		"<arg: <0/1/2/3: AUTO/FULL/RES/HALF>>"
	},
	{
		"txfcs",
		LMAC_XFCS,
		1,
		0,
		lmac_cli_get_txfcs,
		"<arg: 1: ENABLED, 0: DISABLED>"
	},
	{
		"flowcont",
		LMAC_FLOWCON,
		1,
		0,
		lmac_cli_get_flowcon_mode,
		"<arg: <0/1/2/3/4: AUTO/RX/TX/RXTX/DISABLED >>"
	},
	{
		"ipg",
		LMAC_IPG,
		1,
		0,
		lmac_cli_get_ipg,
		"<arg: ipg_val>"
	},
	{
		"preamble",
		LMAC_PREAMBLE,
		1,
		0,
		lmac_cli_get_preamble,
		"<arg: 1 <0 byte>, 0 <7 byte>>"
	},
	{
		"defermode",
		LMAC_DEFERMODE,
		1,
		0,
		lmac_cli_get_defermode,
		"<arg: 1: <BP Enabled in FD Mode> 0: <BP Enabled in HD Mode>>"
	},
	{
		"jps",
		LMAC_JPS,
		2,
		0,
		lmac_cli_get_jps,
		"<arg: 1: <64 bit jam pattern in BP Mode> 0: <32 bit jam pattern in BP Mode>>"
		"<arg: 1: <64 bit jam pattern in No BP Mode> 0: <32 bit jam pattern in No BP Mode>>"
	},
	{
		"loopback",
		LMAC_LB,
		1,
		0,
		lmac_cli_get_loopback,
		"<arg: 1: ENABLED, 0: DISABLED>"
	},
	{
		"txerr",
		LMAC_TXER,
		1,
		0,
		lmac_cli_get_txer,
		"<arg: 1: ENABLED, 0: DISABLED>"
	},
	{
		"lpimonitor",
		LMAC_LPIMONIT,
		1,
		0,
		lmac_cli_get_lpimonitor_mode,
		"<arg: <0/1/2/: TX/RX/TXRX>>"
	},
	{
		"pstat",
		LMAC_PSTAT,
		0,
		0,
		lmac_cli_get_mac_pstat,
		""
	},
	{
		"pisr",
		LMAC_PISR,
		0,
		0,
		lmac_cli_get_mac_pisr,
		""
	},
	{
		"pause_sa_mode",
		LMAC_PAUSE_SA_MODE,
		1,
		0,
		lmac_cli_get_pauseframe_samode,
		"<arg: 1: Port specific SA, 0: Common MAC SA>"
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

void lmac_help(void)
{
	int i = 0;
	int num_of_elem = (sizeof(lmac_cfg) / sizeof(struct _lmac_cfg));

	printf("\n----Legacy MAC Commands----\n\n");

	for (i = 0; i < num_of_elem; i++) {
		if (!strcmp(lmac_cfg[i].cmdname, "rmon")) {
			continue;
		} else {
			printf("switch_cli lmac   <2/3/4..>        %15s             %s\n",
			       lmac_cfg[i].cmdname, lmac_cfg[i].help);
		}
	}

	printf("\n\n");

	for (i = 0; i < num_of_elem; i++) {

		if ((!strcmp(lmac_cfg[i].cmdname, "r")) ||
		    (!strcmp(lmac_cfg[i].cmdname, "w"))) {
			continue;
		} else {
			printf("switch_cli lmac   <2/3/4..>  get   %15s\n",
			       lmac_cfg[i].cmdname);
		}
	}

	printf("\n");

}

int lmac_main(u32 argc, char *argv[], void *fd)
{
	int idx = 0;
	u32 start_arg = 0;
	u32 max_mac = 0;
	GSW_MAC_Cli_t param;
	int i = 0, num_of_elem, found = 0, j = 0, ret = 0;

	memset(&param, 0x00, sizeof(param));

	start_arg++;
	start_arg++;

	if (!argv[start_arg]) {
		lmac_help();
		goto end;
	}

	if (!strcmp(argv[start_arg], "-help")) {
		found = 1;
		lmac_help();
		goto end;
	}

	if (!strcmp(argv[start_arg], "test")) {
		found = 1;
		test_lmac_cli(fd);
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
		lmac_reg_read(fd, &param);

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
		lmac_reg_write(fd, &param);

		goto end;
	}


	if (found)
		goto end;

	if (!strcmp(argv[start_arg], "*")) {
		start_arg++;
		param.set_all = 1;
	} else {
		idx = mac_nstrtoul(argv[start_arg], mac_nstrlen(argv[start_arg]),
				   &start_arg);

		if (gswip_version == GSWIP_3_1)
			max_mac = 3;
		else if (gswip_version == GSWIP_3_2)
			max_mac = 9;

		if ((idx < MAC_2) || (idx > ((max_mac + MAC_2) - 1))) {
			printf("Give valid mac index 2/3/4/...\n");
			return -1;
		}

		param.set_all = 0;

	}

	param.mac_idx = idx;

	if (!argv[start_arg]) {
		lmac_help();
		goto end;
	}

	if (!strcmp(argv[start_arg], "get")) {
		start_arg++;

		if (!argv[start_arg]) {
			lmac_help();
			goto end;
		}

		num_of_elem = (sizeof(lmac_cfg) / sizeof(struct _lmac_cfg));

		for (i = 0; i < num_of_elem; i++) {

			if (!strcmp(argv[start_arg], lmac_cfg[i].cmdname)) {
				start_arg++;
				found = 1;

				for (j = 0; j < lmac_cfg[i].args; j++) {

					if (argv[start_arg]) {
						param.val[j] = mac_nstrtoul(argv[start_arg],
									    mac_nstrlen(argv[start_arg]),
									    &start_arg);
					}
				}

				param.cmdType = lmac_cfg[i].cmdType;
				param.get = 0;

				if (lmac_cfg[i].get_func)
					lmac_cfg[i].get_func(fd, &param);
				else
					ret = cli_ioctl(fd, GSW_LMAC_CFG, &param);

				goto end;
			}
		}

		if (found == 0) {
			lmac_help();
			goto end;
		}
	} else {

		num_of_elem = (sizeof(lmac_cfg) / sizeof(struct _lmac_cfg));

		if (!argv[start_arg]) {
			lmac_help();
			goto end;
		}

		for (i = 0; i < num_of_elem; i++) {

			if (!strcmp(argv[start_arg], lmac_cfg[i].cmdname)) {
				start_arg++;
				found = 1;

				if (!argv[start_arg]) {
					printf("switch_cli lmac   <2/3/4..>        %s   %s\n", lmac_cfg[i].cmdname, lmac_cfg[i].help);
					goto end;
				}

				for (j = 0; j < lmac_cfg[i].args; j++) {

					if (!argv[start_arg]) {
						printf("switch_cli lmac   <2/3/4..>        %s   %s\n", lmac_cfg[i].cmdname, lmac_cfg[i].help);
						goto end;
					}

					param.val[j] = mac_nstrtoul(argv[start_arg],
								    mac_nstrlen(argv[start_arg]),
								    &start_arg);
				}

				param.cmdType = lmac_cfg[i].cmdType;
				param.get = 0;

				if (lmac_cfg[i].set_func)
					lmac_cfg[i].set_func(fd, &param);
				else
					ret = cli_ioctl(fd, GSW_LMAC_CFG, &param);

				if (lmac_cfg[i].cmdType == LMAC_REG_RD) {
					printf("\tREG offset: 0x%04x\n\tData: %08X\n", param.val[0],
					       ret);
				}

				if (lmac_cfg[i].cmdType == LMAC_REG_WR) {
					printf("\tREG offset: 0x%04x\n\tData: %08X\n", param.val[0], param.val[1]);
				}

				goto end;
			}
		}
	}

	if (found == 0) {
		lmac_help();
		goto end;
	}

end:
	return 0;
}


int test_lmac_cli(void *fd)
{
	int i = 0, j = 0, k = 0, m = 0, setget;
	int num_of_elem =
		(sizeof(lmac_cfg) / sizeof(struct _lmac_cfg));
	int argc = 0;
	char **argv;

	argv = malloc(10 * sizeof(char *));     // allocate the array to hold the pointer

	if (argv == NULL) {
		printf("%s memory allocation failed !! %d\n", __func__, __LINE__);
		return -1;
	}

	for (i = 0; i < 10; i++) {
		argv[i] = malloc(255 * sizeof(char));  // allocate each array to hold the strings

		if (argv[i] == NULL) {
			printf("%s memory allocation failed !! %d\n", __func__, __LINE__);

			while (--i >= 0)
				free(argv[i]);              // Free Previous

			free(argv);                     // Free Array
			return -1;
		}
	}


	for (setget = 0; setget < 2; setget++) {
		for (m = 0; m < 3; m++) {
			for (i = 0; i < num_of_elem; i++) {

				k = 0;

				argv[k] = "switch_cli";
				k++;
				argv[k] = "lmac";
				k++;


				if (m == 0)
					strcpy(argv[k], "2");
				else if (m == 1)
					strcpy(argv[k], "3");
				else if (m == 2)
					strcpy(argv[k], "4");

				k++;

				for (j = 3; j < 10; j++)
					memset(argv[j], '\0', 255);

				if (setget == 1) {
					strcpy(argv[k], "get");
					k++;
					strcpy(argv[k], lmac_cfg[i].cmdname);
					k++;



					printf("%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);
					system("sleep 1");
					lmac_main(argc, argv, fd);
				} else {
					strcpy(argv[k], lmac_cfg[i].cmdname);
					k++;

					printf("%s %s %s %s", argv[0], argv[1], argv[2], argv[3]);

					for (j = 0; j < lmac_cfg[i].args; j++) {
						strcpy(argv[k], "1");
						printf(" %s", argv[k]);
						k++;
					}

					printf("\n");
				}

				argc = k;

				lmac_main(argc, argv, fd);
			}
		}
	}

	for (i = 0; i < 10; i++) {
		free(argv[i]);
	}

	free(argv);

	return 0;
}

