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
 * Copyright 2023, SWRTdev.
 * Copyright 2023, paldier <paldier@hotmail.com>.
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

ac_state *find_next_state(ac_state *state, unsigned char transChar)
{
	ac_state *tmp = state;

	if(tmp){
		while(tmp->nextTrans != NULL && tmp->nextTrans->transChar != transChar){
			tmp = tmp->next;
			if(tmp == NULL)
				return NULL;
		}
		tmp = tmp->prevState;
	}
	return tmp;
}

unsigned int prefix_search_index(ac_state *sm, const char *text, unsigned char *baseID)
{
	int n = 0, count = 0;
	size_t len = strlen(text);
	char *ptr = (char *)text;
	ac_state *state_tmp, *state_tmp2;
	match_rule *rule_tmp;

	SM_DEBUG("Prefix search index\n");
	SM_DEBUG("text length = %d:%s\n", len, text);
	state_tmp = find_next_state(sm, *text);
	if(state_tmp == NULL){
		SM_DEBUG("text[0] not match!! return 0\n");
		return 0;
	}

	if(len > 1){
		for(count = 1; ; ++count){
			SM_DEBUG("search text[%d]: %c\n", count, *ptr);
			ptr++;
			state_tmp2 = find_next_state(state_tmp, (unsigned char)*ptr);
			if(state_tmp2 == NULL)
				break;
			n = count + 1;
			if(n == len){
				rule_tmp = state_tmp2->matchRuleList;
				if(rule_tmp){
					*baseID = rule_tmp->baseID;
					return count + 1;
				}
				SM_DEBUG("end search not found pattern!! return 0\n");
				return 0;
			}
			state_tmp = state_tmp2;
		}
		rule_tmp = state_tmp2->matchRuleList;
		if(rule_tmp){
			*baseID = rule_tmp->baseID;
			return count + 1;
		}
		SM_DEBUG("text[%d] not match!! return 0\n");
		return 0;
	}else{
		n = 1;
		count = 0;
		if(n == len){
			rule_tmp = state_tmp->matchRuleList;
			if(rule_tmp){
				*baseID = rule_tmp->baseID;
				return count + 1;
			}
			SM_DEBUG("end search not found pattern!! return 0\n");
		}
	}
	return 0;
}

unsigned char prefix_search(ac_state *sm, const char *text, unsigned char *baseID)
{
	int n = 0, count = 0;
	size_t len = strlen(text);
	char *ptr = (char *)text;
	ac_state *state_tmp, *state_tmp2;
	match_rule *rule_tmp;

	SM_DEBUG("Prefix search\n");
	SM_DEBUG("text length = %d:%s\n", len, text);
	state_tmp = find_next_state(sm, *text);
	if(state_tmp == NULL){
		SM_DEBUG("text[0] not match!! return 0\n");
		return 0;
	}
	if(len > 1){
		for(count = 1; ; ++count){
			SM_DEBUG("search text[%d]: %c\n", count, *ptr);
			ptr++;
			state_tmp2 = find_next_state(state_tmp, (unsigned char)*ptr);
			if(state_tmp2 == NULL)
				break;
			n = count + 1;
			if(n == len){
				rule_tmp = state_tmp2->matchRuleList;
				if(rule_tmp){
					*baseID = rule_tmp->baseID;
					return rule_tmp->ID;
				}
				SM_DEBUG("end search not found pattern!! return 0\n");
				return 0;
			}
			state_tmp = state_tmp2;
		}
		rule_tmp = state_tmp2->matchRuleList;
		if(rule_tmp){
			*baseID = rule_tmp->baseID;
			return rule_tmp->ID;
		}
		SM_DEBUG("text[%d] not match!! return 0\n");
		return 0;
	}else{
		n = 1;
		if(n == len){
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

unsigned char full_search(ac_state *sm, const char *text, unsigned char *baseID)
{
	int count = 0;
	size_t len = strlen(text), len2;
	char *ptr;
	ac_state *state_tmp, *state_tmp2;
	match_rule *rule_tmp;

	SM_DEBUG("Full search\n");
	SM_DEBUG("text length = %d:%s\n", len, text);
	if(len != 1){
		len2 = len - 1;
		ptr = (char *)text;
		while(1){
FULL_AGAIN:
			for(count = 0; ; count++, ptr++){
				state_tmp = find_next_state(sm, (unsigned char)*ptr);
				if(state_tmp)
					break;
				SM_DEBUG("text[%d] not match!! search next\n", count);
				return 0;
				if(count == len2)
					goto FULL_END;
			}
			if(len > count)
				break;
			if(count == len2)
				goto FULL_END;
		}
		ptr = (char *)text;
		for(; ; count++){
			SM_DEBUG("search text[%d]: %c\n", count, *ptr);
			ptr++;
			state_tmp2 = find_next_state(state_tmp, (unsigned char)*ptr);
			if(state_tmp2 == NULL)
				break;
			if(count == len2){
				rule_tmp = state_tmp2->matchRuleList;
				if(rule_tmp){
					*baseID = rule_tmp->baseID;
					return rule_tmp->ID;
				}
				goto FULL_END;;
			}
			if(len > count)
				state_tmp = state_tmp2;
		}
		rule_tmp = state_tmp->matchRuleList;
		if(rule_tmp){
			*baseID = rule_tmp->baseID;
			return rule_tmp->ID;
		}
		SM_DEBUG("text[%d] not match!! search initial state\n", count);
		if(count != len2)
			goto FULL_AGAIN;
	}
FULL_END:
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
	if(type == 22)
		return 1;
	return (type - 30) < 2;
}
