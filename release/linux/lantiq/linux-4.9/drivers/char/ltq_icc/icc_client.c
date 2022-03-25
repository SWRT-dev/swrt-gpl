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

extern uint32_t BLOCK_MSG[MAX_CLIENT];
extern void process_icc_message(unsigned int uiClientId);

void pfn_icc_client_callback(icc_wake_type wake_type){
  int ret;
	icc_msg_t rw;
	if( (wake_type & ICC_READ) )
	{
  	memset(&rw,0,sizeof(icc_msg_t));
  	ret=icc_read(ICC_Client,&rw);
  	if(ret>0){
			/*message from self*/
			if(rw.src_client_id != ICC_Client && rw.msg_id == ICC_MSG_FLOW_CONTROL)
			{
				rw.src_client_id = ICC_Client;
				icc_write(ICC_Client,&rw);
			}
			else if(rw.msg_id == ICC_MSG_FLOW_CONTROL)/*message from peer*/
			{
				/*Flow control OFF for the client and wake it up*/
				if(rw.param[0]){
					BLOCK_MSG[rw.param[1]] = 0;
					process_icc_message(rw.param[1]);
				}else{/*Flow control turn on for the client*/
					BLOCK_MSG[rw.param[1]] = 1;
				}
			}else{
				printk("Not a valid message for icc client\n");
			}
		}else{
			printk("Read from ICC failed\n");
		}
	}
}
static int __init init_icc_client(void)
{
	int result=0;
  result=icc_open((struct inode *)ICC_Client,NULL);
  if(result<0){
    printk("open ICC client  Failed\n");
    return result;
  }
	result=icc_register_callback(ICC_Client,&pfn_icc_client_callback);
 if(result<0){
    printk("Call back register for ICC client  Failed\n");
    return result;
  }
	return 0;
}
MODULE_LICENSE("GPL");
/* Automatically create the entry */
module_init(init_icc_client);
