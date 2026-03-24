/*
 * DFS - Dynamic Frequency Selection
 * Copyright (c) 2002-2013, Jouni Malinen <j@w1.fi>
 * Copyright (c) 2013-2017, Qualcomm Atheros, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/eloop.h"
#include "common/ieee802_11_defs.h"
#include "common/hw_features_common.h"
#include "common/wpa_ctrl.h"
#include "hostapd.h"
#include "ap_drv_ops.h"
#include "drivers/driver.h"
#include "dfs.h"
#include "acs.h"
#include "hw_features.h"
#include "../hostapd/ctrl_iface.h"
#include "drivers/nl80211_copy.h"

int dfs_adjust_debug_dfs_chan (struct hostapd_iface *iface, struct hostapd_channel_data *channel,
			       int *secondary_chan_offset, u8 *vht_oper_centr_freq_seg0_idx)
{
	/* 20 MHz channel configured */
	if (!iface->conf->secondary_channel) {
		*vht_oper_centr_freq_seg0_idx = channel->chan;
		*secondary_chan_offset = 0;
		return 0;
	}

	return hostapd_get_chan_data_from_candidates_5g(channel->chan, hostapd_get_oper_chwidth(iface->conf),
							secondary_chan_offset, vht_oper_centr_freq_seg0_idx);
}


int dfs_get_used_n_chans(struct hostapd_iface *iface, int *seg1)
{
	int n_chans = 1;

	*seg1 = 0;

	if (iface->conf->ieee80211n && iface->conf->secondary_channel)
		n_chans = 2;

	if (iface->conf->ieee80211ac || iface->conf->ieee80211ax) {
		switch (hostapd_get_oper_chwidth(iface->conf)) {
		case CHANWIDTH_USE_HT:
			break;
		case CHANWIDTH_80MHZ:
			n_chans = 4;
			break;
		case CHANWIDTH_160MHZ:
			n_chans = 8;
			break;
		case CHANWIDTH_80P80MHZ:
			n_chans = 4;
			*seg1 = 4;
			break;
		default:
			break;
		}
	}

	return n_chans;
}

/* Cuts channel width by half */
static void dfs_cut_cw_by_half(struct hostapd_iface *iface, bool backup)
{
	/* If this is the first cut, save original channel settings.
	 * Function hostapd_set_freq_params checks if current channel settings are
	 * valid and will refuse to switch if not. */
	if (!iface->conf->cw_reduced && backup) {
		iface->conf->cw_reduced = 1;
		iface->conf->secondary_channel_bak = iface->conf->secondary_channel;
		iface->conf->vht_oper_chwidth_bak = hostapd_get_oper_chwidth(iface->conf);
	}
	if (iface->conf->ieee80211ac || iface->conf->ieee80211ax) {
		switch (hostapd_get_oper_chwidth(iface->conf)) {
		case CHANWIDTH_USE_HT:
			if (iface->conf->ieee80211n)
				iface->conf->secondary_channel = 0;
			break;
		case CHANWIDTH_80MHZ:
			hostapd_set_oper_chwidth(iface->conf,CHANWIDTH_USE_HT);
			break;
		case CHANWIDTH_160MHZ:
		case CHANWIDTH_80P80MHZ:
			hostapd_set_oper_chwidth(iface->conf,CHANWIDTH_80MHZ);
			break;
		default:
			break;
		}
	} else if (iface->conf->ieee80211n)
		iface->conf->secondary_channel = 0;
}

static void dfs_restore_original_cw(struct hostapd_iface *iface)
{
	if (!iface->conf->cw_reduced) return;

	iface->conf->cw_reduced = 0;
	iface->conf->secondary_channel = iface->conf->secondary_channel_bak;
	hostapd_set_oper_chwidth(iface->conf,iface->conf->vht_oper_chwidth_bak);
}

static int dfs_channel_available(struct hostapd_channel_data *chan,
				 int skip_radar)
{
	/*
	 * When radar detection happens, CSA is performed. However, there's no
	 * time for CAC, so radar channels must be skipped when finding a new
	 * channel for CSA, unless they are available for immediate use.
	 */
	if (skip_radar && (chan->flag & HOSTAPD_CHAN_RADAR) &&
	    ((chan->flag & HOSTAPD_CHAN_DFS_MASK) !=
	     HOSTAPD_CHAN_DFS_AVAILABLE))
		return 0;

	if (chan->flag & HOSTAPD_CHAN_DISABLED)
		return 0;
	if ((chan->flag & HOSTAPD_CHAN_RADAR) &&
	    ((chan->flag & HOSTAPD_CHAN_DFS_MASK) ==
	     HOSTAPD_CHAN_DFS_UNAVAILABLE))
		return 0;
	return 1;
}


static int dfs_is_chan_allowed(struct hostapd_channel_data *chan, int n_chans)
{
	/*
	 * The tables contain first valid channel number based on channel width.
	 * We will also choose this first channel as the control one.
	 */
	int allowed_40[] = { 36, 44, 52, 60, 100, 108, 116, 124, 132, 149, 157,
			     184, 192 };
	/*
	 * VHT80, valid channels based on center frequency:
	 * 42, 58, 106, 122, 138, 155
	 */
	int allowed_80[] = { 36, 52, 100, 116, 132, 149 };
	/*
	 * VHT160 valid channels based on center frequency:
	 * 50, 114
	 */
	int allowed_160[] = { 36, 100 };
	int *allowed = allowed_40;
	unsigned int i, allowed_no = 0;

	switch (n_chans) {
	case 2:
		allowed = allowed_40;
		allowed_no = ARRAY_SIZE(allowed_40);
		break;
	case 4:
		allowed = allowed_80;
		allowed_no = ARRAY_SIZE(allowed_80);
		break;
	case 8:
		allowed = allowed_160;
		allowed_no = ARRAY_SIZE(allowed_160);
		break;
	default:
		wpa_printf(MSG_DEBUG, "Unknown width for %d channels", n_chans);
		break;
	}

	for (i = 0; i < allowed_no; i++) {
		if (chan->chan == allowed[i])
			return 1;
	}

	return 0;
}


static struct hostapd_channel_data *
dfs_get_chan_data(struct hostapd_hw_modes *mode, int freq, int first_chan_idx)
{
	int i;

	for (i = first_chan_idx; i < mode->num_channels; i++) {
		if (mode->channels[i].freq == freq)
			return &mode->channels[i];
	}

	return NULL;
}


int dfs_chan_range_available(struct hostapd_hw_modes *mode,
			     int first_chan_idx, int num_chans,
			     int skip_radar)
{
	struct hostapd_channel_data *first_chan, *chan;
	int i;
	u32 bw = num_chan_to_bw(num_chans);

	if (first_chan_idx + num_chans > mode->num_channels) {
		wpa_printf(MSG_DEBUG,
			   "DFS: some channels in range not defined");
		return 0;
	}

	first_chan = &mode->channels[first_chan_idx];

	/* hostapd DFS implementation assumes the first channel as primary.
	 * If it's not allowed to use the first channel as primary, decline the
	 * whole channel range. */
	if (!chan_pri_allowed(first_chan)) {
		wpa_printf(MSG_DEBUG, "DFS: primary chanenl not allowed");
		return 0;
	}

	for (i = 0; i < num_chans; i++) {
		chan = dfs_get_chan_data(mode, first_chan->freq + i * 20,
					 first_chan_idx);
		if (!chan) {
			wpa_printf(MSG_DEBUG, "DFS: no channel data for %d",
				   first_chan->freq + i * 20);
			return 0;
		}

		/* HT 40 MHz secondary channel availability checked only for
		 * primary channel */
		if (!chan_bw_allowed(chan, bw, 1, !i)) {
			wpa_printf(MSG_DEBUG, "DFS: bw now allowed for %d",
				   first_chan->freq + i * 20);
			return 0;
		}

		if (!dfs_channel_available(chan, skip_radar)) {
			wpa_printf(MSG_DEBUG, "DFS: channel not available %d",
				   first_chan->freq + i * 20);
			return 0;
		}
	}

	return 1;
}


static int is_in_chanlist(struct hostapd_iface *iface,
			  struct hostapd_channel_data *chan)
{
	if (!iface->conf->acs_ch_list.num)
		return 1;

	return freq_range_list_includes(&iface->conf->acs_ch_list, chan->chan);
}


/*
 * The function assumes HT40+ operation.
 * Make sure to adjust the following variables after calling this:
 *  - hapd->secondary_channel
 *  - hapd->vht_oper_centr_freq_seg0_idx
 *  - hapd->vht_oper_centr_freq_seg1_idx
 */
static int dfs_find_channel(struct hostapd_iface *iface,
			    struct hostapd_channel_data **ret_chan,
			    int idx, int skip_radar)
{
	struct hostapd_hw_modes *mode;
	struct hostapd_channel_data *chan;
	int i, channel_idx = 0, n_chans, n_chans1;

	mode = iface->current_mode;
	n_chans = dfs_get_used_n_chans(iface, &n_chans1);

	wpa_printf(MSG_DEBUG, "DFS new chan checking %d channels", n_chans);
	for (i = 0; i < mode->num_channels; i++) {
		chan = &mode->channels[i];

		/* Skip HT40/VHT incompatible channels */
		if (iface->conf->ieee80211n &&
		    iface->conf->secondary_channel &&
		    (!dfs_is_chan_allowed(chan, n_chans) ||
		     !(chan->allowed_bw & HOSTAPD_CHAN_WIDTH_40P))) {
			wpa_printf(MSG_DEBUG,
				   "DFS: channel %d (%d) is incompatible",
				   chan->freq, chan->chan);
			continue;
		}

		/* Skip incompatible chandefs */
		if (!dfs_chan_range_available(mode, i, n_chans, skip_radar)) {
			wpa_printf(MSG_DEBUG,
				   "DFS: range not available for %d (%d)",
				   chan->freq, chan->chan);
			continue;
		}

		if (!is_in_chanlist(iface, chan)) {
			wpa_printf(MSG_DEBUG,
				   "DFS: channel %d (%d) not in chanlist",
				   chan->freq, chan->chan);
			continue;
		}

		if (ret_chan && idx == channel_idx) {
			wpa_printf(MSG_DEBUG, "Selected channel %d (%d)",
				   chan->freq, chan->chan);
			*ret_chan = chan;
			return idx;
		}
		wpa_printf(MSG_DEBUG, "Adding channel %d (%d)",
			   chan->freq, chan->chan);
		channel_idx++;
	}
	return channel_idx;
}


static void dfs_adjust_center_freq(struct hostapd_iface *iface,
				   struct hostapd_channel_data *chan,
				   int secondary_channel,
				   u8 *oper_centr_freq_seg0_idx,
				   u8 *oper_centr_freq_seg1_idx)
{
	if (!iface->conf->ieee80211ac)
		return;

	if (!chan)
		return;

	*oper_centr_freq_seg1_idx = 0;

	switch (hostapd_get_oper_chwidth(iface->conf)) {
	case CHANWIDTH_USE_HT:
		if (secondary_channel == 1)
			*oper_centr_freq_seg0_idx = chan->chan + 2;
		else if (secondary_channel == -1)
			*oper_centr_freq_seg0_idx = chan->chan - 2;
		else
			*oper_centr_freq_seg0_idx = chan->chan;
		break;
	case CHANWIDTH_80MHZ:
		*oper_centr_freq_seg0_idx = chan->chan + 6;
		break;
	case CHANWIDTH_160MHZ:
		*oper_centr_freq_seg0_idx = chan->chan + 14;
		break;
	default:
		wpa_printf(MSG_INFO, "DFS only VHT20/40/80/160 is supported now");
		*oper_centr_freq_seg0_idx = 0;
		break;
	}

	wpa_printf(MSG_DEBUG, "DFS adjusting VHT center frequency: %d, %d",
		   *oper_centr_freq_seg0_idx,
		   *oper_centr_freq_seg1_idx);
}


