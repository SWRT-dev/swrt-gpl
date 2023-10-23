#include "cqm.h"
#include <net/lantiq_cbm_api.h>
#include "cqm_config.h"

struct cqm_bm_pool_config bm_pool_conf[CQM_PRX300_NUM_POOLS] = {
	{
	.buf_type = REQ_SIZE0_BUF,
	.segment_mask = 0xFFFFF800,
	.offset_mask = 0x7ff,
	.pool = 0,
	.policy = 0,
	.buf_frm_size_reg = 7
	},
	{
	.buf_type = REQ_SIZE1_BUF,
	.segment_mask = 0xFFFFF800,
	.offset_mask = 0x7ff,
	.pool = 1,
	.policy = 1,
	.buf_frm_size_reg = 7
	},
	{
	.buf_type = REQ_SIZE2_BUF,
	.segment_mask = 0xFFFFF800,
	.offset_mask = 0x7ff,
	.pool = 2,
	.policy = 2,
	.buf_frm_size_reg = 7
	},
	{
	.buf_type = REQ_SIZE3_BUF,
	.segment_mask = 0xFFFFF800,
	.offset_mask = 0x7ff,
	.pool = 3,
	.policy = 3,
	.buf_frm_size_reg = 7
	},
	{
	.buf_frm_size = CQM_SRAM_FRM_SIZE,
	.buf_type = REQ_FSQM_BUF,
	.segment_mask = CQM_SEGMENT_MASK_SIZE,
	.offset_mask = CQM_OFFSET_MASK_SIZE,
	.pool = 7,
	.policy = 7,
	.pool_start_low = (unsigned int)(CQM_SRAM_BASE),
	.pool_end_low = (unsigned int)(CQM_SRAM_BASE + CQM_SRAM_SIZE),
	},
};

