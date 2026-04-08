/*
 * ACS - Automatic Channel Selection module
 * Copyright (c) 2011, Atheros Communications
 * Copyright (c) 2013, Qualcomm Atheros, Inc.
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"
#include <math.h>

#include "utils/common.h"
#include "utils/list.h"
#include "common/ieee802_11_defs.h"
#include "common/hw_features_common.h"
#include "common/wpa_ctrl.h"
#include "drivers/driver.h"
#include "hostapd.h"
#include "ap_drv_ops.h"
#include "ap_config.h"
#include "hw_features.h"
#include "dfs.h"
#include "beacon.h"
#include <assert.h>
#include "acs.h"
#include "rrm.h"
#include "ieee802_11.h"

/*
 * Automatic Channel Selection
 * ===========================
 *
 * More info at
 * ------------
 * http://wireless.kernel.org/en/users/Documentation/acs
 *
 * How to use
 * ----------
 * - make sure you have CONFIG_ACS=y in hostapd's .config
 * - use channel=0 or channel=acs to enable ACS
 *
 * How does it work
 * ----------------
 * 1. passive scans are used to collect survey data
 *    (it is assumed that scan trigger collection of survey data in driver)
 * 2. interference factor is calculated for each channel
 * 3. ideal channel is picked depending on channel width by using adjacent
 *    channel interference factors
 *
 * Known limitations
 * -----------------
 * - Current implementation depends heavily on the amount of time willing to
 *   spend gathering survey data during hostapd startup. Short traffic bursts
 *   may be missed and a suboptimal channel may be picked.
 * - Ideal channel may end up overlapping a channel with 40 MHz intolerant BSS
 *
 * Todo / Ideas
 * ------------
 * - implement other interference computation methods
 *   - BSS/RSSI based
 *   - spectral scan based
 *   (should be possibly to hook this up with current ACS scans)
 * - add wpa_supplicant support (for P2P)
 * - collect a histogram of interference over time allowing more educated
 *   guess about an ideal channel (perhaps CSA could be used to migrate AP to a
 *   new "better" channel while running)
 * - include neighboring BSS scan to avoid conflicts with 40 MHz intolerant BSSs
 *   when choosing the ideal channel
 *
 * Survey interference factor implementation details
 * -------------------------------------------------
 * Generic interference_factor in struct hostapd_channel_data is used.
 *
 * The survey interference factor is defined as the ratio of the
 * observed busy time over the time we spent on the channel,
 * this value is then amplified by the observed noise floor on
 * the channel in comparison to the lowest noise floor observed
 * on the entire band.
 *
 * This corresponds to:
 * ---
 * (busy time - tx time) / (active time - tx time) * 2^(chan_nf + band_min_nf)
 * ---
 *
 * The coefficient of 2 reflects the way power in "far-field"
 * radiation decreases as the square of distance from the antenna [1].
 * What this does is it decreases the observed busy time ratio if the
 * noise observed was low but increases it if the noise was high,
 * proportionally to the way "far field" radiation changes over
 * distance.
 *
 * If channel busy time is not available the fallback is to use channel RX time.
 *
 * Since noise floor is in dBm it is necessary to convert it into Watts so that
 * combined channel interference (e.g., HT40, which uses two channels) can be
 * calculated easily.
 * ---
 * (busy time - tx time) / (active time - tx time) *
 *    2^(10^(chan_nf/10) + 10^(band_min_nf/10))
 * ---
 *
 * However to account for cases where busy/rx time is 0 (channel load is then
 * 0%) channel noise floor signal power is combined into the equation so a
 * channel with lower noise floor is preferred. The equation becomes:
 * ---
 * 10^(chan_nf/5) + (busy time - tx time) / (active time - tx time) *
 *    2^(10^(chan_nf/10) + 10^(band_min_nf/10))
 * ---
 *
 * All this "interference factor" is purely subjective and only time
 * will tell how usable this is. By using the minimum noise floor we
 * remove any possible issues due to card calibration. The computation
 * of the interference factor then is dependent on what the card itself
 * picks up as the minimum noise, not an actual real possible card
 * noise value.
 *
 * Total interference computation details
 * --------------------------------------
 * The above channel interference factor is calculated with no respect to
 * target operational bandwidth.
 *
 * To find an ideal channel the above data is combined by taking into account
 * the target operational bandwidth and selected band. E.g., on 2.4 GHz channels
 * overlap with 20 MHz bandwidth, but there is no overlap for 20 MHz bandwidth
 * on 5 GHz.
 *
 * Each valid and possible channel spec (i.e., channel + width) is taken and its
 * interference factor is computed by summing up interferences of each channel
 * it overlaps. The one with least total interference is picked up.
 *
 * Note: This implies base channel interference factor must be non-negative
 * allowing easy summing up.
 *
 * Example ACS analysis printout
 * -----------------------------
 *
 * ACS: Trying survey-based ACS
 * ACS: Survey analysis for channel 1 (2412 MHz)
 * ACS:  1: min_nf=-113 interference_factor=0.0802469 nf=-113 time=162 busy=0 rx=13
 * ACS:  2: min_nf=-113 interference_factor=0.0745342 nf=-113 time=161 busy=0 rx=12
 * ACS:  3: min_nf=-113 interference_factor=0.0679012 nf=-113 time=162 busy=0 rx=11
 * ACS:  4: min_nf=-113 interference_factor=0.0310559 nf=-113 time=161 busy=0 rx=5
 * ACS:  5: min_nf=-113 interference_factor=0.0248447 nf=-113 time=161 busy=0 rx=4
 * ACS:  * interference factor average: 0.0557166
 * ACS: Survey analysis for channel 2 (2417 MHz)
 * ACS:  1: min_nf=-113 interference_factor=0.0185185 nf=-113 time=162 busy=0 rx=3
 * ACS:  2: min_nf=-113 interference_factor=0.0246914 nf=-113 time=162 busy=0 rx=4
 * ACS:  3: min_nf=-113 interference_factor=0.037037 nf=-113 time=162 busy=0 rx=6
 * ACS:  4: min_nf=-113 interference_factor=0.149068 nf=-113 time=161 busy=0 rx=24
 * ACS:  5: min_nf=-113 interference_factor=0.0248447 nf=-113 time=161 busy=0 rx=4
 * ACS:  * interference factor average: 0.050832
 * ACS: Survey analysis for channel 3 (2422 MHz)
 * ACS:  1: min_nf=-113 interference_factor=2.51189e-23 nf=-113 time=162 busy=0 rx=0
 * ACS:  2: min_nf=-113 interference_factor=0.0185185 nf=-113 time=162 busy=0 rx=3
 * ACS:  3: min_nf=-113 interference_factor=0.0186335 nf=-113 time=161 busy=0 rx=3
 * ACS:  4: min_nf=-113 interference_factor=0.0186335 nf=-113 time=161 busy=0 rx=3
 * ACS:  5: min_nf=-113 interference_factor=0.0186335 nf=-113 time=161 busy=0 rx=3
 * ACS:  * interference factor average: 0.0148838
 * ACS: Survey analysis for channel 4 (2427 MHz)
 * ACS:  1: min_nf=-114 interference_factor=1.58489e-23 nf=-114 time=162 busy=0 rx=0
 * ACS:  2: min_nf=-114 interference_factor=0.0555556 nf=-114 time=162 busy=0 rx=9
 * ACS:  3: min_nf=-114 interference_factor=1.58489e-23 nf=-114 time=161 busy=0 rx=0
 * ACS:  4: min_nf=-114 interference_factor=0.0186335 nf=-114 time=161 busy=0 rx=3
 * ACS:  5: min_nf=-114 interference_factor=0.00621118 nf=-114 time=161 busy=0 rx=1
 * ACS:  * interference factor average: 0.0160801
 * ACS: Survey analysis for channel 5 (2432 MHz)
 * ACS:  1: min_nf=-114 interference_factor=0.409938 nf=-113 time=161 busy=0 rx=66
 * ACS:  2: min_nf=-114 interference_factor=0.0432099 nf=-113 time=162 busy=0 rx=7
 * ACS:  3: min_nf=-114 interference_factor=0.0124224 nf=-113 time=161 busy=0 rx=2
 * ACS:  4: min_nf=-114 interference_factor=0.677019 nf=-113 time=161 busy=0 rx=109
 * ACS:  5: min_nf=-114 interference_factor=0.0186335 nf=-114 time=161 busy=0 rx=3
 * ACS:  * interference factor average: 0.232244
 * ACS: Survey analysis for channel 6 (2437 MHz)
 * ACS:  1: min_nf=-113 interference_factor=0.552795 nf=-113 time=161 busy=0 rx=89
 * ACS:  2: min_nf=-113 interference_factor=0.0807453 nf=-112 time=161 busy=0 rx=13
 * ACS:  3: min_nf=-113 interference_factor=0.0310559 nf=-113 time=161 busy=0 rx=5
 * ACS:  4: min_nf=-113 interference_factor=0.434783 nf=-112 time=161 busy=0 rx=70
 * ACS:  5: min_nf=-113 interference_factor=0.0621118 nf=-113 time=161 busy=0 rx=10
 * ACS:  * interference factor average: 0.232298
 * ACS: Survey analysis for channel 7 (2442 MHz)
 * ACS:  1: min_nf=-113 interference_factor=0.440994 nf=-112 time=161 busy=0 rx=71
 * ACS:  2: min_nf=-113 interference_factor=0.385093 nf=-113 time=161 busy=0 rx=62
 * ACS:  3: min_nf=-113 interference_factor=0.0372671 nf=-113 time=161 busy=0 rx=6
 * ACS:  4: min_nf=-113 interference_factor=0.0372671 nf=-113 time=161 busy=0 rx=6
 * ACS:  5: min_nf=-113 interference_factor=0.0745342 nf=-113 time=161 busy=0 rx=12
 * ACS:  * interference factor average: 0.195031
 * ACS: Survey analysis for channel 8 (2447 MHz)
 * ACS:  1: min_nf=-114 interference_factor=0.0496894 nf=-112 time=161 busy=0 rx=8
 * ACS:  2: min_nf=-114 interference_factor=0.0496894 nf=-114 time=161 busy=0 rx=8
 * ACS:  3: min_nf=-114 interference_factor=0.0372671 nf=-113 time=161 busy=0 rx=6
 * ACS:  4: min_nf=-114 interference_factor=0.12963 nf=-113 time=162 busy=0 rx=21
 * ACS:  5: min_nf=-114 interference_factor=0.166667 nf=-114 time=162 busy=0 rx=27
 * ACS:  * interference factor average: 0.0865885
 * ACS: Survey analysis for channel 9 (2452 MHz)
 * ACS:  1: min_nf=-114 interference_factor=0.0124224 nf=-114 time=161 busy=0 rx=2
 * ACS:  2: min_nf=-114 interference_factor=0.0310559 nf=-114 time=161 busy=0 rx=5
 * ACS:  3: min_nf=-114 interference_factor=1.58489e-23 nf=-114 time=161 busy=0 rx=0
 * ACS:  4: min_nf=-114 interference_factor=0.00617284 nf=-114 time=162 busy=0 rx=1
 * ACS:  5: min_nf=-114 interference_factor=1.58489e-23 nf=-114 time=162 busy=0 rx=0
 * ACS:  * interference factor average: 0.00993022
 * ACS: Survey analysis for channel 10 (2457 MHz)
 * ACS:  1: min_nf=-114 interference_factor=0.00621118 nf=-114 time=161 busy=0 rx=1
 * ACS:  2: min_nf=-114 interference_factor=0.00621118 nf=-114 time=161 busy=0 rx=1
 * ACS:  3: min_nf=-114 interference_factor=0.00621118 nf=-114 time=161 busy=0 rx=1
 * ACS:  4: min_nf=-114 interference_factor=0.0493827 nf=-114 time=162 busy=0 rx=8
 * ACS:  5: min_nf=-114 interference_factor=1.58489e-23 nf=-114 time=162 busy=0 rx=0
 * ACS:  * interference factor average: 0.0136033
 * ACS: Survey analysis for channel 11 (2462 MHz)
 * ACS:  1: min_nf=-114 interference_factor=1.58489e-23 nf=-114 time=161 busy=0 rx=0
 * ACS:  2: min_nf=-114 interference_factor=2.51189e-23 nf=-113 time=161 busy=0 rx=0
 * ACS:  3: min_nf=-114 interference_factor=2.51189e-23 nf=-113 time=161 busy=0 rx=0
 * ACS:  4: min_nf=-114 interference_factor=0.0432099 nf=-114 time=162 busy=0 rx=7
 * ACS:  5: min_nf=-114 interference_factor=0.0925926 nf=-114 time=162 busy=0 rx=15
 * ACS:  * interference factor average: 0.0271605
 * ACS: Survey analysis for channel 12 (2467 MHz)
 * ACS:  1: min_nf=-114 interference_factor=0.0621118 nf=-113 time=161 busy=0 rx=10
 * ACS:  2: min_nf=-114 interference_factor=0.00621118 nf=-114 time=161 busy=0 rx=1
 * ACS:  3: min_nf=-114 interference_factor=2.51189e-23 nf=-113 time=162 busy=0 rx=0
 * ACS:  4: min_nf=-114 interference_factor=2.51189e-23 nf=-113 time=162 busy=0 rx=0
 * ACS:  5: min_nf=-114 interference_factor=0.00617284 nf=-113 time=162 busy=0 rx=1
 * ACS:  * interference factor average: 0.0148992
 * ACS: Survey analysis for channel 13 (2472 MHz)
 * ACS:  1: min_nf=-114 interference_factor=0.0745342 nf=-114 time=161 busy=0 rx=12
 * ACS:  2: min_nf=-114 interference_factor=0.0555556 nf=-114 time=162 busy=0 rx=9
 * ACS:  3: min_nf=-114 interference_factor=1.58489e-23 nf=-114 time=162 busy=0 rx=0
 * ACS:  4: min_nf=-114 interference_factor=1.58489e-23 nf=-114 time=162 busy=0 rx=0
 * ACS:  5: min_nf=-114 interference_factor=1.58489e-23 nf=-114 time=162 busy=0 rx=0
 * ACS:  * interference factor average: 0.0260179
 * ACS: Survey analysis for selected bandwidth 20MHz
 * ACS:  * channel 1: total interference = 0.121432
 * ACS:  * channel 2: total interference = 0.137512
 * ACS:  * channel 3: total interference = 0.369757
 * ACS:  * channel 4: total interference = 0.546338
 * ACS:  * channel 5: total interference = 0.690538
 * ACS:  * channel 6: total interference = 0.762242
 * ACS:  * channel 7: total interference = 0.756092
 * ACS:  * channel 8: total interference = 0.537451
 * ACS:  * channel 9: total interference = 0.332313
 * ACS:  * channel 10: total interference = 0.152182
 * ACS:  * channel 11: total interference = 0.0916111
 * ACS:  * channel 12: total interference = 0.0816809
 * ACS:  * channel 13: total interference = 0.0680776
 * ACS: Ideal channel is 13 (2472 MHz) with total interference factor of 0.0680776
 *
 * [1] http://en.wikipedia.org/wiki/Near_and_far_field
 */


static int acs_request_scan(struct hostapd_iface *iface);
static int acs_survey_is_sufficient(struct freq_survey *survey);
static u32 acs_get_center_chan_index(struct hostapd_iface *iface);
void acs_push_chandef(struct hostapd_iface *iface, acs_chandef *chan);
void acs_pop_chandef(struct hostapd_iface *iface, acs_chandef *chan);


/* Open file for writing/appending. If opened for appending and it is bigger
 * than 10K, file is saved with filename .0 at the end and a new empty file is
 * created. */
static FILE * acs_write_file(const char *name, Boolean append)
{
#define ACS_MAX_LOG_SIZE	10240 /* 10K */

	FILE *fp;
	long int sz;
	int res;
	char *bak_file;
	size_t new_size;

	if (!append)
		return fopen(name, "w");
	fp = fopen(name, "a+");
	if (!fp) {
		wpa_printf(MSG_ERROR, "ACS: cannot open file [%s]. %s", name,
			strerror(errno));
		return fp;
	}
	res = fseek(fp, 0L, SEEK_END);
	if (res == -1) {
		wpa_printf(MSG_ERROR, "ACS: cannot set file position indicator of file [%s]. %s",
			name, strerror(errno));
		fclose(fp);
		return NULL;
	}
	sz = ftell(fp);
	if (sz == -1) {
		wpa_printf(MSG_ERROR, "ACS: cannot tell size of file [%s]. %s", name,
			strerror(errno));
		fclose(fp);
		return NULL;
	}
	if (sz > ACS_MAX_LOG_SIZE) {
		fclose(fp);
		new_size = strlen(name) + 3;
		bak_file = os_malloc(new_size);
		if (bak_file == NULL)
			return NULL;
		os_snprintf(bak_file, new_size, "%s.0", name);
		remove(bak_file);
		res = rename(name, bak_file);
		os_free(bak_file);
		if (res == -1)
			wpa_printf(MSG_WARNING, "ACS: making backup of file [%s] failed. %s", name,
				strerror(errno));
		fp = fopen(name, "w");
		if (!fp) {
			wpa_printf(MSG_ERROR, "ACS: cannot open file [%s]. %s", name,
				strerror(errno));
		}
	}
	return fp;
}


static void acs_clean_chan_surveys(struct hostapd_channel_data *chan)
{
	struct freq_survey *survey, *tmp;

	if (dl_list_empty(&chan->survey_list))
		return;

	dl_list_for_each_safe(survey, tmp, &chan->survey_list,
			      struct freq_survey, list) {
		dl_list_del(&survey->list);
		os_free(survey);
	}
}


void acs_cleanup(struct hostapd_iface *iface)
{
	int i;
	struct hostapd_channel_data *chan;

	for (i = 0; i < iface->current_mode->num_channels; i++) {
		chan = &iface->current_mode->channels[i];

		if (chan->flag & HOSTAPD_CHAN_SURVEY_LIST_INITIALIZED)
			acs_clean_chan_surveys(chan);

		dl_list_init(&chan->survey_list);
		chan->flag |= HOSTAPD_CHAN_SURVEY_LIST_INITIALIZED;
		chan->min_nf = 0;
	}

	iface->chans_surveyed = 0;
	iface->acs_num_completed_scans = 0;
}


