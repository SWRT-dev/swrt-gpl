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
 * $Id$
 *
 *
 *
 * Subsystem providing communication with userspace over
 * NETLINK_USERSOCK netlink protocol.
 *
 */
#include "mtlkinc.h"

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <linux/netlink.h>

#include <net/genetlink.h>
#include "nlmsgs.h"


#define LOG_LOCAL_GID   GID_NLMSGS
#define LOG_LOCAL_FID   1

/* Configuration structure */
static wave_nl_socket_t wave_nl_socket;


/* Module parameter */
extern int mtlk_genl_family_id;
static int _wave_nl_parse_brd_msg_cb (struct sk_buff *skb2, struct genl_info *info);

static struct genl_multicast_group mtlk_mcgrps[] = {
  [NETLINK_SIMPLE_CONFIG_GROUP] = { .name = MTLK_NETLINK_SIMPLE_CONFIG_GROUP_NAME, },
  [NETLINK_IRBM_GROUP] = { .name = MTLK_NETLINK_IRBM_GROUP_NAME, },
  [NETLINK_LOGSERVER_GROUP] = { .name = MTLK_NETLINK_LOGSERVER_GROUP_NAME, },
  [NETLINK_RTLOG_APPS_GROUP] = { .name = MTLK_NETLINK_RTLOG_APPS_GROUP_NAME, },
};

/* command structure */
static struct genl_ops wave_genl_ops[] = {
  {
    .cmd = MTLK_GENL_CMD_EVENT,
    .flags = 0,
    .doit = _wave_nl_parse_brd_msg_cb,
    .dumpit = NULL,
  }
};

/* family structure */
static struct genl_family mtlk_genl_family = {
        .name = MTLK_GENL_FAMILY_NAME,
        .version = MTLK_GENL_FAMILY_VERSION,
        .maxattr = MTLK_GENL_ATTR_MAX,
        .mcgrps = mtlk_mcgrps,
        .n_mcgrps = ARRAY_SIZE(mtlk_mcgrps),
        .ops = wave_genl_ops,
        .n_ops = ARRAY_SIZE(wave_genl_ops),
};

BOOL wave_nl_is_active (void)
{
  return wave_nl_socket.is_active;
}

mtlk_error_t mtlk_nl_send_brd_msg (void *data, int length, gfp_t flags, u32 dst_group, u8 cmd)
{
  struct sk_buff *skb;
  struct nlattr *attr;
  void *msg_header;
  int size, genl_res, send_group;
  int res = MTLK_ERR_UNKNOWN;
  struct mtlk_nl_msghdr *mhdr;
  int full_len = length + sizeof(*mhdr);

  MTLK_ASSERT(data);

  if(!data || cmd >= WAVE_NL_DRV_CMD_COUNT)
    return MTLK_ERR_PARAMS;


  /* allocate memory */
  size = nla_total_size(full_len);

  skb = genlmsg_new(size, flags);
  if (!skb)
    return MTLK_ERR_NO_MEM;

  /* add the genetlink message header */
  msg_header = genlmsg_put(skb, 0, 0, &mtlk_genl_family, 0, MTLK_GENL_CMD_EVENT);
  if (!msg_header)
    goto out_free_skb;

  /* fill the data */
  attr = nla_reserve(skb, MTLK_GENL_ATTR_EVENT, full_len);
  if (!attr)
    goto out_free_skb;

  mhdr = (struct mtlk_nl_msghdr*) (nla_data(attr));
  wave_memcpy(mhdr->fingerprint, sizeof(mhdr->fingerprint), MTLK_FINGERPRINT_TEXT, MTLK_FINGERPRINT_SIZE);
  mhdr->proto_ver = MTLK_NL_PROTOCOL_VERSION;
  mhdr->cmd_id = cmd;
  mhdr->data_len = length;

  wave_memcpy((char *)mhdr + sizeof(*mhdr), length, data, length);

  /* send multicast genetlink message */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
  genl_res = genlmsg_end(skb, msg_header);
  if (genl_res < 0)
    goto out_free_skb;
#else
  genlmsg_end(skb, msg_header);
#endif

  send_group = dst_group;
  genl_res = genlmsg_multicast(&mtlk_genl_family, skb, 0, send_group, flags);

  if (genl_res == -ESRCH) {
    /* We don't care if no one is listening  */
    ILOG1_DD("No listeners available : Error %d sending netlink message (msgid=%u)", genl_res, mhdr->cmd_id);
    return MTLK_ERR_OK;
  } else if (genl_res) {
    mtlk_osal_emergency_print("Error %d sending netlink message (msgid=%u)", genl_res, mhdr->cmd_id);
    return MTLK_ERR_UNKNOWN;
  } else
    return MTLK_ERR_OK;

out_free_skb:
  nlmsg_free(skb);
  return res;
}

static BOOL
_wave_nl_cmd_is_valid (u8 cmd)
{
  BOOL res = (cmd < ARRAY_SIZE(wave_nl_socket.receive_callback));
  if (!res) {
    mtlk_osal_emergency_print("NL Command ID (%u) too big", cmd);
    MTLK_ASSERT(FALSE);
  }
  return res;
}

