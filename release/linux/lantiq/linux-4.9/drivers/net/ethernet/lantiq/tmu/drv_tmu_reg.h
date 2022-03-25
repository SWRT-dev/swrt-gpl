/******************************************************************************

				Copyright (c) 2012
				Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef _drv_tmu_reg_h
#define _drv_tmu_reg_h

#ifndef KSEG1
#define KSEG1 0xA0000000
#endif

#ifdef CONFIG_LTQ_TMU_DEVICE_TREE
/*extern __iomem void *tmu_membase; */
#define TMU_BASE	((unsigned int) tmu_membase)

#else
#ifdef CONFIG_LTQ_TMU_DDR_SIMULATE_REG
extern struct tmu_reg simulate_tmu;
#define TMU_BASE     (&simulate_tmu)
#else

/** address range for tmu
    0x1D404000--0x1D404FFF
	*/
#define TMU_BASE (KSEG1 | 0x1E700000)
#define TMU_TMU_SIZE 0xFFF
/*SGH modified from (KSEG1 | 0x1D404FFF)*/
#define TMU_TMU_END (TMU_BASE + TMU_TMU_SIZE)

#endif	/*end of CONFIG_LTQ_TMU_DDR_SIMULATE_REG */
#endif	/*end of CONFIG_LTQ_TMU_DEVICE_TREE */

/** \addtogroup TMU_REGISTER
   @{
*/

extern struct tmu_reg *tmu;

/* access macros */
#define tmu_r32(reg) reg_r32(&tmu->reg)
#define tmu_w32(val, reg) reg_w32(val, &tmu->reg)
#define tmu_w32_mask(clear, set, reg) reg_w32_mask(clear, set, &tmu->reg)
#define tmu_r32_table(reg, idx) reg_r32_table(tmu->reg, idx)
#define tmu_w32_table(val, reg, idx) reg_w32_table(val, tmu->reg, idx)
#define tmu_w32_table_mask(clear, set, reg, idx) \
	reg_w32_table_mask(clear, set, tmu->reg, idx)

/** TMU register structure */
struct tmu_reg {
   /** Control Register
       This register provides the global TMU controls.
	*/
	unsigned int ctrl;	/* 0x00000000 */
   /** Free Pointer List
       This register provides access to Free Pointer List built on PPL.
	*/
	unsigned int fpl;	/* 0x00000004 */
   /** Free Pointer Counter Register
       This register provides the current free PDU pointer number.
	*/
	unsigned int fpcr;	/* 0x00000008 */
   /** Free Pointer Threshold Register
       This register provides the free PDU pointer threshold.
       0 means all pointers can be used
	*/
	unsigned int fpthr;	/* 0x0000000C */
   /** Timer counter
	This register provides a high resolution timer
	(units of 1 clock period) needed for token bucket shaping
	*/
	unsigned int timer;	/* 0x00000010 */
   /** Linear Feedback Shift Register
       This register provides the Random Pattern
	*/
	unsigned int lfsr;	/* 0x00000014 */
   /** Crawler Period Register
       This register provides the WRED crawler period for QAVG calculation.
	*/
	unsigned int cpr;	/* 0x00000018 */
   /** Crawler Status Register
       This register provides the current crawler status.
	*/
	unsigned int csr;	/* 0x0000001C */
   /** Global Occupancy Register
       This register provides the global TMU occupancy status.
	*/
	unsigned int goccr;	/* 0x00000020 */
   /** Reserved */
	unsigned int res_0[3];	/* 0x00000024 */
   /** Global Occupancy Threshold Register n
       This register provides the global occupancy threshold for color n
	*/
	unsigned int gothr[4];	/* 0x00000030 */
   /** Global PDU Discard Counter Register
       This register provides the global discard counter for color n
	*/
	unsigned int gpdcr[4];	/* 0x00000040 */
   /** Reserved */
	unsigned int res_1[4];	/* 0x00000050 */
   /** Queue Fill Status Register
       This register provides fill status information for queues from 32 x n to 32 x n + 31
	*/
	unsigned int qfill[8];	/* 0x00000060 */
   /** Egress Port Filled Status Register 0
       This register provides fill status information for egress ports from 0 to  31, 32 to  63, 64 to  7
	*/
	unsigned int epfr[3];	/* 0x00000080 */
   /** Reserved */
	unsigned int res_2[5];	/* 0x0000008C */
   /** Low Power Idle Configuration for LAN port n
       This register provides thresholds and timer reload values for the control of the LPI
	*/
	unsigned int lpic[4];	/* 0x000000A0 */
   /** Low Power Idle Timer Status n
       This register provides the timer and status variable for the control of the LPI
	*/
	unsigned int lpit[4];	/* 0x000000B0 */
   /** Enqueue Request Delay Register
       This register provides means to control the delay between acceptance of subsequent Enqueue Requests
	*/
	unsigned int erdr;	/* 0x000000C0 */
   /** Token Accumulation Disable Slot Period Register
       This register provides means to disable slots for token accumulation
	*/
	unsigned int tadspr;	/* 0x000000C4 */
   /** Control Debug Register
       This register provides additional debug controls
	*/
	unsigned int ctrldbg;	/* 0x000000C8 */
   /** Scheduler Relog Control Register
       This register provides means to control the pending relog requests in the relog FIFOs
	*/
	unsigned int relogctrl;	/* 0x000000CC */
   /** Reserved */
	unsigned int res_3[12];	/* 0x000000D0 */
   /** IRN Capture Register
       This register shows the currently active interrupt events masked with the corresponding enable bits of the IRNEN register. The interrupts can be acknowledged by a write operation.
	*/
	unsigned int irncr;	/* 0x00000100 */
   /** IRN Interrupt Control Register
       A write operation directly effects the interrupts. This can be used to trigger events under software control for testing purposes. A read operation returns the unmasked interrupt events.
	*/
	unsigned int irnicr;	/* 0x00000104 */
   /** IRN Interrupt Enable Register
       This register contains the enable (or mask) bits for the interrupts. Disabled interrupts are not visible in the IRNCR register and are not signalled via the interrupt line towards the controller.
	*/
	unsigned int irnen;	/* 0x00000108 */
   /** Reserved */
	unsigned int res_4;	/* 0x0000010C */
   /** Token Bucket ID Capture Register
       This register provides the TBID captured in case of SRC0N or SRC1N interrupts
	*/
	unsigned int tbidcr;	/* 0x00000110 */
   /** Reserved */
	unsigned int res_5[3];	/* 0x00000114 */
   /** Enqueue Request INFO Capture Register
       This register provides the INFO captured in case of PNA interrupts
	*/
	unsigned int erinfocr;	/* 0x00000120 */
   /** Reserved */
	unsigned int res_6[55];	/* 0x00000124 */
   /** Queue Egress Mapping Table Register
       This register provides the mapping of the queue to an egress port.
	*/
	unsigned int qemt;	/* 0x00000200 */
   /** Reserved */
	unsigned int res_7[3];	/* 0x00000204 */
   /** Queue Scheduler Mapping Table Register
       This register provides the mapping of the queue to scheduler input.
	*/
	unsigned int qsmt;	/* 0x00000210 */
   /** Reserved */
	unsigned int res_8[3];	/* 0x00000214 */
   /** Queue Threshold Table Register 0
       This register provides access to Queue Threshold Configuration Parameters.
	*/
	unsigned int qtht0;	/* 0x00000220 */
   /** Queue Threshold Table Register 1
       This register provides access to Queue Threshold Configuration Parameters.
	*/
	unsigned int qtht1;	/* 0x00000224 */
   /** Queue Threshold Table Register 2
       This register provides access to Queue Threshold Configuration Parameters.
	*/
	unsigned int qtht2;	/* 0x00000228 */
   /** Queue Threshold Table Register 3
       This register provides access to Queue Threshold Configuration Parameters.
	*/
	unsigned int qtht3;	/* 0x0000022C */
   /** Queue Threshold Table Register 4
       This register provides access to Queue Threshold Configuration Parameters.
	*/
	unsigned int qtht4;	/* 0x00000230 */
   /** Reserved */
	unsigned int res_9[3];	/* 0x00000234 */
   /** Queue Occupancy Table Register 0
       This register provides access to Queue Occupancy Status and Configuration.
	*/
	unsigned int qoct0;	/* 0x00000240 */
   /** Queue Occupancy Table Register 1
       This register provides access to Instantaneous Queue Occupancy Counter
	*/
	unsigned int qoct1;	/* 0x00000244 */
   /** Queue Occupancy Table Register 2
       This register provides access to Average Queue Occupancy Counter
	*/
	unsigned int qoct2;	/* 0x00000248 */
   /** Reserved */
	unsigned int res_10;	/* 0x0000024C */
   /** Queue Discard Counter Table Register 0
       This register provides access to Queue Discard Counters.
	*/
	unsigned int qdct0;	/* 0x00000250 */
   /** Queue Discard Counter Table Register 1
       This register provides access to Queue Discard Counters.
	*/
	unsigned int qdct1;	/* 0x00000254 */
   /** Queue Discard Counter Table Register 2
       This register provides access to Queue Discard Counters.
	*/
	unsigned int qdct2;	/* 0x00000258 */
   /** Queue Discard Counter Table Register 3
       This register provides access to Queue Discard Counters.
	*/
	unsigned int qdct3;	/* 0x0000025C */
   /** Queue FIFO Manager Table Register 0
       This register provides access to Queue FIFO Occupancy Status.
	*/
	unsigned int qfmt0;	/* 0x00000260 */
   /** Queue FIFO Manager Table Register 1
       This register provides access to Queue FIFO Head and Tail.
	*/
	unsigned int qfmt1;	/* 0x00000264 */
   /** Queue FIFO Manager Table Register 2
       This register provides access to Queue FIFO Head PDU Attributes
	*/
	unsigned int qfmt2;	/* 0x00000268 */
   /** Reserved */
	unsigned int res_11;	/* 0x0000026C */
   /** Queue Manager Table Control Register
       This register provides indirect read / write control to the each of the 6 Queue related memories.
	*/
	unsigned int qmtc;	/* 0x00000270 */
   /** Reserved */
	unsigned int res_12[35];	/* 0x00000274 */
   /** EP Mapping Table Register
       This register provides (indirect) access to the port configuration parameters in EPMT.
	*/
	unsigned int epmt;	/* 0x00000300 */
   /** Reserved */
	unsigned int res_13[3];	/* 0x00000304 */
   /** Egress Port Occupancy Table Register 0
       This register provides (indirect) access to the port occupancy status.
	*/
	unsigned int epot0;	/* 0x00000310 */
   /** Egress Port Occupancy Table Register 1
       This register provides (indirect) access to the port occupancy status.
	*/
	unsigned int epot1;	/* 0x00000314 */
   /** Reserved */
	unsigned int res_14[2];	/* 0x00000318 */
   /** Egress Port Threshold Table Register 0
       This register provides (indirect) access to the port threshold configuration
	*/
	unsigned int eptt0;	/* 0x00000320 */
   /** Egress Port Threshold Table Register 1
       This register provides (indirect) access to the port threshold configuration
	*/
	unsigned int eptt1;	/* 0x00000324 */
   /** Reserved */
	unsigned int res_15[2];	/* 0x00000328 */
   /** Egress Port Discard Table Register 0
       This register provides (indirect) access to the port specific PDU discard counters.
	*/
	unsigned int epdt0;	/* 0x00000330 */
   /** Egress Port Discard Table Register 1
       This register provides (indirect) access to the port specific PDU discard counters.
	*/
	unsigned int epdt1;	/* 0x00000334 */
   /** Egress Port Discard Table Register 2
       This register provides (indirect) access to the port specific PDU discard counters.
	*/
	unsigned int epdt2;	/* 0x00000338 */
   /** Egress Port Discard Table Register 3
       This register provides (indirect) access to the port specific PDU discard counters.
	*/
	unsigned int epdt3;	/* 0x0000033C */
   /** EP Mapping Table Control Register
       This register provides the controls to the indirect access to EPMT.
	*/
	unsigned int epmtc;	/* 0x00000340 */
   /** Reserved */
	unsigned int res_16[47];	/* 0x00000344 */
   /** SB Input Table Register 0
       This register provides (indirect) access to the SBIT.
	*/
	unsigned int sbitr0;	/* 0x00000400 */
   /** SB Input Table Register 1
       This register provides (indirect) access to the SBIT.
	*/
	unsigned int sbitr1;	/* 0x00000404 */
   /** SB Input Table Register 2
       This register provides (indirect) access to the SBIT.
	*/
	unsigned int sbitr2;	/* 0x00000408 */
   /** SB Input Table Register 3
       This register provides (indirect) access to the SBIT.
	*/
	unsigned int sbitr3;	/* 0x0000040C */
   /** SB Input Table Control Register
       This register provides the controls to the indirect access to SBIT.
	*/
	unsigned int sbitc;	/* 0x00000410 */
   /** Reserved */
	unsigned int res_17[3];	/* 0x00000414 */
   /** SB Output Table Register 0
       This register provides (indirect) access to the configuration parameters of SBOT.
	*/
	unsigned int sbotr0;	/* 0x00000420 */
   /** SB Output Table Register 1
       This register provides (indirect) access to the status variables of SBOT.
	*/
	unsigned int sbotr1;	/* 0x00000424 */
   /** Reserved */
	unsigned int res_18[2];	/* 0x00000428 */
   /** SB Output Table Control Register
       This register provides the controls to the indirect access to SBOT.
	*/
	unsigned int sbotc;	/* 0x00000430 */
   /** Reserved */
	unsigned int res_19[51];	/* 0x00000434 */
   /** Token Bucket Shaper Table Register 0
       This register provides (indirect) access to configuration parameters of TBST.
	*/
	unsigned int tbstr0;	/* 0x00000500 */
   /** Token Bucket Shaper Table Register 1
       This register provides (indirect) access to configuration parameters of TBST.
	*/
	unsigned int tbstr1;	/* 0x00000504 */
   /** Token Bucket Shaper Table Register 2
       This register provides (indirect) access to configuration parameters of TBST.
	*/
	unsigned int tbstr2;	/* 0x00000508 */
   /** Reserved */
	unsigned int res_20;	/* 0x0000050C */
   /** Token Bucket Shaper Table Register 3
       This register provides (indirect) access to MBS parameters of TBST.
	*/
	unsigned int tbstr3;	/* 0x00000510 */
   /** Token Bucket Shaper Table Register 4
       This register provides (indirect) access to MBS parameters of TBST.
	*/
	unsigned int tbstr4;	/* 0x00000514 */
   /** Reserved */
	unsigned int res_21[2];	/* 0x00000518 */
   /** Token Bucket Shaper Table Register 5
       This register provides (indirect) access to status variables of TBST.
	*/
	unsigned int tbstr5;	/* 0x00000520 */
   /** Token Bucket Shaper Table Register 6
       This register provides (indirect) access to status variables of TBST.
	*/
	unsigned int tbstr6;	/* 0x00000524 */
   /** Reserved */
	unsigned int res_22[2];	/* 0x00000528 */
   /** Token Bucket Shaper Table Register 7
       This register provides (indirect) access to status variables of TBST.
	*/
	unsigned int tbstr7;	/* 0x00000530 */
   /** Token Bucket Shaper Table Register 8
       This register provides (indirect) access to status variables of TBST.
	*/
	unsigned int tbstr8;	/* 0x00000534 */
   /** Reserved */
	unsigned int res_23[2];	/* 0x00000538 */
   /** Token Bucket Shaper Table Register 9
       This register provides (indirect) access to status variables of TBST.
	*/
	unsigned int tbstr9;	/* 0x00000540 */
   /** Token Bucket Shaper Table Register 10
       This register provides (indirect) access to status variables of TBST.
	*/
	unsigned int tbstr10;	/* 0x00000544 */
   /** Reserved */
	unsigned int res_24[2];	/* 0x00000548 */
   /** Token Bucket Shaper Table Control Register
       This register provides the controls to the indirect access to TBST.
	*/
	unsigned int tbstc;	/* 0x00000550 */
   /** Reserved */
	unsigned int res_25[3];	/* 0x00000554 */
   /** Token Accumulation Period Register
       This register controls Token Acccumulation Period
	*/
	unsigned int tacper;	/* 0x00000560 */
   /** Reserved */
	unsigned int res_26[39];	/* 0x00000564 */
   /** PDU Pointer Table Register 0
       This register holds parameters for accepted PDUs
	*/
	unsigned int ppt0;	/* 0x00000600 */
   /** PDU Pointer Table Register 1
       This register holds parameters for accepted PDUs
	*/
	unsigned int ppt1;	/* 0x00000604 */
   /** PDU Pointer Table Register 2
       This register holds parameters for accepted PDUs
	*/
	unsigned int ppt2;	/* 0x00000608 */
   /** PDU Pointer Table Register 3
       This register holds parameters for accepted PDUs
	*/
	unsigned int ppt3;	/* 0x0000060C */
   /** PDU Pointer Table Control
       This register provides indirect read / write control to the PPT memory.
	*/
	unsigned int pptc;	/* 0x00000610 */
   /** Reserved */
	unsigned int res_27[59];	/* 0x00000614 */
   /** SBIN IWGT Set Data Register
       This register provides the IWGT to be dynamically assigned to an SBIN
	*/
	unsigned int cfgarg0;	/* 0x00000700 */
   /** SBIN TBID Assign Data Register
       This register provides the TBID to be dynamically assigned to an SBIN
	*/
	unsigned int cfgarg1;	/* 0x00000704 */
   /** TBID Bucket MOD, TBU, SRM Parameter Data Register
       This register provides the TBU, and SRM to be dynamically set
	for a bucket within a TBID. The bucket itself is selected
	 by the SEL field in the corresponding control register
	*/
	unsigned int cfgarg2;	/* 0x00000708 */
   /** TBID Bucket MBS Parameter Data Register
       This register provides the TBU, and SRM to be dynamically
	 set for a bucket within a TBID. The bucket itself is selected
	 by the SEL field in the corresponding control register
	*/
	unsigned int cfgarg3;	/* 0x0000070C */
   /** Reserved */
	unsigned int res_28;	/* 0x00000710 */
   /** Overlay for SBIN related Configuration Commands
       This overlay provides the structure for the SBIN
	 reconfiguration commands. It is only valid for
	 the specific CMD Opcodes given below.
	*/
	unsigned int cfgsbin;	/* 0x00000714 */
   /** Overlay for TBID related Configuration Commands
       This overlay provides the structure for the TBID + TB
	 reconfiguration commands. It is only valid for the
	 specific CMD Opcodes given below.
	*/
	unsigned int cfgtbid;	/* 0x00000718 */
   /** Overlay for EPN related Configuration Commands
       This overlay provides the structure for the
	 Egress Port reconfiguration commands.
	 It is only valid for the specific CMD Opcodes given below.
	*/
	unsigned int cfgepn;	/* 0x0000071C */
   /** Configuration Command Register
       This generic register provides control for
	 the dynamic reconfiguration commands. See overlay
	 registers CMDSBIN, CMDSBID, CMDTBID,
	CMDEPN for the specific sizes and layout of the ID field
	*/
	unsigned int cfgcmd;	/* 0x00000720 */

