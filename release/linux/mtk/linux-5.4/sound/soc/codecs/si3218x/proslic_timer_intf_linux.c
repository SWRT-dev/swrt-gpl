// SPDX-License-Identifier: GPL-2.0
/*
 * $Id: proslic_timer_intf_win.c 109 2008-10-22 19:45:09Z lajordan@SILABS.COM $
 *
 * system.c
 * System specific functions implementation file
 *
 * Author(s): 
 * laj
 *
 * Distributed by: 
 * Silicon Laboratories, Inc
 *
 * File Description:
 * This is the implementation file for the system specific functions like timer functions.
 *
 * Dependancies:
 * datatypes.h
 *
 */
#include "config_inc/si_voice_datatypes.h"
#include "inc/si_voice_timer_intf.h"
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include "timer.h"
//#include <asm/div64.h>
#
/*
** Function: SYSTEM_TimerInit
*/
void TimerInit (systemTimer_S *pTimerObj){

}


/*
** Function: SYSTEM_Delay
*/
int time_DelayWrapper (void *hTimer, int timeInMs){
	mdelay(timeInMs);
	return 0;
}


/*
** Function: SYSTEM_TimeElapsed
*/
int time_TimeElapsedWrapper (void *hTimer, void *startTime, int *timeInMs){	*timeInMs = 1000;
	return 0;
}

/*
** Function: SYSTEM_GetTime
*/
int time_GetTimeWrapper (void *hTimer, void *time){
//	time->timestamp=0;
	return 0;
}

/*
** $Log: proslic_timer_intf_win.c,v $
** Revision 1.5  2008/07/24 21:06:16  lajordan
** no message
**
** Revision 1.4  2007/03/22 18:53:43  lajordan
** fixed warningg
**
** Revision 1.3  2007/02/26 16:46:16  lajordan
** cleaned up some warnings
**
** Revision 1.2  2007/02/16 23:55:07  lajordan
** no message
**
** Revision 1.1.1.1  2006/07/13 20:26:08  lajordan
** no message
**
** Revision 1.1  2006/07/07 21:38:56  lajordan
** no message
**
** Revision 1.1.1.1  2006/07/06 22:06:23  lajordan
** no message
**
** Revision 1.1  2006/06/29 19:17:21  laj
** no message
**
** Revision 1.1  2006/06/21 22:42:26  laj
** new api style
**
**
*/
