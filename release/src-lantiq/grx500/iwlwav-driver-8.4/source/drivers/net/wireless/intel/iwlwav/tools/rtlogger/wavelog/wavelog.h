#ifndef __WAVELOG_H__
#define __WAVELOG_H__

#define WAVELOG_MSG_LEN_WITH_HDR      /* Message length with header length */
#define WAVELOG_HDR_PTYPE_SIZE      4 /* FIXME: aligned with sizeof mtlk_log_event_data_t */

/* We need to have uint32 word alignment LOG messages payload */
#define LOGPKT_WORD_ALIGNMENT

#ifdef  LOGPKT_WORD_ALIGNMENT
#define LOGPKT_ITEM_SIZE(x) ((((x) + 3) >> 2) << 2) /* for 4-bytes word */
#else
#define LOGPKT_ITEM_SIZE(x) (x)
#endif

#define FW_SIGN_INFO_STR_LEN 1024
typedef struct _fw_signal_t{
  guint16  src_task_id;
  guint16  dst_task_id;
#ifdef  LOGPKT_WORD_ALIGNMENT
  guint32  len;
#else
  guint16  len;
#endif
  char     info_str[FW_SIGN_INFO_STR_LEN];
}fw_signal_t;

#define SCD_LOG_MSG_INFO_STR_LEN 1024
typedef struct _scd_log_msg_t{
  char     info_str[SCD_LOG_MSG_INFO_STR_LEN];
}scd_log_msg_t;

#define WAVELOG_HDR_PTYPE_FWSIGNAL  6

#define WAVELOG_HDR_PTYPE_MSG_STR   0
#define WAVELOG_HDR_PTYPE_MSG_INT8  1
#define WAVELOG_HDR_PTYPE_MSG_INT32 2
#define WAVELOG_HDR_PTYPE_MSG_INT64 3
#define WAVELOG_HDR_PTYPE_MSG_MAC   4
#define WAVELOG_HDR_PTYPE_MSG_IPV6  5

#define WAVELOG_HDR_PTYPE_MSG_FIRST 0
#define WAVELOG_HDR_PTYPE_MSG_LAST  5

#define MAX_CARDS 4
#define OVERFLOW_THRESHOLD 10000000


typedef struct _wavelog_hdr_logcnv_t{
  guint8  oid;
  guint8  gid;
  guint16 fid;
  guint16 lid;
  guint8  wlanif;
  guint16 src_len;
  const gchar*   src;
  guint16 dst_len;
  const gchar*   dst;
} wavelog_hdr_logcnv_t;


typedef struct _wavelog_hdr_t{
  guint32 oid;
  guint32 gid;
  guint32 fid;
  guint32 lid;
  guint32 data_len;
  guint32 wlanif;

  guint32 timestamp;
  guint64 ts_host;
  guint64 ts_fw[MAX_CARDS];

  guint32  ver;
  gboolean encoding;  /* 1 - network, 0 - Little Endian */

  const gchar *oid_name;
  const gchar *gid_name;
} wavelog_hdr_t;


#define MAX_PROTO_ABBR  1024
typedef struct _dies_info_t {

  GSList *dies_pools_list;

  guint   trees_num;
  guint   fields_num;

  int   hf_idx;
  int   ett_idx;
  char  abbr[MAX_PROTO_ABBR];

} dies_info_t;

#endif  /* __WAVELOG_H__ */
