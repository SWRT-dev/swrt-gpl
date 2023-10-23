/**
 * tscd_regs.h
 * Description: tscd_s HW registers definitions
 * Auto-generated file by RDL Parser, DO NOT CHANGE
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef _PP_TSCD_H_
#define _PP_TSCD_H_

#define PP_TSCD_GEN_DATE_STR                  "Sunday May 17, 2020 [5:26:11 pm]"
#define PP_TSCD_BASE                                             (0xF1000000ULL)

/**
 * SW_REG_NAME : PP_TSCD_PFSCD_DEFQSETHNDL_REG
 * HW_REG_NAME : pfscd_defqsethndl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:16][RO] - MISSING_DESCRIPTION
 *   [15: 0][RW] - Default Queue Set Handle.    0xffff - indicates
 *                 that the Default Queue Set is not valid. It is
 *                 either not initialized or disabled.
 *
 */
#define PP_TSCD_PFSCD_DEFQSETHNDL_REG                   ((TSCD_BASE_ADDR) + 0x0)
#define PP_TSCD_PFSCD_DEFQSETHNDL_RSVD_2_OFF                        (16)
#define PP_TSCD_PFSCD_DEFQSETHNDL_RSVD_2_LEN                        (16)
#define PP_TSCD_PFSCD_DEFQSETHNDL_RSVD_2_MSK                        (0xFFFF0000)
#define PP_TSCD_PFSCD_DEFQSETHNDL_RSVD_2_RST                        (0x0)
#define PP_TSCD_PFSCD_DEFQSETHNDL_OFF                               (0)
#define PP_TSCD_PFSCD_DEFQSETHNDL_LEN                               (16)
#define PP_TSCD_PFSCD_DEFQSETHNDL_MSK                               (0x0000FFFF)
#define PP_TSCD_PFSCD_DEFQSETHNDL_RST                               (0xffff)
/**
 * REG_IDX_ACCESS   : PP_TSCD_PFSCD_DEFQSETHNDL_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_TSCD_PFSCD_DEFQSETHNDL_REG_IDX(idx) \
	(PP_TSCD_PFSCD_DEFQSETHNDL_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_NUMOFCONFPORTS_REG
 * HW_REG_NAME : glscd_numofconfports
 * DESCRIPTION : NUM_OF_CONF_PORTS - This register should be
 *               updated according to total number of currently
 *               configured ports
 *
 *  Register Fields :
 *   [31: 9][RO] - Reserved
 *   [ 8: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_NUMOFCONFPORTS_REG               ((TSCD_BASE_ADDR) + 0x40)
#define PP_TSCD_GLSCD_NUMOFCONFPORTS_RSVD0_OFF                      (9)
#define PP_TSCD_GLSCD_NUMOFCONFPORTS_RSVD0_LEN                      (23)
#define PP_TSCD_GLSCD_NUMOFCONFPORTS_RSVD0_MSK                      (0xFFFFFE00)
#define PP_TSCD_GLSCD_NUMOFCONFPORTS_RSVD0_RST                      (0x0)
#define PP_TSCD_GLSCD_NUMOFCONFPORTS_OFF                            (0)
#define PP_TSCD_GLSCD_NUMOFCONFPORTS_LEN                            (9)
#define PP_TSCD_GLSCD_NUMOFCONFPORTS_MSK                            (0x000001FF)
#define PP_TSCD_GLSCD_NUMOFCONFPORTS_RST                            (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_CMD_DEST_ADDR_REG
 * HW_REG_NAME : glscd_cmd_dest_addr
 * DESCRIPTION :  CMD_DST_ADDR - This register keeps the TSCD
 *               command write destination address
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_CMD_DEST_ADDR_REG                ((TSCD_BASE_ADDR) + 0x44)
#define PP_TSCD_GLSCD_CMD_DEST_ADDR_OFF                             (0)
#define PP_TSCD_GLSCD_CMD_DEST_ADDR_LEN                             (32)
#define PP_TSCD_GLSCD_CMD_DEST_ADDR_MSK                             (0xFFFFFFFF)
#define PP_TSCD_GLSCD_CMD_DEST_ADDR_RST                             (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_RLMTBLRDDATAHI2_REG
 * HW_REG_NAME : glscd_rlmtblrddatahi2
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 3][RO] - MISSING_DESCRIPTION
 *   [ 2: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI2_REG              ((TSCD_BASE_ADDR) + 0x4C)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI2_RSVD_2_OFF                    (3)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI2_RSVD_2_LEN                    (29)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI2_RSVD_2_MSK                    (0xFFFFFFF8)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI2_RSVD_2_RST                    (0x0)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI2_DATA_OFF                      (0)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI2_DATA_LEN                      (3)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI2_DATA_MSK                      (0x00000007)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI2_DATA_RST                      (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_QUANTA_REG
 * HW_REG_NAME : glscd_quanta
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 3][RO] - MISSING_DESCRIPTION
 *   [ 2: 0][RW] - This field is used to set a quanta shift value.
 *                 Quanta is calculated by (1024 andlt;andlt; shift).
 *                 Depending on the setting of the shift value,
 *                 quanta can be set to 1K, 2K, 4K, 8K, and 16K
 *                 values.    Shift values of 4-7 are reserved, and
 *                 should not be used
 *
 */
#define PP_TSCD_GLSCD_QUANTA_REG                       ((TSCD_BASE_ADDR) + 0x80)
#define PP_TSCD_GLSCD_QUANTA_RSVD_2_OFF                             (3)
#define PP_TSCD_GLSCD_QUANTA_RSVD_2_LEN                             (29)
#define PP_TSCD_GLSCD_QUANTA_RSVD_2_MSK                             (0xFFFFFFF8)
#define PP_TSCD_GLSCD_QUANTA_RSVD_2_RST                             (0x0)
#define PP_TSCD_GLSCD_QUANTA_TSCDQUANTA_OFF                         (0)
#define PP_TSCD_GLSCD_QUANTA_TSCDQUANTA_LEN                         (3)
#define PP_TSCD_GLSCD_QUANTA_TSCDQUANTA_MSK                         (0x00000007)
#define PP_TSCD_GLSCD_QUANTA_TSCDQUANTA_RST                         (0x2)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_IFDATA_REG
 * HW_REG_NAME : glscd_ifdata
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RW] - Seeandedsp;andedsp;register description
 *
 */
#define PP_TSCD_GLSCD_IFDATA_REG                       ((TSCD_BASE_ADDR) + 0x84)
#define PP_TSCD_GLSCD_IFDATA_TSCDIFDATA_OFF                         (0)
#define PP_TSCD_GLSCD_IFDATA_TSCDIFDATA_LEN                         (32)
#define PP_TSCD_GLSCD_IFDATA_TSCDIFDATA_MSK                         (0xFFFFFFFF)
#define PP_TSCD_GLSCD_IFDATA_TSCDIFDATA_RST                         (0x0)
/**
 * REG_IDX_ACCESS   : PP_TSCD_GLSCD_IFDATA_REG_IDX
 * NUM OF REGISTERS : 4
 */
#define PP_TSCD_GLSCD_IFDATA_REG_IDX(idx) \
	(PP_TSCD_GLSCD_IFDATA_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_IFICMDL_REG
 * HW_REG_NAME : glscd_ificmdl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:22][WO] - MISSING_DESCRIPTION
 *   [21:19][WO] - Type of the Control operation. (000b) Reserved,
 *                 (001b) Suspend, (010b) Resume, (011b-111b)
 *                 Reserved.
 *   [18: 8][WO] - Index of the entry in the table
 *   [ 7: 4][WO] - Name of the scheduler configuration table accessed
 *                 by command.    (0000b) NodesTable, (0001b) Branch
 *                 Table, (0010b) Bandwidth Limit table, (0011b)
 *                 Bandwidth Limit Summary  table, (0100b) Best
 *                 Effort Table, (0101b) Ready List Mapping Table,
 *                 (0110b) Shared Bandwidth Limit Table,  (0111b)
 *                 Nodes Octet Table, (1000b-1111b) Reserved
 *   [ 3: 0][WO] - Command opcode. (0000b) ReadEntry, (0001b)
 *                 WriteEntry, (0010b) CopyEntry, (0011b) WriteField,
 *                 (0100b)  CopyField, (0101b) CopyAndShift, (0110b)
 *                 Control (0111b-1111b) Reserved
 *
 */
#define PP_TSCD_GLSCD_IFICMDL_REG                      ((TSCD_BASE_ADDR) + 0x94)
#define PP_TSCD_GLSCD_IFICMDL_RSVD_5_OFF                            (22)
#define PP_TSCD_GLSCD_IFICMDL_RSVD_5_LEN                            (10)
#define PP_TSCD_GLSCD_IFICMDL_RSVD_5_MSK                            (0xFFC00000)
#define PP_TSCD_GLSCD_IFICMDL_RSVD_5_RST                            (0x0)
#define PP_TSCD_GLSCD_IFICMDL_CTRLTYPE_OFF                          (19)
#define PP_TSCD_GLSCD_IFICMDL_CTRLTYPE_LEN                          (3)
#define PP_TSCD_GLSCD_IFICMDL_CTRLTYPE_MSK                          (0x00380000)
#define PP_TSCD_GLSCD_IFICMDL_CTRLTYPE_RST                          (0x0)
#define PP_TSCD_GLSCD_IFICMDL_TBLENTRYIDX_OFF                       (8)
#define PP_TSCD_GLSCD_IFICMDL_TBLENTRYIDX_LEN                       (11)
#define PP_TSCD_GLSCD_IFICMDL_TBLENTRYIDX_MSK                       (0x0007FF00)
#define PP_TSCD_GLSCD_IFICMDL_TBLENTRYIDX_RST                       (0x0)
#define PP_TSCD_GLSCD_IFICMDL_TBLTYPE_OFF                           (4)
#define PP_TSCD_GLSCD_IFICMDL_TBLTYPE_LEN                           (4)
#define PP_TSCD_GLSCD_IFICMDL_TBLTYPE_MSK                           (0x000000F0)
#define PP_TSCD_GLSCD_IFICMDL_TBLTYPE_RST                           (0x0)
#define PP_TSCD_GLSCD_IFICMDL_OPCODE_OFF                            (0)
#define PP_TSCD_GLSCD_IFICMDL_OPCODE_LEN                            (4)
#define PP_TSCD_GLSCD_IFICMDL_OPCODE_MSK                            (0x0000000F)
#define PP_TSCD_GLSCD_IFICMDL_OPCODE_RST                            (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_IFICMDH_REG
 * HW_REG_NAME : glscd_ificmdh
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][WO] - MISSING_DESCRIPTION
 *   [30:12][WO] - This register has a different interpretation
 *                 depending on theandedsp;andedsp;scheduler
 *                 configuration command.    It is used for For
 *                 WriteField, CopyEntry and Control Submit/Resume
 *                 commands only.    For the CopyEntries command,
 *                 this register carries an index of the destination
 *                 table entry, and high  7 bits are reserved for
 *                 this operation.    For the Control Submit and
 *                 Resume commands it carries an index of the Branch
 *                 Table entry that should  be used by hardware for
 *                 branch traversal.    For the WriteField commands
 *                 this register carries a Value to be written to the
 *                 field.andedsp;
 *   [11: 7][WO] - This register is used for For WriteField and
 *                 CopyField commands only.    For the WriteField and
 *                 CopyField command it carries a size of the table
 *                 entry field in bits.andedsp;
 *   [ 6: 0][WO] - This register has a different interpretation
 *                 depending on theandedsp;andedsp;scheduler
 *                 configuration command.    It is used for For
 *                 WriteField, CopyField and CopyEntry commands only.
 *                 For WriteField and CopyField commands this
 *                 register carries a field offset in bits within
 *                 table entry.    For CopyEntry command this
 *                 register carries a number of table entries to
 *                 copy. Copy operation must  remain within Nodes
 *                 Octet boundaries both on source and destination
 *                 tables. For larger copies, multiple  CopyEntries
 *                 commands should be used. The maximum value
 *                 permitted for CopyEntries command is 1-7 in
 *                 conjunction  with Octet alignment restrictions
 *                 described above.
 *
 */
#define PP_TSCD_GLSCD_IFICMDH_REG                      ((TSCD_BASE_ADDR) + 0x98)
#define PP_TSCD_GLSCD_IFICMDH_RSVD_4_OFF                            (31)
#define PP_TSCD_GLSCD_IFICMDH_RSVD_4_LEN                            (1)
#define PP_TSCD_GLSCD_IFICMDH_RSVD_4_MSK                            (0x80000000)
#define PP_TSCD_GLSCD_IFICMDH_RSVD_4_RST                            (0x0)
#define PP_TSCD_GLSCD_IFICMDH_VALUE_ENTRYIDX_OFF                    (12)
#define PP_TSCD_GLSCD_IFICMDH_VALUE_ENTRYIDX_LEN                    (19)
#define PP_TSCD_GLSCD_IFICMDH_VALUE_ENTRYIDX_MSK                    (0x7FFFF000)
#define PP_TSCD_GLSCD_IFICMDH_VALUE_ENTRYIDX_RST                    (0x0)
#define PP_TSCD_GLSCD_IFICMDH_FLDSZ_OFF                             (7)
#define PP_TSCD_GLSCD_IFICMDH_FLDSZ_LEN                             (5)
#define PP_TSCD_GLSCD_IFICMDH_FLDSZ_MSK                             (0x00000F80)
#define PP_TSCD_GLSCD_IFICMDH_FLDSZ_RST                             (0x0)
#define PP_TSCD_GLSCD_IFICMDH_FLDOFFS_NUMENTS_OFF                   (0)
#define PP_TSCD_GLSCD_IFICMDH_FLDOFFS_NUMENTS_LEN                   (7)
#define PP_TSCD_GLSCD_IFICMDH_FLDOFFS_NUMENTS_MSK                   (0x0000007F)
#define PP_TSCD_GLSCD_IFICMDH_FLDOFFS_NUMENTS_RST                   (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_IFBCMDL_REG
 * HW_REG_NAME : glscd_ifbcmdl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:22][WO] - MISSING_DESCRIPTION
 *   [21:19][WO] - Type of the Control operation. (000b) BatchEnd,
 *                 (001b) Suspend, (010b) Resume, (011b-111b)
 *                 Reserved.batchEnd  is valid for the Batch
 *                 operations only.
 *   [18: 8][WO] - Index of the entry in the table
 *   [ 7: 4][WO] - Name of the scheduler configuration table accessed
 *                 by command. This register is used for all commands
 *                 but Batch Control    (0000b) NodesTable, (0001b)
 *                 Branch Table, (0010b) Bandwidth Limit table,
 *                 (0011b) Bandwidth Limit Summary  table, (0100b)
 *                 Best Effort Table, (0101b) Ready List Mapping
 *                 Table, (0110b) Shared Bandwidth Limit Table,
 *                 (0111b) Nodes Octet Table, (1000b-1111b) Reserved
 *   [ 3: 0][WO] - Command opcode. (0) ReadEntry, (1) WriteEntry, (2)
 *                 CopyEntry, (3) WriteField, (4) CopyField, (5)
 *                 CopyAndShift,  (6) Batch Control, (7) Reserved
 *
 */
