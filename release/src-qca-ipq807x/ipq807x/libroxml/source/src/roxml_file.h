/**
 * \file roxml_file.h
 * \brief File XML document backend
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#ifndef ROXML_FILE_H
#define ROXML_FILE_H

/** \brief read xml doc function
 *
 * \fn roxml_read_file(int pos, int size, char * buffer, node_t *node)
 * this function read inside a xml file and fill the given buffer
 * \param pos the pos in the xml document
 * \param size the size of the data to read
 * \param buffer the destination buffer
 * \param node the node that belong to the tree we want to read to
 * \return the number of bytes read
 */
ROXML_INT int roxml_read_file(int pos, int size, char *buffer, node_t *node);

/** \brief unload function
 *
 * \fn void roxml_close_file(node_t *n);
 * This function clear a document that was opened as part of a file
 * \param n is any node of the tree to be cleaned
 * \return void
 */
ROXML_INT void roxml_close_file(node_t *root);

/** \brief generic load function
 *
 * \fn node_t* roxml_parse_file(roxml_load_ctx_t *context, roxml_parser_item_t *parser, FILE *file);
 * This function parse a file document and all the corresponding nodes
 * \param context internal data used for parsing
 * \param parser the parser object to run
 * \param file file pointer to the document
 * \return an error code and filled context
 * see roxml_close_buff
 */
ROXML_INT int roxml_parse_file(roxml_load_ctx_t *context, roxml_parser_item_t *parser, FILE *file);

#endif /* ROXML_FILE_H */
