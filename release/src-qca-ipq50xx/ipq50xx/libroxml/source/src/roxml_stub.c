/**
 * \file roxml_stub.c
 * \brief API consistency module
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include "roxml_internal.h"

#ifndef CONFIG_XML_COMMIT
ROXML_STUB int roxml_commit_changes(node_t *n, char *dest, char **buffer, int human)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB int roxml_commit_file(node_t *n, char *dest, int human)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB int roxml_commit_buffer(node_t *n, char **buffer, int human)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB int roxml_commit_fd(node_t *n, int fd, int human)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}
#endif /* CONFIG_XML_COMMIT */

#ifndef CONFIG_XML_EDIT
ROXML_STUB void roxml_del_node(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return;
}

ROXML_STUB node_t *roxml_add_node(node_t *parent, int position, int type, char *name, char *value)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB node_t *roxml_set_ns(node_t *n, node_t *ns)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}
#endif /* CONFIG_XML_EDIT */

#ifndef CONFIG_XML_CONTENT
ROXML_STUB char *roxml_get_content(node_t *n, char *buffer, int bufsize, int *size)
{
	if (size)
		*size = 0;
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB char *roxml_get_name(node_t *n, char *buffer, int size)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB int roxml_get_nodes_nb(node_t *n, int type)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB node_t *roxml_get_nodes(node_t *n, int type, char *name, int nth)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB node_t *roxml_get_ns(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB int roxml_get_pi_nb(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB node_t *roxml_get_pi(node_t *n, int nth)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB int roxml_get_cmt_nb(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB node_t *roxml_get_cmt(node_t *n, int nth)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB int roxml_get_txt_nb(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB node_t *roxml_get_txt(node_t *n, int nth)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB int roxml_get_attr_nb(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB node_t *roxml_get_attr(node_t *n, char *name, int nth)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB int roxml_get_chld_nb(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB node_t *roxml_get_chld(node_t *n, char *name, int nth)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB int roxml_get_type(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return ROXML_INVALID_NODE;
}

ROXML_STUB int roxml_get_node_position(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}
#endif /* CONFIG_XML_CONTENT */

#ifndef CONFIG_XML_XPATH
ROXML_STUB node_t **roxml_xpath(node_t *n, char *path, int *nb_ans)
{
	if (nb_ans)
		*nb_ans = 0;
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}
#endif /* CONFIG_XML_XPATH */

#ifndef CONFIG_XML_NAV
ROXML_STUB node_t *roxml_get_prev_sibling(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB node_t *roxml_get_next_sibling(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB node_t *roxml_get_parent(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB node_t *roxml_get_root(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}
#endif /* CONFIG_XML_NAV */

#ifndef CONFIG_XML_FILE
ROXML_STUB node_t *roxml_load_fd(int fd)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB node_t *roxml_load_doc(char *filename)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB int roxml_read_file(int pos, int size, char *buffer, node_t *node)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB void roxml_close_file(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return;
}

ROXML_STUB int roxml_parse_file(roxml_load_ctx_t *context, roxml_parser_item_t *parser, FILE *file)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}
#endif /* CONFIG_XML_FILE */

#ifndef CONFIG_XML_BUFF
ROXML_STUB node_t *roxml_load_buf(char *buffer)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return NULL;
}

ROXML_STUB int roxml_read_buff(int pos, int size, char *buffer, node_t *node)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}

ROXML_STUB void roxml_close_buff(node_t *n)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return;
}

ROXML_STUB int roxml_parse_buff(roxml_load_ctx_t *context, roxml_parser_item_t *parser, char *buffer)
{
	fprintf(stderr, "** %s: function not implemented. You may need to reconfigure libroxml. **\n", __func__);
	return -1;
}
#endif /* CONFIG_XML_BUFF */
