/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * Copyright 2019, ASUSTeK Inc.
 * Copyright 2023-2024, SWRTdev.
 * Copyright 2023-2024, paldier <paldier@hotmail.com>.
 * All Rights Reserved.
 *
 */
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if_packet.h>
#include <stdio.h>
//#include <linux/in.h>
#if !defined(RTCONFIG_MUSL_LIBC) && !defined(MUSL_LIBC)
#include <linux/if_ether.h>
#endif
#include <net/if.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <bcmnvram.h>
#include "networkmap.h"

#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <signal.h>
#include <asm/byteorder.h>
#include "iboxcom.h"
#include "../shared/shutils.h"
#ifdef RTCONFIG_NOTIFICATION_CENTER
#include <libnt.h>
#endif
#include <json.h>

ac_state *find_next_state(ac_state *state, unsigned char transChar)
{
	ac_trans *tmp = state->nextTrans;

	if(tmp){
		while(tmp->transChar != transChar){
			tmp = tmp->nextTrans;
			if(tmp == NULL)
				return NULL;
		}
	}
	return tmp->nextState;
}

unsigned char prefix_search(ac_state *sm, const char *text, unsigned char *baseID)
{
	int i = 0;
	size_t len = strlen(text);
	ac_state *state_tmp, *state_tmp2;
	match_rule *rule_tmp;
	ac_trans *trans_pt, *trans_pt2;

	SM_DEBUG("Prefix search\n");
	SM_DEBUG("text length = %d:%s\n", len, text);
	trans_pt = sm->nextTrans;
	if(trans_pt == NULL){
		SM_DEBUG("text[0] not match!! return 0\n");
		return 0;
	}
	while(trans_pt->transChar != *text){
		trans_pt = trans_pt->nextTrans;
		if(trans_pt == NULL){
			SM_DEBUG("text[0] not match!! return 0\n");
			return 0;
		}
	}
	state_tmp = trans_pt->nextState;
	if(state_tmp == NULL){
		SM_DEBUG("text[0] not match!! return 0\n");
		return 0;
	}
	if(len > 1){
		for(i = 1; i < len; ++i){
			SM_DEBUG("search text[%d]: %c\n", i, text[i]);
			trans_pt2 = state_tmp->nextTrans;
			if(trans_pt2 == NULL)
				break;
			while(trans_pt2->transChar != text[i]){
				trans_pt2 = trans_pt2->nextTrans;
				if(trans_pt2 == NULL)
					break;
			}
			state_tmp2 = trans_pt2->nextState;
			if(state_tmp2 == NULL)
				break;
			state_tmp = state_tmp2;
		}
		rule_tmp = state_tmp->matchRuleList;
		if(rule_tmp){
			*baseID = rule_tmp->baseID;
			return rule_tmp->ID;
		}
		SM_DEBUG("text[%d] not match!! return 0\n", i);
		return 0;
	}else{
		if(1 == len){
			rule_tmp = state_tmp->matchRuleList;
			if(rule_tmp){
				*baseID = rule_tmp->baseID;
				return rule_tmp->ID;
			}
			SM_DEBUG("end search not found pattern!! return 0\n");
		}
	}
	return 0;
}



unsigned int prefix_search_index(ac_state *sm, const char *text, unsigned char *baseID, unsigned char *typeID)
{
	int i = 0;
	size_t len = strlen(text);
	ac_state *state_tmp, *state_tmp2;
	match_rule *rule_tmp;
	ac_trans *trans_pt, *trans_pt2;

	SM_DEBUG("Prefix search index\n");
	SM_DEBUG("text length = %d:%s\n", len, text);
	trans_pt = sm->nextTrans;
	if(trans_pt == NULL){
		SM_DEBUG("text[0] not match!! return 0\n");
		return 0;
	}
	while(trans_pt->transChar != *text){
		trans_pt = trans_pt->nextTrans;
		if(trans_pt == NULL){
			SM_DEBUG("text[0] not match!! return 0\n");
			return 0;
		}
	}
	state_tmp = trans_pt->nextState;
	if(state_tmp == NULL){
		SM_DEBUG("text[0] not match!! return 0\n");
		return 0;
	}
	if(len > 1){
		for(i = 1; i < len; ++i){
			SM_DEBUG("search text[%d]: %c\n", i, text[i]);
			trans_pt2 = state_tmp->nextTrans;
			if(trans_pt2 == NULL)
				break;
			while(trans_pt2->transChar != text[i]){
				trans_pt2 = trans_pt2->nextTrans;
				if(trans_pt2 == NULL)
					break;
			}
			state_tmp2 = trans_pt2->nextState;
			if(state_tmp2 == NULL)
				break;
			state_tmp = state_tmp2;
		}
		rule_tmp = state_tmp->matchRuleList;
		if(rule_tmp){
			*baseID = rule_tmp->baseID;
			*typeID = rule_tmp->ID;
			return i + 1;
		}
		SM_DEBUG("text[%d] not match!! return 0\n", i);
		return 0;
	}else{
		if(1 == len){
			rule_tmp = state_tmp->matchRuleList;
			if(rule_tmp){
				*baseID = rule_tmp->baseID;
				*typeID = rule_tmp->ID;
				return 1;
			}
			SM_DEBUG("end search not found pattern!! return 0\n");
		}
	}
	return 0;
}

