/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 *
 *
 * Written by: Andriy Fidrya
 *
 */
#include "mtlkinc.h"

#include <linux/module.h>
#include <linux/poll.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/cdev.h>
#define mtlk_char_dev_t struct cdev
#include <linux/proc_fs.h>

#include "mtlkerr.h"

#include "log_drv.h"
#include "compat.h"
#include "mtlk_rtlog.h"

#define IWLWAV_RTLOG_DONT_USE_USP   /* FIXME: temp fix: remove forwarding via usp_entry */

#define PROC_NET init_net.proc_net

#define proc_entry_log "mtlk_log"
#define proc_entry_debug "debug"
#define proc_entry_rtlog "rtlog"
#define proc_entry_statistics "statistics"

#define chrdev_mtlk_log proc_entry_log

#define LOG_LOCAL_GID   GID_LOGDRV
#define LOG_LOCAL_FID   2

int debug = 0;

extern mtlk_log_data_t log_data;

typedef struct _mtlk_cdev_t {
  int initialized;
  int chrdev_region_allocated;
  wait_queue_head_t in_queue;
  wait_queue_head_t out_queue;
  struct fasync_struct *async_queue;
  mtlk_char_dev_t cdev;
} mtlk_cdev_t;

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static int mtlk_major = 0;
#endif

// Buffer size in bytes
#define LOG_BUF_SIZE    1400

// Number of buffers to allocate for Driver, Upper and Lower MACs
#define LOG_BUF_COUNT   64

// Log buffer swap timeout in msec
#define LOG_BUF_SWAP_TIMEOUT 1000

int rdebug         = LOG_DEFAULT_RDEBUG;
int cdebug         = LOG_DEFAULT_CDEBUG;
int bufs_no        = LOG_BUF_COUNT;
int buf_size       = LOG_BUF_SIZE;
int buf_swap_tmout = LOG_BUF_SWAP_TIMEOUT;

// -------------
// Configuration
// -------------

#define MAX_LOG_STRING_LENGTH 1024

// ----------------
// Type definitions
// ----------------

#define USP_ENTRY_CDATA     1
#define USP_ENTRY_READY_BUF 2

#ifdef CONFIG_WAVE_RTLOG_REMOTE
typedef struct _mtlk_usp_entry_t
{
  mtlk_ldlist_entry_t entry; // List entry data
  int type;
  int len; // data length
  int pos; // current read position
  union {
    uint8 *pdata;
    mtlk_log_buf_entry_t *pbuf;
  } u;
} mtlk_usp_entry_t;
#endif

typedef struct _mtlk_log_osdep_data_t
{
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  // Uses locking list implementation, so requires no additional locking
  mtlk_ldlist_t usp_queue; // Userspace data queue

  wait_queue_head_t in_wait_queue;
  
  struct proc_dir_entry *prtlog_entry;
  struct proc_dir_entry *pstat_entry;
#endif
  struct proc_dir_entry *pnet_procfs_dir;
  struct proc_dir_entry *pdebug_entry;

  MTLK_DECLARE_INIT_STATUS;
} mtlk_log_osdep_data_t;

// --------------------
// Forward declarations
// --------------------

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static void rtlog_netdev_cleanup(void);
static int  rtlog_netdev_init(void);

static int cdev_log_open(struct inode *inode, struct file *filp);
static int cdev_log_release(struct inode *inode, struct file *filp);
static ssize_t cdev_log_read(struct file *filp, char __user *buf,
    size_t count, loff_t *f_pos);
static ssize_t cdev_log_write(struct file *filp, const char __user *buf,
    size_t count, loff_t *f_pos);
static unsigned int cdev_log_poll(struct file *filp, poll_table *wait);
static int cdev_log_fasync(int fd, struct file *filp, int mode);
static long cdev_unlocked_ioctl(struct file  *filp,
                                unsigned int  cmd,
                                unsigned long arg);

