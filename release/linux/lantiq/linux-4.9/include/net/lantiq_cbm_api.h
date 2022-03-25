#ifndef __CBM_API_H /* [ */
#define __CBM_API_H
/*! \file cbm_api.h
	\brief This file contains all the Central Buffer Manager (CBM) API
*/

/** \defgroup CBM_Driver CBM Driver
	\brief All API and defines exported by CBM Driver. Look at the constants chapter below.
*/
/* @{ */
/** \defgroup CBM_Driver_Defines CBM Driver Defines
	\brief Defines used in the CBM Driver. Look at the constants chapter below.
*/
/** \defgroup CBM_Driver_Global_Defines CBM Driver Global Defines
	\brief Global Defines used in the CBM Driver. Look at the constants chapter below.
*/
/** \defgroup CBM_Driver_Port_Allocation_Defines CBM Driver Port Allocation Defines
	\brief CBM Port Allocate Defines. Look at the constants chapter below.
*/
/** \defgroup CBM_Driver_Port_Defines CBM Driver Port Defines
	\brief CBM Port Initialization Defines. Look at the constants chapter below.
*/
/** \defgroup CBM_Driver_CPU_Defines CBM Driver CPU Defines
	\brief CBM Driver CPU Related Defines. Look at the constants chapter below.
*/
/** \defgroup CBM_Driver_QMAP_Defines CBM Driver QMAP Defines
	\brief CBM Queue Map Table Field defines. Look at the constants chapter below.
*/
/** \defgroup CBM_Driver_Port_Structures CBM Driver Port Structures
	\brief CBM Port Configuration Structures
*/

/** \defgroup CBM_Control_API CBM Control API
	\brief CBM Control API allows to configure Interrupt, Enqueue and Dequeue manager
	- CBM Enqueue Manager
	- CBM Dequeue Manager
*/

/** \defgroup CBM_Port_Allocation_API CBM Port Allocation API
	\brief CBM Port Allocation API allows to allocate and de-allocate
		- CBM Enqeuue and Dequeue Ports
	- Datapath (PMAC) Ports
*/
/** \defgroup CBM_Port_OPS_API CBM Port Operations API
	\brief CBM Port Operations API - Initialize/Uninitialize, Enable/Disable, Configure CBM port
	- CBM Enqeuue and Dequeue Ports
*/
/** \defgroup CBM_Stats_API CBM Statistics API
	\brief CBM Statistics API
	- CBM Enqeuue and Dequeue Ports
*/
/** \defgroup CBM_CPU_Port_API CBM CPU Port API
	\brief CBM CPU Port API
	- CBM Enqeuue and Dequeue Ports
*/
/** \defgroup CBM_CPU_IRQ_API  CBM CPU IRQ API
	\brief CBM CPU IRQ Control and Enable API
	- CBM Enqeuue and Dequeue Ports
*/
/** \defgroup CBM_CPU_API CBM CPU API
	\brief CBM CPU API
*/
/** \defgroup CBM_CPU_LOAD_SPREADER_API CBM CPU LOAD SPREADER API
	\brief CBM CPU Rx side Load Spreader API.
	- Load Spreader module distributes incoming packet load to multiple Cores/VPEs in GRX500 for maximising SMP Rx Packet performance
*/
/* @} */

#include "datapath_api.h"

#define SINGLE_RX_CH0_ONLY 1

/** \addtogroup CBM_Driver_Global_Defines */
/* @{ */
/*!
  \brief OK return value */
#define CBM_OK			0
/*!
	\brief Error return value */
#define CBM_ERROR		-1
/* @} */


/** \addtogroup CBM_Driver_Port_Allocation_Defines */
/* @{ */
/*!
	\brief Module Flag VRX318 SmartPHY */
#define CBM_MOD_F_VRX318		0x1
/*!
	\brief Module Flag TSO Engine */
#define CBM_MOD_F_TSO			0x2
/*!
	\brief Module Flag LRO Engine */
#define CBM_MOD_F_LRO			0x4
/*!
	\brief Module Flag WAVE500 WLAN */
#define CBM_MOD_F_WAVE500		0x8

/*!
	\brief Flag Legacy 2-DWORD DMA descriptor */
#define CBM_MOD_F_LEGACY		0x10
/*!
	\brief Flag Legacy Desc to 4 DW Invert */
#define CBM_MOD_F_DESC_ORDER_INVERT	0x20
/*!
	\brief Flag CBM Port Type is CPU port */
#define CBM_MOD_F_PORT_TYPE_CPU		0x40
/* @} */

/** \addtogroup CBM_Driver_Port_Defines */
/* @{ */
/*!
	\brief Use Standard Size buffer pool */
#define CBM_PORT_F_STANDARD_BUF		0x1
/*!
	\brief Use Jumbo size Buffer pool */
#define CBM_PORT_F_JUMBO_BUF		0x2

/*!
	\brief CBM Dequeue port */
#define CBM_PORT_F_DEQUEUE_PORT		0x4

/*!
	\brief Disable the CBM port */
#define CBM_PORT_F_DISABLE		0x8

/*!
	\brief CPU CBM port */
#define CBM_PORT_F_CPU_PORT			0x10

/*!
	\brief WAVE500 WLAN special CPU port */
#define CBM_PORT_F_WAVE500_PORT		0x20
/* @} */

/** \addtogroup CBM_Driver_CPU_Defines */
/* @{ */
/*!
	\brief Stop Rx i.e. assert backpressure */
#define CBM_CPU_F_RX_STOP		0x1
/*!
	\brief Start Rx i.e. remove backpressure */
#define CBM_CPU_F_RX_START		0x2

/*!
	\brief RCNT Decrement flag */
#define CBM_CPU_BUF_DEC_RCNT		0x4
/*!
	\brief RCNT Increment flag */
#define CBM_CPU_BUF_INC_RCNT		0x8
/* @} */
/** \addtogroup CBM_Driver_QMAP_Defines */
/* @{ */
/*!
	\brief FlowId bits are don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE		0x1
#define CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE		0x80
#define CBM_QUEUE_MAP_F_SUBIF_DONTCARE		0x100
#define CBM_QUEUE_MAP_F_SUBIF_LSB_DONTCARE		0x200

/*!
	\brief VPN Encrypt bit is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_EN_DONTCARE		0x2
/*!
	\brief VPN Decrypt bit is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_DE_DONTCARE		0x4
/*!
	\brief MPE1 bit is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_MPE1_DONTCARE		0x8
/*!
	\brief MPE2 bit is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_MPE2_DONTCARE		0x10
/*!
	\brief EP/Egress Port field is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_EP_DONTCARE		0x20
/*!
	\brief TC/Class field is don't care for map to QueueId */
#define CBM_QUEUE_MAP_F_TC_DONTCARE		0x40
/* @} */

/*Special input flag to get the Enqueue port resources*/
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

/*Special input flag to enable/disable datapath queues*/
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

/** \addtogroup CBM_Driver_Port_Structures */
/* @{ */
/*! \brief	Datapath Driver Rx callback fn
	\param[in] skb Packet Buffer, skb
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
	*/
typedef
int32_t (*rx_fn)(struct sk_buff *skb,
		uint32_t flags);
