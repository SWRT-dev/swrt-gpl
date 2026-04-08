#ifndef GENETLINK_PRIV_H
#define GENETLINK_PRIV_H

#include <linux/version.h>


/* Family name and version */
#define MTLK_GENL_FAMILY_NAME		"MTLK_WLS"
#define MTLK_GENL_FAMILY_VERSION	1


/* Attributes of the family */
enum {
	MTLK_GENL_ATTR_UNSPEC,
	MTLK_GENL_ATTR_EVENT,
	__MTLK_GENL_ATTR_MAX
};
#define MTLK_GENL_ATTR_MAX	(__MTLK_GENL_ATTR_MAX - 1)


/* Supported commands */
enum {
	MTLK_GENL_CMD_UNSPEC,
	MTLK_GENL_CMD_EVENT,
	__MTLK_GENL_CMD_MAX
};
#define MTLK_GENL_CMD_MAX	(__MTLK_GENL_CMD_MAX - 1)


/* Protocol version */
#define MTLK_NL_PROTOCOL_VERSION	1
/* Command ID */
#define NL_DRV_CMD_RTLOG_NOTIFY		8


/* Group */
/*in-order to match the backported driver - we need to fit the newer kernel defs*/
#if 0 &&LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0)
#define NETLINK_RTLOG_APPS_GROUP			(1L << 3)
#else
#define MTLK_NETLINK_RTLOG_APPS_GROUP_NAME	"mtlk_rtlog_apps"
#define NETLINK_RTLOG_APPS_GROUP			(4L << 0)
#endif


/* Message header */
struct mtlk_nl_msghdr {
	char fingerprint[4];
	u8 proto_ver;
	u8 cmd_id;
	u16 data_len;
} __attribute__ ((aligned(1), packed));

#endif /* GENETLINK_PRIV_H */
