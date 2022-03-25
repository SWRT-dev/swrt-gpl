/******************************************************************************
	Copyright (c) 2012
	Lantiq Deutschland GmbH

	For licensing information, see the file 'LICENSE' in the root folder of
	this software module.
******************************************************************************/

#ifndef _drv_tmu_ll_h_
#define _drv_tmu_ll_h_

/** \addtogroup TMU_LL_REFERENCE GPON Low-level Functions
   @{
*/

/** \addtogroup TMU_LL_TMU Traffic Management Unit Low-level Functions

   Low-level functions to control the Traffic Management Unit (TMU).
   @{
*/

#define CBM_PROC

#ifndef CONFIG_USE_EMULATOR
	/*Copy from chiptest, need change to UGW GRX500 Model macro once ready*/
	#define TMU_SOC_REAL_BOARD
#endif

#define PACKET_POINTER_TABLE_INDEX_MAX  6144	/* 6K */
#define EGRESS_QUEUE_ID_MAX             256
#define EGRESS_PORT_ID_MAX              72
#define SCHEDULER_BLOCK_INPUT_ID_MAX    (128*SCHEDULE_MAX_LEVEL)
#define SCHEDULER_BLOCK_ID_MAX          128
#define EPNNULL_EGRESS_PORT_ID          72
#define EPMT_LEN                        72
#define TOCKEN_BUCKET_ID                256
#define TOKEN_BUCKET_MAX                (TOCKEN_BUCKET_ID-1)
#define NULL_SCHEDULER_BLOCK_ID         127
#define NULL_SCHEDULER_INPUT_ID         1023
/*sgh tmu maximumum support 8 level from HW design */
#define SCHEDULE_MAX_LEVEL              8
/*maximum leaf per scheduler*/
#define SCHEDULER_MAX_LEAF              8
#define NULL_QUEUE_ID			(EGRESS_QUEUE_ID_MAX-1)
#define NULL_TOCKEN_BUCKET_ID		(TOCKEN_BUCKET_ID-1)

/*error defines*/
#define ERROR_SB_CANNOT_BE_REMOVED 1
#define SB_DEL_SUCCESS 0

/** Default global WRED crawler period. Time interval to elapse between QAVG
* calculations on consecutive queues
*/

/*changed as per input from Schaefer Franz J
sef (gpon team) changed on 21 may 2014
*/
#ifndef TMU_SOC_REAL_BOARD
	#define TMU_WRED_CRAWLER_PERIOD_DEFAULT     1
#else
	#define TMU_WRED_CRAWLER_PERIOD_DEFAULT     10
#endif
/** Default global Enqueue Request delay expressed in clocks. */
#define TMU_ENQUEUE_REQUEST_DELAY_DEFAULT   0
/** Default global Token Accumulation period expressed in clocks between
    subsequent TBS.
*/
#define TMU_TOKEN_ACC_PERIOD_DEFAULT        0
/** Number of TMU WRED Curves*/
#define TMU_WRED_CURVE_NUM          2

/* 9k Normal Frame size (9216) and 1K Jumbo Frame size.
In order to easy setting, here only use 9K normal frame number
*/
#define TMU_FSQM_FRAME_MAX 9216

/*Default setting qlobal thershold*/
/*sgh: follow GPON percentage: 0x3000/0x4800 */
#define TMU_GOTH_DEFAULT (TMU_FSQM_FRAME_MAX * 66/100)

/*Default setting egress port thershold*/
#define TMU_EPTT0_DEFAULT 0x0120	/*sgh add  */

/*Default setting queue thershold MITH0 and MITH1 [WRED]*/
/* sgh: follow GPON percentage: 0x11320/0x4800 */
#define TMU_QTHT1_DEFAULT (((TMU_EPTT0_DEFAULT)/8)/2)

/*Default setting queue thershold MATH0 and MATH1 [WRED]*/
/*sgh: follow GPON percentage: 0x11320/0x4800 */
#define TMU_QTHT2_DEFAULT ((TMU_EPTT0_DEFAULT)/8)

/*Default setting queue thershold SLOPE0 and SLOPE1 [WRED]*/
#define TMU_QTHT3_WRED_DEFAULT 0x0fff	/*sgh add  */

/*default queue thershold for unassigned col*/
/*sgh: follow GPON percentage: 0x2c38/0x4800 */
#define TMU_QTTH0_DEFAULT (TMU_FSQM_FRAME_MAX * 61/100)
#define TMU_QTHT4_0_DEFAULT 0x0004	/*sgh add  */

/*default queue thershold for Red col*/
#define TMU_QTHT4_1_DEFAULT 0x0000	/*sgh add  */

/** Minimum configurable value for Committed Information Rate, bytes/s*/
#define TMU_TBS_CIR_MIN	2500UL
/** Maximum configurable value for Committed Information Rate, bytes/s*/
#define TMU_TBS_CIR_MAX	125000000UL
/** Factory default value for Committed Information Rate, bytes/s*/
#define TMU_TBS_CIR_DEF	TMU_TBS_CIR_MAX

/** Minimum configurable value for Peak Information Rate, bytes/s*/
#define TMU_TBS_PIR_MIN	2500UL
/** Maximum configurable value for Peak Information Rate, bytes/s*/
#define TMU_TBS_PIR_MAX	125000000UL
/** Factory default value for Peak Information Rate, bytes/s*/
#define TMU_TBS_PIR_DEF	TMU_TBS_PIR_MAX

/** Maximum configurable value for TBS committed maximum burst size*/
#define TMU_TBS_CBS_MAX	16777215UL
/** Factory default value for TBS committed maximum burst size*/
#define TMU_TBS_CBS_DEF	16000000UL

/** Maximum configurable value for TBS peak maximum burst size*/
#define TMU_TBS_PBS_MAX	16777215UL
/** Factory default value for TBS peak maximum burst size*/
#define TMU_TBS_PBS_DEF	16000000UL

/**
   This structure is used to set the modes and drop thresholds
   for an egress queue.
*/
struct tmu_equeue_drop_params {
	/** Queue Enable */
	bool qe;

	/** color to threshold map
	    Each entry represents a mapping of the color denoted by the name to
	    a threshold denoted by the number assigned.
	    The assigned numbers are restricted to have a value between 0 and 3
	    interpreted as follows:
	    - 0: QTTH0
	    - 1: QTTH1
	    - 2: MATH0
	    - 3: MATH1
	    */
	uint8_t col[4];
	/** Dropping mode (0 = tail drop; 1 = WRED) */
	uint8_t dmod;
	/** Weighting factor for WRED Qavg calculation */
	uint8_t wq;
	/** Reservation threshold */
	uint32_t qrth;
	/** Maximum threshold 0 */
	uint32_t qtth0;
	/** Maximum threshold 1 */
	uint32_t qtth1;
	/** Minimum threshold for WRED curve 0 */
	uint32_t mith0;
	/** Maximum threshold for WRED curve 1 */
	uint32_t math0;
	/** Maximum drop probability at math0 in 1/256 (0...255) */
	uint32_t maxp0;
	/** Minimum threshold for WRED curve 1 */
	uint32_t mith1;
	/** Maximum threshold for WRED curve 1 */
	uint32_t math1;
	/** Maximum drop probability at math1 in 1/256 (0...255) */
	uint32_t maxp1;
};

