/*
 * json_helper.c - JSON utility functions.
 *
 * See LICENSE file for license related information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <easy/easy.h>

#define JSON_MAX_DEPTH 10
#define JSON_MAX_TOKENS 500

#include "json_helper.h"

void json_escape_string(char *txt, size_t maxlen, const char *data, size_t len)
{
	char *end = txt + maxlen;
	size_t i;

	for (i = 0; i < len; i++) {
		if (txt + 4 >= end)
			break;

		switch (data[i]) {
		case '\"':
			*txt++ = '\\';
			*txt++ = '\"';
			break;
		case '\\':
			*txt++ = '\\';
			*txt++ = '\\';
			break;
		case '\n':
			*txt++ = '\\';
			*txt++ = 'n';
			break;
		case '\r':
			*txt++ = '\\';
			*txt++ = 'r';
			break;
		case '\t':
			*txt++ = '\\';
			*txt++ = 't';
			break;
		default:
			if (data[i] >= 32 && data[i] <= 126) {
				*txt++ = data[i];
			} else {
				txt += snprintf(txt, end - txt, "\\u%04x",
						   (unsigned char) data[i]);
			}
			break;
		}
	}

	*txt = '\0';
}

void json_free(struct json_token *json)
{
	if (!json)
		return;

	json_free(json->child);
	json_free(json->sibling);
	free(json->name);
	free(json->string);
	free(json);
}

static char * json_parse_string(const char **json_pos, const char *end)
{
	const char *pos = *json_pos;
	char *str, *spos, *s_end;
	size_t max_len, buf_len;
	uint8_t bin[2];

	pos++; /* skip starting quote */

	max_len = end - pos + 1;
	buf_len = max_len > 10 ? 10 : max_len;
	str = calloc(1, buf_len);
	if (!str)
		return NULL;

	spos = str;
	s_end = str + buf_len;

	for (; pos < end; pos++) {
		if (buf_len < max_len && s_end - spos < 3) {
			char *tmp;
			int idx;

			idx = spos - str;
			buf_len *= 2;
			if (buf_len > max_len)
				buf_len = max_len;
			tmp = realloc(str, buf_len);
			if (!tmp)
				goto fail;
			str = tmp;
			spos = str + idx;
			s_end = str + buf_len;
		}

		switch (*pos) {
		case '\"': /* end string */
			*spos = '\0';
			/* caller will move to the next position */
			*json_pos = pos;
			return str;
		case '\\':
			pos++;
			if (pos >= end) {
				fprintf(stderr, "JSON: Truncated \\ escape\n");
				goto fail;
			}
			switch (*pos) {
			case '"':
			case '\\':
			case '/':
				*spos++ = *pos;
				break;
			case 'n':
				*spos++ = '\n';
				break;
			case 'r':
				*spos++ = '\r';
				break;
			case 't':
				*spos++ = '\t';
				break;
			case 'u':
				if (end - pos < 5 ||
				    strtob((char *)(pos + 1), 2, bin) < 0 ||
				    bin[1] == 0x00) {
					fprintf(stderr, "JSON: Invalid \\u escape\n");
					goto fail;
				}
				if (bin[0] == 0x00) {
					*spos++ = bin[1];
				} else {
					*spos++ = bin[0];
					*spos++ = bin[1];
				}
				pos += 4;
				break;
			default:
				fprintf(stderr, "JSON: Unknown escape '%c'\n", *pos);
				goto fail;
			}
			break;
		default:
			*spos++ = *pos;
			break;
		}
	}

fail:
	free(str);
	return NULL;
}


static int json_parse_number(const char **json_pos, const char *end,
			     int *ret_val)
{
	const char *pos = *json_pos;
	size_t len;
	char *str;

	for (; pos < end; pos++) {
		if (*pos != '-' && (*pos < '0' || *pos > '9')) {
			pos--;
			break;
		}
	}
	if (pos == end)
		pos--;
	if (pos < *json_pos)
		return -1;
	len = pos - *json_pos + 1;
	str = calloc(1, len + 1);
	if (!str)
		return -1;
	memcpy(str, *json_pos, len);
	str[len] = '\0';

	*ret_val = atoi(str);
	free(str);
	*json_pos = pos;
	return 0;
}


