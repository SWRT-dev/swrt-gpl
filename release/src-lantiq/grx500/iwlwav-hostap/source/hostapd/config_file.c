/*
 * hostapd / Configuration file parser
 * Copyright (c) 2003-2018, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#ifndef CONFIG_NATIVE_WINDOWS
#include <grp.h>
#endif /* CONFIG_NATIVE_WINDOWS */

#include "utils/common.h"
#include "utils/uuid.h"
#include "common/ieee802_11_defs.h"
#include "common/sae.h"
#include "crypto/sha256.h"
#include "crypto/tls.h"
#include "drivers/driver.h"
#include "eap_server/eap.h"
#include "radius/radius_client.h"
#include "ap/wpa_auth.h"
#include "ap/ap_config.h"
#include "config_file.h"
#include "utils/crc32.h"
#include "ap/atf.h"

#ifndef CONFIG_NO_VLAN
static int hostapd_config_read_vlan_file(struct hostapd_bss_config *bss,
					 const char *fname)
{
	FILE *f;
	char buf[128], *pos, *pos2, *pos3;
	int line = 0, vlan_id;
	struct hostapd_vlan *vlan;

	f = fopen(fname, "r");
	if (!f) {
		wpa_printf(MSG_ERROR, "VLAN file '%s' not readable.", fname);
		return -1;
	}

	while (fgets(buf, sizeof(buf), f)) {
		line++;

		if (buf[0] == '#')
			continue;
		pos = buf;
		while (*pos != '\0') {
			if (*pos == '\n') {
				*pos = '\0';
				break;
			}
			pos++;
		}
		if (buf[0] == '\0')
			continue;

		if (buf[0] == '*') {
			vlan_id = VLAN_ID_WILDCARD;
			pos = buf + 1;
		} else {
			vlan_id = strtol(buf, &pos, 10);
			if (buf == pos || vlan_id < 1 ||
			    vlan_id > MAX_VLAN_ID) {
				wpa_printf(MSG_ERROR, "Invalid VLAN ID at "
					   "line %d in '%s'", line, fname);
				fclose(f);
				return -1;
			}
		}

		while (*pos == ' ' || *pos == '\t')
			pos++;
		pos2 = pos;
		while (*pos2 != ' ' && *pos2 != '\t' && *pos2 != '\0')
			pos2++;

		if (*pos2 != '\0')
			*(pos2++) = '\0';

		if (*pos == '\0' || os_strlen(pos) > IFNAMSIZ) {
			wpa_printf(MSG_ERROR, "Invalid VLAN ifname at line %d "
				   "in '%s'", line, fname);
			fclose(f);
			return -1;
		}

		while (*pos2 == ' ' || *pos2 == '\t')
			pos2++;
		pos3 = pos2;
		while (*pos3 != ' ' && *pos3 != '\t' && *pos3 != '\0')
			pos3++;
		*pos3 = '\0';

		vlan = os_zalloc(sizeof(*vlan));
		if (vlan == NULL) {
			wpa_printf(MSG_ERROR, "Out of memory while reading "
				   "VLAN interfaces from '%s'", fname);
			fclose(f);
			return -1;
		}

		vlan->vlan_id = vlan_id;
		vlan->vlan_desc.untagged = vlan_id;
		vlan->vlan_desc.notempty = !!vlan_id;
		os_strlcpy(vlan->ifname, pos, sizeof(vlan->ifname));
		os_strlcpy(vlan->bridge, pos2, sizeof(vlan->bridge));
		vlan->next = bss->vlan;
		bss->vlan = vlan;
	}

	fclose(f);

	return 0;
}
#endif /* CONFIG_NO_VLAN */


int hostapd_acl_comp(const void *a, const void *b)
{
	const struct mac_acl_entry *aa = a;
	const struct mac_acl_entry *bb = b;
	return os_memcmp(aa->addr, bb->addr, sizeof(macaddr));
}

int hostapd_soft_block_acl_comp(const void *a, const void *b)
{
	const struct soft_block_acl_entry * const *aa = a;
	const struct soft_block_acl_entry * const *bb = b;
	return os_memcmp((*aa)->addr, (*bb)->addr, sizeof(macaddr));
}

int hostapd_add_acl_maclist(struct mac_acl_entry **acl, int *num,
			    int vlan_id, const u8 *addr)
{
	struct mac_acl_entry *newacl;

	newacl = os_realloc_array(*acl, *num + 1, sizeof(**acl));
	if (!newacl) {
		wpa_printf(MSG_ERROR, "MAC list reallocation failed");
		return -1;
	}

	*acl = newacl;
	os_memcpy((*acl)[*num].addr, addr, ETH_ALEN);
	os_memset(&(*acl)[*num].vlan_id, 0, sizeof((*acl)[*num].vlan_id));
	(*acl)[*num].vlan_id.untagged = vlan_id;
	(*acl)[*num].vlan_id.notempty = !!vlan_id;
	(*num)++;

	return 0;
}


void hostapd_remove_acl_mac(struct mac_acl_entry **acl, int *num,
			    const u8 *addr)
{
	int i = 0;

	while (i < *num) {
		if (os_memcmp((*acl)[i].addr, addr, ETH_ALEN) == 0) {
			os_remove_in_array(*acl, *num, sizeof(**acl), i);
			(*num)--;
		} else {
			i++;
		}
	}
}


static int hostapd_config_read_maclist(const char *fname,
				       struct mac_acl_entry **acl, int *num)
{
	FILE *f;
	char buf[128], *pos;
	int line = 0;
	u8 addr[ETH_ALEN];
	int vlan_id;

	f = fopen(fname, "r");
	if (!f) {
		wpa_printf(MSG_ERROR, "MAC list file '%s' not found.", fname);
		return -1;
	}

	while (fgets(buf, sizeof(buf), f)) {
		int rem = 0;

		line++;

		if (buf[0] == '#')
			continue;
		pos = buf;
		while (*pos != '\0') {
			if (*pos == '\n') {
				*pos = '\0';
				break;
			}
			pos++;
		}
		if (buf[0] == '\0')
			continue;
		pos = buf;
		if (buf[0] == '-') {
			rem = 1;
			pos++;
		}

		if (hwaddr_aton(pos, addr)) {
			wpa_printf(MSG_ERROR, "Invalid MAC address '%s' at "
				   "line %d in '%s'", pos, line, fname);
			fclose(f);
			return -1;
		}

		if (rem) {
			hostapd_remove_acl_mac(acl, num, addr);
			continue;
		}
		vlan_id = 0;
		pos = buf;
		while (*pos != '\0' && *pos != ' ' && *pos != '\t')
			pos++;
		while (*pos == ' ' || *pos == '\t')
			pos++;
		if (*pos != '\0')
			vlan_id = atoi(pos);

		if (hostapd_add_acl_maclist(acl, num, vlan_id, addr) < 0) {
			fclose(f);
			return -1;
		}
	}

	fclose(f);

	if (*acl)
		qsort(*acl, *num, sizeof(**acl), hostapd_acl_comp);

	return 0;
}

/* Soft Block ACL */
int hostapd_soft_block_add_acl_maclist(struct soft_block_acl_db *acl, const u8 *addr)
{
	struct soft_block_acl_entry *newacl;
	struct soft_block_acl_entry **newindexes;

	if (acl->num == 0) {
		dl_list_init(&acl->acls);
	}

	newacl = os_malloc(sizeof(struct soft_block_acl_entry));
	if (!newacl) {
		wpa_printf(MSG_ERROR, "Soft Block MAC new entry allocation failed");
		return -1;
	}

	newindexes = os_realloc_array(acl->indexes, acl->num + 1, sizeof(acl->indexes[0]));
	if (!newindexes) {
		free(newacl);
		wpa_printf(MSG_ERROR, "Soft Block MAC list reallocation failed");
		return -1;
	}

	acl->indexes = newindexes;
	memcpy_s(newacl->addr,ETH_ALEN, addr, ETH_ALEN);
	newacl->wait_handler = NULL;
	newacl->allow_handler = NULL;
	newacl->eloop_ctx = NULL;
	newacl->user_ctx = NULL;
	DL_LIST_ADD(&acl->acls, newacl, list);
	acl->indexes[acl->num] = newacl;
	acl->num++;

	return 0;
}

static void hostapd_softblock_cancel_timeout(eloop_timeout_handler handler, void *eloop_ctx, void *user_ctx)
{
	if (handler) {
		if (eloop_is_timeout_registered(handler, eloop_ctx, user_ctx)) {
			eloop_cancel_timeout(handler, eloop_ctx, user_ctx);
		}
	}
}

void hostapd_soft_block_remove_acl_mac(struct soft_block_acl_db *acl, const u8 *addr)
{
	int i = 0;

	while (i < acl->num) {
		if (os_memcmp(acl->indexes[i]->addr, addr, ETH_ALEN) == 0) {
			hostapd_softblock_cancel_timeout(acl->indexes[i]->wait_handler, acl->indexes[i]->eloop_ctx, acl->indexes[i]->user_ctx);
			hostapd_softblock_cancel_timeout(acl->indexes[i]->allow_handler, acl->indexes[i]->eloop_ctx, acl->indexes[i]->user_ctx);
			dl_list_del(&acl->indexes[i]->list);
			os_free(acl->indexes[i]);
			os_remove_in_array(acl->indexes, acl->num, sizeof(acl->indexes[0]), i);
			acl->num--;
		} else {
			i++;
		}
	}
}

void hostapd_soft_block_acl_clear_list(struct soft_block_acl_db *acl)
{
	int i;

	for (i = 0; i < acl->num; i++) {
		hostapd_softblock_cancel_timeout(acl->indexes[i]->wait_handler, acl->indexes[i]->eloop_ctx, acl->indexes[i]->user_ctx);
		hostapd_softblock_cancel_timeout(acl->indexes[i]->allow_handler, acl->indexes[i]->eloop_ctx, acl->indexes[i]->user_ctx);
		os_free(acl->indexes[i]);
	}
	os_free(acl->indexes);
	acl->indexes = NULL;
	acl->num = 0;
	dl_list_init(&acl->acls);
}

#ifdef EAP_SERVER
#ifdef CONFIG_ALLOW_HOSTAPD
static int hostapd_config_eap_user_salted(struct hostapd_eap_user *user,
					  const char *hash, size_t len,
					  char **pos, int line,
					  const char *fname)
{
	char *pos2 = *pos;

	while (*pos2 != '\0' && *pos2 != ' ' && *pos2 != '\t' && *pos2 != '#')
		pos2++;

	if (pos2 - *pos < (int) (2 * (len + 1))) { /* at least 1 byte of salt */
		wpa_printf(MSG_ERROR,
			   "Invalid salted %s hash on line %d in '%s'",
			   hash, line, fname);
		return -1;
	}

	user->password = os_malloc(len);
	if (!user->password) {
		wpa_printf(MSG_ERROR,
			   "Failed to allocate memory for salted %s hash",
			   hash);
		return -1;
	}

	if (hexstr2bin(*pos, user->password, len) < 0) {
		wpa_printf(MSG_ERROR,
			   "Invalid salted password on line %d in '%s'",
			   line, fname);
		return -1;
	}
	user->password_len = len;
	*pos += 2 * len;

	user->salt_len = (pos2 - *pos) / 2;
	user->salt = os_malloc(user->salt_len);
	if (!user->salt) {
		wpa_printf(MSG_ERROR,
			   "Failed to allocate memory for salted %s hash",
			   hash);
		return -1;
	}

	if (hexstr2bin(*pos, user->salt, user->salt_len) < 0) {
		wpa_printf(MSG_ERROR,
			   "Invalid salt for password on line %d in '%s'",
			   line, fname);
		return -1;
	}

	*pos = pos2;
	return 0;
}


static int hostapd_config_read_eap_user(const char *fname,
					struct hostapd_bss_config *conf)
{
	FILE *f;
	char buf[512], *pos, *start, *pos2;
	int line = 0, ret = 0, num_methods;
	struct hostapd_eap_user *user = NULL, *tail = NULL, *new_user = NULL;

	if (os_strncmp(fname, "sqlite:", 7) == 0) {
#ifdef CONFIG_SQLITE
		os_free(conf->eap_user_sqlite);
		conf->eap_user_sqlite = os_strdup(fname + 7);
		return 0;
#else /* CONFIG_SQLITE */
		wpa_printf(MSG_ERROR,
			   "EAP user file in SQLite DB, but CONFIG_SQLITE was not enabled in the build.");
		return -1;
#endif /* CONFIG_SQLITE */
	}

	f = fopen(fname, "r");
	if (!f) {
		wpa_printf(MSG_ERROR, "EAP user file '%s' not found.", fname);
		return -1;
	}

	/* Lines: "user" METHOD,METHOD2 "password" (password optional) */
	while (fgets(buf, sizeof(buf), f)) {
		line++;

		if (buf[0] == '#')
			continue;
		pos = buf;
		while (*pos != '\0') {
			if (*pos == '\n') {
				*pos = '\0';
				break;
			}
			pos++;
		}
		if (buf[0] == '\0')
			continue;

#ifndef CONFIG_NO_RADIUS
		if (user && os_strncmp(buf, "radius_accept_attr=", 19) == 0) {
			struct hostapd_radius_attr *attr, *a;
			attr = hostapd_parse_radius_attr(buf + 19);
			if (attr == NULL) {
				wpa_printf(MSG_ERROR, "Invalid radius_auth_req_attr: %s",
					   buf + 19);
				user = NULL; /* already in the BSS list */
				goto failed;
			}
			if (user->accept_attr == NULL) {
				user->accept_attr = attr;
			} else {
				a = user->accept_attr;
				while (a->next)
					a = a->next;
				a->next = attr;
			}
			continue;
		}
#endif /* CONFIG_NO_RADIUS */

		user = NULL;

		if (buf[0] != '"' && buf[0] != '*') {
			wpa_printf(MSG_ERROR, "Invalid EAP identity (no \" in "
				   "start) on line %d in '%s'", line, fname);
			goto failed;
		}

		user = os_zalloc(sizeof(*user));
		if (user == NULL) {
			wpa_printf(MSG_ERROR, "EAP user allocation failed");
			goto failed;
		}
		user->force_version = -1;

		if (buf[0] == '*') {
			pos = buf;
		} else {
			pos = buf + 1;
			start = pos;
			while (*pos != '"' && *pos != '\0')
				pos++;
			if (*pos == '\0') {
				wpa_printf(MSG_ERROR, "Invalid EAP identity "
					   "(no \" in end) on line %d in '%s'",
					   line, fname);
				goto failed;
			}

			user->identity = os_memdup(start, pos - start);
			if (user->identity == NULL) {
				wpa_printf(MSG_ERROR, "Failed to allocate "
					   "memory for EAP identity");
				goto failed;
			}
			user->identity_len = pos - start;

			if (pos[0] == '"' && pos[1] == '*') {
				user->wildcard_prefix = 1;
				pos++;
			}
		}
		pos++;
		while (*pos == ' ' || *pos == '\t')
			pos++;

		if (*pos == '\0') {
			wpa_printf(MSG_ERROR, "No EAP method on line %d in "
				   "'%s'", line, fname);
			goto failed;
		}

		start = pos;
		while (*pos != ' ' && *pos != '\t' && *pos != '\0')
			pos++;
		if (*pos == '\0') {
			pos = NULL;
		} else {
			*pos = '\0';
			pos++;
		}
		num_methods = 0;
		while (*start) {
			char *pos3 = os_strchr(start, ',');
			if (pos3) {
				*pos3++ = '\0';
			}
			user->methods[num_methods].method =
				eap_server_get_type(
					start,
					&user->methods[num_methods].vendor);
			if (user->methods[num_methods].vendor ==
			    EAP_VENDOR_IETF &&
			    user->methods[num_methods].method == EAP_TYPE_NONE)
			{
				if (os_strcmp(start, "TTLS-PAP") == 0) {
					user->ttls_auth |= EAP_TTLS_AUTH_PAP;
					goto skip_eap;
				}
				if (os_strcmp(start, "TTLS-CHAP") == 0) {
					user->ttls_auth |= EAP_TTLS_AUTH_CHAP;
					goto skip_eap;
				}
				if (os_strcmp(start, "TTLS-MSCHAP") == 0) {
					user->ttls_auth |=
						EAP_TTLS_AUTH_MSCHAP;
					goto skip_eap;
				}
				if (os_strcmp(start, "TTLS-MSCHAPV2") == 0) {
					user->ttls_auth |=
						EAP_TTLS_AUTH_MSCHAPV2;
					goto skip_eap;
				}
				if (os_strcmp(start, "MACACL") == 0) {
					user->macacl = 1;
					goto skip_eap;
				}
				wpa_printf(MSG_ERROR, "Unsupported EAP type "
					   "'%s' on line %d in '%s'",
					   start, line, fname);
				goto failed;
			}

			num_methods++;
			if (num_methods >= EAP_MAX_METHODS)
				break;
		skip_eap:
			if (pos3 == NULL)
				break;
			start = pos3;
		}
		if (num_methods == 0 && user->ttls_auth == 0 && !user->macacl) {
			wpa_printf(MSG_ERROR, "No EAP types configured on "
				   "line %d in '%s'", line, fname);
			goto failed;
		}

		if (pos == NULL)
			goto done;

		while (*pos == ' ' || *pos == '\t')
			pos++;
		if (*pos == '\0')
			goto done;

		if (os_strncmp(pos, "[ver=0]", 7) == 0) {
			user->force_version = 0;
			goto done;
		}

		if (os_strncmp(pos, "[ver=1]", 7) == 0) {
			user->force_version = 1;
			goto done;
		}

		if (os_strncmp(pos, "[2]", 3) == 0) {
			user->phase2 = 1;
			goto done;
		}

		if (*pos == '"') {
			pos++;
			start = pos;
			while (*pos != '"' && *pos != '\0')
				pos++;
			if (*pos == '\0') {
				wpa_printf(MSG_ERROR, "Invalid EAP password "
					   "(no \" in end) on line %d in '%s'",
					   line, fname);
				goto failed;
			}

			user->password = os_memdup(start, pos - start);
			if (user->password == NULL) {
				wpa_printf(MSG_ERROR, "Failed to allocate "
					   "memory for EAP password");
				goto failed;
			}
			user->password_len = pos - start;

			pos++;
		} else if (os_strncmp(pos, "hash:", 5) == 0) {
			pos += 5;
			pos2 = pos;
			while (*pos2 != '\0' && *pos2 != ' ' &&
			       *pos2 != '\t' && *pos2 != '#')
				pos2++;
			if (pos2 - pos != 32) {
				wpa_printf(MSG_ERROR, "Invalid password hash "
					   "on line %d in '%s'", line, fname);
				goto failed;
			}
			user->password = os_malloc(16);
			if (user->password == NULL) {
				wpa_printf(MSG_ERROR, "Failed to allocate "
					   "memory for EAP password hash");
				goto failed;
			}
			if (hexstr2bin(pos, user->password, 16) < 0) {
				wpa_printf(MSG_ERROR, "Invalid hash password "
					   "on line %d in '%s'", line, fname);
				goto failed;
			}
			user->password_len = 16;
			user->password_hash = 1;
			pos = pos2;
		} else if (os_strncmp(pos, "ssha1:", 6) == 0) {
			pos += 6;
			if (hostapd_config_eap_user_salted(user, "sha1", 20,
							   &pos,
							   line, fname) < 0)
				goto failed;
		} else if (os_strncmp(pos, "ssha256:", 8) == 0) {
			pos += 8;
			if (hostapd_config_eap_user_salted(user, "sha256", 32,
							   &pos,
							   line, fname) < 0)
				goto failed;
		} else if (os_strncmp(pos, "ssha512:", 8) == 0) {
			pos += 8;
			if (hostapd_config_eap_user_salted(user, "sha512", 64,
							   &pos,
							   line, fname) < 0)
				goto failed;
		} else {
			pos2 = pos;
			while (*pos2 != '\0' && *pos2 != ' ' &&
			       *pos2 != '\t' && *pos2 != '#')
				pos2++;
			if ((pos2 - pos) & 1) {
				wpa_printf(MSG_ERROR, "Invalid hex password "
					   "on line %d in '%s'", line, fname);
				goto failed;
			}
			user->password = os_malloc((pos2 - pos) / 2);
			if (user->password == NULL) {
				wpa_printf(MSG_ERROR, "Failed to allocate "
					   "memory for EAP password");
				goto failed;
			}
			if (hexstr2bin(pos, user->password,
				       (pos2 - pos) / 2) < 0) {
				wpa_printf(MSG_ERROR, "Invalid hex password "
					   "on line %d in '%s'", line, fname);
				goto failed;
			}
			user->password_len = (pos2 - pos) / 2;
			pos = pos2;
		}

		while (*pos == ' ' || *pos == '\t')
			pos++;
		if (os_strncmp(pos, "[2]", 3) == 0) {
			user->phase2 = 1;
		}

	done:
		if (tail == NULL) {
			tail = new_user = user;
		} else {
			tail->next = user;
			tail = user;
		}
		continue;

	failed:
		if (user)
			hostapd_config_free_eap_user(user);
		ret = -1;
		break;
	}

	fclose(f);

	if (ret == 0) {
		hostapd_config_free_eap_users(conf->eap_user);
		conf->eap_user = new_user;
	} else {
		hostapd_config_free_eap_users(new_user);
	}

	return ret;
}
#endif /* CONFIG_ALLOW_HOSTAPD */

#endif /* EAP_SERVER */


#ifndef CONFIG_NO_RADIUS
static int
hostapd_config_read_radius_addr(struct hostapd_radius_server **server,
				int *num_server, const char *val, int def_port,
				struct hostapd_radius_server **curr_serv)
{
	struct hostapd_radius_server *nserv;
	int ret;
	static int server_index = 1;

	nserv = os_realloc_array(*server, *num_server + 1, sizeof(*nserv));
	if (nserv == NULL)
		return -1;

	*server = nserv;
	nserv = &nserv[*num_server];
	(*num_server)++;
	(*curr_serv) = nserv;

	os_memset(nserv, 0, sizeof(*nserv));
	nserv->port = def_port;
	ret = hostapd_parse_ip_addr(val, &nserv->addr);
	nserv->index = server_index++;

	return ret;
}



static int hostapd_parse_das_client(struct hostapd_bss_config *bss, char *val)
{
	char *secret;

	secret = os_strchr(val, ' ');
	if (secret == NULL)
		return -1;

	*secret++ = '\0';

	if (hostapd_parse_ip_addr(val, &bss->radius_das_client_addr))
		return -1;

	os_free(bss->radius_das_shared_secret);
	bss->radius_das_shared_secret = (u8 *) os_strdup(secret);
	if (bss->radius_das_shared_secret == NULL)
		return -1;
	bss->radius_das_shared_secret_len = os_strlen(secret);

	return 0;
}
#endif /* CONFIG_NO_RADIUS */


static int hostapd_config_parse_key_mgmt(int line, const char *value)
{
	int val = 0, last;
	char *start, *end, *buf;

	buf = os_strdup(value);
	if (buf == NULL)
		return -1;
	start = buf;

	while (*start != '\0') {
		while (*start == ' ' || *start == '\t')
			start++;
		if (*start == '\0')
			break;
		end = start;
		while (*end != ' ' && *end != '\t' && *end != '\0')
			end++;
		last = *end == '\0';
		*end = '\0';
		if (os_strcmp(start, "WPA-PSK") == 0)
			val |= WPA_KEY_MGMT_PSK;
		else if (os_strcmp(start, "WPA-EAP") == 0)
			val |= WPA_KEY_MGMT_IEEE8021X;
#ifdef CONFIG_IEEE80211R_AP
		else if (os_strcmp(start, "FT-PSK") == 0)
			val |= WPA_KEY_MGMT_FT_PSK;
		else if (os_strcmp(start, "FT-EAP") == 0)
			val |= WPA_KEY_MGMT_FT_IEEE8021X;
#ifdef CONFIG_SHA384
		else if (os_strcmp(start, "FT-EAP-SHA384") == 0)
			val |= WPA_KEY_MGMT_FT_IEEE8021X_SHA384;
#endif /* CONFIG_SHA384 */
#endif /* CONFIG_IEEE80211R_AP */
		else if (os_strcmp(start, "WPA-PSK-SHA256") == 0)
			val |= WPA_KEY_MGMT_PSK_SHA256;
		else if (os_strcmp(start, "WPA-EAP-SHA256") == 0)
			val |= WPA_KEY_MGMT_IEEE8021X_SHA256;
#ifdef CONFIG_SAE
		else if (os_strcmp(start, "SAE") == 0)
			val |= WPA_KEY_MGMT_SAE;
		else if (os_strcmp(start, "FT-SAE") == 0)
			val |= WPA_KEY_MGMT_FT_SAE;
#endif /* CONFIG_SAE */
#ifdef CONFIG_SUITEB
		else if (os_strcmp(start, "WPA-EAP-SUITE-B") == 0)
			val |= WPA_KEY_MGMT_IEEE8021X_SUITE_B;
#endif /* CONFIG_SUITEB */
#ifdef CONFIG_SUITEB192
		else if (os_strcmp(start, "WPA-EAP-SUITE-B-192") == 0)
			val |= WPA_KEY_MGMT_IEEE8021X_SUITE_B_192;
#endif /* CONFIG_SUITEB192 */
#ifdef CONFIG_FILS
		else if (os_strcmp(start, "FILS-SHA256") == 0)
			val |= WPA_KEY_MGMT_FILS_SHA256;
		else if (os_strcmp(start, "FILS-SHA384") == 0)
			val |= WPA_KEY_MGMT_FILS_SHA384;
#ifdef CONFIG_IEEE80211R_AP
		else if (os_strcmp(start, "FT-FILS-SHA256") == 0)
			val |= WPA_KEY_MGMT_FT_FILS_SHA256;
		else if (os_strcmp(start, "FT-FILS-SHA384") == 0)
			val |= WPA_KEY_MGMT_FT_FILS_SHA384;
#endif /* CONFIG_IEEE80211R_AP */
#endif /* CONFIG_FILS */
#ifdef CONFIG_OWE
		else if (os_strcmp(start, "OWE") == 0)
			val |= WPA_KEY_MGMT_OWE;
#endif /* CONFIG_OWE */
#ifdef CONFIG_DPP
		else if (os_strcmp(start, "DPP") == 0)
			val |= WPA_KEY_MGMT_DPP;
#endif /* CONFIG_DPP */
#ifdef CONFIG_HS20
		else if (os_strcmp(start, "OSEN") == 0)
			val |= WPA_KEY_MGMT_OSEN;
#endif /* CONFIG_HS20 */
		else {
			wpa_printf(MSG_ERROR, "Line %d: invalid key_mgmt '%s'",
				   line, start);
			os_free(buf);
			return -1;
		}

		if (last)
			break;
		start = end + 1;
	}

	os_free(buf);
	if (val == 0) {
		wpa_printf(MSG_ERROR, "Line %d: no key_mgmt values "
			   "configured.", line);
		return -1;
	}

	return val;
}


static int hostapd_config_parse_cipher(int line, const char *value)
{
	int val = wpa_parse_cipher(value);
	if (val < 0) {
		wpa_printf(MSG_ERROR, "Line %d: invalid cipher '%s'.",
			   line, value);
		return -1;
	}
	if (val == 0) {
		wpa_printf(MSG_ERROR, "Line %d: no cipher values configured.",
			   line);
		return -1;
	}
	return val;
}


#ifdef CONFIG_WEP
static int hostapd_config_read_wep(struct hostapd_wep_keys *wep, int keyidx,
				   char *val)
{
	size_t len = os_strlen(val);

	if (len == 0) {
		int i, set = 0;

		bin_clear_free(wep->key[keyidx], wep->len[keyidx]);
		wep->key[keyidx] = NULL;
		wep->len[keyidx] = 0;
		for (i = 0; i < NUM_WEP_KEYS; i++) {
			if (wep->key[i])
				set++;
		}
		if (!set)
			wep->keys_set = 0;
		return 0;
	}

	if (wep->key[keyidx] != NULL)
		return -1;

	if (val[0] == '"') {
		if (len < 2 || val[len - 1] != '"')
			return -1;
		len -= 2;
		wep->key[keyidx] = os_memdup(val + 1, len);
		if (wep->key[keyidx] == NULL)
			return -1;
		wep->len[keyidx] = len;
	} else {
		if (len & 1)
			return -1;
		len /= 2;
		wep->key[keyidx] = os_malloc(len);
		if (wep->key[keyidx] == NULL)
			return -1;
		wep->len[keyidx] = len;
		if (hexstr2bin(val, wep->key[keyidx], len) < 0)
			return -1;
	}

	wep->keys_set++;

	return 0;
}
#endif /* CONFIG_WEP */

static int hostapd_parse_unsolicited_frame_support(struct hostapd_config *conf, char *val)
{
	if (val)
	{
		int ufs = atoi(val);

		if (!IS_VALID_RANGE(ufs, DISABLE_UNSOLICITED_FRAME_SUPPORT, FILS_DISCOVERY_ENABLE))
		{
			wpa_printf(MSG_ERROR, "CONFIG: UPDATE_RNR: Bad unsolicited_frame_support %s"
				" choose 0:Disable, 1:20TU_Probe_response, 2:FILS Discovery", val);
			return -1;
		}
		conf->unsolicited_frame_support = ufs;
	}

	return 0;
}

static int hostapd_parse_unsolicited_frame_duration(struct hostapd_config *conf, char *val)
{
	if ( val )
	{
		int ufd = atoi(val);
		if (!IS_VALID_RANGE(ufd, MIN_UNSOLICITED_FRAME_DURATION + 1, MAX_UNSOLICITED_FRAME_DURATION))
		{
			wpa_printf(MSG_ERROR, "CONFIG: UPDATE_RNR: Bad unsolicited_frame_duration %s"
				" must be >0 and  <=20ms", val);
			return -1;
		}
		conf->unsolicited_frame_duration = ufd;
	}

	return 0;
}

static int hostapd_parse_colocated_6g_vap_info(struct hostapd_config *conf,
		struct hostapd_bss_config *bss, char *val)
{
	char *tmp = val, *ptr = NULL;
	int radio_index = 0, param_val = 0;
	errno_t err = EOK;
	u8 psd_20MHz;

	/* expected order: radio_index BSSID SSID multibss_enable is_transmitted_bssid max_tx_power hidden_mode
		eg: 0 11:22:33:44:55:66 test_6g_vap 1 1 30 0*/
	u8 bssid[ETH_ALEN] = {'\0'};
	u8 empty_bssid[ETH_ALEN] = {'\0'};
	u8 ssid[SSID_MAX_LEN] = {'\0'};
	int j = 0;
	bool found = false; // flag will be used to find whether bssid entry found
	bool multibss_enable = false, is_transmitted_bssid = false, unsolicited_frame_support = false, hidden_mode = false;

	PARSE_TOKEN(tmp, val, " ", &ptr);
	param_val = atoi(tmp);
	if(!IS_VALID_RANGE(param_val, 0, MAX_SUPPORTED_6GHZ_RADIO - 1))
	{
		wpa_printf(MSG_ERROR, "CONFIG: UPDATE_RNR: Bad radio_index %s", tmp);
		return -1;
	}
	radio_index = param_val;


	PARSE_TOKEN(tmp, val, " ", &ptr);
	if (hwaddr_aton(tmp, bssid)) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad  BSSID %s", tmp);
		return -1;
	}

	if( !os_memcmp( bssid, empty_bssid, ETH_ALEN)) /* found place holder for new RNR */
	{
		wpa_printf(MSG_ERROR, "CTRL:UPDATE_RNR: all 0's BSSID, not valid?");
		return -1;
	}
	
	PARSE_TOKEN(tmp, val, " ", &ptr);
	if( strnlen_s(tmp, RSIZE_MAX_STR) >= SSID_MAX_LEN  ) {
		wpa_printf(MSG_ERROR,
		   "CTRL: UPDATE_RNR: Bad SSID %s", tmp);
		return -1;
	}
	err = memcpy_s(ssid, SSID_MAX_LEN, tmp, strnlen_s(tmp, RSIZE_MAX_STR));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return -1;
	}

	
	PARSE_TOKEN(tmp, val, " ", &ptr);
	param_val = atoi(tmp);
	if(!IS_VALID_BOOL_RANGE(param_val)) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad multlbss_enable %s", tmp);
		return -1;
	}
	multibss_enable = param_val;

	PARSE_TOKEN(tmp, val," ", &ptr);
	param_val = atoi(tmp);
	if(!IS_VALID_BOOL_RANGE(param_val)) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad is_transmitted_bssid %s", tmp);
		return -1;
	}
	is_transmitted_bssid = param_val;

	PARSE_TOKEN(tmp, val, " ", &ptr);
	param_val = atoi(tmp);
	if(!IS_VALID_BOOL_RANGE(param_val)) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad unsolicited_frame_support %s", tmp);
		return -1;
	}
	unsolicited_frame_support = param_val;

	PARSE_TOKEN(tmp,val," ",&ptr);
	param_val = ( atoi(tmp) - HOSTAP_6GHZ_10LOG_20MHZ ) * HOSTAP_6GHZ_CONVERT_HALF_DB_UNIT;
	if(!IS_VALID_RANGE(param_val, HOSTAP_6GHZ_PSD_20MHZ_MIN, HOSTAP_6GHZ_PSD_20MHZ_MAX)) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: psd 20MHz %s", tmp);
		return -1;
	}
	if ( param_val < 0 )
		psd_20MHz = param_val + HOSTAPD_BYTE_2S_COMPLEMENT;
	else
		psd_20MHz = param_val;

	PARSE_TOKEN(tmp, val, " ", &ptr);
	param_val = atoi(tmp);
	if( !IS_VALID_BOOL_RANGE(param_val) ) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad hideen_mode %s", tmp);
		return -1;
	}
	hidden_mode = param_val;


	/* Update for existing RNR ? */
	for (  j = 0; ( j < MAX_SUPPORTED_6GHZ_VAP_PER_RADIO ) && (found == false); j++ )
	{
		if( !os_memcmp(conf->coloc_6g_ap_info[radio_index].bss_info[j].bssid, bssid, ETH_ALEN) ) 
		{
			found = true;
			break;
		}
	}
		
	/* not existing RNR, find empty buffer in array and update new RNR */
	if( (found == false) )
	{
		for (  j = 0; ( j < MAX_SUPPORTED_6GHZ_VAP_PER_RADIO ) && (found == false) ; j++ ) 
		{
			if( !os_memcmp( conf->coloc_6g_ap_info[radio_index].bss_info[j].bssid,
					empty_bssid, ETH_ALEN)) /* found place holder for new RNR */
			{
				found = true;
				conf->coloc_6g_ap_info[radio_index].TbttInfoCount++;
				break;
			}
		}
	}

	if((found == false) )
	{
		wpa_printf(MSG_ERROR,"No available space to hold new rnr report");
		return -1;
	}
	err = memcpy_s(conf->coloc_6g_ap_info[radio_index].bss_info[j].Ssid, SSID_MAX_LEN, ssid, strnlen_s((const char *)ssid, SSID_MAX_LEN));
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return -1;
	}
	conf->coloc_6g_ap_info[radio_index].bss_info[j].NeighApTbttOffset = TBTT_OFFSET_UNKNOWN; /* TODO (6GHz) Dynamic, 255=unknown */
	err = memcpy_s(conf->coloc_6g_ap_info[radio_index].bss_info[j].bssid, ETH_ALEN, bssid, ETH_ALEN);
	if (EOK != err) {
		wpa_printf(MSG_ERROR, "%s: memcpy_s error %d", __func__, err);
		return -1;
	}
	/*
	OCT Recommended 0
	Same SSID  x
	Multiple BSSID x
	Transmitted BSSID x
	Member of ESS with 2.4/5GHz Co-located AP(#20024) 0
	20 TU Probe Response Active x
	Co-Located  AP 1
	Reserved 0
	0 1 x 0 x x x 0
	*/
	conf->coloc_6g_ap_info[radio_index].bss_info[j].BssParams |= RNR_TBTT_INFO_BSS_PARAM_COLOC_AP; /*TODO (6GHz) Dynamic */
	if ( unsolicited_frame_support )
	{
		conf->coloc_6g_ap_info[radio_index].bss_info[j].BssParams 
			|= RNR_TBTT_INFO_BSS_PARAM_20TU_PROBE_RES;
	}
	/*
	 * Short SSID calculation is identical to FCS and it is defined in
	 * IEEE802.11-REVmd/D2.2 9.4.2.170.3
	 */
	WPA_PUT_LE32(conf->coloc_6g_ap_info[radio_index].bss_info[j].ShortSsid, crc32(ssid, strnlen_s((const char *)ssid, SSID_MAX_LEN)) );
	if( multibss_enable )
	{ 
		conf->coloc_6g_ap_info[radio_index].bss_info[j].BssParams |=  RNR_TBTT_INFO_BSS_PARAM_MULTIPLE_BSSID;
		if (is_transmitted_bssid)
			conf->coloc_6g_ap_info[radio_index].bss_info[j].BssParams |=  RNR_TBTT_INFO_BSS_PARAM_TRANSMITTED_BSSID;
	}
	
	if( ( bss->ssid.ssid_len ) && ( strnlen_s((char *)ssid, SSID_MAX_LEN) == bss->ssid.ssid_len ) &&
		( !os_memcmp( bss->ssid.ssid, ssid, bss->ssid.ssid_len ) ) && !hidden_mode )
	{ 
		wpa_printf(MSG_DEBUG, "CTRL: UPDATE_RNR: Same ssid field set in rnr");
		conf->coloc_6g_ap_info[radio_index].bss_info[j].BssParams |= RNR_TBTT_INFO_BSS_PARAM_SAME_SSID;
	}
	else
		wpa_printf(MSG_DEBUG, "CTRL: UPDATE_RNR: Same ssid field not set in rnr");

	conf->coloc_6g_ap_info[radio_index].bss_info[j].psd_20MHz = psd_20MHz;
	return 0;
}

