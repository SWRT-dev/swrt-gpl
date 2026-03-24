
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * Manager module of ima+ bonding driver.
 */

#include "manager/ima_manager.h"
#include "manager/ima_bonding_debugfs.h"
#include "common/ima_debug_manager.h"
#include "common/ima_modules.h"

/*
 * This is the entry point of IMA+ bonding driver
 */
static int __init manager_init(void)
{
	int i_ret = 0;

pr_info("[%s:%d] Entry\n", __func__, __LINE__);
	i_ret = init_common();
	if (i_ret != RS_SUCCESS)
		goto err_common;

	i_ret = init_atm_rx();
	if (i_ret != RS_SUCCESS)
		goto err_atm_rx;

	i_ret = init_modules();
	if (i_ret != RS_SUCCESS)
		goto err_modules;

	i_ret = init_vrx518_tc_interface();
	if (i_ret != RS_SUCCESS)
		goto err_vrx518_tc_interface;

	i_ret = init_debug_manager();
	if (i_ret != RS_SUCCESS)
		goto err_debug_manager;
	else
		goto success;

err_debug_manager:
	cleanup_vrx518_tc_interface();
err_vrx518_tc_interface:
	cleanup_modules();
err_modules:
	cleanup_atm_rx();
err_atm_rx:
	cleanup_common();
err_common:
success:

pr_info("[%s:%d] Exit\n", __func__, __LINE__);

	return i_ret;
}

/*
 * This is the exit/cleanup point of IMA+ bonding driver
 */
static void __exit manager_exit(void)
{
pr_info("[%s:%d] Entry\n", __func__, __LINE__);
	cleanup_debug_manager();
	cleanup_vrx518_tc_interface();
	cleanup_modules();
	cleanup_atm_rx();
	cleanup_common();
pr_info("[%s:%d] Exit\n", __func__, __LINE__);
}

/* Register IMA+ bonding driver entry, exit point to Linux kernel */
module_init(manager_init);
module_exit(manager_exit);
