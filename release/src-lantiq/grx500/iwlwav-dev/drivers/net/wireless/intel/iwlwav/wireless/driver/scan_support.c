#include "mtlkinc.h"
#include "scan_support.h"
#include "mhi_umi.h"
#include "mhi_umi_propr.h"
#include "mtlk_vap_manager.h"
#include "mtlk_param_db.h"
#include "mtlk_df_priv.h"
#include "mtlk_df_nbuf.h"
#include "core.h"
#include "core_config.h"
#include "mtlkhal.h"
#include "cfg80211.h"
#include "mac80211.h"
#include "mtlk_df.h"
#include "hw_mmb.h"
#include "vendor_cmds.h"

#define LOG_LOCAL_GID GID_SCAN_SUPPORT
#define LOG_LOCAL_FID 0

#define MTLK_CHANNEL_CW_MASK_5_2_GHZ        0x07  /* Mask to calibrate 20, 40 and 80 MHz bandwidths */
#define MTLK_CHANNEL_CW_MASK_2_4_GHZ        0x03  /* Mask to calibrate 20 and 40 MHz bandwidths */

/* FIXME: HW dependent timeout */
#define MTLK_MM_BLOCKED_CALIBRATION_TIMEOUT_GEN6 300000 /* ms */

/* Conversion from 2.4 and 5 GHz channel index in our array of channel scan support data to channel number */
const uint8 chanidx2num_2_5[1 + NUM_TOTAL_CHANS] =
{
  /* --Idx--*/
  /*  0..14 */ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
  /* 15..22 */ 34, 36, 38, 40, 42, 44, 46, 48,
  /* 23..26 */ 52, 56, 60, 64,
  /* 27..38 */ 100, 104, 108, 112, 116, 120, 124, 128, 132, 136, 140, 144,
  /* 39..43 */ 149, 153, 157, 161, 165,
  /* 44..47 */ 184, 188, 192, 196
};

/* Conversion from 2.4 and 5 GHz channel number to its index in our array of channel scan support_data
 * Include also central freqs for 40 MHz mode, in total NUM_5GHZ_CENTRAL_FREQS_BW40
 *   Channels 54, 62, 102, 110, 118, 126, 134, 142, 151, 159 --> Idx  48 ... 57
 * And central freqs for 80 MHz mode, in total NUM_5GHZ_CENTRAL_FREQS_BW80
 *   Channels 58, 106, 122, 138, 155 --> Idx  58 ... 62
 *   Channel 42 is already listed in the table
 * And central freqs for 160 MHz mode, in total NUM_5GHZ_CENTRAL_FREQS_BW160
 *   Channels 50 and 114 --> Idx 63 and 64
 */

/* this table uses indices from 0 to NUM_TOTAL_CHANS and to NUM_TOTAL_CHAN_FREQS, so we do a compile-time check here */
#if (47 != NUM_TOTAL_CHANS)
#error "NUM_TOTAL_CHANS is wrong"
#endif
#if (64 != NUM_TOTAL_CHAN_FREQS)
#error "NUM_TOTAL_CHAN_FREQS is wrong"
#endif

const uint8 channum2idx_2_5[256] =
{
   0,  1,  2,  3,  4,  5,  6,  7,  8,  9, /* 0--9 */
  10, 11, 12, 13, 14,  0,  0,  0,  0,  0, /* 10--19 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 20--29 */
   0,  0,  0,  0, 15,  0, 16,  0, 17,  0, /* 30--39 */
  18,  0, 19,  0, 20,  0, 21,  0, 22,  0, /* 40--49 */
  63,  0, 23,  0, 48,  0, 24,  0, 58,  0, /* 50--59 */
  25,  0, 49,  0, 26,  0,  0,  0,  0,  0, /* 60--69 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 70--79 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 80--89 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 90--99 */
  27,  0, 50,  0, 28,  0, 59,  0, 29,  0, /* 100--109 */
  51,  0, 30,  0, 64,  0, 31,  0, 52,  0, /* 110--119 */
  32,  0, 60,  0, 33,  0, 53,  0, 34,  0, /* 120--129 */
   0,  0, 35,  0, 54,  0, 36,  0, 61,  0, /* 130--139 */
  37,  0, 55,  0, 38,  0,  0,  0,  0, 39, /* 140--149 */
   0, 56,  0, 40,  0, 62,  0, 41,  0, 57, /* 150--159 */
   0, 42,  0,  0,  0, 43,  0,  0,  0,  0, /* 160--169 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 170--179 */
   0,  0,  0,  0, 44,  0,  0,  0, 45,  0, /* 180--189 */
   0,  0, 46,  0,  0,  0, 47,  0,  0,  0, /* 190--199 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 200--209 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 210--219 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 220--229 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 230--239 */
   0,  0,  0,  0,  0,  0,  0,  0,  0,  0, /* 240--249 */
   0,  0,  0,  0,  0,  0                  /* 250--255 */
};

/* IEEE 802.11 5 GHz channel bonding
 *   BW  Channels
 *   20:  36  40  44  48  52  56  60  64   100 104 108 112  116 120 124 128  132 136 140 144  149 153 157 161  165
 *   40:    38      46      54      62       102     110      118     126      134     142      151     159
 *   80:        42              58               106              122              138              155
 *  160:                50                               114
 */

/* Channel bonding table.
 * First element of chan_lower array is for BW 20 MHz i.e. chan_lower[0] = primary channel number.
 * A zero value means that BW is not supported.
 */

static wave_chan_bonding_t _chan_5G_bonding_table[] = {
    /*    Lower Channel             Center Channel              Upper Channel    */
    /*  20   40   80  160          20   40   80  160          20   40   80  160  */
    {{  36,  36,  36,  36 },    {  36,  38,  42,  50 },    {  36,  40,  48,  64 }},
    {{  40,  36,  36,  36 },    {  40,  38,  42,  50 },    {  40,  40,  48,  64 }},
    {{  44,  44,  36,  36 },    {  44,  46,  42,  50 },    {  44,  48,  48,  64 }},
    {{  48,  44,  36,  36 },    {  48,  46,  42,  50 },    {  48,  48,  48,  64 }},
    {{  52,  52,  52,  36 },    {  52,  54,  58,  50 },    {  52,  56,  64,  64 }},
    {{  56,  52,  52,  36 },    {  56,  54,  58,  50 },    {  56,  56,  64,  64 }},
    {{  60,  60,  52,  36 },    {  60,  62,  58,  50 },    {  60,  64,  64,  64 }},
    {{  64,  60,  52,  36 },    {  64,  62,  58,  50 },    {  64,  64,  64,  64 }},

    {{ 100, 100, 100, 100 },    { 100, 102, 106, 114 },    { 100, 104, 112, 128 }},
    {{ 104, 100, 100, 100 },    { 104, 102, 106, 114 },    { 104, 104, 112, 128 }},
    {{ 108, 108, 100, 100 },    { 108, 110, 106, 114 },    { 108, 112, 112, 128 }},
    {{ 112, 108, 100, 100 },    { 112, 110, 106, 114 },    { 112, 112, 112, 128 }},
    {{ 116, 116, 116, 100 },    { 116, 118, 122, 114 },    { 116, 120, 128, 128 }},
    {{ 120, 116, 116, 100 },    { 120, 118, 122, 114 },    { 120, 120, 128, 128 }},
    {{ 124, 124, 116, 100 },    { 124, 126, 122, 114 },    { 124, 128, 128, 128 }},
    {{ 128, 124, 116, 100 },    { 128, 126, 122, 114 },    { 128, 128, 128, 128 }},

    {{ 132, 132, 132,   0 },    { 132, 134, 138,   0 },    { 132, 136, 144,   0 }},
    {{ 136, 132, 132,   0 },    { 136, 134, 138,   0 },    { 136, 136, 144,   0 }},
    {{ 140, 140, 132,   0 },    { 140, 142, 138,   0 },    { 140, 144, 144,   0 }},
    {{ 144, 140, 132,   0 },    { 144, 142, 138,   0 },    { 144, 144, 144,   0 }},

    {{ 149, 149, 149,   0 },    { 149, 151, 155,   0 },    { 149, 153, 161,   0 }},
    {{ 153, 149, 149,   0 },    { 153, 151, 155,   0 },    { 153, 153, 161,   0 }},
    {{ 157, 157, 149,   0 },    { 157, 159, 155,   0 },    { 157, 161, 161,   0 }},
    {{ 161, 157, 149,   0 },    { 161, 159, 155,   0 },    { 161, 161, 161,   0 }},

    {{ 165,   0,   0,   0 },    { 165,   0,   0,   0 },    { 165,   0,   0,   0 }},
};

