/*
 * HND RTE packet buffer definitions.
 *
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: bcm_lbuf.h,v 1.3 2010-02-23 17:43:13 $
 */

#ifndef _bcm_lbuf_h
#define	_bcm_lbuf_h

#if defined(OSL_EXT)

#include "typedefs.h"
#include "bcmdefs.h"
#include "osl.h"
#include "pkt_lbuf.h"
#include "lbuf.h"

/* This should be a multiple of 4 to avoid mis-aligned memory accesses. */
#define TXOFF	172

#else

#include <bcmdefs.h>

#define OSL_PKTTAG_SZ 32



struct lbuf {
	struct lbuf	*next;		/* next lbuf in a chain of lbufs forming one packet */
	struct lbuf	*link;		/* first lbuf of next packet in a list of packets */
	uchar		*head;		/* fixed start of buffer */
	uchar		*end;		/* fixed end of buffer */
	uchar		*data;		/* variable start of data */
	uint16		len;		/* nbytes of data */
	uint16		flags;		/* private flags; don't touch */
	uint16		dmapad;		/* padding to be added for tx dma */
	uint32		pkttag[(OSL_PKTTAG_SZ + 3) / 4];  /* 4-byte-aligned packet tag area */
};

#define	LBUFSZ		sizeof(struct lbuf)

/* Total maximum packet buffer size including lbuf header */
#define MAXPKTBUFSZ	1920		/* enough to fit a 1500 MTU plus overhead */

/* private flags - don't reference directly */
#define	LBF_PRI		0x0007		/* priority (low 3 bits of flags) */
#define LBF_SUM_NEEDED	0x0008
#define LBF_SUM_GOOD	0x0010

#define LBF_MSGTRACE	0x0020
#define	LBP(lb)		((struct lbuf *)(lb))

/* prototypes */
extern void lb_init(void);
#if defined(BCMDBG_MEMFAIL)
extern struct lbuf *lb_alloc(uint size, char *file, int line);
#else
extern struct lbuf *lb_alloc(uint size);
#endif 

extern struct lbuf *lb_dup(struct lbuf *lb);
extern void lb_free(struct lbuf *lb);
extern bool lb_sane(struct lbuf *lb);

static INLINE uchar *
lb_push(struct lbuf *lb, uint len)
{
	ASSERT(lb_sane(lb));
	ASSERT((lb->data - len) >= lb->head);
	lb->data -= len;
	lb->len += len;
	return (lb->data);
}

static INLINE uchar *
lb_pull(struct lbuf *lb, uint len)
{
	ASSERT(lb_sane(lb));
	ASSERT(len <= lb->len);
	lb->data += len;
	lb->len -= len;
	return (lb->data);
}

static INLINE void
lb_setlen(struct lbuf *lb, uint len)
{
	ASSERT(lb_sane(lb));
	ASSERT(lb->data + len <= lb->end);
	lb->len = len;
}

static INLINE uint
lb_pri(struct lbuf *lb)
{
	ASSERT(lb_sane(lb));
	return (lb->flags & LBF_PRI);
}

static INLINE void
lb_setpri(struct lbuf *lb, uint pri)
{
	ASSERT(lb_sane(lb));
	ASSERT((pri & LBF_PRI) == pri);
	lb->flags = (lb->flags & ~LBF_PRI) | (pri & LBF_PRI);
}

static INLINE bool
lb_sumneeded(struct lbuf *lb)
{
	ASSERT(lb_sane(lb));
	return ((lb->flags & LBF_SUM_NEEDED) != 0);
}

static INLINE void
lb_setsumneeded(struct lbuf *lb, bool summed)
{
	ASSERT(lb_sane(lb));
	if (summed)
		lb->flags |= LBF_SUM_NEEDED;
	else
		lb->flags &= ~LBF_SUM_NEEDED;
}

static INLINE bool
lb_sumgood(struct lbuf *lb)
{
	ASSERT(lb_sane(lb));
	return ((lb->flags & LBF_SUM_GOOD) != 0);
}

static INLINE void
lb_setsumgood(struct lbuf *lb, bool summed)
{
	ASSERT(lb_sane(lb));
	if (summed)
		lb->flags |= LBF_SUM_GOOD;
	else
		lb->flags &= ~LBF_SUM_GOOD;
}

static INLINE bool
lb_msgtrace(struct lbuf *lb)
{
	ASSERT(lb_sane(lb));
	return ((lb->flags & LBF_MSGTRACE) != 0);
}

static INLINE void
lb_setmsgtrace(struct lbuf *lb, bool set)
{
	ASSERT(lb_sane(lb));
	if (set)
		lb->flags |= LBF_MSGTRACE;
	else
		lb->flags &= ~LBF_MSGTRACE;
}


/* Common to all ports */
/* dummy decl */
struct osl_info {
	uint pad;
};
typedef struct osl_info osl_t;



/* Absolutely minimalist pktbuffer utilities */
#define TXOFF	170

#if !defined(BCM_OSL)
/* packet primitives */
#define PKTGET(osh, len, send)		(void *)osl_pktget((osh),(len))
#define PKTFREE(osh, lb, send)		osl_pktfree((osh), (lb), (send))
#define	PKTDATA(osh, lb)		LBP(lb)->data
#define	PKTLEN(osh, lb)			LBP(lb)->len
#define	PKTSETLEN(osh, lb, len)		lb_setlen(LBP(lb), (len))
#define	PKTPUSH(osh, lb, bytes)		lb_push(LBP(lb), (bytes))
#define	PKTPULL(osh, lb, bytes)		lb_pull(LBP(lb), (bytes))
#define	PKTTAG(lb)			((void *)((LBP(lb))->pkttag))
#endif /* !BCM_OSL */


extern void *osl_pktget(osl_t *osh, uint size);
extern void osl_pktfree(osl_t *osh, void *lb, bool send);
extern int osl_init(void);

#endif   /* defined(OSL_EXT) */
#endif	/* _bcm_lbuf_h */
