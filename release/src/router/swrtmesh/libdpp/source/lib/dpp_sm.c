/*
 * dpp_sm.c - implements DPP state machine.
 *
 * Copyright (C) 2023 IOPSYS Software Solutions AB. All rights reserved.
 *
 * See LICENSE file for license related information.
 */


#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <easy/easy.h>
#include <wifidefs.h>

#include "debug.h"
#include "dpp_api.h"
#include "dpp_sm.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(s)	sizeof(s)/sizeof(s[0])
#endif

#ifndef UNUSED
#define UNUSED(x)	(void)(x)
#endif


#define D_HANDLER(s)	int handle_ ## s(void *, struct dpp_sm *, struct dpp_sm_event *)

D_HANDLER(DPP_INITIATOR_STATE_NOTHING);
D_HANDLER(DPP_INITIATOR_STATE_BOOTSTRAPPED);
D_HANDLER(DPP_INITIATOR_STATE_AUTHENTICATING);
D_HANDLER(DPP_INITIATOR_STATE_AUTHENTICATED);
D_HANDLER(DPP_INITIATOR_STATE_FINISHED);
D_HANDLER(DPP_INITIATOR_STATE_TERMINATED);

D_HANDLER(DPP_RESPONDER_STATE_BS_ACQUIRING);
D_HANDLER(DPP_RESPONDER_STATE_AWAITING);
D_HANDLER(DPP_RESPONDER_STATE_AUTHENTICATING);
D_HANDLER(DPP_RESPONDER_STATE_AUTHENTICATED);
D_HANDLER(DPP_RESPONDER_STATE_PROVISIONING);
D_HANDLER(DPP_RESPONDER_STATE_PROVISIONING);
D_HANDLER(DPP_RESPONDER_STATE_PROVISIONED);

#define HANDLER(s)	int handle_ ## s(void *ctx, struct dpp_sm *sm, struct dpp_sm_event *event)
#define H(s)		{ .state = s, .handler = handle_ ## s }


HANDLER(DPP_INITIATOR_STATE_NOTHING)
{
	struct dpp_sm *m = sm;

	pr_debug("%s:\n", __func__);
	/* handle only Bootstrapping Event here */
	if (event->type != DPP_EVENT_BOOTSTRAPPING)
		return -1;

	m->event = event->type;

	// Create the dpp_peer and wait for PAs
	// Or, if we received PA earlier and was enqueued, and now we received
	// the BS info for the same enrollee, proceeed to gen Auth-Req

	return 0;
}

HANDLER(DPP_INITIATOR_STATE_BOOTSTRAPPED)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_sm *m = sm;
	struct dpp_peer *peer = (struct dpp_peer *)m->data;
	int ret;

	pr_debug("%s:\n", __func__);
	/* handle only Trigger Event in this state */
	if (event->type != DPP_EVENT_TRIGGER)
		return 0;

	m->event = event->type;
	ret = dpp_build_auth_request(peer->ctx,
				     peer->peer_macaddr,
				     &peer->auth_request,
				     &peer->auth_request_len);
	if (!ret) {
		m->state = DPP_INITIATOR_STATE_AUTHENTICATING;
		pr_debug("=====> State %d\n", m->state);
		m->event = DPP_EVENT_TX_FRAME;
		if (c->cb) {
			ret = c->cb(ctx, peer->peer_macaddr,
				    m->event,
				    peer->auth_request,
				    peer->auth_request_len);
			if (!ret) {
				m->sent = 1;
				m->tmo = 10;
				timer_set(&sm->t, m->tmo * 1000);
			}
		}
	}

	return ret;
}

