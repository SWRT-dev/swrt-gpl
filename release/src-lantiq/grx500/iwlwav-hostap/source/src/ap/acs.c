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
#include <limits.h>

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

extern channel_pair g_channel_pairs_40_24G[NUM_CHAN_PAIRS_40_24G];
extern channel_pair g_channel_pairs_40_5G[NUM_CHAN_PAIRS_40_5G];
extern channel_80   g_channels80[NUM_CHAN_PAIRS_80];
extern channel_160  g_channels160[NUM_CHAN_PAIRS_160];

static int acs_request_scan(struct hostapd_iface *iface);
static int acs_survey_is_sufficient(struct freq_survey *survey);
static u32 acs_get_center_chan_index(struct hostapd_iface *iface);
void acs_push_chandef(struct hostapd_iface *iface, acs_chandef *chan);
void acs_pop_chandef(struct hostapd_iface *iface, acs_chandef *chan);

#define CF0_AND_CF1_IDX_DIFF_IN_160_MHZ 8

enum acs_cwi_map_e
{
	ACS_CWI_MAP_VAL,
	ACS_CWI_MAP_PEN,

	ACS_CWI_MAP_SIZE,
};

#define ACS_CWI_PENALTY_MIN       0
#define ACS_CWI_PENALTY_MAX       100
#define ACS_CWI_PEN_MAP_NUM_COLS  19
#define ACS_CWI_PEN_MAP_NUM_ROWS  ACS_CWI_MAP_SIZE
#define ACS_CWI_MAP_VAL_FIRST_IDX 0
#define ACS_CWI_MAP_VAL_LAST_IDX  (ACS_CWI_PEN_MAP_NUM_COLS - 1)

/* if cwi < -87, penalty is 0
   if cwi > -69, penalty is 100
   others cwi values to penalty value are mapped in the table below */
int acs_cwi_val_penalty_map[ACS_CWI_PEN_MAP_NUM_ROWS][ACS_CWI_PEN_MAP_NUM_COLS] = {
	{-87, -86, -85, -84, -83, -82, -81, -80, -79, -78, -77, -76, -75, -74, -73, -72, -71, -70, -69}, /* CWI */
	{  1,   1,   2,   4,   5,   7,   9,  12,  15,  20,  27,  35,  45,  60,  70,  80,  85,  90,  95}, /* Penalty */
};

static int acs_get_cwi_penalty(int cwi_noise)
{
	int i;

	if (cwi_noise < acs_cwi_val_penalty_map[ACS_CWI_MAP_VAL][ACS_CWI_MAP_VAL_FIRST_IDX])
		return ACS_CWI_PENALTY_MIN;
	if (cwi_noise > acs_cwi_val_penalty_map[ACS_CWI_MAP_VAL][ACS_CWI_MAP_VAL_LAST_IDX])
		return ACS_CWI_PENALTY_MAX;

	for (i = 0; i < ACS_CWI_PEN_MAP_NUM_COLS; i++) {
		if (acs_cwi_val_penalty_map[ACS_CWI_MAP_VAL][i] == cwi_noise)
			return acs_cwi_val_penalty_map[ACS_CWI_MAP_PEN][i];
	}

	wpa_printf(MSG_ERROR, "ACS: Penalty for CWI %d could not be mapped", cwi_noise);
	return ACS_CWI_PENALTY_MAX;
}

enum acs_bss_map_e
{
	ACS_BSS_LOW_VAL,
	ACS_BSS_HIGH_VAL,
	ACS_BSS_MAP_PEN,

	ACS_BSS_NUM_SIZE,
};

#define ACS_BSS_PENALTY_MIN       0
#define ACS_BSS_PENALTY_MAX       100
#define ACS_BSS_PEN_MAP_NUM_COLS  14
#define ACS_BSS_PEN_MAP_NUM_ROWS  ACS_BSS_NUM_SIZE
#define ACS_BSS_MAP_VAL_FIRST_IDX 0
#define ACS_BSS_MAP_VAL_LAST_IDX  (ACS_BSS_PEN_MAP_NUM_COLS - 1)

/* if bss rssi < -84, penalty is 0
   if bss rssi > -15, penalty is 100
   others bss rssi value ranges to penalty value are mapped in the table below */
int acs_bss_rssi_penalty_map[ACS_BSS_PEN_MAP_NUM_ROWS][ACS_BSS_PEN_MAP_NUM_COLS] = {
	{-84, -79, -74, -69, -64, -59, -54, -49, -44, -39, -34, -29, -24, -19}, /* RSSI low range mark */
	{-80, -75, -70, -65, -60, -55, -50, -45, -40, -35, -30, -25, -20, -15}, /* RSSI high range mark */
	{ 20,  40,  60,  80,  90,  91,  92,  93,  94,  95,  96,  97,  98,  99}, /* Penalty */
};

static int acs_get_bss_rssi_penalty(int rssi)
{
	int i;

	if (rssi < acs_bss_rssi_penalty_map[ACS_BSS_LOW_VAL][ACS_BSS_MAP_VAL_FIRST_IDX])
		return ACS_BSS_PENALTY_MIN;
	if (rssi > acs_bss_rssi_penalty_map[ACS_BSS_HIGH_VAL][ACS_BSS_MAP_VAL_LAST_IDX])
		return ACS_BSS_PENALTY_MAX;

	for (i = 0; i < ACS_BSS_PEN_MAP_NUM_COLS; i++) {
		if ((rssi >= acs_bss_rssi_penalty_map[ACS_BSS_LOW_VAL][i]) &&
		    (rssi <= acs_bss_rssi_penalty_map[ACS_BSS_HIGH_VAL][i]))
			return acs_bss_rssi_penalty_map[ACS_BSS_MAP_PEN][i];
	}
	wpa_printf(MSG_ERROR, "ACS: Penalty for BSS RSSI %d could not be mapped", rssi);
	return ACS_BSS_PENALTY_MAX;
}

/* Open file for writing/appending. If opened for appending and it is bigger
 * than 10K, file is saved with filename .0 at the end and a new empty file is
 * created. */
