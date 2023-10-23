/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*! \file  mtlkcdev.c
    \brief Linux Character Device wrapper

    Kernel version independent Character Device API.
*/
#include "mtlkinc.h"
#include "mtlkcdev.h"

#include <linux/module.h>

#define LOG_LOCAL_GID   GID_MTLKCDEV
#define LOG_LOCAL_FID   1

/*****************************************************************
 * MTLK cdev subsystem init/cleanup (called once in driver)
 *****************************************************************/
struct mtlk_cdev_global_data
{
  mtlk_osal_spinlock_t lock;
  mtlk_dlist_t         dev_list;
  MTLK_DECLARE_INIT_STATUS;
};

static struct mtlk_cdev_global_data cdev_global_data;

MTLK_INIT_STEPS_LIST_BEGIN(cdev_global)
  MTLK_INIT_STEPS_LIST_ENTRY(cdev_global, CDEV_GLOBAL_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(cdev_global, CDEV_GLOBAL_LIST)
MTLK_INIT_INNER_STEPS_BEGIN(cdev_global)
MTLK_INIT_STEPS_LIST_END(cdev_global);

int __MTLK_IFUNC
mtlk_cdev_drv_init (void)
{
  struct mtlk_cdev_global_data *data = &cdev_global_data;

  MTLK_INIT_TRY(cdev_global, MTLK_OBJ_PTR(data))
    MTLK_INIT_STEP(cdev_global, CDEV_GLOBAL_LOCK, MTLK_OBJ_PTR(data), 
                   mtlk_osal_lock_init, (&data->lock));
    MTLK_INIT_STEP_VOID(cdev_global, CDEV_GLOBAL_LIST, MTLK_OBJ_PTR(data), 
                        mtlk_dlist_init, (&data->dev_list));
  MTLK_INIT_FINALLY(cdev_global, MTLK_OBJ_PTR(data))
  MTLK_INIT_RETURN(cdev_global, MTLK_OBJ_PTR(data), mtlk_cdev_drv_cleanup, ());
}

void __MTLK_IFUNC
mtlk_cdev_drv_cleanup (void)
{
  struct mtlk_cdev_global_data *data = &cdev_global_data;

  MTLK_CLEANUP_BEGIN(cdev_global, MTLK_OBJ_PTR(data))
    MTLK_CLEANUP_STEP(cdev_global, CDEV_GLOBAL_LIST, MTLK_OBJ_PTR(data), 
                      mtlk_dlist_cleanup, (&data->dev_list));
    MTLK_CLEANUP_STEP(cdev_global, CDEV_GLOBAL_LOCK, MTLK_OBJ_PTR(data), 
                      mtlk_osal_lock_cleanup, (&data->lock));
  MTLK_CLEANUP_END(cdev_global, MTLK_OBJ_PTR(data));
}

static void
_mtlk_cdev_drv_add_to_db (mtlk_dlist_entry_t *lentry)
{
  struct mtlk_cdev_global_data *data = &cdev_global_data;

  mtlk_osal_lock_acquire(&data->lock);
  mtlk_dlist_push_back(&data->dev_list, lentry);
  mtlk_osal_lock_release(&data->lock);
}

static void
_mtlk_cdev_drv_del_from_db (mtlk_dlist_entry_t *lentry)
{
  struct mtlk_cdev_global_data *data = &cdev_global_data;

  mtlk_osal_lock_acquire(&data->lock);
  mtlk_dlist_remove(&data->dev_list, lentry);
  mtlk_osal_lock_release(&data->lock);
}
/*****************************************************************/

#define MTLK_CDEV_MAX_NODES_SUPPORTED MINOR((dev_t)-1)

struct mtlk_cdev_node_private
{
  mtlk_dlist_entry_t        lentry;
  int                       minor;
  mtlk_cdev_t              *cd;
  mtlk_cdev_ioctl_handler_f ioctl_handler;
  mtlk_rmlock_t             ioctl_handler_rmlock;
  mtlk_osal_spinlock_t      ioctl_handler_lock;
  mtlk_atomic_t             node_ref_count;
  mtlk_handle_t             ctx;
};

MTLK_INIT_STEPS_LIST_BEGIN(cdev)
  MTLK_INIT_STEPS_LIST_ENTRY(cdev, CDEV_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(cdev, CDEV_REGION)
  MTLK_INIT_STEPS_LIST_ENTRY(cdev, CDEV_ADD)
  MTLK_INIT_STEPS_LIST_ENTRY(cdev, CDEV_NODES_LIST)
  MTLK_INIT_STEPS_LIST_ENTRY(cdev, CDEV_ADD_TO_DB)
MTLK_INIT_INNER_STEPS_BEGIN(cdev)
MTLK_INIT_STEPS_LIST_END(cdev);

static struct file_operations _mtlk_cdev_fops;

static void
_mtlk_cdev_node_addref (mtlk_cdev_node_t *node)
{
  mtlk_osal_atomic_inc(&node->node_ref_count);
}

static void
_mtlk_cdev_node_release (mtlk_cdev_node_t *node)
{
  if (!node)
    return;

  if(0 == mtlk_osal_atomic_dec(&node->node_ref_count)) {
    mtlk_osal_lock_cleanup(&node->ioctl_handler_lock);
    mtlk_rmlock_cleanup(&node->ioctl_handler_rmlock);
    kfree_tag(node);
  }
}

static mtlk_error_t
_mtlk_cdev_prepare_region (mtlk_cdev_t *cd)
{
  dev_t         dev = MKDEV(cd->cfg.major, 0);
  mtlk_error_t  res = MTLK_ERR_UNKNOWN;
  int           err;

  if (cd->cfg.major) {
    /* Major number was specified on insmod */
    err = register_chrdev_region(dev, cd->cfg.max_nodes, cd->cfg.name);
    if (err < 0) {
      ELOG_DD("Can't register chrdev region (major=%d, err=%d)", cd->cfg.major, err);
      res = MTLK_ERR_NO_RESOURCES;
      goto end;
    }
  } else {
    /* Allocate dynamically */
    err = alloc_chrdev_region(&dev, 0, cd->cfg.max_nodes, cd->cfg.name);
    if (err < 0) {
      ELOG_D("Can't allocate chrdev region (err=%d)", err);
      res = MTLK_ERR_NO_RESOURCES;
      goto end;
    }

    cd->cfg.major = MAJOR(dev);
  }

  res = MTLK_ERR_OK;

end:
  return res;
}

static mtlk_error_t
_mtlk_cdev_add (mtlk_cdev_t *cd)
{
  int res;

  cdev_init(&cd->cdev, &_mtlk_cdev_fops);
  cd->cdev.owner = THIS_MODULE;
  res = cdev_add(&cd->cdev, MKDEV(cd->cfg.major, 0), cd->cfg.max_nodes);
  if (res != 0) {
    ELOG_DD("Can't add cdev (major=%d, err=%d)", cd->cfg.major, res);
    return MTLK_ERR_NO_RESOURCES;
  }

  return MTLK_ERR_OK;
}

/*! \brief  Retrieve character device's node handle by minor number

  \param cd     Handle to the character device
  \param minor  Minor number of node to search

  \return
    Handle to the node or NULL on failure

  \remark
    character device handle must be protected

  \code
    mtlk_osal_lock_acquire(&cd->lock);
    node = _cdev_node_get(cd, minor);
    mtlk_osal_lock_release(&cd->lock);
  \endcode
 */
static mtlk_cdev_node_t*
_cdev_node_get (mtlk_cdev_t *cd, int minor)
{
  mtlk_cdev_node_t *node;
  mtlk_dlist_entry_t *head;
  mtlk_dlist_entry_t *entry;

  mtlk_dlist_foreach(&cd->nodes_list, entry, head) {
    node = MTLK_CONTAINER_OF(entry, mtlk_cdev_node_t, lentry);
    if (minor == node->minor)
      return node;
  }
  return NULL;
}

/*! \brief File operations \b open method for a character device
 *
 *  \param   inode   Pointer to the \c inode corresponding to the device
 *  \param   filp    Pointer to the \c file corresponding to the device
 *
 *  \return  0 on success, an error code on failure
 */
static int
_mtlk_cdev_open (struct inode *inode, struct file *filp)
{
  int major;
  int minor;
  mtlk_dlist_entry_t *head;
  mtlk_dlist_entry_t *entry;
  mtlk_cdev_t        *cd  = NULL;
  mtlk_cdev_node_t   *node = NULL;

  if (!inode || !filp)
    return -ENODEV;

  major = imajor(inode);
  minor = iminor(inode);

  ILOG2_DD("Open %d:%d!", major, minor);
      
  mtlk_osal_lock_acquire(&cdev_global_data.lock);
  mtlk_dlist_foreach(&cdev_global_data.dev_list, entry, head) {
    cd = MTLK_CONTAINER_OF(entry, mtlk_cdev_t, lentry);
    if (cd && (major == cd->cfg.major))
      break;
    cd = NULL;
  }
  mtlk_osal_lock_release(&cdev_global_data.lock);

  MTLK_ASSERT(cd != NULL);
  if (!cd)
    return -EINVAL;

  mtlk_osal_lock_acquire(&cd->lock);
  node = _cdev_node_get(cd, minor);
  mtlk_osal_lock_release(&cd->lock);

  if (!node) {
    return -EINVAL;
  }

  _mtlk_cdev_node_addref(node);
  filp->private_data = node;
  return 0;
}

/*! \brief  File operations \b release method for a character device
 *
 *  \param   inode   Pointer to the \c inode corresponding to the device
 *  \param   filp    Pointer to the \c file corresponding to the device
 *
 *  \return  0
 */
static int
_mtlk_cdev_release (struct inode *inode, struct file *filp)
{
  MTLK_ASSERT(NULL != filp);
  MTLK_ASSERT(NULL != filp->private_data);

  if (filp && filp->private_data) {
    _mtlk_cdev_node_release((mtlk_cdev_node_t *) filp->private_data);
  }

  return 0;
}

static long
_mtlk_cdev_invoke_safe (mtlk_cdev_node_t *node,
                        unsigned int     cmd,
                        unsigned long    arg)
{
  long res = -ENODEV;
  mtlk_cdev_ioctl_handler_f handler = NULL;

  mtlk_osal_lock_acquire(&node->ioctl_handler_lock);

  if(NULL != node->ioctl_handler) {
    mtlk_rmlock_acquire(&node->ioctl_handler_rmlock);
    handler = node->ioctl_handler;
  }

  mtlk_osal_lock_release(&node->ioctl_handler_lock);

  if(NULL != handler) {
    res = handler(node->ctx, cmd, arg);
    mtlk_rmlock_release(&node->ioctl_handler_rmlock);
  }

  return res;
}

/*! \brief  File operations \b unlocked_ioctl method for a character device
 *
 *  \param   filp    Pointer to the \c file corresponding to the device
 *  \param   cmd     IOCTL command to handle
 *  \param   arg     Data passed to the handler
 *
 *  \return  0 on success, an error code on failure
 */
static long 
_mtlk_cdev_unlocked_ioctl (struct file  *filp, 
                           unsigned int  cmd, 
                           unsigned long arg)
{
  mtlk_cdev_node_t *node = NULL;
  long res = -ENODEV;

  MTLK_ASSERT(filp != NULL);
  if (filp)
    node = filp->private_data;

  MTLK_ASSERT(node != NULL);
  if (node)
    res = _mtlk_cdev_invoke_safe(node, cmd, arg);

  return res;
}

static struct file_operations _mtlk_cdev_fops =
{
  .owner          = THIS_MODULE,
  .llseek         = no_llseek,
  .open           = _mtlk_cdev_open,
  .release        = _mtlk_cdev_release,

  .unlocked_ioctl = _mtlk_cdev_unlocked_ioctl,
};

mtlk_error_t __MTLK_IFUNC
mtlk_cdev_init (mtlk_cdev_t *cd, const struct mtlk_cdev_cfg *cfg)
{
  MTLK_ASSERT(cd != NULL);
  MTLK_ASSERT(cfg != NULL);
  if (!cd || !cfg)
    return MTLK_ERR_UNKNOWN;

  cd->cfg = *cfg;
  if (!cd->cfg.max_nodes) {
    ILOG0_D("Max nodes set to %d", MTLK_CDEV_MAX_NODES_SUPPORTED);
    cd->cfg.max_nodes = MTLK_CDEV_MAX_NODES_SUPPORTED;
  }
  else if (cd->cfg.max_nodes > MTLK_CDEV_MAX_NODES_SUPPORTED) {
    MTLK_ASSERT(!"Too many nodes configured");
    cd->cfg.max_nodes = MTLK_CDEV_MAX_NODES_SUPPORTED;
  }

  MTLK_INIT_TRY(cdev, MTLK_OBJ_PTR(cd))
    MTLK_INIT_STEP(cdev, CDEV_LOCK, MTLK_OBJ_PTR(cd), 
                   mtlk_osal_lock_init, (&cd->lock));
    MTLK_INIT_STEP(cdev, CDEV_REGION, MTLK_OBJ_PTR(cd), 
                   _mtlk_cdev_prepare_region, (cd));
    MTLK_INIT_STEP(cdev, CDEV_ADD, MTLK_OBJ_PTR(cd), 
                   _mtlk_cdev_add, (cd));
    MTLK_INIT_STEP_VOID(cdev, CDEV_NODES_LIST, MTLK_OBJ_PTR(cd), 
                        mtlk_dlist_init, (&cd->nodes_list));
    MTLK_INIT_STEP_VOID(cdev, CDEV_ADD_TO_DB, MTLK_OBJ_PTR(cd),
                        _mtlk_cdev_drv_add_to_db, (&cd->lentry));
  MTLK_INIT_FINALLY(cdev, MTLK_OBJ_PTR(cd))
  MTLK_INIT_RETURN(cdev, MTLK_OBJ_PTR(cd), mtlk_cdev_cleanup, (cd))
}

int __MTLK_IFUNC
mtlk_cdev_get_major (mtlk_cdev_t *cd)
{
  MTLK_ASSERT(cd != NULL);
  if (!cd)
    return 0;

  return cd->cfg.major;
}

void __MTLK_IFUNC
mtlk_cdev_cleanup (mtlk_cdev_t *cd)
{
  if (!cd)
    return;

  MTLK_CLEANUP_BEGIN(cdev, MTLK_OBJ_PTR(cd))
    MTLK_CLEANUP_STEP(cdev, CDEV_ADD_TO_DB, MTLK_OBJ_PTR(cd),
                      _mtlk_cdev_drv_del_from_db, (&cd->lentry));
    MTLK_CLEANUP_STEP(cdev, CDEV_NODES_LIST, MTLK_OBJ_PTR(cd),
                      mtlk_dlist_cleanup, (&cd->nodes_list));
    MTLK_CLEANUP_STEP(cdev, CDEV_ADD, MTLK_OBJ_PTR(cd), 
                      cdev_del, (&cd->cdev));
    MTLK_CLEANUP_STEP(cdev, CDEV_REGION, MTLK_OBJ_PTR(cd), 
                      unregister_chrdev_region, (MKDEV(cd->cfg.major, 0), cd->cfg.max_nodes));
    MTLK_CLEANUP_STEP(cdev, CDEV_LOCK, MTLK_OBJ_PTR(cd), 
                      mtlk_osal_lock_cleanup, (&cd->lock));
  MTLK_CLEANUP_END(cdev, MTLK_OBJ_PTR(cd))
}

mtlk_cdev_node_t *__MTLK_IFUNC
mtlk_cdev_node_create (mtlk_cdev_t              *cd, 
                       mtlk_cdev_ioctl_handler_f ioctl_handler, 
                       mtlk_handle_t             ctx)
{
  mtlk_cdev_node_t *node;
  mtlk_error_t      err = MTLK_ERR_OK;

  MTLK_ASSERT(cd != NULL);
  MTLK_ASSERT(ioctl_handler != NULL);
  if (!cd || !ioctl_handler)
    return NULL;

  node = kmalloc_tag(sizeof(*node), GFP_KERNEL, MTLK_MEM_TAG_CDEV);
  if (!node) {
    ELOG_V("Can't allocate NODE");
    return NULL;
  }

  node->cd            = cd;
  node->ioctl_handler = ioctl_handler;
  node->ctx           = ctx;

  if(MTLK_ERR_OK != mtlk_osal_lock_init(&node->ioctl_handler_lock)) {
    ELOG_V("Can't initialize NODE lock");
    kfree_tag(node);
    return NULL;
  }

  if(MTLK_ERR_OK != mtlk_rmlock_init(&node->ioctl_handler_rmlock)) {
    ELOG_V("Can't initialize NODE rmlock");
    mtlk_osal_lock_cleanup(&node->ioctl_handler_lock);
    kfree_tag(node);
    return NULL;
  }

  mtlk_rmlock_acquire(&node->ioctl_handler_rmlock);

  /* protect character device */
  mtlk_osal_lock_acquire(&cd->lock);
  do { /* try */
    if (mtlk_dlist_size(&cd->nodes_list) >= cd->cfg.max_nodes) {
      ELOG_D("Can't add node - MAX nodes limit reached (%u)", cd->cfg.max_nodes);
      err = MTLK_ERR_NO_RESOURCES;
      break; /* throw */
    }

    /* find first unused minor number for new node */
    for (node->minor = 0; node->minor < (int)cd->cfg.max_nodes; node->minor++) {
      if (NULL == _cdev_node_get(cd, node->minor))
        break;
    }

    if (node->minor >= (int)cd->cfg.max_nodes) {
      ELOG_V("No any free minor number available");
      err = MTLK_ERR_NO_RESOURCES;
      break; /* throw */
    }

    mtlk_dlist_push_back(&cd->nodes_list, &node->lentry);
  } while(0);
  /* finally */
  mtlk_osal_lock_release(&cd->lock);

  /* catch */
  if (MTLK_ERR_OK != err) {
    mtlk_osal_lock_cleanup(&node->ioctl_handler_lock);
    mtlk_rmlock_cleanup(&node->ioctl_handler_rmlock);
    kfree_tag(node);
    return NULL;
  }

  /* Creator owns the initial reference to the node */
  mtlk_osal_atomic_set(&node->node_ref_count, 1);

  return node;
}

int __MTLK_IFUNC
mtlk_cdev_node_get_minor (mtlk_cdev_node_t *node)
{
  MTLK_ASSERT(node != NULL);
  MTLK_ASSERT(node->cd != NULL);
  if (!node || !node->cd)
    return 0;

  return node->minor;
}

void __MTLK_IFUNC
mtlk_cdev_node_delete (mtlk_cdev_node_t *node)
{
  MTLK_ASSERT(node != NULL);
  MTLK_ASSERT(node->cd != NULL);
  if (!node || !node->cd)
    return;

  /* Make sure no new open calls will arrive */
  mtlk_osal_lock_acquire(&node->cd->lock);
  mtlk_dlist_remove(&node->cd->nodes_list, &node->lentry);
  mtlk_osal_lock_release(&node->cd->lock);

  /* Make sure ioctl handler will not be invoked anymore */
  mtlk_osal_lock_acquire(&node->ioctl_handler_lock);
  node->ioctl_handler = NULL;
  mtlk_osal_lock_release(&node->ioctl_handler_lock);

  /* Wait for current threads executing ioctl handler to leave it */
  mtlk_rmlock_release(&node->ioctl_handler_rmlock);
  mtlk_rmlock_wait(&node->ioctl_handler_rmlock);

  /* Dereference node memory. It will be freed when last reference gone */
  _mtlk_cdev_node_release(node);
}
