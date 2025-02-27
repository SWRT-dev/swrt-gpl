/*
 * hostapd / Configuration file parser
 * Copyright (c) 2003-2009, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef CONFIG_FILE_H
#define CONFIG_FILE_H

struct hostapd_config * hostapd_config_read(const char *fname);
int hostapd_set_iface(struct hostapd_config *conf,
		      struct hostapd_bss_config *bss, const char *field,
		      char *value);
int hostapd_acl_comp(const void *a, const void *b);
int hostapd_add_acl_maclist(struct mac_acl_entry **acl, int *num,
			    int vlan_id, const u8 *addr);
void hostapd_remove_acl_mac(struct mac_acl_entry **acl, int *num,
			    const u8 *addr);
int hs20_parse_wan_metrics(struct hostapd_bss_config *bss, char *buf, int line);
int hostapd_config_fill(struct hostapd_config *conf,
			       struct hostapd_bss_config *bss,
			       const char *buf, char *pos, int line);

int hostapd_soft_block_add_acl_maclist(struct soft_block_acl_db *acl, const u8 *addr);
void hostapd_soft_block_remove_acl_mac(struct soft_block_acl_db *acl, const u8 *addr);
int hostapd_soft_block_acl_comp(const void *a, const void *b);

#endif /* CONFIG_FILE_H */