/*! \brief	Stop Tx flow control callback fn
	\param[in] pmac_port_id Datapath/PMAC Port Id to apply backpressure on
	\param[in] subif : Sub-interface Id, not used currently
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
typedef
int32_t (*stop_tx_fn)(
	int32_t pmac_port_id,
	uint32_t subif,
	uint32_t flags);

/*! \brief	Start Tx flow control callback fn
	\param[in] pmac_port_id Datapath/PMAC Port Id to apply backpressure on
	\param[in] subif : Sub-interface Id, not used currently
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
typedef
int32_t (*start_tx_fn)(int32_t pmac_port_id,
		uint32_t subif, uint32_t flags);

/*!
	\brief CBM Datapath Registration Callback
*/
typedef struct {
		rx_fn rxfn; /*!< Rx function callback */
		stop_tx_fn stoptxfn; /*!< Stop Tx function callback */
		start_tx_fn starttxfn; /*!< Start Tx function callback */
} cbm_dp_callback_t;
/*!
	\brief CBM Enqueue Manager Control data structure
*/
typedef union cbm_eqm_ctrl {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_eqm_ctrl_t;

/*!
	\brief CBM Dequeue Manager Control data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_dqm_ctrl_t;

/*!
	\brief CBM DMA Dequeue Port Configuration data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_dequeue_dma_port_config_t;
/*!
	\brief CBM DMA Enqueue Port Configuration data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_enqueue_dma_port_config_t;
/*!
	\brief CBM DMA Port Configuration data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
	cbm_enqueue_dma_port_config_t enq_cfg; /*!< CBM Enqueue DMA port config	 */
	cbm_dequeue_dma_port_config_t deq_cfg; /*!< CBM Dequeue DMA port config	 */
} cbm_dma_port_config_t;


/*!
	\brief CBM Enqueue Port Watermark/Threshold Configuration data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	/* Low Watermark & High Watermark */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_port_thresh_t;

/*!
	\brief CBM CPU Dequeue Port Configuration data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_dequeue_cpu_port_config_t;
/*!
	\brief CBM CPU Enqueue Port Configuration data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_enqueue_cpu_port_config_t;
/*!
	\brief CBM CPU Port Configuration data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
	cbm_enqueue_cpu_port_config_t enq_cfg; /*!< CBM Enqueue CPU port config	 */
	cbm_dequeue_cpu_port_config_t deq_cfg; /*!< CBM Dequeue CPU port config	 */
} cbm_cpu_port_config_t;
/*!
	\brief CBM Dequeue CPU Port Interrupt Control data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_dequeue_cpu_port_irq_ctrl_t;
/*!
	\brief CBM Enqueue CPU Port Interrupt Control data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_enqueue_cpu_port_irq_ctrl_t;
/*!
	\brief CBM CPU Port Interrupt Control
*/
typedef union cbm_enqueue_cpu_port_irq_ctrl {
	uint32_t	regval;	/*!< 32-bit register value */
	cbm_enqueue_cpu_port_irq_ctrl_t enq_irq_ctrl; /*!< CBM Enqueue CPU port IRQ Ctrl */
	cbm_dequeue_cpu_port_irq_ctrl_t deq_irq_ctrl; /*!< CBM Dequeue CPU port IRQ Ctrl */
} cbm_cpu_port_irq_ctrl_t;

