/* SPDX-License-Identifier: GPL-2.0
 * Copyright (c) 2016 ~ 2018 Intel Corporation.
 * Copyright (C) 2020 MaxLinear
 */
#ifndef __CBM_API_H
#define __CBM_API_H
#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/page_ref.h>
#include <linux/dma-mapping.h>
#include <net/datapath_dp_cqm.h>

/**
 * file cbm_api.h
 * brief This file contains all the Central Buffer Manager (CBM) API
 */

/**
 * defgroup CBM_Driver CBM Driver
 * brief All API and defines exported by CBM Driver.
 * Look at the constants chapter below.
 */
/**
 * defgroup CBM_Driver_Defines CBM Driver Defines
 * brief Defines used in the CBM Driver. Look at the constants chapter below.
 */
/**
 * defgroup CBM_Driver_Global_Defines CBM Driver Global Defines
 * brief Global Defines used in the CBM Driver.
 * Look at the constants chapter below.
 */
/**
 * defgroup CBM_Driver_Port_Allocation_Defines CBM Driver Port Allocation.
 * Defines brief CBM Port Allocate Defines.
 * Look at the constants chapter below.
 */
/**
 * defgroup CBM_Driver_Port_Defines CBM Driver Port Defines
 * brief CBM Port Initialization Defines. Look at the constants chapter below.
 */
/**
 * defgroup CBM_Driver_CPU_Defines CBM Driver CPU Defines
 * brief CBM Driver CPU Related Defines. Look at the constants chapter below.
 */
/**
 * defgroup CBM_Driver_QMAP_Defines CBM Driver QMAP Defines
 * brief CBM Queue Map Table Field defines.
 * Look at the constants chapter below.
 */
/**
 * defgroup CBM_Driver_Port_Structures CBM Driver Port Structures
 * brief CBM Port Configuration Structures
 */

/**
 * defgroup CBM_Control_API CBM Control API
 * brief CBM Control API allows to configure Interrupt,
 * Enqueue and Dequeue manager
 * - CBM Enqueue Manager
 * - CBM Dequeue Manager
 */

/**
 * defgroup CBM_Port_Allocation_API CBM Port Allocation API
 * brief CBM Port Allocation API allows to allocate and de-allocate
 *  - CBM Enqeuue and Dequeue Ports
 *  - Datapath (PMAC) Ports
 */
/**
 * defgroup CBM_Port_OPS_API CBM Port Operations API
 * brief CBM Port Operations API - Initialize/Uninitialize,
 * Enable/Disable, Configure CBM port
 *  - CBM Enqeuue and Dequeue Ports
 */
/**
 * defgroup CBM_Stats_API CBM Statistics API
 * brief CBM Statistics API
 * - CBM Enqeuue and Dequeue Ports
 */
/**
 * defgroup CBM_CPU_Port_API CBM CPU Port API
 * brief CBM CPU Port API
 * - CBM Enqeuue and Dequeue Ports
 */
/**
 * defgroup CBM_CPU_IRQ_API  CBM CPU IRQ API
 * brief CBM CPU IRQ Control and Enable API
 * - CBM Enqeuue and Dequeue Ports
 */
/**
 * defgroup CBM_CPU_API CBM CPU API
 * brief CBM CPU API
 */
/**
 * defgroup CBM_CPU_LOAD_SPREADER_API CBM CPU LOAD SPREADER API
 * brief CBM CPU Rx side Load Spreader API.
 * - Load Spreader module distributes incoming packet load to multiple
 * Cores/VPEs in GRX500 for maximising SMP Rx Packet performance
 */

#define SINGLE_RX_CH0_ONLY 1

/** addtogroup CBM_Driver_Global_Defines */
/* brief OK return value */
#define CBM_OK			0
/* brief Error return value */
#define CBM_ERROR		-1

/** addtogroup CBM_Driver_Port_Allocation_Defines */
/* brief Module Flag VRX318 SmartPHY */
#define CBM_MOD_F_VRX318		0x1
/* brief Module Flag TSO Engine */
#define CBM_MOD_F_TSO			0x2
/* brief Module Flag LRO Engine */
#define CBM_MOD_F_LRO			0x4
/* brief Module Flag WAVE500 WLAN */
#define CBM_MOD_F_WAVE500		0x8

/* brief Flag Legacy 2-DWORD DMA descriptor */
#define CBM_MOD_F_LEGACY		0x10
/* brief Flag Legacy Desc to 4 DW Invert */
#define CBM_MOD_F_DESC_ORDER_INVERT	0x20
/* brief Flag CBM Port Type is CPU port */
#define CBM_MOD_F_PORT_TYPE_CPU		0x40

/** addtogroup CBM_Driver_Port_Defines */
/* brief Use Standard Size buffer pool */
#define CBM_PORT_F_STANDARD_BUF		0x1
/* brief Use Jumbo size Buffer pool */
#define CBM_PORT_F_JUMBO_BUF		0x2

/* brief CBM Dequeue port */
#define CBM_PORT_F_DEQUEUE_PORT		0x4

/* brief Disable the CBM port */
#define CBM_PORT_F_DISABLE		0x8

/* brief CPU CBM port */
#define CBM_PORT_F_CPU_PORT			0x10

/* brief WAVE500 WLAN special CPU port */
#define CBM_PORT_F_WAVE500_PORT		0x20

#ifdef CONFIG_LGM_CQM
#define PORTS_PER_CPU	2
#define DPDK_MAX_CPU 3
#define CQM_MAX_POLICY_CPU 4   /* Max number of policy per CPU */
#define CQM_ALLOC_BUF_NO_HD_ROOM  BIT(0)
#define MAX_LOOKUP_TBL		2
#define MAX_LOOKUP_TBL_SIZE (8 * 1024 * MAX_LOOKUP_TBL)
#define CQM_BUF_FREE_PA     BIT(31)
#define CQM_CPU_POOL_SIZE     4

#define DW1_PORT              GENMASK(11, 4)
#define DW1_LRO_TYPE          GENMASK(15, 14)
#define DW1_POOL              GENMASK(21, 18)
#define DW1_PRE_L2            GENMASK(27, 26)

#define DW3_LEN               GENMASK(13, 0)
#define DW3_POLICY            GENMASK(21, 14)
#define DW3_ADDRH             GENMASK(26, 23)
#define DW3_DIC               BIT(27)
#define DW3_EOP               BIT(28)
#define DW3_SOP               BIT(29)
#define DW3_OWN               BIT(31)

#define PTRRET_DW3_POOL       GENMASK(3, 0)
#define PTRRET_DW3_POLICY     GENMASK(21, 14)
#define PTRRET_DW3_HIADDR     GENMASK(26, 23)
#else
#define PORTS_PER_CPU	1
#define MAX_LOOKUP_TBL_SIZE (16 * 1024)
#define MAX_LOOKUP_TBL		1
#endif

/** brief Flush the DEQ port */
#define CBM_PORT_F_FLUSH		0x40
#define MAX_PORTS_PER_CPU	2
/** addtogroup CBM_Driver_CPU_Defines */
/* brief Stop Rx i.e. assert backpressure */
#define CBM_CPU_F_RX_STOP		0x1
/* brief Start Rx i.e. remove backpressure */
#define CBM_CPU_F_RX_START		0x2

/* brief RCNT Decrement flag */
#define CBM_CPU_BUF_DEC_RCNT		0x4
/* brief RCNT Increment flag */
#define CBM_CPU_BUF_INC_RCNT		0x8
/** addtogroup CBM_Driver_QMAP_Defines */
/* brief FlowId bits are don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE		0x1
#define CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE		0x80

#define CBM_QUEUE_MAP_F_EGFLAG_DONTCARE			0x800

#define CBM_QUEUE_MAP_F_SUBIF_DONTCARE			0x100

/* brief VPN Encrypt bit is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_EN_DONTCARE		0x2
/* brief VPN Decrypt bit is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_DE_DONTCARE		0x4
/* brief MPE1 bit is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_MPE1_DONTCARE		0x8
/* brief MPE2 bit is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_MPE2_DONTCARE		0x10
/* brief EP/Egress Port field is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_EP_DONTCARE		0x20
/* brief TC/Class field is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_TC_DONTCARE		0x40
#define CBM_QUEUE_MAP_F_COLOR_DONTCARE		0x400
/** Special input flag to get the Enqueue port resources*/
#define DP_F_DEQ_INVALID 0x0
#define	DP_F_ENQ_CPU_0 0x2
#define	DP_F_ENQ_CPU_1 0x3
#define	DP_F_ENQ_CPU_2 0x4
#define	DP_F_ENQ_CPU_3 0x5
#define	DP_F_ENQ_WAVE  0x6
#define	DP_F_ENQ_GSWIPL 0x7
#define DP_F_ENQ_PAE 0x8
#define	DP_F_ENQ_TSO 0x9
#define	DP_F_ENQ_VRX318 0xa
#define	DP_F_ENQ_DL 0xb
#define DP_F_DEQ_CPU 0x2
#define DP_F_DEQ_CPU1 0x3
#define DP_F_DEQ_MPE 0x4
#define DP_F_DEQ_DL 0x5

