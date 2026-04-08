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

#include "mtlkinc.h"
#include "mtlkirbd.h"
#include "dataex.h"
#include "mtlkhal.h"
#include "mhi_dut.h"
#include "hw_mmb.h"

/*
  FIXME:
    The driver helper should not know about EEPROM format.
    To remove code after CRC generation on EEPROM data originator.
*/
#define EEPROM_CRC_ON_DRVHLPR

#if defined(CPTCFG_IWLWAV_EEPROM_SUPPORT)
  #include "eeprom_gpio.h"
#endif /* defined(CPTCFG_IWLWAV_EEPROM_SUPPORT) */

#include "eeprom_irbd.h"

#define LOG_LOCAL_GID   GID_EEPROM
#define LOG_LOCAL_FID   1

#if defined(CPTCFG_IWLWAV_EEPROM_SUPPORT)
  /** Enable direct access to the EEPROM */
  #define DUT_NVM_EEPROM
#endif /* defined(CPTCFG_IWLWAV_EEPROM_SUPPORT) */

/** Enable access to the EEPROM data on flash */
#define DUT_NVM_FLASH

/** Enable direct access to the EFUSE */
#define DUT_NVM_EFUSE

/*****************************************************************************
* Local type definitions
******************************************************************************/

/** DUT Driver command representation */
typedef union
{
  dutNvMemoryAccessParams_t access;
  dutNvMemoryFlushParams_t  flush;
} _dutNvMemoryParams_t;

/** Interface for data operations */
typedef struct {
  /** Handle to the reading routine

      \param obj        Descriptor of EEPROM data IRB interface [I]
      \param offset     Offset of EEPROM data [I]
      \param size       Size of data in bytes [IO]
                        - As input, size of \ref buff in bytes.
                        - As output, count of bytes read to \ref buff
      \param buf        Handle to the destination buffer [O]
      \param full_size  Size of available EEPROM data in bytes [O]

      \return
        error code on failure, MTLK_ERR_OK on success
  */
  int (*read)(mtlk_eeprom_irbd_t* obj, uint32 offset, uint32 *size, uint8 *buf, uint32 *full_size);

  /** Handle to the writing routine

      \param obj        Descriptor of EEPROM data IRB interface [I]
      \param offset     Offset of EEPROM data [I]
      \param size       Size of data in bytes [IO]
                        - As input, size of \ref buff in bytes
                        - As output, count of written bytes to \ref buff
      \param buf        Handle to the source buffer [O]
      \param full_size  Size of available EEPROM data in bytes [O]

      \return
        error code on failure, MTLK_ERR_OK on success
  */
  int (*write)(mtlk_eeprom_irbd_t* obj, uint32 offset, uint32 *size, uint8 const *buf, uint32 *full_size);

  /** Handle to the data saver routine

      \param obj    Descriptor of EEPROM data IRB interface [I]
      \param verify Non zero value enable data verification [I]

      \return
        error code on failure, MTLK_ERR_OK on success
  */
  int (*flush)(mtlk_eeprom_irbd_t* obj, uint8 verify);
} _nvm_ops_t;

#ifdef DUT_NVM_FLASH
/** private handle for EEPROM data on flash */
typedef struct {
  /** handle for file saver, include EEPROM data */
  struct mtlk_file_save *fsave;
  /** pointer to the buffer with EEPROM data
      \note: no direct allocation, offset of \ref fsave only */
  void *buffer;
  /** Represent modification state of \ref buffer */
  BOOL modified;
} _nvm_flash_handle_t;
#endif /* DUT_NVM_FLASH */

/** Descriptor of EEPROM data IRB interface */
struct _mtlk_eeprom_irbd_t {
  /** Handle to the IRBD client */
  mtlk_irbd_handle_t  *irbd_handle;
  /** root handle to the vap */
  mtlk_vap_handle_t   vap;
  /** Protector for IRB interface of EEPROM data*/
  mtlk_osal_mutex_t   mutex;
#ifdef DUT_NVM_FLASH
  /** Private data for EEPROM data on flash */
  _nvm_flash_handle_t  flash_handle;
#endif /* DUT_NVM_FLASH */
};

/** IRB event for DUT driver commands */
static const mtlk_guid_t _IRBE_DUT_DRV_CMD = MTLK_IRB_GUID_DUT_DRV_CMD;

#ifdef DUT_NVM_FLASH
  /** IRB event for EEPROM data storing on flash */
  static mtlk_guid_t const IRBE_FILE_SAVE = MTLK_IRB_GUID_FILE_SAVE;
#endif /* DUT_NVM_FLASH */

/*****************************************************************************
* Function implementation
******************************************************************************/