/*!
	\brief CBM Dequeue DMA Port Interrupt Control data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_dequeue_dma_port_irq_ctrl_t;
/*!
	\brief CBM Enqueue DMA Port Interrupt Control data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_enqueue_dma_port_irq_ctrl_t;
/*!
	\brief CBM DMA Port Interrupt Control
*/
typedef union {
	uint32_t	regval;	/*!< 32-bit register value */
	cbm_enqueue_dma_port_irq_ctrl_t enq_irq_ctrl; /*!< CBM Enqueue DMA port IRQ Ctrl */
	cbm_dequeue_dma_port_irq_ctrl_t deq_irq_ctrl; /*!< CBM Dequeue DMA port IRQ Ctrl */
} cbm_dma_port_irq_ctrl_t;
/*!
	\brief CBM Dequeue CPU Port Interrupt Enable data structure
*/
typedef union cbm_dequeue_cpu_port_irq_enable{
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_dequeue_cpu_port_irq_enable_t;
/*!
	\brief CBM Enqueue CPU Port Interrupt Enable data structure
*/
typedef union cbm_enqueue_cpu_port_irq_enable {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_enqueue_cpu_port_irq_enable_t;
/*!
	\brief CBM CPU Port Interrupt Enable
*/
typedef union {
	uint32_t	regval;	/*!< 32-bit register value */
	cbm_enqueue_cpu_port_irq_enable_t enq_irq_enable; /*!< CBM Enqueue CPU port IRQ Enable */
	cbm_dequeue_cpu_port_irq_enable_t deq_irq_enable; /*!< CBM Dequeue CPU port IRQ Enable */
} cbm_cpu_port_irq_enable_t;

/*!
	\brief CBM Dequeue DMA Port Interrupt Enable data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_dequeue_dma_port_irq_enable_t;
/*!
	\brief CBM Enqueue DMA Port Interrupt Enable data structure
*/
typedef union {
	/* Should be auto-generated by LSD tool */
	uint32_t	regval;	/*!< 32-bit register value */
} cbm_enqueue_dma_port_irq_enable_t;

/*!
	\brief CBM DMA Port Interrupt Enable
*/
typedef union {
	uint32_t	regval;	/*!< 32-bit register value */
	cbm_enqueue_dma_port_irq_enable_t enq_irq_enable; /*!< CBM Enqueue DMA port IRQ Enable */
	cbm_dequeue_dma_port_irq_enable_t deq_irq_enable; /*!< CBM Dequeue DMA port IRQ Enable */
} cbm_dma_port_irq_enable_t;


/*!
	\brief CBM Queue Map table Entry
*/
typedef union {
	uint32_t	regval;	/*!< 32-bit register value */
	struct {
		uint32_t	flowid:2; /*!< FlowId (Bits 7:6) */
		uint32_t	dec:1; /*!< VPN Decrypt flag */
		uint32_t	enc:1; /*!< VPN Encrypt flag */
		uint32_t	mpe1:1; /*!< MPE1 Flag */
		uint32_t	mpe2:1; /*!< MPE2 Flag */
		uint32_t	ep:4; /*!< PMAC Egress Port number */
		uint32_t	tc:4; /*!< Traffic Class */
		uint32_t	sub_if_id:12;/*sub if id*/
		uint32_t	resv:6;/*resv*/
	};
	uint32_t	flags; /*!< Flags stored in DDR CBM Queue map table. Currently, only flag defined is CBM_F_QUEUE_DROP */
} cbm_queue_map_entry_t;

/*!
	\brief CBM Load Spreader Config
*/
typedef struct {
	uint32_t	enable:1; /*!< Whether Load Spreader is enabled */
	uint32_t	clk_div:2; /*!< Divide CBM clock by 2^clk_div for Overflow counter */
	uint32_t	flowid_mode:1; /*!< 1-FlowId mode; 0-WRR mode */
	uint32_t	wp0:2; /*!< Weight of Port 0*/
	uint32_t	wp1:2; /*!< Weight of Port 1*/
	uint32_t	wp2:2; /*!< Weight of Port 2*/
	uint32_t	wp3:2; /*!< Weight of Port 3*/
	uint32_t	wp4:2; /*!< Weight of Port 4*/
	uint32_t	wp5:2; /*!< Weight of Port 5*/
	uint32_t	wp6:2; /*!< Weight of Port 6*/
	uint32_t	wp7:2; /*!< Weight of Port 7*/
	uint32_t	res1:20; /*!< Reserved*/
} cbm_load_spreader_cfg_t;
/*!
	\brief CBM Port Load Spreader Config
*/
typedef struct {
	uint32_t	enable:1; /*!< Whether port is enabled on Load Spreader */
	uint32_t	spreading_enable:1; /*!< 1-Port will participate in load spreading; 0-direct connect to corresponding VPE number */
	uint32_t	irq_en:1; /*!< Rx Interrupt enabled for Port */
	uint32_t	counter_en:1; /*!< Counter enabled for Port */
	uint32_t	queue_thresh:3; /*!< Queue threshold to raise an interrupt */
	uint32_t	counter_thresh:16; /*!< FIXME: Queue threshold to raise an interrupt */
	uint32_t	res1:9; /*!<  REserved */
} cbm_port_load_spreader_cfg_t;

/*!
	\brief TMU resource structure for a TMU port
*/
typedef struct cbm_tmu_res {
	uint32_t tmu_port; /*!< TMU port number, -1 if no TMU port allocated */
	uint32_t cbm_deq_port; /*!< CBM Dequeue port number, -1 if no CBM port allocated */
	int32_t	tmu_sched; /*!< TMU scheduler Id attached to TMU port, -1 if unassigned */
	int32_t	tmu_q; /*!< TMU Queue Id attached to TMU scheduler on this TMU port, -1 if unassigned */
} cbm_tmu_res_t;



/*!
	\brief CBM Dequeue Resource Info for configuring other HW peripherals to
	access the CBM
*/
typedef struct cbm_dq_info {
	uint32_t	port_no;	/*! < CBM Dequeue Port No */
	void		*cbm_dq_port_base; /*!< CBM Dequeue Port Base Address
						 */
	int32_t		dma_tx_chan;	/*! PMAC DMA Tx Channel */
	uint32_t	num_desc; /*!< Number of Descriptors at port base */
	u32 num_free_burst; /*!< Number of free burst size */
	u32 *pkt_base; /*!<packet list base DMA virtual address */
	void *pkt_base_paddr;	/* packet list base DMA physical address */
	size_t dma_size;	/* size of allocated DMA memory */
	int prefill_pkt_num; /* number of pre-fill packet buffer requested */
} cbm_dq_info_t;

/*!
	\brief CBM Enqueue Resource Info for configuring other HW peripherals to
	access the CBM
*/
typedef struct cbm_eq_info {
	uint32_t	port_no;	/*! < CBM Enqueue Port No */
	void		*cbm_eq_port_base; /*!< CBM enqueue Port Base Address
						 */
	int32_t		dma_rx_chan_std;	/*! PMAC DMA Rx Channel for Std Buffer */
	int32_t		dma_rx_chan_jumbo;	/*! PMAC DMA Rx Channel for Jumbo Buffer */
	uint32_t	num_desc; /*!< Number of Descriptors at port base */
} cbm_eq_info_t;
/*!
	\brief CBM Dequeue Port Resources for a Client - All addrsses are PHYSICAL addresses
*/
typedef struct cbm_dq_port_res {
	void		*cbm_buf_free_base; /*!< CBM Buffer Free Port Base
						  Address for CPU Ports. This is NULL for DMA
						  Dequeue Ports */
	uint32_t	num_free_entries; /*!< Number of Free Port entries. This is 0
						when cbm_buf_free_base = NULL */
	int		num_deq_ports; /*!< Number of Dequeue Ports */
	cbm_dq_info_t	*deq_info; /*!< Array of CBM Dequeue Port Related Info
					 of num_deq_ports entries. Caller must free the memory */
} cbm_dq_port_res_t;

struct cbm_tx_data {
	int dp_inst; /*input*/
	int cbm_inst; /*input*/
	u8 *pmac; /*NULL: no pmac,
	otherwise with pmac */
	int pmac_len; /*size of the pmac */
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
	u32 txpush_addr_qos; /* QOS push address after shift or mask from QOS HW point of view */
	u32 txpush_addr;  /*port ring address. should follow HW defintion*/
	u32 tx_ring_size; /*ring size */
	u32 tx_ring_offset;  /*next tx_ring_addr = current tx_ring_addr + tx_ring_offset */
	void *tx_ring_addr_txpush; /* port ring physical base address/TXIN address  
				    *  to configured to QOS TX Push register.
	                            *  In PRX300, it needs to left
				    *  shift some bits.
				    *  In LGM,  ???
				    */

	u32 num_dma_chan;
};


struct cbm_dp_en_data {
	int dp_inst; /*input*/
	int cbm_inst; /*input*/
	u32 deq_port; /*input: -1 means not valid*/
	u32 dma_chnl_init;/*input :0 - no init, 1:- init DMA channel*/
	u32 num_dma_chan;
	int tx_ring_size;  /*!< [in] ACA TXIN Ring size.
			      *   if 0, then change, otherwise try to tune
			      *   down the pre-allocated TXIN ring buffer size.
			      *   Only allowed to tune down.
			      */
};

struct cbm_tx_push {
	s32 deq_port;
	u32 tx_pkt_credit;  /*port tx packet credit */
	u32 tx_b_credit;  /*port tx bytes credit */
	u32 txpush_addr_qos; /* QOS push address after shift or mask from QOS HW point of view */
	u32 txpush_addr;  /*port ring address. should follow HW defintion*/
	u32 tx_ring_size; /*ring size */
	u32 tx_ring_offset;  /*next tx_ring_addr = current tx_ring_addr + tx_ring_offset */
	u32 type; /* output */
};

struct cbm_cpu_port_data {
	u8 dp_inst; /*input*/
	u8 cbm_inst; /*input*/
	/*drop port*/
	struct cbm_tx_push dq_tx_flush_info;
	/*output: deq_port = -1 means not valid*/
	struct cbm_tx_push dq_tx_push_info[CQM_MAX_CPU];
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

/*!
	\brief CBM Enqueue Port Resources for a Client
*/
typedef struct cbm_eq_port_res {
	int		num_eq_ports; /*!< Number of Enqueue Ports */
	cbm_eq_info_t	*eq_info; /*!<	EQ port info */
} cbm_eq_port_res_t;
/* @} */

/** \addtogroup CBM_Control_API */
/* @{ */
/*! \brief	Get CBM Enqueue Manager Control
	\param[out] ctrl CBM EQM Control structure
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_enqueue_mgr_ctrl_get(
	cbm_eqm_ctrl_t *ctrl, /*!< EQM Control data structure */
	uint32_t flags /*!< Flags */
	);
/*! \brief	Set CBM Enqueue Manager Control
	\param[in] ctrl CBM EQM Control structure
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_enqueue_mgr_ctrl_set(
	  cbm_eqm_ctrl_t *ctrl, /*!< EQM Control data structure */
	  uint32_t flags /*!< Flags */
	  );
/*! \brief	Get CBM Dequeue Manager Control
	\param[out] ctrl CBM EQM Control structure
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_dequeue_mgr_ctrl_get(
	cbm_dqm_ctrl_t *ctrl, /*!< EQM Control data structure */
	uint32_t flags /*!< Flags */
	);
/*! \brief	Configures CBM Dequeue Manager Control
	\param[in] ctrl CBM DQM Control structure
	\param[in] flags : Reserved
	\return CBM_OK  / CBM_ERROR
	\note
*/
int32_t
cbm_dequeue_mgr_ctrl_set(
	cbm_dqm_ctrl_t *ctrl, /*!< EQM Control data structure */
	uint32_t flags /*!< Flags */
	);
