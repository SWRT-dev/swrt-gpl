/**
 * \file roxml_commit.c
 * \brief XML serialize to file or buffer module
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "roxml_core.h"

ROXML_STATIC ROXML_INT void roxml_realloc_buf(char **buf, int *len, int min_len)
{
	int append = (min_len | (ROXML_BASE_LEN - 1)) + 1;
	*buf = realloc(*buf, *len + append);
	memset(*buf + *len, 0, append);
	*len += append;
}

/** \brief space printing function
 *
 * \fn roxml_print_spaces(FILE *f, char ** buf, int * offset, int * len, int lvl);
 * this function add some space to output when committing change in human format
 * \param f the file pointer if any
 * \param buf the pointer to string if any
 * \param offset the current offset in stream
 * \param len the total len of buffer if any
 * \param lvl the level in the tree
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_print_spaces(FILE *f, char **buf, int *offset, int *len, int lvl)
{
	int i;

	if (f) {
		if ((*offset)++ > 0)
			fwrite("\n", 1, 1, f);
		for (i = 0; i < lvl; i++)
			fwrite(" ", 1, 1, f);
	} else if (buf && *buf) {
		if (*offset + lvl + 1 >= *len)
			roxml_realloc_buf(buf, len, lvl);
		if (*offset > 0)
			*(*buf + (*offset)++) = '\n';
		for (i = 0; i < lvl; i++)
			*(*buf + (*offset)++) = ' ';
	}
}

/** \brief string writter function
 *
 * \fn void roxml_write_string(FILE *f, char **buf, int *offset, int *len, char *str, ...);
 * this function write a string to output when committing change
 * \param f the file pointer if any
 * \param buf the pointer to string if any
 * \param str the string to write
 * \param offset the current offset in stream
 * \param len the total len of buffer if any
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_write_string(FILE *f, char **buf, int *offset, int *len, char *str, ...)
{
	va_list args;
	int min_len;

	va_start(args, str);
	min_len = vsnprintf(NULL, 0, str, args);
	va_end(args);

	va_start(args, str);

	if (f) {
		vfprintf(f, str, args);
	} else if (buf && *buf) {
		if (*offset + min_len >= *len)
			roxml_realloc_buf(buf, len, min_len);
		*offset += vsprintf(*buf + *offset, str, args);
	}

	va_end(args);
}

ROXML_STATIC ROXML_INT void roxml_write_other_node(node_t *n, FILE *f, char **buf, int *offset, int *len, char *name)
{
	char *value;
	int status;
	char head[8] = "\0";
	char tail[8] = "\0";
	char cdata_head[16] = "\0";
	char cdata_tail[8] = "\0";

	ROXML_GET_BASE_BUFFER(val);

	if ((n->type & ROXML_NODE_TYPES) == ROXML_CMT_NODE) {
		strcpy(head, "<!--");
		strcpy(tail, "-->");
	} else if ((n->type & ROXML_NODE_TYPES) == ROXML_DOCTYPE_NODE) {
		strcpy(head, "<");
		strcpy(tail, ">");
	} else if ((n->type & ROXML_NODE_TYPES) == ROXML_PI_NODE) {
		strcpy(head, "<?");
		strcpy(tail, "?>");
	}

	value = roxml_get_content(n, val, ROXML_BASE_LEN, &status);
	if (status >= ROXML_BASE_LEN)
		value = roxml_get_content(n, NULL, 0, &status);

	if ((n->type & ROXML_CDATA_NODE) == ROXML_CDATA_NODE) {
		strcpy(cdata_head, "<![CDATA[");
		strcpy(cdata_tail, "]]>");
	}

	roxml_write_string(f, buf, offset, len, "%s%s%s%s%s%s%s", head,
			   name[0] ? name : "", name[0] ? " " : "", cdata_head, value, cdata_tail, tail);
	roxml_release(value);

	ROXML_PUT_BASE_BUFFER(val);
}

ROXML_STATIC ROXML_INT void roxml_write_elm_name_open(node_t *n, FILE *f, char **buf, int *offset, int *len,
						      char *name, char *ns)
{
	if (!n->prnt)
		return;

	if (n->ns)
		roxml_get_name(n->ns, ns, ROXML_BASE_LEN);
	else
		ns[0] = '\0';

	roxml_write_string(f, buf, offset, len, "<%s%s%s", ns[0] ? ns : "", ns[0] ? ":" : "", name);
}

ROXML_STATIC ROXML_INT void roxml_write_elm_name_close(node_t *n, FILE *f, char **buf, int *offset, int *len,
						       char *name, char *ns)
{
	char head[8] = "\0";
	char tail[8] = "/>";

	if (n->chld) {
		strcpy(head, "</");
		strcpy(tail, ">");
	}

	roxml_write_string(f, buf, offset, len, "%s%s%s%s%s", head,
			   n->chld && ns[0] ? ns : "", n->chld && ns[0] ? ":" : "", n->chld ? name : "", tail);
}

ROXML_STATIC ROXML_INT void roxml_write_elm_attr(node_t *n, FILE *f, char **buf, int *offset, int *len)
{
	node_t *attr = n->attr;

	while (attr) {
		int ns_node_attr = attr->type & ROXML_NS_NODE;
		int status = 0;
		char arg[ROXML_BASE_LEN];
		char val[ROXML_BASE_LEN];
		char *value;

		ROXML_GET_BASE_BUFFER(ns);

		roxml_get_name(attr, arg, ROXML_BASE_LEN);

		value = roxml_get_content(attr, val, ROXML_BASE_LEN, &status);
		if (status >= ROXML_BASE_LEN)
			value = roxml_get_content(attr, NULL, 0, &status);

		if (attr->ns)
			roxml_get_name(attr->ns, ns, ROXML_BASE_LEN);

		roxml_write_string(f, buf, offset, len, " %s%s%s%s%s=\"%s\"",
				   ns_node_attr ? "xmlns" : "",
				   ns_node_attr && arg[0] ? ":" : "",
				   attr->ns ? ns : "", attr->ns ? ":" : "", arg, value);

		attr = attr->sibl;
		roxml_release(value);

		ROXML_PUT_BASE_BUFFER(ns);
	}
}

/** \brief tree write function
 *
 * \fn roxml_write_node(node_t *n, FILE *f, char ** buf, int human, int lvl, int *offset, int *len);
 * this function write each node of the tree to output
 * \param n the node to write
 * \param f the file pointer if any
 * \param buf the pointer to the buffer string if any
 * \param human 1 to use the human format else 0
 * \param lvl the current level in tree
 * \param offset the current offset in stream
 * \param len the total len of buffer if any
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_write_node(node_t *n, FILE *f, char **buf, int human, int lvl, int *offset, int *len)
{
	ROXML_GET_BASE_BUFFER(name);
	roxml_get_name(n, name, ROXML_BASE_LEN);

	if (n->type & ROXML_ELM_NODE) {
		ROXML_GET_BASE_BUFFER(ns);
		node_t *chld = n->chld;

		if (human)
			roxml_print_spaces(f, buf, offset, len, lvl);
		roxml_write_elm_name_open(n, f, buf, offset, len, name, ns);
		roxml_write_elm_attr(n, f, buf, offset, len);

		if (chld) {
			node_t *prev = chld;

			roxml_write_string(f, buf, offset, len, ">");

			while (chld) {
				int keep_human = human;

				if ((chld->type | prev->type) & ROXML_TXT_NODE)
					keep_human = 0;

				roxml_write_node(chld, f, buf, keep_human, lvl + 1, offset, len);

				prev = chld;
				chld = chld->sibl;

			}
			if (human && !(prev->type & ROXML_TXT_NODE))
				roxml_print_spaces(f, buf, offset, len, lvl);
		}

		roxml_write_elm_name_close(n, f, buf, offset, len, name, ns);
		ROXML_PUT_BASE_BUFFER(ns);
	} else {
		if (human)
			roxml_print_spaces(f, buf, offset, len, lvl);
		roxml_write_other_node(n, f, buf, offset, len, name);
	}

	ROXML_PUT_BASE_BUFFER(name);
}

ROXML_STATIC ROXML_INT void roxml_commit_nodes(node_t *n, FILE *f, char **buf, int human, int *size, int *len)
{
	node_t fakeroot;

	if (n->prnt == NULL) {
		/* Get the first child since we do not write the docroot */
		n = n->chld;
	} else if (n->prnt->prnt == NULL) {
		/* Get the first sibling since it might be a PI */
		n = n->prnt->chld;
	} else {
		/* The root is a subtree: simulate a real root */
		memcpy(&fakeroot, n, sizeof(node_t));
		fakeroot.sibl = NULL;
		n = &fakeroot;
	}

	while (n) {
		roxml_write_node(n, f, buf, human, 0, size, len);
		n = n->sibl;
	}
}

