#define LTQ_MAX_TSO_PORTS 4
#define MAX_NUMBER_OF_CMDS 100
#define LTQ_MAX_LRO_PORTS 8

#define 	USE_TIMER_FOR_SESSION_STOP
#define  	SG_BUFFER_PER_PORT	1
#define 	MAX_TSO_FRAGS_STATS	6

struct toe_sg_buffer {
	unsigned char* 	sgBuffer;
	struct sk_buff*	skb;
};

struct tso_frags_stats
{
	u64	TxBytes;
	u32	TxPackets;
	u32	TxMaxBytes;
};

typedef struct ltq_tso_port {
	unsigned char __iomem 	*membase;	/* Virtual */
	struct tasklet_struct	tasklet;	/* TSO tasklet */
	u32			port_number;	/* TSO port number */
	u32			TxPackets;	/* Total GSO packets transmitted */
	u32			TxErrorPackets;	/* Total Tx trasnmit error Packets */
	u32			TxHwError;	/* Total Tx HW Error Packets */
	u32			TxDonePackets;	/* Total GSO completed */
	u32			irq;		/* TSO port IRQ number */
	u32			rPos;		/* Avaialble SG buffer pos */
	u32			wPos;		/* Expected SG buffer pos */
	atomic_t		availBuffs;	/** Available SG buffers */
	struct timer_list	timer;		/* Timer */
	struct toe_sg_buffer 	sgBuffs[SG_BUFFER_PER_PORT]; 	/*! SG Buffer for tso port */
	struct tso_frags_stats	fragsStats[MAX_TSO_FRAGS_STATS + 1];
	struct sk_buff_head     processQ;
	u32			processQ_maxL;
} ltq_tso_port_t;

typedef struct ltq_lro_port {
	unsigned char __iomem *membase; /* Virtual */
	u32 port_num;
	unsigned int irq_num;
	bool in_use;
	int session_id;
	int to_read;
	uint8_t affinity;
#ifdef USE_TIMER_FOR_SESSION_STOP
	struct timer_list lro_timer;
#endif
} ltq_lro_port_t;

