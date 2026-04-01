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
#include "mtlkirba.h"
#include "mtlkirb_osdep.h"
#include "mtlkirbhash.h"
#include "iniparseraux.h"
#include "mtlknlink.h"
#include "nl.h"

#include <linux/kdev_t.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <signal.h>

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define LOG_LOCAL_GID   GID_MTLKIRBA
#define LOG_LOCAL_FID   1

#define MTLK_NODES_INI             "/proc/net/mtlk/" MTLK_IRB_INI_NAME

#include "mtlkhash.h"

MTLK_MHASH_DECLARE_ENTRY_T(irba, uint32);

MTLK_MHASH_DECLARE_INLINE(irba, uint32);

#define IRBA_HASH_MAX_IDX 6

static __INLINE uint32
_irba_hash_hash_func (const uint32 *key, uint32 nof_buckets)
{
  MTLK_UNREFERENCED_PARAM(nof_buckets);

  return (*key) % IRBA_HASH_MAX_IDX;
}

static __INLINE int
_irba_hash_key_cmp_func (const uint32 *key1,
                         const uint32 *key2)
{
  return (*key1) != (*key2); /* returns 0 if key1 == key2 */
}

MTLK_MHASH_DEFINE_INLINE(irba,
                         uint32,
                         _irba_hash_hash_func,
                         _irba_hash_key_cmp_func);

struct mtlk_irba_app
{
  mtlk_atomic_t        fname_cntr;
  mtlk_irba_t         *root;
  pthread_t            notification_thread;
  uint32               seq_no;
  int                  thread_exit_code;
  int                  stop_pipe_fd[2];
  mtlk_mhash_t         db;
  mtlk_osal_mutex_t    lock;
  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;
};

static struct mtlk_irba_app mtlk_irba_app;

mtlk_irba_t *mtlk_irba_root = NULL;

