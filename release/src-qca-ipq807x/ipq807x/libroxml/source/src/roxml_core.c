/**
 * \file roxml_core.c
 * \brief XML parsing core module
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
#include "roxml_file.h"
#include "roxml_buff.h"
#include "roxml_parser.h"

#ifdef __DEBUG
unsigned int _nb_node = 0;
unsigned int _nb_attr = 0;
unsigned int _nb_text = 0;
#endif

ROXML_API void roxml_close(node_t *n)
{
	node_t *root = n;

	if (root == ROXML_INVALID_DOC)
		return;

	while (root->prnt != NULL)
		root = root->prnt;

	roxml_del_tree(root->chld);
	roxml_del_tree(root->sibl);

	if ((root->type & ROXML_FILE) == ROXML_FILE)
		roxml_close_file(root);
	else
		roxml_close_buff(root);

	roxml_free_node(root);
}

ROXML_INT node_t *roxml_create_node(int pos, void *src, int type)
{
	node_t *n = (node_t *)calloc(1, sizeof(node_t));
	n->type = type;
	n->src.src = src;
	n->pos = pos;
	n->end = pos;

	return n;
}

/** \brief internal function
 *
 * \fn void ROXML_STATIC ROXML_INT roxml_close_node(node_t *n, node_t *close);
 * This function close the node (add the end offset) and parent the node
 * \param n is the node to close
 * \param close is the node that close node n
 * \return void
 */
ROXML_STATIC ROXML_INT void roxml_close_node(node_t *n, node_t *close)
{
	if (n) {
		n->end = close->pos;
		free(close);

#ifdef __DEBUG
		if (n->type & ROXML_ELM_NODE)
			_nb_node++;
		if (n->type & ROXML_ATTR_NODE)
			_nb_attr++;
		if (n->type & ROXML_TXT_NODE)
			_nb_text++;
#endif
	}
}

ROXML_INT void roxml_free_node(node_t *n)
{
	if (!n)
		return;

	if (n->type & ROXML_PENDING)
		if (n->pos == 0)
			free(n->src.buf);

	if (n->priv) {
		unsigned char id = *(unsigned char *)n->priv;

		if (id == ROXML_REQTABLE_ID) {
			xpath_tok_t *tok;
			xpath_tok_table_t *table = (xpath_tok_table_t *)n->priv;

			roxml_lock_destroy(n);

			tok = table->next;
			free(table);
			while (tok) {
				xpath_tok_t *to_delete = tok;
				tok = tok->next;
				free(to_delete);
			}
		} else if (id == ROXML_NS_ID) {
			roxml_ns_t *ns = (roxml_ns_t *)n->priv;
			free(ns);
		}
	}
	free(n);
}

ROXML_INT void roxml_del_tree(node_t *n)
{
	if (n == NULL)
		return;

	if (n != n->chld)
		roxml_del_tree(n->chld);
	if (n != n->sibl)
		roxml_del_tree(n->sibl);
	roxml_del_tree(n->attr);
	roxml_free_node(n);
}

ROXML_INT inline int roxml_is_separator(char sep)
{
	int i = 0;
	char separators[32] = "\r\t\n ";

	while (separators[i] != 0)
		if (sep == separators[i++])
			return 1;
	return 0;
}

/** \brief namespace without alias name creation during parsing
 *
 * \fn roxml_process_unaliased_ns(roxml_load_ctx_t *context);
 * this function create a new namespace without alias (default ns or remove ns)
 * \param context the parsing context
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_process_unaliased_ns(roxml_load_ctx_t *context)
{
	if (context->nsdef) {
		context->nsdef = 0;
		context->candidat_arg->type |= ROXML_NS_NODE;

		if (context->candidat_node->ns && ((context->candidat_node->ns->type & ROXML_INVALID) == ROXML_INVALID))
			roxml_free_node(context->candidat_node->ns);

		if (context->candidat_val->pos == context->candidat_val->end) {
			context->candidat_node->ns = NULL;
			context->candidat_arg->ns = NULL;
		} else {
			roxml_ns_t *ns = calloc(1, sizeof(roxml_ns_t) + 1);
			ns->id = ROXML_NS_ID;
			ns->alias = (char *)ns + sizeof(roxml_ns_t);

			context->candidat_arg->priv = ns;
			context->candidat_arg->ns = context->candidat_arg;
			context->candidat_node->ns = context->candidat_arg;

			context->namespaces = context->candidat_arg;
		}
	}
}

/** \brief node creation during parsing
 *
 * \fn roxml_process_begin_node(roxml_load_ctx_t *context, int position);
 * this function create a new node upon finding new opening sign. It closes previous node if necessary
 * \param context the parsing context
 * \param position the position in the file
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_process_begin_node(roxml_load_ctx_t *context, int position)
{
	if (context->candidat_txt) {
#ifdef IGNORE_EMPTY_TEXT_NODES
		if (context->empty_text_node == 0) {
#endif /* IGNORE_EMPTY_TEXT_NODES */
			node_t *to_be_closed =
			    roxml_create_node(position, context->src, ROXML_TXT_NODE | context->type);
			context->candidat_txt = roxml_append_node(context->current_node, context->candidat_txt);
			roxml_close_node(context->candidat_txt, to_be_closed);
			context->current_node = context->candidat_txt->prnt;
