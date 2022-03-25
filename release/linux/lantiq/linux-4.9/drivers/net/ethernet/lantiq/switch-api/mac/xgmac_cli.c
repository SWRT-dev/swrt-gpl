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
#include <lmac_api.h>
#include <xgmac_mdio.h>
#include <mac_tx_fifo.h>

void cli_set_mtl_tx(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_mtl_tx_mode(ops, pdata->tx_sf_mode);
			xgmac_set_mtl_tx_thresh(ops,
						pdata->tx_threshold);
		}
	} else {
		xgmac_set_mtl_tx_mode(pdev, pdata->tx_sf_mode);
		xgmac_set_mtl_tx_thresh(pdev, pdata->tx_threshold);
	}
}

void cli_set_mtl_rx(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_mtl_rx_mode(ops, pdata->rx_sf_mode);
			xgmac_set_mtl_rx_thresh(ops,
						pdata->rx_threshold);
		}
	} else {
		xgmac_set_mtl_rx_mode(pdev, pdata->rx_sf_mode);
		xgmac_set_mtl_rx_thresh(pdev, pdata->rx_threshold);
	}
}

void cli_flow_ctrl_thresh(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_flow_control_threshold(ops,
							 pdata->rfa,
							 pdata->rfd);
		}
	} else {
		xgmac_set_flow_control_threshold(pdev, pdata->rfa, pdata->rfd);
	}
}

void cli_set_tstamp_addend(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_tstamp_addend(ops,
						pdata->tstamp_addend);
		}
	} else {
		xgmac_set_tstamp_addend(pdev, pdata->tstamp_addend);
	}
}

void cli_set_tstamp_enable(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_enable_ts(ops);
		}
	} else {
		mac_enable_ts(pdev);
	}
}

void cli_set_tstamp_disable(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_disable_ts(ops);
		}
	} else {
		mac_disable_ts(pdev);
	}
}

void cli_set_hwtstamp_settings(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_hwtstamp_settings(ops,
						    pdata->tstamp_config.tx_type,
						    pdata->tstamp_config.rx_filter);
		}
	} else {
		xgmac_set_hwtstamp_settings(pdev,
					    pdata->tstamp_config.tx_type,
					    pdata->tstamp_config.rx_filter);
	}
}

void cli_flush_tx_queues(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_flush_tx_queues(ops);
		}
	} else {
		xgmac_flush_tx_queues(pdev);
	}
}

void cli_set_debug_ctl(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_debug_ctl(ops,
					    pdata->dbg_en, pdata->dbg_mode);
		}
	} else {
		xgmac_set_debug_ctl(pdev, pdata->dbg_en, pdata->dbg_mode);
	}
}

void cli_set_tx_debug_data(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_tx_debug_data(ops, pdata->dbg_pktstate);
		}
	} else {
		xgmac_tx_debug_data(pdev, pdata->dbg_pktstate);
	}
}

void cli_set_rx_debug_data(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_rx_debug_data(ops,
					    &pdata->dbg_pktstate,
					    &pdata->dbg_byteen);
		}
	} else {
		xgmac_rx_debug_data(pdev, &pdata->dbg_pktstate,
				    &pdata->dbg_byteen);
	}
}

void cli_set_debug_data(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_rx_debug_data(ops,
					    &pdata->dbg_pktstate,
					    &pdata->dbg_byteen);
		}
	} else {
		xgmac_set_debug_data(pdev, pdata->dbg_data);
	}
}

void cli_set_rx_debugctrl_int(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_rx_debugctrl_int(ops,
						   pdata->dbg_pktie);
		}
	} else {
		xgmac_set_rx_debugctrl_int(pdev, pdata->dbg_pktie);
	}
}

void cli_set_fifo_reset(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_fifo_reset(ops,
					     pdata->dbg_rst_sel,
					     pdata->dbg_rst_all);
		}
	} else {
		xgmac_set_fifo_reset(pdev, pdata->dbg_rst_sel,
				     pdata->dbg_rst_all);
	}
}

void cli_set_fup_fep_pkt(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_forward_fup_fep_pkt(ops,
						  pdata->fup, pdata->fef);
		}
	} else {
		xgmac_forward_fup_fep_pkt(pdev, pdata->fup, pdata->fef);
	}
}

