/*
 * Copyright (c) 2019, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/delay.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pm_wakeup.h>
#include <linux/rwsem.h>
#include <linux/suspend.h>
#include <soc/qcom/ramdump.h>
#include <soc/qcom/subsystem_notif.h>
#include <soc/qcom/subsystem_restart.h>
#include <linux/platform_device.h>

/*
 * To call subsystem get & put for WCSS alone,
 * 1) insmod testssr.ko (test_id by default is set to 1)
 * 2) rmmod testssr
 * To call subsystem get & put for LPASS alone,
 * 1) insmod testssr.ko test_id=2
 * 2) rmmod testssr
 * To call subsystem get & put for both WCSS and LPASS,
 * 1) insmod testssr.ko test_id=3
 * 2) rmmod testssr
 */

static void *wcss_notif_handle;
static void *wcss_subsys_handle;
static void *adsp_notif_handle;
static void *adsp_subsys_handle;
static void *subsys_handle;

static struct notifier_block nb;
static struct notifier_block atomic_nb;

#define TEST_SSR_WCSS "q6v5-wcss"
#define TEST_SSR_LPASS "q6v6-adsp"

static int test_id = 1;
module_param(test_id, int, S_IRUGO | S_IWUSR | S_IWGRP);

static const char *action_to_string(enum subsys_notif_type action)
{
	switch (action) {

	case	SUBSYS_BEFORE_SHUTDOWN:
		return __stringify(SUBSYS_BEFORE_SHUTDOWN);

	case	SUBSYS_AFTER_SHUTDOWN:
		return __stringify(SUBSYS_AFTER_SHUTDOWN);

	case	SUBSYS_BEFORE_POWERUP:
		return __stringify(SUBSYS_BEFORE_POWERUP);

	case	SUBSYS_AFTER_POWERUP:
		return __stringify(SUBSYS_AFTER_POWERUP);

	case	SUBSYS_RAMDUMP_NOTIFICATION:
		return __stringify(SUBSYS_RAMDUMP_NOTIFICATION);

	case	SUBSYS_POWERUP_FAILURE:
		return __stringify(SUBSYS_POWERUP_FAILURE);

	case	SUBSYS_PROXY_VOTE:
		return __stringify(SUBSYS_PROXY_VOTE);

	case	SUBSYS_PROXY_UNVOTE:
		return __stringify(SUBSYS_PROXY_UNVOTE);

	case	SUBSYS_SOC_RESET:
		return __stringify(SUBSYS_SOC_RESET);

	case	SUBSYS_PREPARE_FOR_FATAL_SHUTDOWN:
		return __stringify(SUBSYS_PREPARE_FOR_FATAL_SHUTDOWN);

	default:
		return "unknown";
	}
}

static int tssr_notifier(struct notifier_block *nb, unsigned long action,
								void *data)
{
	struct notif_data *notif = (struct notif_data *)data;

	pr_emerg("%s:\tReceived [%s] notification from [%s]subsystem\n",
		THIS_MODULE->name, action_to_string(action), notif->pdev->name);

	return NOTIFY_DONE;
}

static void *test_subsys_notif_register(const char *subsys_name)
{
	int ret;

	subsys_handle = subsys_notif_register_notifier(subsys_name, &nb);
	if (IS_ERR_OR_NULL(subsys_handle)) {
		pr_emerg("Subsystem notif reg failed\n");
		return NULL;
	}

	ret = subsys_notif_register_atomic_notifier(subsys_handle, &atomic_nb);
	if (ret < 0) {
		pr_emerg("Subsystem atomic notif reg failed\n");

		ret = subsys_notif_unregister_notifier(subsys_handle, &nb);
		if (ret < 0)
			pr_emerg("Can't unregister subsys notifier\n");

		return NULL;
	}
	return subsys_handle;
}

static void *test_subsystem_get(const char *subsys_name)
{
	int ret;

	pr_info("%s: invoking subsystem_get for %s\n", __func__, subsys_name);
	subsys_handle = subsystem_get(subsys_name);
	if (IS_ERR_OR_NULL(subsys_handle)) {
		pr_emerg("Subsystem get failed\n");

		ret = subsys_notif_unregister_notifier(subsys_handle, &nb);
		if (ret < 0)
			pr_emerg("Can't unregister subsys notifier\n");

		ret = subsys_notif_unregister_atomic_notifier(subsys_handle,
								&atomic_nb);
		if (ret < 0)
			pr_emerg("Can't unregister subsys notifier\n");

		return NULL;
	}
	return subsys_handle;
}

static int __init testssr_init(void)
{
	nb.notifier_call = tssr_notifier;
	atomic_nb.notifier_call = tssr_notifier;

	switch (test_id) {
	case 1:
		wcss_notif_handle = test_subsys_notif_register(TEST_SSR_WCSS);
		if (!wcss_notif_handle)
			goto err;
		wcss_subsys_handle = test_subsystem_get(TEST_SSR_WCSS);
		if (!wcss_subsys_handle)
			goto err;
		break;
	case 2:
		adsp_notif_handle = test_subsys_notif_register(TEST_SSR_LPASS);
		if (!adsp_notif_handle)
			goto err;
		adsp_subsys_handle = test_subsystem_get(TEST_SSR_LPASS);
		if (!adsp_subsys_handle)
			goto err;
		break;
	case 3:
		wcss_notif_handle = test_subsys_notif_register(TEST_SSR_WCSS);
		if (!wcss_notif_handle)
			goto err;
		wcss_subsys_handle = test_subsystem_get(TEST_SSR_WCSS);
		if (!wcss_subsys_handle)
			goto err;
		adsp_notif_handle = test_subsys_notif_register(TEST_SSR_LPASS);
		if (!adsp_notif_handle)
			goto err;
		adsp_subsys_handle = test_subsystem_get(TEST_SSR_LPASS);
		if (!adsp_subsys_handle)
			goto err;
		break;
	default:
		pr_err("Enter a valid test case id\n");
	}

	return 0;
err:
	return -EIO;
}

static void wcss_test_exit(void)
{
	subsystem_put(wcss_subsys_handle);
	subsys_notif_unregister_notifier(wcss_notif_handle, &nb);
	subsys_notif_unregister_atomic_notifier(wcss_notif_handle,
			&atomic_nb);
}

static void adsp_test_exit(void)
{
	subsystem_put(adsp_subsys_handle);
	subsys_notif_unregister_notifier(adsp_notif_handle, &nb);
	subsys_notif_unregister_atomic_notifier(adsp_notif_handle,
			&atomic_nb);
}

static void __exit testssr_exit(void)
{
	switch (test_id) {
	case 1:
		wcss_test_exit();
		break;
	case 2:
		adsp_test_exit();
		break;
	case 3:
		wcss_test_exit();
		adsp_test_exit();
		break;
	default:
		break;
	}
}

module_init(testssr_init);
module_exit(testssr_exit);
MODULE_LICENSE("Dual BSD/GPL");
