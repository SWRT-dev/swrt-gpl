/******************************************************************************

                              Copyright (c) 2013
                            Lantiq Deutschland GmbH
                             http://www.lantiq.com

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

****************************************************************************
   Module      : drv_mps_vmmc_common.c
   Description : This file contains the implementation of the common MPS
                 driver functions.
*******************************************************************************/

/* ============================= */
/* Includes                      */
/* ============================= */
#include "drv_config.h"

#include <linux/version.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/icc/drv_mps.h>
#include "drv_mps_dbg.h"
#include "drv_mps_device.h"

/* ============================= */
/* Local Macros & Definitions    */
/* ============================= */
#define MPS_UNUSED(var) ((void)(var))
/* ============================= */
/* Global variable definition    */
/* ============================= */
static DEFINE_SPINLOCK(g_mps2_lock_malloc);
static DEFINE_SPINLOCK(g_mps2_lock_free);
static DEFINE_SPINLOCK(g_mps2_lock);
static DEFINE_SPINLOCK(g_mps2_lock_read);
static DEFINE_SPINLOCK(g_mps2_lock_write);
mps_comm_dev mps_dev;
/** This variable holds the actual base address of the MPS register block. */
uint32_t mps_reg_base=MPS_BASE_ADDR;  /* MPS registers */
/** This variable holds the actual base address of the MPS SRAM area. */
uint32_t mps_ram_base=(u32)MPS_SRAM;  /* MPS memory */
/** This variable holds the interrupt number of the IRQ associated with the
    MPS status register 4 which is used for AFE and DFE 0 status. */
uint32_t mps_ir4=MPS_IR4;       /* MPS AD0 register interrupt */
uint32_t mps_ir0=MPS_IR0;       /* MPS register interrupt */
uint32_t mps_ir1=MPS_IR1; /* MPS register interrupt */

/* ============================= */
/* Global function declaration   */
/* ============================= */
void mps_mbx_data_upstream (ulong dummy);
#ifndef __LIBRARY__
int32_t mps_bufman_close (void);
int32_t mps_mbx_read_message (mps_fifo * fifo, MbxMsg_s * msg,
                                      uint32_t * bytes);

void *mps_fastbuf_malloc (size_t size, int32_t priority);
void mps_fastbuf_free (const void * ptr);
#ifndef VPE0
int32_t mps_fastbuf_init (void);
int32_t mps_fastbuf_close (void);
uint32_t mps_reset_structures (mps_comm_dev * pDev);
#endif
bool mps_ext_bufman (void);
extern int mpsexcpt;
/* ============================= */
/* Local function declaration    */
/* ============================= */

/* ============================= */
/* Local variable definition     */
/* ============================= */
/* global structure that holds VCPU buffer management data */
mps_buf_mng_t mps_buffer = {
   /* fast buffer manager */
   .malloc = &mps_fastbuf_malloc,
   .free = &mps_fastbuf_free,
#ifndef VPE0
   .init = &mps_fastbuf_init,
   .close = &mps_fastbuf_close,
#endif
};

mps_comm_dev *pMPSDev = &mps_dev;


/******************************************************************************
 * Fast bufferpool
 ******************************************************************************/
#define FASTBUF_USED     0x00000001
#define FASTBUF_FW_OWNED 0x00000002
#define FASTBUF_BUFS     (MPS_BUFFER_INITIAL*2)
#define FASTBUF_BUFSIZE  MPS_MEM_SEG_DATASIZE
uint32_t *fastbuf_ptr;
uint32_t fastbuf_initialized = 0;
uint32_t *fastbuf_pool;
uint32_t *fastbuf_pool_unaligned;

/* cache operations */
#ifdef SYSTEM_4KEC
#define CACHE_LINE_SZ 16
#else
#define CACHE_LINE_SZ 32
#endif
void cache_wb_inv (ulong addr, uint32_t len);
void cache_inv (ulong addr, uint32_t len);

/* ============================= */
/* Local function definition     */
/* ============================= */
/**
 * External buffer management check
 * Checks for external buffer manager (e.g. lib_bufferpool).
 *
 * \param   none
 *
 * \return  TRUE    External buffer manager is used (e.g. lib_bufferpool)
 * \return  FALSE   MPS internal buffer manager is used (fastbuf)
 * \ingroup Internal
 */
bool mps_ext_bufman ()
{
   return (((mps_buffer.malloc != &mps_fastbuf_malloc) ||
            (mps_buffer.free != &mps_fastbuf_free)) ? TRUE : FALSE);
}

/**
 * Buffer allocate
 * Allocates and returns a buffer from the buffer pool.
 *
 * \param   size        Size of requested buffer
 * \param   priority    Ignored, always atomic
 *
 * \return  ptr    Address of the allocated buffer
 * \return  NULL   No buffer available
 * \ingroup Internal
 */
void *mps_fastbuf_malloc (size_t size, int32_t priority)
{
   unsigned long flags;
   uint32_t ptr;
#ifndef VPE0
   int32_t findex = 0;
#else
  int32_t findex = MPS_BUFFER_INITIAL;
#endif

   if (fastbuf_initialized == 0)
   {
#ifdef VPE0
   	uint32_t *ptr;
	 	mps_mbx_reg *MBX_Memory = (mps_mbx_reg *) mps_ram_base;
   	ptr = (uint32_t *)MBX_Memory->MBX_VPE1_PTR;
	 	TRACE (MPS2, DBG_LEVEL_LOW, ("VPE0 buffer pool pointer is %p\n",ptr));
	 	fastbuf_pool=ptr;
		if(!fastbuf_pool){
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("%s() - error, fast buffer not initialised\n", __FUNCTION__));
			return NULL;
		}
   	fastbuf_initialized = 1;
#else
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("%s() - error, fast buffer not initialised\n", __FUNCTION__));
      return NULL;
#endif
   }

   if (size > FASTBUF_BUFSIZE)
   {
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("%s() - error, buffer too large\n", __FUNCTION__));
      return NULL;
   }

   spin_lock_irqsave (&g_mps2_lock_malloc,flags);
   do
   {
      if (fastbuf_pool[findex] & FASTBUF_USED)
         continue;
      ptr = fastbuf_pool[findex];
      fastbuf_pool[findex] |= FASTBUF_USED;
      if ((priority == FASTBUF_FW_OWNED))
         fastbuf_pool[findex] |= priority;
      spin_unlock_irqrestore (&g_mps2_lock_malloc,flags);
      return (void *) ptr;
   } 
#ifndef VPE0
while (++findex < MPS_BUFFER_INITIAL);
#else
while (++findex < FASTBUF_BUFS);
#endif
   spin_unlock_irqrestore (&g_mps2_lock_malloc,flags);
   TRACE (MPS2, DBG_LEVEL_HIGH,
          ("%s() - error, buffer pool empty\n", __FUNCTION__));

   return NULL;
}


/**
 * Buffer free
 * Returns a buffer to the buffer pool.
 *
 * \param   ptr    Address of the allocated buffer
 *
 * \return  none
 * \ingroup Internal
 */