unsigned char full_search(ac_state *sm, const char *text, unsigned char *baseID)
{
	int i, j, k;
	size_t len = strlen(text);
	char *ptr;
	ac_state *state_tmp, *state_tmp2;
	match_rule *rule_tmp;
	ac_trans *trans_pt, *trans_pt2;

	SM_DEBUG("Full search\n");
	SM_DEBUG("text length = %d:%s\n", len, text);
	if(len > 1){
		for(i = 1, j = 0; i < len - 1; i++, j++){
			trans_pt = sm->nextTrans;
			if(trans_pt == NULL){
				SM_DEBUG("text[%d] not match!! search next\n", j);
				continue;
			}
			while(trans_pt && trans_pt->transChar != text[j]){
				trans_pt = trans_pt->nextTrans;
			}
			if(trans_pt == NULL){
				SM_DEBUG("text[%d] not match!! search next\n", j);
				continue;
			}
			state_tmp = trans_pt->nextState;
			if(state_tmp == NULL){
				SM_DEBUG("text[%d] not match!! search next\n", j);
				continue;
			}
			for(k = i; k < len; k++){
				SM_DEBUG("search text[%d]: %c\n", k, text[k]);
				trans_pt2 = state_tmp->nextTrans;
				if(trans_pt2 == NULL){
					rule_tmp = state_tmp->matchRuleList;
					if(rule_tmp){
						*baseID = rule_tmp->baseID;
						return rule_tmp->ID;
					}
					SM_DEBUG("text[%d] not match!! search initial state\n", i);
					break;
				}
				while(trans_pt2 && trans_pt2->transChar != text[k]){
					trans_pt2 = trans_pt2->nextTrans;
				}
				if(trans_pt2 == NULL){
					rule_tmp = state_tmp->matchRuleList;
					if(rule_tmp){
						*baseID = rule_tmp->baseID;
						return rule_tmp->ID;
					}
					SM_DEBUG("text[%d] not match!! search initial state\n", i);
					break;
				}
				state_tmp2 = trans_pt2->nextState;
				if(state_tmp2 == NULL){
					rule_tmp = state_tmp->matchRuleList;
					if(rule_tmp){
						*baseID = rule_tmp->baseID;
						return rule_tmp->ID;
					}
					SM_DEBUG("text[%d] not match!! search initial state\n", i);
					break;
				}
				if(k = len - 1){
					rule_tmp = state_tmp2->matchRuleList;
					if(rule_tmp){
						*baseID = rule_tmp->baseID;
						return rule_tmp->ID;
					}
				}
			}
		}
	}
	SM_DEBUG("end search not found pattern!! return 0\n");
	return 0;
}

ac_state *create_ac_state()
{
	return (ac_state *)calloc(sizeof(ac_state), 1);
}

void add_new_next_state(ac_state *curState, unsigned char pChar, ac_state *nextState)
{
	ac_trans *tmp;

	tmp = (ac_trans *)malloc(sizeof(ac_trans));
	tmp->nextTrans = curState->nextTrans;
	curState->nextTrans = tmp;
	tmp->transChar = pChar;
	tmp->nextState = nextState;
	nextState->prevState = curState;
}

void add_match_rule_to_state(ac_state *state, unsigned char type, unsigned char base)
{
	match_rule *tmp;

	tmp = (match_rule *)malloc(sizeof(match_rule));
	tmp->next = state->matchRuleList;
	tmp->ID = type;
	tmp->baseID = base;
	state->matchRuleList = tmp;
}

