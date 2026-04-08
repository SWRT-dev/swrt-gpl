/*
 * hostapd / Hardware feature query and different modes
 * Copyright 2002-2003, Instant802 Networks, Inc.
 * Copyright 2005-2006, Devicescape Software, Inc.
 * Copyright (c) 2008-2011, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef HW_FEATURES_H
#define HW_FEATURES_H

#ifdef NEED_AP_MLME
void hostapd_free_hw_features(struct hostapd_hw_modes *hw_features,
			      size_t num_hw_features);
int hostapd_get_hw_features(struct hostapd_iface *iface);
int hostapd_acs_completed(struct hostapd_iface *iface, int err);
int hostapd_select_hw_mode(struct hostapd_iface *iface);
const char * hostapd_hw_mode_txt(int mode);
int hostapd_hw_get_freq(struct hostapd_data *hapd, int chan);
int hostapd_hw_get_channel(struct hostapd_data *hapd, int freq);
int hostapd_check_ht_capab(struct hostapd_iface *iface);
int hostapd_prepare_rates(struct hostapd_iface *iface,
			  struct hostapd_hw_modes *mode);
void hostapd_stop_setup_timers(struct hostapd_iface *iface);
struct hostapd_channel_data* hostapd_hw_get_channel_data(struct hostapd_iface *iface, int freq);
struct hostapd_channel_data* hostapd_hw_get_channel_data_from_ch(struct hostapd_iface *iface, int chan);
int hostapd_hw_get_antenna_mask(struct hostapd_data *hapd, int tx_ant,
  int rx_ant, u32 *tx_ant_mask, u32 *rx_ant_mask);
#ifndef CONFIG_NO_STDOUT_DEBUG
char * dfs_info(struct hostapd_channel_data *chan);
#endif
#else /* NEED_AP_MLME */
static inline void
hostapd_free_hw_features(struct hostapd_hw_modes *hw_features,
			 size_t num_hw_features)
{
}

static inline int hostapd_get_hw_features(struct hostapd_iface *iface)
{
	return -1;
}

static inline int hostapd_acs_completed(struct hostapd_iface *iface, int err)
{
	return -1;
}

static inline int hostapd_select_hw_mode(struct hostapd_iface *iface)
{
	return -100;
}

static inline const char * hostapd_hw_mode_txt(int mode)
{
	return NULL;
}

static inline int hostapd_hw_get_freq(struct hostapd_data *hapd, int chan)
{
	return -1;
}

static inline int hostapd_check_ht_capab(struct hostapd_iface *iface)
{
	return 0;
}

static inline int hostapd_prepare_rates(struct hostapd_iface *iface,
					struct hostapd_hw_modes *mode)
{
	return 0;
}

static inline void hostapd_stop_setup_timers(struct hostapd_iface *iface)
{
}

static inline struct hostapd_channel_data* hostapd_hw_get_channel_data(struct hostapd_iface *iface, int freq)
{
  return NULL;
}

static inline struct hostapd_channel_data* hostapd_hw_get_channel_data_from_ch(struct hostapd_iface *iface, int chan)
{
  return NULL;
}
static inline int hostapd_hw_get_antenna_mask(struct hostapd_data *hapd, int tx_ant,
  int rx_ant, u32 *tx_ant_mask, u32 *rx_ant_mask)
{
  return 0;
}
#ifndef CONFIG_NO_STDOUT_DEBUG
static inline char * dfs_info(struct hostapd_channel_data *chan)
{
  return NULL;
}
#endif
#endif /* NEED_AP_MLME */

#endif /* HW_FEATURES_H */