/** Special input flag to enable/disable datapath queues*/
#define CBM_Q_F_DISABLE 0x10000000
#define CBM_Q_F_NODEQUEUE 0x20000000
#define CBM_Q_F_CKSUM 0x40000000
#define CBM_Q_F_FLUSH 0x80000000
#define CBM_Q_F_FORCE_FLUSH 0x00000001
#define CBM_Q_F_RESTORE_ONLY 0x00000002

#define CQM_MAX_CPU 4

#define CBM_PORT_DP_SET			BIT(0)
#define CBM_PORT_DQ_SET			BIT(1)
#define CBM_PORT_DMA_CHAN_SET		BIT(2)
#define CBM_PORT_PKT_CRDT_SET		BIT(3)
#define CBM_PORT_BYTE_CRDT_SET		BIT(4)
#define CBM_PORT_RING_ADDR_SET		BIT(5)
#define CBM_PORT_RING_SIZE_SET		BIT(6)
#define CBM_PORT_RING_OFFSET_SET	BIT(7)

#define CBM_STD_BUF_SIZE 2048

enum DP_SPL_TYPE;
struct dp_spl_cfg;
struct net_device;
struct dp_dma_desc;

/** addtogroup CBM_Driver_Port_Structures */

/* brief CBM Queue Map table Entry*/
typedef struct {
	uint32_t	flowid:2; /* FlowId (Bits 7:6) */
	uint32_t	dec:1; /* VPN Decrypt flag */
	uint32_t	enc:1; /* VPN Encrypt flag */
	uint32_t	mpe1:1; /* MPE1 Flag */
	uint32_t	mpe2:1; /* MPE2 Flag */
	uint32_t	ep:4; /* PMAC Egress Port number */
	uint32_t	tc:4; /* Traffic Class */
	uint32_t	sub_if_id:16;/*sub if id*/
	/**
	 * sub_if_id bit mask
	 * set to 1 means
	 * don't care
	 */
	uint32_t	sub_if_id_mask_bits:16;
	uint32_t        color:2; /* color */
	uint32_t	egflag:1; /* egflag. to select map table */
	uint32_t	resv:15;/*resv*/
} cbm_queue_map_entry_t;

/* brief TMU resource structure for a TMU port */
typedef struct cbm_tmu_res {
	/* TMU port number, -1 if no TMU port allocated */
	u32 tmu_port;
	/* CBM Dequeue port number, -1 if no CBM port allocated */
	u32 cbm_deq_port;
	/* TMU scheduler Id attached to TMU port, -1 if unassigned */
	s32 tmu_sched;
	/**
	 *TMU Queue Id attached to TMU scheduler
	 *on this TMU port, -1 if unassigned
	 */
	s32 tmu_q;
} cbm_tmu_res_t;

/**
 * brief CBM Dequeue Resource Info for configuring other
 * HW peripherals to access the CBM
 */
typedef struct cbm_dq_info {
	u32	port_no;	/* CBM Dequeue Port No */
	void		*cbm_dq_port_base; /* CBM Dequeue Port Base Address */
	s32		dma_tx_chan;	/* PMAC DMA Tx Channel */
	u32	num_desc;	/* Number of Descriptors at port base */
	u32	num_free_burst; /* Number of free burst size */
	u32 *pkt_base; /*packet list base DMA virtual address */
	void *pkt_base_paddr;	/* packet list base DMA physical address */
	size_t dma_size;	/* size of allocated DMA memory */
	int prefill_pkt_num; /* number of pre-fill packet buffer requested */
	void *desc_phy_base;
	void *desc_vir_base;
} cbm_dq_info_t;

/**
 * brief CBM Enqueue Resource Info for configuring other
 * HW peripherals to access the CBM
 */
typedef struct cbm_eq_info {
	u32	port_no;	/* CBM Enqueue Port No */
	void		*cbm_eq_port_base; /* CBM enqueue Port Base Address */
	s32		dma_rx_chan_std;/* PMAC DMA Rx Channel for Std Buffer */
	/* PMAC DMA Rx Channel for Jumbo Buffer */
	s32		dma_rx_chan_jumbo;
	u32	num_desc; /* Number of Descriptors at port base */
	/*Number of alloc Descriptors at port base */
	u32	num_alloc_desc;
} cbm_eq_info_t;
/*
 * brief CBM Dequeue Port Resources for a Client
 * - All addrsses are PHYSICAL addresses
 */
typedef struct cbm_dq_port_res {
	/**
	 *CBM Buffer Free Port Base
	 *Address for CPU Ports. This is NULL for DMA
	 *Dequeue Ports
	 */
	void		*cbm_buf_free_base;
	/**
	 *Number of Free Port entries. This is 0
	 *when cbm_buf_free_base = NULL
	 */
	u32	num_free_entries;
	int		num_deq_ports; /* Number of Dequeue Ports */
	/**
	 *Array of CBM Dequeue Port Related Info
	 *of num_deq_ports entries. Caller must free the memory
	 */
	cbm_dq_info_t	*deq_info;
} cbm_dq_port_res_t;

struct cbm_tx_data {
	int dp_inst; /*input*/
	int cbm_inst; /*input*/
	u8 *pmac; /*NULL: no pmac,otherwise with pmac */
	int pmac_len; /*size of the pmac */
	u8  f_byqos;  /* 1: bypass QOS, 0 with qos */
	u16 l3_chksum_off; /* IP header */
	u16 l4_chksum_off; /* TCP/UDP */
	/* 1: if the destination is a streaming port, else 0 */
	u8  is_streaming;
};

struct cbm_dp_alloc_data {
	int dp_inst; /*input*/
	int cbm_inst; /*input*/
	/*below are output */
	u32 flags;
	u32 dp_port;/*PMAC port number*/
	u32 deq_port_num;/*Number of DQ ports for this dp*/
	u32 deq_port; /*CQE Dequeue Port */
	u32 dma_chan; /* its dma channel, -1 means no DMA channel used */
	u32 tx_pkt_credit;  /*port tx packet credit */
	u32 tx_b_credit;  /*port tx bytes credit */
	/* QOS push addr after shift or mask from QOS HW point of view */
	void *txpush_addr_qos;
	void *txpush_addr; /* QOS push address without any shift/mask */
	u32 tx_ring_size; /* ring size */
	/**
	 *next tx_ring_addr = current tx_ring_addr +
	 * tx_ring_offset
	 */
	u32 tx_ring_offset;
	void *tx_ring_addr_txpush;
	u32 num_dma_chan;
};

struct cbm_dp_en_data {
	int dp_inst; /*input*/
	int cbm_inst; /*input*/
	union {
		u32 deq_port; /*input: -1 means not valid*/
		u32 deq_port_list[8]; /*input: -1 means not valid*/
	};
	int num_deq_port; /* input: number of dequeue port */
	u32 dma_chnl_init;/*input :0 - no init, 1:- init DMA channel*/
	/**
	 *[in] flag to indicate whether need new
	 *   policy for this subif or not.
	 *   if this flat is set, it needs new tx/rx policy
	 */
	u8 f_policy;
	 /**
	  *[in/out] number of continuous policy
	  *   if f_txin_policy set, it is [in]
	  *   else it is [out]
	  */
	int tx_policy_num;
	/**
	 *[in] packet size required
	 *   to alloc one policy
	 *   Valid only if
	 *   f_txin_policy set
	 */
	u16 tx_max_pkt_size[DP_MAX_POLICY_GPID];
	/**
	 *[in/out] policy base
	 *   if f_txin_policy is set, it is [out]
	 *   else it is [in]
	 *   whether it is [in] or [out], CQM driver may
	 *   need to set the policy to these dequeue port
	 *   accordingly
	 */
	u16 tx_policy_base;
	/**
	 *[in] ACA TXIN Ring size.
	 *   if 0, then change, otherwise try to tune
	 *   down the pre-allocated TXIN ring buffer size.
	 *   Only allowed to tune down.
	 */
	int tx_ring_size;
	int rx_policy_num;  /* [in/out] number of continuous rx policy */
	u16 rx_policy_base; /* [in/out] rx policy base */
	u32 num_dma_chan; /* multiple dma tx channel per stream */
	u32 bm_policy_res_id; /* [in] buffer policy id */
};

