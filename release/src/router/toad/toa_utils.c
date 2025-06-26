/*
 * toa_utils.c - traffic ordering agent common utility functions
 *
 * This is the traffic ordering agent, which takes care of assigning a scheduler
 * and priority level to associated stations.
 *
 * Copyright (C) 2014, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: toa_utils.c 462558 2014-03-18 01:10:07Z $
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

#include <typedefs.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <bcmendian.h>
#include <shutils.h>	/* foreach() */

#include "toa_utils.h"

static const char *station_key_fmtstr = "toa-sta-%d";	/* format string, passed to snprintf() */
static const char *bss_key_fmtstr    = "toa-bss-%d";	/* format string, passed to snprintf() */
static const char *default_key_fmtstr = "toa-defs";	/* format string, passed to snprintf() */

#define MAX_KEY_LEN 16				/* the expanded key format string must fit within */
#define MAX_ATT_LEN 50				/* attribute=value must fit within */

const toa_value_map attribute_map[] = {		/* Cannot have aliases for attribute names */
	{ ATTR_TYPE, "type" },
	{ ATTR_PRIO, "prio" },
	{ ATTR_NAME, "name" },
	{ ATTR_ERROR, NULL }
};

const toa_value_map sta_type_map[] = {
	{ STA_TYPE_VIDEO, "video" },
	{ STA_TYPE_DATA, "data" },
	{ STA_TYPE_PUBLIC, "public" },
	{ STA_TYPE_ERROR, NULL }
};

const toa_value_map sta_prio_map[] = {		/* Attributes can have aliases */
	{ STA_PRIO_DEFAULT, "default" },
	{ STA_PRIO_HIGHEST, "highest" },
	{ STA_PRIO_HIGHER, "higher" },
	{ STA_PRIO_HIGH, "high" },
	{ STA_PRIO_LOW, "low" },
	{ STA_PRIO_LOWER, "lower" },
	{ STA_PRIO_LOWEST, "lowest" },
	{ STA_PRIO_ERROR, NULL }
};

int
toa_keyword_to_id(const toa_value_map *map, const char *kw)
{
	int i;
	for (i = 0; map[i].name; ++i) {
		if (!strcmp(map[i].name, kw)) {
			return map[i].id;
		}
	}
	return map[i].id;
}

const char *
toa_id_to_keyword(const toa_value_map *map, int id)
{
	int i;
	for (i = 0; map[i].name; ++i) {
		if (map[i].id == id) {
			return map[i].name;
		}
	}
	return NULL;
}

void
toa_flush_attributes(toa_attribute **headp)
{
	toa_attribute *att;

	while ((att = *headp)) {
		*headp = att->next;
		if (att->name) {
			free((void *)att->name);
		}
		if (att->value) {
			free((void *)att->value);
		}
	}
}

static toa_attribute *
toa_get_attribute_by_name(toa_attribute *head, const char *a_name)
{
	toa_attribute *att;
	for (att = head; att; att = att->next) {
		if (strcmp(att->name, a_name) == 0) {
			return att;
		}
	}
	return NULL;	/* not found in attributes */
}

static int
toa_add_attribute_nv(toa_attribute **headp, const char *attr, const char *value)
{
	toa_attribute *attribute;

	/*
	 * Check whether we are to update or add an attribute.
	 */
	attribute = toa_get_attribute_by_name(*headp, attr);
	if (attribute) {
		/* Update attribute */
		if (attribute->value) {
			free((void *)attribute->value);
		}
		attribute->value = strdup(value);
		return 0;		/* Success */
	} else {
		/* Add attribute */
		attribute = malloc(sizeof(*attribute));
		if (!attribute) {
			toa_log_error("%s: Failed to malloc attribute: %s\n",
				__FUNCTION__, strerror(errno));
		} else {
			memset(attribute, 0, sizeof(*attribute));
			attribute->name = strdup(attr);
			attribute->value = strdup(value);
			attribute->next = *headp;
			*headp = attribute;
			return 0;	/* all is well */
		}
	}
	return -1;
}

static int
toa_validate_attribute(const char *attr, const char *value)
{
	int attr_id = toa_keyword_to_id(attribute_map, attr);
	if (attr_id == ATTR_ERROR) {
		return -1;
	}

	switch (attr_id) {
	case ATTR_TYPE:
		if (toa_keyword_to_id(sta_type_map, value) == STA_TYPE_ERROR) {
			return -2;
		}
		break;
	case ATTR_PRIO:
		if (toa_keyword_to_id(sta_prio_map, value) == STA_PRIO_ERROR) {
			const char *p = value;
			while (*p != '\0') {
				if (isspace(*p)) {
					break;
				} else if (!isdigit(*p)) {
					return -3;
				}
				++p;
			}
		}
		break;
	}

	return 0;
}

