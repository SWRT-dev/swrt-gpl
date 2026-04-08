/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_RF_MGMT_IRB_H__
#define __MTLK_RF_MGMT_IRB_H__

#include "mtlkguid.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

/*******************************************************************************
 * Enable/Disable RF MGMT (APP => DRV only)
 *******************************************************************************/
struct mtlk_rf_mgmt_evt_type
{
  UMI_RF_MGMT_TYPE type;
  uint32           spr_queue_size; /* IN  (enable=1) max SPRs to store             */
  int32            result;         /* OUT MTLK_ERR_... code                        */
} __MTLK_IDATA;
/*******************************************************************************/

struct mtlk_rf_mgmt_evt_def_data
{
  UMI_DEF_RF_MGMT_DATA data;
  int32                result; /* OUT MTLK_ERR_... code                 */
} __MTLK_IDATA;
/*******************************************************************************/

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_RF_MGMT_IRB_H__ */
