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
#include <mac_cfg.h>
#ifdef __KERNEL__
#include <xgmac_ptp.h>
#include <net/switch_api/gsw_irq.h>
#else
#include <gsw_irq.h>
#endif

/* LM: Loopback Mode
 * When this bit is set, the MAC operates in the loopback mode at GMII or
 * XGMII. The (X)GMII Rx clock input (clk_rx_312pt5_i) is required for the
 * loopback to work properly. This is because the Tx clock is not internally
 * looped back.
 */
int xgmac_set_loopback(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_RX_CFG);

	if (MAC_GET_VAL(reg_val, MAC_RX_CFG, LM) != val) {
		mac_dbg("XGMAC %d: LOOPBACK mode: %s\n",
			pdata->mac_idx,  val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(reg_val, MAC_RX_CFG, LM, val);
		XGMAC_RGWR(pdata, MAC_RX_CFG, reg_val);
	}

	return 0;
}

/* TFE: Transmit Flow Control Enable
 * When this bit is set, the MAC enables the flow control operation based on
 * Tx Pause packets.
 * When this bit is reset, the flow control operation in the
 * MAC is disabled, and the MAC does not transmit any Pause packets by
 * itself
 * RFE: Receive Flow Control Enable
 * When this bit is set and the MAC is operating in full-duplex mode, the MAC
 * decodes the received Pause packet and disables its transmitter for a
 * specified (Pause) time. When this bit is reset, the decode function of the
 * Pause packet is disabled.
 * PFCE: Priority Based Flow Control Enable
 * When 1, it enables generation and reception of priority-based flow control
 * (PFC) packets.
 * When 0, it enables generation and reception of 802.3x Pause control packets.
 * This bit is reserved when the Enable Data Center Bridging option is not
 * selected.
 */
int xgmac_disable_tx_flow_ctl(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_TX_FCR);

	if (MAC_GET_VAL(reg_val, MAC_TX_FCR, TFE) != 0) {
		MAC_SET_VAL(reg_val, MAC_TX_FCR, TFE, 0);
		XGMAC_RGWR(pdata, MAC_TX_FCR, reg_val);
	}

	return 0;
}

int xgmac_disable_rx_flow_ctl(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_RX_FCR);

	if (MAC_GET_VAL(reg_val, MAC_RX_FCR, RFE) != 0) {
		mac_dbg("XGMAC %d: Disable RX Flow Control\n",
			pdata->mac_idx);
		MAC_SET_VAL(reg_val, MAC_RX_FCR, RFE, 0);
		XGMAC_RGWR(pdata, MAC_RX_FCR, reg_val);
	}

	return 0;
}

int xgmac_enable_tx_flow_ctl(void *pdev, u32 pause_time)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_TX_FCR);

	if (MAC_GET_VAL(reg_val, MAC_TX_FCR, TFE) != 1) {
		mac_dbg("XGMAC %d: Enable TX Flow Control\n",
			pdata->mac_idx);
		/* Enable transmit flow control */
		MAC_SET_VAL(reg_val, MAC_TX_FCR, TFE, 1);
		/* Set pause time */
		MAC_SET_VAL(reg_val, MAC_TX_FCR, PT, pause_time);

		XGMAC_RGWR(pdata, MAC_TX_FCR, reg_val);
	}

	return 0;
}

int xgmac_enable_rx_flow_ctl(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_RX_FCR);

	if (MAC_GET_VAL(reg_val, MAC_RX_FCR, RFE) != 1) {
		mac_dbg("XGMAC %d: Enable RX Flow Control\n",
			pdata->mac_idx);
		/* Enable Receive flow control */
		MAC_SET_VAL(reg_val, MAC_RX_FCR, RFE, 1);
		// TODO: Need to check whether this is needed
		MAC_SET_VAL(reg_val, MAC_RX_FCR, PFCE, 0);

		XGMAC_RGWR(pdata, MAC_RX_FCR, reg_val);
	}

	return 0;
}

/* FCB: Flow Control Busy
 * This bit initiates a Pause packet if the TFE bit is set. To initiate a Pause
 * packet, the application must set this bit to 1'b1. During Control packet
 * transfer, this bit continues to be set to indicate that a packet
 * transmission is in progress. When Pause packet transmission is complete,
 * the MAC resets this bit to 1'b0. You should not write to this register
 * until this bit is cleared
 */
int xgmac_initiate_pause_tx(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val, idx = 0;

	reg_val = XGMAC_RGRD(pdata, MAC_TX_FCR);

	if (MAC_GET_VAL(reg_val, MAC_TX_FCR, TFE) == 0) {
		mac_dbg("XGMAC %d: Pause pkt txd only if TFE bit is set\n",
			pdata->mac_idx);
		return 0;
	}

	MAC_SET_VAL(reg_val, MAC_TX_FCR, FCB, 1);

	/* Initiate Pause TX */
	XGMAC_RGWR(pdata, MAC_TX_FCR, reg_val);

	do {
		reg_val = XGMAC_RGRD(pdata, MAC_TX_FCR);

		if (MAC_GET_VAL(reg_val, MAC_TX_FCR, FCB) == 0) {
			mac_dbg("XGMAC %d: Pause Pkt Txd complete\n",
				pdata->mac_idx);
			break;
		}


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

/* Clear all the MAC interrupts */
int xgmac_clear_mac_int(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_isr;

	/* Clear all the interrupts which are set */
	mac_isr = XGMAC_RGRD(pdata, MAC_ISR);
	mac_dbg("XGMAC %d Clearing MAC ISR registers with %08x\n",
		pdata->mac_idx, mac_isr);
	XGMAC_RGWR(pdata, MAC_ISR, mac_isr);
	return 0;
}

int xgmac_set_mac_int(void *pdev, u32 event, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 mac_ier = XGMAC_RGRD(pdata, MAC_IER);
	u32 mtl_q_ier = XGMAC_RGRD(pdata, MTL_Q_IER);

	switch (event) {
	/* Enable Timestamp interrupt */
	case XGMAC_TSTAMP_EVNT:
		MAC_SET_VAL(mac_ier, MAC_IER, TSIE, val);
		break;

	/* Enable LPI interrupt (EEE) */
	case XGMAC_LPI_EVNT:
		MAC_SET_VAL(mac_ier, MAC_IER, LPIIE, val);
		break;

	/* Enable transmit error status interrupt */
	case XGMAC_TXERR_STS_EVNT:
		MAC_SET_VAL(mac_ier, MAC_IER, TXESIE, val);
		break;

	/* Enable Receive error status interrupt */
	case XGMAC_RXERR_STS_EVNT:
		MAC_SET_VAL(mac_ier, MAC_IER, RXESIE, val);
		break;

	/* Enable power management interrupt */
	case XGMAC_PMT_EVNT:
		MAC_SET_VAL(mac_ier, MAC_IER, PMTIE, val);
		break;

	/* Tx Q Overflow Interrupt Enable */
	case XGMAC_TXQ_OVFW_EVNT:
		MAC_SET_VAL(mtl_q_ier, MTL_Q_IER, TXUIE, val);
		break;

	/* Rx Q Overflow Interrupt Enable */
	case XGMAC_RXQ_OVFW_EVNT:
		MAC_SET_VAL(mtl_q_ier, MTL_Q_IER, ABPSIE, val);
		break;

	/* Average bits per slot interrupt enable */
	case XGMAC_AVG_BPS_EVNT:
		MAC_SET_VAL(mtl_q_ier, MTL_Q_IER, RXOIE, val);
		break;

	/* Interrupt Enable All */
	case XGMAC_ALL_EVNT:
		mac_ier = 0xFFFFFFFF;
		mtl_q_ier = 0xFFFFFFFF;
		break;

	default:
		mac_dbg("Unsupported Mac Event\n");
		return GSW_statusErr;
	}

	XGMAC_RGWR(pdata, MAC_IER, mac_ier);
	XGMAC_RGWR(pdata, MTL_Q_IER, mtl_q_ier);

	return 0;
}

/* USS = 1
 * SS = 0 10G (USXGMII mode)
 * SS = 1 5G
 * SS = 2 2.5G using XGMII
 * SS = 3 Reserved
 * USS = 0
 * SS = 0 10G using XGMII
 * SS = 2 2.5G using GMII
 * SS = 3 1G using GMII
 */
int xgmac_set_gmii_speed(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tcr = XGMAC_RGRD(pdata, MAC_TX_CFG);

	xgmac_powerdown(pdev);

	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, USS) != 0)
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, USS, 0);

	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, SS) != 0x3) {
		mac_dbg("XGMAC %d: Setting SPEED to GMII 1G\n",
			pdata->mac_idx);
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, SS, 0x3);
	}

	XGMAC_RGWR(pdata, MAC_TX_CFG, mac_tcr);

	xgmac_powerup(pdev);

	return 0;
}