#define PP_TSCD_GLSCD_IFBCMDL_REG                      ((TSCD_BASE_ADDR) + 0x9C)
#define PP_TSCD_GLSCD_IFBCMDL_RSVD_5_OFF                            (22)
#define PP_TSCD_GLSCD_IFBCMDL_RSVD_5_LEN                            (10)
#define PP_TSCD_GLSCD_IFBCMDL_RSVD_5_MSK                            (0xFFC00000)
#define PP_TSCD_GLSCD_IFBCMDL_RSVD_5_RST                            (0x0)
#define PP_TSCD_GLSCD_IFBCMDL_CTRLTYPE_OFF                          (19)
#define PP_TSCD_GLSCD_IFBCMDL_CTRLTYPE_LEN                          (3)
#define PP_TSCD_GLSCD_IFBCMDL_CTRLTYPE_MSK                          (0x00380000)
#define PP_TSCD_GLSCD_IFBCMDL_CTRLTYPE_RST                          (0x0)
#define PP_TSCD_GLSCD_IFBCMDL_TBLENTRYIDX_OFF                       (8)
#define PP_TSCD_GLSCD_IFBCMDL_TBLENTRYIDX_LEN                       (11)
#define PP_TSCD_GLSCD_IFBCMDL_TBLENTRYIDX_MSK                       (0x0007FF00)
#define PP_TSCD_GLSCD_IFBCMDL_TBLENTRYIDX_RST                       (0x0)
#define PP_TSCD_GLSCD_IFBCMDL_TBLTYPE_OFF                           (4)
#define PP_TSCD_GLSCD_IFBCMDL_TBLTYPE_LEN                           (4)
#define PP_TSCD_GLSCD_IFBCMDL_TBLTYPE_MSK                           (0x000000F0)
#define PP_TSCD_GLSCD_IFBCMDL_TBLTYPE_RST                           (0x0)
#define PP_TSCD_GLSCD_IFBCMDL_OPCODE_OFF                            (0)
#define PP_TSCD_GLSCD_IFBCMDL_OPCODE_LEN                            (4)
#define PP_TSCD_GLSCD_IFBCMDL_OPCODE_MSK                            (0x0000000F)
#define PP_TSCD_GLSCD_IFBCMDL_OPCODE_RST                            (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_IFBCMDH_REG
 * HW_REG_NAME : glscd_ifbcmdh
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][WO] - MISSING_DESCRIPTION
 *   [30:12][WO] - This register has a different interpretation
 *                 depending on theandedsp;andedsp;scheduler
 *                 configuration command.    It is used for For
 *                 WriteField and CopyEntry commands only.    For the
 *                 CopyEntries command, this register carries an
 *                 index of the destination table entry, and high  7
 *                 bits are reserved for this operation.    For the
 *                 WriteField commands this register carries a Value
 *                 to be written to the field.andedsp;    For the
 *                 Control Submit and Resume commands it carries an
 *                 index of the Branch Table entry that should  be
 *                 used by hardware for branch traversal.
 *   [11: 7][WO] - This register is used for For WriteField and
 *                 CopyField commands only.    For the WriteField and
 *                 CopyField command it carries a size of the table
 *                 entry field in bits.andedsp;
 *   [ 6: 0][WO] - This register has a different interpretation
 *                 depending on theandedsp;andedsp;scheduler
 *                 configuration command.    It is used for For
 *                 WriteField, CopyField and CopyEntry commands only.
 *                 For WriteField and CopyField commands this
 *                 register carries a field offset in bits within
 *                 table entry.    For CopyEntry command this
 *                 register carries a number of table entries to
 *                 copy. Copy operation must  remain within Nodes
 *                 Octet boundaries both on source and destination
 *                 tables. For larger copies, multiple  CopyEntries
 *                 commands should be used. The maximum value
 *                 permitted for CopyEntries command is 1-7 in
 *                 conjunction  with Octet alignment restrictions
 *                 described above.
 *
 */
#define PP_TSCD_GLSCD_IFBCMDH_REG                     ((TSCD_BASE_ADDR) + 0x0A0)
#define PP_TSCD_GLSCD_IFBCMDH_RSVD_4_OFF                            (31)
#define PP_TSCD_GLSCD_IFBCMDH_RSVD_4_LEN                            (1)
#define PP_TSCD_GLSCD_IFBCMDH_RSVD_4_MSK                            (0x80000000)
#define PP_TSCD_GLSCD_IFBCMDH_RSVD_4_RST                            (0x0)
#define PP_TSCD_GLSCD_IFBCMDH_VALUE_ENTRYIDX_OFF                    (12)
#define PP_TSCD_GLSCD_IFBCMDH_VALUE_ENTRYIDX_LEN                    (19)
#define PP_TSCD_GLSCD_IFBCMDH_VALUE_ENTRYIDX_MSK                    (0x7FFFF000)
#define PP_TSCD_GLSCD_IFBCMDH_VALUE_ENTRYIDX_RST                    (0x0)
#define PP_TSCD_GLSCD_IFBCMDH_FLDSZ_OFF                             (7)
#define PP_TSCD_GLSCD_IFBCMDH_FLDSZ_LEN                             (5)
#define PP_TSCD_GLSCD_IFBCMDH_FLDSZ_MSK                             (0x00000F80)
#define PP_TSCD_GLSCD_IFBCMDH_FLDSZ_RST                             (0x0)
#define PP_TSCD_GLSCD_IFBCMDH_FLDOFFS_NUMENTS_OFF                   (0)
#define PP_TSCD_GLSCD_IFBCMDH_FLDOFFS_NUMENTS_LEN                   (7)
#define PP_TSCD_GLSCD_IFBCMDH_FLDOFFS_NUMENTS_MSK                   (0x0000007F)
#define PP_TSCD_GLSCD_IFBCMDH_FLDOFFS_NUMENTS_RST                   (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_IFSTATUS_REG
 * HW_REG_NAME : glscd_ifstatus
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RO] - If set, transmit scheduler is currently enabled,
 *                 if clear transmit scheduler is suspended.
 *   [ 8: 8][RO] - Error in batch command interface. If bit set, all
 *                 batch commands are discarded. Cleared by setting
 *                 BCMDCLRERR in TSCDIFCMD register
 *   [ 7: 7][RO] - Error in immediate command interface. If bit set,
 *                 all immediate commands are discarded. Cleared by
 *                 setting ICMDCLRERR in TSCDIFCMD register
 *   [ 6: 6][RO] - If set previously posted immediate command is not
 *                 completed yet, and immediate interface cannot be
 *                 used yet.    If clear, new immediate command can
 *                 be posted.
 *   [ 5: 0][RO] - Number of available entries in batch fifo
 *
 */
#define PP_TSCD_GLSCD_IFSTATUS_REG                    ((TSCD_BASE_ADDR) + 0x0A4)
#define PP_TSCD_GLSCD_IFSTATUS_RSVD_6_OFF                           (10)
#define PP_TSCD_GLSCD_IFSTATUS_RSVD_6_LEN                           (22)
#define PP_TSCD_GLSCD_IFSTATUS_RSVD_6_MSK                           (0xFFFFFC00)
#define PP_TSCD_GLSCD_IFSTATUS_RSVD_6_RST                           (0x0)
#define PP_TSCD_GLSCD_IFSTATUS_SCH_ENA_OFF                          (9)
#define PP_TSCD_GLSCD_IFSTATUS_SCH_ENA_LEN                          (1)
#define PP_TSCD_GLSCD_IFSTATUS_SCH_ENA_MSK                          (0x00000200)
#define PP_TSCD_GLSCD_IFSTATUS_SCH_ENA_RST                          (0x0)
#define PP_TSCD_GLSCD_IFSTATUS_BCMDERR_OFF                          (8)
#define PP_TSCD_GLSCD_IFSTATUS_BCMDERR_LEN                          (1)
#define PP_TSCD_GLSCD_IFSTATUS_BCMDERR_MSK                          (0x00000100)
#define PP_TSCD_GLSCD_IFSTATUS_BCMDERR_RST                          (0x0)
#define PP_TSCD_GLSCD_IFSTATUS_ICMDERR_OFF                          (7)
#define PP_TSCD_GLSCD_IFSTATUS_ICMDERR_LEN                          (1)
#define PP_TSCD_GLSCD_IFSTATUS_ICMDERR_MSK                          (0x00000080)
#define PP_TSCD_GLSCD_IFSTATUS_ICMDERR_RST                          (0x0)
#define PP_TSCD_GLSCD_IFSTATUS_ICMDBZ_OFF                           (6)
#define PP_TSCD_GLSCD_IFSTATUS_ICMDBZ_LEN                           (1)
#define PP_TSCD_GLSCD_IFSTATUS_ICMDBZ_MSK                           (0x00000040)
#define PP_TSCD_GLSCD_IFSTATUS_ICMDBZ_RST                           (0x0)
#define PP_TSCD_GLSCD_IFSTATUS_ENTRAVAIL_OFF                        (0)
#define PP_TSCD_GLSCD_IFSTATUS_ENTRAVAIL_LEN                        (6)
#define PP_TSCD_GLSCD_IFSTATUS_ENTRAVAIL_MSK                        (0x0000003F)
#define PP_TSCD_GLSCD_IFSTATUS_ENTRAVAIL_RST                        (0x3f)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_IFCTRL_REG
 * HW_REG_NAME : glscd_ifctrl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 5][WO] - MISSING_DESCRIPTION
 *   [ 4: 4][WO] - Disables the small credits toggle optimization
 *                 used to reduce the number of consecutive
 *                 scheduling  events for a single node.
 *   [ 3: 3][WO] - Scheduler control register. Allows to suspend and
 *                 resume transmit scheduler.    Writing (0) to this
 *                 register suspends scheduler, writing (1) resumes
 *                 scheduler.
 *   [ 2: 2][WO] - Batch Command Error Clear register. Allows clear
 *                 batch command interface error reported by
 *                 hardware.
 *   [ 1: 1][WO] - Immediate Command Error Clear register. Allows
 *                 clear immediate command interface error reported
 *                 by  hardware.
 *   [ 0: 0][WO] - Batch Command Doorbell register. Indicates that
 *                 batch commands are queued, and hardware can start
 *                 batch  execution. Firmware must ring DB every time
 *                 it posts BatchDone command to the TSCDIFBCMD
 *                 register. Hardware  should count number of
 *                 outstanding DB rings, increment this counter with
 *                 each DB ring, and decrement  each time it
 *                 processed BatchDone command. The maximum number of
 *                 outstanding DB rings is 64 - as a depth  of the
 *                 batch command FIFO.
 *
 */
#define PP_TSCD_GLSCD_IFCTRL_REG                      ((TSCD_BASE_ADDR) + 0x0A8)
#define PP_TSCD_GLSCD_IFCTRL_RSVD_6_OFF                             (5)
#define PP_TSCD_GLSCD_IFCTRL_RSVD_6_LEN                             (27)
#define PP_TSCD_GLSCD_IFCTRL_RSVD_6_MSK                             (0xFFFFFFE0)
#define PP_TSCD_GLSCD_IFCTRL_RSVD_6_RST                             (0x0)
#define PP_TSCD_GLSCD_IFCTRL_SMALL_CRED_DIS_OFF                     (4)
#define PP_TSCD_GLSCD_IFCTRL_SMALL_CRED_DIS_LEN                     (1)
#define PP_TSCD_GLSCD_IFCTRL_SMALL_CRED_DIS_MSK                     (0x00000010)
#define PP_TSCD_GLSCD_IFCTRL_SMALL_CRED_DIS_RST                     (0x0)
#define PP_TSCD_GLSCD_IFCTRL_SCH_ENA_OFF                            (3)
#define PP_TSCD_GLSCD_IFCTRL_SCH_ENA_LEN                            (1)
#define PP_TSCD_GLSCD_IFCTRL_SCH_ENA_MSK                            (0x00000008)
#define PP_TSCD_GLSCD_IFCTRL_SCH_ENA_RST                            (0x0)
#define PP_TSCD_GLSCD_IFCTRL_BCMDCLRERR_OFF                         (2)
#define PP_TSCD_GLSCD_IFCTRL_BCMDCLRERR_LEN                         (1)
#define PP_TSCD_GLSCD_IFCTRL_BCMDCLRERR_MSK                         (0x00000004)
#define PP_TSCD_GLSCD_IFCTRL_BCMDCLRERR_RST                         (0x0)
#define PP_TSCD_GLSCD_IFCTRL_ICMDCLRERR_OFF                         (1)
#define PP_TSCD_GLSCD_IFCTRL_ICMDCLRERR_LEN                         (1)
#define PP_TSCD_GLSCD_IFCTRL_ICMDCLRERR_MSK                         (0x00000002)
#define PP_TSCD_GLSCD_IFCTRL_ICMDCLRERR_RST                         (0x0)
#define PP_TSCD_GLSCD_IFCTRL_BCMDDB_OFF                             (0)
#define PP_TSCD_GLSCD_IFCTRL_BCMDDB_LEN                             (1)
#define PP_TSCD_GLSCD_IFCTRL_BCMDDB_MSK                             (0x00000001)
#define PP_TSCD_GLSCD_IFCTRL_BCMDDB_RST                             (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_RLMTBLRDCMD_REG
 * HW_REG_NAME : glscd_rlmtblrdcmd
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 0][RW] - Index of the Ready List Mapping table to read from
 *
 */
#define PP_TSCD_GLSCD_RLMTBLRDCMD_REG                 ((TSCD_BASE_ADDR) + 0x0AC)
#define PP_TSCD_GLSCD_RLMTBLRDCMD_RSVD_2_OFF                        (10)
#define PP_TSCD_GLSCD_RLMTBLRDCMD_RSVD_2_LEN                        (22)
#define PP_TSCD_GLSCD_RLMTBLRDCMD_RSVD_2_MSK                        (0xFFFFFC00)
#define PP_TSCD_GLSCD_RLMTBLRDCMD_RSVD_2_RST                        (0x0)
#define PP_TSCD_GLSCD_RLMTBLRDCMD_RLMTBLIDX_OFF                     (0)
#define PP_TSCD_GLSCD_RLMTBLRDCMD_RLMTBLIDX_LEN                     (10)
#define PP_TSCD_GLSCD_RLMTBLRDCMD_RLMTBLIDX_MSK                     (0x000003FF)
#define PP_TSCD_GLSCD_RLMTBLRDCMD_RLMTBLIDX_RST                     (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_RLMTBLRDSTATUS_REG
 * HW_REG_NAME : glscd_rlmtblrdstatus
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 1][RO] - MISSING_DESCRIPTION
 *   [ 0: 0][RO] - (0) - hardware completed read operation and
 *                 GLSCD_RLMTBLRDDATA register carries a valid
 *                 content of  requested RLM Table Entry.    (1) -
 *                 hardware processing RLM table entry read command.
 *                 Firmware should wait for (0) prior to posting next
 *                 command. Behavior if new command was posted when
 *                 (1) is undefined.
 *
 */