/* Return start channel idx we will use for mode->channels[idx] */
int dfs_get_start_chan_idx(struct hostapd_iface *iface, int *seg1_start)
{
	struct hostapd_hw_modes *mode;
	struct hostapd_channel_data *chan;
	int channel_no = iface->conf->channel;
	int res = -1, i;
	int chan_seg1 = -1;
	u8 seg0_idx = 0, seg1_idx = 0;

	*seg1_start = -1;

	/* HT40- */
	if (iface->conf->ieee80211n && iface->conf->secondary_channel == -1)
		channel_no -= 4;

	seg0_idx = hostapd_get_oper_centr_freq_seg0_idx(iface->conf);
	seg1_idx = hostapd_get_oper_centr_freq_seg1_idx(iface->conf);

	/* VHT/HE */
	if (iface->conf->ieee80211ac || iface->conf->ieee80211ax) {
		switch (hostapd_get_oper_chwidth(iface->conf)) {
		case CHANWIDTH_USE_HT:
			break;
		case CHANWIDTH_80MHZ:
			channel_no =
				seg0_idx - 6;
			break;
		case CHANWIDTH_160MHZ:
			channel_no =
				seg0_idx - 14;
			break;
		case CHANWIDTH_80P80MHZ:
			channel_no =
				seg0_idx - 6;
			chan_seg1 =
				seg1_idx - 6;
			break;
		default:
			wpa_printf(MSG_INFO,
				   "DFS only VHT20/40/80/160/80+80 is supported now");
			channel_no = -1;
			break;
		}
	}

	/* Get idx */
	mode = iface->current_mode;
	for (i = 0; i < mode->num_channels; i++) {
		chan = &mode->channels[i];
		if (chan->chan == channel_no) {
			res = i;
			break;
		}
	}

	if (res != -1 && chan_seg1 > -1) {
		int found = 0;

		/* Get idx for seg1 */
		mode = iface->current_mode;
		for (i = 0; i < mode->num_channels; i++) {
			chan = &mode->channels[i];
			if (chan->chan == chan_seg1) {
				*seg1_start = i;
				found = 1;
				break;
			}
		}
		if (!found)
			res = -1;
	}

	if (res == -1) {
		wpa_printf(MSG_DEBUG,
			   "DFS chan_idx seems wrong; num-ch: %d ch-no: %d conf-ch-no: %d 11n: %d sec-ch: %d oper-width: %d",
			   mode->num_channels, channel_no, iface->conf->channel,
			   iface->conf->ieee80211n,
			   iface->conf->secondary_channel,
			   hostapd_get_oper_chwidth(iface->conf));

		for (i = 0; i < mode->num_channels; i++) {
			wpa_printf(MSG_DEBUG, "Available channel: %d",
				   mode->channels[i].chan);
		}
	}

	return res;
}

int set_first_non_dfs_channel(struct hostapd_iface *iface)
{
	int j;
	int flag;
	struct hostapd_hw_modes *mode = iface->current_mode;
	struct hostapd_config *conf = iface->conf;
	u8 channel = 0;

	for (j = 0; j < mode->num_channels; j++) {
		flag = mode->channels[j].flag;
		if (!(flag & (HOSTAPD_CHAN_DISABLED | HOSTAPD_CHAN_RADAR))) {
			channel = mode->channels[j].chan;
			break;
		}
	}
	if (!channel) {
		wpa_printf(MSG_ERROR, "Could not find non dfs channels");
		return -1;
	}
	conf->channel = channel;
	hostapd_set_oper_chwidth(conf, CHANWIDTH_USE_HT);
	conf->secondary_channel = 0;
	hostapd_set_oper_centr_freq_seg0_idx(conf, conf->channel);
	hostapd_set_oper_centr_freq_seg1_idx(conf, 0);
	iface->freq = hw_get_freq(iface->current_mode, channel);
	wpa_printf(MSG_DEBUG, "Chosen channel %d", channel);
	return 0;
}

/* At least one channel have radar flag */
static int dfs_check_chans_radar(struct hostapd_iface *iface,
				 int start_chan_idx, int n_chans)
{
	struct hostapd_channel_data *channel;
	struct hostapd_hw_modes *mode;
	int i, res = 0;

	mode = iface->current_mode;

	for (i = 0; i < n_chans; i++) {
		channel = &mode->channels[start_chan_idx + i];
		if (channel->flag & HOSTAPD_CHAN_RADAR)
			res++;
	}

	return res;
}


/* All channels available */
static int dfs_check_chans_available(struct hostapd_iface *iface,
				     int start_chan_idx, int n_chans)
{
	struct hostapd_channel_data *channel;
	struct hostapd_hw_modes *mode;
	int i;

	mode = iface->current_mode;

	for (i = 0; i < n_chans; i++) {
		channel = &mode->channels[start_chan_idx + i];

		if (channel->flag & HOSTAPD_CHAN_DISABLED)
			break;

		if (!(channel->flag & HOSTAPD_CHAN_RADAR))
			continue;

		if ((channel->flag & HOSTAPD_CHAN_DFS_MASK) !=
		    HOSTAPD_CHAN_DFS_AVAILABLE)
			break;
	}

	return i == n_chans;
}


/* At least one channel unavailable */
static int dfs_check_chans_unavailable(struct hostapd_iface *iface,
				       int start_chan_idx,
				       int n_chans)
{
	struct hostapd_channel_data *channel;
	struct hostapd_hw_modes *mode;
	int i, res = 0;

	mode = iface->current_mode;

	for (i = 0; i < n_chans; i++) {
		channel = &mode->channels[start_chan_idx + i];
		if (channel->flag & HOSTAPD_CHAN_DISABLED)
			res++;
		if ((channel->flag & HOSTAPD_CHAN_DFS_MASK) ==
		    HOSTAPD_CHAN_DFS_UNAVAILABLE)
			res++;
	}

	return res;
}


static struct hostapd_channel_data *
dfs_get_valid_channel(struct hostapd_iface *iface,
		      int *secondary_channel,
		      u8 *oper_centr_freq_seg0_idx,
		      u8 *oper_centr_freq_seg1_idx,
		      int skip_radar)
{
	struct hostapd_hw_modes *mode;
	struct hostapd_channel_data *chan = NULL;
	int num_available_chandefs;
	int chan_idx;
	u32 _rand;

	wpa_printf(MSG_DEBUG, "DFS: Selecting random channel");
	*secondary_channel = 0;
	*oper_centr_freq_seg0_idx = 0;
	*oper_centr_freq_seg1_idx = 0;

	if (iface->current_mode == NULL)
		return NULL;

	mode = iface->current_mode;
	if (mode->mode != HOSTAPD_MODE_IEEE80211A)
		return NULL;

	/* Get the count first */
	num_available_chandefs = dfs_find_channel(iface, NULL, 0, skip_radar);
	wpa_printf(MSG_DEBUG, "DFS: num_available_chandefs=%d",
		   num_available_chandefs);
	if (num_available_chandefs == 0)
		return NULL;

	if (os_get_random((u8 *) &_rand, sizeof(_rand)) < 0)
		return NULL;
	chan_idx = _rand % num_available_chandefs;
	dfs_find_channel(iface, &chan, chan_idx, skip_radar);
	if (chan)
		wpa_printf(MSG_DEBUG, "DFS: got random channel %d (%d)",
			   chan->freq, chan->chan);
	else
		wpa_printf(MSG_DEBUG, "DFS: no random channel found");

	/* dfs_find_channel() calculations assume HT40+ */
	if (iface->conf->secondary_channel)
		*secondary_channel = 1;
	else
		*secondary_channel = 0;

	dfs_adjust_center_freq(iface, chan,
			       *secondary_channel,
			       oper_centr_freq_seg0_idx,
			       oper_centr_freq_seg1_idx);

	return chan;
}

bool dfs_is_failsafe_enabled(struct hostapd_iface *iface)
{
	return iface->conf->dynamic_failsafe ||
	       (0 != iface->failsafe.freq);
}

static struct hostapd_channel_data *
dfs_get_channel_fixed_failsafe (struct hostapd_iface *iface,
				struct hostapd_freq_params* freq_params,
				int *secondary_channel,
				u8 *vht_oper_centr_freq_seg0_idx,
				u8 *vht_oper_centr_freq_seg1_idx)
{
	struct hostapd_channel_data *channel;

	channel = hostapd_hw_get_channel_data(iface, iface->failsafe.freq);
	if (!channel)
		return channel;

	memset(freq_params, 0, sizeof(*freq_params));
	freq_params->channel = channel->chan;
	freq_params->freq = iface->failsafe.freq;
	freq_params->center_freq1 = iface->failsafe.center_freq1;
	freq_params->center_freq2 = iface->failsafe.center_freq2;
	freq_params->bandwidth = iface->failsafe.bandwidth;
	if (iface->conf->ieee80211n)
		freq_params->ht_enabled = 1;
	if (iface->conf->ieee80211ac)
		freq_params->vht_enabled = 1;
	set_iface_conf(iface, freq_params);
	*secondary_channel = iface->conf->secondary_channel;
	*vht_oper_centr_freq_seg0_idx = hostapd_get_oper_centr_freq_seg0_idx(iface->conf);
	*vht_oper_centr_freq_seg1_idx = hostapd_get_oper_centr_freq_seg1_idx(iface->conf);

	return channel;
}

static struct hostapd_channel_data *
dfs_get_channel_dynamic_failsafe (struct hostapd_iface *iface,
				  struct hostapd_freq_params* freq_params,
				  int *secondary_channel,
				  u8 *vht_oper_centr_freq_seg0_idx,
				  u8 *vht_oper_centr_freq_seg1_idx)
{
#ifdef CONFIG_ACS
	int primary_freq;
	struct hostapd_channel_data *channel;
	struct acs_candidate_table *failsafe;
	failsafe = acs_get_failsafe_channel(iface);

	if (!failsafe)
		return NULL;

	wpa_printf(MSG_DEBUG, "Dynamic failsafe: Got candidate %d, sec %d, freq %d, bw %d",
		   failsafe->primary, failsafe->secondary, failsafe->freq, failsafe->width);

	primary_freq = hostapd_chan_to_freq(failsafe->primary);
	channel = hostapd_hw_get_channel_data(iface, primary_freq);
	if (!channel) {
		wpa_printf(MSG_ERROR, "DFS dynamic failsafe: Failed to get channel data for channel %d",
			   failsafe->primary);
		return NULL;
	}

	memset(freq_params, 0, sizeof(*freq_params));
	freq_params->channel = channel->chan;
	freq_params->freq = primary_freq;

	/* center_freq = low_freq + width / 2 - 10 */
	freq_params->center_freq1 = failsafe->freq + failsafe->width / 2 - 10;
	freq_params->center_freq2 = 0; /* 80+80 Not supported */
	freq_params->bandwidth = failsafe->width;
	if (iface->conf->ieee80211n)
		freq_params->ht_enabled = 1;
	if (iface->conf->ieee80211ac)
		freq_params->vht_enabled = 1;
	set_iface_conf(iface, freq_params);
	*secondary_channel = iface->conf->secondary_channel;
	*vht_oper_centr_freq_seg0_idx = hostapd_get_oper_centr_freq_seg0_idx(iface->conf);
	*vht_oper_centr_freq_seg1_idx = hostapd_get_oper_centr_freq_seg1_idx(iface->conf);

	return channel;
#else
	return NULL;
#endif
}

static struct hostapd_channel_data *
dfs_get_valid_channel_failsafe(struct hostapd_iface *iface,
			       struct hostapd_freq_params* freq_params,
			       int *secondary_channel,
			       u8 *vht_oper_centr_freq_seg0_idx,
			       u8 *vht_oper_centr_freq_seg1_idx)
{
	struct hostapd_channel_data *channel = NULL;

	if (iface->conf->dynamic_failsafe) {
		channel = dfs_get_channel_dynamic_failsafe(iface,
							   freq_params,
							   secondary_channel,
							   vht_oper_centr_freq_seg0_idx,
							   vht_oper_centr_freq_seg1_idx);
	}

	if (channel)
		return channel;

	if (0 != iface->failsafe.freq) {
		wpa_printf(MSG_DEBUG, "Failsafe channel: switching to fixed fail-safe channel %d", iface->failsafe.freq);
		channel = dfs_get_channel_fixed_failsafe(iface,
							 freq_params,
							 secondary_channel,
							 vht_oper_centr_freq_seg0_idx,
							 vht_oper_centr_freq_seg1_idx);
	}

	return channel;
}


