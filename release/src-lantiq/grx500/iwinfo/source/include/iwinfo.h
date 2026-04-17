#ifndef __IWINFO_H_
#define __IWINFO_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <glob.h>
#include <ctype.h>
#include <dirent.h>
#include <stdint.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <net/if.h>
#include <errno.h>


#define IWINFO_BUFSIZE	24 * 1024
#define IWINFO_ESSID_MAX_SIZE	32

enum iwinfo_80211 {
	IWINFO_80211_A = 0,
	IWINFO_80211_B,
	IWINFO_80211_G,
	IWINFO_80211_N,
	IWINFO_80211_AC,
	IWINFO_80211_AD,
	IWINFO_80211_AX,
	IWINFO_80211_BE,

	/* keep last */
	IWINFO_80211_COUNT
};

#define IWINFO_80211_A       (1 << IWINFO_80211_A)
#define IWINFO_80211_B       (1 << IWINFO_80211_B)
#define IWINFO_80211_G       (1 << IWINFO_80211_G)
#define IWINFO_80211_N       (1 << IWINFO_80211_N)
#define IWINFO_80211_AC      (1 << IWINFO_80211_AC)
#define IWINFO_80211_AD      (1 << IWINFO_80211_AD)
#define IWINFO_80211_AX      (1 << IWINFO_80211_AX)
#define IWINFO_80211_BE      (1 << IWINFO_80211_BE)

extern const char * const IWINFO_80211_NAMES[IWINFO_80211_COUNT];


enum iwinfo_band {
	IWINFO_BAND_24 = 0,
	IWINFO_BAND_5,
	IWINFO_BAND_6,
	IWINFO_BAND_60,

	/* keep last */
	IWINFO_BAND_COUNT
};

#define IWINFO_BAND_24       (1 << IWINFO_BAND_24)
#define IWINFO_BAND_5        (1 << IWINFO_BAND_5)
#define IWINFO_BAND_6        (1 << IWINFO_BAND_6)
#define IWINFO_BAND_60       (1 << IWINFO_BAND_60)

extern const char * const IWINFO_BAND_NAMES[IWINFO_BAND_COUNT];


enum iwinfo_cipher {
	IWINFO_CIPHER_NONE = 0,
	IWINFO_CIPHER_WEP40,
	IWINFO_CIPHER_TKIP,
	IWINFO_CIPHER_WRAP,
	IWINFO_CIPHER_CCMP,
	IWINFO_CIPHER_WEP104,
	IWINFO_CIPHER_AESOCB,
	IWINFO_CIPHER_CKIP,
	IWINFO_CIPHER_GCMP,
	IWINFO_CIPHER_CCMP256,
	IWINFO_CIPHER_GCMP256,

	/* keep last */
	IWINFO_CIPHER_COUNT
};

#define IWINFO_CIPHER_NONE    (1 << IWINFO_CIPHER_NONE)
#define IWINFO_CIPHER_WEP40   (1 << IWINFO_CIPHER_WEP40)
#define IWINFO_CIPHER_TKIP    (1 << IWINFO_CIPHER_TKIP)
#define IWINFO_CIPHER_WRAP    (1 << IWINFO_CIPHER_WRAP)
#define IWINFO_CIPHER_CCMP    (1 << IWINFO_CIPHER_CCMP)
#define IWINFO_CIPHER_WEP104  (1 << IWINFO_CIPHER_WEP104)
#define IWINFO_CIPHER_AESOCB  (1 << IWINFO_CIPHER_AESOCB)
#define IWINFO_CIPHER_CKIP    (1 << IWINFO_CIPHER_CKIP)
#define IWINFO_CIPHER_GCMP    (1 << IWINFO_CIPHER_GCMP)
#define IWINFO_CIPHER_CCMP256 (1 << IWINFO_CIPHER_CCMP256)
#define IWINFO_CIPHER_GCMP256 (1 << IWINFO_CIPHER_GCMP256)

extern const char * const IWINFO_CIPHER_NAMES[IWINFO_CIPHER_COUNT];


enum iwinfo_kmgmt {
	IWINFO_KMGMT_NONE = 0,
	IWINFO_KMGMT_8021x,
	IWINFO_KMGMT_PSK,
	IWINFO_KMGMT_SAE,
	IWINFO_KMGMT_OWE,

	/* keep last */
	IWINFO_KMGMT_COUNT
};

