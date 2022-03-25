/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/
/* =========================================================================
 * This file incorporates work covered by the following copyright and
 * permission notice:
 * The Synopsys DWC ETHER XGMAC Software Driver and documentation (hereinafter
 * "Software") is an unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto.  Permission is hereby granted,
 * free of charge, to any person obtaining a copy of this software annotated
 * with this license and the Software, to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * =========================================================================
 */

#include <xgmac.h>
#include <gswss_mac_api.h>

int xgmac_get_tx_cfg(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 txq_en;

	mac_printf("XGMAC %d: MTL_Q CFG\n", pdata->mac_idx);

	txq_en = XGMAC_RGRD_BITS(pdata, MTL_Q_TQOMR, TXQEN);
	mac_printf("\tMTL TX Q is %s by default\n",
		   txq_en ? "ENABLED" : "DISABLED");

	return 0;
}

int xgmac_get_counters_cfg(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 ror = 0;

	ror = XGMAC_RGRD_BITS(pdata, MMC_CR, ROR);

	mac_printf("XGMAC %d: RMON_CFG\n", pdata->mac_idx);

	if (ror)
		mac_printf("\tMMC mode: Counters reset to zero after read\n");
	else
		mac_printf("\tCounters will not get to Zero after a read\n");

	return 0;
}

int xgmac_get_fifo_size(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 rx_fifo_size, tx_fifo_size;

	mac_printf("XGMAC %d: FIFO SIZE\n", pdata->mac_idx);

	rx_fifo_size = XGMAC_RGRD_BITS(pdata, MTL_Q_RQOMR, RQS);
	tx_fifo_size = XGMAC_RGRD_BITS(pdata, MTL_Q_TQOMR, TQS);

	rx_fifo_size = ((rx_fifo_size + 1) * 256);
	tx_fifo_size = ((tx_fifo_size + 1) * 256);

	mac_printf("\tRX Q, RQS %d byte fifo per queue\n",  rx_fifo_size);
	mac_printf("\tTX Q, TQS %d byte fifo per queue\n",  tx_fifo_size);
	return 0;
}

int xgmac_get_flow_control_threshold(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 fifo_size, rx_fc, rfa, rfd;

	mac_printf("XGMAC %d: MTL FLOW Control Thresh\n", pdata->mac_idx);

	fifo_size = XGMAC_RGRD_BITS(pdata, MTL_Q_RQOMR, RQS);
	fifo_size = ((fifo_size + 1) * 256);
	rx_fc = XGMAC_RGRD_BITS(pdata, MTL_Q_RQOMR, EHFC);
	rfa = XGMAC_RGRD_BITS(pdata, MTL_Q_RQFCR, RFA);
	rfd = XGMAC_RGRD_BITS(pdata, MTL_Q_RQFCR, RFD);

	mac_printf("\tRX Q, RQS %d byte fifo per queue\n", fifo_size);
	mac_printf("\tRx Q, Flow control activte Thresh value %d\n", rfa);
	mac_printf("\tRx Q, Flow Control deactivate Thresh value %d\n", rfd);

	if ((fifo_size >= 4096) && (rx_fc == 1))
		mac_printf("\tFLow Control will get triggered according"
			   "to Thresh values\n");

	if (fifo_size < 4096)
		mac_printf("\tRQS is less than 4KB, Flow control"
			   "will not get triggered\n");

	if (rx_fc == 0)
		mac_printf("\tFlow control is disabled\n");

	return 0;
}

int xgmac_get_mtl_rx_flow_ctl(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 rx_fc;

	mac_printf("XGMAC %d: MTL RX Flow Control\n", pdata->mac_idx);

	rx_fc = XGMAC_RGRD_BITS(pdata, MTL_Q_RQOMR, EHFC);
	mac_printf("\tRX Flow control operation is %s\n",
		   rx_fc ? "ENABLED" : "DISABLED");
	return 0;
}

int xgmac_get_mtl_tx(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 tx_mode;

	mac_printf("XGMAC %d: MTL TX mode\n", pdata->mac_idx);

	tx_mode = XGMAC_RGRD_BITS(pdata, MTL_Q_TQOMR, TSF);

	if (tx_mode == 1)
		mac_printf("\tXGMAC %d TX Q Mode: Store and Forward mode\n",
			   pdata->mac_idx);
	else if (tx_mode == 0)
		mac_printf("\tXGMAC %d TX Q Mode: Thresh mode\n",
			   pdata->mac_idx);
	else
		mac_printf("\tXGMAC %d TX Q Mode: unknown mode\n",
			   pdata->mac_idx);

	xgmac_get_tx_threshold(pdev);
	return 0;
}

int xgmac_get_mtl_rx(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 rx_mode;

	mac_printf("XGMAC %d: MTL RX mode\n", pdata->mac_idx);

	rx_mode = XGMAC_RGRD_BITS(pdata, MTL_Q_RQOMR, RSF);

	if (rx_mode == 1)
		mac_printf("\tXGMAC %d RX Q Mode: Store and Forward mode\n",
			   pdata->mac_idx);
	else if (rx_mode == 0)
		mac_printf("\tXGMAC %d RX Q Mode: Thresh mode\n",
			   pdata->mac_idx);
	else
		mac_printf("\tXGMAC %d RX Q Mode: unknown mode\n",
			   pdata->mac_idx);

	xgmac_get_rx_threshold(pdev);
	return 0;
}

int xgmac_get_tx_threshold(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 tx_thresh;

	mac_printf("XGMAC %d: MTL TX Thresh\n", pdata->mac_idx);

	tx_thresh = XGMAC_RGRD_BITS(pdata, MTL_Q_TQOMR, TTC);

	if (tx_thresh == MTL_TX_THRESHOLD_32)
		mac_printf("\tXGMAC %d TX Q operates in Thresh 32 bytes\n",
			   pdata->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_64)
		mac_printf("\tXGMAC %d TX Q operates in Thresh 64 bytes\n",
			   pdata->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_96)
		mac_printf("\tXGMAC %d TX Q operates in Thresh 96 bytes\n",
			   pdata->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_128)
		mac_printf("\tXGMAC %d TX Q operates in Thresh 128 bytes\n",
			   pdata->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_192)
		mac_printf("\tXGMAC %d TX Q operates in Thresh 192 bytes\n",
			   pdata->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_256)
		mac_printf("\tXGMAC %d TX Q operates in Thresh 256 bytes\n",
			   pdata->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_384)
		mac_printf("\tXGMAC %d TX Q operates in Thresh 384 bytes\n",
			   pdata->mac_idx);
	else if (tx_thresh == MTL_TX_THRESHOLD_512)
		mac_printf("\tXGMAC %d TX Q operates in Thresh 512 bytes\n",
			   pdata->mac_idx);

	return 0;
}

int xgmac_get_rx_threshold(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 rx_thresh;

	mac_printf("XGMAC %d: MTL RX Thresh\n", pdata->mac_idx);

	rx_thresh = XGMAC_RGRD_BITS(pdata, MTL_Q_RQOMR, RTC);

	if (rx_thresh == MTL_RX_THRESHOLD_32)
		mac_printf("\tXGMAC %d RX Q operates in Thresh 32 bytes\n",
			   pdata->mac_idx);
	else if (rx_thresh == MTL_RX_THRESHOLD_64)
		mac_printf("\tXGMAC %d RX Q operates in Thresh 64 bytes\n",
			   pdata->mac_idx);
	else if (rx_thresh == MTL_RX_THRESHOLD_96)
		mac_printf("\tXGMAC %d RX Q operates in Thresh 96 bytes\n",
			   pdata->mac_idx);
	else if (rx_thresh == MTL_RX_THRESHOLD_128)
		mac_printf("\tXGMAC %d RX Q operates in Thresh 128 bytes\n",
			   pdata->mac_idx);

	return 0;
}

int xgmac_get_mtl_q_alg(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 tx_mtl_alg, rx_mtl_alg;

	tx_mtl_alg = XGMAC_RGRD_BITS(pdata, MTL_OMR, ETSALG);

	mac_printf("XGMAC %d: MTL_Q ALG\n", pdata->mac_idx);

	if (tx_mtl_alg == MTL_ETSALG_WRR)
		mac_printf("\tTX is set to WRR Algorithm\n");
	else if (tx_mtl_alg == MTL_ETSALG_WFQ)
		mac_printf("\tTX is set to WFQ Algorithm\n");
	else if (tx_mtl_alg == MTL_ETSALG_DWRR)
		mac_printf("\tTX is set to DWRR Algorithm\n");

	rx_mtl_alg = XGMAC_RGRD_BITS(pdata, MTL_OMR, RAA);

	if (rx_mtl_alg == MTL_RAA_SP)
		mac_printf("\tRX is set to Strict Priority Algorithm\n");
	else if (rx_mtl_alg == MTL_RAA_WSP)
		mac_printf("\tRX is set to WSP Algorithm\n");

	return 0;
}

