/**
 * \file roxml_xpath.c
 * \brief xpath execution module
 *
 * (C) Copyright 2014
 * Tristan Lelong <tristan.lelong@libroxml.net>
 *
 * SPDX-Licence-Identifier:	LGPL-2.1+
 * The author added a static linking exception, see License.txt.
 */
#include <stdlib.h>
#include <string.h>
#include "roxml_mem.h"
#include "roxml_core.h"
#include "roxml_xpath.h"
#include "roxml_parser.h"

/** \brief number tester
 *
 * \fn int roxml_is_number(char *input);
 * This function tells if a string is a number
 * \param input string to test
 * \return 1 if the string was a number else 0
 */
ROXML_STATIC ROXML_INT int roxml_is_number(char *input)
{
	char *end;
	int is_number = 0;

	/*
	 * we don't need the value per se and some compiler will
	 * complain about an initialized but unused variable if we
	 * get it.
	 */
	roxml_strtonum(input, &end);

	if ((end == NULL) ||
	    (roxml_is_separator(end[0])) ||
	    (end[0] == '"') ||
	    (end[0] == '\'') ||
	    (end[0] == '\0')) {
		is_number = 1;
	}

	return is_number;
}

/** \brief xpath condition free function
 *
 * \fn roxml_free_xcond(xpath_cond_t *xcond);
 * this function frees an xpath_cond_t cell
 * \param xcond the xcond to free
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_free_xcond(xpath_cond_t *xcond)
{
	if (xcond->next)
		roxml_free_xcond(xcond->next);
	if (xcond->xp)
		roxml_free_xpath(xcond->xp, xcond->func2);
	free(xcond);
}

ROXML_INT void roxml_free_xpath(xpath_node_t *xpath, int nb)
{
	int i = 0;
	for (i = 0; i < nb; i++) {
		if (xpath[i].next)
			roxml_free_xpath(xpath[i].next, 1);
		if (xpath[i].cond)
			roxml_free_xcond(xpath[i].cond);
		free(xpath[i].xp_cond);
	}
	free(xpath);
}

/** \brief node pool presence tester function
 *
 * \fn roxml_in_pool(node_t *root, node_t *n, int req_id);
 * this function test is a node is in a pool
 * \param root the root of the tree
 * \param n the node to test
 * \param req_id the pool id
 * \return
 */
ROXML_STATIC ROXML_INT int roxml_in_pool(node_t *root, node_t *n, int req_id)
{
	roxml_lock(root);
	if (n->priv) {
		xpath_tok_t *tok = (xpath_tok_t *)n->priv;
		if (tok->id == req_id) {
			roxml_unlock(root);
			return 1;
		} else {
			while (tok) {
				if (tok->id == req_id) {
					roxml_unlock(root);
					return 1;
				}
				tok = tok->next;
			}
		}
	}
	roxml_unlock(root);

	return 0;
}

/** \brief pool node delete function
 *
 * \fn roxml_del_from_pool(node_t *root, node_t *n, int req_id);
 * this function remove one node from a pool
 * \param root the root of the tree
 * \param n the node to remove
 * \param req_id the pool id
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_del_from_pool(node_t *root, node_t *n, int req_id)
{
	roxml_lock(root);
	if (n->priv) {
		xpath_tok_t *prev = (xpath_tok_t *)n->priv;
		xpath_tok_t *tok = (xpath_tok_t *)n->priv;
		if (tok->id == req_id) {
			n->priv = (void *)tok->next;
			free(tok);
		} else {
			while (tok) {
				if (tok->id == req_id) {
					prev->next = tok->next;
					free(tok);
					break;
				}
				prev = tok;
				tok = tok->next;
			}
		}
	}
	roxml_unlock(root);
}

/** \brief add a token top node function
 *
 * \fn roxml_add_to_pool(node_t *root, node_t *n, int req_id);
 * this function add a token to target node. This token is used to garanty
 * unicity in xpath results
 * \param root the root node
 * \param n the node to mark
 * \param req_id the id to use
 * \return 0 if already in the pool, else 1
 */
ROXML_STATIC ROXML_INT int roxml_add_to_pool(node_t *root, node_t *n, int req_id)
{
	xpath_tok_t *tok;
	xpath_tok_t *last_tok = NULL;

	if (req_id == 0)
		return 1;

	roxml_lock(root);
	tok = (xpath_tok_t *)n->priv;

	while (tok) {
		if (tok->id == req_id) {
			roxml_unlock(root);
			return 0;
		}
		last_tok = tok;
		tok = (xpath_tok_t *)tok->next;
	}
	if (last_tok == NULL) {
		n->priv = calloc(1, sizeof(xpath_tok_t));
		last_tok = (xpath_tok_t *)n->priv;
	} else {
		last_tok->next = calloc(1, sizeof(xpath_tok_t));
		last_tok = last_tok->next;
	}
	last_tok->id = req_id;
	roxml_unlock(root);

	return 1;
}

/** \brief release id function
 *
 * \fn roxml_release_id(node_t *root, node_t **pool, int pool_len, int req_id);
 * this function release a previously required id and remove all id token from the pool
 * \param root the root of the tree that was used for id request
 * \param pool the result from xpath search using this id
 * \param pool_len the number of node in pool
 * \param req_id the id to release
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_release_id(node_t *root, node_t **pool, int pool_len, int req_id)
{
	int i;
	xpath_tok_table_t *table = (xpath_tok_table_t *)root->priv;

	for (i = 0; i < pool_len; i++)
		roxml_del_from_pool(root, pool[i], req_id);

	roxml_lock(root);
	table->ids[req_id] = 0;
	roxml_unlock(root);
}

/** \brief id reservation function
 *
 * \fn roxml_request_id(node_t *root);
 * this function request an available id for a new xpath search on the tree
 * \param root the root of the tree where id table is stored
 * \return the id between 1 - 255 or -1 if failed
 */
ROXML_STATIC ROXML_INT int roxml_request_id(node_t *root)
{
	int i = 0;
	xpath_tok_table_t *table = (xpath_tok_table_t *)root->priv;

	roxml_lock(root);
	for (i = ROXML_XPATH_FIRST_ID; i < 255; i++) {
		if (table->ids[i] == 0) {
			table->ids[i]++;
			roxml_unlock(root);
			return i;
		}
	}
	roxml_unlock(root);

	return -1;
}

/** \brief node set and function
 *
 * \fn roxml_compute_and(node_t *root, node_t **node_set, int *count, int cur_req_id, int prev_req_id);
 * this function computes the AND of two node pools. The resulting pool will have the same ID as cur_req_id.
 * \param root the root of the tree
 * \param node_set the node set containing the 2 pools
 * \param count number of node in the node set
 * \param cur_req_id the id of the first group
 * \param prev_req_id the id of the second group
 */