HANDLER(DPP_INITIATOR_STATE_AUTHENTICATING)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_sm *m = sm;
	struct dpp_peer *peer = (struct dpp_peer *) m->data;
	uint8_t *smac = peer->peer_macaddr;
	uint8_t *frame = NULL;
	size_t framelen = 0;
	int ret = -1;


	pr_debug("%s:\n", __func__);
	switch (event->type) {
	case DPP_EVENT_TIMEOUT:
		m->event = event->type;
		if (m->sent <= 5) {
			m->tmo = 10;
			m->sent++;
			timer_set(&sm->t, m->tmo * 1000);
		} else {
			m->sent = 0;
			m->state = DPP_INITIATOR_STATE_BOOTSTRAPPED;
			m->event = DPP_EVENT_NO_PEER;
			if (c->cb)
				c->cb(ctx, NULL, m->event, NULL, 0);
		}

		break;
	case DPP_EVENT_RX_FRAME: {
		m->event = event->type;
		framelen = event->len;
		frame = event->data;

		if (!framelen || !frame) {
			pr_debug("%s: Error: framelen = 0 or frame = NULL\n", __func__);
			return -1;
		}

		pr_debug("%s: event = %d, frame = %p, len = %zu\n",
			 __func__, event->type, frame, framelen);

		ret = dpp_process_auth_response(ctx, smac, frame, framelen);
		if (!ret) {
			ret = dpp_build_auth_confirm(peer->ctx,
						     peer->peer_macaddr,
						     &peer->auth_confirm,
						     &peer->auth_confirm_len);
			if (!ret) {
				m->sent = 1;
				m->state = DPP_INITIATOR_STATE_AUTHENTICATED;
				pr_debug("=====> State %d\n", m->state);
				m->event = DPP_EVENT_TX_FRAME;
				if (c->cb) {
					ret = c->cb(ctx,
						    peer->peer_macaddr,
						    m->event,
						    peer->auth_confirm,
						    peer->auth_confirm_len);
					if (!ret) {
						m->sent = 1;
						m->tmo = 10;
						timer_set(&sm->t, m->tmo * 1000);
					}
				}
			}
		} else {
			switch (peer->auth_status) {
			case DPP_STATUS_NOT_COMPATIBLE:
				m->sent = 1;
				m->state = DPP_INITIATOR_STATE_BOOTSTRAPPED;
				pr_debug("=====> State %d\n", m->state);
				m->event = DPP_EVENT_TX_FRAME;
				if (c->cb) {
					ret = c->cb(ctx,
						    peer->peer_macaddr,
						    m->event,
						    peer->auth_confirm,
						    peer->auth_confirm_len);
				}
				break;
			case DPP_STATUS_AUTH_FAILURE:
				m->sent = 0;
				m->state = DPP_INITIATOR_STATE_BOOTSTRAPPED;
				m->event = DPP_EVENT_BAD_AUTH;
				if (c->cb)
					c->cb(ctx, peer->peer_macaddr, m->event,
					      NULL, 0);
				break;
			default:
				pr_debug("%s: unhandled auth-status %d\n",
					__func__, peer->auth_status);
				break;
			}
		}
	}
		break;
	default:
		break;
	}

	return ret;
}

HANDLER(DPP_INITIATOR_STATE_AUTHENTICATED)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_sm *m = sm;
	struct dpp_peer *peer = (struct dpp_peer *) m->data;
	uint8_t *smac = peer->peer_macaddr;
	uint8_t *frame = NULL;
	size_t framelen = 0;
	int ret = -1;


	pr_debug("%s:\n", __func__);
	switch (event->type) {
	case DPP_EVENT_TIMEOUT:
		m->event = event->type;
		if (m->sent <= 5) {
			m->tmo = 10;
			m->sent++;
			timer_set(&sm->t, m->tmo * 1000);
		} else {
			m->sent = 0;
			m->state = DPP_INITIATOR_STATE_TERMINATED;
			m->event = DPP_EVENT_NO_PEER;
			if (c->cb)
				c->cb(ctx, NULL, m->event, NULL, 0);
		}

		break;
	case DPP_EVENT_RX_FRAME:
		{
			m->event = event->type;
			framelen = event->len;
			frame = event->data;
			if (!framelen || !frame) {
				pr_debug("%s: Error: framelen = 0 or frame = NULL\n", __func__);
				return -1;
			}

			pr_debug("%s: event = %d, frame = %p, len = %zu\n", __func__, event->type, frame, framelen);
			/* drop any frame other than config-request, or malformed
			 * config-request received. And, remain in this state.
			 */
			ret = dpp_process_config_request(ctx, smac, frame, framelen);
			if (!ret) {
				ret = dpp_build_config_response(peer->ctx,
								peer->peer_macaddr,
								&peer->config_response,
								&peer->config_response_len);
				if (!ret) {
					m->sent = 1;
					if (peer->conf_status == DPP_STATUS_OK)
						m->state = DPP_INITIATOR_STATE_FINISHED;
					else /* DPP_STATUS_CONFIGURE_FAILURE */
						m->state = DPP_INITIATOR_STATE_TERMINATED;

					pr_debug("=====> State %d\n", m->state);
					m->event = DPP_EVENT_TX_FRAME;
					if (c->cb) {
						ret = c->cb(ctx, peer->peer_macaddr,
							    m->event,
							    peer->config_response,
							    peer->config_response_len);
						if (!ret) {
							m->sent = 1;
							m->tmo = 10;
							timer_set(&sm->t, m->tmo * 1000);
						}
					}
				}
			}
		}
		break;
	default:
		break;
	}

	return ret;
}