void mps_fastbuf_free (const void * ptr)
{
   unsigned long flags;
   int32_t findex = 0;

   spin_lock_irqsave (&g_mps2_lock_free,flags);
   do
   {
      if ((fastbuf_pool[findex] & ~(FASTBUF_FW_OWNED ))
          == ((uint32_t) ptr | FASTBUF_USED))
      {
         fastbuf_pool[findex] &=
            ~(FASTBUF_FW_OWNED);
         fastbuf_pool[findex] &= ~FASTBUF_USED;
         spin_unlock_irqrestore (&g_mps2_lock_free,flags);
         return;
      }
   } 
	 while (++findex < FASTBUF_BUFS);
   spin_unlock_irqrestore (&g_mps2_lock_free,flags);
   TRACE (MPS2, DBG_LEVEL_HIGH,
          ("%s() - error, buffer not inside pool (0x%p)\n", __FUNCTION__, ptr));
}

#ifndef VPE0
/**
 * Create MPS fastbuf proc file output.
 * This function creates the output for the fastbuf proc file
 *
 * \param   buf      Buffer to write the string to
 * \return  len      Lenght of data in buffer
 * \ingroup Internal
 */
int32_t mps_fastbuf_get_proc (struct seq_file *s)
{
   int32_t findex = 0;

   if (fastbuf_initialized == 0)
   {
      seq_printf (s, "Fastbuf not initialized.\n");
      return 0;
   }

   seq_printf (s, "   Buffer   Owner  \n");

   do
   {
      seq_printf (s, "0x%08x ", fastbuf_pool[findex] & 0xfffffffc);
      if (fastbuf_pool[findex] & FASTBUF_USED)
      {
         seq_printf (s, " used - ");
         if (fastbuf_pool[findex] & FASTBUF_FW_OWNED)
            seq_printf (s, " BOOT CORE\n");
         else
            seq_printf (s, " IAP \n");
      }
      else
         seq_printf (s, " free\n");
   } while (++findex != FASTBUF_BUFS);
   return 0;
}


/**
 * Bufferpool init
 * Initializes a buffer pool of size FASTBUF_BUFSIZE * FASTBUF_BUFS and
 * separates it into FASTBUF_BUFS chunks. The 32byte alignment of the chunks
 * is guaranteed by increasing the buffer size accordingly. The pointer to
 * the pool is stored in fastbuf_ptr, while the pointers to the singles chunks
 * are maintained in fastbuf_pool.
 * Bit 0 of the address in fastbuf_pool is used as busy indicator.
 *
 * \return -ENOMEM  Memory allocation failed
 * \return  SUCCESS      Buffer pool initialized
 * \ingroup Internal
 */
int32_t mps_fastbuf_init (void)
{
   uint32_t *ptr, i;
   uint32_t bufsize = (FASTBUF_BUFSIZE + (FASTBUF_BUFSIZE % 32));

   if ((fastbuf_ptr = kmalloc(FASTBUF_BUFS * bufsize,GFP_KERNEL|GFP_DMA)) == NULL)
      return -ENOMEM;
   ptr = fastbuf_ptr;

	 fastbuf_pool_unaligned=kmalloc((FASTBUF_BUFS * sizeof(uint32_t))+PAGE_SIZE,GFP_KERNEL|GFP_DMA);
	 fastbuf_pool=(uint32_t *)(((uint32_t)fastbuf_pool_unaligned+PAGE_SIZE-1)&PAGE_MASK);
	 fastbuf_pool=(uint32_t *)(KSEG1ADDR(fastbuf_pool)); /*changing it to have uncached access always*/
   for (i = 0; i < FASTBUF_BUFS; i++)
   {
      fastbuf_pool[i] = (uint32_t) ptr;
      ptr = (uint32_t *) ((uint32_t) ptr + bufsize);
   }
   fastbuf_initialized = 1;
   return SUCCESS;
}


/**
 * Bufferpool close
 * Frees the buffer pool allocated by mps_fastbuf_init and clears the
 * buffer pool.
 *
 * \return -ENOMEM  Memory allocation failed
 * \return  SUCCESS      Buffer pool initialized
 * \ingroup Internal
 */
int32_t mps_fastbuf_close (void)
{
   int32_t i;

   if (fastbuf_initialized)
   {
      for (i = 0; i < FASTBUF_BUFS; i++)
         fastbuf_pool[i] = 0;
      kfree (fastbuf_ptr);
			kfree (fastbuf_pool_unaligned);
      fastbuf_initialized = 0;
   }
   return SUCCESS;
}
#endif

/******************************************************************************
 * Buffer manager
 ******************************************************************************/
/**
 * Init buffer management
 * This function initializes the buffer management data structures and
 * provides buffer segments to CPU1.
 *
 * \return  0        SUCCESS, initialized and message sent
 * \return  -1       Error during message transmission
 * \ingroup Internal
 */
int32_t mps_bufman_init (void)
{
   int32_t ret = ERROR;
   /* Initialize MPS fastbuf pool only in case of MPS internal buffer
      management. Initialization of MPS fastbuf pool is not required in case of
      external buffer pool management (e.g. lib_bufferpool). */
   if (FALSE == mps_ext_bufman ())
	 {
#ifndef VPE0
      mps_buffer.init ();
#endif
		ret=SUCCESS;
	 }
   return ret;
}


/**
 * Close buffer management
 * This function is called on termination of voice CPU firmware. The registered
 * close function has to take care of freeing buffers still left in VCPU.
 *
 * \return  0        SUCCESS, buffer manage shutdown correctly
 * \return  -1       Error during shutdown
 * \ingroup Internal
 */
int32_t mps_bufman_close (void)
{
   int32_t ret = ERROR;
#ifndef VPE0
   mps_buffer.close ();
#else
   uint32_t i;
   for (i =0 ; i < FASTBUF_BUFS; i++)
   {
      fastbuf_pool[i] = 0;
   }
			fastbuf_initialized=0;
#endif
   return ret;
}


/**
 * Free buffer
 *
 * \ingroup Internal
 */
void mps_bufman_free (const void * ptr)
{
   mps_buffer.free ((void *) KSEG0ADDR (ptr));
}

/**
 * Allocate buffer
 *
 * \ingroup Internal
 */
void *mps_bufman_malloc (size_t size, int32_t priority)
{
   void *ptr;

   ptr = mps_buffer.malloc (size, priority);
   return ptr;
}


/**
 * Overwrite buffer management
 * Allows the upper layer to register its own malloc/free functions in order to do
 * its own buffer managment. To unregister driver needs to be re-initialized.
 *
 * \param   malloc      Buffer allocation - arguments and return value as kmalloc
 * \param   free        Buffer de-allocation - arguments and return value as kmalloc
 * \param   buf_size    Size of buffers provided to voice CPU
 * \param   treshold    Count of buffers provided to voice CPU
 */
