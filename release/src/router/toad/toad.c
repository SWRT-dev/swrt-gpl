/*
 * toad.c - Traffic Ordering Agent Daemon
 *
 * This is the traffic ordering agent daemon, which takes care of assigning a scheduler
 * and priority level to associated stations.
 *
 * Overall design is rather simple at this stage:
 *
 * 1. Query wlan driver for list of associated stations, and get their characteristics.
 * 2. Order Stations and assign scheduler types based on user configured settings.
 * 3. Periodically repeat 1 & 2.
 *
 * Copyright (C) 2014, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: toad.c 462558 2014-03-18 01:10:07Z $
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include <assert.h>
#include <typedefs.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <bcmendian.h>

#include <bcmendian.h>
#include <bcmwifi_channels.h>
#include <wlioctl.h>
#include <wlutils.h>
#include <shutils.h>
#include <proto/ethernet.h>

#include "toa_utils.h"

//#define htod32(val) (val)
//#define dtoh32(val) (val)
//#define htod16(val) (val)
//#define dtoh16(val) (val)

typedef enum {
	TOAD_ACTION_RUN,	/* Default action - run */
	TOAD_ACTION_HELP,	/* provide help and exit */
	TOAD_ACTION_NOP		/* Do nothing, just exit. */
} toad_action;

/* Debugging stuff that can be probably be removed in production */
#define DBG_ERROR_MASK		(1<<0)
#define DBG_TRACE_MASK		(1<<1)
#define DBG_DUMP_MASK		(1<<2)
#if defined(BCMDBG)
#define TOAD_ERROR(c, s) do { if ((c)->options->to_debug_mask & DBG_ERROR_MASK) \
	{ toa_log_error s; } } while (0)
#define TOAD_TRACE(c, s) do { if ((c)->options->to_debug_mask & DBG_TRACE_MASK) \
	{ printf("%s: ", __FUNCTION__); printf s; } } while (0)
#else
#define TOAD_ERROR(c, s) /* nop */
#define TOAD_TRACE(c, s) /* nop */
#endif

/*
 * Internal constants. We allow some of them to be overridden by make options.
 */
#if !defined(MAX_WL_INTERFACES)
#define MAX_WL_INTERFACES 2		/* Number of possible WL interfaces (eth*). More than 2 ? */
#endif

#if !defined(MAX_INTERFACE_BSS)
#define MAX_INTERFACE_BSS 16		/* Max number of BSS per interface (wl0.*). 0 and 1..15 */
#endif

/* TOAD options structure, as parsed from the command line */
typedef struct {
	toad_action	to_action;		/* Action to take (TOAD_ACTION_xxx) */
	unsigned long	to_debug_mask;		/* Debug mask, as the name says */
	unsigned long	to_ordering;		/* Ordering mask -- see bits below */
	unsigned int	to_poll_interval;	/* seconds between station polls */
	char *		to_wl_ifname;		/* WL Interface to operate on (eth1, eth2, ...) */
	char *		to_wl_ifname_list;	/* WL interface list, separated by spaces */
	char *		to_wl_ssid_list;	/* Matching SSID list, separated by spaces */
} toad_options;

#define TOAD_ORDER_AID		(1<<0)	/* Order EBOS stations by association ID */
#define TOAD_ORDER_RSSI		(1<<1)	/* Order EBOS stations by RSSI */
#define TOAD_ORDER_TXRATE	(1<<2)	/* Order EBOS stations by TX rate */
#define TOAD_ORDER_STAPRIO	(1<<3)	/* Order EBOS stations by staprio */
#define TOAD_ORDER_BSSID	(1<<4)	/* Order EBOS stations by BSS index */
#define TOAD_ORDER_USER		(1<<5)	/* Order EBOS stations by user prio */
#define TOAD_ORDER_SCORE	(1<<6)	/* Order EBOS stations by Phase 1 Scheduler Score */

/*
 * Default values. These can be overridden by command line options.
 */
static char default_to_wl_ifname[] = "eth1";
const unsigned int default_to_poll_interval = 5;		/* seconds */
const unsigned int default_to_ordering = (TOAD_ORDER_SCORE|TOAD_ORDER_BSSID|TOAD_ORDER_TXRATE);

typedef enum {
	SCHED_EBOS = 0,
	SCHED_PRR  = 1,
	SCHED_ATOS = 2,
	SCHED_ATOS2 = 3
} toa_scheduler_type;

typedef struct {
	struct ether_addr	ea;		/* Station MAC address                      */
	toa_scheduler_type	sch_type;	/* Scheduler type to use for this station   */
	toa_station_priority	sch_prio;	/* Scheduler priority to use (EBOS only)    */
	sta_info_t		sta_info;	/* Result from wl driver "sta_info" IOVAR   */
	uint32			sta_rssi;	/* Result from wl driver WLC_GET_RSSI ioctl */
	uint8			sta_staprio;	/* Result from wl driver "staprio" IOVAR    */
	uint8			sta_bss_index;	/* BSS index (0-MAX_INTERFACE_BSS)         */
	ssid_info_t		sta_ssid;	/* SSID (proper form, but in ascii only)    */
	uint32			sta_score;	/* Calculated score, as in Phase1 Scheduler */
} toad_station;

typedef struct {
	uint32 count;				/* Count of stations following  */
	toad_station station[1];		/* zero or more stations */
} toad_stations;

