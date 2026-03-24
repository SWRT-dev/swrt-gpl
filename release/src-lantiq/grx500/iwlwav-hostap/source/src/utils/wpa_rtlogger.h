#ifndef WPA_RTLOGGER_H
#define WPA_RTLOGGER_H

#define CONFIG_WPA_RTLOGGER_PRINT_BUFFER_MAX_SIZE	(2048)
//#define CONFIG_WPA_RTLOGGER_USE_SYSLOG			(1)
//#define CONFIG_WPA_RTLOGGER_RUN_DEMO				(1)


#include <stdbool.h>


/* RT Logger's configuration */
struct wpa_rtlogger_config {
	bool is_static;				/* if TRUE, then memory is allocated statically */
	void *ctx;					/* pointer to context */
	char app_name[IFNAMSIZ];	/* application's name */
	u32  gid;					/* GID to substitute: 1 = hostapd, 2 = wpa_supplicant */
	u32  wlan_if;               /* interface id */
};


/* Application's name and OID */
#define WPA_RTLOGGER_APP_NAME_HOSTAPD		"hostapd"
#define WPA_RTLOGGER_APP_OID_HOSTAPD		9
#define WPA_RTLOGGER_APP_GID_HOSTAPD		1
#define WPA_RTLOGGER_APP_NAME_WPASUPPLICANT	"wpa_supplicant"
#define WPA_RTLOGGER_APP_OID_WPASUPPLICANT	9
#define WPA_RTLOGGER_APP_GID_WPASUPPLICANT	2
#define WPA_RTLOGGER_APP_OID_INVALID		255


/* Log target flags */
#define WPA_RTLOGGER_LOG_TARGET_CONSOLE		(1 << 0)
#define WPA_RTLOGGER_LOG_TARGET_REMOTE		(1 << 1)


/* Log types (error, warning, info) */
typedef enum wpa_rtlogger_log_type {
	WPA_RTLOGGER_LOG_TYPE_ERROR,
	WPA_RTLOGGER_LOG_TYPE_WARNING,
	WPA_RTLOGGER_LOG_TYPE_INFO
} wpa_rtlogger_log_type_e;


/* Application's log-message paylod data types */
typedef enum wpa_rtlogger_data_type
{
	WPA_RTLOGGER_LOGMSG_DATA_TYPE_STRING,
	WPA_RTLOGGER_LOGMSG_DATA_TYPE_INT8,
	WPA_RTLOGGER_LOGMSG_DATA_TYPE_INT32,
	WPA_RTLOGGER_LOGMSG_DATA_TYPE_INT64,
	WPA_RTLOGGER_LOGMSG_DATA_TYPE_MACADDR,
	WPA_RTLOGGER_LOGMSG_DATA_TYPE_IP6ADDR,
	WPA_RTLOGGER_LOGMSG_DATA_TYPE_SIGNAL
} wpa_rtlogger_data_type_e;
#define wpa_rtlogger_data_type_e u32


/* Application's log-message payload element type */
struct wpa_rtlogger_app_logmsgelem_type {
	wpa_rtlogger_data_type_e datatype;			/* data type */
} STRUCT_PACKED;


/* Application's log-message payload element size */
struct wpa_rtlogger_app_logmsgelem_len {
	u32 datalen;								/* data length */
} STRUCT_PACKED;


/* Log-message payload elements alingment */
#define WPA_RTLOGGER_LOGMSG_ELEM_WORD_ALINGMENT	(1)	/* 32-bit word alingment */
#ifdef WPA_RTLOGGER_LOGMSG_ELEM_WORD_ALINGMENT
#define WPA_RTLOGGER_LOGMSG_ELEM_SIZE(x)		((((x) + 3) >> 2) << 2)
#else
#define WPA_RTLOGGER_LOGMSG_ELEM_SIZE(x)		(x)
#endif

/* Log-message payload element string */
#define WPA_RTLOGGER_LOGMSG_STRING_HEADER_SIZE													\
			WPA_RTLOGGER_LOGMSG_ELEM_SIZE(sizeof(struct wpa_rtlogger_app_logmsgelem_type) +		\
										  sizeof(struct wpa_rtlogger_app_logmsgelem_len))