static int hostapd_parse_colocated_6g_radio_info(struct hostapd_config *conf, char *val)
{
	char *tmp = val, *ptr = NULL;
	int radio_index = 0, param_val = 0;
	
	/* expected order: radio_index OperatingClass Channel, eg: 0 131 1*/ 

	PARSE_TOKEN(tmp, val, " ", &ptr);
	param_val = atoi(tmp);
	if(!IS_VALID_RANGE(param_val, 0, MAX_SUPPORTED_6GHZ_RADIO - 1)) {
			wpa_printf(MSG_ERROR, "CONFIG: UPDATE_RNR: Bad radio_index %s", tmp);
			return -1;
	}
	radio_index = param_val;
	
	PARSE_TOKEN(tmp, val, " ", &ptr);
	param_val = atoi(tmp);
	if(!IS_VALID_RANGE(param_val, HOSTAP_6GHZ_20MHZ_OP_CLASS, HOSTAP_6GHZ_80P80MHZ_OP_CLASS)) {
		wpa_printf(MSG_ERROR, "CONFIG: UPDATE_RNR: Bad OperatingClass %s", tmp);
		return -1;
	}
	conf->coloc_6g_ap_info[radio_index].OperatingClass = param_val;

	PARSE_TOKEN(tmp,val," ",&ptr);
	param_val = atoi(tmp);
	if(!IS_VALID_RANGE(param_val, HOSTAP_6GHZ_CHANNEL_FIRST, HOSTAP_6GHZ_CHANNEL_LAST)) {
		wpa_printf(MSG_ERROR, "CTRL: UPDATE_RNR: Bad Channel %s", tmp);
		return -1;
	}
	conf->coloc_6g_ap_info[radio_index].ChanNum = (u8)param_val;
	conf->coloc_6g_ap_info[radio_index].Reserved = 0;
	conf->coloc_6g_ap_info[radio_index].TbttInfoFieldType = 0; /* TODO (6GHz) Dynamic , possible 0,1,2,3 ( 1,2,3 are reserverd ) */
	
	return 0;
}

static int hostapd_parse_chanlist(struct hostapd_config *conf, char *val)
{
	char *pos;

	/* for backwards compatibility, translate ' ' in conf str to ',' */
	pos = val;
	while (pos) {
		pos = os_strchr(pos, ' ');
		if (pos)
			*pos++ = ',';
	}
	if (freq_range_list_parse(&conf->acs_ch_list, val))
		return -1;

	return 0;
}

static int hostapd_intlist_size(const char *val)
{
	int count = 0;
	const char *pos = val;

	while (*pos != '\0') {
		if (*pos == ' ')
			count++;
		pos++;
	}
	return count + 1;
}

static int hostapd_parse_intlist(int **int_list, char *val)
{
	int *list;
	int count;
	char *pos, *end;

	os_free(*int_list);
	*int_list = NULL;

	pos = val;
	count = 0;
	while (*pos != '\0') {
		if (*pos == ' ')
			count++;
		pos++;
	}

	list = os_malloc(sizeof(int) * (count + 2));
	if (list == NULL)
		return -1;
	pos = val;
	count = 0;
	while (*pos != '\0') {
		end = os_strchr(pos, ' ');
		if (end)
			*end = '\0'; /* WARN: input buf is modified */

		list[count++] = atoi(pos);
		if (!end)
			break;
		pos = end + 1;
	}
	list[count] = -1;

	*int_list = list;
	return 0;
}

static int hostapd_fill_intlist(int *int_list, char *val, int exp_count)
{
	int count;
	char *pos, *end;

	pos = val;
	count = 0;
	while (*pos != '\0') {
		if (*pos == ' ')
			count++;
		pos++;
	}

	if (exp_count != (count + 1))
		return -1;

	pos = val;
	count = 0;
	while (*pos != '\0') {
		end = os_strchr(pos, ' ');
		if (end)
			*end = '\0';

		int_list[count++] = atoi(pos);
		if (!end)
			break;
		pos = end + 1;
	}
	return 0;
}


static int hostapd_config_bss(struct hostapd_config *conf, const char *ifname)
{
	struct hostapd_bss_config **all, *bss;

	if (*ifname == '\0')
		return -1;

	all = os_realloc_array(conf->bss, conf->num_bss + 1,
			       sizeof(struct hostapd_bss_config *));
	if (all == NULL) {
		wpa_printf(MSG_ERROR, "Failed to allocate memory for "
			   "multi-BSS entry");
		return -1;
	}
	conf->bss = all;

	bss = os_zalloc(sizeof(*bss));
	if (bss == NULL)
		return -1;
	bss->radius = os_zalloc(sizeof(*bss->radius));
	if (bss->radius == NULL) {
		wpa_printf(MSG_ERROR, "Failed to allocate memory for "
			   "multi-BSS RADIUS data");
		os_free(bss);
		return -1;
	}

	bss->bss_idx = conf->num_bss++; /* Store BSS index */
	conf->bss[bss->bss_idx] = bss;
	conf->last_bss = bss;

	hostapd_config_defaults_bss(bss);



	os_strlcpy(bss->iface, ifname, sizeof(bss->iface));
	os_memcpy(bss->ssid.vlan, bss->iface, IFNAMSIZ + 1);

	return 0;
}


#ifdef CONFIG_IEEE80211R_AP

static int rkh_derive_key(const char *pos, u8 *key, size_t key_len)
{
	u8 oldkey[16];
	int ret;

	if (!hexstr2bin(pos, key, key_len))
		return 0;

	/* Try to use old short key for backwards compatibility */
	if (hexstr2bin(pos, oldkey, sizeof(oldkey)))
		return -1;

	ret = hmac_sha256_kdf(oldkey, sizeof(oldkey), "FT OLDKEY", NULL, 0,
			      key, key_len);
	os_memset(oldkey, 0, sizeof(oldkey));
	return ret;
}


static int add_r0kh(struct hostapd_bss_config *bss, char *value)
{
	struct ft_remote_r0kh *r0kh;
	char *pos, *next;

	r0kh = os_zalloc(sizeof(*r0kh));
	if (r0kh == NULL)
		return -1;

	/* 02:01:02:03:04:05 a.example.com 000102030405060708090a0b0c0d0e0f */
	pos = value;
	next = os_strchr(pos, ' ');
	if (next)
		*next++ = '\0';
	if (next == NULL || hwaddr_aton(pos, r0kh->addr)) {
		wpa_printf(MSG_ERROR, "Invalid R0KH MAC address: '%s'", pos);
		os_free(r0kh);
		return -1;
	}

	pos = next;
	next = os_strchr(pos, ' ');
	if (next)
		*next++ = '\0';
	if (next == NULL || next - pos > FT_R0KH_ID_MAX_LEN) {
		wpa_printf(MSG_ERROR, "Invalid R0KH-ID: '%s'", pos);
		os_free(r0kh);
		return -1;
	}
	r0kh->id_len = next - pos - 1;
	os_memcpy(r0kh->id, pos, r0kh->id_len);

	pos = next;
	if (rkh_derive_key(pos, r0kh->key, sizeof(r0kh->key)) < 0) {
		wpa_printf(MSG_ERROR, "Invalid R0KH key: '%s'", pos);
		os_free(r0kh);
		return -1;
	}

	r0kh->next = bss->r0kh_list;
	bss->r0kh_list = r0kh;

	return 0;
}


static int add_r1kh(struct hostapd_bss_config *bss, char *value)
{
	struct ft_remote_r1kh *r1kh;
	char *pos, *next;

	r1kh = os_zalloc(sizeof(*r1kh));
	if (r1kh == NULL)
		return -1;

	/* 02:01:02:03:04:05 02:01:02:03:04:05
	 * 000102030405060708090a0b0c0d0e0f */
	pos = value;
	next = os_strchr(pos, ' ');
	if (next)
		*next++ = '\0';
	if (next == NULL || hwaddr_aton(pos, r1kh->addr)) {
		wpa_printf(MSG_ERROR, "Invalid R1KH MAC address: '%s'", pos);
		os_free(r1kh);
		return -1;
	}

	pos = next;
	next = os_strchr(pos, ' ');
	if (next)
		*next++ = '\0';
	if (next == NULL || hwaddr_aton(pos, r1kh->id)) {
		wpa_printf(MSG_ERROR, "Invalid R1KH-ID: '%s'", pos);
		os_free(r1kh);
		return -1;
	}

	pos = next;
	if (rkh_derive_key(pos, r1kh->key, sizeof(r1kh->key)) < 0) {
		wpa_printf(MSG_ERROR, "Invalid R1KH key: '%s'", pos);
		os_free(r1kh);
		return -1;
	}

	r1kh->next = bss->r1kh_list;
	bss->r1kh_list = r1kh;

	return 0;
}
#endif /* CONFIG_IEEE80211R_AP */


static int hostapd_config_ht_capab(struct hostapd_config *conf,
				   const char *capab)
{
	if (os_strstr(capab, "[LDPC]"))
		conf->ht_capab |= HT_CAP_INFO_LDPC_CODING_CAP;
	if (os_strstr(capab, "[HT40-]")) {
		conf->ht_capab |= HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET;
		if (!conf->secondary_channel_set)
			conf->secondary_channel = -1;
	}
	if (os_strstr(capab, "[HT40+]")) {
		conf->ht_capab |= HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET;
		if (!conf->secondary_channel_set)
			conf->secondary_channel = 1;
	}
	if (os_strstr(capab, "[HT40+]") && os_strstr(capab, "[HT40-]")) {
		conf->ht_capab |= HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET;
		conf->ht40_plus_minus_allowed = 1;
	}
	if (!os_strstr(capab, "[HT40+]") && !os_strstr(capab, "[HT40-]"))
		conf->secondary_channel = 0;
	if (os_strstr(capab, "[GF]"))
		conf->ht_capab |= HT_CAP_INFO_GREEN_FIELD;
	if (os_strstr(capab, "[SHORT-GI-20]"))
		conf->ht_capab |= HT_CAP_INFO_SHORT_GI20MHZ;
	if (os_strstr(capab, "[SHORT-GI-40]"))
		conf->ht_capab |= HT_CAP_INFO_SHORT_GI40MHZ;
	if (os_strstr(capab, "[TX-STBC]"))
		conf->ht_capab |= HT_CAP_INFO_TX_STBC;
	if (os_strstr(capab, "[RX-STBC1]")) {
		conf->ht_capab &= ~HT_CAP_INFO_RX_STBC_MASK;
		conf->ht_capab |= HT_CAP_INFO_RX_STBC_1;
	}
	if (os_strstr(capab, "[RX-STBC12]")) {
		conf->ht_capab &= ~HT_CAP_INFO_RX_STBC_MASK;
		conf->ht_capab |= HT_CAP_INFO_RX_STBC_12;
	}
	if (os_strstr(capab, "[RX-STBC123]")) {
		conf->ht_capab &= ~HT_CAP_INFO_RX_STBC_MASK;
		conf->ht_capab |= HT_CAP_INFO_RX_STBC_123;
	}
	if (os_strstr(capab, "[DELAYED-BA]"))
		conf->ht_capab |= HT_CAP_INFO_DELAYED_BA;
	if (os_strstr(capab, "[MAX-AMSDU-7935]"))
		conf->ht_capab |= HT_CAP_INFO_MAX_AMSDU_SIZE;
	if (os_strstr(capab, "[DSSS_CCK-40]"))
		conf->ht_capab |= HT_CAP_INFO_DSSS_CCK40MHZ;
	if (os_strstr(capab, "[40-INTOLERANT]"))
		conf->ht_capab |= HT_CAP_INFO_40MHZ_INTOLERANT;
	if (os_strstr(capab, "[LSIG-TXOP-PROT]"))
		conf->ht_capab |= HT_CAP_INFO_LSIG_TXOP_PROTECT_SUPPORT;

	return 0;
}