/* TOAD context, passed between functions. */
typedef struct {
	toad_options	*options;	/* Command line options */
	toa_settings	*settings;	/* NVRAM settings */
	toad_stations	*stations;	/* List of associated stations and their characteristics */
	unsigned long	ordering;	/* What ordering to do */
	unsigned long	ingredients;	/* What ingredients to fetch to decide order on */
	char		*wl_ifname[MAX_INTERFACE_BSS];	/* WL interface names for various BSSs */
	char		*wl_ssid[MAX_INTERFACE_BSS];	/* WL SSID for various BSSs */
} toad_context;

/*
 * toad_station_count() - safely get the number of stations currently in our station list.
 *
 * If zero is returned, it is not safe to access the toad_context stations, as the memory may
 * not have been allocated for them.
 */
static inline int
toad_station_count(toad_context *tc)
{
	return (tc->stations) ? tc->stations->count : 0;
}

/*
 * toad_main_ifname() - get the main interface name.
 *
 * This is the name of the wl interface through which we configure the scheduler, as well as the
 * primary (or only) interface.
 *
 * This function will return NULL if there is no primary interface. This would be a fatal error,
 * checked at initialisation time (after determining interfaces) and causing the daemon to exit.
 *
 */
static char *
toad_main_ifname(toad_context *tc)
{
	return (tc->wl_ifname[0]);
}

/*
 * Replacement for enet_ntoa() which cannot be used (duplicate definitions in netinet.h)
 */
static const char *
macaddr_to_ascii(const struct ether_addr *ea)
{
	static char buf[] = "xx:xx:xx:xx:xx:xx";
	snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
		ea->octet[0], ea->octet[1], ea->octet[2],
		ea->octet[3], ea->octet[4], ea->octet[5]);
	return buf;
}

/*
 * toad_wait_for_next_event() - as the name says.
 *
 * TBS: Here we could also monitor association events and configure new stations as
 * they associate. For now, we simply wait a little before allowing the next poll, which
 * achieves pretty much the same - and moreover lets us react to dynamic changes in rssi,
 * rate, or whatever.
 */
static int
toad_wait_for_next_event(toad_context *tc)
{
	TOAD_TRACE(tc, ("Sleeping %d seconds...\n", tc->options->to_poll_interval));

	sleep(tc->options->to_poll_interval);

	return 0;
}

/*
 * toad_station_list_apply() - instruct scheduler about relative priorities of associated stations.
 *
 * This function simply applies the previously arranged station list to the scheduler, using
 * the iovar provided for this purpose by the driver.
 *
 */
static int
toad_station_list_apply(toad_context *tc)
{
	int i, rc;

	for (i = 0; i < toad_station_count(tc); ++i) {
		wl_taf_define_t	td;
		toad_station *sta;

		sta = &tc->stations->station[i];

		TOAD_TRACE(tc, ("WL TAF ( <station=%s> <scheduler=%d> <priority=%d> )\n",
			macaddr_to_ascii(&sta->ea), sta->sch_type, sta->sch_prio));

		memset(&td, 0, sizeof(td));
		td.version = htod16(1);
		td.ea = sta->ea;
		td.sch = htod32(sta->sch_type);
		td.prio = htod32(sta->sch_prio);

		rc  = wl_iovar_set(toad_main_ifname(tc), "taf", &td, sizeof(td));

		if (rc) {
			TOAD_ERROR(tc, ("%s: Failed to set taf, rc = %d (skipped)\n",
				__FUNCTION__, rc));
		}
	}
	return 0;
}

/*
 * toad_scheduler_enabled() - test whether the traffic scheduler is enabled on the main interface.
 *
 * A failure to retrieve the information assumes that the scheduler is not enabled.
 */
static bool
toad_scheduler_enabled(toad_context *tc)
{
	char iob[ sizeof("taf") + sizeof(wl_taf_define_t) + sizeof("enable") ];
	wl_taf_define_t *td;
	int rc;

	memset(iob, 0, sizeof(iob));
	strcpy(iob, "taf");

	td = (wl_taf_define_t *)&iob[ sizeof("taf") ];
	td->version = htod16(1);
	memset(&td->ea, 0xff, sizeof(td->ea));
	strcpy(td->text, "enable");

	rc = wl_ioctl(toad_main_ifname(tc), WLC_GET_VAR, iob, sizeof(iob));
	if (rc == 0) {
		td = (wl_taf_define_t *)iob;
		return (!!td->misc);
	}

	TOAD_TRACE(tc, ("wl get taf enable failed, status %d, assuming not enabled.\n", rc));
	return FALSE;
}

static toa_scheduler_type
toad_get_station_scheduler(toa_record *rec)
{
	if (rec) {
		toa_attribute *att;


		/* Map station type to scheduler */

		att = toa_get_attribute_by_id(rec->attributes, ATTR_TYPE);
		if (att) {
			switch (toa_keyword_to_id(sta_type_map, att->value)) {
			case STA_TYPE_VIDEO:
				return SCHED_EBOS;
			case STA_TYPE_DATA:
				return SCHED_ATOS;
			case STA_TYPE_PUBLIC:
				return SCHED_ATOS2;
			default:
				break;
			}
		}
	}
	return SCHED_ATOS;	/* Return default scheduler */
}

static toa_station_priority
toad_get_station_priority(toa_record *rec)
{
	if (rec) {
		toa_attribute *att;

		att = toa_get_attribute_by_id(rec->attributes, ATTR_PRIO);
		if (att) {
			if (isdigit(att->value[0])) {
				return atoi(att->value);
			} else {
				toa_station_priority prio =
					toa_keyword_to_id(sta_prio_map, att->value);
				return (prio == STA_PRIO_ERROR) ? STA_PRIO_DEFAULT : prio;
			}
		}
	}
	return STA_PRIO_DEFAULT;
}