#ifdef IGNORE_EMPTY_TEXT_NODES
		} else {
			roxml_free_node(context->candidat_txt);
		}
#endif /* IGNORE_EMPTY_TEXT_NODES */
		context->candidat_txt = NULL;
	}
	context->candidat_node = roxml_create_node(position, context->src, ROXML_ELM_NODE | context->type);
}

ROXML_INT node_t *roxml_load(node_t *current_node, FILE *file, char *buffer)
{
	int error = 0;
	node_t *root = current_node;
	roxml_load_ctx_t context;
	roxml_parser_item_t *parser = NULL;

	memset(&context, 0, sizeof(roxml_load_ctx_t));
	context.empty_text_node = 1;
	context.current_node = current_node;

	parser = roxml_append_parser_item(parser, " ", _func_load_white);
	parser = roxml_append_parser_item(parser, "<", _func_load_open_node);
	parser = roxml_append_parser_item(parser, ">", _func_load_close_node);
	parser = roxml_append_parser_item(parser, "/", _func_load_end_node);
	parser = roxml_append_parser_item(parser, "'", _func_load_quoted);
	parser = roxml_append_parser_item(parser, "\"", _func_load_dquoted);
	parser = roxml_append_parser_item(parser, "\t", _func_load_white);
	parser = roxml_append_parser_item(parser, "\n", _func_load_white);
	parser = roxml_append_parser_item(parser, "\r", _func_load_white);
	parser = roxml_append_parser_item(parser, "!", _func_load_open_spec_node);
	parser = roxml_append_parser_item(parser, "]", _func_load_close_cdata);
	parser = roxml_append_parser_item(parser, "-", _func_load_close_comment);
	parser = roxml_append_parser_item(parser, "?", _func_load_close_pi);
	parser = roxml_append_parser_item(parser, ":", _func_load_colon);
	parser = roxml_append_parser_item(parser, "", _func_load_default);

	parser = roxml_parser_prepare(parser);

	if (file)
		error = roxml_parse_file(&context, parser, file);
	else
		error = roxml_parse_buff(&context, parser, buffer);

	roxml_parser_free(parser);

	if (context.empty_text_node == 1)
		roxml_free_node(context.candidat_txt);
	current_node = NULL;

	if (error == 0) {
		current_node = roxml_create_root(root);
	} else {
		roxml_close(root);
		current_node = ROXML_INVALID_DOC;
	}

	return current_node;
}

ROXML_INT node_t *roxml_create_root(node_t *n)
{
	xpath_tok_table_t *table = (xpath_tok_table_t *)calloc(1, sizeof(xpath_tok_table_t));

	table->id = ROXML_REQTABLE_ID;
	table->ids[ROXML_REQTABLE_ID] = 1;

	n->priv = (void *)table;

	roxml_lock_init(n);

	return n;
}

/** \brief name space lookup in list
 *
 * \fn roxml_lookup_nsdef(node_t *nsdef, char * ns);
 * this function look for requested name space in nsdef list
 * \param nsdef the nsdef list
 * \param ns the namespace to find
 * \return the nsdef node or NULL
 */
ROXML_STATIC ROXML_INT node_t *roxml_lookup_nsdef(node_t *nsdef, char *ns)
{
	int len = 0;
	char namespace[MAX_NAME_LEN];

	for (len = 0; ns[len] != '\0' && ns[len] != ':' && len < MAX_NAME_LEN; len++)
		namespace[len] = ns[len];

	if (len == MAX_NAME_LEN)
		return NULL; /* truncated */

	namespace[len] = '\0';

	while (nsdef) {
		if (nsdef->priv && strcmp(namespace, ((roxml_ns_t *)nsdef->priv)->alias) == 0)
			break;
		nsdef = nsdef->next;
	}
	return nsdef;
}

