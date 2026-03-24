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
 *  wlan driver helper for file-system access
 */
#include "mtlkinc.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fshlpr.h"
#include "dataex.h"
#include "mtlkirba.h"

/*
  FIXME: EEPROM_CRC_ON_DRVHLPR
    The driver helper should not know about EEPROM format.
    To remove code after CRC generation on EEPROM data originator.
*/
#if defined(EEPROM_CRC_ON_DRVHLPR)
#include "fshlpr_utest.h"
#endif /* defined(EEPROM_CRC_ON_DRVHLPR) */

#define LOG_LOCAL_GID   GID_IRB
#define LOG_LOCAL_FID   3

/** IRB event for data storing on file-system */
static const mtlk_guid_t IRBE_FILE_SAVE = MTLK_IRB_GUID_FILE_SAVE;

/** Shell command to handle saved data */
static char file_saver[100];

#ifdef EEPROM_CRC_ON_DRVHLPR
/* NOTE: For temporal solution we can use unoptimized version */
static uint32
_crc32 (uint32 crc, uint8 const *p, uint32 len)
{
/*
 * There are multiple 16-bit CRC polynomials in common use, but this is
 * *the* standard CRC-32 polynomial, first popularized by Ethernet.
 * x^32+x^26+x^23+x^22+x^16+x^12+x^11+x^10+x^8+x^7+x^5+x^4+x^2+x^1+x^0
 */
#define CRCPOLY_LE 0xedb88320

  int i;

  while (len--)
  {
    crc ^= *p++;
    for (i = 0; i < 8; i++)
      crc = (crc >> 1) ^ ((crc & 1) ? CRCPOLY_LE : 0);
  }

  eeprom_utest_step(CRC_DONE);

  return crc;
}

#define CIS_SECTION_HEADER_SIZE 2

/**
  This function tries to find a CIS section by ID

  \param id         CIS block id to search for [I]
  \param cis        pointer to the CIS buffer [I]
  \param cis_size   size of CIS buffer in bytes [I]

  \return
    NULL if CIS block not found, otherwise pointer to the CIS section
*/
static void*
_eeprom_cis_find (uint8 id, uint8 *cis, int cis_size)
{
  MTLK_ASSERT(NULL != cis);

  while (cis_size >= 1)
  {
    if (cis[0] == id)
      return cis;

    if (0xFF == cis[0])
      return NULL;

    if (cis_size < 2)
      break;

    if (cis[1] + CIS_SECTION_HEADER_SIZE > cis_size)
    {
      ELOG_DD("Corrupted EEPROM data detected, asked for %d bytes from %d "
              "available bytes", cis[1] + CIS_SECTION_HEADER_SIZE, cis_size);

      eeprom_utest_step(CIS_SIZE_WRONG);

      return NULL;
    }


    cis_size -= cis[1] + CIS_SECTION_HEADER_SIZE;
    cis += cis[1] + CIS_SECTION_HEADER_SIZE;
  }

  return NULL;
}