static void acs_fail(struct hostapd_iface *iface)
{
	wpa_printf(MSG_ERROR, "ACS: failed");
	acs_cleanup(iface);

	if (iface->current_mode->num_channels)
		iface->conf->channel = iface->current_mode->channels[0].chan; /* first chan in mode */
	else
		hostapd_disable_iface(iface);
}


static long double
acs_survey_interference_factor(struct freq_survey *survey, s8 min_nf)
{
	long double factor, busy, total;

	if (survey->filled & SURVEY_HAS_CHAN_TIME_BUSY)
		busy = survey->channel_time_busy;
	else if (survey->filled & SURVEY_HAS_CHAN_TIME_RX)
		busy = survey->channel_time_rx;
	else {
		/* This shouldn't really happen as survey data is checked in
		 * acs_sanity_check() */
		wpa_printf(MSG_ERROR, "ACS: Survey data missing");
		return 0;
	}

	total = survey->channel_time;

	if (survey->filled & SURVEY_HAS_CHAN_TIME_TX) {
		busy -= survey->channel_time_tx;
		total -= survey->channel_time_tx;
	}

	/* TODO: figure out the best multiplier for noise floor base */
	factor = pow(10, survey->nf / 5.0L) +
		(total ? (busy / total) : 0) *
		pow(2, pow(10, (long double) survey->nf / 10.0L) -
		    pow(10, (long double) min_nf / 10.0L));

	return factor;
}


static void
acs_survey_chan_interference_factor(struct hostapd_iface *iface,
				    struct hostapd_channel_data *chan)
{
	struct freq_survey *survey;
	unsigned int i = 0;
	long double int_factor = 0;
	unsigned count = 0;

	if (dl_list_empty(&chan->survey_list) ||
		(chan->flag & HOSTAPD_CHAN_DISABLED))
		return;


	chan->interference_factor = 0;

	dl_list_for_each(survey, &chan->survey_list, struct freq_survey, list)
	{
		i++;

		if (!acs_survey_is_sufficient(survey)) {
			wpa_printf(MSG_DEBUG, "ACS: %d: insufficient data", i);
			continue;
		}

		count++;
		int_factor = acs_survey_interference_factor(survey,
							    iface->lowest_nf);
		chan->interference_factor += int_factor;
		wpa_printf(MSG_DEBUG, "ACS: %d: min_nf=%d interference_factor=%Lg nf=%d time=%lu busy=%lu rx=%lu",
			   i, chan->min_nf, int_factor,
			   survey->nf, (unsigned long) survey->channel_time,
			   (unsigned long) survey->channel_time_busy,
			   (unsigned long) survey->channel_time_rx);
	}

	if (count)
		chan->interference_factor /= count;
}


static int acs_usable_ht40_chan(const struct hostapd_channel_data *chan)
{
	const int allowed[] = { 36, 44, 52, 60, 100, 108, 116, 124, 132, 149,
				157, 184, 192 };
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(allowed); i++)
		if (chan->chan == allowed[i])
			return 1;

	return 0;
}


static int acs_usable_vht80_chan(const struct hostapd_channel_data *chan)
{
	const int allowed[] = { 36, 52, 100, 116, 132, 149 };
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(allowed); i++)
		if (chan->chan == allowed[i])
			return 1;

	return 0;
}

static int acs_usable_vht160_chan(const struct hostapd_channel_data *chan)
{
	const int allowed[] = { 36, 100 };
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(allowed); i++)
		if (chan->chan == allowed[i])
			return 1;

	return 0;
}

static int acs_survey_is_sufficient(struct freq_survey *survey)
{
	if (!(survey->filled & SURVEY_HAS_NF)) {
		wpa_printf(MSG_INFO, "ACS: Survey is missing noise floor");
		return 0;
	}

	if (!(survey->filled & SURVEY_HAS_CHAN_TIME)) {
		wpa_printf(MSG_INFO, "ACS: Survey is missing channel time");
		return 0;
	}

	if (!(survey->filled & SURVEY_HAS_CHAN_TIME_BUSY) &&
	    !(survey->filled & SURVEY_HAS_CHAN_TIME_RX)) {
		wpa_printf(MSG_INFO,
			   "ACS: Survey is missing RX and busy time (at least one is required)");
		return 0;
	}

	return 1;
}


static int acs_survey_list_is_sufficient(struct hostapd_channel_data *chan)
{
	struct freq_survey *survey;
	int ret = -1;

	dl_list_for_each(survey, &chan->survey_list, struct freq_survey, list)
	{
		if (acs_survey_is_sufficient(survey)) {
			ret = 1;
			break;
		}
		ret = 0;
	}

	if (ret == -1)
		ret = 1; /* no survey list entries */

	if (!ret) {
		wpa_printf(MSG_INFO,
			   "ACS: Channel %d has insufficient survey data",
			   chan->chan);
	}

	return ret;
}


static int acs_surveys_are_sufficient(struct hostapd_iface *iface)
{
	int i;
	struct hostapd_channel_data *chan;
	int valid = 0;

	for (i = 0; i < iface->current_mode->num_channels; i++) {
		chan = &iface->current_mode->channels[i];
		if (!(chan->flag & HOSTAPD_CHAN_DISABLED) &&
		    acs_survey_list_is_sufficient(chan))
		valid++;
	}

	/* We need at least survey data for one channel */
	return !!valid;
}


static int acs_usable_chan(struct hostapd_channel_data *chan)
{
	return !dl_list_empty(&chan->survey_list) &&
		!(chan->flag & HOSTAPD_CHAN_DISABLED) &&
		acs_survey_list_is_sufficient(chan);
}


static int is_in_chanlist(struct hostapd_iface *iface,
			  struct hostapd_channel_data *chan)
{
	if (!iface->conf->acs_ch_list.num)
		return 1;

	return freq_range_list_includes(&iface->conf->acs_ch_list, chan->chan);
}


static void acs_survey_all_chans_intereference_factor(
	struct hostapd_iface *iface)
{
	int i;
	struct hostapd_channel_data *chan;

	for (i = 0; i < iface->current_mode->num_channels; i++) {
		chan = &iface->current_mode->channels[i];

		if (!acs_usable_chan(chan))
			continue;

		if (!is_in_chanlist(iface, chan))
			continue;

		wpa_printf(MSG_DEBUG, "ACS: Survey analysis for channel %d (%d MHz)",
			   chan->chan, chan->freq);

		acs_survey_chan_interference_factor(iface, chan);

		wpa_printf(MSG_DEBUG, "ACS:  * interference factor average: %Lg",
			   chan->interference_factor);
	}
}


static struct hostapd_channel_data *acs_find_chan(struct hostapd_iface *iface,
						  int freq)
{
	struct hostapd_channel_data *chan;
	int i;

	for (i = 0; i < iface->current_mode->num_channels; i++) {
		chan = &iface->current_mode->channels[i];

		if (chan->flag & HOSTAPD_CHAN_DISABLED)
			continue;

		if (chan->freq == freq)
			return chan;
	}

	return NULL;
}


static int is_24ghz_mode(enum hostapd_hw_mode mode)
{
	return mode == HOSTAPD_MODE_IEEE80211B ||
		mode == HOSTAPD_MODE_IEEE80211G;
}


static int is_common_24ghz_chan(int chan)
{
	return chan == 1 || chan == 6 || chan == 11;
}


#ifndef ACS_ADJ_WEIGHT
#define ACS_ADJ_WEIGHT 0.85
#endif /* ACS_ADJ_WEIGHT */

#ifndef ACS_NEXT_ADJ_WEIGHT
#define ACS_NEXT_ADJ_WEIGHT 0.55
#endif /* ACS_NEXT_ADJ_WEIGHT */

#ifndef ACS_24GHZ_PREFER_1_6_11
/*
 * Select commonly used channels 1, 6, 11 by default even if a neighboring
 * channel has a smaller interference factor as long as it is not better by more
 * than this multiplier.
 */
#define ACS_24GHZ_PREFER_1_6_11 0.8
#endif /* ACS_24GHZ_PREFER_1_6_11 */

/*
 * At this point it's assumed chan->interface_factor has been computed.
 * This function should be reusable regardless of interference computation
 * option (survey, BSS, spectral, ...). chan->interference factor must be
 * summable (i.e., must be always greater than zero).
 */
static struct hostapd_channel_data *
acs_find_ideal_chan(struct hostapd_iface *iface)
{
	struct hostapd_channel_data *chan, *adj_chan, *ideal_chan = NULL,
		*rand_chan = NULL;
	long double factor, ideal_factor = 0;
	int i, j;
	int n_chans = 1;
	u32 bw;
	unsigned int k;

	/* TODO: HT40- support */

	if (iface->conf->ieee80211n &&
	    iface->conf->secondary_channel == -1) {
		wpa_printf(MSG_ERROR, "ACS: HT40- is not supported yet. Please try HT40+");
		return NULL;
	}

	if (iface->conf->ieee80211n &&
	    iface->conf->secondary_channel)
		n_chans = 2;

	if (iface->conf->ieee80211ac) {
		switch (iface->conf->vht_oper_chwidth) {
		case CHANWIDTH_80MHZ:
			n_chans = 4;
			break;
		case CHANWIDTH_160MHZ:
			n_chans = 8;
			break;
		}
	}

	bw = num_chan_to_bw(n_chans);

	/* TODO: VHT80+80. Update acs_adjust_center_freq() too. */

	wpa_printf(MSG_DEBUG,
		   "ACS: Survey analysis for selected bandwidth %d MHz", bw);

	for (i = 0; i < iface->current_mode->num_channels; i++) {
		double total_weight;
		struct acs_bias *bias, tmp_bias;

		chan = &iface->current_mode->channels[i];

		/* Since in the current ACS implementation the first channel is
		 * always a primary channel, skip channels not available as
		 * primary until more sophisticated channel selection is
		 * implemented. */
		if (!chan_pri_allowed(chan))
			continue;

		if (!is_in_chanlist(iface, chan))
			continue;

		if (!chan_bw_allowed(chan, bw, 1, 1)) {
			wpa_printf(MSG_DEBUG,
				   "ACS: Channel %d: BW %u is not supported",
				   chan->chan, bw);
			continue;
		}

		/* HT40 on 5 GHz has a limited set of primary channels as per
		 * 11n Annex J */
		if (iface->current_mode->mode == HOSTAPD_MODE_IEEE80211A &&
		    iface->conf->ieee80211n &&
		    iface->conf->secondary_channel &&
		    !acs_usable_ht40_chan(chan)) {
			wpa_printf(MSG_DEBUG, "ACS: Channel %d: not allowed as primary channel for HT40",
				   chan->chan);
			continue;
		}

		if (iface->current_mode->mode == HOSTAPD_MODE_IEEE80211A &&
		    iface->conf->ieee80211ac) {
			if (iface->conf->vht_oper_chwidth ==
			    CHANWIDTH_80MHZ &&
			    !acs_usable_vht80_chan(chan)) {
				wpa_printf(MSG_DEBUG,
					   "ACS: Channel %d: not allowed as primary channel for VHT80",
					   chan->chan);
				continue;
			}

			if (iface->conf->vht_oper_chwidth ==
			    CHANWIDTH_160MHZ &&
			    !acs_usable_vht160_chan(chan)) {
				wpa_printf(MSG_DEBUG,
					   "ACS: Channel %d: not allowed as primary channel for VHT160",
					   chan->chan);
				continue;
			}
		}

		factor = 0;
		if (acs_usable_chan(chan))
			factor = chan->interference_factor;
		total_weight = 1;

		for (j = 1; j < n_chans; j++) {
			adj_chan = acs_find_chan(iface, chan->freq + (j * 20));
			if (!adj_chan)
				break;

			if (!chan_bw_allowed(adj_chan, bw, 1, 0)) {
				wpa_printf(MSG_DEBUG,
					   "ACS: PRI Channel %d: secondary channel %d BW %u is not supported",
					   chan->chan, adj_chan->chan, bw);
				break;
			}

			if (acs_usable_chan(adj_chan)) {
				factor += adj_chan->interference_factor;
				total_weight += 1;
			}
		}

		if (j != n_chans) {
			wpa_printf(MSG_DEBUG, "ACS: Channel %d: not enough bandwidth",
				   chan->chan);
			continue;
		}

		/* 2.4 GHz has overlapping 20 MHz channels. Include adjacent
		 * channel interference factor. */
		if (is_24ghz_mode(iface->current_mode->mode)) {
			for (j = 0; j < n_chans; j++) {
				adj_chan = acs_find_chan(iface, chan->freq +
							 (j * 20) - 5);
				if (adj_chan && acs_usable_chan(adj_chan)) {
					factor += ACS_ADJ_WEIGHT *
						adj_chan->interference_factor;
					total_weight += ACS_ADJ_WEIGHT;
				}

				adj_chan = acs_find_chan(iface, chan->freq +
							 (j * 20) - 10);
				if (adj_chan && acs_usable_chan(adj_chan)) {
					factor += ACS_NEXT_ADJ_WEIGHT *
						adj_chan->interference_factor;
					total_weight += ACS_NEXT_ADJ_WEIGHT;
				}

				adj_chan = acs_find_chan(iface, chan->freq +
							 (j * 20) + 5);
				if (adj_chan && acs_usable_chan(adj_chan)) {
					factor += ACS_ADJ_WEIGHT *
						adj_chan->interference_factor;
					total_weight += ACS_ADJ_WEIGHT;
				}

				adj_chan = acs_find_chan(iface, chan->freq +
							 (j * 20) + 10);
				if (adj_chan && acs_usable_chan(adj_chan)) {
					factor += ACS_NEXT_ADJ_WEIGHT *
						adj_chan->interference_factor;
					total_weight += ACS_NEXT_ADJ_WEIGHT;
				}
			}
		}

		factor /= total_weight;

		bias = NULL;
		if (iface->conf->acs_chan_bias) {
			for (k = 0; k < iface->conf->num_acs_chan_bias; k++) {
				bias = &iface->conf->acs_chan_bias[k];
				if (bias->channel == chan->chan)
					break;
				bias = NULL;
			}
		} else if (is_24ghz_mode(iface->current_mode->mode) &&
			   is_common_24ghz_chan(chan->chan)) {
			tmp_bias.channel = chan->chan;
			tmp_bias.bias = ACS_24GHZ_PREFER_1_6_11;
			bias = &tmp_bias;
		}

		if (bias) {
			factor *= bias->bias;
			wpa_printf(MSG_DEBUG,
				   "ACS:  * channel %d: total interference = %Lg (%f bias)",
				   chan->chan, factor, bias->bias);
		} else {
			wpa_printf(MSG_DEBUG,
				   "ACS:  * channel %d: total interference = %Lg",
				   chan->chan, factor);
		}

		if (acs_usable_chan(chan) &&
		    (!ideal_chan || factor < ideal_factor)) {
			ideal_factor = factor;
			ideal_chan = chan;
		}

		/* This channel would at least be usable */
		if (!rand_chan)
			rand_chan = chan;
	}

	if (ideal_chan) {
		wpa_printf(MSG_DEBUG, "ACS: Ideal channel is %d (%d MHz) with total interference factor of %Lg",
			   ideal_chan->chan, ideal_chan->freq, ideal_factor);
		return ideal_chan;
	}

	return rand_chan;
}

static void acs_adjust_center_freq(struct hostapd_iface *iface)
{
	int offset;

	wpa_printf(MSG_DEBUG, "ACS: Adjusting VHT center frequency");

	switch (iface->conf->vht_oper_chwidth) {
	case CHANWIDTH_USE_HT:
		offset = 2 * iface->conf->secondary_channel;
		break;
	case CHANWIDTH_80MHZ:
		offset = 6;
		break;
	case CHANWIDTH_160MHZ:
		offset = 14;
		break;
	default:
		/* TODO: How can this be calculated? Adjust
		 * acs_find_ideal_chan() */
		wpa_printf(MSG_INFO,
			   "ACS: Only VHT20/40/80/160 is supported now");
		return;
	}

	iface->conf->vht_oper_centr_freq_seg0_idx =
		iface->conf->channel + offset;
}


static int acs_study_survey_based(struct hostapd_iface *iface)
{
	wpa_printf(MSG_DEBUG, "ACS: Trying survey-based ACS");

	if (!iface->chans_surveyed) {
		wpa_printf(MSG_ERROR, "ACS: Unable to collect survey data");
		return -1;
	}

	if (!acs_surveys_are_sufficient(iface)) {
		wpa_printf(MSG_ERROR, "ACS: Surveys have insufficient data");
		return -1;
	}

	acs_survey_all_chans_intereference_factor(iface);
	return 0;
}


static int acs_study_options(struct hostapd_iface *iface)
{
	if (acs_study_survey_based(iface) == 0)
		return 0;

	/* TODO: If no surveys are available/sufficient this is a good
	 * place to fallback to BSS-based ACS */

	return -1;
}


static void acs_study(struct hostapd_iface *iface)
{
	struct hostapd_channel_data *ideal_chan;
	int err;

	err = acs_study_options(iface);
	if (err < 0) {
		wpa_printf(MSG_ERROR, "ACS: All study options have failed");
		goto fail;
	}

	ideal_chan = acs_find_ideal_chan(iface);
	if (!ideal_chan) {
		wpa_printf(MSG_ERROR, "ACS: Failed to compute ideal channel");
		err = -1;
		goto fail;
	}

	iface->conf->channel = ideal_chan->chan;

	if (iface->conf->ieee80211ac || iface->conf->ieee80211ax)
		acs_adjust_center_freq(iface);

	err = 0;
fail:
	/*
	 * hostapd_setup_interface_complete() will return -1 on failure,
	 * 0 on success and 0 is HOSTAPD_CHAN_VALID :)
	 */
	if (hostapd_acs_completed(iface, err) == HOSTAPD_CHAN_VALID) {
		acs_cleanup(iface);
		return;
	}

	/* This can possibly happen if channel parameters (secondary
	 * channel, center frequencies) are misconfigured */
	wpa_printf(MSG_ERROR, "ACS: Possibly channel configuration is invalid, please report this along with your config file.");
	acs_fail(iface);
}

