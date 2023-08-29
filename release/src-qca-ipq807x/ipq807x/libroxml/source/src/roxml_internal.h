/**
 * \file roxml_internal.h
 * \brief internal header required by all libroxml sources
 *
 * (c) copyright 2014
 * tristan lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     lgpl-2.1+
 * the author added a static linking exception, see license.txt.
 */

#ifndef ROXML_INTERNAL_H
#define ROXML_INTERNAL_H

#include <errno.h>

#define ROXML_INT
#ifdef __DEBUG
#define ROXML_STATIC
#else
#define ROXML_STATIC static
#endif
#define ROXML_STATIC_INLINE static inline

/* STUB function are here to provide a consistent libroxml binary no matter
 * what the configuration. They are located in a specific section that can be
 * easily optimized out if necessary */
#define ROXML_STUB __attribute__((section("__stub")))

/* PARSE functions are used when doing the parsing of document. This is a time
 * sensitive section and all concerned function are stored inside a same section
 */
#define ROXML_PARSE __attribute__((section("__parse")))

/* PARSE_ERROR macro will raise an parsing error along with its reason */
#ifdef VERBOSE_PARSING
#define ROXML_PARSE_ERROR(str)        do { fprintf(stderr, "Parsing error at offset %d: %s\n", context->pos, str); errno = EINVAL; return -1; } while (0)
#else /* VERBOSE_PARSING */
#define ROXML_PARSE_ERROR(str)        do { errno = EINVAL; return -1; } while (0);
#endif /* VERBOSE_PARSING */

/* must be included first */
#include "roxml_tune.h"

#include "roxml_types.h"
#include "roxml_defines.h"
#include "roxml_utils.h"

/* must be included last */
#include "roxml.h"

#endif /* ROXML_INTERNAL_H */
