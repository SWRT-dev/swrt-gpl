/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 *  Unit test implementation for file-system access
 *
 * FIXME:
 *   The driver helper should not know about EEPROM format.
 *   To remove code after CRC generation on EEPROM data originator.
 */
#ifndef __FSHLPR_UTEST_H__
#define __FSHLPR_UTEST_H__

#ifdef EEPROM_CRC_ON_DRVHLPR
#ifdef RUN_EEPROM_CRC_UTEST

enum {
  CRC_DONE              = 0x00000001,
  CIS_SIZE_WRONG        = 0x00000002,
  EEPROM_SIZE_WRONG     = 0x00000004,
  EEPROM_HEAD_WRONG     = 0x00000008,
  CIS_AREA_SIZE_WRONG   = 0x00000010,
  EEPROM_VERSION_WRONG  = 0x00000020,
  CRC_PRESENT           = 0x00000040,
  TERMINATOR_MISSED     = 0x00000080,
  BUFFER_REUSED         = 0x00000100,
  BUFFER_RECREATED      = 0x00000200,
  CIS_AREA_SIZE_UPDATED = 0x00000400,
  CRC_APPEND_FAILED     = 0x00000800,
  NEW_BUFFER_CLEANED    = 0x00001000
};

void __MTLK_IFUNC
eeprom_utest_step(uint32 step);

void __MTLK_IFUNC
eeprom_utest_write(void* buff, uint32 len);

void __MTLK_IFUNC
run_eeprom_crc_utest(
  void (*_file_save_handler)(
    mtlk_irba_t*,
    mtlk_handle_t,
    const mtlk_guid_t*,
    void*,
    uint32)
);

#else /* RUN_EEPROM_CRC_UTEST */

#define eeprom_utest_step(step)

#define eeprom_utest_write(buff, len)

#define run_eeprom_crc_utest(_file_save_handler)

#endif /* RUN_EEPROM_CRC_UTEST */
#endif /* EEPROM_CRC_ON_DRVHLPR */

#endif /* __FSHLPR_UTEST_H__ */
