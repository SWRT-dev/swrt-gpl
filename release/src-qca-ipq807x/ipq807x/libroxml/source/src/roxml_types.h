/**
 * \file roxml_types.c
 * \brief Type definition for libroxml
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#ifndef ROXML_TYPES_H
#define ROXML_TYPES_H

#include <stdio.h>
#include <roxml_defines.h>

/** \typedef roxml_pos_t
 *
 * \brief type for node indexes in raw tree
 *
 * this is the type used for node indexes in raw tree.
 * this type induce a maximum size for XML document
 * that can be handled (both from file or buffer)
 */
#if(CONFIG_XML_SMALL_INPUT_FILE==1)
typedef unsigned short roxml_pos_t;
#else /* CONFIG_XML_SMALL_INPUT_FILE==0 */
typedef unsigned int roxml_pos_t;
#endif /* CONFIG_XML_SMALL_INPUT_FILE */

/** \struct memory_cell_t
 *
 * \brief memory cell structure
 *
 * This is the structure for a memory cell. It contains the
 * pointer info and type. It also contains the caller id so that
 * it can free without reference to a specific pointer
 */
typedef struct memory_cell {
	int type;		/*!< pointer type from PTR_NODE, PTR_CHAR... */
	int occ;		/*!< number of element */
	void *ptr;		/*!< pointer */
	unsigned long int id;	/*!< thread id of allocator */
	struct memory_cell *next;	/*!< next memory cell */
	struct memory_cell *prev;	/*!< prev memory cell */
} memory_cell_t;

/** \struct xpath_cond_t
 *
 * \brief xpath cond structure
 *
 * This is the structure for a xpath cond. It contains the
 * node condition
 */
typedef struct _xpath_cond {
	char rel;		/*!< relation with previous */
	char axes;		/*!< axes for operator */
	char op;		/*!< operator used */
	char op2;		/*!< operator used on arg2 */
	char func;		/*!< function to process */
	char func2;		/*!< function to process in arg2 */
	char *arg1;		/*!< condition arg1 as string */
	char *arg2;		/*!< condition arg2 as string */
	struct _xpath_node *xp;	/*!< xpath that have to be resolved for condition */
	struct _xpath_cond *next;	/*!< next xpath condition pointer */
} xpath_cond_t;

/** \struct xpath_node_t
 *
 * \brief xpath node structure
 *
 * This is the structure for a xpath node. It contains the
 * node axes and conditions
 */
typedef struct _xpath_node {
	char abs;		/*!< for first node: is path absolute */
	char rel;		/*!< relation with previous */
	char axes;		/*!< axes type */
	char *name;		/*!< axes string */
	struct _xpath_cond *xp_cond;	/*!< condition for total xpath */
	struct _xpath_cond *cond;	/*!< condition list */
	struct _xpath_node *next;	/*!< next xpath pointer */
} xpath_node_t;

/** \struct xpath_tok_table_t
 *
 * \brief xpath token structure
 *
 * This is the structure for a xpath token. It contains the
 * xpath id
 */
typedef struct _xpath_tok_table {
	unsigned char id;	/*!< token id == ROXML_REQTABLE_ID */
	unsigned char ids[256];	/*!< token id table */
	void *lock;		/*!< token table allocation mutex */
	struct _xpath_tok *next;	/*!< next xpath token */
} xpath_tok_table_t;

/** \struct xpath_tok_t
 *
 * \brief xpath token structure
 *
 * This is the structure for a xpath token. It contains the
 * xpath id
 */
typedef struct _xpath_tok {
	unsigned char id;	/*!< token id starts at ROXML_XPATH_FIRST_ID */
	struct _xpath_tok *next;	/*!< next xpath token */
} xpath_tok_t;

/** \struct roxml_ns_t
 *
 * \brief namespace structure
 *
 * This is the structure for a namespace. It contains the
 * namespace alias
 */
typedef struct _roxml_ns {
	unsigned char id;	/*!< priv id == ROXML_NS_ID */
	void *next;		/*!< one other priv data is following */
	char *alias;		/*!< ns alias */
} roxml_ns_t;

/** \struct node_t
 *
 * \brief node_t structure
 *
 * This is the structure for a node. This struct is very
 * little as it only contains offset for node in file and
 * tree links
 */