int xgmac_set_gmii_2500_speed(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tcr = XGMAC_RGRD(pdata, MAC_TX_CFG);

	xgmac_powerdown(pdev);

	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, USS) != 0)
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, USS, 0);

	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, SS) != 0x2) {
		mac_dbg("XGMAC %d: Setting SPEED to GMII 2.5G\n",
			pdata->mac_idx);
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, SS, 0x2);
	}

	XGMAC_RGWR(pdata, MAC_TX_CFG, mac_tcr);

	xgmac_powerup(pdev);

	return 0;
}

int xgmac_set_xgmii_2500_speed(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tcr = XGMAC_RGRD(pdata, MAC_TX_CFG);

	xgmac_powerdown(pdev);

	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, USS) != 1)
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, USS, 1);

	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, SS) != 0x2) {
		mac_dbg("XGMAC %d: Setting SPEED to XGMII 2.5G\n",
			pdata->mac_idx);
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, SS, 0x2);
	}

	XGMAC_RGWR(pdata, MAC_TX_CFG, mac_tcr);

	xgmac_powerup(pdev);

	return 0;
}

int xgmac_set_xgmii_speed(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tcr = XGMAC_RGRD(pdata, MAC_TX_CFG);

	xgmac_powerdown(pdev);

	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, USS) != 0)
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, USS, 0);

	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, SS) != 0) {
		mac_dbg("XGMAC %d: Setting SPEED to XGMII 10G\n",
			pdata->mac_idx);
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, SS, 0);
	}

	XGMAC_RGWR(pdata, MAC_TX_CFG, mac_tcr);

	xgmac_powerup(pdev);

	return 0;
}

int xgmac_pause_frame_filtering(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_pfr = XGMAC_RGRD(pdata, MAC_PKT_FR);

	if (MAC_GET_VAL(mac_pfr, MAC_PKT_FR, PCF) != val) {
		mac_dbg("XGMAC %d: Pause filtering %s\n",
			pdata->mac_idx, val ? "Enabled" : "Disabled");
		/* Pause filtering */
		MAC_SET_VAL(mac_pfr, MAC_PKT_FR, PCF, val);
	}

	if (MAC_GET_VAL(mac_pfr, MAC_PKT_FR, RA) == 1) {
		mac_dbg("XGMAC %d: MAC Filter Receive All: DISABLED\n",
			pdata->mac_idx);
		MAC_SET_VAL(mac_pfr, MAC_PKT_FR, RA, 0);
	}

	XGMAC_RGWR(pdata, MAC_PKT_FR, mac_pfr);

	return 0;
}

/* MAC_PKT_FR:
 * The MAC Packet Filter register contains the filter controls for receiving
 * packets. Some of the controls from
 * this register go to the address check block of the MAC which performs the
 * first level of address filtering. The
 * second level of filtering is performed on the incoming packet based on other
 * controls such as Pass Bad Packets and Pass Control Packets.
 * PR:Promiscuous Mode
 * When this bit is set, the Address Filtering module passes all incoming
 * packets irrespective of the destination or source address.
 * The SA or DA Filter Fails status bits of the Rx Status Word are always
 * cleared when PR is set.
 */
int xgmac_set_promiscuous_mode(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MAC_PKT_FR);

	if (MAC_GET_VAL(reg_val, MAC_PKT_FR, PR) != val) {
		mac_dbg("XGMAC %d: %s promiscuous mode\n",
			pdata->mac_idx,  val ? "Entering" : "Leaving");
		MAC_SET_VAL(reg_val, MAC_PKT_FR, PR, val);

		XGMAC_RGWR(pdata, MAC_PKT_FR, reg_val);
	}

	return 0;
}

/* PM:Pass All Multicast
 * When this bit is set, it indicates that all received packets with a
 * multicast destination address (first bit in the destination address
 * field is '1') are passed. When this bit is reset, filtering of
 * multicast packet depends on HMC bit.
 */
int xgmac_set_all_multicast_mode(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MAC_PKT_FR);

	if (MAC_GET_VAL(reg_val, MAC_PKT_FR, PM) != val) {
		MAC_SET_VAL(reg_val, MAC_PKT_FR, PM, val);
		mac_dbg("XGMAC %d: %s allmulti mode\n",
			pdata->mac_idx,  val ? "Entering" : "Leaving");
		XGMAC_RGWR(pdata, MAC_PKT_FR, reg_val);
	}

	return 0;
}

/* The MAC Address0 High register
 * This field contains the upper 16 bits [47:32] of the first 6-byte MAC
 * address. The MAC uses this field for filtering the received packets
 * and inserting the MAC address in the Transmit Flow Control (Pause)
 * Packets.
 * The MAC Address0 Low register
 * This field contains the lower 32 bits of the first 6-byte MAC address.
 * The MAC uses this field for filtering the received packets and inserting
 * the MAC address in the Transmit Flow Control (Pause) Packets.
 */
int xgmac_set_mac_address(void *pdev, u8 *mac_addr)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_addr_hi = 0, mac_addr_lo = 0;

	mac_addr_hi = (mac_addr[5] <<  8) | (mac_addr[4] <<  0);
	mac_addr_lo = (mac_addr[3] << 24) | (mac_addr[2] << 16) |
		      (mac_addr[1] <<  8) | (mac_addr[0] <<  0);

	/* Since 16 bits only need to check, not checking the previous value */
	XGMAC_RGWR(pdata, MAC_MACA0HR, mac_addr_hi);

	if (XGMAC_RGRD(pdata, MAC_MACA0LR) != mac_addr_lo)
		XGMAC_RGWR(pdata, MAC_MACA0LR, mac_addr_lo);

	return 0;
}

/* Rx Checksum Offload Engine
 * Both IPv4 and IPv6 packet in the received Ethernet packets are detected
 * and processed for data integrity. The MAC receiver identifies IPv4 or
 * IPv6 packets by checking for value 0x0800 or 0x86DD,
 * respectively, in the Type field of the received Ethernet packet.
 * The Rx Checksum Offload Engine calculates the IPv4 header checksums
 * and checks that they match the received
 * IPv4 header checksums. The result of this operation (pass or fail) is
 * given to the RFC module for insertion into the receive status word.
 * IPC:
 * if 1, this bit enables the IPv4 header checksum checking and IPv4
 * or IPv6 TCP, UDP, or ICMP payload checksum checking.
 * if 0, the COE function in the receiver is disabled.
 */

int xgmac_set_checksum_offload(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MAC_RX_CFG);

	if (MAC_GET_VAL(reg_val, MAC_RX_CFG, IPC) != val) {
		mac_dbg("XGMAC %d: Setting rx_checksum_offload as %s\n",
			pdata->mac_idx,  val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(reg_val, MAC_RX_CFG, IPC, val);
		XGMAC_RGWR(pdata, MAC_RX_CFG, reg_val);
	}

	return 0;
}

/* JE:
 * if 1, the MAC allows jumbo packets of 9,018 bytes (9,022
 * bytes for VLAN tagged packets) without reporting a giant packet error in
 * the Rx packet status.
 * WD:Watchdog Disable
 * if 1, the MAC disables the watchdog timer on the receiver.
 * The MAC can receive packets of up to 16,383 bytes.
 * if 0, the MAC does not allow more than 2,048 bytes
 * (10,240 if JE is set high) of the packet being received.
 * The MAC cuts off any bytes received after 2,048 bytes.
 * GPSLCE:
 * if 1, the MAC considers the value in GPSL field in MAC_Rx_Configuration
 * register to declare a received packet as Giant packet.
 * This field must be programmed to more than 1,518 bytes.
 * Otherwise, the MAC considers 1,518 bytes as giant packet limit.
 * if 0, the MAC considers a received packet as Giant packet when its size
 * is greater than 1,518 bytes (1522 bytes for tagged packet)
 * JD: Jabber Disable
 * if 1, the XGMAC disables the jabber timer on the transmitter.
 * Transmission of up to 16,383-byte frames is supported.
 * if 0, the XGMAC cuts off the transmitter if the application sends
 * more than 2,048 bytes of data
 * (10,240 bytes if JE (in MAC_Rx_Configuration register is set high)
 * during transmission.
 * GPSL: Giant Packet Size Limit
 * If the received packet size is greater than the value programmed in this
 * field in units of bytes, the MAC declares the received packet as Giant
 * packet. The value programmed in this field must be greater than or equal
 * to 1,518 bytes. Any other programmed value is considered as 1,518bytes.
 * For VLAN tagged packets, the MAC adds 4 bytes to the programmed value.
 * The value in this field is applicable when the GPSLCE bit is set in
 * MAC_Rx_Configuration register.
 */
