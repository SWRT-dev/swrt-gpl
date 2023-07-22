/*
 * local defines for wpapsk supplicant and authenticator
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: wlc_wpa.h,v 1.3 2009-12-04 01:45:51 $
 */

#ifndef _wlc_wpa_h_
#define _wlc_wpa_h_

#include <bcmcrypto/passhash.h>

/* WPA key_info flag sets */
#define PMSG1_REQUIRED	  (WPA_KEY_PAIRWISE | WPA_KEY_ACK)
#define PMSG1_PROHIBITED  (WPA_KEY_SECURE | WPA_KEY_MIC | WPA_KEY_INSTALL)
#define PMSG2_REQUIRED	  (WPA_KEY_PAIRWISE | WPA_KEY_MIC)
#define PMSG2_PROHIBITED  (WPA_KEY_ACK | WPA_KEY_INDEX_MASK)
#define PMSG3_REQUIRED	  (WPA_KEY_PAIRWISE | WPA_KEY_MIC | WPA_KEY_ACK | WPA_KEY_INSTALL)
#define PMSG3_BRCM_REQUIRED	(PMSG3_REQUIRED | WPA_KEY_SECURE)
#define PMSG3_WPA2_REQUIRED	(PMSG3_REQUIRED | WPA_KEY_SECURE | WPA_KEY_ENCRYPTED_DATA)
#define PMSG3_PROHIBITED  (WPA_KEY_SECURE)
#define PMSG4_REQUIRED	  (WPA_KEY_MIC)
#define PMSG4_PROHIBITED  (WPA_KEY_ACK | WPA_KEY_INDEX_MASK)
#define GMSG1_REQUIRED	  (WPA_KEY_SECURE | WPA_KEY_MIC | WPA_KEY_ACK)
#define GMSG2_REQUIRED	  (WPA_KEY_MIC | WPA_KEY_SECURE)
#define MIC_ERROR_REQUIRED (WPA_KEY_MIC | WPA_KEY_ERROR | WPA_KEY_REQ)

/* Spec says some key_info flags in supplicant response should match what
 * authenticator had in previous message.  Define masks to copy those.
 */
#define PMSG2_MATCH_FLAGS (WPA_KEY_DESC_V1 | WPA_KEY_DESC_V2 |		\
			   WPA_KEY_PAIRWISE | WPA_KEY_INDEX_MASK |	\
			   WPA_KEY_SECURE | WPA_KEY_ERROR | WPA_KEY_REQ)
#define PMSG4_MATCH_FLAGS (WPA_KEY_DESC_V1 | WPA_KEY_DESC_V2 |		\
			   WPA_KEY_PAIRWISE | WPA_KEY_SECURE)
#define GMSG2_MATCH_FLAGS (WPA_KEY_DESC_V1 | WPA_KEY_DESC_V2 | WPA_KEY_PAIRWISE)

typedef enum {
	/* Supplicant States */
	WPA_SUP_DISCONNECTED,
	WPA_SUP_INITIALIZE,
	WPA_SUP_AUTHENTICATION,
	WPA_SUP_STAKEYSTARTP_WAIT_M1 = WPA_SUP_AUTHENTICATION,
	                                /* 4-way handshake: waiting for msg M1 */
	WPA_SUP_STAKEYSTARTP_PREP_M2,	/* 4-way handshake: preparing to send M2 */
	WPA_SUP_STAKEYSTARTP_WAIT_M3,	/* 4-way handshake: waiting for M3 */
	WPA_SUP_STAKEYSTARTP_PREP_M4,	/* 4-way handshake: preparing to send M4 */
	WPA_SUP_STAKEYSTARTG_WAIT_G1,	/* group handshake: waiting for G1 */
	WPA_SUP_STAKEYSTARTG_PREP_G2,	/* group handshake: preparing to send G2 */
	WPA_SUP_KEYUPDATE,		/* handshake complete, keys updated */

	/* Authenticator States */
	WPA_AUTH_INITIALIZE,
	WPA_AUTH_PTKSTART,
	WPA_AUTH_PTKINITNEGOTIATING,
	WPA_AUTH_PTKINITDONE,
	/* for WPA1 group key state machine */
	WPA_AUTH_REKEYNEGOTIATING,
	WPA_AUTH_KEYERROR,
	WPA_AUTH_REKEYESTABLISHED,
	WPA_AUTH_KEYUPDATE		/* handshake complete, keys updated */
} wpapsk_state_t;