struct cbm_tx_push {
	s32 deq_port;
	u32 tx_pkt_credit;  /*port tx packet credit */
	u32 tx_b_credit;  /*port tx bytes credit */
	/* push addr after shift or mask from QOS HW point of view */
	void *txpush_addr_qos;
	/* Virtual push addr after shift or mask from QOS HW point of view */
	void *txpush_addr; /* push address without any shift/mask */
	/* Virtual push address without any shift/mask */
	u32 tx_ring_size; /*ring size */
	/*next tx_ring_addr = current tx_ring_addr + tx_ring_offset */
	u32 tx_ring_offset;
	u32 type; /* output */
	/**
	 *[out] IGP port ID.
	 *   Note: DPDK will get igp/allo_buffer/return buffer
	 *         register address based on igp_id as spec
	 *         defined
	 */
	int igp_id;
	/**
	 *[out] flag to indicate whether this igp with or
	 *   without QOS
	 *   0 -- without QOS
	 *   1  -- with QOS
	 */
	u8  f_igp_qos;
};

enum CBM_CORE_TYPE {
	CBM_CORE_LINUX = 0, /* Owned by Linux */
	CBM_CORE_DPDK, /* Owned by DPDK */
};

enum DPDK_DEQ_MODE {
	DEQ_CQM = 0, /* dequeue from CQM */
	DEQ_PPV4, /* dequeue from PPv4 */
};

enum CQM_POLICYMAP {
	RX_POLICYMAP = BIT(0), /* Rx PolicyMap */
	TX_POLICYMAP = BIT(1), /* Tx PolicyMap */
	SP_GPID_POLICYMAP = BIT(2), /* Special GPID Policy map */
};

struct cbm_cpu_port_data {
	u8 dp_inst; /*input*/
	u8 cbm_inst; /*input*/
	enum CBM_CORE_TYPE type[CQM_MAX_CPU][MAX_PORTS_PER_CPU]; /* output */
	/*drop port*/
	struct cbm_tx_push dq_tx_flush_info;
	/*output: deq_port = -1 means not valid*/
	struct cbm_tx_push dq_tx_push_info[CQM_MAX_CPU][MAX_PORTS_PER_CPU];
	/* the number of continuous policy */
	u32 policy_num[CQM_MAX_CPU][MAX_PORTS_PER_CPU];
	s16 policy_base[CQM_MAX_CPU][MAX_PORTS_PER_CPU];
	/* base policy ID for this CQM CPU dequeue port. */

	/** the default CPU DP should map for the default
	 * exception session and lookup table.
	 * For LGM, DP should set up the default exception
	 * session to use the low priority port/queue of
	 * the default cpu
	 * In fact, this parameter should be also valid
	 * for falcon_mx.
	 */
	u8  default_cpu;
	/* re-insertion port */
	struct cbm_dp_alloc_data re_insertion;
};

struct pib_stat {
	u32 en_stat:1;
	u32 pause_stat:1;
	u32 fifo_size:9;
	u32 fifo_full:1;
	u32 fifo_empty:1;
	u32 cmd_ovflw:1;
	u32 cmd_illegal_port:1;
	u32 wakeup_intr:1;
};

/* brief CBM Enqueue Port Resources for a Client*/
typedef struct cbm_eq_port_res {
	int		num_eq_ports; /* Number of Enqueue Ports */
	cbm_eq_info_t	*eq_info; /*	EQ port info */
} cbm_eq_port_res_t;

/** addtogroup CBM_Control_API */
/**
 * brief Get the number of allocated TMU ports, and allocated TMU queues from
 * the CBM Driver.
 * param[out] tmu_port Highest TMU port number allocated/reserved by CBM Driver
 * param[out] tmu_sched Highest TMU scheduler number
 *		allocated/reserved by CBM Driver
 * param[out] tmu_q Highest TMU queue number allocated/reserved by CBM Driver
 * return 0
 */
int32_t
cbm_reserved_dp_resources_get(u32 *tmu_port, u32 *tmu_sched, u32 *tmu_q);

/** addtogroup CBM_Port_Allocation_API */

/**
 * brief Get the TMU resources for a datapath port
 * param[in] dp_port Datapath (PMAC) Port for which TMU resources are queried
 * param[out] num_tmu_ports Number of TMU ports configured for this DP port
 * - typically 1 param[out] res TMU resources assigned to this DP port
 * param[in]  flags - One of the following flags can be passed:
 * - DP_F_DIRECTPATH_RX
 * - DP_F_MPE_ACCEL
 * note For LAN Ethernet ports, two TMU ports are allocated for each LAN port
 * note Caller needs to free the memory  allocated for res
 * note In the CBM Egress port lookup table, special ports are to be identified
 *  by the same flags as passed in this API (additionally DP_F_FAST_WLAN2,
 *  DP_F_FAST_WLAN3)
 * return 0 if DP Port configured; -1 if DP Port does not exists
 */
int32_t
cbm_dp_port_resources_get(u32 *dp_port,
			  u32 *num_tmu_ports,
			  cbm_tmu_res_t **res_pp,
			  u32  flags);

/**
 * brief Get the Datapath Port number for a TMU port
 * param[in] tmu_port TMU port number for which datapath port
 * number is requested
 * param[in]  flags - One of the following flags can be passed:
 * - DP_F_DIRECTPATH_RX
 * - DP_F_MPE_ACCEL
 * return DP Port if success; -1 if DP Port does not exist
 */
int32_t
cbm_dp_port_get(u32 tmu_port, u32 flags);

s32 cbm_cpu_port_get(struct cbm_cpu_port_data *data, u32 flags);

/**
 * brief Enable/Disable the CBM datapath for the given datapath port
 *	(PMAC port)
 * param[in] owner	 Kernel module pointer which owns the port
 * param[in] port_id PMAC port number allocated for the datapath port
 * param[in] flags : Use flags to disable
 * -  CBM_F_DPATH_DISABLE : Disable the datapath for the PMAC port
 * return 0 if OK / CBM_ERROR on Error
 * note This involves disabling the TMU queues mapped to the TMU port
 * corresponding to the given datapath port. See CBM chapter in SWAS
 */
s32 cbm_dp_enable(struct module *owner, u32 dp_port,
		  struct cbm_dp_en_data *data,
		  u32 flags, u32 alloc_flags);

/**
 * brief	Allocates CBM enqueue port number to a physical interface.
 * The physical port may map to an exclusive netdevice like in the
 * case of ethernet LAN ports.
 * param[in] owner	 Kernel module pointer which owns the port
 * param[in] driver_dev_port Physical Port Number of this device managed
 * by the driver - some drivers manage more than 1 port.
 * param[in] flags : Special inputs to alloc routine
 *					- CBM_WAVE500_WLAN_DEV, CBM_VRX300_DEV
 *return Port Id	/ CBM_ERROR
 *note
 */
int32_t
cbm_enqueue_port_alloc(struct module *owner,
		       u32 driver_dev_port, u32 flags);

/**
 * brief	Deallocate CBM enqueue port number allocated to a physical
 * interface. The physical port may map to an exclusive netdevice like
 * in the case of ethernet LAN ports.
 * param[in] owner	 Kernel module pointer which owns the port
 * param[in] cbm_port_id CBM Port number to deallocate.
 * param[in] flags : Whether the CBM Port is
 * return	CBM_OK	/ CBM_ERROR
 * note CBM Port can be deallocated only after the port is Unitialized
 */
int32_t
cbm_enqueue_port_dealloc(struct module *owner, u32 cbm_port_id,
			 u32 flags);

/**
 * brief	Allocates CBM enqueue port number to a physical interface.
 * The physical port may map to an exclusive netdevice like in the case of
 * ethernet LAN ports.
 * In other cases like WLAN, the physical port is a Radio port,
 * while netdevices are
 * Virtual Access Points (VAPs). In this case, the.
 * AP netdevice can be passed. Alternately,
 * param[in] owner	 Kernel module pointer which owns the port
 * param[in] driver_dev_port Physical Port Number of this
 * device managed by the driver
 * param[in] flags : Special inputs to alloc routine
 * - CBM_WAVE500_WLAN_DEV, CBM_VRX300_DEV
 * return Port Id	/ CBM_ERROR
 * note  Internal API
 */
int32_t
cbm_dequeue_port_alloc(struct module *owner, u32 driver_dev_port,
		       u32 flags);

/**
 * brief	Deallocate CBM dequeue port number allocated to a
 * physical interface.
 * The physical port may map to an exclusive netdevice like in the
 * case of ethernet LAN ports.
 * param[in] owner	 Kernel module pointer which owns the port
 * param[in] cbm_port_id CBM Port number to deallocate.
 * param[in] flags : Reserved
 * return	CBM_OK	/ CBM_ERROR
 * note CBM Port can be deallocated only after the port is Unitialized
 * note Internal API
 */
int32_t
cbm_dequeue_port_dealloc(struct module *owner, u32 cbm_port_id,
			 u32 flags);

