#ifndef BOOT_DEVICE_H
#define BOOT_DEVICE_H

#include "typedefs.h"

// ===================================================
// prototypes of device relevant callback function 
// ===================================================

typedef U32     (*CB_DEV_INIT)(void);
typedef U32     (*CB_DEV_READ)(u8 * buf, u32 offset);
typedef U32     (*CB_DEV_WRITE)(u8 * buf, u32 offset);
typedef bool    (*CB_DEV_ERASE)(u32 offset, u32 offset_limit, u32 size);
typedef void    (*CB_DEV_WAIT_READY)(void);
typedef U32     (*CB_DEV_FIND_SAFE_BLOCK)(u32 offset);
typedef U32     (*CB_DEV_CHKSUM_PER_FILE)(u32 offset, u32 img_size);
typedef U32     (*CB_DEV_CHKSUM)(u32 chksm, char *buf, u32 pktsz);


typedef struct {

// ===================================================
// provided by driver owner
// ===================================================
    CB_DEV_INIT                 dev_init;
    CB_DEV_READ                 dev_read_data;
    CB_DEV_WRITE                dev_write_data;    
    CB_DEV_ERASE                dev_erase_data;    
    CB_DEV_WAIT_READY           dev_wait_ready; // check if device is ready to use
    CB_DEV_FIND_SAFE_BLOCK      dev_find_safe_block; // correct R/W address
    CB_DEV_CHKSUM               dev_cal_chksum_body; 
    CB_DEV_CHKSUM_PER_FILE      dev_chksum_per_file;

} boot_dev_t;

#define BOOTDEV_NAND            (0)
#define BOOTDEV_SDMMC           (1)

#endif /* __BOOT_DEVICE_H__ */