mtlk_error_t wave_nl_update_receive_callback (wave_nl_callback_t rx_clb_fn, void *rx_clb_fn_params, u8 cmd)
{
  wave_nl_socket_t *nl_socket = &wave_nl_socket;

  if(cmd >= WAVE_NL_DRV_CMD_COUNT)
    return MTLK_ERR_PARAMS;

  if (!_wave_nl_cmd_is_valid(cmd))
    return MTLK_ERR_PARAMS;

  if (!nl_socket->is_active)
    return MTLK_ERR_UNKNOWN;

  if (rx_clb_fn && nl_socket->receive_callback[cmd])
    return MTLK_ERR_ALREADY_EXISTS;

  nl_socket->receive_callback_ctx[cmd] = rx_clb_fn_params;
  nl_socket->receive_callback[cmd] = rx_clb_fn;
  return MTLK_ERR_OK;
}

static void _wave_nl_parse_msg (struct mtlk_nl_msghdr *hdr)
{
  wave_nl_socket_t *nl_socket = &wave_nl_socket;

  /* Ignore packets from unknown applications */
  if (memcmp(hdr->fingerprint, MTLK_FINGERPRINT_TEXT, MTLK_FINGERPRINT_SIZE)) {
    return;
  }

  if (hdr->cmd_id != NL_DRV_CMD_RTLOG_NOTIFY)
  {
    return;
  }

  /* Check version of protocol */
  if (hdr->proto_ver != MTLK_NL_PROTOCOL_VERSION) {
    ELOG_DD("Wrong MTLK Protocol version for Netlink socket: got %d, expected %d.",
            hdr->proto_ver, MTLK_NL_PROTOCOL_VERSION);
    return;
  }

  /* Call RX-callback function if any */
  if (nl_socket->receive_callback[hdr->cmd_id]) {
    nl_socket->receive_callback[hdr->cmd_id](nl_socket->receive_callback_ctx[hdr->cmd_id], (void *)(hdr+1));
  }
  else {
    ELOG_V("Fail to parse Notification message from application: RX-callback function not found.");
  }
}

static int _wave_nl_parse_brd_msg_cb (struct sk_buff *skb2, struct genl_info *info)
{
  struct nlattr *attr;
  void          *data;
  size_t         data_len;

  if (info == NULL)
    return 0;

  attr = info->attrs[MTLK_GENL_ATTR_EVENT];
  if (attr) {
    data = nla_data(attr);
    if (NULL == data) {
      ELOG_V("Fail to receive Notification message from application: payload is absent.");
    }
    else {
      data_len = nla_len(attr);
      if (data_len < sizeof(struct mtlk_nl_msghdr)) {
        ELOG_V("Fail to receive Notification message from application: payload len too small.");
      } else {
        _wave_nl_parse_msg((struct mtlk_nl_msghdr *)data);
      }
    }
  }
  else {
    ELOG_D("Fail to receive Notification message from application: "
           "no genl_info.attrs[%d].", MTLK_GENL_ATTR_EVENT);
  }

  return 0;
}

mtlk_error_t mtlk_nl_init (wave_nl_callback_t rx_clb_fn, void *rx_clb_fn_params, u8 cmd)
{
  int result;
  wave_nl_socket_t *nl_socket = &wave_nl_socket;

  memset(nl_socket, 0, sizeof(*nl_socket));

  if (!_wave_nl_cmd_is_valid(cmd))
    return MTLK_ERR_PARAMS;

  if (rx_clb_fn) {
    nl_socket->receive_callback[cmd] = rx_clb_fn;
    nl_socket->receive_callback_ctx[cmd] = rx_clb_fn_params;
  }

  if(mtlk_genl_family_id) {
    mtlk_genl_family.id = mtlk_genl_family_id;
  }

  result = genl_register_family(&mtlk_genl_family);
  if (result) {
    mtlk_osal_emergency_print("Failed to register Netlink family: res=%d", result);
    return MTLK_ERR_UNKNOWN;
  }

  /* Pass actual value back to module param
   * to be readable via sysfs and iwpriv */
  mtlk_genl_family_id = mtlk_genl_family.id;

  nl_socket->is_active = TRUE;
  return MTLK_ERR_OK;

  genl_unregister_family(&mtlk_genl_family);
  memset(nl_socket, 0, sizeof(*nl_socket));
  return MTLK_ERR_UNKNOWN;
}

void mtlk_nl_cleanup (void)
{
  genl_unregister_family(&mtlk_genl_family);
  memset(&wave_nl_socket, 0, sizeof(wave_nl_socket));

  mtlk_osal_emergency_print("Netlink module cleanup done");
}
EXPORT_SYMBOL(wave_nl_is_active);
EXPORT_SYMBOL(wave_nl_update_receive_callback);
EXPORT_SYMBOL(mtlk_nl_send_brd_msg);
