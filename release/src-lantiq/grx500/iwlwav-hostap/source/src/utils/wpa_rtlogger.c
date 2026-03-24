#ifndef CONFIG_NO_WPA_RTLOGGER

#include "includes.h"
#include "common.h"

#include <unistd.h>
#include <sys/time.h>
#include "drivers/genetlink.h"
#include "drivers/genetlink_priv.h"
#include "wpa_rtlogger.h"
#ifdef CONFIG_WPA_RTLOGGER_USE_SYSLOG
#include <syslog.h>
#endif /* CONFIG_WPA_RTLOGGER_USE_SYSLOG */


struct wpa_rtlogger_data {
	struct wpa_rtlogger_config *cfg;
	struct genetlink_data *genetlink;
	pid_t app_pid;			/* application's PID */
	u8 app_oid;				/* application's OID */
	s8 cdbg_lvl;			/* level of console debug */
	s8 rdbg_lvl;			/* level of remote debug */
};


/* Application's and Driver's command IDs */
enum {

	WPA_RTLOGGER_APP_TO_DRV_CMDID_STARTUP = 1,	/* Application's event - Startup */
	WPA_RTLOGGER_APP_TO_DRV_CMDID_CLEANUP,		/* Application's event - Cleanup */
	WPA_RTLOGGER_APP_TO_DRV_CMDID_LOG,			/* Application's event - Log-message */
	WPA_RTLOGGER_DRV_TO_APP_CMDID_CONFIG_INIT,	/* Driver's event - RT Logger's initial Configuration */
	WPA_RTLOGGER_DRV_TO_APP_CMDID_CONFIG_NEW,	/* Driver's event - RT Logger's new Configuration */
	WPA_RTLOGGER_DRV_TO_APP_CMDID_CLEANUP		/* Driver's event - RT Logger's cleanup */
};


#define log_info_get_bfield_lid(var)	((var >> 17) & 0x3FFF)
#define log_info_put_bfield_lid(var)	(((u32)var & 0x3FFF) << 17)
#define log_info_get_bfield_oid(var)	((var >> 12) & 0x001F)
#define log_info_put_bfield_oid(var)	(((u32)var & 0x001F) << 12)
#define log_info_get_bfield_gid(var)	((var >> 5) & 0x007F)
#define log_info_put_bfield_gid(var)	(((u32)var & 0x007F) << 5)
#define log_info_get_bfield_fid(var)	(var & 0x001F)
#define log_info_put_bfield_fid(var)	log_info_get_bfield_fid((u32)var)


/* Application's and Driver's (common) message header */
struct wpa_rtlogger_msghdr {
	u16 cmd_id;				/* command's ID */
	u16 length;				/* length of payload */
	u32 pid;				/* application's PID (type of pid_t) */
	u32 log_info;			/* LID, OID, GID, FID */
	u32 wlan_if;            /* interface/card ID */
	u32 log_time;			/* time-stamp of log-message */
} STRUCT_PACKED;


/* Application's message payload */
struct wpa_rtlogger_app_msgpay {
	char name[IFNAMSIZ];	/* application's name */
} STRUCT_PACKED;


/* Driver's message payload */
struct wpa_rtlogger_drv_msgpay {
	s8 cdbg_lvl;			/* level of console debug */
	s8 rdbg_lvl;			/* level of remote debug */
} STRUCT_PACKED;


/* Application's message */
struct wpa_rtlogger_app_msg {
	struct wpa_rtlogger_msghdr hdr;
	struct wpa_rtlogger_app_msgpay data;
} STRUCT_PACKED;


/* Driver's message */
struct wpa_rtlogger_drv_msg {
	struct wpa_rtlogger_msghdr hdr;
	struct wpa_rtlogger_drv_msgpay data;
} STRUCT_PACKED;


/* Pointer to RT Logger's data */
static struct wpa_rtlogger_data *gwpa_rtlogger = NULL;


static inline bool _is_dbg_lvl_changed(struct wpa_rtlogger_data *wpa_rtlogger,
					   struct wpa_rtlogger_drv_msgpay *data)
{
	if ((data->cdbg_lvl != wpa_rtlogger->cdbg_lvl) ||
		(data->rdbg_lvl != wpa_rtlogger->rdbg_lvl))
		return true;