#define WPA_RTLOGGER_LOGMSG_STRING_SIZE(str)													\
			WPA_RTLOGGER_LOGMSG_ELEM_SIZE(WPA_RTLOGGER_LOGMSG_STRING_HEADER_SIZE +				\
										  str##len__)

#define WPA_RTLOGGER_LOGMSG_PUT_STRING_HEADER(str)															\
		((struct wpa_rtlogger_app_logmsgelem_type *) p__)->datatype = WPA_RTLOGGER_LOGMSG_DATA_TYPE_STRING;	\
		p__ += sizeof(struct wpa_rtlogger_app_logmsgelem_type);												\
		((struct wpa_rtlogger_app_logmsgelem_len *) p__)->datalen = str##len__;								\
		p__ += sizeof(struct wpa_rtlogger_app_logmsgelem_len);

#define WPA_RTLOGGER_LOGMSG_PUT_STRING(str)																	\
		WPA_RTLOGGER_LOGMSG_PUT_STRING_HEADER(str)															\
		memcpy(p__, str, str##len__);																		\
		p__ += WPA_RTLOGGER_LOGMSG_ELEM_SIZE(str##len__);

/* Log-message payload element scalar */
#define WPA_RTLOGGER_LOGMSG_SCALAR_SIZE(val)													\
			WPA_RTLOGGER_LOGMSG_ELEM_SIZE((sizeof(struct wpa_rtlogger_app_logmsgelem_type) +	\
										   sizeof(val)))

#ifdef WPA_RTLOGGER_LOGMSG_ELEM_WORD_ALINGMENT
#define WPA_RTLOGGER_LOGMSG_PUT_INT8(val)																		\
		{																										\
			u32 __word32__ = (u32)val;																			\
			((struct wpa_rtlogger_app_logmsgelem_type *) p__)->datatype = WPA_RTLOGGER_LOGMSG_DATA_TYPE_INT8;	\
			p__ += sizeof(struct wpa_rtlogger_app_logmsgelem_type);												\
			*(u32 *)p__ = __word32__;																			\
			p__ += sizeof(__word32__);																			\
		}
#else /* !WPA_RTLOGGER_LOGMSG_ELEM_WORD_ALINGMENT */
#define WPA_RTLOGGER_LOGMSG_PUT_INT8(val)																	\
		((struct wpa_rtlogger_app_logmsgelem_type *) p__)->datatype = WPA_RTLOGGER_LOGMSG_DATA_TYPE_INT8;	\
		p__ += sizeof(struct wpa_rtlogger_app_logmsgelem_type);												\
		*(s8 *)p__ = val;																					\
		p__ += sizeof(s8);
#endif /* WPA_RTLOGGER_LOGMSG_ELEM_WORD_ALINGMENT */

#define WPA_RTLOGGER_LOGMSG_PUT_INT32(val)																	\
		((struct wpa_rtlogger_app_logmsgelem_type *) p__)->datatype = WPA_RTLOGGER_LOGMSG_DATA_TYPE_INT32;	\
		p__ += sizeof(struct wpa_rtlogger_app_logmsgelem_type);												\
		*(s32 *)p__ = val;																					\
		p__ += sizeof(s32);

#define WPA_RTLOGGER_LOGMSG_PUT_INT64(val)																	\
		((struct wpa_rtlogger_app_logmsgelem_type *) p__)->datatype = WPA_RTLOGGER_LOGMSG_DATA_TYPE_INT64;	\
		p__ += sizeof(struct wpa_rtlogger_app_logmsgelem_type);												\
		*(s64 *)p__ = val;																					\
		p__ += sizeof(s64);

#define WPA_RTLOGGER_LOGMSG_PUT_POINTER(val)																	\
		if (sizeof(val) = sizeof(s32))																			\
			((struct wpa_rtlogger_app_logmsgelem_type *) p__)->datatype = WPA_RTLOGGER_LOGMSG_DATA_TYPE_INT32;	\
		else /*if (sizeof(val) = sizeof(s64))*/																	\
			((struct wpa_rtlogger_app_logmsgelem_type *) p__)->datatype = WPA_RTLOGGER_LOGMSG_DATA_TYPE_INT64;	\
		p__ += sizeof(struct wpa_rtlogger_app_logmsgelem_type);													\
		memcpy(p__, &val, sizeof(val));																			\
		p__ += sizeof(val);

/* Log-message payload element MAC-address */
#define WPA_RTLOGGER_LOGMSG_MACADDR_SIZE(addr)													\
			WPA_RTLOGGER_LOGMSG_ELEM_SIZE((sizeof(struct wpa_rtlogger_app_logmsgelem_type) +	\
										   6))		/* FIXME: to use define instead of digit */

#define WPA_RTLOGGER_LOGMSG_PUT_MACADDR(addr)																	\
		((struct wpa_rtlogger_app_logmsgelem_type *) p__)->datatype = WPA_RTLOGGER_LOGMSG_DATA_TYPE_MACADDR;	\
		p__ += sizeof(struct wpa_rtlogger_app_logmsgelem_type);													\
		memcpy(p__, addr, 6);						/* FIXME: to use define instead of digit */					\
		p__ += WPA_RTLOGGER_LOGMSG_ELEM_SIZE(6);	/* FIXME: to use define instead of digit */

/* Log-message payload element IPv6-address */
#define WPA_RTLOGGER_LOGMSG_IP6ADDR_SIZE(addr)													\
			WPA_RTLOGGER_LOGMSG_ELEM_SIZE((sizeof(struct wpa_rtlogger_app_logmsgelem_type) +	\
										   16))		/* FIXME: to use define instead of digit */

#define WPA_RTLOGGER_LOGMSG_PUT_IP6ADDR(addr)																	\
		((struct wpa_rtlogger_app_logmsgelem_type *) p__)->datatype = WPA_RTLOGGER_LOGMSG_DATA_TYPE_IP6ADDR;	\
		p__ += sizeof(struct wpa_rtlogger_app_logmsgelem_type);													\
		memcpy(p__, addr, 16);						/* FIXME: to use define instead of digit */					\
		p__ += WPA_RTLOGGER_LOGMSG_ELEM_SIZE(16);	/* FIXME: to use define instead of digit */


#ifdef CONFIG_NO_WPA_RTLOGGER
#define wpa_rtlogger_get_if_num(args...)	(0)
#define wpa_rtlogger_init(args...)
#define wpa_rtlogger_deinit(args...)
#define wpa_rtlogger_send_logevent(args...)
#define wpa_rtlogger_get_flags(args...) 	(0)
#define wpa_rtlogger_console_vlog(args...)
#define wpa_rtlogger_remote_vlog(args...)
#define wpa_rtlogger_console_log(args...)
#define wpa_rtlogger_remote_log(args...)
#else /* !CONFIG_NO_WPA_RTLOGGER */
int wpa_rtlogger_get_if_num(struct wpa_rtlogger_config *cfg, const char *ifname);
struct wpa_rtlogger_data * wpa_rtlogger_init(struct wpa_rtlogger_config *cfg);
void wpa_rtlogger_deinit(struct wpa_rtlogger_data *wpa_rtlogger);
int wpa_rtlogger_send_logevent(struct wpa_rtlogger_data *wpa_rtlogger,
		   const u32 ids /* LID, OID, GID, FID */,
		   const u32 timestamp, const void *data, const u16 data_len);
#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
int wpa_rtlogger_get_flags(int level, int gid);
void wpa_rtlogger_console_log(const wpa_rtlogger_log_type_e log_type,
		   const char *fname, const int line_no, const char *log_level,
		   const char *fmt, ...);
/* FIXME: implement this code via script (like we have for WLAN driver) */
void wpa_rtlogger_ilog1_d_9(const char *fname, const int dbg_level,
		   const u8 oid, const u8 gid, const u8 fid, const u16 lid,
		   const char *fmt, s32 d1);
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
int wpa_rtlogger_get_flags(int level);
void wpa_rtlogger_console_vlog(const wpa_rtlogger_log_type_e log_type, const char *fmt, va_list args);
void wpa_rtlogger_remote_vlog(const char *fmt, va_list args);
void wpa_rtlogger_console_log(const wpa_rtlogger_log_type_e log_type, const char *fmt, ...);
void wpa_rtlogger_remote_log(const char *fmt, ...);
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */
#endif /* CONFIG_NO_WPA_RTLOGGER */


#ifdef CONFIG_WPA_RTLOGGER_RUN_DEMO
#define CLOG(fname, line_no, log_level, fmt, ...)	\
	wpa_rtlogger_console_log(WPA_RTLOGGER_LOG_TYPE_INFO, fname, line_no, #log_level, fmt, ## __VA_ARGS__)
#define CINFO(fname, line_no, fmt, ...)				\
	wpa_rtlogger_console_log(WPA_RTLOGGER_LOG_TYPE_INFO, "", 0, "", fmt, ## __VA_ARGS__)
#define CERROR(fname, line_no, fmt, ...)			\
	wpa_rtlogger_console_log(WPA_RTLOGGER_LOG_TYPE_ERROR, fname, line_no, "E", fmt, ## __VA_ARGS__)
#define CWARNING(fname, line_no, fmt, ...)			\
	wpa_rtlogger_console_log(WPA_RTLOGGER_LOG_TYPE_WARNING, fname, line_no, "W", fmt, ## __VA_ARGS__)
#else /* !CONFIG_WPA_RTLOGGER_RUN_DEMO */
#define CLOG(fmt, ...)		\
	wpa_rtlogger_console_log(WPA_RTLOGGER_LOG_TYPE_INFO, fmt, ## __VA_ARGS__)
#define CINFO(fmt, ...)		\
	wpa_rtlogger_console_log(WPA_RTLOGGER_LOG_TYPE_INFO, fmt, ## __VA_ARGS__)
#define CERROR(fmt, ...)	\
	wpa_rtlogger_console_log(WPA_RTLOGGER_LOG_TYPE_ERROR, fmt, ## __VA_ARGS__)
#define CWARNING(fmt, ...)	\
	wpa_rtlogger_console_log(WPA_RTLOGGER_LOG_TYPE_WARNING, fmt, ## __VA_ARGS__)
#define RLOG(fmt, ...)		\
	wpa_rtlogger_remote_log(fmt, ## __VA_ARGS__)
#endif /* CONFIG_WPA_RTLOGGER_RUN_DEMO */

#endif /* WPA_RTLOGGER_H */
