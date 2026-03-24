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
 *  Unit testing for EEPROM data access via IRB interface.
 */
#include "mtlkinc.h"

#include <sys/stat.h>
#include <fcntl.h>

#include "mtlkirba.h"
#include "dataex.h"
#include "mhi_dut.h"
#include "driver_api.h"

#define LOG_LOCAL_GID   GID_DUT_SRV_UTEST
#define LOG_LOCAL_FID   0

#ifdef MTLK_DEBUG

#include "dutserver.h"
#include "dut_utest_nv.h"

/** Template of EEPROM calibration file */
#define FLASH_FILE_TEMPLATE     "/tmp/cal_wlan%u.bin"

/**
  EEPROM data flash request simulation

  \param hw_idx   WAVE hardware index [I]
  \param storage  EEPROM data storage type \ref dutNvMemoryType_e [I]

  \return
    MTLK_ERR_OK on success or error code on failure
*/
static int
__nvm_flush(int hw_idx, int storage)
{
  dutMessage_t msg;
  dutNvMemoryFlushParams_t *param = (dutNvMemoryFlushParams_t *)msg.data;
  int res;

  if (!dut_api_is_connected_to_hw(hw_idx))
    return MTLK_ERR_NOT_SUPPORTED;

  memset(&msg, 0, sizeof(msg));
  msg.msgId = HOST_TO_DUT16(DUT_DGM_FLUSH_NV_MEMORY_REQ);
  msg.msgLength = HOST_TO_DUT16(sizeof(*param));

  param->nvMemoryType = HOST_TO_DUT32(storage);
  param->verifyNvmData = 1;

  res = dut_api_send_drv_command((char *)&msg, sizeof(msg), hw_idx);
  if (res != MTLK_ERR_OK)
  {
    ELOG_D("DUT: Failed to send FLUSH_NV_MEMORY, error %d", res);
    return res;
  }

  if (DUT_DGM_FLUSH_NV_MEMORY_CFM != DUT_TO_HOST16(msg.msgId))
  {
    ELOG_V("DUT: FLUSH_NV_MEMORY is not confirmed");
    return MTLK_ERR_UNKNOWN;
  }

  if (DUT_STATUS_PASS != DUT_TO_HOST16(msg.status))
  {
    ELOG_D("DUT: Failed FLUSH_NV_MEMORY execution, status %d",
           DUT_TO_HOST16(msg.status));
    return MTLK_ERR_UNKNOWN;
  }

  return MTLK_ERR_OK;
}

/**
  EEPROM data size determination

  \param hw_idx   WAVE hardware index [I]
  \param storage  EEPROM data storage type \ref dutNvMemoryType_e [I]

  \return
    size of EEPROM data in bytes on success or zero on failure
*/
static int
__nvm_size(int hw_idx, int storage)
{
  dutMessage_t msg;
  dutNvMemoryAccessParams_t *param = (dutNvMemoryAccessParams_t *)msg.data;
  int res;

  if (!dut_api_is_connected_to_hw(hw_idx))
    return 0;

  memset(&msg, 0, sizeof(msg));
  msg.msgId = HOST_TO_DUT16(DUT_DGM_READ_NV_MEMORY_REQ);
  msg.msgLength = HOST_TO_DUT16(sizeof(*param));

  param->nvMemoryType = HOST_TO_DUT32(storage);
  param->address = HOST_TO_DUT32(0);
  param->length = HOST_TO_DUT32(1);

  res = dut_api_send_drv_command((char *)&msg, sizeof(msg), hw_idx);
  if (res != MTLK_ERR_OK)
  {
    ELOG_D("DUT: Failed to send READ_NV_MEMORY, error %d", res);
    return 0;
  }

  if (DUT_DGM_READ_NV_MEMORY_CFM!= DUT_TO_HOST16(msg.msgId))
  {
    ELOG_V("DUT: READ_NV_MEMORY is not confirmed");
    return 0;
  }

  if (DUT_STATUS_PASS != DUT_TO_HOST16(msg.status))
  {
    ELOG_D("DUT: Failed READ_NV_MEMORY execution, status %d",
           DUT_TO_HOST16(msg.status));
    return 0;
  }

  ILOG0_D("DUT: EEPROM data size: %u", DUT_TO_HOST32(param->eepromSize));

  return DUT_TO_HOST32(param->eepromSize);
}

