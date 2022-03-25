/******************************************************************************

                              Copyright (c) 2013
                            Lantiq Deutschland GmbH
                             http://www.lantiq.com

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/**
   \file drv_mps_vmmc_linux.c  Header file of the MPS driver Linux part.
   This file contains the implementation of the linux specific driver functions.
*/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_config.h"
#include "drv_mps_version.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/vermagic.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/vmalloc.h>
#include <linux/ioctl.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#ifdef FEAT_LINUX_PLATFORM_DRIVER
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#endif /* FEAT_LINUX_PLATFORM_DRIVER */

#ifdef CONFIG_PROC_FS
   #include <linux/proc_fs.h>
   #define MPS_FEAT_PROCFS_LEGACY_API

   #if (LINUX_VERSION_CODE > KERNEL_VERSION(3,8,0))
      /* sequence file is available since Linux 2.6.32 */
      #include <linux/seq_file.h>
      #define MPS_FEAT_SEQFILE
      /* The legacy API of the procfs is no longer supported in Linux 3.10.x */
      #undef MPS_FEAT_PROCFS_LEGACY_API
   #endif
#endif /* CONFIG_PROC_FS */
#include <linux/cdev.h>
#include <asm/setup.h>
#if defined(SYSTEM_4KEC) 
#include <grx500_bootcore_defs.h>
#include <grx500_bootcore_cnfg.h>
#include <grx500_bootcore_chadr.h>
#include <grx500_bootcore_chipreg.h>
#include <grx500_bootcore_emerald_env_regs.h>
#include <grx500_bootcore_uart.h>
#include <grx500_bootcore_time.h>
#include <grx500_bootcore_interrupt.h>
#endif
#include <linux/icc/drv_mps.h>
#include "drv_mps_dbg.h"
#include "drv_mps_device.h"
/* device structure */
extern mps_comm_dev mps_dev;
CREATE_TRACE_GROUP (MPS2);
#ifndef __LIBRARY__
/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#define MPS_DEV_NAME       "ltq_mps2"


/* first minor number */
#define LQ_MPS_FIRST_MINOR 1
/* total file descriptor number */
#define LQ_MPS_TOTAL_FD    2
/* ============================= */
/* Global variable definition    */
/* ============================= */

/* ============================= */
/* Global function declaration   */
extern irqreturn_t mps_ad0_irq (int32_t irq, void * pDev);
extern uint32_t mps_reset_structures (mps_comm_dev * pDev);
extern int32_t mps_bufman_close (void);
int mpsexcpt;
/* ============================= */
/* Local function declaration    */
/* ============================= */
static uint32_t mps_poll (struct file *file_p, poll_table * wait);
#ifndef VPE0
extern int32_t mps_fastbuf_get_proc (struct seq_file *s);
#endif


/* ============================= */
/* Local variable definition     */
/* ============================= */

/* ============================= */
/* Local function definition     */
/* ============================= */

/* external function declaration */

/* local function declaration */

#if (defined(MODULE) && !defined(VMMC_WITH_MPS))
MODULE_AUTHOR ("Lantiq Deutschland GmbH");
MODULE_DESCRIPTION ("MPS/DSP driver for XRX500 family");
#if defined(SYSTEM_GRX500)
MODULE_SUPPORTED_DEVICE ("XRX500 family MIPSInteraptiv");
#elif defined(SYSTE_VR9)
MODULE_SUPPORTED_DEVICE ("XRX500 family MIPS34KC");
#else /* 4KEC model*/
MODULE_SUPPORTED_DEVICE ("XRX500 family MIPS4KEC");
#endif /* */
MODULE_LICENSE ("Dual BSD/GPL");
#endif /* */
static ushort mps_major_id = 0;
module_param (mps_major_id, ushort, 0);
MODULE_PARM_DESC (mps_major_id, "Major ID of device");
char mps_dev_name[10];

/* the driver callbacks */
static struct file_operations mps_fops = {
 owner:THIS_MODULE,
 poll:mps_poll,
 unlocked_ioctl:mps_ioctl,
 open:mps_open,
 release:mps_close
};



#ifdef CONFIG_PROC_FS
static struct proc_dir_entry *mps_proc_dir;
#endif /* CONFIG_PROC_FS */
static char mps_device_version[20];

/**
   This function registers char device in kernel.
\param   pDev     pointer to mps_comm_dev structure
\return  0        success
\return  -ENOMEM
\return  -EPERM
*/
int32_t lq_mps_os_register (mps_comm_dev *pDev)
{
   int32_t ret;

   uint8_t minor;
   dev_t       dev;

   if (mps_major_id)
   {
      dev = MKDEV(mps_major_id, LQ_MPS_FIRST_MINOR);
      ret = register_chrdev_region(dev, LQ_MPS_TOTAL_FD, mps_dev_name);
   }
   else
   {
      /* dynamic major */
      ret = alloc_chrdev_region(&dev, LQ_MPS_FIRST_MINOR, LQ_MPS_TOTAL_FD, mps_dev_name);
      mps_major_id = MAJOR(dev);
   }
   if (ret < 0)
   {
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("MPS: can't get major %d\n", mps_major_id));
      return ret;
   }
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("MPS: major Id %d\n", mps_major_id));
   {
      struct cdev *p_cdev = cdev_alloc();

      if (NULL == p_cdev)
         return -ENOMEM;

      cdev_init(p_cdev, &mps_fops);
      p_cdev->owner = THIS_MODULE;
			minor=LQ_MPS_FIRST_MINOR;
      ret = cdev_add(p_cdev, MKDEV(mps_major_id, minor), 1);
      if (ret != 0)
      {
         cdev_del (p_cdev);
         return -EPERM;
      }

         pDev->mb.mps_cdev = p_cdev;
   }
   return ret;
}

