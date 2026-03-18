/**
 * \file roxml_defines.h
 * \brief Defines used trhoughout the sources
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#ifndef ROXML_DEF_H
#define ROXML_DEF_H

#ifndef ROXML_TUNE_H
#error "roxml_tune.h must be included before roxml_defines.h to tune it."
#endif

#define ROXML_PATH_OR	"|"
#define ROXML_PATH_AND	"&"
#define ROXML_COND_OR	"or"
#define ROXML_COND_AND	"and"

#define ROXML_OPERATOR_OR	1
#define ROXML_OPERATOR_AND	2
#define ROXML_OPERATOR_INF	3
#define ROXML_OPERATOR_SUP	4
#define ROXML_OPERATOR_EINF	5
#define ROXML_OPERATOR_ESUP	6
#define ROXML_OPERATOR_DIFF	7
#define ROXML_OPERATOR_EQU	8
#define ROXML_OPERATOR_ADD	9
#define ROXML_OPERATOR_SUB	10
#define ROXML_OPERATOR_MUL	11
#define ROXML_OPERATOR_DIV	12

#define ROXML_FUNC_INTCOMP	1
#define ROXML_FUNC_STRCOMP	2
#define ROXML_FUNC_POS		3
#define ROXML_FUNC_FIRST	4
#define ROXML_FUNC_LAST		5
#define ROXML_FUNC_TEXT		6
#define ROXML_FUNC_NODE		7
#define ROXML_FUNC_COMMENT	8
#define ROXML_FUNC_PI		9
#define ROXML_FUNC_XPATH	10
#define ROXML_FUNC_NSURI	11
#define ROXML_FUNC_LNAME	12

#define ROXML_FUNC_POS_STR	"position()"
#define ROXML_FUNC_FIRST_STR	"first()"
#define ROXML_FUNC_LAST_STR	"last()"
#define ROXML_FUNC_NSURI_STR	"namespace-uri()"
#define ROXML_FUNC_LNAME_STR	"local-name()"

#define ROXML_FUNC_TEXT_STR	"text()"
#define ROXML_FUNC_NODE_STR	"node()"
#define ROXML_FUNC_COMMENT_STR	"comment()"
#define ROXML_FUNC_PI_STR	"processing-instruction()"

/**
 * \def MAX_NAME_LEN
 *
 * constant for namespace lenght
 */
#define MAX_NAME_LEN		128

/**
 * \def ROXML_BULK_READ
 *
 * This is the internal buffer size for chunk of xml files
 */
#if(CONFIG_XML_SMALL_BUFFER==1)
#define ROXML_BULK_READ		256
#else /* CONFIG_XML_SMALL_BUFFER==0 */
#define ROXML_BULK_READ		4096
#endif /* CONFIG_XML_SMALL_BUFFER */

/**
 * \def ROXML_BASE_LEN
 *
 * This is the max size for XML objects (node name, attribute name and values...)
 * It is caused by the split of the XML file for parsing:
 * 512 bytes are reserved as security to handle splitting inside an XML object.
 * This is also the len of internal buffers. If a requested buffer is bigger,
 * then a malloc/free will occur and may slow down libroxml.
 */
#if(CONFIG_XML_SMALL_BUFFER==1)
#define ROXML_BASE_LEN		64
#else /* CONFIG_XML_SMALL_BUFFER==0 */
#define ROXML_BASE_LEN		512
#endif /* CONFIG_XML_SMALL_BUFFER */

#define ROXML_ID_CHILD		0
#define ROXML_ID_DESC_O_SELF	1
#define ROXML_ID_SELF		2
#define ROXML_ID_PARENT		3
#define ROXML_ID_ATTR		4
#define ROXML_ID_DESC		5
#define ROXML_ID_ANC		6
#define ROXML_ID_NEXT_SIBL	7
#define ROXML_ID_PREV_SIBL	8
#define ROXML_ID_NEXT		9
#define ROXML_ID_PREV		10
#define ROXML_ID_NS		11
#define ROXML_ID_ANC_O_SELF	12

#define ROXML_L_CHILD		"child::"
#define ROXML_L_DESC_O_SELF	"descendant-or-self::"
#define ROXML_L_SELF		"self::"
#define ROXML_L_PARENT		"parent::"
#define ROXML_L_ATTR		"attribute::"
#define ROXML_L_DESC		"descendant::"
#define ROXML_L_ANC		"ancestor::"
#define ROXML_L_NEXT_SIBL	"following-sibling::"
#define ROXML_L_PREV_SIBL	"preceding-sibling::"
#define ROXML_L_NEXT		"following::"
#define ROXML_L_PREV		"preceding::"
#define ROXML_L_NS		"namespace::"
#define ROXML_L_ANC_O_SELF	"ancestor-or-self::"

#define ROXML_S_CHILD
#define ROXML_S_DESC_O_SELF	""
#define ROXML_S_SELF		"."
#define ROXML_S_PARENT		".."
#define ROXML_S_ATTR		"@"

#define ROXML_DIRECT		0
#define ROXML_DESC_ONLY		1
#define ROXML_DESC_O_SELF	2

#define ROXML_REQTABLE_ID	0
#define ROXML_NS_ID		1
#define ROXML_XPATH_FIRST_ID	16

/**
 * \def PTR_NONE
 *
 * constant for void pointers
 */
#define PTR_NONE	-1

/**
 * \def PTR_VOID
 *
 * constant for void pointers
 */
#define PTR_VOID	0