#if 1
#define NVM_RW_CHUNK_SIZE(data) sizeof(data)
#else
#define NVM_RW_CHUNK_SIZE(data) 77
#endif
/**
  EEPROM data read request simulation

  \param hw_idx   WAVE hardware index [I]
  \param storage  EEPROM data storage type \ref dutNvMemoryType_e [I]
  \param offset   Start address of EEPROM data on storage [I]
  \param buffer   Handle to the output data [O]
  \param length   Length of data to be read [O]

  \return
    count of bytes read from EEPROM storage on success or negative value on failure
*/
static int
__nvm_read(int hw_idx, int storage, int offset, uint8 *buffer, uint32 length)
{
  dutMessage_t msg;
  dutNvMemoryAccessParams_t *param = (dutNvMemoryAccessParams_t *)msg.data;
  int res;
  uint8 *p = buffer;

  if (!dut_api_is_connected_to_hw(hw_idx))
    return -1;

  while (length > 0)
  {
    uint32 read_len;

    memset(&msg, 0, sizeof(msg));
    msg.msgId = HOST_TO_DUT16(DUT_DGM_READ_NV_MEMORY_REQ);
    msg.msgLength = HOST_TO_DUT16(sizeof(*param));

    read_len = MIN(length, NVM_RW_CHUNK_SIZE(param->data));

    param->nvMemoryType = HOST_TO_DUT32(storage);
    param->address = HOST_TO_DUT32(offset);
    param->length = HOST_TO_DUT32(read_len);

    res = dut_api_send_drv_command((char *)&msg, sizeof(msg), hw_idx);
    if (res != MTLK_ERR_OK)
    {
      ELOG_D("DUT: Failed to send READ_NV_MEMORY, error %d", res);
      return -1;
    }

    if (DUT_DGM_READ_NV_MEMORY_CFM != DUT_TO_HOST16(msg.msgId))
    {
      ELOG_V("DUT: READ_NV_MEMORY is not confirmed");
      return -1;
    }

    if (DUT_STATUS_PASS != DUT_TO_HOST16(msg.status))
    {
      ELOG_D("DUT: Failed READ_NV_MEMORY execution, status %d",
             DUT_TO_HOST16(msg.status));
      return -1;
    }

    memcpy(p, param->data, read_len);

    p += read_len;
    length -= read_len;
    offset += read_len;
  }

  return (p - buffer);
}

/**
  EEPROM data write request simulation

  \param hw_idx   WAVE hardware index [I]
  \param storage  EEPROM data storage type \ref dutNvMemoryType_e [I]
  \param offset   Start address of EEPROM data on storage [I]
  \param buffer   Handle to the source data [O]
  \param length   Length of data to be write [O]

  \return
    count of bytes writtent to the EEPROM storage on success or negative value on failure
*/
static int
__nvm_write(int hw_idx, int storage, int offset, uint8 *buffer, uint32 length)
{
  dutMessage_t msg;
  dutNvMemoryAccessParams_t *param = (dutNvMemoryAccessParams_t *)msg.data;
  int res;
  uint8 *p = buffer;

  if (!dut_api_is_connected_to_hw(hw_idx))
    return -1;

  while (length > 0)
  {
    uint32 write_len;

    memset(&msg, 0, sizeof(msg));
    msg.msgId = HOST_TO_DUT16(DUT_DGM_WRITE_NV_MEMORY_REQ);
    msg.msgLength = HOST_TO_DUT16(sizeof(*param));

    write_len = MIN(length, NVM_RW_CHUNK_SIZE(param->data));

    param->nvMemoryType = HOST_TO_DUT32(storage);
    param->address = HOST_TO_DUT32(offset);
    param->length = HOST_TO_DUT32(write_len);

    memcpy(param->data, p, write_len);

    res = dut_api_send_drv_command((char *)&msg, sizeof(msg), hw_idx);
    if (res != MTLK_ERR_OK)
    {
      ELOG_D("DUT: Failed to send WRITE_NV_MEMORY, error %d", res);
      return -1;
    }

    if (DUT_DGM_WRITE_NV_MEMORY_CFM != DUT_TO_HOST16(msg.msgId))
    {
      ELOG_V("DUT: WRITE_NV_MEMORY is not confirmed");
      return -1;
    }

    if (DUT_STATUS_PASS != DUT_TO_HOST16(msg.status))
    {
      ELOG_D("DUT: Failed WRITE_NV_MEMORY execution, status %d",
             DUT_TO_HOST16(msg.status));
      return -1;
    }

    p += write_len;
    length -= write_len;
    offset += write_len;
  }

  return (p - buffer);
}