static int json_check_tree_state(struct json_token *token)
{
	if (!token)
		return 0;
	if (json_check_tree_state(token->child) < 0 ||
	    json_check_tree_state(token->sibling) < 0)
		return -1;
	if (token->state != JSON_COMPLETED) {
		fprintf(stderr, "JSON: Unexpected token state %d (name=%s type=%d)\n",
			token->state, token->name ? token->name : "N/A", token->type);
		return -1;
	}
	return 0;
}


static struct json_token *json_alloc_token(unsigned int *tokens)
{
	(*tokens)++;
	if (*tokens > JSON_MAX_TOKENS) {
		fprintf(stderr, "JSON: Maximum token limit exceeded\n");
		return NULL;
	}
	return calloc(1, sizeof(struct json_token));
}


struct json_token *json_parse(const char *data, size_t data_len)
{
	struct json_token *root = NULL, *curr_token = NULL, *token = NULL;
	const char *pos, *end;
	char *str;
	int num;
	unsigned int depth = 0;
	unsigned int tokens = 0;

	pos = data;
	end = data + data_len;

	for (; pos < end; pos++) {
		switch (*pos) {
		case '[': /* start array */
		case '{': /* start object */
			if (!curr_token) {
				token = json_alloc_token(&tokens);
				if (!token)
					goto fail;
				if (!root)
					root = token;
			} else if (curr_token->state == JSON_WAITING_VALUE) {
				token = curr_token;
			} else if (curr_token->parent &&
				   curr_token->parent->type == JSON_ARRAY &&
				   curr_token->parent->state == JSON_STARTED &&
				   curr_token->state == JSON_EMPTY) {
				token = curr_token;
			} else {
				fprintf(stderr, "JSON: Invalid state for start array/object\n");
				goto fail;
			}
			depth++;
			if (depth > JSON_MAX_DEPTH) {
				fprintf(stderr, "JSON: Max depth exceeded\n");
				goto fail;
			}
			token->type = *pos == '[' ? JSON_ARRAY : JSON_OBJECT;
			token->state = JSON_STARTED;
			token->child = json_alloc_token(&tokens);
			if (!token->child)
				goto fail;
			curr_token = token->child;
			curr_token->parent = token;
			curr_token->state = JSON_EMPTY;
			break;
		case ']': /* end array */
		case '}': /* end object */
			if (!curr_token || !curr_token->parent ||
			    curr_token->parent->state != JSON_STARTED) {
				fprintf(stderr, "JSON: Invalid state for end array/object\n");
				goto fail;
			}
			depth--;
			curr_token = curr_token->parent;
			if ((*pos == ']' &&
			     curr_token->type != JSON_ARRAY) ||
			    (*pos == '}' &&
			     curr_token->type != JSON_OBJECT)) {
				fprintf(stderr, "JSON: Array/Object mismatch\n");
				goto fail;
			}
			if (curr_token->child->state == JSON_EMPTY &&
			    !curr_token->child->child &&
			    !curr_token->child->sibling) {
				/* Remove pending child token since the
				 * array/object was empty. */
				json_free(curr_token->child);
				curr_token->child = NULL;
			}
			curr_token->state = JSON_COMPLETED;
			break;
		case '\"': /* string */
			str = json_parse_string(&pos, end);
			if (!str)
				goto fail;
			if (!curr_token) {
				token = json_alloc_token(&tokens);
				if (!token) {
					free(str);
					goto fail;
				}
				token->type = JSON_STRING;
				token->string = str;
				token->state = JSON_COMPLETED;
			} else if (curr_token->parent &&
				   curr_token->parent->type == JSON_ARRAY &&
				   curr_token->parent->state == JSON_STARTED &&
				   curr_token->state == JSON_EMPTY) {
				curr_token->string = str;
				curr_token->state = JSON_COMPLETED;
				curr_token->type = JSON_STRING;
				fprintf(stderr, "JSON: String value: '%s'\n",
					curr_token->string);
			} else if (curr_token->state == JSON_EMPTY) {
				curr_token->type = JSON_VALUE;
				curr_token->name = str;
				curr_token->state = JSON_STARTED;
			} else if (curr_token->state == JSON_WAITING_VALUE) {
				curr_token->string = str;
				curr_token->state = JSON_COMPLETED;
				curr_token->type = JSON_STRING;
				fprintf(stderr, "JSON: String value: '%s' = '%s'\n",
					curr_token->name, curr_token->string);
			} else {
				fprintf(stderr, "JSON: Invalid state for a string\n");
				free(str);
				goto fail;
			}
			break;
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			/* ignore whitespace */
			break;
		case ':': /* name/value separator */
			if (!curr_token || curr_token->state != JSON_STARTED)
				goto fail;
			curr_token->state = JSON_WAITING_VALUE;
			break;
		case ',': /* member separator */
			if (!curr_token)
				goto fail;
			curr_token->sibling = json_alloc_token(&tokens);
			if (!curr_token->sibling)
				goto fail;
			curr_token->sibling->parent = curr_token->parent;
			curr_token = curr_token->sibling;
			curr_token->state = JSON_EMPTY;
			break;
		case 't': /* true */
		case 'f': /* false */
		case 'n': /* null */
			if (!((end - pos >= 4 &&
			       strncmp(pos, "true", 4) == 0) ||
			      (end - pos >= 5 &&
			       strncmp(pos, "false", 5) == 0) ||
			      (end - pos >= 4 &&
			       strncmp(pos, "null", 4) == 0))) {
				fprintf(stderr, "JSON: Invalid literal name\n");
				goto fail;
			}
			if (!curr_token) {
				token = json_alloc_token(&tokens);
				if (!token)
					goto fail;
				curr_token = token;
			} else if (curr_token->state == JSON_WAITING_VALUE) {
				fprintf(stderr, "JSON: Literal name: '%s' = %c\n",
					curr_token->name, *pos);
			} else if (curr_token->parent &&
				   curr_token->parent->type == JSON_ARRAY &&
				   curr_token->parent->state == JSON_STARTED &&
				   curr_token->state == JSON_EMPTY) {
				fprintf(stderr, "JSON: Literal name: %c\n", *pos);
			} else {
				fprintf(stderr, "JSON: Invalid state for a literal name\n");
				goto fail;
			}
			switch (*pos) {
			case 't':
				curr_token->type = JSON_BOOLEAN;
				curr_token->number = 1;
				pos += 3;
				break;
			case 'f':
				curr_token->type = JSON_BOOLEAN;
				curr_token->number = 0;
				pos += 4;
				break;
			case 'n':
				curr_token->type = JSON_NULL;
				pos += 3;
				break;
			}
			curr_token->state = JSON_COMPLETED;
			break;
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			/* number */
			if (json_parse_number(&pos, end, &num) < 0)
				goto fail;
			if (!curr_token) {
				token = json_alloc_token(&tokens);
				if (!token)
					goto fail;
				token->type = JSON_NUMBER;
				token->number = num;
				token->state = JSON_COMPLETED;
			} else if (curr_token->state == JSON_WAITING_VALUE) {
				curr_token->number = num;
				curr_token->state = JSON_COMPLETED;
				curr_token->type = JSON_NUMBER;
				fprintf(stderr,
					   "JSON: Number value: '%s' = '%d'\n",
					   curr_token->name,
					   curr_token->number);
			} else if (curr_token->parent &&
				   curr_token->parent->type == JSON_ARRAY &&
				   curr_token->parent->state == JSON_STARTED &&
				   curr_token->state == JSON_EMPTY) {
				curr_token->number = num;
				curr_token->state = JSON_COMPLETED;
				curr_token->type = JSON_NUMBER;
				fprintf(stderr, "JSON: Number value: %d\n",
					   curr_token->number);
			} else {
				fprintf(stderr, "JSON: Invalid state for a number\n");
				goto fail;
			}
			break;
		case '\0':
			fprintf(stderr, "JSON: End of string\n");
			break;

		default:
			fprintf(stderr, "JSON: Unexpected JSON character: %c\n", *pos);
			goto fail;
		}

		if (!root)
			root = token;
		if (!curr_token)
			curr_token = token;
	}

