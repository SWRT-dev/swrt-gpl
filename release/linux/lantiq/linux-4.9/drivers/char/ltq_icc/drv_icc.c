/******************************************************************************
**
** FILE NAME    : ICC_device.c
** PROJECT      : GRX500
** MODULES      : ICC
**
** DATE         : 10 MAY 2014
** AUTHOR       : Swaroop Sarma
** DESCRIPTION  : ICC module
** COPYRIGHT    :   Copyright (c) 2006
**      Lantiq Communications AG
**      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**   Any use of this software is subject to the conclusion of a respective
**   License agreement. Without such a License agreement no rights to the
**   software are granted
**
** HISTORY
** $Date        $Author  $Comment
*******************************************************************************/

/* Group definitions for Doxygen */
/** \addtogroup API API-Functions */
/** \addtogroup Internal Internally used functions */
/*linux kernel headers*/
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/vmalloc.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/sem.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#include <linux/mm.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
/*asm header files*/
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/unistd.h>
#include <asm/cacheflush.h>     // flush_cache_range
/*icc header file*/
#include <linux/icc/drv_icc.h>
/****************************Local definitions**************************/
#define FIFO_GET_READ_PTR(index)  (ICC_BUFFER[index].fifo_read)
#define FIFO_GET_WRITE_PTR(index) (ICC_BUFFER[index].fifo_write)
#define GET_ICC_READ_MSG(clientid ,readptr)	  (ICC_BUFFER[clientid].MPS_BUFFER[readptr])
#define GET_ICC_WRITE_MSG(clientid , wrptr)	  (ICC_BUFFER[clientid].MPS_BUFFER[wrptr])
CREATE_TRACE_GROUP(ICC);
/********************************Local functions*************************/

#ifndef  __LIBRARY__
unsigned int icc_poll(struct file *file_p, poll_table *wait);
int icc_read_d(struct file *file_p, char *buf, size_t count, loff_t *ppos);
int icc_write_d(struct file *file_p, char *buf, size_t count, loff_t *ppos);
int icc_mmap(struct file *file, struct vm_area_struct *vma);
long icc_ioctl (struct file *file_p,uint32_t nCmd, unsigned long arg);

/******************************Global variable declaration***************/
#ifndef KTHREAD
typedef struct {
  struct work_struct icc_work;
  int    x;
}icc_t;
#endif
typedef struct{
	uint32_t fifo_write;
	uint32_t fifo_read;
	mps_message MPS_BUFFER[MAX_DEPTH];
}icc_fifo_t;

typedef struct{
	uint32_t address[MAX_MMAP];
	uint32_t virtual_addr[MAX_MMAP];
	uint32_t count;	
}mmap_addr_t;

static short icc_major_id = 0;
#ifdef CONFIG_SOC_TYPE_GRX500_TEP
static struct device *icc_char_dev;
static struct class *icc_char_class;
const char icc_dev_name[] = "ltq_icc";
#endif
/*Variable used*/
#ifdef KTHREAD
unsigned int g_num;
static struct task_struct *icc_kthread[MAX_CLIENT];
#else
static struct workqueue_struct *icc_wq[MAX_CLIENT];
void icc_wq_function(struct work_struct *work);
#endif
int icc_excpt[MAX_CLIENT];
icc_dev iccdev[MAX_CLIENT];
uint32_t BLOCK_MSG[MAX_CLIENT];
mmap_addr_t mmap_address[MAX_CLIENT];
icc_fifo_t ICC_BUFFER[MAX_CLIENT];
#ifdef KTHREAD
static struct semaphore icc_callback_sem;
#endif
static DEFINE_SPINLOCK(icc_sync_lock);


#ifdef CONFIG_SOC_TYPE_GRX500_TEP
/* the driver callbacks */
static struct file_operations icc_fops = {
 owner:THIS_MODULE,
 poll:icc_poll,
 read:icc_read_d,
 write:(void *)icc_write_d,
 mmap:icc_mmap,
 unlocked_ioctl:icc_ioctl,
 open:icc_open,
 release:icc_close
};
#endif

/*Local functions*/
static void  FIFO_INC_WRITE_PTR(uint32_t index) 
{	
	if(ICC_BUFFER[index].fifo_write == MAX_DEPTH-1) 
		ICC_BUFFER[index].fifo_write=0; 
	else 
		ICC_BUFFER[index].fifo_write++; 
}
static void FIFO_INC_READ_PTR(uint32_t index) 
{
	if(ICC_BUFFER[index].fifo_read == MAX_DEPTH-1) 
		ICC_BUFFER[index].fifo_read=0; 
	else 
		ICC_BUFFER[index].fifo_read++; 
}
static int FIFO_NOT_EMPTY(uint32_t index) 		
{	
	if((ICC_BUFFER[index].fifo_read == ICC_BUFFER[index].fifo_write)) 
		return 0; 
	else 
		return 1; 
}

static int FIFO_AVAILABLE(uint32_t index) 		
{ 
	if(ICC_BUFFER[index].fifo_read <= ICC_BUFFER[index].fifo_write)
		return ((ICC_BUFFER[index].fifo_read - ICC_BUFFER[index].fifo_write) + MAX_DEPTH); 
	else 
		return (ICC_BUFFER[index].fifo_read - ICC_BUFFER[index].fifo_write -1); 
}


/*******************************************************************************
Description:
Arguments:
Note:
*******************************************************************************/
void clear_mmap_addr(struct vm_area_struct *vma)
{
	uint32_t i,j,flag;
	flag=0;
	for(i=0;i<MAX_CLIENT;i++)
	{
		for(j=0;j<MAX_MMAP;j++)
		{
			if(mmap_address[i].virtual_addr[j]==(uint32_t)vma->vm_start){
				mmap_address[i].address[j]=0;
				mmap_address[i].virtual_addr[j]=0;
				mmap_address[i].count--;
				flag=1;
				break;
			}
		}
		if(flag==1)
		{
			break;
		}
	}
}