#ifdef DUT_NVM_EEPROM
/**
  Flush routine for direct EEPROM access

  \param obj    Descriptor of EEPROM data IRB interface [I]
  \param verify Non zero value enable data verification [I]

  \return MTLK_ERR_OK
*/
static int
_mtlk_eeprom_irbd_eeprom_flush(mtlk_eeprom_irbd_t* obj, uint8 verify)
{
  MTLK_UNREFERENCED_PARAM(obj);

  /* nothing to do, no cashed data */

  return MTLK_ERR_OK;
}

/**
  Reading routine for direct EEPROM access

  \param obj        Descriptor of EEPROM data IRB interface [I]
  \param offset     Offset of EEPROM data [I]
  \param size       Size of data in bytes [IO]
                    - As input, size of buff in bytes.
                    - As output, count of bytes read to buff
  \param buf        Handle to the destination buffer [O]
  \param full_size  Size of available EEPROM data in bytes [O]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_mtlk_eeprom_irbd_eeprom_read(mtlk_eeprom_irbd_t *obj, uint32 offset, uint32* size, uint8 *buf, uint32 *full_size)
{
  uint32 eeprom_size = 0;
  mtlk_ccr_t *ccr;
  int ret = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != obj);
  MTLK_ASSERT(NULL != size);
  MTLK_ASSERT(NULL != buf);
  MTLK_ASSERT(NULL != full_size);

  *full_size = 0;

  (void)mtlk_hw_get_prop(mtlk_vap_get_hwapi(obj->vap), MTLK_HW_PROP_CCR, &ccr, sizeof(&ccr));
  if (NULL == ccr)
  {
    ELOG_D("CID-%04x: Missed HW context for access to EEPROM via GPIO",
           mtlk_vap_get_oid(obj->vap));
    return MTLK_ERR_NO_RESOURCES;
  }

  ret = eeprom_gpio_init(ccr, &eeprom_size);
  if (MTLK_ERR_OK != ret)
  {
    ELOG_D("CID-%04x: Unable to initialize access to EEPROM via GPIO",
           mtlk_vap_get_oid(obj->vap));
    return ret; /* ERROR */
  }

  *full_size = eeprom_size;

  if (offset >= eeprom_size)
  {
    ELOG_DDD("CID-%04x: Wrong offset for access to EEPROM via GPIO [%u:%u]",
             mtlk_vap_get_oid(obj->vap), offset,  eeprom_size);
    ret = MTLK_ERR_PARAMS;
  }
  else
  {
    eeprom_size -= offset;

    if ((*size) > eeprom_size)
      *size = eeprom_size;

    ret = eeprom_gpio_read(ccr, offset, buf, *size);
    if (MTLK_ERR_OK != ret)
    {
      ELOG_D("CID-%04x: Unable to read data from EEPROM via GPIO", mtlk_vap_get_oid(obj->vap));
    }
  }

  eeprom_gpio_clean(ccr);

  return ret;
}

/**
  Writing routine for direct EEPROM access

  \param obj        Descriptor of EEPROM data IRB interface [I]
  \param offset     Offset of EEPROM data [I]
  \param size       Size of data in bytes [IO]
                    - As input, size of \ref buff in bytes
                    - As output, count of written bytes to \ref buff
  \param buf        Handle to the source buffer [O]
  \param full_size  Size of available EEPROM data in bytes [O]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_mtlk_eeprom_irbd_eeprom_write(mtlk_eeprom_irbd_t *obj, uint32 offset, uint32* size, uint8 const *buf, uint32 *full_size)
{
  uint32 eeprom_size = 0;
  mtlk_ccr_t *ccr;
  int ret = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != obj);
  MTLK_ASSERT(NULL != size);
  MTLK_ASSERT(NULL != buf);
  MTLK_ASSERT(NULL != full_size);

  *full_size = 0;

  (void)mtlk_hw_get_prop(mtlk_vap_get_hwapi(obj->vap), MTLK_HW_PROP_CCR, &ccr, sizeof(&ccr));
  if (NULL == ccr)
  {
    ELOG_D("CID-%04x: Missed HW context for access to EEPROM via GPIO",
           mtlk_vap_get_oid(obj->vap));
    return MTLK_ERR_NO_RESOURCES;
  }

  ret = eeprom_gpio_init(ccr, &eeprom_size);
  if (MTLK_ERR_OK != ret)
  {
    ELOG_D("CID-%04x: Unable to initialize access to EEPROM via GPIO",
           mtlk_vap_get_oid(obj->vap));
    return ret; /* ERROR */
  }

  *full_size = eeprom_size;

  if (offset >= eeprom_size)
  {
    ELOG_DDD("CID-%04x: Wrong offset for access to EEPROM via GPIO [%u:%u]",
             mtlk_vap_get_oid(obj->vap), offset,  eeprom_size);
    ret = MTLK_ERR_PARAMS;
  }
  else
  {
    eeprom_size -= offset;

    if ((*size) > eeprom_size)
      *size = eeprom_size;

    ret = eeprom_gpio_write(ccr, offset, buf, *size);
    if (MTLK_ERR_OK != ret)
    {
      ELOG_D("CID-%04x: Unable to write data to EEPROM via GPIO", mtlk_vap_get_oid(obj->vap));
    }
  }

  eeprom_gpio_clean(ccr);

  return ret;

}

