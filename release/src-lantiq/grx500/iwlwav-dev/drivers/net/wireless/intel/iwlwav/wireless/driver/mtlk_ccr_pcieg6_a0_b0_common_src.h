/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#define LOG_LOCAL_GID   GID_CCR_PCIE
#define LOG_LOCAL_FID   1

/*------------------------ ABB and FW PHY libraries ------------------------*/


/* PHY registers access */

#define RegAccess_Read(ccr_mem, addr, value)    _CCR_READ_REG_VALUE_(ccr_mem, addr, value)
#define RegAccess_Write(ccr_mem, addr, value)   _CCR_WRITE_REG_VALUE_(ccr_mem, addr, value)

/* "phy_access_definitions.h" */
#define BYTE_OFFSET_SHIFT_TO_WORD       2 /* FIXME: or 4 ?*/

#define PHY_READ_REG(ccr_mem, base_addr, offset, value) \
        MTLK_STATIC_ASSERT(0 == offset); \
        RegAccess_Read(ccr_mem, base_addr, value)
     /* RegAccess_Read(ccr_mem, base_addr + (offset << BYTE_OFFSET_SHIFT_TO_WORD), value) */

#define PHY_WRITE_REG(ccr_mem, base_addr, offset, value) \
        RegAccess_Write(ccr_mem, base_addr + (offset << BYTE_OFFSET_SHIFT_TO_WORD), value)

/* "wav600_A0/lab/api_wrapper/interface_abb.h" */
#define ABB_DIR_CTRL_WRITE(ccr_mem, port, value)          _pcie_abb_port_write(ccr_mem, port, value)
#define ABB_DIR_CTRL_WRITES(ccr_mem, port, width, value)  _pcie_abb_port_write(ccr_mem, port, value)

/* Only ABB_CENTRAL supported */
#define ABB_CENTRAL     16 /* 1 0000 */

//#define ABB_FCSI_RMW(ccr_mem, antenna, address, mask, value)   ABB_vFcsiRmw(antenna, address, mask, value)
//#define ABB_FCSI_READ(ccr_mem, antenna, address, data)         ABB_usFcsiRead(antenna, address, data)
#define ABB_FCSI_WRITE(ccr_mem, antenna, address, value) \
        MTLK_STATIC_ASSERT(ABB_CENTRAL == antenna); \
        _phy_lib_writeToAfeRegCentral(ccr_mem, address, value)

/* Apply bitfield value to the init value, and write result */
#define _ABB_FCSI_INIT_SET_(ccr_mem, reg_name, info, value) \
  { uint32 reg_val = reg_name ## __INIT; \
    MTLK_BFIELD_SET(reg_val, info, value); \
    ABB_FCSI_WRITE(ccr_mem, ABB_CENTRAL, reg_name, reg_val); \
  }



typedef enum {

  lcpll_pup_d2a,
  lcpll_resn_d2a,
  pd_clk_sc_d2a,
  op_freq_sel_sc_d2a,
  lcpll_div_d2a,
  spare_config_i_d2a,

  FCSI2_RST_N_0,
  FCSI2_RST_N_1,
  FCSI2_RST_N_2,
  FCSI2_RST_N_3,
  FCSI2_RST_N_C,

} _pcie_abb_port_e ;


#define FCSI_CENTRAL_BUSY_WAIT_CNTR     10

static void
_phy_lib_FCSI_WAIT_FOR_CENTRAL_BUSY_FIELD (void *ccr_mem, uint32 exp_val, BOOL *success)
{
    uint32 reg_val = 0;
    int    cntr = FCSI_CENTRAL_BUSY_WAIT_CNTR;

    /* _CCR_DBG_FUNC_; */

    *success = FALSE;
    do {
        /* RegPhyRxTdIfPhyRxtdIf136_u */
        _CCR_READ_REG_VALUE_(ccr_mem, PHY_RX_TD_IF_PHY_RXTD_IF136, reg_val);

#if CCR_REG_DEBUG
        /* emulate busy for 2 times */
        if (cntr > (FCSI_CENTRAL_BUSY_WAIT_CNTR - 2)) continue;
#endif

        if (exp_val == MTLK_BFIELD_GET(reg_val, PHY_RX_TD_IF_PHY_RXTD_IF136_FCSI_BUSY))
        {
            *success = TRUE;
            break;
        }
    } while (--cntr > 0);

#if CCR_REG_DEBUG
    *success = TRUE;
#endif
}

static void
_phy_lib_FCSI_READ_TRANSACTION_CENTRAL (void *ccr_mem, uint16 ms_access, uint16 rf_access,
                                        uint16 addr, uint16 *rd_data, BOOL *success)
{
    uint32 reg_val;

    _phy_lib_FCSI_WAIT_FOR_CENTRAL_BUSY_FIELD(ccr_mem, 0, success);
    if (*success == TRUE)
    {
        /* RegPhyRxTdIfPhyRxtdIfFcsiAccess_u */
        reg_val = MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS_FCSI_GR_ADDR,       addr,       uint32)|
                  MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS_FCSI_GR_RF_ACCESS,  rf_access,  uint32)|
                  MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS_FCSI_GR_MS_ACCESS,  ms_access,  uint32)|
                  MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS_FCSI_GR_RD_WR,   1 /* read */,  uint32)|
                  MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS_FCSI_GR_MASK, 0x10 /*Central*/, uint32);

        PHY_WRITE_REG(ccr_mem, PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS, 0, reg_val);

        _phy_lib_FCSI_WAIT_FOR_CENTRAL_BUSY_FIELD(ccr_mem, 0, success);
        PHY_READ_REG(ccr_mem, PHY_RX_TD_IF_PHY_RXTD_IF131, 0, reg_val);

        *rd_data = MTLK_BFIELD_GET(reg_val, PHY_RX_TD_IF_PHY_RXTD_IF131_FCSI_RD_DATA);

