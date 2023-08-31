/*
 * (C) Copyright 2000-2004
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <arpa/inet.h>

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

void usage(void);
static char *cmdname;
static int g_dbg = 0;

#define pbdt_dbg(fmt, args...)	if (g_dbg > 1) printf(fmt, ##args)
#define pbdt_info(fmt, args...)	if (g_dbg > 0) printf(fmt, ##args)

size_t strlcpy(char *dst, const char *src, size_t size)
{
	size_t srclen, len;

	srclen = strlen(src);
	if (size <= 0)
		return srclen;

	len = (srclen < size) ? srclen : size - 1;
	memcpy(dst, src, len); /* should not overlap */
	dst[len] = '\0';

	return srclen;
}

/* Translate multiple continuously NULL-terminated strings as newline-terminated strings at run-time,
 * and then write it to file.
 * @fp:
 * @s:	multiple continuously NULL-terminated strings, last string must end with two NULL character.
 * @return:
 * 	0:	success
 *  otherwise:	error
 */
static int write_cont_null_strings(FILE *fp, const char *s)
{
	int ret = 0;
	const char *b = s;

	if (!fp || !s)
		return -2;

	for (b = s; *b != '\0'; ) {
		fprintf(fp, "%s\n", b);
		b += strlen(b);
		if (*b == '\0' && *(b + 1) == '\0')
			break;
		b++;
	}

	return ret;
}

/* Extract keyword from each item of @params, search it in @filename, which is a bdwlan.txt, and replace it with new one.
 * @filename:
 * @params:	char* array, last item must be NULL, newline character will be ignored.
 * @return:
 *  >= 0:	number of lines match @ kw are updated
 *  otherwise:	error
 */
int __update_bdata_file(const char *filename, char **params)
{
	static int id = 0;
	const unsigned char end_mark[2] = { 0, 0 };
	long flen = 0, params_tlen = 0;
	int ret = 0, i, found, copy;
	FILE *fp;
	size_t l, kw_len;
	char kw[1024], *p, **v, *data;
	struct buf_ctrl_s {
		char *buf, *b;
		size_t remain, tlen;
	} bc[2], *src, *dst;

	if (!filename || !params)
		return -1;

	/* Calculate buffer size and allocate two buffers. */
	for (i = 0, v = params; *v != NULL; ++v, ++i) {
		pbdt_dbg("i %4d = \"%s\"\n", i, *v);
		params_tlen += strlen(*v) + 1;
	}
	if (!params_tlen)
		return 0;

	if (!(fp = fopen(filename, "r+")))
		return - 1;

	fseek(fp, 0, SEEK_END);
	flen = ftell(fp);
	for (i = 0, src = &bc[0]; i < ARRAY_SIZE(bc); ++i, ++src) {
		src->buf = malloc(flen + params_tlen);
		if (!src->buf) {
			ret = -3;
			break;
		}

		src->remain = src->tlen = flen + params_tlen + 1;
		*src->buf = '\0';
	}

	if (ret)
		goto exit_update_bdata_file;

	/* Read @filename to 1-st buffer.
	 * All '\n' are replaced with '\0' temporary.
	 */
	src = &bc[0];
	src->b = src->buf;
	rewind(fp);
	while (fgets(kw, sizeof(kw), fp)) {
		/* replace '\n' with '\0' temporary. */
		if ((p = strchr(kw, '\n')) != NULL)
			*p = '\0';

		l = snprintf(src->b, src->remain, "%s", kw) + 1;
		src->b += l;
		src->remain -= l;
	}
	*src->b++ = '\0';
	src->remain--;

	dst = &bc[1];
	for (i = 0, v = &params[0]; *v != NULL; ++i, ++v) {
		p = *v + strlen(*v) - 1;
		/* Remove tailing newline */
		while (p >= *v) {
			if (*p != '\n' && *p != '\r')
				break;
			*p-- = '\0';
		}

		/* remove space/tab after number */
		while (p >= *v) {
			if (*p != ' ' && *p != '\t')
				break;
			*p-- = '\0';
		}

		strlcpy(kw, *v, sizeof(kw));
		p = kw + strlen(kw) - 1;
		/* Remove tailing number. */
		while (p >= kw) {
			if (*p < '0' || *p > '9')
				break;
			*p-- = '\0';
		}
		/* remove '-' if it's negative number. */
		if (p >= kw && *p == '-')
			*p-- = '\0';
		/* remove space/tab between field name and number */
		while (p >= kw) {
			if (*p != ' ' && *p != '\t')
				break;
			*p-- = '\0';
		}

		kw_len = strlen(kw);
		found = 0;
		src = &bc[0 ^ (i & 1)];
		dst = &bc[1 ^ (i & 1)];
		src->b = src->buf;
		dst->b = dst->buf;
		dst->remain = dst->tlen;
		memcpy(dst->b, end_mark, 2);
		pbdt_info("i %4d Replace \"%s.*$\" with \"%s\" ...\n", i, kw, *v);
		while (*(src->b) != '\0') {
			copy = 1;
			data = src->b;
			if (!strncmp(src->b, kw, kw_len)
			 && (*(src->b + kw_len) == ' ' || *(src->b + kw_len) == '\t'))
			{
				found++;
				if (found == 1) {
					/* replace parameter */
					data = *v;
				} else {
					/* skip duplicated parameter */
					copy = 0;
				}
			}

			if (copy) {
				l = snprintf(dst->b, dst->remain, "%s", data) + 1;
				dst->b += l;
				dst->remain -= l;
			}
			src->b += strlen(src->b) + 1;
		}
		if (!found){
			/* should not happen when we modify bdwlan*.txt */
			printf("\"%s\" not found in %s!\n", kw, filename);
		}
		*dst->b++ = '\0';
		dst->remain--;
		if (g_dbg > 1) {
			FILE *f;
			char fn[64];

			snprintf(fn, sizeof(fn), "%s.%d", filename, ++id);
			f = fopen(fn, "w");
			write_cont_null_strings(f, dst->buf);
			fclose(f);
			truncate(fn, dst->tlen - dst->remain - 1);
		}
		ret += found;
	}

	rewind(fp);
	write_cont_null_strings(fp, dst->buf);
	fclose(fp);
	/* last '\0' is not wrote, same offset in file maybe non-zero value. */
	truncate(filename, dst->tlen - dst->remain - 1);

 exit_update_bdata_file:

	for (i = 0, src = &bc[0]; i < ARRAY_SIZE(bc); ++i, ++src)
		if (src->buf)
			free(src->buf);

	return ret;
}

