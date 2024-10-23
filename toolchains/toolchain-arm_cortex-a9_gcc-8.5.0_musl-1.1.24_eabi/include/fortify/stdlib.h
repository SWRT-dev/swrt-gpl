/*
 * Copyright (C) 2015 Dimitris Papastamos <sin@2f30.org>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _FORTIFY_STDLIB_H
#define _FORTIFY_STDLIB_H

__extension__
#include_next <stdlib.h>

#if defined(_FORTIFY_SOURCE) && _FORTIFY_SOURCE > 0 && defined(__OPTIMIZE__) && __OPTIMIZE__ > 0
#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
__extension__
#include_next <limits.h>
#endif

#include "fortify-headers.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(_XOPEN_SOURCE) || defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#undef realpath
_FORTIFY_FN(realpath) char *realpath(const char *__p, char *__r)
{
	size_t __b = __builtin_object_size(__r, 0);

	if (__r) {
#ifndef PATH_MAX
#error PATH_MAX unset. A fortified realpath will not work.
#else
		char __buf[PATH_MAX], *__ret;
		size_t __l;

		__ret = __orig_realpath(__p, __buf);
		if (!__ret)
			return NULL;
		__l = __builtin_strlen(__ret) + 1;
		if (__l > __b)
			__builtin_trap();
		__builtin_memcpy(__r, __ret, __l);
		return __r;
#endif
	}
	return __orig_realpath(__p, __r);
}
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