static int hostapd_config_ht_tx_bf_capab(struct hostapd_config *conf,
				   const char *capab)
{
	if (conf->ht_tx_bf_capab_from_hw) {
		wpa_printf(MSG_DEBUG, "Use HT TX_BF Capability from HW\n");
		return 0;
	}

	if (os_strstr(capab, "[IMPL-TXBF-RX]"))
		conf->ht_tx_bf_capab |= TX_BF_CAP_IMPLICIT_TXBF_RX_CAP;
	if (os_strstr(capab, "[RX-STAG-SND]"))
		conf->ht_tx_bf_capab |= TX_BF_CAP_RX_STAGGERED_SOUNDING_CAP;
	if (os_strstr(capab, "[TX-STAG-SND]"))
		conf->ht_tx_bf_capab |= TX_BF_CAP_TX_STAGGERED_SOUNDING_CAP;
	if (os_strstr(capab, "[RX-NDP]"))
		conf->ht_tx_bf_capab |= TX_BF_CAP_RX_NDP_CAP;
	if (os_strstr(capab, "[TX-NDP]"))
		conf->ht_tx_bf_capab |= TX_BF_CAP_TX_NDP_CAP;
	if (os_strstr(capab, "[IMPL-TXBF]"))
		conf->ht_tx_bf_capab |= TX_BF_CAP_IMPLICIT_TX_BF_CAP;

	if (os_strstr(capab, "[CALIB-RESP]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_CALIBRATION_MASK;
		conf->ht_tx_bf_capab |= (u32)(1 << TX_BF_CAP_CALIB_OFFSET);
	}
	if (os_strstr(capab, "[CALIB-RESP+REQ]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_CALIBRATION_MASK;
		conf->ht_tx_bf_capab |= (u32)(3 << TX_BF_CAP_CALIB_OFFSET);
	}

	if (os_strstr(capab, "[EXPL-CSI-TXBF]"))
		conf->ht_tx_bf_capab |= TX_BF_CAP_EXPLICIT_CSI_TXBF_CAP;
	if (os_strstr(capab, "[EXPL-NONCOMPR-STEER]"))
		conf->ht_tx_bf_capab |= TX_BF_CAP_EXPLICIT_NONCOMPR_STEERING_CAP;
	if (os_strstr(capab, "[EXPL-COMPR-STEER]"))
		conf->ht_tx_bf_capab |= TX_BF_CAP_EXPLICIT_COMPR_STEERING_CAP;


	if (os_strstr(capab, "[EXPL-TXBF-CSI-FBACK-DELAYED]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_EXPLICIT_TX_BF_CSI_FEEDBACK_MASK;
		conf->ht_tx_bf_capab |= (u32)(1 << TX_BF_CAP_EXPLICIT_TX_BF_CSI_FEEDBACK_OFFSET);
	}
	if (os_strstr(capab, "[EXPL-TXBF-CSI-FBACK-IMM]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_EXPLICIT_TX_BF_CSI_FEEDBACK_MASK;
		conf->ht_tx_bf_capab |= (u32)(2 << TX_BF_CAP_EXPLICIT_TX_BF_CSI_FEEDBACK_OFFSET);
	}
	if (os_strstr(capab, "[EXPL-TXBF-CSI-FBACK-DELAYED+IMM]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_EXPLICIT_TX_BF_CSI_FEEDBACK_MASK;
		conf->ht_tx_bf_capab |= (u32)(3 << TX_BF_CAP_EXPLICIT_TX_BF_CSI_FEEDBACK_OFFSET);
	}


	if (os_strstr(capab, "[EXPL-NONCOMPR-FB-FBACK-DELAYED]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_EXPLICIT_UNCOMPR_STEERING_MATRIX_FEEDBACK_MASK;
		conf->ht_tx_bf_capab |= (u32)(1 << TX_BF_CAP_EXPLICIT_UNCOMPR_STEERING_MATRIX_FEEDBACK_OFFSET);
	}
	if (os_strstr(capab, "[EXPL-NONCOMPR-FB-FBACK-IMM]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_EXPLICIT_UNCOMPR_STEERING_MATRIX_FEEDBACK_MASK;
		conf->ht_tx_bf_capab |= (u32)(2 << TX_BF_CAP_EXPLICIT_UNCOMPR_STEERING_MATRIX_FEEDBACK_OFFSET);
	}
	if (os_strstr(capab, "[EXPL-NONCOMPR-FB-FBACK-DELAYED+IMM]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_EXPLICIT_UNCOMPR_STEERING_MATRIX_FEEDBACK_MASK;
		conf->ht_tx_bf_capab |= (u32)(3 << TX_BF_CAP_EXPLICIT_UNCOMPR_STEERING_MATRIX_FEEDBACK_OFFSET);
	}

	if (os_strstr(capab, "[EXPL-COMPR-FB-FBACK-DELAYED]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_EXPLICIT_COMPRESSED_STEERING_MATRIX_FEEDBACK_MASK;
		conf->ht_tx_bf_capab |= (u32)(1 << TX_BF_CAP_EXPLICIT_COMPRESSED_STEERING_MATRIX_FEEDBACK_OFFSET);
	}
	if (os_strstr(capab, "[EXPL-COMPR-FB-FBACK-IMM]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_EXPLICIT_COMPRESSED_STEERING_MATRIX_FEEDBACK_MASK;
		conf->ht_tx_bf_capab |= (u32)(2 << TX_BF_CAP_EXPLICIT_COMPRESSED_STEERING_MATRIX_FEEDBACK_OFFSET);
	}
	if (os_strstr(capab, "[EXPL-COMPR-FB-FBACK-DELAYED+IMM]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_EXPLICIT_COMPRESSED_STEERING_MATRIX_FEEDBACK_MASK;
		conf->ht_tx_bf_capab |= (u32)(3 << TX_BF_CAP_EXPLICIT_COMPRESSED_STEERING_MATRIX_FEEDBACK_OFFSET);
	}

	if (os_strstr(capab, "[MIN-GROUP-12]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_MINIMAL_GROUPING_MASK;
		conf->ht_tx_bf_capab |= (u32)(1 << TX_BF_CAP_MINIMAL_GROUPING_OFFSET);
	}
	if (os_strstr(capab, "[MIN-GROUP-14]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_MINIMAL_GROUPING_MASK;
		conf->ht_tx_bf_capab |= (u32)(2 << TX_BF_CAP_MINIMAL_GROUPING_OFFSET);
	}
	if (os_strstr(capab, "[MIN-GROUP-124]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_MINIMAL_GROUPING_MASK;
		conf->ht_tx_bf_capab |= (u32)(3 << TX_BF_CAP_MINIMAL_GROUPING_OFFSET);
	}


	if (os_strstr(capab, "[CSI-BF-ANT-1]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_CSI_NUM_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(0 << TX_BF_CAP_CSI_NUM_BEAMFORMER_ANT_OFFSET);
	}
	if (os_strstr(capab, "[CSI-BF-ANT-2]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_CSI_NUM_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(1 << TX_BF_CAP_CSI_NUM_BEAMFORMER_ANT_OFFSET);
	}
	if (os_strstr(capab, "[CSI-BF-ANT-3]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_CSI_NUM_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(2 << TX_BF_CAP_CSI_NUM_BEAMFORMER_ANT_OFFSET);
	}
	if (os_strstr(capab, "[CSI-BF-ANT-4]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_CSI_NUM_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(3 << TX_BF_CAP_CSI_NUM_BEAMFORMER_ANT_OFFSET);
	}


	if (os_strstr(capab, "[NONCOMPS-BF-ANT-1]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_UNCOMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(0 << TX_BF_CAP_UNCOMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_OFFSET);
	}
	if (os_strstr(capab, "[NONCOMPS-BF-ANT-2]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_UNCOMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(1 << TX_BF_CAP_UNCOMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_OFFSET);
	}
	if (os_strstr(capab, "[NONCOMPS-BF-ANT-3]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_UNCOMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(2 << TX_BF_CAP_UNCOMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_OFFSET);
	}
	if (os_strstr(capab, "[NONCOMPS-BF-ANT-4]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_UNCOMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(3 << TX_BF_CAP_UNCOMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_OFFSET);
	}

	if (os_strstr(capab, "[COMPS-BF-ANT-1]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_COMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(0 << TX_BF_CAP_COMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_OFFSET);
	}
	if (os_strstr(capab, "[COMPS-BF-ANT-2]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_COMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(1 << TX_BF_CAP_COMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_OFFSET);
	}
	if (os_strstr(capab, "[COMPS-BF-ANT-3]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_COMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(2 << TX_BF_CAP_COMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_OFFSET);
	}
	if (os_strstr(capab, "[COMPS-BF-ANT-4]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_COMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_MASK;
		conf->ht_tx_bf_capab |= (u32)(3 << TX_BF_CAP_COMPRESSED_STEERING_MATRIX_BEAMFORMER_ANT_OFFSET);
	}


	if (os_strstr(capab, "[CSI-MAX-ROWS-BF-1]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_SCI_MAX_OF_ROWS_BEANFORMER_SUPPORTED_MASK;
		conf->ht_tx_bf_capab |= (u32)(0 << TX_BF_CAP_SCI_MAX_OF_ROWS_BEANFORMER_SUPPORTED_OFFSET);
	}
	if (os_strstr(capab, "[CSI-MAX-ROWS-BF-2]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_SCI_MAX_OF_ROWS_BEANFORMER_SUPPORTED_MASK;
		conf->ht_tx_bf_capab |= (u32)(1 << TX_BF_CAP_SCI_MAX_OF_ROWS_BEANFORMER_SUPPORTED_OFFSET);
	}
	if (os_strstr(capab, "[CSI-MAX-ROWS-BF-3]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_SCI_MAX_OF_ROWS_BEANFORMER_SUPPORTED_MASK;
		conf->ht_tx_bf_capab |= (u32)(2 << TX_BF_CAP_SCI_MAX_OF_ROWS_BEANFORMER_SUPPORTED_OFFSET);
	}
	if (os_strstr(capab, "[CSI-MAX-ROWS-BF-4]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_SCI_MAX_OF_ROWS_BEANFORMER_SUPPORTED_MASK;
		conf->ht_tx_bf_capab |= (u32)(3 << TX_BF_CAP_SCI_MAX_OF_ROWS_BEANFORMER_SUPPORTED_OFFSET);
	}


	if (os_strstr(capab, "[CHE-SPACE-TIME-STR-1]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_CHANNEL_ESTIMATION_CAP_MASK;
		conf->ht_tx_bf_capab |= (u32)(0 << TX_BF_CAP_CHANNEL_ESTIMATION_CAP_OFFSET);
	}
	if (os_strstr(capab, "[CHE-SPACE-TIME-STR-2]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_CHANNEL_ESTIMATION_CAP_MASK;
		conf->ht_tx_bf_capab |= (u32)(1 << TX_BF_CAP_CHANNEL_ESTIMATION_CAP_OFFSET);
	}
	if (os_strstr(capab, "[CHE-SPACE-TIME-STR-3]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_CHANNEL_ESTIMATION_CAP_MASK;
		conf->ht_tx_bf_capab |= (u32)(2 << TX_BF_CAP_CHANNEL_ESTIMATION_CAP_OFFSET);
	}
	if (os_strstr(capab, "[CHE-SPACE-TIME-STR-4]")) {
		conf->ht_tx_bf_capab &= ~TX_BF_CAP_CHANNEL_ESTIMATION_CAP_MASK;
		conf->ht_tx_bf_capab |= (u32)(3 << TX_BF_CAP_CHANNEL_ESTIMATION_CAP_OFFSET);
	}

	return 0;
}

#ifdef CONFIG_IEEE80211AC
static int hostapd_config_vht_capab(struct hostapd_config *conf,
				    const char *capab)
{
	if (os_strstr(capab, "[MAX-MPDU-7991]"))
		conf->vht_capab |= VHT_CAP_MAX_MPDU_LENGTH_7991;
	if (os_strstr(capab, "[MAX-MPDU-11454]"))
		conf->vht_capab |= VHT_CAP_MAX_MPDU_LENGTH_11454;
	if (os_strstr(capab, "[VHT160]"))
		conf->vht_capab |= VHT_CAP_SUPP_CHAN_WIDTH_160MHZ;
	if (os_strstr(capab, "[VHT160-80PLUS80]"))
		conf->vht_capab |= VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ;
	if (os_strstr(capab, "[RXLDPC]"))
		conf->vht_capab |= VHT_CAP_RXLDPC;
	if (os_strstr(capab, "[SHORT-GI-80]"))
		conf->vht_capab |= VHT_CAP_SHORT_GI_80;
	if (os_strstr(capab, "[SHORT-GI-160]"))
		conf->vht_capab |= VHT_CAP_SHORT_GI_160;
	if (os_strstr(capab, "[TX-STBC-2BY1]"))
		conf->vht_capab |= VHT_CAP_TXSTBC;
	if (os_strstr(capab, "[RX-STBC-1]"))
		conf->vht_capab |= VHT_CAP_RXSTBC_1;
	if (os_strstr(capab, "[RX-STBC-12]"))
		conf->vht_capab |= VHT_CAP_RXSTBC_2;
	if (os_strstr(capab, "[RX-STBC-123]"))
		conf->vht_capab |= VHT_CAP_RXSTBC_3;
	if (os_strstr(capab, "[RX-STBC-1234]"))
		conf->vht_capab |= VHT_CAP_RXSTBC_4;
	if (os_strstr(capab, "[SU-BEAMFORMER]"))
		conf->vht_capab |= VHT_CAP_SU_BEAMFORMER_CAPABLE;
	if (os_strstr(capab, "[SU-BEAMFORMEE]"))
		conf->vht_capab |= VHT_CAP_SU_BEAMFORMEE_CAPABLE;
	if (os_strstr(capab, "[BF-ANTENNA-2]"))
		conf->vht_capab |= (1 << VHT_CAP_BEAMFORMEE_STS_OFFSET);
	if (os_strstr(capab, "[BF-ANTENNA-3]"))
		conf->vht_capab |= (2 << VHT_CAP_BEAMFORMEE_STS_OFFSET);
	if (os_strstr(capab, "[BF-ANTENNA-4]"))
		conf->vht_capab |= (3 << VHT_CAP_BEAMFORMEE_STS_OFFSET);
	if (os_strstr(capab, "[SOUNDING-DIMENSION-2]") &&
	    (conf->vht_capab & VHT_CAP_SU_BEAMFORMER_CAPABLE))
		conf->vht_capab |= (1 << VHT_CAP_SOUNDING_DIMENSION_OFFSET);
	if (os_strstr(capab, "[SOUNDING-DIMENSION-3]") &&
	    (conf->vht_capab & VHT_CAP_SU_BEAMFORMER_CAPABLE))
		conf->vht_capab |= (2 << VHT_CAP_SOUNDING_DIMENSION_OFFSET);
	if (os_strstr(capab, "[SOUNDING-DIMENSION-4]") &&
	    (conf->vht_capab & VHT_CAP_SU_BEAMFORMER_CAPABLE))
		conf->vht_capab |= (3 << VHT_CAP_SOUNDING_DIMENSION_OFFSET);
	if (os_strstr(capab, "[MU-BEAMFORMER]"))
		conf->vht_capab |= VHT_CAP_MU_BEAMFORMER_CAPABLE;
	if (os_strstr(capab, "[VHT-TXOP-PS]"))
		conf->vht_capab |= VHT_CAP_VHT_TXOP_PS;
	if (os_strstr(capab, "[HTC-VHT]"))
		conf->vht_capab |= VHT_CAP_HTC_VHT;
	if (os_strstr(capab, "[MAX-A-MPDU-LEN-EXP7]"))
		conf->vht_capab |= VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MAX;
	else if (os_strstr(capab, "[MAX-A-MPDU-LEN-EXP6]"))
		conf->vht_capab |= VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_6;
	else if (os_strstr(capab, "[MAX-A-MPDU-LEN-EXP5]"))
		conf->vht_capab |= VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_5;
	else if (os_strstr(capab, "[MAX-A-MPDU-LEN-EXP4]"))
		conf->vht_capab |= VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_4;
	else if (os_strstr(capab, "[MAX-A-MPDU-LEN-EXP3]"))
		conf->vht_capab |= VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_3;
	else if (os_strstr(capab, "[MAX-A-MPDU-LEN-EXP2]"))
		conf->vht_capab |= VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_2;
	else if (os_strstr(capab, "[MAX-A-MPDU-LEN-EXP1]"))
		conf->vht_capab |= VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_1;
	if (os_strstr(capab, "[VHT-LINK-ADAPT2]") &&
	    (conf->vht_capab & VHT_CAP_HTC_VHT))
		conf->vht_capab |= VHT_CAP_VHT_LINK_ADAPTATION_VHT_UNSOL_MFB;
	if (os_strstr(capab, "[VHT-LINK-ADAPT3]") &&
	    (conf->vht_capab & VHT_CAP_HTC_VHT))
		conf->vht_capab |= VHT_CAP_VHT_LINK_ADAPTATION_VHT_MRQ_MFB;
	if (os_strstr(capab, "[RX-ANTENNA-PATTERN]"))
		conf->vht_capab |= VHT_CAP_RX_ANTENNA_PATTERN;
	if (os_strstr(capab, "[TX-ANTENNA-PATTERN]"))
		conf->vht_capab |= VHT_CAP_TX_ANTENNA_PATTERN;
	if (os_strstr(capab, "[EXT-NSS-BW1]"))
		conf->vht_capab |= VHT_CAP_EXT_NSS_BW_1;
	if (os_strstr(capab, "[EXT-NSS-BW2]"))
		conf->vht_capab |= VHT_CAP_EXT_NSS_BW_2;
	if (os_strstr(capab, "[EXT-NSS-BW3]"))
		conf->vht_capab |= VHT_CAP_EXT_NSS_BW_3;
	return 0;
}
#endif /* CONFIG_IEEE80211AC */

#ifdef CONFIG_INTERWORKING
static int parse_roaming_consortium(struct hostapd_bss_config *bss, char *pos,
				    int line)
{
	size_t len = os_strlen(pos);
	u8 oi[MAX_ROAMING_CONSORTIUM_LEN];

	struct hostapd_roaming_consortium *rc;

	if ((len & 1) || len < 2 * 3 || len / 2 > MAX_ROAMING_CONSORTIUM_LEN ||
	    hexstr2bin(pos, oi, len / 2)) {
		wpa_printf(MSG_ERROR, "Line %d: invalid roaming_consortium "
			   "'%s'", line, pos);
		return -1;
	}
	len /= 2;

	rc = os_realloc_array(bss->roaming_consortium,
			      bss->roaming_consortium_count + 1,
			      sizeof(struct hostapd_roaming_consortium));
	if (rc == NULL)
		return -1;

	os_memcpy(rc[bss->roaming_consortium_count].oi, oi, len);
	rc[bss->roaming_consortium_count].len = len;

	bss->roaming_consortium = rc;
	bss->roaming_consortium_count++;

	return 0;
}


static int parse_lang_string(struct hostapd_lang_string **array,
			     unsigned int *count, char *pos)
{
	char *sep, *str = NULL;
	size_t clen, nlen, slen;
	struct hostapd_lang_string *ls;
	int ret = -1;

	if (*pos == '"' || (*pos == 'P' && pos[1] == '"')) {
		str = wpa_config_parse_string(pos, &slen);
		if (!str)
			return -1;
		pos = str;
	}

	sep = os_strchr(pos, ':');
	if (sep == NULL)
		goto fail;
	*sep++ = '\0';

	clen = os_strlen(pos);
	if (clen < 2 || clen > sizeof(ls->lang))
		goto fail;
	nlen = os_strlen(sep);
	if (nlen > 252)
		goto fail;

	ls = os_realloc_array(*array, *count + 1,
			      sizeof(struct hostapd_lang_string));
	if (ls == NULL)
		goto fail;

	*array = ls;
	ls = &(*array)[*count];
	(*count)++;

	os_memset(ls->lang, 0, sizeof(ls->lang));
	os_memcpy(ls->lang, pos, clen);
	ls->name_len = nlen;
	os_memcpy(ls->name, sep, nlen);

	ret = 0;
fail:
	os_free(str);
	return ret;
}


static int parse_venue_name(struct hostapd_bss_config *bss, char *pos,
			    int line)
{
	if (parse_lang_string(&bss->venue_name, &bss->venue_name_count, pos)) {
		wpa_printf(MSG_ERROR, "Line %d: Invalid venue_name '%s'",
			   line, pos);
		return -1;
	}
	return 0;
}


static int parse_venue_url(struct hostapd_bss_config *bss, char *pos,
			    int line)
{
	char *sep;
	size_t nlen;
	struct hostapd_venue_url *url;
	int ret = -1;

	sep = os_strchr(pos, ':');
	if (!sep)
		goto fail;
	*sep++ = '\0';

	nlen = os_strlen(sep);
	if (nlen > 254)
		goto fail;

	url = os_realloc_array(bss->venue_url, bss->venue_url_count + 1,
			       sizeof(struct hostapd_venue_url));
	if (!url)
		goto fail;

	bss->venue_url = url;
	url = &bss->venue_url[bss->venue_url_count++];

	url->venue_number = atoi(pos);
	url->url_len = nlen;
	os_memcpy(url->url, sep, nlen);

	ret = 0;
fail:
	if (ret)
		wpa_printf(MSG_ERROR, "Line %d: Invalid venue_url '%s'",
			   line, pos);
	return ret;
}


static int parse_3gpp_cell_net(struct hostapd_bss_config *bss, char *buf,
			       int line)
{
	size_t count;
	char *pos;
	u8 *info = NULL, *ipos;

	/* format: <MCC1,MNC1>[;<MCC2,MNC2>][;...] */

	count = 1;
	for (pos = buf; *pos; pos++) {
		if ((*pos < '0' || *pos > '9') && *pos != ';' && *pos != ',')
			goto fail;
		if (*pos == ';')
			count++;
	}
	if (1 + count * 3 > 0x7f)
		goto fail;

	info = os_zalloc(2 + 3 + count * 3);
	if (info == NULL)
		return -1;

	ipos = info;
	*ipos++ = 0; /* GUD - Version 1 */
	*ipos++ = 3 + count * 3; /* User Data Header Length (UDHL) */
	*ipos++ = 0; /* PLMN List IEI */
	/* ext(b8) | Length of PLMN List value contents(b7..1) */
	*ipos++ = 1 + count * 3;
	*ipos++ = count; /* Number of PLMNs */

	pos = buf;
	while (pos && *pos) {
		char *mcc, *mnc;
		size_t mnc_len;

		mcc = pos;
		mnc = os_strchr(pos, ',');
		if (mnc == NULL)
			goto fail;
		*mnc++ = '\0';
		pos = os_strchr(mnc, ';');
		if (pos)
			*pos++ = '\0';

		mnc_len = os_strlen(mnc);
		if (os_strlen(mcc) != 3 || (mnc_len != 2 && mnc_len != 3))
			goto fail;

		/* BC coded MCC,MNC */
		/* MCC digit 2 | MCC digit 1 */
		*ipos++ = ((mcc[1] - '0') << 4) | (mcc[0] - '0');
		/* MNC digit 3 | MCC digit 3 */
		*ipos++ = (((mnc_len == 2) ? 0xf0 : ((mnc[2] - '0') << 4))) |
			(mcc[2] - '0');
		/* MNC digit 2 | MNC digit 1 */
		*ipos++ = ((mnc[1] - '0') << 4) | (mnc[0] - '0');
	}

	os_free(bss->anqp_3gpp_cell_net);
	bss->anqp_3gpp_cell_net = info;
	bss->anqp_3gpp_cell_net_len = 2 + 3 + 3 * count;
	wpa_hexdump(MSG_MSGDUMP, "3GPP Cellular Network information",
		    bss->anqp_3gpp_cell_net, bss->anqp_3gpp_cell_net_len);

	return 0;

fail:
	wpa_printf(MSG_ERROR, "Line %d: Invalid anqp_3gpp_cell_net: %s",
		   line, buf);
	os_free(info);
	return -1;
}


static int parse_nai_realm(struct hostapd_bss_config *bss, char *buf, int line)
{
	struct hostapd_nai_realm_data *realm;
	size_t i, j, len;
	int *offsets;
	char *pos, *end, *rpos;

	offsets = os_calloc(bss->nai_realm_count * MAX_NAI_REALMS,
			    sizeof(int));
	if (offsets == NULL)
		return -1;

	for (i = 0; i < bss->nai_realm_count; i++) {
		realm = &bss->nai_realm_data[i];
		for (j = 0; j < MAX_NAI_REALMS; j++) {
			offsets[i * MAX_NAI_REALMS + j] =
				realm->realm[j] ?
				realm->realm[j] - realm->realm_buf : -1;
		}
	}

	realm = os_realloc_array(bss->nai_realm_data, bss->nai_realm_count + 1,
				 sizeof(struct hostapd_nai_realm_data));
	if (realm == NULL) {
		os_free(offsets);
		return -1;
	}
	bss->nai_realm_data = realm;

	/* patch the pointers after realloc */
	for (i = 0; i < bss->nai_realm_count; i++) {
		realm = &bss->nai_realm_data[i];
		for (j = 0; j < MAX_NAI_REALMS; j++) {
			int offs = offsets[i * MAX_NAI_REALMS + j];
			if (offs >= 0)
				realm->realm[j] = realm->realm_buf + offs;
			else
				realm->realm[j] = NULL;
		}
	}
	os_free(offsets);

	realm = &bss->nai_realm_data[bss->nai_realm_count];
	os_memset(realm, 0, sizeof(*realm));

	pos = buf;
	realm->encoding = atoi(pos);
	pos = os_strchr(pos, ',');
	if (pos == NULL)
		goto fail;
	pos++;

	end = os_strchr(pos, ',');
	if (end) {
		len = end - pos;
		*end = '\0';
	} else {
		len = os_strlen(pos);
	}

	if (len > MAX_NAI_REALMLEN) {
		wpa_printf(MSG_ERROR, "Too long a realm string (%d > max %d "
			   "characters)", (int) len, MAX_NAI_REALMLEN);
		goto fail;
	}
	os_memcpy(realm->realm_buf, pos, len);

	if (end)
		pos = end + 1;
	else
		pos = NULL;

	while (pos && *pos) {
		struct hostapd_nai_realm_eap *eap;

		if (realm->eap_method_count >= MAX_NAI_EAP_METHODS) {
			wpa_printf(MSG_ERROR, "Too many EAP methods");
			goto fail;
		}

		eap = &realm->eap_method[realm->eap_method_count];
		realm->eap_method_count++;

		end = os_strchr(pos, ',');
		if (end == NULL)
			end = pos + os_strlen(pos);

		eap->eap_method = atoi(pos);
		for (;;) {
			pos = os_strchr(pos, '[');
			if (pos == NULL || pos > end)
				break;
			pos++;
			if (eap->num_auths >= MAX_NAI_AUTH_TYPES) {
				wpa_printf(MSG_ERROR, "Too many auth params");
				goto fail;
			}
			eap->auth_id[eap->num_auths] = atoi(pos);
			pos = os_strchr(pos, ':');
			if (pos == NULL || pos > end)
				goto fail;
			pos++;
			eap->auth_val[eap->num_auths] = atoi(pos);
			pos = os_strchr(pos, ']');
			if (pos == NULL || pos > end)
				goto fail;
			pos++;
			eap->num_auths++;
		}

		if (*end != ',')
			break;

		pos = end + 1;
	}

	/* Split realm list into null terminated realms */
	rpos = realm->realm_buf;
	i = 0;
	while (*rpos) {
		if (i >= MAX_NAI_REALMS) {
			wpa_printf(MSG_ERROR, "Too many realms");
			goto fail;
		}
		realm->realm[i++] = rpos;
		rpos = os_strchr(rpos, ';');
		if (rpos == NULL)
			break;
		*rpos++ = '\0';
	}

	bss->nai_realm_count++;

	return 0;

fail:
	wpa_printf(MSG_ERROR, "Line %d: invalid nai_realm '%s'", line, buf);
	return -1;
}


static int parse_anqp_elem(struct hostapd_bss_config *bss, char *buf, int line)
{
	char *delim;
	u16 infoid;
	size_t len;
	struct wpabuf *payload;
	struct anqp_element *elem;

	delim = os_strchr(buf, ':');
	if (!delim)
		return -1;
	delim++;
	infoid = atoi(buf);
	len = os_strlen(delim);
	if (len & 1)
		return -1;
	len /= 2;
	payload = wpabuf_alloc(len);
	if (!payload)
		return -1;
	if (hexstr2bin(delim, wpabuf_put(payload, len), len) < 0) {
		wpabuf_free(payload);
		return -1;
	}

	dl_list_for_each(elem, &bss->anqp_elem, struct anqp_element, list) {
		if (elem->infoid == infoid) {
			/* Update existing entry */
			wpabuf_free(elem->payload);
			elem->payload = payload;
			return 0;
		}
	}

	/* Add a new entry */
	elem = os_zalloc(sizeof(*elem));
	if (!elem) {
		wpabuf_free(payload);
		return -1;
	}
	elem->infoid = infoid;
	elem->payload = payload;
	DL_LIST_ADD(&bss->anqp_elem, elem, list);

	return 0;
}


static int parse_qos_map_set(struct hostapd_bss_config *bss,
			     char *buf, int line)
{
	u8 qos_map_set[16 + 2 * 21], count = 0;
	char *pos = buf;
	int val;

	for (;;) {
		if (count == sizeof(qos_map_set)) {
			wpa_printf(MSG_ERROR, "Line %d: Too many qos_map_set "
				   "parameters '%s'", line, buf);
			return -1;
		}

		val = atoi(pos);
		if (!IS_VALID_U8_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid qos_map_set "
				   "'%s'", line, buf);
			return -1;
		}

		qos_map_set[count++] = (u8)val;
		pos = os_strchr(pos, ',');
		if (!pos)
			break;
		pos++;
	}

	if (count < 16 || count & 1) {
		wpa_printf(MSG_ERROR, "Line %d: Invalid qos_map_set '%s'",
			   line, buf);
		return -1;
	}

	os_memcpy(bss->qos_map_set, qos_map_set, count);
	bss->qos_map_set_len = count;

	return 0;
}

#endif /* CONFIG_INTERWORKING */


#ifdef CONFIG_HS20
static int hs20_parse_conn_capab(struct hostapd_bss_config *bss, char *buf,
				 int line)
{
	s32 val;
	u8 *conn_cap;
	char *pos;

	if (bss->hs20_connection_capability_len >= 0xfff0)
		return -1;

	conn_cap = os_realloc(bss->hs20_connection_capability,
			      bss->hs20_connection_capability_len + 4);
	if (conn_cap == NULL)
		return -1;

	bss->hs20_connection_capability = conn_cap;
	conn_cap += bss->hs20_connection_capability_len;
	pos = buf;
	val = atoi(pos);
	if(!IS_VALID_U8_RANGE(val))
	  return -1;
	conn_cap[0] = (u8)val;
	pos = os_strchr(pos, ':');
	if (pos == NULL)
		return -1;
	pos++;
	val = atoi(pos);
	if(!IS_VALID_U16_RANGE(val))
	  return -1;
	WPA_PUT_LE16(conn_cap + 1, val);
	pos = os_strchr(pos, ':');
	if (pos == NULL)
		return -1;
	pos++;
	val = atoi(pos);
	if(!IS_VALID_U8_RANGE(val))
	  return -1;
	conn_cap[3] = (u8)val;
	bss->hs20_connection_capability_len += 4;

	return 0;
}


int hs20_parse_wan_metrics(struct hostapd_bss_config *bss, char *buf, int line)
{
	s32 val;
	u8 *wan_metrics;
	char *pos;

	/* <WAN Info>:<DL Speed>:<UL Speed>:<DL Load>:<UL Load>:<LMD> */

	wan_metrics = os_zalloc(13);
	if (wan_metrics == NULL)
		return -1;

	pos = buf;
	/* WAN Info */
	if (hexstr2bin(pos, wan_metrics, 1) < 0)
		goto fail;
	pos += 2;
	if (*pos != ':')
		goto fail;
	pos++;

	/* Downlink Speed */
	WPA_PUT_LE32(wan_metrics + 1, strtoul(pos, NULL, 10));
	pos = os_strchr(pos, ':');
	if (pos == NULL)
		goto fail;
	pos++;

	/* Uplink Speed */
	WPA_PUT_LE32(wan_metrics + 5, strtoul(pos, NULL, 10));
	pos = os_strchr(pos, ':');
	if (pos == NULL)
		goto fail;
	pos++;

	/* Downlink Load */
	val = atoi(pos);
	if(!IS_VALID_U8_RANGE(val))
		goto fail;
	wan_metrics[9] = (u8)val;
	pos = os_strchr(pos, ':');
	if (pos == NULL)
		goto fail;
	pos++;

	/* Uplink Load */
	val = atoi(pos);
	if(!IS_VALID_U8_RANGE(val))
		goto fail;
	wan_metrics[10] = (u8)val;
	pos = os_strchr(pos, ':');
	if (pos == NULL)
		goto fail;
	pos++;

	/* LMD */
	val = atoi(pos);
	if(!IS_VALID_U16_RANGE(val))
		goto fail;
	WPA_PUT_LE16(wan_metrics + 11, (u16)val);

	os_free(bss->hs20_wan_metrics);
	bss->hs20_wan_metrics = wan_metrics;

	return 0;

fail:
	wpa_printf(MSG_ERROR, "Line %d: Invalid hs20_wan_metrics '%s'",
		   line, buf);
	os_free(wan_metrics);
	return -1;
}


static int hs20_parse_oper_friendly_name(struct hostapd_bss_config *bss,
					 char *pos, int line)
{
	if (parse_lang_string(&bss->hs20_oper_friendly_name,
			      &bss->hs20_oper_friendly_name_count, pos)) {
		wpa_printf(MSG_ERROR, "Line %d: Invalid "
			   "hs20_oper_friendly_name '%s'", line, pos);
		return -1;
	}
	return 0;
}


static int hs20_parse_icon(struct hostapd_bss_config *bss, char *pos)
{
	s32 val;
	struct hs20_icon *icon;
	char *end;

	icon = os_realloc_array(bss->hs20_icons, bss->hs20_icons_count + 1,
				sizeof(struct hs20_icon));
	if (icon == NULL)
		return -1;
	bss->hs20_icons = icon;
	icon = &bss->hs20_icons[bss->hs20_icons_count];
	os_memset(icon, 0, sizeof(*icon));

	val = atoi(pos);
	if(!IS_VALID_U16_RANGE(val))
		return -1;
	icon->width = (u16)val;
	pos = os_strchr(pos, ':');
	if (pos == NULL)
		return -1;
	pos++;

	val = atoi(pos);
	if(!IS_VALID_U16_RANGE(val))
		return -1;
	icon->height = (u16)val;
	pos = os_strchr(pos, ':');
	if (pos == NULL)
		return -1;
	pos++;

	end = os_strchr(pos, ':');
	if (end == NULL || end - pos > 3)
		return -1;
	os_memcpy(icon->language, pos, end - pos);
	pos = end + 1;

	end = os_strchr(pos, ':');
	if (end == NULL || end - pos > 255)
		return -1;
	os_memcpy(icon->type, pos, end - pos);
	pos = end + 1;

	end = os_strchr(pos, ':');
	if (end == NULL || end - pos > 255)
		return -1;
	os_memcpy(icon->name, pos, end - pos);
	pos = end + 1;

	if (os_strlen(pos) > 255)
		return -1;
	os_memcpy(icon->file, pos, os_strlen(pos));

	bss->hs20_icons_count++;

	return 0;
}


static int hs20_parse_osu_ssid(struct hostapd_bss_config *bss,
			       char *pos, int line)
{
	size_t slen;
	char *str;

	str = wpa_config_parse_string(pos, &slen);
	if (str == NULL || slen < 1 || slen > SSID_MAX_LEN) {
		wpa_printf(MSG_ERROR, "Line %d: Invalid SSID '%s'", line, pos);
		os_free(str);
		return -1;
	}

	os_memcpy(bss->osu_ssid, str, slen);
	bss->osu_ssid_len = slen;
	os_free(str);

	return 0;
}


static int hs20_parse_osu_server_uri(struct hostapd_bss_config *bss,
				     char *pos, int line)
{
	struct hs20_osu_provider *p;

	p = os_realloc_array(bss->hs20_osu_providers,
			     bss->hs20_osu_providers_count + 1, sizeof(*p));
	if (p == NULL)
		return -1;

	bss->hs20_osu_providers = p;
	bss->last_osu = &bss->hs20_osu_providers[bss->hs20_osu_providers_count];
	bss->hs20_osu_providers_count++;
	os_memset(bss->last_osu, 0, sizeof(*p));
	bss->last_osu->server_uri = os_strdup(pos);

	return 0;
}


static int hs20_parse_osu_friendly_name(struct hostapd_bss_config *bss,
					char *pos, int line)
{
	if (bss->last_osu == NULL) {
		wpa_printf(MSG_ERROR, "Line %d: Unexpected OSU field", line);
		return -1;
	}

	if (parse_lang_string(&bss->last_osu->friendly_name,
			      &bss->last_osu->friendly_name_count, pos)) {
		wpa_printf(MSG_ERROR, "Line %d: Invalid osu_friendly_name '%s'",
			   line, pos);
		return -1;
	}

	return 0;
}


static int hs20_parse_osu_nai(struct hostapd_bss_config *bss,
			      char *pos, int line)
{
	if (bss->last_osu == NULL) {
		wpa_printf(MSG_ERROR, "Line %d: Unexpected OSU field", line);
		return -1;
	}

	os_free(bss->last_osu->osu_nai);
	bss->last_osu->osu_nai = os_strdup(pos);
	if (bss->last_osu->osu_nai == NULL)
		return -1;

	return 0;
}


static int hs20_parse_osu_nai2(struct hostapd_bss_config *bss,
			       char *pos, int line)
{
	if (bss->last_osu == NULL) {
		wpa_printf(MSG_ERROR, "Line %d: Unexpected OSU field", line);
		return -1;
	}

	os_free(bss->last_osu->osu_nai2);
	bss->last_osu->osu_nai2 = os_strdup(pos);
	if (bss->last_osu->osu_nai2 == NULL)
		return -1;
	bss->hs20_osu_providers_nai_count++;

	return 0;
}


static int hs20_parse_osu_method_list(struct hostapd_bss_config *bss, char *pos,
				      int line)
{
	if (bss->last_osu == NULL) {
		wpa_printf(MSG_ERROR, "Line %d: Unexpected OSU field", line);
		return -1;
	}

	if (hostapd_parse_intlist(&bss->last_osu->method_list, pos)) {
		wpa_printf(MSG_ERROR, "Line %d: Invalid osu_method_list", line);
		return -1;
	}

	return 0;
}


static int hs20_parse_osu_icon(struct hostapd_bss_config *bss, char *pos,
			       int line)
{
	char **n;
	struct hs20_osu_provider *p = bss->last_osu;

	if (p == NULL) {
		wpa_printf(MSG_ERROR, "Line %d: Unexpected OSU field", line);
		return -1;
	}

	n = os_realloc_array(p->icons, p->icons_count + 1, sizeof(char *));
	if (n == NULL)
		return -1;
	p->icons = n;
	p->icons[p->icons_count] = os_strdup(pos);
	if (p->icons[p->icons_count] == NULL)
		return -1;
	p->icons_count++;

	return 0;
}


static int hs20_parse_osu_service_desc(struct hostapd_bss_config *bss,
				       char *pos, int line)
{
	if (bss->last_osu == NULL) {
		wpa_printf(MSG_ERROR, "Line %d: Unexpected OSU field", line);
		return -1;
	}

	if (parse_lang_string(&bss->last_osu->service_desc,
			      &bss->last_osu->service_desc_count, pos)) {
		wpa_printf(MSG_ERROR, "Line %d: Invalid osu_service_desc '%s'",
			   line, pos);
		return -1;
	}

	return 0;
}


static int hs20_parse_operator_icon(struct hostapd_bss_config *bss, char *pos,
				    int line)
{
	char **n;

	n = os_realloc_array(bss->hs20_operator_icon,
			     bss->hs20_operator_icon_count + 1, sizeof(char *));
	if (!n)
		return -1;
	bss->hs20_operator_icon = n;
	bss->hs20_operator_icon[bss->hs20_operator_icon_count] = os_strdup(pos);
	if (!bss->hs20_operator_icon[bss->hs20_operator_icon_count])
		return -1;
	bss->hs20_operator_icon_count++;

	return 0;
}

#endif /* CONFIG_HS20 */

#ifdef CONFIG_ALLOW_HOSTAPD
#ifdef CONFIG_ACS
static int hostapd_config_parse_acs_chan_bias(struct hostapd_config *conf,
					      char *pos)
{
	struct acs_bias *bias = NULL, *tmp;
	unsigned int num = 0;
	char *end;

	while (*pos) {
		tmp = os_realloc_array(bias, num + 1, sizeof(*bias));
		if (!tmp)
			goto fail;
		bias = tmp;

		bias[num].channel = atoi(pos);
		if (bias[num].channel <= 0)
			goto fail;
		pos = os_strchr(pos, ':');
		if (!pos)
			goto fail;
		pos++;
		bias[num].bias = strtod(pos, &end);
		if (end == pos || bias[num].bias < 0.0)
			goto fail;
		pos = end;
		if (*pos != ' ' && *pos != '\0')
			goto fail;
		num++;
	}

	os_free(conf->acs_chan_bias);
	conf->acs_chan_bias = bias;
	conf->num_acs_chan_bias = num;

	return 0;
fail:
	os_free(bias);
	return -1;
}
#endif /* CONFIG_ACS */
#endif /* CONFIG_ALLOW_HOSTAPD */

static int parse_wpabuf_hex(int line, const char *name, struct wpabuf **buf,
			    const char *val)
{
	struct wpabuf *elems;

	if (val[0] == '\0') {
		wpabuf_free(*buf);
		*buf = NULL;
		return 0;
	}

	elems = wpabuf_parse_bin(val);
	if (!elems) {
		wpa_printf(MSG_ERROR, "Line %d: Invalid %s '%s'",
			   line, name, val);
		return -1;
	}

	wpabuf_free(*buf);
	*buf = elems;

	return 0;
}


#ifdef CONFIG_FILS
static int parse_fils_realm(struct hostapd_bss_config *bss, const char *val)
{
	struct fils_realm *realm;
	size_t len;

	len = os_strlen(val);
	realm = os_zalloc(sizeof(*realm) + len + 1);
	if (!realm)
		return -1;

	os_memcpy(realm->realm, val, len);
	if (fils_domain_name_hash(val, realm->hash) < 0) {
		os_free(realm);
		return -1;
	}
	DL_LIST_ADD_TAIL(&bss->fils_realms, realm, list);

	return 0;
}
#endif /* CONFIG_FILS */


#ifdef EAP_SERVER
#ifdef CONFIG_ALLOW_HOSTAPD
static unsigned int parse_tls_flags(const char *val)
{
	unsigned int flags = 0;

	/* Disable TLS v1.3 by default for now to avoid interoperability issue.
	 * This can be enabled by default once the implementation has been fully
	 * completed and tested with other implementations. */
	flags |= TLS_CONN_DISABLE_TLSv1_3;

	if (os_strstr(val, "[ALLOW-SIGN-RSA-MD5]"))
		flags |= TLS_CONN_ALLOW_SIGN_RSA_MD5;
	if (os_strstr(val, "[DISABLE-TIME-CHECKS]"))
		flags |= TLS_CONN_DISABLE_TIME_CHECKS;
	if (os_strstr(val, "[DISABLE-TLSv1.0]"))
		flags |= TLS_CONN_DISABLE_TLSv1_0;
	if (os_strstr(val, "[ENABLE-TLSv1.0]"))
		flags |= TLS_CONN_ENABLE_TLSv1_0;
	if (os_strstr(val, "[DISABLE-TLSv1.1]"))
		flags |= TLS_CONN_DISABLE_TLSv1_1;
	if (os_strstr(val, "[ENABLE-TLSv1.1]"))
		flags |= TLS_CONN_ENABLE_TLSv1_1;
	if (os_strstr(val, "[DISABLE-TLSv1.2]"))
		flags |= TLS_CONN_DISABLE_TLSv1_2;
	if (os_strstr(val, "[ENABLE-TLSv1.2]"))
		flags |= TLS_CONN_ENABLE_TLSv1_2;
	if (os_strstr(val, "[DISABLE-TLSv1.3]"))
		flags |= TLS_CONN_DISABLE_TLSv1_3;
	if (os_strstr(val, "[ENABLE-TLSv1.3]"))
		flags &= ~TLS_CONN_DISABLE_TLSv1_3;
	if (os_strstr(val, "[SUITEB]"))
		flags |= TLS_CONN_SUITEB;
	if (os_strstr(val, "[SUITEB-NO-ECDH]"))
		flags |= TLS_CONN_SUITEB_NO_ECDH | TLS_CONN_SUITEB;

	return flags;
}
#endif /* CONFIG_ALLOW_HOSTAPD */
#endif /* EAP_SERVER */


#ifdef CONFIG_AIRTIME_POLICY
static int add_airtime_weight(struct hostapd_bss_config *bss, char *value)
{
	struct airtime_sta_weight *wt;
	char *pos, *next;

	wt = os_zalloc(sizeof(*wt));
	if (!wt)
		return -1;

	/* 02:01:02:03:04:05 10 */
	pos = value;
	next = os_strchr(pos, ' ');
	if (next)
		*next++ = '\0';
	if (!next || hwaddr_aton(pos, wt->addr)) {
		wpa_printf(MSG_ERROR, "Invalid station address: '%s'", pos);
		os_free(wt);
		return -1;
	}

	pos = next;
	wt->weight = atoi(pos);
	if (!wt->weight) {
		wpa_printf(MSG_ERROR, "Invalid weight: '%s'", pos);
		os_free(wt);
		return -1;
	}

	wt->next = bss->airtime_weight_list;
	bss->airtime_weight_list = wt;
	return 0;
}
#endif /* CONFIG_AIRTIME_POLICY */


#ifdef CONFIG_SAE
static int parse_sae_password(struct hostapd_bss_config *bss, const char *val)
{
	struct sae_password_entry *pw;
	const char *pos = val, *pos2, *end = NULL;

	pw = os_zalloc(sizeof(*pw));
	if (!pw)
		return -1;
	os_memset(pw->peer_addr, 0xff, ETH_ALEN); /* default to wildcard */

	pos2 = os_strstr(pos, "|mac=");
	if (pos2) {
		end = pos2;
		pos2 += 5;
		if (hwaddr_aton(pos2, pw->peer_addr) < 0)
			goto fail;
		pos = pos2 + ETH_ALEN * 3 - 1;
	}

	pos2 = os_strstr(pos, "|vlanid=");
	if (pos2) {
		if (!end)
			end = pos2;
		pos2 += 8;
		pw->vlan_id = atoi(pos2);
	}

#ifdef CONFIG_SAE_PK
	pos2 = os_strstr(pos, "|pk=");
	if (pos2) {
		const char *epos;
		char *tmp;

		if (!end)
			end = pos2;
		pos2 += 4;
		epos = os_strchr(pos2, '|');
		if (epos) {
			tmp = os_malloc(epos - pos2 + 1);
			if (!tmp)
				goto fail;
			os_memcpy(tmp, pos2, epos - pos2);
			tmp[epos - pos2] = '\0';
		} else {
			tmp = os_strdup(pos2);
			if (!tmp)
				goto fail;
		}

		pw->pk = sae_parse_pk(tmp);
		str_clear_free(tmp);
		if (!pw->pk)
			goto fail;
	}
#endif /* CONFIG_SAE_PK */

	pos2 = os_strstr(pos, "|id=");
	if (pos2) {
		if (!end)
			end = pos2;
		pos2 += 4;
		pw->identifier = os_strdup(pos2);
		if (!pw->identifier)
			goto fail;
	}

	if (!end) {
		pw->password = os_strdup(val);
		if (!pw->password)
			goto fail;
	} else {
		pw->password = os_malloc(end - val + 1);
		if (!pw->password)
			goto fail;
		os_memcpy(pw->password, val, end - val);
		pw->password[end - val] = '\0';
	}

#ifdef CONFIG_SAE_PK
	if (pw->pk && !sae_pk_valid_password(pw->password)) {
		wpa_printf(MSG_INFO,
			   "Invalid SAE password for a SAE-PK sae_password entry");
		goto fail;
	}
#endif /* CONFIG_SAE_PK */

	pw->next = bss->sae_passwords;
	bss->sae_passwords = pw;

	return 0;
fail:
	str_clear_free(pw->password);
	os_free(pw->identifier);
#ifdef CONFIG_SAE_PK
	sae_deinit_pk(pw->pk);
#endif /* CONFIG_SAE_PK */
	os_free(pw);
	return -1;
}
#endif /* CONFIG_SAE */

#ifdef CONFIG_DPP2
static int hostapd_dpp_controller_parse(struct hostapd_bss_config *bss,
					const char *pos)
{
	struct dpp_controller_conf *conf;
	char *val;

	conf = os_zalloc(sizeof(*conf));
	if (!conf)
		return -1;
	val = get_param(pos, "ipaddr=");
	if (!val || hostapd_parse_ip_addr(val, &conf->ipaddr))
		goto fail;
	os_free(val);
	val = get_param(pos, "pkhash=");
	if (!val || os_strlen(val) != 2 * SHA256_MAC_LEN ||
	    hexstr2bin(val, conf->pkhash, SHA256_MAC_LEN) < 0)
		goto fail;
	os_free(val);
	conf->next = bss->dpp_controller;
	bss->dpp_controller = conf;
	return 0;
fail:
	os_free(val);
	os_free(conf);
	return -1;
}
#endif /* CONFIG_DPP2 */

#ifdef CONFIG_WIFI_CERTIFICATION
static int hostapd_config_parse_wpa_hex_buf(struct hostapd_bss_config *bss,const char *value)
{
#define WPA_STR_LEN 48
#define WPA_STR_LEN_WITH_GROUP_MGMT_CIPHER 56

	char *start, *end;
	int last, len, ret = -1;
	u8 *wpa_buf = NULL;
	struct wpa_ie_data wpa_ie = {0};
	int wpa_buf_len = 0;

	char *buf = os_strdup(value);

	if (buf == NULL)
		return 1;

	start = buf;
	while (*start != '\0') {
		while (*start == ' ' || *start == '\t')
			start++;
		if (*start == '\0')
			break;
		end = start;
		while (*end != ' ' && *end != '\t' && *end != '\0')
			end++;
		last = *end == '\0';
		*end = '\0';

		len = os_strlen(start);
		if(len != WPA_STR_LEN && len != WPA_STR_LEN_WITH_GROUP_MGMT_CIPHER) {
			wpa_printf(MSG_ERROR, "invalid wpa hex buf length %d", len);
			break;
		}
		/* converting hexstr to byte, so bytearray length will be half of the hex string */
		wpa_buf_len = len / 2;
		wpa_buf = os_malloc(wpa_buf_len);
		if(wpa_buf == NULL) {
			goto end;
		}

		if (hexstr2bin(start, wpa_buf, wpa_buf_len)) {
			goto end;
		}

		if (wpa_buf[0] == WLAN_EID_RSN && wpa_buf_len >= sizeof(struct rsn_ie_hdr)) {
			if (wpa_parse_wpa_ie_rsn(wpa_buf, wpa_buf_len, &wpa_ie)) {
				wpa_printf(MSG_ERROR, "wpa_ie_rsn parsing failed of length %d", wpa_buf_len);
				goto end;
			}
		} else if (wpa_buf[0] == WLAN_EID_VENDOR_SPECIFIC && wpa_buf_len >= sizeof(struct wpa_ie_hdr)) {
			if (wpa_parse_wpa_ie_wpa(wpa_buf, wpa_buf_len, &wpa_ie)) {
				wpa_printf(MSG_ERROR, "wpa_ie_wpa parsing failed of length %d", wpa_buf_len);
				goto end;
			}

		}

		/* Default GROUP_MGMT_CIPHER is WPA_CIPHER_AES_128_CMAC,
		 * For certification, we will advertise if it includes in "wpa hex input"
		 */
		if(len != WPA_STR_LEN_WITH_GROUP_MGMT_CIPHER) {
			wpa_ie.mgmt_group_cipher = WPA_CIPHER_NONE;
		}

		/* Update wpa data */
		bss->wpa = wpa_ie.proto;
		bss->group_cipher = wpa_ie.group_cipher;
		bss->wpa_pairwise = wpa_ie.pairwise_cipher;
		bss->wpa_key_mgmt = wpa_ie.key_mgmt;
		bss->group_mgmt_cipher = wpa_ie.mgmt_group_cipher;

		if (bss->wpa_key_mgmt == WPA_KEY_MGMT_IEEE8021X ||
			bss->wpa_key_mgmt == WPA_KEY_MGMT_IEEE8021X_SHA256) {
			bss->ieee802_1x = 1;
		} else {
			bss->ieee802_1x = 0;
		}

		if ((wpa_ie.capabilities & WPA_CAPABILITY_MFPR)
				&& (wpa_ie.capabilities & WPA_CAPABILITY_MFPC)) {
			bss->ieee80211w = MGMT_FRAME_PROTECTION_REQUIRED;
		} else if (wpa_ie.capabilities & WPA_CAPABILITY_MFPC) {
			bss->ieee80211w = MGMT_FRAME_PROTECTION_OPTIONAL;
		} else {
			bss->ieee80211w = NO_MGMT_FRAME_PROTECTION;
		}

		if (wpa_ie.capabilities & (RSN_NUM_REPLAY_COUNTERS_16 << 2)) {
			bss->wmm_enabled = 1;
		} else {
			bss->wmm_enabled = 0;
		}

		if (wpa_ie.capabilities & WPA_CAPABILITY_PREAUTH) {
			bss->rsn_preauth = 1;
		} else {
			bss->rsn_preauth = 0;
		}

		ret=0;
		if (last)
			break;
		start = end + 1;
	}
end:
	if(buf)
		os_free(buf);
	if(wpa_buf)
		os_free(wpa_buf);
	return ret;
}
#endif

/**
 * Read WHM configuration from whm.conf file
 * Syntax of whm.conf file
 * @param_name=@param_value
 * Example : whm_enable=1 #Enables the main whm flag
 */
int hostapd_whm_read_config(int* whm_cfg, const char* fname)
{
	char buf[256], *pos;
	int line = 0,val=0;
	FILE *f = fopen(fname, "r");
	if (f == NULL) {
		wpa_printf(MSG_ERROR, "WHM: Could not open config file '%s'.", fname);
		return -1;
	}

	/* We don't use any special handling for '\n' because isspace('\n') returns true. */
	while (fgets(buf, sizeof(buf), f)) {
		line++;

		/* Skip leading spaces. */
		pos = buf;
		while (isspace(*pos))
			pos++;
		if (*pos != '\0' && *pos != '#') { /* <name> = <value> */
			char *name, *value, *endptr;
			int  name_len, value_len;
			read_token (&pos, &name, &name_len, "=");
			if(*pos != '=')
				goto syntax_error;
			pos++;

			/* Value is the rest of the line, it can contain spaces */
			/* However, cut off the leading and trailing spaces */
			while (isspace(*pos))
				(pos)++;
			value = pos;
			value_len = 0;
			while (*pos != '\0' && *pos != '#') {
				if (!isspace(*pos))
					value_len = pos - value + 1;
				pos++;
			}

			/* Make name and value null-terminated strings */
			name[name_len] = 0;
			value[value_len] = 0;

			if (strcmp(name, "whm_enable") == 0) {
				val = strtol(value, &endptr, 10);
				if (!IS_VALID_BOOL_RANGE(val)) {
					wpa_printf(MSG_ERROR, "line %d: invalid whm_enable %d", line, val);
					goto syntax_error;
				}
				*whm_cfg |= val << WHM_ENABLE;
				if (*endptr != '\0')
					goto syntax_error;
			}
			else if (strcmp(name, "whm_hostapd_warn") == 0) {
				val = strtol(value, &endptr, 10);
				if (!IS_VALID_BOOL_RANGE(val)) {
					wpa_printf(MSG_ERROR, "line %d: invalid whm_hostapd_warn %d", line, val);
					goto syntax_error;
				}
				*whm_cfg |= val << WHM_HOSTAP_WARN;
				if (*endptr != '\0')
					goto syntax_error;
			}
			else if (strcmp(name, "whm_driver_warn") == 0) {
				val = strtol(value, &endptr, 10);
				if (!IS_VALID_BOOL_RANGE(val)) {
					wpa_printf(MSG_ERROR, "line %d: invalid whm_driver_warn %d", line, val);
					goto syntax_error;
				}
				*whm_cfg |= val << WHM_DRV_WARN;
				if (*endptr != '\0')
					goto syntax_error;
			}
			else if (strcmp(name, "whm_fw_warn") == 0) {
				val = strtol(value, &endptr, 10);
				if (!IS_VALID_BOOL_RANGE(val)) {
					wpa_printf(MSG_ERROR, "line %d: invalid whm_fw_warn %d", line, val);
					goto syntax_error;
				}
				*whm_cfg |= val << WHM_FW_WARN;
				if (*endptr != '\0')
					goto syntax_error;
			}
			else if (strcmp(name, "whm_phy_warn") == 0) {
				val = strtol(value, &endptr, 10);
				if (!IS_VALID_BOOL_RANGE(val)) {
					wpa_printf(MSG_ERROR, "line %d: invalid whm_phy_warn %d", line, val);
					goto syntax_error;
				}
				*whm_cfg |= val << WHM_PHY_WARN;
				if (*endptr != '\0')
					goto syntax_error;
			}
			else /* Unknown parameter name */
				continue ;
		}
	}

	fclose (f);
	wpa_printf(MSG_DEBUG, "WHM: Reading config file %s successful (%d lines)", fname, line);
	return 0; /* success */

syntax_error:
	fclose (f);
	*whm_cfg = DEFAULT_WHM;
	wpa_printf(MSG_ERROR, "WHM: Syntax error in config file '%s', line %d.", fname, line);
	return -1;
}

int hostapd_config_fill(struct hostapd_config *conf,
			       struct hostapd_bss_config *bss,
			       const char *buf, char *pos, int line)
{
	if (os_strcmp(buf, "interface") == 0) {
		os_strlcpy(conf->bss[0]->iface, pos,
			   sizeof(conf->bss[0]->iface));
	} else if (os_strcmp(buf, "bridge") == 0) {
		os_strlcpy(bss->bridge, pos, sizeof(bss->bridge));
	} else if (os_strcmp(buf, "vlan_bridge") == 0) {
		os_strlcpy(bss->vlan_bridge, pos, sizeof(bss->vlan_bridge));
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "wds_bridge") == 0) {
		os_strlcpy(bss->wds_bridge, pos, sizeof(bss->wds_bridge));
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "driver") == 0) {
		int j;
		const struct wpa_driver_ops *driver = NULL;

		for (j = 0; wpa_drivers[j]; j++) {
			if (os_strcmp(pos, wpa_drivers[j]->name) == 0) {
				driver = wpa_drivers[j];
				break;
			}
		}
		if (!driver) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid/unknown driver '%s'",
				   line, pos);
			return 1;
		}
		conf->driver = driver;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "driver_params") == 0) {
		os_free(conf->driver_params);
		conf->driver_params = os_strdup(pos);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "debug") == 0) {
		wpa_printf(MSG_DEBUG, "Line %d: DEPRECATED: 'debug' configuration variable is not used anymore",
			   line);
	} else if (os_strcmp(buf, "logger_syslog_level") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, HOSTAPD_LEVEL_DEBUG_VERBOSE, HOSTAPD_LEVEL_WARNING)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid logger_syslog_level %d", line, val);
			return 1;
		}
		bss->logger_syslog_level = val;
	} else if (os_strcmp(buf, "logger_stdout_level") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, HOSTAPD_LEVEL_DEBUG_VERBOSE, HOSTAPD_LEVEL_WARNING)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid logger_stdout_level %d",line, val);
			return 1;
		}
		bss->logger_stdout_level = val;
	} else if (os_strcmp(buf, "logger_syslog") == 0) {
		bss->logger_syslog = atoi(pos);
	} else if (os_strcmp(buf, "logger_stdout") == 0) {
		bss->logger_stdout = atoi(pos);
	} else if (os_strcmp(buf, "dump_file") == 0) {
		wpa_printf(MSG_INFO, "Line %d: DEPRECATED: 'dump_file' configuration variable is not used anymore",
			   line);
	} else if (os_strcmp(buf, "ssid") == 0) {
		bss->ssid.ssid_len = os_strlen(pos);
		if (bss->ssid.ssid_len > SSID_MAX_LEN ||
		    bss->ssid.ssid_len < 1) {
			wpa_printf(MSG_ERROR, "Line %d: invalid SSID '%s'",
				   line, pos);
			return 1;
		}
		os_memcpy(bss->ssid.ssid, pos, bss->ssid.ssid_len);
		bss->ssid.ssid_set = 1;
	} else if (os_strcmp(buf, "ssid2") == 0) {
		size_t slen;
		char *str = wpa_config_parse_string(pos, &slen);
		if (str == NULL || slen < 1 || slen > SSID_MAX_LEN) {
			wpa_printf(MSG_ERROR, "Line %d: invalid SSID '%s'",
				   line, pos);
			os_free(str);
			return 1;
		}
		os_memcpy(bss->ssid.ssid, str, slen);
		bss->ssid.ssid_len = slen;
		bss->ssid.ssid_set = 1;
		os_free(str);
	} else if (os_strcmp(buf, "utf8_ssid") == 0) {
		bss->ssid.utf8_ssid = atoi(pos) > 0;
	} else if (os_strcmp(buf, "macaddr_acl") == 0) {
		enum macaddr_acl acl = atoi(pos);

		if (acl != ACCEPT_UNLESS_DENIED &&
		    acl != DENY_UNLESS_ACCEPTED &&
		    acl != USE_EXTERNAL_RADIUS_AUTH) {
			wpa_printf(MSG_ERROR, "Line %d: unknown macaddr_acl %d",
				   line, acl);
			return 1;
		}
		bss->macaddr_acl = acl;
	} else if (os_strcmp(buf, "accept_mac_file") == 0) {
		if (hostapd_config_read_maclist(pos, &bss->accept_mac,
						&bss->num_accept_mac)) {
			wpa_printf(MSG_ERROR, "Line %d: Failed to read accept_mac_file '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "deny_mac_file") == 0) {
		if (hostapd_config_read_maclist(pos, &bss->deny_mac,
						&bss->num_deny_mac)) {
			wpa_printf(MSG_ERROR, "Line %d: Failed to read deny_mac_file '%s'",
				   line, pos);
			return 1;
		}
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "wds_sta") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid wds_sta %d",
				line, val);
			return 1;
		}
		bss->wds_sta = val;
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "start_disabled") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid start_disabled %d",
				line, val);
			return 1;
		}
		bss->start_disabled = val;
	} else if (os_strcmp(buf, "ap_isolate") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ap_isolate %d",
				line, val);
			return 1;
		}
		bss->isolate = val;
	} else if (os_strcmp(buf, "enable_hairpin") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid enable_hairpin %d",
				line, val);
			return 1;
		}
		bss->enable_hairpin = val;
	} else if (os_strcmp(buf, "ap_max_inactivity") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_MAX_INACTIVITY_MIN, AP_MAX_INACTIVITY_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ap_max_inactivity %d",
				   line, val);
			return 1;
		}
		bss->ap_max_inactivity = val;
	} else if (os_strcmp(buf, "ap_protected_keep_alive_required") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ap_protected_keep_alive_"
			"required %d", line, val);
			return 1;
		}
		bss->ap_protected_keep_alive_required = val;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "skip_inactivity_poll") == 0) {
		bss->skip_inactivity_poll = atoi(pos);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "country_code") == 0) {
		if (pos[0] < 'A' || pos[0] > 'Z' ||
		    pos[1] < 'A' || pos[1] > 'Z') {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid country_code '%s'",
				   line, pos);
			return 1;
		}
		os_memcpy(conf->country, pos, 2);
	} else if (os_strcmp(buf, "country3") == 0) {
		conf->country[2] = strtol(pos, NULL, 16);
		if(((conf->country[2] < GLOBAL_OPERATING_CLASSES_MIN) || (conf->country[2] > GLOBAL_OPERATING_CLASSES_MAX)) &&
			(conf->country[2] != ALL_ENVIRONMENTS) && (conf->country[2] != OUTDOOR_ENVIRONMENT_ONLY) &&
			(conf->country[2] != INDOOR_ENVIRONMENT_ONLY) &&
			(conf->country[2]) != NONCOUNTRY_ENTITY) {
			wpa_printf(MSG_ERROR, "Line %d: invalid country3 value %x",
				line, conf->country[2]);
			return 1;
		}
	}
	else if (os_strcmp(buf, "dynamic_country") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid dynamic_country %d", line, val);
			return 1;
		}
		conf->dynamic_country = val;
	}
	else if (os_strcmp(buf, "ieee80211d") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ieee80211d %d",
				line, val);
			return 1;
		}
		conf->ieee80211d = val;
	} else if (os_strcmp(buf, "ieee80211h") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ieee80211h %d",
				line, val);
			return 1;
		}
		conf->ieee80211h = val;
	} else if (os_strcmp(buf, "sub_band_dfs") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_RANGE(val, SUBBAND_DFS_MIN, SUBBAND_DFS_MAX)) {
			wpa_printf(MSG_ERROR, "Invalid sub_band_dfs value, must be in range 0..60");
			return 1;
		}
		conf->sub_band_dfs = val;
	} else if (os_strcmp(buf, "ieee8021x") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ieee80211x %d",
				line, val);
			return 1;
		}
		bss->ieee802_1x = val;
	} else if (os_strcmp(buf, "eapol_version") == 0) {
		int eapol_version = atoi(pos);

#ifdef CONFIG_MACSEC
		if (eapol_version < 1 || eapol_version > 3) {
#else /* CONFIG_MACSEC */
		if (eapol_version < 1 || eapol_version > 2) {
#endif /* CONFIG_MACSEC */
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid EAPOL version (%d): '%s'.",
				   line, eapol_version, pos);
			return 1;
		}
		bss->eapol_version = eapol_version;
		wpa_printf(MSG_DEBUG, "eapol_version=%d", bss->eapol_version);
  } else if (os_strcmp(buf, "ap_max_num_sta") == 0) {
    int val = atoi(pos);
    if (!IS_VALID_RANGE(val, 0, MAX_STA_COUNT)) {
      wpa_printf(MSG_ERROR, "Line %d: Invalid "
          "ap_max_num_sta=%d; allowed range "
          "0..%d", line, val,
          MAX_STA_COUNT);
      return 1;
    }
    conf->ap_max_num_sta = val;
	} else if (os_strcmp(buf, "testbed_mode") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid testbed_mode %d",
				line, val);
			return 1;
		}
		conf->testbed_mode = val;
