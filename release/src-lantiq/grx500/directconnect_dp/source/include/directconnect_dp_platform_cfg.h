/*
 * DirectConnect provides a common interface for the network devices to achieve the full or partial
   acceleration services from the underlying packet acceleration engine
 * Copyright (c) 2017-2020, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef _DIRECTCONNECT_DP_PLATFORM_CFG_H_
#define _DIRECTCONNECT_DP_PLATFORM_CFG_H_

#if defined(CONFIG_X86_INTEL_LGM)
  /* LGM */
  #define DC_DP_CFG_MAX_TXIN_RING_SZ        0x20   /* 32 */
  #define DC_DP_CFG_MAX_TXOUT_RING_SZ       0x20   /* 32 */
  #define DC_DP_CFG_MAX_RXIN_RING_SZ        0x20   /* 32 */
  #define DC_DP_CFG_MAX_RXOUT_RING_SZ       0x20   /* 32 */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_CDB 0xA000 /* 40K */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_CDB 0x8800 /* 34K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_24G 0xA000 /* 40K */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_24G 0x8800 /* 34K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_5G  0xA000 /* 40K */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_5G  0x8800 /* 34K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_6G  0xA000 /* 40K */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_6G  0x8800 /* 34K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_500     0xA000 /* 40K */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_500     0x8800 /* 34K */

#elif defined(CONFIG_X86_PUMA7)
  /* CgR/PUMA/GRX750 */
  #define DC_DP_CFG_MAX_TXIN_RING_SZ        0x1000 /* 4K */
  #define DC_DP_CFG_MAX_TXOUT_RING_SZ       0x1000 /* 4K */
  #define DC_DP_CFG_MAX_RXOUT_RING_SZ       0x1000 /* 4K */
#ifdef CONFIG_HIGH_PPS
  #define DC_DP_CFG_MAX_RXIN_RING_SZ        0x400  /* 1K */
#else
  #define DC_DP_CFG_MAX_RXIN_RING_SZ        0x1000 /* 4K */
#endif
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_CDB 0xC000 /* 48K */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_CDB 0x8800 /* 34K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_24G 0xc00  /* 3K  */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_24G 0x1000 /* 4k  */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_5G  0x1400 /* 5K  */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_5G  0x2000 /* 8K  */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_6G  0x2000 /* 8K  */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_6G  0x5000 /* 20K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_500     0xC000 /* 48K */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_500     0x8800 /* 34K */

#elif defined(CONFIG_PRX300_CQM)
  /* PRX */
  #define DC_DP_CFG_MAX_TXIN_RING_SZ        0x20   /* 32 */
  #define DC_DP_CFG_MAX_TXOUT_RING_SZ       0x20   /* 32 */
  #define DC_DP_CFG_MAX_RXOUT_RING_SZ       0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_RXIN_RING_SZ        DC_DP_CFG_MAX_RXOUT_RING_SZ
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_CDB 0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_CDB 0x8800 /* 34K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_24G 0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_24G 0x8800 /* 34K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_5G  0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_5G  0x8800 /* 34K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_6G  0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_6G  0x8800 /* 34K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_500     0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_500     0x8800 /* 34K */

#elif defined(CONFIG_SOC_GRX500)
  /* GRX500 */
  #define DC_DP_CFG_MAX_TXIN_RING_SZ        0x20   /* 32 */
  #define DC_DP_CFG_MAX_TXOUT_RING_SZ       0x20   /* 32 */
  #define DC_DP_CFG_MAX_RXOUT_RING_SZ       0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_RXIN_RING_SZ        DC_DP_CFG_MAX_RXOUT_RING_SZ
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_CDB 0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_CDB 0x1000 /* 4K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_24G 0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_24G 0x1000 /* 4K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_5G  0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_5G  0x1000 /* 4K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_600_6G  0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_600_6G  0x1000 /* 4K */
  #define DC_DP_CFG_MAX_RX_BUFS_WAV_500     0x0FFF /* 4K - 1 */
  #define DC_DP_CFG_MAX_TX_BUFS_WAV_500     0x1000 /* 4K */

#else
  #error "Not supported"
#endif

#endif /* _DIRECTCONNECT_DP_PLATFORM_CFG_H_ */
