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
 * $Id$
 *
 * 
 *
 * DF UI API implementation for working with Linux Proc_FS
 *
 */


/********************************************************************
 * Includes
 ********************************************************************/
#include "mtlkinc.h"

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/module.h>

#include "mtlk_df_priv.h"


/********************************************************************
 * Local definitions
 ********************************************************************/
#define LOG_LOCAL_GID   GID_PROCAUX
#define LOG_LOCAL_FID   1

/***
 * Linux OS related definitions
 ***/

#define PROC_NET    init_net.proc_net

/********************************************************************
 * Local types definitions
 ********************************************************************/

/* structure to hold proc ops also base address and size */
struct mtlk_proc_ops {
  mtlk_dlist_entry_t          list_entry;
  void                        *df;
  mtlk_df_proc_entry_read_f   read_func;
  mtlk_df_proc_entry_write_f  write_func;
  uint32                      io_base;
  uint32                      io_size;
};

/* structure to hold seq_ops near mtlk_core_t pointer
 * so we can get it from seq_ functions
 */
struct mtlk_seq_ops {
  mtlk_dlist_entry_t      list_entry;
  void                    *df;
  struct semaphore        sem;
  struct seq_operations   seq_ops;
  unsigned                data_size; /* fixed data size */
};

struct _mtlk_df_proc_fs_node_t {
  /* parent node */
  struct _mtlk_df_proc_fs_node_t *parent;

  /* our dir */
  struct proc_dir_entry *dir;

  /* our name */
  const uint8* name;

  mtlk_dlist_t proc_ops_list;

  /* list of structs with seq_ops needed for some proc entries.*/
  mtlk_dlist_t seq_ops_list;

  uint16 num_entries;
};

/********************************************************************
 * Functions implementation
 *********************************************************************/

mtlk_df_proc_fs_node_t* __MTLK_IFUNC
mtlk_df_proc_node_create(const uint8 *name, mtlk_df_proc_fs_node_t* parent)
{
  mtlk_df_proc_fs_node_t *proc_node;

  MTLK_ASSERT(NULL != name);

  /* allocate node */
  proc_node = mtlk_osal_mem_alloc(sizeof(*proc_node), MTLK_MEM_TAG_PROC);
  if (NULL == proc_node){
    ELOG_V("Cannot allocate memory for mtlk_df_proc_fs_node_t structure.");
    goto alloc_err;
  }

  memset(proc_node, 0, sizeof(*proc_node));

  proc_node->name = name;

  /* if parent node is NULL */
  if (NULL == parent) {
    /*- create root node (usually /proc/net/ */
    proc_node->dir = PROC_NET;
  } else {
    /*- create new node (usually /proc/net/... */
    proc_node->parent = parent;
    proc_node->dir = proc_mkdir(name, proc_node->parent->dir);
    if (NULL == proc_node->dir) {
      goto create_err;
    }
  }

  mtlk_dlist_init(&proc_node->proc_ops_list);

  /* initialize list of seq_ops structures */
  mtlk_dlist_init(&proc_node->seq_ops_list);

  if (NULL == parent) {
    ILOG1_S("Proc node %s/ created", proc_node->name);
  } else {
    ILOG1_SS("Proc node %s/%s created", proc_node->parent->name, proc_node->name);
  }

  return proc_node;

create_err:
  /* free node */
  mtlk_osal_mem_free(proc_node);
alloc_err:
  ELOG_S("Failed to create proc node %s", name);
  return NULL;
}

void __MTLK_IFUNC
mtlk_df_proc_node_delete(mtlk_df_proc_fs_node_t* proc_node)
{
  mtlk_dlist_entry_t* entry;
  struct mtlk_seq_ops* seq_ops;
  struct mtlk_proc_ops* proc_ops;

  /*check node entries - all entries should be removed */
  if (0 != proc_node->num_entries) {
    WLOG_D("Some (%u) Proc entries are not deleted", proc_node->num_entries);
    MTLK_ASSERT(0);
    return;
  }

  if (NULL != proc_node->parent) {
    /* it's not root node - remove Proc_Fs entry */
    remove_proc_entry(proc_node->name, proc_node->parent->dir);
  }

  if (NULL == proc_node->parent) {
    ILOG1_S("Proc node %s/ deleted", proc_node->name);
  } else {
    ILOG1_SS("Proc node %s/%s deleted", proc_node->parent->name, proc_node->name);
  }

  while(NULL != (entry = mtlk_dlist_pop_back(&proc_node->proc_ops_list))) {
    proc_ops = MTLK_LIST_GET_CONTAINING_RECORD(entry, struct mtlk_proc_ops, list_entry);
    mtlk_osal_mem_free(proc_ops);
  }

  /* freeing all dynamically allocated mtlk_seq_ops structures needed for some proc entries */
  while(NULL != (entry = mtlk_dlist_pop_back(&proc_node->seq_ops_list))) {
    seq_ops = MTLK_LIST_GET_CONTAINING_RECORD(entry, struct mtlk_seq_ops, list_entry);
    mtlk_osal_mem_free(seq_ops);
  }

  mtlk_dlist_cleanup(&proc_node->proc_ops_list);
  mtlk_dlist_cleanup(&proc_node->seq_ops_list);

  /* free node */
  mtlk_osal_mem_free(proc_node);
}

