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
 * $Id: $
 *
 *  Read/Write data into the EEPROM.
 *  The EEPROM interface fulfilled by using GPIO bits, controlled by
 *  a register in the HRC in address offset 0xc0.
 *  Register 0xc0 bits description:
 *      bit 0 - Software override enable.
 *      bit 4 - Clock Input.
 *      bit 5 - Scl- Clock Output.
 *      bit 8 - Data Input.
 *      bit 9 - SDA- Data Output.
 *      bit 10- OE- Data Output Enable.
 *
 *  The interface was planned to work with EEprom-AT24C02A/08A
 */

#include "mtlkinc.h"
#include "hw_mmb.h"
#include "eeprom_gpio.h"

#define LOG_LOCAL_GID   GID_EEPROM
#define LOG_LOCAL_FID   2

/*****************************************************************************
* Local definitions
******************************************************************************/

#define EEPROM_TEST_VALUE 0xAE

#define REG_LOCAL_EEPROM_OVERRIDE             0x00000001
#define REG_LOCAL_EEPROM_CLOCK_OUTPUT         0x00000020
#define REG_LOCAL_EEPROM_CLOCK_OUTPUT_ENABLE  0x00000040
#define REG_LOCAL_EEPROM_DATA_OUTPUT          0x00000200
#define REG_LOCAL_EEPROM_OUTPUT_ENABLE        0x00000400

#define OE_0_SDA_1_SCL_1   0x261

#define EEPROM_CHIP_ID (0x50)

/**
  Amount of bytes to write at one request

  \remarks
    The IS24CXX is capable of page-WRITE operation
      8-byte for 24C01/02
      16-byte for 24C04/08/16
*/
#define EEPROM_PAGE_SIZE 8

#define EEPROM_BIT_DELAY 5  /* usec */  /* TLow, THigh = 5usec */
#define EEPROM_SEQ_DELAY 10 /* msec*/   /* delay between stop(after eprom write) and start, TW= 10ms */

#define I2C_SCL(x)      mtlk_ccr_eeprom_override_write(ccr, (x ? REG_LOCAL_EEPROM_CLOCK_OUTPUT : 0), REG_LOCAL_EEPROM_CLOCK_OUTPUT)
#define I2C_SDA(x)      mtlk_ccr_eeprom_override_write(ccr, (x ? REG_LOCAL_EEPROM_DATA_OUTPUT  : 0), REG_LOCAL_EEPROM_DATA_OUTPUT)
#define I2C_DELAY       mtlk_udelay(EEPROM_BIT_DELAY)
#define I2C_DELAY_LONG  mtlk_mdelay(EEPROM_SEQ_DELAY)
#define I2C_TRISTATE    mtlk_ccr_eeprom_override_write(ccr, 0, REG_LOCAL_EEPROM_OUTPUT_ENABLE)
#define I2C_ACTIVE      mtlk_ccr_eeprom_override_write(ccr, REG_LOCAL_EEPROM_OUTPUT_ENABLE, REG_LOCAL_EEPROM_OUTPUT_ENABLE)
#define I2C_READ        (((mtlk_ccr_eeprom_override_read(ccr)) & 0x100) ? 1 : 0)

/*****************************************************************************
* Function implementation
******************************************************************************/

/**
  Transmit start sequence over I2C bus.

  \param ccr    Handle to the common HW accessor
*/
static void
I2C_START (mtlk_ccr_t *ccr)
{
    I2C_DELAY;
    I2C_SDA(1);
    I2C_ACTIVE;
    I2C_DELAY;
    I2C_SCL(1);
    I2C_DELAY;
    I2C_SDA(0);
    I2C_DELAY;
}

/**
  Transmit stop sequence over I2C bus.

  \param ccr    Handle to the common HW accessor
*/
static void
I2C_STOP (mtlk_ccr_t *ccr)
{
  I2C_SCL(0);
  I2C_DELAY;
  I2C_SDA(0);
  I2C_ACTIVE;
  I2C_DELAY;
  I2C_SCL(1);
  I2C_DELAY;
  I2C_SDA(1);
  I2C_DELAY;
  I2C_TRISTATE;
}