/* TODO: To review the table for 6GHz band scan */
static wave_chan_bonding_t _chan_6G_bonding_table[] = {
    /*         Lower Channel             Center Channel              Upper Channel                  */
    /*    20   40      80     160        20    40      80      160       20    40       80     160  */
        {{1,    1,      1,      1},     {1,     3,      7,      15},    {1,     5,      13,     29}},
        {{5,    1,      1,      1},     {5,     3,      7,      15},    {5,     5,      13,     29}},
        {{9,    9,      1,      1},     {9,     11,     7,      15},    {9,     13,     13,     29}},
        {{13,   9,      1,      1},     {13,    11,     7,      15},    {13,    13,     13,     29}},
        {{17,   17,     17,     1},     {17,    19,     23,     15},    {17,    21,     29,     29}},
        {{21,   17,     17,     1},     {21,    19,     23,     15},    {21,    21,     29,     29}},
        {{25,   25,     17,     1},     {25,    27,     23,     15},    {25,    29,     29,     29}},
        {{29,   25,     17,     1},     {29,    27,     23,     15},    {29,    29,     29,     29}},
        {{33,   33,     33,     33},    {33,    35,     39,     47},    {33,    37,     45,     61}},
        {{37,   33,     33,     33},    {37,    35,     39,     47},    {37,    37,     45,     61}},
        {{41,   41,     33,     33},    {41,    43,     39,     47},    {41,    45,     45,     61}},
        {{45,   41,     33,     33},    {45,    43,     39,     47},    {45,    45,     45,     61}},
        {{49,   49,     49,     33},    {49,    51,     55,     47},    {49,    53,     61,     61}},
        {{53,   49,     49,     33},    {53,    51,     55,     47},    {53,    53,     61,     61}},
        {{57,   57,     49,     33},    {57,    59,     55,     47},    {57,    61,     61,     61}},
        {{61,   57,     49,     33},    {61,    59,     55,     47},    {61,    61,     61,     61}},
        {{65,   65,     65,     65},    {65,    67,     71,     79},    {65,    69,     77,     93}},
        {{69,   65,     65,     65},    {69,    67,     71,     79},    {69,    69,     77,     93}},
        {{73,   73,     65,     65},    {73,    75,     71,     79},    {73,    77,     77,     93}},
        {{77,   73,     65,     65},    {77,    75,     71,     79},    {77,    77,     77,     93}},
        {{81,   81,     81,     65},    {81,    83,     87,     79},    {81,    85,     93,     93}},
        {{85,   81,     81,     65},    {85,    83,     87,     79},    {85,    85,     93,     93}},
        {{89,   89,     81,     65},    {89,    91,     87,     79},    {89,    93,     93,     93}},
        {{93,   89,     81,     65},    {93,    91,     87,     79},    {93,    93,     93,     93}},
        {{97,   97,     97,     97},    {97,    99,     103,    111},   {97,    101,    109,    125}},
        {{101,  97,     97,     97},    {101,   99,     103,    111},   {101,   101,    109,    125}},
        {{105,  105,    97,     97},    {105,   107,    103,    111},   {105,   109,    109,    125}},
        {{109,  105,    97,     97},    {109,   107,    103,    111},   {109,   109,    109,    125}},
        {{113,  113,    113,    97},    {113,   115,    119,    111},   {113,   117,    125,    125}},
        {{117,  113,    113,    97},    {117,   115,    119,    111},   {117,   117,    125,    125}},
        {{121,  121,    113,    97},    {121,   123,    119,    111},   {121,   125,    125,    125}},
        {{125,  121,    113,    97},    {125,   123,    119,    111},   {125,   125,    125,    125}},
        {{129,  129,    129,    129},   {129,   131,    135,    143},   {129,   133,    141,    157}},
        {{133,  129,    129,    129},   {133,   131,    135,    143},   {133,   133,    141,    157}},
        {{137,  137,    129,    129},   {137,   139,    135,    143},   {137,   141,    141,    157}},
        {{141,  137,    129,    129},   {141,   139,    135,    143},   {141,   141,    141,    157}},
        {{145,  145,    145,    129},   {145,   147,    151,    143},   {145,   149,    157,    157}},
        {{149,  145,    145,    129},   {149,   147,    151,    143},   {149,   149,    157,    157}},
        {{153,  153,    145,    129},   {153,   155,    151,    143},   {153,   157,    157,    157}},
        {{157,  153,    145,    129},   {157,   155,    151,    143},   {157,   157,    157,    157}},
        {{161,  161,    161,    161},   {161,   163,    167,    175},   {161,   165,    173,    189}},
        {{165,  161,    161,    161},   {165,   163,    167,    175},   {165,   165,    173,    189}},
        {{169,  169,    161,    161},   {169,   171,    167,    175},   {169,   173,    173,    189}},
        {{173,  169,    161,    161},   {173,   171,    167,    175},   {173,   173,    173,    189}},
        {{177,  177,    177,    161},   {177,   179,    183,    175},   {177,   181,    189,    189}},
        {{181,  177,    177,    161},   {181,   179,    183,    175},   {181,   181,    189,    189}},
        {{185,  185,    177,    161},   {185,   187,    183,    175},   {185,   189,    189,    189}},
        {{189,  185,    177,    161},   {189,   187,    183,    175},   {189,   189,    189,    189}},
        {{193,  193,    193,    193},   {193,   195,    199,    207},   {193,   197,    205,    221}},
        {{197,  193,    193,    193},   {197,   195,    199,    207},   {197,   197,    205,    221}},
        {{201,  201,    193,    193},   {201,   203,    199,    207},   {201,   205,    205,    221}},
        {{205,  201,    193,    193},   {205,   203,    199,    207},   {205,   205,    205,    221}},
        {{209,  209,    209,    193},   {209,   211,    215,    207},   {209,   213,    221,    221}},
        {{213,  209,    209,    193},   {213,   211,    215,    207},   {213,   213,    221,    221}},
        {{217,  217,    209,    193},   {217,   219,    215,    207},   {217,   221,    221,    221}},
        {{221,  217,    209,    193},   {221,   219,    215,    207},   {221,   221,    221,    221}},
        {{225,  225,    0,      0},     {225,   227,    0,      0},     {225,   229,    0,      0}},
        {{229,  225,    0,      0},     {229,   227,    0,      0},     {229,   229,    0,      0}},
        {{233,  0,      0,      0},     {233,   0,      0,      0},     {233,   0,      0,      0}}
    
};

static wave_chan_bonding_t *
_wave_scan_support_find_5G_chan_bonding (unsigned chan) {
  wave_chan_bonding_t *entry;
  wave_chan_bonding_t *found = NULL;
  unsigned i;

  for (i = 0; i < MTLK_ARRAY_SIZE(_chan_5G_bonding_table); i++) {
    entry = &_chan_5G_bonding_table[i];
    if (chan == entry->lower_chan[0]) {
      found = entry;
    }
  }

  if (!found) {
    ELOG_D("Unsupported channel %u", chan);
  }

  return found;
}

static wave_chan_bonding_t *
_wave_scan_support_find_6G_chan_bonding (unsigned chan) {
  wave_chan_bonding_t *entry;
  wave_chan_bonding_t *found = NULL;
  unsigned i;

  for (i = 0; i < MTLK_ARRAY_SIZE(_chan_6G_bonding_table); i++) {
    entry = &_chan_6G_bonding_table[i];
    if (chan == entry->lower_chan[0]) {
      found = entry;
    }
  }

  if (!found) {
    ELOG_D("Unsupported channel %u", chan);
  }

  return found;
}

void __MTLK_IFUNC
wave_scan_support_fill_chan_bonding_by_chan (wave_chan_bonding_t *out_data, unsigned center_chan, unsigned primary_chan)
{
  wave_chan_bonding_t *cfg = NULL;

  MTLK_ASSERT(out_data);
  MTLK_ASSERT(MAX_UINT8 >= primary_chan);
  MTLK_ASSERT(MAX_UINT8 >= center_chan);

  memset(out_data, 0, sizeof(*out_data));

  if (MTLK_HW_BAND_5_2_GHZ == channel_to_band(primary_chan)) {
    /* 5 GHz band -- search in the table and fill if found */
    cfg = _wave_scan_support_find_5G_chan_bonding(primary_chan);
    if (cfg) {
      *out_data = *cfg;
    }
  } else {
    /* 2.4 GHz band -- fill out_data for BWs 20 MHz and 40 MHz */
    /* Fill BW 20 data always i.e. for any BW setting */
    out_data->center_chan[CW_20] = primary_chan;
    out_data->lower_chan[CW_20]  = primary_chan;
    out_data->upper_chan[CW_20]  = primary_chan;
    /* Fill BW 40 data only if required */
    if (primary_chan != center_chan) { /* BW 40 */
      unsigned lower, upper;
      if (primary_chan < center_chan) {
        lower = primary_chan;
        upper = primary_chan + CHANNUMS_PER_20MHZ;
      } else {
        lower = primary_chan - CHANNUMS_PER_20MHZ;
        upper = primary_chan;
      }

      out_data->center_chan[CW_40] = center_chan;
      out_data->lower_chan[CW_40]  = lower;
      out_data->upper_chan[CW_40]  = upper;
    }
  }
}

void __MTLK_IFUNC
wave_scan_support_fill_chan_bonding_by_chan_6g (wave_chan_bonding_t *out_data, unsigned center_chan, unsigned primary_chan)
{
  wave_chan_bonding_t *cfg = NULL;

  MTLK_ASSERT(out_data);
  MTLK_ASSERT(MAX_UINT8 >= primary_chan);
  MTLK_ASSERT(MAX_UINT8 >= center_chan);

  memset(out_data, 0, sizeof(*out_data));

  /* 6 GHz band -- search in the table and fill if found */
    cfg = _wave_scan_support_find_6G_chan_bonding(primary_chan);
    if (cfg) {
      *out_data = *cfg;
    }
}

void __MTLK_IFUNC
wave_scan_support_fill_chan_bonding_by_freq (wave_chan_bonding_t *out_data, unsigned center_freq, unsigned primary_freq)
{
  unsigned centre_chan, pri_chan;
  centre_chan = ieee80211_frequency_to_channel(center_freq);
  pri_chan = ieee80211_frequency_to_channel(primary_freq);
  if(WAVE_FREQ_IS_6G(center_freq)) {
    return wave_scan_support_fill_chan_bonding_by_chan_6g(out_data, centre_chan, pri_chan);
  } else {
    return wave_scan_support_fill_chan_bonding_by_chan(out_data, centre_chan, pri_chan);
  }
}

/* the scan timer calls this one, which passes the serious work to the serializer */
/* any context calling abort_scan may also call it */
int scan_hrtimer_clb_func(mtlk_osal_hrtimer_t *timer, mtlk_handle_t nic)
{
  mtlk_core_t *core = (mtlk_core_t *) nic;

  MTLK_ASSERT(core == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle)));

  if (wave_rcvry_mac_fatal_pending_get(mtlk_vap_get_hw(core->vap_handle)))
    return MTLK_ERR_OK;

  /* this interface is way too heavy for the simple context switch we need to do */
  _mtlk_df_user_invoke_core_async(mtlk_vap_get_df(core->vap_handle), WAVE_RADIO_REQ_SCAN_TIMEOUT, NULL, 0, NULL, 0);

  return MTLK_ERR_OK;
}

/* called from radio module */
int scan_support_init(mtlk_core_t *core, mtlk_scan_support_t *ss)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_pdb_size_t size;
  int res = MTLK_ERR_OK;

  memset(ss, 0, sizeof(*ss));

  ss->last_chan_idx = -1;
  ss->last_chan_band = -1;
  size = sizeof(ss->iwpriv_scan_params);
  res = WAVE_RADIO_PDB_GET_BINARY(radio, PARAM_DB_RADIO_SCAN_PARAMS, &ss->iwpriv_scan_params, &size);

  ss->PassiveValid = msecs_to_jiffies(1000 * ss->iwpriv_scan_params.passive_scan_valid_time);
  ss->ActiveValid = msecs_to_jiffies(1000 * ss->iwpriv_scan_params.active_scan_valid_time);

#ifdef CONFIG_WAVE_DEBUG
  ss->dfs_debug_params.beacon_count    = -1;
#endif
  ss->master_core = core; /* save the core too for convenience */

  mtlk_osal_hrtimer_init(&ss->scan_timer, &scan_hrtimer_clb_func, HANDLE_T(core), HRTIMER_MODE_ABS);
  mtlk_osal_event_init(&ss->scan_end_event);

  return res;
}

/* called from radio module */
void scan_support_cleanup(mtlk_scan_support_t *ss)
{
  mtlk_osal_event_cleanup(&ss->scan_end_event);
  mtlk_osal_hrtimer_cleanup(&ss->scan_timer);
}


int low_chan_160[] = { 36, 100 };
int low_chan_80[] = { 36, 52, 100, 116, 132, 149 };
int low_chan_40[] = { 36, 44, 52, 60, 100, 108, 116, 124, 132, 140, 149, 157 };

