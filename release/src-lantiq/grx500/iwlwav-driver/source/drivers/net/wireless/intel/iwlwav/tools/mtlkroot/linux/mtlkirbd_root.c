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
#include "mtlkirbd.h"
#include "mtlkcdev.h"
#include "mtlkirbhash.h"
#include "nlmsgs.h"

#include <linux/seq_file.h>
#include <linux/module.h>

#define LOG_LOCAL_GID   GID_IRB
#define LOG_LOCAL_FID   0

#define MTLK_IRB_HDR_ALIGNMENT 4

static mtlk_guid_t MTLK_IRB_NODE_DISAPPEARS = MTLK_IRB_GUID_NODE_DISAPPEARS;

typedef enum
{
  MTLK_NT_ROOT,
  MTLK_NT_DRV,
  MTLK_NT_CARD,
  MTLK_NT_WLAN,
  MTLK_NT_STA,
  MTLK_NT_STA_FLAGS,
  MTLK_NT_LAST
} mtlk_node_type_e;

struct mtlk_irbd_private
{
  mtlk_cdev_node_t    *cdev_node;
  mtlk_dlist_t         sons;
  mtlk_osal_spinlock_t lock;
  mtlk_dlist_entry_t   lentry;
  char                *unique_desc;
  mtlk_irbd_t         *parent;
  mtlk_node_type_e     type;
  mtlk_atomic_t        notify_seq_no;
  mtlk_irb_hash_t      hash;
  MTLK_DECLARE_INIT_STATUS;
};

struct mtlk_irbd_drv {
  mtlk_cdev_t   cdev;
  mtlk_irbd_t  *root_irbd;
  MTLK_DECLARE_INIT_STATUS;
};

static struct mtlk_irbd_drv mtlk_irbd_drv;

static long
_mtlk_irbd_ioctl_handler (mtlk_handle_t ctx, 
                          unsigned int  cmd, 
                          unsigned long arg)
{
  long                     res  = -EINVAL;
  mtlk_irbd_t             *irbd = HANDLE_T_PTR(mtlk_irbd_t, ctx);
  char __user             *buff = (char __user *)arg;
  struct mtlk_irb_call_hdr hdr;
  void                    *data = NULL;

  if (!capable(CAP_NET_ADMIN)) {
    ELOG_V("Permission denied");
    res = -EPERM;
    goto end;
  }

  if (!irbd) {
    res  = -EINVAL;
    goto end;
  }

  if (cmd != MTLK_CDEV_IRB_IOCTL) {
    res  = -EINVAL;
    goto end;
  }

  if (0 != (copy_from_user(&hdr, buff, sizeof(hdr)))) {
    res  = -EINVAL;
    goto end;
  }  

  if (hdr.data_size) {
    data = kmalloc_tag(hdr.data_size, GFP_KERNEL, MTLK_MEM_TAG_IRB_DATA);
    if (NULL == data) {
      res = -ENOMEM;
      goto end;
    }

    if (0 != (copy_from_user(data, buff + sizeof(hdr), hdr.data_size))) {
      res = -EINVAL;
      goto end;
    }
  }

  _mtlk_irb_hash_on_evt(&irbd->hash, &hdr.evt, data, &hdr.data_size, HANDLE_T(irbd));
  if (hdr.data_size && 
      (0 != (copy_to_user(buff + sizeof(hdr), data, hdr.data_size)))) {
    res = -EINVAL;
    goto end;
  }
  res = 0;

end:
  if (data) {
    kfree_tag(data);
  }
  return res;
}

static void
_mtlk_irbd_add_son (mtlk_irbd_t *irbd, mtlk_irbd_t *to_add)
{
  mtlk_osal_lock_acquire(&irbd->lock);
  mtlk_dlist_push_back(&irbd->sons, &to_add->lentry);
  mtlk_osal_lock_release(&irbd->lock);
}

static void
_mtlk_irbd_del_son (mtlk_irbd_t *irbd, mtlk_irbd_t *to_del)
{
  mtlk_osal_lock_acquire(&irbd->lock);
  mtlk_dlist_remove(&irbd->sons, &to_del->lentry);
  mtlk_osal_lock_release(&irbd->lock);
}

static int _mtlk_irbd_init(mtlk_irbd_t *irbd,
                           mtlk_irbd_t *parent,
                           const char  *unique_desc);

