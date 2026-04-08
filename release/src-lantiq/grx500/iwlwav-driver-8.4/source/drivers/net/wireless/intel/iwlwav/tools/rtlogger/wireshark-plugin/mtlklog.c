/* mtlklog.c
 * Routines for mtlklog dissection
 * Copyright 2009, Nikita Izyumtsev <nikita.izyumtsev@gmail.com>
 *
 * $Id$
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * Copied from WHATEVER_FILE_YOU_USED (where "WHATEVER_FILE_YOU_USED"
 * is a dissector file; if you just copied this from README.developer,
 * don't bother with the "Copied from" - you don't even need to put
 * in a "Copied from" if you copied an existing dissector, especially
 * if the bulk of the code in the new dissector is your code)
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include <epan/packet.h>
#include <epan/prefs.h>

/* Initialize the protocol and fields identifiers */
static int proto_mtlklog      = -1;
static int hf_mtlklog_oid     = -1;
static int hf_mtlklog_gid     = -1;
static int hf_mtlklog_fid     = -1;
static int hf_mtlklog_lid     = -1;
static int hf_mtlklog_wlanif  = -1;
static int hf_mtlklog_src_len = -1;
static int hf_mtlklog_src     = -1;
static int hf_mtlklog_dst_len = -1;
static int hf_mtlklog_dst     = -1;
static int hf_mtlklog_msg_len = -1;
static int hf_mtlklog_msg     = -1;

/* Initialize the subtree pointers */
static gint ett_mtlklog = -1;

enum mtlklog_tree_leafs  {
  MTLKLOG_TL_OID,
  MTLKLOG_TL_GID,
  MTLKLOG_TL_FID,
  MTLKLOG_TL_LID,
  MTLKLOG_TL_WLANIF,
  MTLKLOG_TL_SRC,
  MTLKLOG_TL_DST,
  MTLKLOG_TL_MSG,
  MTLKLOG_TL_LAST
};

static int
dissect_mtlklog(tvbuff_t *tvb, packet_info *pinfo, proto_tree *tree)
{
  proto_tree  *mtlklog_tree = NULL;
  proto_item  *ti           = NULL;
  int          offset       = 0;
  guint8       oid;
  guint8       gid;
  guint16      fid;
  guint16      lid;
  guint8       wlanif;
  guint16      src_len = 0;
  const gchar *src     = NULL;
  guint16      dst_len = 0;
  const gchar *dst     = NULL;
  guint16      msg_len = 0;
  const gchar *msg;

  if (tree) {
    ti = proto_tree_add_item(tree, proto_mtlklog, tvb, 0, tvb_length(tvb), FALSE);
    mtlklog_tree = proto_item_add_subtree(ti, ett_mtlklog);
  }

  /* get OID if required */
  if (tree) {
    oid = tvb_get_guint8(tvb, offset);
    proto_tree_add_uint(mtlklog_tree, hf_mtlklog_oid, tvb, offset,
                        sizeof(oid), oid);
  }
  offset += sizeof(oid);

  /* get GID if required */
  if (tree) {
    gid = tvb_get_guint8(tvb, offset);
    proto_tree_add_uint(mtlklog_tree, hf_mtlklog_gid, tvb, offset,
                        sizeof(gid), gid);
  }
  offset += sizeof(gid);

  /* get FID if required */
  if (tree) {
    fid = tvb_get_ntohs(tvb, offset);
    proto_tree_add_uint(mtlklog_tree, hf_mtlklog_fid, tvb, offset,
                        sizeof(fid), fid);
  }
  offset += sizeof(fid);

  /* get LID if required */
  if (tree) {
    lid = tvb_get_ntohs(tvb, offset);
    proto_tree_add_uint(mtlklog_tree, hf_mtlklog_lid, tvb, offset,
                        sizeof(lid), lid);
  }
  offset += sizeof(lid);
  
  /* get WLANIF if required */
  if (tree) {
    wlanif = tvb_get_guint8(tvb, offset);
    proto_tree_add_uint(mtlklog_tree, hf_mtlklog_wlanif, tvb, offset,
                        sizeof(wlanif), wlanif);
  }
  offset += sizeof(wlanif);

  /* get SRC len */
  src_len = tvb_get_ntohs(tvb, offset);
  if (tree) {
    proto_tree_add_uint(mtlklog_tree, hf_mtlklog_src_len, tvb, offset,
                        sizeof(src_len), src_len);
  }
  offset += sizeof(src_len);

  /* get SRC string */        
  src = (gchar*)tvb_get_ptr(tvb, offset, src_len);
  if (tree) {
    proto_tree_add_string(mtlklog_tree, hf_mtlklog_src, tvb, offset, 
                          src_len, src);
  }
  offset += src_len;

  /* get DST len */
  dst_len = tvb_get_ntohs(tvb, offset);
  if (tree) {
    proto_tree_add_uint(mtlklog_tree, hf_mtlklog_dst_len, tvb, offset,
                        sizeof(dst_len), dst_len);
  }
  offset += sizeof(dst_len);

  /* get DST string */        
  dst = (gchar*)tvb_get_ptr(tvb, offset, dst_len);
  if (tree) {
    proto_tree_add_string(mtlklog_tree, hf_mtlklog_dst, tvb, offset, 
                          dst_len, dst);
  }
  offset += dst_len;

  /* get MSG len */
  msg_len = tvb_get_ntohs(tvb, offset);
  if (tree) {
    proto_tree_add_uint(mtlklog_tree, hf_mtlklog_msg_len, tvb, offset,
                        sizeof(msg_len), msg_len);
  }
  offset += sizeof(msg_len);

  /* get MSG string */        
  msg = (gchar*)tvb_get_ptr(tvb, offset, msg_len);
  if (tree) {
    proto_tree_add_string(mtlklog_tree, hf_mtlklog_msg, tvb, offset, 
                          msg_len, msg);
  }
  offset += msg_len;

  if (src_len) {
    SET_ADDRESS(&pinfo->src, AT_STRINGZ, src_len, src);
  }

  if (dst_len) {
    SET_ADDRESS(&pinfo->dst, AT_STRINGZ, dst_len, dst);
  }

  if (check_col(pinfo->cinfo, COL_PROTOCOL)) {
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "mtlklog");
  }

  if (check_col(pinfo->cinfo, COL_INFO)) {
    col_add_str(pinfo->cinfo, COL_INFO, msg);
  }

  return tvb_length(tvb);
}