/**
   This structure is used to get the complete status information of an
   egress queue.
*/
struct tmu_equeue_status {
	/** queue filled */
	bool qf;
	/** occupancy in PDUs*/
	uint32_t pocc;
	/** occupancy in segments */
	uint32_t qocc;
	/** occupancy average */
	uint32_t qavg;
	/** head packet pointer */
	uint32_t hqpp;
	/** tail packet pointer */
	uint32_t tqpp;
	/** QoS length of packet at head of egress queue */
	uint32_t qosl;
	/** color of packet at head of egress queue */
	uint32_t col;
	/** Segment length of packet at head of egress queue */
	uint32_t segl;
};

/**
   This structure is used to get the complete status information of a
   scheduler block input.
*/
struct tmu_sched_blk_in_status {
	/** scheduler input filled */
	bool sif;
	/** scheduler input passes token bucket shaper, if present */
	bool sip;
	/** virtual departure time if scheduler input (lowest gets scheduled
	    next)
	*/
	uint32_t vdt;
	/** QoS length of packet waiting next at this input */
	uint32_t qosl;
	/** color of packet waiting next at this input */
	uint32_t col;
};

/**
   This structure is used to get the complete status information of a
   scheduler block output.
*/
struct tmu_sched_blk_out_status {
	/** scheduler output filled */
	bool sof;
	/** winner leaf to be selected next to transmit from this SB */
	uint32_t wl;
	/** winner egress queue to be selected next to transmit from this SB */
	uint32_t wqid;
};

/**
   This structure is used to get the complete status information of an
   egress port.
*/
struct tmu_egress_port_status {
	/** egress port filled */
	bool epfilled;
	/** egress port occupancy counters */
	uint32_t epoc[4];
};

/**
   This structure is used to set the tail drop thresholds for an egress port.
*/
struct tmu_egress_port_thr {
	uint32_t epth[4];
};

/**
   This structure is used to set the global tail drop thresholds
*/
struct tmu_global_thr {
	uint32_t goth[4];
};

/**
   This structure is used to set the WFQ weights in a scheduler block
*/
struct tmu_sched_blk_in_weights {
	uint32_t iwgt[8];
};

/**
   This structure is used to link the egress port to a scheduler block
*/
struct tmu_eport_link {
	uint8_t epe;
	uint32_t sbid;
};

/**
   This structure is used to link the input of a scheduler block to a queue
   or another scheduler block in the hierarchy below.
*/
struct tmu_sched_blk_in_link {
	uint8_t sie;
	uint8_t sit;		/*0: QID 1: SBID */
	uint8_t tbe;		/* SGH add for tmu_sched_blk_in_link_get only */

	uint32_t iwgt;
	uint32_t qsid;
	uint32_t tbid;		/*SGH add for tmu_sched_blk_in_link_get only */

};

/**
   This structure is used to link the output of a scheduler block to an egress
   port or another scheduler block input in the hierarchy above.
*/
struct tmu_sched_blk_out_link {
	uint8_t soe;
	uint8_t lvl;
	uint8_t v;
	uint32_t omid;

};

/**
   This structure is used to link the output of a scheduler block to an egress
   port or another scheduler block input in the hierarchy above.
*/
struct tmu_equeue_link {
	uint8_t qe;	/** queue enable flag */
	uint32_t epn;	/**egree port id */
	uint32_t sbin;	/**sbin */

};

/**
   This structure is used to assign a token bucket shaper to the input of a
   scheduler block.
*/
struct tmu_sched_blk_in_tbs {
	uint8_t tbe;
	uint32_t tbid;

};

/**
   This structure is used to set the parameters of a token bucket shaper.

   \remarks The rate parameters pir and cir are given in bytes/sec.
   \remarks The burst size parameters pbs and cbs are given in bytes.
*/
struct tmu_token_bucket_shaper_params {
	/**
	    This field indicates the TB operating mode.
	    - 0H: MOD_0, strict color blind.
	    - 1H: MOD_1, RFC 2698 color aware.
	    - 2H: MOD_2, RFC 4115 color aware.
	    - 3H: MOD_3, loose color blind.
	    */
	uint32_t mod;
	/**
	    This bit indicates if the PIR/PBS bucket (0) is enabled.
	    - 0H: DIS, Disable.
	    - 1H: EN, Enable.
	    */
	uint32_t tbe0;
	/**
	    Peak Information Rate in bytes/s.
	    Supported range is 3 to 156,250,000 bytes/s
	    */
	uint32_t pir;

	/**
	    Peak Burst Size in bytes.
	    */
	uint32_t pbs;
	/**
	    This bit indicates if the CIR/CBS bucket (1) is enabled.
	    - 0H: DIS, Disable.
	    - 1H: EN, Enable.
	    */
	uint32_t tbe1;
	/**
	    Committed Information Rate in bytes/s.
	    Supported range is 3 to 156,250,000 bytes/s
	    */
	uint32_t cir;
	/**
	    Committed Burst Size in bytes.
	    */
	uint32_t cbs;
};

/**
   This structure is used to get the complete status information of a
   dual token bucket shaper.
*/
struct tmu_token_bucket_shaper_status {
	/** current pass condition at token bucket 0 */
	bool pass0;
	/** rate counter at token bucket 0 */
	uint32_t src0;
	/** token bucket counter at token bucket 0 */
	uint32_t tbc0;

	/** current pass condition at token bucket 1 */
	bool pass1;
	/** rate counter at token bucket 1 */
	uint32_t src1;
	/** token bucket counter at token bucket 1 */
	uint32_t tbc1;

	/** QoS length of packet waiting for emission at this shaper */
	uint32_t qosl;
	/** color of packet waiting for emission at this shaper */
	uint32_t col;
};

/**
   This structure is used to get the status information the LPI on a LAN port
*/
struct tmu_low_pwr_idle_status {
	/** buffer fill status  */
	bool lpstate;
	/** LPI_request signal status */
	bool lpireq;
	/** Timer */
	uint32_t lptimer;
};

/**
   This structure is used to set the parameters for the LPI mechanism.
*/
struct tmu_low_pwr_idle_params {
	/**Indicates the port occupancy threshold in units of segments where
	    power should be switched on immediately. THX = 0 disables the LPI
	    mechanism on a port, effective in a permanent setting of signal
	    LPI_request = 0.
		The recommended default value is defined by TMU_TMU_LPI_THX.
	*/
	uint32_t thx;
	/**Indicates the maximum time period(in units of 4096 clocks = 6.65 us)
	that may elapse from the event of data arriving to an empty port until
	LPI_request is deasserted.
	The recommended default value is defined by TMU_TMU_LPI_TOF.
	*/
	uint32_t tof;
	/**
	Indicates the maximum time period  (in units of 4096 clocks = 6.65 us)
	that may elapse from the event of a port getting empty until LPI_request
	is asserted.
	The recommended default value is defined by TMU_TMU_LPI_TON.
	*/
	uint32_t ton;
};