/** \brief node type setter function
 *
 * \fn roxml_set_type(node_t *n, int type);
 * this function change the type of a node
 * \param n the node to modify
 * \param type the new type to set
 * \return
 */
ROXML_STATIC ROXML_INT inline void roxml_set_type(node_t *n, int type)
{
	n->type &= ~(ROXML_ATTR_NODE | ROXML_ELM_NODE | ROXML_TXT_NODE | ROXML_CMT_NODE | ROXML_PI_NODE);
	n->type |= type;
}

ROXML_INT node_t *roxml_set_parent(node_t *parent, node_t *n)
{
	if (parent == NULL)
		return NULL;

	n->prnt = parent;

	if (parent->ns && ((parent->ns->type & ROXML_INVALID) != ROXML_INVALID)
	    && parent->ns->priv && ((roxml_ns_t *)parent->ns->priv)->alias[0] == '\0')
		if (n->ns == NULL)
			n->ns = parent->ns;
	return n;
}

ROXML_STATIC ROXML_INT node_t *roxml_append_attr(node_t *parent, node_t *n)
{
	if (parent->attr) {
		node_t *attr = parent->attr;
		while (attr->sibl)
			attr = attr->sibl;
		attr->sibl = n;
	} else {
		parent->attr = n;
	}

	return n;
}

ROXML_STATIC ROXML_INT node_t *roxml_append_other(node_t *parent, node_t *n)
{
	if (parent->next)
		parent->next->sibl = n;
	else
		parent->chld = n;
	parent->next = n;

	return n;
}

ROXML_INT node_t *roxml_append_node(node_t *parent, node_t *n)
{
	if (!roxml_set_parent(parent, n))
		return n;

	if (n->type & ROXML_ATTR_NODE)
		roxml_append_attr(parent, n);
	else
		roxml_append_other(parent, n);

	return n;
}

ROXML_INT int _func_load_quoted(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;

	if (context->state != STATE_NODE_CONTENT && context->state != STATE_NODE_COMMENT) {
		if (context->mode == MODE_COMMENT_NONE)
			context->mode = MODE_COMMENT_QUOTE;
		else if (context->mode == MODE_COMMENT_QUOTE)
			context->mode = MODE_COMMENT_NONE;
	}

	return 0;
}

ROXML_INT int _func_load_dquoted(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;

	if (context->state != STATE_NODE_CONTENT && context->state != STATE_NODE_COMMENT) {
		if (context->mode == MODE_COMMENT_NONE)
			context->mode = MODE_COMMENT_DQUOTE;
		else if (context->mode == MODE_COMMENT_DQUOTE)
			context->mode = MODE_COMMENT_NONE;
	}

	return 0;
}

ROXML_INT int _func_load_open_node(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	int cur = 1;
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;

	switch (context->state) {
	case STATE_NODE_CDATA:
	case STATE_NODE_COMMENT:
		break;
	case STATE_NODE_ATTR:
	case STATE_NODE_BEG:
		if (context->mode == MODE_COMMENT_NONE)
			ROXML_PARSE_ERROR("unexpected '<' within node definition");
		break;
	default:
		context->state = STATE_NODE_BEG;
		context->previous_state = STATE_NODE_BEG;
		break;
	}

	context->pos += cur;
	return cur;
}