#define IWINFO_KMGMT_NONE    (1 << IWINFO_KMGMT_NONE)
#define IWINFO_KMGMT_8021x   (1 << IWINFO_KMGMT_8021x)
#define IWINFO_KMGMT_PSK     (1 << IWINFO_KMGMT_PSK)
#define IWINFO_KMGMT_SAE     (1 << IWINFO_KMGMT_SAE)
#define IWINFO_KMGMT_OWE     (1 << IWINFO_KMGMT_OWE)

extern const char * const IWINFO_KMGMT_NAMES[IWINFO_KMGMT_COUNT];


enum iwinfo_auth {
	IWINFO_AUTH_OPEN = 0,
	IWINFO_AUTH_SHARED,

	/* keep last */
	IWINFO_AUTH_COUNT
};

#define IWINFO_AUTH_OPEN     (1 << IWINFO_AUTH_OPEN)
#define IWINFO_AUTH_SHARED   (1 << IWINFO_AUTH_SHARED)

extern const char * const IWINFO_AUTH_NAMES[IWINFO_AUTH_COUNT];


enum iwinfo_freq_flag {
	IWINFO_FREQ_NO_10MHZ = 0,
	IWINFO_FREQ_NO_20MHZ,
	IWINFO_FREQ_NO_HT40PLUS,
	IWINFO_FREQ_NO_HT40MINUS,
	IWINFO_FREQ_NO_80MHZ,
	IWINFO_FREQ_NO_160MHZ,
	IWINFO_FREQ_NO_HE,
	IWINFO_FREQ_NO_IR,
	IWINFO_FREQ_INDOOR_ONLY,

	/* keep last */
	IWINFO_FREQ_FLAG_COUNT,
};

#define IWINFO_FREQ_NO_10MHZ       (1 << IWINFO_FREQ_NO_10MHZ)
#define IWINFO_FREQ_NO_20MHZ       (1 << IWINFO_FREQ_NO_20MHZ)
#define IWINFO_FREQ_NO_HT40PLUS    (1 << IWINFO_FREQ_NO_HT40PLUS)
#define IWINFO_FREQ_NO_HT40MINUS   (1 << IWINFO_FREQ_NO_HT40MINUS)
#define IWINFO_FREQ_NO_80MHZ       (1 << IWINFO_FREQ_NO_80MHZ)
#define IWINFO_FREQ_NO_160MHZ      (1 << IWINFO_FREQ_NO_160MHZ)
#define IWINFO_FREQ_NO_HE          (1 << IWINFO_FREQ_NO_HE)
#define IWINFO_FREQ_NO_IR          (1 << IWINFO_FREQ_NO_IR)
#define IWINFO_FREQ_INDOOR_ONLY    (1 << IWINFO_FREQ_INDOOR_ONLY)

extern const char * const IWINFO_FREQ_FLAG_NAMES[IWINFO_FREQ_FLAG_COUNT];


enum iwinfo_opmode {
	IWINFO_OPMODE_UNKNOWN = 0,
	IWINFO_OPMODE_MASTER,
	IWINFO_OPMODE_ADHOC,
	IWINFO_OPMODE_CLIENT,
	IWINFO_OPMODE_MONITOR,
	IWINFO_OPMODE_AP_VLAN,
	IWINFO_OPMODE_WDS,
	IWINFO_OPMODE_MESHPOINT,
	IWINFO_OPMODE_P2P_CLIENT,
	IWINFO_OPMODE_P2P_GO,

	/* keep last */
	IWINFO_OPMODE_COUNT
};

extern const char * const IWINFO_OPMODE_NAMES[IWINFO_OPMODE_COUNT];


enum iwinfo_htmode {
	IWINFO_HTMODE_HT20 = 0,
	IWINFO_HTMODE_HT40,
	IWINFO_HTMODE_VHT20,
	IWINFO_HTMODE_VHT40,
	IWINFO_HTMODE_VHT80,
	IWINFO_HTMODE_VHT80_80,
	IWINFO_HTMODE_VHT160,
	IWINFO_HTMODE_NOHT,
	IWINFO_HTMODE_HE20,
	IWINFO_HTMODE_HE40,
	IWINFO_HTMODE_HE80,
	IWINFO_HTMODE_HE80_80,
	IWINFO_HTMODE_HE160,
	IWINFO_HTMODE_EHT20,
	IWINFO_HTMODE_EHT40,
	IWINFO_HTMODE_EHT80,
	IWINFO_HTMODE_EHT80_80,
	IWINFO_HTMODE_EHT160,
	IWINFO_HTMODE_EHT320,

	/* keep last */
	IWINFO_HTMODE_COUNT
};

