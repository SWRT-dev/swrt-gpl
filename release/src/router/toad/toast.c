/*
 * toast.c - Traffic Ordering Agent Settings Tool
 *
 * This is the traffic ordering agent settings tool, which takes care of providing a simple
 * command line interface to operate on the toa settings, hiding the underlying storage mechanism.
 *
 * This is just an example - settings could be stored differently, and/or be configured through
 * some fancy graphical user interface or whatever - but that would be a project on its own.
 *
 * Copyright (C) 2014, Broadcom Corporation
 * All Rights Reserved.
 * 
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;
 * the contents of this file may not be disclosed to third parties, copied
 * or duplicated in any form, in whole or in part, without the prior
 * written permission of Broadcom Corporation.
 *
 * $Id: toast.c 462558 2014-03-18 01:10:07Z $
 *
 */

#include <stdio.h>
#include <string.h>
#include "toa_utils.h"

typedef enum {
	COMMAND_SHOW,		/* Show settings (load and show nvram settings) */
	COMMAND_GET,		/* get <obj-type> <obj-id> */
	COMMAND_SET,		/* set <obj-type> <obj-id> [attr=val] */
	COMMAND_ADD,		/* set <obj-type> <obj-id> [attr=val] */
	COMMAND_DEL,		/* delete <obj-type> <obj-id> */
	COMMAND_ERASE,		/* erase all settings */
	COMMAND_HELP,
	COMMAND_NOP
} toast_action;

enum {
	OBJ_ERROR,
	OBJ_STATION,
	OBJ_BSSID,
	OBJ_DEFAULT
};

static toa_value_map toast_commands[] = {
	{ COMMAND_SHOW, "show" },
	{ COMMAND_SHOW, "list" },
	{ COMMAND_GET, "get" },
	{ COMMAND_SET, "set" },
	{ COMMAND_ADD, "add" },
	{ COMMAND_DEL, "del" },
	{ COMMAND_ERASE, "erase" },
	{ COMMAND_HELP, "help" },
	{ COMMAND_NOP, NULL }
};
static toa_value_map objects[] = {
	{ OBJ_STATION, "station" },
	{ OBJ_STATION, "sta" },
	{ OBJ_BSSID, "bss" },
	{ OBJ_DEFAULT, "default" },
	{ OBJ_ERROR, NULL }
};

typedef struct {
	toast_action	to_action;	/* Action to take (COMMAND_xxx) */
	const char *	obj_type;	/* object type to operate on (station, ssid, ...) */
	const char *	obj_key;	/* object key (station address, ssid name, ...) */
	toa_attribute	*attributes;	/* attribute list, for set command */
} toast_options;

typedef struct {
	toast_options	*options;
	toa_settings	*settings;
} toast_context;

/*
 * parse_cmdline() - parse the command line options into passed toast_options structure.
 *
 * We need to support some form of command to set the attribute(s) for a station in order to
 * facilitate command line configuration, in particular since the nvram key is a sequentially
 * increasing number. We also want to be able to delete a station (renumbering the list), and
 * list all stations.
 *
 * Returns 0 on success.
 */
int
parse_cmdline(toast_options *top, int argc, char **argv)
{
	int argn = 1;
	int action;

	top->to_action = COMMAND_HELP;

	/* Process command line options and determine action to take. */

	while (argn < argc) {

		int have_arg = ((argc-1) - argn);
		char *opt = argv[argn];

		switch ((action = toa_keyword_to_id(toast_commands, opt))) {
		case COMMAND_ERASE:
		case COMMAND_SHOW:
		case COMMAND_HELP:
			top->to_action = action;
			return 0; /* no arguments */
		case COMMAND_ADD:
		case COMMAND_SET:
		case COMMAND_GET:
		case COMMAND_DEL:
			if (have_arg < 1) {
				fprintf(stderr, "%s %s what ?\n",
					argv[0], toa_id_to_keyword(toast_commands, action));
				return -1;
			}
			top->obj_type = argv[++argn];
			if (strcmp(top->obj_type, toa_id_to_keyword(objects, OBJ_DEFAULT)) == 0) {
				top->obj_key = "default";	/* Special case for "default" key */
			} else {
				if (have_arg < 2) {
					fprintf(stderr, "usage: %s %s <obj-type> <obj-id>\n",
						argv[0], toa_id_to_keyword(toast_commands, action));
					return -1;
				}
				top->obj_key = argv[++argn];
			}
			if (toa_keyword_to_id(objects, top->obj_type) == OBJ_ERROR) {
				fprintf(stderr, "%s: object type '%s' unknown.\n",
					argv[0], top->obj_type);
				return -1;
			}
			top->to_action = action;
			break;

		case COMMAND_NOP:
			if (((top->to_action == COMMAND_ADD) || (top->to_action == COMMAND_SET)) &&
				strchr(opt, '=')) {
				if (toa_add_attribute(&top->attributes, opt)) {
					fprintf(stderr, "%s: bad option '%s', try 'help' ?\n",
						argv[0], opt);
					return -1;
				}
			} else {
				fprintf(stderr, "%s: Unrecognized command '%s', try 'help' ?\n",
					argv[0], opt);
				return -1;
			}
			break;
		} /* switch */
		++argn;
	}

	/* Check for leftover arguments */

	if (argn < argc) {
		fprintf(stderr, "%s: Excess argument starting at \"%s\"\n", argv[0], argv[argn]);
		top->to_action = COMMAND_NOP;
		return -1;
	}

	return 0;
}

