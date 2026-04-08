/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#include <asm/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/file.h>
#include <sys/wait.h>

#include "drvhlpr.h"
#include "ledsctrl.h"
#include "mtlk_osal.h"

#define LOG_LOCAL_GID   GID_WPSSCTRL
#define LOG_LOCAL_FID   1

#define MTLK_SLEEP_OPEN_PIPE_FAILURE     2


static mtlk_osal_thread_t wps_thread;
static mtlk_osal_event_t  wps_thread_stop;
static uint8  wps_status=0;
static int led_resolution=1;
static int pipe_rslt=0;
static int open_file=1;           
static const char leds_fifo_file[] = "/tmp/WPS_LED_EVENTS";
static char *wps_script_path = "";
static int reconf_security;

static mtlk_handle_t wps_ctrl_start (void);
static void wps_ctrl_stop (mtlk_handle_t ctx);

const mtlk_component_api_t
irb_wps_ctrl_api = {
  wps_ctrl_start,
  NULL,
  wps_ctrl_stop
};


/****************************************************************
* wps_thrdatapipe
* New thread to handle the pipe read. The read value is evaluated in the SM.
* It reads and return also for non new value
* (!? no need to blocked until value is written to it?)
*/

static int32 __MTLK_IFUNC
wps_thrdatapipe (mtlk_handle_t context)
{
  char* script = wps_script_path;
  uint8 wps_status_tmp;
  int read_count = 0;
  int pid = 0;
  int status=0;
  int stop_thread = 0;

  /*Open is a loop:
    because the WPS is disabled/enabled, we may need to reopen file
  */
  while (stop_thread == 0) {
    if (open_file != 1) {
      goto end;
    }

    pipe_rslt = open (leds_fifo_file, O_RDONLY | O_NONBLOCK);
    if(pipe_rslt==-1)
    {
      ILOG2_S("failed to open %s pipe file for reading, try agin\n", leds_fifo_file);
      goto end;
    }
    else 
    {
      /*file is successfully opened*/
      open_file = 1;
      
      for (;;) {
        read_count= read(pipe_rslt, &wps_status_tmp, 1);
        if (read_count < 0) {
          ELOG_V("failed to read pipe file\n");
          close(pipe_rslt);
          goto end;
        }

        if (read_count == 0) {
          int wres = mtlk_osal_event_wait(&wps_thread_stop, led_resolution * 1000);
          if (wres == MTLK_ERR_OK) { /* stop  event is signalled */
            stop_thread = 1;
            goto end;
          }
        }
        else {
          wps_status = wps_status_tmp;
          /*set reconf_security to read security from conf file*/
          reconf_security = MTLK_DELAY_READ_SECURITY_CONF;
          leds_ctrl_set_reconf_security(reconf_security);

          if (wps_status >= '0')
            wps_status = wps_status-'0';
          if (script) {
            /*call to script with WPS event (all events !!)*/
            char tmp[] = {(char)(wps_status+'0'), '\0'};
            pid = fork();
            if (pid == 0) {
              ILOG2_DS("fork: write %d to %s script",wps_status,wps_script_path);
              if (execl(script, script, tmp, (char *)NULL) == -1) {
                ELOG_V("error execute execl command");
              }
              exit(EVENT_DO_NOTHING);
            }
          }
          ILOG2_D("Read char = 0x%x\n", wps_status);
          leds_ctrl_set_wps_stat(wps_status);
        }
        if (pid != 0)
          waitpid(pid, &status, 0);
        ILOG2_V("out waitpid");
      }
    }
end:
    if (pipe_rslt > 0) {
      ILOG2_V("close file\n");
      close(pipe_rslt);
    }

    if (!stop_thread) {
      int wres = mtlk_osal_event_wait(&wps_thread_stop, 
                                      (led_resolution + MTLK_SLEEP_OPEN_PIPE_FAILURE) * 1000);
      if (wres == MTLK_ERR_OK) { /* stop  event is signalled */
        stop_thread = 1;
      }
    }
  }

  return 0;
}


