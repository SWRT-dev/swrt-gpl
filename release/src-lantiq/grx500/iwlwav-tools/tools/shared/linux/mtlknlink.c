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

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <linux/if.h>

#include "mtlkinc.h"
#include "mtlknlink.h"

#define LOG_LOCAL_GID   GID_MTLKNLINK
#define LOG_LOCAL_FID   1

enum _RECEIVE_EVENT
{
  EVENT_DATA_ARRIVED = 1,
  EVENT_STOP_ARRIVED,
  EVENT_SIGNAL_ARRIVED,
};

#define MAX_PAYLOAD 1024  /* maximum payload size*/

#define FINGERPRINT_TEXT    "mtlk"
#define FINGERPRINT_SIZE    sizeof(FINGERPRINT_TEXT)-1

static int
nlink_wait_events(int socket_fd, int stop_fd)
{
  fd_set read_fds;
  int res = 0;

  FD_ZERO(&read_fds);
  FD_SET(stop_fd, &read_fds);
  FD_SET(socket_fd, &read_fds);

  res = select(MAX(socket_fd, stop_fd) + 1, &read_fds, NULL, NULL, NULL);
  if (-1 == res)
  {
    if (EINTR == errno)
      return EVENT_SIGNAL_ARRIVED;

    ELOG_SD("select call failed with error: %s (%d)", strerror(errno), errno);
    return -errno; /* select failed */
  }
  else if (FD_ISSET(stop_fd, &read_fds))
  {
    return EVENT_STOP_ARRIVED; /* stop signal received */
  }
  else if (FD_ISSET(socket_fd, &read_fds))
  {
    return EVENT_DATA_ARRIVED; /* socket contains data */
  }
  WLOG_V("select returned without a reason");

  return -1;
}

static int
nlink_parse_msg(mtlk_nlink_socket_t* nlink_socket, struct mtlk_nl_msghdr *phdr)
{
  /* Silently ignore packets from other applications */
  if (memcmp(phdr->fingerprint, MTLK_FINGERPRINT_TEXT, MTLK_FINGERPRINT_SIZE)) {
    return 0;
  }

  /* Silently ignore non-IRBM/non-RTLOG packets */
  if ((phdr->cmd_id != NL_DRV_IRBM_NOTIFY) &&
      (phdr->cmd_id != NL_DRV_CMD_RTLOG_NOTIFY)) {
    return 0;
  }

  if (phdr->proto_ver != MTLK_NL_PROTOCOL_VERSION) {
    ELOG_DD("Version mismatch: got %u, expected %u",
      phdr->proto_ver, MTLK_NL_PROTOCOL_VERSION);
    return 0;
  }

  nlink_socket->receive_callback(nlink_socket->receive_callback_ctx, (void *) (phdr + 1));
  return 0;
}

int wave_nlink_send_brd_msg (mtlk_nlink_socket_t *nlink_socket, const void *data, uint16 data_len, uint8 cmd)
{
  struct nl_msg *msg;
  struct nlattr *attr;
  struct mtlk_nl_msghdr *phdr;
  int res = -1;

  msg = nlmsg_alloc();
  if (NULL == msg)
    return res;

  if (NULL == genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, nlink_socket->family, 0, 0,
          MTLK_GENL_CMD_EVENT, MTLK_GENL_FAMILY_VERSION))
    goto end;

  attr = nla_reserve(msg, MTLK_GENL_ATTR_EVENT, data_len + sizeof(*phdr));
  if (NULL == attr)
    goto end;

  phdr = (struct mtlk_nl_msghdr*)nla_data(attr);
  wave_memcpy(phdr->fingerprint, sizeof(phdr->fingerprint), FINGERPRINT_TEXT, FINGERPRINT_SIZE);
  phdr->proto_ver = MTLK_NL_PROTOCOL_VERSION;
  phdr->cmd_id = cmd;
  phdr->data_len = data_len;
  wave_memcpy((char *)(phdr + 1), data_len, data, data_len);

  res = nl_send_auto(nlink_socket->sock, msg);

end:
  nlmsg_free(msg);
  return res;
}

