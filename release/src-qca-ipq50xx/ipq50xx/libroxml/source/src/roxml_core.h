/**
 * \file roxml_core.h
 * \brief XML parsing core module
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#ifndef ROXML_CORE_H
#define ROXML_CORE_H

#include "roxml_internal.h"

#ifdef __DEBUG
unsigned int _nb_node;
unsigned int _nb_attr;
unsigned int _nb_text;
#endif

/** \brief internal function
 *
 * \fn node_t* ROXML_INT roxml_create_node(int pos, void * src, int type);
 * This function allocate a new node
 * \param pos is the beginning offset of the node in the file
 * \param src is the pointer to the buffer or file
 * \param type is the type of node between arg and val
 * \return the new node
 */
ROXML_INT node_t *roxml_create_node(int pos, void *src, int type);

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_free_node(node_t *n);
 * This function delete a node without handling its tree
 * \param n is one node of the tree
 * \return void
 */
ROXML_INT void roxml_free_node(node_t *n);

/** \brief internal function
 *
 * \fn void ROXML_INT roxml_del_tree(node_t *n);
 * This function delete a tree recursively
 * \param n is one node of the tree
 * \return void
 * see roxml_close
 */
ROXML_INT void roxml_del_tree(node_t *n);

/** \brief internal function
 *
 * \fn ROXML_INT node_t *roxml_create_root(node_t *n);
 * This function initialize the root node with xpath table
 * \param n is the node to initialize
 * \return void
 * see roxml_close
 */
ROXML_INT node_t *roxml_create_root(node_t *n);

/** \brief separator tester
 *
 * \fn roxml_is_separator(char sep);
 * This function tells if a char is a string separator
 * \param sep char to test
 * \return 1 if the char was a separator else 0
 */
ROXML_INT int roxml_is_separator(char sep);

/** \brief generic load function
 *
 * \fn node_t*  roxml_load(node_t *current_node, FILE *file, char *buffer);
 * This function load a document and all the corresponding nodes
 * file and buffer params are exclusive. You usualy want to load
 * either a file OR a buffer
 * \param current_node the XML root
 * \param file file descriptor of document
 * \param buffer address of buffer that contains xml
 * \return the root node or NULL
 * see roxml_close
 */
ROXML_INT node_t *roxml_load(node_t *current_node, FILE * file, char *buffer);

/** \brief node parenting function
 *
 * \fn node_t *roxml_set_parent(node_t *parent, node_t *n);
 * this function updates a node to set its parent and namespace if
 * necessary.
 * \param parent the parent node
 * \param n the node to parent
 * \return
 */
ROXML_INT node_t *roxml_set_parent(node_t *parent, node_t *n);

/** \brief node append function
 *
 * \fn roxml_append_node(node_t *parent, node_t *n);
 * this function add a given node as the last child of another node
 * \param parent the parent node
 * \param n the node to parent
 * \return
 */
ROXML_INT node_t *roxml_append_node(node_t *parent, node_t * n);

ROXML_INT ROXML_PARSE int _func_load_quoted(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_dquoted(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_open_node(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_close_node(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_open_spec_node(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_close_comment(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_close_cdata(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_close_pi(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_end_node(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_white(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_colon(roxml_parser_item_t *parser, char *chunk, void *data);
ROXML_INT ROXML_PARSE int _func_load_default(roxml_parser_item_t *parser, char *chunk, void *data);

#endif /* ROXML_CORE_H */
