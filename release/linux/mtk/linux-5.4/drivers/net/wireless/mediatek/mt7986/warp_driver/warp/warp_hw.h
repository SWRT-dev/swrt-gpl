/*
 ***************************************************************************
 * MediaTek Inc.
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************

	Module Name: warp_hw HAL
	warp_hw.h
*/

#ifndef _WARP_HW_H_
#define _WARP_HW_H_

#include "regs/coda/wed_hw.h"
#include "regs/coda/wdma_hw.h"

struct warp_entry;
struct wed_entry;
struct wifi_entry;
struct wdma_entry;
struct warp_bus;
struct warp_cputracer;
struct warp_ring;
struct wed_ser_state;
struct woif_bus;

#define DMADONE_DONE 1
#define WED_POLL_MAX 100
#define PCIE_POLL_MODE_ASSERT 1
#define PCIE_POLL_MODE_ALWAYS 2
#define FREE_CR_SIZE (sizeof(u32))

#define MIOD_CNT 16
#define FB_CMD_CNT 1024
#define RRO_QUE_CNT 8192

enum {
	WED_DRAM_PROF_OFF = 0,
	WED_DRAM_PROF_TX,
	WED_DRAM_PROF_RX,
	WED_DRAM_PROF_MAX
};

enum {
	WARP_RESET_IDX_ONLY,
	WARP_RESET_IDX_MODULE,
	WARP_RESET_INTERFACE,
};

enum wed_int_agent {
	WPDMA_INT_AGENT,
	WDMA_INT_AGENT,
	ALL_INT_AGENT,
};

enum wed_dummy_cr_idx {
	WED_DUMMY_CR_FWDL = 0,
	WED_DUMMY_CR_WO_STATUS = 1
};

enum WO_FW_BIN_MODE {
	WO_FW_EMBEDDED,
	WO_FW_BIN,
	WO_FW_MAX
};

struct warp_io {
	unsigned long base_addr;
};

void warp_int_ctrl_hw(struct wed_entry *wed, struct wifi_entry *wifi,
	struct wdma_entry *wdma, u32 int_agent,
	u8 enable, u32 pcie_ints_offset, int idx);
void warp_eint_ctrl_hw(struct wed_entry *wed, u8 enable);
#if defined(WED_DYNAMIC_TXBM_SUPPORT) || defined(WED_DYNAMIC_RXBM_SUPPORT)
void dybm_eint_ctrl(struct wed_entry *wed, bool enable, u8 type);
#endif	/* WED_DYNAMIC_TXBM_SUPPORT || WED_DYNAMIC_RXBM_SUPPORT */
void warp_eint_init_hw(struct wed_entry *wed);
void warp_eint_get_stat_hw(struct wed_entry *wed, u32 *state);
void warp_eint_clr_dybm_stat_hw(struct wed_entry *wed);
void warp_eint_work_hw(struct wed_entry *wed, u32 status);
void warp_wed_init_hw(struct wed_entry *wed, struct wdma_entry *wdma);
void warp_wed_512_support_hw(struct wed_entry *wed, u8 *enable);
void warp_wdma_init_hw(struct wed_entry *wed, struct wdma_entry *wdma, int idx);
void warp_wdma_ring_init_hw(struct wed_entry *wed, struct wdma_entry *wdma);
void warp_dma_ctrl_hw(struct wed_entry *wed, u8 txrx);
void warp_bus_init_hw(struct warp_bus *bus);
void warp_trace_set_hw(struct warp_cputracer *tracer);
void warp_bfm_update_hw(struct wed_entry *wed, u8 reduce);
void warp_btkn_update_hw(struct wed_entry *wed, u8 reduce);
u32 warp_get_recycle_grp_idx(struct wed_entry *wed);
void warp_bfm_get_tx_freecnt_hw(struct wed_entry *wed, u32 *cnt);
void warp_bfm_init_hw(struct wed_entry *wed);
void warp_mtable_build_hw(struct warp_entry *warp);
void warp_atc_set_hw(struct wifi_entry *wifi, struct warp_bus *bus, int idx,
			u8 enable);