int set_dfs_state_freq(struct hostapd_iface *iface, int freq, u32 state)
{
	struct hostapd_hw_modes *mode;
	struct hostapd_channel_data *chan = NULL;
	int i;

	mode = iface->current_mode;
	if (mode == NULL)
		return 0;

	wpa_printf(MSG_DEBUG, "set_dfs_state 0x%X for %d MHz", state, freq);
	for (i = 0; i < iface->current_mode->num_channels; i++) {
		chan = &iface->current_mode->channels[i];
		if (chan->freq == freq) {
			if (chan->flag & HOSTAPD_CHAN_RADAR) {
				chan->flag &= ~HOSTAPD_CHAN_DFS_MASK;
				chan->flag |= state;
				chan->nop_time_passed_min = 0;
				return 1; /* Channel found */
			}
		}
	}
	wpa_printf(MSG_WARNING, "Can't set DFS state for freq %d MHz", freq);
	return 0;
}


static int set_dfs_state(struct hostapd_iface *iface, int freq, int ht_enabled,
			 int chan_offset, int chan_width, int cf1,
			 int cf2, u8 rbm, u32 state)
{
	int n_chans = 1, i;
	struct hostapd_hw_modes *mode;
	int frequency = freq;
	int frequency2 = 0;
	int ret = 0;

	mode = iface->current_mode;
	if (mode == NULL)
		return 0;

	if (mode->mode != HOSTAPD_MODE_IEEE80211A) {
		wpa_printf(MSG_WARNING, "current_mode != IEEE80211A");
		return 0;
	}

	/* Seems cf1 and chan_width is enough here */
	switch (chan_width) {
	case CHAN_WIDTH_20_NOHT:
	case CHAN_WIDTH_20:
		n_chans = 1;
		if (frequency == 0)
			frequency = cf1;
		break;
	case CHAN_WIDTH_40:
		n_chans = 2;
		frequency = cf1 - 10;
		break;
	case CHAN_WIDTH_80:
		n_chans = 4;
		frequency = cf1 - 30;
		break;
	case CHAN_WIDTH_80P80:
		n_chans = 4;
		frequency = cf1 - 30;
		frequency2 = cf2 - 30;
		break;
	case CHAN_WIDTH_160:
		n_chans = 8;
		frequency = cf1 - 70;
		break;
	default:
		wpa_printf(MSG_INFO, "DFS chan_width %d not supported",
			   chan_width);
		break;
	}

	wpa_printf(MSG_DEBUG, "DFS freq: %dMHz, n_chans: %d", frequency,
		   n_chans);
	for (i = 0; i < n_chans; i++) {
		if (rbm) {
			if (rbm & 1 << i)
				ret += set_dfs_state_freq(iface, frequency, state);
		}
		else {
			ret += set_dfs_state_freq(iface, frequency, state);
		}
		frequency = frequency + 20;

		if (chan_width == CHAN_WIDTH_80P80) {
			ret += set_dfs_state_freq(iface, frequency2, state);
			frequency2 = frequency2 + 20;
		}
	}

	return ret;
}


static int dfs_are_channels_overlapped(struct hostapd_iface *iface, int freq,
				       int chan_width, int cf1, int cf2)
{
	int start_chan_idx, start_chan_idx1;
	struct hostapd_hw_modes *mode;
	struct hostapd_channel_data *chan;
	int n_chans, n_chans1, i, j, frequency = freq, radar_n_chans = 1;
	u8 radar_chan;
	int res = 0;

	/* Our configuration */
	mode = iface->current_mode;
	start_chan_idx = dfs_get_start_chan_idx(iface, &start_chan_idx1);
	n_chans = dfs_get_used_n_chans(iface, &n_chans1);

	/* Check we are on DFS channel(s) */
	if (!dfs_check_chans_radar(iface, start_chan_idx, n_chans))
		return 0;

	/* Reported via radar event */
	switch (chan_width) {
	case CHAN_WIDTH_20_NOHT:
	case CHAN_WIDTH_20:
		radar_n_chans = 1;
		if (frequency == 0)
			frequency = cf1;
		break;
	case CHAN_WIDTH_40:
		radar_n_chans = 2;
		frequency = cf1 - 10;
		break;
	case CHAN_WIDTH_80:
		radar_n_chans = 4;
		frequency = cf1 - 30;
		break;
	case CHAN_WIDTH_160:
		radar_n_chans = 8;
		frequency = cf1 - 70;
		break;
	default:
		wpa_printf(MSG_INFO, "DFS chan_width %d not supported",
			   chan_width);
		break;
	}

	ieee80211_freq_to_chan(frequency, &radar_chan);

	for (i = 0; i < n_chans; i++) {
		chan = &mode->channels[start_chan_idx + i];
		if (!(chan->flag & HOSTAPD_CHAN_RADAR))
			continue;
		for (j = 0; j < radar_n_chans; j++) {
			wpa_printf(MSG_DEBUG, "checking our: %d, radar: %d",
				   chan->chan, radar_chan + j * 4);
			if (chan->chan == radar_chan + j * 4)
				res++;
		}
	}

	wpa_printf(MSG_DEBUG, "overlapped: %d", res);

	return res;
}


static unsigned int dfs_get_cac_time(struct hostapd_iface *iface,
				     int start_chan_idx, int n_chans)
{
	struct hostapd_channel_data *channel;
	struct hostapd_hw_modes *mode;
	int i;
	unsigned int cac_time_ms = 0;

	mode = iface->current_mode;

	for (i = 0; i < n_chans; i++) {
		channel = &mode->channels[start_chan_idx + i];
		if (!(channel->flag & HOSTAPD_CHAN_RADAR))
			continue;
		if (channel->dfs_cac_ms > cac_time_ms)
			cac_time_ms = channel->dfs_cac_ms;
	}

	return cac_time_ms;
}

void hostapd_dfs_debug_set_chan_dfs_state (struct hostapd_iface *iface, int state,
					   int start_freq, int n_chans)
{
	int i, cur_freq, kernel_state;
	struct hostapd_channel_data* channel;

	switch (state) {
	case HOSTAPD_CHAN_DFS_USABLE:
		kernel_state = NL80211_DFS_USABLE;
		break;
	case HOSTAPD_CHAN_DFS_UNAVAILABLE:
		kernel_state = NL80211_DFS_UNAVAILABLE;
		break;
	case HOSTAPD_CHAN_DFS_AVAILABLE:
		kernel_state = NL80211_DFS_AVAILABLE;
		break;
	default:
		wpa_printf(MSG_ERROR, "DFS_DEBUG: Requested invalid DFS state %d", state);
		return;
	}

	for (i = 0; i < n_chans; i++) {
		cur_freq = start_freq + i * 20;

		channel = hostapd_hw_get_channel_data(iface, cur_freq);
		if (!channel) {
			wpa_printf(MSG_ERROR, "DFS_DEBUG: Channel %d not found", cur_freq);
			continue;
		}

		if (channel->flag & HOSTAPD_CHAN_DISABLED)
			continue;

		if (!(channel->flag & HOSTAPD_CHAN_RADAR))
			continue;

		wpa_printf(MSG_DEBUG, "DFS_DEBUG: Update channel %d to DFS state 0x%x", cur_freq, state);
		if (hostapd_set_chan_dfs_state(iface, kernel_state, cur_freq, 0, 1))
			wpa_printf(MSG_ERROR, "DFS_DEBUG: Update kernel DFS state to 0x%x for channel %d failed",
				   state, cur_freq);

		if (set_dfs_state_freq(iface, cur_freq, state) != 1)
			wpa_printf(MSG_ERROR, "DFS_DEBUG: Update hostapd DFS state to 0x%x for channel %d failed",
				   state, cur_freq);
	}

}

/*
 * Main DFS handler
 * 1 - continue channel/ap setup
 * 0 - channel/ap setup will be continued after CAC
 * -1 - hit critical error
 */
int hostapd_handle_dfs(struct hostapd_iface *iface)
{
	struct hostapd_channel_data *channel;
	int res, n_chans, n_chans1, start_chan_idx, start_chan_idx1;
	int skip_radar = 0;
	u8 width = 0, seg0_idx = 0, seg1_idx = 0;

	if (is_6ghz_freq(iface->freq))
		return 1;

	if (!iface->current_mode) {
		/*
		 * This can happen with drivers that do not provide mode
		 * information and as such, cannot really use hostapd for DFS.
		 */
		wpa_printf(MSG_DEBUG,
			   "DFS: No current_mode information - assume no need to perform DFS operations by hostapd");
		return 1;
	}

	iface->cac_started = 0;

	do {
		/* Get start (first) channel for current configuration */
		start_chan_idx = dfs_get_start_chan_idx(iface,
							&start_chan_idx1);
		if (start_chan_idx == -1)
			return -1;

		/* Get number of used channels, depend on width */
		n_chans = dfs_get_used_n_chans(iface, &n_chans1);

		/* Setup CAC time */
		iface->dfs_cac_ms = dfs_get_cac_time(iface, start_chan_idx,
						     n_chans);

		/* Check if any of configured channels require DFS */
		res = dfs_check_chans_radar(iface, start_chan_idx, n_chans);
		wpa_printf(MSG_DEBUG,
			   "DFS %d channels required radar detection",
			   res);
		if (!res)
			return 1;

		/* Check if all channels are DFS available */
		res = dfs_check_chans_available(iface, start_chan_idx, n_chans);
		wpa_printf(MSG_DEBUG,
			   "DFS all channels available, (SKIP CAC): %s",
			   res ? "yes" : "no");
		if (res)
			return 1;

		/* Check if any of configured channels is unavailable */
		res = dfs_check_chans_unavailable(iface, start_chan_idx,
						  n_chans);

		/* For radar simulation */
		/* Allow to switch to the unavailable channel for dfs debug */
		if (res && iface->conf->dfs_debug_chan) {
			int low_freq = iface->current_mode->channels[start_chan_idx].freq;
			wpa_printf(MSG_INFO, "DFS_DEBUG: Forcing to stay on the channel %u", iface->conf->channel);
			hostapd_dfs_debug_set_chan_dfs_state(iface, HOSTAPD_CHAN_DFS_USABLE, low_freq, n_chans);
			res = 0;
		}

		wpa_printf(MSG_DEBUG, "DFS %d chans unavailable - choose other channel: %s",
			   res, res ? "yes": "no");
		if (res) {
			int sec = 0;
			u8 cf1 = 0, cf2 = 0;

			channel = dfs_get_valid_channel(iface, &sec, &cf1, &cf2,
							skip_radar);
			if (!channel) {
				wpa_printf(MSG_ERROR, "could not get valid channel");
				hostapd_set_state(iface, HAPD_IFACE_DFS);
				return 0;
			}

			iface->freq = channel->freq;
			iface->conf->channel = channel->chan;
			iface->conf->secondary_channel = sec;
			hostapd_set_oper_centr_freq_seg0_idx(iface->conf, cf1);
			hostapd_set_oper_centr_freq_seg1_idx(iface->conf, cf2);
		}
	} while (res);

	if ((iface->state == HAPD_IFACE_ENABLED) ||
	    (iface->set_freq_done && (iface->state == HAPD_IFACE_ACS))) {
		/* The Kernel does not allow to perform CAC from a running (beaconning)
		 * interface, and thus if we continue to start the CAC, we will fail.
		 * Instead, Disable the AP, and restart it on the new channel. */

		wpa_printf(MSG_DEBUG, "Hostapd is running, disabling it for CAC");
		iface->whm_if_fail = false;
		if (hostapd_disable_iface(iface) < 0) {
			wpa_printf(MSG_ERROR, "Disabling of interface failed");
			return -1;
		}

		if (hostapd_enable_iface(iface) < 0) {
			wpa_printf(MSG_ERROR, "Enabling of interface failed");
			return -1;
		}

		return 0;
	}

	/* Finally start CAC */
	hostapd_set_state(iface, HAPD_IFACE_DFS);
	wpa_printf(MSG_DEBUG, "DFS start CAC on %d MHz", iface->freq);

	width = hostapd_get_oper_chwidth(iface->conf);
	seg0_idx =  hostapd_get_oper_centr_freq_seg0_idx(iface->conf);
	seg1_idx = hostapd_get_oper_centr_freq_seg1_idx(iface->conf);

	wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, DFS_EVENT_CAC_START
		"freq=%d chan=%d sec_chan=%d, width=%d, seg0=%d, seg1=%d, cac_time=%ds",
		iface->freq,
		iface->conf->channel, iface->conf->secondary_channel,
		width,
		seg0_idx,
		seg1_idx,
		iface->dfs_cac_ms / 1000);

	res = hostapd_start_dfs_cac(
		iface, iface->conf->hw_mode, iface->freq, iface->conf->channel,
		iface->conf->ieee80211n, iface->conf->ieee80211ac,
		iface->conf->ieee80211ax,
		iface->conf->secondary_channel,
		width,
		seg0_idx,
		seg1_idx);

	if (res) {
		wpa_printf(MSG_ERROR, "DFS start_dfs_cac() failed, %d", res);
		return -1;
	}

	return 0;
}