/* @} */

/** \addtogroup CBM_Port_Allocation_API */
/* @{ */

/*! \brief Get the number of allocated TMU ports, and allocated TMU queues from
  the CBM Driver.
	\param[out] tmu_port Highest TMU port number allocated/reserved by CBM Driver
	\param[out] tmu_sched Highest TMU scheduler number allocated/reserved by CBM Driver
	\param[out] tmu_q Highest TMU queue number allocated/reserved by CBM Driver
	\return 0
*/
int32_t
cbm_reserved_dp_resources_get(
	uint32_t *tmu_port,
	uint32_t *tmu_sched,
	uint32_t *tmu_q
	);

/*! \brief Get the TMU resoures for a datapath port
	\param[in] dp_port Datapath (PMAC) Port for which TMU resources are queried
	\param[out] num_tmu_ports Number of TMU ports configured for this DP port - typically 1
	\param[out] res TMU resources assigned to this DP port
	\param[in]  flags - One of the following flags can be passed:
	- DP_F_DIRECTPATH_RX
	- DP_F_MPE_ACCEL
	\note For LAN Ethernet ports, two TMU ports are allocated for each LAN port
	\note Caller needs to free the memory  allocated for res
	\note In the CBM Egress port lookup table, special ports are to be identified
	by the same flags as passed in this API (additionally DP_F_FAST_WLAN2,
	DP_F_FAST_WLAN3)
	\return 0 if DP Port configured; -1 if DP Port does not exist
s
*/
int32_t
cbm_dp_port_resources_get(
	uint32_t *dp_port,
	uint32_t *num_tmu_ports,
	cbm_tmu_res_t **res_pp,
	uint32_t  flags
	);


/*! \brief Get the Datapath Port number for a TMU port
	\param[in] tmu_port TMU port number for which datapath port number is requested
	\param[in]  flags - One of the following flags can be passed:
	- DP_F_DIRECTPATH_RX
	- DP_F_MPE_ACCEL
  \return DP Port if success; -1 if DP Port does not exist
*/
int32_t
cbm_dp_port_get(
	uint32_t tmu_port,
	uint32_t flags
	);

s32 cbm_cpu_port_get(
	struct cbm_cpu_port_data *data,
	u32 flags
);


/*! \brief Enable/Disable the CBM datapath for the given datapath port (PMAC port)
	\param[in] owner	 Kernel module pointer which owns the port
	\param[in] port_id PMAC port number allocated for the datapath port
	\param[in] flags : Use flags to disable
	-  CBM_F_DPATH_DISABLE : Disable the datapath for the PMAC port
	\return 0 if OK / CBM_ERROR on Error
	\note This involves disabling the TMU queues mapped to the TMU port
	corresponding to the given datapath port. See CBM chapter in SWAS
*/
s32 cbm_dp_enable(
	struct module *owner,
	u32 dp_port,
	struct cbm_dp_en_data *data,
	u32 flags,
	u32 alloc_flags
);

/*! \brief	Allocates CBM enqueue port number to a physical interface. The physical
	port may map to an exclusive netdevice like in the case of ethernet LAN ports.
	\param[in] owner	 Kernel module pointer which owns the port
	\param[in] driver_dev_port Physical Port Number of this device managed by the driver - some drivers manage more than 1 port.
	\param[in] flags : Special inputs to alloc routine - CBM_WAVE500_WLAN_DEV, CBM_VRX300_DEV
	\return Port Id	/ CBM_ERROR
	\note
*/
int32_t
cbm_enqueue_port_alloc(
	struct module *owner,
	uint32_t driver_dev_port,
	uint32_t flags
	);

/*! \brief	Deallocate CBM enqueue port number allocated to a physical interface. The physical
	port may map to an exclusive netdevice like in the case of ethernet LAN ports.
	\param[in] owner	 Kernel module pointer which owns the port
	\param[in] cbm_port_id CBM Port number to deallocate.
	\param[in] flags : Whether the CBM Port is
	\return	CBM_OK	/ CBM_ERROR
	\note CBM Port can be deallocated only after the port is Unitialized
*/
int32_t
cbm_enqueue_port_dealloc(
	struct module *owner,
	uint32_t cbm_port_id,
	uint32_t flags
	);

/*! \brief	Allocates CBM enqueue port number to a physical interface. The physical
	port may map to an exclusive netdevice like in the case of ethernet LAN ports.
	In other cases like WLAN, the physical port is a Radio port, while netdevices are
	Virtual Access Points (VAPs). In this case, the	 AP netdevice can be passed. Alternately,
\param[in] owner	 Kernel module pointer which owns the port
	\param[in] driver_dev_port Physical Port Number of this device managed by the driver
	\param[in] flags : Special inputs to alloc routine - CBM_WAVE500_WLAN_DEV, CBM_VRX300_DEV
	\return Port Id	/ CBM_ERROR
	\note  Internal API
*/
int32_t
cbm_dequeue_port_alloc(
	struct module *owner,
	uint32_t driver_dev_port,
	uint32_t flags
	);

/*! \brief	Deallocate CBM dequeue port number allocated to a physical interface. The physical
	port may map to an exclusive netdevice like in the case of ethernet LAN ports.
	\param[in] owner	 Kernel module pointer which owns the port
	\param[in] cbm_port_id CBM Port number to deallocate.
	\param[in] flags : Reserved
	\return	CBM_OK	/ CBM_ERROR
	\note CBM Port can be deallocated only after the port is Unitialized
	\note Internal API
*/
int32_t
cbm_dequeue_port_dealloc(
	struct module *owner,
	uint32_t cbm_port_id,
	uint32_t flags
	);

/*! \brief	Allocates datapath PMAC port number to a physical interface. The physical
	port may map to an exclusive netdevice like in the case of ethernet LAN ports.
	In other cases like WLAN, the physical port is a Radio port, while netdevices are
	Virtual Access Points (VAPs). In this case, the	 AP netdevice can be passed. Alternately,
	\param[in] owner	 Kernel module pointer which owns the port
	\param[in] dev Pointer to Linux netdevice structure
	\param[in] dev_port Physical Port Number of this device managed by the driver
	\param[in] port_id Optional port_id number requested. Usually, 0 and allocated by driver
	\param[in] flags : Special inputs to alloc routine - CBM_WAVE500_WLAN_DEV, CBM_VRX300_DEV
	\return Port Id	/ CBM_ERROR
	\note
	\note DT map
	\note	DrvMod Name | DrvDevNo | PMACPort No
	\note	laneth		|	0	   | 1
	\note	laneth		|	1	   | 2
	\note	waneth		|	0	   | 15

*/
s32 cbm_dp_port_alloc(
	struct module *owner,
	struct net_device *dev,
	u32 dev_port,
	s32 dp_port,
	struct cbm_dp_alloc_data *data,
	u32 flags
);


