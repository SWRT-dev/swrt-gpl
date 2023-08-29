/**
 * \file roxml_content.c
 * \brief XML content access module
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include <string.h>
#include <stdlib.h>
#include "roxml_mem.h"
#include "roxml_file.h"
#include "roxml_buff.h"

struct roxml_escape_char {
	char code;
	char *escape;
};

struct roxml_escape_char specials[5] = {
	{ .code = '"', .escape = "&quot;" },
	{ .code = '\'', .escape = "&apos;" },
	{ .code = '<', .escape = "&lt;"},
	{ .code = '>', .escape = "&gt;"},
	{ .code = '&', .escape = "&amp;"}
};

ROXML_API int roxml_escape(const char *buf, int decode, char *out)
{
	size_t i = 0;
	size_t l = 0;
	for (; i < strlen(buf); ) {
		int s = 0;
		for (; s < 5; s++) {
			if (decode && (strncmp(specials[s].escape, buf+i, strlen(specials[s].escape)) == 0)) {
				if (out)
					out[l] = specials[s].code;
				l += 1;
				i += strlen(specials[s].escape);
				break;
			} else if (!decode && (buf[i] == specials[s].code)) {
				if (out)
					strcpy(out+l, specials[s].escape);
				l += strlen(specials[s].escape);
				i += 1;
				break;
			}
		}
		if (s == 5) {
			if (out)
				out[l] = buf[i];
			l += 1;
			i += 1;
		}
	}
	if (out)
		out[l] = '\0';
	return l;
}

/** \brief read xml doc function
 *
 * \fn roxml_read(int pos, int size, char * buffer, node_t *node)
 * this function read inside a xml doc (file or buffer) and fill the given buffer
 * \param pos the pos in the xml document
 * \param size the size of the data to read
 * \param buffer the destination buffer
 * \param node the node that belong to the tree we want to read to
 * \return the number of bytes read
 */
ROXML_STATIC ROXML_INT inline int roxml_read(int pos, int size, char *buffer, node_t *node)
{
	int len = 0;

	if (size > 0 && buffer) {
		if (node->type & ROXML_FILE)
			len = roxml_read_file(pos, size, buffer, node);
		else
			len = roxml_read_buff(pos, size, buffer, node);
	}

	return len;
}

ROXML_STATIC ROXML_INT inline int roxml_content_size(node_t *n, int *offset)
{
	int total = 0;

	*offset = 0;

	if (!n)
		return 0;

	if (n->type & ROXML_ELM_NODE) {
		node_t *ptr = n->chld;
		while (ptr) {
			if ((ptr->type & ROXML_NODE_TYPES) == ROXML_TXT_NODE)
				total += ptr->end - ptr->pos;
			ptr = ptr->sibl;
		}
	} else if (n->type & ROXML_ATTR_NODE) {
		if (n->chld) {
			total = n->chld->end - n->chld->pos;
			*offset = n->chld->pos;
		}
	} else {
		int name_len = 0;

		ROXML_GET_BASE_BUFFER(name);

		roxml_get_name(n, name, ROXML_BASE_LEN);
		name_len = strlen(name);

		ROXML_PUT_BASE_BUFFER(name);

		if (n->type & ROXML_DOCTYPE_NODE)
			name_len += 2;
		else if (n->type & ROXML_TXT_NODE)
			name_len = 0;
		else if (n->type & ROXML_CMT_NODE)
			name_len = 4;
		else if (n->type & ROXML_PI_NODE)
			name_len += 3;

		total = n->end - n->pos - name_len;
		*offset = n->pos + name_len;
	}
	return total;
}

ROXML_STATIC ROXML_INT inline char *roxml_prepare_buffer(node_t *n, char *buffer, int contentsize, int size)
{
	if (n == NULL) {
		if (buffer)
			memset(buffer, 0, size);
		return NULL;
	}
	if (buffer == NULL)
		buffer = roxml_malloc(sizeof(char), contentsize + 1, PTR_CHAR);
	memset(buffer, 0, size);

	return buffer;
}