/** brief	Allocates datapath PMAC port number to a physical
 *			interface. The physical port may map to an
 *			exclusive netdevice like in the case of ethernet
 *			LAN ports. In other cases like WLAN, the physical
 *			port is a Radio port,while netdevices are Virtual
 *			Access Points (VAPs). In this case, the	 AP
 *			netdevice can be passed. Alternately,
 * param[in] owner	 Kernel module pointer which owns the port
 * param[in] dev Pointer to Linux netdevice structure
 * param[in] dev_port Physical Port Number of this device
 *			managed by the driver
 * param[in] port_id Optional port_id number requested.
 *			Usually, 0 and allocated by driver
 * param[in] flags : Special inputs to alloc routine
 *			- CBM_WAVE500_WLAN_DEV, CBM_VRX300_DEV
 * return Port Id	/ CBM_ERROR
 * note
 * note DT map
 * note	DrvMod Name | DrvDevNo | PMACPort No
 * note	laneth		|	0	   | 1
 * note	laneth		|	1	   | 2
 * note	waneth		|	0	   | 15
 */
s32 cbm_dp_port_alloc(struct module *owner,
		      struct net_device *dev,
		      u32 dev_port,
		      s32 dp_port,
		      struct cbm_dp_alloc_data *data,
		      u32 flags);

struct cbm_dp_alloc_complete_data {
	/**
	 * [in] number of rx ring from DC device to Host.
	 * num_rx_ring requirement:
	 * @num_rings <= @DP_RX_RING_NUM
	 * GRX350/Falcon_MX:1 rx ring
	 * LGM: up to 2 rx ring, like Docsis can use 2 rings
	 * For two ring case:
	 * 1st rxout ring without qos
	 * 2nd rxout ring with qos
	 */
	u8 num_rx_ring;
	/**
	 * [in] number of tx ring from Host to DC device
	 * num_rx_ring requirement:
	 * @num_rings <= @DP_TX_RING_NUM
	 * Normally it is 1 TX ring only.
	 * But for 5G, it can support up to 8 TX ring
	 * For docsis, alhtough it is 16 dequeue port to WIB.
	 * But the final ring only 1, ie, WIB to Dcosis
	 */
	u8 num_tx_ring;
	/**
	 * [in] number of UMT port.
	 * Normally is 1 only. But Docsis can use up to 2
	 */
	u8 num_umt_port;
	/* [in/out] DC rx ring info*/
	struct dp_rx_ring *rx_ring[DP_RX_RING_NUM];
	/*[in/out] DC tx ring info*/
	struct dp_tx_ring *tx_ring[DP_TX_RING_NUM];

	/**
	 * [in] for GPID tx information
	 * Valid only if @f_gpid valid.
	 */
	struct dp_gpid_tx_info gpid_info;
	u32 enable_cqm_meta : 1; /* enable CQM buffer meta data marking */
	int alloc_flags; /* original alloc flags used in the dp_alloc_port */
	u32 deq_port;	/* [in] port id which was returned in the alloc */
	int qid_base; /* [in] EPON baseport */
	int num_qid; /* [in] EPON num qid */
	u32 bm_policy_res_id; /* [in] buffer policy id */
	struct dp_dev_opt_param opt_param; /* [in] optional parameters */
	int tx_policy_base; /* [out] tx policy id base */
	int tx_policy_num; /* [out] tx policy num */
};

s32 cbm_dp_port_alloc_complete(struct module *owner,
			       struct net_device *dev,
			       u32 dev_port,
			       s32 dp_port,
			       struct cbm_dp_alloc_complete_data *data,
			       u32 flags);

/**
 * brief	De-Allocates datapath PMAC port number to a physical interface.
 * Virtual Access Points (VAPs). In this case, the
 * AP netdevice can be passed. Alternately,
 * param[in] owner	 Kernel module pointer which owns the port
 * param[in] dev_port Physical Port Number of this device managed by the driver
 * param[in] cbm_port_id CBM port_id number to de-allocate.
 * param[in] flags : Reserved
 * return Port Id	/ CBM_ERROR
 * note  Will be called by the datapath driver when port/interface is deleted
 * like when driver is rmmod
 */
s32 cbm_dp_port_dealloc(struct module *owner,
			u32 dev_port,
			s32 cbm_port_id,
			struct cbm_dp_alloc_data *data,
			u32 flags);
/**
 * brief	Get the CBM CPU Dequeue Path resources for a DP port
 * param[in] dp_port  Datapath/PMAC port number for
 *				which Dequeue resources are asked
 * param[out] res	 CBM Resources on Dequeue Path
 *				in terms of address, number etc
 * param[in] flags :Special Flags can be:
 * DP_F_DIRECTPATH_RX
 * DP_F_MPE_ACCEL
 * DP_F_LRO
 * return CBM_OK / CBM_ERROR
 * note  The Device Tree table for Dequeue Ports,
 * will have two extra parameters indicating
 * a) CPU Dequeue Port (1) or DMA Dequeue Port (0)
 * b) Number of Descriptors on that port
 * This info is going to be used in the API below.
 * Sanity checks on the DT info is desirable at init time.
 */
int32_t
cbm_dequeue_port_resources_get(u32 dp_port,
			       cbm_dq_port_res_t *res,
			       u32 flags);

/**
 * brief	Get the CBM CPU Enqueue Path resources for a DP port
 * param[out] res CBM Resources on Dequeue Path in terms
 * of address, number etc
 * param[in] flags :Special Flags can be:
 * DP_F_ENQ_PAE
 * DP_F_ENQ_VRX318
 * DP_F_ENQ_GSWIPL
 * DP_F_ENQ_TSO
 * return CBM_OK / CBM_ERROR
 * note  The Device Tree table for Enqueue Ports,
 *		will have two extra parameters indicating
 * a) CPU Enqueue Port (1) or DMA Enqueue Port (0)
 * b) Number of Descriptors on that port
 * This info is going to be used in the API below.
 *		Sanity checks on the DT info is desirable
 * at init time.
 */
int32_t
cbm_enqueue_port_resources_get(cbm_eq_port_res_t *res,
			       u32 flags);

/** addtogroup CBM_Port_OPS_API */

/**
 * brief	Initializes CBM Enqueue DMA Port
 * param[in] cbm_port_id CBM port_id to initialize.
 * param[in] dma_hw_num Instance of DMA Engine H/w
 * param[in] std_dma_chan_num DMA Rx Channel Number for
 *			Standard size buffers on the specified DMA Engine
 * param[in] jumbo_dma_chan_num DMA Rx Channel Number for
 *			Jumbo size buffers on the specified DMA Engine
 * param[in] flags : Flags for Port Init routine
 * - CBM_PORT_F_STANDARD_BUF
 * - CBM_PORT_F_JUMBO_BUF
 * return CBM_OK / CBM_ERROR
 * note  One or both of CBM_PORT_F_STANDARD_BUF
 *		and CBM_PORT_F_JUMBO_BUF  must be specified
 */
int32_t
cbm_enqueue_dma_port_init(s32 cbm_port_id,
			  s32 dma_hw_num,
			  u32 std_dma_chan_num,
			  u32 jumbo_dma_chan_num,
			  u32 flags);
/**
 * brief	Uninitialize a initialized CBM Enqueue DMA port,
 * which dissociates all DMA setup with the port.
 * param[in] cbm_port_id CBM port_id to un-initialize.
 * param[in] flags : Reserved
 * return CBM_OK / CBM_ERROR
 * note
 */
int32_t
cbm_enqueue_dma_port_uninit(s32 cbm_port_id, u32 flags);

/**
 * brief	Initializes CBM Dequeue DMA Port
 * param[in] cbm_port_id CBM port_id to initialize.
 * param[in] dma_hw_num Instance of DMA Engine H/w
 * param[in] dma_chan_num DMA Tx Channel Number on the
 * specified DMA Engine
 * param[in] flags : Flags for Port Init routine
 * return CBM_OK / CBM_ERROR
 * note  On the Dequeue port, the same DMA channel
 * can carry Standard or Jumbo sized buffers
 */
int32_t
cbm_dequeue_dma_port_init(s32 cbm_port_id,
			  s32 dma_hw_num,
			  u32 dma_chan_num,
			  u32 flags);

/**
 * brief	Uninitialize a initialized CBM dequeue DMA port,
 * which dissociates all DMA setup with the port.
 * param[in] cbm_port_id CBM port_id to un-initialize.
 * param[in] flags : Reserved
 * return CBM_OK / CBM_ERROR
 * note
 */
int32_t
cbm_dequeue_dma_port_uninit(s32 cbm_port_id, u32 flags);

/**
 * brief	Enable CBM Enqueue or Dequeue DMA Port
 * param[in] cbm_port_id CBM port_id to initialize.
 * param[in] flags : Flags for Port Enable routine
 * - CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
 * - CBM_PORT_F_DISABLE - if set, disable the port; else enable the port
 * return CBM_OK / CBM_ERROR
 * note
 */

int32_t
cbm_dma_port_enable(s32 cbm_port_id, u32 flags);