/** Interface for direct EEPROM data operations */
static _nvm_ops_t const eeprom_ops = {
  _mtlk_eeprom_irbd_eeprom_read,
  _mtlk_eeprom_irbd_eeprom_write,
  _mtlk_eeprom_irbd_eeprom_flush
  };

#endif /* DUT_NVM_EEPROM */

#ifdef DUT_NVM_FLASH
/**
  Flush the EEPROM data in to the flash

  \param obj    Descriptor of EEPROM data IRB interface [I]
  \param verify Non zero value enable data verification [I]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_mtlk_eeprom_irbd_flash_flush(mtlk_eeprom_irbd_t* obj, uint8 verify)
{
  _nvm_flash_handle_t *handle = &obj->flash_handle;
  int ret = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != obj);

  if (FALSE == handle->modified) {
    WLOG_V("Nothing modified. Ignore flush request.");
    return ret;
  }

  MTLK_ASSERT(NULL != handle->fsave);
  MTLK_ASSERT(handle->fsave->size > 0);

  ILOG1_DSD("CID-%04x: DUT DRV Send EEPROM data to FLASH [%s:%d]",
            mtlk_vap_get_oid(obj->vap), handle->fsave->fname, handle->fsave->size);

  ret = mtlk_irbd_notify_app(mtlk_irbd_get_root(), &IRBE_FILE_SAVE,
    (void*)handle->fsave, sizeof(*handle->fsave) + handle->fsave->size);
  if (MTLK_ERR_OK != ret)
  {
    ELOG_DD("CID-%04x: DUT DRV Can not send EEPROM data to FLASH [%d]",
            mtlk_vap_get_oid(obj->vap), ret);
    return ret;
  }

#ifdef EEPROM_CRC_ON_DRVHLPR
  MTLK_UNREFERENCED_PARAM(verify);
#else /* EEPROM_CRC_ON_DRVHLPR */
  if (verify)
  {
    mtlk_df_fw_file_buf_t fb;

    /* simply give a time for file saver task (run system scheduler)
      \note:
        The request to save data in to the file can be lost,
        because of IRB events based on netlink protocol without
        any acknowledge mechanicm. Acknowledges can not be used
        because of application notifications can be called from
        different contexts, which do not allow context switch.
      \todo: think about IRB mechanism
    */
    mtlk_osal_msleep(3000);

    ret = mtlk_hw_load_file(mtlk_vap_get_hwapi(obj->vap), handle->fsave->fname, &fb);
    if (MTLK_ERR_OK != ret)
    {
      ELOG_D("CID-%04x: DUT DRV can not load EEPROM data for validation",
             mtlk_vap_get_oid(obj->vap));
      return ret;
    }

    if (fb.size != handle->fsave->size)
    {
      ELOG_DDD("CID-%04x: DUT DRV EEPROM data validation failed, "
               "detected %u bytes, expected %u bytes",
               mtlk_vap_get_oid(obj->vap), fb.size, handle->fsave->size);
      ret = MTLK_ERR_EEPROM;
    }
    else if (0 != memcmp(handle->buffer, fb.buffer, fb.size))
    {
      ELOG_D("CID-%04x: DUT DRV EEPROM data validation failed, "
               "data mismatch", mtlk_vap_get_oid(obj->vap));
      ret = MTLK_ERR_EEPROM;
    }
    else
    {
      ILOG2_DSD("CID-%04x: DUT DRV EEPROM data successfully validated [%s:%d]",
                mtlk_vap_get_oid(obj->vap), handle->fsave->fname, fb.size);

    }

    mtlk_hw_unload_file(mtlk_vap_get_hwapi(obj->vap), &fb);
  }
#endif /* EEPROM_CRC_ON_DRVHLPR */

  if (MTLK_ERR_OK == ret)
    handle->modified = FALSE;

  return ret;
}