/**
   This function unregisters char device from kernel.
\param   pDev     pointer to mps_comm_dev structure
*/
void lq_mps_os_unregister (mps_comm_dev *pDev)
{

   {
         cdev_del (pDev->mb.mps_cdev);
         pDev->mb.mps_cdev = NULL;
   }
   unregister_chrdev_region (MKDEV(mps_major_id, LQ_MPS_FIRST_MINOR), LQ_MPS_TOTAL_FD);
}


/**
 * Open MPS device.
 * Open the device from user mode (e.g. application) or kernel mode. An inode
 * value of 1 indicates a kernel mode access. In such a case the inode value
 * is used as minor ID.
 *
 * \param   inode   Pointer to device inode
 * \param   file_p  Pointer to file descriptor
 * \return  0       SUCCESS, device opened
 * \return  EMFILE  Device already open
 * \return  EINVAL  Invalid minor ID
 * \ingroup API
 */
int32_t mps_open (struct inode * inode, struct file * file_p)
{
   mps_comm_dev *pDev = &mps_dev;
   mps_mbx_dev *pMBDev;
   int32_t from_kernel = 0;
   int32_t num;

   /* Check whether called from user or kernel mode */

   /* a trick: VMMC driver passes the first parameter as a value So, if inode value is
      [1...LQ_MPS_TOTAL_FD-1], then we make sure that we are calling from
      kernel space. */
   if (((int32_t) inode > 0) &&
       ((int32_t) inode < LQ_MPS_TOTAL_FD))
   {
      from_kernel = 1;
      num = (int32_t) inode;
   }
   else
   {
      num =  MINOR (inode->i_rdev);        /* the real device */
   }
	 TRACE( MPS2, DBG_LEVEL_LOW, ("device number is %d\n",num));
   /* check the device number */
   switch (num)
   {
      case 1:
         pMBDev = &(pDev->mb);
         break;
      default:
         TRACE (MPS2, DBG_LEVEL_HIGH,
                ("MPS ERROR: max. device number exceed!\n"));
         return -EINVAL;
   }

   if ((SUCCESS) ==
       mps_common_open (pDev, pMBDev,from_kernel))
   {
      if (!from_kernel)
      {

         /* installation was successfull */
         /* and use file_p->private_data to point to the device data */
         file_p->private_data = pMBDev;
      }
      return 0;
   }
   else
   {
      /* installation failed */
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("MPS ERROR: Device %d is already open!\n", num));
      return -EMFILE;
   }
}


/**
 * Close MPS device.
 * Close the device from user mode (e.g. application) or kernel mode. An inode
 * value of 1 indicates a kernel mode access. In such a case the inode value
 * is used as minor ID.
 *
 * \param   inode   Pointer to device inode
 * \param   file_p  Pointer to file descriptor
 * \return  0       SUCCESS, device closed
 * \return  ENODEV  Device invalid
 * \return  EINVAL  Invalid minor ID
 * \ingroup API
 */
int32_t mps_close (struct inode * inode, struct file * file_p)
{
   mps_mbx_dev *pMBDev;
   int32_t from_kernel = 0;

   if (((int32_t) inode > 0) &&
       ((int32_t) inode <= LQ_MPS_TOTAL_FD))
   {
      from_kernel = 1;
 			switch ((int32_t)inode)
   		{
      	case 1:
        	 pMBDev = &(mps_dev.mb);
         break;
      	default:
        	 TRACE (MPS2, DBG_LEVEL_HIGH,
                ("MPS ERROR: max. device number exceed!\n"));
         return -EINVAL;
   		}
   }
   else
   {
      pMBDev = file_p->private_data;
   }

   if (NULL != pMBDev)
   {
      /* device is still available */
      if (mps_common_close (pMBDev, from_kernel) != SUCCESS)
      {
         TRACE (MPS2, DBG_LEVEL_HIGH,
                ("MPS ERROR: Device is not open!\n"));
         return -ENODEV;
      }

      return 0;
   }
   else
   {
      /* something went totally wrong */
      TRACE (MPS2, DBG_LEVEL_HIGH, ("MPS2 ERROR: pMBDev pointer is NULL!\n"));
      return -ENODEV;
   }
}


/**
 * Poll handler.
 * The select function of the driver. A user space program may sleep until
 * the driver wakes it up.
 *
 * \param   file_p  File structure of device
 * \param   wait    Internal table of poll wait queues
 * \return  mask    If new data is available the POLLPRI bit is set,
 *                  triggering an exception indication. If the device pointer
 *                  is null POLLERR is set.
 * \ingroup API
 */