void mps_bufman_register (void *
                                    (*malloc) (size_t size,
                                               int32_t priority),
                                    void (*free) (const void * ptr))
{
   mps_buffer.malloc = malloc;
   mps_buffer.free = free;
}
#endif/* __LIBRARY__*/
/******************************************************************************
 * FIFO Managment
 ******************************************************************************/

/**
 * Clear FIFO
 * This function clears the FIFO by resetting the pointers. The data itself is
 * not cleared.
 *
 * \param   fifo    Pointer to FIFO structure
 * \ingroup Internal
 */
void mps_fifo_clear (mps_fifo * fifo)
{
   *fifo->pread_off = fifo->size - 4;
   *fifo->pwrite_off = fifo->size - 4;
   return;
}


/**
 * Check FIFO for being not empty
 * This function checks whether the referenced FIFO contains at least
 * one unread data byte.
 *
 * \param   fifo     Pointer to FIFO structure
 * \return  1        TRUE if data to be read is available in FIFO,
 * \return  0        FALSE if FIFO is empty.
 * \ingroup Internal
 */
bool mps_fifo_not_empty (mps_fifo * fifo)
{
   if (*fifo->pwrite_off == *fifo->pread_off)
      return FALSE;

   else
      return TRUE;
}

#ifdef __LIBRARY__
bool check_mps_fifo_not_empty(void){
	return mps_fifo_not_empty(mps_dev.mb.upstrm_fifo);
}
#endif
/**
 * Check FIFO for free memory
 * This function returns the amount of free bytes in FIFO.
 *
 * \param   fifo     Pointer to FIFO structure
 * \return  0        The FIFO is full,
 * \return  count    The number of available bytes
 * \ingroup Internal
 */
uint32_t mps_fifo_mem_available (mps_fifo * fifo)
{
   uint32_t retval;

   retval =
      (fifo->size - 1 - (*fifo->pread_off - *fifo->pwrite_off)) & (fifo->size -
                                                                   1);
   return (retval);
}


/**
 * Check FIFO for requested amount of memory
 * This function checks whether the requested FIFO is capable to store
 * the requested amount of data bytes.
 * The selected Fifo should be a downstream direction Fifo.
 *
 * \param   fifo     Pointer to mailbox structure to be checked
 * \param   bytes    Requested data bytes
 * \return  1        TRUE if space is available in FIFO,
 * \return  0        FALSE if not enough space in FIFO.
 * \ingroup Internal
 */
bool mps_fifo_mem_request (mps_fifo * fifo, uint32_t bytes)
{
   uint32_t bytes_avail = mps_fifo_mem_available (fifo);

   if (bytes_avail > bytes)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}


/**
 * Update FIFO read pointer
 * This function updates the position of the referenced FIFO.In case of
 * reaching the FIFO's end the pointer is set to the start position.
 *
 * \param   fifo      Pointer to FIFO structure
 * \param   increment Increment for read index
 * \ingroup Internal
 */
void mps_fifo_read_ptr_inc (mps_fifo * fifo, uint8_t increment)
{
   int32_t new_read_index =
      (int32_t) (*fifo->pread_off) - (int32_t) increment;

   if ((uint32_t) increment > fifo->size)
   {
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("%s(): Invalid offset passed: %d !\n", __FUNCTION__, increment));
      return;
   }

   if (new_read_index >= 0)
   {
      *(fifo->pread_off) = (uint32_t) new_read_index;
   }
   else
   {
      *(fifo->pread_off) = (uint32_t) (new_read_index + (int32_t) (fifo->size));        /* overflow */
   }

   return;
}


/**
 * Update FIFO write pointer
 * This function updates the position of the write pointer of the referenced FIFO.
 * In case of reaching the FIFO's end the pointer is set to the start position.
 *
 * \param   fifo      Pointer to FIFO structure
 * \param   increment Increment of write index
 * \ingroup Internal
 */
void mps_fifo_write_ptr_inc (mps_fifo * fifo, u16 increment)
{
   /* calculate new index ignoring ring buffer overflow */
   int32_t new_write_index =
      (int32_t) (*fifo->pwrite_off) - (int32_t) increment;

   if ((uint32_t) increment > fifo->size)
   {
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("%s(): Invalid offset passed: %d !\n", __FUNCTION__, increment));
      return;
   }

   if (new_write_index >= 0)
   {
      *fifo->pwrite_off = (uint32_t) new_write_index;       /* no overflow */
   }
   else
   {
      *fifo->pwrite_off =
         (uint32_t) (new_write_index + (int32_t) (fifo->size));
   }
   return;
}


/**
 * Write data word to FIFO
 * This function writes a data word (32bit) to the referenced FIFO. The word is
 * written to the position defined by the current write pointer index and the
 * offset being passed.
 *
 * \param   fifo           Pointer to FIFO structure
 * \param   data           Data word to be written
 * \param   offset         Byte offset to be added to write pointer position
 * \return  0              SUCCESS, word written
 * \return  -1             Invalid offset.
 * \ingroup Internal
 */
int32_t mps_fifo_write (mps_fifo * fifo, uint32_t data,
                                uint8_t offset)
{
   /* calculate write position */
   int32_t new_write_index =
      (int32_t) * fifo->pwrite_off - (int32_t) offset;
   uint32_t write_address;

   if (offset > fifo->size)
   {
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("%s(): Invalid offset passed !\n", __FUNCTION__));
      return -1;
   }
   write_address =
      (uint32_t) fifo->pend + *fifo->pwrite_off - (uint32_t) offset;
   if (new_write_index < 0)
   {
      write_address += fifo->size;
   }
   *(uint32_t *) write_address = data;
   return 0;
}



/**
 * Read data word from FIFO
 * This function reads a data word (32bit) from the referenced FIFO. It first
 * calculates and checks the address defined by the FIFO's read index and passed
 * offset. The read pointer is not updated by this function.
 * It has to be updated after the complete message has been read.
 *
 * \param   fifo          Pointer to FIFO structure
 * \param   offset        Offset to read pointer position to be read from
 * \return  count         Number of data words read.
 * \return  -1            Invalid offset
 * \ingroup Internal
 */
int32_t mps_fifo_read (mps_fifo * fifo, uint8_t offset,
                               uint32_t * pData)
{
   uint32_t read_address;
   int32_t new_read_index =
      ((int32_t) * fifo->pread_off) - (int32_t) offset;
   int32_t ret;

   if (!mps_fifo_not_empty (fifo))
   {
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("%s(): FIFO %p is empty\n", __FUNCTION__, fifo));
      ret = ERROR;
   }
   else
   {
      if (offset > fifo->size)
      {
         TRACE (MPS2, DBG_LEVEL_HIGH,
                ("%s(): Invalid offset passed: %d !\n", __FUNCTION__, offset));
         return -1;
      }
      read_address =
         (uint32_t) fifo->pend + (uint32_t) * fifo->pread_off -
         (uint32_t) offset;
      if (new_read_index < 0)
      {
         read_address += fifo->size;
      }
      *pData = *(uint32_t *) read_address;
      ret = SUCCESS;
   }
   return (ret);
}