struct cbm_dp_alloc_complete_data {
	u8 num_rx_ring;   /*!< [in] number of rx ring from DC device to Host.
			    *   num_rx_ring requirement:
			    *   @num_rings <= @DP_RX_RING_NUM
			    *   GRX350/Falcon_MX:1 rx ring
			    *   LGM: up to 2 rx ring, like Docsis can use 2 rings
			    *   For two ring case:
			    *    1st rxout ring without qos
			    *    2nd rxout ring with qos
			    */
	u8 num_tx_ring;   /*!< [in] number of tx ring from Host to DC device
			    *   num_rx_ring requirement:
			    *   @num_rings <= @DP_TX_RING_NUM
			    *   Normally it is 1 TX ring only.
			    *   But for 5G, it can support up to 8 TX ring
			    *   For docsis, alhtough it is 16 dequeue port to WIB.
			    *   But the final ring only 1, ie, WIB to Dcosis
			    */
	u8 num_umt_port;   /*!< [in] number of UMT port.
			     *    Normally is 1 only. But Docsis can use up to 2
			     */
	struct dp_rx_ring rx_ring[DP_RX_RING_NUM]; /*!< [in/out] DC rx ring info
						    */
	struct dp_tx_ring tx_ring[DP_TX_RING_NUM]; /*!< [in/out] DC tx ring info
						    */

#if !IS_ENABLED(CONFIG_GRX500_CBM) /*GRX500 GSWIP30*/
	struct dp_umt_port umt[DP_MAX_UMT]; /*!< [in/out] DC umt information */
#endif
	u32 enable_cqm_meta : 1; /*!< enable CQM buffer meta data marking */
	int alloc_flags; /*!< original alloc flags used in the dp_alloc_port */
	u32 deq_port;	/* [in] port id which was returned in the alloc */
	u32 qid_base; /* [in] EPON baseport */
	u32 num_qid; /* [in] EPON num qid */
};

s32 cbm_dp_port_alloc_complete(
	struct module *owner,
	struct net_device *dev,
	u32 dev_port,
	s32 dp_port,
	struct cbm_dp_alloc_complete_data *data,
	u32 flags
);

/*! \brief	De-Allocates datapath PMAC port number to a physical interface.
	Virtual Access Points (VAPs). In this case, the	 AP netdevice can be passed. Alternately,
	\param[in] owner	 Kernel module pointer which owns the port
	\param[in] dev_port Physical Port Number of this device managed by the driver
	\param[in] cbm_port_id CBM port_id number to de-allocate.
	\param[in] flags : Reserved
	\return Port Id	/ CBM_ERROR
	\note  Will be called by the datapath driver when port/interface is deleted
	like when driver is rmmod
*/
s32 cbm_dp_port_dealloc(
	struct module *owner,
	u32 dev_port,
	s32 cbm_port_id,
	struct cbm_dp_alloc_data *data,
	u32 flags
);
/*! \brief	Get the CBM CPU Dequeue Path resources for a DP port
	\param[in] dp_port  Datapath/PMAC port number for which Dequeue resources are asked
	\param[out] res	 CBM Resources on Dequeue Path in terms of address, number etc
	\param[in] flags :Special Flags can be:
	DP_F_DIRECTPATH_RX
	DP_F_MPE_ACCEL
	DP_F_LRO
	\return CBM_OK / CBM_ERROR
	\note  The Device Tree table for Dequeue Ports, will have two extra parameters indicating
	a) CPU Dequeue Port (1) or DMA Dequeue Port (0)
	b) Number of Descriptors on that port
	This info is going to be used in the API below. Sanity checks on the DT info is desirable
	at init time.
*/
int32_t
cbm_dequeue_port_resources_get(
	uint32_t	dp_port,
	cbm_dq_port_res_t *res,
	uint32_t flags
	);

/*! \brief	Get the CBM CPU Enqueue Path resources for a DP port
	\param[out] res	 CBM Resources on Dequeue Path in terms of address, number etc
	\param[in] flags :Special Flags can be:
	DP_F_ENQ_PAE
	DP_F_ENQ_VRX318
	DP_F_ENQ_GSWIPL
	DP_F_ENQ_TSO
	\return CBM_OK / CBM_ERROR
	\note  The Device Tree table for Enqueue Ports, will have two extra parameters indicating
	a) CPU Enqueue Port (1) or DMA Enqueue Port (0)
	b) Number of Descriptors on that port
	This info is going to be used in the API below. Sanity checks on the DT info is desirable
	at init time.
*/
int32_t
cbm_enqueue_port_resources_get(
	cbm_eq_port_res_t *res,
	uint32_t flags
	);
/* @} */

/** \addtogroup CBM_Port_OPS_API */
/* @{ */

/*! \brief	Initializes CBM Enqueue DMA Port
	\param[in] cbm_port_id CBM port_id to initialize.
	\param[in] dma_hw_num Instance of DMA Engine H/w
	\param[in] std_dma_chan_num DMA Rx Channel Number for Standard size buffers on the specified DMA Engine
	\param[in] jumbo_dma_chan_num DMA Rx Channel Number for Jumbo size buffers on the specified DMA Engine
	\param[in] flags : Flags for Port Init routine
		- CBM_PORT_F_STANDARD_BUF
		- CBM_PORT_F_JUMBO_BUF
	\return CBM_OK / CBM_ERROR
	\note  One or both of CBM_PORT_F_STANDARD_BUF and CBM_PORT_F_JUMBO_BUF  must be specified
*/
int32_t
cbm_enqueue_dma_port_init(
	int32_t cbm_port_id,
	int32_t dma_hw_num,
	uint32_t std_dma_chan_num,
	uint32_t jumbo_dma_chan_num,
	uint32_t flags
	);
/*! \brief	Uninitialize a initialized CBM Enqueue DMA port, which dissociates all DMA
	setup with the port.
	\param[in] cbm_port_id CBM port_id to un-initialize.
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_enqueue_dma_port_uninit(
	int32_t cbm_port_id,
	uint32_t flags
	);

/*! \brief	Initializes CBM Dequeue DMA Port
	\param[in] cbm_port_id CBM port_id to initialize.
	\param[in] dma_hw_num Instance of DMA Engine H/w
	\param[in] dma_chan_num DMA Tx Channel Number on the specified DMA Engine
	\param[in] flags : Flags for Port Init routine
	\return CBM_OK / CBM_ERROR
	\note  On the Dequeue port, the same DMA channel can carry Standard or Jumbo sized buffers
*/
int32_t
cbm_dequeue_dma_port_init(
	int32_t cbm_port_id,
	int32_t dma_hw_num,
	uint32_t dma_chan_num,
	uint32_t flags
	);
