/*
 * util.c - includes utility functions.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include "util.h"

void get_random_bytes(int num, uint8_t *buf)
{
	unsigned int seed;
	struct timespec res = {0};
	int i;

	clock_gettime(CLOCK_REALTIME, &res);

	seed = res.tv_nsec;

	srand(seed);
	for (i = 0; i < num; i++)
		buf[i] = rand_r(&seed) & 0xff;
}


void _bufprintf(const char *label, uint8_t *buf, int len)
{
	int rows, residue;
	int i;
	int k;

	if (label)
		fprintf(stderr, "---- %s ----\n", label);

	rows = len / 16;

	for (k = 0; k < rows; k++) {
		fprintf(stderr, "\n   0x%08x | ", k * 16);

		for (i = 0; i < 16; i++) {
			if (!(i % 4))
				fprintf(stderr, "  ");

			fprintf(stderr, "%02x ", buf[k*16 + i] & 0xff);
		}

		fprintf(stderr, "%8c", ' ');
		for (i = 0; i < 16; i++) {
			fprintf(stderr, "%c ",
				isalnum(buf[k*16 + i] & 0xff) ? buf[k*16 + i] : '.');
		}
	}

	residue = len % 16;
	k = len - len % 16;

	if (residue) {
		fprintf(stderr, "\n   0x%08x | ", rows * 16);
		for (i = k; i < len; i++) {
			if (!(i % 4))
				fprintf(stderr, "  ");

			fprintf(stderr, "%02x ", buf[i] & 0xff);
		}

		for (i = residue; i < 16; i++) {
			if (!(i % 4))
				fprintf(stderr, "  ");

			fprintf(stderr, "%s ", "  ");
		}

		fprintf(stderr, "%8c", ' ');
		for (i = k; i < len; i++) {
			fprintf(stderr, "%c ",
				isalnum(buf[i] & 0xff) ? buf[i] : '.');
		}

	}

	if (label)
		fprintf(stderr, "\n--------------\n");
}

void bufprintf(const char *label, const uint8_t *buf, int len)
{
//	_bufprintf(label, buf, len);

	fprintf(stderr, "=== %s ===\n", label);
	for (int i = 0; i < len; i++)
		fprintf(stderr, "%02x ", buf[i] & 0xff);
	fprintf(stderr, "\n\n");
}

void dump(const uint8_t *buf, int len, char *label)
{
	int i;

	if (label)
		printf("---- %s ----", label);

	for (i = 0; i < len; i++) {
		if (!(i % 4))
			printf("  ");
		if (!(i % 16))
			printf("\n ");
		printf("%02x ", buf[i] & 0xff);
	}

	if (label)
		printf("\n--------------\n");
}

int has_ctrl_char(const uint8_t *data, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		if (data[i] < 32 || data[i] == 127)
			return 1;
	}
	return 0;
}

static inline int os_snprintf_error(size_t size, int res)
{
	return res < 0 || (unsigned int) res >= size;
}


static inline int _snprintf_hex(char *buf, size_t buf_size, uint8_t *data,
				    size_t len, int uppercase)
{
	size_t i;
	char *pos = buf, *end = buf + buf_size;
	int ret;
	if (buf_size == 0)
		return 0;
	for (i = 0; i < len; i++) {
		ret = snprintf(pos, end - pos, uppercase ? "%02X" : "%02x", /* Flawfinder: ignore */
				  data[i]);
		if (os_snprintf_error(end - pos, ret)) {
			end[-1] = '\0';
			return pos - buf;
		}
		pos += ret;
	}
	end[-1] = '\0';
	return pos - buf;
}


int snprintf_hex(char *buf, size_t buf_size, uint8_t *data, size_t len)
{
	return _snprintf_hex(buf, buf_size, data, len, 0);
}

void printf_encode(char *txt, size_t maxlen, const uint8_t *data, size_t len)
{
	char *end = txt + maxlen;
	size_t i;

	for (i = 0; i < len; i++) {
		if (txt + 4 >= end)
			break;

		switch (data[i]) {
		case '\"':
			*txt++ = '\\';
			*txt++ = '\"';
			break;
		case '\\':
			*txt++ = '\\';
			*txt++ = '\\';
			break;
		case '\033':
			*txt++ = '\\';
			*txt++ = 'e';
			break;
		case '\n':
			*txt++ = '\\';
			*txt++ = 'n';
			break;
		case '\r':
			*txt++ = '\\';
			*txt++ = 'r';
			break;
		case '\t':
			*txt++ = '\\';
			*txt++ = 't';
			break;
		default:
			if (data[i] >= 32 && data[i] <= 126) {
				*txt++ = data[i];
			} else {
				txt += snprintf(txt, end - txt, "\\x%02x",
						   data[i]);
			}
			break;
		}
	}

	*txt = '\0';
}

