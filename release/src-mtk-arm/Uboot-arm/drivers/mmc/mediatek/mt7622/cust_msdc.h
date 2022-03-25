#ifndef CUST_MSDC_H
#define CUST_MSDC_H

#include "msdc_cfg.h"

#define MSDC_CD_PIN_EN      (1 << 0)  /* card detection pin is wired   */
#define MSDC_WP_PIN_EN      (1 << 1)  /* write protection pin is wired */
#define MSDC_RST_PIN_EN     (1 << 2)  /* emmc reset pin is wired       */
#define MSDC_SDIO_IRQ       (1 << 3)  /* use internal sdio irq (bus)   */
#define MSDC_EXT_SDIO_IRQ   (1 << 4)  /* use external sdio irq         */
#define MSDC_REMOVABLE      (1 << 5)  /* removable slot                */
#define MSDC_SYS_SUSPEND    (1 << 6)  /* suspended by system           */
#define MSDC_HIGHSPEED      (1 << 7)  /* high-speed mode support       */
#define MSDC_UHS1           (1 << 8)  /* uhs-1 mode support            */
#define MSDC_DDR            (1 << 9)  /* ddr mode support              */
#define MSDC_HS200          (1 << 10) /* hs200 mode support(eMMC4.5)   */
#define MSDC_HS400          (1 << 11) /* hs200 mode support(eMMC5.0)   */

#define MSDC_SMPL_RISING        (0)
#define MSDC_SMPL_FALLING       (1)
#define MSDC_SMPL_SEPERATE      (2)


struct msdc_cust {
    unsigned char  clk_src;           /* host clock source             */
    unsigned char  hclk_src;           /* host clock source             */
    unsigned char  cmd_edge;          /* command latch edge            */
    unsigned char  data_edge;         /* data latch edge               */
    unsigned char  clk_drv;           /* clock pad driving             */
    unsigned char  cmd_drv;           /* command pad driving           */
    unsigned char  dat_drv;           /* data pad driving              */
    unsigned char  rst_drv;           /* reset pin pad driving         */
    unsigned char  ds_drv;            /* ds pad driving                */
    unsigned char  clk_18v_drv;       /* clock pad driving             */
    unsigned char  cmd_18v_drv;       /* command pad driving on 1.8V   */
    unsigned char  dat_18v_drv;       /* data pad driving on 1.8V      */
    unsigned char  data_pins;         /* data pins                     */
    unsigned int   data_offset;       /* data address offset           */    
    unsigned int   flags;             /* hardware capability flags     */
};

extern struct msdc_cust msdc_cap[MSDC_MAX_NUM];

#endif /* end of _MSDC_CUST_H_ */