/*! \brief	Uninitialize a initialized CBM dequeue DMA port, which dissociates all DMA
	setup with the port.
	\param[in] cbm_port_id CBM port_id to un-initialize.
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_dequeue_dma_port_uninit(
	int32_t cbm_port_id,
	uint32_t flags
	);

/*! \brief	Enable CBM Enqueue or Dequeue DMA Port
	\param[in] cbm_port_id CBM port_id to initialize.
	\param[in] flags : Flags for Port Enable routine
		- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
		- CBM_PORT_F_DISABLE - if set, disable the port; else enable the port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_dma_port_enable(
	int32_t cbm_port_id,
	uint32_t flags
	);

/*! \brief	Get the CBM DMA port Config parameters
	\param[in] cbm_port_id CBM port_id to un-initialize.
	\param[out] cfg CBM DMA Port Configuration datastruct
	\param[in] flags : Flags that qualify the configuration
		- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_dma_port_config_get(
	int32_t cbm_port_id,
	cbm_dma_port_config_t *cfg,
	uint32_t flags
	);

/*! \brief	Configure the CBM DMA port parameters
	\param[in] cbm_port_id CBM port_id to un-initialize.
	\param[in] cfg CBM DMA Port Configuration datastruct
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_dma_port_config_set(
	int32_t cbm_port_id,
	cbm_dma_port_config_t *cfg,
	uint32_t flags
	);

/*! \brief	Get the CBM port watermark thresholds
	\param[in] cbm_port_id CBM port_id to get thresholds.
	\param[out] thresh CBM Enqueue Port Watermark thresholds
	\param[in] flags :
		- CBM_PORT_F_CPU_PORT
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_enqueue_port_thresh_get(
	int32_t cbm_port_id,
	cbm_port_thresh_t *thresh,
	uint32_t flags
	);

/*! \brief	Set the CBM port watermark thresholds
	\param[in] cbm_port_id CBM port_id to get thresholds.
	\param[in] thresh CBM Enqueue Port Watermark thresholds
	\param[in] flags :
		- CBM_PORT_F_CPU_PORT
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_enqueue_port_thresh_set(
	int32_t cbm_port_id,
	cbm_port_thresh_t *thresh,
	uint32_t flags
	);

/*! \brief	Enable/Disable a Datapath Port TMU Queue in CBM
   \param[in] dp_port_id Datapath/PMAC port number whose queue to operate on.
		if dp_port_id=-1, the operation is carried out only on qnum or based
		on special queue flags like CBM_Q_F_CKSUM.
	\param[in] qnum : Queue number to enable/disable. If qnum=-1, then special flags
		implicitly determine the queue number.
	\param[in] flags : Flags for Port Enable routine
			- CBM_Q_F_DISABLE - Disable the Queue
			- CBM_Q_F_NODEQUEUE - if set, Don't drain out all
			the enqueued packets before disabling the queue which
			is the default behaviour when disabling a queue
			- CBM_Q_F_CKSUM : Special Queue for checksum
	\return CBM_OK / CBM_ERROR
	\note  CBM Driver allows a safe enable/disable of the queue, by first
	mapping all CBM Queue Map entries to this queue to the Global Drop queue,
	draining all packets out if required, enabling/disabling the queue, and
	mapping the previous CBM Queue Map entries back to this queue
*/
int32_t
cbm_dp_q_enable(
	int cbm_inst,
	int32_t dp_port_id,
	int32_t qnum,
	int32_t tmu_port_id,
	int32_t remap_to_qid,
	uint32_t timeout,
	int32_t qidt_valid,
	uint32_t flags
	);
/* @} */

/** \addtogroup CBM_Stats_API */
/* @{ */
/*! \brief	Get the CBM Enqueue DMA Port Stats
	\param[in] cbm_port_id CBM port_id for which to get stats
	\param[out] occupancy_ctr	Enqueue Port Occupancy Counter
	\param[out] enq_ctr	Enqueue Port Enqueue Counter
	\param[in] flags : Flags that qualify the configuration
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_enqueue_dma_port_stats_get(
	int32_t cbm_port_id,
	uint32_t *occupancy_ctr,
	uint32_t *enq_ctr,
	uint32_t flags
	);

/*! \brief	Get the CBM Dequeue DMA Port Stats
	\param[in] cbm_port_id CBM port_id for which to get stats
	\param[out] deq_ctr	Dequeue Port Dequeue Counter
	\param[in] flags : Flags that qualify the configuration
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_dequeue_dma_port_stats_get(
	int32_t cbm_port_id,
	uint32_t *deq_ctr,
	uint32_t flags
	);

/*! \brief	Get the CBM DMA Queue Stats
	\param[in] cbm_queue_id CBM/TMU Queue Id [0-255] for which stats are to be returned
	\param[out] ctr	Queue Pkt/Byte counter as per Queue Count config
	\param[in] flags : Flags that qualify the configuration
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_queue_stats_get(
	  int32_t cbm_queue_id,
	  uint32_t *ctr,
	  uint32_t flags
	  );
/* @} */

/** \addtogroup CBM_CPU_Port_API */
/* @{ */
/*! \brief	Get the CBM CPU port Config parameters
	\param[in] cbm_port_id CBM port_id to un-initialize.
	\param[out] cfg CBM CPU Port Configuration datastruct
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_cpu_port_config_get(
	int32_t cbm_port_id,
	cbm_cpu_port_config_t *cfg,
	uint32_t flags
	);
/*! \brief	Set the CBM CPU port Config parameters
	\param[in] cbm_port_id CBM port_id to un-initialize.
	\param[in] cfg CBM CPU Port Configuration datastruct
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_cpu_port_config_set(
	int32_t cbm_port_id,
	cbm_cpu_port_config_t *cfg,
	uint32_t flags
	);
/* @} */

/** \addtogroup CBM_CPU_API */
/* @{ */
/*! \brief	Request a CBM Buffer of the indicated Buffer Size Pool/Bin
	\param[in] vpe_id  CPU/VPE# (0-3) which requests the buffer
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_DEQUEUE_PORT - if set, Get Standard Pool Buffers
			- CBM_PORT_F_JUMBO_BUF - if set, Get Jumbo Pool Buffers
	\return CBM Buffer Pointer / NULL if out of buffers
	\note
*/
void *cbm_buffer_alloc(u32 pid, u32 flag, u32 size);
void *cqm_buffer_alloc_by_policy(u32 pid, u32 flag, u32 policy);

struct cqm_bm_free {
	int cbm_inst;
	u32 flag;
	void *buf;   /* Physical address */
	u32 policy_base; /* base policy */
	int policy_num;  /* number of policy */
};

int cqm_buffer_free_by_policy(struct cqm_bm_free *free_info);

/*! \brief	Free a CBM Buffer to one of the CBM Free Buffer Pools
	\param[in] vpe_id  CPU/VPE# (0-3) which requests the buffer ; if -1, current VPE Id is used
	\param[in] buf  Pointer to CBM Buffer
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_DEQUEUE_PORT - if set, Get Standard Pool Buffers
			- CBM_PORT_F_JUMBO_BUF - if set, Get Jumbo Pool Buffers
	\return CBM_OK / CBM_ERROR if Buffer pointer is invalid
	\note Pointer to CBM buffer need not be at the very start of the buffer. It can point to anywhere inside the buffer space
*/
int cbm_buffer_free(u32 pid, void *v_buf, u32 flag);

/*! \brief	Tx a Packet Buffer to the Egress Port
	\param[in] skb  Pointer to Linux skb packet meta-data/buffer
	\param[in] pmac_port_id	Egress PMAC/Datapath Port Id
	\param[in] flags : Flags that qualify the configuration
	\return CBM_OK / CBM_ERROR if Buffer pointer is invalid
	\note Pointer to CBM buffer need not be at the very start of the buffer. It can point to anywhere inside the buffer space
*/
s32 cbm_cpu_pkt_tx(
	struct sk_buff *skb,
	struct cbm_tx_data *data,
	u32 flags
	);


/*! \brief	Rx Flow Control function
	\param[in] pmac_port_id Datapath/PMAC port Id on which Datapath/PMAC port Id on which datapath driver indicates backpressure
	\param[in] subif	 sub-interface Id on which backpressure is asserted (currently reserved)
	\param[in] flags : Flags that qualify the backpressure mechanism
			- CBM_CPU_F_RX_STOP - Rx Stop
			- CBM_CPU_F_RX_START - Rx Start
	\return CBM_OK / CBM_ERROR
	\note Use of subif is reserved for future
*/
int32_t
cbm_cpu_rx_flow_control(
	int32_t pmac_port_id,
	uint32_t subif,
	uint32_t flags
	);