#ifdef CPTCFG_IWLWAV_DEBUG
    mtlk_osal_emergency_print("(%4d) FCSI_READ  ms %u, rf %u, addr 0x%04X, data 0x%04X",
                              __LINE__, ms_access, rf_access, addr, *rd_data);
#endif
    }
}

static void
_phy_lib_FCSI_WRITE_TRANSACTION_CENTRAL (void *ccr_mem, uint16 ms_access, uint16 rf_access,
                                         uint16 addr, uint16 data, BOOL *success)
{

    uint32 reg_val;

#ifdef CPTCFG_IWLWAV_DEBUG
    mtlk_osal_emergency_print("(%4d) FCSI_WRITE ms %u, rf %u, addr 0x%04X, data 0x%04X",
                              __LINE__, ms_access, rf_access, addr, data);
#endif
    _phy_lib_FCSI_WAIT_FOR_CENTRAL_BUSY_FIELD(ccr_mem, 0, success);
    if (*success == TRUE)
    {
        /* RegPhyRxTdIfPhyRxtdIfFcsiAccess_u */
        reg_val = MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS_FCSI_GR_ADDR,       addr,       uint32)|
                  MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS_FCSI_GR_RF_ACCESS,  rf_access,  uint32)|
                  MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS_FCSI_GR_MS_ACCESS,  ms_access,  uint32)|
                  MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS_FCSI_GR_RD_WR,   0 /* write */, uint32)|
                  MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS_FCSI_GR_MASK, 0x10 /*Central*/, uint32);

        PHY_WRITE_REG(ccr_mem, PHY_RX_TD_IF_PHY_RXTD_IF12C, 0,
             MTLK_BFIELD_VALUE(PHY_RX_TD_IF_PHY_RXTD_IF12C_FCSI_GR_WR_DATA, data, uint32));

        PHY_WRITE_REG(ccr_mem, PHY_RX_TD_IF_PHY_RXTD_IF_FCSI_ACCESS, 0, reg_val);
    }
}

static void
_phy_lib_writeToAfeRegCentral (void *ccr_mem, uint32 regAddr, uint16 regValue)
{
    BOOL    success;
    uint16  centralVal = 0;
#ifdef CONFIG_WAVE_DEBUG
    mtlk_osal_emergency_print("(%4d) wrToAfeRegC addr 0x%04X  value 0x%04X",
                              __LINE__, regAddr, regValue);
#endif

        _phy_lib_FCSI_READ_TRANSACTION_CENTRAL(ccr_mem, 1, 0, regAddr, &centralVal, &success);
        centralVal = regValue; /* whole word */
        _phy_lib_FCSI_WRITE_TRANSACTION_CENTRAL(ccr_mem, 1, 0, regAddr, centralVal, &success);
}

static void
_phy_fcsi_ctrl_antenna (void *ccr_mem, uint32 ant_mask, uint32 ant_num, uint32 value)
{
  uint32 reg_val = 0;
  uint32 reg_addr = PHY_RXTD_ANT0_PHY_RXTD_ANT_REG4B;

  if (0 == ((1 << ant_num) & ant_mask))
    return;

  reg_addr = PHY_RXTD_ANT0_PHY_RXTD_ANT_REG4B;
  switch (ant_num)
  {
    case 0: break; /* already */
    case 1: reg_addr += (B0_PHY_RXTD_ANT1_OFFSET_FROM_BAR0 - B0_PHY_RXTD_ANT0_OFFSET_FROM_BAR0); break;
    case 2: reg_addr += (B0_PHY_RXTD_ANT2_OFFSET_FROM_BAR0 - B0_PHY_RXTD_ANT0_OFFSET_FROM_BAR0); break;
    case 3: reg_addr += (B0_PHY_RXTD_ANT3_OFFSET_FROM_BAR0 - B0_PHY_RXTD_ANT0_OFFSET_FROM_BAR0); break;
    default:
      MTLK_ASSERT(FALSE);
      return;
  }

  _CCR_READ_REG_VALUE_(ccr_mem, reg_addr, reg_val);
  MTLK_BFIELD_SET(reg_val, PHY_RXTD_ANT0_PHY_RXTD_ANT_REG4B_FCSI_MS_RESET_ANT_N, value);
  MTLK_BFIELD_SET(reg_val, PHY_RXTD_ANT0_PHY_RXTD_ANT_REG4B_FCSI_RF_RESET_N, value);
  _CCR_WRITE_REG_VALUE_(ccr_mem, reg_addr, reg_val);
}