static void
do_foreach_settings(toast_context *tc, void (*f) (toa_record*, int))
{
	toa_station *rec;

	for (rec = tc->settings->bss_settings; rec; rec = rec->next) {
		f(rec, OBJ_BSSID);
	}
	for (rec = tc->settings->station_settings; rec; rec = rec->next) {
		f(rec, OBJ_STATION);
	}
	for (rec = tc->settings->default_settings; rec; rec = rec->next) {
		f(rec, OBJ_DEFAULT);
	}
}

static void
display_record(toa_record *rec, int obj_type)
{
	toa_attribute *att;

	printf("%s %s", toa_id_to_keyword(objects, obj_type), rec->key);
	for (att = rec->attributes; att; att = att->next) {
		printf(" %s=%s", att->name, att->value);
	}
	printf("\n");
}

static void
erase_record(toa_record *rec, int obj_type)
{
	*rec->key = '\0';	/* Null key to delete rec */
}

static void
toast_usage(const char *pname)
{
	printf("usage: %s <options>\n", pname);
	printf("options are:\n"
		"   show                  show traffic ordering agent settings\n"
		"   del <object> <id>  delete a specific object\n"
		"   get <object> <id>     show settings for a specific object\n"
		"   add <object> <id> [attr=value] create and/or alter objects\n"
		"   erase                 erase all settings\n"
		"\n"
		"\n"
		"objects are:\n"
		"   station <addr>        Values for a specific station (overrides ssid)\n"
		"   bss <index>           Values for a specific BSS id (overrides default)\n"
		"   default               Default values for all stations or BSSs\n"
		"\n"
		"examples:\n"
		"\n"
		"%s set bss 0 type=data\n"
		"%s set bss 2 type=video\n"
		"%s set bss 3 type=public\n"
		"%s add station 10:9a:dd:64:02:fe2 type=video prio=1\n"
		"%s del station 10:9a:dd:64:02:fe2\n",
		pname, pname, pname, pname, pname);
}

int main(int argc, char **argv)
{
	toast_options to = {};
	toast_context tc = { .options = &to };
	int rc;

	tc.settings = toa_load_settings();
	if (!tc.settings) {
		return -1;	/* failed to allocate settings */
	}

	rc = parse_cmdline(&to, argc, argv);
	if (rc == 0) {
		switch (to.to_action) {

		case COMMAND_HELP:
			toast_usage(argv[0]);
			break;

		case COMMAND_SHOW:
			do_foreach_settings(&tc, display_record);
			break;

		case COMMAND_GET:
		case COMMAND_DEL:
		case COMMAND_SET:
		case COMMAND_ADD:
			{
				toa_record *rec = NULL;
				int otype;
				int create = (to.to_action == COMMAND_ADD);

				otype = toa_keyword_to_id(objects, tc.options->obj_type);

				switch (otype) {
				case OBJ_STATION:
					rec = toa_match_record(&tc.settings->station_settings,
						tc.options->obj_key, create);
					break;
				case OBJ_BSSID:
					rec = toa_match_record(&tc.settings->bss_settings,
						tc.options->obj_key, create);
					break;
				case OBJ_DEFAULT:
					rec = toa_match_record(&tc.settings->default_settings,
						tc.options->obj_key, create);
					break;
				case OBJ_ERROR:
					/* should have been caught by parser */
					break;
				}

				if (rec) {
					if (to.to_action == COMMAND_GET) {
						display_record(rec, otype);
					} else
					if (to.to_action == COMMAND_DEL) {
						erase_record(rec, otype);
						toa_save_settings(tc.settings);
					} else
					if ((to.to_action == COMMAND_SET) ||
					    (to.to_action == COMMAND_ADD)) {
						/* Transfer attributes from options to record */

						toa_move_attributes(&rec->attributes,
							&to.attributes);

						toa_save_settings(tc.settings);
					}
				} else {
					fprintf(stderr, "unknown %s \"%s\"\n",
						toa_id_to_keyword(objects, otype),
						tc.options->obj_key);
				}
			}
			break;

		case COMMAND_ERASE:
			do_foreach_settings(&tc, erase_record);
			toa_save_settings(tc.settings);
			break;

		case COMMAND_NOP:
			break;
		}
	}

	/* Clean up */
	toa_flush_attributes(&to.attributes);
	toa_free_settings(tc.settings);

	return rc;
}
