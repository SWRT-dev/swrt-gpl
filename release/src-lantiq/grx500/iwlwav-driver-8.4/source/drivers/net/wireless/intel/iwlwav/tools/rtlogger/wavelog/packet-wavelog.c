/* packet-wavelog.c
 * Routines for Wave Log Viewer protocol packet disassembly
 * By Andrejs Vasiljevs <andrejs.vasiljevs@lantiq.com>
 * Copyright 2012 Andrejs Vasiljevs
 *
 * $Id:$
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "config.h"

#include <string.h>
#include <epan/packet.h>
#include <epan/prefs.h>
#include <epan/proto_data.h>

#include <scd_reader.h>
#include <scd_dwarf_parser.h>

#include <wiretap/wtap.h>

#define DBG_DEIS_DUMP

#ifdef  DBG_DEIS_DUMP
#define DBG_DEIS_DUMP_STDERR    /* use stderr */

#ifdef  DBG_DEIS_DUMP_STDERR
#include <stdio.h>
#else
#include <wsutil/file_util.h>
#endif  /* DBG_DEIS_DUMP_STDERR */

#endif  /* DBG_DEIS_DUMP */

#include "wavelog.h"

#define UNUSED_PARAM(_param) ((_param)=(_param))

#define TAB_STOP_LAST 44
static gint field_name_tab_stop[] = {20, 28, 36, TAB_STOP_LAST};
static gchar white_space_str[TAB_STOP_LAST+2+1] = "                                              "; /* +2 delimiter; +1 null term. */

static gint32 fw_offsets[MAX_CARDS] = {0};
static guint32 ts_hiword = 1; /* must start with at least 1 to avoid negative timestamps (when 32bit offset is subtracted) */
static guint32 ts_latest = 0;
static guint32 last_frame = 0;

typedef struct wavelog_pkt_data {
  guint64 ts_fw[MAX_CARDS];
  guint64 ts_host;
} wavelog_pkt_data;

static void scd_evt_oid_fmt(gchar *result, guint16 oid);
static void scd_evt_gid_fmt(gchar *result, guint16 gid);

static scd_info_t   *g_scd_info;
static dies_info_t  g_dies_info;

static hf_register_info  *hf_dies = NULL;
static gint **ett_dies = NULL;

static wavelog_hdr_t  wavelog_hdr;
static fw_signal_t    fw_sign;
static scd_log_msg_t  scd_log_msg;

static const char scd_filename[]  = "all.scdm";

static int proto_wavelog = -1;

static int hf_wavelog_evt_info_w0 = -1;
static int hf_wavelog_evt_info_w1 = -1;

static int hf_wavelog_datalen   = -1;
static int hf_wavelog_fid       = -1;
static int hf_wavelog_lid       = -1;
static int hf_wavelog_wlanif    = -1;

static int hf_wavelog_timestamp = -1;
static int hf_wavelog_ts_host   = -1;
static int hf_wavelog_ts_fw0    = -1;
static int hf_wavelog_ts_fw1    = -1;
static int hf_wavelog_ts_fw2    = -1;
static int hf_wavelog_ts_fw3    = -1;
static int hf_wavelog_ts_fw4    = -1;

static int hf_wavelog_bigend    = -1;
static int hf_wavelog_version   = -1;
static int hf_wavelog_oid       = -1;
static int hf_wavelog_gid       = -1;

static int hf_wavelog_log_msg = -1;

static int hf_wavelog_sign = -1;
static int hf_wavelog_sign_src = -1;
static int hf_wavelog_sign_dst = -1;
static int hf_wavelog_sign_len = -1;


static gint ett_evt_info_w0 = -1;
static gint ett_evt_info_w1 = -1;
static gint ett_evt_ts      = -1;
//static gint ett_evt_data_len_v2 = -1;

static gint ett_msghdr = -1;
static gint ett_fwsignal = -1;
static gint ett_fwsighdr = -1;

static gint ett_log_msg = -1;

static gint *ett_wavelog[] = {
    &ett_evt_info_w0,
    &ett_evt_info_w1,
    &ett_evt_ts,
    //&ett_evt_data_len_v2,
    &ett_msghdr,
    &ett_fwsignal,
    &ett_fwsighdr,
    &ett_log_msg
  };

const value_string endianess_info[] = {
    {0x0100,     "Native (Big-endian)"},
    {0x0001,     "Reversed (Little-endian)"},
    {0,      NULL}
};
const value_string endianess_internal[] = {
    {0,     "Native (Big-endian)"},
    {1,     "Reversed (Little-endian)"},
    {0,      NULL}
};

const value_string *val_str_task_id = NULL;

//#define BITFIELD(field) field
#define BITMASK(offset, size) (((1 << size)- 1) << offset)
#define GET_BITS(data, offset, size) ((data >> offset) & ((1 << size)- 1))

void proto_reg_handoff_wavelog(void);

/* Fixed fields definitions */
#if 1
static hf_register_info log_event_hf[] = {

  { &hf_wavelog_evt_info_w0,
    { "Event header info word0", "wl.info_w0",
      FT_UINT32, BASE_HEX, NULL, 0, "", HFILL
    }
  },

  { &hf_wavelog_evt_info_w1,
    { "Event header info word1", "wl.info_w1",
      FT_UINT32, BASE_HEX, NULL, 0, "", HFILL
    }
  },

  { &hf_wavelog_datalen,
    { "Data length", "wl.datalen",
      FT_UINT32, BASE_DEC, NULL, 0, "", HFILL
    }
  },
//  { &hf_wavelog_datalen_v2,
//    { "Data length v2", "wl.datalen",
//      FT_UINT16, BASE_DEC, NULL, 0, "", HFILL
//    }
//  },

  { &hf_wavelog_oid,
    { "Originator ID", "wl.oid",
    // FT_UINT16, BASE_CUSTOM, scd_evt_oid_fmt, BITMASK(12,  5), "", HFILL
    FT_UINT32, BASE_CUSTOM, scd_evt_oid_fmt, BITMASK(12,  5), "", HFILL
    }
  },
  { &hf_wavelog_gid,
    { "Group ID", "wl.gid",
      FT_UINT32, BASE_CUSTOM, scd_evt_gid_fmt, BITMASK( 5,  7), "", HFILL
    }
  },
  { &hf_wavelog_fid,
    { "File ID", "wl.fid",
      FT_UINT32, BASE_DEC, NULL, BITMASK( 0,  5), "", HFILL
    }
  },
  { &hf_wavelog_lid,
    { "Line number", "wl.lid",
      FT_UINT32, BASE_DEC, NULL, BITMASK(17, 14), "", HFILL
    }
  },

  { &hf_wavelog_wlanif,
    { "Wlan interface", "wl.wlanif",
      FT_UINT16, BASE_DEC, NULL, BITMASK( 9,  2), "", HFILL
    }
  },
  { &hf_wavelog_bigend,
    { "Endianess", "wl.endianess",
      FT_UINT16, BASE_HEX, VALS(endianess_info), BITMASK(0, 1) | BITMASK(8, 1), "", HFILL
    }
  },
  { &hf_wavelog_version,
    { "Version", "wl.version",
      FT_UINT16, BASE_DEC, NULL, BITMASK( 1,  4), "", HFILL
    }
  },

  { &hf_wavelog_timestamp,
    { "Timestamp", "wl.timestamp",
      FT_UINT32, BASE_DEC, NULL, 0, "", HFILL
    }
  },

  { &hf_wavelog_ts_host,
    { "TS host", "wl.ts_host",
      FT_UINT64, BASE_DEC, NULL, 0, "", HFILL
    }
  },

  { &hf_wavelog_ts_fw0,
    { "TS FW0", "wl.ts_fw0",
      FT_UINT64, BASE_DEC, NULL, 0, "", HFILL
    }
  },

  { &hf_wavelog_ts_fw1,
    { "TS FW1", "wl.ts_fw1",
      FT_UINT64, BASE_DEC, NULL, 0, "", HFILL
    }
  },

  { &hf_wavelog_ts_fw2,
    { "TS FW2", "wl.ts_fw2",
      FT_UINT64, BASE_DEC, NULL, 0, "", HFILL
    }
  },

  { &hf_wavelog_ts_fw3,
    { "TS FW3", "wl.ts_fw3",
      FT_UINT64, BASE_DEC, NULL, 0, "", HFILL
    }
  },

  { &hf_wavelog_log_msg,
    { "Log message", "wl.log_msg",
      FT_NONE, BASE_NONE, NULL, 0x0, "", HFILL
    }
  },

  { &hf_wavelog_sign,
    { "FW Internal Signal", "wl.sig",
      FT_NONE, BASE_NONE, NULL, 0x0, "", HFILL
    }
  },

  { &hf_wavelog_sign_src,
    { "Signal Source TaskID", "wl.sig.src",
      FT_UINT16, BASE_HEX, NULL, 0x0, "", HFILL
    }
  },

  { &hf_wavelog_sign_dst,
    { "Signal Destination TaskID", "wl.sig.dst",
      FT_UINT16, BASE_HEX, NULL, 0x0, "", HFILL
    }
  },

  { &hf_wavelog_sign_len,
    { "Signal data length", "wl.sig.len",
#ifdef  LOGPKT_WORD_ALIGNMENT
      FT_UINT32, BASE_HEX, NULL, 0x0, "", HFILL
#else
      FT_UINT16, BASE_HEX, NULL, 0x0, "", HFILL
#endif
    }
  }
};
# endif