int xgmac_config_jumbo_pkt(void *pdev, u32 mtu)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 mac_rcr, mac_tcr;

	mac_rcr = XGMAC_RGRD(pdata, MAC_RX_CFG);
	mac_tcr = XGMAC_RGRD(pdata, MAC_TX_CFG);

	if (mtu < XGMAC_MAX_GPSL) { /* upto 9018 configuration */
		MAC_SET_VAL(mac_rcr, MAC_RX_CFG, JE, 1);
		MAC_SET_VAL(mac_rcr, MAC_RX_CFG, WD, 0);
		MAC_SET_VAL(mac_rcr, MAC_RX_CFG, GPSLCE, 0);
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, JD, 0);
	} else { /* upto 16K configuration */
		MAC_SET_VAL(mac_rcr, MAC_RX_CFG, JE, 0);
		MAC_SET_VAL(mac_rcr, MAC_RX_CFG, WD, 1);
		MAC_SET_VAL(mac_rcr, MAC_RX_CFG, GPSLCE, 1);
		MAC_SET_VAL(mac_rcr, MAC_RX_CFG, GPSL, XGMAC_MAX_SUPPORTED_MTU);
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, JD, 1);
	}

	XGMAC_RGWR(pdata, MAC_RX_CFG, mac_rcr);
	XGMAC_RGWR(pdata, MAC_TX_CFG, mac_tcr);

	return 0;
}

int xgmac_config_std_pkt(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 mac_rcr, mac_tcr;

	mac_rcr = XGMAC_RGRD(pdata, MAC_RX_CFG);
	mac_tcr = XGMAC_RGRD(pdata, MAC_TX_CFG);

	MAC_SET_VAL(mac_rcr, MAC_RX_CFG, JE, 0);
	MAC_SET_VAL(mac_rcr, MAC_RX_CFG, WD, 0);
	MAC_SET_VAL(mac_rcr, MAC_RX_CFG, GPSLCE, 0);
	MAC_SET_VAL(mac_tcr, MAC_TX_CFG, JD, 0);

	XGMAC_RGWR(pdata, MAC_RX_CFG, mac_rcr);
	XGMAC_RGWR(pdata, MAC_TX_CFG, mac_tcr);

	return 0;
}

int xgmac_set_rxcrc(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_rcr;

	mac_rcr = XGMAC_RGRD(pdata, MAC_RX_CFG);
	MAC_SET_VAL(mac_rcr, MAC_RX_CFG, DCRCC, val);

	mac_dbg("XGMAC %d: Rx CRC check: %s\n", pdata->mac_idx,
		val ? "DISABLED" : "ENABLED");

	XGMAC_RGWR(pdata, MAC_RX_CFG, mac_rcr);

	return 0;
}

/* RE:
 * When this bit is set, the Rx state machine of the MAC is enabled for
 * receiving packets from the GMII or XGMII interface
 * ACS:
 * If 1, the MAC strips the Pad or FCS field on the incoming
 * packets only if the value of the length field is less than 1,536 bytes. All
 * received packets with length field greater than or equal to 1,536 bytes are
 * passed to the application without stripping the Pad or FCS field.
 * if 0, the MAC passes all incoming packets to the
 * application, without any modification.
 * CST:
 * if 1, the last four bytes (FCS) of all packets of Ether type
 * (field greater than 1,536) are stripped and dropped before forwarding the
 * packet to the application.
 * DCRCC:
 * if 1, the MAC receiver does not check the CRC field in the received packets.
 * if 0, the MAC receiver always checks the CRC field in the received packets.
 */

int xgmac_powerup(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tcr = XGMAC_RGRD(pdata, MAC_TX_CFG);
	u32 mac_rcr = XGMAC_RGRD(pdata, MAC_RX_CFG);
	u32 mac_pfr = XGMAC_RGRD(pdata, MAC_PKT_FR);

	/* Enable MAC Tx */
	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, TE) != 1) {
		mac_dbg("XGMAC %d: MAC TX: ENABLED\n", pdata->mac_idx);
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, TE, 1);
		XGMAC_RGWR(pdata, MAC_TX_CFG, mac_tcr);
	}

	/* Enable MAC Rx */
	if (MAC_GET_VAL(mac_rcr, MAC_RX_CFG, RE) != 1) {
		mac_dbg("XGMAC %d: MAC RX: ENABLED\n", pdata->mac_idx);
		MAC_SET_VAL(mac_rcr, MAC_RX_CFG, RE, 1);
		XGMAC_RGWR(pdata, MAC_RX_CFG, mac_rcr);
	}

	/* Enable MAC Filter Rx All */
	if (MAC_GET_VAL(mac_pfr, MAC_PKT_FR, RA) != 1) {
		mac_dbg("XGMAC %d: MAC Filter Receive All: ENABLED\n",
			pdata->mac_idx);
		MAC_SET_VAL(mac_pfr, MAC_PKT_FR, RA, 1);
		XGMAC_RGWR(pdata, MAC_PKT_FR, mac_pfr);
	}

	return 0;
}

int xgmac_powerdown(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tcr = XGMAC_RGRD(pdata, MAC_TX_CFG);
	u32 mac_rcr = XGMAC_RGRD(pdata, MAC_RX_CFG);
	u32 mac_pfr = XGMAC_RGRD(pdata, MAC_PKT_FR);

	/* Disable MAC Tx */
	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, TE) != 0) {
		mac_dbg("XGMAC %d: MAC TX: DISABLED\n", pdata->mac_idx);
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, TE, 0);
		XGMAC_RGWR(pdata, MAC_TX_CFG, mac_tcr);
	}

	/* Disable MAC Rx */
	if (MAC_GET_VAL(mac_rcr, MAC_RX_CFG, RE) != 0) {
		mac_dbg("XGMAC %d: MAC RX: DISABLED\n", pdata->mac_idx);
		MAC_SET_VAL(mac_rcr, MAC_RX_CFG, RE, 0);
		XGMAC_RGWR(pdata, MAC_RX_CFG, mac_rcr);
	}

	/* Disable MAC Filter Rx All */
	if (MAC_GET_VAL(mac_pfr, MAC_PKT_FR, RA) != 0) {
		mac_dbg("XGMAC %d: MAC Filter Receive All: DISABLED\n",
			pdata->mac_idx);
		MAC_SET_VAL(mac_pfr, MAC_PKT_FR, RA, 0);
		XGMAC_RGWR(pdata, MAC_PKT_FR, mac_pfr);
	}

	return 0;
}

/* EEE CONFIGURATIONS */

/* brief This sequence is used to enable EEE mode
 * LPITXA: LPI Transmit Automate
 * If the LPITXA and LPITXEN bits are set to 1, the MAC enters the LPI mode
 * only after all outstanding packets (in the core) and pending packets (in the
 * application interface) have been transmitted. The MAC comes out of the LPI
 * mode when the application presents any packet for transmission or the
 * application issues a Tx FIFO Flush command. In addition, the MAC
 * automatically clears the LPITXEN bit when it exits the LPI state. If Tx FIFO
 * Flush is set in the FTQ bit of MTL_TxQ0_Operation_Mode register, when
 * the MAC is in the LPI mode, it exits the LPI mode.
 * When this bit is 0, the LPITXEN bit directly controls behavior of the MAC
 * when it is entering or coming out of the LPI mode.
 * LPITXEN: LPI Transmit Enable
 * When this bit is set, it instructs the MAC Transmitter to enter the LPI state
 * When this bit is reset, it instructs the MAC to exit the LPI state and resume
 * normal transmission.
 * This bit is cleared when the LPITXA bit is set and the MAC exits the LPI
 * state because of the arrival of a new packet for transmission
 *
 */
