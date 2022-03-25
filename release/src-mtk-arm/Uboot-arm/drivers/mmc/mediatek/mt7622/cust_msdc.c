#include "cust_msdc.h"
#include "msdc.h"


struct msdc_cust msdc_cap[MSDC_MAX_NUM] =
{
    {
        MSDC_CLKSRC_DEFAULT, /* host clock source          */
	0, /* host clock source 		 */
        MSDC_SMPL_RISING,   /* command latch edge            */
        MSDC_SMPL_RISING,   /* data latch edge               */
        MSDC_DRVN_DONT_CARE,/* clock pad driving             */
        MSDC_DRVN_DONT_CARE,/* command pad driving           */
        MSDC_DRVN_DONT_CARE,/* data pad driving              */
        MSDC_DRVN_GEAR1,/* rst pad driving               */
        MSDC_DRVN_DONT_CARE,/* ds pad driving                */
        MSDC_DRVN_GEAR0,    /* clock pad driving on 1.8V     */
        MSDC_DRVN_GEAR0,    /* command pad driving on 1.8V   */
        MSDC_DRVN_GEAR0,    /* data pad driving on 1.8V      */
        8,                  /* data pins                     */
        0,                  /* data address offset           */
#if defined(MMC_MSDC_DRV_CTP)
        MSDC_HIGHSPEED | MSDC_DDR  | MSDC_HS200 
#else
        MSDC_HIGHSPEED
#endif
    }

#if (MSDC_MAX_NUM > 1)
    ,
    {
        MSDC_CLKSRC_DEFAULT, /* host clock source          */
	0, /* host clock source 		 */
        MSDC_SMPL_RISING,   /* command latch edge            */
        MSDC_SMPL_RISING,   /* data latch edge               */
        MSDC_DRVN_GEAR2,    /* clock pad driving             */
        MSDC_DRVN_GEAR1,    /* command pad driving           */
        MSDC_DRVN_GEAR1,    /* data pad driving              */
        MSDC_DRVN_DONT_CARE,/* no need for not-emmc card     */
        MSDC_DRVN_DONT_CARE,/* no need for not-emmc card     */
        MSDC_DRVN_GEAR0,    /* clock pad driving on 1.8V     */
        MSDC_DRVN_GEAR0,    /* command pad driving on 1.8V   */
        MSDC_DRVN_GEAR0,    /* data pad driving on 1.8V      */
        4,                  /* data pins                     */
        0,                  /* data address offset           */

        /* hardware capability flags     */
#if defined(MMC_MSDC_DRV_CTP)
        MSDC_HIGHSPEED | MSDC_UHS1 | MSDC_DDR
#else
        MSDC_HIGHSPEED
#endif
    }
#endif

#if (MSDC_MAX_NUM > 2)
    ,
    {
        MSDC30_CLKSRC_DEFAULT, /* host clock source          */
		MSDC_DONOTCARE_HCLK, /* host clock source 		 */
        MSDC_SMPL_RISING,   /* command latch edge            */
        MSDC_SMPL_RISING,   /* data latch edge               */
        MSDC_DRVN_GEAR1,    /* clock pad driving             */
        MSDC_DRVN_GEAR1,    /* command pad driving           */
        MSDC_DRVN_GEAR1,    /* data pad driving              */
        MSDC_DRVN_DONT_CARE,/* no need for not-emmc card     */
        MSDC_DRVN_DONT_CARE,/* no need for not-emmc card     */
        MSDC_DRVN_GEAR1,    /* clock pad driving on 1.8V     */
        MSDC_DRVN_GEAR1,    /* command pad driving on 1.8V   */
        MSDC_DRVN_GEAR1 ,   /* data pad driving on 1.8V      */
        4,                  /* data pins                     */
        0,                  /* data address offset           */

        /* hardware capability flags     */
        MSDC_HIGHSPEED | MSDC_UHS1 | MSDC_DDR //|MSDC_UHS1|MSDC_SDIO_IRQ|MSDC_DDR
    }
#endif

#if (MSDC_MAX_NUM > 3)
    ,
    {
        MSDC50_CLKSRC_DEFAULT, /* host clock source          */
		MSDC50_CLKSRC4HCLK_182MHZ, /* host clock source 		 */
        MSDC_SMPL_RISING,   /* command latch edge            */
        MSDC_SMPL_RISING,   /* data latch edge               */
        MSDC_DRVN_DONT_CARE,/* clock pad driving             */
        MSDC_DRVN_DONT_CARE,/* command pad driving           */
        MSDC_DRVN_DONT_CARE,/* data pad driving              */
        MSDC_DRVN_GEAR1,    /* rst pad driving       */
        MSDC_DRVN_GEAR1,    /* ds pad driving      */
        MSDC_DRVN_GEAR1,    /* clock pad driving on 1.8V     */
        MSDC_DRVN_GEAR1,    /* command pad driving on 1.8V   */
        MSDC_DRVN_GEAR1,    /* data pad driving on 1.8V         */
        8,                  /* data pins                               */
        0,                  /* data address offset                */

        /* hardware capability flags     */
#if defined(MMC_MSDC_DRV_CTP)
		MSDC_HIGHSPEED | MSDC_DDR  | MSDC_HS200 | MSDC_HS400
#else
		MSDC_HIGHSPEED
#endif
    }
#endif

#if (MSDC_MAX_NUM > 4)
#error "*** Please try to add extra information!!!."
#endif
};



