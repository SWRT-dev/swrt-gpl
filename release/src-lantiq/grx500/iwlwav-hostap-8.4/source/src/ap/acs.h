/*
 * ACS - Automatic Channel Selection module
 * Copyright (c) 2011, Atheros Communications
 * Copyright (c) 2013, Qualcomm Atheros, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef ACS_H
#define ACS_H

#ifdef CONFIG_ACS

enum hostapd_chan_status acs_init(struct hostapd_iface *iface);
void acs_cleanup(struct hostapd_iface *iface);
int acs_recalc_ranks_and_set_chan(struct hostapd_iface *iface, int switch_reason);
void acs_update_intolerant_channels(struct hostapd_iface *iface, u8 chan);
void acs_update_radar(struct hostapd_iface *iface);
void acs_radar_switch(struct hostapd_iface *iface);
int acs_do_switch_channel(struct hostapd_iface *iface, int block_tx);
int get_num_width(int vht_width, int secondary);
int acs_chan_to_freq(int channel);
int acs_current_channel_request_scan(struct hostapd_iface *iface);
int acs_get_chan_data_from_candidates_5g (int primary_chan, u8 ch_width,
										   int *secondary_chan_offset, u8 *vht_oper_centr_freq_seg0_idx);

typedef struct {
  u8 pri;
  u8 sec;
  u8 chwidth;
  u8 cent_freq;
    int sel_cand;
} acs_chandef;

#else /* CONFIG_ACS */

static inline enum hostapd_chan_status acs_init(struct hostapd_iface *iface)
{
	wpa_printf(MSG_ERROR, "ACS was disabled on your build, rebuild hostapd with CONFIG_ACS=y or set channel");
	return HOSTAPD_CHAN_INVALID;
}

static inline void acs_cleanup(struct hostapd_iface *iface)
{
}

#endif /* CONFIG_ACS */

#endif /* ACS_H */
