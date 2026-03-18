/**
 * \file roxml_parser.h
 * \brief Parsing engine
 *
 * (c) copyright 2014
 * tristan lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     lgpl-2.1+
 * the author added a static linking exception, see license.txt.
 */

#ifndef ROXML_PARSER_H
#define ROXML_PARSER_H

#include "roxml_internal.h"

/** \brief parser item creation function
 *
 * \fn roxml_append_parser_item(roxml_parser_item_t *parser, char * key, roxml_parse_func func);
 * this function create a new parser item and append it to the parser list.
 * \param parser the parser parser list or NULL for first item
 * \param key the char to trigger callback for
 * \param func the function to call on match
 * \return the parser item
 */
ROXML_INT roxml_parser_item_t *roxml_append_parser_item(roxml_parser_item_t * parser, char *key, roxml_parse_func func);

/** \brief parser table allocation
 *
 * \fn roxml_parser_allocate(void);
 * this function allocate an empty parser object
 * \return the allocated parser
 */
ROXML_INT roxml_parser_item_t *roxml_parser_allocate(void);

/** \brief parser table deletion
 *
 * \fn roxml_parser_free(roxml_parser_item_t *parser);
 * this function delete a prepared parser object
 * \param parser the parser object
 * \return
 */
ROXML_INT void roxml_parser_free(roxml_parser_item_t * parser);

/** \brief parser preparation function
 *
 * \fn roxml_parser_prepare(roxml_parser_item_t *parser);
 * this function compile a parser list into a table and calculate count variables for parsing optim
 * \param parser the parser object
 * \return
 */
ROXML_INT roxml_parser_item_t *roxml_parser_prepare(roxml_parser_item_t * parser);

/** \brief line parsing function
 *
 * \fn roxml_parse_line(roxml_parser_item_t * parser, char *line, int len, void * ctx);
 * this function parse a line : it calls parsing functions when key matches
 * \param parser the parser object
 * \param line the string to parse
 * \param len the len of string or 0 if auto calculate len (using \0)
 * \param ctx user data passed to the callbacks
 * \return the number of bytes processed
 */
ROXML_INT ROXML_PARSE int roxml_parse_line(roxml_parser_item_t * parser, char *line, int len, void *ctx);

#endif /* ROXML_PARSER_H */