/* ***************************************************************************/

/**
   Set Activate/Deactivate switch for TMU state machines.

   The TMU configuration is divided into two major phases:

   1. STATIC configuration takes place before activation
   2. DYNAMIC configuration takes place after activation

   \remarks Some functions are restricted to STATIC configuration only !
   \remarks See the corresponding remarks in the function description
*/
void tmu_enable(bool act);

/**
   Get Activate/Deactivate switch of  TMU state machines.
*/
bool tmu_is_enabled(void);

/**
   Initialize the TMU block.
*/
int tmu_ll_init(unsigned long base_addr);
int tmu_ll_exit(void);

void tmu_qemt_write(const uint32_t qid, const uint32_t epn);
void tmu_qemt_read(const uint32_t qid, uint32_t *epn);

void tmu_qsmt_write(const uint32_t qid, const uint32_t sbin);
void tmu_qsmt_read(const uint32_t qid, uint32_t *sbin);

void tmu_qtht_write(const uint32_t qid, const uint32_t *qtht);
void tmu_qtht_read(const uint32_t qid, uint32_t *qtht);

void tmu_qoct_write(const uint32_t qid,
		    const uint32_t wq,
		    const uint32_t qrth,
		    const uint32_t qocc, const uint32_t qavg);
void tmu_qoct_read(const uint32_t qid,
		   uint32_t *wq,
		   uint32_t *qrth, uint32_t *qocc, uint32_t *qavg);

void tmu_qdct_write(const uint32_t qid, const uint32_t *qdc);
void tmu_qdct_read(const uint32_t qid, uint32_t *qdc);

void tmu_qfmt_write(const uint32_t qid, const uint32_t *qfm);
void tmu_qfmt_read(const uint32_t qid, uint32_t *qfm);

void tmu_epmt_write(const uint32_t epn, const uint32_t epe,
		    const uint32_t sbid);
void tmu_epmt_read(const uint32_t epn, uint32_t *epe, uint32_t *sbid);

void tmu_epot_write(const uint32_t epn, const uint32_t *epoc);
void tmu_epot_read(const uint32_t epn, uint32_t *epoc);

void tmu_eptt_write(const uint32_t epn, const uint32_t *ept);
void tmu_eptt_read(const uint32_t epn, uint32_t *ept);

void tmu_epdt_write(const uint32_t epn, const uint32_t *epd);
void tmu_epdt_read(const uint32_t epn, uint32_t *epd);

void tmu_cfgcmd_write(const uint32_t cfgcmd);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
void tmu_cfgcmd_read(uint32_t *cfgcmd);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

void tmu_sbit_write(const uint32_t sbin, const uint32_t *sbit);
void tmu_sbit_read(const uint32_t sbin, uint32_t *sbit);

void tmu_sbot_write(const uint32_t sbin, const uint32_t *sbot);
void tmu_sbot_read(const uint32_t sbin, uint32_t *sbot);
void tmu_sbot_write_cfg(const uint32_t sbin, const uint32_t *sbot);
void tmu_sbot_read_cfg(const uint32_t sbin, uint32_t *sbot);

void tmu_tbst_write(const uint32_t tbid, const uint32_t *tbst);
void tmu_tbst_read(const uint32_t tbid, uint32_t *tbst);
void tmu_tbst_write_cfg(const uint32_t tbid, const uint32_t *tbst);
void tmu_tbst_read_cfg(const uint32_t tbid, uint32_t *tbst);

/**
   Set Enable/Disable of an egress queue.

	\param   qid   egress queue identifier of egress queue to be disabled
	\param   ena   enable = true, disable = false

      \remarks dynamic configuration allowed
*/
void tmu_equeue_enable(const uint32_t qid, bool ena);

/**
   Get Enable/Disable of an egress queue.

	\param   qid   egress queue identifier of egress queue to be disabled
*/
bool tmu_is_equeue_enabled(const uint32_t qid);

/**
   Set link to scheduler block input and egress port on an egress queue.

	\param   qid   egress queue identifier of egress queue to be enabled
	\param   link  \ref tmu_equeue_link

      \remarks static configuration only
*/
void tmu_equeue_link_set(const uint32_t qid, struct tmu_equeue_link *link);

/**
   Get link to scheduler block input and egress port on an egress queue.

	\param   qid   egress queue identifier of egress queue to be enabled
	\param   link  \ref tmu_equeue_link
*/
void tmu_equeue_link_get(const uint32_t qid, struct tmu_equeue_link *link);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Set Color to Threshold mapping.

	\param   qid   egress queue identifier of egress queue to be configured
	\param   map   uses col array from
	structure \ref tmu_equeue_drop_params

      \remarks the threshold indices in the map are restricted to values
	between 0 and 3

      \remarks dynamic configuration allowed
*/
void tmu_equeue_color_thr_map_set(const uint32_t qid,
				  struct tmu_equeue_drop_params *map);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get Color to Threshold mapping.

	\param   qid   egress queue identifier of egress queue to be configured
	\param   map   uses col array from
	structure \ref tmu_equeue_drop_params
*/
void tmu_equeue_color_thr_map_get(const uint32_t qid,
				  struct tmu_equeue_drop_params *map);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/**
   Set Drop Mode and Thresholds for an egress queue.

	\param   qid   egress queue identifier of egress queue to be configured
	\param   thx   structure \ref tmu_equeue_drop_params
	with all relevant modes and thresholds in units of segments

      \remarks the internal granularity of thresholds is eight segments.
      \remarks the provided values will be truncated to the next lower
	multiple of 8.

      \remarks dynamic configuration allowed
*/
void tmu_equeue_drop_params_set(uint32_t qid,
				struct tmu_equeue_drop_params *thx);

/**
   Set Drop Probability.

	\param   curve  curve identifier, 0 or 1.
	\param   qid    egress queue identifier of egress queue to be configured
	\param   max_pr maximum drop probability
*/
void tmu_wred_curve_max_px_set(const uint32_t curve,
			       const uint32_t qid, const uint32_t max_pr);

/**
   Get Drop Probability.

	\param   curve  curve identifier, 0 or 1.
	\param   qid    egress queue identifier of egress queue to be configured

      \return  Maximum Drop Probability
*/
uint32_t tmu_wred_curve_max_px_get(const uint32_t curve, const uint32_t qid);

/**
   Get Drop Mode and Thresholds for an egress queue.

	\param   qid   egress queue identifier of egress queue to be read
	\param   thx   structure \ref tmu_equeue_drop_params
	with all relevant modes and thresholds in units of segments

      \remarks the internal granularity of thresholds is eight segments.
*/
void tmu_equeue_drop_params_get(uint32_t qid,
				struct tmu_equeue_drop_params *thx);