#ifndef IWLWAV_RTLOG_DONT_USE_USP
static mtlk_usp_entry_t *alloc_usp_entry(void);
#endif

static void release_usp_entry(mtlk_usp_entry_t *usp_entry);
static void usp_queue_release_entries(void);
static int setup_cdev(void);
static int logger_stat_open(struct inode *inode, struct file *file);
#endif

static ssize_t logger_debug_write(struct file *file, const char __user *buffer,
                                  size_t count, loff_t *ppos);
static int logger_debug_open(struct inode *inode, struct file *file);

// ----------------
// Global variables
// ----------------

mtlk_log_osdep_data_t log_data_osdep;

#ifdef CONFIG_WAVE_RTLOG_REMOTE
// Log device can be opened by only one process at a time
static atomic_t log_dev_available = ATOMIC_INIT(1);

mtlk_cdev_t mtlk_log_dev;

struct file_operations mtlk_log_fops =
{
  .owner =     THIS_MODULE,
  .llseek =    no_llseek,
  .open =      cdev_log_open,
  .release =   cdev_log_release,
  .read =      cdev_log_read,
  .write =     cdev_log_write,
  .poll =      cdev_log_poll,
  .fasync =    cdev_log_fasync,
  .unlocked_ioctl = cdev_unlocked_ioctl,
};

// -------------------
// Interface functions
// -------------------

static int
log_cdev_init(void)
{
  int res;
  dev_t dev = MKDEV(mtlk_major, 0);

  memset(&mtlk_log_dev, 0, sizeof(mtlk_log_dev));
  if (mtlk_major) {
    // Major number was specified on insmod
    res = register_chrdev_region(dev, 1, chrdev_mtlk_log);
    if (res < 0)
      return MTLK_ERR_NO_RESOURCES;
  } else {
    // Allocate dynamically
    res = alloc_chrdev_region(&dev, 0, 1, chrdev_mtlk_log);
    if (res < 0)
      return MTLK_ERR_NO_RESOURCES;
    mtlk_major = MAJOR(dev);
  }

  mtlk_log_dev.chrdev_region_allocated = 1;

  if (setup_cdev() < 0)
    return MTLK_ERR_NO_RESOURCES;
  return MTLK_ERR_OK;
}

static void
log_cdev_cleanup(void)
{
  if (!mtlk_log_dev.chrdev_region_allocated)
    return;

  if (mtlk_log_dev.initialized)
    cdev_del(&mtlk_log_dev.cdev);
  unregister_chrdev_region(MKDEV(mtlk_major, 0), 1);

  mtlk_major = 0;
}

static void
log_rtlog_cleanup(void)
{
    remove_proc_entry(proc_entry_rtlog, log_data_osdep.pnet_procfs_dir);
    rtlog_before_cleanup();
    rtlog_netdev_cleanup();
    rtlog_on_cleanup();
}
#endif