HANDLER(DPP_INITIATOR_STATE_FINISHED)
{
	struct dpp_sm *m = sm;
	pr_debug("%s:\n", __func__);

	pr_debug("=====> State %d\n", m->state);
	return 0;
}

HANDLER(DPP_INITIATOR_STATE_TERMINATED)
{
	struct dpp_sm *m = sm;
	pr_debug("%s:\n", __func__);

	pr_debug("=====> State %d\n", m->state);
	return 0;
}

HANDLER(DPP_RESPONDER_STATE_BS_ACQUIRING)
{
	struct dpp_sm *m = sm;
	pr_debug("%s:\n", __func__);

	pr_debug("=====> State %d\n", m->state);
	return 0;
}

HANDLER(DPP_RESPONDER_STATE_AWAITING)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_sm *m = sm;
	struct dpp_peer *peer = (struct dpp_peer *) m->data;
	uint8_t *smac = peer->peer_macaddr;
	uint8_t *frame = NULL;
	size_t framelen = 0;
	int ret = 0;


	pr_debug("%s:\n", __func__);
	if (event->type != DPP_EVENT_RX_FRAME)
		return 0;

	m->event = event->type;
	framelen = event->len;
	frame = event->data;
	if (!framelen || !frame) {
		pr_debug("%s: Error: frame = NULL || framelen = 0\n", __func__);
		goto out;
	}

	pr_debug("%s: event = %d, frame = %p, len = %zu\n", __func__, m->event, frame, framelen);

	ret = dpp_process_auth_request(ctx, smac, frame, framelen);
	if (!ret) {
		ret = dpp_build_auth_response(peer->ctx,
					     peer->peer_macaddr,
					     &peer->auth_response,
					     &peer->auth_response_len);
		if (!ret) {
			m->sent = 1;
			m->state = DPP_RESPONDER_STATE_AUTHENTICATING;
			pr_debug("=====> State %d\n", m->state);
			m->event = DPP_EVENT_TX_FRAME;
			if (c->cb) {
				ret = c->cb(ctx, peer->peer_macaddr, m->event,
					    peer->auth_response,
					    peer->auth_response_len);
				if (!ret) {
					m->sent = 1;
					m->tmo = 10;
					timer_set(&sm->t, m->tmo * 1000);
				}
			}
		}
	} else {
		switch (peer->auth_status) {
		case DPP_STATUS_NOT_COMPATIBLE:
			m->sent = 1;
			m->state = DPP_RESPONDER_STATE_AWAITING;
			pr_debug("=====> State %d\n", m->state);
			m->event = DPP_EVENT_TX_FRAME;
			if (c->cb) {
				ret = c->cb(ctx,
					    peer->peer_macaddr,
					    m->event,
					    peer->auth_confirm,
					    peer->auth_confirm_len);
			}
			break;
		default:
			m->sent = 0;
			pr_debug("%s: unhandled auth-status %d\n",
				__func__, peer->auth_status);
			break;
		}
	}

out:
	return ret;
}

HANDLER(DPP_RESPONDER_STATE_AUTHENTICATING)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_sm *m = sm;
	struct dpp_peer *peer = (struct dpp_peer *) m->data;
	uint8_t *smac = peer->peer_macaddr;
	uint8_t *frame = NULL;
	size_t framelen = 0;
	int ret = 0;


	pr_debug("%s:\n", __func__);

	if (event->type != DPP_EVENT_RX_FRAME)
		return 0;

	switch (event->type) {
	case DPP_EVENT_TIMEOUT:
		m->event = event->type;
		if (m->sent <= 5) {
			m->tmo = 10;
			m->sent++;
			timer_set(&sm->t, m->tmo * 1000);
		} else {
			m->sent = 0;
			m->state = DPP_RESPONDER_STATE_AWAITING;
			m->event = DPP_EVENT_BAD_AUTH;
			if (c->cb)
				c->cb(ctx, smac, m->event, NULL, 0);
		}
		break;

	case DPP_EVENT_RX_FRAME:
		{
			m->event = event->type;
			frame = event->data;
			framelen = event->len;
			if (!framelen || !frame) {
				pr_debug("%s: Err: frame = NULL || framelen = 0\n", __func__);
				goto out;
			}

			pr_debug("%s: event = %d, frame = %p, len = %zu\n",
				 __func__, event->type, frame, framelen);

			ret = dpp_process_auth_confirm(ctx, smac, frame, framelen);
			if (!ret) {
				m->sent = 0;
				m->state = DPP_RESPONDER_STATE_AUTHENTICATED;
				m->event = DPP_EVENT_AUTHENTICATED;
				pr_debug("=====> State %d\n", m->state);
				if (c->cb) {
					ret = c->cb(ctx, peer->peer_macaddr, m->event, NULL, 0);
				}
			} else {
				switch (peer->auth_status) {
				case DPP_STATUS_NOT_COMPATIBLE:
				case DPP_STATUS_AUTH_FAILURE:
					m->sent = 0;
					m->state = DPP_RESPONDER_STATE_AWAITING;
					pr_debug("=====> State %d\n", m->state);
					break;
				default:
					m->sent = 0;
					m->state = DPP_RESPONDER_STATE_AWAITING;
					m->event = DPP_EVENT_BAD_AUTH;
					if (c->cb)
						c->cb(ctx, smac, m->event, NULL, 0);

					break;
				}
			}
		}
		break;

	default:
		break;
	}