int xgmac_get_crc_settings(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 cst, acs, dcrcc;

	mac_printf("XGMAC %d: CRC Stripping\n", pdata->mac_idx);
	cst = XGMAC_RGRD_BITS(pdata, MAC_RX_CFG, CST);
	mac_printf("\tCRC stripping for Type packets: %s\n",
		   cst ? "ENABLED" : "DISABLED");
	acs = XGMAC_RGRD_BITS(pdata, MAC_RX_CFG, ACS);
	mac_printf("\tAutomatic Pad or CRC Stripping: %s\n",
		   acs ? "ENABLED" : "DISABLED");
	dcrcc = XGMAC_RGRD_BITS(pdata, MAC_RX_CFG, DCRCC);
	mac_printf("\tMAC RX do not check the CRC field in the rx pkt: %s\n",
		   dcrcc ? "ENABLED" : "DISABLED");
	return 0;
}

int xgmac_dbg_eee_status(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 varmac_lps;

	varmac_lps = XGMAC_RGRD(pdata, MAC_LPI_CSR);

	mac_printf("XGMAC %d: EEE Status\n", pdata->mac_idx);

	mac_printf("\tMAC_LPI_Control_Status = %x\n", varmac_lps);

	if (varmac_lps & MAC_LPS_TLPIEN)
		mac_printf("\tMAC Transmitter has entered the LPI state\n");

	if (varmac_lps & MAC_LPS_TLPIEX)
		mac_printf("\tMAC Transmitter has exited the LPI state\n");

	if (varmac_lps & MAC_LPS_RLPIEN)
		mac_printf("\tMAC Receiver has entered the LPI state\n");

	if (varmac_lps & MAC_LPS_RLPIEX)
		mac_printf("\tMAC Receiver has exited the LPI state\n");

	return 0;
}

int xgmac_get_eee_settings(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 twt, lst, pls, lpitxa, lpitxen;

	twt = XGMAC_RGRD_BITS(pdata, MAC_LPI_TCR, TWT);
	lst = XGMAC_RGRD_BITS(pdata, MAC_LPI_TCR, LST);
	pls = XGMAC_RGRD_BITS(pdata, MAC_LPI_CSR, PLS);
	lpitxa = XGMAC_RGRD_BITS(pdata, MAC_LPI_CSR, LPITXA);
	lpitxen = XGMAC_RGRD_BITS(pdata, MAC_LPI_CSR, LPITXEN);

	mac_printf("XGMAC %d: EEE Settings\n", pdata->mac_idx);

	mac_printf("\tLPI LS TIMER: %d\n", lst);
	mac_printf("\tLPI TW TIMER: %d\n", twt);
	mac_printf("\tPhy link Status: %s\n",
		   pls ? "ENABLED" : "DISABLED");
	mac_printf("\tLPI Transmit Automate: %s\n",
		   lpitxa ? "ENABLED" : "DISABLED");
	mac_printf("\tLPI Transmit Enable: %s\n",
		   lpitxen ? "ENABLED" : "DISABLED");

	xgmac_dbg_eee_status(pdev);
	return 0;
}

int xgmac_get_mac_settings(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 te, re, ra;

	mac_printf("XGMAC %d: MAC Settings\n", pdata->mac_idx);

	te = XGMAC_RGRD_BITS(pdata, MAC_TX_CFG, TE);
	mac_printf("\tMAC TX: %s\n", te ? "ENABLED" : "DISABLED");
	re = XGMAC_RGRD_BITS(pdata, MAC_RX_CFG, RE);
	mac_printf("\tMAC RX: %s\n", re ? "ENABLED" : "DISABLED");
	ra = XGMAC_RGRD_BITS(pdata, MAC_PKT_FR, RA);
	mac_printf("\tMAC Filter RX ALL: %s\n", ra ? "ENABLED" : "DISABLED");
	return 0;
}

int xgmac_get_mac_rxtx_sts(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 rwt, tjt, wd, jd;

	mac_printf("XGMAC %d: MAC RXTX Status\n", pdata->mac_idx);

	jd = XGMAC_RGRD_BITS(pdata, MAC_TX_CFG, JD);
	wd = XGMAC_RGRD_BITS(pdata, MAC_RX_CFG, WD);

	mac_printf("\tWATCHDOG Disable       : %s\n",
		   wd ? "ENABLED" : "DISABLED");
	mac_printf("\tJABBER TIMEOUT Disable : %s\n",
		   jd ? "ENABLED" : "DISABLED");

	rwt = XGMAC_RGRD_BITS(pdata, MAC_RXTX_STS, RWT);

	if (wd)
		mac_printf("\tMAC : %s\n",
			   rwt ?
			   "Received packet > 16383 bytes with WD=1" :
			   "No packet recived with RWT");
	else
		mac_printf("\tMAC : %s\n",
			   rwt ?
			   "Received packet > 2048 bytes with WD=0" :
			   "No packet recived with RWT");

	tjt = XGMAC_RGRD_BITS(pdata, MAC_RXTX_STS, TJT);

	if (jd)
		mac_printf("\tMAC : %s\n",
			   tjt ?
			   "Transmitted packet > 16383 bytes with JD=1" :
			   "No packet transmitted with TJT");
	else
		mac_printf("\tMAC : %s\n",
			   tjt ?
			   "Transmitted packet > 2048 bytes with JD=0" :
			   "No packet transmitted with TJT");

	return 0;
}

int xgmac_get_mtu_settings(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_rcr, je, wd, gpslce, jd, gpsl;

	mac_rcr = XGMAC_RGRD(pdata, MAC_RX_CFG);

	je = MAC_GET_VAL(mac_rcr, MAC_RX_CFG, JE);
	wd = MAC_GET_VAL(mac_rcr, MAC_RX_CFG, WD);
	gpslce = MAC_GET_VAL(mac_rcr, MAC_RX_CFG, GPSLCE);
	gpsl = MAC_GET_VAL(mac_rcr, MAC_RX_CFG, GPSL);
	jd = XGMAC_RGRD_BITS(pdata, MAC_TX_CFG, JD);

	mac_printf("XGMAC %d: MTU Settings\n", pdata->mac_idx);

	mac_printf("\tMTU CONFIGURED %d\n", pdata->mtu);

	if (je)
		mac_printf("\tJumbo Enabled: 1, MAC allows jumbo packets of "
			   "9,018 bytes (9,022 bytes for VLAN tagged packets)"
			   "without reporting a giant packet error\n");

	if (wd)
		mac_printf("\tWatchdog Disable: 1,MAC disables the "
			   "watchdog timer on the receiver. The MAC can "
			   "receive packets of up to 16,383 bytes.\n");
	else
		mac_printf("\tWatchdog Disable: 0, MAC does not allow more "
			   "than 2,048 bytes (10,240 if JE is 1) of the pkt "
			   "being received.The MAC cuts off any bytes "
			   "received after 2,048 bytes\n");

	if (gpslce) {
		mac_printf("\tGPSLCE: 1, MAC considers the value in "
			   "GPSL field to declare a received packet "
			   "as Giant packet\n");
		mac_printf("\tGPSL: %04x\n", gpsl);
	} else {
		mac_printf("\tGPSLCE: 0, MAC considers a RX packet as Giant  "
			   "packet when its size is greater than 1,518 bytes"
			   "(1522 bytes for tagged packet)\n");
	}

	if (jd)
		mac_printf("\tJabber Disable: 1, XGMAC disables the "
			   "jabber timer on the tx. Tx of up to 16,383-byte  "
			   "frames is supported.\n");
	else
		mac_printf("\tJabber Disable: 0, XGMAC cuts off the TX "
			   "if the application sends more than 2,048 bytes "
			   "of data (10,240 bytes if JE is 1 during TX\n");

	return 0;
}

int xgmac_get_checksum_offload(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 co = 0;

	co = XGMAC_RGRD_BITS(pdata, MAC_RX_CFG, IPC);
	mac_printf("XGMAC %d: Checksum Offload Settings\n", pdata->mac_idx);
	mac_printf("\tChecksum Offload : %s\n", co ? "ENABLED" : "DISABLED");
	return 0;
}

int xgmac_get_mac_addr(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_addr_hi, mac_addr_lo;
	u8 mac_addr[6];

	mac_addr_hi = XGMAC_RGRD(pdata, MAC_MACA0HR);
	mac_addr_lo = XGMAC_RGRD(pdata, MAC_MACA0LR);

	mac_printf("XGMAC %d: MAC ADDR ", pdata->mac_idx);

	mac_addr[5] = ((mac_addr_hi & 0x0000FF00) >> 8);
	mac_addr[4] = (mac_addr_hi & 0x000000FF);
	mac_addr[3] = ((mac_addr_lo & 0xFF000000) >> 24);
	mac_addr[2] = ((mac_addr_lo & 0x00FF0000) >> 16);
	mac_addr[1] = ((mac_addr_lo & 0x0000FF00) >> 8);
	mac_addr[0] = (mac_addr_lo & 0x000000FF);

	mac_printf(" %02x:%02x:%02x:%02x:%02x:%02x\n",
		   mac_addr[0],
		   mac_addr[1],
		   mac_addr[2],
		   mac_addr[3],
		   mac_addr[4],
		   mac_addr[5]);
	return 0;
}

