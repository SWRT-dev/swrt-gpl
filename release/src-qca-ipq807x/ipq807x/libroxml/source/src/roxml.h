/**
 * \file roxml.h
 * \brief header for libroxml.so
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:     LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#ifndef ROXML_H
#define ROXML_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \def ROXML_API
 *
 * part of the public API
 */
#define ROXML_API

#ifndef ROXML_INT
/** \typedef node_t
 *
 * \brief node_t structure
 *
 * This is the structure for a node. This struct is very
 * little as it only contains offset for node in file and
 * tree links
 */
typedef struct node node_t;
#endif

/**
 * \def ROXML_INVALID_NODE
 *
 * constant for invalid nodes
 */
#define ROXML_INVALID_NODE	0x000

/**
 * \def ROXML_ATTR_NODE
 *
 * constant for attribute nodes
 * \see roxml_add_node
 */
#define ROXML_ATTR_NODE	0x008

/**
 * \def ROXML_STD_NODE
 *
 * \deprecated
 * constant for standard nodes
 *
 * \see roxml_add_node
 *
 */
#define ROXML_STD_NODE	0x010

/**
 * \def ROXML_ELM_NODE
 *
 * constant for element nodes
 * \see roxml_add_node
 */
#define ROXML_ELM_NODE	0x010

/**
 * \def ROXML_TXT_NODE
 *
 * constant for text nodes
 * \see roxml_add_node
 */
#define ROXML_TXT_NODE	0x020

/**
 * \def ROXML_CMT_NODE
 *
 * constant for comment nodes
 * \see roxml_add_node
 */
#define ROXML_CMT_NODE	0x040

/**
 * \def ROXML_PI_NODE
 *
 * constant for processing_intruction nodes
 * \see roxml_add_node
 */
#define ROXML_PI_NODE	0x080

/**
 * \def ROXML_NS_NODE
 *
 * constant for namespace nodes
 * \see roxml_add_node
 */
#define ROXML_NS_NODE	0x100

/**
 * \def ROXML_NSDEF_NODE
 *
 * constant for namespace definition nodes
 * \see roxml_add_node
 */
#define ROXML_NSDEF_NODE	(ROXML_NS_NODE | ROXML_ATTR_NODE)

/**
 * \def ROXML_CDATA_MOD
 *
 * constant for cdata nodes modifier applied to ROXML_TXT_NODE.
 * \see roxml_add_node
 */
#define ROXML_CDATA_MOD		0x200

/**
 * \def ROXML_CDATA_NODE
 *
 * constant for cdata nodes
 * \see roxml_add_node
 */
#define ROXML_CDATA_NODE	(ROXML_TXT_NODE | ROXML_CDATA_MOD)

/**
 * \def ROXML_DOCTYPE_NODE
 *
 * constant for doctype nodes
 * \see roxml_add_node
 */
#define ROXML_DOCTYPE_NODE	0x400

/**
 * \def ROXML_ALL_NODES
 *
 * constant for all types of nodes
 * \see roxml_add_node
 */
#define ROXML_ALL_NODES	(ROXML_PI_NODE | ROXML_CMT_NODE | ROXML_TXT_NODE | ROXML_ATTR_NODE | ROXML_ELM_NODE)

/**
 * \def ROXML_ALL_NODE
 *
 * constant for all types of nodes for backward compatibility
 * \see roxml_add_node
 */
#define ROXML_ALL_NODE	ROXML_ALL_NODES

/**
 * \def ROXML_NODE_TYPES
 *
 * constant for all nodes types
 * \see roxml_get_types
 */
#define ROXML_NODE_TYPES	0x05f8

/**
 * \def ROXML_ESCAPED_MOD
 *
 * constant for node modifier use to indicate escaped text.
 * \see roxml_add_node
 */
#define ROXML_ESCAPED_MOD 0x800

/**
 * \def ROXML_NON_ESCAPABLE_NODES
 *
 * constant for nodes that should not be escaped.
 * \see roxml_add_node
 */
#define ROXML_NON_ESCAPABLE_NODES (ROXML_CMT_NODE | ROXML_PI_NODE | ROXML_NS_NODE | ROXML_CDATA_MOD | ROXML_DOCTYPE_NODE)

/**
 * \def ENCODE
 *
 * constant for requesting escape of a string.
 * \see roxml_escape
 */
#define ENCODE 0

/**
 * \def DECODE
 *
 * constant for requesting unescape of a string.
 * \see roxml_escape
 */
#define DECODE 1

/**
 * \def RELEASE_ALL
 *
 * when used with roxml_release, release all memory allocated by current thread
 * \see roxml_release
 */
#define RELEASE_ALL	(void*)-1

/**
 * \def RELEASE_LAST
 *
 * when used with roxml_release, release last variable allocated
 * \see roxml_release
 *
 * example:
 * \code
 * #include <stdio.h>
 * #include <roxml.h>
 * int main(void)
 * {
 *	int len;
 * 	node_t *root = roxml_load_doc("/tmp/doc.xml");
 *
 *	// roxml_get_content allocate a buffer and store the content in it if no buffer was given
 *	printf("root content = '%s'\n", roxml_get_content(root, NULL, 0, &len));
 *
 *	// release the last allocated buffer even if no pointer is maintained by the user
 *	roxml_release(RELEASE_LAST);
 *
 *	// here no memory leak can occur.
 *
 *	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 */
#define RELEASE_LAST	(void*)-2

/**
 * \def ROXML_INVALID_DOC
 *
 * constant for invalid documents
 */