#ifndef  __LIBRARY__
/******************************************************************************
 * Global Routines
 ******************************************************************************/

/**
 * Open MPS device
 * Open routine for the MPS device driver.
 *
 * \param   mps_device  MPS communication device structure
 * \param   pMBDev      Pointer to mailbox device structure
 * \return  0           SUCCESS, successfully opened
 * \return  -1          ERROR, Driver already installed
 * \ingroup Internal
 */
int32_t mps_common_open (mps_comm_dev * mps_device,
                                 mps_mbx_dev * pMBDev,
                                 bool from_kernel)
{
   unsigned long flags;

   spin_lock_irqsave (&g_mps2_lock,flags);

   /* device is already installed or unknown device ID used */
   if ((pMBDev->Installed == TRUE) || (pMBDev->devID == 0))
   {
      spin_unlock_irqrestore (&g_mps2_lock,flags);
      return (ERROR);
   }
   pMBDev->Installed = TRUE;
   spin_unlock_irqrestore (&g_mps2_lock,flags);
      if (from_kernel)
      {
         pMBDev->upstrm_fifo = &mps_device->upstrm_fifo;
         pMBDev->dwstrm_fifo = &mps_device->dwstrm_fifo;
      }
      else
      {
         pMBDev->upstrm_fifo = &mps_device->sw_upstrm_fifo;
         pMBDev->dwstrm_fifo = &mps_device->dwstrm_fifo;
      }

   return (SUCCESS);
}

/**
 * Register callback.
 * Allows the upper layer to register a callback function either for
 * downstream (tranmsit mailbox space available) or for upstream (read data
 * available)
 *
 * \param   type     DSP device entity ( 1 - command, 2 - voice0, 3 - voice1,
 *                   4 - voice2, 5 - voice3, 6 - voice4 )
 * \param   callback Callback function to register
 * \return  0        SUCCESS, callback registered successfully
 * \return  ENXIO    Wrong DSP device entity (only 1-5 supported)
 * \return  EBUSY    Callback already registered
 * \return  EINVAL   Callback parameter null
 * \ingroup API
*/
int32_t mps_register_callback (void (*callback) (void))
{
   mps_mbx_dev *pMBDev;

   if (callback == NULL)
   {
      return (-EINVAL);
   }

   /* Get corresponding mailbox device structure */
   if ((pMBDev = &mps_dev.mb) == 0)
      return (-ENXIO);
   if (pMBDev->up_callback != NULL)
	  {
      return (-EBUSY);
   }
   else
   {
      pMBDev->up_callback = callback;
   } 
	return (SUCCESS);
}

/**
 * UnRegister callback.
 * Allows the upper layer to unregister a callback function either for
 *
 * \param   type     DSP device entity ( 1 - command, 2 - voice0, 3 - voice1,
 *                   4 - voice2, 5 - voice3, 6 - voice4 )
 * \param   callback Callback function to register
 * \return  0        SUCCESS, callback registered successfully
 * \return  ENXIO    Wrong DSP device entity (only 1-5 supported)
 * \return  EINVAL   nothing to unregister
 * \ingroup API
*/
int32_t mps_unregister_callback (void)
{
   mps_mbx_dev *pMBDev;

   /* Get corresponding mailbox device structure */
   if ((pMBDev = &mps_dev.mb) == 0)
      return (-ENXIO);
   if (pMBDev->up_callback == NULL)
	  {
      return (-EINVAL);
   }
   else
   {
      pMBDev->up_callback = NULL;
   } 
	return (SUCCESS);
}


/**
 * Close routine for MPS device driver
 * This function closes the channel assigned to the passed mailbox
 * device structure.
 *
 * \param   pMBDev   Pointer to mailbox device structure
 * \return  0        SUCCESS, will never fail
 * \ingroup Internal
 */
int32_t mps_common_close (mps_mbx_dev * pMBDev,
                                  bool from_kernel)
{
   MPS_UNUSED(from_kernel);

   /* clean data structures */
   if (pMBDev->Installed == FALSE)
   {
      return (ERROR);
   }
   pMBDev->Installed = FALSE;

   /* Clear the downstream queues for voice fds only */
   {
#ifdef CONFIG_PROC_FS
      pMBDev->upstrm_fifo->min_space = MBX_DATA_UPSTRM_FIFO_SIZE;
      pMBDev->dwstrm_fifo->min_space = MBX_DATA_DNSTRM_FIFO_SIZE;
#endif /* */
      /* clean-up messages left in software fifo... */
      while (mps_fifo_not_empty (pMBDev->upstrm_fifo))
      {
         uint32_t bytes_read;
         MbxMsg_s msg;
         mps_mbx_read_message (pMBDev->upstrm_fifo, &msg, &bytes_read);
         pMBDev->upstrm_fifo->discards++;
      }
      /* reset software fifo... */
      *pMBDev->upstrm_fifo->pwrite_off = (pMBDev->upstrm_fifo->size - 4);
      *pMBDev->upstrm_fifo->pread_off = (pMBDev->upstrm_fifo->size - 4);
   }
   return (SUCCESS);
}
#endif/*ifndef __LIBRARY__*/
/**
 * MPS Structure Initialization
 * This function initializes the data structures of the Multi Processor System
 * that are necessary for inter processor communication
 *
 * \param   pDev     Pointer to MPS device structure to be initialized
 * \return  0        SUCCESS, if initialization was successful
 * \return  -1       ERROR, allocation or semaphore access problem
 * \ingroup Internal
 */