	if (json_check_tree_state(root) < 0) {
		fprintf(stderr, "JSON: Incomplete token in the tree\n");
		goto fail;
	}

	return root;
fail:
	fprintf(stderr, "JSON: Parsing failed\n");
	json_free(root);
	return NULL;
}

struct json_token *json_get_member(struct json_token *json, const char *name)
{
	struct json_token *token, *ret = NULL;

	if (!json || json->type != JSON_OBJECT)
		return NULL;
	/* Return last matching entry */
	for (token = json->child; token; token = token->sibling) {
		if (token->name && strcmp(token->name, name) == 0)
			ret = token;
	}
	return ret;
}

void json_add_int(uint8_t *json, const char *name, int val)
{
	sprintf((char *)(json + strlen((char *)json)), "\"%s\":%d", name, val); /* Flawfinder: ignore */
}

void json_add_string(uint8_t *json, const char *name, const char *val)
{
	sprintf((char *)(json + strlen((char *)json)), "\"%s\":\"%s\"", name, val); /* Flawfinder: ignore */
}

int json_add_string_escape(uint8_t *json, const char *name,
			   const void *val, size_t len)
{
	char *tmp;
	size_t tmp_len = 6 * len + 1;

	tmp = calloc(1, tmp_len);
	if (!tmp)
		return -1;

	json_escape_string(tmp, tmp_len, val, len);
	json_add_string(json + strlen((char *)json), name, tmp);
	memset(tmp, 0, tmp_len);
	free(tmp);

	return 0;
}

