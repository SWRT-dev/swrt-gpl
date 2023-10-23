/*
 * hostapd - WPA/RSN IE and KDE definitions
 * Copyright (c) 2004-2007, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef WPA_AUTH_IE_H
#define WPA_AUTH_IE_H

u8 * wpa_add_kde(u8 *pos, u32 kde, const u8 *data, size_t data_len,
		 const u8 *data2, size_t data2_len);
int wpa_auth_gen_wpa_ie(struct wpa_authenticator *wpa_auth);
int wpa_auth_conf_gen_multibss_wpa_ie(struct wpa_auth_config *conf, u8 **wpa_ie, size_t *wpa_ie_len);
#ifdef CONFIG_WDS_WPA
int wpa_modify_wpa_ie(struct wpa_state_machine *sm, u32 group_suite, u16 capab, int size);
#endif
#endif /* WPA_AUTH_IE_H */
