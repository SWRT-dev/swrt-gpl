/**
 * egr_uc_writer_regs.h
 * Description: egr_uc_writer_s HW registers definitions
 *              NOTE: B Step ONLY module
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_UC_EGR_WRITER_H_
#define _PP_UC_EGR_WRITER_H_

#define PP_UC_EGR_WRITER_GEN_DATE_STR         "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_UC_EGR_WRITER_BASE                                    (0xF07E0000ULL)

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_CPU_EN_REG
 * HW_REG_NAME : reg_cpu_en
 * DESCRIPTION : Writer CPU Enable, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - CpuEnable[c]=1 indicates that uC #c is eligible
 *                 for receiving jobs. By default, Enable is all 0,
 *                 namely, all the uCs are disabled. Can be modified
 *                 at run tim
 *
 */
#define PP_UC_EGR_WRITER_CPU_EN_REG            ((UC_EGR_WRITER_BASE_ADDR) + 0x0)
#define PP_UC_EGR_WRITER_CPU_EN_RSVD0_OFF                           (1)
#define PP_UC_EGR_WRITER_CPU_EN_RSVD0_LEN                           (31)
#define PP_UC_EGR_WRITER_CPU_EN_RSVD0_MSK                           (0xFFFFFFFE)
#define PP_UC_EGR_WRITER_CPU_EN_RSVD0_RST                           (0x0)
#define PP_UC_EGR_WRITER_CPU_EN_CPU_EN_OFF                          (0)
#define PP_UC_EGR_WRITER_CPU_EN_CPU_EN_LEN                          (1)
#define PP_UC_EGR_WRITER_CPU_EN_CPU_EN_MSK                          (0x00000001)
#define PP_UC_EGR_WRITER_CPU_EN_CPU_EN_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_CPU_EN_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_UC_EGR_WRITER_CPU_EN_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_CPU_EN_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_MAP_REG
 * HW_REG_NAME : reg_map
 * DESCRIPTION : Map port to Queue, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 3][RO] - Reserved
 *   [ 2: 0][RW] - Port #p is mapped to the queue Map[c][p] in uC
 *                 #c.[br]
 *                 Note that this mapping is relevant only in case of
 *                 queue_credit[c][p]()0.Programmed at init
 *
 */
#define PP_UC_EGR_WRITER_MAP_REG             ((UC_EGR_WRITER_BASE_ADDR) + 0x400)
#define PP_UC_EGR_WRITER_MAP_RSVD0_OFF                              (3)
#define PP_UC_EGR_WRITER_MAP_RSVD0_LEN                              (29)
#define PP_UC_EGR_WRITER_MAP_RSVD0_MSK                              (0xFFFFFFF8)
#define PP_UC_EGR_WRITER_MAP_RSVD0_RST                              (0x0)
#define PP_UC_EGR_WRITER_MAP_MAP_OFF                                (0)
#define PP_UC_EGR_WRITER_MAP_MAP_LEN                                (3)
#define PP_UC_EGR_WRITER_MAP_MAP_MSK                                (0x00000007)
#define PP_UC_EGR_WRITER_MAP_MAP_RST                                (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_MAP_REG_IDX
 * NUM OF REGISTERS : 80
 */
#define PP_UC_EGR_WRITER_MAP_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_MAP_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_QU_CREDIT_REG
 * HW_REG_NAME : reg_qu_credit
 * DESCRIPTION : Credit per Port per CPU, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RW] - If queue_credit[c][p])0 then Writer can forward
 *                 incoming job from Port #p to uC #c, to queue
 *                 Map[c][p].queue_credit[c][] is programmed at init,
 *                 before enabling CPU #c, namely when CpuEnable[c]=0
 *
 */