static void
_pcie_abb_port_write (void *ccr_mem, _pcie_abb_port_e port, uint32 value)
{
  uint32 ant_mask = 0x0F; /* TBD */

  MTLK_ASSERT(NULL != ccr_mem);

  switch (port)
  {
    case lcpll_pup_d2a:
      _CCR_RMW_REG_FIELD_(ccr_mem, 1, SYS_IF_UM_PLL_CTL,  SYS_IF_UM_PLL_CTL_LCPLL_PUP_D2A,    value);
      break;
    case lcpll_resn_d2a:
      _CCR_RMW_REG_FIELD_(ccr_mem, 1, SYS_IF_UM_PLL_CTL,  SYS_IF_UM_PLL_CTL_LCPLL_RESETN_D2A, value);
      break;
    case pd_clk_sc_d2a:
      _CCR_RMW_REG_FIELD_(ccr_mem, 1, SYS_IF_UM_PLL_EN,   SYS_IF_UM_PLL_EN_WLAN_CLK_PDN_D2A, value);
      break;
    case op_freq_sel_sc_d2a:
      _CCR_RMW_REG_FIELD_(ccr_mem, 1, SYS_IF_UM_PLL_DIV,  SYS_IF_UM_PLL_DIV_WLAN_CLK_DIV_D2A, value);
      break;
    case lcpll_div_d2a: /* FIXME: nothing */
      break;
    case spare_config_i_d2a:
      _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_GEN_REGS_SPARE_CONFIG_D2A,
                                      MAC_GEN_REGS_SPARE_CONFIG_D2A_SPARE_CONFIG_D2A, value);
      break;

    case FCSI2_RST_N_0:
      _phy_fcsi_ctrl_antenna(ccr_mem, ant_mask, 0, value);
      break;
    case FCSI2_RST_N_1:
      _phy_fcsi_ctrl_antenna(ccr_mem, ant_mask, 1, value);
      break;
    case FCSI2_RST_N_2:
      _phy_fcsi_ctrl_antenna(ccr_mem, ant_mask, 2, value);
      break;
    case FCSI2_RST_N_3:
      _phy_fcsi_ctrl_antenna(ccr_mem, ant_mask, 3, value);
      break;

    case FCSI2_RST_N_C:
      _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_GEN_REGS_FCSI_CTL, MAC_GEN_REGS_FCSI_CTL_FCSI_MS_RESET_QC_D2A, value);
      _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_GEN_REGS_FCSI_CTL, MAC_GEN_REGS_FCSI_CTL_FCSI_RF_RESET_QC_D2A, value);
      break;

    default: /* should not be here */
      MTLK_ASSERT(FALSE);
      break;
  }
}

static void
_abb_central_init_pll (void *ccr_mem)
{
  _CCR_DBG_FUNC_;

  /* program reset values */
  ABB_DIR_CTRL_WRITE (ccr_mem, lcpll_pup_d2a,         0x0);
  ABB_DIR_CTRL_WRITE (ccr_mem, lcpll_resn_d2a,        0x0);
  ABB_DIR_CTRL_WRITE (ccr_mem, pd_clk_sc_d2a,         0x1);
  ABB_DIR_CTRL_WRITES(ccr_mem, op_freq_sel_sc_d2a, 2, 0x0); /* FIXME: reset value 1 */
  ABB_DIR_CTRL_WRITE (ccr_mem, lcpll_div_d2a,         0x0); /* FIXME: nothing */
  ABB_DIR_CTRL_WRITES(ccr_mem, spare_config_i_d2a, 8, 0x00);

  /* lcpll powerup */
  ABB_DIR_CTRL_WRITE (ccr_mem, lcpll_pup_d2a,        0x1);
  ABB_DIR_CTRL_WRITE (ccr_mem, lcpll_resn_d2a,       0x1);
}

#define ABB_CENTRAL_LOCK_PLL_WAIT_CNTR  100

static int
_abb_central_lock_pll (void *ccr_mem)
{
  uint32 reg_val = 0;
  int    cntr = ABB_CENTRAL_LOCK_PLL_WAIT_CNTR;
  int    res  = MTLK_ERR_HW;

  _CCR_DBG_FUNC_;

  /* Wait for PLL lock */
  do {
    mtlk_osal_msleep(1);
    _CCR_READ_REG_VALUE_(ccr_mem, SYS_IF_UM_PLL_LOCK, reg_val);

#if CCR_REG_DEBUG
    /* emulate non lock for 3 times */
    reg_val = (cntr > (ABB_CENTRAL_LOCK_PLL_WAIT_CNTR - 3)) ? 0 : 1;
#endif

    if (0 != MTLK_BFIELD_GET(reg_val, SYS_IF_UM_PLL_LOCK_PLL_LOCK)) {
      res = MTLK_ERR_OK; /* locked */
      break;
    }
  }
  while(--cntr > 0);

  return res;
}

static void
_abb_central_init_clkd (void *ccr_mem)
{
  _CCR_DBG_FUNC_;

  /* affects: 640M and 160M clk_sc_a2d and stb_sc_a2d and
           abb internal clock distribution
     CTRLSYNCDL=100=default
   */
  ABB_DIR_CTRL_WRITE(ccr_mem, pd_clk_sc_d2a, 0x0);
}

static void
_abb_cpu_clock_select (void *ccr_mem, uint32 clk_sel)
{
  _CCR_DBG_FUNC_ARG1(clk_sel);

  switch (clk_sel)
  {
    default:
      MTLK_ASSERT(!"Wrong CLOCK_SELECT config");
      break;
    case SYS_IF_UM_CLOCK_SELECT_CPU_CLOCK_SELECT_XO:
    case SYS_IF_UM_CLOCK_SELECT_CPU_CLOCK_SELECT_PLL:
      __pcie_open_secure_write_register_upper(ccr_mem);
      _CCR_WRITE_REG_VALUE_(ccr_mem, SYS_IF_UM_CLOCK_SELECT, clk_sel);
      break;
  }
}

static __INLINE void
__abb_api_central_trim_bias (void *ccr_mem, uint32 vref, uint32 iptat, uint32 ibias)
{
    ABB_FCSI_WRITE(ccr_mem, ABB_CENTRAL, CENTRAL_FCSI_ICENTBIAS0,
        (MTLK_BFIELD_VALUE(CENTRAL_FCSI_ICENTBIAS0_BIASTRIM, ibias, uint32)|
         MTLK_BFIELD_VALUE(CENTRAL_FCSI_ICENTBIAS0_PTATTRIM, iptat, uint32)|
         MTLK_BFIELD_VALUE(CENTRAL_FCSI_ICENTBIAS0_VREFTRIM, vref,  uint32)));
}