/**
  Memory dump in to the console

  \param buf  Handle to the memory [I]
  \param l    Count of bytes to dump [I]
*/
static void
__DUMP (const void *buf, unsigned int l)
{
#define LOG_BUFFER             printf

  unsigned int i,j;
  unsigned char *cp = (unsigned char*)buf;
  LOG_BUFFER("cp= 0x%p l=%d\n", cp, l);
  for (i = 0; i < l/16; i++) {
    LOG_BUFFER("%04x:  ", 16*i);
    for (j = 0; j < 16; j++)
      LOG_BUFFER("%02x %s", *cp++, j== 7 ? " " : "");
    LOG_BUFFER("\n");
  }
  if (l & 0x0f) {
    LOG_BUFFER("%04x:  ", 16*i);
    for (j = 0; j < (l&0x0f); j++)
      LOG_BUFFER("%02x %s", *cp++, j== 7 ? " " : "");
    LOG_BUFFER("\n");
  }
}

/**
  Safely prepare EEPROM file with random data

  \param hw_idx   Hardware index used for [I]
  \param size     Count of random bytes to be written [I]

  \note
    Original file is backed upped if exists.
    Do not overwrite backup file.
*/
static void
_flash_file_prepare (int32 hw_idx, uint32 size)
{
  char f_name_orig[0x20];
  char f_name_bak[0x20];
  struct stat st;

  snprintf(f_name_orig, sizeof(f_name_orig), FLASH_FILE_TEMPLATE, hw_idx);
  snprintf(f_name_bak, sizeof(f_name_bak), "%s.dut.bak", f_name_orig);

  ILOG0_SD("Create dummy EEPROM data file '%s' (size: %d)", f_name_orig, size);

  if (0 == stat(f_name_orig, &st))
  {
    if (0 == stat(f_name_bak, &st))
    {
      ILOG0_S("Backup file '%s' already exists", f_name_bak);
    }
    else if (0 != rename(f_name_orig, f_name_bak))
    {
      ELOG_SSS("Can not backup file '%s' as  '%s' [%s]",
              f_name_orig, f_name_bak, strerror(errno));
      return;
    }
  }

  if (size > 0)
  {
    int fd = -1;

    srand(mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp()));

    fd = open(f_name_orig, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (-1 == fd)
    {
      ELOG_SS("Can not create eeprom file '%s' [%s]",
              f_name_orig, strerror(errno));
      return;
    }
    else
    {
      uint8 b[64], i;

      while(size > 0)
      {
        int to_write = sizeof(b);

        if (size < to_write)
          to_write = size;

        for (i = 0; i < to_write; i++)
          b[i] = rand() % 0xFF;

        write(fd, b, to_write);

        size -= to_write;
      }

      if (-1 == close(fd))
      {
        ELOG_SS("Can not close eeprom file '%s' [%s]",
                f_name_orig, strerror(errno));
        return;
      }
    }
  }
}

/**
  Restore EEPROM file

  \param hw_idx   Hardware index used for [I]
*/
static void
_flash_file_restore (int32 hw_idx)
{
  char f_name_orig[0x20];
  char f_name_bak[0x20];
  struct stat st;

  /* wait for helper completion, can save the file with some delays */
  sleep (3);

  snprintf(f_name_orig, sizeof(f_name_orig), FLASH_FILE_TEMPLATE, hw_idx);
  snprintf(f_name_bak, sizeof(f_name_bak), "%s.dut.bak", f_name_orig);

  (void)remove(f_name_orig);

  if (0 == stat(f_name_bak, &st))
  {
    if (0 != rename(f_name_bak, f_name_orig))
    {
      ELOG_SSS("Can not restore file '%s' as  '%s' [%s]",
              f_name_bak, f_name_orig, strerror(errno));
      return;
    }
  }

  (void)remove(f_name_bak);
}

/**
  Compare calibration file with buffer

  \param hw_idx   Hardware index used for [I]

  \return
    integer less than, equal to, or greater than zero if the file data
    (or the first n bytes thereof) is found, respectively, to be less than,
    to match, or be greater than buf
*/
static int
_flash_file_cmp(int32 hw_idx, uint8 *buf, uint32 size)
{
  int ret;
  int fd;
  char f_name[0x20];
  struct stat st;
  uint32 offset = 0;
  int b[0x10];

  snprintf(f_name, sizeof(f_name), FLASH_FILE_TEMPLATE, hw_idx);

  if (0 != stat(f_name, &st))
  {
    ELOG_S("Missed eeprom file '%s'", f_name);
    return (0 - (int)size);
  }

  ret = ((int)st.st_size - (int)size);
  if (0 != ret)
  {
    ELOG_SDD("Wrong size of eeprom file '%s' [%d:%d]", f_name, st.st_size, size);
    return ret;
  }

  fd = open(f_name, O_RDONLY);
  if (-1 == fd)
  {
    ELOG_SS("Can not open eeprom file '%s' [%s]",
            f_name, strerror(errno));
    return -1;
  }

  while (size > 0)
  {
    uint32 to_read = sizeof(b);

    if (size < to_read)
      to_read = size;

    ret = read(fd, b, to_read);
    if (ret != to_read)
    {
      ELOG_SDD("Wrong read for eeprom file '%s' [%d:%d]", f_name, ret, to_read);
      ret = (ret - (int)to_read);
      goto Finish;
    }

    ret = memcmp(b, buf + offset, to_read);
    if (0 != ret)
    {
      ELOG_D("Data mismatch at %u", offset);
      __DUMP(b, to_read);
      __DUMP(buf + offset, to_read);
      goto Finish;
    }

    offset += to_read;
    size -= to_read;
  }
  ret = 0;

Finish:
  if (-1 == close(fd))
  {
    ELOG_SS("Can not close eeprom file '%s' [%s]",
            f_name, strerror(errno));
    return -1;
  }

  return ret;
}