static uint32_t mps_poll (struct file *file_p, poll_table * wait)
{
   mps_mbx_dev *pMBDev = file_p->private_data;
   uint32_t mask;

   /* add to poll queue */
	 poll_wait(file_p, &(pMBDev->mps_wakeuplist), wait);
   mask = 0;
		if(mpsexcpt){
			mpsexcpt=0;
			TRACE (MPS2, DBG_LEVEL_LOW, ("waking up sleeping process\n"));
   		/* upstream queue */
   		if (*pMBDev->upstrm_fifo->pwrite_off != *pMBDev->upstrm_fifo->pread_off)
   		{
      	mask = POLLIN | POLLRDNORM;
   		}
   		/* no downstream queue in case of event mailbox */
   		if (pMBDev->dwstrm_fifo == NULL)
      	return mask;

   		/* downstream queue */
   		if (mps_fifo_mem_available (pMBDev->dwstrm_fifo) != 0)
   		{
      	/* queue is not full */
      	mask |= POLLOUT | POLLWRNORM;
   		}
	}
   return mask;
}


/**
 * MPS IOCTL handler.
 * An inode value of 0..7 indicates a kernel mode access. In such a case the
 * inode value is used as minor ID.
 * The following IOCTLs are supported for the MPS device.
 * - #FIO_MPS_MB_READ
 * - #FIO_MPS_MB_WRITE
 * - #FIO_MPS_GETVERSION
 * - #FIO_MPS_RESET
 * - #FIO_MPS_RESTART
 *
 * \param   inode        Inode of device
 * \param   file_p       File structure of device
 * \param   nCmd         IOCTL command
 * \param   arg          Argument for some IOCTL commands
 * \return  0            Setting the LED bits was successfull
 * \return  -EINVAL      Invalid minor ID
 * \return  -ENOIOCTLCMD Invalid command
 * \ingroup API
 */
