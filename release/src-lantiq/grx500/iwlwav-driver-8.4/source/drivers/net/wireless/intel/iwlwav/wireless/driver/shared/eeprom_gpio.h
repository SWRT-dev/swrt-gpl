/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 * $Id: $
 *
 *  Read/Write data into the EEPROM.
 */
#ifndef __EEPROM_GPIO_H__
#define __EEPROM_GPIO_H__

int __MTLK_IFUNC
eeprom_gpio_init(mtlk_ccr_t *ccr, uint32 *size);

void __MTLK_IFUNC
eeprom_gpio_clean(mtlk_ccr_t *ccr);

int __MTLK_IFUNC
eeprom_gpio_read(mtlk_ccr_t *ccr, uint32 offset, void *buffer, int len);

int __MTLK_IFUNC
eeprom_gpio_write(mtlk_ccr_t *ccr, uint32 offset, void const *buffer, int len);

#endif /* __EEPROM_GPIO_H__ */