#define IWINFO_HTMODE_HT20       (1 << IWINFO_HTMODE_HT20)
#define IWINFO_HTMODE_HT40       (1 << IWINFO_HTMODE_HT40)
#define IWINFO_HTMODE_VHT20      (1 << IWINFO_HTMODE_VHT20)
#define IWINFO_HTMODE_VHT40      (1 << IWINFO_HTMODE_VHT40)
#define IWINFO_HTMODE_VHT80      (1 << IWINFO_HTMODE_VHT80)
#define IWINFO_HTMODE_VHT80_80   (1 << IWINFO_HTMODE_VHT80_80)
#define IWINFO_HTMODE_VHT160     (1 << IWINFO_HTMODE_VHT160)
#define IWINFO_HTMODE_NOHT       (1 << IWINFO_HTMODE_NOHT)
#define IWINFO_HTMODE_HE20       (1 << IWINFO_HTMODE_HE20)
#define IWINFO_HTMODE_HE40       (1 << IWINFO_HTMODE_HE40)
#define IWINFO_HTMODE_HE80       (1 << IWINFO_HTMODE_HE80)
#define IWINFO_HTMODE_HE80_80    (1 << IWINFO_HTMODE_HE80_80)
#define IWINFO_HTMODE_HE160      (1 << IWINFO_HTMODE_HE160)
#define IWINFO_HTMODE_EHT20      (1 << IWINFO_HTMODE_EHT20)
#define IWINFO_HTMODE_EHT40      (1 << IWINFO_HTMODE_EHT40)
#define IWINFO_HTMODE_EHT80      (1 << IWINFO_HTMODE_EHT80)
#define IWINFO_HTMODE_EHT80_80   (1 << IWINFO_HTMODE_EHT80_80)
#define IWINFO_HTMODE_EHT160     (1 << IWINFO_HTMODE_EHT160)
#define IWINFO_HTMODE_EHT320     (1 << IWINFO_HTMODE_EHT320)

extern const char * const IWINFO_HTMODE_NAMES[IWINFO_HTMODE_COUNT];


struct iwinfo_rate_entry {
	uint32_t rate;
	int8_t mcs;
	uint8_t is_40mhz:1;
	uint8_t is_short_gi:1;
	uint8_t is_ht:1;
	uint8_t is_vht:1;
	uint8_t is_he:1;
	uint8_t is_eht:1;
	uint8_t he_gi;
	uint8_t he_dcm;
	uint8_t mhz;
	uint8_t nss;
	uint8_t mhz_hi;
	uint8_t eht_gi;
};

struct iwinfo_assoclist_entry {
	uint8_t	mac[6];
	int8_t signal;
	int8_t signal_avg;
	int8_t noise;
	uint32_t inactive;
	uint32_t connected_time;
	uint32_t rx_packets;
	uint32_t tx_packets;
	uint64_t rx_drop_misc;
	struct iwinfo_rate_entry rx_rate;
	struct iwinfo_rate_entry tx_rate;
	uint64_t rx_bytes;
	uint64_t tx_bytes;
	uint32_t tx_retries;
	uint32_t tx_failed;
	uint64_t t_offset;
	uint8_t is_authorized:1;
	uint8_t is_authenticated:1;
	uint8_t is_preamble_short:1;
	uint8_t is_wme:1;
	uint8_t is_mfp:1;
	uint8_t is_tdls:1;
	uint32_t thr;
	uint16_t llid;
	uint16_t plid;
	char plink_state[16];
	char local_ps[16];
	char peer_ps[16];
	char nonpeer_ps[16];
};

struct iwinfo_survey_entry {
	uint64_t active_time;
	uint64_t busy_time;
	uint64_t busy_time_ext;
	uint64_t rxtime;
	uint64_t txtime;
	uint32_t mhz;
	uint8_t noise;
};

struct iwinfo_txpwrlist_entry {
	uint8_t  dbm;
	uint16_t mw;
};

struct iwinfo_freqlist_entry {
	uint8_t band;
	uint8_t channel;
	uint32_t mhz;
	uint8_t restricted;
	uint32_t flags;
};

struct iwinfo_crypto_entry {
	uint8_t	enabled;
	uint8_t wpa_version;
	uint16_t group_ciphers;
	uint16_t pair_ciphers;
	uint8_t auth_suites;
	uint8_t auth_algs;
};

struct iwinfo_scanlist_ht_chan_entry {
	uint8_t primary_chan;
	uint8_t secondary_chan_off;
	uint8_t chan_width;
};