/*
 * Miscellaneous compare functions for qsort.
 */

#define TOAD_COMPARE(a, b) ((a) < (b) ? -1 : ((a) > (b) ? 1 : 0))

static int
toad_station_compare_tx_rate(const void *s1, const void *s2)
{
	const toad_station *sta1 = s1;
	const toad_station *sta2 = s2;

	return TOAD_COMPARE(sta2->sta_info.tx_rate, sta1->sta_info.tx_rate);
}

static int
toad_station_compare_rssi(const void *s1, const void *s2)
{
	const toad_station *sta1 = s1;
	const toad_station *sta2 = s2;

	return TOAD_COMPARE(sta2->sta_rssi, sta1->sta_rssi);
}

static int
toad_station_compare_aid(const void *s1, const void *s2)
{
	const toad_station *sta1 = s1;
	const toad_station *sta2 = s2;

	return TOAD_COMPARE(WL_STA_AID(sta1->sta_info.aid), WL_STA_AID(sta2->sta_info.aid));
}

static int
toad_station_compare_staprio(const void *s1, const void *s2)
{
	const toad_station *sta1 = s1;
	const toad_station *sta2 = s2;

	return TOAD_COMPARE(sta1->sta_staprio, sta2->sta_staprio);
}

static int
toad_station_compare_ssid_index(const void *s1, const void *s2)
{
	const toad_station *sta1 = s1;
	const toad_station *sta2 = s2;

	return TOAD_COMPARE(sta1->sta_bss_index, sta2->sta_bss_index);
}

static int
toad_station_compare_user_prio(const void *s1, const void *s2)
{
	const toad_station *sta1 = s1;
	const toad_station *sta2 = s2;

	return TOAD_COMPARE(sta1->sch_prio, sta2->sch_prio);
}

static int
toad_station_compare_score(const void *s1, const void *s2)
{
	const toad_station *sta1 = s1;
	const toad_station *sta2 = s2;

	return TOAD_COMPARE(sta2->sta_score, sta1->sta_score);	/* Higher score is better */
}


/*
 * toad_station_list_arrange() - As the name says, arrange the station list according to user setup.
 *
 * For static (configuration-based) ordering, there is no need to sort the array, as the priority
 * is set by the user defined configuration and applied as is to the scheduler.
 *
 * Dynamic ordering simply sorts the station list (array) based on the value(s) being sorted on,
 * in the order defined by the sorters[] array below (last entry = highest rank). After sorting,
 * we make a final pass over the station list and assign priorities from high to low.
 *
 */
static int
toad_station_list_arrange(toad_context *tc)
{
	static struct {
		uint32	order_bit;				/* Sort order option bit and */
		int (*comparator)(const void *, const void *);	/* matching compare function */
	} sorters[] = {
		{ TOAD_ORDER_AID,	toad_station_compare_aid },
		{ TOAD_ORDER_RSSI,	toad_station_compare_rssi },
		{ TOAD_ORDER_TXRATE,	toad_station_compare_tx_rate },
		{ TOAD_ORDER_STAPRIO,	toad_station_compare_staprio },
		{ TOAD_ORDER_BSSID,	toad_station_compare_ssid_index },
		{ TOAD_ORDER_USER,	toad_station_compare_user_prio },
		{ TOAD_ORDER_SCORE,	toad_station_compare_score },
		{ 0, NULL }
	};
	int i;

	if (toad_station_count(tc) == 0) {
		return 0;
	}

	TOAD_TRACE(tc, ("ordering %08lx ingredients %08lx, count %d\n", tc->ordering,
		tc->ingredients, toad_station_count(tc)));

	if (tc->ordering && toad_station_count(tc)) {
		int sid;
		int priority = STA_PRIO_HIGHEST;
		void *mbase = tc->stations->station;
		size_t mcount = toad_station_count(tc);
		size_t msize = sizeof(tc->stations->station[0]);

		TOAD_TRACE(tc, ("sorting mbase %p[%d] sz %d\n", mbase, mcount, msize));

		/* Step 2a: Sort stations depending on chosen ordering options */

		for (sid = 0; sorters[sid].order_bit; ++sid) {
			if (tc->ordering & sorters[sid].order_bit) {
				qsort(mbase, mcount, msize, sorters[sid].comparator);
			}
		}

		/* Step 2b: Once sorted, reassign the EBOS priorities. */
		TOAD_TRACE(tc, ("re-prioritizing %d stations\n", mcount));

		for (i = 0; i < mcount; ++i) {
			toad_station *tsta;

			tsta = &tc->stations->station[i];

			if (tsta->sch_type == SCHED_EBOS) {
				tsta->sch_prio = priority++;
			}
		}
	}


	return 0;
}

/*
 * Miscellaneous information retrieval functions. These will be called depending on the user
 * selected ordering options to retrieve information related to the option from the driver.
 *
 * The information is updated in the toad_station, where it resides in host byte order.
 */

/*
 * toad_retrieve_sta_info() - retrieve station "sta_info" information from the driver.
 *
 * We only care about the tx rate and aid here. If any other fields are needed, we must make
 * sure to convert them to host byte order as well in this function.
 */
