
#include <dpp_api.h>

int process_events(void *dpp,)
{
	switch (ev) {
	case RX_FRAME:
		//dpp_sm_process(dpp, DPP_EVENT_RX_FRAME, smac, frame, framelen);
		break;
	case TIMEOUT:
		//dpp_sm_process(dpp, DPP_EVENT_TIMEOUT, peer);	  //peer from timer
		break;
	case BOOTSTRAPPING:
		//dpp_sm_process(dpp, DPP_EVENT_BOOTSTRAPPING, peer);	  //peer from timer
		break;
	}

}

int on_event_rx_frame(void *dpp, uint8_t *src_macaddr, uint8_t *frame, size_t len)
{
	switch (frame-type) {
	default:
		dpp_sm_process(dpp, DPP_EVENT_RX_FRAME, src_macaddr, frame, len);
		break;
	}

	return 0;
}

int on_event_bootstrap(void *dpp, uint8_t *uri)
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

int dpp_handle_output_events(void *ctx, enum dpp_event e, ...)
{
	int ret = 0;
	va_list args;


	printf("%s:\n", __func__);
	va_start(args, e);

	switch (e) {
	case DPP_EVENT_TX_FRAME:
		uint8_t *frame;
		size_t framelen;
		uint8_t *smac;

		smac = (uint8_t *)va_arg(args, void *);
		if (!smac) {
			fprintf(stderr, "%s: Error: smac = NULL\n", __func__);
			ret = -1;
			break;
		}

		frame = (uint8_t *)va_arg(args, void *);
		if (!frame) {
			fprintf(stderr, "%s: Error: frame = NULL\n", __func__);
			ret = -1;
			break;
		}

		framelen = (size_t)va_arg(args, size_t);
		if (!framelen) {
			fprintf(stderr, "%s: Err: framelen = 0\n", __func__);
			ret = -1;
			break;
		}

		fprintf(stderr, "%s: DPP_EVENT_TX_FRAME: smac = " MACFMT", frame = %p, len = %zu\n",
			__func__, MAC2STR(smac), frame, framelen);

		/* take appropriate action for this frame */
		break;
	case TIMEOUT:
		/* take action as appropriate */
		break;
	case BOOTSTRAPPING:
		/* maybe notify user, and/or take action as appropriate */
		break;
	default:
		break;
	}

	va_end(args);
	return ret;
}

int main()
{
	void *dpp;
	int ret;

	ret = dpp_init(&dpp);
	if (ret)
		return -1;

	dpp_set_ofn(dpp, dpp_handle_output_events);

	//dpp_chirp

	// setup to receive dpp frames from peer, timer events etc.
}