int get_num_width(int vht_width, int secondary)
{
  switch (vht_width) {
    case CHANWIDTH_USE_HT:   return secondary ? 40 : 20;
    case CHANWIDTH_80MHZ:    return 80;
    case CHANWIDTH_160MHZ:   return 160;
    case CHANWIDTH_80P80MHZ:
    default: wpa_printf(MSG_ERROR, "ACS: wrong or unsupported channel VHT width %d", vht_width);
  }

  return -1; /* should never happen */
}

int get_vht_width(int width)
{
  switch (width) {
    case 20:
    case 40:  return CHANWIDTH_USE_HT;
    case 80:  return CHANWIDTH_80MHZ;
    case 160: return CHANWIDTH_160MHZ;
    default: wpa_printf(MSG_ERROR, "ACS: wrong or unsupported channel width %d", width);
  }

  return -1; /* should never happen */
}

static inline unsigned acs_freq_to_chan(int freq)
{
	u8 chan;
	ieee80211_freq_to_chan(freq, &chan);
	return chan;
}

int acs_chan_to_freq(int channel)
{
  int res = 0;

  /* IEEE Std 802.11-2012: 20.3.15 Channel numbering and channelization
  Channel center frequency = starting frequency + 5 * ch
  */

#define CHANNEL_THRESHOLD 180

  /* channels 1..14 */
  if (channel >= 1 && channel <= 14) {
    res = 2407 + 5 * channel;
    if (channel == 14) /* IEEE Std 802.11-2012: 17.4.6.3 Channel Numbering of operating channels */
      res += 7;
  }
  else if (channel >= 36 && channel <= CHANNEL_THRESHOLD)
    res = 5000 + 5 * channel;
  else if (channel)
    res = 4000 + 5 * channel;

#undef CHANNEL_THRESHOLD
  return res;
}

static void acs_record_bsses(struct hostapd_iface *iface, struct wpa_scan_results *scan_res, FILE *fp)
{
	struct hostapd_hw_modes *mode = iface->current_mode;
	int i, j;

	wpa_printf(MSG_DEBUG, "ACS: recording BSS-es");

	/* init the counters */
	for (j = 0; j < mode->num_channels; j++) {
		struct hostapd_channel_data *chan = &mode->channels[j];
		chan->num_bss_pri = chan->num_bss_sec = chan->num_bss_tert = chan->num_bss_quat = 0;
		chan->num_bss_conflicting_20 = chan->num_bss_conflicting_ht40plus = chan->num_bss_conflicting_ht40minus = 0;
	}

	fprintf(fp, "# ssid                          \twidth\tprimary\tsecondary\tcenter0\tcenter1\n");

	for (i = 0; i < scan_res->num; i++) {
		struct wpa_scan_res *bss = scan_res->res[i];
		struct ieee802_11_elems elems;
		struct ieee80211_ht_capabilities *ht_cap = NULL;
		struct ieee80211_ht_operation *ht_oper = NULL;
		struct ieee80211_vht_operation *vht_oper = NULL;
		int bss_sec_freq = 0;
		int bss_center_freq0 = bss->freq;
		int bss_center_freq1 = 0;
		int bss_width = 0;
		const char *bss_printable_width = "20";

		ieee802_11_parse_elems((u8 *) (bss + 1), bss->ie_len, &elems, 0);

		/* get the HT capabilities element, needed later when we analyze each channel */
		if (elems.ht_capabilities)
			ht_cap = (struct ieee80211_ht_capabilities *) elems.ht_capabilities;

		/* get the secondary channel's frequency */
		if (elems.ht_operation) {
			ht_oper = (struct ieee80211_ht_operation *) elems.ht_operation;

			if (ht_oper->ht_param & HT_INFO_HT_PARAM_STA_CHNL_WIDTH) {
				int sec = ht_oper->ht_param & HT_INFO_HT_PARAM_SECONDARY_CHNL_OFF_MASK;
				if (sec == HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE) {
					bss_printable_width = "40";
					bss_sec_freq = bss->freq + 20;
				} else if (sec == HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW) {
					bss_printable_width = "40";
					bss_sec_freq = bss->freq - 20;
				}

				bss_center_freq0 = (bss->freq + bss_sec_freq) / 2;
			}
		}

		/* get the vht operational info if in 5 GHz band */
		if (mode->mode == HOSTAPD_MODE_IEEE80211A
		    && elems.vht_operation) {
			vht_oper = (struct ieee80211_vht_operation *) elems.vht_operation;
			bss_width = vht_oper->vht_op_info_chwidth;

			if (bss_width > CHANWIDTH_USE_HT) {
				bss_printable_width = bss_width == CHANWIDTH_80MHZ ? "80" : "160";

				bss_center_freq0 = 5000 + 5 * vht_oper->vht_op_info_chan_center_freq_seg0_idx;
				if (bss_width == CHANWIDTH_80P80MHZ) {
					bss_printable_width = "80+80";
					bss_center_freq1 = 5000 + 5 * vht_oper->vht_op_info_chan_center_freq_seg1_idx;
				}
			}
		}

		fprintf(fp, "%-32.*s\t%5s\t%7u\t%9u\t%7u\t%7u\n",
			elems.ssid_len, elems.ssid, bss_printable_width,
			acs_freq_to_chan(bss->freq),
			bss_sec_freq > 0 ? acs_freq_to_chan(bss_sec_freq) : 0,
			acs_freq_to_chan(bss_center_freq0),
			bss_center_freq1 > 0 ? acs_freq_to_chan(bss_center_freq1) : 0);

		/* increase the relevant counters in every channel affected */
		for (j = 0; j < mode->num_channels; j++) {
			struct hostapd_channel_data *chan = &mode->channels[j];

			if (chan->freq == bss->freq) {
				chan->num_bss_pri++;

				if (ht_cap
				    && (le_to_host16(ht_cap->ht_capabilities_info) & HT_CAP_INFO_40MHZ_INTOLERANT)) {
					chan->num_bss_conflicting_ht40minus++;
					chan->num_bss_conflicting_ht40plus++;
				} else if (bss_sec_freq == bss->freq + 20) {   /* bss has upper bonding */
					chan->num_bss_conflicting_ht40minus++; /* so we can't have lower */
				} else if (bss_sec_freq == bss->freq - 20) {   /* and vice versa */
					chan->num_bss_conflicting_ht40plus++;
				}
			} else {
				if (chan->freq == bss_sec_freq) { /* BSS's secondary channel is on current */
					chan->num_bss_sec++;
				} else if (bss_width == CHANWIDTH_80MHZ) {
					if (bss_center_freq0 - 40 + 10 <= chan->freq
					    && chan->freq <= bss_center_freq0 + 40 - 10) /* BSS's tertiary channel is on current */
						chan->num_bss_tert++;
				} else if (bss_width == CHANWIDTH_80P80MHZ) {
					if (bss_center_freq0 - 40 + 10 <= chan->freq
					    && chan->freq <= bss_center_freq0 + 40 - 10) /* BSS's tertiary channel is on current */
						chan->num_bss_tert++;
					if (bss_center_freq1 > 0
					    && bss_center_freq1 - 40 + 10 <= chan->freq
					    && chan->freq <= bss_center_freq1 + 40 - 10) /* BSS's quaternary channel is on current */
						chan->num_bss_quat++;
				}
				else if (bss_width == CHANWIDTH_160MHZ) {
					if (bss_center_freq0 - 80 + 10 <= chan->freq
					    && chan->freq <= bss_center_freq0 + 80 - 10) {
						/* BSS's tertiary or quaternary channel is on current */
						int side_pri = bss->freq < bss_center_freq0 ? -1 : 1;
						int side_chan = chan->freq < bss_center_freq0 ? -1 : 1;
						if (side_pri == side_chan)
							chan->num_bss_tert++;
						else
							chan->num_bss_quat++;
					}
				}

				if (chan->freq - 15 <= bss->freq && bss->freq <= chan->freq + 15)
					chan->num_bss_conflicting_20++;

				if ((chan->freq - 15 <= bss->freq && bss->freq <= chan->freq + 35)
				    || (chan->freq - 15 <= bss_sec_freq && bss_sec_freq <= chan->freq + 35))
					chan->num_bss_conflicting_ht40plus++;

				if ((chan->freq - 35 <= bss->freq && bss->freq <= chan->freq + 15)
				    || (chan->freq - 35 <= bss_sec_freq && bss_sec_freq <= chan->freq + 15))
					chan->num_bss_conflicting_ht40minus++;
			}
		}
	}
}

#define ACS_NUMBSS_TOO_MANY_STAS 100

static void acs_compute_badness(struct hostapd_iface *iface, FILE *fp)
{
	struct hostapd_hw_modes *mode = iface->current_mode;
	int *coefs = iface->conf->acs_numbss_coeflist;
	int j;

	wpa_printf(MSG_DEBUG, "ACS: computing badness");

	fprintf(fp, "\n# p_w1\tp_w2\tp_w3\tp_w4\ts_w1\ts_w2\ts_w3\ts_w4\tt_w1\tt_w2\tt_w3\tt_w4\tw20\tw40\tw80\n");
	fprintf(fp, "%6i\t%4i\t%4i\t%4i\t%4i\t%4i\t%4i\t%4i\t%4i\t%4i\t%4i\t%4i\t%3i\t%3i\t%3i\n",
		coefs[ACS_NUMBSS_P_W1], coefs[ACS_NUMBSS_P_W2],
		coefs[ACS_NUMBSS_P_W3], coefs[ACS_NUMBSS_P_W4],
		coefs[ACS_NUMBSS_S_W1], coefs[ACS_NUMBSS_S_W2],
		coefs[ACS_NUMBSS_S_W3], coefs[ACS_NUMBSS_S_W4],
		coefs[ACS_NUMBSS_T_W1], coefs[ACS_NUMBSS_T_W2],
		coefs[ACS_NUMBSS_T_W3], coefs[ACS_NUMBSS_T_W4],
		coefs[ACS_NUMBSS_W20], coefs[ACS_NUMBSS_W40], coefs[ACS_NUMBSS_W80]);

	/* compute the "badness" for 20 MHz */
	for (j = 0; j < mode->num_channels; j++) {
		struct hostapd_channel_data *chan = &mode->channels[j];

		chan->badness20 = -1;
		chan->badness40plus = -1;
		chan->badness40minus = -1;
		chan->badness80 = -1;

		if (chan->flag & HOSTAPD_CHAN_DISABLED)
			continue;

		/* avoid working with numbers that are too large */
		if (chan->num_bss_pri > ACS_NUMBSS_TOO_MANY_STAS)
			chan->num_bss_pri = ACS_NUMBSS_TOO_MANY_STAS;
		if (chan->num_bss_sec > ACS_NUMBSS_TOO_MANY_STAS)
			chan->num_bss_sec = ACS_NUMBSS_TOO_MANY_STAS;
		if (chan->num_bss_tert > ACS_NUMBSS_TOO_MANY_STAS)
			chan->num_bss_tert = ACS_NUMBSS_TOO_MANY_STAS;
		if (chan->num_bss_quat > ACS_NUMBSS_TOO_MANY_STAS)
			chan->num_bss_quat = ACS_NUMBSS_TOO_MANY_STAS;

		if (chan->num_bss_conflicting_20 == 0)
			chan->badness20 = (coefs[ACS_NUMBSS_P_W1] * chan->num_bss_pri
					   + coefs[ACS_NUMBSS_P_W2] * chan->num_bss_sec
					   + coefs[ACS_NUMBSS_P_W3] * chan->num_bss_tert
					   + coefs[ACS_NUMBSS_P_W4] * chan->num_bss_quat);
	}

	if (!iface->conf->ieee80211n
	    || !(iface->conf->ht_capab & HT_CAP_INFO_SUPP_CHANNEL_WIDTH_SET)) {
		wpa_printf(MSG_DEBUG, "ACS: skipping badness 40 computation (not configured)");
		return;
	}

	/* compute the "badness" for 40 MHz */
	for (j = 0; j < mode->num_channels; j++) {
		struct hostapd_channel_data *sec_chan_up, *sec_chan_down;
		struct hostapd_channel_data *chan = &mode->channels[j];

		if (chan->badness20 == -1) /* if it was no good for 20, it can't be good for 40 */
			continue;

		sec_chan_up = acs_find_chan(iface, chan->freq + 20);
		sec_chan_down = acs_find_chan(iface, chan->freq - 20);

		if (sec_chan_up
		    && (sec_chan_up->chan < 14 || acs_usable_ht40_chan(chan))
		    && chan->num_bss_conflicting_ht40plus == 0)
			chan->badness40plus = (chan->badness20
					       + coefs[ACS_NUMBSS_S_W1] * sec_chan_up->num_bss_pri
					       + coefs[ACS_NUMBSS_S_W2] * sec_chan_up->num_bss_sec
					       + coefs[ACS_NUMBSS_S_W3] * sec_chan_up->num_bss_tert
					       + coefs[ACS_NUMBSS_S_W4] * sec_chan_up->num_bss_quat);

		if (sec_chan_down
		    && (chan->chan < 14 || acs_usable_ht40_chan(sec_chan_down))
		    && chan->num_bss_conflicting_ht40minus == 0)
			chan->badness40minus = (chan->badness20
						+ coefs[ACS_NUMBSS_S_W1] * sec_chan_down->num_bss_pri
						+ coefs[ACS_NUMBSS_S_W2] * sec_chan_down->num_bss_sec
						+ coefs[ACS_NUMBSS_S_W3] * sec_chan_down->num_bss_tert
						+ coefs[ACS_NUMBSS_S_W4] * sec_chan_down->num_bss_quat);
	}

	if (mode->mode != HOSTAPD_MODE_IEEE80211A
	    || !iface->conf->ieee80211ac) {
		wpa_printf(MSG_DEBUG, "ACS: skipping badness 80 computation (wrong mode or not configured)");
		return;
	}

	/* compute the "badness" for 80 MHz */
	for (j = 0; j < mode->num_channels; j++) {
		struct hostapd_channel_data *chan1, *chan2, *chan3, *chan_1, *chan_2, *chan_3;
		struct hostapd_channel_data *chan = &mode->channels[j];

		/* we could retrieve the neighboring channels ourselves faster, but all the
		 * checks that need to be done are a pain. Therefore, we use the find function
		 * even though it slows the execution a little.
		 */
		chan1 = acs_find_chan(iface, chan->freq + 20);
		chan2 = acs_find_chan(iface, chan->freq + 40);
		chan3 = acs_find_chan(iface, chan->freq + 60);
		chan_1 = acs_find_chan(iface, chan->freq - 20);
		chan_2 = acs_find_chan(iface, chan->freq - 40);
		chan_3 = acs_find_chan(iface, chan->freq - 60);

		if (acs_usable_vht80_chan(chan)) { /* ours is the lowest chan in possible 80 MHz range */
			chan->prim_chan_idx = 0;

			if (chan->badness40plus == -1) /* if it was no good for 40+, it can't be good for 80 */
				continue;

			if (chan1 && chan2 && chan3
			    && chan->num_bss_conflicting_ht40plus == 0)
				chan->badness80 = (chan->badness40plus
						   + coefs[ACS_NUMBSS_T_W1] * (chan2->num_bss_pri + chan3->num_bss_pri)
						   + coefs[ACS_NUMBSS_T_W2] * (chan2->num_bss_sec + chan3->num_bss_sec)
						   + coefs[ACS_NUMBSS_T_W3] * (chan2->num_bss_tert + chan3->num_bss_tert)
						   + coefs[ACS_NUMBSS_T_W4] * (chan2->num_bss_quat + chan3->num_bss_quat));
		} else if (chan_1 && acs_usable_vht80_chan(chan_1)) { /* ours is the second chan in 80 Mhz */
			chan->prim_chan_idx = 1;

			if (chan->badness40minus == -1) /* if it was no good for 40-, it can't be good for 80 */
				continue;

			if (chan1 && chan2
			    && chan->num_bss_conflicting_ht40minus == 0)
				chan->badness80 = (chan->badness40minus
						   + coefs[ACS_NUMBSS_T_W1] * (chan1->num_bss_pri + chan2->num_bss_pri)
						   + coefs[ACS_NUMBSS_T_W2] * (chan1->num_bss_sec + chan2->num_bss_sec)
						   + coefs[ACS_NUMBSS_T_W3] * (chan1->num_bss_tert + chan2->num_bss_tert)
						   + coefs[ACS_NUMBSS_T_W4] * (chan1->num_bss_quat + chan2->num_bss_quat));
		} else if (chan_2 && acs_usable_vht80_chan(chan_2)) { /* ours is the third chan in 80 Mhz */
			chan->prim_chan_idx = 2;

			if (chan->badness40plus == -1) /* if it was no good for 40+, it can't be good for 80 */
				continue;

			if (chan_1 && chan1
			    && chan->num_bss_conflicting_ht40plus == 0)
				chan->badness80 = (chan->badness40plus
						   + coefs[ACS_NUMBSS_T_W1] * (chan_2->num_bss_pri + chan_1->num_bss_pri)
						   + coefs[ACS_NUMBSS_T_W2] * (chan_2->num_bss_sec + chan_1->num_bss_sec)
						   + coefs[ACS_NUMBSS_T_W3] * (chan_2->num_bss_tert + chan_1->num_bss_tert)
						   + coefs[ACS_NUMBSS_T_W4] * (chan_2->num_bss_quat + chan_1->num_bss_quat));
		} else if (chan_3 && acs_usable_vht80_chan(chan_3)) { /* ours is the last chan in 80 Mhz */
			chan->prim_chan_idx = 3;

			if (chan->badness40minus == -1) /* if it was no good for 40-, it can't be good for 80 */
				continue;

			if (chan_1 && chan_2
			    && chan->num_bss_conflicting_ht40minus == 0)
				chan->badness80 = (chan->badness40minus
						   + coefs[ACS_NUMBSS_T_W1] * (chan_3->num_bss_pri + chan_2->num_bss_pri)
						   + coefs[ACS_NUMBSS_T_W2] * (chan_3->num_bss_sec + chan_2->num_bss_sec)
						   + coefs[ACS_NUMBSS_T_W3] * (chan_3->num_bss_tert + chan_2->num_bss_tert)
						   + coefs[ACS_NUMBSS_T_W4] * (chan_3->num_bss_quat + chan_2->num_bss_quat));
		}
	}
}

