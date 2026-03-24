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
 * Driver protocol
 *
 * Written by: Andrey Fidrya
 *
 */

#include "mtlkinc.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stddef.h>
#include <syslog.h>

#include <sys/ioctl.h>

#include "compat.h"
#include "logserver.h"
#include "logsrv_utils.h"
#include "proto_drv.h"
#include "db.h"
#include "mtlkerr.h"

#define LOG_LOCAL_GID   GID_PROTO_DRV
#define LOG_LOCAL_FID   1

char scd_text_not_found[] = "    SCD Text not found!";
char scd_text_found[] = "    SCD Text = ";

struct log_ver_info_req
{
  mtlk_log_ctrl_hdr_t           hdr;
  mtlk_log_ctrl_ver_info_data_t data;
};

BOOL
cdev_get_logger_ver (cdev_t *dev)
{
  struct log_ver_info_req ver_info_req;
  BOOL res = FALSE;
  int  tmp;

  ver_info_req.hdr.id        = (uint16)MTLK_LOG_CTRL_ID_VERINFO;
  ver_info_req.hdr.data_size = (uint16)sizeof(ver_info_req.data);

  tmp = ioctl(dev->fd, SIOCDEVPRIVATE, &ver_info_req);
  if (tmp != 0) {
    ELOG_D("MTLK_LOG_CTRL_ID_VERINFO failed (%d)", tmp);
    goto end;
  }

  dev->log_ver_major = ver_info_req.data.major;
  dev->log_ver_minor = ver_info_req.data.minor;

  res = TRUE;

end:
  return res;
}

cdev_t *
cdev_open(const char *dev_name)
{
  cdev_t *dev = NULL;
  int fd = -1;
  BOOL res = FALSE;
  BOOL dev_name_initialized = FALSE;

  ILOG9_V("cdev_open");

  fd = open(dev_name, O_RDWR | O_NONBLOCK);
  if (-1 == fd) {
    ELOG_SS("%s: %s", dev_name, strerror(errno));
    goto end;
  }

  dev = (cdev_t *) malloc(sizeof(cdev_t));
  if (!dev) {
    ELOG_V("Out of memory while allocating a file handle");
    goto end;
  }

  dev->name = strdup(dev_name);
  if (!dev->name) {
    ELOG_V("Out of memory");
    goto end;
  }
  dev_name_initialized = TRUE;
  dev->fd = fd;

  if (!cdev_get_logger_ver(dev)) {
    ELOG_V("Can't get version");
    goto end;
  }

  res = TRUE;

end:
  if (!res) {
    if (dev) {
      if (dev_name_initialized)
        free(dev->name);
      free(dev);
      dev = NULL;
    }
      
    if (fd != -1) {
      close(fd);
    }
  }
  return dev;
}

int
cdev_close(cdev_t *dev)
{
  int retval;

  ILOG9_V("cdev_close");

  retval = close(dev->fd);
  if (-1 == retval) {
    ELOG_SS("%s: %s", dev->name, strerror(errno));
  }

  free(dev->name);

  free(dev);

  return retval;
}

int
cdev_read_to_q(cqueue_t *pqueue, cdev_t *dev)
{
  ASSERT(cqueue_space_left(pqueue) > 0);
  return cqueue_read(pqueue, dev->fd) > 0;
}