/**
  Cleanup routine for EEPROM data on flash

  \param obj    Descriptor of EEPROM data IRB interface [I]

  \return MTLK_ERR_OK

  \note
    Cached data in force to be removed. The modified data will be flushed,
    but without any guaranty, due to skip of error on flush operation.
*/
static int
_mtlk_eeprom_irbd_flash_close(mtlk_eeprom_irbd_t* obj)
{
  _nvm_flash_handle_t *handle = &obj->flash_handle;

  MTLK_ASSERT(NULL != obj);

  if (NULL != handle->fsave)
  {
    (void)_mtlk_eeprom_irbd_flash_flush(obj, 0 /* no verification */);

    mtlk_osal_mem_free(handle->fsave);
    handle->fsave = NULL;
  }

  handle->buffer = NULL;
  handle->modified = FALSE;

  return MTLK_ERR_OK;
}

/**
   Initialization routine for EEPROM data on flash

  \param obj    Descriptor of EEPROM data IRB interface [I]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_mtlk_eeprom_irbd_flash_open(mtlk_eeprom_irbd_t* obj)
{
  _nvm_flash_handle_t *handle = &obj->flash_handle;
  int ret = MTLK_ERR_OK;
  mtlk_df_fw_file_buf_t fb;
  uint32 max_size = MTLK_MAX_EEPROM_SIZE;
  char fname[0x40];

  MTLK_ASSERT(NULL != obj);

  if (NULL != handle->fsave)
  {
    /* the context already opened and ready for usage */
    return MTLK_ERR_OK;
  }

  ILOG2_D("CID-%04x: DUT DRV load EEPROM data from flash", mtlk_vap_get_oid(obj->vap));

  (void)mtlk_hw_get_prop(mtlk_vap_get_hwapi(obj->vap), MTLK_HW_EEPROM_NAME, fname, sizeof(fname));
  ret = mtlk_hw_load_file(mtlk_vap_get_hwapi(obj->vap), fname, &fb);
  if (MTLK_ERR_OK != ret)
  {
    ELOG_D("CID-%04x: DUT DRV can not load EEPROM data", mtlk_vap_get_oid(obj->vap));
    return ret;
  }

  /* Always allocate a buffer of maximal supported size */
  if (fb.size > max_size) {
    ELOG_DSDD("CID-%04x: DUT DRV: EEPROM file `%s' too big (%u > %u)",
        mtlk_vap_get_oid(obj->vap), fname, fb.size, max_size);
    goto unload;

  }

  handle->fsave = (struct mtlk_file_save*)mtlk_osal_mem_alloc(
    sizeof(*handle->fsave) + max_size, MTLK_MEM_TAG_NVM_ACCESS);
  if(NULL == handle->fsave)
  {
    ret = MTLK_ERR_NO_MEM;
  }
  else
  {
    mtlk_snprintf(handle->fsave->fname, sizeof(handle->fsave->fname), "%s", fname);
    handle->fsave->size = max_size;

    handle->buffer = (handle->fsave + 1);
    memset(handle->buffer, 0, max_size);
    wave_memcpy (handle->buffer, max_size, fb.buffer, fb.size);

    ILOG2_DD("CID-%04x: DUT DRV successfully loaded %u bytes",
            mtlk_vap_get_oid(obj->vap), handle->fsave->size);
  }
unload:
  mtlk_hw_unload_file(mtlk_vap_get_hwapi(obj->vap), &fb);
  return ret;
}

/**
  Reading routine for EEPROM data on flash

  \param obj        Descriptor of EEPROM data IRB interface [I]
  \param offset     Offset of EEPROM data [I]
  \param size       Size of data in bytes [IO]
                    - As input, size of buff in bytes.
                    - As output, count of bytes read to buff
  \param buf        Handle to the destination buffer [O]
  \param full_size  Size of available data on flash in bytes [O]

  \return
    error code on failure, MTLK_ERR_OK on success

  \remark
    Data reading is not available in write-only mode,
    the error MTLK_ERR_BUSY is returned in that case.
*/
static int
_mtlk_eeprom_irbd_flash_read(mtlk_eeprom_irbd_t* obj, uint32 offset, uint32* size, uint8 *buf, uint32 *full_size)
{
  int ret = MTLK_ERR_OK;
  _nvm_flash_handle_t *handle = &obj->flash_handle;

  MTLK_ASSERT(NULL != obj);
  MTLK_ASSERT(NULL != size);
  MTLK_ASSERT(NULL != buf);
  MTLK_ASSERT(NULL != full_size);

  *full_size = 0;

  ret = _mtlk_eeprom_irbd_flash_open(obj);
  if (MTLK_ERR_OK != ret)
    return ret;

  MTLK_ASSERT(NULL != handle->fsave);
  MTLK_ASSERT(NULL != handle->buffer);

  *full_size = handle->fsave->size;

  if (offset >= handle->fsave->size)
  {
    ELOG_DDD("CID-%04x: Wrong offset for access to EEPROM on FLASH [%u:%u]",
             mtlk_vap_get_oid(obj->vap), offset,  handle->fsave->size);
    return MTLK_ERR_PARAMS;
  }

  if ((*size) > (handle->fsave->size - offset))
    *size = (handle->fsave->size - offset);

  wave_memcpy(buf, *size, handle->buffer + offset, *size);

  return MTLK_ERR_OK;
}