/* Is the channel with the given chan_num allowed as lower channel ? */
static __INLINE
int low_chan_allowed(int chan_num, unsigned cw, mtlk_hw_band_e band)
{
  int i, low_count, *low_list;

  if (MTLK_HW_BAND_5_2_GHZ != band) return TRUE; /* only checking on 5.2 */

  switch (cw) {
    case CW_160:
      low_list = low_chan_160;
      low_count = sizeof(low_chan_160) / sizeof(int);
      break;

    case CW_80:
      low_list = low_chan_80;
      low_count = sizeof(low_chan_80) / sizeof(int);
      break;

    case CW_40:
      low_list = low_chan_40;
      low_count = sizeof(low_chan_40) / sizeof(int);
      break;

    case CW_20:
      return TRUE;

    default:
      ELOG_D("unsupported channel width %d", cw);
      return 0;
  }

  for (i = 0; i < low_count; i++)
    if (chan_num == low_list[i]) return TRUE;

  return 0;
}


/* Is the channel with the given chan_num enabled in the array of channels? */
static __INLINE
int is_chan_enabled(struct mtlk_channel channels[], int num_channels, int chan_num)
{
  int i;

  for (i = 0; i < num_channels; i++)
    if (freq2lowchannum(channels[i].center_freq, CW_20) == chan_num)
      return !(channels[i].flags & MTLK_CHAN_DISABLED);

  return 0;
}

int send_calibrate(mtlk_core_t *core, mtlk_hw_band_e band, unsigned cw,
                   const u8 chans[MAX_CALIB_CHANS], unsigned num_chans, uint32 *status)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_CALIBRATE_PARAMS *req;
  int res = MTLK_ERR_OK;

  ILOG3_V("Running");

  /* prepare msg for the FW */
  if (!(man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), &res)))
  {
    ELOG_DD("CID-%04x: UM_MAN_CALIBRATE_REQ init failed, err=%i",
            mtlk_vap_get_oid(core->vap_handle), res);
    res = MTLK_ERR_NO_RESOURCES;
    goto end;
  }

  man_entry->id = UM_MAN_CALIBRATE_REQ;
  man_entry->payload_size = sizeof(*req);
  req = (UMI_CALIBRATE_PARAMS *) man_entry->payload;
  memset(req, 0, sizeof(*req));

  wave_memcpy(req->chan_nums, sizeof(req->chan_nums), chans, num_chans);
  req->num_chans = num_chans;
  req->chan_width = cw;

  mtlk_dump(1, req, sizeof(*req), "dump of UMI_CALIBRATE_PARAMS:");

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_CALIBRATION_TIMEOUT_GEN6);

  if (res != MTLK_ERR_OK)
  {
      ELOG_DD("CID-%04x: UM_MAN_CALIBRATE_REQ send failed, err=%i",
              mtlk_vap_get_oid(core->vap_handle), res);
  }
  else if (req->Status)
  {
    ELOG_DD("CID-%04x: UM_MAN_CALIBRATE_REQ execution failed, status=0x%08x",
            mtlk_vap_get_oid(core->vap_handle), req->Status);
    *status = req->Status;
  }

  mtlk_txmm_msg_cleanup(&man_msg);

end:
  return res;
}

static __INLINE
unsigned mtlk_channum2cssidx(int chan_num, mtlk_hw_band_e band)
{
  return ((band == MTLK_HW_BAND_6_GHZ) ? channum6g2cssidx(chan_num) : channum2csysidx(chan_num));
}

/* PHY_RX_STATUS */
static struct channel_survey_support *
scan_get_csys_by_chan(mtlk_core_t *core, uint32 channel, mtlk_hw_band_e band)
{
  unsigned cssidx = mtlk_channum2cssidx(channel, band);
  struct channel_survey_support *csys = NULL;
  mtlk_scan_support_t *ss = __wave_core_scan_support_get(core);

  if (cssidx != CHAN_IDX_ILLEGAL)
    csys = ((band == MTLK_HW_BAND_6_GHZ) ? &ss->csys_6g[cssidx] : &ss->csys[cssidx]);
  return csys;
}

static struct channel_survey_support *
scan_get_curr_csys(mtlk_core_t *core)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  uint32 channel = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_CHANNEL_CUR);

  if (!channel) /* is not set yet */
    return NULL;
  return scan_get_csys_by_chan(core, channel, wave_radio_band_get(radio));
}

void scan_update_curr_chan_info(mtlk_core_t *core, int noise, unsigned ch_load)
{
    struct channel_survey_support *csys;

    csys = scan_get_curr_csys(core);
    if (NULL != csys) {
        ILOG3_DDD("CID-%04x: noise %d, ch_load %d",
                   mtlk_vap_get_oid(core->vap_handle),
                   noise, ch_load);

        csys->load  = ch_load;
        csys->noise = noise;
    }
}

void scan_chan_survey_set_chan_load (struct channel_survey_support *csys, unsigned ch_load)
{
    ILOG3_DD("Set channel load: 0x%02X (%d)", ch_load, ch_load);
    if (csys)
        csys->load = ch_load;
}

int scan_send_get_chan_load(const mtlk_core_t *core, uint32 *chan_load)
{
  mtlk_txmm_msg_t            man_msg;
  mtlk_txmm_data_t          *man_entry;
  UMI_GET_CHANNEL_LOAD_REQ  *req;
  int res = MTLK_ERR_OK;

  /* prepare msg for the FW */
  if (!(man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), &res))) {
    ELOG_D("CID-%04x: GET_CHANNEL_LOAD_REQ init failed", mtlk_vap_get_oid(core->vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_GET_CHANNEL_LOAD_REQ;
  man_entry->payload_size = sizeof(*req);
  req = (UMI_GET_CHANNEL_LOAD_REQ *)man_entry->payload;
  memset(req, 0, sizeof(*req));

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (res != MTLK_ERR_OK) {
    ELOG_DD("CID-%04x: GET_CHANNEL_LOAD_REQ send failed, err=%i",
      mtlk_vap_get_oid(core->vap_handle), res);
  }
  else {
    *chan_load = MAC_TO_HOST32(req->channelLoad);
  }

  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

/* Arranges that the scan gets aborted right away but does not wait for it to really end */
void abort_scan(struct scan_support *ss)
{
  if (is_scan_running(ss))
  {
    ILOG0_V("Aborting the scan");
    ss->flags |= SDF_ABORT;

    /* we ought to have a memory write barrier between setting the flag and enqueueing the scan timer func
     * but the timer cancellation function is complicated enough (locks, etc.)
     * that it should serve as such automatically
     */
    mtlk_osal_hrtimer_cancel_sync(&ss->scan_timer);

    /* call it one more time to make sure it has seen the ABORT flag set */
    scan_hrtimer_clb_func(NULL, HANDLE_T(ss->master_core));
  }
}

static int scan_timeout_func(mtlk_core_t *core, mtlk_scan_support_t *ss);

/* Arranges that the scan gets aborted right away and waits for it. */
/* supposed to be run on the master VAP's serializer */
int abort_scan_sync(mtlk_core_t *mcore)
{
  struct scan_support *ss = __wave_core_scan_support_get(mcore);

  MTLK_ASSERT(!in_atomic());
  MTLK_ASSERT(mcore == mtlk_core_get_master(mcore));

  if (!is_scan_running(ss))
    return MTLK_ERR_OK;

  ILOG0_V("Aborting the scan and waiting for it to end");
  ss->flags |= SDF_ABORT;

  mtlk_osal_hrtimer_cancel_sync(&ss->scan_timer);

  /* we need to call the scan timeout function to make sure it has seen the ABORT flag */
  scan_timeout_func(mcore, ss);

  return MTLK_ERR_OK;
}

/* supposed to be run on the master VAP's serializer */
int pause_or_prevent_scan(mtlk_core_t *mcore)
{
  int res = MTLK_ERR_UNKNOWN;
  struct scan_support *ss = __wave_core_scan_support_get(mcore);

  MTLK_ASSERT(!in_atomic());
  MTLK_ASSERT(mcore == mtlk_core_get_master(mcore));

  if (ss->flags & SDF_PAUSE_OR_PREVENT)
  {
    ELOG_V("The scan has already been paused/prevented");
    return MTLK_ERR_OK;
  }

  ss->flags |= SDF_PAUSE_OR_PREVENT; /* prevent a new scan from starting */

  if (!is_scan_running(ss))
  {
    ILOG2_V("New scans prevented");
    return MTLK_ERR_OK;
  }

  ILOG2_V("Pausing the scan");
  mtlk_osal_hrtimer_cancel_sync(&ss->scan_timer);

  res = scan_timeout_func(mcore, ss);

  ILOG2_V("Scan paused");
  return res;
}

/* supposed to be run on the master VAP's serializer */
int resume_or_allow_scan(mtlk_core_t *mcore)
{
  struct scan_support *ss = __wave_core_scan_support_get(mcore);
  int res = MTLK_ERR_UNKNOWN;

  MTLK_ASSERT(!in_atomic());
  MTLK_ASSERT(mcore == mtlk_core_get_master(mcore));

  if (!(ss->flags & SDF_PAUSE_OR_PREVENT))
  {
    ELOG_V("The scan is not paused/prevented");
    return MTLK_ERR_BUSY;
  }

  ss->flags &= ~SDF_PAUSE_OR_PREVENT;
  ss->scan_next_ktime = ktime_set(0,0); /* reset timer for synchronization */

  if (!is_scan_running(ss))
  {
    ILOG2_V("New scans allowed");
    return MTLK_ERR_OK;
  }

  ILOG2_V("Resuming the scan");

  res = scan_timeout_func(mcore, ss);

  ILOG2_S("Scan resume %s", (res == MTLK_ERR_OK) ? "Success" : "Failed");

  return res;
}

static BOOL is_channel_disabled(struct mtlk_chan_def *cd)
{
  return (cd->chan.flags & MTLK_CHAN_DISABLED);
}

static void
_mtlk_update_scan_pause(mtlk_vap_handle_t vap_handle, struct scan_support *ss)
{
  mtlk_df_t *df               = mtlk_vap_get_df(vap_handle);
  mtlk_df_user_t *df_user     = mtlk_df_get_user(df);
  struct wireless_dev *wdev   = mtlk_df_user_get_wdev(df_user);
  BOOL is_pause = FALSE;

  if (WAVE_RADIO_PDB_GET_INT(wave_vap_radio_get(vap_handle), PARAM_DB_RADIO_SCAN_PAUSE_BG_CACHE)) {
    if (((ss->flags & SDF_BACKGROUND) && (ss->flags & SDF_BG_BREAK)) ||
         (!ss->flags & SDF_RUNNING)) {
      is_pause = TRUE;
    }
  }

  wv_cfg80211_set_scan_pause(wdev, is_pause);
}

/* gets done only from the master serializer's context */
int clean_up_after_this_scan(mtlk_core_t *core, struct scan_support *ss)
{
  mtlk_df_t *df = mtlk_vap_get_df(core->vap_handle);
  const char *name = mtlk_df_get_name(df);
  int res = MTLK_ERR_OK;
  struct mtlk_chan_def *cd = __wave_core_chandef_get(core);
  int cur_freq = cd->center_freq1;

  MTLK_UNREFERENCED_PARAM(name); /* printouts only */

  /* Make the current channel "uncertain". it will probably stay uncertain until
   * hostapd issues some command that will end up setting it. We do the above as
   * it's nice to ignore frames because they're quite likely not on the channel
   * we're going to use and because we haven't yet bothered to set the channel number in param DB.
   */
  cd->center_freq1 = 0;

  wmb(); /* so that channel gets marked uncertain before we mark the scan as having ended */

  ss->flags = 0; /* do this before set_channel as it influences the way set_channel is done */

  wmb(); /* so that set_channel really knows the scan has ended */
  _mtlk_update_scan_pause(core->vap_handle, ss);

  /* we have to restore the channel, if we had one, and set normal mode (not scan mode) */
  if (is_channel_certain(&ss->orig_chandef) && !is_channel_disabled(&ss->orig_chandef)) {
    ILOG1_S("%s: Setting orig channel", name);
    res = core_cfg_set_chan(core, &ss->orig_chandef, NULL); /* this takes care of hdk config, etc. */
  }
  else {
    cd->center_freq1 = cur_freq;
    res = wave_radio_set_first_non_dfs_chandef(wave_vap_radio_get(core->vap_handle));
    if (res == MTLK_ERR_OK) {
      ILOG0_SD("%s: Setting channel: %d to normal", name, cd->center_freq1);
      res = core_cfg_set_chan(core, cd, NULL);
    }
  }

  /* else it will soon get set explicitly by hostapd and most likely has been set by the scan, just the mode is still SCAN */

  if (ss->set_chan_man_msg.data) /* if the set_chan msg was initialized */
  {
    mtlk_txmm_msg_cleanup(&ss->set_chan_man_msg);
    ss->set_chan_man_msg.data = NULL;
  }

  if (ss->probe_req)
  {
    mtlk_osal_mem_free(ss->probe_req);
    ss->probe_req = NULL;
    ss->probe_req_len = 0;
  }

  ss->last_chan_idx = -1;
  ss->chan_in_chunk = 0;

  return res;
}

int mtlk_alter_scan(mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  unsigned size;
  int res = MTLK_ERR_OK;
  mtlk_alter_scan_t *alter_scan;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **)data;
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);

  MTLK_ASSERT(sizeof(mtlk_clpb_t *) == data_size);

  /* supposed to be run on the Master VAP's serializer */
  MTLK_ASSERT(core == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle)));

  alter_scan = mtlk_clpb_enum_get_next(clpb, &size);
  MTLK_CLPB_TRY(alter_scan, size)
    if (alter_scan->abort_scan)
      abort_scan_sync(core);

    if (alter_scan->pause_or_prevent)
      pause_or_prevent_scan(core);

    if (alter_scan->resume_or_allow)
      resume_or_allow_scan(core);
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