ac_state *construct_ac_trie(convType *type)
{
	ac_state *state_tmp, *state_tmp1, *state_tmp2;
	convType *conv_tmp;
	char *signature;
	size_t len;
	size_t len2;
	size_t count;

	state_tmp = create_ac_state();
	if(state_tmp && type->signature){
		for(conv_tmp = type; conv_tmp->signature; conv_tmp++){
			SM_DEBUG("##### %d %s\n", conv_tmp->type, conv_tmp->signature);
			signature = conv_tmp->signature;
			len = strlen(signature);
			if(len){
				len2 = len - 1;
				state_tmp1 = state_tmp;
				count = 0;
				while(1){
					state_tmp2 = find_next_state(state_tmp, signature[count]);
					if(!state_tmp2)
						break;
					if(len2 == count)
						add_match_rule_to_state(state_tmp2, conv_tmp->type, conv_tmp->base);
					++count;
					if(len == count){
						break;
					}
					state_tmp1 = state_tmp2;
					signature = conv_tmp->signature;
				}
				if(count < len){
					while(len != count){
						state_tmp2 = state_tmp1;
						state_tmp1 = create_ac_state();
						if(len2 == count)
							add_match_rule_to_state(state_tmp1, conv_tmp->type, conv_tmp->base);
						add_new_next_state(state_tmp1, conv_tmp->signature[count], state_tmp2);
						count++;
						state_tmp2->next = state_tmp->next;
						state_tmp->next = state_tmp2;
					}
				}
			}
		}
	}
	return state_tmp;
}

int isBaseType(int type)
{
	if(type == TYPE_LINUX_DEVICE || type == TYPE_WINDOWS || type == TYPE_ANDROID)
		return 1;
	return 0;
}

