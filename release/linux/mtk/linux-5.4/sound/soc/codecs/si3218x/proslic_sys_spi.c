// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/spinlock.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#include "proslic_sys.h"

/*****************************************************************************************************/

#define PROSLIC_REG_ID         0
#define PROSLIC_REG_RAM_WAIT   4
#define PROSLIC_REG_RAM_HI     5 
#define PROSLIC_REG_RAM_D0     6
#define PROSLIC_REG_RAM_D1     7
#define PROSLIC_REG_RAM_D2     8
#define PROSLIC_REG_RAM_D3     9
#define PROSLIC_REG_RAM_LO     10
#define PROSLIC_REG_WRVSLIC    12
#define PROSLIC_REG_WRVDAA     34
#define PROSLIC_REG_ISDAA      11

#define PROSLIC_CW_BYTE        0
#define PROSLIC_REG_BYTE       1
#define RAM_ADR_HIGH_MASK(ADDR) (((ADDR)>>3)&0xE0)

/* Basic register definitions, regardless of chipset ('4x, '2x, '17x, '26x compatible */

#define PROSLIC_CW_RD          0x60
#define PROSLIC_CW_WR          0x20
#define PROSLIC_CW_BCAST       0x80
#define PROSLIC_BCAST          0xFF

#ifndef PROSLIC_XFER_BUF
#define PROSLIC_XFER_BUF (SILABS_BITS_PER_WORD/8)
#endif

/* Now do some sanity checks */
#if (SILABS_BITS_PER_WORD != 32) && (SILABS_BITS_PER_WORD != 16) && (SILABS_BITS_PER_WORD != 8)
#error "SILABS_BITS_PER_WORD needs to be either 8, 16 or 32"
#endif

#if (((SILABS_BITS_PER_WORD == 16) && (PROSLIC_XFER_BUF != 2) ) \
    || ((SILABS_BITS_PER_WORD == 32) && (PROSLIC_XFER_BUF != 4) ) ) 
#error "SILABS_BITS_PER_WORD & PROSLIC_XFER_BUF size mismatch"
#endif

#if (SILABS_BITS_PER_WORD != 8)
#define SILABS_CW_BYTE   1
#define SILABS_REG_BYTE 0
#else
#define SILABS_CW_BYTE   0
#define SILABS_REG_BYTE 1
#endif

typedef struct 
{
  proslic_dev_t deviceType[SILABS_MAX_CHANNELS];  
  spinlock_t bus_lock;
  uInt8 channel_count; /* In most cases this is the channel count as well */
} proslic_spi_data_t;

typedef struct
{
  /* Placeholder for now */
} proslic_spi_pform_t;

/* Chan address is sent in reverse bit order */
static uInt8 chanAddr[SILABS_MAX_CHAN] =
{
  0x00, 0x10, 
#if (SILABS_MAX_CHAN > 2)
  0x08, 0x18,
#endif
#if (SILABS_MAX_CHAN > 4)
  0x04, 0x14, 
#endif
#if (SILABS_MAX_CHAN > 6)
  0x0C, 0x1C,
#endif
#if (SILABS_MAX_CHAN > 8)
  0x02, 0x12, 
#endif
#if (SILABS_MAX_CHAN > 10)
  0x0A, 0x1A,
#endif
#if (SILABS_MAX_CHAN > 12)
  0x06, 0x16, 
#endif
#if (SILABS_MAX_CHAN > 14)
  0x0E, 0x1E,
#endif
#if (SILABS_MAX_CHAN > 16)
  0x01, 0x11,
#endif
#if (SILABS_MAX_CHAN > 18)
  0x09, 0x19,
#endif
#if (SILABS_MAX_CHAN > 20)
  0x05, 0x15,
#endif
#if (SILABS_MAX_CHAN > 22)
  0x0D, 0x1D,
#endif
#if (SILABS_MAX_CHAN > 24)
  0x03, 0x13,
#endif
#if (SILABS_MAX_CHAN > 26)
  0x0B, 0x1B,
#endif
#if (SILABS_MAX_CHAN > 28)
  0x07,0x17,
#endif
#if (SILABS_MAX_CHAN > 30)
  0x0F, 0x1F
#endif
};