MTLK_INIT_STEPS_LIST_BEGIN(logdrv_osdep)
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  MTLK_INIT_STEPS_LIST_ENTRY(logdrv_osdep, LOGDRV_USP_QUEUE)
  MTLK_INIT_STEPS_LIST_ENTRY(logdrv_osdep, LOGDRV_USP_QUEUE_ENTRIES)
  MTLK_INIT_STEPS_LIST_ENTRY(logdrv_osdep, LOGDRV_WAIT_QUEUE)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(logdrv_osdep, LOGDRV_PROC_DIR)
  MTLK_INIT_STEPS_LIST_ENTRY(logdrv_osdep, LOGDRV_PROC_DEBUG)
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  MTLK_INIT_STEPS_LIST_ENTRY(logdrv_osdep, LOGDRV_PROC_STAT)
  MTLK_INIT_STEPS_LIST_ENTRY(logdrv_osdep, LOGDRV_CDEV_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(logdrv_osdep, LOGDRV_RTLOG_INIT)
#endif
MTLK_INIT_INNER_STEPS_BEGIN(logdrv_osdep)
MTLK_INIT_STEPS_LIST_END(logdrv_osdep);

void __MTLK_IFUNC
mtlk_log_on_cleanup(void)
{
  MTLK_CLEANUP_BEGIN(logdrv_osdep, MTLK_OBJ_PTR(&log_data_osdep))
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_CLEANUP_STEP(logdrv_osdep, LOGDRV_RTLOG_INIT, MTLK_OBJ_PTR(&log_data_osdep),
                      log_rtlog_cleanup, ());
    MTLK_CLEANUP_STEP(logdrv_osdep, LOGDRV_CDEV_INIT, MTLK_OBJ_PTR(&log_data_osdep),
                      log_cdev_cleanup, ());
    MTLK_CLEANUP_STEP(logdrv_osdep, LOGDRV_PROC_STAT, MTLK_OBJ_PTR(&log_data_osdep),
                      remove_proc_entry, (proc_entry_statistics, log_data_osdep.pnet_procfs_dir));
#endif
    MTLK_CLEANUP_STEP(logdrv_osdep, LOGDRV_PROC_DEBUG, MTLK_OBJ_PTR(&log_data_osdep),
                      remove_proc_entry, (proc_entry_debug, log_data_osdep.pnet_procfs_dir));
    MTLK_CLEANUP_STEP(logdrv_osdep, LOGDRV_PROC_DIR, MTLK_OBJ_PTR(&log_data_osdep),
                      remove_proc_entry, (proc_entry_log, PROC_NET));
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_CLEANUP_STEP(logdrv_osdep, LOGDRV_WAIT_QUEUE, MTLK_OBJ_PTR(&log_data_osdep),
                      MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(logdrv_osdep, LOGDRV_USP_QUEUE_ENTRIES, MTLK_OBJ_PTR(&log_data_osdep),
                      usp_queue_release_entries, ());
    MTLK_CLEANUP_STEP(logdrv_osdep, LOGDRV_USP_QUEUE, MTLK_OBJ_PTR(&log_data_osdep),
                      mtlk_ldlist_cleanup, (&log_data_osdep.usp_queue));
#endif
  MTLK_CLEANUP_END(logdrv_osdep, MTLK_OBJ_PTR(&log_data_osdep));
}

static const struct file_operations logger_debug_fops = {
  .owner      = THIS_MODULE,
  .open       = logger_debug_open,
  .read       = seq_read,
  .llseek     = seq_lseek,
  .release    = single_release,
  .write      = logger_debug_write
};

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static struct file_operations logger_stat_fops = {
  .owner      = THIS_MODULE,
  .open       = logger_stat_open,
  .read       = seq_read,
  .llseek     = seq_lseek,
  .release    = single_release,
};

/* --- HW RT-Logger --------------------------------------------- */

static int
show_logger_rtlog(struct seq_file *seq, void *v)
{
    mtlk_log_get_rtlog(seq);
    return 0;
}

static int logger_rtlog_open(struct inode *inode, struct file *file)
{
  return single_open(file, show_logger_rtlog, NULL);
}

static ssize_t logger_rtlog_write(struct file *file, const char __user *buffer,
                                  size_t count, loff_t *ppos)
{
  char *debug_conf = NULL;
  int result = 0;

  debug_conf = kmalloc(count + 1, GFP_KERNEL);
  if (!debug_conf) {
    ELOG_D("Unable to allocate %lu bytes", count);
    result = -EFAULT;
    goto FINISH;
  }
  if (0 != (copy_from_user(debug_conf, buffer, count))) {
    result = -EFAULT;
    goto FINISH;
  }
  debug_conf[count] = '\0';
  ILOG1_S("Received command: %s", debug_conf);
  mtlk_log_set_rtlog(debug_conf);
  result = count;
FINISH:
  if (debug_conf)
    kfree(debug_conf);
  return result;
}

static const struct file_operations logger_rtlog_fops = {
  .owner      = THIS_MODULE,
  .open       = logger_rtlog_open,
  .read       = seq_read,
  .llseek     = seq_lseek,
  .release    = single_release,
  .write      = logger_rtlog_write
};

static int
log_rtlog_init(void)
{
    int res = MTLK_ERR_OK;

    /* /proc entry */
    log_data_osdep.prtlog_entry =
        proc_create(proc_entry_rtlog, S_IFREG|S_IRUSR|S_IRGRP, log_data_osdep.pnet_procfs_dir, &logger_rtlog_fops);

    if (NULL == log_data_osdep.prtlog_entry) {
        res = MTLK_ERR_NO_RESOURCES;
    }
    else {
        res = rtlog_netdev_init();
        if (res == MTLK_ERR_OK) {
            res = rtlog_on_init();
        }
    }

    return res;
}
#endif

int __MTLK_IFUNC
mtlk_log_on_init(void)
{
  memset(&log_data_osdep, 0, sizeof(log_data_osdep));

  MTLK_INIT_TRY(logdrv_osdep, MTLK_OBJ_PTR(&log_data_osdep))
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_INIT_STEP_VOID(logdrv_osdep, LOGDRV_USP_QUEUE, MTLK_OBJ_PTR(&log_data_osdep),
                        mtlk_ldlist_init, (&log_data_osdep.usp_queue));

    MTLK_INIT_STEP_VOID(logdrv_osdep, LOGDRV_USP_QUEUE_ENTRIES, MTLK_OBJ_PTR(&log_data_osdep),
                        MTLK_NOACTION, ());

    MTLK_INIT_STEP_VOID(logdrv_osdep, LOGDRV_WAIT_QUEUE, MTLK_OBJ_PTR(&log_data_osdep),
                        init_waitqueue_head, (&log_data_osdep.in_wait_queue));
#endif
    MTLK_INIT_STEP_EX(logdrv_osdep, LOGDRV_PROC_DIR, MTLK_OBJ_PTR(&log_data_osdep),
                      proc_mkdir, (proc_entry_log, PROC_NET), log_data_osdep.pnet_procfs_dir,
                      NULL != log_data_osdep.pnet_procfs_dir, MTLK_ERR_NO_RESOURCES);

    MTLK_INIT_STEP_EX(logdrv_osdep, LOGDRV_PROC_DEBUG, MTLK_OBJ_PTR(&log_data_osdep),
                      proc_create, (proc_entry_debug, S_IFREG|S_IRUSR|S_IRGRP, log_data_osdep.pnet_procfs_dir, &logger_debug_fops),
                      log_data_osdep.pdebug_entry, NULL != log_data_osdep.pdebug_entry,
                      MTLK_ERR_NO_RESOURCES);
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_INIT_STEP_EX(logdrv_osdep, LOGDRV_PROC_STAT, MTLK_OBJ_PTR(&log_data_osdep),
                      proc_create, (proc_entry_statistics, S_IFREG|S_IRUSR|S_IRGRP, log_data_osdep.pnet_procfs_dir, &logger_stat_fops),
                      log_data_osdep.pstat_entry, NULL != log_data_osdep.pstat_entry,
                      MTLK_ERR_NO_RESOURCES);

    MTLK_INIT_STEP_VOID(logdrv_osdep, LOGDRV_CDEV_INIT, MTLK_OBJ_PTR(&log_data_osdep),
                        log_cdev_init, ());

    MTLK_INIT_STEP(logdrv_osdep, LOGDRV_RTLOG_INIT, MTLK_OBJ_PTR(&log_data_osdep),
                   log_rtlog_init, ());
#endif
    MTLK_INIT_FINALLY(logdrv_osdep, MTLK_OBJ_PTR(&log_data_osdep))
    MTLK_INIT_RETURN(logdrv_osdep, MTLK_OBJ_PTR(&log_data_osdep), mtlk_log_on_cleanup, ());
}

#ifdef CONFIG_WAVE_RTLOG_REMOTE
int __MTLK_IFUNC
mtlk_log_on_buf_ready(void)
{
  mtlk_log_buf_entry_t *ready_buf;
  mtlk_usp_entry_t *usp_entry;
  int num_bufs_processed;

  num_bufs_processed = 0;
  while ((ready_buf = mtlk_log_get_ready_buf()) != NULL) {

    mtlk_ndev_send_log_packet(_mtlk_log_buf_get_data_buffer(ready_buf), ready_buf->data_size); /* RT Logger */

#ifdef IWLWAV_RTLOG_DONT_USE_USP
    usp_entry = NULL;
    mtlk_log_put_free_buf(ready_buf);   /* just free buf */

#else
    usp_entry = alloc_usp_entry();
    if (!usp_entry) {
      mtlk_log_put_free_buf(ready_buf);
      ELOG_V("Out of memory: log data lost");
      return MTLK_ERR_NO_MEM;
    }
    usp_entry->type = USP_ENTRY_READY_BUF;
    usp_entry->len = ready_buf->data_size;
    usp_entry->pos = 0;
    usp_entry->u.pbuf = ready_buf;
    mtlk_ldlist_push_back(&log_data_osdep.usp_queue,
        (mtlk_ldlist_entry_t *) usp_entry);
    ++num_bufs_processed;

#endif /* IWLWAV_RTLOG_DONT_USE_USP */
  }

  // This check is neccessary because other thread could already process
  // the buffer this notification arrived for
  if (num_bufs_processed > 0)
    wake_up_interruptible(&log_data_osdep.in_wait_queue);
  return MTLK_ERR_OK;
}

// ---------------
// Local functions
// ---------------

static int
setup_cdev(void)
{
  int res;
  int devno = MKDEV(mtlk_major, 0);

  init_waitqueue_head(&mtlk_log_dev.in_queue);
  init_waitqueue_head(&mtlk_log_dev.out_queue);

  cdev_init(&mtlk_log_dev.cdev, &mtlk_log_fops);
  mtlk_log_dev.cdev.owner = THIS_MODULE;
  res = cdev_add(&mtlk_log_dev.cdev, devno, 1);
  if (res != 0) {
    ELOG_D("Can't initialize mtlk character device: error %d", res);
    return -1;
  }

  mtlk_log_dev.initialized = 1;
  return 0;
}

static int
cdev_log_open(struct inode *inode, struct file *filp)
{
  int ret;
  unsigned int minor = MINOR(inode->i_rdev);

  if (minor)
    return -ENOTSUPP;

  // Disallow multiple open
  if (!atomic_dec_and_test(&log_dev_available)) {
    atomic_inc(&log_dev_available);
    return -EBUSY;
  }

  // FIXME: this is a workaround: see cdev_log_release for problem description
  usp_queue_release_entries();

  // If there are events accumulated in ready queue, send them to user
  // TODO: implement old ready buffers dropping in get_free_buf (overflow)
  ret = mtlk_log_on_buf_ready(); // check ready queue
  if (ret != MTLK_ERR_OK)
    return (ret == MTLK_ERR_NO_MEM) ? -ENOMEM : -EFAULT;

  filp->private_data = &mtlk_log_dev;

  ret = nonseekable_open(inode, filp);
  if (ret < 0)
    return ret;

  return 0;
}

static int
cdev_log_release(struct inode *inode, struct file *filp)
{
  //mtlk_cdev_t *dev = filp->private_data;

  // Remove filp from asynchronous notifications list
  cdev_log_fasync(-1, filp, 0);

  // FIXME: on_ready could potentially insert a new buffer entry to usp_queue
  // before device is released: change ldlist to dlist and add manual locking.
  // Currently as workaround we're calling usp_queue_release_entries
  // on cdev_log_open to eliminate this buffer entry.
  usp_queue_release_entries();

  // Allow other processes to open the device
  atomic_inc(&log_dev_available);

  return 0;
}

static ssize_t
cdev_log_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
  mtlk_usp_entry_t *usp_entry;
  long to_send;
  long not_copied;
  ssize_t written = 0;
  uint8 *copy_src;

next_entry:
  while (!(usp_entry = (mtlk_usp_entry_t *)
        mtlk_ldlist_pop_front(&log_data_osdep.usp_queue))) {
    if (written)
      return written;
    if (filp->f_flags & O_NONBLOCK)
      return -EAGAIN;

    if (wait_event_interruptible(log_data_osdep.in_wait_queue,
          !mtlk_ldlist_is_empty(&log_data_osdep.usp_queue)))
      return -ERESTARTSYS; // signal
  }

  // Now there is some data to send
  to_send = usp_entry->len - usp_entry->pos;
  if (to_send <= 0) {
    release_usp_entry(usp_entry);
    goto next_entry;
  }

  if ((unsigned long)to_send > count)
    to_send = count;

  switch (usp_entry->type) {
  case USP_ENTRY_CDATA:
    copy_src = usp_entry->u.pdata;
    break;
  case USP_ENTRY_READY_BUF:
    copy_src = _mtlk_log_buf_get_data_buffer(usp_entry->u.pbuf);
    break;
  default:
    ASSERT(0);
    // FIXME: group similar messages
    ELOG_V("Unknown data queue item type");
    return -EFAULT;
  }

  not_copied = copy_to_user(buf + written, copy_src + usp_entry->pos,
      to_send);
  if (not_copied != 0) {
    mtlk_ldlist_push_front(&log_data_osdep.usp_queue,
        (mtlk_ldlist_entry_t *) usp_entry);
    if (written)
      return written;
    else
      return -EFAULT;
  }
  written += to_send;
  count -= to_send;
  usp_entry->pos += to_send;

  if (usp_entry->pos == usp_entry->len) {
    // No more data to write in this entry, try the next one
    release_usp_entry(usp_entry);
    goto next_entry;
  } else {
    // No more space in userspace buffer
    ASSERT(count == 0);
    mtlk_ldlist_push_front(&log_data_osdep.usp_queue,
          (mtlk_ldlist_entry_t *) usp_entry);
  }
  return written;
}

static ssize_t
cdev_log_write(struct file *filp, const char __user *buf, size_t count,
                    loff_t *f_pos)
{
  return -EFAULT;
}

static unsigned int
cdev_log_poll(struct file *filp, poll_table *wait)
{
  unsigned int mask = 0;

  poll_wait(filp, &log_data_osdep.in_wait_queue, wait);
  if (!mtlk_ldlist_is_empty(&log_data_osdep.usp_queue)) {
    mask |= POLLIN | POLLRDNORM; // readable
  }
  return mask;
}

static int
cdev_log_fasync(int fd, struct file *filp, int mode)
{
  mtlk_cdev_t *dev = filp->private_data;

  return fasync_helper(fd, filp, mode, &dev->async_queue);
}

static long
cdev_on_logserv_ctrl (char __user *buf)
{
  long                res = -EINVAL;
  mtlk_log_ctrl_hdr_t hdr;
  char               *data = NULL;

  if (0 != (copy_from_user(&hdr, buf, sizeof(hdr)))) {
    res = -EFAULT;
    goto end;
  }
  buf += sizeof(hdr);

  if (hdr.data_size) {
    data = (char *)kmalloc(hdr.data_size, GFP_KERNEL);
    if (!data) {
      res = -ENOMEM;
      goto end;
    }

    if (0 != (copy_from_user(data, buf, hdr.data_size))) {
      res = -EFAULT;
      goto end;
    }
  }

  switch (hdr.id) {
  case MTLK_LOG_CTRL_ID_VERINFO:
    if (hdr.data_size == sizeof(mtlk_log_ctrl_ver_info_data_t)) {
      mtlk_log_ctrl_ver_info_data_t *ver_info = (mtlk_log_ctrl_ver_info_data_t *)data;

      ver_info->major = RTLOGGER_VER_MAJOR;
      ver_info->minor = RTLOGGER_VER_MINOR;
      res = 0;
    }
    break;
  default:
    res = -ENOTSUPP;
    break;
  }

  if (hdr.data_size &&
      (0 != (copy_to_user(buf, data, hdr.data_size)))) {
      res = -EFAULT;
      goto end;
  }

end:
  if (data) {
    kfree(data);
  }

  return res;
}

static long
cdev_on_ioctl (struct file  *filp,
               unsigned int  cmd,
               unsigned long arg)
{
  int          res = -ENOTSUPP;
  char __user *buf = (char __user *)arg;

  switch (cmd) {
  case SIOCDEVPRIVATE:
    res = cdev_on_logserv_ctrl(buf);
    break;
  default:
    break;
  }

  return res;
}

static long
cdev_unlocked_ioctl (struct file  *filp,
                     unsigned int  cmd,
                     unsigned long arg)
{
  return cdev_on_ioctl(filp, cmd, arg);
}

// TODO: make a pool of free usp_entries
#ifndef IWLWAV_RTLOG_DONT_USE_USP
static mtlk_usp_entry_t *
alloc_usp_entry(void)
{
    return mtlk_osal_mem_alloc(sizeof(mtlk_usp_entry_t), MTLK_MEM_TAG_LOGGER);
}
#endif

static void
release_usp_entry(mtlk_usp_entry_t *usp_entry)
{
  ASSERT(usp_entry);

  switch (usp_entry->type) {
  case USP_ENTRY_CDATA:
    // Pointer to constant data, do not free
    break;
  case USP_ENTRY_READY_BUF:
    mtlk_log_put_free_buf(usp_entry->u.pbuf);
    break;
  default:
    ASSERT(0);
    ELOG_V("Unknown data queue item type");
    break;
  }
  mtlk_osal_mem_free(usp_entry);
}

static void
usp_queue_release_entries(void)
{
  mtlk_usp_entry_t *usp_entry;
  while ((usp_entry = (mtlk_usp_entry_t *)
        mtlk_ldlist_pop_front(&log_data_osdep.usp_queue)) != NULL) {
    release_usp_entry(usp_entry);
  }
}
#endif

static ssize_t logger_debug_write(struct file *file, const char __user *buffer,
                                  size_t count, loff_t *ppos)
{
  char *debug_conf = NULL;
  int result = 0;

  debug_conf = kmalloc(count + 1, GFP_KERNEL);
  if (!debug_conf) {
    ELOG_D("Unable to allocate %lu bytes", count);
    result = -EFAULT;
    goto FINISH;
  }
  if (0 != (copy_from_user(debug_conf, buffer, count))) {
    result = -EFAULT;
    goto FINISH;
  }
  debug_conf[count] = '\0';
  mtlk_log_set_conf(debug_conf);
  result = count;
FINISH:
  if (debug_conf)
    kfree(debug_conf);
  return result;
}

static int show_logger_debug (struct seq_file *seq, void *v)
{
  mtlk_log_get_conf(seq);
  return 0;
}

static int logger_debug_open(struct inode *inode, struct file *file)
{
  return single_open(file, show_logger_debug, NULL);
}

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static int show_logger_stat(struct seq_file *seq, void *v)
{
  mtlk_osal_lock_acquire(&log_data.data_lock);

  seq_printf(seq, "%10u\tLog packets processed successfully\n",
             log_data.log_pkt_reservations_succeeded);
  seq_printf(seq, "%10u\tLog packets failed to process\n",
             log_data.log_pkt_reservations_failed);
  seq_printf(seq, "%10u\tLog buffer allocations succeeded\n",
             log_data.log_buff_allocations_succeeded);
  seq_printf(seq, "%10u\tLog buffer allocations failed\n",
             log_data.log_buff_allocations_failed);

  mtlk_osal_lock_release(&log_data.data_lock);
  return 0;
}

static int logger_stat_open(struct inode *inode, struct file *file)
{
  return single_open(file, show_logger_stat, NULL);
}

/* ----------------------------------------------------------------- */
/* HW RT-Logger specific
 */

#ifdef CPTCFG_IWLWAV_DEBUG
#define IWLWAV_RTLOG_DEBUG_TRACE   mtlk_osal_emergency_print("%s(%s)", __FUNCTION__, dev->name)
#else
#define IWLWAV_RTLOG_DEBUG_TRACE
#endif

struct net_device        *rtlog_netdev = NULL;
struct net_device_stats  *rtlog_stats;

#define rtlog_netdev_name   proc_entry_rtlog    /* the same as /proc entry name */

static int rtlog_hard_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    ++rtlog_stats->tx_packets;

    /* just drop buf */
    dev_kfree_skb(skb);

    return 0;
}