static int
_mtlk_df_proc_open(struct inode *inode,
                   struct file *file)
{
  file->private_data = PDE_DATA(inode);

  return 0;
}

#define PROC_READ_BUF_LEN PAGE_SIZE

static ssize_t
_mtlk_df_proc_read(struct file *file,
                   char *buffer,
                   size_t length,
                   loff_t *offset)
{
  struct mtlk_proc_ops *tmp_proc_ops;
  int bytes_read = 0;
  int retval = 0;
  char *usebuffer = NULL;

  tmp_proc_ops = (struct mtlk_proc_ops*) file->private_data;
  if (!tmp_proc_ops->read_func) {
    return -EPERM;
  }

  usebuffer = kmalloc(PROC_READ_BUF_LEN, GFP_KERNEL);
  if (!usebuffer) {
    ELOG_V("Memory allocation failed");
    return -ENOMEM;
  }

  if (length > PROC_READ_BUF_LEN) {
    length = PROC_READ_BUF_LEN;
  }

  bytes_read = tmp_proc_ops->read_func(usebuffer, *offset, length, tmp_proc_ops);
  if (0 < bytes_read) {
    retval = copy_to_user(buffer, usebuffer, bytes_read);
    if (retval) {
        ELOG_D("copy_to_user() failed: %d", retval);
        bytes_read = -EFAULT;
        goto out;
    }
    *offset += bytes_read;
  }

out:
  kfree(usebuffer);

  return bytes_read;
}

static ssize_t
_mtlk_df_proc_write(struct file *file,
                    const char *buffer,
                    size_t len,
                    loff_t *offset)
{
  struct mtlk_proc_ops *tmp_proc_ops;
  int retval = 0;

  tmp_proc_ops = (struct mtlk_proc_ops*) file->private_data;
  if (!tmp_proc_ops->write_func) {
    return -EPERM;
  }

  retval = tmp_proc_ops->write_func(file, buffer, len, tmp_proc_ops);
  if (0 < retval) {
    *offset += retval;
  }

  return retval;
}

struct file_operations _mtlk_df_proc_entry_fops = {
  .owner   = THIS_MODULE,
  .open    = _mtlk_df_proc_open,
  .read    = _mtlk_df_proc_read,
  .write   = _mtlk_df_proc_write
};

/***
 * Proc_Fs files API implementation
 *
 ***/
static int
_mtlk_df_proc_node_add_entry(const char *name,
                            mtlk_df_proc_fs_node_t *parent_node,
                            void *df,
                            mode_t mode,
                            mtlk_df_proc_entry_read_f  read_func,
                            mtlk_df_proc_entry_write_f write_func,
                            uint32 io_base, uint32 io_size)
{
  struct mtlk_proc_ops *tmp_proc_ops;
  struct proc_dir_entry *pde;

  MTLK_ASSERT(NULL != parent_node);
  MTLK_ASSERT(NULL != name);

  tmp_proc_ops = mtlk_osal_mem_alloc(sizeof(*tmp_proc_ops), MTLK_MEM_TAG_PROC);
  if (NULL == tmp_proc_ops){
    ELOG_V("Cannot allocate memory for mtlk_proc_ops structure.");
    goto alloc_err;
  }
  memset(tmp_proc_ops, 0, sizeof(*tmp_proc_ops));

  tmp_proc_ops->read_func  = read_func;
  tmp_proc_ops->write_func = write_func;
  tmp_proc_ops->df         = df;
  tmp_proc_ops->io_base    = io_base;
  tmp_proc_ops->io_size    = io_size;

  pde = proc_create_data(name, mode, parent_node->dir, &_mtlk_df_proc_entry_fops, tmp_proc_ops);
  if (NULL == pde) {
    goto pde_err;
  };

  mtlk_dlist_push_back(&parent_node->proc_ops_list, &tmp_proc_ops->list_entry);

  parent_node->num_entries++;

  ILOG1_SS("Proc entry %s/%s created", parent_node->name, name);

  return MTLK_ERR_OK;

pde_err:
  mtlk_osal_mem_free(tmp_proc_ops);

alloc_err:
  ELOG_S("Failed to create proc entry for %s", name);
  return MTLK_ERR_UNKNOWN;
}