void cli_set_int(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_int_enable(ops);
			xgmac_set_mac_int(ops, pdata->enable_mac_int, 1);
			xgmac_set_mtl_int(ops, pdata->enable_mtl_int);
		}
	} else {
		mac_int_enable(pdev);
		xgmac_set_mac_int(pdev, pdata->enable_mac_int, 1);
		xgmac_set_mtl_int(pdev, pdata->enable_mtl_int);
	}
}

/* MAC REGISTER SETTINGS */
void cli_set_mac_enable(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);

			if (pdata->mac_en)
				xgmac_powerup(ops);
			else
				xgmac_powerdown(ops);
		}
	} else {
		if (pdata->mac_en)
			xgmac_powerup(pdev);
		else
			xgmac_powerdown(pdev);
	}
}

void cli_set_mac_address(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_mac_address(ops, pdata->mac_addr);
		}
	} else {
		xgmac_set_mac_address(pdev, pdata->mac_addr);
	}
}

void cli_set_mac_rx_mode(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_promiscuous_mode(ops,
						   pdata->promisc_mode);
			xgmac_set_all_multicast_mode(ops,
						     pdata->all_mcast_mode);
		}
	} else {
		xgmac_set_promiscuous_mode(pdev, pdata->promisc_mode);
		xgmac_set_all_multicast_mode(pdev, pdata->all_mcast_mode);
	}
}

void cli_set_mtu(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	mac_printf("MTU going to set %d\n", pdata->mtu);

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_set_mtu(ops, pdata->mtu);
		}
	} else {
		mac_set_mtu(pdev, pdata->mtu);
	}
}

void cli_set_pause_frame_ctrl(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);

			if (pdata->pause_frm_enable) {
				xgmac_enable_tx_flow_ctl(ops,
							 pdata->pause_time);
				xgmac_enable_rx_flow_ctl(ops);
			} else {
				xgmac_disable_tx_flow_ctl(ops);
				xgmac_disable_rx_flow_ctl(ops);
			}
		}
	} else {
		if (pdata->pause_frm_enable) {
			xgmac_enable_tx_flow_ctl(pdev, pdata->pause_time);
			xgmac_enable_rx_flow_ctl(pdev);
		} else {
			xgmac_disable_tx_flow_ctl(pdev);
			xgmac_disable_rx_flow_ctl(pdev);
		}
	}
}

void cli_set_pause_frame_filter(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_pause_frame_filtering(ops,
						    pdata->pause_filter);
		}
	} else {
		xgmac_pause_frame_filtering(pdev, pdata->pause_filter);
	}
}

void cli_initiate_pause_tx(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_initiate_pause_tx(ops);
		}
	} else {
		xgmac_initiate_pause_tx(pdev);
	}
}

void cli_set_mac_speed(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_set_physpeed(ops, pdata->phy_speed);
		}
	} else {
		mac_set_physpeed(pdev, pdata->phy_speed);
	}
}

void cli_set_duplex_mode(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_set_duplex(ops, pdata->duplex_mode);
		}
	} else {
		mac_set_duplex(pdev, pdata->duplex_mode);
	}
}

void cli_set_csum_offload(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_checksum_offload(ops,
						   pdata->rx_checksum_offload);
		}
	} else {
		xgmac_set_checksum_offload(pdev, pdata->rx_checksum_offload);
	}
}

void cli_set_loopback(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_config_loopback(ops, pdata->loopback);
		}
	} else {
		mac_config_loopback(pdev, pdata->loopback);
	}
}

void cli_set_eee_mode(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_eee_timer(ops,
					    pdata->twt, pdata->lst);
			xgmac_set_eee_mode(ops, pdata->eee_enable);
		}
	} else {
		xgmac_set_eee_timer(pdev, pdata->twt, pdata->lst);
		xgmac_set_eee_mode(pdev, pdata->eee_enable);
	}
}

void cli_set_crc_strip_type(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_crc_strip_type(ops,
						 pdata->crc_strip_type);
		}
	} else {
		xgmac_set_crc_strip_type(pdev, pdata->crc_strip_type);
	}
}

void cli_set_crc_strip_acs(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_acs(ops, pdata->padcrc_strip);
		}
	} else {
		xgmac_set_acs(pdev, pdata->padcrc_strip);
	}
}