   /** Reserved */
	unsigned int res_29[567];	/* 0x00000724 */
};

/* Fields of "Control Register" */
/** Maximum TBID
	This field specifies the Maximum TBID that can be used for shaping.
	Initially 255. Modify only when ACT = 0. Read only for SW after ACT = 1
*/
#define TMU_CTRL_MAXTB_MASK 0x0000FF00
/** field offset */
#define TMU_CTRL_MAXTB_OFFSET 8
/** Relog Pipeline Sequential
	This bit enables the sequential mode of the Relog Pipeline,
	as a fallback solution
*/
#define TMU_CTRL_RPS 0x00000010
/* Relog Pipeline Sequential disabled
#define TMU_CTRL_RPS_RPS0 0x00000000
*/
/** Relog Pipeline Sequential enabled */
#define TMU_CTRL_RPS_RPS1 0x00000010
/** Disable Token Accumulation
    This bit disables the Token Bucket Accumulation Crawler, for debug purpose
*/
#define TMU_CTRL_DTA 0x00000004
/* Token Accumulation is enabled
#define TMU_CTRL_DTA_DTA0 0x00000000
*/
/** Token Accumulation is disabled */
#define TMU_CTRL_DTA_DTA1 0x00000004
/** Freeze Core State Machine
    This bit freezes the TMU state machines, for debug purpose
*/
#define TMU_CTRL_FRZ 0x00000002
/* Disable
#define TMU_CTRL_FRZ_DIS 0x00000000
*/
/** Enable */
#define TMU_CTRL_FRZ_EN 0x00000002
/** Activate Core State Machine
	This bit enables the TMU state machines. Initially 0.
	As long as ACT = 0, all table fields can be
	written. After ACT = 1, fields marked rwh are read only for SW
*/
#define TMU_CTRL_ACT 0x00000001
/* Disable
#define TMU_CTRL_ACT_DIS 0x00000000
*/
/** Enable */
#define TMU_CTRL_ACT_EN 0x00000001

/* Fields of "Free Pointer List" */
/** Tail Free PDU Pointer
    Address of the PPL PDU Pointer at the tail of the free list
*/
#define TMU_FPL_TFPP_MASK 0x3FFF0000
/** field offset */
#define TMU_FPL_TFPP_OFFSET 16
/** Head Free PDU Pointer
    Address of the PPL PDU Pointer at the head of the free list
*/
#define TMU_FPL_HFPP_MASK 0x00003FFF
/** field offset */
#define TMU_FPL_HFPP_OFFSET 0

/* Fields of "Free Pointer Counter Register" */
/** Free Pointer Counter
	This register holds the number of free PDU pointers in the PPT,
	after activation read only for SW
*/
#define TMU_FPCR_FPC_MASK 0x00003FFF
/** field offset */
#define TMU_FPCR_FPC_OFFSET 0

/* Fields of "Free Pointer Threshold Register" */
/** Free Pointer Threshold
	This register holds the FPL threshold from where the TMU
	returns NIL pointers and rejects PDU inserts
*/
#define TMU_FPTHR_FPTH_MASK 0x00003FFF
/** field offset */
#define TMU_FPTHR_FPTH_OFFSET 0

/* Fields of "Timer counter" */
/** Current TBID processed by the TAC
	This field reflects the TBID thai is currently processed
	by the Token Accumulation Crawler
*/
#define TMU_TIMER_TACTBID_MASK 0xFF000000
/** field offset */
#define TMU_TIMER_TACTBID_OFFSET 24
/** TBS Timer
	This timer is used to measure the elapesed time since the last
	token accumulation
*/
#define TMU_TIMER_TIME_MASK 0x0000FFFF
/** field offset */
#define TMU_TIMER_TIME_OFFSET 0

/* Fields of "Linear Feedback Shift Register" */
/** Random Number
	This field gives a random pattern, interpreted
	as a fractional binary between 0 and 1
*/
#define TMU_LFSR_RN_MASK 0x0000FFFF
/** field offset */
#define TMU_LFSR_RN_OFFSET 0

/* Fields of "Crawler Period Register" */
/** Crawler Period
    The WRED crawler period indicates the time interval to
	elapse between QAVG calculations on consecutive queues.
	It is specified with a granularity of 64 clocks (1024 clocks on A12).
	All 256 queues are updated in a round. This means that a
	period of 16384 clock cycles elapses between QAVG updates on
	the same queue. A value of 0 means that crawler is
	disabled and the QAVG values remain unchanged
*/
#define TMU_CPR_CP_MASK 0x000000FF
/** field offset */
#define TMU_CPR_CP_OFFSET 0

/* Fields of "Crawler Status Register" */
/** Crawler Count
    This gives the current value of the down counter in WRED crawler
*/
#define TMU_CSR_CNT_MASK 0xFFFFFF00
/** field offset */
#define TMU_CSR_CNT_OFFSET 8
/** Last QID Served
    This field gives the last Queue ID served by the WRED crawler
*/
#define TMU_CSR_LQS_MASK 0x000000FF
/** field offset */
#define TMU_CSR_LQS_OFFSET 0

/* Fields of "Global Occupancy Register" */
/** Global Occupancy
    This register defines the global TMU fill level in units of segments
*/
#define TMU_GOCCR_GOCC_MASK 0x00007FFF
/** field offset */
#define TMU_GOCCR_GOCC_OFFSET 0

/* Fields of "Global Occupancy Threshold Register 0" */
/** Global Occupancy Threshold 0
    When fill level GOCC raises above this threshold,
	PDUs with color 0 are discarded. Threshold in units of segments.
*/
#define TMU_GOTHR0_GOTH0_MASK 0x00007FFF
/** field offset */
#define TMU_GOTHR0_GOTH0_OFFSET 0

/* Fields of "Global Occupancy Threshold Register 1" */
/** Global Occupancy Threshold 1
    When fill level GOCC raises above this threshold, PDUs with color 1 are
	discarded.Threshold in units of segments.
*/
#define TMU_GOTHR1_GOTH1_MASK 0x00007FFF
/** field offset */
#define TMU_GOTHR1_GOTH1_OFFSET 0

/* Fields of "Global Occupancy Threshold Register 2" */
/** Global Occupancy Threshold 2
    When fill level GOCC raises above this threshold, PDUs with color 2
	are discarded. Threshold in units of segments.
*/
#define TMU_GOTHR2_GOTH2_MASK 0x00007FFF
/** field offset */
#define TMU_GOTHR2_GOTH2_OFFSET 0

/* Fields of "Global Occupancy Threshold Register 3" */
/** Global Occupancy Threshold 3
    When fill level GOCC raises above this threshold, PDUs with color 3 are
	discarded. Threshold in units of segments.
*/
#define TMU_GOTHR3_GOTH3_MASK 0x00007FFF
/** field offset */
#define TMU_GOTHR3_GOTH3_OFFSET 0

/* Fields of "Global PDU Discard Counter Register 0" */
/** Global PDU Discard Counter 0
    Counts PDUs with color 0 discarded because a global
	threshold was exceeded in the BAARoll over at max.
*/
#define TMU_GPDCR0_GPDC0_MASK 0xFFFFFFFF
/** field offset */
#define TMU_GPDCR0_GPDC0_OFFSET 0

/* Fields of "Global PDU Discard Counter Register 1" */
/** Global PDU Discard Counter 1
    Counts PDUs with color 1 discarded because a global threshold
	was exceeded in the BAARoll over at max.
*/
#define TMU_GPDCR1_GPDC1_MASK 0xFFFFFFFF
/** field offset */
#define TMU_GPDCR1_GPDC1_OFFSET 0

/* Fields of "Global PDU Discard Counter Register 2" */
/** Global PDU Discard Counter 2
    Counts PDUs with color 2 discarded because a global threshold was exceeded in the BAARoll over at max.
*/
#define TMU_GPDCR2_GPDC2_MASK 0xFFFFFFFF
/** field offset */
#define TMU_GPDCR2_GPDC2_OFFSET 0

/* Fields of "Global PDU Discard Counter Register 3" */
/** Global PDU Discard Counter 3
    Counts PDUs with color 3 discarded because a global threshold was exceeded in the BAARoll over at max.
*/
#define TMU_GPDCR3_GPDC3_MASK 0xFFFFFFFF
/** field offset */
#define TMU_GPDCR3_GPDC3_OFFSET 0

/* Fields of "Queue Fill Status Register 0" */
/** Queue Fill Status Bit for queues 0 to 31
    Each bit shows the fill status of the corresponding queue. 0: queue is not filled, 1: queue is filled
*/
#define TMU_QFILL0_QFS0_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QFILL0_QFS0_OFFSET 0

/* Fields of "Queue Fill Status Register 1" */
/** Queue Fill Status Bit for queues 32 to 63
    Each bit shows the fill status of the corresponding queue. 0: queue is not filled, 1: queue is filled
*/
#define TMU_QFILL1_QFS1_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QFILL1_QFS1_OFFSET 0

/* Fields of "Queue Fill Status Register 2" */
/** Queue Fill Status Bit for queues 64 to 95
    Each bit shows the fill status of the corresponding queue. 0: queue is not filled, 1: queue is filled
*/
#define TMU_QFILL2_QFS2_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QFILL2_QFS2_OFFSET 0

