/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __LOG_DEFS_H__
#define __LOG_DEFS_H__

#include "mtlkbfield.h"

#define MTLK_PACK_ON
#include "mtlkpack.h"

#define RTLOGGER_VER_MAJOR 0
#define RTLOGGER_VER_MINOR 1

/****************************************************************************************
 * 'info' member is bitmask as following:
 *
 * BITS | 31 | 30 | 29 | 28 | 27 | 26 | 25 | 24 | 23 | 22 | 21 | 20 | 19 | 18 | 17 | 16 |
 *      |      OID     |       SID HIGH         | E2 |    GID                           |
 *
 * BITS | 15 | 14 | 13 | 12 | 11 | 10 | 09 | 08 | 07 | 06 | 05 | 04 | 03 | 02 | 01 | 00 |
 *      | E1 |      SID LOW      |  Data Length                                         |
 *
 * E1, E2 - endianess bits (E1 = 0, E2 = 1).
 *          These bits are used for Endianess detection during record parsing:
 *          if the E1 == 0 and E2 == 1 then the record source runs with the same
 *          endianess as parser, otherwise (E1 == 1 and E2 == 0) - the endianess
 *          WARNING: these bits CAN NOT be moved to other positions!
 *          of the parser is different from the record source's one.
 * OID    - record's Origin ID (for example, Driver, LMAC, UMAC etc.)
 * GID    - record's Group ID (for example, per component inside a origin)
 * SID    - record's String ID (particular log event ID). It is divided to 2 parts
 *          (high bits and low bits) but both these parts are representing single SID
 *          value.
 ****************************************************************************************/

/* Log info word 0 (32 bits) */
#define LOG_INFO_W0_EXP     MTLK_BFIELD_INFO(31,  1) /* 31        */
#define LOG_INFO_W0_LID     MTLK_BFIELD_INFO(17, 14) /* 17 ... 30 */
#define LOG_INFO_W0_OID     MTLK_BFIELD_INFO(12,  5) /* 12 ... 16 */
#define LOG_INFO_W0_GID     MTLK_BFIELD_INFO( 5,  7) /*  5 ... 11 */
#define LOG_INFO_W0_FID     MTLK_BFIELD_INFO( 0,  5) /*  0 ...  4 */

/*
  Logprep.pl performs maximum values checks
  for following fields. When length of fields
  changed corresponding constants in logprep.pl
  must be edited as well.
*/
#define MAX_OID (1 << 5)    /* FIXME */
#define MAX_GID (1 << 7)    /* FIXME */

/* Log info word 1 (16 bits) */
#define LOG_INFO_W1_DROP     MTLK_BFIELD_INFO(15,  1) /* 15        */
                                                      /* 13 ... 14 reserved */
#define LOG_INFO_W1_DEST     MTLK_BFIELD_INFO(11,  2) /* 11 ... 12 */
#define LOG_INFO_W1_WLAN_IF  MTLK_BFIELD_INFO( 9,  2) /*  9 ... 10 */
#define LOG_INFO_W1_BE1      MTLK_BFIELD_INFO( 8,  1) /*  8        */
#define LOG_INFO_W1_PRIOR    MTLK_BFIELD_INFO( 5,  3) /*  5 ...  7 */
#define LOG_INFO_W1_VERSION  MTLK_BFIELD_INFO( 1,  4) /*  1 ...  4 */
#define LOG_INFO_W1_BE0      MTLK_BFIELD_INFO( 0,  1) /*  0        */

/* Compose uint32 info_w0 */
#define LOG_MAKE_INFO_W0(exp, lid, oid, gid, fid) \
  (MTLK_BFIELD_VALUE(LOG_INFO_W0_EXP,       (exp), uint32) | \
   MTLK_BFIELD_VALUE(LOG_INFO_W0_LID,       (lid), uint32) | \
   MTLK_BFIELD_VALUE(LOG_INFO_W0_OID,       (oid), uint32) | \
   MTLK_BFIELD_VALUE(LOG_INFO_W0_GID,       (gid), uint32) | \
   MTLK_BFIELD_VALUE(LOG_INFO_W0_FID,       (fid), uint32))

/* Compose uint16 info_w1. Note: BE0 = 0, BE1 = 1. */
/* FIXME: can be missed the "prior" field (assumed 0) */
#define LOG_MAKE_INFO_W1(dest, wif, prior, ver) \
  (MTLK_BFIELD_VALUE(LOG_INFO_W1_DEST   ,  (dest), uint16) | \
   MTLK_BFIELD_VALUE(LOG_INFO_W1_WLAN_IF,   (wif), uint16) | \
   MTLK_BFIELD_VALUE(LOG_INFO_W1_PRIOR  , (prior), uint16) | \
   MTLK_BFIELD_VALUE(LOG_INFO_W1_VERSION,   (ver), uint16) | \
   MTLK_BFIELD_VALUE(LOG_INFO_W1_BE1    ,     (1), uint16) | \
   MTLK_BFIELD_VALUE(LOG_INFO_W1_BE0    ,     (0), uint16))

