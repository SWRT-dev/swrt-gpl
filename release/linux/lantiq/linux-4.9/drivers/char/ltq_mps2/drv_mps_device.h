#ifndef _DRV_MPS_VMMC_DEVICE_H
#define _DRV_MPS_VMMC_DEVICE_H
/******************************************************************************

                              Copyright (c) 2013
                            Lantiq Deutschland GmbH
                             http://www.lantiq.com

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

****************************************************************************
   Module      : drv_mps_vmmc_device.h
   Description : This file contains the defines, the structures declarations
                 the tables declarations and the device specific functions
                 declarations.
*******************************************************************************/
//#include <asm/ltq_vpe.h>
#include <irq.h>
//#include <lantiq_irq.h>

/** This variable holds the actual base address of the MPS register block. */
extern uint32_t mps_reg_base;  /* MPS registers */
/** This variable holds the actual base address of the MPS SRAM area. */
extern uint32_t mps_ram_base;  /* MPS memory */
/** This variable holds the interrupt number of the IRQ associated with the
    MPS status register 4 which is used for AFE and DFE 0 status. */
extern uint32_t mps_ir4;       /* MPS AD0 register interrupt */
extern uint32_t mps_ir0;       /* MPS register interrupt */
extern uint32_t mps_ir1;       /* MPS register interrupt */

/* MPS register block */
#ifndef SYSTEM_VR9
	#define MPS_BASE_ADDR        (KSEG1 | 0x1F107400)
	#define MPS_SRAM             ((volatile u32 *)(KSEG1 | 0x1F201000))
#else
	#define MPS_BASE_ADDR        (KSEG1 | 0x1F107000)
	#define MPS_SRAM             ((volatile u32 *)(KSEG1 | 0x1F200000))
#endif
#define MPS_RAD0SR              ((u32 *)(mps_reg_base + 0x0040))
#define MPS_RAD1SR              ((u32 *)(mps_reg_base + 0x0084))
#define MPS_SAD0SR              ((u32 *)(mps_reg_base + 0x0048))
#define MPS_SAD1SR              ((u32 *)(mps_reg_base + 0x0070))
#define MPS_CAD0SR              ((u32 *)(mps_reg_base + 0x0050))
#define MPS_CAD1SR              ((u32 *)(mps_reg_base + 0x0080))
#define MPS_AD0ENR              ((u32 *)(mps_reg_base + 0x0058))
#define MPS_AD1ENR              ((u32 *)(mps_reg_base + 0x0074))

#ifndef SYSTEM_VR9
/* Interrupt vectors */
	#define MPS_IR4  /* AD0 */          						 22
	#define MPS_IR0  /* Global interrupt */          223
	#define MPS_IR1  /* Global interrupt */          224
#else
/* Interrupt vectors */
	#define MPS_IR4  /* AD0 */                       154
	#define MPS_IR0  /* Global interrupt */          8
	#define MPS_IR1  /* Global interrupt */          9
#endif
/* ============================= */
/* MPS Common defines            */
/* ============================= */
/*---------------------------------------------------------------------------*/
/* Mailbox definitions                                                       */
/*---------------------------------------------------------------------------*/
#ifdef VPE0
	#if defined(SYSTEM_GRX500)||defined(SYSTEM_4KEC)
		#define MBX_DATA_UPSTRM_FIFO_SIZE 496
		#define MBX_DATA_DNSTRM_FIFO_SIZE 492
	#else
		#define MBX_DATA_UPSTRM_FIFO_SIZE 240
		#define MBX_DATA_DNSTRM_FIFO_SIZE 236
	#endif