#define ROXML_INVALID_DOC	(node_t*)0

/** \brief load function for buffers
 *
 * \fn node_t*  roxml_load_buf(char *buffer);
 * This function load a document by parsing all the corresponding nodes.
 * The document must be contained inside the char * buffer given in parameter
 * and remain valid until the roxml_close() function is called
 * \param buffer the XML buffer to load
 * \return the root node or ROXML_INVALID_DOC (NULL). errno is set to EINVAL in case of parsing error
 * \see roxml_close
 * \see roxml_load_fd
 * \see roxml_load_doc
 */
ROXML_API node_t * roxml_load_buf(char *buffer);

/** \brief load function for files
 *
 * \fn node_t*  roxml_load_doc(char *filename);
 * This function load a file document by parsing all the corresponding nodes
 * \warning the file is not fully copied and thus, it should stay untouched until roxml_close is called on the corresponding XML tree.
 * \param filename the XML document to load
 * \return the root node or ROXML_INVALID_DOC (NULL). errno is set to EINVAL in case of parsing error
 * \see roxml_close
 * \see roxml_load_fd
 * \see roxml_load_buf
 */
ROXML_API node_t * roxml_load_doc(char *filename);

/** \brief load function for file descriptors
 *
 * \fn node_t*  roxml_load_fd(int fd);
 * This function load a document by parsing all the corresponding nodes
 * \param fd the opened file descriptor to XML document to load
 * \return the root node or ROXML_INVALID_DOC (NULL). errno is set to EINVAL in case of parsing error
 * \see roxml_close
 * \see roxml_load_doc
 * \see roxml_load_buf
 */
ROXML_API node_t * roxml_load_fd(int fd);

/** \brief unload function
 *
 * \fn void  roxml_close(node_t *n);
 * This function clear a document and all the corresponding nodes
 * It release all memory allocated during roxml_load_doc or roxml_load_file or roxml_add_node.
 * All nodes from the tree are not available anymore.
 * \param n is any node of the tree to be cleaned
 * \return void
 * \see roxml_load_doc
 * \see roxml_load_buf
 * \see roxml_add_node
 */
ROXML_API void  roxml_close(node_t *n);

/** \brief next sibling getter function
 *
 * \fn node_t*  roxml_get_next_sibling(node_t *n);
 * This function returns the next sibling of a given node
 * \param n is one node of the tree
 * \return the next sibling node
 */
ROXML_API node_t * roxml_get_next_sibling(node_t *n);

/** \brief prev sibling getter function
 *
 * \fn node_t*  roxml_get_prev_sibling(node_t *n);
 * This function returns the prev sibling of a given node
 * \param n is one node of the tree
 * \return the prev sibling node
 */
ROXML_API node_t * roxml_get_prev_sibling(node_t *n);

/** \brief parent getter function
 *
 * \fn node_t*  roxml_get_parent(node_t *n);
 * This function returns the parent of a given node
 * \param n is one node of the tree
 * \return the parent node
 */
ROXML_API node_t * roxml_get_parent(node_t *n);

/** \brief root getter function
 *
 * \fn node_t*  roxml_get_root(node_t *n);
 * This function returns the root of a tree containing the given node
 * The root is defined as a virtual node that contains all first rank nodes if document is
 * not a valid xml document:
 * \verbatim
<data1>
 <item/>
 <item/>
</data1>
<data2>
 <item/>
 <item/>
</data2>
\endverbatim
 * will be processed successfully and the root node will have 2 children: data1 and data2
 *
 * if document was:
 * \verbatim
<?xml version="1.0">
<doc>
 <data1>
  <item/>
  <item/>
 </data1>
 <data2>
  <item/>
  <item/>
 </data2>
</doc>
\endverbatim
 * In this case, the node "doc" will be the root, and will contain 2 children: data1 and data2
 *
 * For a document to be valid, following conditions must be met:
 * <ul>
 * <li>the first node is a processing instruction with the string "xml" as target application.</li>
 * <li>there is only one ELM node containing all document (but there may be several process-instructions or comments)</li>
 * </ul>
 * \param n is one node of the tree
 * \return the root node
 */
ROXML_API node_t * roxml_get_root(node_t *n);

/** \brief namespace getter function
 *
 * \fn node_t*  roxml_get_ns(node_t *n);
 * This function returns the namespace of a node
 *
 * \param n is one node of the tree
 * \return the namespace or NULL if none are set for this node
 * \see roxml_add_node
 * \see roxml_set_ns
 * \see roxml_get_nodes
 *
 * example:
 * given the following xml file
 * \verbatim
<xml xmlns:test="http://www.test.org">
 <test:item1 test:value1="3"/>
</xml>
\endverbatim
 * \code
 * #include <stdio.h>
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_load_doc("/tmp/doc.xml");
 * 	node_t *xml =  roxml_get_chld(root, NULL,  0);
 *	node_t *nsdef = roxml_get_attr(xml, NULL, 0);
 * 	node_t *node1 = roxml_get_chld(xml, NULL, 0);
 * 	node_t *attr1 = roxml_get_attr(node1, NULL, 0);
 * 	node_t *node1_ns = roxml_get_ns(node1);
 * 	node_t *attr1_ns = roxml_get_ns(attr1);
 *
 * 	// here node1_ns and attr1_ns are the "test:" namespace
 * 	if(node1_ns == nsdef) {
 * 		printf("got the correct namespace node for elem\n");
 *	}
 * 	if(attr1_ns == nsdef) {
 * 		printf("got the correct namespace node for attr\n");
 *	}
 * 	if(strcmp(roxml_get_name(node1_ns, NULL, 0), "test") == 0) {
 * 		printf("got the correct namespace alias\n");
 * 	}
 * 	if(strcmp(roxml_get_content(node1_ns, NULL, 0, NULL), "http://www.test.org") == 0) {
 * 		printf("got the correct namespace\n");
 * 	}
 *
 * 	roxml_close(root);
 * 	return 0;
 * }
 *
 * \endcode
 */
