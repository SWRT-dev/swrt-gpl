
/****************************************************************************** 
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
 
  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.
 
******************************************************************************/

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include "common/ima_modules.h"
#include "atm/ima_tx_module.h"
#include "atm/ima_atm_rx.h"
#include "asm/ima_asm.h"

/* Define(or Undefine) this macro to run the tasklet using kthread(or hrtimer) */
#define USE_KTHREAD

static struct {

	spinlock_t spinlock;
#ifdef USE_KTHREAD
	struct task_struct * tasklet_thread;
#else
	ktime_t tx_poll_ktime, rx_poll_ktime, asm_poll_ktime;
	struct hrtimer tx_hr_timer, rx_hr_timer, asm_hr_timer;
#endif
	struct tasklet_struct tx_tasklet, rx_tasklet, asm_tasklet;

} g_workq_priv_data;

#define PRIV (&(g_workq_priv_data))
#define POLLINTERVAL_IN_JIFFIES(interval) usecs_to_jiffies((interval))

static void ima_tx_bh(unsigned long unused);  //schedule tx_process()
static void ima_rx_bh(unsigned long unused);  //schedule rx_process() 
static void ima_asm_bh(unsigned long unused); //schedule asm_process()

#ifdef USE_KTHREAD
	static int tasklet_threadfn(void *dummy);
#else
	enum hrtimer_restart ima_tx_isr(struct hrtimer *timer);
	enum hrtimer_restart ima_rx_isr(struct hrtimer *timer);
	enum hrtimer_restart ima_asm_isr(struct hrtimer *timer);
#endif

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
 * Task of this function is to create new tasklet, one each for tx, rx and asm.
 * Name of these work-queues are "tx_tasklet", "rx_tasklet" and
 * "asm_tasklet" respectively.
 * It gets called from vrx518 tc module(bonding_device_init function)
 */
s32 start_modules(unsigned int pollinterval)
{
	tasklet_init(&PRIV->tx_tasklet,  ima_tx_bh,  0);
	tasklet_init(&PRIV->rx_tasklet,  ima_rx_bh,  0);
	tasklet_init(&PRIV->asm_tasklet,  ima_asm_bh,  0);

#ifdef USE_KTHREAD
	spin_lock_init(&PRIV->spinlock);
	PRIV->tasklet_thread = NULL;

	if ( (PRIV->tasklet_thread = kthread_run( tasklet_threadfn, NULL, "ima_tasklet_timer" )) == NULL )
	{
		pr_crit( "Bonding Driver: Failed to start timer thread\n" );
		return -1;
	}
#else
	PRIV->tx_poll_ktime = ktime_set( 0, 500000UL ); // 0 seconds, 500 microseconds
	PRIV->rx_poll_ktime = ktime_set( 0, 1000000UL ); // 0 seconds, 1000 microseconds
	PRIV->asm_poll_ktime = ktime_set( 0, 1000000UL ); // 0 seconds, 1000 microseconds

	hrtimer_init(&PRIV->tx_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer_init(&PRIV->rx_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer_init(&PRIV->asm_hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	
	PRIV->tx_hr_timer.function = &ima_tx_isr;
	PRIV->rx_hr_timer.function = &ima_rx_isr;
	PRIV->asm_hr_timer.function = &ima_asm_isr;

	hrtimer_start( &PRIV->tx_hr_timer, PRIV->tx_poll_ktime, HRTIMER_MODE_REL);
	hrtimer_start( &PRIV->rx_hr_timer, PRIV->rx_poll_ktime, HRTIMER_MODE_REL);
	hrtimer_start( &PRIV->asm_hr_timer, PRIV->asm_poll_ktime, HRTIMER_MODE_REL);
#endif

	/* modules started */

	return RS_SUCCESS;
}

/*
 *	This module stops the kthread or stops the hrtimers
 *	for each task 
 */
s32 stop_modules(void)
{
#ifdef USE_KTHREAD
	if (PRIV->tasklet_thread)
	{
		/* Wait for the timer thread to terminate itself */
		kthread_stop( PRIV->tasklet_thread );	
		PRIV->tasklet_thread = NULL;
	}
#else
	hrtimer_cancel(&PRIV->tx_hr_timer);
	hrtimer_cancel(&PRIV->rx_hr_timer);
	hrtimer_cancel(&PRIV->asm_hr_timer);
#endif

	return RS_SUCCESS;
}
/*
 * Task of each of these tasklet functions   
 * is to call its processing-engine  
 */
void ima_tx_bh(unsigned long unused)
{
	process_tx();
#ifndef USE_KTHREAD
	hrtimer_start( &PRIV->tx_hr_timer, PRIV->tx_poll_ktime, HRTIMER_MODE_REL);
#endif
}

void ima_rx_bh(unsigned long unused)
{
	process_rx();
#ifndef USE_KTHREAD
	hrtimer_start( &PRIV->rx_hr_timer, PRIV->rx_poll_ktime, HRTIMER_MODE_REL);
#endif
}

void ima_asm_bh(unsigned long unused)
{
	process_asm();
#ifndef USE_KTHREAD
	hrtimer_start( &PRIV->asm_hr_timer, PRIV->asm_poll_ktime, HRTIMER_MODE_REL);
#endif
}

#ifdef USE_KTHREAD
static int tasklet_threadfn(void *dummy)
{
	u32 multiple;

	for(multiple = 0; !kthread_should_stop(); ++multiple)
	{
		spin_lock_bh( &PRIV->spinlock );
		tasklet_hi_schedule( &PRIV->tx_tasklet );
		if((multiple % 2) == 0) 
		{
			tasklet_hi_schedule( &PRIV->rx_tasklet );
			tasklet_hi_schedule( &PRIV->asm_tasklet );            
		}
		multiple++;
		spin_unlock_bh( &PRIV->spinlock );

		usleep_range(490 , 510);
	}
	/* Kill the tasklets */
	tasklet_kill( &PRIV->asm_tasklet );
	tasklet_kill( &PRIV->tx_tasklet );
	tasklet_kill( &PRIV->rx_tasklet );
		
	return 0;		
}
#else
/*
 *	Task of these timer functions  
 *	is to schedule the tasklet functions
 */

enum hrtimer_restart ima_tx_isr(struct hrtimer *timer)
{
	tasklet_hi_schedule( &PRIV->tx_tasklet );
	return HRTIMER_NORESTART;
}

enum hrtimer_restart ima_rx_isr(struct hrtimer *timer)
{
	tasklet_hi_schedule( &PRIV->rx_tasklet );
	return HRTIMER_NORESTART;
}

enum hrtimer_restart ima_asm_isr(struct hrtimer *timer)
{
	tasklet_hi_schedule( &PRIV->asm_tasklet );
	return HRTIMER_NORESTART;
}
#endif
