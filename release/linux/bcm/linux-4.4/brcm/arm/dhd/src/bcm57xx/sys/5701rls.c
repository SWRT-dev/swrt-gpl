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
#ifdef INCLUDE_5701_AX_FIX

#include "mm.h"
#include "5701rls.h"

LM_STATUS LM_LoadRlsFirmware(PLM_DEVICE_BLOCK pDevice)
{
  T3_FWIMG_INFO FwImgInfo;

  FwImgInfo.StartAddress = t3FwStartAddr;
  FwImgInfo.Text.Buffer = (PLM_UINT8)t3FwText;
  FwImgInfo.Text.Offset  = t3FwTextAddr;
  FwImgInfo.Text.Length  = t3FwTextLen;
  FwImgInfo.ROnlyData.Buffer = (PLM_UINT8)t3FwRodata;
  FwImgInfo.ROnlyData.Offset  = t3FwRodataAddr;
  FwImgInfo.ROnlyData.Length  = t3FwRodataLen;
  FwImgInfo.Data.Buffer = (PLM_UINT8)t3FwData;
  FwImgInfo.Data.Offset  = t3FwDataAddr;
  FwImgInfo.Data.Length  = t3FwDataLen;

  if (LM_LoadFirmware(pDevice,
                      &FwImgInfo,
                      T3_RX_CPU_ID | T3_TX_CPU_ID,
                      T3_RX_CPU_ID) != LM_STATUS_SUCCESS)
    {
      return LM_STATUS_FAILURE;
    }
  
  return LM_STATUS_SUCCESS;
}

#endif /* INCLUDE_5701_AX_FIX */