#define PP_TSCD_GLSCD_RLMTBLRDSTATUS_REG              ((TSCD_BASE_ADDR) + 0x0B0)
#define PP_TSCD_GLSCD_RLMTBLRDSTATUS_RSVD_2_OFF                     (1)
#define PP_TSCD_GLSCD_RLMTBLRDSTATUS_RSVD_2_LEN                     (31)
#define PP_TSCD_GLSCD_RLMTBLRDSTATUS_RSVD_2_MSK                     (0xFFFFFFFE)
#define PP_TSCD_GLSCD_RLMTBLRDSTATUS_RSVD_2_RST                     (0x0)
#define PP_TSCD_GLSCD_RLMTBLRDSTATUS_VALID_OFF                      (0)
#define PP_TSCD_GLSCD_RLMTBLRDSTATUS_VALID_LEN                      (1)
#define PP_TSCD_GLSCD_RLMTBLRDSTATUS_VALID_MSK                      (0x00000001)
#define PP_TSCD_GLSCD_RLMTBLRDSTATUS_VALID_RST                      (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_RLMTBLRDDATALO_REG
 * HW_REG_NAME : glscd_rlmtblrddatalo
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_RLMTBLRDDATALO_REG              ((TSCD_BASE_ADDR) + 0x0B4)
#define PP_TSCD_GLSCD_RLMTBLRDDATALO_DATA_OFF                       (0)
#define PP_TSCD_GLSCD_RLMTBLRDDATALO_DATA_LEN                       (32)
#define PP_TSCD_GLSCD_RLMTBLRDDATALO_DATA_MSK                       (0xFFFFFFFF)
#define PP_TSCD_GLSCD_RLMTBLRDDATALO_DATA_RST                       (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_RLMTBLRDDATAHI_REG
 * HW_REG_NAME : glscd_rlmtblrddatahi
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI_REG              ((TSCD_BASE_ADDR) + 0x0B8)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI_DATA_OFF                       (0)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI_DATA_LEN                       (32)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI_DATA_MSK                       (0xFFFFFFFF)
#define PP_TSCD_GLSCD_RLMTBLRDDATAHI_DATA_RST                       (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_PRGPERFCOUNT_REG
 * HW_REG_NAME : glscd_prgperfcount
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_PRGPERFCOUNT_REG                ((TSCD_BASE_ADDR) + 0x0BC)
#define PP_TSCD_GLSCD_PRGPERFCOUNT_OFF                              (0)
#define PP_TSCD_GLSCD_PRGPERFCOUNT_LEN                              (32)
#define PP_TSCD_GLSCD_PRGPERFCOUNT_MSK                              (0xFFFFFFFF)
#define PP_TSCD_GLSCD_PRGPERFCOUNT_RST                              (0x0)
/**
 * REG_IDX_ACCESS   : PP_TSCD_GLSCD_PRGPERFCOUNT_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_TSCD_GLSCD_PRGPERFCOUNT_REG_IDX(idx) \
	(PP_TSCD_GLSCD_PRGPERFCOUNT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_PRGPERFCTRL_REG
 * HW_REG_NAME : glscd_prgperfcontrol
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:29][RO] - MISSING_DESCRIPTION
 *   [28:26][RW] - This field is used to specify a Traffic Class
 *                 index when TC is selected in RESOURCESELECT field.
 *   [25:16][RW] - MISSING_DESCRIPTION
 *   [15: 8][RW] - This field is used to specify a Port Index when
 *                 the Port or TC is selected in RESOURCESELECT
 *                 field.
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 4][RW] - This field is used to select a physical resource
 *                 associated with the counter:    0x0 - None. When
 *                 selected counter should not be incremented.    0x1
 *                 - Port. When selected, the respective port index
 *                 should specified in PORTINDEX.    0x2 - TC. When
 *                 selected, the respective port index and TC index
 *                 should be specified in PORTINDEX and  TCINDEX
 *                 fields respectively    0x3andedsp;andedsp;- Queue
 *                 Set. When selected the respective Queue Set index
 *                 should be specified in the  QSINDEX field.
 *   [ 3: 3][RO] - MISSING_DESCRIPTION
 *   [ 2: 0][RW] - This field defines a type of the counter:    0x0 -
 *                 None. Counter should not be incremented.    0x1 -
 *                 Number of Scheduled Quanta    0x2 - Number of
 *                 Credit Updates requested by pipeline    0x3 -
 *                 Number of Work Available updates    0x4 - Number
 *                 of No Work Available updates    0x5 - Number of
 *                 Scheduled Quanta for Low Latency Traffic    0x6 -
 *                 Number of Low Latency Credit Allocation Requests
 *                 0x7 - Number of Low Latency Stalls due to lack of
 *                 credits
 *
 */
#define PP_TSCD_GLSCD_PRGPERFCTRL_REG                 ((TSCD_BASE_ADDR) + 0x0FC)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD3_OFF                         (29)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD3_LEN                         (3)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD3_MSK                         (0xE0000000)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD3_RST                         (0x0)
#define PP_TSCD_GLSCD_PRGPERFCTRL_TCINDEX_OFF                       (26)
#define PP_TSCD_GLSCD_PRGPERFCTRL_TCINDEX_LEN                       (3)
#define PP_TSCD_GLSCD_PRGPERFCTRL_TCINDEX_MSK                       (0x1C000000)
#define PP_TSCD_GLSCD_PRGPERFCTRL_TCINDEX_RST                       (0x0)
#define PP_TSCD_GLSCD_PRGPERFCTRL_QSINDEX_OFF                       (16)
#define PP_TSCD_GLSCD_PRGPERFCTRL_QSINDEX_LEN                       (10)
#define PP_TSCD_GLSCD_PRGPERFCTRL_QSINDEX_MSK                       (0x03FF0000)
#define PP_TSCD_GLSCD_PRGPERFCTRL_QSINDEX_RST                       (0x0)
#define PP_TSCD_GLSCD_PRGPERFCTRL_PORTINDEX_OFF                     (8)
#define PP_TSCD_GLSCD_PRGPERFCTRL_PORTINDEX_LEN                     (8)
#define PP_TSCD_GLSCD_PRGPERFCTRL_PORTINDEX_MSK                     (0x0000FF00)
#define PP_TSCD_GLSCD_PRGPERFCTRL_PORTINDEX_RST                     (0x0)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD2_OFF                         (6)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD2_LEN                         (2)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD2_MSK                         (0x000000C0)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD2_RST                         (0x0)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RESOURCEELECT_OFF                 (4)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RESOURCEELECT_LEN                 (2)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RESOURCEELECT_MSK                 (0x00000030)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RESOURCEELECT_RST                 (0x0)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD1_OFF                         (3)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD1_LEN                         (1)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD1_MSK                         (0x00000008)
#define PP_TSCD_GLSCD_PRGPERFCTRL_RSVD1_RST                         (0x0)
#define PP_TSCD_GLSCD_PRGPERFCTRL_CNTTYPE_OFF                       (0)
#define PP_TSCD_GLSCD_PRGPERFCTRL_CNTTYPE_LEN                       (3)
#define PP_TSCD_GLSCD_PRGPERFCTRL_CNTTYPE_MSK                       (0x00000007)
#define PP_TSCD_GLSCD_PRGPERFCTRL_CNTTYPE_RST                       (0x0)
/**
 * REG_IDX_ACCESS   : PP_TSCD_GLSCD_PRGPERFCTRL_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_TSCD_GLSCD_PRGPERFCTRL_REG_IDX(idx) \
	(PP_TSCD_GLSCD_PRGPERFCTRL_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_LLPREALTHRESH_REG
 * HW_REG_NAME : glscd_llprealthresh
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 0][RW] - This field indicates number of Quanta's that can
 *                 be preallocate for the Low Latency Queue Set.
 *
 */
#define PP_TSCD_GLSCD_LLPREALTHRESH_REG               ((TSCD_BASE_ADDR) + 0x13C)
#define PP_TSCD_GLSCD_LLPREALTHRESH_RSVD_2_OFF                      (4)
#define PP_TSCD_GLSCD_LLPREALTHRESH_RSVD_2_LEN                      (28)
#define PP_TSCD_GLSCD_LLPREALTHRESH_RSVD_2_MSK                      (0xFFFFFFF0)
#define PP_TSCD_GLSCD_LLPREALTHRESH_RSVD_2_RST                      (0x0)
#define PP_TSCD_GLSCD_LLPREALTHRESH_OFF                             (0)
#define PP_TSCD_GLSCD_LLPREALTHRESH_LEN                             (4)
#define PP_TSCD_GLSCD_LLPREALTHRESH_MSK                             (0x0000000F)
#define PP_TSCD_GLSCD_LLPREALTHRESH_RST                             (0x3)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_INCSCHEDCFGCOUNT_REG
 * HW_REG_NAME : glscd_incschedcfgcount
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_INCSCHEDCFGCOUNT_REG            ((TSCD_BASE_ADDR) + 0x140)
#define PP_TSCD_GLSCD_INCSCHEDCFGCOUNT_OFF                          (0)
#define PP_TSCD_GLSCD_INCSCHEDCFGCOUNT_LEN                          (32)
#define PP_TSCD_GLSCD_INCSCHEDCFGCOUNT_MSK                          (0xFFFFFFFF)
#define PP_TSCD_GLSCD_INCSCHEDCFGCOUNT_RST                          (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_CREDITSPERQUANTA_REG
 * HW_REG_NAME : glscd_creditsperquanta
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:18][RO] - MISSING_DESCRIPTION
 *   [17: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_CREDITSPERQUANTA_REG            ((TSCD_BASE_ADDR) + 0x144)
#define PP_TSCD_GLSCD_CREDITSPERQUANTA_RSVD_2_OFF                   (18)
#define PP_TSCD_GLSCD_CREDITSPERQUANTA_RSVD_2_LEN                   (14)
#define PP_TSCD_GLSCD_CREDITSPERQUANTA_RSVD_2_MSK                   (0xFFFC0000)
#define PP_TSCD_GLSCD_CREDITSPERQUANTA_RSVD_2_RST                   (0x0)
#define PP_TSCD_GLSCD_CREDITSPERQUANTA_TSCDCREDITSPERQUANTA_OFF     (0)
#define PP_TSCD_GLSCD_CREDITSPERQUANTA_TSCDCREDITSPERQUANTA_LEN     (18)
#define PP_TSCD_GLSCD_CREDITSPERQUANTA_TSCDCREDITSPERQUANTA_MSK     (0x0003FFFF)
#define PP_TSCD_GLSCD_CREDITSPERQUANTA_TSCDCREDITSPERQUANTA_RST     (0x40)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_BWLCREDUPDATE_REG
 * HW_REG_NAME : glscd_bwlcredupdate
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_BWLCREDUPDATE_REG               ((TSCD_BASE_ADDR) + 0x148)
#define PP_TSCD_GLSCD_BWLCREDUPDATE_OFF                             (0)
#define PP_TSCD_GLSCD_BWLCREDUPDATE_LEN                             (32)
#define PP_TSCD_GLSCD_BWLCREDUPDATE_MSK                             (0xFFFFFFFF)
#define PP_TSCD_GLSCD_BWLCREDUPDATE_RST                             (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_BWLLINESPERARB_REG
 * HW_REG_NAME : glscd_bwllinesperarb
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:11][RO] - MISSING_DESCRIPTION
 *   [10: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_BWLLINESPERARB_REG              ((TSCD_BASE_ADDR) + 0x14C)
#define PP_TSCD_GLSCD_BWLLINESPERARB_RSVD_2_OFF                     (11)
#define PP_TSCD_GLSCD_BWLLINESPERARB_RSVD_2_LEN                     (21)
#define PP_TSCD_GLSCD_BWLLINESPERARB_RSVD_2_MSK                     (0xFFFFF800)
#define PP_TSCD_GLSCD_BWLLINESPERARB_RSVD_2_RST                     (0x0)
#define PP_TSCD_GLSCD_BWLLINESPERARB_OFF                            (0)
#define PP_TSCD_GLSCD_BWLLINESPERARB_LEN                            (11)
#define PP_TSCD_GLSCD_BWLLINESPERARB_MSK                            (0x000007FF)
#define PP_TSCD_GLSCD_BWLLINESPERARB_RST                            (0x1)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_ERRSTATREG_REG
 * HW_REG_NAME : glscd_errstatreg
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 2][RO] - MISSING_DESCRIPTION
 *   [ 1: 1][RO] - This bit is set when hardware failed to complete
 *                 bandwidth limit credits update cycle prior to
 *                 timer  expiration. Such behavior may impact
 *                 correct bandwidth limit control, and to avoid this
 *                 the configuration  of GLSCD_BYTESPERCREDIT,
 *                 GLSCD_BWLCREDUPDATE and GLSCD_BWLLINESPERARB need
 *                 to be adjusted.
 *   [ 0: 0][RO] - This bit is set when scheduler hardware detected
 *                 infinite loop in transmit scheduler configuration.
 *
 */
#define PP_TSCD_GLSCD_ERRSTATREG_REG                  ((TSCD_BASE_ADDR) + 0x150)
#define PP_TSCD_GLSCD_ERRSTATREG_RSVD_3_OFF                         (2)
#define PP_TSCD_GLSCD_ERRSTATREG_RSVD_3_LEN                         (30)
#define PP_TSCD_GLSCD_ERRSTATREG_RSVD_3_MSK                         (0xFFFFFFFC)
#define PP_TSCD_GLSCD_ERRSTATREG_RSVD_3_RST                         (0x0)
#define PP_TSCD_GLSCD_ERRSTATREG_SHRTBWLIMUPDATEPER_OFF             (1)
#define PP_TSCD_GLSCD_ERRSTATREG_SHRTBWLIMUPDATEPER_LEN             (1)
#define PP_TSCD_GLSCD_ERRSTATREG_SHRTBWLIMUPDATEPER_MSK             (0x00000002)
#define PP_TSCD_GLSCD_ERRSTATREG_SHRTBWLIMUPDATEPER_RST             (0x0)
#define PP_TSCD_GLSCD_ERRSTATREG_LOOP_DETECTED_OFF                  (0)
#define PP_TSCD_GLSCD_ERRSTATREG_LOOP_DETECTED_LEN                  (1)
#define PP_TSCD_GLSCD_ERRSTATREG_LOOP_DETECTED_MSK                  (0x00000001)
#define PP_TSCD_GLSCD_ERRSTATREG_LOOP_DETECTED_RST                  (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_LANTCBCMDS_REG
 * HW_REG_NAME : glscd_lantcbcmds
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 7][RO] - MISSING_DESCRIPTION
 *   [ 6: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_LANTCBCMDS_REG                  ((TSCD_BASE_ADDR) + 0x154)
#define PP_TSCD_GLSCD_LANTCBCMDS_RSVD_2_OFF                         (7)
#define PP_TSCD_GLSCD_LANTCBCMDS_RSVD_2_LEN                         (25)
#define PP_TSCD_GLSCD_LANTCBCMDS_RSVD_2_MSK                         (0xFFFFFF80)
#define PP_TSCD_GLSCD_LANTCBCMDS_RSVD_2_RST                         (0x0)
#define PP_TSCD_GLSCD_LANTCBCMDS_NUMLANTCBCMDS_OFF                  (0)
#define PP_TSCD_GLSCD_LANTCBCMDS_NUMLANTCBCMDS_LEN                  (7)
#define PP_TSCD_GLSCD_LANTCBCMDS_NUMLANTCBCMDS_MSK                  (0x0000007F)
#define PP_TSCD_GLSCD_LANTCBCMDS_NUMLANTCBCMDS_RST                  (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_RLMTBLRD2CMD_REG
 * HW_REG_NAME : glscd_rlmtblrd2cmd
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 0][RW] - Index of the Ready List Mapping table to read from
 *
 */