	return false;
}


#ifndef CONFIG_WPA_RTLOGGER_RUN_DEMO
static inline s8 _convert_debug_level(s8 level)
{
	s8 res = (s8)MSG_WARNING - level;

	if (res < 0)
		res = 0;
	else if (res > (MSG_MUTE))
		res = MSG_MUTE;

	return res;
}
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */


#ifdef CONFIG_WPA_RTLOGGER_USE_SYSLOG
static inline int _get_syslog_prio(wpa_rtlogger_log_type_e log_type)
{
	int prio = LOG_INFO;

	switch (log_type)
	{
		case WPA_RTLOGGER_LOG_TYPE_ERROR:
		{
			prio = LOG_ERR;
			break;
		}
		case WPA_RTLOGGER_LOG_TYPE_WARNING:
		{
			prio = LOG_WARNING;
			break;
		}
		case WPA_RTLOGGER_LOG_TYPE_INFO:
		{
			prio = LOG_INFO;
			break;
		}
		default: break;
	}
	return prio;
}
#endif /* CONFIG_WPA_RTLOGGER_USE_SYSLOG */


static inline unsigned long _get_timestamp(void)
{
	struct timeval ts;

	if (gettimeofday(&ts, NULL) != 0)
		return 0;
	return ts.tv_usec + (ts.tv_sec * 1000000);
}


static void wpa_rtlogger_parse_event(void *param, void *packet)
{
	u8 oid;
	bool is_cfg_changed = false;
	struct wpa_rtlogger_data *wpa_rtlogger = param;
	struct wpa_rtlogger_drv_msg *msg = (struct wpa_rtlogger_drv_msg *)packet;
	struct wpa_rtlogger_msghdr *hdr = &msg->hdr;
	struct wpa_rtlogger_drv_msgpay *data = &msg->data;

	oid = log_info_get_bfield_oid(hdr->log_info);

	wpa_printf(/*MSG_INFO*/MSG_DEBUG, "wpa_rtlogger: Notification message received from mtlkroot: "
		   "cmd_id=0x%04X, data_len=%d, oid=%d, params=[cdebug=%d, rdebug=%d]",
		   hdr->cmd_id, hdr->length, oid, data->cdbg_lvl, data->rdbg_lvl);

#ifndef CONFIG_WPA_RTLOGGER_RUN_DEMO
	/* Convert debug levels to internal format */
	data->cdbg_lvl = _convert_debug_level(data->cdbg_lvl);
	data->rdbg_lvl = _convert_debug_level(data->rdbg_lvl);
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */

	switch (hdr->cmd_id)
	{
		case WPA_RTLOGGER_DRV_TO_APP_CMDID_CONFIG_INIT:
		{
			/* Check received PID, if invalid then ignore message */
			if (hdr->pid == (u32)wpa_rtlogger->app_pid) {
				/* Save OID */
				wpa_rtlogger->app_oid = oid;
				is_cfg_changed = true;
			}
			else {
				wpa_printf(/*MSG_INFO*/MSG_DEBUG, "wpa_rtlogger: Notification message ignored from mtlkroot "
					   "due to invalid PID: own_pid=%d, received_pid=%d",
					   wpa_rtlogger->app_pid, hdr->pid);
			}
			break;
		}
		case WPA_RTLOGGER_DRV_TO_APP_CMDID_CONFIG_NEW:
		{
			/* Check received OID, if invalid then ignore message */
			if (oid == (u32)wpa_rtlogger->app_oid) {
				/* Check that debug level is changed, if not changed then ignore the message */
				is_cfg_changed = _is_dbg_lvl_changed(wpa_rtlogger, data);
				if (!is_cfg_changed) {
					wpa_printf(/*MSG_INFO*/MSG_DEBUG, "wpa_rtlogger: Notification message ignored from mtlkroot "
						   "due to unchanged debug level");
				}
			}
			else {
				wpa_printf(/*MSG_INFO*/MSG_DEBUG, "wpa_rtlogger: Notification message ignored from mtlkroot "
					   "due to invalid OID: own_oid=%d, received_oid=%d",
					   wpa_rtlogger->app_oid, oid);
			}
			break;
		}
		case WPA_RTLOGGER_DRV_TO_APP_CMDID_CLEANUP:
		{
			/* Disable remote debug */
			data->cdbg_lvl = wpa_rtlogger->cdbg_lvl;
#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
			data->rdbg_lvl = -1;
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
			data->rdbg_lvl = MSG_ERROR + 1;
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */
			is_cfg_changed = _is_dbg_lvl_changed(wpa_rtlogger, data);
			break;
		}
		default: break;
	}

	if (is_cfg_changed) {
		wpa_rtlogger->cdbg_lvl = data->cdbg_lvl;
		wpa_rtlogger->rdbg_lvl = data->rdbg_lvl;
		wpa_printf(/*MSG_INFO*/MSG_DEBUG, "wpa_rtlogger: Notification message from mtlkroot processed: "
			   "debug level set to: [cdebug=%s, rdebug=%s]",
			   debug_level_str(wpa_rtlogger->cdbg_lvl), debug_level_str(wpa_rtlogger->rdbg_lvl));
	}

#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
	/* DEMO: only this code line */
	wpa_rtlogger_ilog1_d_9(__FUNCTION__, 1, 9, 1, 2, __LINE__,
		   "wpa_rtlogger: Notification message received from mtlkroot: cmd_id=0x%04X", hdr->cmd_id);
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
	wpa_printf(MSG_INFO, "wpa_rtlogger: Notification message received from mtlkroot: "
		   "cmd_id=0x%04X", hdr->cmd_id);
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */
}


