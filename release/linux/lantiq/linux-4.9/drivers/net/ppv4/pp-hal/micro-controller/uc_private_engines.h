/*
 * Description: PP micro-comtrollers private accelerators definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019 Intel Corporation
 */

#ifndef __PP_UC_PRIVATE_ENGINES_H__
#define __PP_UC_PRIVATE_ENGINES_H__

/* this file defines the <AID,RID> of the HW entities in the
 * uC  (aka Private engines)
 *  +-------------------------------------------------------+
 *  | Important: the <AID,RID> must be aligned with the RDL |
 *  +-------------------------------------------------------+
 * Comments:
 *(1): internal register, can be used for debug.
 *(2): set by host
 */

/* Private Engines AID: */
/**
 * @define boot configuration
 */
#define BOOT_AID           0
#define MTSR_AID           3
#define GP_REG_A_AID       8
#define GP_REG_B_AID       16
#define AUX_REG_A_AID      9
#define AUX_REG_B_AID      17
#define POPA_A_AID         12
#define POPA_B_AID         4

////////////////////////////////
// RID of registers, per AID: //
////////////////////////////////

/* Boot: */
/**
 * @define (2) uC vector table
 */
#define BOOT_RST_VEC       0
/**
 * @define (2) outputs the signal arc_run_req_a
 */
#define BOOT_CORE_RUNHALT  1
/**
 * @define (1) holds MyCID, hard-wired
 */
#define BOOT_MYCID         2
/**
 * @define general-purpose 32 bits
 */
#define BOOT_GPREG         3
/**
 * @define RW, array, debug
 */
#define BOOT_SWREG         4
/**
 * @define (2) inputs the signal arc_run_ack
 */
#define BOOT_RUNHALT_ACK   5
/**
 * @define holds the value of MY_FUSE[1:0] (MAC-CPU)
 */
#define BOOT_GETFUSE       6

/* MTSR */
#define MTSR_READY         0

/* AUX_REG */
#define AUX_REG_A_PC       0
#define AUX_REG_B_PC       0
#define AUX_REG_A_S32      1 /* status32 */
#define AUX_REG_B_S32      1 /* status32 */

/* POPA */
#define POPA_A_ACT_LO      6
#define POPA_B_ACT_LO      6
#define POPA_A_ACT_HI      7
#define POPA_B_ACT_HI      7

#endif /* __PP_UC_PRIVATE_ENGINES_H__ */