/**
   Get fill status of all egress queues.

	\param   qfill array of 8 * 32 bit

      \remarks the egress queue fill status is read only
*/
void tmu_equeue_fill_status_get(uint32_t *qfill);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get egress queue segment occupancy counters

	\param   qid   egress queue identifier of egress queue to be read
	\param   qocc  the occupancy of the queue in units of segments

      \remarks the egress queue occupancy status is read only after activation
*/
void tmu_equeue_seg_occupancy_get(uint32_t qid, uint32_t *qocc);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get egress queue PDU occupancy counters

	\param   qid   egress queue identifier of egress queue to be read
	\param   pocc  the occupancy of the queue in units of PDUs

      \remarks the egress queue occupancy status is read only after activation
*/
void tmu_equeue_pdu_occupancy_get(uint32_t qid, uint32_t *pocc);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get egress queue average counters

	\param   qid   egress queue identifier of egress queue to be read
	\param   qavg  the queue average as calculated for the WRED algorithm
*/
void tmu_equeue_average_get(uint32_t qid, uint32_t *qavg);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Set egress queue discard counters

	\param   qid   egress queue identifier of egress queue to be read
	\param   qdc  the array holding the discard counters for the four colors

      \remarks discards are counted in units of packets

      \remarks dynamic configuration allowed
*/
void tmu_equeue_discard_counters_set(uint32_t qid, uint32_t *qdc);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/**
   Get egress queue discard counters

	\param   qid   egress queue identifier of egress queue to be read
	\param   qdc  the array holding the discard counters for the four colors

      \remarks discards are counted in units of packets
*/
void tmu_equeue_discard_counters_get(uint32_t qid, uint32_t *qdc);

/**
   Get egress port discard counters

	\param   epn  egress port number of egress queue to be read
	\param   epd  the array holding the discard counters for the four colors

      \remarks discards are counted in units of packets
*/
void tmu_eport_discard_counters_get(const uint32_t epn, uint32_t *epd);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get the complete egress queue status.

	\param   qid      scheduler block output identifier
	\param   status   \ref tmu_equeue_status
*/
void tmu_equeue_status_get(const uint32_t qid,
			   struct tmu_equeue_status *status);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/**
   Set Enable/Disable bit on an egress port.

	\param   epn   egress port identifier of egress port to be
	enabled/disabled
	\param   ena   enable = true, disable = false

      \remarks dynamic configuration allowed

   When enabled during operation, the transmission through the egress port
   resumes.
*/
void tmu_egress_port_enable(const uint32_t epn, bool ena);

/**
   Get Enable/Disable status of an egress port.

	\param   epn   egress port identifier of egress port to be disabled
*/
bool tmu_is_egress_port_enabled(const uint32_t epn);

/**
   Set the scheduler block assignment to the egress port.

	\param   epn   egress port identifier of egress port
	\param   sbid  scheduler block identifier to be assigned to egress port

      \remarks static configuration only
*/
void tmu_egress_port_link_set(const uint32_t epn, uint32_t sbid);

/**
   Read back the scheduler block assignment to the egress port.

	\param   epn   egress port identifier of egress port
	\param   link  scheduler block identifier assigned to egress port and
		     enable bit
*/
void tmu_egress_port_link_get(const uint32_t epn,
			      struct tmu_eport_link *link);

/**
   Set Tail Drop Thresholds for an egress port.

	\param   epn   egress port identifier of egress port to be configured
	\param   epth  the four egress port thresholds provided
	in units of segments

      \remarks the internal granularity of thresholds is eight segments.
      \remarks the provided values will be truncated to the next
	lower multiple of 8.

      \remarks dynamic configuration allowed
*/
void tmu_egress_port_tail_drop_thr_set(uint32_t epn,
				       struct tmu_egress_port_thr *epth);

/**
   Get Tail Drop Thresholds for an egress port.

	\param   epn   egress port identifier of egress port to be read
	\param   epth  the four egress port thresholds configured
	in units of segments

      \remarks the internal granularity of thresholds is eight segments.
*/
void tmu_egress_port_tail_drop_thr_get(uint32_t epn,
				       struct tmu_egress_port_thr *epth);

/**
   Get fill status of all egress ports.

	\param   epfill 2 * 32 + 1 * 16 bits in epfilled registers

      \remarks the egress port fill status is read only
*/
void tmu_egress_port_fill_status_get(uint32_t *epfill);

/**
   Get egress port segment occupancy counters

	\param   epn   egress port identifier of egress port to be read
	\param   epoc  the occupancy of the port per color in units of segments

      \remarks the egress queue occupancy status is read only after activation
      \remarks epoc[0] holds the total occupancy in segments
      \remarks epoc[1] holds the occupancy in segments for green packets
      \remarks epoc[2] holds the occupancy in segments for yellow packets
      \remarks epoc[3] holds the occupancy in segments for red packets
*/
void tmu_egress_port_seg_occupancy_get(uint32_t epn, uint32_t *epoc);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get egress port segment occupancy counters

	\param   epn      egress port identifier of egress port to be read
	\param   status   \ref tmu_egress_port_status
*/
void tmu_egress_port_status_get(uint32_t epn,
				struct tmu_egress_port_status *status);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/**
   Set Enable/Disable on a scheduler block input.

	\param   sbin   scheduler block input identifier of input to be enabled
	\param   ena   enable = true, disable = false

      \remarks dynamic configuration allowed

   When enabled during operation, the scheduler block input is relogged into the
   scheduling process.

   When disabled during operation, the scheduler block input is removed from the
   scheduling process.
*/
void tmu_sched_blk_in_enable(const uint32_t sbin, bool ena);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get Enable/Disable of a scheduler block input.

	\param   sbin   scheduler block input identifier of input to be disabled
*/
bool tmu_is_sched_blk_in_enabled(const uint32_t sbin);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/**
   Set the link from the scheduler block input to an egress queue or
   another scheduler block in the hierarchy

	\param   sbin   scheduler block input identifier of input to be linked
	\param   link   \ref tmu_sched_blk_in_link

      \remarks static configuration only !

   When link->sit = 0 , then link->qsid is interpreted as a qid.

   When link->sit = 1 , then link->qsid is interpreted as a sbid.
*/
void tmu_sched_blk_in_link_set(const uint32_t sbin,
			       struct tmu_sched_blk_in_link *link);

/**
   Get the link from the scheduler block input to an egress queue or
   another scheduler block in the hierarchy

	\param   sbin   scheduler block input identifier of input to be read
	\param   link   \ref tmu_sched_blk_in_link

   When link->sit = 0 , then link->qsid is interpreted as a qid.

   When link->sit = 1 , then link->qsid is interpreted as a sbid.
*/
void tmu_sched_blk_in_link_get(const uint32_t sbin,
			       struct tmu_sched_blk_in_link *link);

