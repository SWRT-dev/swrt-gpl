/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "dutserver.h"
#include "dut_utest_nv.h"

#define LOG_LOCAL_GID   GID_DUTSERVER
#define LOG_LOCAL_FID   1

BOOL toEndServer = FALSE;

int
main(int argc, char *argv[])
{
  setsid();

  ILOG0_SD("Lantiq DUT Server application v.%s, pid = %d", MTLK_SOURCE_VERSION, (int)getpid());

  dut_msg_clbk_init();

  if (MTLK_ERR_OK != dut_api_init(argc, argv))
  {
    ELOG_V("Failed to initialize DUT API");
    getchar();
    return 0;
  }

#ifdef MTLK_DEBUG
  if (NULL != strstr(argv[0], "dut_utest"))
  {
    dut_utest_nv();
    /* dut_utest_***(); */
  }
  else
#endif /* MTLK_DEBUG */
  {
    if (MTLK_ERR_OK != dut_comm_start_server())
    {
      ELOG_V("Failed to start listen socket");
      dut_api_cleanup();
      getchar();
      return 0;
    }

    /* suspend the thread until a signal is received */
    while (mtlk_osal_pause() != -1) {
      if (toEndServer)
        break;
    }

    dut_comm_end_server();
  }

  ILOG0_V("Dutserver is unloading...");
  dut_api_cleanup();

  return 0;
}