/* Fields of "Queue Fill Status Register 3" */
/** Queue Fill Status Bit for queues 96 to 127
    Each bit shows the fill status of the corresponding queue. 0: queue is not filled, 1: queue is filled
*/
#define TMU_QFILL3_QFS3_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QFILL3_QFS3_OFFSET 0

/* Fields of "Queue Fill Status Register 4" */
/** Queue Fill Status Bit for queues 128 to 159
    Each bit shows the fill status of the corresponding queue. 0: queue is not filled, 1: queue is filled
*/
#define TMU_QFILL4_QFS4_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QFILL4_QFS4_OFFSET 0

/* Fields of "Queue Fill Status Register 5" */
/** Queue Fill Status Bit for queues 160 to 191
    Each bit shows the fill status of the corresponding queue. 0: queue is not filled, 1: queue is filled
*/
#define TMU_QFILL5_QFS5_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QFILL5_QFS5_OFFSET 0

/* Fields of "Queue Fill Status Register 6" */
/** Queue Fill Status Bit for queues 192 to 223
    Each bit shows the fill status of the corresponding queue. 0: queue is not filled, 1: queue is filled
*/
#define TMU_QFILL6_QFS6_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QFILL6_QFS6_OFFSET 0

/* Fields of "Queue Fill Status Register 7" */
/** Queue Fill Status Bit for queues 224 to 255
    Each bit shows the fill status of the corresponding queue. 0: queue is not filled, 1: queue is filled
*/
#define TMU_QFILL7_QFS7_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QFILL7_QFS7_OFFSET 0

/* Fields of "Egress Port Filled Status Register 0" */
/** Egress Port Filled Status 0
    Indicaes if egress ports in the range 0 to 31 are filled
*/
#define TMU_EPFR0_EPF0_MASK 0xFFFFFFFF
/** field offset */
#define TMU_EPFR0_EPF0_OFFSET 0

/* Fields of "Egress Port Filled Status Register 1" */
/** Egress Port Filled Status 1
    Indicaes if egress ports in the range 32 to 63 are filled
*/
#define TMU_EPFR1_EPF1_MASK 0xFFFFFFFF
/** field offset */
#define TMU_EPFR1_EPF1_OFFSET 0

/* Fields of "Egress Port Filled Status Register 2" */
/** Egress Port Filled Status 2
    Indicaes if egress ports in the range 64 to 71 are filled
*/
#define TMU_EPFR2_EPF2_MASK 0x000000FF
/** field offset */
#define TMU_EPFR2_EPF2_OFFSET 0

/* Fields of "Low Power Idle Configuration for LAN port 0" */
/** Threshold
    Indicates the port occupancy threshold in units of segments where power should be switched on immediately. THX = 0 disables the LPI mechanism on this port, effective in a permanent setting of LPI_request = 0
*/
#define TMU_LPIC0_THX_MASK 0xFF000000
/** field offset */
#define TMU_LPIC0_THX_OFFSET 24
/** Timer OFF
    Indicates the maximum time period (in units of 4096 clocks) that may elapse from the event of data arriving to an empty port until LPI_request is deasserted
*/
#define TMU_LPIC0_TOF_MASK 0x00FFF000
/** field offset */
#define TMU_LPIC0_TOF_OFFSET 12
/** Timer ON
    Indicates the maximum time period (in units of 4096 clocks) that may elapse from the event of a port getting empty until LPI_request is asserted
*/
#define TMU_LPIC0_TON_MASK 0x00000FFF
/** field offset */
#define TMU_LPIC0_TON_OFFSET 0

/* Fields of "Low Power Idle Configuration for LAN port 1" */
/** Threshold
    Indicates the port occupancy threshold in units of segments where power should be switched on immediately. THX = 0 disables the LPI mechanism on this port, effective in a permanent setting of LPI_request = 0
*/
#define TMU_LPIC1_THX_MASK 0xFF000000
/** field offset */
#define TMU_LPIC1_THX_OFFSET 24
/** Timer OFF
    Indicates the maximum time period (in units of 4096 clocks) that may elapse from the event of data arriving to an empty port until LPI_request is deasserted
*/
#define TMU_LPIC1_TOF_MASK 0x00FFF000
/** field offset */
#define TMU_LPIC1_TOF_OFFSET 12
/** Timer ON
    Indicates the maximum time period (in units of 4096 clocks) that may elapse from the event of a port getting empty until LPI_request is asserted
*/
#define TMU_LPIC1_TON_MASK 0x00000FFF
/** field offset */
#define TMU_LPIC1_TON_OFFSET 0

/* Fields of "Low Power Idle Configuration for LAN port 2" */
/** Threshold
    Indicates the port occupancy threshold in units of segments where power should be switched on immediately. THX = 0 disables the LPI mechanism on this port, effective in a permanent setting of LPI_request = 0
*/
#define TMU_LPIC2_THX_MASK 0xFF000000
/** field offset */
#define TMU_LPIC2_THX_OFFSET 24
/** Timer OFF
    Indicates the maximum time period (in units of 4096 clocks) that may elapse from the event of data arriving to an empty port until LPI_request is deasserted
*/
#define TMU_LPIC2_TOF_MASK 0x00FFF000
/** field offset */
#define TMU_LPIC2_TOF_OFFSET 12
/** Timer ON
    Indicates the maximum time period (in units of 4096 clocks) that may elapse from the event of a port getting empty until LPI_request is asserted
*/
#define TMU_LPIC2_TON_MASK 0x00000FFF
/** field offset */
#define TMU_LPIC2_TON_OFFSET 0

/* Fields of "Low Power Idle Configuration for LAN port 3" */
/** Threshold
    Indicates the port occupancy threshold in units of segments where power should be switched on immediately. THX = 0 disables the LPI mechanism on this port, effective in a permanent setting of LPI_request = 0
*/
#define TMU_LPIC3_THX_MASK 0xFF000000
/** field offset */
#define TMU_LPIC3_THX_OFFSET 24
/** Timer OFF
    Indicates the maximum time period (in units of 4096 clocks) that may elapse from the event of data arriving to an empty port until LPI_request is deasserted
*/
#define TMU_LPIC3_TOF_MASK 0x00FFF000
/** field offset */
#define TMU_LPIC3_TOF_OFFSET 12
/** Timer ON
    Indicates the maximum time period (in units of 4096 clocks) that may elapse from the event of a port getting empty until LPI_request is asserted
*/
#define TMU_LPIC3_TON_MASK 0x00000FFF
/** field offset */
#define TMU_LPIC3_TON_OFFSET 0

/* Fields of "Low Power Idle Timer Status 0" */
/** LPI State
    Buffer fill status, triggers LPI_request change after timer elapse, after activation read only for SW
*/
#define TMU_LPIT0_LPSTATE 0x80000000
/* Buffer filled, when timer elapses LPI_request is deasserted
#define TMU_LPIT0_LPSTATE_OFF 0x00000000
*/
/** Buffer empty, when timer elapses LPI_request is asserted */
#define TMU_LPIT0_LPSTATE_ON 0x80000000
/** LPI Request
    Mirrors the state of the LPI_request signal sent to the Interfaces, after activation read only for SW
*/
#define TMU_LPIT0_LPIREQ 0x40000000
/** Low Power Timer
    Timer Variable, decremented every clock. Stops at 0 and triggers LPI_timer actions, after activation read only for SW
*/
#define TMU_LPIT0_LPTIMER_MASK 0x00FFFFFF
/** field offset*/
#define TMU_LPIT0_LPTIMER_OFFSET 0

/* Fields of "Low Power Idle Timer Status 1" */
/** LPI State
    Buffer fill status, triggers LPI_request change after timer elapse, after activation read only for SW
*/
#define TMU_LPIT1_LPSTATE 0x80000000
/* Buffer filled, when timer elapses LPI_request is deasserted
#define TMU_LPIT1_LPSTATE_OFF 0x00000000
*/
/** Buffer empty, when timer elapses LPI_request is asserted */
#define TMU_LPIT1_LPSTATE_ON 0x80000000
/** LPI Request
    Mirrors the state of the LPI_request signal sent to the Interfaces, after activation read only for SW
*/
#define TMU_LPIT1_LPIREQ 0x40000000
/** Low Power Timer
    Timer Variable, decremented every clock. Stops at 0 and triggers LPI_timer actions, after activation read only for SW
*/
#define TMU_LPIT1_LPTIMER_MASK 0x00FFFFFF
/** field offset */
#define TMU_LPIT1_LPTIMER_OFFSET 0

/* Fields of "Low Power Idle Timer Status 2" */
/** LPI State
    Buffer fill status, triggers LPI_request change after timer elapse, after activation read only for SW
*/
#define TMU_LPIT2_LPSTATE 0x80000000
/* Buffer filled, when timer elapses LPI_request is deasserted
#define TMU_LPIT2_LPSTATE_OFF 0x00000000
*/
/** Buffer empty, when timer elapses LPI_request is asserted */
#define TMU_LPIT2_LPSTATE_ON 0x80000000
/** LPI Request
    Mirrors the state of the LPI_request signal sent to the Interfaces,
after activation read only for SW
*/
#define TMU_LPIT2_LPIREQ 0x40000000
/** Low Power Timer
    Timer Variable, decremented every clock. Stops at 0 and triggers
LPI_timer actions, after activation read only for SW
*/
#define TMU_LPIT2_LPTIMER_MASK 0x00FFFFFF
/** field offset */
#define TMU_LPIT2_LPTIMER_OFFSET 0

/* Fields of "Low Power Idle Timer Status 3"
*/
/** LPI State
    Buffer fill status, triggers LPI_request change after timer elapse,
after activation read only for SW
*/
#define TMU_LPIT3_LPSTATE 0x80000000
/* Buffer filled, when timer elapses LPI_request is deasserted
#define TMU_LPIT3_LPSTATE_OFF 0x00000000
*/
/** Buffer empty, when timer elapses LPI_request is asserted */
#define TMU_LPIT3_LPSTATE_ON 0x80000000
/** LPI Request
    Mirrors the state of the LPI_request signal sent to the Interfaces,
after activation read only for SW
*/
#define TMU_LPIT3_LPIREQ 0x40000000
/** Low Power Timer
    Timer Variable, decremented every clock. Stops at 0 and triggers
LPI_timer actions, after activation read only for SW
*/
#define TMU_LPIT3_LPTIMER_MASK 0x00FFFFFF
/** field offset */
#define TMU_LPIT3_LPTIMER_OFFSET 0

/* Fields of "Enqueue Request Delay Register" */
/** Enqueue Request Delay
Determines an additional internal delay in number of clock cycles between
acceptance of an Enqueue Request by the TMU and the time the
TMU is ready to receive the next Enqueue Request.
*/
#define TMU_ERDR_ERD_MASK 0x0000FFFF
/** field offset */
#define TMU_ERDR_ERD_OFFSET 0

/* Fields of "Token Accumulation Disable Slot Period Register" */
/** Token Accumulation Disable Slot Period
    Determines period in number of clock cycles whenever a slot is disabled
internally for token accumulation
*/
#define TMU_TADSPR_TADSP_MASK 0x0000FFFF
/** field offset */
#define TMU_TADSPR_TADSP_OFFSET 0

/* Fields of "Control Debug Register" */
/** Stop Token Accumulation TBID counter
    By writing a 1 to this bit, token accumulation crawler is stopped
*/
#define TMU_CTRLDBG_STACTBID 0x00000800
/** Stop Token Accumulation Timer
    By writing a 1 to this bit, token accumulation timer is stopped
*/
#define TMU_CTRLDBG_STACT 0x00000400
/** Step Token Accumulation
    By writing a 1 to this bit, the token accumulation crawler is stepped
for one iteration, if debug mode is enabled with TDBG
*/
#define TMU_CTRLDBG_STEPTAC 0x00000200
/** Token Accumulation Debug
    Enables token accumulation debugging mode, holds the token accumulation
pipeline. Stepping by writing to STEPTAC bit
*/
#define TMU_CTRLDBG_TDBG 0x00000100
/* Token Accumulation Debug disabled
#define TMU_CTRLDBG_TDBG_TDBG0 0x00000000
*/
/** Token Accumulation Debug enabled */
#define TMU_CTRLDBG_TDBG_TDBG1 0x00000100
/** Disable CPU Relog
    By writing a 1 to this bit, the scheduler relog pipeline is disabled
for CPU Relog Requests
*/
#define TMU_CTRLDBG_DRELCPU 0x00000080
/** Disable Token Accumulation Relog
    By writing a 1 to this bit, the scheduler relog pipeline is disabled for
Token Accumulation Requests
*/
#define TMU_CTRLDBG_DRELTAC 0x00000040
/** Disable Schedule Relog
    By writing a 1 to this bit, the scheduler relog pipeline is disabled for
Schedule Requests
*/
#define TMU_CTRLDBG_DRELDEQ 0x00000020
/** Disable Enqueue Relog
    By writing a 1 to this bit, the scheduler relog pipeline is disabled for
Enqueue Requests
*/
#define TMU_CTRLDBG_DRELENQ 0x00000010
/** Select Relog Request Fifo
    Determines the selection of the relog request fifo
*/
#define TMU_CTRLDBG_SELREL_MASK 0x0000000C
/** field offset */
#define TMU_CTRLDBG_SELREL_OFFSET 2
/** Schedule Request Relogs only */
#define TMU_CTRLDBG_SELREL_SELREL_0 0x00000000
/** Enqueue Request Relogs only */
#define TMU_CTRLDBG_SELREL_SELREL_1 0x00000004
/** CPU Request Relogs only */
#define TMU_CTRLDBG_SELREL_SELREL_2 0x00000008
/** Token Accumulation Request Relogs only */
#define TMU_CTRLDBG_SELREL_SELREL_3 0x0000000C
/** Step Relog
By writing a 1 to this bit, the scheduler relog pipeline is
stepped for one iteration, if debug mode is enabled with RDBG
*/
#define TMU_CTRLDBG_STEPREL 0x00000002
/** Relog Pipeline Debug
Enables relog debugging mode, holds the scheduler relog pipeline.
Stepping by writing to STEPREL bit
*/
#define TMU_CTRLDBG_RDBG 0x00000001
/* Relog Pipeline Debug disabled
#define TMU_CTRLDBG_RDBG_RDBG0 0x00000000
*/
/** Relog Pipeline Debug enabled */
#define TMU_CTRLDBG_RDBG_RDBG1 0x00000001