#define PP_UC_EGR_WRITER_QU_CREDIT_REG       ((UC_EGR_WRITER_BASE_ADDR) + 0x800)
#define PP_UC_EGR_WRITER_QU_CREDIT_RSVD0_OFF                        (8)
#define PP_UC_EGR_WRITER_QU_CREDIT_RSVD0_LEN                        (24)
#define PP_UC_EGR_WRITER_QU_CREDIT_RSVD0_MSK                        (0xFFFFFF00)
#define PP_UC_EGR_WRITER_QU_CREDIT_RSVD0_RST                        (0x0)
#define PP_UC_EGR_WRITER_QU_CREDIT_QU_CREDIT_OFF                    (0)
#define PP_UC_EGR_WRITER_QU_CREDIT_QU_CREDIT_LEN                    (8)
#define PP_UC_EGR_WRITER_QU_CREDIT_QU_CREDIT_MSK                    (0x000000FF)
#define PP_UC_EGR_WRITER_QU_CREDIT_QU_CREDIT_RST                    (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_QU_CREDIT_REG_IDX
 * NUM OF REGISTERS : 80
 */
#define PP_UC_EGR_WRITER_QU_CREDIT_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_QU_CREDIT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_JOB_INDEX_REG
 * HW_REG_NAME : reg_job_index
 * DESCRIPTION : Job Index per queue per CPU, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RW] - JobIdx[c][p] holds the index where to write the
 *                 next job in the queue mapped to port #p in uC #c
 *
 */
#define PP_UC_EGR_WRITER_JOB_INDEX_REG      ((UC_EGR_WRITER_BASE_ADDR) + 0x0C00)
#define PP_UC_EGR_WRITER_JOB_INDEX_RSVD0_OFF                        (8)
#define PP_UC_EGR_WRITER_JOB_INDEX_RSVD0_LEN                        (24)
#define PP_UC_EGR_WRITER_JOB_INDEX_RSVD0_MSK                        (0xFFFFFF00)
#define PP_UC_EGR_WRITER_JOB_INDEX_RSVD0_RST                        (0x0)
#define PP_UC_EGR_WRITER_JOB_INDEX_JOB_INDEX_OFF                    (0)
#define PP_UC_EGR_WRITER_JOB_INDEX_JOB_INDEX_LEN                    (8)
#define PP_UC_EGR_WRITER_JOB_INDEX_JOB_INDEX_MSK                    (0x000000FF)
#define PP_UC_EGR_WRITER_JOB_INDEX_JOB_INDEX_RST                    (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_JOB_INDEX_REG_IDX
 * NUM OF REGISTERS : 80
 */
#define PP_UC_EGR_WRITER_JOB_INDEX_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_JOB_INDEX_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_JOB_CNT_REG
 * HW_REG_NAME : reg_job_counter
 * DESCRIPTION : Job counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - JobIdx[c][p] holds the index where to write the
 *                 next job in the queue mapped to port #p in uC #c
 *
 */
#define PP_UC_EGR_WRITER_JOB_CNT_REG        ((UC_EGR_WRITER_BASE_ADDR) + 0x1000)
#define PP_UC_EGR_WRITER_JOB_CNT_JOB_INDEX_OFF                      (0)
#define PP_UC_EGR_WRITER_JOB_CNT_JOB_INDEX_LEN                      (32)
#define PP_UC_EGR_WRITER_JOB_CNT_JOB_INDEX_MSK                      (0xFFFFFFFF)
#define PP_UC_EGR_WRITER_JOB_CNT_JOB_INDEX_RST                      (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_JOB_CNT_REG_IDX
 * NUM OF REGISTERS : 20
 */
#define PP_UC_EGR_WRITER_JOB_CNT_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_JOB_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_JOB_DONE_CNT_REG
 * HW_REG_NAME : reg_job_done_counter
 * DESCRIPTION : Job done counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - It counts the number of jobs the Reader pushed out
 *
 */
#define PP_UC_EGR_WRITER_JOB_DONE_CNT_REG   ((UC_EGR_WRITER_BASE_ADDR) + 0x1400)
#define PP_UC_EGR_WRITER_JOB_DONE_CNT_JOB_INDEX_OFF                 (0)
#define PP_UC_EGR_WRITER_JOB_DONE_CNT_JOB_INDEX_LEN                 (32)
#define PP_UC_EGR_WRITER_JOB_DONE_CNT_JOB_INDEX_MSK                 (0xFFFFFFFF)
#define PP_UC_EGR_WRITER_JOB_DONE_CNT_JOB_INDEX_RST                 (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_JOB_DONE_CNT_REG_IDX
 * NUM OF REGISTERS : 20
 */
