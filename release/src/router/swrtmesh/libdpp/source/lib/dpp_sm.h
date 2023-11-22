/*
 * dpp_sm.h - defines DPP state-machine.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */

#ifndef DPP_SM_H
#define DPP_SM_H

struct dpp_sm;


struct dpp_sm_event {
	int type;
	size_t len;
	uint8_t *data;
};

struct dpp_sm_handler {
	int state;
	int event;
	int (*handler)(void *ctx, struct dpp_sm *sm, struct dpp_sm_event *event);
};

struct dpp_sm {
	int initiator;
	uint8_t state;
	uint8_t event;
	const struct dpp_sm_handler *handlers;
	size_t num_handlers;
	uint32_t sent;
	uint32_t tmo;		/* timer interval in secs */
	atimer_t t;
	void *data;
	int (*ofn)(void *ctx, struct dpp_sm *sm, struct dpp_sm_event *event);
};


int dpp_sm_create(int initiator, void **m);


int dpp_sm_process(struct dpp_context *ctx, struct dpp_sm *sm, void *event);

void dpp_sm_set_data(struct dpp_sm *sm, void *data);
void dpp_sm_set_state(struct dpp_sm *sm, int state);
int dpp_sm_default_ofn(void *ctx, struct dpp_sm *sm, struct dpp_sm_event *event);
void dpp_sm_set_ofn(struct dpp_sm *sm, int (*ofn)(void *, struct dpp_sm *, struct dpp_sm_event *));


#endif /* DPP_SM_H */