typedef struct node {
	unsigned short type;	/*!< document or buffer / attribute or value */
	union {
		char *buf;	/*!< buffer address */
		FILE *fil;	/*!< loaded document */
		void *src;	/*!< xml src address */
	} src;			/*!< xml tree source */
	roxml_pos_t pos;	/*!< offset of begining of opening node in file */
	roxml_pos_t end;	/*!< offset of begining of closing node in file */
	struct node *sibl;	/*!< ref to brother */
	struct node *chld;	/*!< ref to chld */
	struct node *prnt;	/*!< ref to parent */
	struct node *attr;	/*!< ref to attribute */
	struct node *next;	/*!< ref to last chld (internal use) */
	struct node *ns;	/*!< ref to namespace definition */
	void *priv;		/*!< ref to xpath tok (internal use) or alias for namespaces */
} node_t;

/** \struct roxml_load_ctx_t
 *
 * \brief xml parsing context
 *
 * obscure structure that contains all the xml
 * parsing variables
 */
typedef struct _roxml_load_ctx {
	int pos;			/*!< position in file */
	int lvl;			/*!< nested elem count */
	int empty_text_node;		/*!< if text node is empty (only contains tabs, spaces, carriage return and line feed */
	int state;			/*!< state (state machine main var) */
	int previous_state;		/*!< previous state */
	int mode;			/*!< mode quoted or normal */
	int inside_node_state;		/*!< sub state for attributes */
	int content_quoted;		/*!< content of attribute was quoted */
	int type;			/*!< source type (file or buffer) */
	int nsdef;			/*!< indicate if this is a nsdef */
	int ns;				/*!< indicate if a ns is used for this node */
	void *src;			/*!< source (file or buffer) */
	node_t *candidat_node;		/*!< node being processed */
	node_t *candidat_txt;		/*!< text node being processed */
	node_t *candidat_arg;		/*!< attr node being processed */
	node_t *candidat_val;		/*!< attr value being processed */
	node_t *current_node;		/*!< current node */
	node_t *namespaces;		/*!< available namespaces */
	node_t *last_ns;		/*!< last declared namespaces */
	char curr_name[MAX_NAME_LEN+1];	/*!< current node name (attr or elm) */
	int curr_name_len;		/*!< current node name (attr or elm) lenght */
	int doctype;			/*!< nested doctype count */
} roxml_load_ctx_t;

/** \struct roxml_xpath_ctx_t
 * \brief xpath parsing context
 *
 * obscure structure that contains all the xapth
 * parsing variables
 */
typedef struct _roxml_xpath_ctx {
	int pos;		/*!< position in string */
	int is_first_node;	/*!< is it the first node of xpath */
	int wait_first_node;	/*!< are we waiting for the first node of a xpath */
	int shorten_cond;	/*!< is the cond a short condition */
	int nbpath;		/*!< number of xpath in this string */
	int bracket;		/*!< are we inside two brackets */
	int parenthesys;	/*!< are we inside two parenthesys */
	int quoted;		/*!< are we quoted (') */
	int dquoted;		/*!< are we double quoted (") */
	int context;		/*!< is it an inside xpath */
	int content_quoted;	/*!< content of val was quoted */
	xpath_node_t *first_node;	/*!< the very first node of xpath string */
	xpath_node_t *new_node;	/*!< current xpath node */
	xpath_cond_t *new_cond;	/*!< current xpath cond */
} roxml_xpath_ctx_t;

/** \typedef roxml_parse_func
 *
 * \brief parser callback functions
 *
 * This is the prototype for a parser callback function. It receive as argument
 * the chunk that matched the key, and the context as a void. It should return the
 * number of handled bytes or 0 if doesn't want to handle this key
 */
typedef struct _roxml_parser_item roxml_parser_item_t;
typedef int (*roxml_parse_func) (roxml_parser_item_t *parser, char *chunk, void *data);

/** \struct roxml_parser_item_t
 *
 * \brief the parser item struct
 *
 * this struct contains the key and callback.
 */
struct _roxml_parser_item {
	roxml_parse_func func;			/*!< callback function */
	struct _roxml_parser_item *next;	/*!< next item (default or multiple) */
};

#endif /* ROXML_TYPES_H */
