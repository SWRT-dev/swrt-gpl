/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/* $Id$ */

#ifndef __HW_MTLK_CARD_TYPES_H__
#define __HW_MTLK_CARD_TYPES_H__

typedef enum
{
  MTLK_CARD_UNKNOWN = 0x8888, /* Just magics to avoid    */
  MTLK_CARD_FIRST = 0xABCD,   /* accidental coincidences */
  MTLK_CARD_PCIEG6_A0_B0,
  MTLK_CARD_PCIEG6_D2,
  MTLK_CARD_PCIEG7,
  MTLK_CARD_LAST
} mtlk_card_type_t;

typedef uint8 mtlk_card_type_info_t;

static __INLINE BOOL
wave_known_card_type(mtlk_card_type_t x)
{
  return (x > MTLK_CARD_FIRST) && (x < MTLK_CARD_LAST);
}

/* PCI config area */
#define PCI_VENDOR_ID_INTEL               0x8086
#define PCI_VENDOR_ID_INTEL_WAVE600       0x09D0
#define PCI_VENDOR_ID_INTEL_WAVE600_D2    0x0D5A

#define PCI_VENDOR_ID_MAXLINEAR           0x17E6
#define PCI_DEVICE_ID_MAXLINEAR_WAVE700   0x1234 /* TBD */

#define UNKNOWN_DEVICE_ID                 0x0000


/* WAVE Chip IDs (PCI internal) */
#define WAVE_WAVE600_AUX_CHIP_ID      0x0000
#define WAVE_WAVE600_A0_CHIP_ID       0x0900
#define WAVE_WAVE600_B0_CHIP_ID       0x0910
#define WAVE_WAVE600_D2_CHIP_ID       0x0980
#define WAVE_WAVE700_CHIP_ID          0x0A00

static inline BOOL _chipid_is_gen6_aux (uint32 id)
{
  return (id == WAVE_WAVE600_AUX_CHIP_ID);
}

static inline BOOL _chipid_is_gen6_a0 (uint32 id)
{
  return (id == WAVE_WAVE600_A0_CHIP_ID);
}

static inline BOOL _chipid_is_gen6_b0 (uint32 id)
{
  return (id == WAVE_WAVE600_B0_CHIP_ID);
}

static inline BOOL _chipid_is_gen6_d2 (uint32 id)
{
  return (id == WAVE_WAVE600_D2_CHIP_ID);
}

static inline BOOL _chipid_is_gen6 (uint32 id)
{
  return (_chipid_is_gen6_a0(id) || _chipid_is_gen6_b0(id) || _chipid_is_gen6_d2(id));
}

static inline BOOL _chipid_is_gen7 (uint32 id)
{
  return (id == WAVE_WAVE700_CHIP_ID);
}

static inline BOOL _chipid_is_gen6_d2_or_gen7 (uint32 id)
{
  return (_chipid_is_gen6_d2(id) || _chipid_is_gen7(id));
}

/* Wave600 HW types and versions */
#define HW_TYPE_HAPS70_G6              0x60 /* FPGA or Emulation (PDXP) */
#define HW_VERS_600_FPGA_5G_D          0x41 /* 5.2 GHz      dummy phy */
#define HW_VERS_600_FPGA_5G_R          0x42 /* 5.2 GHz      real  phy */
#define HW_VERS_600_FPGA_2G_D          0x43 /* 2.4 GHz      dummy phy */
#define HW_VERS_600_FPGA_2G_R          0x44 /* 2.4 GHz      real  phy */
#define HW_VERS_600_EMUL_5G_D          0x45 /* 5.2 GHz      dummy phy */
#define HW_VERS_600_EMUL_5G_R          0x46 /* 5.2 GHz      real  phy */
#define HW_VERS_600_EMUL_2G_D          0x47 /* 2.4 GHz      dummy phy */
#define HW_VERS_600_EMUL_2G_R          0x48 /* 2.4 GHz      real  phy */
#define HW_VERS_600_FPGA_DB_D          0x49 /* Both         dummy phy */
#define HW_VERS_600_FPGA_DB_R          0x4A /* Both         real  phy */
#define HW_VERS_600_EMUL_DB_D          0x4B /* Both         dummy phy */
#define HW_VERS_600_EMUL_DB_R          0x4C /* Both         real  phy */
#define HW_VERS_600_EMUL_wrx665_6G_D   0x4D /* 6 GHz new RF dummy phy */
#define HW_VERS_600_EMUL_wrx665_6G_R   0x4E /* 6 GHz new RF real  phy */
#define HW_VERS_600_FPGA_wrx665_6G_D   0x4F /* 6 GHz new RF dummy phy */
#define HW_VERS_600_FPGA_wrx665_6G_R   0x52 /* 6 GHz new RF real  phy */

