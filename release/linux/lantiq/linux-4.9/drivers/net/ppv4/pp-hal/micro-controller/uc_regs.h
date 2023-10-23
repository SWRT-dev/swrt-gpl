/*
 * Description: PP micro-comtroller definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef __PP_UC_REGS_H__
#define __PP_UC_REGS_H__

#include "uc_private_engines.h"
#include "uc_shared_engines.h"

extern u64 uc_egr_base_addr;
extern u64 uc_ing_base_addr;

/* uC Cluster has base address: */
#define EGRESS_UC_BASE  uc_egr_base_addr
#define INGRESS_UC_BASE uc_ing_base_addr

/* RTL parameters */
#define UC_MAXPORT             16
#define UC_MAXCPU              4
#define UC_EGR_MAXTASK         32
#define UC_EGR_MAXQUEUE        5
#define UC_EGR_MAXJOB          8
#define UC_EGR_JOBSIZE         256
#define UC_EGR_QUEUESIZE       2048
#define UC_EGR_MAXMBOXCHANNEL  8
#define UC_EGR_LN_JOBSIZE      8
#define UC_EGR_LN_MAXJOB       3
#define UC_EGR_LN_MAXQUEUE     3
#define UC_EGR_LN_MAXPORT      4
#define UC_EGR_MAXSYNCOBJ      16
#define UC_EGR_MAX_QU_CREDIT   7

#define UC_ING_MAXQUEUE        2
#define UC_ING_MAXTASK         16
#define UC_ING_MAXJOB          6
#define UC_ING_JOBSIZE         1024
#define UC_ING_QUEUESIZE       8192
#define UC_ING_LN_JOBSIZE      10
#define UC_ING_LN_MAXJOB       3
#define UC_ING_LN_MAXQUEUE     1

/* From the host's point of view,
 * a uC Cluster address map is divided into 4 peripherals,
 * with IDs:
 */
/**
 * @define DCCM of an individual uC
 */
#define PERIPH_ID_DCCM        0

/**
 * @define The shared SRAM in the cluster
 */
#define PERIPH_ID_SHARED_SRAM 1

/**
 * @define The entities inside an individual uC, except its DCCM
 */
#define PERIPH_ID_PRVT_ACCL   2

/**
 * @define The entities which are common to all uC in the
 *         Cluster, except the shared SRAM
 */
#define PERIPH_ID_SHARED_ACCL 3

/*********************/
/* Private registers */
/*********************/

/* Address of a register is: <NAME>_UC_BASE|OFFSET,
 * where NAME is INGRESS or EGRESS, and the OFFSET has the following format:
 * +------+-------------------+-----+-----+------+----------+---+
 * |Size: |                2  |  2  |  5  |  5   |    6     | 2 |
 * +------+-------------------+-----+-----+------+----------+---+
 * |Name: |Peripheral_ID      |uCID | AID | RID  |Index     | - |
 * +------+-------------------+-----+-----+------+----------+---+
 * |Value:|PERIPH_ID_PRVT_ACCL|uCID | AID | RID  |Index/CTID| 0 |
 * +------+-------------------+-----+-----+------+----------+---+
 * |Range:|        21:20      |19:18|17:13| 12:8 |    7:2   |1:0|
 * +------+-------------------+-----+-----+------+----------+---+
 *
 * conventions:
 * t - uC type, either INGRESS or EGRESS
 * c - uC id, in range 0..MAXCPU
 * a - accelerator name
 * r - register name
 * q - queue id, in range 0..MAXQUEUE
 * p - port id, in range 0..MAXPORT
 * n - channel number, in range 0..MAXMBOXCHANNEL
 * e - event number, in range 0..MAXHWEVENT
 * x,y - indices
 * z - offset in memory
 */
/**
 * @define address of integer at offset z in DCCM:
 */
