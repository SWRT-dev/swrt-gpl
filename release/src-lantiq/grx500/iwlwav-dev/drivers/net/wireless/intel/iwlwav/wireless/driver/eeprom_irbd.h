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
 * $Id$
 *
 *  Read/Write data into the EEPROM via IRB interface.
 */
#ifndef __EEPROM_IRBA_H__
#define __EEPROM_IRBA_H__

typedef struct _mtlk_eeprom_irbd_t mtlk_eeprom_irbd_t;

mtlk_eeprom_irbd_t* __MTLK_IFUNC
mtlk_eeprom_irbd_register (mtlk_vap_handle_t vap);

void __MTLK_IFUNC
mtlk_eeprom_irbd_unregister (mtlk_eeprom_irbd_t* obj);

#endif /* __EEPROM_IRBA_H__ */
