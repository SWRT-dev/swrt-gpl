/*
 * utf8.c - utf8 utility functions.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


int unicode_to_utf8(uint32_t codepoint, char *utf8_buffer) {
	if (codepoint <= 0x7F) {
		utf8_buffer[0] = (char)codepoint;
		utf8_buffer[1] = '\0';
		return 1;
	} else if (codepoint <= 0x7FF) {
		utf8_buffer[0] = (char)(0xC0 | ((codepoint >> 6) & 0x1F));
		utf8_buffer[1] = (char)(0x80 | (codepoint & 0x3F));
		utf8_buffer[2] = '\0';
		return 2;
	} else if (codepoint <= 0xFFFF) {
		utf8_buffer[0] = (char)(0xE0 | ((codepoint >> 12) & 0x0F));
		utf8_buffer[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
		utf8_buffer[2] = (char)(0x80 | (codepoint & 0x3F));
		utf8_buffer[3] = '\0';
		return 3;
	} else if (codepoint <= 0x10FFFF) {
		utf8_buffer[0] = (char)(0xF0 | ((codepoint >> 18) & 0x07));
		utf8_buffer[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
		utf8_buffer[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
		utf8_buffer[3] = (char)(0x80 | (codepoint & 0x3F));
		utf8_buffer[4] = '\0';
		return 4;
	} else {
		/* Invalid code point */
		utf8_buffer[0] = '?';
		utf8_buffer[1] = '\0';
		return 1;
	}
}

char *str_to_utf8(char *buf, size_t *outlen)
{
	char *outbuf;
	size_t idx = 0;
	int i;
	uint32_t codepoint;

	outbuf = calloc(1, (4 * strlen(buf)));
	if (!outbuf) {
		*outlen = 0;
		return NULL;
	}

	for (i = 0; buf[i] != '\0';) {
		size_t written;

		/* Check if the character is a valid UTF-8 sequence */
		if ((buf[i] & 0xC0) == 0x80) {
			/* Invalid continuation byte, skip it */
			i++;
			continue;
		}

		/* Decode the UTF-8 sequence into a Unicode code point */
		if ((buf[i] & 0x80) == 0x00) {
			/* Single-byte UTF-8 sequence */
			codepoint = buf[i];
			i++;
		} else if ((buf[i] & 0xE0) == 0xC0) {
			/* Two-byte UTF-8 sequence */
			codepoint = ((buf[i] & 0x1F) << 6) | (buf[i + 1] & 0x3F);
			i += 2;
		} else if ((buf[i] & 0xF0) == 0xE0) {
			/* Three-byte UTF-8 sequence */
			codepoint = ((buf[i] & 0x0F) << 12) | ((buf[i + 1] & 0x3F) << 6) | (buf[i + 2] & 0x3F);
			i += 3;
		} else if ((buf[i] & 0xF8) == 0xF0) {
			/* Four-byte UTF-8 sequence */
			codepoint = ((buf[i] & 0x07) << 18) | ((buf[i + 1] & 0x3F) << 12) | ((buf[i + 2] & 0x3F) << 6) | (buf[i + 3] & 0x3F);
			i += 4;
		} else {
			/* Invalid UTF-8 sequence, skip it */
			i++;
			continue;
		}

		/* Encode the Unicode code point as UTF-8 */
		written = unicode_to_utf8(codepoint, outbuf + idx);
		idx += written;
	}

	/* Null-terminate the output buffer */
	outbuf[idx] = '\0';
	*outlen = idx;
	return outbuf;
}


#if 0
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

int main() {
	char *utf8;
	int outlen = 0;

	utf8 = str_to_utf8("test", &outlen);

	/* Print the UTF-8 representation */
	printf("%s\n", utf8);
	dump(utf8, outlen, "test");
	return 0;
}
#endif
