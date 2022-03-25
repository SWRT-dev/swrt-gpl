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

/*  RSF: Receive Q store and forward
 *  if 1, XGMAC reads packet from Rx Q after the complete packet has been
 *  written
 *  if 0, Rx Queue operates in threshold mode,
 *  The received packet is transferred to the application when the
 *  packet size within the MTL Rx queue is larger than the threshold
 */
int xgmac_set_mtl_rx_mode(void *pdev, u32 rx_mode)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MTL_Q_RQOMR);

	if (MAC_GET_VAL(reg_val, MTL_Q_RQOMR, RSF) != rx_mode) {
		mac_dbg("XGMAC %d: Setting MTL RX mode to: %s\n",
			pdata->mac_idx,
			rx_mode ? "Store and Forward" : "Threshold");
		MAC_SET_VAL(reg_val, MTL_Q_RQOMR, RSF, rx_mode);
		XGMAC_RGWR(pdata, MTL_Q_RQOMR, reg_val);
	}

	return 0;
}

int xgmac_set_mtl_rx_thresh(void *pdev, u32 rx_thresh)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MTL_Q_RQOMR);

	if (MAC_GET_VAL(reg_val, MTL_Q_RQOMR, RTC) != rx_thresh) {
		mac_dbg("XGMAC %d: Setting MTL RX threshold to: %d\n",
			pdata->mac_idx,  rx_thresh);
		MAC_SET_VAL(reg_val, MTL_Q_RQOMR, RTC, rx_thresh);
		XGMAC_RGWR(pdata, MTL_Q_RQOMR, reg_val);
	}

	return 0;
}

/*  TSF: Transmit Q Store and Forward
 *  if 1, the transmission starts when a full packet resides in the MTL Tx queue
 *  When this bit is set, the TX threshold value specified in the register are
 *  ignored.
 *  if 0, Tx Queue operates in threshold mode,
 *  This bit should be changed only when the transmission is stopped.
 */
int xgmac_set_mtl_tx_mode(void *pdev, u32 tx_mode)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MTL_Q_TQOMR);

	if (MAC_GET_VAL(reg_val, MTL_Q_TQOMR, TSF) != tx_mode) {
		mac_dbg("XGMAC %d: Setting MTL TX mode to: %s\n",
			pdata->mac_idx,
			tx_mode ? "Store and Forward" : "Threshold");
		MAC_SET_VAL(reg_val, MTL_Q_TQOMR, TSF, tx_mode);
		XGMAC_RGWR(pdata, MTL_Q_TQOMR, reg_val);
	}

	return 0;
}

int xgmac_set_mtl_tx_thresh(void *pdev, u32 tx_thresh)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MTL_Q_TQOMR);

	if (MAC_GET_VAL(reg_val, MTL_Q_TQOMR, TTC) != tx_thresh) {
		mac_dbg("XGMAC %d: Setting MTL TX threshold to: %d\n",
			pdata->mac_idx,  tx_thresh);
		MAC_SET_VAL(reg_val, MTL_Q_TQOMR, TTC, tx_thresh);
		XGMAC_RGWR(pdata, MTL_Q_TQOMR, reg_val);
	}

	return 0;
}

/* Clear all the MTL Q interrupts */
int xgmac_clear_mtl_int(void *pdev, u32 event)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mtl_q_isr = XGMAC_RGRD(pdata, MTL_Q_ISR);

	if ((event & XGMAC_TXQ_OVFW_EVNT) &&
	    (MAC_GET_VAL(mtl_q_isr, MTL_Q_ISR, TXUNFIS))) {
		mac_dbg("XGMAC: %d Clearing MTL Q TXUNFIS"
			"Interrupt Status\n", pdata->mac_idx);
		MAC_SET_VAL(mtl_q_isr, MTL_Q_ISR, TXUNFIS, 1);
	}

	if ((event & XGMAC_RXQ_OVFW_EVNT) &&
	    (MAC_GET_VAL(mtl_q_isr, MTL_Q_ISR, ABPSIS))) {
		mac_dbg("XGMAC: %d Clearing MTL Q ABPSIS"
			"Interrupt Status\n", pdata->mac_idx);
		MAC_SET_VAL(mtl_q_isr, MTL_Q_ISR, ABPSIS, 1);
	}

	if ((event & XGMAC_AVG_BPS_EVNT) &&
	    (MAC_GET_VAL(mtl_q_isr, MTL_Q_ISR, RXOVFIS))) {
		mac_dbg("XGMAC: %d Clearing MTL Q RXOVFIS"
			"Interrupt Status\n", pdata->mac_idx);
		MAC_SET_VAL(mtl_q_isr, MTL_Q_ISR, RXOVFIS, 1);
	}

	XGMAC_RGWR(pdata, MTL_Q_ISR, mtl_q_isr);

	return 0;
}