void
proto_register_mtlklog(void)
{
  static hf_register_info hf[] = {
    { &hf_mtlklog_oid, 
      { "Originator ID", "mtlklog.oid",
        FT_UINT8, BASE_DEC, NULL, 0x0, "", HFILL
      }
    },
    { &hf_mtlklog_gid, 
      { "Group ID", "mtlklog.gid",
        FT_UINT8, BASE_DEC, NULL, 0x0, "", HFILL
      }
    },
    { &hf_mtlklog_fid, 
      { "File ID", "mtlklog.fid",
        FT_UINT16, BASE_DEC, NULL, 0x0, "", HFILL
      }
    },
    { &hf_mtlklog_lid, 
      { "Line number", "mtlklog.lid",
        FT_UINT16, BASE_DEC, NULL, 0x0, "", HFILL
      }
    },
    { &hf_mtlklog_wlanif, 
      { "Wlan interface", "mtlklog.wlanif",
        FT_UINT8, BASE_DEC, NULL, 0x0, "", HFILL
      }
    },
    { &hf_mtlklog_src_len, 
      { "Source length", "mtlklog.src_len",
        FT_UINT16, BASE_HEX, NULL, 0x0, "", HFILL
      }
    },
    { &hf_mtlklog_src, 
      { "Source", "mtlklog.src",
        FT_STRING, BASE_NONE, NULL, 0x0, "", HFILL
      }
    },
    { &hf_mtlklog_dst_len, 
      { "Destination length", "mtlklog.dst_len",
        FT_UINT16, BASE_HEX, NULL, 0x0, "", HFILL
      }
    },
    { &hf_mtlklog_dst, 
      { "Destination", "mtlklog.dst",
        FT_STRING, BASE_NONE, NULL, 0x0, "", HFILL
      }
    },
    { &hf_mtlklog_msg_len, 
      { "Message length", "mtlklog.msg_len",
        FT_UINT16, BASE_HEX, NULL, 0x0, "", HFILL
      }
    },
    { &hf_mtlklog_msg, 
      { "Message", "mtlklog.msg",
        FT_STRING, BASE_NONE, NULL, 0x0, "", HFILL
      }
    }
  };

  static gint *ett[] = {
    &ett_mtlklog
  };

  proto_mtlklog = proto_register_protocol("Metalink Logger Protocol", "mtlklog", "mtlklog");
  register_dissector("mtlklog", dissect_mtlklog, proto_mtlklog);
  proto_register_field_array(proto_mtlklog, hf, array_length(hf));
  proto_register_subtree_array(ett, array_length(ett));
}

void
proto_reg_handoff_mtlklog(void)
{
  dissector_handle_t mtlklog_handle;
  mtlklog_handle = new_create_dissector_handle(dissect_mtlklog, proto_mtlklog);
  dissector_add("wtap_encap", WTAP_ENCAP_USER0, mtlklog_handle);
}