#ifdef CONFIG_WIFI_CERTIFICATION
		extern int wpa_cert;
		wpa_cert = val;
#endif
#ifdef EAP_SERVER
	} else if (os_strcmp(buf, "eap_authenticator") == 0) {
		bss->eap_server = atoi(pos);
		wpa_printf(MSG_ERROR, "Line %d: obsolete eap_authenticator used; this has been renamed to eap_server", line);
	} else if (os_strcmp(buf, "eap_server") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid eap_server %d",
				line, val);
			return 1;
		}
		bss->eap_server = val;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "eap_user_file") == 0) {
		if (hostapd_config_read_eap_user(pos, bss))
			return 1;
	} else if (os_strcmp(buf, "ca_cert") == 0) {
		os_free(bss->ca_cert);
		bss->ca_cert = os_strdup(pos);
	} else if (os_strcmp(buf, "server_cert") == 0) {
		os_free(bss->server_cert);
		bss->server_cert = os_strdup(pos);
	} else if (os_strcmp(buf, "server_cert2") == 0) {
		os_free(bss->server_cert2);
		bss->server_cert2 = os_strdup(pos);
	} else if (os_strcmp(buf, "private_key") == 0) {
		os_free(bss->private_key);
		bss->private_key = os_strdup(pos);
	} else if (os_strcmp(buf, "private_key2") == 0) {
		os_free(bss->private_key2);
		bss->private_key2 = os_strdup(pos);
	} else if (os_strcmp(buf, "private_key_passwd") == 0) {
		os_free(bss->private_key_passwd);
		bss->private_key_passwd = os_strdup(pos);
	} else if (os_strcmp(buf, "private_key_passwd2") == 0) {
		os_free(bss->private_key_passwd2);
		bss->private_key_passwd2 = os_strdup(pos);
	} else if (os_strcmp(buf, "check_cert_subject") == 0) {
		if (!pos[0]) {
			wpa_printf(MSG_ERROR, "Line %d: unknown check_cert_subject '%s'",
				   line, pos);
			return 1;
		}
		os_free(bss->check_cert_subject);
		bss->check_cert_subject = os_strdup(pos);
		if (!bss->check_cert_subject)
			return 1;
	} else if (os_strcmp(buf, "check_crl") == 0) {
		bss->check_crl = atoi(pos);
	} else if (os_strcmp(buf, "check_crl_strict") == 0) {
		bss->check_crl_strict = atoi(pos);
	} else if (os_strcmp(buf, "crl_reload_interval") == 0) {
		bss->crl_reload_interval = atoi(pos);
	} else if (os_strcmp(buf, "tls_session_lifetime") == 0) {
		bss->tls_session_lifetime = atoi(pos);
	} else if (os_strcmp(buf, "tls_flags") == 0) {
		bss->tls_flags = parse_tls_flags(pos);
	} else if (os_strcmp(buf, "max_auth_rounds") == 0) {
		bss->max_auth_rounds = atoi(pos);
	} else if (os_strcmp(buf, "max_auth_rounds_short") == 0) {
		bss->max_auth_rounds_short = atoi(pos);
	} else if (os_strcmp(buf, "ocsp_stapling_response") == 0) {
		os_free(bss->ocsp_stapling_response);
		bss->ocsp_stapling_response = os_strdup(pos);
	} else if (os_strcmp(buf, "ocsp_stapling_response_multi") == 0) {
		os_free(bss->ocsp_stapling_response_multi);
		bss->ocsp_stapling_response_multi = os_strdup(pos);
	} else if (os_strcmp(buf, "dh_file") == 0) {
		os_free(bss->dh_file);
		bss->dh_file = os_strdup(pos);
	} else if (os_strcmp(buf, "openssl_ciphers") == 0) {
		os_free(bss->openssl_ciphers);
		bss->openssl_ciphers = os_strdup(pos);
	} else if (os_strcmp(buf, "openssl_ecdh_curves") == 0) {
		os_free(bss->openssl_ecdh_curves);
		bss->openssl_ecdh_curves = os_strdup(pos);
	} else if (os_strcmp(buf, "fragment_size") == 0) {
		bss->fragment_size = atoi(pos);
	} else if (os_strcmp(buf, "auth_fail_blacklist_duration") == 0) {
		bss->auth_fail_blacklist_duration = atoi(pos);
	} else if (os_strcmp(buf, "max_eap_failure") == 0) {
		bss->max_eap_failure = atoi(pos);
	} else if (os_strcmp(buf, "eap_req_id_retry_interval") == 0) {
		bss->eap_req_id_retry_interval = atoi(pos);
	} else if (os_strcmp(buf, "eap_aaa_req_retries") == 0) {
		bss->eap_aaa_req_retries = atoi(pos);
	} else if (os_strcmp(buf, "eap_aaa_req_timeout") == 0) {
		bss->eap_aaa_req_timeout = atoi(pos);
	} else if (os_strcmp(buf, "failed_authentication_quiet_period") == 0) {
		bss->auth_quiet_period = atoi(pos);
#ifdef EAP_SERVER_FAST
	} else if (os_strcmp(buf, "pac_opaque_encr_key") == 0) {
		os_free(bss->pac_opaque_encr_key);
		bss->pac_opaque_encr_key = os_malloc(16);
		if (bss->pac_opaque_encr_key == NULL) {
			wpa_printf(MSG_ERROR,
				   "Line %d: No memory for pac_opaque_encr_key",
				   line);
			return 1;
		} else if (hexstr2bin(pos, bss->pac_opaque_encr_key, 16)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid pac_opaque_encr_key",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "eap_fast_a_id") == 0) {
		size_t idlen = os_strlen(pos);
		if (idlen & 1) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid eap_fast_a_id",
				   line);
			return 1;
		}
		os_free(bss->eap_fast_a_id);
		bss->eap_fast_a_id = os_malloc(idlen / 2);
		if (bss->eap_fast_a_id == NULL ||
		    hexstr2bin(pos, bss->eap_fast_a_id, idlen / 2)) {
			wpa_printf(MSG_ERROR, "Line %d: Failed to parse eap_fast_a_id",
				   line);
			os_free(bss->eap_fast_a_id);
			bss->eap_fast_a_id = NULL;
			return 1;
		} else {
			bss->eap_fast_a_id_len = idlen / 2;
		}
	} else if (os_strcmp(buf, "eap_fast_a_id_info") == 0) {
		os_free(bss->eap_fast_a_id_info);
		bss->eap_fast_a_id_info = os_strdup(pos);
	} else if (os_strcmp(buf, "eap_fast_prov") == 0) {
		bss->eap_fast_prov = atoi(pos);
	} else if (os_strcmp(buf, "pac_key_lifetime") == 0) {
		bss->pac_key_lifetime = atoi(pos);
	} else if (os_strcmp(buf, "pac_key_refresh_time") == 0) {
		bss->pac_key_refresh_time = atoi(pos);
#endif /* EAP_SERVER_FAST */
#ifdef EAP_SERVER_TEAP
	} else if (os_strcmp(buf, "eap_teap_auth") == 0) {
		int val = atoi(pos);

		if (val < 0 || val > 2) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid eap_teap_auth value",
				   line);
			return 1;
		}
		bss->eap_teap_auth = val;
	} else if (os_strcmp(buf, "eap_teap_pac_no_inner") == 0) {
		bss->eap_teap_pac_no_inner = atoi(pos);
	} else if (os_strcmp(buf, "eap_teap_separate_result") == 0) {
		bss->eap_teap_separate_result = atoi(pos);
	} else if (os_strcmp(buf, "eap_teap_id") == 0) {
		bss->eap_teap_id = atoi(pos);
#endif /* EAP_SERVER_TEAP */
#ifdef EAP_SERVER_SIM
	} else if (os_strcmp(buf, "eap_sim_db") == 0) {
		os_free(bss->eap_sim_db);
		bss->eap_sim_db = os_strdup(pos);
	} else if (os_strcmp(buf, "eap_sim_db_timeout") == 0) {
		bss->eap_sim_db_timeout = atoi(pos);
	} else if (os_strcmp(buf, "eap_sim_aka_result_ind") == 0) {
		bss->eap_sim_aka_result_ind = atoi(pos);
	} else if (os_strcmp(buf, "eap_sim_id") == 0) {
		bss->eap_sim_id = atoi(pos);
#endif /* EAP_SERVER_SIM */
#ifdef EAP_SERVER_TNC
	} else if (os_strcmp(buf, "tnc") == 0) {
		bss->tnc = atoi(pos);
#endif /* EAP_SERVER_TNC */
#ifdef EAP_SERVER_PWD
	} else if (os_strcmp(buf, "pwd_group") == 0) {
		bss->pwd_group = atoi(pos);
#endif /* EAP_SERVER_PWD */
#ifdef CONFIG_ERP
	} else if (os_strcmp(buf, "eap_server_erp") == 0) {
		bss->eap_server_erp = atoi(pos);
#endif /* CONFIG_ERP */
#endif /* CONFIG_ALLOW_HOSTAPD */
#endif /* EAP_SERVER */
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "eap_message") == 0) {
		char *term;
		os_free(bss->eap_req_id_text);
		bss->eap_req_id_text = os_strdup(pos);
		if (bss->eap_req_id_text == NULL) {
			wpa_printf(MSG_ERROR, "Line %d: Failed to allocate memory for eap_req_id_text",
				   line);
			return 1;
		}
		bss->eap_req_id_text_len = os_strlen(bss->eap_req_id_text);
		term = os_strstr(bss->eap_req_id_text, "\\0");
		if (term) {
			*term++ = '\0';
			os_memmove(term, term + 1,
				   bss->eap_req_id_text_len -
				   (term - bss->eap_req_id_text) - 1);
			bss->eap_req_id_text_len--;
		}
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "erp_send_reauth_start") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid erp_send_reauth_start %d",
				line, val);
			return 1;
		}
		bss->erp_send_reauth_start = val;
	} else if (os_strcmp(buf, "erp_domain") == 0) {
		rsize_t len = strnlen_s(pos, AP_MAX_VALID_CHAR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, AP_MAX_VALID_CHAR - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid erp_domain len %zu",
				   line, len);
			return 1;
		}
		os_free(bss->erp_domain);
		bss->erp_domain = os_strdup(pos);
#ifdef CONFIG_ALLOW_HOSTAPD
#ifdef CONFIG_WEP
	} else if (os_strcmp(buf, "wep_key_len_broadcast") == 0) {
		int val = atoi(pos);

		if (val < 0 || val > 13) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid WEP key len %d (= %d bits)",
				   line, val, val * 8);
			return 1;
		}
		bss->default_wep_key_len = val;
	} else if (os_strcmp(buf, "wep_key_len_unicast") == 0) {
		int val = atoi(pos);

		if (val < 0 || val > 13) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid WEP key len %d (= %d bits)",
				   line, val, val * 8);
			return 1;
		}
		bss->individual_wep_key_len = val;
	} else if (os_strcmp(buf, "wep_rekey_period") == 0) {
		bss->wep_rekeying_period = atoi(pos);
		if (bss->wep_rekeying_period < 0) {
			wpa_printf(MSG_ERROR, "Line %d: invalid period %d",
				   line, bss->wep_rekeying_period);
			return 1;
		}
#endif /* CONFIG_WEP */
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "eap_reauth_period") == 0) {
		int val = atoi(pos);
		if (val < 0) {
			wpa_printf(MSG_ERROR, "Line %d: invalid period %d",
				   line, bss->eap_reauth_period);
			return 1;
		}
		bss->eap_reauth_period = val;
	} else if (os_strcmp(buf, "eapol_key_index_workaround") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: eapol_key_index_workaround %d, range 0..1",
				   line, val);
			return 1;
		}
		bss->eapol_key_index_workaround = val;
#ifdef CONFIG_IAPP
	} else if (os_strcmp(buf, "iapp_interface") == 0) {
		wpa_printf(MSG_INFO, "DEPRECATED: iapp_interface not used");
#endif /* CONFIG_IAPP */
	} else if (os_strcmp(buf, "own_ip_addr") == 0) {
		if (hostapd_parse_ip_addr(pos, &bss->own_ip_addr)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid IP address '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "nas_identifier") == 0) {
		os_free(bss->nas_identifier);
		bss->nas_identifier = os_strdup(pos);
#ifndef CONFIG_NO_RADIUS
	} else if (os_strcmp(buf, "radius_client_addr") == 0) {
		if (hostapd_parse_ip_addr(pos, &bss->radius->client_addr)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid IP address '%s'",
				   line, pos);
			return 1;
		}
		bss->radius->force_client_addr = 1;
	} else if (os_strcmp(buf, "auth_server_addr") == 0) {
		if (hostapd_config_read_radius_addr(
			    &bss->radius->auth_servers,
			    &bss->radius->num_auth_servers, pos, 1812,
			    &bss->radius->auth_server)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid IP address '%s'",
				   line, pos);
			return 1;
		}
	} else if (bss->radius->auth_server &&
		   os_strcmp(buf, "auth_server_addr_replace") == 0) {
		if (hostapd_parse_ip_addr(pos,
					  &bss->radius->auth_server->addr)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid IP address '%s'",
				   line, pos);
			return 1;
		}
	} else if (bss->radius->auth_server &&
		   os_strcmp(buf, "auth_server_port") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_PORT_MIN, AP_PORT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid auth_server_port %d",
				   line, val);
			return 1;
		}
		bss->radius->auth_server->port = val;
	} else if (bss->radius->auth_server &&
		   os_strcmp(buf, "auth_server_shared_secret") == 0) {
		int len = os_strlen(pos);
		if (len == 0) {
			/* RFC 2865, Ch. 3 */
			wpa_printf(MSG_ERROR, "Line %d: empty shared secret is not allowed",
				   line);
			return 1;
		}
		os_free(bss->radius->auth_server->shared_secret);
		bss->radius->auth_server->shared_secret = (u8 *) os_strdup(pos);
		bss->radius->auth_server->shared_secret_len = len;
	} else if (os_strcmp(buf, "acct_server_addr") == 0) {
		if (hostapd_config_read_radius_addr(
			    &bss->radius->acct_servers,
			    &bss->radius->num_acct_servers, pos, 1813,
			    &bss->radius->acct_server)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid IP address '%s'",
				   line, pos);
			return 1;
		}
	} else if (bss->radius->acct_server &&
		   os_strcmp(buf, "acct_server_addr_replace") == 0) {
		if (hostapd_parse_ip_addr(pos,
					  &bss->radius->acct_server->addr)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid IP address '%s'",
				   line, pos);
			return 1;
		}
	} else if (bss->radius->acct_server &&
		   os_strcmp(buf, "acct_server_port") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_PORT_MIN, AP_PORT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid acct_server_port %d",
				   line, val);
			return 1;
		}
		bss->radius->acct_server->port = val;
	} else if (bss->radius->acct_server &&
		   os_strcmp(buf, "acct_server_shared_secret") == 0) {
		int len = os_strlen(pos);
		if (len == 0) {
			/* RFC 2865, Ch. 3 */
			wpa_printf(MSG_ERROR, "Line %d: empty shared secret is not allowed",
				   line);
			return 1;
		}
		os_free(bss->radius->acct_server->shared_secret);
		bss->radius->acct_server->shared_secret = (u8 *) os_strdup(pos);
		bss->radius->acct_server->shared_secret_len = len;
	} else if (os_strcmp(buf, "radius_retry_primary_interval") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_TIMER_MIN, AP_TIMER_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid retry_primary_interval %d",
				   line, val);
			return 1;
		}
		bss->radius->retry_primary_interval = val;
	} else if (os_strcmp(buf, "radius_acct_interim_interval") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, RADIUS_ACCT_INTERIM_INTERVAL_MIN, RADIUS_ACCT_INTERIM_INTERVAL_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid radius_acct_interim_interval value %d",
				line, val);
			return 1;
		}
		bss->acct_interim_interval = val;
	} else if (os_strcmp(buf, "radius_request_cui") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid radius_request_cui %d",
				line, val);
			return 1;
		}
		bss->radius_request_cui = val;
	} else if (os_strcmp(buf, "radius_auth_req_attr") == 0) {
		struct hostapd_radius_attr *attr, *a;
		attr = hostapd_parse_radius_attr(pos);
		if (attr == NULL) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid radius_auth_req_attr",
				   line);
			return 1;
		} else if (bss->radius_auth_req_attr == NULL) {
			bss->radius_auth_req_attr = attr;
		} else {
			a = bss->radius_auth_req_attr;
			while (a->next)
				a = a->next;
			a->next = attr;
		}
	} else if (os_strcmp(buf, "radius_acct_req_attr") == 0) {
		struct hostapd_radius_attr *attr, *a;
		attr = hostapd_parse_radius_attr(pos);
		if (attr == NULL) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid radius_acct_req_attr",
				   line);
			return 1;
		} else if (bss->radius_acct_req_attr == NULL) {
			bss->radius_acct_req_attr = attr;
		} else {
			a = bss->radius_acct_req_attr;
			while (a->next)
				a = a->next;
			a->next = attr;
		}
	} else if (os_strcmp(buf, "radius_req_attr_sqlite") == 0) {
		os_free(bss->radius_req_attr_sqlite);
		bss->radius_req_attr_sqlite = os_strdup(pos);
	} else if (os_strcmp(buf, "radius_das_port") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_PORT_MIN, AP_PORT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid radius_das_port %d",
				line, val);
			return 1;
		}
		bss->radius_das_port = val;
	} else if (os_strcmp(buf, "radius_das_client") == 0) {
		if (hostapd_parse_das_client(bss, pos) < 0) {
			wpa_printf(MSG_ERROR, "Line %d: invalid DAS client",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "radius_das_time_window") == 0) {
		bss->radius_das_time_window = strtoul(pos, NULL, 10);
	} else if (os_strcmp(buf, "radius_das_require_event_timestamp") == 0) {
		bss->radius_das_require_event_timestamp = atoi(pos);
	} else if (os_strcmp(buf, "radius_das_require_message_authenticator") ==
		   0) {
		int val = atoi(pos);
		if(!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: radius_das_require_message_authenticator %d, range 0..1", line, val);
			return 1;
		}
		bss->radius_das_require_message_authenticator = val;
#endif /* CONFIG_NO_RADIUS */
	} else if (os_strcmp(buf, "auth_algs") == 0) {
		int val = atoi(pos);
		if (val == 0) {
			wpa_printf(MSG_ERROR, "Line %d: no authentication algorithms allowed",
				   line);
			return 1;
		}
		bss->auth_algs = val;
	} else if (os_strcmp(buf, "max_num_sta") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, 0, MAX_STA_COUNT)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid max_num_sta=%d; allowed range 0..%d",
				   line, val, MAX_STA_COUNT);
			return 1;
		}
		bss->max_num_sta = val;
	} else if (os_strcmp(buf, "num_vrt_bkh_netdevs") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_RANGE(val, NUM_VRT_BKH_NETDEVS_MIN, NUM_VRT_BKH_NETDEVS_MAX)) {
			 wpa_printf(MSG_ERROR, "Line %d: Invalid num_vrt_bkh_netdevs=%d; allowed range 0..%d",
				    line, val, NUM_VRT_BKH_NETDEVS_MAX);
			 return 1;
		}
		bss->num_vrt_bkh_netdevs = val;
	} else if (os_strcmp(buf, "num_res_sta") == 0) {
    bss->num_res_sta = atoi(pos);
    if (bss->num_res_sta < 0) {
      wpa_printf(MSG_ERROR, "Line %d: Invalid "
          "num_res_sta=%d; allowed range "
          "0..max_num_sta", line, bss->num_res_sta);
      return 1;
    }
	} else if (os_strcmp(buf, "wpa") == 0) {
		bss->wpa = atoi(pos);
	} else if (os_strcmp(buf, "wpa_group_rekey") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_TIMER_MIN, AP_TIMER_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid wpa_group_rekey %d",
				   line, val);
			return 1;
		}
		bss->wpa_group_rekey = val;
		bss->wpa_group_rekey_set = 1;
	} else if (os_strcmp(buf, "wpa_strict_rekey") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid wpa_strict_rekey %d",
				line, val);
			return 1;
		}
		bss->wpa_strict_rekey = val;
	} else if (os_strcmp(buf, "wpa_gmk_rekey") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_TIMER_MIN, AP_TIMER_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid wpa_gmk_rekey %d",
				   line, val);
			return 1;
		}
		bss->wpa_gmk_rekey = val;
	} else if (os_strcmp(buf, "wpa_ptk_rekey") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_TIMER_MIN, AP_TIMER_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid wpa_ptk_rekey %d",
				   line, val);
			return 1;
		}
		bss->wpa_ptk_rekey = val;
	} else if (os_strcmp(buf, "wpa_deny_ptk0_rekey") == 0) {
		bss->wpa_deny_ptk0_rekey = atoi(pos);
		if (bss->wpa_deny_ptk0_rekey < 0 ||
		    bss->wpa_deny_ptk0_rekey > 2) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid wpa_deny_ptk0_rekey=%d; allowed range 0..2",
				   line, bss->wpa_deny_ptk0_rekey);
			return 1;
		}
	} else if (os_strcmp(buf, "wpa_group_update_count") == 0) {
		char *endp;
		unsigned long val = strtoul(pos, &endp, 0);

		if (*endp || val < 1 || val > (u32) -1) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid wpa_group_update_count=%lu; allowed range 1..4294967295",
				   line, val);
			return 1;
		}
		bss->wpa_group_update_count = (u32) val;
	} else if (os_strcmp(buf, "wpa_pairwise_update_count") == 0) {
		char *endp;
		unsigned long val = strtoul(pos, &endp, 0);

		if (*endp || val < 1 || val > (u32) -1) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid wpa_pairwise_update_count=%lu; allowed range 1..4294967295",
				   line, val);
			return 1;
		}
		bss->wpa_pairwise_update_count = (u32) val;
	} else if (os_strcmp(buf, "wpa_disable_eapol_key_retries") == 0) {
		int val =  atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid wpa_disable_eapol_key_retries %d",
				line, val);
			return 1;
		}
		bss->wpa_disable_eapol_key_retries = val;
	} else if (os_strcmp(buf, "wpa_passphrase") == 0) {
		int len = os_strlen(pos);
		if (!IS_VALID_RANGE(len, WPA_PASSPHRASE_LEN_MIN, WPA_PASSPHRASE_LEN_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid WPA passphrase length %d (expected 8..63)",
				   line, len);
			return 1;
		}
		os_free(bss->ssid.wpa_passphrase);
		bss->ssid.wpa_passphrase = os_strdup(pos);
		if (bss->ssid.wpa_passphrase) {
			hostapd_config_clear_wpa_psk(&bss->ssid.wpa_psk);
			bss->ssid.wpa_passphrase_set = 1;
		}
	} else if (os_strcmp(buf, "wpa_psk") == 0) {
		hostapd_config_clear_wpa_psk(&bss->ssid.wpa_psk);
		bss->ssid.wpa_psk = os_zalloc(sizeof(struct hostapd_wpa_psk));
		if (bss->ssid.wpa_psk == NULL)
			return 1;
		if (hexstr2bin(pos, bss->ssid.wpa_psk->psk, PMK_LEN) ||
		    pos[PMK_LEN * 2] != '\0') {
			wpa_printf(MSG_ERROR, "Line %d: Invalid PSK '%s'.",
				   line, pos);
			hostapd_config_clear_wpa_psk(&bss->ssid.wpa_psk);
			return 1;
		}
		bss->ssid.wpa_psk->group = 1;
		os_free(bss->ssid.wpa_passphrase);
		bss->ssid.wpa_passphrase = NULL;
		bss->ssid.wpa_psk_set = 1;
	} else if (os_strcmp(buf, "wpa_psk_file") == 0) {
		os_free(bss->ssid.wpa_psk_file);
		bss->ssid.wpa_psk_file = os_strdup(pos);
		if (!bss->ssid.wpa_psk_file) {
			wpa_printf(MSG_ERROR, "Line %d: allocation failed",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "mem_only_cred") == 0) {
		bss->ssid.request_credentials = (atoi(pos)) ? 1 : 0;
	} else if (os_strcmp(buf, "cred_req_timeout") == 0) {
		conf->cred_req_timeout = atoi(pos);
		if (conf->cred_req_timeout <= 0) {
			wpa_printf(MSG_ERROR,
						"Line %d: wrong credentials request timeout %d",
						line, conf->cred_req_timeout);
			return 1;
		}
	} else if (os_strcmp(buf, "wpa_key_mgmt") == 0) {
		bss->wpa_key_mgmt = hostapd_config_parse_key_mgmt(line, pos);
		if (bss->wpa_key_mgmt == -1)
			return 1;
	} else if (os_strcmp(buf, "wpa_psk_radius") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_RANGE(val, PSK_RADIUS_IGNORED, PSK_RADIUS_REQUIRED)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: unknown wpa_psk_radius %d",
				   line, val);
			return 1;
		}
		bss->wpa_psk_radius = val;
	} else if (os_strcmp(buf, "wpa_pairwise") == 0) {
		bss->wpa_pairwise = hostapd_config_parse_cipher(line, pos);
		if (bss->wpa_pairwise == -1 || bss->wpa_pairwise == 0)
			return 1;
		if (bss->wpa_pairwise &
		    (WPA_CIPHER_NONE | WPA_CIPHER_WEP40 | WPA_CIPHER_WEP104)) {
			wpa_printf(MSG_ERROR, "Line %d: unsupported pairwise cipher suite '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "rsn_pairwise") == 0) {
		bss->rsn_pairwise = hostapd_config_parse_cipher(line, pos);
		if (bss->rsn_pairwise == -1 || bss->rsn_pairwise == 0)
			return 1;
		if (bss->rsn_pairwise &
		    (WPA_CIPHER_NONE | WPA_CIPHER_WEP40 | WPA_CIPHER_WEP104)) {
			wpa_printf(MSG_ERROR, "Line %d: unsupported pairwise cipher suite '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "group_cipher") == 0) {
		bss->group_cipher = hostapd_config_parse_cipher(line, pos);
		if (bss->group_cipher == -1 || bss->group_cipher == 0)
			return 1;
		if (bss->group_cipher != WPA_CIPHER_TKIP &&
		    bss->group_cipher != WPA_CIPHER_CCMP &&
		    bss->group_cipher != WPA_CIPHER_GCMP &&
		    bss->group_cipher != WPA_CIPHER_GCMP_256 &&
		    bss->group_cipher != WPA_CIPHER_CCMP_256) {
			wpa_printf(MSG_ERROR,
				   "Line %d: unsupported group cipher suite '%s'",
				   line, pos);
			return 1;
		}
#ifdef CONFIG_RSN_PREAUTH
	} else if (os_strcmp(buf, "rsn_preauth") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rsn_preauth %d",
				line, val);
			return 1;
		}
		bss->rsn_preauth = val;
	} else if (os_strcmp(buf, "rsn_preauth_interfaces") == 0) {
		os_free(bss->rsn_preauth_interfaces);
		bss->rsn_preauth_interfaces = os_strdup(pos);
#endif /* CONFIG_RSN_PREAUTH */
	} else if (os_strcmp(buf, "peerkey") == 0) {
		wpa_printf(MSG_INFO,
			   "Line %d: Obsolete peerkey parameter ignored", line);
#ifdef CONFIG_IEEE80211R_AP
	} else if (os_strcmp(buf, "mobility_domain") == 0) {
		if (os_strlen(pos) != 2 * MOBILITY_DOMAIN_ID_LEN ||
		    hexstr2bin(pos, bss->mobility_domain,
			       MOBILITY_DOMAIN_ID_LEN) != 0) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid mobility_domain '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "r1_key_holder") == 0) {
		if (os_strlen(pos) != 2 * FT_R1KH_ID_LEN ||
		    hexstr2bin(pos, bss->r1_key_holder, FT_R1KH_ID_LEN) != 0) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid r1_key_holder '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "r0_key_lifetime") == 0) {
		/* DEPRECATED: Use ft_r0_key_lifetime instead. */
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, R0_KEY_LIFETIME_MIN, R0_KEY_LIFETIME_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid r0_key_lifetime value %d",
				   line, val);
			return 1;
		}
		bss->r0_key_lifetime = atoi(pos) * 60;
	} else if (os_strcmp(buf, "ft_r0_key_lifetime") == 0) {
		u32 val = atoi(pos);
		if (!IS_VALID_RANGE(val, FT_R0_KEY_LIFETIME_MIN, FT_R0_KEY_LIFETIME_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ft_r0_key_lifetime value %u",
				line, val);
			return 1;
		}
		bss->r0_key_lifetime = val;
	} else if (os_strcmp(buf, "r1_max_key_lifetime") == 0) {
		bss->r1_max_key_lifetime = atoi(pos);
	} else if (os_strcmp(buf, "reassociation_deadline") == 0) {
		u32 val = atoi(pos);
		if (!IS_VALID_RANGE(val, REASSOCIATION_DEADLINE_MIN, REASSOCIATION_DEADLINE_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid reassociation_deadline value %u",
				line, val);
			return 1;
		}
		bss->reassociation_deadline = val;
	} else if (os_strcmp(buf, "rkh_pos_timeout") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_TIMER_MIN, AP_TIMER_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rkh_pos_timeout %d",
				   line, val);
			return 1;
		}
		bss->rkh_pos_timeout = val;
	} else if (os_strcmp(buf, "rkh_neg_timeout") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_TIMER_MIN, AP_TIMER_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rkh_neg_timeout %d",
				   line, val);
			return 1;
		}
		bss->rkh_neg_timeout = val;
	} else if (os_strcmp(buf, "rkh_pull_timeout") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, RKH_PULL_TIMEOUT_MIN, RKH_PULL_TIMEOUT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rkh_pull_timeout %d",
				   line, val);
			return 1;
		}
		bss->rkh_pull_timeout = val;
	} else if (os_strcmp(buf, "rkh_pull_retries") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, RKH_PULL_RETRIES_MIN, RKH_PULL_RETRIES_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rkh_pull_retries %d",
				   line, val);
			return 1;
		}
		bss->rkh_pull_retries = val;
	} else if (os_strcmp(buf, "r0kh") == 0) {
		if (add_r0kh(bss, pos) < 0) {
			wpa_printf(MSG_DEBUG, "Line %d: Invalid r0kh '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "r1kh") == 0) {
		if (add_r1kh(bss, pos) < 0) {
			wpa_printf(MSG_DEBUG, "Line %d: Invalid r1kh '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "pmk_r1_push") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid pmk_r1_push %d",
				line, val);
			return 1;
		}
		bss->pmk_r1_push = val;
	} else if (os_strcmp(buf, "ft_over_ds") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ft_over_ds %d",
				line, val);
			return 1;
		}
		bss->ft_over_ds = val;
	} else if (os_strcmp(buf, "ft_psk_generate_local") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ft_psk_generate_local %d",
				line, val);
			return 1;
		}
		bss->ft_psk_generate_local = val;
