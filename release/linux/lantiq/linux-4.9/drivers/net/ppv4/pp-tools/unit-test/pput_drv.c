/*
 * pput_drv.c
 * Description: Packet Processor unit test infrastructure
 *
 * Author:
 * Ofir Bitton<ofir1.bitton@intel.com>
 *
 * Versions:
 * Ofir Bitton: 0.0.0.1 - initial version
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/debugfs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/kthread.h>
#include "pput.h"
#include "pp_common.h"
#include "pp_debugfs_common.h"

#define MOD_NAME "PP Unit Test"
#define MOD_VERSION "0.0.1"

static struct task_struct *thread_st;

const char * const component_str[] = {
	"PP Infra", "PP Session", "PP Port", "PP Qos", "PP Bm", "PP Parser"
};

/**
 * struct registered_test : registered test in test db
 */
struct registered_test {
	struct list_head link;
	struct pp_test_t test;
};

/**
 * struct pput_private : module private data
 */
struct pput_private {
	struct dentry *dentry;
	struct list_head tlist;
	bool   stop_test[PP_UT_NUM_COMPONENTS];
};

static int pput_add(struct pp_test_t *pp_test);
static void pput_remove(struct pp_test_t *pp_test);
static int pput_run(u32 component, u32 level, char *single_test_name);
static bool pput_should_run_test(struct pp_test_t *test,
				 u32 component,
				 u32 level);
static void pput_dump(u32 component, u32 level);
static struct registered_test *pput_lookup(struct pp_test_t *pp_test);

/**
 * Kernel module private structure
 */
static struct pput_private *priv;

/**
 * Decide if to run test or not
 *
 * @param test test structure to quary
 * @param component component to test
 * @param level level to test
 *
 * @return bool - true to run test
 */
static bool pput_should_run_test(struct pp_test_t *test,
				 u32 component, u32 level)
{
	/* Run test if level and component are correct */
	if (((test->level_bitmap & level) ||
	     (test->level_bitmap & PP_UT_LEVEL_MUST_RUN)) &&
	    ((component == PP_UT_NUM_COMPONENTS) ||
	     (test->component == component))) {
		/* No mandatory test failed */
		if (!priv->stop_test[test->component])
			return true;
		/* Mandatory test failed but this test must run */
		else if (test->level_bitmap & PP_UT_LEVEL_MUST_RUN)
			return true;
		/* Mandatory test failed discard all tests */
		else
			return false;
	} else {
		return false;
	}
}

struct thread_args {
	u32 component;
	u32 level;
	char *single_test_name;
};