static int
toad_retrieve_sta_info(toad_context *tc, char *wl_ifname, toad_station *sta)
{
	char *buf;
	int len, rc;

	/* Ensure we can use the sta_info buffer space for the retrieval. */
	STATIC_ASSERT(sizeof(*sta) >= sizeof("sta_info")+1+sizeof(sta->ea));

	buf = (char *)&sta->sta_info;
	len  = sprintf(buf, "sta_info");
	memcpy(&buf[len+1], &sta->ea, sizeof(sta->ea));

	rc = wl_ioctl(wl_ifname, WLC_GET_VAR, buf, sizeof(sta->sta_info));

	if (!rc) {
		sta->sta_info.tx_rate = dtoh32(sta->sta_info.tx_rate);
		sta->sta_info.aid = dtoh16(sta->sta_info.aid);
	}
	return rc;
}

/*
 * toad_retrieve_rssi() - retrieve station RSSI from the driver.
 */
static int
toad_retrieve_rssi(toad_context *tc, char *wl_ifname, toad_station *sta)
{
	int rc;
	scb_val_t scb_val;

	sta->sta_rssi = 0;

	scb_val.ea = sta->ea;
	rc = wl_ioctl(wl_ifname, WLC_GET_RSSI, &scb_val, sizeof(scb_val));
	if (!rc) {
		sta->sta_rssi = dtoh32(scb_val.val);
	}
	return rc;
}

/*
 * toad_retrieve_staprio() - retrieve station "staprio" value from the driver.
 */
static int
toad_retrieve_staprio(toad_context *tc, char *wl_ifname, toad_station *sta)
{
	static const int staprio_buf_len = sizeof("staprio")+sizeof(sta->ea);
	char buf[staprio_buf_len];
	int len, rc;

	sta->sta_staprio = 0;
	len  = sprintf(buf, "staprio");
	memcpy(&buf[len+1], &sta->ea, sizeof(sta->ea));
	rc = wl_ioctl(wl_ifname, WLC_GET_VAR, buf, staprio_buf_len);
	if (!rc) {
		wl_staprio_cfg_t *sp = (wl_staprio_cfg_t *)buf;

		sta->sta_staprio = sp->prio;
	}
	return rc;
}

static int
toad_retrieve_sta_ssid(toad_context *tc, char *wl_ifname, toad_station *sta)
{
	 return 0;
}

/*
 * toad_rate_to_bits() - helper function to convert the rate to an 8-bit value.
 *
 * This replicates a part of the behaviour of the Phase 1 scheduler internal ordering agent.
 */
static int
toad_rate_to_bits(uint32 rateval)
{
	uint32 rate = rateval/512;
	int inc = 0;

	/*
	 * This algorithm below is a kind of simplified logarithm to reduce the bits used
	 * by the rate value and to ensure that small rates will still be strictly ordered.
	 * Parameters have been carefully chosen for an 8 bits mask, do not change them!
	 * Rates above 2.8 GBps will be capped at 0xff.
	 */
	while (inc++ < 4 && 0x40 * inc < rate)
		rate = rate/4  + 0x30 * inc;

	return rate;
}

/*
 * toad_calculate_score() - calculate the equivalent of the phase 1 scheduler score.
 *
 * This function assumes that all necessary ingredients are already in the toad_station,
 * which is ensured by having it called after the required fetchers.
 *
 * The 'ingredients' bits tells us what ingredients have been fetched, and we only take these
 * into account for calculating the score.
 */
static int
toad_calculate_score(toad_context *tc, char *wl_ifname, toad_station *sta)
{
	uint32	score = 0;
	uint32  ingredients = tc->ingredients;

	if (ingredients & TOAD_ORDER_AID) { /* Lower AID preferred */
		score |= (0xf - (WL_STA_AID(sta->sta_info.aid) & 0xf));
	}

	if (ingredients & TOAD_ORDER_RSSI) { /* Higher RSSI preferred */
		score |= ((sta->sta_rssi & 0xff) << 4);
	}

	if (ingredients & TOAD_ORDER_TXRATE) { /* Higher rate preferred */
		score |= ((toad_rate_to_bits(sta->sta_info.tx_rate) & 0xff) << (8+4));
	}

	if (ingredients & TOAD_ORDER_STAPRIO) { /* Higher staprio preferred */
		score |= ((sta->sta_staprio & 0xf) << (8+8+4));
	}

	if (ingredients & TOAD_ORDER_BSSID) { /* Lower BSS preferred */
		score |= ((0xf - (sta->sta_bss_index & 0xf)) << (4+8+8+4));
	}

	if (ingredients & TOAD_ORDER_USER) {	/* Lower user prio preferred */
		score |= ((0xf - (sta->sch_prio & 0xf)) << (4+4+8+8+4));
	}

	sta->sta_score = score;

	TOAD_TRACE(tc, ("Phase 1 score is 0x%08x\n", score));

	return 0;
}

static void
toad_load_sta_scheduler(toad_context *tc, toad_station *sta)
{
	toa_record *srec = toa_match_record(&tc->settings->station_settings,
		macaddr_to_ascii(&sta->ea), FALSE);

	if (!srec) {
		char bss[12];
		snprintf(bss, sizeof(bss), "%d", sta->sta_bss_index);
		/* If no match in station settings, fall back to bss index settings */
		srec = toa_match_record(&tc->settings->bss_settings, bss, FALSE);
		/* We could add more fallbacks here if needed. */
		/* (may want to make the different settings an array?) */
	}

	if (!srec) {
		/* If no match in ssid settings either, fall back to default settings */
		srec = tc->settings->default_settings;	/* (Only one exists of these) */
	}

	/*
	 * Note - we could get fancy here and merge attributes from different sources,
	 * so that specific attributes would override more generic ones. But let's not
	 * over-engineer this right now - if requested, this is easy enough to add.
	 */

	sta->sch_type = toad_get_station_scheduler(srec);

	sta->sch_prio = (sta->sch_type  == SCHED_EBOS) ?
		toad_get_station_priority(srec) : 0;
}