static int chan_width_enum_to_freq(enum chan_width width) {
	switch (width) {
	case CHAN_WIDTH_20_NOHT:
	case CHAN_WIDTH_20:
		return 20;
	case CHAN_WIDTH_40:
		return 40;
	case CHAN_WIDTH_80:
	case CHAN_WIDTH_80P80:
		return 80;
	case CHAN_WIDTH_160:
		return 160;
	default:
		return 0;
	}
}

static int hostapd_config_dfs_chan_available(struct hostapd_iface *iface)
{
	int n_chans, n_chans1, start_chan_idx, start_chan_idx1;

	/* Get the start (first) channel for current configuration */
	start_chan_idx = dfs_get_start_chan_idx(iface, &start_chan_idx1);
	if (start_chan_idx < 0)
		return 0;

	/* Get the number of used channels, depending on width */
	n_chans = dfs_get_used_n_chans(iface, &n_chans1);

	/* Check if all channels are DFS available */
	return dfs_check_chans_available(iface, start_chan_idx, n_chans);
}

/* Implemented to catch one known rare racing, when radar detected immediately
 * after CAC Finished. (beacon set failed) In this case try to restart iface internally */
static void
hostapd_setup_iface_complete_restart_on_error (struct hostapd_iface *iface)
{
	struct hostapd_channel_data *channel = NULL;
	int secondary_channel;
	u8 seg0_idx;
	u8 seg1_idx;
	int res;
	size_t tmp_terminate_on_error = iface->interfaces->terminate_on_error;

	iface->interfaces->terminate_on_error = 0;
	res = hostapd_setup_interface_complete(iface, 0);
	iface->interfaces->terminate_on_error = tmp_terminate_on_error;

	if (!res)
		return;

	channel = dfs_get_valid_channel(iface, &secondary_channel,
		&seg0_idx,
		&seg1_idx,
		1);

	if (!channel) {
		wpa_printf(MSG_ERROR, "No Valid non-DFS channel available");
		eloop_terminate();
		return;
	}

	iface->freq = channel->freq;
	iface->conf->channel = channel->chan;
	iface->conf->secondary_channel = secondary_channel;
	hostapd_set_oper_centr_freq_seg0_idx(iface->conf, seg0_idx);
	hostapd_set_oper_centr_freq_seg1_idx(iface->conf, seg1_idx);

	hostapd_disable_iface(iface);
	hostapd_enable_iface(iface);
}

static int hostapd_dfs_channel_state_file_update(struct hostapd_iface *iface, int update_time);
static int hostapd_sb_dfs_can_switch_to_full_bw (struct hostapd_iface *iface);
static int hostapd_sub_dfs_switch_chan (struct hostapd_iface *iface, u8 rbm, int chan,
					int bandwidth, int secondary_offset);

int hostapd_dfs_complete_cac(struct hostapd_iface *iface, enum hostapd_cac_finished_stat cac_status,
			     int freq, int ht_enabled, int chan_offset,
			     int chan_width, int cf1, int cf2)
{
	if (iface->conf->dfs_debug_chan)
		iface->dbg_dfs_in_cac = 0;

	wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, DFS_EVENT_CAC_COMPLETED
		"cac_status=%d freq=%d ht_enabled=%d chan_offset=%d chan_width=%d cf1=%d cf2=%d"
		" timeout=%d",
		cac_status, freq, ht_enabled, chan_offset, chan_width, cf1, cf2,
		iface->dfs_cac_ms / 1000);

	if (cac_status == HOSTAPD_CAC_STAT_SUCCESS) {
		/* Complete iface/ap configuration */
		if (iface->drv_flags & WPA_DRIVER_FLAGS_DFS_OFFLOAD) {
			/* Complete AP configuration for the first bring up. */
			if (iface->state != HAPD_IFACE_ENABLED)
				hostapd_setup_interface_complete(iface, 0);
			else
				iface->cac_started = 0;
		} else {
			set_dfs_state(iface, freq, ht_enabled, chan_offset,
				      chan_width, cf1, cf2, 0,
				      HOSTAPD_CHAN_DFS_AVAILABLE);
			/*
			 * Just mark the channel available when CAC completion
			 * event is received in enabled state. CAC result could
			 * have been propagated from another radio having the
			 * same regulatory configuration. When CAC completion is
			 * received during non-HAPD_IFACE_ENABLED state, make
			 * sure the configured channel is available because this
			 * CAC completion event could have been propagated from
			 * another radio.
			 */

			if (iface->conf->dfs_channels_state_file && (iface->dfs_domain == HOSTAPD_DFS_REGION_ETSI)) {
				hostapd_dfs_channel_state_file_update(iface, 0);
			}
#ifdef CONFIG_ACS
			if (iface->conf->acs_init_done)
				acs_update_radar(iface);
#endif
			if (iface->cac_started) {
				if (hostapd_config_dfs_chan_available(iface) && !iface->block_tx) {
					iface->cac_started = 0;
					hostapd_setup_iface_complete_restart_on_error(iface);
				}
			}
			else if (iface->sb_dfs_cntr) {
				if (hostapd_sb_dfs_can_switch_to_full_bw(iface))
					hostapd_sub_dfs_switch_chan(iface, 0, 0, 0, 0);
			}
		}
	} else if (cac_status == HOSTAPD_CAC_STAT_PAUSED) {
		/* CAC was paused for some reason (probably becuase user
		 * initiated a scan request via iw). Restart CAC once scan is
		 * done. */
		wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, "CAC paused, restart CAC when scan is done");
		iface->cac_required = 1;
	}

	return 0;
}


int hostapd_dfs_pre_cac_expired(struct hostapd_iface *iface, int freq,
				int ht_enabled, int chan_offset, int chan_width,
				int cf1, int cf2)
{
	wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, DFS_EVENT_PRE_CAC_EXPIRED
		"freq=%d ht_enabled=%d chan_offset=%d chan_width=%d cf1=%d cf2=%d",
		freq, ht_enabled, chan_offset, chan_width, cf1, cf2);

	/* Proceed only if DFS is not offloaded to the driver */
	if (iface->drv_flags & WPA_DRIVER_FLAGS_DFS_OFFLOAD)
		return 0;

	set_dfs_state(iface, freq, ht_enabled, chan_offset, chan_width,
		      cf1, cf2, 0, HOSTAPD_CHAN_DFS_USABLE);

	return 0;
}

struct hostapd_channel_data *
dfs_get_debug_dfs_chan(struct hostapd_iface *iface, int *secondary_channel,
						u8 *seg0_idx,  u8 *seg1_idx)
{
	int i, found = 0;
	struct hostapd_channel_data *channel;
	struct hostapd_hw_modes *mode = iface->current_mode;
	u8 channel_bak, seg0_idx_bak, seg1_idx_bak;
	int secondary_channel_bak;

	/* hostapd_is_dfs_required checks if DFS is required for current channel, but
	   we need to check it for dfs_debug_chan. Therefore need to backup, check
	   and restore. */
	channel_bak = iface->conf->channel;
	secondary_channel_bak = iface->conf->secondary_channel;
	seg0_idx_bak = hostapd_get_oper_centr_freq_seg0_idx(iface->conf);
	seg1_idx_bak = hostapd_get_oper_centr_freq_seg1_idx(iface->conf);

	for (i = 0; i < mode->num_channels; i++) {
		channel = &mode->channels[i];

		if (channel->chan == iface->conf->dfs_debug_chan) {
			wpa_printf(MSG_INFO, "DFS_DEBUG: Selected ch. #%d", channel->chan);
			found = 1;
			break;
		}
	}

	if (!found) {
		wpa_printf(MSG_INFO, "DFS_DEBUG: Did not find ch. #%d", iface->conf->dfs_debug_chan);
		return NULL;
	}

	/* Set the same channel bandwidth as configured in conf file */
	if (dfs_adjust_debug_dfs_chan(iface, channel, secondary_channel, seg0_idx)) {
		wpa_printf(MSG_ERROR, "DFS_DEBUG: Failed to adjust channel %d parameters", channel->chan);
		return NULL;
	}

	/* Not supported */
	*seg1_idx = 0;

	iface->conf->channel = iface->conf->dfs_debug_chan;
	iface->conf->secondary_channel = *secondary_channel;
	hostapd_set_oper_centr_freq_seg0_idx(iface->conf, *seg0_idx);
	hostapd_set_oper_centr_freq_seg1_idx(iface->conf, *seg1_idx);

	iface->conf->dfs_dbg_need_cac = hostapd_is_dfs_required(iface) > 0 ? true :
		false;

	/* restore original channel */
	iface->conf->channel = channel_bak;
	iface->conf->secondary_channel = secondary_channel_bak;
	hostapd_set_oper_centr_freq_seg0_idx(iface->conf,seg0_idx_bak);
	hostapd_set_oper_centr_freq_seg1_idx(iface->conf,seg1_idx_bak);

	return channel;
}

void static hostapd_handle_acs_dfs_failure(struct hostapd_iface *iface)
{
	/* WHM to trigger if CH_SWTICH happens in case of RADAR detected.
	 * and selects a lower bandwidth (< 80MHz)
	 */
	u8 ch_width = get_num_width(hostapd_get_oper_chwidth(iface->conf), iface->conf->secondary_channel);
	if (ch_width < CHWIDTH_80)
	{
		wpa_printf(MSG_ERROR, "WHM-ACS: RADAR detcted, Channel Switch to channel %d and width %d ", iface->conf->channel, ch_width);
		hostapd_handle_acs_whm_warning(iface);
	}
}

int hostapd_dfs_start_channel_switch_cac(struct hostapd_iface *iface)
{
	struct hostapd_channel_data *channel = NULL;
	int secondary_channel;
	u8 oper_centr_freq_seg0_idx = 0;
	u8 oper_centr_freq_seg1_idx = 0;
	int skip_radar = 0;
	int err = 1;
	struct csa_settings settings;

	memset(&settings, 0, sizeof(settings));
	/* Radar detected during active CAC */
	iface->cac_started = 0;
	if (iface->conf->dfs_debug_chan) {
		channel = dfs_get_debug_dfs_chan(iface, &secondary_channel,
						&oper_centr_freq_seg0_idx,
						&oper_centr_freq_seg1_idx);
	}
	else if (dfs_is_failsafe_enabled(iface)) {
		/* switch to failsafe channel */
		channel = dfs_get_valid_channel_failsafe(iface,
							 &settings.freq_params,
							 &secondary_channel,
							 &oper_centr_freq_seg0_idx,
							 &oper_centr_freq_seg1_idx);
		if (!channel) {
			wpa_printf(MSG_ERROR, "Failed to select failsafe channel");
		}
	}

	if (!channel)
		channel = dfs_get_valid_channel(iface, &secondary_channel,
						&oper_centr_freq_seg0_idx,
						&oper_centr_freq_seg1_idx,
						skip_radar);

	if (!channel) {
		int n_chans1;
		/* if channel width is more than 20 MHz, cut it by half and try
		 * to find a channel */
		if (dfs_get_used_n_chans(iface, &n_chans1) > 1) {
			dfs_cut_cw_by_half(iface, false);
			return hostapd_dfs_start_channel_switch_cac(iface);
		}
		wpa_printf(MSG_ERROR, "No valid channel available");
		hostapd_disable_iface(iface);
		return err;
	}

	wpa_printf(MSG_DEBUG, "DFS will switch to a new channel %d",
		   channel->chan);
	wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, DFS_EVENT_NEW_CHANNEL
		"freq=%d chan=%d sec_chan=%d", channel->freq,
		channel->chan, secondary_channel);

	iface->freq = channel->freq;
	iface->conf->channel = channel->chan;
	iface->conf->secondary_channel = secondary_channel;
	hostapd_set_oper_centr_freq_seg0_idx(iface->conf, oper_centr_freq_seg0_idx);
	hostapd_set_oper_centr_freq_seg1_idx(iface->conf, oper_centr_freq_seg1_idx);

	err = 0;
	hostapd_channel_switch_reason_set(iface, HAPD_CHAN_SWITCH_RADAR_DETECTED);
	hostapd_setup_interface_complete(iface, err);

	/* WHM to trigger if CH_SWTICH happens in case of RADAR detected.
	 * and selects a lower bandwidth (< 80MHz)
	 */
	hostapd_handle_acs_dfs_failure(iface);

	return err;
}


