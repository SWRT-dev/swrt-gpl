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
 * This file contains compartibility stuff
 * between different Linux kernels.
 *
 */
#include "mtlkinc.h"

#include "compat.h"
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include <linux/firmware.h>
#else
#include <linux/kmod.h>

#include "mem_leak.h"

static struct firmware firmw;

#ifdef WITH_FW_BINS
#include "Mkbins/mkbins.c"
#endif

#define LOG_LOCAL_GID   GID_COMPAT
#define LOG_LOCAL_FID   1

#ifdef WITH_FW_BINS

int request_firmware (const struct firmware **fw, const char *name,
                      const void *device)
{
  int i;
  *fw= &firmw;
  i= 0;
  while(ft[i].name){
    if(strcmp(ft[i].name, name) == 0){
      ILOG3_S("loading %s\n", name);
      firmw.size= ft[i].size;
      firmw.data= ft[i].filep;
      return(0);
    }
    i++;
  }
  *fw = NULL;
  return(1);
}

void release_firmware (const struct firmware *fw)
{
}

#else

#ifndef ALIGN
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))
#endif

#define MTLK_FIRMWARE_PROC_NAME "mtlk_firmware"
#define MTLK_FIRMWARE_PATH      "/tmp/"

typedef struct
  {
    struct completion done;
    char *file_name;
    struct firmware fw;
    size_t count;
    /* currently allocated size */
    size_t alloc_size;
  } FIRMWARE_DATA;

struct proc_dir_entry *firmware_pentry;

static int load_firmware_open(struct inode *i, struct file *f)
{
  return 0;
}

static int load_firmware_release(struct inode *i, struct file *f)
{
  FIRMWARE_DATA *data;

  data = (FIRMWARE_DATA *)firmware_pentry->data;
  if(data->count != 0)
    data->fw.size=data->count;
  if(data->count == 0)
    ELOG_V("File is missing...");
  else
    ILOG3_D("done (%d bytes)\n", data->count);
    ILOG1_SSD("Finished reading %s%s firmware file. Received %d bytes...\n",
      MTLK_FIRMWARE_PATH, data->file_name, data->count);
  complete(&data->done);  
  return 0;
}

static int load_firmware_realloc_buffer(FIRMWARE_DATA *fw_priv, int min_size)
{
  u8 *new_data;
  int new_size = fw_priv->alloc_size;

  if (min_size <= fw_priv->alloc_size)
    return 0;

  new_size = ALIGN(min_size, PAGE_SIZE);
  new_data = vmalloc_tag(new_size, MTLK_MEM_TAG_COMPAT);
  if(!new_data) {
    ELOG_S("%s: unable to alloc buffer\n", __FUNCTION__);
    /* Make sure that we don't keep incomplete data */
    //fw_load_abort(fw_priv);
    return -ENOMEM;
  }
  fw_priv->alloc_size = new_size;
  if(fw_priv->fw.data) {
    wave_memcpy(new_data, new_size, fw_priv->fw.data, fw_priv->count);
    vfree_tag(fw_priv->fw.data);
  }
  fw_priv->fw.data = new_data;
  BUG_ON(min_size > fw_priv->alloc_size);
  return 0;
}

static int load_firmware_write(struct file *f, const char *buffer, size_t count, loff_t *off)
{
  FIRMWARE_DATA *data;
  int retval;

  data = (FIRMWARE_DATA *)firmware_pentry->data;
  retval=load_firmware_realloc_buffer(data, data->count+count);
  if (retval)
    return 0;
  if(copy_from_user(data->fw.data + *off, buffer, count)) {
    return -EINVAL;
  }
  data->count += count;
  *off += count;
  return count;
}

struct file_operations load_firmware_fops = {
    .open    = load_firmware_open,
    .release = load_firmware_release,
    .write   = load_firmware_write,
};

static int load_firmware_thread(void *arg)
{
  char temp[128];
  char *argv[] = {"/bin/sh", "-c", NULL, NULL};
  int result = 0;
  char *envp[] = {"HOME=/", "TERM=linux", "PATH=/sbin:/usr/sbin:/bin:/usr/bin", NULL};

  mtlk_snprintf(temp, sizeof(temp), "/bin/cat "MTLK_FIRMWARE_PATH"%s > /proc/"MTLK_FIRMWARE_PROC_NAME, (char *)arg);
  argv[2] = temp;
  ILOG2_SS("Loading firmware file %s%s... ", MTLK_FIRMWARE_PATH, (char *)arg);
  result = exec_usermodehelper(argv[0], argv, envp);
  if (result) {
    ILOG2_DSS("Failed [return = %d] to load firmware file %s%s", result, MTLK_FIRMWARE_PATH, (char *)arg);
  }
  ILOG2_V("Finished loading firmware");
  return result;
}

int request_firmware (const struct firmware **fw, const char *name,
                      const void *device)
{
  int pid;
  int result;
  FIRMWARE_DATA data;
  int waitpid_result;

  memset(&data,0,sizeof(data));
  init_completion(&data.done);
  data.file_name = (char *)name;
  result = 1;
  /* Registering /proc file... */
  firmware_pentry = create_proc_entry(MTLK_FIRMWARE_PROC_NAME, S_IFREG | S_IRUSR | S_IRGRP, &proc_root);
  if(firmware_pentry == NULL) {
    ILOG2_V("Failed to create temporary proc-fs file");
    goto done;
  }
  firmware_pentry->data = (void *)&data;
  firmware_pentry->proc_fops = &load_firmware_fops;
  /* Starting reader thread... */
  pid = kernel_thread(load_firmware_thread, (void *)name, CLONE_VFORK);
  if (pid < 0) {
    ILOG2_V("Failed to start thread...");
    goto done;
  }
  /* Waiting for a miracle... */
  wait_for_completion(&data.done);
  waitpid_result = sys_wait4((int)pid, NULL, __WALL, NULL); /* remove zombie */
  if (waitpid_result != pid)
    ELOG_DD("sys_wait4(%d,...) failed, errno %d", pid, -waitpid_result);
  if(data.count == 0) {
    result = 1;
  }
  else {
    result = 0;
  }
done:
  remove_proc_entry(MTLK_FIRMWARE_PROC_NAME, &proc_root);
  firmw=data.fw;
  if (data.count)
    *fw=&firmw;
  else
    *fw=NULL;
  return result;
}

void release_firmware (const struct firmware *fw)
{
  if (fw->data)
    vfree_tag(fw->data);
}
#endif
#endif