/**
   Set single scheduler block input weight.

	\param   sbin     scheduler block input of SB to be configured
	\param   weight   the relative WFQ weight of the inputs

      \remarks The weight is restricted to values between 0 and 1023.
      \remarks A value of 0 denotes
	the highest weight and effectively provides strict priority of
	the respective inputs over all other inputs with higher weight.
      \remarks In case where several inputs of the same scheduler block have
	the same value of 0, the input with the lower input number gets
	strict priority over the input with the higher input number
      \remarks dynamic configuration allowed
*/
void tmu_sched_blk_in_weight_set(uint32_t sbin, uint16_t weight);

/**
   Get single scheduler block input weights.

	\param   sbin     scheduler block identifier of SB to be read
	\param   weight   the relative WFQ weights of the inputs
      \remarks The weight is restricted to values between 0 and 1023.
      \remarks A value of 0 denotes
	the highest weight and effectively provides strict priority of
	the respective inputs over all other inputs with higher weight.
      \remarks In case where several inputs of the same scheduler block have
	the same value of 0, the input with the lower input number gets
	strict priority over the input with the higher input number
*/
void tmu_sched_blk_in_weight_get(uint32_t sbin, uint16_t *weight);

/**
   Set all scheduler block input weights.

	\param   sbid     scheduler block identifier of SB to be configured
	\param   weights  the eight relative WFQ weights of the inputs
      \remarks The weights are restricted to values between 0 and 1023.
      \remarks A value of 0 denotes
	the highest weight and effectively provides strict priority of
	the respective inputs over all other inputs with higher weight.
      \remarks In case where several inputs of the same scheduler block have
	the same value of 0, the input with the lower input number gets
	strict priority over the input with the higher input number
      \remarks dynamic configuration allowed
*/
void tmu_sched_blk_in_weights_set(uint32_t sbid,
				  struct tmu_sched_blk_in_weights *weights);

/**
   Get all scheduler block input weights.

	\param   sbid     scheduler block identifier of SB to be read
	\param   weights  the eight relative WFQ weights of the inputs

      \remarks The weights are restricted to values between 0 and 1023.
      \remarks A value of 0 denotes
	the highest weight and effectively provides strict priority of
	the respective inputs over all other inputs with higher weight.
      \remarks In case where several inputs of the same scheduler block have
	the same value of 0, the input with the lower input number gets
	strict priority over the input with the higher input number
*/
void tmu_sched_blk_in_weights_get(uint32_t sbid,
				  struct tmu_sched_blk_in_weights *weights);

/**
   Set token bucket shaper assignment to a scheduler block input.
	\param   sbin   scheduler block input identifier
	\param   tbs    \ref tmu_sched_blk_in_tbs
      \remarks static configuration only
*/
void tmu_sched_blk_in_shaper_assign_set(const uint32_t sbin,
					struct tmu_sched_blk_in_tbs *tbs);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get token bucket shaper assignment to a scheduler block input.
	\param   sbin   scheduler block input identifier
	\param   tbs    \ref tmu_sched_blk_in_tbs
*/
void tmu_sched_blk_in_shaper_assign_get(const uint32_t sbin,
					struct tmu_sched_blk_in_tbs *tbs);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
      Set Enable/Disable of the token bucket shaper on this scheduler
	block input.
	\param   sbin   scheduler block input identifier
	\param   ena   enable = true, disable = false
      \remarks A valid token bucket identifier MUST be assigned before enabling
      \remarks Use function \ref tmu_sched_blk_in_shaper_assign_set to do this
      \remarks dynamic configuration allowed

	After enabling, subsequent
	transmissions from this scheduler block input are
	subject to rate shaping according to the parameters set for the
	token bucket shaper assigned to this input.
	After disabling, Subsequent transmissions from this scheduler
	block input are
	no longer subject to rate shaping.
*/
void tmu_sched_blk_in_shaper_enable(const uint32_t sbin, bool ena);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get Enable/Disable status of the token bucket shaper on this
   scheduler block input.
	\param   sbin   scheduler block input identifier
*/
bool tmu_is_sched_blk_in_shaper_enabled(const uint32_t sbin);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get the complete scheduler block input status.
	\param   sbin   scheduler block input identifier
	\param   status \ref tmu_sched_blk_in_status
*/
void tmu_sched_blk_in_status_get(const uint32_t sbin,
				 struct tmu_sched_blk_in_status *status);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/**
   Set Enable/Disable on a scheduler block output.

	\param   sbid   scheduler block identifier of output to be enabled
	\param   ena   enable = true, disable = false

	When enabled during operation,
	the scheduler block output is relogged to the
	scheduling process.
	When disabled during operation,
	the scheduler block output is removed from
	the scheduling process.
*/
void tmu_sched_blk_out_enable(const uint32_t sbid, bool ena);

/**
   Get Enable/Disable of a scheduler block output.
	\param   sbid   scheduler block identifier of output to be enabled
      \remarks dynamic configuration allowed
*/
bool tmu_is_sched_blk_out_enabled(const uint32_t sbid);

/**
   Set the link from the scheduler block output to an egress port or
   another scheduler block input in the hierarchy above
	\param   sbid   scheduler block input identifier of input to be linked
	\param   link   \ref tmu_sched_blk_out_link
      \remarks static configuration only
	When link->v = 0 , then link->omid is interpreted as a epn.
	When link->v = 1 , then link->omid is interpreted as a sbin.
*/
void tmu_sched_blk_out_link_set(const uint32_t sbid,
				struct tmu_sched_blk_out_link *link);

/**
   Get the link from the scheduler block output to an egress port or
   another scheduler block input in the hierarchy above
	\param   sbid   scheduler block input identifier of input to be linked
	\param   link   \ref tmu_sched_blk_out_link
	When link->v = 0 , then link->omid is interpreted as a epn.
	When link->v = 1 , then link->omid is interpreted as a sbin.
*/
void tmu_sched_blk_out_link_get(const uint32_t sbid,
				struct tmu_sched_blk_out_link *link);

/**
   Get the complete scheduler block output status.
	\param   sbid   scheduler block output identifier
	\param   status \ref tmu_sched_blk_out_status
*/
void tmu_sched_blk_out_status_get(const uint32_t sbid,
				  struct tmu_sched_blk_out_status *status);

/**
   Set dual token bucket shaper link to scheduler block input

	\param   tbid   token bucket shaper to be configured
	\param   sbin   scheduler block input

      \remarks dynamic configuration allowed
*/

void tmu_token_bucket_shaper_link_set(const uint32_t tbid,
				      const uint32_t sbin);

/**
   Get dual token bucket shaper link to scheduler block input

	\param   tbid   token bucket shaper to be configured
	\param   sbin   scheduler block input

      \remarks dynamic configuration allowed
*/

void tmu_token_bucket_shaper_link_get(const uint32_t tbid, uint32_t *sbin);
/**
   Set dual token bucket shaper configuration

	\param   tbid   token bucket shaper to be configured
	\param   para   token bucket shaper configuration parameters

      \remarks dynamic configuration allowed
*/
void
tmu_token_bucket_shaper_cfg_set(const uint32_t tbid,
				struct tmu_token_bucket_shaper_params *para);