int xgmac_get_mac_rx_mode(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mode = 0;

	mac_printf("XGMAC %d: MAC RX MODE\n", pdata->mac_idx);

	mode = XGMAC_RGRD_BITS(pdata, MAC_PKT_FR, PR);
	mac_printf("\tPromiscous Mode    : %s\n",
		   mode ? "ENABLED" : "DISABLED");
	mode = XGMAC_RGRD_BITS(pdata, MAC_PKT_FR, PM);
	mac_printf("\tPass All Multicast : %s\n",
		   mode ? "ENABLED" : "DISABLED");
	return 0;
}

int xgmac_get_rx_vlan_filtering_mode(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 vtfe = 0;

	vtfe = XGMAC_RGRD_BITS(pdata, MAC_PKT_FR, VTFE);

	mac_printf("XGMAC %d: RX VLAN Filtering\n", pdata->mac_idx);
	mac_printf("\tRX VLAN Filtering is %s\n",
		   vtfe ? "ENABLED" : "DISABLED");
	return 0;
}

int xgmac_get_mac_speed(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 speed;

	speed  = XGMAC_RGRD_BITS(pdata, MAC_TX_CFG, SS);
	mac_printf("XGMAC %d: MAC Speed\n", pdata->mac_idx);

	if (speed == 0)
		mac_printf("\tXGMAC configured for XGMII 10G speed\n");
	else if (speed == 2)
		mac_printf("\tXGMAC configured for GMII 2.5G speed\n");
	else if (speed == 3)
		mac_printf("\tXGMAC configured for GMII 1G speed\n");

	return 0;
}

int xgmac_get_pause_frame_ctl(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 pfce = 0, rfe = 0, tfe = 0, pt = 0;

	pfce = XGMAC_RGRD_BITS(pdata, MAC_RX_FCR, PFCE);
	rfe = XGMAC_RGRD_BITS(pdata, MAC_RX_FCR, RFE);
	tfe = XGMAC_RGRD_BITS(pdata, MAC_TX_FCR, TFE);
	pt = XGMAC_RGRD_BITS(pdata, MAC_TX_FCR, PT);

	mac_printf("XGMAC %d: Pause Frame Settings\n", pdata->mac_idx);

	mac_printf("\tPriority based Flow control: %s\n",
		   pfce ? "ENABLED" : "DISABLED");

	if (pfce)
		mac_printf("\tEnables TX of priority based flow Ctrl Pkts\n");
	else
		mac_printf("\tEnables TX and RX of 802.3x Pause Ctrl Pkts\n");

	mac_printf("\tReceive Flow control:        %s\n",
		   rfe ? "ENABLED" : "DISABLED");

	if (rfe)
		mac_printf("\tMAC decodes the Rx Pause packets and "
			   "disables the TX for a specified Pause time\n");
	else
		mac_printf("\tMAC doesnot decode the Pause packet\n");

	mac_printf("\tTransmit Flow control:       %s\n",
		   tfe ? "ENABLED" : "DISABLED");

	if (tfe)
		mac_printf("\tMAC enables Flow control operation "
			   "based on Pause frame\n");
	else
		mac_printf("\tMAC does not transmit and Pause packets by "
			   "itself\n");

	mac_printf("\tPause Time:                  %d\n", pt);
	return 0;
}

int xgmac_get_mac_loopback_mode(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 lm;

	lm = XGMAC_RGRD_BITS(pdata, MAC_RX_CFG, LM);

	mac_printf("XGMAC %d: MAC Loopback\n", pdata->mac_idx);
	mac_printf("\tMAC Loopback mode: %s\n", lm ? "ENABLED" : "DISABLED");
	return 0;
}

int xgmac_get_tstamp_settings(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tscr, val;

	mac_tscr = XGMAC_RGRD(pdata, MAC_TSTAMP_CR);

	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENA);

	mac_printf("XGMAC %d: Timestamp Settings\n", pdata->mac_idx);
	mac_printf("\tTimestamp is added for TX and RX packets: %s\n",
		   val ? "ENABLED" : "DISABLED");

	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSCFUPDT);
	mac_printf("\tTimestamp Update type: %s\n", val ? "FINE" : "COARSE");
	mac_printf("\tAddend present only in FINE update, "
		   "Timestamp Addend value %d\n",
		   XGMAC_RGRD(pdata, MAC_TSTAMP_ADDNDR));
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENALL);
	mac_printf("\tTimestamp for All Packets Received: %s\n",
		   val ? "ENABLED" : "DISABLED");
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSCTRLSSR);
	mac_printf("\tTimestamp Digital or Binary Rollover Control %s\n",
		   val ?
		   "TIME STAMP DIGITAL (1ns accuracy)" :
		   "BINARY ROLLOVER");
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSTRIG);
	mac_printf("\tTSTRIG: Timestamp Interrupt trigger: %s\n",
		   val ? "ENABLED" : "DISABLED");
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSVER2ENA);
	mac_printf("\tTSVER2ENA: PTP Pkt Processing for Ver 2 Format: %s\n",
		   val ? "ENABLED" : "DISABLED");
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSIPENA);
	mac_printf("\tTSIPENA: MAC receiver processes the PTP packets "
		   "encapsulated directly in the Ethernet packets: %s\n",
		   val ? "ENABLED" : "DISABLED");
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSIPV6ENA);
	mac_printf("\tTSIPV6ENA: MAC receiver processes the PTP packets "
		   "encapsulated in IPv6-UDP packets: %s\n",
		   val ? "ENABLED" : "DISABLED");
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSIPV4ENA);
	mac_printf("\tTSIPV4ENA: MAC receiver processes the PTP packets "
		   "encapsulated in IPv4-UDP packets: %s\n",
		   val ? "ENABLED" : "DISABLED");
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSEVNTENA);
	mac_printf("\tTSEVNTENA: Timestamp snapshot is taken only for "
		   "event msg (SYNC, Delay_Req, Pdelay_Req, or "
		   "Pdelay_Resp): %s\n",
		   val ? "ENABLED" : "DISABLED");
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, SNAPTYPSEL);
	mac_printf("\tSNAPTYPSEL: These bits, along with TSMSTRENA "
		   "TSEVNTENA, decide the set of PTP packet types for which "
		   "snapshot needs to be taken. %s\n",
		   val ? "ENABLED" : "DISABLED");
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSMSTRENA);
	mac_printf("\tTSMSTRENA: Snapshot is taken only for the "
		   "messages that are relevant to the master node: %s\n",
		   val ? "ENABLED" : "DISABLED");
	return 0;
}

