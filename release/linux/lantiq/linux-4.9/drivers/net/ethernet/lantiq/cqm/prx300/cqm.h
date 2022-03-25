#ifndef __CQM_H__
#define __CQM_H__
#include <linux/unistd.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/netdevice.h>
#include <linux/dma-mapping.h>
#include <linux/dma/lantiq_dmax.h>
#include <linux/skbuff.h>
#include <linux/if_ether.h>
#include <linux/klogging.h>
#include <net/lantiq_cbm_api.h>
#include <net/datapath_api.h>
#include <linux/debugfs.h>
#include "reg/cbm_ls.h"
#include "reg/cqem.h"
#include "reg/cqem_deq.h"
#include "reg/cqem_enq.h"
#include "reg/cqem_desc64b.h"
#include "reg/fsqm.h"
#include "reg/buffer_manager.h"
#include "reg/qos_wred.h"
#include "reg/txpush_dqm.h"
#include "reg/tx_manager.h"
#include "reg/cqem_pon_ip_if.h"
#include "cqm_config.h"

#define FREE_BUDGET 30
/*#define CBM_DEBUG 1*/
/*#define CBM_DEBUG_LVL_1 1*/
#define FLAG_PIB_ENABLE		BIT(0)
#define FLAG_PIB_PAUSE		BIT(1)
#define FLAG_PIB_OVFLW_INTR	BIT(2)
#define FLAG_PIB_ILLEGAL_INTR	BIT(3)
#define FLAG_PIB_WAKEUP_INTR	BIT(4)
#define FLAG_PIB_BYPASS		BIT(5)
#define FLAG_PIB_DELAY		BIT(6)
#define FLAG_PIB_PKT_LEN_ADJ	BIT(7)
#define DMA_PORT_FOR_FLUSH 25
#define PRX300_CQM_DROP_Q	0x0
#define PRX300_CQM_Q_MASK	0xff

/***********************
 * ENUM
 ***********************/
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
	DEQ_DMA_CHNL = 8,
	ENQ_DMA_FSQM_CHNL = 1,
	ENQ_DMA_SIZE0_CHNL = 2,
	ENQ_DMA_SIZE1_CHNL = 3,
	ENQ_DMA_SIZE2_CHNL = 4,
	ENQ_DMA_SIZE3_CHNL = 5,
	DEQ_DMA_CHNL_NOT_APPL = 0XFF,
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
	/*GSWIPR_VRX318,*/
	GSWIPR_REDIRECT,
	GSWIPR_ETHWAN,
	VRX318_PORT,
	MAX_DQM_PORT,
};

/*! PMAC port flag */
enum FREE_FLAG {
	/*! The port is free */
	P_FREE = 0,
	P_ALLOCATED,
	P_REGISTERED,
	P_FLAG_NO_VALID
};

enum DQM_PORT_TYPE {
	DQM_CPU_START_ID = 0,
	DQM_CPU_END_ID = 3,
	DQM_ACA_START_ID = 4,
	DQM_ACA_END_ID = 6,
	DQM_DMA_START_ID = 7,
	DQM_VUNI_START_ID = 24,
	DQM_VUNI_END_ID = 24,
	DQM_DMA_END_ID = 22,
	DQM_PON_START_ID = 26,
	DQM_PON_END_ID = 89,
};

enum EQM_PORT_TYPE {
	EQM_CPU_START_ID = 0,
	EQM_CPU_END_ID = 3,
	EQM_DMA_START_ID = 5,
	EQM_DMA_END_ID = 8,
};

enum EQM_DQM_PORT_TYPE {
	DQM_CPU_TYPE = 0,
	DQM_ACA_TYPE = 4,
	DQM_DMA_TYPE = 7,
	DQM_PON_TYPE = 26,
	DQM_VUNI_TYPE = 27,
	EQM_CPU_TYPE = 100,
	EQM_DMA_TYPE = 200,
	NONE_TYPE = 0xffff
};

enum DP_DEV_TYPE {
	PMAC_ETH_LAN_START_ID = 3,
	PMAC_ETH_LAN_END_ID = 4,
	PMAC_ALLOC_START_ID = 5,
	PMAC_ALLOC_END_ID = 11,
	PMAC_ETH_WAN_ID = 2,
};

enum CQM_QIDT_STATE {
	STATE_CLASS = 0,
	STATE_EP = 1,
	STATE_MPE1 = 2,
	STATE_MPE2 = 3,
	STATE_ENC = 4,
	STATE_DEC = 5,
	STATE_FLOWID_LOW = 6,
	STATE_FLOWID_HIGH = 7,
	STATE_SUBIF_ID = 8,
	STATE_SUBIF_LSB_DC = 9,
	STATE_NONE = 0xffff
};