static proslic_spi_data_t   *proslic_data;
static unsigned int proslic_reset_gpio = SILABS_RESET_GPIO;
struct spi_driver *proslic_spi;  
struct spi_device *proslic_spidev;

/*****************************************************************************************************/
uInt8 proslic_read_register_private(void *hCtrl, uInt8 channel, uInt8 regAddr)
{
  u_int8_t data_stream[4];
//  proslic_spi_data_t *spi_data = (proslic_spi_data_t *)hCtrl;
  if (proslic_spidev == NULL)
   	printk ("%s : small spi NULL\n",__FUNCTION__);

  if( unlikely(channel >= SILABS_MAX_CHANNELS) )
  {
    return PROSLIC_SPI_NOK;
  }
  else
  {
    data_stream[PROSLIC_CW_BYTE] = PROSLIC_CW_RD | chanAddr[channel];
  }  

  data_stream[PROSLIC_REG_BYTE] = regAddr;

  data_stream[2] = 0xFF; /* Set the register to a non-zero value, just in case */
#if (PROSLIC_XFER_BUF == 4) || (PROSLIC_XFER_BUF == 2)
  spi_write_then_read(proslic_spidev, data_stream, 2, &data_stream[2], 2); /* In essence, 2 16 bit ops: 1 to send the request, 1 to get the data, needs to be 32 bit aligned. */
  proslic_debug("%s(%d): chan = %u reg = %u data = 0x%02X", __FUNCTION__, __LINE__, channel, regAddr, data_stream[2]);
#else
  {
    int i;
    for(i = 0; i < 2; i++)
    {
      spi_write(proslic_spidev, &data_stream[i], 1);
    }
  }
  spi_read(proslic_spidev, &data_stream[2], 1);
#endif  

  return data_stream[2];
}

/*****************************************************************************************************/

uInt8 proslic_read_register(void *hCtrl, uInt8 channel, uInt8 regAddr)
{
  uInt8 data;
  //unsigned long irqSettings;
  //proslic_spi_data_t *spi_data = (proslic_spi_data_t *)hCtrl;
  //spin_lock_irqsave(&spi_data->bus_lock, irqSettings);
  data = proslic_read_register_private(hCtrl, channel, regAddr);
  //spin_unlock_irqrestore(&spi_data->bus_lock, irqSettings);
  proslic_debug("%s(%d): chan = %u reg = %u data = 0x%02X", __FUNCTION__, __LINE__, channel, regAddr, data);
  return data;
}
EXPORT_SYMBOL(proslic_read_register);


/*****************************************************************************************************/
/* ProSLIC API register write function */
#if PROSLIC_XFER_BUF == 4
#define proslic_write_register_private proslic_write_register
#endif