ROXML_STATIC ROXML_INT void roxml_compute_and(node_t *root, node_t **node_set, int *count, int cur_req_id,
					      int prev_req_id)
{
	int i = 0;
	int limit = *count;
	int pool1 = 0, pool2 = 0;

	for (i = 0; i < limit; i++) {
		if (pool1 == 0)
			if (roxml_in_pool(root, node_set[i], cur_req_id))
				pool1++;
		if (pool2 == 0)
			if (roxml_in_pool(root, node_set[i], prev_req_id))
				pool2++;
		if (pool1 && pool2)
			break;
	}

	if (!pool1 || !pool2) {
		for (i = 0; i < limit; i++) {
			roxml_del_from_pool(root, node_set[i], cur_req_id);
			roxml_del_from_pool(root, node_set[i], prev_req_id);
		}
		*count = 0;
	}
}

/** \brief node set or function
 *
 * \fn roxml_compute_or(node_t *root, node_t **node_set, int *count, int req_id, int glob_id);
 * this function computes the OR of two node pools. The resulting pool will have the same ID as glob_id.
 * \param root the root of the tree
 * \param node_set the node set containing the 2 pools
 * \param count number of node in the node set
 * \param req_id the id of the first group
 * \param glob_id the id of the second group
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_compute_or(node_t *root, node_t **node_set, int *count, int req_id, int glob_id)
{
	int i = 0;
	for (i = 0; i < *count; i++) {
		if (roxml_in_pool(root, node_set[i], req_id)) {
			roxml_add_to_pool(root, node_set[i], glob_id);
			roxml_del_from_pool(root, node_set[i], req_id);
		}
	}
}

/** \brief xpath parsing function
 *
 * \fn roxml_parse_xpath(char *path, xpath_node_t **xpath, int context);
 * this function convert an xpath string to a table of list of xpath_node_t
 * \param path the xpath string
 * \param xpath the parsed xpath
 * \param context 0 for a real xpath, 1 for a xpath in predicat
 * \return the number of xpath list in the table
 */
ROXML_STATIC ROXML_INT int roxml_parse_xpath(char *path, xpath_node_t **xpath, int context)
{
	int ret = 0;
	roxml_xpath_ctx_t ctx;
	roxml_parser_item_t *parser = NULL;
	ctx.pos = 0;
	ctx.nbpath = 1;
	ctx.bracket = 0;
	ctx.parenthesys = 0;
	ctx.quoted = 0;
	ctx.dquoted = 0;
	ctx.content_quoted = 0;
	ctx.is_first_node = 1;
	ctx.wait_first_node = 1;
	ctx.shorten_cond = 0;
	ctx.context = context;
	ctx.first_node = calloc(1, sizeof(xpath_node_t));
	ctx.new_node = ctx.first_node;
	ctx.new_cond = NULL;
	ctx.first_node->rel = ROXML_OPERATOR_OR;

	parser = roxml_append_parser_item(parser, " ", _func_xpath_ignore);
	parser = roxml_append_parser_item(parser, "\t", _func_xpath_ignore);
	parser = roxml_append_parser_item(parser, "\n", _func_xpath_ignore);
	parser = roxml_append_parser_item(parser, "\r", _func_xpath_ignore);
	parser = roxml_append_parser_item(parser, "\"", _func_xpath_dquote);
	parser = roxml_append_parser_item(parser, "\'", _func_xpath_quote);
	parser = roxml_append_parser_item(parser, "/", _func_xpath_new_node);
	parser = roxml_append_parser_item(parser, "(", _func_xpath_open_parenthesys);
	parser = roxml_append_parser_item(parser, ")", _func_xpath_close_parenthesys);
	parser = roxml_append_parser_item(parser, "[", _func_xpath_open_brackets);
	parser = roxml_append_parser_item(parser, "]", _func_xpath_close_brackets);
	parser = roxml_append_parser_item(parser, "=", _func_xpath_operator_equal);
	parser = roxml_append_parser_item(parser, ">", _func_xpath_operator_sup);
	parser = roxml_append_parser_item(parser, "<", _func_xpath_operator_inf);
	parser = roxml_append_parser_item(parser, "!", _func_xpath_operator_diff);
	parser = roxml_append_parser_item(parser, "0", _func_xpath_number);
	parser = roxml_append_parser_item(parser, "1", _func_xpath_number);
	parser = roxml_append_parser_item(parser, "2", _func_xpath_number);
	parser = roxml_append_parser_item(parser, "3", _func_xpath_number);
	parser = roxml_append_parser_item(parser, "4", _func_xpath_number);
	parser = roxml_append_parser_item(parser, "5", _func_xpath_number);
	parser = roxml_append_parser_item(parser, "6", _func_xpath_number);
	parser = roxml_append_parser_item(parser, "7", _func_xpath_number);
	parser = roxml_append_parser_item(parser, "8", _func_xpath_number);
	parser = roxml_append_parser_item(parser, "9", _func_xpath_number);
	parser = roxml_append_parser_item(parser, "+", _func_xpath_operator_add);
	parser = roxml_append_parser_item(parser, "-", _func_xpath_operator_subs);
	parser = roxml_append_parser_item(parser, ROXML_PATH_OR, _func_xpath_path_or);
	parser = roxml_append_parser_item(parser, ROXML_COND_OR, _func_xpath_condition_or);
	parser = roxml_append_parser_item(parser, ROXML_COND_AND, _func_xpath_condition_and);
	parser = roxml_append_parser_item(parser, ROXML_FUNC_POS_STR, _func_xpath_position);
	parser = roxml_append_parser_item(parser, ROXML_FUNC_FIRST_STR, _func_xpath_first);
	parser = roxml_append_parser_item(parser, ROXML_FUNC_LAST_STR, _func_xpath_last);
	parser = roxml_append_parser_item(parser, ROXML_FUNC_NSURI_STR, _func_xpath_nsuri);
	parser = roxml_append_parser_item(parser, ROXML_FUNC_LNAME_STR, _func_xpath_lname);
	parser = roxml_append_parser_item(parser, "", _func_xpath_default);

	parser = roxml_parser_prepare(parser);
	ret = roxml_parse_line(parser, path, 0, &ctx);
	roxml_parser_free(parser);

	if (ret >= 0) {
		if (xpath)
			*xpath = ctx.first_node;
		return ctx.nbpath;
	}

	roxml_free_xpath(ctx.first_node, ctx.nbpath);
	return -1;
}

/** \brief node absolute position get
 *
 * \fn roxml_get_node_internal_position(node_t *n);
 * this function returns the absolute position of the node between siblings
 * \param n the node
 * \return the absolute position starting at 1
 */
ROXML_STATIC ROXML_INT int roxml_get_node_internal_position(node_t *n)
{
	int idx = 1;
	node_t *prnt;
	node_t *first;
	if (n == NULL)
		return 0;

	prnt = n->prnt;
	if (!prnt)
		return 1;
	first = prnt->chld;

	while ((first) && (first != n)) {
		idx++;
		first = first->sibl;
	}

	return idx;
}

