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
 * ========================================================================= */

#ifndef __XGMAC_PTP_H__
#define __XGMAC_PTP_H__

/* PTP V2 message type */
enum {
	PTP_MSGTYPE_SYNC			= 0x0,
	PTP_MSGTYPE_DELREQ			= 0x1,
	PTP_MSGTYPE_PDELREQ			= 0x2,
	PTP_MSGTYPE_PDELRESP			= 0x3,
	PTP_MSGTYPE_FLWUP			= 0x8,
	PTP_MSGTYPE_DELRESP			= 0x9,
	PTP_MSGTYPE_PDELRES_FLWUP		= 0xA,
	PTP_MSGTYPE_ANNOUNCE			= 0xB,
	PTP_MSGTYPE_SGNLNG			= 0xC,
	PTP_MSGTYPE_MNGMNT			= 0xD,
	PTP_MSGTYPE_INVALID			= 0xE,
};

/* Byte offset of data in the PTP V2 headers */
#define PTP_OFFS_MSG_TYPE		0
#define PTP_OFFS_FLAGS			6

/* To achieve 20 ns accuracy need 50 MHz clock update frequency
 * To achieve 4 ns 50 * (20/4) Mhz
 * Xgmac cannot have same clock update frequency and ptp clock frequency
 */

#define CLOCK_PRECISION_NS		4
#define CLOCK_UPDATE_FREQ		(50 * (20/CLOCK_PRECISION_NS))

/* Linux PTP driver cannot handle bigger values and erroneously
 * converts any value > +32767999 to -32768000.
 */
#define MAX_FREQ_ADJUSTMENT		32767999

#define IS_2STEP(pdata)	(pdata->tstamp_config.tx_type == HWTSTAMP_TX_ON)
#define IS_1STEP(pdata)	(pdata->tstamp_config.tx_type == HWTSTAMP_TX_ONESTEP_SYNC)

#ifdef __KERNEL__
int xgmac_tx_hwts(void *pdev, struct sk_buff *skb);
int xgmac_rx_hwts(void *pdev, struct sk_buff *skb);
int xgmac_set_hwts(void *pdev, struct ifreq *ifr);
int xgmac_get_hwts(void *pdev, struct ifreq *ifr);
int xgmac_ptp_init(void *pdev);
void xgmac_ptp_remove(void *pdev);
int xgmac_config_timer_reg(void *pdev, u32 mac_tscr);
int xgmac_ptp_tx_work(struct work_struct *work);
int xgmac_get_ts_info(void *pdev,
		      struct ethtool_ts_info *ts_info);
#endif


#endif