int proslic_write_register_private(void *hCtrl, uInt8 channel, uInt8 regAddr, uInt8 data)
{
  u_int8_t data_stream[4];
  //struct spi_device *spi_data = (struct spi_device *)hCtrl;

#if PROSLIC_XFER_BUF != 4
  int i;
#endif

  if( unlikely(channel == PROSLIC_BCAST) )
  {
    data_stream[PROSLIC_CW_BYTE] = PROSLIC_CW_BCAST | PROSLIC_CW_WR;
  }
  else if( unlikely(channel >= SILABS_MAX_CHANNELS) )
  {
    return PROSLIC_SPI_NOK;
  }
  else
  {
    data_stream[PROSLIC_CW_BYTE] = PROSLIC_CW_WR | chanAddr[channel];
  }  

  proslic_trace("%s(%d): chan = %u reg = %u data = 0x%02X", __FUNCTION__, __LINE__, channel, regAddr, data);

  data_stream[PROSLIC_REG_BYTE] = regAddr;

  data_stream[2] = data_stream[3] = data;

#if PROSLIC_XFER_BUF == 4
  spi_write(proslic_spidev, data_stream, PROSLIC_XFER_BUF);
#else
#if PROSLIC_XFER_BUF == 2 /* 16 bit mode */
  for(i = 0; i < 4; i+=2)
#else /* Byte by byte */
  for(i = 0; i < 3; i++)
#endif
  {
    spi_write(proslic_spidev, &data_stream[i], PROSLIC_XFER_BUF);
  }
#endif  

  return PROSLIC_SPI_OK;
}
/*****************************************************************************************************/
#if PROSLIC_XFER_BUF != 4
int proslic_write_register(void *hCtrl, uInt8 channel, uInt8 regAddr, uInt8 data)
{
  unsigned long irqSettings;
  int rc;
  //proslic_spi_data_t *spi_data = (proslic_spi_data_t *)hCtrl;
  //spin_lock_irqsave(&spi_data->bus_lock, irqSettings);
  rc  = proslic_write_register_private(hCtrl, channel, regAddr, data);
  //spin_unlock_irqrestore(&spi_data->bus_lock, irqSettings);
  return rc;
}
EXPORT_SYMBOL(proslic_write_register);
#endif

/*****************************************************************************************************/

/* 
 * wait for RAM access
 *
 * return code 0 = OK
 *
 */

static int wait_ram(void *hCtrl, uInt8 channel)
{
  unsigned int count = SILABS_MAX_RAM_WAIT;
  uInt8 data;

  do
  { 
    data = proslic_read_register_private(hCtrl, channel, PROSLIC_REG_RAM_WAIT) & 0x1;
    if(unlikely(data))
    {
     mdelay(5);
    }  
  }while((data) && (count--));

  if(likely(count))
  {
    return 0;
  }
  return -1; /* Timed out */
}

/*****************************************************************************************************/
/* RAM Write wrapper */
int proslic_write_ram(void *hCtrl, uInt8 channel, uInt16 ramAddr, ramData data )
{

  ramData myData = data;
  int rc = 0;
  //unsigned long irqSettings;
  //struct spi_device *spi_data = (struct spi_device *)hCtrl;
  hCtrl = proslic_spidev;

  proslic_trace("%s(%d): chan = %u ram = %u data = 0x%08X", __FUNCTION__, __LINE__, channel, ramAddr, data);
  //spin_lock_irqsave(&spi_data->bus_lock, irqSettings);

  if(wait_ram(hCtrl,channel) != 0)
  {
    return PROSLIC_SPI_NOK;
  }

 
#ifdef SILABS_RAM_BLOCK_ACCESS
  {
    uInt8 ramWriteData[24]; /* This encapsulates the 6 reg writes into 1 block */
    const uInt8 regAddr[6] = {PROSLIC_REG_RAM_HI, PROSLIC_REG_RAM_D0, PROSLIC_REG_RAM_D1, 
                              PROSLIC_REG_RAM_D2, PROSLIC_REG_RAM_D3, PROSLIC_REG_RAM_LO};
    int i;
    uInt8 scratch;

    /* Setup control word & registers for ALL the reg access */
    scratch = chanAddr[channel] | PROSLIC_CW_WR;

    for(i = 0; i < 6; i++)
    {
      ramWriteData[(i<<2)]       = scratch ; 
      ramWriteData[(i<<2)+1]     = regAddr[i];
    }

    /* For the data, we send the same byte twice to keep things 32 bit aligned */
    ramWriteData[2] = ramWriteData[3] = RAM_ADR_HIGH_MASK(ramAddr);

    ramWriteData[6] = ramWriteData[7] = (uInt8)(myData<<3);
    myData = myData >> 5;

    ramWriteData[10] = ramWriteData[11] = (uInt8)(myData & 0xFF);
    myData = myData >> 8;

    ramWriteData[14] = ramWriteData[15] = (uInt8)(myData & 0xFF);
    myData = myData >> 8;

    ramWriteData[18] = ramWriteData[19] = (uInt8)(myData & 0xFF);

    ramWriteData[22] = ramWriteData[23] = (uInt8)(ramAddr& 0xFF);

    spi_write(hCtrl, ramWriteData, 24);
    
  }
#else
  proslic_write_register_private(hCtrl, channel, RAM_ADDR_HI, RAM_ADR_HIGH_MASK(ramAddr));

  proslic_write_register_private(hCtrl, channel, RAM_DATA_B0, ((unsigned char)(myData<<3)));
 
  myData = myData >> 5;

  proslic_write_register_private(hCtrl, channel, RAM_DATA_B1, ((unsigned char)(myData & 0xFF)));

  myData = myData >> 8;
  
  proslic_write_register_private(hCtrl, channel, RAM_DATA_B2, ((unsigned char)(myData & 0xFF)));

  myData = myData >> 8;
  
  proslic_write_register_private(hCtrl, channel, RAM_DATA_B3, ((unsigned char)(myData & 0xFF)));

  proslic_write_register_private(hCtrl, channel, RAM_ADDR_LO, (unsigned char)(ramAddr & 0xFF));
#endif
  if(wait_ram(hCtrl,channel) != 0)
  {
    rc = PROSLIC_SPI_NOK;
  }
  else
  {
    rc = PROSLIC_SPI_OK;
  }
  //spin_unlock_irqrestore(&spi_data->bus_lock, irqSettings);

  return rc;
  
}
EXPORT_SYMBOL(proslic_write_ram);

