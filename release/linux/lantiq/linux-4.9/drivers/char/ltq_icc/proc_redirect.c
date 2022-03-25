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
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/icc/drv_icc.h>

#ifdef CONFIG_PROC_FS
#include <internal.h>
#endif /* CONFIG_PROC_FS */

extern struct proc_dir_entry *proc_lokup_dir;
struct proc_dir_entry *bootcore_proc_dir;
unsigned int proc_wakeup;
unsigned int proc_flag;
static wait_queue_head_t proc_wakeuplist;
#define ICC_MAX_PROC_SIZE (2*4096)
#define PROC_SIZE 4096
#define PROC_SHIFT 12
icc_msg_t rw;
icc_msg_t write;
static char a[][36]={"cpuinfo","meminfo","slabinfo","interrupts","cmdline","stat","vmstat",
										"driver/ltq_mps/mailbox","sys/vm/drop_caches"};

static int create_procredirect_entry(void);
CREATE_TRACE_GROUP(PROC);
static void get_proc_entry(void){
	struct proc_dir_entry *de;
	de=proc_lokup_dir;
  while(de->parent != NULL){
		if(!memcmp(de->name,"bootcore",8)){
    	de=de->parent;
			continue;
		}
    if(!memcmp(de->name,"/proc",5)){
			memmove((uint8_t *)write.param[0]+5,(uint8_t *)write.param[0],write.param[1]);
			memcpy((uint8_t *)write.param[0],de->name,de->namelen);
			write.param[1] += de->namelen;
      break;
		}
		memmove((uint8_t *)write.param[0]+de->namelen+1,(uint8_t *)write.param[0],write.param[1]);
		memcpy((uint8_t *)write.param[0],"/",1);
		write.param[1]++;
		memcpy((uint8_t *)write.param[0]+1,de->name,de->namelen);
		write.param[1] += de->namelen;
    de=de->parent;
  }

}

static int redirect_proc_read(struct seq_file *s, void *v)
{
	int i;
	uint8_t *ptr;
	write.param[1]=0;
	if( proc_flag == 0){
		int ret;
		memset((uint32_t *)write.param[0],0,ICC_MAX_PROC_SIZE);
		get_proc_entry();
		TRACE(PROC, DBG_LEVEL_LOW, ("procfs entry is %s\n",(uint8_t *)write.param[0]));
		cache_wb_inv(write.param[0],ICC_MAX_PROC_SIZE);
		icc_write(PR,&write);	

		/*Wait for a max of 1 sec, HZ is no of jiffies in one sec*/
		ret=wait_event_interruptible_timeout(proc_wakeuplist,proc_wakeup != 0,HZ);
		/*if ret ==0 means, condition is false even after timeout of 1 sec*/
		if(ret == 0)
		{
			seq_printf(s,"Bootcore not running or not responding\n");
			return 0;
		}

		proc_wakeup=0;
		if( rw.param[1] > PROC_SIZE ){
				if(rw.param[1] %  PROC_SIZE == 0 )
					proc_flag = rw.param[1] >> PROC_SHIFT ;
				else
					proc_flag = ( rw.param[1] >> PROC_SHIFT ) + 1;
		}
	}
	if(	proc_flag > 0 )
		proc_flag--;
	ptr=(uint8_t *)rw.param[0];
	for(i=0;i<rw.param[1];i++)
		seq_printf(s, "%c",ptr[i]);
	return 0;
}

/*
 * Write to perf counter registers based on text input
 */

#define TXTBUFSZ 1000