/**
  Writing routine for EEPROM data on flash

  \param obj        Descriptor of EEPROM data IRB interface [I]
  \param offset     Offset of EEPROM data [I]
  \param size       Size of data in bytes [IO]
                    - As input, size of \ref buff in bytes
                    - As output, count of written bytes to \ref buff
  \param buf        Handle to the source buffer [O]
  \param full_size  Size of available data on flash in bytes [O]

  \return
    error code on failure, MTLK_ERR_OK on success
*/
static int
_mtlk_eeprom_irbd_flash_write(mtlk_eeprom_irbd_t* obj, uint32 offset, uint32* size, uint8 const *buf, uint32 *full_size)
{
  int ret = MTLK_ERR_OK;
  _nvm_flash_handle_t *handle = &obj->flash_handle;

  MTLK_ASSERT(NULL != obj);
  MTLK_ASSERT(NULL != size);
  MTLK_ASSERT(NULL != buf);
  MTLK_ASSERT(NULL != full_size);

  *full_size = 0;

  ret = _mtlk_eeprom_irbd_flash_open(obj);
  if (MTLK_ERR_OK != ret)
    return ret;

  MTLK_ASSERT(NULL != handle->fsave);
  MTLK_ASSERT(NULL != handle->buffer);

  *full_size = handle->fsave->size;

  if (offset >= handle->fsave->size)
  {
    ILOG1_DDD("CID-%04x: DUT Wrong offset for access to EEPROM via FLASH [%u:%u]",
             mtlk_vap_get_oid(obj->vap), offset,  handle->fsave->size);
    return MTLK_ERR_PARAMS;
  }

  if (0 == (*size))
    return MTLK_ERR_OK; /* nothing to do */

  if ((*size) > (handle->fsave->size - offset))
    *size = (handle->fsave->size - offset);

  wave_memcpy(handle->buffer + offset, *size, buf, *size);

  handle->modified = TRUE;

  return MTLK_ERR_OK;
}

/** Interface for EEPROM data on flash operations */
static _nvm_ops_t const flash_ops = {
  _mtlk_eeprom_irbd_flash_read,
  _mtlk_eeprom_irbd_flash_write,
  _mtlk_eeprom_irbd_flash_flush
  };
#endif /* DUT_NVM_FLASH */

#ifdef DUT_NVM_EFUSE

typedef enum {
  eFuse_read_op,
  eFuse_write_op,
} eFuse_ops_t;

/**
R/W routine for eFuse access

\param obj        Descriptor of eFuse data IRB interface [I]
\param offset     Offset of eFuse data [I]
\param size       Size of data in bytes [IO]
- As input, size of buff in bytes.
- As output, count of bytes read/written to/from buff
\param buf        pointer to the dst/src buffer [O]
\param full_size  Size of available eFuse data in bytes [O]
\param op         Requested operation [I]

\return
error code on failure, MTLK_ERR_OK on success
*/
static int
_mtlk_eeprom_irbd_efuse_rw(mtlk_eeprom_irbd_t *obj, uint32 offset, uint32 *size, uint8 *buf, uint32 *full_size, eFuse_ops_t op)
{
  mtlk_ccr_t *ccr = NULL;
  int bytes_handled = 0;

  MTLK_ASSERT(NULL != obj);
  MTLK_ASSERT(NULL != size);
  MTLK_ASSERT(NULL != buf);
  MTLK_ASSERT(NULL != full_size);
  if ((NULL == obj) || (NULL == size) ||
      (NULL == buf) || (NULL == full_size)) {
    return MTLK_ERR_PARAMS;
  }

  *full_size = 0;

  if ((op != eFuse_read_op) && (op != eFuse_write_op)) {
    *size = 0;
    return MTLK_ERR_PARAMS;
  }

  (void)mtlk_hw_get_prop(mtlk_vap_get_hwapi(obj->vap), MTLK_HW_PROP_CCR, &ccr, sizeof(&ccr));
  if (NULL == ccr)
  {
    *size = 0;
    ELOG_D("CID-%04x: Missed HW context for access to eFuse",
      mtlk_vap_get_oid(obj->vap));
    return MTLK_ERR_NO_RESOURCES;
  }

  *full_size = __mtlk_ccr_efuse_get_size(ccr);
  if (0 == *full_size)
  {
    *size = 0;
    ELOG_D("CID-%04x: eFuse OPs are not supported",
      mtlk_vap_get_oid(obj->vap));
    return MTLK_ERR_NOT_SUPPORTED;
  }

  /* checks of offset, size vs max_size are done within read/write methods */
  switch (op) {
  case eFuse_read_op:
    bytes_handled = __mtlk_ccr_efuse_read(ccr, buf, offset, *size);
    break;
  case eFuse_write_op:
    bytes_handled = __mtlk_ccr_efuse_write(ccr, buf, offset, *size);
    break;
  }

  *size = bytes_handled;
  if (0 == bytes_handled) {
    ELOG_D("CID-%04x: Unable to perform eFuse r-w operation", mtlk_vap_get_oid(obj->vap));
    return MTLK_ERR_EEPROM;
  }

  return MTLK_ERR_OK;
}

