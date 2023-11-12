#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <libubus.h>
#include <libubox/blobmsg_json.h>
#include <libubox/blobmsg.h>
#include <uci.h>

#include <json-validator.h>
#include <json-c/json.h>
#include <json-editor.h>

#include <json-c/json_tokener.h>
#include <easy/easy.h>
#include <wifi.h>

#include "wifimngr.h"
#include "test_utils.h"


struct test_ctx {
	FILE *fp;
};

int wifimngr_event_main(void *ctx, const char *evmap_file);


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

static void test_api_register_event(void **state)
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
}

// TODO: properly implement
static void test_api_recv_event(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;
	struct json_object *jobj, *tmp;
	const char *prefix = "recv_event";

	jobj = poll_test_log(ctx->fp, prefix);
	assert_non_null(jobj);

	tmp = json_object_get_by_string(jobj, "ifname");
	assert_string_equal(json_object_get_string(tmp), FIVE_IFACE);

	json_object_put(jobj);
}

static int group_setup(void **state)
{
	struct test_ctx *ctx = calloc(1, sizeof(struct test_ctx));

	if (!ctx)
		return -1;

	remove("/tmp/test.log");

	*state = ctx;
	return 0;
}

static int group_teardown(void **state)
{
	struct test_ctx *ctx = (struct test_ctx *) *state;

	wifimngr_event_unregister(FIVE_IFACE);

	free(ctx);
	remove("/tmp/test.log");

	/* TODO: fix event poll file */
	//if (ctx->fp)
		//fclose(ctx->fp);
	return 0;
}

static int setup(void **state)
{
	//struct test_ctx *ctx = (struct test_ctx *) *state;

	return 0;
}

int main(void)
{
	const struct CMUnitTest tests[] = {
		cmocka_unit_test_setup(test_api_register_event, setup),
		// TODO: implement a trigger somehow? netlink event?
		//cmocka_unit_test_setup(test_api_recv_event, setup),
	};


	return cmocka_run_group_tests(tests, group_setup, group_teardown);
}