/* Fields of "Scheduler Relog Control Register" */
/** Disable fix for dynamic reconfiguration
This bit disables A21 fix for dynamic reconfiguration
(multiple scheduler hierarchies) and keeps A11 chip compatibility
behavior.Do not use except in case of scheduler relog
problems (fallback solution)!
*/
#define TMU_RELOGCTRL_DISFRECFG 0x00020000
/* Fix for dynamic reconfiguration enabled
#define TMU_RELOGCTRL_DISFRECFG_DISFRECFG0 0x00000000
*/
/** Fix for dynamic reconfiguration disabled */
#define TMU_RELOGCTRL_DISFRECFG_DISFRECFG1 0x00020000
/** Disable relog memories lock signals
This bit disables the memory lock signals for read-modify-write cycles
in the scheduler relog pipeline.Do not use except in case of
scheduler relog problems (fallback solution)!
*/
#define TMU_RELOGCTRL_DISMLOCK 0x00010000
/* Relog memories lock signals enabled
#define TMU_RELOGCTRL_DISMLOCK_DISMLOCK0 0x00000000
*/
/** Relog memories lock signals disabled */
#define TMU_RELOGCTRL_DISMLOCK_DISMLOCK1 0x00010000
/** Full Threshold for relog input FIFO token accumulation
(Crawler Relog Requests) If the number of Token Accumulation
Crawler Relog Requests in the corresponding FIFO equals or exceeds
this value, token accumulation updates are delayed
*/
#define TMU_RELOGCTRL_TACFTHR_MASK 0x00000F00
/** field offset */
#define TMU_RELOGCTRL_TACFTHR_OFFSET 8
/** Full Threshold for relog input FIFO dequeue (Schedule Relog Requests)
If the number of Schedule Relog Requests in the corresponding FIFO equals
or exceeds this value, backpressure is given to the
OCTRL (ready signal deasserted)
*/
#define TMU_RELOGCTRL_DEQFTHR_MASK 0x00000007
/** field offset */
#define TMU_RELOGCTRL_DEQFTHR_OFFSET 0

/* Fields of "IRN Capture Register" */
/** SRC1 Negative Limit
    This bit is set if the Sustainable Rate Counter 1 (SRC1) of a Token Bucket reaches the negative limit 0x3FFFFThe TBID of the token bucket where this happens is captured in the capture register TBIDCR This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_SRC1N 0x02000000
/* Nothing
#define TMU_IRNCR_SRC1N_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_SRC1N_INTACK 0x02000000
/** Read: Interrupt occurred. */
#define TMU_IRNCR_SRC1N_INTOCC 0x02000000
/** SRC0 Negative Limit
    This bit is set if the Sustainable Rate Counter 0 (SRC0) of a Token Bucket reaches the negative limit 0x3FFFFThe TBID of the token bucket where this happens is captured in the capture register TBIDCR This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_SRC0N 0x01000000
/* Nothing
#define TMU_IRNCR_SRC0N_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_SRC0N_INTACK 0x01000000
/** Read: Interrupt occurred. */
#define TMU_IRNCR_SRC0N_INTOCC 0x01000000
/** CPU Command Relog Overflow ==> Not Applicable
    This bit is set if a CPU command driven relog overflow occurs at the relog arbiter. The bit is reset by SW. This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_CRO 0x00800000
/* Nothing
#define TMU_IRNCR_CRO_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_CRO_INTACK 0x00800000
/** Read: Interrupt occurred. */
#define TMU_IRNCR_CRO_INTOCC 0x00800000
/** Token Accumulation Relog Overflow ==> Not Applicable
    This bit is set if a token accumulation driven relog overflow occurs at the relog arbiter. The bit is reset by SW. This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_TRO 0x00400000
/* Nothing
#define TMU_IRNCR_TRO_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_TRO_INTACK 0x00400000
/** Read: Interrupt occurred. */
#define TMU_IRNCR_TRO_INTOCC 0x00400000
/** Enqueue Relog Overflow ==> Not Applicable
    This bit is set if a enqueue driven relog overflow occurs at the relog arbiter. The bit is reset by SW. This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_ERO 0x00200000
/* Nothing
#define TMU_IRNCR_ERO_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_ERO_INTACK 0x00200000
/** Read: Interrupt occurred. */
#define TMU_IRNCR_ERO_INTOCC 0x00200000
/** Dequeue Relog Overflow
    This bit is set if a dequeue driven relog overflow occurs at the relog arbiter. The bit is reset by SW. This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_DRO 0x00100000
/* Nothing
#define TMU_IRNCR_DRO_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_DRO_INTACK 0x00100000
/** Read: Interrupt occurred. */
#define TMU_IRNCR_DRO_INTOCC 0x00100000
/** SB NIL Result Error
    This bit is set if the Schedule_Request command returned a NIL LSA as result. The bit is reset by SW. This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_NIL 0x00040000
/* Nothing
#define TMU_IRNCR_NIL_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_NIL_INTACK 0x00040000
/** Read: Interrupt occurred. */
#define TMU_IRNCR_NIL_INTOCC 0x00040000
/** VDT Overflow Error
    This bit is set if the VDT update results in a value greater than 0x3FFFF for VDT. The bit is reset by SW. In this case VDT value is forced to 0xFFFF This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_VOE 0x00020000
/* Nothing
#define TMU_IRNCR_VOE_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_VOE_INTACK 0x00020000
/** Read: Interrupt occurred. */
#define TMU_IRNCR_VOE_INTOCC 0x00020000
/** Egress CPU Port 3 Filled
    This bit is set if the Egress CPU Port 3 is filled. The bit is reset by SW. This bit is level-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_EPFC3 0x00000800
/* Nothing
#define TMU_IRNCR_EPFC3_NULL 0x00000000
*/
/** Read: Interrupt occurred. */
#define TMU_IRNCR_EPFC3_INTOCC 0x00000800
/** Egress CPU Port 2 Filled
    This bit is set if the Egress CPU Port 2 is filled. The bit is reset by SW. This bit is level-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_EPFC2 0x00000400
/* Nothing
#define TMU_IRNCR_EPFC2_NULL 0x00000000
*/
/** Read: Interrupt occurred. */
#define TMU_IRNCR_EPFC2_INTOCC 0x00000400
/** Egress CPU Port 1 Filled
    This bit is set if the Egress CPU Port 1 is filled. The bit is reset by SW. This bit is level-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_EPFC1 0x00000200
/* Nothing
#define TMU_IRNCR_EPFC1_NULL 0x00000000
*/
/** Read: Interrupt occurred. */
#define TMU_IRNCR_EPFC1_INTOCC 0x00000200
/** Egress CPU Port 0 Filled
    This bit is set if the Egress CPU Port 0 is filled. The bit is reset by SW. This bit is level-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_EPFC0 0x00000100
/* Nothing
#define TMU_IRNCR_EPFC0_NULL 0x00000000
*/
/** Read: Interrupt occurred. */
#define TMU_IRNCR_EPFC0_INTOCC 0x00000100
/** Global Occupancy threshold 3 Exceeded
    This bit is set if the Global Occupancy Counter (GOCC) exceeds the Global Occupancy Threshold GOTH[3] and a PDU is discarded for this reason. The bit is reset by SW. This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_GOE3 0x00000080
/* Nothing
#define TMU_IRNCR_GOE3_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_GOE3_INTACK 0x00000080
/** Read: Interrupt occurred. */
#define TMU_IRNCR_GOE3_INTOCC 0x00000080
/** Global Occupancy threshold 2 Exceeded
    This bit is set if the Global Occupancy Counter (GOCC) exceeds the Global Occupancy Threshold GOTH[2] and a PDU is discarded for this reason. The bit is reset by SW. This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_GOE2 0x00000040
/* Nothing
#define TMU_IRNCR_GOE2_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_GOE2_INTACK 0x00000040
/** Read: Interrupt occurred. */
#define TMU_IRNCR_GOE2_INTOCC 0x00000040
/** Global Occupancy threshold 1 Exceeded
    This bit is set if the Global Occupancy Counter (GOCC) exceeds the Global Occupancy Threshold GOTH[1] and a PDU is discarded for this reason. The bit is reset by SW. This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_GOE1 0x00000020
/* Nothing
#define TMU_IRNCR_GOE1_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_GOE1_INTACK 0x00000020
/** Read: Interrupt occurred. */
#define TMU_IRNCR_GOE1_INTOCC 0x00000020
/** Global Occupancy threshold 0 Exceeded
    This bit is set if the Global Occupancy Counter (GOCC) exceeds the Global Occupancy Threshold GOTH[0] and a PDU is discarded for this reason. The bit is reset by SW. This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_GOE0 0x00000010
/* Nothing
#define TMU_IRNCR_GOE0_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_GOE0_INTACK 0x00000010
/** Read: Interrupt occurred. */
#define TMU_IRNCR_GOE0_INTOCC 0x00000010
/** Free Pointer Underflow
    This bit is set if the Free Pointer Counter (FPC) is less or equal to the Free Pointer Threshold (FPTH). The bit is reset by SW. This bit is level-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_FPU 0x00000002
/* Nothing
#define TMU_IRNCR_FPU_NULL 0x00000000
*/
/** Read: Interrupt occurred. */
#define TMU_IRNCR_FPU_INTOCC 0x00000002
/** PDU Not Accepted
    This bit is set if the Buffer Acceptance Algorithm does not accept a PDU for any reason (INFO != 0). The bit is reset by SW.The INFO of the Enqueue Request is captured in the capture register ERINFOCR This bit is edge-sensitive. This bit contributes to the indirect interrupt.
*/
#define TMU_IRNCR_PNA 0x00000001
/* Nothing
#define TMU_IRNCR_PNA_NULL 0x00000000
*/
/** Write: Acknowledge the interrupt. */
#define TMU_IRNCR_PNA_INTACK 0x00000001
/** Read: Interrupt occurred. */
#define TMU_IRNCR_PNA_INTOCC 0x00000001

/* Fields of "IRN Interrupt Control Register" */
/** SRC1 Negative Limit
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_SRC1N 0x02000000
/** SRC0 Negative Limit
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_SRC0N 0x01000000
/** CPU Command Relog Overflow ==> Not Applicable
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_CRO 0x00800000
/** Token Accumulation Relog Overflow ==> Not Applicable
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_TRO 0x00400000
/** Enqueue Relog Overflow ==> Not Applicable
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_ERO 0x00200000
/** Dequeue Relog Overflow
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_DRO 0x00100000
/** SB NIL Result Error
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_NIL 0x00040000
/** VDT Overflow Error
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_VOE 0x00020000
/** Egress CPU Port 3 Filled
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_EPFC3 0x00000800
/** Egress CPU Port 2 Filled
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_EPFC2 0x00000400
/** Egress CPU Port 1 Filled
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_EPFC1 0x00000200
/** Egress CPU Port 0 Filled
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_EPFC0 0x00000100
/** Global Occupancy threshold 3 Exceeded
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_GOE3 0x00000080
/** Global Occupancy threshold 2 Exceeded
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_GOE2 0x00000040
/** Global Occupancy threshold 1 Exceeded
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_GOE1 0x00000020
/** Global Occupancy threshold 0 Exceeded
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_GOE0 0x00000010
/** Free Pointer Underflow
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_FPU 0x00000002
/** PDU Not Accepted
    Interrupt control bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNICR_PNA 0x00000001

/* Fields of "IRN Interrupt Enable Register"
*/
/** SRC1 Negative Limit
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_SRC1N 0x02000000
/* Disable
#define TMU_IRNEN_SRC1N_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_SRC1N_EN 0x02000000
/** SRC0 Negative Limit
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_SRC0N 0x01000000
/* Disable
#define TMU_IRNEN_SRC0N_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_SRC0N_EN 0x01000000
/** CPU Command Relog Overflow ==> Not Applicable
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_CRO 0x00800000
/* Disable
#define TMU_IRNEN_CRO_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_CRO_EN 0x00800000
/** Token Accumulation Relog Overflow ==> Not Applicable
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_TRO 0x00400000
/* Disable
#define TMU_IRNEN_TRO_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_TRO_EN 0x00400000
/** Enqueue Relog Overflow ==> Not Applicable
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_ERO 0x00200000
/* Disable
#define TMU_IRNEN_ERO_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_ERO_EN 0x00200000
/** Dequeue Relog Overflow
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_DRO 0x00100000
/* Disable
#define TMU_IRNEN_DRO_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_DRO_EN 0x00100000
/** SB NIL Result Error
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_NIL 0x00040000
/* Disable
#define TMU_IRNEN_NIL_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_NIL_EN 0x00040000
/** VDT Overflow Error
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_VOE 0x00020000
/* Disable
#define TMU_IRNEN_VOE_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_VOE_EN 0x00020000
/** Egress CPU Port 3 Filled
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_EPFC3 0x00000800
/* Disable
#define TMU_IRNEN_EPFC3_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_EPFC3_EN 0x00000800
/** Egress CPU Port 2 Filled
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_EPFC2 0x00000400
/* Disable
#define TMU_IRNEN_EPFC2_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_EPFC2_EN 0x00000400
/** Egress CPU Port 1 Filled
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_EPFC1 0x00000200
/* Disable
#define TMU_IRNEN_EPFC1_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_EPFC1_EN 0x00000200
/** Egress CPU Port 0 Filled
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_EPFC0 0x00000100
/* Disable
#define TMU_IRNEN_EPFC0_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_EPFC0_EN 0x00000100
/** Global Occupancy threshold 3 Exceeded
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_GOE3 0x00000080
/* Disable
#define TMU_IRNEN_GOE3_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_GOE3_EN 0x00000080
/** Global Occupancy threshold 2 Exceeded
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_GOE2 0x00000040
/* Disable
#define TMU_IRNEN_GOE2_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_GOE2_EN 0x00000040
/** Global Occupancy threshold 1 Exceeded
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_GOE1 0x00000020
/* Disable
#define TMU_IRNEN_GOE1_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_GOE1_EN 0x00000020
/** Global Occupancy threshold 0 Exceeded
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_GOE0 0x00000010
/* Disable
#define TMU_IRNEN_GOE0_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_GOE0_EN 0x00000010
/** Free Pointer Underflow
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_FPU 0x00000002
/* Disable
#define TMU_IRNEN_FPU_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_FPU_EN 0x00000002
/** PDU Not Accepted
    Interrupt enable bit for the corresponding bit in the IRNCR register.
*/
#define TMU_IRNEN_PNA 0x00000001
/* Disable
#define TMU_IRNEN_PNA_DIS 0x00000000
*/
/** Enable */
#define TMU_IRNEN_PNA_EN 0x00000001