ROXML_API int roxml_commit_file(node_t *n, char *dest, int human)
{
	FILE *fout;
	int size = 0;
	int len = ROXML_BASE_LEN;

	if (n == ROXML_INVALID_DOC)
		return 0;

	if (!dest)
		return 0;

	fout = fopen(dest, "w");
	if (!fout)
		return 0;

	roxml_commit_nodes(n, fout, NULL, human, &size, &len);

	size = ftell(fout);
	fclose(fout);

	return size;
}

ROXML_API int roxml_commit_buffer(node_t *n, char **buffer, int human)
{
	int size = 0;
	int len = ROXML_BASE_LEN;

	if (n == ROXML_INVALID_DOC)
		return 0;

	if (!buffer)
		return 0;

	*buffer = malloc(ROXML_BASE_LEN);
	if (!*buffer)
		return 0;
	memset(*buffer, 0, ROXML_BASE_LEN);

	roxml_commit_nodes(n, NULL, buffer, human, &size, &len);

	return size;
}

ROXML_API int roxml_commit_fd(node_t *n, int fd, int human)
{
	int fd2;
	FILE *fout;
	int size = 0;
	int len = ROXML_BASE_LEN;

	if (n == ROXML_INVALID_DOC)
		return 0;

	if (fd <= 0)
		return 0;

	/* in order to avoid accidental close of fd, we need dup() it */
	fd2 = dup(fd);
	if (fd2 < 0)
		return 0;

	/*
	 * fdopen(fd, "w") does not truncate the file - so the user
	 * must have called ftruncate() before calling this function
	 * if needed ; otherwise, data will be appended to the file.
	 */

	fout = fdopen(fd2, "w");
	if (!fout)
		return 0;

	roxml_commit_nodes(n, fout, NULL, human, &size, &len);

	/* since we pass a fd to this function, we expect it to not be
	 * buffered, so flush everything before doing anything else */
	fflush(fout);

	size = ftell(fout);
	fclose(fout);

	return size;
}

ROXML_API int roxml_commit_changes(node_t *n, char *dest, char **buffer, int human)
{
	if (dest)
		return roxml_commit_file(n, dest, human);
	else if (buffer)
		return roxml_commit_buffer(n, buffer, human);
	return 0;
}