int32_t mps_init_structures (mps_comm_dev * pDev)
{
   mps_mbx_reg *MBX_Memory;

   /* Initialize MPS main structure */
   memset ((void *) pDev, 0, sizeof (mps_comm_dev));
   pDev->base_global = (mps_mbx_reg *) mps_ram_base;
   pDev->flags = 0x00000000;
   MBX_Memory = pDev->base_global;

   /* * Initialize common mailbox definition area which is used by both CPUs
      for MBX communication. These are: mailbox base address, mailbox size, *
      mailbox read index and mailbox write index. for command and voice
      mailbox, * upstream and downstream direction. */
#ifndef VPE0
   memset ( /* avoid to overwrite CPU boot registers */
            (void *) MBX_Memory,
            0,
            sizeof (mps_mbx_reg));
#else
   memset ( /* avoid to overwrite CPU boot registers */
            (void *) MBX_Memory,
            0,
            sizeof (mps_mbx_reg) - (sizeof(uint32_t)));
#endif
   MBX_Memory->MBX_UPSTR_DATA_BASE =
      (uint32_t *) CPHYSADDR ((uint32_t) MBX_UPSTRM_DATA_FIFO_BASE);
   MBX_Memory->MBX_UPSTR_DATA_SIZE = MBX_DATA_UPSTRM_FIFO_SIZE;
   MBX_Memory->MBX_DNSTR_DATA_BASE =
      (uint32_t *) CPHYSADDR ((uint32_t) MBX_DNSTRM_DATA_FIFO_BASE);
   MBX_Memory->MBX_DNSTR_DATA_SIZE = MBX_DATA_DNSTRM_FIFO_SIZE;

   /* set read and write pointers below to the FIFO's uppermost address */
   MBX_Memory->MBX_UPSTR_DATA_READ = (MBX_Memory->MBX_UPSTR_DATA_SIZE - 4);
   MBX_Memory->MBX_UPSTR_DATA_WRITE = MBX_Memory->MBX_UPSTR_DATA_READ;
   MBX_Memory->MBX_DNSTR_DATA_READ = (MBX_Memory->MBX_DNSTR_DATA_SIZE - 4);
   MBX_Memory->MBX_DNSTR_DATA_WRITE = MBX_Memory->MBX_DNSTR_DATA_READ;
#ifndef VPE0
	/*If the buffer management is by external module he should write this adress
		here as well*/
#ifndef __LIBRARY__
	 if(FALSE == mps_ext_bufman ())
	 MBX_Memory->MBX_VPE1_PTR = (uint32_t)fastbuf_pool;
#endif
#endif

   /* voice upstream data mailbox area */
   pDev->upstrm_fifo.pstart =
      (uint32_t *) KSEG1ADDR (MBX_Memory->MBX_UPSTR_DATA_BASE +
                                  ((MBX_Memory->MBX_UPSTR_DATA_SIZE - 4) >> 2));
   pDev->upstrm_fifo.pend =
      (uint32_t *) KSEG1ADDR (MBX_Memory->MBX_UPSTR_DATA_BASE);
   pDev->upstrm_fifo.pwrite_off =
      (uint32_t *) & (MBX_Memory->MBX_UPSTR_DATA_WRITE);
   pDev->upstrm_fifo.pread_off =
      (uint32_t *) & (MBX_Memory->MBX_UPSTR_DATA_READ);
   pDev->upstrm_fifo.size = MBX_Memory->MBX_UPSTR_DATA_SIZE;

#ifdef CONFIG_PROC_FS
   pDev->upstrm_fifo.min_space = MBX_Memory->MBX_UPSTR_DATA_SIZE;
   pDev->upstrm_fifo.discards = 0;
#endif /* */

   /* voice downstream data mailbox area */
   pDev->dwstrm_fifo.pstart =
      (uint32_t *) KSEG1ADDR (MBX_Memory->MBX_DNSTR_DATA_BASE +
                                  ((MBX_Memory->MBX_DNSTR_DATA_SIZE - 4) >> 2));
   pDev->dwstrm_fifo.pend =
      (uint32_t *) KSEG1ADDR (MBX_Memory->MBX_DNSTR_DATA_BASE);
   pDev->dwstrm_fifo.pwrite_off =
      (uint32_t *) & (MBX_Memory->MBX_DNSTR_DATA_WRITE);
   pDev->dwstrm_fifo.pread_off =
      (uint32_t *) & (MBX_Memory->MBX_DNSTR_DATA_READ);
   pDev->dwstrm_fifo.size = MBX_Memory->MBX_DNSTR_DATA_SIZE;

#ifdef CONFIG_PROC_FS
   pDev->dwstrm_fifo.min_space = MBX_Memory->MBX_UPSTR_DATA_SIZE;
#endif /* */

   /* configure voice channel communication structure fields that are common to
      all voice channels */
   {
      /* voice upstream data software fifo */
      pDev->sw_upstrm_fifo.pend =
         kmalloc(MBX_Memory->MBX_UPSTR_DATA_SIZE + 8,GFP_ATOMIC);
      pDev->sw_upstrm_fifo.pstart =
         (pDev->sw_upstrm_fifo.pend +
          ((MBX_Memory->MBX_UPSTR_DATA_SIZE - 4) >> 2));
      pDev->sw_upstrm_fifo.pwrite_off =
         (pDev->sw_upstrm_fifo.pend +
          ((MBX_Memory->MBX_UPSTR_DATA_SIZE) >> 2));
      *pDev->sw_upstrm_fifo.pwrite_off =
         (MBX_Memory->MBX_UPSTR_DATA_SIZE - 4);
      pDev->sw_upstrm_fifo.pread_off =
         (pDev->sw_upstrm_fifo.pend +
          ((MBX_Memory->MBX_UPSTR_DATA_SIZE + 4) >> 2));
      *pDev->sw_upstrm_fifo.pread_off =
         (MBX_Memory->MBX_UPSTR_DATA_SIZE - 4);
      pDev->sw_upstrm_fifo.size = MBX_Memory->MBX_UPSTR_DATA_SIZE;
#ifdef CONFIG_PROC_FS
      pDev->sw_upstrm_fifo.min_space = MBX_Memory->MBX_UPSTR_DATA_SIZE;
      pDev->sw_upstrm_fifo.discards = 0;
#endif /* */
      memset ((void *) pDev->sw_upstrm_fifo.pend, 0x00,
              MBX_Memory->MBX_UPSTR_DATA_SIZE);

      /* upstrm fifo pointer might be changed on open... */
      pDev->mb.upstrm_fifo = &pDev->upstrm_fifo;
      pDev->mb.dwstrm_fifo = &pDev->dwstrm_fifo;
      pDev->mb.Installed = FALSE;  /* current mbx installation
                                                   status */
      /* select mechanism implemented for each queue */
      init_waitqueue_head(&(pDev->mb.mps_wakeuplist));
   }

   /* set channel identifiers */
   pDev->mb.devID = 1;
   /* allocate buffer for firmware image data */
   return 0;
}

#ifndef __LIBRARY__
/**
 * MPS Structure Reset
 * This function resets the global structures into inital state
 *
 * \param   pDev     Pointer to MPS device structure
 * \return  0        SUCCESS, if initialization was successful
 * \return  -1       ERROR, allocation or semaphore access problem
 * \ingroup Internal
 */
uint32_t mps_reset_structures (mps_comm_dev * pDev)
{
#ifdef CONFIG_PROC_FS
   pDev->dwstrm_fifo.min_space = pDev->dwstrm_fifo.size;
   pDev->dwstrm_fifo.bytes = 0;
   pDev->dwstrm_fifo.pkts = 0;
   pDev->dwstrm_fifo.discards = 0;
#endif /* */
   {
      mps_fifo_clear (pDev->mb.dwstrm_fifo);
      mps_fifo_clear (pDev->mb.upstrm_fifo);

#ifdef CONFIG_PROC_FS
      pDev->mb.upstrm_fifo->min_space =
         pDev->mb.upstrm_fifo->size;
      pDev->mb.upstrm_fifo->bytes = 0;
      pDev->mb.upstrm_fifo->pkts = 0;
      pDev->mb.upstrm_fifo->discards = 0;
#endif /* */
   }
   return SUCCESS;
}

#endif/* __LIBRARY__*/

