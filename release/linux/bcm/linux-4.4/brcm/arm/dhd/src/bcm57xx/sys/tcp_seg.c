/*
 * Copyright (C) 2016, Broadcom. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * <<Broadcom-WL-IPTag/Open:>>
 *
 */

#include <bcm57xx_cfg.h>
#include "mm.h"

#ifdef INCLUDE_TCP_SEG_SUPPORT
#include "fw_stkoffld.h"
#include "fw_lso05.h"

LM_UINT32 LM_GetStkOffLdFirmwareSize(PLM_DEVICE_BLOCK pDevice)
{
  LM_UINT32 FwSize;

  if (T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5750)
  {
      return 0;
  }
  if (T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5705) 
    {
      FwSize =  (LM_UINT32)(t3StkOffLd05FwTextLen + 
			    t3StkOffLd05FwRodataLen + 
			    t3StkOffLd05FwDataLen +
			    t3StkOffLd05FwSbssLen +
			    t3StkOffLd05FwBssLen);
    }
  else
    {
      FwSize = (LM_UINT32)(t3StkOffLdFwTextLen +
			   t3StkOffLdFwRodataLen +
			   t3StkOffLdFwDataLen +
			   t3StkOffLdFwSbssLen +
			   t3StkOffLdFwBssLen);
    }

  return FwSize;
}

LM_STATUS LM_LoadStkOffLdFirmware(PLM_DEVICE_BLOCK pDevice)
{
  T3_FWIMG_INFO FwImgInfo;
  LM_UINT32 Cpu;

  if (T3_ASIC_IS_575X_PLUS(pDevice->ChipRevId))
  {
      return LM_STATUS_SUCCESS;
  }
  if (T3_ASIC_REV(pDevice->ChipRevId) == T3_ASIC_REV_5705) 
    {
      FwImgInfo.StartAddress = t3StkOffLd05FwStartAddr;
      FwImgInfo.Text.Buffer = (PLM_UINT8)t3StkOffLd05FwText;
      FwImgInfo.Text.Offset  = t3StkOffLd05FwTextAddr;
      FwImgInfo.Text.Length  = t3StkOffLd05FwTextLen;
      FwImgInfo.ROnlyData.Buffer = (PLM_UINT8)t3StkOffLd05FwRodata;
      FwImgInfo.ROnlyData.Offset  = t3StkOffLd05FwRodataAddr;
      FwImgInfo.ROnlyData.Length  = t3StkOffLd05FwRodataLen;
      FwImgInfo.Data.Buffer = (PLM_UINT8)t3StkOffLd05FwData;
      FwImgInfo.Data.Offset  = t3StkOffLd05FwDataAddr;
      FwImgInfo.Data.Length  = t3StkOffLd05FwDataLen;
      FwImgInfo.Sbss.Offset = t3StkOffLd05FwSbssAddr;
      FwImgInfo.Sbss.Length = t3StkOffLd05FwSbssLen;
      FwImgInfo.Bss.Offset = t3StkOffLd05FwBssAddr;
      FwImgInfo.Bss.Length = t3StkOffLd05FwBssLen;
      Cpu = T3_RX_CPU_ID;
    }
  else
    {
      FwImgInfo.StartAddress = t3StkOffLdFwStartAddr;
      FwImgInfo.Text.Buffer = (PLM_UINT8)t3StkOffLdFwText;
      FwImgInfo.Text.Offset  = t3StkOffLdFwTextAddr;
      FwImgInfo.Text.Length  = t3StkOffLdFwTextLen;
      FwImgInfo.ROnlyData.Buffer = (PLM_UINT8)t3StkOffLdFwRodata;
      FwImgInfo.ROnlyData.Offset  = t3StkOffLdFwRodataAddr;
      FwImgInfo.ROnlyData.Length  = t3StkOffLdFwRodataLen;
      FwImgInfo.Data.Buffer = (PLM_UINT8)t3StkOffLdFwData;
      FwImgInfo.Data.Offset  = t3StkOffLdFwDataAddr;
      FwImgInfo.Data.Length  = t3StkOffLdFwDataLen;
      FwImgInfo.Sbss.Offset = t3StkOffLdFwSbssAddr;
      FwImgInfo.Sbss.Length = t3StkOffLdFwSbssLen;
      FwImgInfo.Bss.Offset = t3StkOffLdFwBssAddr;
      FwImgInfo.Bss.Length = t3StkOffLdFwBssLen;
      Cpu = T3_TX_CPU_ID;
    }

  if (LM_LoadFirmware(pDevice,
                      &FwImgInfo,
                      Cpu,
                      Cpu) != LM_STATUS_SUCCESS)
    {
      return LM_STATUS_FAILURE;
    }
  
  return LM_STATUS_SUCCESS;
}

#endif /* INCLUDE_TCP_SEG_SUPPORT */
