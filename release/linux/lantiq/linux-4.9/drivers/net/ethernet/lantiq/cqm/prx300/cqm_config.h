#ifndef __CQM_CONFIG_H__
#define __CQM_CONFIG_H__

#include <linux/pp_buffer_mgr_api.h>

#define CQM_PRX300_NUM_BM_POOLS	4
#define CQM_PRX300_NUM_POOLS	(CQM_PRX300_NUM_BM_POOLS + 1)
#define CQM_PRX300_POOL_POL_HDR_SIZE	4
#define CQM_QIDT_DW_NUM	0x1000
#define CQM_QID2EP_DW_NUM	0x100
#define CQM_SRAM_SIZE	0xE0000
#define CQM_SRAM_FRM_SIZE 128
#define CQM_CPU_POOL_BUF_ALW_NUM	128
#define CQM_SRAM_BASE	0x18D00000
#define CQM_CPU_POOL 0
#define CQM_CPU_POLICY 7
#define CQM_FSQM_POOL 7
#define CQM_FSQM_POLICY 7

#define MAX_QOS_QUEUES 256
#define PRX300_MAX_PORT_PER_EP 1
#define PRX300_MAX_PORT_MAP 2
#define PRX300_MAX_PON_PORTS 64
#define PRX300_SKB_PTR_SIZE 4
#define PRX300_WAN_AON_MODE BIT(0)
#define PRX300_WAN_PON_MODE 0

#define PRX300_MAX_RESOURCE 16
#define PRX300_MAX_INTR_LINE 8
#define PRX300_SEL7TO4_POS 10
#define PRX300_MPE2_POS 9
#define PRX300_MPE1_POS 8
#define PRX300_EP_POS 4
#define PRX300_SEL3TO0_POS 0

#if CQM_SRAM_FRM_SIZE == 128
#define CQM_SEGMENT_MASK_SIZE 0xfffff800
#define CQM_OFFSET_MASK_SIZE 0x7ff
#define CQM_SRAM_FRM_SIZE_BIT 0
#else
#define CQM_SEGMENT_MASK_SIZE 0xffffff80
#define CQM_OFFSET_MASK_SIZE 0x7f
#define CQM_SRAM_FRM_SIZE_BIT 1
#endif

enum {
	REQ_FSQM_BUF = 0,
	REQ_SIZE0_BUF = 1,
	REQ_SIZE1_BUF = 2,
	REQ_SIZE2_BUF = 3,
	REQ_SIZE3_BUF = 4,
};

struct dqm_dma_port {
	u32 port_range;
	u32 port;
	u32 queue;
	u32 queue_nos;
	u32 dma_ctrl;
	u32 dma_chan;
	u32 port_enable;
	u32 num_desc;
	u32 txpush_desc;
};

struct dqm_pon_port {
	u32 port_range;
	u32 port;
	u32 queue;
	u32 queue_nos;
	u32 dma_ctrl;
	u32 dma_chan;
	u32 port_enable;
	u32 num_desc;
	u32 txpush_desc;
};

struct dqm_aca_port {
	u32 port;
	u32 aca_port_type;
	u32 dma_ctrl;
	u32 dma_chan;
	u32 num_desc;
	u32 txpush_desc;
	u32 num_free_burst;
};

struct dqm_cpu_port {
	u32 port;
	u32 queue;
	u32 cpu_port_type;
	u32 num_desc;
	u32 txpush_desc;
};

struct eqm_dma_port {
	u32 port;
	u32 port_range;
	u32 dma_ctrl;
	u32 dma_chnl;
	u32 dma_chnl_type;
	u32 port_type;
	u32 num_desc;
};

struct eqm_cpu_port {
	u32 port;
	u32 port_type;
	u32 num_desc;
};

struct cqm_config {
	u32 type;
	union {
		struct dqm_dma_port dqm_dma;
		struct dqm_cpu_port dqm_cpu;
		struct dqm_aca_port dqm_aca;
		struct eqm_dma_port eqm_dma;
		struct eqm_cpu_port eqm_cpu;
	} data;
};

struct cqm_bm_pool_config {
	unsigned int buf_frm_size;
	unsigned int buf_frm_size_reg;
	unsigned int buf_frm_num;
	unsigned int pool_start_low;
	unsigned int pool_start_high;
	unsigned int pool_end_low;
	unsigned int pool_end_high;
	dma_addr_t pool_dma;
	unsigned int segment_mask;
	unsigned int offset_mask;
	unsigned int pool;
	unsigned int policy;
	unsigned int buf_type;
};

extern const struct cqm_config prx300_cqm_config[];
extern struct cqm_ctrl g_prx300_ctrl;
extern struct cqm_bm_pool_config bm_pool_conf[CQM_PRX300_NUM_POOLS];

#endif /* __CQM_CONFIG_H__ */