typedef struct {
	wpapsk_state_t state;	/* state of WPA PSK key msg exchanges */
	uint16 auth_wpaie_len;	/* length of authenticator's WPA info element */
	uint16 sup_wpaie_len;	/* length of supplicant's WPA info element */
	uchar *auth_wpaie;	/* authenticator's WPA info element */
	uchar *sup_wpaie;	/* supplicant's WPA info element */
	ushort ucipher;		/* negotiated unicast cipher */
	ushort mcipher;		/* negotiated multicast cipher */
	ushort ptk_len;		/* PTK len, used in PRF calculation */
	ushort gtk_len;         /* Group (mcast) key length */
	ushort tk_len;		/* TK len, used when loading key into driver */
	ushort desc;		/* key descriptor type */
	uint8 anonce[EAPOL_WPA_KEY_NONCE_LEN];	/* AP's nonce */
	uint8 snonce[EAPOL_WPA_KEY_NONCE_LEN];	/* STA's nonce */
	uint8 replay[EAPOL_KEY_REPLAY_LEN];	/* AP's replay counter */
	uint8 last_replay[EAPOL_KEY_REPLAY_LEN]; /* AP's last replay counter (for WOWL) */
	uint8 gtk[TKIP_KEY_SIZE];               /* group transient key */

	/* fields of WPA key hierarchy (together forming the PTK) */
	uint8 eapol_mic_key[WPA_MIC_KEY_LEN];
	uint8 eapol_encr_key[WPA_ENCR_KEY_LEN];
	uint8 temp_encr_key[WPA_TEMP_ENCR_KEY_LEN];
	uint8 temp_tx_key[WPA_TEMP_TX_KEY_LEN];
	uint8 temp_rx_key[WPA_TEMP_RX_KEY_LEN];
#if defined(BCMROMBUILD)
	uint32 rn;	/* reassociation request number (refreshed per session key) */
	/* fields of CCKM key hierarchy */
	uint8 key_refresh_key[CCKM_KRK_LEN];
	uint8 base_transient_key[CCKM_BTK_LEN];
#endif 
	uint8 gtk_index;
	uint32 WPA_auth;
#ifdef BCMAUTH_PSK
	uint8 retries;		/* retry count */
#endif /* BCMAUTH_PSK */
} wpapsk_t;

/* persistent WPA stuff (survives set_sup initialization ) */
typedef struct wpapsk_info {
	wlc_info_t *wlc;		/* pointer to main wlc structure */
	ushort psk_len;			/* len of pre-shared key */
	ushort pmk_len;			/* len of pairwise master key */
	uchar  psk[WSEC_MAX_PSK_LEN];	/* saved pre-shared key */
	uint8 pmk[PMK_LEN];		/* saved pairwise master key */
	/* break lengthy passhash() calculation into smaller chunks */
	struct wl_timer *passhash_timer; /* timer for passhash */
	passhash_t passhash_states;	/* states for passhash */
#ifdef BCMAUTH_PSK
	void *retry_timer;	/* auth retry timer */
#endif /* BCMAUTH_PSK */
} wpapsk_info_t;

typedef enum {
	PMSG1, PMSG2, PMSG3, PMSG4, GMSG1, GMSG2, MIC_FAILURE
} wpa_msg_t;

extern void wlc_wpapsk_free(wlc_info_t *wlc, wpapsk_t *wpa);
extern bool wlc_wpapsk_start(osl_t *osh, int WPA_auth, wpapsk_t *wpa, uint8 *sup_ies,
	uint sup_ies_len, uint8 *auth_ies, uint auth_ies_len);
extern int wlc_wpa_cobble_pmk(wpapsk_info_t *info, char *psk, size_t psk_len,
	uchar *ssid, uint ssid_len);
extern bool wlc_wpa_set_ucipher(wpapsk_t *wpa, ushort ucipher, bool wep_ok);
extern int wlc_wpa_set_pmk(wlc_bsscfg_t *bsscfg, wpapsk_info_t *info,
	wpapsk_t *wpa, wsec_pmk_t *pmk, bool assoc);
extern void wlc_wpa_plumb_tk(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, uint8 *tk,
	uint32 tk_len, uint32 cipher, struct ether_addr *ea);
extern void *wlc_eapol_pktget(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg,
	struct ether_addr *da, uint len);
extern void
wlc_wpa_plumb_gtk(wlc_info_t *wlc, wlc_bsscfg_t *bsscfg, uint8 *gtk, uint32 gtk_len,
	uint32 key_index, uint32 cipher, uint8 *rsc, bool primary_key);

typedef struct _sup_pmkid {
	struct ether_addr	BSSID;
	uint8			PMKID[WPA2_PMKID_LEN];
	uint8			PMK[PMK_LEN];
	bool			opportunistic;
} sup_pmkid_t;

#define SUP_MAXPMKID	16 /* Supplementary Max PMK ID */

#endif	/* _wlc_wpa_h_ */