static BOOL
create_wps_task(void)
{
  int ret_val = 0;

  signal(SIGPIPE, SIG_IGN);

  pipe_rslt = 0;

  ret_val = mtlk_osal_event_init(&wps_thread_stop);
  if (ret_val != MTLK_ERR_OK) {
    ELOG_D("wps_thread init failed (err=%d)", ret_val);
    goto evt_init_failed;
  }

  ret_val = mtlk_osal_thread_init(&wps_thread);
  if (ret_val != MTLK_ERR_OK) {
    ELOG_D("wps_thread init failed (err=%d)", ret_val);
    goto thread_init_failed;
  }

  ret_val = mtlk_osal_thread_run(&wps_thread,
                                 wps_thrdatapipe,
                                 HANDLE_T(0));
  if (ret_val != MTLK_ERR_OK) {
    ELOG_D("wps_thread start failed (err=%d)", ret_val);
    goto thread_run_failed;
  }

  return TRUE;

thread_run_failed:
  mtlk_osal_thread_cleanup(&wps_thread);
thread_init_failed:
  mtlk_osal_event_cleanup(&wps_thread_stop);
evt_init_failed:
  return FALSE;
}

static void
delete_wps_task (void)
{
  mtlk_osal_event_set(&wps_thread_stop);

  mtlk_osal_thread_wait(&wps_thread, NULL);
  mtlk_osal_thread_cleanup(&wps_thread);
  mtlk_osal_event_cleanup(&wps_thread_stop);
}

static void leds_signal(int s);

static mtlk_handle_t
wps_ctrl_start (void)
{
  int              res = 0;
  struct sigaction setup_action;
  sigset_t         block_mask;

  ILOG1_V("wps_ctrl_start");

  ILOG2_V("set receive signal with SIGUSR1 and SIGUSR2 signals");
  sigemptyset (&block_mask);
  sigaddset (&block_mask, SIGUSR1);
  sigaddset (&block_mask, SIGUSR2);
  setup_action.sa_handler = leds_signal;
  setup_action.sa_mask = block_mask;
  setup_action.sa_flags = 0;

  if (sigaction (SIGUSR1, &setup_action, NULL) != 0) {
    ELOG_V("return error from sigaction ");
    goto end;
  }

  if (sigaction (SIGUSR2, &setup_action, NULL) != 0) {
    ELOG_V("return error from sigaction ");
    goto end;
  }

  res = create_wps_task();

end:
  return HANDLE_T(res);
}


static void
wps_ctrl_stop (mtlk_handle_t ctx)
{
  ILOG1_V("wps_ctrl_stop");
  delete_wps_task();
}

/**********************************************************************
* APIs
**********************************************************************/
void wps_ctrl_set_leds_resolution(int val)
{
  led_resolution = val;
  ILOG2_D("wps API: led_resolution = %d",led_resolution);
}

int wps_ctrl_set_wps_script_path(const char* str)
{
  return drvhlpr_set_script_path (&wps_script_path, str);
}

/****************************************************************
* leds_signal
* sigaction: examine and change signal action.
* s contains the signal sent.
* on SIGUSR1 close WPS_LED_EVENTS file,
* on SIGUSR2 reopen it.
*/
static void
leds_signal(int s) 
{
  /*need to close the file and wait to SIGUSR2*/
  ILOG2_DD("catch the signal, count = %d, signal = %d",open_file, s);
  if (s == 10) {
    /*on first entry close the file*/
    ILOG2_V("on first entry close the file");
    close(pipe_rslt);
    open_file = 0;
  }
  else {
    /*on second entry re-open the file, catch bad close of wps here as well*/
    if (open_file == 1) {
      open_file = 0; /*patch- in some cases leds_thrdatapipe reads garbage from the fifo,
                       so I set clear open_file in it occur before close file  */
      ILOG2_V("close signal missed");
      if (pipe_rslt > 0)
        close(pipe_rslt);
    }
    open_file = 1;
  }
}