uint32_t fetch_userto_kernel_addr(int num, uint32_t Param)
{
	uint32_t j;
		for(j=0;j<MAX_MMAP;j++)
		{
			if(mmap_address[num].virtual_addr[j] == Param){
				return mmap_address[num].address[j];
			}
		}
		return 0xFFFFFFFF;
}


void simple_vma_open(struct vm_area_struct *vma)
{
		vma->vm_pgoff=(uint32_t)mps_buffer.malloc(MEM_SEG_SIZE,0xFF);
		vma->vm_pgoff = __pa(vma->vm_pgoff);
		vma->vm_pgoff = vma->vm_pgoff>>PAGE_SHIFT;
		TRACE(ICC, DBG_LEVEL_LOW, ("mmap without giving the physical address, so allocating memory\n"));
    TRACE(ICC, DBG_LEVEL_LOW, (KERN_NOTICE "Simple VMA open, virt %lx, phys pfn %lx\n",
            vma->vm_start, vma->vm_pgoff));
}

void simple_vma_close(struct vm_area_struct *vma)
{
			clear_mmap_addr(vma);
			mps_buffer.free((void *)__va(vma->vm_pgoff<<PAGE_SHIFT));
			TRACE(ICC, DBG_LEVEL_LOW, ("freeing the allocated memory on munmap, if allocated previously"));
    	TRACE(ICC, DBG_LEVEL_LOW, (KERN_NOTICE "Simple VMA close.\n"));
}

static struct vm_operations_struct simple_remap_vm_ops = {
    .open =  simple_vma_open,
    .close = simple_vma_close,
};

#ifndef SYSTEM_4KEC
void complex_vma_open(struct vm_area_struct *vma)
{
			vma->vm_pgoff=(uint32_t)alloc_pages_exact(vma->vm_end - vma->vm_start,GFP_KERNEL|GFP_DMA);
			vma->vm_pgoff = __pa(vma->vm_pgoff);
			vma->vm_pgoff = vma->vm_pgoff>>PAGE_SHIFT;
			TRACE(ICC, DBG_LEVEL_LOW, ("mmap without giving the physical address, so allocating memory\n"));
	
    	TRACE(ICC, DBG_LEVEL_LOW, (KERN_NOTICE "Complex VMA open, virt %lx, phys pfn %lx\n",
            vma->vm_start, vma->vm_pgoff));
}

void complex_vma_close(struct vm_area_struct *vma)
{
			clear_mmap_addr(vma);
			free_pages_exact((void *)(__va(vma->vm_pgoff<<PAGE_SHIFT)),vma->vm_end - vma->vm_start);
			TRACE(ICC, DBG_LEVEL_LOW, ("freeing the allocated memory on munmap, if allocated previously"));
    	TRACE(ICC, DBG_LEVEL_LOW, (KERN_NOTICE "Complex VMA close.\n"));
}

static struct vm_operations_struct complex_remap_vm_ops = {
    .open =  complex_vma_open,
    .close = complex_vma_close,
};
#endif

void generic_vma_open(struct vm_area_struct *vma)
{
    	TRACE(ICC, DBG_LEVEL_LOW, (KERN_NOTICE "Generic VMA open\n"));
}

void generic_vma_close(struct vm_area_struct *vma)
{
			clear_mmap_addr(vma);
    	TRACE(ICC, DBG_LEVEL_LOW, (KERN_NOTICE "Generic VMA close.\n"));
}


static struct vm_operations_struct generic_remap_vm_ops = {
    .open =  generic_vma_open,
    .close = generic_vma_close,
};


int icc_mmap(struct file *file, struct vm_area_struct *vma)
{
  size_t size = vma->vm_end - vma->vm_start;
	unsigned long offset;	
	uint32_t num,i;
	offset=vma->vm_pgoff;

	num=(int)file->private_data;
	if(mmap_address[num].count >= MAX_MMAP){
		TRACE(ICC, DBG_LEVEL_HIGH, ("MAX Count %d exceeded for mmap\n",MAX_MMAP));
		return -EAGAIN;
	}
	/*For remap pfn range we have to give the page frame number as the third 
	argument*/
	if(offset == 0 )
	{
#ifdef SYSTEM_4KEC
		if(size > MEM_SEG_SIZE){
			TRACE(ICC, DBG_LEVEL_HIGH, ("Cant allocate from bootcore memory\n"));
			return -EINVAL;
		}
#else
		if(size > MEM_SEG_SIZE){
			vma->vm_ops = &complex_remap_vm_ops;
  		complex_vma_open(vma);
		}else
#endif
		{
			vma->vm_ops = &simple_remap_vm_ops;
  		simple_vma_open(vma);
		}
		offset = vma->vm_pgoff;
	}else{
		vma->vm_ops = &generic_remap_vm_ops;
    generic_vma_open(vma);	
	}

	for(i=0;i<MAX_MMAP;i++){
		if(mmap_address[num].address[i]==0){
			mmap_address[num].address[i]=(uint32_t)__va(vma->vm_pgoff<<PAGE_SHIFT);
			mmap_address[num].virtual_addr[i]=(uint32_t)vma->vm_start;
			break;
		}
	}
	mmap_address[num].count++;
#ifdef SYSTEM_4KEC
	/*Always uncached access from 4kec, because of caching complexities since cache is VIPT*/
	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);