static int wpa_rtlogger_send_event(struct wpa_rtlogger_data *wpa_rtlogger, const u8 cmd)
{
	struct wpa_rtlogger_app_msg msg;
	struct wpa_rtlogger_msghdr *hdr = &msg.hdr;
	struct wpa_rtlogger_app_msgpay *data = &msg.data;

	//if (wpa_rtlogger == NULL)
	//	return - 1;

	os_memset(hdr, 0, sizeof(*hdr));
	hdr->cmd_id = (u16)cmd;
	hdr->length = sizeof(*data);
	hdr->pid = (u32)wpa_rtlogger->app_pid;
	os_strlcpy(data->name, wpa_rtlogger->cfg->app_name, sizeof(data->name));

	return genetlink_send_msg(wpa_rtlogger->genetlink, (void *)&msg, sizeof(msg));
}


int wpa_rtlogger_send_logevent(struct wpa_rtlogger_data *wpa_rtlogger,
		   const u32 ids /* LID, OID, GID, FID */,
		   const u32 timestamp, const void *data, const u16 data_len)
{
	/* Note! Console log is allowed in this function only */
	int res;
	void *msg;
	size_t msg_len;
	struct wpa_rtlogger_msghdr *msgbody_hdr;
	struct wpa_rtlogger_app_msgpay *msgbody_app;
	void *msgbody_log;
	errno_t err;

	if ((data == NULL) || (data_len == 0)) {
#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
		wpa_printf(/*MSG_INFO*/MSG_DEBUG, "wpa_rtlogger: Failed to send log-message: "
			   "data buffer is empty");
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
		if (wpa_rtlogger_get_flags(MSG_ERROR) & WPA_RTLOGGER_LOG_TARGET_CONSOLE)
			CERROR("wpa_rtlogger: Failed to send log-message: data buffer is empty");
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */
		return 0;
	}

	if (wpa_rtlogger == NULL) {
#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
		wpa_printf(/*MSG_INFO*/MSG_DEBUG, "wpa_rtlogger: Failed to send log-message: "
			   "RT-logger not initialized yet");
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
		if (wpa_rtlogger_get_flags(MSG_ERROR) & WPA_RTLOGGER_LOG_TARGET_CONSOLE)
			CERROR("wpa_rtlogger: Failed to send log-message: RT-logger not initialized yet");
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */
		return 0;
	}

	msg_len = (size_t)data_len + sizeof(struct wpa_rtlogger_app_msg);
	msg = os_malloc(msg_len);
	if (msg == NULL) {
#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
		wpa_printf(MSG_ERROR, "wpa_rtlogger: Failed to allocate memory for message");
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
		if (wpa_rtlogger_get_flags(MSG_ERROR) & WPA_RTLOGGER_LOG_TARGET_CONSOLE)
			CERROR("wpa_rtlogger: Failed to allocate memory for message");
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */
		return -1;
	}

	msgbody_hdr = (struct wpa_rtlogger_msghdr *)msg;
	msgbody_app = (struct wpa_rtlogger_app_msgpay *)(msgbody_hdr + 1);
	msgbody_log = (void *)(msgbody_app + 1);

	msgbody_hdr->cmd_id = (u16)WPA_RTLOGGER_APP_TO_DRV_CMDID_LOG;
	msgbody_hdr->length = data_len + (u16)sizeof(*msgbody_app);
	msgbody_hdr->pid = (u32)wpa_rtlogger->app_pid;
	msgbody_hdr->log_info = ids;
	msgbody_hdr->wlan_if = wpa_rtlogger->cfg->wlan_if;
	msgbody_hdr->log_time = timestamp;
	os_strlcpy(msgbody_app->name, wpa_rtlogger->cfg->app_name, sizeof(msgbody_app->name));
	err = memcpy_s(msgbody_log, (size_t)data_len, data, (size_t)data_len);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		os_free(msg);
		return -1;
	}

	res = genetlink_send_msg(wpa_rtlogger->genetlink, msg, msg_len);
	os_free(msg);
	return res;
}