#define PP_UC_EGR_WRITER_JOB_DONE_CNT_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_JOB_DONE_CNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_JOB_DONE_WD_REG
 * HW_REG_NAME : reg_job_done_wd
 * DESCRIPTION : Job done wr; Programmed at init, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31:18][RO] - Reserved
 *   [17:16][RW] - WDT_cfg =  job_done_wd[17:16]. The field WDT_cfg
 *                 defines the behavior of the WDT: WDT_cfg[0]=1
 *                 enables the mechanism.
 *                 [br]WDT_cfg[1]=1 indicates to freeze upon WDT
 *                 expiration
 *   [15: 0][RW] - WDT_val = job_done_wd[15:0]. 16*WDT_val is the
 *                 maximum time, in ms, that a uC is allowed to
 *                 process a job.
 *                 [br]See Timeout mechanism in HAS
 *
 */
#define PP_UC_EGR_WRITER_JOB_DONE_WD_REG    ((UC_EGR_WRITER_BASE_ADDR) + 0x1800)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_RSVD0_OFF                      (18)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_RSVD0_LEN                      (14)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_RSVD0_MSK                      (0xFFFC0000)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_RSVD0_RST                      (0x0)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_WDT_CFG_OFF                    (16)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_WDT_CFG_LEN                    (2)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_WDT_CFG_MSK                    (0x00030000)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_WDT_CFG_RST                    (0x0)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_WDT_VAL_OFF                    (0)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_WDT_VAL_LEN                    (16)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_WDT_VAL_MSK                    (0x0000FFFF)
#define PP_UC_EGR_WRITER_JOB_DONE_WD_WDT_VAL_RST                    (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_JOB_DONE_WD_REG_IDX
 * NUM OF REGISTERS : 20
 */
#define PP_UC_EGR_WRITER_JOB_DONE_WD_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_JOB_DONE_WD_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_IRR_REG
 * HW_REG_NAME : reg_IRR
 * DESCRIPTION : Job done counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - holds the interrupt request. IRR[p] indicates that
 *                 the WDT of port p expired
 *
 */
#define PP_UC_EGR_WRITER_IRR_REG            ((UC_EGR_WRITER_BASE_ADDR) + 0x1C00)
#define PP_UC_EGR_WRITER_IRR_RSVD0_OFF                              (1)
#define PP_UC_EGR_WRITER_IRR_RSVD0_LEN                              (31)
#define PP_UC_EGR_WRITER_IRR_RSVD0_MSK                              (0xFFFFFFFE)
#define PP_UC_EGR_WRITER_IRR_RSVD0_RST                              (0x0)
#define PP_UC_EGR_WRITER_IRR_IRR_OFF                                (0)
#define PP_UC_EGR_WRITER_IRR_IRR_LEN                                (1)
#define PP_UC_EGR_WRITER_IRR_IRR_MSK                                (0x00000001)
#define PP_UC_EGR_WRITER_IRR_IRR_RST                                (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_IRR_REG_IDX
 * NUM OF REGISTERS : 20
 */
#define PP_UC_EGR_WRITER_IRR_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_IRR_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_IER_REG
 * HW_REG_NAME : reg_IER
 * DESCRIPTION : Job done counter, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - This is interrupt enable register. If for some i
 *                 IRR[i]=1 and IER[i]=1 then the Reader outputs
 *                 reader_interrupt signal to CCU. The IER can be
 *                 programmed at run time
 *
 */