#endif

  /* Remap-pfn-range will mark the range VM_IO */
  if (remap_pfn_range(vma,
          vma->vm_start,
          offset,
          size,
          vma->vm_page_prot)) {
		TRACE(ICC, DBG_LEVEL_HIGH, ("remap failed\n"));
    return -EAGAIN;
  }
  return 0;
}
#endif/*__LIBRARY__*/
void mps_icc_fill(mps_message *pMpsmsg, icc_msg_t *rw){
	rw->src_client_id=pMpsmsg->header.Hd.src_id;
	rw->dst_client_id=pMpsmsg->header.Hd.dst_id;
	rw->msg_id=pMpsmsg->header.Hd.msg_id;
	rw->param_attr=pMpsmsg->header.Hd.param_attr;
	memcpy(rw->param,pMpsmsg->data,sizeof(uint32_t)*MAX_UPSTRM_DATAWORDS);
}

void icc_mps_fill(icc_msg_t *rw,mps_message *pMpsmsg){
	pMpsmsg->header.Hd.src_id=rw->src_client_id;
	pMpsmsg->header.Hd.dst_id=rw->dst_client_id;
	pMpsmsg->header.Hd.msg_id=rw->msg_id;
	pMpsmsg->header.Hd.param_attr=rw->param_attr;
	memcpy(pMpsmsg->data,rw->param,sizeof(uint32_t)*MAX_UPSTRM_DATAWORDS);
}
#ifndef __LIBRARY__
/*API for read*/
int icc_read(icc_devices icdev,icc_msg_t *rw)
{
	int num=(int)icdev;
	int readptr;
	mps_message *pMpsmsg;
	icc_msg_t icc_msg;
	if((num >= MAX_CLIENT) || (!FIFO_NOT_EMPTY(num)))/*fifo is empty*/
		return -EFAULT;
	readptr=FIFO_GET_READ_PTR(num);
	FIFO_INC_READ_PTR(num);
	pMpsmsg=&GET_ICC_READ_MSG(num,readptr);
	/*Convert data from MPS to ICC*/
	mps_icc_fill(pMpsmsg,rw);
	/*memset the global structure*/
	memset(pMpsmsg,0,sizeof(mps_message));
	if(FIFO_AVAILABLE(num) >= MAX_THRESHOLD && BLOCK_MSG[num] == 1){
		memset(&icc_msg,0,sizeof(icc_msg_t));
		icc_msg.src_client_id=icc_msg.dst_client_id=ICC_Client;
		icc_msg.msg_id = ICC_MSG_FLOW_CONTROL;
		icc_msg.param[0] = 0;
		icc_msg.param[1] = num;
		icc_write(ICC_Client,&icc_msg);
	}
	return sizeof(icc_msg_t);
}


/*ioctl functions supported by icc*/
long icc_ioctl (struct file *file_p,uint32_t nCmd, unsigned long arg){
		int num,i;
		icc_commit_t icc_address;
		switch(nCmd){
			case ICC_IOC_REG_CLIENT:
					num=(int)arg;
				/*check for validity of client Id*/
	 			if(num>=0 && num < (MAX_CLIENT)){
				/*check if its already opened*/
		 			if(iccdev[num].Installed==1){
     				TRACE(ICC, DBG_LEVEL_HIGH, (" Device %d is already open!\n", num));
						return -EMFILE;
		 			}
		 			/*mark that the device is opened in global structure
					to avoid further open of the device*/
		 			iccdev[num].Installed=1;
#if defined(CONFIG_SOC_GRX500_BOOTCORE) || defined(CONFIG_SOC_PRX300_BOOTCORE)
					/*Intimate to IAP that bootcore is ready if sec services gets registered*/
					if(num == SEC_SERVICE)
					{
						mps_message Mpsmsg;

						memset(&Mpsmsg,0,sizeof(mps_message));
						Mpsmsg.header.Hd.src_id = Mpsmsg.header.Hd.dst_id = 0;
  					Mpsmsg.header.Hd.msg_id = ICC_BOOTCORE_UP;
						/*write to the mps mailbox*/
    				mps_write_mailbox(&Mpsmsg);
					}
#endif
		 			/* Initialize a wait_queue list for the system poll/wait_event function. */
  	 			init_waitqueue_head(&iccdev[num].wakeuplist);
					file_p->private_data = (void *)num;
				}
				break;
			case ICC_IOC_MEM_INVALIDATE:
			case ICC_IOC_MEM_COMMIT:
				num = (int)file_p->private_data;
				/*
				 * Initialize destination and copy mps_message
				 * from usermode
				 */
				memset(&icc_address, 0, sizeof(icc_commit_t));
				if (copy_from_user(&icc_address,
					(void *)arg,
					sizeof(icc_commit_t)) != 0) {
					TRACE(ICC, DBG_LEVEL_HIGH, ("[%s %s %d] copy_from_user error\n",
						__FILE__, __func__, __LINE__));
					return -EFAULT;
				}
				for(i=0;i<icc_address.count;i++){
					uint32_t mmap_addr;

					mmap_addr=fetch_userto_kernel_addr(num,icc_address.address[i]);

					if (mmap_addr == 0xFFFFFFFF){
						mmap_addr = icc_address.address[i];
					}

					icc_address.address[i] = mmap_addr+icc_address.offset[i];
					if(nCmd == ICC_IOC_MEM_COMMIT)
						cache_wb_inv(icc_address.address[i],icc_address.length[i]);
					else
						cache_inv(icc_address.address[i],icc_address.length[i]);
				}
				
				break;
		}
		return 0;
}
/*read call back function registered with driver */
int icc_read_d(struct file *file_p, char *buf, size_t count, loff_t *ppos)
{
	int num=(int)file_p->private_data;/*Accessing the stored client number 
																		in file data*/
	icc_msg_t icc_msg;
	mps_message *pMpsmsg;
	int readptr;

	if(!FIFO_NOT_EMPTY(num))/*fifo is empty*/
		return -EFAULT;

	readptr=FIFO_GET_READ_PTR(num);
	FIFO_INC_READ_PTR(num);
	pMpsmsg=&GET_ICC_READ_MSG(num,readptr);
	memset(&icc_msg,0,sizeof(icc_msg_t));
	/*Convert data from MPS to ICC*/
		mps_icc_fill(pMpsmsg,&icc_msg);
	if (copy_to_user(buf, &icc_msg, sizeof(icc_msg_t)) != 0)
  {
		TRACE(ICC, DBG_LEVEL_HIGH, ("[%s %s %d]: copy_to_user error\r\n",__FILE__, __func__, __LINE__));
		return -EAGAIN;
  }
	/*memset the global structure*/
	memset(pMpsmsg,0,sizeof(mps_message));
	if(FIFO_AVAILABLE(num) >= MAX_THRESHOLD && BLOCK_MSG[num] == 1){
		memset(&icc_msg,0,sizeof(icc_msg_t));
		icc_msg.src_client_id=icc_msg.dst_client_id=ICC_Client;
		icc_msg.msg_id = ICC_MSG_FLOW_CONTROL;
		icc_msg.param[0] = 0;
		icc_msg.param[1] = num;
		icc_write(ICC_Client,&icc_msg);
	}

	return sizeof(icc_msg_t);
}
#endif
/*******************************************************************************
Description:
Arguments:
Note:
*******************************************************************************/
int icc_write(icc_devices icdev, icc_msg_t *buf)
{
	mps_message Mpsmsg;
	int ret;
	memset(&Mpsmsg,0,sizeof(mps_message));
	/*Fill in the mps data structure*/
	icc_mps_fill(buf,&Mpsmsg);
	{
		/*write to the mps mailbox*/
		ret=mps_write_mailbox(&Mpsmsg);
		if(ret==-EIO)
		{
			TRACE(ICC, DBG_LEVEL_HIGH,("Peer side not responding, needs reboot\n"));
			BLOCK_MSG[buf->src_client_id] = 1;
		}
		/*release the semaphore of data mailbox after writing*/
	}
	if(ret < 0)
		return ret;
	else
  	return sizeof(icc_msg_t);
}