/* supposed to be run on the Master VAP's serializer */
int scan_timeout_async_func(mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  struct scan_support *ss = __wave_core_scan_support_get(core);

  MTLK_ASSERT(core == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle)));

  return scan_timeout_func(core, ss); /* call the function that will get the real work done */
}

#define MGMT_FRAME_HEADER_LEN 24 /* we could also get this from some OS-specific includes, e.g. ieee80211.h */

static __INLINE
void finalize_and_send_probe_req(mtlk_core_t *core, struct scan_support *ss, struct mtlk_scan_request *req, int k)
{
  uint64 cookie;
  u8 *ie_ptr = ss->probe_req + MGMT_FRAME_HEADER_LEN; /* the spot where the IEs start in probe requests */
  u8 *ie_ptr_end = ss->probe_req + ss->probe_req_len;
  size_t len = wave_strlen(req->ssids[k], MAX_SSID_LEN);

  *ie_ptr++ = SSID_IE_TAG;
  *ie_ptr++ = len;

  wave_memcpy(ie_ptr, ie_ptr_end - ie_ptr, req->ssids[k], len);
  ie_ptr += len;

  /* if any bits were set in the band ratemask, use the appropriate prepared supported rates array */
  if (req->rates[ss->last_chan_band])
  {
    size_t size = ss->supp_rates[ss->last_chan_band][1] + 2;
    wave_memcpy(ie_ptr, ie_ptr_end - ie_ptr, ss->supp_rates[ss->last_chan_band], size);
    ie_ptr += size;
  }

  /* always? */
  wave_memcpy(ie_ptr, ie_ptr_end - ie_ptr, ss->ht_capab[ss->last_chan_band], HT_CAPAB_LEN + 2);
  ie_ptr += HT_CAPAB_LEN + 2;

  if (req->ie_len)
  {
    wave_memcpy(ie_ptr, ie_ptr_end - ie_ptr, req->ie, req->ie_len);
    ie_ptr += req->ie_len;
  }

  len = ie_ptr - ss->probe_req;

  ILOG2_SD("Sending a probe request for SSID '%s' on channel %u", req->ssids[k], ss->last_chan_num);
  mtlk_dump(3, ss->probe_req, len, "dump of probe_req:");
  mtlk_mmb_bss_mgmt_tx(core->vap_handle, ss->probe_req, ie_ptr - ss->probe_req, ss->last_chan_num,
                       FALSE, TRUE, FALSE /* unicast */, &cookie, PROCESS_MANAGEMENT, NULL,
                       FALSE, NTS_TID_USE_DEFAULT);
}

int mtlk_send_chan_data (mtlk_vap_handle_t vap_handle, void *data, int data_len)
{
  mtlk_df_t *df               = mtlk_vap_get_df(vap_handle);
  mtlk_df_user_t *df_user     = mtlk_df_get_user(df);
  struct wireless_dev *wdev   = mtlk_df_user_get_wdev(df_user);
  mtlk_nbuf_t *evt_nbuf;
  uint8 *cp;

  MTLK_ASSERT(NULL != wdev);

  evt_nbuf = wv_cfg80211_vendor_event_alloc(wdev, data_len,
                                            LTQ_NL80211_VENDOR_EVENT_CHAN_DATA);
  if (!evt_nbuf)
  {
    return MTLK_ERR_NO_MEM;
  }

  cp = mtlk_df_nbuf_put(evt_nbuf, data_len);
  wave_memcpy(cp, data_len, data, data_len);


  ILOG3_D("CID-%04x: channel data", mtlk_vap_get_oid(vap_handle));
  mtlk_dump(3, evt_nbuf->data, evt_nbuf->len, "channel data vendor event");
  wv_cfg80211_vendor_event(evt_nbuf);

  return MTLK_ERR_OK;
}


/* this runs from the Master VAP's serializer because it waits on a lot of things from the FW */
static int scan_timeout_func(mtlk_core_t *core, mtlk_scan_support_t *ss)
{
  mtlk_hw_t *hw;
  struct mtlk_chan_def *ccd;
  struct mtlk_scan_request *req = &ss->req;
  struct channel_scan_support *css;
  struct channel_survey_support *csys;
  mtlk_df_t *df = mtlk_vap_get_df(core->vap_handle);
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  const char *name = mtlk_df_get_name(df);
  uint32 old_freq1, chan_flags = 0;
  unsigned idx;
  int res = MTLK_ERR_OK, max_num_surveys;
  unsigned num = 0;
  mtlk_hw_band_e band = wave_radio_band_get(radio);
  int numchans;

  int i;

  if (wave_radio_get_is_zwdfs_radio(radio) &&
      !wave_radio_get_zwdfs_ant_enabled(radio)) {
    ILOG0_V("Ignoring channel switches for ZWDFS VAP: Antenna mask not active yet");
    goto end_scan;
  }

  if (band == MTLK_HW_BAND_6_GHZ) {
    css = ss->css_6g;
    csys = ss->csys_6g;
    numchans = NUM_TOTAL_CHANS_6G;
    max_num_surveys = MAX_NUM_SURVEYS_6G;
  } else {
    css = ss->css;
    csys = ss->csys;
    numchans = NUM_TOTAL_CHANS;
    max_num_surveys = MAX_NUM_SURVEYS;
  }

  if (req->n_channels > numchans) {
    return MTLK_ERR_VALUE;
  }

  MTLK_UNREFERENCED_PARAM(name); /* only in printouts */

  MTLK_ASSERT(!in_atomic());
  MTLK_ASSERT(core == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle)));

  ccd = __wave_core_chandef_get(core);

  ss->flags |= SDF_IGNORE_FRAMES;

  if (!is_scan_running(ss))
  {
    ILOG0_S("%s: Scan no longer running", name);
    return MTLK_ERR_OK;
  }

  if (ss->flags & SDF_ABORT)
  {
    res = MTLK_ERR_CANCELED;
    goto end_scan;
  }

  if (ss->flags & SDF_PAUSE_OR_PREVENT)
  {
    /* Need to ensure that we'll repeat scanning from last_chan_idx again, if it had been started */
    if (ss->last_chan_idx >= 0)
      ss->chan_flags |= SDF_PAUSE_OR_PREVENT;
    /* else do nothing, will start the normal way once PAUSE_OR_PREVENT is cleared */

    return MTLK_ERR_OK;
  }

  ILOG3_SDDD("%s: chan_flags=0x%02x, last_chan_idx=%i, last_chan_num=%i",
             name, ss->chan_flags, ss->last_chan_idx, ss->last_chan_num);

  /* sync timer with current time for first run */
  if (ktime_to_ns(ss->scan_next_ktime) == 0){
    ss->scan_next_ktime = ktime_get();
    ss->window_slice_num = ss->chan_in_chunk_time = 0; /* reset window slices */
  }

  if (ss->chan_flags & SDF_PAUSE_OR_PREVENT) /* if we're running the first time after a pause */
  {
    ss->chan_flags &= ~SDF_PAUSE_OR_PREVENT;
    i = ss->last_chan_idx; /* note that PAUSE_OR_PREVENT in chan_flags will not have been set if last_chan_idx is still -1 */
    num = ss->last_chan_num;
    band = ss->last_chan_band;
    chan_flags = ss->last_chan_flags;

    /* We must repeat scanning the channel that was attempted before the pause, so just jump right to doing it */
    goto set_chan;
  }

  /* not the initial invocation and not returning from a BG-break and not in middle of window slices*/
  if (ss->last_chan_idx >= 0 && !(ss->flags & SDF_BG_BREAK) && !(ss->chan_in_chunk_time))
  {
    idx = mtlk_channum2cssidx(ss->last_chan_num, ss->last_chan_band);
    if (idx == CHAN_IDX_ILLEGAL) {
      WLOG_D("Invalid channel index is got for channel %d", ss->last_chan_num);
      goto continue_chan;
    }

    /* mark last scan times for the just scanned channel */
    if (ss->chan_flags & SDF_ACTIVE)
      css[idx].last_active_scan_time = ss->scan_start_time;

    if (ss->chan_flags & SDF_PASSIVE)
      css[idx].last_passive_scan_time = ss->scan_start_time;

    if (ss->num_surveys < max_num_surveys)
    {
      struct intel_vendor_channel_data ch_data;
      mtlk_osal_msec_t cur_time = mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp());
      csys += ss->num_surveys;

      memset(csys, 0, sizeof(*csys));
      csys->channel = req->channels + ss->last_chan_idx;

      memset(&ch_data, 0, sizeof(ch_data));
      ch_data.channel = ss->last_chan_num;
      ch_data.BW = 20;
      ch_data.primary = ss->last_chan_num;
      ch_data.secondary = 0;
      ch_data.freq = channel_to_frequency(ch_data.channel, wave_radio_band_get(radio));
      ch_data.total_time = 255;
      ch_data.calibration = css[idx].calib_done_mask;

      /* mtlk_hw_get_statistics is called directly from scan context of
       * current core  and not from stats timer context of any other core,
       * to avoid both scan(current core) and stats(for any other core)
       * from overwriting the common stats buffer.
       * */

      res = mtlk_hw_get_statistics(mtlk_vap_get_hw(core->vap_handle));
      if (res != MTLK_ERR_OK) {
        ELOG_V("Failed to retrieve Statistics ");
        return res;
      }

      res = scan_get_aocs_info(core, &ch_data, csys);

      /* save_chan_statistics_info is used to save all the scanned channels
       * hw_statistics info, so at later point of time application can
       * fetch the saved results for any channel
       * */
      mtlk_core_save_chan_statistics_info(core, &ch_data);

      core->acs_updated_ts = cur_time;

      ch_data.filled_mask |= CHDATA_SCAN_MODE | CHDATA_CALIB | CHDATA_TOTAL_TIME;

      if (MTLK_ERR_OK == res) res = mtlk_send_chan_data(core->vap_handle, &ch_data, sizeof(ch_data));

      ss->num_surveys++;
    }
    else
      ELOG_D("%i surveys already collected, cannot collect more", ss->num_surveys);
  }