long mps_ioctl (struct file *file_p,
                           uint32_t nCmd, ulong arg)
{
   int32_t retvalue = -EINVAL;
   mps_message rw_struct;
   mps_mbx_dev *pMBDev;
   int32_t from_kernel = 0;

   /* a trick: VMMC driver passes the first parameter as a value of
      'mps_devices' enum type, which in fact is [0..8]; So, if inode value is
      [1], then we make sure that we are calling from
      kernel space. */
   if (((int32_t) file_p > 0) &&
       ((int32_t) file_p <= LQ_MPS_TOTAL_FD))
   {
      from_kernel = 1;
			
      /* Get corresponding mailbox device structure */
      if ((pMBDev =&mps_dev.mb) == 0)
			{
         return (-EINVAL);
      }
      file_p = NULL;
   }
   else
   {
      pMBDev = file_p->private_data;
   }

   switch (nCmd)
   {
      case FIO_MPS_MB_READ:
         /* Read the data from mailbox stored in local FIFO */
         if (from_kernel)
         {
            retvalue = mps_mbx_read (pMBDev, (mps_message *) arg, 0);
         }
         else
         {
            uint32_t UserBuf[16]={0};

            /* Initialize destination and copy mps_message from usermode */
            memset (&rw_struct, 0, sizeof (mps_message));
            if (0<
                copy_from_user (&rw_struct, (void *) arg,
                                   sizeof (mps_message)))
            {
               TRACE (MPS2, DBG_LEVEL_HIGH,
                      (KERN_ERR "[%s %s %d]: copy_from_user error\r\n",
                       __FILE__, __func__, __LINE__));
            }
						memcpy(UserBuf,rw_struct.data,16);
																																	/* Remember
                                                                   usermode
                                                                   buffer */

            /* read data from upstream mailbox FIFO */
            retvalue = mps_mbx_read (pMBDev, &rw_struct, 0);
            if (retvalue != SUCCESS)
               return -ENOMSG;

            /* Copy data to usermode buffer... */
						if(CHECK_PTR(rw_struct.header.Hd.param_attr,0)){
            	if (0<
              	  copy_to_user ((uint8_t *)UserBuf[0], (uint8_t *)rw_struct.data[0],
                                 rw_struct.data[1]))
            	{
               	TRACE (MPS2, DBG_LEVEL_HIGH,
                	      (KERN_ERR "[%s %s %d]: copy_to_user error\r\n", __FILE__,
                  	     __func__, __LINE__));
            	}
            	mps_bufman_free ((void *)rw_struct.data[0]);
						}

            /* ... and finally restore the buffer pointer and copy mps_message
               back! */
            memcpy(rw_struct.data,UserBuf,16);
            if (0<
                copy_to_user ((void *) arg, &rw_struct,
                                 sizeof (mps_message)))
            {
               TRACE (MPS2, DBG_LEVEL_HIGH,
                      (KERN_ERR "[%s %s %d]: copy_to_user error\r\n", __FILE__,
                       __func__, __LINE__));
            }
         }
         break;
      case FIO_MPS_MB_WRITE:
         /* Write data to send to the mailbox into the local FIFO */
         if (from_kernel)
         {
               return (mps_mbx_write_data (pMBDev, (mps_message *) arg));
         }
         else
         {
            uint32_t UserBuf[16]={0};
            if (0<
                copy_from_user (&rw_struct, (void *) arg,
                                   sizeof (mps_message)))
            {
               TRACE (MPS2, DBG_LEVEL_HIGH,
                      (KERN_ERR "[%s %s %d]: copy_from_user error\r\n",
                       __FILE__, __func__, __LINE__));
            }

            /* Remember usermode buffer */
						memcpy(UserBuf,rw_struct.data,16);

            /* Allocate kernelmode buffer for writing data */
				if(CHECK_PTR(rw_struct.header.Hd.param_attr,0)){	
#ifndef VPE0
            rw_struct.data[0] =(uint32_t)
               mps_bufman_malloc (rw_struct.data[1], 0x02);
#else
            rw_struct.data[0] =(uint32_t)
               mps_bufman_malloc (rw_struct.data[1], 0xff);
#endif
					
            if (rw_struct.data[0] == 0)
            {
               return (-ENOMEM);
            }

            /* copy data to kernelmode buffer and write to mailbox FIFO */
            if (0<
                copy_from_user ((uint8_t *)rw_struct.data[0], (uint8_t *)UserBuf[0],
                                   rw_struct.data[1]))
            {
               TRACE (MPS2, DBG_LEVEL_HIGH,
                      (KERN_ERR "[%s %s %d]: copy_from_user error\r\n",
                       __FILE__, __func__, __LINE__));
            }
				}
               if ((retvalue =
                    mps_mbx_write_data (pMBDev, &rw_struct)) != SUCCESS)
                  mps_bufman_free ((void *)rw_struct.data[0]);
            /* ... and finally restore the buffer pointer and copy mps_message
               back! */
            memcpy(rw_struct.data,UserBuf,16);
            if (0<
                copy_to_user ((void *) arg, &rw_struct,
                                 sizeof (mps_message)))
            {
               TRACE (MPS2, DBG_LEVEL_HIGH,
                      (KERN_ERR "[%s %s %d]: copy_to_user error\r\n", __FILE__,
                       __func__, __LINE__));
            }
         }
         break;
      case FIO_MPS_GETVERSION:
         if (from_kernel)
         {
            memcpy ((char *) arg, (char *) mps_device_version,
                    strlen (mps_device_version));
         }
         else
         {
            if (0<
                copy_to_user ((void *) arg, mps_device_version,
                                 strlen (mps_device_version)))
            {
               TRACE (MPS2, DBG_LEVEL_HIGH,
                      (KERN_ERR "[%s %s %d]: copy_to_user error\r\n", __FILE__,
                       __func__, __LINE__));
            }
         }
         retvalue = SUCCESS;
         break;
      case FIO_MPS_RESET:
         
   			/* reset driver */
   			mps_bufman_close ();
         break;
      case FIO_MPS_RESTART:
            retvalue = mps_bufman_init ();
         break;
      default:
         {
            TRACE (MPS2, DBG_LEVEL_HIGH,
                   ("MPS_Ioctl: Invalid IOCTL handle %d passed.\n", nCmd));
            retvalue = -ENOIOCTLCMD;
            break;
         }
   }
   return retvalue;
}


/**
 * Read from mailbox upstream FIFO.
 * This function reads from the mailbox upstream FIFO selected by type.
 *
 * \param   type  DSP device entity ( 1 - command, 2 - voice0, 3 - voice1,
 *                4 - voice2, 5 - voice3 )
 * \param   rw    Pointer to message structure for received data
 * \return  0     SUCCESS, successful read operation
 * \return  ENXIO Wrong DSP device entity (only 1-5 supported)
 * \return  -1    ERROR, in case of read error.
 * \ingroup API
 */
int32_t mps_read_mailbox (mps_message * rw)
{
   int32_t ret;
   ret = mps_mbx_read (&mps_dev.mb, rw, 0);
   return (ret);
}


/**
 * Write to downstream mailbox buffer.
 * This function writes data to either the command or to the voice FIFO
 *
 * \param   type  1-data
 * \param   rw    Pointer to message structure
 * \return  0       SUCCESS, successful write operation
 * \return  -ENXIO  Wrong DSP device entity (only 1-5 supported)
 * \return  -EAGAIN ERROR, in case of FIFO overflow.
 * \ingroup API
 */
int32_t mps_write_mailbox (mps_message * rw)
{
   int32_t ret;
   ret = mps_mbx_write_data (&mps_dev.mb, rw);
   return (ret);
}


#ifdef CONFIG_PROC_FS

#ifdef MPS_FEAT_SEQFILE
/**
 * Create MPS version proc file output.
 * This function creates the output for the MPS version proc file
 *
 * \param   s        Pointer to seq_file struct.
 * \return  0 on success
 * \ingroup Internal
 */
static int mps_get_version_proc (struct seq_file *s)
{
   seq_printf(s, "%s%s\n", MPS_INFO_STR, mps_device_version);
#if GCC_VERSION < 50000
   seq_printf(s, "Compiled on %s, %s for Linux kernel %s\n",
               __DATE__, __TIME__, UTS_RELEASE);
#endif

   return 0;
}
#endif /* MPS_FEAT_SEQFILE */


