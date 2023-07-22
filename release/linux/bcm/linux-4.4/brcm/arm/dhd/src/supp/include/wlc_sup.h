/*
 * Exposed interfaces of wlc_sup.c
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: wlc_sup.h,v 1.3 2010-03-08 22:49:25 $
 */

#ifndef _wlc_sup_h_
#define _wlc_sup_h_

#if defined(BCMSUP_LEAP)
#include <proto/802.11_ccx.h>
#endif 

/* Initiate supplicant private context */
extern void *wlc_sup_attach(wlc_info_t *wlc, wlc_bsscfg_t *cfg);

/* Remove supplicant private context */
extern void wlc_sup_detach(struct supplicant *sup);

/* Down the supplicant, return the number of callbacks/timers pending */
extern int wlc_sup_down(struct supplicant *sup);

/* Send received EAPOL to supplicant; Return whether packet was used
 * (might still want to send it up to other supplicant)
 */
extern bool wlc_sup_eapol(struct supplicant *sup, eapol_header_t *eapol_hdr,
	bool encrypted, bool *auth_pending);


/* Values for type parameter of wlc_set_sup() */
#define SUP_UNUSED	0 /* Supplicant unused */
#if defined(BCMSUP_LEAP)
#define SUP_LEAP	1 /* Used for Leap */
#endif 
#ifdef	BCMSUP_PSK
#define SUP_WPAPSK	2 /* Used for WPA-PSK */
#if defined(BCMSUP_LEAP)
#define SUP_LEAP_WPA	3 /* Used for LEAP-WPA */
#endif 
#endif /* BCMSUP_PSK */

extern bool wlc_set_sup(struct supplicant *sup, int type,
	/* parameters used only for PSK follow */
	uint8 *sup_ies, uint sup_ies_len, uint8 *auth_ies, uint auth_ies_len,
	int btamp_enabled);
extern void wlc_sup_set_ea(supplicant_t *sup, struct ether_addr *ea);
/* helper fn to find supplicant and authenticator ies from assocreq and prbresp */
extern void wlc_find_sup_auth_ies(wlc_info_t *wlc, uint8 **sup_ies,
	uint *sup_ies_len, uint8 **auth_ies, uint *auth_ies_len);

#ifdef	BCMSUP_PSK
#ifdef	BCMWPA2
extern void wlc_sup_clear_pmkid_store(struct supplicant *sup);
extern void wlc_sup_pmkid_cache_req(struct supplicant *sup);
#endif /* BCMWPA2 */

/* Install WPA PSK material in supplicant */
extern int wlc_sup_set_pmk(struct supplicant *sup, wsec_pmk_t *psk, bool assoc);

/* Send SSID to supplicant for PMK computation */
extern int wlc_sup_set_ssid(struct supplicant *sup, uchar ssid[], int ssid_len);

/* send deauthentication */
extern void wlc_wpa_senddeauth(wlc_bsscfg_t *bsscfg, char *da, int reason);

/* tell supplicant to send a MIC failure report */
extern void wlc_sup_send_micfailure(struct supplicant *sup, bool ismulti);
#endif	/* BCMSUP_PSK */

#if defined(BCMSUP_LEAP)
extern int wlc_set_leapauth(struct supplicant*sup, int auth);
#endif

#if defined(BCMSUP_LEAP)
extern void wlc_sup_leap_starttime(struct supplicant *sup, int leap_start);
extern int wlc_sup_get_leap_starttime(struct supplicant* sup);
extern void wlc_set_leap_state(struct supplicant *sup, bool state);
extern bool wlc_get_leap_state(struct supplicant *sup);
#endif 

#if	defined(BCMSUP_LEAP)
/* Return whether the given SSID matches on in the LEAP list. */
extern bool wlc_ccx_leap_ssid(struct supplicant *sup, uchar SSID[], int len);

/* Handle WLC_[GS]ET_LEAP_LIST ioctls */
extern int wlc_ccx_set_leap_list(struct supplicant *sup, void *pval);
extern int wlc_ccx_get_leap_list(struct supplicant *sup, void *pval);

/* Time-out  LEAP authentication and presume the AP is a rogue */
extern void wlc_ccx_rogue_timer(struct supplicant *sup, struct ether_addr *ap_mac);

/* Register a rogue AP report */
extern void wlc_ccx_rogueap_update(struct supplicant *sup, uint16 reason,
	struct ether_addr *ap_mac);

/* Return whether the supplicant state indicates successful authentication */
extern bool wlc_ccx_authenticated(struct supplicant *sup);

/* leap supplicant retry timer callback */
extern void wlc_leapsup_timer_callback(struct supplicant *sup);

extern void wlc_ccx_sup_init(struct supplicant *sup, int sup_type);
#endif 


#ifdef BCMSUP_PSK
/* Manage supplicant 4-way handshake timer */
extern uint32 wlc_sup_get_wpa_psk_tmo(struct supplicant *sup);
extern void wlc_sup_set_wpa_psk_tmo(struct supplicant *sup, uint32 tmo);
extern void wlc_sup_wpa_psk_timer(struct supplicant *sup, bool start);
#endif /* BCMSUP_PSK */

#if defined(BCMSUP_PSK) || defined(BCMSUP_LEAP)
/* Return the supplicant authentication status */
extern sup_auth_status_t wlc_sup_get_auth_status(struct supplicant *sup);

/* Return the extended supplicant authentication status */
extern sup_auth_status_t wlc_sup_get_auth_status_extended(struct supplicant *sup);

/* Send a supplicant status event */
extern void wlc_wpa_send_sup_status(struct supplicant *sup, uint reason);
#else
#define wlc_wpa_send_sup_status(sup, reason) { }
#endif 


#if defined(WOWL) && defined(BCMSUP_PSK)
extern uint16 aes_invsbox[];
extern uint16 aes_xtime9dbe[];
extern void *wlc_sup_hw_wowl_init(struct supplicant *sup);
extern void wlc_sup_sw_wowl_update(struct supplicant *sup);
#else
#define wlc_sup_hw_wowl_init(a) NULL
#define wlc_sup_sw_wowl_update(a) do { } while (0)
#endif /* defined(WOWL) && defined (BCMSUP_PSK) */

#endif	/* _wlc_sup_h_ */