/* Fields of "Token Bucket ID Capture Register" */
/** Token Bucket Identifier Captured
    TBID captured in case of SRC0N or SRC1N interrupts
*/
#define TMU_TBIDCR_TBIDC_MASK 0x00007FFF
/** field offset */
#define TMU_TBIDCR_TBIDC_OFFSET 0

/* Fields of "Enqueue Request INFO Capture Register" */
/** Enqueue Request INFO Return value
    INFO field in Enqueue Request captured in case of PNA interrupts
*/
#define TMU_ERINFOCR_INFO_MASK 0x0000000F
/** field offset */
#define TMU_ERINFOCR_INFO_OFFSET 0

/* Fields of "Queue Egress Mapping Table Register" */
/** Egress Port Number
    Indicates the egress port where traffic from this queue will be transmitted.The value is used for port occupancy and discard counting. All valid queues should have a EPN configured before activation. Valid Range 0:71. NIL value 0x7F
*/
#define TMU_QEMT_EPN_MASK 0x0000007F
/** field offset */
#define TMU_QEMT_EPN_OFFSET 0

/* Fields of "Queue Scheduler Mapping Table Register" */
/** Scheduler Block Input Number
    Index into the SB Input Table (SBIT). Indicates the SB Input to which the queue is connected to.It is possible to modify this assignment after activation, however care must be taken to assure the queue is disabled and empty.
*/
#define TMU_QSMT_SBIN_MASK 0x000003FF
/** field offset */
#define TMU_QSMT_SBIN_OFFSET 0

/* Fields of "Queue Threshold Table Register 0" */
/** Queue Enable
    This bit must be set to enable admission of PDUs into the queue
*/
#define TMU_QTHT0_QE 0x80000000
/* Disable
#define TMU_QTHT0_QE_DIS 0x00000000
*/
/** Enable */
#define TMU_QTHT0_QE_EN 0x80000000
/** Dropping Mode
    This field selects the dropping mode of the queue as tail drop (0) or WRED (1)
*/
#define TMU_QTHT0_DMOD 0x00010000
/* Tail Drop
#define TMU_QTHT0_DMOD_TD 0x00000000
*/
/** WRED */
#define TMU_QTHT0_DMOD_WR 0x00010000
/** Threshold index for Color 3 (red)
    Selects the threshold for the a given color according to the following conventiondefault: 0
*/
#define TMU_QTHT0_COL3_MASK 0x00003000
/** field offset */
#define TMU_QTHT0_COL3_OFFSET 12
/** Index 0 fixed to QTTH0 */
#define TMU_QTHT0_COL3_QTTH0 0x00000000
/** Index 1 fixed to QTTH1 */
#define TMU_QTHT0_COL3_QTTH1 0x00001000
/** Index 2 fixed to MATH0 */
#define TMU_QTHT0_COL3_MATH0 0x00002000
/** Index 3 fixed to MATH1 */
#define TMU_QTHT0_COL3_MATH1 0x00003000
/** Threshold index for Color 2 (yellow)
    Selects the threshold for the a given color according to the following conventiondefault: 1
*/
#define TMU_QTHT0_COL2_MASK 0x00000300
/** field offset */
#define TMU_QTHT0_COL2_OFFSET 8
/** Index 0 fixed to QTTH0 */
#define TMU_QTHT0_COL2_QTTH0 0x00000000
/** Index 1 fixed to QTTH1 */
#define TMU_QTHT0_COL2_QTTH1 0x00000100
/** Index 2 fixed to MATH0 */
#define TMU_QTHT0_COL2_MATH0 0x00000200
/** Index 3 fixed to MATH1 */
#define TMU_QTHT0_COL2_MATH1 0x00000300
/** Threshold index for Color 1 (green)
    Selects the threshold for the a given color according to the following conventiondefault: 2
*/
#define TMU_QTHT0_COL1_MASK 0x00000030
/** field offset */
#define TMU_QTHT0_COL1_OFFSET 4
/** Index 0 fixed to QTTH0 */
#define TMU_QTHT0_COL1_QTTH0 0x00000000
/** Index 1 fixed to QTTH1 */
#define TMU_QTHT0_COL1_QTTH1 0x00000010
/** Index 2 fixed to MATH0 */
#define TMU_QTHT0_COL1_MATH0 0x00000020
/** Index 3 fixed to MATH1 */
#define TMU_QTHT0_COL1_MATH1 0x00000030
/** Threshold index for Color 0 (unassigned)
    Selects the threshold for the a given color according to the following conventiondefault: 3
*/
#define TMU_QTHT0_COL0_MASK 0x00000003
/** field offset */
#define TMU_QTHT0_COL0_OFFSET 0
/** Index 0 fixed to QTTH0 */
#define TMU_QTHT0_COL0_QTTH0 0x00000000
/** Index 1 fixed to QTTH1 */
#define TMU_QTHT0_COL0_QTTH1 0x00000001
/** Index 2 fixed to MATH0 */
#define TMU_QTHT0_COL0_MATH0 0x00000002
/** Index 3 fixed to MATH1 */
#define TMU_QTHT0_COL0_MATH1 0x00000003

/* Fields of "Queue Threshold Table Register 1" */
/** Minimum Threshold of WRED Curve 1
    Minimum threshold of WRED curve 1 in units of 8 segments.
*/
#define TMU_QTHT1_MITH1_MASK 0x0FFF0000
/** field offset */
#define TMU_QTHT1_MITH1_OFFSET 16
/** Minimum Threshold of WRED Curve 0
    Minimum threshold of WRED curve 0 in units of 8 segments.
*/
#define TMU_QTHT1_MITH0_MASK 0x00000FFF
/** field offset */
#define TMU_QTHT1_MITH0_OFFSET 0

/* Fields of "Queue Threshold Table Register 2" */
/** Maximum Threshold of WRED Curve 1, also used as Queue Tail Drop Threshold 3
    Maximum threshold of WRED curve 1, or tail drop for threshold index 3, in units of 8 segments. Above this threshold, arriving PDUs with matching color are discarded without further check. Highest functional value is 2304, as queue occupies complete SSB
*/
#define TMU_QTHT2_MATH1_MASK 0x0FFF0000
/** field offset */
#define TMU_QTHT2_MATH1_OFFSET 16
/** Maximum Threshold of WRED Curve 0, also used as Queue Tail Drop Threshold 2
Maximum threshold of WRED curve 0, or tail drop for threshold index 2, in units
of 8 segments. Above this threshold, arriving PDUs with matching color are
discarded without further check. Highest functional value is 2304, as
queue occupies complete SSB
*/
#define TMU_QTHT2_MATH0_MASK 0x00000FFF
/** field offset */
#define TMU_QTHT2_MATH0_OFFSET 0

/* Fields of "Queue Threshold Table Register 3" */
/** Slope of WRED Curve 1
Slope is defined by the point where the WRED curve crosses MATH1,
it is given by 12 bit which represent a fraction of the probability range 0..1.
*/
#define TMU_QTHT3_SLOPE1_MASK 0x0FFF0000
/** field offset */
#define TMU_QTHT3_SLOPE1_OFFSET 16
/** Slope of WRED Curve 0
Slope is defined by the point where the WRED curve crosses MATH0, it is given
by 12 bit which represent a fraction of the probability range 0..1.
*/
#define TMU_QTHT3_SLOPE0_MASK 0x00000FFF
/** field offset */
#define TMU_QTHT3_SLOPE0_OFFSET 0

/* Fields of "Queue Threshold Table Register 4" */
/** Queue Tail Drop Threshold 1
Tail drop for threshold index 1 in units of 8 segments.
Above this threshold, arriving PDUs with matching color are discarded without
further check.Highest functional value is 2304, as queue occupies complete SSB
*/
#define TMU_QTHT4_QTTH1_MASK 0x0FFF0000
/** field offset */
#define TMU_QTHT4_QTTH1_OFFSET 16
/** Queue Tail Drop Threshold 0
Tail drop for threshold index 0 in units of 8 segments. Above this threshold,
arriving PDUs with matching color are discarded without further check.Highest
functional value is 2304, as queue occupies complete SSB
*/
#define TMU_QTHT4_QTTH0_MASK 0x00000FFF
/** field offset */
#define TMU_QTHT4_QTTH0_OFFSET 0

/* Fields of "Queue Occupancy Table Register 0" */
/** Weight
a weighting factor of 2**(-WQ) is used in the calculation of the average queue
length QAVG used for WRED. The QAVG calculation itself is done periodically on
each queue. The calculation interval can be adjusted by the global parameter
CP in register CPR
*/
#define TMU_QOCT0_WQ_MASK 0x000F0000
/** field offset */
#define TMU_QOCT0_WQ_OFFSET 16
/** Queue Reservation Threshold
this threshold specifies the minimum queue buffer reservation in units
of 8 segments. Below this threshold, PDUs are accepted without consideration
in the global shared buffer.It is possible to modify this threshold after
activation, but it is not recommended to do so unless the SSB is completely
emptied of stored PDUs and the ingress traffic to TMU is blocked
*/
#define TMU_QOCT0_QRTH_MASK 0x00000FFF
/** field offset */
#define TMU_QOCT0_QRTH_OFFSET 0

/* Fields of "Queue Occupancy Table Register 1" */
/** Queue Occupancy
Instantaneous queue fill level in units of segments, after
activation read only for SW
*/
#define TMU_QOCT1_QOCC_MASK 0x00007FFF
/** field offset */
#define TMU_QOCT1_QOCC_OFFSET 0

/* Fields of "Queue Occupancy Table Register 2" */
/** Queue Average
Average queue fill level in units of segments with 8 fractional bits,
as calculated for WRED, after activation read only for SW
*/
#define TMU_QOCT2_QAVG_MASK 0x007FFFFF
/** field offset */
#define TMU_QOCT2_QAVG_OFFSET 0

/* Fields of "Queue Discard Counter Table Register 0" */
/** Queue PDU Discard Counter for Color 0
Counts PDUs with color 0 discarded because a queue threshold was exceeded
in the BAARoll over at max, after activation read only for SW
*/
#define TMU_QDCT0_QDC0_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QDCT0_QDC0_OFFSET 0

/* Fields of "Queue Discard Counter Table Register 1" */
/** Queue PDU Discard Counter for Color 1
Counts PDUs with color 1 discarded because a queue threshold was exceeded
in the BAARoll over at max, after activation read only for SW
*/
#define TMU_QDCT1_QDC1_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QDCT1_QDC1_OFFSET 0

/* Fields of "Queue Discard Counter Table Register 2" */
/** Queue PDU Discard Counter for Color 2
Counts PDUs with color 2 discarded because a queue threshold was exceeded
in the BAARoll over at max, after activation read only for SW
*/
#define TMU_QDCT2_QDC2_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QDCT2_QDC2_OFFSET 0

/* Fields of "Queue Discard Counter Table Register 3" */
/** Queue PDU Discard Counter for Color 3
Counts PDUs with color 3 discarded because a queue threshold was exceeded
in the BAARoll over at max, after activation read only for SW
*/
#define TMU_QDCT3_QDC3_MASK 0xFFFFFFFF
/** field offset */
#define TMU_QDCT3_QDC3_OFFSET 0

/* Fields of "Queue FIFO Manager Table Register 0" */
/** Queue Filled
This status bit indicates if the queue is filled or not. Initially, for the
empty queue this bit is set to 0, after activation read only for SW
*/
#define TMU_QFMT0_QF 0x80000000
/** PDU occupancy
Instantaneous queue fill level in units of PDUs, after activation read only for SW
*/
#define TMU_QFMT0_POCC_MASK 0x3FFF0000
/** field offset */
#define TMU_QFMT0_POCC_OFFSET 16

/* Fields of "Queue FIFO Manager Table Register 1" */
/** Tail Queue PDU Pointer
Index into the PPT where the PDU Pointer of the PDU at the tail of the
queue is located, after activation read only for SW
*/
#define TMU_QFMT1_TQPP_MASK 0x3FFF0000
/** field offset */
#define TMU_QFMT1_TQPP_OFFSET 16
/** Head Queue PDU Pointer
Index into the PPT where the PDU Pointer of the PDU at the head of the
queue is located, after activation read only for SW
*/
#define TMU_QFMT1_HQPP_MASK 0x00003FFF
/** field offset */
#define TMU_QFMT1_HQPP_OFFSET 0

/* Fields of "Queue FIFO Manager Table Register 2" */
/** QOS Length
Internally assigned PDU length in bytes used for QoS guarantees
in scheduling and shaping, after activation read only for SW
*/
#define TMU_QFMT2_QOSL_MASK 0xFFFF0000
/** field offset */
#define TMU_QFMT2_QOSL_OFFSET 16
/** PDU Color
    PDU Color, after activation read only for SW
*/
#define TMU_QFMT2_COL_MASK 0x00003000
/** field offset */
#define TMU_QFMT2_COL_OFFSET 12
/** color unassigned */
#define TMU_QFMT2_COL_COL_0 0x00000000
/** color green */
#define TMU_QFMT2_COL_COL_1 0x00001000
/** color yellow */
#define TMU_QFMT2_COL_COL_2 0x00002000
/** color red */
#define TMU_QFMT2_COL_COL_3 0x00003000
/** Segment Length
    PDU Length in segments, after activation read only for SW
*/
#define TMU_QFMT2_SEGL_MASK 0x000003FF
/** field offset */
#define TMU_QFMT2_SEGL_OFFSET 0