ROXML_INT int _func_load_close_node(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	int cur = 1;
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;

	switch (context->state) {
	case STATE_NODE_NAME:
		context->empty_text_node = 1;
		context->current_node = roxml_append_node(context->current_node, context->candidat_node);
		break;
	case STATE_NODE_ATTR:
		if ((context->mode != MODE_COMMENT_DQUOTE) && (context->mode != MODE_COMMENT_QUOTE)) {
			if (context->inside_node_state == STATE_INSIDE_VAL) {
				node_t *to_be_closed = NULL;
				if (context->content_quoted) {
					context->content_quoted = 0;
					to_be_closed =
					    roxml_create_node(context->pos - 1, context->src,
							      ROXML_ATTR_NODE | context->type);
				} else {
					to_be_closed =
					    roxml_create_node(context->pos, context->src,
							      ROXML_ATTR_NODE | context->type);
				}
				roxml_close_node(context->candidat_val, to_be_closed);
			}
			context->current_node = roxml_append_node(context->current_node, context->candidat_node);
			context->inside_node_state = STATE_INSIDE_ARG_BEG;
			roxml_process_unaliased_ns(context);
		} else {
			context->pos++;
			return 1;
		}
		break;
	case STATE_NODE_SINGLE:
		if (context->doctype) {
			context->doctype--;
			if (context->doctype > 0) {
				context->pos++;
				return 1;
			}
			context->candidat_node->end = context->pos;
		}
		context->empty_text_node = 1;
		context->current_node = roxml_append_node(context->current_node, context->candidat_node);
		if (context->current_node->prnt != NULL) {
			context->current_node = context->current_node->prnt;
		}
		roxml_process_unaliased_ns(context);
		break;
	case STATE_NODE_END:
		context->empty_text_node = 1;
		roxml_close_node(context->current_node, context->candidat_node);
		context->candidat_node = NULL;
		if (context->current_node->prnt != NULL) {
			context->current_node = context->current_node->prnt;
		}
		break;
	case STATE_NODE_CDATA:
	case STATE_NODE_CONTENT:
	default:
		context->pos++;
		return 1;
		break;
	}

	if (context->candidat_node && context->candidat_node->ns
	    && ((context->candidat_node->ns->type & ROXML_INVALID) == ROXML_INVALID)) {
		roxml_free_node(context->candidat_node->ns);
		context->candidat_node->ns = NULL;
	}

	context->state = STATE_NODE_CONTENT;
	context->previous_state = STATE_NODE_CONTENT;
	context->candidat_txt = roxml_create_node(context->pos + 1, context->src, ROXML_TXT_NODE | context->type);
#ifdef IGNORE_EMPTY_TEXT_NODES
	while (chunk[cur] != '\0') {
		if (chunk[cur] == '<') {
			break;
		} else if (!ROXML_WHITE(chunk[cur])) {
			context->empty_text_node = 0;
			break;
		}
		cur++;
	}
#endif /* IGNORE_EMPTY_TEXT_NODES */
	while ((chunk[cur] != '<') && (chunk[cur] != '\0'))
		cur++;

	context->pos += cur;
	return cur;
}

ROXML_INT int _func_load_open_spec_node(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	int cur = 1;

	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;

	if (context->state == STATE_NODE_BEG) {
		if (strncmp(chunk, "!--", 3) == 0) {
			cur = 3;
			roxml_process_begin_node(context, context->pos - 1);
			roxml_set_type(context->candidat_node, ROXML_CMT_NODE);
			context->state = STATE_NODE_COMMENT;
			while ((chunk[cur] != '-') && (chunk[cur] != '\0'))
				cur++;
		} else if (strncmp(chunk, "![CDATA[", 8) == 0) {
			roxml_process_begin_node(context, context->pos - 1);
			roxml_set_type(context->candidat_node, ROXML_CDATA_NODE);
			context->state = STATE_NODE_CDATA;
			while ((chunk[cur] != '[') && (chunk[cur] != '\0'))
				cur++;
		} else {
			if (context->doctype == 0) {
				roxml_process_begin_node(context, context->pos - 1);
				roxml_set_type(context->candidat_node, ROXML_DOCTYPE_NODE);
			}
			context->state = STATE_NODE_SINGLE;
			context->previous_state = STATE_NODE_SINGLE;
			context->doctype++;
		}
	}

	context->pos += cur;
	return cur;
}

ROXML_INT int _func_load_close_comment(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	int cur = 1;
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;

	if (context->state == STATE_NODE_COMMENT) {
		if (chunk[1] == '-') {
			cur = 2;
			context->state = STATE_NODE_SINGLE;
			context->candidat_node->end = context->pos;
		}
	}

	context->pos += cur;
	return cur;
}

ROXML_INT int _func_load_close_cdata(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	int cur = 1;
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;

	if (context->state == STATE_NODE_CDATA) {
		if (chunk[1] == ']') {
			cur = 2;
			context->state = STATE_NODE_SINGLE;
			context->candidat_node->pos += 9;
			context->candidat_node->end = context->pos;
		}
	}

	context->pos += cur;
	return cur;
}