int hostapd_dfs_start_channel_switch(struct hostapd_iface *iface)
{
	struct hostapd_channel_data *channel = NULL;
	int secondary_channel;
	u8 oper_centr_freq_seg0_idx;
	u8 oper_centr_freq_seg1_idx;
	int skip_radar = 1;
	struct csa_settings csa_settings;
	unsigned int i;
	int err = 1;
	int failsafe = 0;
	int dfs_debug = 0;

	wpa_printf(MSG_DEBUG, "%s called (CAC active: %s, CSA active: %s)",
		   __func__, iface->cac_started ? "yes" : "no",
		   hostapd_csa_in_progress(iface) ? "yes" : "no");

	/* Check if CSA in progress */
	if (hostapd_csa_in_progress(iface))
		return 0;

	/* Check if active CAC */
	if (iface->cac_started)
		return hostapd_dfs_start_channel_switch_cac(iface);

	/*
	 * Allow selection of DFS channel in ETSI to comply with
	 * uniform spreading.
	 */
	if ((iface->dfs_domain == HOSTAPD_DFS_REGION_ETSI) && (!iface->conf->dynamic_failsafe))
		skip_radar = 0;

	os_memset(&csa_settings, 0, sizeof(csa_settings));

	/* For radar simulation */
	if (iface->conf->dfs_debug_chan) {
		channel = dfs_get_debug_dfs_chan(iface, &secondary_channel,
			                         &oper_centr_freq_seg0_idx,
						 &oper_centr_freq_seg1_idx);
		if (channel) {
			dfs_debug = 1;
			goto switch_chan;
		}
	}

	if (dfs_is_failsafe_enabled(iface)) {
		/* switch to failsafe channel */
		channel = dfs_get_valid_channel_failsafe(iface,
							 &csa_settings.freq_params,
							 &secondary_channel,
							 &oper_centr_freq_seg0_idx,
							 &oper_centr_freq_seg1_idx);
		if (channel)
			failsafe = 1;
		else
			wpa_printf(MSG_ERROR, "Failed to select failsafe channel");
	}

	if (!channel) {
		/* Perform channel switch/CSA */
		channel = dfs_get_valid_channel(iface, &secondary_channel,
						&oper_centr_freq_seg0_idx,
						&oper_centr_freq_seg1_idx,
						skip_radar);
	}

	if (!channel) {
		int n_chans1;

		/* if channel width is more than 20 MHz, cut it by half and try
		 * to find a channel */
		if (dfs_get_used_n_chans(iface, &n_chans1) > 1) {
			dfs_cut_cw_by_half(iface, true);
			return hostapd_dfs_start_channel_switch(iface);
		}

		dfs_restore_original_cw(iface);

		/*
		 * If there is no channel to switch immediately to, check if
		 * there is another channel where we can switch even if it
		 * requires to perform a CAC first.
		 */
		skip_radar = 0;
		channel = dfs_get_valid_channel(iface, &secondary_channel,
						&oper_centr_freq_seg0_idx,
						&oper_centr_freq_seg1_idx,
						skip_radar);
		if (!channel) {
			wpa_printf(MSG_INFO,
				   "%s: no DFS channels left, waiting for NOP to finish",
				   __func__);
			hostapd_disable_iface(iface);
			return err;
		}

		iface->freq = channel->freq;
		iface->conf->channel = channel->chan;
		iface->conf->secondary_channel = secondary_channel;
		hostapd_set_oper_centr_freq_seg0_idx(iface->conf, oper_centr_freq_seg0_idx);
		hostapd_set_oper_centr_freq_seg1_idx(iface->conf, oper_centr_freq_seg1_idx);

		hostapd_disable_iface(iface);
		hostapd_enable_iface(iface);
		return 0;
	}

switch_chan:
	wpa_printf(MSG_DEBUG, "DFS will switch to a new channel %d",
		   channel->chan);
	wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, DFS_EVENT_NEW_CHANNEL
		"freq=%d chan=%d sec_chan=%d", channel->freq,
		channel->chan, secondary_channel);

	/* WHM to trigger if CH_SWTICH happens in case of RADAR detected.
	 * and selects a lower bandwidth (< 80MHz)
	 */
	hostapd_handle_acs_dfs_failure(iface);

	/* Setup CSA request */
	csa_settings.cs_count = hostapd_get_cs_count(iface);
	csa_settings.block_tx = 1;
	if (!failsafe) {
		err = hostapd_set_freq_params(&csa_settings.freq_params,
					      iface->conf->hw_mode,
					      channel->freq,
					      channel->chan,
					      iface->conf->enable_edmg,
					      iface->conf->edmg_channel,
					      iface->conf->ieee80211n,
					      iface->conf->ieee80211ac,
					      iface->conf->ieee80211ax,
					      secondary_channel,
					      hostapd_get_oper_chwidth(iface->conf),
					      oper_centr_freq_seg0_idx,
					      oper_centr_freq_seg1_idx,
					      iface->current_mode->vht_capab,
					      &iface->current_mode->he_capab);

		if (err) {
			wpa_printf(MSG_ERROR, "DFS failed to calculate CSA freq params");
			hostapd_disable_iface(iface);
			return err;
		}
	}

	if (dfs_debug) {
		int n_chans = csa_settings.freq_params.bandwidth / 20;
		int low_freq  = csa_settings.freq_params.center_freq1 - (csa_settings.freq_params.bandwidth / 2) + 10;
		hostapd_dfs_debug_set_chan_dfs_state(iface, HOSTAPD_CHAN_DFS_AVAILABLE, low_freq, n_chans);
	}

	if (failsafe && iface->failsafe.tx_ant > 0) {
		err = hostapd_drv_set_antenna(iface->bss[0],
					      iface->failsafe.tx_ant,
					      iface->failsafe.rx_ant);
		if (err < 0) {
			wpa_printf(MSG_ERROR, "hostapd_dfs_start_channel_switch: setting antenna "
					      "failed: %s", strerror(errno));
		}
	}

	/* If channel width was reduced, restore original value, otherwise channel
	 * switch will be refused. The new channel settings we need to switch to
	 * are stored in csa_settings structure and are not modified by this call. */
	if (iface->conf->cw_reduced)
		dfs_restore_original_cw(iface);

	err = hostapd_prepare_and_send_csa_deauth_cfg_to_driver(iface->bss[0]);
	if (err) {
		wpa_printf(MSG_ERROR, "hostapd_prepare_and_send_csa_deauth_cfg_to_driver failed: %s",
					iface->bss[0]->conf->iface);
		return err;
	}

	hostapd_channel_switch_reason_set(iface, HAPD_CHAN_SWITCH_RADAR_DETECTED);
	for (i = 0; i < iface->num_bss; i++) {
		err = hostapd_switch_channel(iface->bss[i], &csa_settings);
		if (err)
			break;
	}

	if (err) {
		wpa_printf(MSG_WARNING, "DFS failed to schedule CSA (%d) - trying fallback",
			   err);
		iface->freq = channel->freq;
		iface->conf->channel = channel->chan;
		iface->conf->secondary_channel = secondary_channel;
		hostapd_set_oper_centr_freq_seg0_idx(iface->conf, oper_centr_freq_seg0_idx);
		hostapd_set_oper_centr_freq_seg1_idx(iface->conf, oper_centr_freq_seg1_idx);

		hostapd_disable_iface(iface);
		hostapd_enable_iface(iface);
		return 0;
	}

	/* Channel configuration will be updated once CSA completes and
	 * ch_switch_notify event is received */

	wpa_printf(MSG_DEBUG, "DFS waiting channel switch event");
	return 0;
}

/* Re-start CAC in case it was paused */
void hostapd_dfs_cac_restart(struct hostapd_iface *iface)
{

	int res, start_chan_idx, start_chan_idx1, n_chans, n_chans1;

	/* If scan was initiated during CAC and radar was detected on the
	 * operating ch during the scan CAC is no longer required */
	if (iface->state != HAPD_IFACE_DFS || !iface->cac_started) {
		wpa_printf(MSG_DEBUG, "CAC restart is not required, iface state=%d; cac_started=%d",
			   iface->state, iface->cac_started);
		return;
	}

	n_chans = dfs_get_used_n_chans(iface, &n_chans1);

	start_chan_idx = dfs_get_start_chan_idx(iface,
						&start_chan_idx1);
	if (start_chan_idx == -1) {
		wpa_printf(MSG_ERROR, "Error getting configured CAC channels");
		iface->cac_started = 0;
		return;
	}

	/* Check if any of the configured channels is unavailable (state may
	 * have changed during the scan that caused the CAC to pause)
	 * If so, CAC can't be restarted. Need to select a new channel
	 * TODO: handle SB-DFS */
	if (dfs_check_chans_unavailable(iface, start_chan_idx, n_chans)) {
		wpa_printf(MSG_DEBUG, "CAC won't be restarted, some channels are unavailable");
#ifdef CONFIG_ACS
		if (!dfs_is_failsafe_enabled(iface) && iface->conf->acs_init_done && !iface->conf->dfs_debug_chan)
			acs_radar_switch(iface);
		else
#endif
			hostapd_dfs_start_channel_switch(iface);
		return;
	}

	wpa_printf(MSG_DEBUG, "DFS re-start CAC on %d MHz", iface->freq);
	wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, DFS_EVENT_CAC_START
		"freq=%d chan=%d sec_chan=%d, width=%d, seg0=%d, seg1=%d, cac_time=%ds",
		iface->freq,
		iface->conf->channel, iface->conf->secondary_channel,
		hostapd_get_oper_chwidth(iface->conf),
		hostapd_get_oper_centr_freq_seg0_idx(iface->conf),
		hostapd_get_oper_centr_freq_seg1_idx(iface->conf),
		iface->dfs_cac_ms / 1000);

	res = hostapd_start_dfs_cac(iface, iface->conf->hw_mode,
				    iface->freq,
				    iface->conf->channel,
				    iface->conf->ieee80211n,
				    iface->conf->ieee80211ac,
				    iface->conf->ieee80211ax,
				    iface->conf->secondary_channel,
				    hostapd_get_oper_chwidth(iface->conf),
				    hostapd_get_oper_centr_freq_seg0_idx(iface->conf),
				    hostapd_get_oper_centr_freq_seg1_idx(iface->conf));

	if (res) {
		wpa_printf(MSG_ERROR, "Can't restart CAC");
	}
}

void hostapd_send_dfs_debug_channel_flag(struct hostapd_iface *iface)
{
	int i;
	u8 dfs_required = 0;
	struct hostapd_channel_data *channel;
	struct hostapd_data *hapd = iface->bss[0];
	struct hostapd_hw_modes *mode = iface->current_mode;

	if (!mode || !hapd) {
		wpa_printf(MSG_ERROR, "HW mode or hostapd data not initialized, dfs debug channel flag won't be sent");
		return;
	}

	/* dfs_required flag should be set, if there is any DFS channel in HW mode,
	 * because bandwidth may change and if it wouldn't be required at lower BW,
	 * it may be required at higher BW and we don't know the highest BW
	 * supported. */
	if (iface->conf->dfs_debug_chan) {
		for (i = 0; i < mode->num_channels; i++) {
			channel = &mode->channels[i];
			if (channel->flag & HOSTAPD_CHAN_RADAR) {
				dfs_required = 1;
				break;
			}
		}
	}

	if (hostapd_drv_vendor_cmd(hapd, OUI_LTQ, LTQ_NL80211_VENDOR_SUBCMD_DFS_DEBUG,
							   &dfs_required, sizeof(dfs_required), NULL))
		wpa_printf(MSG_INFO, "Failed to send driver vendor command LTQ_NL80211_VENDOR_SUBCMD_DFS_DEBUG");
}