/**
  Transmit ACK sequence over I2C bus

  \param ccr    Handle to the common HW accessor
  \param ack    Acknowledge marker
*/
static __INLINE void
I2C_ACK (mtlk_ccr_t *ccr, int ack)
{
    I2C_SCL(0);
    I2C_DELAY;
    I2C_ACTIVE;
    I2C_SDA(ack);
    I2C_DELAY;
    I2C_SCL(1);
    I2C_DELAY;
    I2C_DELAY;
    I2C_SCL(0);
    I2C_DELAY;
}

/**
  Write a single byte in to the I2C bus

  \param ccr    Handle to the common HW accessor
  \param data   data to write

  \return
    Acknowledge marker (ack/nack)
*/
static int
_byte_write (mtlk_ccr_t *ccr, uint8 data)
{
    int j;
    int nack;

    I2C_ACTIVE;
    for (j=0; j <8; j++) {
        I2C_SCL(0);
        I2C_DELAY;
        I2C_SDA(data & 0x80);
        I2C_DELAY;
        I2C_SCL(1);
        I2C_DELAY;
        I2C_DELAY;
        data <<=1;
    }
    /*
    * Look for an <ACK>(negative logic) and return it.
    */
    I2C_SCL(0);
    I2C_DELAY;
    I2C_SDA(1);
    I2C_TRISTATE;
    I2C_DELAY;
    I2C_SCL(1);
    I2C_DELAY;
    I2C_DELAY;
    nack = I2C_READ;
    I2C_SCL(0);
    I2C_DELAY;
    I2C_ACTIVE;

    return(nack);   /* not a nack is an ack */
}

/**
  Read a single byte from I2C bus

  \param ccr    Handle to the common HW accessor
  \param ack    Acknowledge marker (ack/nack)

  \return
    data byte from EEPROM
*/
static uint8
_byte_read (mtlk_ccr_t *ccr, int ack)
{
  int  data;
  int  j;

  /*
  * Read 8 bits, MSB first.
  */
  I2C_TRISTATE;
  data = 0;
  for(j = 0; j < 8; j++)
  {
    I2C_SCL(0);
    I2C_DELAY;
    I2C_SCL(1);
    I2C_DELAY;
    data <<= 1;
    data |= I2C_READ;
    I2C_DELAY;
  }
  I2C_ACK(ccr, ack);

  return(data);
}

/**
  Read data from EEPROM via GPIO in to the buffer

  \param ccr    Handle to the common HW accessor
  \param chip   EEPROM ChipID
  \param addr   EEPROM data address
  \param alen   EEPROM data address length
  \param buffer Pointer to the destination buffer
  \param len    Size of destination buffer

  \return
    MTLK_ERR_...
*/
static __INLINE int
_buffer_read (mtlk_ccr_t *ccr, uint8 chip, uint32 addr, int alen, uint8 *buffer, int len)
{
  int shift;

  I2C_START(ccr);
  if(alen > 0)
  {
    if(_byte_write(ccr, chip << 1)) /* write cycle */
    {
      I2C_STOP(ccr);
      // no chip responded
      return MTLK_ERR_EEPROM;
    }
    shift = (alen-1) * 8;
    while(alen-- > 0)
    {
      if(_byte_write(ccr, addr >> shift))
      {
        // address not <ACK>ed
        return MTLK_ERR_EEPROM;
      }
      shift -= 8;
    }
    I2C_START(ccr);
  }

  /*
  * Send the chip address again, this time for a read cycle.
  * Then read the data.  On the last byte, we do a NACK instead
  * of an ACK(len == 0) to terminate the read.
  */
  _byte_write(ccr, (chip << 1) | 1);    /* read cycle */

  while(len-- > 0)
  {
    *buffer++ = _byte_read(ccr, len == 0);
  }
  I2C_STOP(ccr);

  return MTLK_ERR_OK;
}