ROXML_API node_t * roxml_get_ns(node_t *n);

/** \brief namespace setter function
 *
 * \fn node_t*  roxml_set_ns(node_t *n, node_t *ns);
 * This function set the namespace of a node to the given namespace definition.
 * The namespace must be previously defined in the xml tree in an ancestor of node n.
 *
 * \param n is one node of the tree
 * \param ns is one nsdef node of the tree
 * \return the node or ROXML_INVALID_DOC (NULL) if ns cannot be set
 * \see roxml_add_node
 * \see roxml_get_ns
 * \see roxml_get_nodes
 *
 * \warning: Setting a namespace to a node is recursif:
 * - it will update all element and attribute that are descendant from current node
 * - namespace will be applied to all new node added as descendant as current node
 */
ROXML_API node_t * roxml_set_ns(node_t *n, node_t * ns);

/** \brief comment getter function
 *
 * \fn node_t*  roxml_get_cmt(node_t *n, int nth);
 * This function returns the nth comment of a node
 *
 * \param n is one node of the tree
 * \param nth is the id of the cmt to get
 * \return the comment corresponding to id
 * \see roxml_get_cmt_nb
 * \see roxml_get_nodes
 *
 * example:
 * given the following xml file
 * \verbatim
<xml>
 <item1/>
 <!--comment1-->
 <!--comment2-->
 <item2/>
 <item3/>
</xml>
\endverbatim
 * \code
 * #include <stdio.h>
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_load_doc("/tmp/doc.xml");
 * 	node_t *xml =  roxml_get_chld(root, NULL,  0);
 * 	node_t *cmt1 = roxml_get_cmt(xml, 0);
 * 	node_t *cmt2 = roxml_get_cmt(xml, 1);
 *
 * 	// here cmt1 is the "comment1" node
 * 	if(strcmp(roxml_get_content(cmt1, NULL, 0, NULL), "comment1") == 0) {
 * 		printf("got the first comment\n");
 * 	}
 * 	// and cmt2 is the "comment2" node
 * 	if(strcmp(roxml_get_content(cmt2, NULL, 0, NULL), "comment2") == 0) {
 * 		printf("got the second comment\n");
 * 	}
 *
 * 	roxml_close(root);
 * 	return 0;
 * }
 *
 * \endcode
 */
ROXML_API node_t * roxml_get_cmt(node_t *n, int nth);

/** \brief comments number getter function
 *
 * \fn int  roxml_get_cmt_nb(node_t *n);
 * This function return the number of comments for a given node
 * \param n is one node of the tree
 * \return the number of comments
 * \see roxml_get_cmt_nb
 * \see roxml_get_nodes
 */
ROXML_API int  roxml_get_cmt_nb(node_t *n);

/** \brief chld getter function
 *
 * \fn node_t*  roxml_get_chld(node_t *n, char * name, int nth);
 * This function returns a given chld of a node etheir by name, or the nth child.
 *
 * \param n is one node of the tree
 * \param name is the name of the child to get
 * \param nth is the id of the chld to get
 * \return the chld corresponding to name or id (if both are set, name is used)
 * \see roxml_get_chld_nb
 *
 * example:
 * given the following xml file
 * \verbatim
<xml>
 <item1/>
 <item2/>
 <item3/>
</xml>
\endverbatim
 * \code
 * #include <stdio.h>
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_load_doc("/tmp/doc.xml");
 *
 *	node_t *child_by_name = roxml_get_chld(root, "item2", 0);
 *	node_t *child_by_nth = roxml_get_chld(root, NULL, 2);
 *
 *	// here child_by_name == child_by_nth
 *	if(child_by_name == child_by_nth) {
 *		printf("Nodes are equal\n");
 *	}
 *
 *	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 */
ROXML_API node_t * roxml_get_chld(node_t *n, char *name, int nth);

/** \brief chlds number getter function
 *
 * \fn int  roxml_get_chld_nb(node_t *n);
 * This function return the number of chlidren for a given node
 * \param n is one node of the tree
 * \return  the number of chlildren
 */
ROXML_API int  roxml_get_chld_nb(node_t *n);

/** \brief process-instruction getter function
 *
 * \fn node_t*  roxml_get_pi(node_t *n, int nth);
 * This function returns the nth process-instruction of a node.
 *
 * \param n is one node of the tree
 * \param nth is the id of the pi to get
 * \return the process-instruction corresponding to id
 * \see roxml_get_pi_nb
 * \see roxml_get_nodes
 *
 * example:
 * given the following xml file
 * \verbatim
<xml>
 <item1/>
 <?test value="2"?>
 <?test param="3"?>
 <item2/>
 <item3/>
</xml>
\endverbatim
 * \code
 * #include <stdio.h>
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_load_doc("/tmp/doc.xml");
 * 	node_t *xml =  roxml_get_chld(root, NULL,  0);
 * 	node_t *pi1 = roxml_get_pi(xml, 0);
 * 	node_t *pi2 = roxml_get_pi(xml, 1);
 *
 * 	// here pi1 is the <?value="2"?> node
 * 	if(strcmp(roxml_get_content(pi1, NULL, 0, NULL), "value=\"2\"") == 0) {
 * 		printf("got the first process-instruction\n");
 * 	}
 * 	// and pi2 is the <?param="3"?> node
 * 	if(strcmp(roxml_get_content(pi2, NULL, 0, NULL), "param=\"3\"") == 0) {
 * 		printf("got the second process-instruction\n");
 * 	}
 *
 * 	roxml_close(root);
 * 	return 0;
 * }
 *
 * \endcode
 */