static int hostapd_dfs_channel_state_file_update(struct hostapd_iface *iface, int update_time)
{
	struct hostapd_channel_data *channel;
	int i;
	struct hostapd_hw_modes *mode = iface->current_mode;
	FILE *f;
	int keep_going = 0;

	if (!iface->conf->dfs_channels_state_file)
		return 0;

	f = fopen(iface->conf->dfs_channels_state_file, "w");
	if (!f) {
		wpa_printf(MSG_ERROR, "dfs_channels_state_file file '%s' not writable.", iface->conf->dfs_channels_state_file);
		os_free(iface->conf->dfs_channels_state_file);
		iface->conf->dfs_channels_state_file = NULL;
		return 0;
	}

	fprintf(f,"#state=0(USABLE),1(AVAILABLE),2(UNAVAILABLE)\n");

	for (i = 0; i < mode->num_channels; i++) {
		channel = &mode->channels[i];
		if (channel->flag & HOSTAPD_CHAN_RADAR && is_in_chanlist(iface, channel)) {
			if (((channel->flag & HOSTAPD_CHAN_DFS_MASK) == HOSTAPD_CHAN_DFS_AVAILABLE) &&
					(iface->dfs_domain == HOSTAPD_DFS_REGION_ETSI)) {
				fprintf(f,"channel=%d,state=1,nop_time=0\n", channel->chan);
			}
			if ((channel->flag & HOSTAPD_CHAN_DFS_MASK) == HOSTAPD_CHAN_DFS_UNAVAILABLE) {
				if (channel->nop_time_passed_min < 30) {
					if (update_time) channel->nop_time_passed_min++;
					keep_going = 1;
					fprintf(f,"channel=%d,state=2,nop_time=%d\n", channel->chan, channel->nop_time_passed_min);
				}
			}
		}
	}
	fclose(f);
	return keep_going;
}


static void hostapd_dfs_channel_state_file_update_eloop(void *eloop_ctx, void *timeout_ctx)
{
	struct hostapd_iface *iface = eloop_ctx;
	if (hostapd_dfs_channel_state_file_update(iface, 1))
		eloop_register_timeout(60, 0, hostapd_dfs_channel_state_file_update_eloop, iface, NULL);
}

static void hostapd_dfs_channel_state_file_parse(struct hostapd_iface *iface)
{
	struct hostapd_channel_data *ch;
	struct hostapd_hw_modes *mode = iface->current_mode;
	FILE *f;
	int line = 0;
	char buf[128], *pos;
	int channel, state, nop_time;
	f = fopen(iface->conf->dfs_channels_state_file, "r");
	if (!f) {
	/* no file yet, will be created later on*/
		return;
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
		if (sscanf(buf, "channel=%d,state=%d,nop_time=%d", &channel, &state, &nop_time) != 3) {
			wpa_printf(MSG_ERROR, "Invalid text at "
					   "line %d in '%s'", line, iface->conf->dfs_channels_state_file);
			continue;
		}
		ch = hw_get_channel_chan(mode, channel, NULL);
		if ( !ch || !is_in_chanlist(iface, ch)) {
			wpa_printf(MSG_ERROR, "Invalid channel at "
					   "line %d in '%s'", line, iface->conf->dfs_channels_state_file);
			continue;
		}
		if (state == 1) {
			if (iface->dfs_domain != HOSTAPD_DFS_REGION_ETSI) {
				wpa_printf(MSG_ERROR, "No support for available channels in non ETSI regions - at "
					   "line %d in '%s'", line, iface->conf->dfs_channels_state_file);
				continue;
			}
			if ((ch->flag & HOSTAPD_CHAN_DFS_MASK) == HOSTAPD_CHAN_DFS_UNAVAILABLE) {
			/* A radar was detected while hostapd was down */
				goto state2;
			}

			set_dfs_state_freq(iface, ch->freq, HOSTAPD_CHAN_DFS_AVAILABLE);
		}
		if (state == 2) {
			if (nop_time == 30) {
				/* nop ended */
				continue;
			}
state2:
			set_dfs_state_freq(iface, ch->freq, HOSTAPD_CHAN_DFS_UNAVAILABLE);
			if ((nop_time >= 0) && (nop_time < 30)) {
				ch->nop_time_passed_min = nop_time;
			}
			else
			{
				wpa_printf(MSG_ERROR, "Invalid nop_time at "
					   "line %d in '%s'", line, iface->conf->dfs_channels_state_file);
				continue;
			}
		}
	}

	fclose(f);
}

int hostapd_send_dfs_channel_state(struct hostapd_iface *iface)
{
	int i;
	int update = 0;
	struct hostapd_channel_data *channel;
	struct hostapd_data *hapd = iface->bss[0];
	struct hostapd_hw_modes *mode = iface->current_mode;

	if (!mode || !hapd) {
		wpa_printf(MSG_ERROR, "HW mode or hostapd data not initialized, dfs channels state won't be sent");
		return -1;
	}

	if (!iface->conf->dfs_channels_state_file)
		return 0;

	/* So we will update the kernel only once, in boot, and not in every disable-enable */
	if (!iface->boot) {
		hostapd_dfs_channel_state_file_parse(iface);

		for (i = 0; i < mode->num_channels; i++) {
			channel = &mode->channels[i];
			if (channel->flag & HOSTAPD_CHAN_RADAR) {
				if (((channel->flag & HOSTAPD_CHAN_DFS_MASK) == HOSTAPD_CHAN_DFS_AVAILABLE) &&
						(iface->dfs_domain == HOSTAPD_DFS_REGION_ETSI)) {
					hostapd_set_chan_dfs_state(iface,
								NL80211_DFS_AVAILABLE, channel->freq, 0, 0);
				}
				if ((channel->flag & HOSTAPD_CHAN_DFS_MASK) == HOSTAPD_CHAN_DFS_UNAVAILABLE) {
					hostapd_set_chan_dfs_state(iface,
								NL80211_DFS_UNAVAILABLE, channel->freq, channel->nop_time_passed_min, 0);
					update = 1;
				}
			}
		}
		iface->boot = true;
	}
	/* In case we had some radars while disabled */
	else {
		update = 1;
	}
	hostapd_dfs_channel_state_file_update(iface, 0);
	if (update) {
		/* Only register if not already registered - not needed otherwise */
		if (!eloop_is_timeout_registered(hostapd_dfs_channel_state_file_update_eloop, iface, NULL))
			eloop_register_timeout(60, 0, hostapd_dfs_channel_state_file_update_eloop, iface, NULL);
	}
	return 0;
}

#define NUM_MAX_CHANS_IN_CHANDEF 8

static int
hostapd_get_num_chans (struct hostapd_iface *iface, u8 vht_oper_chwidth, int secondary)
{
	int n_chans = 1;

	if (iface->conf->ieee80211n && secondary)
		n_chans = 2;

	if (iface->conf->ieee80211ac) {
		switch (vht_oper_chwidth) {
		case CHANWIDTH_USE_HT:
			break;
		case CHANWIDTH_80MHZ:
			n_chans = 4;
			break;
		case CHANWIDTH_160MHZ:
			n_chans = 8;
			break;
		case CHANWIDTH_80P80MHZ:
			n_chans = 4;
			break;
		default:
			break;
		}
	}

	return n_chans;
}

static int
hostapd_get_low_chan_idx (struct hostapd_iface *iface, u8 centr_freq_seg0_idx, u8 chwidth, u8 primary, int secondary)
{
	int i;
	u8 low_chan;
	int idx = -1;
	struct hostapd_channel_data *chan;
	struct hostapd_hw_modes *mode = iface->current_mode;

	if (!mode)
		return idx;

	if (iface->conf->ieee80211ac && chwidth >= CHANWIDTH_80MHZ) {
		low_chan = centr_freq_seg0_idx - get_num_width(chwidth, secondary)/2/5 + 2;
	}
	else {
		if (secondary >= 0)
			low_chan = primary;
		else
			low_chan = primary - 4;
	}

	for (i = 0; i < mode->num_channels; i++) {
		chan = &mode->channels[i];
		if (chan->chan == low_chan) {
			idx = i;
			break;
		}
	}

	return idx;
}

static void hostapd_sb_dfs_inc_cntr(struct hostapd_iface *iface, u8 rbm);

static bool
hostapd_sb_dfs_check_cntr (struct hostapd_iface *iface, u8 rbm)
{
	int cur_count = iface->sb_dfs_cntr;
	hostapd_sb_dfs_inc_cntr(iface, rbm);

	if ((iface->sb_dfs_cntr - 1) > iface->conf->sub_band_dfs) {
		iface->sb_dfs_cntr = cur_count;
		return true;
	}

	iface->sb_dfs_cntr = cur_count;
	return false;
}

static bool
hostapd_sub_dfs_curr_better_than_failsafe (struct hostapd_iface *iface)
{
#ifdef CONFIG_ACS
	u8 chan_failsafe_prim;
	int rank_current_idx, rank_failsafe_idx, chan_failsafe_sec_offs, failsafe_center_idx;
	int chan_current_bw = 20;

	if (iface->failsafe.freq == 0)
		return false;

	if (iface->conf->acs_algo != ACS_ALGO_SMART)
		return false;

	if (!iface->conf->acs_init_done)
		return false;

	if (HOSTAPD_MODE_IEEE80211A != ieee80211_freq_to_chan(iface->failsafe.freq, &chan_failsafe_prim))
		return false;

	if (iface->conf->secondary_channel)
		chan_current_bw = 40;

	if (hostapd_get_oper_chwidth(iface->conf) == CHANWIDTH_80MHZ)
		chan_current_bw = 80;
	else if (hostapd_get_oper_chwidth(iface->conf) == CHANWIDTH_160MHZ)
		chan_current_bw = 160;

	rank_current_idx = hostapd_acs_get_candidates_rank_idx(iface,
							       iface->conf->channel,
							       iface->conf->secondary_channel,
							       chan_current_bw);

	failsafe_center_idx = 36 + (iface->failsafe.center_freq1 - 5180) / 5;

	chan_failsafe_sec_offs = hostapd_get_secondary_offset(chan_failsafe_prim, iface->failsafe.bandwidth, failsafe_center_idx);
	rank_failsafe_idx = hostapd_acs_get_candidates_rank_idx(iface,
								chan_failsafe_prim,
								chan_failsafe_sec_offs,
								iface->failsafe.bandwidth);

	wpa_printf(MSG_DEBUG, "SB DFS: Rank indexes BW comparison: current %d, failsafe %d", rank_current_idx, rank_failsafe_idx);
	return hostapd_acs_compare_ranks(iface, rank_current_idx, rank_failsafe_idx);
#else
	return false;
#endif /* CONFIG_ACS */
}