ROXML_API char *roxml_get_content(node_t *n, char *buffer, int bufsize, int *size)
{
	int total = 0;
	int content_offset;
	int content_size;
	char *content;

	if (n == ROXML_INVALID_DOC)
		return NULL;

	content_size = roxml_content_size(n, &content_offset);
	content = roxml_prepare_buffer(n, buffer, content_size, bufsize);

	if (buffer != content)
		bufsize = content_size + 1;
	if (content_size > bufsize - 1)
		content_size = bufsize - 1;
	if (content == NULL) {
		if (size)
			*size = 0;
		return buffer;
	}

	if (n->type & ROXML_ELM_NODE) {
		node_t *ptr = n->chld;

		while (ptr) {
			if (ptr->type & ROXML_TXT_NODE) {
				int len = 0;
				int read_size = ptr->end - ptr->pos;

				if (total + read_size > bufsize - 1)
					read_size = bufsize - total - 1;
				len += roxml_read(ptr->pos, read_size, content + total, ptr);

				total += len;
			}
			ptr = ptr->sibl;
		}
	} else {
		node_t *target = n;
		if (n->type & ROXML_ATTR_NODE)
			target = n->chld;
		total = roxml_read(content_offset, content_size, content, target);
	}

	content[total] = '\0';
	if (size)
		*size = total + 1;
	return content;
}

ROXML_STATIC ROXML_INT inline int roxml_name_size(node_t *n, int size, int *offset)
{
	int total = 0;

	*offset = 0;

	if (!n)
		return 0;

	if ((n->type & ROXML_TXT_NODE) || (n->type & ROXML_CMT_NODE)) {
		total = 0;
	} else {
		*offset = n->pos;

		if (n->type & ROXML_PI_NODE)
			*offset += 2;
		else if (n->type & ROXML_DOCTYPE_NODE)
			*offset += 1;

		total = ROXML_BASE_LEN;
	}

	return total;
}

ROXML_API char *roxml_get_name(node_t *n, char *buffer, int size)
{
	int content_offset;
	int content_size;
	char *content;

	if (n == ROXML_INVALID_DOC)
		return NULL;

	content_size = roxml_name_size(n, size, &content_offset);
	content = roxml_prepare_buffer(n, buffer, content_size, size);

	if (buffer != content)
		size = content_size + 1;
	if (content_size > size - 1)
		content_size = size - 1;
	if (content == NULL)
		return buffer;
	if (content_size == 0)
		return content;

	memset(content, 0, content_size);

	if (n->prnt == NULL) {
		strcpy(content, "documentRoot");
	} else if (n->type & ROXML_NS_NODE) {
		roxml_ns_t *ns = (roxml_ns_t *)n->priv;
		if (ns)
			strncpy(content, ns->alias, size);
		else
			content[0] = '\0';
	} else {
		int total = 0;
		int count = 0;
		char *begin = content;

		total = roxml_read(content_offset, content_size, content, n);

		while (ROXML_WHITE(begin[0]) || begin[0] == '<')
			begin++;

		if (n->type & ROXML_PI_NODE) {
			for (; count < total; count++) {
				if (ROXML_WHITE(begin[count]))
					break;
				else if ((begin[count] == '?') && (begin[count + 1] == '>'))
					break;
			}
		} else if (n->type & ROXML_ELM_NODE) {
			for (; count < total; count++) {
				if (ROXML_WHITE(begin[count]))
					break;
				else if ((begin[count] == '/') && (begin[count + 1] == '>'))
					break;
				else if (begin[count] == '>')
					break;
			}
		} else if (n->type & ROXML_ATTR_NODE) {
			for (; count < total; count++) {
				if (ROXML_WHITE(begin[count]))
					break;
				else if (begin[count] == '=')
					break;
				else if (begin[count] == '>')
					break;
				else if ((begin[count] == '/') && (begin[count + 1] == '>'))
					break;
			}
		} else if (n->type & ROXML_DOCTYPE_NODE) {
			for (; count < total; count++) {
				if (ROXML_WHITE(begin[count]))
					break;
				else if (begin[count] == '>')
					break;
			}
		}
		begin[count++] = '\0';
		memmove(content, begin, count);
	}

	return content;
}

ROXML_API int roxml_get_nodes_nb(node_t *n, int type)
{
	node_t *ptr = n;
	int nb;

	if (n == ROXML_INVALID_DOC)
		return -1;

	nb = 0;
	if (ptr->chld) {
		ptr = ptr->chld;
		do {
			if (ptr->type & type)
				nb++;
			ptr = ptr->sibl;
		} while (ptr);
	}

	if (type & ROXML_ATTR_NODE) {
		ptr = n->attr;
		while (ptr) {
			nb++;
			ptr = ptr->sibl;
		}
	}
	return nb;
}