int xgmac_get_tstamp_status(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tscr, val;
	u64 time;

	mac_tscr = XGMAC_RGRD(pdata, MAC_TSTAMP_STSR);

	mac_printf("XGMAC %d: Timestamp Status\n", pdata->mac_idx);

	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, ATSNS);
	mac_printf("\tNumber of Auxiliary Timestamp Snapshots:          %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, ATSSTM);
	mac_printf("\tAuxiliary Timestamp Snapshot Trigger Missed:      %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, ATSSTN);
	mac_printf("\tAuxiliary Timestamp Snapshot Trigger Identifier   %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TXTSC);
	mac_printf("\tTX Timestamp Captured:                            %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TSTRGTERR3);
	mac_printf("\tTimestamp Target Time Error:                      %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TSTARGT3);
	mac_printf("\tTimestamp Target Time Reached for Time PPS3:      %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TSTRGTERR2);
	mac_printf("\tTimestamp Target Time Error:                      %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TSTARGT2);
	mac_printf("\tTimestamp Target Time Reached for Time PPS2:      %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TSTRGTERR1);
	mac_printf("\tTimestamp Target Time Error:                      %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TSTARGT1);
	mac_printf("\tTimestamp Target Time Reached for Time PPS1:      %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TSTRGTERR0);
	mac_printf("\tTimestamp Target Time Error:                      %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TSTARGT0);
	mac_printf("\tTimestamp Target Time Reached:                    %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, AUXTSTRIG);
	mac_printf("\tAuxiliary Timestamp Trigger Snapshot:             %d\n",
		   val);
	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TSSOVF);
	mac_printf("\tTimestamp Seconds Overflow                        %d\n",
		   val);

	mac_printf("\n\n");

	time = xgmac_get_systime(pdev);
	mac_printf("\t64 bit system time in nsec                      %lld\n",
		   time);

	val = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_STSR, TTSNS);
	mac_printf("\tTstamp captured count in xgmac Fifo:              %d\n",
		   val);

	time = xgmac_get_tx_tstamp(pdev);
	mac_printf("\tTimestamp captured in nsec:                     %lld\n",
		   time);

	return 0;
}

int xgmac_print_system_time(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u64 nsec;
	u32 reg_sec, nanosec, sec;
	u32 min, hr, days;

	reg_sec = XGMAC_RGRD(pdata, MAC_SYS_TIME_SEC);
	nanosec = XGMAC_RGRD(pdata, MAC_SYS_TIME_NSEC);
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

	mac_printf("Uptime(d:h:m:s):     %02d:%02d:%02d:%02d\n",
		   days, hr, min, sec);

	mac_printf("Sec                  %d\n", reg_sec);
	mac_printf("NanoSec              %d\n", nanosec);
	mac_printf("Total in nsec        %lld\n", nsec);
	return 0;
}

int xgmac_get_txtstamp_mode(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tscr = 0, mac_txtstamp;
	u32 snaptypesel, tsmstrena, tsevntena, tsena;

	mac_txtstamp = GSWSS_MAC_RGRD(pdata, MAC_TXTS_CIC(pdata->mac_idx));
	mac_printf("TTSE:         %s\n",
		   GET_N_BITS(mac_txtstamp, 4, 1) ? "ENABLED" : "DISABLED");
	mac_printf("OSTC:         %s\n",
		   GET_N_BITS(mac_txtstamp, 3, 1) ? "ENABLED" : "DISABLED");
	mac_printf("OSTC_AVAIL:   %s\n",
		   GET_N_BITS(mac_txtstamp, 2, 1) ? "ENABLED" : "DISABLED");

	mac_tscr = XGMAC_RGRD(pdata, MAC_TSTAMP_CR);

	snaptypesel = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, SNAPTYPSEL);
	tsevntena = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSEVNTENA);
	tsmstrena = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSMSTRENA);
	tsena = MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENA);

	mac_printf("snaptypesel   %d\n", snaptypesel);
	mac_printf("tsevntena     %d\n", tsevntena);
	mac_printf("tsmstrena     %d\n", tsmstrena);
	mac_printf("tsena         %d\n", tsena);
	return 0;
}

int xgmac_get_debug_sts(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 dbg_sts, dbg_ctl, fifo_sel, pktstate, byteen;

	dbg_ctl = XGMAC_RGRD(pdata, MTL_DBG_CTL);
	dbg_sts = XGMAC_RGRD(pdata, MTL_DBG_STS);

	fifo_sel = MAC_GET_VAL(dbg_sts, MTL_DBG_CTL, FIFOSEL);

	mac_printf("XGMAC %d: Debug Status\n", pdata->mac_idx);

	if (MAC_GET_VAL(dbg_sts, MTL_DBG_STS, FIFOBUSY)) {
		mac_printf("\tA FIFO operation is in progress in the MAC, "
			   "All other fields in this register is Invalid\n");
		return 0;
	}

	pktstate = MAC_GET_VAL(dbg_sts, MTL_DBG_CTL, PKTSTATE);

	if (fifo_sel == 0) {
		mac_printf("\tTX FIFO Selected\n");

		if (pktstate == 0)
			mac_printf("\tType of Data Read: PACKET_DATA\n");
		else if (pktstate == 1)
			mac_printf("\tType of Data Read: CONTROL_WORD\n");
		else if (pktstate == 2)
			mac_printf("\tType of Data Read: SOP_DATA\n");
		else if (pktstate == 3)
			mac_printf("\tType of Data Read: EOP_DATA\n");

		mac_printf("\tSpace Available in Tx Fifo %d\n",
			   MAC_GET_VAL(dbg_sts, MTL_DBG_STS, LOCR));
	} else if (fifo_sel == 3) {
		mac_printf("\tRX FIFO Selected\n");

		if (pktstate == 0)
			mac_printf("\tType of Data Read: PACKET_DATA\n");
		else if (pktstate == 1)
			mac_printf("\tType of Data Read: NORMAL_STS\n");
		else if (pktstate == 2)
			mac_printf("\tType of Data Read: LAST_STS\n");
		else if (pktstate == 3)
			mac_printf("\tType of Data Read: EOP\n");

		mac_printf("\tSpace Available in Rx Fifo %d\n",
			   MAC_GET_VAL(dbg_sts, MTL_DBG_STS, LOCR));
	}

	byteen = MAC_GET_VAL(dbg_ctl, MTL_DBG_CTL, BYTEEN);

	if (MAC_GET_VAL(dbg_sts, MTL_DBG_STS, PKTI))
		mac_printf("\tFull packet is available in RxFIFO\n");

	return 0;
}

int xgmac_get_debug_data(void *pdev, u32 *dbg_data)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 fifo_sel, dbg_ctrl;

	dbg_ctrl = XGMAC_RGRD(pdata, MTL_DBG_CTL);
	*dbg_data = XGMAC_RGRD(pdata, MTL_DBG_DAT);
	fifo_sel = MAC_GET_VAL(dbg_ctrl, MTL_DBG_CTL, FIFOSEL);

	mac_printf("XGMAC %d: Debug Data\n", pdata->mac_idx);

	if (fifo_sel == 0)
		mac_printf("\tData pointer in the Tx FIFO %08x\n", *dbg_data);
	else if (fifo_sel == 3)
		mac_printf("\tData pointer in the Rx FIFO %08x\n", *dbg_data);

	return 0;
}

int xgmac_get_fifo_space_left(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 locr, dbg_ctl, fifo_sel, fdbgen, dbgmod;

	locr = XGMAC_RGRD_BITS(pdata, MTL_DBG_STS, LOCR);

	dbg_ctl = XGMAC_RGRD(pdata, MTL_DBG_CTL);

	fifo_sel = MAC_GET_VAL(dbg_ctl, MTL_DBG_CTL, FIFOSEL);
	fdbgen = MAC_GET_VAL(dbg_ctl, MTL_DBG_CTL, FDBGEN);
	dbgmod = MAC_GET_VAL(dbg_ctl, MTL_DBG_CTL, DBGMOD);

	mac_printf("XGMAC %d: Fifo Space Left\n", pdata->mac_idx);

	if (fdbgen && !dbgmod) {
		if (fifo_sel == 0)
			mac_printf("\tSlave Mode: Space Available in the "
				   "TX FIFO %d\n", locr);

		if (fifo_sel == 3)
			mac_printf("\tSlave Mode: Space Available in the "
				   "RX FIFO %d\n", locr);
	}

	if (fdbgen && dbgmod) {
		if (fifo_sel == 0)
			mac_printf("\tData to be written to the Tx FIFO %d\n",
				   locr);

		if (fifo_sel == 3)
			mac_printf("\tData to be written to the Rx FIFO %d\n",
				   locr);
	}

	return 0;
}

int xgmac_dbg_int_sts(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_isr = 0, val;

	mac_printf("XGMAC %d: MAC Interrupt Status\n", pdata->mac_idx);
	mac_isr = XGMAC_RGRD(pdata, MAC_ISR);
	val = XGMAC_RGRD(pdata, MAC_IER);
	mac_printf("\tMAC_IER interrupts  %s %08x\n",
		   val ? "ENABLED" : "DISABLED", val);

	/* Enable Timestamp interrupt */
	if (val & MASK(MAC_IER, TSIE))
		mac_printf("\t\tTimestamp Interrupt Enabled\n");

	/* Enable LPI interrupt (EEE) */
	if (val & MASK(MAC_IER, LPIIE))
		mac_printf("\t\tLPI interrupt (EEE) Enabled\n");

	/* Enable transmit error status interrupt */
	if (val & MASK(MAC_IER, TXESIE))
		mac_printf("\t\tTransmit error status interrupt Enabled\n");

	/* Enable Receive error status interrupt */
	if (val & MASK(MAC_IER, RXESIE))
		mac_printf("\t\tReceive error status interrupt Enabled\n");

	/* Enable power management interrupt */
	if (val & MASK(MAC_IER, PMTIE))
		mac_printf("\tPower Management interrupt Enabled\n");

	if (!mac_isr) {
		mac_printf("\tNo MAC interrupt status available %d\n",
			   mac_isr);
	} else {
		mac_printf("XGMAC %d: MAC Interrupt Status %08x\n",
			   pdata->mac_idx, mac_isr);
		val = MAC_GET_VAL(mac_isr, MAC_ISR, LSI);

		if (val)
			mac_printf("\tMAC_INT_STS: Link Status bits change "
				   "their value\n");

		val = MAC_GET_VAL(mac_isr, MAC_ISR, SMI);

		if (val)
			mac_printf("\tMAC_INT_STS: Any of the bits in the "
				   "MDIO Interrupt Status Register is set\n");

		val = MAC_GET_VAL(mac_isr, MAC_ISR, PMTIS);

		if (val)
			mac_printf("\tMAC_INT_STS: A Magic packet or "
				   "Wake-on-LAN packet is received in the "
				   "power-down mode\n");

		val = MAC_GET_VAL(mac_isr, MAC_ISR, LPIIS);

		if (val)
			mac_printf("\tMAC_INT_STS: It is set for any LPI "
				   "state entry or exit in the MAC Tx/Rx\n");

		val = MAC_GET_VAL(mac_isr, MAC_ISR, MMCRXIS);

		if (val)
			mac_printf("\tMAC_INT_STS: Interrupt is generated "
				   "in the MMC Receive Interrupt Register\n");

		val = MAC_GET_VAL(mac_isr, MAC_ISR, MMCTXIS);

		if (val)
			mac_printf("\tMAC_INT_STS: Interrupt is generated in "
				   "the MMC Transmit Interrupt Register\n");

		val = MAC_GET_VAL(mac_isr, MAC_ISR, TSIS);

		if (val)
			mac_printf("\tMAC_INT_STS: Timestamp Interrupt Status"
				   "is set\n");

		val = MAC_GET_VAL(mac_isr, MAC_ISR, TXESIS);

		if (val)
			mac_printf("\tMAC_INT_STS: Transmit Error, "
				   "Jabber Timeout (TJT) event occurs during "
				   "transmission\n");

		val = MAC_GET_VAL(mac_isr, MAC_ISR, RXESIS);

		if (val)
			mac_printf("\tMAC_INT_STS: Receive Error, Watchdog "
				   "Timeout (WDT) event occurs during Rx.\n");

		val = MAC_GET_VAL(mac_isr, MAC_ISR, GPIIS);

		if (val)
			mac_printf("\tMAC_INT_STS: GPIO Interrupt status "
				   "is set\n");

		val = MAC_GET_VAL(mac_isr, MAC_ISR, LS);

		if (val == 0)
			mac_printf("\tMAC_INT_STS: Current Link Status: %s\n",
				   "LINK OK");

		if (val == 2)
			mac_printf("\tMAC_INT_STS: Current Link Status: %s\n",
				   "Local Link Fault");

		if (val == 3)
			mac_printf("\tMAC_INT_STS: Current Link Status: %s\n",
				   "Remote Link Fault");

		xgmac_clear_mac_int(pdev);

		return val;
	}

	val = xgmac_get_mtl_int_sts(pdev);
	return val;
}


int xgmac_get_mtl_underflow_pkt_cnt(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mtl_q;
	u32 val;

	mtl_q = XGMAC_RGRD(pdata, MTL_Q_TQUR);
	mac_printf("XGMAC %d: MTL Underflow Pkt Counter %08x\n",
		   pdata->mac_idx, mtl_q);

	val = MAC_GET_VAL(mtl_q, MTL_Q_TQUR, UFCNTOVF);

	if (val)
		mac_printf("\t\tOverflow bit of Underflow pkt counter %d\n",
			   val);

	val = MAC_GET_VAL(mtl_q, MTL_Q_TQUR, UFPKTCNT);

	if (val)
		mac_printf("\t\tUnerflow packet counter %d\n", val);

	return 0;
}

int xgmac_get_mtl_missed_pkt_cnt(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mtl_q;
	u32 val;

	mtl_q = XGMAC_RGRD(pdata, MTL_Q_RQMPOCR);
	mac_printf("XGMAC %d: MTL Missed Overflow Pkt Counter %08x\n",
		   pdata->mac_idx, mtl_q);

	val = MAC_GET_VAL(mtl_q, MTL_Q_RQMPOCR, MISCNTOVF);

	if (val)
		mac_printf("\t\tRx Queue Missed Packet Counter crossed the "
			   "maximum limit %d\n", val);

	val = MAC_GET_VAL(mtl_q, MTL_Q_RQMPOCR, MISPKTCNT);

	if (val)
		mac_printf("\t\tNumber of packets missed by XGMAC %d\n", val);

	val = MAC_GET_VAL(mtl_q, MTL_Q_RQMPOCR, OVFCNTOVF);

	if (val)
		mac_printf("\t\tRx Queue Overflow Packet Counter field "
			   "crossed the maximum limit %d\n", val);

	val = MAC_GET_VAL(mtl_q, MTL_Q_RQMPOCR, OVFPKTCNT);

	if (val)
		mac_printf("\tNo: of packets discarded by the XGMAC %d\n",
			   val);

	return 0;
}

int xgmac_get_mtl_int_sts(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mtl_q_isr;
	u32 val;

	mtl_q_isr = XGMAC_RGRD(pdata, MTL_Q_ISR);
	val = XGMAC_RGRD(pdata, MTL_Q_IER);

	mac_printf("XGMAC %d: MTL Interrupt Status\n", pdata->mac_idx);

	mac_printf("\tMTL_Q_IER interrupts %s %08x\n",
		   val ? "ENABLED" : "DISABLED", val);

	/* Tx Q Overflow Interrupt Enable */
	if (val & MASK(MTL_Q_IER, TXUIE))
		mac_printf("\t\tTx Q Overflow Interrupt Enabled\n");

	/* Average bits per slot interrupt enable */
	if (val & MASK(MTL_Q_IER, ABPSIE))
		mac_printf("\t\tAverage bits per slot interrupt Enabled\n");

	/* Rx Q Overflow Interrupt Enable */
	if (val & MASK(MTL_Q_IER, RXOIE))
		mac_printf("\t\tRx Q Overflow Interrupt Enabled\n");

	if (!mtl_q_isr) {
		mac_printf("\tNo MTL interrupt status available\n");
	} else {
		/* Tx Q Overflow Interrupt Enable */
		val = MAC_GET_VAL(mtl_q_isr, MTL_Q_ISR, TXUNFIS);

		if (val)
			mac_printf("\tTransmit Queue had an Underflow "
				   "during packet transmission\n");

		// TODO: Check whether this bit is reserved since traffic class is 1
		/* Average bits per slot interrupt enable */
		val = MAC_GET_VAL(mtl_q_isr, MTL_Q_ISR, ABPSIS);

		if (val)
			mac_printf("\tMAC has updated the ABS value for "
				   "Traffic Class 0\n");

		/* Rx Q Overflow Interrupt Enable */
		val = MAC_GET_VAL(mtl_q_isr, MTL_Q_ISR, RXOVFIS);

		if (val)
			mac_printf("\tReceive Queue had an Overflow during "
				   "packet reception\n");
	}

	return 0;
}

int xgmac_get_fup_fep_setting(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 fup, fef;

	mac_printf("XGMAC %d: FUP/FEP Settings\n", pdata->mac_idx);

	fup = XGMAC_RGRD_BITS(pdata, MTL_Q_RQOMR, FUP);
	mac_printf("\tForward Undersized Good Packets for RxQ: %s\n",
		   fup ? "ENABLE" : "DISABLE");
	fef = XGMAC_RGRD_BITS(pdata, MTL_Q_RQOMR, FEF);
	mac_printf("\tForward Error Packets for RxQ:           %s\n",
		   fef ? "ENABLE" : "DISABLE");
	return 0;
}

int xgmac_get_ipg(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 ipg, ifp, speed;

	ipg = XGMAC_RGRD_BITS(pdata, MAC_TX_CFG, IPG);
	ifp = XGMAC_RGRD_BITS(pdata, MAC_TX_CFG, IFP);
	speed = XGMAC_RGRD_BITS(pdata, MAC_TX_CFG, SS);

	mac_printf("XGMAC %d: IPG Settings\n", pdata->mac_idx);

	if (((speed == 3) || (speed == 2)) && (ifp == 0)) {
		mac_printf("\tGMMI mode Minimum IPG between packets during "
			   "TX is %d bits\n", (96 - (ipg * 8)));
		return 0;
	}

	if ((speed == 0) && (ifp == 0))
		mac_printf("\tMinimum IPG between packets during TX is %d "
			   "bits, XGMII mode No reduction possible\n", 96);

	if (ifp)
		mac_printf("\tMinimum IPG between packets during "
			   "TX is %d bits\n", (96 - (ipg * 32)));

	return 0;
}

int xgmac_get_extcfg(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_extcfg, val;

	mac_extcfg = XGMAC_RGRD(pdata, MAC_EXTCFG);

	val = MAC_GET_VAL(mac_extcfg, MAC_EXTCFG, SBDIOEN);
	mac_printf("XGMAC %d: MAC Extended CFG SGDIOEN: %s\n",
		   pdata->mac_idx, val ? "ENABLED" : "DISABLED");
	return 0;
}

int xgmac_dbg_pmt(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_pmtcsr, val, i = 0;
	u32 value[8];

	mac_pmtcsr = XGMAC_RGRD(pdata, MAC_PMT_CSR);

	mac_printf("XGMAC %d: PMT Settings\n", pdata->mac_idx);

	val = MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, MGKPKTEN);
	mac_printf("Magic_Packet_Enable:          %s\n",
		   val ? "ENABLED" : "DISABLED");

	val = MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, RWKPKTEN);
	mac_printf("Remote_Wakeup_Packet_Enable: %s\n",
		   val ? "ENABLED" : "DISABLED");

	val = MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, PWRDWN);
	mac_printf("Power_Down:           %s\n",
		   val ? "ENABLED" : "DISABLED");

	val = MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, MGKPRCVD);
	mac_printf("Magic_Packet :                %s\n",
		   val ? "Received" : "Not Received");

	val = MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, RWKPRCVD);
	mac_printf("Remote_Wakeup_Packet :       %s\n",
		   val ? "Received" : "Not Received");

	val = MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, RWKPTR);
	mac_printf("Remote_Wakeup_FIFO_Pointer : %d\n", val);

	val = MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, GLBLUCAST);
	mac_printf("Global_unicast:               %d\n", val);

	for (i = 0; i < 8; i++) {
		value[i] = XGMAC_RGRD(pdata, MAC_RWK_PFR);
		mac_printf("Remote_Wakeup_Packet_REG[%d]:       %08x\n",
			   i, value[i]);
	}

	return 0;
}