/* Note: use scd_evt_oid_fmt before of scd_evt_gid_fmt */
static guint16 tmp_oid = 0;

static void
scd_evt_oid_fmt(gchar *result, guint16 oid)
{
  //g_snprintf(result, ITEM_LABEL_LENGTH, "%s (%d)", wavelog_hdr.oid_name, wavelog_hdr.oid);
  g_snprintf(result, ITEM_LABEL_LENGTH, "%s (%d)", scd_get_oid_name(g_scd_info, oid), oid);

  tmp_oid = oid;    /* will be used by scd_evt_gid_fmt */
}

static void
scd_evt_gid_fmt(gchar *result, guint16 gid)
{
  //g_snprintf(result, ITEM_LABEL_LENGTH, "%s (%d)", wavelog_hdr.gid_name, wavelog_hdr.gid);
  g_snprintf(result, ITEM_LABEL_LENGTH, "%s (%d)", scd_get_gid_name(g_scd_info, tmp_oid, gid), gid);
}

static void
decorate_msghdr(tvbuff_t *tvb, proto_tree *tree, proto_item  *item){

  int offset;
  proto_item  *item_info_w0;
  proto_item  *item_info_w1;

  proto_tree  *tree_info_w0;
  proto_tree  *tree_info_w1;

  /* Decorate MSGHDR item */
  if (!tree) return;

  offset = 0;

  /* Decorate event info (word0) */
  item_info_w0 = proto_tree_add_item(tree, hf_wavelog_evt_info_w0, tvb, offset + 0, sizeof(guint32), wavelog_hdr.encoding);

#if 1
  tree_info_w0 = proto_item_add_subtree(item_info_w0, ett_evt_info_w0);

  proto_tree_add_item(tree_info_w0, hf_wavelog_oid       , tvb, offset + 0, sizeof(guint32), wavelog_hdr.encoding);
  proto_tree_add_item(tree_info_w0, hf_wavelog_gid       , tvb, offset + 0, sizeof(guint32), wavelog_hdr.encoding);
  proto_tree_add_item(tree_info_w0, hf_wavelog_fid       , tvb, offset + 0, sizeof(guint32), wavelog_hdr.encoding);
  proto_tree_add_item(tree_info_w0, hf_wavelog_lid       , tvb, offset + 0, sizeof(guint32), wavelog_hdr.encoding);
#endif

  proto_item_set_text(item_info_w0, "INFO_W0: OID:%s, GID:%s, FID:%d, LID:%d",
    wavelog_hdr.oid_name,
    wavelog_hdr.gid_name,
    wavelog_hdr.fid,
    wavelog_hdr.lid);

  /* Decorate event info (word1) */
  item_info_w1 = proto_tree_add_item(tree, hf_wavelog_evt_info_w1, tvb, offset + 6, sizeof(guint16), wavelog_hdr.encoding);

#if 1
  tree_info_w1 = proto_item_add_subtree(item_info_w1, ett_evt_info_w1);

  proto_tree_add_item(tree_info_w1, hf_wavelog_bigend    , tvb, offset + 6, sizeof(guint16), FALSE); /* Always HOST */
  proto_tree_add_item(tree_info_w1, hf_wavelog_wlanif    , tvb, offset + 6, sizeof(guint16), wavelog_hdr.encoding);
  proto_tree_add_item(tree_info_w1, hf_wavelog_version   , tvb, offset + 6, sizeof(guint16), wavelog_hdr.encoding);
#endif

  proto_item_set_text(item_info_w1, "INFO_W1: Ver: %d, IF: %d, Endianess: %s",
    wavelog_hdr.ver,
    wavelog_hdr.wlanif,
    val_to_str(wavelog_hdr.encoding, endianess_internal, "Unknown(%u)"));

  /* Decorate tree root */
  proto_item_set_text(item, "LOG Msg header. IF %d, OID:%s, GID:%s, FID:%d, LID:%d, Len:%d",
    wavelog_hdr.wlanif,
    wavelog_hdr.oid_name,
    wavelog_hdr.gid_name,
    wavelog_hdr.fid,
    wavelog_hdr.lid,
    wavelog_hdr.data_len);

  /* Data length */
  proto_tree_add_item(tree, hf_wavelog_datalen   , tvb, offset + 4, sizeof(guint16), wavelog_hdr.encoding);

  /* Timestamp */
  proto_tree_add_item(tree, hf_wavelog_timestamp , tvb, offset + 8, sizeof(guint32), wavelog_hdr.encoding);
}

static gint
dissect_msghdr(tvbuff_t *tvb){
  guint32 evt_info_w0, evt_ts;
  guint16 evt_datalen;
  guint16 evt_info_w1;
  gint    bytes_read;
  gboolean encoding;

  evt_info_w1 = tvb_get_ntohs(tvb, 6);
  encoding = (0 != GET_BITS(evt_info_w1, 0,  1));  /* BE0 */

  wavelog_hdr.encoding = encoding;

  if (encoding){
    /* Reversed */
    evt_info_w0 = tvb_get_letohl(tvb, 0);
    evt_datalen = tvb_get_letohs(tvb, 4);
    evt_info_w1 = tvb_get_letohs(tvb, 6);
    evt_ts      = tvb_get_letohl(tvb, 8);
  } else {
    /* Native */
    evt_info_w0 = tvb_get_ntohl(tvb, 0);
    evt_datalen = tvb_get_ntohs(tvb, 4);
    evt_info_w1 = tvb_get_ntohs(tvb, 6);
    evt_ts      = tvb_get_ntohl(tvb, 8);
  }

  wavelog_hdr.lid       = GET_BITS(evt_info_w0, 17, 14);
  wavelog_hdr.oid       = GET_BITS(evt_info_w0, 12,  5);
  wavelog_hdr.gid       = GET_BITS(evt_info_w0,  5,  7);
  wavelog_hdr.fid       = GET_BITS(evt_info_w0,  0,  5);

  wavelog_hdr.ver       = GET_BITS(evt_info_w1,  1,  4);
  wavelog_hdr.wlanif    = GET_BITS(evt_info_w1,  9,  2);

  wavelog_hdr.data_len  = evt_datalen;
  wavelog_hdr.timestamp = evt_ts;

  bytes_read = sizeof(evt_info_w0) + sizeof(evt_info_w1) + sizeof(evt_datalen) + sizeof(evt_ts);

  wavelog_hdr.gid_name = scd_get_gid_name(g_scd_info, wavelog_hdr.oid, wavelog_hdr.gid);
  wavelog_hdr.oid_name = scd_get_oid_name(g_scd_info, wavelog_hdr.oid);

  return bytes_read;
}