out:
	return ret;
}

/* Here responder is enrollee */
HANDLER(DPP_RESPONDER_STATE_AUTHENTICATED)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_sm *m = sm;
	struct dpp_peer *peer = (struct dpp_peer *)m->data;
	int ret;

	pr_debug("%s:\n", __func__);
	/* Trigger Event starts the provisioning for enrollee */
	if (event->type != DPP_EVENT_TRIGGER)
		return 0;

	m->event = event->type;
	ret = dpp_build_config_request(peer->ctx,
				       peer->peer_macaddr,
				       &peer->config_request,
				       &peer->config_request_len);
	if (!ret) {
		m->state = DPP_RESPONDER_STATE_PROVISIONING;
		pr_debug("=====> State %d\n", m->state);
		m->event = DPP_EVENT_TX_FRAME;
		if (c->cb) {
			ret = c->cb(ctx, peer->peer_macaddr,
				    m->event,
				    peer->config_request,
				    peer->config_request_len);
			if (!ret) {
				m->sent = 1;
				m->tmo = 10;
				timer_set(&sm->t, m->tmo * 1000);
			}
		}
	}

	return ret;
}

HANDLER(DPP_RESPONDER_STATE_PROVISIONING)
{
	struct dpp_context *c = (struct dpp_context *)ctx;
	struct dpp_sm *m = sm;
	struct dpp_peer *peer = (struct dpp_peer *) m->data;
	uint8_t *smac = peer->peer_macaddr;
	uint8_t *frame = NULL;
	size_t framelen = 0;
	int ret = -1;


	pr_debug("%s:\n", __func__);
	switch (event->type) {
	case DPP_EVENT_TIMEOUT:
		m->event = event->type;
		if (m->sent <= 5) {
			m->tmo = 10;
			m->sent++;
			timer_set(&sm->t, m->tmo * 1000);
		} else {
			m->sent = 0;
			m->state = DPP_RESPONDER_STATE_TERMINATED;
			m->event = DPP_EVENT_NO_PEER;
			if (c->cb)
				c->cb(ctx, NULL, m->event, NULL, 0);
		}

		break;
	case DPP_EVENT_RX_FRAME: {
		m->event = event->type;
		framelen = event->len;
		frame = event->data;
		if (!framelen || !frame) {
			pr_debug("%s: Error: framelen = 0 or frame = NULL\n", __func__);
			return -1;
		}

		pr_debug("%s: event = %d, frame = %p, len = %zu\n", __func__, event->type, frame, framelen);
		//TODO: drop any frame other than config-response
		//and remain in this state.
		ret = dpp_process_config_response(ctx, smac, frame, framelen);
		if (!ret) {
			m->sent = 0;
			m->state = DPP_RESPONDER_STATE_PROVISIONED;
			pr_debug("=====> State %d\n", m->state);
			m->event = DPP_EVENT_PROVISIONED;
			if (c->cb)
				ret = c->cb(ctx, peer->peer_macaddr, m->event, NULL, 0);
		} else {
			m->sent = 0;
			m->state = DPP_RESPONDER_STATE_TERMINATED;
			m->event = DPP_EVENT_PROVISIONED_FAIL;
			if (c->cb)
				ret = c->cb(ctx, peer->peer_macaddr, m->event, NULL, 0);
		}
	}
	default:
		break;
	}

	return ret;
}

HANDLER(DPP_RESPONDER_STATE_PROVISIONED)
{
	pr_debug("%s:\n", __func__);

	return 0;
}

HANDLER(DPP_RESPONDER_STATE_TERMINATED)
{
	pr_debug("%s:\n", __func__);

	return 0;
}