void process_icc_message(unsigned int uiClientId)
{
#ifndef KTHREAD
	icc_t *icc_workq;
	struct work_struct *wq;
#endif
			/*waking up the sleeping process on wait queue*/
			icc_excpt[uiClientId] = 1;
			wake_up_interruptible(&iccdev[uiClientId].wakeuplist);
#ifndef KTHREAD
			if (icc_wq[uiClientId]) {
				icc_workq = kmalloc(sizeof(icc_t), GFP_ATOMIC);
				if (icc_workq) {
					wq = (struct work_struct *)icc_workq;
					INIT_WORK(wq, icc_wq_function);
					icc_workq->x = uiClientId;
					queue_work(icc_wq[uiClientId], wq);
				} else {
					TRACE(ICC, DBG_LEVEL_HIGH,
						("cant schedule icc wq %d\n",
						uiClientId));
					return;
				}
			}
#endif
}


#ifdef CONFIG_REGMAP_ICC
static int icc_sync_write(icc_msg_t *buf, icc_msg_t *retbuf);

/* regmap write function */
int icc_regmap_sync_write(icc_devices ClientId,
				phys_addr_t paddr, unsigned int val)
{
	icc_msg_t retmsg = {0}, msg = {0};
	int ret;
	unsigned long flags;

	/* if we assume irqsave already at top, we may hit up issues */
	/* dont allow anything to run on this CPU */
	local_irq_save(flags);
	/* disable mps irq , as it may hit the other CPU */
	mps_disable_irq();

	msg.src_client_id = ClientId;
	msg.dst_client_id = ClientId;
	msg.msg_id = REGMAP_WR_MSGID;
	msg.param[0] = (uint32_t)paddr;
	msg.param[1] = val;

	ret = icc_sync_write(&msg, &retmsg);

	/* enable irq  */
	mps_enable_irq();
	local_irq_restore(flags);
	return ret;
}
EXPORT_SYMBOL(icc_regmap_sync_write);

/* regmap read function */
int icc_regmap_sync_read(icc_devices ClientId,
				phys_addr_t paddr, unsigned int *val)
{
	icc_msg_t retmsg = {0}, msg = {0};
	unsigned long flags;
	int ret;

	if (!val)
		return -EINVAL;

	/* if we assume irqsave already at top, we may hit up issues */
	/* dont allow anything to run on this CPU */
	local_irq_save(flags);
	/* disable mps irq , as it may hit the other CPU */
	mps_disable_irq();

	msg.src_client_id = ClientId;
	msg.dst_client_id = ClientId;
	msg.msg_id = REGMAP_RD_MSGID;
	msg.param[0] = (uint32_t)paddr;

	ret = icc_sync_write(&msg, &retmsg);
	if (!retmsg.param[2])
		*val = retmsg.param[1];

	/* enable irq  */
	mps_enable_irq();
	local_irq_restore(flags);
	return ret;
}
EXPORT_SYMBOL(icc_regmap_sync_read);