void cli_set_ipg(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_config_ipg(ops, pdata->ipg);
		}
	} else {
		mac_config_ipg(pdev, pdata->ipg);
	}
}

void cli_set_pmt_magic(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_magic_pmt(ops, pdata->magic_pkt_en);
		}
	} else {
		xgmac_set_magic_pmt(pdev, pdata->magic_pkt_en);
	}
}

void cli_set_pmt_gucast(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_pmt_gucast(ops, pdata->gucast);
		}
	} else {
		xgmac_set_pmt_gucast(pdev, pdata->gucast);
	}
}

void cli_set_pmt_rwk(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_rwk_filter_registers(ops,
						       pdata->rwk_filter_length,
						       pdata->rwk_filter_values);
			xgmac_set_rwk_pmt(ops, pdata->rwk_pkt_en);
		}
	} else {
		xgmac_set_rwk_filter_registers(pdev,
					       pdata->rwk_filter_length,
					       pdata->rwk_filter_values);
		xgmac_set_rwk_pmt(pdev, pdata->rwk_pkt_en);
	}
}

int cli_get_rmon(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	if (pdata->set_all)
		xgmac_get_stats_all(pdev);
	else
		xgmac_get_stats(pdev);

	return 0;
}

/* OTHERS */
void cli_clear_rmon_all(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_clear_rmon(ops, pdata->rmon_reset);
		}
	} else {
		xgmac_clear_rmon(pdev, pdata->rmon_reset);
	}
}

void cli_reset(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_reset(ops);
		}
	} else {
		mac_reset(pdev);
	}
}

void cli_init(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_init(ops);
		}
	} else {
		mac_init(pdev);
	}
}

void cli_set_extcfg(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_extcfg(ops, pdata->extcfg);
		}
	} else {
		xgmac_set_extcfg(pdev, pdata->extcfg);
	}
}

void cli_set_macrxtxcfg(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_mac_rxtx(ops, pdata->wd, pdata->jd);
		}
	} else {
		xgmac_set_mac_rxtx(pdev, pdata->wd, pdata->jd);
	}
}

void cli_set_linksts(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_set_linksts(ops, pdata->linksts);
		}
	} else {
		mac_set_linksts(pdev, pdata->linksts);
	}
}

void cli_set_lpitx(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_mac_lpitx(ops, pdata->lpitxa);
		}
	} else {
		xgmac_set_mac_lpitx(pdev, pdata->lpitxa);
	}
}

void cli_set_mdio_cl(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mdio_set_clause(ops, pdata->mdio_cl,
					pdata->phy_id);
		}
	} else {
		mdio_set_clause(pdev, pdata->mdio_cl, pdata->phy_id);
	}
}

void cli_mdio_rd_cont(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			print_mdio_rd_cnt(ops,
					  pdata->dev_adr,
					  pdata->phy_id,
					  pdata->phy_reg_st,
					  pdata->phy_reg_end);
		}
	} else {
		print_mdio_rd_cnt(pdev,
				  pdata->dev_adr,
				  pdata->phy_id,
				  pdata->phy_reg_st,
				  pdata->phy_reg_end);
	}
}

void cli_mdio_rd(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_mdio_single_rd(ops,
					     pdata->dev_adr,
					     pdata->phy_id,
					     pdata->phy_reg,
					     &pdata->phy_data);
		}
	} else {
		xgmac_mdio_single_rd(pdev,
				     pdata->dev_adr,
				     pdata->phy_id,
				     pdata->phy_reg,
				     &pdata->phy_data);
	}
}

void cli_mdio_wr(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_mdio_single_wr(ops,
					     pdata->dev_adr,
					     pdata->phy_id,
					     pdata->phy_reg,
					     pdata->phy_data);
		}
	} else {
		xgmac_mdio_single_wr(pdev,
				     pdata->dev_adr,
				     pdata->phy_id,
				     pdata->phy_reg,
				     pdata->phy_data);
	}
}

void cli_set_mdio_int(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mdio_set_interrupt(ops, pdata->mdio_int);
		}
	} else {
		mdio_set_interrupt(pdev, pdata->mdio_int);
	}
}

