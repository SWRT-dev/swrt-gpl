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
 * $Id: bcm_linux_lbuf.h,v 1.4 2009-04-13 16:03:35 $
 */

#ifndef _bcm_linux_lbuf_h
#define	_bcm_linux_lbuf_h

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

#endif	/* _bcm_linux_lbuf_h */