#define UC_DCCM(t, c, z)               \
	((t ## _UC_BASE) | (PERIPH_ID_DCCM << 20) | ((c) << 18) | (z))

/**
 * @define address of integer at offset z in shared SRAM:
 */
#define UC_SSRAM(t, z)                 \
	((t ## _UC_BASE) | (PERIPH_ID_SHARED_SRAM << 20) | (z))

/**
 * @define private accelerators registers base address
 * @param t uc type name INGRESS/EGRESS
 */
#define UC_PRIV_BASE(t)                \
	((t ## _UC_BASE) | (PERIPH_ID_PRVT_ACCL << 20))

/**
 * @define private accelerators address
 * @param t uc type name INGRESS/EGRESS
 * @param c uc cpu id
 * @param a accelerator name AID
 */
#define UC_PRIV_ACC(t, c, a)           \
	(UC_PRIV_BASE(t) | ((c) << 18) | (a##_AID << 13))

/**
 * @define private accelerators register address
 * @param t uc type name INGRESS/EGRESS
 * @param c uc cpu id
 * @param a accelerator name AID
 * @param r register id RID
 */
#define UC_PRIV_REG(t, c, a, r) \
	(UC_PRIV_ACC(t, c, a) | (a##_##r << 8))

/**
 * @define private accelerators register word address
 * @param t uc type name INGRESS/EGRESS
 * @param c uc cpu id
 * @param a accelerator name AID
 * @param r register name RID
 * @param w word index
 */
#define UC_PRIV_REG_IDX(t, c, a, r, w) \
	(UC_PRIV_REG(t, c, a, r) | ((w) << 2))

/**
 * @define task gp register address
 * @param t uc type name INGRESS/EGRESS
 * @param c uc cpu id
 * @param r register id
 * @param tid task id
 */
#define UC_A_PRIV_GP_REG(t, c, r, tid) \
	(UC_PRIV_ACC(t, c, GP_REG_A) | ((r) << 8) | ((tid) << 2))
#define UC_B_PRIV_GP_REG(t, c, r, tid) \
	(UC_PRIV_ACC(t, c, GP_REG_B) | ((r) << 8) | ((tid) << 2))

/**  BOOT **/

/**
 * SW_REG_NAME : UC_BOOT_RST_VEC_REG
 * HW_REG_NAME : reg_boot_reset_vector
 * DESCRIPTION : uC vector table
 */
#define UC_BOOT_RST_VEC_WORDS          64
#define UC_BOOT_RST_VEC_MSK            GENMASK(31, 0)
#define UC_BOOT_RST_VEC_REG(t, c, x)   UC_PRIV_REG_IDX(t, c, BOOT, RST_VEC, x)

/**
 * SW_REG_NAME : UC_BOOT_CORE_RUN_REG
 * HW_REG_NAME : reg_boot_core_run_halt
 * DESCRIPTION : 0 upon reset
 */
#define UC_BOOT_CORE_RUN_MSK           GENMASK(1, 0)
#define UC_BOOT_CORE_RUN_REG(t, c)     UC_PRIV_REG(t, c, BOOT, CORE_RUNHALT)

/**
 * SW_REG_NAME : UC_BOOT_RUN_ACK_REG
 * HW_REG_NAME : reg_boot_run_halt_ack
 * DESCRIPTION : Run Ack
 */
#define UC_BOOT_RUN_ACK_MSK            GENMASK(1, 0)
#define UC_BOOT_RUN_ACK_REG(t, c)      UC_PRIV_REG(t, c, BOOT, RUNHALT_ACK)

/**
 * SW_REG_NAME : UC_BOOT_GPREG_REG
 * HW_REG_NAME : reg_boot_gp_reg
 * DESCRIPTION : General Purpose Register
 */
#define UC_BOOT_GPREG_MSK              GENMASK(31, 0)
#define UC_BOOT_GPREG_REG(t, c)        UC_PRIV_REG(t, c, BOOT, GPREG)

#define UC_MTSR_READY_REG(t, c, tid)  UC_PRIV_REG_IDX(t, c, MTSR, READY, tid)
#define UC_A_AUX_PC_REG(t, c, tid)    UC_PRIV_REG_IDX(t, c, AUX_REG_A, PC, tid)
#define UC_B_AUX_PC_REG(t, c, tid)    UC_PRIV_REG_IDX(t, c, AUX_REG_B, PC, tid)
#define UC_A_AUX_S32_REG(t, c, tid)   UC_PRIV_REG_IDX(t, c, AUX_REG_A, S32, tid)
#define UC_B_AUX_S32_REG(t, c, tid)   UC_PRIV_REG_IDX(t, c, AUX_REG_B, S32, tid)
#define UC_A_POPA_ACT_LO_REG(t, c, q) UC_PRIV_REG_IDX(t, c, POPA_A, ACT_LO, q)
#define UC_A_POPA_ACT_HI_REG(t, c, q) UC_PRIV_REG_IDX(t, c, POPA_A, ACT_HI, q)
#define UC_B_POPA_ACT_LO_REG(t, c, q) UC_PRIV_REG_IDX(t, c, POPA_B, ACT_LO, q)
#define UC_B_POPA_ACT_HI_REG(t, c, q) UC_PRIV_REG_IDX(t, c, POPA_B, ACT_HI, q)

/*********************/
/* SR Counters       */
/*********************/
/**
 * @define address of a SR counter at offset z:
 * Note that SR counters fall under the SSRAM region.
 */
#define UC_SR(t, z)                 \
	((t ## _UC_BASE) | (PERIPH_ID_SHARED_SRAM << 20) | (1 << 19) | (z))
#define UC_SR_COUNT_A_MSB 12
#define UC_SR_COUNT_REGION_SIZE (1 << (UC_SR_COUNT_A_MSB - 1))
#define UC_SR_RESET_OFF (2 * UC_SR_COUNT_REGION_SIZE - 4)

/*********************/
/* Shared registers */
/*********************/

/* Address of a register at the Cluster level is: <NAME>_UC_BASE|OFFSET,
 * where NAME is INGRESS or EGRESS, and the OFFSET has the following format:
 * +------+----------------------+-----+-----+------+------+---+
 * |Size: |   2                  |  5  |  5  |  2   |   6  | 2 |
 * +------+----------------------+-----+-----+------+------+---+
 * |Name: |Peripheral_ID         | AID | RID | uCID | Index| - |
 * +------+----------------------+-----+-----+------+------+---+
 * |Value:|PERIPH_ID_SHARED_ACCL | AID | RID |Index2|Index1| 0 |
 * +------+----------------------+-----+-----+------+------+---+
 * |Range:|  21:20               |19:15|14:10| 9:8  | 7:2  |1:0|
 * +------+----------------------+-----+-----+------+------+---+
 */

/**
 * @define shared accelerators registers base address
 * @param t uc type name INGRESS/EGRESS
 */
#define UC_SHRD_BASE(t)                \
	((t ## _UC_BASE) | (PERIPH_ID_SHARED_ACCL << 20))

/**
 * @define shared accelerators register address
 * @param t uc type name INGRESS/EGRESS
 * @param a accelerator name AID
 * @param r register name RID
 */
#define UC_SHRD_REG(t, a, r)           \
	(UC_SHRD_BASE(t) | (a ## _AID << 15) | (a ## _ ## r << 10))

/**
 * @define shared accelerators register word address
 * @param t uc type name INGRESS/EGRESS
 * @param a accelerator name AID
 * @param r register name RID
 * @param w word index
 */
#define UC_SHRD_REG_IDX(t, a, r, w)    (UC_SHRD_REG(t, a, r) | ((w) << 2))

/***** READER REGISTERS *****/
/* READER_IRR */
#define UC_A_READER_IRR(t)             UC_SHRD_REG(t, READER_A, IRR)
#define UC_B_READER_IRR(t)             UC_SHRD_REG(t, READER_B, IRR)
/* READER_WRED_BASE */
#define UC_A_READER_WRED_BASE(t)       UC_SHRD_REG(t, READER_A, WRED_BASE)
#define UC_B_READER_WRED_BASE(t)       UC_SHRD_REG(t, READER_B, WRED_BASE)
/* READER_QMNGR_BASE */
#define UC_A_READER_QMNGR_BASE(t)      UC_SHRD_REG(t, READER_A, QMNGR_BASE)
#define UC_B_READER_QMNGR_BASE(t)      UC_SHRD_REG(t, READER_B, QMNGR_BASE)
/* READER_BMNGR_BASE */
#define UC_READER_BMNGR_BASE_MSK       GENMASK(15, 0)
#define UC_A_READER_BMNGR_BASE(t)      UC_SHRD_REG(t, READER_A, BMNGR_BASE)
#define UC_B_READER_BMNGR_BASE(t)      UC_SHRD_REG(t, READER_B, BMNGR_BASE)
/* READER_CLIENT_BASE */
#define UC_A_READER_CLIENT_BASE(t)     UC_SHRD_REG(t, READER_A, CLIENT_BASE)
#define UC_B_READER_CLIENT_BASE(t)     UC_SHRD_REG(t, READER_B, CLIENT_BASE)
/* READER_WRED_CLIENT */
#define UC_READER_WRED_CLIENT_MSK      GENMASK(15, 0)
#define UC_A_READER_WRED_CLIENT(t, p)  UC_SHRD_REG_IDX(t, READER_A, WRED_CLIENT, p)
#define UC_B_READER_WRED_CLIENT(t, p)  UC_SHRD_REG_IDX(t, READER_B, WRED_CLIENT, p)
/* READER_MB_RPLYCNT */
#define UC_A_READER_MB_RPLYCNT(t, n)   UC_SHRD_REG_IDX(t, READER_A, MB_RPLYCNT, n)
#define UC_B_READER_MB_RPLYCNT(t, n)   UC_SHRD_REG_IDX(t, READER_B, MB_RPLYCNT, n)
/*READER_MB_ADDR */
#define UC_A_READER_MB_ADDR(t, n)      UC_SHRD_REG_IDX(t, READER_A, MB_ADDR, n)
#define UC_B_READER_MB_ADDR(t, n)      UC_SHRD_REG_IDX(t, READER_B, MB_ADDR, n)
/* READER_MB_LEN */
#define UC_A_READER_MB_LEN(t, n)       UC_SHRD_REG_IDX(t, READER_A, MB_LEN, n)
#define UC_B_READER_MB_LEN(t, n)       UC_SHRD_REG_IDX(t, READER_B, MB_LEN, n)
/* READER_MB_FLAGS */
#define UC_A_READER_MB_FLAGS(t, n)     UC_SHRD_REG_IDX(t, READER_A, MB_FLAGS, n)
#define UC_B_READER_MB_FLAGS(t, n)     UC_SHRD_REG_IDX(t, READER_B, MB_FLAGS, n)
/* READER_MB_CNT */
#define UC_A_READER_MB_CNT(t, n)       UC_SHRD_REG_IDX(t, READER_A, MB_CNT, n)
#define UC_B_READER_MB_CNT(t, n)       UC_SHRD_REG_IDX(t, READER_B, MB_CNT, n)
/* READER_MB_IRR */
#define UC_A_READER_MB_IRR(t)          UC_SHRD_REG(t, READER_A, MB_IRR)
#define UC_B_READER_MB_IRR(t)          UC_SHRD_REG(t, READER_B, MB_IRR)
/* READER_MB_IER */
#define UC_A_READER_MB_IER(t)          UC_SHRD_REG(t, READER_A, MB_IER)
#define UC_B_READER_MB_IER(t)          UC_SHRD_REG(t, READER_B, MB_IER)
/* READER_MB_ICR */
#define UC_A_READER_MB_ICR(t)          UC_SHRD_REG(t, READER_A, MB_ICR)
#define UC_B_READER_MB_ICR(t)          UC_SHRD_REG(t, READER_B, MB_ICR)
/* READER_MB_BUSY */
#define UC_A_READER_MB_BUSY(t, n)      UC_SHRD_REG_IDX(t, READER_A, MB_BUSY, n)
#define UC_B_READER_MB_BUSY(t, n)      UC_SHRD_REG_IDX(t, READER_B, MB_BUSY, n)

#define UC_B_WRITER_PORT_SPEC(t, x, y)\
	(UC_SHRD_REG(t, WRITER_B, PORT_SPEC) | ((((x) * 12) + (y)) << 2))

/***** WRITER EGRESS REGISTERS *****/
/* WRITER_TX_PORT */
#define UC_EGR_A_WRITER_TX_PORT(p)  \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, TX_PORT, p)
#define UC_EGR_B_WRITER_TX_PORT(p)     \
	UC_B_WRITER_PORT_SPEC(EGRESS, p, 1)

/* WRITER_PORT_ENABLE */
#define UC_EGR_A_WRITER_PORT_ENABLE(p) \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, PORT_ENABLE, p)
#define UC_EGR_B_WRITER_PORT_ENABLE(p) \
	UC_SHRD_REG_IDX(EGRESS, WRITER_B, PORT_ENABLE, p)

/* WRITER_PORT */
#define UC_EGR_A_WRITER_PORT_ADDR(p)  \
	(UC_SHRD_REG(EGRESS, WRITER_A, PORT) | ((p) << 4))
#define UC_EGR_B_WRITER_PORT_ADDR(p)   \
	UC_B_WRITER_PORT_SPEC(EGRESS, p, 4)

/* WRITER_PORT MAP */
#define UC_EGR_A_WRITER_MAP(c, p) \
	(UC_SHRD_REG_IDX(EGRESS, WRITER_A, MAP, p) | (c << 6))
#define UC_EGR_B_WRITER_MAP(c, p) \
	(UC_SHRD_REG_IDX(EGRESS, WRITER_B, MAP, p) | (c << 6))

/* WRITER_PORT QUEUE CREDIT */
#define UC_EGR_A_WRITER_QUEUE_CREDIT(c, p) \
	(UC_SHRD_REG_IDX(EGRESS, WRITER_A, QUEUE_CREDIT, p) | (c << 6))
#define UC_EGR_B_WRITER_QUEUE_CREDIT(c, p) \
	(UC_SHRD_REG_IDX(EGRESS, WRITER_B, QUEUE_CREDIT, p) | (c << 6))

/* WRITER_PORT_CREDIT */
#define UC_EGR_A_WRITER_PORT_CREDIT(p) \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, PORT_CREDIT, p)
#define UC_EGR_B_WRITER_PORT_CREDIT(p)   \
	UC_B_WRITER_PORT_SPEC(EGRESS, p, 2)


/***** WRITER MB EGRESS REGISTERS *****/
/* WRITER_MB_MESSAGE */
#define UC_EGR_MBOX_MSG0_WR_ADDRESS    \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_MESSAGE, 0)
#define UC_EGR_B_MBOX_PORT_WR_ADDRESS(p)\
	UC_B_WRITER_PORT_SPEC(EGRESS, p, 2)

#define UC_EGR_MBOX_MSG0_WR_LENGTH     \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_MESSAGE, 1)
#define UC_EGR_B_MBOX_PORT_WR_LENGTH_FLAGS(p)\
	UC_B_WRITER_PORT_SPEC(EGRESS, p, 3)

#define UC_EGR_MBOX_MSG0_WR_DATA0      \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_MESSAGE, 2)
#define UC_EGR_B_MBOX_PORT_WR_DATA0(p) \
	UC_B_WRITER_PORT_SPEC(EGRESS, p, 0)

#define UC_EGR_MBOX_MSG0_WR_DATA1      \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_MESSAGE, 3)
#define UC_EGR_B_MBOX_PORT_WR_DATA1(p) \
	UC_B_WRITER_PORT_SPEC(EGRESS, p, 1)

#define UC_EGR_MBOX_MSG0_WR_FLAGS      \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_FLAGS,   0)
#define UC_EGR_MBOX_PORT_WR_CHANNEL(p) \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_CHANNEL, 0)

#define UC_EGR_MBOX_MSG1_WR_ADDRESS    \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_MESSAGE, 4)
#define UC_EGR_MBOX_MSG1_WR_LENGTH     \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_MESSAGE, 5)
#define UC_EGR_MBOX_MSG1_WR_DATA0      \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_MESSAGE, 6)
#define UC_EGR_MBOX_MSG1_WR_DATA1      \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_MESSAGE, 7)
#define UC_EGR_MBOX_MSG1_WR_FLAGS      \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_FLAGS,   1)
#define UC_EGR_MBOX_MSG1_WR_CHANNEL    \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_CHANNEL, 1)
#define UC_EGR_MBOX_MSG1_WR_STATUS     \
	UC_SHRD_REG_IDX(EGRESS, WRITER_A, MB_STATUS,  1)

// CCU Common:
#define UC_A_CCU_ISR_REG(t)            UC_SHRD_REG(t, CCU_A, ISR)
#define UC_B_CCU_ISR_REG(t)            UC_SHRD_REG(t, CCU_B, ISR)

#define UC_A_CCU_IRR_REG(t)            UC_SHRD_REG(t, CCU_A, IRR)
#define UC_B_CCU_IRR_REG(t)            UC_SHRD_REG(t, CCU_B, IRR)

#define UC_A_CCU_IER_REG(t)            UC_SHRD_REG(t, CCU_A, IER)
#define UC_B_CCU_IER_REG(t)            UC_SHRD_REG(t, CCU_B, IER)

#define UC_CCU_MAXCPU_MSK              GENMASK(7, 0)

#define UC_A_CCU_MAXCPU_REG(t)         UC_SHRD_REG(t, CCU_A, MAXCPU)
#define UC_B_CCU_MAXCPU_REG(t)         UC_SHRD_REG(t, CCU_B, MAXCPU)

#define UC_CCU_GPREG_MAX               8
#define UC_CCU_GPREG_MSK               GENMASK(31, 0)
#define UC_A_CCU_GPREG_REG(t, c)       UC_SHRD_REG_IDX(t, CCU_A, GPREG, c)
#define UC_B_CCU_GPREG_REG(t, c)       UC_SHRD_REG_IDX(t, CCU_B, GPREG, c)

#define UC_A_CCU_UC_UNRESET_REG(t, c)  UC_SHRD_REG_IDX(t, CCU_A, UC_UNRESET, c)
#define UC_B_CCU_UC_UNRESET_REG(t, c)  UC_SHRD_REG_IDX(t, CCU_B, UC_UNRESET, c)

#define UC_A_CCU_UC_CLK_EN_REG(t, c)   UC_SHRD_REG_IDX(t, CCU_A, UC_CLK_EN, c)
#define UC_B_CCU_UC_CLK_EN_REG(t, c)   UC_SHRD_REG_IDX(t, CCU_B, UC_CLK_EN, c)

/**
 * CLUSTER BRIDGE FAT A-step
 */
#define UC_A_FAT_MAX_ENT               14
#define UC_A_FAT_EN_BIT_SHIFT          8
#define UC_A_PP_FAT_ENT                0xA
#define UC_A_PP_FAT_VAL                0xF
#define UC_A_IOC_FAT_ENT               0xB
#define UC_A_IOC_FAT_VAL               0xC5
#define UC_A_FAT_VAL_MSK               GENMASK(7, 0)
#define UC_A_FAT_EN_MSK                BIT(8)
#define UC_A_CLUSTERBRIDGE_FAT(t, x)   \
	UC_SHRD_REG_IDX(t, CLUSTERBRIDGE_A, FAT, x)

/**
 * CLUSTER BRIDGE FAT B-step
 */
#define UC_B_FAT_MAX_ENT               48
#define UC_B_FAT_BASE_MAX_ENT          8
#define UC_B_FAT_ALL_MAX_ENT           (UC_B_FAT_BASE_MAX_ENT+UC_B_FAT_MAX_ENT)
#define UC_B_PP_FAT_ENT_BASE           0x28
#define UC_B_PP_FAT_VAL_BASE           0x3C
#define UC_B_IOC_FAT_ENT_BASE          0x2C
#define UC_B_IOC_C5_FAT_VAL_BASE       0x314 /* 0xC5 << 2 */
#define UC_B_IOC_C6_FAT_ENT_BASE       0x30
#define UC_B_IOC_C6_FAT_VAL_BASE(ind)  (((0xC6 << 2) + ind) << 22)
#define UC_B_PP_FAT_NUM_ENT            4
#define UC_B_FAT_VAL_MSK               GENMASK(9, 0)
#define UC_B_FAT_EN_MSK                BIT(0)

#define UC_B_CLUSTERBRIDGE_FAT(t, x)   \
	UC_SHRD_REG_IDX(t, CLUSTERBRIDGE_B, FAT, x)

#define UC_B_CLUSTERBRIDGE_VALID(t, x) \
	UC_SHRD_REG_IDX(t, CLUSTERBRIDGE_B, VALID, x)

#define UC_B_CLUSTERBRIDGE_BASE(t, x)  \
	UC_SHRD_REG_IDX(t, CLUSTERBRIDGE_B, BASE, x)

#endif /* __PP_UC_REGS_H__ */
