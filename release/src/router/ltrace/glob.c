/*
 * This file is part of ltrace.
 * Copyright (C) 2007,2008,2012,2013 Petr Machata, Red Hat Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include <sys/types.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

static ssize_t
match_character_class(const char *glob, size_t length, size_t from)
{
	assert(length > 0);
	const char *colon = memchr(glob + from + 2, ':', length - 1);
	if (colon == NULL || colon[1] != ']')
		return -1;
	return colon - glob;
}

static ssize_t
match_brack(const char *glob, size_t length, size_t from, int *exclmp)
{
	size_t i = from + 1;

	if (i >= length)
		return -1;

	/* Complement operator.  */
	*exclmp = 0;
	if (glob[i] == '^' || glob[i] == '!') {
		*exclmp = glob[i++] == '!';
		if (i >= length)
			return -1;
	}

	/* On first character, both [ and ] are legal.  But when [ is
	 * followed with :, it's character class.  */
	if (glob[i] == '[' && glob[i + 1] == ':') {
		ssize_t j = match_character_class(glob, length, i);
		if (j < 0)
		fail:
			return -1;
		i = j;
	}
	++i; /* skip any character, including [ or ]  */

	for (; i < length; ++i) {
		char c = glob[i];
		if (c == '[' && glob[i + 1] == ':') {
			ssize_t j = match_character_class(glob, length, i);
			if (j < 0)
				goto fail;
			i = j;

		} else if (c == ']') {
			return i;
		}
	}
	return -1;
}

static int
append(char **bufp, const char *str, size_t str_size,
       size_t *sizep, size_t *allocp)
{
	if (str_size == 0)
		str_size = strlen(str);
	size_t nsize = *sizep + str_size;
	if (nsize > *allocp) {
		size_t nalloc = nsize * 2;
		char *nbuf = realloc(*bufp, nalloc);
		if (nbuf == NULL)
			return -1;
		*allocp = nalloc;
		*bufp = nbuf;
	}

	memcpy(*bufp + *sizep, str, str_size);
	*sizep = nsize;
	return 0;
}

static int
glob_to_regex(const char *glob, char **retp)
{
	size_t allocd = 0;
	size_t size = 0;
	char *buf = NULL;

	size_t length = strlen(glob);
	int escape = 0;
	size_t i;
	for(i = 0; i < length; ++i) {
		char c = glob[i];
		if (escape) {
			if (c == '\\') {
				if (append(&buf, "\\\\", 0,
					   &size, &allocd) < 0) {
				fail:
					free(buf);
					return REG_ESPACE;
				}

			} else if (c == '*') {
				if (append(&buf, "\\*", 0, &size, &allocd) < 0)
					goto fail;
			} else if (c == '?') {
				if (append(&buf, "?", 0, &size, &allocd) < 0)
					goto fail;
			} else if (append(&buf, (char[]){ '\\', c }, 2,
					  &size, &allocd) < 0)
				goto fail;
			escape = 0;
		} else {
			if (c == '\\')
				escape = 1;
			else if (c == '[') {
				int exclm;
				ssize_t j = match_brack(glob, length, i, &exclm);
				if (j < 0) {
					free(buf);
					return REG_EBRACK;
				}
				if (exclm
				    && append(&buf, "[^", 2,
					      &size, &allocd) < 0)
					goto fail;
				if (append(&buf, glob + i + 2*exclm,
					   j - i + 1 - 2*exclm,
					   &size, &allocd) < 0)
					goto fail;
				i = j;

			} else if (c == '*') {
				if (append(&buf, ".*", 0, &size, &allocd) < 0)
					goto fail;
			} else if (c == '?') {
				if (append(&buf, ".", 0, &size, &allocd) < 0)
					goto fail;
			} else if (c == '.') {
				if (append(&buf, "\\.", 0, &size, &allocd) < 0)
					goto fail;
			} else if (append(&buf, &c, 1, &size, &allocd) < 0)
				goto fail;
		}
	}

	if (escape) {
		free(buf);
		return REG_EESCAPE;
	}

	{
		char c = 0;
		if (append(&buf, &c, 1, &size, &allocd) < 0)
			goto fail;
	}
	*retp = buf;
	return 0;
}

int
globcomp(regex_t *preg, const char *glob, int cflags)
{
	char *regex = NULL;
	int status = glob_to_regex(glob, &regex);
	if (status != 0)
		return status;
	assert(regex != NULL);
	status = regcomp(preg, regex, cflags);
	free(regex);
	return status;
}

#ifdef TEST
#include <stdio.h>

static void
translate(const char *glob, int exp_status, const char *expect)
{
	char *pattern = NULL;
	int status = glob_to_regex(glob, &pattern);
	if (status != exp_status) {
		fprintf(stderr, "translating %s, expected status %d, got %d\n",
			glob, exp_status, status);
		return;
	}

	if (status == 0) {
		assert(pattern != NULL);
		if (strcmp(pattern, expect) != 0)
			fprintf(stderr, "translating %s, expected %s, got %s\n",
				glob, expect, pattern);
		free(pattern);
	} else {
		assert(pattern == NULL);
	}
}

static void
try_match(const char *glob, const char *str, int expect)
{
	regex_t preg;
	int status = globcomp(&preg, glob, 0);
	assert(status == 0);
	status = regexec(&preg, str, 0, NULL, 0);
	assert(status == expect);
	regfree(&preg);
}

int
main(void)
{
        translate("*", 0, ".*");
        translate("?", 0, ".");
        translate(".*", 0, "\\..*");
        translate("*.*", 0, ".*\\..*");
        translate("*a*", 0, ".*a.*");
        translate("[abc]", 0, "[abc]");
        translate("[^abc]", 0, "[^abc]");
        translate("[!abc]", 0, "[^abc]");
        translate("[]]", 0, "[]]");
        translate("[[]", 0, "[[]");
        translate("[^]]", 0, "[^]]");
        translate("[^a-z]", 0, "[^a-z]");
        translate("[abc\\]]", 0, "[abc\\]]");
        translate("[abc\\]def]", 0, "[abc\\]def]");
        translate("[[:space:]]", 0, "[[:space:]]");
        translate("[^[:space:]]", 0, "[^[:space:]]");
        translate("[![:space:]]", 0, "[^[:space:]]");
        translate("[^a-z]*", 0, "[^a-z].*");
        translate("[^a-z]bar*", 0, "[^a-z]bar.*");
	translate("*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.*.", 0,
		  ".*\\..*\\..*\\..*\\..*\\..*\\..*\\..*\\."
		  ".*\\..*\\..*\\..*\\..*\\..*\\..*\\..*\\.");

        translate("\\", REG_EESCAPE, NULL);
        translate("[^[:naotuh\\", REG_EBRACK, NULL);
        translate("[^[:", REG_EBRACK, NULL);
        translate("[^[", REG_EBRACK, NULL);
        translate("[^", REG_EBRACK, NULL);
        translate("[\\", REG_EBRACK, NULL);
        translate("[", REG_EBRACK, NULL);
        translate("abc[", REG_EBRACK, NULL);

	try_match("abc*def", "abc012def", 0);
	try_match("abc*def", "ab012def", REG_NOMATCH);
	try_match("[abc]*def", "a1def", 0);
	try_match("[abc]*def", "b1def", 0);
	try_match("[abc]*def", "d1def", REG_NOMATCH);

	return 0;
}

#endif