void
toa_move_attributes(toa_attribute **dst, toa_attribute **src)
{
	toa_attribute *a;

	for (a = *src; a; a = a->next) {
		toa_add_attribute_nv(dst, a->name, a->value);
		free((void *)a->name);
		free((void *)a->value);
	}
	*src = NULL;	/* src has been transferred to dst, clear it */
}

toa_attribute *
toa_get_attribute_by_id(toa_attribute *head, toa_attribute_id aid)
{
	const char *a_name;

	a_name = toa_id_to_keyword(attribute_map, aid);
	if (!a_name) {
		return NULL;	/* Must add attribute ID and name to map */
	}

	return toa_get_attribute_by_name(head, a_name);
}

int
toa_add_attribute(toa_attribute **headp, char *token)
{
	/* Must be some form of "attr=value" */
	char *attr, *value;

	attr = strtok(token, "=");
	value = strtok(NULL, "=");
	if (attr && value && !toa_validate_attribute(attr, value)) {
		return toa_add_attribute_nv(headp, attr, value);
	}
	return -1;
}

/*
 * toa_load_record() - load a single record.
 *
 * This function loads a single record from storage (nvram).
 *
 * The return value is a pointer to a newly allocated toa_record structure, or NULL on failure.
 *
 */
static toa_station *
toa_load_record(const char *keyfmt, int stain)
{
	char token[MAX_ATT_LEN];
	char keybuf[MAX_KEY_LEN];
	toa_record *toast;
	char *next, *tokens;

	if (snprintf(keybuf, sizeof(keybuf), keyfmt, stain) >= sizeof(keybuf)) {
		toa_log_error("%s: key buffer too small\n", __FUNCTION__);
		return NULL;
	}

	tokens = nvram_get(keybuf);
	if (!tokens) {
		return NULL;
	}

	/* Allocate toa station structure */
	toast = malloc(sizeof(*toast));
	if (!toast) {
		toa_log_error("%s: Failed to allocated toa_record: %s\n",
			__FUNCTION__, strerror(errno));
		return NULL;
	}
	memset(toast, 0, sizeof(*toast));

	/* Parse toa station parameters into structure */

	foreach(token, tokens, next) { /* I hate this opaque macro - but it works */

		if (toast->key == NULL) {
			/* First argument must be key - check (TBS) and take a copy */
			toast->key = strdup(token);
			if (!toast->key) {
				toa_log_error("%s: Failed to strdup key: %s\n",
					__FUNCTION__, strerror(errno));
				free(toast);
				return NULL;
			}
		} else {
			if (toa_add_attribute(&toast->attributes, token)) {
				toa_log_error("%s: [%s]Invalid or missing attr=val - ignored\n",
					__FUNCTION__, toast->key);
			}
		}
	}

	return toast;
}

/*
 * toa_load_station() - wrapper function to load a station record.
 *
 * The station record key is the station mac address.
 *
 */
static toa_station *
toa_load_station(int stain)
{
	return toa_load_record(station_key_fmtstr, stain);
}

/*
 * toa_load_bss() - wrapper function to load a bss record.
 *
 * The bss record key is the bss index in decimal.
 *
 */
static toa_ssid *
toa_load_bss(int stain)
{
	return toa_load_record(bss_key_fmtstr, stain);
}

static toa_record *
toa_load_defaults(void)
{
	return toa_load_record(default_key_fmtstr, 0);
}

/*
 * toa_match_record() - find or create a specific record.
 *
 * This function looks up a record for a specific key in a list of records.
 *
 * Returns NULL if the record was neither found nor created.
 *
 */
toa_record *
toa_match_record(toa_record **list_head, const char *key, int create)
{
	toa_record *rec;

	for (rec = *list_head; rec; rec = rec->next) {
		if (!strcasecmp(rec->key, key)) {
			return rec;
		}
	}

	if (create) {
		rec = malloc(sizeof(*rec));
		if (rec) {
			memset(rec, 0, sizeof(*rec));
			rec->key = strdup(key);
			if (!rec->key) {
				toa_log_error("%s: Failed to strdup key: %s\n",
					__FUNCTION__, strerror(errno));
				free(rec);
				rec = NULL;
			} else {
				rec->next = *list_head;
				*list_head = rec;
			}
		} else {
			toa_log_error("%s: Failed to malloc record: %s\n",
				__FUNCTION__, strerror(errno));
		}
	}
	return rec;
}