/* this function is registered as a callback for
* all "valid" messages received on the socket.
* it parses the generic netlink message and calls
* local parser.
*/
static int parse_nl_cb(struct nl_msg *msg, void *arg)
{
  static struct nla_policy mtlk_genl_policy[MTLK_GENL_ATTR_MAX + 1] =
  {
    [MTLK_GENL_ATTR_EVENT] = { .type = NLA_UNSPEC,
                               .minlen = sizeof(struct mtlk_nl_msghdr),
                               .maxlen = 0 },
  };

  struct nlmsghdr *nlh = nlmsg_hdr(msg);
  struct nlattr *attrs[MTLK_GENL_ATTR_MAX+1];
  mtlk_nlink_socket_t* nlink_socket = (mtlk_nlink_socket_t*) arg;

  /* if not our family - do nothing */
  if (nlh->nlmsg_type != nlink_socket->family)
    return NL_SKIP;

  /* Validate message and parse attributes */
  if (genlmsg_parse(nlh, 0, attrs, MTLK_GENL_ATTR_MAX, mtlk_genl_policy) < 0)
    return NL_SKIP;

  /* Call the mtlk message parsing function */
  if (attrs[MTLK_GENL_ATTR_EVENT])
    nlink_parse_msg(nlink_socket,
      (struct mtlk_nl_msghdr *) nla_data(attrs[MTLK_GENL_ATTR_EVENT]));

  return NL_OK;
}

int __MTLK_IFUNC
mtlk_nlink_create(mtlk_nlink_socket_t* nlink_socket, const char* group_name,
                  mtlk_netlink_callback_t receive_callback, void* receive_callback_ctx)
{
  int res = 0;
  int broadcast_group;

  MTLK_ASSERT(NULL != nlink_socket);

  /* Register callback */
  nlink_socket->receive_callback = receive_callback;
  nlink_socket->receive_callback_ctx = receive_callback_ctx;

  /* Allocate a new netlink socket */
  nlink_socket->sock = nl_handle_alloc();
  if (NULL == nlink_socket->sock) {
    ELOG_V("Generic netlink socket allocation failed");
    res = -1;
    goto end;
  }

  /* Connect to generic netlink socket on kernel side */
  if (genl_connect(nlink_socket->sock) < 0) {
    ELOG_V("Connect to generic netlink controller failed");
    res = -1;
    goto err_dealloc;
  }

  /* Ask kernel to resolve family name to family id */
  nlink_socket->family = genl_ctrl_resolve(nlink_socket->sock, MTLK_GENL_FAMILY_NAME);
  if (nlink_socket->family < 0) {
    ELOG_V("Cannot get Generic Netlink family identifier.");
    res = -1;
    goto err_dealloc;
  }

  /* Get broadcast group id by family and group name */
  broadcast_group = genl_ctrl_resolve_grp(nlink_socket->sock, MTLK_GENL_FAMILY_NAME, group_name);
  if (broadcast_group < 0) {
    ELOG_V("Cannot get Generic Netlink Broadcast droup ID.");
    res = -1;
    goto err_dealloc;
  }

  /* register to receive messages from interested group */
  if (nl_socket_add_membership(nlink_socket->sock, broadcast_group) < 0) {
    ELOG_D("Cannot add membership in %d group.", broadcast_group);
    res = -1;
    goto err_dealloc;
  }

  /* This socket have to process all messages and not
     only explicitly requested as it is should be in
     event processing */
  nl_disable_sequence_check(nlink_socket->sock);

  /* Allow large data reception  */
  nl_socket_enable_msg_peek(nlink_socket->sock);

  /* set callback for all valid messages */
  nl_socket_modify_cb(nlink_socket->sock, NL_CB_VALID,
    NL_CB_CUSTOM, parse_nl_cb, nlink_socket);

  goto end;

err_dealloc:
  nl_handle_destroy(nlink_socket->sock);
end:
  return res;
}

int __MTLK_IFUNC
mtlk_nlink_receive_loop(mtlk_nlink_socket_t* nlink_socket, int stop_fd)
{
  int res = 0;

  MTLK_ASSERT(NULL != nlink_socket);

  /* Read message from kernel */
  for (;;) {
    int wait_results = nlink_wait_events(nl_socket_get_fd(nlink_socket->sock), stop_fd);

    switch (wait_results)
    {
      case EVENT_DATA_ARRIVED:
        res = nl_recvmsgs_default(nlink_socket->sock);
        if (res < 0)
          return res;
        break;
      case EVENT_SIGNAL_ARRIVED:
        continue;
      case EVENT_STOP_ARRIVED:
        return 0;
      default:
        ELOG_D("Wait for data failed (%d)", wait_results);
        return wait_results;
    }
  }

  MTLK_ASSERT(!"Should never be here");
}

void __MTLK_IFUNC
mtlk_nlink_cleanup(mtlk_nlink_socket_t* nlink_socket)
{
  MTLK_ASSERT(NULL != nlink_socket);
  nl_close(nlink_socket->sock);
  nl_handle_destroy(nlink_socket->sock);
}