ROXML_INT int _func_load_close_pi(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	int cur = 1;
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;

	if (context->state == STATE_NODE_BEG) {
		cur = 1;
		context->state = STATE_NODE_PI;
		context->previous_state = STATE_NODE_PI;
		roxml_process_begin_node(context, context->pos - 1);
		roxml_set_type(context->candidat_node, ROXML_PI_NODE);
		//      while((chunk[cur] != '?')&&(chunk[cur] != '\0')) { cur++; }
	} else if (context->state == STATE_NODE_PI) {
		if (context->mode == MODE_COMMENT_NONE) {
			cur = 1;
			context->candidat_node->end = context->pos;
			context->previous_state = STATE_NODE_PI;
			context->state = STATE_NODE_SINGLE;
		}
	}

	context->pos += cur;
	return cur;
}

ROXML_INT int _func_load_end_node(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	int cur = 1;
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;

	switch (context->state) {
	case STATE_NODE_BEG:
		context->lvl--;
		roxml_process_begin_node(context, context->pos - 1);
		context->state = STATE_NODE_END;
		if (context->lvl < 0)
			ROXML_PARSE_ERROR("missing opening node");
		break;
	case STATE_NODE_NAME:
		context->lvl--;
		context->state = STATE_NODE_SINGLE;
		break;
	case STATE_NODE_ATTR:
		if ((context->mode != MODE_COMMENT_DQUOTE) && (context->mode != MODE_COMMENT_QUOTE)) {
			if (context->inside_node_state == STATE_INSIDE_VAL) {
				node_t *to_be_closed = NULL;
				if (context->content_quoted) {
					context->content_quoted = 0;
					to_be_closed =
					    roxml_create_node(context->pos - 1, context->src,
							      ROXML_ATTR_NODE | context->type);
				} else {
					to_be_closed =
					    roxml_create_node(context->pos, context->src,
							      ROXML_ATTR_NODE | context->type);
				}
				roxml_close_node(context->candidat_val, to_be_closed);
			}
			context->inside_node_state = STATE_INSIDE_ARG_BEG;
			context->state = STATE_NODE_SINGLE;
			context->lvl--;
		}
		break;
	}

	context->pos += cur;
	return cur;
}

ROXML_INT int _func_load_white(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	int cur = 1;
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;

	switch (context->state) {
	case STATE_NODE_SINGLE:
		context->state = context->previous_state;
		break;
	case STATE_NODE_NAME:
		context->state = STATE_NODE_ATTR;
		context->inside_node_state = STATE_INSIDE_ARG_BEG;
		break;
	case STATE_NODE_ATTR:
		if (context->mode == MODE_COMMENT_NONE) {
			if (context->inside_node_state == STATE_INSIDE_VAL) {
				node_t *to_be_closed = NULL;
				if (context->content_quoted) {
					context->content_quoted = 0;
					to_be_closed =
					    roxml_create_node(context->pos - 1, context->src,
							      ROXML_ATTR_NODE | context->type);
				} else {
					to_be_closed =
					    roxml_create_node(context->pos, context->src,
							      ROXML_ATTR_NODE | context->type);
				}
				roxml_close_node(context->candidat_val, to_be_closed);
				context->inside_node_state = STATE_INSIDE_ARG_BEG;
				roxml_process_unaliased_ns(context);
			}
		}
		break;
	}
	context->pos += cur;
	return cur;
}

ROXML_INT int _func_load_colon(roxml_parser_item_t *parser, char *chunk, void *data)
{
	int cur = 1;
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */

	if (context->state == STATE_NODE_NAME) {
		context->state = STATE_NODE_BEG;
		context->candidat_node->ns = roxml_lookup_nsdef(context->namespaces, context->curr_name);
		if (!context->candidat_node->ns) {
			char *nsname = malloc(context->curr_name_len + 1);
			memcpy(nsname, context->curr_name, context->curr_name_len);
			nsname[context->curr_name_len] = '\0';
			context->candidat_node->ns =
			    roxml_create_node(0, nsname, ROXML_NSDEF_NODE | ROXML_PENDING | ROXML_INVALID);
		}
		context->candidat_node->pos += context->curr_name_len + 2;
		context->ns = 1;
	} else if (context->state == STATE_NODE_ATTR) {
		if (context->inside_node_state == STATE_INSIDE_ARG) {
			context->inside_node_state = STATE_INSIDE_ARG_BEG;
			if ((context->curr_name_len == 5) && (strncmp(context->curr_name, "xmlns", 5) == 0)) {
				context->candidat_arg->type |= ROXML_NS_NODE;
				context->nsdef = 1;
			} else {
				context->candidat_arg->ns = roxml_lookup_nsdef(context->namespaces, context->curr_name);
				context->candidat_arg->pos += context->curr_name_len + 2;
				context->ns = 1;
			}

		}
	}

	context->pos += cur;
	return cur;
}