/* Enable and disable all the MTL Q interrupts */
int xgmac_set_mtl_int(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mtl_q_isr;

	mtl_q_isr = XGMAC_RGRD(pdata, MTL_Q_IER);

	/* Tx Q Overflow Interrupt Enable */
	if (val & MASK(MTL_Q_IER, TXUIE))
		MAC_SET_VAL(mtl_q_isr, MTL_Q_IER, TXUIE, val);

	/* Average bits per slot interrupt enable */
	if (val & MASK(MTL_Q_IER, ABPSIE))
		MAC_SET_VAL(mtl_q_isr, MTL_Q_IER, ABPSIE, val);

	/* Rx Q Overflow Interrupt Enable */
	if (val & MASK(MTL_Q_IER, RXOIE))
		MAC_SET_VAL(mtl_q_isr, MTL_Q_IER, RXOIE, val);

	XGMAC_RGWR(pdata, MTL_Q_IER, mtl_q_isr);
	return 0;
}

/* SNPVER: Synopsys-defined Version
 * FTQ:
 *	if 1, the Tx Queue Controller logic is reset to its default values.
 *	Therefore,  all the data in the Tx queue is lost or flushed.
 *	This bit is internally reset when the flushing operation is complete.
 *	You should not write to this register until this bit is reset.
 *	The data which is already accepted by the MAC transmitter is not
 *	flushed. It is scheduled for transmission and results in underflow
 *	and runt packet transmission.
 *	Note: The flush operation is complete only when the Tx queue is empty.
 *	To complete this flush operation, the PHY Tx clock must be active.
 */
int xgmac_flush_tx_queues(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 ftq = 0;
	int idx = 0;

	mac_dbg("XGMAC %d: Flushing TX Q\n", pdata->mac_idx);

	XGMAC_RGWR_BITS(pdata, MTL_Q_TQOMR, FTQ, 1);

	do {
		ftq = XGMAC_RGRD_BITS(pdata, MTL_Q_TQOMR, FTQ);

		if (ftq == 0)
			break;

		idx++;

#ifdef __KERNEL__
		/* To put a small delay and make sure previous operations
		 * are complete
		 */
		usleep_range(100, 200);
#endif
	} while (idx <= MAX_RETRY);

	return 0;
}

/*RQS: Receive Queue Size
 *RFA:
 *	Threshold for activating Flow Control
 *	These bits control the threshold (fill-level of RxQ0) at which the flow
 *	control is activated
 *	The flow control is triggered when EHFC=1 and RQS >= 4KB
 *RFD:
 *	Threshold for deactivating Flow Control
 *	These bits control the threshold (fill-level of RxQ0) at which the flow
 *	control is de-asserted once it is activated.
 */
int xgmac_set_flow_control_threshold(void *pdev, u32 rfa, u32 rfd)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MTL_Q_RQFCR);

	/* Activate flow control when less than 4k left in fifo */
	if (MAC_GET_VAL(reg_val, MTL_Q_RQFCR, RFA) != rfa) {
		mac_dbg("XGMAC %d: Set Thresh for activate Flow Ctrl %d\n",
			pdata->mac_idx, rfa);
		MAC_SET_VAL(reg_val, MTL_Q_RQFCR, RFA, rfa);
	}

	/* De-activate flow control when more than 6k left in fifo */
	if (MAC_GET_VAL(reg_val, MTL_Q_RQFCR, RFD) != rfd) {
		mac_dbg("XGMAC %d: Set Thresh for deact Flow Ctrl as %d\n",
			pdata->mac_idx, rfd);
		MAC_SET_VAL(reg_val, MTL_Q_RQFCR, RFD, rfd);
	}

	XGMAC_RGWR(pdata, MTL_Q_RQFCR, reg_val);

	return 0;
}

