/* udp_redirect_symb.c: Hook to receive packets directly from the network stack.
 * Copyright (C) 2011-2016  Lantiq Deutschland GmbH (www.lantiq.com)
 * Copyright (c) 2017 Intel Corporation
 */

/* ============================= */
/* Includes                      */
/* ============================= */
#include <linux/module.h>
#include <linux/udp_redirect.h>

/* ============================= */
/* Global variable definition    */
/* ============================= */
int (*udp_do_redirect_fn)(struct sock *sk, struct sk_buff *skb) = NULL;

/* ============================= */
/* Global function definition    */
/* ============================= */

EXPORT_SYMBOL(udp_do_redirect_fn);
