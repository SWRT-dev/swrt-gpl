/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 * 
 *
 * Driver API
 *
 */
#ifndef __DRIVER_API_H__
#define __DRIVER_API_H__

int __MTLK_IFUNC
dut_api_init(int argc, char *argv[]);

void __MTLK_IFUNC
dut_api_cleanup(void);

int __MTLK_IFUNC
dut_api_driver_stop(BOOL doReset);

int __MTLK_IFUNC
dut_api_driver_start(BOOL doReset);

int __MTLK_IFUNC
dut_api_upload_progmodel (dut_progmodel_t *data, int length, int hw_idx);

int __MTLK_IFUNC
dut_api_send_fw_command(const char *data, int length, int hw_idx);

int __MTLK_IFUNC
dut_api_send_drv_command(const char *data, int length, int hw_idx);

int __MTLK_IFUNC
dut_api_send_platform_command(const char *data, int length, int hw_idx);

int __MTLK_IFUNC
dut_api_eeprom_data_on_flash_prepare(uint32 size, int hw_idx);

BOOL __MTLK_IFUNC
dut_api_is_connected_to_hw(int hw_idx);

#endif /* __DRIVER_API_H__ */