/*
 * toad_load_stations_from_assoclist() - As the name says, load stations from assoclist list.
 *
 * This function takes an assoclist (struct maclist *) as input, containing zero or more new
 * stations to append to our internal association list. It will allocate or resize that list
 * as needed, adding the new stations at the end and updating the station count. Ingredients
 * for the new stations will be retrieved as well, and composite values (like SCORE) will be
 * updated.
 *
 */
static int
toad_load_stations_from_assoclist(toad_context *tc, char *wl_ifname, int bssindex, char *bssname,
	struct maclist *assoclist)
{
	static struct {
		uint32	ingredients;	/* Bits determining whether we need to call fetch_func() */
		int (*fetch_func)(toad_context *, char *wl_ifname, toad_station *);
		const char *description;	/* Asciz description, for debugging */
	} fetchers[] = {
		/* Ingredients */
		{ TOAD_ORDER_AID | TOAD_ORDER_TXRATE, toad_retrieve_sta_info, "AID, Tx Rate" },
		{ TOAD_ORDER_RSSI, toad_retrieve_rssi, "rssi" },
		{ TOAD_ORDER_BSSID, toad_retrieve_sta_ssid, "BSS Index" },
		{ TOAD_ORDER_STAPRIO, toad_retrieve_staprio, "staprio" },
		/* Many more possibilities can be thought of, such as link quality etc. */
		/* Composite values (which may need some of the above ingredients) */
		{ TOAD_ORDER_SCORE, toad_calculate_score, "Phase 1 Scheduler Score" },
		{ 0, NULL, NULL }
	};
	int i, rc, station_count;
	void *p;


	/* Create or extend the current station list (array). */
	station_count = toad_station_count(tc);

	i = sizeof(toad_stations)
		- sizeof(toad_station)	/* account for the one-element array size declaration */
		+ sizeof(toad_station) * (station_count + assoclist->count);

	p = realloc(tc->stations, i);	/* Create or extend the station array */
	if (!p) {
		TOAD_ERROR(tc, ("%s: realloc failed, %s\n", __FUNCTION__, strerror(errno)));
		/* Note: tc->stations remains untouched if realloc fails, keep it as is. */
		return -1;
	}

	/* Update address in case it changed, set up station count */
	tc->stations = p;
	tc->stations->count = station_count;

	/* Clear the newly added stations and add them to the totals */
	p = tc->stations;
	memset(p + i - (assoclist->count*sizeof(toad_station)), 0,
		assoclist->count * sizeof(toad_station));

	tc->stations->count += assoclist->count;

	for (i = 0; i < assoclist->count; ++i) {

		toad_station *sta = &tc->stations->station[station_count+i];

		sta->ea = assoclist->ea[i];
		sta->sta_bss_index = bssindex;
		if (bssname) {
			strncpy((char *)sta->sta_ssid.ssid, bssname, sizeof(sta->sta_ssid.ssid));
			sta->sta_ssid.ssid_len = strlen(bssname);
			if (sta->sta_ssid.ssid_len >= sizeof(sta->sta_ssid.ssid)) {
				sta->sta_ssid.ssid_len = sizeof(sta->sta_ssid.ssid) - 1;
				sta->sta_ssid.ssid[sta->sta_ssid.ssid_len] = '\0';
			}
		} else {
			sta->sta_ssid.ssid[0] = '\0';
			sta->sta_ssid.ssid_len = 0;
		}

		toad_load_sta_scheduler(tc, sta);

		/*
		 * Retrieve any additional per-station info as needed for advanced station ordering.
		 */
		if (tc->ingredients) {
			int f;

			for (f = 0; fetchers[f].ingredients; ++f) {
				if (tc->ingredients & fetchers[f].ingredients) {

					TOAD_TRACE(tc, ("Fetching station %s %s\n",
						macaddr_to_ascii(&sta->ea),
						fetchers[f].description));

					rc = fetchers[f].fetch_func(tc, wl_ifname, sta);
					if (rc) {
						TOAD_ERROR(tc, ("%s: Failed to fetch station %s %s,"
							" rc=%d\n", __FUNCTION__,
							macaddr_to_ascii(&sta->ea),
							fetchers[f].description, rc));
					}
				}
			}
		}
	}
	return 0;
}

/*
 * toad_station_list_retrieve() - update the list of currently associated stations and related info.
 *
 * This function retrieves the list of currently associated stations from the driver, and fetches
 * any per-station information we may need to arrange the stations for the scheduler.
 *
 * The user configuration provides us with the interface name to use, ie, eth1, eth2, etc. Based
 * on that information, we can query the wl driver - but we also want to query mbss interfaces,
 * such as wl0.1 - for these we need to go through some hoops to figure them from nvram settings.
 *
 * If no nvram settings exist, we only handle the user specified interface.
 */