/** \brief double operation function
 *
 * \fn roxml_double_oper(double a, double b, int op);
 * this function  compare two doubles using one defined operator
 * \param a first operand
 * \param b second operand
 * \param op the operator to use
 * \return 1 if comparison is ok, esle 0
 */
ROXML_STATIC ROXML_INT double roxml_double_oper(double a, double b, int op)
{
	if (op == ROXML_OPERATOR_ADD)
		return (a + b);
	else if (op == ROXML_OPERATOR_SUB)
		return (a - b);
	else if (op == ROXML_OPERATOR_MUL)
		return (a * b);
	else if (op == ROXML_OPERATOR_DIV)
		return (a / b);
	return 0;
}

/** \brief  double comparison function
 *
 * \fn roxml_double_cmp(double a, double b, int op);
 * this function  compare two doubles using one defined operator
 * \param a first operand
 * \param b second operand
 * \param op the operator to use
 * \return 1 if comparison is ok, esle 0
 */
ROXML_STATIC ROXML_INT int roxml_double_cmp(double a, double b, int op)
{
	if (op == ROXML_OPERATOR_DIFF)
		return (a != b);
	else if (op == ROXML_OPERATOR_EINF)
		return (a <= b);
	else if (op == ROXML_OPERATOR_INF)
		return (a < b);
	else if (op == ROXML_OPERATOR_ESUP)
		return (a >= b);
	else if (op == ROXML_OPERATOR_SUP)
		return (a > b);
	else if (op == ROXML_OPERATOR_EQU)
		return (a == b);
	return 0;
}

/** \brief  string comparison function
 *
 * \fn roxml_string_cmp(char *sa, char *sb, int op);
 * this function compare two strings using one defined operator
 * \param sa first operand
 * \param sb second operand
 * \param op the operator to use
 * \return 1 if comparison is ok, else 0
 */
ROXML_STATIC ROXML_INT int roxml_string_cmp(char *sa, char *sb, int op)
{
	int result;

	if (!sa)
		sa = "";
	if (!sb)
		sb = "";

	result = strcmp(sa, sb);

	if (op == ROXML_OPERATOR_DIFF)
		return (result != 0);
	else if (op == ROXML_OPERATOR_EINF)
		return (result <= 0);
	else if (op == ROXML_OPERATOR_INF)
		return (result < 0);
	else if (op == ROXML_OPERATOR_ESUP)
		return (result >= 0);
	else if (op == ROXML_OPERATOR_SUP)
		return (result > 0);
	else if (op == ROXML_OPERATOR_EQU)
		return (result == 0);
	return 0;
}

/** \brief predicat validation function
 *
 * \fn roxml_validate_predicat(xpath_node_t *xn, xpath_cond_t *condition, node_t *candidat);
 * this function check for predicat validity. predicat is part between brackets
 * \param xn the xpath node containing the predicat to test
 * \param condition the xpath condition to test
 * \param candidat the node to test
 * \return 1 if predicat is validated, else 0
 */
ROXML_STATIC ROXML_INT int roxml_validate_predicat(xpath_node_t *xn, xpath_cond_t *condition, node_t *candidat)
{
	int valid;

	if (!condition)
		return 1;

	valid = (condition->rel == ROXML_OPERATOR_AND);

	while (condition) {
		int status = 0;
		double iarg2 = 1;
		double iarg1 = 0;
		char *sarg1 = NULL;
		node_t *val = candidat;
		node_t **node_set;

		if (xn->name[0] == '*')
			iarg1 = roxml_get_node_internal_position(candidat);
		else
			iarg1 = roxml_get_node_position(candidat);

		switch (condition->func) {
		case ROXML_FUNC_POS:
			iarg2 = roxml_strtonum(condition->arg2, NULL);
			status = roxml_double_cmp(iarg1, iarg2, condition->op);
			break;
		case ROXML_FUNC_LAST:
			iarg2 = roxml_get_chld_nb(candidat->prnt);
			  // fall through
		case ROXML_FUNC_FIRST:
			if (condition->op > 0)
				iarg2 = roxml_double_oper(iarg2, roxml_strtonum(condition->arg2, NULL), condition->op);
			status = roxml_double_cmp(iarg1, iarg2, ROXML_OPERATOR_EQU);
			break;
		case ROXML_FUNC_INTCOMP:
			if (condition->arg1)
				val = roxml_get_attr(candidat, condition->arg1 + 1, 0);
			sarg1 = roxml_get_content(val, NULL, 0, &status);
			iarg1 = roxml_strtonum(sarg1, NULL);
			iarg2 = roxml_strtonum(condition->arg2, NULL);
			status = roxml_double_cmp(iarg1, iarg2, condition->op);
			roxml_release(sarg1);
			break;
		case ROXML_FUNC_NSURI:
			val = roxml_get_ns(candidat);
			sarg1 = roxml_get_content(val, NULL, 0, &status);
			status = roxml_string_cmp(sarg1, condition->arg2, condition->op);
			roxml_release(sarg1);
			break;
		case ROXML_FUNC_STRCOMP:
			if (condition->arg1)
				val = roxml_get_attr(candidat, condition->arg1 + 1, 0);
			sarg1 = roxml_get_content(val, NULL, 0, &status);
			status = roxml_string_cmp(sarg1, condition->arg2, condition->op);
			roxml_release(sarg1);
			break;
		case ROXML_FUNC_LNAME:
			sarg1 = roxml_get_name(candidat, NULL, 0);
			status = strcmp(sarg1, condition->arg2) == 0;
			roxml_release(sarg1);
			break;
		case ROXML_FUNC_XPATH:
			val = roxml_get_root(candidat);
			node_set = roxml_exec_xpath(val, candidat, condition->xp, condition->func2, &status);
			roxml_release(node_set);
			status = ! !status;
			break;
		default:
			break;
		}

		if (condition->rel == ROXML_OPERATOR_OR)
			valid = valid || status;
		else if (condition->rel == ROXML_OPERATOR_AND)
			valid = valid && status;
		else
			valid = status;

		condition = condition->next;
	}

	return valid;
}

ROXML_STATIC ROXML_INT void roxml_add_to_set(node_t *root, node_t *candidat, node_t ***ans, int *nb, int *max,
					     int req_id)
{
	if (roxml_add_to_pool(root, candidat, req_id)) {
		if (ans) {
			if ((*nb) >= (*max)) {
				int new_max = (*max) * 2;
				node_t **new_ans = roxml_malloc(sizeof(node_t *), new_max, PTR_NODE_RESULT);
				memcpy(new_ans, (*ans), *(max) * sizeof(node_t *));
				roxml_release(*ans);
				*ans = new_ans;
				*max = new_max;
			}
			(*ans)[*nb] = candidat;
		}
		(*nb)++;
	}
}

