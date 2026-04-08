/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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