/* Fields of "Queue Manager Table Control Register" */
/** QFMT Valid Indicator
This bit defines valid results in the QFMT, SW shall wait with a
subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_QMTC_QFV 0x40000000
/** QDCT Valid Indicator
This bit defines valid results in the QDCT, SW shall wait with a
subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_QMTC_QDV 0x20000000
/** QOCT Configuration Command Valid Indicator
This bit indicates validity of the access to the WQ and QRTH
configuration fields of QOCT, SW shall wait with a subsequent
access until the bit is switched on (=1) by HW
*/
#define TMU_QMTC_QCV 0x10000000
/** QOCT Valid Indicator
This bit defines valid results in the QOCT, SW shall wait with a
subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_QMTC_QOV 0x08000000
/** QTHT Valid Indicator
This bit defines valid results in the QTHT, SW shall wait with a
subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_QMTC_QTV 0x04000000
/** QSMT Valid Indicator
This bit defines valid results in the QSMT, SW shall wait with a
subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_QMTC_QSV 0x02000000
/** QEMT Valid Indicator
This bit defines valid results in the QEMT, SW shall wait with a
subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_QMTC_QEV 0x01000000
/** QFMT Read Selector
    This bit defines read access to the QFMT
*/
#define TMU_QMTC_QFR 0x00400000
/** QDCT Read Selector
    This bit defines read access to the QDCT
*/
#define TMU_QMTC_QDR 0x00200000
/** QOCT Configuration Read Selector
    This bit defines read access to the WQ and QRTH configuration fields of QOCT
*/
#define TMU_QMTC_QCR 0x00100000
/** QOCT Read Selector
    This bit defines read access to the QOCT
*/
#define TMU_QMTC_QOR 0x00080000
/** QTHT Read Selector
    This bit defines read access to the QTHT
*/
#define TMU_QMTC_QTR 0x00040000
/** QSMT Read Selector
    This bit defines read access to the QSMT
*/
#define TMU_QMTC_QSR 0x00020000
/** QEMT Read Selector
    This bit defines read access to the QEMT
*/
#define TMU_QMTC_QER 0x00010000
/** QFMT Write Selector
    This bit defines write access to the QFMT
*/
#define TMU_QMTC_QFW 0x00004000
/** QDCT Write Selector
    This bit defines write access to the QDCT
*/
#define TMU_QMTC_QDW 0x00002000
/** QOCT Configuration Write Selector
This bit defines write access to the WQ and QRTH configuration fields of
QOCT
*/
#define TMU_QMTC_QCW 0x00001000
/** QOCT Write Selector
This bit defines write access to the QOCT for both configuration
and status fields.
*/
#define TMU_QMTC_QOW 0x00000800
/** QTHT Write Selector
    This bit defines write access to the QTHT
*/
#define TMU_QMTC_QTW 0x00000400
/** QSMT Write Selector
    This bit defines write access to the QSMT
*/
#define TMU_QMTC_QSW 0x00000200
/** QEMT Write Selector
    This bit defines write access to the QEMT
*/
#define TMU_QMTC_QEW 0x00000100
/** Queue Identifier
    Index into the QMT. This value to address the queue in the QMT.
*/
#define TMU_QMTC_QID_MASK 0x000000FF
/** field offset */
#define TMU_QMTC_QID_OFFSET 0

/* Fields of "EP Mapping Table Register" */
/** Egress Port Enable
This bit indicates if the Egress Port is enabled for transmission.
If disabled, the egress port filled status is seen as 0 by the outside world
*/
#define TMU_EPMT_EPE 0x80000000
/* Disable
#define TMU_EPMT_EPE_DIS 0x00000000
*/
/** Enable */
#define TMU_EPMT_EPE_EN 0x80000000
/** Scheduler Block Identifier
This filed gives the Identifier of a SB which is attached to this entryIt is
possible to modify this assignment after activation, however care must be
taken to assure the port ans all assigned queues are disabled and empty.
*/
#define TMU_EPMT_SBID_MASK 0x0000007F
/** field offset */
#define TMU_EPMT_SBID_OFFSET 0

/* Fields of "Egress Port Occupancy Table Register 0" */
/** Egress Port Occupancy Counter 1
Instantaneous port specific fill level in segments for color 1 (green),
after activation read only for SW
*/
#define TMU_EPOT0_EPOC1_MASK 0x7FFF0000
/** field offset */
#define TMU_EPOT0_EPOC1_OFFSET 16
/** Egress Port Occupancy Counter 0
Instantaneous port specific fill level in segments for color 0 (unassigned),
after activation read only for SW
*/
#define TMU_EPOT0_EPOC0_MASK 0x00007FFF
/** field offset */
#define TMU_EPOT0_EPOC0_OFFSET 0

/* Fields of "Egress Port Occupancy Table Register 1" */
/** Egress Port Occupancy Counter 3
Instantaneous port specific fill level in segments for color 3 (red),
after activation read only for SW
*/
#define TMU_EPOT1_EPOC3_MASK 0x7FFF0000
/** field offset */
#define TMU_EPOT1_EPOC3_OFFSET 16
/** Egress Port Occupancy Counter 2
Instantaneous port specific fill level in segments for color 2 (yellow),
after activation read only for SW
*/
#define TMU_EPOT1_EPOC2_MASK 0x00007FFF
/** field offset */
#define TMU_EPOT1_EPOC2_OFFSET 0

/* Fields of "Egress Port Threshold Table Register 0" */
/** Egress Port Discard Threshold 1
    Discard threshold in units of 8 segments for color 1 (green)
*/
#define TMU_EPTT0_EPTH1_MASK 0x0FFF0000
/** field offset */
#define TMU_EPTT0_EPTH1_OFFSET 16
/** Egress Port Discard Threshold 0
    Discard threshold in units of 8 segments for color 0 (unassigned)
*/
#define TMU_EPTT0_EPTH0_MASK 0x00000FFF
/** field offset */
#define TMU_EPTT0_EPTH0_OFFSET 0

/* Fields of "Egress Port Threshold Table Register 1" */
/** Egress Port Discard Threshold 3
    Discard threshold in units of 8 segments for color 3 (red)
*/
#define TMU_EPTT1_EPTH3_MASK 0x0FFF0000
/** field offset */
#define TMU_EPTT1_EPTH3_OFFSET 16
/** Egress Port Discard Threshold 2
    Discard threshold in units of 8 segments for color 2 (yellow)
*/
#define TMU_EPTT1_EPTH2_MASK 0x00000FFF
/** field offset */
#define TMU_EPTT1_EPTH2_OFFSET 0

/* Fields of "Egress Port Discard Table Register 0" */
/** Egress Port Discard Counter 0
Counts PDUs with color 0 discarded because a port threshold was exceeded
in the BAARoll over at max. After activation read only for SW
*/
#define TMU_EPDT0_EPDC0_MASK 0xFFFFFFFF
/** field offset */
#define TMU_EPDT0_EPDC0_OFFSET 0

/* Fields of "Egress Port Discard Table Register 1" */
/** Egress Port Discard Counter 1
Counts PDUs with color 1 discarded because a port threshold was exceeded
in the BAARoll over at max. After activation read only for SW
*/
#define TMU_EPDT1_EPDC1_MASK 0xFFFFFFFF
/** field offset */
#define TMU_EPDT1_EPDC1_OFFSET 0

/* Fields of "Egress Port Discard Table Register 2" */
/** Egress Port Discard Counter 2
Counts PDUs with color 2 discarded because a port threshold was exceeded
in the BAARoll over at max. After activation read only for SW
*/
#define TMU_EPDT2_EPDC2_MASK 0xFFFFFFFF
/** field offset */
#define TMU_EPDT2_EPDC2_OFFSET 0

/* Fields of "Egress Port Discard Table Register 3" */
/** Egress Port Discard Counter 3
Counts PDUs with color 3 discarded because a port threshold was exceeded
in the BAARoll over at max. After activation read only for SW
*/
#define TMU_EPDT3_EPDC3_MASK 0xFFFFFFFF
/** field offset */
#define TMU_EPDT3_EPDC3_OFFSET 0

/* Fields of "EP Mapping Table Control Register" */
/** Egress Port Discard Valid Indicator
This bit defines the validity of the read access to EPDT, SW shall
wait with a subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_EPMTC_EDV 0x08000000
/** Egress Port Threshold Valid Indicator
This bit defines the validity of the read access to EPTT,
SW shall wait with a subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_EPMTC_ETV 0x04000000
/** Egress Port Occupancy Valid Indicator
This bit defines the validity of the read access to EPOT, SW shall wait
with a subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_EPMTC_EOV 0x02000000
/** Egress Port Mapping Valid Indicator
This bit defines the validity of the read access to EPMT, SW shall wait
with a subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_EPMTC_EMV 0x01000000
/** Egress Port Discard Read Selector
    This bit defines the indirect read access to EPDT
*/
#define TMU_EPMTC_EDR 0x00080000
/** Egress Port Threshold Read Selector
    This bit defines the indirect read access to EPTT
*/
#define TMU_EPMTC_ETR 0x00040000
/** Egress Port Occupancy Read Selector
    This bit defines the indirect read access to EPOT
*/
#define TMU_EPMTC_EOR 0x00020000
/** Egress Port Mapping Read Selector
    This bit defines the indirect read access to EPMT
*/
#define TMU_EPMTC_EMR 0x00010000
/** Egress Port Discard Write Selector
    This bit defines the indirect write access to EPDT
*/
#define TMU_EPMTC_EDW 0x00000800
/** Egress Port Threshold Write Selector
    This bit defines the indirect write access to EPTT
*/
#define TMU_EPMTC_ETW 0x00000400
/** Egress Port Occupancy Write Selector
    This bit defines the indirect write access to EPOT
*/
#define TMU_EPMTC_EOW 0x00000200
/** Egress Port Mapping Write Selector
    This bit defines the indirect write access to EPMT
*/
#define TMU_EPMTC_EMW 0x00000100
/** Egress Port Number
    This field holds the Egress Port Number, used as an index into the EPMT
*/
#define TMU_EPMTC_EPN_MASK 0x0000007F
/** field offset */
#define TMU_EPMTC_EPN_OFFSET 0

/* Fields of "SB Input Table Register 0" */
/** Scheduler Input Enable
This bit indicates if the SB Input is enabled. If the SB input is
disabled, the entry does not participate in the scheduling
*/
#define TMU_SBITR0_SIE 0x80000000
/* Disable
#define TMU_SBITR0_SIE_DIS 0x00000000
*/
/** Enable */
#define TMU_SBITR0_SIE_EN 0x80000000
/** Inverse WFQ Weight
This gives the inverse weight of the SB input for WFQ scheduling.
IWGT=0 means strict priority
*/
#define TMU_SBITR0_IWGT_MASK 0x03FF0000
/** field offset */
#define TMU_SBITR0_IWGT_OFFSET 16
/** Scheduler Input Type
This bit indicates if the entity attached at the SB Input
is a Queue or a Scheduler BlockIt is possible to modify this assignment
after activation, however care must be taken to assure the assigned
queue or SB is disabled and completely empty.
*/
#define TMU_SBITR0_SIT 0x00008000
/* QSID identifies a Queue
#define TMU_SBITR0_SIT_QID 0x00000000
*/
/** QSID identifies a Scheduler Block */
#define TMU_SBITR0_SIT_SBID 0x00008000
/** Combined Queue/Scheduler Identifier
This field gives the Identifier of a queue or SB, whichever
is attached to this entry, depending on the SIT bitIt is possible to
modify this assignment after activation, however care must be taken to
assure the assigned queue or SB is disabled and completely empty.
*/
#define TMU_SBITR0_QSID_MASK 0x000000FF
/** field offset */
#define TMU_SBITR0_QSID_OFFSET 0

/* Fields of "SB Input Table Register 1" */
/** Token Bucket Enabled
    This bit indicates if the dual TB with TBID is enabled on this entry
*/
#define TMU_SBITR1_TBE 0x80000000
/* Disable
#define TMU_SBITR1_TBE_DIS 0x00000000
*/
/** Enable */
#define TMU_SBITR1_TBE_EN 0x80000000
/** Token Bucket Identifier
This field gives the TBID of the dual token bucket attached to
this input.It is possible to modify this assignment after
activation, however care must be taken to assure the assigned
queue or SB is disabled and completely empty.
*/
#define TMU_SBITR1_TBID_MASK 0x000000FF
/** field offset */
#define TMU_SBITR1_TBID_OFFSET 0

/* Fields of "SB Input Table Register 2" */
/** Scheduler Input Filled
This field gives the filled status of the Q/SB attached to this SB
input, after activation read only for SW
*/
#define TMU_SBITR2_SIF 0x80000000
/* Scheduler Input Empty
#define TMU_SBITR2_SIF_EMP 0x00000000
*/
/** Scheduler Input Filled */
#define TMU_SBITR2_SIF_FIL 0x80000000
/** Scheduler Input Passed
This field gives the passed status of the SB input,
after activation read only for SW
*/
#define TMU_SBITR2_SIP 0x40000000
/* Scheduler Input Passed = FALSE
#define TMU_SBITR2_SIP_SIP_0 0x00000000
*/
/** Scheduler Input Passed = TRUE */
#define TMU_SBITR2_SIP_SIP_1 0x40000000
/** Virtual Departure Time
This field defines the current VDT as it is calculated by
the WFQ algorithm, after activation read only for SW
*/
#define TMU_SBITR2_VDT_MASK 0x0003FFFF
/** field offset */
#define TMU_SBITR2_VDT_OFFSET 0

/* Fields of "SB Input Table Register 3" */
/** PDU Color
    PDU Color, after activation read only for SW
*/
#define TMU_SBITR3_COL_MASK 0x00030000
/** field offset */
#define TMU_SBITR3_COL_OFFSET 16
/** color unassigned */
#define TMU_SBITR3_COL_COL_0 0x00000000
/** color green */
#define TMU_SBITR3_COL_COL_1 0x00010000
/** color yellow */
#define TMU_SBITR3_COL_COL_2 0x00020000
/** color red */
#define TMU_SBITR3_COL_COL_3 0x00030000
/** QOS Length
Internally assigned PDU length in bytes used for QoS guarantees in
scheduling and shaping, after activation read only for SW
*/
#define TMU_SBITR3_QOSL_MASK 0x0000FFFF
/** field offset */
#define TMU_SBITR3_QOSL_OFFSET 0

