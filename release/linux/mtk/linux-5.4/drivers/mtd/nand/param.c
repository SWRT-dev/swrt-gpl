/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2023 - Mediatek
 *
 * Author: SkyLake <SkyLake.Huang@mediatek.com>
 */

#include <linux/mtd/param.h>

u16 nanddev_crc16(u16 crc, u8 const *p, size_t len)
{
	int i;

	while (len--) {
		crc ^= *p++ << 8;
		for (i = 0; i < 8; i++)
			crc = (crc << 1) ^ ((crc & 0x8000) ? 0x8005 : 0);
	}

	return crc;
}

/*
 * Recover data with bit-wise majority
 */
void nanddev_bit_wise_majority(const void **srcbufs,
				   unsigned int nsrcbufs,
				   void *dstbuf,
				   unsigned int bufsize)
{
	int i, j, k;

	for (i = 0; i < bufsize; i++) {
		u8 val = 0;

		for (j = 0; j < 8; j++) {
			unsigned int cnt = 0;

			for (k = 0; k < nsrcbufs; k++) {
				const u8 *srcbuf = srcbufs[k];

				if (srcbuf[i] & BIT(j))
					cnt++;
			}

			if (cnt > nsrcbufs / 2)
				val |= BIT(j);
		}

		((u8 *)dstbuf)[i] = val;
	}
}