static void
_abb_central_fusing (void *ccr_mem, wave_efuse_afe_data_v6_t *afe, size_t size)
{
    uint32  trim_indicator = 0;
    uint32  ibiastrim, iptattrim, vreftrim;
    uint32  ldo_central_pll1v8, ldo_central_pllref, ldo_central_pllpost;
    uint32  ldo_central_clkd, ldo_central_clkin;

    _CCR_DBG_FUNC_ARG1((uint32)size);

    MTLK_ASSERT(NULL != afe);

    /* Function is called after FCSI reset! */

    if (size) {
      if (AFE_CALIBRATION_DATA_SIZE_GEN6 != size) {
        WLOG_DD("Wrong AFE data size %u (%u expected)", size, AFE_CALIBRATION_DATA_SIZE_GEN6);
      } else {
        trim_indicator = afe->trim_indicator;
      }
    }

    if (trim_indicator) {
      ILOG0_V("Trim by AFE data");
      ibiastrim           = afe->ibiastrim;
      iptattrim           = afe->iptattrim;
      vreftrim            = afe->vreftrim;
      ldo_central_pll1v8  = afe->ldo_central_pll1v8;
      ldo_central_pllref  = afe->ldo_central_pllref;
      ldo_central_pllpost = afe->ldo_central_pllpost;
      ldo_central_clkd    = afe->ldo_central_clkd;
      ldo_central_clkin   = afe->ldo_central_clkin;
    } else {
      ILOG0_V("Trim to default values");
      ibiastrim           = CENTRAL_FCSI_ICENTBIAS0_BIASTRIM__INIT;
      iptattrim           = CENTRAL_FCSI_ICENTBIAS0_PTATTRIM__INIT;
      vreftrim            = CENTRAL_FCSI_ICENTBIAS0_VREFTRIM__INIT;
      ldo_central_pll1v8  = CENTRAL_FCSI_IVREG1V8HV0_OUTTRIM__INIT;
      ldo_central_pllref  = CENTRAL_FCSI_ILDOPLLREF0_OUTTRIM_0V951__VAL;
      ldo_central_pllpost = CENTRAL_FCSI_ILDOPLLPOST0_OUTTRIM_0V951__VAL;
      ldo_central_clkd    = CENTRAL_FCSI_ILDOCLKD0_OUTTRIM__INIT;
      ldo_central_clkin   = CENTRAL_FCSI_IVREG0V950_OUTTRIM__INIT;
    }

#ifdef CPTCFG_IWLWAV_DEBUG
#define _DBG_(n)  mtlk_osal_emergency_print("%-20s 0x%02X", #n, n);
#else
#define _DBG_(n)
#endif

    _DBG_(ibiastrim          );
    _DBG_(vreftrim           );
    _DBG_(ldo_central_pll1v8 );
    _DBG_(ldo_central_pllref );
    _DBG_(ldo_central_pllpost);
    _DBG_(iptattrim          );
    _DBG_(ldo_central_clkd   );
    _DBG_(ldo_central_clkin  );

#undef _DBG_

    /* Trim biasing and central ldos to default where required */
    __abb_api_central_trim_bias(ccr_mem, vreftrim, iptattrim, ibiastrim);

    _ABB_FCSI_INIT_SET_(ccr_mem, CENTRAL_FCSI_IVREG0V950,
                                 CENTRAL_FCSI_IVREG0V950_OUTTRIM, ldo_central_clkin);

    _ABB_FCSI_INIT_SET_(ccr_mem, CENTRAL_FCSI_IVREG1V8HV0,
                                 CENTRAL_FCSI_IVREG1V8HV0_OUTTRIM, ldo_central_pll1v8);

    _ABB_FCSI_INIT_SET_(ccr_mem, CENTRAL_FCSI_ILDOPLLREF0,
                                 CENTRAL_FCSI_ILDOPLLREF0_OUTTRIM, ldo_central_pllref);

    _ABB_FCSI_INIT_SET_(ccr_mem, CENTRAL_FCSI_ILDOPLLPOST0,
                                 CENTRAL_FCSI_ILDOPLLPOST0_OUTTRIM, ldo_central_pllpost);

    _ABB_FCSI_INIT_SET_(ccr_mem, CENTRAL_FCSI_ILDOCLKD0,
                                 CENTRAL_FCSI_ILDOCLKD0_OUTTRIM, ldo_central_clkd);
}

static void
_phy_lib_PHY_SET_MAC_BAND (void *ccr_mem, uint32 band, uint32 value)
{
  _CCR_DBG_FUNC_ARG2(band, value);

  if (band) { /* band 1 */
    _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_GEN_REGS_PHY_BAND_CONFIG,
                                    MAC_GEN_REGS_PHY_BAND_CONFIG_B1_PHY_ACTIVE, value);
  } else { /* band 0 */
    _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_GEN_REGS_PHY_BAND_CONFIG,
                                    MAC_GEN_REGS_PHY_BAND_CONFIG_B0_PHY_ACTIVE, value);
  }
}

static void
_phy_lib_PHY_Reset (void *ccr_mem, uint32 band, uint32 value)
{
  _CCR_DBG_FUNC_ARG2(band, value);

  if (band) { /* band 1 */
    _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_GEN_REGS_PHY_EXT_RESET,
                              MAC_GEN_REGS_PHY_EXT_RESET_MAC_TO_PHY_BAND1_RESET_N, value);
  } else { /* band 0 */
    _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_GEN_REGS_PHY_EXT_RESET,
                              MAC_GEN_REGS_PHY_EXT_RESET_MAC_TO_PHY_BAND0_RESET_N, value);
  }
}

