
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

/*
 * Task of this module is to run the RX process from a forever-looping worker function
 * This worker function goes to sleep after each call to process_rx(). And gets woken
 * through a waitqueue that gets triggered by a kernel timer launched everytime by
 * the worker function before it goes to sleep.
 */

#include <linux/kernel.h> // workqueue, timer, waitqueue
#include <linux/workqueue.h> // workqueue
#include <linux/wait.h> // wake_up_interruptible
#include <linux/kthread.h> // kthread_should_stop
#include <linux/delay.h>
#include "common/ima_modules.h"
#include "atm/ima_tx_module.h"  // process_tx
#include "atm/ima_atm_rx.h"  // process_rx
#include "asm/ima_asm.h"  // process_asm

static struct {

	struct workqueue_struct *tx_wq, *rx_wq, *asm_wq;
	struct work_struct tx_work, rx_work, asm_work;

	wait_queue_head_t tx_waitq, rx_waitq, asm_waitq; // waitq-s for each worker loop
	bool tx_done, rx_done, asm_done;
	bool tx_stop, rx_stop, asm_stop;

	struct timer_list tx_timer, rx_timer, asm_timer; // polling timer for each worker loop
	unsigned int tx_pollinterval, rx_pollinterval, asm_pollinterval; // in usecs

} g_workq_priv_data;

#define PRIV (&(g_workq_priv_data))
#define POLLINTERVAL_IN_JIFFIES(interval) usecs_to_jiffies((interval))

/*
 * Task of this function is to initialize the ima-workqueue data
 */
s32 init_modules(void)
{
	return RS_SUCCESS;
}

/*
 * Task of this function is to cleanup all the ima-workqueue initialized data
 */
s32 cleanup_modules(void)
{
	stop_modules();

	return RS_SUCCESS;
}

/*
 * These are tx, rx and asm worker functions. Task of each function is to repeatedly call
 * its processing-engine until time comes for the worker-function to exit.
 * Each worker function goes to sleep after processing-engine returns.
 * And wakes up after polling interval.
 */
static void ima_tx_process(struct work_struct *work)
{
//pr_info("[%s:%d] Entry\n", __func__, __LINE__);
	while (!PRIV->tx_stop && !kthread_should_stop()) {
		process_tx();
		//PRIV->tx_done = 0;
		//mod_timer(&PRIV->tx_timer, (jiffies + POLLINTERVAL_IN_JIFFIES(PRIV->tx_pollinterval)));
		//wait_event_interruptible(PRIV->tx_waitq,
		//	(PRIV->tx_done || PRIV->tx_stop || kthread_should_stop()));
		usleep_range(490, 510);
	}
//pr_info("[%s:%d] Exit\n", __func__, __LINE__);

	return;
}

static void ima_rx_process(struct work_struct *work)
{
//pr_info("[%s:%d] Entry\n", __func__, __LINE__);
	while (!PRIV->rx_stop && !kthread_should_stop()) {
		process_rx();
		//PRIV->rx_done = 0;
		//mod_timer(&PRIV->rx_timer, (jiffies + POLLINTERVAL_IN_JIFFIES(PRIV->rx_pollinterval)));
		//wait_event_interruptible(PRIV->rx_waitq,
		//	(PRIV->rx_done || PRIV->rx_stop || kthread_should_stop()));
		usleep_range(990, 1010);
	}
//pr_info("[%s:%d] Exit\n", __func__, __LINE__);

	return;
}

static void ima_asm_process(struct work_struct *work)
{
//pr_info("[%s:%d] Entry\n", __func__, __LINE__);
	while (!PRIV->asm_stop && !kthread_should_stop()) {
		process_asm();
		PRIV->asm_done = 0;
		mod_timer(&PRIV->asm_timer, (jiffies + POLLINTERVAL_IN_JIFFIES(PRIV->asm_pollinterval)));
		wait_event_interruptible(PRIV->asm_waitq,
			(PRIV->asm_done || PRIV->asm_stop || kthread_should_stop()));
	}
//pr_info("[%s:%d] Exit\n", __func__, __LINE__);

	return;
}

/*
 * This are the timer handler functions, one per worker-process. Task of each function
 * is to wake up corresponding ima_process function.
 */
static void ima_tx_timer_handler(unsigned long param)
{
	PRIV->tx_done = 1;
	wake_up_interruptible(&PRIV->tx_waitq);

	return;
}
static void ima_rx_timer_handler(unsigned long param)
{
	PRIV->rx_done = 1;
	wake_up_interruptible(&PRIV->rx_waitq);

	return;
}
static void ima_asm_timer_handler(unsigned long param)
{
	PRIV->asm_done = 1;
	wake_up_interruptible(&PRIV->asm_waitq);

	return;
}

/*
 * Task of this function is to create new work-queues, one each for tx, rx and asm.
 * Name of these work-queues are "ima_tx_workqueue", "ima_rx_workqueue" and
 * "ima_asm_workqueue" respectively.
 * It gets called from vrx518 tc module(bonding_device_init function)
 */