/**
 * brief	Enable/Disable a Datapath Port TMU Queue in CBM
 * param[in] dp_port_id Datapath/PMAC port number whose queue to operate on.
 * if dp_port_id=-1, the operation is carried out only on qnum or based
 * on special queue flags like CBM_Q_F_CKSUM.
 * param[in] qnum : Queue number to enable/disable.
 * If qnum=-1, then special flags implicitly determine the queue number.
 * param[in] flags : Flags for Port Enable routine
 * CBM_Q_F_DISABLE - Disable the Queue
 * - CBM_Q_F_NODEQUEUE - if set, Don't drain out all
 * the enqueued packets before disabling the queue which
 * is the default behaviour when disabling a queue
 * - CBM_Q_F_CKSUM : Special Queue for checksum
 * return CBM_OK / CBM_ERROR
 * note  CBM Driver allows a safe enable/disable of the queue, by first
 * mapping all CBM Queue Map entries to this queue to the Global Drop queue,
 * draining all packets out if required, enabling/disabling the queue, and
 * mapping the previous CBM Queue Map entries back to this queue
 */
int32_t
cbm_dp_q_enable(int cbm_inst, s32 dp_port_id,
		s32 qnum,
		s32 tmu_port_id,
		s32 remap_to_qid,
		u32 timeout,
		s32 qidt_valid,
		u32 flags);

/** addtogroup CBM_Stats_API */
/**
 * brief	Get the CBM Enqueue DMA Port Stats
 * param[in] cbm_port_id CBM port_id for which to get stats
 * param[out] occupancy_ctr	Enqueue Port Occupancy Counter
 * param[out] enq_ctr	Enqueue Port Enqueue Counter
 * param[in] flags : Flags that qualify the configuration
 * return CBM_OK / CBM_ERROR
 * note
 */
int32_t
cbm_enqueue_dma_port_stats_get(s32 cbm_port_id,
			       u32 *occupancy_ctr,
			       u32 *enq_ctr,
			       u32 flags);

/** brief	Get the CBM Dequeue DMA Port Stats
 * param[in] cbm_port_id CBM port_id for which to get stats
 * param[out] deq_ctr	Dequeue Port Dequeue Counter
 * param[in] flags : Flags that qualify the configuration
 * return CBM_OK / CBM_ERROR
 * note
 */
int32_t
cbm_dequeue_dma_port_stats_get(s32 cbm_port_id,
			       u32 *deq_ctr, u32 flags);

/**
 * brief	Get the CBM DMA Queue Stats
 * param[in] cbm_queue_id CBM/TMU Queue Id [0-255]
 *		for which stats are to be returned
 * param[out] ctr	Queue Pkt/Byte counter as per Queue Count config
 * param[in] flags : Flags that qualify the configuration
 * return CBM_OK / CBM_ERROR
 * note
 */
int32_t
cbm_queue_stats_get(s32 cbm_queue_id, u32 *ctr, u32 flags);

/**
 * brief	Request a CBM Buffer of the indicated Buffer Size Pool/Bin
 * param[in] vpe_id  CPU/VPE# (0-3) which requests the buffer
 * param[in] flags : Flags that qualify the configuration
 * - CBM_PORT_F_DEQUEUE_PORT - if set, Get Standard Pool Buffers
 * - CBM_PORT_F_JUMBO_BUF - if set, Get Jumbo Pool Buffers
 * return CBM Buffer Pointer / NULL if out of buffers
 * note
 */
void *cbm_buffer_alloc(u32 pid, u32 flag, u32 size);
/**
 * if this bit is set,
 * cqm_buffer_alloc_by_policy should
 * return the base without reserve
 * any header room.
 * especially for DC RXOUT ring buffer
 * Note: it is mainly for falcon_mx to
 * pre-allocate buffer for rxout packet
 * buffer
 */

struct cqm_bm_alloc {
	int cbm_inst;
	u32 pid;
	u32 flag;
	u32 policy;
	unsigned long dma_addr;
};

struct cqm_bm_free {
	int cbm_inst;
	u32 flag;
	void *buf;   /* Physical address */
	u32 policy_base; /* base policy */
	int policy_num;  /* number of policy */
};

void *cqm_buffer_alloc_by_policy(struct cqm_bm_alloc *alloc_info);
int cqm_buffer_free_by_policy(struct cqm_bm_free *free_info);

/**
 * brief	Free a CBM Buffer to one of the CBM Free Buffer Pools
 * param[in] vpe_id  CPU/VPE# (0-3) which requests the buffer ;
 *				if -1, current VPE Id is used
 * param[in] buf  Pointer to CBM Buffer
 * param[in] flags : Flags that qualify the configuration
 * - CBM_PORT_F_DEQUEUE_PORT - if set, Get Standard Pool Buffers
 * - CBM_PORT_F_JUMBO_BUF - if set, Get Jumbo Pool Buffers
 * return CBM_OK / CBM_ERROR if Buffer pointer is invalid
 * note Pointer to CBM buffer need not be at the very start of
 * the buffer. It can point to anywhere inside the buffer space
 */
int cbm_buffer_free(u32 pid, void *v_buf, u32 flag);

/**
 * brief	Tx a Packet Buffer to the Egress Port
 * param[in] skb  Pointer to Linux skb packet meta-data/buffer
 * param[in] pmac_port_id	Egress PMAC/Datapath Port Id
 * param[in] flags : Flags that qualify the configuration
 * return CBM_OK / CBM_ERROR if Buffer pointer is invalid
 * note Pointer to CBM buffer need not be at the very start of
 *	the buffer. It can point to anywhere inside the buffer space
 */
s32 cbm_cpu_pkt_tx(struct sk_buff *skb,
		   struct cbm_tx_data *data,
		   u32 flags);

/**
 * brief	Rx Flow Control function
 * param[in] pmac_port_id Datapath/PMAC port Id on which Datapath/PMAC port
 *			Id on which datapath driver indicates backpressure
 * param[in] subif	 sub-interface Id on which backpressure is
 *				asserted (currently reserved)
 * param[in] flags : Flags that qualify the backpressure mechanism
 * - CBM_CPU_F_RX_STOP - Rx Stop
 * - CBM_CPU_F_RX_START - Rx Start
 * return CBM_OK / CBM_ERROR
 * note Use of subif is reserved for future
 */
int32_t
cbm_cpu_rx_flow_control(s32 pmac_port_id,
			u32 subif,
			u32 flags);

/**
 * brief	CBM Queue Map table entry/entries Set
 * param[in] tmu_queue_id TMU QueueId (0-255)
 * param[in] entry	 pointer to Queue map structure
 * param[in] flags : Flags that qualify the Mapping mechanism
 * - CBM_QUEUE_MAP_F_FLOWID_DONTCARE
 * - CBM_QUEUE_MAP_F_EN_DONTCARE
 * - CBM_QUEUE_MAP_F_DE_DONTCARE
 * - CBM_QUEUE_MAP_F_MPE1_DONTCARE
 * - CBM_QUEUE_MAP_F_MPE2_DONTCARE
 * - CBM_QUEUE_MAP_F_EP_DONTCARE
 * - CBM_QUEUE_MAP_F_TC_DONTCARE
 * return CBM_OK / CBM_ERROR
 * note Map Entry fields which are specified as DONTCARE in the flags,
 * result in multiple entries being created which map to the same tmu_queue_id.
 * Also, VRX318 has TC=qid setting.
 * note The API implementation needs to maintain a DDR Queue Map table
 * corresponding to the CBM HW QIDT with an extra flags field. Marking DROP flag
 * in this field of an entry internally, means QIDT entry in CBM HW set to
 * Global Drop Queue (instead of queue num in Queue Map table entry). This
 * allows to easily remap back to the right queue after operations like disable
 * etc.
 * note The implementation must also maintain an array
 * of all queue numbers, which contains the reference count of how many
 * Queue Map table entries point to this queue
 * On every set, reference count of original queue
 * is decremented, and new queue is incremented.
 */
int32_t
cbm_queue_map_set(int cbm_inst,
		  s32 queue_id,
		  cbm_queue_map_entry_t *entry,
		  u32 flags);

/**
 * brief	CBM Queue Map table entry/entries get for a given tmu_queue_id
 * param[in] tmu_queue_id TMU QueueId (0-255)
 * param[out] num_entries returned in entries array which
 * is allocated by CBM Driver
 * param[out] entries	pointer to Queue map structure
 * param[in] flags : Reserved
 * return CBM_OK / CBM_ERROR
 * note The caller needs to free up the entries buffer
 * note if tmu_queue_id = -1, then all queue map entries are returned,
 *	aka the whole CBM Queue Map table
 */
int32_t
cbm_queue_map_get(int cbm_inst, s32 queue_id,
		  s32 *num_entries,
		  cbm_queue_map_entry_t **entries,
		  u32 flags);

/** brief	CBM Queue Map entries buffer free
 *  param[in] entries	pointer to Queue map structure
 */