static void acs_numbss_adjust_vht_center_freq(struct hostapd_iface *iface, unsigned prim_chan_index)
{
	int offset;

	wpa_printf(MSG_DEBUG, "ACS NUMBSS: Adjusting VHT center frequency");

	switch (iface->conf->vht_oper_chwidth) {
	case CHANWIDTH_USE_HT:
		offset = 2 * iface->conf->secondary_channel;
		break;
	case CHANWIDTH_80MHZ:
		offset = 6 - (prim_chan_index * 4);
		break;
	default:
		/* TODO: How can this be calculated? Adjust
		 * acs_find_ideal_chan() */
		wpa_printf(MSG_INFO, "ACS NUMBSS: Only VHT20/40/80 is supported now");
		return;
	}

	iface->conf->vht_oper_centr_freq_seg0_idx =
		iface->conf->channel + offset;
}

static void acs_smart_adjust_vht_center_freq(struct hostapd_iface *iface, unsigned center_chan_index)
{
  wpa_printf(MSG_DEBUG, "ACS SMART: Adjusting VHT center frequency");

  switch (iface->conf->vht_oper_chwidth) {
  case CHANWIDTH_USE_HT:
    iface->conf->vht_oper_centr_freq_seg0_idx = iface->conf->channel + 2 * iface->conf->secondary_channel;
    break;
  case CHANWIDTH_80MHZ:
    iface->conf->vht_oper_centr_freq_seg0_idx =
      center_chan_index ? center_chan_index : acs_get_center_chan_index(iface);
    break;
  case CHANWIDTH_160MHZ:
    if (center_chan_index)
      iface->conf->vht_oper_centr_freq_seg0_idx = center_chan_index;
    else
      wpa_printf(MSG_ERROR, "ACS SMART: adjust VHT freq: must supply center channel for 160MHz");
    break;
  default:
    wpa_printf(MSG_INFO, "ACS SMART: Only VHT20/40/80/160 is supported at the moment");
    return;
  }
}

static void acs_find_min_badness(struct hostapd_iface *iface, FILE *fp)
{
	struct hostapd_hw_modes *mode = iface->current_mode;
	int *coefs = iface->conf->acs_numbss_coeflist;
	struct hostapd_channel_data *chan20 = NULL;
	struct hostapd_channel_data *chan40plus = NULL;
	struct hostapd_channel_data *chan40minus = NULL;
	struct hostapd_channel_data *chan80 = NULL;
	unsigned prim_chan_idx = 0;
	int min20 = -1;
	int min40plus = -1;
	int min40minus = -1;
	int min40 = -1;
	int min80 = -1;
	int n_chans = 1;
	int j;

	wpa_printf(MSG_DEBUG, "ACS: finding minimum badness");

	fprintf(fp, "\n# channel\t flags\tnum_bss1\tnum_bss2\tnum_bss3\tnum_bss4"
		"\tnum_confl20\tnum_confl40+\tnum_confl40-\tbadness20\tbadness40+\tbadness40-\tbadness80\n");

	for (j = 0; j < mode->num_channels; j++) {
		struct hostapd_channel_data *chan = &mode->channels[j];

		fprintf(fp, "%9i\t0x%04x\t%8u\t%8u\t%8u\t%8u\t%11u\t%12u\t%12u\t%9i\t%10i\t%10i\t%9i\n",
			chan->chan, chan->flag,
			chan->num_bss_pri, chan->num_bss_sec, chan->num_bss_tert, chan->num_bss_quat,
			chan->num_bss_conflicting_20, chan->num_bss_conflicting_ht40plus, chan->num_bss_conflicting_ht40minus,
			chan->badness20, chan->badness40plus, chan->badness40minus, chan->badness80);

		if ((unsigned) chan->badness20 < (unsigned) min20) {
			min20 = chan->badness20;
			chan20 = chan;
		}

		if ((unsigned) chan->badness40plus < (unsigned) min40plus) {
			min40plus = chan->badness40plus;
			chan40plus = chan;
		}

		if ((unsigned) chan->badness40minus < (unsigned) min40minus) {
			min40minus = chan->badness40minus;
			chan40minus = chan;
		}

		if ((unsigned) chan->badness80 < (unsigned) min80) {
			min80 = chan->badness80;
			chan80 = chan;
		}
	}

	fprintf(fp, "\n# best20\tmin_badness20\tbest40+\tmin_badness40+\tbest40-\tmin_badness40-\tbest80\tmin_badness80\n");
	fprintf(fp, "%8hi\t%13i\t%7hi\t%14i\t%7hi\t%14i\t%6hi\t%13i\n",
		chan20 ? chan20->chan : 0, min20,
		chan40plus ? chan40plus->chan : 0, min40plus,
		chan40minus ? chan40minus->chan : 0, min40minus,
		chan80 ? chan80->chan : 0, min80);

	wpa_printf(MSG_DEBUG, "ACS: best20: %hi (badness %i)", chan20 ? chan20->chan : 0, min20);
	wpa_printf(MSG_DEBUG, "ACS: best40+: %hi (badness %i)", chan40plus ? chan40plus->chan : 0, min40plus);
	wpa_printf(MSG_DEBUG, "ACS: best40-: %hi (badness %i)", chan40minus ? chan40minus->chan : 0, min40minus);
	wpa_printf(MSG_DEBUG, "ACS: best80: %hi (badness %i)", chan80 ? chan80->chan : 0, min80);

	/* Now we choose between 80, 40 and 20 MHz */
	min40 = (unsigned) min40minus < (unsigned) min40plus ? min40minus : min40plus;

	if (iface->conf->ieee80211n
	    && iface->conf->secondary_channel
	    && min40 != -1)
		n_chans = 2;

	if (mode->mode == HOSTAPD_MODE_IEEE80211A
	    && iface->conf->ieee80211ac
      && iface->conf->vht_oper_chwidth > CHANWIDTH_80MHZ) {
		wpa_printf(MSG_ERROR, "ACS: currently there is no support for 160 MHz or 80+80 MHz modes, considering 80");
		iface->conf->vht_oper_chwidth = CHANWIDTH_80MHZ;
	}

	if (mode->mode == HOSTAPD_MODE_IEEE80211A
	    && iface->conf->ieee80211ac
      && iface->conf->vht_oper_chwidth > CHANWIDTH_USE_HT
	    && min40 != -1
	    && min80 != -1)
		n_chans = 4;

	if (n_chans == 4) {
		if (coefs[ACS_NUMBSS_W40] * min40 < coefs[ACS_NUMBSS_W80] * min80) {
			wpa_printf(MSG_DEBUG, "ACS: 40 MHz badness %i superior to 80 MHz badness %i: switching to 40 MHz",
				   min40, min80);
			n_chans = 2;
		} else if (coefs[ACS_NUMBSS_W20] * min20 < coefs[ACS_NUMBSS_W80] * min80) {
			wpa_printf(MSG_DEBUG, "ACS: 20 MHz badness %i superior to 80 MHz badness %i: switching to 20 MHz",
				   min20, min80);
			n_chans = 1;
		}
	}

	if (n_chans == 2
		&& coefs[ACS_NUMBSS_W20] * min20 < coefs[ACS_NUMBSS_W40] * min40) {
		wpa_printf(MSG_DEBUG, "ACS: 20 MHz badness %i superior to 40 MHz badness %i: switching to 20 MHz", min20, min40);
		n_chans = 1;
	}

	if (n_chans == 4) {
		if (chan80 != NULL) {
			iface->conf->channel = chan80->chan;
			iface->conf->secondary_channel = (chan80->prim_chan_idx & 0x01) ? -1 : 1;
			prim_chan_idx = chan80->prim_chan_idx;
			wpa_printf(MSG_DEBUG, "ACS: Chose 80 MHz bandwidth with primary channel %hi (badness %i)", chan80->chan, min80);
		}
		else {
			wpa_printf(MSG_DEBUG, "ACS: Can't chose 80 MHz channel due to missed channel data");
			return;
		}
	} else if (n_chans == 2) {
		if (((min40plus == min40) && (chan40plus != NULL)) || ((min40plus != min40) && (chan40minus != NULL))) {
			iface->conf->vht_oper_chwidth = CHANWIDTH_USE_HT;

			if (min40plus == min40) {
				iface->conf->channel = chan40plus->chan;
				iface->conf->secondary_channel = 1;
			} else {
				iface->conf->channel = chan40minus->chan;
				iface->conf->secondary_channel = -1;
			}

			wpa_printf(MSG_DEBUG, "ACS: Chose 40 MHz bandwidth with primary channel %hi (badness %i, %s bonding)",
				   iface->conf->channel, min40, iface->conf->secondary_channel > 0 ? "upper" : "lower");
		}
		else {
			wpa_printf(MSG_DEBUG, "ACS: Can't chose 40 MHz channel due to missed channel data");
			return;
		}
	} else if (chan20) {
		iface->conf->vht_oper_chwidth = CHANWIDTH_USE_HT;
		iface->conf->channel = chan20->chan;
		iface->conf->secondary_channel = 0;
		wpa_printf(MSG_DEBUG, "ACS: Chose 20 MHz bandwidth on channel %hi (badness %i)", chan20->chan, min20);
	} else {
		for (j = 0; j < mode->num_channels; j++) {
			struct hostapd_channel_data *chan = &mode->channels[j];

			if (!(chan->flag & HOSTAPD_CHAN_DISABLED))
				break;
		}

		if (j >= mode->num_channels) {
			wpa_printf(MSG_ERROR, "ACS: every channel disabled");
			j = 0;
		}

		iface->conf->channel = mode->channels[j].chan;
		iface->conf->vht_oper_chwidth = CHANWIDTH_USE_HT;
		iface->conf->secondary_channel = 0;
		wpa_printf(MSG_ERROR, "ACS: NumBSS-based approach failed, picking channel %hi", iface->conf->channel);
	}

	acs_numbss_adjust_vht_center_freq(iface, prim_chan_idx);
}

static FILE* acs_fopen(const char *fname, Boolean append, const char *name)
{
	FILE *fp = acs_write_file(fname, append);

	if (!fp) {
		wpa_printf(MSG_ERROR, "Error opening the %s '%s': %s",
			   name, fname, strerror(errno));
		return stderr;
	}
	else
		return fp;
}

static void acs_fclose(FILE *fp, const char *name)
{
	if (fp != stderr) {
		if (fclose(fp))	wpa_printf(MSG_ERROR, "Error closing the %s: %s", name, strerror(errno));
	}
}

static void acs_count_bsses(struct hostapd_iface *iface, struct wpa_scan_results *scan_res)
{
	FILE *fp = acs_fopen(iface->conf->acs_numbss_info_file, FALSE, "ACS NUMBSS info file");

	acs_record_bsses(iface, scan_res, fp);

	acs_compute_badness(iface, fp);

	acs_find_min_badness(iface, fp);

	acs_fclose(fp, "ACS NUMBSS info file");

	/* hostapd_setup_interface_complete() will return -1 on failure and 0 on success */
	if (hostapd_acs_completed(iface, 0) == 0) {
		acs_cleanup(iface);
		return;
	}

	/* If we're here then somehow ACS chose an invalid channel */
	wpa_printf(MSG_ERROR, "ACS: NUMBSS procedure failed. If reporting, please include your config file and info file '%s'.", iface->conf->acs_numbss_info_file);
	acs_fail(iface);
}

Boolean channels_overlap(u32 freq1, u32 width1, u32 freq2, u32 width2)
{

#define SINGLE_CHANNEL_WIDTH 20

  /* convert the first channel's center frequency to the start of frequency range */
  freq1 -= SINGLE_CHANNEL_WIDTH >> 1;
  freq2 -= SINGLE_CHANNEL_WIDTH >> 1;

  if ((freq2 >= freq1) && (freq1 + width1 > freq2)) return TRUE;
  if ((freq1 >= freq2) && (freq2 + width2 > freq1)) return TRUE;
  return FALSE;
}

Boolean is_vht_or_he(struct hostapd_iface *iface)
{
  if ((iface->conf->ieee80211ac) || (iface->conf->ieee80211ax)) {
    return TRUE;
  }
  return FALSE;
}

void acs_smart_record_bsses(struct hostapd_iface *iface, struct wpa_scan_results *scan_res, FILE *fp)
{
  struct hostapd_hw_modes *mode = iface->current_mode;
  struct acs_candidate_table *candidates = iface->candidates;
  int i, j;

  wpa_printf(MSG_DEBUG, "ACS_SMART: recording BSS-es");

  /* init the counters */
  for (j = 0; j < mode->num_channels; j++) {
    struct hostapd_channel_data *chan = &mode->channels[j];
    chan->num_bss_pri = chan->num_bss_sec = chan->num_bss_tert = chan->num_bss_quat = 0;
    chan->num_bss_conflicting_20 = chan->num_bss_conflicting_ht40plus = chan->num_bss_conflicting_ht40minus = 0;
  }
  iface->acs_num_bss = 0;

  for (j = 0; j < iface->num_candidates; j++) {
    candidates[j].num_bss = candidates[j].num_bss_pri = candidates[j].num_adjacent =
      candidates[j].overlap40 = candidates[j].overlap80 = candidates[j].overlap160 =
      candidates[j].primary_on_overlap_4080160 = candidates[j].overlap_vht_he =
      candidates[j].intolerant40 = 0;
  }

  fprintf(fp, "\n# ssid                          \twidth\tprimary\tsecondary\tcenter0\tcenter1\trssi\n");

  for (i = 0; i < scan_res->num; i++) {
    struct wpa_scan_res *bss = scan_res->res[i];
    struct ieee802_11_elems elems;
    struct ieee80211_ht_capabilities *ht_cap = NULL;
    struct ieee80211_ht_operation *ht_oper = NULL;
    struct ieee80211_vht_operation *vht_oper = NULL;
    int bss_sec_freq = 0;
    int bss_center_freq0 = bss->freq;
    int bss_center_freq1 = 0;
    int bss_width = 0;
    int bss_base_freq;
    int num_width = 20;
    const char *bss_printable_width = "20";

    if (!acs_find_chan(iface, bss->freq))
      continue;

    ieee802_11_parse_elems((u8 *)(bss + 1), bss->ie_len, &elems, 0);

    /* get the HT capabilities element, needed later when we analyze each channel */
    if (elems.ht_capabilities)
      ht_cap = (struct ieee80211_ht_capabilities *) elems.ht_capabilities;

    /* get the secondary channel's frequency */
    if (elems.ht_operation) {
      ht_oper = (struct ieee80211_ht_operation *) elems.ht_operation;

      if (ht_oper->ht_param & HT_INFO_HT_PARAM_STA_CHNL_WIDTH) {
        int sec = ht_oper->ht_param & HT_INFO_HT_PARAM_SECONDARY_CHNL_OFF_MASK;
        if (sec == HT_INFO_HT_PARAM_SECONDARY_CHNL_ABOVE) {
          bss_printable_width = "40";
          num_width = 40;
          bss_sec_freq = bss->freq + 20;
        }
        else if (sec == HT_INFO_HT_PARAM_SECONDARY_CHNL_BELOW) {
          bss_printable_width = "40";
          bss_sec_freq = bss->freq - 20;
          num_width = 40;
        }
        bss_center_freq0 = (bss->freq + bss_sec_freq) / 2;
      }
    }

    /* get the vht operational info if in 5 GHz band */
    if (mode->mode == HOSTAPD_MODE_IEEE80211A && elems.vht_operation) {
      vht_oper = (struct ieee80211_vht_operation *) elems.vht_operation;
      bss_width = vht_oper->vht_op_info_chwidth;

      if (bss_width > CHANWIDTH_USE_HT) {
        bss_printable_width = bss_width == CHANWIDTH_80MHZ ? "80" : "160";
        num_width = (bss_width == CHANWIDTH_80MHZ) ? 80 : 160;

        bss_center_freq0 = 5000 + 5 * vht_oper->vht_op_info_chan_center_freq_seg0_idx;
        if (bss_width == CHANWIDTH_80P80MHZ) {
          bss_printable_width = "80+80";
          bss_center_freq1 = 5000 + 5 * vht_oper->vht_op_info_chan_center_freq_seg1_idx;
          num_width = 160;
        }
      }
    }

    fprintf(fp, "%-32.*s\t%5s\t%7u\t%9u\t%7u\t%7u\t%4d\n",
      elems.ssid_len, elems.ssid, bss_printable_width,
      acs_freq_to_chan(bss->freq),
      bss_sec_freq > 0 ? acs_freq_to_chan(bss_sec_freq) : 0,
      acs_freq_to_chan(bss_center_freq0),
      bss_center_freq1 > 0 ? acs_freq_to_chan(bss_center_freq1) : 0,
      bss->level);

    bss_base_freq = bss_center_freq0 - (num_width >> 1) + (SINGLE_CHANNEL_WIDTH >> 1);

    /* increase the relevant counters in every channel affected */
    for (j = 0; j < iface->num_candidates; j++)
    {
      if (channels_overlap(candidates[j].freq, candidates[j].width, bss_base_freq, num_width)) {

        Boolean primary_ch_overlap = channels_overlap(acs_chan_to_freq(candidates[j].primary), 20, bss_base_freq, num_width);
        if (primary_ch_overlap) {
          candidates[j].num_bss_pri++;
        }

        candidates[j].num_bss++;

        /* Common case for 40, 80, 160 - just increase counters */
        if (bss_width == CHANWIDTH_USE_HT) {
          if (bss_sec_freq) {
            if (primary_ch_overlap) {
              wpa_printf(MSG_DEBUG, "pri_overlap2040 - %d + %d vs. %d + %d , bss2 = %d overlaps ?", candidates[j].freq, 20, bss_base_freq, num_width, bss_sec_freq);
              candidates[j].primary_on_overlap_4080160++;
            }
          }
        } else if (bss_width == CHANWIDTH_80MHZ) {
          candidates[j].overlap80++;
          os_get_reltime(&candidates[j].ts_overlap80);
          if (primary_ch_overlap) {
            candidates[j].primary_on_overlap_4080160++;
            wpa_printf(MSG_DEBUG, "pri_overlap80 - %d + %d vs. %d + %d , bss2 = %d overlaps ?", candidates[j].freq, 20, bss_base_freq, num_width, bss_sec_freq);
          }
        } else if (bss_width == CHANWIDTH_160MHZ) { /* 80+80 or 160 */
          candidates[j].overlap160++;
          os_get_reltime(&candidates[j].ts_overlap160);
          if (primary_ch_overlap) {
            candidates[j].primary_on_overlap_4080160++;
            wpa_printf(MSG_DEBUG, "pri_overlap160 - %d + %d vs. %d + %d , bss2 = %d overlaps ?", candidates[j].freq, 20, bss_base_freq, num_width, bss_sec_freq);
          }
        }

        if (!ht_cap &&
            (candidates[j].primary != acs_freq_to_chan(bss->freq)) &&
            (candidates[j].width == 40)) {
          os_get_reltime(&candidates[j].ts_legacy);
        }

        if (iface->conf->obss_interval) { /* Check if 20/40 CoEx enabled */
          if (candidates[j].width == 40) { /* 20/40 rules */
            if (((bss_width == CHANWIDTH_USE_HT) && bss_sec_freq && /* pri and sec must be equal */
              ((acs_chan_to_freq(candidates[j].primary) != bss->freq) ||
                (acs_chan_to_freq(candidates[j].secondary) != bss_sec_freq))) ||
              (!bss_sec_freq && acs_chan_to_freq(candidates[j].primary) != bss->freq))
            { /* primary channel must be the same as the operating channel of all 20MHz BSSs */
              if (iface->conf->obss_beacon_rssi_threshold < bss->level) {
                candidates[j].overlap40++;
                os_get_reltime(&candidates[j].ts_overlap40);
              }
            }
          }

          if (ht_cap &&
              (le_to_host16(ht_cap->ht_capabilities_info) & HT_CAP_INFO_40MHZ_INTOLERANT) &&
              candidates[j].secondary) {
            candidates[j].intolerant40++;
            os_get_reltime(&candidates[j].ts_intolerant40);
          }
        } else {
          /* If no CoEx enabled - check relevant candidates for overlap rules */
          if (primary_ch_overlap && (candidates[j].width >= 40) &&
              (iface->conf->obss_beacon_rssi_threshold < bss->level)) {
            if (is_vht_or_he(iface)) {
              /* Primary must not be equal for VHT and HE. If not - mark candidate and exclude later */
              if (acs_chan_to_freq(candidates[j].primary) == bss->freq) {
                candidates[j].overlap_vht_he++;
                os_get_reltime(&candidates[j].ts_overlap_vht_he);
              }
            } else {
              /* Primary must be equal for HT. If not - mark candidate and exclude later */
              if (acs_chan_to_freq(candidates[j].primary) != bss->freq) {
                candidates[j].overlap40++;
                os_get_reltime(&candidates[j].ts_overlap40);
              }
            }
          }
        }
      } /* overlaps */

      candidates[j].num_adjacent = candidates[j].num_bss - candidates[j].num_bss_pri;
    } /* for - candidates */

    iface->acs_num_bss++;
  }

  iface->conf->acs_init_done = 1;

  fprintf(fp, "\nnum_bss = %d\n\n", iface->acs_num_bss);
}