/**
  Append to the EEPROM data in the buffer CRC section

  \param buffer   handle to buffer with EEPROM data
  \param size     handle to the buffer with size of EEPROM data

  \return
    error code or MTLK_ERR_OK on success

  \remark
    The new buffer can be allocated. In that case the size value
    will be updated also.
*/
static int
_eeprom_crc_append(void **buffer, int *size)
{
#define EEPROM_CIS_HEADER_OFFSET 0x40
#define EEPROM_CIS_HEADER_SIZE 8

#define EEPROM_CIS_CRC_ID 0x85
#define EEPROM_CIS_CRC_LEN 4
#define EEPROM_TERMINATOR_ID 0xFF

#define EEPROM_CRC_SECTION_SIZE (CIS_SECTION_HEADER_SIZE + EEPROM_CIS_CRC_LEN)
#define EEPROM_TERMINATOR_SIZE 1

  uint8 *p, *dst, *src = *buffer;
  uint16 data_size, used_data;
  uint32 crc;

  /* Check for minimum EEPROM data size */
  if (*size < (EEPROM_CIS_HEADER_OFFSET + EEPROM_CIS_HEADER_SIZE))
  {
    ELOG_D("FS HLPR: Wrong size of EEPROM data [%d]", *size);

    eeprom_utest_step(EEPROM_SIZE_WRONG);

    return MTLK_ERR_EEPROM;
  }

  /* Check for valid EEPROM data header */
  if ((0xFC != src[0]) || (0x1B != src[1]))
  {
    ELOG_DD("FS HLPR: Wrong EEPROM data header [%02x %02x]", src[0], src[1]);

    eeprom_utest_step(EEPROM_HEAD_WRONG);

    return MTLK_ERR_EEPROM;
  }

  /* Check for valid CIS area data size */
  data_size  = src[EEPROM_CIS_HEADER_OFFSET + 0];
  data_size |= src[EEPROM_CIS_HEADER_OFFSET + 1] << 8;
  if (*size < (EEPROM_CIS_HEADER_OFFSET + data_size))
  {
    ELOG_D("FS HLPR: Wrong size of EEPROM CIS data [%d]", data_size);

    eeprom_utest_step(CIS_AREA_SIZE_WRONG);

    return MTLK_ERR_EEPROM;
  }

  /* Check for valid EEPROM version */
  if (4 != src[EEPROM_CIS_HEADER_OFFSET + 7])
  {
    ELOG_D("FS HLPR: Unsupported version of the EEPROM [%d]",
           src[EEPROM_CIS_HEADER_OFFSET + 7]);

    eeprom_utest_step(EEPROM_VERSION_WRONG);

    return MTLK_ERR_EEPROM;
  }

  /* Check for CRC field presence */
  if (NULL != _eeprom_cis_find(EEPROM_CIS_CRC_ID,
                               src + EEPROM_CIS_HEADER_OFFSET + EEPROM_CIS_HEADER_SIZE,
                               data_size - EEPROM_CIS_HEADER_SIZE))
  {
    /* NOTE: The input signal to remove that code. */
    ILOG0_V("FS HLPR: The CRC section already present in EEPROM data");

    eeprom_utest_step(CRC_PRESENT);

    return MTLK_ERR_OK;
  }

  /* Find EEPROM data terminator */
  p = _eeprom_cis_find(EEPROM_TERMINATOR_ID,
                       src + EEPROM_CIS_HEADER_OFFSET + EEPROM_CIS_HEADER_SIZE,
                       data_size - EEPROM_CIS_HEADER_SIZE);
  if (NULL == p)
  {
    ELOG_V("FS HLPR: Missed terminator for CIS section in EEPROM data");

    eeprom_utest_step(TERMINATOR_MISSED);

    return MTLK_ERR_EEPROM;
  }

  used_data = ((p - src) + EEPROM_TERMINATOR_SIZE);
  used_data += EEPROM_CRC_SECTION_SIZE;

  /* Check for available buffer */
  if ((*size) >= used_data)
  {
    ILOG3_V("FS HLPR: Use the same buffer for EEPROM data");
    /* reuse current buffer */
    dst = src;
    /* p: already store write pointer */

    eeprom_utest_step(BUFFER_REUSED);
  }
  else
  {
    ILOG3_D("FS HLPR: Allocate new buffer for EEPROM data [%d]", used_data);

    /* allocate new buffer */
    dst = malloc(used_data);
    if (NULL == dst)
    {
      ELOG_D("FS HLPR: Failed to allocate memory [%d]", used_data);
      return MTLK_ERR_NO_MEM;
    }

    memcpy(dst, src, used_data - EEPROM_CRC_SECTION_SIZE);

    *buffer = dst;
    *size = used_data;

    /* prepare write pointer */
    p = dst + (p - src);

    eeprom_utest_step(BUFFER_RECREATED);
  }

  /* Correct valid CIS area data size if needed */
  if (data_size < (used_data - EEPROM_CIS_HEADER_OFFSET))
  {
    ILOG3_DD("FS HLPR: Valid date size in CIS area is updated from [%d] to [%d]",
             data_size, used_data - EEPROM_CIS_HEADER_OFFSET);

    data_size = used_data - EEPROM_CIS_HEADER_OFFSET;

    dst[EEPROM_CIS_HEADER_OFFSET + 0] = (data_size >> 0) & 0x00FF;
    dst[EEPROM_CIS_HEADER_OFFSET + 1] = (data_size >> 8) & 0x00FF;

    eeprom_utest_step(CIS_AREA_SIZE_UPDATED);
  }

#define EEPROM_PUT8(v) { (*p) = (v); p++; }

  EEPROM_PUT8(EEPROM_CIS_CRC_ID);
  EEPROM_PUT8(EEPROM_CIS_CRC_LEN);

  /* Do CRC calculation */
  crc = _crc32(~0L, dst, p - dst);
  crc ^= ~0L;

  ILOG1_DD("FS HLPR: Calculate CRC32 [0x%08x] for EEPROM data [%d]",
           crc, p - dst);

  /* Complete EEPROM data (big-endian) */
  EEPROM_PUT8((crc >>  0) & 0x000000FF);
  EEPROM_PUT8((crc >>  8) & 0x000000FF);
  EEPROM_PUT8((crc >> 16) & 0x000000FF);
  EEPROM_PUT8((crc >> 24) & 0x000000FF);
  EEPROM_PUT8(EEPROM_TERMINATOR_ID);

  return MTLK_ERR_OK;
}
#endif /* EEPROM_CRC_ON_DRVHLPR */