static int pput_thread(void *args)
{
	struct registered_test *rtest;
	int result, n = 0, buf_sz = (8 * 1024);
	char *buf, *str, *tok, *single_test_name;
	u32 component, level;

	component = ((struct thread_args *)args)->component;
	level = ((struct thread_args *)args)->level;
	single_test_name = ((struct thread_args *)args)->single_test_name;

	buf = kzalloc(buf_sz, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	pr_buf(buf, buf_sz, n, "\n\n");
	pr_buf_cat(buf, buf_sz, n,
		   "+====================== Running Level %01d Tests =====================+\n",
	       level);
	pr_buf_cat(buf, buf_sz, n,
		   "|==================================================================|\n");
	pr_buf_cat(buf, buf_sz, n,
		   "|=== Comp ===|========== Test Name ===========|====== Result ======|\n");
	pr_buf_cat(buf, buf_sz, n,
		   "|============|================================|====================|\n");
	list_for_each_entry(rtest, &priv->tlist, link) {
		if (pput_should_run_test(&rtest->test, component, level) ||
		    (single_test_name &&
		     !strncmp(single_test_name,
			      rtest->test.test_name,
			      strlen(rtest->test.test_name)))) {
			pr_info("Running Test: %s\n", rtest->test.test_name);
			priv->stop_test[rtest->test.component] = false;
			result = rtest->test.fn(rtest->test.test_data);
			pr_buf_cat(buf, buf_sz, n,
				   "|%-12s|%-32s|        %s        |\n",
				   component_str[rtest->test.component],
				   rtest->test.test_name,
				   result ? "PASS" : "FAIL");
			/* If a must pass test just failed, */
			/* Do not run next tests            */
			if (!result && (rtest->test.level_bitmap &
							PP_UT_LEVEL_MUST_PASS))
				priv->stop_test[rtest->test.component] = true;
		}
	}
	pr_buf_cat(buf, buf_sz, n,
		   "+==================================================================+\n");

	/* seems that printk has a limited length so we print line by line */
	str = buf;
	while ((tok = strsep(&str, "\n")) != NULL)
		pr_info("%s\n", tok);
	kfree(buf);
	kfree(args);

	thread_st = NULL;
	do_exit(0);
	return 0;
}

/**
 * Run all test in certain level
 *
 * @param component Component to run
 * @param level Level to run
 *
 * @return zero for success
 */
static int pput_run(u32 component, u32 level, char *single_test_name)
{
	struct thread_args *args;

	/* Allow only 1 thread at a time */
	if (thread_st)
		kthread_stop(thread_st);

	args = kzalloc(sizeof(*args), GFP_KERNEL);
	args->component = component;
	args->level = level;
	args->single_test_name = single_test_name;

	thread_st = kthread_run(pput_thread, args, "pput_thread");

	return 0;
}

/**
 * Dump all test in DB
 *
 * @param component Component to dump
 * @param level Level to dump
 *
 * @return void
 */
static void pput_dump(u32 component, u32 level)
{
	struct registered_test *rtest;

	list_for_each_entry(rtest, &priv->tlist, link) {
		if (rtest->test.level_bitmap & level)
			if ((component == PP_UT_NUM_COMPONENTS) ||
				(rtest->test.component == component))
				pr_info("%s - %s\n",
					component_str[rtest->test.component],
					rtest->test.test_name);
	}
}

/**
 * Search test in test DB
 *
 * @param dev : Device for search
 *
 * @return struct registered_test* : registered test pointer if
 *         found, NULL if not exist
 */
static struct registered_test *pput_lookup(struct pp_test_t *pp_test)
{
	struct registered_test *rtest;

	if (pp_test) {
		list_for_each_entry(rtest, &priv->tlist, link) {
			if ((rtest->test.fn == pp_test->fn) &&
			    (rtest->test.test_data == pp_test->test_data))
				return rtest;
		}
	}

	return NULL;
}

/**
 * Remove test from test DB
 *
 * @param pp_test test to remove
 */
static void pput_remove(struct pp_test_t *pp_test)
{
	struct registered_test *rtest = pput_lookup(pp_test);

	if (unlikely(!rtest || list_empty(&priv->tlist))) {
		pr_err("test does not exist\n");
		return;
	}

	list_del(&rtest->link);
	kfree(rtest);
}

/**
 * Add test to test DB
 *
 * @param pp_test test to add
 *
 * @return int - zero for success
 */
static int pput_add(struct pp_test_t *pp_test)
{
	struct registered_test *rtest = pput_lookup(pp_test);

	if (unlikely(rtest))
		goto reg_test_add_add_done;

	rtest = kzalloc(sizeof(struct registered_test), GFP_KERNEL);
	if (unlikely(!rtest))
		return -ENOMEM;

	memcpy(&rtest->test, pp_test, sizeof(struct pp_test_t));
	rtest->test.test_name[PP_UT_NAME_LEN_MAX - 1] = '\0';
	list_add_tail(&rtest->link, &priv->tlist);
	pr_debug("Test %s was added successfully\n", rtest->test.test_name);

reg_test_add_add_done:
	return 0;
}

extern int pp_register_test(struct pp_test_t *pp_test)
{
	if (pput_add(pp_test))
		pr_err("pput_add failed");

	return 0;
}
EXPORT_SYMBOL(pp_register_test);

extern int pp_unregister_test(struct pp_test_t *pp_test)
{
	pput_remove(pp_test);

	return 0;
}
EXPORT_SYMBOL(pp_unregister_test);


void commands_show(struct seq_file *sf)
{
	u32 idx;

	seq_printf(sf, "\"DUMP <0..%u component (%u-all)> <1..3 level>\"\n",
		 PP_UT_NUM_COMPONENTS - 1, PP_UT_NUM_COMPONENTS);
	seq_printf(sf, "\"RUN <0..%u component (%u-all)> <1..3 level>\"\n\n",
		 PP_UT_NUM_COMPONENTS - 1, PP_UT_NUM_COMPONENTS);
	seq_printf(sf, "\"SINGLE <0..%u component><test_name>\"\n\n",
		   PP_UT_NUM_COMPONENTS - 1);
	seq_puts(sf, "Componenets:\n");
	for (idx = 0 ; idx < PP_UT_NUM_COMPONENTS ; idx++)
		seq_printf(sf, "%d. %s\n", idx, component_str[idx]);
}

void commands_write(char *cmd_buf, void *data)
{
	long word_cnt = 0;
	char *cmd[5] = {0, 0, 0, 0, 0};
	char *pch;
	long level;
	long component;

	pch = strsep(&cmd_buf, " ");
	for (word_cnt = 0 ; (word_cnt < 5) && (pch != NULL) ; word_cnt++) {
		cmd[word_cnt] = pch;
		pch = strsep(&cmd_buf, " ");
	}

	if (word_cnt != 3) {
		pr_err("Accept only 3 arguments\n");
		goto commands_write_done;
	}

	if (strncasecmp(cmd[0], "DUMP", sizeof("DUMP") - 1) == 0) {
		if (kstrtol(cmd[1], 10, &component))
			goto commands_write_done;
		if (kstrtol(cmd[2], 10, &level))
			goto commands_write_done;
		if (component > PP_UT_NUM_COMPONENTS) {
			pr_info("\"DUMP <0..%u component (%u-all)> <1..3 level>\"\n\n",
				PP_UT_NUM_COMPONENTS - 1, PP_UT_NUM_COMPONENTS);
			goto commands_write_done;
		}
		if ((level == 0) || (level > PP_UT_NUM_LEVELS)) {
			pr_info("\"DUMP <0..%u component (%u-all)> <1..3 level>\"\n\n",
				PP_UT_NUM_COMPONENTS - 1, PP_UT_NUM_COMPONENTS);
			goto commands_write_done;
		}
		pput_dump(component, level);
	} else if (strncasecmp(cmd[0], "RUN", sizeof("RUN") - 1) == 0) {
		if (kstrtol(cmd[1], 10, &component))
			goto commands_write_done;
		if (kstrtol(cmd[2], 10, &level))
			goto commands_write_done;
		if (component > PP_UT_NUM_COMPONENTS) {
			pr_info("\"RUN <0..%u component (%u-all)> <1..3 level>\"\n\n",
				PP_UT_NUM_COMPONENTS - 1, PP_UT_NUM_COMPONENTS);
			goto commands_write_done;
		}
		if ((level == 0) || (level > PP_UT_NUM_LEVELS)) {
			pr_info("\"RUN <0..%u component (%u-all)> <1..3 level>\"\n\n",
				PP_UT_NUM_COMPONENTS - 1, PP_UT_NUM_COMPONENTS);
			goto commands_write_done;
		}
		pput_run(component, level, NULL);
	} else if (strncasecmp(cmd[0], "SINGLE", sizeof("SINGLE") - 1) == 0) {
		if (kstrtol(cmd[1], 10, &component))
			goto commands_write_done;
		if (component > PP_UT_NUM_COMPONENTS) {
			pr_info("\"SINGLE <0..%u component><test_name>\"\n\n",
				PP_UT_NUM_COMPONENTS - 1);
			goto commands_write_done;
		}
		pput_run(component, 0, cmd[2]);
	} else {
		pr_info("Invalid action %s", cmd[0]);
	}

commands_write_done:
	return;
}

PP_DEFINE_DEBUGFS(commands, commands_show, commands_write);

static struct debugfs_file files[] = {
	{ "commands", &PP_DEBUGFS_FOPS(commands) },
};

int pput_init(struct dentry *parent)
{
	s32 ret;

	priv = kzalloc(sizeof(*priv), GFP_KERNEL);
	if (unlikely(!priv))
		return -ENOMEM;

	ret = pp_debugfs_create(parent, "pput", &priv->dentry, files,
				ARRAY_SIZE(files), NULL);
	if (unlikely(ret)) {
		pr_err("Failed to create the debugfs dir\n");
		return ret;
	}

	INIT_LIST_HEAD(&priv->tlist);

	/* Register all static tests */
#ifdef CONFIG_PPV4_LGM
	pp_infra_tests_init();
	pp_session_tests_init();
	pp_port_tests_init();
	pp_prsr_tests_init();
#endif /* CONFIG_PPV4_LGM */
	pp_qos_tests_init();
	pp_bm_tests_init();
	return 0;
}

void pput_exit(void)
{
	pr_debug("%s: start\n", __func__);
	if (unlikely(!priv))
		return;

	debugfs_remove_recursive(priv->dentry);
	kfree(priv);
	priv = NULL;
	pr_debug("%s: done\n", __func__);
}

