/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#ifdef CONFIG_WAVE_RTLOG_REMOTE
#include "mtlk_rtlog_app.h"
#endif

int mtlk_log_get_flags(int level, int oid, int gid)
{
  int flags = 0;

  MTLK_UNREFERENCED_PARAM(oid); /* each application has constant unique OID */
  MTLK_UNREFERENCED_PARAM(gid); /* each application uses the same debug level for all GIDs */

  if (level <= IWLWAV_RTLOG_DLEVEL_VAR) {
    flags |= LOG_TARGET_CONSOLE;
  }
  if (level <= IWLWAV_RTLOG_DLEVEL_VAR_REMOTE) {
    flags |= LOG_TARGET_REMOTE;
  }
  return flags;
}

#ifdef CONFIG_WAVE_RTLOG_REMOTE
mtlk_log_buf_entry_t *mtlk_log_new_pkt_reserve(uint32 pkt_size, uint8 **ppdata)
{
  mtlk_log_buf_entry_t *res = (mtlk_log_buf_entry_t *)malloc(sizeof(mtlk_log_buf_entry_t));

  if (NULL != res) {
    memset((void *)res, 0, sizeof(*res));

    res->data = (uint8 *)malloc(pkt_size);
    *ppdata = res->data;

    if (NULL == res->data) {
      free((void *)res);
      return NULL;
    }
    memset((void *)res->data, 0, pkt_size);

    res->size = pkt_size;
  }
  return res;
}

void mtlk_log_new_pkt_release(mtlk_log_buf_entry_t *buf)
{
  uint32 ids;
  mtlk_log_event_t *hdr = (mtlk_log_event_t *)buf->data;

  ids = LOG_INFO_MAKE_WORD(LOG_INFO_GET_LID(*hdr),
                           LOG_INFO_GET_OID(*hdr),
                           LOG_INFO_GET_GID(*hdr),
                           LOG_INFO_GET_FID(*hdr));

  mtlk_rtlog_app_send_log_msg(ids, (uint32)timestamp(),
                              (void *)(buf->data + sizeof(*hdr)),
                              (uint16)(buf->size - sizeof(*hdr)));
  free((void *)buf->data);
  free((void *)buf);
}
#endif