/* MMC_Control
 * The MMC Control register establishes the operating mode of the management
 * counters
 * ROR:
 * When this bit is set, the MMC counters are reset to zero after a read. The
 * counters are cleared when the least significant byte lane (bits[7:0]) is read
 * When the MMC module is enabled for the 64-bit counter mode, the lower and
 * upper counters are cleared only when the least significant byte lane
 * (bits[7:0]) of the corresponding counter is read.
 * CR:
 * Counters Reset
 * When this bit is set, all counters are reset.
 * This bit is cleared automatically after 1 clock cycle.
 */
int xgmac_set_mmc(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MMC_CR);

	/* Set counters to reset on read */
	if (MAC_GET_VAL(reg_val, MMC_CR, ROR) != 0) {
		mac_dbg("XGMAC %d: reset on read %s\n",
			pdata->mac_idx, "DISABLED");
		MAC_SET_VAL(reg_val, MMC_CR, ROR, 0);
		XGMAC_RGWR(pdata, MMC_CR, reg_val);
	}

	return 0;
}

int xgmac_clear_rmon(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	/* Reset the counters */
	mac_printf("XGMAC %d: Resetting the counters\n",
		   pdata->mac_idx);
	XGMAC_RGWR_BITS(pdata, MMC_CR, CR, 1);
	memset(&prv_data[pdata->mac_idx].mmc_stats, 0,
	       sizeof(struct xgmac_mmc_stats));

	while (1) {
		if (XGMAC_RGRD_BITS(pdata, MMC_CR, CR) == 0)
			break;
	}

	return 0;
}

/* GB - Good Bad frames
 * G  - Good frames
 */
#if defined(CHIPTEST) && CHIPTEST
u32 xgmac_mmc_read(void *pdev, u32 reg_lo)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 val_lo = 0, val_hi = 0;
	u32 val = 0;

	val_lo = XGMAC_RGRD(pdata, reg_lo);
	val_hi = XGMAC_RGRD(pdata, reg_lo + 4);

	val = ((u64)val_hi << 32) | val_lo;

	return val;
}
#else
u64 xgmac_mmc_read(void *pdev, u32 reg_lo)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 val_lo, val_hi;
	u64 val;

	val_lo = XGMAC_RGRD(pdata, reg_lo);
	val_hi = XGMAC_RGRD(pdata, reg_lo + 4);

	val = ((u64)val_hi << 32) | val_lo;

	return val;
}
#endif

int xgmac_read_mmc_stats(void *pdev, struct xgmac_mmc_stats *stats)
{
	stats->txoctetcount_gb =
		xgmac_mmc_read(pdev, MMC_TXOCTETCOUNT_GB_LO);

	stats->txframecount_gb =
		xgmac_mmc_read(pdev, MMC_TXFRAMECOUNT_GB_LO);

	stats->txunderflowerror =
		xgmac_mmc_read(pdev, MMC_TXUNDERFLOWERROR_LO);

	stats->txoctetcount_g =
		xgmac_mmc_read(pdev, MMC_TXOCTETCOUNT_G_LO);

	stats->txframecount_g =
		xgmac_mmc_read(pdev, MMC_TXFRAMECOUNT_G_LO);

	stats->txpauseframes =
		xgmac_mmc_read(pdev, MMC_TXPAUSEFRAMES_LO);

	stats->rxframecount_gb =
		xgmac_mmc_read(pdev, MMC_RXFRAMECOUNT_GB_LO);

	stats->rxoctetcount_gb =
		xgmac_mmc_read(pdev, MMC_RXOCTETCOUNT_GB_LO);

	stats->rxoctetcount_g =
		xgmac_mmc_read(pdev, MMC_RXOCTETCOUNT_G_LO);

	stats->rxcrcerror =
		xgmac_mmc_read(pdev, MMC_RXCRCERROR_LO);

	stats->rxpauseframes =
		xgmac_mmc_read(pdev, MMC_RXPAUSEFRAMES_LO);

	stats->rxfifooverflow =
		xgmac_mmc_read(pdev, MMC_RXFIFOOVERFLOW_LO);

	return 0;
}