static void acs_smart_process_bsses(struct hostapd_iface *iface, struct wpa_scan_results *scan_res)
{
  FILE *fp = acs_fopen(iface->conf->acs_history_file, iface->conf->acs_init_done ? TRUE : FALSE, "ACS history file");

  acs_smart_record_bsses(iface, scan_res, fp);

  acs_fclose(fp, "ACS history file");

  acs_update_radar(iface);
  acs_recalc_ranks_and_set_chan(iface, SWR_INITIAL);

  /* hostapd_setup_interface_complete() will return -1 on failure and 0 on success */
  if (hostapd_acs_completed(iface, 0) == 0)
  {
    acs_cleanup(iface);
    return;
  }

  /* If we're here then somehow ACS chose an invalid channel */
  wpa_printf(MSG_ERROR, "SmartACS: BSS procedure failed. If reporting, please include your config file and info file '%s'.", iface->conf->acs_smart_info_file);
  acs_fail(iface);
}

static void acs_bg_scan_complete(struct hostapd_iface *iface)
{
  struct wpa_scan_results *scan_res = NULL;
  FILE *fp;
  acs_chandef cur_chan;

  if (iface->acs_update_in_progress) {
    wpa_printf(MSG_INFO, "ACS Update already in progress");
    return;
  }

  iface->in_scan = 0;
  iface->acs_update_in_progress = 1;

  scan_res = hostapd_driver_get_scan_results(iface->bss[0]);

  if (scan_res == NULL) {
    wpa_printf(MSG_ERROR, "ACS: Failed to get BSS data from BG scan");
    iface->acs_update_in_progress = 0;
    return;
  }

  acs_push_chandef(iface, &cur_chan);

  wpa_printf(MSG_INFO, "BSS data from BG scan received");
  fp = acs_fopen(iface->conf->acs_history_file, TRUE, "ACS history file");

  acs_smart_record_bsses(iface, scan_res, fp);
  wpa_scan_results_free(scan_res);

  acs_fclose(fp, "ACS history file");

  if (acs_recalc_ranks_and_set_chan(iface, SWR_BG_SCAN)) {
    if (acs_do_switch_channel(iface, 0) < 0) {
      /* back out with channel switch */
      acs_pop_chandef(iface, &cur_chan);
    }
  }

  iface->acs_update_in_progress = 0;
  return;
}

static void acs_scan_complete(struct hostapd_iface *iface)
{
	struct wpa_scan_results *scan_res = NULL;
	int err;

        iface->in_scan = 0;

        /* Change the scan callback function for BG scan results */
        iface->scan_cb = acs_bg_scan_complete;

	if (iface->conf->acs_algo == ACS_ALGO_SURVEY) {
		wpa_printf(MSG_DEBUG, "ACS: Using survey based algorithm (acs_num_scans=%d)",
			   iface->conf->acs_num_scans);

		err = hostapd_drv_get_survey(iface->bss[0], 0);
		if (err) {
			wpa_printf(MSG_ERROR, "ACS: Failed to get survey data");
			goto fail;
		}
	} else if ((iface->conf->acs_algo == ACS_ALGO_NUMBSS) || (iface->conf->acs_algo == ACS_ALGO_SMART)) {
                wpa_printf(MSG_DEBUG, "ACS: Using BSS based algorithm (acs_num_scans=%d)",
			   iface->conf->acs_num_scans);

		scan_res = hostapd_driver_get_scan_results(iface->bss[0]);
		if (scan_res == NULL) {
			wpa_printf(MSG_ERROR, "ACS: Failed to get BSS data");
			goto fail;
		}
	}

	if (++iface->acs_num_completed_scans < iface->conf->acs_num_scans) {
		err = acs_request_scan(iface);
		if (err) {
			wpa_printf(MSG_ERROR, "ACS: Failed to request scan");
			goto fail;
		}

		wpa_scan_results_free(scan_res);
		return;
	}

        switch (iface->conf->acs_algo) {

        case ACS_ALGO_SURVEY:
                acs_study(iface);
                break;

        case ACS_ALGO_NUMBSS:
                acs_count_bsses(iface, scan_res);
                break;

        case ACS_ALGO_SMART:
                acs_smart_process_bsses(iface, scan_res);
                break;

        default:
                wpa_printf(MSG_ERROR, "ACS: unknown algo");
	}

	wpa_scan_results_free(scan_res);
	return;
fail:
	wpa_scan_results_free(scan_res);
	hostapd_acs_completed(iface, 1);
	acs_fail(iface);
}


typedef struct {
    u32 primary;
    u32 secondary;
} channel_pair;

channel_pair channel_pairs_40_24G[] = { { 1,  5 },
                                       {  5,  1 },
                                       {  2,  6 },
                                       {  6,  2 },
                                       {  3,  7 },
                                       {  7,  3 },
                                       {  4,  8 },
                                       {  8,  4 },
                                       {  5,  9 },
                                       {  9,  5 },
                                       {  6, 10 },
                                       { 10,  6 },
                                       {  7, 11 },
                                       { 11,  7 },
                                       {  8, 12 },
                                       { 12,  8 },
                                       {  9, 13 },
                                       { 13,  9 } };

channel_pair channel_pairs_40_5G[] = { {  36, 40 },
                                       {  40, 36 },
                                       {  44, 48 },
                                       {  48, 44 },
                                       {  52, 56 },
                                       {  56, 52 },
                                       {  60, 64 },
                                       {  64, 60 },
                                       { 100,104 },
                                       { 104,100 },
                                       { 108,112 },
                                       { 112,108 },
                                       { 116,120 },
                                       { 120,116 },
                                       { 124,128 },
                                       { 128,124 },
                                       { 132,136 },
                                       { 136,132 },
                                       { 140,144 },
                                       { 144,140 },
                                       { 149,153 },
                                       { 153,149 },
                                       { 157,161 },
                                       { 161,157 } };


typedef struct {
    u32 chan_no;
    u32 primary;
    u32 secondary;
    u32 freq;
} channel_80;

channel_80 channels80[] = { {  42,  36, 40, 5000 + 36 * 5 },
                            {  42,  40, 36, 5000 + 36 * 5 },
                            {  42,  44, 48, 5000 + 36 * 5 },
                            {  42,  48, 44, 5000 + 36 * 5 },

                            {  58,  52, 56, 5000 + 52 * 5 },
                            {  58,  56, 52, 5000 + 52 * 5 },
                            {  58,  60, 64, 5000 + 52 * 5 },
                            {  58,  64, 60, 5000 + 52 * 5 },

                            { 106, 100, 104, 5000 + 100 * 5 },
                            { 106, 104, 100, 5000 + 100 * 5 },
                            { 106, 108, 112, 5000 + 100 * 5 },
                            { 106, 112, 108, 5000 + 100 * 5 },

                            { 122, 116, 120, 5000 + 116 * 5 },
                            { 122, 120, 116, 5000 + 116 * 5 },
                            { 122, 124, 128, 5000 + 116 * 5 },
                            { 122, 128, 124, 5000 + 116 * 5 },

                            { 138, 132, 136, 5000 + 132 * 5 },
                            { 138, 136, 132, 5000 + 132 * 5 },
                            { 138, 140, 144, 5000 + 132 * 5 },
                            { 138, 144, 140, 5000 + 132 * 5 },

                            { 155, 149, 153, 5000 + 149 * 5 },
                            { 155, 153, 149, 5000 + 149 * 5 },
                            { 155, 157, 161, 5000 + 149 * 5 },
                            { 155, 161, 157, 5000 + 149 * 5 } };

typedef struct {
  u32 chan_no;
  u32 primary;
  u32 secondary;
  u32 freq;
} channel_160;

channel_160 channels160[] = { {  50,  36,  40, 5000 +  36 * 5 },
                              {  50,  40,  36, 5000 +  36 * 5 },
                              {  50,  44,  48, 5000 +  36 * 5 },
                              {  50,  48,  44, 5000 +  36 * 5 },
                              {  50,  52,  56, 5000 +  36 * 5 },
                              {  50,  56,  52, 5000 +  36 * 5 },
                              {  50,  60,  64, 5000 +  36 * 5 },
                              {  50,  64,  60, 5000 +  36 * 5 },

                              { 114, 100, 104, 5000 + 100 * 5 },
                              { 114, 104, 100, 5000 + 100 * 5 },
                              { 114, 108, 112, 5000 + 100 * 5 },
                              { 114, 112, 108, 5000 + 100 * 5 },
                              { 114, 116, 120, 5000 + 100 * 5 },
                              { 114, 120, 116, 5000 + 100 * 5 },
                              { 114, 124, 128, 5000 + 100 * 5 },
                              { 114, 128, 124, 5000 + 100 * 5 } };

/* Gets secondary channel offset and VHT operational center frequency segment 0 index
 * from 40 MHz, 80 MHz and 160 MHz ACS candidate tables for 5 GHz band */
int acs_get_chan_data_from_candidates_5g (int primary_chan, u8 ch_width,
					  int *secondary_chan_offset, u8 *vht_oper_centr_freq_seg0_idx)
{
	int i;

	if (ch_width == CHANWIDTH_USE_HT) { /* 40 MHz */
		for (i = 0; i < sizeof(channel_pairs_40_5G)/sizeof(channel_pairs_40_5G[0]); i++) {
			if (channel_pairs_40_5G[i].primary == primary_chan) {
				*secondary_chan_offset = channel_pairs_40_5G[i].secondary > channel_pairs_40_5G[i].primary ? 1 : -1;
				*vht_oper_centr_freq_seg0_idx = channel_pairs_40_5G[i].primary + 2 * (*secondary_chan_offset);
				return 0;
			}
		}
	}
	else if (ch_width == CHANWIDTH_80MHZ){ /*80 MHz*/
		for (i = 0; i < sizeof(channels80)/sizeof(channels80[0]); i++) {
			if (channels80[i].primary == primary_chan) {
				*secondary_chan_offset = channels80[i].secondary > channels80[i].primary ? 1 : -1;
				*vht_oper_centr_freq_seg0_idx = channels80[i].chan_no;
				return 0;
			}
		}
	}
	else if (ch_width == CHANWIDTH_160MHZ){ /*160 MHz*/
		for (i = 0; i < sizeof(channels160)/sizeof(channels160[0]); i++) {
			if (channels160[i].primary == primary_chan) {
				*secondary_chan_offset = channels160[i].secondary > channels160[i].primary ? 1 : -1;
				*vht_oper_centr_freq_seg0_idx = channels160[i].chan_no;
				return 0;
			}
		}
	}

	wpa_printf(MSG_ERROR, "Channel %d width %d not found in 5 GHz ACS candidates table", primary_chan, ch_width);
	return 1;
}

static u32 acs_get_center_chan_index(struct hostapd_iface *iface)
{
  int i;
  u32 primary, secondary;

  if (iface->conf->vht_oper_chwidth != CHANWIDTH_80MHZ)
    return 0;

  primary   = iface->conf->channel;
  secondary = iface->conf->secondary_channel ? iface->conf->channel + iface->conf->secondary_channel * 4 : 0;

  for(i = 0; i < sizeof(channels80)/sizeof(channels80[0]); i++)
    if (channels80[i].primary == primary &&
      channels80[i].secondary == secondary)
      return channels80[i].chan_no;

  wpa_printf(MSG_ERROR, "didn't find center channel index from primary channel "
    "%d and secondary channel %d", primary, secondary);
  return 0;
}

static int acs_is_subchannel_allowed(struct hostapd_iface *iface, int channel)
{
  int j;
  struct hostapd_hw_modes *mode = iface->current_mode;

  for (j = 0; j < mode->num_channels; j++) {
    if (mode->channels[j].flag & HOSTAPD_CHAN_DISABLED)
      continue;
    if (mode->channels[j].chan == channel) return 1;
  }

  return 0;
}

static int acs_is_channel_disabled(struct hostapd_iface *iface, int freq, int width)
{
  int i, j;
  struct hostapd_hw_modes *mode = iface->current_mode;

  for (j = 0; j < mode->num_channels; j++) {
    struct hostapd_channel_data *chan = &mode->channels[j];

    for (i = 0; i < (width / SINGLE_CHANNEL_WIDTH); i++) {
      if (chan->freq == freq + SINGLE_CHANNEL_WIDTH * i)
      {
        if (chan->flag & HOSTAPD_CHAN_DISABLED) return 1;

        switch (width) {
          case 40: /* TODO: check for HT40 flags
                   if (chan->flag & HOSTAPD_CHAN_NO_HT40_MINUS)) return 1;*/
                   break;

          case 80: if (chan->flag & HOSTAPD_CHAN_NO_80MHZ) return 1;
                   break;

          case 160:if (chan->flag & HOSTAPD_CHAN_NO_160MHZ) return 1;
                   break;
        }
      }
    }
  }

  return 0;
}

static int acs_is_radar_affected_channel(struct hostapd_iface *iface, int channel)
{
  int j;
  struct hostapd_hw_modes *mode = iface->current_mode;

  for (j = 0; j < mode->num_channels; j++) {
    if (mode->channels[j].chan == channel) {
      if (mode->channels[j].flag & HOSTAPD_CHAN_RADAR)
        return 1;
      else return 0;
    }
  }

  return 0;
}

static int acs_is_40_mhz_overlap_chan (channel_pair *candidate)
{
  if ((candidate->primary == 1  && candidate->secondary == 5)  ||
      (candidate->primary == 5  && candidate->secondary == 1)  ||
      (candidate->primary == 7  && candidate->secondary == 11) ||
      (candidate->primary == 11 && candidate->secondary == 7))
    return 0;

  return 1;
}