static int rtlog_set_mac_address(struct net_device *dev, void *addr)
{
    IWLWAV_RTLOG_DEBUG_TRACE;

    /* mac addr should be correct here */
    eth_commit_mac_addr_change(dev, addr);

    return 0;
}

/* This is where ifconfig comes down and tells us who we are, etc.
 * We can just ignore this.
 */
static int rtlog_config(struct net_device *dev, struct ifmap *map)
{
    return 0;
}

/* Change MTU is not permitted */
static int rtlog_change_mtu(struct net_device *dev, int new_mtu)
{
    IWLWAV_RTLOG_DEBUG_TRACE;
    return -1;
}

static int rtlog_open(struct net_device *dev)
{
    IWLWAV_RTLOG_DEBUG_TRACE;

    /* start up the transmission queue */
    netif_start_queue(dev);

    return 0;
}

static int rtlog_close(struct net_device *dev)
{
    IWLWAV_RTLOG_DEBUG_TRACE;

    /* shutdown the transmission queue */
    netif_stop_queue(dev);

    return 0;
}

static struct net_device_stats *rtlog_get_stats(struct net_device *dev)
{
    return rtlog_stats;
}

static struct net_device_ops ndo = {
    .ndo_open       = rtlog_open,
    .ndo_stop       = rtlog_close,
    .ndo_start_xmit = rtlog_hard_start_xmit,
#if 0
    .ndo_do_ioctl   = rtlog_do_ioctl,
#endif
    .ndo_get_stats  = rtlog_get_stats,
    .ndo_set_config = rtlog_config,
    .ndo_change_mtu = rtlog_change_mtu,
    .ndo_set_mac_address = rtlog_set_mac_address,
};