/* DBGMOD: Debug Mode Access to FIFO
 *	if 1, it indicates that the current access to the FIFO is
 *	debug access. Data transfers from Tx FIFO to MAC Transmitter and
 *	from MAC Receiver to Rx FIFO is disabled. In this mode, the
 *following access types are allowed:
 *	Direct Read and Write access to all locations of Tx FIFO,
 *	Descriptor Cache and Rx FIFO memories.
 *FDBGEN: FIFO Debug Access Enable
 * if 1, it indicates that the debug/slave mode access to the FIFO is enabled.
 * if 0, it indicates that the FIFO can be accessed only through a master
 * interface (ATI, ARI, DMA).
 *PKTSTATE: Write Control Type
 *	This field is used to decide the tag bits along with the data being
 *	written into the FIFO.
 *	When the TxFIFO is written in Slave mode or Debug mode, this field
 *	marks the type of data written as follows:
 *	00 Packet Data
 *	01 Control Word
 *	10 SOP Data
 *	11 EOP Data
 *	When the Rx FIFO is being written in slave mode, this field marks the
 *	type of data written as follows:
 *	00 Packet Data
 *	01 Normal Status
 *	10 Last Status
 *	11 EOP
 *FIFOSEL: FIFO Selected for Access
 *	This field indicates the FIFO selected for debug access:
 *	00 Tx FIFO
 *	01 Descriptor cache (only when DBGMOD=1)
 *	10 TSO FIFO (only when DBGMOD = 1)
 *	11 Rx FIFO
 *BYTEEN: Write Data Byte Enable
 *	This field indicates the number of data bytes valid in the data register
 *	during Write operation. This is valid only when PKTSTATE is 2'b11
 *	(EOP) and Tx FIFO or Rx FIFO is selected.
 *	00 Byte 0 valid
 *	01 Byte 0 and Byte 1 are valid
 *	10 Byte 0, Byte 1, and Byte 2 are valid
 *	11 All four bytes are valid
 *FIFOWREN: FIFO Write Enable
 *		if 1, it enables the Write operation on selected FIFO.
 *FIFORDEN: FIFO Read Enable
 *		if 1, it enables the Read operation on selected FIFO.
 *PKTIE:  Receive Packet Available Interrupt Status Enable
 *		if 1, an interrupt is generated when EOP of received
 *		packet is written by the MAC to the Rx FIFO.
 *RSTSEL: Reset Pointers of Selected FIFO
 *		if 1, the pointers of the currently-selected FIFO are
 *		reset.
 *RSTALL: Reset All Pointers
 *		if 1, the pointers of all FIFOs are reset.
 */

int xgmac_set_debug_ctl(void *pdev, u32 dbg_en, u32 dbg_mode)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 dbg_ctl = XGMAC_RGRD(pdata, MTL_DBG_CTL);

	u32 enable = dbg_en ? 1 : 0;
	u32 mode = dbg_mode ? 1 : 0;

	if (enable) {
		if (mode) {	/* Debug Mode */
			MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, FDBGEN, enable);
			MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, DBGMOD, enable);
		} else {	/* Slave Mode */
			MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, FDBGEN, enable);
			MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, DBGMOD, 0);
		}
	} else {
		MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, FDBGEN, 0);
		MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, DBGMOD, 0);
	}

	XGMAC_RGWR(pdata, MTL_DBG_CTL, dbg_ctl);
	return 0;
}

int xgmac_set_debug_data(void *pdev, u32 dbg_data)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	XGMAC_RGWR(pdata, MTL_DBG_DAT, dbg_data);
	return 0;
}