int wpa_rtlogger_get_if_num(struct wpa_rtlogger_config *cfg, const char *ifname)
{
	char *first_digit = strpbrk(ifname, "0123456789");
	int  if_num;

	if (!first_digit) {
		wpa_printf(MSG_ERROR, "Error occurred getting interface number: %s\n", ifname);
		return -1;
	}

	if_num = (int)(first_digit[0] - '0') / 2;
	if (if_num < 0) {
		wpa_printf(MSG_ERROR, "Sanity check error occurred getting interface number %d\n", if_num);
		return -1;
	}

	cfg->wlan_if = if_num;
	return 0;
}

struct wpa_rtlogger_data * wpa_rtlogger_init(struct wpa_rtlogger_config *cfg)
{
	struct wpa_rtlogger_data *wpa_rtlogger;
	struct genetlink_config *genetlink_cfg;

	wpa_rtlogger = os_zalloc(sizeof(*wpa_rtlogger));
	if (wpa_rtlogger == NULL)
		return NULL;

	wpa_rtlogger->cfg = cfg;
	wpa_rtlogger->app_pid = getpid();
	wpa_rtlogger->app_oid = WPA_RTLOGGER_APP_OID_INVALID;
#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
	wpa_rtlogger->cdbg_lvl = (s8)(-1);
	wpa_rtlogger->rdbg_lvl = (s8)(-1);
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
	wpa_rtlogger->cdbg_lvl = (s8)(MSG_INFO);
	wpa_rtlogger->rdbg_lvl = (s8)(MSG_ERROR + 1);	/* remote logger is disabled */
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */

	genetlink_cfg = os_zalloc(sizeof(*genetlink_cfg));
	if (genetlink_cfg == NULL) {
		wpa_printf(MSG_ERROR, "wpa_rtlogger: Failed to allocate memory for genetlink "
			   "configuration");
		wpa_rtlogger_deinit(wpa_rtlogger);
		return NULL;
	}
	genetlink_cfg->is_static = false;
	genetlink_cfg->ctx = (void *)wpa_rtlogger;
	genetlink_cfg->rxmsg_cb = wpa_rtlogger_parse_event;
	genetlink_cfg->genetlink_group = NETLINK_RTLOG_APPS_GROUP;
	genetlink_cfg->genetlink_cmd = NL_DRV_CMD_RTLOG_NOTIFY;
	wpa_rtlogger->genetlink = genetlink_init(genetlink_cfg);
	if (wpa_rtlogger->genetlink == NULL) {
		wpa_printf(MSG_ERROR, "wpa_rtlogger: Failed to initialize genetlink");
		wpa_rtlogger_deinit(wpa_rtlogger);
		return NULL;
	}
	gwpa_rtlogger = wpa_rtlogger;

#ifdef CONFIG_WPA_RTLOGGER_USE_SYSLOG
	openlog(cfg->app_name, 0, LOG_DAEMON);
#endif /* CONFIG_WPA_RTLOGGER_USE_SYSLOG */

