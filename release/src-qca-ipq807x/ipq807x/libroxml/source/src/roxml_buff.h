/**
 * \file roxml_buff.h
 * \brief buffer XML document backend
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#ifndef ROXML_BUFF_H
#define ROXML_BUFF_H

/** \brief read xml doc function
 *
 * \fn roxml_read_buff(int pos, int size, char * buffer, node_t *node)
 * this function read inside a xml buffer and fill the given buffer
 * \param pos the pos in the xml document
 * \param size the size of the data to read
 * \param buffer the destination buffer
 * \param node the node that belong to the tree we want to read to
 * \return the number of bytes read
 */
ROXML_INT int roxml_read_buff(int pos, int size, char *buffer, node_t *node);

/** \brief unload function
 *
 * \fn void roxml_close_buff(node_t *n);
 * This function clear a document that was opened as part of a buffer
 * \param n is any node of the tree to be cleaned
 * \return void
 */
ROXML_INT void roxml_close_buff(node_t *n);

/** \brief generic load function
 *
 * \fn node_t* roxml_parse_buff(roxml_load_ctx_t *context, roxml_parser_item_t *parser, char *buffer);
 * This function parse a buffer document and all the corresponding nodes
 * \param context internal data used for parsing
 * \param parser the parser object to run
 * \param buffer address of buffer that contains xml
 * \return an error code and filled context
 * see roxml_close_buff
 */
ROXML_INT int roxml_parse_buff(roxml_load_ctx_t *context, roxml_parser_item_t *parser, char *buffer);

#endif /* ROXML_BUFF_H */