// -1 - error
//  0 - success
static int
drv_process_pkt(cqueue_t *pqueue)
{
  int printed;
  mtlk_log_event_t log_evt;
  mtlk_log_event_data_t log_evt_data;
  int pktlen;
  int at = 0;
  char buf[256];
  char *scd_text = NULL;
  int oid, gid, fid, lid, dsize;

  cqueue_get(pqueue, at, sizeof(log_evt), (unsigned char *) &log_evt);

  dsize = LOG_INFO_GET_DSIZE(log_evt);
  oid   = LOG_INFO_GET_OID(log_evt);
  gid   = LOG_INFO_GET_GID(log_evt);
  fid   = LOG_INFO_GET_FID(log_evt);
  lid   = LOG_INFO_GET_LID(log_evt);

  if (scd_data && (log_to_console || log_to_syslog || text_protocol))
    scd_text = scd_get_text(oid, gid, fid, lid);

  if (log_to_console) {
    ILOG0_DDDDDD("! Log Event: TS=%lu, OID=%u, GID=%u, FID=%u, LID=%u, datalen=%u",
        log_evt.timestamp, oid, gid, fid, lid, dsize);
    if (scd_data) {
      if (!scd_text)
        ILOG0_S("%s", scd_text_not_found);
      else
        ILOG0_SS("%s%s", scd_text_found, scd_text);
    }
  }
  if (log_to_syslog) {
    syslog(syslog_pri, "! Log Event: TS=%lu, OID=%u, GID=%u, FID=%u, LID=%u, datalen=%u",
        (unsigned long)log_evt.timestamp, oid, gid, fid, lid, dsize);
    if (scd_data) {
      if (!scd_text)
        syslog(syslog_pri, "%s", scd_text_not_found);
      else
        syslog(syslog_pri, "%s%s", scd_text_found, scd_text);
    }
  }
  if (text_protocol) {
    printed = snprintf(buf, sizeof(buf),
        "! Log Event: TS=%lu, OID=%u, GID=%u, FID=%u, LID=%u, datalen=%u\r\n",
        (unsigned long)log_evt.timestamp, oid, gid, fid, lid, dsize);
    if (printed < 0 || printed >= sizeof(buf)) {
      ELOG_V("Buffer too small");
      return -1;
    }
    send_to_all(buf, printed);
    if (scd_data) {
      if (!scd_text) {
        send_to_all(scd_text_not_found, ARRAY_SIZE(scd_text_not_found) - 1);
      } else {
        send_to_all(scd_text_found, ARRAY_SIZE(scd_text_found) - 1);
        send_to_all(scd_text, strlen(scd_text));
        send_to_all("\r\n", 2);
      }
    }
  }
  pktlen = sizeof(log_evt) + dsize;

  at += sizeof(log_evt);
  while (at < pktlen) {
    if (pktlen - at < sizeof(log_evt_data)) {
      ELOG_V("Data corrupted");
      return -1;
    }
    cqueue_get(pqueue, at, sizeof(log_evt_data),
        (unsigned char *) &log_evt_data);
    at += sizeof(log_evt_data);

    /* TODO: The code meant to print log
             messages to the console is
             temporary. This code is to 
             be removed as soon as logconv 
             rewritten on C and linked with
             log server. This will allow to 
             have fully-functional debugging 
             output from log server to the 
             console and code below will be
             unnecessary. */

    switch (log_evt_data.datatype) {
    case LOG_DT_LSTRING:
      {
        mtlk_log_lstring_t lstr;
        unsigned char *pstrdata;
        if (pktlen - at < sizeof(lstr)) {
          ELOG_V("Data corrupted");
          return -1;
        }
        cqueue_get(pqueue, at, sizeof(lstr),
            (unsigned char *) &lstr);
        at += sizeof(lstr);
        if (pktlen - at < lstr.len) {
          ELOG_V("Data corrupted");
          return -1;
        }
        pstrdata = (unsigned char *) malloc(lstr.len + 1);
        if (!pstrdata) {
          ELOG_V("Out of memory");
          return -1;
        }
        cqueue_get(pqueue, at, lstr.len, pstrdata);
        at += lstr.len;
        // TODO: replace \0 with ' ' in output
        pstrdata[lstr.len] = '\0';
        if (log_to_console) {
          ILOG0_DS("    datatype = LOG_DT_LSTRING: len = %u, data = %s",
              lstr.len, pstrdata);
        }
        if (log_to_syslog) {
          syslog(syslog_pri,
              "    datatype = LOG_DT_LSTRING: len = %u, data = %s",
              lstr.len, pstrdata);
        }
        if (text_protocol) {
          printed = snprintf(buf, sizeof(buf),
              "    datatype = LOG_DT_LSTRING: len = %u, data = ",
              lstr.len);
          if (printed < 0 || printed >= sizeof(buf)) {
            free(pstrdata);
            ELOG_V("Buffer too small");
            return -1;
          }
          send_to_all(buf, printed);
          send_to_all((char*)pstrdata, lstr.len);
          send_to_all("\r\n", 2);
        }
        free(pstrdata);
      }
      break;
    case LOG_DT_INT8:
      {
        int8 val;
        if (pktlen - at < sizeof(val)) {
          ELOG_V("Data corrupted");
          return -1;
        }
        cqueue_get(pqueue, at, sizeof(val), (unsigned char *) &val);
        at += sizeof(val);
        if (log_to_console) {
          ILOG0_C("    datatype = LOG_DT_INT8: value = %c", val);
        }
        if (log_to_syslog) {
          syslog(syslog_pri, "    datatype = LOG_DT_INT8: value = %c", val);
        }
        if (text_protocol) {
          printed = snprintf(buf, sizeof(buf),
              "    datatype = LOG_DT_INT8: value = %c\r\n", val);
          if (printed < 0 || printed >= sizeof(buf)) {
            ELOG_V("Buffer too small");
            return -1;
          }
          send_to_all(buf, printed);
        }
      }
      break;
    case LOG_DT_INT32:
      {
        int32 val;
        if (pktlen - at < sizeof(val)) {
          ELOG_V("Data corrupted");
          return -1;
        }
        cqueue_get(pqueue, at, sizeof(val), (unsigned char *) &val);
        at += sizeof(val);
        if (log_to_console) {
          ILOG0_D("    datatype = LOG_DT_INT32: value = %ld", val);
        }
        if (log_to_syslog) {
          syslog(syslog_pri, "    datatype = LOG_DT_INT32: value = %ld", (long int)val);
        }
        if (text_protocol) {
          printed = snprintf(buf, sizeof(buf),
              "    datatype = LOG_DT_INT32: value = %ld\r\n", (long int)val);
          if (printed < 0 || printed >= sizeof(buf)) {
            ELOG_V("Buffer too small");
            return -1;
          }
          send_to_all(buf, printed);
        }
      }
      break;
    case LOG_DT_INT64:
      {
        int64 val;
        if (pktlen - at < sizeof(val)) {
          ELOG_V("Data corrupted");
          return -1;
        }
        cqueue_get(pqueue, at, sizeof(val), (unsigned char *) &val);
        at += sizeof(val);
        if (log_to_console) {
          ILOG0_H("    datatype = LOG_DT_INT64: value = %lld", val);
        }
        if (log_to_syslog) {
          syslog(syslog_pri, "    datatype = LOG_DT_INT64: value = %lld", (long long) val);
        }
        if (text_protocol) {
          printed = snprintf(buf, sizeof(buf),
              "    datatype = LOG_DT_INT64: value = %lld\r\n", (long long) val);
          if (printed < 0 || printed >= sizeof(buf)) {
            ELOG_V("Buffer too small");
            return -1;
          }
          send_to_all(buf, printed);
        }
      }
      break;
    case LOG_DT_MACADDR:
      {
        char val[MAC_ADDR_LENGTH];
        if (pktlen - at < sizeof(val)) {
          ELOG_V("Data corrupted");
          return -1;
        }
        cqueue_get(pqueue, at, sizeof(val), (unsigned char *) &val);
        at += sizeof(val);
        if (log_to_console) {
          ILOG0_Y("    datatype = LOG_DT_MACADDR: value = %Y", val);
        }
        if (log_to_syslog) {
          syslog(syslog_pri, "    datatype = LOG_DT_MACADDR: value = " MAC_PRINTF_FMT, MAC_PRINTF_ARG(val));
        }
        if (text_protocol) {
          printed = snprintf(buf, sizeof(buf),
              "    datatype = LOG_DT_MACADDR: value = " MAC_PRINTF_FMT "\r\n", MAC_PRINTF_ARG(val));
          if (printed < 0 || printed >= sizeof(buf)) {
            ELOG_V("Buffer too small");
            return -1;
          }
          send_to_all(buf, printed);
        }
      }
      break;
    case LOG_DT_IP6ADDR:
      {
        char val[IP6_ADDR_LENGTH];
        if (pktlen - at < sizeof(val)) {
          ELOG_V("Data corrupted");
          return -1;
        }
        cqueue_get(pqueue, at, sizeof(val), (unsigned char *) &val);
        at += sizeof(val);
        if (log_to_console) {
          ILOG0_K("    datatype = LOG_DT_IP6ADDR: value = %K", val);
        }
        if (log_to_syslog) {
          syslog(syslog_pri, "    datatype = LOG_DT_IP6ADDR: value = " IP6_PRINTF_FMT, IP6_PRINTF_ARG(val));
        }
        if (text_protocol) {
          printed = snprintf(buf, sizeof(buf),
              "    datatype = LOG_DT_IP6ADDR: value = " IP6_PRINTF_FMT "\r\n", IP6_PRINTF_ARG(val));
          if (printed < 0 || printed >= sizeof(buf)) {
            ELOG_V("Buffer too small");
            return -1;
          }
          send_to_all(buf, printed);
        }
      }
      break;
    default:
      ELOG_D("Unknown log event data type (%u)", log_evt_data.datatype);
      break;
    }
  }

  return 0;
}

// -1 - error
// 0  - no complete packets in queue
// 1  - packet processed succesfully
int
drv_process_next_pkt(cqueue_t *pqueue)
{
  int sz = cqueue_size(pqueue);
  mtlk_log_event_t log_evt;
  uint32 pktlen;
  uint32 datalen;

  if (sz < sizeof(log_evt))
    return 0;

  cqueue_get(pqueue, 0, sizeof(log_evt), (unsigned char *) &log_evt);

  datalen = LOG_INFO_GET_DSIZE(log_evt);
  pktlen  = datalen + sizeof(mtlk_log_event_t);
  ILOG9_DD("pktlen %lu, datalen %lu", pktlen, datalen);

  if (sz < pktlen)
    return 0;

  // At this point there's a complete packet in queue at offset 0.
  // Process this packet.
  if (0 != drv_process_pkt(pqueue)) {
    return -1;
  }

  cqueue_pop_front(pqueue, pktlen);

  return 1;
}