static int
toad_station_list_retrieve(toad_context *tc)
{
	struct maclist *assoclist;
	int rc, bssindex;
	char *wl_ifname;

	/* Free previous station list */
	if (tc->stations) {
		free(tc->stations);
		tc->stations = NULL;
	}

	if (!toad_scheduler_enabled(tc)) {
		TOAD_TRACE(tc, ("TAF is not enabled on interface %s\n", toad_main_ifname(tc)));
		return 0;
	}

	assoclist = malloc(WLC_IOCTL_MEDLEN);
	if (!assoclist) {
		TOAD_ERROR(tc, ("%s: malloc failed, %s\n", __FUNCTION__, strerror(errno)));
		return -1;
	}

	for (bssindex = 0; bssindex < MAX_INTERFACE_BSS; ++bssindex)
	{
		if (!(wl_ifname = tc->wl_ifname[bssindex])) {	/* Skip null interfaces */
			continue;
		}

		/* Fetch the assoclist for this interface */

		assoclist->count = htod32((WLC_IOCTL_MEDLEN - sizeof(int)) / ETHER_ADDR_LEN);
		rc = wl_ioctl(wl_ifname, WLC_GET_ASSOCLIST, assoclist, WLC_IOCTL_MEDLEN);
		if (rc < 0) {
			TOAD_ERROR(tc, ("%s: Failed to get assoclist for %s, rc=%d\n",
				__FUNCTION__, wl_ifname, rc));
			free(assoclist);
			return rc;
		}
		assoclist->count = dtoh32(assoclist->count);

		TOAD_TRACE(tc, ("Loading %d stations for [%2d]%s:%s\n",
			assoclist->count, bssindex, wl_ifname, tc->wl_ssid[bssindex]));

		if (assoclist->count) {
			rc = toad_load_stations_from_assoclist(tc, wl_ifname,
				bssindex, tc->wl_ssid[bssindex], assoclist);
			if (rc) {
				TOAD_ERROR(tc, ("%s: Failed to load stations for %s\n",
					__FUNCTION__, wl_ifname));
			}
		}
	}
	free(assoclist);
	return 0;
}

/*
 * toad_main_loop() - Traffic Ordering Agent Daemon main loop.
 *
 * Daemonize (unless in debug mode)
 * Get the list of current associated stations.
 * Get detailed info for each associated station.
 * Match against config, apply settings if found.
 * Set up timer for periodic station status poll and eventual reordering.
 * Wait for something to happen.
 *
 */
static int
toad_main_loop(toad_context *tc)
{
	int rc = 0;

	toa_log_error("Broadcom Traffic Ordering Agent -- starting on %s as %s...\n",
		toad_main_ifname(tc),
		(tc->options->to_debug_mask & ~DBG_ERROR_MASK) ?
		"foreground task": "daemon process");

	/* Daemonize, unless we are debugging. Allow the error mask for the daemon though. */
	if ((tc->options->to_debug_mask & ~DBG_ERROR_MASK) == 0) {

		rc = daemon(0, 0);

		if (rc < 0) {
			TOAD_ERROR(tc, ("%s: daemon() failed, %s\n",
				__FUNCTION__, strerror(errno)));
			return rc;
		}
	}

	while (rc == 0) {
		rc = toad_station_list_retrieve(tc) ||
			toad_station_list_arrange(tc) ||
			toad_station_list_apply(tc) ||
			toad_wait_for_next_event(tc);
	}

	toa_log_error("Broadcom Traffic Ordering Agent -- exiting with status %d\n", rc);
	return rc;
}

/*
 * toad_parse_cmdline() - parse the command line options into passed toad_options structure.
 *
 * We need to support some form of command to set the attribute(s) for a station in order to
 * facilitate command line configuration, in particular since the nvram key is a sequentially
 * increasing number. We also want to be able to delete a station (renumbering the list), and
 * list all stations.
 *
 * Returns 0 on success.
 */
int
toad_parse_cmdline(toad_options *top, int argc, char **argv)
{
	int argn = 1;

	top->to_action = TOAD_ACTION_RUN;

	/* Process command line options and determine action to take. */

	while ((argn < argc) && (argv[argn][0] == '-')) {
		int have_arg = (argn < (argc-1));
		char *opt = argv[argn];

		if ((!strcmp(opt, "-i") || !strcmp(opt, "--interface")) && have_arg) {
			top->to_wl_ifname = argv[++argn];
		} else
		if ((!strcmp(opt, "-I") || !strcmp(opt, "--interfacelist")) && have_arg) {
			top->to_wl_ifname_list = argv[++argn];
		} else
		if ((!strcmp(opt, "-S") || !strcmp(opt, "--ssidlist")) && have_arg) {
			top->to_wl_ssid_list = argv[++argn];
		} else
		if ((!strcmp(opt, "-O") || !strcmp(opt, "--ordering")) && have_arg) {
			top->to_ordering = strtoul(argv[++argn], NULL, 0);
		} else
		if ((!strcmp(opt, "-D") || !strcmp(opt, "--debug")) && have_arg) {
			top->to_debug_mask = strtoul(argv[++argn], NULL, 0);
		} else
		if ((!strcmp(opt, "-p") || !strcmp(opt, "--poll")) && have_arg) {
			top->to_poll_interval = strtoul(argv[++argn], NULL, 0);
		} else
		if (!strcmp(opt, "-?") ||!strcmp(opt, "-h") || !strcmp(opt, "--help")) {
			top->to_action = TOAD_ACTION_HELP;
			return 0;
		} else {
			fprintf(stderr, "%s: unrecognized option '%s'\n"
				"Try '%s --help' for more information.\n",
				argv[0], opt, argv[0]);
			return -1;
		}
		++argn;
	}

	while (argn < argc) {
		fprintf(stderr, "%s: Excess argument \"%s\"\n", argv[0], argv[argn]);
		top->to_action = TOAD_ACTION_NOP;
		++argn;
	}

	return 0;
}