continue_chan:
  /* now move on to the next channel */
  for (i = ss->last_chan_idx + 1; i < (int)req->n_channels; i++)
  {
    band = req->channels[i].band;
    num = freq2lowchannum(req->channels[i].center_freq, CW_20);
    chan_flags = req->channels[i].flags;
    idx = mtlk_channum2cssidx(num, band);
    if (idx == CHAN_IDX_ILLEGAL) {
      WLOG_D("Invalid channel index is got for channel %d", num);
      continue;
    }

    if (mtlk_core_is_band_supported(core, band) != MTLK_ERR_OK)
    {
      ILOG2_SD("%s: Skipping band-not-supported chan %i", name, num);
      continue;
    }

    /* process the reasons to skip this channel */
    if (req->channels[i].flags & MTLK_CHAN_DISABLED)
    {
      ILOG2_SD("%s: Skipping disabled chan %i", name, num);
      continue;
    }

    if (css[idx].calib_failed_mask & (1 << CW_20))
    {
      ILOG2_SD("%s: Skipping calib-failed chan %i", name, num);
      continue;
    }

    ss->chan_flags = ss->flags; /* copy channel flags from the overall scan flags */

    if (MTLK_HW_BAND_6_GHZ == band)
      ss->chan_flags &= ~SDF_ACTIVE;

    if ((ss->chan_flags & SDF_ACTIVE) /* active asked when active not allowed */
        && (req->channels[i].flags & (MTLK_CHAN_NO_IR | MTLK_CHAN_RADAR)))
    {
      struct ieee80211_channel *c = ieee80211_get_channel(req->wiphy, req->channels[i].center_freq);

      if (!c) {
        ELOG_D("Getting channel structure for frequency %d MHz failed",
               req->channels[i].center_freq);
        continue;
      }

      if (c->dfs_state == NL80211_DFS_UNAVAILABLE) {
        ILOG2_SD("%s: Skipping NL80211_DFS_UNAVAILABLE chan %i", name, num);

        continue;
      } else if (c->dfs_state == NL80211_DFS_USABLE) {

        ILOG2_SD("%s: Turning off active scan for no-ir/radar chan %i", name, num);
        ss->chan_flags &= ~SDF_ACTIVE;

        if (!(ss->chan_flags & SDF_PASSIVE)
            && (ss->ScanModifierFlags & SMF_AUTO_PASSIVE_ON_NO_IR))
        {
          ILOG2_SD("%s: Adding passive scan for no-ir chan %i", name, num);
          ss->chan_flags |= SDF_PASSIVE;
        }

      }
    }

    if ((req->type == MTLK_SCAN_AP || req->type == MTLK_SCAN_SCHED_AP) && /* is an AP scan */
        (ss->chan_flags & SDF_PASSIVE) && /* passive scan needed */
        ss->scan_start_time < ss->PassiveValid + css[idx].last_passive_scan_time) /* but still is valid */
    {
      ILOG2_SD("%s: Turning off passive scan on still-valid chan %i", name, num);
      ss->chan_flags &= ~SDF_PASSIVE; /* turn it off */
    }

    if ((req->type == MTLK_SCAN_AP || req->type == MTLK_SCAN_SCHED_AP) && /* is an AP scan */
        (ss->chan_flags & SDF_ACTIVE) && /* active scan needed */
        ss->scan_start_time < ss->ActiveValid + css[idx].last_active_scan_time) /* but still is valid */
    {
      ILOG2_SD("%s: Turning off active scan on still-valid chan %i", name, num);
      ss->chan_flags &= ~SDF_ACTIVE; /* turn it off */
    }

    if (!(ss->chan_flags & (SDF_ACTIVE | SDF_PASSIVE)))
    {
      ILOG2_SD("%s: Skipping no-scans-remaining chan %i", name, num);
      continue;
    }

    break; /* get out and process the channel that has passed all the tests */
  } /* for (i = ss->last_chan_idx + 1; i < req->num_freqs; i++) */

  if (i >= (int)req->n_channels) /* no more channels that need processing in this scan */
    goto end_scan;

set_chan:

  MTLK_ASSERT(i >= 0);

  /* AP background scans need to get some breaks, i.e., need to return back to original channel, or else we might miss too much */
  if (ss->flags & SDF_BACKGROUND)
  {
    if (ss->flags & SDF_BG_BREAK) /* if returning from a background scan break */
    {
      ss->flags &= ~SDF_BG_BREAK;
      ss->chan_in_chunk = 0;
    }

    if (++ss->chan_in_chunk > ss->NumChansInChunk)
    {
      mtlk_country_code_t country_code;
      uint32 beacons_num;     /* Number of beacons from channel scan start time */
      uint64 timeout_val = 0; /* us, target time to switch channel for scan window slice */
      ILOG2_S("%s: taking a break in a BG scan", name);
      ss->flags |= SDF_BG_BREAK;

      _mtlk_update_scan_pause(core->vap_handle, ss);

      ILOG2_H("Scan cb diff time:%lld(ms)", ktime_to_ms(ktime_sub(ktime_get(), ss->scan_next_ktime)));

      /* this skips the HDK config and pays no attention to is_scan_running() */
      idx = mtlk_channum2cssidx(ieee80211_frequency_to_channel(ss->orig_chandef.chan.center_freq), band);
      mtlk_hw_reset_channel_rssi(mtlk_vap_get_hw(core->vap_handle), idx);

      core_cfg_country_code_get(core, &country_code);

      /* set TX power limits */
      core_cfg_set_tx_power_limit(core, ss->orig_chandef.center_freq1, ss->orig_chandef.width,
                                  ss->orig_chandef.chan.center_freq, country_code, MTLK_POWER_NOT_SET);

      if ((res = core_cfg_send_set_chan(core, &ss->orig_chandef, NULL)) != MTLK_ERR_OK)
        goto end_scan;

      if (ss->chan_in_chunk_time) {
        beacons_num = ((ss->chan_in_chunk_time + ss->ChanChunkInterval) / BEACON_FRAME_INTERVAL_US);
        timeout_val = ((beacons_num * BEACON_FRAME_INTERVAL_US) - ss->chan_in_chunk_time) +    /* Next beacon time position*/
                       (ss->window_slice_num * (ss->window_slice - ss->window_slice_overlap));  /* Next window slice position */
        ILOG2_DDDD("chan_in_chunk_time:%d(us) timeout_val:%d(us) beacons_num:%d slice_num:%d",
                    ss->chan_in_chunk_time, timeout_val, beacons_num, ss->window_slice_num);
        ss->chan_in_chunk_time += timeout_val;
      } else
        timeout_val = ss->ChanChunkInterval;

      ss->scan_next_ktime = ktime_add_us(ss->scan_next_ktime, timeout_val);

      ILOG2_DH("update scan timer:%d(us) next kernel_time:%lli(ns)", timeout_val, ktime_to_ns(ss->scan_next_ktime));
      mtlk_osal_hrtimer_set(&ss->scan_timer, ktime_to_ns(ss->scan_next_ktime), HRTIMER_MODE_ABS);
      return MTLK_ERR_OK;
    }
  }

  ILOG2_SDDDD("%s: switching to channel with index %i in the request, number %i, chan_in_chunk %i/%i",
              name, i, num, ss->chan_in_chunk, ss->NumChansInChunk);
  ss->set_chan_req->low_chan_num = num;

  /* Scan always scans 20 MHz channels so 1 or 0 is only valid bitmaps for that */
  ss->set_chan_req->isRadarDetectionNeeded = (chan_flags & MTLK_CHAN_RADAR) ? 1 : 0;
  ss->set_chan_req->isContinuousInterfererDetectionNeeded = is_interf_det_needed(num);

  /* typically either the chan will change or at least the switch type, so setting the channel is now unconditional */
  /* there is a small chance that we issue set_channel-s with equal params consecutively, but it is legal */

  old_freq1 = ccd->center_freq1;
  ccd->center_freq1 = 0; /* this marks that channel is "uncertain" (in this case changing) */

  wmb(); /* so that channel gets marked uncertain well before we start switching it */

  ILOG2_H("Scan cb diff time:%lld(ms)", ktime_to_ms(ktime_sub(ktime_get(), ss->scan_next_ktime)));

  /* set TX power limits */
  core_cfg_set_tx_power_limit(core, req->channels[i].center_freq, CW_20,
                              req->channels[i].center_freq, req->country_code,
                              MTLK_POWER_NOT_SET);

  band = req->channels[i].band;
  num = ieee80211_frequency_to_channel(req->channels[i].center_freq);
  idx = mtlk_channum2cssidx(num, band);
  mtlk_hw_reset_channel_rssi(mtlk_vap_get_hw(core->vap_handle), idx);

  if (!(ss->flags & SDF_BACKGROUND) ||
       (ieee80211_frequency_to_channel(ss->orig_chandef.chan.center_freq) != num)) {
    /* prevent to switching to the same channels in case of BG scan due to CTS-TO-SELF */
    res = core_cfg_send_set_chan_by_msg(&ss->set_chan_man_msg);
    ss->scan_next_ktime = ktime_get();
  } else {
    ILOG4_D("switch to the same channel:%d is skipped", num);
  }
  mtlk_hw_reset_chan_statistics(mtlk_vap_get_hw(core->vap_handle), ieee80211_frequency_to_channel(req->channels[i].center_freq), wave_radio_id_get(radio));

  /* Send CCA threshold on every set channel */
  if (MTLK_ERR_OK == res) {
    mtlk_core_cfg_send_actual_cca_threshold(core);
    (void)mtlk_core_set_coc_pause_power_mode(core);
  }

  if (res != MTLK_ERR_OK)
  {
    ccd->center_freq1 = old_freq1; /* this makes it "certain" again (assuming it was certain to begin with) */
    goto end_scan;
  }

  ccd->chan = req->channels[i]; /* struct assignment */
  ccd->width = CW_20;
  ccd->center_freq2 = 0;
  __wave_core_chan_switch_type_set(core, ST_SCAN);
  /* don't set the current channel in param db; it will get set when the scan ends: either by us
   * restoring the channel we had before the scan, or by hostapd setting the channel to work on
   */

  wmb(); /* so that the last entry we write is the one that marks the channel "certain" again */
  ccd->center_freq1 = req->channels[i].center_freq;
  ss->flags &= ~SDF_IGNORE_FRAMES; /* from this moment on we're ready to report the frames */

  _mtlk_update_scan_pause(core->vap_handle, ss);

  ss->last_chan_num = num;
  ss->last_chan_band = band;
  ss->last_chan_flags = chan_flags;

  {
    uint64 timeout_val = 0;

    if (ss->chan_flags & SDF_ACTIVE)
    {
      int j, k;

      for (j = ss->NumProbeReqs; j > 0; j--)
      {
        for (k = 0; k < req->n_ssids; k++)
          finalize_and_send_probe_req(ss->requester_core, ss, req, k);

        if (j > 1) /* exclude last iteration */
          usleep_range((unsigned long)ss->ProbeReqInterval,((unsigned long)ss->ProbeReqInterval) + 1);
      }

      timeout_val = ss->ActiveScanWaitTime;
    }

    if (ss->chan_flags & SDF_PASSIVE)
    {
      timeout_val = MAX(timeout_val, ss->PassiveScanWaitTime);
    }


    /* CTS-to-SELF max time is 32ms plus we need overhead for messages to fw,
     * when we need to scan more time than we need to do it by window slices
     */
    if ((ss->flags & SDF_BACKGROUND) && (timeout_val > MAX_SCAN_TIME_WO_SLICE_US)) {
      ss->chan_in_chunk_time += ss->window_slice;
      ss->window_slice_num++;
      if (ss->window_slice_num * (ss->window_slice - ss->window_slice_overlap) >= timeout_val) {
        ss->last_chan_idx = i;
        ss->window_slice_num = ss->chan_in_chunk_time = 0;
      }
      timeout_val = ss->window_slice;
    } else
      ss->last_chan_idx = i;


    if (req->type == MTLK_SCAN_STA_ROC || req->type == MTLK_ROC_NORMAL) {
      wave_radio_roc_in_progress_set(radio, 1);
      timeout_val = req->duration * MTLK_OSAL_USEC_IN_MSEC;
      wv_ieee80211_scan_roc_ready_on_chan(wave_vap_ieee80211_hw_get(core->vap_handle));
      ss->last_chan_idx = i;
      ss->window_slice_num = ss->chan_in_chunk_time = 0;
    }

    if (req->type == MTLK_SCAN_CCA) {
      timeout_val = req->duration * MTLK_OSAL_USEC_IN_MSEC;
      ss->last_chan_idx = i;
      ss->window_slice_num = ss->chan_in_chunk_time = 0;
    }

    ss->scan_next_ktime = ktime_add_us(ss->scan_next_ktime, timeout_val);
    ILOG2_DH("update scan timer:%d(us) next kernel_time:%lli(ns)", timeout_val, ktime_to_ns(ss->scan_next_ktime));

    mtlk_osal_hrtimer_set(&ss->scan_timer, ktime_to_ns(ss->scan_next_ktime), HRTIMER_MODE_ABS);
    return MTLK_ERR_OK;
  }