static int
hostapd_sub_dfs_get_new_chan (struct hostapd_iface *iface, u8 rbm,
			      int *new_bw, int *secondary_offset)
{
	int i;
	u8 cons_no_radar = 0; /* Consecutive no radar channels */
	u8 prev_no_radar_idx = 0;
	u8 do_sb_switch = 0;
	u8 last_cons_chan_idx = 0;
	u8 max_cons_no_radar = 0;
	u8 max_last_cons_chan_idx = 0;
	int n_chans, n_chans1;
	struct hostapd_hw_modes *mode;
	u8 radar_vs_channel_map[NUM_MAX_CHANS_IN_CHANDEF][2] = {{0}};
	struct hostapd_channel_data *chan;
	int start_chan_idx, start_chan_idx1;

	mode = iface->current_mode;
	if (!mode)
		return 0;

	if (hostapd_csa_in_progress(iface)) {
		wpa_printf(MSG_INFO, "SB DFS: CSA already in progress, ignore");
		return -1;
	}

	if (iface->cac_started) {
		wpa_printf(MSG_INFO, "SB DFS: CAC in progress, will switch to new channel");
		return 0;
	}

	if (hostapd_sb_dfs_check_cntr(iface, rbm)) {
		wpa_printf(MSG_INFO, "SB DFS: Counter exceeded");

		if (hostapd_config_dfs_chan_available(iface) &&
		    hostapd_sub_dfs_curr_better_than_failsafe(iface)) {
			/* Sub band DFS will select the same channel */
			wpa_printf(MSG_INFO, "SB DFS: Staying on the same channel (failsafe rank is worse)");
		}
		else {
			return 0;
		}
	}

	if (iface->sb_dfs_cntr) {
		start_chan_idx = hostapd_get_low_chan_idx(iface, iface->sb_orig_chandef.vht_oper_centr_freq_seg0_idx,
							  iface->sb_orig_chandef.vht_oper_chwidth,
							  iface->sb_orig_chandef.channel,
							  iface->sb_orig_chandef.secondary_offset);
		n_chans = hostapd_get_num_chans(iface, iface->sb_orig_chandef.vht_oper_chwidth, iface->sb_orig_chandef.secondary_offset);
	}
	else {
		start_chan_idx = dfs_get_start_chan_idx(iface, &start_chan_idx1);
		n_chans = dfs_get_used_n_chans(iface, &n_chans1);
	}

	if (-1 == start_chan_idx) {
		wpa_printf(MSG_ERROR, "SB DFS: Cannot get start channel index");
		return 0;
	}

	wpa_printf(MSG_DEBUG, "SB DFS: n_chans %d, start idx %d", n_chans, start_chan_idx);

	for (i = 0; i < n_chans; i++) {
		chan = &mode->channels[start_chan_idx + i];
		radar_vs_channel_map[i][0] = chan->chan;

		if (!(chan->flag & HOSTAPD_CHAN_RADAR)) {
			do_sb_switch = 1;
			continue;
		}

		if ((chan->flag & HOSTAPD_CHAN_DFS_MASK) == HOSTAPD_CHAN_DFS_UNAVAILABLE)
			radar_vs_channel_map[i][1] = 1;
		else
			do_sb_switch = 1;
	}

	if (!do_sb_switch) /* All channels UNAVAILABLE */
		return 0;

	for (i = 0; i < n_chans; i++) {
		if (!radar_vs_channel_map[i][1]) {
			if (0 == cons_no_radar || (i == prev_no_radar_idx + 1)) {
				cons_no_radar++;
				last_cons_chan_idx = i;
			}
			prev_no_radar_idx = i;
		}
		else {
			if (cons_no_radar > max_cons_no_radar) {
				max_cons_no_radar = cons_no_radar;
				max_last_cons_chan_idx = last_cons_chan_idx;
			}
			cons_no_radar = 0;
		}
	}

	if (max_cons_no_radar > cons_no_radar) {
		cons_no_radar = max_cons_no_radar;
		last_cons_chan_idx = max_last_cons_chan_idx;
	}

	wpa_printf(MSG_DEBUG, "SB DFS: Consecutive no radar channels: %d", cons_no_radar);

	for (i = 0; i < n_chans; i++) {
		wpa_printf(MSG_DEBUG, "SB DFS: Chan[%i] = %d, radar = %d", i, radar_vs_channel_map[i][0], radar_vs_channel_map[i][1]);
	}

	if (cons_no_radar == n_chans) {
		wpa_printf(MSG_DEBUG, "SB DFS: All channels are available, wrong bit map provided?");
		return -1;
	}

	/* This sub band selection makes sure secondary channel offset will be 1 if not 20 MHz bw chosen */
	*secondary_offset = 1;
	switch (cons_no_radar) {
	case 1:
		*new_bw = CHAN_WIDTH_20;
		*secondary_offset = 0;
		return radar_vs_channel_map[last_cons_chan_idx][0];
	case 2:
		if (last_cons_chan_idx % 2) {
			*new_bw = CHAN_WIDTH_40;
			return radar_vs_channel_map[last_cons_chan_idx - 1][0];
		}
		else {
			*new_bw = CHAN_WIDTH_20;
			*secondary_offset = 0;
			return radar_vs_channel_map[last_cons_chan_idx - 1][0];
		}
	case 3:
		*new_bw = CHAN_WIDTH_40;
		if (last_cons_chan_idx % 2)
			return radar_vs_channel_map[last_cons_chan_idx - 1][0];
		else
			return radar_vs_channel_map[last_cons_chan_idx - 2][0];
	case 4:
	case 5:
	case 6:
	case 7:
		if (7 == last_cons_chan_idx) {
			*new_bw = CHAN_WIDTH_80;
			return radar_vs_channel_map[4][0];
		}
		else if ((cons_no_radar - 1) == last_cons_chan_idx) {
			*new_bw = CHAN_WIDTH_80;
			return radar_vs_channel_map[0][0];
		}
		else if (last_cons_chan_idx % 2) {
			*new_bw = CHAN_WIDTH_40;
			return radar_vs_channel_map[last_cons_chan_idx - 1][0];
		}
		else {
			*new_bw = CHAN_WIDTH_40;
			return radar_vs_channel_map[last_cons_chan_idx - 2][0];
		}
	case 8:
		wpa_printf(MSG_DEBUG, "SB DFS: Radar detected on non DFS channel?");
		return -1;
	default:
		return 0;
	}

	return 0;
}

int hostapd_bandwidth_to_nl_sb_dfs_bw (int bandwidth)
{
	switch (bandwidth) {
	case CHAN_WIDTH_20_NOHT:
	case CHAN_WIDTH_20:
		return NL80211_SB_DFS_BW_20;
	case CHAN_WIDTH_40:
		return NL80211_SB_DFS_BW_40;
	case CHAN_WIDTH_80:
		return NL80211_SB_DFS_BW_80;
	default:
		return -1;
	}
}

static int
hostapd_sb_dfs_is_on_adjacent (struct hostapd_iface *iface, u8 channel)
{
	int i;
	int n_chans, n_chans1;
	int start_chan_idx, start_chan_idx1;
	struct hostapd_channel_data *chan;
	struct hostapd_hw_modes *mode;

	mode = iface->current_mode;
	if (!mode)
		return 0;

	start_chan_idx = dfs_get_start_chan_idx(iface, &start_chan_idx1);
	n_chans = dfs_get_used_n_chans(iface, &n_chans1);

	for (i = 0; i < n_chans; i++) {
		chan = &mode->channels[start_chan_idx + i];

		/* On operationl BW */
		if (chan->chan == channel)
			return 0;
	}

	return 1;
}

/* Counter is incremented on first radar detection, to track afterwards if
 * sub band DFS is active, or when radar detected on non-operational sub band */
static void
hostapd_sb_dfs_inc_cntr (struct hostapd_iface *iface, u8 rbm)
{
	int i, start_chan_idx, n_chans;
	struct hostapd_channel_data *chan;
	struct hostapd_hw_modes *mode;

	mode = iface->current_mode;
	if (!mode)
		return;

	if (!iface->sb_dfs_cntr) {
		iface->sb_dfs_cntr++;
		return;
	}

	start_chan_idx = hostapd_get_low_chan_idx(iface, iface->sb_orig_chandef.vht_oper_centr_freq_seg0_idx,
						  iface->sb_orig_chandef.vht_oper_chwidth,
						  iface->sb_orig_chandef.channel,
						  iface->sb_orig_chandef.secondary_offset);
	n_chans = hostapd_get_num_chans(iface, iface->sb_orig_chandef.vht_oper_chwidth, iface->sb_orig_chandef.secondary_offset);

	if (-1 == start_chan_idx) {
		wpa_printf(MSG_ERROR, "SB DFS: Cannot get start channel index");
		return;
	}

	for (i = 0; i < n_chans; i++) {
		chan = &mode->channels[start_chan_idx + i];

		if (!(chan->flag & HOSTAPD_CHAN_RADAR))
			continue;

		if (!(rbm & 1 << i))
			continue;

		if (hostapd_sb_dfs_is_on_adjacent(iface, chan->chan)) {
			iface->sb_dfs_cntr++;
			return;
		}

	}
}

static int
hostapd_sub_dfs_switch_chan (struct hostapd_iface *iface, u8 rbm, int chan,
			     int bandwidth, int secondary_offset)
{
	int i, err;
	u8 vht_oper_chwidth;
	u8 orig_vht_oper_chwidth;
	u8 vht_oper_centr_freq_seg0_idx = 0;
	u8 vht_oper_centr_freq_seg1_idx = 0; /* 80+80 is not supported by DFS */
	int secondary_channel = secondary_offset;
	struct csa_settings csa_settings;
	struct hostapd_channel_data *channel = NULL;
	int switch_to_full = !chan && !bandwidth && !secondary_offset;

	os_memset(&csa_settings, 0, sizeof(csa_settings));
	csa_settings.cs_count = hostapd_get_cs_count(iface);
	csa_settings.block_tx = 1;

	if (switch_to_full)
		chan = iface->sb_orig_chandef.channel;

	channel = hostapd_get_mode_channel(iface, (hostapd_chan_to_freq(chan)));
	if (!channel) {
		wpa_printf(MSG_ERROR, "SB DFS: Channel %d not supported by HW", chan);
		return 1;
	}

	if (switch_to_full) {
		csa_settings.sb_dfs_bw = NL80211_SB_DFS_BW_NORMAL;
		secondary_channel = iface->sb_orig_chandef.secondary_offset;
		vht_oper_chwidth = iface->sb_orig_chandef.vht_oper_chwidth;
		vht_oper_centr_freq_seg0_idx = iface->sb_orig_chandef.vht_oper_centr_freq_seg0_idx;
	}
	else {
		csa_settings.sb_dfs_bw = hostapd_bandwidth_to_nl_sb_dfs_bw(bandwidth);

		if (csa_settings.sb_dfs_bw == -1) {
			wpa_printf(MSG_ERROR, "SB DFS: Could not set new operational sub band bandwidth");
			return 1;
		}

		orig_vht_oper_chwidth = hostapd_get_oper_chwidth(iface->conf);

		if (NL80211_SB_DFS_BW_80 == csa_settings.sb_dfs_bw) {
			vht_oper_chwidth = CHANWIDTH_80MHZ;
			hostapd_set_oper_chwidth(iface->conf,vht_oper_chwidth);
		} else {
			vht_oper_chwidth = CHANWIDTH_USE_HT;
			hostapd_set_oper_chwidth(iface->conf,vht_oper_chwidth);
		}

		dfs_adjust_center_freq(iface, channel, secondary_channel,
					   &vht_oper_centr_freq_seg0_idx,
					   &vht_oper_centr_freq_seg1_idx);

		hostapd_set_oper_chwidth(iface->conf,orig_vht_oper_chwidth);

		if (iface->sb_dfs_cntr == 0) {
			iface->sb_orig_chandef.channel = iface->conf->channel;
			iface->sb_orig_chandef.secondary_offset = iface->conf->secondary_channel;
			iface->sb_orig_chandef.vht_oper_chwidth = hostapd_get_oper_chwidth(iface->conf) ;
			iface->sb_orig_chandef.vht_oper_centr_freq_seg0_idx = hostapd_get_oper_centr_freq_seg0_idx(iface->conf);
		}
	}

	err = hostapd_set_freq_params(&csa_settings.freq_params,
				      iface->conf->hw_mode,
				      channel->freq,
				      channel->chan,
                                      iface->conf->enable_edmg,
                                      iface->conf->edmg_channel,
				      iface->conf->ieee80211n,
				      iface->conf->ieee80211ac,
				      iface->conf->ieee80211ax,
				      secondary_channel,
				      vht_oper_chwidth,
				      vht_oper_centr_freq_seg0_idx,
				      vht_oper_centr_freq_seg1_idx,
				      iface->current_mode->vht_capab,
				      &iface->current_mode->he_capab);

	if (hostapd_prepare_and_send_csa_deauth_cfg_to_driver(iface->bss[0])) {
		wpa_printf(MSG_ERROR, "hostapd_prepare_and_send_csa_deauth_cfg_to_driver failed: %s",
			   iface->bss[0]->conf->iface);
		return 1;
	}

	hostapd_channel_switch_reason_set(iface, HAPD_CHAN_SWITCH_RADAR_DETECTED);
	for (i = 0; i < iface->num_bss; i++) {
		err = hostapd_switch_channel(iface->bss[i], &csa_settings);
		if (err)
			break;
	}

	if (err){
		wpa_printf(MSG_WARNING, "SB DFS: Failed to schedule CSA (%d)", err);
		return 1;
	}
	else {
		if (rbm)
			hostapd_sb_dfs_inc_cntr(iface, rbm);
	}
	return 0;
}