/**
  Write data from the buffer to the EEPROM via GPIO

  \param ccr    Handle to the common HW accessor
  \param chip   EEPROM ChipID
  \param addr   EEPROM data address
  \param alen   EEPROM data address length
  \param buffer Pointer to the source buffer
  \param len    Size of source buffer

  \return
    MTLK_ERR_...

  \remarks
    The IS24CXX is capable of page-WRITE (8-byte for 24C01/
    02 and 16-byte for 24C04/08/16) operation. A page-WRITE
    is initiated in the same manner as a byte write, but instead
    of terminating the internal write cycle after the first data word
    is transferred, the master device can transmit up to N more
    bytes (N=7 for 24C01/02 and N=15 for 24C04/08/16). After
    the receipt of each data word, the IS24CXX responds
    immediately with an ACKnowledge on SDA line, and the
    three lower (24C01/24C02) or four lower (24C04/24C08/
    24C16) order data word address bits are internally
    incremented by one, while the higher order bits of the data
    word address remain constant. If the master device should
    transmit more than N+1 (N=7 for 24C01/02 and N=15 for
    24C04/08/16) words, prior to issuing the STOP condition,
    the address counter will 'roll over', and the previously
    written data will be overwritten. Once all N+1 (N=7 for
    24C01/02 and N=15 for 24C04/08/16) bytes are received
    and the STOP condition has been sent by the Master, the
     internal programming cycle begins. At this point, all received
    data is written to the IS24CXX in a single write cycle. All
    inputs are disabled until completion of the internal WRITE
    cycle.
*/
static __INLINE int
_page_write (mtlk_ccr_t *ccr, uint8 chip, uint32 addr, int alen, uint8 const *buffer, int len)
{
  int shift, failures = 0;

  MTLK_ASSERT(len <= EEPROM_PAGE_SIZE);

  I2C_START(ccr);

  if(_byte_write(ccr, chip << 1)) /* write cycle */
  {
    I2C_STOP(ccr);
    // no chip responded
    return MTLK_ERR_EEPROM;
  }

  shift = (alen-1) * 8;
  while(alen-- > 0)
  {
    if(_byte_write(ccr, addr >> shift))
    {
      // address not <ACK>ed
      return MTLK_ERR_EEPROM;
    }
    shift -= 8;
  }

  while(len-- > 0)
  {
    if(_byte_write(ccr, *buffer++))
    {
      failures++;
    }
  }

  I2C_STOP(ccr);
  I2C_DELAY_LONG;

  if (failures > 0)
  {
    return MTLK_ERR_EEPROM;
  }

  return MTLK_ERR_OK;
}

/**
  Write data from the buffer to the EEPROM via GPIO

  \param ccr    Handle to the common HW accessor
  \param offset Destination data offset
  \param buffer Pointer to the source buffer
  \param len    Size of source buffer

  \return
    MTLK_ERR_...
*/
int __MTLK_IFUNC
eeprom_gpio_write (mtlk_ccr_t *ccr, uint32 offset, void const *buffer, int len)
{
  int const alen = 1;
  int ret = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != ccr);
  MTLK_ASSERT(NULL != buffer);

  while (len > 0)
  {
    int data_size = EEPROM_PAGE_SIZE;
    int size = len;

    data_size -= offset & (EEPROM_PAGE_SIZE - 1);

    if (size > data_size)
      size = data_size;

    ret = _page_write(ccr, (EEPROM_CHIP_ID | (((offset) >> 8) & 7)), offset, alen, buffer, size);
    if (MTLK_ERR_OK != ret)
      break;

    len -= size;
    offset += size;
    buffer += size;
  }

  return ret;
}

/**
  Read data from EEPROM via GPIO in to the buffer

  \param ccr    Handle to the common HW accessor
  \param offset Source data offset
  \param buffer Pointer to the destination buffer
  \param len    Size of destination buffer

  \return
    MTLK_ERR_...
*/
int __MTLK_IFUNC
eeprom_gpio_read (mtlk_ccr_t *ccr, uint32 offset, void *buffer, int len)
{
  MTLK_ASSERT(NULL != ccr);
  MTLK_ASSERT(NULL != buffer);

  return _buffer_read(ccr, (EEPROM_CHIP_ID | (((offset) >> 8) & 7)), offset, 1, buffer, len);
}