MTLK_INIT_STEPS_LIST_BEGIN(irbd_drv)
  MTLK_INIT_STEPS_LIST_ENTRY(irbd_drv, IRBD_CDEV)
  MTLK_INIT_STEPS_LIST_ENTRY(irbd_drv, IRBD_ROOT_IRBD_ALLOC)
  MTLK_INIT_STEPS_LIST_ENTRY(irbd_drv, IRBD_ROOT_IRBD_INIT)
MTLK_INIT_INNER_STEPS_BEGIN(irbd_drv)
MTLK_INIT_STEPS_LIST_END(irbd_drv);

int __MTLK_IFUNC
mtlk_irbd_root_init (void)
{
  static const struct mtlk_cdev_cfg irdb_cdev_cfg = {
    .major     = 0,         /* Dynamic assignment */
    .max_nodes = 0,         /* Maximum supported  */
    .name      = "mtlk_irb",
  };
  struct mtlk_irbd_drv *drv_data = &mtlk_irbd_drv;

  MTLK_INIT_TRY(irbd_drv, MTLK_OBJ_PTR(drv_data))
    MTLK_INIT_STEP(irbd_drv, IRBD_CDEV, MTLK_OBJ_PTR(drv_data), 
                   mtlk_cdev_init, (&drv_data->cdev, &irdb_cdev_cfg));
    MTLK_INIT_STEP_EX(irbd_drv, IRBD_ROOT_IRBD_ALLOC, MTLK_OBJ_PTR(drv_data), 
                      mtlk_irbd_alloc, (), 
                      drv_data->root_irbd,
                      drv_data->root_irbd != NULL,
                      MTLK_ERR_NO_MEM);
    MTLK_INIT_STEP(irbd_drv, IRBD_ROOT_IRBD_INIT, MTLK_OBJ_PTR(drv_data), 
                   _mtlk_irbd_init, (drv_data->root_irbd, NULL, MTLK_IRB_ROOT_DESC));
    MTLK_INIT_FINALLY(irbd_drv, MTLK_OBJ_PTR(drv_data))    
  MTLK_INIT_RETURN(irbd_drv, MTLK_OBJ_PTR(drv_data), mtlk_irbd_root_cleanup, ())
}

void __MTLK_IFUNC 
mtlk_irbd_root_cleanup (void)
{
  struct mtlk_irbd_drv *drv_data = &mtlk_irbd_drv;

  MTLK_CLEANUP_BEGIN(irbd_drv, MTLK_OBJ_PTR(drv_data))
    MTLK_CLEANUP_STEP(irbd_drv, IRBD_ROOT_IRBD_INIT,  MTLK_OBJ_PTR(drv_data), 
                      mtlk_irbd_cleanup, (drv_data->root_irbd));
    MTLK_CLEANUP_STEP(irbd_drv, IRBD_ROOT_IRBD_ALLOC, MTLK_OBJ_PTR(drv_data), 
                      mtlk_irbd_free, (drv_data->root_irbd));
    MTLK_CLEANUP_STEP(irbd_drv, IRBD_CDEV, MTLK_OBJ_PTR(drv_data), 
                      mtlk_cdev_cleanup, (&drv_data->cdev));
  MTLK_CLEANUP_END(irbd_drv, MTLK_OBJ_PTR(drv_data))
}


