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


#ifndef _XGMAC_CLI_
#define _XGMAC_CLI_

#include <xgmac_common.h>

/* CLI SET API's */

void cli_set_mtl_tx(void *pdev);
void cli_set_mtl_rx(void *pdev);
void cli_flow_ctrl_thresh(void *pdev);
void cli_set_tstamp_addend(void *pdev);
void cli_set_tstamp_enable(void *pdev);
void cli_set_hwtstamp_settings(void *pdev);
void cli_flush_tx_queues(void *pdev);
void cli_set_debug_ctl(void *pdev);
void cli_set_tx_debug_data(void *pdev);
void cli_set_rx_debug_data(void *pdev);
void cli_set_debug_data(void *pdev);
void cli_set_rx_debugctrl_int(void *pdev);
void cli_set_fifo_reset(void *pdev);
void cli_set_fup_fep_pkt(void *pdev);
void cli_set_int(void *pdev);
void cli_set_mac_enable(void *pdev);
void cli_set_mac_address(void *pdev);
void cli_set_mac_rx_mode(void *pdev);
void cli_set_mtu(void *pdev);
void cli_set_pause_frame_ctrl(void *pdev);
void cli_initiate_pause_tx(void *pdev);
void cli_set_mac_speed(void *pdev);
void cli_set_csum_offload(void *pdev);
void cli_set_loopback(void *pdev);
void cli_set_eee_mode(void *pdev);
void cli_set_crc_strip_type(void *pdev);
void cli_set_crc_strip_acs(void *pdev);
void cli_set_ipg(void *pdev);
void cli_set_pmt_magic(void *pdev);
void cli_set_pmt_rwk(void *pdev);
void cli_clear_rmon_all(void *pdev);
void cli_set_duplex_mode(void *pdev);
void cli_reset(void *pdev);
void cli_init(void *pdev);
void cli_test_all_reg(void *pdev);
void cli_rx_packet_dbgmode(void *pdev);
void cli_set_extcfg(void *pdev);
void cli_set_macrxtxcfg(void *pdev);
void cli_set_pmt_gucast(void *pdev);
void cli_rx_packet_slavemode(void *pdev);
void cli_set_ptpoff_settings(void *pdev);
void cli_set_ptpoff_mode(void *pdev);
void cli_set_linksts(void *pdev);
void cli_set_lpitx(void *pdev);
void cli_set_mdio_cl(void *pdev);
void cli_mdio_rd(void *pdev);
void cli_mdio_wr(void *pdev);
void cli_set_mdio_int(void *pdev);
void cli_set_fcsgen(void *pdev);
void cli_mdio_rd_cont(void *pdev);
void cli_set_tstamp_disable(void *pdev);
void cli_set_txtstamp_mode(void *pdev);
int cli_get_rmon(void *pdev);
void cli_set_pause_frame_filter(void *pdev);
void cli_set_gint(void *pdev);
void cli_set_rxcrc(void *pdev);
int cli_get_fifo(void *pdev);
void cli_add_fifo(void *pdev);
void cli_del_fifo(void *pdev);
void cli_set_extsrc(void *pdev);

#endif