ROXML_STATIC ROXML_INT int roxml_validate_axe_func(node_t *root, node_t **candidat, xpath_node_t *xn)
{
	int valid = 0;
	char *axes = xn->name;
	int type = (*candidat)->type;

	if ((axes == NULL) || (strcmp("node()", axes) == 0)) {
		valid = 1;
	} else if (strcmp("*", axes) == 0) {
		if (type & (ROXML_ELM_NODE | ROXML_ATTR_NODE))
			valid = 1;
	} else if (strcmp("comment()", axes) == 0) {
		if (type & ROXML_CMT_NODE)
			valid = 1;
	} else if (strcmp("processing-instruction()", axes) == 0) {
		if (type & ROXML_PI_NODE)
			valid = 1;
	} else if (strcmp("text()", axes) == 0) {
		if (type & ROXML_TXT_NODE)
			valid = 1;
	} else if (strcmp("", axes) == 0) {
		if (xn->abs) {
			*candidat = root;
			valid = 1;
		}
	}

	/* comments and pi can only be reached using their comment()
	 * and processing-instruction() functions */
	if (!valid)
		if (type & (ROXML_PI_NODE | ROXML_CMT_NODE))
			return -1;
	return valid;
}

ROXML_STATIC ROXML_INT int roxml_validate_axe_name(node_t *candidat, xpath_node_t *xn)
{
	int valid = 0;
	int ns_len;
	char *name;

	ROXML_GET_BASE_BUFFER(intern_buff);

	if (candidat->ns) {
		name = roxml_get_name(candidat->ns, intern_buff, ROXML_BASE_LEN);
		ns_len = strlen(name);
		if (ns_len)
			name[ns_len++] = ':';
	} else {
		ns_len = 0;
		name = intern_buff;
	}

	roxml_get_name(candidat, name + ns_len, ROXML_BASE_LEN - ns_len);
	if (name && strcmp(name, xn->name) == 0)
		valid = 1;

	ROXML_PUT_BASE_BUFFER(intern_buff);

	return valid;
}

/** \brief axe validation function
 *
 * \fn roxml_validate_axes(node_t *root, node_t *candidat, node_t ***ans, int *nb, int *max, xpath_node_t *xn, int req_id);
 * this function validate if an axe is matching the current node
 * \param root the root node
 * \param candidat the node to test
 * \param ans the pointer to answers pool
 * \param nb the number of answers in pool
 * \param max the current size of the pool
 * \param xn the xpath node to test
 * \param req_id the pool id
 * \return 1 if axe is validated, else 0
 */
ROXML_STATIC ROXML_INT int roxml_validate_axes(node_t *root, node_t *candidat, node_t ***ans, int *nb, int *max,
					       xpath_node_t *xn, int req_id)
{
	int valid = 0;
	int path_end = 0;
	xpath_node_t empty;

	if (xn == NULL) {
		valid = 1;
		path_end = 1;
		xn = &empty;
		memset(xn, 0, sizeof(empty));
	} else {
		valid = roxml_validate_axe_func(root, &candidat, xn);

		if (xn->next == NULL)
			path_end = 1;
		if ((xn->axes == ROXML_ID_SELF) || (xn->axes == ROXML_ID_PARENT))
			valid = 1;

		if (valid == 0)
			valid = roxml_validate_axe_name(candidat, xn);
	}

	if (valid == 1)
		valid = roxml_validate_predicat(xn, xn->cond, candidat);
	if (valid == 1)
		valid = roxml_validate_predicat(xn, xn->xp_cond, candidat);

	if ((valid == 1) && (path_end))
		roxml_add_to_set(root, candidat, ans, nb, max, req_id);

	return (valid == 1);
}

/** \brief real xpath validation function
 *
 * \fn roxml_check_node(xpath_node_t *xp, node_t *root, node_t *context, node_t ***ans, int *nb, int *max, int ignore, int req_id);
 * this function perform the xpath test on a tree
 * \param xp the xpath nodes to test
 * \param root the root node
 * \param context the current context node
 * \param ans the pointer to answers pool
 * \param nb the number of answers in pool
 * \param max the current size of the pool
 * \param ignore a flag for some axes that are going thru all document
 * \param req_id the pool id
 * \return
 */
ROXML_STATIC ROXML_INT void roxml_check_node(xpath_node_t *xp, node_t *root, node_t *context, node_t ***ans,
					     int *nb, int *max, int ignore, int req_id)
{
	int validate_node = 0;

	if ((req_id == 0) && (*nb > 0))
		return;

	if (!xp)
		return;

	// if found a "all document" axes
	if (ignore == ROXML_DESC_ONLY) {
		node_t *current = context->chld;
		while (current) {
			roxml_check_node(xp, root, current, ans, nb, max, ignore, req_id);
			current = current->sibl;
		}
	}

	switch (xp->axes) {
	case ROXML_ID_CHILD:{
			node_t *current = context->chld;
			while (current) {
				validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
				if (validate_node)
					roxml_check_node(xp->next, root, current, ans, nb, max, ROXML_DIRECT, req_id);
				current = current->sibl;
			}
			if ((xp->name == NULL) || (strcmp(xp->name, "text()") == 0)
			    || (strcmp(xp->name, "node()") == 0)) {
				node_t *current = roxml_get_txt(context, 0);
				while (current) {
					validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
					current = current->sibl;
				}
			}
			if ((xp->name == NULL) || (strcmp(xp->name, "node()") == 0)) {
				node_t *current = context->attr;
				while (current) {
					validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
					current = current->sibl;
				}
			}
		}
		break;
	case ROXML_ID_DESC:{
			roxml_check_node(xp->next, root, context, ans, nb, max, ROXML_DESC_ONLY, req_id);
		}
		break;
	case ROXML_ID_DESC_O_SELF:{
			xp = xp->next;
			validate_node = roxml_validate_axes(root, context, ans, nb, max, xp, req_id);
			if (validate_node)
				roxml_check_node(xp->next, root, context, ans, nb, max, ROXML_DIRECT, req_id);
			roxml_check_node(xp, root, context, ans, nb, max, ROXML_DESC_ONLY, req_id);
		}
		break;
	case ROXML_ID_SELF:{
			validate_node = roxml_validate_axes(root, context, ans, nb, max, xp, req_id);
			roxml_check_node(xp->next, root, context, ans, nb, max, ROXML_DIRECT, req_id);
		}
		break;
	case ROXML_ID_PARENT:{
			if (context->prnt) {
				validate_node = roxml_validate_axes(root, context->prnt, ans, nb, max, xp, req_id);
				roxml_check_node(xp->next, root, context->prnt, ans, nb, max, ROXML_DIRECT, req_id);
			} else {
				validate_node = 0;
			}
		}
		break;
	case ROXML_ID_ATTR:{
			node_t *attribute = context->attr;
			while (attribute) {
				validate_node = roxml_validate_axes(root, attribute, ans, nb, max, xp, req_id);
				if (validate_node)
					roxml_check_node(xp->next, root, context, ans, nb, max, ROXML_DIRECT, req_id);
				attribute = attribute->sibl;
			}
		}
		break;
	case ROXML_ID_ANC:{
			node_t *current = context->prnt;
			while (current) {
				validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
				if (validate_node)
					roxml_check_node(xp->next, root, current, ans, nb, max, ROXML_DIRECT, req_id);
				current = current->prnt;
			}
		}
		break;
	case ROXML_ID_NEXT_SIBL:{
			node_t *current = context->sibl;
			while (current) {
				validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
				if (validate_node)
					roxml_check_node(xp->next, root, current, ans, nb, max, ROXML_DIRECT, req_id);
				current = current->sibl;
			}
		}
		break;
	case ROXML_ID_PREV_SIBL:{
			node_t *current = context->prnt->chld;
			while (current != context) {
				validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
				if (validate_node)
					roxml_check_node(xp->next, root, current, ans, nb, max, ROXML_DIRECT, req_id);
				current = current->sibl;
			}
		}
		break;
	case ROXML_ID_NEXT:{
			node_t *current = context;
			while (current) {
				node_t *following = current->sibl;
				while (following) {
					validate_node = roxml_validate_axes(root, following, ans, nb, max, xp, req_id);
					if (validate_node) {
						roxml_check_node(xp->next, root, following, ans, nb, max, ROXML_DIRECT,
								 req_id);
					} else {
						xp->axes = ROXML_ID_CHILD;
						roxml_check_node(xp, root, following, ans, nb, max, ROXML_DESC_ONLY,
								 req_id);
						xp->axes = ROXML_ID_NEXT;
					}
					following = following->sibl;
				}
				following = current->prnt->chld;
				while (following != current)
					following = following->sibl;
				current = following->sibl;
			}
		}
		break;
	case ROXML_ID_PREV:{
			node_t *current = context;
			while (current && current->prnt) {
				node_t *preceding = current->prnt->chld;
				while (preceding != current) {
					validate_node = roxml_validate_axes(root, preceding, ans, nb, max, xp, req_id);
					if (validate_node) {
						roxml_check_node(xp->next, root, preceding, ans, nb, max, ROXML_DIRECT,
								 req_id);
					} else {
						xp->axes = ROXML_ID_CHILD;
						roxml_check_node(xp, root, preceding, ans, nb, max, ROXML_DESC_ONLY,
								 req_id);
						xp->axes = ROXML_ID_PREV;
					}
					preceding = preceding->sibl;
				}
				current = current->prnt;
			}
		}
		break;
	case ROXML_ID_NS:{
			validate_node = roxml_validate_axes(root, context->ns, ans, nb, max, xp, req_id);
			if (validate_node)
				roxml_check_node(xp->next, root, context, ans, nb, max, ROXML_DIRECT, req_id);
		}
		break;
	case ROXML_ID_ANC_O_SELF:{
			node_t *current = context;
			while (current) {
				validate_node = roxml_validate_axes(root, current, ans, nb, max, xp, req_id);
				if (validate_node)
					roxml_check_node(xp->next, root, current, ans, nb, max, ROXML_DIRECT, req_id);
				current = current->prnt;
			}
		}
		break;
	}

	return;
}

