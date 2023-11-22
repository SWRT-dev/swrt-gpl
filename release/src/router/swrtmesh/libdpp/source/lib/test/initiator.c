#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <easy/easy.h>
#include <wifidefs.h>

#include "util.h"
#include "timer.h"
#include "dpp_api.h"


int on_event_rx_frame(void *dpp, uint8_t *src_macaddr, int frametype, uint8_t *frame, size_t len)
{
	switch (frametype) {
	case DPP_PA_PRESENCE_ANNOUNCEMENT:
		dpp_process_presence_announcement(dpp, src_macaddr, frame, len);
		break;
	default:
		void *ev = dpp_sm_create_event(dpp, src_macaddr, DPP_EVENT_RX_FRAME, len, frame);

		if (ev)
			dpp_trigger(dpp, src_macaddr, ev);

		break;
	}

	return 0;
}

int on_event_bootstrap(void *dpp, const char *uri)
{
	struct dpp_bootstrap_info *new;
	int ret;

	new = calloc(1, sizeof(*new));
	if (!new)
		return -1;

	ret = dpp_build_bootstrap_info_from_uri(uri, new);
	if (ret) {
		goto fail;
	}

	ret = dpp_bootstrap_add(dpp, new);
	if (ret)
		goto fail;

	return 0;

fail:
	fprintf(stderr, "%s: Failed to register bootstrap info\n", __func__);
	dpp_bootstrap_free(new);
	return ret;
}

int dpp_handle_output_events(void *dpp, uint8_t *smac, enum dpp_event e, uint8_t *data, size_t len)
{
	int ret = 0;

	printf("%s:\n", __func__);
	switch (e) {
	case DPP_EVENT_TX_FRAME:
		{
			uint8_t *frame;
			size_t framelen;

			if (!smac) {
				fprintf(stderr, "%s: Error: smac = NULL\n", __func__);
				ret = -1;
				break;
			}

			frame = data;
			if (!frame) {
				fprintf(stderr, "%s: Error: frame = NULL\n", __func__);
				ret = -1;
				break;
			}

			framelen = len;
			if (!framelen) {
				fprintf(stderr, "%s: Err: framelen = 0\n", __func__);
				ret = -1;
				break;
			}

			fprintf(stderr, "%s: DPP_EVENT_TX_FRAME: smac = " MACFMT", frame = %p, len = %zu\n",
				__func__, MAC2STR(smac), frame, framelen);

			/* XXX: take appropriate action for this frame */
		}
		break;
	case DPP_EVENT_AUTHENTICATED:
		{
			if (!smac) {
				fprintf(stderr, "%s: Error: smac = NULL\n", __func__);
				ret = -1;
				break;
			}

			void *ev = dpp_sm_create_event(dpp, smac, DPP_EVENT_TRIGGER, 0, NULL);

			if (ev)
				ret = dpp_trigger(dpp, smac, ev);
		}
		break;
	case DPP_EVENT_TIMEOUT:
		/* take action as appropriate */
		break;
	case DPP_EVENT_BOOTSTRAPPING:
		/* maybe notify user, and/or take action as appropriate */
		break;
	default:
		break;
	}

	return ret;
}

int main()
{
	char *argv[] = {"-I", "-C", "-V", "2"};
	int argc = 4;
	void *dpp;
	int ret;

	ret = dpp_init(&dpp, argc, argv);
	if (ret)
		return -1;

	dpp_register_cb(dpp, dpp_handle_output_events);

	// setup to receive dpp frames from peer, timer events etc.
}