/**
  Test for EEPROM data file creation on reset request

  \param storage  EEPROM data storage type [I]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_create_on_reset(int hw_idx)
{
  uint32 const EEPROM_DATA_SIZE = 137 /* prime number */;
  uint8 data[DUT_MSG_HEADER_LENGTH + sizeof(dutResetParams_t)];
  dutResetParams_t *param = (dutResetParams_t*)(data + DUT_MSG_HEADER_LENGTH);
  uint8 empty_buffer[EEPROM_DATA_SIZE];
  int ret = MTLK_ERR_UNKNOWN;

  ILOG1_DS("DUT%d: %s", hw_idx, __FUNCTION__);

  (void)dut_api_driver_stop(TRUE);

  memset(empty_buffer, 0, sizeof(empty_buffer));

  /* remove file */
  _flash_file_prepare(hw_idx, 0);

  /*** STEP 1 - SUCCESS ***/
  /* create new empty file */
  memset(data, 0, sizeof(data));

  data[0] = 'M';
  data[1] = 'T';
  /* data[2] = not used */
  data[3]   = hw_idx & 0xF;
  data[3] <<= 4;
  data[3]  |= DUT_SERVER_MSG_RESET_MAC & 0xF;

  *((uint32*)(data + 4)) = HOST_TO_DUT32(sizeof(dutResetParams_t));

  param->nvMemoryType = HOST_TO_DUT32(DUT_NV_MEMORY_FLASH);
  param->eepromSize = HOST_TO_DUT32(EEPROM_DATA_SIZE);

  dut_hostif_handle_packet((char*)data, sizeof(data));

  /* validate file */
  if (0 != _flash_file_cmp(hw_idx, empty_buffer, sizeof(empty_buffer)))
  {
    ELOG_D("DUT%d test failed: data mismatched", hw_idx);
    goto out;
  }

  /*** STEP 1 - FAIL ***/
  /* try to create another new empty file */
  memset(data, 0, sizeof(data));

  data[0] = 'M';
  data[1] = 'T';
  /* data[2] = not used */
  data[3]   = hw_idx & 0xF;
  data[3] <<= 4;
  data[3]  |= DUT_SERVER_MSG_RESET_MAC & 0xF;

  *((uint32*)(data + 4)) = HOST_TO_DUT32(sizeof(dutResetParams_t));

  param->nvMemoryType = HOST_TO_DUT32(DUT_NV_MEMORY_FLASH);
  /* new size */
  param->eepromSize = HOST_TO_DUT32(EEPROM_DATA_SIZE * 2);

  dut_hostif_handle_packet((char*)data, sizeof(data));

  /* validate file, the file should be the same, no any recreation */
  if (0 != _flash_file_cmp(hw_idx, empty_buffer, sizeof(empty_buffer)))
  {
    ELOG_D("DUT%d test failed: data mismatched", hw_idx);
    goto out;
  }

  ret = MTLK_ERR_OK;

out:

  _flash_file_restore(hw_idx);

  if (!dut_api_is_connected_to_hw(hw_idx))
    (void)dut_api_driver_start(TRUE);

  return ret;
}