enum CQM_BUFF_SIZE {
	CQM_SIZE0_BUF_SIZE = 0,
	CQM_SIZE1_BUF_SIZE,
	CQM_SIZE2_BUF_SIZE,
	CQM_SIZE3_BUF_SIZE,
};

enum CQM_QOS_LINK_MODE {
	FULL_QOS_10G = 0,
	SHORT_QOS_10G,
	FULL_QOS_1G,
	SHORT_QOS_1G,
};

/**************************************************
 *
 * Macros
 *
 ***************************************************/
/**************************
 *Address Calculation
 **************************/
#define FSQM_LLT_RAM(fsqm_base, idx)\
	(((fsqm_base) + RAM + ((idx) << 2)))

#define FSQM_RCNT(fsqm_base, idx)\
	(((fsqm_base) + RCNT + ((idx) << 2)))

#define EQ_CPU_PORT(idx, reg)\
(CFG_CPU_IGP_0 + ((idx) * 0x1000) + offsetof(struct cqm_eqm_cpu_igp_reg, reg))

#define EQ_DMA_PORT(idx, reg)\
(CFG_CPU_IGP_0 + ((idx) * 0x1000) + offsetof(struct cqm_eqm_dma_igp_reg, reg))

#define DQ_CPU_PORT(idx, reg)\
(CFG_CPU_EGP_0 + ((idx) * 0x1000) + offsetof(struct cqm_dqm_cpu_egp_reg, reg))

#define DQ_SCPU_PORT(idx, reg)\
(CFG_CPU_EGP_0 + ((idx) * 0x1000) + offsetof(struct cqm_dqm_aca_egp_reg, reg))

#define DQ_DMA_PORT(idx, reg)\
(CFG_CPU_EGP_0 + ((idx) * 0x1000) + offsetof(struct cqm_dqm_dma_egp_reg, reg))

#define DQ_PON_PORT(idx, reg)\
(CFG_PON_EGP_26 + (((idx) - 26) * 0x400) \
+ offsetof(struct cqm_dqm_dma_egp_reg, reg))

#define CQM_INT_LINE(idx, reg)\
((idx) * 0x40 + offsetof(struct cqm_int_reg, reg))

#define CQM_ENQ_DMA_DESC(pid, des_idx)\
(DESC0_0_IGP_4 + (((pid) - 4) * 0x1000) + (des_idx) * 16)

#define CQM_DEQ_DMA_DESC(pid, des_idx)\
(DESC0_0_EGP_7  + (((pid) - 7) * 0x1000) + (des_idx) * 16)

#define CQM_DEQ_PON_DMA_DESC(pid, des_idx)\
(DESC0_EGP_PON_0)

/***********************
 * Default  Value Definition
 **********************/
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

#define CBM_NOTFOUND             2
#define CBM_RESCHEDULE          4
#define PMAC_MAX_NUM 16
#define CQM_PORT_MAX 128
#define CQM_ENQ_PORT_MAX 16
#define CQM_DEQ_PORT_MAX 89
#define CQM_DEQ_BUF_SIZE 64
#define CQM_POOL_METADATA 4
#define CQM_FIXED_RX_OFFSET\
	(CBM_DMA_DATA_OFFSET + NET_IP_ALIGN + NET_SKB_PAD + CQM_POOL_METADATA)
#define BSL_THRES\
	(CQM_FIXED_RX_OFFSET + SKB_DATA_ALIGN(sizeof(struct skb_shared_info)))
#define QID_MODE_SEL_REG 4
#define QID_MODE_ENTRY_PER_REG 16
#define DMA_CH_STR_LEN 16
#define CQEM_DEBUG_COUNTER_ENABLE
#ifdef CQEM_DEBUG_COUNTER_ENABLE
#define UP_STATS(atomic) atomic_add(1, &(atomic))
#else
#define UP_STATS(atomic)
#endif
#define STATS_GET(atomic) atomic_read(&(atomic))
#define CQM_MAX_POOL_NUM (CQM_FSQM_POOL + 1)
#define CQM_MAX_POLICY_NUM (CQM_FSQM_POLICY + 1)

#define TOT_DMA_HNDL (CQM_PRX300_NUM_BM_POOLS + 1)
/*************************
 * Structure definition
 *************************/
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
struct cqm_scpu_ptr_rtn {
	u32 ptr_rtn0;
	u32 ptr_rtn1;
};

#ifdef CONFIG_CPU_LITTLE_ENDIAN
struct dma_desc {
	/* DWORD 0 */
	u32 dest_sub_if_id:15;
	u32 eth_type:2;
	u32 flow_id:8;
	u32 tunnel_id:4;
	u32 resv0:2;
	u32 redir:1;