ROXML_INT node_t **roxml_exec_xpath(node_t *root, node_t *n, xpath_node_t *xpath, int index, int *count)
{
	int path_id;
	int max_answers = 1;
	int glob_id = 0;
	int *req_ids = NULL;
	node_t **node_set;

	*count = 0;
	glob_id = roxml_request_id(root);
	if (glob_id < 0)
		return NULL;
	req_ids = calloc(index, sizeof(int));
	node_set = roxml_malloc(sizeof(node_t *), max_answers, PTR_NODE_RESULT);

	/* process all AND xpath */
	for (path_id = 0; path_id < index; path_id++) {
		xpath_node_t *cur_xpath = NULL;
		xpath_node_t *next_xpath = NULL;
		cur_xpath = &xpath[path_id];

		if (path_id < index - 1)
			next_xpath = &xpath[path_id + 1];

		if ((cur_xpath->rel == ROXML_OPERATOR_AND) || ((next_xpath) && (next_xpath->rel == ROXML_OPERATOR_AND))) {
			int req_id = roxml_request_id(root);
			node_t *orig = n;

			if (cur_xpath->abs)
				/* context node is root */
				orig = root;

			roxml_check_node(cur_xpath, root, orig, &node_set, count, &max_answers, ROXML_DIRECT, req_id);

			if (cur_xpath->rel == ROXML_OPERATOR_AND)
				roxml_compute_and(root, node_set, count, req_id, req_ids[path_id - 1]);
			req_ids[path_id] = req_id;
		}
	}

	/* process all OR xpath */
	for (path_id = 0; path_id < index; path_id++) {
		xpath_node_t *cur_xpath = &xpath[path_id];

		if (cur_xpath->rel == ROXML_OPERATOR_OR) {
			node_t *orig = n;

			if (req_ids[path_id] == 0) {
				if (cur_xpath->abs)
					orig = root;

				/* assign a new request ID */
				roxml_check_node(cur_xpath, root, orig, &node_set, count, &max_answers, ROXML_DIRECT,
						 glob_id);
			} else {
				roxml_compute_or(root, node_set, count, req_ids[path_id + 1], glob_id);
				roxml_release_id(root, node_set, *count, req_ids[path_id + 1]);
			}
		}
	}

	roxml_release_id(root, node_set, *count, glob_id);

	for (path_id = 0; path_id < index; path_id++)
		if (req_ids[path_id] != 0)
			roxml_release_id(root, node_set, *count, req_ids[path_id]);
	free(req_ids);

	return node_set;
}

ROXML_API node_t **roxml_xpath(node_t *n, char *path, int *nb_ans)
{
	int count = 0;
	node_t **node_set = NULL;
	int index = 0;
	xpath_node_t *xpath = NULL;
	node_t *root = n;
	char *full_path_to_find;

	if (n == ROXML_INVALID_DOC) {
		if (nb_ans)
			*nb_ans = 0;
		return NULL;
	}

	root = roxml_get_root(n);

	full_path_to_find = strdup(path);

	index = roxml_parse_xpath(full_path_to_find, &xpath, 0);

	if (index >= 0) {
		node_set = roxml_exec_xpath(root, n, xpath, index, &count);
		roxml_free_xpath(xpath, index);

		if (count == 0) {
			roxml_release(node_set);
			node_set = NULL;
		}
	}
	if (nb_ans)
		*nb_ans = count;
	free(full_path_to_find);

	return node_set;
}

/** \brief axes setter function
 *
 * \fn roxml_set_axes(xpath_node_t *node, char *axes, int *offset);
 * this function set the axe to a xpath node from xpath string
 * \param node the xpath node to fill
 * \param axes the string where axe is extracted from
 * \param offset the detected offset in axe string
 * \return the filled xpath_node
 */