/* Fields of "SB Input Table Control Register" */
/** Valid
This bit indicates that the CPU command has been completed and
the respond parameters are valid, SW shall wait with a subsequent access
until the bit is switched on (=1) by HW
*/
#define TMU_SBITC_VAL 0x00040000
/** Field Selector
    This bit defines the indirect access to configuration only or to all table fields
*/
#define TMU_SBITC_SEL 0x00020000
/* Only configuration fields (marked rw, not rwh) can be read / written in table
#define TMU_SBITC_SEL_SELCFG 0x00000000
*/
/** All table fields (marked rw and rwh) can be read / written */
#define TMU_SBITC_SEL_SELALL 0x00020000
/** Read/Write Selector
    This bit defines the indirect access to table, as read or write
*/
#define TMU_SBITC_RW 0x00010000
/* Read access to table registers
#define TMU_SBITC_RW_R 0x00000000
*/
/** Write access to table registers */
#define TMU_SBITC_RW_W 0x00010000
/** Scheduler Block Input Number
    This field holds the SB input number
*/
#define TMU_SBITC_SBIN_MASK 0x000003FF
/** field offset */
#define TMU_SBITC_SBIN_OFFSET 0

/* Fields of "SB Output Table Register 0" */
/** SB Output Enable
    Scheduler Block Output enable or disable.
*/
#define TMU_SBOTR0_SOE 0x80000000
/* Disable
#define TMU_SBOTR0_SOE_DIS 0x00000000
*/
/** Enable */
#define TMU_SBOTR0_SOE_EN 0x80000000
/** Scheduler Block Level
The level of this SB in the hierarchy. Starts with level 0 at
ingress side. Maximum is 8 levels.Note: A high level may cause performance
problems with high bandwidth.It is possible to modify this field after
activation, however care must be taken to assure the assigned queues or SBs
in the hierarchy below are disabled and completely empty.
*/
#define TMU_SBOTR0_LVL_MASK 0x00070000
/** field offset */
#define TMU_SBOTR0_LVL_OFFSET 16
/** Valid SB Input
Indicates that this SB is attached to another SB Input (SB) or
to an Egress Port (EPN).It is possible to modify this assignment after
activation, however care must be taken to assure the assigned queues or SBs
in the hierarchy below are disabled and completely empty.
*/
#define TMU_SBOTR0_V 0x00008000
/* if V = 0, OMID identifies an Egress Port
#define TMU_SBOTR0_V_EPN 0x00000000
*/
/** if V = 1, OMID identifies an SB Input */
#define TMU_SBOTR0_V_SBIN 0x00008000
/** Output Mapping Identifier
Identifies the SBIN of the next higher SB or the EPN of the egress
portIt is possible to modify this assignment after activation, however care
must be taken to assure the assigned queues or SBs in the hierarchy below
are disabled and completely empty.
*/
#define TMU_SBOTR0_OMID_MASK 0x000003FF
/** field offset */
#define TMU_SBOTR0_OMID_OFFSET 0
/** if V = 0, OMID identifies an Egress Port */
#define TMU_SBOTR0_OMID_EPN 0x00000000
/** if V = 1, OMID identifies an SB Input */
#define TMU_SBOTR0_OMID_SBIN 0x00000001

/* Fields of "SB Output Table Register 1" */
/** SB Output Filled
	This bit gives the status whether the SB is filled or not,
	after activation read only for SW
*/
#define TMU_SBOTR1_SOF 0x80000000
/* Empty: no data available
#define TMU_SBOTR1_SOF_EMP 0x00000000
*/
/** Filled: some data available */
#define TMU_SBOTR1_SOF_FIL 0x80000000
/** Winner Leaf
	This field defines the current winner leaf as it is calculated
	by the WFQ algorithm. Local SB input number 0..7,
	after activation read only for SW
*/
#define TMU_SBOTR1_WL_MASK 0x00070000
/** field offset */
#define TMU_SBOTR1_WL_OFFSET 16
/** Winner QID
	This field refers to the current winner QID at the leaf,
	after activation read only for SW
*/
#define TMU_SBOTR1_WQID_MASK 0x000000FF
/** field offset */
#define TMU_SBOTR1_WQID_OFFSET 0

/* Fields of "SB Output Table Control Register" */
/** Valid
	This bit indicates that the CPU command has been completed
	and the respond parameters are valid, SW shall wait with a subsequent
	access until the bit is switched on (=1) by HW
*/
#define TMU_SBOTC_VAL 0x00040000
/** Field Selector
	This bit defines the indirect access to configuration only or to
	all table fields
*/
#define TMU_SBOTC_SEL 0x00020000
/* Only configuration fields (marked rw, not rwh) can be read / written in table
#define TMU_SBOTC_SEL_SELCFG 0x00000000
*/
/** All table fields (marked rw and rwh) can be read / written */
#define TMU_SBOTC_SEL_SELALL 0x00020000
/** Read/Write Selector
    This bit defines the indirect access to table, as read or write
*/
#define TMU_SBOTC_RW 0x00010000
/* Read access to table registers
#define TMU_SBOTC_RW_R 0x00000000
*/
/** Write access to table registers */
#define TMU_SBOTC_RW_W 0x00010000
/** Scheduler Block Identifier
    This field holds the SB identifier
*/
#define TMU_SBOTC_SBID_MASK 0x0000007F
/** field offset */
#define TMU_SBOTC_SBID_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 0" */
/** Token Bucket Mode
    This field indicates the TB operating mode
*/
#define TMU_TBSTR0_MOD_MASK 0x30000000
/** field offset */
#define TMU_TBSTR0_MOD_OFFSET 28
/** strict color blind */
#define TMU_TBSTR0_MOD_MOD_0 0x00000000
/** RFC 2698 color aware */
#define TMU_TBSTR0_MOD_MOD_1 0x10000000
/** RFC 4115 color aware */
#define TMU_TBSTR0_MOD_MOD_2 0x20000000
/** loose color blind */
#define TMU_TBSTR0_MOD_MOD_3 0x30000000
/** Scheduler Block Input Number
This field holds the SB Input this TBS is connected toIt is possible to
modify this assignment after activation, however care must be taken to assure
the assigned queues or SBs in the hierarchy below are disabled and completely
empty and the reverse assignment is also made at the SBIT
*/
#define TMU_TBSTR0_SBIN_MASK 0x000003FF
/** field offset */
#define TMU_TBSTR0_SBIN_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 1" */
/** Token Bucket Enable 0
    This bit indicates if the TB 0 is enabled
*/
#define TMU_TBSTR1_TBE0 0x80000000
/* Disable
#define TMU_TBSTR1_TBE0_DIS 0x00000000
*/
/** Enable */
#define TMU_TBSTR1_TBE0_EN 0x80000000
/** Token Bucket Unit 0
    The number of Bytes that are added to the TBC0 with each SRC0 elapse
*/
#define TMU_TBSTR1_TBU0_MASK 0x30000000
/** field offset */
#define TMU_TBSTR1_TBU0_OFFSET 28
/** 1 Byte */
#define TMU_TBSTR1_TBU0_TBU_0 0x00000000
/** 16 Bytes */
#define TMU_TBSTR1_TBU0_TBU_1 0x10000000
/** 256 Bytes */
#define TMU_TBSTR1_TBU0_TBU_2 0x20000000
/** 1024 Bytes */
#define TMU_TBSTR1_TBU0_TBU_3 0x30000000
/** Accumulation Time for Token Bucket 0
	This field indicates the time between two consecutive accumulations of
	the bucket 0 in units of 16 clocks. Together with the token bucket unit
	it determines the shaper rate.
*/
#define TMU_TBSTR1_SRM0_MASK 0x00003FFF
/** field offset */
#define TMU_TBSTR1_SRM0_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 2" */
/** Token Bucket Enable 1
    This bit indicates if the TB 1 is enabled
*/
#define TMU_TBSTR2_TBE1 0x80000000
/* Disable
#define TMU_TBSTR2_TBE1_DIS 0x00000000
*/
/** Enable */
#define TMU_TBSTR2_TBE1_EN 0x80000000
/** Token Bucket Unit 1
    The number of Bytes that are added to the TBC1 with each SRC1 elapse
*/
#define TMU_TBSTR2_TBU1_MASK 0x30000000
/** field offset */
#define TMU_TBSTR2_TBU1_OFFSET 28
/** 1 Byte */
#define TMU_TBSTR2_TBU1_TBU_0 0x00000000
/** 16 Bytes */
#define TMU_TBSTR2_TBU1_TBU_1 0x10000000
/** 256 Bytes */
#define TMU_TBSTR2_TBU1_TBU_2 0x20000000
/** 1024 Bytes */
#define TMU_TBSTR2_TBU1_TBU_3 0x30000000
/** Accumulation Time for Token Bucket 1
	This field indicates the time between two consecutive accumulations of
	the bucket 1 in units of 16 clocks. Together with the token bucket unit
	it determines the shaper rate.
*/
#define TMU_TBSTR2_SRM1_MASK 0x00003FFF
/** field offset */
#define TMU_TBSTR2_SRM1_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 3" */
/** Maximum Bucket Size
    This field gives the Maximum Size of the Token Bucket 0 in bytes.
*/
#define TMU_TBSTR3_MBS0_MASK 0x00FFFFFF
/** field offset */
#define TMU_TBSTR3_MBS0_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 4" */
/** Maximum Bucket Size
    This field gives the Maximum Size of the Token Bucket 1 in bytes.
*/
#define TMU_TBSTR4_MBS1_MASK 0x00FFFFFF
/** field offset */
#define TMU_TBSTR4_MBS1_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 5" */
/** Shaper Rate Counter 0
	This field gives the current Shaper Rate Counter 0 value in clocks,
	after activation read only for SW
*/
#define TMU_TBSTR5_SRC0_MASK 0x0007FFFF
/** field offset */
#define TMU_TBSTR5_SRC0_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 6" */
/** Shaper Rate Counter 1
	This field gives the currrent Shaper Rate Counter 1 value in clocks,
	after activation read only for SW
*/
#define TMU_TBSTR6_SRC1_MASK 0x0007FFFF
/** field offset */
#define TMU_TBSTR6_SRC1_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 7" */
/** Token Bucket PASS0ED status
	This bit indicates if the Bucket has sufficient credits to pass
	the next PDU, after activation read only for SW
*/
#define TMU_TBSTR7_PASS0 0x80000000
/** Token Bucket Filling Level
	This field gives the current token bucket filling level in bytes,
	after activation read only for SW
*/
#define TMU_TBSTR7_TBC0_MASK 0x01FFFFFF
/** field offset */
#define TMU_TBSTR7_TBC0_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 8" */
/** Token Bucket PASS1ED status
	This bit indicates if the Bucket has sufficient credits to pass
	the next PDU, after activation read only for SW
*/
#define TMU_TBSTR8_PASS1 0x80000000
/** Token Bucket Filling Level
	This field gives the current token bucket filling level in bytes,
	after activation read only for SW
*/
#define TMU_TBSTR8_TBC1_MASK 0x01FFFFFF
/** field offset */
#define TMU_TBSTR8_TBC1_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 9" */
/** Color
	This field gives the COL of the PDU to pass this TBS next,
	after activation read only for SW
*/
#define TMU_TBSTR9_COL_MASK 0x00030000
/** field offset */
#define TMU_TBSTR9_COL_OFFSET 16
/** QOS Length
	This field gives the length (bytes) of the PDU to pass this TBS next,
	after activation read only for SW
*/
#define TMU_TBSTR9_QOSL_MASK 0x0000FFFF
/** field offset */
#define TMU_TBSTR9_QOSL_OFFSET 0

/* Fields of "Token Bucket Shaper Table Register 10" */
/** Timestamp
	This field gives the Timestamp of the last Token Accumulation,
	after activation read only for SW
*/
#define TMU_TBSTR10_TS_MASK 0x0000FFFF
/** field offset */
#define TMU_TBSTR10_TS_OFFSET 0

/* Fields of "Token Bucket Shaper Table Control Register" */
/** Valid
	This bit indicates that the CPU command has been completed
	and the respond parameters are valid, SW shall wait with a subsequent
	access until the bit is switched on (=1) by HW
*/
#define TMU_TBSTC_VAL 0x00040000
/* Field Selector
	This bit defines the indirect access to configuration only or to
	all table fields
*/
#define TMU_TBSTC_SEL 0x00020000
/* Only configuration fields (marked rw, not rwh) can be read / written in table
#define TMU_TBSTC_SEL_SELCFG 0x00000000
*/
/** All table fields (marked rw and rwh) can be read / written */
#define TMU_TBSTC_SEL_SELALL 0x00020000
/** Read/Write Selector
    This bit defines the indirect access to table, as read or write
*/
#define TMU_TBSTC_RW 0x00010000
/* Read access to table registers
#define TMU_TBSTC_RW_R 0x00000000
*/
/** Write access to table registers */
#define TMU_TBSTC_RW_W 0x00010000
/** Token Bucket Identifier
    This field holds the TB identifier
*/
#define TMU_TBSTC_TBID_MASK 0x000000FF
/** field offset */
#define TMU_TBSTC_TBID_OFFSET 0

/* Fields of "Token Accumulation Period Register" */
/** Token Accumulation Period
    This field holds the token accumulation crawler period in number
	of clock cycles between triggers to calculate at subsequent TBIDs
*/
#define TMU_TACPER_TACP_MASK 0x000000FF
/** field offset */
#define TMU_TACPER_TACP_OFFSET 0

/* Fields of "PDU Pointer Table Register 0" */
/** Next PDU pointer
    Pointer to next PDU, after activation read only for SW
*/
#define TMU_PPT0_PNEXT_MASK 0x3FFF0000
/** field offset */
#define TMU_PPT0_PNEXT_OFFSET 16
/** Offset for old PDU header
    Offset from start of old PDU header in bytes
*/
#define TMU_PPT0_OFFS_MASK 0x0000FF00
/** field offset */
#define TMU_PPT0_OFFS_OFFSET 8
/** Length of new PDU header
    Length of new PDU header in bytes
*/
#define TMU_PPT0_HDRL_MASK 0x000000FF
/** field offset */
#define TMU_PPT0_HDRL_OFFSET 0