MTLK_INIT_STEPS_LIST_BEGIN(irba_app)
  MTLK_INIT_STEPS_LIST_ENTRY(irba_app, IRBA_APP_INIT_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(irba_app, IRBA_APP_INIT_DB)
  MTLK_INIT_STEPS_LIST_ENTRY(irba_app, IRBA_APP_ALLOC_ROOT)
  MTLK_INIT_STEPS_LIST_ENTRY(irba_app, IRBA_APP_INIT_ROOT)
MTLK_INIT_INNER_STEPS_BEGIN(irba_app)
MTLK_INIT_STEPS_LIST_END(irba_app);

MTLK_START_STEPS_LIST_BEGIN(irba_app)
  MTLK_START_STEPS_LIST_ENTRY(irba_app, IRBA_APP_PIPE)
  MTLK_START_STEPS_LIST_ENTRY(irba_app, IRBA_APP_THREAD)
MTLK_START_INNER_STEPS_BEGIN(irba_app)
MTLK_START_STEPS_LIST_END(irba_app);

void
signal_callback_handler(int signum)
{
  struct mtlk_irba_app *app_data = &mtlk_irba_app;
  write(app_data->stop_pipe_fd[1], "x", 1);
}

static void _mtlk_irba_packet_processor (void* param, void* packet);

void *
_irba_notification_thread_proc (void* param)
{
  struct mtlk_irba_app *app_data = &mtlk_irba_app;
  int res;
  mtlk_nlink_socket_t nl_socket;

  res = mtlk_nlink_create(&nl_socket, MTLK_NETLINK_IRBM_GROUP_NAME, _mtlk_irba_packet_processor, app_data);

  if(res < 0) {
    ELOG_SD("Failed to create netlink socket: %s (%d)", strerror(res), res);
    goto end;
  }

  res = mtlk_nlink_receive_loop(&nl_socket, app_data->stop_pipe_fd[0]);
  if(res < 0) {
    ELOG_SD("Netlink socket receive failed: %s (%d)", strerror(res), res);
  }

  mtlk_nlink_cleanup(&nl_socket);

end:
  app_data->thread_exit_code = res;
  return (void *) &app_data->thread_exit_code;
}

static void
_mtlk_irba_app_cleanup (struct mtlk_irba_app *app_data)
{
  MTLK_CLEANUP_BEGIN(irba_app, MTLK_OBJ_PTR(app_data))
    MTLK_CLEANUP_STEP(irba_app, IRBA_APP_INIT_ROOT, MTLK_OBJ_PTR(app_data),
                      mtlk_irba_cleanup, (app_data->root));
    MTLK_CLEANUP_STEP(irba_app, IRBA_APP_ALLOC_ROOT, MTLK_OBJ_PTR(app_data),
                      mtlk_irba_free, (app_data->root));
    MTLK_CLEANUP_STEP(irba_app, IRBA_APP_INIT_DB, MTLK_OBJ_PTR(app_data),
                      mtlk_mhash_cleanup_irba, (&app_data->db));
    MTLK_CLEANUP_STEP(irba_app, IRBA_APP_INIT_LOCK, MTLK_OBJ_PTR(app_data),
                      mtlk_osal_mutex_cleanup, (&app_data->lock));
  MTLK_CLEANUP_END(irba_app, MTLK_OBJ_PTR(app_data))
}

static void
_mtlk_irba_app_terminate_thread (struct mtlk_irba_app *app_data)
{
  void* status;
  int res;

  /* Signal the notification thread to stop */
  write(app_data->stop_pipe_fd[1], "x", 1);

  /* Wait for the notification thread to process the signal */
  res = pthread_join(app_data->notification_thread, &status);

  if(0 != res)
    ELOG_SD("Failed to terminate the notification thread: %s (%d)",
      strerror(res), res);
}

static void
_mtlk_irba_app_close_pipe_handles (struct mtlk_irba_app *app_data)
{
  close(app_data->stop_pipe_fd[0]);
  close(app_data->stop_pipe_fd[1]);
}

static void
_mtlk_irba_app_stop (struct mtlk_irba_app *app_data)
{
  MTLK_STOP_BEGIN(irba_app, MTLK_OBJ_PTR(app_data))
    MTLK_STOP_STEP(irba_app, IRBA_APP_THREAD, MTLK_OBJ_PTR(app_data), _mtlk_irba_app_terminate_thread, (app_data));
    MTLK_STOP_STEP(irba_app, IRBA_APP_PIPE, MTLK_OBJ_PTR(app_data), _mtlk_irba_app_close_pipe_handles, (app_data));
  MTLK_STOP_END(irba_app, MTLK_OBJ_PTR(app_data))
}

static void
_mtlk_irba_app_add_to_db (uint32 key, MTLK_MHASH_ENTRY_T(irba) *hentry)
{
  struct mtlk_irba_app *app_data = &mtlk_irba_app;

  mtlk_osal_mutex_acquire(&app_data->lock);
  mtlk_mhash_insert_irba(&app_data->db, &key, hentry);
  mtlk_osal_mutex_release(&app_data->lock);
}

static void
_mtlk_irba_app_del_from_db (MTLK_MHASH_ENTRY_T(irba) *hentry)
{
  struct mtlk_irba_app *app_data = &mtlk_irba_app;

  mtlk_osal_mutex_acquire(&app_data->lock);
  mtlk_mhash_remove_irba(&app_data->db, hentry);
  mtlk_osal_mutex_release(&app_data->lock);
}

mtlk_error_t __MTLK_IFUNC
mtlk_irba_app_init (mtlk_irba_rm_handler_f handler,
                    mtlk_handle_t          context)
{
  struct mtlk_irba_app *app_data = &mtlk_irba_app;

  MTLK_ASSERT(mtlk_irba_root == NULL);

  mtlk_osal_atomic_set(&app_data->fname_cntr, 0);
  app_data->seq_no = 0;

  MTLK_INIT_TRY(irba_app, MTLK_OBJ_PTR(app_data))
    MTLK_INIT_STEP(irba_app, IRBA_APP_INIT_LOCK,  MTLK_OBJ_PTR(app_data),
                   mtlk_osal_mutex_init, (&app_data->lock));
    MTLK_INIT_STEP(irba_app, IRBA_APP_INIT_DB,  MTLK_OBJ_PTR(app_data), mtlk_mhash_init_irba,
                   (&app_data->db, IRBA_HASH_MAX_IDX));
    MTLK_INIT_STEP_EX(irba_app, IRBA_APP_ALLOC_ROOT,  MTLK_OBJ_PTR(app_data),
                      mtlk_irba_alloc, (),
                      app_data->root,
                      app_data->root != NULL,
                      MTLK_ERR_NO_MEM);
    MTLK_INIT_STEP(irba_app, IRBA_APP_INIT_ROOT,  MTLK_OBJ_PTR(app_data),
                   mtlk_irba_init, (app_data->root, MTLK_IRB_ROOT_DESC, handler, context));
  MTLK_INIT_FINALLY(irba_app, MTLK_OBJ_PTR(app_data))
  MTLK_INIT_RETURN(irba_app, MTLK_OBJ_PTR(app_data), _mtlk_irba_app_cleanup, (app_data))
}

mtlk_error_t __MTLK_IFUNC
mtlk_irba_app_start (void)
{
  struct mtlk_irba_app *app_data = &mtlk_irba_app;
  pthread_attr_t        attr;
  int                   err;

  MTLK_ASSERT(mtlk_irba_root == NULL);

  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  MTLK_START_TRY(irba_app, MTLK_OBJ_PTR(app_data))
    MTLK_START_STEP_EX(irba_app, IRBA_APP_PIPE, MTLK_OBJ_PTR(app_data), pipe, (app_data->stop_pipe_fd),
                       err,
                       err == 0,
                       MTLK_ERR_NO_RESOURCES);
    MTLK_START_STEP_EX(irba_app, IRBA_APP_THREAD, MTLK_OBJ_PTR(app_data), pthread_create,
                       (&app_data->notification_thread, &attr, _irba_notification_thread_proc, NULL),
                       err,
                       err == 0,
                       MTLK_ERR_NO_RESOURCES);
  MTLK_START_FINALLY(irba_app, MTLK_OBJ_PTR(app_data))
    pthread_attr_destroy(&attr);
    mtlk_irba_root = app_data->root;
  MTLK_START_RETURN(irba_app, MTLK_OBJ_PTR(app_data), _mtlk_irba_app_stop, (app_data))
}

int __MTLK_IFUNC
mtlk_irba_app_create_stop_pipe (void)
{
  if (pipe(mtlk_irba_app.stop_pipe_fd))
  {
    return MTLK_ERR_NO_RESOURCES;
  }

  struct sigaction act;
  act.sa_handler = &signal_callback_handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;

  if (sigaction(SIGINT, &act, NULL) < 0)
  {
    fprintf(stderr, "Sigaction failed for SIGINT: %s\n", strerror(errno));
    return MTLK_ERR_SYSTEM;
  }

  if (sigaction(SIGTERM, &act, NULL) < 0)
  {
    fprintf(stderr, "Sigaction failed for SIGTERM: %s\n", strerror(errno));
    return MTLK_ERR_SYSTEM;
  }

  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
mtlk_irba_app_stop (void)
{
  struct mtlk_irba_app *app_data = &mtlk_irba_app;

  MTLK_ASSERT(mtlk_irba_root != NULL);
  mtlk_irba_root = NULL;

  _mtlk_irba_app_stop(app_data);
}

void __MTLK_IFUNC
mtlk_irba_app_close_pipe_handles(void)
{
  struct mtlk_irba_app *app_data = &mtlk_irba_app;
  _mtlk_irba_app_close_pipe_handles(app_data);
}

void __MTLK_IFUNC
mtlk_irba_app_cleanup (void)
{
  struct mtlk_irba_app *app_data = &mtlk_irba_app;

  MTLK_ASSERT(mtlk_irba_root == NULL);

  _mtlk_irba_app_cleanup(app_data);
}

struct mtlk_irba_private_handler
{
  mtlk_guid_t             evt;
  mtlk_irba_evt_handler_f handler;
  mtlk_handle_t           context;
};

static void
_mtlk_irba_on_node_disappears(mtlk_irba_t       *irba,
                              mtlk_handle_t      context,
                              const mtlk_guid_t *evt,
                              void              *buffer,
                              uint32             size);

static const struct mtlk_irba_private_handler mtlk_irba_private_handlers[] = {
  { MTLK_IRB_GUID_NODE_DISAPPEARS, _mtlk_irba_on_node_disappears, HANDLE_T(0) }
};

struct mtlk_irba_private
{
  char                    *unique_desc;
  int                      major;
  int                      minor;
  int                      fd;
  char                     fname[256];
  mtlk_irb_hash_t          hash;
  MTLK_MHASH_ENTRY_T(irba) hentry;
  mtlk_irba_rm_handler_f   rm_handler;
  mtlk_handle_t            rm_handler_ctx;
  mtlk_irba_handle_t      *private_handlers[ARRAY_SIZE(mtlk_irba_private_handlers)];
  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_INIT_LOOP(IRBA_INTERNAL_HANDLERS);
};

MTLK_INIT_STEPS_LIST_BEGIN(irba)
  MTLK_INIT_STEPS_LIST_ENTRY(irba, IRBA_GET_MAJOR)
  MTLK_INIT_STEPS_LIST_ENTRY(irba, IRBA_GET_MINOR)
  MTLK_INIT_STEPS_LIST_ENTRY(irba, IRBA_DUP_DESC)
  MTLK_INIT_STEPS_LIST_ENTRY(irba, IRBA_MKNOD)
  MTLK_INIT_STEPS_LIST_ENTRY(irba, IRBA_OPEN)
  MTLK_INIT_STEPS_LIST_ENTRY(irba, IRBA_HASH)
  MTLK_INIT_STEPS_LIST_ENTRY(irba, IRBA_ADD_TO_DB)
  MTLK_INIT_STEPS_LIST_ENTRY(irba, IRBA_INTERNAL_HANDLERS)
MTLK_INIT_INNER_STEPS_BEGIN(irba)
  MTLK_INIT_STEPS_LIST_ENTRY(irba, IRBA_INI_LOAD)
MTLK_INIT_STEPS_LIST_END(irba);

static void
_mtlk_irba_on_node_disappears (mtlk_irba_t       *irba,
                               mtlk_handle_t      context,
                               const mtlk_guid_t *evt,
                               void              *buffer,
                               uint32             size)
{
  MTLK_ASSERT(irba != NULL);
  MTLK_ASSERT(irba->rm_handler != NULL);

  irba->rm_handler(irba, irba->rm_handler_ctx);
}

static void
_mtlk_irba_packet_processor (void* param, void* packet)
{
  struct mtlk_irba_app     *app_data = &mtlk_irba_app;
  struct mtlk_irb_ntfy_hdr *hdr      = (struct mtlk_irb_ntfy_hdr *)packet;
  char                     *data     = (void*)(hdr + 1);
  mtlk_mhash_find_t         ctx;
  MTLK_MHASH_ENTRY_T(irba) *h;

  /* Check sequence number of received packet */
  if( (-1 != app_data->seq_no) &&
      (hdr->seq_no - app_data->seq_no > 1) )
    WLOG_DD("APP notifications arrived out of order "
            "(current SN: %d, received SN: %d)",
            app_data->seq_no, hdr->seq_no);
  app_data->seq_no = hdr->seq_no;

  mtlk_osal_mutex_acquire(&app_data->lock);
  h = mtlk_mhash_find_first_irba(&app_data->db, &hdr->cdev_minor, &ctx);
  while (h) {
    mtlk_irba_t *irba = MTLK_CONTAINER_OF(h, mtlk_irba_t, hentry);
    _mtlk_irb_hash_on_evt(&irba->hash, &hdr->evt, data, &hdr->data_size, HANDLE_T(irba));
    h = mtlk_mhash_find_next_irba(&app_data->db, &hdr->cdev_minor, &ctx);
  }
  mtlk_osal_mutex_release(&app_data->lock);
}

void __MTLK_IFUNC
_mtlk_irb_call_handler (mtlk_handle_t      evt_contex,
                        void              *handler,
                        mtlk_handle_t      handler_contex,
                        const mtlk_guid_t *evt,
                        void              *buffer,
                        uint32            *size)
{
  mtlk_irba_t             *irba = HANDLE_T_PTR(mtlk_irba_t, evt_contex);
  mtlk_irba_evt_handler_f  h    = (mtlk_irba_evt_handler_f)handler;

  MTLK_ASSERT(irba != NULL);
  MTLK_ASSERT(h    != NULL);

  if (irba && h) {
    h(irba, handler_contex, evt, buffer, *size);
  } else {
    if (size)
      *size = 0;
  }
}

static BOOL
_mtlk_irba_check_desc (const char *desc)
{
  if (strpbrk(desc, "\\/%"))
    return FALSE;

  if (strstr(desc, ".."))
    return FALSE;

  return TRUE;
}

static mtlk_error_t
_mtlk_irba_get_path (char *dest_path, size_t size, const char *env_path, const char *def_path)
{
  const char *path = env_path ? env_path : def_path;

  /* Relative path or path traversal is not allowed */
  if ((path[0] == '\0') || path[0] != '/')
    return MTLK_ERR_PARAMS;

  if (strpbrk(path, "\\%"))
    return MTLK_ERR_PARAMS;

  if (strstr(path, ".."))
    return MTLK_ERR_PARAMS;

  if (!wave_strcopy(dest_path, path, size))
    return MTLK_ERR_BUF_TOO_SMALL;

  return MTLK_ERR_OK;
}

static mtlk_error_t
_mtlk_irba_get_fname (char *buffer, size_t size, const char *unique_desc)
{
  uint32 ctr = mtlk_osal_atomic_inc(&mtlk_irba_app.fname_cntr);
  uint32 pid = (uint32)getpid();
  const char* env_path = getenv("DEV_CREAT_PATH");
  char path[256];
  mtlk_error_t res;

  /* Check descriptor */
  if (!_mtlk_irba_check_desc(unique_desc))
    return MTLK_ERR_PARAMS;

  res = _mtlk_irba_get_path(path, sizeof(path), env_path, "/tmp");
  if (MTLK_ERR_OK != res)
    return res;

  /* in openWRT default permissions do not allow creating devices on /tmp */
  /* Use /tmp unless otherwise configured in env var */
  res = sprintf_s(buffer, size, "%s/mtlk_%s_%u_%u", path, unique_desc, pid, ctr);
  if (res < 0)
    return MTLK_ERR_PARAMS;
  if (res >= size)
    return MTLK_ERR_BUF_TOO_SMALL;

  return MTLK_ERR_OK;
}

mtlk_irba_t * __MTLK_IFUNC
mtlk_irba_alloc (void)
{
  mtlk_irba_t *irba = malloc(sizeof(*irba));
  if (irba) memset(irba, 0, sizeof(*irba));
  return irba;
}

mtlk_error_t __MTLK_IFUNC
mtlk_irba_init (mtlk_irba_t           *irba,
                const char            *unique_desc,
                mtlk_irba_rm_handler_f handler,
                mtlk_handle_t          context)
{
  int err = -1;
  size_t len, i;
  mtlk_error_t res;
  dictionary *dict = NULL;

  MTLK_ASSERT(unique_desc != NULL);
  MTLK_ASSERT(handler != 0);
  if (!irba || !unique_desc || !handler)
    return MTLK_ERR_PARAMS;

  res = _mtlk_irba_get_fname(irba->fname, sizeof(irba->fname), unique_desc);
  if (MTLK_ERR_OK != res)
    return res;
  unlink(irba->fname); /* "just in case" - nobody can now use this file (it contains PID and cntr) */

  len = strnlen_s(unique_desc, MTLK_IRB_MAX_UNIQ_DESC_LEN);
  /* RM handler related assignment must be done before the IRBA_INTERNAL_HANDLERS */
  irba->rm_handler     = handler;
  irba->rm_handler_ctx = context;

  MTLK_INIT_TRY(irba, MTLK_OBJ_PTR(irba))
    MTLK_INIT_STEP_EX(irba, IRBA_INI_LOAD, MTLK_OBJ_NONE,
                      iniparser_load, (MTLK_NODES_INI),
                      dict,
                      dict != NULL,
                      MTLK_ERR_NO_RESOURCES);
    MTLK_INIT_STEP_EX(irba, IRBA_GET_MAJOR, MTLK_OBJ_PTR(irba),
                      iniparser_aux_getint, (dict, unique_desc, MTLK_IRB_INI_CDEV_MAJOR, -1),
                      irba->major,
                      irba->major > 0,
                      MTLK_ERR_NO_ENTRY);
    MTLK_INIT_STEP_EX(irba, IRBA_GET_MINOR, MTLK_OBJ_PTR(irba),
                      iniparser_aux_getint, (dict, unique_desc, MTLK_IRB_INI_CDEV_MINOR, -1),
                      irba->minor,
                      irba->minor >= 0,
                      MTLK_ERR_NO_ENTRY);
    MTLK_INIT_STEP_EX(irba, IRBA_DUP_DESC, MTLK_OBJ_PTR(irba),
                      malloc, (len + 1),
                      irba->unique_desc,
                      irba->unique_desc != NULL,
                      MTLK_ERR_NO_MEM);
    wave_memcpy(irba->unique_desc, len + 1, unique_desc, len + 1);
    MTLK_INIT_STEP_EX(irba, IRBA_MKNOD, MTLK_OBJ_PTR(irba),
                      mknod, (irba->fname, S_IRUSR | S_IFCHR, MKDEV(irba->major, irba->minor)),
                      err,
                      err != -1,
                      MTLK_ERR_SYSTEM);
    MTLK_INIT_STEP_EX(irba, IRBA_OPEN, MTLK_OBJ_PTR(irba),
                      open, (irba->fname, O_RDWR),
                      irba->fd,
                      irba->fd != -1,
                      MTLK_ERR_SYSTEM);
    MTLK_INIT_STEP(irba, IRBA_HASH, MTLK_OBJ_PTR(irba),
                   _mtlk_irb_hash_init, (&irba->hash));
    MTLK_INIT_STEP_VOID(irba, IRBA_ADD_TO_DB, MTLK_OBJ_PTR(irba),
                        _mtlk_irba_app_add_to_db, (irba->minor, &irba->hentry));

    for (i = 0; i < ARRAY_SIZE(irba->private_handlers); i++) {
      const struct mtlk_irba_private_handler *h = &mtlk_irba_private_handlers[i];
      MTLK_INIT_STEP_LOOP_EX(irba, IRBA_INTERNAL_HANDLERS,  MTLK_OBJ_PTR(irba),
                             mtlk_irba_register, (irba, &h->evt, 1, h->handler, h->context),
                             irba->private_handlers[i],
                             irba->private_handlers[i] != NULL,
                             MTLK_ERR_NO_MEM);
    }
  MTLK_INIT_FINALLY(irba, MTLK_OBJ_PTR(irba))
    unlink(irba->fname); /* "delete on close" implementation */
    MTLK_CLEANUP_STEP(irba, IRBA_INI_LOAD, MTLK_OBJ_NONE, iniparser_freedict, (dict));
  MTLK_INIT_RETURN(irba, MTLK_OBJ_PTR(irba), mtlk_irba_cleanup, (irba))
}

void __MTLK_IFUNC
mtlk_irba_cleanup (mtlk_irba_t *irba)
{
  size_t i;

  MTLK_ASSERT(irba != NULL);
  if (!irba)
    return;

  MTLK_CLEANUP_BEGIN(irba, MTLK_OBJ_PTR(irba))
    for (i = 0; i < ARRAY_SIZE(irba->private_handlers); i++) {
      if (NULL == irba->private_handlers[i]) {
        continue;
      }

      MTLK_CLEANUP_STEP_LOOP(irba, IRBA_INTERNAL_HANDLERS, MTLK_OBJ_PTR(irba),
                             mtlk_irba_unregister, (irba, irba->private_handlers[i]));

      irba->private_handlers[i] = NULL;
    }

    MTLK_CLEANUP_STEP(irba, IRBA_ADD_TO_DB, MTLK_OBJ_PTR(irba),
                      _mtlk_irba_app_del_from_db, (&irba->hentry));
    MTLK_CLEANUP_STEP(irba, IRBA_HASH, MTLK_OBJ_PTR(irba),
                      _mtlk_irb_hash_cleanup, (&irba->hash));
    MTLK_CLEANUP_STEP(irba, IRBA_OPEN, MTLK_OBJ_PTR(irba),
                      close, (irba->fd));
    MTLK_CLEANUP_STEP(irba, IRBA_MKNOD, MTLK_OBJ_PTR(irba),
                      unlink, (irba->fname));
    MTLK_CLEANUP_STEP(irba, IRBA_DUP_DESC, MTLK_OBJ_PTR(irba),
                      free, (irba->unique_desc));
    MTLK_CLEANUP_STEP(irba, IRBA_GET_MINOR, MTLK_OBJ_PTR(irba), MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(irba, IRBA_GET_MAJOR, MTLK_OBJ_PTR(irba), MTLK_NOACTION, ());
  MTLK_CLEANUP_END(irba, MTLK_OBJ_PTR(irba))
}

void __MTLK_IFUNC
mtlk_irba_free (mtlk_irba_t *irba)
{
  MTLK_ASSERT(irba != NULL);
  if (irba)
    free(irba);
}

mtlk_irba_handle_t *__MTLK_IFUNC
mtlk_irba_register (mtlk_irba_t            *irba,
                    const mtlk_guid_t      *evts,
                    uint32                  nof_evts,
                    mtlk_irba_evt_handler_f handler,
                    mtlk_handle_t           context)
{
  mtlk_handle_t h;

  MTLK_ASSERT(irba != NULL);
  if (!irba)
    return NULL;

  h = _mtlk_irb_hash_register(&irba->hash, evts, nof_evts, handler, context);

  return HANDLE_T_PTR(mtlk_irba_handle_t, h);
}

void __MTLK_IFUNC
mtlk_irba_unregister (mtlk_irba_t        *irba,
                      mtlk_irba_handle_t *irbah)
{
  MTLK_ASSERT(irba != NULL);
  if (!irba)
    return;

  _mtlk_irb_hash_unregister(&irba->hash, HANDLE_T(irbah));
}

mtlk_error_t __MTLK_IFUNC
mtlk_irba_call_drv (mtlk_irba_t       *irba,
                    const mtlk_guid_t *evt,
                    void              *buffer,
                    uint32             size)
{
  struct mtlk_irb_call_hdr *hdr;
  mtlk_error_t              res = MTLK_ERR_UNKNOWN;

  MTLK_ASSERT(irba != NULL);
  MTLK_ASSERT(evt != NULL);
  MTLK_ASSERT(!size || (buffer != NULL));
  if(!irba || !evt || (!buffer && size))
    return MTLK_ERR_PARAMS;

  hdr = (struct mtlk_irb_call_hdr *)malloc(sizeof(*hdr) + size);
  if (!hdr) {
    ELOG_D("Can't allocate IRB call driver struct of %u bytes", sizeof(*hdr) + size);
    res = MTLK_ERR_NO_MEM;
    goto end;
  }

  hdr->evt       = *evt;
  hdr->data_size = size;
  wave_memcpy(hdr + 1, size, buffer, size);

  res = ioctl(irba->fd, MTLK_CDEV_IRB_IOCTL, hdr);
  if (res != 0) {
    ELOG_D("IRB IOCTL failed (%d)", res);
    res = MTLK_ERR_UNKNOWN;
    goto end;

  }

  wave_memcpy(buffer, size, hdr + 1, size);

  res = MTLK_ERR_OK;

end:
  if (hdr) {
    free(hdr);
  }

  return res;
}