void
cbm_queue_map_buf_free(cbm_queue_map_entry_t *entries);

/**
 * brief	CBM Mode table entry/entries Set
 * param[in] entry	 pointer to mode structure
 *			(only ep, mpe1, mpe2, mode fields are setup)
 * param[in] flags : Flags that qualify the Mapping mechanism
 * - CBM_QUEUE_MAP_F_MPE1_DONTCARE
 * - CBM_QUEUE_MAP_F_MPE2_DONTCARE
 * - CBM_QUEUE_MAP_F_EP_DONTCARE
 * return CBM_OK / CBM_ERROR
 * note Map Entry fields which are specified as DONTCARE in the flags,
 * result in multiple entries being created which map to the same mode.
 */
s32
cqm_mode_table_set(int cbm_inst,
		   cbm_queue_map_entry_t *entry,
		   u32 mode, u32 flags);

/**
 * brief	CBM Mode table entry/entries get for a given ep/mpe1/mpe2 flags
 * param[in] mode
 * param[out] num_entries returned in entries array
 *			which is allocated by CBM Driver
 * param[out] entries	pointer to Queue map structure where only the
 *				ep, mpe1, mpe2, mode fields are valid
 * param[in] flags : Reserved
 * return CBM_OK / CBM_ERROR
 * note The caller needs to free up the entries buffer
 * note if mode = -1, then all mode table entries are returned,
 *			aka the whole CBM mode table
 */

s32
cqm_mode_table_get(int cbm_inst, int *mode,
		   cbm_queue_map_entry_t *entry,
		   u32 flags);

s32 cqm_qid2ep_map_set(int qid, int port);
s32 cqm_qid2ep_map_get(int qid, int *port);

/**
 * brief	CBM Dequeue Port Queues Flush in TMU
 * param[in] cbm_port_id CBM Dequeue Egress Port
 * param[in] tmu_queue_id TMU queue_id attached to cbm_port_id.
 *		If tmu_queue_id=-1, all queues attached to cbm_port_id need
 *		to be flushed - Currently only all queues attached to
 *		the port can be flushed
 * param[in] flags : Reserved
 * return CBM_OK / CBM_ERROR
 * note The caller needs to free up the entries buffer
 */
int32_t
cbm_queue_flush(s32 cbm_port_id, s32 tmu_queue_id,
		u32 timeout, u32 flags);

/**
 * brief	CBM Queue Flush
 * param[in] cqm_inst CBM Instance
 * param[in] cqm_drop_port CBM Dequeue Egress Port
 * param[in] qid : Queue ID
 * param[in] nodeid : logical Queue ID
 * return CBM_OK / CBM_ERROR
 */
int32_t cqm_qos_queue_flush(s32 cqm_inst,
			    s32 cqm_drop_port,
			    s32 qid,
			    s32 nodeid);

/**
 * brief	CBM Buffer Update Repeat Count (RCNT)
 * param[in] vpe_id Use CBM CPU RCNTINC/DEC port corresponding
 *	to VPE/CPU Number
 * param[in] buffer Pointer to CBM buffer
 * param[in] flags : Whether to Increment or Decrement RCNT for buffer
 * - CBM_CPU_BUF_INC_RCNT
 * - CBM_CPU_BUF_DEC_RCNT
 * return RCNT / CBM_ERROR
 * note The buffer pointer passed can be anywhere in the span of the CBM buffer
 */
int32_t
cbm_buffer_upd_rcnt(s32 vpe_id, void *buffer, u32 flags);
/** addtogroup CBM_Stats_API */
/**
 * brief	Build an skb using cbm buffer
 * param[out] data	 Pointer to cbm data buffer
 * param[in] frag_size : Reserved
 * return Pointer to sk_buff allocated / NULL on error
 * note
 */
struct sk_buff *cbm_build_skb(void *data, unsigned int frag_size,
			      gfp_t priority);

/**
 * brief	Copy an skb using cbm buffer to another skb using cbm buffer
 * param[in] size	Size of data buffer requested
 * param[in] priority : Priority of the allocation
 *		- GFP_ATOMIC, GFP_KERNEL etc
 * return Pointer to sk_buff allocated / NULL on error
 * note
 */
struct sk_buff *cbm_copy_skb(const struct sk_buff *skb,
			     gfp_t gfp_mask);

/**
 * brief	Allocate an skb using cbm buffer
 * param[in] size	Size of data buffer requested
 * param[in] priority : Priority of the allocation - GFP_ATOMIC, GFP_KERNEL etc
 * return Pointer to sk_buff allocated / NULL on error
 * note
 */
struct sk_buff *cbm_alloc_skb(unsigned int size,
			      gfp_t priority);

/**
 * brief	Linearize an skb using cbm buffer so that pkt can tx to CBM
 * param[in] size	Size of data buffer requested
 * return Pointer to sk_buff allocated / NULL on error
 * note The function will check if there is any need to linearize
 * the input skb, if not it will return the input skb pointer
 */
struct sk_buff *cbm_linearize_buf(struct sk_buff *skb);

struct cbm_desc {
	u32 desc0;
	u32 desc1;
	u32 desc2;
	u32 desc3;
};

struct sk_buff *
cbm_alloc_skb_by_size(u32 size, gfp_t priority);
int32_t
cbm_enqueue_port_overhead_set(s32 port_id, int8_t ovh);
int32_t
cbm_enqueue_port_overhead_get(s32 port_id, int8_t *ovh);

struct cbm_lookup {
	u32 egflag;
	u32 index;
	u32 qid;
};

struct cbm_lookup_entry {
	int entry[MAX_LOOKUP_TBL_SIZE];
	u32 egflag[MAX_LOOKUP_TBL];
	 /* num of valid lookup entries save in entry[] array */
	int num[MAX_LOOKUP_TBL];
	u32 qid;
	s32 ppv4_drop_q;
};

/**
 * flags : STD Buffer or Jumbo buffer using existing defines
 * Caller must have checked the parameter validity etc before
 * calling this API
 */
int cbm_cpu_enqueue_hw(u32 pid,
		       struct cbm_desc *desc,
		       void *data_pointer,
		       int flags);
int cbm_get_wlan_umt_pid(u32 ep_id,  u32 *cbm_pid);
int cbm_counter_mode_set(int enq, int mode);
int cbm_counter_mode_get(int enq, int *mode);
int cbm_q_thres_set(u32 length);
int cbm_q_thres_get(u32 *length);
s32 cbm_igp_delay_set(s32 cbm_port_id, s32 delay);
int32_t
cbm_igp_delay_get(s32 cbm_port_id, s32 *delay);
s32 cbm_port_quick_reset(s32 cbm_port_id, u32 flags);
u32 cbm_get_dptr_scpu_egp_count(u32 cbm_port_id, u32 flags);
u32 cbm_get_std_free_count(void);
void set_lookup_qid_via_index(struct cbm_lookup *info);
#if !IS_ENABLED(CONFIG_GRX500_CBM)
uint8_t get_lookup_qid_via_index(struct cbm_lookup *info);
#else
uint8_t get_lookup_qid_via_index(u32 index);
#endif
void cqm_map_to_drop_q(struct cbm_lookup_entry *lu_info);
void cqm_restore_orig_q(const struct cbm_lookup_entry *lu);
#if !IS_ENABLED(CONFIG_GRX500_CBM)
int check_ptr_validation(void *buf);
#else
int check_ptr_validation(u32 buf);
#endif
void cbm_setup_DMA_p2p(void);
int cbm_turn_on_DMA_p2p(void);
void reset_enq_counter_all(void);
void reset_deq_counter_all(void);
void reset_enq_counter(uint32_t index);
void reset_deq_counter(uint32_t index);
int32_t cbm_get_egress_port_info(u32 cbm_port, u32 *tx_ch,
				 u32 *flags);
int cbm_buff_resv_bytes_get(int cbm_inst, int size);
s32 cbm_port_quick_reset(s32 cbm_port_id, u32 flags);
struct cbm_dc_res {
	int cqm_inst;    /*! <in> cqm instance */
	int dp_port;     /*! <in> dp port id/lpid/ep */
	int res_id;      /*! <in> resource ID */
	int alloc_flags; /*! <in> port alloc_flags */
	struct dp_dc_rx_res rx_res; /*! <out> rx resource */
	struct dp_dc_tx_res tx_res; /*! <out> tx resource */
};

int cbm_dp_get_dc_config(struct cbm_dc_res *res, int flag);
struct cbm_gpid_lpid {
	int cbm_inst; /* [in] */
	u16 gpid; /* [in] */
	u16 lpid; /* [in] */
	int flag; /* [in] reserved for future */
};

struct cbm_dflt_resv {
	int dp_inst; /* [in] */
	int cbm_inst; /* [in] */
	u32 alloc_flags; /* [in] */
	u32 dp_port;/* [in] dp port id */
	 /* [in] for CPU Special connection only */
	enum DP_SPL_TYPE spl_conn_type;
	int headroom; /* [out] default headroom reservation in bytes */
	int tailroom;  /* [out] default headroom reservation in bytes */
};