ROXML_STATIC ROXML_INT xpath_node_t *roxml_set_axes(xpath_node_t *node, char *axes, int *offset)
{
	struct _xpath_axes {
		char id;
		char *name;
	};

	struct _xpath_axes xpath_axes[14] = {
		{ROXML_ID_PARENT, ROXML_L_PARENT},
		{ROXML_ID_PARENT, ROXML_S_PARENT},
		{ROXML_ID_SELF, ROXML_L_SELF},
		{ROXML_ID_SELF, ROXML_S_SELF},
		{ROXML_ID_ATTR, ROXML_L_ATTR},
		{ROXML_ID_ATTR, ROXML_S_ATTR},
		{ROXML_ID_ANC, ROXML_L_ANC},
		{ROXML_ID_ANC_O_SELF, ROXML_L_ANC_O_SELF},
		{ROXML_ID_NEXT_SIBL, ROXML_L_NEXT_SIBL},
		{ROXML_ID_PREV_SIBL, ROXML_L_PREV_SIBL},
		{ROXML_ID_NEXT, ROXML_L_NEXT},
		{ROXML_ID_PREV, ROXML_L_PREV},
		{ROXML_ID_NS, ROXML_L_NS},
		{ROXML_ID_CHILD, ROXML_L_CHILD},
	};

	xpath_node_t *tmp_node;
	if (axes[0] == '/') {
		axes[0] = '\0';
		*offset += 1;
		axes++;
	}
	if (axes[0] == '/') {
		/* ROXML_S_DESC_O_SELF */
		node->axes = ROXML_ID_DESC_O_SELF;
		node->name = axes + 1;
		tmp_node = calloc(1, sizeof(xpath_node_t));
		tmp_node->axes = ROXML_ID_CHILD;
		node->next = tmp_node;
		if (strlen(node->name) > 0) {
			tmp_node = calloc(1, sizeof(xpath_node_t));
			node->next->next = tmp_node;
			node = roxml_set_axes(tmp_node, axes + 1, offset);
		}
	} else if (strncmp(ROXML_L_DESC_O_SELF, axes, strlen(ROXML_L_DESC_O_SELF)) == 0) {
		/* ROXML_L_DESC_O_SELF */
		node->axes = ROXML_ID_DESC_O_SELF;
		node->name = axes + strlen(ROXML_L_DESC_O_SELF);
		*offset += strlen(ROXML_L_DESC_O_SELF);
		tmp_node = calloc(1, sizeof(xpath_node_t));
		tmp_node->axes = ROXML_ID_CHILD;
		node->next = tmp_node;
		node = roxml_set_axes(tmp_node, axes + strlen(ROXML_L_DESC_O_SELF), offset);
	} else if (strncmp(ROXML_L_DESC, axes, strlen(ROXML_L_DESC)) == 0) {
		/* ROXML_L_DESC */
		node->axes = ROXML_ID_DESC;
		node->name = axes + strlen(ROXML_L_DESC);
		*offset += strlen(ROXML_L_DESC);
		tmp_node = calloc(1, sizeof(xpath_node_t));
		tmp_node->axes = ROXML_ID_CHILD;
		node->next = tmp_node;
		node = roxml_set_axes(tmp_node, axes + strlen(ROXML_L_DESC), offset);
	} else {
		int i = 0;

		/* ROXML_S_CHILD is default */
		node->axes = ROXML_ID_CHILD;
		node->name = axes;

		for (i = 0; i < 14; i++) {
			int len = strlen(xpath_axes[i].name);
			if (strncmp(xpath_axes[i].name, axes, len) == 0) {
				node->axes = xpath_axes[i].id;
				node->name = axes + len;
				break;
			}
		}
	}
	return node;
}

ROXML_INT int _func_xpath_ignore(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	return 1;
}

ROXML_INT int _func_xpath_new_node(roxml_parser_item_t *parser, char *chunk, void *data)
{
	int cur = 0;
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	if (!ctx->quoted && !ctx->dquoted && !ctx->parenthesys && !ctx->bracket) {
		int offset = 0;
		xpath_node_t *tmp_node = calloc(1, sizeof(xpath_node_t));
		if ((chunk[cur] == '/') && (ctx->is_first_node)) {
			free(tmp_node);
			ctx->new_node = ctx->first_node;
			ctx->first_node->abs = 1;
		} else if ((chunk[cur] == '/') && (ctx->wait_first_node)) {
			free(tmp_node);
			ctx->first_node->abs = 1;
		} else if ((ctx->is_first_node) || (ctx->wait_first_node)) {
			free(tmp_node);
		} else {
			if (ctx->new_node)
				ctx->new_node->next = tmp_node;
			ctx->new_node = tmp_node;
		}
		ctx->is_first_node = 0;
		ctx->wait_first_node = 0;
		ctx->new_node = roxml_set_axes(ctx->new_node, chunk + cur, &offset);
		cur = offset + 1;
	}
	ctx->shorten_cond = 0;
	return cur;
}

ROXML_INT int _func_xpath_quote(roxml_parser_item_t *parser, char *chunk, void *data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	if (!ctx->dquoted) {
		if (ctx->quoted && ctx->content_quoted == MODE_COMMENT_QUOTE) {
			ctx->content_quoted = MODE_COMMENT_NONE;
			chunk[0] = '\0';
		}
		ctx->quoted = (ctx->quoted + 1) % 2;
	}
	ctx->shorten_cond = 0;
	return 1;
}

ROXML_INT int _func_xpath_dquote(roxml_parser_item_t *parser, char *chunk, void *data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	if (!ctx->quoted) {
		if (ctx->dquoted && ctx->content_quoted == MODE_COMMENT_DQUOTE) {
			ctx->content_quoted = MODE_COMMENT_NONE;
			chunk[0] = '\0';
		}
		ctx->dquoted = (ctx->dquoted + 1) % 2;
	}
	ctx->shorten_cond = 0;
	return 1;
}

ROXML_INT int _func_xpath_open_parenthesys(roxml_parser_item_t *parser, char *chunk, void *data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	if (!ctx->quoted && !ctx->dquoted)
		ctx->parenthesys = (ctx->parenthesys + 1) % 2;
	ctx->shorten_cond = 0;
	return 1;
}

ROXML_INT int _func_xpath_close_parenthesys(roxml_parser_item_t *parser, char *chunk, void *data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	if (!ctx->quoted && !ctx->dquoted)
		ctx->parenthesys = (ctx->parenthesys + 1) % 2;
	ctx->shorten_cond = 0;
	return 1;
}

ROXML_INT int _func_xpath_open_brackets(roxml_parser_item_t *parser, char *chunk, void *data)
{
	xpath_cond_t *tmp_cond;
	int cur = 0;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
	if (!ctx->quoted && !ctx->dquoted) {
		ctx->bracket = (ctx->bracket + 1) % 2;
		chunk[0] = '\0';

		ctx->shorten_cond = 1;
		tmp_cond = calloc(1, sizeof(xpath_cond_t));
		ctx->new_node->cond = tmp_cond;
		ctx->new_cond = tmp_cond;
		ctx->new_cond->arg1 = chunk + cur + 1;
	} else {
		ctx->shorten_cond = 0;
	}
	cur++;
	return 1;
}