	/* DWORD 1 */
	u32 classid:4;
	u32 ip:4;
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
	u32 pool:3;
	u32 resv01:1;
	u32 policy:3;
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
	u32 ip:4;
	u32 classid:4;

	/* DWORD 0 */
	u32 redir:1;
	u32 resv0:2;
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
	u32 policy:3;
	u32 resv1:1;
	u32 pool:3;
	u32 data_len:16;

	/*DWORD 2 */
	u32 data_ptr;
};
#endif

struct cbm_desc_list {
	struct cbm_desc desc;
	struct cbm_desc_list *next;
};

struct cqm_qidt_mask {
	u32 classid_mask;
	u32 ep_mask;
	u32 mpe1_mask;
	u32 mpe2_mask;
	u32 enc_mask;
	u32 dec_mask;
	u32 flowid_lmask;
	u32 flowid_hmask;
	u32 sub_if_id_mask;
};

struct cqm_qidt_elm {
	u32 clsid;
	u32 ep;
	u32 mpe1;
	u32 mpe2;
	u32 enc;
	u32 dec;
	u32 flowidl;
	u32 flowidh;
	u32 sub_if_id;
};

struct cqm_eqm_cpu_igp_reg {
	u32 cfg;
	u32 resv0[2];
	u32 eqpc;
	struct cbm_desc disc;
	u32 irncr;  /*Interrupt Capture :  Status + Clear */
	u32 irnicr;
	u32 irnen;  /*Interrupt Enable Mask */
	u32 resv1[3];
	u32 dicc;
	u32 drop;
	u32 npbpc;
	u32 ns0pc;
	u32 ns1pc;
	u32 ns2pc;
	u32 ns3pc;
	u32 resv2[11];
	u32 new_pbptr;
	u32 new_s0ptr;
	u32 new_s1ptr;
	u32 new_s2ptr;
	u32 new_s3ptr;
	u32 resv3[2];
	u32 dcntr;
	u32 resv4[24];
	struct cbm_desc desc0;
	struct cbm_desc desc1;
};

struct cqm_eqm_dma_igp_reg {
	u32 cfg;
	u32 resv0[2];
	u32 eqpc;
	struct cbm_desc disc;
	u32 irncr;
	u32 irnicr;
	u32 irnen;
	u32 resv1[1];
	u32 dptr;
	u32 resv2;
	u32 dicc;
	u32 drop;
	u32 resv3[23];
	u32 dcntr;

};

struct cqm_dqm_cpu_egp_reg {
	u32 cfg;
	u32 dqpc;
	u32 resv0[6];
	u32 irncr;
	u32 irnicr;
	u32 irnen;
	u32 resv1;
	u32 dptr;
	u32 bprc;
	u32 resv2[50];
	u32 ptr_rtn_dw2;
	u32 ptr_rtn_dw3;
	u32 resv3[62];
	struct cbm_desc desc0;
	struct cbm_desc desc1;
};

struct cqm_dqm_aca_egp_reg {
	u32 cfg;
	u32 dqpc;
	u32 resv0[6];
	u32 irncr;
	u32 irnicr;
	u32 irnen;
	u32 resv1;
	u32 dptr;
	u32 bprc;
	u32 resv2[50];
	struct cqm_scpu_ptr_rtn scpu_ptr_rtn[32];
	struct cbm_desc desc[32];
};

struct cqm_dqm_dma_egp_reg {
	u32 cfg;
	u32 dqpc;
	u32 resv0[6];
	u32 irncr;
	u32 irnicr;
	u32 irnen;
	u32 resv1;
	u32 dptr;
};

struct cqm_int_reg {
	u32 cbm_irncr;
	u32 cbm_irnicr;
	u32 cbm_irnen;
	u32 res0;
	u32 igp_irncr;
	u32 igp_irnicr;
	u32 igp_irnen;
	u32 res1;
	u32 egp_irncr;
	u32 egp_irnicr;
	u32 egp_irnen;
};

struct cqm_egp_map {
	u32 epg;
	u32 pmac;
	u32 port_type;
};

/*! CBM port information*/
struct cqm_dqm_port_info {
	u32 dma_ch;
	u32 dma_dt_ch;
	u32 dma_dt_ctrl;
	u32 dma_dt_init_type;
	bool dma_ch_in_use;
	u32 def_qid;
	u32 def_schd;
	u32 def_tmu_pid;
	void		*cbm_buf_free_base;
	u32	num_free_entries;/*!< Number of Free Port entries */
	cbm_dq_info_t	deq_info;
	u32 dq_txpush_num;
	u32 egp_type;
	char dma_chan_str[DMA_CH_STR_LEN];
	u32 cpu_port_type;
	u32 valid;
};