ROXML_API node_t * roxml_get_pi(node_t *n, int nth);

/** \brief process-instruction number getter function
 *
 * \fn int  roxml_get_pi_nb(node_t *n);
 * This function return the number of process-instruction in a given node
 * \param n is one node of the tree
 * \return  the number of process-instructions
 * \see roxml_get_pi
 * \see roxml_get_nodes_nb
 */
ROXML_API int  roxml_get_pi_nb(node_t *n);

/** \brief name getter function
 *
 * \fn char*  roxml_get_name(node_t *n, char * buffer, int size);
 * This function return the name of the node or fill the current buffer with it if not NULL.
 * if name is NULL, the function will allocate a buffer that user should free using
 * roxml_release when no longer needed.
 * depending on node type it will return:
 * <ul>
 * <li> ROXML_ELM_NODE: returns the node name</li>
 * <li> ROXML_ATTR_NODE: returns the attribute name</li>
 * <li> ROXML_PI_NODE: returns the process-instruction targeted application</li>
 * <li> ROXML_TXT_NODE: returns NULL (or empty string if you provided a buffer in buffer param)</li>
 * <li> ROXML_CMT_NODE: returns NULL (or empty string if you provided a buffer in buffer param)</li>
 * <li> ROXML_NS_NODE: returns the namespace alias associated with the ns node </li>
 * </ul>
 * Be carreful as if your buffer is too short for the returned string, it will be truncated
 * \param n is one node of the tree
 * \param buffer a buffer pointer or NULL if has to be auto allocated
 * \param size the size of buffer pointed by buffer if not NULL
 * \return the name of the node (return our buffer pointer if it wasn't NULL)
 * \see roxml_release
 */
ROXML_API char * roxml_get_name(node_t *n, char *buffer, int size);

/** \brief content getter function
 *
 * \fn char *  roxml_get_content(node_t *n, char * buffer, int bufsize, int * size);
 *
 * This function returns the content of a node.;
 * if the returned pointer is NULL then the node either has no content or this function is irrelevant for this kind of node.
 * depending on node type it will return:
 * <ul>
 * <li> ROXML_ELM_NODE: returns the concatenation of all direct text node children</li>
 * <li> ROXML_ATTR_NODE: returns the attribute value</li>
 * <li> ROXML_PI_NODE: returns the process-instruction instruction</li>
 * <li> ROXML_TXT_NODE: returns the text content of the node</li>
 * <li> ROXML_CMT_NODE: returns the text content of the comment</li>
 * <li> ROXML_NS_NODE: returns the namespace definition (usually an URL)</li>
 * </ul>
 * returned string should be freed using roxml_release when not used anymore
 * \param n is one node of the tree
 * \param buffer is the buffer where result will be written or NULL for internal allocation
 * \param bufsize the size if using custom buffer
 * \param size the actual size of copied result. returned size should be less that buffer size since roxml_get_content
 * will add the \0. if buffer was not NULL and size == buf_size, then given buffer was too small and node content was truncated
 * \return the text content
 * \see roxml_release
 */
ROXML_API char * roxml_get_content(node_t *n, char *buffer, int bufsize, int *size);

/** \brief XML encoding/decoding function
 *
 * \fn roxml_escape(const char *buf, int decode, char *out)
 * this function converts the escape codes into ascii char or special carachers in escape codes.
 * \param buf the bytes to be converted
 * \param decode indicates whether to encde or decode escape caracteres.
 * \param out is filled with the encoded/decoded data if not null.
 * \return the size of encoded/decoded data
 * \see roxml_add_node
 * \see ENCODE
 * \see DECODE
 */
ROXML_API int roxml_escape(const char *buf, int decode, char *out);

/** \brief number of nodes getter function
 *
 * \fn int  roxml_get_nodes_nb(node_t *n, int type);
 *
 * This function returns the number of nodes matching type flag contained in a given node
 * all other roxml_get_*_nb are wrapper to this
 * \param n is one node of the tree
 * \param type is the bitmask of node types we want to consider
 * \return the number of nodes
 * \see roxml_get_attr_nb
 * \see roxml_get_chld_nb
 * \see roxml_get_txt_nb
 * \see roxml_get_cmt_nb
 * \see roxml_get_pi_nb
 *
 * example:
 * given the following xml file
 * \verbatim
<xml>
 <!-- comment -->
 <?value="2"?>
 <product id="42" class="item"/>
 <product id="43" class="item"/>
</xml>
\endverbatim
 * \code
 * #include <stdio.h>
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_load_doc("/tmp/doc.xml");
 *
 *	int all_nodes_1 = roxml_get_nodes_nb(root, ROXML_ELM_NODE | ROXML_CMT_NODE | ROXML_PI_NODE | ROXML_TXT_NODE | ROXML_ATTR_NODE);
 *	int all_nodes_2 = roxml_get_nodes_nb(root, ROXML_ALL_NODES);
 *
 * 	// here all_nodes_1 == all_nodes_2
 * 	if(all_nodes_1 == all_nodes_2) {
 * 		printf("%d Nodes are contained in root\n", all_nodes_1);
 * 	}
 *
 *	// let's count elm node (== children)
 *	int elm_nodes1 = roxml_get_nodes_nb(root, ROXML_ELM_NODE);
 *	int elm_nodes2 = roxml_get_chld_nb(root);
 *	// here elm_nodes1 == elm_nodes2 == 2
 *	if(elm_nodes1 == elm_nodes2) {
 *		printf("%d ELM Nodes are contained in root\n", elm_nodes_1);
 *	}
 *
 *	// we can also count all node except elm nodes, doing:
 *	int almost_all_nodes = roxml_get_nodes_nb(root, ROXML_ALL_NODES & ~ROXML_ELM_NODE);
 *	// here almost_all_nodes = 2 since we have one comment node and one processing-instruction node
 *	if(almost_all_nodes == 2) {
 *		printf("%d non ELM Nodes are contained in root\n", almost_all_nodes_1);
 *	}
 *
 * 	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 */
