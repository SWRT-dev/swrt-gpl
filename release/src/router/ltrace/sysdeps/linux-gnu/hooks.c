/*
 * This file is part of ltrace.
 * Copyright (C) 2012, 2013, 2015 Petr Machata
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

#define _GNU_SOURCE

#include <sys/types.h>
#include <alloca.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "backend.h"
#include "dict.h"
#include "options.h"
#include "sysdep.h"
#include "vect.h"

static char *
append(const char *str1, const char *str2)
{
	char *ret = malloc(strlen(str1) + strlen(str2) + 2);
	if (ret == NULL)
		return ret;
	strcpy(stpcpy(ret, str1), str2);
	return ret;
}

static void
add_dir(struct vect *dirs, const char *str1, const char *str2)
{
	char *dir = append(str1, str2);
	if (dir != NULL
	    && VECT_PUSHBACK(dirs, &dir) < 0)
		fprintf(stderr,
			"Couldn't store candidate config directory %s%s: %s.\n",
			str1, str2, strerror(errno));
}

static enum callback_status
add_dir_component_cb(struct opt_F_t *entry, void *data)
{
	struct vect *dirs = data;
	if (opt_F_get_kind(entry) == OPT_F_DIR)
		add_dir(dirs, entry->pathname, "/ltrace");
	return CBS_CONT;
}

static void
destroy_opt_F_cb(struct opt_F_t *entry, void *data)
{
	opt_F_destroy(entry);
}

static char *g_home_dir = NULL;

int
os_get_config_dirs(int private, const char ***retp)
{
	/* Vector of char *.  Contains first pointers to local paths,
	 * then NULL, then pointers to system paths, then another
	 * NULL.  SYS_START points to the beginning of the second
	 * part.  */
	static struct vect dirs;
	static ssize_t sys_start = 0;

again:
	if (sys_start != 0) {
		if (sys_start == -1)
			return -1;

		if (retp != NULL) {
			if (private)
				*retp = VECT_ELEMENT(&dirs, const char *, 0);
			else
				*retp = VECT_ELEMENT(&dirs, const char *,
						     (size_t)sys_start);
		}

		return 0;
	}

	VECT_INIT(&dirs, char *);

	char *home = getenv("HOME");
	if (home == NULL) {
		struct passwd *pwd = getpwuid(getuid());
		if (pwd != NULL)
			home = pwd->pw_dir;
	}

	size_t home_len = home != NULL ? strlen(home) : 0;

	/* The values coming from getenv and getpwuid may not be
	 * persistent.  */
	if (home != NULL) {
		free(g_home_dir);
		g_home_dir = strdup(home);
		if (g_home_dir != NULL) {
			home = g_home_dir;
		} else {
			char *tmp = alloca(home_len + 1);
			strcpy(tmp, home);
			home = tmp;
		}
	}

	char *xdg_home = getenv("XDG_CONFIG_HOME");
	if (xdg_home == NULL && home != NULL) {
		xdg_home = alloca(home_len + sizeof "/.config");
		sprintf(xdg_home, "%s/.config", home);
	}
	if (xdg_home != NULL)
		add_dir(&dirs, xdg_home, "/ltrace");
	if (home != NULL)
		add_dir(&dirs, home, "/.ltrace");

	char *delim = NULL;
	if (VECT_PUSHBACK(&dirs, &delim) < 0) {
	fail:
		/* This can't work :(  */
		fprintf(stderr,
			"Couldn't initialize list of config directories: %s.\n",
			strerror(errno));
		VECT_DESTROY(&dirs, const char *, dict_dtor_string, NULL);
		sys_start = -1;
		return -1;
	}
	sys_start = vect_size(&dirs);

	/* """preference-ordered set of base directories to search for
	 * configuration files in addition to the $XDG_CONFIG_HOME
	 * base directory. The directories in $XDG_CONFIG_DIRS should
	 * be seperated with a colon ':'."""  */
	char *xdg_sys = getenv("XDG_CONFIG_DIRS");
	if (xdg_sys != NULL) {
		struct vect v;
		VECT_INIT(&v, struct opt_F_t);
		if (parse_colon_separated_list(xdg_sys, &v, OPT_F_ENVIRON) < 0
		    || VECT_EACH(&v, struct opt_F_t, NULL,
				 add_dir_component_cb, &dirs) != NULL)
			fprintf(stderr,
				"Error processing $XDG_CONFIG_DIRS '%s': %s\n",
				xdg_sys, strerror(errno));
		VECT_DESTROY(&v, struct opt_F_t, destroy_opt_F_cb, NULL);
	}

	/* PKGDATADIR is passed via -D when compiling.  */
	const char *pkgdatadir = PKGDATADIR;
	if (pkgdatadir != NULL)
		add_dir(&dirs, pkgdatadir, "");

	if (VECT_PUSHBACK(&dirs, &delim) < 0)
		goto fail;

	goto again;
}

int
os_get_ltrace_conf_filenames(struct vect *retp)
{
	char *homepath = NULL;
	char *syspath = NULL;

#define FN ".ltrace.conf"
	if (g_home_dir == NULL)
		os_get_config_dirs(0, NULL);

	if (g_home_dir != NULL) {
		homepath = malloc(strlen(g_home_dir) + 1 + sizeof FN);
		if (homepath == NULL
		    || sprintf(homepath, "%s/%s", g_home_dir, FN) < 0) {
		fail:
			free(syspath);
			free(homepath);
			return -1;
		}
	}

	/* SYSCONFDIR is passed via -D when compiling.  */
	const char *sysconfdir = SYSCONFDIR;
	if (sysconfdir != NULL && *sysconfdir != '\0') {
		/* No +1, we skip the initial period.  */
		syspath = malloc(strlen(sysconfdir) + sizeof FN);
		if (syspath == NULL
		    || sprintf(syspath, "%s/%s", sysconfdir, FN + 1) < 0)
			goto fail;
	}

	if (VECT_PUSHBACK(retp, &homepath) < 0
	    || VECT_PUSHBACK(retp, &syspath) < 0)
		goto fail;

	return 0;
}