void cli_set_fcsgen(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			mac_set_fcs_gen(ops, pdata->fcsgen);
		}
	} else {
		mac_set_fcs_gen(pdev, pdata->fcsgen);
	}
}

void cli_set_txtstamp_mode(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_ptp_txtstamp_mode(ops,
						pdata->snaptype,
						pdata->tsmstrena,
						pdata->tsevntena);
		}
	} else {
		xgmac_ptp_txtstamp_mode(pdev,
					pdata->snaptype,
					pdata->tsmstrena,
					pdata->tsevntena);
	}
}

void cli_set_gint(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_gint(ops, pdata->val);
		}
	} else {
		xgmac_set_gint(pdev, pdata->val);
	}
}

void cli_set_rxcrc(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_rxcrc(ops, pdata->val);
		}
	} else {
		xgmac_set_rxcrc(pdev, pdata->val);
	}
}

int cli_get_fifo(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			print_fifo(ops);
		}
	} else {
		print_fifo(pdev);
	}

	return 0;
}

void cli_add_fifo(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			fifo_entry_add(ops, 1, 1, 1, 0, 0, 0);
		}
	} else {
		fifo_entry_add(pdev, 1, 1, 1, 0, 0, 0);
	}
}

void cli_del_fifo(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			fifo_entry_del(ops, pdata->rec_id);
		}
	} else {
		fifo_entry_del(pdev, pdata->rec_id);
	}
}
void cli_set_extsrc(void *pdev)
{
	u32 i = 0;
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);
	struct mac_ops *ops;

	if (pdata->set_all) {
		for (i = 0; i < pdata->max_mac; i++) {
			ops = gsw_get_mac_ops(0, i);
			xgmac_set_exttime_source(ops, pdata->val);
		}
	} else {
		xgmac_set_exttime_source(pdev, pdata->val);
	}
}

void cli_test_all_reg(void *pdev)
{
	lmac_test_all_reg(pdev);
	gswss_test_all_reg(&adap_priv_data);
}

void cli_rx_packet_slavemode(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	pdata->dbg_pktstate = 0;
	pdata->dbg_data = 0;
	pdata->dbg_byteen = 0;

	pdata->dbg_en = 1;
	pdata->dbg_mode = 0,
	       xgmac_set_debug_ctl(pdev, pdata->dbg_en, pdata->dbg_mode);

	xgmac_set_rx_debugctrl_int(pdev, 1);

	pdata->dbg_rst_all = 1,
	       xgmac_set_fifo_reset(pdev, 0, pdata->dbg_rst_all);

	// Poll for the packet data
	xgmac_poll_pkt_rx(pdata);

	pdata->dbg_pktstate = RX_PKT_DATA;
	xgmac_rx_debug_data(pdev, &pdata->dbg_pktstate, &pdata->dbg_byteen);
	xgmac_get_debug_data(pdev, &pdata->dbg_data);

	xgmac_clr_debug_sts(pdev);

	mac_printf("Packet State %x\n", pdata->dbg_pktstate);
	mac_printf("Byteen %x\n", pdata->dbg_byteen);
	mac_printf("Dbg Data %x\n", pdata->dbg_data);
}

void cli_rx_packet_dbgmode(void *pdev)
{
	struct mac_prv_data *pdata = GET_MAC_PDATA(pdev);

	pdata->dbg_pktstate = 0;
	pdata->dbg_data = 0;
	pdata->dbg_byteen = 0;

	pdata->dbg_en = 1;
	pdata->dbg_mode = 1,

	       xgmac_set_debug_ctl(pdev, pdata->dbg_en, pdata->dbg_mode);

	pdata->dbg_rst_all = 1,
	       xgmac_set_fifo_reset(pdev, 0, pdata->dbg_rst_all);

	pdata->dbg_pktstate = RX_PKT_DATA;
	xgmac_rx_debug_data(pdev, &pdata->dbg_pktstate, &pdata->dbg_byteen);
	xgmac_get_debug_data(pdev, &pdata->dbg_data);

	mac_printf("Packet State %x\n", pdata->dbg_pktstate);
	mac_printf("Byteen %x\n", pdata->dbg_byteen);
	mac_printf("Dbg Data %x\n", pdata->dbg_data);
}