/**
  IRBE_FILE_SAVE event handler

  \param irba     Handle to the IRBD interface [I]
  \param context  unused [I]
  \param evt      IRB event GUID [I]
  \param buffer   Handle to the event data [I]
  \param size     Size of event data [I]
*/
static void
_file_save_handler (mtlk_irba_t        *irba,
                    mtlk_handle_t      context,
                    const mtlk_guid_t  *evt,
                    void               *buffer,
                    uint32             size)
{
  struct mtlk_file_save *evt_data = buffer;
  void *data;
  int fd, written = 0, data_size;
  /** \note:
    The last six characters of template must be XXXXXX and these are
    replaced with a string that makes the filename unique. */
  char fname_template[] = "/tmp/tmp_fshlpr.XXXXXX";

  MTLK_UNREFERENCED_PARAM(irba);
  MTLK_UNREFERENCED_PARAM(context);
  MTLK_UNREFERENCED_PARAM(evt);

  MTLK_ASSERT(mtlk_guid_compare(&IRBE_FILE_SAVE, evt) == 0);
  MTLK_ASSERT(NULL != evt_data);

  if (size != (evt_data->size + sizeof(struct mtlk_file_save)))
  {
    ELOG_D("FS HLPR: wrong event received [%u]", size);
    return;
  }

  fd = mkstemp(fname_template);
  if (-1 == fd)
  {
    ELOG_SS("FS HLPR: can't create temporary file '%s' [%s]",
            fname_template, strerror(errno));
    return;
  }

  /* The data to be written into the file is placed after event header */
  data = (evt_data + 1);
  data_size = evt_data->size;
#ifdef EEPROM_CRC_ON_DRVHLPR
  if (MTLK_ERR_OK != _eeprom_crc_append(&data, &data_size))
  {
    ELOG_S("FS HLPR: Failed to append CRC value for EEPROM data '%s'",
            evt_data->fname);

    eeprom_utest_step(CRC_APPEND_FAILED);

    goto OUT;
  }
#endif /* EEPROM_CRC_ON_DRVHLPR */

#if defined(EEPROM_CRC_ON_DRVHLPR) && defined(RUN_EEPROM_CRC_UTEST)
  MTLK_UNREFERENCED_PARAM(written);
  eeprom_utest_write(data, data_size);
#else /* defined(EEPROM_CRC_ON_DRVHLPR) && defined(RUN_EEPROM_CRC_UTEST) */

  written = write(fd, data, data_size);

  close(fd);

  if (-1 == written)
  {
    ELOG_SS("FS HLPR: write error in to the temporary file '%s' [%s]",
            fname_template, strerror(errno));
  }
  else if (written != data_size)
  {
    ELOG_DSD("FS HLPR: can't write '%d' bytes to temporary file '%s' [%d]",
             data_size, fname_template, written);
  }
  else
  {
    char cmd[0x100];

    snprintf(cmd, sizeof(cmd), "%s '%s' '%s'", file_saver, fname_template,
             evt_data->fname);

    ILOG1_S("Run file saver cmd:%s", cmd);
    if (0 != system(cmd))
    {
      ELOG_S("FS HLPR: failed 'file saver' command [%s]", cmd);
    }
  }
#endif /* defined(EEPROM_CRC_ON_DRVHLPR) && defined(RUN_EEPROM_CRC_UTEST) */

#ifdef EEPROM_CRC_ON_DRVHLPR
  if (data != (void*)(evt_data + 1))
  {
    eeprom_utest_step(NEW_BUFFER_CLEANED);

    free(data);
    data = NULL;
  }
OUT:
#endif /* EEPROM_CRC_ON_DRVHLPR */

  if (0 != remove(fname_template))
    ELOG_SS("FS HLPR: can't remove temporary file '%s' [%s]",
           fname_template, strerror(errno));

}