end_scan:
  /* Do not clean up and end scan in case of MAC Fatal pending and fast or full recovery will be started
     Return MTLK_ERR_OK so in case its initial invocation, we'll notify hostapd that scan stared successfully */
  hw = mtlk_vap_get_hw(core->vap_handle);
  if (wave_rcvry_mac_fatal_pending_get(hw) && wave_rcvry_is_configured(hw))
    return MTLK_ERR_OK;

  clean_up_after_this_scan(core, ss);

  ILOG2_PPPP("  ss=%p, master_core=%p, saved_request=%p, wiphy=%p",
      ss, ss->master_core, ss->req.saved_request, ss->req.wiphy);
/*  ILOG2_P("wiphy->bands[0]=%p", ((struct wiphy *) ss->req.wiphy)->bands[0]); */

  if(req->type == MTLK_SCAN_STA_ROC || req->type == MTLK_ROC_NORMAL) {
    wave_radio_roc_in_progress_set(radio, 0);
    wv_ieee80211_scan_roc_completed(wave_vap_ieee80211_hw_get(core->vap_handle));
  }
  else if (req->type == MTLK_SCAN_CCA) {
    wave_radio_cca_msr_done(wave_vap_radio_get(core->vap_handle), core);
  }
  else {
    wv_ieee80211_scan_completed(wave_vap_ieee80211_hw_get(core->vap_handle), res != MTLK_ERR_OK);
    wave_radio_set_scan_done_since_prev_sta_poll(radio, TRUE);
  }

  mtlk_df_resume_stat_timers(mtlk_df_get_user(df));

  mtlk_osal_event_set(&ss->scan_end_event);
  return res;
}

static const IEEE_ADDR BCAST_MAC_ADDR = { {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF} };

int scan_do_scan(mtlk_handle_t hcore, const void* data, uint32 data_size)
 {
  int res = MTLK_ERR_OK;
  unsigned r_size;
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **)data;
  struct mtlk_scan_request *request;

  MTLK_ASSERT(sizeof(mtlk_clpb_t *) == data_size);

  request = mtlk_clpb_enum_get_next(clpb, &r_size);

  MTLK_ASSERT(NULL != request);
  MTLK_ASSERT(sizeof(*request) == r_size);

  core_cfg_country_code_get(core, &request->country_code);
  res = _scan_do_scan(core, request);

  return mtlk_clpb_push(clpb, &res, sizeof(res));
}

#define MAX_BASIC_RATES_LEN 12
#define BASIC_RATE_BIT 0x80
#define MUL10_TO_MUL2_DIV 5

static size_t wv_fill_supported_rates(mtlk_core_t *core, struct mtlk_scan_request *request, mtlk_hw_band_e band, u32 ratemask, u8 *buf, size_t len)
{
  struct wiphy *wiphy = (struct wiphy *) request->wiphy;
  int nlband = mtlkband2nlband(band);
  uint8 basic_rates[MAX_BASIC_RATES_LEN];
  uint32 size;
  unsigned i;

  MTLK_ASSERT(buf && len && wiphy->bands[nlband]);

  size = wave_core_param_db_basic_rates_get(core, band, basic_rates, sizeof(basic_rates));

  if ((int)len > wiphy->bands[nlband]->n_bitrates)
    len = wiphy->bands[nlband]->n_bitrates;

  for (i = 0; i < len; i++)
  {
    unsigned j;

    if (!(ratemask & (0x01 << i))) /* this rate not needed */
      continue;

    buf[i] = wiphy->bands[nlband]->bitrates[i].bitrate / MUL10_TO_MUL2_DIV;

    for (j = 0; j < size; j++)
      if (buf[i] == basic_rates[j]) /* found this rate among the basic rates */
      {
        buf[i] |= BASIC_RATE_BIT;
        break;
      }
  }

  return i;
}

