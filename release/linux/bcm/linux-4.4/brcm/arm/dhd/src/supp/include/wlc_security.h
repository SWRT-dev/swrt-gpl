/*
 * Michael Messge Integrity Check (MIC) algorithm and other security
 * interface definitions
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: wlc_security.h,v 1.2 2009-10-07 21:14:02 $
 */

#ifndef _wlc_security_h_
#define _wlc_security_h_

#ifndef LINUX_CRYPTO
#include <bcmcrypto/rc4.h>
#include <bcmcrypto/tkhash.h>
#endif

#include <bcmcrypto/bcmccx.h>

typedef struct wsec_tkip_info {
	bool	TKIP;		/* enabled/disabled flag */
#ifdef NOTYET
	uint8	mic_key[8];	/* key for MIC */
#else
	uint32	mic_key0;	/* key for MIC */
	uint32	mic_key1;	/* key for MIC */
#endif /* NOTYET */
} wsec_tkip_info_t;

struct wlc_frminfo;

/* Check if the received frame passes security filter */
#ifdef WLWSEC
extern bool wlc_wsec_recvdata(wlc_info_t *wlc, osl_t *osh, struct scb *scb,
	struct wlc_frminfo *frminfo, uint8 prio);

#ifndef LINUX_CRYPTO
extern bool wlc_wsec_sw_encrypt_data(wlc_info_t *wlc, osl_t *osh, void *p,
	wlc_bsscfg_t *cfg, struct scb *scb, wsec_key_t *key);
#endif

extern void wlc_wsec_rxiv_update(wlc_info_t *wlc, struct wlc_frminfo *f);


extern bool wlc_wsec_miccheck(wlc_info_t *wlc, osl_t *osh, struct scb *scb,
	struct wlc_frminfo *f);

#ifndef LINUX_CRYPTO
/* cobble TKIP MIC for an out-bound frag */
extern void wlc_dofrag_tkip(wlc_pub_t *wlp, void *p, uint frag, uint nfrags, osl_t *osh,
	struct wlc_bsscfg *cfg, struct scb *scb, struct ether_header *eh,
	wsec_key_t *key, uint8 prio, uint frag_length);
#endif
/* compute length of tkip fragment
 * flen_hdr is frag_length + ETHER_HDR_LEN
 */
extern uint wlc_wsec_tkip_nextfrag_len(wlc_pub_t *wlp, osl_t *osh,
	void *pkt_curr, void *pkt_next, uint flen_hdr);

extern int wlc_wsec_tx_tkmic_offset(wlc_pub_t *wlp, wlc_bsscfg_t *cfg, struct scb *scb);
extern int wlc_wsec_rx_tkmic_offset(wlc_pub_t *wlp, wlc_bsscfg_t *cfg, struct scb *scb);

#else
	#define wlc_wsec_recvdata(a, b, c, d, e) (FALSE)
	#define wlc_wsec_sw_encrypt_data(a, b, c, d, e, f) (FALSE)
	#define wlc_wsec_rxiv_update(a, b) do {} while (0)
	#define wlc_wsec_miccheck(a, b, c, d) (FALSE)
	#define wlc_dofrag_tkip(a, b, c, d, e, f, g, h, i, j, k) do {} while (0)
	#define wlc_wsec_tkip_nextfrag_len(a, b, c, d, e) (0)
	#define wlc_wsec_tx_tkmic_offset(a, b, c) (0)
	#define wlc_wsec_rx_tkmic_offset(a, b, c) (0)
#endif   /* WL_WSEC */


/* 802.1X LLC header */
extern const uint8 wlc_802_1x_hdr[];



#ifdef WSEC_TEST
void wlc_wsec_tkip_runst(uint32 k0, uint32 k1, uint8 *mref, uint32 cref0, uint32 cref1);
int wlc_wsec_tkip_runtest_mic(void);
int wlc_wsec_tkip_runtest(void);
#endif /* WSEC_TEST */

#endif /* _wlc_security_h_ */