/**
Flush routine for eFuse access

\param obj    Descriptor of eFuse data IRB interface [I]
\param verify Non zero value enable data verification [I]

\return MTLK_ERR_OK
*/
static int
_mtlk_eeprom_irbd_efuse_flush(mtlk_eeprom_irbd_t* obj, uint8 verify)
{
  MTLK_UNREFERENCED_PARAM(obj);
  MTLK_UNREFERENCED_PARAM(verify);

  /* nothing to do, no cashed data */
  return MTLK_ERR_OK;
}

/**
Reading routine for eFuse access

\param obj        Descriptor of eFuse data IRB interface [I]
\param offset     Offset of eFuse data [I]
\param size       Size of data in bytes [IO]
- As input, size of buff in bytes.
- As output, count of bytes read to buff
\param buf        Handle to the destination buffer [O]
\param full_size  Size of available eFuse data in bytes [O]

\return
error code on failure, MTLK_ERR_OK on success
*/
static int
_mtlk_eeprom_irbd_efuse_read(mtlk_eeprom_irbd_t *obj, uint32 offset, uint32* size, uint8 *buf, uint32 *full_size)
{
  ILOG0_DD("offset: %d, size: %d", offset, (NULL != size ? *size : 0));
  return _mtlk_eeprom_irbd_efuse_rw(obj, offset, size, buf, full_size, eFuse_read_op);
}

/**
Writing routine for eFuse access

\param obj        Descriptor of eFuse data IRB interface [I]
\param offset     Offset of eFuse data [I]
\param size       Size of data in bytes [IO]
- As input, size of buff in bytes.
- As output, count of bytes read to buff
\param buf        Handle to the destination buffer [O]
\param full_size  Size of available eFuse data in bytes [O]

\return
error code on failure, MTLK_ERR_OK on success
*/
static int
_mtlk_eeprom_irbd_efuse_write(mtlk_eeprom_irbd_t* obj, uint32 offset, uint32 *size, uint8 const *buf, uint32 *full_size)
{
  ILOG0_DD("offset: %d, size: %d", offset, (NULL != size ? *size : 0));
  return _mtlk_eeprom_irbd_efuse_rw(obj, offset, size, (uint8 *)buf, full_size, eFuse_write_op);
}

/** Interface for direct EFUSE data operations */
static _nvm_ops_t const efuse_ops = {
    _mtlk_eeprom_irbd_efuse_read,
    _mtlk_eeprom_irbd_efuse_write,
    _mtlk_eeprom_irbd_efuse_flush
  };
#endif /* DUT_NVM_EFUSE */