void warp_wifi_set_hw(struct wed_entry *wed, struct wifi_entry *wifi);
void warp_bus_set_hw(struct wed_entry *wed, struct warp_bus *bus,
			int idx, bool msi_enable, u32 hif_type);
void warp_pdma_mask_set_hw(struct wed_entry *wed, u32 int_mask);
void warp_restore_hw(struct wed_entry *wed, struct wdma_entry *wdma);
int warp_reset_hw(struct wed_entry *wed, u32 reset_type);
int wed_dram_prof(struct wed_entry *wed, u8 direction);
void warp_ser_trigger_hw(struct wifi_entry *wifi);
void warp_ser_update_hw(struct wed_entry *wed, struct wed_ser_state *state);
void warp_dbginfo_dump_wed_hw(struct wed_entry *wed);
void warp_procinfo_dump_cfg_hw(struct warp_entry *warp, struct seq_file *seq);
void warp_procinfo_dump_rxinfo_hw(struct warp_entry *warp, struct seq_file *output);
void warp_procinfo_dump_txinfo_hw(struct warp_entry *warp, struct seq_file *output);

void warp_tx_ring_get_hw(struct warp_entry *warp, struct warp_ring *ring,
			 u8 idx);
int warp_tx_ring_init_hw(struct wed_entry *wed, struct wifi_entry *wifi);
void warp_wdma_rx_ring_get_hw(struct warp_ring *ring, u8 idx);
void warp_wdma_tx_ring_get_hw(struct warp_ring *ring, u8 idx);
int warp_rx_ring_init_hw(struct wed_entry *wed, struct wifi_entry *wifi);
void warp_bus_cputracer_work_hw(struct warp_cputracer *tracer);
void warp_wed_ver(struct wed_entry *wed);
void warp_conf_hwcap(struct wed_entry *wed);
int warp_rx_data_ring_init_hw(struct wed_entry *wed, struct wifi_entry *wifi);
int warp_rx_dybm_mod_thrd(struct wed_entry *wed, u32 operation);
int warp_rx_bm_init_hw(struct wed_entry *wed, struct wifi_entry *wifi);
void warp_rx_data_ring_get_hw(struct warp_entry *warp,
			      struct warp_rx_ring *ring, u8 idx);

int warp_dummy_cr_set(struct wed_entry *wed, u8 index, u32 value);

int warp_dummy_cr_get(struct wed_entry *wed, u8 index, u32 *value);

int warp_rx_rro_init_hw(struct wed_entry *wed, struct wifi_entry *wifi);

bool warp_rx_dybm_cache_drained(struct wed_entry *wed);
int warp_rx_dybm_w_cache(struct wed_entry *wed, u32 dma_pa);
int warp_rx_dybm_r_cache(struct wed_entry *wed, struct warp_dma_buf *desc);

int warp_rx_route_qm_init_hw(struct wed_entry *wed, struct wifi_entry *wifi);
u32 warp_rxbm_left_query(struct wed_entry *wed);
u32 warp_woif_bus_get_tx_res(struct woif_bus *bus);
void warp_woif_bus_kickout(struct woif_bus *bus, int cpu_idx);
void warp_woif_bus_init_hw(struct woif_bus *bus);
void warp_woif_bus_get_rx_res(struct woif_bus *bus, u32 *didx, u32 *cidx);
void warp_woif_bus_set_rx_res(struct woif_bus *bus, u32 cidx);
void bus_setup(struct warp_bus *bus);
u8 warp_get_pcie_slot(struct pci_dev *pdev);
u8 *warp_get_wo_bin_ptr(u8 wed_idx);
u32 warp_get_wo_bin_size(u8 wed_idx);
void warp_get_dts_idx(u8 *dts_idx);
char *warp_get_wo_emi_node(u8 wed_idx);
char *warp_get_wo_ilm_node(u8 wed_idx);
int warp_wed_rro_init(struct wed_entry *wed);
void warp_whole_chip_wo_reset(void);
void warp_wo_reset(u8 wed_idx);
void warp_wo_pc_lr_cr_dump(u8 wed_idx);
void warp_wo_set_apsrc_idle(u8 wed_idx);
void warp_wdma_int_sel(struct wed_entry *wed,int idx);

#endif