#endif /* CONFIG_IEEE80211R_AP */
#ifndef CONFIG_NO_CTRL_IFACE
	} else if (os_strcmp(buf, "ctrl_interface") == 0) {
		os_free(bss->ctrl_interface);
		bss->ctrl_interface = os_strdup(pos);
	} else if (os_strcmp(buf, "ctrl_interface_group") == 0) {
#ifndef CONFIG_NATIVE_WINDOWS
		struct group *grp;
		char *endp;
		const char *group = pos;

		grp = getgrnam(group);
		if (grp) {
			bss->ctrl_interface_gid = grp->gr_gid;
			bss->ctrl_interface_gid_set = 1;
			wpa_printf(MSG_DEBUG, "ctrl_interface_group=%d (from group name '%s')",
				   bss->ctrl_interface_gid, group);
			return 0;
		}

		/* Group name not found - try to parse this as gid */
		bss->ctrl_interface_gid = strtol(group, &endp, 10);
		if (*group == '\0' || *endp != '\0') {
			wpa_printf(MSG_DEBUG, "Line %d: Invalid group '%s'",
				   line, group);
			return 1;
		}
		bss->ctrl_interface_gid_set = 1;
		wpa_printf(MSG_DEBUG, "ctrl_interface_group=%d",
			   bss->ctrl_interface_gid);
#endif /* CONFIG_NATIVE_WINDOWS */
#endif /* CONFIG_NO_CTRL_IFACE */
#ifdef RADIUS_SERVER
	} else if (os_strcmp(buf, "radius_server_clients") == 0) {
		os_free(bss->radius_server_clients);
		bss->radius_server_clients = os_strdup(pos);
	} else if (os_strcmp(buf, "radius_server_auth_port") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_PORT_MIN, AP_PORT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid radius_server_auth_port %d",
				line, val);
			return 1;
		}
		bss->radius_server_auth_port = val;
	} else if (os_strcmp(buf, "radius_server_acct_port") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_PORT_MIN, AP_PORT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid radius_server_acct_port %d",
				line, val);
			return 1;
		}
		bss->radius_server_acct_port = val;
	} else if (os_strcmp(buf, "radius_server_ipv6") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid radius_server_ipv6 %d",
				line, val);
			return 1;
		}
		bss->radius_server_ipv6 = val;
#endif /* RADIUS_SERVER */
	} else if (os_strcmp(buf, "use_pae_group_addr") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid use_pae_group_addr %d",
				line, val);
			return 1;
		}
		bss->use_pae_group_addr = val;
	} else if (os_strcmp(buf, "hw_mode") == 0) {
		if (os_strcmp(pos, "a") == 0)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211A;
		else if (os_strcmp(pos, "b") == 0)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211B;
		else if (os_strcmp(pos, "g") == 0)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211G;
		else if (os_strcmp(pos, "ad") == 0)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211AD;
		else if (os_strcmp(pos, "any") == 0)
			conf->hw_mode = HOSTAPD_MODE_IEEE80211ANY;
		else {
			wpa_printf(MSG_ERROR, "Line %d: unknown hw_mode '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "wps_rf_bands") == 0) {
		if (os_strcmp(pos, "ad") == 0)
			bss->wps_rf_bands = WPS_RF_60GHZ;
		else if (os_strcmp(pos, "a") == 0)
			bss->wps_rf_bands = WPS_RF_50GHZ;
		else if (os_strcmp(pos, "g") == 0 ||
			 os_strcmp(pos, "b") == 0)
			bss->wps_rf_bands = WPS_RF_24GHZ;
		else if (os_strcmp(pos, "ag") == 0 ||
			 os_strcmp(pos, "ga") == 0)
			bss->wps_rf_bands = WPS_RF_24GHZ | WPS_RF_50GHZ;
		else {
			wpa_printf(MSG_ERROR,
				   "Line %d: unknown wps_rf_band '%s'",
				   line, pos);
			return 1;
		}
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "acs_exclude_dfs") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid acs_exclude_dfs %d",
				line, val);
			return 1;
		}
		conf->acs_exclude_dfs = val;
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "op_class") == 0) {
		/* op_class and channel are dependent values
		   and will be checked when configuration is read completely
		   and both variables are set. */
		conf->op_class = (u8)atoi(pos);
	} else if (os_strcmp(buf, "channel") == 0) {
		if (os_strcmp(pos, "acs_survey") == 0 || os_strcmp(pos, "acs_smart") == 0) {
#ifndef CONFIG_ACS
			wpa_printf(MSG_ERROR, "Line %d: tries to enable ACS but CONFIG_ACS disabled",
				   line);
			return 1;
#else /* CONFIG_ACS */
			conf->acs = 1;
			conf->channel = 0;
			if (!os_strcmp(pos, "acs_survey"))
				conf->acs_algo = ACS_ALGO_SURVEY;
			else
				conf->acs_algo = ACS_ALGO_SMART;
#endif /* CONFIG_ACS */
		} else if (os_strcmp(pos, "first_non_dfs") == 0) {
			conf->channel = FIRST_NON_DFS_CHAN;
			conf->acs = 0;
		} else {
			conf->channel = (u8)atoi(pos);
			conf->acs = conf->channel == 0;
#ifdef CONFIG_ACS
			conf->acs_algo = ACS_ALGO_SMART;
#endif /* CONFIG_ACS */
		}
	}
#ifdef CONFIG_ACS
	/* Smart ACS */
	else if (os_strcmp(buf, "acs_smart_info_file") == 0) {
		os_free(conf->acs_smart_info_file);
		conf->acs_smart_info_file = os_strdup(pos);
	}
	else if (os_strcmp(buf, "acs_history_file") == 0) {
		os_free(conf->acs_history_file);
		conf->acs_history_file = os_strdup(pos);
	}
	else if (os_strcmp(buf, "acs_use24overlapped") == 0) {
		int val = atoi(pos);
		conf->acs_use24overlapped = val;
	}
	else if (os_strcmp(buf, "acs_bg_scan_do_switch") == 0) {
		int val = atoi(pos);
		if ((val != 0) && (val != 1)) {
			wpa_printf(MSG_ERROR, "Line %d: Wrong value of acs_bg_scan_do_switch %d, must be 0 or 1",
				   line, val);
			return 1;
		}
		conf->acs_bg_scan_do_switch = val;
	}
	else if (os_strcmp(buf, "acs_update_do_switch") == 0) {
		int val = atoi(pos);
		if ((val != 0) && (val != 1)) {
			wpa_printf(MSG_ERROR, "Line %d: Wrong value of acs_update_do_switch %d, must be 0 or 1",
				   line, val);
			return 1;
		}
		conf->acs_update_do_switch = val;
	}
	else if (os_strcmp(buf, "acs_fallback_chan") == 0) {
		if (3 != sscanf(pos, "%d %d %d", &conf->acs_fallback_chan.primary,
						 &conf->acs_fallback_chan.secondary,
						 &conf->acs_fallback_chan.width)) {
			wpa_printf(MSG_ERROR, "Line %d: ACS SMART: invalid fallback channel %s",
				   line, pos);
			return 1;
		}
	}
	else if (os_strcmp(buf, "acs_penalty_factors") == 0) {
		int i = 0;
		if (hostapd_parse_intlist(&conf->acs_penalty_factors, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ACS penalty factors list", line);
			return 1;
		}
		/* it would be great to adopt a rule that "int-lists" contain their
		* element count in the member at index 0, then we wouldn't have to count...
		*/
		while (conf->acs_penalty_factors[i] >= 0)
			i++;
		if (i != ACS_NUM_PENALTY_FACTORS) {
			wpa_printf(MSG_ERROR, "Line %d: ACS penalty factors list has %i elements instead of %i",
				   line, i, ACS_NUM_PENALTY_FACTORS);
			return 1;
		}
		for (i = 0; i < ACS_NUM_PENALTY_FACTORS; i++) {
			if (!IS_VALID_RANGE(conf->acs_penalty_factors[i], ACS_PEN_MIN, ACS_PEN_MAX)) {
				wpa_printf(MSG_ERROR, "Line %d: Invalid ACS penalty factor %d, must be %d..%d",
					   line, conf->acs_penalty_factors[i], ACS_PEN_MIN, ACS_PEN_MAX);
				return 1;
			}
		}
	}
	else if (os_strcmp(buf, "acs_chan_cust_penalty") == 0) {
		char *cur_pos = pos;

		while (cur_pos) {
			int penalty, chan;

			if (sscanf(cur_pos, "%d:%d", &chan, &penalty)) {
				if (!IS_VALID_RANGE(chan, 0 , ACS_MAX_CHANNELS)) {
					wpa_printf(MSG_ERROR, "Line %d: invalid ACS channel penalty, channel out of range", line);
					return 1;
				}
				if (!IS_VALID_RANGE(penalty, ACS_PEN_MIN, ACS_PEN_MAX)) {
					wpa_printf(MSG_ERROR, "Line %d: invalid ACS channel penalty, penalty out of range", line);
					return 1;
				}
				conf->acs_chan_cust_penalty[chan] = penalty;
			}

			cur_pos = strchr(cur_pos, ',');
			if (cur_pos) cur_pos++;
		}
	}
	else if (os_strcmp(buf, "acs_to_degradation") == 0) {
		int i = 0;
		if (hostapd_parse_intlist(&conf->acs_to_degradation, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ACS timeout and degradation factors list", line);
			return 1;
		}
		/* it would be great to adopt a rule that "int-lists" contain their
		* element count in the member at index 0, then we wouldn't have to count...
		*/
		while (conf->acs_to_degradation[i] >= 0)
			i++;
		if (i != ACS_NUM_DEGRADATION_FACTORS) {
			wpa_printf(MSG_ERROR, "Line %d: ACS timeout and degradation factors list has %i elements instead of %i",
				   line, i, ACS_NUM_DEGRADATION_FACTORS);
			return 1;
		}
	}
	else if (os_strcmp(buf, "acs_policy") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid channel selection mode %d", line, val);
			return 1;
		}
		conf->acs_policy = val;
	}
	else if (os_strcmp(buf, "acs_switch_thresh") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, 0, ACS_SWITCH_THRESHOLD)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid channel switch threshold %d", line, val);
			return 1;
		}
		conf->acs_switch_thresh = val;
	}
	else if (os_strcmp(buf, "acs_grp_priorities_throughput") == 0) {
		int i = 0;
		if (hostapd_parse_intlist(&conf->acs_grp_priorities_throughput, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ACS group priorities list (throughput policy)", line);
			return 1;
		}
		for (i = 0; i < ACS_NUM_GRP_PRIORITIES; i++) {
			if (!IS_VALID_RANGE(conf->acs_grp_priorities_throughput[i], 0, ACS_NUM_GRP_PRIORITIES - 1)) {
				wpa_printf(MSG_ERROR, "Line %d: invalid ACS group priorities list entry (throughput policy) %i, priority > %i",
					   line, i, ACS_NUM_GRP_PRIORITIES);
				return 1;
			}
			conf->acs_grp_prio_tp_map[conf->acs_grp_priorities_throughput[i]] = i;
		}
	}
	else if (os_strcmp(buf, "acs_grp_priorities_reach") == 0) {
		int i = 0;
		if (hostapd_parse_intlist(&conf->acs_grp_priorities_reach, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ACS group priorities list (reach policy)", line);
			return 1;
		}
		for (i = 0; i < ACS_NUM_GRP_PRIORITIES; i++) {
			if (!IS_VALID_RANGE(conf->acs_grp_priorities_reach[i], 0, ACS_NUM_GRP_PRIORITIES - 1)) {
				wpa_printf(MSG_ERROR, "Line %d: invalid ACS group priorities list entry (reach policy) %i, priority > %i",
					   line, i, ACS_NUM_GRP_PRIORITIES);
				return 1;
			}
			conf->acs_grp_prio_reach_map[conf->acs_grp_priorities_reach[i]] = i;
		}
	}
	else if (os_strcmp(buf, "acs_bw_comparison") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid BW comparison value %d, range is 0..1", val, line);
			return 1;
		}
		conf->acs_bw_comparison = val;
	}
#endif /* CONFIG_ACS */
	else if (os_strcmp(buf, "chanlist") == 0) {
		if (hostapd_parse_chanlist(conf, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid channel list",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "dfs_debug_chan") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, DFS_DEBUG_CHAN_MIN, DFS_DEBUG_CHAN_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid dfs debug channel %d",
					 line, val);
			return 1;
		}
		conf->dfs_debug_chan = (u8)val;
	} else if (os_strcmp(buf, "dfs_channels_state_file_location") == 0) {
		conf->dfs_channels_state_file = os_strdup(pos);
	} else if (os_strcmp(buf, "freqlist") == 0) {
		if (freq_range_list_parse(&conf->acs_freq_list, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid frequency list",
				   line);
			return 1;
		}
		conf->acs_freq_list_present = 1;
	} else if (os_strcmp(buf, "beacon_int") == 0) {
		int val = atoi(pos);
		/* MIB defines range as 1..65535, but very small values
		 * cause problems with the current implementation.
		 * Since it is unlikely that this small numbers are
		 * useful in real life scenarios, do not allow beacon
		 * period to be set below 10 TU. */
		if (!IS_VALID_RANGE(val, BEACON_INTERVAL_MIN, BEACON_INTERVAL_MAX)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid beacon_int %d (expected 10..65535)",
				   line, val);
			return 1;
		}
		conf->beacon_int = val;
	} else if (os_strcmp(buf, "bss_beacon_int") == 0) {
		int val = atoi(pos);
		/* MIB defines range as 1..65535, but very small values
		 * cause problems with the current implementation.
		 * Since it is unlikely that this small numbers are
		 * useful in real life scenarios, do not allow beacon
		 * period to be set below 10 TU. */
		if (val != 0 && !IS_VALID_RANGE(val, BEACON_INTERVAL_MIN, BEACON_INTERVAL_MAX)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid bss_beacon_int %d (expected 0, 10..65535)",
				   line, val);
			return 1;
		}
		bss->beacon_int = (u16)val;
#ifdef CONFIG_ACS
	} else if (os_strcmp(buf, "acs_num_scans") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, ACS_MIN_NUM_SCANS, ACS_MAX_NUM_SCANS)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid acs_num_scans %d (expected 1..100)",
				   line, val);
			return 1;
		}
		conf->acs_num_scans = val;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "acs_chan_bias") == 0) {
		if (hostapd_config_parse_acs_chan_bias(conf, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid acs_chan_bias",
				   line);
			return -1;
		}
#endif /* CONFIG_ALLOW_HOSTAPD */
#endif /* CONFIG_ACS */
	} else if (os_strcmp(buf, "dtim_period") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, DTIM_PERIOD_MIN, DTIM_PERIOD_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid dtim_period %d",
				   line, val);
			return 1;
		}
		bss->dtim_period = val;
	} else if (os_strcmp(buf, "bss_load_update_period") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, BSS_LOAD_PERIOD_MIN, BSS_LOAD_PERIOD_MAX)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid bss_load_update_period %d",
				   line, val);
			return 1;
		}
		bss->bss_load_update_period = (u32)val;
	} else if (os_strcmp(buf, "chan_util_avg_period") == 0) {
		s64 val = atoll(pos);
		if (val < 0) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid chan_util_avg_period",
				   line);
			return 1;
		}
		bss->chan_util_avg_period = (u32)val;
	} else if (os_strcmp(buf, "rts_threshold") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, RTS_THRESHOLD_MIN, RTS_THRESHOLD_MAX)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid rts_threshold %d",
				   line, val);
			return 1;
		}
		conf->rts_threshold = val;
	} else if (os_strcmp(buf, "fragm_threshold") == 0) {
		int val = atoi(pos);
		if (val == -1) {
			/* allow a value of -1 */
		} else if (!IS_VALID_RANGE(val, FRAGM_THRESHOLD_MIN, FRAGM_THRESHOLD_MAX)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid fragm_threshold %d",
				   line, conf->fragm_threshold);
			return 1;
		}
		conf->fragm_threshold = val;
	} else if (os_strcmp(buf, "send_probe_response") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid send_probe_response %d (expected 0 or 1)",
				   line, val);
			return 1;
		}
		bss->send_probe_response = (u8)val;
	} else if (os_strcmp(buf, "supported_rates") == 0) {
		if (hostapd_parse_intlist(&conf->supported_rates, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rate list",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "basic_rates") == 0) {
		if (hostapd_parse_intlist(&conf->basic_rates, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rate list",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "beacon_rate") == 0) {
		int val;

		if (os_strncmp(pos, "ht:", 3) == 0) {
			val = atoi(pos + 3);
			if (!IS_VALID_RANGE(val, BEACON_RATE_HT_MCS_MIN, BEACON_RATE_HT_MCS_MAX)) {
				wpa_printf(MSG_ERROR,
					   "Line %d: invalid beacon_rate HT-MCS %d",
					   line, val);
				return 1;
			}
			conf->rate_type = BEACON_RATE_HT;
			conf->beacon_rate = (u32)val;
		} else if (os_strncmp(pos, "vht:", 4) == 0) {
			val = atoi(pos + 4);
			if (!IS_VALID_RANGE(val, BEACON_RATE_VHT_MCS_MIN, BEACON_RATE_VHT_MCS_MAX)) {
				wpa_printf(MSG_ERROR,
					   "Line %d: invalid beacon_rate VHT-MCS %d",
					   line, val);
				return 1;
			}
			conf->rate_type = BEACON_RATE_VHT;
			conf->beacon_rate = (u32)val;
		} else {
			val = atoi(pos);
			if (!IS_VALID_RANGE(val, BEACON_RATE_LEGACY_MCS_MIN, BEACON_RATE_LEGACY_MCS_MAX)) {
				wpa_printf(MSG_ERROR,
					   "Line %d: invalid legacy beacon_rate %d",
					   line, val);
				return 1;
			}
			conf->rate_type = BEACON_RATE_LEGACY;
			conf->beacon_rate = (u32)val;
		}
	} else if (os_strcmp(buf, "management_frames_rate") == 0) {
		int val = atoi(pos);
		if ((val != MGMT_FRAMES_RATE_DISABLED) &&
			!IS_VALID_RANGE(val, MGMT_FRAMES_RATE_ENABLED_MIN, MGMT_FRAMES_RATE_ENABLED_MAX))
		{
			wpa_printf(MSG_ERROR, "Line %d: invalid management_frames_rate %d (expected %d or [%d...%d])",
					   line, val,
					   MGMT_FRAMES_RATE_DISABLED,
					   MGMT_FRAMES_RATE_ENABLED_MIN,
					   MGMT_FRAMES_RATE_ENABLED_MAX);
			return 1;
		}
		bss->management_frames_rate = val;
	} else if (os_strcmp(buf, "dynamic_multicast_mode") == 0) {
		int val = atoi(pos);
		if (!((val >= DYNAMIC_MC_DISABLE) && (val <= DYNAMIC_MC_MODE_STATIC)))
		{
			wpa_printf(MSG_ERROR, "Line %d: invalid dynamic_multicast_mode : %d",line, val);
			return 1;
		}
		bss->dynamic_multicast_mode = val;
	} else if (os_strcmp(buf, "dynamic_multicast_rate") == 0) {
		int val = atoi(pos);
		if (!((val >= DYNAMIC_MC_RATE_MIN) && (val <= DYNAMIC_MC_RATE_MAX)))
		{
			wpa_printf(MSG_ERROR, "Line %d: invalid dynamic_multicast_rate : %d",line, val);
			return 1;
		}
		bss->dynamic_multicast_rate = val;
	} else if (os_strcmp(buf, "preamble") == 0) {
		if (atoi(pos))
			conf->preamble = SHORT_PREAMBLE;
		else
			conf->preamble = LONG_PREAMBLE;
	} else if (os_strcmp(buf, "ignore_broadcast_ssid") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, IGNORE_BROADCAST_SSID_DISABLED, IGNORE_BROADCAST_SSID_CLEAR_SSID)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ignore_broadcast_ssid value %d",
					line, val);
			return 1;
		}
		bss->ignore_broadcast_ssid = val;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "no_probe_resp_if_max_sta") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid no_probe_resp_if_max_sta %d",
				line, val);
			return 1;
		}
		bss->no_probe_resp_if_max_sta = val;
#endif /* CONFIG_ALLOW_HOSTAPD */
#ifdef CONFIG_WEP
	} else if (os_strcmp(buf, "wep_default_key") == 0) {
		int idx = atoi(pos);
		if (!IS_VALID_RANGE(idx, 0, NUM_WEP_KEYS - 1)) {
			wpa_printf(MSG_ERROR,
				   "Invalid wep_default_key index %d", idx);
			return 1;
		}
		bss->ssid.wep.idx = (u8)idx;
	} else if (os_strcmp(buf, "wep_key0") == 0 ||
		   os_strcmp(buf, "wep_key1") == 0 ||
		   os_strcmp(buf, "wep_key2") == 0 ||
		   os_strcmp(buf, "wep_key3") == 0) {
		if (hostapd_config_read_wep(&bss->ssid.wep,
					    buf[7] - '0', pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid WEP key '%s'",
				   line, buf);
			return 1;
		}
#endif /* CONFIG_WEP */
#ifndef CONFIG_NO_VLAN
	} else if (os_strcmp(buf, "dynamic_vlan") == 0) {
		int vlan;
		vlan = atoi(pos);
		if (!IS_VALID_RANGE(vlan, DYNAMIC_VLAN_DISABLED, DYNAMIC_VLAN_REQUIRED)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid dynamic_vlan value %d",
				line, vlan);
			return 1;
		}
		bss->ssid.dynamic_vlan = vlan;
	} else if (os_strcmp(buf, "per_sta_vif") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid per_sta_vif %d",
				line, val);
			return 1;
		}
		bss->ssid.per_sta_vif = val;
	} else if (os_strcmp(buf, "vlan_file") == 0) {
		if (hostapd_config_read_vlan_file(bss, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: failed to read VLAN file '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "vlan_naming") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, 0, DYNAMIC_VLAN_NAMING_END - 1)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid naming scheme %d",
				   line, bss->ssid.vlan_naming);
			return 1;
		}
		bss->ssid.vlan_naming = val;
#ifdef CONFIG_FULL_DYNAMIC_VLAN
	} else if (os_strcmp(buf, "vlan_tagged_interface") == 0) {
		os_free(bss->ssid.vlan_tagged_interface);
		bss->ssid.vlan_tagged_interface = os_strdup(pos);
#endif /* CONFIG_FULL_DYNAMIC_VLAN */
#endif /* CONFIG_NO_VLAN */
	} else if (os_strcmp(buf, "ap_table_max_size") == 0) {
		conf->ap_table_max_size = atoi(pos);
	} else if (os_strcmp(buf, "ap_table_expiration_time") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_TABLE_EXPIRATION_TIME_MIN, AP_TABLE_EXPIRATION_TIME_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ap_table_expiration_time %d",
				   line, val);
			return 1;
		}
		conf->ap_table_expiration_time = val;
	} else if (os_strncmp(buf, "tx_queue_", 9) == 0) {
		if (hostapd_config_tx_queue(conf->tx_queue, buf, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid TX queue item",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "wme_enabled") == 0 ||
		   os_strcmp(buf, "wmm_enabled") == 0) {
		bss->wmm_enabled = atoi(pos);
	} else if (os_strcmp(buf, "uapsd_advertisement_enabled") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid uapsd_advertisement_enabled %d",
				line, val);
			return 1;
		}
		bss->wmm_uapsd = val;
	} else if (os_strncmp(buf, "wme_ac_", 7) == 0 ||
		   os_strncmp(buf, "wmm_ac_", 7) == 0) {
		if (hostapd_config_wmm_ac(conf->wmm_ac_params, buf, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid WMM ac item",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "bss") == 0) {
		if (hostapd_config_bss(conf, pos)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid bss item",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "bssid") == 0) {
		if (hwaddr_aton(pos, bss->bssid)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid bssid item",
				   line);
			return 1;
		}
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "use_driver_iface_addr") == 0) {
		conf->use_driver_iface_addr = atoi(pos);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "mesh_mode") == 0) {
		mesh_mode_t mesh_mode = hostapd_config_parse_mesh_mode(pos);
		if (mesh_mode >= MESH_MODE_LAST) {
			wpa_printf(MSG_ERROR, "Line %d: invalid mesh_mode: %s", line, pos);
			return 1;
		}
		bss->mesh_mode = mesh_mode;
		bss->mesh_mode_set = 1;
	} else if (os_strcmp(buf, "ul_csa") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid owl %d",
				line, val);
			return 1;
		}
		bss->ul_csa = val;
	} else if (os_strcmp(buf, "bap_vifs_follow_bridge") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid bap_vifs_follow_bridge %d",
				line, val);
			return 1;
		}
		bss->bap_vifs_follow_bridge = val;
	}
	else if (os_strcmp(buf, "owl") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid owl %d",
				line, val);
			return 1;
		}
		conf->owl = val;
	}
	else if (os_strcmp(buf, "notify_action_frame") == 0) {
		conf->notify_action_frame = atoi(pos);
	} else if (os_strcmp(buf, "ScanRssiTh") == 0) {
		conf->ScanRssiTh = atoi(pos);
	} else if (os_strcmp(buf, "acs_scan_no_flush") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid acs_scan_no_flush %d",
				line, val);
			return 1;
		}
		conf->acs_scan_no_flush = val;
	} else if (os_strcmp(buf, "ieee80211w") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, NO_MGMT_FRAME_PROTECTION, MGMT_FRAME_PROTECTION_REQUIRED)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ieee80211w value %d",
				line, val);
			return 1;
		}
		bss->ieee80211w = val;
	} else if (os_strcmp(buf, "beacon_protection_enabled") == 0) {
		bss->beacon_protection_enabled = atoi(pos);
	} else if (os_strcmp(buf, "disable_bigtk_rekey") == 0) {
		bss->disable_bigtk_rekey = atoi(pos);
	} else if (os_strcmp(buf, "group_mgmt_cipher") == 0) {
		if (os_strcmp(pos, "AES-128-CMAC") == 0) {
			bss->group_mgmt_cipher = WPA_CIPHER_AES_128_CMAC;
		} else if (os_strcmp(pos, "BIP-GMAC-128") == 0) {
			bss->group_mgmt_cipher = WPA_CIPHER_BIP_GMAC_128;
		} else if (os_strcmp(pos, "BIP-GMAC-256") == 0) {
			bss->group_mgmt_cipher = WPA_CIPHER_BIP_GMAC_256;
		} else if (os_strcmp(pos, "BIP-CMAC-256") == 0) {
			bss->group_mgmt_cipher = WPA_CIPHER_BIP_CMAC_256;
		} else {
			wpa_printf(MSG_ERROR, "Line %d: invalid group_mgmt_cipher: %s",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "assoc_sa_query_max_timeout") == 0) {
		bss->assoc_sa_query_max_timeout = atoi(pos);
		if (bss->assoc_sa_query_max_timeout == 0) {
			wpa_printf(MSG_ERROR, "Line %d: invalid assoc_sa_query_max_timeout",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "assoc_sa_query_retry_timeout") == 0) {
		bss->assoc_sa_query_retry_timeout = atoi(pos);
		if (bss->assoc_sa_query_retry_timeout == 0) {
			wpa_printf(MSG_ERROR, "Line %d: invalid assoc_sa_query_retry_timeout",
				   line);
			return 1;
		}
#ifdef CONFIG_OCV
	} else if (os_strcmp(buf, "ocv") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ocv %d",
				line, val);
			return 1;
		}
		bss->ocv = val;
		if (bss->ocv && !bss->ieee80211w)
			bss->ieee80211w = 1;
#endif /* CONFIG_OCV */
	} else if (os_strcmp(buf, "ieee80211n") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ieee80211n %d",
				line, val);
			return 1;
		}
		conf->ieee80211n = val;
	} else if (os_strcmp(buf, "ht_capab") == 0) {
		if (hostapd_config_ht_capab(conf, pos) < 0) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ht_capab",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "secondary_channel") == 0) {
		conf->secondary_channel = atoi(pos);
		conf->secondary_channel_set = true;
	} else if (os_strcmp(buf, "ht_tx_bf_capab_from_hw") == 0) {
		conf->ht_tx_bf_capab_from_hw = atoi(pos);
	} else if (os_strcmp(buf, "ht_tx_bf_capab") == 0) {
		if (hostapd_config_ht_tx_bf_capab(conf, pos) < 0) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ht_tx_bf_capab",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "ht_rifs") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ht_rifs %d",
				line, val);
			return 1;
		}
		conf->ht_rifs = val;
	} else if (os_strcmp(buf, "require_ht") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid require_ht %d",
				line, val);
			return 1;
		}
		conf->require_ht = val;
	} else if (os_strcmp(buf, "obss_interval") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, OBSS_INTERVAL_MIN, OBSS_INTERVAL_MAX) && (val != 0)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid obss_interval value %u",
				line, val);
			return 1;
		}
		conf->obss_interval = val;
	} else if (os_strcmp(buf, "assoc_rsp_rx_mcs_mask") == 0) {
		conf->assoc_rsp_rx_mcs_mask = atoi(pos);
	} else if  (os_strcmp(buf, "scan_passive_dwell") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SCAN_PASSIVE_DWELL_MIN, SCAN_PASSIVE_DWELL_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid scan_passive_dwell value %u",
				line, val);
			return 1;
		}
		conf->scan_passive_dwell = (u16)val;
	} else if  (os_strcmp(buf, "scan_active_dwell") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SCAN_ACTIVE_DWELL_MIN, SCAN_ACTIVE_DWELL_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid scan_active_dwell value %u",
				line, val);
			return 1;
		}
		conf->scan_active_dwell = (u16)val;
	} else if  (os_strcmp(buf, "scan_passive_total_per_channel") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SCAN_PASSIVE_TOTAL_PER_CHANNEL_MIN, SCAN_PASSIVE_TOTAL_PER_CHANNEL_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid scan_active_total_per_channel value %u",
				line, val);
			return 1;
		}
		conf->scan_passive_total_per_channel = (u16)val;
	} else if  (os_strcmp(buf, "scan_active_total_per_channel") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SCAN_ACTIVE_TOTAL_PER_CHANNEL_MIN, SCAN_ACTIVE_TOTAL_PER_CHANNEL_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid scan_active_total_per_channel value %u",
				line, val);
			return 1;
		}
		conf->scan_active_total_per_channel = (u16)val;
	} else if  (os_strcmp(buf, "channel_transition_delay_factor") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, CHANNEL_TRANSITION_DELAY_FACTOR_MIN, CHANNEL_TRANSITION_DELAY_FACTOR_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid channel_transition_delay_factor value %u",
				line, val);
			return 1;
		}
		conf->channel_transition_delay_factor = (u16)val;
	} else if  (os_strcmp(buf, "scan_activity_threshold") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SCAN_ACTIVITY_THRESHOLD_MIN, SCAN_ACTIVITY_THRESHOLD_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid scan_activity_threshold value %u",
				line, val);
			return 1;			
		}
		conf->scan_activity_threshold = (u16)val;
	} else if  (os_strcmp(buf, "allow_scan_during_cac") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid allow_Scan_during_cac %d",
				line, val);
			return 1;
		}
		conf->allow_scan_during_cac = (u32)val;
	} else if  (os_strcmp(buf, "obss_beacon_rssi_threshold") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, OBSS_BEACON_RSSI_TH_MIN, OBSS_BEACON_RSSI_TH_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid obss_beacon_rssi_threshold",
				   line);
			return 1;
		}
		conf->obss_beacon_rssi_threshold = val;
	} else if (os_strcmp(buf, "ignore_40_mhz_intolerant") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ignore_40_mhz_intolerant %d",
				line, val);
			return 1;
		}
		conf->ignore_40_mhz_intolerant = val;
	} else if (os_strcmp(buf, "acs_scan_mode") == 0) {
		conf->acs_scan_mode = atoi(pos);
	} else if (os_strcmp(buf, "ht_maximum_ampdu_length_exponent") == 0) {
		conf->ht_cap_ampdu_parameters &= ~HT_CAP_AMPDU_MAXIMUM_AMPDU_LENGTH_EXPONENT;
		conf->ht_cap_ampdu_parameters |= (atoi(pos) << 0) & HT_CAP_AMPDU_MAXIMUM_AMPDU_LENGTH_EXPONENT;
		conf->ht_cap_ampdu_parameters_override_mask |= 0x1;
	} else if (os_strcmp(buf, "ht_minimum_mpdu_start_spacing") == 0) {
		conf->ht_cap_ampdu_parameters &= ~HT_CAP_AMPDU_MINIMUM_MPDU_START_SPACING;
		conf->ht_cap_ampdu_parameters |= (atoi(pos) << 2) & HT_CAP_AMPDU_MINIMUM_MPDU_START_SPACING;
		conf->ht_cap_ampdu_parameters_override_mask |= 0x2;
	} else if (os_strcmp(buf, "ht_mcs_set_part0") == 0) {
	  conf->ht_mcs_set[0] = host_to_le32(atoi(pos));
	} else if (os_strcmp(buf, "ht_mcs_set_part1") == 0) {
	  conf->ht_mcs_set[1] = host_to_le32(atoi(pos));
	} else if (os_strcmp(buf, "ht_mcs_set_part2") == 0) {
	  conf->ht_mcs_set[2] = host_to_le32(atoi(pos));
	} else if (os_strcmp(buf, "ht_mcs_set_part3") == 0) {
	  conf->ht_mcs_set[3] = host_to_le32(atoi(pos));
	} else if (os_strcmp(buf, "ieee80211n_acax_compat") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ieee80211n_acax_compat %d",
				line, val);
			return 1;
		}
		conf->ieee80211n_acax_compat = val;