#if defined(CHIPTEST) && CHIPTEST
int xgmac_get_stats_all(void *pdev)
{
	int i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	for (i = 0; i < pdata->max_mac; i++) {
		mac_printf("XGMAC %d: Reading rmon\n", i);
		xgmac_read_mmc_stats(&prv_data[i], &prv_data[i].mmc_stats);
	}

	mac_printf("\nTYPE                      %s\t%s\t%s\n", "      XGMAC 2", "    XGMAC 3", "    XGMAC 4\n");

	mac_printf("Rx_Packets                = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t", prv_data[i].mmc_stats.rxframecount_gb);

	mac_printf("\n");

	mac_printf("Rx_Bytes                  = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t", prv_data[i].mmc_stats.rxoctetcount_gb);

	mac_printf("\n");

	mac_printf("Rx_Byte_errors            = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t",
			   (prv_data[i].mmc_stats.rxoctetcount_gb -
			    prv_data[i].mmc_stats.rxoctetcount_g));

	mac_printf("\n");

	mac_printf("Rx_Pauseframe             = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t", prv_data[i].mmc_stats.rxpauseframes);

	mac_printf("\n");

	mac_printf("Rx_Crc_Errors             = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t", prv_data[i].mmc_stats.rxcrcerror);

	mac_printf("\n");

	mac_printf("Rx_Fifo_Errors            = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t", prv_data[i].mmc_stats.rxfifooverflow);

	mac_printf("\n");
	mac_printf("\n");

	mac_printf("Tx_Packets                = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t", prv_data[i].mmc_stats.txframecount_gb);

	mac_printf("\n");

	mac_printf("Tx_Bytes                  = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t", prv_data[i].mmc_stats.txoctetcount_gb);

	mac_printf("\n");

	mac_printf("Tx_Packet_Errors          = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t",
			   (prv_data[i].mmc_stats.txframecount_gb -
			    prv_data[i].mmc_stats.txframecount_g));

	mac_printf("\n");

	mac_printf("Tx_Byte_Errors            = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t",
			   (prv_data[i].mmc_stats.txoctetcount_gb -
			    prv_data[i].mmc_stats.txoctetcount_g));

	mac_printf("\n");

	mac_printf("Tx_Pauseframe             = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t", prv_data[i].mmc_stats.txpauseframes);

	mac_printf("\n");

	mac_printf("Tx_underflow_error        = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%11d\t", prv_data[i].mmc_stats.txunderflowerror);

	mac_printf("\n");
	return 0;
}

