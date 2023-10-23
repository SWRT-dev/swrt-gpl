/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef DATAPATH_MISC30_H
#define DATAPATH_MISC30_H

#define PMAC_MAX_NUM  16
#define PAMC_LAN_MAX_NUM 7
#define VAP_OFFSET 8
#define VAP_MASK  0xF
#define VAP_DSL_OFFSET 3
#define NEW_CBM_API 1

#define GSWIP_L 0
#define GSWIP_R 1
#define MAX_SUBIF_PER_PORT 16
#define PMAC_SIZE 8

struct gsw_itf {
	u8 ep; /*-1 means no assigned yet for dynamic case */
	u8 fixed; /*fixed (1) or dynamically allocate (0)*/
	u16 start;
	u16 end;
	u16 n;
};

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_CPUFREQ)
/* threshold data for D0:D3 */
struct dp_coc_threshold {
	int	th_d0;
	int	th_d1;
	int	th_d2;
	int	th_d3;
};
#endif

#define RESET_PMAC_PORTMAP(pmac, port_id) do { \
	if ((port_id) <= 7) \
		(pmac)->port_map2 = ~(1 << (port_id)); \
	else \
		(pmac)->port_map = ~(1 << (port_id - 8)); } \
	while (0)

#define SET_PMAC_PORTMAP(pmac, port_id) do { \
	if ((port_id) <= 7) \
		(pmac)->port_map2 = 1 << (port_id); \
	else \
		(pmac)->port_map = (1 << (port_id - 8)); } \
	while (0)

#define SET_PMAC_SUBIF(pmac, subif) do { \
	(pmac)->src_sub_inf_id2 = (subif) & 0xff; \
	(pmac)->src_sub_inf_id =  ((subif) >> 8) & 0x1f; } \
	while (0)

#ifdef CONFIG_DEBUG_FS
int dp_sub_proc_install_30(void);
#else
static inline int dp_sub_proc_install_30(void) { return 0; }
#endif
void dp_sys_mib_reset_30(u32 flag);
int dp_set_gsw_parser_30(u8 flag, u8 cpu, u8 mpe1, u8 mpe2, u8 mpe3);
int dp_get_gsw_parser_30(u8 *cpu, u8 *mpe1, u8 *mpe2, u8 *mpe3);
int gsw_mib_reset_30(int dev, u32 flag);
int dp_pmac_set_30(int inst, u32 port, dp_pmac_cfg_t *pmac_cfg);
int32_t dp_rx_30(struct sk_buff *skb, u32 flags);
int _dp_tx(struct sk_buff *skb, struct dp_tx_common *cmn);

static inline char *parser_flag_str(u8 f)
{
	if (f == DP_PARSER_F_DISABLE)
		return "No Parser";
	else if (f == DP_PARSER_F_HDR_ENABLE)
		return "Parser Flag only";
	else if (f == DP_PARSER_F_HDR_OFFSETS_ENABLE)
		return "Parser Full";
	else
		return "Reserved";
}

ssize_t proc_get_qid_via_index(struct file *file, const char *buf,
			       size_t count, loff_t *ppos);
int lookup_dump30(struct seq_file *s, int pos);
int lookup_start30(void);
ssize_t proc_get_qid_via_index30(struct file *file, const char *buf,
				 size_t count, loff_t *ppos);
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_CPUFREQ)
int dp_coc_cpufreq_exit(void);
int dp_coc_cpufreq_init(void);
int dp_handle_cpufreq_event_30(int event_id, void *cfg);
void proc_coc_read_30(struct seq_file *s);
ssize_t proc_coc_write_30(struct file *file, const char *buf, size_t count,
			  loff_t *ppos);
#endif
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_MIB)
int dp_reset_sys_mib(u32 flag);
int dp_mib_init(u32 flag);
void dp_mib_exit(void);
#endif

#endif