	if (wpa_rtlogger_send_event(wpa_rtlogger, WPA_RTLOGGER_APP_TO_DRV_CMDID_STARTUP) < 0) {
		wpa_rtlogger_deinit(wpa_rtlogger);
		return NULL;
	}

	wpa_printf(/*MSG_INFO*/MSG_DEBUG, "wpa_rtlogger: Initialization done");
	return wpa_rtlogger;
}


void wpa_rtlogger_deinit(struct wpa_rtlogger_data *wpa_rtlogger)
{
	if (wpa_rtlogger == NULL)
		return;

	gwpa_rtlogger = NULL;

	if (wpa_rtlogger->genetlink != NULL) {
		wpa_rtlogger_send_event(wpa_rtlogger, WPA_RTLOGGER_APP_TO_DRV_CMDID_CLEANUP);
		genetlink_deinit(wpa_rtlogger->genetlink);
	}

#ifdef CONFIG_WPA_RTLOGGER_USE_SYSLOG
	closelog();
#endif /* CONFIG_WPA_RTLOGGER_USE_SYSLOG */

	if (!wpa_rtlogger->cfg->is_static)
		os_free(wpa_rtlogger->cfg);
	os_free(wpa_rtlogger);

	wpa_printf(/*MSG_INFO*/MSG_DEBUG, "wpa_rtlogger: Cleanup done");
}


#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
int wpa_rtlogger_get_flags(int level, int gid)
{
	int flags = 0;
	gid = gid;	/* application uses the same debug level for all GIDs */

	if (gwpa_rtlogger != NULL) {
		if (level <= gwpa_rtlogger->cdbg_lvl)
			flags |= WPA_RTLOGGER_LOG_TARGET_CONSOLE;
		if (level <= gwpa_rtlogger->rdbg_lvl)
			flags |= WPA_RTLOGGER_LOG_TARGET_REMOTE;
	}
	return flags;
}
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
int wpa_rtlogger_get_flags(int level)
{
	int flags = 0;

	if (gwpa_rtlogger != NULL) {
		if (level >= gwpa_rtlogger->cdbg_lvl)
			flags |= WPA_RTLOGGER_LOG_TARGET_CONSOLE;
		if (level >= gwpa_rtlogger->rdbg_lvl)
			flags |= WPA_RTLOGGER_LOG_TARGET_REMOTE;
	}
	else {
		if (level >= wpa_debug_level)
			flags |= WPA_RTLOGGER_LOG_TARGET_CONSOLE;
	}
	return flags;
}


void wpa_rtlogger_console_vlog(const wpa_rtlogger_log_type_e log_type, const char *fmt, va_list args)
{
	int cnt = 0, offs = 0;
	char buffer[CONFIG_WPA_RTLOGGER_PRINT_BUFFER_MAX_SIZE] = {0};

	//if (gwpa_rtlogger == NULL)
	//	return;

	cnt = sprintf_s(buffer, sizeof(buffer) - offs, "[%010lu]: ", _get_timestamp());
	if (cnt <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, cnt);
		goto end;
	}

	offs += cnt;
	vsnprintf_s(buffer + offs, sizeof(buffer) - offs, fmt, args);
end:
	buffer[sizeof(buffer) - 1] = 0;

#ifdef CONFIG_WPA_RTLOGGER_USE_SYSLOG
	/* Print to syslog */
	syslog(_get_syslog_prio(log_type), "%s", buffer);
#endif /* CONFIG_WPA_RTLOGGER_USE_SYSLOG */
	/* Print to stderr */
	fprintf(stderr, "%s\n", buffer);
}
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */


void wpa_rtlogger_console_log(const wpa_rtlogger_log_type_e log_type,
#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
		   const char *fname, const int line_no, const char *log_level,
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */
		   const char *fmt, ...)
{
#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
	int cnt = 0, offs = 0;
	char buffer[CONFIG_WPA_RTLOGGER_PRINT_BUFFER_MAX_SIZE] = {0};
	va_list args;

	//if (gwpa_rtlogger == NULL)
	//	return;

	cnt = sprintf_s(buffer, sizeof(buffer) - offs, "[%010lu] %s%s[%s:%d]: ", _get_timestamp(),
			gwpa_rtlogger->cfg->app_name, log_level, fname, line_no);
	if (cnt <= 0) {
		wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, cnt);
		goto end;
	}

	offs += cnt;

	va_start(args, fmt);
	vsnprintf_s(buffer + offs, sizeof(buffer) - offs, fmt, args);
	va_end(args);