void add_channel_pair_candidates(struct hostapd_iface *iface, channel_pair *channel_pairs, u32 nof_pairs, u32 base_freq, struct hostapd_channel_data *chan)
{
  int i;
  struct acs_candidate_table *candidates = iface->candidates;

  for (i = 0; i < nof_pairs; i++) {
    if ((channel_pairs[i].primary == chan->chan) && acs_is_subchannel_allowed(iface, channel_pairs[i].secondary)) {

      /* Special rule to allow only 2 non-overlapping 40 MHz candidates (1--5, 7--11) in 2.4 GHz,
       * if special acs_use24overlapped configuration parameter is set to 0 (default) */
      if ((base_freq < 5000) && (!iface->conf->acs_use24overlapped) &&
          (acs_is_40_mhz_overlap_chan(&channel_pairs[i])))
        continue;

      if (channel_pairs[i].primary < channel_pairs[i].secondary)
        candidates[iface->num_candidates].freq = chan->freq;
      else candidates[iface->num_candidates].freq = base_freq + 5 * channel_pairs[i].secondary;
      candidates[iface->num_candidates].primary = channel_pairs[i].primary;
      candidates[iface->num_candidates].secondary = channel_pairs[i].secondary;
      candidates[iface->num_candidates].chan = (channel_pairs[i].primary + channel_pairs[i].secondary) >> 1;
      candidates[iface->num_candidates].width = 40;

      if ((chan->flag & HOSTAPD_CHAN_RADAR) ||
           acs_is_radar_affected_channel(iface, channel_pairs[i].secondary))
        candidates[iface->num_candidates].radar_affected = TRUE;

      wpa_printf(MSG_DEBUG, "ACS: adding candidate %d, freq %d chan %d pri %d sec %d width %d",
        iface->num_candidates,
        candidates[iface->num_candidates].freq,
        candidates[iface->num_candidates].chan,
        candidates[iface->num_candidates].primary,
        candidates[iface->num_candidates].secondary,
        candidates[iface->num_candidates].width);

      iface->num_candidates++;
    }
  }
}

void add_80_channels(struct hostapd_iface *iface, channel_80 *channel_pairs, u32 nof_pairs, struct hostapd_channel_data *chan)
{
  int i;
  struct acs_candidate_table *candidates = iface->candidates;

  for (i = 0; i < nof_pairs; i++) {
    if ((channel_pairs[i].primary == chan->chan) &&
        !acs_is_channel_disabled(iface, channel_pairs[i].freq, 80)) {
      candidates[iface->num_candidates].freq = channel_pairs[i].freq;
      candidates[iface->num_candidates].chan = channel_pairs[i].chan_no;
      candidates[iface->num_candidates].primary = channel_pairs[i].primary;
      candidates[iface->num_candidates].secondary = channel_pairs[i].secondary;
      candidates[iface->num_candidates].width = 80;

      if (acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no - 6) ||
          acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no - 2) ||
          acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no + 2) ||
          acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no + 6))
        candidates[iface->num_candidates].radar_affected = TRUE;

      wpa_printf(MSG_DEBUG, "ACS: adding candidate %d, freq %d chan %d pri %d sec %d width %d",
        iface->num_candidates,
        candidates[iface->num_candidates].freq,
        candidates[iface->num_candidates].chan,
        candidates[iface->num_candidates].primary,
        candidates[iface->num_candidates].secondary,
        candidates[iface->num_candidates].width);

      iface->num_candidates++;
    }
  }
}

void add_160_channels(struct hostapd_iface *iface, channel_160 *channel_pairs, u32 nof_pairs, struct hostapd_channel_data *chan)
{
  int i;
  struct acs_candidate_table *candidates = iface->candidates;

  for (i = 0; i < nof_pairs; i++) {
    if ((channel_pairs[i].primary == chan->chan) &&
        !acs_is_channel_disabled(iface, channel_pairs[i].freq, 160)) {
      candidates[iface->num_candidates].freq = channel_pairs[i].freq;
      candidates[iface->num_candidates].chan = channel_pairs[i].chan_no;
      candidates[iface->num_candidates].primary = channel_pairs[i].primary;
      candidates[iface->num_candidates].secondary = channel_pairs[i].secondary;
      candidates[iface->num_candidates].width = 160;

      if (acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no - 14) ||
          acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no - 10) ||
          acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no -  6) ||
          acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no -  2) ||
          acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no +  2) ||
          acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no +  6) ||
          acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no + 10) ||
          acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no + 14))
        candidates[iface->num_candidates].radar_affected = TRUE;

      wpa_printf(MSG_DEBUG, "ACS: adding candidate %d, freq %d chan %d pri %d sec %d width %d",
        iface->num_candidates,
        candidates[iface->num_candidates].freq,
        candidates[iface->num_candidates].chan,
        candidates[iface->num_candidates].primary,
        candidates[iface->num_candidates].secondary,
        candidates[iface->num_candidates].width);

      iface->num_candidates++;
    }
  }
}

static void acs_init_candidate_table(struct hostapd_iface *iface)
{
  int i;
  struct hostapd_hw_modes *mode = iface->current_mode;
  struct acs_candidate_table *candidates = iface->candidates;

  memset(candidates, 0, sizeof(iface->candidates));
  iface->num_candidates = 0;
  iface->selected_candidate = -1;

  wpa_printf(MSG_DEBUG, "ACS: init candidate table, num_chan = %d", mode->num_channels);

  for (i = 0; i < mode->num_channels; i++) {
    struct hostapd_channel_data *chan = &mode->channels[i];
    if (chan->flag & HOSTAPD_CHAN_DISABLED) continue;

    if (!is_in_chanlist(iface, chan))
     continue;

    if (iface->conf->acs_use24overlapped ||
        (chan->chan == 1) || (chan->chan == 6) || (chan->chan == 11) ||
        (chan->freq >= 5000))
    {
      if (chan->flag & HOSTAPD_CHAN_RADAR) candidates[iface->num_candidates].radar_affected = TRUE;

      candidates[iface->num_candidates].freq = chan->freq;
      candidates[iface->num_candidates].chan = chan->chan;
      candidates[iface->num_candidates].primary = chan->chan;
      candidates[iface->num_candidates].secondary = 0;
      candidates[iface->num_candidates].width = 20;

      wpa_printf(MSG_DEBUG, "ACS: adding candidate %d, freq %d chan %d pri %d sec %d width %d",
                        iface->num_candidates,
                        candidates[iface->num_candidates].freq,
                        candidates[iface->num_candidates].chan,
                        candidates[iface->num_candidates].primary,
                        candidates[iface->num_candidates].secondary,
                        candidates[iface->num_candidates].width);

      iface->num_candidates++;
    }

    if (!iface->conf->secondary_channel) { /* 40 MHz not supported */
      continue;
    }

    if (chan->freq < 5000) { /* 2.4 GHz */
      add_channel_pair_candidates(iface, channel_pairs_40_24G, sizeof(channel_pairs_40_24G) / sizeof(channel_pair), 2407, chan);
    } else { /* 5 GHz */
      add_channel_pair_candidates(iface, channel_pairs_40_5G, sizeof(channel_pairs_40_5G) / sizeof(channel_pair), 5000, chan);

      if (iface->conf->vht_oper_chwidth >= CHANWIDTH_80MHZ) { /* 80 MHz supported */
        add_80_channels(iface, channels80, sizeof(channels80) / sizeof(channel_80), chan);
      }
      if (iface->conf->vht_oper_chwidth == CHANWIDTH_160MHZ) { /* 160 MHz supported */
        add_160_channels(iface, channels160, sizeof(channels160) / sizeof(channel_160), chan);
      }
    }
  }
  /* 80p80 is not supported */

  wpa_printf(MSG_DEBUG, "ACS: init candidate table done");

  return;
}


static int acs_request_scan(struct hostapd_iface *iface)
{
	struct wpa_driver_scan_params params;
	struct hostapd_channel_data *chan;
	int i, *freq;

	os_memset(&params, 0, sizeof(params));
	params.freqs = os_calloc(iface->current_mode->num_channels + 1,
				 sizeof(params.freqs[0]));
	if (params.freqs == NULL)
		return -1;

	freq = params.freqs;
	for (i = 0; i < iface->current_mode->num_channels; i++) {
		chan = &iface->current_mode->channels[i];
		if (chan->flag & HOSTAPD_CHAN_DISABLED)
			continue;

		if (!is_in_chanlist(iface, chan))
			continue;

		*freq++ = chan->freq;
	}
	*freq = 0;

	iface->scan_cb = acs_scan_complete;

	wpa_printf(MSG_DEBUG, "ACS: Scanning %d / %d",
		   iface->acs_num_completed_scans + 1,
		   iface->conf->acs_num_scans);

        iface->in_scan = 1;
	if (hostapd_driver_scan(iface->bss[0], &params) < 0) {
		wpa_printf(MSG_ERROR, "ACS: Failed to request initial scan");
                iface->in_scan = 0;
		acs_cleanup(iface);
		os_free(params.freqs);
		return -1;
	}

	os_free(params.freqs);
	return 0;
}


enum hostapd_chan_status acs_init(struct hostapd_iface *iface)
{

	wpa_printf(MSG_INFO, "ACS: Automatic channel selection started, this may take a bit");

	if (iface->drv_flags & WPA_DRIVER_FLAGS_ACS_OFFLOAD) {
		wpa_printf(MSG_INFO, "ACS: Offloading to driver");
		if (hostapd_drv_do_acs(iface->bss[0]))
			return HOSTAPD_CHAN_INVALID;
		return HOSTAPD_CHAN_ACS;
	}

	if (!iface->current_mode)
		return HOSTAPD_CHAN_INVALID;

	acs_cleanup(iface);

#ifndef UNIT_TEST
  if (!iface->conf->acs_init_done)
    acs_init_candidate_table(iface);
#endif
	if (acs_request_scan(iface) < 0)
		return HOSTAPD_CHAN_INVALID;

	hostapd_set_state(iface, HAPD_IFACE_ACS);
	wpa_msg(iface->bss[0]->msg_ctx, MSG_INFO, ACS_EVENT_STARTED);

	return HOSTAPD_CHAN_ACS;
}


char acs_to_exclude_candidate(struct hostapd_iface *iface, u32 i, struct os_reltime *now)
{
  struct acs_candidate_table *candidates = iface->candidates;

  if (acs_is_channel_disabled(iface, candidates[i].freq, candidates[i].width)) {
    wpa_printf(MSG_DEBUG, "ACS: skip candidate %d, disabled", i);
    return ACS_EXCLUDE_DISABLED;
  }

  if (candidates[i].radar_detected) {
    wpa_printf(MSG_DEBUG, "ACS: skip candidate %d, radar detect", i);
    return ACS_EXCLUDE_RADAR;
  }

  if (candidates[i].intolerant40 &&
    !os_reltime_expired(now, &candidates[i].ts_intolerant40, iface->conf->acs_to_degradation[T_40INTO])) {
    wpa_printf(MSG_DEBUG, "ACS: skip candidate %d, intolerant no timeout", i);
    return ACS_EXCLUDE_INTOLERANT;
  }

  if (candidates[i].overlap40 &&
    !os_reltime_expired(now, &candidates[i].ts_overlap40, iface->conf->acs_to_degradation[T_LNTO])) {
    wpa_printf(MSG_DEBUG, "ACS: skip candidate %d, overlap40", i);
    return ACS_EXCLUDE_OVERLAP40;
  }

  if (candidates[i].overlap_vht_he &&
    !os_reltime_expired(now, &candidates[i].ts_overlap_vht_he, iface->conf->acs_to_degradation[T_LNTO])) {
    wpa_printf(MSG_DEBUG, "ACS: skip candidate %d, overlap_vht_he", i);
    return ACS_EXCLUDE_OVERLAP_VHT_HE;
  }

  if (candidates[i].cwi_noise + 128 > iface->conf->acs_to_degradation[D_CWI]) {
    wpa_printf(MSG_DEBUG, "ACS: skip candidate %d, CWI noise %i > %i", i, candidates[i].cwi_noise, iface->conf->acs_to_degradation[D_CWI] - 128);
    return ACS_EXCLUDE_CWI;
  }

  wpa_printf(MSG_DEBUG, "ACS: do NOT skip candidate %d", i);
  return ACS_NO_EXCLUSION;
}


typedef struct {
  u32 rank;
  u32 bw_rank;
  u32 cand_idx;
} calibration_group;


#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif


static int get_secondary_offset(int primary, int secondary)
{
  if (secondary) {
    return ((secondary > primary) ? 1 : -1);
  } else return 0;
}

int hostapd_csa_in_progress(struct hostapd_iface *iface);
int hostapd_dfs_start_channel_switch_cac(struct hostapd_iface *iface);

int acs_do_switch_channel(struct hostapd_iface *iface, int block_tx)
{
  struct acs_candidate_table *candidates = iface->candidates;
  struct hostapd_hw_modes *mode = iface->current_mode;
  struct csa_settings csa_settings;
  unsigned int i;
  int err = 1;

  /* Check if CSA in progress */
  if (hostapd_csa_in_progress(iface)) {
    wpa_printf(MSG_DEBUG, "will NOT switch to a new channel - CSA already in progress");
    return -1;
  }

  wpa_printf(MSG_DEBUG, "try switch to a new channel %d, params freq %d sec %d vht_chwidth %d seg0 %d seg1 %d block_tx %d", iface->conf->channel,
    acs_chan_to_freq(iface->conf->channel),
    iface->conf->secondary_channel,
    iface->conf->vht_oper_chwidth,
    iface->conf->vht_oper_centr_freq_seg0_idx,
    iface->conf->vht_oper_centr_freq_seg1_idx,
    block_tx);

  /* Check if active CAC */
  if (iface->cac_started)
    return hostapd_setup_interface_complete(iface, 0);

  /* check CAC required */
  for (i = 0; i < mode->num_channels; i++) {
    struct hostapd_channel_data *channel;

    channel = &mode->channels[i];

    if (channels_overlap(channel->freq, 20, candidates[iface->selected_candidate].freq,
                                            candidates[iface->selected_candidate].width) &&
        (channel->flag & HOSTAPD_CHAN_RADAR) &&
        ((channel->flag & HOSTAPD_CHAN_DFS_MASK) != HOSTAPD_CHAN_DFS_AVAILABLE))
    {
      iface->freq = channel->freq;
      hostapd_setup_interface_complete(iface, 0);
      return 0;
    }
  }

  wpa_printf(MSG_DEBUG, "will switch to a new channel %d, params freq %d sec %d vht_chwidth %d seg0 %d seg1 %d", iface->conf->channel,
             acs_chan_to_freq(iface->conf->channel),
             iface->conf->secondary_channel,
             iface->conf->vht_oper_chwidth,
             iface->conf->vht_oper_centr_freq_seg0_idx,
             iface->conf->vht_oper_centr_freq_seg1_idx);

  /* Setup CSA request */
  os_memset(&csa_settings, 0, sizeof(csa_settings));
  csa_settings.cs_count = hostapd_get_cs_count(iface);
  csa_settings.block_tx = block_tx;

  if (!iface->conf->ieee80211n) wpa_printf(MSG_ERROR, "interface HT == OFF, please report !!!");

  err = hostapd_set_freq_params(&csa_settings.freq_params,
    iface->conf->hw_mode,
    acs_chan_to_freq(iface->conf->channel),
    iface->conf->channel,
    iface->conf->ieee80211n,
    iface->conf->ieee80211ac,
    iface->conf->ieee80211ax,
    iface->conf->secondary_channel,
    iface->conf->vht_oper_chwidth,
    iface->conf->vht_oper_centr_freq_seg0_idx,
    iface->conf->vht_oper_centr_freq_seg1_idx,
    iface->current_mode->vht_capab);

  if (err) {
    wpa_printf(MSG_ERROR, "failed to calculate CSA freq params");
    /* possibly recover actual iface params */
    hostapd_disable_iface(iface);
    return -1;
  }

  err = hostapd_prepare_and_send_csa_deauth_cfg_to_driver(iface->bss[0]);
  if (err) {
    wpa_printf(MSG_ERROR, "hostapd_prepare_and_send_csa_deauth_cfg_to_driver failed: %s",
                            iface->bss[0]->conf->iface);
    return err;
  }
  for (i = 0; i < iface->num_bss; i++) {
    err = hostapd_switch_channel(iface->bss[i], &csa_settings);
    if (err) {
      wpa_printf(MSG_ERROR, "Unable to switch channel !");
      return -1;
    }
  }

  return TRUE;
}


void acs_print_info_header(int level)
{
  wpa_printf(level, " #  1y  2y freq BW  R#     NF load bss pri cal aff rdr ovl40 ovl80 ovl160 priOvl int40 cwi dyn20 dyn40 dyn80 dyn160 RSSI SNR RXevt extSTA");
}

void acs_print_info_header_file(FILE *fp)
{
  fprintf(fp, "\n #  1y  2y freq BW  R#     NF load bss pri cal aff rdr ovl40 ovl80 ovl160 priOvl int40 cwi dyn20 dyn40 dyn80 dyn160 RSSI SNR RXevt ext txp skip\n");
}

void acs_print_cand_no(int level, struct hostapd_iface *iface, int i)
{
  struct acs_candidate_table *candidates = iface->candidates;

  wpa_printf(level, "%2d %3d %3d %4d %3d %5d %4d %3d %2d  %2d   %1d  %2d  %2d   %2d    %2d    %2d     %2d     %2d  %4d %2d     %2d    %2d    %2d   %4d %3d   %2d   %2d",
  i,
  candidates[i].primary,
  candidates[i].secondary,
  candidates[i].freq,
  candidates[i].width,
  candidates[i].rank,

  candidates[i].noise_floor,
  candidates[i].channel_load,
  candidates[i].num_bss,
  candidates[i].num_bss_pri,
  candidates[i].calibrated,
  candidates[i].radar_affected,
  candidates[i].radar_detected,
  candidates[i].overlap40,
  candidates[i].overlap80,
  candidates[i].overlap160,
  candidates[i].primary_on_overlap_4080160,
  candidates[i].intolerant40,
  candidates[i].cwi_noise,
  candidates[i].dynBW20,
  candidates[i].dynBW40,
  candidates[i].dynBW80,
  candidates[i].dynBW160,
  candidates[i].rssi,
  candidates[i].snr,
  candidates[i].not_80211_rx_evt,
  candidates[i].not_my_sta_low_rssi);
}