#ifdef MPS_FEAT_SEQFILE
/**
 * Create MPS status proc file output.
 * This function creates the output for the MPS status proc file
 *
 * \param   s        Pointer to seq_file struct.
 * \return  0 on success
 * \ingroup Internal
 */
int32_t mps_get_status_proc (struct seq_file *s)
{
   seq_printf (s, "Status registers:\n");
   seq_printf (s, "   AD0ENR = 0x%08x\n", *MPS_AD0ENR);
   seq_printf (s, "   RAD0SR = 0x%08x\n", *MPS_RAD0SR);
   seq_printf (s, "   AD1ENR = 0x%08x\n", *MPS_AD1ENR);
   seq_printf (s, "   RAD1SR = 0x%08x\n", *MPS_RAD1SR);

   seq_printf (s, "\n * VOICE *\t\tUP\t\tDO\n");
   seq_printf (s, "   Size: \t  %8d\t  %8d\n",
               mps_dev.upstrm_fifo.size,
               mps_dev.dwstrm_fifo.size);
   seq_printf (s, "   Fill: \t  %8d\t  %8d\n",
               mps_dev.upstrm_fifo.size - 1 -
               mps_fifo_mem_available (&mps_dev.upstrm_fifo),
               mps_dev.dwstrm_fifo.size - 1 -
               mps_fifo_mem_available (&mps_dev.dwstrm_fifo));
   seq_printf (s, "   Free: \t  %8d\t  %8d\n",
               mps_fifo_mem_available (&mps_dev.upstrm_fifo),
               mps_fifo_mem_available (&mps_dev.dwstrm_fifo));
   seq_printf (s, "   Pkts: \t  %8d\t  %8d\n",
               mps_dev.upstrm_fifo.pkts,
               mps_dev.dwstrm_fifo.pkts);
   seq_printf (s, "   Bytes: \t  %8d\t  %8d\n",
               mps_dev.upstrm_fifo.bytes,
               mps_dev.dwstrm_fifo.bytes);
   seq_printf (s, "   Discd: \t  %8d\n",
               mps_dev.upstrm_fifo.discards);
   seq_printf (s, "   minLv: \t  %8d\t  %8d\n",
               mps_dev.upstrm_fifo.min_space,
               mps_dev.dwstrm_fifo.min_space);
   {
      seq_printf (s, "\n * CH *\t\tUP\t\tDO\t%s\n", 
                  (mps_dev.mb.Installed ==
                   FALSE) ? "(idle)" : "(active)");
      seq_printf (s, "   Size: \t  %8d\n",
                  mps_dev.mb.upstrm_fifo->size);
      seq_printf (s, "   Fill: \t  %8d\n",
                  mps_dev.mb.upstrm_fifo->size - 1 -
                  mps_fifo_mem_available (mps_dev.mb.
                                              upstrm_fifo));
      seq_printf (s, "   Free: \t  %8d\n",
                  mps_fifo_mem_available (mps_dev.mb.
                                              upstrm_fifo));
      seq_printf (s, "   Pkts: \t  %8d\n",
                  mps_dev.mb.upstrm_fifo->pkts);
      seq_printf (s, "   Bytes: \t  %8d\n",
                  mps_dev.mb.upstrm_fifo->bytes);
      seq_printf (s, "   Discd: \t  %8d\n",
                  mps_dev.mb.upstrm_fifo->discards);
      seq_printf (s, "   minLv: \t  %8d\n",
                  mps_dev.mb.upstrm_fifo->min_space);
   }

   return 0;
}
#endif /* MPS_FEAT_SEQFILE */


#ifdef MPS_FEAT_SEQFILE
/**
 * Create MPS mailbox proc file output.
 * This function creates the output for the MPS mailbox proc file
 *
 * \param   s        Pointer to seq_file struct.
 * \return  0 on success
 * \ingroup Internal
 */