#ifdef CONFIG_IEEE80211AC
	} else if (os_strcmp(buf, "ieee80211ac") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ieee80211ac %d",
				line, val);
			return 1;
		}
		conf->ieee80211ac = val;
		conf->orig_ieee80211ac = conf->ieee80211ac;
	} else if (os_strcmp(buf, "vht_capab") == 0) {
		if (hostapd_config_vht_capab(conf, pos) < 0) {
			wpa_printf(MSG_ERROR, "Line %d: invalid vht_capab",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "require_vht") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid require_vht %d",
				line, val);
			return 1;
		}
		conf->require_vht = val;
	} else if (os_strcmp(buf, "vht_oper_chwidth") == 0) {
		conf->vht_oper_chwidth = atoi(pos);
	} else if (os_strcmp(buf, "vht_oper_centr_freq_seg0_idx") == 0) {
		conf->vht_oper_centr_freq_seg0_idx = atoi(pos);
	} else if (os_strcmp(buf, "vht_oper_centr_freq_seg1_idx") == 0) {
		conf->vht_oper_centr_freq_seg1_idx = atoi(pos);
	} else if (os_strcmp(buf, "vendor_vht") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid vendor_vht %d",
				line, val);
			return 1;
		}
		bss->vendor_vht = val;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "use_sta_nsts") == 0) {
		bss->use_sta_nsts = atoi(pos);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "vht_mcs_set_part0") == 0) {
	  conf->vht_mcs_set[0] = host_to_le32(atoi(pos));
	} else if (os_strcmp(buf, "vht_mcs_set_part1") == 0) {
		conf->vht_mcs_set[1] = host_to_le32(atoi(pos));
#endif /* CONFIG_IEEE80211AC */
#ifdef CONFIG_IEEE80211AX
	} else if (os_strcmp(buf, "ieee80211ax") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ieee80211ax %d",
				line, val);
			return 1;
		}
		conf->ieee80211ax = val;
	} else if (os_strcmp(buf, "sDynamicMuTypeDownLink") == 0) {
		conf->sDynamicMuTypeDownLink = atoi(pos);
	} else if (os_strcmp(buf, "sDynamicMuTypeUpLink") == 0) {
		conf->sDynamicMuTypeUpLink = atoi(pos);
	} else if (os_strcmp(buf, "sDynamicMuMinStationsInGroup") == 0) {
		conf->sDynamicMuMinStationsInGroup = atoi(pos);
	} else if (os_strcmp(buf, "sDynamicMuMaxStationsInGroup") == 0) {
		conf->sDynamicMuMaxStationsInGroup  = atoi(pos);
	} else if (os_strcmp(buf, "sDynamicMuCdbConfig") == 0) {
		conf->sDynamicMuCdbConfig = atoi(pos);
	} else if (os_strcmp(buf, "sFixedLtfGi") == 0) {
		conf->sFixedLtfGi = atoi(pos);
	} else if (os_strcmp(buf, "he_spatial_reuse_ie_present_in_beacon") == 0) {
			conf->he_spatial_reuse_ie_present_in_beacon = atoi(pos);
	} else if (os_strcmp(buf, "he_spatial_reuse_ie_present_in_assoc_response") == 0) {
		conf->he_spatial_reuse_ie_present_in_assoc_response = atoi(pos);
	} else if (os_strcmp(buf, "he_spatial_reuse_ie_present_in_probe_response") == 0) {
		conf->he_spatial_reuse_ie_present_in_probe_response = atoi(pos);
	} else if (os_strcmp(buf, "he_mu_edca_ie_present") == 0) {
		conf->he_mu_edca_ie_present = atoi(pos);
	} else if (os_strcmp(buf, "require_he") == 0) {
		conf->require_he = atoi(pos);
	} else if (os_strcmp(buf, "he_phy_channel_width_set") == 0) {
		clr_set_he_cap(&conf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP0_IDX],
					   atoi(pos), HE_PHY_CAP0_CHANNEL_WIDTH_SET);
		if(conf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP0_IDX] &
				HE_PHY_CAP0_CHANNEL_WIDTH_SET_B0)
			conf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP8_IDX] |=
				HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_2_4_GHZ_BAND;
		if(conf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP0_IDX] &
				HE_PHY_CAP0_CHANNEL_WIDTH_SET_B2) {
			conf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP8_IDX] |=
				HE_PHY_CAP8_20MHZ_IN_160MHZ_HE_PPDU;
			conf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP8_IDX] |=
				HE_PHY_CAP8_80MHZ_IN_160MHZ_HE_PPDU;
			conf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP0_IDX] |=
				HE_PHY_CAP0_CHANNEL_WIDTH_SET_B1;
		}
	} else if (os_strcmp(buf, "he_phy_preamble_puncturing_rx") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid he_phy_preamble_puncturing_rx %d",
				line, val);
			return 1;
		}
		if (val) {
			val = HE_PHY_CAP1_PUN_PREAM_RX;
		}
		clr_set_he_cap(&conf->he_capab.he_phy_capab_info[HE_PHYCAP_CAP1_IDX],
						   val, HE_PHY_CAP1_PUN_PREAM_RX);
		conf->he_cap_non_adv_punc_pream_rx_support = val;
		conf->he_cap_punc_pream_rx_support_override = 1;

	} else if (os_strcmp(buf, "he_operation_default_pe_duration") == 0) {
		clr_set_he_cap(&conf->he_oper.he_oper_params[HE_OPERATION_CAP0_IDX],
					   atoi(pos), HE_OPERATION_CAP0_DEFAULT_PE_DURATION);
	} else if (os_strcmp(buf, "he_operation_twt_required") == 0) {
		clr_set_he_cap(&conf->he_oper.he_oper_params[HE_OPERATION_CAP0_IDX],
					   atoi(pos), HE_OPERATION_CAP0_TWT_REQUIRED);
	} else if (os_strcmp(buf, "he_operation_txop_duration_rts_threshold") == 0) {
		clr_set_he_cap(&conf->he_oper.he_oper_params[HE_OPERATION_CAP0_IDX],
					   atoi(pos), HE_OPERATION_CAP0_TXOP_DUR_RTS_TH);
		clr_set_he_cap(&conf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX],
					   (atoi(pos) >> 4), HE_OPERATION_CAP1_TXOP_DUR_RTS_TH);
	} else if (os_strcmp(buf, "he_operation_cohosted_bss") == 0) {
		clr_set_he_cap(&conf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX],
					   atoi(pos), HE_OPERATION_CAP1_CO_LOCATED_BSS);
	} else if (os_strcmp(buf, "he_operation_er_su_disable") == 0) {
		clr_set_he_cap(&conf->he_oper.he_oper_params[HE_OPERATION_CAP2_IDX],
					   atoi(pos), HE_OPERATION_CAP2_ER_SU_DISABLE);
	} else if (os_strcmp(buf, "he_operation_vht_operation_information_present") == 0) {
		clr_set_he_cap(&conf->he_oper.he_oper_params[HE_OPERATION_CAP1_IDX],
					   atoi(pos), HE_OPERATION_CAP1_VHT_OPER_INFO_PRESENT);
	} else if (os_strcmp(buf, "he_bss_color") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_RANGE(val, 1, HE_OPERATION_BSS_COLOR)) {
			wpa_printf(MSG_ERROR, "Invalid HE bss color value, must be in range 1..63");
			return 1;
		}
		conf->he_oper.bss_color_info |= set_he_cap(val, HE_OPERATION_BSS_COLOR);
	} else if (os_strcmp(buf, "he_operation_bss_color_disabled") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid he_operation_bss_color_disabled = %d",
				line, val);
			return 1;
		}
		conf->he_oper.bss_color_info |=
			set_he_cap(atoi(pos), HE_OPERATION_BSS_COLOR_DISABLED);
	} else if (os_strcmp(buf, "he_operation_max_mcs_for_1ss") == 0) {
		clr_set_he_cap(&conf->he_oper.he_mcs_nss_set[0],
					   atoi(pos), HE_MCS_NSS_FOR_1SS);
	} else if (os_strcmp(buf, "he_operation_max_mcs_for_2ss") == 0) {
		clr_set_he_cap(&conf->he_oper.he_mcs_nss_set[0],
					   atoi(pos), HE_MCS_NSS_FOR_2SS);
	} else if (os_strcmp(buf, "he_operation_max_mcs_for_3ss") == 0) {
		clr_set_he_cap(&conf->he_oper.he_mcs_nss_set[0],
					   atoi(pos), HE_MCS_NSS_FOR_3SS);
	} else if (os_strcmp(buf, "he_operation_max_mcs_for_4ss") == 0) {
		clr_set_he_cap(&conf->he_oper.he_mcs_nss_set[0],
					   atoi(pos), HE_MCS_NSS_FOR_4SS);
	} else if (os_strcmp(buf, "he_operation_max_mcs_for_5ss") == 0) {
		clr_set_he_cap(&conf->he_oper.he_mcs_nss_set[1],
					   atoi(pos), HE_MCS_NSS_FOR_5SS);
	} else if (os_strcmp(buf, "he_operation_max_mcs_for_6ss") == 0) {
		clr_set_he_cap(&conf->he_oper.he_mcs_nss_set[1],
					   atoi(pos), HE_MCS_NSS_FOR_6SS);
	} else if (os_strcmp(buf, "he_operation_max_mcs_for_7ss") == 0) {
		clr_set_he_cap(&conf->he_oper.he_mcs_nss_set[1],
					   atoi(pos), HE_MCS_NSS_FOR_7SS);
	} else if (os_strcmp(buf, "he_operation_max_mcs_for_8ss") == 0) {
		clr_set_he_cap(&conf->he_oper.he_mcs_nss_set[1],
					   atoi(pos), HE_MCS_NSS_FOR_8SS);
	} else if (os_strcmp(buf, "he_operation_vht_channel_width") == 0) {
		conf->he_oper.vht_op_info_chwidth = atoi(pos);
	} else if (os_strcmp(buf, "he_operation_vht_channel_center_segment0") == 0) {
		conf->he_oper.vht_op_info_chan_center_freq_seg0_idx = atoi(pos);
	} else if (os_strcmp(buf, "he_operation_vht_channel_center_segment1") == 0) {
		conf->he_oper.vht_op_info_chan_center_freq_seg1_idx = atoi(pos);
	} else if (os_strcmp(buf, "he_operation_max_cohosted_bssid_indicator") == 0) {
		conf->he_oper.max_co_located_bssid_ind = atoi(pos);
	} else if (os_strcmp(buf, "he_mu_edca_qos_info_parameter_set_update_count") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_qos_info,
					   atoi(pos), HE_QOS_INFO_EDCA_PARAM_SET_COUNT);
	} else if (os_strcmp(buf, "he_mu_edca_qos_info_q_ack") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_qos_info,
					   atoi(pos), HE_QOS_INFO_Q_ACK);
	} else if (os_strcmp(buf, "he_mu_edca_qos_info_queue_request") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_qos_info,
					   atoi(pos), HE_QOS_INFO_QUEUE_REQUEST);
	} else if (os_strcmp(buf, "he_mu_edca_qos_info_txop_request") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_qos_info,
					   atoi(pos), HE_QOS_INFO_TXOP_REQUEST);
	} else if (os_strcmp(buf, "he_mu_edca_ac_be_aifsn") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_be_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_AIFSN);
	} else if (os_strcmp(buf, "he_mu_edca_ac_be_acm") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_be_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ACM);
	} else if (os_strcmp(buf, "he_mu_edca_ac_be_aci") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_be_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ACI);
	} else if (os_strcmp(buf, "he_mu_edca_ac_be_ecwmin") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_be_param[HE_MU_AC_PARAM_ECWMIN_ECWMAX_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ECWMIN);
	} else if (os_strcmp(buf, "he_mu_edca_ac_be_ecwmax") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_be_param[HE_MU_AC_PARAM_ECWMIN_ECWMAX_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ECWMAX);
	} else if (os_strcmp(buf, "he_mu_edca_ac_be_timer") == 0) {
		conf->he_mu_edca.he_mu_ac_be_param[HE_MU_AC_PARAM_MU_EDCA_TIMER_IDX] =
			atoi(pos) & 0xff;
	} else if (os_strcmp(buf, "he_mu_edca_ac_bk_aifsn") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_AIFSN);
	} else if (os_strcmp(buf, "he_mu_edca_ac_bk_acm") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ACM);
	} else if (os_strcmp(buf, "he_mu_edca_ac_bk_aci") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ACI);
	} else if (os_strcmp(buf, "he_mu_edca_ac_bk_ecwmin") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_ECWMIN_ECWMAX_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ECWMIN);
	} else if (os_strcmp(buf, "he_mu_edca_ac_bk_ecwmax") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_ECWMIN_ECWMAX_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ECWMAX);
	} else if (os_strcmp(buf, "he_mu_edca_ac_bk_timer") == 0) {
		conf->he_mu_edca.he_mu_ac_bk_param[HE_MU_AC_PARAM_MU_EDCA_TIMER_IDX] =
			atoi(pos) & 0xff;
	} else if (os_strcmp(buf, "he_mu_edca_ac_vi_aifsn") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_AIFSN);
	} else if (os_strcmp(buf, "he_mu_edca_ac_vi_acm") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ACM);
	} else if (os_strcmp(buf, "he_mu_edca_ac_vi_aci") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ACI);
	} else if (os_strcmp(buf, "he_mu_edca_ac_vi_ecwmin") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_ECWMIN_ECWMAX_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ECWMIN);
	} else if (os_strcmp(buf, "he_mu_edca_ac_vi_ecwmax") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_ECWMIN_ECWMAX_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ECWMAX);
	} else if (os_strcmp(buf, "he_mu_edca_ac_vi_timer") == 0) {
		conf->he_mu_edca.he_mu_ac_vi_param[HE_MU_AC_PARAM_MU_EDCA_TIMER_IDX] =
			atoi(pos) & 0xff;
	} else if (os_strcmp(buf, "he_mu_edca_ac_vo_aifsn") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_AIFSN);
	} else if (os_strcmp(buf, "he_mu_edca_ac_vo_acm") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ACM);
	} else if (os_strcmp(buf, "he_mu_edca_ac_vo_aci") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_ACI_AIFSN_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ACI);
	} else if (os_strcmp(buf, "he_mu_edca_ac_vo_ecwmin") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_ECWMIN_ECWMAX_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ECWMIN);
	} else if (os_strcmp(buf, "he_mu_edca_ac_vo_ecwmax") == 0) {
		clr_set_he_cap(&conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_ECWMIN_ECWMAX_IDX],
					   atoi(pos), HE_MU_AC_PARAM_ECWMAX);
	} else if (os_strcmp(buf, "he_mu_edca_ac_vo_timer") == 0) {
		conf->he_mu_edca.he_mu_ac_vo_param[HE_MU_AC_PARAM_MU_EDCA_TIMER_IDX] =
			atoi(pos) & 0xff;
	} else if (os_strcmp(buf, "sr_control_field_srp_disallowed") == 0) {
		clr_set_he_cap(&conf->he_spatial_reuse.he_sr_control,
					   atoi(pos), HE_SRP_DISALLOWED);
	} else if (os_strcmp(buf, "sr_control_field_non_srg_obss_pd_sr_disallowed") == 0) {
		clr_set_he_cap(&conf->he_spatial_reuse.he_sr_control,
					   atoi(pos), HE_SRP_NON_SRG_OBSS_PD_SR_DIS);
	} else if (os_strcmp(buf, "sr_control_field_non_srg_offset_present") == 0) {
		clr_set_he_cap(&conf->he_spatial_reuse.he_sr_control,
					   atoi(pos), HE_SRP_NON_SRG_OFFSET_PRESENT);
	} else if (os_strcmp(buf, "sr_control_field_srg_information_present") == 0) {
		clr_set_he_cap(&conf->he_spatial_reuse.he_sr_control,
					   atoi(pos), HE_SRP_SRG_INFO_PRESENT);
	} else if (os_strcmp(buf, "sr_control_field_hesiga_spatial_reuse_value15_allowed") == 0) {
		clr_set_he_cap(&conf->he_spatial_reuse.he_sr_control,
					   atoi(pos), HE_SRP_HESIGA_SPATIAL_REUSE_VAL15_ALLOW);
	} else if (os_strcmp(buf, "non_srg_obss_pd_max_offset") == 0) {
		conf->he_spatial_reuse.he_non_srg_obss_pd_max_offset = atoi(pos);
	} else if (os_strcmp(buf, "srg_obss_pd_min_offset") == 0) {
		conf->he_spatial_reuse.he_srg_obss_pd_min_offset = atoi(pos);
	} else if (os_strcmp(buf, "srg_obss_pd_max_offset") == 0) {
		conf->he_spatial_reuse.he_srg_obss_pd_max_offset = atoi(pos);
	} else if (os_strcmp(buf, "srg_bss_color_bitmap_part1") == 0) {
		conf->he_spatial_reuse.he_srg_bss_color_bitmap[0] = atoi(pos);
	} else if (os_strcmp(buf, "srg_bss_color_bitmap_part2") == 0) {
		conf->he_spatial_reuse.he_srg_bss_color_bitmap[1] = atoi(pos);
	} else if (os_strcmp(buf, "srg_bss_color_bitmap_part3") == 0) {
		conf->he_spatial_reuse.he_srg_bss_color_bitmap[2] = atoi(pos);
	} else if (os_strcmp(buf, "srg_bss_color_bitmap_part4") == 0) {
		conf->he_spatial_reuse.he_srg_bss_color_bitmap[3] = atoi(pos);
	} else if (os_strcmp(buf, "srg_bss_color_bitmap_part5") == 0) {
		conf->he_spatial_reuse.he_srg_bss_color_bitmap[4] = atoi(pos);
	} else if (os_strcmp(buf, "srg_bss_color_bitmap_part6") == 0) {
		conf->he_spatial_reuse.he_srg_bss_color_bitmap[5] = atoi(pos);
	} else if (os_strcmp(buf, "srg_bss_color_bitmap_part7") == 0) {
		conf->he_spatial_reuse.he_srg_bss_color_bitmap[6] = atoi(pos);
	} else if (os_strcmp(buf, "srg_bss_color_bitmap_part8") == 0) {
		conf->he_spatial_reuse.he_srg_bss_color_bitmap[7] = atoi(pos);
	} else if (os_strcmp(buf, "srg_partial_bssid_bitmap_part1") == 0) {
		conf->he_spatial_reuse.he_srg_partial_bssid_bitmap[0] = atoi(pos);
	} else if (os_strcmp(buf, "srg_partial_bssid_bitmap_part2") == 0) {
		conf->he_spatial_reuse.he_srg_partial_bssid_bitmap[1] = atoi(pos);
	} else if (os_strcmp(buf, "srg_partial_bssid_bitmap_part3") == 0) {
		conf->he_spatial_reuse.he_srg_partial_bssid_bitmap[2] = atoi(pos);
	} else if (os_strcmp(buf, "srg_partial_bssid_bitmap_part4") == 0) {
		conf->he_spatial_reuse.he_srg_partial_bssid_bitmap[3] = atoi(pos);
	} else if (os_strcmp(buf, "srg_partial_bssid_bitmap_part5") == 0) {
		conf->he_spatial_reuse.he_srg_partial_bssid_bitmap[4] = atoi(pos);
	} else if (os_strcmp(buf, "srg_partial_bssid_bitmap_part6") == 0) {
		conf->he_spatial_reuse.he_srg_partial_bssid_bitmap[5] = atoi(pos);
	} else if (os_strcmp(buf, "srg_partial_bssid_bitmap_part7") == 0) {
		conf->he_spatial_reuse.he_srg_partial_bssid_bitmap[6] = atoi(pos);
	} else if (os_strcmp(buf, "srg_partial_bssid_bitmap_part8") == 0) {
		conf->he_spatial_reuse.he_srg_partial_bssid_bitmap[7] = atoi(pos);
	} else if (os_strcmp(buf, "he_nfr_buffer_threshold") == 0) {
		conf->he_nfr_buffer_threshold = atoi(pos);
	} else if (os_strcmp(buf, "multibss_enable") == 0) {
		conf->multibss_enable = atoi(pos);
	/* Section for override non-advertised HE caps */
	} else if (os_strcmp(buf, "he_cap_non_adv_multi_bss_rx_control_support") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid he_cap_non_adv_multi_bss_rx_control_support %d",
				line, val);
			return 1;
		}
		conf->he_cap_non_adv_multi_bss_rx_control_support = val;
		conf->he_cap_non_adv_multi_bss_rx_control_support_override = 1;
	/* HE debug */
	} else if (os_strcmp(buf, "enable_he_debug_mode") == 0) {
		conf->enable_he_debug_mode = atoi(pos);
	} else if (hostapd_conf_get_he_mac_capab_info(conf->he_capab.he_mac_capab_info,
						      buf, pos, &conf->override_hw_capab)) {
	} else if (hostapd_conf_get_he_phy_capab_info(conf->he_capab.he_phy_capab_info,
						      buf, pos, &conf->override_hw_capab)) {
	} else if (hostapd_conf_get_he_txrx_mcs_support(conf->he_capab.optional,
						        buf, pos, &conf->override_hw_capab)) {
	} else if (hostapd_conf_get_he_ppe_thresholds(conf->he_capab.optional + HE_PPE_THRES_INDEX,
						      buf, pos, &conf->override_hw_capab)) {
	} else if (os_strcmp(buf, "he_oper_chwidth") == 0) {
		conf->he_oper_chwidth = atoi(pos);
	} else if (os_strcmp(buf, "he_oper_centr_freq_seg0_idx") == 0) {
		conf->he_oper_centr_freq_seg0_idx = atoi(pos);
	} else if (os_strcmp(buf, "he_oper_centr_freq_seg1_idx") == 0) {
		conf->he_oper_centr_freq_seg1_idx = atoi(pos);
#endif /* CONFIG_IEEE80211AX */
	} else if (os_strcmp(buf, "ext_cap_max_num_msdus_in_amsdu") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, EXT_CAP_MAX_NUM_MSDU_IN_AMSDU_MIN, EXT_CAP_MAX_NUM_MSDU_IN_AMSDU_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ext_cap_max_num_msdus_in_amsdu %d (expected [%d...%d])",
					   line, val, EXT_CAP_MAX_NUM_MSDU_IN_AMSDU_MIN, EXT_CAP_MAX_NUM_MSDU_IN_AMSDU_MAX);
			return 1;
		}
		conf->ext_cap_max_num_msdus_in_amsdu = (u8)val;
	} else if (os_strcmp(buf, "twt_responder_support") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid twt_responder_support %d",
				line, val);
			return 1;
		}
		bss->twt_responder_support = val;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "max_listen_interval") == 0) {
		bss->max_listen_interval = atoi(pos);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "disable_pmksa_caching") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid disable_pmksa_caching %d",
				line, val);
			return 1;
		}
		bss->disable_pmksa_caching = val;
	} else if (os_strcmp(buf, "okc") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid okc %d",
				line, val);
			return 1;
		}
		bss->okc = val;
	} else if (os_strcmp(buf, "pmksa_interval") == 0) {
		bss->pmksa_interval = atoi(pos);
	} else if (os_strcmp(buf, "pmksa_life_time") == 0) {
		bss->pmksa_life_time = atoi(pos);
#ifdef CONFIG_DPP
	} else if (os_strcasecmp(buf, "dpp_init_max_tries") == 0) {
		bss->dpp_init_max_tries = atoi(pos);
	} else if (os_strcasecmp(buf, "dpp_init_retry_time") == 0) {
		bss->dpp_init_retry_time = atoi(pos);
	} else if (os_strcasecmp(buf, "dpp_resp_wait_time") == 0) {
		bss->dpp_resp_wait_time = atoi(pos);
	} else if (os_strcasecmp(buf, "dpp_resp_max_tries") == 0) {
		bss->dpp_resp_max_tries = atoi(pos);
	} else if (os_strcasecmp(buf, "dpp_resp_retry_time") == 0) {
		bss->dpp_resp_retry_time = atoi(pos);
	} else if (os_strcasecmp(buf, "dpp_gas_query_timeout_period") == 0) {
		bss->dpp_gas_query_timeout_period = atoi(pos);
#endif /* CONFIG_DPP */
#ifdef CONFIG_WPS
	} else if (os_strcmp(buf, "wps_state") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, WPS_STATE_MIN, WPS_STATE_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid wps_state",
				   line);
			return 1;
		}
		bss->wps_state = val;
	} else if (os_strcmp(buf, "wps_independent") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid wps_independent %d",
				line, val);
			return 1;
		}
		bss->wps_independent = val;
	} else if (os_strcmp(buf, "ap_setup_locked") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ap_setup_locked %d",
				line, val);
			return 1;
		}
		bss->ap_setup_locked = val;
	} else if (os_strcmp(buf, "uuid") == 0) {
		if (uuid_str2bin(pos, bss->uuid)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid UUID", line);
			return 1;
		}
	} else if (os_strcmp(buf, "wps_pin_requests") == 0) {
		os_free(bss->wps_pin_requests);
		bss->wps_pin_requests = os_strdup(pos);
	} else if (os_strcmp(buf, "device_name") == 0) {
		if (os_strlen(pos) > WPS_DEV_NAME_MAX_LEN) {
			wpa_printf(MSG_ERROR, "Line %d: Too long "
				   "device_name", line);
			return 1;
		}
		os_free(bss->device_name);
		bss->device_name = os_strdup(pos);
	} else if (os_strcmp(buf, "manufacturer") == 0) {
		if (os_strlen(pos) > 64) {
			wpa_printf(MSG_ERROR, "Line %d: Too long manufacturer",
				   line);
			return 1;
		}
		os_free(bss->manufacturer);
		bss->manufacturer = os_strdup(pos);
	} else if (os_strcmp(buf, "model_name") == 0) {
		if (os_strlen(pos) > 32) {
			wpa_printf(MSG_ERROR, "Line %d: Too long model_name",
				   line);
			return 1;
		}
		os_free(bss->model_name);
		bss->model_name = os_strdup(pos);
	} else if (os_strcmp(buf, "model_number") == 0) {
		if (os_strlen(pos) > 32) {
			wpa_printf(MSG_ERROR, "Line %d: Too long model_number",
				   line);
			return 1;
		}
		os_free(bss->model_number);
		bss->model_number = os_strdup(pos);
	} else if (os_strcmp(buf, "serial_number") == 0) {
		if (os_strlen(pos) > 32) {
			wpa_printf(MSG_ERROR, "Line %d: Too long serial_number",
				   line);
			return 1;
		}
		os_free(bss->serial_number);
		bss->serial_number = os_strdup(pos);
	} else if (os_strcmp(buf, "device_type") == 0) {
		if (wps_dev_type_str2bin(pos, bss->device_type))
			return 1;
	} else if (os_strcmp(buf, "config_methods") == 0) {
		os_free(bss->config_methods);
		bss->config_methods = os_strdup(pos);
	} else if (os_strcmp(buf, "os_version") == 0) {
		if (hexstr2bin(pos, bss->os_version, 4)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid os_version",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "ap_pin") == 0) {
		os_free(bss->ap_pin);
		if (*pos == '\0')
			bss->ap_pin = NULL;
		else
			bss->ap_pin = os_strdup(pos);
	} else if (os_strcmp(buf, "skip_cred_build") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid skip_cred_build %d",
				line, val);
			return 1;
		}
		bss->skip_cred_build = val;
	} else if (os_strcmp(buf, "extra_cred") == 0) {
		os_free(bss->extra_cred);
		bss->extra_cred = (u8 *) os_readfile(pos, &bss->extra_cred_len);
		if (bss->extra_cred == NULL) {
			wpa_printf(MSG_ERROR, "Line %d: could not read Credentials from '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "wps_cred_processing") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, WPS_CRED_PROCESS, WPS_CRED_PROCESS_AND_PASS)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid wps_cred_processing value %d",
				line, val);
			return 1;
		}
		bss->wps_cred_processing = val;
	} else if (os_strcmp(buf, "wps_cred_add_sae") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid wps_cred_add_sae %d",
				line, val);
			return 1;
		}
		bss->wps_cred_add_sae = val;
	} else if (os_strcmp(buf, "ap_settings") == 0) {
		os_free(bss->ap_settings);
		bss->ap_settings =
			(u8 *) os_readfile(pos, &bss->ap_settings_len);
		if (bss->ap_settings == NULL) {
			wpa_printf(MSG_ERROR, "Line %d: could not read AP Settings from '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "multi_ap_backhaul_ssid") == 0) {
		size_t slen;
		char *str = wpa_config_parse_string(pos, &slen);

		if (!str || slen < 1 || slen > SSID_MAX_LEN) {
			wpa_printf(MSG_ERROR, "Line %d: invalid SSID '%s'",
				   line, pos);
			os_free(str);
			return 1;
		}
		os_memcpy(bss->multi_ap_backhaul_ssid.ssid, str, slen);
		bss->multi_ap_backhaul_ssid.ssid_len = slen;
		bss->multi_ap_backhaul_ssid.ssid_set = 1;
		os_free(str);
	} else if (os_strcmp(buf, "multi_ap_backhaul_wpa_passphrase") == 0) {
		int len = os_strlen(pos);

		if (!IS_VALID_RANGE(len, WPA_PASSPHRASE_LEN_MIN, WPA_PASSPHRASE_LEN_MAX)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid WPA passphrase length %d (expected 8..63)",
				   line, len);
			return 1;
		}
		os_free(bss->multi_ap_backhaul_ssid.wpa_passphrase);
		bss->multi_ap_backhaul_ssid.wpa_passphrase = os_strdup(pos);
		if (bss->multi_ap_backhaul_ssid.wpa_passphrase) {
			hostapd_config_clear_wpa_psk(
				&bss->multi_ap_backhaul_ssid.wpa_psk);
			bss->multi_ap_backhaul_ssid.wpa_passphrase_set = 1;
		}
	} else if (os_strcmp(buf, "multi_ap_backhaul_wpa_psk") == 0) {
		hostapd_config_clear_wpa_psk(
			&bss->multi_ap_backhaul_ssid.wpa_psk);
		bss->multi_ap_backhaul_ssid.wpa_psk =
			os_zalloc(sizeof(struct hostapd_wpa_psk));
		if (!bss->multi_ap_backhaul_ssid.wpa_psk)
			return 1;
		if (hexstr2bin(pos, bss->multi_ap_backhaul_ssid.wpa_psk->psk,
			       PMK_LEN) ||
		    pos[PMK_LEN * 2] != '\0') {
			wpa_printf(MSG_ERROR, "Line %d: Invalid PSK '%s'.",
				   line, pos);
			hostapd_config_clear_wpa_psk(
				&bss->multi_ap_backhaul_ssid.wpa_psk);
			return 1;
		}
		bss->multi_ap_backhaul_ssid.wpa_psk->group = 1;
		os_free(bss->multi_ap_backhaul_ssid.wpa_passphrase);
		bss->multi_ap_backhaul_ssid.wpa_passphrase = NULL;
		bss->multi_ap_backhaul_ssid.wpa_psk_set = 1;
	} else if (os_strcmp(buf, "upnp_iface") == 0) {
		os_free(bss->upnp_iface);
		bss->upnp_iface = os_strdup(pos);
	} else if (os_strcmp(buf, "friendly_name") == 0) {
		rsize_t len = strnlen_s(pos, FRIENDLY_NAME_MAX_LEN);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, FRIENDLY_NAME_MAX_LEN - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid friendly_name len %zu",
				   line, len);
			return 1;
		}
		os_free(bss->friendly_name);
		bss->friendly_name = os_strdup(pos);
	} else if (os_strcmp(buf, "manufacturer_url") == 0) {
		rsize_t len = strnlen_s(pos, AP_MAX_VALID_CHAR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, AP_MAX_VALID_CHAR - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid manufacturer_url len %zu",
				   line, len);
			return 1;
		}
		os_free(bss->manufacturer_url);
		bss->manufacturer_url = os_strdup(pos);
	} else if (os_strcmp(buf, "model_description") == 0) {
		rsize_t len = strnlen_s(pos, MODEL_DESCRIPTION_MAX_LEN);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, MODEL_DESCRIPTION_MAX_LEN - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid model_description len %zu",
				   line, len);
			return 1;
		}
		os_free(bss->model_description);
		bss->model_description = os_strdup(pos);
	} else if (os_strcmp(buf, "model_url") == 0) {
		rsize_t len = strnlen_s(pos, AP_MAX_VALID_CHAR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, AP_MAX_VALID_CHAR - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid model_url len %zu",
				   line, len);
			return 1;
		}
		os_free(bss->model_url);
		bss->model_url = os_strdup(pos);
	} else if (os_strcmp(buf, "upc") == 0) {
		rsize_t len = strnlen_s(pos, UNIVERSAL_PRODUCT_CODE_MAX_DIGIT - 1);
		if (len != UNIVERSAL_PRODUCT_CODE_MAX_DIGIT) {
			wpa_printf(MSG_ERROR, "Line %d: invalid Universal Product Code (upc) len %zu",
				line, len);
			return 1;
		}
		os_free(bss->upc);
		bss->upc = os_strdup(pos);
	} else if (os_strcmp(buf, "pbc_in_m1") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid pbc_in_m1 %d",
				line, val);
			return 1;
		}
		bss->pbc_in_m1 = val;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "server_id") == 0) {
		os_free(bss->server_id);
		bss->server_id = os_strdup(pos);
	} else if (os_strcmp(buf, "wps_application_ext") == 0) {
		wpabuf_free(bss->wps_application_ext);
		bss->wps_application_ext = wpabuf_parse_bin(pos);
#ifdef CONFIG_WPS_NFC
	} else if (os_strcmp(buf, "wps_nfc_dev_pw_id") == 0) {
		bss->wps_nfc_dev_pw_id = atoi(pos);
		if (bss->wps_nfc_dev_pw_id < 0x10 ||
		    bss->wps_nfc_dev_pw_id > 0xffff) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid wps_nfc_dev_pw_id value",
				   line);
			return 1;
		}
		bss->wps_nfc_pw_from_config = 1;
	} else if (os_strcmp(buf, "wps_nfc_dh_pubkey") == 0) {
		wpabuf_free(bss->wps_nfc_dh_pubkey);
		bss->wps_nfc_dh_pubkey = wpabuf_parse_bin(pos);
		bss->wps_nfc_pw_from_config = 1;
	} else if (os_strcmp(buf, "wps_nfc_dh_privkey") == 0) {
		wpabuf_free(bss->wps_nfc_dh_privkey);
		bss->wps_nfc_dh_privkey = wpabuf_parse_bin(pos);
		bss->wps_nfc_pw_from_config = 1;
	} else if (os_strcmp(buf, "wps_nfc_dev_pw") == 0) {
		wpabuf_free(bss->wps_nfc_dev_pw);
		bss->wps_nfc_dev_pw = wpabuf_parse_bin(pos);
		bss->wps_nfc_pw_from_config = 1;
#endif /* CONFIG_WPS_NFC */
#endif /* CONFIG_ALLOW_HOSTAPD */
#endif /* CONFIG_WPS */
#ifdef CONFIG_P2P_MANAGER
	} else if (os_strcmp(buf, "manage_p2p") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid manage_p2p %d",
				line, val);
			return 1;
		}
		if (val)
			bss->p2p |= P2P_MANAGE;
		else
			bss->p2p &= ~P2P_MANAGE;
	} else if (os_strcmp(buf, "allow_cross_connection") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid allow_cross_connection %d",
				line, val);
			return 1;
		}
		if (val)
			bss->p2p |= P2P_ALLOW_CROSS_CONNECTION;
		else
			bss->p2p &= ~P2P_ALLOW_CROSS_CONNECTION;