/* state-machine handlers */
const static struct dpp_sm_handler smh[] = {
	H(DPP_INITIATOR_STATE_NOTHING),
	H(DPP_INITIATOR_STATE_BOOTSTRAPPED),
	H(DPP_INITIATOR_STATE_AUTHENTICATING),
	H(DPP_INITIATOR_STATE_AUTHENTICATED),
	H(DPP_INITIATOR_STATE_FINISHED),
	H(DPP_INITIATOR_STATE_TERMINATED),
	H(DPP_RESPONDER_STATE_AWAITING),
	H(DPP_RESPONDER_STATE_BS_ACQUIRING),
	H(DPP_RESPONDER_STATE_AUTHENTICATING),
	H(DPP_RESPONDER_STATE_AUTHENTICATED),
	H(DPP_RESPONDER_STATE_PROVISIONING),
	H(DPP_RESPONDER_STATE_PROVISIONED),
	H(DPP_RESPONDER_STATE_TERMINATED),
};

#define smh_array_size	ARRAY_SIZE(smh)


#define SM(sm, s, e, d, l)	H(s, e)(sm, d, l)

int dpp_sm_process(struct dpp_context *ctx, struct dpp_sm *sm, void *event)
{
	struct dpp_sm_event *e = (struct dpp_sm_event *)event;

	pr_debug("We are about to pass to handler ===>\n");
	pr_debug("\tstate: %d\n", sm->state);
	pr_debug("\tevent: %d\n", e->type);

	return sm->handlers[sm->state].handler(ctx, sm, e);
}

static void dpp_sm_timer_cb(atimer_t *t)
{
	struct dpp_sm *m = container_of(t, struct dpp_sm, t);
	struct dpp_peer *peer = (struct dpp_peer *)m->data;
	struct dpp_context *c = (struct dpp_context *)peer->ctx;

	UNUSED(m);

	c->cb(c, peer->peer_macaddr, DPP_EVENT_TIMEOUT, NULL, 0);
}

int dpp_sm_create(int initiator, void **m)
{
	struct dpp_sm *sm;

	*m = NULL;
	sm = calloc(1, sizeof(*sm));
	if (!sm)
		return -1;

	sm->initiator = initiator;
	sm->state = initiator ? DPP_INITIATOR_STATE_NOTHING :
				DPP_RESPONDER_STATE_AWAITING;
	sm->handlers = smh;
	sm->num_handlers = DPP_NUM_STATES;
	sm->data = NULL;

	timer_init(&sm->t, dpp_sm_timer_cb);

	*m = sm;
	return 0;
}

void dpp_sm_set_data(struct dpp_sm *sm, void *data)
{
	if (sm)
		sm->data = data;
}

void dpp_sm_set_state(struct dpp_sm *sm, int state)
{
	if (sm)
		sm->state = state;
}

int dpp_sm_default_ofn(void *ctx, struct dpp_sm *sm, struct dpp_sm_event *event)
{
	int ret = 0;

	switch (event->type) {
	case DPP_EVENT_NO_PEER:
		pr_debug("Event: No Peer\n");
		break;
	case DPP_EVENT_BAD_AUTH:
		pr_debug("Event: Bad Auth\n");
		break;
	case DPP_EVENT_TX_FRAME: {
		//bufprintf("Event: Tx-frame", event->data, event->len);
		break;
	}
	default:
		break;
	}

	return ret;
}

void dpp_sm_set_ofn(struct dpp_sm *sm, int (*ofn)(void *, struct dpp_sm *, struct dpp_sm_event *))
{
	if (sm)
		sm->ofn = ofn;
}


#if 0
int main()
{
	int ev[] = { DPP_EVENT_BOOTSTRAPPING, DPP_EVENT_TRIGGER, DPP_EVENT_RX_FRAME, DPP_EVENT_TIMEOUT };
	int next_state = DPP_INITIATOR_STATE_NOTHING;
	uint8_t dummy[] = "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa";
	struct dpp_sm *mc;
	int ret;


	ret = dpp_sm_create(1, &mc);
	if (ret) {
		pr_debug("failed to create sm\n");
		exit(0);
	}

	dpp_sm_set_ofn(mc, dpp_sm_default_ofn);

	for (int i = 0; i < ARRAY_SIZE(ev); i++) {
		pr_debug("Event: %d\n", ev[i]);
		ret = dpp_sm_process(mc, ev[i]);
		if (ret) {
			pr_debug("dpp-sm-process() error\n");
			break;
		}
	}

	return 0;
}
#endif
