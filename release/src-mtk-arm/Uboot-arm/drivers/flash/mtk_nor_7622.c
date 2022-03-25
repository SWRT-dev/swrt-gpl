/*----------------------------------------------------------------------------*
 * No Warranty                                                                *
 * Except as may be otherwise agreed to in writing, no warranties of any      *
 * kind, whether express or implied, are given by MTK with respect to any MTK *
 * Deliverables or any use thereof, and MTK Deliverables are provided on an   *
 * "AS IS" basis.  MTK hereby expressly disclaims all such warranties,        *
 * including any implied warranties of merchantability, non-infringement and  *
 * fitness for a particular purpose and any warranties arising out of course  *
 * of performance, course of dealing or usage of trade.  Parties further      *
 * acknowledge that Company may, either presently and/or in the future,       *
 * instruct MTK to assist it in the development and the implementation, in    *
 * accordance with Company's designs, of certain softwares relating to        *
 * Company's product(s) (the "Services").  Except as may be otherwise agreed  *
 * to in writing, no warranties of any kind, whether express or implied, are  *
 * given by MTK with respect to the Services provided, and the Services are   *
 * provided on an "AS IS" basis.  Company further acknowledges that the       *
 * Services may contain errors, that testing is important and Company is      *
 * solely responsible for fully testing the Services and/or derivatives       *
 * thereof before they are used, sublicensed or distributed.  Should there be *
 * any third party action brought against MTK, arising out of or relating to  *
 * the Services, Company agree to fully indemnify and hold MTK harmless.      *
 * If the parties mutually agree to enter into or continue a business         *
 * relationship or other arrangement, the terms and conditions set forth      *
 * hereunder shall remain effective and, unless explicitly stated otherwise,  *
 * shall prevail in the event of a conflict in the terms in any agreements    *
 * entered into between the parties.                                          *
 *---------------------------------------------------------------------------*/
/******************************************************************************
* [File]			mtk_nor.c
* [Version]			v1.0
* [Revision Date]	2011-05-04
* [Author]			Shunli Wang, shunli.wang@mediatek.inc, 82896, 2012-04-27
* [Description]
*	SPI-NOR Driver Source File
* [Copyright]
*	Copyright (C) 2011 MediaTek Incorporation. All Rights Reserved.
******************************************************************************/

#define __UBOOT_NOR__ 1


#if defined(__UBOOT_NOR__)
#include <errno.h>
#include <linux/mtd/mtd.h>
#include <common.h>
#include <command.h>
#include <malloc.h>
#include "mtk_nor_7622.h" 
#include <asm/cache.h>
#endif // !defined(__UBOOT_NOR__)

int enable_protect_ro=0;
int protect_2nd_nor=0;
UINT8 u4MenuID=0;
// get ns time

struct mt53xx_nor
{
  //struct mutex lock;
  //struct mtd_info mtd;

  /* flash information struct obejct pointer
     */
  SFLASHHW_VENDOR_T *flash_info;

  /* flash number
     * total capacity
     */
  u32 total_sz;
  u8 flash_num;
  
  /* current accessed flash index
     * current command
     */
  u8 cur_flash_index;
  u8 cur_cmd_val;

  /* isr enable or not
     * dma read enable or not
     */
  u8 host_act_attr;

  /* current sector index
     */

  u32 cur_sector_index;
  u32 cur_addr_offset;
  
};

static struct mt53xx_nor *mt53xx_nor;