#define PP_TSCD_GLSCD_RLMTBLRD2CMD_REG                ((TSCD_BASE_ADDR) + 0x158)
#define PP_TSCD_GLSCD_RLMTBLRD2CMD_RSVD_2_OFF                       (10)
#define PP_TSCD_GLSCD_RLMTBLRD2CMD_RSVD_2_LEN                       (22)
#define PP_TSCD_GLSCD_RLMTBLRD2CMD_RSVD_2_MSK                       (0xFFFFFC00)
#define PP_TSCD_GLSCD_RLMTBLRD2CMD_RSVD_2_RST                       (0x0)
#define PP_TSCD_GLSCD_RLMTBLRD2CMD_RLMTBLIDX_OFF                    (0)
#define PP_TSCD_GLSCD_RLMTBLRD2CMD_RLMTBLIDX_LEN                    (10)
#define PP_TSCD_GLSCD_RLMTBLRD2CMD_RLMTBLIDX_MSK                    (0x000003FF)
#define PP_TSCD_GLSCD_RLMTBLRD2CMD_RLMTBLIDX_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_RLMTBLRD2STATUS_REG
 * HW_REG_NAME : glscd_rlmtblrd2status
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 1][RO] - MISSING_DESCRIPTION
 *   [ 0: 0][RO] - (0) - hardware completed read operation and
 *                 GLSCD_RLMTBLRDDATA register carries a valid
 *                 content of  requested RLM Table Entry.    (1) -
 *                 hardware processing RLM table entry read command.
 *                 Firmware should wait for (0) prior to posting next
 *                 command. Behavior if new command was posted when
 *                 (1) is undefined.
 *
 */
