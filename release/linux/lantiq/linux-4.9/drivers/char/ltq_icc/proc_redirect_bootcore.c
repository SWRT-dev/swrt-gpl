/*
 * /proc hooks for MIPS MT scheduling policy management for 34K cores
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * Copyright (C) 2006 Mips Technologies, Inc
 */

#include <linux/kernel.h>

#include <asm/cpu.h>
#include <asm/processor.h>
#include <asm/barrier.h>
#include <asm/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

#include <linux/icc/drv_icc.h>

extern struct proc_dir_entry *proc_lokup_dir;
struct proc_dir_entry *bootcore_proc_dir;
icc_msg_t rw;
icc_msg_t write;

#define ICC_MAX_PROC_SIZE (2*4096)
CREATE_TRACE_GROUP(PROC);
static int proc_redirect_handler(void)
{
	loff_t i;
	char *tmpdata;
	struct file* filp = NULL;
	mm_segment_t fs;
		/*param 0 and 2 contains pointer and 1,3 contains their respective length*/
		write.param[1]=0;
		write.param[0]=rw.param[0];
		TRACE(PROC, DBG_LEVEL_LOW, ("procfs entry is %s\n",(uint8_t *)rw.param[0]));
		tmpdata=(uint8_t *)rw.param[0]+strlen((uint8_t *)rw.param[0])+1;
		fs = get_fs();
		set_fs(get_ds());
		filp = filp_open((uint8_t *)rw.param[0],O_RDWR,0666);
		if(IS_ERR(filp)) {
        goto error;
    }else if (tmpdata[0]==0){
			memset((uint32_t *)write.param[0],0,ICC_MAX_PROC_SIZE);
			i=0;
			while (vfs_read(filp,(uint8_t *)write.param[0]+write.param[1],1, &i) == 1)
      {
				write.param[1]++;
      }
			if(write.param[1]==0)
				goto error;
		}else{
			i=0;
				TRACE(PROC, DBG_LEVEL_LOW, ("data to be written to proc is %s\n",tmpdata));
				vfs_write(filp,tmpdata,strlen(tmpdata), &i);
				memset((uint32_t *)write.param[0],0,ICC_MAX_PROC_SIZE);
				memcpy((uint8_t *)write.param[0],"written\n",8);
				write.param[1]=8;
		}
		cache_wb_inv(write.param[0],ICC_MAX_PROC_SIZE);
		icc_write(PR,&write);
		filp_close(filp, NULL);
		set_fs(fs);
		return 0;	
error:
		set_fs(fs);
		memset((uint32_t *)write.param[0],0,ICC_MAX_PROC_SIZE);
		sprintf((uint8_t *)write.param[0],"Noprocfile\n");
		write.param[1]=12;
		cache_wb_inv(write.param[0],ICC_MAX_PROC_SIZE);
		icc_write(PR,&write);	
	return 0;
}

void pfn_ltq_proc_callback(icc_wake_type wake_type){
  int ret;
	if( (wake_type & ICC_READ) )
	{
  	memset(&rw,0,sizeof(icc_msg_t));
  	ret=icc_read(PR,&rw);
		cache_inv(rw.param[0],rw.param[1]);
		proc_redirect_handler();
	}
}
static int __init init_bootcore_proc(void)
{
	int result=0;
	/*For proc redirector we will always malloc and pass this pointer from IAP to 4kec
		As it remains forever we are not freeing it and re-use same buffer for other transactions also*/
  result=icc_open((struct inode *)PR,NULL);
  if(result<0){
    TRACE(PROC, DBG_LEVEL_HIGH, ("open icc Failed\n"));
    return result;
  }
	result=icc_register_callback(PR,&pfn_ltq_proc_callback);
 if(result<0){
    TRACE(PROC, DBG_LEVEL_HIGH, ("CallBack Register with ICC Failed\n"));
    return result;
  }
	/*source and dest client id to be filled*/
	write.src_client_id=write.dst_client_id=PR;
	write.param_attr=ICC_PARAM_PTR;
	return 0;
}


/* Automagically create the entry */
module_init(init_bootcore_proc);
MODULE_LICENSE("GPL");