static FILE * acs_write_file(const char *name, bool append)
{
#define ACS_MAX_LOG_SIZE	102400 /* 100K */

	FILE *fp;
	long int sz;
	int res;
	char *bak_file;
	size_t new_size;
	rsize_t len;

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
		len = strnlen_s(name, RSIZE_MAX_STR);
		if (!IS_VALID_RANGE(len, AP_MIN_VALID_CHAR, RSIZE_MAX_STR - 1)) {
			wpa_printf(MSG_ERROR, "acs_write_file - wrong len");
			return NULL;
		}
		new_size = len + 3;
		bak_file = os_malloc(new_size);
		if (bak_file == NULL)
			return NULL;
		res = sprintf_s(bak_file, new_size, "%s.0", name);
		if (res <= 0) {
			wpa_printf(MSG_ERROR, "%s: sprintf_s error %d", __func__, res);
			fclose(fp);
			os_free(bak_file);
			return NULL;
		}
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


static void acs_survey_all_chans_interference_factor(
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
 * At this point it's assumed chan->interference_factor has been computed.
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

	if (iface->conf->ieee80211ac || iface->conf->ieee80211ax) {
		switch (hostapd_get_oper_chwidth(iface->conf)) {
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
		    (iface->conf->ieee80211ac || iface->conf->ieee80211ax)) {
			if (hostapd_get_oper_chwidth(iface->conf) ==
			    CHANWIDTH_80MHZ &&
			    !acs_usable_vht80_chan(chan)) {
				wpa_printf(MSG_DEBUG,
					   "ACS: Channel %d: not allowed as primary channel for VHT80",
					   chan->chan);
				continue;
			}

			if (hostapd_get_oper_chwidth(iface->conf) ==
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

	switch (hostapd_get_oper_chwidth(iface->conf)) {
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

	hostapd_set_oper_centr_freq_seg0_idx(iface->conf,
                                             iface->conf->channel + offset);
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

	acs_survey_all_chans_interference_factor(iface);
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
	if (hostapd_acs_completed(iface, err, 1) == HOSTAPD_CHAN_VALID) {
		acs_cleanup(iface);
		return;
	}

	/* This can possibly happen if channel parameters (secondary
	 * channel, center frequencies) are misconfigured */
	wpa_printf(MSG_ERROR, "ACS: Possibly channel configuration is invalid, please report this along with your config file.");
	acs_fail(iface);
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

static void acs_smart_adjust_vht_center_freq(struct hostapd_iface *iface, unsigned center_chan_index)
{
	wpa_printf(MSG_DEBUG, "ACS SMART: Adjusting VHT center frequency");

	switch (hostapd_get_oper_chwidth(iface->conf)) {
	case CHANWIDTH_USE_HT:
		hostapd_set_oper_centr_freq_seg0_idx(iface->conf,iface->conf->channel + 2 * iface->conf->secondary_channel);
		break;
	case CHANWIDTH_80MHZ:
		hostapd_set_oper_centr_freq_seg0_idx(iface->conf,center_chan_index ? center_chan_index : acs_get_center_chan_index(iface));
		break;
	case CHANWIDTH_160MHZ:
		if (center_chan_index)
			hostapd_set_oper_centr_freq_seg0_idx(iface->conf,center_chan_index);
		else
			wpa_printf(MSG_ERROR, "ACS SMART: adjust VHT freq: must supply center channel for 160MHz");
		break;
	default:
		wpa_printf(MSG_INFO, "ACS SMART: Only VHT20/40/80/160 is supported at the moment");
		return;
	}
}

static FILE* acs_fopen(const char *fname, bool append, const char *name)
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

bool channels_overlap(u32 freq1, u32 width1, u32 freq2, u32 width2)
{

#define SINGLE_CHANNEL_WIDTH 20

	/* convert the first channel's center frequency to the start of frequency range */
	freq1 -= SINGLE_CHANNEL_WIDTH >> 1;
	freq2 -= SINGLE_CHANNEL_WIDTH >> 1;

	if ((freq2 >= freq1) && (freq1 + width1 > freq2)) return true;
	if ((freq1 >= freq2) && (freq2 + width2 > freq1)) return true;
	return false;
}

bool is_vht_or_he(struct hostapd_iface *iface)
{
	if ((iface->conf->ieee80211ac) || (iface->conf->ieee80211ax)) {
		return true;
	}
	return false;
}

void acs_smart_record_bsses(struct hostapd_iface *iface, struct wpa_scan_results *scan_res, FILE *fp)
{
	struct hostapd_hw_modes *mode = iface->current_mode;
	struct acs_candidate_table *candidates = iface->candidates;
	int i, j;

	wpa_printf(MSG_DEBUG, "ACS_SMART: recording BSS-es");

	iface->acs_num_bss = 0;

	for (j = 0; j < iface->num_candidates; j++) {
		candidates[j].num_bss = 0;
		candidates[j].num_bss_pri = 0;
		candidates[j].overlap40 = 0;
		candidates[j].overlap_vht_he = 0;
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
		int cf = 0;
		int bss_width = 0;
		int bss_base_freq;
		int num_width = 20;
		int bss_rssi_penalty;
		const char *bss_printable_width = "20";

		if (!acs_find_chan(iface, bss->freq))
			continue;

		ieee802_11_parse_elems((u8 *)(bss + 1), bss->ie_len, &elems, 0);

		if (bss->level < iface->conf->ScanRssiTh) {
			wpa_printf(MSG_DEBUG, "Skip due to low BSS:%-32.*s RSSI:%d", elems.ssid_len, elems.ssid, bss->level);
			continue;
		}

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

			/* IEEE802.11-2016, 9.4.2.159 VHT Operation IE, Table 9-252 */
			if (bss_width > VHT_OPER_CHANWIDTH_20_40MHZ) {
				num_width = 80;
				bss_printable_width = "80";
				bss_center_freq0 = 5000 + 5 * vht_oper->vht_op_info_chan_center_freq_seg0_idx;

				switch (bss_width) {
				case VHT_OPER_CHANWIDTH_80_160_80P80MHZ:
					if (vht_oper->vht_op_info_chan_center_freq_seg1_idx) {
						bss_center_freq1 = 5000 + 5 * vht_oper->vht_op_info_chan_center_freq_seg1_idx;

						/* IEEE802.11-2016, 9.4.2.159 VHT Operation IE, Table 9-253 */
						if (abs(vht_oper->vht_op_info_chan_center_freq_seg1_idx -
							vht_oper->vht_op_info_chan_center_freq_seg0_idx) == CF0_AND_CF1_IDX_DIFF_IN_160_MHZ) {
							bss_printable_width = "160";
							num_width = 160;
						}
						else {
							bss_printable_width = "80+80";
						}
					}
					break;
				case VHT_OPER_CHANWIDTH_160MHZ: /* Deprecated format */
					bss_printable_width = "160";
					num_width = 160;
					break;
				case VHT_OPER_CHANWIDTH_80P80MHZ: /* Deprecated format */
					bss_center_freq1 = 5000 + 5 * vht_oper->vht_op_info_chan_center_freq_seg1_idx;
					bss_printable_width = "80+80";
					break;
				default:
					wpa_printf(MSG_WARNING, "Smart ACS: BSS with Unsupported VHT channel width %d detected", bss_width);
					break;
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

		/* for 80+80 BSS only the first segment considered for Candidates ranking */
		if (os_strcmp(bss_printable_width, "80+80") == 0)
			bss_center_freq1 = 0;

		cf = (bss_center_freq1 ? bss_center_freq1 : bss_center_freq0);
		bss_base_freq = cf - (num_width >> 1) + (SINGLE_CHANNEL_WIDTH >> 1);

		/* increase the relevant counters in every channel affected */
		for (j = 0; j < iface->num_candidates; j++)
		{
			bool primary_ch_overlap;
			if (!channels_overlap(candidates[j].freq, candidates[j].width, bss_base_freq, num_width))
				continue;

			primary_ch_overlap = channels_overlap(hostapd_chan_to_freq(candidates[j].primary), 20, bss_base_freq, num_width);
			if (primary_ch_overlap) {
				candidates[j].num_bss_pri++;
			}
			candidates[j].num_bss++;
			bss_rssi_penalty = acs_get_bss_rssi_penalty(bss->level);
			candidates[j].num_bss_pen += bss_rssi_penalty;
			wpa_printf(MSG_EXCESSIVE, "ACS: Adding BSS penalty %d (from RSSI %d) to candidate idx %d",
				   bss_rssi_penalty, bss->level, j);

			/* Exclude candidates based on CoEx rules from standard for HT and VHT */
			if (candidates[j].width < 40)
				continue;

			if (ht_cap && (le_to_host16(ht_cap->ht_capabilities_info) & HT_CAP_INFO_40MHZ_INTOLERANT)) {
				candidates[j].intolerant40++;
				os_get_reltime(&candidates[j].ts_intolerant40);
				continue;
			}

			if (bss->level < iface->conf->obss_beacon_rssi_threshold)
				continue;

			/* HT 2.4 GHz: IEEE Std 802.11-2016 Chapter 11.16.3.2 */
			if ((mode->mode == HOSTAPD_MODE_IEEE80211G) &&
			   ((hostapd_chan_to_freq(candidates[j].primary) != bss->freq) ||
			    (bss_sec_freq && (hostapd_chan_to_freq(candidates[j].secondary) != bss_sec_freq)))) {
				candidates[j].overlap40 = true;
				os_get_reltime(&candidates[j].ts_overlap40);
				continue;
			}

			/* VHT: IEEE Std 802.11-2016 Chapter 11.40.2 */
			if (is_vht_or_he(iface) &&
			    (bss_sec_freq && (hostapd_chan_to_freq(candidates[j].primary) == bss_sec_freq))) {
				candidates[j].overlap_vht_he = 1;
				os_get_reltime(&candidates[j].ts_overlap_vht_he);
			}
		} /* for - candidates */
		iface->acs_num_bss++;
	}

	for (i = 0; i < iface->num_candidates; i++) {
		if (candidates[i].num_bss_pen > iface->max_num_bss_pen)
			iface->max_num_bss_pen = candidates[i].num_bss_pen;
	}

	iface->conf->acs_init_done = 1;

	fprintf(fp, "\nnum_bss = %d\n\n", iface->acs_num_bss);
}

static bool acs_is_chan_scanned(struct hostapd_iface *iface, short chan)
{
	int i;
	struct acs_candidate_table *candidate = iface->candidates;

	for (i = 0; (i < iface->num_candidates && i < MAX_CANDIDATES); i++) {
		if ((candidate[i].primary == chan) &&
		    (20 == candidate[i].width))
			return candidate[i].scanned;
	}

	return false;
}

static bool acs_are_all_available_chans_scanned(struct hostapd_iface *iface)
{
	int i, num_available_chans = 0, num_scanned = 0;
	struct hostapd_hw_modes *mode = iface->current_mode;

	for (i = 0; i < mode->num_channels; i++) {
		if (!(mode->channels[i].flag & HOSTAPD_CHAN_DISABLED) &&
		    is_in_chanlist(iface, &mode->channels[i])) {

			/* we care only about available channels, as we can't
			 * do CSA to usable or unavailable channels anyway */
			if ((mode->channels[i].flag & HOSTAPD_CHAN_RADAR) &&
			    ((mode->channels[i].flag & HOSTAPD_CHAN_DFS_MASK) != HOSTAPD_CHAN_DFS_AVAILABLE))
				continue;

			num_available_chans++;

			if (acs_is_chan_scanned(iface, mode->channels[i].chan))
				num_scanned++;
		}
	}

	wpa_printf(MSG_DEBUG, "Dynamic failsafe: num channels %u (available %d), scanned %u",
		   mode->num_channels, num_available_chans, num_scanned);

	return num_available_chans == num_scanned;
}

static void acs_smart_process_bsses(struct hostapd_iface *iface, struct wpa_scan_results *scan_res)
{
	int res;
	FILE *fp = acs_fopen(iface->conf->acs_history_file, iface->conf->acs_init_done ? true : false, "ACS history file");

	acs_smart_record_bsses(iface, scan_res, fp);

	acs_fclose(fp, "ACS history file");

	acs_update_radar(iface);
	res = acs_recalc_ranks_and_set_chan(iface, SWR_INITIAL);

	/* hostapd_setup_interface_complete() will return -1 on failure and 0 on success */
	if (hostapd_acs_completed(iface, 0, (res == 1)) == 0)
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
	fp = acs_fopen(iface->conf->acs_history_file, true, "ACS history file");

	acs_smart_record_bsses(iface, scan_res, fp);
	wpa_scan_results_free(scan_res);

	acs_fclose(fp, "ACS history file");

	if (acs_recalc_ranks_and_set_chan(iface, SWR_BG_SCAN) > 0) {
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

	if (iface->conf->acs_algo == ACS_ALGO_SURVEY) {
		wpa_printf(MSG_DEBUG, "ACS: Using survey based algorithm (acs_num_scans=%d)",
			   iface->conf->acs_num_scans);

		err = hostapd_drv_get_survey(iface->bss[0], 0);
		if (err) {
			wpa_printf(MSG_ERROR, "ACS: Failed to get survey data");
			goto fail;
		}
	} else if ((iface->conf->acs_algo == ACS_ALGO_SMART)) {
		wpa_printf(MSG_DEBUG, "ACS: Using Smart ACS based algorithm (acs_num_scans=%d)",
			   iface->conf->acs_num_scans);

		scan_res = hostapd_driver_get_scan_results(iface->bss[0]);
		if (scan_res == NULL) {
			wpa_printf(MSG_ERROR, "ACS: Failed to get BSS data");
			goto fail;
		}

		/* Change the scan callback function for BG scan results */
		iface->scan_cb = acs_bg_scan_complete;
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
	hostapd_acs_completed(iface, 1, 1);
	acs_fail(iface);
}

static u32 acs_get_center_chan_index(struct hostapd_iface *iface)
{
	int i;
	u32 primary, secondary;

	if (hostapd_get_oper_chwidth(iface->conf) != CHANWIDTH_80MHZ)
		return 0;

	primary   = iface->conf->channel;
	secondary = iface->conf->secondary_channel ? iface->conf->channel + iface->conf->secondary_channel * 4 : 0;

	for(i = 0; i < sizeof(g_channels80)/sizeof(g_channels80[0]); i++)
		if (g_channels80[i].primary == primary && g_channels80[i].secondary == secondary)
			return g_channels80[i].chan_no;

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
		if (mode->channels[j].chan == channel)
			return 1;
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
			if (chan->freq == freq + SINGLE_CHANNEL_WIDTH * i) {
				if (chan->flag & HOSTAPD_CHAN_DISABLED)
					return 1;

				switch (width) {
				case 40:
					/* TODO: check for HT40 flags
					if (chan->flag & HOSTAPD_CHAN_NO_HT40_MINUS)) return 1;*/
					break;

				case 80:
					if (chan->flag & HOSTAPD_CHAN_NO_80MHZ)
						return 1;
					break;

				case 160:
					if (chan->flag & HOSTAPD_CHAN_NO_160MHZ)
						return 1;
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
			else
				return 0;
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
			else
				candidates[iface->num_candidates].freq = base_freq + 5 * channel_pairs[i].secondary;

			candidates[iface->num_candidates].primary = channel_pairs[i].primary;
			candidates[iface->num_candidates].secondary = channel_pairs[i].secondary;
			candidates[iface->num_candidates].chan = (channel_pairs[i].primary + channel_pairs[i].secondary) >> 1;
			candidates[iface->num_candidates].width = 40;

			if ((chan->flag & HOSTAPD_CHAN_RADAR) || acs_is_radar_affected_channel(iface, channel_pairs[i].secondary))
				candidates[iface->num_candidates].radar_affected = true;

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
		if ((channel_pairs[i].primary == chan->chan) && !acs_is_channel_disabled(iface, channel_pairs[i].freq, 80)) {
			candidates[iface->num_candidates].freq = channel_pairs[i].freq;
			candidates[iface->num_candidates].chan = channel_pairs[i].chan_no;
			candidates[iface->num_candidates].primary = channel_pairs[i].primary;
			candidates[iface->num_candidates].secondary = channel_pairs[i].secondary;
			candidates[iface->num_candidates].width = 80;

			if (acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no - 6) ||
			    acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no - 2) ||
			    acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no + 2) ||
			    acs_is_radar_affected_channel(iface, channel_pairs[i].chan_no + 6))
				candidates[iface->num_candidates].radar_affected = true;

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
		if ((channel_pairs[i].primary == chan->chan) && !acs_is_channel_disabled(iface, channel_pairs[i].freq, 160)) {
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
				candidates[iface->num_candidates].radar_affected = true;

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
		if (chan->flag & HOSTAPD_CHAN_DISABLED)
			continue;

		if (!is_in_chanlist(iface, chan))
			continue;

		if (iface->conf->acs_use24overlapped || (chan->chan == 1) ||
		    (chan->chan == 6) || (chan->chan == 11) || (chan->freq >= 5000)) {
			if (chan->flag & HOSTAPD_CHAN_RADAR)
				candidates[iface->num_candidates].radar_affected = true;

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
			add_channel_pair_candidates(iface, g_channel_pairs_40_24G, sizeof(g_channel_pairs_40_24G) / sizeof(channel_pair), 2407, chan);
		} else { /* 5 GHz */
			add_channel_pair_candidates(iface, g_channel_pairs_40_5G, sizeof(g_channel_pairs_40_5G) / sizeof(channel_pair), 5000, chan);

			if (hostapd_get_oper_chwidth(iface->conf) >= CHANWIDTH_80MHZ) { /* 80 MHz supported */
				add_80_channels(iface, g_channels80, sizeof(g_channels80) / sizeof(channel_80), chan);
			}
			if (hostapd_get_oper_chwidth(iface->conf) == CHANWIDTH_160MHZ) { /* 160 MHz supported */
				add_160_channels(iface, g_channels160, sizeof(g_channels160) / sizeof(channel_160), chan);
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

	if (params.freqs == freq) {
		wpa_printf(MSG_ERROR, "ACS: No available channels found");
		os_free(params.freqs);
		return -1;
	}

	if (!iface->conf->acs_scan_no_flush)
		params.only_new_results = 1;

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


	if (!iface->conf->acs_init_done)
		acs_init_candidate_table(iface);

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

	if (candidates[i].cwi_noise[PRI_20] + 128 > iface->conf->acs_to_degradation[D_CWI]) {
		wpa_printf(MSG_DEBUG, "ACS: skip candidate %d, CWI noise %i > %i", i, candidates[i].cwi_noise[PRI_20],
			   iface->conf->acs_to_degradation[D_CWI] - 128);
		return ACS_EXCLUDE_CWI;
	}

	wpa_printf(MSG_DEBUG, "ACS: do NOT skip candidate %d", i);
	return ACS_NO_EXCLUSION;
}


typedef struct {
	u32 rank;
	u32 cand_idx;
	int rank_util_primary;
	int bw_comp_util;
	int bw_rank_cand_idx;

	/* For excluded due to Primary utilization param storage */
	u32 ex_rank;
	u32 ex_cand_idx;
	int ex_rank_util_primary;
} calibration_group;


#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

#ifndef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#endif


static int get_secondary_offset(int primary, int secondary)
{
	if (secondary)
		return ((secondary > primary) ? 1 : -1);
	else
		return 0;
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

	wpa_printf(MSG_DEBUG, "try switch to a new channel %d, params freq %d sec %d vht_chwidth %d seg0 %d seg1 %d block_tx %d",
		   iface->conf->channel,
		   hostapd_chan_to_freq(iface->conf->channel),
		   iface->conf->secondary_channel,
		   hostapd_get_oper_chwidth(iface->conf),
		   hostapd_get_oper_centr_freq_seg0_idx(iface->conf),
		   hostapd_get_oper_centr_freq_seg1_idx(iface->conf),
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
		   ((channel->flag & HOSTAPD_CHAN_DFS_MASK) != HOSTAPD_CHAN_DFS_AVAILABLE)) {
			iface->freq = channel->freq;
			hostapd_setup_interface_complete(iface, 0);
			return 0;
		}
	}

	wpa_printf(MSG_DEBUG, "will switch to a new channel %d, params freq %d sec %d vht_chwidth %d seg0 %d seg1 %d",
		   iface->conf->channel,
		   hostapd_chan_to_freq(iface->conf->channel),
		   iface->conf->secondary_channel,
		   hostapd_get_oper_chwidth(iface->conf),
		   hostapd_get_oper_centr_freq_seg0_idx(iface->conf),
		   hostapd_get_oper_centr_freq_seg1_idx(iface->conf));

	/* Setup CSA request */
	os_memset(&csa_settings, 0, sizeof(csa_settings));
	csa_settings.cs_count = hostapd_get_cs_count(iface);
	csa_settings.block_tx = block_tx;

	if (!iface->conf->ieee80211n) wpa_printf(MSG_ERROR, "interface HT == OFF, please report !!!");

	err = hostapd_set_freq_params(&csa_settings.freq_params,
				      iface->conf->hw_mode,
				      hostapd_chan_to_freq(iface->conf->channel),
				      iface->conf->channel,
				      iface->conf->enable_edmg,
				      iface->conf->edmg_channel,
				      iface->conf->ieee80211n,
				      iface->conf->ieee80211ac,
				      iface->conf->ieee80211ax,
				      iface->conf->secondary_channel,
				      hostapd_get_oper_chwidth(iface->conf),
				      hostapd_get_oper_centr_freq_seg0_idx(iface->conf),
				      hostapd_get_oper_centr_freq_seg1_idx(iface->conf),
				      iface->current_mode->vht_capab,
				      &iface->current_mode->he_capab);

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

	return true;
}


void acs_print_info_header(int level)
{
	wpa_printf(level, " #  1y  2y freq  BW    R#   RB# u_p u_bw l_p l_s ls40 ls80  bss  pri aff rdr Ovl int40 c_p c_s cs40 cs80");
}

void acs_print_cand_no(int level, struct hostapd_iface *iface, int i)
{
	struct acs_candidate_table *candidates = iface->candidates;
	UNUSED_VAR(candidates);

	wpa_printf(level, "%2d %3d %3d %4d %3d %5d %5d %3d %4d %3d %3d %4d %4d %4d %4d %3d %3d %3d %5d %3d %3d %4d %4d",
		   i,
		   candidates[i].primary,
		   candidates[i].secondary,
		   candidates[i].freq,
		   candidates[i].width,
		   candidates[i].rank,
		   candidates[i].bw_comp_rank,
		   candidates[i].rank_util_primary,
		   candidates[i].bw_comp_util,
		   candidates[i].channel_load[PRI_20],
		   candidates[i].channel_load[SEC_20],
		   candidates[i].channel_load[SEC_MAX_40],
		   candidates[i].channel_load[SEC_MAX_80],
		   candidates[i].num_bss,
		   candidates[i].num_bss_pri,
		   candidates[i].radar_affected,
		   candidates[i].radar_detected,
		   candidates[i].overlap40 | candidates[i].overlap_vht_he,
		   candidates[i].intolerant40,
		   candidates[i].cwi_noise[PRI_20],
		   candidates[i].cwi_noise[SEC_20],
		   candidates[i].cwi_noise[SEC_MAX_40],
		   candidates[i].cwi_noise[SEC_MAX_80]);
}

void acs_print_info_header_file(FILE *fp)
{
	fprintf(fp, "\n #  1y  2y freq  BW    R#   RB# u_p u_bw l_p l_s ls40 ls80  bss  pri aff rdr Ovl int40  c_p  c_s cs40 cs80 txp skip\n");
}

void acs_print_cand_no_file(FILE *fp, struct hostapd_iface *iface, int i)
{
	struct acs_candidate_table *candidates = iface->candidates;

	fprintf(fp, "%2d %3d %3d %4d %3d %5d %5d %3d %4d %3d %3d %4d %4d %4d %4d %3d %3d %3d %5d %4d %4d %4d %4d %3d %4c\n",
		i,
		candidates[i].primary,
		candidates[i].secondary,
		candidates[i].freq,
		candidates[i].width,
		candidates[i].rank,
		candidates[i].bw_comp_rank,
		candidates[i].rank_util_primary,
		candidates[i].bw_comp_util,
		candidates[i].channel_load[PRI_20],
		candidates[i].channel_load[SEC_20],
		candidates[i].channel_load[SEC_MAX_40],
		candidates[i].channel_load[SEC_MAX_80],
		candidates[i].num_bss,
		candidates[i].num_bss_pri,
		candidates[i].radar_affected,
		candidates[i].radar_detected,
		candidates[i].overlap40 | candidates[i].overlap_vht_he,
		candidates[i].intolerant40,
		candidates[i].cwi_noise[PRI_20],
		candidates[i].cwi_noise[SEC_20],
		candidates[i].cwi_noise[SEC_MAX_40],
		candidates[i].cwi_noise[SEC_MAX_80],
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
		    "       RB# = calculated rank for BW comparison\n" \
		    "       u_p = ACS calculated utilization for Primary channel\n" \
		    "      u_bw = ACS calculated utilization for BW comparison\n" \
		    "       l_p = channel load on primary channel\n" \
		    "       l_s = channel load on secondary channel (valid if width > 20)\n" \
		    "      ls40 = channel load on secondary 40 MHz part (valid if width > 40)\n" \
		    "      ls80 = channel load on secondary 80 MHz part (valid if width > 80)\n" \
		    "       bss = number of APs detected overlapping with channel\n" \
		    "       pri = number of APs detected overlapping with primary channel\n" \
		    "       aff = radar affected\n" \
		    "       rdr = radar detected\n" \
		    "       Ovl = Does not comply with CoEx standard rules (IEEE 802.11-2016: 11.16.3.2 or 11.40.2)\n" \
		    "     int40 = intolerant 40MHz\n" \
		    "       c_p = CWI noise level detected on primary channel\n" \
		    "       c_s = CWI noise level detected on secondary channel (valid if width > 20)\n" \
		    "      cs40 = CWI noise level detected on secondary 40 MHz part (valid if width > 40)\n" \
		    "      cs80 = CWI noise level detected on secondary 80 MHz part (valid if width > 80)\n" \
		    "       txp = TX power\n"
		    "      skip = reason, why this channel candidate was excluded from ranking/selection: "
				  "r = radar, i = intolerant, o = overlap40, v = overlap VHT/HE, c = CWI noise, "
				  "u = Primary channel utilization <= 20%%\n");
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
	int res = false;

	actual_primary   = iface->conf->channel;
	actual_secondary = iface->conf->secondary_channel ? iface->conf->channel + iface->conf->secondary_channel * 4 : 0;
	actual_width     = get_num_width(hostapd_get_oper_chwidth(iface->conf), actual_secondary);

	/* validate previously selected candidate */
	if (iface->selected_candidate >= 0) {
		if ((candidates[iface->selected_candidate].primary != actual_primary) ||
		    (candidates[iface->selected_candidate].width != actual_width) ||
		    (candidates[iface->selected_candidate].secondary != actual_secondary)) {
			wpa_printf(MSG_WARNING, "invalid previously candidate %d detected, current channel %d:%d width %d",
				   iface->selected_candidate, actual_primary, actual_secondary, actual_width);
			iface->selected_candidate = -1; /* invalid */
		}
	}
	else {
		wpa_printf(MSG_DEBUG, "no previously selected candidate, current channel %d:%d width %d",
			   actual_primary, actual_secondary, actual_width);
	}

	/* got previous, now check threshold */
	if ((iface->selected_candidate < 0) || /* if previously selected candidate is not known - switch */
	    (switch_reason == SWR_RADAR) || (switch_reason == SWR_INITIAL) || /* in these cases, switch regardless of threshold */
	    (candidates[iface->selected_candidate].rank == -1) || /* If previously selected candidates rank is now invalid, switch regardless of threshold*/
	    ((iface->selected_candidate != min_rank_cand_idx) &&
	     (candidates[min_rank_cand_idx].rank < candidates[iface->selected_candidate].rank * (100 - iface->conf->acs_switch_thresh) / 100))) {
		if ((candidates[min_rank_cand_idx].primary   != actual_primary) ||
		    (candidates[min_rank_cand_idx].secondary != actual_secondary) ||
		    (candidates[min_rank_cand_idx].width     != actual_width)) {
			wpa_printf(MSG_DEBUG, "ACS going from candidate %d to %d", iface->selected_candidate, min_rank_cand_idx);
			acs_print_info_header(MSG_DEBUG);
			acs_print_info_header_file(fp_hist);
			if (iface->selected_candidate >= 0) {
				acs_print_cand_no(MSG_DEBUG, iface, iface->selected_candidate);
				acs_print_cand_no_file(fp_hist, iface, iface->selected_candidate);
			}
			else {
				fprintf(fp_hist, "switch from channel %d:%d width %d\n", actual_primary, actual_secondary, actual_width);
			}
			acs_print_cand_no(MSG_DEBUG, iface, min_rank_cand_idx);
			acs_print_cand_no_file(fp_hist, iface, min_rank_cand_idx);

			/* set the new channel */
			iface->conf->channel = candidates[min_rank_cand_idx].primary;
			hostapd_set_oper_chwidth(iface->conf,get_vht_width(candidates[min_rank_cand_idx].width));
			iface->conf->secondary_channel = get_secondary_offset(candidates[min_rank_cand_idx].primary, candidates[min_rank_cand_idx].secondary);
			iface->selected_candidate = min_rank_cand_idx;

			acs_smart_adjust_vht_center_freq(iface, candidates[min_rank_cand_idx].chan);

			res = true;
		}
	}

	return res;
}
#ifndef CONFIG_NO_STDOUT_DEBUG
static const char * acs_penalty_fact_to_str(int penalty_factor)
{
	switch (penalty_factor) {
	case K_CWI:
		return "Noise floor (CWI)";
	case K_CL:
		return "Channel load";
	case K_TX_POWER:
		return "Delta from TX power";
	case K_NUMOFBSS:
		return "BSSes on candidate";
	default:
		return "UNKNOWN";
	}
}

static const char * acs_norm_to_degr_to_str(int to_degr_factor)
{
	switch (to_degr_factor) {
	case T_LNTO:
		return "Overlapping detection timeout";
	case T_40INTO:
		return "40 MHz intolerant timeout";
	case D_CWI:
		return "CWI noise detection degradation";
	default:
		return "UNKNOWN";
	}
}

static void acs_print_penalty_factors(int *acs_penalty_factors)
{
	int i;
	wpa_printf(MSG_DEBUG, "ACS: Penalty factors:");
	for (i = 0; i < ACS_NUM_PENALTY_FACTORS; i++) {
		wpa_printf(MSG_DEBUG, "\t%s penalty factor %d", acs_penalty_fact_to_str(i), *acs_penalty_factors);
		acs_penalty_factors++;
	}
}

static void acs_print_timeout_and_degradation_factors(int *acs_to_degradation)
{
	int i;
	wpa_printf(MSG_DEBUG, "ACS: Timeout or Degradation factors:");
	for (i = 0; i < ACS_NUM_DEGRADATION_FACTORS; i++) {
		wpa_printf(MSG_DEBUG, "\t%s factor %d", acs_norm_to_degr_to_str(i), *acs_to_degradation);
		acs_to_degradation++;
	}
}
#endif /* CONFIG_NO_STDOUT_DEBUG */

static inline int acs_apply_penalty_factor(struct hostapd_iface *iface, enum penalty_factor_e f, int rank)
{
	return iface->conf->acs_penalty_factors[f] * rank;
}

#define ACS_RANK_FACTOR1 88
#define ACS_RANK_FACTOR2 75
#define ACS_RANK_FACTOR3 50

#define ACS_RANK_UTIL_FACTOR_20  10000 /* (100 ∗ 100) */
#define ACS_RANK_UTIL_FACTOR_40  15000 /* (100 ∗ 100) + 0.5 ∗ (100 ∗ 100) */
#define ACS_RANK_UTIL_FACTOR_80  22500 /* (100 ∗ 100) + 0.75 ∗ (100 ∗ 100) + 0.5 ∗ (100 ∗ 100) */
#define ACS_RANK_UTIL_FACTOR_160 31300 /* (100 ∗ 100) + 0.88 ∗ (100 ∗ 100) + 0.75 ∗ (100 ∗ 100) + 0.5 ∗ (100 ∗ 100) */

#define ACS_RANK_UTIL_MIN 20
#define ACS_RANK_UTIL_MAX 50

static int acs_get_rank_bw_factor(int candidate_bw, enum acs_chan_per_bw_elems_e sec_sb)
{
	switch (candidate_bw) {
	case 40:
		return ACS_RANK_FACTOR3;
	case 80:
		if (SEC_20 == sec_sb)
			return ACS_RANK_FACTOR2;
		else
			return ACS_RANK_FACTOR3;
	case 160:
		if (SEC_20 == sec_sb)
			return ACS_RANK_FACTOR1;
		else if (SEC_MAX_40 == sec_sb)
			return ACS_RANK_FACTOR2;
		else
			return ACS_RANK_FACTOR3;
	}

	wpa_printf(MSG_ERROR, "ACS: failed to get rank bw factor for bandwidth %d, secondary subband %d",
		   candidate_bw, sec_sb);

	return 0;
}

static bool
acs_is_chan_in_sec_80 (struct acs_candidate_table *candidate, int chan)
{
	if ((candidate->primary < candidate->chan) &&
	    (chan < candidate->chan))
		return false;

	if ((candidate->primary > candidate->chan) &&
	    (chan > candidate->chan))
		return false;

	return true;
}

static void acs_update_load_if_needed(struct acs_candidate_table *candidate, int load,
				      enum acs_chan_per_bw_elems_e sec_sb)
{
	if (load > candidate->channel_load[sec_sb])
		candidate->channel_load[sec_sb] = load;
}

static void acs_update_cwi_if_needed(struct acs_candidate_table *candidate, int cwi,
				      enum acs_chan_per_bw_elems_e sec_sb)
{
	if ((0 == candidate->cwi_noise[sec_sb]) ||
	    (cwi > candidate->cwi_noise[sec_sb]))
		candidate->cwi_noise[sec_sb] = cwi;
}

static void acs_update_chan_sub_band_load_and_cwi (struct hostapd_iface *iface,
						   int chan, int load, int cwi)
{
	int i;
	struct acs_candidate_table *candidates = iface->candidates;

	for (i = 0; (i < iface->num_candidates && i < MAX_CANDIDATES); i++) {
		/* primary and secondary already set in hostapd_ltq_update_channel_data */
		if (chan == candidates[i].primary || chan == candidates[i].secondary)
			continue;

		if (channels_overlap(hostapd_chan_to_freq(chan), 20, candidates[i].freq, candidates[i].width)) {
			if (candidates[i].width == 80) {
				acs_update_load_if_needed(&candidates[i], load, SEC_MAX_40);
				acs_update_cwi_if_needed(&candidates[i], cwi, SEC_MAX_40);
			}
			else if (candidates[i].width == 160) {
				if (acs_is_chan_in_sec_80(&candidates[i], chan)) {
					acs_update_load_if_needed(&candidates[i], load, SEC_MAX_80);
					acs_update_cwi_if_needed(&candidates[i], cwi, SEC_MAX_80);
				}
				else {
					acs_update_load_if_needed(&candidates[i], load, SEC_MAX_40);
					acs_update_cwi_if_needed(&candidates[i], cwi, SEC_MAX_40);
				}
			}
		}
	}
}

static void acs_update_sub_band_load_and_cwi (struct hostapd_iface *iface)
{
	int i;
	struct acs_candidate_table *candidates = iface->candidates;

	for (i = 0; (i < iface->num_candidates && i < MAX_CANDIDATES); i++) {
		candidates[i].channel_load[SEC_MAX_40] = 0;
		candidates[i].channel_load[SEC_MAX_80] = 0;
		candidates[i].cwi_noise[SEC_MAX_40] = 0;
		candidates[i].cwi_noise[SEC_MAX_80] = 0;
	}

	for (i = 0; (i < iface->num_candidates && i < MAX_CANDIDATES); i++) {
		if (20 == candidates[i].width)
			acs_update_chan_sub_band_load_and_cwi(iface, candidates[i].chan,
							      candidates[i].channel_load[PRI_20],
							      candidates[i].cwi_noise[PRI_20]);
	}
}

static void acs_calculate_rank_utilization (struct acs_candidate_table *candidate, int rank_primary)
{
	int bw_factor, utilization;

	switch (candidate->width) {
	case 20:
		bw_factor = ACS_RANK_UTIL_FACTOR_20;
		break;
	case 40:
		bw_factor = ACS_RANK_UTIL_FACTOR_40;
		break;
	case 80:
		bw_factor = ACS_RANK_UTIL_FACTOR_80;
		break;
	case 160:
		bw_factor = ACS_RANK_UTIL_FACTOR_160;
		break;
	default:
		wpa_printf(MSG_ERROR, "Unsupported bandwidth %d", candidate->width);
		return;
	}

	utilization = 100 - (100 * candidate->bw_comp_rank / bw_factor);
	if (utilization < 0)
		utilization = 0;

	candidate->bw_comp_util = utilization;

	if (20 != candidate->width) {
		utilization = 100 - (100 * rank_primary / ACS_RANK_UTIL_FACTOR_20);
		if (utilization < 0)
			utilization = 0;
	}

	candidate->rank_util_primary = utilization;
	wpa_printf(MSG_DEBUG, "ACS: Utilization primary: %d, BW comparison utilization: %d",
		   candidate->rank_util_primary, candidate->bw_comp_util);
}

/* Return: -1: error; 0: stay on the same channel; 1: set new channel */
int acs_recalc_ranks_and_set_chan (struct hostapd_iface *iface, int switch_reason)
{
	struct os_reltime now;
	calibration_group cg[ACS_NUM_GRP_PRIORITIES];
	int *grp_map;
	struct acs_candidate_table *candidates = iface->candidates;
	FILE *fp      = NULL;
	FILE *fp_hist = NULL;
	int i, prio;
	int res = -1;
	bool all_candidates_util_below_thresh = true;

	fp = fopen(iface->conf->acs_smart_info_file, "w");
	if (!fp) {
		wpa_printf(MSG_ERROR, "ACS: cannot open info file");
		goto end;
	}

	fp_hist = acs_write_file(iface->conf->acs_history_file, true);
	if (!fp_hist) {
		wpa_printf(MSG_ERROR, "ACS: cannot open history file");
		goto end;
	}

	memset(&cg, 0, sizeof(cg));
	for (int i = 0; i < ACS_NUM_GRP_PRIORITIES; i++) {
		cg[i].rank = cg[i].ex_rank = UINT32_MAX;
		cg[i].bw_comp_util = -1;
	}

	grp_map = iface->conf->acs_policy == ACS_POLICY_REACH ? iface->conf->acs_grp_prio_reach_map : iface->conf->acs_grp_prio_tp_map;

	wpa_printf(MSG_DEBUG, "ACS: recalculate candidate table, acs_policy %d, "
			      "acs_switch_thresh %d, acs_bw_comparison %d, "
			      "acs_grp_priorities_throughput %d-%d-%d-%d, acs_grp_priorities_reach %d-%d-%d-%d",
			      iface->conf->acs_policy,
			      iface->conf->acs_switch_thresh, iface->conf->acs_bw_comparison,
			      iface->conf->acs_grp_priorities_throughput[0],
			      iface->conf->acs_grp_priorities_throughput[1], iface->conf->acs_grp_priorities_throughput[2],
			      iface->conf->acs_grp_priorities_throughput[3], iface->conf->acs_grp_priorities_reach[0],
			      iface->conf->acs_grp_priorities_reach[1], iface->conf->acs_grp_priorities_reach[2],
			      iface->conf->acs_grp_priorities_reach[3]);
#ifndef CONFIG_NO_STDOUT_DEBUG
	acs_print_penalty_factors(iface->conf->acs_penalty_factors);
	acs_print_timeout_and_degradation_factors(iface->conf->acs_to_degradation);
#endif /* CONFIG_NO_STDOUT_DEBUG */

	acs_print_info_legend(fp);
	acs_print_info_header_file(fp);

	os_get_reltime(&now);

	acs_update_sub_band_load_and_cwi(iface);

	for (i = 0; (i < iface->num_candidates && i < MAX_CANDIDATES); i++) {
		int cg_idx = CALIB_20;
		int cwi_rank, chan_load_rank, tx_power_rank, num_bss_rank = 0;
		int rank_util_primary = 0;

		wpa_printf(MSG_DEBUG, "ACS: recalculate candidate idx %d", i);

		acs_print_info_header(MSG_DEBUG);
		acs_print_cand_no(MSG_DEBUG, iface, i);

		candidates[i].exclude_reason = acs_to_exclude_candidate(iface, i, &now);

		/* skip channels to be excluded */
		if (candidates[i].exclude_reason == ACS_NO_EXCLUSION) {
			tx_power_rank = acs_apply_penalty_factor(iface, K_TX_POWER,
					MIN(100, (abs(iface->max_tx_power - candidates[i].tx_power) * 10 / 8)));
			wpa_printf(MSG_DEBUG, "ACS calc rank for tx power(%d) and max tx power(%d): %d",
				   candidates[i].tx_power, iface->max_tx_power, tx_power_rank);

			if (iface->max_num_bss_pen)
				num_bss_rank = acs_apply_penalty_factor(iface, K_NUMOFBSS,
						candidates[i].num_bss_pen * 100 / iface->max_num_bss_pen);
			wpa_printf(MSG_DEBUG, "ACS calc rank for num BSSes(%d), pen sum(%d), pen max(%d): %d",
				   candidates[i].num_bss, candidates[i].num_bss_pen, iface->max_num_bss_pen, num_bss_rank);

			chan_load_rank = acs_apply_penalty_factor(iface, K_CL, candidates[i].channel_load[PRI_20]);
			wpa_printf(MSG_DEBUG, "ACS calc primary rank for channel load(%d): %d",
				   candidates[i].channel_load[PRI_20], chan_load_rank);

			cwi_rank = acs_apply_penalty_factor(iface, K_CWI, acs_get_cwi_penalty(candidates[i].cwi_noise[PRI_20]));
			wpa_printf(MSG_DEBUG, "ACS calc primary rank for CWI(%d): %d", candidates[i].cwi_noise[PRI_20], cwi_rank);

			candidates[i].rank = cwi_rank + tx_power_rank + num_bss_rank + chan_load_rank;
			rank_util_primary = chan_load_rank + cwi_rank;

			if (candidates[i].width > 20) {
				chan_load_rank = acs_apply_penalty_factor(iface, K_CL, candidates[i].channel_load[SEC_20]);
				wpa_printf(MSG_DEBUG, "ACS calc secondary 20 rank for channel load(%d): %d",
					   candidates[i].channel_load[SEC_20], chan_load_rank);

				cwi_rank = acs_apply_penalty_factor(iface, K_CWI, acs_get_cwi_penalty(candidates[i].cwi_noise[SEC_20]));
				wpa_printf(MSG_DEBUG, "ACS calc secondary 20 rank for CWI(%d): %d",
					   candidates[i].cwi_noise[SEC_20], cwi_rank);

				candidates[i].rank += acs_get_rank_bw_factor(candidates[i].width, SEC_20) * (chan_load_rank + cwi_rank) / 100;
			}

			if (candidates[i].width > 40) {
				chan_load_rank = acs_apply_penalty_factor(iface, K_CL, candidates[i].channel_load[SEC_MAX_40]);
				wpa_printf(MSG_DEBUG, "ACS calc secondary 40 rank for channel load(%d): %d",
					   candidates[i].channel_load[SEC_MAX_40], chan_load_rank);

				cwi_rank = acs_apply_penalty_factor(iface, K_CWI, acs_get_cwi_penalty(candidates[i].cwi_noise[SEC_MAX_40]));
				wpa_printf(MSG_DEBUG, "ACS calc secondary 40 rank for CWI(%d): %d",
					   candidates[i].cwi_noise[SEC_MAX_40], cwi_rank);

				candidates[i].rank += acs_get_rank_bw_factor(candidates[i].width, SEC_MAX_40) * (chan_load_rank + cwi_rank) / 100;
			}

			if (candidates[i].width > 80) {
				chan_load_rank = acs_apply_penalty_factor(iface, K_CL, candidates[i].channel_load[SEC_MAX_80]);
				wpa_printf(MSG_DEBUG, "ACS calc secondary 80 rank for channel load(%d): %d",
					   candidates[i].channel_load[SEC_MAX_80], chan_load_rank);

				cwi_rank = acs_apply_penalty_factor(iface, K_CWI, acs_get_cwi_penalty(candidates[i].cwi_noise[SEC_MAX_80]));
				wpa_printf(MSG_DEBUG, "ACS calc secondary 80 rank for CWI(%d): %d",
					   candidates[i].cwi_noise[SEC_MAX_80], cwi_rank);

				candidates[i].rank += acs_get_rank_bw_factor(candidates[i].width, SEC_MAX_80) * (chan_load_rank + cwi_rank) / 100;
			}

			candidates[i].rank = iface->conf->acs_chan_cust_penalty[candidates[i].chan] * candidates[i].rank / 100;
			candidates[i].bw_comp_rank = candidates[i].rank - tx_power_rank - num_bss_rank;
			acs_calculate_rank_utilization(&candidates[i], rank_util_primary);

			if (candidates[i].rank_util_primary <= ACS_RANK_UTIL_MIN)
				candidates[i].exclude_reason = ACS_EXCLUDE_LOW_PRIMARY_UTIL;
		} else {
			candidates[i].rank = -1;
			candidates[i].rank_util_primary = -1;
			candidates[i].bw_comp_rank = -1;
			candidates[i].bw_comp_util = -1;
		}

		wpa_printf(MSG_DEBUG, "ACS: candidate idx %d rank %d, bw_rank %d, primary util %d, util %d",
			   i, candidates[i].rank, candidates[i].bw_comp_rank, candidates[i].rank_util_primary, candidates[i].bw_comp_util);

		acs_print_cand_no_file(fp, iface, i);

		/* skip channels to be excluded */
		if (candidates[i].exclude_reason != ACS_NO_EXCLUSION &&
		    candidates[i].exclude_reason != ACS_EXCLUDE_LOW_PRIMARY_UTIL)
			continue;

		/* select calibration group */
		switch (candidates[i].width) {
		case 20:
			cg_idx = CALIB_20;
			break;
		case 40:
			cg_idx = CALIB_40;
			break;
		case 80:
			cg_idx = CALIB_80;
			break;
		case 160:
			cg_idx = CALIB_160;
			break;
		default:
			wpa_printf(MSG_ERROR, "Wrong width");
			assert(0);
		}

		if (candidates[i].exclude_reason == ACS_EXCLUDE_LOW_PRIMARY_UTIL) {
			if ((candidates[i].rank_util_primary > cg[cg_idx].ex_rank_util_primary) ||
			    ((candidates[i].rank_util_primary == cg[cg_idx].ex_rank_util_primary) &&
			     (candidates[i].rank < cg[cg_idx].ex_rank))) {
				cg[cg_idx].ex_rank_util_primary = candidates[i].rank_util_primary;
				cg[cg_idx].ex_rank = candidates[i].rank;
				cg[cg_idx].ex_cand_idx = i;
			}
			continue;
		}

		all_candidates_util_below_thresh = false;
		if ((candidates[i].rank < cg[cg_idx].rank) ||
		    ((candidates[i].rank == cg[cg_idx].rank) &&
		     (candidates[i].rank_util_primary > cg[cg_idx].rank_util_primary))) {
			cg[cg_idx].rank_util_primary = candidates[i].rank_util_primary;
			cg[cg_idx].rank = candidates[i].rank;
			cg[cg_idx].cand_idx = i;
		}
		if (candidates[i].bw_comp_util > cg[cg_idx].bw_comp_util) {
			cg[cg_idx].bw_comp_util = candidates[i].bw_comp_util;
			cg[cg_idx].bw_rank_cand_idx = i;
		}

	}

	if ((!iface->conf->acs_bg_scan_do_switch && (SWR_BG_SCAN == switch_reason)) ||
	    iface->cac_required || !(iface->conf->acs_init_done)) {
		res = 0;
		goto end;
	}

	if (!iface->conf->acs_update_do_switch && (SWR_UPDATE == switch_reason)) {
		res = 0;
		goto end;
	}

	for (prio = 0; prio < ACS_NUM_GRP_PRIORITIES; prio++) {
		int rank;
		int min_rank_cand_idx;
		int bw_comp_util;

		if (all_candidates_util_below_thresh) {
			if (UINT32_MAX == cg[grp_map[prio]].ex_rank)
				continue;

			min_rank_cand_idx = cg[grp_map[prio]].ex_cand_idx;
			wpa_printf(MSG_WARNING, "ACS: All channels below Primary utilization threshold, selecting idx %d, grp %d",
				   min_rank_cand_idx, grp_map[prio]);
			goto set_chan;
		}

		rank = (int)cg[grp_map[prio]].rank;
		if (rank == -1)
			continue;

		min_rank_cand_idx = cg[grp_map[prio]].cand_idx;
		bw_comp_util = candidates[min_rank_cand_idx].bw_comp_util;

		/* comparison between bandwidth categories - pick up the one with the best rank */
		if ((iface->conf->acs_bw_comparison) &&
		    (iface->conf->acs_policy == ACS_POLICY_THROUGHPUT) &&
		    ((prio + 1) < ACS_NUM_GRP_PRIORITIES)) /* for the last category, there will be no rank to compare with */
		{
			if ((-1 == bw_comp_util) || (-1 == cg[grp_map[prio + 1]].bw_comp_util))
				continue;

			if (bw_comp_util < ACS_RANK_UTIL_MAX) {
				if (bw_comp_util < cg[grp_map[prio + 1]].bw_comp_util / 2) {
					wpa_printf(MSG_INFO, "ACS: Decreasing BW, from cand idx %d to %d",
						   min_rank_cand_idx, cg[grp_map[prio + 1]].bw_rank_cand_idx);
					min_rank_cand_idx = cg[grp_map[prio + 1]].bw_rank_cand_idx;
				}
			}
		}

set_chan:
		res = acs_set_new_chan_if_ok(iface, min_rank_cand_idx, switch_reason, fp_hist);
		if (res) {
			wpa_printf(MSG_DEBUG, "ACS: selected a new channel %d, width %d, secondary %d, center %d, priority group %d candidate idx %d\n",
				  iface->conf->channel,
				  get_num_width(hostapd_get_oper_chwidth(iface->conf), candidates[min_rank_cand_idx].secondary),
				  candidates[min_rank_cand_idx].secondary,
				  hostapd_get_oper_centr_freq_seg0_idx(iface->conf),
				  prio, min_rank_cand_idx);
			fprintf(fp, "ACS: selected a new channel %d, width %d, secondary %d, center %d, priority group %d candidate idx %d\n",
				iface->conf->channel, get_num_width(hostapd_get_oper_chwidth(iface->conf),
				candidates[min_rank_cand_idx].secondary), candidates[min_rank_cand_idx].secondary,
				hostapd_get_oper_centr_freq_seg0_idx(iface->conf), prio, min_rank_cand_idx);
			fprintf(fp_hist, "selected a new channel %d, width %d, secondary %d, center %d, priority group %d candidate idx %d switch reason %s\n",
				iface->conf->channel, get_num_width(hostapd_get_oper_chwidth(iface->conf),
				candidates[min_rank_cand_idx].secondary), candidates[min_rank_cand_idx].secondary,
				hostapd_get_oper_centr_freq_seg0_idx(iface->conf), prio, min_rank_cand_idx, sw_reasons[switch_reason]);
		}
		wpa_printf(MSG_DEBUG, "ACS: exit recalculate, selected prio %d min_rank_cand_idx %d", prio, min_rank_cand_idx);
		goto end;
	}

	/* if we are still here - use the fallback channel */
	iface->conf->channel = iface->conf->acs_fallback_chan.primary;
	hostapd_set_oper_chwidth(iface->conf,get_vht_width(iface->conf->acs_fallback_chan.width));
	iface->conf->secondary_channel = get_secondary_offset(iface->conf->acs_fallback_chan.primary,
							      iface->conf->acs_fallback_chan.secondary);
	acs_smart_adjust_vht_center_freq(iface, 0);
	wpa_printf(MSG_DEBUG, "ACS: exit recalculate, using fallback channel");
	fprintf(fp_hist, "ACS: using fallback channel\n");
	res = true;

	/* WHM to trigger if ACS selects fallback channel */
	wpa_printf(MSG_ERROR, "WHM-ACS: using fallback channel new channel %d, width %d", iface->conf->channel,hostapd_get_oper_chwidth(iface->conf));
	hostapd_handle_acs_whm_warning(iface);
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

	affected_start = hostapd_chan_to_freq((iface->conf->secondary_channel < 0) ? iface->conf->channel - 4 : iface->conf->channel);
	aff_width = (iface->conf->ieee80211n && iface->conf->secondary_channel) ? 40 : 20;

	wpa_printf(MSG_DEBUG, "BT is searching for %d MHz channels", width);
	for (i = 0; i < mode->num_channels; i++) {
		chan = &mode->channels[i];

		/* Skip incompatible chandefs */
		if (chan->flag & HOSTAPD_CHAN_DISABLED)
			continue;

		if (width == 40) {
			struct hostapd_channel_data *sec_chan;

			sec_chan = hostapd_get_mode_channel(iface, chan->freq + 20);
			if (!sec_chan)
				continue;
			if (sec_chan->flag & HOSTAPD_CHAN_DISABLED)
				continue;
		}

		if (channels_overlap(affected_start, aff_width, chan->freq, width))
			continue;

		iface->freq = chan->freq;
		iface->conf->channel = chan->chan;
		iface->conf->secondary_channel = (width == 40) ? 1 : 0;

		wpa_printf(MSG_DEBUG, "Selected ch. #%d", chan->chan);
		return true;
	}

	return 0;
}

void acs_push_chandef(struct hostapd_iface *iface, acs_chandef *chan)
{
	chan->pri = iface->conf->channel;
	chan->sec = iface->conf->secondary_channel,
	chan->chwidth = hostapd_get_oper_chwidth(iface->conf);
	chan->cent_freq = hostapd_get_oper_centr_freq_seg0_idx(iface->conf);
	chan->sel_cand = iface->selected_candidate;
}

void acs_pop_chandef(struct hostapd_iface *iface, acs_chandef *chan)
{
	iface->conf->channel = chan->pri;
	iface->conf->secondary_channel = chan->sec;
	hostapd_set_oper_chwidth(iface->conf,chan->chwidth);
	hostapd_set_oper_centr_freq_seg0_idx(iface->conf,chan->cent_freq);
	iface->selected_candidate = chan->sel_cand;
}

void hostapd_ltq_update_channel_data(struct hostapd_iface *iface, const u8 *data, size_t data_len)
{
	struct intel_vendor_channel_data *ch_data = (struct intel_vendor_channel_data *) data;
	struct acs_candidate_table *candidates = iface->candidates;
	int j;

	if (!data || (data_len != sizeof(struct intel_vendor_channel_data))) {
		wpa_printf(MSG_ERROR, "bad channel data event, size %zu expected %zu",
			   data_len, sizeof(struct intel_vendor_channel_data));
		return;
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

	if (!iface->conf->acs_init_done) {
		if (ch_data->filled_mask & CHDATA_BT_INTERF_MODE) {
			if ((ch_data->primary == iface->conf->channel) &&
			    (ch_data->cwi_noise + 128 > iface->conf->acs_to_degradation[D_CWI])) {
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
					if (acs_do_switch_channel(iface, 0) < 0) {
						wpa_printf(MSG_WARNING, "BT failed to schedule CSA - trying fallback");

						/* back out with channel switch */
						acs_pop_chandef(iface, &orig_chan);

						hostapd_disable_iface(iface);
						hostapd_enable_iface(iface);
					}
				}
			}
			return;
		}
		else {
			if (!(ch_data->filled_mask & CHDATA_SCAN_MODE))
				return;
			if (iface->num_candidates == 0)
				acs_init_candidate_table(iface);
		}
	}

	if (iface->acs_update_in_progress) return;

	iface->acs_update_in_progress = 1;

	wpa_printf(MSG_DEBUG, "ACS: parse channel data event");
	wpa_printf(MSG_DEBUG, "%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d 0x%X",
		   ch_data->channel,
		   ch_data->BW,
		   ch_data->primary,
		   ch_data->secondary,
		   ch_data->freq,
		   ch_data->load,
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

	for (j = 0; j < iface->num_candidates; j++) {
		candidates[j].filled_mask = ch_data->filled_mask;

		if (candidates[j].primary == ch_data->primary) {
			wpa_printf(MSG_DEBUG, "channel data event update candidate %d chan %d BW %d pri %d sec %d freq %d",
				   j, candidates[j].chan, candidates[j].width, candidates[j].primary, candidates[j].secondary, candidates[j].freq);

			/* overwrite channel data */
			if (ch_data->filled_mask & CHDATA_LOAD)
				candidates[j].channel_load[PRI_20] = ch_data->load;

			if (ch_data->filled_mask & CHDATA_CWI_NOISE)
				candidates[j].cwi_noise[PRI_20] = ch_data->cwi_noise;

			if (ch_data->filled_mask & CHDATA_TX_POWER) {
				candidates[j].tx_power = ch_data->tx_power;
				if (ch_data->tx_power > iface->max_tx_power) iface->max_tx_power = ch_data->tx_power;
			}

			if ((20 == candidates[j].width) &&
			    (ch_data->filled_mask & CHDATA_ALL_BITS) &&
			    (ch_data->filled_mask & CHDATA_SCAN_MODE))
				candidates[j].scanned = true;
		}
		else if (candidates[j].secondary == ch_data->primary) {
			if (ch_data->filled_mask & CHDATA_LOAD)
				candidates[j].channel_load[SEC_20] = ch_data->load;

			if (ch_data->filled_mask & CHDATA_CWI_NOISE)
				candidates[j].cwi_noise[SEC_20] = ch_data->cwi_noise;
		}
	}

	if (!(ch_data->filled_mask & CHDATA_SCAN_MODE)) {
		acs_chandef cur_chan;

		acs_push_chandef(iface, &cur_chan);

		if (acs_recalc_ranks_and_set_chan(iface, SWR_UPDATE) > 0) {
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

	freq = hostapd_chan_to_freq(chan);
	for (j = 0; j < iface->num_candidates; j++) {
		if (candidates[j].width == 40 && channels_overlap(freq, 20, candidates[j].freq, 40)) {
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
		candidates[j].radar_detected = false;

		for (i = 0; i < mode->num_channels; i++) {
			struct hostapd_channel_data *chan;

			chan = &iface->current_mode->channels[i];

			if (channels_overlap(candidates[j].freq, candidates[j].width, chan->freq, 20)) {
				if ((chan->flag & HOSTAPD_CHAN_DFS_MASK) == HOSTAPD_CHAN_DFS_UNAVAILABLE) {
					candidates[j].radar_detected = true;
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

	hostapd_channel_switch_reason_set(iface, HAPD_CHAN_SWITCH_RADAR_DETECTED);

	if ((ret <= 0) || (acs_do_switch_channel(iface, 1) < 0)) {
		/* back out with channel switch */
		acs_pop_chandef(iface, &cur_chan);
		hostapd_disable_iface(iface);
		wpa_printf(MSG_ERROR, "Unable to switch channel on radar detection !");
	}
}

int hostapd_acs_get_candidates_rank_idx (struct hostapd_iface *iface,
					 int primary_channel,
					 int secondary_offset,
					 int bandwidth)
{
	struct acs_candidate_table *candidates = iface->candidates;
	int i, secondary_channel = 0;

	if (secondary_offset)
		secondary_channel = primary_channel + (secondary_offset * 4);

	for (i = 0; (i < iface->num_candidates && i < MAX_CANDIDATES); i++) {
		if ((candidates[i].primary   == primary_channel)   &&
		    (candidates[i].secondary == secondary_channel) &&
		    (candidates[i].width     == bandwidth))
		    return i;
	}

	wpa_printf(MSG_ERROR, "ACS candidate primary %d, secondary %d, bandwidth %d does not exist",
		   primary_channel, secondary_channel, bandwidth);
	return -1;
}

/* Must pass 2 adjacent bandwidths, return true if lower BW rank is better */
static bool hostapd_acs_compare_bw_utilization (int util_high_bw, int util_low_bw)
{
	if (util_high_bw < ACS_RANK_UTIL_MAX) {
		if (util_high_bw < util_low_bw / 2) {
			return true;
		}
	}

	return false;
}

/* Returns true if first rank is better than second rank, false otherwise and on errors */
bool hostapd_acs_compare_ranks (struct hostapd_iface *iface, int rank1_idx, int rank2_idx)
{
	struct acs_candidate_table *cand1, *cand2;

	if ((-1 == rank1_idx) || (-1 == rank2_idx))
		return false;

	cand1 = &iface->candidates[rank1_idx];
	cand2 = &iface->candidates[rank2_idx];

	if (cand1->width == cand2->width) {
		return cand1->rank < cand2->rank;
	}
	else if (cand1->width > cand2->width) {
		if ((cand2->width * 2) != cand1->width)
			return false;

		if (hostapd_acs_compare_bw_utilization(cand1->bw_comp_util, cand2->bw_comp_util))
			return false;

		return true;
	}
	else { /* cand1->width < cand2->width */
		if ((cand1->width * 2) != cand2->width)
			return false;

		return hostapd_acs_compare_bw_utilization(cand2->bw_comp_util, cand1->bw_comp_util);
	}
}

enum acs_failsafe_bw
{
	FAILSAFE_20,
	FAILSAFE_40,
	FAILSAFE_80,
	FAILSAFE_160,

	FAILSAFE_NUM_BW,
};
struct acs_failsafe_table
{
	int index_rank[FAILSAFE_NUM_BW];
	int index_bw_util[FAILSAFE_NUM_BW];
};

static bool
acs_candidate_dfs_available(struct hostapd_iface *iface,
			    struct acs_candidate_table *candidate)
{
	int i;
	struct hostapd_channel_data *channel;
	struct hostapd_hw_modes *mode = iface->current_mode;

	for (i = 0; i < mode->num_channels; i++) {
		channel = &mode->channels[i];

		if (channels_overlap(channel->freq, 20, candidate->freq, candidate->width)) {
			if ((channel->flag & HOSTAPD_CHAN_RADAR) &&
			    ((channel->flag & HOSTAPD_CHAN_DFS_MASK) != HOSTAPD_CHAN_DFS_AVAILABLE))
			return false;
		}
	}

	return true;
}

static void
acs_get_failsafe_indexes_per_bw(struct hostapd_iface *iface,
				struct acs_failsafe_table *table)
{
	int i;
	struct acs_candidate_table *candidate = iface->candidates;
	int best20 = INT_MAX, best40 = INT_MAX, best80 = INT_MAX, best160 = INT_MAX;
	int best20_util = 0, best40_util = 0, best80_util = 0, best160_util = 0;

	for (i = FAILSAFE_20; i < FAILSAFE_NUM_BW; i++) {
		table->index_rank[i] = -1;
		table->index_bw_util[i] = -1;
	}

	for (i = 0; (i < iface->num_candidates && i < MAX_CANDIDATES); i++) {
		if (candidate[i].rank == -1)
			continue;

		if (!acs_candidate_dfs_available(iface, &candidate[i]))
			continue;

		switch (candidate[i].width) {
		case 20:
			if (candidate[i].rank < best20) {
				best20 = candidate[i].rank;
				table->index_rank[FAILSAFE_20] = i;
			}
			if (candidate[i].bw_comp_util > best20_util) {
				best20_util = candidate[i].bw_comp_util;
				table->index_bw_util[FAILSAFE_20] = i;
			}
			break;
		case 40:
			if (candidate[i].rank < best40) {
				best40 = candidate[i].rank;
				table->index_rank[FAILSAFE_40] = i;
			}
			if (candidate[i].bw_comp_util > best40_util) {
				best40_util = candidate[i].bw_comp_util;
				table->index_bw_util[FAILSAFE_40] = i;
			}
			break;
		case 80:
			if (candidate[i].rank < best80) {
				best80 = candidate[i].rank;
				table->index_rank[FAILSAFE_80] = i;
			}
			if (candidate[i].bw_comp_util > best80_util) {
				best80_util = candidate[i].bw_comp_util;
				table->index_bw_util[FAILSAFE_80] = i;
			}
			break;
		case 160:
			if (candidate[i].rank < best160) {
				best160 = candidate[i].rank;
				table->index_rank[FAILSAFE_160] = i;
			}
			if (candidate[i].bw_comp_util > best160_util) {
				best160_util = candidate[i].bw_comp_util;
				table->index_bw_util[FAILSAFE_160] = i;
			}
			break;
		default:
			continue;
		}
	}
}

struct acs_candidate_table *
acs_get_failsafe_channel(struct hostapd_iface *iface)
{
	int i;
	int best_rank_idx = -1;
	struct acs_failsafe_table table = {0};
	struct acs_candidate_table *candidate = iface->candidates;

	if (iface->num_candidates == 0) {
		wpa_printf(MSG_DEBUG, "Dynamic failsafe channel: ACS candidates not initialized");
		return NULL;
	}

	if (!acs_are_all_available_chans_scanned(iface)) {
		wpa_printf(MSG_DEBUG, "Dynamic failsafe channel: Not all available channels scanned");
		return NULL;
	}

	acs_get_failsafe_indexes_per_bw(iface, &table);

	if (!iface->conf->acs_bw_comparison) {
		int current_bw = 20;
		u8 config_bw = hostapd_get_oper_chwidth(iface->conf);
		if (iface->conf->secondary_channel)
			current_bw = 40;
		if (config_bw == CHANWIDTH_80MHZ)
			current_bw = 80;
		else if(config_bw == CHANWIDTH_160MHZ)
			current_bw = 160;

		for (i = FAILSAFE_160; i >= FAILSAFE_20; i--) {
			if ((-1 != table.index_rank[i]) &&
			    (candidate[table.index_rank[i]].width <= current_bw))
				return &candidate[table.index_rank[i]];
		}

		wpa_printf(MSG_ERROR, "Dynamic failsafe channel: No DFS AVAILABLE channels available");
		return NULL;
	}

	/* BW comparison */
	for (i = FAILSAFE_160; i > FAILSAFE_20; i--) {
		if ((-1 == table.index_bw_util[i]) || (-1 == table.index_bw_util[i - 1])) {
			continue;
		}

		if (hostapd_acs_compare_bw_utilization(candidate[table.index_bw_util[i]].bw_comp_util,
						       candidate[table.index_bw_util[i - 1]].bw_comp_util))
			best_rank_idx = table.index_bw_util[i - 1];
		else
			best_rank_idx = table.index_bw_util[i];

		break;
	}

	/* Should never happen */
	if (-1 == best_rank_idx)
		return NULL;

	return &candidate[best_rank_idx];
}

void acs_set_bg_scan_callback(struct hostapd_iface *iface)
{
	if (iface->conf->acs_init_done)
		iface->scan_cb = acs_bg_scan_complete;
}