ROXML_API int  roxml_get_nodes_nb(node_t *n, int type);

/** \brief nodes getter function
 *
 * \fn char*  roxml_get_nodes(node_t *n, int type, char * name, int nth);
 * This function get the nth node matching type contained in a node, or the first node named name.
 * All other roxml_get_* are wrapper to this function.
 * When asking for several node type (let say ROXML_ALL_NODES), all ROXML_ATTR_NODE will be
 * placed first, then, all other nodes will come mixed, depending on xml document order.
 *
 * \param n is one node of the tree
 * \param type is the bitmask of node types we want to consider
 * \param name is the name of the child to get. This parameter is only relevant for node with types: \ref ROXML_ELM_NODE, \ref ROXML_ATTR_NODE, \ref ROXML_PI_NODE
 * \param nth the id of attribute to read
 * \return the node corresponding to name or id (if both are set, name is used)
 * \see roxml_get_attr
 * \see roxml_get_chld
 * \see roxml_get_txt
 * \see roxml_get_cmt
 * \see roxml_get_pi
 */
ROXML_API node_t * roxml_get_nodes(node_t *n, int type, char *name, int nth);

/** \brief number of attribute getter function
 *
 * \fn int  roxml_get_attr_nb(node_t *n);
 *
 * This function returns the number of attributes for a given node
 * \param n is one node of the tree
 * \return the number of attributes in node
 */
ROXML_API int  roxml_get_attr_nb(node_t *n);

/** \brief attribute getter function
 *
 * \fn node_t*  roxml_get_attr(node_t *n, char * name, int nth);
 * This function get the nth attribute of a node.
 * \param n is one node of the tree
 * \param name is the name of the attribute to get
 * \param nth the id of attribute to read
 * \return the attribute corresponding to name or id (if both are set, name is used)
 *
 * example:
 * given the following xml file
 * \verbatim
<xml>
 <product id="42" class="item"/>
</xml>
\endverbatim
 * \code
 * #include <stdio.h>
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_load_doc("/tmp/doc.xml");
 * 	node_t *item1 = roxml_get_chld(root, NULL, 0);
 * 	node_t *item2 = roxml_get_chld(item1, NULL, 0);
 *
 * 	node_t *attr_by_name = roxml_get_attr(item2, "id", 0);
 * 	node_t *attr_by_nth = roxml_get_attr(item2, NULL, 0);
 *
 * 	// here attr_by_name == attr_by_nth
 * 	if(attr_by_name == attr_by_nth) {
 * 		printf("Nodes are equal\n");
 * 	}
 *
 * 	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 */
ROXML_API node_t * roxml_get_attr(node_t *n, char *name, int nth);

/** \brief exec path function
 *
 * \fn roxml_xpath(node_t *n, char * path, int *nb_ans);
 * This function return a node set (table of nodes) corresponding to a given xpath.
 * resulting node set should be roxml_release when not used anymore (but not individual nodes)
 * \param n is one node of the tree
 * \param path the xpath to use
 * \param nb_ans the number of results
 * \return the node table or NULL
 *
 * handled xpath are described in \ref xpath
 */
ROXML_API node_t ** roxml_xpath(node_t *n, char *path, int *nb_ans);

/** \brief node type function
 *
 * \fn roxml_get_type(node_t *n);
 * This function tells if a node is an \ref ROXML_ATTR_NODE, \ref ROXML_TXT_NODE, \ref ROXML_PI_NODE, \ref ROXML_CMT_NODE or \ref ROXML_ELM_NODE.
 * Warning: ROXML_CDATA_NODE are special. They return a type as ROXML_TXT_NODE.
 * \param n is the node to test
 * \return the node type
 */
ROXML_API int  roxml_get_type(node_t *n);

/** \brief node get position function
 *
 * \fn roxml_get_node_position(node_t *n);
 * This function tells the index of a node between all its siblings homonyns.
 * \param n is the node to test
 * \return the postion between 1 and N
 */
ROXML_API int  roxml_get_node_position(node_t *n);

/** \brief memory cleanning function
 *
 * \fn roxml_release(void * data);
 * This function release the memory pointed by pointer
 * just like free would but for memory allocated with roxml_malloc.
 * Freeing a NULL pointer won't do
 * anything. roxml_release also allow you to remove all
 * previously allocated memory by using \ref RELEASE_ALL as argument.
 * You can also safely use \ref RELEASE_LAST argument that will release the
 * previously allocated varable within the current thread (making this
 * function thread safe).
 * if using roxml_release on a variable non roxml_mallocated, nothing will happen (ie variable won't be freed)
 * \param data the pointer to delete or NULL or \ref RELEASE_ALL or \ref RELEASE_LAST
 * \return void
 */
