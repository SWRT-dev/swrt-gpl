/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/* $Id$ */

#ifndef __HW_MTLK_CARD_TYPES_H__
#define __HW_MTLK_CARD_TYPES_H__

typedef enum
{
  MTLK_CARD_UNKNOWN = 0x8888, /* Just magics to avoid    */
  MTLK_CARD_FIRST = 0xABCD,   /* accidental coincidences */
  MTLK_CARD_PCIEG6,
  MTLK_CARD_LAST
} mtlk_card_type_t;

typedef uint8 mtlk_card_type_info_t;

/* PCI config area */
#define MTLK_VENDOR_ID_LANTIQ         0x1bef
#define MTLK_V_WAVE500_B11_DEVICE_ID  0x0810
#define INTEL_VENDOR_ID               0x8086
#define INTEL_WAVE600_DEVICE_ID       0x09D0
#define INTEL_WAVE600_D2_DEVICE_ID    0x0D5A
#define UNKNOWN_DEVICE_ID             0x0000

/* WAVE Chip IDs (PCI internal) */
#define WAVE_WAVE500_B11_CHIP_ID      0x0810
#define WAVE_WAVE600_A0_CHIP_ID       0x0900
#define WAVE_WAVE600_B0_CHIP_ID       0x0910
#define WAVE_WAVE600_D2_CHIP_ID       0x0980
#define WAVE_WAVE600_AUX_CHIP_ID      0x0000

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

/* Wave600 HW types and versions */
#define HW_TYPE_HAPS70_G6       0x60 /* FPGA or Emulation (PDXP) */
#define HW_VERS_600_FPGA_5G_D   0x41 /* 5.2 GHz dummy phy */
#define HW_VERS_600_FPGA_5G_R   0x42 /* 5.2 GHz real  phy */
#define HW_VERS_600_FPGA_2G_D   0x43 /* 2.4 GHz dummy phy */
#define HW_VERS_600_FPGA_2G_R   0x44 /* 2.4 GHz real  phy */
#define HW_VERS_600_EMUL_5G_D   0x45 /* 5.2 GHz dummy phy */
#define HW_VERS_600_EMUL_5G_R   0x46 /* 5.2 GHz real  phy */
#define HW_VERS_600_EMUL_2G_D   0x47 /* 2.4 GHz dummy phy */
#define HW_VERS_600_EMUL_2G_R   0x48 /* 2.4 GHz real  phy */
#define HW_VERS_600_FPGA_DB_D   0x49 /* Both    dummy phy */
#define HW_VERS_600_FPGA_DB_R   0x4A /* Both    real  phy */
#define HW_VERS_600_EMUL_DB_D   0x4B /* Both    dummy phy */
#define HW_VERS_600_EMUL_DB_R   0x4C /* Both    real  phy */

#define HW_TYPE_WRX_600         0x61 /* Wave600 ASIC */
#define HW_VERS_600_GPB614_2G   0x41 /* 2.4 GHz */
#define HW_VERS_600_GPB624_5G   0x42 /* 5.2 GHz */
#define HW_VERS_600_GPB654_DB   0x45 /* Both    */
#define HW_VERS_600_85743_5G    0x43 /* 5.2 GHz */
#define HW_VERS_600_85747_5G    0x44 /* 5.2 GHz */
#define HW_VERS_600_EVB600_2G   0x46 /* 2.4 GHz */
#define HW_VERS_600_85747_DB    0x47 /* Both    */
#define HW_VERS_600_85743_DB    0x48 /* Both    */

/*----------------------------------------------------------------------------*/

static inline BOOL _mtlk_card_is_emul (unsigned hw_vers)
{
  return((hw_vers == HW_VERS_600_EMUL_5G_D) ||
         (hw_vers == HW_VERS_600_EMUL_5G_R) ||
         (hw_vers == HW_VERS_600_EMUL_2G_D) ||
         (hw_vers == HW_VERS_600_EMUL_2G_R) ||
         (hw_vers == HW_VERS_600_EMUL_DB_D) ||
         (hw_vers == HW_VERS_600_EMUL_DB_R));
}

/* All odd values of hw_vers are related to Dummy PHY, all even - to Real PHY */
static inline BOOL _mtlk_card_is_phy_dummy (unsigned hw_vers)
{
    return (0 != (1u & hw_vers)); /* odd - dummy, even - real */
}

/*----------------------------------------------------------------------------*/
/* FIXME: hard-coded band support for Gen6 FPGA/ASIC */

static inline BOOL _mtlk_card_g6_asic_is_band_2_4 (unsigned hw_type, unsigned hw_vers)
{
  return((hw_type == HW_TYPE_WRX_600) &&
         ((hw_vers == HW_VERS_600_GPB614_2G) || (hw_vers == HW_VERS_600_EVB600_2G)));
}

static inline BOOL _mtlk_card_g6_asic_is_band_5_2 (unsigned hw_type, unsigned hw_vers)
{
  return((hw_type == HW_TYPE_WRX_600) &&
         ((hw_vers == HW_VERS_600_GPB624_5G)||
          (hw_vers == HW_VERS_600_85743_5G) ||
          (hw_vers == HW_VERS_600_85747_5G)));
}