static int32_t mps_get_mailbox_proc (struct seq_file *s)
{
   uint32_t i;

   seq_printf (s, "\n * VOICE * UP");
   seq_printf (s, " (wr:0x%08x, rd: 0x%08x)\n",
               (uint32_t) mps_dev.upstrm_fifo.pend +
               (uint32_t) * mps_dev.upstrm_fifo.pwrite_off,
               (uint32_t) mps_dev.upstrm_fifo.pend +
               (uint32_t) * mps_dev.upstrm_fifo.pread_off);
   for (i = 0; i < mps_dev.upstrm_fifo.size; i += 16)
   {
      seq_printf (s, "   0x%08x: %08x %08x %08x %08x\n",
                  (uint32_t) (mps_dev.upstrm_fifo.pend + (i / 4)),
                  *(mps_dev.upstrm_fifo.pend + (i / 4)),
                  *(mps_dev.upstrm_fifo.pend + 1 + (i / 4)),
                  *(mps_dev.upstrm_fifo.pend + 2 + (i / 4)),
                  *(mps_dev.upstrm_fifo.pend + 3 + (i / 4)));
   }

   seq_printf (s, "\n * VOICE * DO");
   seq_printf (s, " (wr: 0x%08x, rd: 0x%08x)\n",
               (uint32_t) mps_dev.dwstrm_fifo.pend +
               (uint32_t) * mps_dev.dwstrm_fifo.pwrite_off,
               (uint32_t) mps_dev.dwstrm_fifo.pend +
               (uint32_t) * mps_dev.dwstrm_fifo.pread_off);
   for (i = 0; i < mps_dev.dwstrm_fifo.size; i += 16)
   {
      seq_printf (s, "   0x%08x: %08x %08x %08x %08x\n",
                  (uint32_t) (mps_dev.dwstrm_fifo.pend + (i / 4)),
                  *(mps_dev.dwstrm_fifo.pend + (i / 4)),
                  *(mps_dev.dwstrm_fifo.pend + 1 + (i / 4)),
                  *(mps_dev.dwstrm_fifo.pend + 2 + (i / 4)),
                  *(mps_dev.dwstrm_fifo.pend + 3 + (i / 4)));
   }

   return 0;
}
#endif /* MPS_FEAT_SEQFILE */


#ifdef MPS_FEAT_SEQFILE
/**
 * Create MPS sw fifo proc file output.
 * This function creates the output for the sw fifo proc file
 *
 * \param   s        Pointer to seq_file struct.
 * \return  0 on success
 * \ingroup Internal
 */
static int32_t mps_get_swfifo_proc (struct seq_file *s)
{
   int32_t i;
   {
      seq_printf (s, "\n"
                     " * CH * UP");
      seq_printf (s, " (wr:0x%08x, rd: 0x%08x)\n",
                  (uint32_t) mps_dev.sw_upstrm_fifo.pend +
                  (uint32_t) * mps_dev.sw_upstrm_fifo.pwrite_off,
                  (uint32_t) mps_dev.sw_upstrm_fifo.pend +
                  (uint32_t) * mps_dev.sw_upstrm_fifo.pread_off);

      for (i = 0; i < mps_dev.sw_upstrm_fifo.size; i += 16)
      {
         seq_printf (s, "   0x%08x: %08x %08x %08x %08x\n",
                     (uint32_t) (mps_dev.sw_upstrm_fifo.pend +
                                     (i / 4)),
                     *(mps_dev.sw_upstrm_fifo.pend + (i / 4)),
                     *(mps_dev.sw_upstrm_fifo.pend + 1 + (i / 4)),
                     *(mps_dev.sw_upstrm_fifo.pend + 2 + (i / 4)),
                     *(mps_dev.sw_upstrm_fifo.pend + 3 + (i / 4)));
      }
   }

   return 0;
}
#endif /* MPS_FEAT_SEQFILE */



#ifndef MPS_FEAT_PROCFS_LEGACY_API
typedef void (*mps_dump) (struct seq_file *s);

static int mps_proc_show ( struct seq_file *s, void *p )
{
   mps_dump dump = s->private;

   if (dump != NULL)
      dump(s);

   return 0;
}

static int mps_proc_open ( struct inode *inode, struct file *file )
{
   return single_open (file, mps_proc_show, PDE_DATA(inode));
}

struct proc_entry
{
   const char *name;
   void *read_function;
   void *write_function;
   struct file_operations ops;
};

static struct proc_entry proc_entries[] =
{
   { "version", mps_get_version_proc},
   { "status", mps_get_status_proc},
   { "mailbox", mps_get_mailbox_proc},
   { "swfifo", mps_get_swfifo_proc},
#ifndef VPE0
	 { "fastbuf",mps_fastbuf_get_proc},
#endif
};

static void mps_proc_entrycreate ( struct proc_dir_entry *parent_node,
                 struct proc_entry *proc_entry)
{
   memset(&proc_entry->ops, 0, sizeof(struct file_operations));
   proc_entry->ops.owner   = THIS_MODULE;
   proc_entry->ops.open    = mps_proc_open;
   proc_entry->ops.read    = seq_read;
   proc_entry->ops.write   = proc_entry->write_function;
   proc_entry->ops.llseek  = seq_lseek;
   proc_entry->ops.release = single_release;

   proc_create_data ( proc_entry->name, 0, parent_node,
            &proc_entry->ops, proc_entry->read_function);
}
#endif /* MPS_FEAT_PROCFS_LEGACY_API */

#endif /* CONFIG_PROC_FS */