static SFLASHHW_VENDOR_T _aVendorFlash[] =
{
    { 0x01, 0x02, 0x12, 0x0,  0x80000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "Spansion(S25FL004A)" },
    { 0x01, 0x02, 0x13, 0x0, 0x100000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "Spansion(S25FL08A)" },
    { 0x01, 0x02, 0x14, 0x0, 0x200000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "Spansion(S25FL016A)" },
    { 0x01, 0x02, 0x15, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "Spansion(S25FL032A)" },
    { 0x01, 0x02, 0x16, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "Spansion(S25FL064A)" },
    { 0x01, 0x20, 0x18, 0x0, 0x1000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "Spansion(S25FL128P)" },
	{ 0x01, 0x40, 0x15, 0x0, 0x200000,	0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "Spansion(S25FL216K)" },
    { 0x01, 0x02, 0x19, 0x0, 0x1000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "Spansion(S25FL256S)" },

    { 0xC2, 0x20, 0x13, 0x0,  0x80000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L400)" },
    { 0xC2, 0x20, 0x14, 0x0, 0x100000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L800)" },
    { 0xC2, 0x20, 0x15, 0x0, 0x200000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x3B, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L160)" },
    { 0xC2, 0x24, 0x15, 0x0, 0x200000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x3B, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L1635D)" },
    { 0xC2, 0x20, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x3B, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L320)" },
    { 0xC2, 0x20, 0x17, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x3B, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L640)" },
    { 0xC2, 0x20, 0x18, 0x0, 0x1000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L128)" },
    { 0xC2, 0x5E, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L3235D)" },
    { 0xC2, 0x20, 0x19, 0x0, 0x2000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L256)" },
    { 0xC2, 0x20, 0x1a, 0x0, 0x4000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L512)" },
    { 0xC2, 0x20, 0x1b, 0x0, 0x8000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "MXIC(25L1G)" },

    { 0xC8, 0x40, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x3B, SFLASH_WRITE_PROTECTION_VAL, "GD(GD25Q32)" },
    { 0xC8, 0x40, 0x17, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x3B, SFLASH_WRITE_PROTECTION_VAL, "GD(GD25Q64)" },
    { 0xC8, 0x40, 0x18, 0x0, 0x1000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x3B, SFLASH_WRITE_PROTECTION_VAL, "GD(25L127)" },
    { 0xC8, 0x40, 0x19, 0x0, 0x2000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x3B, SFLASH_WRITE_PROTECTION_VAL, "GD(GD25Q256)" },
    { 0xC8, 0x40, 0x20, 0x0, 0x4000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x3B, SFLASH_WRITE_PROTECTION_VAL, "GD(GD25Q512)" },

    { 0x20, 0x20, 0x14, 0x0, 0x100000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ST(M25P80)" },
    { 0x20, 0x20, 0x15, 0x0, 0x200000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ST(M25P16)" },
    { 0x20, 0x20, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ST(M25P32)" },
    { 0x20, 0x20, 0x17, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ST(M25P64)" },
    { 0x20, 0x20, 0x18, 0x0, 0x1000000, 0x40000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ST(M25P128)" },
    { 0x20, 0x71, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ST(M25PX32)" },
    { 0x20, 0x71, 0x17, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ST(M25PX64)" },
	{ 0x20, 0xBA, 0x17, 0x0, 0x800000,	0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ST(N25Q064A13ESE40)" },
	{ 0x20, 0xBA, 0x19, 0x0, 0x2000000,	0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ST(N25Q256)" },

    { 0xEF, 0x30, 0x13, 0x0,  0x80000,   0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00,SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25X40)" },
    { 0xEF, 0x30, 0x14, 0x0, 0x100000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25X80)" },
    { 0xEF, 0x30, 0x15, 0x0, 0x200000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25X16)" },
    { 0xEF, 0x30, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25X32)" },
    { 0xEF, 0x30, 0x17, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25X64)" },

	{ 0xEF, 0x40, 0x15, 0x0, 0x200000,	0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25Q16FV)" },
	{ 0xEF, 0x40, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25Q32BV)" },
    { 0xEF, 0x40, 0x17, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25Q64FV)" },
    { 0xEF, 0x40, 0x18, 0x0, 0x1000000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00,SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25Q128BV)" },
    { 0xEF, 0x40, 0x19, 0x0, 0x2000000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00,SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25Q256)" },
    { 0xEF, 0x71, 0x19, 0x0, 0x4000000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00,SFLASH_WRITE_PROTECTION_VAL, "WINBOND(W25Q512JVEIQ)" },
#if 0 // sector size is not all 64KB, not support!!
    { 0x1C, 0x20, 0x15, 0x0, 0x200000,  0x1000,  60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25B16)" },
    { 0x1C, 0x20, 0x16, 0x0, 0x400000,  0x1000,  60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25B32)" },
    { 0x7F, 0x37, 0x20, 0x0, 0x200000,  0x1000,  60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "AMIC(A25L16P)"},
    { 0xBF, 0x25, 0x41, 0x0, 0x200000,  0x1000,  60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0xAD, 0xAD, 0x00, SFLASH_WRITE_PROTECTION_VAL, "SST(25VF016B)" },
    { 0x8C, 0x20, 0x15, 0x0, 0x200000,  0x1000,  60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0xAD, 0xAD, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ESMT(F25L016A)" },
#endif

#if (defined(CC_MT5360B) || defined(CC_MT5387) ||defined(CC_MT5363) ||defined(ENABLE_AAIWRITE))
    { 0xBF, 0x43, 0x10, 0x0,  0x40000,  0x8000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0x52, 0x60, 0x02, 0xAF, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF020)" },
    { 0xBF, 0x25, 0x8D, 0x0,  0x80000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0xAD, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF040B)" },
    { 0xBF, 0x25, 0x8E, 0x0, 0x100000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0xAD, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF080B)" },
    { 0xBF, 0x25, 0x41, 0x0, 0x200000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0xAD, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF016B)" },
    { 0xBF, 0x25, 0x4A, 0x0, 0x400000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0xAD, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF032B)" },
    { 0xBF, 0x25, 0x4B, 0x0, 0x800000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0xAD, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF064C)" },
#else
    { 0xBF, 0x43, 0x10, 0x1,  0x40000,  0x8000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0x52, 0x60, 0x02, 0xAF, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF020)" },
    { 0xBF, 0x25, 0x8D, 0x1,  0x80000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0xAD, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF040B)" },
    { 0xBF, 0x25, 0x8E, 0x1, 0x100000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0xAD, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF080B)" },
    { 0xBF, 0x25, 0x41, 0x1, 0x200000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0xAD, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF016B)" },
    { 0xBF, 0x25, 0x4A, 0x1, 0x400000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0xAD, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF032B)" },
    { 0xBF, 0x25, 0x4B, 0x1, 0x800000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0xAD, 0x00,  SFLASH_WRITE_PROTECTION_VAL,"SST(SST25VF064C)" },
#endif

    { 0x1F, 0x47, 0x00, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ATMEL(AT25DF321)" },
    { 0x1F, 0x48, 0x00, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "ATMEL(AT25DF641)" },

    { 0x1C, 0x20, 0x13, 0x0,  0x80000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25B40)" },
    { 0x1C, 0x31, 0x14, 0x0, 0x100000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25F80)" },
    { 0x1C, 0x20, 0x15, 0x0, 0x200000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25P16)" },
    { 0x1C, 0x70, 0x15, 0x0, 0x200000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25NEW)" },
    { 0x1C, 0x70, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25QH32)" },
    { 0x1C, 0x70, 0x17, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25QH64)" },
    { 0x1C, 0x70, 0x18, 0x0, 0x1000000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25QH128)" },
    { 0x1C, 0x20, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25P32)" },
    { 0x1C, 0x20, 0x17, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25P64)" },
    { 0x1C, 0x30, 0x17, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0xBB, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25Q64)" },
    { 0x1C, 0x30, 0x18, 0x0, 0x1000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "EON(EN25P128)" },

    { 0x7F, 0x37, 0x20, 0x0, 0x200000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "AMIC(A25L40P)" },
    { 0x37, 0x30, 0x13, 0x0, 0x100000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "AMIC(A25L040)" },
    { 0x37, 0x30, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "AMIC(A25L032)" },

    { 0xFF, 0xFF, 0x10, 0x0,  0x10000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xC7, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "512Kb" },
//    { 0xFF, 0xFF, 0x11, 0x0,  0x20000,  0x8000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "1Mb" },
    { 0xFF, 0xFF, 0x12, 0x0,  0x40000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "2Mb" },
    { 0xFF, 0xFF, 0x13, 0x0,  0x80000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "4Mb" },
    { 0xFF, 0xFF, 0x14, 0x0, 0x100000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "8Mb" },
    { 0xFF, 0xFF, 0x15, 0x0, 0x200000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "16Mb" },
    { 0xFF, 0xFF, 0x16, 0x0, 0x400000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "32Mb" },
    { 0xFF, 0xFF, 0x17, 0x0, 0x800000,  0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "64Mb" },
    { 0xFF, 0xFF, 0x18, 0x0, 0x1000000, 0x10000, 60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "128Mb" },

    { 0x00, 0x00, 0x00, 0x0, 0x000000,  0x00000, 0x000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, SFLASH_WRITE_PROTECTION_VAL, "NULL Device" },
};


static void mt53xx_nor_PinMux(void)
{

}

static u32 mt53xx_nor_Offset2Index(struct mt53xx_nor *mt53xx_nor, u32 offset)
{
  u8 index;
  u32 chipLimit = 0, chipStart = 0, chipSectorSz;

  if(offset >= mt53xx_nor->total_sz)
  {
    printf( "invalid offset exceeds max possible address!\b");
	return -EINVAL;
  }

  for(index = 0; index < mt53xx_nor->flash_num; index++)
  {
    chipStart = chipLimit;
    chipLimit += mt53xx_nor->flash_info[index].u4ChipSize;
	chipSectorSz = mt53xx_nor->flash_info[index].u4SecSize;
	
    if(offset < chipLimit)
    {
      mt53xx_nor->cur_flash_index = index;
	  mt53xx_nor->cur_addr_offset = offset - chipStart;
	  mt53xx_nor->cur_sector_index = mt53xx_nor->cur_addr_offset/(chipSectorSz - 1); 
	  break;
    }
  }

  return 0;
  
}

static u8 mt53xx_nor_Region2Info(struct mt53xx_nor *mt53xx_nor, u32 offset, u32 len)
{
  u8 info = 0, flash_cout = mt53xx_nor->flash_num;
  u32 first_chip_sz = mt53xx_nor->flash_info[0].u4ChipSize;

  if(len == mt53xx_nor->total_sz)
  {
    info |= REGION_RANK_TWO_WHOLE_FLASH;
    return info;
  }

  if(offset == 0)
  {
    info |= REGION_RANK_FIRST_FLASH;
	if(len >= first_chip_sz)
	{
      info |= REGION_RANK_FIRST_WHOLE_FLASH;
	  if((len > first_chip_sz) && (flash_cout > 1))
	  {
        info |= REGION_RANK_SECOND_FLASH;
	  }
	}
  }
  else if(offset < first_chip_sz)
  {
    info |= REGION_RANK_FIRST_FLASH;
	if(((offset + len) == mt53xx_nor->total_sz) && (flash_cout > 1))
	{
      info |= (REGION_RANK_SECOND_FLASH | REGION_RANK_SECOND_WHOLE_FLASH);
	}
  }
  else if((offset == first_chip_sz) && (flash_cout > 1))
  {
    info |= REGION_RANK_SECOND_FLASH;
    if(len == mt53xx_nor->total_sz - first_chip_sz)
    {
      info |= REGION_RANK_SECOND_WHOLE_FLASH;
    }
  }
  else if((offset > first_chip_sz) && (flash_cout > 1))
  {
    info |= REGION_RANK_SECOND_FLASH;
  }

  return info;
  
}

static void mt53xx_nor_SetDualRead(struct mt53xx_nor *mt53xx_nor)
{
  u8 attr = mt53xx_nor->host_act_attr;
  u8 dualread = mt53xx_nor->flash_info[0].u1DualREADCmd;
  u32 u4DualReg = SFLASH_RREG32(SFLASH_DUAL_REG);
  

  /* make sure the same dual read command about two sflash
     */
  if((attr & SFLASH_USE_DUAL_READ) &&
  	 (dualread != 0x00))
  {
    SFLASH_WREG8(SFLASH_PRGDATA3_REG, dualread);
    if (dualread == 0xbb)
	    u4DualReg |= 0x3;
    else
	    u4DualReg |= 0x1;
    SFLASH_WREG8(SFLASH_DUAL_REG, u4DualReg & 0xff);
  }
  else
  {
    u4DualReg &= ~0x3;
    SFLASH_WREG8(SFLASH_DUAL_REG, u4DualReg & 0xff);
  }
}

static void mt53xx_nor_DisableDualRead(struct mt53xx_nor *mt53xx_nor)
{
  u8 attr = mt53xx_nor->host_act_attr;
  u8 dualread = mt53xx_nor->flash_info[0].u1DualREADCmd;
  u32 u4DualReg = SFLASH_RREG32(SFLASH_DUAL_REG);
  
  if((attr & SFLASH_USE_DUAL_READ) &&
  	 (dualread != 0x00))
  {
	SFLASH_WREG8(SFLASH_PRGDATA3_REG, 0x0);
	u4DualReg &= ~0x3;
	SFLASH_WREG8(SFLASH_DUAL_REG, u4DualReg & 0xff);

  }
}

static void mt53xx_nor_SendCmd(struct mt53xx_nor *mt53xx_nor)
{
  u8 index = mt53xx_nor->cur_flash_index;
  u8 cmdval = mt53xx_nor->cur_cmd_val;
  
  if((index < 0) || (index > MAX_FLASHCOUNT - 1))
  {
    printf( "invalid flash index:%d!\n", index);
    return ;
  }
  SFLASH_WREG8(SFLASH_CMD_REG, (index * 0x40) + cmdval);
  
}

static u32 mt53xx_nor_PollingReg(u8 u1RegOffset, u8 u8Compare)
{
  u32 u4Polling;
  u8 u1Reg;

  u4Polling = 0;
  while(1)
  {
    u1Reg = SFLASH_RREG8(u1RegOffset);
    if (0x00 == (u1Reg & u8Compare))
    {
      break;
    }
    u4Polling++;
    if(u4Polling > SFLASH_POLLINGREG_COUNT)
    {
      printf("polling reg%02X using compare val%02X timeout!\n", 
	  	                                 u1RegOffset, u8Compare);
      return -1;
    }
  }
  
  return 0;
  
}

static u32 mt53xx_nor_ExecuteCmd(struct mt53xx_nor *mt53xx_nor)
{
  u8 val = (mt53xx_nor->cur_cmd_val) & 0x1F;

  mt53xx_nor_SendCmd(mt53xx_nor);

  return mt53xx_nor_PollingReg(SFLASH_CMD_REG, val);
  
}

static void mt53xx_nor_SetCfg1(u8 sf_num, u32 first_sf_cap)
{
    u8 u1Reg;

    u1Reg = SFLASH_RREG8(SFLASH_CFG1_REG);
    u1Reg &= (~0x1C);

	
	SFLASH_WREG8(SFLASH_CFG1_REG, u1Reg);

	return;
	
}

static u32 mt53xx_nor_GetFlashInfo(SFLASHHW_VENDOR_T *tmp_flash_info)
{
  u32 i = 0;
  SFLASHHW_VENDOR_T shadow_flash = {0xFF, 0xFF, 0xFF, 0x0, 0x1000000, 0x10000,
	  60000, 0x06, 0x04, 0x05, 0x01, 0x03, 0x0B, 0x9F, 0xD8, 0xC7, 0x02, 0x00,
	  0xBB, SFLASH_WRITE_PROTECTION_VAL, "shadow"};

  while(_aVendorFlash[i].u1MenuID != 0x00)
  {
    if((_aVendorFlash[i].u1MenuID == tmp_flash_info->u1MenuID) &&
       (_aVendorFlash[i].u1DevID1 == tmp_flash_info->u1DevID1) &&
       (_aVendorFlash[i].u1DevID2 == tmp_flash_info->u1DevID2))
    {	
      memcpy((VOID*)tmp_flash_info, (VOID*)&(_aVendorFlash[i]), sizeof(SFLASHHW_VENDOR_T));
      printf( "Setup flash information successful, support list index: %d\n", i);

      return 0;
    }
	
    i++;
  }

  memcpy((VOID*)tmp_flash_info, (VOID*)&shadow_flash, sizeof(SFLASHHW_VENDOR_T));
  printf("Read flash id fail, apply shadow flash parameters.\n");

  return 0;
}

static u32 mt53xx_nor_GetID(struct mt53xx_nor *mt53xx_nor)
{
  u32 u4Index, u4Ret;
  u8 cmdval;
  SFLASHHW_VENDOR_T *tmp_flash_info;

  mt53xx_nor->flash_num = 0;
  mt53xx_nor->total_sz = 0;
  
  for(u4Index = 0;u4Index < MAX_FLASHCOUNT;u4Index++)
  {
    /* we can not use this sentence firstly because of empty flash_info:
        * cmdval = mt53xx_nor->flash_info[u4Index].u1READIDCmd;
        * so it is necessary to consider read id command as constant.
        */
    cmdval = 0x9F;
    SFLASH_WREG8(SFLASH_PRGDATA5_REG, cmdval);
    SFLASH_WREG8(SFLASH_PRGDATA4_REG, 0x00);
    SFLASH_WREG8(SFLASH_PRGDATA3_REG, 0x00);
    SFLASH_WREG8(SFLASH_PRGDATA2_REG, 0x00);
    SFLASH_WREG8(SFLASH_CNT_REG, 32);

	
    /* pointer to the current flash info struct object
        * Save the current flash index
        */
    tmp_flash_info = mt53xx_nor->flash_info + u4Index;
	mt53xx_nor->cur_flash_index = u4Index;
	mt53xx_nor->cur_cmd_val = 0x04;

	if(0 != mt53xx_nor_ExecuteCmd(mt53xx_nor))
	{
      break;
	}

    tmp_flash_info->u1DevID2 = SFLASH_RREG8(SFLASH_SHREG0_REG);
    tmp_flash_info->u1DevID1 = SFLASH_RREG8(SFLASH_SHREG1_REG);	
    tmp_flash_info->u1MenuID = SFLASH_RREG8(SFLASH_SHREG2_REG);
	u4MenuID=tmp_flash_info->u1MenuID;

	mt53xx_nor->cur_cmd_val = 0x00;
    mt53xx_nor_SendCmd(mt53xx_nor);
    printf( "Flash Index: %d, MenuID: %02x, DevID1: %02x, DevID2: %02x\n", 
                                   u4Index, tmp_flash_info->u1MenuID,
                                            tmp_flash_info->u1DevID1,
                                            tmp_flash_info->u1DevID2);

    u4Ret = mt53xx_nor_GetFlashInfo(tmp_flash_info);
	if(0 == u4Ret)
	{
	    mt53xx_nor->flash_num++;
		mt53xx_nor->total_sz += tmp_flash_info->u4ChipSize;
		
	}
	else
	{
      break;
	}
	
  }

  if(0 == mt53xx_nor->flash_num)
  {
    printf( "any flash is not found!\n");
	return -1;
  }

  mt53xx_nor_SetCfg1(mt53xx_nor->flash_num, mt53xx_nor->flash_info[0].u4ChipSize); 

  return 0;
  
}

static u32 mt53xx_nor_ReadStatus(struct mt53xx_nor *mt53xx_nor, u8 *status)
{
  if(status == NULL)
  {
    return -1;
  }

  mt53xx_nor->cur_cmd_val = 0x02;
  if(-1 == mt53xx_nor_ExecuteCmd(mt53xx_nor))
  {
    printf( "read status failed!\n");
    return -1;
  }
	
  *status = SFLASH_RREG8(SFLASH_RDSR_REG);

  return 0;
  
}

static u32 mt53xx_nor_WriteStatus(struct mt53xx_nor *mt53xx_nor, u8 status)
{
  SFLASH_WREG8(SFLASH_PRGDATA5_REG, status);
  SFLASH_WREG8(SFLASH_CNT_REG,8);

  mt53xx_nor->cur_cmd_val = 0x20;
  if(-1 == mt53xx_nor_ExecuteCmd(mt53xx_nor))
  {
    printf( "write status failed!\n");
    return -1;
  }
	  
  return 0;

}

static u32 mt53xx_nor_WaitBusy(struct mt53xx_nor *mt53xx_nor, u32 timeout)
{
  u32 count;
  u8 reg;

  count = 0;
  while(1)
  {
    if(mt53xx_nor_ReadStatus(mt53xx_nor, &reg) != 0)
    {
      return -1;
    }

    if(0 == (reg & 0x1))
    {
      break;
    }

    count++;
    if(count > timeout)
    {
      printf( "wait write busy timeout, failed!\n");
      return -1;
    }

    udelay(5);
  }

  return 0;
  
}

static u32 mt53xx_nor_WaitBusySleep(struct mt53xx_nor *mt53xx_nor, u32 timeout)
{
  u32 count;
  u8 reg;

  count = 0;
  while(1)
  {
    if(mt53xx_nor_ReadStatus(mt53xx_nor, &reg) != 0)
    {
      return -1;
    }

    if(0 == (reg & 0x1))
    {
      break;
    }

    count++;
    if(count > timeout)
    {
      printf( "wait write busy timeout, failed!\n");
      return -1;
    }

    //msleep(5);
     mdelay(5);
  }

  return 0;
  
}

static u32 mt53xx_nor_WriteEnable(struct mt53xx_nor *mt53xx_nor)
{
  u8 cur_flash_index, val;

  cur_flash_index = mt53xx_nor->cur_flash_index;
  val = mt53xx_nor->flash_info[cur_flash_index].u1WRENCmd;

  SFLASH_WREG8(SFLASH_PRGDATA5_REG, val);
  SFLASH_WREG8(SFLASH_CNT_REG,8);

  mt53xx_nor->cur_cmd_val = 0x04;
  if(-1 == mt53xx_nor_ExecuteCmd(mt53xx_nor))
  {
    printf( "write enable failed!\n");
    return -1;
  }
	
  return 0;

}

static u32 mt53xx_nor_WriteBufferEnable(struct mt53xx_nor *mt53xx_nor)
{
  u32 polling;
  u8 reg;
  
  SFLASH_WREG8(SFLASH_CFG2_REG, 0x0D);

  polling = 0;
  while(1)
  {
    reg = SFLASH_RREG8(SFLASH_CFG2_REG);
    if (0x01 == (reg & 0x01))
    {
      break;
    }

    polling++;
    if(polling > SFLASH_POLLINGREG_COUNT)
    {
      return -1;
    }
  }

  return 0;
}


static u32 mt53xx_nor_WriteBufferDisable(struct mt53xx_nor *mt53xx_nor)
{
  u32 polling;
  u8 reg;

  SFLASH_WREG8(SFLASH_CFG2_REG, 0xC);
  
  polling = 0;
  while(1)    // Wait for finish write buffer
  {
    reg = SFLASH_RREG8(SFLASH_CFG2_REG);
    if (0x0C == (reg & 0xF))
    {
      break;
    }

    polling++;
    if(polling > SFLASH_POLLINGREG_COUNT)
    {
      return -1;
    }
  }

  return 0;
  
}


static int mt53xx_nor_WriteProtect(struct mt53xx_nor *mt53xx_nor, u32 fgEn)
{  
  u8 val = fgEn?0x3C:0x00;
  
  if(0 != mt53xx_nor_WaitBusy(mt53xx_nor, SFLASH_WRITEBUSY_TIMEOUT))
  {
    printf( "wait write busy #1 failed in write protect process!\n");
    return -1;
  }

  if(0 != mt53xx_nor_WriteEnable(mt53xx_nor))
  {
    printf( "write enable failed in write protect process!\n");
    return -1;
  }

  if (0 != mt53xx_nor_WriteStatus(mt53xx_nor, val))
  {
    printf( "write status failed in write protect process!\n");
    return -1;
  }

  if(mt53xx_nor_WaitBusy(mt53xx_nor, SFLASH_WRITEBUSY_TIMEOUT) != 0)
  {
    printf( "wait write busy #2 failed in write protect process!\n");
    return -1;
  }

  return 0;
  
}

static u32 mt53xx_nor_WriteProtectAllChips(struct mt53xx_nor *mt53xx_nor, u32 fgEn)
{ 
  u8 index, cur_index = mt53xx_nor->cur_flash_index;

  for(index = 0; index < mt53xx_nor->flash_num; index++)
  {
    mt53xx_nor->cur_flash_index = index;
    if(0 != mt53xx_nor_WriteProtect(mt53xx_nor, fgEn))
    {
      printf( "Flash #%d: write protect disable failed!\n", index);
	  mt53xx_nor->cur_flash_index = cur_index;
	  return -1;
    }
	else
		printf("Flash #%d: write protect disable successfully!\n", index);
  }

  mt53xx_nor->cur_flash_index = cur_index;
  
  return 0;

}

static u32 mt53xx_nor_EraseChip(struct mt53xx_nor *mt53xx_nor)
{
  u8 cur_flash_index, val;
  
  cur_flash_index = mt53xx_nor->cur_flash_index;
  val = mt53xx_nor->flash_info[cur_flash_index].u1CHIPERASECmd;
  printf( "%s \n",__FUNCTION__);

  if(mt53xx_nor_WaitBusySleep(mt53xx_nor, SFLASH_WRITEBUSY_TIMEOUT) != 0)
  {
    printf( "wait write busy #1 failed in erase chip process!\n");
    return -1;
  }
	
  if(mt53xx_nor_WriteEnable(mt53xx_nor) != 0)
  {
    printf( "write enable failed in erase chip process!\n");
    return -1;
  }
	
  /* Execute sector erase command
     */
  SFLASH_WREG8(SFLASH_PRGDATA5_REG, val);
  SFLASH_WREG8(SFLASH_CNT_REG, 8);

  mt53xx_nor->cur_cmd_val = 0x04;
  if(-1 == mt53xx_nor_ExecuteCmd(mt53xx_nor))
  {
    printf( "erase chip#%d failed!\n", cur_flash_index);
    return -1;
  }

  if(mt53xx_nor_WaitBusySleep(mt53xx_nor, SFLASH_CHIPERASE_TIMEOUT) != 0)
  {
    printf( "wait write busy #2 failed in erase chip process!\n");
    return -1;
  }

  mt53xx_nor->cur_cmd_val = 0x00;
  mt53xx_nor_SendCmd(mt53xx_nor);
	
  return 0;

}


static u32 mt53xx_nor_EraseSector(struct mt53xx_nor *mt53xx_nor, u32 offset)
{
  u32 addr;
  u8 cmdval, index;

  if(0 != mt53xx_nor_WaitBusySleep(mt53xx_nor, SFLASH_WRITEBUSY_TIMEOUT))
  {
    printf( "wait write busy #1 failed in erase sector process!\n");
    return -1;
  }

  if(0 != mt53xx_nor_WriteEnable(mt53xx_nor))
  {
    printf( "write enable failed in erase sector process!\n");
    return -1;
  }

  if(0 != mt53xx_nor_Offset2Index(mt53xx_nor, offset))
  {
    printf( "offset parse failed in erase sector process!\n");
	return -1;
  }

  /* Execute sector erase command
     */
  index = mt53xx_nor->cur_flash_index;
  addr = mt53xx_nor->cur_addr_offset;
  cmdval = mt53xx_nor->flash_info[index].u1SECERASECmd;
  SFLASH_WREG8(SFLASH_PRGDATA5_REG, cmdval);
#ifdef BD_NOR_ENABLE
  if (mt53xx_nor->flash_info[index].u4ChipSize >= 0x2000000)
  {
  	SFLASH_WREG8(SFLASH_PRGDATA4_REG, HiByte(HiWord(addr))); // Write
  	SFLASH_WREG8(SFLASH_PRGDATA3_REG, LoByte(HiWord(addr))); // Write
  	SFLASH_WREG8(SFLASH_PRGDATA2_REG, HiByte(LoWord(addr))); // Write
  	SFLASH_WREG8(SFLASH_PRGDATA1_REG, LoByte(LoWord(addr))); // Write
  	SFLASH_WREG8(SFLASH_CNT_REG, 40);       // Write SF Bit Count
  }
  else
  {
  	SFLASH_WREG8(SFLASH_PRGDATA4_REG, LoByte(HiWord(addr)));
  	SFLASH_WREG8(SFLASH_PRGDATA3_REG, HiByte(LoWord(addr)));
  	SFLASH_WREG8(SFLASH_PRGDATA2_REG, LoByte(LoWord(addr)));
  	SFLASH_WREG8(SFLASH_CNT_REG, 32);
  }
#else
  SFLASH_WREG8(SFLASH_PRGDATA4_REG, LoByte(HiWord(addr)));
  SFLASH_WREG8(SFLASH_PRGDATA3_REG, HiByte(LoWord(addr)));
  SFLASH_WREG8(SFLASH_PRGDATA2_REG, LoByte(LoWord(addr)));
  SFLASH_WREG8(SFLASH_CNT_REG, 32);
#endif
  mt53xx_nor->cur_cmd_val = 0x04;
  if(0 != mt53xx_nor_ExecuteCmd(mt53xx_nor))
  {
    printf( "erase sector(offset:%08x flash#%d, sector index:%d, offset:%08x) failed!\n", 
		                                                      offset, index, 
		                                                      mt53xx_nor->cur_sector_index, 
		                                                      mt53xx_nor->cur_addr_offset);
    return -1;
  }

  if(0 != mt53xx_nor_WaitBusySleep(mt53xx_nor, SFLASH_ERASESECTOR_TIMEOUT))
  {
    printf( "wait write busy #2 failed in erase sector process!\n");
    return -1;
  }

  mt53xx_nor->cur_cmd_val = 0x00;
  mt53xx_nor_SendCmd(mt53xx_nor);
	
  return 0;

}

u32 mtk_nor_erase(u32 addr, u32 len)
{
  u32 first_chip_sz, total_sz, rem, ret=0;
  u8 info;
  first_chip_sz = mt53xx_nor->flash_info[0].u4ChipSize;
  total_sz = mt53xx_nor->total_sz;

//#if NOR_DEBUG
  printf( "%s addr=0x%x,len=0x%x\n",__FUNCTION__,addr,len);
//#endif

  //if the address of nor < 0x30000, the system will be assert
  if((addr < 0x30000)&&(enable_protect_ro==1))
  {
	 printf("Error: nor erase address at 0x%x < 0x30000",addr);
	  //while(1) {};
	 BUG_ON(1);
	 return -EINVAL;
  }

  if((addr + len) > mt53xx_nor->total_sz)
  {
  	printf( "Error: nor erase address at 0x%x ",addr + len);
  	return -EINVAL;
  }
  
  rem = len - (len & (~(mt53xx_nor->flash_info[0].u4SecSize - 1))); // Roger modify
  if(rem)
  {
  	return -EINVAL;
  }

  //mutex_lock(&mt53xx_nor->lock);

  /*
     * pimux switch firstly 
     */
  mt53xx_nor_PinMux(); 

  
  /* get rank-flash info of region indicated by addr and len
     */
  info = mt53xx_nor_Region2Info(mt53xx_nor, addr, len);

  /*
     * disable write protect of all related chips 
     */
  if(info & REGION_RANK_FIRST_FLASH)
  {
    mt53xx_nor->cur_flash_index = 0;
	if(0 != mt53xx_nor_WriteProtect(mt53xx_nor, 0))
    {
      printf( "disable write protect of flash#0 failed!\n");
	  ret = -1;
	  goto done;
    }
  }
  if(info & REGION_RANK_SECOND_FLASH)
  {
    mt53xx_nor->cur_flash_index = 1;
	if(0 != mt53xx_nor_WriteProtect(mt53xx_nor, 0))
    {
      printf( "disable write protect of flash#1 failed!\n");
	  ret = -1;
	  goto done;
    }
  }  
 
  //printf( "erase addr: %08x~%08x\n", addr, len);
  
  /* erase the first whole flash if necessary
     */
  if(info & REGION_RANK_FIRST_WHOLE_FLASH)
  {
    mt53xx_nor->cur_flash_index = 0;
    if(mt53xx_nor_EraseChip(mt53xx_nor))
    {
      printf( "erase whole flash#0 failed!\n");
	  ret = -EIO; 
	  goto done;
    }
	else
	printf( "erase whole flash#0 successful!\n");

    addr += first_chip_sz;
	len -= first_chip_sz;
  }

  /* erase involved region of the first flash if necessary
     */
  mt53xx_nor->cur_flash_index = 0;
  while((len)&&(addr < first_chip_sz))
  {
    if(mt53xx_nor_EraseSector(mt53xx_nor, addr))
    {
      printf( "erase part of flash#0 failed!\n");
      ret = -EIO; 
	  goto done;
    }
	else
		printf( "erase part flash#0 successful!\n");

    addr += mt53xx_nor->flash_info[0].u4SecSize;
    len -= mt53xx_nor->flash_info[0].u4SecSize;
  }

  /* erase the first whole flash if necessary
     */
  if(info & REGION_RANK_SECOND_WHOLE_FLASH)
  {
    mt53xx_nor->cur_flash_index = 1;
    if(mt53xx_nor_EraseChip(mt53xx_nor))
    {
      printf( "erase whole flash#1 failed!\n");
	  ret = -EIO; 
	  goto done;  
    }
	else
		printf( "erase whole flash#1 successful!\n");

    addr += first_chip_sz;
	len -= first_chip_sz;
  }

  /* erase involved region of the first flash if necessary
     */
  mt53xx_nor->cur_flash_index = 1;
  while((len)&&(addr < total_sz))
  {
    if(mt53xx_nor_EraseSector(mt53xx_nor, addr))
    {
      printf( "erase part of flash#1 failed!\n");
      ret = -EIO; 
	  goto done;
    }
	else
	printf( "erase part flash#1 successful!\n");

    addr += mt53xx_nor->flash_info[0].u4SecSize;
    len -= mt53xx_nor->flash_info[0].u4SecSize;
  }

done:
	
  //mutex_unlock(&mt53xx_nor->lock);

  return ret;
} 

static int sf_do_dma(u32 src, u32 dest, u32 len)
{
	/* reset */
	SFLASH_WREG32(SF_FDMA_CTL, 0);
	SFLASH_WREG32(SF_FDMA_CTL, 0x2);
	SFLASH_WREG32(SF_FDMA_CTL, 0x4);

	/* flash source address */
	SFLASH_WREG32(SF_FDMA_FADR, src);

	/* DRAM destination address */
	SFLASH_WREG32(SF_FDMA_DADR, dest);
	SFLASH_WREG32(SF_FDMA_END_DADR, dest + len);

	/* Enable */
	SFLASH_WREG32(SF_FDMA_CTL, 0x5);

	while ((SFLASH_RREG32(SF_FDMA_CTL) & 0x1) != 0);

	return len;
}

#define MEMCPY_THRESHOLD 0x200
#define BUF_LOCAL_LEN    0x1000
#define DMA_ADDR_ALIGN   0x40
static u8 nor_dma_buf[BUF_LOCAL_LEN]  __attribute__((aligned(DMA_ADDR_ALIGN)));
static u32 mt53xx_nor_Read_mixed(struct mt53xx_nor *mt53xx_nor, u32 addr, u32 len, u32 *retlen, u8 *buf)
{
	u32 i, data_header;
	u32 buf_cnt, cur_addr, cur_index;
	u32 read_cnt = 0;

	if (len == 0) {
		printf("read data len 0.\n");
		return 0;
	}

	if (!(((addr % DMA_ADDR_ALIGN) || (len % DMA_ADDR_ALIGN) || ((u32)buf % DMA_ADDR_ALIGN)))) {
		invalidate_dcache_range((unsigned long)buf, (unsigned long)buf + len);
		sf_do_dma(addr, (u32)buf, len);
		invalidate_dcache_range((unsigned long)buf, (unsigned long)buf + len);
		goto done;
	}

	if (len < MEMCPY_THRESHOLD) {
		memcpy(buf,(u8 *)(SFLASH_MEM_BASE+addr), len);
		goto done;
	}

	data_header = addr % DMA_ADDR_ALIGN;
	buf_cnt = (data_header + len + BUF_LOCAL_LEN - 1) / BUF_LOCAL_LEN;
	cur_addr = addr - data_header;
	cur_index = 0;

	if (buf_cnt == 1) {
		invalidate_dcache_range((unsigned long)nor_dma_buf, (unsigned long)nor_dma_buf + BUF_LOCAL_LEN);
		sf_do_dma(cur_addr, (u32)nor_dma_buf, BUF_LOCAL_LEN);
		invalidate_dcache_range((unsigned long)nor_dma_buf, (unsigned long)nor_dma_buf + BUF_LOCAL_LEN);
		memcpy(buf, nor_dma_buf + data_header, len);
		goto done;
	}

	invalidate_dcache_range((unsigned long)nor_dma_buf, (unsigned long)nor_dma_buf + BUF_LOCAL_LEN);
	sf_do_dma(cur_addr, (u32)nor_dma_buf, BUF_LOCAL_LEN);
	invalidate_dcache_range((unsigned long)nor_dma_buf, (unsigned long)nor_dma_buf + BUF_LOCAL_LEN);
	memcpy(buf, nor_dma_buf + data_header, BUF_LOCAL_LEN - data_header);
	read_cnt += BUF_LOCAL_LEN - data_header;
	cur_index = BUF_LOCAL_LEN - data_header;
	cur_addr += BUF_LOCAL_LEN;

	for(i = 0; i < buf_cnt - 2; i++) {
		invalidate_dcache_range((unsigned long)nor_dma_buf, (unsigned long)nor_dma_buf + BUF_LOCAL_LEN);
		sf_do_dma(cur_addr, (u32)nor_dma_buf, BUF_LOCAL_LEN);
		invalidate_dcache_range((unsigned long)nor_dma_buf, (unsigned long)nor_dma_buf + BUF_LOCAL_LEN);
		memcpy(buf + cur_index, nor_dma_buf, BUF_LOCAL_LEN);
		read_cnt += BUF_LOCAL_LEN;
		cur_index += BUF_LOCAL_LEN;
		cur_addr += BUF_LOCAL_LEN;
	}

	if (len > read_cnt) {
		invalidate_dcache_range((unsigned long)nor_dma_buf, (unsigned long)nor_dma_buf + BUF_LOCAL_LEN);
		sf_do_dma(cur_addr, (u32)nor_dma_buf, BUF_LOCAL_LEN);
		invalidate_dcache_range((unsigned long)nor_dma_buf, (unsigned long)nor_dma_buf + BUF_LOCAL_LEN);
		memcpy(buf + cur_index, nor_dma_buf, (len - read_cnt));
	}

done:
	(*retlen) += len;
	return 0;
}

static u32 mt53xx_nor_Read(struct mt53xx_nor *mt53xx_nor, u32 addr, u32 len, u32 *retlen, u8 *buf)
{
  u32 ret = 0, len_align = 0;
  u8 info;

  mt53xx_nor_PinMux();

  mt53xx_nor_SetDualRead(mt53xx_nor);
	
  /* get rank-flash info of region indicated by addr and len
     */
  info = mt53xx_nor_Region2Info(mt53xx_nor, addr, len);
	
  /* Wait till previous write/erase is done. 
     */
  if(info & REGION_RANK_FIRST_FLASH)
  {
    mt53xx_nor->cur_flash_index = 0;
    if(0 != mt53xx_nor_WaitBusy(mt53xx_nor, SFLASH_WRITEBUSY_TIMEOUT))
    {
      printf( "Read Wait Busy of flash#0 failed!\n");
      ret = -1;
      goto done;
    }
  }
  if(info & REGION_RANK_SECOND_FLASH)
  {
    mt53xx_nor->cur_flash_index = 1;
    if(0 != mt53xx_nor_WaitBusy(mt53xx_nor, SFLASH_WRITEBUSY_TIMEOUT))
    {
      printf( " Read Wait Busy  of flash#1 failed!\n");
      ret = -1;
      goto done;
    }
  }  

  /* read operation is going on
     */
  mt53xx_nor->cur_flash_index = 0;
	
  /* Disable pretch write buffer  
     */
  if(0 != mt53xx_nor_WriteBufferDisable(mt53xx_nor))
  {
    printf( "disable write buffer in read process failed!\n");
    ret = -1;
    goto done;    
  }

  if(len > 0)
  {
    len_align = len;
	 
    if(0 != mt53xx_nor_Read_mixed(mt53xx_nor, addr, len_align, retlen, buf))
    {
      printf( "read in pio mode #2 failed!\n");
      ret = -1;
      goto done;	  
    }   
	  
    addr += len_align;
    len  -= len_align;
    buf += len_align;
  } 

  mt53xx_nor_DisableDualRead(mt53xx_nor);
done:

  return ret;

}

/*[20130314]Copy nor flash data to buffer for checking data write error or memory overwrite.
It affects the nor flash performance so remove it in release version.*/
/*
typedef struct __NorReadRecord
{
    u32 nor_magic;
    u32 nor_from;
    u32 nor_len;
    u32 nor_retlen;
    u32 nor_f00085a0;
    u32 nor_f00085a4;
    u8 *nor_buf;
    u32 nor_magic2;
    u8  nor_data[1024*8];
} NorReadRecord_T;

#define NOR_READ_ARRAY_SIZE         32
static NorReadRecord_T arNorReadArray[NOR_READ_ARRAY_SIZE] __attribute__ ((aligned (16)));
static u32 nor_index = 0;
static u32 nor_init = 0;
*/
#ifdef MULTI_READ_DMA
static u8 _pu1NorTemp1Buf[SFLASH_MAX_DMA_SIZE+64]  __attribute__((aligned(0x20)));
#endif

u32 mtk_nor_read(u32 from, u32 len, size_t *retlen, u_char *buf)
{
  u32 ret = 0;
   u8 *readBuf;
   u32 readLen;
   //struct timespec tv,tv1;
   //u32 readtime = 0;

  if (!len)
  {
    return 0;
  }

  //if (from + len > mt53xx_nor->mtd.size)
  if (from + len > mt53xx_nor->total_sz)
  {
    return -EINVAL;
  }

  /* Byte count starts at zero. */
  *retlen = 0;


  //mutex_lock(&mt53xx_nor->lock);
  
 //make sure the 0xFF had been write to dram
//MultiRead:
 /*  memset(buf,0xAA,len);
 if(virt_addr_valid((u32)buf ))
 	{
	   phyaddr = dma_map_single(NULL, buf, len, DMA_TO_DEVICE);
	   dma_unmap_single(NULL, phyaddr, len, DMA_TO_DEVICE);
 	}

*/

	readBuf=buf;
    readLen=len;

  if(0 != mt53xx_nor_Read(mt53xx_nor, (u32)from, (u32)readLen, (u32 *)retlen, (u8 *)readBuf))
  {
    printf( "read failed(addr = %08x, len = %08x, retlen = %08x)\n", (u32)from,
		                                                                     (u32)readLen,
		                                                                     (u32)(*retlen)); 
	ret = -1;
  }
  else
  {

  }

//done:
//	getnstimeofday(&tv1);
//	readtime = (1000000000 * tv1.tv_sec + tv1.tv_nsec) - (1000000000 * tv.tv_sec + tv.tv_nsec);

  //mutex_unlock(&mt53xx_nor->lock);
//  printf( "%s (addr = %08x, len = %08x, retlen = %08x buf = %08x  readtime=%08x speed=%08x %08x)\n", __FUNCTION__,(u32)from,(u32)len,(u32)(*retlen),buf,readtime, tv1.tv_nsec, tv.tv_nsec);

  return ret;

}
//-----------------------------------------------------------------------------
/*
*Enter 4 bytes address mode
*return 0 success, return others fial
*/


static u32 mt53xx_nor_WriteSingleByte(struct mt53xx_nor *mt53xx_nor, u32 u4addr, u8 u1data)
{
  u32 addr;
  u8 index;

  if(u4addr >= mt53xx_nor->total_sz)
  {
    printf( "invalid write address exceeds MAX possible address!\n");
	return -1;
  }

#if 0
  if(0 != mt53xx_nor_ReadSingleByte(mt53xx_nor, u4addr, &data))
  {
    printf( "read byte before write failed!\n");
  }
  if(data != 0xFF)
  {
    printf( "read byte before write value: %02x(addr%08x, target:%02x)\n", data, u4addr, u1data);
	while(1);
  }
#endif

  if(0 != mt53xx_nor_Offset2Index(mt53xx_nor, u4addr))
  {
    printf( "offset parse failed in write byte process!\n");
	return -1;
  }

  index = mt53xx_nor->cur_flash_index;
  addr = mt53xx_nor->cur_addr_offset;

  SFLASH_WREG8(SFLASH_WDATA_REG, u1data);
  SFLASH_WREG8(SFLASH_RADR3_REG, HiByte(HiWord(addr)));
  SFLASH_WREG8(SFLASH_RADR2_REG, LoByte(HiWord(addr)));
  SFLASH_WREG8(SFLASH_RADR1_REG, HiByte(LoWord(addr)));
  SFLASH_WREG8(SFLASH_RADR0_REG, LoByte(LoWord(addr)));

  mt53xx_nor->cur_cmd_val = 0x10;
  if(0 != mt53xx_nor_ExecuteCmd(mt53xx_nor))
  {
    printf( "write byte(offset:%08x flash#%d, sector index:%d, offset:%08x) failed!\n", 
		                                                      u4addr, index, 
		                                                      mt53xx_nor->cur_sector_index, 
		                                                      mt53xx_nor->cur_addr_offset);
    return -1;
  }

  mt53xx_nor->cur_cmd_val = 0x00;
  mt53xx_nor_SendCmd(mt53xx_nor);

  if(0 != mt53xx_nor_WaitBusy(mt53xx_nor, SFLASH_WRITEBUSY_TIMEOUT))
  {
    printf( "wait write busy failed in write byte process!\n");
    return -1;
  }

#if 0
	if(0 != mt53xx_nor_ReadSingleByte(mt53xx_nor, u4addr, &data))
	{
	  printf( "read byte after write failed!\n");
	}
	if(data != u1data)
	{
	  printf( "read byte after write value: %02x(addr%08x, target:%02x)\n", data, u4addr, u1data);
	  while(1);
	}
#endif


  return 0;
  
}

static u32 mt53xx_nor_WriteBuffer(struct mt53xx_nor *mt53xx_nor, 
	                                      u32 u4addr, u32 u4len, const u8 *buf)
{
  u32 i, j, bufidx, data;
  u8 index;

  if(buf == NULL)
  {
    return -1;
  }

  if(0 != mt53xx_nor_Offset2Index(mt53xx_nor, u4addr))
  {
    printf( "offset parse failed in write buffer process!\n");
	return -1;
  }

  index = mt53xx_nor->cur_flash_index;
  SFLASH_WREG8(SFLASH_RADR3_REG, HiByte(HiWord(u4addr))); // Write
  SFLASH_WREG8(SFLASH_RADR2_REG, LoByte(HiWord(u4addr))); // Write
  SFLASH_WREG8(SFLASH_RADR1_REG, HiByte(LoWord(u4addr))); // Write
  SFLASH_WREG8(SFLASH_RADR0_REG, LoByte(LoWord(u4addr))); // Write

  bufidx = 0;
  for(i=0; i<u4len; i+=4)
  {
    for(j=0; j<4; j++)
    {
      (*((u8 *)&data + j)) = buf[bufidx];
      bufidx++;
    }
    SFLASH_WREG32(SFLASH_PP_DATA_REG, data);
  }

  mt53xx_nor->cur_cmd_val = 0x10;
  if(0 != mt53xx_nor_ExecuteCmd(mt53xx_nor))
  {
    printf( "write buffer(offset:%08x flash#%d, sector index:%d, offset:%08x) failed!\n", 
		                                                      u4addr, index, 
		                                                      mt53xx_nor->cur_sector_index, 
		                                                      mt53xx_nor->cur_addr_offset);
    return -1;
  }

  mt53xx_nor->cur_cmd_val = 0x00;
  mt53xx_nor_SendCmd(mt53xx_nor);

  if(0 != mt53xx_nor_WaitBusy(mt53xx_nor, SFLASH_WRITEBUSY_TIMEOUT))
  {
    printf( "wait write buffer failed in write buffer process!\n");
    return -1;
  }

  return 0;
  
}

static u32 mt53xx_nor_Write(struct mt53xx_nor *mt53xx_nor, u32 addr, u32 len, u32 *retlen, const u8 *buf)
{
  u32 i, ret = 0;
  u8 info;
  
#ifdef BD_NOR_ENABLE
  u32 count, pgalign = 0;
#endif

  mt53xx_nor_PinMux();
	  
  /* get rank-flash info of region indicated by addr and len
     */

  info = mt53xx_nor_Region2Info(mt53xx_nor, addr, len);
	
  /*
     * disable write protect of all related chips 
     * make sure all of related flash status is idle
     * make sure flash can be write
     */
  if(info & REGION_RANK_FIRST_FLASH)
  {
    mt53xx_nor->cur_flash_index = 0;
	
    if(0 != mt53xx_nor_WriteProtect(mt53xx_nor, 0))
    {
      printf( "disable write protect of flash#0 failed!\n");
      ret = -1;
      goto done;
    }

   
    if(0 != mt53xx_nor_WaitBusy(mt53xx_nor, SFLASH_WRITEBUSY_TIMEOUT))
    {
      printf( "wait write busy of flash#0 failed!\n");
      ret = -1;
      goto done;
    }

    if(0 != mt53xx_nor_WriteEnable(mt53xx_nor))
    {
      printf( "write enable of flash#0 failed!\n");
      ret = -1;
      goto done;
    }


  }
	  
  if(info & REGION_RANK_SECOND_FLASH)
  {
    mt53xx_nor->cur_flash_index = 1;

    if(protect_2nd_nor)
    	{
    		printf( "Cancel the protect at 2nd nor before write data\n");
    		//printf( "[%s(%d)]%s (addr = %08x, len = %08x, retlen = %08x)\n", current->comm, current->pid, __FUNCTION__, 
    		//         (u32)addr,(u32)len,(u32)(*retlen));
		    if(0 != mt53xx_nor_WriteProtect(mt53xx_nor, 0))
		    {
		      printf( "disable write protect of flash#1 failed!\n");
		      ret = -1;
		      goto done;
		    }
    	}



    if(0 != mt53xx_nor_WaitBusy(mt53xx_nor, SFLASH_WRITEBUSY_TIMEOUT))
    {
      printf( "wait write busy of flash#1 failed!\n");
      ret = -1;
      goto done;
    }


    if(0 != mt53xx_nor_WriteEnable(mt53xx_nor))
    {
      printf( "write enable of flash#1 failed!\n");
      ret = -1;
      goto done;
    }	


  } 
#ifdef BD_NOR_ENABLE
  /* handle no-buffer-align case */ 
  pgalign = addr % SFLASH_WRBUF_SIZE;
  if(pgalign != 0)
  {
    for(i=0; i<(SFLASH_WRBUF_SIZE - pgalign); i++)
    {
      if(mt53xx_nor_WriteSingleByte(mt53xx_nor, addr, *buf) != 0)
      {
        ret = -1;
        goto done;
      }
      addr++;
      buf++;
      len--;
      (*retlen)++;
	  
      if(len == 0)
      {
        ret = 0;
        len = (u32)len;
        goto done;		
      }
    }
  }
	
  /* handle buffer case */
  if(mt53xx_nor_WriteBufferEnable(mt53xx_nor) != 0)
  {
    return -1;
  }
  while((u32)len > 0)
  {
    if(len >= SFLASH_WRBUF_SIZE)
    {
      count = SFLASH_WRBUF_SIZE;
    }
    else
    {
      // Not write-buffer alignment
      break;
    }
	
    if(mt53xx_nor_WriteBuffer(mt53xx_nor, addr, count, buf) != 0)
    {
      if(mt53xx_nor_WriteBufferDisable(mt53xx_nor) != 0)
      {
        ret = -1;
      }
      ret = -1;
      goto done;
    }
		
    len -= count;
    (*retlen) += count;
    addr += count;
    buf += count;
  }
  if(mt53xx_nor_WriteBufferDisable(mt53xx_nor) != 0)
  {
    ret = -1;
    goto done;
  }
	
  /* handle remain case */
  if((INT32)len > 0)
  {
    for(i=0; i<len; i++,  (*retlen)++)
    {
      if(mt53xx_nor_WriteSingleByte(mt53xx_nor, addr, *buf) != 0)
      {
        if(mt53xx_nor_WriteBufferDisable(mt53xx_nor) != 0)
        {
          ret =  -1;
        }
        ret =  -1;
        goto done;
      }
		  
      addr++;
      buf++;
    }
  }
  
#else


  for(i=0; i<((u32)len); i++, (*retlen)++)
  {
    if(mt53xx_nor_WriteSingleByte(mt53xx_nor, addr, *buf) != 0)
    {
      ret = -1;
      goto done;
    }
    addr++;
    buf++;
  }
  if(!enable_protect_ro)
  	 printf("%s (%d)\n",__FUNCTION__,mt53xx_nor->cur_flash_index,mt53xx_nor->cur_addr_offset,mt53xx_nor->cur_sector_index);
  if(info & REGION_RANK_SECOND_FLASH)
   {

  		if(protect_2nd_nor)
    	{
    		printf( "  Protect 2nd nor after  write data\n");
    		printf( "[%s(%d)]%s (addr = %08x, len = %08x, retlen = %08x)\n", current->comm, current->pid, __FUNCTION__, 
    		         (u32)addr,(u32)len,(u32)(*retlen));
		    if(0 != mt53xx_nor_WriteProtect(mt53xx_nor, 0))
		    {
		      printf( "disable write protect of flash#1 failed!\n");
		      ret = -1;
		      goto done;
		    }
    	}
  	}

#endif

done:

	return ret;

}
#if NOR_DEBUG
static u32 Get_Kernel_rootfs_checksum()
{
	u32 i,j,count=0;
	u8 *nor_baseAddress;
	u32 ichecksum;

	nor_baseAddress=0xf8000000+0x30000;
	for(i=0;i<0x93;i++)
		{
		nor_baseAddress +=0x10000;
		ichecksum=0;
		for(j=0;j<0x10000;j++)
			{
				ichecksum +=nor_baseAddress[j];

			}
		
		printf( "0X%08x checksum:0x%08x\n",nor_baseAddress,ichecksum);
		
		}

}
#endif

u32 mtk_nor_write(u32 to, u32 len, size_t *retlen, const u_char *buf)
{

  u32 ret = 0;
#if NOR_DEBUG
  struct timeval t0;
#endif
  if (retlen)
    *retlen = 0;

  //if the address of nor < 0x30000, the system will be assert
 if((to < 0x30000)&&(enable_protect_ro==1))
  {
	 printf("Error: nor write address at 0x%x < 0x30000\n",to);
	 //while(1) {};
	 BUG_ON(1);
	 return -EINVAL;
  }
  if (!len)
  {
    return 0;
  }
	
  //if (to + len > mt53xx_nor->mtd.size)
  if (to + len > mt53xx_nor->total_sz)
  {
  	printf( "Error: nor write address to +len at 0x%x >0xc00000\n",to+len);
	//while(1) {};
    return -EINVAL;
  }

  /* Byte count starts at zero. */
  *retlen = 0;

  //mutex_lock(&mt53xx_nor->lock);

  if(0 != mt53xx_nor_Write(mt53xx_nor, (u32)to, (u32)len, (u32 *)retlen, (u8 *)buf))
  {
    ret = -1;
  }
 #if NOR_DEBUG
  do_gettimeofday(&t0);
  printf( "[Time trace:%s(%d) time = %ld\n", __FUNCTION__,__LINE__,t0.tv_sec*1000+t0.tv_usec); 
#endif	
  //mutex_unlock(&mt53xx_nor->lock);

  return ret;
  
}

//static int mt53xx_nor_read(struct mtd_info *mtd, loff_t from, size_t len, size_t *retlen, u_char *buf)

static unsigned short NFI_gpio_uffs(unsigned short x)
{
	unsigned int r = 1;

	if (!x)
		        return 0;

	if (!(x & 0xff)) {
		        x >>= 8;
			        r += 8;
	}

	if (!(x & 0xf)) {
		        x >>= 4;
			        r += 4;
	}

	if (!(x & 3)) {
		        x >>= 2;
			        r += 2;
	}

	if (!(x & 1)) {
		        x >>= 1;
			        r += 1;
	}

	return r;
}

static void NFI_GPIO_SET_FIELD(unsigned int  reg, unsigned int  field, unsigned int  val)
{
	unsigned short tv = (unsigned short)(*(volatile unsigned short *)(reg));
	tv &= ~(field);
	tv |= ((val) << (NFI_gpio_uffs((unsigned short)(field)) - 1));
	(*(volatile unsigned short *)(reg) = (unsigned short)(tv));
}

static void NFI_set_gpio_mode(int gpio, int value)
{
	unsigned short ori_gpio, offset, rev_gpio;
	unsigned short* gpio_mode_p;

	gpio_mode_p = (unsigned short *)( 0x10005760+ (gpio/5) * 0x10); // Use APMCU COMPILE

	ori_gpio = *gpio_mode_p;
	offset = gpio % 5;
	rev_gpio = ori_gpio & (0xffff ^ (0x7<<(offset*3)));
	rev_gpio = rev_gpio |(value << (offset*3));
	*gpio_mode_p = rev_gpio;
}

#define             EFUSE_Is_IO_33V()               (((*((volatile unsigned int *)(0x102061c0)))&0x8)?0:1) // 0 : 3.3v (MT8130 default),

static s32 _SetFlashExit4Byte(struct mt53xx_nor *mt53xx_nor)
{
	u32 u4DualReg, u4Polling;

	u4DualReg = SFLASH_RREG32(SFLASH_DUAL_REG); 
	u4DualReg &= ~0x10;
	SFLASH_WREG32(SFLASH_DUAL_REG, u4DualReg);

	u4Polling = 0;
	while(1)
	{
		u4DualReg = SFLASH_RREG32(SFLASH_DUAL_REG); 
		if (!(u4DualReg & 0x10))
		{
			break;
        	}
		u4Polling++;
		if(u4Polling > SFLASH_POLLINGREG_COUNT)
		{
			printk("exit 4 byte mode polling fail %x\n", u4DualReg);
			return 1;
		}
	}

	if(mt53xx_nor_WriteEnable(mt53xx_nor) != 0)
	{
		printk("enter 4 byte mode write enable fail\n");
		return 1;
	}

	if( (mt53xx_nor->flash_info[0].u1MenuID == 0x01) &&(mt53xx_nor->flash_info[0].u1DevID1 == 0x02) &&(mt53xx_nor->flash_info[0].u1DevID2 == 0x19) )	//for spansion S25FL256s flash
	{
		SFLASH_WREG8(SFLASH_PRGDATA5_REG, 0x17);	// Write Bank register
		SFLASH_WREG8(SFLASH_PRGDATA4_REG, 0x0);		// Clear 4B mode
		SFLASH_WREG8(SFLASH_CNT_REG,16); 		// Write SF Bit Count
	}
		else
	{
		SFLASH_WREG8(SFLASH_PRGDATA5_REG, 0xE9);	//Exit 4B cmd
		SFLASH_WREG8(SFLASH_CNT_REG,8); 		// Write SF Bit Count
	}
   
	mt53xx_nor->cur_cmd_val = 0x04;
	if(-1 == mt53xx_nor_ExecuteCmd(mt53xx_nor))
	{
		printk(KERN_ERR "4byte addr enable failed!\n");
		return -1;
	}
	return 0;
}

static s32 _SetFlashEnter4Byte(struct mt53xx_nor *mt53xx_nor)
{
	u32 u4DualReg, u4Polling;

    // dont enter 4 byte mode if size < 32MB
    if (mt53xx_nor->flash_info[0].u4ChipSize < 0x2000000)
    {
	    printk("size = %x, exit 4 byte mode\n", mt53xx_nor->flash_info[0].u4ChipSize);
	    return _SetFlashExit4Byte(mt53xx_nor);
    }
    else
	    printk("size = %x, enter 4 byte mode\n", mt53xx_nor->flash_info[0].u4ChipSize);

    if(mt53xx_nor_WriteEnable(mt53xx_nor) != 0)
    {
	printk("enter 4 byte mode write enable fail\n");
        return 1;
    }	

	
    if( (mt53xx_nor->flash_info[0].u1MenuID == 0x01) &&(mt53xx_nor->flash_info[0].u1DevID1 == 0x02) &&(mt53xx_nor->flash_info[0].u1DevID2 == 0x19) )	//for spansion S25FL256s flash
    {
        SFLASH_WREG8(SFLASH_PRGDATA5_REG, 0x17);	//Enter EN4B cmd
        SFLASH_WREG8(SFLASH_PRGDATA4_REG, 0x80);	
        SFLASH_WREG8(SFLASH_CNT_REG,16); 			// Write SF Bit Count
    }
    else
    {
        SFLASH_WREG8(SFLASH_PRGDATA5_REG, 0xb7);	//Enter EN4B cmd
        SFLASH_WREG8(SFLASH_CNT_REG,8); 			// Write SF Bit Count
    }
   
    mt53xx_nor->cur_cmd_val = 0x04;
    if(-1 == mt53xx_nor_ExecuteCmd(mt53xx_nor))
    {
        printk(KERN_ERR "4byte addr enable failed!\n");
        return -1;
    }
	
	u4DualReg = SFLASH_RREG32(SFLASH_DUAL_REG); 
	u4DualReg |= 0x10;
	SFLASH_WREG32(SFLASH_DUAL_REG, u4DualReg);

    u4Polling = 0;
    while(1)
    {
        u4DualReg = SFLASH_RREG32(SFLASH_DUAL_REG); 
        if (0x10 == (u4DualReg & 0x10))
        {
            break;
        }
        u4Polling++;
        if(u4Polling > SFLASH_POLLINGREG_COUNT)
        {
	    printk("enter 4 byte mode polling fail\n");
            return 1;
        }
    }	
	//LOG(0, "...cdd Enter 4 bytes address!\n");
        printk(KERN_ERR "4byte addr enable done!\n");
	return 0;
}

#if defined(__UBOOT_NOR__)
static struct mt53xx_nor mt53xx_nor_tmp;
static SFLASHHW_VENDOR_T flash_info_tmp;


int mtk_nor_init(void)
{
	// to do: need to remove later
	{
		unsigned char is33v;
		unsigned int  val;

		NFI_set_gpio_mode(236, 1);    //Switch EXT_SDIO3 to non-default mode
		NFI_set_gpio_mode(237, 1);    //Switch EXT_SDIO2 to non-default mode
		NFI_set_gpio_mode(238, 1);    //Switch EXT_SDIO1 to non-default mode
		NFI_set_gpio_mode(239, 1);    //Switch EXT_SDIO0 to non-default mode
		NFI_set_gpio_mode(240, 1);    //Switch EXT_XCS to non-default mode
		NFI_set_gpio_mode(241, 1);    //Switch EXT_SCK to non-default mode

		NFI_GPIO_SET_FIELD(0x10005360, 0xF000, 0xF);    //set NOR IO pull up
		NFI_GPIO_SET_FIELD(0x10005370, 0x3, 0x3);   //set NOR IO pull up


		is33v = EFUSE_Is_IO_33V();
		val = is33v ? 0x0c : 0x00;


		NFI_GPIO_SET_FIELD(0x10005c00, 0xf,   0x0a);    /* TDSEL change value to 0x0a*/
		NFI_GPIO_SET_FIELD(0x10005c00, 0x3f0, val);     /* RDSEL change value to val*/
	}
	
	if (!mt53xx_nor)
	{
		mt53xx_nor = &mt53xx_nor_tmp;
		#if 0
 		mt53xx_nor = malloc(sizeof (*mt53xx_nor));
 		if(!mt53xx_nor)
 			return -ENOMEM;
		#endif
	}

	if (!mt53xx_nor->flash_info)
	{
	    mt53xx_nor->flash_info = &flash_info_tmp;
	    #if 0
		mt53xx_nor->flash_info = malloc(MAX_FLASHCOUNT*sizeof( SFLASHHW_VENDOR_T));
  		if(!mt53xx_nor->flash_info)
  		{
  			kfree(mt53xx_nor);
  			return -ENOMEM;
		}
		#endif
  	}
                    
 	mt53xx_nor->cur_flash_index = 0;
 	mt53xx_nor->cur_cmd_val = 0;
 
  	/*
     	 * switch pinmux for nor flash
     	 */
 	mt53xx_nor_PinMux();

 	SFLASH_WREG32(SFLASH_CFG1_REG, 0x20);

 	SFLASH_WREG32(SFLASH_WRPROT_REG, 0x30);
 	//set clock
 	SFLASH_WRITE32(0x100000B0, SFLASH_READ32(0x100000B0) & ~0x3); // to do: remove Hard code

	SFLASH_WREG32(SFLASH_FIN_REG, 0x4);
	
	mt53xx_nor_GetID(mt53xx_nor);
	printf("Roger debug: size=%x, flash_num=%d\n", mt53xx_nor->total_sz, mt53xx_nor->flash_num); // Roger debug

 	mt53xx_nor_WriteProtectAllChips(mt53xx_nor, 0);

	mt53xx_nor->cur_flash_index = 0;
 	
	if(0 != mt53xx_nor_WriteProtect(mt53xx_nor, 0))
 	{
        	printf( "Flash #%d: write protect disable failed!\n", mt53xx_nor->cur_flash_index);
	 
 	}
	else
	{
		printf("Flash #%d: write protect enable successfully\n", mt53xx_nor->cur_flash_index);
	}

 	mt53xx_nor->host_act_attr = SFLASH_USE_DUAL_READ; // no DMA/ISR in Uboot
 	_SetFlashEnter4Byte(mt53xx_nor);

	return 0;
}


#define MTK_NOR_DBG_CMD

static int mtk_nor_command(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int addr;
	int len, i;
	size_t retlen = 0;
	u8 *p = NULL;

	if (!strncmp(argv[1], "id", 3)) {
		SFLASHHW_VENDOR_T tmp_flash_info;
		u8 cmdval = 0x9F;
		SFLASH_WREG8(SFLASH_PRGDATA5_REG, cmdval);
		SFLASH_WREG8(SFLASH_PRGDATA4_REG, 0x00);
		SFLASH_WREG8(SFLASH_PRGDATA3_REG, 0x00);
		SFLASH_WREG8(SFLASH_PRGDATA2_REG, 0x00);
		SFLASH_WREG8(SFLASH_CNT_REG, 32);

		mt53xx_nor->cur_flash_index = 0;
		mt53xx_nor->cur_cmd_val = 0x04;

		if(0 != mt53xx_nor_ExecuteCmd(mt53xx_nor))
		{
			return -EIO;
		}

		tmp_flash_info.u1DevID2 = SFLASH_RREG8(SFLASH_SHREG0_REG);
		tmp_flash_info.u1DevID1 = SFLASH_RREG8(SFLASH_SHREG1_REG);	
		tmp_flash_info.u1MenuID = SFLASH_RREG8(SFLASH_SHREG2_REG);
		u4MenuID=tmp_flash_info.u1MenuID;

		mt53xx_nor->cur_cmd_val = 0x00;
		mt53xx_nor_SendCmd(mt53xx_nor);
		printf( "Flash MenuID: %02x, DevID1: %02x, DevID2: %02x\n", 
                                            tmp_flash_info.u1MenuID,
                                            tmp_flash_info.u1DevID1,
                                            tmp_flash_info.u1DevID2);

	}
	else if (!strncmp(argv[1], "read", 5)) {
		addr = (unsigned int)simple_strtoul(argv[2], NULL, 16);
		len = (int)simple_strtoul(argv[3], NULL, 16);
		p = (u8 *)malloc(len);
		if (!p) {
			printf("malloc failed\n");
			return 0;
		}
		if (mtk_nor_read(addr, len, &retlen, p))
		{
			free(p);
			printf("read fail\n");
			return 0;
		}
		free(p);

	}
	else if (!strncmp(argv[1], "erase", 6)) {
		addr = (unsigned int)simple_strtoul(argv[2], NULL, 16);
		len = (int)simple_strtoul(argv[3], NULL, 16);
		if (mtk_nor_erase(addr, len))
			printf("Erase Fail\n");
	}
	else if (!strncmp(argv[1], "write", 6)) {
		u8 *porig;
		char t[3] = {0};
		addr = (unsigned int)simple_strtoul(argv[2], NULL, 16);
		len = strlen(argv[3]) / 2;

		porig = malloc(len+32);
		if (!porig)
			printf("malloc fail\n");
		p = (u8 *)((((u32)porig+31)/32)*32);
		for (i = 0; i < len; i++) {
			t[0] = argv[3][2*i];
			t[1] = argv[3][2*i+1];
			*(p + i) = simple_strtoul(t, NULL, 16);
		}
		mtk_nor_write(addr, len, &retlen, p);

		free(porig);
	}
	else if (!strncmp(argv[1], "init", 5)) {
		mtk_nor_init();
	}
	else
		printf("Usage:\n%s\n use \"help nor\" for detail!\n", cmdtp->usage);
					     
	return 0;
}


#ifdef MTK_NOR_DBG_CMD

static int mtk_nor_command(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[]);

U_BOOT_CMD(
		nor,   4,      1,      mtk_nor_command,
		"nor   - nor flash command\n",
		"nor usage:\n"
		"  nor id\n"
		"  nor read <addr> <len>\n"
		"  nor write <addr> <data...>\n"
		"  nor erase <addr> <len>\n"
		"  nor init\n"
	  );

#endif // MTK_NOR_DBG_CMD

static int mtk_uboot_nor_command(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned int addr;
	int len;
	size_t retlen = 0;
	u8 *p = NULL;

	if (!strncmp(argv[1], "read", 5)) {
		p = (u8 *)simple_strtoul(argv[2], NULL, 16);
		addr = (unsigned int)simple_strtoul(argv[3], NULL, 16);
		len = (int)simple_strtoul(argv[4], NULL, 16);
		if (!p) {
			printf("malloc failed\n");
			return 0;
		}
		if (mtk_nor_read(addr, len, &retlen, p))
		{
			printf("read fail\n");
			return 0;
		}
		printf("read len: %d\n", retlen);
	}
	else if (!strncmp(argv[1], "erase", 6)) {
		addr = (unsigned int)simple_strtoul(argv[2], NULL, 16);
		len = (int)simple_strtoul(argv[3], NULL, 16);
		if (mtk_nor_erase(addr, len))
			printf("Erase Fail\n");
	}
	else if (!strncmp(argv[1], "write", 6)) {
		p = (u8 *)simple_strtoul(argv[2], NULL, 16);
		addr = (unsigned int)simple_strtoul(argv[3], NULL, 16);
		len = (int)simple_strtoul(argv[4], NULL, 16);
		mtk_nor_write(addr, len, &retlen, p);
	}
	else
		printf("Usage:\n%s\n use \"help nor\" for detail!\n", cmdtp->usage);
					     
	return 0;
}

U_BOOT_CMD(
		snor,   5,      1,      mtk_uboot_nor_command,
		"snor   - spi-nor flash command\n",
		"snor usage:\n"
		"  snor read <load_addr> <addr> <len>\n"
		"  snor write <load_addr> <addr> <len>\n"
		"  snor erase <addr> <len>\n"
	  );

#endif // defined(__UBOOT_NOR__)