ROXML_API void  roxml_release(void *data);

/** \brief add a node to the tree
 *
 * \fn roxml_add_node(node_t *parent, int position, int type, char *name, char *value);
 * this function add a new node to the tree. This will not update de buffer or file,
 * only the RAM loaded tree. One should call \ref roxml_commit_changes to save modifications.
 * If the parent node is an \ref ROXML_ELM_NODE, then, new node will be added as a child. Else
 * the node will be added as a sibling of the parent node. In the later case, position parameter describes
 * the position in the sibling list, instead of position in the children list.
 * \param parent the parent node
 * \param position the position as a child of parent 1 is the first child, 0 for auto position at the end of children list...
 * \param type the type of node between \ref ROXML_ATTR_NODE, \ref ROXML_ELM_NODE, \ref ROXML_TXT_NODE, \ref ROXML_CDATA_NODE, \ref ROXML_PI_NODE, \ref ROXML_CMT_NODE, \ref ROXML_NSDEF_NODE, \ref ROXML_NS_NODE.
 * \param name the name of the node (mandatory for \ref ROXML_ATTR_NODE and \ref ROXML_ELM_NODE and \ref ROXML_PI_NODE and \ref ROXML_NSDEF_NODE and \ref ROXML_NS_NODE only)
 * \param value the text content (mandatory for \ref ROXML_TXT_NODE, \ref ROXML_CDATA_NODE, \ref ROXML_CMT_NODE, \ref ROXML_ATTR_NODE and \ref ROXML_NSDEF_NODE optional for \ref ROXML_ELM_NODE, \ref ROXML_PI_NODE). The text content for an attribute is the attribute value
 * \return the newly created node
 * \see roxml_commit_changes
 * \see roxml_commit_buffer
 * \see roxml_commit_file
 * \see roxml_del_node
 * \see roxml_close
 *
 * Escaping the as per XML specifications can be done by adding the ROXML_ESCAPED_MOD modifier to node type as showed below:
 * \code
 * roxml_add_node(NULL, 0, ROXML_ESCAPE_MODE | ROXML_ELM_NODE, "node1", "content <that> needs escaping");
 * \endcode
 * This will create a node that exports as:
 * \verbatim
 * <node1>content &lt;that&gt; needs escaping</node1>
 * \endverbatim
 *
 * paramaters name and value depending on node type:
 * - \ref ROXML_ELM_NODE take at least a node name. the parameter value is optional and represents the text content.
 * - \ref ROXML_TXT_NODE ignore the node name. the parameter value represents the text content.
 * - \ref ROXML_CDATA_NODE ignore the node name. the parameter value represents the text content that will be encapsulated in CDATA tags.
 * - \ref ROXML_CMT_NODE ignore the node name. the parameter value represents the comment.
 * - \ref ROXML_PI_NODE take the node name as process-instruction target. the parameter value represents the content of processing-instruction.
 * - \ref ROXML_ATTR_NODE take an attribute name. and the attribute value as given by parameter value.
 * - \ref ROXML_NSDEF_NODE take an attribute name (empty string for default namespace). and the namespace value as given by parameter value.
 * - \ref ROXML_NS_NODE take an attribute name (empty string for default namespace).
 *
 * some examples to obtain this xml result file
\verbatim
<root>
 <!-- sample XML file -->
 <item id="42">
  <price>
   24
  </price>
 </item>
</root>
\endverbatim
 *
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", "24");
 * 	roxml_commit_changes(root, "/tmp/test.xml", NULL, 1);
 *	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 * Or also:
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", NULL);
 * 	tmp = roxml_add_node(tmp, 0, ROXML_TXT_NODE, NULL, "24");
 * 	roxml_commit_changes(root, "/tmp/test.xml", NULL, 1);
 *	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 *
 * If you need a valid XML doc, just start by adding a corresponding process-instruction.
 * Example, to obtain this xml file:
 * \verbatim
<?xml version="1.0" encoding="UTF-8"?>
<!--sample file-->
<doc>
 basic content
 <item/>
 <item/>
 <item/>
</doc>
\endverbatim
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 *	node_t *root = roxml_add_node(NULL, 0, ROXML_PI_NODE, "xml", "version=\"1.0\" encoding=\"UTF-8\"");
 *	node_t *node = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample file");
 *	node = roxml_add_node(root, 0, ROXML_ELM_NODE, "doc", "basic content");
 *	roxml_add_node(node, 0, ROXML_ELM_NODE, "item", NULL);
 *	roxml_add_node(node, 0, ROXML_ELM_NODE, "item", NULL);
 *	roxml_add_node(node, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_commit_changes(root, "/tmp/test.xml", NULL, 1);
 *	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 */
ROXML_API node_t * roxml_add_node(node_t *parent, int position, int type, char *name, char *value);