#endif /* CONFIG_P2P_MANAGER */
	} else if (os_strcmp(buf, "disassoc_low_ack") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid disassoc_low_ack %d",
				line, val);
			return 1;
		}
		bss->disassoc_low_ack = val;
	} else if (os_strcmp(buf, "tdls_prohibit") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid tdls_prohibit %d",
				line, val);
			return 1;
		}
		if (val)
			bss->tdls |= TDLS_PROHIBIT;
		else
			bss->tdls &= ~TDLS_PROHIBIT;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "tdls_prohibit_chan_switch") == 0) {
		if (atoi(pos))
			bss->tdls |= TDLS_PROHIBIT_CHAN_SWITCH;
		else
			bss->tdls &= ~TDLS_PROHIBIT_CHAN_SWITCH;
#endif /* CONFIG_ALLOW_HOSTAPD */
#ifdef CONFIG_RSN_TESTING
	} else if (os_strcmp(buf, "rsn_testing") == 0) {
		extern int rsn_testing;
		rsn_testing = atoi(pos);
#endif /* CONFIG_RSN_TESTING */
	} else if (os_strcmp(buf, "time_advertisement") == 0) {
		int val = atoi(pos);
		if ((val != TIME_ADVERTISEMENT_DISABLED) && (val != TIME_ADVERTISEMENT_ENABLED)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid time_advertisement value %d",
				line, val);
			return 1;
		}
		bss->time_advertisement = val;
	} else if (os_strcmp(buf, "time_zone") == 0) {
		size_t tz_len = os_strlen(pos);
		if (!IS_VALID_RANGE(tz_len, TIME_ZONE_MIN, TIME_ZONE_MAX)) {
			wpa_printf(MSG_DEBUG, "Line %d: invalid time_zone",
				   line);
			return 1;
		}
		os_free(bss->time_zone);
		bss->time_zone = os_strdup(pos);
		if (bss->time_zone == NULL)
			return 1;
#ifdef CONFIG_WNM_AP
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "wnm_sleep_mode") == 0) {
		bss->wnm_sleep_mode = atoi(pos);
	} else if (os_strcmp(buf, "wnm_sleep_mode_no_keys") == 0) {
		bss->wnm_sleep_mode_no_keys = atoi(pos);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "bss_transition") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid bss_transition %d",
				line, val);
			return 1;
		}
		bss->bss_transition = val;
#endif /* CONFIG_WNM_AP */
#ifdef CONFIG_INTERWORKING
	} else if (os_strcmp(buf, "interworking") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid interworking %d",
				line, val);
			return 1;
		}
		bss->interworking = val;
	} else if (os_strcmp(buf, "access_network_type") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_RANGE(val, ACCESS_NETWORK_TYPE_MIN, ACCESS_NETWORK_TYPE_MAX)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid access_network_type",
				   line);
			return 1;
		}
		bss->access_network_type = val;
	} else if (os_strcmp(buf, "internet") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid internet %d",
				line, val);
			return 1;
		}
		bss->internet = val;
	} else if (os_strcmp(buf, "asra") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid asra %d",
				line, val);
			return 1;
		}
		bss->asra = val;
	} else if (os_strcmp(buf, "esr") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid esr %d",
				line, val);
			return 1;
		}
		bss->esr = val;
	} else if (os_strcmp(buf, "uesa") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid uesa %d",
				line, val);
			return 1;
		}
		bss->uesa = val;
	} else if (os_strcmp(buf, "venue_group") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_U8_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid venue_group %d",
				line, val);
			return 1;
		}
		bss->venue_group = (u8)val;
		bss->venue_info_set = 1;
	} else if (os_strcmp(buf, "venue_type") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_U8_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid venue_type %d",
				line, val);
			return 1;
		}
		bss->venue_type = (u8)val;
		bss->venue_info_set = 1;
	} else if (os_strcmp(buf, "hessid") == 0) {
		if (hwaddr_aton(pos, bss->hessid)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid hessid", line);
			return 1;
		}
	} else if (os_strcmp(buf, "roaming_consortium") == 0) {
		if (parse_roaming_consortium(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "roaming_anqp_ois_count") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_U8_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid roaming_anqp_ois_count %d",
				line, val);
			return 1;
		}
		bss->roaming_anqp_ois_count = val;
		bss->roaming_anqp_ois_count_set = 1;
	} else if (os_strcmp(buf, "venue_name") == 0) {
		if (parse_venue_name(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "venue_url") == 0) {
		if (parse_venue_url(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "network_auth_type") == 0) {
		u8 auth_type;
		u16 redirect_url_len;
		if (hexstr2bin(pos, &auth_type, 1)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid network_auth_type '%s'",
				   line, pos);
			return 1;
		}
		if (auth_type == 0 || auth_type == 2)
			redirect_url_len = os_strlen(pos + 2);
		else
			redirect_url_len = 0;
		os_free(bss->network_auth_type);
		bss->network_auth_type = os_malloc(redirect_url_len + 3 + 1);
		if (bss->network_auth_type == NULL)
			return 1;
		*bss->network_auth_type = auth_type;
		WPA_PUT_LE16(bss->network_auth_type + 1, redirect_url_len);
		if (redirect_url_len)
			os_memcpy(bss->network_auth_type + 3, pos + 2,
				  redirect_url_len);
		bss->network_auth_type_len = 3 + redirect_url_len;
	} else if (os_strcmp(buf, "ipaddr_type_availability") == 0) {
		if (hexstr2bin(pos, &bss->ipaddr_type_availability, 1)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid ipaddr_type_availability '%s'",
				   line, pos);
			bss->ipaddr_type_configured = 0;
			return 1;
		}
		bss->ipaddr_type_configured = 1;
	} else if (os_strcmp(buf, "domain_name") == 0) {
		int j, num_domains, domain_len, domain_list_len = 0;
		char *tok_start, *tok_prev;
		u8 *domain_list, *domain_ptr;

		domain_list_len = os_strlen(pos) + 1;
		domain_list = os_malloc(domain_list_len);
		if (domain_list == NULL)
			return 1;

		domain_ptr = domain_list;
		tok_prev = pos;
		num_domains = 1;
		while ((tok_prev = os_strchr(tok_prev, ','))) {
			num_domains++;
			tok_prev++;
		}
		tok_prev = pos;
		for (j = 0; j < num_domains; j++) {
			tok_start = os_strchr(tok_prev, ',');
			if (tok_start) {
				domain_len = tok_start - tok_prev;
				*domain_ptr = domain_len;
				os_memcpy(domain_ptr + 1, tok_prev, domain_len);
				domain_ptr += domain_len + 1;
				tok_prev = ++tok_start;
			} else {
				domain_len = os_strlen(tok_prev);
				*domain_ptr = domain_len;
				os_memcpy(domain_ptr + 1, tok_prev, domain_len);
				domain_ptr += domain_len + 1;
			}
		}

		os_free(bss->domain_name);
		bss->domain_name = domain_list;
		bss->domain_name_len = domain_list_len;
	} else if (os_strcmp(buf, "anqp_3gpp_cell_net") == 0) {
		if (parse_3gpp_cell_net(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "nai_realm") == 0) {
		if (parse_nai_realm(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "anqp_elem") == 0) {
		if (parse_anqp_elem(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "gas_frag_limit") == 0) {
		int val = atoi(pos);

		if (val <= 0) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid gas_frag_limit '%s'",
				   line, pos);
			return 1;
		}
		bss->gas_frag_limit = val;
	} else if (os_strcmp(buf, "gas_comeback_delay") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, GAS_COMEBACK_DELAY_MIN, GAS_COMEBACK_DELAY_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid gas_comeback_delay %d",
				   line, val);
			return 1;
		}
		bss->gas_comeback_delay = val;
	} else if (os_strcmp(buf, "qos_map_set") == 0) {
		if (parse_qos_map_set(bss, pos, line) < 0)
			return 1;
#endif /* CONFIG_INTERWORKING */
#ifdef CONFIG_RADIUS_TEST
	} else if (os_strcmp(buf, "dump_msk_file") == 0) {
		os_free(bss->dump_msk_file);
		bss->dump_msk_file = os_strdup(pos);
#endif /* CONFIG_RADIUS_TEST */
#ifdef CONFIG_PROXYARP
	} else if (os_strcmp(buf, "proxy_arp") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid proxy_arp %d",
				line, val);
			return 1;
		}
		bss->proxy_arp = val;
        } else if (os_strcmp(buf, "enable_snooping") == 0) {
                bss->enable_snooping = atoi(pos);
#endif /* CONFIG_PROXYARP */
#ifdef CONFIG_HS20
	} else if (os_strcmp(buf, "hs20") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid hs20 %d",
				line, val);
			return 1;
		}
		bss->hs20 = val;
	} else if (os_strcmp(buf, "hs20_release") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SUPPORTED_HS20_VERSION_MIN, SUPPORTED_HS20_VERSION_MAX)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Unsupported hs20_release: %s",
				   line, pos);
			return 1;
		}
		bss->hs20_release = val;
	} else if (os_strcmp(buf, "offload_gas") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid offload_gas value: %s",
				   line, pos);
			return 1;
		}
		bss->offload_gas = val;
	} else if (os_strcmp(buf, "disable_dgaf") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid disable_dgaf %d",
				line, val);
			return 1;
		}
		bss->disable_dgaf = val;
	} else if (os_strcmp(buf, "opmode_notif") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid opmode_notif %d",
				line, val);
			return 1;
		}
		bss->opmode_notif = val;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "na_mcast_to_ucast") == 0) {
		bss->na_mcast_to_ucast = atoi(pos);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "osen") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid osen %d",
				line, val);
			return 1;
		}
		bss->osen = val;
	} else if (os_strcmp(buf, "anqp_domain_id") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_U16_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid anqp_domain_id %d",
				line, val);
			return 1;
		}
		bss->anqp_domain_id = (u16)val;
	} else if (os_strcmp(buf, "hs20_deauth_req_timeout") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, HS20_DEAUTH_REQ_TIMEOUT_MIN, HS20_DEAUTH_REQ_TIMEOUT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid hs20_deauth_req_timeout %d",
				   line, val);
			return 1;
		}
		bss->hs20_deauth_req_timeout = val;
	} else if (os_strcmp(buf, "hs20_oper_friendly_name") == 0) {
		if (hs20_parse_oper_friendly_name(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "hs20_wan_metrics") == 0) {
		if (hs20_parse_wan_metrics(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "hs20_conn_capab") == 0) {
		if (hs20_parse_conn_capab(bss, pos, line) < 0) {
			return 1;
		}
	} else if (os_strcmp(buf, "hs20_operating_class") == 0) {
		u8 *oper_class;
		size_t oper_class_len;
		oper_class_len = os_strlen(pos);
		if (oper_class_len < 2 || (oper_class_len & 0x01)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid hs20_operating_class '%s'",
				   line, pos);
			return 1;
		}
		oper_class_len /= 2;
		oper_class = os_malloc(oper_class_len);
		if (oper_class == NULL)
			return 1;
		if (hexstr2bin(pos, oper_class, oper_class_len)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid hs20_operating_class '%s'",
				   line, pos);
			os_free(oper_class);
			return 1;
		}
		os_free(bss->hs20_operating_class);
		bss->hs20_operating_class = oper_class;
		bss->hs20_operating_class_len = oper_class_len;
	} else if (os_strcmp(buf, "hs20_icon") == 0) {
		if (hs20_parse_icon(bss, pos) < 0) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid hs20_icon '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "osu_ssid") == 0) {
		if (hs20_parse_osu_ssid(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "osu_server_uri") == 0) {
		rsize_t len = strnlen_s(pos, AP_MAX_VALID_CHAR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, AP_MAX_VALID_CHAR - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid osu_server_uri len %zu",
				   line, len);
			return 1;
		}
		if (hs20_parse_osu_server_uri(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "osu_friendly_name") == 0) {
		if (hs20_parse_osu_friendly_name(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "osu_nai") == 0) {
		rsize_t len = strnlen_s(pos, AP_MAX_VALID_CHAR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, AP_MAX_VALID_CHAR - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid osu_nai len %zu",
				   line, len);
			return 1;
		}
		if (hs20_parse_osu_nai(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "osu_nai2") == 0) {
		rsize_t len = strnlen_s(pos, AP_MAX_VALID_CHAR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, AP_MAX_VALID_CHAR - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid osu_nai2 len %zu",
				   line, len);
			return 1;
		}
		if (hs20_parse_osu_nai2(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "osu_method_list") == 0) {
		rsize_t len = strnlen_s(pos, AP_MAX_VALID_CHAR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, AP_MAX_VALID_CHAR - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid osu_method_list len %zu",
				   line, len);
			return 1;
		}
		if (hs20_parse_osu_method_list(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "osu_icon") == 0) {
		rsize_t len = strnlen_s(pos, AP_MAX_VALID_CHAR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, AP_MAX_VALID_CHAR - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid osu_icon len %zu",
				   line, len);
			return 1;
		}
		if (hs20_parse_osu_icon(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "osu_service_desc") == 0) {
		if (hs20_parse_osu_service_desc(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "operator_icon") == 0) {
		rsize_t len = strnlen_s(pos, AP_MAX_VALID_CHAR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, AP_MAX_VALID_CHAR - 1)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid friendly_name len %zu",
				   line, len);
			return 1;
		}
		if (hs20_parse_operator_icon(bss, pos, line) < 0)
			return 1;
	} else if (os_strcmp(buf, "subscr_remediation_url") == 0) {
		os_free(bss->subscr_remediation_url);
		bss->subscr_remediation_url = os_strdup(pos);
	} else if (os_strcmp(buf, "subscr_remediation_method") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SUBSCR_REMEDIATION_METHOD_MIN, SUBSCR_REMEDIATION_METHOD_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid subscr_remediation_method %d",
				   line, val);
			return 1;
		}
		bss->subscr_remediation_method = val;
	} else if (os_strcmp(buf, "hs20_t_c_filename") == 0) {
		os_free(bss->t_c_filename);
		bss->t_c_filename = os_strdup(pos);
	} else if (os_strcmp(buf, "hs20_t_c_timestamp") == 0) {
		bss->t_c_timestamp = strtol(pos, NULL, 0);
	} else if (os_strcmp(buf, "hs20_t_c_server_url") == 0) {
		os_free(bss->t_c_server_url);
		bss->t_c_server_url = os_strdup(pos);
	} else if (os_strcmp(buf, "hs20_sim_provisioning_url") == 0) {
		os_free(bss->hs20_sim_provisioning_url);
		bss->hs20_sim_provisioning_url = os_strdup(pos);
#endif /* CONFIG_HS20 */
#ifdef CONFIG_MBO
	} else if (os_strcmp(buf, "mbo") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid mbo %d",
				line, val);
			return 1;
		}
		bss->mbo_enabled = val;
	} else if (os_strcmp(buf, "mbo_cell_data_conn_pref") == 0) {
		int val = atoi(pos);
		if ((val != AP_STA_NO_CELLULAR_DATA_CONN) && (val != AP_STA_PREFERS_NO_CELLULAR_DATA_CONN) &&
			(val != AP_STA_PREFERS_CELLULAR_DATA_CONN)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid mbo_cell_data_conn_pref value %d",
				line, val);
			return 1;
		}
		bss->mbo_cell_data_conn_pref = val;
	} else if (os_strcmp(buf, "mbo_cell_aware") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid mbo_cell_aware %d",
				line, val);
			return 1;
		}
		bss->mbo_cell_aware = val;
	} else if (os_strcmp(buf, "mbo_pmf_bypass") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid mbo_pmf_bypass %d",
				line, val);
			return 1;
		}
		bss->mbo_pmf_bypass = val;
	} else if (os_strcmp(buf, "oce") == 0) {
		int val;
		val = atoi(pos);
		if (val > (OCE_STA | OCE_STA_CFON | OCE_AP)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid oce value %d",
				line, val);
			return 1;
		}
		bss->oce = val;
#endif /* CONFIG_MBO */
#ifdef CONFIG_TESTING_OPTIONS
#define PARSE_TEST_PROBABILITY(_val)				\
	} else if (os_strcmp(buf, #_val) == 0) {		\
		char *end;					\
								\
		conf->_val = strtod(pos, &end);			\
		if (*end || conf->_val < 0.0 ||			\
		    conf->_val > 1.0) {				\
			wpa_printf(MSG_ERROR,			\
				   "Line %d: Invalid value '%s'", \
				   line, pos);			\
			return 1;				\
		}
#ifdef CONFIG_ALLOW_HOSTAPD
	PARSE_TEST_PROBABILITY(ignore_probe_probability)
	PARSE_TEST_PROBABILITY(ignore_auth_probability)
	PARSE_TEST_PROBABILITY(ignore_assoc_probability)
	PARSE_TEST_PROBABILITY(ignore_reassoc_probability)
	PARSE_TEST_PROBABILITY(corrupt_gtk_rekey_mic_probability)
	} else if (os_strcmp(buf, "ecsa_ie_only") == 0) {
		conf->ecsa_ie_only = atoi(pos);
	} else if (os_strcmp(buf, "bss_load_test") == 0) {
		WPA_PUT_LE16(bss->bss_load_test, atoi(pos));
		pos = os_strchr(pos, ':');
		if (pos == NULL) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid bss_load_test",
				   line);
			return 1;
		}
		pos++;
		bss->bss_load_test[2] = atoi(pos);
		pos = os_strchr(pos, ':');
		if (pos == NULL) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid bss_load_test",
				   line);
			return 1;
		}
		pos++;
		WPA_PUT_LE16(&bss->bss_load_test[3], atoi(pos));
		bss->bss_load_test_set = 1;
	} else if (os_strcmp(buf, "radio_measurements") == 0) {
		/*
		 * DEPRECATED: This parameter will be removed in the future.
		 * Use rrm_neighbor_report instead.
		 */
		int val = atoi(pos);

		if (val & BIT(0))
			bss->radio_measurements[0] |=
				WLAN_RRM_CAPS_NEIGHBOR_REPORT;
	} else if (os_strcmp(buf, "own_ie_override") == 0) {
		struct wpabuf *tmp;
		size_t len = os_strlen(pos) / 2;

		tmp = wpabuf_alloc(len);
		if (!tmp)
			return 1;

		if (hexstr2bin(pos, wpabuf_put(tmp, len), len)) {
			wpabuf_free(tmp);
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid own_ie_override '%s'",
				   line, pos);
			return 1;
		}

		wpabuf_free(bss->own_ie_override);
		bss->own_ie_override = tmp;
	} else if (os_strcmp(buf, "sae_reflection_attack") == 0) {
		bss->sae_reflection_attack = atoi(pos);
	} else if (os_strcmp(buf, "sae_commit_status") == 0) {
		bss->sae_commit_status = atoi(pos);
	} else if (os_strcmp(buf, "sae_pk_omit") == 0) {
		bss->sae_pk_omit = atoi(pos);
	} else if (os_strcmp(buf, "sae_commit_override") == 0) {
		wpabuf_free(bss->sae_commit_override);
		bss->sae_commit_override = wpabuf_parse_bin(pos);
	} else if (os_strcmp(buf, "rsnxe_override_eapol") == 0) {
		wpabuf_free(bss->rsnxe_override_eapol);
		bss->rsnxe_override_eapol = wpabuf_parse_bin(pos);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "rsne_override_ft") == 0) {
		wpabuf_free(bss->rsne_override_ft);
		bss->rsne_override_ft = wpabuf_parse_bin(pos);
	} else if (os_strcmp(buf, "rsnxe_override_ft") == 0) {
		wpabuf_free(bss->rsnxe_override_ft);
		bss->rsnxe_override_ft = wpabuf_parse_bin(pos);
	} else if (os_strcmp(buf, "gtk_rsc_override") == 0) {
		wpabuf_free(bss->gtk_rsc_override);
		bss->gtk_rsc_override = wpabuf_parse_bin(pos);
	} else if (os_strcmp(buf, "igtk_rsc_override") == 0) {
		wpabuf_free(bss->igtk_rsc_override);
		bss->igtk_rsc_override = wpabuf_parse_bin(pos);
	} else if (os_strcmp(buf, "no_beacon_rsnxe") == 0) {
		bss->no_beacon_rsnxe = atoi(pos);
	} else if (os_strcmp(buf, "skip_prune_assoc") == 0) {
		bss->skip_prune_assoc = atoi(pos);
	} else if (os_strcmp(buf, "ft_rsnxe_used") == 0) {
		bss->ft_rsnxe_used = atoi(pos);
	} else if (os_strcmp(buf, "oci_freq_override_eapol_m3") == 0) {
		bss->oci_freq_override_eapol_m3 = atoi(pos);
	} else if (os_strcmp(buf, "oci_freq_override_eapol_g1") == 0) {
		bss->oci_freq_override_eapol_g1 = atoi(pos);
	} else if (os_strcmp(buf, "oci_freq_override_saquery_req") == 0) {
		bss->oci_freq_override_saquery_req = atoi(pos);
	} else if (os_strcmp(buf, "oci_freq_override_saquery_resp") == 0) {
		bss->oci_freq_override_saquery_resp = atoi(pos);
	} else if (os_strcmp(buf, "oci_freq_override_ft_assoc") == 0) {
		bss->oci_freq_override_ft_assoc = atoi(pos);
	} else if (os_strcmp(buf, "oci_freq_override_fils_assoc") == 0) {
		bss->oci_freq_override_fils_assoc = atoi(pos);
	} else if (os_strcmp(buf, "oci_freq_override_wnm_sleep") == 0) {
		bss->oci_freq_override_wnm_sleep = atoi(pos);
#endif /* CONFIG_TESTING_OPTIONS */
#ifdef CONFIG_SAE
	} else if (os_strcmp(buf, "sae_password") == 0) {
		if (parse_sae_password(bss, pos) < 0) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid sae_password",
				   line);
			return 1;
		}
#endif /* CONFIG_SAE */
	} else if (os_strcmp(buf, "vendor_elements") == 0) {
		if (parse_wpabuf_hex(line, buf, &bss->vendor_elements, pos))
			return 1;
	} else if (os_strcmp(buf, "authresp_elements") == 0) {
		if (parse_wpabuf_hex(line, buf, &bss->authresp_elements, pos))
			return 1;
	} else if (os_strcmp(buf, "assocresp_elements") == 0) {
		if (parse_wpabuf_hex(line, buf, &bss->assocresp_elements, pos))
			return 1;
	} else if (os_strcmp(buf, "sae_anti_clogging_threshold") == 0) {
		bss->sae_anti_clogging_threshold = strtoul(pos, NULL, 10);
	} else if (os_strcmp(buf, "sae_sync") == 0) {
		bss->sae_sync = atoi(pos);
	} else if (os_strcmp(buf, "sae_groups") == 0) {
		if (hostapd_parse_intlist(&bss->sae_groups, pos)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid sae_groups value '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "sae_require_mfp") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid sae_require_mfp %d",
				line, val);
			return 1;
		}
		bss->sae_require_mfp = val;
	} else if (os_strcmp(buf, "sae_confirm_immediate") == 0) {
		bss->sae_confirm_immediate = atoi(pos);
	} else if (os_strcmp(buf, "sae_pwe") == 0) {
		bss->sae_pwe = atoi(pos);
	} else if (os_strcmp(buf, "local_pwr_constraint") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, LOCAL_PWR_CONSTRAINT_MIN, LOCAL_PWR_CONSTRAINT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid local_pwr_constraint %d (expected 0..255)",
				   line, val);
			return 1;
		}
		conf->local_pwr_constraint = val;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "spectrum_mgmt_required") == 0) {
		conf->spectrum_mgmt_required = atoi(pos);
	} else if (os_strcmp(buf, "wowlan_triggers") == 0) {
		os_free(bss->wowlan_triggers);
		bss->wowlan_triggers = os_strdup(pos);
#ifdef CONFIG_FST
	} else if (os_strcmp(buf, "fst_group_id") == 0) {
		size_t len = os_strlen(pos);

		if (!len || len >= sizeof(conf->fst_cfg.group_id)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid fst_group_id value '%s'",
				   line, pos);
			return 1;
		}

		if (conf->fst_cfg.group_id[0]) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Duplicate fst_group value '%s'",
				   line, pos);
			return 1;
		}

		os_strlcpy(conf->fst_cfg.group_id, pos,
			   sizeof(conf->fst_cfg.group_id));
	} else if (os_strcmp(buf, "fst_priority") == 0) {
		char *endp;
		long int val;

		if (!*pos) {
			wpa_printf(MSG_ERROR,
				   "Line %d: fst_priority value not supplied (expected 1..%u)",
				   line, FST_MAX_PRIO_VALUE);
			return -1;
		}

		val = strtol(pos, &endp, 0);
		if (*endp || val < 1 || val > FST_MAX_PRIO_VALUE) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid fst_priority %ld (%s) (expected 1..%u)",
				   line, val, pos, FST_MAX_PRIO_VALUE);
			return 1;
		}
		conf->fst_cfg.priority = (u8) val;
	} else if (os_strcmp(buf, "fst_llt") == 0) {
		char *endp;
		long int val;

		if (!*pos) {
			wpa_printf(MSG_ERROR,
				   "Line %d: fst_llt value not supplied (expected 1..%u)",
				   line, FST_MAX_LLT_MS);
			return -1;
		}
		val = strtol(pos, &endp, 0);
		if (*endp || val < 1 ||
		    (unsigned long int) val > FST_MAX_LLT_MS) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid fst_llt %ld (%s) (expected 1..%u)",
				   line, val, pos, FST_MAX_LLT_MS);
			return 1;
		}
		conf->fst_cfg.llt = (u32) val;
#endif /* CONFIG_FST */
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "track_sta_max_num") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, TRACK_STA_MAX_NUM_MIN, TRACK_STA_MAX_NUM_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid track_sta_max_num %d",
				   line, val);
			return 1;
		}
		conf->track_sta_max_num = (u32)val;
	} else if (os_strcmp(buf, "track_sta_max_age") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, TRACK_STA_MAX_AGE_MIN, TRACK_STA_MAX_AGE_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid track_sta_max_age %d",
				   line, val);
			return 1;
		}
		conf->track_sta_max_age = (u32)val;
	} else if (os_strcmp(buf, "no_probe_resp_if_seen_on") == 0) {
		os_free(bss->no_probe_resp_if_seen_on);
		bss->no_probe_resp_if_seen_on = os_strdup(pos);
	} else if (os_strcmp(buf, "no_auth_if_seen_on") == 0) {
		os_free(bss->no_auth_if_seen_on);
		bss->no_auth_if_seen_on = os_strdup(pos);
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "lci") == 0) {
		wpabuf_free(conf->lci);
		conf->lci = wpabuf_parse_bin(pos);
		if (conf->lci && wpabuf_len(conf->lci) == 0) {
			wpabuf_free(conf->lci);
			conf->lci = NULL;
		}
	} else if (os_strcmp(buf, "civic") == 0) {
		wpabuf_free(conf->civic);
		conf->civic = wpabuf_parse_bin(pos);
		if (conf->civic && wpabuf_len(conf->civic) == 0) {
			wpabuf_free(conf->civic);
			conf->civic = NULL;
		}
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "rrm_neighbor_report") == 0) {
		if (atoi(pos))
			bss->radio_measurements[0] |=
				WLAN_RRM_CAPS_NEIGHBOR_REPORT;
	} else if (os_strcmp(buf, "rrm_beacon_report_passive") == 0) {
		if (atoi(pos))
			bss->radio_measurements[0] |= WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE;
	} else if (os_strcmp(buf, "rrm_beacon_report_active") == 0) {
		if (atoi(pos))
			bss->radio_measurements[0] |= WLAN_RRM_CAPS_BEACON_REPORT_ACTIVE;
	} else if (os_strcmp(buf, "rrm_beacon_report_table") == 0) {
		if (atoi(pos))
			bss->radio_measurements[0] |= WLAN_RRM_CAPS_BEACON_REPORT_TABLE;
	} else if (os_strcmp(buf, "rrm_manipulate_measurement") == 0) {
		bss->rrm_manipulate_measurement = atoi(pos);
	} else if (os_strcmp(buf, "rrm_link_measurement") == 0) {
		if (atoi(pos))
			bss->radio_measurements[0] |= WLAN_RRM_CAPS_LINK_MEASUREMENT;
	} else if (os_strcmp(buf, "rrm_sta_statistics") == 0) {
		if (atoi(pos))
			bss->radio_measurements[1] |= WLAN_RRM_CAPS_STATISTICS_MEASUREMENT;
	} else if (os_strcmp(buf, "rrm_channel_load") == 0) {
		if (atoi(pos))
			bss->radio_measurements[1] |= WLAN_RRM_CAPS_CHANNEL_LOAD;
	} else if (os_strcmp(buf, "rrm_noise_histogram") == 0) {
		if (atoi(pos))
			bss->radio_measurements[1] |= WLAN_RRM_CAPS_NOISE_HISTOGRAM;
	} else if (os_strcmp(buf, "wnm_bss_trans_query_auto_resp") == 0) {
		bss->wnm_bss_trans_query_auto_resp = atoi(pos);
	} else if (os_strcmp(buf, "rrm_beacon_report") == 0) {
		if (atoi(pos))
			bss->radio_measurements[0] |=
				WLAN_RRM_CAPS_BEACON_REPORT_PASSIVE |
				WLAN_RRM_CAPS_BEACON_REPORT_ACTIVE |
				WLAN_RRM_CAPS_BEACON_REPORT_TABLE;

#ifdef CONFIG_WDS_WPA
	} else if (os_strcmp(buf, "wds_wpa_sta_file") == 0) {
		if (hostapd_config_read_maclist(pos, &bss->wds_wpa_sta,
						&bss->num_wds_wpa_sta)) {
			wpa_printf(MSG_ERROR, "Line %d: Failed to read wds_wpa_sta_file '%s'",
					line, pos);
			return 1;
		}
#endif
	} else if (os_strcmp(buf, "gas_address3") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, GAS_ADDRESS3_MIN, GAS_ADDRESS3_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid gas_address3 value %d",
				line, val);
			return 1;
		}
		bss->gas_address3 = val;
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "stationary_ap") == 0) {
		conf->stationary_ap = atoi(pos);
	} else if (os_strcmp(buf, "ftm_responder") == 0) {
		bss->ftm_responder = atoi(pos);
	} else if (os_strcmp(buf, "ftm_initiator") == 0) {
		bss->ftm_initiator = atoi(pos);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "assoc_rsp_rx_mcs_mask") == 0) {
		conf->assoc_rsp_rx_mcs_mask = atoi(pos);
	} else if (os_strcmp(buf, "atf_config_file") == 0) {
		os_free(conf->atf_config_file);
		conf->atf_config_file = os_strdup(pos);
		if (conf->atf_config_file != NULL && conf->atf_config_file[0] != '\0')
			hostapd_atf_read_config(&conf->atf_cfg, conf->atf_config_file);
		else
			hostapd_atf_clean_config(&conf->atf_cfg);
	} else if (os_strcmp(buf, "enable_bss_load_ie") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid enable_bss_load_ie %d",
					line, val);
			return 1;
		}
		bss->enable_bss_load_ie = val;
