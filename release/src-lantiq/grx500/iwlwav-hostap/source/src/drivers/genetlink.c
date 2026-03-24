#ifndef CONFIG_NO_WPA_RTLOGGER

#include "includes.h"
#include "common.h"
#include "eloop.h"

#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include "genetlink.h"
#include "genetlink_priv.h"
#include "wpa_rtlogger.h"


struct genetlink_data {
	struct genetlink_config *cfg;
	struct nl_sock *sock;
	int sock_fd;
	int family;
};


static struct nla_policy rxmsg_policy[MTLK_GENL_ATTR_MAX + 1] =
{
	[MTLK_GENL_ATTR_EVENT] = { .type = NLA_UNSPEC,
							   .minlen = sizeof(struct mtlk_nl_msghdr),
							   .maxlen = 0 }
};


static void parse_rxmsg(struct genetlink_data *genetlink, struct mtlk_nl_msghdr *hdr)
{
	if (os_memcmp(hdr->fingerprint, MTLK_FINGERPRINT_TEXT, MTLK_FINGERPRINT_SIZE)) {
		wpa_printf(MSG_ERROR, "genetlink: wrong fingerprint (%s) in "
			   "incoming message's header, expected (%s)", hdr->fingerprint,
			   MTLK_FINGERPRINT_TEXT);
		return;
	}
	if (hdr->cmd_id != genetlink->cfg->genetlink_cmd) {
		wpa_printf(MSG_ERROR, "genetlink: wrong command's id (0x%02X) in "
			   "incoming message's header, expected (0x%02X)", hdr->cmd_id,
			   genetlink->cfg->genetlink_cmd);
		return;
	}
	if (hdr->proto_ver != MTLK_NL_PROTOCOL_VERSION) {
		wpa_printf(MSG_ERROR, "genetlink: wrong protocol's version (%d) in "
			   "incoming message's header, expected (%d)", hdr->proto_ver,
			   MTLK_NL_PROTOCOL_VERSION);
		return;
	}
	if (genetlink->cfg->rxmsg_cb == NULL) {
		wpa_printf(MSG_ERROR, "genetlink: user's callback function not found");
		return;
	}
	genetlink->cfg->rxmsg_cb(genetlink->cfg->ctx, (void *)(hdr + 1));
}


static int genetlink_parse_rxmsg_cb(struct nl_msg *msg, void *arg)
{
	struct nlmsghdr *hdr = nlmsg_hdr(msg);
	struct nlattr *attr[MTLK_GENL_ATTR_MAX + 1];
	struct genetlink_data *genetlink = arg;

	if (hdr->nlmsg_type != genetlink->family) {
		wpa_printf(MSG_ERROR, "genetlink: Invalid family ID for incoming message");
		return NL_SKIP;
	}
	if (genlmsg_parse(hdr, 0, attr, MTLK_GENL_ATTR_MAX, rxmsg_policy) < 0) {
		wpa_printf(MSG_ERROR, "genetlink: Failed to parse incoming message");
		return NL_SKIP;
	}

	if (attr[MTLK_GENL_ATTR_EVENT])
		parse_rxmsg(genetlink, (struct mtlk_nl_msghdr *)nla_data(attr[MTLK_GENL_ATTR_EVENT]));
	else
		wpa_printf(MSG_ERROR, "genetlink: Failed to find correct attribute in incoming message");

	return NL_OK;
}


static void genetlink_receive(int sock_fd, void *eloop_ctx, void *sock_ctx)
{
	struct genetlink_data *genetlink = eloop_ctx;

	if (nl_recvmsgs_default(genetlink->sock) < 0) {
		wpa_printf(MSG_ERROR, "genetlink: Failed to receive message: %s",
			   strerror(errno));
	}
}


struct genetlink_data * genetlink_init(struct genetlink_config *cfg)
{
	int genetlink_bcast_group;
	struct genetlink_data *genetlink;

	genetlink = os_zalloc(sizeof(*genetlink));
	if (genetlink == NULL)
		return NULL;

	genetlink->cfg = cfg;
	genetlink->sock = nl_socket_alloc();
	if (genetlink->sock == NULL) {
		wpa_printf(MSG_ERROR, "genetlink: Failed to open generic netlink "
			   "socket: %s", strerror(errno));
		genetlink_deinit(genetlink);
		return NULL;
	}

	nl_socket_disable_seq_check(genetlink->sock);
	nl_socket_disable_auto_ack(genetlink->sock);
	nl_socket_enable_msg_peek(genetlink->sock);
	nl_socket_modify_cb(genetlink->sock, NL_CB_VALID, NL_CB_CUSTOM,
		   genetlink_parse_rxmsg_cb, genetlink);