/**
   Get dual token bucket shaper configuration.

	\param   tbid   token bucket shaper to be configured
	\param   para   token bucket shaper configuration parameters
*/
void
tmu_token_bucket_shaper_cfg_get(const uint32_t tbid,
				struct tmu_token_bucket_shaper_params *para);

/**
   Get the complete token bucket shaper status.

	\param   tbid   token bucket shaper identifier
	\param   sts    \ref tmu_token_bucket_shaper_status
*/
void
tmu_token_bucket_shaper_status_get(const uint32_t tbid,
				   struct tmu_token_bucket_shaper_status
				   *sts);

/**
   Set global Tail Drop Thresholds for TMU

	\param   thx   the four global thresholds provided
	in units of segments
      \remarks dynamic configuration allowed
*/
void tmu_global_tail_drop_thr_set(struct tmu_global_thr *thx);

/**
   Get global Tail Drop Thresholds for TMU

	\param   thx   the four global thresholds provided
	in units of segments
*/
void tmu_global_tail_drop_thr_get(struct tmu_global_thr *thx);

/**
   Set enable/disable status of token bucket accumulation

	\param   dta  enable = false , disable = true

      \remarks static configuration only
      \remarks must not be used after activation of the state machine
*/
void tmu_token_accumulation_disable(bool dta);

/**
   Get enable/disable status of token bucket accumulation
*/
bool tmu_is_token_accumulation_disabled(void);

/**
   Set relog pipeline sequential mode

	\param rps pipelined = false , sequential = true

      \remarks static configuration only
      \remarks must not be used after activation of the state machine
*/
void tmu_relog_sequential(bool rps);

/**
   Get relog pipeline sequential mode
*/
bool tmu_is_relog_sequential(void);

/**
   Set maximum token bucket number

	\param   maxtb  maximum number of token buckets supported

      \remarks static configuration only
      \remarks must not be used after activation of the state machine
*/
void tmu_max_token_bucket_set(uint32_t maxtb);

/**
   Get maximum token bucket number

	\param   maxtb  maximum number of token buckets supported
*/
void tmu_max_token_bucket_get(uint32_t *maxtb);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Set global free pointer counter

	\param   fpc  the number of available free pointers

      \remarks static configuration only
      \remarks must not be used after activation of the state machine
*/
void tmu_free_pointer_counter_set(uint32_t fpc);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/**
   Get global free pointer counter

	\param   fpc  the number of available free pointers
*/
void tmu_free_pointer_counter_get(uint32_t *fpc);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Set global free pointer threshold

	\param   fpth  the number of pointers that remain unused

      \remarks static configuration only
      \remarks must not be used after activation of the state machine
*/
void tmu_free_pointer_thr_set(uint32_t fpth);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get global free pointer threshold

	\param   fpth  the number of pointers that remain unused
*/
void tmu_free_pointer_thr_get(uint32_t *fpth);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/**
   Set global WRED crawler period

	\param   cp the time interval to elapse between QAVG calculations on
			consecutive queues

      \remarks static configuration only
      \remarks must not be used after activation of the state machine
*/
void tmu_crawler_period_set(uint32_t cp);

/**
   Get global WRED crawler period

	\param   cp the time interval to elapse between QAVG calculations on
			consecutive queues
*/
void tmu_crawler_period_get(uint32_t *cp);

/**
   Set global LFSR

	\param   rn the new seed of the LFSR random number generator

      \remarks static configuration only
      \remarks must not be used after activation of the state machine
*/
void tmu_random_number_set(uint32_t rn);

/**
   Get global LFSR

	\param   rn the current value of the LFSR random number generator
*/
void tmu_random_number_get(uint32_t *rn);

/**
   Set global Enqueue Request delay

	\param   erd enqueue request delay in clocks

      \remarks static configuration only
      \remarks must not be used after activation of the state machine
*/
void tmu_enqueue_delay_set(uint32_t erd);

/**
   Get global Enqueue Request delay

	\param   erd enqueue request delay in clocks
*/
void tmu_enqueue_delay_get(uint32_t *erd);

/**
   Set global Token Accumulation period
	\param   tacp token accumulation period delay in clocks between
	subsequent TBS
      \remarks static configuration only
      \remarks must not be used after activation of the state machine
*/
void tmu_tacc_period_set(uint32_t tacp);

/**
   Get global Token Accumulation period delay
	\param   tacp token accumulation period delay in clocks between
	subsequent TBS
*/
void tmu_tacc_period_get(uint32_t *tacp);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Set global SSB occupancy counters for TMU

	\param   gocc	the occupancy status of the SSB portion shared in the
	TMU in units of segments

      \remarks static configuration only
      \remarks must not be used after activation of the state machine

*/
void tmu_global_occupancy_set(uint32_t gocc);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/**
   Get global SSB occupancy counters for TMU

	\param   gocc  the occupancy status of the SSB portion shared in the
			TMU in units of segments

*/
void tmu_global_occupancy_get(uint32_t *gocc);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Set global discard counters

	\param   gpdc  the array holding the discard counters for the four
			colors

      \remarks dynamic configuration allowed

*/
void tmu_global_discard_counters_set(uint32_t *gpdc);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/**
   Set global discard counters

	\param   gpdc  the array holding the discard counters for the four
			colors

*/
void tmu_global_discard_counters_get(uint32_t *gpdc);

/**
   Set configuration parameters for Low Power Idle mechanism on a LAN port

	\param   lan_idx LAN port index (0, 1, 2, 3)
	\param   lpi     \ref tmu_low_pwr_idle_params with threshold and timer
			settings


*/
void tmu_low_power_idle_cfg_set(const uint8_t lan_idx,
				struct tmu_low_pwr_idle_params *lpi);

/**
   Get configuration parameters for Low Power Idle mechanism on a LAN port

	\param   lan_idx LAN port index (0, 1, 2, 3)
	\param   lpi     \ref tmu_low_pwr_idle_params with threshold and timer
			settings

      \remarks static configuration only
      \remarks must not be used after activation of the state machine

*/
void tmu_low_power_idle_cfg_get(const uint8_t lan_idx,
				struct tmu_low_pwr_idle_params *lpi);

/**
   Get Low Power Idle status on a LAN port.

	\param   lan_idx	LAN port index (0, 1, 2, or 3)
	\param   status   \ref tmu_low_pwr_idle_status


*/
void tmu_low_power_idle_status_get(const uint8_t lan_idx,
				   struct tmu_low_pwr_idle_status *status);

