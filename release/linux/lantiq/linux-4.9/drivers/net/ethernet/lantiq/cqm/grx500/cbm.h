#ifndef __CBM_COMMON_H__
#define __CBM_COMMON_H__
#include <linux/unistd.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/err.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/sysctl.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/irq.h>
#include <linux/seq_file.h>
#include <linux/semaphore.h>

#include <lantiq.h>
#include <lantiq_soc.h>
#include <linux/netdevice.h>
#include <linux/ltq_hwmcpy.h>
#include <net/lantiq_cbm_api.h>
#include <net/datapath_api.h>
#include "cbm.h"
#include "reg/cbm_ls.h"
#include "reg/cbm.h"
#include "reg/cbm_dqm.h"
#include "reg/cbm_eqm.h"
#include "reg/cbm_desc64b.h"
#include "reg/fsqm.h"
#include <net/drv_tmu_ll.h>

/*#define CBM_DEBUG 1*/
/*#define CBM_DEBUG_LVL_1 1*/

/*ENUM*/
enum {
	CBM1_INT_VPE0 = 174,
	CBM1_INT_VPE1 = 175,
	CBM1_INT_VPE2 = 176,
	CBM1_INT_VPE3 = 177,
	CBM2_INT_VPE0 = 178,
	CBM2_INT_VPE1 = 179,
	CBM2_INT_VPE2 = 180,
	CBM2_INT_VPE3 = 181,
};

enum {
	DQM_DMA = 1,
	EQM_DMA = 2
};

enum {
	SBA0_STD = 1,
	SBA0_JBO = 2,
	SBA1_STD = 3,
	SBA1_JBO = 4,
};

enum CBM_EGP_TYPE {
	CPU_EG_PORT = 0,
	MPE_EG_PORT,
	WAVE500_PORT,
	LRO_PORT,
	GSWIPL_LP_P1,
	GSWIPL_LP_P2,
	GSWIPL_LP_P3,
	GSWIPL_LP_P4,
	GSWIPL_LP_P5,
	GSWIPL_LP_P6,
	GSWIPL_HP_P1,
	GSWIPL_HP_P2,
	GSWIPL_HP_P3,
	GSWIPL_HP_P4,
	GSWIPL_HP_P5,
	GSWIPL_HP_P6,
	/*    GSWIPR_VRX318,*/
	GSWIPR_REDIRECT,
	GSWIPR_ETHWAN,
	VRX318_PORT,
	MAX_DQM_PORT,
};

/*! PMAC port flag */
enum FREE_FLAG {
	P_FREE = 0,/*! The port is free */
	/*the port is already allocated to some driver,
	 *but not registered or no need to register at all.\n
	 *for example, LRO/CAPWA, only need to allocate,
	 *but no need to register
	 */
	P_ALLOCATED,
	P_REGISTERED,/*! Registered already. */
	P_FLAG_NO_VALI/*! Not valid flag */
};

enum DQM_PORT_TYPE {
	DQM_CPU_START_ID = 0,
	DQM_CPU_END_ID = 4,
	DQM_SCPU_START_ID = 5,
	DQM_SCPU_END_ID = 5,
	DQM_DMA_START_ID = 6,
	DQM_DMA_END_ID = 22,
	DQM_LDMA_START_ID = 23,
	DQM_LDMA_END_ID = 23,
	DQM_WAVE_START_ID = 24,
	DQM_WAVE_END_ID = 26,
	DQM_CHECKSUM_ID = 20,
	DQM_DIRECTPATH_RX = 21
};

enum EQM_PORT_TYPE {
	EQM_CPU_START_ID = 0,
	EQM_CPU_END_ID = 3,
	EQM_DMA_START_ID = 5,
	EQM_DMA_END_ID = 8,
};

enum EQM_DQM_PORT_TYPE {
	DQM_CPU_TYPE = 0,
	DQM_SCPU_TYPE = 5,
	DQM_DMA_TYPE = 6,
	DQM_LDMA_TYPE = 23,
	DQM_WAVE_TYPE = 24,
	EQM_CPU_TYPE = 100,
	EQM_DMA_TYPE = 200,
	EQM_TOE_TYPE = 9,
	EQM_DL_TYPE = 12,
	EQM_VRX318_TYPE = 15,
	NONE_TYPE = 0xffff
};

enum DP_DEV_TYPE {
	PMAC_ETH_LAN_START_ID = 1,
	PMAC_ETH_LAN_END_ID = 6,
	PMAC_ALLOC_START_ID = 7,
	PMAC_ALLOC_END_ID = 14,
	PMAC_ETH_WAN_ID = 15,
};

