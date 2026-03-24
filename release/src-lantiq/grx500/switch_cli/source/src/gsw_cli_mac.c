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
#include <adap_ops.h>

#include "gsw_cli_mac.h"

int test_cli(void *fd);

struct _xgmac_cfg {
	char name[256];
	u32 cmdType;
	u32 args;
	u32 *data1;
	u32 *data2;
	u32 *data3;
	u32 *data4;
	u32 *data5;
	u32 *data6;
	int (*set_func)(void *, GSW_MAC_Cli_t *);
	int (*get_func)(void *, GSW_MAC_Cli_t *);
	char help[1024];
};

u32 val_0 = 0, val_1 = 0, val_2 = 0, val_3 = 0, val_4 = 0, val_5 = 0;

u32 gsw_mac_reg_read(void *fd, GSW_MAC_Cli_t *param)
{
	int ret = 0;
	u32 offset = param->val[0];

	param->cmdType = MAC_CLI_REG_RD;

	ret = cli_ioctl(fd, GSW_XGMAC_CFG, param);

	printf("\tREG offset: 0x%04x\n\tData: %08X\n", offset, param->val[0]);
	return ret;
}

int gsw_mac_reg_write(void *fd, GSW_MAC_Cli_t *param)
{
	param->cmdType = MAC_CLI_REG_WR;

	cli_ioctl(fd, GSW_XGMAC_CFG, param);


	return 0;
}

u32 xgmac_reg_rd(void *fd, GSW_MAC_Cli_t *param, u32 reg)
{
	param->cmdType = MAC_CLI_REG_RD;
	param->val[0] = reg;

	cli_ioctl(fd, GSW_XGMAC_CFG, param);

	return param->val[0];
}

void xgmac_reg_wr(void *fd, GSW_MAC_Cli_t *param, u32 reg, u32 data)
{
	param->cmdType = MAC_CLI_REG_RD;
	param->val[0] = reg;
	param->val[1] = data;

	cli_ioctl(fd, GSW_XGMAC_CFG, param);
}

