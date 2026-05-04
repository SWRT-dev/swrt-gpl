/*
 * This file is part of ltrace.
 * Copyright (C) 2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2003,2008,2009 Juan Cespedes
 * Copyright (C) 2006 Ian Wienand
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

#include "config.h"

#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "summary.h"
#include "dict.h"
#include "library.h"
#include "options.h"

struct entry_st {
	const char *name;
	unsigned count;
	struct timeval tv;
};

struct fill_struct_data {
	struct vect entries;
	unsigned tot_count;
	unsigned long tot_usecs;
};

struct opt_c_struct {
	int count;
	struct timeval tv;
};

static struct dict *dict_opt_c;

struct timedelta
calc_time_spent(struct timeval start)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	struct timeval diff;
	diff.tv_sec = tv.tv_sec - start.tv_sec;
	if (tv.tv_usec >= start.tv_usec) {
		diff.tv_usec = tv.tv_usec - start.tv_usec;
	} else {
		diff.tv_sec--;
		diff.tv_usec = 1000000 + tv.tv_usec - start.tv_usec;
	}

	struct timedelta ret = { diff };
	return ret;
}

static enum callback_status
fill_struct(const char **namep, struct opt_c_struct *st, void *u)
{
	struct fill_struct_data *data = u;
	struct entry_st entry = { *namep, st->count, st->tv };
	if (VECT_PUSHBACK(&data->entries, &entry) < 0)
		return CBS_STOP;

	data->tot_count += st->count;
	data->tot_usecs += 1000000 * st->tv.tv_sec;
	data->tot_usecs += st->tv.tv_usec;
	return CBS_CONT;
}

static int
compar(const struct entry_st *en1, const struct entry_st *en2)
{
	if (en2->tv.tv_sec - en1->tv.tv_sec)
		return en2->tv.tv_sec - en1->tv.tv_sec;
	else
		return en2->tv.tv_usec - en1->tv.tv_usec;
}

static enum callback_status
dump_one(struct entry_st *entry, void *u)
{
	struct fill_struct_data *data = u;
	unsigned long long int c;
	unsigned long long int p;
	c = 1000000 * (int)entry->tv.tv_sec +
		(int)entry->tv.tv_usec;
	p = 100000 * c / data->tot_usecs + 5;
	fprintf(options.output, "%3lu.%02lu %4d.%06d %11lu %9d %s\n",
		(unsigned long int)(p / 1000),
		(unsigned long int)((p / 10) % 100),
		(int)entry->tv.tv_sec, (int)entry->tv.tv_usec,
		(unsigned long int)(c / entry->count),
		entry->count,
#ifdef USE_DEMANGLE
		options.demangle ? my_demangle(entry->name) :
#endif
		entry->name);

	return CBS_CONT;
}

void
show_summary(void)
{
	struct fill_struct_data cdata = {};
	VECT_INIT(&cdata.entries, struct entry_st);

	if (dict_opt_c != NULL) {
		DICT_EACH(dict_opt_c, const char *, struct opt_c_struct, NULL,
			  fill_struct, &cdata);

		VECT_QSORT(&cdata.entries, struct entry_st, &compar);
	}

	fprintf(options.output,
		"%% time     seconds  usecs/call     calls      function\n");
	fprintf(options.output,
		"------ ----------- ----------- --------- --------------------\n");

	VECT_EACH(&cdata.entries, struct entry_st, NULL, dump_one, &cdata);

	fprintf(options.output,
		"------ ----------- ----------- --------- --------------------\n");
	fprintf(options.output, "100.00 %4lu.%06lu             %9d total\n",
		cdata.tot_usecs / 1000000,
		cdata.tot_usecs % 1000000, cdata.tot_count);

	vect_destroy(&cdata.entries, NULL, NULL);
}

static void
free_stringp_cb(const char **stringp, void *data)
{
	free((char *)*stringp);
}

void
summary_account_call(struct library_symbol *libsym, struct timedelta spent)
{
	assert(options.summary);

	if (dict_opt_c == NULL) {
		dict_opt_c = malloc(sizeof(*dict_opt_c));
		if (dict_opt_c == NULL) {
		oom:
			fprintf(stderr,
				"Can't allocate memory for "
				"keeping track of -c.\n");
			free(dict_opt_c);
			options.summary = 0;
			return;
		}
		DICT_INIT(dict_opt_c, char *, struct opt_c_struct,
			  dict_hash_string, dict_eq_string, NULL);
	}

	struct opt_c_struct *st = DICT_FIND_REF(dict_opt_c, &libsym->name,
						struct opt_c_struct);
	if (st == NULL) {
		const char *na = strdup(libsym->name);
		struct opt_c_struct new_st = {.count = 0, .tv = {0, 0}};
		if (na == NULL
		    || DICT_INSERT(dict_opt_c, &na, &new_st) < 0) {
			free((char *) na);
			DICT_DESTROY(dict_opt_c, const char *,
				     struct opt_c_struct,
				     free_stringp_cb, NULL, NULL);
			goto oom;
		}
		st = DICT_FIND_REF(dict_opt_c, &libsym->name,
				   struct opt_c_struct);
		assert(st != NULL);
	}

	if (st->tv.tv_usec + spent.tm.tv_usec > 1000000) {
		st->tv.tv_usec += spent.tm.tv_usec - 1000000;
		st->tv.tv_sec++;
	} else {
		st->tv.tv_usec += spent.tm.tv_usec;
	}
	st->count++;
	st->tv.tv_sec += spent.tm.tv_sec;
	return;
}
