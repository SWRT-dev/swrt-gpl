/*
 * hostapd / UNIX domain socket -based control interface
 * Copyright (c) 2004, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef CTRL_IFACE_H
#define CTRL_IFACE_H

#ifndef CONFIG_NO_CTRL_IFACE
int hostapd_ctrl_iface_init(struct hostapd_data *hapd);
void hostapd_ctrl_iface_deinit(struct hostapd_data *hapd);
int hostapd_global_ctrl_iface_init(struct hapd_interfaces *interface);
void hostapd_global_ctrl_iface_deinit(struct hapd_interfaces *interface);
#else /* CONFIG_NO_CTRL_IFACE */
static inline int hostapd_ctrl_iface_init(struct hostapd_data *hapd)
{
	return 0;
}

static inline void hostapd_ctrl_iface_deinit(struct hostapd_data *hapd)
{
}

static inline int
hostapd_global_ctrl_iface_init(struct hapd_interfaces *interface)
{
	return 0;
}

static inline void
hostapd_global_ctrl_iface_deinit(struct hapd_interfaces *interface)
{
}
#endif /* CONFIG_NO_CTRL_IFACE */

void
set_iface_conf(struct hostapd_iface *iface,
        const struct hostapd_freq_params *freq_params);
int hostapd_get_secondary_offset(int primary, int bandwidth, int vht_seg0_idx);
struct hostapd_data *get_bss_index(const char *cmd, struct hostapd_iface *iface);


#endif /* CTRL_IFACE_H */