/**
   Enable/Disable specific TMU interrupts

	\param   mask_clr   clear bitmap according to the fields below
	\param   mask_set   set bitmap according to the fields below

   - 0   PNA   PDU Not Accepted
   - 1   FPU   Free Pointer Underflow
   - 4   GOE0  Global Occupancy Threshold 0 Exceeded
   - 5   GOE1  Global Occupancy Threshold 1 Exceeded
   - 6   GOE2  Global Occupancy Threshold 2 Exceeded
   - 7   GOE3  Global Occupancy Threshold 3 Exceeded
   - 8   EPFC0 Egress CPU Port 0 Filled
   - 9   EPFC0 Egress CPU Port 1 Filled
   - 10  EPFC0 Egress CPU Port 2 Filled
   - 11  EPFC0 Egress CPU Port 3 Filled
   - 17  VOE   VDT Overflow Error
   - 18  NIL   SB NIL Result Error
   - 24  SRC0N SRC0 Negative Limit
   - 25  SRC1N SRC1 Negative Limit

*/
void tmu_interrupt_enable_set(const uint32_t mask_clr,
			      const uint32_t mask_set);

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get TMU interrupt mask

   - 0   PNA   PDU Not Accepted
   - 1   FPU   Free Pointer Underflow
   - 4   GOE0  Global Occupancy Threshold 0 Exceeded
   - 5   GOE1  Global Occupancy Threshold 1 Exceeded
   - 6   GOE2  Global Occupancy Threshold 2 Exceeded
   - 7   GOE3  Global Occupancy Threshold 3 Exceeded
   - 8   EPFC0 Egress CPU Port 0 Filled
   - 9   EPFC0 Egress CPU Port 1 Filled
   - 10  EPFC0 Egress CPU Port 2 Filled
   - 11  EPFC0 Egress CPU Port 3 Filled
   - 17  VOE   VDT Overflow Error
   - 18  NIL   SB NIL Result Error
   - 24  SRC0N SRC0 Negative Limit
   - 25  SRC1N SRC1 Negative Limit

*/
uint32_t tmu_interrupt_enable_get(void);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Set TMU interrupt ctrl

	\param   ctrl   bitmap according to the fields below

   - 0   PNA   PDU Not Accepted
   - 1   FPU   Free Pointer Underflow
   - 4   GOE0  Global Occupancy Threshold 0 Exceeded
   - 5   GOE1  Global Occupancy Threshold 1 Exceeded
   - 6   GOE2  Global Occupancy Threshold 2 Exceeded
   - 7   GOE3  Global Occupancy Threshold 3 Exceeded
   - 8   EPFC0 Egress CPU Port 0 Filled
   - 9   EPFC0 Egress CPU Port 1 Filled
   - 10  EPFC0 Egress CPU Port 2 Filled
   - 11  EPFC0 Egress CPU Port 3 Filled
   - 17  VOE   VDT Overflow Error
   - 18  NIL   SB NIL Result Error
   - 24  SRC0N SRC0 Negative Limit
   - 25  SRC1N SRC1 Negative Limit

*/
void tmu_interrupt_control_set(const uint32_t ctrl);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get TMU interrupt ctrl

	\param   ctrl   bitmap according to the fields below

   - 0   PNA   PDU Not Accepted
   - 1   FPU   Free Pointer Underflow
   - 4   GOE0  Global Occupancy Threshold 0 Exceeded
   - 5   GOE1  Global Occupancy Threshold 1 Exceeded
   - 6   GOE2  Global Occupancy Threshold 2 Exceeded
   - 7   GOE3  Global Occupancy Threshold 3 Exceeded
   - 8   EPFC0 Egress CPU Port 0 Filled
   - 9   EPFC0 Egress CPU Port 1 Filled
   - 10  EPFC0 Egress CPU Port 2 Filled
   - 11  EPFC0 Egress CPU Port 3 Filled
   - 17  VOE   VDT Overflow Error
   - 18  NIL   SB NIL Result Error
   - 24  SRC0N SRC0 Negative Limit
   - 25  SRC1N SRC1 Negative Limit

*/
void tmu_interrupt_control_get(uint32_t *ctrl);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Set TMU interrupt capture

	\param   capt   bitmap according to the fields below

   - 0   PNA   PDU Not Accepted
   - 1   FPU   Free Pointer Underflow
   - 4   GOE0  Global Occupancy Threshold 0 Exceeded
   - 5   GOE1  Global Occupancy Threshold 1 Exceeded
   - 6   GOE2  Global Occupancy Threshold 2 Exceeded
   - 7   GOE3  Global Occupancy Threshold 3 Exceeded
   - 8   EPFC0 Egress CPU Port 0 Filled
   - 9   EPFC0 Egress CPU Port 1 Filled
   - 10  EPFC0 Egress CPU Port 2 Filled
   - 11  EPFC0 Egress CPU Port 3 Filled
   - 17  VOE   VDT Overflow Error
   - 18  NIL   SB NIL Result Error
   - 24  SRC0N SRC0 Negative Limit
   - 25  SRC1N SRC1 Negative Limit

*/
void tmu_interrupt_capture_set(const uint32_t capt);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

#ifdef INCLUDE_UNUSED_LL_FUNCTIONS
/**
   Get TMU interrupt capture

	\param   capt   bitmap according to the fields below

   - 0   PNA   PDU Not Accepted
   - 1   FPU   Free Pointer Underflow
   - 4   GOE0  Global Occupancy Threshold 0 Exceeded
   - 5   GOE1  Global Occupancy Threshold 1 Exceeded
   - 6   GOE2  Global Occupancy Threshold 2 Exceeded
   - 7   GOE3  Global Occupancy Threshold 3 Exceeded
   - 8   EPFC0 Egress CPU Port 0 Filled
   - 9   EPFC0 Egress CPU Port 1 Filled
   - 10  EPFC0 Egress CPU Port 2 Filled
   - 11  EPFC0 Egress CPU Port 3 Filled
   - 17  VOE   VDT Overflow Error
   - 18  NIL   SB NIL Result Error
   - 24  SRC0N SRC0 Negative Limit
   - 25  SRC1N SRC1 Negative Limit

*/
void tmu_interrupt_capture_get(uint32_t *capt);
#endif				/* #ifdef INCLUDE_UNUSED_LL_FUNCTIONS */

/* =============================================================
   High Level Functions
   =============================================================
*/

/**
   This function is used to activate a formerly unused scheduler block
   for transmission and either assign it to an egress port or to another
   scheduler block input.
   In the latter case the function is called as part of the stepwise process of
   building a scheduler hierarchy.

	\param   sbid  scheduler block identifier of scheduler block to be
		created
	\param   lvl	level within the scheduling hierarchy. This number is
			equal to the longest path counted in number of
			schedulers seen from this scheduler down the hierarchy.
	\param   omid  scheduler block input or egress port number,
			depending on parameter v
	\param   v     specifies if parameter omid is interpreted as
			a scheduler block input number (v == 1) or
			an egress port number (v == 0)
	\param   weight	specifies the initial weight of this scheduler within
			the next level hierarchy

      \remarks This function is the first step in the process of building
a data path through the TMU.

   Functional requirements:
   -# Establishment of a bidirectional link between a scheduler block output and
      - an egress port in case it is a toplevel scheduler block (v == 0)
      - a scheduler block input in case it is not a toplevel scheduler (v == 1)
   -# Enabling of the scheduler block output for transmission

*/
void tmu_sched_blk_create(const uint32_t sbid, const uint8_t lvl,
			  const uint32_t omid, const uint8_t v,
			  const uint16_t weight);

