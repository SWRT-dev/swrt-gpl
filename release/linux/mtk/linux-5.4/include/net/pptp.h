/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _NET_PPTP_H
#define _NET_PPTP_H

#include <net/gre.h>

#define PPP_LCP_ECHOREQ 0x09
#define PPP_LCP_ECHOREP 0x0A
#define SC_RCV_BITS     (SC_RCV_B7_1|SC_RCV_B7_0|SC_RCV_ODDP|SC_RCV_EVNP)

#define MISSING_WINDOW 20
#define WRAPPED(curseq, lastseq)\
	((((curseq) & 0xffffff00) == 0) &&\
	(((lastseq) & 0xffffff00) == 0xffffff00))

#define PPTP_HEADER_OVERHEAD (2+sizeof(struct pptp_gre_header))
struct pptp_gre_header {
	struct gre_base_hdr gre_hd;
	__be16 payload_len;
	__be16 call_id;
	__be32 seq;
	__be32 ack;
} __packed;

/* symbol exported from linux kernel driver/net/ppp/pptp.c */
extern int (*mtk_pptp_seq_next)(uint16_t call_id, uint32_t *val);

#endif