#ifdef FEAT_LINUX_PLATFORM_DRIVER
/**
   Linux platform driver probe function.

   Read parameters from device tree and configure driver.

   Example entry of needed device tree nodes.
   \verbatim
   mps@107000 {
      compatible = "lantiq,prx300-mps2","lantiq,mps2-xrx500";
      reg = <0x107000 0x400>;
      interrupt-parent = <&icu0>;
      interrupts = <154 155>;
      lantiq,mbx = <&mpsmbx>;
   };

   mpsmbx: mpsmbx@20000 {
      reg = <0x200000 0x200>;
   };
   \endverbatim

   \param  pdev         Pointer to struct platform_device.

   \return
   0 Successful
   !0 Failed to find the config or the device.
*/
int ltq_mps_probe(struct platform_device *pdev)
{
   struct device_node   *pdn;
   struct resource      *res = NULL;
	 int uiFlag;
	static void __iomem *mps_reg_base;
	static void __iomem *mps_ram_base;

   /* Clear parameters to detect if all can be set below. */
   mps_ir4 = 0;
   mps_reg_base = mps_ram_base = 0;
#ifdef VPE0
   /* Interrupt number connected to the AD0 register. */
   mps_ir4 = platform_get_irq(pdev,0);
	 uiFlag=(mps_ir4==0)?0:1;
#else
   mps_ir0 = platform_get_irq(pdev,0);
   mps_ir1 = platform_get_irq(pdev,1);
	 uiFlag=(mps_ir0==0||mps_ir1==0)?0:1;
#endif
   /* Baseaddress of the MPS register block. */
   res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
   if (res)
   {
      mps_reg_base = devm_ioremap_resource(&pdev->dev, res);
      if (IS_ERR(mps_reg_base))
         return PTR_ERR(mps_reg_base);
   }

   /* Baseaddress of the MPS SRAM region. This is used for mailboxes. */
   /* Dereference the phandle pointing to the node with the actual address. */
   pdn = of_parse_phandle(pdev->dev.of_node, "lantiq,mbx", 0);
   if (pdn)
   {
      struct resource mbx_res;

      if (of_address_to_resource(pdn, 0, &mbx_res) == 0)
      {
         mps_ram_base = devm_ioremap_resource(&pdev->dev, &mbx_res);
         if (IS_ERR(mps_ram_base))
         {
            of_node_put(pdn);
            return PTR_ERR(mps_ram_base);
         }
      }
      of_node_put(pdn);
   }
   /* Verify that all parameters were set. */
   if (!uiFlag || !mps_reg_base || !mps_ram_base)
   {
#ifdef VPE0
      if (!mps_ir4)
      {
         TRACE ( MPS2,DBG_LEVEL_HIGH,(KERN_ERR "Failed to find MPS2 AD0 irq in device tree.\n"));
      }
#else
      if (!mps_ir0 || !mps_ir1)
      {
         TRACE ( MPS2,DBG_LEVEL_HIGH,(KERN_ERR "Failed to find MPS2 AD0 irq in device tree.\n"));
      }
#endif
      if (!mps_reg_base)
      {
         TRACE ( MPS2,DBG_LEVEL_HIGH,(KERN_ERR "Failed to find MPS2 reg base in device tree.\n"));
      }
      if (!mps_ram_base)
      {
         TRACE ( MPS2,DBG_LEVEL_HIGH,(KERN_ERR "Failed to find MPS2 mbx base in device tree.\n"));
      }

      return -ENXIO;
   }

   return 0;
}

static const struct of_device_id ltq_mps_match[] = {
  { .compatible = "lantiq,prx300-mps2" },
  { .compatible = "lantiq,mps2-xrx500" },
  {},
};
MODULE_DEVICE_TABLE(of, ltq_asc_match);

static struct platform_driver ltqmps_driver = {
  .driver   = {
    .name = "lantiq,mps2",
    .owner  = THIS_MODULE,
    .of_match_table = ltq_mps_match,
  },
};

#endif /* FEAT_LINUX_PLATFORM_DRIVER */
#if defined(VPE0) && defined(SYSTEM_4KEC)
static struct irqaction mps_irqaction = {
      .handler        = mps_ad0_irq,
      .flags          = 0, /* for disable nested interrupts */
      /* Lior.H - when we need to use-> IRQF_NOBALANCING ? */
      .name           = "4kec_mps4",
			.dev_id					= &mps_dev,
};
#endif
/**
   This function initializes the module.
\param
   None.
\return  SUCCESS, module initialized
\return  EPERM    Reset of CPU1 failed
\return  ENOMEM   No memory left for structures
*/
static int32_t __init
mps_init_module (void)
{
   int32_t result;
   int32_t i;
   unsigned int virt;

   sprintf (mps_device_version, "%d.%d.%d.%d", MAJORSTEP, MINORSTEP,
            VERSIONSTEP, VERS_TYPE);

   TRACE (MPS2, DBG_LEVEL_HIGH,
          ("%s%s, (c) 2006-2013 Lantiq Deutschland GmbH\n", MPS_INFO_STR,
           mps_device_version));

   sprintf (mps_dev_name, MPS_DEV_NAME);

#ifdef FEAT_LINUX_PLATFORM_DRIVER
	 result = platform_driver_probe(&ltqmps_driver, ltq_mps_probe);
	 if(result)
			return result;
#endif
   mps_bufman_init ();

   /* init the device driver structure */
   if (0 != mps_init_structures (&mps_dev))
      return -ENOMEM;
   /* register char module in kernel */
   result = lq_mps_os_register (&mps_dev);
   if (result)
      return result;
#if defined(SYSTEM_VR9)
#ifdef VPE0
   /* Set handler for interrupt generated by voice-FW AD0 status register. */
   virt = irq_create_mapping(NULL, mps_ir4);
   if (!virt)
      return -1;
   result = request_irq (virt,
                         mps_ad0_irq, 0
                         , "mps_mbx_0", &mps_dev);
 if (result)
      return result;
#else
   /* Set handler for interrupt generated by voice-FW AD0 status register. */
   virt = irq_create_mapping(NULL, mps_ir0);
   if (!virt)
      return -1;
   result = request_irq (virt,
                         mps_ad0_irq, 0
                         , "mps_mbx_0", &mps_dev);
 if (result)
      return result;
#endif

#else	
#ifdef VPE0
     virt=mps_ir4;
	 grx500_bootcore_register_static_irq(virt,GRX500_BOOTCORE_MPS2_OUT_INDEX,&mps_irqaction,grx500_bootcore_mps_irq);
#else
	 virt=mps_ir0;
   result = request_irq (virt,
                         mps_ad0_irq, 0
                         , "mps2_mbx_0", &mps_dev);
 if (result)
      return result;
#endif
#endif
  mps_enable_mailbox_int ();

#ifdef CONFIG_PROC_FS
   /* install the proc entry */
   mps_proc_dir = proc_mkdir ("driver/" MPS_DEV_NAME, NULL);
   if (mps_proc_dir != NULL)
   {
      for(i=0; i<sizeof(proc_entries)/sizeof(proc_entries[0]);i++) {
         mps_proc_entrycreate (mps_proc_dir, &proc_entries[i]);
      }
   }
   else
   {
      TRACE (MPS2, DBG_LEVEL_HIGH, ("MPS2: cannot create proc entry\n"));
   }
#endif /* */
  return SUCCESS;
}


