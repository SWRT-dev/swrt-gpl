/*
 * bcm_supenv.h -- Minimal data structures to support wlc_sup.c in user space
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: bcm_supenv.h,v 1.9 2010-03-08 22:49:25 $
 */

#ifndef _bcm_supenv_h
#define _bcm_supenv_h

#include <proto/ethernet.h>
#include <proto/802.11.h>

#if defined(BCM_OSL)
#include "osl.h"
#else
#include <bcm_osl.h>
#include <bcm_lbuf.h>
#endif

#include <bcmutils.h>

typedef struct wsec_iv {
	uint32		hi;	/* upper 32 bits of IV */
	uint16		lo;	/* lower 16 bits of IV */
} wsec_iv_t;

#include <wlioctl.h>

#if defined(BCM_OSL)
#define TXOFF 170
#endif


typedef struct wlc_bss_info
{
	struct ether_addr BSSID;	/* network BSSID */
	uint8		SSID_len;	/* the length of SSID */
	uint8		SSID[32];	/* SSID string */
	struct dot11_bcn_prb *bcn_prb;	/* beacon/probe response frame (ioctl na) */
	uint16		bcn_prb_len;	/* beacon/probe response frame length (ioctl na) */

} wlc_bss_info_t;

struct wlc_pub {
	uint unit;
	wlc_bss_info_t	current_bss;	/* STA BSS if active, else first AP BSS */
	osl_t		*osh;			/* pointer to os handle */
#ifdef WLCNT
	wl_cnt_t	_cnt;			/* monolithic counters struct */
	wl_wme_cnt_t	_wme_cnt;		/* Counters for WMM */
#endif /* WLCNT */
#ifdef BCMWPA2
	pmkid_cand_t	pmkid_cand[MAXPMKID];	/* PMKID candidate list */
	uint		npmkid_cand;	/* num PMKID candidates */
	pmkid_t		pmkid[MAXPMKID];	/* PMKID cache */
	uint		npmkid;			/* num cached PMKIDs */
#endif /* BCMWPA2 */


};
typedef struct wlc_pub wlc_pub_t;

#ifdef WLCNT
#define WLCNTINCR(a) ((a)++)
#else
#define WLCNTINCR(a)
#endif /* WLCNT */

/* Lots of fakery to avoid source code changes! */

#define BCM_SUPFRMBUFSZ		2048
#define WLC_MAXBSSCFG		4

struct wlc_bsscfg {
	struct wlc_info	*wlc;		/* wlc to which this bsscfg belongs. */
	bool		inuse;	/* is this config in use */
	bool		up;		/* is this configuration up */
	bool		enable;		/* is this configuration enabled */
	bool		_ap;		/* is this configuration an AP */


	void		*sup;		/* pointer to supplicant state */
	int		sup_type;	/* type of supplicant */
	void		*authenticator;	/* pointer to authenticator state */
	uint8		SSID_len;	/* the length of SSID */
	uint8		SSID[DOT11_MAX_SSID_LEN];	/* SSID string */

	struct ether_addr   BSSID;      /* BSSID */
	struct ether_addr   cur_etheraddr;  /* BSSID */
	uint32		WPA_auth;	/* WPA: authenticated key management */

	wsec_iv_t	wpa_none_txiv;	/* global txiv for WPA_NONE, tkip and aes */
	bool have_keys;		/* keys installed */
	bool auth_pending;
	ctx_t  ctx;	/* holds context pointers */

	uint8 sup_ies[256];		/* assoc req ie */
	uint8	sup_ies_len;
	uint8 auth_ies[256];	/* probe resp ie */
	uint8 auth_ies_len;
	struct dot11_assoc_resp *assoc_resp;	/* last (re)association response */
	uint		assoc_resp_len;
	int			btamp_enabled;


};

typedef struct wlc_bsscfg wlc_bsscfg_t;
struct wlc_info {

	wlc_pub_t	pub;			/* wlc public state (must be first field of wlc) */
	void		*wl;			/* pointer to os-specific private state */
	bool        sta_associated; /* true if STA bsscfg associated to AP */
	/* BSS Configurations */
	wlc_bsscfg_t	bsscfg[WLC_MAXBSSCFG];	/* set of BSS configurations */

	wlc_bsscfg_t cfg;			/* the primary bsscfg (can be AP or STA) */
	int sup_wpa2_eapver;
	bool sup_m3sec_ok;
	struct dot11_assoc_resp *assoc_resp; /* last (re)association response */
	uint					 assoc_resp_len;

};

typedef struct wlc_info wlc_info_t;


typedef struct supplicant supplicant_t;


/* Absolutely minimalist pktbuffer utilities */

extern void bcm_supenv_init(void);

extern void
wlc_mac_event(wlc_info_t* wlc, uint msg, const struct ether_addr* addr,
	uint result, uint status, uint auth_type, void *data, int datalen);

extern void
wlc_getrand(wlc_info_t *wlc, uint8 *buf, int buflen);

extern char *
bcm_ether_ntoa(const struct ether_addr *ea, char *buf);


/* macros to help with different environments */
#define SEND_PKT(bsscfg, p) wpaif_sendpkt((bsscfg), (p))

#define PLUMB_TK(a, b)	wpaif_plumb_ptk((a), (b))

#define PLUMB_GTK(pkey, bsscfg) \
	wpaif_plumb_gtk((pkey), (bsscfg))

#define AUTHORIZE(bsscfg) \
				wpaif_set_authorize(bsscfg)
#define PUSH_KRK_TO_WLDRIVER(bsscfg, krk, krk_len)	\
				wpaif_set_krk(bsscfg, krk, krk_len)

#define DEAUTHENTICATE(bsscfg, reason) \
				wpaif_set_deauth(bsscfg, reason);

extern int
wpaif_plumb_ptk(wl_wsec_key_t *key, wlc_bsscfg_t *bsscfg);
extern int
wpaif_plumb_gtk(wl_wsec_key_t *key, wlc_bsscfg_t *bsscfg);
extern int
wpaif_set_authorize(wlc_bsscfg_t *bsscfg);
extern int
wpaif_set_deauth(wlc_bsscfg_t *bsscfg, int reason);

extern int
wpaif_sendpkt(wlc_bsscfg_t *bsscfg, void *p);
extern int
wpaif_set_krk(wlc_bsscfg_t *bsscfg, uint8 *krk, int krk_len);

/* supplicant status callback */
void wpaif_forward_mac_event_cb(wlc_bsscfg_t *bsscfg, uint reason, uint status);

#endif /* _bcm_supenv_h */