/**
  Determine the Eeprom size by writing to the eeprom assumed last
  address and verify the data had been written

  \param ccr    Handle to the common HW accessor
  \param size   eeprom size

  \return
    MTLK_ERR_...
*/
static __INLINE int
_size_get (mtlk_ccr_t *ccr, uint32 *size)
{
  uint8  testData, savedData; /* both of same size */
  uint32 eeprom_size, offset;
  uint32 data_size = sizeof(testData);

  /* Test eeprom size */
  for(eeprom_size = MTLK_MAX_EEPROM_SIZE; eeprom_size >= MTLK_MIN_EEPROM_SIZE; eeprom_size >>= 1)
  {
    /* Try to read/write data by last data offset */
    offset = eeprom_size - data_size;

    /* Save eeprom data from tested cell */
    eeprom_gpio_read(ccr, offset, &savedData, data_size);

    /* Test eeprom cell */
    testData = EEPROM_TEST_VALUE;
    eeprom_gpio_write(ccr, offset, &testData, data_size);

    testData = 0;
    eeprom_gpio_read(ccr, offset, &testData, data_size);

    /* Restore eeprom data to tested cell */
    eeprom_gpio_write(ccr, offset, &savedData, data_size);

    if(testData == EEPROM_TEST_VALUE)
    {
      *size = eeprom_size;
      return MTLK_ERR_OK;
    }
  }

  return MTLK_ERR_EEPROM;
}

/**
  Initialize EEPROM after reset or interrupt

  \param ccr    Handle to the common HW accessor
*/
static __INLINE void
_reset (mtlk_ccr_t *ccr)
{
  int i;

  mtlk_ccr_eeprom_override_write(ccr,
    OE_0_SDA_1_SCL_1,
    REG_LOCAL_EEPROM_CLOCK_OUTPUT |
    REG_LOCAL_EEPROM_DATA_OUTPUT |
    REG_LOCAL_EEPROM_OUTPUT_ENABLE );

  I2C_DELAY;

  I2C_SCL(1);
  I2C_SDA(1);
  I2C_TRISTATE;

  for(i = 0; i < 9; i++) {
    I2C_SCL(0);
    I2C_DELAY;
    I2C_DELAY;
    I2C_SCL(1);
    I2C_DELAY;
    I2C_DELAY;
  }

  I2C_STOP(ccr);
  I2C_TRISTATE;
}

/**
  Configure GPIO for access to the EEPROM device

  \param ccr    Handle to the common HW accessor
  \param size   eeprom size

  \return
    MTLK_ERR_...
*/
int __MTLK_IFUNC
eeprom_gpio_init (mtlk_ccr_t *ccr, uint32 *size)
{
  int ret = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != ccr);
  MTLK_ASSERT(NULL != size);

  mtlk_ccr_eeprom_init(ccr);
  mtlk_ccr_eeprom_protect(ccr);

  ret = mtlk_ccr_eeprom_override_write(ccr,
          REG_LOCAL_EEPROM_OVERRIDE |
          REG_LOCAL_EEPROM_CLOCK_OUTPUT_ENABLE ,
          REG_LOCAL_EEPROM_OVERRIDE |
          REG_LOCAL_EEPROM_CLOCK_OUTPUT |
          REG_LOCAL_EEPROM_CLOCK_OUTPUT_ENABLE |
          REG_LOCAL_EEPROM_DATA_OUTPUT |
          REG_LOCAL_EEPROM_OUTPUT_ENABLE);

  if (MTLK_ERR_OK == ret)
  {
    /* at this state the eeprom is owend to SW ,
    clock en = 1 , clock out = 0
    data en = 0, data = 0 */
    mtlk_udelay(EEPROM_BIT_DELAY);
    mtlk_udelay(EEPROM_BIT_DELAY);
    mtlk_udelay(EEPROM_BIT_DELAY);

    _reset(ccr);

    ret = _size_get(ccr, size);
  }

  if (MTLK_ERR_OK != ret)
    mtlk_ccr_eeprom_release(ccr);

  return ret;
}

void __MTLK_IFUNC
eeprom_gpio_clean (mtlk_ccr_t *ccr)
{
  MTLK_ASSERT(NULL != ccr);

  mtlk_ccr_eeprom_release(ccr);
}