/** \brief text node getter function
 *
 * \fn roxml_get_txt(node_t *n, int nth);
 * this function return the text content of a node as a \ref ROXML_TXT_NODE
 * the content of the text node can be read using the roxml_get_content function
 * \param n the node that contains text
 * \param nth the nth text node to retrieve
 * \return the text node or ROXML_INVALID_DOC (NULL)
 * \see roxml_get_txt_nb
 * \see roxml_get_nodes
 * \see roxml_get_content
 *
 * example:
 * given this xml file:
 * \verbatim
<xml>
  This is
  <item/>
  an example
  <item/>
  of text nodes
</xml>
\endverbatim
 * \code
 * #include <stdio.h>
 * #include <roxml.h>
 *
 * int main(void)
 * {
 *	int len;
 *	node_t *root = roxml_load_doc("/tmp/doc.xml");
 *	node_t *item = roxml_get_chld(root, NULL, 0);
 *
 *	node_t *text = roxml_get_txt(item, 0);
 *	char * text_content = roxml_get_content(text, NULL, 0, &len);
 *	// HERE text_content is equal to "This is"
 *	printf("text_content = '%s'\n", text_content);
 *
 *	text = roxml_get_txt(item, 1);
 *	text_content = roxml_get_content(text, NULL, 0, &len);
 *	// HERE text_content is equal to "an example"
 *	printf("text_content = '%s'\n", text_content);
 *
 *	text = roxml_get_txt(item, 2);
 *	text_content = roxml_get_content(text, NULL, 0, &len);
 *	// HERE text_content is equal to "of text nodes"
 *	printf("text_content = '%s'\n", text_content);
 *
 *	roxml_close(item);
 *	return 0;
 * }
 * \endcode
 */
ROXML_API node_t * roxml_get_txt(node_t *n, int nth);

/** \brief text node number getter function
 *
 * \fn roxml_get_txt_nb(node_t *n);
 * this function return the number of text nodes in
 * a standard node
 * \param n the node to search into
 * \return the number of text node
 * \see roxml_get_txt
 */
ROXML_API int  roxml_get_txt_nb(node_t *n);

/** \brief node deletion function
 *
 * \fn roxml_del_node(node_t *n);
 * this function delete a node from the tree. The node is not really deleted
 * from the file or buffer until the roxml_commit_changes is called, but it won't be
 * visible anymore in the XML tree.
 * \param n the node to delete
 * \return
 * \see roxml_add_node
 * \see roxml_commit_changes
 * \see roxml_commit_buffer
 * \see roxml_commit_file
 *
 * \warning when removing a nsdef node, all node using this namespace will be updated and inherit their parent namespace
 */
ROXML_API void  roxml_del_node(node_t *n);

/** \brief sync function
 *
 * \fn roxml_commit_changes(node_t *n, char * dest, char ** buffer, int human);
 * \deprecated
 * this function sync changes from the RAM tree to the given buffer or file in human or one-line format
 * The tree will be processed starting with the root node 'n' and following with all its children or if n is the root, all its siblings and children.
 * The tree will be dumped to a file if 'dest' is not null and contains a valid path.
 * The tree will be dumped to a buffer if 'buffer' is not null. the buffer is allocated by the library
 * and a pointer to it will be stored into 'buffer'. The allocated buffer can be freed usinf free()
 * \warning in the case of a tree loaded using roxml_load_doc, the roxml_commit_changes cannot be done to that same file
 * since it may override datas it need. This usually result in a new file filled with garbages. The solution is to write it to a temporary file and rename it after roxml_close the current tree.
 *
 * This function is now deprecated and one should use roxml_commit_buffer or roxml_commit_file that achieves the exact same goal.
 *
 * \param n the root node of the tree to write
 * \param dest the path to a file to write tree to
 * \param buffer the address of a buffer where the tree will be written. This buffer have to be freed after use
 * \param human 0 for one-line tree, or 1 for human format (using tabs, newlines...)
 * \return the number of bytes written to file or buffer
 * \see roxml_commit_buffer
 * \see roxml_commit_file
 *
 * This is a legacy function that proposes the same functionnalities as both
 * \c roxml_commit_file and \c roxml_commit_buffer. New code should use the
 * latter functions when needed.
 *
 * One should do:
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", "24");
 * 	roxml_commit_changes(root, "/tmp/test.xml", NULL, 1);
 * 	return 0;
 * }
 * \endcode
 * to generate the following xml bloc:
\verbatim
<root>
 <!-- sample XML file -->
 <item id="42">
  <price>
   24
  </price>
 </item>
</root>
\endverbatim
 * or also
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", "24");
 * 	roxml_commit_changes(root, "/tmp/test.xml", NULL, 0);
 *	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 * to generate the following xml bloc:
\verbatim
<root><!-- sample XML file --><item id="42"><price>24</price></item></root>
\endverbatim
 * the buffer variant works more or less the same way
 * \code
 * #include <stdio.h>
 * #include <roxml.h>
 *
 * int main(void)
 * {
 *	int len = 0;
 *	char * buffer = NULL;
 *	FILE * file_out;
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", "24");
 *
 * 	len = roxml_commit_changes(root, NULL, &buffer, 0);
 *
 *	file_out = fopen("/tmp/test.xml", "w");
 *      fwrite(buffer, 1, len, file_out);
 *      fclose(file_out);
 *
 *	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 * to generate the following xml bloc:
\verbatim
<root><!-- sample XML file --><item id="42"><price>24</price></item></root>
\endverbatim
 * \see roxml_commit_file
 * \see roxml_commit_buffer
 * \see roxml_commit_fd
 */
ROXML_API int  roxml_commit_changes(node_t *n, char *dest, char **buffer, int human);