ROXML_INT int _func_xpath_close_brackets(roxml_parser_item_t *parser, char *chunk, void *data)
{
	int cur = 0;
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	if (!ctx->quoted && !ctx->dquoted) {
		ctx->bracket = (ctx->bracket + 1) % 2;
		chunk[0] = '\0';

		if (ctx->new_cond) {
			if (ctx->new_cond->func == ROXML_FUNC_XPATH) {
				xpath_node_t *xp;
				ctx->new_cond->func2 = roxml_parse_xpath(ctx->new_cond->arg1, &xp, 1);
				ctx->new_cond->xp = xp;
			}
		} else {
			return -1;
		}
	}
	cur++;
	ctx->shorten_cond = 0;
	return 1;
}

ROXML_INT int _func_xpath_condition_or(roxml_parser_item_t *parser, char *chunk, void *data)
{
	xpath_node_t *tmp_node;
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
	int cur = 0;
	int len = 0;
	xpath_cond_t *tmp_cond;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */

	len = strlen(ROXML_COND_OR);

	if (strncmp(chunk, ROXML_COND_OR, len) == 0) {
		if (roxml_is_separator(*(chunk - 1)) && roxml_is_separator(*(chunk + len))) {
			if (!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
				if (ctx->context != 1) {
					return 0;
				}
				chunk[-1] = '\0';
				cur += strlen(ROXML_COND_OR);
				tmp_node = calloc(ctx->nbpath + 1, sizeof(xpath_node_t));
				memcpy(tmp_node, ctx->first_node, ctx->nbpath * sizeof(xpath_node_t));
				free(ctx->first_node);
				ctx->first_node = tmp_node;
				ctx->wait_first_node = 1;
				ctx->new_node = tmp_node + ctx->nbpath;
				ctx->new_node->rel = ROXML_OPERATOR_OR;
				ctx->nbpath++;
			} else if (ctx->bracket && !ctx->quoted && !ctx->dquoted) {
				if (ctx->new_cond->func != ROXML_FUNC_XPATH) {
					chunk[-1] = '\0';
					cur += strlen(ROXML_COND_OR);
					tmp_cond = calloc(1, sizeof(xpath_cond_t));
					if (ctx->new_cond) {
						ctx->new_cond->next = tmp_cond;
					}
					ctx->new_cond = tmp_cond;
					ctx->new_cond->rel = ROXML_OPERATOR_OR;
					ctx->new_cond->arg1 = chunk + cur + 1;
				}
			}
		}
	}
	if (cur)
		ctx->shorten_cond = 0;
	return cur;
}

ROXML_INT int _func_xpath_condition_and(roxml_parser_item_t *parser, char *chunk, void *data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
	int cur = 0;
	int len = 0;
	xpath_node_t *tmp_node;
	xpath_cond_t *tmp_cond;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */

	len = strlen(ROXML_COND_AND);

	if (strncmp(chunk, ROXML_COND_AND, len) == 0) {
		if (roxml_is_separator(*(chunk - 1)) && roxml_is_separator(*(chunk + len))) {
			if (!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
				if (ctx->context != 1)
					return 0;
				chunk[-1] = '\0';
				cur += strlen(ROXML_COND_AND);
				tmp_node = calloc(ctx->nbpath + 1, sizeof(xpath_node_t));
				memcpy(tmp_node, ctx->first_node, ctx->nbpath * sizeof(xpath_node_t));
				free(ctx->first_node);
				ctx->first_node = tmp_node;
				ctx->wait_first_node = 1;
				ctx->new_node = tmp_node + ctx->nbpath;
				ctx->new_node->rel = ROXML_OPERATOR_AND;
				ctx->nbpath++;
			} else if (ctx->bracket && !ctx->quoted && !ctx->dquoted) {
				if (ctx->new_cond->func != ROXML_FUNC_XPATH) {
					chunk[-1] = '\0';
					cur += strlen(ROXML_COND_AND);
					tmp_cond = calloc(1, sizeof(xpath_cond_t));
					if (ctx->new_cond) {
						ctx->new_cond->next = tmp_cond;
					}
					ctx->new_cond = tmp_cond;
					ctx->new_cond->rel = ROXML_OPERATOR_AND;
					ctx->new_cond->arg1 = chunk + cur + 1;
				}
			}
		}
	}
	if (cur)
		ctx->shorten_cond = 0;
	return cur;
}

ROXML_INT int _func_xpath_path_or(roxml_parser_item_t *parser, char *chunk, void *data)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
	int cur = 0;
	xpath_node_t *tmp_node;
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */

	if (!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		chunk[-1] = '\0';
		cur += strlen(ROXML_PATH_OR);
		tmp_node = calloc(ctx->nbpath + 1, sizeof(xpath_node_t));
		memcpy(tmp_node, ctx->first_node, ctx->nbpath * sizeof(xpath_node_t));
		free(ctx->first_node);
		ctx->first_node = tmp_node;
		ctx->wait_first_node = 1;
		ctx->new_node = tmp_node + ctx->nbpath;
		ctx->new_node->rel = ROXML_OPERATOR_OR;
		ctx->nbpath++;
	}
	ctx->shorten_cond = 0;
	return cur;
}

ROXML_INT int _func_xpath_operators(roxml_parser_item_t *parser, char *chunk, void *data, int operator, int operator_bis)
{
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
	int cur = 0;
	if (!ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		xpath_node_t *xp_root = ctx->new_node;
		xpath_cond_t *xp_cond = calloc(1, sizeof(xpath_cond_t));
		xp_root->xp_cond = xp_cond;
		chunk[cur] = '\0';
		xp_cond->op = operator;
		if (ROXML_WHITE(chunk[cur - 1]))
			chunk[cur - 1] = '\0';
		if (chunk[cur + 1] == '=') {
			chunk[++cur] = '\0';
			xp_cond->op = operator_bis;
		}
		if (ROXML_WHITE(chunk[cur + 1]))
			chunk[++cur] = '\0';

		xp_cond->arg2 = chunk + cur + 1;
		if (xp_cond->arg2[0] == '"') {
			ctx->content_quoted = MODE_COMMENT_DQUOTE;
			xp_cond->arg2++;
		} else if (xp_cond->arg2[0] == '\'') {
			ctx->content_quoted = MODE_COMMENT_QUOTE;
			xp_cond->arg2++;
		}
		if (!xp_cond->func) {
			xp_cond->func = ROXML_FUNC_INTCOMP;
			if (!roxml_is_number(xp_cond->arg2))
				xp_cond->func = ROXML_FUNC_STRCOMP;
		}
		cur++;
	} else if (ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if (ctx->new_cond->func != ROXML_FUNC_XPATH) {
			chunk[cur] = '\0';
			ctx->new_cond->op = operator;
			if (ROXML_WHITE(chunk[cur - 1]))
				chunk[cur - 1] = '\0';
			if (chunk[cur + 1] == '=') {
				chunk[++cur] = '\0';
				ctx->new_cond->op = operator_bis;
			}
			if (ROXML_WHITE(chunk[cur + 1]))
				chunk[++cur] = '\0';
			ctx->new_cond->arg2 = chunk + cur + 1;
			if (ctx->new_cond->arg2[0] == '"') {
				ctx->content_quoted = MODE_COMMENT_DQUOTE;
				ctx->new_cond->arg2++;
			} else if (ctx->new_cond->arg2[0] == '\'') {
				ctx->content_quoted = MODE_COMMENT_QUOTE;
				ctx->new_cond->arg2++;
			}
			if (ctx->new_cond->func == 0) {
				ctx->new_cond->func = ROXML_FUNC_INTCOMP;
				if (!roxml_is_number(ctx->new_cond->arg2))
					ctx->new_cond->func = ROXML_FUNC_STRCOMP;
			}
			cur++;
		}
	}
	ctx->shorten_cond = 0;
	return cur;
}