void json_start_object(uint8_t *json, const char *name)
{
	if (name)
		sprintf((char *)(json + strlen((char *)json)), "\"%s\":", name); /* Flawfinder: ignore */

	sprintf((char *)(json + strlen((char *)json)), "%c", '{');
}

void json_end_object(uint8_t *json)
{
	sprintf((char *)(json + strlen((char *)json)), "%c", '}');
}

void json_start_array(uint8_t *json, const char *name)
{
	if (name)
		sprintf((char *)(json + strlen((char *)json)), "\"%s\":", name); /* Flawfinder: ignore */

	sprintf((char *)(json + strlen((char *)json)), "%c", '[');
}

void json_end_array(uint8_t *json)
{
	sprintf((char *)(json + strlen((char *)json)), "%c", ']');
}

void json_value_sep(uint8_t *json)
{
	sprintf((char *)(json + strlen((char *)json)), "%c", ',');
}

int json_add_base64url(uint8_t *json, const char *name, const void *val,
		       size_t len)
{
	char *b64;
	size_t blen = 0;

	b64 = calloc(1, ((len +1) / 3) * 4 + 64);
	if (!b64)
		return -1;

	blen = base64url_encode((unsigned char *)b64, (unsigned char *)val, len);

	json_add_string(json + strlen((char *)json), name, b64);
	free(b64);
	return blen;
}

uint8_t *json_get_member_base64(struct json_token *json,
				       const char *name, size_t *buflen)
{
	struct json_token *token;
	uint8_t ret;
	*buflen = 258;
	uint8_t *buf = calloc(1, 258);
	if (!buf)
		return NULL;

	token = json_get_member(json, name);
	if (!token || token->type != JSON_STRING)
		goto fail;

	ret = base64_decode((const unsigned char *)token->string, strlen(token->string), buf, buflen);
	if (ret)
		goto fail;
	return buf;
fail:
	free(buf);
	return NULL;
}