#define xgmac_reg_rd_bits(fd, param, reg, field)		\
	GET_N_BITS(xgmac_reg_rd(fd, param, reg),		\
		   reg##_##field##_POS,				\
		   reg##_##field##_WIDTH)

#define xgmac_reg_wr_bits(fd, param, reg, field, _val)		\
	do {							\
		u32 reg_val = xgmac_reg_rd(fd, param, reg);	\
		SET_N_BITS(reg_val,				\
			   reg##_##field##_POS,			\
			   reg##_##field##_WIDTH, (_val));	\
		xgmac_reg_wr(fd, param, reg, reg_val);		\
	} while (0)

#define xgmac_get_val(var, reg, field)				\
	GET_N_BITS((var),				\
		   reg##_##field##_POS, 		\
		   reg##_##field##_WIDTH)

#define xgmac_set_val(var, reg, field, val)			\
	SET_N_BITS((var),				\
		   reg##_##field##_POS, 		\
		   reg##_##field##_WIDTH, (val))

static int print_mdio_rd_cnt(void *fd, GSW_MAC_Cli_t *param)

{
	int clause;
	int i, phy_reg_data;
	u32 start = 0, end = 0, read_reg = 0;

	param->cmdType = MAC_CLI_GET_MDIO_CL;
	clause = cli_ioctl(fd, GSW_XGMAC_CFG, param);

	start = param->val[2];
	end = param->val[3];

	printf("OP    \tCL    \tDEVADR\tPHYID \tPHYREG\tDATA\n");
	printf("============================================\n");

	for (i = 0; i <= (end - start); i++) {

		param->cmdType = MAC_CLI_SET_MDIO_RD;
		read_reg = start + i;
		param->val[2] = read_reg;

		phy_reg_data = cli_ioctl(fd, GSW_XGMAC_CFG, param);

		printf("%s\t", "RD");
		printf("%4s\t", clause ? "CL22" : "CL45");
		printf("%6X\t", param->val[0]);
		printf("%5X\t", param->val[1]);
		printf("%6X\t", param->val[2]);
		printf("%4X\t", phy_reg_data);
		printf("\n");
	}

	return 0;
}

static int print_mdio_rd(void *fd, GSW_MAC_Cli_t *param)

{
	int clause;
	int phy_reg_data;

	param->cmdType = MAC_CLI_GET_MDIO_CL;
	clause = cli_ioctl(fd, GSW_XGMAC_CFG, param);

	printf("OP    \tCL    \tDEVADR\tPHYID \tPHYREG\tDATA\n");
	printf("============================================\n");

	param->cmdType = MAC_CLI_SET_MDIO_RD;
	phy_reg_data = cli_ioctl(fd, GSW_XGMAC_CFG, param);

	printf("%s\t", "RD");
	printf("%4s\t", clause ? "CL22" : "CL45");
	printf("%6X\t", param->val[0]);
	printf("%5X\t", param->val[1]);
	printf("%6X\t", param->val[2]);
	printf("%4X\t", phy_reg_data);
	printf("\n");

	return 0;
}

static int xgmac_cli_mdio_get_int_sts(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mdio_isr = xgmac_reg_rd(fd, param, MDIO_ISR);
	u32 mdio_ier = xgmac_reg_rd(fd, param, MDIO_IER);

	printf("XGMAC %d: MDIO Interrupt Status\n", param->mac_idx);
	printf("\tMDIO_IER interrupts  %s %08x\n",
	       mdio_ier ? "ENABLED" : "DISABLED", mdio_ier);

	if (mdio_ier & MASK(MDIO_IER, CWCOMPIE))
		printf("Continuous Write Completion Interrupt Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, SNGLCOMPIE))
		printf("Single Command Completion Interrupt Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT3ALIE))
		printf("Dev Present Sts Change Interrupt P3 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT2ALIE))
		printf("Dev Present Sts Change Interrupt P2 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT1ALIE))
		printf("Dev Present Sts Change Interrupt P1 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT0ALIE))
		printf("Dev Present Sts Change Interrupt P0 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT3LSIE))
		printf("Link Status Change Interrupt P3 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT2LSIE))
		printf("Link Status Change Interrupt P2 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT1LSIE))
		printf("Link Status Change Interrupt P1 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT0LSIE))
		printf("Link Status Change Interrupt P0 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT3CONIE))
		printf("Connect/Disconnect Event Interrupt P3 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT2CONIE))
		printf("Connect/Disconnect Event Interrupt P2 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT1CONIE))
		printf("Connect/Disconnect Event Interrupt P1 Enabled\n");

	if (mdio_ier & MASK(MDIO_IER, PRT0CONIE))
		printf("Connect/Disconnect Event Interrupt P0 Enabled\n");

	if (!mdio_isr) {
		printf("\tNo MDIO interrupt status available %08x\n",
		       mdio_isr);
	} else {
		printf("\tMDIO interrupt status available %08x\n",
		       mdio_isr);

		if (mdio_isr & MASK(MDIO_ISR, CWCOMPINT))
			printf("Continuous WR Completion Int Set\n");

		if (mdio_isr & MASK(MDIO_ISR, SNGLCOMPINT))
			printf("Single Cmd Completion Int Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT3ALINT))
			printf("Dev Present Sts Change Int P3 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT2ALINT))
			printf("Dev Present Sts Change Int P2 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT1ALINT))
			printf("Dev Present Sts Change Int P1 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT0ALINT))
			printf("Dev Present Sts Change Int P0 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT3LSINT))
			printf("Link Sts Change Int P3 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT2LSINT))
			printf("Link Sts Change Int P2 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT1LSINT))
			printf("Link Sts Change Int P1 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT0LSINT))
			printf("Link Sts Change Int P0 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT3CONINT))
			printf("Connect/Disconnect Event Int P3 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT2CONINT))
			printf("Connect/Disconnect Event Int P2 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT1CONINT))
			printf("Connect/Disconnect Event Int P1 Set\n");

		if (mdio_isr & MASK(MDIO_ISR, PRT0CONINT))
			printf("Connect/Disconnect Event Int P0 Set\n");
	}

	return 0;
}


static int xgmac_cli_get_tx_cfg(void *fd, GSW_MAC_Cli_t *param)
{
	u32 txq_en;

	printf("XGMAC %d: MTL_Q CFG\n", param->mac_idx);

	txq_en = xgmac_reg_rd_bits(fd, param, MTL_Q_TQOMR, TXQEN);
	printf("\tMTL TX Q is %s by default\n",
	       txq_en ? "ENABLED" : "DISABLED");

	return 0;
}

static int xgmac_cli_get_counters_cfg(void *fd, GSW_MAC_Cli_t *param)
{
	u32 ror = 0;

	ror = xgmac_reg_rd_bits(fd, param, MMC_CR, ROR);

	printf("XGMAC %d: RMON_CFG\n", param->mac_idx);

	if (ror)
		printf("\tMMC mode: Counters reset to zero after read\n");
	else
		printf("\tCounters will not get to Zero after a read\n");

	return 0;
}

static int xgmac_cli_get_fifo_size(void *fd, GSW_MAC_Cli_t *param)
{
	u32 rx_fifo_size, tx_fifo_size;

	printf("XGMAC %d: FIFO SIZE\n", param->mac_idx);

	rx_fifo_size = xgmac_reg_rd_bits(fd, param, MTL_Q_RQOMR, RQS);
	tx_fifo_size = xgmac_reg_rd_bits(fd, param, MTL_Q_TQOMR, TQS);

	rx_fifo_size = ((rx_fifo_size + 1) * 256);
	tx_fifo_size = ((tx_fifo_size + 1) * 256);

	printf("\tRX Q, RQS %d byte fifo per queue\n",  rx_fifo_size);
	printf("\tTX Q, TQS %d byte fifo per queue\n",  tx_fifo_size);
	return 0;
}

static int xgmac_cli_get_flow_control_threshold(void *fd, GSW_MAC_Cli_t *param)
{
	u32 fifo_size, rx_fc, rfa, rfd;

	printf("XGMAC %d: MTL FLOW Control Thresh\n", param->mac_idx);

	fifo_size = xgmac_reg_rd_bits(fd, param, MTL_Q_RQOMR, RQS);
	fifo_size = ((fifo_size + 1) * 256);
	rx_fc = xgmac_reg_rd_bits(fd, param, MTL_Q_RQOMR, EHFC);
	rfa = xgmac_reg_rd_bits(fd, param, MTL_Q_RQFCR, RFA);
	rfd = xgmac_reg_rd_bits(fd, param, MTL_Q_RQFCR, RFD);

	printf("\tRX Q, RQS %d byte fifo per queue\n", fifo_size);
	printf("\tRx Q, Flow control activte Thresh value %d\n", rfa);
	printf("\tRx Q, Flow Control deactivate Thresh value %d\n", rfd);

	if ((fifo_size >= 4096) && (rx_fc == 1))
		printf("\tFLow Control will get triggered according"
		       "to Thresh values\n");

	if (fifo_size < 4096)
		printf("\tRQS is less than 4KB, Flow control"
		       "will not get triggered\n");

	if (rx_fc == 0)
		printf("\tFlow control is disabled\n");

	return 0;
}

static int xgmac_cli_get_mtl_rx_flow_ctl(void *fd, GSW_MAC_Cli_t *param)
{
	u32 rx_fc;

	printf("XGMAC %d: MTL RX Flow Control\n", param->mac_idx);

	rx_fc = xgmac_reg_rd_bits(fd, param, MTL_Q_RQOMR, EHFC);
	printf("\tRX Flow control operation is %s\n",
	       rx_fc ? "ENABLED" : "DISABLED");
	return 0;
}

static int xgmac_cli_get_mtl_tx(void *fd, GSW_MAC_Cli_t *param)
{
	u32 tx_mode;

	printf("XGMAC %d: MTL TX mode\n", param->mac_idx);

	tx_mode = xgmac_reg_rd_bits(fd, param, MTL_Q_TQOMR, TSF);

	if (tx_mode == 1)
		printf("\tXGMAC %d TX Q Mode: Store and Forward mode\n",
		       param->mac_idx);
	else if (tx_mode == 0)
		printf("\tXGMAC %d TX Q Mode: Thresh mode\n",
		       param->mac_idx);
	else
		printf("\tXGMAC %d TX Q Mode: unknown mode\n",
		       param->mac_idx);

	u32 tx_thresh;

	printf("XGMAC %d: MTL TX Thresh\n", param->mac_idx);

	tx_thresh = xgmac_reg_rd_bits(fd, param, MTL_Q_TQOMR, TTC);

	if (tx_thresh == MTL_TX_THRESHOLD_32)
		printf("\tXGMAC %d TX Q operates in Thresh 32 bytes\n",
		       param->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_64)
		printf("\tXGMAC %d TX Q operates in Thresh 64 bytes\n",
		       param->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_96)
		printf("\tXGMAC %d TX Q operates in Thresh 96 bytes\n",
		       param->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_128)
		printf("\tXGMAC %d TX Q operates in Thresh 128 bytes\n",
		       param->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_192)
		printf("\tXGMAC %d TX Q operates in Thresh 192 bytes\n",
		       param->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_256)
		printf("\tXGMAC %d TX Q operates in Thresh 256 bytes\n",
		       param->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_384)
		printf("\tXGMAC %d TX Q operates in Thresh 384 bytes\n",
		       param->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_512)
		printf("\tXGMAC %d TX Q operates in Thresh 512 bytes\n",
		       param->mac_idx);

	return 0;

}

static int xgmac_cli_get_mtl_rx(void *fd, GSW_MAC_Cli_t *param)
{
	u32 rx_mode;

	printf("XGMAC %d: MTL RX mode\n", param->mac_idx);

	rx_mode = xgmac_reg_rd_bits(fd, param, MTL_Q_RQOMR, RSF);

	if (rx_mode == 1)
		printf("\tXGMAC %d RX Q Mode: Store and Forward mode\n",
		       param->mac_idx);
	else if (rx_mode == 0)
		printf("\tXGMAC %d RX Q Mode: Thresh mode\n",
		       param->mac_idx);
	else
		printf("\tXGMAC %d RX Q Mode: unknown mode\n",
		       param->mac_idx);

	u32 rx_thresh;

	printf("XGMAC %d: MTL RX Thresh\n", param->mac_idx);

	rx_thresh = xgmac_reg_rd_bits(fd, param, MTL_Q_RQOMR, RTC);

	if (rx_thresh == MTL_RX_THRESHOLD_32)
		printf("\tXGMAC %d RX Q operates in Thresh 32 bytes\n",
		       param->mac_idx);
	else if (rx_thresh == MTL_RX_THRESHOLD_64)
		printf("\tXGMAC %d RX Q operates in Thresh 64 bytes\n",
		       param->mac_idx);
	else if (rx_thresh == MTL_RX_THRESHOLD_96)
		printf("\tXGMAC %d RX Q operates in Thresh 96 bytes\n",
		       param->mac_idx);
	else if (rx_thresh == MTL_RX_THRESHOLD_128)
		printf("\tXGMAC %d RX Q operates in Thresh 128 bytes\n",
		       param->mac_idx);

	return 0;
}

static int xgmac_cli_get_mtl_q_alg(void *fd, GSW_MAC_Cli_t *param)
{
	u32 tx_mtl_alg, rx_mtl_alg;

	tx_mtl_alg = xgmac_reg_rd_bits(fd, param, MTL_OMR, ETSALG);

	printf("XGMAC %d: MTL_Q ALG\n", param->mac_idx);

	if (tx_mtl_alg == MTL_ETSALG_WRR)
		printf("\tTX is set to WRR Algorithm\n");
	else if (tx_mtl_alg == MTL_ETSALG_WFQ)
		printf("\tTX is set to WFQ Algorithm\n");
	else if (tx_mtl_alg == MTL_ETSALG_DWRR)
		printf("\tTX is set to DWRR Algorithm\n");

	rx_mtl_alg = xgmac_reg_rd_bits(fd, param, MTL_OMR, RAA);

	if (rx_mtl_alg == MTL_RAA_SP)
		printf("\tRX is set to Strict Priority Algorithm\n");
	else if (rx_mtl_alg == MTL_RAA_WSP)
		printf("\tRX is set to WSP Algorithm\n");

	return 0;
}

static int xgmac_cli_get_crc_settings(void *fd, GSW_MAC_Cli_t *param)
{
	u32 cst, acs, dcrcc;

	printf("XGMAC %d: CRC Stripping\n", param->mac_idx);
	cst = xgmac_reg_rd_bits(fd, param, MAC_RX_CFG, CST);
	printf("\tCRC stripping for Type packets: %s\n",
	       cst ? "ENABLED" : "DISABLED");
	acs = xgmac_reg_rd_bits(fd, param, MAC_RX_CFG, ACS);
	printf("\tAutomatic Pad or CRC Stripping: %s\n",
	       acs ? "ENABLED" : "DISABLED");
	dcrcc = xgmac_reg_rd_bits(fd, param, MAC_RX_CFG, DCRCC);
	printf("\tMAC RX do not check the CRC field in the rx pkt: %s\n",
	       dcrcc ? "ENABLED" : "DISABLED");
	return 0;
}

static int xgmac_cli_dbg_eee_status(void *fd, GSW_MAC_Cli_t *param)
{
	u32 varmac_lps;

	varmac_lps = xgmac_reg_rd(fd, param, MAC_LPI_CSR);

	printf("XGMAC %d: EEE Status\n", param->mac_idx);

	printf("\tMAC_LPI_Control_Status = %x\n", varmac_lps);

	if (varmac_lps & MAC_LPS_TLPIEN)
		printf("\tMAC Transmitter has entered the LPI state\n");

	if (varmac_lps & MAC_LPS_TLPIEX)
		printf("\tMAC Transmitter has exited the LPI state\n");

	if (varmac_lps & MAC_LPS_RLPIEN)
		printf("\tMAC Receiver has entered the LPI state\n");

	if (varmac_lps & MAC_LPS_RLPIEX)
		printf("\tMAC Receiver has exited the LPI state\n");

	return 0;
}

static int xgmac_cli_get_eee_settings(void *fd, GSW_MAC_Cli_t *param)
{
	u32 twt, lst, pls, lpitxa, lpitxen;

	twt = xgmac_reg_rd_bits(fd, param, MAC_LPI_TCR, TWT);
	lst = xgmac_reg_rd_bits(fd, param, MAC_LPI_TCR, LST);
	pls = xgmac_reg_rd_bits(fd, param, MAC_LPI_CSR, PLS);
	lpitxa = xgmac_reg_rd_bits(fd, param, MAC_LPI_CSR, LPITXA);
	lpitxen = xgmac_reg_rd_bits(fd, param, MAC_LPI_CSR, LPITXEN);

	printf("XGMAC %d: EEE Settings\n", param->mac_idx);

	printf("\tLPI LS TIMER: %d\n", lst);
	printf("\tLPI TW TIMER: %d\n", twt);
	printf("\tPhy link Status: %s\n",
	       pls ? "ENABLED" : "DISABLED");
	printf("\tLPI Transmit Automate: %s\n",
	       lpitxa ? "ENABLED" : "DISABLED");
	printf("\tLPI Transmit Enable: %s\n",
	       lpitxen ? "ENABLED" : "DISABLED");

	xgmac_cli_dbg_eee_status(fd, param);
	return 0;
}

static int xgmac_cli_get_mac_settings(void *fd, GSW_MAC_Cli_t *param)
{
	u32 te, re, ra;

	printf("XGMAC %d: MAC Settings\n", param->mac_idx);

	te = xgmac_reg_rd_bits(fd, param, MAC_TX_CFG, TE);
	printf("\tMAC TX: %s\n", te ? "ENABLED" : "DISABLED");
	re = xgmac_reg_rd_bits(fd, param, MAC_RX_CFG, RE);
	printf("\tMAC RX: %s\n", re ? "ENABLED" : "DISABLED");
	ra = xgmac_reg_rd_bits(fd, param, MAC_PKT_FR, RA);
	printf("\tMAC Filter RX ALL: %s\n", ra ? "ENABLED" : "DISABLED");
	return 0;
}

static int xgmac_cli_get_mac_rxtx_sts(void *fd, GSW_MAC_Cli_t *param)
{
	u32 rwt, tjt, wd, jd;

	printf("XGMAC %d: MAC RXTX Status\n", param->mac_idx);

	jd = xgmac_reg_rd_bits(fd, param, MAC_TX_CFG, JD);
	wd = xgmac_reg_rd_bits(fd, param, MAC_RX_CFG, WD);

	printf("\tWATCHDOG Disable       : %s\n",
	       wd ? "ENABLED" : "DISABLED");
	printf("\tJABBER TIMEOUT Disable : %s\n",
	       jd ? "ENABLED" : "DISABLED");

	rwt = xgmac_reg_rd_bits(fd, param, MAC_RXTX_STS, RWT);

	if (wd)
		printf("\tMAC : %s\n",
		       rwt ?
		       "Received packet > 16383 bytes with WD=1" :
		       "No packet recived with RWT");
	else
		printf("\tMAC : %s\n",
		       rwt ?
		       "Received packet > 2048 bytes with WD=0" :
		       "No packet recived with RWT");

	tjt = xgmac_reg_rd_bits(fd, param, MAC_RXTX_STS, TJT);

	if (jd)
		printf("\tMAC : %s\n",
		       tjt ?
		       "Transmitted packet > 16383 bytes with JD=1" :
		       "No packet transmitted with TJT");
	else
		printf("\tMAC : %s\n",
		       tjt ?
		       "Transmitted packet > 2048 bytes with JD=0" :
		       "No packet transmitted with TJT");

	return 0;
}

static int xgmac_cli_get_mtu_settings(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_rcr, je, wd, gpslce, jd, gpsl;
	int ret = 0;

	mac_rcr = xgmac_reg_rd(fd, param, MAC_RX_CFG);

	je = xgmac_get_val(mac_rcr, MAC_RX_CFG, JE);
	wd = xgmac_get_val(mac_rcr, MAC_RX_CFG, WD);
	gpslce = xgmac_get_val(mac_rcr, MAC_RX_CFG, GPSLCE);
	gpsl = xgmac_get_val(mac_rcr, MAC_RX_CFG, GPSL);
	jd = xgmac_reg_rd_bits(fd, param, MAC_TX_CFG, JD);

	param->cmdType = MAC_CLI_SET_MTU;
	ret = cli_ioctl(fd, GSW_XGMAC_CFG, param);

	printf("XGMAC %d: MTU Settings\n", param->mac_idx);

	printf("\tMTU CONFIGURED %d\n", ret);

	if (je)
		printf("\tJumbo Enabled: 1, MAC allows jumbo packets of "
		       "9,018 bytes (9,022 bytes for VLAN tagged packets)"
		       "without reporting a giant packet error\n");

	if (wd)
		printf("\tWatchdog Disable: 1,MAC disables the "
		       "watchdog timer on the receiver. The MAC can "
		       "receive packets of up to 16,383 bytes.\n");
	else
		printf("\tWatchdog Disable: 0, MAC does not allow more "
		       "than 2,048 bytes (10,240 if JE is 1) of the pkt "
		       "being received.The MAC cuts off any bytes "
		       "received after 2,048 bytes\n");

	if (gpslce) {
		printf("\tGPSLCE: 1, MAC considers the value in "
		       "GPSL field to declare a received packet "
		       "as Giant packet\n");
		printf("\tGPSL: %04x\n", gpsl);
	} else {
		printf("\tGPSLCE: 0, MAC considers a RX packet as Giant  "
		       "packet when its size is greater than 1,518 bytes"
		       "(1522 bytes for tagged packet)\n");
	}

	if (jd)
		printf("\tJabber Disable: 1, XGMAC disables the "
		       "jabber timer on the tx. Tx of up to 16,383-byte  "
		       "frames is supported.\n");
	else
		printf("\tJabber Disable: 0, XGMAC cuts off the TX "
		       "if the application sends more than 2,048 bytes "
		       "of data (10,240 bytes if JE is 1 during TX\n");

	return 0;
}

static int xgmac_cli_get_checksum_offload(void *fd, GSW_MAC_Cli_t *param)
{
	u32 co = 0;

	co = xgmac_reg_rd_bits(fd, param, MAC_RX_CFG, IPC);
	printf("XGMAC %d: Checksum Offload Settings\n", param->mac_idx);
	printf("\tChecksum Offload : %s\n", co ? "ENABLED" : "DISABLED");
	return 0;
}

static int xgmac_cli_get_mac_addr(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_addr_hi, mac_addr_lo;
	u8 mac_addr[6];

	mac_addr_hi = xgmac_reg_rd(fd, param, MAC_MACA0HR);
	mac_addr_lo = xgmac_reg_rd(fd, param, MAC_MACA0LR);

	printf("XGMAC %d: MAC ADDR\n", param->mac_idx);

	printf("\tmac_addr_hi = %08x\n", mac_addr_hi);
	printf("\tmac_addr_lo = %08x\n", mac_addr_lo);

	mac_addr[5] = ((mac_addr_hi & 0x0000FF00) >> 8);
	mac_addr[4] = (mac_addr_hi & 0x000000FF);
	mac_addr[3] = ((mac_addr_lo & 0xFF000000) >> 24);
	mac_addr[2] = ((mac_addr_lo & 0x00FF0000) >> 16);
	mac_addr[1] = ((mac_addr_lo & 0x0000FF00) >> 8);
	mac_addr[0] = (mac_addr_lo & 0x000000FF);

	printf("\tSet mac address %02x:%02x:%02x:%02x:%02x:%02x\n",
	       mac_addr[0],
	       mac_addr[1],
	       mac_addr[2],
	       mac_addr[3],
	       mac_addr[4],
	       mac_addr[5]);
	return 0;
}

static int xgmac_cli_get_mac_rx_mode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mode = 0;

	printf("XGMAC %d: MAC RX MODE\n", param->mac_idx);

	mode = xgmac_reg_rd_bits(fd, param, MAC_PKT_FR, PR);
	printf("\tPromiscous Mode    : %s\n",
	       mode ? "ENABLED" : "DISABLED");
	mode = xgmac_reg_rd_bits(fd, param, MAC_PKT_FR, PM);
	printf("\tPass All Multicast : %s\n",
	       mode ? "ENABLED" : "DISABLED");
	return 0;
}

static int xgmac_cli_get_rx_vlan_filtering_mode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 vtfe = 0;

	vtfe = xgmac_reg_rd_bits(fd, param, MAC_PKT_FR, VTFE);

	printf("XGMAC %d: RX VLAN Filtering\n", param->mac_idx);
	printf("\tRX VLAN Filtering is %s\n",
	       vtfe ? "ENABLED" : "DISABLED");
	return 0;
}

static int xgmac_cli_get_mac_speed(void *fd, GSW_MAC_Cli_t *param)
{
	u32 speed;

	speed  = xgmac_reg_rd_bits(fd, param, MAC_TX_CFG, SS);
	printf("XGMAC %d: MAC Speed\n", param->mac_idx);

	if (speed == 0)
		printf("\tXGMAC configured for XGMII 10G speed\n");
	else if (speed == 2)
		printf("\tXGMAC configured for GMII 2.5G speed\n");
	else if (speed == 3)
		printf("\tXGMAC configured for GMII 1G speed\n");

	return 0;
}

static int xgmac_cli_get_pause_frame_ctl(void *fd, GSW_MAC_Cli_t *param)
{
	u32 pfce = 0, rfe = 0, tfe = 0, pt = 0;

	pfce = xgmac_reg_rd_bits(fd, param, MAC_RX_FCR, PFCE);
	rfe = xgmac_reg_rd_bits(fd, param, MAC_RX_FCR, RFE);
	tfe = xgmac_reg_rd_bits(fd, param, MAC_TX_FCR, TFE);
	pt = xgmac_reg_rd_bits(fd, param, MAC_TX_FCR, PT);

	printf("XGMAC %d: Pause Frame Settings\n", param->mac_idx);

	printf("\tPriority based Flow control: %s\n",
	       pfce ? "ENABLED" : "DISABLED");

	if (pfce)
		printf("\tEnables TX of priority based flow Ctrl Pkts\n");
	else
		printf("\tEnables TX and RX of 802.3x Pause Ctrl Pkts\n");

	printf("\tReceive Flow control:        %s\n",
	       rfe ? "ENABLED" : "DISABLED");

	if (rfe)
		printf("\tMAC decodes the Rx Pause packets and "
		       "disables the TX for a specified Pause time\n");
	else
		printf("\tMAC doesnot decode the Pause packet\n");

	printf("\tTransmit Flow control:       %s\n",
	       tfe ? "ENABLED" : "DISABLED");

	if (tfe)
		printf("\tMAC enables Flow control operation "
		       "based on Pause frame\n");
	else
		printf("\tMAC does not transmit and Pause packets by "
		       "itself\n");

	printf("\tPause Time:                  %d\n", pt);
	return 0;
}

static int xgmac_cli_get_mac_loopback_mode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 lm;

	lm = xgmac_reg_rd_bits(fd, param, MAC_RX_CFG, LM);

	printf("XGMAC %d: MAC Loopback\n", param->mac_idx);
	printf("\tMAC Loopback mode: %s\n", lm ? "ENABLED" : "DISABLED");
	return 0;
}

static int xgmac_cli_get_tstamp_settings(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_tscr, val;

	mac_tscr = xgmac_reg_rd(fd, param, MAC_TSTAMP_CR);

	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSENA);

	printf("XGMAC %d: Timestamp Settings\n", param->mac_idx);
	printf("\tTimestamp is added for TX and RX packets: %s\n",
	       val ? "ENABLED" : "DISABLED");

	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSCFUPDT);
	printf("\tTimestamp Update type: %s\n", val ? "FINE" : "COARSE");
	printf("\tAddend present only in FINE update, "
	       "Timestamp Addend value %d\n",
	       xgmac_reg_rd(fd, param, MAC_TSTAMP_ADDNDR));
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSENALL);
	printf("\tTimestamp for All Packets Received: %s\n",
	       val ? "ENABLED" : "DISABLED");
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSCTRLSSR);
	printf("\tTimestamp Digital or Binary Rollover Control %s\n",
	       val ?
	       "TIME STAMP DIGITAL (1ns accuracy)" :
	       "BINARY ROLLOVER");
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSTRIG);
	printf("\tTSTRIG: Timestamp Interrupt trigger: %s\n",
	       val ? "ENABLED" : "DISABLED");
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSVER2ENA);
	printf("\tTSVER2ENA: PTP Pkt Processing for Ver 2 Format: %s\n",
	       val ? "ENABLED" : "DISABLED");
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSIPENA);
	printf("\tTSIPENA: MAC receiver processes the PTP packets "
	       "encapsulated directly in the Ethernet packets: %s\n",
	       val ? "ENABLED" : "DISABLED");
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSIPV6ENA);
	printf("\tTSIPV6ENA: MAC receiver processes the PTP packets "
	       "encapsulated in IPv6-UDP packets: %s\n",
	       val ? "ENABLED" : "DISABLED");
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSIPV4ENA);
	printf("\tTSIPV4ENA: MAC receiver processes the PTP packets "
	       "encapsulated in IPv4-UDP packets: %s\n",
	       val ? "ENABLED" : "DISABLED");
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSEVNTENA);
	printf("\tTSEVNTENA: Timestamp snapshot is taken only for "
	       "event msg (SYNC, Delay_Req, Pdelay_Req, or "
	       "Pdelay_Resp): %s\n",
	       val ? "ENABLED" : "DISABLED");
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, SNAPTYPSEL);
	printf("\tSNAPTYPSEL: These bits, along with TSMSTRENA "
	       "TSEVNTENA, decide the set of PTP packet types for which "
	       "snapshot needs to be taken. %s\n",
	       val ? "ENABLED" : "DISABLED");
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSMSTRENA);
	printf("\tTSMSTRENA: Snapshot is taken only for the "
	       "messages that are relevant to the master node: %s\n",
	       val ? "ENABLED" : "DISABLED");
	return 0;
}

static int xgmac_cli_get_txtstamp_cnt(void *fd, GSW_MAC_Cli_t *param)
{
	int ret = 0;

	param->cmdType = MAC_CLI_GET_TXTSTAMP_CNT;
	ret = cli_ioctl(fd, GSW_XGMAC_CFG, param);

	printf("XGMAC %d: Tx Tstamp captured count in xgmac Fifo %d\n",
	       param->mac_idx, ret);

	return 0;
}

static int xgmac_cli_get_txtstamp_pktid(void *fd, GSW_MAC_Cli_t *param)
{
	int ret = 0;

	param->cmdType = MAC_CLI_GET_TXTSTAMP_PKTID;
	ret = cli_ioctl(fd, GSW_XGMAC_CFG, param);

	printf("XGMAC %d: Tx Tstamp pkt ID %d\n",
	       param->mac_idx, ret);

	return 0;
}

static int xgmac_cli_get_tstamp_status(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_tscr, val;
	u64 time;

	mac_tscr = xgmac_reg_rd(fd, param, MAC_TSTAMP_STSR);

	printf("XGMAC %d: Timestamp Status\n", param->mac_idx);

	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, ATSNS);
	printf("\tNumber of Auxiliary Timestamp Snapshots:          %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, ATSSTM);
	printf("\tAuxiliary Timestamp Snapshot Trigger Missed:      %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, ATSSTN);
	printf("\tAuxiliary Timestamp Snapshot Trigger Identifier   %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TXTSC);
	printf("\tTX Timestamp Captured:                            %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TSTRGTERR3);
	printf("\tTimestamp Target Time Error:                      %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TSTARGT3);
	printf("\tTimestamp Target Time Reached for Time PPS3:      %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TSTRGTERR2);
	printf("\tTimestamp Target Time Error:                      %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TSTARGT2);
	printf("\tTimestamp Target Time Reached for Time PPS2:      %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TSTRGTERR1);
	printf("\tTimestamp Target Time Error:                      %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TSTARGT1);
	printf("\tTimestamp Target Time Reached for Time PPS1:      %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TSTRGTERR0);
	printf("\tTimestamp Target Time Error:                      %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TSTARGT0);
	printf("\tTimestamp Target Time Reached:                    %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, AUXTSTRIG);
	printf("\tAuxiliary Timestamp Trigger Snapshot:             %d\n",
	       val);
	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TSSOVF);
	printf("\tTimestamp Seconds Overflow                        %d\n",
	       val);

	printf("\n\n");

	param->cmdType = MAC_CLI_GET_SYSTEM_TIME;
	cli_ioctl(fd, GSW_XGMAC_CFG, param);

	time = param->time;
	printf("\t64 bit system time in nsec                      %lld\n",
	       time);

	val = xgmac_get_val(mac_tscr, MAC_TSTAMP_STSR, TTSNS);
	printf("\tTstamp captured count in xgmac Fifo:              %d\n",
	       val);

	param->cmdType = MAC_CLI_GET_TX_TSTAMP;
	cli_ioctl(fd, GSW_XGMAC_CFG, param);

	time = param->time;
	printf("\tTimestamp captured in nsec:                     %lld\n",
	       time);

	return 0;
}

static int xgmac_cli_print_system_time(void *fd, GSW_MAC_Cli_t *param)
{
	u64 nsec;
	u32 reg_sec, nanosec, sec;
	u32 min, hr, days;

	reg_sec = xgmac_reg_rd(fd, param, MAC_SYS_TIME_SEC);
	nanosec = xgmac_reg_rd(fd, param, MAC_SYS_TIME_NSEC);
	nsec = NSEC_TO_SEC * reg_sec;
	nsec += nanosec;

	if (reg_sec)
		sec = (reg_sec * (nanosec / NANOSEC_IN_ONESEC));
	else
		sec = (nanosec / NANOSEC_IN_ONESEC);

	if (sec >= 60) {
		min = sec / 60;
		sec = sec - (min * 60);
	} else {
		min = 0;
	}

	if (min >= 60) {
		hr = min / 60;
		min = min - (hr * 60);
	} else {
		hr = 0;
	}

	if (hr >= 24) {
		days = hr / 24;
		hr = hr - (days * 24);
	} else {
		days = 0;
	}

	printf("Uptime(d:h:m:s):     %02d:%02d:%02d:%02d\n",
	       days, hr, min, sec);

	printf("Sec                  %d\n", reg_sec);
	printf("NanoSec              %d\n", nanosec);
	printf("Total in nsec        %lld\n", nsec);
	return 0;
}

int gswss_reg_wr(void *fd, GSW_MAC_Cli_t *param, u32 reg, u32 data)
{
	param->cmdType = GSWSS_REG_RD;
	param->val[0] = reg;
	param->val[1] = data;

	cli_ioctl(fd, GSW_GSWSS_CFG, param);

	return 0;
}

int gswss_reg_rd(void *fd, GSW_MAC_Cli_t *param, u32 reg)
{
	param->cmdType = GSWSS_REG_WR;
	param->val[0] = reg;

	cli_ioctl(fd, GSW_GSWSS_CFG, param);

	return param->val[0];
}

static int xgmac_cli_get_txtstamp_mode(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_tscr = 0, mac_txtstamp;
	u32 snaptypesel, tsmstrena, tsevntena, tsena;

	mac_txtstamp = gswss_reg_rd(fd, param, MAC_TXTS_CIC(param->mac_idx));
	printf("TTSE:         %s\n",
	       GET_N_BITS(mac_txtstamp, 4, 1) ? "ENABLED" : "DISABLED");
	printf("OSTC:         %s\n",
	       GET_N_BITS(mac_txtstamp, 3, 1) ? "ENABLED" : "DISABLED");
	printf("OSTC_AVAIL:   %s\n",
	       GET_N_BITS(mac_txtstamp, 2, 1) ? "ENABLED" : "DISABLED");

	mac_tscr = xgmac_reg_rd(fd, param, MAC_TSTAMP_CR);

	snaptypesel = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, SNAPTYPSEL);
	tsevntena = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSEVNTENA);
	tsmstrena = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSMSTRENA);
	tsena = xgmac_get_val(mac_tscr, MAC_TSTAMP_CR, TSENA);

	printf("snaptypesel   %d\n", snaptypesel);
	printf("tsevntena     %d\n", tsevntena);
	printf("tsmstrena     %d\n", tsmstrena);
	printf("tsena         %d\n", tsena);
	return 0;
}

static int xgmac_cli_get_debug_sts(void *fd, GSW_MAC_Cli_t *param)
{
	u32 dbg_sts, dbg_ctl, fifo_sel, pktstate, byteen;

	dbg_ctl = xgmac_reg_rd(fd, param, MTL_DBG_CTL);
	dbg_sts = xgmac_reg_rd(fd, param, MTL_DBG_STS);

	fifo_sel = xgmac_get_val(dbg_sts, MTL_DBG_CTL, FIFOSEL);

	printf("XGMAC %d: Debug Status\n", param->mac_idx);

	if (xgmac_get_val(dbg_sts, MTL_DBG_STS, FIFOBUSY)) {
		printf("\tA FIFO operation is in progress in the MAC, "
		       "All other fields in this register is Invalid\n");
		return 0;
	}

	pktstate = xgmac_get_val(dbg_sts, MTL_DBG_CTL, PKTSTATE);

	if (fifo_sel == 0) {
		printf("\tTX FIFO Selected\n");

		if (pktstate == 0)
			printf("\tType of Data Read: PACKET_DATA\n");
		else if (pktstate == 1)
			printf("\tType of Data Read: CONTROL_WORD\n");
		else if (pktstate == 2)
			printf("\tType of Data Read: SOP_DATA\n");
		else if (pktstate == 3)
			printf("\tType of Data Read: EOP_DATA\n");

		printf("\tSpace Available in Tx Fifo %d\n",
		       xgmac_get_val(dbg_sts, MTL_DBG_STS, LOCR));
	} else if (fifo_sel == 3) {
		printf("\tRX FIFO Selected\n");

		if (pktstate == 0)
			printf("\tType of Data Read: PACKET_DATA\n");
		else if (pktstate == 1)
			printf("\tType of Data Read: NORMAL_STS\n");
		else if (pktstate == 2)
			printf("\tType of Data Read: LAST_STS\n");
		else if (pktstate == 3)
			printf("\tType of Data Read: EOP\n");

		printf("\tSpace Available in Rx Fifo %d\n",
		       xgmac_get_val(dbg_sts, MTL_DBG_STS, LOCR));
	}

	if (pktstate == 3 && (fifo_sel == 0 || fifo_sel == 3)) {
		byteen = xgmac_get_val(dbg_ctl, MTL_DBG_CTL, BYTEEN);
		switch (byteen) {
		case 0:
			printf("\tByte 0 is valid\n");
			break;
		case 1:
			printf("\tByte 0~1 are valid\n");
			break;
		case 2:
			printf("\tByte 0~2 are valid\n");
			break;
		default:
			printf("\tByte 0~3 are valid\n");
			break;
		}
	}

	if (xgmac_get_val(dbg_sts, MTL_DBG_STS, PKTI))
		printf("\tFull packet is available in RxFIFO\n");

	return 0;
}

static int xgmac_cli_get_mtl_int_sts(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mtl_q_isr;
	u32 val;

	mtl_q_isr = xgmac_reg_rd(fd, param, MTL_Q_ISR);
	val = xgmac_reg_rd(fd, param, MTL_Q_IER);

	printf("XGMAC %d: MTL Interrupt Status\n", param->mac_idx);

	printf("\tMTL_Q_IER interrupts %s %08x\n",
	       val ? "ENABLED" : "DISABLED", val);

	/* Tx Q Overflow Interrupt Enable */
	if (val & MASK(MTL_Q_IER, TXUIE))
		printf("\t\tTx Q Overflow Interrupt Enabled\n");

	/* Average bits per slot interrupt enable */
	if (val & MASK(MTL_Q_IER, ABPSIE))
		printf("\t\tAverage bits per slot interrupt Enabled\n");

	/* Rx Q Overflow Interrupt Enable */
	if (val & MASK(MTL_Q_IER, RXOIE))
		printf("\t\tRx Q Overflow Interrupt Enabled\n");

	if (!mtl_q_isr) {
		printf("\tNo MTL interrupt status available\n");
	} else {
		/* Tx Q Overflow Interrupt Enable */
		val = xgmac_get_val(mtl_q_isr, MTL_Q_ISR, TXUNFIS);

		if (val)
			printf("\tTransmit Queue had an Underflow "
			       "during packet transmission\n");

		// TODO: Check whether this bit is reserved since traffic class is 1
		/* Average bits per slot interrupt enable */
		val = xgmac_get_val(mtl_q_isr, MTL_Q_ISR, ABPSIS);

		if (val)
			printf("\tMAC has updated the ABS value for "
			       "Traffic Class 0\n");

		/* Rx Q Overflow Interrupt Enable */
		val = xgmac_get_val(mtl_q_isr, MTL_Q_ISR, RXOVFIS);

		if (val)
			printf("\tReceive Queue had an Overflow during "
			       "packet reception\n");
	}

	return 0;
}

static int xgmac_cli_dbg_int_sts(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_isr = 0, val;

	printf("XGMAC %d: MAC Interrupt Status\n", param->mac_idx);
	mac_isr = xgmac_reg_rd(fd, param, MAC_ISR);
	val = xgmac_reg_rd(fd, param, MAC_IER);
	printf("\tMAC_IER interrupts  %s %08x\n",
	       val ? "ENABLED" : "DISABLED", val);

	/* Enable Timestamp interrupt */
	if (val & MASK(MAC_IER, TSIE))
		printf("\t\tTimestamp Interrupt Enabled\n");

	/* Enable LPI interrupt (EEE) */
	if (val & MASK(MAC_IER, LPIIE))
		printf("\t\tLPI interrupt (EEE) Enabled\n");

	/* Enable transmit error status interrupt */
	if (val & MASK(MAC_IER, TXESIE))
		printf("\t\tTransmit error status interrupt Enabled\n");

	/* Enable Receive error status interrupt */
	if (val & MASK(MAC_IER, RXESIE))
		printf("\t\tReceive error status interrupt Enabled\n");

	/* Enable power management interrupt */
	if (val & MASK(MAC_IER, PMTIE))
		printf("\tPower Management interrupt Enabled\n");

	if (!mac_isr) {
		printf("\tNo MAC interrupt status available %d\n",
		       mac_isr);
	} else {
		printf("XGMAC %d: MAC Interrupt Status %08x\n",
		       param->mac_idx, mac_isr);
		val = xgmac_get_val(mac_isr, MAC_ISR, LSI);

		if (val)
			printf("\tMAC_INT_STS: Link Status bits change "
			       "their value\n");

		val = xgmac_get_val(mac_isr, MAC_ISR, SMI);

		if (val)
			printf("\tMAC_INT_STS: Any of the bits in the "
			       "MDIO Interrupt Status Register is set\n");

		val = xgmac_get_val(mac_isr, MAC_ISR, PMTIS);

		if (val)
			printf("\tMAC_INT_STS: A Magic packet or "
			       "Wake-on-LAN packet is received in the "
			       "power-down mode\n");

		val = xgmac_get_val(mac_isr, MAC_ISR, LPIIS);

		if (val)
			printf("\tMAC_INT_STS: It is set for any LPI "
			       "state entry or exit in the MAC Tx/Rx\n");

		val = xgmac_get_val(mac_isr, MAC_ISR, MMCRXIS);

		if (val)
			printf("\tMAC_INT_STS: Interrupt is generated "
			       "in the MMC Receive Interrupt Register\n");

		val = xgmac_get_val(mac_isr, MAC_ISR, MMCTXIS);

		if (val)
			printf("\tMAC_INT_STS: Interrupt is generated in "
			       "the MMC Transmit Interrupt Register\n");

		val = xgmac_get_val(mac_isr, MAC_ISR, TSIS);

		if (val)
			printf("\tMAC_INT_STS: Timestamp Interrupt Status"
			       "is set\n");

		val = xgmac_get_val(mac_isr, MAC_ISR, TXESIS);

		if (val)
			printf("\tMAC_INT_STS: Transmit Error, "
			       "Jabber Timeout (TJT) event occurs during "
			       "transmission\n");

		val = xgmac_get_val(mac_isr, MAC_ISR, RXESIS);

		if (val)
			printf("\tMAC_INT_STS: Receive Error, Watchdog "
			       "Timeout (WDT) event occurs during Rx.\n");

		val = xgmac_get_val(mac_isr, MAC_ISR, GPIIS);

		if (val)
			printf("\tMAC_INT_STS: GPIO Interrupt status "
			       "is set\n");

		val = xgmac_get_val(mac_isr, MAC_ISR, LS);

		if (val == 0)
			printf("\tMAC_INT_STS: Current Link Status: %s\n",
			       "LINK OK");

		if (val == 2)
			printf("\tMAC_INT_STS: Current Link Status: %s\n",
			       "Local Link Fault");

		if (val == 3)
			printf("\tMAC_INT_STS: Current Link Status: %s\n",
			       "Remote Link Fault");

		return val;
	}

	val = xgmac_cli_get_mtl_int_sts(fd, param);
	return val;
}


static int xgmac_cli_get_mtl_underflow_pkt_cnt(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mtl_q;
	u32 val;

	mtl_q = xgmac_reg_rd(fd, param, MTL_Q_TQUR);
	printf("XGMAC %d: MTL Underflow Pkt Counter %08x\n",
	       param->mac_idx, mtl_q);

	val = xgmac_get_val(mtl_q, MTL_Q_TQUR, UFCNTOVF);

	if (val)
		printf("\t\tOverflow bit of Underflow pkt counter %d\n",
		       val);

	val = xgmac_get_val(mtl_q, MTL_Q_TQUR, UFPKTCNT);

	if (val)
		printf("\t\tUnerflow packet counter %d\n", val);

	return 0;
}

static int xgmac_cli_get_mtl_missed_pkt_cnt(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mtl_q;
	u32 val;

	mtl_q = xgmac_reg_rd(fd, param, MTL_Q_RQMPOCR);
	printf("XGMAC %d: MTL Missed Overflow Pkt Counter %08x\n",
	       param->mac_idx, mtl_q);

	val = xgmac_get_val(mtl_q, MTL_Q_RQMPOCR, MISCNTOVF);

	if (val)
		printf("\t\tRx Queue Missed Packet Counter crossed the "
		       "maximum limit %d\n", val);

	val = xgmac_get_val(mtl_q, MTL_Q_RQMPOCR, MISPKTCNT);

	if (val)
		printf("\t\tNumber of packets missed by XGMAC %d\n", val);

	val = xgmac_get_val(mtl_q, MTL_Q_RQMPOCR, OVFCNTOVF);

	if (val)
		printf("\t\tRx Queue Overflow Packet Counter field "
		       "crossed the maximum limit %d\n", val);

	val = xgmac_get_val(mtl_q, MTL_Q_RQMPOCR, OVFPKTCNT);

	if (val)
		printf("\tNo: of packets discarded by the XGMAC %d\n",
		       val);

	return 0;
}

static int xgmac_cli_get_fup_fep_setting(void *fd, GSW_MAC_Cli_t *param)
{
	u32 fup, fef;

	printf("XGMAC %d: FUP/FEP Settings\n", param->mac_idx);

	fup = xgmac_reg_rd_bits(fd, param, MTL_Q_RQOMR, FUP);
	printf("\tForward Undersized Good Packets for RxQ: %s\n",
	       fup ? "ENABLE" : "DISABLE");
	fef = xgmac_reg_rd_bits(fd, param, MTL_Q_RQOMR, FEF);
	printf("\tForward Error Packets for RxQ:           %s\n",
	       fef ? "ENABLE" : "DISABLE");
	return 0;
}

static int xgmac_cli_get_ipg(void *fd, GSW_MAC_Cli_t *param)
{
	u32 ipg, ifp, speed;

	ipg = xgmac_reg_rd_bits(fd, param, MAC_TX_CFG, IPG);
	ifp = xgmac_reg_rd_bits(fd, param, MAC_TX_CFG, IFP);
	speed = xgmac_reg_rd_bits(fd, param, MAC_TX_CFG, SS);

	printf("XGMAC %d: IPG Settings\n", param->mac_idx);

	if (((speed == 3) || (speed == 2)) && (ifp == 0)) {
		printf("\tGMMI mode Minimum IPG between packets during "
		       "TX is %d bits\n", (96 - (ipg * 8)));
		return 0;
	}

	if ((speed == 0) && (ifp == 0))
		printf("\tMinimum IPG between packets during TX is %d "
		       "bits, XGMII mode No reduction possible\n", 96);

	if (ifp)
		printf("\tMinimum IPG between packets during "
		       "TX is %d bits\n", (96 - (ipg * 32)));

	return 0;
}

static int xgmac_cli_get_extcfg(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_extcfg, val;

	mac_extcfg = xgmac_reg_rd(fd, param, MAC_EXTCFG);

	val = xgmac_get_val(mac_extcfg, MAC_EXTCFG, SBDIOEN);
	printf("XGMAC %d: MAC Extended CFG SGDIOEN: %s\n",
	       param->mac_idx, val ? "ENABLED" : "DISABLED");
	return 0;
}

static int xgmac_cli_dbg_pmt(void *fd, GSW_MAC_Cli_t *param)
{
	u32 mac_pmtcsr, val, i = 0;
	u32 value[8];

	mac_pmtcsr = xgmac_reg_rd(fd, param, MAC_PMT_CSR);

	printf("XGMAC %d: PMT Settings\n", param->mac_idx);

	val = xgmac_get_val(mac_pmtcsr, MAC_PMT_CSR, MGKPKTEN);
	printf("Magic_Packet_Enable:          %s\n",
	       val ? "ENABLED" : "DISABLED");

	val = xgmac_get_val(mac_pmtcsr, MAC_PMT_CSR, RWKPKTEN);
	printf("Remote_Wakeup_Packet_Enable: %s\n",
	       val ? "ENABLED" : "DISABLED");

	val = xgmac_get_val(mac_pmtcsr, MAC_PMT_CSR, PWRDWN);
	printf("Power_Down:           %s\n",
	       val ? "ENABLED" : "DISABLED");

	val = xgmac_get_val(mac_pmtcsr, MAC_PMT_CSR, MGKPRCVD);
	printf("Magic_Packet :                %s\n",
	       val ? "Received" : "Not Received");

	val = xgmac_get_val(mac_pmtcsr, MAC_PMT_CSR, RWKPRCVD);
	printf("Remote_Wakeup_Packet :       %s\n",
	       val ? "Received" : "Not Received");

	val = xgmac_get_val(mac_pmtcsr, MAC_PMT_CSR, RWKPTR);
	printf("Remote_Wakeup_FIFO_Pointer : %d\n", val);

	val = xgmac_get_val(mac_pmtcsr, MAC_PMT_CSR, GLBLUCAST);
	printf("Global_unicast:               %d\n", val);

	for (i = 0; i < 8; i++) {
		value[i] = xgmac_reg_rd(fd, param, MAC_RWK_PFR);
		printf("Remote_Wakeup_Packet_REG[%d]:       %08x\n",
		       i, value[i]);
	}

	return 0;
}

#define BUF_SIZE 512

static int xgmac_cli_get_rmon(void *fd, GSW_MAC_Cli_t *param)
{
	u32 i = 0;
	static char buf[BUF_SIZE] = {'\0'};
	u32 max_mac = 0;
	u64 dyn_val = 0;
	struct xgmac_mmc_stats mmc_stats[10];

	if (param->set_all) {
		if (gswip_version == GSWIP_3_1)
			max_mac = 3;
		else if (gswip_version == GSWIP_3_2)
			max_mac = 9;

		memset((char *)buf, '\0', BUF_SIZE);

		param->set_all = 0;

		for (i = 0; i < max_mac; i++) {
			param->mac_idx = MAC_2 + i;

			memset(&mmc_stats[i], 0, sizeof(struct xgmac_mmc_stats));
			memset(&param->pstats, 0, sizeof(struct xgmac_mmc_stats));

			printf("XGMAC %d: Reading rmon\n", param->mac_idx);

			param->cmdType = MAC_CLI_GET_RMON;

			cli_ioctl(fd, GSW_XGMAC_CFG, param);
			memcpy(&mmc_stats[i], &param->pstats, sizeof(struct xgmac_mmc_stats));
		}

		printf("\nTYPE                        ");

		for (i = 0; i < max_mac; i++)
			sprintf(buf + strlen(buf), "%18s %d\t", "XGMAC", MAC_2 + i); /* For mac Idx is from 0, 1, 2 though from user level it is from 2,3,4 */

		printf("%s", buf);
		printf("\n");
	} else {

		memset((char *)buf, '\0', BUF_SIZE);
		memset(&param->pstats, 0, sizeof(struct xgmac_mmc_stats));

		param->cmdType = MAC_CLI_GET_RMON;

		cli_ioctl(fd, GSW_XGMAC_CFG, param);
		memcpy(&mmc_stats[0], &param->pstats, sizeof(struct xgmac_mmc_stats));

		printf("\nTYPE                        %18s %d\n", "XGMAC", param->mac_idx);
		max_mac = 1;
	}

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Rx_Packets                = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			mmc_stats[i].rxframecount_gb);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Rx_Bytes                  = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			mmc_stats[i].rxoctetcount_gb);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Rx_Byte_errors            = ");

	for (i = 0; i < max_mac; i++) {
		if (mmc_stats[i].rxoctetcount_g <= mmc_stats[i].rxoctetcount_gb)
			sprintf(buf + strlen(buf), "%20llu\t",
				(mmc_stats[i].rxoctetcount_gb -
				 mmc_stats[i].rxoctetcount_g));
		else
			sprintf(buf + strlen(buf), "%20llu\t", dyn_val);
	}

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Rx_Pauseframe             = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			mmc_stats[i].rxpauseframes);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Rx_Crc_Errors             = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			mmc_stats[i].rxcrcerror);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Rx_Fifo_Errors            = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			mmc_stats[i].rxfifooverflow);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("\n");

	printf("Tx_Packets                = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			mmc_stats[i].txframecount_gb);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Tx_Bytes                  = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			mmc_stats[i].txoctetcount_gb);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Tx_Packet_Errors          = ");

	for (i = 0; i < max_mac; i++) {
		if (mmc_stats[i].txframecount_g <= mmc_stats[i].txframecount_gb)
			sprintf(buf + strlen(buf), "%20llu\t",
				(mmc_stats[i].txframecount_gb -
				 mmc_stats[i].txframecount_g));
		else
			sprintf(buf + strlen(buf), "%20llu\t", dyn_val);
	}

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Tx_Byte_Errors            = ");

	for (i = 0; i < max_mac; i++) {
		if (mmc_stats[i].txoctetcount_g <= mmc_stats[i].txoctetcount_gb)
			sprintf(buf + strlen(buf), "%20llu\t",
				(mmc_stats[i].txoctetcount_gb -
				 mmc_stats[i].txoctetcount_g));
		else
			sprintf(buf + strlen(buf), "%20llu\t", dyn_val);
	}

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Tx_Pauseframe             = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			mmc_stats[i].txpauseframes);

	printf("%s", buf);
	printf("\n");

	memset((char *)buf, '\0', BUF_SIZE);

	printf("Tx_underflow_error        = ");

	for (i = 0; i < max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			mmc_stats[i].txunderflowerror);

	printf("%s", buf);
	printf("\n");

	return 0;
}

static int xgmac_cli_print_hw_cap(void *fd, GSW_MAC_Cli_t *param)
{
	memset(&param->hw_feat, 0, sizeof(struct xgmac_hw_features));

	param->cmdType = MAC_CLI_GET_HW_FEAT;
	cli_ioctl(fd, GSW_XGMAC_CFG, param);

	printf("========== Hardware feature register 0 ==============\n");

	if (param->hw_feat.gmii)
		printf("\t 1Gbps Supported\n");

	if (param->hw_feat.vlhash)
		printf("\t Enable Address Filter VLAN Hash Table "
		       "option is selected.\n");

	if (param->hw_feat.sma)
		printf("\t Enable Station Management Block "
		       "(MDIO Interface) option is selected.\n");

	if (param->hw_feat.rwk)
		printf("\t Enable Remote Wake-Up Packet Detection "
		       "option is selected\n");

	if (param->hw_feat.mgk)
		printf("\t Enable Magic Packet Detection option "
		       "is selected.\n");

	if (param->hw_feat.mmc)
		printf("\t Enable XGMAC Management Counter (MMC) option "
		       "is selected.\n");

	if (param->hw_feat.aoe)
		printf("\t Enable IPv4 ARP Offload option "
		       "is selected\n");

	if (param->hw_feat.ts)
		printf("\t Enable IEEE 1588 Timestamp Support option "
		       "is selected.\n");

	if (param->hw_feat.eee)
		printf("\t Enable Energy Efficient Ethernet (EEE) option "
		       "is selected.\n");

	if (param->hw_feat.tx_coe)
		printf("\t Enable Transmit TCP/IP Checksum Offload option"
		       "is selected.\n");

	if (param->hw_feat.rx_coe)
		printf("\t Enable Receive TCP/IP Checksum Check option"
		       "is selected.\n");

	if (param->hw_feat.addn_mac)
		printf("\t Number of additional MAC addresses "
		       "selected = %d\n", param->hw_feat.addn_mac);

	if (param->hw_feat.ts_src) {
		if (param->hw_feat.ts_src == 1)
			printf("\t Time Stamp time source: INTERNAL\n");
		else if (param->hw_feat.ts_src == 2)
			printf("\t Time Stamp time source: EXTERNAL\n");
		else if (param->hw_feat.ts_src == 3)
			printf("\t Time Stamp time source: "
			       "INTERNAL & EXTERNAL\n");
	} else {
		printf("\t Time Stamp time source: RESERVED\n");
	}

	if (param->hw_feat.sa_vlan_ins)
		printf("\t Enable SA and VLAN Insertion on "
		       "Tx option is selected.\n");

	if (param->hw_feat.vxn)
		printf("\t Support for VxLAN/NVGRE is selected\n");

	if (param->hw_feat.ediffc)
		printf("\t EDMA mode Separate Memory is selected "
		       "for the Descriptor Cache.\n");

	if (param->hw_feat.edma)
		printf("\t Enhanced DMA option is selected.\n");

	printf("========== Hardware feature register 1 ==============\n");

	if (param->hw_feat.rx_fifo_size)
		printf("\t Rx Fifo Size %d:%d bytes\n",
		       param->hw_feat.rx_fifo_size,
		       (1 << (param->hw_feat.rx_fifo_size + 7)));

	if (param->hw_feat.tx_fifo_size)
		printf("\t Tx Fifo Size %d:%d bytes\n",
		       param->hw_feat.tx_fifo_size,
		       (1 << (param->hw_feat.tx_fifo_size + 7)));

	if (param->hw_feat.osten)
		printf("\t One Step Timestamping Enabled\n");

	if (param->hw_feat.ptoen)
		printf("\t Enable PTP Timestamp Offload Feature is "
		       "selected\n");

	if (param->hw_feat.adv_ts_hi)
		printf("\t Add IEEE 1588 Higher Word Register option is "
		       "selected.\n");

	if (param->hw_feat.dma_width == 0)
		printf("\t Dma Width: 32\n");
	else if (param->hw_feat.dma_width == 1)
		printf("\t Dma Width: 40\n");
	else if (param->hw_feat.dma_width == 2)
		printf("\t Dma Width: 48\n");
	else if (param->hw_feat.dma_width == 3)
		printf("\t Dma Width: RESERVED\n");

	if (param->hw_feat.dcb)
		printf("\t Enable Data Center Bridging option is "
		       "selected.\n");

	if (param->hw_feat.sph)
		printf("\t Enable Split Header Structure option is "
		       "selected.\n");

	if (param->hw_feat.tso)
		printf("\t Enable TCP Segmentation Offloading for "
		       "TCP/IP Packets option is selected.\n");

	if (param->hw_feat.dma_debug)
		printf("\t Enable Debug Memory Access option is "
		       "selected.\n");

	if (param->hw_feat.rss)
		printf("\t RSS Feature Enabled\n");

	printf("\t Number of traffic classes selected: %d\n",
	       (param->hw_feat.tc_cnt + 1));

	if (param->hw_feat.hash_table_size)
		printf("\t Hash table size: %d\n",
		       (64 << (param->hw_feat.hash_table_size - 1)));
	else
		printf("\t Hash table size: No hash table selected\n");

	if (param->hw_feat.l3l4_filter_num)
		printf("\t Total number of L3 or L4 filters %d\n",
		       param->hw_feat.l3l4_filter_num);

	printf("========== Hardware feature register 2 ==============\n");
	printf("\t Number of MTL RX Q: %d\n", (param->hw_feat.rx_q_cnt + 1));

	printf("\t Number of MTL TX Q: %d\n", (param->hw_feat.tx_q_cnt + 1));

	if (param->hw_feat.rx_ch_cnt)
		printf("\t Number of DMA Receive channels: %d\n",
		       (param->hw_feat.rx_ch_cnt + 1));

	if (param->hw_feat.tx_ch_cnt)
		printf("\t Number of DMA transmit channels: %d\n",
		       (param->hw_feat.tx_ch_cnt + 1));

	if (param->hw_feat.pps_out_num)
		printf("\t Number of PPS outputs %d\n",
		       param->hw_feat.pps_out_num);

	if (param->hw_feat.aux_snap_num) {
		if (param->hw_feat.aux_snap_num > 4)
			printf("\t Number of Auxiliary Snapshot Inputs: "
			       "RESERVED\n");
		else
			printf("\t Number of Auxiliary Snapshot Inputs: "
			       "%d\n", param->hw_feat.aux_snap_num);
	} else {
		printf("\t No Auxiliary input\n");
	}

	return 0;
}

int xgmac_cli_get_pch_crc_cnt(void *fd, GSW_MAC_Cli_t *param)
{
	u32 crc_cnt;

	crc_cnt = xgmac_reg_rd(fd, param, MAC_RX_PCH_CRC_CNT);

	printf("XGMAC %d: PCH CRC Counter: %d\n",
	       param->mac_idx, crc_cnt);

	return 0;
}

static int xgmac_cli_get_all_hw_settings(void *fd, GSW_MAC_Cli_t *param)
{
	printf("\n\n\t\t\tGET ALL SETTINGS for XGMAC %d\n",
	       param->mac_idx);
	printf("\n");
	xgmac_cli_get_tx_cfg(fd, param);
	xgmac_cli_get_counters_cfg(fd, param);
	xgmac_cli_get_fifo_size(fd, param);
	xgmac_cli_get_flow_control_threshold(fd, param);
	xgmac_cli_get_mtl_rx_flow_ctl(fd, param);
	xgmac_cli_get_mtl_tx(fd, param);
	xgmac_cli_get_mtl_rx(fd, param);
	xgmac_cli_get_mtl_q_alg(fd, param);

	xgmac_cli_get_crc_settings(fd, param);

	xgmac_cli_get_eee_settings(fd, param);
	xgmac_cli_dbg_eee_status(fd, param);

	xgmac_cli_get_mac_settings(fd, param);
	xgmac_cli_get_mtu_settings(fd, param);
	xgmac_cli_get_checksum_offload(fd, param);
	xgmac_cli_get_mac_addr(fd, param);
	xgmac_cli_get_mac_rx_mode(fd, param);
	xgmac_cli_get_rx_vlan_filtering_mode(fd, param);
	xgmac_cli_get_mac_speed(fd, param);
	xgmac_cli_get_pause_frame_ctl(fd, param);
	xgmac_cli_get_mac_loopback_mode(fd, param);
	xgmac_cli_get_tstamp_settings(fd, param);
	xgmac_cli_get_debug_sts(fd, param);
	xgmac_cli_dbg_int_sts(fd, param);
	xgmac_cli_get_fup_fep_setting(fd, param);
	xgmac_cli_get_ipg(fd, param);
	xgmac_cli_dbg_pmt(fd, param);
	xgmac_cli_get_mac_rxtx_sts(fd, param);
	xgmac_cli_get_mtl_missed_pkt_cnt(fd, param);

	return 0;
}

struct _xgmac_cfg xgmac_cfg_table[] = {
	/* Name  WritePtr ReadPtr  Args,Data1,Data2,Data3,Data4	Help	*/
	{
		"reset           ",
		MAC_CLI_RESET,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		0,
		"<XGMAC Reset 1/0 Reset/No reset>"
	},
	/* MTL REGISTER SETTINGS */
	{
		"mtl_tx          ",
		MAC_CLI_MTL_TX,
		0,
		&val_0, &val_1, 0, 0, 0, 0,
		0,
		xgmac_cli_get_mtl_tx,
		"<args 2: 1/0: store_forward/threshold arg 2 0:64, 1:res, 2:96, 3:128, 4:192, 5:256, 6:384, 7:512>"
	},
	{
		"mtl_rx          ",
		MAC_CLI_MTL_RX,
		0,
		&val_0, &val_1, 0, 0, 0, 0,
		0,
		xgmac_cli_get_mtl_rx,
		"<args 2: 1/0: store_forward/threshold args 2: 0:64, 1:res, 2:96, 3:128>"
	},
	{
		"flow_ctrl_thresh",
		MAC_CLI_FCTHR,
		0,
		&val_0, &val_1, 0, 0, 0, 0,
		0,
		xgmac_cli_get_flow_control_threshold,
		"<args 2: Thresh to act Flow Ctrl, Thresh to de-act Flow Ctrl>"
	},
	{
		"ts_addend       ",
		MAC_CLI_SET_TS_ADDEND,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		0,
		"<args 1: Timestamd addend val>"
	},
	{
		"ptp_tx_mode     ",
		MAC_CLI_TXTSTAMP_MODE,
		0,
		&val_0, &val_1, &val_2, 0, 0, 0,
		0,
		xgmac_cli_get_txtstamp_mode,
		"<1/0: SnaptypeSel, 1/0: tsmstrena, 1/0: tseventena>"
	},
	{
		"hwtstamp        ",
		MAC_CLI_HW_TSTAMP,
		0,
		&val_0, &val_1, 0, 0, 0, 0,
		0,
		xgmac_cli_get_tstamp_settings,
		"<args 2: 1/0 TX timestamp ON/OFF, FILTER_TYPE: 0-14/ None/ALL/Some/V1L4EVENT/V1L4SYNC/V1L4DELAY_REQ/V2L4EVENT/V2L4SYNC/V2L4DELAYREQ/V2L2EVENT/V2L2SYNC/V2L2DELAYREQ/V2EVENT/V2SYNC/V2DELAYREQ>"
	},
	{
		"error_pkt_fwd   ",
		MAC_CLI_ERR_PKT_FWD,
		0,
		&val_0, &val_1, 0, 0, 0, 0,
		0,
		xgmac_cli_get_fup_fep_setting,
		"<args 2: <1/0 enable/disable FUP, 1/0 enable/disable FEF>"
	},
	{
		"int_en          ",
		MAC_CLI_SET_INT,
		0,
		&val_0, &val_1, 0, 0, 0, 0,
		0,
		xgmac_cli_dbg_int_sts,
		"<MTL and MAC Interrupt Enable and get status>"
	},
	{
		"mac_enable      ",
		MAC_CLI_EN,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_mac_settings,
		"<args 1: 1/0: MAC enable/disable>"
	},
	{
		"mac_addr        ",
		MAC_CLI_SET_MAC_ADDR,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_mac_addr,
		"<args 1: xx:xx:xx:xx:xx:xx>"
	},
	{
		"mac_rx_mode     ",
		MAC_CLI_SET_RX_MODE,
		0,
		&val_0, &val_1, 0, 0, 0, 0,
		0,
		xgmac_cli_get_mac_rx_mode,
		"<args 2: <1/0 enable/disable promisc, 1/0 accept/deny allmcast>"
	},
	{
		"mtu             ",
		MAC_CLI_SET_MTU,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		0,
		"<args 1: MTU value>"
	},
	{
		"pause_frame_ctrl",
		MAC_CLI_PFCTRL,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_pause_frame_ctl,
		"<args 1: 0/1/2/3/4: Auto/RX/TX/RXTX/Disabled>"
	},
	{
		"pause_filter    ",
		MAC_CLI_SET_PF_FILTER,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		0,
		"<args 1: 1/0: ENABLED / DISABLED>"
	},
	{
		"speed           ",
		MAC_CLI_SPEED,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		0,
		"<args 1: 0/1/2/3:LMAC 10/100/200/1G    4/5/6/7/8/9:XGMAC 10/100/1G/2G5/5G/10G    10: 2G5-GMII>"
	},
	{
		"duplex          ",
		MAC_CLI_SET_DPX_MODE,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		0,
		"<args 1: 0/1/2:Full/Half/Auto>"
	},
	{
		"rx_csum_offload ",
		MAC_CLI_CSUM_OFFLOAD,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_checksum_offload,
		"<args 1: 1/0: ENABLED / DISABLED>"
	},
	{
		"loopback        ",
		MAC_CLI_LB,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_mac_loopback_mode,
		"<args 1: 1/0: ENABLED / DISABLED>"
	},
	{
		"eee             ",
		MAC_CLI_EEE,
		0,
		&val_0, &val_1, &val_2, 0, 0, 0,
		0,
		xgmac_cli_get_eee_settings,
		"<args 3: 1/0: ENABLED / DISABLED,   Timer 0,   Timer 1>"
	},
	{
		"crc_strip_type  ",
		MAC_CLI_CRC_STRIP,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_crc_settings,
		"<args 1: 1/0: ENABLED / DISABLED>"
	},
	{
		"crc_strip_acs   ",
		MAC_CLI_CRC_STRIP_ACS,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_crc_settings,
		"<args 1: 1/0: ENABLED / DISABLED>"
	},
	{
		"ipg             ",
		MAC_CLI_IPG,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_ipg,
		"<args 1: IPG val 0 - 4, default val 0>,"
	},
	{
		"magic_pmt       ",
		MAC_CLI_MAGIC_PMT,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_dbg_pmt,
		"<args 1: 1/0: ENABLED / DISABLED>"
	},
	{
		"gucast_pmt      ",
		MAC_CLI_PMT_GUCAST,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_dbg_pmt,
		"<args 1: 1/0: ENABLED / DISABLED>"
	},
	{
		"extcfg          ",
		MAC_CLI_EXTCFG,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_extcfg,
		"<args 1: 1/0: ENABLED / DISABLED SBDIOEN>"
	},
	{
		"rxtx            ",
		MAC_CLI_SET_RXTXCFG,
		0,
		&val_0, &val_1, 0, 0, 0, 0,
		0,
		xgmac_cli_get_mac_rxtx_sts,
		"<args 2: 1/0 Jabber Enable/Disable 1/0 Watchdog Enable/Disable>"
	},
	{
		"linksts         ",
		MAC_CLI_SET_LINKSTS,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		0,
		"<args 1: 0/1/2 UP/DOWN/AUTO>"
	},
	{
		"lpitx           ",
		MAC_CLI_SET_LPITX,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_eee_settings,
		"<args 1: 1/0: ENABLED / DISABLED>"
	},
	{
		"mdio_cl         ",
		MAC_CLI_SET_MDIO_CL,
		0,
		&val_0, &val_1, 0, 0, 0, 0,
		0,
		0,
		"<args 2: 1/0: CL22/CL45, phy_id: 0~3 (4~31 CL45 only)>"
	},
	{
		"mdio_rd         ",
		MAC_CLI_SET_MDIO_RD,
		0,
		&val_0, &val_1, &val_2, 0, 0, 0,
		print_mdio_rd,
		0,
		"<args 3: dev_adr, phy_id, phy_reg>"
	},
	{
		"mdio_rd_cont    ",
		MAC_CLI_SET_MDIO_RD_CNT,
		0,
		&val_0, &val_1, &val_2, &val_3, 0, 0,
		print_mdio_rd_cnt,
		0,
		"<args 4: dev_adr, phy_id, phy_reg_st, phy_reg_end>"
	},
	{
		"mdio_wr         ",
		MAC_CLI_SET_MDIO_WR,
		0,
		&val_0, &val_1, &val_2, &val_3, 0, 0,
		0,
		0,
		"<args 4: dev_adr, phy_id, phy_reg, phy_data>"
	},
	{
		"mdio_int        ",
		MAC_CLI_SET_MDIO_INT,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_mdio_get_int_sts,
		"<args 1: <mdio_int>"
	},
	{
		"mdio_int        ",
		MAC_CLI_GET_MDIO_CL,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		"Get MDIO CL"
	},
	{
		"fcs_gen         ",
		MAC_CLI_SET_FCSGEN,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_crc_settings,
		"<args 1: 0/1/2 CRC_PAD_ENA/CRC_EN_PAD_DIS/CRC_PAD_DIS>"
	},
	{
		"gint             ",
		MAC_CLI_SET_GINT,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		0,
		"<args 1: 1/0: G.INT ENABLED / DISABLED>"
	},
	{
		"rx_crc             ",
		MAC_CLI_SET_RXCRC,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_crc_settings,
		"<args 1: 1/0: ENABLED / DISABLED Rx CRC check>"
	},
	{
		"fifo_add             ",
		MAC_CLI_ADD_FIFO,
		0,
		&val_0, &val_1, &val_2, &val_3, &val_4, &val_5,
		0,
		0,
		"<Add Tx Fifo>"
	},
	{
		"fifo_del             ",
		MAC_CLI_DEL_FIFO,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		0,
		"<Del Tx Fifo arg1 <rec_id>>"
	},
	{
		"ts_src_sel             ",
		MAC_CLI_SET_EXT_TS_SRC,
		0,
		&val_0, 0, 0, 0, 0, 0,
		0,
		0,
		"<REF: 0/1 - Internal/External>"
	},
	{
		"flush_tx_q      ",
		MAC_CLI_FLUSH_TX_Q,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		"flush MTL transmit Q>"
	},
	{
		"fifo             ",
		MAC_CLI_GET_FIFO,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		"<Get Tx Fifo>"
	},
	{
		"init            ",
		MAC_CLI_INIT,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		"<Initialize the module based on the Index Set>"
	},
	{
		"systime         ",
		MAC_CLI_GET_SYSTEM_TIME,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_print_system_time,
		"Print System time"
	},
	{
		"ts_enable       ",
		MAC_CLI_SET_TSTAMP_EN,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		"Tx Timestamp Enable"
	},
	{
		"ts_disable      ",
		MAC_CLI_SET_TSTAMP_DIS,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		"Disable TX and RX timestamp"
	},

	/* MAC REGISTER SETTINGS */

	{
		"pause_tx        ",
		MAC_CLI_PAUSE_TX,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		"<Initiate PAUSE packet transmit>"
	},
	{
		"miss_ovf_pkt_cnt",
		MAC_CLI_GET_MISS_PKT_CNT,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_mtl_missed_pkt_cnt,
		"<Missed Overflow packet count>"
	},
	{
		"uflow_pkt_cnt   ",
		MAC_CLI_GET_UFLOW_PKT_CNT,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_mtl_underflow_pkt_cnt,
		"<Underflow packet count>"
	},
	{
		"tstamp_sts      ",
		MAC_CLI_GET_TSTAMP_STS,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_tstamp_status,
		"<Get timestamp status>"
	},
	{
		"txtstamp_cnt      ",
		MAC_CLI_GET_TXTSTAMP_CNT,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_txtstamp_cnt,
		"<Get txtstamp count>"
	},
	{
		"txtstamp_pktid      ",
		MAC_CLI_GET_TXTSTAMP_PKTID,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_txtstamp_pktid,
		"<Get txtstamp pktid>"
	},
	/* OTHERS */
	{
		"rmon            ",
		MAC_CLI_GET_RMON,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_rmon,
		"<args 1: 1: reset 0: no reset>"
	},
	{
		"clear_rmon      ",
		MAC_CLI_CLEAR_RMON,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		0,
		"<clear rmon>"
	},
	{
		"rmon_cfg        ",
		MAC_CLI_GET_CNTR_CFG,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_counters_cfg,
		"<RMON config>"
	},
	{
		"hw_feat         ",
		MAC_CLI_GET_HW_FEAT,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_print_hw_cap,
		"<get all hw features>"
	},
	{
		"all             ",
		MAC_CLI_GET_ALL,
		0,
		0, 0, 0, 0, 0, 0,
		0,
		xgmac_cli_get_all_hw_settings,
		"<Get all HW settings>"
	},
};

void xgmac_menu(void)
{
	int i = 0;

	int num_of_elem = (sizeof(xgmac_cfg_table) / sizeof(struct _xgmac_cfg));

	printf("\n MAC SET API's\n\n");

	for (i = 0; i < num_of_elem; i++) {
		if (xgmac_cfg_table[i].data1 || xgmac_cfg_table[i].data2 ||
		    xgmac_cfg_table[i].data3 || xgmac_cfg_table[i].data4)
			printf("switch_cli xgmac    <2/3/4/* MacIdx>        %15s \t %s \n",
			       xgmac_cfg_table[i].name,
			       xgmac_cfg_table[i].help);
	}

	printf("\n MAC GET API's\n\n");

	for (i = 0; i < num_of_elem; i++) {
		if (xgmac_cfg_table[i].get_func)
			printf("switch_cli xgmac    <2/3/4/ MacIdx>   get   %15s\n",
			       xgmac_cfg_table[i].name);
	}

	printf("\n");
}

static int con_mac_adr_str(char *mac_adr_str, unsigned char *mac_adr_ptr)
{
	char *str_ptr = mac_adr_str;
	char *endptr;
	int i;
	unsigned long int val;

	if (strlen(mac_adr_str) != (12 + 5)) {
		printf("ERROR: Invalid length of mac address (xx:xx:xx:xx:xx:xx)!\n");
		return 0;
	}

	for (i = 0; i < 6; i++) {
		val = strtoul(str_ptr, &endptr, 16);

		if ((*endptr != 0) && (*endptr != ':') && (*endptr != '-'))
			return 0;

		*(mac_adr_ptr + i) = (unsigned char)(val & 0xFF);
		str_ptr = endptr + 1;
	}

	return 1;
}
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

void removeSpace(char *str)
{
	char *p1 = str, *p2 = str;

	do
		while (*p2 == ' ')
			p2++;

	while ((*p1++ = *p2++));
}

void xgmac_cli_init(void)
{
	int i = 0;
	int num_of_elem = (sizeof(xgmac_cfg_table) / sizeof(struct _xgmac_cfg));

	for (i = 0; i < num_of_elem; i++) {
		removeSpace(xgmac_cfg_table[i].name);

		xgmac_cfg_table[i].args = 4;

		if (xgmac_cfg_table[i].data1)
			xgmac_cfg_table[i].args += 1;

		if (xgmac_cfg_table[i].data2)
			xgmac_cfg_table[i].args += 1;

		if (xgmac_cfg_table[i].data3)
			xgmac_cfg_table[i].args += 1;

		if (xgmac_cfg_table[i].data4)
			xgmac_cfg_table[i].args += 1;

		if (xgmac_cfg_table[i].data5)
			xgmac_cfg_table[i].args += 1;

		if (xgmac_cfg_table[i].data6)
			xgmac_cfg_table[i].args += 1;
	}
}

void set_pdata(char *arg, u32 *data, u32 *start_arg)
{
	u32 val =  mac_nstrtoul(arg, mac_nstrlen(arg), start_arg);
	*data = val;
}

void set_data(char *argv[], int i, u32 *start_arg)
{
	if (xgmac_cfg_table[i].data1)
		set_pdata(argv[*start_arg], xgmac_cfg_table[i].data1, start_arg);

	if (xgmac_cfg_table[i].data2)
		set_pdata(argv[*start_arg], xgmac_cfg_table[i].data2, start_arg);

	if (xgmac_cfg_table[i].data3)
		set_pdata(argv[*start_arg], xgmac_cfg_table[i].data3, start_arg);

	if (xgmac_cfg_table[i].data4)
		set_pdata(argv[*start_arg], xgmac_cfg_table[i].data4, start_arg);

	if (xgmac_cfg_table[i].data5)
		set_pdata(argv[*start_arg], xgmac_cfg_table[i].data5, start_arg);

	if (xgmac_cfg_table[i].data6)
		set_pdata(argv[*start_arg], xgmac_cfg_table[i].data6, start_arg);
}

int xgmac_main(u32 argc, char *argv[], void *fd)
{
	u32 i = 0, j = 0, found = 0;
	u32 start_arg = 0;
	int idx = 0;
	u32 max_mac = 3;
	u32 nanosec;
	u32 sec, min, hr, days;
	int num_of_elem =
		(sizeof(xgmac_cfg_table) / sizeof(struct _xgmac_cfg));
	GSW_MAC_Cli_t param;

	memset(&param, 0x00, sizeof(param));

	xgmac_cli_init();

	start_arg++;
	start_arg++;

	if (!argv[start_arg]) {
		xgmac_menu();
		goto end;
	}

	if (!strcmp(argv[start_arg], "-help")) {
		found = 1;
		xgmac_menu();
		goto end;
	}

	if (!strcmp(argv[start_arg], "test")) {
		found = 1;
		test_cli(fd);
		goto end;
	}

	if (found)
		goto end;

	if (!strcmp(argv[start_arg], "*")) {
		start_arg++;
		param.set_all = 1;
	} else {
		idx = mac_nstrtoul(argv[start_arg],
				   mac_nstrlen(argv[start_arg]), &start_arg);

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
		xgmac_menu();
		goto end;
	}

	if (!strcmp(argv[start_arg], "uptime")) {
		found = 1;

		param.val[0] = 0x0d08;
		sec = gsw_mac_reg_read(fd, &param);
		param.val[0] = 0x0d0c;
		nanosec = gsw_mac_reg_read(fd, &param);

		if (sec >= 60) {
			min = sec / 60;
			sec = sec - (min * 60);
		} else {
			min = 0;
		}

		if (min >= 60) {
			hr = min / 60;
			min = min - (hr * 60);
		} else {
			hr = 0;
		}

		if (hr >= 24) {
			days = hr / 24;
			hr = hr - (days * 24);
		} else {
			days = 0;
		}

		printf("Uptime(d:h:m:s): %02d:%02d:%02d:%02d\n",
		       days, hr, min, sec);
		printf("\t%u nanoseconds\n", nanosec);

		goto end;
	}

	if (!strcmp(argv[start_arg], "r")) {
		start_arg++;
		found = 1;
#if defined(PC_UTILITY) || defined(__KERNEL__)

		if ((strstr(argv[start_arg], "0x")) ||
		    (strstr(argv[start_arg], "0X")))
			printf("matches with 0x\n");
		else
			printf("Please give the address with "
			       "0x firmat\n");

#endif

		param.val[0] = mac_nstrtoul(argv[start_arg],
					    mac_nstrlen(argv[start_arg]),
					    &start_arg);
		gsw_mac_reg_read(fd, &param);

		goto end;
	}

	if (!strcmp(argv[start_arg], "w")) {
		start_arg++;
		found = 1;

#if defined(PC_UTILITY) || defined(__KERNEL__)

		if ((strstr(argv[start_arg], "0x")) ||
		    (strstr(argv[start_arg], "0X")))
			printf("matches with 0x\n");
		else
			printf("Please give the address with "
			       "0x format\n");

#endif

		param.val[0] = mac_nstrtoul(argv[start_arg],
					    mac_nstrlen(argv[start_arg]),
					    &start_arg);
		param.val[1] = mac_nstrtoul(argv[start_arg],
					    mac_nstrlen(argv[start_arg]),
					    &start_arg);
		gsw_mac_reg_write(fd, &param);

		goto end;
	}

	if (!strcmp(argv[start_arg], "get")) {
		start_arg++;

		if (!argv[start_arg]) {
			xgmac_menu();
			goto end;
		}

		for (i = 0; i < num_of_elem; i++) {
			removeSpace(xgmac_cfg_table[i].name);

			if (!strcmp(xgmac_cfg_table[i].name, argv[start_arg])) {

				param.cmdType = xgmac_cfg_table[i].cmdType;
#if 0

				if (param.set_all)
					printf("switch_cli xgmac * get %s\n", xgmac_cfg_table[i].name);
				else
					printf("switch_cli xgmac %d get %s\n", param.mac_idx, xgmac_cfg_table[i].name);

#endif

				param.get = 1;

				if (param.set_all) {

					if (xgmac_cfg_table[i].cmdType == MAC_CLI_GET_RMON) {
						if (xgmac_cfg_table[i].get_func) {
							xgmac_cfg_table[i].get_func(fd, &param);
						}

						found = 1;
						break;
					}

					if (gswip_version == GSWIP_3_1)
						max_mac = 3;
					else if (gswip_version == GSWIP_3_2)
						max_mac = 9;

					for (j = 0; j < max_mac; j++) {
						param.mac_idx = MAC_2 + j;

						if (xgmac_cfg_table[i].get_func) {
							xgmac_cfg_table[i].get_func(fd, &param);
						} else {

							if (cli_ioctl(fd, GSW_XGMAC_CFG, &param) != 0)
								return (-1);
						}
					}
				} else {
					if (xgmac_cfg_table[i].get_func) {
						xgmac_cfg_table[i].get_func(fd, &param);
					} else {

						if (cli_ioctl(fd, GSW_XGMAC_CFG, &param) != 0)
							return (-1);
					}
				}

				found = 1;
				break;
			}
		}
	}

	if (found)
		goto end;

	for (i = 0; i < num_of_elem; i++) {
		removeSpace(argv[start_arg]);

		if (!strcmp(xgmac_cfg_table[i].name, argv[start_arg])) {
			start_arg++;

			if (argc != xgmac_cfg_table[i].args) {
				printf("[USAGE:]\n");
				printf("switch_cli xgmac <idx> %s %s\n",
				       xgmac_cfg_table[i].name,
				       xgmac_cfg_table[i].help);
				return 0;
			}

			param.cmdType = xgmac_cfg_table[i].cmdType;

#if 1

			if (param.set_all)
				printf("switch_cli xgmac * %s", xgmac_cfg_table[i].name);
			else
				printf("switch_cli xgmac %d %s", param.mac_idx, xgmac_cfg_table[i].name);

#endif

			if (xgmac_cfg_table[i].cmdType == MAC_CLI_SET_MAC_ADDR) {
				con_mac_adr_str(argv[start_arg], param.mac_addr);
				//printf(" %02x:%02x:%02x:%02x:%02x:%02x",
				//	param.mac_addr[0],param.mac_addr[1],param.mac_addr[2],
				//	param.mac_addr[3],param.mac_addr[4],param.mac_addr[5]);
			} else {
				set_data(argv, i, &start_arg);

				if (xgmac_cfg_table[i].data1) {
					param.val[0] = *(xgmac_cfg_table[i].data1);
					//printf(" %d", param.val[0]);
				}

				if (xgmac_cfg_table[i].data2) {
					param.val[1] = *(xgmac_cfg_table[i].data2);
					//printf(" %d", param.val[1]);
				}

				if (xgmac_cfg_table[i].data3) {
					param.val[2] = *(xgmac_cfg_table[i].data3);
					//printf(" %d", param.val[2]);
				}

				if (xgmac_cfg_table[i].data4) {
					param.val[3] = *(xgmac_cfg_table[i].data4);
					//printf(" %d", param.val[3]);
				}

				if (xgmac_cfg_table[i].data5) {
					param.val[4] = *(xgmac_cfg_table[i].data5);
					//printf(" %d", param.val[4]);
				}

				if (xgmac_cfg_table[i].data6) {
					param.val[5] = *(xgmac_cfg_table[i].data6);
					//printf(" %d", param.val[5]);
				}
			}

			printf("\n");

			param.get = 0;

			if (param.set_all) {

				if (gswip_version == GSWIP_3_1)
					max_mac = 3;
				else if (gswip_version == GSWIP_3_2)
					max_mac = 9;

				for (j = 0; j < max_mac; j++) {
					param.mac_idx = MAC_2 + j;

					if (xgmac_cfg_table[i].set_func) {
						xgmac_cfg_table[i].set_func(fd, &param);
					} else {
						if (cli_ioctl(fd, GSW_XGMAC_CFG, &param) != 0)
							return (-1);
					}
				}
			} else {

				if (xgmac_cfg_table[i].set_func) {
					xgmac_cfg_table[i].set_func(fd, &param);
				} else {
					if (cli_ioctl(fd, GSW_XGMAC_CFG, &param) != 0)
						return (-1);
				}
			}

			found = 1;
			break;
		}
	}

end:

	if (found == 0)
		printf("command entered is invalid, use --help to display"
		       "cmds\n");

	return 0;
}


int test_cli(void *fd)
{
	int i = 0, j = 0, k = 0;
	int num_of_elem =
		(sizeof(xgmac_cfg_table) / sizeof(struct _xgmac_cfg));
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


	argv[0] = "switch_cli";
	argv[1] = "xgmac";

	for (k = 0; k < 5; k++) {
		if (k == 0)
			strcpy(argv[2], "2");
		else if (k == 1)
			strcpy(argv[2], "3");
		else if (k == 2)
			strcpy(argv[2], "4");
		else if (k == 3)
			strcpy(argv[2], "*");

		for (i = 1; i < num_of_elem; i++) {

			for (j = 3; j < 10; j++)
				memset(argv[j], '\0', 255);

			argc = xgmac_cfg_table[i].args;


			if (xgmac_cfg_table[i].data1 || xgmac_cfg_table[i].data2 ||
			    xgmac_cfg_table[i].data3 || xgmac_cfg_table[i].data4 ||
			    xgmac_cfg_table[i].data5 || xgmac_cfg_table[i].data6) {

				if (k == 4) {
					strcpy(argv[2], "2");
				}

				strcpy(argv[3], xgmac_cfg_table[i].name);

				if (xgmac_cfg_table[i].data1)
					strcpy(argv[4], "1");

				if (xgmac_cfg_table[i].data2)
					strcpy(argv[5], "2");

				if (xgmac_cfg_table[i].data3)
					strcpy(argv[6], "3");

				if (xgmac_cfg_table[i].data4)
					strcpy(argv[7], "4");

				if (xgmac_cfg_table[i].data5)
					strcpy(argv[8], "5");

				if (xgmac_cfg_table[i].data6)
					strcpy(argv[9], "6");

				printf("%s %s %s %s\n", argv[0], argv[1], argv[2], argv[3]);

				if (argv[4])
					printf(" %s", argv[4]);

				if (argv[5])
					printf(" %s", argv[5]);

				if (argv[6])
					printf(" %s", argv[6]);

				if (argv[7])
					printf(" %s", argv[7]);

				if (argv[8])
					printf(" %s", argv[8]);

				if (argv[9])
					printf(" %s", argv[9]);

				printf("\n");

			} else {

				strcpy(argv[3], "get");
				strcpy(argv[4], xgmac_cfg_table[i].name);

				printf("%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);
			}

			system("sleep 1");
			xgmac_main(argc, argv, fd);

			if (xgmac_cfg_table[i].get_func) {

				for (j = 3; j < 10; j++)
					memset(argv[j], '\0', 255);

				strcpy(argv[3], "get");
				strcpy(argv[4], xgmac_cfg_table[i].name);

				printf("%s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4]);
				system("sleep 1");
				xgmac_main(argc, argv, fd);
			}
		}
	}

	free(argv);

	return 0;
}