/* Make whole mtlk_log_event_t data structure */
#define LOG_MAKE_INFO(log_evt, ver, datalen, wif, dest, prior, exp, oid, gid, fid, lid) \
    (log_evt).info_w0 = LOG_MAKE_INFO_W0(exp, lid, oid, gid, fid); \
    (log_evt).info_w1 = LOG_MAKE_INFO_W1(dest, wif, prior, ver); \
    (log_evt).dsize   = datalen + sizeof(mtlk_log_event_t); /* including the header */

/* Chehking of endianness */
#define LOG_IS_STRAIGHT_ENDIAN(log_evt)     (MTLK_BFIELD_GET((log_evt).info_w1, LOG_INFO_W1_BE1) == 1)
#define LOG_IS_INVERSED_ENDIAN(log_evt)     (MTLK_BFIELD_GET((log_evt).info_w1, LOG_INFO_W1_BE0) == 1)

#define LOG_IS_CORRECT_INFO(log_evt) \
  (MTLK_BFIELD_GET((log_evt).info_w1, LOG_INFO_W1_BE0) != \
   MTLK_BFIELD_GET((log_evt).info_w1, LOG_INFO_W1_BE1))

/* Extract some fields */
#define LOG_INFO_GET_OID(log_evt)       MTLK_BFIELD_GET((log_evt).info_w0, LOG_INFO_W0_OID)
#define LOG_INFO_GET_GID(log_evt)       MTLK_BFIELD_GET((log_evt).info_w0, LOG_INFO_W0_GID)
#define LOG_INFO_GET_FID(log_evt)       MTLK_BFIELD_GET((log_evt).info_w0, LOG_INFO_W0_FID)
#define LOG_INFO_GET_LID(log_evt)       MTLK_BFIELD_GET((log_evt).info_w0, LOG_INFO_W0_LID)

#define LOG_INFO_GET_VERSION(log_evt)   MTLK_BFIELD_GET((log_evt).info_w1, LOG_INFO_W1_VERSION)
#define LOG_INFO_GET_WLAN_IF(log_evt)   MTLK_BFIELD_GET((log_evt).info_w1, LOG_INFO_W1_WLAN_IF)

#define LOG_INFO_GET_DSIZE(log_evt)     ((log_evt).dsize)

/* WARNING: This structure is used by macros generated by logprep.pl   */
/* The script generates code that relies on order and number of fields */
/* in the structure. Whether this structure changes logprep.pl must be */
/* changed as well.                                                    */
typedef struct _mtlk_log_event_t
{                   /* offset in bytes */
  uint32 info_w0;   /* 0 */
  uint16 dsize;     /* 4 */
  uint16 info_w1;   /* 6 */
  uint32 timestamp; /* 8 */
} __MTLK_PACKED mtlk_log_event_t;

typedef struct _mtlk_tsf_event_t
{                       /* offset in bytes */
  uint32 msg_type;      /* 0 */
  uint32 hw_idx;        /* 4 */
  uint32 drv_timestamp; /* 8 */
  int32  fw_offset;     /* 12 */
} __MTLK_PACKED mtlk_tsf_event_t;

typedef struct _mtlk_log_event_data_t
{
  uint32 datatype;
} __MTLK_PACKED mtlk_log_event_data_t;

typedef struct _mtlk_log_lstring_t
{
  uint32 len;
} __MTLK_PACKED mtlk_log_lstring_t;

typedef struct _mtlk_log_signal_t
{
  uint16 src_task_id;
  uint16 dst_task_id;
  uint32 len;
} __MTLK_PACKED mtlk_log_signal_t;

#define LOG_DT_LSTRING  0
#define LOG_DT_INT8     1
#define LOG_DT_INT32    2
#define LOG_DT_INT64    3
#define LOG_DT_MACADDR  4
#define LOG_DT_IP6ADDR  5
#define LOG_DT_SIGNAL   6
#define LOG_DT_TLOG     7

/* printf symbol for IP4 ADDR extension */
#define MTLK_LOG_FMT_IP4    'B'
/* printf symbol for IP6 ADDR extension */
#define MTLK_LOG_FMT_IP6    'K'
/* printf symbol for MAC ADDR extension */
#define MTLK_LOG_FMT_MAC    'Y'

typedef enum _mtlk_log_ctrl_id_e
{
  MTLK_LOG_CTRL_ID_VERINFO, 
  MTLK_LOG_CTRL_ID_LAST
} mtlk_log_ctrl_id_e;

typedef struct _mtlk_log_ctrl_hdr_t
{
  uint16 id;
  uint16 data_size;
} __MTLK_PACKED mtlk_log_ctrl_hdr_t;

typedef struct _mtlk_log_ctrl_ver_info_data_t
{
  uint16              major;
  uint16              minor;
} __MTLK_PACKED mtlk_log_ctrl_ver_info_data_t;

typedef struct _mtlk_log_TLOG_t
{
    uint16 src_task_id;
    uint16 dst_task_id;
    uint32 len;
} __MTLK_PACKED mtlk_log_TLOG_t;

#define MTLK_PACK_OFF
#include "mtlkpack.h"

#endif