#define PP_UC_EGR_WRITER_IER_REG            ((UC_EGR_WRITER_BASE_ADDR) + 0x2000)
#define PP_UC_EGR_WRITER_IER_RSVD0_OFF                              (1)
#define PP_UC_EGR_WRITER_IER_RSVD0_LEN                              (31)
#define PP_UC_EGR_WRITER_IER_RSVD0_MSK                              (0xFFFFFFFE)
#define PP_UC_EGR_WRITER_IER_RSVD0_RST                              (0x0)
#define PP_UC_EGR_WRITER_IER_IER_OFF                                (0)
#define PP_UC_EGR_WRITER_IER_IER_LEN                                (1)
#define PP_UC_EGR_WRITER_IER_IER_MSK                                (0x00000001)
#define PP_UC_EGR_WRITER_IER_IER_RST                                (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_IER_REG_IDX
 * NUM OF REGISTERS : 20
 */
#define PP_UC_EGR_WRITER_IER_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_IER_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_PORT_EN_REG
 * HW_REG_NAME : reg_port_en
 * DESCRIPTION : port_en, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RW] - programmed at init, after programming the other
 *                 ports registers, except the CpuEnable, which can
 *                 be programmed at any time
 *
 */
#define PP_UC_EGR_WRITER_PORT_EN_REG        ((UC_EGR_WRITER_BASE_ADDR) + 0x4000)
#define PP_UC_EGR_WRITER_PORT_EN_RSVD0_OFF                          (1)
#define PP_UC_EGR_WRITER_PORT_EN_RSVD0_LEN                          (31)
#define PP_UC_EGR_WRITER_PORT_EN_RSVD0_MSK                          (0xFFFFFFFE)
#define PP_UC_EGR_WRITER_PORT_EN_RSVD0_RST                          (0x0)
#define PP_UC_EGR_WRITER_PORT_EN_PORT_EN_OFF                        (0)
#define PP_UC_EGR_WRITER_PORT_EN_PORT_EN_LEN                        (1)
#define PP_UC_EGR_WRITER_PORT_EN_PORT_EN_MSK                        (0x00000001)
#define PP_UC_EGR_WRITER_PORT_EN_PORT_EN_RST                        (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_PORT_EN_REG_IDX
 * NUM OF REGISTERS : 20
 */
#define PP_UC_EGR_WRITER_PORT_EN_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_PORT_EN_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_REG
 * HW_REG_NAME : reg_pre_payload_offset
 * DESCRIPTION : pre_payload_offset, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31:16][RO] - Reserved
 *   [15: 0][RW] - programmed at init
 *
 */
#define PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_REG \
	((UC_EGR_WRITER_BASE_ADDR) + 0x4400)
#define PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_RSVD0_OFF               (16)
#define PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_RSVD0_LEN               (16)
#define PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_RSVD0_MSK               (0xFFFF0000)
#define PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_RSVD0_RST               (0x0)
#define PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_PRE_PAYLOAD_OFFSET_OFF  (0)
#define PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_PRE_PAYLOAD_OFFSET_LEN  (16)
#define PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_PRE_PAYLOAD_OFFSET_MSK  (0x0000FFFF)
#define PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_PRE_PAYLOAD_OFFSET_RST  (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_REG_IDX
 * NUM OF REGISTERS : 20
 */
#define PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_PRE_PAYLOAD_OFFSET_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_PORT_SPECIFIC_REG
 * HW_REG_NAME : reg_port_specific
 * DESCRIPTION : Port_specific, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RW] - Port #p can have up to 12 specific registers; each
 *                 such specific register in the Port is connected
 *                 (wired) to one of the registers
 *                 Port_specific[p][n], n=0..9.Thus the meaning of
 *                 Port_specific[p][] is according the type of the
 *                 port #p. see HAS for details.
 *
 */
#define PP_UC_EGR_WRITER_PORT_SPECIFIC_REG  ((UC_EGR_WRITER_BASE_ADDR) + 0x4800)
#define PP_UC_EGR_WRITER_PORT_SPECIFIC_PORT_SPECIFIC_OFF            (0)
#define PP_UC_EGR_WRITER_PORT_SPECIFIC_PORT_SPECIFIC_LEN            (32)
#define PP_UC_EGR_WRITER_PORT_SPECIFIC_PORT_SPECIFIC_MSK            (0xFFFFFFFF)
#define PP_UC_EGR_WRITER_PORT_SPECIFIC_PORT_SPECIFIC_RST            (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_PORT_SPECIFIC_REG_IDX
 * NUM OF REGISTERS : 240
 */