	if (genl_connect(genetlink->sock) < 0) {
		wpa_printf(MSG_ERROR, "genetlink: Failed to connect to generic netlink "
			   "controller: %s", strerror(errno));
		genetlink_deinit(genetlink);
		return NULL;
	}
	genetlink->sock_fd = nl_socket_get_fd(genetlink->sock);
	if (genetlink->sock_fd < 0) {
		wpa_printf(MSG_ERROR, "genetlink: Failed to get generic netlink file "
			   "descriptor: %s", strerror(errno));
		genetlink_deinit(genetlink);
		return NULL;
	}
	genetlink->family = genl_ctrl_resolve(genetlink->sock, MTLK_GENL_FAMILY_NAME);
	if (genetlink->family < 0) {
		wpa_printf(MSG_ERROR, "genetlink: Failed to get generic netlink family "
			   "identifier: %s", strerror(errno));
		genetlink_deinit(genetlink);
		return NULL;
	}
/*in-order to match the backported driver - we need to fit the newer kernel defs*/
#if 0 && LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0)
	genetlink_bcast_group = genetlink->family + cfg->genetlink_group - 1;
#else
	genetlink_bcast_group = genl_ctrl_resolve_grp(genetlink->sock,
		   MTLK_GENL_FAMILY_NAME, MTLK_NETLINK_RTLOG_APPS_GROUP_NAME);
	if (genetlink_bcast_group < 0) {
		wpa_printf(MSG_ERROR, "genetlink: Failed to get generic netlink "
			   "broadcast group identifier: %s", strerror(errno));
		genetlink_deinit(genetlink);
		return NULL;
	}
#endif
	if (nl_socket_add_membership(genetlink->sock, genetlink_bcast_group) < 0) {
		wpa_printf(MSG_ERROR, "genetlink: Failed to add membership to generic netlink "
			   "socket group %d: %s", genetlink_bcast_group, strerror(errno));
		genetlink_deinit(genetlink);
		return NULL;
	}

	if (eloop_register_read_sock(genetlink->sock_fd,
			   genetlink_receive, genetlink, NULL) < 0) {
		wpa_printf(MSG_ERROR, "genetlink: Failed to register read socket "
			   "in eloop: %d", -1);
		genetlink_deinit(genetlink);
		return NULL;
	}
	wpa_printf(/*MSG_INFO*/MSG_DEBUG, "genetlink: Initialization done.");

	return genetlink;
}


void genetlink_deinit(struct genetlink_data *genetlink)
{
	if (genetlink == NULL)
		return;
	if (genetlink->sock != NULL) {
		eloop_unregister_read_sock(genetlink->sock_fd);
		nl_close(genetlink->sock);
		nl_socket_free(genetlink->sock);
	}
	if (!genetlink->cfg->is_static)
		os_free(genetlink->cfg);
	os_free(genetlink);

	wpa_printf(/*MSG_INFO*/MSG_DEBUG, "genetlink: Cleanup done");
}


int genetlink_send_msg(struct genetlink_data *genetlink, const void *data,
			   const int data_len)
{
	/* Note! Console log is allowed in this function only */
	struct nl_msg *msg;
	struct nlattr *attr;
	void *msg_hdr;
	struct mtlk_nl_msghdr *hdr;
	int res = -1;
	errno_t err;

	if ((genetlink == NULL) || (data == NULL) || (data_len == 0)) {
#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
		wpa_printf(MSG_ERROR, "genetlink: Failed to send message: "
			   "data buffer is empty");
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
		if (wpa_rtlogger_get_flags(MSG_ERROR) & WPA_RTLOGGER_LOG_TARGET_CONSOLE)
			CERROR("genetlink: Failed to send message: data buffer is empty");
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */
		return res;
	}

	msg = nlmsg_alloc();
	if (msg == NULL)
		return res;

	msg_hdr = genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, genetlink->family,
			    0, 0, MTLK_GENL_CMD_EVENT, MTLK_GENL_FAMILY_VERSION);
	if (msg_hdr == NULL)
		goto out;

	attr = nla_reserve(msg, MTLK_GENL_ATTR_EVENT, data_len + sizeof(*hdr));
	if (attr == NULL)
		goto out;

	hdr = (struct mtlk_nl_msghdr *)nla_data(attr);
	err = memcpy_s(hdr->fingerprint, sizeof(hdr->fingerprint), MTLK_FINGERPRINT_TEXT, MTLK_FINGERPRINT_SIZE);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		res = -1;
		goto out;
	}
	hdr->proto_ver = MTLK_NL_PROTOCOL_VERSION;
	hdr->cmd_id = genetlink->cfg->genetlink_cmd;
	hdr->data_len = data_len;
	err = memcpy_s((char *)hdr + sizeof(*hdr), data_len, data, data_len);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		res = -1;
		goto out;
	}

#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
	wpa_printf(MSG_DEBUG, "genetlink: Trying to send message");
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
	if (wpa_rtlogger_get_flags(MSG_DEBUG) & WPA_RTLOGGER_LOG_TARGET_CONSOLE)
		CLOG("genetlink: Trying to send message");
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */

	res = nl_send_auto_complete(genetlink->sock, msg);
	if (res < 0) {
#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
		wpa_printf(MSG_ERROR, "genetlink: Failed to send message via "
			   "generic netlink: %s", strerror(errno));
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
		if (wpa_rtlogger_get_flags(MSG_ERROR) & WPA_RTLOGGER_LOG_TARGET_CONSOLE) {
			CERROR("genetlink: Failed to send message via generic netlink: "
				   "%s", strerror(errno));
		}
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */
	}
out:
	nlmsg_free(msg);
	return res;
}

#endif /* CONFIG_NO_WPA_RTLOGGER */
