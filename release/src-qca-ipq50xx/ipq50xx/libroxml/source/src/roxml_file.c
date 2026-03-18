/**
 * \file roxml_file.c
 * \brief File XML document backend
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include <stdlib.h>
#include <string.h>
#include "roxml_core.h"
#include "roxml_parser.h"
#include "roxml_internal.h"

ROXML_INT int roxml_read_file(int pos, int size, char *buffer, node_t *node)
{
	int ret_len = 0;

	if (fseek(node->src.fil, pos, SEEK_SET) == 0)
		ret_len = fread(buffer, 1, size, node->src.fil);

	return ret_len;
}

ROXML_INT void roxml_close_file(node_t *root)
{
	fclose(root->src.fil);
}

ROXML_INT int roxml_parse_file(roxml_load_ctx_t *context, roxml_parser_item_t *parser, FILE *file)
{
	char *int_buffer;
	int error = 0;
	int circle = 0;
	int int_len = 0;

	context->type = ROXML_FILE;
	context->src = (void *)file;
	context->pos = 0;

	int_buffer = malloc(ROXML_BULK_READ + 1);

	do {
		int ret = 0;
		int chunk_len = 0;
		int_len = fread(int_buffer + circle, 1, ROXML_BULK_READ - circle, file) + circle;
		int_buffer[int_len] = '\0';

		if (int_len == ROXML_BULK_READ)
			chunk_len = int_len - ROXML_BASE_LEN;
		else
			chunk_len = int_len;

		ret = roxml_parse_line(parser, int_buffer, chunk_len, context);
		circle = int_len - ret;
		if ((ret < 0) || (circle < 0)) {
			error = 1;
			break;
		}
		memmove(int_buffer, int_buffer + ret, circle);
	} while (int_len == ROXML_BULK_READ);

	free(int_buffer);

	if (error == 0 && context->lvl != 0)
		ROXML_PARSE_ERROR("mismatch in open/close nodes");
	return error;
}

ROXML_API node_t *roxml_load_fd(int fd)
{
	FILE *file = NULL;
	node_t *current_node = NULL;

	if (fd < 0)
		return NULL;

	file = fdopen(fd, "r");
	if (file == NULL)
		return ROXML_INVALID_DOC;

	current_node = roxml_create_node(0, file, ROXML_ELM_NODE | ROXML_FILE);

	return roxml_load(current_node, file, NULL);
}

ROXML_API node_t *roxml_load_doc(char *filename)
{
	node_t *current_node = NULL;
	FILE *file = fopen(filename, "rb");

	if (file == NULL)
		return ROXML_INVALID_DOC;

	current_node = roxml_create_node(0, file, ROXML_ELM_NODE | ROXML_FILE);

	return roxml_load(current_node, file, NULL);
}