void acs_print_cand_no_file(FILE *fp, struct hostapd_iface *iface, int i)
{
  struct acs_candidate_table *candidates = iface->candidates;

  fprintf(fp, "%2d %3d %3d %4d %3d %5d %4d %3d %2d  %2d   %c  %2d  %2d   %2d    %2d     %2d    %2d     %2d  %4d %2d     %2d    %2d    %2d   %4d %3d  %2d    %2d  %3d  %c\n",
    i,
    candidates[i].primary,
    candidates[i].secondary,
    candidates[i].freq,
    candidates[i].width,
    candidates[i].rank,

    candidates[i].noise_floor,
    candidates[i].channel_load,
    candidates[i].num_bss,
    candidates[i].num_bss_pri,
    candidates[i].calibrated ? 'y' : 'n',
    candidates[i].radar_affected,
    candidates[i].radar_detected,
    candidates[i].overlap40,
    candidates[i].overlap80,
    candidates[i].overlap160,
    candidates[i].primary_on_overlap_4080160,
    candidates[i].intolerant40,
    candidates[i].cwi_noise,
    candidates[i].dynBW20,
    candidates[i].dynBW40,
    candidates[i].dynBW80,
    candidates[i].dynBW160,
    candidates[i].rssi,
    candidates[i].snr,
    candidates[i].not_80211_rx_evt,
    candidates[i].not_my_sta_low_rssi,
    candidates[i].tx_power,
    candidates[i].exclude_reason);
}

void acs_print_info_legend(FILE *fp)
{
  fprintf(fp, "Legend:  # = candidate no\n" \
              "        1y = primary channel\n" \
              "        2y = secondary channel\n" \
              "      freq = frequency\n" \
              "        BW = bandwidth, channel width\n" \
              "        R# = calculated rank (not valid for skipped channels)\n" \
              "        NF = noise floor\n" \
              "       bsy = channel busy time\n" \
              "       bss = number of APs detected overlapping with channel\n" \
              "       pri = number of APs detected overlapping with primary channel\n" \
              "       cal = calibrated ? yes or no\n" \
              "       aff = radar affected ?\n" \
              "       rdr = radar detected\n" \
              "     ovl40 = overlapping 40MHz APs\n" \
              "     ovl80 = overlapping 80MHz APs\n" \
              "    ovl160 = overlapping 160MHz APs\n" \
              "    priOvl = primary channel overlapping with 40 or 80 or 160MHZ BSS\n" \
              "     int40 = intolerant 40MHz\n" \
              "       cwi = CWI noise level detected\n" \
              "     dyn20 = dynamicBW20MHz - number of instances when 20MHz transmission was aborted due to CCA busy\n" \
              "     dyn40 = dynamicBW40MHz - number of instances when 40MHz transmission was aborted due to CCA busy\n" \
              "     dyn80 = dynamicBW80MHz - number of instances when 80MHz transmission was aborted due to CCA busy\n" \
              "    dyn160 = dynamicBW160MHz- number of instances when 160MHz transmission was aborted due to CCA busy\n" \
              "      RSSI = average RSSI level in channel. Value in dB\n" \
              "       SNR = average SNR in channel. Value in dB\n" \
              "     RXevt = number of non 802.11 Rx events\n" \
              "       ext = number of packets received from stations not connected to this AP\n" \
              "       txp = TX power\n"
              "      skip = reason, why this channel candidate was excluded from ranking/selection: r = radar, i = intolerant, o = overlap40, v = overlap VHT HE p = primary overlap, c = CWI noise\n");
}

const char *sw_reasons[] = {
  "initial scan",
  "radar",
  "background scan",
  "periodic update",
};


int acs_set_new_chan_if_ok(struct hostapd_iface *iface, int min_rank_cand_idx, int switch_reason, FILE *fp_hist)
{
  struct acs_candidate_table *candidates = iface->candidates;
  int actual_primary, actual_secondary, actual_width;
  int res = FALSE;

  actual_primary   = iface->conf->channel;
  actual_secondary = iface->conf->secondary_channel ? iface->conf->channel + iface->conf->secondary_channel * 4 : 0;
  actual_width     = get_num_width(iface->conf->vht_oper_chwidth, actual_secondary);

  /* validate previously selected candidate */
  if (iface->selected_candidate >= 0)
  {
    if ((candidates[iface->selected_candidate].primary != actual_primary) ||
        (candidates[iface->selected_candidate].width != actual_width) ||
        (candidates[iface->selected_candidate].secondary != actual_secondary))
    {
      wpa_printf(MSG_WARNING, "invalid previously candidate %d detected, current channel %d:%d width %d",
        iface->selected_candidate, actual_primary, actual_secondary, actual_width);
      iface->selected_candidate = -1; /* invalid */
    }
  } else wpa_printf(MSG_DEBUG, "no previously selected candidate, current channel %d:%d width %d",
                               actual_primary, actual_secondary, actual_width);

  /* got previous, now check threshold */
  if ((iface->selected_candidate < 0) || /* if previously selected candidate is not known - switch */
      (switch_reason == SWR_RADAR) || (switch_reason == SWR_INITIAL) || /* in these cases, switch regardless of threshold */
      (candidates[iface->selected_candidate].rank == -1) || /* If previously selected candidates rank is now invalid, switch regardless of threshold*/
      ((iface->selected_candidate != min_rank_cand_idx) &&
       (candidates[min_rank_cand_idx].rank < candidates[iface->selected_candidate].rank * (100 - iface->conf->acs_switch_thresh) / 100)))
  {
    if ((candidates[min_rank_cand_idx].primary   != actual_primary) ||
        (candidates[min_rank_cand_idx].secondary != actual_secondary) ||
        (candidates[min_rank_cand_idx].width     != actual_width))
    {
      wpa_printf(MSG_DEBUG, "ACS going from candidate %d to %d", iface->selected_candidate, min_rank_cand_idx);
      acs_print_info_header(MSG_DEBUG);
      acs_print_info_header_file(fp_hist);
      if (iface->selected_candidate >= 0) {
        acs_print_cand_no(MSG_DEBUG, iface, iface->selected_candidate);
        acs_print_cand_no_file(fp_hist, iface, iface->selected_candidate);
      } else {
        fprintf(fp_hist, "switch from channel %d:%d width %d\n", actual_primary, actual_secondary, actual_width);
      }
      acs_print_cand_no(MSG_DEBUG, iface, min_rank_cand_idx);
      acs_print_cand_no_file(fp_hist, iface, min_rank_cand_idx);

      /* set the new channel */
      iface->conf->channel = candidates[min_rank_cand_idx].primary;
      iface->conf->vht_oper_chwidth = get_vht_width(candidates[min_rank_cand_idx].width);
      iface->conf->secondary_channel = get_secondary_offset(candidates[min_rank_cand_idx].primary, candidates[min_rank_cand_idx].secondary);
      iface->selected_candidate = min_rank_cand_idx;

      acs_smart_adjust_vht_center_freq(iface, candidates[min_rank_cand_idx].chan);

      res = TRUE;
    }
  }

  return res;
}

