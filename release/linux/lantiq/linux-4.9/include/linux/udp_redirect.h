#ifndef _UDP_REDIRECT_H
#define _UDP_REDIRECT_H

/* udp_redirect.h: Hook to receive packets directly from the network stack.
 *
 * Copyright (C) 2011-2016  Lantiq Deutschland GmbH (www.lantiq.com)
 * Copyright (c) 2017 Intel Corporation
 */

/* ============================= */
/* Includes                      */
/* ============================= */
#ifndef _LINUX_TYPES_H
#include <linux/types.h>
#endif
#include <linux/skbuff.h>

/* ============================= */
/* Definitions                   */
/* ============================= */
#define UDP_REDIRECT_MAGIC (void *)0x55445052L

/* ============================= */
/* Global variable declaration   */
/* ============================= */
extern int (*udp_do_redirect_fn)(struct sock *sk, struct sk_buff *skb);

#endif
