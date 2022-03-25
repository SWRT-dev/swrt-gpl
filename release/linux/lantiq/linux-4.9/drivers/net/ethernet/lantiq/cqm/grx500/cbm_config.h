#ifndef __CBM_CONFIG_H__
#define __CBM_CONFIG_H__

struct dqm_dma_port {
	u32 tmu_port;
	u32 tmu_queue;
	u32 tmu_queue_nos;
	u32 dma_ctrl;
	u32 dma_chan;
	u32 port_enable;
	u32 num_desc;
};

struct dqm_cpu_port {
	u32 tmu_port;
	u32 tmu_queue;
	u32 cpu_port_type;
	u32 num_desc;
};

struct dqm_spl_port {
	u32 tmu_port;
	u32 tmu_queue;
	u32 port_enable;
	u32 num_desc;
};

struct eqm_dma_port {
	u32 tmu_port;
	u32 dma_ctrl;
	u32 dma_chnl;
	u32 dma_chnl_type;
	u32 port_type;
	u32 num_desc;
};

struct eqm_cpu_port {
	u32 tmu_port;
	u32 port_type;
	u32 num_desc;
};

struct eqm_dl_port {
	u32 tmu_port;
	u32 dma_ctrl;
	u32 dma_chnl;
	u32 port_type;
	u32 num_desc;
};

struct cqm_config {
	u32 type;
	union {
		struct dqm_dma_port dqm_dma;
		struct dqm_cpu_port dqm_cpu;
		struct dqm_spl_port dqm_ldma;
		struct dqm_spl_port dqm_scpu;
		struct eqm_dma_port eqm_dma;
		struct eqm_cpu_port eqm_cpu;
		struct eqm_cpu_port eqm_toe;
		struct eqm_cpu_port eqm_vrx318;
		struct eqm_dl_port eqm_dl;
	} data;
};

extern const struct cqm_config xrx500_cbm_config[];

#endif /* __CBM_CONFIG_H__ */
