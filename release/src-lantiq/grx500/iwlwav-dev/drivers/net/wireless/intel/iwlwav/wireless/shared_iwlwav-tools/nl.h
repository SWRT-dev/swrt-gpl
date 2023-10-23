/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

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

/* MTLK Protocol version */
#define MTLK_NL_PROTOCOL_VERSION  1

/* Command IDs for MTLK messages */
enum {
  NL_DRV_CMD_MAN_FRAME        = 4,
  NL_DRV_IRBM_NOTIFY          = 5,
  NL_DRV_CMD_RTLOG_NOTIFY     = 6,
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
#define MTLK_NETLINK_RTLOG_APPS_GROUP_NAME    "mtlk_rtlog_apps"

#define NETLINK_SIMPLE_CONFIG_GROUP     (0L << 0) /* the group that wsccmd application (Simple Config) joins */
#define NETLINK_IRBM_GROUP              (1L << 0) /* the group that IRB clients join */
#define NETLINK_LOGSERVER_GROUP         (2L << 0) /* the group that logserver joins */
#define NETLINK_RTLOG_APPS_GROUP        (3L << 0) /* the group that user's space applications join */

#define MTLK_FINGERPRINT_TEXT  "mtlk" /* printable */
#define MTLK_FINGERPRINT_SIZE  (sizeof(MTLK_FINGERPRINT_TEXT) - 1)

/* MTLK header */
struct mtlk_nl_msghdr {
  char   fingerprint[MTLK_FINGERPRINT_SIZE];
  uint8  proto_ver;      /* MTLK_NL_PROTOCOL_VERSION */
  uint8  cmd_id;         /* NL_DRV_CMD_* */
  uint16 data_len;       /* payload size, in bytes */
} __attribute__ ((aligned(1), packed));

#endif /* __NL_H__ */