/*! \brief	CBM Queue Map table entry/entries Set
	\param[in] tmu_queue_id TMU QueueId (0-255)
	\param[in] entry	 pointer to Queue map structure
	\param[in] flags : Flags that qualify the Mapping mechanism
			- CBM_QUEUE_MAP_F_FLOWID_DONTCARE
			- CBM_QUEUE_MAP_F_EN_DONTCARE
			- CBM_QUEUE_MAP_F_DE_DONTCARE
			- CBM_QUEUE_MAP_F_MPE1_DONTCARE
			- CBM_QUEUE_MAP_F_MPE2_DONTCARE
			- CBM_QUEUE_MAP_F_EP_DONTCARE
			- CBM_QUEUE_MAP_F_TC_DONTCARE
	\return CBM_OK / CBM_ERROR
	\note Map Entry fields which are specified as DONTCARE in the flags, result in multiple entries being created which map to the same tmu_queue_id.
	Also, VRX318 has TC=qid setting.
	\note The API implementation needs to maintain a DDR Queue Map table
	corresponding to the CBM HW QIDT with an extra flags field. Marking DROP flag
	in this field of an entry internally, means QIDT entry in CBM HW set to
	Global Drop Queue (instead of queue num in Queue Map table entry). This
	allows to easily remap back to the right queue after operations like disable
	etc.
	\note The implementation must also maintain an array of all queue numbers, which
	contains the reference count of how many Queue Map table entries point to this queue
	On every set, reference count of original queue is decremented, and new queue is
	incremented.
*/
int32_t
cbm_queue_map_set(
	int cbm_inst,
	int32_t queue_id,
	cbm_queue_map_entry_t *entry,
	uint32_t flags
	);

/*! \brief	CBM Queue Map table entry/entries get for a given tmu_queue_id
	\param[in] tmu_queue_id TMU QueueId (0-255)
	\param[out] num_entries returned in entries array which is allocated by CBM Driver
	\param[out] entries	pointer to Queue map structure
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note The caller needs to free up the entries buffer
	\note if tmu_queue_id = -1, then all queue map entries are returned, aka the whole CBM Queue Map table
*/
int32_t
cbm_queue_map_get(
	int cbm_inst,
	int32_t queue_id,
	int32_t *num_entries,
	cbm_queue_map_entry_t **entries,
	uint32_t flags
	);

/*! \brief	CBM Mode table entry/entries Set
	\param[in] entry	 pointer to mode structure (only ep, mpe1, mpe2, mode fields are setup)
	\param[in] flags : Flags that qualify the Mapping mechanism
			- CBM_QUEUE_MAP_F_MPE1_DONTCARE
			- CBM_QUEUE_MAP_F_MPE2_DONTCARE
			- CBM_QUEUE_MAP_F_EP_DONTCARE
	\return CBM_OK / CBM_ERROR
	\note Map Entry fields which are specified as DONTCARE in the flags, result in multiple entries being created which map to the same mode.
*/
s32
cqm_mode_table_set(
	int cbm_inst,
	cbm_queue_map_entry_t *entry,
	u32 mode,
	u32 flags
	);

/*! \brief	CBM Mode table entry/entries get for a given ep/mpe1/mpe2 flags
	\param[in] mode
	\param[out] num_entries returned in entries array which is allocated by CBM Driver
	\param[out] entries	pointer to Queue map structure where only the ep, mpe1, mpe2, mode fields are valid
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note The caller needs to free up the entries buffer
	\note if mode = -1, then all mode table entries are returned, aka the whole CBM mode table
*/

s32
cqm_mode_table_get(
	int cbm_inst,
	int *mode,
	cbm_queue_map_entry_t *entry,
	u32 flags
	);

s32 cqm_qid2ep_map_set(int qid, int port);
s32 cqm_qid2ep_map_get(int qid, int *port);


/*! \brief	CBM Dequeue Port Queues Flush in TMU
	\param[in] cbm_port_id CBM Dequeue Egress Port
	\param[in] tmu_queue_id TMU queue_id attached to cbm_port_id. If tmu_queue_id=-1, all queues attached to cbm_port_id need to be flushed - Currently only all queues attached to the port can be flushed
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note The caller needs to free up the entries buffer
*/
int32_t
cbm_queue_flush(
	int32_t cbm_port_id,
	int32_t tmu_queue_id,
	uint32_t timeout,
	uint32_t flags
	);

/*! \brief	CBM Queue Flush
	\param[in] cqm_inst CBM Instance
	\param[in] cqm_drop_port CBM Dequeue Egress Port
	\param[in] qid : Queue ID
	\return CBM_OK / CBM_ERROR
*/
int32_t cqm_qos_queue_flush(int32_t cqm_inst, int32_t cqm_drop_port, int32_t qid);

/*! \brief	CBM Buffer Update Repeat Count (RCNT)
	\param[in] vpe_id Use CBM CPU RCNTINC/DEC port corresponding to VPE/CPU Number
	\param[in] buffer Pointer to CBM buffer
	\param[in] flags : Whether to Increment or Decrement RCNT for buffer
	- CBM_CPU_BUF_INC_RCNT
	- CBM_CPU_BUF_DEC_RCNT
	\return RCNT / CBM_ERROR
	\note The buffer pointer passed can be anywhere in the span of the CBM buffer
*/
int32_t
cbm_buffer_upd_rcnt(
	int32_t vpe_id,
	void *buffer,
	uint32_t flags
	);
/* @} */

/** \addtogroup CBM_CPU_IRQ_API */
/* @{ */
/*! \brief	Set the CBM CPU IRQ Ctrl Config - s/w trigger interrupts for debug
	\param[in] cbm_cpu_port_id CBM CPU Port Id
	\param[in] cfg CBM CPU Port IRQ Control datastruct
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_WAVE500_PORT - if set, special CPU port that has additional parameters
			- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_cpu_port_intr_ctrl_set(
	uint32_t cbm_cpu_port_id,
	cbm_cpu_port_irq_ctrl_t *cfg,
	uint32_t flags
	);

/*! \brief	Set the CBM CPU IRQ Enable Config
	\param[in] cbm_cpu_port_id CBM CPU Port Id
	\param[in] cfg CBM CPU Port IRQ Enable datastruct
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_WAVE500_PORT - if set, special CPU port that has additional parameters
			- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_cpu_port_intr_enable_set(
	uint32_t cbm_cpu_port_id,
	cbm_cpu_port_irq_enable_t *cfg,
	uint32_t flags
	);
/*! \brief	Get the CBM CPU IRQ Enable Config
	\param[in] cbm_cpu_port_id CBM CPU Port Id
	\param[out] cfg CBM CPU Port IRQ Enable datastruct
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_WAVE500_PORT - if set, special CPU port that has additional parameters
			- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_cpu_port_intr_ctrl_get(
	uint32_t cbm_cpu_port_id,
	cbm_cpu_port_irq_enable_t *cfg,
	uint32_t flags
	);
/*! \brief	Set the CBM DMA IRQ Ctrl Config - s/w trigger interrupts for debug
	\param[in] cbm_dma_port_id CBM DMA Port Id
	\param[in] cfg CBM DMA Port IRQ Control datastruct
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_WAVE500_PORT - if set, special DMA port that has additional parameters
			- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_dma_port_intr_ctrl_set(
	  uint32_t cbm_dma_port_id,
	  cbm_dma_port_irq_ctrl_t *cfg,
	  uint32_t flags
	  );

/*! \brief	Set the CBM DMA IRQ Enable Config
	\param[in] cbm_dma_port_id CBM DMA Port Id
	\param[in] cfg CBM DMA Port IRQ Enable datastruct
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_WAVE500_PORT - if set, special DMA port that has additional parameters
			- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_dma_port_intr_enable_set(
	uint32_t cbm_dma_port_id,
	cbm_dma_port_irq_enable_t *cfg,
	uint32_t flags
	);
/*! \brief	Get the CBM DMA IRQ Enable Config
	\param[in] cbm_dma_port_id CBM DMA Port Id
	\param[out] cfg CBM DMA Port IRQ Enable datastruct
	\param[in] flags : Flags that qualify the configuration
			- CBM_PORT_F_WAVE500_PORT - if set, special DMA port that has additional parameters
			- CBM_PORT_F_DEQUEUE_PORT - if set, Dequeue port; else Enqueue port
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_dma_port_intr_ctrl_get(
	uint32_t cbm_dma_port_id,
	cbm_dma_port_irq_enable_t *cfg,
	uint32_t flags
	);
/* @} */
/** \addtogroup CBM_Stats_API */
/* @{ */
/*! \brief	Get the CBM Enqueue CPU Port Stats
	\param[in] cbm_port_id CBM port_id for which to get stats
	\param[out] occupancy_ctr	Enqueue Port Occupancy Counter
	\param[out] enq_ctr	Enqueue Port Enqueue Counter
	\param[in] flags : Flags that qualify the configuration
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_enqueue_cpu_port_stats_get(
	int32_t cbm_port_id,
	uint32_t *occupancy_ctr,
	uint32_t *enq_ctr,
	uint32_t flags
	);

/*! \brief	Get the CBM Dequeue CPU Port Stats
	\param[in] cbm_port_id CBM port_id for which to get stats
	\param[out] deq_ctr	Dequeue Port Dequeue Counter
	\param[in] flags : Flags that qualify the configuration
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_dequeue_cpu_port_stats_get(
	int32_t cbm_port_id,
	uint32_t *deq_ctr,
	uint32_t flags
	);

/* @} */