/*****************************************************************************************************/
ramData proslic_read_ram(void *hCtrl, uInt8 channel, uInt16 ramAddr)
{
  ramData data;
  //unsigned long irqSettings;
  //proslic_spi_data_t *spi_data = (proslic_spi_data_t *)hCtrl;
  hCtrl = proslic_spidev;

  if(wait_ram(hCtrl,channel) != 0)
  {
    return PROSLIC_SPI_NOK;
  }

  //spin_lock_irqsave(&spi_data->bus_lock, irqSettings);
  /* TODO: could combine these 2 writes into 1 spi_write call... */
  proslic_write_register_private(hCtrl, channel, PROSLIC_REG_RAM_HI, RAM_ADR_HIGH_MASK(ramAddr));

  proslic_write_register_private(hCtrl, channel, PROSLIC_REG_RAM_LO, (unsigned char)(ramAddr&0xFF));

  if(wait_ram(hCtrl,channel) != 0)
  {
    return PROSLIC_SPI_NOK;
  }

  data = proslic_read_register_private(hCtrl, channel, PROSLIC_REG_RAM_D3);
  data = data << 8;
  data |= proslic_read_register_private(hCtrl, channel, PROSLIC_REG_RAM_D2);
  data = data << 8;
  data |= proslic_read_register_private(hCtrl, channel, PROSLIC_REG_RAM_D1);
  data = data << 8;
  data |= proslic_read_register_private(hCtrl, channel, PROSLIC_REG_RAM_D0);
  //spin_unlock_irqrestore(&spi_data->bus_lock, irqSettings);
 
  data = data >>3;

  proslic_trace("%s(%d): chan = %u ram = %u data = 0x%08X", __FUNCTION__, __LINE__, channel, ramAddr, data);

  return data;

}
EXPORT_SYMBOL(proslic_read_ram);

/*****************************************************************************************************/
int proslic_reset(void *hCtrl, int in_reset)
{
  proslic_trace("%s(%d): in_reset = %d", __FUNCTION__, __LINE__, in_reset);
  gpio_direction_output(proslic_reset_gpio, (in_reset == 0) );

  return PROSLIC_SPI_OK;
}
EXPORT_SYMBOL(proslic_reset);

/*****************************************************************************************************/

/*
 * Do a simple write/verify test on a given register.  0 = OK.
 */