/**
  DUT driver command handler

  \param irbd     Handle to the IRBD interface [I]
  \param context  Handle to the EEPROM data IRB interface [I]
  \param evt      IRB event GUID [I]
  \param buf      Handle to the event data [I]
  \param size     Size of event data [I]
*/
static void __MTLK_IFUNC
_mtlk_eeprom_irbd_on_dut_drv_cmd
                (mtlk_irbd_t        *irbd,
                 mtlk_handle_t      context,
                 mtlk_guid_t const  *evt,
                 void               *buf,
                 uint32             *size)
{
  mtlk_eeprom_irbd_t *obj = (mtlk_eeprom_irbd_t*)context;
  dutMessage_t *msg = (dutMessage_t *)buf;
  _dutNvMemoryParams_t *param = NULL;
  _nvm_ops_t const *ops = NULL;
  uint16 msgId;
  dutNvMemoryType_e storage;
  uint32 address = 0;
  uint32 length = 0;
  uint16 msgLength = 0;
  int err = MTLK_ERR_OK;

  MTLK_UNREFERENCED_PARAM(irbd);

  /* paranoidal assertion */
  MTLK_ASSERT(NULL != obj);
  MTLK_ASSERT(0 == mtlk_guid_compare(&_IRBE_DUT_DRV_CMD, evt));
  MTLK_ASSERT(NULL != msg);
  MTLK_ASSERT(NULL != size);

  mtlk_dump(4, buf, MIN(16,(*size)), "DRV cmd");

  if (*size > sizeof(dutMessage_t))
  {
    ELOG_DDD("CID-%04x: DUT DRV request of unexpected size %u bytes arrived (expected %u bytes or less)",
             mtlk_vap_get_oid(obj->vap), *size, sizeof(*msg));
    *size = 0;
    return;
  }

  msgLength = DUT_TO_HOST16(msg->msgLength);

  if (*size < (msgLength + 8 /* dutMessage_t header */))
  {
    ELOG_DDD("CID-%04x: DUT DRV request of unexpected size %u bytes arrived (expected %u bytes or greater)",
             mtlk_vap_get_oid(obj->vap), *size, msgLength + 8);
    *size = 0;
    return;
  }

  msgId = DUT_TO_HOST16(msg->msgId);

  ILOG1_DDD("CID-%04x: DUT DRV request received '%u' with data length '%u'",
            mtlk_vap_get_oid(obj->vap), msgId, msgLength);

  param = (_dutNvMemoryParams_t*)(msg->data);
  switch(msgId)
  {
#ifdef DUT_NVM_FLASH
    case DUT_DGM_GET_NV_FLASH_NAME_REQ:
      err = mtlk_hw_get_prop(mtlk_vap_get_hwapi(obj->vap), MTLK_HW_EEPROM_NAME, msg->data, msgLength);
      goto out;
#endif /* DUT_NVM_EEPROM */
    case DUT_DGM_READ_NV_MEMORY_REQ:
    case DUT_DGM_WRITE_NV_MEMORY_REQ:
      storage = DUT_TO_HOST32(param->access.nvMemoryType);
      address = DUT_TO_HOST32(param->access.address);
      length = DUT_TO_HOST32(param->access.length);

      if (length > sizeof(param->access.data))
      {
        ELOG_DDD("CID-%04x: DUT DRV request with wrong length to the EEPROM data"
                " (%u > %u)", mtlk_vap_get_oid(obj->vap), length, sizeof(param->access.data));
        err = MTLK_ERR_PARAMS;
        goto out; /* ERROR */
      }
      break;
    case DUT_DGM_FLUSH_NV_MEMORY_REQ:
      storage = DUT_TO_HOST32(param->flush.nvMemoryType);
      break;
    case DUT_DGM_CHANGE_BAND_REQ:
        {
          dutChangeBand_t *changeRadio = NULL;
          bandRadioType_e radio_id;

          changeRadio = (dutChangeBand_t*)(msg->data);
          radio_id = changeRadio->changeToRadio;

          /* check if radio_id_txmm fits our supported values */
          if ((radio_id >= 0) && (radio_id < WAVE_CARD_RADIO_NUM_MAX))
          {
            wave_vap_radio_id_txmm_set(obj->vap, radio_id); /* set radio_id_txmm to the received value */
            err = MTLK_ERR_OK;
            goto out; /* send DUT_DGM_CHANGE_BAND_CFM message */
          } else {
            ELOG_D("DRIVER_FW_GENERAL: received unexpected radio_id_txmm value %d", radio_id);
            err = MTLK_ERR_PARAMS;
            goto out; /* ERROR */
          }
        }
    default:
      ILOG1_DD("CID-%04x: DUT DRV request '%u' skipped by EEPROM module",
               mtlk_vap_get_oid(obj->vap), msgId);
      return;
  }

  ILOG1_DD("CID-%04x: Storage type is %d", mtlk_vap_get_oid(obj->vap), storage);
  switch (storage)
  {
#ifdef DUT_NVM_EEPROM
    case DUT_NV_MEMORY_EEPROM:
      ops = &eeprom_ops;
      break;
#endif /* DUT_NVM_EEPROM */
#ifdef DUT_NVM_FLASH
    case DUT_NV_MEMORY_FLASH:
      ops = &flash_ops;
      break;
#endif /* DUT_NVM_FLASH */
#ifdef DUT_NVM_EFUSE
    case DUT_NV_MEMORY_EFUSE:
      ops = &efuse_ops;
      break;
#endif /* DUT_NVM_EFUSE*/
    default:
      ELOG_DD("CID-%04x: DUT DRV request for not supported EEPROM data "
              "storage type (%d)", mtlk_vap_get_oid(obj->vap), storage);
      err = MTLK_ERR_PARAMS;
      goto out; /* ERROR */
  }

  MTLK_ASSERT(NULL != ops->read);
  MTLK_ASSERT(NULL != ops->write);
  MTLK_ASSERT(NULL != ops->flush);

  mtlk_osal_mutex_acquire(&obj->mutex);

  switch(msgId)
  {
    case DUT_DGM_READ_NV_MEMORY_REQ:
    {
      uint32 eepromSize;

      ILOG2_DDDD("CID-%04x: DUT DRV request to read %u bytes from %u (s:%d)",
                 mtlk_vap_get_oid(obj->vap), length, address, storage);

      err = ops->read(obj, address, &length, param->access.data, &eepromSize);

      mtlk_dump(3, param->access.data, MIN(16,(length)), "EEPROM read");

      param->access.length = HOST_TO_DUT32(length);
      param->access.eepromSize = HOST_TO_DUT32(eepromSize);
      break;
    }
    case DUT_DGM_WRITE_NV_MEMORY_REQ:
    {
      uint32 eepromSize;

      ILOG2_DDDD("CID-%04x: DUT DRV request to write %u bytes to %u (s:%d)",
                 mtlk_vap_get_oid(obj->vap), length, address, storage);

      mtlk_dump(3, param->access.data, MIN(16,(length)), "EEPROM write");

      err = ops->write(obj, address, &length, param->access.data, &eepromSize);

      param->access.length = HOST_TO_DUT32(length);
      param->access.eepromSize = HOST_TO_DUT32(eepromSize);
      break;
    }
    case DUT_DGM_FLUSH_NV_MEMORY_REQ:
      ILOG2_DD("CID-%04x: DUT DRV request to flush EEPROM data (storage:%d)",
               mtlk_vap_get_oid(obj->vap), storage);

      err = ops->flush(obj, param->flush.verifyNvmData);
      break;
    default:
      ELOG_DD("CID-%04x: Invalid request ID: %d", mtlk_vap_get_oid(obj->vap), msgId);
      err= MTLK_ERR_PARAMS;
      break;
  }

  mtlk_osal_mutex_release(&obj->mutex);

out:
  if (MTLK_ERR_OK == err)
    msg->status = HOST_TO_DUT16(DUT_STATUS_PASS);
  else
    msg->status = HOST_TO_DUT16(DUT_STATUS_FAIL);

  ILOG2_DDD("CID-%04x: DUT DRV request '%u' finished (e:%d)",
            mtlk_vap_get_oid(obj->vap), msgId, err);

  msgId++; /* CFM */

  msg->msgId = HOST_TO_DUT16(msgId);
}