static void
_phy_lib_FCSI_RESET (void *ccr_mem)
{
  _CCR_DBG_FUNC_;

  /* Set single band mode (both bands are Off by default) */
  _phy_lib_PHY_SET_MAC_BAND (ccr_mem, 0, 1); /* band 0 On */

  /* Take the phy out of reset, ensure 0 to 1 */
  _phy_lib_PHY_Reset(ccr_mem, 0, 0); /* band 0 */
  _phy_lib_PHY_Reset(ccr_mem, 1, 0); /* band 1 */
  _phy_lib_PHY_Reset(ccr_mem, 0, 1); /* band 0 */
  _phy_lib_PHY_Reset(ccr_mem, 1, 1); /* band 1 */

  /* apply LO-->HI on direct control lines */
  /* set to LO (0) */
  ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_0, 0);
  ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_1, 0);
  ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_2, 0);
  ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_3, 0);
  ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_C, 0);
  /* set to HI (1) */
  ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_0, 1);
  ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_1, 1);
  ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_2, 1);
  ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_3, 1);
  ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_C, 1);
}

static void
_phy_lib_FCSI_CENTRAL_RESET (void *ccr_mem, uint32 value)
{
  _CCR_DBG_FUNC_ARG1(value);
  _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_GEN_REGS_PHY_BAND_CONFIG,
                                  MAC_GEN_REGS_PHY_BAND_CONFIG_FCSI_CEN_RESET_N, value);
}

static void
_phy_lib_FCSI_CENTRAL_ENABLE (void *ccr_mem, uint32 value)
{
  _CCR_DBG_FUNC_ARG1(value);
  _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_GEN_REGS_PHY_BAND_CONFIG,
                                  MAC_GEN_REGS_PHY_BAND_CONFIG_FCSI_CEN_BLOCK_EN, value);
}

static void
_phy_lib_FCSI_CENTRAL_WR_EXT_RF_CLKDIV (void *ccr_mem, uint32 value)
{
  _CCR_DBG_FUNC_ARG1(value);
  _CCR_RMW_REG_FIELD_(ccr_mem, 0, PHY_RX_TD_IF_PHY_RXTD_IF126,
                                  PHY_RX_TD_IF_PHY_RXTD_IF126_FCSI_CLK_WR_DIVR_EXT_RF, value);
}

static void
_phy_lib_FCSI_CENTRAL_RD_EXT_RF_CLKDIV (void *ccr_mem, uint32 value)
{
  _CCR_DBG_FUNC_ARG1(value);
  _CCR_RMW_REG_FIELD_(ccr_mem, 0, PHY_RX_TD_IF_PHY_RXTD_IF126,
                                  PHY_RX_TD_IF_PHY_RXTD_IF126_FCSI_CLK_RD_DIVR_EXT_RF, value);
}

static void
_phy_lib_FCSI_CENTRAL_WR_AFE_CLKDIV (void *ccr_mem, uint32 value)
{
  _CCR_DBG_FUNC_ARG1(value);
  _CCR_RMW_REG_FIELD_(ccr_mem, 0, PHY_RX_TD_IF_PHY_RXTD_IF127,
                                  PHY_RX_TD_IF_PHY_RXTD_IF127_FCSI_CLK_WR_DIVR_AFE, value);
}

static void
_phy_lib_FCSI_CENTRAL_RD_AFE_CLKDIV (void *ccr_mem, uint32 value)
{
  _CCR_DBG_FUNC_ARG1(value);
  _CCR_RMW_REG_FIELD_(ccr_mem, 0, PHY_RX_TD_IF_PHY_RXTD_IF127,
                                  PHY_RX_TD_IF_PHY_RXTD_IF127_FCSI_CLK_RD_DIVR_AFE, value);
}


static void
_abb_fcsi_enable (void *ccr_mem)
{
  _CCR_DBG_FUNC_;

  /* enable the fcsi central */
  _phy_lib_FCSI_CENTRAL_ENABLE(ccr_mem, 1);

  /* disable fcsi_reset central, ensure 0 to 1 */
  _phy_lib_FCSI_CENTRAL_RESET(ccr_mem, 0);
  _phy_lib_FCSI_CENTRAL_RESET(ccr_mem, 1);

  /* this control is done in _phy_lib_FCSI_RESET */
  /* ABB_DIR_CTRL_WRITE(ccr_mem, FCSI2_RST_N_C,1); */

  _phy_lib_FCSI_CENTRAL_WR_EXT_RF_CLKDIV(ccr_mem, 4);
  _phy_lib_FCSI_CENTRAL_RD_EXT_RF_CLKDIV(ccr_mem, 15);
  _phy_lib_FCSI_CENTRAL_WR_AFE_CLKDIV(ccr_mem, 4);
  _phy_lib_FCSI_CENTRAL_RD_AFE_CLKDIV(ccr_mem, 15);
}

/*-------------------------------------------------------------------*/
/* Activate FCSI access and start PLL
 */

static int _mtlk_pcie_efuse_timer_config (void *ccr_mem);
uint8 * __MTLK_IFUNC    wave_hw_get_afe_data (mtlk_hw_t *hw, unsigned *size);

static int
_mtlk_pcie_init_pll (void *ccr_mem, uint32 chip_id)
{
  mtlk_pcie_ccr_t *pcie_mem = (mtlk_pcie_ccr_t *)ccr_mem;
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != ccr_mem);

  _CCR_DBG_FUNC_;

  if (!wave_hw_mmb_card_is_asic(pcie_mem->hw))
  {
    WLOG_V("Skip PLL init due to non-ASIC");
#if CCR_REG_DEBUG
    /* continue */
#else
    return MTLK_ERR_OK;
#endif
  }

  /* take PHY out of reset */
  _phy_lib_FCSI_RESET(ccr_mem);

  _abb_fcsi_enable (ccr_mem);

  /* Apply AFE calibration data from eFuse */
  {
    wave_efuse_afe_data_v6_t  *afe_data;
    uint32  size;

    afe_data = (wave_efuse_afe_data_v6_t *)wave_hw_get_afe_data(pcie_mem->hw, &size);
    _abb_central_fusing(ccr_mem, afe_data, size);
  }

  _abb_central_init_pll(ccr_mem);

  /* Wait for PLL lock */
  if(MTLK_ERR_OK != _abb_central_lock_pll(ccr_mem)) {
    ELOG_V("PLL is not locked !!!");
#if CCR_REG_DEBUG
    res = MTLK_ERR_OK; /* continue */
#else
    return MTLK_ERR_HW;
#endif
  }

  _abb_central_init_clkd(ccr_mem);

  /* Switch to PLL */
  _abb_cpu_clock_select(ccr_mem, SYS_IF_UM_CLOCK_SELECT_CPU_CLOCK_SELECT_PLL);

  /* Configure EFUSE timers according to the current clock */
  res = _mtlk_pcie_efuse_timer_config(ccr_mem);

  return res;
}

