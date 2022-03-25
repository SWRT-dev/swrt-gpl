/*
 * Copyright 2010-2018 MediaTek, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 as published by the Free Software Foundation.
 */

#ifndef __MTK_WDT_H__
#define __MTK_WDT_H__

#define MTK_WDT_BASE			RGU_BASE

#define MTK_WDT_MODE			(MTK_WDT_BASE+0x0000)
#define MTK_WDT_LENGTH			(MTK_WDT_BASE+0x0004)
#define MTK_WDT_RESTART			(MTK_WDT_BASE+0x0008)
#define MTK_WDT_STATUS			(MTK_WDT_BASE+0x000C)
#define MTK_WDT_INTERVAL		(MTK_WDT_BASE+0x0010)
#define MTK_WDT_SWRST			(MTK_WDT_BASE+0x0014)
#define MTK_WDT_SWSYSRST		(MTK_WDT_BASE+0x0018)
#define MTK_WDT_NONRST_REG		(MTK_WDT_BASE+0x0020)
#define MTK_WDT_NONRST_REG2		(MTK_WDT_BASE+0x0024)
#define MTK_WDT_REQ_MODE		(MTK_WDT_BASE+0x0030)
#define MTK_WDT_REQ_IRQ_EN		(MTK_WDT_BASE+0x0034)
#define MTK_WDT_DEBUG_CTL		(MTK_WDT_BASE+0x0040)
#define MTK_WDT_DEBUG_2_REG		(MTK_WDT_BASE+0x0508)



/*WDT_MODE*/
#define MTK_WDT_MODE_KEYMASK		(0xff00)
#define MTK_WDT_MODE_KEY		(0x22000000)

#define MTK_WDT_MODE_DDR_RESERVE  (0x0080)
#define MTK_WDT_MODE_DUAL_MODE  (0x0040)
#define MTK_WDT_MODE_IN_DIS		(0x0020) /* Reserved */
#define MTK_WDT_MODE_AUTO_RESTART	(0x0010) /* Reserved */
#define MTK_WDT_MODE_IRQ		(0x0008)
#define MTK_WDT_MODE_EXTEN		(0x0004)
#define MTK_WDT_MODE_EXT_POL		(0x0002)
#define MTK_WDT_MODE_ENABLE		(0x0001)


/*WDT_LENGTH*/
#define MTK_WDT_LENGTH_TIME_OUT		(0xffe0)
#define MTK_WDT_LENGTH_KEYMASK		(0x001f)
#define MTK_WDT_LENGTH_KEY		(0x0008)

/*WDT_RESTART*/
#define MTK_WDT_RESTART_KEY		(0x1971)

/*WDT_STATUS*/
#define MTK_WDT_STATUS_HWWDT_RST	(0x80000000)
#define MTK_WDT_STATUS_SWWDT_RST	(0x40000000)
#define MTK_WDT_STATUS_IRQWDT_RST	(0x20000000)
#define MTK_WDT_STATUS_DEBUGWDT_RST	(0x00080000)
#define MTK_WDT_STATUS_SPMWDT_RST	(0x0002)
#define MTK_WDT_STATUS_SPM_THERMAL_RST	(0x0001)
#define MTK_WDT_STATUS_THERMAL_DIRECT_RST	(1<<18)
#define MTK_WDT_STATUS_SECURITY_RST	(1<<28)




/*WDT_INTERVAL*/
#define MTK_WDT_INTERVAL_MASK		(0x0fff)

/*WDT_SWRST*/
#define MTK_WDT_SWRST_KEY		(0x1209)