/**
  Register IRB handler for EEPROM access

  \param vap  root handle to the vap [I]

  \return
    Descriptor of EEPROM data IRB interface on success,
    NULL on failure
*/
mtlk_eeprom_irbd_t* __MTLK_IFUNC
mtlk_eeprom_irbd_register (mtlk_vap_handle_t vap)
{
  mtlk_eeprom_irbd_t *obj;

  MTLK_ASSERT(NULL != vap);

  obj = (mtlk_eeprom_irbd_t *)mtlk_osal_mem_alloc(sizeof(*obj), MTLK_MEM_TAG_NVM_ACCESS);
  if(NULL == obj)
  {
    ELOG_D("CID-%04x: Unable to allocate memory for EEPROM IRB interface",
           mtlk_vap_get_oid(vap));
    return NULL;
  }

  memset(obj, 0, sizeof(*obj));

  obj->vap = vap;

  obj->irbd_handle = mtlk_irbd_register(mtlk_vap_get_irbd(obj->vap), &_IRBE_DUT_DRV_CMD, 1, _mtlk_eeprom_irbd_on_dut_drv_cmd, HANDLE_T(obj));
  if (NULL == obj->irbd_handle)
  {
    ELOG_D("CID-%04x: Unable to register EEPROM IRB interface", mtlk_vap_get_oid(vap));
    mtlk_osal_mem_free(obj);
    return NULL;
  }

  mtlk_osal_mutex_init(&obj->mutex);

  return obj;
}

/**
  Unregister IRB handler for EEPROM access

  \param obj  Descriptor of EEPROM data IRB interface [I]
*/
void __MTLK_IFUNC
mtlk_eeprom_irbd_unregister (mtlk_eeprom_irbd_t* obj)
{
  if (NULL == obj)
    return; /* nothing todo */

#ifdef DUT_NVM_FLASH
  mtlk_osal_mutex_acquire(&obj->mutex);

  /* in force close opened sessions */
  _mtlk_eeprom_irbd_flash_close(obj);

  mtlk_osal_mutex_release(&obj->mutex);
#endif /* DUT_NVM_FLASH */

  if (NULL != obj->irbd_handle)
    mtlk_irbd_unregister(mtlk_vap_get_irbd(obj->vap), obj->irbd_handle);

  obj->irbd_handle = NULL;

  mtlk_osal_mutex_cleanup(&obj->mutex);

  /* paranoidal cleanup */
  memset(obj, 0, sizeof(*obj));

  mtlk_osal_mem_free(obj);
}