#define MBX_RW_POINTER_AREA_SIZE 32
/* base addresses for mailboxes (upstream and downstream ) */
#define MBX_UPSTRM_DATA_FIFO_BASE   (mps_ram_base + MBX_RW_POINTER_AREA_SIZE)
#define MBX_DNSTRM_DATA_FIFO_BASE   (MBX_UPSTRM_DATA_FIFO_BASE + MBX_DATA_UPSTRM_FIFO_SIZE)
#else
	#if defined(SYSTEM_GRX500)||defined(SYSTEM_4KEC)
		#define MBX_DATA_UPSTRM_FIFO_SIZE 492
		#define MBX_DATA_DNSTRM_FIFO_SIZE 496
	#else
		#define MBX_DATA_UPSTRM_FIFO_SIZE 236
		#define MBX_DATA_DNSTRM_FIFO_SIZE 240
	#endif
#define MBX_RW_POINTER_AREA_SIZE 32
/* base addresses for mailboxes (upstream and downstream ) */
#define MBX_DNSTRM_DATA_FIFO_BASE   (mps_ram_base + MBX_RW_POINTER_AREA_SIZE)
#define MBX_UPSTRM_DATA_FIFO_BASE   (MBX_DNSTRM_DATA_FIFO_BASE + MBX_DATA_DNSTRM_FIFO_SIZE)
#endif


#if defined(SYSTEM_GRX500)||defined(SYSTEM_4KEC)
	#define MBX_DATA_WORDS 247
#else 
	#define MBX_DATA_WORDS 119
#endif


#define MAX_FIFO_WRITE_RETRIES 80
#define MBX_LENGTH 20

/*---------------------------------------------------------------------------*/
/* MPS buffer provision management structure definitions                   */
/*---------------------------------------------------------------------------*/

#define MPS_BUFFER_INITIAL   MEM_SIZE 
#define MPS_MEM_SEG_DATASIZE MEM_SEG_SIZE

/*---------------------------------------------------------------------------*/
/* DEVICE DEPENDENCIES                                                       */
/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/
/* Register structure definitions                                            */
/*---------------------------------------------------------------------------*/
typedef enum
{
   UPSTREAM,
   DOWNSTREAM
} MbxDirection_e;

typedef struct
{
#ifdef CONFIG_CPU_BIG_ENDIAN
   uint32_t src_id:8;
   uint32_t dst_id:8;
   uint32_t msg_id:8;
   uint32_t param_attr:8;
#else
   uint32_t param_attr:8;
   uint32_t msg_id:8;
   uint32_t dst_id:8;
   uint32_t src_id:8;
#endif
}MbxMsgHd;

typedef union
{
   uint32_t val;
	 MbxMsgHd Hd;
} MbxMsgHd_u;
 

typedef struct
{
   MbxMsgHd_u header;
   uint32_t data[MAX_UPSTRM_DATAWORDS];
} MbxMsg_s;

/*---------------------------------------------------------------------------*/
/* FIFO structure                                                            */
/*---------------------------------------------------------------------------*/
typedef struct
{
   volatile uint32_t *volatile pstart;     /**< Pointer to FIFO's read/write start address */
   volatile uint32_t *volatile pend;       /**< Pointer to FIFO's read/write end address */
   volatile uint32_t *volatile pwrite_off; /**< Pointer to FIFO's write index location */
   volatile uint32_t *volatile pread_off;  /**< Pointer to FIFO's read index location */
   volatile uint32_t size;        /**< FIFO size */
   volatile uint32_t min_space;   /**< FIFO size */
   volatile uint32_t bytes;
   volatile uint32_t pkts;
   volatile uint32_t discards;
} mps_fifo;

/*
 * This structure represents the MPS mailbox definition area that is shared
 * by CCPU and VCPU. It comprises the mailboxes' base addresses and sizes in bytes as well as the
 *
 *
 */
