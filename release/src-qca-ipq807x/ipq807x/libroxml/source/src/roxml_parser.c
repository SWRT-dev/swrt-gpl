/**
 * \file roxml_parser.c
 * \brief Parsing engine
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include <stdlib.h>
#include <string.h>
#include "roxml_parser.h"

/* #define DEBUG_PARSING */
#define ROXML_PARSER_NCELLS	256
#define ROXML_PARSER_NCELLS_MAX	512

#define ROXML_PARSER_ITEM(parser, item)	(parser + item)

ROXML_INT int roxml_parser_multiple(roxml_parser_item_t *parser, char *chunk, void *data)
{
	roxml_parser_item_t *item = ROXML_PARSER_ITEM(parser, *chunk)->next;

	/* collision can only occurs on callbacks
	 * that verify the chunk value so we can
	 * just call all cb in turns */
	while (item) {
		int ret = item->func(parser, chunk, data);

		if (ret > 0)
			return ret;
		else if (ret < 0)
			return -1;
		item = item->next;
	}
	return 0;
}

ROXML_INT roxml_parser_item_t *roxml_append_parser_item(roxml_parser_item_t *parser, char *key, roxml_parse_func func)
{
	if (!parser)
		parser = roxml_parser_allocate();

	if (!ROXML_PARSER_ITEM(parser, *key)->func) {
		/* first callback registered */
		ROXML_PARSER_ITEM(parser, *key)->func = func;
	} else if (!ROXML_PARSER_ITEM(parser, *key)->next){
		/* second callback registered:
		 * 1) find free cells in collision pool
		 * 2) initialize cells
		 * 3) enable multiple callbacks.
		 */
		roxml_parser_item_t *collision = parser + ROXML_PARSER_NCELLS;

		while (collision->func)
			collision++;

		/* relocate existing callback in collision pool */
		ROXML_PARSER_ITEM(parser, *key)->next = collision;
		collision->func = ROXML_PARSER_ITEM(parser, *key)->func;
		collision->next = collision + 1;

		/* next available callback is right after (since we do not free single entries) */
		collision++;
		collision->func = func;
		collision->next = NULL;

		ROXML_PARSER_ITEM(parser, *key)->func = roxml_parser_multiple;
	} else {
		/* third or more callback registered: just append */
		roxml_parser_item_t *last;
		roxml_parser_item_t *collision = ROXML_PARSER_ITEM(parser, *key);
		while (collision->next)
			collision = collision->next;
		last = collision;
		while (collision->func)
			collision++;
		last->next = collision;
		collision->func = func;
		collision->next = NULL;
	}

	return parser;
}

ROXML_INT roxml_parser_item_t *roxml_parser_allocate(void)
{
	roxml_parser_item_t *parser;

	/* allocate a new parser.
	 * 512 cells allow up to 256 collisions. */
	parser = malloc(sizeof(roxml_parser_item_t) * ROXML_PARSER_NCELLS_MAX);
	memset(parser, 0 , sizeof(roxml_parser_item_t) * ROXML_PARSER_NCELLS_MAX);

	return parser;
}

ROXML_INT void roxml_parser_free(roxml_parser_item_t *parser)
{
	free(parser);
}

ROXML_INT roxml_parser_item_t *roxml_parser_prepare(roxml_parser_item_t *parser)
{
	int i, j;
	roxml_parser_item_t *new;
	roxml_parser_item_t *collision;

	new = roxml_parser_allocate();
	collision = new + ROXML_PARSER_NCELLS;

	for (i = 0; i < ROXML_PARSER_NCELLS; i++) {
		/* copy parser and assign default */
		if (parser[i].func) {
			new[i].func = parser[i].func;

			if (parser[i].next) {
				roxml_parser_item_t *item = parser[i].next;

				new[i].next = collision;

				/* group all collision callbacks in collision pool */
				while (1) {
					collision->func = item->func;

					if (item->next)
						collision->next = collision + 1;
					else
						break;

					collision = collision->next;
					item = item->next;
				}
				collision++;
			}
		} else {
			new[i].func = parser[0].func;
			new[i].next = parser[0].next;
		}
	}

	roxml_parser_free(parser);

	return new;
}

ROXML_INT int roxml_parse_line(roxml_parser_item_t *parser, char *line, int len, void *ctx)
{
	char *line_end = line;
	char *chunk = line;

	if (len > 0)
		line_end = line + len;
	else
		line_end = line + strlen(line);

	while (chunk < line_end) {
		/* main callbacks */
		int ret = ROXML_PARSER_ITEM(parser, *chunk)->func(parser, chunk, ctx);

		if (ret > 0) {
			chunk += ret;
			continue;
		} else if (ret < 0) {
			return ret;
		}

		/* default callbacks */
		ret = ROXML_PARSER_ITEM(parser, 0)->func(parser, chunk, ctx);

		if (ret > 0) {
			chunk += ret;
			continue;
		} else if (ret < 0) {
			return ret;
		}
	}

	return (chunk - line);
}