/*-------------------------------------------------------------------*/
/* CCR APIs */

static int
_mtlk_pcie_ccr_get_fw_info (void *ccr_mem, wave_fw_num_e fw_num, mtlk_fw_info_t *fw_info)
{
  mtlk_pcie_ccr_t *pcie_mem = (mtlk_pcie_ccr_t *)ccr_mem;
  MTLK_ASSERT(ARRAY_SIZE(fw_info->mem) >= 3);

  switch (fw_num)
  {
    case WAVE_FW_NUM_UM: /* URAM */
      /* initialize memory chunks for upper-mac CPU */
      fw_info->mem[0].start   = ((unsigned char*)pcie_mem->pas) + WAVE_PCIE_URAM_IRAM_START;
      fw_info->mem[0].length  = WAVE_PCIE_URAM_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case WAVE_FW_NUM_TX:
      /* initialize memory chunks for TxSender CPU 0 */
      fw_info->mem[0].start   = ((unsigned char*)pcie_mem->pas) + WAVE_PCIE_B0_TXCPU_IRAM_START;
      fw_info->mem[0].length  = WAVE_PCIE_B0_TXCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case WAVE_FW_NUM_TX_1:
      /* initialize memory chunks for TxSender CPU 1 */
      fw_info->mem[0].start   = ((unsigned char*)pcie_mem->pas) + WAVE_PCIE_B1_TXCPU_IRAM_START;
      fw_info->mem[0].length  = WAVE_PCIE_B1_TXCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case WAVE_FW_NUM_IF:
      /* initialize memory chunks for Host Interface GenRISC CPU */
      fw_info->mem[0].start   = ((unsigned char*)pcie_mem->pas) + WAVE_PCIE_IFCPU_IRAM_START;
      fw_info->mem[0].length  = WAVE_PCIE_IFCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case WAVE_FW_NUM_RX:
      /* initialize memory chunks for RX handler GenRISC CPU 0 */
      fw_info->mem[0].start   = ((unsigned char*)pcie_mem->pas) + WAVE_PCIE_B0_RXCPU_IRAM_START;
      fw_info->mem[0].length  = WAVE_PCIE_B0_RXCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case WAVE_FW_NUM_RX_1:
      /* initialize memory chunks for RX handler GenRISC CPU 1 */
      fw_info->mem[0].start   = ((unsigned char*)pcie_mem->pas) + WAVE_PCIE_B1_RXCPU_IRAM_START;
      fw_info->mem[0].length  = WAVE_PCIE_B1_RXCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;

    default:
      return MTLK_ERR_NOT_SUPPORTED;
  }

  return MTLK_ERR_OK;
}

static void
_mtlk_pcie_set_msi_intr_mode (void *ccr_mem, uint32 irq_mode)
{
#ifdef CPTCFG_IWLWAV_X86_PUMA7_WAVE600_NOSNOOP
  RegSocRegsPcieAppsNsCtrl_u ns;
#endif

  MTLK_ASSERT(NULL != ccr_mem);

#ifdef CPTCFG_IWLWAV_X86_PUMA7_WAVE600_NOSNOOP
  ns.val = 0;
  ns.bitFields.pcie0RdTlpNoSnoop = TRUE; /* SoC --> Wave600 card */
  ns.bitFields.pcie0WrTlpNoSnoop = TRUE; /* Wave600 card --> SoC */
  ns.bitFields.pcie1RdTlpNoSnoop = TRUE;
  ns.bitFields.pcie1WrTlpNoSnoop = TRUE;
  ILOG0_DD("No snoop PCIe mode: SOC_REGS_PCIE_APPS_NS_CTRL=%x, val=%x\n",
           SOC_REGS_PCIE_APPS_NS_CTRL, ns.val);
  _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_PCIE_APPS_NS_CTRL, ns.val);
#endif

  if (MTLK_IRQ_MODE_LEGACY == irq_mode) {
    /* Legacy IRQ mode */
    _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_PCIE_MSI_ENABLE, PCIE_MSI_DISABLED);
    _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_PCIE_LEGACY_INT_ENA, PCIE_LEGACY_INT_ENABLED);

    _CCR_RMW_REG_FIELD_(ccr_mem, 0, PCIE_CFG_REGS_MSI_CAPABILITY,
                                              MSI_CAPABILITY_REGISTER_CTRL_MSI_ENABLE,
                                              MSI_CAPABILITY_REGISTER_CTRL_MSI_DISABLED);
  } else {
    /* MSI IRQ mode */
    _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_PCIE_MSI_ENABLE, PCIE_MSI_ENABLED);
    _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_PCIE_LEGACY_INT_ENA, PCIE_LEGACY_INT_DISABLED);
    _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_PCIE_MSI_INTR_MODE, irq_mode);

    _CCR_RMW_REG_FIELD_(ccr_mem, 0, PCIE_CFG_REGS_MSI_CAPABILITY,
                                              MSI_CAPABILITY_REGISTER_CTRL_MSI_ENABLE,
                                              MSI_CAPABILITY_REGISTER_CTRL_MSI_ENABLED);
  }
}