const char *ssid_to_str(const uint8_t *ssid, size_t ssid_len)
{
	static char ssid_txt[32 * 4 + 1];

	if (ssid == NULL) {
		ssid_txt[0] = '\0';
		return ssid_txt;
	}

	printf_encode(ssid_txt, sizeof(ssid_txt), ssid, ssid_len);
	return ssid_txt;
}

int ieee80211_chan_to_freq(uint8_t op_class, uint8_t chan)
{
	/* Table E-4 in IEEE Std 802.11-2012 - Global operating classes */
	switch (op_class) {
	case 81:
		/* channels 1..13 */
		if (chan < 1 || chan > 13)
			return -1;
		return 2407 + 5 * chan;
	case 82:
		/* channel 14 */
		if (chan != 14)
			return -1;
		return 2414 + 5 * chan;
	case 83: /* channels 1..9; 40 MHz */
	case 84: /* channels 5..13; 40 MHz */
		if (chan < 1 || chan > 13)
			return -1;
		return 2407 + 5 * chan;
	case 115: /* channels 36,40,44,48; indoor only */
	case 116: /* channels 36,44; 40 MHz; indoor only */
	case 117: /* channels 40,48; 40 MHz; indoor only */
	case 118: /* channels 52,56,60,64; dfs */
	case 119: /* channels 52,60; 40 MHz; dfs */
	case 120: /* channels 56,64; 40 MHz; dfs */
		if (chan < 36 || chan > 64)
			return -1;
		return 5000 + 5 * chan;
	case 121: /* channels 100-140 */
	case 122: /* channels 100-142; 40 MHz */
	case 123: /* channels 104-136; 40 MHz */
		if (chan < 100 || chan > 140)
			return -1;
		return 5000 + 5 * chan;
	case 124: /* channels 149,153,157,161 */
		if (chan < 149 || chan > 161)
			return -1;
		return 5000 + 5 * chan;
	case 125: /* channels 149,153,157,161,165,169,173,177 */
	case 126: /* channels 149,157,165,173; 40 MHz */
	case 127: /* channels 153,161,169,177; 40 MHz */
		if (chan < 149 || chan > 177)
			return -1;
		return 5000 + 5 * chan;
	case 128: /* center freqs 42, 58, 106, 122, 138, 155, 171; 80 MHz */
	case 130: /* center freqs 42, 58, 106, 122, 138, 155, 171; 80 MHz */
		if (chan < 36 || chan > 177)
			return -1;
		return 5000 + 5 * chan;
	case 129: /* center freqs 50, 114, 163; 160 MHz */
		if (chan < 36 || chan > 177)
			return -1;
		return 5000 + 5 * chan;
	case 131: /* UHB channels, 20 MHz: 1, 5, 9.. */
	case 132: /* UHB channels, 40 MHz: 3, 11, 19.. */
	case 133: /* UHB channels, 80 MHz: 7, 23, 39.. */
	case 134: /* UHB channels, 160 MHz: 15, 47, 79.. */
	case 135: /* UHB channels, 80+80 MHz: 7, 23, 39.. */
		if (chan < 1 || chan > 233)
			return -1;
		return 5950 + chan * 5;
	case 136: /* UHB channels, 20 MHz: 2 */
		if (chan == 2)
			return 5935;
		return -1;
	case 180: /* 60 GHz band, channels 1..8 */
		if (chan < 1 || chan > 8)
			return -1;
		return 56160 + 2160 * chan;
	case 181: /* 60 GHz band, EDMG CB2, channels 9..15 */
		if (chan < 9 || chan > 15)
			return -1;
		return 56160 + 2160 * (chan - 8);
	case 182: /* 60 GHz band, EDMG CB3, channels 17..22 */
		if (chan < 17 || chan > 22)
			return -1;
		return 56160 + 2160 * (chan - 16);
	case 183: /* 60 GHz band, EDMG CB4, channel 25..29 */
		if (chan < 25 || chan > 29)
			return -1;
		return 56160 + 2160 * (chan - 24);
	}
	return -1;
}