int xgmac_set_eee_mode(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_lpiscr = XGMAC_RGRD(pdata, MAC_LPI_CSR);

	if (MAC_GET_VAL(mac_lpiscr, MAC_LPI_CSR, LPITXEN) != val) {
		mac_dbg("XGMAC %d: LPI Transmit Enable: %s\n",
			pdata->mac_idx,  val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(mac_lpiscr, MAC_LPI_CSR, LPITXA, val);
		MAC_SET_VAL(mac_lpiscr, MAC_LPI_CSR, LPITXEN, val);
	}

	XGMAC_RGWR(pdata, MAC_LPI_CSR, mac_lpiscr);

	return 0;
}

u32 xgmac_get_eee_mode(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 lpitxen;

	lpitxen = XGMAC_RGRD_BITS(pdata, MAC_LPI_CSR, LPITXEN);

	mac_dbg("\tLPI Transmit Enable: %s\n",
		lpitxen ? "ENABLED" : "DISABLED");

	return lpitxen;
}

/* brief This sequence is used to set PLS bit
 * PLS: Phy link Status
 * This bit indicates the link status of the PHY. The MAC Transmitter asserts
 * the LPI pattern only when the link status is up (OKAY) at least for the time
 * indicated by the LPI LS TIMER.
 * When this bit is set, the link is considered to be okay (UP) and when this
 * bit is reset, the link is considered to be down.
 */

int xgmac_set_eee_pls(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val = XGMAC_RGRD(pdata, MAC_LPI_CSR);

	/* Disable MAC Tx */
	if (MAC_GET_VAL(reg_val, MAC_LPI_CSR, PLS) != val) {
		mac_dbg("XGMAC %d: Phy link Status: %s\n",
			pdata->mac_idx,  val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(reg_val, MAC_LPI_CSR, PLS, val);
		XGMAC_RGWR(pdata, MAC_LPI_CSR, reg_val);
	}

	return 0;
}

/* brief This sequence is used to set EEE timer values
 * LPI LS TIMER:
 * This field specifies the minimum time (in milliseconds) for which the
 * link status from the PHY should be up (OKAY) before the LPI pattern
 * can be transmitted to the PHY. The MAC does not transmit the LPI pattern
 * even when the LPITXEN bit is set unless the LPI LS Timer reaches the
 * programmed terminal count. The default value of the LPI LS Timer is
 * 1000 (1 sec) as defined in the IEEE standard.
 * LPI TW TIMER
 * This field specifies the minimum time (in microseconds) for which
 * the MAC waits after it stops transmitting the LPI pattern to the
 * PHY and before it resumes the normal transmission. The TLPIEX status
 * bit is set after the expiry of this timer.
 */

int xgmac_set_eee_timer(void *pdev, u32 twt, u32 lst)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_LPI_TCR);

	/* Waits till  the TWT(usec), before starting normal transmission */
	MAC_SET_VAL(reg_val, MAC_LPI_TCR, TWT, twt);
	/* Waits till  the LST(msec), before starting to send LPI pattern */
	MAC_SET_VAL(reg_val, MAC_LPI_TCR, LST, lst);

	XGMAC_RGWR(pdata, MAC_LPI_TCR, reg_val);

	return 0;
}

/* CRC STRIPPING */

/* CST: CRC stripping for Type packets
 * When this bit is set, the last four bytes (FCS) of all packets of Ether type
 * (field greater than 1,536) are stripped and dropped before forwarding the
 * packet to the application.
 */
int xgmac_set_crc_strip_type(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_RX_CFG);

	if (MAC_GET_VAL(reg_val, MAC_RX_CFG, CST) != val) {
		mac_dbg("XGMAC %d: CRC stripping for Type packets: %s\n",
			pdata->mac_idx,  val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(reg_val, MAC_RX_CFG, CST, val);
		XGMAC_RGWR(pdata, MAC_RX_CFG, reg_val);
	}

	return 0;
}

/* ACS: Automatic Pad or CRC Stripping
 * When this bit is set, the MAC strips the Pad or FCS field on the incoming
 * packets only if the value of the length field is less than 1,536 bytes. All
 * received packets with length field greater than or equal to 1,536 bytes are
 * passed to the application without stripping the Pad or FCS field.
 * When this bit is reset, the MAC passes all incoming packets to the
 * application, without any modification.
 */
int xgmac_set_acs(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_RX_CFG);

	if (MAC_GET_VAL(reg_val, MAC_RX_CFG, ACS) != val) {
		mac_dbg("XGMAC %d: Automatic Pad or CRC Stripping: %s\n",
			pdata->mac_idx,  val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(reg_val, MAC_RX_CFG, ACS, val);
		XGMAC_RGWR(pdata, MAC_RX_CFG, reg_val);
	}

	return 0;
}

/* In GMII mode (1G or 2.5G) and when IFP is cleared, the
 * minimum IPG between transmitted frames is reduced.
 * In all modes (10G/2.5G/1G), when IFP is set, the minimum IPG is
 * increased in steps of 32 bits as follows. In 10G (XGMII) mode, when IFP
 * is set, it overrides the functions of DDIC and ISM bits.
 */
int xgmac_set_ipg(void *pdev, u32 ipg)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_TX_CFG);

	if (MAC_GET_VAL(reg_val, MAC_TX_CFG, IPG) != ipg) {
		mac_dbg("XGMAC %d: IPG set to %d bytes\n",
			pdata->mac_idx, ((96 + (ipg * 32)) / 8));
		MAC_SET_VAL(reg_val, MAC_TX_CFG, IPG, ipg);
		XGMAC_RGWR(pdata, MAC_TX_CFG, reg_val);
	}

	return 0;
}

/* PWRDWN: Power Down
 * if 1, the MAC receiver drops all received packets until
 * it receives the expected magic packet or remote wake-up packet.
 * This bit is then self-cleared and the power-down mode is disabled.
 * The software can clear this bit before the expected magic packet or
 * remote wake-up packet is received. The packets received by the
 * MAC after this bit is cleared are forwarded to the application. This bit
 * must only be set when the Magic Packet Enable, Global Unicast, or
 * Remote Wake-Up Packet Enable bit is set high.
 * MGKPKTEN: Magic Packet Enable
 * if 1, a power management event is generated when
 * the MAC receives a magic packet.
 */
int xgmac_set_magic_pmt(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 mac_pmtcsr;

	mac_pmtcsr = XGMAC_RGRD(pdata, MAC_PMT_CSR);

	if (MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, MGKPKTEN) != val) {
		mac_dbg("XGMAC %d: Magic Packet: %s\n",
			pdata->mac_idx, val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(mac_pmtcsr, MAC_PMT_CSR, MGKPKTEN, val);
	}

	if (MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, PWRDWN) != val) {
		mac_dbg("XGMAC %d: Power Down Mode: %s\n",
			pdata->mac_idx, val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(mac_pmtcsr, MAC_PMT_CSR, PWRDWN, val);
	}

	XGMAC_RGWR(pdata, MAC_PMT_CSR, mac_pmtcsr);
	return 0;
}

/* Reading MGKPRCVD or RWKPRCVD will clear the XGMAC PMT Interrupt Status */
int xgmac_pmt_int_clr(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_pmtcsr;
	int ret = -1;

	mac_pmtcsr = XGMAC_RGRD(pdata, MAC_PMT_CSR);

	if (MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, MGKPRCVD)) {
		mac_dbg("XGMAC %d: Clearing Magic packet "
			"Interrupt\n", pdata->mac_idx);
		ret = 0;
	}

	if (MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, RWKPRCVD)) {
		mac_dbg("XGMAC %d: Clearing Remote wakeup packet "
			"Interrupt\n", pdata->mac_idx);
		ret = 0;
	}

	return ret;
}

/* Reading TLPIEN or RLPIEN will clear the XGMAC LPI Interrupt Status */
int xgmac_lpi_int_clr(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 varmac_lps;
	int ret = -1;

	varmac_lps = XGMAC_RGRD(pdata, MAC_LPI_CSR);

	if (MAC_GET_VAL(varmac_lps, MAC_LPI_CSR, TLPIEN)) {
		//mac_dbg("XGMAC %d: Clearing LPI TX Interrupt Status\n",
		//	   pdata->mac_idx);
		ret = 0;
	}

	if (MAC_GET_VAL(varmac_lps, MAC_LPI_CSR, RLPIEN)) {
		//mac_dbg("XGMAC %d: Clearing LPI RX Interrupt Status\n",
		//	   pdata->mac_idx);
		ret = 0;
	}

	return ret;
}