int xgmac_get_stats(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	int i = 0;
	struct xgmac_mmc_stats *pstats = &pdata->mmc_stats;

	mac_printf("XGMAC %d: Reading rmon\n", pdata->mac_idx);

	xgmac_read_mmc_stats(pdev, pstats);

	mac_printf("\nTYPE                            XGMAC %d\n\n",
		   pdata->mac_idx);
	mac_printf("Rx_Packets                = ");

	mac_printf("%11d\t", pdata->mmc_stats.rxframecount_gb);

	mac_printf("\n");

	mac_printf("Rx_Bytes                  = ");

	mac_printf("%11d\t", pdata->mmc_stats.rxoctetcount_gb);

	mac_printf("\n");

	mac_printf("Rx_Byte_errors            = ");

	mac_printf("%11d\t",
		   (pdata->mmc_stats.rxoctetcount_gb -
		    pdata->mmc_stats.rxoctetcount_g));

	mac_printf("\n");

	mac_printf("Rx_Pauseframe             = ");

	mac_printf("%11d\t", pdata->mmc_stats.rxpauseframes);

	mac_printf("\n");

	mac_printf("Rx_Crc_Errors             = ");

	mac_printf("%11d\t", pdata->mmc_stats.rxcrcerror);

	mac_printf("\n");

	mac_printf("Rx_Fifo_Errors            = ");

	mac_printf("%11d\t", pdata->mmc_stats.rxfifooverflow);

	mac_printf("\n");
	mac_printf("\n");

	mac_printf("Tx_Packets                = ");

	mac_printf("%11d\t", pdata->mmc_stats.txframecount_gb);

	mac_printf("\n");

	mac_printf("Tx_Bytes                  = ");

	mac_printf("%11d\t", pdata->mmc_stats.txoctetcount_gb);

	mac_printf("\n");

	mac_printf("Tx_Packet_Errors          = ");

	mac_printf("%11d\t",
		   (pdata->mmc_stats.txframecount_gb -
		    pdata->mmc_stats.txframecount_g));

	mac_printf("\n");

	mac_printf("Tx_Byte_Errors            = ");

	mac_printf("%11d\t",
		   (pdata->mmc_stats.txoctetcount_gb -
		    pdata->mmc_stats.txoctetcount_g));

	mac_printf("\n");

	mac_printf("Tx_Pauseframe             = ");

	mac_printf("%11d\t", pdata->mmc_stats.txpauseframes);

	mac_printf("\n");

	mac_printf("Tx_underflow_error        = ");

	mac_printf("%11d\t", pdata->mmc_stats.txunderflowerror);

	mac_printf("\n");
	return 0;
}
#endif

#if defined(PC_UTILITY) && PC_UTILITY
int xgmac_get_stats_all(void *pdev)
{
	u32 i = 0;
	struct mac_ops *ops;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	for (i = 0; i < pdata->max_mac; i++) {
		ops = gsw_get_mac_ops(0, i);
		mac_printf("XGMAC %d: Reading rmon\n", i);
		xgmac_read_mmc_stats(ops, &prv_data[i].mmc_stats);
	}

	mac_printf("\nTYPE                        ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%18s %d", "XGMAC", i);

	mac_printf("\n");

	mac_printf("Rx_Packets                = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu", prv_data[i].mmc_stats.rxframecount_gb);

	mac_printf("\n");

	mac_printf("Rx_Bytes                  = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu", prv_data[i].mmc_stats.rxoctetcount_gb);

	mac_printf("\n");

	mac_printf("Rx_Byte_errors            = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu",
			   (prv_data[i].mmc_stats.rxoctetcount_gb -
			    prv_data[i].mmc_stats.rxoctetcount_g));

	mac_printf("\n");

	mac_printf("Rx_Pauseframe             = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu", prv_data[i].mmc_stats.rxpauseframes);

	mac_printf("\n");

	mac_printf("Rx_Crc_Errors             = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu", prv_data[i].mmc_stats.rxcrcerror);

	mac_printf("\n");

	mac_printf("Rx_Fifo_Errors            = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu", prv_data[i].mmc_stats.rxfifooverflow);

	mac_printf("\n");
	mac_printf("\n");

	mac_printf("Tx_Packets                = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu", prv_data[i].mmc_stats.txframecount_gb);

	mac_printf("\n");

	mac_printf("Tx_Bytes                  = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu", prv_data[i].mmc_stats.txoctetcount_gb);

	mac_printf("\n");

	mac_printf("Tx_Packet_Errors          = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu",
			   (prv_data[i].mmc_stats.txframecount_gb -
			    prv_data[i].mmc_stats.txframecount_g));

	mac_printf("\n");

	mac_printf("Tx_Byte_Errors            = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu",
			   (prv_data[i].mmc_stats.txoctetcount_gb -
			    prv_data[i].mmc_stats.txoctetcount_g));

	mac_printf("\n");

	mac_printf("Tx_Pauseframe             = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu", prv_data[i].mmc_stats.txpauseframes);

	mac_printf("\n");

	mac_printf("Tx_underflow_error        = ");

	for (i = 0; i < pdata->max_mac; i++)
		mac_printf("%20llu", prv_data[i].mmc_stats.txunderflowerror);

	return 0;
}