ROXML_INT int _func_load_default(roxml_parser_item_t *parser, char *chunk, void *data)
{
	int cur = 1;
	roxml_load_ctx_t *context = (roxml_load_ctx_t *)data;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */

	switch (context->state) {
	case STATE_NODE_SINGLE:
		context->state = context->previous_state;
		break;
	case STATE_NODE_BEG:
		if (context->ns == 0) {
			context->lvl++;
			roxml_process_begin_node(context, context->pos - 1);
		}
		context->ns = 0;
		context->state = STATE_NODE_NAME;
		strncpy(context->curr_name, chunk, MAX_NAME_LEN);
		while (!ROXML_WHITE(chunk[cur]) && (chunk[cur] != '>') && (chunk[cur] != '/') && (chunk[cur] != ':')
		       && (chunk[cur] != '\0')) {
			cur++;
		}
		context->curr_name_len = cur;
		break;
	case STATE_NODE_ATTR:
		if (context->inside_node_state == STATE_INSIDE_ARG_BEG) {
			if (context->nsdef) {
				if (context->namespaces == NULL) {
					context->namespaces = context->candidat_arg;
					context->last_ns = context->candidat_arg;
				} else {
					context->last_ns->next = context->candidat_arg;
					context->last_ns = context->candidat_arg;
				}
			} else if (context->ns == 0) {
				context->candidat_arg =
				    roxml_create_node(context->pos - 1, context->src, ROXML_ATTR_NODE | context->type);
				context->candidat_arg =
				    roxml_append_node(context->candidat_node, context->candidat_arg);
			}
			context->ns = 0;
			context->inside_node_state = STATE_INSIDE_ARG;
			strncpy(context->curr_name, chunk, MAX_NAME_LEN);
			while ((chunk[cur] != '=') && (chunk[cur] != '>') && (chunk[cur] != ':')
			       && (chunk[cur] != '\0')) {
				cur++;
			}
			context->curr_name_len = cur;
			if (context->nsdef) {
				roxml_ns_t *ns = calloc(1, sizeof(roxml_ns_t) + (1 + context->curr_name_len));
				ns->id = ROXML_NS_ID;
				ns->alias = (char *)ns + sizeof(roxml_ns_t);
				memcpy(ns->alias, context->curr_name, context->curr_name_len);
				context->candidat_arg->priv = ns;
				context->nsdef = 0;
				if (context->candidat_node->ns) {
					if ((context->candidat_node->ns->type & ROXML_INVALID) == ROXML_INVALID) {
						if (strcmp(context->candidat_arg->prnt->ns->src.buf, ns->alias) == 0) {
							roxml_free_node(context->candidat_node->ns);
							context->candidat_node->ns = context->candidat_arg;
						}
					}
				}
			}
		} else if (context->inside_node_state == STATE_INSIDE_VAL_BEG) {
			if (context->mode != MODE_COMMENT_NONE) {
				context->content_quoted = 1;
				context->candidat_val =
				    roxml_create_node(context->pos + 1, context->src, ROXML_TXT_NODE | context->type);
			} else {
				context->candidat_val =
				    roxml_create_node(context->pos, context->src, ROXML_TXT_NODE | context->type);
			}
			context->candidat_val = roxml_append_node(context->candidat_arg, context->candidat_val);
			context->inside_node_state = STATE_INSIDE_VAL;
		} else if ((context->inside_node_state == STATE_INSIDE_ARG) && (chunk[0] == '=')) {
			node_t *to_be_closed;

			context->inside_node_state = STATE_INSIDE_VAL_BEG;
			to_be_closed = roxml_create_node(context->pos, context->src, ROXML_ATTR_NODE | context->type);
			roxml_close_node(context->candidat_arg, to_be_closed);
			if ((context->curr_name_len == 5) && (strncmp(context->curr_name, "xmlns", 5) == 0)) {
				context->nsdef = 1;
				if (context->namespaces == NULL) {
					context->namespaces = context->candidat_arg;
					context->last_ns = context->candidat_arg;
				} else {
					context->last_ns->next = context->candidat_arg;
					context->last_ns = context->candidat_arg;
				}
			}
		}
		break;
	}

	context->pos += cur;
	return cur;
}