static int simple_wrv(void *hCtrl, uInt8 channel, uInt8 regAddr)
{
  uInt8 data;

  proslic_write_register(hCtrl, channel, regAddr, 0);
  data = proslic_read_register(hCtrl, channel, regAddr);
  if(unlikely(data != 0) )
  {
    return -1;
  }

  proslic_write_register(hCtrl, channel, regAddr, 0x5A);
  data = proslic_read_register(hCtrl, channel, regAddr);
  if(unlikely(data != 0x5A) )
  {
    return -2;
  }

  return 0;
}

/*****************************************************************************************************/

/* 
 * Determine if this a ProSLIC or DAA or unknown type - NOT fully tested against ALL chipsets, may not
 * properly work with Si3218/9 parts.
 */
static proslic_dev_t proslic_detect_type(void *hCtrl, uInt8 channel)
{
  /* Guess it's a ProSLIC first */
  uInt8 data;

  data = proslic_read_register(hCtrl, channel, PROSLIC_REG_ISDAA);
  proslic_debug("%s(%d): channel = %d data = 0x%0X", __FUNCTION__, __LINE__, channel, data);

  /* For ProSLIC ISDAA = 5, for DAA it is not (it is non-zero and not FF */

  if( unlikely((data != 0xFF) && data ))
  {
    /* Likely a ProSLIC, let's confirm it by doing a few register write/verify operations */
    if( data == 5) 
    {
      if(unlikely(simple_wrv(hCtrl, channel, PROSLIC_REG_WRVSLIC) == 0))
      {
        proslic_debug("%s(%d): channel = %d is_proslic", __FUNCTION__, __LINE__, channel);
        return PROSLIC_IS_PROSLIC;
      }
    }
    else /* Likely a DAA/Si3050 device */
    {
      if(unlikely(simple_wrv(hCtrl, channel, PROSLIC_REG_WRVDAA) == 0))
      {
        proslic_debug("%s(%d): channel = %d is_daa", __FUNCTION__, __LINE__, channel);
        return PROSLIC_IS_DAA;
      }
    }
  }

  proslic_debug("%s(%d): channel = %d is_unknown", __FUNCTION__, __LINE__, channel);
  return PROSLIC_IS_UNKNOWN;
}

/*****************************************************************************************************/
/*  Pull in any device tree parameters */
#ifdef CONFIG_OF
static void proslic_of_probe(struct spi_device *spi)
{
  int len;
  const __be32 *property;
  u8 scratch;

  printk(KERN_INFO "proslic_of_probe()\n");
 
  /* see if the user specified number of channels */
  property = of_get_property( spi->dev.of_node, "channel_count", &len); 

  if(property && (len >= sizeof(__be32)) )
  {
    scratch = be32_to_cpup(property);

    if(( scratch <= SILABS_MAX_CHANNELS ) 
      && (scratch > 0) )
    {
        proslic_channel_count = scratch;
    }
  }

  /* See if the user specified a debug setting */
  property = of_get_property( spi->dev.of_node, "debug_level", &len); 

  if(property && (len >= sizeof(__be32)) )
  {
    scratch = be32_to_cpup(property);

    if(( scratch <= SILABS_DEFAULT_DBG ) 
      && (scratch > 0) )
    {
        proslic_debug_setting = scratch;
  	printk(KERN_INFO "debug_level = %d\n", proslic_debug_setting);
    }
  }

  printk(KERN_INFO "[previous]reset_gpio = %d\n", proslic_reset_gpio);
  proslic_reset_gpio =
	  of_get_named_gpio(spi->dev.of_node, "reset_gpio", 0);

  printk(KERN_INFO "reset_gpio = %d\n", proslic_reset_gpio);
}
#endif