const struct cqm_config prx300_cqm_config[] = {
	{
	.type = DQM_CPU_TYPE,
	.data.dqm_cpu.port = 0,
	.data.dqm_cpu.cpu_port_type = 0,
	.data.dqm_cpu.num_desc = 2,
	.data.dqm_cpu.txpush_desc = 2,
	},
	{
	.type = DQM_CPU_TYPE,
	.data.dqm_cpu.port = 1,
	.data.dqm_cpu.cpu_port_type = 0,
	.data.dqm_cpu.num_desc = 2,
	.data.dqm_cpu.txpush_desc = 2,
	},
	{
	.type = DQM_CPU_TYPE,
	.data.dqm_cpu.port = 2,
	.data.dqm_cpu.cpu_port_type = 0,
	.data.dqm_cpu.num_desc = 2,
	.data.dqm_cpu.txpush_desc = 2,
	},
	{
	.type = DQM_CPU_TYPE,
	.data.dqm_cpu.port = 3,
	.data.dqm_cpu.cpu_port_type = 0,
	.data.dqm_cpu.num_desc = 2,
	.data.dqm_cpu.txpush_desc = 2,
	},
	{
	.type = DQM_ACA_TYPE,
	.data.dqm_aca.port = 4,
	.data.dqm_aca.aca_port_type = 1,
	.data.dqm_aca.dma_ctrl = 1,
	.data.dqm_aca.dma_chan = 13,
	.data.dqm_aca.num_desc = 32,
	.data.dqm_aca.txpush_desc = 32,
	.data.dqm_aca.num_free_burst = 32,
	},
	{
	.type = DQM_ACA_TYPE,
	.data.dqm_aca.port = 5,
	.data.dqm_aca.aca_port_type = 1,
	.data.dqm_aca.dma_ctrl = 1,
	.data.dqm_aca.dma_chan = 14,
	.data.dqm_aca.num_desc = 32,
	.data.dqm_aca.txpush_desc = 32,
	.data.dqm_aca.num_free_burst = 32,
	},
	{
	.type = DQM_ACA_TYPE,
	.data.dqm_aca.port = 6,
	.data.dqm_aca.aca_port_type = 1,
	.data.dqm_aca.dma_ctrl = 1,
	.data.dqm_aca.dma_chan = 15,
	.data.dqm_aca.num_desc = 32,
	.data.dqm_aca.txpush_desc = 32,
	.data.dqm_aca.num_free_burst = 32,
	},
	{
	.type = DQM_DMA_TYPE,
	.data.dqm_dma.port = 7,
	.data.dqm_dma.port_range = 7,
	.data.dqm_dma.dma_ctrl = 1,
	.data.dqm_dma.dma_chan = 0,
	.data.dqm_dma.port_enable = 1,
	.data.dqm_dma.num_desc = 8,
	.data.dqm_dma.txpush_desc = 8,
	},
	{
	.type = DQM_DMA_TYPE,
	.data.dqm_dma.port = 15,
	.data.dqm_dma.port_range = 7,
	.data.dqm_dma.dma_ctrl = 1,
	.data.dqm_dma.dma_chan = 8,
	.data.dqm_dma.port_enable = 1,
	.data.dqm_dma.num_desc = 8,
	.data.dqm_dma.txpush_desc = 8,
	},
	{
	.type = DQM_VUNI_TYPE,
	.data.dqm_dma.port = 24,
	.data.dqm_dma.port_range = 0,
	.data.dqm_dma.dma_ctrl = 2,
	.data.dqm_dma.dma_chan = 1,
	.data.dqm_dma.port_enable = 1,
	.data.dqm_dma.num_desc = 8,
	.data.dqm_dma.txpush_desc = 8,
	},
	{
	.type = DQM_DMA_TYPE,
	.data.dqm_dma.port = DMA_PORT_FOR_FLUSH,
	.data.dqm_dma.port_range = 0,
	.data.dqm_dma.dma_ctrl = 3,
	.data.dqm_dma.dma_chan = 0,
	.data.dqm_dma.port_enable = 1,
	.data.dqm_dma.num_desc = 8,
	.data.dqm_dma.txpush_desc = 8,
	},
	{
	.type = DQM_DMA_TYPE,
	.data.dqm_dma.port = DMA_PORT_RE_INSERTION,
	.data.dqm_dma.port_range = 0,
	.data.dqm_dma.dma_ctrl = 1,
	.data.dqm_dma.dma_chan = 16,
	.data.dqm_dma.port_enable = 1,
	.data.dqm_dma.num_desc = 8,
	.data.dqm_dma.txpush_desc = 8,
	},
	{
	.type = DQM_PON_TYPE,
	.data.dqm_dma.port = 26,
	.data.dqm_dma.port_range = 0,
	.data.dqm_dma.dma_ctrl = 2,
	.data.dqm_dma.dma_chan = 0,
	.data.dqm_dma.port_enable = 1,
	.data.dqm_dma.num_desc = 16,
	.data.dqm_dma.txpush_desc = 8,
	},
	{
	.type = EQM_CPU_TYPE,
	.data.eqm_cpu.port = 0,
	.data.eqm_cpu.port_type = 2,
	.data.eqm_cpu.num_desc = 2
	},
	{
	.type = EQM_CPU_TYPE,
	.data.eqm_cpu.port = 1,
	.data.eqm_cpu.port_type = 2,
	.data.eqm_cpu.num_desc = 2
	},
	{
	.type = EQM_CPU_TYPE,
	.data.eqm_cpu.port = 2,
	.data.eqm_cpu.port_type = 2,
	.data.eqm_cpu.num_desc = 2
	},
	{
	.type = EQM_CPU_TYPE,
	.data.eqm_cpu.port = 3,
	.data.eqm_cpu.port_type = 2,
	.data.eqm_cpu.num_desc = 2
	},
	{
	.type = EQM_DMA_TYPE,
	.data.eqm_dma.port = 4,
	.data.eqm_dma.port_range = 4,
	.data.eqm_dma.dma_ctrl = 1,
	.data.eqm_dma.dma_chnl = 0,
	.data.eqm_dma.num_desc = 8,
	},
	{
	.type = EQM_DMA_TYPE,
	.data.eqm_dma.port = 9,
	.data.eqm_dma.port_range = 4,
	.data.eqm_dma.dma_ctrl = 2,
	.data.eqm_dma.dma_chnl = 0,
	.data.eqm_dma.num_desc = 8,
	},
	{
	.type = NONE_TYPE
	}
};

struct cqm_ctrl g_prx300_ctrl = {
	.num_dqm_ports = CQM_DEQ_PORT_MAX,
	.num_eqm_ports = CQM_ENQ_PORT_MAX,
	.max_queues = 256,
	.num_pools = CQM_PRX300_NUM_POOLS,
	.num_intrs = 5,
	.cqm_cfg = prx300_cqm_config,
	.ifmux_reg_offset = 0x120,
	.bm_buf_base[0] = NULL,
	.bm_buf_base[1] = NULL,
	.bm_buf_base[2] = NULL,
	.bm_buf_base[3] = NULL,

};
