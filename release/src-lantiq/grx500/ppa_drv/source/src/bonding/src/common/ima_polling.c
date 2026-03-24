
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * Task of this module is to poll several resources like host umt address,
 * device side rxout ring owneship bit and wake up atm transmit tasklet, common
 * receive processor tasklet
 */

#include "common/ima_polling.h"
#include "atm/ima_atm_rx.h"
#include "atm/ima_tx_module.h"

static struct task_struct *g_st_p_worker_thread = NULL;

/*
 * Task of this module is to initialize the polling module
 */
s32 init_polling(void)
{
	s32 ret = RS_SUCCESS;

	return ret;
}

/*
 * Task of this function is to cleanup all the module initialized data
 */
s32 cleanup_polling(void)
{
	s32 ret = RS_SUCCESS;

	stop_polling_thread();

	return ret;
}

/*
 * This is the polling thread function. Task of this function is to poll
 * several resources like host umt address, device side rxout ring owneship
 * bit and wake up atm transmit tasklet, common receive processor tasklet
 */
static int th_worker_thread(void *p_v_p_param)
{
	int ret = 0;

	while (!kthread_should_stop()) {
		process_tx();
		process_rx();
		usleep_range(DELAY_MINIMUM, DELAY_MAXIMUM);
	}

	return ret;
}

/*
 * Task of this function is to create and run a kernel thread. This function
 * shall be called by vrx518 tc module(bonding_device_init function)
 */
s32 start_polling_thread(void)
{
	s32 ret = RS_SUCCESS;

	do {
		g_st_p_worker_thread = kthread_run(th_worker_thread, NULL, "ima_polling");

		if (!g_st_p_worker_thread) {
			ret = RS_FAILURE;
			ERROR_PRINTK("unable to create polling thread");
			break;
		}

	} while (0);

	return ret;
}

/*
 * Task of this functin is to stop a runnig kernel thread. This function shall
 * be called by either vrx518 tc module(bonding_device_deinit function) or from
 * this module cleanup_polling function
 */
s32 stop_polling_thread(void)
{
	s32 ret = 0;

	do {
		if (!g_st_p_worker_thread) {
			ret = RS_FAILURE;
			ERROR_PRINTK("failed to stop polling thread(null pointer)");
			break;
		}

		kthread_stop(g_st_p_worker_thread);
		g_st_p_worker_thread = NULL;

	} while (0);

	return ret;
}