#define HW_TYPE_WRX_600                0x61 /* Wave600 ASIC   */
#define HW_VERS_600_GPB614_2G          0x41 /* 2.4 GHz        */
#define HW_VERS_600_GPB624_5G          0x42 /* 5.2 GHz        */
#define HW_VERS_600_GPB654_DB          0x45 /* Both           */
#define HW_VERS_600_85743_5G           0x43 /* 5.2 GHz        */
#define HW_VERS_600_85747_5G           0x44 /* 5.2 GHz        */
#define HW_VERS_600_EVB600_2G          0x46 /* 2.4 GHz        */
#define HW_VERS_600_85747_DB           0x47 /* Both           */
#define HW_VERS_600_85743_DB           0x48 /* Both           */
#define HW_VERS_600_GPB_DFS_DB         0x4A /* Both           */
#define HW_VERS_600_GPB664             0x4E /* 6 GHz          */
#define HW_VERS_600_GPB615_wrx654_2G   0x53 /* 2.4 GHz old RF */
#define HW_VERS_600_GPB665_wrx654_5G   0x54 /* 5.2 GHz old RF */
#define HW_VERS_600_GPB665_wrx665_6G   0x55 /* 6   GHz new RF */
#define HW_VERS_600_GPB665_wrx665_5G   0x56 /* 5.2 GHz new RF */
#define HW_VERS_600_GPB615_wrx654_2G_Q 0x57 /* 2.4 GHz old RF */
#define HW_VERS_600_GPB665_wrx665_5G_Q 0x58 /* 5.2 GHz new RF */
#define HW_VERS_600_GPB665_wrx665_6G_Q 0x59 /* 6   GHz new RF */

/* Wave700 HW types and versions */
#define HW_TYPE_HAPS80_G7              0x60 /* FPGA or Emulation (PDXP) */
/* RF wave700  (wrx_700) */
#define HW_VERS_700_FPGA_5G_D          0x41 /* 5.2 GHz      dummy phy */
#define HW_VERS_700_FPGA_5G_R          0x42 /* 5.2 GHz      real  phy */
#define HW_VERS_700_FPGA_2G_D          0x43 /* 2.4 GHz      dummy phy */
#define HW_VERS_700_FPGA_2G_R          0x44 /* 2.4 GHz      real  phy */
#define HW_VERS_700_EMUL_5G_D          0x45 /* 5.2 GHz      dummy phy */
#define HW_VERS_700_EMUL_5G_R          0x46 /* 5.2 GHz      real  phy */
#define HW_VERS_700_EMUL_2G_D          0x47 /* 2.4 GHz      dummy phy */
#define HW_VERS_700_EMUL_2G_R          0x48 /* 2.4 GHz      real  phy */
#define HW_VERS_700_FPGA_DB_D          0x49 /* Both         dummy phy */
#define HW_VERS_700_FPGA_DB_R          0x4A /* Both         real  phy */
#define HW_VERS_700_EMUL_DB_D          0x4B /* Both         dummy phy */
#define HW_VERS_700_EMUL_DB_R          0x4C /* Both         real  phy */
#define HW_VERS_700_EMUL_6G_D          0x4D /* 6 GHz        dummy phy */
#define HW_VERS_700_EMUL_6G_R          0x4E /* 6 GHz        real  phy */
#define HW_VERS_700_FPGA_6G_D          0x4F /* 6 GHz        dummy phy */
#define HW_VERS_700_FPGA_6G_R          0x50 /* 6 GHz        real  phy */