/* regmap sync function */
static int icc_sync_write(icc_msg_t *buf, icc_msg_t *ret_buf)
{
	int ret = 0;
	unsigned int lock_try_count = 0xFFFFF;
	unsigned int mailbox_count = 0xFFFFF;
	unsigned int read_count = 0;
	mps_message mps_buf, *mps_ptr;
	unsigned int uiClientId, wrptr;


	uiClientId = buf->src_client_id;
	if (uiClientId < MAX_CLIENT && iccdev[uiClientId].Installed == 0)
		icc_open((struct inode *)uiClientId, NULL);

	memset(ret_buf, 0, sizeof(icc_msg_t));
	ret_buf->param[2] = 1;

	/* before writing flush the messages on this client */
	while (read_count < 32 && icc_read(buf->src_client_id, ret_buf) > 0) {
		TRACE(ICC, DBG_LEVEL_HIGH, ("some invalid messages in pipe\n"));
		memset(ret_buf, 0, sizeof(icc_msg_t));
		read_count++;
	}
	if (read_count == 32) {
		TRACE(ICC, DBG_LEVEL_HIGH, ("Is TEP compromised !!!!!!\n"));
		return -EINVAL;
	}

	ret = icc_write(buf->src_client_id, buf);
	if (ret < 0) {
		TRACE(ICC, DBG_LEVEL_HIGH, ("icc write failed\n"));
		return ret;
	}

	/* Try for spin lock till count becomes 0 */
	while (lock_try_count) {
		if (spin_trylock(&icc_sync_lock)) {
			/* disable mps irq , as it might have been enabled
			 * from another CPU
			 */
			mps_disable_irq();

			memset(ret_buf, 0, sizeof(icc_msg_t));
			/* after spinlock check in fifo for safety */
			if (icc_read(buf->src_client_id, ret_buf) > 0) {
				spin_unlock(&icc_sync_lock);
				break;
			}

			/* read out MAX_MSG and we dont find our response or
			 * TEP is not alive break from loop
			 */
			read_count = 0;
			while (read_count < 32) {
				read_count++;
				memset(&mps_buf, 0, sizeof(mps_message));
				/* check for mail box content availability */
				while (mailbox_count &&
					mps_read_mailbox(&mps_buf) != 0)
					mailbox_count--;

				if (mailbox_count == 0) {
					TRACE(ICC, DBG_LEVEL_HIGH,
						("TEP not responding\n"));
					spin_unlock(&icc_sync_lock);
					return -EBUSY;
				}
				/* reset count as TEP is alive */
				mailbox_count = 0xFFFFF;
				uiClientId = mps_buf.header.Hd.dst_id;
				TRACE(ICC, DBG_LEVEL_LOW,
					("command type %x client Id is %d\n",
					mps_buf.header.val, uiClientId));
				if (uiClientId >= MAX_CLIENT ||
					iccdev[uiClientId].Installed == 0) {
					TRACE(ICC, DBG_LEVEL_HIGH,
						("client[%d] not opened yet\n",
						uiClientId));
					continue;
				}
				/* else case is not for sync clients as there
				 * will be only one in q put it to client
				 * and read from there or wakeup
				 */
				if (FIFO_AVAILABLE(uiClientId) > 0) {
					wrptr = FIFO_GET_WRITE_PTR(uiClientId);
					FIFO_INC_WRITE_PTR(uiClientId);
					mps_ptr = &GET_ICC_WRITE_MSG(uiClientId,
							wrptr);
					memcpy(mps_ptr, &mps_buf,
						sizeof(mps_message));
				} else {
					TRACE(ICC, DBG_LEVEL_HIGH,
						("Fifo full  for client %d, Dropping\n",
						uiClientId));
				}

				/* same client read and break,else wakeup */
				if (uiClientId == buf->src_client_id) {
					memset(ret_buf, 0, sizeof(icc_msg_t));
					icc_read(buf->src_client_id, ret_buf);
					spin_unlock(&icc_sync_lock);
					return 0;
				}
				process_icc_message(uiClientId);
			}
			TRACE(ICC, DBG_LEVEL_HIGH,
				("client[%u] not responding\n", uiClientId));
			spin_unlock(&icc_sync_lock);
			return -EBUSY;
		}
		memset(ret_buf, 0, sizeof(icc_msg_t));
		if (icc_read(buf->src_client_id, ret_buf) > 0)
			break;
		lock_try_count--;
	}
	if (lock_try_count == 0) {
		TRACE(ICC, DBG_LEVEL_HIGH, ("icc sync spin lock get failed\n"));
		ret_buf->param[2] = 1;
		return -EBUSY;
	}
	return 0;
}
#endif

#ifndef __LIBRARY__
/*write call back function of the driver*/
int icc_write_d(struct file *file_p, char *buf, size_t count, loff_t *ppos)
{
	mps_message Mpsmsg;
	icc_msg_t icc_msg;
	int ret , i , num;
	memset(&Mpsmsg,0,sizeof(mps_message));
	memset(&icc_msg,0,sizeof(icc_msg_t));
	if (copy_from_user(&icc_msg, buf, sizeof(icc_msg_t)) != 0)
  {
		TRACE(ICC, DBG_LEVEL_HIGH, ("[%s %s %d]: copy_from_user error\r\n",__FILE__, __func__, __LINE__));
		return -EAGAIN;
  }
	num=(int)file_p->private_data;
	for(i=0;i<MAX_UPSTRM_DATAWORDS;i++){
		if(icc_msg.param[i]!=0 && CHECK_PTR(icc_msg.param_attr,i) && 
				!(CHECK_PTR_IOCU(icc_msg.param_attr,i))){
				uint32_t mmap_addr;
				mmap_addr=fetch_userto_kernel_addr(num,icc_msg.param[i]);
				if(mmap_addr != 0xFFFFFFFF){
					icc_msg.param[i] = mmap_addr;
					TRACE(ICC, DBG_LEVEL_NORMAL, ("user pointer maintained by driver\n"));
				}
		}
	}
	/*Fill in the mps data structure*/
	icc_mps_fill(&icc_msg,&Mpsmsg);
	{
		/*write to mps mailbox*/
		ret=mps_write_mailbox(&Mpsmsg);
		if(ret==-EIO)
		{
			TRACE(ICC, DBG_LEVEL_HIGH,("Peer side not responding, needs reboot\n"));
			BLOCK_MSG[icc_msg.src_client_id] = 1;
		}
	}
	if(ret<0)
		return ret;
	else
  	return sizeof(icc_msg_t);
}