void handle_timestamps(tvbuff_t *tvb, proto_tree *tree, packet_info *pinfo)
{
  int i;
  wavelog_pkt_data* wd;

  wd = (wavelog_pkt_data*)p_get_proto_data(wmem_file_scope(), pinfo, proto_wavelog, 0);
  if (!wd) {
    wd = wmem_new(wmem_file_scope(), wavelog_pkt_data);
    p_add_proto_data(wmem_file_scope(), pinfo, proto_wavelog, 0, wd);
    memset(wd, 0, sizeof(wavelog_pkt_data));

    if (wavelog_hdr.oid >= 2) /* supplied timestamp is from linux */
    {
      if ((guint64)wavelog_hdr.timestamp + OVERFLOW_THRESHOLD < ts_latest)
        ts_hiword++; /* detected overflow */
      ts_latest = wavelog_hdr.timestamp;

      for (i = 0; i < MAX_CARDS; i++) {
        if (fw_offsets[i]) {
          wavelog_hdr.ts_fw[i] = ((guint64)ts_hiword << 32) + wavelog_hdr.timestamp + fw_offsets[i];
          wd->ts_fw[i] = wavelog_hdr.ts_fw[i];
        }
      }
      wd->ts_host = wavelog_hdr.ts_host = ((guint64)ts_hiword << 32) | wavelog_hdr.timestamp;
    }
    else /* fw_ts = host_ts + fw_off[x] */
    { /* intentional 32bit overflow */
      guint64 host_ts = wavelog_hdr.timestamp - fw_offsets[wavelog_hdr.wlanif];

      host_ts |= ((guint64)ts_hiword << 32);

      if ((host_ts + OVERFLOW_THRESHOLD) < (((guint64)ts_hiword << 32) | ts_latest)) {
        host_ts += (guint64)1 << 32; /* do not update ts_hiword here */
      }

      for (i = 0; i < MAX_CARDS; i++) {
        if (fw_offsets[i]) {
          wavelog_hdr.ts_fw[i] = host_ts + fw_offsets[i];
          wd->ts_fw[i] = wavelog_hdr.ts_fw[i];
        }
      }
      wd->ts_host = wavelog_hdr.ts_host = host_ts;
    }

  } else { /* already have timestamps calculated */
    for (i = 0; i < MAX_CARDS; i++) {
      wavelog_hdr.ts_fw[i] = wd->ts_fw[i];
    }
    wavelog_hdr.ts_host = wd->ts_host;
  }

  /* Timestamp FW per card */
  proto_tree_add_uint64(tree, hf_wavelog_ts_host, tvb, 8, sizeof(guint64), wavelog_hdr.ts_host);

  proto_tree_add_uint64(tree, hf_wavelog_ts_fw0, tvb, 8, sizeof(guint64), wavelog_hdr.ts_fw[0]);
  proto_tree_add_uint64(tree, hf_wavelog_ts_fw1, tvb, 8, sizeof(guint64), wavelog_hdr.ts_fw[1]);
  proto_tree_add_uint64(tree, hf_wavelog_ts_fw2, tvb, 8, sizeof(guint64), wavelog_hdr.ts_fw[2]);
  proto_tree_add_uint64(tree, hf_wavelog_ts_fw3, tvb, 8, sizeof(guint64), wavelog_hdr.ts_fw[3]);
}

static const gchar*
get_padding(const gchar* str){

  gint  n, i;
  const gchar* padding;

  padding = &white_space_str[TAB_STOP_LAST];
  n = (gint)strlen(str);
  for(i = 0; i < (gint)array_length(field_name_tab_stop); i++){
    if ( n < field_name_tab_stop[i]){
      padding -= field_name_tab_stop[i] - n;
      break;
    }
  }
  return padding;
}

static dies_memb_t *
dissec_fw_sign_by_memb(dies_memb_t *dies_memb, tvbuff_t *tvb, int offset, proto_tree  *tree_parent, proto_item  *ti_parent){

  proto_tree  *tree_dies_memb;
  proto_item  *ti_dies_memb;
  UNUSED_PARAM(ti_parent);

  /* Proceed all sub elements of DIES Entry*/
  while(1){
    gint is_last = dies_memb->flag & DIES_MEMB_FLAG_IS_LAST;

    /* Try to parse incomplete structures */
#if 0
    if (tvb_length(tvb) < (offset + dies_memb->size)){
      offset  = offset;     /* just place holder for break point */
      return (dies_memb+1); /* some thing wrong. PC must not hit here */
    }
#endif

    ti_dies_memb = proto_tree_add_item(
      tree_parent,                 /* Parent tree */
      dies_memb->hf_id,            /* Item ID */
      tvb,                         /* Packet Buffer */
      offset + dies_memb->offset,  /* Start */
      dies_memb->size,             /* Length */
      wavelog_hdr.encoding);       /* Encoding */

    proto_item_prepend_text(ti_dies_memb, "%s%s",
      dies_memb->type_name,
      get_padding(dies_memb->type_name));

    if (dies_memb->flag & DIES_MEMB_FLAG_IS_PARENT) {

      tree_dies_memb = proto_item_add_subtree(ti_dies_memb, dies_memb->ett_id);

      if (dies_memb->memb_type == DIES_MEMB_TYPE_ARRAY){
        guint array_size, array_index, array_offset;
        dies_memb_t *dies_array_el;

        array_offset = offset + dies_memb->offset;
        array_index = 0;
        array_size = dies_memb->size;

        dies_array_el = dies_memb+1;
        while (array_size >= dies_array_el->size) {
          dies_memb = dissec_fw_sign_by_memb(dies_array_el, tvb, array_offset, tree_dies_memb, ti_dies_memb); /* Recursive call */

          proto_item_prepend_text(tree_dies_memb->last_child, "[%d] ", array_index);

          array_size -= dies_array_el->size;
          array_offset += dies_array_el->size;
          array_index += 1;
        }

        /* for zero length arrays */
        if (array_index == 0 && array_size < dies_array_el->size) {
          dies_memb++;
        }

      } else {
        /* Proceed with 1st member of next level */
        dies_memb = dissec_fw_sign_by_memb(dies_memb+1, tvb, offset + dies_memb->offset, tree_dies_memb, ti_dies_memb); /* Recursive call */
      }

      /* Decorate TREE item */
      // TODO: Decorate TREE item
    }

    /* Proceed with next member on that level */
    if (is_last){
      break;
    }

    dies_memb++;

  } /* while(1)*/

  return dies_memb;
}

static gchar*
find_next_fmt(gchar *pos){

  while(pos){
    pos = strchr(pos, '%');
    if (pos == NULL) break;
    if (pos[1] != '%') break;
    pos += 2;
  }
  return pos;
}