/**
 * \def PTR_CHAR
 *
 * constant for char pointers
 */
#define PTR_CHAR	2

/**
 * \def PTR_CHAR_STAR
 *
 * constant for char table pointers
 */
#define PTR_CHAR_STAR	3

/**
 * \def PTR_NODE
 *
 * constant for node pointers
 */
#define PTR_NODE	4

/**
 * \def PTR_NODE_STAR
 *
 * constant for node table pointers
 */
#define PTR_NODE_STAR	5

/**
 * \def PTR_INT
 *
 * constant for int pointer
 */
#define PTR_INT	6

/**
 * \def PTR_INT_STAR
 *
 * constant for int table pointers
 */
#define PTR_INT_STAR	7

/**
 * \def PTR_NODE_RESULT
 *
 * constant for node table pointers where node are not to delete
 */
#define PTR_NODE_RESULT	8

/**
 * \def PTR_IS_STAR(a)
 *
 * macro returning if a memory_cell is a star cell
 */
#define PTR_IS_STAR(a)	((a)->type % 2)

/**
 * \def ROXML_FILE
 *
 * constant for argument node
 */
#define ROXML_FILE	0x01

/**
 * \def ROXML_BUFF
 *
 * constant for buffer document
 */
#define ROXML_BUFF	0x02

/**
 * \def ROXML_PENDING
 *
 * constant for pending node
 */
#define ROXML_PENDING	0x04

/**
 * \def ROXML_INVALID
 *
 * constant for invalid node
 */
#define ROXML_INVALID	0x03

/**
 * \def STATE_NODE_NONE
 *
 * state for the state machine for init
 */
#define STATE_NODE_NONE		0

/**
 * \def STATE_NODE_BEG
 *
 * state for the state machine for begining of a node
 */
#define STATE_NODE_BEG		1

/**
 * \def STATE_NODE_NAME
 *
 * state for the state machine for name read
 */
#define STATE_NODE_NAME		2

/**
 * \def STATE_NODE_END
 *
 * state for the state machine for end of node
 */
#define STATE_NODE_END		3

/**
 * \def STATE_NODE_STRING
 *
 * state for the state machine for string reading
 */
#define STATE_NODE_STRING	4

/**
 * \def STATE_NODE_ARG
 *
 * state for the state machine for attribute name reading
 */
#define STATE_NODE_ARG		5

/**
 * \def STATE_NODE_ARGVAL
 *
 * state for the state machine for attribute value reading
 */
#define STATE_NODE_ARGVAL	6

/**
 * \def STATE_NODE_SEP
 *
 * state for the state machine for separator reading
 */
#define STATE_NODE_SEP		7

/**
 * \def STATE_NODE_PI
 *
 * state for the state machine for separator reading
 */
#define STATE_NODE_PI		8

/**
 * \def STATE_NODE_SINGLE
 *
 * state for the state machine for single nodes
 */
#define STATE_NODE_SINGLE	9

/**
 * \def STATE_NODE_ATTR
 *
 * state for the state machine for attribut reading
 */
#define STATE_NODE_ATTR		10

/**
 * \def STATE_NODE_CONTENT
 *
 * state for the state machine for content read
 */
#define STATE_NODE_CONTENT	11

/**
 * \def STATE_NODE_COMMENT_BEG
 *
 * state for the state machine for separator reading
 */
#define STATE_NODE_COMMENT_BEG		12

/**
 * \def STATE_NODE_COMMENT
 *
 * state for the state machine for separator reading
 */
#define STATE_NODE_COMMENT		14

/**
 * \def STATE_NODE_COMMENT_END
 *
 * state for the state machine for separator reading
 */
#define STATE_NODE_COMMENT_END		15

/**
 * \def STATE_NODE_CDATA_BEG
 *
 * state for the state machine for separator reading
 */
#define STATE_NODE_CDATA_BEG		16

/**
 * \def STATE_NODE_CDATA
 *
 * state for the state machine for separator reading
 */
#define STATE_NODE_CDATA		22

/**
 * \def STATE_NODE_CDATA_END
 *
 * state for the state machine for separator reading
 */
#define STATE_NODE_CDATA_END		23

/**
 * \def MODE_COMMENT_NONE
 *
 * mode init in state machine
 */
#define MODE_COMMENT_NONE	0

/**
 * \def MODE_COMMENT_QUOTE
 *
 * mode quoted in state machine
 */
#define MODE_COMMENT_QUOTE	1

/**
 * \def MODE_COMMENT_DQUOTE
 *
 * mode double quoted in state machine
 */
#define MODE_COMMENT_DQUOTE	2

/**
 * \def STATE_INSIDE_ARG_BEG
 *
 * inside node state begining (attribute declaration)
 */
#define STATE_INSIDE_ARG_BEG	0

/**
 * \def STATE_INSIDE_ARG
 *
 * inside node state arg name
 */
#define STATE_INSIDE_ARG	1

/**
 * \def STATE_INSIDE_VAL_BEG
 *
 * inside node state arg value
 */
#define STATE_INSIDE_VAL_BEG	2

/**
 * \def STATE_INSIDE_VAL
 *
 * inside node state arg value
 */
#define STATE_INSIDE_VAL	3

/**
 * \def ROXML_WHITE(n)
 *
 * save current document position and recall to node
 */
#define ROXML_WHITE(n) ((n==' ')||(n=='\t')||(n=='\n')||(n=='\r'))

#endif /* ROXML_DEF_H */