/**
  Initialization routine for file-system helper

  \return
    Handle to the IRB client
*/
static mtlk_handle_t
_fs_hlpr_start (void)
{
  mtlk_irba_handle_t *h =
    mtlk_irba_register(MTLK_IRBA_ROOT, &IRBE_FILE_SAVE, 1,
                       _file_save_handler, 0);

#if defined(EEPROM_CRC_ON_DRVHLPR) && defined(RUN_EEPROM_CRC_UTEST)
  run_eeprom_crc_utest(_file_save_handler);
#endif /* defined(EEPROM_CRC_ON_DRVHLPR) && defined(RUN_EEPROM_CRC_UTEST) */

  return HANDLE_T(h);
}

/**
  Cleanup routine for file-system helper

  \param ctx  Handle to the IRB client [I]
*/
static void
_fs_hlpr_stop (mtlk_handle_t ctx)
{
  mtlk_irba_unregister(MTLK_IRBA_ROOT, HANDLE_T_PTR(mtlk_irba_handle_t, ctx));
}

/**
  File-system helper API
*/
const mtlk_component_api_t
irb_fs_hlpr_api = {
  _fs_hlpr_start,
  NULL,
  _fs_hlpr_stop
};

/**
  Configure shell command to handle saved data, see \ref file_saver

  \param cmd  Null-terminated string with shell command [I]

  \return
    error code or MTLK_ERR_OK on success

  \remark
    The cmd buffer can be modified during rtrim operation
*/
int __MTLK_IFUNC
fs_hlpr_set_saver (char *cmd)
{
  struct stat sb = {0};
  char *end;

  if (NULL == cmd)
  {
    ELOG_V("FS HLPR: missed file saver name");
    return MTLK_ERR_PARAMS;
  }

  /* ltrim */
  while ((' ' == (*cmd)) || ('\t' == (*cmd))) cmd++;

  /* rtrim */
  end = cmd + strlen(cmd);
  while ((end != cmd) && ((' ' == (*end)) || ('\t' == (*end)))) end--;
  *end = '\0';

  if (0 == strlen(cmd))
  {
    ELOG_V("FS HLPR: empty file saver name");
    return MTLK_ERR_PARAMS;
  }

  if (strlen(cmd) > sizeof(file_saver))
  {
    ELOG_D("FS HLPR: file saver name exceed the limit %d", sizeof(file_saver));
    return MTLK_ERR_PARAMS;
  }

  if (0 != stat(cmd, &sb))
  {
    ELOG_S("FS HLPR: file saver '%s' do not exists", cmd);
    return MTLK_ERR_PARAMS;
  }

  if (0 == (sb.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)))
  {
    ELOG_S("FS HLPR: file saver '%s' do not executable", cmd);
    return MTLK_ERR_PARAMS;
  }

  snprintf(file_saver, sizeof(file_saver), "%s", cmd);
  ILOG1_S("Set file_saver as:%s", cmd);

  return MTLK_ERR_OK;
}