enum CBM_QIDT_STATE {
	STATE_CLASS = 0,
	STATE_EP = 1,
	STATE_MPE1 = 2,
	STATE_MPE2 = 3,
	STATE_ENC = 4,
	STATE_DEC = 5,
	STATE_FLOWID_LOW = 6,
	STATE_FLOWID_HIGH = 7,
	STATE_NONE = 0xffff
};

/*Macros*/

#define CONFIG_BRONZE_RTL307 1

/*Address Calculation*/
#define FSQM_LLT_RAM(fsqm_base, idx) \
(((fsqm_base) + RAM + ((idx) << 2)))
#define FSQM_RCNT(fsqm_base, idx) \
(((fsqm_base) + RCNT + ((idx) << 2)))
#define CBM_EQM_CPU_PORT(idx, reg) \
(CFG_CPU_IGP_0 + (idx) * 0x1000 + (offsetof(struct cbm_eqm_cpu_igp_reg, reg)))
#define CBM_EQM_DMA_PORT(idx, reg) \
(CFG_DMA_IGP_5 + (idx) * 0x1000 + (offsetof(struct cbm_eqm_dma_igp_reg, reg)))
#define CBM_DQM_CPU_PORT(idx, reg) \
(CFG_CPU_EGP_0 + (idx) * 0x1000 + (offsetof(struct cbm_dqm_cpu_egp_reg, reg)))
#define CBM_DQM_SCPU_PORT(idx, reg) \
(CFG_CPU_EGP_0 + (idx) * 0x1000 + (offsetof(struct cbm_dqm_scpu_egp_reg, reg)))
#define CBM_DQM_DMA_PORT(idx, reg) \
(CFG_DMA_EGP_6 + (idx) * 0x1000 + (offsetof(struct cbm_dqm_dma_egp_reg, reg)))
#define CBM_IOCU_ADDR(addr) \
(((addr) & 0x1FFFFFFF) | 0xC0000000)
#define CBM_INT_LINE(idx, reg) \
((idx) * 0x40 + offsetof(struct cbm_int_reg, reg))
#define CBM_INT_LINE_DQM(idx, reg) \
((idx) * 0x40 + offsetof(struct cbm_int_reg, reg))
#define CBM_EQM_DMA_DESC(pid, des_idx, jbo_flag) \
(SDESC0_0_IGP_5 + ((pid) - 5) * 0x1000 + (des_idx) * 16 + (jbo_flag) * 0x800)
#define CBM_DQM_DMA_DESC(pid, des_idx) \
(DESC0_0_EGP_5  + ((pid) - 5) * 0x1000 + (des_idx) * 16)
#define CBM_EQM_LDMA_DESC(des_idx)	(SDESC0_0_IGP_15 + (des_idx) * 8)

/*Default  Value Definition*/
#define DEFAULT_CPU_HWM         256
#define DEFAULT_DMA_HWM         128
#define CPU_EQM_PORT_NUM        4
#define SCPU_EQM_PORT_NUM       1
#define DMA_EQM_PORT_NUM        10
#define CPU_DQM_PORT_NUM        4
#define SCPU_DQM_PORT_NUM       1
#define DMA_DQM_PORT_NUM        17

#define CBM_QEQCNTR_SIZE        0x400
#define CBM_QDQCNTR_SIZE        0x400
#define CBM_BASIC_JBO_NUM       100
#define CBM_BASIC_STD_NUM       600
#define SBID_START 16
#define CBM_NOTFOUND             2
#define CBM_RESCHEDULE          4
#define CBM_PORT_MAX			64
#define CBM_EQM_PORT_MAX		16
#define CBM_DQM_DMA		1
#define PMAC_MAX_NUM 16
#define CBM_JUMBO_BUF_SPLIT_GUARD_BAND	16
#define CBM_MAX_RCNT 7
#define MAX_TMU_QUEUES 256
#define CBM_FIXED_RX_OFFSET \
(CBM_GRX550_DMA_DATA_OFFSET + NET_IP_ALIGN + NET_SKB_PAD)

/*Structure definition*/
#ifdef CONFIG_CPU_BIG_ENDIAN
struct flowid_desc {
	uint8_t row:5;
	uint8_t col:3;
};

#else
struct flowid_desc {
	uint8_t col:3;
	uint8_t row:5;
};

#endif
struct cbm_scpu_ptr_rtn {
	u32 ptr_rtn;
};

#ifdef CONFIG_CPU_LITTLE_ENDIAN
struct dma_desc {
	/* DWORD 0 */
	u32 dest_sub_if_id:15;
	u32 eth_type:2;
	u32 flow_id:8;
	u32 tunnel_id:4;
	u32 resv0:3;

	/* DWORD 1 */
	u32 classid:4;
	u32 resv1:4;
	u32 ep:4;
	u32 color:2;
	u32 mpe1:1;
	u32 mpe2:1;
	u32 enc:1;
	u32 dec:1;
	u32 nat:1;
	u32 tcp_err:1;
	u32 session_id:12;