#define PP_TSCD_GLSCD_RLMTBLRD2STATUS_REG             ((TSCD_BASE_ADDR) + 0x15C)
#define PP_TSCD_GLSCD_RLMTBLRD2STATUS_RSVD_2_OFF                    (1)
#define PP_TSCD_GLSCD_RLMTBLRD2STATUS_RSVD_2_LEN                    (31)
#define PP_TSCD_GLSCD_RLMTBLRD2STATUS_RSVD_2_MSK                    (0xFFFFFFFE)
#define PP_TSCD_GLSCD_RLMTBLRD2STATUS_RSVD_2_RST                    (0x0)
#define PP_TSCD_GLSCD_RLMTBLRD2STATUS_VALID_OFF                     (0)
#define PP_TSCD_GLSCD_RLMTBLRD2STATUS_VALID_LEN                     (1)
#define PP_TSCD_GLSCD_RLMTBLRD2STATUS_VALID_MSK                     (0x00000001)
#define PP_TSCD_GLSCD_RLMTBLRD2STATUS_VALID_RST                     (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_RLMTBLRD2DATALO_REG
 * HW_REG_NAME : glscd_rlmtblrd2datalo
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_RLMTBLRD2DATALO_REG             ((TSCD_BASE_ADDR) + 0x160)
#define PP_TSCD_GLSCD_RLMTBLRD2DATALO_DATA_OFF                      (0)
#define PP_TSCD_GLSCD_RLMTBLRD2DATALO_DATA_LEN                      (32)
#define PP_TSCD_GLSCD_RLMTBLRD2DATALO_DATA_MSK                      (0xFFFFFFFF)
#define PP_TSCD_GLSCD_RLMTBLRD2DATALO_DATA_RST                      (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_RLMTBLRD2DATAHI_REG
 * HW_REG_NAME : glscd_rlmtblrd2datahi
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI_REG             ((TSCD_BASE_ADDR) + 0x164)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI_DATA_OFF                      (0)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI_DATA_LEN                      (32)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI_DATA_MSK                      (0xFFFFFFFF)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI_DATA_RST                      (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_RLMTBLRD2DATAHI2_REG
 * HW_REG_NAME : glscd_rlmtblrd2datahi2
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 3][RO] - MISSING_DESCRIPTION
 *   [ 2: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI2_REG            ((TSCD_BASE_ADDR) + 0x168)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI2_RSVD_2_OFF                   (3)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI2_RSVD_2_LEN                   (29)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI2_RSVD_2_MSK                   (0xFFFFFFF8)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI2_RSVD_2_RST                   (0x0)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI2_DATA_OFF                     (0)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI2_DATA_LEN                     (3)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI2_DATA_MSK                     (0x00000007)
#define PP_TSCD_GLSCD_RLMTBLRD2DATAHI2_DATA_RST                     (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_SCHED_DEBUG_IF_L_REG
 * HW_REG_NAME : glscd_sched_debug_if_l
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_SCHED_DEBUG_IF_L_REG            ((TSCD_BASE_ADDR) + 0x16C)
#define PP_TSCD_GLSCD_SCHED_DEBUG_IF_L_DBG_OFF                      (0)
#define PP_TSCD_GLSCD_SCHED_DEBUG_IF_L_DBG_LEN                      (32)
#define PP_TSCD_GLSCD_SCHED_DEBUG_IF_L_DBG_MSK                      (0xFFFFFFFF)
#define PP_TSCD_GLSCD_SCHED_DEBUG_IF_L_DBG_RST                      (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_SCHED_DEBUG_IF_H_REG
 * HW_REG_NAME : glscd_sched_debug_if_h
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_SCHED_DEBUG_IF_H_REG            ((TSCD_BASE_ADDR) + 0x170)
#define PP_TSCD_GLSCD_SCHED_DEBUG_IF_H_DBG_OFF                      (0)
#define PP_TSCD_GLSCD_SCHED_DEBUG_IF_H_DBG_LEN                      (32)
#define PP_TSCD_GLSCD_SCHED_DEBUG_IF_H_DBG_MSK                      (0xFFFFFFFF)
#define PP_TSCD_GLSCD_SCHED_DEBUG_IF_H_DBG_RST                      (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_WA_DEBUG_IF_L_REG
 * HW_REG_NAME : glscd_wa_debug_if_l
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_WA_DEBUG_IF_L_REG               ((TSCD_BASE_ADDR) + 0x174)
#define PP_TSCD_GLSCD_WA_DEBUG_IF_L_DBG_OFF                         (0)
#define PP_TSCD_GLSCD_WA_DEBUG_IF_L_DBG_LEN                         (32)
#define PP_TSCD_GLSCD_WA_DEBUG_IF_L_DBG_MSK                         (0xFFFFFFFF)
#define PP_TSCD_GLSCD_WA_DEBUG_IF_L_DBG_RST                         (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_WA_DEBUG_IF_H_REG
 * HW_REG_NAME : glscd_wa_debug_if_h
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_WA_DEBUG_IF_H_REG               ((TSCD_BASE_ADDR) + 0x178)
#define PP_TSCD_GLSCD_WA_DEBUG_IF_H_DBG_OFF                         (0)
#define PP_TSCD_GLSCD_WA_DEBUG_IF_H_DBG_LEN                         (32)
#define PP_TSCD_GLSCD_WA_DEBUG_IF_H_DBG_MSK                         (0xFFFFFFFF)
#define PP_TSCD_GLSCD_WA_DEBUG_IF_H_DBG_RST                         (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_L_REG
 * HW_REG_NAME : glscd_bw_cred_debug_if_l
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_L_REG          ((TSCD_BASE_ADDR) + 0x17C)
#define PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_L_DBG_OFF                    (0)
#define PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_L_DBG_LEN                    (32)
#define PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_L_DBG_MSK                    (0xFFFFFFFF)
#define PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_L_DBG_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_H_REG
 * HW_REG_NAME : glscd_bw_cred_debug_if_h
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_H_REG          ((TSCD_BASE_ADDR) + 0x180)
#define PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_H_DBG_OFF                    (0)
#define PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_H_DBG_LEN                    (32)
#define PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_H_DBG_MSK                    (0xFFFFFFFF)
#define PP_TSCD_GLSCD_BW_CRED_DEBUG_IF_H_DBG_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_L_REG
 * HW_REG_NAME : glscd_bw_node_debug_if_l
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_L_REG          ((TSCD_BASE_ADDR) + 0x184)
#define PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_L_DBG_OFF                    (0)
#define PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_L_DBG_LEN                    (32)
#define PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_L_DBG_MSK                    (0xFFFFFFFF)
#define PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_L_DBG_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_H_REG
 * HW_REG_NAME : glscd_bw_node_debug_if_h
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_H_REG          ((TSCD_BASE_ADDR) + 0x188)
#define PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_H_DBG_OFF                    (0)
#define PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_H_DBG_LEN                    (32)
#define PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_H_DBG_MSK                    (0xFFFFFFFF)
#define PP_TSCD_GLSCD_BW_NODE_DEBUG_IF_H_DBG_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_L_REG
 * HW_REG_NAME : glscd_cred_ret_debug_if_l
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_L_REG         ((TSCD_BASE_ADDR) + 0x18C)
#define PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_L_DBG_OFF                   (0)
#define PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_L_DBG_LEN                   (32)
#define PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_L_DBG_MSK                   (0xFFFFFFFF)
#define PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_L_DBG_RST                   (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_H_REG
 * HW_REG_NAME : glscd_cred_ret_debug_if_h
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_H_REG         ((TSCD_BASE_ADDR) + 0x190)
#define PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_H_DBG_OFF                   (0)
#define PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_H_DBG_LEN                   (32)
#define PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_H_DBG_MSK                   (0xFFFFFFFF)
#define PP_TSCD_GLSCD_CRED_RET_DEBUG_IF_H_DBG_RST                   (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_INIT_DEBUG_IF_L_REG
 * HW_REG_NAME : glscd_init_debug_if_l
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_INIT_DEBUG_IF_L_REG             ((TSCD_BASE_ADDR) + 0x194)
#define PP_TSCD_GLSCD_INIT_DEBUG_IF_L_DBG_OFF                       (0)
#define PP_TSCD_GLSCD_INIT_DEBUG_IF_L_DBG_LEN                       (32)
#define PP_TSCD_GLSCD_INIT_DEBUG_IF_L_DBG_MSK                       (0xFFFFFFFF)
#define PP_TSCD_GLSCD_INIT_DEBUG_IF_L_DBG_RST                       (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_INIT_DEBUG_IF_H_REG
 * HW_REG_NAME : glscd_init_debug_if_h
 * DESCRIPTION : Debug Register (2 per internal TSCD machine) Low
 *               Part, NOTE, B Step ONLY register
 *
 *  Register Fields :
 *   [31: 0][RO] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_INIT_DEBUG_IF_H_REG             ((TSCD_BASE_ADDR) + 0x198)
#define PP_TSCD_GLSCD_INIT_DEBUG_IF_H_DBG_OFF                       (0)
#define PP_TSCD_GLSCD_INIT_DEBUG_IF_H_DBG_LEN                       (32)
#define PP_TSCD_GLSCD_INIT_DEBUG_IF_H_DBG_MSK                       (0xFFFFFFFF)
#define PP_TSCD_GLSCD_INIT_DEBUG_IF_H_DBG_RST                       (0x0)

/**
 * SW_REG_NAME : PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_REG
 * HW_REG_NAME : tscd_shared_bw_limit_table_cfg
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to 0
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_REG        ((TSCD_BASE_ADDR) + 0x200)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_14_OFF          (20)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_14_LEN          (12)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_14_MSK          (0xFFF00000)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_14_RST          (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RM_OFF                   (16)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RM_LEN                   (4)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RM_MSK                   (0x000F0000)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RM_RST                   (0x2)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_12_OFF          (13)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_12_LEN          (3)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_12_MSK          (0x0000E000)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_12_RST          (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RME_OFF                  (12)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RME_LEN                  (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RME_MSK                  (0x00001000)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RME_RST                  (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_10_OFF          (10)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_10_LEN          (2)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_10_MSK          (0x00000C00)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_10_RST          (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ERR_CNT_OFF              (9)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ERR_CNT_LEN              (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ERR_CNT_MSK              (0x00000200)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ERR_CNT_RST              (0x1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_FIX_CNT_OFF              (8)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_FIX_CNT_LEN              (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_FIX_CNT_MSK              (0x00000100)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_FIX_CNT_RST              (0x1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_7_OFF           (6)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_7_LEN           (2)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_7_MSK           (0x000000C0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_RESERVED_7_RST           (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_MASK_INT_OFF             (5)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_MASK_INT_LEN             (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_MASK_INT_MSK             (0x00000020)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_MASK_INT_RST             (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_LS_BYPASS_OFF            (4)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_LS_BYPASS_LEN            (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_LS_BYPASS_MSK            (0x00000010)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_LS_BYPASS_RST            (0x1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_LS_FORCE_OFF             (3)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_LS_FORCE_LEN             (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_LS_FORCE_MSK             (0x00000008)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_LS_FORCE_RST             (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_INVERT_2_OFF         (2)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_INVERT_2_LEN         (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_INVERT_2_MSK         (0x00000004)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_INVERT_2_RST         (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_INVERT_1_OFF         (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_INVERT_1_LEN         (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_INVERT_1_MSK         (0x00000002)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_INVERT_1_RST         (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_EN_OFF               (0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_EN_LEN               (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_EN_MSK               (0x00000001)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_CFG_ECC_EN_RST               (0x1)

/**
 * SW_REG_NAME : PP_TSCD__BW_LIMIT_TABLE_CFG_REG
 * HW_REG_NAME : tscd_bw_limit_table_cfg
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to 0
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_TSCD__BW_LIMIT_TABLE_CFG_REG               ((TSCD_BASE_ADDR) + 0x204)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_14_OFF                 (20)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_14_LEN                 (12)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_14_MSK                 (0xFFF00000)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_14_RST                 (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RM_OFF                          (16)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RM_LEN                          (4)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RM_MSK                          (0x000F0000)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RM_RST                          (0x2)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_12_OFF                 (13)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_12_LEN                 (3)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_12_MSK                 (0x0000E000)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_12_RST                 (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RME_OFF                         (12)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RME_LEN                         (1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RME_MSK                         (0x00001000)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RME_RST                         (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_10_OFF                 (10)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_10_LEN                 (2)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_10_MSK                 (0x00000C00)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_10_RST                 (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ERR_CNT_OFF                     (9)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ERR_CNT_LEN                     (1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ERR_CNT_MSK                     (0x00000200)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ERR_CNT_RST                     (0x1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_FIX_CNT_OFF                     (8)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_FIX_CNT_LEN                     (1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_FIX_CNT_MSK                     (0x00000100)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_FIX_CNT_RST                     (0x1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_7_OFF                  (6)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_7_LEN                  (2)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_7_MSK                  (0x000000C0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_RESERVED_7_RST                  (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_MASK_INT_OFF                    (5)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_MASK_INT_LEN                    (1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_MASK_INT_MSK                    (0x00000020)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_MASK_INT_RST                    (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_LS_BYPASS_OFF                   (4)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_LS_BYPASS_LEN                   (1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_LS_BYPASS_MSK                   (0x00000010)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_LS_BYPASS_RST                   (0x1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_LS_FORCE_OFF                    (3)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_LS_FORCE_LEN                    (1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_LS_FORCE_MSK                    (0x00000008)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_LS_FORCE_RST                    (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_INVERT_2_OFF                (2)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_INVERT_2_LEN                (1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_INVERT_2_MSK                (0x00000004)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_INVERT_2_RST                (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_INVERT_1_OFF                (1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_INVERT_1_LEN                (1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_INVERT_1_MSK                (0x00000002)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_INVERT_1_RST                (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_EN_OFF                      (0)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_EN_LEN                      (1)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_EN_MSK                      (0x00000001)
#define PP_TSCD__BW_LIMIT_TABLE_CFG_ECC_EN_RST                      (0x1)

/**
 * SW_REG_NAME : PP_TSCD__NODE_TABLE_CFG_REG
 * HW_REG_NAME : tscd_node_table_cfg
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to 0
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_TSCD__NODE_TABLE_CFG_REG                   ((TSCD_BASE_ADDR) + 0x210)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_14_OFF                     (20)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_14_LEN                     (12)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_14_MSK                     (0xFFF00000)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_14_RST                     (0x0)
#define PP_TSCD__NODE_TABLE_CFG_RM_OFF                              (16)
#define PP_TSCD__NODE_TABLE_CFG_RM_LEN                              (4)
#define PP_TSCD__NODE_TABLE_CFG_RM_MSK                              (0x000F0000)
#define PP_TSCD__NODE_TABLE_CFG_RM_RST                              (0x2)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_12_OFF                     (13)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_12_LEN                     (3)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_12_MSK                     (0x0000E000)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_12_RST                     (0x0)
#define PP_TSCD__NODE_TABLE_CFG_RME_OFF                             (12)
#define PP_TSCD__NODE_TABLE_CFG_RME_LEN                             (1)
#define PP_TSCD__NODE_TABLE_CFG_RME_MSK                             (0x00001000)
#define PP_TSCD__NODE_TABLE_CFG_RME_RST                             (0x0)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_10_OFF                     (10)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_10_LEN                     (2)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_10_MSK                     (0x00000C00)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_10_RST                     (0x0)
#define PP_TSCD__NODE_TABLE_CFG_ERR_CNT_OFF                         (9)
#define PP_TSCD__NODE_TABLE_CFG_ERR_CNT_LEN                         (1)
#define PP_TSCD__NODE_TABLE_CFG_ERR_CNT_MSK                         (0x00000200)
#define PP_TSCD__NODE_TABLE_CFG_ERR_CNT_RST                         (0x1)
#define PP_TSCD__NODE_TABLE_CFG_FIX_CNT_OFF                         (8)
#define PP_TSCD__NODE_TABLE_CFG_FIX_CNT_LEN                         (1)
#define PP_TSCD__NODE_TABLE_CFG_FIX_CNT_MSK                         (0x00000100)
#define PP_TSCD__NODE_TABLE_CFG_FIX_CNT_RST                         (0x1)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_7_OFF                      (6)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_7_LEN                      (2)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_7_MSK                      (0x000000C0)
#define PP_TSCD__NODE_TABLE_CFG_RESERVED_7_RST                      (0x0)
#define PP_TSCD__NODE_TABLE_CFG_MASK_INT_OFF                        (5)
#define PP_TSCD__NODE_TABLE_CFG_MASK_INT_LEN                        (1)
#define PP_TSCD__NODE_TABLE_CFG_MASK_INT_MSK                        (0x00000020)
#define PP_TSCD__NODE_TABLE_CFG_MASK_INT_RST                        (0x0)
#define PP_TSCD__NODE_TABLE_CFG_LS_BYPASS_OFF                       (4)
#define PP_TSCD__NODE_TABLE_CFG_LS_BYPASS_LEN                       (1)
#define PP_TSCD__NODE_TABLE_CFG_LS_BYPASS_MSK                       (0x00000010)
#define PP_TSCD__NODE_TABLE_CFG_LS_BYPASS_RST                       (0x1)
#define PP_TSCD__NODE_TABLE_CFG_LS_FORCE_OFF                        (3)
#define PP_TSCD__NODE_TABLE_CFG_LS_FORCE_LEN                        (1)
#define PP_TSCD__NODE_TABLE_CFG_LS_FORCE_MSK                        (0x00000008)
#define PP_TSCD__NODE_TABLE_CFG_LS_FORCE_RST                        (0x0)
#define PP_TSCD__NODE_TABLE_CFG_ECC_INVERT_2_OFF                    (2)
#define PP_TSCD__NODE_TABLE_CFG_ECC_INVERT_2_LEN                    (1)
#define PP_TSCD__NODE_TABLE_CFG_ECC_INVERT_2_MSK                    (0x00000004)
#define PP_TSCD__NODE_TABLE_CFG_ECC_INVERT_2_RST                    (0x0)
#define PP_TSCD__NODE_TABLE_CFG_ECC_INVERT_1_OFF                    (1)
#define PP_TSCD__NODE_TABLE_CFG_ECC_INVERT_1_LEN                    (1)
#define PP_TSCD__NODE_TABLE_CFG_ECC_INVERT_1_MSK                    (0x00000002)
#define PP_TSCD__NODE_TABLE_CFG_ECC_INVERT_1_RST                    (0x0)
#define PP_TSCD__NODE_TABLE_CFG_ECC_EN_OFF                          (0)
#define PP_TSCD__NODE_TABLE_CFG_ECC_EN_LEN                          (1)
#define PP_TSCD__NODE_TABLE_CFG_ECC_EN_MSK                          (0x00000001)
#define PP_TSCD__NODE_TABLE_CFG_ECC_EN_RST                          (0x1)

/**
 * SW_REG_NAME : PP_TSCD__RL_MAP_TABLE_CFG_REG
 * HW_REG_NAME : tscd_rl_map_table_cfg
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to logic 0
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_TSCD__RL_MAP_TABLE_CFG_REG                 ((TSCD_BASE_ADDR) + 0x214)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_14_OFF                   (20)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_14_LEN                   (12)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_14_MSK                   (0xFFF00000)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_14_RST                   (0x0)
#define PP_TSCD__RL_MAP_TABLE_CFG_RM_OFF                            (16)
#define PP_TSCD__RL_MAP_TABLE_CFG_RM_LEN                            (4)
#define PP_TSCD__RL_MAP_TABLE_CFG_RM_MSK                            (0x000F0000)
#define PP_TSCD__RL_MAP_TABLE_CFG_RM_RST                            (0x2)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_12_OFF                   (13)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_12_LEN                   (3)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_12_MSK                   (0x0000E000)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_12_RST                   (0x0)
#define PP_TSCD__RL_MAP_TABLE_CFG_RME_OFF                           (12)
#define PP_TSCD__RL_MAP_TABLE_CFG_RME_LEN                           (1)
#define PP_TSCD__RL_MAP_TABLE_CFG_RME_MSK                           (0x00001000)
#define PP_TSCD__RL_MAP_TABLE_CFG_RME_RST                           (0x0)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_10_OFF                   (10)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_10_LEN                   (2)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_10_MSK                   (0x00000C00)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_10_RST                   (0x0)
#define PP_TSCD__RL_MAP_TABLE_CFG_ERR_CNT_OFF                       (9)
#define PP_TSCD__RL_MAP_TABLE_CFG_ERR_CNT_LEN                       (1)
#define PP_TSCD__RL_MAP_TABLE_CFG_ERR_CNT_MSK                       (0x00000200)
#define PP_TSCD__RL_MAP_TABLE_CFG_ERR_CNT_RST                       (0x1)
#define PP_TSCD__RL_MAP_TABLE_CFG_FIX_CNT_OFF                       (8)
#define PP_TSCD__RL_MAP_TABLE_CFG_FIX_CNT_LEN                       (1)
#define PP_TSCD__RL_MAP_TABLE_CFG_FIX_CNT_MSK                       (0x00000100)
#define PP_TSCD__RL_MAP_TABLE_CFG_FIX_CNT_RST                       (0x1)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_7_OFF                    (6)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_7_LEN                    (2)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_7_MSK                    (0x000000C0)
#define PP_TSCD__RL_MAP_TABLE_CFG_RESERVED_7_RST                    (0x0)
#define PP_TSCD__RL_MAP_TABLE_CFG_MASK_INT_OFF                      (5)
#define PP_TSCD__RL_MAP_TABLE_CFG_MASK_INT_LEN                      (1)
#define PP_TSCD__RL_MAP_TABLE_CFG_MASK_INT_MSK                      (0x00000020)
#define PP_TSCD__RL_MAP_TABLE_CFG_MASK_INT_RST                      (0x0)
#define PP_TSCD__RL_MAP_TABLE_CFG_LS_BYPASS_OFF                     (4)
#define PP_TSCD__RL_MAP_TABLE_CFG_LS_BYPASS_LEN                     (1)
#define PP_TSCD__RL_MAP_TABLE_CFG_LS_BYPASS_MSK                     (0x00000010)
#define PP_TSCD__RL_MAP_TABLE_CFG_LS_BYPASS_RST                     (0x1)
#define PP_TSCD__RL_MAP_TABLE_CFG_LS_FORCE_OFF                      (3)
#define PP_TSCD__RL_MAP_TABLE_CFG_LS_FORCE_LEN                      (1)
#define PP_TSCD__RL_MAP_TABLE_CFG_LS_FORCE_MSK                      (0x00000008)
#define PP_TSCD__RL_MAP_TABLE_CFG_LS_FORCE_RST                      (0x0)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_INVERT_2_OFF                  (2)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_INVERT_2_LEN                  (1)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_INVERT_2_MSK                  (0x00000004)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_INVERT_2_RST                  (0x0)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_INVERT_1_OFF                  (1)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_INVERT_1_LEN                  (1)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_INVERT_1_MSK                  (0x00000002)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_INVERT_1_RST                  (0x0)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_EN_OFF                        (0)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_EN_LEN                        (1)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_EN_MSK                        (0x00000001)
#define PP_TSCD__RL_MAP_TABLE_CFG_ECC_EN_RST                        (0x1)

/**
 * SW_REG_NAME : PP_TSCD__BRANCH_TABLE_CFG_REG
 * HW_REG_NAME : tscd_branch_table_cfg
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:20][RO] - MISSING_DESCRIPTION
 *   [19:16][RW] - Read-Write margin Input for port A. This input is
 *                 used for setting the Read-Write margin. It
 *                 programs  the sense amp differential setting. and
 *                 allows the trade off between speed and robustness.
 *                 RM[1:0] = 2'b00 is the slowest possible mode of
 *                 operation for the memory. This setting is required
 *                 for VDDMIN operation.    RM[1:0] values control
 *                 access time and cycle time of the memory. Refer to
 *                 the timing table for more  details.    RM[3:2] are
 *                 factory pins reserved for debug mode. User should
 *                 tie these pins to 0
 *   [15:13][RO] - MISSING_DESCRIPTION
 *   [12:12][RW] - Read-Write margin Enable Input for port A. This
 *                 selects between the default RM setting, and the RM
 *                 settting.
 *   [11:10][RO] - MISSING_DESCRIPTION
 *   [ 9: 9][RW] - Count ECC un-correctable error events
 *   [ 8: 8][RW] - Count ECC correctable error events
 *   [ 7: 6][RO] - MISSING_DESCRIPTION
 *   [ 5: 5][RW] - Mask ECC un-correctable error interrupt
 *                 generation.
 *   [ 4: 4][RW] - Light Sleep mechanism bypass. '1' - prevent the
 *                 memory from entering light-sleep mode.
 *   [ 3: 3][RW] - Light Sleep Force    Force the entrence of the
 *                 memory to the Light Sleep mode.
 *   [ 2: 2][RW] - ECC Invert for testing
 *   [ 1: 1][RW] - ECC Invert for testing
 *   [ 0: 0][RW] - ECC mechanism enable. '1' - Enabled. '0' -
 *                 Disabled
 *
 */
#define PP_TSCD__BRANCH_TABLE_CFG_REG                 ((TSCD_BASE_ADDR) + 0x218)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_14_OFF                   (20)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_14_LEN                   (12)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_14_MSK                   (0xFFF00000)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_14_RST                   (0x0)
#define PP_TSCD__BRANCH_TABLE_CFG_RM_OFF                            (16)
#define PP_TSCD__BRANCH_TABLE_CFG_RM_LEN                            (4)
#define PP_TSCD__BRANCH_TABLE_CFG_RM_MSK                            (0x000F0000)
#define PP_TSCD__BRANCH_TABLE_CFG_RM_RST                            (0x2)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_12_OFF                   (13)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_12_LEN                   (3)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_12_MSK                   (0x0000E000)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_12_RST                   (0x0)
#define PP_TSCD__BRANCH_TABLE_CFG_RME_OFF                           (12)
#define PP_TSCD__BRANCH_TABLE_CFG_RME_LEN                           (1)
#define PP_TSCD__BRANCH_TABLE_CFG_RME_MSK                           (0x00001000)
#define PP_TSCD__BRANCH_TABLE_CFG_RME_RST                           (0x0)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_10_OFF                   (10)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_10_LEN                   (2)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_10_MSK                   (0x00000C00)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_10_RST                   (0x0)
#define PP_TSCD__BRANCH_TABLE_CFG_ERR_CNT_OFF                       (9)
#define PP_TSCD__BRANCH_TABLE_CFG_ERR_CNT_LEN                       (1)
#define PP_TSCD__BRANCH_TABLE_CFG_ERR_CNT_MSK                       (0x00000200)
#define PP_TSCD__BRANCH_TABLE_CFG_ERR_CNT_RST                       (0x1)
#define PP_TSCD__BRANCH_TABLE_CFG_FIX_CNT_OFF                       (8)
#define PP_TSCD__BRANCH_TABLE_CFG_FIX_CNT_LEN                       (1)
#define PP_TSCD__BRANCH_TABLE_CFG_FIX_CNT_MSK                       (0x00000100)
#define PP_TSCD__BRANCH_TABLE_CFG_FIX_CNT_RST                       (0x1)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_7_OFF                    (6)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_7_LEN                    (2)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_7_MSK                    (0x000000C0)
#define PP_TSCD__BRANCH_TABLE_CFG_RESERVED_7_RST                    (0x0)
#define PP_TSCD__BRANCH_TABLE_CFG_MASK_INT_OFF                      (5)
#define PP_TSCD__BRANCH_TABLE_CFG_MASK_INT_LEN                      (1)
#define PP_TSCD__BRANCH_TABLE_CFG_MASK_INT_MSK                      (0x00000020)
#define PP_TSCD__BRANCH_TABLE_CFG_MASK_INT_RST                      (0x0)
#define PP_TSCD__BRANCH_TABLE_CFG_LS_BYPASS_OFF                     (4)
#define PP_TSCD__BRANCH_TABLE_CFG_LS_BYPASS_LEN                     (1)
#define PP_TSCD__BRANCH_TABLE_CFG_LS_BYPASS_MSK                     (0x00000010)
#define PP_TSCD__BRANCH_TABLE_CFG_LS_BYPASS_RST                     (0x1)
#define PP_TSCD__BRANCH_TABLE_CFG_LS_FORCE_OFF                      (3)
#define PP_TSCD__BRANCH_TABLE_CFG_LS_FORCE_LEN                      (1)
#define PP_TSCD__BRANCH_TABLE_CFG_LS_FORCE_MSK                      (0x00000008)
#define PP_TSCD__BRANCH_TABLE_CFG_LS_FORCE_RST                      (0x0)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_INVERT_2_OFF                  (2)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_INVERT_2_LEN                  (1)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_INVERT_2_MSK                  (0x00000004)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_INVERT_2_RST                  (0x0)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_INVERT_1_OFF                  (1)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_INVERT_1_LEN                  (1)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_INVERT_1_MSK                  (0x00000002)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_INVERT_1_RST                  (0x0)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_EN_OFF                        (0)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_EN_LEN                        (1)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_EN_MSK                        (0x00000001)
#define PP_TSCD__BRANCH_TABLE_CFG_ECC_EN_RST                        (0x1)

/**
 * SW_REG_NAME : PP_TSCD__NODE_TABLE_STATUS_REG
 * HW_REG_NAME : tscd_node_table_status
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_TSCD__NODE_TABLE_STATUS_REG                ((TSCD_BASE_ADDR) + 0x220)
#define PP_TSCD__NODE_TABLE_STATUS_RESERVED_5_OFF                   (4)
#define PP_TSCD__NODE_TABLE_STATUS_RESERVED_5_LEN                   (28)
#define PP_TSCD__NODE_TABLE_STATUS_RESERVED_5_MSK                   (0xFFFFFFF0)
#define PP_TSCD__NODE_TABLE_STATUS_RESERVED_5_RST                   (0x0)
#define PP_TSCD__NODE_TABLE_STATUS_GLOBAL_INIT_DONE_OFF             (3)
#define PP_TSCD__NODE_TABLE_STATUS_GLOBAL_INIT_DONE_LEN             (1)
#define PP_TSCD__NODE_TABLE_STATUS_GLOBAL_INIT_DONE_MSK             (0x00000008)
#define PP_TSCD__NODE_TABLE_STATUS_GLOBAL_INIT_DONE_RST             (0x1)
#define PP_TSCD__NODE_TABLE_STATUS_INIT_DONE_OFF                    (2)
#define PP_TSCD__NODE_TABLE_STATUS_INIT_DONE_LEN                    (1)
#define PP_TSCD__NODE_TABLE_STATUS_INIT_DONE_MSK                    (0x00000004)
#define PP_TSCD__NODE_TABLE_STATUS_INIT_DONE_RST                    (0x1)
#define PP_TSCD__NODE_TABLE_STATUS_ECC_FIX_OFF                      (1)
#define PP_TSCD__NODE_TABLE_STATUS_ECC_FIX_LEN                      (1)
#define PP_TSCD__NODE_TABLE_STATUS_ECC_FIX_MSK                      (0x00000002)
#define PP_TSCD__NODE_TABLE_STATUS_ECC_FIX_RST                      (0x0)
#define PP_TSCD__NODE_TABLE_STATUS_ECC_ERR_OFF                      (0)
#define PP_TSCD__NODE_TABLE_STATUS_ECC_ERR_LEN                      (1)
#define PP_TSCD__NODE_TABLE_STATUS_ECC_ERR_MSK                      (0x00000001)
#define PP_TSCD__NODE_TABLE_STATUS_ECC_ERR_RST                      (0x0)

/**
 * SW_REG_NAME : PP_TSCD__RL_MAP_TABLE_STATUS_REG
 * HW_REG_NAME : tscd_rl_map_table_status
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_TSCD__RL_MAP_TABLE_STATUS_REG              ((TSCD_BASE_ADDR) + 0x224)
#define PP_TSCD__RL_MAP_TABLE_STATUS_RESERVED_5_OFF                 (4)
#define PP_TSCD__RL_MAP_TABLE_STATUS_RESERVED_5_LEN                 (28)
#define PP_TSCD__RL_MAP_TABLE_STATUS_RESERVED_5_MSK                 (0xFFFFFFF0)
#define PP_TSCD__RL_MAP_TABLE_STATUS_RESERVED_5_RST                 (0x0)
#define PP_TSCD__RL_MAP_TABLE_STATUS_GLOBAL_INIT_DONE_OFF           (3)
#define PP_TSCD__RL_MAP_TABLE_STATUS_GLOBAL_INIT_DONE_LEN           (1)
#define PP_TSCD__RL_MAP_TABLE_STATUS_GLOBAL_INIT_DONE_MSK           (0x00000008)
#define PP_TSCD__RL_MAP_TABLE_STATUS_GLOBAL_INIT_DONE_RST           (0x1)
#define PP_TSCD__RL_MAP_TABLE_STATUS_INIT_DONE_OFF                  (2)
#define PP_TSCD__RL_MAP_TABLE_STATUS_INIT_DONE_LEN                  (1)
#define PP_TSCD__RL_MAP_TABLE_STATUS_INIT_DONE_MSK                  (0x00000004)
#define PP_TSCD__RL_MAP_TABLE_STATUS_INIT_DONE_RST                  (0x1)
#define PP_TSCD__RL_MAP_TABLE_STATUS_ECC_FIX_OFF                    (1)
#define PP_TSCD__RL_MAP_TABLE_STATUS_ECC_FIX_LEN                    (1)
#define PP_TSCD__RL_MAP_TABLE_STATUS_ECC_FIX_MSK                    (0x00000002)
#define PP_TSCD__RL_MAP_TABLE_STATUS_ECC_FIX_RST                    (0x0)
#define PP_TSCD__RL_MAP_TABLE_STATUS_ECC_ERR_OFF                    (0)
#define PP_TSCD__RL_MAP_TABLE_STATUS_ECC_ERR_LEN                    (1)
#define PP_TSCD__RL_MAP_TABLE_STATUS_ECC_ERR_MSK                    (0x00000001)
#define PP_TSCD__RL_MAP_TABLE_STATUS_ECC_ERR_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD__BW_LIMIT_TABLE_STATUS_REG
 * HW_REG_NAME : tscd_bw_limit_table_status
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_REG            ((TSCD_BASE_ADDR) + 0x228)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_RESERVED_5_OFF               (4)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_RESERVED_5_LEN               (28)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_RESERVED_5_MSK               (0xFFFFFFF0)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_RESERVED_5_RST               (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_GLOBAL_INIT_DONE_OFF         (3)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_GLOBAL_INIT_DONE_LEN         (1)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_GLOBAL_INIT_DONE_MSK         (0x00000008)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_GLOBAL_INIT_DONE_RST         (0x1)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_INIT_DONE_OFF                (2)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_INIT_DONE_LEN                (1)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_INIT_DONE_MSK                (0x00000004)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_INIT_DONE_RST                (0x1)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_ECC_FIX_OFF                  (1)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_ECC_FIX_LEN                  (1)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_ECC_FIX_MSK                  (0x00000002)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_ECC_FIX_RST                  (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_ECC_ERR_OFF                  (0)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_ECC_ERR_LEN                  (1)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_ECC_ERR_MSK                  (0x00000001)
#define PP_TSCD__BW_LIMIT_TABLE_STATUS_ECC_ERR_RST                  (0x0)

/**
 * SW_REG_NAME : PP_TSCD__BRANCH_TABLE_STATUS_REG
 * HW_REG_NAME : tscd_branch_table_status
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_TSCD__BRANCH_TABLE_STATUS_REG              ((TSCD_BASE_ADDR) + 0x230)
#define PP_TSCD__BRANCH_TABLE_STATUS_RESERVED_5_OFF                 (4)
#define PP_TSCD__BRANCH_TABLE_STATUS_RESERVED_5_LEN                 (28)
#define PP_TSCD__BRANCH_TABLE_STATUS_RESERVED_5_MSK                 (0xFFFFFFF0)
#define PP_TSCD__BRANCH_TABLE_STATUS_RESERVED_5_RST                 (0x0)
#define PP_TSCD__BRANCH_TABLE_STATUS_GLOBAL_INIT_DONE_OFF           (3)
#define PP_TSCD__BRANCH_TABLE_STATUS_GLOBAL_INIT_DONE_LEN           (1)
#define PP_TSCD__BRANCH_TABLE_STATUS_GLOBAL_INIT_DONE_MSK           (0x00000008)
#define PP_TSCD__BRANCH_TABLE_STATUS_GLOBAL_INIT_DONE_RST           (0x1)
#define PP_TSCD__BRANCH_TABLE_STATUS_INIT_DONE_OFF                  (2)
#define PP_TSCD__BRANCH_TABLE_STATUS_INIT_DONE_LEN                  (1)
#define PP_TSCD__BRANCH_TABLE_STATUS_INIT_DONE_MSK                  (0x00000004)
#define PP_TSCD__BRANCH_TABLE_STATUS_INIT_DONE_RST                  (0x1)
#define PP_TSCD__BRANCH_TABLE_STATUS_ECC_FIX_OFF                    (1)
#define PP_TSCD__BRANCH_TABLE_STATUS_ECC_FIX_LEN                    (1)
#define PP_TSCD__BRANCH_TABLE_STATUS_ECC_FIX_MSK                    (0x00000002)
#define PP_TSCD__BRANCH_TABLE_STATUS_ECC_FIX_RST                    (0x0)
#define PP_TSCD__BRANCH_TABLE_STATUS_ECC_ERR_OFF                    (0)
#define PP_TSCD__BRANCH_TABLE_STATUS_ECC_ERR_LEN                    (1)
#define PP_TSCD__BRANCH_TABLE_STATUS_ECC_ERR_MSK                    (0x00000001)
#define PP_TSCD__BRANCH_TABLE_STATUS_ECC_ERR_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_REG
 * HW_REG_NAME : tscd_shared_bw_limit_table_status
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 4][RO] - MISSING_DESCRIPTION
 *   [ 3: 3][RO] - All block's memories initialization completed
 *                 indication.
 *   [ 2: 2][RO] - Memory initialization completed indication.
 *   [ 1: 1][RO] - ECC correctable error indication (cleared on read)
 *   [ 0: 0][RO] - ECC un-correctable error indication (cleared on
 *                 read)
 *
 */
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_REG     ((TSCD_BASE_ADDR) + 0x234)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_RESERVED_5_OFF        (4)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_RESERVED_5_LEN        (28)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_RESERVED_5_MSK        (0xFFFFFFF0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_RESERVED_5_RST        (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_GLOBAL_INIT_DONE_OFF  (3)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_GLOBAL_INIT_DONE_LEN  (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_GLOBAL_INIT_DONE_MSK  (0x00000008)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_GLOBAL_INIT_DONE_RST  (0x1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_INIT_DONE_OFF         (2)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_INIT_DONE_LEN         (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_INIT_DONE_MSK         (0x00000004)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_INIT_DONE_RST         (0x1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_ECC_FIX_OFF           (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_ECC_FIX_LEN           (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_ECC_FIX_MSK           (0x00000002)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_ECC_FIX_RST           (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_ECC_ERR_OFF           (0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_ECC_ERR_LEN           (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_ECC_ERR_MSK           (0x00000001)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_STATUS_ECC_ERR_RST           (0x0)

/**
 * SW_REG_NAME : PP_TSCD__ECC_UNCOR_ERR_REG
 * HW_REG_NAME : tscd_ecc_uncor_err
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:12][RO] - RESERVED
 *   [11: 0][RO] - Counter
 *
 */
#define PP_TSCD__ECC_UNCOR_ERR_REG                    ((TSCD_BASE_ADDR) + 0x238)
#define PP_TSCD__ECC_UNCOR_ERR_RESERVED_2_OFF                       (12)
#define PP_TSCD__ECC_UNCOR_ERR_RESERVED_2_LEN                       (20)
#define PP_TSCD__ECC_UNCOR_ERR_RESERVED_2_MSK                       (0xFFFFF000)
#define PP_TSCD__ECC_UNCOR_ERR_RESERVED_2_RST                       (0x0)
#define PP_TSCD__ECC_UNCOR_ERR_CNT_OFF                              (0)
#define PP_TSCD__ECC_UNCOR_ERR_CNT_LEN                              (12)
#define PP_TSCD__ECC_UNCOR_ERR_CNT_MSK                              (0x00000FFF)
#define PP_TSCD__ECC_UNCOR_ERR_CNT_RST                              (0x0)

/**
 * SW_REG_NAME : PP_TSCD__ECC_COR_ERR_REG
 * HW_REG_NAME : tscd_ecc_cor_err
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:12][RO] - RESERVED
 *   [11: 0][RO] - Counter
 *
 */
#define PP_TSCD__ECC_COR_ERR_REG                      ((TSCD_BASE_ADDR) + 0x23C)
#define PP_TSCD__ECC_COR_ERR_RESERVED_2_OFF                         (12)
#define PP_TSCD__ECC_COR_ERR_RESERVED_2_LEN                         (20)
#define PP_TSCD__ECC_COR_ERR_RESERVED_2_MSK                         (0xFFFFF000)
#define PP_TSCD__ECC_COR_ERR_RESERVED_2_RST                         (0x0)
#define PP_TSCD__ECC_COR_ERR_CNT_OFF                                (0)
#define PP_TSCD__ECC_COR_ERR_CNT_LEN                                (12)
#define PP_TSCD__ECC_COR_ERR_CNT_MSK                                (0x00000FFF)
#define PP_TSCD__ECC_COR_ERR_CNT_RST                                (0x0)

/**
 * SW_REG_NAME : PP_TSCD_PFGEN_PORTNUM__REG
 * HW_REG_NAME : pfgen_portnum_tscd
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved.
 *   [ 1: 0][RW] - Port Number. Indicates the LAN port connected to
 *                 this function.    00b = Port 0.    01b = Port 1.
 *                 10b = Port 2.    11b = Port 3.
 *
 */
#define PP_TSCD_PFGEN_PORTNUM__REG                    ((TSCD_BASE_ADDR) + 0x240)
#define PP_TSCD_PFGEN_PORTNUM__RSVD_2_OFF                           (2)
#define PP_TSCD_PFGEN_PORTNUM__RSVD_2_LEN                           (30)
#define PP_TSCD_PFGEN_PORTNUM__RSVD_2_MSK                           (0xFFFFFFFC)
#define PP_TSCD_PFGEN_PORTNUM__RSVD_2_RST                           (0x0)
#define PP_TSCD_PFGEN_PORTNUM__PORT_NUM_OFF                         (0)
#define PP_TSCD_PFGEN_PORTNUM__PORT_NUM_LEN                         (2)
#define PP_TSCD_PFGEN_PORTNUM__PORT_NUM_MSK                         (0x00000003)
#define PP_TSCD_PFGEN_PORTNUM__PORT_NUM_RST                         (0x0)
/**
 * REG_IDX_ACCESS   : PP_TSCD_PFGEN_PORTNUM__REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_TSCD_PFGEN_PORTNUM__REG_IDX(idx) \
	(PP_TSCD_PFGEN_PORTNUM__REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_TSCD_PF_VT_PFALLOC__REG
 * HW_REG_NAME : pf_vt_pfalloc_tscd
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][RW] - The FIRSTVF and LASTVF fields in this register are
 *                 valid. If cleared no VFs are allocated to this PF.
 *                 If cleared, the SR-IOV capability should not be
 *                 exposed for this PF.
 *   [30:16][RO] - MISSING_DESCRIPTION
 *   [15: 8][RW] - The last VF allocated to this PF. Valid only if
 *                 the VALID flag is set.    Valid values are 0-127
 *   [ 7: 0][RW] - The first VF allocated to this PF. Valid only if
 *                 the VALID flag is set.    Valid values are 0-127
 *
 */
#define PP_TSCD_PF_VT_PFALLOC__REG                    ((TSCD_BASE_ADDR) + 0x280)
#define PP_TSCD_PF_VT_PFALLOC__VALID_OFF                            (31)
#define PP_TSCD_PF_VT_PFALLOC__VALID_LEN                            (1)
#define PP_TSCD_PF_VT_PFALLOC__VALID_MSK                            (0x80000000)
#define PP_TSCD_PF_VT_PFALLOC__VALID_RST                            (0x0)
#define PP_TSCD_PF_VT_PFALLOC__RESERVED_3_OFF                       (16)
#define PP_TSCD_PF_VT_PFALLOC__RESERVED_3_LEN                       (15)
#define PP_TSCD_PF_VT_PFALLOC__RESERVED_3_MSK                       (0x7FFF0000)
#define PP_TSCD_PF_VT_PFALLOC__RESERVED_3_RST                       (0x0)
#define PP_TSCD_PF_VT_PFALLOC__LASTVF_OFF                           (8)
#define PP_TSCD_PF_VT_PFALLOC__LASTVF_LEN                           (8)
#define PP_TSCD_PF_VT_PFALLOC__LASTVF_MSK                           (0x0000FF00)
#define PP_TSCD_PF_VT_PFALLOC__LASTVF_RST                           (0x0)
#define PP_TSCD_PF_VT_PFALLOC__FIRSTVF_OFF                          (0)
#define PP_TSCD_PF_VT_PFALLOC__FIRSTVF_LEN                          (8)
#define PP_TSCD_PF_VT_PFALLOC__FIRSTVF_MSK                          (0x000000FF)
#define PP_TSCD_PF_VT_PFALLOC__FIRSTVF_RST                          (0x0)
/**
 * REG_IDX_ACCESS   : PP_TSCD_PF_VT_PFALLOC__REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_TSCD_PF_VT_PFALLOC__REG_IDX(idx) \
	(PP_TSCD_PF_VT_PFALLOC__REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_TSCD_VSI_PORT_REG
 * HW_REG_NAME : vsi_port
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 2][RO] - Reserved
 *   [ 1: 0][RW] - Port Number - Indicates the LAN port connected to
 *                 this VSI    00 - Port 0    01 - Port 1    10 -
 *                 Port 2    11 - Port 3
 *
 */
#define PP_TSCD_VSI_PORT_REG                          ((TSCD_BASE_ADDR) + 0x2C0)
#define PP_TSCD_VSI_PORT_RSVD_2_OFF                                 (2)
#define PP_TSCD_VSI_PORT_RSVD_2_LEN                                 (30)
#define PP_TSCD_VSI_PORT_RSVD_2_MSK                                 (0xFFFFFFFC)
#define PP_TSCD_VSI_PORT_RSVD_2_RST                                 (0x0)
#define PP_TSCD_VSI_PORT_PORT_NUM_OFF                               (0)
#define PP_TSCD_VSI_PORT_PORT_NUM_LEN                               (2)
#define PP_TSCD_VSI_PORT_PORT_NUM_MSK                               (0x00000003)
#define PP_TSCD_VSI_PORT_PORT_NUM_RST                               (0x0)
/**
 * REG_IDX_ACCESS   : PP_TSCD_VSI_PORT_REG_IDX
 * NUM OF REGISTERS : 384
 */
#define PP_TSCD_VSI_PORT_REG_IDX(idx) \
	(PP_TSCD_VSI_PORT_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_TSCD__NODE_TABLE_0_DBG_CTL_REG
 * HW_REG_NAME : tscd_node_table_0_dbg_ctl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][RO] - This bit is cleared after the START bit is set. It
 *                 is set back again when the Read transaction is
 *                 complete  and Valid Data is in the DATA Register.
 *   [30:30][RW] - Writing a 1b to this bit causes the Read
 *                 Transaction. It is self-cleared by the hardware
 *                 when transaction  is finished.andedsp;
 *   [29:26][RO] - MISSING_DESCRIPTION
 *   [25:18][RW] - This field determines which DWORD part of the
 *                 Memory Data will be returned to the Data Register
 *                 (In  case the Memory width andgt; 32b, otherwise
 *                 this field is neglected).
 *   [17: 0][RW] - This field is written along with START and
 *                 DWORD_SELECT fields to indicate which Memory Row
 *                 to read.
 *
 */
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_REG             ((TSCD_BASE_ADDR) + 0x8C0)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_DONE_OFF                      (31)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_DONE_LEN                      (1)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_DONE_MSK                      (0x80000000)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_DONE_RST                      (0x1)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_RD_EN_OFF                     (30)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_RD_EN_LEN                     (1)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_RD_EN_MSK                     (0x40000000)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_RD_EN_RST                     (0x0)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_RESERVED_3_OFF                (26)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_RESERVED_3_LEN                (4)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_RESERVED_3_MSK                (0x3C000000)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_RESERVED_3_RST                (0x0)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_DW_SEL_OFF                    (18)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_DW_SEL_LEN                    (8)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_DW_SEL_MSK                    (0x03FC0000)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_DW_SEL_RST                    (0x0)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_ADR_OFF                       (0)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_ADR_LEN                       (18)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_ADR_MSK                       (0x0003FFFF)
#define PP_TSCD__NODE_TABLE_0_DBG_CTL_ADR_RST                       (0x0)

/**
 * SW_REG_NAME : PP_TSCD__NODE_TABLE_1_DBG_CTL_REG
 * HW_REG_NAME : tscd_node_table_1_dbg_ctl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][RO] - This bit is cleared after the START bit is set. It
 *                 is set back again when the Read transaction is
 *                 complete  and Valid Data is in the DATA Register.
 *   [30:30][RW] - Writing a 1b to this bit causes the Read
 *                 Transaction. It is self-cleared by the hardware
 *                 when transaction  is finished.andedsp;
 *   [29:26][RO] - MISSING_DESCRIPTION
 *   [25:18][RW] - This field determines which DWORD part of the
 *                 Memory Data will be returned to the Data Register
 *                 (In  case the Memory width andgt; 32b, otherwise
 *                 this field is neglected).
 *   [17: 0][RW] - This field is written along with START and
 *                 DWORD_SELECT fields to indicate which Memory Row
 *                 to read.
 *
 */
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_REG             ((TSCD_BASE_ADDR) + 0x8C4)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_DONE_OFF                      (31)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_DONE_LEN                      (1)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_DONE_MSK                      (0x80000000)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_DONE_RST                      (0x1)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_RD_EN_OFF                     (30)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_RD_EN_LEN                     (1)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_RD_EN_MSK                     (0x40000000)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_RD_EN_RST                     (0x0)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_RESERVED_3_OFF                (26)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_RESERVED_3_LEN                (4)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_RESERVED_3_MSK                (0x3C000000)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_RESERVED_3_RST                (0x0)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_DW_SEL_OFF                    (18)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_DW_SEL_LEN                    (8)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_DW_SEL_MSK                    (0x03FC0000)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_DW_SEL_RST                    (0x0)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_ADR_OFF                       (0)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_ADR_LEN                       (18)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_ADR_MSK                       (0x0003FFFF)
#define PP_TSCD__NODE_TABLE_1_DBG_CTL_ADR_RST                       (0x0)

/**
 * SW_REG_NAME : PP_TSCD__NODE_TABLE_2_DBG_CTL_REG
 * HW_REG_NAME : tscd_node_table_2_dbg_ctl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][RO] - This bit is cleared after the START bit is set. It
 *                 is set back again when the Read transaction is
 *                 complete  and Valid Data is in the DATA Register.
 *   [30:30][RW] - Writing a 1b to this bit causes the Read
 *                 Transaction. It is self-cleared by the hardware
 *                 when transaction  is finished.andedsp;
 *   [29:26][RO] - MISSING_DESCRIPTION
 *   [25:18][RW] - This field determines which DWORD part of the
 *                 Memory Data will be returned to the Data Register
 *                 (In  case the Memory width andgt; 32b, otherwise
 *                 this field is neglected).
 *   [17: 0][RW] - This field is written along with START and
 *                 DWORD_SELECT fields to indicate which Memory Row
 *                 to read.
 *
 */
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_REG             ((TSCD_BASE_ADDR) + 0x8C8)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_DONE_OFF                      (31)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_DONE_LEN                      (1)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_DONE_MSK                      (0x80000000)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_DONE_RST                      (0x1)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_RD_EN_OFF                     (30)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_RD_EN_LEN                     (1)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_RD_EN_MSK                     (0x40000000)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_RD_EN_RST                     (0x0)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_RESERVED_3_OFF                (26)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_RESERVED_3_LEN                (4)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_RESERVED_3_MSK                (0x3C000000)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_RESERVED_3_RST                (0x0)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_DW_SEL_OFF                    (18)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_DW_SEL_LEN                    (8)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_DW_SEL_MSK                    (0x03FC0000)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_DW_SEL_RST                    (0x0)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_ADR_OFF                       (0)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_ADR_LEN                       (18)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_ADR_MSK                       (0x0003FFFF)
#define PP_TSCD__NODE_TABLE_2_DBG_CTL_ADR_RST                       (0x0)

/**
 * SW_REG_NAME : PP_TSCD__NODE_TABLE_3_DBG_CTL_REG
 * HW_REG_NAME : tscd_node_table_3_dbg_ctl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][RO] - This bit is cleared after the START bit is set. It
 *                 is set back again when the Read transaction is
 *                 complete  and Valid Data is in the DATA Register.
 *   [30:30][RW] - Writing a 1b to this bit causes the Read
 *                 Transaction. It is self-cleared by the hardware
 *                 when transaction  is finished.andedsp;
 *   [29:26][RO] - MISSING_DESCRIPTION
 *   [25:18][RW] - This field determines which DWORD part of the
 *                 Memory Data will be returned to the Data Register
 *                 (In  case the Memory width andgt; 32b, otherwise
 *                 this field is neglected).
 *   [17: 0][RW] - This field is written along with START and
 *                 DWORD_SELECT fields to indicate which Memory Row
 *                 to read.
 *
 */
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_REG             ((TSCD_BASE_ADDR) + 0x8CC)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_DONE_OFF                      (31)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_DONE_LEN                      (1)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_DONE_MSK                      (0x80000000)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_DONE_RST                      (0x1)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_RD_EN_OFF                     (30)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_RD_EN_LEN                     (1)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_RD_EN_MSK                     (0x40000000)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_RD_EN_RST                     (0x0)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_RESERVED_3_OFF                (26)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_RESERVED_3_LEN                (4)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_RESERVED_3_MSK                (0x3C000000)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_RESERVED_3_RST                (0x0)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_DW_SEL_OFF                    (18)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_DW_SEL_LEN                    (8)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_DW_SEL_MSK                    (0x03FC0000)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_DW_SEL_RST                    (0x0)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_ADR_OFF                       (0)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_ADR_LEN                       (18)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_ADR_MSK                       (0x0003FFFF)
#define PP_TSCD__NODE_TABLE_3_DBG_CTL_ADR_RST                       (0x0)

/**
 * SW_REG_NAME : PP_TSCD__BRANCH_TABLE_DBG_CTL_REG
 * HW_REG_NAME : tscd_branch_table_dbg_ctl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][RO] - This bit is cleared after the START bit is set. It
 *                 is set back again when the Read transaction is
 *                 complete  and Valid Data is in the DATA Register.
 *   [30:30][RW] - Writing a 1b to this bit causes the Read
 *                 Transaction. It is self-cleared by the hardware
 *                 when transaction  is finished.andedsp;
 *   [29:26][RO] - MISSING_DESCRIPTION
 *   [25:18][RW] - This field determines which DWORD part of the
 *                 Memory Data will be returned to the Data Register
 *                 (In  case the Memory width andgt; 32b, otherwise
 *                 this field is neglected).
 *   [17: 0][RW] - This field is written along with START and
 *                 DWORD_SELECT fields to indicate which Memory Row
 *                 to read.
 *
 */
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_REG             ((TSCD_BASE_ADDR) + 0x8D0)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_DONE_OFF                      (31)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_DONE_LEN                      (1)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_DONE_MSK                      (0x80000000)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_DONE_RST                      (0x1)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_RD_EN_OFF                     (30)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_RD_EN_LEN                     (1)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_RD_EN_MSK                     (0x40000000)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_RD_EN_RST                     (0x0)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_RESERVED_3_OFF                (26)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_RESERVED_3_LEN                (4)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_RESERVED_3_MSK                (0x3C000000)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_RESERVED_3_RST                (0x0)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_DW_SEL_OFF                    (18)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_DW_SEL_LEN                    (8)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_DW_SEL_MSK                    (0x03FC0000)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_DW_SEL_RST                    (0x0)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_ADR_OFF                       (0)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_ADR_LEN                       (18)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_ADR_MSK                       (0x0003FFFF)
#define PP_TSCD__BRANCH_TABLE_DBG_CTL_ADR_RST                       (0x0)

/**
 * SW_REG_NAME : PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_REG
 * HW_REG_NAME : tscd_bw_limit_table_0_dbg_ctl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][RO] - This bit is cleared after the START bit is set. It
 *                 is set back again when the Read transaction is
 *                 complete  and Valid Data is in the DATA Register.
 *   [30:30][RW] - Writing a 1b to this bit causes the Read
 *                 Transaction. It is self-cleared by the hardware
 *                 when transaction  is finished.andedsp;
 *   [29:26][RO] - MISSING_DESCRIPTION
 *   [25:18][RW] - This field determines which DWORD part of the
 *                 Memory Data will be returned to the Data Register
 *                 (In  case the Memory width andgt; 32b, otherwise
 *                 this field is neglected).
 *   [17: 0][RW] - This field is written along with START and
 *                 DWORD_SELECT fields to indicate which Memory Row
 *                 to read.
 *
 */
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_REG         ((TSCD_BASE_ADDR) + 0x8D4)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_DONE_OFF                  (31)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_DONE_LEN                  (1)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_DONE_MSK                  (0x80000000)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_DONE_RST                  (0x1)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_RD_EN_OFF                 (30)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_RD_EN_LEN                 (1)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_RD_EN_MSK                 (0x40000000)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_RD_EN_RST                 (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_RESERVED_3_OFF            (26)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_RESERVED_3_LEN            (4)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_RESERVED_3_MSK            (0x3C000000)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_RESERVED_3_RST            (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_DW_SEL_OFF                (18)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_DW_SEL_LEN                (8)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_DW_SEL_MSK                (0x03FC0000)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_DW_SEL_RST                (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_ADR_OFF                   (0)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_ADR_LEN                   (18)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_ADR_MSK                   (0x0003FFFF)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_CTL_ADR_RST                   (0x0)

/**
 * SW_REG_NAME : PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_REG
 * HW_REG_NAME : tscd_bw_limit_table_1_dbg_ctl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][RO] - This bit is cleared after the START bit is set. It
 *                 is set back again when the Read transaction is
 *                 complete  and Valid Data is in the DATA Register.
 *   [30:30][RW] - Writing a 1b to this bit causes the Read
 *                 Transaction. It is self-cleared by the hardware
 *                 when transaction  is finished.andedsp;
 *   [29:26][RO] - MISSING_DESCRIPTION
 *   [25:18][RW] - This field determines which DWORD part of the
 *                 Memory Data will be returned to the Data Register
 *                 (In  case the Memory width andgt; 32b, otherwise
 *                 this field is neglected).
 *   [17: 0][RW] - This field is written along with START and
 *                 DWORD_SELECT fields to indicate which Memory Row
 *                 to read.
 *
 */
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_REG         ((TSCD_BASE_ADDR) + 0x8D8)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_DONE_OFF                  (31)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_DONE_LEN                  (1)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_DONE_MSK                  (0x80000000)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_DONE_RST                  (0x1)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_RD_EN_OFF                 (30)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_RD_EN_LEN                 (1)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_RD_EN_MSK                 (0x40000000)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_RD_EN_RST                 (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_RESERVED_3_OFF            (26)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_RESERVED_3_LEN            (4)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_RESERVED_3_MSK            (0x3C000000)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_RESERVED_3_RST            (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_DW_SEL_OFF                (18)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_DW_SEL_LEN                (8)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_DW_SEL_MSK                (0x03FC0000)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_DW_SEL_RST                (0x0)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_ADR_OFF                   (0)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_ADR_LEN                   (18)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_ADR_MSK                   (0x0003FFFF)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_CTL_ADR_RST                   (0x0)

/**
 * SW_REG_NAME : PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_REG
 * HW_REG_NAME : tscd_shared_bw_limit_table_dbg_ctl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][RO] - This bit is cleared after the START bit is set. It
 *                 is set back again when the Read transaction is
 *                 complete  and Valid Data is in the DATA Register.
 *   [30:30][RW] - Writing a 1b to this bit causes the Read
 *                 Transaction. It is self-cleared by the hardware
 *                 when transaction  is finished.andedsp;
 *   [29:26][RO] - MISSING_DESCRIPTION
 *   [25:18][RW] - This field determines which DWORD part of the
 *                 Memory Data will be returned to the Data Register
 *                 (In  case the Memory width andgt; 32b, otherwise
 *                 this field is neglected).
 *   [17: 0][RW] - This field is written along with START and
 *                 DWORD_SELECT fields to indicate which Memory Row
 *                 to read.
 *
 */
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_REG    ((TSCD_BASE_ADDR) + 0x8DC)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_DONE_OFF             (31)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_DONE_LEN             (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_DONE_MSK             (0x80000000)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_DONE_RST             (0x1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_RD_EN_OFF            (30)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_RD_EN_LEN            (1)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_RD_EN_MSK            (0x40000000)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_RD_EN_RST            (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_RESERVED_3_OFF       (26)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_RESERVED_3_LEN       (4)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_RESERVED_3_MSK       (0x3C000000)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_RESERVED_3_RST       (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_DW_SEL_OFF           (18)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_DW_SEL_LEN           (8)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_DW_SEL_MSK           (0x03FC0000)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_DW_SEL_RST           (0x0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_ADR_OFF              (0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_ADR_LEN              (18)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_ADR_MSK              (0x0003FFFF)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_CTL_ADR_RST              (0x0)

/**
 * SW_REG_NAME : PP_TSCD__RL_MAP_TABLE_DBG_CTL_REG
 * HW_REG_NAME : tscd_rl_map_table_dbg_ctl
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31:31][RO] - This bit is cleared after the START bit is set. It
 *                 is set back again when the Read transaction is
 *                 complete  and Valid Data is in the DATA Register.
 *   [30:30][RW] - Writing a 1b to this bit causes the Read
 *                 Transaction. It is self-cleared by the hardware
 *                 when transaction  is finished.andedsp;
 *   [29:26][RO] - MISSING_DESCRIPTION
 *   [25:18][RW] - This field determines which DWORD part of the
 *                 Memory Data will be returned to the Data Register
 *                 (In  case the Memory width andgt; 32b, otherwise
 *                 this field is neglected).
 *   [17: 0][RW] - This field is written along with START and
 *                 DWORD_SELECT fields to indicate which Memory Row
 *                 to read.
 *
 */
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_REG             ((TSCD_BASE_ADDR) + 0x8E0)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_DONE_OFF                      (31)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_DONE_LEN                      (1)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_DONE_MSK                      (0x80000000)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_DONE_RST                      (0x1)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_RD_EN_OFF                     (30)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_RD_EN_LEN                     (1)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_RD_EN_MSK                     (0x40000000)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_RD_EN_RST                     (0x0)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_RESERVED_3_OFF                (26)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_RESERVED_3_LEN                (4)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_RESERVED_3_MSK                (0x3C000000)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_RESERVED_3_RST                (0x0)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_DW_SEL_OFF                    (18)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_DW_SEL_LEN                    (8)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_DW_SEL_MSK                    (0x03FC0000)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_DW_SEL_RST                    (0x0)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_ADR_OFF                       (0)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_ADR_LEN                       (18)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_ADR_MSK                       (0x0003FFFF)
#define PP_TSCD__RL_MAP_TABLE_DBG_CTL_ADR_RST                       (0x0)

/**
 * SW_REG_NAME : PP_TSCD__NODE_TABLE_0_DBG_DATA_REG
 * HW_REG_NAME : tscd_node_table_0_dbg_data
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - DWORD part of the Memory Row returned by the
 *                 Hardware.andedsp;    Which DWORD part to return is
 *                 determined by the DWORD_SELECT filed in the CTL
 *                 CSR.
 *
 */
#define PP_TSCD__NODE_TABLE_0_DBG_DATA_REG            ((TSCD_BASE_ADDR) + 0x8E8)
#define PP_TSCD__NODE_TABLE_0_DBG_DATA_RD_DW_OFF                    (0)
#define PP_TSCD__NODE_TABLE_0_DBG_DATA_RD_DW_LEN                    (32)
#define PP_TSCD__NODE_TABLE_0_DBG_DATA_RD_DW_MSK                    (0xFFFFFFFF)
#define PP_TSCD__NODE_TABLE_0_DBG_DATA_RD_DW_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD__NODE_TABLE_1_DBG_DATA_REG
 * HW_REG_NAME : tscd_node_table_1_dbg_data
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - DWORD part of the Memory Row returned by the
 *                 Hardware.andedsp;    Which DWORD part to return is
 *                 determined by the DWORD_SELECT filed in the CTL
 *                 CSR.
 *
 */
#define PP_TSCD__NODE_TABLE_1_DBG_DATA_REG            ((TSCD_BASE_ADDR) + 0x8EC)
#define PP_TSCD__NODE_TABLE_1_DBG_DATA_RD_DW_OFF                    (0)
#define PP_TSCD__NODE_TABLE_1_DBG_DATA_RD_DW_LEN                    (32)
#define PP_TSCD__NODE_TABLE_1_DBG_DATA_RD_DW_MSK                    (0xFFFFFFFF)
#define PP_TSCD__NODE_TABLE_1_DBG_DATA_RD_DW_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD__NODE_TABLE_2_DBG_DATA_REG
 * HW_REG_NAME : tscd_node_table_2_dbg_data
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - DWORD part of the Memory Row returned by the
 *                 Hardware.andedsp;    Which DWORD part to return is
 *                 determined by the DWORD_SELECT filed in the CTL
 *                 CSR.
 *
 */
#define PP_TSCD__NODE_TABLE_2_DBG_DATA_REG            ((TSCD_BASE_ADDR) + 0x8F0)
#define PP_TSCD__NODE_TABLE_2_DBG_DATA_RD_DW_OFF                    (0)
#define PP_TSCD__NODE_TABLE_2_DBG_DATA_RD_DW_LEN                    (32)
#define PP_TSCD__NODE_TABLE_2_DBG_DATA_RD_DW_MSK                    (0xFFFFFFFF)
#define PP_TSCD__NODE_TABLE_2_DBG_DATA_RD_DW_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD__NODE_TABLE_3_DBG_DATA_REG
 * HW_REG_NAME : tscd_node_table_3_dbg_data
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - DWORD part of the Memory Row returned by the
 *                 Hardware.andedsp;    Which DWORD part to return is
 *                 determined by the DWORD_SELECT filed in the CTL
 *                 CSR.
 *
 */
#define PP_TSCD__NODE_TABLE_3_DBG_DATA_REG            ((TSCD_BASE_ADDR) + 0x8F4)
#define PP_TSCD__NODE_TABLE_3_DBG_DATA_RD_DW_OFF                    (0)
#define PP_TSCD__NODE_TABLE_3_DBG_DATA_RD_DW_LEN                    (32)
#define PP_TSCD__NODE_TABLE_3_DBG_DATA_RD_DW_MSK                    (0xFFFFFFFF)
#define PP_TSCD__NODE_TABLE_3_DBG_DATA_RD_DW_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD__BRANCH_TABLE_DBG_DATA_REG
 * HW_REG_NAME : tscd_branch_table_dbg_data
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - DWORD part of the Memory Row returned by the
 *                 Hardware.andedsp;    Which DWORD part to return is
 *                 determined by the DWORD_SELECT filed in the CTL
 *                 CSR.
 *
 */
#define PP_TSCD__BRANCH_TABLE_DBG_DATA_REG            ((TSCD_BASE_ADDR) + 0x8F8)
#define PP_TSCD__BRANCH_TABLE_DBG_DATA_RD_DW_OFF                    (0)
#define PP_TSCD__BRANCH_TABLE_DBG_DATA_RD_DW_LEN                    (32)
#define PP_TSCD__BRANCH_TABLE_DBG_DATA_RD_DW_MSK                    (0xFFFFFFFF)
#define PP_TSCD__BRANCH_TABLE_DBG_DATA_RD_DW_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD__BW_LIMIT_TABLE_0_DBG_DATA_REG
 * HW_REG_NAME : tscd_bw_limit_table_0_dbg_data
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - DWORD part of the Memory Row returned by the
 *                 Hardware.andedsp;    Which DWORD part to return is
 *                 determined by the DWORD_SELECT filed in the CTL
 *                 CSR.
 *
 */
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_DATA_REG        ((TSCD_BASE_ADDR) + 0x8FC)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_DATA_RD_DW_OFF                (0)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_DATA_RD_DW_LEN                (32)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_DATA_RD_DW_MSK                (0xFFFFFFFF)
#define PP_TSCD__BW_LIMIT_TABLE_0_DBG_DATA_RD_DW_RST                (0x0)

/**
 * SW_REG_NAME : PP_TSCD__BW_LIMIT_TABLE_1_DBG_DATA_REG
 * HW_REG_NAME : tscd_bw_limit_table_1_dbg_data
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - DWORD part of the Memory Row returned by the
 *                 Hardware.andedsp;    Which DWORD part to return is
 *                 determined by the DWORD_SELECT filed in the CTL
 *                 CSR.
 *
 */
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_DATA_REG        ((TSCD_BASE_ADDR) + 0x900)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_DATA_RD_DW_OFF                (0)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_DATA_RD_DW_LEN                (32)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_DATA_RD_DW_MSK                (0xFFFFFFFF)
#define PP_TSCD__BW_LIMIT_TABLE_1_DBG_DATA_RD_DW_RST                (0x0)

/**
 * SW_REG_NAME : PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_DATA_REG
 * HW_REG_NAME : tscd_shared_bw_limit_table_dbg_data
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - DWORD part of the Memory Row returned by the
 *                 Hardware.andedsp;    Which DWORD part to return is
 *                 determined by the DWORD_SELECT filed in the CTL
 *                 CSR.
 *
 */
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_DATA_REG   ((TSCD_BASE_ADDR) + 0x904)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_DATA_RD_DW_OFF           (0)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_DATA_RD_DW_LEN           (32)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_DATA_RD_DW_MSK           (0xFFFFFFFF)
#define PP_TSCD__SHARED_BW_LIMIT_TABLE_DBG_DATA_RD_DW_RST           (0x0)

/**
 * SW_REG_NAME : PP_TSCD__RL_MAP_TABLE_DBG_DATA_REG
 * HW_REG_NAME : tscd_rl_map_table_dbg_data
 * DESCRIPTION : MISSING Description
 *
 *  Register Fields :
 *   [31: 0][RO] - DWORD part of the Memory Row returned by the
 *                 Hardware.andedsp;    Which DWORD part to return is
 *                 determined by the DWORD_SELECT filed in the CTL
 *                 CSR.
 *
 */
#define PP_TSCD__RL_MAP_TABLE_DBG_DATA_REG            ((TSCD_BASE_ADDR) + 0x908)
#define PP_TSCD__RL_MAP_TABLE_DBG_DATA_RD_DW_OFF                    (0)
#define PP_TSCD__RL_MAP_TABLE_DBG_DATA_RD_DW_LEN                    (32)
#define PP_TSCD__RL_MAP_TABLE_DBG_DATA_RD_DW_MSK                    (0xFFFFFFFF)
#define PP_TSCD__RL_MAP_TABLE_DBG_DATA_RD_DW_RST                    (0x0)

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_ALIASSED_Q_REG
 * HW_REG_NAME : glscd_aliassed_queue
 * DESCRIPTION : ALIASSED QUEUE - This register is a configuration
 *               bitmap of Aliased queues Q[31:0](+32*N)
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_ALIASSED_Q_REG                  ((TSCD_BASE_ADDR) + 0x910)
#define PP_TSCD_GLSCD_ALIASSED_Q_OFF                                (0)
#define PP_TSCD_GLSCD_ALIASSED_Q_LEN                                (32)
#define PP_TSCD_GLSCD_ALIASSED_Q_MSK                                (0xFFFFFFFF)
#define PP_TSCD_GLSCD_ALIASSED_Q_RST                                (0x0)
/**
 * REG_IDX_ACCESS   : PP_TSCD_GLSCD_ALIASSED_Q_REG_IDX
 * NUM OF REGISTERS : 16
 */
#define PP_TSCD_GLSCD_ALIASSED_Q_REG_IDX(idx) \
	(PP_TSCD_GLSCD_ALIASSED_Q_REG + ((idx) << 2))

/**
 * SW_REG_NAME : PP_TSCD_GLSCD_HIDDEN_Q_REG
 * HW_REG_NAME : glscd_hidden_queue
 * DESCRIPTION : HIDDEN QUEUE - This register is a configuration
 *               bitmap of hidden or disabled to TSCD queues
 *               Q[31:0](+32*N)
 *
 *  Register Fields :
 *   [31: 0][RW] - MISSING_DESCRIPTION
 *
 */
#define PP_TSCD_GLSCD_HIDDEN_Q_REG                    ((TSCD_BASE_ADDR) + 0x950)
#define PP_TSCD_GLSCD_HIDDEN_Q_OFF                                  (0)
#define PP_TSCD_GLSCD_HIDDEN_Q_LEN                                  (32)
#define PP_TSCD_GLSCD_HIDDEN_Q_MSK                                  (0xFFFFFFFF)
#define PP_TSCD_GLSCD_HIDDEN_Q_RST                                  (0x0)
/**
 * REG_IDX_ACCESS   : PP_TSCD_GLSCD_HIDDEN_Q_REG_IDX
 * NUM OF REGISTERS : 32
 */
#define PP_TSCD_GLSCD_HIDDEN_Q_REG_IDX(idx) \
	(PP_TSCD_GLSCD_HIDDEN_Q_REG + ((idx) << 2))

#endif