int main(int argc, char **argv)
{
	int c, l, len, opt, ret = 0, args_size, buf_size, remove = 0, add = 0, modify = 0;
	char *p, *args[5001], **v, *buf, *buf_end, *b, line[1024];
	const char *patch_fn = NULL, *output_fn = NULL;
	FILE *sfp;
	struct stat s;

	cmdname = argv[0];
	while ((opt = getopt(argc, argv, "p:b:d")) != -1) {
		switch (opt) {
		case 'p':
			patch_fn = optarg;
			break;
		case 'b':
			output_fn = optarg;
			break;
		case 'd':
			g_dbg++;
			break;
		default:
			usage();
		}
	}

	if (!patch_fn) {
		printf("patch_fn is not specified!\n");
		return -1;
	}
	if (!output_fn) {
		printf("output_fn is not specified!\n");
		return -1;
	}
	if (stat(output_fn, &s)) {
		printf("%s doesn't exist!\n", output_fn);
		return -1;
	}
	if (stat(patch_fn, &s)) {
		printf("stat %s failed %d (%s)!\n", patch_fn, errno, strerror(errno));
		return -1;
	}
	buf_size = s.st_size;
	if (!(b = buf = malloc(buf_size))) {
		printf("Allocate %d bytes as buffer failed!\n", buf_size);
		return -2;
	}
	buf_end = buf + buf_size;

	if (!(sfp = fopen(patch_fn, "r"))) {
		fprintf(stderr, "%s: Can't open %s: %s\n", cmdname, patch_fn,
			strerror(errno));
		return -3;
	}

	v = &args[0];
	c = l = 0;
	args_size = ARRAY_SIZE(args);
	while ((fgets(line, sizeof(line), sfp)) != NULL) {
		if ((*line != '-' && *line != '+')
		 || (*line == '-' && *(line + 1) == '-')
		 || (*line == '+' && *(line + 1) == '+')) {
			continue;
		}

		if (*line == '-') {
			remove++;
			continue;
		}
		else if (*line == '+') {
			add++;
		}

		p = line + strlen(line) - 1;
		/* Remove tailing newline */
		while (p >= line) {
			if (*p != '\n' && *p != '\r')
				break;
			*p-- = '\0';
		}
		/* remove space/tab after number */
		while (p >= line) {
			if (*p != ' ' && *p != '\t')
				break;
			*p-- = '\0';
		}

		/* collect data as much as possible
		 * if pointer/buffer isn't enough, process it.
		 */
		len = strlen(line + 1);
		if ((l + 1) >= args_size || (b + len) >= buf_end) {
			*v = NULL;
			pbdt_info("Patch %d lines\n", l);
			c = __update_bdata_file(output_fn, &args[0]);
			if (c > 0)
				modify += c;
			v = &args[0];
			l = 0;
			b = buf;
		}
		if (c < 0)
			break;

		strlcpy(b, line + 1, (buf_size - (b - buf)));
		*v++ = b;
		l++;
		b += len + 1;
		pbdt_dbg("Add \"%s\" to %d-th entry\n", line + 1, l);
	}
	if (l > 0) {
		*v = NULL;
		printf("Patch %d lines\n", l);
		c = __update_bdata_file(output_fn, &args[0]);
		if (c > 0)
			modify += c;
	}
	printf("%s: modified %d lines. (add %d, remove %d)\n", patch_fn, modify, add, remove);

	free(buf);
	return ret;
}

void usage(void)
{
	fprintf(stderr,
		"Usage: %s -p patch_fn -b new_bdwlan.txt -d\n",
		cmdname);
	exit(-3);
}