/* supposed to be run on the Master VAP's serializer */
int _scan_do_scan(mtlk_core_t *core, struct mtlk_scan_request *request)
{
  mtlk_hw_t *hw = mtlk_vap_get_hw(core->vap_handle);
  mtlk_df_t *df = mtlk_vap_get_df(core->vap_handle);
  const char *name = mtlk_df_get_name(df);
  mtlk_vap_manager_t *vap_mgr = mtlk_vap_get_manager(core->vap_handle);
  int core_state, radio_mode;
  struct scan_support *ss = __wave_core_scan_support_get(core);
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  int res = MTLK_ERR_OK;
  wv_mac80211_t *mac80211 = wave_radio_mac80211_get(radio);
  BOOL has_peer_connections = mtlk_vap_manager_has_peer_connections(vap_mgr);
  BOOL has_stas_connected = wv_mac80211_has_sta_connections(mac80211);
  BOOL do_background_scan = FALSE;
  int i;

  MTLK_UNREFERENCED_PARAM(name); /* printouts only */

  MTLK_ASSERT(core == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle)));
  MTLK_ASSERT(NULL != request);

  if (mtlk_core_is_stopping(core)) /* not a good time for a new scan */
  {
    ELOG_S("%s: Core is stopping, cannot start the scan", name);
    res = MTLK_ERR_NOT_READY;
    goto no_clean_exit;
  }

  /* if some scan is already going on, cancel this one, except if restarted from fast or full recovery */
  if (is_scan_running(ss) && !wave_rcvry_restart_scan_get(hw, vap_mgr))
  {
    ELOG_S("%s: A scan is already running, cannot start another", name);
    res = MTLK_ERR_BUSY;
    goto no_clean_exit;
  }

  /* if mac scan is already going on, cancel this one */
  if (is_mac_scan_running(ss)) {
    ELOG_S("%s: A mac scan is already running, cannot start another", name);
    res = MTLK_ERR_BUSY;
    goto no_clean_exit;
  }

  /* delay scan, waiting for beacons */
  if (mtlk_core_is_block_tx_mode(mtlk_vap_manager_get_master_core(vap_mgr))) {
    res = MTLK_ERR_NOT_READY;
    goto no_clean_exit;
  }

  /* This will be true although the above checks
   * are false for radar detection, so delay new scans */
  if (mtlk_core_is_in_scan_mode(mtlk_vap_manager_get_master_core(vap_mgr))) {
    res = MTLK_ERR_RETRY;
    goto no_clean_exit;
  }

  if (ss->flags & SDF_PAUSE_OR_PREVENT)
  {
    ELOG_S("%s: A new scan temporarily disallowed", name);
    res = MTLK_ERR_BUSY;
    goto no_clean_exit;
  }

  /* Save the info to what channel (and how wide) we have to return to after the scan */
  ss->orig_chandef = * __wave_core_chandef_get(core);
  ss->req = *request; /* save the request */

  RLOG_SD("%s: Beginning scan, requester_vap_index=%i", name, ss->req.requester_vap_index);

  if (request->type == MTLK_SCAN_STA || request->type == MTLK_SCAN_SCHED_STA || request->type == MTLK_SCAN_CCA)
  {
    if (has_peer_connections)
    {
      ILOG0_V("Scan by sta - do drv bg scan");

      if (!is_channel_certain(&ss->orig_chandef))
      {
        ELOG_S("%s: Should do background scan but channel isn't certain", name);
        res = MTLK_ERR_SCAN_FAILED;
        goto no_clean_exit;
      }

      do_background_scan = TRUE;
    }
  }

  ss->flags |= (SDF_RUNNING | SDF_IGNORE_FRAMES); /* prevent heard frames from being reported yet */

  if (!request->n_channels)
  {
    ELOG_S("%s: This is an empty scan", name);
    res = MTLK_ERR_OK;
    goto error_exit;
  }

  if (ss->flags & SDF_ABORT) /* we'll check this before all lengthy operations */
  {
    ELOG_S("%s: Scan has been canceled", name);
    res = MTLK_ERR_CANCELED;
    goto error_exit;
  }

  /* may not scan unless there has been a Master VAP activated */
  if (core->is_stopped &&
      (res = mtlk_mbss_send_vap_activate(core, request->channels[0].band)) != MTLK_ERR_OK)
  {
    ELOG_SD("%s: Could not activate the master VAP, err=%i", name, res);
    goto error_exit;
  }

  radio_mode = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_MODE_CURRENT);
  ILOG0_D("radio mode %d", radio_mode);
  res = mtlk_core_radio_enable_if_needed(core);
  if (MTLK_ERR_OK != res) {
    goto error_exit;
  }

  /* get calibration status from radio module */
  {
    mtlk_hw_band_e band = wave_radio_band_get(radio);
    int num_chans = ((MTLK_HW_BAND_6_GHZ == band) ? NUM_TOTAL_CHANS_6G : NUM_TOTAL_CHANS);
    struct channel_scan_support *loc_css = ((MTLK_HW_BAND_6_GHZ == band) ? ss->css : ss->css_6g);
    for (i = 0; i <= num_chans; i++) {
        wave_radio_calibration_status_get(radio, i, &loc_css[i].calib_done_mask, &loc_css[i].calib_failed_mask);
    }
  }
  ILOG1_PPPPD("  ss=%p, master_core=%p, saved_request=%p, wiphy=%p, type=%i",
                     ss, ss->master_core, ss->req.saved_request, ss->req.wiphy, ss->req.type);

  core_state = mtlk_core_get_net_state(core);
  ss->ScanModifierFlags = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_SCAN_MODIFS);

  /* Decide whether to do background or foreground for AP scan request */
  if (request->type == MTLK_SCAN_AP || request->type == MTLK_SCAN_SCHED_AP
    || request->type == MTLK_SCAN_STA_ROC  || request->type == MTLK_ROC_NORMAL || request->type == MTLK_SCAN_CCA) {
    if (is_channel_certain(&ss->orig_chandef)) /* mandatory condition for a background scan---initial set-channel has been done */
    {
      do_background_scan = ((request->flags & MTLK_SCAN_FLAG_LOW_PRIORITY) /* if it's a background scan for real */
                  || (mtlk_vap_manager_did_ap_started(vap_mgr)
                      && (ss->ScanModifierFlags & SMF_AUTO_ENABLE_BG))
                  || has_stas_connected || has_peer_connections);
    }

    ILOG0_SDD("AP %s scan - orig_certain=%i, has_stas_connected=%i",
              do_background_scan ? "background" : "foreground",
              is_channel_certain(&ss->orig_chandef), (int)has_stas_connected);
  }

  if (do_background_scan && !core->vap_in_fw_is_active && !has_stas_connected)
  {
    ELOG_S("%s: VAP activation process is ongoing, cannot start BG-scan", name);
    ss->flags &= ~(SDF_RUNNING | SDF_IGNORE_FRAMES);
    res = MTLK_ERR_NOT_READY;
    goto no_clean_exit;
  }

  ILOG1_SDDDDSDDDDD("%s: Starting scan preparation, modifier_flags=0x%02x, flags=0x%02x, "
      "active_vaps=%i, has_connections=%i, core_state=%s, orig_freq1=%u, request_flags=0x%02x, "
      "low_priority_flag=%i, rates2.4=%i, rates5=%i", name, ss->ScanModifierFlags, ss->flags,
      mtlk_vap_manager_get_active_vaps_number(vap_mgr), has_peer_connections, mtlk_net_state_to_string(core_state),
      ss->orig_chandef.center_freq1, request->flags, (request->flags & MTLK_SCAN_FLAG_LOW_PRIORITY),
      request->rates[UMI_PHY_TYPE_BAND_2_4_GHZ], request->rates[UMI_PHY_TYPE_BAND_5_2_GHZ]);

  if (do_background_scan)
  {
    struct cfg80211_chan_def orig_chandef = {0};
    struct mtlk_chan_def* orig_mtlk_chandef = &(ss->orig_chandef);

    mtlk_pdb_size_t size = sizeof(ss->iwpriv_scan_params_bg);
    WAVE_RADIO_PDB_GET_BINARY(radio, PARAM_DB_RADIO_SCAN_PARAMS_BG,
                        &ss->iwpriv_scan_params_bg, &size);
    ss->flags |= SDF_BACKGROUND;
    ss->PassiveScanWaitTime = ss->iwpriv_scan_params_bg.passive_scan_time * MTLK_OSAL_USEC_IN_MSEC;
    ss->ActiveScanWaitTime = ss->iwpriv_scan_params_bg.active_scan_time * MTLK_OSAL_USEC_IN_MSEC;
    ss->NumProbeReqs = ss->iwpriv_scan_params_bg.num_probe_reqs;
    ss->ProbeReqInterval = ss->iwpriv_scan_params_bg.probe_req_interval * MTLK_OSAL_USEC_IN_MSEC;
    ss->NumChansInChunk = ss->iwpriv_scan_params_bg.num_chans_in_chunk;
    ss->window_slice = ss->iwpriv_scan_params_bg.window_slice * MTLK_OSAL_USEC_IN_MSEC;
    ss->window_slice_overlap = ss->iwpriv_scan_params_bg.window_slice_overlap * MTLK_OSAL_USEC_IN_MSEC;
    MTLK_ASSERT(ss->NumChansInChunk != 0);

    /* In order to support %83 radar detection probability use the following:
     * Minimum bg scan break duration without traffic: 1 second
     * Minimum bg scan break duration with traffic for 5Ghz band: 5 seconds
     * Whether stations are connected or not will be checked once before bg
     * scan is started, not during the scan. */
    orig_chandef.chan = ieee80211_get_channel(ss->req.wiphy, orig_mtlk_chandef->chan.center_freq);
    orig_chandef.width = mtlkcw2nlcw(orig_mtlk_chandef->width, orig_mtlk_chandef->is_noht);
    orig_chandef.center_freq1 = orig_mtlk_chandef->center_freq1;
    orig_chandef.center_freq2 = orig_mtlk_chandef->center_freq2;

    if ((mtlk_core_is_band_supported(core, UMI_PHY_TYPE_BAND_5_2_GHZ) == MTLK_ERR_OK)
        && wv_cfg80211_chandef_dfs_required(ss->req.wiphy, &orig_chandef)
        && has_peer_connections)
      ss->ChanChunkInterval = ss->iwpriv_scan_params_bg.chan_chunk_interval_busy * MTLK_OSAL_USEC_IN_MSEC;
    else
      ss->ChanChunkInterval = ss->iwpriv_scan_params_bg.chan_chunk_interval * MTLK_OSAL_USEC_IN_MSEC;

  }
  else /* not a background scan */
  {
    mtlk_pdb_size_t size = sizeof(ss->iwpriv_scan_params);
    WAVE_RADIO_PDB_GET_BINARY(radio, PARAM_DB_RADIO_SCAN_PARAMS,
                        &ss->iwpriv_scan_params, &size);

    ss->PassiveValid = msecs_to_jiffies(1000 * ss->iwpriv_scan_params.passive_scan_valid_time);
    ss->ActiveValid = msecs_to_jiffies(1000 * ss->iwpriv_scan_params.active_scan_valid_time);
    ss->PassiveScanWaitTime = ss->iwpriv_scan_params.passive_scan_time * MTLK_OSAL_USEC_IN_MSEC;
    ss->ActiveScanWaitTime = ss->iwpriv_scan_params.active_scan_time * MTLK_OSAL_USEC_IN_MSEC;
    ss->NumProbeReqs = ss->iwpriv_scan_params.num_probe_reqs;
    ss->ProbeReqInterval = ss->iwpriv_scan_params.probe_req_interval * MTLK_OSAL_USEC_IN_MSEC;
  }

  if (ss->flags & SDF_ABORT) /* check this before we set the flags marking scan as running */
  {
    ELOG_S("%s: Scan has been canceled", name);
    res = MTLK_ERR_CANCELED;
    goto error_exit;
  }

  /* apply scan type modifications */
  if ((request->n_ssids <= 0) || (MTLK_HW_BAND_6_GHZ == wave_radio_band_get(radio)))/* passive */
  {
    ss->probe_req = NULL; /* it should already be NULL, BTW */
    ss->probe_req_len = 0;
    ss->flags |= SDF_PASSIVE;

    if (ss->ScanModifierFlags & SMF_ADD_ACTIVE)
      ss->flags |= SDF_ACTIVE;

    if (ss->ScanModifierFlags & SMF_REMOVE_PASSIVE)
      ss->flags &= ~SDF_PASSIVE;
  }
  else /* active */
  {
    ss->flags |= SDF_ACTIVE;

    if (ss->ScanModifierFlags & SMF_ADD_PASSIVE)
      ss->flags |= SDF_PASSIVE;

    if (ss->ScanModifierFlags & SMF_REMOVE_ACTIVE)
      ss->flags &= ~SDF_ACTIVE;
  }

  /* prepare the set_chan msg as much as possible */
  {
    mtlk_txmm_data_t *man_entry;

    /* prepare msg for the FW */
    if (!(man_entry = mtlk_txmm_msg_init_with_empty_data(&ss->set_chan_man_msg,
                                                         mtlk_vap_get_txmm(core->vap_handle), &res)))
    {
      ELOG_DD("CID-%04x: UM_MAN_SET_CHAN_REQ init for scan failed, err=%i",
              mtlk_vap_get_oid(core->vap_handle), res);
      res = MTLK_ERR_NO_RESOURCES;
      goto nomem_exit;
    }

    man_entry->id = UM_SET_CHAN_REQ;
    man_entry->payload_size = sizeof(*ss->set_chan_req);
    ss->set_chan_req = (UM_SET_CHAN_PARAMS *) man_entry->payload;
    memset(ss->set_chan_req, 0, sizeof(*ss->set_chan_req));
    ss->set_chan_req->RegulationType = REGD_CODE_UNKNOWN;
    ss->set_chan_req->chan_width = CW_20;
    if (request->type == MTLK_SCAN_STA_ROC)
    {
      ss->set_chan_req->switch_type = ST_DPP_ACTION;
      ss->set_chan_req->cts_to_self_duration = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_CTS_TO_SELF_TO);
    } else {
      ss->set_chan_req->switch_type = ST_SCAN;
      if (do_background_scan)
        ss->set_chan_req->cts_to_self_duration = ss->iwpriv_scan_params_bg.cts_to_self_duration;
    }
    ss->set_chan_req->bg_scan = ss->flags & SDF_BACKGROUND;
  }

  /* init the probe req frame if in the end the scan has become active */
  if (ss->flags & SDF_ACTIVE)
  {
    uint8 sa_addr[ETH_ALEN];
    mtlk_vap_handle_t requester_vap_handle;
    size_t probe_req_max_len = sizeof(struct ieee80211_mgmt)
      + MAX_SSID_LEN + 2 + MAX_SUPPORTED_RATES + 2 + HT_CAPAB_LEN + 2
      + request->ie_len;
    struct ieee80211_mgmt *header = NULL;

    res = mtlk_vap_manager_get_vap_handle(vap_mgr, request->requester_vap_index, &requester_vap_handle);
    if (MTLK_ERR_OK != res) {
      ELOG_S("%s: failed to get requester vap handle", name);
      res = MTLK_ERR_UNKNOWN;
      goto error_exit;
    }

    header = mtlk_osal_mem_alloc(probe_req_max_len, MTLK_MEM_TAG_SCAN_DATA);
    if (!header)
      goto nomem_exit;

    ss->requester_core = mtlk_vap_get_core(requester_vap_handle);
    wave_pdb_get_mac(mtlk_vap_get_param_db(requester_vap_handle), PARAM_DB_CORE_MAC_ADDR, sa_addr);

    ss->probe_req = (uint8 *) header;
    ss->probe_req_len = probe_req_max_len;
    memset(ss->probe_req, 0, probe_req_max_len);

    header->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |IEEE80211_STYPE_PROBE_REQ);
    /* Leave duration and seq_control alone---I don't know what should go in there */
    mtlk_osal_copy_eth_addresses(header->sa, sa_addr);
    mtlk_osal_copy_eth_addresses(header->da, BCAST_MAC_ADDR.au8Addr);
    mtlk_osal_copy_eth_addresses(header->bssid, BCAST_MAC_ADDR.au8Addr);

    if (mtlk_core_is_band_supported(core, UMI_PHY_TYPE_BAND_2_4_GHZ) == MTLK_ERR_OK)
    {
      /* What if hostapd was configured to not work in HT, do we still want this element? how do we know? */
      ss->ht_capab[UMI_PHY_TYPE_BAND_2_4_GHZ][0] = HT_CAPABILITIES_IE_TAG;
      ss->ht_capab[UMI_PHY_TYPE_BAND_2_4_GHZ][1] = HT_CAPAB_LEN;
      wv_fill_ht_capab(request, UMI_PHY_TYPE_BAND_2_4_GHZ,
                       ss->ht_capab[UMI_PHY_TYPE_BAND_2_4_GHZ] + 2,
                       sizeof(ss->ht_capab[UMI_PHY_TYPE_BAND_2_4_GHZ]) - 2);

      /* In theory, if there are a lot of bits in the rates, we may also need the extended supported rates */
      /* In practice there are just 8 for 2.4 GHz and nothing for 5 GHz. No relation to what's actually in hostapd.conf */
      ss->supp_rates[UMI_PHY_TYPE_BAND_2_4_GHZ][0] = SUPPORTED_RATES_IE_TAG;
      ss->supp_rates[UMI_PHY_TYPE_BAND_2_4_GHZ][1] = wv_fill_supported_rates(core, request, UMI_PHY_TYPE_BAND_2_4_GHZ,
                                                                             request->rates[UMI_PHY_TYPE_BAND_2_4_GHZ],
                                                                             ss->supp_rates[UMI_PHY_TYPE_BAND_2_4_GHZ] + 2,
                                                                             sizeof(ss->supp_rates[UMI_PHY_TYPE_BAND_2_4_GHZ]) - 2);
    }

    if (mtlk_core_is_band_supported(core, UMI_PHY_TYPE_BAND_5_2_GHZ) == MTLK_ERR_OK)
    {
      ss->ht_capab[UMI_PHY_TYPE_BAND_5_2_GHZ][0] = HT_CAPABILITIES_IE_TAG;
      ss->ht_capab[UMI_PHY_TYPE_BAND_5_2_GHZ][1] = HT_CAPAB_LEN;
      wv_fill_ht_capab(request, UMI_PHY_TYPE_BAND_5_2_GHZ,
                       ss->ht_capab[UMI_PHY_TYPE_BAND_5_2_GHZ] + 2,
                       sizeof(ss->ht_capab[UMI_PHY_TYPE_BAND_5_2_GHZ]) - 2);

      ss->supp_rates[UMI_PHY_TYPE_BAND_5_2_GHZ][0] = SUPPORTED_RATES_IE_TAG;
      ss->supp_rates[UMI_PHY_TYPE_BAND_5_2_GHZ][1] = wv_fill_supported_rates(core, request, UMI_PHY_TYPE_BAND_5_2_GHZ,
                                                                             request->rates[UMI_PHY_TYPE_BAND_5_2_GHZ],
                                                                             ss->supp_rates[UMI_PHY_TYPE_BAND_5_2_GHZ] + 2,
                                                                             sizeof(ss->supp_rates[UMI_PHY_TYPE_BAND_5_2_GHZ]) - 2);
    }
    ILOG0_SY("%s: scan preparation - active scan, sa=%Y", name, header->sa);
  } else {
    ILOG0_S("%s: scan preparation - passive scan", name);
  }

  ss->last_chan_idx = -1; /* make sure timeout_func knows it's the very start of the scan */
  ss->num_surveys = 0;

  ss->param_algomask = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_CALIBRATION_ALGO_MASK);
  ss->param_oalgomask = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_ONLINE_ACM);

  ss->scan_start_time = get_jiffies_64(); /* 32-bit jiffies wrap and that can mess us up */
  ss->scan_next_ktime = ktime_set(0,0); /* reset timer for synchronization */
  ss->chan_in_chunk_time = 0;
  ss->window_slice_num = 0;

  if (ss->flags & SDF_ABORT) /* one last check because it's good to avoid an unneeded scan */
  {
    ELOG_S("%s: Scan has been canceled", name);
    res = MTLK_ERR_CANCELED;
    goto error_exit;
  }

  mtlk_osal_event_reset(&ss->scan_end_event);

  ILOG1_SDDD("%s: Scan preparation complete: channels=%u, modifier_flags=0x%02x, flags=0x%02x",
             name, request->n_channels, ss->ScanModifierFlags, ss->flags);

  mtlk_df_stop_stat_timers(mtlk_df_get_user(df));

  /* Now just invoke the regular scan timeout handling func, which will get the real work done */
  return scan_timeout_func(core, ss);