int xgmac_get_stats(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct xgmac_mmc_stats *pstats = &pdata->mmc_stats;

	mac_printf("XGMAC %d: Reading rmon\n", pdata->mac_idx);

	xgmac_read_mmc_stats(pdev, pstats);

	mac_printf("\nTYPE                            XGMAC %d\n\n",
		   pdata->mac_idx);
	mac_printf("Rx_Packets                = ");

	mac_printf("%20llu\n", pdata->mmc_stats.rxframecount_gb);
	mac_printf("\n");

	mac_printf("Rx_Bytes                  = ");

	mac_printf("%20llu\n", pdata->mmc_stats.rxoctetcount_gb);
	mac_printf("\n");

	mac_printf("Rx_Byte_errors            = ");

	mac_printf("%20llu\n",
		   (pdata->mmc_stats.rxoctetcount_gb -
		    pdata->mmc_stats.rxoctetcount_g));
	mac_printf("\n");

	mac_printf("Rx_Pauseframe             = ");

	mac_printf("%20llu\n", pdata->mmc_stats.rxpauseframes);
	mac_printf("\n");

	mac_printf("Rx_Crc_Errors             = ");

	mac_printf("%20llu\n", pdata->mmc_stats.rxcrcerror);
	mac_printf("\n");

	mac_printf("Rx_Fifo_Errors            = ");

	mac_printf("%20llu\n", pdata->mmc_stats.rxfifooverflow);
	mac_printf("\n");

	mac_printf("\n");

	mac_printf("Tx_Packets                = ");

	mac_printf("%20llu\n", pdata->mmc_stats.txframecount_gb);
	mac_printf("\n");

	mac_printf("Tx_Bytes                  = ");

	mac_printf("%20llu\n", pdata->mmc_stats.txoctetcount_gb);
	mac_printf("\n");

	mac_printf("Tx_Packet_Errors          = ");

	mac_printf("%20llu\n",
		   (pdata->mmc_stats.txframecount_gb -
		    pdata->mmc_stats.txframecount_g));
	mac_printf("\n");

	mac_printf("Tx_Byte_Errors            = ");

	mac_printf("%20llu\n",
		   (pdata->mmc_stats.txoctetcount_gb -
		    pdata->mmc_stats.txoctetcount_g));
	mac_printf("\n");

	mac_printf("Tx_Pauseframe             = ");

	mac_printf("%20llu\n", pdata->mmc_stats.txpauseframes);
	mac_printf("\n");

	mac_printf("Tx_underflow_error        = ");

	mac_printf("%20llu\n", pdata->mmc_stats.txunderflowerror);
	mac_printf("\n");

	return 0;
}
#endif

#ifdef __KERNEL__
int xgmac_get_stats_all(void *pdev)
{
	u32 i = 0;
	struct mac_ops *ops;
	static char buf[256] = {'\0'};
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	memset((char *)buf, '\0', 256);

	for (i = 0; i < pdata->max_mac; i++) {
		ops = gsw_get_mac_ops(0, i);
		mac_printf("XGMAC %d: Reading rmon\n", i);
		xgmac_read_mmc_stats(ops, &prv_data[i].mmc_stats);
	}

	mac_printf("\nTYPE                        ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%18s %d\t", "XGMAC", i);

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Rx_Packets                = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			prv_data[i].mmc_stats.rxframecount_gb);

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Rx_Bytes                  = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			prv_data[i].mmc_stats.rxoctetcount_gb);

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Rx_Byte_errors            = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			(prv_data[i].mmc_stats.rxoctetcount_gb -
			 prv_data[i].mmc_stats.rxoctetcount_g));

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Rx_Pauseframe             = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			prv_data[i].mmc_stats.rxpauseframes);

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Rx_Crc_Errors             = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			prv_data[i].mmc_stats.rxcrcerror);

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Rx_Fifo_Errors            = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			prv_data[i].mmc_stats.rxfifooverflow);

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("\n");

	mac_printf("Tx_Packets                = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			prv_data[i].mmc_stats.txframecount_gb);

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Tx_Bytes                  = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			prv_data[i].mmc_stats.txoctetcount_gb);

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Tx_Packet_Errors          = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			(prv_data[i].mmc_stats.txframecount_gb -
			 prv_data[i].mmc_stats.txframecount_g));

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Tx_Byte_Errors            = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			(prv_data[i].mmc_stats.txoctetcount_gb -
			 prv_data[i].mmc_stats.txoctetcount_g));

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Tx_Pauseframe             = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			prv_data[i].mmc_stats.txpauseframes);

	mac_printf("%s", buf);

	memset((char *)buf, '\0', 256);

	mac_printf("Tx_underflow_error        = ");

	for (i = 0; i < pdata->max_mac; i++)
		sprintf(buf + strlen(buf), "%20llu\t",
			prv_data[i].mmc_stats.txunderflowerror);

	mac_printf("%s", buf);

	return 0;
}

int xgmac_get_stats(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct xgmac_mmc_stats *pstats = &pdata->mmc_stats;

	mac_printf("XGMAC %d: Reading rmon\n", pdata->mac_idx);

	xgmac_read_mmc_stats(pdev, pstats);

	mac_printf("\nTYPE                        %18s %d\n", "XGMAC", pdata->mac_idx);

	mac_printf("Rx_Packets                = ");

	mac_printf("%20llu\n", pdata->mmc_stats.rxframecount_gb);

	mac_printf("Rx_Bytes                  = ");

	mac_printf("%20llu\n", pdata->mmc_stats.rxoctetcount_gb);

	mac_printf("Rx_Byte_errors            = ");

	mac_printf("%20llu\n",
		   (pdata->mmc_stats.rxoctetcount_gb -
		    pdata->mmc_stats.rxoctetcount_g));

	mac_printf("Rx_Pauseframe             = ");

	mac_printf("%20llu\n", pdata->mmc_stats.rxpauseframes);

	mac_printf("Rx_Crc_Errors             = ");

	mac_printf("%20llu\n", pdata->mmc_stats.rxcrcerror);

	mac_printf("Rx_Fifo_Errors            = ");

	mac_printf("%20llu\n", pdata->mmc_stats.rxfifooverflow);

	mac_printf("\n");

	mac_printf("Tx_Packets                = ");

	mac_printf("%20llu\n", pdata->mmc_stats.txframecount_gb);

	mac_printf("Tx_Bytes                  = ");

	mac_printf("%20llu\n", pdata->mmc_stats.txoctetcount_gb);

	mac_printf("Tx_Packet_Errors          = ");

	mac_printf("%20llu\n",
		   (pdata->mmc_stats.txframecount_gb -
		    pdata->mmc_stats.txframecount_g));

	mac_printf("Tx_Byte_Errors            = ");

	mac_printf("%20llu\n",
		   (pdata->mmc_stats.txoctetcount_gb -
		    pdata->mmc_stats.txoctetcount_g));

	mac_printf("Tx_Pauseframe             = ");

	mac_printf("%20llu\n", pdata->mmc_stats.txpauseframes);

	mac_printf("Tx_underflow_error        = ");

	mac_printf("%20llu\n", pdata->mmc_stats.txunderflowerror);

	return 0;
}
#endif

int xgmac_get_priv_data(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	mac_printf("XGMAC MODULE          = %d\n\n", pdata->mac_idx);
	mac_printf("xgmac_ctrl_reg        = %08x\n", pdata->xgmac_ctrl_reg);
	mac_printf("xgmac_data0_reg       = %08x\n", pdata->xgmac_data0_reg);
	mac_printf("xgmac_data1_reg       = %08x\n", pdata->xgmac_data1_reg);
	mac_printf("tx_q_count            = %d\n", pdata->tx_q_count);
	mac_printf("rx_q_count            = %d\n", pdata->rx_q_count);
	mac_printf("tx_sf_mode            = %d\n", pdata->tx_sf_mode);
	mac_printf("tx_threshold          = %d\n", pdata->tx_threshold);
	mac_printf("rx_sf_mode            = %d\n", pdata->rx_sf_mode);
	mac_printf("rx_threshold          = %d\n", pdata->rx_threshold);
	mac_printf("tx_pause              = %d\n", pdata->tx_pause);
	mac_printf("rx_pause              = %d\n", pdata->rx_pause);
	mac_printf("mac_addr              = %02x:%02x:%02x:%02x:%02x:%02x\n",
		   pdata->mac_addr[0],
		   pdata->mac_addr[1],
		   pdata->mac_addr[2],
		   pdata->mac_addr[3],
		   pdata->mac_addr[4],
		   pdata->mac_addr[5]);
	mac_printf("tstamp_addend         = %d\n", pdata->tstamp_addend);
#if defined(CHIPTEST) && CHIPTEST
	mac_printf("tx_tstamp             = %d\n", pdata->tx_tstamp);
#else
	mac_printf("tx_tstamp             = %llu\n", pdata->tx_tstamp);
#endif
	mac_printf("promisc_mode          = %d\n", pdata->promisc_mode);
	mac_printf("all_mcast_mode        = %d\n", pdata->all_mcast_mode);
	mac_printf("rfa                   = %d\n", pdata->rfa);
	mac_printf("rfd                   = %d\n", pdata->rfd);
	mac_printf("tx_mtl_alg            = %d\n", pdata->tx_mtl_alg);
	mac_printf("rx_mtl_alg            = %d\n", pdata->rx_mtl_alg);
	mac_printf("mtu                   = %d\n", pdata->mtu);
	mac_printf("pause_time            = %d\n", pdata->pause_time);
	mac_printf("rx_checksum_offload   = %d\n",
		   pdata->rx_checksum_offload);
	mac_printf("pause_frm_enable      = %d\n", pdata->pause_frm_enable);
	mac_printf("rmon_reset            = %d\n", pdata->rmon_reset);
	mac_printf("reg_off               = %d\n", pdata->reg_off);
	mac_printf("reg_val               = %d\n", pdata->reg_val);
	mac_printf("sec                   = %d\n", pdata->sec);
	mac_printf("nsec                  = %d\n", pdata->nsec);
	mac_printf("phy_speed             = %d\n", pdata->phy_speed);
	mac_printf("phy_link 			    = %d\n", pdata->phy_link);
	mac_printf("loopback              = %d\n", pdata->loopback);
	mac_printf("twt                   = %d\n", pdata->twt);
	mac_printf("lst                   = %d\n", pdata->lst);
	mac_printf("lpitxa                = %d\n", pdata->lpitxa);
	mac_printf("crc_strip             = %d\n", pdata->crc_strip);
	mac_printf("crc_strip_type        = %d\n", pdata->crc_strip_type);
	mac_printf("padcrc_strip          = %d\n", pdata->padcrc_strip);
	mac_printf("dbg_en                = %d\n", pdata->dbg_en);
	mac_printf("dbg_pktie             = %d\n", pdata->dbg_pktie);
	mac_printf("dbg_rst_sel           = %d\n", pdata->dbg_rst_sel);
	mac_printf("dbg_rst_all           = %d\n", pdata->dbg_rst_all);
	mac_printf("fef                   = %d\n", pdata->fef);
	mac_printf("fup                   = %d\n", pdata->fup);
	return 0;
}