ROXML_STATIC ROXML_INT inline node_t *roxml_get_nodes_by_name(node_t *n, int type, char *name)
{
	node_t *ptr;

	if (n->attr && (type & ROXML_ATTR_NODE))
		ptr = n->attr;
	else
		ptr = n->chld;

	while (ptr) {
		if ((ptr->type & ROXML_NODE_TYPES) & type) {
			int ans = strcmp(roxml_get_name(ptr, NULL, 0), name);
			roxml_release(RELEASE_LAST);
			if (ans == 0)
				return ptr;
		}
		ptr = ptr->sibl;
	}
	return NULL;
}

ROXML_STATIC ROXML_INT inline node_t *roxml_get_nodes_by_nth(node_t *n, int type, int nth)
{
	node_t *ptr;
	int count = 0;

	if (n->ns && (type & ROXML_NS_NODE)) {
		ptr = n->ns;
		if (nth == 0)
			return ptr;
	} else if (n->attr && (type & ROXML_ATTR_NODE)) {
		ptr = n->attr;
		if (nth == 0)
			return ptr;
		while ((ptr->sibl) && (nth > count)) {
			ptr = ptr->sibl;
			count++;
		}
	} else {
		ptr = n->chld;
		while (ptr && !((ptr->type & ROXML_NODE_TYPES) & type))
			ptr = ptr->sibl;
	}
	if (nth > count) {
		ptr = n->chld;
		while (ptr && !((ptr->type & ROXML_NODE_TYPES) & type))
			ptr = ptr->sibl;
		while (ptr && (ptr->sibl) && (nth > count)) {
			ptr = ptr->sibl;
			if ((ptr->type & ROXML_NODE_TYPES) & type)
				count++;
		}
	}
	if (nth > count)
		return NULL;
	return ptr;
}

ROXML_API node_t *roxml_get_nodes(node_t *n, int type, char *name, int nth)
{
	if (n == ROXML_INVALID_DOC)
		return ROXML_INVALID_DOC;
	else if (name == NULL)
		return roxml_get_nodes_by_nth(n, type, nth);
	else
		return roxml_get_nodes_by_name(n, type, name);
}

ROXML_API inline node_t *roxml_get_ns(node_t *n)
{
	return roxml_get_nodes(n, ROXML_NS_NODE, NULL, 0);
}

ROXML_API inline int roxml_get_pi_nb(node_t *n)
{
	return roxml_get_nodes_nb(n, ROXML_PI_NODE);
}

ROXML_API inline node_t *roxml_get_pi(node_t *n, int nth)
{
	return roxml_get_nodes(n, ROXML_PI_NODE, NULL, nth);
}

ROXML_API inline int roxml_get_cmt_nb(node_t *n)
{
	return roxml_get_nodes_nb(n, ROXML_CMT_NODE);
}

ROXML_API inline node_t *roxml_get_cmt(node_t *n, int nth)
{
	return roxml_get_nodes(n, ROXML_CMT_NODE, NULL, nth);
}

ROXML_API inline int roxml_get_txt_nb(node_t *n)
{
	return roxml_get_nodes_nb(n, ROXML_TXT_NODE);
}

ROXML_API inline node_t *roxml_get_txt(node_t *n, int nth)
{
	return roxml_get_nodes(n, ROXML_TXT_NODE, NULL, nth);
}

ROXML_API inline int roxml_get_attr_nb(node_t *n)
{
	return roxml_get_nodes_nb(n, ROXML_ATTR_NODE);
}

ROXML_API inline node_t *roxml_get_attr(node_t *n, char *name, int nth)
{
	return roxml_get_nodes(n, ROXML_ATTR_NODE, name, nth);
}

ROXML_API inline int roxml_get_chld_nb(node_t *n)
{
	return roxml_get_nodes_nb(n, ROXML_ELM_NODE);
}

ROXML_API inline node_t *roxml_get_chld(node_t *n, char *name, int nth)
{
	return roxml_get_nodes(n, ROXML_ELM_NODE, name, nth);
}

ROXML_API inline int roxml_get_type(node_t *n)
{
	if (n == ROXML_INVALID_DOC)
		return ROXML_INVALID_NODE;
	return (n->type & ROXML_NODE_TYPES);
}

ROXML_API int roxml_get_node_position(node_t *n)
{
	int idx = 1;
	char name[256];
	node_t *prnt;
	node_t *first;

	if (n == ROXML_INVALID_DOC)
		return 0;

	roxml_get_name(n, name, 256);

	prnt = n->prnt;
	if (!prnt)
		return 1;
	first = prnt->chld;

	while ((first) && (first != n)) {
		char twin[256];

		roxml_get_name(first, twin, 256);
		if (strcmp(name, twin) == 0)
			idx++;
		first = first->sibl;
	}

	return idx;
}