int __MTLK_IFUNC
mtlk_df_proc_node_add_io_entry(const char *name,
                               mtlk_df_proc_fs_node_t *parent_node,
                               void *df,
                               mtlk_df_proc_entry_read_f read_func,
                               uint32 io_base, uint32 io_size)
{
  return _mtlk_df_proc_node_add_entry(
            name, parent_node, df,
            S_IFREG|S_IRUSR|S_IRGRP,
            read_func, NULL, io_base, io_size);
}

int __MTLK_IFUNC
mtlk_df_proc_node_add_ro_entry(char *name,
                               mtlk_df_proc_fs_node_t *parent_node,
                               void *df,
                               mtlk_df_proc_entry_read_f  read_func)
{
  return _mtlk_df_proc_node_add_entry(
            name, parent_node, df,
            S_IFREG|S_IRUSR|S_IRGRP,
            read_func, NULL, 0, 0);
}

int  __MTLK_IFUNC
mtlk_df_proc_node_add_wo_entry(char *name,
                               mtlk_df_proc_fs_node_t *parent_node,
                               void *df,
                               mtlk_df_proc_entry_write_f write_func)
{
  return _mtlk_df_proc_node_add_entry(
            name, parent_node, df,
            S_IFREG|S_IWUSR,
            NULL, write_func, 0, 0);
}

int  __MTLK_IFUNC
mtlk_df_proc_node_add_rw_entry(char *name,
                               mtlk_df_proc_fs_node_t *parent_node,
                               void *df,
                               mtlk_df_proc_entry_read_f  read_func,
                               mtlk_df_proc_entry_write_f write_func)
{
  return _mtlk_df_proc_node_add_entry(
            name, parent_node, df,
            S_IFREG|S_IRUGO|S_IWUSR, /*Read all, change only root*/
            read_func, write_func, 0, 0);
}

/***
 * SEQ Proc_Fs files API implementation
 ***/

/* these three functions are used to "emulate" simplified
 * seq_file methods (single_*). we can't use it because
 * we have older kernels (e.g. 2.4.20 on cavium) which
 * lack this functionality.
 */
static void*
_mtlk_df_proc_seq_entry_start_ops(struct seq_file *s, loff_t *pos)
{
  return *pos? NULL : SEQ_START_TOKEN;
}

static void*
_mtlk_df_proc_seq_entry_next_ops(struct seq_file *s, void *v, loff_t *pos)
{
  ++*pos;
  return NULL;
}

static void
_mtlk_df_proc_seq_entry_stop_ops(struct seq_file *s, void *v)
{
}

/* addition to the method .open for case of fixed size */
static int
_seq_open_size(struct file *file, struct mtlk_seq_ops *ops, unsigned size)
{
  char *buf;
  int   ret;

  size += 1;  /* +1 to prevent buf overflow */
  MTLK_ASSERT(size);

  buf = kmalloc(size, GFP_KERNEL);
  if (!buf)
    return -ENOMEM;

  ret = seq_open(file, &ops->seq_ops);
  if (ret) {
    kfree(buf);
    return ret;
  }
  ((struct seq_file *)file->private_data)->buf = buf;
  ((struct seq_file *)file->private_data)->size = size;

  return 0;
}

/* these two functions are registered as .open and .release
 * methods of the file_operations structure used by all
 * "one-function" seq files.
 */
static int
_mtlk_df_proc_seq_entry_open_ops(struct inode *inode, struct file *file)
{
  struct mtlk_seq_ops   *ops = PDE_DATA(inode);
  unsigned size;

  size = ops->data_size;

  /* try to get semaphore without sleep first */
  if(down_trylock(&ops->sem)) {
    if(file->f_flags & O_NONBLOCK)
      return -EAGAIN;
    /* ok, we'll have to sleep if it's still not free */
    if(down_interruptible(&ops->sem))
      return -EINTR;
  };

  /* we've got semaphore, so proceed */
  if (size) {
    return _seq_open_size(file, ops, size);
  } else {
    return seq_open(file, &ops->seq_ops);
  }
}