struct cbm_mtu {
	int cbm_inst;	/* [in] */
	int dp_port;	/* [in] */
	int policy_map;	/* [in] */
	int alloc_flag;	/* [in] */
	int subif_flag;	/* [in] */
	int mtu;	/* [out] */
};

s32 cbm_get_mtu_size(struct cbm_mtu *mtu);

#ifdef CONFIG_LGM_CQM
/**
 * brief      Build an skb using cbm buffer
 * param[out] data         Pointer to cbm data buffer
 * param[in] len data length
 * param[in] pool
 * param[in] policy
 * return Pointer to sk_buff allocated / NULL on error
 * note
 */
struct sk_buff *cbm_build_skb_rx(const struct dp_dma_desc *desc);

/**
 * brief     Get policymap which is based on the buffer
 *	size match with sys pool
 * param[out] policy_map
 * param[in] policy base number
 * param[in] range of policies
 * param[in] alloc flags from DPM
 * param[in] RX/TX Policy Map
 * note
 */
s32 cqm_get_policy_map(int inst,
		       int base,
		       int range,
		       u32 alloc_flags,
		       int flags);

/**
 * brief     do a dummy read to trigger buffer allocation
 * param[in] enq port id
 * param[in] deq port id
 * note
 */
s32 cqm_dc_buffer_dummy_request(u32 enqport, u32 deqport);

int cbm_skb_buff_free(u32 pid, struct sk_buff *skb, u32 flag);

struct dma_rx_desc_0 {
	/* DWORD 0 */
	union {
		struct {
			u32 dest_sub_if_id:16;
			u32 dw0bit16:1;
			u32 flow_id:8;
			u32 dw0bit31:7;
		} __packed field;
		struct {
			u32 dest_sub_if_id:16;
			u32 dw0bit16:1;
			u32 flow_id:8;
			u32 res:1;
			u32 h_mode:1;
			u32 egflag:1;
			u32 classid:4;
		} __packed qos_fmt;
		u32 all;
	};
} __packed;

struct dma_rx_desc_1 {
	/* DWORD 1 */
	union {
		struct {
			u32 classid:4;
			u32 ep:8;	/* This is the Gpid/Port */
			u32 color:2;
			u32 lro_type:2;
			u32 enc:1;
			u32 dec:1;
			u32 src_pool:4;
			u32 pkt_type:2;
			u32 fcs:1;
			u32 classen:1;
			u32 pre_l2:2;
			u32 ts:1;
			u32 pmac:1;
			u32 header_mode:1;
			u32 redir:1;  /* egflag */
		} __packed field;
		u32 all;
	};
} __packed;

struct dma_rx_desc_2 {
	/*DWORD 2 */
	union {
		struct {
			u32 byte_offset:3;
			u32 data_ptr:29;
		} __packed field;
		u32 all;
	};

} __packed;

struct dma_rx_desc_3 {
	/*DWORD 3 */
	union {
		struct {
			u32 data_len:14;
			u32 pool_policy:8;
			u32 sp:1;
			u32 haddr:4;
			u32 dic:1;
			u32 eop:1;
			u32 sop:1;
			u32 c:1;
			u32 own:1;
		} __packed field;
		u32 all;
	};
} __packed;

#define dma_tx_desc_0 dma_rx_desc_0
#define dma_tx_desc_1 dma_rx_desc_1
#define dma_tx_desc_2 dma_rx_desc_2
#define dma_tx_desc_3 dma_rx_desc_3

extern u16 CPUBUFF_MASK[];
extern unsigned long CPUBUFF_SZ[];
extern u8 CPUBUFF_SHF[];

struct cqm_cpubuff_pool {
	dma_addr_t start;
	int size;
	atomic_t *ref;
	unsigned long *ref_chk;
};

struct cqm_cpubuff_regs {
	void __iomem *ret;
};

struct cqm_cpubuff {
	struct cqm_cpubuff_pool pool[CQM_CPU_POOL_SIZE];
	struct cqm_cpubuff_regs regs[NR_CPUS];
	struct device *dev;
	int policy_base;
	int pool_base;
	int size;
};

/** get default cbm headroom/tailroom reservation in bytes */
int cqm_get_dflt_resv(struct cbm_dflt_resv *resv, int flag);
int cbm_gpid_lpid_map(struct cbm_gpid_lpid  *map);
int cbm_dp_spl_conn(int cbm_inst, struct dp_spl_cfg *conn);

/**
 * intel_pcie_dc_iatu_inbound_add - add inbound iatu region for device
 * @sysdata: pointer to PCI sysdata. It should be passed from pci device
 * @start: start address to be translated
 * @size: address size to be translated
 * @atu_index: pointer to the returned iatu index.
 *
 * The inbound address translation for the specified address and size. It
 * is a workaround for 32bit device to access 36 bit address range or IOC->
 * NIOC.
 * Please note, start address and size must be at least 4KB alignment. Caller
 * should keep atu_index which will be used when the iatu region is removed.
 * caller should make sure no overlap address range as start and start + size -1
 * as parameters. The assumption is that allocated address from OS should have
 * no overlapped.
 * return value must be checked before moving forward after calling this API
 */
int intel_pcie_dc_iatu_inbound_add(void *sysdata, u32 start, size_t size,
				   u32 *atu_index);

/**
 * intel_pcie_dc_iatu_inbound_remove - remove inbound iatu mapping for device
 * @sysdata: PCI sysdata. It should be passed back from pci device
 * @atu_index: returned iatu index from intel_pcie_dc_iatu_inbound_add.
 *
 * Remove atu_index specified address translation region
 */
int intel_pcie_dc_iatu_inbound_remove(void *sysdata, u32 atu_index);
#endif

