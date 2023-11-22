/*
 * json_helper.h - json utility header.
 *
 * See LICENSE file for license related information.
 */

#ifndef JSON_HELPER_H
#define JSON_HELPER_H

struct json_token {
	enum json_type {
		JSON_VALUE,
		JSON_OBJECT,
		JSON_ARRAY,
		JSON_STRING,
		JSON_NUMBER,
		JSON_BOOLEAN,
		JSON_NULL,
	} type;
	enum json_parsing_state {
		JSON_EMPTY,
		JSON_STARTED,
		JSON_WAITING_VALUE,
		JSON_COMPLETED,
	} state;
	char *name;
	char *string;
	int number;
	struct json_token *parent, *child, *sibling;
};

void json_escape_string(char *txt, size_t maxlen, const char *data, size_t len);

int json_add_string_escape(uint8_t *json, const char *name, const void *val, size_t len);
void json_add_string(uint8_t *json, const char *name, const char *val);
void json_add_int(uint8_t *json, const char *name, int val);

void json_free(struct json_token *json);
struct json_token * json_parse(const char *data, size_t data_len);

struct json_token *json_get_member(struct json_token *json, const char *name);

void json_start_object(uint8_t *json, const char *name);
void json_end_object(uint8_t *json);

void json_start_array(uint8_t *json, const char *name);
void json_end_array(uint8_t *json);

void json_value_sep(uint8_t *json);

int json_add_base64url(uint8_t *json, const char *name, const void *val,
		       size_t len);
uint8_t *json_get_member_base64(struct json_token *json,
				const char *name, size_t *buflen);
#endif
