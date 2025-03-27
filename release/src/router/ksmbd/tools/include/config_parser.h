/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 *   Copyright (C) 2018 Samsung Electronics Co., Ltd.
 *
 *   linux-cifsd-devel@lists.sourceforge.net
 */

#ifndef __KSMBD_CONFIG_H__
#define __KSMBD_CONFIG_H__

#include <ksmbdtools.h>

struct smbconf_group {
	char			*name;
	struct LIST		*kv;
};

struct smbconf_parser {
	struct LIST		*groups;
	struct smbconf_group	*current;
};

extern struct smbconf_parser parser;

int cp_parse_external_smbconf_group(char *name, char *opts);
int cp_smbconfig_hash_create(const char *smbconf);
void cp_smbconfig_destroy(void);

int cp_parse_pwddb(const char *pwddb);
int cp_parse_smbconf(const char *smbconf);
int cp_parse_reload_smbconf(const char *smbconf);
int cp_parse_subauth(const char *subauth_path);

char *cp_ltrim(char *v);
int cp_key_cmp(char *k, char *v);
char *cp_get_group_kv_string(char *v);
int cp_get_group_kv_bool(char *v);
unsigned long cp_get_group_kv_long_base(char *v, int base);
unsigned long cp_get_group_kv_long(char *v);
int cp_get_group_kv_config_opt(char *v);
char **cp_get_group_kv_list(char *v);
void cp_group_kv_list_free(char **list);

#endif /* __KSMBD_CONFIG_H__ */