struct cqm_eqm_port_info {
	u32 port_type;
	int num_eq_ports;/*!< Number of Enqueue Ports */
	cbm_eq_info_t eq_info;/*!<  EQ port info */
	u32 dma_dt_ch;
	u32 dma_dt_ctrl;
	u32 dma_dt_init_type;
	char dma_chan_str[DMA_CH_STR_LEN];
	u32 valid;
};

struct cqm_pmac_port_map {
	int flags;/*! port flag */
	struct module *owner;
	struct net_device *dev;
	u32 dev_port;
	u32 pmac;
	u32 egp_port_map[PRX300_MAX_PORT_MAP];/*bit map to egp port*/
	u32 qid_num;/*queue numbers allocated to that pmac port*/
	u32 qids[16];/*qid array*/
	/* e.g. DP_F_FAST_ETH_LAN/DP_F_FAST_ETH_WAN/DP_F_DIRECT/
	 *High priority/Low priority
	 */
	u32 egp_type;
	struct list_head list;
};

struct cqm_res_t {
	u32 cqm_deq_port;
};

struct cbm_cntr_mode {
	void __iomem *reg;
	u32 msel_mask;
	u32 msel_pos;
	u32 qen_mask;
	u32 qen_pos;
	void (*reset_counter)(void);
};

struct pib_ctrl {
	u32 pib_en:1;
	u32 pib_pause:1;
	u32 cmd_ovflw_intr_en:1;
	u32 cmd_illegal_port_intr_en:1;
	u32 wakeup_intr_en:1;
	u32 pib_bypass:1;
	u32 pkt_len_adj:1;
	u32 deq_delay:4;
};

struct cqm_ctrl {
	int id;
	struct device *dev;
	struct net_device dummy_dev;
	struct napi_struct napi;
	int num_dqm_ports;
	int num_eqm_ports;
	int max_queues;
	int num_pools;
	int num_dt_entries;
	int num_intrs;
	const struct cqm_config *cqm_cfg;
	struct clk *cbm_clk;
	u32 cbm_irq[PRX300_MAX_INTR_LINE];
	u32 cbm_line[PRX300_MAX_INTR_LINE];
	u32 cpu_port_alloc;
	struct tasklet_struct cqm_tasklet[NR_CPUS];
#ifdef CPU_POOL_ALLOWED
	struct tasklet_struct cqm_cpu_free_tasklet;
	u32 *cpu_rtn_ptr;
	u32 *cpu_rtn_aligned;
#endif
#ifdef ENABLE_LL_DEBUG
	struct tasklet_struct cqm_debug_tasklet;
#endif
	const struct cbm_ops *cqm_ops;
	void *cqm_qmgr_buf_base;
	void *bm_buf_base[CQM_PRX300_NUM_BM_POOLS];
	u32 ifmux_reg_offset;
	struct regmap *syscfg;
	bool force_xpcs;
	void __iomem *txpush;
	void __iomem *dmadesc;
	void __iomem *dmadesc_phys;
	void __iomem *cqm;
	void __iomem *qidt;
	void __iomem *ls;
	void __iomem *enq;
	void __iomem *deq;
	void __iomem *deq_phy;
	void __iomem *qid2ep;
	void __iomem *fsqm;
	void __iomem *pib;
	void __iomem *pon_dqm_cntr;
	u32 max_mem_alloc;
	dma_addr_t dma_hndl_qos;
	dma_addr_t dma_hndl_p[TOT_DMA_HNDL];
	struct dentry *debugfs;
	const char *name;
	u32 prx300_pool_ptrs[CQM_PRX300_NUM_BM_POOLS];
	u32 prx300_pool_size[CQM_PRX300_NUM_BM_POOLS];
	int gint_mode;
};

struct cqm_buf_dbg_cnt {
	atomic_t rx_cnt;
	atomic_t tx_cnt;
	atomic_t free_cnt;
	atomic_t alloc_cnt;
	atomic_t isr_free_cnt;
};

struct fsq {
	u16 head;
	u16 tail;
};

extern
struct cqm_buf_dbg_cnt cqm_dbg_cntrs[CQM_MAX_POLICY_NUM][CQM_MAX_POOL_NUM];

/************************
 * Function prototypes
 ************************/
void cqm_read_dma_desc_prep(int port, void **base_addr, int *num_desc);
void cqm_read_dma_desc_end(int port);
int get_fsqm_ofsc(void);
bool fsqm_check(uint16_t len);
void *cqm_get_enq_base(void);
void *cqm_get_deq_base(void);
void *cqm_get_dma_desc_base(void);
void *cqm_get_ctrl_base(void);
void *cqm_get_ls_base(void);
int cqm_debugfs_init(struct cqm_ctrl *pctrl);

#endif