#ifdef CONFIG_FILS
	} else if (os_strcmp(buf, "fils_cache_id") == 0) {
		if (hexstr2bin(pos, bss->fils_cache_id, FILS_CACHE_ID_LEN)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid fils_cache_id '%s'",
				   line, pos);
			return 1;
		}
		bss->fils_cache_id_set = 1;
	} else if (os_strcmp(buf, "fils_realm") == 0) {
		if (parse_fils_realm(bss, pos) < 0)
			return 1;
	} else if (os_strcmp(buf, "fils_dh_group") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, FILS_DH_GROUP_MIN, FILS_DH_GROUP_MAX)) {
		{
			wpa_printf(MSG_ERROR, "Line %d: invalid fils_dh_group value %d",
				line, val);
			return 1;
		}
		bss->fils_dh_group = val;
	} else if (os_strcmp(buf, "dhcp_server") == 0) {
		if (hostapd_parse_ip_addr(pos, &bss->dhcp_server)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid IP address '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "dhcp_rapid_commit_proxy") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid dhcp_rapid_commit_proxy %d",
				line, val);
			return 1;
		}
		bss->dhcp_rapid_commit_proxy = val;
	} else if (os_strcmp(buf, "fils_hlp_wait_time") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, FILS_HLP_WAIT_TIME_MIN, FILS_HLP_WAIT_TIME_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid fils_hlp_wait_time %d",
				   line, pos);
			return 1;
		}
		bss->fils_hlp_wait_time = val;
	} else if (os_strcmp(buf, "dhcp_server_port") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_PORT_MIN, AP_PORT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid dhcp_server_port %d",
				line, val);
			return 1;
		}
		bss->dhcp_server_port = val;
	} else if (os_strcmp(buf, "dhcp_relay_port") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, AP_PORT_MIN, AP_PORT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid dhcp_relay_port %d",
				line, val);
			return 1;
		}
		bss->dhcp_relay_port = val;
#endif /* CONFIG_FILS */
#ifdef CONFIG_ALLOW_HOSTAPD
	} else if (os_strcmp(buf, "multicast_to_unicast") == 0) {
		bss->multicast_to_unicast = atoi(pos);
#endif /* CONFIG_ALLOW_HOSTAPD */
	} else if (os_strcmp(buf, "broadcast_deauth") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid broadcast_deauth %d",
				line, val);
			return 1;
		}
		bss->broadcast_deauth = val;
#ifdef CONFIG_DPP
	} else if (os_strcmp(buf, "dpp_name") == 0) {
		os_free(bss->dpp_name);
		bss->dpp_name = os_strdup(pos);
	} else if (os_strcmp(buf, "dpp_mud_url") == 0) {
		os_free(bss->dpp_mud_url);
		bss->dpp_mud_url = os_strdup(pos);
	} else if (os_strcmp(buf, "dpp_connector") == 0) {
		os_free(bss->dpp_connector);
		bss->dpp_connector = os_strdup(pos);
	} else if (os_strcmp(buf, "dpp_netaccesskey") == 0) {
		if (parse_wpabuf_hex(line, buf, &bss->dpp_netaccesskey, pos))
			return 1;
	} else if (os_strcmp(buf, "dpp_netaccesskey_expiry") == 0) {
		bss->dpp_netaccesskey_expiry = strtol(pos, NULL, 0);
	} else if (os_strcmp(buf, "dpp_csign") == 0) {
		if (parse_wpabuf_hex(line, buf, &bss->dpp_csign, pos))
			return 1;
#ifdef CONFIG_ALLOW_HOSTAPD
#ifdef CONFIG_DPP2
	} else if (os_strcmp(buf, "dpp_controller") == 0) {
		if (hostapd_dpp_controller_parse(bss, pos))
			return 1;
	} else if (os_strcmp(buf, "dpp_configurator_connectivity") == 0) {
		bss->dpp_configurator_connectivity = atoi(pos);
	} else if (os_strcmp(buf, "dpp_pfs") == 0) {
		int val = atoi(pos);

		if (val < 0 || val > 2) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid dpp_pfs value '%s'",
				   line, pos);
			return -1;
		}
		bss->dpp_pfs = val;
#endif /* CONFIG_DPP2 */
#endif /* CONFIG_ALLOW_HOSTAPD */
#endif /* CONFIG_DPP */
#ifdef CONFIG_OWE
	} else if (os_strcmp(buf, "owe_transition_bssid") == 0) {
		if (hwaddr_aton(pos, bss->owe_transition_bssid)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid owe_transition_bssid",
				   line);
			return 1;
		}
	} else if (os_strcmp(buf, "owe_transition_ssid") == 0) {
		size_t slen;
		char *str = wpa_config_parse_string(pos, &slen);

		if (!str || slen < 1 || slen > SSID_MAX_LEN) {
			wpa_printf(MSG_ERROR, "Line %d: invalid SSID '%s'",
				   line, pos);
			os_free(str);
			return 1;
		}
		os_memcpy(bss->owe_transition_ssid, str, slen);
		bss->owe_transition_ssid_len = slen;
		os_free(str);
	} else if (os_strcmp(buf, "owe_transition_ifname") == 0) {
		os_strlcpy(bss->owe_transition_ifname, pos,
			   sizeof(bss->owe_transition_ifname));
	} else if (os_strcmp(buf, "owe_groups") == 0) {
		if (hostapd_parse_intlist(&bss->owe_groups, pos)) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid owe_groups value '%s'",
				   line, pos);
			return 1;
		}
	} else if (os_strcmp(buf, "owe_ptk_workaround") == 0) {
		bss->owe_ptk_workaround = atoi(pos);
	} else if (os_strcmp(buf, "coloc_intf_reporting") == 0) {
		bss->coloc_intf_reporting = atoi(pos);
#endif /* CONFIG_OWE */
	} else if (os_strcmp(buf, "scan_timeout") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SCAN_TIMEOUT_MIN, SCAN_TIMEOUT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid scan timeout %d, must be between %d..%d",
				   line, val, SCAN_TIMEOUT_MIN, SCAN_TIMEOUT_MAX);
			return 1;
		}
		bss->scan_timeout = (u32)val;

	} else if (os_strcmp(buf, "sAggrConfig") == 0) {
		bss->sAggrConfigSize = hostapd_intlist_size(pos) * sizeof(int);
		if(hostapd_parse_intlist(&bss->sAggrConfig, pos)){
			bss->sAggrConfigSize = 0;
			return 1;
		}

	} else if (os_strcmp(buf, "s11nProtection") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, N_PROTECTION_MIN, N_PROTECTION_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid s11nProtection %d",
				line, val);
			return 1;
		}
		bss->s11nProtection = val;

	} else if (os_strcmp(buf, "sDisableSoftblock") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid sDisableSoftblock %d",
				line, val);
			return 1;
		}
		bss->sDisableSoftblock = val;

	} else if (os_strcmp(buf, "sFwrdUnkwnMcast") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid sFwrdUnkwnMcast %d",
				line, val);
			return 1;
		}
		bss->sFwrdUnkwnMcast = val;

	} else if (os_strcmp(buf, "sBridgeMode") == 0) {
		bss->sBridgeMode = atoi(pos);

	} else if (os_strcmp(buf, "sAddPeerAP") == 0) {
		char * start = pos;
		int i, j=0;
		char local_macaddr[18];
		bss->num_sAddPeerAP = 1;

		while (*pos != '\0') {
			if (*pos == ',')
				bss->num_sAddPeerAP++;
			pos++;
		}
		wpa_printf(MSG_WARNING, "bss->num_sAddPeerAP '%d'", bss->num_sAddPeerAP);
		bss->sAddPeerAP = malloc(sizeof(macaddr)*bss->num_sAddPeerAP);

		pos = start;
		while (*pos != '\0') {

			for (i=0; i < sizeof(local_macaddr) - 1; i++) {
				local_macaddr[i] = *pos;
				pos++;
			}
			local_macaddr[sizeof(local_macaddr) - 1] = '\0';

			if (hwaddr_aton(local_macaddr, bss->sAddPeerAP[j])) {
				wpa_printf(MSG_ERROR, "Invalid MAC address '%s'", local_macaddr);
				free(bss->sAddPeerAP);
				bss->sAddPeerAP = NULL;
				pos = start;
				return 1;
			}
			j++;

			if (*pos != ',' && *pos != '\0') {
				wpa_printf(MSG_ERROR, "Invalid MAC address at sAddPeerAP");
				free(bss->sAddPeerAP);
				bss->sAddPeerAP = NULL;
				pos = start;
				return 1;
			}
			if (*pos != '\0') pos++;
		}
		pos = start;

	} else if (os_strcmp(buf, "sUdmaEnabled") == 0) {
			int val = atoi(pos);
			if (!IS_VALID_BOOL_RANGE(val)) {
				wpa_printf(MSG_ERROR, "Line %d: invalid sUdmaEnabled %d",
					line, val);
				return 1;
			}
			bss->sUdmaEnabled = val;

	} else if (os_strcmp(buf, "sUdmaVlanId") == 0) {
			bss->sUdmaVlanId = atoi(pos);

	} else if (os_strcmp(buf, "ap_retry_limit") == 0) {
		uint32_t ap_retry_limit = atoi(pos);
		if (ap_retry_limit > AP_TX_RETRY_LIMIT_MAX) {
			wpa_printf(MSG_ERROR, "Invalid AP retry limit value %d",
				ap_retry_limit);
			return 1;
		}
		conf->ap_retry_limit = ap_retry_limit;
	} else if (os_strcmp(buf, "ap_retry_limit_data") == 0) {
		uint32_t ap_retry_limit_data = atoi(pos);
		if (ap_retry_limit_data > AP_TX_RETRY_LIMIT_MAX) {
			wpa_printf(MSG_ERROR, "Invalid AP retry limit data value %d",
					ap_retry_limit_data);
			return 1;
		}
		conf->ap_retry_limit_data = ap_retry_limit_data;
	} else if (os_strcmp(buf, "ap_retry_limit_exce") == 0) {
		u8 ap_retry_limit_exce = atoi(pos);
		if (ap_retry_limit_exce > AP_TX_EXCE_RETRY_LIMIT_DEFAULT) {
			wpa_printf(MSG_ERROR, "Invalid AP exce retry limit value %d",
					ap_retry_limit_exce);
			return 1;
		}
		bss->ap_retry_limit_exce = ap_retry_limit_exce;
	} else if (os_strcmp(buf, "sPowerSelection") == 0) {
		conf->sPowerSelection = atoi(pos);

	} else if (os_strcmp(buf, "sCoCPower") == 0) {
		conf->sCoCPowerSize = hostapd_intlist_size(pos);
		if(hostapd_parse_intlist(&conf->sCoCPower, pos))
			return 1;

	} else if (os_strcmp(buf, "sCoCAutoCfg") == 0) {
		if(hostapd_parse_intlist(&conf->sCoCAutoCfg, pos))
			return 1;

	} else if (os_strcmp(buf, "sErpSet") == 0) {
		if(hostapd_parse_intlist(&conf->sErpSet, pos))
			return 1;

	} else if (os_strcmp(buf, "sRadarRssiTh") == 0) {
		conf->sRadarRssiTh = atoi(pos);

	} else if (os_strcmp(buf, "sStationsStat") == 0) {
		conf->sStationsStat = atoi(pos);

	} else if (os_strcmp(buf, "sBfMode") == 0) {
		conf->sBfMode = atoi(pos);

	} else if (os_strcmp(buf, "sMaxMpduLen") == 0) {
		conf->sMaxMpduLen = atoi(pos);

	} else if (os_strcmp(buf, "sProbeReqCltMode") == 0) {
		conf->sProbeReqCltMode = atoi(pos);

	} else if (os_strcmp(buf, "sRTSmode") == 0) {
		if (hostapd_fill_intlist(conf->sRTSmode, pos, ARRAY_SIZE(conf->sRTSmode))) {
			wpa_printf(MSG_ERROR, "Line %d: wrong number of sRTSmode parameters, expected %d",
				   line, (int)ARRAY_SIZE(conf->sRTSmode));
			return 1;
		}
	} else if (os_strcmp(buf, "sFixedRateCfg") == 0) {
		if(hostapd_parse_intlist(&conf->sFixedRateCfg, pos))
			return 1;

	} else if (os_strcmp(buf, "tx_mcs_set_defined") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid tx_mcs_set_defined %d",
				line, val);
			return 1;
		}
		conf->tx_mcs_set_defined = val;

	} else if (os_strcmp(buf, "ignore_supported_channels_errors") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid ignore_supported_channels_errors %d",
				line, val);
			return 1;
		}
		conf->ignore_supported_channels_errors = val;

	} else if (os_strcmp(buf, "sQAMplus") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid sQAMplus %d",
				line, val);
			return 1;
		}
		conf->sQAMplus = val;

	} else if (os_strcmp(buf, "sInterferDetThresh") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, INTERFERENCE_DET_THRESH_MIN, INTERFERENCE_DET_THRESH_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid interference detection threshold value %d",
				   line, val);
			return 1;
		}
		conf->sInterferDetThresh = val;
	} else if (os_strcmp(buf, "sCcaAdapt") == 0) {
		if(hostapd_fill_intlist(conf->sCcaAdapt, pos, ARRAY_SIZE(conf->sCcaAdapt))) {
			wpa_printf(MSG_ERROR, "Line %d: wrong number of sCcaAdapt parameters, expected %d",
				   line, (int)ARRAY_SIZE(conf->sCcaAdapt));
			return 1;
		}
	} else if (os_strcmp(buf, "sFWRecovery") == 0) {
		conf->sFWRecoverySize = hostapd_intlist_size(pos) * sizeof(int);
		if(hostapd_parse_intlist(&conf->sFWRecovery, pos)){
			conf->sFWRecoverySize = 0;
			return 1;
		}
	} else if (os_strcmp(buf, "sConfigMRCoex") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, COEX_MODE_MIN, COEX_MODE_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid sConfigMRCoex '%s'",
				line, buf);
			return 1;
		}
		conf->sConfigMRCoex = val;

	} else if (os_strcmp(buf, "sConfigMRCoexActiveTime") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, COEX_ACTIVE_TIME_MIN, COEX_ACTIVE_TIME_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid sConfigMRCoexActiveTime '%s'",
				line, buf);
			return 1;
		}
		conf->sConfigMRCoexActiveTime = val;

	} else if (os_strcmp(buf, "sConfigMRCoexInactiveTime") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, COEX_INACTIVE_TIME_MIN, COEX_INACTIVE_TIME_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid sConfigMRCoexInactiveTime '%s'",
				line, buf);
			return 1;
		}
		conf->sConfigMRCoexInactiveTime = val;

	} else if (os_strcmp(buf, "sConfigMRCoexCts2SelfActive") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid sConfigMRCoexCts2SelfActive '%s'",
				line, buf);
			return 1;
		}
		conf->sConfigMRCoexCts2SelfActive = val;

	} else if (os_strcmp(buf, "multi_ap") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_RANGE(val, MULTI_AP_MIN, MULTI_AP_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid multi_ap '%s'",
				   line, buf);
			return -1;
		}
		bss->multi_ap = val;
	} else if (os_strcmp(buf, "multi_ap_profile") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_RANGE(val, MULTI_AP_PROFILE_MIN, MULTI_AP_PROFILE_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid multi_ap_profile '%s'",
				   line, buf);
			return -1;
		}
		conf->multi_ap_profile = val;
	} else if (os_strcmp(buf, "multi_ap_profile1_disallow") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid multi_ap_profile1_disallow '%s'",
				   line, buf);
			return -1;
		}
		bss->multi_ap_profile1_disallow = val;
	} else if (os_strcmp(buf, "multi_ap_profile2_disallow") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid multi_ap_profile2_disallow '%s'",
				   line, buf);
			return -1;
		}
		bss->multi_ap_profile2_disallow = val;
	} else if (os_strcmp(buf, "multi_ap_primary_vlanid") == 0) {
		int val = atoi(pos);
		if(!IS_VALID_RANGE(val, VLAN_ID_DISABLED, MAX_VLAN_ID)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid multi_ap_primary_vlanid '%s'",
				   line, buf);
			return -1;
		}
		conf->multi_ap_primary_vlanid = val;
	} else if (os_strcmp(buf, "multi_ap_vlan_per_bsta") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid multi_ap_vlan_per_bsta '%s'",
				   line, buf);
			return -1;
		}
		bss->multi_ap_vlan_per_bsta = val;
	} else if (os_strcmp(buf, "rssi_reject_assoc_rssi") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, RSSI_REJECT_ASSOC_RSSI_MIN, RSSI_REJECT_ASSOC_RSSI_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rssi_reject_assoc_rssi value %d",
				line, val);
			return 1;
		}
		conf->rssi_reject_assoc_rssi = val;
	} else if (os_strcmp(buf, "rssi_reject_assoc_timeout") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, RSSI_REJECT_ASSOC_TIMEOUT_MIN, RSSI_REJECT_ASSOC_TIMEOUT_MAX)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rssi_reject_assoc_timeout value %d",
				line, val);
			return 1;
		}
		conf->rssi_reject_assoc_timeout = val;
	} else if (os_strcmp(buf, "pbss") == 0) {
		bss->pbss = atoi(pos);
	} else if (os_strcmp(buf, "transition_disable") == 0) {
		bss->transition_disable = strtol(pos, NULL, 16);
#ifdef CONFIG_AIRTIME_POLICY
	} else if (os_strcmp(buf, "airtime_mode") == 0) {
		int val = atoi(pos);

		if (val < 0 || val > AIRTIME_MODE_MAX) {
			wpa_printf(MSG_ERROR, "Line %d: Unknown airtime_mode",
				   line);
			return 1;
		}
		conf->airtime_mode = val;
	} else if (os_strcmp(buf, "airtime_update_interval") == 0) {
		conf->airtime_update_interval = atoi(pos);
	} else if (os_strcmp(buf, "airtime_bss_weight") == 0) {
		bss->airtime_weight = atoi(pos);
	} else if (os_strcmp(buf, "airtime_bss_limit") == 0) {
		int val = atoi(pos);

		if (val < 0 || val > 1) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid airtime_bss_limit (must be 0 or 1)",
				   line);
			return 1;
		}
		bss->airtime_limit = val;
	} else if (os_strcmp(buf, "airtime_sta_weight") == 0) {
		if (add_airtime_weight(bss, pos) < 0) {
			wpa_printf(MSG_ERROR,
				   "Line %d: Invalid airtime weight '%s'",
				   line, pos);
			return 1;
	}
#endif /* CONFIG_AIRTIME_POLICY */
#ifdef CONFIG_MACSEC
	} else if (os_strcmp(buf, "macsec_policy") == 0) {
		int macsec_policy = atoi(pos);

		if (macsec_policy < 0 || macsec_policy > 1) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid macsec_policy (%d): '%s'.",
				   line, macsec_policy, pos);
			return 1;
		}
		bss->macsec_policy = macsec_policy;
	} else if (os_strcmp(buf, "macsec_integ_only") == 0) {
		int macsec_integ_only = atoi(pos);

		if (macsec_integ_only < 0 || macsec_integ_only > 1) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid macsec_integ_only (%d): '%s'.",
				   line, macsec_integ_only, pos);
			return 1;
		}
		bss->macsec_integ_only = macsec_integ_only;
	} else if (os_strcmp(buf, "macsec_replay_protect") == 0) {
		int macsec_replay_protect = atoi(pos);

		if (macsec_replay_protect < 0 || macsec_replay_protect > 1) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid macsec_replay_protect (%d): '%s'.",
				   line, macsec_replay_protect, pos);
			return 1;
		}
		bss->macsec_replay_protect = macsec_replay_protect;
	} else if (os_strcmp(buf, "macsec_replay_window") == 0) {
		bss->macsec_replay_window = atoi(pos);
	} else if (os_strcmp(buf, "macsec_port") == 0) {
		int macsec_port = atoi(pos);

		if (macsec_port < 1 || macsec_port > 65534) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid macsec_port (%d): '%s'.",
				   line, macsec_port, pos);
			return 1;
		}
		bss->macsec_port = macsec_port;
	} else if (os_strcmp(buf, "mka_priority") == 0) {
		int mka_priority = atoi(pos);

		if (mka_priority < 0 || mka_priority > 255) {
			wpa_printf(MSG_ERROR,
				   "Line %d: invalid mka_priority (%d): '%s'.",
				   line, mka_priority, pos);
			return 1;
		}
		bss->mka_priority = mka_priority;
	} else if (os_strcmp(buf, "mka_cak") == 0) {
		size_t len = os_strlen(pos);

		if (len > 2 * MACSEC_CAK_MAX_LEN ||
		    (len != 2 * 16 && len != 2 * 32) ||
		    hexstr2bin(pos, bss->mka_cak, len / 2)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid MKA-CAK '%s'.",
				   line, pos);
			return 1;
		}
		bss->mka_cak_len = len / 2;
		bss->mka_psk_set |= MKA_PSK_SET_CAK;
	} else if (os_strcmp(buf, "mka_ckn") == 0) {
		size_t len = os_strlen(pos);

		if (len > 2 * MACSEC_CKN_MAX_LEN || /* too long */
		    len < 2 || /* too short */
		    len % 2 != 0 /* not an integral number of bytes */) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid MKA-CKN '%s'.",
				   line, pos);
			return 1;
		}
		bss->mka_ckn_len = len / 2;
		if (hexstr2bin(pos, bss->mka_ckn, bss->mka_ckn_len)) {
			wpa_printf(MSG_ERROR, "Line %d: Invalid MKA-CKN '%s'.",
				   line, pos);
			return -1;
		}
		bss->mka_psk_set |= MKA_PSK_SET_CKN;
#endif /* CONFIG_MACSEC */
	} else if (os_strcmp(buf, "vendor_4addr") == 0) {
		bss->vendor_4addr = atoi(pos);
	} else if (os_strcmp(buf, "colocated_6g_radio_info") == 0) {
		if( hostapd_parse_colocated_6g_radio_info(conf, pos) )
			return 1;
	} else if (os_strcmp(buf, "colocated_6g_vap_info") == 0) {
		if ( hostapd_parse_colocated_6g_vap_info(conf, bss, pos) )
			return 1;
	 } else if (os_strcmp(buf, "unsolicited_frame_support") == 0) {
                if ( hostapd_parse_unsolicited_frame_support(conf, pos) )
                        return 1;
	 } else if (os_strcmp(buf, "unsolicited_frame_duration") == 0) {
                if ( hostapd_parse_unsolicited_frame_duration(conf, pos) )
                        return 1;
	} else if (os_strcmp(buf, "he_beacon") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid he_beacon %d",
				line, val);
			return 1;
		}
		conf->he_beacon = val;
	} else if (os_strcmp(buf, "rnr_auto_update") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "Line %d: invalid rnr_auto_update %d",
				line, val);
			return 1;
		}
		bss->rnr_auto_update = val;
#ifdef CONFIG_WIFI_CERTIFICATION
/* This is required for WFA test (5.2.4 STAUT) where AP is expected to be configured with different security modes.
 * WFA script will provide wpa hex buffer containing different suites.
 * This is per BSS configuration and valid for 6G band.
 * and must be set in conf at end of each BSS configuration.
 */
	} else if (is_6ghz_op_class(conf->op_class) && os_strcmp(buf, "wpa_hex_buf") == 0) {
		int ret;
		ret = hostapd_config_parse_wpa_hex_buf(bss,pos);
		if(ret) {
			wpa_printf(MSG_ERROR,
                        "wpa hex buf parsing failed");
			return 1;
		}
#endif
	} else if (os_strcmp(buf, "edmg_channel") == 0) {
		conf->edmg_channel = atoi(pos);
	} else if (os_strcmp(buf, "enable_edmg") == 0) {
		conf->enable_edmg = atoi(pos);
	} else if (os_strcmp(buf, "soft_block_acl_enable") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "line %d: invalid soft_block_acl_enable %d", line, val);
			return 1;
		}
		bss->soft_block_acl_enable = val;
	} else if (os_strcmp(buf, "soft_block_acl_wait_time") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SOFT_BLOCK_WAIT_TIME_MIN, SOFT_BLOCK_WAIT_TIME_MAX)) {
			wpa_printf(MSG_ERROR, "line %d: invalid soft_block_acl_wait_time %d", line, val);
			return 1;
		}
		bss->soft_block_acl_wait_time = (unsigned)val;
	} else if (os_strcmp(buf, "soft_block_acl_allow_time") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SOFT_BLOCK_ALLOW_TIME_MIN, SOFT_BLOCK_ALLOW_TIME_MAX)) {
			wpa_printf(MSG_ERROR, "line %d: invalid soft_block_acl_allow_time %d", line, val);
			return 1;
		}
		bss->soft_block_acl_allow_time = (unsigned)val;
	} else if (os_strcmp(buf, "soft_block_acl_on_probe_req") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SOFT_BLOCK_ON_PROBE_REQ_MIN, SOFT_BLOCK_ON_PROBE_REQ_MAX)) {
			wpa_printf(MSG_ERROR, "line %d: invalid soft_block_acl_on_probe_req %d", line, val);
			return 1;
		}
		bss->soft_block_acl_on_probe_req = val;
	} else if (os_strcmp(buf, "soft_block_acl_on_auth_req") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, SOFT_BLOCK_ON_AUTH_REQ_STATUS_CODE_MIN, SOFT_BLOCK_ON_AUTH_REQ_STATUS_CODE_MAX)) {
			wpa_printf(MSG_ERROR, "line %d: invalid soft_block_acl_on_auth_req %d", line, val);
			return 1;
		}
		bss->soft_block_acl_on_auth_req = val;
	} else if (os_strcmp(buf, "dynamic_failsafe") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "line %d: invalid dynamic_failsafe %d", line, val);
			return 1;
		}
		conf->dynamic_failsafe = val;
	} else if (os_strcmp(buf, "mu_mimo_operation") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "line %d: invalid mu mimo opeartion %d", line, val);
			return 1;
		}
		conf->sMuOperation = val;
	} else if (os_strcmp(buf, "adv_proto_query_resp_len") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, ADV_PROTO_QUERY_RESP_LEN_MIN, ADV_PROTO_QUERY_RESP_LEN_MAX)) {
			wpa_printf(MSG_ERROR, "line %d: invalid adv_proto_query_resp_len %d", line, val);
			return 1;
		}
		conf->adv_proto_query_resp_len = (unsigned)val;
	} else if (os_strcmp(buf, "override_6g_mbssid_default_mode") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val)) {
			wpa_printf(MSG_ERROR, "line %d: invalid 6g mbssid default mode override value %d", line, val);
			return 1;
		}
		conf->override_6g_mbssid_default_mode = val;
	} else if (os_strcmp(buf, "start_after") == 0) {
		os_strlcpy(conf->start_after, pos, IFNAMSIZ);
	} else if (os_strcmp(buf, "start_after_delay") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, IFACE_START_DELAY_MIN, IFACE_START_DELAY_MAX)) {
			wpa_printf(MSG_ERROR, "line %d: start_after_delay value %d outside of valid range %d..%d",
			line, val, IFACE_START_DELAY_MIN, IFACE_START_DELAY_MAX);
			return 1;
		}
		conf->start_after_delay = val;
	} else if (os_strcmp(buf, "event_cache_interval") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, 0, MAX_EVENT_CACHE_INTERVAL)) {
			wpa_printf(MSG_ERROR, "line %d: event_cache_interval value %d outside of valid range",
			line, val);
			return 1;
		}
		conf->event_cache_interval = val;
	} else if (os_strcmp(buf, "start_after_watchdog_time") == 0) {
		int val = atoi(pos);
		if (!IS_VALID_RANGE(val, IFACE_START_DELAY_MIN, IFACE_START_DELAY_MAX)) {
			wpa_printf(MSG_ERROR, "line %d: start_after_watchdog_time value %d outside of valid range %d..%d",
			line, val, IFACE_START_DELAY_MIN, IFACE_START_DELAY_MAX);
			return 1;
		}
		conf->start_after_watchdog_time = val;
	} else if (os_strcmp(buf, "whm_config_file") == 0) {
		os_free(conf->whm_config_file);
		conf->whm_config_file = os_strdup(pos);
		if (conf->whm_config_file != NULL && conf->whm_config_file[0] != '\0')
			hostapd_whm_read_config(&conf->whm_cfg_flag, conf->whm_config_file);
		else
			wpa_printf(MSG_ERROR, "Error Reading whm config file");
	} else if (os_strcmp(buf, "wpa_ie_required_for_mbss") == 0) {
		/* This is valid for Non-transmit BSS (MultiBSS scenario).
		 * if this is set, Non transmit BSS will always carry Security IE (RSN/RSNX/WPA) etc.
		 * irrespective of Security method is same as it's Transmit BSS or not.
		 * And this can be set under testbed_mode only.
		 */
		int val = atoi(pos);
		if (!IS_VALID_BOOL_RANGE(val) || (!conf->testbed_mode)) {
				wpa_printf(MSG_ERROR, "Line %d: wpa_ie_required_for_mbss failed !!! %d",
                line, val);
				return 1;
		}
		bss->wpa_ie_required_for_mbss = val;
	} else {
		wpa_printf(MSG_ERROR,
			   "Line %d: unknown configuration item '%s'",
			   line, buf);
		return 1;
	}

	return 0;
}


/**
 * hostapd_config_read - Read and parse a configuration file
 * @fname: Configuration file name (including path, if needed)
 * Returns: Allocated configuration data structure
 */
struct hostapd_config * hostapd_config_read(const char *fname)
{
	struct hostapd_config *conf;
	FILE *f;
	char buf[4096], *pos;
	int line = 0;
	int errors = 0;
	size_t i;

	f = fopen(fname, "r");
	if (f == NULL) {
		wpa_printf(MSG_ERROR, "Could not open configuration file '%s' "
			   "for reading.", fname);
		return NULL;
	}

	conf = hostapd_config_defaults();
	if (conf == NULL) {
		fclose(f);
		return NULL;
	}

	/* set default driver based on configuration */
	conf->driver = wpa_drivers[0];
	if (conf->driver == NULL) {
		wpa_printf(MSG_ERROR, "No driver wrappers registered!");
		hostapd_config_free(conf);
		fclose(f);
		return NULL;
	}

	conf->last_bss = conf->bss[0];

	while (fgets(buf, sizeof(buf), f)) {
		struct hostapd_bss_config *bss;

		bss = conf->last_bss;
		line++;

		if (buf[0] == '#')
			continue;
		pos = buf;
		while (*pos != '\0') {
			if (*pos == '\n') {
				*pos = '\0';
				break;
			}
			pos++;
		}
		if (buf[0] == '\0')
			continue;

		pos = os_strchr(buf, '=');
		if (pos == NULL) {
			wpa_printf(MSG_ERROR, "Line %d: invalid line '%s'",
				   line, buf);
			errors++;
			continue;
		}
		*pos = '\0';
		pos++;
		if (hostapd_config_fill(conf, bss, buf, pos, line))
			errors++;
	}

	fclose(f);

	for (i = 0; i < conf->num_bss; i++) {
		hostapd_set_security_params(conf->bss[i], 1);
		if (conf->bss[i]->twt_responder_support == -1) {
			conf->bss[i]->twt_responder_support = conf->ieee80211ax;
		}
	}

	if (hostapd_config_check(conf, 1))
		errors++;

#ifndef WPA_IGNORE_CONFIG_ERRORS
	if (errors) {
		wpa_printf(MSG_ERROR, "%d errors found in configuration file "
			   "'%s'", errors, fname);
		hostapd_config_free(conf);
		conf = NULL;
	}
#endif /* WPA_IGNORE_CONFIG_ERRORS */

	return conf;
}


int hostapd_set_iface(struct hostapd_config *conf,
		      struct hostapd_bss_config *bss, const char *field,
		      char *value)
{
	int errors;
	size_t i;

	errors = hostapd_config_fill(conf, bss, field, value, 0);
	if (errors) {
		wpa_printf(MSG_INFO, "Failed to set configuration field '%s' "
			   "to value '%s'", field, value);
		return -1;
	}

	for (i = 0; i < conf->num_bss; i++)
		hostapd_set_security_params(conf->bss[i], 0);

	if (hostapd_config_check(conf, 0)) {
		wpa_printf(MSG_ERROR, "Configuration check failed");
		return -1;
	}

	return 0;
}
