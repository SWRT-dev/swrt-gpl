/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "drvver.h"
#include "log_drv.h"
#include "nlmsgs.h"
#include "mtlkirbd.h"
#include "mtlkcdev.h"

#include <linux/module.h>

#define LOG_LOCAL_GID   GID_ROOT
#define LOG_LOCAL_FID   0

extern int rdebug;
extern int cdebug;
extern int bufs_no;
extern int buf_size;
extern int buf_swap_tmout;

#ifdef MTLK_DEBUG
int step_to_fail;
#endif

module_param(cdebug, int, 0);
module_param(rdebug, int, 0);
module_param(bufs_no, int, 0);
module_param(buf_size, int, 0);
module_param(buf_swap_tmout, int, 0);

int mtlk_genl_family_id = 0;
module_param(mtlk_genl_family_id, int, 0444);
EXPORT_SYMBOL(mtlk_genl_family_id);

#ifdef MTLK_DEBUG
module_param(step_to_fail, int, 0);
#endif

MODULE_PARM_DESC(cdebug,         "Console debug level");
MODULE_PARM_DESC(rdebug,         "Remote debug level");
MODULE_PARM_DESC(bufs_no,        "Number of buffers to allocate for Driver, Upper and Lower MACs");
MODULE_PARM_DESC(buf_size,       "Buffer size in bytes");
MODULE_PARM_DESC(buf_swap_tmout, "Log buffer swap timeout in msec");

#ifdef MTLK_DEBUG
MODULE_PARM_DESC(step_to_fail, "Init step to simulate fail");
#endif

MTLK_INIT_STEPS_LIST_BEGIN(root_drv)
  MTLK_INIT_STEPS_LIST_ENTRY(root_drv, ROOT_LOG_OSDEP)
  MTLK_INIT_STEPS_LIST_ENTRY(root_drv, ROOT_OSAL)
  MTLK_INIT_STEPS_LIST_ENTRY(root_drv, ROOT_CDEV_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(root_drv, ROOT_LOG)
  MTLK_INIT_STEPS_LIST_ENTRY(root_drv, ROOT_NL_INIT)
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  MTLK_INIT_STEPS_LIST_ENTRY(root_drv, ROOT_LOG_INIT_FINALIZE)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(root_drv, ROOT_IRBD)
MTLK_INIT_INNER_STEPS_BEGIN(root_drv)
MTLK_INIT_STEPS_LIST_END(root_drv);

static struct tag_ROOT_DRV_CONTEXT
{
  MTLK_DECLARE_INIT_STATUS;
} gRootDrv = {0};

static void
_mtlk_rootdrv_cleanup(void)
{
  MTLK_CLEANUP_BEGIN(root_drv, MTLK_OBJ_PTR(&gRootDrv))
    MTLK_CLEANUP_STEP(root_drv, ROOT_IRBD, MTLK_OBJ_PTR(&gRootDrv),
                      mtlk_irbd_root_cleanup, ());
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_CLEANUP_STEP(root_drv, ROOT_LOG_INIT_FINALIZE, MTLK_OBJ_PTR(&gRootDrv),
                      wave_log_before_cleanup, ());
#endif
    MTLK_CLEANUP_STEP(root_drv, ROOT_NL_INIT, MTLK_OBJ_PTR(&gRootDrv),
                      mtlk_nl_cleanup, ());
    MTLK_CLEANUP_STEP(root_drv, ROOT_LOG, MTLK_OBJ_PTR(&gRootDrv),
                      mtlk_log_cleanup, ());
    MTLK_CLEANUP_STEP(root_drv, ROOT_CDEV_INIT, MTLK_OBJ_PTR(&gRootDrv),
                      mtlk_cdev_drv_cleanup, ());
    MTLK_CLEANUP_STEP(root_drv, ROOT_OSAL, MTLK_OBJ_PTR(&gRootDrv),
                      mtlk_osal_cleanup, ());
    MTLK_CLEANUP_STEP(root_drv, ROOT_LOG_OSDEP, MTLK_OBJ_PTR(&gRootDrv),
                      log_osdep_cleanup, ());
  MTLK_CLEANUP_END(root_drv, MTLK_OBJ_PTR(&gRootDrv));
}

static int
_mtlk_rootdrv_init(void)
{
  MTLK_INIT_TRY(root_drv, MTLK_OBJ_PTR(&gRootDrv))
    MTLK_INIT_STEP_VOID(root_drv, ROOT_LOG_OSDEP, MTLK_OBJ_PTR(&gRootDrv),
                        log_osdep_init, ());
    MTLK_INIT_STEP(root_drv, ROOT_OSAL, MTLK_OBJ_PTR(&gRootDrv),
                   mtlk_osal_init, ());
    MTLK_INIT_STEP(root_drv, ROOT_CDEV_INIT, MTLK_OBJ_PTR(&gRootDrv),
                   mtlk_cdev_drv_init, ());
    MTLK_INIT_STEP(root_drv, ROOT_LOG, MTLK_OBJ_PTR(&gRootDrv),
                   mtlk_log_init, ());
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_INIT_STEP(root_drv, ROOT_NL_INIT, MTLK_OBJ_PTR(&gRootDrv),
                   mtlk_nl_init, (mtlk_log_parse_notification_msg, NULL, NL_DRV_CMD_RTLOG_NOTIFY));
#else
    MTLK_INIT_STEP(root_drv, ROOT_NL_INIT, MTLK_OBJ_PTR(&gRootDrv),
                   mtlk_nl_init, (NULL, NULL, 0));
#endif
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_INIT_STEP_VOID(root_drv, ROOT_LOG_INIT_FINALIZE, MTLK_OBJ_PTR(&gRootDrv),
                        MTLK_NOACTION, ());
#endif
    MTLK_INIT_STEP(root_drv, ROOT_IRBD, MTLK_OBJ_PTR(&gRootDrv),
                   mtlk_irbd_root_init, ());
  MTLK_INIT_FINALLY(root_drv, MTLK_OBJ_PTR(&gRootDrv))
  MTLK_INIT_RETURN(root_drv, MTLK_OBJ_PTR(&gRootDrv), _mtlk_rootdrv_cleanup, ())
}

static int __init
_mtlk_rootdrv_init_module(void)
{
  return (MTLK_ERR_OK == _mtlk_rootdrv_init()) ? 0 : -EFAULT;
}

static void __exit
_mtlk_rootdrv_cleanup_module(void)
{
  _mtlk_rootdrv_cleanup();
}

module_init(_mtlk_rootdrv_init_module);
module_exit(_mtlk_rootdrv_cleanup_module);

EXPORT_SYMBOL(mtlk_log_get_flags);

#ifdef CONFIG_WAVE_RTLOG_REMOTE
EXPORT_SYMBOL(mtlk_log_check_version);
EXPORT_SYMBOL(mtlk_log_new_pkt_reserve);
EXPORT_SYMBOL(mtlk_log_new_pkt_release);
EXPORT_SYMBOL(mtlk_log_new_pkt_reserve_multi);
EXPORT_SYMBOL(mtlk_log_new_pkt_release_multi);
#endif

MODULE_DESCRIPTION(DRV_DESCRIPTION);
MODULE_AUTHOR(MTLK_COPYRIGHT);
MODULE_LICENSE("GPL");
