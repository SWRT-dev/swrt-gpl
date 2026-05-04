/*
 * This file is part of ltrace.
 * Copyright (C) 2012 Petr Machata, Red Hat Inc.
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

/* This file contains declarations and types for working with symbol
 * filters.  */

#ifndef FILTER_H
#define FILTER_H

#include <sys/types.h>
#include <regex.h>

#include "forward.h"

enum filter_lib_matcher_type {
	/* Match by soname.  */
	FLM_SONAME,
	/* Match by path name.  */
	FLM_PATHNAME,
	/* Match main binary.  */
	FLM_MAIN,
};

struct filter_lib_matcher {
	enum filter_lib_matcher_type type;
	regex_t libname_re;
};

enum filter_rule_type {
	FR_ADD,
	FR_SUBTRACT,
};

struct filter_rule {
	struct filter_rule *next;
	struct filter_lib_matcher *lib_matcher;
	regex_t symbol_re; /* Regex for matching symbol name.  */
	enum filter_rule_type type;
};

struct filter {
	struct filter *next;
	struct filter_rule *rules;
};

void filter_init(struct filter *filt);
void filter_destroy(struct filter *filt);

/* Both SYMBOL_RE and MATCHER are owned and destroyed by RULE.  */
void filter_rule_init(struct filter_rule *rule, enum filter_rule_type type,
		      struct filter_lib_matcher *matcher,
		      regex_t symbol_re);

void filter_rule_destroy(struct filter_rule *rule);

/* RULE is added to FILT and owned and destroyed by it.  */
void filter_add_rule(struct filter *filt, struct filter_rule *rule);

/* Create a matcher that matches library name.  RE is owned and
 * destroyed by MATCHER.  TYPE shall be FLM_SONAME or
 * FLM_PATHNAME.  */
void filter_lib_matcher_name_init(struct filter_lib_matcher *matcher,
				  enum filter_lib_matcher_type type,
				  regex_t re);

/* Create a matcher that matches main binary.  */
void filter_lib_matcher_main_init(struct filter_lib_matcher *matcher);

void filter_lib_matcher_destroy(struct filter_lib_matcher *matcher);

/* Ask whether FILTER might match a symbol in LIB.  0 if no, non-0 if
 * yes.  Note that positive answer doesn't mean that anything will
 * actually be matched, just that potentially it could.  */
int filter_matches_library(struct filter *filt, struct library *lib);

/* Ask whether FILTER matches this symbol.  Returns 0 if it doesn't,
 * or non-0 value if it does.  */
int filter_matches_symbol(struct filter *filt, const char *sym_name,
			  struct library *lib);

#endif /* FILTER_H */