/**
  Test for read operations with different block sizes

  \param hw_idx   Hardware index used for [I]
  \param storage  EEPROM data storage type [I]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_read_cmp (int hw_idx, int storage)
{
  typedef struct {
    void *data;
    uint32 data_size;
  } __acc_t;

  int ret = MTLK_ERR_OK;

  __acc_t acc_vector[] = {
    {NULL, 0},
    {NULL, 0},
    {NULL, 0}
  };

  int const acc_count = ARRAY_SIZE(acc_vector);
  int acc_idx = 0;
  __acc_t *acc_pattern = NULL;

  ILOG1_DSD("DUT%d: %s (%d)", hw_idx, __FUNCTION__, acc_count);

  /* read data */
  for (acc_idx = 0; acc_idx < acc_count; acc_idx++)
  {
    int size;
    __acc_t *acc = acc_vector + acc_idx;

    if (NULL == acc_pattern)
      acc_pattern = acc;

    acc->data_size = __nvm_size(hw_idx, storage);
    if (0 == acc->data_size)
    {
      WLOG_DD("DUT%d.%d: EEPROM data missed", hw_idx, acc_idx);
      goto close;
    }

    acc->data = mtlk_osal_mem_alloc(acc->data_size, MTLK_MEM_TAG_EEPROM);
    if (NULL == acc->data)
    {
      ELOG_DD("DUT%d.%d test failed: can't allocate memory", hw_idx, acc_idx);
      ret = MTLK_ERR_NO_MEM;
      goto close;
    }

    memset(acc->data, 0, acc->data_size);

    size = __nvm_read(hw_idx, storage, 0, acc->data, acc->data_size);
    if (size != acc->data_size)
    {
      ELOG_DDDD("DUT%d.%d test failed: data reading error [%d:%d]", \
              hw_idx, acc_idx, size, acc->data_size);
      ret = MTLK_ERR_EEPROM;
      goto close;
    }
  }

  /* validate */
  for (acc_idx = 0; acc_idx < acc_count; acc_idx++)
  {
    __acc_t *acc = acc_vector + acc_idx;

    if (acc == acc_pattern)
      continue;

    if (acc_pattern->data_size != acc->data_size)
    {
      ELOG_DDDD("DUT%d.%d test failed: wrong amount of data read [%d:%d]",
              hw_idx, acc_idx, acc_pattern->data_size, acc->data_size);
      ret = MTLK_ERR_UNKNOWN;
      goto close;
    }

    if (0 != memcmp(acc_pattern->data, acc->data, acc->data_size))
    {
      ELOG_DD("DUT%d.%d test failed: data mismatched", hw_idx, acc_idx);
      __DUMP(acc_pattern->data, acc->data_size);
      __DUMP(acc->data, acc->data_size);
      ret = MTLK_ERR_UNKNOWN;
    }
  }

close:
  if (DUT_NV_MEMORY_FLASH == storage)
  {
    if (0 != _flash_file_cmp(hw_idx, acc_pattern->data, acc_pattern->data_size))
    {
      ELOG_D("DUT%d test failed: data mismatched", hw_idx);
      ret = MTLK_ERR_UNKNOWN;
    }
  }

  /* release data */
  for (acc_idx = 0; acc_idx < acc_count; acc_idx++)
  {
    __acc_t *acc = acc_vector + acc_idx;

    if (NULL == acc->data)
      continue;

    mtlk_osal_mem_free(acc->data);
    acc->data = NULL;
  }

  return ret;
}

