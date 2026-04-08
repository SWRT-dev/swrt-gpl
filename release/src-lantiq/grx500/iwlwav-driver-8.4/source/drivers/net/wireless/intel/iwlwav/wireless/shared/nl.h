/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __NL_H__
#define __NL_H__

#include <linux/version.h>

#define MTLK_GENL_FAMILY_NAME     "MTLK_WLS"
#define MTLK_GENL_FAMILY_VERSION  1

/* attributes of the family */
enum {
        MTLK_GENL_ATTR_UNSPEC,
        MTLK_GENL_ATTR_EVENT,
        __MTLK_GENL_ATTR_MAX,
};
#define MTLK_GENL_ATTR_MAX (__MTLK_GENL_ATTR_MAX -1)

/* supported commands, genl commands, not mtlk commands (arp, wd, rmmod) */
enum {
        MTLK_GENL_CMD_UNSPEC,
        MTLK_GENL_CMD_EVENT,
        __MTLK_GENL_CMD_MAX,
};
#define MTLK_GENL_CMD_MAX (__MTLK_GENL_CMD_MAX -1)

/* Command IDs for MTLK messages */
enum {
  NL_DRV_CMD_MAN_FRAME = 4,
  NL_DRV_IRBM_NOTIFY,
  NL_DRV_CMD_RESERVED_UNUSED,
  NL_DRV_CMD_FAPI_NOTIFY,
  NL_DRV_CMD_RTLOG_NOTIFY,
  NL_DRV_CMD_COUNT
};

#define WAVE_NL_DRV_CMD_COUNT NL_DRV_CMD_COUNT

#define NETLINK_MESSAGE_GROUP           (0L << 0)/* the group that drvhlpr, hostapd and wpa_supplicant join */
/* TODO: change NETLINK_SIMPLE_CONFIG_GROUP to group that WPS application will listen
 * it should not match any other active group
 */

#define MTLK_NETLINK_SIMPLE_CONFIG_GROUP_NAME "mtlk_config"
#define MTLK_NETLINK_IRBM_GROUP_NAME          "mtlk_irb"
#define MTLK_NETLINK_LOGSERVER_GROUP_NAME     "mtlk_logserver"
#define WAVE_NETLINK_FAPI_GROUP_NAME          "wave_fapi"
#define MTLK_NETLINK_RTLOG_APPS_GROUP_NAME    "mtlk_rtlog_apps"

#define NETLINK_SIMPLE_CONFIG_GROUP     (0L << 0) /* the group that wsccmd application (Simple Config) joins */
#define NETLINK_IRBM_GROUP              (1L << 0) /* the group that IRB clients join */
#define NETLINK_LOGSERVER_GROUP         (2L << 0) /* the group that logserver joins */
#define NETLINK_FAPI_GROUP              (3L << 0) /* the group that FAPI joins */
#define NETLINK_RTLOG_APPS_GROUP        (4L << 0) /* the group that user's space applications join */

/* MTLK Protocol version */
#define MTLK_NL_PROTOCOL_VERSION  1

/* MTLK header */
struct mtlk_nl_msghdr {
  char   fingerprint[4]; /* "mtlk" */
  uint8  proto_ver;      /* MTLK_NL_PROTOCOL_VERSION */
  uint8  cmd_id;         /* NL_DRV_CMD_* */
  uint16 data_len;       /* MTLK payload size, in bytes */
} __attribute__ ((aligned(1), packed));

#endif /* __NL_H__ */