s32 start_modules(unsigned int pollinterval)
{
	PRIV->tx_wq = alloc_workqueue( "ima_tx_workqueue", WQ_MEM_RECLAIM|WQ_HIGHPRI|WQ_UNBOUND, 1); //create_workqueue("ima_tx_workqueue");
	if (!PRIV->tx_wq) {
		ERROR_PRINTK("unable to create Tx work queue");
		return RS_FAILURE; // -ENOWORKQUEUE perhaps ?
	}

	PRIV->rx_wq = alloc_workqueue( "ima_rx_workqueue", WQ_MEM_RECLAIM|WQ_HIGHPRI|WQ_UNBOUND, 1); //create_workqueue("ima_rx_workqueue");
	if (!PRIV->rx_wq) {
		ERROR_PRINTK("unable to create Rx work queue");
		return RS_FAILURE; // -ENOWORKQUEUE perhaps ?
	}

	PRIV->asm_wq = alloc_workqueue( "ima_asm_workqueue", WQ_MEM_RECLAIM|WQ_HIGHPRI|WQ_UNBOUND, 1); //create_workqueue("ima_asm_workqueue");
	if (!PRIV->asm_wq) {
		ERROR_PRINTK("unable to create ASM work queue");
		return RS_FAILURE; // -ENOWORKQUEUE perhaps ?
	}

	init_waitqueue_head(&PRIV->tx_waitq);
	init_waitqueue_head(&PRIV->rx_waitq);
	init_waitqueue_head(&PRIV->asm_waitq);

	PRIV->tx_pollinterval = pollinterval;
	setup_timer(&PRIV->tx_timer, ima_tx_timer_handler, 0);
	PRIV->rx_pollinterval = pollinterval;
	setup_timer(&PRIV->rx_timer, ima_rx_timer_handler, 0);
	PRIV->asm_pollinterval = pollinterval;
	setup_timer(&PRIV->asm_timer, ima_asm_timer_handler, 0);

	/* Pin workqueues to processors, or otherwise, depending upon Makefile directives */
	PRIV->tx_stop = 0;
	INIT_WORK(&PRIV->tx_work, ima_tx_process);
	PRIV->rx_stop = 0;
	INIT_WORK(&PRIV->rx_work, ima_rx_process);
	PRIV->asm_stop = 0;
	INIT_WORK(&PRIV->asm_work, ima_asm_process);
#if defined TX_CPU0_RX_CPU2
	queue_work_on(0,PRIV->tx_wq, &PRIV->tx_work);
	queue_work_on(2,PRIV->rx_wq, &PRIV->rx_work);
	queue_work_on(0,PRIV->asm_wq, &PRIV->asm_work);
#elif defined TX_CPU2_RX_CPU0
	queue_work_on(2,PRIV->tx_wq, &PRIV->tx_work);
	queue_work_on(0,PRIV->rx_wq, &PRIV->rx_work);
	queue_work_on(2,PRIV->asm_wq, &PRIV->asm_work);
#else
	queue_work(PRIV->tx_wq, &PRIV->tx_work);
	queue_work(PRIV->rx_wq, &PRIV->rx_work);
	queue_work(PRIV->asm_wq, &PRIV->asm_work);
#endif

	/* work started */

	return RS_SUCCESS;
}

/*
 * Task of this function is to stop all the workqueues and timers. Prior to
 * stopping a workqueue, its worker function needs to be made to stop.
 * This gets done by setting up the stop-flag of the worker function,
 * and then waking it up.
 */
s32 stop_modules(void)
{
	if (PRIV->tx_wq) {
		PRIV->tx_stop = 1;
		wake_up_interruptible(&PRIV->tx_waitq);
	}

	if (PRIV->rx_wq) {
		PRIV->rx_stop = 1;
		wake_up_interruptible(&PRIV->rx_waitq);
	}

	if (PRIV->asm_wq) {
		PRIV->asm_stop = 1;
		wake_up_interruptible(&PRIV->asm_waitq);
	}

	del_timer(&PRIV->tx_timer);
	del_timer(&PRIV->rx_timer);
	del_timer(&PRIV->asm_timer);

	if (PRIV->tx_wq) {
		flush_workqueue(PRIV->tx_wq);
		destroy_workqueue(PRIV->tx_wq);
		PRIV->tx_wq = NULL;
	}

	if (PRIV->rx_wq) {
		flush_workqueue(PRIV->rx_wq);
		destroy_workqueue(PRIV->rx_wq);
		PRIV->rx_wq = NULL;
	}

	if (PRIV->asm_wq) {
		flush_workqueue(PRIV->asm_wq);
		destroy_workqueue(PRIV->asm_wq);
		PRIV->asm_wq = NULL;
	}

	return RS_SUCCESS;
}