/**
   This function is needed to activate a formerly unused egress queue and
   assign it to a scheduler block input.

	\param   qid	egress queue identifier of egress queue to be created
	\param   sbin  scheduler block input number where the egress queue
	is assigned to
	\param   epn	egress port number where egress queue belongs to via
	scheduler hierarchy
      \remarks This function is the last step in the process of building a
	data path through the TMU.
      \remarks It is assumed that a scheduler block hierarchy has been created
	from sbin up to epn beforehand.
      \remarks Parameter epn MUST identify the egress port assigned to the
	toplevel scheduler in the hierarchy above sbin.
      \remarks static configuration only
      \remarks must not be used after activation of the state machine

   Functional requirements:
   - Establishment of a bidirectional link between an egress queue and a
	scheduler block input.
   - Enabling of the scheduler block input for PDU transmission.
   - Enabling of the egress queue for PDU acceptance.
*/
void tmu_egress_queue_create(const uint32_t qid, const uint32_t sbin,
			     const uint32_t epn);

/**
   This function is needed to activate a formerly unused token bucket shaper
   and assign it to a scheduler block input.

	\param   tbid  token bucket shaper identifier of token bucket shaper
	to be created
	\param   sbin  scheduler block input number where token bucket shaper
	is assigned to
      \remarks This function is an optional step in the process of building a
	data path through the TMU.
      \remarks It is assumed that the scheduler block identified by sbin has
	been created beforehand.
      \remarks It is assumed that the token bucket shaper identified by tbid has
	been configured beforehand.

      \remarks static configuration only
      \remarks must not be used after activation of the state machine

   Functional requirements:
   -# Establishment a bidirectional link between token bucket shaper and
	a scheduler block input
   -# Enabling of the token bucket shaper on the scheduler block input
*/
void tmu_token_bucket_shaper_create(const uint32_t tbid, const uint32_t sbin);

/**
   \todo add description
*/
void tmu_token_bucket_shaper_delete(const uint32_t tbid, const uint32_t sbin);

/**
   Create a flat homogeneous path (one scheduler block per egress port)
   through the TMU.

	\param	num_ports   number of contiguous egress ports to be created
	\param	base_epn	smallest epn  in a contiguous range
	\param	base_sbid	smallest sbid in a contiguous range
	\param	base_qid	smallest qid  in a contiguous range
	\param	qid_per_sb	the number of egress queues to be
				assigned per sb

      \remarks static configuration only
      \remarks must not be used after activation of the state machine


*/
void tmu_create_flat_egress_path(const uint16_t num_ports,
				 const uint16_t base_epn,
				 const uint16_t base_sbid,
				 const uint16_t base_qid,
				 const uint16_t qid_per_sb);

/**
	Delete queue from the configuration
	The following operation will done:
	1. TMU Register: QTHT0 - Queue Threshold Table Register 0
	   (Set 0 to QE field to disable the queue)
	   FIRST : disable Queue

	2. TMU Register: QEMT - Queue Egress Mapping Table Register
	   (Reset the EPN field with Default/NONE valid EPN value)
	   TMU Register: QSMT - Queue Scheduler Mapping Table Register
	   (Reset the SBIN field with Default/NONE valid SBIN value)

	3. Reset the QEMT and QSMT to non-vaild EPN/SB
*/
void tmu_egress_queue_delete(const uint32_t qid);

/**
	Delete scheduler block from the configuration
	The following operation will done:
	1. check SB inputs (read sbit0 for every leaf)
	   get qsid from sbit
	   if any of the SB inlink has valid SB/Queue id
	   SB can not be removed/deleted
	   return ERROR

	2. Disable scheduler block output
	3. disable next SB's input link or disable port
	4. Disable all scheduler block inputs for this scheduler (0-7)
	   TMU Register: SBITR0 - SB Input Table Register 0
	   (Reset the QSID field with Default/NONE valid QID value
	    and Set 0 to SIE field )
	5. delete token buket attached to the sb leaf
*/
int tmu_sched_blk_delete(const uint32_t sbid);


extern uint32_t read_write_reg_tbst(uint32_t cmd, uint32_t regid,
				    char *tsbst_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_sbit(uint32_t set_cmd, uint32_t sbin,
				    char *sbit_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_sbot(uint32_t set_cmd, uint32_t sbid,
				    char *sbot_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_epot(uint32_t set_cmd, uint32_t epn,
				    char *epot_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_eptt(uint32_t set_cmd, uint32_t epn,
				    char *eptt_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_epdt(uint32_t set_cmd, uint32_t epn,
				    char *epdt_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_epmt(uint32_t set_cmd, uint32_t epn,
				    char *epmt_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_qtht(uint32_t set_cmd, uint32_t qid,
				    char *qtht_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_qoct(uint32_t set_cmd, uint32_t qid,
				    char *qoct_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_qdct(uint32_t set_cmd, uint32_t qid,
				    char *qdct_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_qfmt(uint32_t set_cmd, uint32_t qid,
				    char *qfmt_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_qemt(uint32_t set_cmd, uint32_t qid,
				    char *qemt_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_qsmt(uint32_t set_cmd, uint32_t qid,
				    char *qsmt_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_qmtc(uint32_t set_cmd, uint32_t qid,
				    char *qmtc_reg_name, uint32_t bit_offset,
				    uint32_t bit_mask, uint32_t *value,
				    uint32_t flag);
extern uint32_t read_write_reg_gothr(uint32_t set_cmd, uint32_t qid,
				   char *gothr_reg_name, uint32_t bit_offset,
				   uint32_t bit_mask, uint32_t *value,
				   uint32_t flag);
extern uint32_t read_write_reg_cpr(uint32_t set_cmd, uint32_t qid,
				   char *cpr_reg_name, uint32_t bit_offset,
				   uint32_t bit_mask, uint32_t *value,
				   uint32_t flag);
extern uint32_t read_write_reg_csr(uint32_t set_cmd, uint32_t qid,
				   char *csr_reg_name, uint32_t bit_offset,
				   uint32_t bit_mask, uint32_t *value,
				   uint32_t flag);

extern uint32_t read_write_var_proc_id(uint32_t cmd, uint32_t regid,
				       uint32_t bit_offset, uint32_t bit_mask,
				       uint32_t flag);

extern uint32_t get_enq_counter(uint32_t index);
extern uint32_t get_deq_counter(uint32_t index);
extern char *get_dma_flags_str(uint32_t epn, char *buf, int buf_len);

extern int tmu_ll_stack_init(void);
extern int tmu_ll_stack_exit(void);

extern int tmu_proc_install(struct dentry *parent);
extern int tmu_queue_list(int tmu_dp, uint32_t *q_buff, int q_size,
	int *q_num, struct seq_file *s);
extern int tmu_reset_mib_all(u32 flag);


/*! @} */

/*! @} */

#endif /*_drv_tmu_h_ */