/**
  Test for write operations

  \param hw_idx   Hardware index used for [I]
  \param storage  EEPROM data storage type [I]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_write_cmp (int hw_idx, int storage)
{
  int ret = MTLK_ERR_OK;
  uint8 *bak = NULL, *pattern = NULL, *data = NULL;
  uint32 data_size = 0;
  int i, size;

  ILOG1_DS("DUT%d: %s", hw_idx, __FUNCTION__);

  data_size = __nvm_size(hw_idx, storage);
  if (0 == data_size)
    return MTLK_ERR_OK;

  bak = mtlk_osal_mem_alloc(data_size, MTLK_MEM_TAG_EEPROM);
  if (NULL == bak)
    goto close;

  pattern = mtlk_osal_mem_alloc(data_size, MTLK_MEM_TAG_EEPROM);
  if (NULL == pattern)
    goto close;

  data = mtlk_osal_mem_alloc(data_size, MTLK_MEM_TAG_EEPROM);
  if (NULL == data)
    goto close;

  memset(bak, 0, sizeof(*bak));
  memset(pattern, 0, sizeof(*pattern));
  memset(data, 0, sizeof(*data));

  /* read original */
  size = __nvm_read(hw_idx, storage, 0, bak, data_size);
  if (size != data_size)
  {
    ELOG_DD("DUT%d test failed: data reading error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
    goto close;
  }

  /* generate pseudo-random data */
  srand(mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp()));
  for (i = 0; i < data_size; i++)
    pattern[i] = (uint8)(rand() % 0xFF);

  /* write random data */
  size = __nvm_write(hw_idx, storage, 0, pattern, data_size);
  if (size != data_size)
  {
    ELOG_DD("DUT%d test failed: data writing error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
  }

  /* read random data */
  size = __nvm_read(hw_idx, storage, 0, data, data_size);
  if (size != data_size)
  {
    ELOG_DD("DUT%d test failed: data reading error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
  }
  else
  {
    /* validate random data */
    if (0 != memcmp(pattern, data, data_size))
    {
      ELOG_D("DUT%d test failed: data mismatched", hw_idx);
      __DUMP(pattern, data_size);
      __DUMP(data, data_size);
      ret = MTLK_ERR_EEPROM;
    }
  }

  /* restore original data */
  size = __nvm_write(hw_idx, storage, 0, bak, data_size);
  if (size != data_size)
  {
    ELOG_DD("DUT%d test failed: data restore error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
  }

  /* read restored data */
  size = __nvm_read(hw_idx, storage, 0, data, data_size);
  if (size != data_size)
  {
    ELOG_DD("DUT%d test failed: data reading error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
    goto close;
  }

  /* validate original */
  if (0 != memcmp(bak, data, data_size))
  {
    ELOG_D("DUT%d test failed: data was not restored", hw_idx);
    __DUMP(bak, data_size);
    __DUMP(data, data_size);
    ret = MTLK_ERR_EEPROM;
  }

close:
  (void) __nvm_flush(hw_idx, storage);

  if (NULL != bak)
    mtlk_osal_mem_free(bak);

  if (NULL != pattern)
    mtlk_osal_mem_free(pattern);

  if (NULL != data)
    mtlk_osal_mem_free(data);

  return ret;
}

/**
  Test borders for write operations

  \param hw_idx   Hardware index used for [I]
  \param storage  EEPROM data storage type [I]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_write_border_cmp (int hw_idx, int storage)
{
  int ret = MTLK_ERR_OK;

  uint32 const block_size = 32;
  uint32 const pattern_size = block_size / 3;

  uint8 bak[block_size];
  uint8 pattern[pattern_size];
  uint8 data[block_size];

  int i, size, offset = 0;

  ILOG1_DS("DUT%d: %s", hw_idx, __FUNCTION__);

  memset(bak, 0, sizeof(bak));

  /* read original */
  size = __nvm_read(hw_idx, storage, offset, bak, block_size);
  if (size != block_size)
  {
    ELOG_DD("DUT%d test failed: data reading error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
    goto close;
  }

  /* generate pseudo-random data */
  srand(mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp()));
  for (i = 0; i < pattern_size; i++)
    pattern[i] = (uint8)(rand() % 0xFF);

  /* write random data */
  size = __nvm_write(hw_idx, storage, offset + pattern_size, pattern, pattern_size);
  if (size != pattern_size)
  {
    ELOG_DD("DUT%d test failed: data writing error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
  }
  else
  {
    (void) __nvm_flush(hw_idx, storage);
  }

  /* read random data */
  size = __nvm_read(hw_idx, storage, offset + pattern_size, data, pattern_size);
  if (size != pattern_size)
  {
    ELOG_DD("DUT%d test failed: data reading error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
  }
  else
  {
    /* validate random data */
    if (0 != memcmp(pattern, data, pattern_size))
    {
      ELOG_D("DUT%d test failed: data mismatched", hw_idx);
      __DUMP(pattern, pattern_size);
      __DUMP(data, pattern_size);
      ret = MTLK_ERR_EEPROM;
    }
  }

  /* restore original data */
  size = __nvm_write(hw_idx, storage, offset + pattern_size, bak + pattern_size, pattern_size);
  if (size != pattern_size)
  {
    ELOG_DD("DUT%d test failed: data restore error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
    goto close;
  }

  (void) __nvm_flush(hw_idx, storage);

  /* read restored data */
  size = __nvm_read(hw_idx, storage, offset, data, block_size);
  if (size != block_size)
  {
    ELOG_DD("DUT%d test failed: data reading error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
    goto close;
  }

  /* validate original */
  if (0 != memcmp(bak, data, block_size))
  {
    ELOG_D("DUT%d test failed: data was not restored", hw_idx);
    __DUMP(bak, block_size);
    __DUMP(data, block_size);
    ret = MTLK_ERR_EEPROM;
  }

close:

  return ret;
}

/**
  Test for flush operations during rmmod
  (write was done, flush was not)

  \param hw_idx   Hardware index used for [I]
  \param storage  EEPROM data storage type [I]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_flush_on_rmmod (int hw_idx, int storage)
{
  int ret = MTLK_ERR_OK;
  uint8 *bak = NULL, *pattern = NULL;
  uint32 data_size = 0;
  int i, size;

  ILOG1_DS("DUT%d: %s", hw_idx, __FUNCTION__);

  if (DUT_NV_MEMORY_FLASH != storage)
    return MTLK_ERR_OK;

  data_size = __nvm_size(hw_idx, storage);
  if (0 == data_size)
  {
    ELOG_DD("DUT%d test skipped: ignore storage (%d)",
            hw_idx, storage);
    return MTLK_ERR_OK;
  }

  bak = mtlk_osal_mem_alloc(data_size, MTLK_MEM_TAG_EEPROM);
  if (NULL == bak)
  {
    ELOG_DD("DUT%d test failed: can not allocate memory (%d)",
            hw_idx, data_size);
    ret = MTLK_ERR_NO_MEM;
    goto close;
  }

  pattern = mtlk_osal_mem_alloc(data_size, MTLK_MEM_TAG_EEPROM);
  if (NULL == pattern)
  {
    ELOG_DD("DUT%d test failed: can not allocate memory (%d)",
            hw_idx, data_size);
    ret = MTLK_ERR_NO_MEM;
    goto close;
  }

  memset(bak, 0, sizeof(*bak));
  memset(pattern, 0, sizeof(*pattern));

  /* read original */
  size = __nvm_read(hw_idx, storage, 0, bak, data_size);
  if (size != data_size)
  {
    ELOG_DD("DUT%d test failed: data reading error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
    goto close;
  }

  /* generate pseudo-random data */
  srand(mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp()));
  for (i = 0; i < data_size; i++)
    pattern[i] = (uint8)(rand() % 0xFF);

  /* write random data */
  size = __nvm_write(hw_idx, storage, 0, pattern, data_size);
  if (size != data_size)
  {
    ELOG_DD("DUT%d test failed: data writing error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
  }
  else
  {
#if 0 /* in force skip the flush operation */
    (void) __nvm_flush(hw_idx, storage);
#endif /* 0 */

#if 0
    (void)dut_api_driver_stop(TRUE);
#else
    if (0 != system("rmmod mtlk"))
    {
      ELOG_DS("DUT%d test failed: failed to unload driver (%s)",
              hw_idx, strerror(errno));
      ret = MTLK_ERR_EEPROM;
    }
#endif

    sleep(3);

    /* validate random data */
    if (0 != _flash_file_cmp(hw_idx, pattern, data_size))
    {
      ELOG_D("DUT%d test failed: data mismatched", hw_idx);
      ret = MTLK_ERR_EEPROM;
    }

    sleep(2);

    /* reload */
    (void)dut_api_driver_stop(TRUE);
    (void)dut_api_driver_start(TRUE);
  }

  /* restore original data */
  size = __nvm_write(hw_idx, storage, 0, bak, data_size);
  if (size != data_size)
  {
    ELOG_DD("DUT%d test failed: data restore error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
  }
  else
  {
    (void) __nvm_flush(hw_idx, storage);
  }

  /* read restored data */
  size = __nvm_read(hw_idx, storage, 0, pattern, data_size);
  if (size != data_size)
  {
    ELOG_DD("DUT%d test failed: data reading error (%d)", hw_idx, size);
    ret = MTLK_ERR_EEPROM;
    goto close;
  }

  /* validate original */
  if (0 != memcmp(bak, pattern, data_size))
  {
    ELOG_D("DUT%d test failed: data was not restored", hw_idx);
    __DUMP(bak, data_size);
    __DUMP(pattern, data_size);
    ret = MTLK_ERR_EEPROM;
  }

close:
  if (NULL != bak)
    mtlk_osal_mem_free(bak);

  if (NULL != pattern)
    mtlk_osal_mem_free(pattern);

  return ret;
}


/**
  Test for error cases during read and write operations

  \param hw_idx   Hardware index used for [I]
  \param storage  EEPROM data storage type [I]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_rw_error (int hw_idx, int storage)
{
  int ret = MTLK_ERR_OK;
  uint8 buf[5];

  int32 offset_vector[] = {
    -100,
    -10,
    -1,
    0,
    1,
    10,
    100,
  };

  int offset_count = ARRAY_SIZE(offset_vector);

  int i;

  ILOG1_DSD("DUT%d: %s (%d)", hw_idx, __FUNCTION__, offset_count);

  for (i = 0; (MTLK_ERR_OK == ret) && (i < offset_count); i++)
  {
    uint32 offset = (uint32)offset_vector[i];
    int res = 0;

    if (offset_vector[i] >= 0)
      offset += __nvm_size(hw_idx, storage);

    res = __nvm_read(hw_idx, storage, offset, buf, sizeof(buf));
    if ((-1 != res) && ((0 != res) || (0 != offset_vector[i])))
    {
      ELOG_DD("DUT%d.%d test failed: read done with wrong arguments", hw_idx, i);
      ret = MTLK_ERR_UNKNOWN;
    }

    res = __nvm_write(hw_idx, storage, offset, buf, sizeof(buf));
    if (-1 != res)
    {
      ELOG_DDD("DUT%d.%d test failed: write done with wrong arguments (%d)",
               hw_idx, i, offset_vector[i]);
      ret = MTLK_ERR_UNKNOWN;
    }
  }

  /* check that nothing broken */
  if (-1 == __nvm_read(hw_idx, storage, 0, buf, sizeof(buf)))
  {
    ELOG_D("DUT%d test failed: can't read data", hw_idx);
    ret = MTLK_ERR_UNKNOWN;
  }

  /* check that nothing broken */
  if (-1 == __nvm_write(hw_idx, storage, 0, buf, sizeof(buf)))
  {
    ELOG_D("DUT%d test failed: can't write data", hw_idx);
    ret = MTLK_ERR_UNKNOWN;
  }

  (void)__nvm_flush(hw_idx, storage);

  return ret;
}

/**
  Unit testing for EEPROM API
*/
void __MTLK_IFUNC
dut_utest_nv (void)
{
  int failed = 0, warnings = 0;
  int res;
  int (**do_test)(int hw_idx, int storage);
  int (*test_vector[])(int hw_idx, int storage) = {
    _flush_on_rmmod,
    _rw_error,
    _read_cmp,
    _write_cmp,
    _write_border_cmp,
    NULL
  };
  int const storage_vector[] = {
    DUT_NV_MEMORY_EEPROM,
    DUT_NV_MEMORY_FLASH,
#if 0 /* \todo: pending implementation */
    DUT_NV_MEMORY_EFUSE,
#endif /* 0 */
  };
  int storage_idx;
  int const storage_count = ARRAY_SIZE(storage_vector);
  int hw_idx = 0;

  res = dut_api_driver_start(TRUE);
  if (MTLK_ERR_OK != res)
  {
    ELOG_S("DUT: Failed to start driver api '%s'", mtlk_get_error_text(res));
    return;
  }


  while (dut_api_is_connected_to_hw(hw_idx))
  {
    res = _create_on_reset(hw_idx);
    if (MTLK_ERR_OK != res)
    {
      ILOG0_DS("DUT%d: utest[-1] - FAIL [%s]", hw_idx, mtlk_get_error_text(res));
      failed++;
    }
    else
    {
      ILOG0_D("DUT%d: utest[-1] - SUCCESS", hw_idx);
    }

    for (storage_idx = 0; storage_idx < storage_count; storage_idx++)
    {
      if (DUT_NV_MEMORY_FLASH == storage_vector[storage_idx])
        _flash_file_prepare(hw_idx, 512);

      if (0 == __nvm_size(hw_idx, storage_vector[storage_idx]))
      {
        ILOG0_DD("DUT%d: utest(%d) - WARNING [storage not supported]",
                 hw_idx, storage_vector[storage_idx]);
        warnings++;
        continue;
      }

      for(do_test = test_vector; NULL != (*do_test); do_test++)
      {
        res = (*do_test)(hw_idx, storage_vector[storage_idx]);

        if (MTLK_ERR_OK != res)
        {
          ILOG0_DDDS("DUT%d: utest[%d](%d) - FAIL [%s]", hw_idx, do_test - test_vector,
                      storage_vector[storage_idx], mtlk_get_error_text(res));
          failed++;
        }
        else
        {
          ILOG0_DDD("DUT%d: utest[%d](%d) - SUCCESS", hw_idx, do_test - test_vector,
                    storage_vector[storage_idx]);
        }
      }

      if (DUT_NV_MEMORY_FLASH == storage_vector[storage_idx])
        _flash_file_restore(hw_idx);
    }

    hw_idx++;
  }

  (void)dut_api_driver_stop(TRUE);

  if (0 == hw_idx)
  {
    ILOG0_V("DUT: No devices for unit testing");
    failed++;
  }

  if (0 != failed)
  {
    printf("DUT: utest result - FAIL [e:%d][w:%d]\n", failed, warnings);
    ELOG_DD("DUT: utest result - FAIL [e:%d][w:%d]",
            failed, warnings);
  }
  else if (0 != warnings)
  {
    printf("DUT: utest result - WARNING [w:%d]\n", warnings);
    ILOG0_D("DUT: utest result - WARNING [w:%d]", warnings);
  }
  else
  {
    printf("DUT: utest result - SUCCESS\n");
    ILOG0_V("DUT: utest result - SUCCESS");
  }
}

#endif /* MTLK_DEBUG */
