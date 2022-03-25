#ifndef _MSDC_CFG_H_
#define _MSDC_CFG_H_

//#include "platform.h"

/*--------------------------------------------------------------------------*/
/* Common Definition                                                        */
/*--------------------------------------------------------------------------*/
#if CFG_FPGA_PLATFORM
#define FPGA_PLATFORM
//#define FPGA_SDCARD_TRIALRUN
#else
//#define MTK_MSDC_BRINGUP_DEBUG
#endif


#define MSDC_MAX_NUM            (2)

#define MSDC_USE_REG_OPS_DUMP   (0)
#define MSDC_USE_DMA_MODE       (0)


#if (1 == MSDC_USE_DMA_MODE)
//#define MSDC_DMA_BOUNDARY_LIMITAION
#define MSDC_DMA_DEFAULT_MODE            MSDC_MODE_DMA_BASIC
#endif

//#define FEATURE_MMC_UHS1
//#define FEATURE_MMC_BOOT_MODE
#define FEATURE_MMC_WR_TUNING
//#define FEATURE_MMC_CARD_DETECT
//#define FEATURE_MMC_STRIPPED
#define FEATURE_MMC_RD_TUNING
#define FEATURE_MMC_CM_TUNING
//#define FEATURE_MSDC_ENH_DMA_MODE
#define FEATURE_MMC_DATA_TUNING

/* Maybe we discard these macro definition */
//#define MMC_PROFILING
//#define FEATURE_MMC_SDIO
//#define FEATURE_MMC_BOOT_MODE
//#define MTK_EMMC_SUPPORT_OTP
//#define MMC_TEST
//#define MMC_BOOT_TEST
//#define MMC_ICE_DOWNLOAD
#define MSDC_USE_PATCH_BIT2_TURNING_WITH_ASYNC
//#define ACMD23_WITH_NEW_PATH
//#define FEATURE_MMC_CARD_DETECT

/*--------------------------------------------------------------------------*/
/* Debug Definition                                                         */
/*--------------------------------------------------------------------------*/
#define KEEP_SLIENT_BUILD
#define ___MSDC_DEBUG___
#endif /* end of _MSDC_CFG_H_ */