toa_settings *
toa_load_settings()
{
	toa_settings *toasts;
	toa_record *toast;
	int stain;

	toasts = malloc(sizeof(*toasts));
	if (!toasts) {
		toa_log_error("%s: Failed to malloc record: %s\n", __FUNCTION__, strerror(errno));
		return NULL;
	}
	memset(toasts, 0, sizeof(*toasts));

	/* Load station specific settings */
	for (stain = 1; (toast = toa_load_station(stain)); ++stain) {
		toast->next = toasts->station_settings;
		toasts->station_settings = toast;
	}

	/* Load ssid specific settings */
	for (stain = 1; (toast = toa_load_bss(stain)); ++stain) {
		toast->next = toasts->bss_settings;
		toasts->bss_settings = toast;
	}

	/* Load default settings */
	if ((toast = toa_load_defaults())) {
		toast->next = toasts->default_settings;
		toasts->default_settings = toast;
	}

	return toasts;
}

/*
 * toa_save_record_list() - save a list of records, using a specific key
 */
static int
toa_save_record_list(toa_record *listhead, const char *key_fmtstr)
{
#define BUFFER_SIZE 512
	char *buffer;
	int buflen = BUFFER_SIZE;
	toa_record *toast;
	int stain = 1;
	int ndeleted = 0;
	char keybuf[MAX_KEY_LEN];

	buffer = malloc(buflen);
	if (!buffer) {
		toa_log_error("%s: Failed to malloc buffer: %s\n", __FUNCTION__, strerror(errno));
		return -1;
	}

	/* This is where keeping the attr/val as a tlv list or so comes in handy */

	for (toast = listhead; toast; toast = toast->next) {
		toa_attribute *att;

		/* If the record key is null, this is a delete - do not save it. */
		if (*toast->key == '\0') {
			++ndeleted;
			continue;
		}
		snprintf(buffer, buflen, "%s", toast->key);
		for (att = toast->attributes; att; att = att->next) {
			if (att->name && att->value) {
				strncat(buffer, " ", buflen-strlen(buffer));
				strncat(buffer, att->name, buflen-strlen(buffer));
				strncat(buffer, "=", buflen-strlen(buffer));
				strncat(buffer, att->value, buflen-strlen(buffer));
			}
		}
		snprintf(keybuf, MAX_KEY_LEN, key_fmtstr, stain);
		nvram_set(keybuf, buffer);
		++stain;
	}

	/* Purge any deleted records from nvram */
	while (ndeleted) {
		snprintf(keybuf, MAX_KEY_LEN, key_fmtstr, stain);
		nvram_unset(keybuf);
		++stain;
		--ndeleted;
	}
	free(buffer);
	return 0;
}

/*
 * toa_save_settings() - save all settings to some storage, nvram here, but could be anything.
 *
 */
int
toa_save_settings(toa_settings *toasts)
{
	int rc;

	rc = toa_save_record_list(toasts->station_settings, station_key_fmtstr) ||
	toa_save_record_list(toasts->bss_settings, bss_key_fmtstr) ||
	toa_save_record_list(toasts->default_settings, default_key_fmtstr);

	nvram_commit();

	return rc;
}

/*
 * toa_free_record() - release memory allocated for a single record.
 */
static void
toa_free_record(toa_record *rec)
{
	if (rec->key) {
		free(rec->key);
	}
	toa_flush_attributes(&rec->attributes);
	free(rec);
}

/*
 * toa_free_settings() - release memory allocated for all the settings.
 */
void
toa_free_settings(toa_settings *toasts)
{
	toa_record *toast;

	if (toasts) {
		while ((toast = toasts->station_settings)) {
			toasts->station_settings = toast->next;
			toa_free_record(toast);
		}
		while ((toast = toasts->bss_settings)) {
			toasts->bss_settings = toast->next;
			toa_free_record(toast);
		}
		while ((toast = toasts->default_settings)) {
			toasts->default_settings = toast->next;
			toa_free_record(toast);
		}
		free(toasts);
	}
}

/*
 * toa_log_error() - log an error message to whereever appropriate, stderr or syslog, depending
 * on whether we are running as a daemon or not.
 */
void
toa_log_error(const char *fmtstr, ...)
{
	va_list args;

	va_start(args, fmtstr);
	if (isatty(STDIN_FILENO)) {
		vfprintf(stderr, fmtstr, args);
	} else {
		vsyslog(LOG_DAEMON, fmtstr, args);
	}
	va_end(args);
}