/** \addtogroup CBM_CPU_LOAD_SPREADER_API */
/* @{ */
/*! \brief	Get the CBM Global Load Spreader Configuration
	\param[out] cfg	CBM DQM Load Spreader Global Config
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_load_spreader_cfg_get(
	cbm_load_spreader_cfg_t *cfg,
	uint32_t flags
	);
/*! \brief	Set the CBM CPU Dequeue Port Load Spreader Configuration
	\param[in] cfg  CBM DQM Load Spreader Port Config
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_load_spreader_cfg_set(
	cbm_load_spreader_cfg_t *cfg,
	uint32_t flags
	);
/*! \brief	Set the CBM Global Load Spreader Configuration
	\param[in] cfg  CBM DQM Load Spreader Global Config
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_port_load_spreader_cfg_set(
	uint32_t port_id,
	cbm_port_load_spreader_cfg_t *cfg,
	uint32_t flags
	);
/*! \brief	Get the CBM CPU Dequeue Port Load Spreader Configuration
	\param[out] cfg	CBM DQM Load Spreader Port Config
	\param[in] flags : Reserved
	\return CBM_OK / CBM_ERROR
	\note
*/
int32_t
cbm_port_load_spreader_cfg_get(
	uint32_t port_id,
	cbm_port_load_spreader_cfg_t *cfg,
	uint32_t flags
	);

/*! \brief	Build an skb using cbm buffer
	\param[out] data	 Pointer to cbm data buffer
	\param[in] frag_size : Reserved
	\return Pointer to sk_buff allocated / NULL on error
	\note
*/
struct sk_buff *cbm_build_skb(
	void *data,
	unsigned int frag_size,
	gfp_t priority
	);


/*! \brief	Copy an skb using cbm buffer to another skb using cbm buffer
	\param[in] size	Size of data buffer requested
	\param[in] priority : Priority of the allocation - GFP_ATOMIC, GFP_KERNEL etc
	\return Pointer to sk_buff allocated / NULL on error
	\note
*/
struct sk_buff *cbm_copy_skb(
	const struct sk_buff *skb,
	gfp_t gfp_mask
	);

/*! \brief	Allocate an skb using cbm buffer
	\param[in] size	Size of data buffer requested
	\param[in] priority : Priority of the allocation - GFP_ATOMIC, GFP_KERNEL etc
	\return Pointer to sk_buff allocated / NULL on error
	\note
*/
struct sk_buff *cbm_alloc_skb(
	unsigned int size,
	gfp_t priority
	);

/*! \brief	Linearize an skb using cbm buffer so that pkt can tx to CBM
	\param[in] size	Size of data buffer requested
	\return Pointer to sk_buff allocated / NULL on error
	\note The function will check if there is any need to linearize the input skb, if not
	it will return the input skb pointer
*/
struct sk_buff *cbm_linearize_buf(
	struct sk_buff *
	);
/* @} */

struct cbm_desc{
    uint32_t desc0;
    uint32_t desc1;
    uint32_t desc2;
    uint32_t desc3;
};

struct sk_buff *
cbm_alloc_skb_by_size (uint32_t size, gfp_t priority);
int32_t
cbm_enqueue_port_overhead_set(
	int32_t port_id,
	int8_t ovh
	);
int32_t
cbm_enqueue_port_overhead_get(
	int32_t port_id,
	int8_t *ovh
	);

/* flags : STD Buffer or Jumbo buffer using existing defines
Caller must have checked the parameter validity etc before calling this API	 */
int cbm_cpu_enqueue_hw(uint32_t pid, struct cbm_desc *desc, void *data_pointer,	 int flags);
int cbm_get_wlan_umt_pid(u32 ep_id,  u32 *cbm_pid);
int32_t cbm_port_quick_reset(
int32_t cbm_port_id,
uint32_t flags);
int cbm_counter_mode_set(int enq, int mode);
int cbm_counter_mode_get(int enq, int *mode);
int cbm_q_thres_set(uint32_t length);
int cbm_q_thres_get(uint32_t *length);
int32_t cbm_igp_delay_set(
	int32_t cbm_port_id,
	int32_t delay
	);
int32_t
cbm_igp_delay_get(
	int32_t cbm_port_id,
	int32_t *delay
	);
int32_t cbm_port_quick_reset(int32_t cbm_port_id, uint32_t flags);

uint32_t cbm_get_dptr_scpu_egp_count (uint32_t cbm_port_id, uint32_t flags);
uint32_t cbm_get_std_free_count (void);
void set_lookup_qid_via_index(uint32_t index, uint32_t qid);
uint8_t get_lookup_qid_via_index(uint32_t index);
int check_ptr_validation(uint32_t buf);
void cbm_setup_DMA_p2p(void);
int cbm_turn_on_DMA_p2p (void);
void reset_enq_counter_all(void);
void reset_deq_counter_all(void);
void reset_enq_counter(uint32_t index);
void reset_deq_counter(uint32_t index);
int32_t cbm_get_egress_port_info(uint32_t cbm_port, uint32_t *tx_ch,
				 uint32_t *flags);
int cbm_buff_resv_bytes_get(int cbm_inst, int size);
s32 cbm_get_mtu_size(u32 *mtu_size);
/*
 * cbm_eqm_ctrl_t
 * cbm_dqm_ctrl_t
 * cbm_dma_port_config_t
 * cbm_cpu_port_config_t
 * cbm_cpu_port_irq_enable_t
 * cbm_queue_map_entry_t
 */
#endif