/** \brief sync to named file function
 *
 * \fn roxml_commit_file(node_t *n, char * dest, int human);
 * this function sync changes from the RAM tree to the given file in human or one-line format
 * The tree will be processed starting with the root node 'n' and following with all its children or if n is the root, all its siblings and children.
 * The tree will be dumped to a file if 'dest' is not null and contains a valid path.
 * \warning in the case of a tree loaded using roxml_load_doc, the roxml_commit_file cannot be done to that same file
 * since it may override datas it needs. This usually resultis in a new file filled with garbage. The solution is to write it to a temporary file and rename it after roxml_close the current tree.
 *
 * \param n the root node of the tree to write
 * \param dest the path to a file to write tree to
 * \param human 0 for one-line tree, or 1 for human format (using tabs, newlines...)
 * \return the number of bytes written to file or buffer
 *
 * One should do:
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", "24");
 * 	roxml_commit_file(root, "/tmp/test.xml", 1);
 * 	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 * to generate the following xml bloc:
\verbatim
<root>
 <!-- sample XML file -->
 <item id="42">
  <price>
   24
  </price>
 </item>
</root>
\endverbatim
 * or also
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", "24");
 * 	roxml_commit_file(root, "/tmp/test.xml", 0);
 *	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 * to generate the following xml bloc:
\verbatim
<root><!-- sample XML file --><item id="42"><price>24</price></item></root>
\endverbatim
 * \see roxml_commit_changes
 * \see roxml_commit_buffer
 * \see roxml_commit_fd
 */
ROXML_API int  roxml_commit_file(node_t *n, char *dest, int human);

/** \brief sync to a memory buffer function
 *
 * \fn roxml_commit_buffer(node_t *n, char ** buffer, int human);
 * this function syncs changes from the RAM tree to the given buffer in human or one-line format
 * The tree will be processed starting with the root node 'n' and following with all its children or if n is the root, all its siblings and children.
 * The tree will be dumped to a buffer if 'buffer' is not null. the buffer is allocated by the library
 * and a pointer to it will be stored into 'buffer'. The allocated buffer can be freed using free()
 *
 * \param n the root node of the tree to write
 * \param buffer the address of a buffer where the tree will be written. This buffer have to be freed after use
 * \param human 0 for one-line tree, or 1 for human format (using indentation, newlines...)
 * \return the number of bytes written to file or buffer
 *
 * One should do:
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	char *buffer = NULL;
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", "24");
 * 	roxml_commit_changes(root, &buffer, 1);
 * 	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 * to generate the following xml bloc:
\verbatim
<root>
 <!-- sample XML file -->
 <item id="42">
  <price>
   24
  </price>
 </item>
</root>
\endverbatim
 * or also
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	char *buffer = NULL;
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", "24");
 * 	roxml_commit_buffer(root, &buffer, 0);
 *	roxml_close(root);
 * 	return 0;
 * }
 * \endcode
 * to generate the following xml bloc:
\verbatim
<root><!-- sample XML file --><item id="42"><price>24</price></item></root>
\endverbatim
 * \see roxml_commit_changes
 * \see roxml_commit_file
 * \see roxml_commit_fd
 */
ROXML_API int  roxml_commit_buffer(node_t *n, char **buffer, int human);

/** \brief sync to file descriptor function
 *
 * \fn roxml_commit_fd(node_t *n, int fd, int human);
 * this function synchronizes changes from the RAM tree to the given file in human or one-line format.
 * The tree will be processed starting with the root node 'n' and following with all its children or if n is the root, all its siblings and children.
 * The tree will be dumped to a file if fd is a valid file descriptor.
 * \warning in the case of a tree loaded using roxml_load_doc, the roxml_commit_fd cannot be done to that same file
 * since it may override datas it needs. This usually results in a new file filled with garbage. The solution is to write it to a temporary file and rename it after roxml_close the current tree.
 *
 * \param n the root node of the tree to write
 * \param fd the file descriptor to write tree to
 * \param human 0 for one-line tree, or 1 for human format (using tabs, newlines...)
 * \return the number of bytes written to file
 *
 * The file described by fd is not truncated so this function allows one to append an XML subtree to an existing file.
 *
 * One should do:
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	int fd = open("/tmp/test.xml", O_TRUNC|O_WRONLY, 0666);
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", "24");
 * 	roxml_commit_fd(root, fd, 1);
 * 	roxml_close(root);
 * 	close(fd);
 * 	return 0;
 * }
 * \endcode
 * to generate the following xml bloc:
\verbatim
<root>
 <!-- sample XML file -->
 <item id="42">
  <price>
   24
  </price>
 </item>
</root>
\endverbatim
 * or also
 * \code
 * #include <roxml.h>
 *
 * int main(void)
 * {
 * 	int fd = open("/tmp/test.xml", O_TRUNC|O_WRONLY, 0666);
 * 	node_t *root = roxml_add_node(NULL, 0, ROXML_ELM_NODE, "xml", NULL);
 * 	node_t *tmp = roxml_add_node(root, 0, ROXML_CMT_NODE, NULL, "sample XML file");
 * 	tmp = roxml_add_node(root, 0, ROXML_ELM_NODE, "item", NULL);
 * 	roxml_add_node(tmp, 0, ROXML_ATTR_NODE, "id", "42");
 * 	tmp = roxml_add_node(tmp, 0, ROXML_ELM_NODE, "price", "24");
 * 	roxml_commit_fd(root, fd, 0);
 *	roxml_close(root);
 * 	close(fd);
 * 	return 0;
 * }
 * \endcode
 * to generate the following xml bloc:
\verbatim
<root><!-- sample XML file --><item id="42"><price>24</price></item></root>
\endverbatim
 * \see roxml_commit_changes
 * \see roxml_commit_file
 * \see roxml_commit_buffer
 */
ROXML_API int  roxml_commit_fd(node_t *n, int fd, int human);

#ifdef __cplusplus
}
#endif

#endif /* ROXML_H */
