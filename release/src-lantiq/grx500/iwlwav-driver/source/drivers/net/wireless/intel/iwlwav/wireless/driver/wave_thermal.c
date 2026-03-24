/******************************************************************************
  Copyright (c) 2020, MaxLinear, Inc.

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

The source code contained or described herein and all documents related to the
source code ("Material") are owned by Intel Corporation or its suppliers or
licensors. Title to the Material remains with Maxlinear Inc. or its suppliers
and licensors. The Material contains trade secrets and proprietary and
confidential information of Maxlinear or its suppliers and licensors. The
Material is protected by worldwide copyright and trade secret laws and treaty
provisions. No part of the Material may be used, copied, reproduced, modified,
published, uploaded, posted, transmitted, distributed, or disclosed in any way
without Maxlinear's prior express written permission.

No license under any patent, copyright, trade secret or other intellectual
property right is granted to or conferred upon you by disclosure or delivery of
the Materials, either expressly, by implication, inducement, estoppel or
otherwise. Any license under such intellectual property rights must be express
and approved by Maxlinear in writing.

******************************************************************************/

#include "mtlkinc.h"
#include "hw_mmb.h"

#ifdef CONFIG_THERMAL
#include <linux/thermal.h>
#endif
#include <linux/of.h>
#include <linux/units.h>

#define LOG_LOCAL_GID   GID_THERMAL
#define LOG_LOCAL_FID   1

#ifdef CONFIG_THERMAL
struct wave_thermal {
  struct thermal_zone_device *tzd;
};

void __MTLK_IFUNC
wave_hw_set_thermal_mgmt_data (mtlk_hw_t *hw, struct wave_thermal *thermal_mgmt);

struct wave_thermal*  __MTLK_IFUNC
wave_hw_get_thermal_mgmt_data (mtlk_hw_t *hw);

static void
_wave_thermal_cooling_device_register (void)
{
  ILOG1_V("Thermal: DUMMY Register cooling device");
}

static void
_wave_thermal_cooling_device_unregister (void)
{
  ILOG1_V("Thermal: DUMMY  Unregister cooling device");
}

int __MTLK_IFUNC
wave_thermal_zone_get_temp (void *data, int *temperature)
{
  uint8 card_idx;
  int temp = 0;
  mtlk_core_t* master_core;
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_hw_t *hw = ((mtlk_hw_t*) data);

  MTLK_ASSERT(hw != NULL);
  card_idx = mtlk_hw_mmb_get_card_idx(hw);

  /* Query the temperature sensor value when core state is connected */
  master_core = wave_hw_get_first_radio_master_vap_core(hw);
  if (mtlk_core_get_net_state(master_core) != NET_STATE_CONNECTED) {
    ILOG2_D("Thermal: CID %02x Failed to read temperature, core not ready", card_idx);
    return _mtlk_df_mtlk_to_linux_error_code(MTLK_ERR_NOT_READY);
  }

  /* read temperature value */
  res = mtlk_core_get_temperature_req(master_core, &temp, 0);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Thermal: CID %02x Failed to read temperature data for tzd", card_idx);
    return _mtlk_df_mtlk_to_linux_error_code(res);
  }

  /* convert to millicelsius */
  *temperature = temp * MILLIDEGREE_PER_DEGREE;

  ILOG2_DD("Thermal: CID %02x temperature %d", card_idx, *temperature);

  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static struct thermal_zone_of_device_ops tzd_ops = {
  .get_temp = wave_thermal_zone_get_temp,
};

static mtlk_error_t
_wave_thermal_thermal_zone_register (struct pci_dev *pdev, mtlk_hw_t *hw)
{
  struct device_node *np;
  struct wave_thermal *thermal;

  uint8 card_idx = mtlk_hw_mmb_get_card_idx(hw);
  wave_radio_descr_t *radio_descr = wave_card_radio_descr_get(hw);

  /* Temperature sensor are per card. Should be OK using radio0 master core */
  wave_radio_t *radio = wave_radio_descr_wave_radio_get(radio_descr, 0);
  uint8 wave_band = wave_radio_band_get(radio);

  ILOG1_DD("Thermal: CID %02x Register thermal zone device, band %d", card_idx, wave_band);

  if (pdev->dev.of_node) {
      goto register_sensor;
  }

  /* PCI  device has no node in the DT, search for compatible devices */
  for_each_compatible_node(np, NULL, "mxl,wave") {
    u32 band;

    if (of_property_read_u32(np, "wave-band", &band) == 0) {
      if (band == wave_band) {
        ILOG1_D("Thermal: CID %02x found DT node for PCI dev", card_idx);
        pdev->dev.of_node = of_node_get(np);
        goto register_sensor;
      }
    }
  }

  /* TODO: No PCI device node, no compatible node, try to register a thermal
   * zone and don't use DT */

  ILOG0_D("Thermal: CID %02x No DT node for PCI dev", card_idx);
  /* DT node not found. Lets not fail, Skip tzd registeration */
  return MTLK_ERR_OK;

register_sensor:
  thermal = mtlk_osal_mem_alloc(sizeof(*thermal), MTLK_MEM_TAG_HW);
  if (!thermal) {
    ELOG_D("Thermal: CID %02x Failed to allocate thermal data", card_idx);
    return MTLK_ERR_NO_MEM;
  }

  thermal->tzd = devm_thermal_zone_of_sensor_register(&pdev->dev,
                   0, hw, &tzd_ops);
  if (IS_ERR(thermal->tzd)) {
    pr_err(" Thermal: CID %02x Failed to register thermal zone device"
           "( err %ld)", card_idx, PTR_ERR(thermal->tzd));
    thermal->tzd = NULL;
    mtlk_osal_mem_free(thermal);
    thermal = NULL;
    return MTLK_ERR_UNKNOWN;
  }

  /* per card thermal data */
  wave_hw_set_thermal_mgmt_data(hw, thermal);

  return MTLK_ERR_OK;
}

static void
_wave_thermal_thermal_zone_unregister (mtlk_hw_t *hw)
{
  struct wave_thermal *thermal = wave_hw_get_thermal_mgmt_data(hw);

  ILOG1_D("Thermal: CID %02x Unregister thermal zone device", mtlk_hw_mmb_get_card_idx(hw));

  if (!thermal)
    return;

  if (thermal->tzd) {
    thermal_zone_device_unregister(thermal->tzd);
    thermal->tzd = NULL;
  }

  mtlk_osal_mem_free(thermal);
  wave_hw_set_thermal_mgmt_data(hw, NULL);
}
#endif /*CONFIG_THERMAL */

mtlk_error_t __MTLK_IFUNC
wave_thermal_init (struct pci_dev *pdev, mtlk_hw_t *hw)
{
  mtlk_error_t res = MTLK_ERR_OK;
#ifdef CONFIG_THERMAL
  _wave_thermal_cooling_device_register();
  res = _wave_thermal_thermal_zone_register(pdev, hw);
#endif
  return res;
}

void __MTLK_IFUNC
wave_thermal_exit (mtlk_hw_t *hw)
{
#ifdef CONFIG_THERMAL
  _wave_thermal_cooling_device_unregister();
  _wave_thermal_thermal_zone_unregister(hw);
#endif
}