int xgmac_tx_debug_data(void *pdev, u32 dbg_pktstate)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 dbg_ctl = XGMAC_RGRD(pdata, MTL_DBG_CTL);

	MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, PKTSTATE, dbg_pktstate);

	/* Set Fifo Sel to TX FIFO */
	MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, FIFOSEL, 0);
	/* Enables write on TX FIFO */
	MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, FIFOWREN, 1);

	if (dbg_pktstate == TX_EOP_DATA)
		MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, BYTEEN, 3);

	XGMAC_RGWR(pdata, MTL_DBG_CTL, dbg_ctl);

	return 0;
}

int xgmac_rx_debug_data(void *pdev, u32 *dbg_pktstate, u32 *dbg_byteen)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 dbg_ctl = XGMAC_RGRD(pdata, MTL_DBG_CTL);

	/* Set Fifo Sel to RX FIFO */
	MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, FIFOSEL, 3);
	/* Enables write on RX FIFO */
	MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, FIFORDEN, 1);

	XGMAC_RGWR(pdata, MTL_DBG_CTL, dbg_ctl);

	while (XGMAC_RGRD_BITS(pdata, MTL_DBG_STS, FIFOBUSY) == 1);

	dbg_ctl = XGMAC_RGRD(pdata, MTL_DBG_CTL);

	*dbg_pktstate = MAC_GET_VAL(dbg_ctl, MTL_DBG_CTL, PKTSTATE);

	if (*dbg_pktstate == RX_EOP_DATA)
		*dbg_byteen = XGMAC_RGRD_BITS(pdata, MTL_DBG_STS, BYTEEN);

	return 0;
}

int xgmac_set_rx_debugctrl_int(void *pdev, u32 dbg_pktie)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	XGMAC_RGWR_BITS(pdata, MTL_DBG_CTL, PKTIE, dbg_pktie);
	return 0;
}

int xgmac_set_fifo_reset(void *pdev, u32 dbg_rst_sel, u32 dbg_rst_all)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 dbg_ctl = XGMAC_RGRD(pdata, MTL_DBG_CTL);

	MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, RSTSEL, dbg_rst_sel);
	MAC_SET_VAL(dbg_ctl, MTL_DBG_CTL, RSTALL, dbg_rst_all);

	XGMAC_RGWR(pdata, MTL_DBG_CTL, dbg_ctl);
	return 0;
}

int xgmac_poll_fifo_sts(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	while (1) {
		if (XGMAC_RGRD_BITS(pdata, MTL_DBG_STS, FIFOBUSY) == 0)
			break;
	}

	return 0;
}

int xgmac_poll_pkt_rx(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	while (1) {
		if (XGMAC_RGRD_BITS(pdata, MTL_DBG_STS, PKTI) == 1)
			break;
	}

	return 0;
}

int xgmac_clr_debug_sts(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	/* Clear the debug interrupt status */
	XGMAC_RGWR_BITS(pdata, MTL_DBG_STS, PKTI, 1);

	return 0;
}

/* FEF: Forward Error Packets
 *	if 0, the Rx queue drops packets with error status (CRC
 *	error, GMII_ER, watchdog timeout, or overflow).
 *	if 1, all packets except the runt error packets are
 *	forwarded to the application or DMA.
 * FUP: Forward Undersized Good Packets
 *	if 1, the Rx queue forwards the undersized good packets
 *	(with no error and length less than 64 bytes), including pad-bytes and
 *	CRC. if 0, the Rx queue drops all packets of less than 64 bytes.
 */

int xgmac_forward_fup_fep_pkt(void *pdev, u32 fup, u32 fef)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MTL_Q_RQOMR);

	if (MAC_GET_VAL(reg_val, MTL_Q_RQOMR, FUP) != fup) {
		mac_dbg("XGMAC %d: Set Forward Undersized Good Packets\n",
			pdata->mac_idx);
		MAC_SET_VAL(reg_val, MTL_Q_RQOMR, FUP, fup);
	}

	if (MAC_GET_VAL(reg_val, MTL_Q_RQOMR, FEF) != fef) {
		mac_dbg("XGMAC %d: Set Forward Error Packets\n",
			pdata->mac_idx);
		MAC_SET_VAL(reg_val, MTL_Q_RQOMR, FEF, fef);
	}

	XGMAC_RGWR(pdata, MTL_Q_RQOMR, reg_val);

	return 0;
}