typedef struct
{
#ifdef VPE0
   volatile uint32_t *MBX_UPSTR_DATA_BASE; /**< Upstream Data FIFO Base Address */
   volatile uint32_t MBX_UPSTR_DATA_SIZE;  /**< Upstream Data FIFO size in byte */
   volatile uint32_t *MBX_DNSTR_DATA_BASE; /**< Downstream Data FIFO Base Address */
   volatile uint32_t MBX_DNSTR_DATA_SIZE;  /**< Downstream Data FIFO size in byte */
   volatile uint32_t MBX_UPSTR_DATA_READ;   /**< Upstream Data FIFO Read Index */
   volatile uint32_t MBX_UPSTR_DATA_WRITE;  /**< Upstream Data FIFO Write Index */
   volatile uint32_t MBX_DNSTR_DATA_READ;   /**< Downstream Data FIFO Read Index */
   volatile uint32_t MBX_DNSTR_DATA_WRITE;  /**< Downstream Data FIFO Write Index */
#else
   volatile uint32_t *MBX_DNSTR_DATA_BASE; /**< Downstream Data FIFO Base Address */
   volatile uint32_t MBX_DNSTR_DATA_SIZE;  /**< Downstream Data FIFO size in byte */
   volatile uint32_t *MBX_UPSTR_DATA_BASE; /**< Upstream Data FIFO Base Address */
   volatile uint32_t MBX_UPSTR_DATA_SIZE;  /**< Upstream Data FIFO size in byte */
   volatile uint32_t MBX_DNSTR_DATA_READ;   /**< Downstream Data FIFO Read Index */
   volatile uint32_t MBX_DNSTR_DATA_WRITE;  /**< Downstream Data FIFO Write Index */
   volatile uint32_t MBX_UPSTR_DATA_READ;   /**< Upstream Data FIFO Read Index */
   volatile uint32_t MBX_UPSTR_DATA_WRITE;  /**< Upstream Data FIFO Write Index */
#endif
   volatile uint32_t MBX_DATA[MBX_DATA_WORDS];
   volatile uint32_t MBX_VPE1_PTR;  				/**< Downstream Data FIFO Write Index */
} mps_mbx_reg;

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Device connection structure                                               */
/*---------------------------------------------------------------------------*/

/**
 * Mailbox Device Structure.
 * This Structure holds top level parameters of the mailboxes used to allow
 * the communication between the control CPU and the Voice CPU
 */
typedef struct
{
   /* Wakeuplist for the select mechanism */
   wait_queue_head_t  mps_wakeuplist;
   mps_fifo *upstrm_fifo;    /**< Data exchange FIFO for read (upstream) */
   mps_fifo *dwstrm_fifo;    /**< Data exchange FIFO for write (downstream) */
	 void (*up_callback) (void);
	 int32_t devID;
   volatile int32_t Installed;
   struct cdev *mps_cdev;
} mps_mbx_dev;

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Device structure                                                          */
/*---------------------------------------------------------------------------*/

/**
 * Mailbox Device Structure.
 * This Structure represents the communication device that provides the resources
 * for the communication between CPU0 and CPU1
 */
typedef struct
{
   mps_mbx_reg *base_global;   /**< global register pointer for the ISR */
   uint32_t flags;                   /**< Pointer to private date of the specific handler */
   mps_mbx_dev mb;     /**< Data upstream and downstream mailboxes */
   mps_fifo upstrm_fifo;
   mps_fifo dwstrm_fifo;
   mps_fifo sw_upstrm_fifo;
} mps_comm_dev;

/*---------------------------------------------------------------------------*/
int32_t mps_common_open (mps_comm_dev * pDev, mps_mbx_dev * pMBDev,
                                 bool from_kernel);
int32_t mps_common_close (mps_mbx_dev * pMBDev,
                                  bool from_kernel);
int32_t mps_mbx_read (mps_mbx_dev * pMBDev, mps_message * pPkg,
                              int32_t timeout);
int32_t mps_mbx_write_data (mps_mbx_dev * pMBDev,
                                    mps_message * readWrite);
int32_t mps_init_structures (mps_comm_dev * pDev);
uint32_t mps_fifo_mem_available (mps_fifo * mbx);
int32_t mps_bufman_init (void);
void mps_bufman_free (const void * ptr);
void *mps_bufman_malloc (size_t size, int32_t priority);
void mps_disable_mailbox_int (void);
void mps_disable_all_int (void);
void mps_enable_mailbox_int (void);
DECLARE_TRACE_GROUP (MPS2);
#endif /* _DRV_MPS_VMMC_DEVICE_H */