/* #define DEBUG_CCR_CHECK_REGISTERS */
#ifdef  DEBUG_CCR_CHECK_REGISTERS
static void debug_read_registers(void *ccr_mem, char *msg)
{
  uint32 val, read_chip_id;

  MTLK_ASSERT(NULL != ccr_mem);

  __ccr_mem_readl(ccr_mem, SOC_REGS_SOFT_RESET, val);
  ILOG0_SSPD("DEBUG(%s): %s:address is 0x%p, value=0x%08x", msg, "hw_reset", SOC_REGS_SOFT_RESET, val);

  __ccr_mem_readl(ccr_mem, SOC_REGS_PCIE_MSI_ENABLE, val);
  ILOG0_SSPD("DEBUG(%s): %s:address is 0x%p, value=0x%08x", msg, "pcie_msi_intr_enable_mask", SOC_REGS_PCIE_MSI_ENABLE, val);

  __ccr_mem_readl(ccr_mem, SOC_REGS_PCIE_LEGACY_INT_ENA, val);
  ILOG0_SSPD("DEBUG(%s): %s:address is 0x%p, value=0x%08x", msg, "legacy_int", SOC_REGS_PCIE_LEGACY_INT_ENA, val);

  __ccr_mem_readl(ccr_mem, SOC_REGS_PCIE_MSI_INTR_MODE, val);
  ILOG0_SSPD("DEBUG(%s): %s:address is 0x%p, value=0x%08x", msg, "pcie_msi_intr_mode", SOC_REGS_PCIE_MSI_INTR_MODE, val);

  __ccr_mem_readl(ccr_mem, PCIE_CFG_REGS_MSI_CAPABILITY, val);
  ILOG0_SSPD("DEBUG(%s): %s:address is 0x%p, value=0x%08x", msg, "msi_capability", PCIE_CFG_REGS_MSI_CAPABILITY, val);
}
#endif  /* DEBUG_CCR_CHECK_REGISTERS */

static __INLINE void
__pcie_reset_cpu_lower(void *ccr_mem, e_lmac_cpu_num  lmac_cpu_num)
{
  pcie_regs_t mips_ctrl =
    (lmac_cpu_num == LMAC_CPU_NUM0) ?
    SYS_IF_LM0_MIPS_CTL :
    SYS_IF_LM1_MIPS_CTL;

  ILOG0_D("Reset LMAC%d", lmac_cpu_num);
  __ccr_mem_writel(ccr_mem, G6_MIPS_RESET, mips_ctrl);
}

static __INLINE void
__pcie_release_cpu_lower(void *ccr_mem, e_lmac_cpu_num  lmac_cpu_num)
{
  pcie_regs_t mips_ctrl =
    (lmac_cpu_num == LMAC_CPU_NUM0) ?
    SYS_IF_LM0_MIPS_CTL :
    SYS_IF_LM1_MIPS_CTL;

  ILOG0_D("Release LMAC%d", lmac_cpu_num);
  __ccr_mem_writel(ccr_mem, G6_RELEASE_FROM_RESET, mips_ctrl);
}

static void
_mtlk_pcie_put_cpus_to_reset(void *ccr_mem)
{
  {
    int i;
    for (i = LMAC_CPU_NUM0; i < LMAC_CPU_NUM_TOTAL; i++) {
      __pcie_open_secure_write_register_lower(ccr_mem, i);
      __pcie_reset_cpu_lower(ccr_mem, i);
      mtlk_osal_msleep(20);
    }
  }

  ILOG0_V("Reset UMAC");
  __pcie_open_secure_write_register_upper(ccr_mem);
  __ccr_mem_writel(ccr_mem, G6_MIPS_RESET, SYS_IF_UM_MIPS_CTL);
  mtlk_osal_msleep(20);
}

static void _mtlk_pcie_reset_mac (void *ccr_mem)
{
  MTLK_ASSERT(NULL != ccr_mem);

#define G6_WLAN_SW_RESET    0x1u

#ifdef  DEBUG_CCR_CHECK_REGISTERS
  debug_read_registers(ccr_mem, "BEFORE_RST");
#endif  /* DEBUG_CCR_CHECK_REGISTERS */

  ILOG0_PD("Reset WLAN SOC: address is 0x%p, value=0x%08x", SOC_REGS_SOFT_RESET, G6_WLAN_SW_RESET);
  __ccr_mem_resetl(ccr_mem, SOC_REGS_SOFT_RESET, G6_WLAN_SW_RESET);
  mtlk_osal_msleep(20);

#ifdef  DEBUG_CCR_CHECK_REGISTERS
  debug_read_registers(ccr_mem, "AFTER_RST");
#endif  /* DEBUG_CCR_CHECK_REGISTERS */
}