static gint
process_nonstd_fmt(tvbuff_t *tvb, int *p_offset, char **p_log_msg_pos, gint log_msg_rem, guint8 payload_type, gchar **p_scd_msg){

/* TODO: check g_snprintf return value for WIN32 and GNUC */

  int offset;
  char *out_pos, *scd_msg_pos, *delim_pos;
  int res;
  gint out_bw;
  guint tvb_remaining;

  offset = *p_offset;
  out_pos = *p_log_msg_pos;
  scd_msg_pos = *p_scd_msg + 1; /* +1 to skip '%' */
  out_bw = 0;

  res = tvb_captured_length_remaining(tvb, offset);
  if (res > 0) tvb_remaining = (guint)res;
  else return -1;

  /* Process non standard formats */
  res = RESULT_OK;
  switch(*scd_msg_pos){
    case 'Y' :
      {  /* MAC DD:DD:DD:DD:DD:DD */
#ifdef  LOGPKT_WORD_ALIGNMENT
        guint8 mac[8];  /* round up to 4x */
#else
        guint8 mac[6];
#endif
        if (payload_type != WAVELOG_HDR_PTYPE_MSG_MAC){ res = RESULT_FAIL; break; }
        if (tvb_remaining < sizeof(mac)){ res = RESULT_FAIL; break; }

        tvb_memcpy(tvb, mac, offset, sizeof(mac)); /* TODO: Check byte order from FW & DRV */
        offset += sizeof(mac);
        out_bw += g_snprintf(out_pos, log_msg_rem, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
        break;
      }

    case 'B' :
      { /* IP4 DD.DD.DD.DD */
        guint8 ipv4[4];
        if (payload_type != WAVELOG_HDR_PTYPE_MSG_INT32){ res = RESULT_FAIL; break; }
        if (tvb_remaining < sizeof(ipv4)){ res = RESULT_FAIL; break; }

        tvb_memcpy(tvb, ipv4, offset, sizeof(ipv4)); /* TODO: Check byte order from FW & DRV */
        offset += sizeof(ipv4);
        out_bw += g_snprintf(out_pos, log_msg_rem, "%d.%d.%d.%d",
          ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
        break;
      }

    case 'K' :
      { /* IPv6 DDDD:DDDD:DDDD:DDDD:DDDD:DDDD:DDDD:DDDD */
        guint16 ipv6[8];
        if (payload_type != WAVELOG_HDR_PTYPE_MSG_IPV6){ res = RESULT_FAIL; break; }
        if (tvb_remaining < sizeof(ipv6)){ res = RESULT_FAIL; break; }

        tvb_memcpy(tvb, ipv6, offset, sizeof(ipv6));
        offset += sizeof(ipv6);
        out_bw += g_snprintf(out_pos, log_msg_rem, "%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X",
          ipv6[0], ipv6[1], ipv6[2], ipv6[3], ipv6[4], ipv6[5], ipv6[6], ipv6[7]);
        break;
      }
  };

  if (res == RESULT_FAIL){
    return -1;  /* Error - terminate packet processing */
  }

  if (out_bw == 0){
    return 0;   /* nonstd format not recognized - continue processing */
  }

  /* Append SCD message remainder till next '%'*/
  out_pos += out_bw;
  scd_msg_pos ++;

  delim_pos = find_next_fmt(scd_msg_pos);

  /* Truncate SCD till next '%' */
  if (delim_pos){ *delim_pos = 0; }

  out_bw += g_snprintf(out_pos, log_msg_rem - out_bw, "%s", scd_msg_pos);

  /* Restore truncated string */
  if (delim_pos){ *delim_pos = '%'; }

  *p_offset = offset;
  *p_log_msg_pos += out_bw;
  *p_scd_msg = delim_pos; /* return NULL if SCD_MSG fully processed */

  return out_bw;
}

static gint
process_std_fmt(tvbuff_t *tvb, gint *p_offset, gchar **p_log_msg_pos, gint log_msg_rem, guint8 payload_type, gchar **scd_msg){

/* TODO: Check byte order from FW & DRV */
/* TODO: check g_snprintf return value for WIN32 and GNUC */

  gint res;
  gchar *out_pos, *scd_msg_pos, *delim_pos;
  gint  offset, out_bw;
  guint tvb_remaining;

  guint   payload_len = 0;
  guint8  payload_uint8;
  guint32 payload_uint32;
  guint64 payload_uint64;

  offset = *p_offset;
  out_pos = *p_log_msg_pos;
  scd_msg_pos = *scd_msg;
  out_bw = 0;

  res = tvb_captured_length_remaining(tvb, offset);
  if (res > 0) tvb_remaining = (guint)res;
  else return -1;

  /* Append SCD message remainder till next '%'*/
  delim_pos = find_next_fmt(scd_msg_pos+1);

  /* Truncate SCD till next '%' */
  if (delim_pos){ *delim_pos = 0; }

  res = RESULT_OK;
  switch(payload_type){
    case WAVELOG_HDR_PTYPE_MSG_STR   :
      { guint8  *payload_str = NULL;

        /* Get string length */
#ifdef  LOGPKT_WORD_ALIGNMENT
        if (tvb_remaining < sizeof(guint32)){ res = RESULT_FAIL; break;}
        payload_len = wavelog_hdr.encoding ? tvb_get_letohl(tvb, offset) : tvb_get_ntohl(tvb, offset);
#else
        if (tvb_remaining < sizeof(guint16)){ res = RESULT_FAIL; break;}
        payload_len = wavelog_hdr.encoding ? tvb_get_letohs(tvb, offset) : tvb_get_ntohs(tvb, offset);
#endif

        /* Get string itself*/
#ifdef  LOGPKT_WORD_ALIGNMENT
        if (tvb_remaining < payload_len + sizeof(guint32)) { res = RESULT_FAIL; break;}
        payload_str = tvb_get_string_enc(NULL, tvb, offset + sizeof(guint32), payload_len, ENC_ASCII);
        payload_len += sizeof(guint32);
        payload_len  = LOGPKT_ITEM_SIZE(payload_len);
#else
        if (tvb_remaining < payload_len + sizeof(guint16)) { res = RESULT_FAIL; break;}
        payload_str = tvb_get_string(tvb, offset + sizeof(guint16), payload_len);
        payload_len += sizeof(guint16);
#endif

        out_bw = g_snprintf(out_pos, (gulong)log_msg_rem, scd_msg_pos, payload_str);
        g_free(payload_str);
        break;
      }
    case WAVELOG_HDR_PTYPE_MSG_INT8  :
#ifdef  LOGPKT_WORD_ALIGNMENT
      if (tvb_remaining < sizeof(guint32)){ res = RESULT_FAIL; break;}
      payload_uint8 = wavelog_hdr.encoding ? tvb_get_letohl(tvb, offset) : tvb_get_ntohl(tvb, offset);
      payload_len = 4;
#else
      if (tvb_remaining < sizeof(guint8)){ res = RESULT_FAIL; break;}
      payload_uint8 = tvb_get_guint8(tvb, offset);
      payload_len = 1;
#endif

      out_bw = g_snprintf(out_pos, (gulong)log_msg_rem, scd_msg_pos, payload_uint8);
      break;
    case WAVELOG_HDR_PTYPE_MSG_INT32 :
      if (tvb_remaining < sizeof(guint32)){ res = RESULT_FAIL; break;}
      payload_uint32 = wavelog_hdr.encoding ? tvb_get_letohl(tvb, offset) : tvb_get_ntohl(tvb, offset);
      payload_len = 4;
      out_bw = g_snprintf(out_pos, (gulong)log_msg_rem, scd_msg_pos, payload_uint32);
      break;

    case WAVELOG_HDR_PTYPE_MSG_INT64 :
      if (tvb_remaining < sizeof(guint64)){ res = RESULT_FAIL; break;}
      payload_uint64 = wavelog_hdr.encoding ? tvb_get_letoh64(tvb, offset) : tvb_get_ntoh64(tvb, offset);
      payload_len = sizeof(guint64);
      out_bw = g_snprintf(out_pos, (gulong)log_msg_rem, scd_msg_pos, payload_uint64);
      break;

    /* Non std format */
    case WAVELOG_HDR_PTYPE_MSG_MAC   :
    case WAVELOG_HDR_PTYPE_MSG_IPV6  :
      res = RESULT_FAIL;
      break;
  }

  /* Restore truncated string */
  if (delim_pos){ *delim_pos = '%'; }

  if (res == RESULT_FAIL){
    return -1;  /* Error - terminate packet processing */
  }

  if (out_bw < 0){
    out_bw = g_snprintf(out_pos, (gulong)log_msg_rem, "%%invalid qualifier%%");
    return -1;
  }

  offset += payload_len;  /* advance offset in case of recognized format qualifier */

  *p_offset = offset;
  *p_log_msg_pos += out_bw;
  *scd_msg = delim_pos; /* return NULL if SCD_MSG fully processed */

  return out_bw; /* if 0 std format not recognized - continue processing */
}

static int
dissect_log_msg(tvbuff_t *tvb, gint prev_offset, char *scd_msg, proto_tree *tree, proto_item  *item){

  gint chars_written;
  gint offset;
  gint log_msg_rem;

  gchar *log_msg;

  gchar *scd_msg_pos;
  gchar *log_msg_pos;
  gchar *delim_pos;

  UNUSED_PARAM(tree);

  log_msg = (gchar *)g_malloc(SCD_LINE_SIZE);
  log_msg[0] = 0;

  offset = prev_offset;
  scd_msg_pos = scd_msg;
  log_msg_pos = log_msg;
  log_msg_rem = SCD_LINE_SIZE;

  /* Process SCD MSG till first '%' */
  delim_pos = find_next_fmt(scd_msg_pos);

  if (delim_pos){ *delim_pos = 0; }     /* Truncate on first %'*/
  chars_written = g_snprintf(log_msg_pos, (gulong)log_msg_rem, "%s", scd_msg_pos);
  if (chars_written < 0){
    chars_written = g_snprintf(log_msg_pos, (gulong)log_msg_rem, "%%invalid qualifier%%");
  }
  log_msg_pos += chars_written;
  if (delim_pos){ *delim_pos = '%'; }   /* Restore truncated string */

  /* Process payload fields if exist */
  while(delim_pos){
    guint8 payload_type;

    log_msg_rem -= chars_written;

    if (tvb_captured_length_remaining(tvb, offset) >= WAVELOG_HDR_PTYPE_SIZE){

#if (WAVELOG_HDR_PTYPE_SIZE == 4) /* 4 bytes */
      payload_type = wavelog_hdr.encoding ? tvb_get_letohl(tvb, offset) : tvb_get_ntohl(tvb, offset);
      offset += 4;
#else /* old: 1 byte */
      payload_type = tvb_get_guint8(tvb, offset);
      offset += 1;
#endif

    }else{
      break;
    }

    /* Process SCD string starting from '%' */
    chars_written = process_nonstd_fmt(tvb, &offset, &log_msg_pos, log_msg_rem, payload_type, &delim_pos);
    if (chars_written == -1) break;
    if (chars_written > 0) continue;

    /* (res == 0) -> non std format not found */

    chars_written = process_std_fmt(tvb, &offset, &log_msg_pos, log_msg_rem, payload_type, &delim_pos);
    if (chars_written == -1) break;
    if (chars_written > 0) continue;

    /* (chars_written == 0) -> format not recognized or null string */
  }

  /* Set LOG message */
  proto_item_set_text(item, "LOG MSG: %s", log_msg);

  /* Prepare FW signal info for columns */
  /* TODO: add FW/DRV info */
  g_snprintf(scd_log_msg.info_str, SCD_LOG_MSG_INFO_STR_LEN,"%s", log_msg);

  g_free(log_msg);

  return -1;
}

static int
dissect_fw_signal(tvbuff_t *tvb, gint prev_offset, scd_evt_t *scd_evt, proto_tree *tree, proto_item  *item){

  proto_tree  *tree_fw_sig_hdr;
  proto_item  *ti_fw_sig_hdr;
  gint offset;
  guint8 payload_type;

  dies_memb_t *dies_memb_root;
  proto_tree  *tree_dies_memb = NULL;
  proto_item  *ti_dies_memb;

  offset = prev_offset;

  /* HW Signal doesn't include Signal Header and payload_type, start processing data*/
  if (scd_evt->tag == SCD_EVT_TAG_HWSIG) {
    goto process_data;
  }

#if (WAVELOG_HDR_PTYPE_SIZE == 4) /* 4 bytes */
  payload_type = wavelog_hdr.encoding ? tvb_get_letohl(tvb, offset + 0) : tvb_get_ntohl(tvb, offset + 0);
  offset += 4;
#else /* old: 1 byte */
  payload_type = tvb_get_guint8(tvb, offset + 0);
  offset += 1;
#endif

  if (payload_type != WAVELOG_HDR_PTYPE_FWSIGNAL){
    /* TODO: report some error */
    return -1;
  }

  if (wavelog_hdr.encoding){
    fw_sign.src_task_id = tvb_get_letohs(tvb, offset + 0);
    fw_sign.dst_task_id = tvb_get_letohs(tvb, offset + 2);
#ifdef  LOGPKT_WORD_ALIGNMENT
    fw_sign.len         = tvb_get_letohl(tvb, offset + 4);
#else
    fw_sign.len         = tvb_get_letohs(tvb, offset + 4);
#endif
  }else{
    fw_sign.src_task_id = tvb_get_ntohs(tvb, offset + 0);
    fw_sign.dst_task_id = tvb_get_ntohs(tvb, offset + 2);
#ifdef  LOGPKT_WORD_ALIGNMENT
    fw_sign.len         = tvb_get_ntohl(tvb, offset + 4);
#else
    fw_sign.len         = tvb_get_ntohs(tvb, offset + 4);
#endif
  }

  ti_fw_sig_hdr   = proto_tree_add_item(tree, proto_wavelog, tvb, offset, 6, wavelog_hdr.encoding);
  tree_fw_sig_hdr = proto_item_add_subtree(ti_fw_sig_hdr, ett_fwsighdr);

  proto_tree_add_item(tree_fw_sig_hdr, hf_wavelog_sign_src, tvb, offset + 0, sizeof(fw_sign.src_task_id), wavelog_hdr.encoding);
  proto_tree_add_item(tree_fw_sig_hdr, hf_wavelog_sign_dst, tvb, offset + 2, sizeof(fw_sign.dst_task_id), wavelog_hdr.encoding);
  proto_tree_add_item(tree_fw_sig_hdr, hf_wavelog_sign_len, tvb, offset + 4, sizeof(fw_sign.len), wavelog_hdr.encoding);

#ifdef  LOGPKT_WORD_ALIGNMENT
  offset += 8;
#else
  offset += 6;
#endif

  /* Decorate FW SIGNAL tree */
  proto_item_set_text(ti_fw_sig_hdr,  "%s -> %s",
    val_to_str(fw_sign.src_task_id, val_str_task_id, "(%u)"),
    val_to_str(fw_sign.dst_task_id, val_str_task_id, "(%u)"));

  /* Prepare FW signal info for columns */
  g_snprintf(fw_sign.info_str, FW_SIGN_INFO_STR_LEN,"SIGNAL: %s -> %s, %s",
      val_to_str(fw_sign.src_task_id, val_str_task_id, "(%u)"),
      val_to_str(fw_sign.dst_task_id, val_str_task_id, "(%u)"),
      scd_evt->name);

  proto_item_set_text(item, "%s", fw_sign.info_str);


  /* *** DATA TYPE processing started here  ***/
process_data:
  dies_memb_root = (dies_memb_t*)scd_evt->user_data;

  if (NULL == dies_memb_root){
    if (scd_evt->tag == SCD_EVT_TAG_HWSIG) {
      proto_item *new_item = proto_tree_add_item(tree, hf_wavelog_sign, tvb, prev_offset, -1, FALSE);
      proto_item_set_text(new_item, "%s", scd_evt->name);
    }

    proto_item_append_text(item, " (Data type unknown)");
    return -1;
  }

  /* For items with zero size (most probably not enough information in elf file) */
  if (dies_memb_root->size == 0) {
    proto_item_append_text(item, " (Zero size item)");
    return -1;
  }

  /* Add type ROOT Entry */
  ti_dies_memb = proto_tree_add_item(
    tree,                        /* Parent tree */
    dies_memb_root->hf_id,       /* Item ID */
    tvb,                         /* Packet Buffer */
    offset,                      /* Start */
    dies_memb_root->size,        /* Length */
    wavelog_hdr.encoding);       /* Encoding */

  /* Add subtree for non single ROOT entries */
  if (dies_memb_root->flag != DIES_MEMB_FLAG_IS_ROOT) {
    tree_dies_memb = proto_item_add_subtree(ti_dies_memb, dies_memb_root->ett_id);
  }

  /* Cont with other members */
  dissec_fw_sign_by_memb(dies_memb_root+1, tvb, offset, tree_dies_memb, ti_dies_memb); /* Recursive call */

  /* Append some fields from dies memb to FW_SIGNAL description */
  /* TODO: ... */

  /* Append some fields from dies memb to COL info */
  /* TODO: ... */

  return -1;
}

static void
decorate_cols_by_scd_log_msg(packet_info *pinfo){

  col_set_str(pinfo->cinfo, COL_PROTOCOL, "LOG");
  col_add_str(pinfo->cinfo, COL_INFO, scd_log_msg.info_str);

  return;
}

static void
decorate_cols_by_signal(packet_info *pinfo){

  const char *src_str, *dst_str;

  src_str = val_to_str(fw_sign.src_task_id, val_str_task_id, "(%u)");
  dst_str = val_to_str(fw_sign.dst_task_id, val_str_task_id, "(%u)");

  set_address(&pinfo->src, AT_STRINGZ, strlen(src_str), src_str);
  set_address(&pinfo->dst, AT_STRINGZ, strlen(dst_str), dst_str);

  col_set_str(pinfo->cinfo, COL_PROTOCOL, "SIGNAL");

  col_add_str(pinfo->cinfo, COL_INFO, fw_sign.info_str);

  return;
}

static void
decorate_cols_by_msghdr(packet_info *pinfo)
{
  col_set_str(pinfo->cinfo, COL_PROTOCOL, "wavelog");
  col_add_str(pinfo->cinfo, COL_INFO, "Unknown message. EVT not found in SCD file");

  return;
}


static int
_dissect_wavelog(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  proto_tree    *tree_msghdr  = NULL;
  proto_item    *ti_msghdr    = NULL;

  gint          msghdr_offset;

  const gchar   *log_msg;
  scd_evt_t     *scd_evt;
  gint          temp;

  msghdr_offset = dissect_msghdr(tvb);

  if (tree) {
    ti_msghdr   = proto_tree_add_item(tree, proto_wavelog, tvb, 0, msghdr_offset, FALSE);

    tree_msghdr = proto_item_add_subtree(ti_msghdr, ett_msghdr);
    decorate_msghdr(tvb, tree_msghdr, ti_msghdr);
  }

  /* Try to find SCD message by GOLF */
  log_msg = NULL;

  scd_evt = scd_get_by_ids(g_scd_info,
      wavelog_hdr.oid,
      wavelog_hdr.gid,
      wavelog_hdr.fid,
      wavelog_hdr.lid);

  if (NULL == scd_evt){
    decorate_cols_by_msghdr(pinfo);
    return tvb_captured_length(tvb);
  } else handle_timestamps(tvb, tree_msghdr, pinfo);

  switch (scd_evt->tag){
    case SCD_EVT_TAG_SIG:

      if (tree) {
        proto_tree  *tree_signal  = NULL;
        proto_item  *ti_signal    = NULL;

        ti_signal   = proto_tree_add_item(tree, hf_wavelog_sign, tvb, msghdr_offset, -1, FALSE);
        tree_signal = proto_item_add_subtree(ti_signal, ett_fwsignal);
        dissect_fw_signal(tvb, msghdr_offset, scd_evt, tree_signal, ti_signal);
      }
      decorate_cols_by_signal(pinfo);
      break;

    case SCD_EVT_TAG_HWSIG: {
        char info_str[FW_SIGN_INFO_STR_LEN];

        col_set_str(pinfo->cinfo, COL_PROTOCOL, "SIGNAL");
        g_snprintf(info_str, FW_SIGN_INFO_STR_LEN,"HWSIGNAL: %s", scd_evt->name);
        col_add_str(pinfo->cinfo, COL_INFO, info_str);

        if (tree) {
          dissect_fw_signal(tvb, msghdr_offset, scd_evt, tree, NULL);
        }
      }
      break;

    case SCD_EVT_TAG_MSG:
      log_msg = scd_evt->name;
      if (tree) {
        proto_item  *ti_log_msg   = NULL;
        proto_tree  *tree_log_msg = NULL;
        gint item_offset;

        /* Set item offset to 0 for empty packets. Set offset to payload otherwise; */
        item_offset = tvb_captured_length_remaining(tvb, msghdr_offset) ? msghdr_offset : 0;

        /* Add item without payload */
        ti_log_msg = proto_tree_add_item(tree, hf_wavelog_log_msg, tvb, item_offset, -1, FALSE);

        tree_log_msg = proto_item_add_subtree(ti_log_msg, ett_log_msg);
        dissect_log_msg(tvb, msghdr_offset, (gchar*)log_msg, tree_log_msg, ti_log_msg);
      }
      decorate_cols_by_scd_log_msg(pinfo);
      break;
    default:
      /* Payload unknown */
      decorate_cols_by_msghdr(pinfo);
      break;
  }

#ifdef WAVELOG_MSG_LEN_WITH_HDR
  temp = wavelog_hdr.data_len;
#else
  temp = msghdr_offset + wavelog_hdr.data_len;
#endif

  return temp;
}

#define __MSGHDR_PADDING    (2)     /* Padding between UDP header and 1st LOG msghdr */
#define WL_PCK_HDR_SIZE     12
#define TYPE_TSF            0xFFFFFFFF

static gint
_dissect_wavelog_packet(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    gint        len, total;
    gint        offset = 0;
    tvbuff_t    *next_tvb;

    if (!pinfo->fd->flags.visited && (pinfo->fd->num <= last_frame)) {
      ts_hiword = 1; /* start from 1 every time */
      ts_latest = 0;
    }

    last_frame = pinfo->fd->num;

    total = tvb_captured_length(tvb);

    offset += __MSGHDR_PADDING;     /* skip padding */

    while (WL_PCK_HDR_SIZE <= tvb_captured_length_remaining(tvb, offset)) {
        /* get msg size */
        next_tvb = tvb_new_subset(tvb, offset, -1, -1);

        if (TYPE_TSF == tvb_get_ntohl(next_tvb, 0))
        {
          gint hw_idx;
          guint32 ts;

          hw_idx = tvb_get_ntohl(next_tvb, 4);
          if (hw_idx >= MAX_CARDS) break;
          fw_offsets[hw_idx] = tvb_get_ntohl(next_tvb, 12);
          offset += 16;

          ts = tvb_get_ntohl(next_tvb, 8);
          if ((guint64)ts + OVERFLOW_THRESHOLD < ts_latest)
            ts_hiword++; /* detected overflow */
          ts_latest = ts;

          continue;
        }

#ifdef WAVELOG_MSG_LEN_WITH_HDR
        dissect_msghdr(next_tvb);       /* parse header */
        len  = 0;
#else
        len  = dissect_msghdr(next_tvb);    /* len of header */
#endif

        len += wavelog_hdr.data_len;        /* + data */

        if (offset + len > total)
            break;

        /* get message */
        next_tvb = tvb_new_subset(tvb, offset, len, len);
        add_new_data_source(pinfo, next_tvb, "LogMsg");

        //offset +=
            _dissect_wavelog(next_tvb, pinfo, tree);

        offset += len;
    }

    return total;

    //return dissect_wavelog(tvb, pinfo, tree);
}


#if (VERSION_MAJOR==1)
#if (VERSION_MINOR >= 10)
static void
dissect_wavelog(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    _dissect_wavelog(tvb, pinfo, tree);
}

static int new_dissect_wavelog(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* param)
{
    UNUSED_PARAM(param);
    return _dissect_wavelog(tvb, pinfo, tree);
}

static gint new_dissect_wavelog_packet(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* param)
{
    UNUSED_PARAM(param);
    return _dissect_wavelog_packet(tvb, pinfo, tree);
}
#else
static int
dissect_wavelog(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    return _dissect_wavelog(tvb, pinfo, tree);
}

static int new_dissect_wavelog(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    return _dissect_wavelog(tvb, pinfo, tree);
}

static gint new_dissect_wavelog_packet(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
    return _dissect_wavelog_packet(tvb, pinfo, tree);
}
#endif /* (VERSION_MINOR >= 10) */
#else
static int dissect_wavelog(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data _U_)
{
    return _dissect_wavelog(tvb, pinfo, tree);
}

static int new_dissect_wavelog(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data)
{
    return _dissect_wavelog(tvb, pinfo, tree);
}

static gint new_dissect_wavelog_packet(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree, void* data)
{
    return _dissect_wavelog_packet(tvb, pinfo, tree);
}
#endif /* (VERSION_MAJOR==1) */


static dies_memb_t *
calc_trees_fields_by_die(dies_info_t  *dies_info, dies_memb_t *dies_memb){

  while(1){
    gint is_last = dies_memb->flag & DIES_MEMB_FLAG_IS_LAST;

    dies_info->fields_num++;

    if (dies_memb->flag & DIES_MEMB_FLAG_IS_PARENT) {

      if ( dies_memb->memb_type != DIES_MEMB_TYPE_STRUCT &&
           dies_memb->memb_type != DIES_MEMB_TYPE_ARRAY){
        dies_memb->memb_type = dies_memb->memb_type ;
      }

      /* Struct or Array */
      dies_info->trees_num++;

      /* Proceed with 1st member of next level */
      dies_memb++;
      dies_memb = calc_trees_fields_by_die(dies_info, dies_memb);      /* Recursive call */
    }

    /* Proceed with next member on that level */
    if (is_last){
      break;
    }

    dies_memb++;

  } /* while(1)*/

  return dies_memb;

}

static void
calc_trees_fields_num(dies_info_t  *dies_info){

  GSList *list_entry;

  /* Calcs number of trees and fields, store result in dies_info */
  /* Number of struct is number of ALL roots + number of structs and arrays in pool */
  /* Number of fields is number of nonSTRUCT and nonARRAY members in pool */

  dies_info->trees_num = 0;
  dies_info->fields_num = 0;

  list_entry = g_slist_nth(dies_info->dies_pools_list, 0);
  while(list_entry){
    dies_pool_t *dies_pool;

    dies_pool = (dies_pool_t *)g_slist_nth_data(list_entry, 0);
    if (dies_pool->size > 0){
      calc_trees_fields_by_die(dies_info, dies_pool->dies);
    }

    list_entry = g_slist_next(list_entry);
  }

}

static char str_NONAME[] = "NONAME";

static dies_memb_t *
fillup_reg_info_by_die(hf_register_info *hf, gint **ett, dies_info_t *dies_info, dies_memb_t *dies_memb){

  char                *abbr_str;
  hf_register_info    *hf_curr;
  size_t              abbr_str_len, abbr_node_len;

  static hf_register_info hf_blank =
      { NULL,
        { NULL, NULL,
        FT_NONE, BASE_NONE,
        NULL, 0x0,
        NULL, HFILL }};

  abbr_node_len = strlen(dies_info->abbr);

  while(1){
    gchar *abbr_name;

    gint is_last = dies_memb->flag & DIES_MEMB_FLAG_IS_LAST;

    /* Use type name for root entries */
    abbr_name = strlen(dies_memb->field_name) ? dies_memb->field_name : dies_memb->type_name;
    /* Use NONAME for unnamed fields */
    if(0 == strlen(abbr_name)) {
        abbr_name = str_NONAME;
        fprintf(stderr, "%s: abbrev %s has no a name -> use \"NONAME\"\n", __FUNCTION__, dies_info->abbr);
    }

    /* Replace all spaces (' ') to underscores ('_') */
    {
        gchar *cp = abbr_name;

        for ( ; *cp; cp++) {
            if (*cp == ' ') {
                *cp  = '_';
            }
        }
    }

    /* Compose full abbrev */
    abbr_str_len = abbr_node_len + 1 + strlen(abbr_name) + 1; /* xxx.yyy/0 */
    abbr_str = (char *)g_malloc(abbr_str_len);
    g_snprintf(abbr_str, abbr_str_len, "%s.%s", dies_info->abbr, abbr_name);

    /* Init with blank template*/
    hf_curr = &hf[dies_info->hf_idx++];
    memcpy(hf_curr, &hf_blank, sizeof(*hf));

    hf_curr->p_id = &dies_memb->hf_id;
    hf_curr->hfinfo.name  = abbr_name;
    hf_curr->hfinfo.abbrev = abbr_str;

    hf_curr->hfinfo.type = dies_memb->format;
    hf_curr->hfinfo.display = dies_memb->display;

    if (dies_memb->bit_size > 0)
    {
      hf_curr->hfinfo.bitmask = BITMASK(dies_memb->bit_offset, dies_memb->bit_size);
    }

    /* Set default type and format for single ROOT entries (e.g. typedefs) */
    if (dies_memb->flag == DIES_MEMB_FLAG_IS_ROOT) {
        hf_curr->hfinfo.type    = FT_UINT32;
        hf_curr->hfinfo.display = BASE_DEC_HEX;
    }

    hf_curr->hfinfo.strings = dies_memb->strings;
    hf_curr->hfinfo.blurb = abbr_name;

    if (dies_memb->flag & DIES_MEMB_FLAG_IS_PARENT) {

      /* Overwrite format for well known structs types */
      if (0 == strcmp(dies_memb->type_name, "IEEE_ADDR")){
        hf_curr->hfinfo.type = FT_ETHER;
      }

      /* Structure or Array */
      ett[dies_info->ett_idx++] = &dies_memb->ett_id;

      /* Proceed with 1st member of next level */
      dies_memb++;

      strcpy(dies_info->abbr, abbr_str);                                   /* Prepare abbreviation for next level */
      dies_memb = fillup_reg_info_by_die(hf, ett, dies_info, dies_memb);   /* Recursive call */
      dies_info->abbr[abbr_node_len] = 0;                                  /* Restore abbreviation */
    }

    /* Proceed with next member on that level */
    if (is_last){
      break;
    }

    dies_memb++;

  } /* while(1)*/

  return dies_memb;
}

static void
fillup_reg_info(hf_register_info *hf, gint **ett, dies_info_t *dies_info){

  GSList *list_entry;

  dies_info->hf_idx = 0;
  dies_info->ett_idx = 0;

  strcpy(dies_info->abbr, "wl");

  list_entry = g_slist_nth(dies_info->dies_pools_list, 0);
  while(list_entry){
    dies_pool_t *dies_pool;

    dies_pool = (dies_pool_t*)g_slist_nth_data(list_entry, 0);
    if (dies_pool->size > 0){
      fillup_reg_info_by_die(hf, ett, dies_info, dies_pool->dies);
    }

    list_entry = g_slist_next(list_entry);
  }

  return;
}

static void
link_dies_with_scd(dies_info_t *dies_info, scd_info_t *scd_info){

  guint i, j;

  dies_memb_t   *dies_memb;
  dies_pool_t   *dies_pool;

  guint         dies_size;
  scd_evt_t     *scd_evt;
  int           last_oid;

  const char *oid_elf_fname;

  fprintf(stderr, ">>> %s\n", __FUNCTION__); fflush(stderr);

  dies_pool = NULL;
  last_oid = -1;  /* set to any not existing value */
  oid_elf_fname = NULL;

  scd_evt = scd_info->scd_evt_table;
  for (j = 0; j < scd_info->scd_evt_table_size; j++, scd_evt++){

    if (scd_evt->tag != SCD_EVT_TAG_SIG && scd_evt->tag != SCD_EVT_TAG_HWSIG)
      continue;

    scd_evt->user_data = NULL;

    /* check ELF filename if OID changed */
    if (last_oid != scd_evt->oid){

      GSList *list_entry;

      last_oid = scd_evt->oid;

      dies_size = 0;

      /* Get elf file name of new OID */
      oid_elf_fname = scd_get_oid_elfname(scd_info, scd_evt->oid);
      if (oid_elf_fname == NULL){
        fprintf(stderr, "%s: oid_elf_fname is not specified for OID %d => SKIPPED\n", __FUNCTION__, last_oid);
        fflush(stderr);
        continue;
      }

      /* Find dies pool within dies list with the same elf file name as OID has */
      list_entry = g_slist_nth(dies_info->dies_pools_list, 0);
      while(list_entry){
        dies_pool = (dies_pool_t*)g_slist_nth_data(list_entry, 0);

        if (0 == strcmp(dies_pool->elf_fname, oid_elf_fname)){
          break;
        }

        list_entry = g_slist_next(list_entry);
      } /* End of find proper dies pool by name */

    } /* End of OID changed */

    if (oid_elf_fname == NULL){
      continue;
    }

    dies_memb = dies_pool->dies;
    dies_size = dies_pool->size;
    for (i = 0; i < dies_size; i++, dies_memb++){

      if (!(dies_memb->flag & DIES_MEMB_FLAG_IS_ROOT)){
        continue;
      }

      if (0 == strcmp(scd_evt->name, dies_memb->type_name)){
        scd_evt->user_data = dies_memb;
        break;
      }

    } /* DIES for loop */

    if (NULL == scd_evt->user_data) {
        fprintf(stderr, "WARN: not linked (%s)\n", scd_evt->name);
    }
  } /* SCD for loop */

    fprintf(stderr, "<<< %s\n", __FUNCTION__); fflush(stderr);

  return;
}

void
proto_register_wavelog(void)
{
  proto_wavelog = proto_register_protocol(
      "Lantiq WAVE Log Viewer",   /* Name */
      "Wave Log",                 /* Short Name */
      "wl");                      /* Abbrev */

  /* Register packet processing function */
  register_dissector("wavelog", dissect_wavelog, proto_wavelog);

  /* Fixed Part */
  proto_register_field_array(proto_wavelog, log_event_hf, array_length(log_event_hf));
  proto_register_subtree_array(&ett_wavelog[0], array_length(ett_wavelog));

  g_scd_info = scd_init( SCD_HASH_FLAGS_TMSG   |
                       SCD_HASH_FLAGS_GID      |
                       SCD_HASH_FLAGS_TAG_OID  |
                       SCD_HASH_FLAGS_LID );

  if (g_scd_info == NULL)
    goto end_of_proto_register_wavelog;

  process_scd(scd_filename, g_scd_info);

  scd_process_finish(g_scd_info);

  scd_dwarf_parse(
    scd_filename,                 /* in - SCD file name */
    &g_dies_info.dies_pools_list);  /* out - DIES pool */

  link_dies_with_scd(&g_dies_info, g_scd_info);

#ifdef DBG_DEIS_DUMP
  {
  GSList *list_entry;
  FILE *fp_dbg;
  int   fp_cntr = 0;

#ifdef  DBG_DEIS_DUMP_STDERR
  fp_dbg = stderr;
#else
  fp_dbg = ws_fopen("drv_dies_pool.die", "w");
#endif

  list_entry = g_slist_nth(g_dies_info.dies_pools_list, 0);
  while(list_entry){
    dies_pool_t *dies_pool;

    fprintf(fp_dbg, "%s: parse list_entry %d\n", __FUNCTION__, fp_cntr++);

    dies_pool = (dies_pool_t*)g_slist_nth_data(list_entry, 0);
    if (dies_pool->size > 0){
      dies_memb_t *die;
      guint dies_size;
      guint i;

      die = dies_pool->dies;
      dies_size = dies_pool->size;

      for(i = 0; i < dies_size; i++){
        fprintf(fp_dbg, "%-4d  %-3d  %-30s  %-30s    fl:%-1d  o:%-5d  s:%-5d   bo:%-5d  bs:%-5d  f:%-5d  d:%d\n",
          die->self_id, die->memb_type, die->type_name, die->field_name,
        die->flag, die->offset, die->size, die->bit_offset, die->bit_size, die->format, die->display);
        die++;
      }
    } /* dies pool not empty*/

    fflush(fp_dbg);

    list_entry = g_slist_next(list_entry);
  } /* end of pool list while */

  fprintf(fp_dbg, "%s: %d list_entries are parsed\n", __FUNCTION__, fp_cntr);

#ifdef DBG_DEIS_DUMP_STDERR
  fflush(fp_dbg);   /* flush stderr */
#else
  fclose(fp_dbg);
#endif
  }
#endif

  /* Compose fields and trees arrays by DIES pool */
  calc_trees_fields_num(&g_dies_info);

  hf_dies  = (hf_register_info *)g_malloc_n(g_dies_info.fields_num, sizeof(hf_register_info));
  ett_dies = (gint**)g_malloc_n(g_dies_info.trees_num, sizeof(gint*));

  fillup_reg_info(hf_dies, ett_dies, &g_dies_info);

  /* Register fields and trees. DIES depended part */
  proto_register_field_array(proto_wavelog, hf_dies, g_dies_info.fields_num);

  proto_register_subtree_array(ett_dies, g_dies_info.trees_num);

end_of_proto_register_wavelog:

  return;
}

void
proto_reg_handoff_wavelog(void)
{

  static gboolean initialized = FALSE;
  static dissector_handle_t wavelog_handle;
  static dissector_handle_t wavelog_packet_handle;

  if (!initialized) {

/*  Use new_create_dissector_handle() to indicate that dissect_wavelog()
 *  returns the number of bytes it dissected (or 0 if it thinks the packet
 *  does not belong to PROTONAME).
 */
    wavelog_handle = create_dissector_handle(new_dissect_wavelog, proto_wavelog);
    initialized = TRUE;
    dissector_add_uint("wtap_encap", WTAP_ENCAP_USER1, wavelog_handle);

    /* for UDP packet too */
    wavelog_packet_handle = create_dissector_handle(new_dissect_wavelog_packet, proto_wavelog);
    dissector_add_uint("udp.port", 2009, wavelog_packet_handle);

    } else {

      /*
        If you perform registration functions which are dependent upon
        prefs the you should de-register everything which was associated
        with the previous settings and re-register using the new prefs
        settings here. In general this means you need to keep track of
        the PROTOABBREV_handle and the value the preference had at the time
        you registered.  The PROTOABBREV_handle value and the value of the
        preference can be saved using local statics in this
        function (proto_reg_handoff).
      */

    }
}