/*poll function handler of the driver*/
unsigned int icc_poll(struct file *file_p, poll_table *wait){
  int ret = 0;
	int num=(int)file_p->private_data;
	icc_dev *pIccdev;
	pIccdev=&iccdev[num];
  /* install the poll queues of events to poll on */
  poll_wait(file_p, &pIccdev->wakeuplist, wait);
	/*If exception flag is set unlock bot read and write fd*/
	if(icc_excpt[num]){
		if(FIFO_NOT_EMPTY(num))
			ret |= ( POLLIN | POLLRDNORM );
		if(BLOCK_MSG[num] == 0)
			ret |= (POLLOUT | POLLWRNORM);
		icc_excpt[num]=0;
		return ret;
	}
	return ret;
}

#ifdef CONFIG_SOC_TYPE_GRX500_TEP
/*register the char region with the linux*/
int icc_os_register(void)
{
	int ret = 0;

	ret = register_chrdev(icc_major_id, icc_dev_name, &icc_fops);
	if (ret < 0) {
		TRACE(ICC, DBG_LEVEL_HIGH, ("Not able to register chrdev\n"));
		return ret;
	} else if (icc_major_id == 0) {
		icc_major_id = ret;

		icc_char_class = class_create(THIS_MODULE, icc_dev_name);
		if (IS_ERR(icc_char_class)) {
			TRACE(ICC, DBG_LEVEL_HIGH, ("failed to create device class\n"));
			unregister_chrdev(icc_major_id, icc_dev_name);
			return PTR_ERR(icc_char_class);
		}

		icc_char_dev = device_create(icc_char_class, NULL,
					     MKDEV(icc_major_id, 0),
					     NULL, "%s", icc_dev_name);
		if (IS_ERR(icc_char_dev)) {
			TRACE(ICC, DBG_LEVEL_HIGH, ("failed to create device\n"));
			class_destroy(icc_char_class);
			unregister_chrdev(icc_major_id, icc_dev_name);
			return PTR_ERR(icc_char_dev);
		}
	}
	TRACE(ICC, DBG_LEVEL_HIGH, ("Major Id is %d\n", icc_major_id));
   	TRACE(ICC, DBG_LEVEL_HIGH, ("ICC driver registered\n"));
	return 0;
}

/**
   This function unregisters char device from kernel.
*/
void icc_os_unregister (void)
{
	 /*unregister the driver region completely*/
	 device_unregister(icc_char_dev);
	 class_destroy(icc_char_class);
   unregister_chrdev(icc_major_id, icc_dev_name);
	 TRACE(ICC, DBG_LEVEL_HIGH, ("ICC driver un-registered\n"));
}
#else
static int icc_os_register(void)
{
	return 0;
}
static void icc_os_unregister(void)
{
}
#endif /* CONFIG_SOC_TYPE_GRX500_TEP */

/*Global Functions*/
/**
 * Open ICC device.
 * Open the device from user mode (e.g. application) or kernel mode. An inode
 * value of 1..MAX_CLIENT-1 indicates a kernel mode access. In such a case the inode value
 * is used as minor ID.
 *
 * \param   inode   Pointer to device inode
 * \param   file_p  Pointer to file descriptor
 * \return  0       device opened
 * \return  EMFILE  Device already open
 * \ingroup API
 */
int icc_open (struct inode * inode, struct file * file_p)
{
   int from_kernel = 0;
	 int num;
   /* a trick: if inode value is
      [1..MAX_CLIENT-1], then we make sure that we are calling from
      kernel space. */
   if (((int) inode >= 0) &&
       ((int) inode < (MAX_CLIENT)))
   {
      from_kernel = 1;
      num = (int) inode;
   }
   else
   {
      return 0;        /* the real device */
   }
	if (iccdev[num].Installed == 1){
		TRACE(ICC, DBG_LEVEL_HIGH,
			(" Device %d is already open!\n", num));
			return -EMFILE;
	}
	iccdev[num].Installed = 1;
	init_waitqueue_head(&iccdev[num].wakeuplist);
	return 0;
}


/**
 * Close ICC device.
 * Close the device from user mode (e.g. application) or kernel mode. An inode
 * value of 1..MAX_CLIENT-1 indicates a kernel mode access. In such a case the inode value
 * is used as minor ID.
 *
 * \param   inode   Pointer to device inode
 * \param   file_p  Pointer to file descriptor

 * \return  0       device closed
 * \return  ENODEV  Device invalid
 * \return  EINVAL  Invalid minor ID
 * \ingroup API
 */
int icc_close(struct inode * inode, struct file * file_p){
   int from_kernel = 0;
   int num;
   /* a trick: if inode value is
      [1..MAX_CLIENT-1], then we make sure that we are calling from
      kernel space. */
   if (((int) inode > 0) &&
       ((int) inode <= (MAX_CLIENT-1)))
   {
      from_kernel = 1;
			num = (int) inode;
	 }else
   {
			num=(int)file_p->private_data;        /* the real device */
   }
	/*check for valid client Id and whether it is installed already or not*/
	 if(num>=0&&num<(MAX_CLIENT)){
		if(iccdev[num].Installed==1){
			/*Flush all the messages*/
			//memset(&MPS_BUFFER[num],0,sizeof(mps_message));
			/*Mark the global structure that its free to open now*/
			iccdev[num].Installed=0;
			return 0;
		}
	 }

	 TRACE(ICC, DBG_LEVEL_HIGH, ("Invalid device Id %d\n",num+1));
	 return -ENODEV;
}