static ssize_t bootcore_proc_write(struct file *file, const char __user *buffer, 
				size_t count, loff_t *data1)
{
	size_t len = 0;
	write.param[1]=0;
	memset((uint32_t *)write.param[0],0,ICC_MAX_PROC_SIZE);
	get_proc_entry();
	write.param[1]++;
	TRACE(PROC, DBG_LEVEL_LOW, ("procfs entry is %s\n",(uint8_t *)write.param[0]));
		if(count >= TXTBUFSZ) len = TXTBUFSZ-1;
	else len = count;
	if(copy_from_user((uint8_t *)write.param[0]+write.param[1], buffer, len)){ return -EFAULT;}
  write.param[1] += len;	
	cache_wb_inv(write.param[0],ICC_MAX_PROC_SIZE);
	icc_write(PR,&write);	
	wait_event_interruptible(proc_wakeuplist,proc_wakeup != 0);
	proc_wakeup=0;
	return (len);
}

static int bootcore_proc_open(struct inode *inode, struct file *file)
{
 return single_open(file,redirect_proc_read, NULL);
}
static const struct file_operations bootcore_proc_fops = {
 .open           = bootcore_proc_open,
 .read           = seq_read,
 .write    			 = bootcore_proc_write,
 .llseek         = seq_lseek,
 .release        = single_release,
};


void pfn_ltq_proc_callback(icc_wake_type wake_type){
  int ret;
	if( (wake_type & ICC_READ) )
	{
  	memset(&rw,0,sizeof(icc_msg_t));
  	ret=icc_read(PR,&rw);
  	if(ret>0){
			cache_inv(rw.param[0],rw.param[1]);
			proc_wakeup=1;
			wake_up_interruptible(&proc_wakeuplist);
		}else{
			TRACE(PROC, DBG_LEVEL_HIGH, ("Read from ICC failed\n"));
		}
	}
}
static int __init init_bootcore_proc(void)
{
	int result=0;
  result=icc_open((struct inode *)PR,NULL);
  if(result<0){
    TRACE(PROC, DBG_LEVEL_HIGH, ("open ICC Failed\n"));
    return result;
  }
	result=icc_register_callback(PR,&pfn_ltq_proc_callback);
 if(result<0){
    TRACE(PROC, DBG_LEVEL_HIGH, ("CallBack Register with ICC Failed\n"));
    return result;
  }
	bootcore_proc_dir = proc_mkdir ("bootcore",NULL);
	create_procredirect_entry();
	write.param[0]=(uint32_t)kmalloc(ICC_MAX_PROC_SIZE,GFP_KERNEL|GFP_DMA);
	init_waitqueue_head(&proc_wakeuplist);
	write.src_client_id=write.dst_client_id=PR;
	write.param_attr=ICC_PARAM_PTR;
	return 0;
}
static struct proc_dir_entry * check_directory(struct proc_dir_entry *dir, char *s){
	struct proc_dir_entry *tmp;
	for (tmp = dir->subdir; tmp; tmp = tmp->next)
	if (strcmp(tmp->name,s) == 0) {
      TRACE(PROC, DBG_LEVEL_HIGH, ("proc_dir_entry '%s/%s' already registered\n",
                                dir->name,s));
      return tmp;
  }
	return NULL;
}
static int parse_proc_entry(char *s){
	int i;
	i=0;
	while(*s != 0){
		if(*s=='/'){
			i++;
			*s=0;
		}
		s++;
	}
	return i;
}
static int create_procredirect_entry(){
	int i,j;
	struct proc_dir_entry *tmp,*tmp1;
	char *ptr;
	i=0;

	while(i<9){
		ptr=a[i];
		j=parse_proc_entry(ptr);
		tmp=bootcore_proc_dir;
		if(j > 0){
			while(j >= 1){
				tmp1=tmp;
				tmp=check_directory(tmp,ptr);
				if(tmp == NULL){
					tmp=proc_mkdir(ptr,tmp1);
					if( tmp == NULL ){
						return 0;
					}
				}
				ptr += strlen(ptr)+1;
				j--;
			}
		}
		if(check_directory(tmp,ptr)==NULL){		
			proc_create(ptr, 0644,tmp,&bootcore_proc_fops);
		}
		
		i++;
	}
	return 0;
}


MODULE_LICENSE("GPL");
/* Automatically create the entry */
module_init(init_bootcore_proc);