ac_state *construct_ac_file(convType *type, const char * parse_filename)
{
	int lock, data_array_len, i, ac_type = 0, ac_os_type = 0, j, k;
	size_t ac_keyword_len;
	char ac_keyword[128] = {0};
	ac_state *state_tmp, *state_pt, *state_pt2, *ac_state;
	ac_trans *trans_pt;
	match_rule *match_pt;
	struct json_object *data_array = NULL, *o_data_content, *o_keyword, *o_type, *o_os_type;
	state_tmp = create_ac_state();
	if(!state_tmp)
		return NULL;
	if(!check_if_file_exist(parse_filename)){
		SM_DEBUG("%s >> parse [%s], file not exist \n", __func__, parse_filename);
		free(state_tmp);
		return NULL;
	}
	state_pt = state_tmp;
	lock = file_lock(parse_filename);
	data_array = json_object_from_file(parse_filename);
	file_unlock(lock);
	if(data_array){
		data_array_len = json_object_array_length(data_array);
		SM_DEBUG("%s >> parse [%s], data_array_len = %d\n", __func__, parse_filename, data_array_len);
		if(data_array_len > 0){
			for(i = 0; i < data_array_len; i++){
				o_data_content = json_object_array_get_idx(data_array, i);
				json_object_object_get_ex(o_data_content, "keyword", &o_keyword);
				json_object_object_get_ex(o_data_content, "type", &o_type);
				json_object_object_get_ex(o_data_content, "os_type", &o_os_type);
				if(o_keyword)
					snprintf(ac_keyword, sizeof(ac_keyword), "%s", json_object_get_string(o_keyword));
				if(o_type)
					ac_type = json_object_get_int(o_type);
				if(o_os_type)
					ac_os_type = json_object_get_int(o_os_type);
				SM_DEBUG("keyword[%d] = %s, type = %d, os_type = %d \n", i, ac_keyword, ac_type, ac_os_type);
				ac_keyword_len = strlen(ac_keyword);
				if(ac_keyword_len){
					if(state_pt->nextTrans){
						j = 0;
						trans_pt = state_pt->nextTrans;
						while(trans_pt){
							while(trans_pt->transChar != ac_keyword[j]){
								trans_pt = trans_pt->nextTrans;
								if(trans_pt == NULL){
									if(j < ac_keyword_len){
										for(k = j; k < ac_keyword_len; k++){
											ac_state = create_ac_state();
											if(k == ac_keyword_len - 1){
												match_pt = (match_rule *)malloc(sizeof(match_rule));
												match_pt->ID = ac_type;
												match_pt->baseID = ac_os_type;
												match_pt->next = ac_state->matchRuleList;
												ac_state->matchRuleList = match_pt;
											}
											trans_pt = (ac_trans *)malloc(sizeof(ac_trans));
											trans_pt->transChar = ac_keyword[k];
											trans_pt->nextState = ac_state;
											trans_pt->nextTrans = state_pt2->nextTrans;
											state_pt2->nextTrans = trans_pt;
											ac_state->prevState = state_pt2;
											ac_state->next = state_pt->next;
											state_pt2 = ac_state;
											state_pt->next = ac_state;
										}
									}
									break;
								}
							}
							state_pt2 = trans_pt->nextState;
							if(state_pt2 == NULL){
								if(j < ac_keyword_len){
									for(k = j; k < ac_keyword_len; k++){
										ac_state = create_ac_state();
										if(k == ac_keyword_len - 1){
											match_pt = (match_rule *)malloc(sizeof(match_rule));
											match_pt->ID = ac_type;
											match_pt->baseID = ac_os_type;
											match_pt->next = ac_state->matchRuleList;
											ac_state->matchRuleList = match_pt;
										}
										trans_pt = (ac_trans *)malloc(sizeof(ac_trans));
										trans_pt->transChar = ac_keyword[k];
										trans_pt->nextState = ac_state;
										trans_pt->nextTrans = state_pt2->nextTrans;
										state_pt2->nextTrans = trans_pt;
										ac_state->prevState = state_pt2;
										ac_state->next = state_pt->next;
										state_pt2 = ac_state;
										state_pt->next = ac_state;
									}
								}
								break;
							}
							if(j >= ac_keyword_len){
								match_pt = (match_rule *)malloc(sizeof(match_rule));
								match_pt->ID = ac_type;
								match_pt->baseID = ac_os_type;
								state_pt2 = trans_pt->nextState;
								match_pt->next = state_pt2->matchRuleList;
								state_pt2->matchRuleList = match_pt;
								break;
							}
							trans_pt = state_pt2->nextTrans;
							if(trans_pt == NULL){
								for(k = j; k < ac_keyword_len; k++){
									ac_state = create_ac_state();
									if(k == ac_keyword_len - 1){
										match_pt = (match_rule *)malloc(sizeof(match_rule));
										match_pt->ID = ac_type;
										match_pt->baseID = ac_os_type;
										match_pt->next = ac_state->matchRuleList;
										ac_state->matchRuleList = match_pt;
									}
									trans_pt = (ac_trans *)malloc(sizeof(ac_trans));
									trans_pt->transChar = ac_keyword[k];
									trans_pt->nextState = ac_state;
									trans_pt->nextTrans = state_pt2->nextTrans;
									state_pt2->nextTrans = trans_pt;
									ac_state->prevState = state_pt2;
									ac_state->next = state_pt->next;
									state_pt2 = ac_state;
									state_pt->next = ac_state;
								}
								break;
							}
							j++;
						}
					}else{
						state_pt2 = state_pt;
						j = 0;
						for(k = j; k < ac_keyword_len; k++){
							ac_state = create_ac_state();
							if(k == ac_keyword_len - 1){
								match_pt = (match_rule *)malloc(sizeof(match_rule));
								match_pt->ID = ac_type;
								match_pt->baseID = ac_os_type;
								match_pt->next = ac_state->matchRuleList;
								ac_state->matchRuleList = match_pt;
							}
							trans_pt = (ac_trans *)malloc(sizeof(ac_trans));
							trans_pt->transChar = ac_keyword[k];
							trans_pt->nextState = ac_state;
							trans_pt->nextTrans = state_pt2->nextTrans;
							state_pt2->nextTrans = trans_pt;
							ac_state->prevState = state_pt2;
							ac_state->next = state_pt->next;
							state_pt2 = ac_state;
							state_pt->next = ac_state;
						}
					}
				}
			}
		}
		return state_tmp;
	}
	SM_DEBUG("%s >> parse [%s], data_array read error, %s \n", __func__, parse_filename, "null");
	free(state_tmp);
	return NULL;
}

int free_ac_state(ac_state *state)
{
	ac_state *state_tmp;
	ac_trans *trans_tmp;
	match_rule *rule_tmp;

	if(state){
		while(state){
			state_tmp = state;
			state = state->next;
			trans_tmp = state_tmp->nextTrans;
			if(trans_tmp){
				while(trans_tmp){
					state_tmp->nextTrans = trans_tmp->nextTrans;
					free(trans_tmp);
					trans_tmp = state_tmp->nextTrans;
				}
			}
			rule_tmp = state_tmp->matchRuleList;
			if(rule_tmp){
				while(rule_tmp){
					state_tmp->matchRuleList = rule_tmp->next;
					free(rule_tmp);
					rule_tmp = state_tmp->matchRuleList;
				}
			}
			free(state_tmp);
		}
	}
	SM_DEBUG("free state machine success!\n");
	return 0;
}