nomem_exit:
  res = MTLK_ERR_NO_MEM;

error_exit:
  if (!wave_rcvry_mac_fatal_pending_get(hw) || !wave_rcvry_is_configured(hw))
    clean_up_after_this_scan(core, ss);

no_clean_exit:
  if (!wave_rcvry_mac_fatal_pending_get(hw) || !wave_rcvry_is_configured(hw)) {
    if (res != MTLK_ERR_OK && res != MTLK_ERR_NOT_READY)
      ELOG_SD("%s: Scan failed: res=%i", name, res);
  } else {
    wave_rcvry_restart_scan_set(hw, vap_mgr, TRUE);
    ILOG1_V("Scan will be started again during fast or full recovery");
    res = MTLK_ERR_OK; /* Tell hostapd that scan has successfully started. Fast recovery will continue the scan */
  }
  mtlk_osal_event_set(&ss->scan_end_event);
  return res;
}

int scan_dump_survey(mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  struct scan_support *ss = __wave_core_scan_support_get(core);
  struct mtlk_chan_def *chandef_in_use = is_scan_running(ss) ? &ss->orig_chandef : __wave_core_chandef_get(core);
  int *idx;
  unsigned idx_size;
  struct mtlk_channel_status cs;
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(core == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle)));
  MTLK_ASSERT(sizeof(mtlk_clpb_t *) == data_size);

  ILOG3_V("Running");

  idx = mtlk_clpb_enum_get_next(clpb, &idx_size);

  MTLK_ASSERT(NULL != idx);
  if (!mtlk_clpb_check_data(idx, idx_size, sizeof(*idx), __FUNCTION__, __LINE__ )) {
    return MTLK_ERR_UNKNOWN;
  }

  memset(&cs, 0, sizeof(cs));

  if (*idx >= ss->num_surveys)
    cs.channel = NULL;
  else
  {
    cs.channel = ss->csys[*idx].channel;
    cs.current_primary_chan_freq = chandef_in_use->chan.center_freq;
    cs.load = ss->csys[*idx].load;
    cs.noise = ss->csys[*idx].noise;
  }

  res = mtlk_clpb_push(clpb, &cs, sizeof(cs));

  return res;
}

int mtlk_scan_recovery_get_channel(mtlk_core_t *core,
        struct mtlk_chan_def *chandef)
{
  struct scan_support *ss = __wave_core_scan_support_get(core);
  struct mtlk_chan_def *chandef_in_use = is_scan_running(ss) ? &ss->orig_chandef : __wave_core_chandef_get(core);

  if (chandef == NULL) {
    return MTLK_ERR_PARAMS;
  }
  *chandef = *chandef_in_use;
  return MTLK_ERR_OK;
}

int mtlk_scan_recovery_continue_scan (mtlk_core_t *core, BOOL is_bg_scan)
{
  int res;
  mtlk_txmm_data_t *man_entry;
  struct scan_support *ss = __wave_core_scan_support_get(core);
  mtlk_hw_t *hw = mtlk_vap_get_hw(core->vap_handle);
  mtlk_vap_manager_t *vap_manager = mtlk_vap_get_manager(core->vap_handle);

  ILOG0_D("CID-%04x: Resuming background scan", mtlk_vap_get_oid(core->vap_handle));

  if (wave_rcvry_restart_scan_get(hw, vap_manager) && !is_bg_scan) {
    if (!(ss->flags & SDF_PAUSE_OR_PREVENT)) {
      ELOG_V("The scan is not paused/prevented");
      return MTLK_ERR_BUSY;
    }

    ss->flags &= ~SDF_PAUSE_OR_PREVENT;

    if (!is_scan_running(ss)) {
      ILOG0_V("No scan is running");
      return MTLK_ERR_OK;
    }

    res = _scan_do_scan(core, &ss->req);
    wave_rcvry_restart_scan_set(hw, vap_manager, FALSE);
    return res;
  }

  /* prepare msg for the FW which was cleaned up due to FW crash */
  if (!(man_entry = mtlk_txmm_msg_init_with_empty_data(&ss->set_chan_man_msg,
      mtlk_vap_get_txmm(core->vap_handle), &res))) {
    ELOG_DD("CID-%04x: UM_MAN_SET_CHAN_REQ init for scan failed, err=%i",
    mtlk_vap_get_oid(core->vap_handle), res);
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_SET_CHAN_REQ;
  man_entry->payload_size = sizeof(*ss->set_chan_req);
  ss->set_chan_req = (UM_SET_CHAN_PARAMS *)man_entry->payload;
  memset(ss->set_chan_req, 0, sizeof(*ss->set_chan_req));
  ss->set_chan_req->RegulationType = REGD_CODE_UNKNOWN;
  ss->set_chan_req->chan_width = CW_20;
  ss->set_chan_req->switch_type = ST_SCAN;

  if (is_bg_scan)
    ss->set_chan_req->bg_scan |= SDF_BACKGROUND;

  return resume_or_allow_scan(core);
}