/* RWKPKTEN: Remote Wake-Up Packet Enable
 * if 1, a power management event is generated when
 * the MAC receives a remote wake-up packet.
 */
int xgmac_set_rwk_pmt(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u32 mac_pmtcsr;

	mac_pmtcsr = XGMAC_RGRD(pdata, MAC_PMT_CSR);

	if (MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, RWKPKTEN) != val) {
		mac_dbg("XGMAC %d: Remote Wakeup Packet: %s\n",
			pdata->mac_idx, val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(mac_pmtcsr, MAC_PMT_CSR, RWKPKTEN, val);
	}

	if (MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, PWRDWN) != val) {
		mac_dbg("XGMAC %d: Power Down Mode: %s\n",
			pdata->mac_idx, val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(mac_pmtcsr, MAC_PMT_CSR, PWRDWN, val);
	}

	XGMAC_RGWR(pdata, MAC_PMT_CSR, mac_pmtcsr);
	return 0;
}

/* RWKFILTRST:
 * Remote Wake-Up Packet Filter Register Pointer Reset
 * When this bit is set, the remote wake-up packet filter register pointer
 * (RWKPTR) is reset to 0. It is automatically cleared after 1 clock cycle.
 */
int xgmac_set_rwk_filter_registers(void *pdev, u32 count, u32 *val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 i;

	for (i = 0; i < count; i++)
		XGMAC_RGWR(pdata, MAC_RWK_PFR, val[i]);

	return 0;
}

int xgmac_set_pmt_gucast(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_pmtcsr;

	mac_pmtcsr = XGMAC_RGRD(pdata, MAC_PMT_CSR);

	if (MAC_GET_VAL(mac_pmtcsr, MAC_PMT_CSR, GLBLUCAST) != val) {
		mac_dbg("XGMAC %d: PMT Global Unicast: %s\n",
			pdata->mac_idx, val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(mac_pmtcsr, MAC_PMT_CSR, GLBLUCAST, val);
	}

	XGMAC_RGWR(pdata, MAC_PMT_CSR, mac_pmtcsr);

	return 0;
}

int xgmac_set_extcfg(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_extcfg;

	mac_extcfg = XGMAC_RGRD(pdata, MAC_EXTCFG);

	if (MAC_GET_VAL(mac_extcfg, MAC_EXTCFG, SBDIOEN) != val) {
		mac_dbg("XGMAC %d: MAC Extended CFG SBDIOEN: %s\n",
			pdata->mac_idx, val ? "ENABLED" : "DISABLED");

		MAC_SET_VAL(mac_extcfg, MAC_EXTCFG, SBDIOEN, val);

		XGMAC_RGWR(pdata, MAC_EXTCFG, mac_extcfg);
	}

	return 0;
}

int xgmac_set_mac_rxtx(void *pdev, u32 wd, u32 jd)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_RX_CFG);

	if (MAC_GET_VAL(reg_val, MAC_RX_CFG, WD) != wd) {
		mac_dbg("XGMAC %d: WD: %s\n",
			pdata->mac_idx, wd ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(reg_val, MAC_RX_CFG, WD, wd);
		XGMAC_RGWR(pdata, MAC_RX_CFG, reg_val);
	}

	reg_val = XGMAC_RGRD(pdata, MAC_TX_CFG);

	if (MAC_GET_VAL(reg_val, MAC_TX_CFG, JD) != jd) {
		mac_dbg("XGMAC %d: JD: %s\n",
			pdata->mac_idx, jd ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(reg_val, MAC_TX_CFG, JD, jd);
		XGMAC_RGWR(pdata, MAC_TX_CFG, reg_val);
	}

	return 0;
}

/* LPIATE: LPI Auto Timer Enable
 * This bit controls the automatic entry of the MAC Transmitter into
 * and exit out of the LPI state. When LPIATE, LPITXA and LPITXEN bits
 * are set, the MAC Transmitter will enter LPI state only when the
 * complete MAC TX data path is IDLE for a period indicated by the
 * MAC_LPI_Auto_Entry_Timer register. After entering LPI state,
 * if the data path becomes non-IDLE (due to a new packet being accepted
 * for transmission), the Transmitter will exit LPI state
 * but will not clear LPITXEN bit. This enables the re-entry into LPI state
 * when it is IDLE again.
 * When LPIATE is 0, the LPI Auto timer is disabled and MAC Transmitter will
 * enter LPI state based on the settings of LPITXA and LPITXEN bit
 * descriptions.
 * LPITXA: LPI Tx Automate
 * This bit controls the behavior of the MAC when it is entering or coming out
 * of the LPI mode on the Transmit side. This bit is not functional in the
 * XGMAC-CORE configurations in which the Tx clock gating is done during
 * the LPI mode.
 * If the LPITXA and LPITXEN bits are set to 1, the MAC enters the LPI mode
 * only after all outstanding packets (in the core) and pending packets (in the
 * application interface) have been transmitted. The MAC comes out of the LPI
 * mode when the application presents any packet for transmission or the
 * application issues a Tx FIFO Flush command. In addition, the MAC
 * automatically clears the LPITXEN bit when it exits the LPI state. If Tx FIFO
 * Flush is set in the FTQ bit of MTL_TxQ0_Operation_Mode register, when
 * the MAC is in the LPI mode, it exits the LPI mode.
 * When this bit is 0, the LPITXEN bit directly controls behavior of the MAC
 * when it is entering or coming out of the LPI mode.
 */
int xgmac_set_mac_lpitx(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 lpiate;

	lpiate = XGMAC_RGRD(pdata, MAC_LPI_CSR);

	if (MAC_GET_VAL(lpiate, MAC_LPI_CSR, LPIATE) != val) {
		mac_dbg("XGMAC %d: LPIATE: %s\n",
			pdata->mac_idx, val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(lpiate, MAC_LPI_CSR, LPIATE, val);
	}

	if (MAC_GET_VAL(lpiate, MAC_LPI_CSR, LPITXA) != val) {
		mac_dbg("XGMAC %d: LPITXA: %s\n",
			pdata->mac_idx, val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(lpiate, MAC_LPI_CSR, LPITXA, val);
	}

	XGMAC_RGWR(pdata, MAC_LPI_CSR, lpiate);

	return 0;
}

/* This sequence is used to adjust the ptp operating frequency.
 * TSADDREG:
 * The Timestamp Addend register is present only when the IEEE 1588 Timestamp
 * feature is selected without external timestamp input. This register value
 * is used only when the system time is configured for Fine
 * Update mode (TSCFUPDT bit in the MAC_Timestamp_Control register).
 * The content of this register is added to a 32-bit accumulator in every
 * clock cycle and the system time is updated whenever the accumulator
 * overflows.
 * This field indicates the 32-bit time value to be added to the
 * Accumulator register to achieve time synchronization.
 */
int xgmac_set_tstamp_addend(void *pdev, u32 tstamp_addend)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	/* Set the addend register value and tell the device */
	XGMAC_RGWR(pdata, MAC_TSTAMP_ADDNDR, tstamp_addend);
	XGMAC_RGWR_BITS(pdata, MAC_TSTAMP_CR, TSADDREG, 1);

	/* Wait for addend update to complete */
	while (1) {
		if (XGMAC_RGRD_BITS(pdata, MAC_TSTAMP_CR, TSADDREG) == 0)
			break;
	}

	return 0;
}

/* This sequence is used to initialize the system time
 * TSINIT:
 * if 1, the system time is initialized (overwritten) with the
 * value specified in the MAC Register 80
 * This bit should be zero before it is updated.
 * This bit is reset when the initialization is complete.
 */
int xgmac_init_systime(void *pdev, u32 sec, u32 nsec)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	/* Set the time values and tell the device */
	XGMAC_RGWR(pdata, MAC_SYS_TIME_SEC_UPD, sec);
	XGMAC_RGWR(pdata, MAC_SYS_TIME_NSEC_UPD, nsec);
	XGMAC_RGWR_BITS(pdata, MAC_TSTAMP_CR, TSINIT, 1);

	/* Wait for time update to complete */
	while (1) {
		if (XGMAC_RGRD_BITS(pdata, MAC_TSTAMP_CR, TSINIT) == 0)
			break;
	}

	return 0;
}

/* This sequence is used to adjust/update the system time
 * ADDSUB:Add or Subtract Time
 * When this bit is set, the time value is subtracted with the contents of the
 * update register. When this bit is reset, the time value is added with the
 * contents of the update register.
 * TSUPDT:Update Timestamp
 * When this bit is set, the system time is updated (added or subtracted)
 * with the value specified in MAC_System_Time_Seconds_Update and
 * MAC_System_Time_Nanoseconds_Update.
 * This bit should be zero before updating it. This bit is reset when the
 * update is complete in hardware. The Timestamp Higher Word register
 * (if enabled during core configuration) is not updated.
 * TSCTRLSSR :Timestamp Digital or Binary Rollover Control
 * When this bit is set, the Timestamp Low register rolls over after
 * 0x3B9A_C9FF value (that is, 1 nanosecond accuracy) and increments
 * the timestamp (High) seconds. When this bit is reset, the rollover value
 * of sub-second register is 0x7FFF_FFFF. The sub-second increment
 * must be programmed correctly depending on the PTP reference clock
 * frequency and the value of this bit.
 */
int xgmac_adjust_systime(void *pdev, u32 sec, u32 nsec, u32 add_sub,
			 u32 one_nsec_accuracy)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	if (add_sub) {
		/* If the new sec value needs to be subtracted with
		 * the system time, then MAC_SYS_TIME_SEC_UPD reg should be
		 * programmed with (2^32 - <new_sec_value>)
		 */
		sec = (0x100000000ull - sec);

		/* If the new nsec value need to be subtracted with
		 * the system time, then MAC_STNSUR.TSSS field should be
		 * programmed with,
		 * (10^9 - <new_nsec_value>) if MAC_TX_CFG.TSCTRLSSR is set or
		 * (2^31 - <new_nsec_value> if MAC_TX_CFG.TSCTRLSSR is reset)
		 */
		if (one_nsec_accuracy)
			nsec = (0x3B9ACA00 - nsec);
		else
			nsec = (0x80000000 - nsec);
	}

	/* Set the time values and tell the device */
	XGMAC_RGWR(pdata, MAC_SYS_TIME_SEC_UPD, sec);
	XGMAC_RGWR(pdata, MAC_SYS_TIME_NSEC_UPD, nsec);
	XGMAC_RGWR_BITS(pdata, MAC_SYS_TIME_NSEC_UPD, ADDSUB, add_sub);
	XGMAC_RGWR_BITS(pdata, MAC_TSTAMP_CR, TSUPDT, 1);

	/* Wait for time update to complete */
	while (1) {
		if (XGMAC_RGRD_BITS(pdata, MAC_TSTAMP_CR, TSUPDT) == 0)
			break;
	}

	return 0;
}