/**
   This function cleans up the module.
\param
   None.
\return
   None.
*/
static void __exit
mps_cleanup_module (void)
{
	mps_disable_mailbox_int ();
   /* disable all MPS interrupts */
   mps_disable_all_int ();
   /* unregister char module from kernel */
   lq_mps_os_unregister (&mps_dev);

#ifdef VPE0
   /* release all interrupts at the system */
#ifdef SYSTEM_VR9
   free_irq (mps_ir4, &mps_dev);
#elif defined(SYSTEM_4KEC)
	 grx500_bootcore_unregister_irq(mps_ir4,11,&mps_dev);
#endif
#else
   /* release all interrupts at the system */
   free_irq (mps_ir0, &mps_dev);
#endif
#ifdef CONFIG_PROC_FS
   remove_proc_entry ("mailbox", mps_proc_dir);
   remove_proc_entry ("swfifo", mps_proc_dir);
   remove_proc_entry ("version", mps_proc_dir);
   remove_proc_entry ("status", mps_proc_dir);
#ifndef VPE0
   remove_proc_entry ("fastbuf", mps_proc_dir);
#endif
   remove_proc_entry ("driver/" MPS_DEV_NAME, NULL);
#endif /* CONFIG_PROC_FS */

   TRACE (MPS2, DBG_LEVEL_HIGH, (KERN_INFO "Lantiq MPS2 driver: cleanup done\n"));
}

module_init (mps_init_module);
module_exit (mps_cleanup_module);

EXPORT_SYMBOL (mps_write_mailbox);
EXPORT_SYMBOL (mps_read_mailbox);
EXPORT_SYMBOL (mps_open);
EXPORT_SYMBOL (mps_close);
EXPORT_SYMBOL (mps_register_callback);
EXPORT_SYMBOL (mps_unregister_callback);
#else/*ifndef __LIBRARY__*/
int 
mps_init (void)
{
   /* init the device driver structure */
   if (0 != mps_init_structures (&mps_dev))
      return -ENOMEM;
	return 0;
}
/**
 * Read from mailbox upstream FIFO.
 * This function reads from the mailbox upstream FIFO selected by type.
 *
 * \param   type  DSP device entity ( 1 - command, 2 - voice0, 3 - voice1,
 *                4 - voice2, 5 - voice3 )
 * \param   rw    Pointer to message structure for received data
 * \return  0     SUCCESS, successful read operation
 * \return  ENXIO Wrong DSP device entity (only 1-5 supported)
 * \return  -1    ERROR, in case of read error.
 * \ingroup API
 */
int32_t mps_read_mailbox (mps_message * rw)
{
   int32_t ret;
   ret = mps_mbx_read (&mps_dev.mb, rw, 0);
   return (ret);
}


/**
 * Write to downstream mailbox buffer.
 * This function writes data to either the command or to the voice FIFO
 *
 * \param   type  1-data
 * \param   rw    Pointer to message structure
 * \return  0       SUCCESS, successful write operation
 * \return  -ENXIO  Wrong DSP device entity (only 1-5 supported)
 * \return  -EAGAIN ERROR, in case of FIFO overflow.
 * \ingroup API
 */
int32_t mps_write_mailbox (mps_message * rw)
{
   int32_t ret;
   ret = mps_mbx_write_data (&mps_dev.mb, rw);
   return (ret);
}

EXPORT_SYMBOL (mps_write_mailbox);
EXPORT_SYMBOL (mps_read_mailbox);
EXPORT_SYMBOL(mps_init);
#endif