end:
	buffer[sizeof(buffer) - 1] = 0;

#ifdef CONFIG_WPA_RTLOGGER_USE_SYSLOG
	/* Print to syslog */
	syslog(_get_syslog_prio(log_type), "%s", buffer);
#endif /* CONFIG_WPA_RTLOGGER_USE_SYSLOG */
	/* Print to stderr */
	fprintf(stderr, "%s\n", buffer);
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
	va_list args;

	va_start(args, fmt);
	wpa_rtlogger_console_vlog(log_type, fmt, args);
	va_end(args);
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */
}


#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
/* FIXME: implement this code via script (like we have for WLAN driver) */
void wpa_rtlogger_ilog1_d_9(const char *fname, const int dbg_level,
		   const u8 oid, const u8 gid, const u8 fid, const u16 lid,
		   const char *fmt, s32 d1)
{
	int flags = wpa_rtlogger_get_flags(dbg_level, 0);

	if (flags & WPA_RTLOGGER_LOG_TARGET_REMOTE) {
		u8 *data__;
		u32 lid_u32 = (u32)lid;
		size_t fnamelen__ = strnlen_s(fname, RSIZE_MAX_STR) + 1;
		if (!IS_VALID_RANGE(fnamelen__, AP_MIN_VALID_CHAR, RSIZE_MAX_STR - 1)) {
			wpa_msg(wpa_s, MSG_ERROR, "wpa_rtlogger_ilog1_d_9 - wrong len");
			return;
		}
		size_t datalen__ = WPA_RTLOGGER_LOGMSG_STRING_SIZE(fname) +
						   WPA_RTLOGGER_LOGMSG_SCALAR_SIZE(lid_u32) +
						   WPA_RTLOGGER_LOGMSG_SCALAR_SIZE(d1);

		if (datalen__ > 2047)
			return;

		data__ = (u8 *)os_malloc(datalen__);

		if (data__ != NULL) {
			u32 ids__ = 0;
			u8 *p__ = data__;

			WPA_RTLOGGER_LOGMSG_PUT_STRING(fname);
			WPA_RTLOGGER_LOGMSG_PUT_INT32(lid_u32);
			WPA_RTLOGGER_LOGMSG_PUT_INT32(d1);

			ids__ = log_info_put_bfield_lid(lid) |
					log_info_put_bfield_oid(oid) |
					log_info_put_bfield_gid(gid) |
					log_info_put_bfield_fid(fid);

			wpa_rtlogger_send_logevent(gwpa_rtlogger, ids__, (u32)_get_timestamp(),
					   data__, datalen__);
			os_free(data__);
		}
	}
	if (flags & WPA_RTLOGGER_LOG_TARGET_CONSOLE) {
		CLOG(fname, lid, 1, fmt, d1);
	}
}
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
void wpa_rtlogger_remote_vlog(const char *fmt, va_list args)
{
	u8 *data;
	size_t argslen__, datalen;

	//if (gwpa_rtlogger == NULL) return;

	argslen__ = (size_t)vsnprintf(NULL, 0, fmt, args) + 1;
	datalen = WPA_RTLOGGER_LOGMSG_STRING_SIZE(args);

	if (datalen > 2047)	return;

	data = (u8 *)os_zalloc(datalen);

	if (data != NULL) {
		u8 *p__ = data;

		WPA_RTLOGGER_LOGMSG_PUT_STRING_HEADER(args);
		vsnprintf_s((char *)p__, argslen__, fmt, args);

		wpa_rtlogger_send_logevent(gwpa_rtlogger,
									log_info_put_bfield_lid(1) |
							        log_info_put_bfield_oid(gwpa_rtlogger->app_oid) |
							        log_info_put_bfield_gid(gwpa_rtlogger->cfg->gid) |
							        log_info_put_bfield_fid(1),
									(u32)_get_timestamp(),
									(void *)data, (u16)datalen);
		os_free((void *)data);
	}
}


void wpa_rtlogger_remote_log(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	wpa_rtlogger_remote_vlog(fmt, args);
	va_end(args);
}
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */

#endif /* CONFIG_NO_WPA_RTLOGGER */