static int
_mtlk_df_proc_seq_entry_release_ops(struct inode *inode, struct file *file)
{
  struct mtlk_seq_ops *ops = PDE_DATA(inode);

  /* release semaphore acquired in the .open function */
  up(&ops->sem);
  return seq_release(inode, file);
}

/* this is the common structure used for all one
 * function seq_file files. open method gets per
 * file semaphore and calls seq_open, release
 * method releases per-file semaphore and calls
 * seq_release.
 */
struct file_operations _mtlk_df_proc_seq_entry_fops = {
  .owner   = THIS_MODULE,
  .open    = _mtlk_df_proc_seq_entry_open_ops,
  .read    = seq_read,
  .llseek  = seq_lseek,
  .release = _mtlk_df_proc_seq_entry_release_ops
};


int __MTLK_IFUNC
mtlk_df_proc_node_add_seq_entry_size(char *name,
                                 mtlk_df_proc_fs_node_t *parent_node,
                                 void *df,
                                 mtlk_df_proc_seq_entry_show_f show_func,
                                 unsigned size)
{
  struct mtlk_seq_ops *tmp_seq_ops;
  struct proc_dir_entry *pde;

  MTLK_ASSERT(NULL != parent_node);
  MTLK_ASSERT(NULL != name);

  tmp_seq_ops = mtlk_osal_mem_alloc(sizeof(*tmp_seq_ops), MTLK_MEM_TAG_PROC);
  if (NULL == tmp_seq_ops){
    ELOG_V("Cannot allocate memory for mtlk_seq_ops structure.");
    goto alloc_err;
  }
  memset(tmp_seq_ops, 0, sizeof(*tmp_seq_ops));

  /* initialize newly allocated mtlk_seq_ops structure */
  sema_init(&tmp_seq_ops->sem, 1);
  tmp_seq_ops->df = df;
  tmp_seq_ops->data_size = size;
  tmp_seq_ops->seq_ops = (struct seq_operations) {
    .start = _mtlk_df_proc_seq_entry_start_ops,
    .next  = _mtlk_df_proc_seq_entry_next_ops,
    .stop  = _mtlk_df_proc_seq_entry_stop_ops,
    .show  = show_func
  };

  /* create and init proc entry */
  pde = proc_create_data(name, S_IFREG|S_IRUSR|S_IRGRP|S_IROTH, parent_node->dir,
                         &_mtlk_df_proc_seq_entry_fops, tmp_seq_ops);
  if (NULL == pde) {
    goto pde_err;
  };

  proc_set_size(pde, size); /* set entry size */

  /* adding allocated structure to the list, so it
   * can be easily tracked and freed on cleanup
   */
  mtlk_dlist_push_back(&parent_node->seq_ops_list, &tmp_seq_ops->list_entry);

  parent_node->num_entries++;

  ILOG1_SS("Proc entry %s/%s created", parent_node->name, name);

  return MTLK_ERR_OK;

pde_err:
  mtlk_osal_mem_free(tmp_seq_ops);

alloc_err:
  ELOG_S("Failed to create proc entry for %s", name);
  return MTLK_ERR_UNKNOWN;
}

void __MTLK_IFUNC
mtlk_df_proc_node_remove_entry(const char *name,
                               mtlk_df_proc_fs_node_t *parent_node)
{
  remove_proc_entry(name, parent_node->dir);

  if (0 < parent_node->num_entries) {
    parent_node->num_entries--;
  } else {
    WLOG_S("Some entries in %s have been deleted more then once", parent_node->name);
  }

  ILOG1_SS("Proc entry %s/%s deleted", parent_node->name, name);
}

void* __MTLK_IFUNC
mtlk_df_proc_seq_entry_get_df(mtlk_seq_entry_t *seq_ctx)
{
  return (container_of(seq_ctx->op, struct mtlk_seq_ops, seq_ops))->df;
}

void* __MTLK_IFUNC
mtlk_df_proc_entry_get_df (struct mtlk_proc_ops *ctx)
{
  return ctx->df;
}

void __MTLK_IFUNC
mtlk_df_proc_entry_get_io_range (struct mtlk_proc_ops *ctx, uint32 *io_base, uint32 *io_size)
{
  *io_base = ctx->io_base;
  *io_size = ctx->io_size;
}