static inline BOOL _mtlk_card_g6_asic_is_band_both (unsigned hw_type, unsigned hw_vers)
{
  return((hw_type == HW_TYPE_WRX_600) &&
         ((hw_vers == HW_VERS_600_GPB654_DB)||
          (hw_vers == HW_VERS_600_85747_DB) ||
          (hw_vers == HW_VERS_600_85743_DB)));
}

static inline BOOL _mtlk_card_g6_haps_is_band_2_4 (unsigned hw_type, unsigned hw_vers)
{
  return((hw_type == HW_TYPE_HAPS70_G6) &&
         ((hw_vers == HW_VERS_600_FPGA_2G_R) ||
          (hw_vers == HW_VERS_600_FPGA_2G_D) ||
          (hw_vers == HW_VERS_600_EMUL_2G_R) ||
          (hw_vers == HW_VERS_600_EMUL_2G_D)));
}

static inline BOOL _mtlk_card_g6_haps_is_band_5_2 (unsigned hw_type, unsigned hw_vers)
{
  return((hw_type == HW_TYPE_HAPS70_G6) &&
         ((hw_vers == HW_VERS_600_FPGA_5G_R) ||
          (hw_vers == HW_VERS_600_FPGA_5G_D) ||
          (hw_vers == HW_VERS_600_EMUL_5G_R) ||
          (hw_vers == HW_VERS_600_EMUL_5G_D)));
}

static inline BOOL _mtlk_card_g6_haps_is_band_both (unsigned hw_type, unsigned hw_vers)
{
  return((hw_type == HW_TYPE_HAPS70_G6) &&
         ((hw_vers == HW_VERS_600_FPGA_DB_D) ||
          (hw_vers == HW_VERS_600_FPGA_DB_R) ||
          (hw_vers == HW_VERS_600_EMUL_DB_D) ||
          (hw_vers == HW_VERS_600_EMUL_DB_R)));
}

static inline BOOL _mtlk_card_is_band_2_4 (unsigned hw_type, unsigned hw_vers)
{
  return(_mtlk_card_g6_haps_is_band_2_4(hw_type, hw_vers) ||
         _mtlk_card_g6_asic_is_band_2_4(hw_type, hw_vers));
}

static inline BOOL _mtlk_card_is_band_5_2 (unsigned hw_type, unsigned hw_vers)
{
  return(_mtlk_card_g6_haps_is_band_5_2(hw_type, hw_vers) ||
         _mtlk_card_g6_asic_is_band_5_2(hw_type, hw_vers));
}

static inline BOOL _mtlk_card_is_band_both (unsigned hw_type, unsigned hw_vers)
{
  return(_mtlk_card_g6_asic_is_band_both(hw_type, hw_vers) ||
         _mtlk_card_g6_haps_is_band_both(hw_type, hw_vers));
}

/*----------------------------------------------------------------------------*/

static inline BOOL _mtlk_card_is_asic (unsigned hw_type)
{
  return(hw_type != HW_TYPE_HAPS70_G6);
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
  { UNKNOWN_DEVICE_ID,           NULL   }
};
#else /* SAFE_PLACE_TO_DEFINE_CHIP_INFO */
extern mtlk_chip_info_t const mtlk_chip_info[];
#endif/* SAFE_PLACE_TO_DEFINE_CHIP_INFO */

#undef SAFE_PLACE_TO_DEFINE_CHIP_INFO

static __INLINE mtlk_chip_info_t const *
mtlk_chip_info_get(uint32 id)
{
  mtlk_chip_info_t const *p;

  for(p = mtlk_chip_info; UNKNOWN_DEVICE_ID != p->id; p++)
  {
    if (p->id == id){
      return p;
    }
  }
  
  mtlk_osal_emergency_print("ChipID is not found");
  return NULL;
}

/* Number of radios the card supports */
#define WAVE_CARD_RADIO_NUM_GEN5     1
#define WAVE_CARD_RADIO_NUM_GEN6     2
#define WAVE_CARD_RADIO_NUM_MAX      (MAX(WAVE_CARD_RADIO_NUM_GEN5, WAVE_CARD_RADIO_NUM_GEN6))
#define WAVE_CARD_RADIO_NUM_MIN      (MIN(WAVE_CARD_RADIO_NUM_GEN5, WAVE_CARD_RADIO_NUM_GEN6))
/* Number of radios driver supports depending on band config */
#define WAVE_CARD_RADIO_NUM_CDB      WAVE_CARD_RADIO_NUM_MAX
#define WAVE_CARD_RADIO_NUM_SB       WAVE_CARD_RADIO_NUM_MIN

static __INLINE BOOL
wave_card_num_of_radios_is_valid(unsigned num)
{
  return ((WAVE_CARD_RADIO_NUM_MIN <= num) &&
          (WAVE_CARD_RADIO_NUM_MAX >= num));
}

static __INLINE mtlk_error_t wave_card_radio_number_get (mtlk_card_type_t hw_type, unsigned *radio_number)
{
  switch (hw_type) {
    case MTLK_CARD_PCIEG6:
      *radio_number = 2;
      break;
    default:
      return MTLK_ERR_PARAMS;
      break;
  }
  return MTLK_ERR_OK;
}

#endif /* __HW_MTLK_CARD_TYPES_H__ */