#define PP_UC_EGR_WRITER_PORT_SPECIFIC_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_PORT_SPECIFIC_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_STATUS0_REG
 * HW_REG_NAME : reg_status0
 * DESCRIPTION : status, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - Status[p][0] indicates FIFO full
 *
 */
#define PP_UC_EGR_WRITER_STATUS0_REG        ((UC_EGR_WRITER_BASE_ADDR) + 0x4C00)
#define PP_UC_EGR_WRITER_STATUS0_RSVD0_OFF                          (1)
#define PP_UC_EGR_WRITER_STATUS0_RSVD0_LEN                          (31)
#define PP_UC_EGR_WRITER_STATUS0_RSVD0_MSK                          (0xFFFFFFFE)
#define PP_UC_EGR_WRITER_STATUS0_RSVD0_RST                          (0x0)
#define PP_UC_EGR_WRITER_STATUS0_STATUS_OFF                         (0)
#define PP_UC_EGR_WRITER_STATUS0_STATUS_LEN                         (1)
#define PP_UC_EGR_WRITER_STATUS0_STATUS_MSK                         (0x00000001)
#define PP_UC_EGR_WRITER_STATUS0_STATUS_RST                         (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_STATUS0_REG_IDX
 * NUM OF REGISTERS : 20
 */
#define PP_UC_EGR_WRITER_STATUS0_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_STATUS0_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_STATUS1_REG
 * HW_REG_NAME : reg_status1
 * DESCRIPTION : status, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 1][RO] - Reserved
 *   [ 0: 0][RO] - Status[p][0] indicates FIFO full
 *
 */
#define PP_UC_EGR_WRITER_STATUS1_REG        ((UC_EGR_WRITER_BASE_ADDR) + 0x5000)
#define PP_UC_EGR_WRITER_STATUS1_RSVD0_OFF                          (1)
#define PP_UC_EGR_WRITER_STATUS1_RSVD0_LEN                          (31)
#define PP_UC_EGR_WRITER_STATUS1_RSVD0_MSK                          (0xFFFFFFFE)
#define PP_UC_EGR_WRITER_STATUS1_RSVD0_RST                          (0x0)
#define PP_UC_EGR_WRITER_STATUS1_STATUS_OFF                         (0)
#define PP_UC_EGR_WRITER_STATUS1_STATUS_LEN                         (1)
#define PP_UC_EGR_WRITER_STATUS1_STATUS_MSK                         (0x00000001)
#define PP_UC_EGR_WRITER_STATUS1_STATUS_RST                         (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_STATUS1_REG_IDX
 * NUM OF REGISTERS : 20
 */
#define PP_UC_EGR_WRITER_STATUS1_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_STATUS1_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_REG
 * HW_REG_NAME : reg_min_qu_credit_cpu_t
 * DESCRIPTION : Minimum Credit per Port per CPU, NOTE, B Step ONLY
 *               register
 *
 *  Register Fields :
 *   [31: 8][RO] - Reserved
 *   [ 7: 0][RW] - Minimum Queue Credit. Sticky. Writable by Host
 *
 */
#define PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_REG \
	((UC_EGR_WRITER_BASE_ADDR) + 0x6000)
#define PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_RSVD0_OFF                (8)
#define PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_RSVD0_LEN                (24)
#define PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_RSVD0_MSK                (0xFFFFFF00)
#define PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_RSVD0_RST                (0x0)
#define PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_MIN_QU_CREDIT_OFF        (0)
#define PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_MIN_QU_CREDIT_LEN        (8)
#define PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_MIN_QU_CREDIT_MSK        (0x000000FF)
#define PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_MIN_QU_CREDIT_RST        (0x0)
/**
 * REG_IDX_ACCESS   : PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_REG_IDX
 * NUM OF REGISTERS : 80
 */
#define PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_REG_IDX(idx) \
	(PP_UC_EGR_WRITER_MIN_QU_CREDIT_CPU_REG + ((idx) << 2))

#endif
