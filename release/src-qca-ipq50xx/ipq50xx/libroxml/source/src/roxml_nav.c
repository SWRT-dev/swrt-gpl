/**
 * \file roxml_nav.c
 * \brief XML document navigation module
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include "roxml_internal.h"

ROXML_API node_t *roxml_get_prev_sibling(node_t *n)
{
	node_t *prev = NULL;
	node_t *prev_elm = NULL;

	if (n == ROXML_INVALID_DOC)
		return ROXML_INVALID_DOC;

	if (n->prnt) {
		prev = n->prnt->chld;
		while (prev && prev != n) {
			if ((prev->type & ROXML_NODE_TYPES) == ROXML_ELM_NODE) {
				prev_elm = prev;
			}
			prev = prev->sibl;
		}
	}
	return prev_elm;
}

ROXML_API node_t *roxml_get_next_sibling(node_t *n)
{
	if (n == ROXML_INVALID_DOC)
		return ROXML_INVALID_DOC;

	while (n->sibl && (n->sibl->type & ROXML_NODE_TYPES) != ROXML_ELM_NODE) {
		n = n->sibl;
	}
	return n->sibl;
}

ROXML_API node_t *roxml_get_parent(node_t *n)
{
	if (n == ROXML_INVALID_DOC)
		return ROXML_INVALID_DOC;

	if (n->prnt == NULL)
		return n;
	return n->prnt;
}

ROXML_API node_t *roxml_get_root(node_t *n)
{
	node_t *root;

	if (n == ROXML_INVALID_DOC)
		return ROXML_INVALID_DOC;

	root = n;
	while (root->prnt)
		root = root->prnt;
	return root;
}