struct cbm_ops {
	s32 (*cbm_queue_delay_enable_set)(s32 enable, s32 queue);
	s32 (*cbm_igp_delay_set)(s32 cbm_port_id, s32 delay);
	s32 (*cbm_igp_delay_get)(s32 cbm_port_id, s32 *delay);
	struct sk_buff *(*cbm_build_skb)(void *data, unsigned int frag_size,
					 gfp_t priority);
	struct sk_buff *(*cbm_build_skb_rx)(const struct dp_dma_desc *desc);
	s32 (*cbm_queue_map_get)(int cbm_inst, s32 queue_id, s32 *num_entries,
				 cbm_queue_map_entry_t **entries, u32 flags);
	s32 (*cbm_queue_map_set)(int cbm_inst, s32 queue_id,
				 cbm_queue_map_entry_t *entry, u32 flags);
	void (*cbm_queue_map_buf_free)(cbm_queue_map_entry_t *entry);
	s32 (*cqm_qid2ep_map_get)(int qid, int *port);
	s32 (*cqm_qid2ep_map_set)(int qid, int port);
	int (*cbm_gpid_lpid_map)(struct cbm_gpid_lpid  *map);
	s32 (*cqm_mode_table_get)(int cbm_inst, int *mode,
				  cbm_queue_map_entry_t *entry, u32 flags);
	s32 (*cqm_mode_table_set)(int cbm_inst,
				  cbm_queue_map_entry_t *entry, u32 mode,
				  u32 flags);
	int (*cbm_setup_desc)(struct cbm_desc *desc, u32 data_ptr, u32 data_len,
			      u32 DW0, u32 DW1);
	int (*cbm_cpu_enqueue_hw)(u32 pid, struct cbm_desc *desc,
				  void *data_pointer,  int flags);
	void *(*cbm_buffer_alloc)(u32 pid, u32 flag, u32 size);
	void *(*cqm_buffer_alloc_by_policy)(struct cqm_bm_alloc *alloc_info);
	s32 (*cqm_buffer_free_by_policy)(struct cqm_bm_free *info);
	struct sk_buff *(*cbm_copy_skb)(const struct sk_buff *skb,
					gfp_t gfp_mask);
	struct sk_buff *(*cbm_alloc_skb)(unsigned int size, gfp_t priority);
	int (*cbm_buffer_free)(u32 pid, void *v_buf, u32 flag);
	int (*cbm_skb_buff_free)(u32 pid, struct sk_buff *skb, u32 flag);
	#if !IS_ENABLED(CONFIG_GRX500_CBM)
	int (*check_ptr_validation)(void *buf);
	#else
	int (*check_ptr_validation)(u32 buf);
	#endif
	s32 (*cbm_cpu_pkt_tx)(struct sk_buff *skb, struct cbm_tx_data *data,
			      u32 flags);
	s32 (*cbm_port_quick_reset)(s32 cbm_port_id, u32 flags);
	u32 (*cbm_get_dptr_scpu_egp_count)(u32 cbm_port_id, u32 flags);
	s32 (*cbm_dp_port_alloc)(struct module *owner, struct net_device *dev,
				 u32 dev_port, s32 dp_port,
				 struct cbm_dp_alloc_data *data, u32 flags);
	s32 (*cbm_dp_port_alloc_complete)(struct module *owner,
					  struct net_device *dev,
		u32 dev_port, s32 dp_port,
		struct cbm_dp_alloc_complete_data *data,
		u32 flags);
	s32 (*cbm_dp_spl_conn)(int cbm_inst, struct dp_spl_cfg *conn);
	int (*cbm_get_wlan_umt_pid)(u32 ep_id, u32 *cbm_pid);
	s32 (*cbm_dp_enable)(struct module *owner, u32 dp_port,
			     struct cbm_dp_en_data *data, u32 flags,
			     u32 alloc_flags);
	s32 (*cqm_qos_queue_flush)(s32 cqm_inst, s32 cqm_drop_port, int qid,
				   int nodeid);
	s32 (*cbm_queue_flush)(s32 cbm_port_id, s32 queue_id, u32 timeout,
			       u32 flags);
	s32 (*cbm_dp_q_enable)(int cbm_inst, s32 dp_port_id, s32 qnum,
			       s32 tmu_port_id, s32 remap_to_qid, u32 timeout,
			       s32 qidt_valid, u32 flags);
	s32 (*cbm_enqueue_port_resources_get)(cbm_eq_port_res_t *res,
					      u32 flags);
	s32 (*cbm_dequeue_port_resources_get)(u32 dp_port,
					      cbm_dq_port_res_t *res,
					      u32 flags);
	s32 (*cbm_dp_port_resources_get)(u32 *dp_port, u32 *num_tmu_ports,
					 cbm_tmu_res_t **res_pp,
					 u32 flags);
	s32 (*cbm_reserved_dp_resources_get)(u32 *tmu_port, u32 *tmu_sched,
					     u32 *tmu_q);
	s32 (*cbm_get_egress_port_info)(u32 cbm_port, u32 *tx_ch, u32 *flags);
	s32 (*cbm_enqueue_port_overhead_set)(s32 port_id, int8_t ovh);
	s32 (*cbm_enqueue_port_overhead_get)(s32 port_id, int8_t *ovh);
	s32 (*cbm_enqueue_cpu_port_stats_get)(s32 cbm_port_id,
					      u32 *occupancy_ctr, u32 *enq_ctr,
					      u32 flags);
	s32 (*cbm_dequeue_dma_port_stats_get)(s32 cbm_port_id, u32 *deq_ctr,
					      u32 flags);
	s32 (*cbm_enqueue_dma_port_stats_get)(s32 cbm_port_id,
					      u32 *occupancy_ctr,
					      u32 *enq_ctr,
					      u32 flags);
	void (*set_lookup_qid_via_index)(struct cbm_lookup *info);
	#if !IS_ENABLED(CONFIG_GRX500_CBM)
	uint8_t (*get_lookup_qid_via_index)(struct cbm_lookup *info);
	#else
	uint8_t (*get_lookup_qid_via_index)(u32 index);
	#endif
	void (*cqm_map_to_drop_q)(struct cbm_lookup_entry *lu_info);
	void (*cqm_restore_orig_q)(const struct cbm_lookup_entry *lu);
	u8 (*get_lookup_qid_via_bits)(u32 ep, u32 classid, u32 mpe1, u32 mpe2,
				      u32 enc, u32 dec,	u8 flow_id, u32 dic);
	int (*cbm_q_thres_get)(u32 *length);
	int (*cbm_q_thres_set)(u32 length);
	s32 (*cbm_dp_port_dealloc)(struct module *owner, u32 dev_port,
				   s32 cbm_port_id,
				   struct cbm_dp_alloc_data *data, u32 flags);
	int (*cbm_counter_mode_set)(int enq, int mode);
	int (*cbm_counter_mode_get)(int enq, int *mode);
	s32 (*cbm_cpu_port_get)(struct cbm_cpu_port_data *data, u32 flags);
	s32 (*pib_program_overshoot)(u32 overshoot_bytes);
	s32 (*pib_status_get)(struct pib_stat *ctrl);
	s32 (*pib_ovflw_cmd_get)(u32 *cmd);
	s32 (*pib_illegal_cmd_get)(u32 *cmd);
	s32 (*pon_deq_cntr_get)(int port, u32 *count);
	void (*cbm_setup_DMA_p2p)(void);
	int (*cbm_turn_on_DMA_p2p)(void);
	int (*cqm_get_dflt_resv)(struct cbm_dflt_resv *resv, int flag);
	s32 (*cqm_dc_buffer_dummy_request)(u32 enqport, u32 deqport);
	s32 (*cbm_enable_backpressure)(s32 port_id, bool flag);
	s32 (*cbm_get_mtu_size)(struct cbm_mtu *mtu);
	s32 (*cqm_get_policy_map)(int inst, s32 policy_base, s32 policy_range,
				  u32 alloc_flags, int flags);
	int (*cbm_dp_get_dc_config)(struct cbm_dc_res *res, int flag);
};

#ifdef CONFIG_LGM_CQM
extern struct cqm_cpubuff *cpubuff;

static inline bool buff_in_cpupool(dma_addr_t pa, int idx)
{
	return pa - cpubuff->pool[idx].start < cpubuff->pool[idx].size;
}

static inline bool cqm_cpubuff_is_valid(dma_addr_t pa)
{
	return pa - cpubuff->pool[0].start < cpubuff->size;
}

static inline int cqm_cpubuff_pool_idx(dma_addr_t pa)
{
	WARN_ON(pa & BIT_ULL(35));
	if (!cpubuff || !cpubuff->size)
		return -1;
	if (!cqm_cpubuff_is_valid(pa))
		return -1;
	if (buff_in_cpupool(pa, 2))
		return 2;
	if (buff_in_cpupool(pa, 1))
		return 1;
	if (buff_in_cpupool(pa, 0))
		return 0;
	return 3;
}

static inline void cqm_cpubuff_return(phys_addr_t pa, int idx)
{
	u64 ioc_pa;
	u32 hi;

	ioc_pa = pa;
	hi = FIELD_PREP(PTRRET_DW3_POOL, idx + cpubuff->pool_base) |
	     FIELD_PREP(PTRRET_DW3_POLICY, idx + cpubuff->policy_base) |
	     FIELD_PREP(PTRRET_DW3_HIADDR, upper_32_bits(ioc_pa));

#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
	writeq_relaxed(lower_32_bits(ioc_pa) | ((u64)hi << 32ULL),
		       cpubuff->regs[smp_processor_id()].ret);
#else
	local_irq_disable();
	writel(pa, cpubuff->regs[smp_processor_id()].ret);
	writel(hi, cpubuff->regs[smp_processor_id()].ret + 4);
	local_irq_enable();
#endif
}

static inline int cqm_ioc_free(u64 ioc_pa)
{
	dma_addr_t pa = ioc_pa;
	int idx;

	idx = cqm_cpubuff_pool_idx(pa);
	if (idx < 0)
		return -ENOENT;
	cqm_cpubuff_return(pa, idx);
	return 0;
}

static inline int cqm_buff_free(void *va)
{
	dma_addr_t pa;
	int idx;

	pa = virt_to_phys(va);
	idx = cqm_cpubuff_pool_idx(pa);
	if (idx < 0)
		return -ENOENT;

	cqm_cpubuff_return(pa, idx);
	return 0;
}

static inline int get_buf_idx(dma_addr_t pa, int pool_idx)
{
	dma_addr_t offset = pa - cpubuff->pool[pool_idx].start;

	return offset >> CPUBUFF_SHF[pool_idx];
}

static inline atomic_t *cqm_page_ref_cnt(int pool_idx, int idx)
{
	return &cpubuff->pool[pool_idx].ref[idx];
}

static inline int cqm_page_ref(dma_addr_t pa)
{
	int pool_idx, idx;

	pool_idx = cqm_cpubuff_pool_idx(pa);
	if (pool_idx < 0)
		return -ENOENT;

	idx = get_buf_idx(pa, pool_idx);
	set_bit(idx, cpubuff->pool[pool_idx].ref_chk);
	return atomic_inc_return(cqm_page_ref_cnt(pool_idx, idx));
}

static inline int cqm_page_unref(dma_addr_t pa)
{
	int pool_idx, idx;

	pool_idx = cqm_cpubuff_pool_idx(pa);
	if (pool_idx < 0)
		return -ENOENT;

	idx = get_buf_idx(pa, pool_idx);
	if (test_bit(idx, cpubuff->pool[pool_idx].ref_chk)) {
		if (!atomic_dec_and_test(cqm_page_ref_cnt(pool_idx, idx)))
			return 0;
		clear_bit(idx, cpubuff->pool[pool_idx].ref_chk);
	}
	cqm_cpubuff_return(pa, pool_idx);
	return 0;
}
#endif
#endif