static int
hostapd_add_dfs_history_entry(struct hostapd_iface *iface,
			      int cf1, int cf2, int chan_width, u8 rbm, u8 type)
{
	struct os_time time;
	u8 seg0_idx = 0, seg1_idx = 0;
	hostapd_dfs_history_t *entry, *first_entry;

	/* If maximum number of entries reached, delete the oldest one */
	if (iface->dfs_hist_entry_cnt >= MAX_DFS_HIST_ENTRIES) {
		first_entry = dl_list_first(&iface->dfs_history, hostapd_dfs_history_t, list);

		if (first_entry) {
			dl_list_del(&first_entry->list);
			iface->dfs_hist_entry_cnt--;
			os_free(first_entry);
		}
	}

	entry = os_zalloc(sizeof(*entry));

	if (!entry)
		return 1;

	ieee80211_freq_to_chan(cf1, &seg0_idx);
	ieee80211_freq_to_chan(cf2, &seg1_idx);
	entry->seg0_idx = seg0_idx;
	entry->seg1_idx = seg0_idx;
	entry->bandwidth = chan_width_enum_to_freq(chan_width);
	entry->rbm = rbm;
	entry->primary = iface->conf->channel;
	entry->type = type;
	os_get_time(&time);
	entry->detect_time = time.sec;

	DL_LIST_ADD_TAIL(&iface->dfs_history, entry, list);
	iface->dfs_hist_entry_cnt++;

	return 0;
}

int hostapd_dfs_radar_detected(struct hostapd_iface *iface, int freq,
			       int ht_enabled, int chan_offset, int chan_width,
			       int cf1, int cf2, u8 rbm)
{
	int res = 0;
	int sb_dfs_new_prim = 0;
	int sb_dfs_new_bw = CHAN_WIDTH_20;
	int secondary_offset = 0;

	wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, DFS_EVENT_RADAR_DETECTED
		"freq=%d ht_enabled=%d chan_offset=%d chan_width=%d cf1=%d cf2=%d rbm=%d",
		freq, ht_enabled, chan_offset, chan_width, cf1, cf2, rbm);

	/* Proceed only if DFS is not offloaded to the driver */
	if (iface->drv_flags & WPA_DRIVER_FLAGS_DFS_OFFLOAD)
		return 0;

	if (!iface->conf->ieee80211h)
		return 0;

	/* In debug dfs mode, radar detection during CAC will lead to restart of CAC on
	 * the same channel, but because the kernel first stops CAC and notifies hostapd
	 * CAC_ABORT, and only then notifies about RADAR_DETECTION, reaching hostapd_dfs_radar_detected
	 * while iface->cac_started=true, means this is an old radar detection event from a burst
	 * of radars. continuing here, and attempting to restart CAC will lead to failure (-16 busy) */
	if (iface->conf->dfs_debug_chan && iface->dbg_dfs_in_cac) {
		wpa_printf(MSG_DEBUG, "received an old radar detection, ignoring");
		return 0;
	}

	/* mark radar frequency as invalid */
	set_dfs_state(iface, freq, ht_enabled, chan_offset, chan_width,
		      cf1, cf2, rbm, HOSTAPD_CHAN_DFS_UNAVAILABLE);

	if (iface->conf->dfs_channels_state_file) {
		/* If registered restart the timeout, so we won't update the nop time too soon */
		if (eloop_is_timeout_registered(hostapd_dfs_channel_state_file_update_eloop, iface, NULL))
			eloop_cancel_timeout(hostapd_dfs_channel_state_file_update_eloop, iface, NULL);
		hostapd_dfs_channel_state_file_update(iface, 0);
		eloop_register_timeout(60, 0, hostapd_dfs_channel_state_file_update_eloop, iface, NULL);
	}

#ifdef CONFIG_ACS
	if (iface->conf->acs_init_done)
		acs_update_radar(iface);
#endif

	if (!iface->conf->sub_band_dfs) {
		/* Skip if reported radar event not overlapped our channels */
		res = dfs_are_channels_overlapped(iface, freq, chan_width, cf1, cf2);
		if (!res)
			return 0;
	}

	wpa_printf(MSG_DEBUG, "SB DFS: Radar Bit Map 0x%02x", rbm);

	if (iface->in_scan) {
		wpa_printf(MSG_INFO, "Radar detected during scan, not initiating channel switch");
		hostapd_add_dfs_history_entry(iface, cf1, cf2, chan_width, rbm, DFS_NO_SWITCH);
		return 0;
	}

	/* if radar event happens during a scan triggered by iw, which started
	 * while CAC was in progress ignore event */
	if (iface->cac_required){
		wpa_printf(MSG_INFO, "CAC pending, not initiating channel switch");
		hostapd_add_dfs_history_entry(iface, cf1, cf2, chan_width, rbm, DFS_NO_SWITCH);
		return 0;
	}

	if (iface->conf->sub_band_dfs && !iface->conf->dfs_debug_chan) {
		sb_dfs_new_prim = hostapd_sub_dfs_get_new_chan(iface, rbm, &sb_dfs_new_bw, &secondary_offset);

		if (sb_dfs_new_prim == -1) { /* Stay on the same channel */
			hostapd_add_dfs_history_entry(iface, cf1, cf2, chan_width, rbm, DFS_NO_SWITCH);
			return 0;
		}
		else if (sb_dfs_new_prim > 0) {
			wpa_printf(MSG_INFO, "SB DFS: New chan %d, bw %s, Secondary %d",
				   sb_dfs_new_prim, channel_width_to_string(sb_dfs_new_bw), secondary_offset);
			hostapd_add_dfs_history_entry(iface, cf1, cf2, chan_width, rbm, DFS_SUB_BAND_SWICH);

			if (hostapd_sub_dfs_switch_chan(iface, rbm, sb_dfs_new_prim, sb_dfs_new_bw, secondary_offset))
				wpa_printf(MSG_ERROR, "SB DFS: Failed to switch to sub band, will try to switch to new channel");
			else
				return 0;
		}
		else { /* Will switch to new channel */
			wpa_printf(MSG_INFO, "SB DFS: No sub band switch possible, will switch to new channel");
			/* Restore original channel params */
			if (iface->sb_dfs_cntr) {
				hostapd_set_oper_chwidth(iface->conf,iface->sb_orig_chandef.vht_oper_chwidth);
				iface->conf->secondary_channel = iface->sb_orig_chandef.secondary_offset;
				iface->conf->channel = iface->sb_orig_chandef.channel;
				hostapd_set_oper_centr_freq_seg0_idx(iface->conf,iface->sb_orig_chandef.vht_oper_centr_freq_seg0_idx);
			}
		}
	}

	hostapd_add_dfs_history_entry(iface, cf1, cf2, chan_width, rbm, DFS_CHAN_SWITCH);
#ifdef CONFIG_ACS
	if (!dfs_is_failsafe_enabled(iface) && iface->conf->acs_init_done && !iface->conf->dfs_debug_chan)
		acs_radar_switch(iface);
	else
#endif
	/* radar detected while operating, switch the channel. */
	res = hostapd_dfs_start_channel_switch(iface);

	return res;
}

static int
hostapd_sb_dfs_can_switch_to_full_bw (struct hostapd_iface *iface)
{
	int i;
	int n_chans;
	struct hostapd_hw_modes *mode;
	struct hostapd_channel_data *chan;
	int start_chan_idx = -1;

	mode = iface->current_mode;
	if (!mode)
		return 0;

	n_chans = hostapd_get_num_chans(iface, iface->sb_orig_chandef.vht_oper_chwidth, iface->sb_orig_chandef.secondary_offset);
	start_chan_idx = hostapd_get_low_chan_idx(iface, iface->sb_orig_chandef.vht_oper_centr_freq_seg0_idx,
						  iface->sb_orig_chandef.vht_oper_chwidth,
						  iface->sb_orig_chandef.channel,
						  iface->sb_orig_chandef.secondary_offset);

	if (-1 == start_chan_idx) {
		wpa_printf(MSG_ERROR, "SB DFS: Could not get channel idx for channel %d",
			   iface->sb_orig_chandef.channel);
		return 0;
	}

	for (i = 0; i < n_chans; i++) {
		chan = &mode->channels[start_chan_idx + i];

		if (!(chan->flag & HOSTAPD_CHAN_RADAR))
			continue;

		if ((chan->flag & HOSTAPD_CHAN_DFS_MASK) != HOSTAPD_CHAN_DFS_AVAILABLE)
			return 0;
	}

	wpa_printf(MSG_INFO, "SB DFS: Switching back to full bandwidth");
	return 1;
}

int hostapd_dfs_nop_finished(struct hostapd_iface *iface, int freq,
			     int ht_enabled, int chan_offset, int chan_width,
			     int cf1, int cf2)
{
	int ret;

	/* Proceed only if DFS is not offloaded to the driver */
	if (iface->drv_flags & WPA_DRIVER_FLAGS_DFS_OFFLOAD)
		return 0;

	/* TODO add correct implementation here */
	ret = set_dfs_state(iface, freq, ht_enabled, chan_offset, chan_width,
		      cf1, cf2, 0, HOSTAPD_CHAN_DFS_USABLE);

	if (iface->conf->dfs_channels_state_file) {
		hostapd_dfs_channel_state_file_update(iface, 0);
	}

	if (ret > 0)
		wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, DFS_EVENT_NOP_FINISHED
			"freq=%d ht_enabled=%d chan_offset=%d chan_width=%d cf1=%d cf2=%d",
			freq, ht_enabled, chan_offset, chan_width, cf1, cf2);
#ifdef CONFIG_ACS
	if (iface->conf->acs_init_done)
		acs_update_radar(iface);
#endif

	/* Handle cases where all channels became unavailable */
	if (iface->state == HAPD_IFACE_DISABLED)
		hostapd_dfs_start_channel_switch(iface);
	/* Handle cases where all channels were initially unavailable */
	if (iface->state == HAPD_IFACE_DFS && !iface->cac_started) {
		hostapd_handle_dfs(iface);
	}

	return 0;
}


int hostapd_is_dfs_required(struct hostapd_iface *iface)
{
	int n_chans, n_chans1, start_chan_idx, start_chan_idx1, res;

	if (!iface->conf->ieee80211h || !iface->current_mode ||
	    iface->current_mode->mode != HOSTAPD_MODE_IEEE80211A)
		return 0;

	/* Get start (first) channel for current configuration */
	start_chan_idx = dfs_get_start_chan_idx(iface, &start_chan_idx1);
	if (start_chan_idx == -1)
		return -1;

	/* Get number of used channels, depend on width */
	n_chans = dfs_get_used_n_chans(iface, &n_chans1);

	/* Check if any of configured channels require DFS */
	res = dfs_check_chans_radar(iface, start_chan_idx, n_chans);
	if (res)
		return res;
	if (start_chan_idx1 >= 0 && n_chans1 > 0)
		res = dfs_check_chans_radar(iface, start_chan_idx1, n_chans1);
	return res;
}


int hostapd_dfs_start_cac(struct hostapd_iface *iface, int freq,
			  int ht_enabled, int chan_offset, int chan_width,
			  int cf1, int cf2)
{
	/* This is called when the driver indicates that an offloaded DFS has
	 * started CAC. */
	hostapd_set_state(iface, HAPD_IFACE_DFS);
	/* TODO: How to check CAC time for ETSI weather channels? */
	iface->dfs_cac_ms = 60000;
	wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, DFS_EVENT_CAC_START
		"freq=%d chan=%d chan_offset=%d width=%d seg0=%d "
		"seg1=%d cac_time=%ds",
		freq, (freq - 5000) / 5, chan_offset, chan_width, cf1, cf2,
		iface->dfs_cac_ms / 1000);
	iface->cac_started = 1;
	os_get_reltime(&iface->dfs_cac_start);
	return 0;
}


/*
 * Main DFS handler for offloaded case.
 * 2 - continue channel/AP setup for non-DFS channel
 * 1 - continue channel/AP setup for DFS channel
 * 0 - channel/AP setup will be continued after CAC
 * -1 - hit critical error
 */
int hostapd_handle_dfs_offload(struct hostapd_iface *iface)
{
	wpa_printf(MSG_DEBUG, "%s: iface->cac_started: %d",
		   __func__, iface->cac_started);

	/*
	 * If DFS has already been started, then we are being called from a
	 * callback to continue AP/channel setup. Reset the CAC start flag and
	 * return.
	 */
	if (iface->cac_started) {
		wpa_printf(MSG_DEBUG, "%s: iface->cac_started: %d",
			   __func__, iface->cac_started);
		iface->cac_started = 0;
		return 1;
	}

	if (ieee80211_is_dfs(iface->freq, iface->hw_features,
			     iface->num_hw_features)) {
		wpa_printf(MSG_DEBUG, "%s: freq %d MHz requires DFS",
			   __func__, iface->freq);
		return 0;
	}

	wpa_printf(MSG_DEBUG,
		   "%s: freq %d MHz does not require DFS. Continue channel/AP setup",
		   __func__, iface->freq);
	return 2;
}
