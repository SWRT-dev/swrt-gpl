/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *   Copyright (C) 2018 Samsung Electronics Co., Ltd.
 *
 *   linux-cifsd-devel@lists.sourceforge.net
 */

#ifndef __KSMBDTOOLS_H__
#define __KSMBDTOOLS_H__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <poll.h>
#include <getopt.h>
#include <pthread.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

char *ascii_strdown(char *str, size_t len);

int atomic_int_add(volatile int *atomic, int val);
void atomic_int_inc(volatile int *atomic);

int atomic_int_compare_and_exchange(volatile int *atomic, int oldval, int newval);

#define KEY_ID 0x0
#define KEY_STRING 0x1
struct LIST {
	struct LIST *prev;
	struct LIST *next;
	int type;
	unsigned long long id;
	char *keystr;
	void *item;
	pthread_mutex_t *mutex;
};

struct LIST *list_init(struct LIST **list);
int list_add_str(struct LIST **list, void *item, char *str);
int list_add(struct LIST **list, void *item, unsigned long long id);
void list_append(struct LIST **list, void *item);
int list_remove(struct LIST **list, unsigned long long id);
int list_remove_dec(struct LIST **list, unsigned long long id);
void *list_get(struct LIST **list, unsigned long long id);
void list_clear(struct LIST **list);
int list_foreach(struct LIST **list, void (*func)(void *item, unsigned long long id, void *user_data), void *user_data);

static unsigned long long list_tokey(void *ptr)
{
	size_t p = (size_t)ptr;
	return p;
}

static void *list_fromkey(unsigned long long key)
{
	size_t p = key;

	return (void *)p;
}

typedef enum {
	step_A, step_B, step_C
} base64_encodestep;

typedef struct {
	base64_encodestep step;
	char result;
	int stepcount;
} base64_encodestate;

typedef enum {
	step_a, step_b, step_c, step_d
} base64_decodestep;

typedef struct {
	base64_decodestep step;
	char plainchar;
} base64_decodestate;

struct smbconf_global {
	int flags;
	int map_to_guest;
	char *guest_account;

	char *server_string;
	char *work_group;
	char *netbios_name;
	char *server_min_protocol;
	char *server_max_protocol;
	char *root_dir;
	int server_signing;
	int sessions_cap;
	int restrict_anon;
	unsigned short tcp_port;
	unsigned short ipc_timeout;
	unsigned int deadtime;
	int bind_interfaces_only;
	char **interfaces;
	unsigned long file_max;
	unsigned int smb2_max_read;
	unsigned int smb2_max_write;
	unsigned int smb2_max_trans;
	unsigned int share_fake_fscaps;
	unsigned int gen_subauth[3];
	char *krb5_keytab_file;
	char *krb5_service_name;
	char *pwddb;
	char *smbconf;
};

#define KSMBD_LOCK_FILE		"/tmp/ksmbd.lock"

#define KSMBD_RESTRICT_ANON_TYPE_1	1
#define KSMBD_RESTRICT_ANON_TYPE_2	2

extern struct smbconf_global global_conf;

#define KSMBD_CONF_MAP_TO_GUEST_NEVER		(0)
#define KSMBD_CONF_MAP_TO_GUEST_BAD_USER	(1 << 0)
#define KSMBD_CONF_MAP_TO_GUEST_BAD_PASSWORD	(1 << 1)
#define KSMBD_CONF_MAP_TO_GUEST_BAD_UID		(1 << 2)

#define KSMBD_CONF_DEFAULT_NETBIOS_NAME	"KSMBD SERVER"
#define KSMBD_CONF_DEFAULT_SERVER_STRING	"SMB SERVER"
#define KSMBD_CONF_DEFAULT_WORK_GROUP		"WORKGROUP"

#define KSMBD_CONF_DEFAULT_GUEST_ACCOUNT	"nobody"
#define KSMBD_CONF_FALLBACK_GUEST_ACCOUNT	"ftp"

#define KSMBD_CONF_DEFAULT_SESS_CAP	1024
#define KSMBD_CONF_DEFAULT_TPC_PORT	445

#define KSMBD_CONF_FILE_MAX		10000

#define PATH_PWDDB	"/etc/ksmbd/ksmbdpwd.db"
#define PATH_SMBCONF	"/etc/ksmbd/smb.conf"

#define KSMBD_HEALTH_START		(0)
#define KSMBD_HEALTH_RUNNING		(1 << 0)
#define KSMBD_SHOULD_RELOAD_CONFIG	(1 << 1)

extern int ksmbd_health_status;

#define TRACING_DUMP_NL_MSG	0

#define ARRAY_SIZE(X) (sizeof(X) / sizeof((X)[0]))

//---------------------------------------------------------------//
#define LOGAPP		"[%s/%d]:"
#define PRERR		LOGAPP" ERROR: "
#define PRINF		LOGAPP" INFO: "
#define PRDEBUG		LOGAPP" DEBUG: "

#define PR_ERROR	0
#define PR_INFO		1
#define PR_DEBUG	2

static int log_level = PR_DEBUG;

#define PR_LOGGER_STDIO         0
#define PR_LOGGER_SYSLOG        1

/*
 * A thread-safe strerror() wrapper, uses static TLS buffer.
 * NOTE: a signal handler can concurrent modify the buffer,
 * but the buffer should always be nul-terminated.
 */
char *strerr(int err);

__attribute__((format(printf, 2, 3)))
extern void __pr_log(int level, const char *fmt, ...);
extern void set_logger_app_name(const char *an);
extern const char *get_logger_app_name(void);
extern void pr_logger_init(int flags);

#define pr_log(l, f, ...)						\
	do {								\
		if ((l) <= log_level)					\
			__pr_log((l), (f), get_logger_app_name(),	\
					getpid(),			\
					##__VA_ARGS__);			\
	} while (0)
#ifdef HAVE_DEBUG
#define pr_debug(f, ...)	\
	pr_log(PR_DEBUG, PRDEBUG f, ##__VA_ARGS__)
#else
#define pr_debug(f, ...) do {} while (0)
#endif
#define pr_info(f, ...)	\
	pr_log(PR_INFO, PRINF f, ##__VA_ARGS__)
#define pr_err(f, ...)	\
	pr_log(PR_ERROR, PRERR f, ##__VA_ARGS__)

//---------------------------------------------------------------//

void pr_hex_dump(const void *mem, size_t sz);

char *base64_encode(unsigned char *src, size_t srclen);
unsigned char *base64_decode(char const *src, size_t *dstlen);

char *ksmbd_gconvert(const char *str, size_t str_len, int to_codeset, int from_codeset, size_t *bytes_read, size_t *bytes_written);

enum charset_idx {
	KSMBD_CHARSET_UTF8 = 0,
	KSMBD_CHARSET_UTF16LE,
	KSMBD_CHARSET_UCS2LE,
	KSMBD_CHARSET_UTF16BE,
	KSMBD_CHARSET_UCS2BE,
	KSMBD_CHARSET_MAX = 5,
};

#define KSMBD_CHARSET_DEFAULT		KSMBD_CHARSET_UTF8

extern char *ksmbd_conv_charsets[KSMBD_CHARSET_MAX + 1];

void notify_ksmbd_daemon(void);
void terminate_ksmbd_daemon(void);
int test_file_access(char *conf);

#endif				/* __KSMBDTOOLS_H__ */