/* This sequence is used get 64-bit system time in nano sec */
u64 xgmac_get_systime(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	u64 nsec;

	nsec = XGMAC_RGRD(pdata, MAC_SYS_TIME_SEC);
	nsec *= NSEC_TO_SEC;
	nsec += XGMAC_RGRD(pdata, MAC_SYS_TIME_NSEC);

	return nsec;
}

/* This sequence is used to check whether a timestamp has been
 * captured for the corresponding transmit packet. Returns val if
 * timestamp is taken else returns 0
 */
u64 xgmac_get_tx_tstamp(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u64 nsec;

	nsec = XGMAC_RGRD(pdata, MAC_TXTSTAMP_SECR);
	nsec *= NSEC_TO_SEC;
	nsec += XGMAC_RGRD(pdata, MAC_TXTSTAMP_NSECR);

	return nsec;
}

/* This sequence is used to get the number of tx timestamp snapshots captured.
 */
int xgmac_get_txtstamp_cnt(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 tx_sts;
	u32 ttsns;

	tx_sts = XGMAC_RGRD(pdata, MAC_TSTAMP_STSR);

	ttsns = MAC_GET_VAL(tx_sts, MAC_TSTAMP_STSR, TTSNS);
	mac_dbg("\tTx Timestamp Fifo: %d\n", ttsns);

	/* Max Fifo Value*/
	if (ttsns == 16)
		mac_dbg("XGMAC %d: Tx Timestamp Fifo is Full\n",
			pdata->mac_idx);

	return ttsns;
}

/* This sequence is used to get PktID of Transmitted Packet.
 */
int xgmac_get_txtstamp_pktid(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 tx_sts;
	int pktid;

	tx_sts = XGMAC_RGRD(pdata, MAC_TXTSTAMP_STS);

	pktid = MAC_GET_VAL(tx_sts, MAC_TXTSTAMP_STS, PKTID);
	mac_dbg("\tTx Timestamp PacketID: %d\n", pktid);

	return pktid;
}

/* TSCTRLSSR:Timestamp Digital or Binary Rollover Control
 * if 1, the Timestamp Low register rolls over after 0x3B9A_C9FF value
 * (that is, 1 nanosecond accuracy) and increments
 * the timestamp (High) seconds.
 * if 0, the rollover value of sub-second register is 0x7FFF_FFFF.
 * The sub-second increment must be programmed correctly
 * depending on the PTP reference clock frequency and the value of this bit.
 * TSCFUPDT:Fine or Coarse Timestamp Update
 * if 1, the Fine method is used to update system timestamp.
 * if 0, Coarse method is used to update the system timestamp
 * TXTSSTSM:Transmit Timestamp Status Mode
 * if 1, the MAC overwrites the earlier transmit timestamp status even
 * if it is not read by the software.
 * The MAC indicates this by setting the TXTSSTSMIS bit of the
 * MAC_TxTimestamp_Status_Nanoseconds register.
 * if 0, the MAC ignores the timestamp status of current packet if the
 * timestamp status of previous packet is not read by the software.
 * The MAC indicates this by setting the TXTSSTSMIS bit of the
 * MAC_TxTimestamp_Status_Nanoseconds register
 * SSINC:
 * The value programmed in this field is accumulated with the
 * contents of the sub-second register.
 * For example, when the PTP clock is 50 MHz (period is 20 ns),
 * you should program 20 (0x14)
 * when the System TimeNanoseconds register has an accuracy of 1 ns
 * [Bit 9 (TSCTRLSSR) is set in MAC_Timestamp_Control].
 * When TSCTRLSSR is clear, the Nanoseconds register has a resolution
 * of ~0.465 ns. In this case, you
 * should program a value of 43 (0x2B) which is derived by 20 ns/0.465.
 * SNSINC:
 * This field contains the sub-nanosecond increment value, represented
 * in nanoseconds multiplied by 28.
 * This value is accumulated with the sub-nanoseconds field of the
 * sub-second register.
 * For example, when TSCTRLSSR field in the
 * MAC_Timestamp_Control register is set.
 * and if the required increment is 5.3ns, then SSINC should be 0x05
 * and SNSINC should be 0x4C.
 */
int xgmac_config_tstamp(void *pdev, u32 mac_tscr)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	/* Exit if timestamping is not enabled */
	if (!MAC_GET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENA))
		return 0;

	/* Set one nano-second accuracy */
	if (pdata->one_nsec_accuracy)
		MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSCTRLSSR, 1);
	else
		MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSCTRLSSR, 0);

	/* Set fine timestamp update */
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSCFUPDT, 1);

	/* Overwrite earlier timestamps */
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TXTSSTSM, 1);

	XGMAC_RGWR(pdata, MAC_TSTAMP_CR, mac_tscr);

#ifdef __KERNEL__
#else
	/* Initialize time registers */
	xgmac_config_subsec_inc(pdev, pdata->ptp_clk);

	xgmac_set_tstamp_addend(pdev, pdata->def_addend);
	xgmac_init_systime(pdev, pdata->sec, pdata->nsec);
#endif

	return 0;
}

int xgmac_config_subsec_inc(void *pdev, u32 ptp_clk)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 val;

	if (XGMAC_RGRD_BITS(pdata, MAC_TSTAMP_CR, TSCFUPDT) == 1)
		val = ((1 * NSEC_TO_SEC) / MHZ_TO_HZ(CLOCK_UPDATE_FREQ));
	else
		val = ((1 * NSEC_TO_SEC) / ptp_clk);

	/* 0.465ns accurecy */
	if (XGMAC_RGRD_BITS(pdata, MAC_TSTAMP_CR, TSCTRLSSR) == 0)
		val = (val * 1000) / 465;

	XGMAC_RGWR_BITS(pdata, MAC_SUBSEC_INCR, SSINC, val);

	return 0;
}

