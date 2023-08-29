/**
 * \file roxml_buff.c
 * \brief buffer XML document backend
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include "roxml_core.h"
#include "roxml_parser.h"
#include "roxml_internal.h"

ROXML_INT int roxml_read_buff(int pos, int size, char *buffer, node_t *node)
{
	int ret_len = 0;
	char *r1 = buffer;
	char const *r2 = node->src.buf + pos;

	while (size-- && (*r1++ = *r2++)) ;

	ret_len = r1 - buffer;

	return ret_len;
}

ROXML_INT void roxml_close_buff(node_t *n)
{
	return;
}

ROXML_INT int roxml_parse_buff(roxml_load_ctx_t *context, roxml_parser_item_t *parser, char *buffer)
{
	int ret = 0;

	context->type = ROXML_BUFF;
	context->src = (void *)buffer;
	ret = roxml_parse_line(parser, buffer, 0, context);

	if (ret >= 0 && context->lvl != 0)
		ROXML_PARSE_ERROR("mismatch in open/close nodes");
	return (ret < 0) ? 1 : 0;
}

ROXML_API node_t *roxml_load_buf(char *buffer)
{
	node_t *current_node = NULL;
	if (buffer == NULL) {
		errno = ENODATA;
		return NULL;
	}

	current_node = roxml_create_node(0, buffer, ROXML_ELM_NODE | ROXML_BUFF);

	return roxml_load(current_node, NULL, buffer);
}