	/*DWORD 2 */
	u32 data_ptr;

	/*DWORD 3*/
	u32 data_len:16;
	u32 mpoa_mode:2;
	u32 mpoa_pt:1;
	u32 qid:4;
	u32 byte_offset:3;
	u32 pdu_type:1;
	u32 dic:1;
	u32 eop:1;
	u32 sop:1;
	u32 c:1;
	u32 own:1;

};

#else
struct dma_desc {
	/* DWORD 1 */
	u32 session_id:12;
	u32 tcp_err:1;
	u32 nat:1;
	u32 dec:1;
	u32 enc:1;
	u32 mpe2:1;
	u32 mpe1:1;
	u32 color:2;
	u32 ep:4;
	u32 resv1:4;
	u32 classid:4;

	/* DWORD 0 */
	u32 resv0:3;
	u32 tunnel_id:4;
	u32 flow_id:8;
	u32 eth_type:2;
	u32 dest_sub_if_id:15;

	/*DWORD 3*/
	u32 own:1;
	u32 c:1;
	u32 sop:1;
	u32 eop:1;
	u32 dic:1;
	u32 pdu_type:1;
	u32 byte_offset:3;
	u32 qid:4;
	u32 mpoa_pt:1;
	u32 mpoa_mode:2;
	u32 data_len:16;

	/*DWORD 2 */
	u32 data_ptr;

};

struct cbm_ctrl {
	int id;
	struct device *dev;
	struct net_device dummy_dev;
	struct napi_struct napi;
	u32 jumbo_size_mask;
	const struct cbm_ops *g_cbm_ops;
	dma_addr_t dma_handle_std;
	dma_addr_t dma_handle_jbo;
};

#endif

struct cbm_desc_list {
	struct cbm_desc desc;
	struct cbm_desc_list *next;
};

struct cbm_qidt_mask {
	u32 classid_mask;
	u32 ep_mask;
	u32 mpe1_mask;
	u32 mpe2_mask;
	u32 enc_mask;
	u32 dec_mask;
	u32 flowid_lmask;
	u32 flowid_hmask;
};

struct cbm_qidt_elm {
	u32 clsid;
	u32 ep;
	u32 mpe1;
	u32 mpe2;
	u32 enc;
	u32 dec;
	u32 flowidl;
	u32 flowidh;
};

struct cbm_eqm_cpu_igp_reg {
	u32 cfg;
	u32 wm;
	u32 pocc;
	u32 eqpc;
	struct cbm_desc disc;
	u32 irncr;  /*Interrupt Capture :  Status + Clear */
	u32 irnicr;
	u32 irnen;  /*Interrupt Enable Mask */
	u32 resv0[2];
	u32 rcnt;
	u32 dicc;
	u32 rcntc;
	u32 nsbpc;
	u32 njbpc;
	u32 resv1[1];
	u32 dcntr;
	u32 resv2[12];
	u32 new_sptr;
	u32 resv3[3];
	u32 new_jptr;
	u32 resv4[27];

	struct cbm_desc desc0;
	struct cbm_desc desc1;
};

struct cbm_eqm_dma_igp_reg {
	u32 cfg;
	u32 wm;
	u32 pocc;
	u32 eqpc;
	struct cbm_desc disc;
	u32 irncr;
	u32 irnicr;
	u32 irnen;
	u32 resv0;
	u32 dptr;
	u32 resv1;
	u32 dicc;
};

struct cbm_dqm_cpu_egp_reg {
	u32 cfg;
	u32 dqpc;
	u32 resv0[6];
	u32 irncr;
	u32 irnicr;
	u32 irnen;
	u32 resv1;
	u32 dptr;
	u32 bprc;
	u32 resv2[18];
	u32 ptr_rtn;
	u32 resv3[31];
	struct cbm_desc desc0;
	struct cbm_desc desc1;
};

struct cbm_dqm_scpu_egp_reg {
	u32 cfg;
	u32 dqpc;
	u32 resv0[6];
	u32 irncr;
	u32 irnicr;
	u32 irnen;
	u32 resv1;
	u32 dptr;
	u32 bprc;
	u32 brptr;
	u32 resv2[17];
	struct cbm_scpu_ptr_rtn scpu_ptr_rtn[32];
	struct cbm_desc desc[32];
};

struct cbm_dqm_dma_egp_reg {
	u32 cfg;
	u32 dqpc;
	u32 resv0[6];
	u32 irncr;
	u32 irnicr;
	u32 irnen;
	u32 resv1;
	u32 dptr;
};