int xgmac_disable_tstamp(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tscr = 0;

	memset(&pdata->ptp_flgs, 0, sizeof(pdata->ptp_flgs));

	/* Basically set all bits in MAC_TSTAMP_CR to 0, beloc code is
	 * actually not needed
	 */
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENA, 0);
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENALL, 0);

	XGMAC_RGWR(pdata, MAC_TSTAMP_CR, mac_tscr);

	return 0;
}

int xgmac_get_hwtstamp_settings(void *pdev,
				struct hwtstamp_config *config)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	memcpy(config, &pdata->tstamp_config, sizeof(pdata->tstamp_config));
	return 0;
}

/* TSENALL:
 * Enable Timestamp for All Packets
 * When this bit is set, the timestamp snapshot is enabled for all packets
 * received by the MAC.
 * TSENA:
 * When this bit is set, the timestamp is added for Transmit and
 * Receive packets.
 * On the Receive side, the MAC processes the 1588 packets only
 * if this bit is set.
 * TSVER2ENA:
 * When this bit is set, the IEEE 1588 version 2 format is used to
 * process the PTP packets
 * TSIPV4ENA:
 * When this bit is set, the MAC receiver processes the PTP packets
 * encapsulated in IPv4-UDP packets.
 * TSIPV6ENA:
 * When this bit is set, the MAC receiver processes the PTP packets
 * encapsulated in IPv6-UDP packets.
 * SNAPTYPSEL:
 * These bits, along with Bits 15 and 14, decide the set of PTP packet
 * types for which snapshot needs to be taken.
 * TSENA:
 * When this bit is set, the timestamp is added for Transmit and
 * Receive packets.
 * On the Receive side, the MAC processes the 1588 packets only
 * if this bit is set.
 * TSEVNTENA:
 * if 1, the timestamp snapshot is taken only for event messages
 * (SYNC, Delay_Req, Pdelay_Req, or Pdelay_Resp).
 * if 0, the snapshot is taken for all messages except
 * Announce, Management, and Signaling.
 * TSMSTRENA:
 * if 1, the snapshot is taken only for the messages that are
 * relevant to the master node.
 * if 0, the snapshot is taken for the messages relevant
 * to the slave node.
 * AV8021ASMEN:
 * if 1, the MAC processes only untagged PTP over Ethernet packets
 * for providing PTP status
 * and capturing timestamp snapshots, that is, IEEE 802.1AS mode of operation.
 * TSIPENA:
 * if 1, the MAC receiver processes the PTP packets encapsulated directly
 * in the Ethernet packets.
 * if 0, the MAC ignores the PTP over Ethernet packets.
 */

int xgmac_set_hwtstamp_settings(void *pdev,
				u32 tx_type,
				u32 rx_filter)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tscr = 0;
	struct ptp_flags ptp_flgs = {0};

	mac_dbg("Mac Idx %d\n", pdata->mac_idx);
	mac_dbg("tx_type = %d, rx_filter = %d\n", tx_type, rx_filter);

	switch (tx_type) {
	case HWTSTAMP_TX_OFF:
		break;

	case HWTSTAMP_TX_ON:
	case HWTSTAMP_TX_ONESTEP_SYNC:
		ptp_flgs.ptp_tx_en = 1;
		MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENA, 1);
		break;

	default:
		return 0;
	}

	/* This is based on Table 7-146, Receive side timestamp Capture
	 * scenarios
	 */

	switch (rx_filter) {
	case HWTSTAMP_FILTER_NONE:
		break;

	case HWTSTAMP_FILTER_ALL:
		mac_printf("HW tstamp config: Filter All\n");
		ptp_flgs.ptp_rx_en |= PTP_RX_EN_ALL;
		break;

	/* PTP v1, UDP, any kind of event packet */
	case HWTSTAMP_FILTER_PTP_V2_L4_EVENT:
		ptp_flgs.ptp_rx_en |= PTP_RX_V2;

	case HWTSTAMP_FILTER_PTP_V1_L4_EVENT:
		mac_printf("PTP v1, UDP, any kind of event packet\n");
		ptp_flgs.ptp_rx_en |=
			(PTP_RX_OVER_IPV4_UDP |
			 PTP_RX_OVER_IPV6_UDP | PTP_RX_SNAP);
		break;

	/* PTP v1, UDP, Sync packet */
	case HWTSTAMP_FILTER_PTP_V2_L4_SYNC:
		ptp_flgs.ptp_rx_en |= PTP_RX_V2;

	case HWTSTAMP_FILTER_PTP_V1_L4_SYNC:
		mac_printf("PTP v1, UDP, Sync packet\n");
		ptp_flgs.ptp_rx_en |=
			(PTP_RX_OVER_IPV4_UDP |
			 PTP_RX_OVER_IPV6_UDP | PTP_RX_EVNT);
		break;

	/* PTP v1, UDP, Delay_req packet */
	case HWTSTAMP_FILTER_PTP_V2_L4_DELAY_REQ:
		ptp_flgs.ptp_rx_en |= PTP_RX_V2;

	case HWTSTAMP_FILTER_PTP_V1_L4_DELAY_REQ:
		mac_printf("PTP v1, UDP, Delay_req packet\n");
		ptp_flgs.ptp_rx_en |=
			(PTP_RX_OVER_IPV4_UDP |
			 PTP_RX_OVER_IPV6_UDP | PTP_RX_EVNT | PTP_RX_MSTR);
		break;

	/* 802.AS1, Ethernet, any kind of event packet */
	case HWTSTAMP_FILTER_PTP_V2_L2_EVENT:
		mac_printf("802.AS1, Ethernet, any kind of event packet\n");
		ptp_flgs.ptp_rx_en |=
			(PTP_RX_V2 | PTP_RX_OVER_ETH | PTP_RX_SNAP);
		break;

	/* 802.AS1, Ethernet, Sync packet */
	case HWTSTAMP_FILTER_PTP_V2_L2_SYNC:
		mac_printf("802.AS1, Ethernet, Sync packet\n");
		ptp_flgs.ptp_rx_en |=
			(PTP_RX_V2 | PTP_RX_OVER_ETH | PTP_RX_EVNT);
		break;

	/* 802.AS1, Ethernet, Delay_req packet */
	case HWTSTAMP_FILTER_PTP_V2_L2_DELAY_REQ:
		mac_printf("802.AS1, Ethernet, Delay_req packet\n");
		ptp_flgs.ptp_rx_en |=
			(PTP_RX_V2 | PTP_RX_OVER_ETH |
			 PTP_RX_EVNT | PTP_RX_MSTR);
		break;

	/* PTP v2/802.AS1, any layer, any kind of event packet */
	case HWTSTAMP_FILTER_PTP_V2_EVENT:
		mac_printf("PTP v2/802.AS1,any layer,any kind of event pkt\n");
		ptp_flgs.ptp_rx_en |=
			(PTP_RX_V2 | PTP_RX_OVER_ANY_LYR | PTP_RX_SNAP);
		break;

	/* PTP v2/802.AS1, any layer, Sync packet */
	case HWTSTAMP_FILTER_PTP_V2_SYNC:
		mac_printf("PTP v2/802.AS1, any layer, Sync packet\n");
		ptp_flgs.ptp_rx_en |=
			(PTP_RX_V2 | PTP_RX_OVER_ANY_LYR | PTP_RX_EVNT);
		break;

	/* PTP v2/802.AS1, any layer, Delay_req packet */
	case HWTSTAMP_FILTER_PTP_V2_DELAY_REQ:
		mac_printf("PTP v2/802.AS1, any layer, Delay_req packet\n");
		ptp_flgs.ptp_rx_en |=
			(PTP_RX_V2 | PTP_RX_OVER_ANY_LYR |
			 PTP_RX_EVNT | PTP_RX_MSTR);
		break;

	default:
		return 0;
	}

	if (ptp_flgs.ptp_rx_en != 0) {
		/* In PRX300 all packets have timestamp */
		ptp_flgs.ptp_rx_en |= PTP_RX_EN_ALL;

		MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENA, 1);

		if (ptp_flgs.ptp_rx_en & PTP_RX_V2)
			MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSVER2ENA, 1);

		if (ptp_flgs.ptp_rx_en &
		    (PTP_RX_OVER_ANY_LYR | PTP_RX_OVER_IPV4_UDP))
			MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSIPV4ENA, 1);

		if (ptp_flgs.ptp_rx_en &
		    (PTP_RX_OVER_ANY_LYR | PTP_RX_OVER_IPV6_UDP))
			MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSIPV6ENA, 1);

		if (ptp_flgs.ptp_rx_en & PTP_RX_OVER_ETH)
			MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, AV8021ASMEN, 1);

		if (ptp_flgs.ptp_rx_en & PTP_RX_OVER_ANY_LYR)
			MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSIPENA, 1);

		if (ptp_flgs.ptp_rx_en & PTP_RX_EVNT)
			MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSEVNTENA, 1);

		if (ptp_flgs.ptp_rx_en & PTP_RX_SNAP)
			MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, SNAPTYPSEL, 1);

		if (ptp_flgs.ptp_rx_en & PTP_RX_MSTR)
			MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSMSTRENA, 1);

		if (ptp_flgs.ptp_rx_en & PTP_RX_EN_ALL)
			MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENALL, 1);
	}

	if ((pdata->ptp_flgs.ptp_rx_en == ptp_flgs.ptp_rx_en) &&
	    (pdata->ptp_flgs.ptp_tx_en == ptp_flgs.ptp_tx_en)) {
		return 0;
	} else {
		pdata->ptp_flgs.ptp_rx_en = ptp_flgs.ptp_rx_en;
		pdata->ptp_flgs.ptp_tx_en = ptp_flgs.ptp_tx_en;
	}

	if (pdata->ptp_flgs.ptp_rx_en & PTP_RX_EN_ALL &&
	    pdata->ptp_flgs.ptp_tx_en) {
		mac_enable_ts(pdev);
	} else {
		mac_disable_ts(pdev);
	}

	xgmac_config_tstamp(pdev, mac_tscr);