void pfn_icc_callback(void){
	unsigned int uiClientId, wrptr, process_budget = 128;
	mps_message rw;
	mps_message *mps_msg;

	/* sync apis already holding the lock so just return */
	if (spin_trylock(&icc_sync_lock) == 0)
		return;

	memset(&rw, 0, sizeof(mps_message));
	while (process_budget && mps_read_mailbox(&rw) == 0) {
		process_budget--;
		uiClientId = rw.header.Hd.dst_id;
		TRACE(ICC, DBG_LEVEL_LOW,("command type %x client Id is %d\n",rw.header.val,uiClientId));
		if (uiClientId >= MAX_CLIENT ||
			iccdev[uiClientId].Installed == 0) {
			TRACE(ICC, DBG_LEVEL_HIGH,
				("client[%d] not opened yet\n", uiClientId));
			memset(&rw, 0, sizeof(mps_message));
			continue;
		}
		if(FIFO_AVAILABLE(uiClientId) > 0){
			wrptr=FIFO_GET_WRITE_PTR(uiClientId);
			FIFO_INC_WRITE_PTR(uiClientId);
			memcpy(&GET_ICC_WRITE_MSG(uiClientId,wrptr),&rw,sizeof(mps_message));	
			process_icc_message(uiClientId);
			/*Flow control logic*/
			/*since we cant write in interrupt context schedule it and write*/
			/*To avoid potential problems disable interrupts till we send this message out*/
			if(FIFO_AVAILABLE(uiClientId) <= (MIN_THRESHOLD) && BLOCK_MSG[uiClientId] == 0){
				wrptr=FIFO_GET_WRITE_PTR(ICC_Client);
				FIFO_INC_WRITE_PTR(ICC_Client);
				mps_msg=&GET_ICC_WRITE_MSG(ICC_Client,wrptr);
				memset(mps_msg,0,sizeof(mps_message));
				mps_msg->header.Hd.src_id=uiClientId;
				mps_msg->header.Hd.dst_id=ICC_Client;
				mps_msg->header.Hd.msg_id = ICC_MSG_FLOW_CONTROL;
				mps_msg->data[0] = 1;
				mps_msg->data[1] = uiClientId;
				process_icc_message(ICC_Client);
			}
		}else{
			TRACE(ICC, DBG_LEVEL_HIGH, \
				("Fifo full  for client[%d], Dropping\n",
				uiClientId));
			memset(&rw, 0, sizeof(mps_message));
			continue;
		}
		memset(&rw, 0, sizeof(mps_message));
	}
	spin_unlock(&icc_sync_lock);
	TRACE(ICC, DBG_LEVEL_LOW, ("Process budget is %u\n", process_budget));
}
/*Init module routine*/
int __init icc_init_module (void){
	int result = 0;

	result = mps_open((struct inode *)1, NULL);
	if (result < 0) {
		TRACE(ICC, DBG_LEVEL_HIGH, ("open MPS2 Failed\n"));
		goto finish;
	}
	result = mps_register_callback(&pfn_icc_callback);
	if (result < 0) {
		TRACE(ICC, DBG_LEVEL_HIGH, ("Data CallBack Register with MPS2 Failed\n"));
		goto finish;
	}

	/*Init structures if required*/
	/* register char module in kernel */
	result = icc_os_register();
	if (IS_ENABLED(CONFIG_SOC_TYPE_GRX500_TEP) && result)
		goto finish;
#ifdef KTHREAD
	sema_init(&icc_callback_sem,1);
#endif
	return 0;
finish:
	mps_unregister_callback();
	mps_close((struct inode *)1, NULL);
	return result;
}