/**
 * Read message from upstream data mailbox
 * This function reads a complete data message from the upstream data mailbox.
 * It reads the header checks how many payload words are included in the message
 * and reads the payload afterwards. The mailbox's read pointer is updated afterwards
 * by the amount of words read.
 *
 * \param   fifo        Pointer to mailbox structure to be read from
 * \param   msg         Pointer to message structure read from buffer
 * \param   bytes       Pointer to number of bytes included in read message
 * \return  0           SUCCESS, successful read operation,
 * \return  -1          Invalid length field read.
 * \ingroup Internal
 */
int32_t mps_mbx_read_message (mps_fifo * fifo, MbxMsg_s * msg,
                                      uint32_t * bytes)
{
   int32_t i, j,ret;
#ifndef __LIBRARY__
   unsigned long flags;
   spin_lock_irqsave (&g_mps2_lock_read,flags);
#endif
	 ret = mps_fifo_read (fifo, (uint8_t)0, &msg->header.val);
	 if (ret == ERROR)
   {
#ifndef __LIBRARY__
    spin_unlock_irqrestore (&g_mps2_lock_read,flags);
#endif
   	return ret;
   }
	 j=MBX_LENGTH-4;
   for (i = 0; i < j; i += 4)      /* read message payload
                                                         */
   {
      ret = mps_fifo_read (fifo, (uint8_t) (i + 4), &msg->data[i / 4]);
      if (ret == ERROR)
      {
#ifndef __LIBRARY__
         spin_unlock_irqrestore (&g_mps2_lock_read,flags);
#endif
         return ret;
      }
   }
   *bytes = MBX_LENGTH;
   mps_fifo_read_ptr_inc (fifo,MBX_LENGTH);
#ifndef __LIBRARY__
   spin_unlock_irqrestore (&g_mps2_lock_read,flags);
#endif
   return SUCCESS;
}


/**
 * Read message from FIFO
 * This function reads a message from the upstream data mailbox and passes it
 * to the calling function. A call to the notify_upstream function will trigger
 * another wakeup in case there is already more data available.
 *
 * \param   pMBDev   Pointer to mailbox device structure
 * \param   pPkg     Pointer to data transfer structure (output parameter)
 * \param   timeout  Currently unused
 * \return  0        SUCCESS, successful read operation,
 * \return  -1       ERROR, in case of read error.
 * \return  -ENODATA No data was available
 * \return  -EBADMSG Accidential read of buffer message
 * \ingroup Internal
 */
int32_t mps_mbx_read (mps_mbx_dev * pMBDev, mps_message * pPkg,
                              int32_t timeout)
{
   MbxMsg_s msg;
   uint32_t bytes = 0;
   mps_fifo *fifo;
   int32_t retval = ERROR;
#ifndef __LIBRARY__
	uint32_t i;
#endif

   MPS_UNUSED(timeout);

   fifo = pMBDev->upstrm_fifo;
   memset (&msg, 0, sizeof (msg));      /* initialize msg pointer */
   if (!mps_fifo_not_empty (fifo))
   {
      /* Nothing available for this channel... */
      return -ENODATA;
   }

   /* read message from mailbox */
   if (mps_mbx_read_message (fifo, &msg, &bytes) == 0)
   {
      {
				memcpy(pPkg,&msg,sizeof(msg));
#if !defined(__LIBRARY__)
				for(i=0;i<MAX_UPSTRM_DATAWORDS;i++){
					if(CHECK_PTR(pPkg->header.Hd.param_attr,i)){
#ifdef SYSTEM_GRX500
						if(CHECK_PTR_IOCU(pPkg->header.Hd.param_attr,i)) 
									pPkg->data[i]=RPHYSADDR(pPkg->data[i])|KSEG2;
						else
									pPkg->data[i]=RPHYSADDR(pPkg->data[i])|KSEG0;
#else
									pPkg->data[i]=KSEG0ADDR(pPkg->data[i]);
#endif
					}
				}
#endif
            retval = SUCCESS;
#ifdef CONFIG_PROC_FS
            pMBDev->upstrm_fifo->bytes += bytes;
#endif /* */
      }
   }
   return retval;
}


/**
 * Build 32 bit word starting at byte_ptr.
 * This function builds a 32 bit word out of 4 consecutive bytes
 * starting at byte_ptr position.
 *
 * \param   byte_ptr  Pointer to first byte (most significat 8 bits) of word calculation
 * \return  value     Returns value of word starting at byte_ptr position
 * \ingroup Internal
 */
uint32_t mps_mbx_build_word (uint8_t * byte_ptr)
{
   uint32_t result = 0x00000000;
   int32_t i;

   for (i = 0; i < 4; i++)
   {
      result += (uint32_t) (*(byte_ptr + i)) << ((3 - i) * 8);
   }
   return (result);
}


/**
 * Write to Downstream Mailbox of MPS.
 * This function writes messages into the downstream mailbox to be read
 * by CPU1
 *
 * \param   pMBDev    Pointer to mailbox device structure
 * \param   msg_ptr   Pointer to message
 * \param   msg_bytes Number of bytes in message
 * \return  0         Returns SUCCESS in case of successful write operation
 * \return  -EAGAIN   in case of access fails with FIFO overflow while in irq
 * \return  -EIO      in case of access fails with FIFO overflow in task context
 * \ingroup Internal
 */
