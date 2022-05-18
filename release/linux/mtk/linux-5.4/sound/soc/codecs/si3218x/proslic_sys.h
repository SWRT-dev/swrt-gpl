/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __PROSLIC_SYS_HDR__
#define __PROSLIC_SYS_HDR__ 1
#include <linux/types.h>
#include <linux/time64.h> /* for struct timespec */

#include <linux/spi/spi.h>

#include "proslic_sys_cfg.h"



/* Do a quick sanity check on  channel setting */
#define SILABS_MAX_CHAN 32 
#if (SILABS_MAX_CHANNELS > SILABS_MAX_CHAN)
#error "ProSLIC only supports up to 32 channels"
#endif

#define PROSLIC_BCAST       0xFF
#define PROSLIC_SPI_OK      0
#define PROSLIC_SPI_NOK     8 /* This is what the ProSLIC API calls SPI_FAIL */ 
#define PROSLIC_TIMER_ERROR 15

/* Debug macros */
#define PROSLIC_API_HDR "PROSLIC_API "

#define proslic_trace(fmt, arg...) if(proslic_debug_setting & 0x1) printk( KERN_NOTICE PROSLIC_API_HDR "TRC: " fmt "\n", ##arg)
#define proslic_debug(fmt, arg...) if(proslic_debug_setting & 0x2) printk( KERN_DEBUG PROSLIC_API_HDR "DBG: " fmt "\n", ##arg)
#define  proslic_error(fmt, arg...) if(proslic_debug_setting & 0x4) printk( KERN_ERR PROSLIC_API_HDR "ERR: " fmt "\n", ##arg)

/* Data types compatible with the ProSLIC API system services layer */
typedef u_int8_t            BOOLEAN;
typedef int8_t              int8;
typedef u_int8_t            uInt8;
typedef uInt8               uChar;
typedef int16_t             int16;
typedef u_int16_t           uInt16;
typedef int32_t             int32;
typedef u_int32_t           uInt32;
typedef u_int32_t           ramData;


#ifndef CTRL_H
/* Function prototypes compatible with the ProSLIC API system services layer */
typedef int (*ctrl_Reset_fptr) (void *hCtrl, int in_reset);

typedef int (*ctrl_WriteRegister_fptr) (void *hCtrl, uInt8 channel, uInt8 regAddr, uInt8 data);
typedef int (*ctrl_WriteRAM_fptr) (void *hCtrl, uInt8 channel, uInt16 ramAddr, ramData data);

typedef uInt8 (*ctrl_ReadRegister_fptr) (void *hCtrl, uInt8 channel, uInt8 regAddr);
typedef ramData (*ctrl_ReadRAM_fptr) (void *hCtrl, uInt8 channel, uInt16 ramAddr);

typedef int (*ctrl_Semaphore_fptr) (void *hCtrl, int in_critical_section);
#endif

typedef void *(*get_hctrl) (uInt8 channel); /* Get a pointer to the control interface */

typedef int (*system_delay_fptr) (void *hTimer, int timeInMs);
typedef int (*system_timeElapsed_fptr) (void *hTimer, void *startTime, int *timeInMs);
typedef int (*system_getTime_fptr) (void *hTimer, void *time);
typedef void *(get_Timer)(void); /* return a pointer to "the" hTimer */

/* Function table entries to reduce namespace pollution */
typedef struct
{
  ctrl_Reset_fptr             reset;

  ctrl_WriteRegister_fptr     write_reg;
  ctrl_WriteRAM_fptr          write_ram;

  ctrl_ReadRegister_fptr      read_reg;
  ctrl_ReadRAM_fptr           read_ram;

  ctrl_Semaphore_fptr         sem;
} proslic_spi_fptrs_t;

typedef struct
{
  system_delay_fptr          slic_delay;
  system_timeElapsed_fptr    elapsed_time;
  system_getTime_fptr        get_time;
} proslic_timer_fptrs_t;

/* Timer container */
typedef struct 
{
  struct timespec64 timerObj;
} proslic_timeStamp;

extern int proslic_channel_count;
extern int proslic_debug_setting;
int proslic_spi_setup(void);
void proslic_spi_shutdown(void);

typedef enum
{
  PROSLIC_IS_UNKNOWN,
  PROSLIC_IS_PROSLIC,
  PROSLIC_IS_DAA
} proslic_dev_t;

/* Exported functions/values */
extern proslic_timer_fptrs_t proslic_timer_if;
extern proslic_spi_fptrs_t proslic_spi_if; 
int proslic_reset(void *hCtrl, int in_reset);
int proslic_write_register(void *hCtrl, uInt8 channel, uInt8 regAddr, uInt8 data);
int proslic_write_ram(void *hCtrl, uInt8 channel, uInt16 ramAddr, ramData data);
uInt8 proslic_read_register(void *hCtrl, uInt8 channel, uInt8 regAddr);
ramData proslic_read_ram(void *hCtrl, uInt8 channel, uInt16 ramAddr);

int proslic_get_channel_count(void);
proslic_dev_t proslic_get_device_type(uint8_t channel);
void *proslic_get_hCtrl(uint8_t channel);
int proslic_spi_probe(struct spi_device *spi, struct spi_driver *spi_drv);
int proslic_spi_remove(struct spi_device *spi);


#endif /* End of __PROSLIC_SYS_HDR__ */