#ifdef __KERNEL__
	xgmac_config_timer_reg(pdev, mac_tscr);
#endif

	return 0;
}

int xgmac_set_exttime_source(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 reg_val;

	reg_val = XGMAC_RGRD(pdata, MAC_TSTAMP_CR);

	if (MAC_GET_VAL(reg_val, MAC_TSTAMP_CR, ESTI) != val) {
		mac_dbg("XGMAC %d: External Ref Time: %s\n",
			pdata->mac_idx,  val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(reg_val, MAC_TSTAMP_CR, ESTI, val);
		XGMAC_RGWR(pdata, MAC_TSTAMP_CR, reg_val);
	}

	return 0;
}

int xgmac_ptp_txtstamp_mode(void *pdev,
			    u32 snaptypesel,
			    u32 tsmstrena,
			    u32 tsevntena)
{
	u32 mac_tscr = 0;

	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSVER2ENA, 1);
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSIPENA, 1);
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSIPV4ENA, 1);
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSIPV6ENA, 1);
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, SNAPTYPSEL, snaptypesel);
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSEVNTENA, tsevntena);
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSMSTRENA, tsmstrena);
	MAC_SET_VAL(mac_tscr, MAC_TSTAMP_CR, TSENA, 1);

	xgmac_config_tstamp(pdev, mac_tscr);

	return 0;
}

int xgmac_set_gint(void *pdev, u32 val)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_tcr = XGMAC_RGRD(pdata, MAC_TX_CFG);

	/* Enable/Disable MAC G9991EN */
	if (MAC_GET_VAL(mac_tcr, MAC_TX_CFG, G9991EN) != val) {
		mac_dbg("XGMAC %d: G9991EN: %s\n", pdata->mac_idx,
			val ? "ENABLED" : "DISABLED");
		MAC_SET_VAL(mac_tcr, MAC_TX_CFG, G9991EN, val);
		XGMAC_RGWR(pdata, MAC_TX_CFG, mac_tcr);
	}

	return 0;
}

int xgmac_get_hw_capability(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	u32 mac_hfr0, mac_hfr1, mac_hfr2;
	/* Hardware features of the device */
	struct xgmac_hw_features *hw_feat = &pdata->hw_feat;

	mac_hfr0 = XGMAC_RGRD(pdata, MAC_HW_F0);
	mac_hfr1 = XGMAC_RGRD(pdata, MAC_HW_F1);
	mac_hfr2 = XGMAC_RGRD(pdata, MAC_HW_F2);

	memset(hw_feat, 0, sizeof(*hw_feat));

	hw_feat->version = XGMAC_RGRD(pdata, MAC_VR);

	/* Hardware feature register 0 */
	hw_feat->gmii        = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, GMIISEL);
	hw_feat->vlhash      = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, VLHASH);
	hw_feat->sma         = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, SMASEL);
	hw_feat->rwk         = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, RWKSEL);
	hw_feat->mgk         = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, MGKSEL);
	hw_feat->mmc         = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, MMCSEL);
	hw_feat->aoe         = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, ARPOFFSEL);
	hw_feat->ts          = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, TSSEL);
	hw_feat->eee         = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, EEESEL);
	hw_feat->tx_coe      = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, TXCOESEL);
	hw_feat->rx_coe      = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, RXCOESEL);
	hw_feat->addn_mac   = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, ADDMACADRSEL);
	hw_feat->ts_src      = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, TSSTSSEL);
	hw_feat->sa_vlan_ins = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, SAVLANINS);
	hw_feat->vxn = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, VXN);
	hw_feat->ediffc = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, EDIFFC);
	hw_feat->edma = MAC_GET_VAL(mac_hfr0, MAC_HW_F0, EDMA);
	/* Hardware feature register 1 */
	hw_feat->rx_fifo_size  =
		MAC_GET_VAL(mac_hfr1, MAC_HW_F1, RXFIFOSIZE);
	hw_feat->tx_fifo_size  =
		MAC_GET_VAL(mac_hfr1, MAC_HW_F1, TXFIFOSIZE);
	hw_feat->osten  = MAC_GET_VAL(mac_hfr1, MAC_HW_F1, OSTEN);
	hw_feat->ptoen  = MAC_GET_VAL(mac_hfr1, MAC_HW_F1, PTOEN);
	hw_feat->adv_ts_hi     = MAC_GET_VAL(mac_hfr1, MAC_HW_F1, ADVTHWORD);
	hw_feat->dma_width     = MAC_GET_VAL(mac_hfr1, MAC_HW_F1, ADDR64);
	hw_feat->dcb           = MAC_GET_VAL(mac_hfr1, MAC_HW_F1, DCBEN);
	hw_feat->sph           = MAC_GET_VAL(mac_hfr1, MAC_HW_F1, SPHEN);
	hw_feat->tso           = MAC_GET_VAL(mac_hfr1, MAC_HW_F1, TSOEN);
	hw_feat->dma_debug     = MAC_GET_VAL(mac_hfr1, MAC_HW_F1, DBGMEMA);
	hw_feat->rss           = MAC_GET_VAL(mac_hfr1, MAC_HW_F1, RSSEN);
	hw_feat->tc_cnt        = MAC_GET_VAL(mac_hfr1, MAC_HW_F1, NUMTC);
	hw_feat->hash_table_size =
		MAC_GET_VAL(mac_hfr1, MAC_HW_F1, HASHTBLSZ);
	hw_feat->l3l4_filter_num =
		MAC_GET_VAL(mac_hfr1, MAC_HW_F1, L3L4FNUM);
	/* Hardware feature register 2 */
	hw_feat->rx_q_cnt     = MAC_GET_VAL(mac_hfr2, MAC_HW_F2, RXQCNT);
	hw_feat->tx_q_cnt     = MAC_GET_VAL(mac_hfr2, MAC_HW_F2, TXQCNT);
	hw_feat->rx_ch_cnt    = MAC_GET_VAL(mac_hfr2, MAC_HW_F2, RXCHCNT);
	hw_feat->tx_ch_cnt    = MAC_GET_VAL(mac_hfr2, MAC_HW_F2, TXCHCNT);
	hw_feat->pps_out_num  = MAC_GET_VAL(mac_hfr2, MAC_HW_F2, PPSOUTNUM);
	hw_feat->aux_snap_num = MAC_GET_VAL(mac_hfr2, MAC_HW_F2, AUXSNAPNUM);
	hw_feat->tc_cnt++;
	hw_feat->rx_q_cnt++;
	hw_feat->tx_q_cnt++;

	return 0;
}
