/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * 
 *
 * LogGrabber protocol
 *
 * Written by: Andrey Fidrya
 *
 */

#include "mtlkinc.h"
#include <stddef.h>

#include "compat.h"
#include "logsrv_utils.h"
#include "cqueue.h"

#define LOG_LOCAL_GID   GID_PROTO_LG
#define LOG_LOCAL_FID   1

#define LG_PROTO_VER 1

#define MSGID_REQ_CONNECT          0x01
#define MSGID_REQ_CONFIG           0x02
#define MSGID_REQ_START_LOGGING    0x03
#define MSGID_REQ_STOP_LOGGING     0x04

#define MSGID_RESP_CONNECT         0x81
#define MSGID_RESP_CONFIG          0x82
#define MSGID_RESP_START_LOGGING   0x83
#define MSGID_RESP_STOP_LOGGING    0x84

#define MSGID_EVENTS_DATA_PKT      0xC0

typedef struct _lg_datapkt_hdr {
  uint8 id1; // 'M'
  uint8 id2; // 'T'
  uint8 version;
  uint8 msgid;
  uint32 datalen;
} __PACKED lg_datapkt_hdr;

// -1 - error
//  0 - success
static int
process_pkt(cqueue_t *pqueue)
{
  uint8 msgid;

  ASSERT(SIZEOF_MEMB(lg_datapkt_hdr, msgid) == sizeof(msgid));
  cqueue_get(pqueue, offsetof(lg_datapkt_hdr, msgid), sizeof(msgid),
    (unsigned char *) &msgid);

  switch (msgid)
  {
  case MSGID_REQ_CONNECT:
    ILOG9_V("Received MSGID_REQ_CONNECT");
    break;

  case MSGID_REQ_CONFIG:
    ILOG9_V("Received MSGID_REQ_CONFIG");
    break;

  case MSGID_REQ_START_LOGGING:
    ILOG9_V("Received MSGID_REQ_START_LOGGIN");
    break;

  case MSGID_REQ_STOP_LOGGING:
    ILOG9_V("Received MSGID_REQ_STOP_LOGGIN");
    break;

  case MSGID_RESP_CONNECT:
    ILOG9_V("Received MSGID_RESP_CONNECT");
    break;

  case MSGID_RESP_CONFIG:
    ILOG9_V("Received MSGID_RESP_CONFIG");
    break;

  case MSGID_RESP_START_LOGGING:
    ILOG9_V("Received MSGID_RESP_START_LOGGING");
    break;

  case MSGID_RESP_STOP_LOGGING:
    ILOG9_V("Received MSGID_RESP_STOP_LOGGING");
    break;

  case MSGID_EVENTS_DATA_PKT:
    ELOG_V("Unexpected events data packet received");
    return -1;
  
  default:
    ELOG_D("Unknown message (id %d)", msgid);
    return -1;
  }

  return 0;
}

// -1 - error
// 0  - no complete packets in queue
// 1  - packet processed succesfully
int
lg_process_next_pkt(cqueue_t *pqueue)
{
  int sz = cqueue_size(pqueue);
  uint32 datalen;
  uint32 pktlen;

  if (sz < sizeof(lg_datapkt_hdr))
    return 0;

  ASSERT(SIZEOF_MEMB(lg_datapkt_hdr, datalen) == sizeof(datalen));
  ILOG9_V("Calling cqueue_get");
  datalen = 0;
  cqueue_get(pqueue, offsetof(lg_datapkt_hdr, datalen), sizeof(datalen),
    (unsigned char *) &datalen);
  ILOG9_D("cqueue_get returned datalen=%lu", datalen);

  pktlen = datalen + sizeof(lg_datapkt_hdr);

  ILOG9_DDS("sz=%d < pktlen=%lu ? %s", sz, pktlen, sz < pktlen ? "true" : "false");
  if (sz < pktlen)
    return 0;

  // At this point there's a complete packet in queue at offset 0.
  // Process this packet.

  ILOG9_V("Calling process_pkt");
  if (0 != process_pkt(pqueue)) {
    return -1;
  }

  ILOG9_V("Calling cqueue_pop_front");
  cqueue_pop_front(pqueue, pktlen);

  return 1;
}