struct cbm_int_reg {
	u32 cbm_irncr;
	u32 cbm_irnicr;
	u32 cbm_irnen;
	u32 resv0[1];
	u32 igp_irncr;
	u32 igp_irnicr;
	u32 igp_irnen;
	u32 resv1[1];
	u32 egp_irncr;
	u32 egp_irnicr;
	u32 egp_irnen;
};

struct flow_id {
#ifdef CONFIG_CPU_LITTLE_ENDIAN

	u32 q0:4;
	u32 q1:4;
	u32 q2:4;
	u32 q3:4;
	u32 q4:4;
	u32 q5:4;
	u32 q6:4;
	u32 q7:4;

#else

	u32 q7:4;
	u32 q6:4;
	u32 q5:4;
	u32 q4:4;
	u32 q3:4;
	u32 q2:4;
	u32 q1:4;
	u32 q0:4;
#endif
};

struct cbm_egp_tmu_map {
	u32 egp_type;
	u32 qid;/* qid value*/
	/*u32 q_num;*//*total queue number*/
	u32 inc;/*increase qid with class id*/
	struct cbm_qidt_elm qidt_elm;
	struct cbm_qidt_mask qidt_mask;
};

struct cbm_egp_map {
	u32 epg;
	u32 pmac;
	u32 port_type;
};

/*! CBM port information*/
struct cbm_dqm_port_info {
	u32 dma_ch;
	u32 dma_dt_ch;
	u32 dma_dt_ctrl;
	u32 dma_dt_init_type;
	u32 def_qid;
	u32 def_schd;
	u32 def_tmu_pid;
	void		*cbm_buf_free_base;
	u32	num_free_entries;/*!< Number of Free Port entries */
	cbm_dq_info_t	deq_info;
	u32 egp_type;
};

struct cbm_eqm_port_info {
	u32 port_type;
	int		num_eq_ports;/*!< Number of Enqueue Ports */
	cbm_eq_info_t	eq_info;/*!<  EQ port info */
	u32 dma_dt_std_ch;
	u32 dma_dt_jum_ch;
	u32 dma_dt_ctrl;
	u32 dma_dt_init_type;
};

struct cbm_pmac_port_map {
	/*! port flag */
	enum FREE_FLAG  flags;
	struct module *owner;
	struct net_device *dev;
	u32 dev_port;
	u32 pmac;
	/*bit map to egp port*/
	u32 egp_port_map;
	/*queue numbers allocated to that pmac port*/
	u32 qid_num;
	u32 qids[16];/*qid array*/
	/*e.g. DP_F_FAST_ETH_LAN/DP_F_FAST_ETH_WAN/
	 *DP_F_DIRECT/High priority/Low priority
	 */
	u32 egp_type;
	struct list_head list;
};

struct rcnt_idx {
	u32 fsqm_idx;
	u32 frm_size;
	u32 rcnt_base;
	u32 buf_addr;
};

struct cbm_cntr_mode {
void __iomem *reg;
u32 msel_mask;
u32 msel_pos;
u32 qen_mask;
u32 qen_pos;
void (*reset_counter)(void);
};

struct cbm_base_addr {
	void __iomem *tmu_addr_base;
	void __iomem *cbm_addr_base;
	void __iomem *fsqm0_addr_base;
	void __iomem *fsqm1_addr_base;
	void __iomem *cbm_eqm_addr_base;
	void __iomem *cbm_dqm_addr_base;
	void __iomem *cbm_ls_addr_base;
	void __iomem *cbm_qidt_addr_base;
	void __iomem *cbm_qeqcnt_addr_base;
	void __iomem *cbm_qdqcnt_addr_base;
	void __iomem *cbm_dmadesc_addr_base;
};

struct cbm_buff_info {
	unsigned int std_buf_addr;
	unsigned int jbo_buf_addr;
	unsigned int std_buf_size;
	unsigned int jbo_buf_size;
	unsigned int std_frm_num;
	unsigned int jbo_frm_num;
	unsigned int std_frm_size;
	unsigned int jbo_frm_size;
	unsigned int std_fsqm_idx;
	unsigned int jbo_fsqm_idx;
};

struct cbm_buff_stat {
	unsigned int std_alloc_err;
	unsigned int jbo_alloc_err;
	unsigned int xmit_alloc_err;
};

struct cbm_jumbo_divide {
s32 jumbo_buf_used;
void *curr_jumbo_buf;
s32 curr_jumbo_buf_bytes_used;
s32 curr_jumbo_buf_alloc_no;
};

struct cbm_qlink_get {
u32 qid[EGRESS_QUEUE_ID_MAX];
u32 qlink_sbin[EGRESS_QUEUE_ID_MAX];
};

struct cbm_buff_stat *cbm_buff_stat_get(void);

#endif /* __CBM_COMMON_H__ */