MTLK_INIT_STEPS_LIST_BEGIN(irbd)
  MTLK_INIT_STEPS_LIST_ENTRY(irbd, IRBD_INIT_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(irbd, IRBD_DUP_DESC)
  MTLK_INIT_STEPS_LIST_ENTRY(irbd, IRBD_CREATE_CDEV_NODE)
  MTLK_INIT_STEPS_LIST_ENTRY(irbd, IRBD_INIT_HASH)
  MTLK_INIT_STEPS_LIST_ENTRY(irbd, IRBD_INIT_SONS_DB)
  MTLK_INIT_STEPS_LIST_ENTRY(irbd, IRBD_ADD_TO_PARENT_DB)
  MTLK_INIT_STEPS_LIST_ENTRY(irbd, IRBD_NOTIFY_APPEARANCE)
MTLK_INIT_INNER_STEPS_BEGIN(irbd)
MTLK_INIT_STEPS_LIST_END(irbd);

static int
_mtlk_irbd_init (mtlk_irbd_t *irbd,
                 mtlk_irbd_t *parent,
                 const char  *unique_desc)
{
  mtlk_node_type_e type  = parent?(parent->type + 1):MTLK_NT_ROOT;
  void            *buf   = NULL;
  int              len   = wave_strlen(unique_desc, MTLK_IRB_STR_MAX_LEN); /* unique_desc is NUL-terminated */

  MTLK_ASSERT(type < MTLK_NT_LAST);

  mtlk_osal_atomic_set(&irbd->notify_seq_no, 0);
  irbd->parent = parent;
  irbd->type   = type;

  MTLK_INIT_TRY(irbd, MTLK_OBJ_PTR(irbd))
    MTLK_INIT_STEP(irbd, IRBD_INIT_LOCK, MTLK_OBJ_PTR(irbd), 
                   mtlk_osal_lock_init, (&irbd->lock));
    MTLK_INIT_STEP_EX(irbd, IRBD_DUP_DESC, MTLK_OBJ_PTR(irbd), 
                      kmalloc_tag, (len + 1, GFP_KERNEL, MTLK_MEM_TAG_IRB_DESC),
                      buf,
                      buf != NULL,
                      MTLK_ERR_NO_MEM);
    irbd->unique_desc = (char *)buf;
    wave_strcopy(irbd->unique_desc, unique_desc, len + 1);

    MTLK_INIT_STEP_EX(irbd, IRBD_CREATE_CDEV_NODE, MTLK_OBJ_PTR(irbd), 
                      mtlk_cdev_node_create, (&mtlk_irbd_drv.cdev, _mtlk_irbd_ioctl_handler, HANDLE_T(irbd)),
                      irbd->cdev_node,
                      irbd->cdev_node != NULL,
                      MTLK_ERR_UNKNOWN);
    MTLK_INIT_STEP(irbd, IRBD_INIT_HASH, MTLK_OBJ_PTR(irbd), 
                   _mtlk_irb_hash_init, (&irbd->hash));
    MTLK_INIT_STEP_VOID(irbd, IRBD_INIT_SONS_DB, MTLK_OBJ_PTR(irbd), 
                        mtlk_dlist_init, (&irbd->sons));
    MTLK_INIT_STEP_VOID_IF((parent != NULL), irbd, IRBD_ADD_TO_PARENT_DB, MTLK_OBJ_PTR(irbd), 
                           _mtlk_irbd_add_son, (parent, irbd));
    MTLK_INIT_STEP_VOID(irbd, IRBD_NOTIFY_APPEARANCE, MTLK_OBJ_PTR(irbd), MTLK_NOACTION, ());
  MTLK_INIT_FINALLY(irbd,  MTLK_OBJ_PTR(irbd))
  MTLK_INIT_RETURN(irbd,  MTLK_OBJ_PTR(irbd), mtlk_irbd_cleanup, (irbd))
}

mtlk_irbd_t *__MTLK_IFUNC
mtlk_irbd_alloc (void)
{
  mtlk_irbd_t *irbd = kmalloc_tag(sizeof(*irbd), GFP_KERNEL, MTLK_MEM_TAG_IRB_OBJ);

  if (irbd) {
    memset(irbd, 0, sizeof(*irbd));
  }

  return irbd;
}

void __MTLK_IFUNC
mtlk_irbd_free (mtlk_irbd_t *irbd)
{
  MTLK_ASSERT(irbd != NULL);
  if (irbd)
    kfree_tag(irbd);
}

int __MTLK_IFUNC
mtlk_irbd_init (mtlk_irbd_t *irbd,
                mtlk_irbd_t *parent, 
                const char  *unique_desc)
{
  MTLK_ASSERT(irbd        != NULL);
  MTLK_ASSERT(parent      != NULL);
  MTLK_ASSERT(unique_desc != NULL);

  if (!irbd || !unique_desc)
    return MTLK_ERR_PARAMS;

  return _mtlk_irbd_init(irbd, parent, unique_desc);
}

/* WARNING: the function mtlk_irbd_cleanup() cannot be called from a spinlock context!!! */
void __MTLK_IFUNC
mtlk_irbd_cleanup (mtlk_irbd_t *irbd)
{
  MTLK_ASSERT(irbd != NULL);
  if (!irbd)
    return;

  MTLK_CLEANUP_BEGIN(irbd, MTLK_OBJ_PTR(irbd))
    MTLK_CLEANUP_STEP(irbd, IRBD_NOTIFY_APPEARANCE, MTLK_OBJ_PTR(irbd), 
                      mtlk_irbd_notify_app, (irbd, &MTLK_IRB_NODE_DISAPPEARS, NULL, 0));
    MTLK_CLEANUP_STEP(irbd, IRBD_ADD_TO_PARENT_DB, MTLK_OBJ_PTR(irbd), 
                      _mtlk_irbd_del_son, (irbd->parent, irbd))
    MTLK_CLEANUP_STEP(irbd, IRBD_INIT_SONS_DB, MTLK_OBJ_PTR(irbd), 
                      mtlk_dlist_cleanup, (&irbd->sons));
    MTLK_CLEANUP_STEP(irbd, IRBD_INIT_HASH, MTLK_OBJ_PTR(irbd), 
                      _mtlk_irb_hash_cleanup, (&irbd->hash));
    MTLK_CLEANUP_STEP(irbd, IRBD_CREATE_CDEV_NODE, MTLK_OBJ_PTR(irbd), 
                      mtlk_cdev_node_delete, (irbd->cdev_node));
    MTLK_CLEANUP_STEP(irbd, IRBD_DUP_DESC, MTLK_OBJ_PTR(irbd), 
                      kfree_tag, (irbd->unique_desc));
    MTLK_CLEANUP_STEP(irbd, IRBD_INIT_LOCK, MTLK_OBJ_PTR(irbd), 
                      mtlk_osal_lock_cleanup, (&irbd->lock));
  MTLK_CLEANUP_END(irbd, MTLK_OBJ_PTR(irbd))
}

mtlk_irbd_handle_t *__MTLK_IFUNC
mtlk_irbd_register(mtlk_irbd_t            *irbd,
                   const mtlk_guid_t      *evts,
                   uint32                  nof_evts,
                   mtlk_irbd_evt_handler_f handler,
                   mtlk_handle_t           context)
{
  mtlk_handle_t h;

  MTLK_ASSERT(irbd != NULL);
  if (!irbd)
    return NULL;

  h = _mtlk_irb_hash_register(&irbd->hash, evts, nof_evts, handler, context);

  return HANDLE_T_PTR(mtlk_irbd_handle_t, h);
}

void __MTLK_IFUNC
mtlk_irbd_unregister (mtlk_irbd_t        *irbd,
                      mtlk_irbd_handle_t *irbdh)
{
  MTLK_ASSERT(irbd != NULL);
  if (!irbd)
    return;

  _mtlk_irb_hash_unregister(&irbd->hash, HANDLE_T(irbdh));
}

int __MTLK_IFUNC
mtlk_irbd_notify_app (mtlk_irbd_t       *irbd,
                      const mtlk_guid_t *evt,
                      void              *buffer,
                      uint32             size)
{
  int                       res;
  uint8                    *msg_buf;
  struct mtlk_irb_ntfy_hdr *hdr;

  MTLK_ASSERT(irbd != NULL);
  MTLK_ASSERT(evt != NULL);
  MTLK_ASSERT((0 == size) || (NULL != buffer));
  if(!irbd || !evt || (!buffer && size))
    return MTLK_ERR_PARAMS;

  msg_buf = mtlk_osal_mem_alloc(sizeof(*hdr) + size, MTLK_MEM_TAG_IRBNOTIFY);

  if (NULL == msg_buf) {
    return MTLK_ERR_NO_MEM;
  }

  hdr = (struct mtlk_irb_ntfy_hdr *)msg_buf;
 
  hdr->evt         = *evt;
  hdr->cdev_minor  = mtlk_cdev_node_get_minor(irbd->cdev_node);
  hdr->data_size   = size;
  hdr->seq_no      = mtlk_osal_atomic_inc(&irbd->notify_seq_no);

  wave_memcpy(msg_buf + sizeof(*hdr), size, buffer, size);

  res = mtlk_nl_send_brd_msg(msg_buf, 
                             sizeof(*hdr) + size,
                             GFP_ATOMIC,
                             NETLINK_IRBM_GROUP, 
                             NL_DRV_IRBM_NOTIFY);

  mtlk_osal_mem_free(msg_buf);

  return res;
}

void __MTLK_IFUNC
_mtlk_irb_call_handler (mtlk_handle_t      evt_contex, 
                        void              *handler, 
                        mtlk_handle_t      handler_contex, 
                        const mtlk_guid_t *evt,
                        void              *buffer,
                        uint32            *size)
{
  mtlk_irbd_t             *irbd = HANDLE_T_PTR(mtlk_irbd_t, evt_contex);
  mtlk_irbd_evt_handler_f  h    = (mtlk_irbd_evt_handler_f)handler;

  MTLK_ASSERT(irbd != NULL);
  MTLK_ASSERT(h    != NULL);

  if (irbd && h) {
    h(irbd, handler_contex, evt, buffer, size);
  } else {
    if (size)
      *size = 0;
  }
}

static void 
_mtlk_irbd_print_irbd_topology (struct seq_file *s, mtlk_irbd_t *irbd)
{
  mtlk_dlist_entry_t *e, *head;

  seq_printf(s, "[%s]\n", irbd->unique_desc);
#ifdef CONFIG_WAVE_DEBUG
  seq_printf(s, MTLK_IRB_INI_PARENT "=%s\n", irbd->parent?irbd->parent->unique_desc:"");
#endif
  seq_printf(s, MTLK_IRB_INI_CDEV_MAJOR "=%d\n", mtlk_cdev_get_major(&mtlk_irbd_drv.cdev));
  seq_printf(s, MTLK_IRB_INI_CDEV_MINOR "=%d\n", mtlk_cdev_node_get_minor(irbd->cdev_node));

#ifdef CONFIG_WAVE_DEBUG
  {
    int i = 0;
    mtlk_dlist_foreach(&irbd->sons, e, head) {
      mtlk_irbd_t* son = MTLK_CONTAINER_OF(e, mtlk_irbd_t, lentry);
      seq_printf(s, MTLK_IRB_INI_SON_PREFIX"%d=%s\n", i, son->unique_desc);
      ++i;
    }
    seq_printf(s, MTLK_IRB_INI_NOF_SONS"=%d\n\n", i);
  }
#endif

  mtlk_dlist_foreach(&irbd->sons, e, head) {
    mtlk_irbd_t* son = MTLK_CONTAINER_OF(e, mtlk_irbd_t, lentry);

    _mtlk_irbd_print_irbd_topology(s, son);
  }
}

void __MTLK_IFUNC
mtlk_irbd_print_topology (struct seq_file *s)
{
  mtlk_irbd_t *irbd = mtlk_irbd_drv.root_irbd;

  if (!s)
    return;

  mtlk_osal_lock_acquire(&irbd->lock);
  _mtlk_irbd_print_irbd_topology(s, mtlk_irbd_drv.root_irbd);
  mtlk_osal_lock_release(&irbd->lock);
}

mtlk_irbd_t * __MTLK_IFUNC
mtlk_irbd_get_root (void)
{
  return mtlk_irbd_drv.root_irbd;
}

BOOL __MTLK_IFUNC
mtlk_irbd_hanlder_ctx_evt_valid (mtlk_irbd_t       *irbd,
                                 mtlk_handle_t      ctx,
                                 const mtlk_guid_t *evt,
                                 void              *buff,
                                 uint32            *size,
                                 const mtlk_guid_t *evt_exp,
                                 mtlk_slid_t        caller_slid)
{
  char *msg = NULL;

  if (!irbd) {
      msg = "IRB ptr is NULL";
  } else if (!ctx) {
      msg = "IRB ctx is 0";
  } else if (!size) {
      msg = "IRB size ptr is NULL";
  } else if (*size && !buff) {
      msg = "IRB buff is NULL while size is not 0";
  } else if (!evt) {
      msg = "IRB evt ptr is NULL";
  } else if (mtlk_guid_compare(evt, evt_exp)) {
      msg = "IRB event mismatch";
  } else {
    return TRUE;
  }

  mtlk_assert_log_s(caller_slid, msg);
  return FALSE;
}
EXPORT_SYMBOL(mtlk_irbd_hanlder_ctx_evt_valid);

EXPORT_SYMBOL(mtlk_irbd_notify_app);
EXPORT_SYMBOL(mtlk_irbd_register);
EXPORT_SYMBOL(mtlk_irbd_unregister);
EXPORT_SYMBOL(mtlk_irbd_print_topology);
EXPORT_SYMBOL(mtlk_irbd_get_root);
EXPORT_SYMBOL(mtlk_irbd_alloc);
EXPORT_SYMBOL(mtlk_irbd_init);
EXPORT_SYMBOL(mtlk_irbd_free);
EXPORT_SYMBOL(mtlk_irbd_cleanup);

