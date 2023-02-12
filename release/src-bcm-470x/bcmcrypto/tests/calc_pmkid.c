/*
 * calc_pmkid.c  - calculate PMKID - w/ sha1 and kdf
 * Broadcom Proprietary and Confidential. Copyright (C) 2016,
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom.
 *
 * $Id: $
 */

#include <typedefs.h>

#include <bcmutils.h>
#include <bcmendian.h>
#include <proto/802.11.h>
#include <bcmcrypto/prf.h>
#include <bcmcrypto/sha1.h>
#include <bcmcrypto/sha256.h>
#include <bcmcrypto/hmac_sha256.h>

#define true  TRUE 
#define false FALSE

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct opts_t {
	uchar       key[32];
	uchar		bssid[6];
	uchar		macaddr[6];
	int			kdf; 
} opts_t;

void usage(const char* p)
{
	fprintf(stderr, "usage: %s <args>\n", p);
	fprintf(stderr, "	-key <hex>\n");
	fprintf(stderr, "	-bssid <hex>\n");
	fprintf(stderr, "	-macaddr <hex>\n");
	fprintf(stderr, "	-kdf [0|1]\n");
}

unsigned h2i(int c)
{
        if (c >= 'A' && c <= 'F')
                return c - 'A' + 10;
        else if (c >= 'a' && c <= 'f')
                return c - 'a' + 10;
        else if (c >= '0' && c <= '9')
                return c - '0';
        else
                return 0;
}

void hex2bin(const char* in, uint isz, uchar* out, uint olim, uint* osz)
{
	*osz = 0;
	if (!in || !olim)
		return;

	int ch_hi;
	while (isz && olim) {
		int ch_lo;

		/* consume the first hex digit */
		ch_hi	= *in++; isz--;
		if (!ch_hi) break;
		if (!isxdigit(ch_hi)) continue;

		/* next digit */
		if (!isz) break;
		ch_lo = *in++; isz--;
		while (ch_lo && !isxdigit(ch_lo)) {
			ch_lo = *in++; isz--;
			if (!isz) break;
		}
		if (!ch_lo) break;
		*out++ =  h2i(ch_hi) << 4 | h2i(ch_lo); (*osz)++; olim--;
	}
}

bool output(int ofd, uchar* out, size_t osz)
{
	if (!out)
		return true;

	uint nw=0;
	while (osz--) {
		if (nw %16 == 0) {
			char lbuf[16];
			if (nw)
				write(ofd, "\n", 1);
			sprintf(lbuf, " %04d: ", nw);
			write(ofd, lbuf, strlen(lbuf));
		}
			
		char hbuf[4];
		sprintf(hbuf, "%02x ", *out++);
		write(ofd, hbuf, 3);
		nw++;
	}
	write(ofd, "\n", 1);
	return true;
}

bool parse_opts(opts_t* opts, int argc, char** argv)
{
	int i;
	uint ign;

	memset(opts, 0, sizeof(opts_t));
	for (i = 1; i < argc; i += 2) {
		const char* arg = argv[i];
		const char* val = 0;
		if (i+1 >= argc)
			return false;
		val = argv[i+1];

		if (!strncasecmp(arg, "-key", 4)) {
			hex2bin(val, strlen(val), opts->key, sizeof(opts->key), &ign);
		} else if (!strncasecmp(arg, "-bssid", 3)) {
			hex2bin(val, strlen(val), opts->bssid, sizeof(opts->bssid), &ign);
		} else if (!strncasecmp(arg, "-macaddr", 3)) {
			hex2bin(val, strlen(val), opts->macaddr, sizeof(opts->macaddr), &ign);
		} else if (!strncasecmp(arg, "-kdf", 4)) {
			opts->kdf = atoi(val);
		} else {
			fprintf(stderr, "Invalid option specified\n");
			return false;
		}
	}

	return true;
}

#define PMKID_SIZE 16
int
main(int argc, char **argv)
{
	int ret = -1;
	opts_t s_opts, *opts=&s_opts;
	uchar pmkid[MAX(SHA1HashSize, SHA256_DIGEST_LENGTH) /* note size of the digest */ ];
	const char prefix[] = "PMK Name";
	uchar inbuf[sizeof(prefix) + (2 * 6)];
	int off;

	if (!parse_opts(opts, argc, argv)) {
		usage(argv[0]);
		goto cleanup;
	}

	off = 0;
	memcpy(&inbuf[off], prefix, sizeof(prefix) - 1);
	off += sizeof(prefix) - 1;
	memcpy(&inbuf[off], opts->bssid, sizeof(opts->bssid));
	off += sizeof(opts->bssid);
	memcpy(&inbuf[off], opts->macaddr, sizeof(opts->macaddr));
	off += sizeof(opts->macaddr);

	if (opts->kdf)
		hmac_sha256(inbuf, off, opts->key, sizeof(opts->key), pmkid, NULL);
	else
		hmac_sha1(inbuf, off, opts->key, sizeof(opts->key), pmkid);

	write(1, (uchar*)"pmk:\n", sizeof("pmk: "));
	(void)output(1, opts->key, sizeof(opts->key));

	write(1, (uchar*)"bssid:\n", sizeof("bssid: "));
	(void)output(1, opts->bssid, sizeof(opts->bssid));

	write(1, (uchar*)"macaddr:\n", sizeof("macaddr: "));
	(void)output(1, opts->macaddr, sizeof(opts->macaddr));

	write(1, (uchar*)"PMKID:\n", sizeof("PMKID: "));
	(void)output(1, pmkid, PMKID_SIZE);

	ret = 0;

cleanup:
	return ret;
}
