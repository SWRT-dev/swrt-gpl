/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/*
 * $Id$
 *
 *
 *
 * Interface for Recovery messages sent via Netlink socket
 *
 *
 */
#ifndef __WAVE_RCVRY_NL_IFACE_H__
#define __WAVE_RCVRY_NL_IFACE_H__

//#define WAVE_RCVRY_NL_EXTERNAL_HEADER /* please, comment this line if using in driver repository */

#ifdef WAVE_RCVRY_NL_EXTERNAL_HEADER
#include <linux/version.h>
#else
#include "nl.h"
#endif

/* Helper macro */
#ifndef __PACKED__
#define __PACKED__ __attribute__ ((aligned(1), packed))
#endif

#ifdef WAVE_RCVRY_NL_EXTERNAL_HEADER

/* MTLK Family name and version */
#define MTLK_GENL_FAMILY_NAME     "MTLK_WLS"
#define MTLK_GENL_FAMILY_VERSION  1

/* Attributes of the family */
enum {
  MTLK_GENL_ATTR_UNSPEC,
  MTLK_GENL_ATTR_EVENT,
  __MTLK_GENL_ATTR_MAX,
};
#define MTLK_GENL_ATTR_MAX (__MTLK_GENL_ATTR_MAX -1)

/* Supported commands */
enum {
  MTLK_GENL_CMD_UNSPEC,
  MTLK_GENL_CMD_EVENT,
  __MTLK_GENL_CMD_MAX,
};
#define MTLK_GENL_CMD_MAX (__MTLK_GENL_CMD_MAX -1)

/* MTLK Protocol version */
#define MTLK_NL_PROTOCOL_VERSION  1

/* Command ID for Recovery messages */
#define NL_DRV_CMD_FAPI_NOTIFY    7

/* MTLK header */
struct mtlk_nl_msghdr {
  char   fingerprint[4]; /* "mtlk" */
  uint8  proto_ver;      /* MTLK_NL_PROTOCOL_VERSION */
  uint8  cmd_id;         /* NL_DRV_CMD_FAPI_NOTIFY */
  uint16 data_len;       /* payload size, in bytes */
} __PACKED__;

/* Netlink group (used for joining the desired group to send/receive messages to/from) */
#define WAVE_NETLINK_FAPI_GROUP_NAME    "wave_fapi"
#define NETLINK_FAPI_GROUP   (3L << 0)

/* Sub-command IDs for FAPI messages */
enum {
  WAVE_FAPI_SUBCMDID_RESERVED,                  /* Reserved (not used) */
  WAVE_FAPI_SUBCMDID_PROD_CAL_FILE_EVT,         /* Production Calibration File Event */
  WAVE_FAPI_SUBCMDID_LAST                       /* Last (not used) */
};

#endif /* WAVE_RCVRY_NL_EXTERNAL_HEADER */


/* Message header */
typedef struct {
  uint8 subcmd_id; /* sub-command ID (WAVE_FAPI_SUBCMDID_*) */
  uint8 card_idx;  /* PCI-card index */
} __PACKED__ wave_fapi_msghdr_t;

/* Message payload for Dump Evacuation request/response */
typedef struct {
  wave_fapi_msghdr_t hdr; /* header */
} __PACKED__ wave_fapi_msgpay_rcvry_dump_evac_t;

/* Message payload for Complete Recovery Flow request */
typedef struct {
  wave_fapi_msghdr_t hdr; /* header */
} __PACKED__ wave_fapi_msgpay_rcvry_complete_flow_t;

/* Message payload for Check Socket request (used by driver only to check connection with FAPI) */
typedef struct {
  wave_fapi_msghdr_t hdr; /* header */
} __PACKED__ wave_fapi_msgpay_rcvry_check_sock_t;

/* Error types to use for payload of MAC Error event */
enum {
  WAVE_RCVRY_ERRORTYPE_RESERVED,              /* Reserved (not used) */
  WAVE_RCVRY_ERRORTYPE_MAC_EXTERNAL_ASSERT,   /* MAC External Assert */
  WAVE_RCVRY_ERRORTYPE_MAC_EXCEPTION,         /* MAC Exception Error */
  WAVE_RCVRY_ERRORTYPE_MAC_FATAL,             /* MAC Fatal Error */
  WAVE_RCVRY_ERRORTYPE_LAST                   /* Last (not used) */
};

/* Message payload for MAC Error event (sent by driver before Dump Evacuation request in case Recovery is off) */
typedef struct {
  wave_fapi_msghdr_t hdr;        /* header */
  uint8              error_type; /* payload - error type */
} __PACKED__ wave_fapi_msgpay_rcvry_mac_error_t;

/* Message payload for Calibration File event */
typedef struct {
  wave_fapi_msghdr_t hdr;           /* header */
} __PACKED__ wave_fapi_msgpay_prod_cal_file_t;

/* Helper macros */
#define WAVE_FAPI_MSG_HDR_PUT(_phdr, _cmd, _card)   \
    ((wave_fapi_msghdr_t*)_phdr)->subcmd_id = _cmd; \
    ((wave_fapi_msghdr_t*)_phdr)->card_idx = _card
#define WAVE_FAPI_MSG_HDR_GET(_cmd, _card, _phdr)   \
    _cmd = ((wave_fapi_msghdr_t*)_phdr)->subcmd_id; \
    _card = ((wave_fapi_msghdr_t*)_phdr)->card_idx

#endif /* __WAVE_RCVRY_NL_IFACE_H__ */