/*
 * toad_usage() - provide a help message.
 */
static void
toad_usage(char *pname)
{
	printf("usage: %s [options]\n", pname);
	printf("options are:\n"
		"    -D <mask>       Specify the debugging bitmask\n"
		"    -h              This help text\n"
		"    -i <ifname>     Specify the WL driver interface to operate on (default=%s)\n"
		"    -I \"<ifname> <ifname> ...\"    Specify a list of WL driver"
		" interfaces to operate on\n"
		"    -S \"<ssid> <ssid> ...\"        Specify a list of SSIDs"
		" matching the interface list\n"
		"    -O <mask>       Specify the TOAD Ordering options, as a bitmask:\n"
		"       (no value)   Order stations statically, as instructed by configuration\n"
		"       %08x     Order stations dynamically by Phase 1 scheduler score\n"
		"       %08x     Order stations dynamically by user priority\n"
		"       %08x     Order stations dynamically by BSS index\n"
		"       %08x     Order stations dynamically by staprio\n"
		"       %08x     Order stations dynamically by TX Rate\n"
		"       %08x     Order stations dynamically by RSSI\n"
		"       %08x     Order stations dynamically by Association ID\n"
		"      (%08x is the default ordering options bitmask)\n"
		"    -p <seconds>    Specify the poll interval (default=%d seconds)\n",
		default_to_wl_ifname,
		TOAD_ORDER_SCORE,
		TOAD_ORDER_USER,
		TOAD_ORDER_BSSID,
		TOAD_ORDER_STAPRIO,
		TOAD_ORDER_TXRATE,
		TOAD_ORDER_RSSI,
		TOAD_ORDER_AID,
		default_to_ordering,
		default_to_poll_interval);
	printf("Note that if an interface list is specified, the first interface"
		" must be the master interface.\n");
}

/*
 * toad_cleanup_interfaces() - free any memory associated with the interface names and ssids.
 */
static void
toad_cleanup_interfaces(toad_context *tc)
{
	int i;

	for (i = 0; i < MAX_INTERFACE_BSS; ++i) {
		if (tc->wl_ifname[i]) {
			free(tc->wl_ifname[i]);
			tc->wl_ifname[i] = NULL;
		}
		if (tc->wl_ssid[i]) {
			free(tc->wl_ssid[i]);
			tc->wl_ssid[i] = NULL;
		}
	}
}
/*
 * toad_cleanup() - clean up whatever needs to be cleaned up before exiting.
 *
 * - Free station list if it exists
 * - Free configuration settings if they exist
 */
static void
toad_cleanup(toad_context *tc)
{
	if (tc->stations) {
		free(tc->stations);
		tc->stations = NULL;
	}

	if (tc->settings) {
		toa_free_settings(tc->settings);
		tc->settings = NULL;
	}
	toad_cleanup_interfaces(tc);

}

/*
 * toad_determine_interfaces_from_nvram() - determine which interfaces we are to operate on.
 *
 * Here we rely on the nvram settings for the router, however, there may be cases where these
 * are not available and some other method needs to be implemented - command line, specific
 * iovar, or whatever, requiring customisation of this function here.
 *
 * NVRAM settings on the router are as follows for eth1. Note there can be holes in the sequence
 * and the interface names can be different, hence we need to scan and remember at all of them:
 * wl0_ifname=eth1
 * wl0_ssid=SSID0
 * wl0.1_ifname=wl0.1
 * wl0.1_ssid=SSID1
 * wl0.7_ifname=wl0.7
 * wl0.7_ssid=SSID7
 *
 */
static int
toad_determine_interfaces_from_nvram(toad_context *tc)
{
	char nvram_key[32];
	int wlindex, bssindex, max_bssindex = MAX_INTERFACE_BSS-1;
	char *wl_ssid = NULL;
	char *wl_ifname = tc->options->to_wl_ifname;

	/* Find out the wl interface index for the specified interface. */
	for (wlindex = 0; wlindex < MAX_WL_INTERFACES; ++wlindex) {

		snprintf(nvram_key, sizeof(nvram_key), "wl%d_ifname", wlindex);

		if (nvram_match(nvram_key, wl_ifname)) {
			snprintf(nvram_key, sizeof(nvram_key), "wl%d_ssid", wlindex);
			wl_ssid = nvram_get(nvram_key);
			break;
		}
	}

	if (wlindex >= MAX_WL_INTERFACES) {
		max_bssindex = 0;	/* No match = no idea about prefix = no mbss. */
	}

	/* Assign interface and SSID for the main and eventual mbss ssids */

	for (bssindex = 0; bssindex <= max_bssindex; ++bssindex) {

		if (wl_ifname) {

			TOAD_TRACE(tc, ("Found wl interface #%d: Name '%s' SSID '%s'\n",
				bssindex, wl_ifname, wl_ssid));

			tc->wl_ifname[bssindex] = strdup(wl_ifname);
			if (!tc->wl_ifname[bssindex]) {
				TOAD_ERROR(tc, ("%s: strdup of interface name failed\n",
					__FUNCTION__));
				return -1;
			}

			if (wl_ssid) {
				tc->wl_ssid[bssindex] = strdup(wl_ssid);
				if (!tc->wl_ssid[bssindex]) {
					TOAD_ERROR(tc, ("%s: strdup of ssid failed\n",
						__FUNCTION__));
					return -1;
				}
			}
		}
		/* Prepare for next bss, if any */
		if (bssindex < max_bssindex) {

			snprintf(nvram_key, sizeof(nvram_key), "wl%d.%d_ifname",
				wlindex, bssindex+1);
			wl_ifname = nvram_get(nvram_key);

			snprintf(nvram_key, sizeof(nvram_key), "wl%d.%d_ssid",
				wlindex, bssindex+1);
			wl_ssid =  nvram_get(nvram_key);
		}
	}
	return 0;
}