/*Exit module routine*/
void __exit icc_cleanup_module (void){
	mps_close((struct inode *)1,NULL);
	mps_unregister_callback();
	icc_os_unregister ();
	return;
}
#ifdef KTHREAD
/*Common thread handler function for the ICC kernel threads*/
static int icc_thread_handler(void *arg)
{
	unsigned int num;
	icc_wake_type wake_type;
/*Take the global variable of thread number into local variable*/
	num=g_num;
/*Release the call back semaphore as other callback can now update thread number*/
 	up(&icc_callback_sem); 
/*check whether the thread has to be stopped*/
	/*if not*/
  while (!kthread_should_stop()) {
		TRACE(ICC, DBG_LEVEL_LOW, ("thread %u going to wait\n",num));
		/*wait for an event to occur and check for exception condition*/
    wait_event_interruptible(iccdev[num].wakeuplist,icc_excpt[num] != 0);
		/*reset exception variable*/
		icc_excpt[num]=0;
		/*check the whether the wake up event is for stopping the thread*/
    if (kthread_should_stop())
      break;
	 /*if not for stopping thread set current state to running and 
			invoke callback*/
		__set_current_state(TASK_RUNNING);	
		TRACE(ICC, DBG_LEVEL_LOW, ("Thread %u woken up\n",num));
    /*calling call back to process data*/
		/*complete functionality inside the call back will now run in 
			kernel thread context*/
		wake_type = ICC_INVALID;
		if(BLOCK_MSG[num] == 0)
			 wake_type |= ICC_WRITE;
		if(FIFO_NOT_EMPTY(num))
			 wake_type |= ICC_READ;
		iccdev[num].up_callback(wake_type);
  }
	/*if thread stop is set, set current state to running and exit 
		from thread*/
  __set_current_state(TASK_RUNNING);

  return 0;
}
#else
void icc_wq_function(struct work_struct *work){
	icc_t *icc_work = (icc_t *)work;
	int num=icc_work->x;
	icc_wake_type wake_type;

	wake_type = ICC_INVALID;
	if(BLOCK_MSG[num] == 0)
		 wake_type |= ICC_WRITE;
	if(FIFO_NOT_EMPTY(num))
		 wake_type |= ICC_READ;
	//printk("pointer is %p\n",icc_work);
	iccdev[num].up_callback(wake_type);
	kfree((void *)icc_work);
	return;
}
#endif
/**
 * Register callback.
 * Allows the upper layer to register a callback function  *
 * \param   type
 * \param   callback Callback function to register
 * \return  0        callback registered successfully
 * \return  ENXIO    Wrong device entity 
 * \return  EBUSY    Callback already registered
 * \return  EINVAL   Callback parameter null
 * \ingroup API
*/
int icc_register_callback (icc_devices type,
                                            void(*callback) (icc_wake_type))
{
   unsigned int num=type;
	 char cThreadName[20]={0};
		/*check that call back is not null*/
   if (callback == NULL)
   {
      return (-EINVAL);
   }

   /* check validity of device number */
   if (type > (MAX_CLIENT-1) )
      return (-ENXIO);
	/*check whether the call back is already registered*/
   if (iccdev[num].up_callback != NULL)
   {
      return (-EBUSY);
   }/*if not registered*/
   else
   {
			/*Assign the call back to global structure*/
      iccdev[num].up_callback = callback;
			/*Make the thread name with client Id as postfix*/
#ifdef KTHREAD
			sprintf(cThreadName,"ICCThread%d",type);
#else
			sprintf(cThreadName,"ICCWQ%d",type);
#endif
#ifdef KTHREAD
			/*Lock the call back semaphore, so that subsequent registration 
      of call back will succeed only after releasing this in kernel thread creation*/
			if(down_interruptible(&icc_callback_sem))
				return -EBUSY;
			/*take the thread number to a global variable*/
			g_num=num;
			/*Store the task_struct into global structure, we can use it for stopping the thread*/
			icc_kthread[num]=kthread_run(icc_thread_handler,
             NULL, cThreadName);
		 /*If thread creation fails*/
		 if(icc_kthread[num]<0){
				TRACE(ICC, DBG_LEVEL_HIGH, ("Thread creation failed un-registering callback for %d\n",num+1));
				/*Assign global variable as null*/
				icc_kthread[num]=NULL;
				/*point call back to null*/
				iccdev[num].up_callback=NULL;
				/*release the call back semaphore*/
				up(&icc_callback_sem);
				return -ENOMEM;
		 }

#else
			icc_wq[num]=create_workqueue(cThreadName);
		 /*If thread creation fails*/
		 if(icc_wq[num]<0){
				TRACE(ICC, DBG_LEVEL_HIGH, ("WQ creation failed for client %d un-registering callback\n",num+1));
				/*Assign global variable as null*/
				icc_wq[num]=NULL;
				/*point call back to null*/
				iccdev[num].up_callback=NULL;
				return -ENOMEM;
		 }

#endif
   }
  return 0;
}
/**
 * UnRegister callback.
 * Allows the upper layer to unregister a callback function 
 * \param   type     
 * \param   callback Callback function to register
 * \return  0        callback registered successfully
 * \return  ENXIO    Wrong device entity 
 * \return  EINVAL   nothing to unregister
 * \ingroup API
*/
int icc_unregister_callback (icc_devices type)
{
	 unsigned int num=type;
   /* check validity of register type */
   if ( type > (MAX_CLIENT-1) )
      return (-ENXIO);
		/*check whether the call back is registered or not*/
   if (iccdev[num].up_callback == NULL)
   {
      return (-EINVAL);
   }
   else
   {
			/*Make the call back null*/
      iccdev[num].up_callback = NULL;
#ifdef KTHREAD
			/*set the exception flag on device, condition after wake*/
			icc_excpt[num]=1;
			/*stop the kthread*/
			kthread_stop(icc_kthread[num]);
			/*After kthread returns make the global task struct null*/
			icc_kthread[num]=NULL;
#else
		flush_workqueue(icc_wq[num]);
  	destroy_workqueue(icc_wq[num]);
#endif
   }
  return 0;
}

/*Module related definitions*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Swaroop Sarma");
MODULE_PARM_DESC (icc_major_id, "Major ID of device");
module_param (icc_major_id, short, 0);
/*module functions of ICC driver*/
arch_initcall(icc_init_module);
module_exit (icc_cleanup_module);
/*exported symbols from ICC driver*/
EXPORT_SYMBOL(icc_open);
EXPORT_SYMBOL(icc_close);
EXPORT_SYMBOL(icc_register_callback);
EXPORT_SYMBOL(icc_unregister_callback);
EXPORT_SYMBOL(icc_read);
EXPORT_SYMBOL(process_icc_message);
#endif/*__LIBRARY__*/
#ifdef __LIBRARY__
int icc_init(void){
  int result=0;
  result=mps_init();
  if(result<0){
    printk("Init MPS Failed\n");
    return result;
  }
	return 0;
}
int icc_raw_read (icc_msg_t *rw){
	mps_message Mpsmsg;
	int ret;
	while(!check_mps_fifo_not_empty());
	memset(&Mpsmsg,0,sizeof(mps_message));
  /*Convert data from MPS to ICC*/
	ret=mps_read_mailbox(&Mpsmsg);
	if(ret<0)
		return -EAGAIN;
  mps_icc_fill(&Mpsmsg,rw);	
	return sizeof(icc_msg_t);
}
EXPORT_SYMBOL(icc_raw_read);
EXPORT_SYMBOL(icc_init);
#endif/* __LIBRARY__*/
EXPORT_SYMBOL(icc_write);
