#ifndef ATF_H
#define ATF_H

#define ATF_GRANT_SCALE 10000 /* Grant equal to this number corresponds
                               * to 100% of air time */
#define ATF_MIN_VAP_GRANT 100 /* Min VAP grant in the per-VAP cfg mode */

struct hostapd_data;

struct atf_sta_grant
{
	uint8_t    sta_mac[ETH_ALEN];
	uint16_t   sta_grant;
};

struct atf_vap_config
{
	char*     vap_name;
	uint16_t  vap_grant;
	int       n_stations;    /* number of stations having grants */
	struct    atf_sta_grant* sta_grants;
};

struct atf_config /* Air Time Fairness configuration */
{
	uint8_t   debug;         /* do debug printing to logs */
	uint8_t   distr_type;    /* disabled / dynamic / static */
	uint8_t   algo_type;     /* global or weighted */
	uint8_t   weighted_type; /* PER_STATION / PER_STATION_PER_AC / PER_VAP / ... */
	uint8_t   per_vap;       /* 1 - ATF is per VAP (0 - per radio) */
	uint8_t   per_station;   /* 0 - station list is disallowed */
	uint32_t  interval;      /* as in .conf file, passed to FW unchanged */
	uint32_t  free_time;     /* as in .conf file, passed to FW unchanged */

	uint32_t  n_vaps;
	struct atf_vap_config* vap_cfg; /* memory allocated for n_vaps VAPs */
};

/* Free allocated memory and reset contents */
void hostapd_atf_clean_config(struct atf_config* atf_cfg);

/* Flush ATF data for all stations */
void hostapd_atf_clean_stations(struct hostapd_data *hapd);

/* Read ATF configuration from file */
int hostapd_atf_read_config(struct atf_config* atf_cfg, const char* pathname);

/* Calculate quotas and format a message to be sent to Driver */
struct intel_vendor_atf_quotas* hostapd_atf_calc_quotas(struct atf_config* atf_cfg,
		struct hostapd_data *hapd, const u8* changed_sta,
		int in_driver, int not_in_driver);

/* Does ATF configuration and capacity allow the given station to connect */
int hostapd_atf_is_sta_allowed(struct hostapd_data *bss, const u8 *address);

void read_token (char **p, char** token_start, int* token_len, char* delimiters);
#endif /* ATF_H */