#if (MTLK_CCR_DEBUG_PRINT_INTR_REGS)
static void
_mtlk_pcie_ccr_print_irq_regs(void *ccr_mem)
{
  mtlk_pcie_ccr_t *pci_mem = (mtlk_pcie_ccr_t *)ccr_mem;
  uint32 host_irq_mask, host_irq_status;
  uint32 pcie_msi_intr_enable_mask = 0;
  uint32 legacy_int = 0;
  uint32 pcie_msi_intr_mode = 0;
  uint32 msi_capability = 0;
  uint32 phi_interrupt_clear = 0;

  MTLK_ASSERT(NULL != pci_mem);

  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_HOST_IRQ_MASK, host_irq_mask);
  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_HOST_IRQ_STATUS, host_irq_status);
  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_PHI_INTERRUPT_CLEAR, phi_interrupt_clear);

  __ccr_mem_readl(ccr_mem, SOC_REGS_PCIE_MSI_ENABLE, pcie_msi_intr_enable_mask);
  __ccr_mem_readl(ccr_mem, SOC_REGS_PCIE_LEGACY_INT_ENA, legacy_int);
  __ccr_mem_readl(ccr_mem, SOC_REGS_PCIE_MSI_INTR_MODE, pcie_msi_intr_mode);
  __ccr_mem_readl(ccr_mem, PCIE_CFG_REGS_MSI_CAPABILITY, msi_capability);

  ILOG0_P  ("PCIE pas=%p\n", pci_mem->pas);
  ILOG0_DDD("PCIE IRQ regs: host_irq_mask=%08X, host_irq_status=%08X, phi_interrupt_clear=%08X",
            host_irq_mask, host_irq_status, phi_interrupt_clear);
  ILOG0_DDD("PCIE SOC_REGS: pcie_msi_intr_enable_mask=%08X, legacy_int=%08X, pcie_msi_intr_mode=%08X",
            pcie_msi_intr_enable_mask, legacy_int, pcie_msi_intr_mode);
  ILOG0_D  ("PCIE CFG_REGS: msi_capability=%08X", msi_capability);

  mtlk_hw_debug_print_all_ring_regs(pci_mem->hw);
}
#endif /* MTLK_CCR_DEBUG_PRINT_INTR_REGS */

static void
_mtlk_pcie_release_cpus_reset(void *ccr_mem)
{

  MTLK_ASSERT(NULL != ccr_mem);

  {
    int i;
    for (i = LMAC_CPU_NUM0; i < LMAC_CPU_NUM_TOTAL; i++) {
      __pcie_open_secure_write_register_lower(ccr_mem, i);
      __pcie_release_cpu_lower(ccr_mem, i);
      mtlk_osal_msleep(20);
    }
  }

  ILOG0_V("Release UMAC");
  __pcie_open_secure_write_register_upper(ccr_mem);
  __ccr_mem_writel(ccr_mem, G6_RELEASE_FROM_RESET, SYS_IF_UM_MIPS_CTL);
  mtlk_osal_msleep(10);
}

static void
_mtlk_pcie_put_ctl_to_reset(void *ccr_mem)
{
  MTLK_ASSERT(NULL != ccr_mem);

  /* Disable RX */
  __ccr_mem_resetl(ccr_mem, PAC_RX_CONTROL, PAC_RX_CONTROL_RXC_ENABLE);
}

static void
_mtlk_pcie_power_on_cpus(void *ccr_mem)
{
  MTLK_ASSERT(NULL != ccr_mem);

  /* Enable all MAC GenRisc CPU clocks */
  _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_HT_EXT_CLK_CONTROL_REG_DYN_EN,
                            MAC_GEN_REGS_CLK_CONTROL_REG_DYN_EN_Q_MANAGERS03,
                            MAC_GEN_REGS_CLK_CONTROL_REG_DYN_EN_Q_MANAGERS03_ENABLE);
}

/* Unsupported APIs */
#define _mtlk_pcie_read_hw_type       mtlk_ccr_dummy_ret_uzero
#define _mtlk_pcie_read_hw_rev        mtlk_ccr_dummy_ret_uzero

static const mtlk_ccr_api_t mtlk_ccr_api = {
  _mtlk_pcie_ccr_init,
  _mtlk_pcie_ccr_cleanup,
  _mtlk_pcie_ccr_read_chip_id,
  _mtlk_pcie_ccr_get_fw_info,
  _mtlk_pcie_ccr_get_fw_dump_info,
  _mtlk_pcie_ccr_get_hw_dump_info,
  _mtlk_pcie_enable_interrupts,
  _mtlk_pcie_disable_interrupts,
  _mtlk_pcie_clear_interrupts_if_pending,
  _mtlk_pcie_disable_interrupts_if_pending,
  _mtlk_pcie_initiate_doorbell_interrupt,
  _mtlk_pcie_is_interrupt_pending,
  _mtlk_pcie_recover_interrupts,
  _mtlk_pcie_release_ctl_from_reset,
  _mtlk_pcie_put_ctl_to_reset,
  _mtlk_pcie_exit_debug_mode,
  _mtlk_pcie_put_cpus_to_reset,
  _mtlk_pcie_reset_mac,
  _mtlk_pcie_power_on_cpus,
  _mtlk_pcie_get_afe_value,
  _mtlk_pcie_get_progmodel_version,
  _mtlk_pcie_bist_efuse,
  _mtlk_pcie_init_pll,
  _mtlk_pcie_release_cpus_reset,
  _mtlk_pcie_check_bist,
  _mtlk_pcie_eeprom_init,
  _mtlk_pcie_eeprom_override_write,
  _mtlk_pcie_eeprom_override_read,
  _mtlk_pcie_ccr_get_mips_freq,
  _mtlk_pcie_set_msi_intr_mode,
#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
  _mtlk_pcie_read_hw_timestamp,
#endif
  _mtlk_pcie_ccr_ctrl_ring_init,
  _mtlk_pcie_ccr_ring_get_hds_to_process,
  _mtlk_pcie_ccr_ring_clear_interrupt,
  _mtlk_pcie_ccr_ring_initiate_doorbell_interrupt,
  _mtlk_pcie_ccr_print_irq_regs,
  _mtlk_pcie_efuse_read_data,
  _mtlk_pcie_efuse_write_data,
  _mtlk_pcie_efuse_get_size,
  _mtlk_pcie_ccr_read_uram_version,
  _mtlk_pcie_read_phy_inband_power,
  _mtlk_pcie_read_hw_type,
  _mtlk_pcie_read_hw_rev,
};

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID
