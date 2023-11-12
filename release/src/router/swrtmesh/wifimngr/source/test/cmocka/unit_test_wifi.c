#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include <libubox/blobmsg.h>

#include <json-validator.h>
#include <json-c/json.h>
#include <json-editor.h>

#include <json-c/json_tokener.h>
#include <easy/easy.h>
#include <wifi.h>

#include <uci.h>

#include "wifimngr.h"
#include "test_utils.h"

struct test_ctx {
	struct blob_buf bb;
	struct ubus_object radio;
	struct ubus_object ap;
	FILE *fp;
};

/* declare wifimngr functions */
int wl_radio_status(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);


int wl_radio_get_param(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int sta_disconnect(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int nbr_transition(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int nbr_request(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int assoc_control(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int nbr_add(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int wps_start(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int wps_stop(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int vsie_add(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *ureq, const char *method,
		struct blob_attr *msg);

int vsie_del(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *ureq, const char *method,
		struct blob_attr *msg);

int nbr_del(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int sta_monitor_add(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int wl_scan(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int wl_scan_ex(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int wl_autochannel(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int wps_set_ap_pin(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int subscribe_frame(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int unsubscribe_frame(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int wl_add_iface(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int wl_del_iface(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int sta_monitor_add(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int sta_monitor_del(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg);

int wifimngr_event_main(void *ctx, const char *evmap_file);

/* overload ubus_send_reply to prevent segfault*/
int ubus_send_reply(struct ubus_context *ctx, struct ubus_request_data *req,
		    struct blob_attr *msg)
{
	return 0;
}

static void test_api_radio_disconnect(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->ap;
	struct json_object *jobj, *tmp;
	const char *prefix = "disconnect";

	blobmsg_add_string(bb, "sta", FIVE_CLIENT);

	sta_disconnect(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "sta");
	assert_string_equal(json_object_get_string(tmp), FIVE_CLIENT);

	json_object_put(jobj);
	return;
}

static void test_api_req_bss_transition(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->ap;
	struct json_object *jobj, *tmp;
	const char *prefix = "req_bss_transition";
	void *arr;

	blobmsg_add_string(bb, "client", FIVE_CLIENT);
	arr = blobmsg_open_array(bb, "bssid");
	blobmsg_add_string(bb, "NULL", "22:22:22:22:22:22");
	blobmsg_add_string(bb, "NULL", "11:11:11:11:11:11");
	blobmsg_close_array(bb, arr);

	nbr_transition(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "client");
	assert_string_equal(json_object_get_string(tmp), FIVE_CLIENT);

	tmp = json_object_get_by_string(jobj, "bssid");
	assert_string_equal(json_object_get_string(tmp), "[ \"22:22:22:22:22:22\", \"11:11:11:11:11:11\" ]");

	json_object_put(jobj);
	return;
}

static void test_api_req_beacon_report(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->ap;
	struct json_object *jobj, *tmp;
	const char *prefix = "req_beacon_report";

	blobmsg_add_string(bb, "client", FIVE_CLIENT);
	//blobmsg_add_string(bb, "ssid", "Test SSID 5Ghz");

	nbr_request(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	/* TODO: not implemented */
	//tmp = json_object_get_by_string(jobj, "ssid");
	//assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "client");
	assert_string_equal(json_object_get_string(tmp), FIVE_CLIENT);

	json_object_put(jobj);
	return;
}

static void test_api_add_nbr(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->ap;
	struct json_object *jobj, *tmp;
	const char *prefix = "add_neighbor";

	blobmsg_add_string(bb, "bssid", "00:11:12:13:14:15");
	blobmsg_add_u32(bb, "channel", 36);
	blobmsg_add_string(bb, "bssid_info", "5");
	blobmsg_add_u32(bb, "reg", 5);
	blobmsg_add_u32(bb, "phy", 5);

	nbr_add(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "bssid");
	assert_string_equal(json_object_get_string(tmp), "00:11:12:13:14:15");

	tmp = json_object_get_by_string(jobj, "channel");
	assert_int_equal(json_object_get_int(tmp), 36);

	tmp = json_object_get_by_string(jobj, "bssid_info");
	assert_int_equal(json_object_get_int(tmp), 5);

	tmp = json_object_get_by_string(jobj, "reg");
	assert_int_equal(json_object_get_int(tmp), 5);

	tmp = json_object_get_by_string(jobj, "phy");
	assert_int_equal(json_object_get_int(tmp), 5);

	json_object_put(jobj);
	return;
}

static void test_api_wps_start(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->ap;
	struct json_object *jobj, *tmp;
	const char *prefix = "start_wps";

	/* test pin & registrar */
	blobmsg_add_string(bb, "ifname", "test5");
	blobmsg_add_string(bb, "mode", "pin");
	blobmsg_add_string(bb, "role", "registrar");
	blobmsg_add_string(bb, "pin", PIN);
	wps_start(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "role");
	assert_int_equal(json_object_get_int(tmp), WPS_REGISTRAR);

	tmp = json_object_get_by_string(jobj, "mode");
	assert_int_equal(json_object_get_int(tmp), WPS_METHOD_PIN);

	tmp = json_object_get_by_string(jobj, "pin");
	assert_int_equal(json_object_get_int64(tmp), strtoul(PIN, NULL, 10));

	json_object_put(jobj);

	/* test enrollee and iface test2 */
	blob_buf_init(bb, 0);
	blobmsg_add_string(bb, "ifname", "test2");
	blobmsg_add_string(bb, "role", "enrollee");
	wps_start(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), TWO_IFACE);

	tmp = json_object_get_by_string(jobj, "role");
	assert_int_equal(json_object_get_int(tmp), WPS_ENROLLEE);

	/* test pbc and iface test2 */
	blob_buf_init(bb, 0);
	blobmsg_add_string(bb, "ifname", "test2");
	blobmsg_add_string(bb, "mode", "pbc");
	wps_start(NULL, obj, NULL, NULL, bb->head);

	json_object_put(jobj);

	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), TWO_IFACE);

	tmp = json_object_get_by_string(jobj, "mode");
	assert_int_equal(json_object_get_int(tmp), WPS_METHOD_PBC);

	json_object_put(jobj);

	return;
}

/* TODO: this test works "by chance" because we are looking for 5ghz,
 * how can we fix this? */
static void test_api_wps_stop(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->ap;
	struct json_object *jobj, *tmp;
	const char *prefix = "stop_wps";

	wps_stop(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	json_object_put(jobj);
	return;
}

static void test_api_add_vendor_ie(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->ap;
	struct json_object *jobj, *tmp;
	const char *prefix = "add_vendor_ie";
	const char *oui = "112233";
	const char *data = "efa567";

	blobmsg_add_u32(bb, "mgmt", 5);
	blobmsg_add_string(bb, "oui", oui);
	blobmsg_add_string(bb, "data", data);
	vsie_add(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "oui");
	assert_string_equal(json_object_get_string(tmp), oui);

	tmp = json_object_get_by_string(jobj, "data");
	/* hex data, prepend dd+len in hex fmt */
	assert_string_equal(json_object_get_string(tmp), data);

	json_object_put(jobj);
	return;
}

static void test_api_del_vendor_ie(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->ap;
	struct json_object *jobj, *tmp;
	const char *prefix = "del_vendor_ie";
	const char *oui = "112233";
	const char *data = "efa567";

	blobmsg_add_u32(bb, "mgmt", 5);
	blobmsg_add_string(bb, "oui", oui);
	blobmsg_add_string(bb, "data", data);
	vsie_del(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "oui");
	assert_string_equal(json_object_get_string(tmp), oui);

	tmp = json_object_get_by_string(jobj, "data");
	assert_string_equal(json_object_get_string(tmp), data); // hex data, prepend dd+len in hex fmt

	json_object_put(jobj);
	return;
}

static void test_api_del_neighbor(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->ap;
	struct json_object *jobj, *tmp;
	const char *prefix = "del_neighbor";

	blobmsg_add_string(bb, "bssid", "50:10:00:11:22:33");
	nbr_del(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "bssid");
	assert_string_equal(json_object_get_string(tmp), "50:10:00:11:22:33");

	json_object_put(jobj);
	return;
}

static void test_api_scan(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->radio;
	struct json_object *jobj, *tmp;
	const char *prefix = "scan";

	blobmsg_add_string(bb, "bssid", "00:00:00:11:22:33");
	blobmsg_add_string(bb, "ssid", "test");
	blobmsg_add_u32(bb, "channel", 36);
	wl_scan(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	/* TODO: unused?
	tmp = json_object_get_by_string(jobj, "bssid");
	assert_string_equal(json_object_get_string(tmp), "00:00:00:11:22:33");
	*/

	tmp = json_object_get_by_string(jobj, "ssid");
	assert_string_equal(json_object_get_string(tmp), "test");

	/* TODO: unused?
	tmp = json_object_get_by_string(jobj, "channel");
	assert_int_equal(json_object_get_int(tmp), 36);
	*/

	json_object_put(jobj);
	return;
}

static void test_set_wps_ap_pin(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->ap;
	struct json_object *jobj, *tmp;
	const char *prefix = "set_wps_ap_pin";

	blobmsg_add_string(bb, "ifname", FIVE_IFACE);
	blobmsg_add_string(bb, "pin", PIN);
	wps_set_ap_pin(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "pin");
	assert_int_equal(json_object_get_int64(tmp), strtoul(PIN, NULL, 10));

	json_object_put(jobj);
	return;
}

static int group_setup(void **state)
{
	struct test_ctx *ctx = calloc(1, sizeof(struct test_ctx));

	if (!ctx)
		return -1;

	remove("/tmp/test.log");

	ctx->radio.name = "wifi.radio.test5";
	ctx->ap.name = "wifi.ap.test5";

	memset(&ctx->bb, 0, sizeof(struct blob_buf));

	*state = ctx;
	return 0;
}

static void test_api_acs(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->radio;
	struct json_object *jobj, *tmp;
	const char *prefix = "acs";

	blobmsg_add_string(bb, "ifname", FIVE_IFACE);
	wl_autochannel(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	json_object_put(jobj);
	return;
}

static void test_subscribe_frame(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->radio;
	struct json_object *jobj, *tmp;
	const char *prefix = "iface_subscribe_frame";

	blobmsg_add_u32(bb, "type", 0);
	blobmsg_add_u32(bb, "stype", 8);

	subscribe_frame(NULL, obj, NULL, NULL, bb->head);

	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "type");
	assert_int_equal(json_object_get_int(tmp), 0);

	tmp = json_object_get_by_string(jobj, "stype");
	assert_int_equal(json_object_get_int(tmp), 8);

	json_object_put(jobj);
	return;
}

static void test_unsubscribe_frame(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->radio;
	struct json_object *jobj, *tmp;
	const char *prefix = "iface_unsubscribe_frame";

	blobmsg_add_u32(bb, "type", 1);
	blobmsg_add_u32(bb, "stype", 4);
	unsubscribe_frame(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "type");
	assert_int_equal(json_object_get_int(tmp), 1);

	tmp = json_object_get_by_string(jobj, "stype");
	assert_int_equal(json_object_get_int(tmp), 4);

	json_object_put(jobj);
	return;
}

bool iface_uci_match(struct uci_context *uci_ctx, struct uci_section *s, char *name, char *mode, char *device)
{
	char *uci_name, *uci_mode, *uci_device;

	uci_name = uci_lookup_option_string(uci_ctx, s, "ifname");
	if (!uci_name || strncmp(uci_name, name, 128)) // arbitrary len
		return false;

	uci_mode = uci_lookup_option_string(uci_ctx, s, "mode");

	if (!uci_mode || strncmp(uci_mode, mode, 128)) // arbitrary len
		return false;

	uci_device = uci_lookup_option_string(uci_ctx, s, "device");
	if (!uci_device || strncmp(uci_device, device, 128)) // arbitrary len
		return false;

	return true;
}

static void test_add_iface(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->radio;
	struct json_object *jobj, *tmp, *elem, *mode, *name;
	struct uci_context *uci_ctx;
	struct uci_package *uci_wireless;
	struct uci_element *e;
	const char *prefix = "add_iface";
	void *table;
	bool found = false;
	int i;

	uci_ctx = uci_alloc_context();
	assert_non_null(uci_ctx);

	uci_wireless = init_package(&uci_ctx, "wireless");
	assert_non_null(uci_wireless);

	// verify no uci section exists prior
	uci_foreach_element(&uci_wireless->sections, e){
		struct uci_section *s = uci_to_section(e);

		found = iface_uci_match(uci_ctx, s, "test", "monitor", "test5");
	}
	assert_false(found);

	// perform tests adding iface and uci section
	blobmsg_add_u32(bb, "config", 1);
	table = blobmsg_open_table(bb, "args");
	blobmsg_add_string(bb, "mode", "monitor");
	blobmsg_add_string(bb, "ifname", "test");
	blobmsg_close_table(bb, table);
	wl_add_iface(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "wifi_mode");
	assert_int_equal(json_object_get_int(tmp), WIFI_MODE_MONITOR);

	tmp = json_object_get_by_string(jobj, "argv");
	assert_true(0 < json_object_array_length(tmp));
	for (i = 0; i < json_object_array_length(tmp); i++) {
		elem = json_object_array_get_idx(tmp, i);

		if (!strncmp(json_object_get_string(elem), "mode", 4))
			mode = json_object_array_get_idx(tmp, i+1);
		if (!strncmp(json_object_get_string(elem), "ifname", 4))
			name = json_object_array_get_idx(tmp, i+1);
	}
	assert_string_equal(json_object_get_string(mode), "monitor");

	uci_wireless = init_package(&uci_ctx, "wireless");
	assert_non_null(uci_wireless);

	// verify uci section exists post
	uci_foreach_element(&uci_wireless->sections, e){
		struct uci_section *s = uci_to_section(e);

		found = iface_uci_match(uci_ctx, s, "test", "monitor", "test5");
	}
	assert_true(found);

	json_object_put(jobj);
	uci_free_context(uci_ctx);
	return;
}

static void test_del_iface(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->radio;
	struct json_object *jobj, *tmp;
	struct uci_context *uci_ctx;
	struct uci_package *uci_wireless;
	struct uci_element *e;
	const char *prefix = "del_iface";
	bool found = false;

	uci_ctx = uci_alloc_context();
	assert_non_null(uci_ctx);

	uci_wireless = init_package(&uci_ctx, "wireless");
	assert_non_null(uci_wireless);

	// verify uci section exists prior
	uci_foreach_element(&uci_wireless->sections, e){
		struct uci_section *s = uci_to_section(e);

		found = iface_uci_match(uci_ctx, s, "test", "monitor", "test5");
	}
	assert_true(found);

	blobmsg_add_string(bb, "ifname", "test");
	blobmsg_add_u32(bb, "config", 1);
	wl_del_iface(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);

	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "iface");
	assert_string_equal(json_object_get_string(tmp), "test");

	uci_wireless = init_package(&uci_ctx, "wireless");
	assert_non_null(uci_wireless);

	// verify no uci section exists post
	uci_foreach_element(&uci_wireless->sections, e){
		struct uci_section *s = uci_to_section(e);

		found = iface_uci_match(uci_ctx, s, "test", "monitor", "test5");
	}
	assert_false(found);

	json_object_put(jobj);
	uci_free_context(uci_ctx);
	return;
}

static void test_add_monitor(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->radio;
	struct json_object *jobj, *tmp;
	const char *prefix = "monitor_sta";
	const char *sta = "11:22:33:44:55:66";

	blobmsg_add_string(bb, "sta", sta);
	sta_monitor_add(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);

	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "sta");
	assert_string_equal(json_object_get_string(tmp), sta);

	tmp = json_object_get_by_string(jobj, "enable");
	assert_true(json_object_get_boolean(tmp));

	json_object_put(jobj);
	return;
}

static void test_del_monitor(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct blob_buf *bb = &ctx->bb;
	struct ubus_object *obj = &ctx->radio;
	struct json_object *jobj, *tmp;
	const char *prefix = "monitor_sta";
	const char *sta = "11:22:33:44:55:66";

	blobmsg_add_string(bb, "sta", sta);
	sta_monitor_del(NULL, obj, NULL, NULL, bb->head);
	jobj = poll_test_log(ctx->fp, prefix);

	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "sta");
	assert_string_equal(json_object_get_string(tmp), sta);

	tmp = json_object_get_by_string(jobj, "enable");
	assert_false(json_object_get_boolean(tmp));

	json_object_put(jobj);
	return;
}

int wifimngr_recv_event(const char *ifname, void *handle)
{
	int err;

	for (;;) {
		err = wifi_recv_event((char *)ifname, handle);
		if (err < 0)
			return -1;
	}

	return 0;
}

static void test_api_events(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct json_object *jobj, *tmp;
	const char *prefix = "register_event";

	wifimngr_event_main(NULL, "/etc/wifi.json");

	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	tmp = json_object_get_by_string(jobj, "family");
	assert_string_equal(json_object_get_string(tmp), "easysoc");

	tmp = json_object_get_by_string(jobj, "group");
	assert_string_equal(json_object_get_string(tmp), "notify");

	json_object_put(jobj);

	prefix = "recv_event";

	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	json_object_put(jobj);
}

static int group_teardown(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;

	blob_buf_free(&ctx->bb);
	free(ctx);
	remove("/tmp/test.log");

	/* TODO: fix event poll file */
	//if (ctx->fp)
		//fclose(ctx->fp);
	return 0;
}

static int setup(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;

	blob_buf_init(&ctx->bb, 0);

	return 0;
}

static int iface_setup(void **state)
{
	int rv;

	setup(state);

	remove("/etc/config/wireless");
	rv = cp("/etc/config/wireless", "../files/etc/config/wireless");
	assert_int_not_equal(rv, -1);

	return 0;
}

static int del_iface_setup(void **state)
{
	iface_setup(state);

	// lazy, use another test to setup iface
	test_add_iface(state);

	return 0;
}

static int iface_teardown(void **state)
{
	int rv;

	remove("/etc/config/wireless");
	rv = cp("/etc/config/wireless", "../files/etc/config/wireless");
	assert_int_not_equal(rv, -1);

	return 0;
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_api_radio_disconnect, setup),
		cmocka_unit_test_setup(test_api_req_bss_transition, setup),
		cmocka_unit_test_setup(test_api_req_beacon_report, setup),
		cmocka_unit_test_setup(test_api_add_nbr, setup),
		cmocka_unit_test_setup(test_api_wps_start, setup),
		cmocka_unit_test_setup(test_api_wps_stop, setup),
		cmocka_unit_test_setup(test_api_add_vendor_ie, setup),
		cmocka_unit_test_setup(test_api_del_vendor_ie, setup),
		cmocka_unit_test_setup(test_api_del_neighbor, setup),
		cmocka_unit_test_setup(test_api_scan, setup),
		cmocka_unit_test_setup(test_set_wps_ap_pin, setup),
		cmocka_unit_test_setup(test_api_acs, setup),
		cmocka_unit_test_setup(test_subscribe_frame, setup),
		cmocka_unit_test_setup(test_unsubscribe_frame, setup),
		cmocka_unit_test_setup_teardown(test_add_iface, iface_setup, iface_teardown),
		cmocka_unit_test_setup_teardown(test_del_iface, del_iface_setup, iface_teardown),
		cmocka_unit_test_setup(test_add_monitor, setup),
		cmocka_unit_test_setup(test_del_monitor, setup),
	};

	return cmocka_run_group_tests(tests, group_setup, group_teardown);
}