struct iwinfo_scanlist_vht_chan_entry {
	uint8_t chan_width;
	uint8_t center_chan_1;
	uint8_t center_chan_2;
};

extern const char * const ht_secondary_offset[4];
/* 0 = 20 MHz
   1 = 40 MHz or higher (refer to vht if supported) */
extern const uint16_t ht_chan_width[2];
/* 0 = 40 MHz or lower (refer to ht to a more precise width)
   1 = 80 MHz
   2 = 160 MHz
   3 = 80+80 MHz */
extern const uint16_t vht_chan_width[4];
/* 0 = 20 MHz
   1 = 40 MHz
   2 = 80 MHz
   3 = 80+80 or 160 MHz
   4 = 160+160 or 320 MHz */
extern const uint16_t eht_chan_width[5];

struct iwinfo_scanlist_entry {
	uint8_t mac[6];
	char ssid[IWINFO_ESSID_MAX_SIZE+1];
	enum iwinfo_opmode mode;
	uint8_t band;
	uint8_t channel;
	uint32_t mhz;
	uint8_t signal;
	uint8_t quality;
	uint8_t quality_max;
	struct iwinfo_crypto_entry crypto;
	struct iwinfo_scanlist_ht_chan_entry ht_chan_info;
	struct iwinfo_scanlist_vht_chan_entry vht_chan_info;
	struct iwinfo_scanlist_vht_chan_entry he_chan_info;
	struct iwinfo_scanlist_vht_chan_entry eht_chan_info;
};

struct iwinfo_country_entry {
	uint16_t iso3166;
	char ccode[4];
};

struct iwinfo_iso3166_label {
	uint16_t iso3166;
	const char name[28];
};

struct iwinfo_hardware_id {
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t subsystem_vendor_id;
	uint16_t subsystem_device_id;
	char compatible[128];
};

struct iwinfo_hardware_entry {
	char vendor_name[64];
	char device_name[64];
	uint16_t vendor_id;
	uint16_t device_id;
	uint16_t subsystem_vendor_id;
	uint16_t subsystem_device_id;
	int16_t txpower_offset;
	int16_t frequency_offset;
	char compatible[128];
};

extern const struct iwinfo_iso3166_label IWINFO_ISO3166_NAMES[];

#define IWINFO_HARDWARE_FILE	"/usr/share/libiwinfo/devices.txt"


struct iwinfo_ops {
	const char *name;

	int (*probe)(const char *ifname);
	int (*mode)(const char *, int *);
	int (*channel)(const char *, int *);
	int (*center_chan1)(const char *, int *);
	int (*center_chan2)(const char *, int *);
	int (*frequency)(const char *, int *);
	int (*frequency_offset)(const char *, int *);
	int (*txpower)(const char *, int *);
	int (*txpower_offset)(const char *, int *);
	int (*bitrate)(const char *, int *);
	int (*signal)(const char *, int *);
	int (*noise)(const char *, int *);
	int (*quality)(const char *, int *);
	int (*quality_max)(const char *, int *);
	int (*mbssid_support)(const char *, int *);
	int (*hwmodelist)(const char *, int *);
	int (*htmodelist)(const char *, int *);
	int (*htmode)(const char *, int *);
	int (*ssid)(const char *, char *);
	int (*bssid)(const char *, char *);
	int (*country)(const char *, char *);
	int (*hardware_id)(const char *, char *);
	int (*hardware_name)(const char *, char *);
	int (*encryption)(const char *, char *);
	int (*phyname)(const char *, char *);
	int (*assoclist)(const char *, char *, int *);
	int (*txpwrlist)(const char *, char *, int *);
	int (*scanlist)(const char *, char *, int *);
	int (*freqlist)(const char *, char *, int *);
	int (*countrylist)(const char *, char *, int *);
	int (*survey)(const char *, char *, int *);
	int (*lookup_phy)(const char *, char *);
	int (*phy_path)(const char *phyname, const char **path);
	void (*close)(void);
};

const char * iwinfo_type(const char *ifname);
const struct iwinfo_ops * iwinfo_backend(const char *ifname);
const struct iwinfo_ops * iwinfo_backend_by_name(const char *name);
void iwinfo_finish(void);

extern const struct iwinfo_ops wext_ops;
extern const struct iwinfo_ops madwifi_ops;
extern const struct iwinfo_ops nl80211_ops;
extern const struct iwinfo_ops wl_ops;

#include "iwinfo/utils.h"

#endif