/* RF wave700 mixed (wrx_700_mixed) */
#define HW_VERS_700_MIX_FIRST          HW_VERS_700_MIX_FPGA_5G_D
#define HW_VERS_700_MIX_FPGA_5G_D      0x51 /* 5.2 GHz      dummy phy */
#define HW_VERS_700_MIX_FPGA_5G_R      0x52 /* 5.2 GHz      real  phy */
#define HW_VERS_700_MIX_FPGA_2G_D      0x53 /* 2.4 GHz      dummy phy */
#define HW_VERS_700_MIX_FPGA_2G_R      0x54 /* 2.4 GHz      real  phy */
#define HW_VERS_700_MIX_EMUL_5G_D      0x55 /* 5.2 GHz      dummy phy */
#define HW_VERS_700_MIX_EMUL_5G_R      0x56 /* 5.2 GHz      real  phy */
#define HW_VERS_700_MIX_EMUL_2G_D      0x57 /* 2.4 GHz      dummy phy */
#define HW_VERS_700_MIX_EMUL_2G_R      0x58 /* 2.4 GHz      real  phy */
#define HW_VERS_700_MIX_FPGA_DB_D      0x59 /* Both         dummy phy */
#define HW_VERS_700_MIX_FPGA_DB_R      0x5A /* Both         real  phy */
#define HW_VERS_700_MIX_EMUL_DB_D      0x5B /* Both         dummy phy */
#define HW_VERS_700_MIX_EMUL_DB_R      0x5C /* Both         real  phy */
#define HW_VERS_700_MIX_EMUL_6G_D      0x5D /* 6 GHz        dummy phy */
#define HW_VERS_700_MIX_EMUL_6G_R      0x5E /* 6 GHz        real  phy */
#define HW_VERS_700_MIX_FPGA_6G_D      0x5F /* 6 GHz        dummy phy */
#define HW_VERS_700_MIX_FPGA_6G_R      0x60 /* 6 GHz        real  phy */
#define HW_VERS_700_MIX_LAST           HW_VERS_700_MIX_FPGA_6G_R

/*----------------------------------------------------------------------------*/

static inline BOOL
wave_card_g7_is_rf_mixed (unsigned hw_vers)
{
  return ((HW_VERS_700_MIX_FIRST <= hw_vers) && (hw_vers <= HW_VERS_700_MIX_LAST));
}

static inline BOOL _mtlk_card_is_emul (unsigned hw_vers)
{
  return((hw_vers == HW_VERS_600_EMUL_5G_D) ||
         (hw_vers == HW_VERS_600_EMUL_5G_R) ||
         (hw_vers == HW_VERS_600_EMUL_2G_D) ||
         (hw_vers == HW_VERS_600_EMUL_2G_R) ||
         (hw_vers == HW_VERS_600_EMUL_DB_D) ||
         (hw_vers == HW_VERS_600_EMUL_DB_R) ||
         (hw_vers == HW_VERS_600_EMUL_wrx665_6G_D) ||
         (hw_vers == HW_VERS_600_EMUL_wrx665_6G_R) ||
         (hw_vers == HW_VERS_700_EMUL_5G_D) ||
         (hw_vers == HW_VERS_700_EMUL_5G_R) ||
         (hw_vers == HW_VERS_700_EMUL_2G_D) ||
         (hw_vers == HW_VERS_700_EMUL_2G_R) ||
         (hw_vers == HW_VERS_700_EMUL_DB_D) ||
         (hw_vers == HW_VERS_700_EMUL_DB_R) ||
         (hw_vers == HW_VERS_700_EMUL_6G_D) ||
         (hw_vers == HW_VERS_700_EMUL_6G_R) ||
         (hw_vers == HW_VERS_700_MIX_EMUL_5G_D) ||
         (hw_vers == HW_VERS_700_MIX_EMUL_5G_R) ||
         (hw_vers == HW_VERS_700_MIX_EMUL_2G_D) ||
         (hw_vers == HW_VERS_700_MIX_EMUL_2G_R) ||
         (hw_vers == HW_VERS_700_MIX_EMUL_DB_D) ||
         (hw_vers == HW_VERS_700_MIX_EMUL_DB_R) ||
         (hw_vers == HW_VERS_700_MIX_EMUL_6G_D) ||
         (hw_vers == HW_VERS_700_MIX_EMUL_6G_R));
}

/* All odd values of hw_vers are related to Dummy PHY, all even - to Real PHY */
static inline BOOL _mtlk_card_is_phy_dummy (unsigned hw_vers)
{
    return (0 != (1u & hw_vers)); /* odd - dummy, even - real */
}

/*----------------------------------------------------------------------------*/
/* FIXME: hard-coded band support for Gen6 FPGA/ASIC */

static inline BOOL _mtlk_card_g6_asic_is_band_6_wrx665 (uint32 hw_type, uint32 hw_vers)
{
  return((hw_type == HW_TYPE_WRX_600) &&
          ((hw_vers == HW_VERS_600_GPB665_wrx665_6G_Q) ||
           (hw_vers == HW_VERS_600_GPB665_wrx665_6G)));
}