static void rtlog_setup(struct net_device *dev)
{
    IWLWAV_RTLOG_DEBUG_TRACE;

    ether_setup(dev);

    dev->netdev_ops = &ndo;
    rtlog_stats = &dev->stats;
}

/* ------------- */

static int
rtlog_netdev_init(void)
{
    rtlog_netdev = alloc_netdev(0, rtlog_netdev_name "%d", NET_NAME_UNKNOWN, rtlog_setup);

    if (NULL == rtlog_netdev) {
        mtlk_osal_emergency_print("%s: Failed to alloc", __FUNCTION__);
        goto FAIL;
    }

    if (register_netdev(rtlog_netdev)) {
        mtlk_osal_emergency_print("%s: Failed to register", __FUNCTION__);
        free_netdev(rtlog_netdev);
        rtlog_netdev = NULL;
        goto FAIL;
    }
    mtlk_osal_emergency_print("Allocated netdev %s", dev_name(&rtlog_netdev->dev));

    return MTLK_ERR_OK;

FAIL:
    return MTLK_ERR_NO_RESOURCES;
}

static void
rtlog_netdev_cleanup(void)
{
    mtlk_osal_emergency_print("Unloading rtlog_netdev");
    unregister_netdev(rtlog_netdev);
    free_netdev(rtlog_netdev);
    rtlog_netdev = NULL;
}
#endif