/*
 * toad_determine_interfaces_from_cmdline() - determine interface and ssid list from command line.
 *
 * This is a simple example of how to parse the command line into interface and SSID lists, using
 * the toad command line as follows (including the quotes):
 *
 *	toad -I "eth1 wl0.1 wl0.7 wl0.15" -S "main-ssid slave1-ssid slave2-ssid"
 *
 * If specified, these lists override the main interface command line qualifier. This could be used
 * for example if there are no NVRAM settings to look at for determining the slave names and SSIDs.
 *
 * Note:
 * - The first interface in the list must be the main interface, through which we address the taf.
 * - The order of the interfaces determines their index (0..MAX_INTERFACE_BSS), which may be used
 *   for ordering.
 */
static int
toad_determine_interfaces_from_cmdline(toad_context *tc)
{
	char wl_name[INTF_NAME_SIZ+1];
	char *next;
	int bssindex;

	bssindex = 0;
	foreach(wl_name, tc->options->to_wl_ifname_list, next) {
		tc->wl_ifname[bssindex] = strdup(wl_name);
		if (!tc->wl_ifname[bssindex]) {
			TOAD_ERROR(tc, ("%s: strdup of interface name failed\n", __FUNCTION__));
			return -1;
		}
		++bssindex;
		if (bssindex >= MAX_INTERFACE_BSS) {
			TOAD_ERROR(tc, ("Interface names past #%d ignored\n", MAX_INTERFACE_BSS-1));
			break;
		}
	}

	/* If no ssid list was given, we are done (and foreach() does not like null pointers) */
	if (tc->options->to_wl_ssid_list == NULL) {
		return 0;
	}

	bssindex = 0;
	foreach(wl_name, tc->options->to_wl_ssid_list, next) {
		tc->wl_ssid[bssindex] = strdup(wl_name);
		if (!tc->wl_ssid[bssindex]) {
			TOAD_ERROR(tc, ("%s: strdup of interface ssid failed\n", __FUNCTION__));
			return -1;
		}
		++bssindex;
		if (bssindex >= MAX_INTERFACE_BSS) {
			TOAD_ERROR(tc, ("Interface SSIDs past #%d ignored\n", MAX_INTERFACE_BSS-1));
			break;
		}
	}
	return 0;
}

/*
 * toad_determine_interfaces() - determine which interfaces we are to operate on (for MBSS).
 *
 * This function takes care of figuring out which interfaces make up a multi-bss interface,
 * and populate the wl_ifname[] and wl_ssid[] arays with pointers to the corresponding names,
 * which need to be released in toad_cleanup_interfaces().
 */
static int
toad_determine_interfaces(toad_context *tc)
{
	int rc;

	/* Clean up for a start so this can be used more than once if desired. */
	toad_cleanup_interfaces(tc);

	if (tc->options->to_wl_ifname_list) {
		rc = toad_determine_interfaces_from_cmdline(tc);
	} else {
		rc = toad_determine_interfaces_from_nvram(tc);
	}

	if (rc == 0) {
		if (toad_main_ifname(tc) == NULL) {
			TOAD_ERROR(tc, ("%s: Could not determine main WL interface\n",
				__FUNCTION__));
			rc = -1;
		}
	}

	return rc;
}

/*
 * toad_initialise() - perform initialisation tasks.
 *
 * - Load configuration settings.
 * - Determine the list of interfaces to operate on.
 *
 */
static int
toad_initialise(toad_context *tc)
{
	tc->settings = toa_load_settings();
	if (!tc->settings) {
		return -1;
	}

	/* Determine ordering and ingredients based on command line. Fix up for combos */

	tc->ordering = tc->options->to_ordering;
	tc->ingredients = tc->ordering;

	if (tc->ordering & TOAD_ORDER_SCORE) {
		tc->ordering = TOAD_ORDER_SCORE;		/* Limit ordering to score only */
		tc->ingredients = tc->options->to_ordering;	/* and use selected ingredients */
	}

	toad_determine_interfaces(tc);

	return (toad_main_ifname(tc)) ? 0 : -1;	/* Fail and quit if we have no main interface */
}
/*
 * main()
 *
 * Initialise, process command line, load settings, execute, cleanup, exit.
 *
 */
int main(int argc, char **argv)
{
	toad_options to = {
		.to_wl_ifname = default_to_wl_ifname,
		.to_ordering = default_to_ordering,
		.to_debug_mask = (DBG_ERROR_MASK),
		.to_poll_interval = default_to_poll_interval
	};
	toad_context tc = { .options = &to };
	int rc;

	rc = toad_parse_cmdline(&to, argc, argv);

	if (rc == 0) {
		switch (to.to_action) {

		case TOAD_ACTION_RUN:
			rc = toad_initialise(&tc) || toad_main_loop(&tc);
			break;

		case TOAD_ACTION_HELP:
			toad_usage(argv[0]);
			break;

		case TOAD_ACTION_NOP:
			break;
		}
	}

	toad_cleanup(&tc);

	return rc;
}