static inline BOOL _mtlk_card_g6_asic_is_band_5_wrx665 (uint32 hw_type, uint32 hw_vers)
{
  return((hw_type == HW_TYPE_WRX_600) &&
         ((hw_vers == HW_VERS_600_GPB665_wrx665_5G_Q) ||
          (hw_vers == HW_VERS_600_GPB665_wrx665_5G)));
}

static inline BOOL _mtlk_card_g6_haps_is_band_6_wrx665 (uint32 hw_type, uint32 hw_vers)
{
  return((hw_type == HW_TYPE_HAPS70_G6) &&
         ((hw_vers == HW_VERS_600_FPGA_wrx665_6G_R) ||
          (hw_vers == HW_VERS_600_FPGA_wrx665_6G_D) ||
          (hw_vers == HW_VERS_600_EMUL_wrx665_6G_R) ||
          (hw_vers == HW_VERS_600_EMUL_wrx665_6G_D)));
}

static inline BOOL _mtlk_card_is_wrx665 (uint32 hw_type, uint32 hw_vers)
{
  return(_mtlk_card_g6_haps_is_band_6_wrx665(hw_type, hw_vers) ||
         _mtlk_card_g6_asic_is_band_5_wrx665(hw_type, hw_vers) ||
         _mtlk_card_g6_asic_is_band_6_wrx665(hw_type, hw_vers));
}
/*----------------------------------------------------------------------------*/

static inline BOOL _mtlk_card_is_asic (unsigned hw_type)
{
  return((hw_type != HW_TYPE_HAPS70_G6) && (hw_type != HW_TYPE_HAPS80_G7));
}

typedef struct {
  uint32 id;
  char   *name;
} mtlk_chip_info_t;

#if defined(SAFE_PLACE_TO_DEFINE_CHIP_INFO)
mtlk_chip_info_t const mtlk_chip_info[] = {
  { WAVE_WAVE600_A0_CHIP_ID,    "gen6"  }, /* PCIE */
  { WAVE_WAVE600_B0_CHIP_ID,    "gen6b" }, /* PCIE */
  { WAVE_WAVE600_D2_CHIP_ID,    "gen6d2"}, /* PCIE */
  { WAVE_WAVE700_CHIP_ID,       "gen7"  }, /* PCIE */
  { 0,                           NULL   }
};
#else /* SAFE_PLACE_TO_DEFINE_CHIP_INFO */
extern mtlk_chip_info_t const mtlk_chip_info[];
#endif/* SAFE_PLACE_TO_DEFINE_CHIP_INFO */

#undef SAFE_PLACE_TO_DEFINE_CHIP_INFO

static __INLINE mtlk_chip_info_t const *
mtlk_chip_info_get(uint32 id)
{
  mtlk_chip_info_t const *p;

  for(p = mtlk_chip_info; 0 != p->id; p++)
  {
    if (p->id == id) {
      return p;
    }
  }
  
  mtlk_osal_emergency_print("ChipID 0x%04X is not found", id);
  return NULL;
}

/* Number of radios the card supports */
#define WAVE_CARD_RADIO_NUM_MAX_G6   2
#define WAVE_CARD_RADIO_NUM_MAX_G7   2
#define WAVE_CARD_RADIO_NUM_MAX      (MAX(WAVE_CARD_RADIO_NUM_MAX_G6, WAVE_CARD_RADIO_NUM_MAX_G7))
#define WAVE_CARD_RADIO_NUM_MIN      1

static __INLINE BOOL
wave_card_num_of_radios_is_valid(unsigned num)
{
  return ((WAVE_CARD_RADIO_NUM_MIN <= num) &&
          (WAVE_CARD_RADIO_NUM_MAX >= num));
}

/*----------------------------------------------------------------------------*/

#include "mtlk_card_selector.h"

static __INLINE mtlk_error_t wave_card_max_radio_number_get (mtlk_card_type_t card_type, unsigned *radio_number)
{
  switch (card_type) {
    __CASE_PCIEG6
      *radio_number = WAVE_CARD_RADIO_NUM_MAX_G6;
      break;
    __CASE_PCIEG7
      *radio_number = WAVE_CARD_RADIO_NUM_MAX_G7;
      break;
    default:
      return MTLK_ERR_PARAMS;
      break;
  }
  return MTLK_ERR_OK;
}

#endif /* __HW_MTLK_CARD_TYPES_H__ */