int32_t mps_mbx_write_message (mps_mbx_dev * pMBDev,
                                       uint8_t * msg_ptr,
                                       uint32_t msg_bytes)
{
   mps_fifo *mbx;
   uint32_t i;
#ifndef __LIBRARY__
   unsigned long flags;
   MPS_Ad0Reg_u Ad0Reg;
#endif
   int32_t retval = -EAGAIN;
   int32_t retries = 0;
   uint32_t word = 0;
   bool word_aligned = TRUE;
   static uint32_t trace_fag;

#ifndef __LIBRARY__
   spin_lock_irqsave (&g_mps2_lock_write,flags);
#endif
   mbx = pMBDev->dwstrm_fifo;   /* set pointer to downstream mailbox FIFO
                                   structure */
   if ((uint32_t) msg_ptr & 0x00000003)
   {
      word_aligned = FALSE;
      TRACE (MPS2, DBG_LEVEL_HIGH,
             ("%s(): Passed message not word aligned !!!\n", __FUNCTION__));
   }

   /* request for downstream mailbox buffer memory, make MAX_FIFO_WRITE_RETRIES
      attempts in case not enough memory is not available */
   while (++retries <= MAX_FIFO_WRITE_RETRIES)
   {
      if (mps_fifo_mem_request (mbx, msg_bytes) == TRUE)
      {
         trace_fag = 0;
         break;
      }
#ifndef __LIBRARY__

      if (in_interrupt ())
      {
         retries = MAX_FIFO_WRITE_RETRIES + 1;
         break;
      }
      else
      {
         {
            spin_unlock_irqrestore (&g_mps2_lock_write,flags);
            udelay (125);
            spin_lock_irqsave (&g_mps2_lock_write,flags);
         }
      }
#endif
   }

   if (retries <= MAX_FIFO_WRITE_RETRIES)
   {
      /* write message words to mailbox buffer starting at write pointer
         position and update the write pointer index by the amount of written
         data afterwards */
      for (i = 0; i < msg_bytes; i += 4)
      {
         if (word_aligned)
            mps_fifo_write (mbx, *(uint32_t *) (msg_ptr + i), i);
         else
         {
            word = mps_mbx_build_word (msg_ptr + i);
            mps_fifo_write (mbx, word, i);
         }
      }
      mps_fifo_write_ptr_inc (mbx, msg_bytes);
#ifndef __LIBRARY__
#ifndef VPE0
   		Ad0Reg.val = *MPS_RAD0SR;
#else
   		Ad0Reg.val = *MPS_RAD1SR;
#endif
   		Ad0Reg.fld.du_mbx = 1;
#ifndef VPE0
      *MPS_SAD0SR |= Ad0Reg.val;
#else
      *MPS_SAD1SR |= Ad0Reg.val;
#endif			
#endif

      retval = SUCCESS;

#ifdef CONFIG_PROC_FS
      pMBDev->dwstrm_fifo->pkts++;
      pMBDev->dwstrm_fifo->bytes += msg_bytes;
      if (mbx->min_space > mps_fifo_mem_available (mbx))
         mbx->min_space = mps_fifo_mem_available (mbx);
#endif /* CONFIG_PROC_FS */
   }
   else
   {
      /* insufficient space in the mailbox for writing the data */

      /** \todo update error statistics */

      if (!trace_fag)           /* protect from trace flood */
      {
         TRACE (MPS2, DBG_LEVEL_LOW,
                ("%s(): write message timeout\n", __FUNCTION__));

         /* trace only once until write succeeds at least one time */
         trace_fag = 1;
      }
#ifndef __LIBRARY__

      /* If the command downstream mailbox stays full for several milliseconds,
         a fatal error has occurred and the voice CPU should be restarted */
      if (!in_interrupt ())
      {

         /* -> return fatal error */
         retval = -EIO;
      }
#endif
   }
#ifndef __LIBRARY__
   spin_unlock_irqrestore (&g_mps2_lock_write,flags);
#endif
   return retval;
}


/**
 * Write to Downstream Data Mailbox of MPS.
 * This function writes the passed message into the downstream data mailbox.
 *
 * \param   pMBDev     Pointer to mailbox device structure
 * \param   readWrite  Pointer to message structure
 * \return  0          SUCCESS in case of successful write operation
 * \return  -1         ERROR in case of access fails with FIFO overflow
 * \ingroup Internal
 */
int32_t mps_mbx_write_data (mps_mbx_dev * pMBDev,
                                    mps_message * readWrite)
{
   int32_t retval = ERROR;
	 int32_t i;
   {
  		for(i=0;i<MAX_UPSTRM_DATAWORDS;i++){
  			if(CHECK_PTR(readWrite->header.Hd.param_attr,i)){
#ifdef VPE0
      		readWrite->data[i] = CPHYSADDR ((uint32_t) readWrite->data[i]);
#else
			#ifdef SYSTEM_GRX500
      		readWrite->data[i] = RPHYSADDR ((uint32_t) readWrite->data[i]);
			#else
      		readWrite->data[i] = CPHYSADDR ((uint32_t) readWrite->data[i]);
			#endif
#endif
				}
			}
      if ((retval =
           mps_mbx_write_message (pMBDev, (uint8_t *)readWrite,
                                      MBX_LENGTH)) != SUCCESS)
      {
         TRACE (MPS2, DBG_LEVEL_LOW,
                ("%s(): Writing data failed ! *\n", __FUNCTION__));
      }
   }

   return retval;
}

#ifndef  __LIBRARY__

/**
 * Notify queue about upstream data reception
 * This function checks the channel identifier included in the header
 * of the message currently pointed to by the upstream data mailbox's
 * read pointer. It wakes up the related queue to read the received data message
 * out of the mailbox for further processing. The process is repeated
 * as long as upstream messages are avaiilable in the mailbox.
 * The function is attached to the driver's poll/select functionality.
 *
 * \param   dummy    Tasklet parameter, not used.
 * \ingroup Internal
 */
void mps_mbx_data_upstream (ulong dummy)
{
   mps_fifo *mbx;
   mps_mbx_dev *mbx_dev;
   MbxMsg_s msg;
   uint32_t bytes_read = 0;
   unsigned long flags;
   int32_t ret;

   MPS_UNUSED(dummy);

   /* set pointer to data upstream mailbox*/
   mbx = &pMPSDev->upstrm_fifo;
   while (mps_fifo_not_empty (mbx))
   {
      /* select mailbox device structure acc. to channel ID read from current
         msg */
      {
            mbx_dev = (mps_mbx_dev *) & (pMPSDev->mb);
      }
			
#ifdef CONFIG_PROC_FS
      if (mbx->min_space > mps_fifo_mem_available (mbx))
         mbx->min_space = mps_fifo_mem_available (mbx);
      mbx->pkts++;
      mbx->bytes +=  MBX_LENGTH;
#endif /* */
      {
         /* discard packet in case no one is listening... */
         if (mbx_dev->Installed == FALSE)
         {
          data_upstream_discard:
            mps_mbx_read_message (mbx, &msg, &bytes_read);
            mbx_dev->upstrm_fifo->discards++;
            continue;
         }
				 if (mbx_dev->up_callback != NULL)
				 {
						spin_lock_irqsave (&g_mps2_lock,flags);
  					mbx_dev->up_callback();
						spin_unlock_irqrestore(&g_mps2_lock,flags);
						continue;
  			 }
				 else
         {
            int32_t i, msg_bytes;

            msg_bytes = MBX_LENGTH;
            if (mps_fifo_mem_request (mbx_dev->upstrm_fifo, msg_bytes) !=
                TRUE)
            {
               goto data_upstream_discard;
            }
						spin_lock_irqsave (&g_mps2_lock,flags);
            /* Copy message into sw fifo */
            for (i = 0; i < msg_bytes; i += 4)
            {
               uint32_t data;

               ret = mps_fifo_read (mbx, (uint8_t) i, &data);
               if (ret == ERROR){
                  return;
							 }
               mps_fifo_write (mbx_dev->upstrm_fifo, data, (uint8_t) i);
            }
            mps_fifo_read_ptr_inc (mbx, msg_bytes);
            mps_fifo_write_ptr_inc (mbx_dev->upstrm_fifo, msg_bytes);
						spin_unlock_irqrestore(&g_mps2_lock,flags);
#ifdef CONFIG_PROC_FS
            if (mbx_dev->upstrm_fifo->min_space >
                mps_fifo_mem_available (mbx_dev->upstrm_fifo))
               mbx_dev->upstrm_fifo->min_space =
                  mps_fifo_mem_available (mbx_dev->upstrm_fifo);
            mbx_dev->upstrm_fifo->pkts++;
#endif /* CONFIG_PROC_FS */
						mpsexcpt=1;
            /* use queue wake up to notify about data reception */
            wake_up_interruptible(&(mbx_dev->mps_wakeuplist));
         }
      }
   }
   return;
}