/*****************************************************************************************************/
int proslic_spi_probe(struct spi_device *spi, struct spi_driver *spi_drv)
{
  proslic_spi_pform_t *pform_data;
  unsigned int channel;
  int rc;

  printk(KERN_INFO "PROSLIC module being probed\n");

  proslic_spi = spi_drv;
  pform_data = (proslic_spi_pform_t *) &(spi->dev.platform_data);

#ifdef CONFIG_OF
  proslic_of_probe(spi);
#endif

  rc = gpio_request(proslic_reset_gpio,"proslic_reset");
  if(rc == 0)
  {
    printk(KERN_INFO "PROSLIC GPIO registered OK");
    gpio_export( proslic_reset_gpio, 0);
    proslic_reset(NULL, 0);
  }
  else
  {
    printk(KERN_INFO "PROSLIC GPIO registered FAil!! rc = %d", rc);
    return -ENODEV;
  }

  if(unlikely(!pform_data))
  {
    return -ENODEV;
  }

  proslic_data = kzalloc(sizeof(*proslic_data), GFP_KERNEL);

  if(unlikely(!proslic_data))
  {
    return -ENOMEM;
  }

  spin_lock_init(&(proslic_data->bus_lock));
  proslic_spidev = spi;
#ifdef CONFIG_OF 
  spi->bits_per_word = SILABS_BITS_PER_WORD; 
  spi->max_speed_hz =  SILABS_SPI_RATE;
  spi->mode = SPI_MODE_3;
  if( spi_setup(spi) != 0)
  {
    printk(KERN_ERR PROSLIC_API_HDR "failed to configure spi mode");
    kfree(proslic_data);
    return -EIO;
  }
#endif
  spi_set_drvdata(spi, proslic_data);

  /* Probe to determine the number of DAA's or ProSLIC's present */
  for(channel = 0; channel < proslic_channel_count; channel++)
  {
    proslic_data->deviceType[channel] = proslic_detect_type(proslic_spidev, channel);
    if( proslic_data->deviceType[channel] != PROSLIC_IS_UNKNOWN)
    {
      proslic_data->channel_count++;
    }
  }

  if(proslic_data->channel_count)
  {
    return 0;
  }
  else
  {
    return -ENXIO;
  }
}
EXPORT_SYMBOL(proslic_spi_probe);

/*****************************************************************************************************/
int proslic_spi_remove(struct spi_device *spi)
{
  void *ptr;

  printk(KERN_INFO "ProSLIC module being removed");
  /* Just put the device(s) into reset - assumes 1 reset per SPI device */
  proslic_reset(NULL, 1);

  ptr = spi_get_drvdata(spi);
  if(ptr != NULL)
  {
    kfree(ptr);
  }

  return 0;
}
EXPORT_SYMBOL(proslic_spi_remove);

/*****************************************************************************************************/
int proslic_spi_setup()
{
  int rc;
  printk(KERN_INFO "proslic_spi_setup()\n");

  rc = spi_register_driver(proslic_spi);

  if(rc != 0)
  {
    proslic_error("%s(%d): spi_register driver returned = %d", __FUNCTION__, __LINE__, rc);
  }
  else
  {
    proslic_debug("%s(%d): spi driver registered", __FUNCTION__, __LINE__);
  }

  proslic_trace("%s(%d): completed", __FUNCTION__, __LINE__);
  return rc;
}


void proslic_spi_shutdown()
{
  spi_unregister_driver(proslic_spi);
  gpio_unexport( proslic_reset_gpio );
  gpio_free( proslic_reset_gpio );
}
/*****************************************************************************************************/
int proslic_get_channel_count()
{
  return proslic_data->channel_count;
}

proslic_dev_t proslic_get_device_type(uint8_t channel_number)
{
  if(channel_number < proslic_data->channel_count)
  {
    return proslic_data->deviceType[channel_number];
  }
  else
  {
    return PROSLIC_IS_UNKNOWN;
  }
}

void *proslic_get_hCtrl(uint8_t channel)
{
  if(channel < proslic_data->channel_count)
  {
    return proslic_data;
  }
  else
  {
    return NULL;
  }
}

/*****************************************************************************************************/
proslic_spi_fptrs_t proslic_spi_if = 
{
  proslic_reset, 

  proslic_write_register,
  proslic_write_ram,

  proslic_read_register,
  proslic_read_ram,

  NULL, /* semaphore */

};