int acs_recalc_ranks_and_set_chan (struct hostapd_iface *iface, int switch_reason)
{
  struct os_reltime now;
  u32 numerator, denominator, base_denominator;
  calibration_group cg[ACS_NUM_GRP_PRIORITIES];
  int *grp_map;
  struct acs_candidate_table *candidates = iface->candidates;
  FILE *fp      = NULL;
  FILE *fp_hist = NULL;
  int i, prio;
  int res = FALSE;

#if UNIT_TEST
  FILE *ftest = NULL;

  ftest = fopen(iface->conf->smart_acs_test_vector, "r");
  if (!ftest) {
    wpa_printf(MSG_ERROR, "ACS: cannot open test-vector file");
    goto end;
  }

  memset(candidates, 0, sizeof(candidates));
  iface->num_candidates = 0;

  fgets(ftest, line); /* header line */

  for (i = 0; !foef(ftest), i++) {
    fgets(ftest, line);
    if (25 != sscanf(line, "%2d %2d %2d %4d %2d %2d %4d %3d %2d  %2d   %1d  %2d  %2d   %2d    %2d    %2d     %2d  %4d %2d     %2d    %2d   %4d %3d  %2d  %2d\n",
                            &j,
                            &candidates[i].primary,
                            &candidates[i].secondary,
                            &candidates[i].freq,
                            &candidates[i].width,
                            &candidates[i].rank,

                            &candidates[i].noise_floor,
                            &candidates[i].busy_time,
                            &candidates[i].num_bss,
                            &candidates[i].num_bss_pri,
                            &candidates[i].calibrated,
                            &candidates[i].radar_affected,
                            &candidates[i].radar_detected,
                            &candidates[i].overlap40,
                            &candidates[i].overlap80,
                            &candidates[i].primary_on_overlap_4080,
                            &candidates[i].intolerant40,
                            &candidates[i].cwi_noise,
                            &candidates[i].dynBW20,
                            &candidates[i].dynBW40,
                            &candidates[i].dynBW80,
                            &candidates[i].rssi,
                            &candidates[i].snr,
                            &candidates[i].not_80211_rx_evt,
                            &candidates[i].not_my_sta_low_rssi) {
      printf("epic fail in unit test mode");
      fclose(ftest);
      goto end;
    }
    iface->num_candidates++;
  }
  fclose(ftest);
#endif

  fp = fopen(iface->conf->acs_smart_info_file, "w");
  if (!fp) {
    wpa_printf(MSG_ERROR, "ACS: cannot open info file");
    goto end;
  }

  fp_hist = acs_write_file(iface->conf->acs_history_file, TRUE);
  if (!fp_hist) {
    wpa_printf(MSG_ERROR, "ACS: cannot open history file");
    goto end;
  }

  memset(&cg, 0xFF, sizeof(cg));

  grp_map = iface->conf->acs_policy == ACS_POLICY_REACH ? iface->conf->acs_grp_prio_reach_map : iface->conf->acs_grp_prio_tp_map;

  wpa_printf(MSG_DEBUG, "ACS: recalculate candidate table");

  denominator = base_denominator =
    iface->conf->acs_penalty_factors[K_BWCCA]    + iface->conf->acs_penalty_factors[K_RSSI] +
    iface->conf->acs_penalty_factors[K_SNR]      + iface->conf->acs_penalty_factors[K_NF] +
    iface->conf->acs_penalty_factors[K_NOT80211] + iface->conf->acs_penalty_factors[K_CL] +
    iface->conf->acs_penalty_factors[K_NOTMYSTA] + iface->conf->acs_penalty_factors[K_DELTA_TX_POWER] +
    iface->conf->acs_penalty_factors[K_NUMOFBSS] + iface->conf->acs_penalty_factors[K_DYN_BW];

  wpa_printf(MSG_DEBUG, "ACS recalc base denominator %d", denominator);

  acs_print_info_legend(fp);
  acs_print_info_header_file(fp);

  os_get_reltime(&now);

  for (i = 0; (i < iface->num_candidates && i < MAX_CANDIDATES); i++) {
    int dyn_bwccs, rssi, nf, snr, non80211evt, ext_sta, num_bss, channel_load,
        vht_dynamic_bw80, vht_dynamic_bw160;
    int tx_power, cg_idx = CALIB_20;
    denominator = base_denominator;
    numerator = 0;

    wpa_printf(MSG_DEBUG, "ACS: recalculate X: candidate %d", i);

    acs_print_info_header(MSG_DEBUG);
    acs_print_cand_no(MSG_DEBUG, iface, i);

    candidates[i].exclude_reason = acs_to_exclude_candidate(iface, i, &now);

    /* skip channels to be excluded */
    if (candidates[i].exclude_reason == ACS_NO_EXCLUSION) {

      /* CS WLAN dynamic BW/CCS */

      switch (candidates[i].width) {
        case 20 : dyn_bwccs = candidates[i].dynBW20;  break;
        case 40 : dyn_bwccs = candidates[i].dynBW40;  break;
        case 80 : dyn_bwccs = candidates[i].dynBW80;  break;
        case 160: /* fallthrough - 80p80 go here */
        default : dyn_bwccs = candidates[i].dynBW160; break;
      }

      dyn_bwccs = iface->conf->acs_penalty_factors[K_BWCCA] *
        MIN(AOCS_NORM_BWCCA, dyn_bwccs); /* normalization, sort of */

      numerator += dyn_bwccs;

      wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN dynamic BW/CCS %d", dyn_bwccs);

      /* CS WLAN RSSI */

      rssi = iface->conf->acs_penalty_factors[K_RSSI] * AOCS_NORM_RSSI * (candidates[i].rssi + 128) / 255;
      numerator += rssi;

      wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN RSSI %d", rssi);

      /* CS WLAN SNR */

      snr = iface->conf->acs_penalty_factors[K_SNR] * AOCS_NORM_SNR * (candidates[i].snr + 128) / 255;
      numerator += snr;

      wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN SNR %d", snr);

      /* CS WLAN NF - temporary change to use CWI value instead of NF since NF not available during scan */

      nf = iface->conf->acs_penalty_factors[K_NF] * AOCS_NORM_NF * (candidates[i].cwi_noise /*noise_floor*/ + 128) / 255;
      numerator += nf;

      wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN NF %d", nf);

      /* CS WLAN not 802.11 RX event */

      non80211evt = iface->conf->acs_penalty_factors[K_NOT80211] * MIN(AOCS_NORM_NOT_80211_EVT, candidates[i].not_80211_rx_evt);
      numerator += non80211evt;

      wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN not 802.11 %d", non80211evt);

      /* CS WLAN not my STA low RSSI */

      ext_sta = iface->conf->acs_penalty_factors[K_NOTMYSTA] * MIN(AOCS_NORM_EXT_STA, candidates[i].not_my_sta_low_rssi);
      numerator += ext_sta;

      wpa_printf(MSG_DEBUG, "ACS recalc CS ext_sta %d", ext_sta);

      /* CS WLAN TX power */

      wpa_printf(MSG_DEBUG, "calc TX power %d max %d pen %d", candidates[i].tx_power, iface->max_tx_power, iface->conf->acs_penalty_factors[K_DELTA_TX_POWER]);
      tx_power = candidates[i].tx_power - iface->conf->acs_penalty_factors[K_DELTA_TX_POWER];
      tx_power = iface->conf->acs_penalty_factors[K_DELTA_TX_POWER] * MIN(100, (abs(iface->max_tx_power - tx_power) * 10 / 8));
      numerator += tx_power;

      wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN tx power %d", tx_power);

      /* CS WLAN num of BSS */

      num_bss = iface->conf->acs_penalty_factors[K_NUMOFBSS] * 100 * candidates[i].num_bss / (iface->acs_num_bss + 1);
      numerator += num_bss;

      wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN numBSS %d", num_bss);

      /* CS WLAN Channel Load - already normalized */

      channel_load = iface->conf->acs_penalty_factors[K_CL] * candidates[i].channel_load;
      numerator += channel_load;

      wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN Chan load %d", channel_load);

      /* CS WLAN 80 MHz rank */

      if (iface->conf->acs_vht_dynamic_bw)
        vht_dynamic_bw80 = iface->conf->acs_penalty_factors[K_DYN_BW] * MIN(100, AOCS_NORM_DYNAMIC_BW_DISABLED * candidates[i].overlap80);
      else
        vht_dynamic_bw80 = iface->conf->acs_penalty_factors[K_DYN_BW] *
        MIN(100, AOCS_NORM_DYNAMIC_BW_ENABLED * candidates[i].overlap80 * candidates[i].primary_on_overlap_4080160);

      numerator += vht_dynamic_bw80;

      wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN 80 rank %d", vht_dynamic_bw80);

      /* CS WLAN 160 MHz rank */

      if (iface->conf->acs_vht_dynamic_bw)
        vht_dynamic_bw160 = iface->conf->acs_penalty_factors[K_DYN_BW] * MIN(100, AOCS_NORM_DYNAMIC_BW_DISABLED * candidates[i].overlap160);
      else
        vht_dynamic_bw160 = iface->conf->acs_penalty_factors[K_DYN_BW] *
        MIN(100, AOCS_NORM_DYNAMIC_BW_ENABLED * candidates[i].overlap160 * candidates[i].primary_on_overlap_4080160);

      numerator += vht_dynamic_bw160;

      wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN 160 rank %d", vht_dynamic_bw160);

      /* CI */

      /*  if (cur_time > (chan_update_time + t_sto)) */
      if (os_reltime_expired(&now, &candidates[i].ts_update, iface->conf->acs_to_degradation[T_STO])) {
        double n = (double)candidates[i].cwi_noise; /* noise_floor; */
        double m = (double)iface->min_noise;
        double ci;

        ci = iface->conf->acs_penalty_factors[K_CI] *
          (pow(10, n / 5) + ((double)candidates[i].busy_time / (double)candidates[i].total_time) *
           pow(2, pow(10, n / 10) - pow(10, m / 10)));

        numerator += (int)ci;
        denominator += iface->conf->acs_penalty_factors[K_CI];

        wpa_printf(MSG_DEBUG, "ACS recalc CS WLAN CI %f", ci);
      }

      wpa_printf(MSG_DEBUG, "ACS recalc numerator %d denominator %d noise pen %d cust_pen %d", numerator, denominator,
        iface->conf->acs_chan_noise_penalty[candidates[i].chan], iface->conf->acs_chan_cust_penalty[candidates[i].chan]);

      /* final calculation */
      candidates[i].rank = (int) iface->conf->acs_chan_noise_penalty[candidates[i].chan] *
        iface->conf->acs_chan_cust_penalty[candidates[i].chan] *
        numerator / denominator;

      /* add penalty to avoid switching to DFS channels, thus losing any connectivity during CAC */
      if (iface->conf->channel) { /* not initial selection */
        if ((candidates[i].primary != iface->conf->channel) ||
            (candidates[i].secondary != (iface->conf->channel + iface->conf->secondary_channel * 4)) ||
            (candidates[i].width > get_num_width(iface->conf->vht_oper_chwidth, iface->conf->secondary_channel)))
        {
          candidates[i].rank += candidates[i].radar_affected << 6;
        }
      }
    } else candidates[i].rank = -1;

    acs_print_cand_no_file(fp, iface, i);

    /* skip channels to be excluded */
    if (candidates[i].exclude_reason != ACS_NO_EXCLUSION) continue;

    /* select calibration group */
    switch (candidates[i].width) {
      case 20: cg_idx = CALIB_20; break;
      case 40: cg_idx = CALIB_40; break;
      case 80: cg_idx = CALIB_80; break;
      case 160:cg_idx = CALIB_160; break;
      default: wpa_printf(MSG_ERROR, "Wrong width");
               assert(0);
    }

    /* update the best rank in the selected group */
    if (candidates[i].rank < cg[cg_idx].rank) {
      cg[cg_idx].rank = candidates[i].rank;
      cg[cg_idx].cand_idx = i;
    }
  }

  if ((!iface->conf->acs_bg_scan_do_switch && (SWR_BG_SCAN == switch_reason))
      || iface->cac_required ) {
    goto end;
  }

  /* comparison between BWs */
  if ((iface->conf->acs_bw_comparison) && (iface->conf->acs_policy == ACS_POLICY_THROUGHPUT))
  { /* the first priority will not be considered, we start with it */
    for (prio = 1; prio < ACS_NUM_GRP_PRIORITIES; prio++) {
	  calibration_group *bg = &cg[grp_map[prio]];

      if (bg->rank != -1)
        bg->bw_rank = bg->rank + (((int)bg->rank * iface->conf->acs_bw_threshold[prio - 1]) / 100);

      wpa_printf(MSG_DEBUG, "ACS: BW comparison: thresh %d", iface->conf->acs_bw_threshold[prio - 1]);

	  wpa_printf(MSG_DEBUG, "ACS: BW comparison: prio %d, mapped %d, rank %d, bw rank %d\n",
		  prio, grp_map[prio], bg->rank, bg->bw_rank);
    }
  }

  for (prio = 0; prio < ACS_NUM_GRP_PRIORITIES; prio++)
  {
    u32 rank = cg[grp_map[prio]].rank;

    if (rank != -1)
    {
      int min_rank_cand_idx = cg[grp_map[prio]].cand_idx;

      /* comparison between bandwidth categories - pick up the one with the best rank */
	  if ((iface->conf->acs_bw_comparison) &&
          (iface->conf->acs_policy == ACS_POLICY_THROUGHPUT) &&
		 ((prio + 1) < ACS_NUM_GRP_PRIORITIES)) /* for the last category, there will be no rank to compare with */
      {
	    u32 bw_rank = cg[grp_map[prio + 1]].bw_rank;

        if ((bw_rank < rank) && (bw_rank != -1)) {
		  wpa_printf(MSG_DEBUG, "ACS: BW comparison: skipped candidate idx %d, rank %d, bw rank %d\n",
				min_rank_cand_idx, rank, bw_rank);
          continue;
        }
        wpa_printf(MSG_DEBUG, "ACS: BW comparison: selected candidate idx %d, bw rank %d\n",
          min_rank_cand_idx, bw_rank);
      }

      if (acs_set_new_chan_if_ok(iface, min_rank_cand_idx, switch_reason, fp_hist))
      {
        wpa_printf(MSG_DEBUG, "ACS: selected a new channel %d, width %d, secondary %d, center %d, priority group %d candidate idx %d\n",
          iface->conf->channel,
          get_num_width(iface->conf->vht_oper_chwidth, candidates[min_rank_cand_idx].secondary),
          iface->conf->secondary_channel,
          iface->conf->vht_oper_centr_freq_seg0_idx,
          prio, min_rank_cand_idx);
        fprintf(fp, "ACS: selected a new channel %d, width %d, secondary %d, center %d, priority group %d candidate idx %d\n",
          iface->conf->channel, get_num_width(iface->conf->vht_oper_chwidth,
            candidates[min_rank_cand_idx].secondary), iface->conf->secondary_channel,
          iface->conf->vht_oper_centr_freq_seg0_idx, prio, min_rank_cand_idx);
        fprintf(fp_hist, "selected a new channel %d, width %d, secondary %d, center %d, priority group %d candidate idx %d switch reason %s\n",
          iface->conf->channel, get_num_width(iface->conf->vht_oper_chwidth,
            candidates[min_rank_cand_idx].secondary), iface->conf->secondary_channel,
          iface->conf->vht_oper_centr_freq_seg0_idx, prio, min_rank_cand_idx, sw_reasons[switch_reason]);

        res = TRUE;
      }

      wpa_printf(MSG_DEBUG, "ACS: exit recalculate, selected prio %d min_rank_cand_idx %d", prio, min_rank_cand_idx);
      goto end;
    }
  }

  /* if we are still here - use the fallback channel */
  iface->conf->channel = iface->conf->acs_fallback_chan.primary;
  iface->conf->vht_oper_chwidth = get_vht_width(iface->conf->acs_fallback_chan.width);
  iface->conf->secondary_channel = get_secondary_offset(iface->conf->acs_fallback_chan.primary,
                                                        iface->conf->acs_fallback_chan.secondary);
  acs_smart_adjust_vht_center_freq(iface, 0);
  wpa_printf(MSG_DEBUG, "ACS: exit recalculate, using fallback channel");
  fprintf(fp_hist, "ACS: using fallback channel\n");
  res = TRUE;

end:
  if (fp) fclose(fp);
  if (fp_hist) fclose(fp_hist);
  return res;
}


static int bt_find_clean_channel(struct hostapd_iface *iface, int width)
{
  struct hostapd_hw_modes *mode;
  struct hostapd_channel_data *chan;
  int i, affected_start, aff_width;

  WPA_ASSERT((width == 40) || (width == 20));

  mode = iface->current_mode;

  affected_start = acs_chan_to_freq((iface->conf->secondary_channel < 0) ? iface->conf->channel - 4 : iface->conf->channel);
  aff_width = (iface->conf->ieee80211n && iface->conf->secondary_channel) ? 40 : 20;

  wpa_printf(MSG_DEBUG, "BT is searching for %d MHz channels", width);
  for (i = 0; i < mode->num_channels; i++) {
    chan = &mode->channels[i];

    /* Skip incompatible chandefs */
    if (chan->flag & HOSTAPD_CHAN_DISABLED) continue;

    if (width == 40) {
      struct hostapd_channel_data *sec_chan;

      sec_chan = hostapd_get_mode_channel(iface, chan->freq + 20);
      if (!sec_chan) continue;
      if (sec_chan->flag & HOSTAPD_CHAN_DISABLED) continue;
    }

    if (channels_overlap(affected_start, aff_width, chan->freq, width)) continue;

    iface->freq = chan->freq;
    iface->conf->channel = chan->chan;
    iface->conf->secondary_channel = (width == 40) ? 1 : 0;

    wpa_printf(MSG_DEBUG, "Selected ch. #%d", chan->chan);
    return TRUE;
  }
  return 0;
}

void acs_push_chandef(struct hostapd_iface *iface, acs_chandef *chan)
{
  chan->pri = iface->conf->channel;
  chan->sec = iface->conf->secondary_channel,
  chan->chwidth = iface->conf->vht_oper_chwidth,
  chan->cent_freq = iface->conf->vht_oper_centr_freq_seg0_idx;
  chan->sel_cand = iface->selected_candidate;
}

void acs_pop_chandef(struct hostapd_iface *iface, acs_chandef *chan)
{
  iface->conf->channel = chan->pri;
  iface->conf->secondary_channel = chan->sec;
  iface->conf->vht_oper_chwidth = chan->chwidth;
  iface->conf->vht_oper_centr_freq_seg0_idx = chan->cent_freq;
  iface->selected_candidate = chan->sel_cand;
}

void hostapd_ltq_update_channel_data(struct hostapd_iface *iface, const u8 *data, size_t data_len)
{
  struct intel_vendor_channel_data *ch_data = (struct intel_vendor_channel_data *) data;
  struct acs_candidate_table *candidates = iface->candidates;
  int j;

  if (data_len != sizeof(struct intel_vendor_channel_data)) {
	wpa_printf(MSG_ERROR, "bad channel data event, size %zu expected %zu",
		  data_len, sizeof(struct intel_vendor_channel_data));
  }

  /* Check if CSA in progress */
  if (hostapd_csa_in_progress(iface)) {
    wpa_printf(MSG_DEBUG, "discard update - CSA in progress");
    return;
  }

  /* Check if active CAC */
  if (iface->cac_started) {
    wpa_printf(MSG_DEBUG, "discard update - CAC in progress");
    return;
  }

  if (!iface->conf->acs_init_done)
  {
    if (ch_data->filled_mask & CHDATA_BT_INTERF_MODE)
    {
      if ((ch_data->primary == iface->conf->channel) &&
          (ch_data->cwi_noise + 128 > iface->conf->acs_to_degradation[D_CWI]))
      {
        acs_chandef orig_chan;
        int ret = 0;

        acs_push_chandef(iface, &orig_chan);

        wpa_printf(MSG_DEBUG, "%s called (CAC active: %s, CSA active: %s)",
          __func__, iface->cac_started ? "yes" : "no",
          hostapd_csa_in_progress(iface) ? "yes" : "no");

        if (iface->conf->ieee80211n && iface->conf->secondary_channel)
          ret = bt_find_clean_channel(iface, 40);

        if (!ret)
          ret = bt_find_clean_channel(iface, 20);

        if (!ret) { /* unable to find a clean channel */
          /* We cannot do anything about it !!! try to work as is */
          wpa_printf(MSG_WARNING, "BT failed to find a clean channel, continuing on the same channel");
        }
        else {
          /* Perform channel switch/CSA */
          if (acs_do_switch_channel(iface, 0) < 0)
          {
            wpa_printf(MSG_WARNING, "BT failed to schedule CSA - trying fallback");

            /* back out with channel switch */
            acs_pop_chandef(iface, &orig_chan);

            hostapd_disable_iface(iface);
            hostapd_enable_iface(iface);
          }
        }
      }
      return;
    } else if (!(ch_data->filled_mask & CHDATA_SCAN_MODE) ||
               (iface->num_candidates == 0)) return;
  }

  if (iface->acs_update_in_progress) return;

  iface->acs_update_in_progress = 1;

  wpa_printf(MSG_DEBUG, "ACS: parse channel data event");
  wpa_printf(MSG_DEBUG, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d 0x%X",
    ch_data->channel,
    ch_data->BW,
    ch_data->primary,
    ch_data->secondary,
    ch_data->freq,
    ch_data->load,
    ch_data->noise_floor,
    ch_data->busy_time,
    ch_data->total_time,
    ch_data->calibration,
    ch_data->num_bss,
    ch_data->dynBW20,
    ch_data->dynBW40,
    ch_data->dynBW80,
    ch_data->dynBW160,
    ch_data->tx_power,
    ch_data->rssi,
    ch_data->snr,
    ch_data->cwi_noise,
    ch_data->not_80211_rx_evt,
    ch_data->ext_sta_rx,
    ch_data->filled_mask);

  for (j = 0; j < iface->num_candidates; j++)
  {
    candidates[j].filled_mask = ch_data->filled_mask;

    if (candidates[j].freq == ch_data->freq)
    {
      if (ch_data->filled_mask & CHDATA_CALIB) {
        switch (candidates[j].width) {
          case 20: candidates[j].calibrated = ch_data->calibration & (1 << CW_20); break;
          case 40: candidates[j].calibrated = ch_data->calibration & (1 << CW_40); break;
          case 80: candidates[j].calibrated = ch_data->calibration & (1 << CW_80); break;
          case 160:candidates[j].calibrated = ch_data->calibration & (1 << CW_160);break;
          case -160: candidates[j].calibrated = ch_data->calibration & (1 << CW_80_80);
          default: break;
        }
      }
    }

    if (candidates[j].primary == ch_data->primary)
    {
      wpa_printf(MSG_DEBUG, "channel data event update candidate %d chan %d BW %d pri %d sec %d freq %d",
               j, candidates[j].chan, candidates[j].width, candidates[j].primary, candidates[j].secondary, candidates[j].freq);

      /* overwrite channel data */
      if (ch_data->filled_mask & CHDATA_NOISE_FLOOR) {
        candidates[j].noise_floor = ch_data->noise_floor;
        if ((u32)ch_data->noise_floor < iface->min_noise) iface->min_noise = (u32)ch_data->noise_floor;
      }

      if (ch_data->filled_mask & CHDATA_BUSY_TIME) {
        candidates[j].busy_time = ch_data->busy_time;

        /* DEBUG
        if (candidates[j].primary == iface->conf->channel)
          candidates[j].busy_time = 250;*/
      }

      if (ch_data->filled_mask & CHDATA_TOTAL_TIME) {
        candidates[j].total_time = ch_data->total_time;
      }

      if (ch_data->filled_mask & CHDATA_LOAD) {
        candidates[j].channel_load = ch_data->load;
      }

      if (ch_data->filled_mask & CHDATA_NUM_BSS) {
        candidates[j].num_bss = ch_data->num_bss;
      }

      if (ch_data->filled_mask & CHDATA_DYNBW) { /* these counters are reset by FW */
        candidates[j].dynBW160 += ch_data->dynBW160;
        candidates[j].dynBW80  += ch_data->dynBW80;
        candidates[j].dynBW40  += ch_data->dynBW40;
        candidates[j].dynBW20  += ch_data->dynBW20;
      }

      if (ch_data->filled_mask & CHDATA_RSSI) {
        candidates[j].rssi = ch_data->rssi;
      }

      if (ch_data->filled_mask & CHDATA_SNR) {
        candidates[j].snr = ch_data->snr;
      }

      if (ch_data->filled_mask & CHDATA_CWI_NOISE) {
        candidates[j].cwi_noise = ch_data->cwi_noise;
      }

      if (ch_data->filled_mask & CHDATA_NOT_80211_EVT) {
        candidates[j].not_80211_rx_evt = ch_data->not_80211_rx_evt;
      }

      if (ch_data->filled_mask & CHDATA_LOW_RSSI) { /* reset by FW on read */
        candidates[j].not_my_sta_low_rssi += ch_data->ext_sta_rx;
      }

      if (ch_data->filled_mask & CHDATA_TX_POWER) {
        candidates[j].tx_power = ch_data->tx_power;
        if (ch_data->tx_power > iface->max_tx_power) iface->max_tx_power = ch_data->tx_power;
      }
    }

    if (ch_data->filled_mask & CHDATA_ALL_BITS)
    {
      if (ch_data->filled_mask & CHDATA_SCAN_MODE) {
        candidates[j].entry_init_done = TRUE;
        os_get_reltime(&candidates[j].ts_scan);
      }

      os_get_reltime(&candidates[j].ts_update);
    }
  }

  if (!(ch_data->filled_mask & CHDATA_SCAN_MODE)) {
    acs_chandef cur_chan;

    acs_push_chandef(iface, &cur_chan);

    if (acs_recalc_ranks_and_set_chan(iface, SWR_UPDATE)) {
      if (acs_do_switch_channel(iface, 0) < 0) {
        /* back out with channel switch */
        acs_pop_chandef(iface, &cur_chan);
      }
    }
  }

  iface->acs_update_in_progress = 0;
}

void acs_update_intolerant_channels(struct hostapd_iface *iface, u8 chan)
{
  struct acs_candidate_table *candidates = iface->candidates;
  int j, freq;

  freq = acs_chan_to_freq(chan);
  for (j = 0; j < iface->num_candidates; j++) {
    if (candidates[j].width == 40 &&
      channels_overlap(freq, 20, candidates[j].freq, 40)) {
      candidates[j].intolerant40++;
      os_get_reltime(&candidates[j].ts_intolerant40);
    }
  }
}


void acs_update_radar(struct hostapd_iface *iface)
{
  struct acs_candidate_table *candidates = iface->candidates;
  struct hostapd_hw_modes *mode = iface->current_mode;
  int i, j;

  for (j = 0; j < iface->num_candidates; j++) {
    candidates[j].radar_detected = FALSE;

    for (i = 0; i < mode->num_channels; i++) {
      struct hostapd_channel_data *chan;

      chan = &iface->current_mode->channels[i];

      if (channels_overlap(candidates[j].freq, candidates[j].width, chan->freq, 20)) {
        if ((chan->flag & HOSTAPD_CHAN_DFS_MASK) == HOSTAPD_CHAN_DFS_UNAVAILABLE) {
          candidates[j].radar_detected = TRUE;
          break;
        }
      }
    }
  }
}

void acs_radar_switch(struct hostapd_iface *iface)
{
  int ret;
  acs_chandef cur_chan;

  if (iface->in_scan) {
    wpa_printf(MSG_WARNING, "Radar during scan !");
    return;
  }

  acs_push_chandef(iface, &cur_chan);

  ret = acs_recalc_ranks_and_set_chan(iface, SWR_RADAR);

  iface->chan_switch_reason = HAPD_CHAN_SWITCH_RADAR_DETECTED;

  if (!ret || (acs_do_switch_channel(iface, 1) < 0)) {
    /* back out with channel switch */
    acs_pop_chandef(iface, &cur_chan);
    hostapd_disable_iface(iface);
    wpa_printf(MSG_ERROR, "Unable to switch channel on radar detection !");
  }
}

static void acs_current_channel_scan_complete(struct hostapd_iface *iface)
{
	if (iface->self_beacon_req) {
		hostapd_handle_self_beacon_report_scan_results(iface->bss[0]);
	} else {
		hostapd_handle_sta_beacon_report_scan_results(iface);
	}
}

int acs_current_channel_request_scan(struct hostapd_iface *iface)
{
	struct wpa_driver_scan_params params;

	os_memset(&params, 0, sizeof(params));
	params.freqs = os_calloc(1 + 1, sizeof(params.freqs[0]));
	if (params.freqs == NULL)
		return -1;

	params.freqs[0] = iface->freq;
	iface->scan_cb = acs_current_channel_scan_complete;

	if (hostapd_driver_scan(iface->bss[0], &params) < 0) {
		wpa_printf(MSG_ERROR, "ACS: Failed to request initial scan");
		acs_cleanup(iface);
		os_free(params.freqs);
		return -1;
	}

	os_free(params.freqs);
	return 0;
}
