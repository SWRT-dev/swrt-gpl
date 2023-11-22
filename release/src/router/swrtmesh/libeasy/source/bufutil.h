/*
 * bufutil.h - utility functions for unaligned buffer access.
 *
 * Copyright (C) 2021 IOPSYS Software Solutions AB. All rights reserved.
 *
 * Author: anjan.chanda@iopsys.eu
 *
 * See LICENSE file for license related information.
 *
 */


#ifndef BUFUTIL_H
#define BUFUTIL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* get/put assuming big-endian formatted buffer */

static inline uint16_t buf_get_be16(const uint8_t *buf)
{
	return (buf[0] << 8) | buf[1];
}

#define BUF_GET_BE16(b)		buf_get_be16((uint8_t *)&(b))

static inline uint32_t buf_get_be24(const uint8_t *buf)
{
        return (buf[0] << 16) | (buf[1] << 8) | buf[2];
}

#define BUF_GET_BE24(b)         buf_get_be24((uint8_t *)&(b))


static inline uint32_t buf_get_be32(const uint8_t *buf)
{
	return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

#define BUF_GET_BE32(b)		buf_get_be32((uint8_t *)&(b))


static inline uint64_t buf_get_be64(const uint8_t *buf)
{
	return (((uint64_t)buf[0] << 56) | ((uint64_t)buf[1] << 48) |
			((uint64_t)buf[2] << 40) | ((uint64_t)buf[3] << 32) |
			((uint64_t)buf[4] << 24) | ((uint64_t)buf[5] << 16) |
			((uint64_t)buf[6] << 8) | (uint64_t)buf[7]);
}

#define BUF_GET_BE64(b)	buf_get_be64((uint8_t *)&(b))


static inline void buf_put_be16(uint8_t *buf, uint16_t val)
{
	buf[0] = val >> 8;
	buf[1] = val & 0xff;
}

#define BUF_PUT_BE16(b, v)	buf_put_be16((uint8_t *)&(b), v)


static inline void buf_put_be24(uint8_t *buf, uint32_t val)
{
	buf[0] = (val >> 16) & 0xff;
	buf[1] = (val >> 8) & 0xff;
	buf[2] = val & 0xff;
}

#define BUF_PUT_BE24(b, v)	buf_put_be24((uint8_t *)&(b), v)

static inline void buf_put_be32(uint8_t *buf, uint32_t val)
{
	buf[0] = (val >> 24) & 0xff;
	buf[1] = (val >> 16) & 0xff;
	buf[2] = (val >> 8) & 0xff;
	buf[3] = val & 0xff;
}

#define BUF_PUT_BE32(b, v)	buf_put_be32((uint8_t *)&(b), v)


static inline void buf_put_be64(uint8_t *buf, uint64_t val)
{
	buf[0] = (val >> 56) & 0xff;
	buf[1] = (val >> 48) & 0xff;
	buf[2] = (val >> 40) & 0xff;
	buf[3] = (val >> 32) & 0xff;
	buf[4] = (val >> 24) & 0xff;
	buf[5] = (val >> 16) & 0xff;
	buf[6] = (val >> 8) & 0xff;
	buf[7] = val & 0xff;
}

#define BUF_PUT_BE64(b, v)	buf_put_be64((uint8_t *)&(b), v)


/* get/put assuming little-endian formatted buffer */

static inline uint16_t buf_get_le16(const uint8_t *buf)
{
	return buf[0] | (buf[1] << 8);
}

#define BUF_GET_LE16(b)		buf_get_le16((uint8_t *)&(b))

static inline uint32_t buf_get_le32(const uint8_t *buf)
{
	return buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}

#define BUF_GET_LE32(b)		buf_get_le32((uint8_t *)&(b))

static inline void buf_put_le16(uint8_t *buf, uint16_t val)
{
	buf[0] = val & 0xff;
	buf[1] = (val >> 8) & 0xff;
}

#define BUF_PUT_LE16(b, v)	buf_put_le16((uint8_t *)&(b), v)

static inline void buf_put_le32(uint8_t *buf, uint32_t val)
{
	buf[0] = val & 0xff;
	buf[1] = (val >> 8) & 0xff;
	buf[2] = (val >> 16) & 0xff;
	buf[3] = (val >> 24) & 0xff;
}

#define BUF_PUT_LE32(b, v)	buf_put_le32((uint8_t *)&(b), v)

#define bufptr_put_u8(b, v)		\
do {					\
	*b = (uint8_t)v;		\
	b += 1;				\
} while(0)


#define bufptr_put_be16(_b, v)		\
do {					\
	buf_put_be16(_b, v);		\
	_b += 2;			\
} while(0)


#define bufptr_put_le16(_b, v)		\
do {					\
	buf_put_le16(_b, v);		\
	_b += 2;			\
} while(0)


#define bufptr_put_be32(_b, v)		\
do {					\
	buf_put_be32(_b, v);		\
	_b += 4;			\
} while(0)


#define bufptr_put_le32(_b, v)		\
do {					\
	buf_put_le32(_b, v);		\
	_b += 4;			\
} while(0)


#define bufptr_put(b, v, s)		\
do {					\
	memcpy(b, v, s);		\
	b += s;				\
} while(0)


#define bufptr_get(b, v, s)		\
do {					\
	memcpy(v, b, s);		\
	b += s;				\
} while(0)


#if 0
int test_bufutil()
{
	uint8_t buffer[] = "\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc";
	uint16_t v = 0x5060;


	printf("le = 0x%04x\n", buf_get_le16(buffer));
	printf("be = 0x%04x\n", buf_get_be16(buffer));

	buf_put_be16(buffer, v);
	printf("be = 0x %02x-%02x\n", buffer[0] & 0xff, buffer[1] & 0xff);

	buf_put_le16(buffer, v);
	printf("be = 0x %02x-%02x\n", buffer[0] & 0xff, buffer[1] & 0xff);
}
#endif

#ifdef __cplusplus
}
#endif

#endif /* BUFUTIL_H */