/* Fields of "PDU Pointer Table Register 1" */
/** QOS Length
	Internally assigned PDU length in bytes used for QoS
	guarantees in scheduling and shaping, after activation read only for SW
*/
#define TMU_PPT1_QOSL_MASK 0xFFFF0000
/** field offset */
#define TMU_PPT1_QOSL_OFFSET 16
/** PDU Type*/
#define TMU_PPT1_PDUT_MASK 0x00007000
/** field offset */
#define TMU_PPT1_PDUT_OFFSET 12
/** Segment Length
    PDU Length in segments, after activation read only for SW
*/
#define TMU_PPT1_SEGL_MASK 0x000003FF
/** field offset */
#define TMU_PPT1_SEGL_OFFSET 0

/* Fields of "PDU Pointer Table Register 2" */
/** PDU Color
    PDU Color, after activation read only for SW
*/
#define TMU_PPT2_COL_MASK 0x03000000
/** field offset */
#define TMU_PPT2_COL_OFFSET 24
/** color unassigned */
#define TMU_PPT2_COL_COL_0 0x00000000
/** color green */
#define TMU_PPT2_COL_COL_1 0x01000000
/** color yellow */
#define TMU_PPT2_COL_COL_2 0x02000000
/** color red */
#define TMU_PPT2_COL_COL_3 0x03000000
/** GEM Port Index
    The GEM Port Index is mapped from the GEM Port Identifier in ICTRLx
*/
#define TMU_PPT2_GPIX_MASK 0x00FF0000
/** field offset */
#define TMU_PPT2_GPIX_OFFSET 16
/** PDU Body Length
    PDU Body Length in Bytes, after activation read only for SW
*/
#define TMU_PPT2_BDYL_MASK 0x0000FFFF
/** field offset */
#define TMU_PPT2_BDYL_OFFSET 0

/* Fields of "PDU Pointer Table Register 3" */
/** Tail LSA
	The logical segment address in SSB of the stored PDU tail,
	after activation read only for SW
*/
#define TMU_PPT3_TLSA_MASK 0x7FFF0000
/** field offset */
#define TMU_PPT3_TLSA_OFFSET 16
/** Head LSA
	The logical segment address in SSB of the
	stored PDU head, after activation read only for SW
*/
#define TMU_PPT3_HLSA_MASK 0x00007FFF
/** field offset */
#define TMU_PPT3_HLSA_OFFSET 0

/* Fields of "PDU Pointer Table Control" */
/** Valid
	This bit indicates that the CPU command has been completed
	and the respond parameters are valid, SW shall wait with a
	subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_PPTC_VAL 0x00020000
/** Read/Write Selector
    This bit defines the indirect access to table, as read or write
*/
#define TMU_PPTC_RW 0x00010000
/* Read access to table registers
#define TMU_PPTC_RW_R 0x00000000
*/
/** Write access to table registers */
#define TMU_PPTC_RW_W 0x00010000
/** PDU Pointer Table Index */
#define TMU_PPTC_PPTIND_MASK 0x00003FFF
/** field offset */
#define TMU_PPTC_PPTIND_OFFSET 0

/* Fields of "SBIN IWGT Set Data Register" */
/** Inverse WFQ Weight
	This gives the inverse weight of the SB input for
	WFQ scheduling. IWGT=0 means strict priority
*/
#define TMU_CFGARG0_IWGT_MASK 0x000003FF
/** field offset */
#define TMU_CFGARG0_IWGT_OFFSET 0

/* Fields of "SBIN TBID Assign Data Register" */
/** Token Bucket Identifier
    This field holds the TB identifier
*/
#define TMU_CFGARG1_TBID_MASK 0x000000FF
/** field offset */
#define TMU_CFGARG1_TBID_OFFSET 0

/* Fields of "TBID Bucket MOD, TBU, SRM Parameter Data Register" */
/** Token Bucket Mode
    This field indicates the TB operating mode
*/
#define TMU_CFGARG2_MOD_MASK 0xC0000000
/** field offset */
#define TMU_CFGARG2_MOD_OFFSET 30
/** strict color blind */
#define TMU_CFGARG2_MOD_MOD_0 0x00000000
/** RFC 2698 color aware */
#define TMU_CFGARG2_MOD_MOD_1 0x40000000
/** RFC 4115 color aware */
#define TMU_CFGARG2_MOD_MOD_2 0x80000000
/** loose color blind */
#define TMU_CFGARG2_MOD_MOD_3 0xC0000000
/** Token Bucket Unit
    The number of Bytes that are added to the TBC with each SRC elapse
*/
#define TMU_CFGARG2_TBU_MASK 0x30000000
/** field offset */
#define TMU_CFGARG2_TBU_OFFSET 28
/** 1 Byte */
#define TMU_CFGARG2_TBU_TBU_0 0x00000000
/** 16 Bytes */
#define TMU_CFGARG2_TBU_TBU_1 0x10000000
/** 256 Bytes */
#define TMU_CFGARG2_TBU_TBU_2 0x20000000
/** 1024 Bytes */
#define TMU_CFGARG2_TBU_TBU_3 0x30000000
/** Accumulation Time for Token Bucket
	This field indicates the time between two consecutive accumulations
	of the bucket in units of 16 clocks.
	Together with the token bucket unit it determines the shaper rate.
*/
#define TMU_CFGARG2_SRM_MASK 0x00003FFF
/** field offset */
#define TMU_CFGARG2_SRM_OFFSET 0

/* Fields of "TBID Bucket MBS Parameter Data Register" */
/** Maximum Bucket Size
    This field gives the Maximum Size of the Token Bucket in bytes.
*/
#define TMU_CFGARG3_MBS_MASK 0x00FFFFFF
/** field offset */
#define TMU_CFGARG3_MBS_OFFSET 0

/* Fields of "Overlay for SBIN related Configuration Commands" */
/** Command Opcode */
#define TMU_CFGSBIN_CMDSBIN_MASK 0xF0000000
/** field offset */
#define TMU_CFGSBIN_CMDSBIN_OFFSET 28
/** Enable SB Input by setting SIE to 1 on SBIT Entry identified by SBIN */
#define TMU_CFGSBIN_CMDSBIN_SB_INPUT_ON 0x00000000
/** Disable SB Input by setting SIE to 0 on SBIT Entry identified by SBIN */
#define TMU_CFGSBIN_CMDSBIN_SB_INPUT_OFF 0x10000000
/** Set IWGT from CMDARG0 on SBIT Entry identified by SBIN */
#define TMU_CFGSBIN_CMDSBIN_SB_INPUT_WEIGHT 0x20000000
/** Set TBID from CMDARG1 on SBIT Entry identified by SBIN and
SBIN on TBST Entry identified by TBID
*/
#define TMU_CFGSBIN_CMDSBIN_SB_INPUT_BUCKET_SET 0x30000000
/** Enable TB assigned to SB input by setting TBE to 1 on SBIT Entry
identified by SBIN
*/
#define TMU_CFGSBIN_CMDSBIN_SB_INPUT_BUCKET_ON 0x40000000
/** Disable TB assigned to SB input by setting TBE to 0 on
SBIT Entry identified by SBIN
*/
#define TMU_CFGSBIN_CMDSBIN_SB_INPUT_BUCKET_OFF 0x50000000
/** Enable SB Output by setting SOE to 1 on SBOT Entry identified by SBID */
#define TMU_CFGSBIN_CMDSBIN_SB_OUTPUT_ON 0x60000000
/** Enable SB Output by setting SOE to 0 on SBOT Entry identified by SBID */
#define TMU_CFGSBIN_CMDSBIN_SB_OUTPUT_OFF 0x70000000
/** Valid
	This bit indicates that the CPU command has been completed
	and the respond parameters are valid, SW shall wait with a
	subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_CFGSBIN_VAL 0x00010000
/** Scheduler Block Input Number
	Index into the SB Input Table (SBIT).
	Indicates the SB Input to which the queue is connected to.
	It is possible to modify this assignment after activation,
	however care must be taken to assure the queue is disabled and empty.
*/
#define TMU_CFGSBIN_SBIN_MASK 0x000003FF
/** field offset */
#define TMU_CFGSBIN_SBIN_OFFSET 0

/* Fields of "Overlay for TBID related Configuration Commands" */
/** Command Opcode */
#define TMU_CFGTBID_CMDTBID_MASK 0xF0000000
/** field offset */
#define TMU_CFGTBID_CMDTBID_OFFSET 28
/** Enable Token Bucket B by setting TBE[B] to 1 on TBST
Entry identified by TBID
*/
#define TMU_CFGTBID_CMDTBID_TB_ON 0x80000000
/** Disable Token Bucket B by setting TBE[B] to 0 on TBST Entry
identified by TBID
*/
#define TMU_CFGTBID_CMDTBID_TB_OFF 0x90000000
/** Set MOD, TBU, SRM from CMDARG2 and MBS from CMDARG3 on Token
Bucket B of TBST Entry identified by TBID
*/
#define TMU_CFGTBID_CMDTBID_TB_SET 0xA0000000
/** Valid
This bit indicates that the CPU command has been completed
and the respond parameters are valid, SW shall wait with a
subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_CFGTBID_VAL 0x00010000
/** Token Bucket Identifier
    This field holds the TB identifier
*/
#define TMU_CFGTBID_TBID_MASK 0x000001FE
/** field offset */
#define TMU_CFGTBID_TBID_OFFSET 1
/** Token Bucket Selector
	This bit selects between token bucket 0 and token bucket 1
	within a dual bucket TBID
*/
#define TMU_CFGTBID_TB 0x00000001
/* Token Bucket 0
#define TMU_CFGTBID_TB_TB0 0x00000000
*/
/** Token Bucket 1 */
#define TMU_CFGTBID_TB_TB1 0x00000001

/* Fields of "Overlay for EPN related Configuration Commands" */
/** Command Opcode */
#define TMU_CFGEPN_CMDEPN_MASK 0xF0000000
/** field offset */
#define TMU_CFGEPN_CMDEPN_OFFSET 28
/** Enable Egress Port by setting EPE to 1 on EPMT Entry identified by EPN */
#define TMU_CFGEPN_CMDEPN_EP_ON 0xB0000000
/** Disable Egress Port by setting EPE to 0 on EPMT Entry identified by EPN */
#define TMU_CFGEPN_CMDEPN_EP_OFF 0xC0000000
/** Valid
	This bit indicates that the CPU command has been completed and
	the respond parameters are valid, SW shall wait with a subsequent access
	until the bit is switched on (=1) by HW
*/
#define TMU_CFGEPN_VAL 0x00010000
/** Egress Port Number
    This field holds the Egress Port Number, used as an index into the EPMT
*/
#define TMU_CFGEPN_EPN_MASK 0x0000007F
/** field offset */
#define TMU_CFGEPN_EPN_OFFSET 0

/* Fields of "Configuration Command Register" */
/** Command Opcode */
#define TMU_CFGCMD_CMD_MASK 0xF0000000
/** field offset */
#define TMU_CFGCMD_CMD_OFFSET 28
/** Enable SB Input by setting SIE to 1 on SBIT Entry identified by SBIN */
#define TMU_CFGCMD_CMD_SB_INPUT_ON 0x00000000
/** Disable SB Input by setting SIE to 0 on SBIT Entry identified by SBIN */
#define TMU_CFGCMD_CMD_SB_INPUT_OFF 0x10000000
/** Set IWGT from CMDARG0 on SBIT Entry identified by SBIN */
#define TMU_CFGCMD_CMD_SB_INPUT_WEIGHT 0x20000000
/** Set TBID from CMDARG1 on SBIT Entry identified by SBIN
and SBIN on TBST Entry identified by TBID
*/
#define TMU_CFGCMD_CMD_SB_INPUT_BUCKET_SET 0x30000000
/** Enable TB assigned to SB input by setting TBE to 1 on SBIT
Entry identified by SBIN
*/
#define TMU_CFGCMD_CMD_SB_INPUT_BUCKET_ON 0x40000000
/** Disable TB assigned to SB input by setting TBE to 0 on SBIT
Entry identified by SBIN
*/
#define TMU_CFGCMD_CMD_SB_INPUT_BUCKET_OFF 0x50000000
/** Enable SB Output by setting SOE to 1 on SBOT Entry identified by SBID */
#define TMU_CFGCMD_CMD_SB_OUTPUT_ON 0x60000000
/** Enable SB Output by setting SOE to 0 on SBOT Entry identified by SBID */
#define TMU_CFGCMD_CMD_SB_OUTPUT_OFF 0x70000000
/** Enable Token Bucket B by setting TBE[B] to 1 on TBST Entry
identified by TBID
*/
#define TMU_CFGCMD_CMD_TB_ON 0x80000000
/** Disable Token Bucket B by setting TBE[B] to 0 on TBST Entry
identified by TBID
*/
#define TMU_CFGCMD_CMD_TB_OFF 0x90000000
/** Set MOD, TBU, SRM from CMDARG2 and MBS from CMDARG3 on Token Bucket
B of TBST Entry identified by TBID
*/
#define TMU_CFGCMD_CMD_TB_SET 0xA0000000
/** Enable Egress Port by setting EPE to 1 on EPMT Entry identified by EPN */
#define TMU_CFGCMD_CMD_EP_ON 0xB0000000
/** Disable Egress Port by setting EPE to 0 on EPMT Entry identified by EPN */
#define TMU_CFGCMD_CMD_EP_OFF 0xC0000000
/** Valid
	This bit indicates that the CPU command has been
	completed and the respond parameters are valid, SW shall wait
	with a subsequent access until the bit is switched on (=1) by HW
*/
#define TMU_CFGCMD_VAL 0x00010000
/** Command Identifier
    One of SBIN, SBID, TBID + B, EPN
*/
#define TMU_CFGCMD_ID_MASK 0x0000FFFF
/** field offset */
#define TMU_CFGCMD_ID_OFFSET 0

	  /*! @} *//* TMU_REGISTER */

#endif				/* _drv_tmu_reg_h */