/*WDT_SWSYSRST*/
#define MTK_WDT_SWSYS_RST_PWRAP_SPI_CTL_RST	(0x0800)
#define MTK_WDT_SWSYS_RST_APMIXED_RST	(0x0400)
#define MTK_WDT_SWSYS_RST_MD_LITE_RST	(0x0200)
#define MTK_WDT_SWSYS_RST_INFRA_AO_RST	(0x0100)
#define MTK_WDT_SWSYS_RST_MD_RST	(0x0080)
#define MTK_WDT_SWSYS_RST_DDRPHY_RST	(0x0040)
#define MTK_WDT_SWSYS_RST_IMG_RST	(0x0020)
#define MTK_WDT_SWSYS_RST_VDEC_RST	(0x0010)
#define MTK_WDT_SWSYS_RST_VENC_RST	(0x0008)
#define MTK_WDT_SWSYS_RST_MFG_RST	(0x0004)
#define MTK_WDT_SWSYS_RST_DISP_RST	(0x0002)
#define MTK_WDT_SWSYS_RST_INFRA_RST	(0x0001)


//#define MTK_WDT_SWSYS_RST_KEY		(0x1500)
#define MTK_WDT_SWSYS_RST_KEY		(0x88000000)
#define MTK_WDT_REQ_IRQ_KEY		(0x44000000)

/* WDT_NONRST_REG */
#define MTK_WDT_NONRST_DL               (0x00008000)

//MTK_WDT_DEBUG_CTL
#define MTK_DEBUG_CTL_KEY           (0x59000000)
#define MTK_RG_DDR_PROTECT_EN       (0x00001)
#define MTK_RG_MCU_LATH_EN          (0x00002)
#define MTK_RG_DRAMC_SREF           (0x00100)
#define MTK_RG_DRAMC_ISO            (0x00200)
#define MTK_RG_CONF_ISO             (0x00400)
#define MTK_DDR_RESERVE_RTA         (0x10000)  //sta
#define MTK_DDR_SREF_STA            (0x20000)  //sta


/* Reboot reason */
#define RE_BOOT_REASON_UNKNOW           (0x00)
#define RE_BOOT_BY_WDT_HW               (0x01)
#define RE_BOOT_BY_WDT_SW               (0x02)
#define RE_BOOT_WITH_INTTERUPT          (0x04)
#define RE_BOOT_BY_SPM_THERMAL          (0x08)
#define RE_BOOT_BY_SPM              	(0x10)
#define RE_BOOT_BY_THERMAL_DIRECT       (0x20)
#define RE_BOOT_BY_DEBUG        		(0x40)
#define RE_BOOT_BY_SECURITY        		(0x80)



#define RE_BOOT_ABNORMAL                (0xF0)

/* Reboot from which stage */
#define RE_BOOT_FROM_UNKNOW             (0x00)
#define RE_BOOT_FROM_PRE_LOADER         (0x01)
#define RE_BOOT_FROM_U_BOOT             (0x02)
#define RE_BOOT_FROM_KERNEL             (0x03)
#define RE_BOOT_FROM_POWER_ON           (0x04)

#define WDT_NORMAL_REBOOT		(0x01)
#define WDT_BY_PASS_PWK_REBOOT		(0x02)
#define WDT_NOT_WDT_REBOOT		(0x00)

typedef enum wd_swsys_reset_type {
	WD_MD_RST,
}WD_SYS_RST_TYPE;

extern void mtk_wdt_init(void);
//extern void mtk_wdt_reset(void);
//extern unsigned int mtk_wdt_check_status(void);
//extern unsigned int mtk_wdt_get_trig_reboot_stage(void);
extern bool mtk_is_rgu_trigger_reset(void);
extern void mtk_arch_reset(char mode);
extern int mtk_wdt_boot_check(void);
int rgu_dram_reserved(int enable);
int rgu_is_reserve_ddr_enabled(void);

int rgu_is_dram_slf(void);

void rgu_release_rg_dramc_conf_iso(void);

void rgu_release_rg_dramc_iso(void);

void rgu_release_rg_dramc_sref(void);
int rgu_is_reserve_ddr_mode_success(void);
void rgu_swsys_reset(WD_SYS_RST_TYPE reset_type);

//Iverson 20150522 : export the function to leopard_evb.c
extern void mtk_wdt_disable(void);

extern unsigned g_rgu_status;

#endif   /*__MTK_WDT_H__*/