ROXML_INT int _func_xpath_operator_equal(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	return _func_xpath_operators(parser, chunk, data, ROXML_OPERATOR_EQU, ROXML_OPERATOR_EQU);
}

ROXML_INT int _func_xpath_operator_sup(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	return _func_xpath_operators(parser, chunk, data, ROXML_OPERATOR_SUP, ROXML_OPERATOR_ESUP);
}

ROXML_INT int _func_xpath_operator_inf(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	return _func_xpath_operators(parser, chunk, data, ROXML_OPERATOR_INF, ROXML_OPERATOR_EINF);
}

ROXML_INT int _func_xpath_operator_diff(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	return _func_xpath_operators(parser, chunk, data, ROXML_OPERATOR_DIFF, ROXML_OPERATOR_DIFF);
}

ROXML_INT int _func_xpath_number(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
	int cur = 0;
	if (ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if ((ctx->new_cond->func != ROXML_FUNC_XPATH) && (ctx->shorten_cond)) {
			cur = 1;
			ctx->new_cond->func = ROXML_FUNC_POS;
			ctx->new_cond->op = ROXML_OPERATOR_EQU;
			ctx->new_cond->arg2 = chunk;
			while ((chunk[cur + 1] >= '0') && (chunk[cur + 1] <= '9'))
				cur++;
		}
	}
	ctx->shorten_cond = 0;
	return cur;
}

ROXML_INT int _func_xpath_funcs(roxml_parser_item_t *parser, char *chunk, void *data, int func, char *name)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
	int cur = 0;

	if (strncmp(chunk, name, strlen(name)) == 0) {
		if (ctx->new_cond->func != func) {
			cur += strlen(name);
			ctx->new_cond->func = func;
		}
	}
	if (cur)
		ctx->shorten_cond = 0;
	return cur;
}

ROXML_INT int _func_xpath_position(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	return _func_xpath_funcs(parser, chunk, data, ROXML_FUNC_POS, ROXML_FUNC_POS_STR);
}

ROXML_INT int _func_xpath_first(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	return _func_xpath_funcs(parser, chunk, data, ROXML_FUNC_FIRST, ROXML_FUNC_FIRST_STR);
}

ROXML_INT int _func_xpath_last(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	return _func_xpath_funcs(parser, chunk, data, ROXML_FUNC_LAST, ROXML_FUNC_LAST_STR);
}

ROXML_INT int _func_xpath_nsuri(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	return _func_xpath_funcs(parser, chunk, data, ROXML_FUNC_NSURI, ROXML_FUNC_NSURI_STR);
}

ROXML_INT int _func_xpath_lname(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	return _func_xpath_funcs(parser, chunk, data, ROXML_FUNC_LNAME, ROXML_FUNC_LNAME_STR);
}

ROXML_INT int _func_xpath_operator_add(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
	int cur = 0;
	if (ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if (ctx->new_cond->func != ROXML_FUNC_XPATH) {
			if ((ctx->new_cond->func == ROXML_FUNC_LAST) || (ctx->new_cond->func == ROXML_FUNC_FIRST))
				ctx->new_cond->op = ROXML_OPERATOR_ADD;
			chunk[cur] = '\0';
			if (ROXML_WHITE(chunk[cur + 1]))
				chunk[++cur] = '\0';
			ctx->new_cond->arg2 = chunk + cur + 1;
		}
	}
	ctx->shorten_cond = 0;
	return cur;
}

ROXML_INT int _func_xpath_operator_subs(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;
	int cur = 0;
	if (ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if (ctx->new_cond->func != ROXML_FUNC_XPATH) {
			if ((ctx->new_cond->func == ROXML_FUNC_LAST) || (ctx->new_cond->func == ROXML_FUNC_FIRST))
				ctx->new_cond->op = ROXML_OPERATOR_SUB;
			chunk[cur] = '\0';
			if (ROXML_WHITE(chunk[cur + 1]))
				chunk[++cur] = '\0';
			ctx->new_cond->arg2 = chunk + cur + 1;
		}
	}
	ctx->shorten_cond = 0;
	return cur;
}

ROXML_INT int _func_xpath_default(roxml_parser_item_t *parser, char *chunk, void *data)
{
#ifdef DEBUG_PARSING
	fprintf(stderr, "calling func %s chunk %c\n", __func__, chunk[0]);
#endif /* DEBUG_PARSING */
	int cur = 0;
	roxml_xpath_ctx_t *ctx = (roxml_xpath_ctx_t *)data;

	if ((ctx->is_first_node) || (ctx->wait_first_node)) {
		if (!ctx->quoted && !ctx->dquoted && !ctx->parenthesys && !ctx->bracket) {
			int offset = 0;
			xpath_node_t *tmp_node = calloc(1, sizeof(xpath_node_t));
			if ((chunk[cur] == '/') && (ctx->is_first_node)) {
				free(tmp_node);
				ctx->new_node = ctx->first_node;
				ctx->first_node->abs = 1;
			} else if ((chunk[cur] == '/') && (ctx->wait_first_node)) {
				free(tmp_node);
				ctx->first_node->abs = 1;
			} else if ((ctx->is_first_node) || (ctx->wait_first_node)) {
				free(tmp_node);
			} else {
				if (ctx->new_node)
					ctx->new_node->next = tmp_node;
				ctx->new_node = tmp_node;
			}
			ctx->is_first_node = 0;
			ctx->wait_first_node = 0;
			ctx->new_node = roxml_set_axes(ctx->new_node, chunk + cur, &offset);
			cur += offset;
		}
	} else if (ctx->bracket && !ctx->quoted && !ctx->dquoted) {
		if (ctx->new_cond->func != ROXML_FUNC_XPATH) {
			if (ctx->shorten_cond) {
				int bracket_lvl = 1;
				ctx->new_cond->func = ROXML_FUNC_XPATH;
				ctx->new_cond->arg1 = chunk + cur;
				while (bracket_lvl > 0) {
					if (chunk[cur] == '[')
						bracket_lvl++;
					else if (chunk[cur] == ']')
						bracket_lvl--;
					cur++;
				}
				cur--;
			}
		}
	}
	ctx->shorten_cond = 0;
	return cur > 0 ? cur : 1;
}