int xgmac_print_hw_cap(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	/* Hardware features of the device */
	struct xgmac_hw_features *hw_feat = &pdata->hw_feat;

	mac_printf("========== Hardware feature register 0 ==============\n");

	if (hw_feat->gmii)
		mac_printf("\t 1Gbps Supported\n");

	if (hw_feat->vlhash)
		mac_printf("\t Enable Address Filter VLAN Hash Table "
			   "option is selected.\n");

	if (hw_feat->sma)
		mac_printf("\t Enable Station Management Block "
			   "(MDIO Interface) option is selected.\n");

	if (hw_feat->rwk)
		mac_printf("\t Enable Remote Wake-Up Packet Detection "
			   "option is selected\n");

	if (hw_feat->mgk)
		mac_printf("\t Enable Magic Packet Detection option "
			   "is selected.\n");

	if (hw_feat->mmc)
		mac_printf("\t Enable XGMAC Management Counter (MMC) option "
			   "is selected.\n");

	if (hw_feat->aoe)
		mac_printf("\t Enable IPv4 ARP Offload option "
			   "is selected\n");

	if (hw_feat->ts)
		mac_printf("\t Enable IEEE 1588 Timestamp Support option "
			   "is selected.\n");

	if (hw_feat->eee)
		mac_printf("\t Enable Energy Efficient Ethernet (EEE) option "
			   "is selected.\n");

	if (hw_feat->tx_coe)
		mac_printf("\t Enable Transmit TCP/IP Checksum Offload option"
			   "is selected.\n");

	if (hw_feat->rx_coe)
		mac_printf("\t Enable Receive TCP/IP Checksum Check option"
			   "is selected.\n");

	if (hw_feat->addn_mac)
		mac_printf("\t Number of additional MAC addresses "
			   "selected = %d\n", hw_feat->addn_mac);

	if (hw_feat->ts_src) {
		if (hw_feat->ts_src == 1)
			mac_printf("\t Time Stamp time source: INTERNAL\n");
		else if (hw_feat->ts_src == 2)
			mac_printf("\t Time Stamp time source: EXTERNAL\n");
		else if (hw_feat->ts_src == 3)
			mac_printf("\t Time Stamp time source: "
				   "INTERNAL & EXTERNAL\n");
	} else {
		mac_printf("\t Time Stamp time source: RESERVED\n");
	}

	if (hw_feat->sa_vlan_ins)
		mac_printf("\t Enable SA and VLAN Insertion on "
			   "Tx option is selected.\n");

	if (hw_feat->vxn)
		mac_printf("\t Support for VxLAN/NVGRE is selected\n");

	if (hw_feat->ediffc)
		mac_printf("\t EDMA mode Separate Memory is selected "
			   "for the Descriptor Cache.\n");

	if (hw_feat->edma)
		mac_printf("\t Enhanced DMA option is selected.\n");

	mac_printf("========== Hardware feature register 1 ==============\n");

	if (hw_feat->rx_fifo_size)
		mac_printf("\t Rx Fifo Size %d:%d bytes\n",
			   hw_feat->rx_fifo_size,
			   (1 << (hw_feat->rx_fifo_size + 7)));

	if (hw_feat->tx_fifo_size)
		mac_printf("\t Tx Fifo Size %d:%d bytes\n",
			   hw_feat->tx_fifo_size,
			   (1 << (hw_feat->tx_fifo_size + 7)));

	if (hw_feat->osten)
		mac_printf("\t One Step Timestamping Enabled\n");

	if (hw_feat->ptoen)
		mac_printf("\t Enable PTP Timestamp Offload Feature is "
			   "selected\n");

	if (hw_feat->adv_ts_hi)
		mac_printf("\t Add IEEE 1588 Higher Word Register option is "
			   "selected.\n");

	if (hw_feat->dma_width == 0)
		mac_printf("\t Dma Width: 32\n");
	else if (hw_feat->dma_width == 1)
		mac_printf("\t Dma Width: 40\n");
	else if (hw_feat->dma_width == 2)
		mac_printf("\t Dma Width: 48\n");
	else if (hw_feat->dma_width == 3)
		mac_printf("\t Dma Width: RESERVED\n");

	if (hw_feat->dcb)
		mac_printf("\t Enable Data Center Bridging option is "
			   "selected.\n");

	if (hw_feat->sph)
		mac_printf("\t Enable Split Header Structure option is "
			   "selected.\n");

	if (hw_feat->tso)
		mac_printf("\t Enable TCP Segmentation Offloading for "
			   "TCP/IP Packets option is selected.\n");

	if (hw_feat->dma_debug)
		mac_printf("\t Enable Debug Memory Access option is "
			   "selected.\n");

	if (hw_feat->rss)
		mac_printf("\t RSS Feature Enabled\n");

	mac_printf("\t Number of traffic classes selected: %d\n",
		   (hw_feat->tc_cnt + 1));

	if (hw_feat->hash_table_size)
		mac_printf("\t Hash table size: %d\n",
			   (64 << (hw_feat->hash_table_size - 1)));
	else
		mac_printf("\t Hash table size: No hash table selected\n");

	if (hw_feat->l3l4_filter_num)
		mac_printf("\t Total number of L3 or L4 filters %d\n",
			   hw_feat->l3l4_filter_num);

	mac_printf("========== Hardware feature register 2 ==============\n");
	mac_printf("\t Number of MTL RX Q: %d\n", (hw_feat->rx_q_cnt + 1));

	mac_printf("\t Number of MTL TX Q: %d\n", (hw_feat->tx_q_cnt + 1));

	if (hw_feat->rx_ch_cnt)
		mac_printf("\t Number of DMA Receive channels: %d\n",
			   (hw_feat->rx_ch_cnt + 1));

	if (hw_feat->tx_ch_cnt)
		mac_printf("\t Number of DMA transmit channels: %d\n",
			   (hw_feat->tx_ch_cnt + 1));

	if (hw_feat->pps_out_num)
		mac_printf("\t Number of PPS outputs %d\n",
			   hw_feat->pps_out_num);

	if (hw_feat->aux_snap_num) {
		if (hw_feat->aux_snap_num > 4)
			mac_printf("\t Number of Auxiliary Snapshot Inputs: "
				   "RESERVED\n");
		else
			mac_printf("\t Number of Auxiliary Snapshot Inputs: "
				   "%d\n", hw_feat->aux_snap_num);
	} else {
		mac_printf("\t No Auxiliary input\n");
	}

	return 0;
}

int xgmac_get_all_hw_settings(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	mac_printf("\n\n\t\t\tGET ALL SETTINGS for XGMAC %d\n",
		   pdata->mac_idx);
	mac_printf("\n");
	xgmac_get_tx_cfg(pdev);
	xgmac_get_counters_cfg(pdev);
	xgmac_get_fifo_size(pdev);
	xgmac_get_flow_control_threshold(pdev);
	xgmac_get_mtl_rx_flow_ctl(pdev);
	xgmac_get_mtl_tx(pdev);
	xgmac_get_mtl_rx(pdev);
	xgmac_get_mtl_q_alg(pdev);

	xgmac_get_crc_settings(pdev);

	xgmac_get_eee_settings(pdev);
	xgmac_dbg_eee_status(pdev);

	xgmac_get_mac_settings(pdev);
	xgmac_get_mtu_settings(pdev);
	xgmac_get_checksum_offload(pdev);
	xgmac_get_mac_addr(pdev);
	xgmac_get_mac_rx_mode(pdev);
	xgmac_get_rx_vlan_filtering_mode(pdev);
	xgmac_get_mac_speed(pdev);
	xgmac_get_pause_frame_ctl(pdev);
	xgmac_get_mac_loopback_mode(pdev);
	xgmac_get_tstamp_settings(pdev);
	xgmac_get_debug_sts(pdev);
	xgmac_dbg_int_sts(pdev);
	xgmac_get_fup_fep_setting(pdev);
	xgmac_get_ipg(pdev);
	xgmac_dbg_pmt(pdev);
	xgmac_get_mac_rxtx_sts(pdev);
	xgmac_get_mtl_missed_pkt_cnt(pdev);

	return 0;
}