/**
   This function enables mailbox interrupts on Danube.
\param
   None.
\return
   None.
*/

void mps_enable_mailbox_int ()
{
   MPS_Ad0Reg_u Ad0Reg;
#ifdef VPE0
   Ad0Reg.val = *MPS_AD0ENR;
#else
   Ad0Reg.val = *MPS_AD1ENR;
#endif
   Ad0Reg.fld.du_mbx = 1;
#ifdef VPE0
   *MPS_AD0ENR = Ad0Reg.val;
#else
   *MPS_AD1ENR = Ad0Reg.val;
#endif
}

/**
   This function disables mailbox interrupts on Danube.
\param
   None.
\return
   None.
*/
void mps_disable_mailbox_int ()
{
   MPS_Ad0Reg_u Ad0Reg;

#ifdef VPE0
   Ad0Reg.val = *MPS_AD0ENR;
#else
   Ad0Reg.val = *MPS_AD1ENR;
#endif
   Ad0Reg.fld.du_mbx = 0;
#ifdef VPE0
   *MPS_AD0ENR = Ad0Reg.val;
#else
   *MPS_AD1ENR = Ad0Reg.val;
#endif
}

/**
   This function disables all MPS interrupts on Danube.
\param
   None.
\return
   None.
*/
void mps_disable_all_int ()
{
#ifdef VPE0
   *MPS_SAD0SR = 0x00000000;
#else
   *MPS_SAD1SR = 0x00000000;
#endif
}

/******************************************************************************
 * Interrupt service routines
 ******************************************************************************/

/**
 * Upstream data interrupt handler
 * This function is called on occurence of an data upstream interrupt.
 * Depending on the occured interrupt either the command or data upstream
 * message processing is started via tasklet
 *
 * \param   irq      Interrupt number
 * \param   pDev     Pointer to MPS communication device structure
 * \ingroup Internal
 */
irqreturn_t mps_ad0_irq (int32_t irq, mps_comm_dev * pDev)
{
   MPS_Ad0Reg_u MPS_Ad0StatusReg;

   MPS_UNUSED(pDev);
#ifdef VPE0
   /* read interrupt status */
   MPS_Ad0StatusReg.val = *MPS_RAD0SR;
   /* acknowledge interrupt */
   *MPS_CAD0SR = MPS_Ad0StatusReg.val;
   /* handle only enabled interrupts */
   MPS_Ad0StatusReg.val &= *MPS_AD0ENR;
#else
	 /* read interrupt status */
   MPS_Ad0StatusReg.val = *MPS_RAD1SR;
   /* acknowledge interrupt */
   *MPS_CAD1SR = MPS_Ad0StatusReg.val;
   /* handle only enabled interrupts */
   MPS_Ad0StatusReg.val &= *MPS_AD1ENR;
#endif
   if (MPS_Ad0StatusReg.fld.du_mbx)
   {
#ifdef CONFIG_PROC_FS
      pMPSDev->mb.upstrm_fifo->pkts++;
#endif /* CONFIG_PROC_FS */
      mps_mbx_data_upstream (0);
   }

   return IRQ_HANDLED;
}
/**
 * Issue a Hit_Invalidate_D operation on a data segment of a
 * given size at given address. Address range is aligned to the
 * multiple of a cache line size.
 *
 * \param   addr    Address of a data segment
 * \param   len     Length in bytes of a data segment
 * \return  none
 * \ingroup Internal
 * \remarks addr parameter must be in KSEG0
 */
void cache_inv (ulong addr, uint32_t len)
{
   ulong aline = addr & ~(CACHE_LINE_SZ - 1);
   ulong end   = (addr + len - 1) & ~(CACHE_LINE_SZ - 1);

	 if(addr<KSEG0||addr>KSEG1){
			TRACE (MPS2, DBG_LEVEL_HIGH, ("Invalid address for cache ops %#lx\n",addr));
			return;
	 }

   while (1)
   {
      __asm__ __volatile__(
         " .set    push        \n"
         " .set    noreorder   \n"
         " .set    mips3\n\t   \n"
         " cache   0x11, 0(%0) \n"
         " .set    pop         \n"
         : :"r"(aline));

#ifndef VPE0
      __asm__ __volatile__(
         " .set    push        \n"
         " .set    noreorder   \n"
         " .set    mips3\n\t   \n"
         " cache   0x13, 0(%0) \n"
         " .set    pop         \n"
         : :"r"(aline));
#endif

      if (aline == end)
         break;

      aline += CACHE_LINE_SZ;
   }
}

/**
 * Issue a Hit_Writeback_Inv_D operation on a data segment of a
 * given size at given address. Address range is aligned to the
 * multiple of a cache line size.
 *
 * \param   addr    Address of a data segment
 * \param   len     Length in bytes of a data segment
 * \return  none
 * \ingroup Internal
 * \remarks addr parameter must be in KSEG0
 */
void cache_wb_inv (ulong addr, uint32_t len)
{
   ulong aline = addr & ~(CACHE_LINE_SZ - 1);
   ulong end   = (addr + len - 1) & ~(CACHE_LINE_SZ - 1);
	 if(addr<KSEG0||addr>KSEG1){
			TRACE (MPS2, DBG_LEVEL_HIGH, ("Invalid address for cache ops %#lx\n",addr));
			return;
	 }
   while (1)
   {
      __asm__ __volatile__(
         " .set    push            \n"
         " .set    noreorder       \n"
         " .set    mips3\n\t       \n"
         " cache   0x15, 0(%0)     \n"
         " .set    pop             \n"
         : :"r" (aline));

#ifndef VPE0
      __asm__ __volatile__(
         " .set    push            \n"
         " .set    noreorder       \n"
         " .set    mips3\n\t       \n"
         " cache   0x17, 0(%0)     \n"
         " .set    pop             \n"
         : :"r" (aline));
#endif
      if (aline == end)
         break;

      aline += CACHE_LINE_SZ;
   }
   /* MIPS multicore write reordering workaround:
      writing to on-chip SRAM and off-chip SDRAM can be reordered in time on
      MIPS multicore, in other words, there is no guarantee that write
      operation to SDRAM is finished at the moment of passing a data pointer to
      voice CPU  through data mailbox in SRAM.
      Workaround sequence:
      1) Write back (and invalidate) all used cache lines
      2) SYNC
      3) Read-back uncahed one word
      4) SYNC
      5) Write data pointer message to the mailbox in the on-chip SRAM */
   __asm__ __volatile__(" sync \n");
   /* dummy read back uncached */
   *((volatile uint32_t *)KSEG1ADDR(aline));
   __asm__ __volatile__(" sync \n");
}
EXPORT_SYMBOL (mps_bufman_register);
EXPORT_SYMBOL (mps_buffer);
EXPORT_SYMBOL (cache_wb_inv);
EXPORT_SYMBOL (cache_inv);
#else 
EXPORT_SYMBOL(check_mps_fifo_not_empty);
#endif /* __LIBRARY__*/
