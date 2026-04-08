/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include "hw_mmb.h"
#include "mtlk_ccr_pcieg6.h"
#include "mtlk_ccr_pcieg6_regs.h"
#include "mtlk_ccr_api.h"
#include "mtlk_ccr_common.h"

#define LOG_LOCAL_GID   GID_CCR_PCIEG6
#define LOG_LOCAL_FID   0

#define __PCIE_IO_MASK__        ((1u << 25) - 1) /* 32M - total PCIe io space */
#define __CCR_IO_MASK__         ((1u << 24) - 1) /* 16M - accessible from the host */
/* Checking for ability to access from the host also for alignment to 4-bytes word */
#define __CCR_IO_CHECK__(addr)  ((0u == (~(__CCR_IO_MASK__) & (addr))) && (0u == ((addr) & 3u)))

#define WLAN_CLOCK_XO_MHZ                           40  /* "slow" XO  clock  40 MHz */
#define WLAN_CLOCK_PLL_MHZ                          640 /* "fast" PLL clock 640 MHz */

#define WLAN_TIME_US_TO_CLOCKS_MHZ(mhz, us)         ((mhz) * (us))
#define WLAN_TIME_NS_TO_CLOCKS_MHZ(mhz, ns)         MAX(1, ((mhz) * (ns) / 1000)) /* at least 1 */


/* Concatenate 2 names */
#define _N_N_(name1, name2)     name1 ## _ ## name2

/* Enum with addresses for access to the registers listed in the include file */
/* Item example:
 *  Input   _HW_REG_DEF_(SYS_IF_UM, REG_EMERALD_ENV, SYSTEM_INFO)
 *  Output  SYS_IF_UM_SYSTEM_INFO = REG_EMERALD_ENV_BAR0 + (REG_EMERALD_ENV_SYSTEM_INFO - EMERALD_ENV_BASE_ADDRESS)
*/
typedef enum _pcieg6_regs {
#define _HW_REG_DEF_(ccr_block, reg_block, name) \
            _N_N_(ccr_block, name) = _N_N_(ccr_block, BAR0) + \
            (_N_N_(REG_ ## reg_block, name) - _N_N_(reg_block, BASE_ADDRESS)),

        _PCIEG6_HW_REGS_LIST_
        _PCIEG6_HW_DEPEND_REGS_LIST_

#undef  _HW_REG_DEF_
} pcieg6_regs_t;

#define MAC_GEN_REGS_FPGA_VERSION   MAC_GEN_REGS_HYP_MAC_TOP_GPI_INT


/* Checking for ability to access from the host for all listed registers */
#define _HW_REG_DEBUG_  1

#if _HW_REG_DEBUG_

static pcieg6_regs_t    _pcieg6_regs_table[] = {
#define _HW_REG_DEF_(ccr_block, reg_block, name)    _N_N_(ccr_block, name),

        _PCIEG6_HW_REGS_LIST_
        _PCIEG6_HW_DEPEND_REGS_LIST_

#undef  _HW_REG_DEF_
};

static BOOL
_mtlk_ccr_is_addr_valid (pcieg6_regs_t addr)
{
    BOOL res = TRUE;
    char *fmt = "PCIEG6 addr 0x%08X %s";
    char *msg = "Ok";

    MTLK_UNREFERENCED_PARAM(fmt);

    if (0u != ((addr) & 3u)) {
        msg = "is not word aligned";
        res = FALSE;
    } else if (0u != (~(__CCR_IO_MASK__) & (addr))) {
        msg = "is not accessible from the host";
        res = FALSE;
    }

    if (res) {
      ILOG1_DS(fmt, addr, msg);
    } else {
      ELOG_DS(fmt, addr, msg);
    }

    return res;
}

static int
_mtlk_ccr_pcieg6_all_regs_validate (void)
{
    BOOL    is_valid = TRUE;
    int     i;

    for (i = 0; i < ARRAY_SIZE(_pcieg6_regs_table); i++) {
        if (!_mtlk_ccr_is_addr_valid(_pcieg6_regs_table[i])) {
            is_valid = FALSE;
        }
    }

    return (is_valid ?  MTLK_ERR_OK : MTLK_ERR_UNKNOWN);
}

#else /* _HW_REG_DEBUG_ */
static int
_mtlk_ccr_pcieg6_all_regs_validate (void)
{
    return MTLK_ERR_OK;
}
#endif /* _HW_REG_DEBUG_ */

/*--------------------------------------------------------------*/
/* CCR mem base address */
#define CCR_MEM_PAS(ccr_mem)        ((volatile void *)((_mtlk_pcieg6_ccr_t *)ccr_mem)->pas)
#define CCR_MEM_ADDR(ccr_mem, addr) (CCR_MEM_PAS(ccr_mem) + (addr))

/* Read/write access using ccr_mem */
#define __ccr_mem_readl(ccr_mem, addr, val)     __ccr_readl(CCR_MEM_ADDR(ccr_mem, addr), (val))
#define __ccr_mem_writel(ccr_mem, val, addr)    __ccr_writel((val), CCR_MEM_ADDR(ccr_mem, addr))
#define __ccr_mem_resetl(ccr_mem, addr, flag)   __ccr_resetl(CCR_MEM_ADDR(ccr_mem, addr), flag)
#define __ccr_mem_setl(ccr_mem, addr, flag)     __ccr_setl(CCR_MEM_ADDR(ccr_mem, addr), flag)

/*--------------------------------------------------------------*/

#define MTLK_GEN6_DISABLE_INTERRUPT     (0x0)

#define PCIEG6_MIPS_FREQ_DEFAULT  240

#define PCIEG6_SYS_IF_UM_SYSTEM_INFO_CHIPID_OFFSET (0x00000000)
#define PCIEG6_SYS_IF_UM_SYSTEM_INFO_CHIPID_MASK   (0x0000FFFF)


#define PCIEG6_FPGA_VERSION_MINOR_OFFSET                  (0)
#define PCIEG6_FPGA_VERSION_MINOR_MASK                    (0x0000000F)
#define PCIEG6_FPGA_VERSION_MAJOR_OFFSET                  (4)
#define PCIEG6_FPGA_VERSION_MAJOR_MASK                    (0x000000F0)
#define PCIEG6_WLAN_HW_BASE_VERSION_MINOR_OFFSET          (8)
#define PCIEG6_WLAN_HW_BASE_VERSION_MINOR_MASK            (0x00003F00)
#define PCIEG6_WLAN_HW_BASE_VERSION_MAJOR_OFFSET          (14)
#define PCIEG6_WLAN_HW_BASE_VERSION_MAJOR_MASK            (0x0000C000)

/* ============================================== */

/* Reset/Active bit */
#define G6_CPU_RAB_Reset              (0 << 1)
#define G6_CPU_RAB_Active             (1 << 1)

/* Boot mode bits */
#define G6_CPU_RAB_IRAM               (0 << 2)
#define G6_CPU_RAB_SHRAM              (2 << 2)
#define G6_CPU_RAB_DEBUG              (3 << 2)

/* Override bit */
#define PCIEG6_CPU_RAB_Override       (1 << 4)

#define G6_MASK_RX_ENABLED            (1)

#define PCIEG6_UCPU_32K_BLOCKS        (1)

#define G6_MIPS_RESET                 0x02
#define G6_RELEASE_FROM_RESET         0x01

typedef enum {
  LMAC_CPU_NUM0,
  LMAC_CPU_NUM1,
  LMAC_CPU_NUM_TOTAL
} e_lmac_cpu_num;

/*--------- Table of FW dump files ---------------*/
/* All IO base addresses are offsets from BAR0
 * Required order in the table:
 *    Common files for both radios
 *    (B0) Radio 0 specific
 *    (B1) Radio 1 specific
*/

enum g6_proc_fw_dump_idx { /* index in array */
  /* Common files for both radios */
  G6_FW_IDX_CMN_FIRST = 0,
  G6_FW_IDX_IRAM      = G6_FW_IDX_CMN_FIRST,
  G6_FW_IDX_SHRAM,
  G6_FW_IDX_IF_COUNTERS,
  G6_FW_IDX_IF_RAM,
  G6_FW_IDX_IF_REG,
  G6_FW_IDX_IF_SCP,
  G6_FW_IDX_DESC_RAM,
  G6_FW_IDX_DLM,
  G6_FW_IDX_DMA_LL,
  G6_FW_IDX_RX_PP_RAM,
  G6_FW_IDX_CMN_LAST,
  /* (B0) Radio 0 specific */
  G6_FW_IDX_B0_FIRST  = G6_FW_IDX_CMN_LAST,
  G6_FW_IDX_B0_TX_RAM = G6_FW_IDX_B0_FIRST,
  G6_FW_IDX_B0_TX_REG,
  G6_FW_IDX_B0_TX_SCP,
  G6_FW_IDX_B0_TX_SEL,
  G6_FW_IDX_B0_TX_STD,
  G6_FW_IDX_B0_RX_RAM,
  G6_FW_IDX_B0_RX_REG,
  G6_FW_IDX_B0_RX_SCP,
  G6_FW_IDX_B0_AUTO_RESP,
  G6_FW_IDX_B0_TEST_BUS,
  G6_FW_IDX_B0_PHY_RX_TD_REG096,

#if 0 /* FIXME: unavailable */
  G6_FW_IDX_B0_MPDU_DESC,
#endif
  G6_FW_IDX_B0_LAST,

  /* (B1) Radio 1 specific */
  G6_FW_IDX_B1_FIRST  = G6_FW_IDX_B0_LAST,
  G6_FW_IDX_B1_TX_RAM = G6_FW_IDX_B1_FIRST,
  G6_FW_IDX_B1_TX_REG,
  G6_FW_IDX_B1_TX_SCP,
  G6_FW_IDX_B1_TX_SEL,
  G6_FW_IDX_B1_TX_STD,
  G6_FW_IDX_B1_RX_RAM,
  G6_FW_IDX_B1_RX_REG,
  G6_FW_IDX_B1_RX_SCP,
  G6_FW_IDX_B1_AUTO_RESP,
  G6_FW_IDX_B1_TEST_BUS,
  G6_FW_IDX_B1_PHY_RX_TD_REG096,
#if 0 /* FIXME: unavailable */
  G6_FW_IDX_B1_MPDU_DESC,
#endif
  G6_FW_IDX_B1_LAST,
  /* Total number */
  G6_FW_IDX_TOTAL = G6_FW_IDX_B1_LAST
};

/* Special defines for "test_bus_b0" */
#define TEST_BUS_SINGLE_BAND_SIZE           (B0_PHY_TEST_BUS_SIZE)          /* Single band - full size */
#define TEST_BUS_DUAL_BAND_SIZE             ((B0_PHY_TEST_BUS_SIZE) / 2)    /* Dual band   - half size */

/* todo: review */
#define DLM_DUMP_SIZE                       (0x0000ABE4)

/* Partial dump of MAC_GENRISC_HOST_REG and B0/B1_MAC_GENRISC_TX/RX_REG */
#define MAC_GENRISC_REG_DUMP_SIZE           (0x100)
#define BX_MAC_GENRISC_REG_DUMP_SIZE        MAC_GENRISC_REG_DUMP_SIZE

static wave_fw_dump_info_t  g6_proc_fw_dump_files[G6_FW_IDX_TOTAL] = {
  /* Common files for both radios */
  [ G6_FW_IDX_IRAM         ] = { "iram",                PROC_FW_IO,  CPU_IRAM_OFFSET_FROM_BAR0,                CPU_IRAM_SIZE                },
  [ G6_FW_IDX_SHRAM        ] = { "shram",               PROC_FW_IO,  SHARED_RAM_OFFSET_FROM_BAR0,              SHARED_RAM_SIZE              },
  [ G6_FW_IDX_IF_COUNTERS  ] = { "host_if_counters",    PROC_FW_IO,  HOST_IF_COUNTERS_OFFSET_FROM_BAR0,        HOST_IF_COUNTERS_SIZE        },
  [ G6_FW_IDX_IF_RAM       ] = { "host_if_ram",         PROC_FW_IO,  MAC_GENRISC_HOST_IRAM_OFFSET_FROM_BAR0,   MAC_GENRISC_HOST_IRAM_SIZE   },
  [ G6_FW_IDX_IF_REG       ] = { "host_if_reg",         PROC_FW_IO,  MAC_GENRISC_HOST_REG_OFFSET_FROM_BAR0,    MAC_GENRISC_REG_DUMP_SIZE    }, /* MAC_GENRISC_HOST_REG_SIZE */
  [ G6_FW_IDX_IF_SCP       ] = { "host_if_scpad",       PROC_FW_IO,  MAC_GENRISC_HOST_SPRAM_OFFSET_FROM_BAR0,  MAC_GENRISC_HOST_SPRAM_SIZE  },
  [ G6_FW_IDX_DESC_RAM     ] = { "descriptor_ram",      PROC_FW_IO,  DESCRIPTOR_RAM_OFFSET_FROM_BAR0,          DESCRIPTOR_RAM_SIZE          },
  [ G6_FW_IDX_DLM          ] = { "dlm",                 PROC_FW_IO,  DLM_OFFSET_FROM_BAR0,                     DLM_DUMP_SIZE                },
  [ G6_FW_IDX_DMA_LL       ] = { "dma_linked_list",     PROC_FW_IO,  WLAN_ARM_LL_MEM_OFFSET_FROM_BAR0,         WLAN_ARM_LL_MEM_SIZE         },
  [ G6_FW_IDX_RX_PP_RAM    ] = { "rxpp_ram",            PROC_FW_IO,  RX_PP_OFFSET_FROM_BAR0,                   RX_PP_SIZE                   },
  /* (B0) Radio 0 specific */
  [ G6_FW_IDX_B0_TX_SEL    ] = { "tx_selector_b0",      PROC_FW_IO,  B0_TX_SELECTOR_OFFSET_FROM_BAR0,          B0_TX_SELECTOR_SIZE          },
  [ G6_FW_IDX_B0_TX_RAM    ] = { "tx_sender_ram_b0",    PROC_FW_IO,  B0_MAC_GENRISC_TX_IRAM_OFFSET_FROM_BAR0,  B0_MAC_GENRISC_TX_IRAM_SIZE  },
  [ G6_FW_IDX_B0_TX_REG    ] = { "tx_sender_reg_b0",    PROC_FW_IO,  B0_MAC_GENRISC_TX_REG_OFFSET_FROM_BAR0,   BX_MAC_GENRISC_REG_DUMP_SIZE }, /* B0_MAC_GENRISC_TX_REG_SIZE */
  [ G6_FW_IDX_B0_TX_SCP    ] = { "tx_sender_scpad_b0",  PROC_FW_IO,  B0_MAC_GENRISC_TX_SPRAM_OFFSET_FROM_BAR0, B0_MAC_GENRISC_TX_SPRAM_SIZE },
  [ G6_FW_IDX_B0_TX_STD    ] = { "tx_sender_std_b0",    PROC_FW_IO,  B0_MAC_GENRISC_TX_STD_OFFSET_FROM_BAR0,   B0_MAC_GENRISC_TX_STD_SIZE   },
  [ G6_FW_IDX_B0_RX_RAM    ] = { "rx_handler_ram_b0",   PROC_FW_IO,  B0_MAC_GENRISC_RX_IRAM_OFFSET_FROM_BAR0,  B0_MAC_GENRISC_RX_IRAM_SIZE  },
  [ G6_FW_IDX_B0_RX_REG    ] = { "rx_handler_reg_b0",   PROC_FW_IO,  B0_MAC_GENRISC_RX_REG_OFFSET_FROM_BAR0,   BX_MAC_GENRISC_REG_DUMP_SIZE }, /* B0_MAC_GENRISC_RX_REG_SIZE */
  [ G6_FW_IDX_B0_RX_SCP    ] = { "rx_handler_scpad_b0", PROC_FW_IO,  B0_MAC_GENRISC_RX_SPRAM_OFFSET_FROM_BAR0, B0_MAC_GENRISC_RX_SPRAM_SIZE },
  [ G6_FW_IDX_B0_AUTO_RESP ] = { "auto_resp_ram_b0",    PROC_FW_IO,  B0_AUTO_RESP_MEMS_OFFSET_FROM_BAR0,       B0_AUTO_RESP_MEMS_SIZE       },
  [ G6_FW_IDX_B0_TEST_BUS  ] = { "test_bus_b0",         PROC_FW_IO,  B0_PHY_TEST_BUS_OFFSET_FROM_BAR0,         TEST_BUS_SINGLE_BAND_SIZE    },
  [ G6_FW_IDX_B0_PHY_RX_TD_REG096 ] =
                               { "phy_rx_td_reg096_b0", PROC_FW_IO,  B0_PHY_RX_TD_PHY_RXTD_REG096,             (sizeof(uint32) * 1)         },
  /* (B1) Radio 1 specific */
  [ G6_FW_IDX_B1_TX_SEL    ] = { "tx_selector_b1",      PROC_FW_IO,  B1_TX_SELECTOR_OFFSET_FROM_BAR0,          B1_TX_SELECTOR_SIZE          },
  [ G6_FW_IDX_B1_TX_RAM    ] = { "tx_sender_ram_b1",    PROC_FW_IO,  B1_MAC_GENRISC_TX_IRAM_OFFSET_FROM_BAR0,  B1_MAC_GENRISC_TX_IRAM_SIZE  },
  [ G6_FW_IDX_B1_TX_REG    ] = { "tx_sender_reg_b1",    PROC_FW_IO,  B1_MAC_GENRISC_TX_REG_OFFSET_FROM_BAR0,   BX_MAC_GENRISC_REG_DUMP_SIZE }, /* B1_MAC_GENRISC_TX_REG_SIZE */
  [ G6_FW_IDX_B1_TX_SCP    ] = { "tx_sender_scpad_b1",  PROC_FW_IO,  B1_MAC_GENRISC_TX_SPRAM_OFFSET_FROM_BAR0, B1_MAC_GENRISC_TX_SPRAM_SIZE },
  [ G6_FW_IDX_B1_TX_STD    ] = { "tx_sender_std_b1",    PROC_FW_IO,  B1_MAC_GENRISC_TX_STD_OFFSET_FROM_BAR0,   B1_MAC_GENRISC_TX_STD_SIZE   },
  [ G6_FW_IDX_B1_RX_RAM    ] = { "rx_handler_ram_b1",   PROC_FW_IO,  B1_MAC_GENRISC_RX_IRAM_OFFSET_FROM_BAR0,  B1_MAC_GENRISC_RX_IRAM_SIZE  },
  [ G6_FW_IDX_B1_RX_REG    ] = { "rx_handler_reg_b1",   PROC_FW_IO,  B1_MAC_GENRISC_RX_REG_OFFSET_FROM_BAR0,   BX_MAC_GENRISC_REG_DUMP_SIZE }, /* B1_MAC_GENRISC_RX_REG_SIZE */
  [ G6_FW_IDX_B1_RX_SCP    ] = { "rx_handler_scpad_b1", PROC_FW_IO,  B1_MAC_GENRISC_RX_SPRAM_OFFSET_FROM_BAR0, B1_MAC_GENRISC_RX_SPRAM_SIZE },
  [ G6_FW_IDX_B1_AUTO_RESP ] = { "auto_resp_ram_b1",    PROC_FW_IO,  B1_AUTO_RESP_MEMS_OFFSET_FROM_BAR0,       B1_AUTO_RESP_MEMS_SIZE       },
  [ G6_FW_IDX_B1_TEST_BUS  ] = { "test_bus_b1",         PROC_FW_IO,  B1_PHY_TEST_BUS_OFFSET_FROM_BAR0,         TEST_BUS_DUAL_BAND_SIZE      },
  [ G6_FW_IDX_B1_PHY_RX_TD_REG096 ] =
                               { "phy_rx_td_reg096_b1", PROC_FW_IO,  B1_PHY_RX_TD_PHY_RXTD_REG096,             (sizeof(uint32) * 1)         },

#if 0 /* FIXME: temporarily disabled */
  [ G6_FW_IDX_B0_MPDU_DESC ] = { "mpdu_desc_ram_b0",    PROC_FW_IO,  B0_MPDU_DESC_MEM_OFFSET_FROM_BAR0,        B0_MPDU_DESC_MEM_SIZE        },
  [ G6_FW_IDX_B1_MPDU_DESC ] = { "mpdu_desc_ram_b1",    PROC_FW_IO,  B1_MPDU_DESC_MEM_OFFSET_FROM_BAR0,        B1_MPDU_DESC_MEM_SIZE        },
#endif
};

/*------------------------------------------------*/

#if defined(CPTCFG_IWLWAV_G6_FPGA_IRAM_TEST)
void iram_rw_test(void *ccr_mem)
{
  _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  uint8 *iram_ptr = (uint8 *)pcieg6_mem->pas + MTLK_G6_URAM_IRAM_START;
  uint32 wr, rd;
  int i;
  wr = 0xbeefcaf0;

  printk("!!! test IRAM starting addr %p !!!\n", iram_ptr);

  for (i = 0; i < 3; i++) {
    __ccr_writel(wr, iram_ptr);
    mtlk_osal_msleep(10);
    __ccr_readl(iram_ptr, rd);
    mtlk_osal_msleep(10);
    if(wr != rd) printk("!!! test IRAM addr %p failure: wr 0x%08x, rd %08x !!!\n", iram_ptr, wr, rd);
    wr++;
  }

  wr = 0xdeadfee0;
  iram_ptr += 4;
  for (i = 0; i < 3; i++) {
    __ccr_writel(wr, iram_ptr);
    mtlk_osal_msleep(10);
    __ccr_readl(iram_ptr, rd);
    mtlk_osal_msleep(10);
    if(wr != rd) printk("!!! test IRAM addr %p failure: wr 0x%08x, rd %08x !!!", iram_ptr, wr, rd);
    wr++;
  }

  printk("!!! test IRAM end !!!\n");
}
#endif

static uint32
_mtlk_pcieg6_ccr_read_chip_id(void *ccr_mem)
{
  uint32 system_info;

  __ccr_mem_readl(ccr_mem, SYS_IF_UM_SYSTEM_INFO, system_info);

  system_info &=  PCIEG6_SYS_IF_UM_SYSTEM_INFO_CHIPID_MASK;
  system_info >>= PCIEG6_SYS_IF_UM_SYSTEM_INFO_CHIPID_OFFSET;

  mtlk_osal_emergency_print("PCIE bus ChipVersion:%s ChipID is:0x%04X address is:0x%px",
                            ((system_info == WAVE_WAVE600_A0_CHIP_ID) ? "A0" :
                            ((system_info == WAVE_WAVE600_B0_CHIP_ID) ? "A1" :
                            ((system_info == WAVE_WAVE600_D2_CHIP_ID) ? "D2" : "UNKNOWN"))),
                            system_info, CCR_MEM_PAS(ccr_mem) + SYS_IF_UM_SYSTEM_INFO);

#if defined(CPTCFG_IWLWAV_G6_FPGA_IRAM_TEST)
  iram_rw_test(ccr_mem);
#endif

  return system_info;
}

static uint32
_mtlk_pcieg6_ccr_read_uram_version(void *ccr_mem)
{
  uint32 fpga_version, a, b, c, d;

  __ccr_mem_readl(ccr_mem, MAC_GEN_REGS_FPGA_VERSION, fpga_version);
  a = fpga_version & PCIEG6_WLAN_HW_BASE_VERSION_MAJOR_MASK;
  a >>= PCIEG6_WLAN_HW_BASE_VERSION_MAJOR_OFFSET;
  b = fpga_version & PCIEG6_WLAN_HW_BASE_VERSION_MINOR_MASK;
  b >>= PCIEG6_WLAN_HW_BASE_VERSION_MINOR_OFFSET;
  c = fpga_version & PCIEG6_FPGA_VERSION_MAJOR_MASK;
  c >>= PCIEG6_FPGA_VERSION_MAJOR_OFFSET;
  d = fpga_version & PCIEG6_FPGA_VERSION_MINOR_MASK;
  d >>= PCIEG6_FPGA_VERSION_MINOR_OFFSET;

  return fpga_version;
}


BOOL __MTLK_IFUNC   wave_hw_radio_band_cfg_is_single (mtlk_hw_t *hw);

static int
_mtlk_pcieg6_ccr_get_size_fw_dump_info (void *ccr_mem, wave_fw_dump_info_t *fw_info)
{
  _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  int size = ARRAY_SIZE(g6_proc_fw_dump_files);

  MTLK_ASSERT(ccr_mem);

  /* Size of the B0_TEST_BUS file depends on Single/Dual band mode */
  if (wave_hw_radio_band_cfg_is_single(pcieg6_mem->hw)) {
    fw_info[G6_FW_IDX_B0_TEST_BUS].io_size = TEST_BUS_SINGLE_BAND_SIZE;
    size = G6_FW_IDX_B0_LAST;
  } else {
    fw_info[G6_FW_IDX_B0_TEST_BUS].io_size = TEST_BUS_DUAL_BAND_SIZE;
    size = G6_FW_IDX_B1_LAST;
  }

  /* Set size to 0 for files which are unsupported by dummy phy */
  /* FIXME: what about PDXP (Emulation) with real PHY ? */
  if (wave_hw_mmb_card_is_phy_dummy(pcieg6_mem->hw)) {
    fw_info[G6_FW_IDX_B0_TEST_BUS].io_size  = 0;
    fw_info[G6_FW_IDX_B1_TEST_BUS].io_size  = 0;
    fw_info[G6_FW_IDX_B0_PHY_RX_TD_REG096].io_size = 0;
    fw_info[G6_FW_IDX_B1_PHY_RX_TD_REG096].io_size = 0;
  }

  /* Different size of DESC_RAM */
  if (wave_hw_mmb_card_is_asic(pcieg6_mem->hw)) {
    fw_info[G6_FW_IDX_DESC_RAM].io_size = DESCRIPTOR_RAM_SIZE;
  } else { /* non ASIC */
    fw_info[G6_FW_IDX_DESC_RAM].io_size = REDUCED_DESCRIPTOR_RAM_SIZE;
  }

  return size;
}

static int
_mtlk_pcieg6_ccr_get_fw_dump_info (void *ccr_mem, wave_fw_dump_info_t **fw_info)
{
  size_t alloc_size = 0;

  MTLK_ASSERT(NULL != fw_info);
  MTLK_ASSERT(NULL == *fw_info);

  alloc_size = sizeof(g6_proc_fw_dump_files);
  *fw_info = mtlk_osal_mem_alloc(alloc_size, LQLA_MEM_TAG_FW_RECOVERY);
  if (*fw_info == NULL) {
    ELOG_V("Failed to alloc memory for fw_info");
    return MTLK_ERR_NO_MEM;
  }

  wave_memcpy(*fw_info, alloc_size, &g6_proc_fw_dump_files[0], sizeof(g6_proc_fw_dump_files));

  return _mtlk_pcieg6_ccr_get_size_fw_dump_info(ccr_mem, *fw_info);
}

static int
_mtlk_pcieg6_ccr_get_hw_dump_info (void *ccr_mem, wave_hw_dump_info_t **hw_dump_info)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(hw_dump_info);
  return 0; /* zero size */
}

static int
_mtlk_pcieg6_ccr_get_fw_info (void *ccr_mem, uint8 cpu, mtlk_fw_info_t *fw_info)
{
  _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  MTLK_ASSERT(ARRAY_SIZE(fw_info->mem) >= 3);

  switch (cpu)
  {
    case CHI_CPU_NUM_UM: /* URAM */
      /* initialize memory chunks for upper-mac CPU */
      fw_info->mem[0].start   = ((unsigned char*)pcieg6_mem->pas) + MTLK_G6_URAM_IRAM_START;
      fw_info->mem[0].length  = MTLK_G6_URAM_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case CHI_CPU_NUM_TX:
      /* initialize memory chunks for TxSender CPU 0 */
      fw_info->mem[0].start   = ((unsigned char*)pcieg6_mem->pas) + MTLK_G6_B0_TXCPU_IRAM_START;
      fw_info->mem[0].length  = MTLK_G6_B0_TXCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case CHI_CPU_NUM_TX_1:
      /* initialize memory chunks for TxSender CPU 1 */
      fw_info->mem[0].start   = ((unsigned char*)pcieg6_mem->pas) + MTLK_G6_B1_TXCPU_IRAM_START;
      fw_info->mem[0].length  = MTLK_G6_B1_TXCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case CHI_CPU_NUM_IF:
      /* initialize memory chunks for Host Interface GenRISC CPU */
      fw_info->mem[0].start   = ((unsigned char*)pcieg6_mem->pas) + MTLK_G6_IFCPU_IRAM_START;
      fw_info->mem[0].length  = MTLK_G6_IFCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case CHI_CPU_NUM_RX:
      /* initialize memory chunks for RX handler GenRISC CPU 0 */
      fw_info->mem[0].start   = ((unsigned char*)pcieg6_mem->pas) + MTLK_G6_B0_RXCPU_IRAM_START;
      fw_info->mem[0].length  = MTLK_G6_B0_RXCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case CHI_CPU_NUM_RX_1:
      /* initialize memory chunks for RX handler GenRISC CPU 1 */
      fw_info->mem[0].start   = ((unsigned char*)pcieg6_mem->pas) + MTLK_G6_B1_RXCPU_IRAM_START;
      fw_info->mem[0].length  = MTLK_G6_B1_RXCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;

    case CHI_CPU_NUM_LM: /* LM is not supported in URAM */
    default:
      return MTLK_ERR_NOT_SUPPORTED;
  }

  return MTLK_ERR_OK;
}

#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
static void
_mtlk_pcieg6_read_hw_timestamp(void *ccr_mem, uint32 *low, uint32 *high)
{
  MTLK_ASSERT(NULL != ccr_mem);

  __ccr_mem_readl(ccr_mem, TSF_TIMER_TSF_TIMER_LOW_RPORT, *low);
  __ccr_mem_readl(ccr_mem, TSF_TIMER_TSF_TIMER_HIGH_RPORT, *high);
}
#endif /* CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED */


#if (!MTLK_CCR_DEBUG_PRINT_INTR_REGS)
#define _mtlk_pcieg6_ccr_print_irq_regs mtlk_ccr_dummy_ret_void
#else
static void
_mtlk_pcieg6_ccr_print_irq_regs(void *ccr_mem)
{
  _mtlk_pcieg6_ccr_t *g6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  uint32 host_irq_mask, host_irq_status;
  uint32 pcie_msi_intr_enable_mask, legacy_int, pcie_msi_intr_mode, msi_capability, phi_interrupt_clear;

  MTLK_ASSERT(NULL != g6_mem);

  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_HOST_IRQ_MASK, host_irq_mask);
  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_HOST_IRQ_STATUS, host_irq_status);
  __ccr_mem_readl(ccr_mem, SOC_REGS_PCIE_MSI_ENABLE, pcie_msi_intr_enable_mask);
  __ccr_mem_readl(ccr_mem, SOC_REGS_PCIE_LEGACY_INT_ENA, legacy_int);
  __ccr_mem_readl(ccr_mem, SOC_REGS_PCIE_MSI_INTR_MODE, pcie_msi_intr_mode);
  __ccr_mem_readl(ccr_mem, PCIE_CFG_REGS_MSI_CAPABILITY, msi_capability);
  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_PHI_INTERRUPT_CLEAR, phi_interrupt_clear);

  WLOG_P  ("Gen6 pas=%p\n", g6_mem->pas);
  WLOG_DD ("Gen6 IRQ regs: MAC_GEN_REGS.host_irq_mask=%08X, host_irq_status=%08X",
          host_irq_mask, host_irq_status);
  WLOG_DDD("  SOC_REGS.pcie_msi_intr_enable_mask=%08X, SOC_REGS.legacy_int=%08X, SOC_REGS.pcie_msi_intr_mode=%08X",
          pcie_msi_intr_enable_mask, legacy_int, pcie_msi_intr_mode);
  WLOG_DD ("  PCIE_CFG_REGS.msi_capability=%08X, MAC_GEN_REGS.phi_interrupt_clear=%08X",
          msi_capability, phi_interrupt_clear);

  mtlk_hw_debug_print_all_ring_regs(g6_mem->hw);
}
#endif /* MTLK_CCR_DEBUG_PRINT_INTR_REGS */


#define _mtlk_pcieg6_get_afe_value mtlk_ccr_dummy_ret_void_array
#define _mtlk_pcieg6_bist_efuse mtlk_ccr_dummy_ret_void_param
#define _mtlk_pcieg6_exit_debug_mode mtlk_ccr_dummy_ret_void
#define _mtlk_pcieg6_release_ctl_from_reset mtlk_ccr_dummy_ret_void_param

/* Put interrupt-related functions into single section */
static void   _mtlk_pcieg6_enable_interrupts(void *ccr_mem, uint32 mask)                    __MTLK_INT_HANDLER_SECTION;
static void   _mtlk_pcieg6_disable_interrupts(void *ccr_mem, uint32 mask)                   __MTLK_INT_HANDLER_SECTION;
static uint32 _mtlk_pcieg6_clear_interrupts_if_pending(void *ccr_mem, uint32 clear_mask)    __MTLK_INT_HANDLER_SECTION;
static BOOL   _mtlk_pcieg6_disable_interrupts_if_pending(void *ccr_mem, uint32 mask)        __MTLK_INT_HANDLER_SECTION;
static void   _mtlk_pcieg6_initiate_doorbell_interrupt(void *ccr_mem)                       __MTLK_INT_HANDLER_SECTION;
static void   _mtlk_pcieg6_ccr_ctrl_ring_init(void *ccr_mem, void *ring_regs, unsigned char *mmb_base) __MTLK_INT_HANDLER_SECTION;
static uint32 _mtlk_pcieg6_ccr_ring_get_hds_to_process(void *ccr_mem, void *ring_regs)      __MTLK_INT_HANDLER_SECTION;
static uint32 _mtlk_pcieg6_is_interrupt_pending(void *ccr_mem)                              __MTLK_INT_HANDLER_SECTION;
static void   _mtlk_pcieg6_recover_interrupts(void *ccr_mem)                                __MTLK_INT_HANDLER_SECTION;

static void
_mtlk_pcieg6_enable_interrupts(void *ccr_mem, uint32 mask)
{
  MTLK_ASSERT(NULL != ccr_mem);

  /* Note: check the implementation for the future FPGA */
  __ccr_mem_writel(ccr_mem, mask, HOST_IF_IRQS_HOST_IRQ_MASK);
}

static void
_mtlk_pcieg6_disable_interrupts(void *ccr_mem, uint32 mask)
{
  MTLK_ASSERT(NULL != ccr_mem);

  /* Note: check the implementation for the future FPGA */
  __ccr_mem_writel(ccr_mem, MTLK_GEN6_DISABLE_INTERRUPT, HOST_IF_IRQS_HOST_IRQ_MASK);
}

static uint32
_mtlk_pcieg6_is_interrupt_pending (void *ccr_mem)
{
  uint32 status;

  MTLK_ASSERT(NULL != ccr_mem);

  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_HOST_IRQ_STATUS, status);

   return status;
}

static void
_mtlk_pcieg6_recover_interrupts (void *ccr_mem)
{
  uint32 irq_mask_dummy;
  uint32 irq_mask_cur;

  MTLK_ASSERT(NULL != ccr_mem);

  /* Read current status of interrupt mask */
  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_HOST_IRQ_MASK, irq_mask_cur);

  /* Disable interrupts */
  __ccr_mem_writel(ccr_mem, MTLK_GEN6_DISABLE_INTERRUPT, HOST_IF_IRQS_HOST_IRQ_MASK);
  mmiowb();
  /* Read back to ensure transaction is reached end point */
  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_HOST_IRQ_MASK, irq_mask_dummy);
  /* Enable interrupts even they were disabled side effect: one dummy interrupt */
  __ccr_mem_writel(ccr_mem, MTLK_IRQ_ALL, HOST_IF_IRQS_HOST_IRQ_MASK);
}

/* WARNING                                                      */
/* Currently we do not utilize (and have no plans to utilize)   */
/* interrupt sharing on Gen 6 platform. However, Gen 6          */
/* hardware supports this ability.                              */
/* For now, in all *_if_pending functions we assume there is no */
/* interrupt sharing, so we may not check whether arrived       */
/* interrupt is our. This save us CPU cycles and code lines.    */
/* In case interrupt sharing will be used, additional checks    */
/* for interrupt appurtenance to be added into these functions. */

static uint32
_mtlk_pcieg6_clear_interrupts_if_pending(void *ccr_mem, uint32 clear_mask)
{
  uint32 status;

  MTLK_ASSERT(NULL != ccr_mem);

#define MTLK_GEN6_CLEAR_RAB_INTERRUPT       (0x1)

  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_HOST_IRQ_STATUS, status);

  if (status & MTLK_IRQ_LEGACY & clear_mask) {
    __ccr_mem_writel(ccr_mem, MTLK_GEN6_CLEAR_RAB_INTERRUPT,
               HOST_IF_IRQS_PHI_INTERRUPT_CLEAR);
  }

  return status;
}

static BOOL
_mtlk_pcieg6_disable_interrupts_if_pending(void *ccr_mem, uint32 mask)
{
  _mtlk_pcieg6_disable_interrupts(ccr_mem, mask);
  return TRUE;
}

static void
_mtlk_pcieg6_initiate_doorbell_interrupt(void *ccr_mem)
{
#define MTLK_GEN6_GENERATE_DOOR_BELL    (0x1)

  MTLK_ASSERT(NULL != ccr_mem);

  __ccr_mem_writel(ccr_mem, MTLK_GEN6_GENERATE_DOOR_BELL,
               HOST_IF_IRQS_NPU2UPI_INTERRUPT_SET);
}

static void
_mtlk_pcieg6_switch_to_iram_boot(void *ccr_mem)
{
  _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != pcieg6_mem);

  pcieg6_mem->next_boot_mode = G6_CPU_RAB_IRAM;
}

static void
_mtlk_pcieg6_boot_from_bus(void *ccr_mem)
{
  _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != pcieg6_mem);

  pcieg6_mem->next_boot_mode = G6_CPU_RAB_SHRAM;
}

static void
_mtlk_pcieg6_clear_boot_from_bus(void *ccr_mem)
{
  _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != pcieg6_mem);

  pcieg6_mem->next_boot_mode = G6_CPU_RAB_DEBUG;
}

static __INLINE void
__pcieg6_open_secure_write_register_upper(void *ccr_mem)
{
  ILOG2_V("UMAC");
  wmb();
  __ccr_mem_writel(ccr_mem, 0xAAAA, SYS_IF_UM_SECURE_WRITE);
  wmb();
  __ccr_mem_writel(ccr_mem, 0x5555, SYS_IF_UM_SECURE_WRITE);
  wmb();
}

static __INLINE void
__pcieg6_open_secure_write_register_lower(void *ccr_mem, e_lmac_cpu_num  lmac_cpu_num)
{
  pcieg6_regs_t secure_write_register =
    (lmac_cpu_num == LMAC_CPU_NUM0) ?
    SYS_IF_LM0_SECURE_WRITE :
    SYS_IF_LM1_SECURE_WRITE;

  ILOG2_D("LMAC%d", lmac_cpu_num);
  wmb();
  __ccr_mem_writel(ccr_mem, 0xAAAA, secure_write_register);
  wmb();
  __ccr_mem_writel(ccr_mem, 0x5555, secure_write_register);
  wmb();
}

static __INLINE void
__pcieg6_reset_cpu_lower(void *ccr_mem, e_lmac_cpu_num  lmac_cpu_num)
{
  pcieg6_regs_t mips_ctrl =
    (lmac_cpu_num == LMAC_CPU_NUM0) ?
    SYS_IF_LM0_MIPS_CTL :
    SYS_IF_LM1_MIPS_CTL;

  ILOG0_D("Reset LMAC%d", lmac_cpu_num);
  __ccr_mem_writel(ccr_mem, G6_MIPS_RESET, mips_ctrl);
}

static __INLINE void
__pcieg6_release_cpu_lower(void *ccr_mem, e_lmac_cpu_num  lmac_cpu_num)
{
  pcieg6_regs_t mips_ctrl =
    (lmac_cpu_num == LMAC_CPU_NUM0) ?
    SYS_IF_LM0_MIPS_CTL :
    SYS_IF_LM1_MIPS_CTL;

  ILOG0_D("Release LMAC%d", lmac_cpu_num);
  __ccr_mem_writel(ccr_mem, G6_RELEASE_FROM_RESET, mips_ctrl);
}

/* #define DEBUG_CCR_CHECK_REGISTERS */
#ifdef  DEBUG_CCR_CHECK_REGISTERS
static void debug_read_registers(void *ccr_mem, char *msg)
{
  uint32 val;

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

static void _mtlk_pcieg6_reset_mac(void *ccr_mem)
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

static void
_mtlk_pcieg6_put_cpus_to_reset(void *ccr_mem)
{
  _mtlk_pcieg6_ccr_t *g6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;

  MTLK_ASSERT(NULL != g6_mem);

  /* FIXME: not used on anymore */
  g6_mem->current_ucpu_state =
    g6_mem->current_lcpu_state =
      G6_CPU_RAB_IRAM;

  {
    int i;
    for (i = LMAC_CPU_NUM0; i < LMAC_CPU_NUM_TOTAL; i++) {
      __pcieg6_open_secure_write_register_lower(ccr_mem, i);
      __pcieg6_reset_cpu_lower(ccr_mem, i);
      mtlk_osal_msleep(20);
    }
  }

  ILOG0_V("Reset UMAC");
  __pcieg6_open_secure_write_register_upper(ccr_mem);
  __ccr_mem_writel(ccr_mem, G6_MIPS_RESET, SYS_IF_UM_MIPS_CTL);
  mtlk_osal_msleep(20);

#if defined(CPTCFG_IWLWAV_G6_FPGA_IRAM_TEST)
  printk("after CPU reset\n");
  iram_rw_test(ccr_mem);
#endif
}

static __INLINE uint32
_mtlk_pcieg6_get_progmodel_version(void *ccr_mem)
{
  uint32 val;

  MTLK_ASSERT(NULL != ccr_mem);

  __ccr_mem_readl(ccr_mem, PHY_RX_TD_PHY_PROGMODEL_VER, val);

  return val;
}

/*------------------------ ABB and FW PHY libraries ------------------------*/

/* API to CCR registers access */

/* Debug print instead of real read/write */
/* #define CCR_REG_DEBUG   1 */
#define CCR_REG_DEBUG   0   /* no debug */

#if 0 /* Trace CCR functions */
#define _CCR_DBG_FUNC_          mtlk_osal_emergency_print("(%04d): %s",     __LINE__, __FUNCTION__);
#define _CCR_DBG_FUNC_ARG1(a)   mtlk_osal_emergency_print("(%04d): %s(%u)", __LINE__, __FUNCTION__, (a));
#define _CCR_DBG_FUNC_ARG2(a,b) mtlk_osal_emergency_print("(%04d): %s(%u, %u)", \
                                                            __LINE__, __FUNCTION__, (a), (b));
#else
#define _CCR_DBG_FUNC_
#define _CCR_DBG_FUNC_ARG1(a)
#define _CCR_DBG_FUNC_ARG2(a,b)
#endif

#if CCR_REG_DEBUG
/* Write REG with value */
#define _CCR_WRITE_REG_VALUE_(ccr_mem, reg_name, reg_val) \
        { mtlk_osal_emergency_print("(%4d) WR  addr 0x%08X  data 0x%08X", __LINE__, reg_name, reg_val); }
        /* __ccr_mem_writel(ccr_mem, reg_val, reg_name) */

/* Read REG value */
#define _CCR_READ_REG_VALUE_(ccr_mem, reg_name, reg_val) \
        { mtlk_osal_emergency_print("(%4d) RD  addr 0x%08X  data 0x%08X", __LINE__, reg_name, reg_val); }
        /* __ccr_mem_readl(ccr_mem, reg_name, reg_val) */

/* Read/Modify/Write secured operation by BFIELD_INFO */
#define _CCR_RMW_REG_FIELD_(ccr_mem, secure, reg_name, info, value) \
  { uint32 reg_val = 0; \
    mtlk_osal_emergency_print("(%4d) RMW addr 0x%08X  field(%d,%d)  value 0x%08X  secure %d", \
                                __LINE__, reg_name, \
                                _MTLK_BFIELD_INFO_UNPACK(info, 0), \
                                _MTLK_BFIELD_INFO_UNPACK(info, 8), \
                                value, secure); \
    _CCR_READ_REG_VALUE_(ccr_mem, reg_name, reg_val); \
    MTLK_BFIELD_SET(reg_val, info, value); \
    if (secure) __pcieg6_open_secure_write_register_upper(ccr_mem); \
    _CCR_WRITE_REG_VALUE_(ccr_mem, reg_name, reg_val); \
  }

#else /* CCR_REG_DEBUG */

/* Write REG with value */
#define _CCR_WRITE_REG_VALUE_(ccr_mem, reg_name, reg_val) \
    __ccr_mem_writel(ccr_mem, reg_val, reg_name)

/* Read REG value */
#define _CCR_READ_REG_VALUE_(ccr_mem, reg_name, reg_val) \
    __ccr_mem_readl(ccr_mem, reg_name, reg_val)

#if 0 /* FIXME: TBD */
/* Read REG by BFIELD_INFO */
#define _CCR_READ_REG_FIELD_(ccr_mem, reg_name, info) \
  { uint32 reg_val; \
    _CCR_READ_REG_VALUE_(ccr_mem, reg_name, reg_val); \
    return MTLK_BFIELD_GET(reg_val, info); \
  }
#endif

/* Read/Modify/Write secured operation by BFIELD_INFO */
#define _CCR_RMW_REG_FIELD_(ccr_mem, secure, reg_name, info, value) \
  { uint32 reg_val; \
    MTLK_HW_RW_REORD_DELAY; \
    _CCR_READ_REG_VALUE_(ccr_mem, reg_name, reg_val); \
    rmb(); \
    MTLK_BFIELD_SET(reg_val, info, value); \
    if (secure) __pcieg6_open_secure_write_register_upper(ccr_mem); \
    _CCR_WRITE_REG_VALUE_(ccr_mem, reg_name, reg_val); \
    wmb(); \
  }

#endif /* CCR_REG_DEBUG */


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

#if 0 /* is printed below with a read value */
    mtlk_osal_emergency_print("(%4d) FCSI_READ  ms %u, rf %u, addr 0x%04X",
                              __LINE__, ms_access, rf_access, addr);
#endif

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

    mtlk_osal_emergency_print("(%4d) FCSI_READ  ms %u, rf %u, addr 0x%04X, data 0x%04X",
                              __LINE__, ms_access, rf_access, addr, *rd_data);
    }
}

static void
_phy_lib_FCSI_WRITE_TRANSACTION_CENTRAL (void *ccr_mem, uint16 ms_access, uint16 rf_access,
                                         uint16 addr, uint16 data, BOOL *success)
{

    uint32 reg_val;

    mtlk_osal_emergency_print("(%4d) FCSI_WRITE ms %u, rf %u, addr 0x%04X, data 0x%04X",
                              __LINE__, ms_access, rf_access, addr, data);

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

    mtlk_osal_emergency_print("(%4d) wrToAfeRegC addr 0x%04X  value 0x%04X",
                              __LINE__, regAddr, regValue);

#if 1 /* FIXME: to be removed */
        _phy_lib_FCSI_READ_TRANSACTION_CENTRAL(ccr_mem, 1, 0, regAddr, &centralVal, &success);
#endif
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
      __pcieg6_open_secure_write_register_upper(ccr_mem);
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

#define _DBG_(n)  mtlk_osal_emergency_print("%-20s 0x%02X", #n, n);

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

static int _mtlk_pcieg6_efuse_timer_config (void *ccr_mem);
uint8 * __MTLK_IFUNC    wave_hw_get_afe_data (mtlk_hw_t *hw, unsigned *size);

static int
_mtlk_pcieg6_init_pll (void *ccr_mem, uint32 chip_id)
{
  _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != ccr_mem);

  _CCR_DBG_FUNC_;

  if (!wave_hw_mmb_card_is_asic(pcieg6_mem->hw)) {
    WLOG_V("Skip PLL init due to non ASIC");
#if CCR_REG_DEBUG
    /* continue */
#else
    return MTLK_ERR_OK;
#endif
  }

  /* Switch to XO clock */
#if 0 /* FIXME: required ? */
  _abb_cpu_clock_select(ccr_mem, SYS_IF_UM_CLOCK_SELECT_CPU_CLOCK_SELECT_XO);
#endif

  /* take PHY out of reset */
  _phy_lib_FCSI_RESET(ccr_mem);

  _abb_fcsi_enable (ccr_mem);

  /* Apply AFE calibration data from eFuse */
  {
    wave_efuse_afe_data_v6_t  *afe_data;
    uint32  size;

    afe_data = (wave_efuse_afe_data_v6_t *)wave_hw_get_afe_data(pcieg6_mem->hw, &size);
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
  res = _mtlk_pcieg6_efuse_timer_config(ccr_mem);

  return res;
}

/*-------------------------------------------------------------------*/

static void
_mtlk_pcieg6_put_ctl_to_reset(void *ccr_mem)
{
  MTLK_ASSERT(NULL != ccr_mem);

#if defined(CPTCFG_IWLWAV_G6_FPGA_IRAM_TEST)
  printk("before put_ctl_to_reset\n");
  iram_rw_test(ccr_mem);
#endif

  /* Disable RX */
  __ccr_mem_resetl(ccr_mem, PAC_RX_CONTROL, G6_MASK_RX_ENABLED);

#if defined(CPTCFG_IWLWAV_G6_FPGA_IRAM_TEST)
  printk("after put_ctl_to_reset\n");
  iram_rw_test(ccr_mem);
#endif
}

static void
_mtlk_pcieg6_power_on_cpus(void *ccr_mem)
{
  MTLK_ASSERT(NULL != ccr_mem);

  /* Enable all MAC GenRisc CPU clocks */
  _CCR_RMW_REG_FIELD_(ccr_mem, 0, MAC_HT_EXT_CLK_CONTROL_REG_DYN_EN,
                            MAC_GEN_REGS_CLK_CONTROL_REG_DYN_EN_Q_MANAGERS03,
                            MAC_GEN_REGS_CLK_CONTROL_REG_DYN_EN_Q_MANAGERS03_ENABLE);
}

static void
_mtlk_pcieg6_release_cpus_reset(void *ccr_mem)
{
  _mtlk_pcieg6_ccr_t *g6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;

  MTLK_ASSERT(NULL != ccr_mem);
  MTLK_ASSERT(G6_CPU_RAB_IRAM == g6_mem->next_boot_mode);

  /* FIXME: not used anymore */
  g6_mem->current_ucpu_state =
    g6_mem->current_lcpu_state =
      G6_CPU_RAB_Active | g6_mem->next_boot_mode;

  {
    int i;
    for (i = LMAC_CPU_NUM0; i < LMAC_CPU_NUM_TOTAL; i++) {
      __pcieg6_open_secure_write_register_lower(ccr_mem, i);
      __pcieg6_release_cpu_lower(ccr_mem, i);
      mtlk_osal_msleep(20);
    }
  }

  ILOG0_V("Release UMAC");
  __pcieg6_open_secure_write_register_upper(ccr_mem);
  __ccr_mem_writel(ccr_mem, G6_RELEASE_FROM_RESET, SYS_IF_UM_MIPS_CTL);
  mtlk_osal_msleep(10);
}

static BOOL
_mtlk_pcieg6_check_bist(void *ccr_mem, uint32 *bist_result)
{
  MTLK_ASSERT(NULL != ccr_mem);
  /* Not needed on G6 */
  MTLK_UNREFERENCED_PARAM(ccr_mem);

  *bist_result = 0;
  return TRUE;
}

static int
_mtlk_pcieg6_eeprom_override_write (void *ccr_mem, uint32 data, uint32 mask)
{
  uint32 val;

  MTLK_ASSERT(NULL != ccr_mem);

  __ccr_mem_readl(ccr_mem, SOC_REGS_EEPROM_SW_ACCESS, val);

  val = (val & (~mask))| data;

  __ccr_mem_writel(ccr_mem, val, SOC_REGS_EEPROM_SW_ACCESS);

  return MTLK_ERR_OK;
}

static uint32
_mtlk_pcieg6_eeprom_override_read (void *ccr_mem)
{
  uint32 val;

  MTLK_ASSERT(NULL != ccr_mem);

  __ccr_mem_readl(ccr_mem, SOC_REGS_EEPROM_SW_ACCESS, val);

  return val;
}

static uint32
_mtlk_pcieg6_ccr_get_mips_freq(void *ccr_mem, uint32 chip_id)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  return PCIEG6_MIPS_FREQ_DEFAULT;
}

static int
_mtlk_pcieg6_ccr_init(void *ccr_mem, mtlk_hw_t *hw, void *bar0, void *bar1, void *bar1_phy, void *dev_ctx, void **shram)
{
  _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != pcieg6_mem);

  pcieg6_mem->hw = hw;
  pcieg6_mem->pas = bar1;
  pcieg6_mem->pas_physical = bar1_phy;
  pcieg6_mem->dev_ctx = dev_ctx;

  *shram = bar1 + SHRAM_WRITABLE_OFFSET;

  /* This is a state of cpu on power-on */
  pcieg6_mem->current_ucpu_state =
  pcieg6_mem->current_lcpu_state = G6_CPU_RAB_Active | G6_CPU_RAB_DEBUG;

  pcieg6_mem->next_boot_mode = G6_CPU_RAB_DEBUG;

  return _mtlk_ccr_pcieg6_all_regs_validate();
}

static void _mtlk_pcieg6_ccr_cleanup(void *ccr_mem)
{
  _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != pcieg6_mem);

  /* cleanup all values at once */
  memset(pcieg6_mem, 0, sizeof(_mtlk_pcieg6_ccr_t));
}

#if defined (CPTCFG_IWLWAV_G6_BAR0_TEST)
#include <linux/random.h>
void _bar0_test_code(void *ccr_mem, char *msg, void *data_tx_r, void *data_rx_r, void *bss_tx_r, void *bss_rx_r) {
#define _SEED_VAL 0xfee1dead
#define _BAR0_OFFSET0 0x2E35F0
#define _BAR0_OFFSET1 0x2E35F4
#define _ITERATIONS_NUM 100000

  mtlk_ring_regs *dtx_r = (mtlk_ring_regs *)data_tx_r;
  mtlk_ring_regs *drx_r = (mtlk_ring_regs *)data_rx_r;
  mtlk_ring_regs *btx_r = (mtlk_ring_regs *)bss_tx_r;
  mtlk_ring_regs *brx_r = (mtlk_ring_regs *)bss_rx_r;
 _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  uint32 *ptr0 = (uint32 *)((uint8 *)pcieg6_mem->pas + _BAR0_OFFSET0);
  uint32 *ptr1 = (uint32 *)((uint8 *)pcieg6_mem->pas + _BAR0_OFFSET1);
  uint32 read_val0, read_val1, rand_val0, rand_val1;
  uint32 /*dtx_st, drx_st,*/ btx_st, brx_st;
  int i;

  MTLK_ASSERT(NULL != pcieg6_mem);
  MTLK_ASSERT(NULL != dtx_r);
  MTLK_ASSERT(NULL != drx_r);
  MTLK_ASSERT(NULL != btx_r);
  MTLK_ASSERT(NULL != brx_r);

  printk("--> start write-read test step1 %s\n", msg);
  for (i = 0; i < _ITERATIONS_NUM; i++) {
    prandom_seed(_SEED_VAL);
    rand_val0 = get_random_int();
    rand_val1 = get_random_int();
    __ccr_mem_writel(ccr_mem, rand_val0, ptr0);
    __ccr_mem_writel(ccr_mem, rand_val1, ptr1);
    __ccr_mem_readl(ccr_mem, ptr0, read_val0);
    __ccr_mem_readl(ccr_mem, ptr1, read_val1);
    if (read_val0 != rand_val0)
      printk("---> wrong read try %d from addr0 %p read 0x%x expected 0x%x\n", i, ptr0, read_val0, rand_val0);
    if (read_val1 != rand_val1)
      printk("---> wrong read try %d from addr1 %p read 0x%x expected 0x%x\n", i, ptr1, read_val1, rand_val1);
  }
  printk("--> stop write-read test step1\n");

  printk("--> start write-read test step2 %s\n", msg);
  for (i = 0; i < _ITERATIONS_NUM; i++) {
    prandom_seed(_SEED_VAL);
    rand_val0 = get_random_int();
    rand_val1 = get_random_int();
    __ccr_mem_writel(ccr_mem, rand_val0, ptr0);
    __ccr_mem_readl(ccr_mem, ptr0, read_val0);
    __ccr_mem_writel(ccr_mem, rand_val1, ptr1);
    __ccr_mem_readl(ccr_mem, ptr1, read_val1);
#if 0
    dtx_st = LE32_TO_CPU(mtlk_raw_readl(dtx_r->p_OUT_status));
    drx_st = LE32_TO_CPU(mtlk_raw_readl(drx_r->p_OUT_status));
#endif
    btx_st = LE32_TO_CPU(mtlk_raw_readl(btx_r->p_OUT_status));
    brx_st = LE32_TO_CPU(mtlk_raw_readl(brx_r->p_OUT_status));
    if (read_val0 != rand_val0)
      printk("---> wrong read try %d from addr0 %p read 0x%x expected 0x%x\n", i, ptr0, read_val0, rand_val0);
    if (read_val1 != rand_val1)
      printk("---> wrong read try %d from addr1 %p read 0x%x expected 0x%x\n", i, ptr1, read_val1, rand_val1);
#if 0
    if (dtx_st)
      printk("---> read number %d data tx status: %d\n", i, dtx_st);
    if (drx_st)
      printk("---> read number %d data rx status: %d\n", i, drx_st);
#endif
    if (btx_st)
      printk("---> read number %d bss tx status: %d\n", i, btx_st);
    if (brx_st)
      printk("---> read number %d bss rx status: %d\n", i, brx_st);
  }
  printk("--> stop write-read test step2\n");
}
#endif

static void _mtlk_pcieg6_set_msi_intr_mode (void *ccr_mem, uint32 irq_mode)
{
#ifdef CPTCFG_IWLWAV_X86_PUMA7_WAV600_NOSNOOP
  RegSocRegsPcieAppsNsCtrl_u ns;
#endif

  MTLK_ASSERT(NULL != ccr_mem);

#if defined(CPTCFG_IWLWAV_G6_FPGA_IRAM_TEST)
  printk("before set_msi_intr_mode\n");
  iram_rw_test(ccr_mem);
#endif

#ifdef CPTCFG_IWLWAV_X86_PUMA7_WAV600_NOSNOOP
  ns.val = 0;
  ns.bitFields.pcie0RdTlpNoSnoop = TRUE; /* SoC --> Wave600 card */
  ns.bitFields.pcie0WrTlpNoSnoop = TRUE; /* Wave600 card --> SoC */
  ns.bitFields.pcie1RdTlpNoSnoop = TRUE;
  ns.bitFields.pcie1WrTlpNoSnoop = TRUE;
  ILOG0_DD("No snoop PCIe mode: SOC_REGS_PCIE_APPS_NS_CTRL=%x, val=%x\n", SOC_REGS_PCIE_APPS_NS_CTRL, ns.val);
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

static void
_mtlk_pcieg6_ccr_ctrl_ring_init(void *ccr_mem, void *ring_regs, unsigned char *mmb_base)
{
  mtlk_ring_regs *regs = (mtlk_ring_regs *)ring_regs;

  MTLK_ASSERT(NULL != regs);
  MTLK_UNREFERENCED_PARAM(mmb_base);

  regs->p_IN_cntr    = (uint32*)(CCR_MEM_ADDR(ccr_mem, HOST_IF_ACC_HOST_MAC_MAILBOX_ACCUM_ADD));
  regs->p_OUT_status = (uint32*)(CCR_MEM_ADDR(ccr_mem, HOST_IF_ACC_MAC_HOST_MAILBOX_ACCUM_COUNT));
  regs->p_OUT_cntr   = (uint32*)(CCR_MEM_ADDR(ccr_mem, HOST_IF_ACC_MAC_HOST_MAILBOX_ACCUM_SUB));
  regs->int_OUT_cntr = mtlk_raw_readl(regs->p_OUT_status);

  ILOG0_PPP("[Ring Regs] p_IN_cntr:0x%p, p_OUT_status:0x%p, p_OUT_cntr:0x%p",
  	regs->p_IN_cntr, regs->p_OUT_status, regs->p_OUT_cntr);

  MTLK_ASSERT(0 == regs->int_OUT_cntr);

  return;
}

static uint32
_mtlk_pcieg6_ccr_ring_get_hds_to_process (void *ccr_mem, void *ring_regs)
{
  mtlk_ring_regs *regs = (mtlk_ring_regs *)ring_regs;
  uint32 value;

  MTLK_ASSERT(NULL != regs);

  /* p_tx_OUT_status already contains number of HDs */
  value = mtlk_raw_readl(regs->p_OUT_status);
  rmb();
  return LE32_TO_CPU(value);
}

static void
_mtlk_pcieg6_ccr_ring_clear_interrupt (void *ring_regs, uint32 hds_processed)
{
  mtlk_ring_regs *regs = (mtlk_ring_regs *)ring_regs;

  MTLK_ASSERT(NULL != regs);

  regs->int_OUT_cntr += hds_processed;

  /* clear interrupt */
  mtlk_raw_writel(CPU_TO_LE32(hds_processed), regs->p_OUT_cntr);
  wmb();
}

static void
_mtlk_pcieg6_ccr_ring_initiate_doorbell_interrupt (void *ccr_mem, void *ring_regs)
{
  mtlk_ring_regs *regs = (mtlk_ring_regs*)ring_regs;

  MTLK_ASSERT(NULL != regs);

  /* just one HD added, accumulator register */
  mtlk_raw_writel(CPU_TO_LE32(1), regs->p_IN_cntr);
  wmb();
}

static void
_mtlk_pcieg6_eeprom_init (void *ccr_mem)
{
  uint32 reg_val;

#define EEPROM_PULLUP_GPIO_SCL (1<<4)
#define EEPROM_PULLUP_GPIO_SDA (1<<5)

  MTLK_ASSERT(NULL != ccr_mem);

  __ccr_mem_readl(ccr_mem, SOC_REGS_GPIO_PULLUP, reg_val);
  reg_val |= EEPROM_PULLUP_GPIO_SCL;
  reg_val |= EEPROM_PULLUP_GPIO_SDA;
  __ccr_mem_writel(ccr_mem, reg_val, SOC_REGS_GPIO_PULLUP);
}

/*------ PCIEG6 EFUSE mapping ---------------------*/

#define PCIEG6_MAX_EFUSE_SIZE                       (128)   /* 1024 bits = 128 bytes = 32 words */
#define PCIEG6_EFUSE_STATUS_READ_MAX                (1000)  /* max number of attempts to read */

/* SOC_REGS_EFUSE_ADDRESS */
#define SOC_REGS_EFUSE_BITS_PER_WORD                (2 << 5) /* uint32 word access */
#define SOC_REGS_EFUSE_ADDRESS_BIT_ADDR             MTLK_BFIELD_INFO(0, 5) /* for write ops only */
#define SOC_REGS_EFUSE_ADDRESS_WORD_ADDR            MTLK_BFIELD_INFO(5, 5) /* for read and wrire */

/* SOC_REGS_EFUSE_CONTROL */
#define SOC_REGS_EFUSE_CONTROL_EFUSE_WR_RDN         MTLK_BFIELD_INFO(0, 1) /* 0 - read, 1 - write */
#define SOC_REGS_EFUSE_CONTROL_EFUSE_READ_REQ       MTLK_BFIELD_VALUE(SOC_REGS_EFUSE_CONTROL_EFUSE_WR_RDN, 0, uint32)
#define SOC_REGS_EFUSE_CONTROL_EFUSE_WRITE_REQ      MTLK_BFIELD_VALUE(SOC_REGS_EFUSE_CONTROL_EFUSE_WR_RDN, 1, uint32)

/* SOC_REGS_EFUSE_STATUS */
#define SOC_REGS_EFUSE_STATUS_EFUSE_BUSY            MTLK_BFIELD_INFO(0, 1) /* 0 - cleared */
#define SOC_REGS_EFUSE_STATUS_EFUSE_BUSY_CLEARED    (0)
#define SOC_REGS_EFUSE_STATUS_EFUSE_INIT_DONE       MTLK_BFIELD_INFO(1, 1) /* 1 - init done */
#define SOC_REGS_EFUSE_STATUS_EFUSE_INIT_DONE_SET   (1)
#define SOC_REGS_EFUSE_STATUS_EFUSE_FSM_STATE       MTLK_BFIELD_INFO(2, 3) /* state machine (for debug) */

/* SOC_REGS_EFUSE_PCIE_FSM_OVR */
#define SOC_REGS_EFUSE_PCIE_FSM_OVR_CTL_OVERRIDE    (1)
#define SOC_REGS_EFUSE_PCIE_FSM_OVR_CTL_NO_OVERRIDE (0)

/* SOC_REGS_EFUSE_PD */
#define SOC_REGS_EFUSE_PD_DISABLE                   (0)
#define SOC_REGS_EFUSE_PD_ENABLE                    (1)

/* SOC_REGS_EFUSE_TIMER_CONFIG1 */
#define SOC_REGS_EFUSE_TIMER_CONFIG1_EFUSE_TPGM             MTLK_BFIELD_INFO( 0, 16)
#define SOC_REGS_EFUSE_TIMER_CONFIG1_EFUSE_TSU_CS_STROBE    MTLK_BFIELD_INFO(16, 16)

/* SOC_REGS_EFUSE_TIMER_CONFIG2 */
#define SOC_REGS_EFUSE_TIMER_CONFIG2_EFUSE_TRD              MTLK_BFIELD_INFO( 0, 16)
#define SOC_REGS_EFUSE_TIMER_CONFIG2_EFUSE_TSU_PS_CSB       MTLK_BFIELD_INFO(16, 16)

#define SOC_REGS_EFUSE_TIMER_CONFIG1_EFUSE_TPGM_US          (12)  /* microseconds */
#define SOC_REGS_EFUSE_TIMER_CONFIG1_EFUSE_TSU_CS_STROBE_NS (8)   /* nanoseconds */
#define SOC_REGS_EFUSE_TIMER_CONFIG2_EFUSE_TRD_NS           (200) /* nanoseconds */
#define SOC_REGS_EFUSE_TIMER_CONFIG2_EFUSE_TSU_PS_CSB_NS    (71)  /* nanoseconds */

/* Configure EFUSE timers according to MAC clocking */
static int
_mtlk_pcieg6_efuse_timer_config (void *ccr_mem)
{
    uint32  clk_sel, clk_mhz;
    uint32  field1, field2; /* fields */
    uint32  timer1, timer2; /* words */

    _CCR_READ_REG_VALUE_(ccr_mem, SYS_IF_UM_CLOCK_SELECT, clk_sel);
    clk_sel = MTLK_BFIELD_GET(clk_sel, SYS_IF_UM_CLOCK_SELECT_CPU_CLOCK_SELECT);

    switch (clk_sel)
    {
      case SYS_IF_UM_CLOCK_SELECT_CPU_CLOCK_SELECT_XO:
        clk_mhz = WLAN_CLOCK_XO_MHZ;
        break;
      case SYS_IF_UM_CLOCK_SELECT_CPU_CLOCK_SELECT_PLL:
        clk_mhz = WLAN_CLOCK_PLL_MHZ;
        break;
      default:
        WLOG_D("Don't configure EFUSE timers due to wrong CLOCK_SELECT value %u", clk_sel);
        return MTLK_ERR_NOT_SUPPORTED;
    }

    field1 = WLAN_TIME_US_TO_CLOCKS_MHZ(clk_mhz, SOC_REGS_EFUSE_TIMER_CONFIG1_EFUSE_TPGM_US);
    field2 = WLAN_TIME_NS_TO_CLOCKS_MHZ(clk_mhz, SOC_REGS_EFUSE_TIMER_CONFIG1_EFUSE_TSU_CS_STROBE_NS);

    timer1 = MTLK_BFIELD_VALUE(SOC_REGS_EFUSE_TIMER_CONFIG1_EFUSE_TPGM,          field1, uint32) |
             MTLK_BFIELD_VALUE(SOC_REGS_EFUSE_TIMER_CONFIG1_EFUSE_TSU_CS_STROBE, field2, uint32);

    /* TRD requires additional 4 clock cycles */
    field1 = WLAN_TIME_NS_TO_CLOCKS_MHZ(clk_mhz, SOC_REGS_EFUSE_TIMER_CONFIG2_EFUSE_TRD_NS) + 4;
    field2 = WLAN_TIME_NS_TO_CLOCKS_MHZ(clk_mhz, SOC_REGS_EFUSE_TIMER_CONFIG2_EFUSE_TSU_PS_CSB_NS);

    timer2 = MTLK_BFIELD_VALUE(SOC_REGS_EFUSE_TIMER_CONFIG2_EFUSE_TRD,           field1, uint32) |
             MTLK_BFIELD_VALUE(SOC_REGS_EFUSE_TIMER_CONFIG2_EFUSE_TSU_PS_CSB,    field2, uint32);

    ILOG1_DDD("Set EFUSE timers to 0x%08X and 0x%08X for clock %u MHz", timer1, timer2, clk_mhz);

    _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_TIMER_CONFIG1, timer1);
    _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_TIMER_CONFIG2, timer2);

    return MTLK_ERR_OK;
}

static __INLINE BOOL
__mtlk_pcieg6_efuse_status_is_busy_cleared (uint32 reg_val)
{
  return (MTLK_BFIELD_GET(reg_val, SOC_REGS_EFUSE_STATUS_EFUSE_BUSY) ==
                                   SOC_REGS_EFUSE_STATUS_EFUSE_BUSY_CLEARED);
}

static __INLINE BOOL
__mtlk_pcieg6_efuse_status_is_init_done_set (uint32 reg_val)
{
  return (MTLK_BFIELD_GET(reg_val, SOC_REGS_EFUSE_STATUS_EFUSE_INIT_DONE) ==
                                   SOC_REGS_EFUSE_STATUS_EFUSE_INIT_DONE_SET);
}

static int
_mtlk_pcieg6_efuse_open (_mtlk_pcieg6_ccr_t *ccr_mem)
{
  uint32 reg_val;
  BOOL   status = FALSE;
  int    res;
  int    n;

  /* Configure EFUSE timers according to the current clock */
  res = _mtlk_pcieg6_efuse_timer_config(ccr_mem);
  if (MTLK_ERR_OK != res) {
    return res; /* already logged */
  }

  /* 1. Disable PD bit in efuse_PD register and set control "override" */
  _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_PD, SOC_REGS_EFUSE_PD_DISABLE);
  _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_PCIE_FSM_OVR, SOC_REGS_EFUSE_PCIE_FSM_OVR_CTL_OVERRIDE);

  /* 2. Check eFuse control status for busy bit is cleared and init_done bit is set */
  for (n = 0; ((!status) && (n < PCIEG6_EFUSE_STATUS_READ_MAX)); n++) {
    _CCR_READ_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_STATUS, reg_val);
    status = (__mtlk_pcieg6_efuse_status_is_busy_cleared(reg_val) &&
              __mtlk_pcieg6_efuse_status_is_init_done_set(reg_val));
  }

  if (status) {
      return MTLK_ERR_OK;
  } else {
    ELOG_D("eFuse open: status not ready: 0x%02X", reg_val);
    return MTLK_ERR_NOT_READY;
  }
}

static void
_mtlk_pcieg6_efuse_close(void *ccr_mem)
{
  /* 7. Set control "no override" and Enable PD bit in efuse_PD register */
  _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_PCIE_FSM_OVR, SOC_REGS_EFUSE_PCIE_FSM_OVR_CTL_NO_OVERRIDE);
  _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_PD, SOC_REGS_EFUSE_PD_ENABLE);
}

static __INLINE BOOL
__mtlk_pcieg6_efuse_status_busy_cleared (void *ccr_mem)
{
  uint32 reg_val;
  int    n;
  BOOL   status = FALSE;

  /* 5. Poll the eFuse status register until the control_busy bit is cleared */
  for (n = 0; ((!status) && (n < PCIEG6_EFUSE_STATUS_READ_MAX)); n++) {
    _CCR_READ_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_STATUS, reg_val);
    status = __mtlk_pcieg6_efuse_status_is_busy_cleared(reg_val);
  }

  if (!status) {
    ELOG_D("eFuse busy timeout, eFuse status 0x%08X", reg_val);
  }

  return status;
}

/* Checking for eFuse access addr/size/buf */
/* Gen6 specific: word access */
static BOOL
_mtlk_pcieg6_efuse_access_check (const char *op, void *buf, uint32 addr, uint32 size)
{
  ILOG0_SDDP("eFuse %s: addr %u, size %u, buf 0x%p", op, addr, size, buf);
  if ((sizeof(uint32) - 1) & ((wave_addr)buf | addr | size)) {
    ELOG_S("eFuse %s: unaligned uint32 access: at least one of buf/addr/size", op);
    return FALSE;
  } else
  if (PCIEG6_MAX_EFUSE_SIZE <= addr ||
      PCIEG6_MAX_EFUSE_SIZE <  size ||
      PCIEG6_MAX_EFUSE_SIZE <  (addr + size))
  {
    ELOG_SDD("eFuse %s: addr and/or size too big: %u, %u", op, addr, size);
    return FALSE;
  }

  return TRUE;
}

/* Read data from eFuse */
/* Gen6 specific: word access */
/* Return value - number of bytes was read */
static uint32
_mtlk_pcieg6_efuse_read_data (void *ccr_mem, void *buf, uint32 addr, uint32 size)
{
  _mtlk_pcieg6_ccr_t *pcieg6_mem = (_mtlk_pcieg6_ccr_t *)ccr_mem;
  uint32    word_addr, word_num;
  uint32    bytes_read = 0;
  uint32   *words, word;

  MTLK_ASSERT(NULL != pcieg6_mem);
  MTLK_ASSERT(NULL != buf);

  if (!_mtlk_pcieg6_efuse_access_check("read", buf, addr, size)) {
    return 0; /* bytes_read */
  }

  if (MTLK_ERR_OK != _mtlk_pcieg6_efuse_open(pcieg6_mem))
    goto FINISH;

  words = (uint32 *)buf;
  word_addr = addr / sizeof(uint32);
  for (word_num = 0; word_num < (size / sizeof(uint32)); word_num++, word_addr++) {
    /* Write the Word Address */
    _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_ADDRESS,
                          MTLK_BFIELD_VALUE(SOC_REGS_EFUSE_ADDRESS_WORD_ADDR, word_addr, uint32));

    /* Write "Read Request" to the eFuse control register. */
    _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_CONTROL, SOC_REGS_EFUSE_CONTROL_EFUSE_READ_REQ);

    /* Wait for request is finished */
    if (FALSE == __mtlk_pcieg6_efuse_status_busy_cleared(ccr_mem))
      goto FINISH;

    /* Read the value from the eFuse data register */

    _CCR_READ_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_DATA, word);
    words[word_num] = HOST_TO_MAC32(word); /* bytes reordering */
    ILOG1_DDD("eFuse[%2u] = 0x%08X (0x%08X)", word_num, words[word_num], word);
  }

  bytes_read = word_num * sizeof(uint32); /* all data complete */

FINISH:
  _mtlk_pcieg6_efuse_close(pcieg6_mem);

  return bytes_read;
}

static void
_mtlk_pcieg6_efuse_pre_write_check(void *ccr_mem, uint8 *check_buf, uint32 addr, uint32 size)
{
#define EFUSE_CLEAN_PATTERN 0x00
  int i;

  if (!_mtlk_pcieg6_efuse_read_data(ccr_mem, check_buf, addr, size)) {
    ELOG_V("eFuse read error: Pre-write validation is not possible");
    return;
  }

  for (i = 0; i < size; i++) {
    if (EFUSE_CLEAN_PATTERN != check_buf[i]) {
      ELOG_V("eFuse not clean: correct write may not be possible!");
      mtlk_dump(0, check_buf, size, "eFuse target area:");
      break;
    }
  }
}

static void
_mtlk_pcieg6_efuse_post_write_check(void *ccr_mem, uint8 *check_buf, uint8 *write_buf, uint32 addr, uint32 size)
{
  int i;
  BOOL write_err = FALSE;

  if (!_mtlk_pcieg6_efuse_read_data(ccr_mem, check_buf, addr, size)) {
    ELOG_V("eFuse read error: Post-write validation is not possible");
    return;
  }

  for (i = 0; i < size; i++) {
    if (write_buf[i] != check_buf[i]) {
      write_err = TRUE;
      WLOG_DDD("eFuse write validation error: offset: %d, written: 0x%x, read back: 0x%x",
        (i + addr), write_buf[i], check_buf[i]);
    }
  }
  if (!write_err) {
    ILOG0_V("eFuse write validation passed");
  }
}

/* Write data to eFuse */
/* Gen6 specific: word access */
/* Return value - number of bytes was written */
/* */
static uint32
_mtlk_pcieg6_efuse_write_data(void *ccr_mem, void *buf, uint32 addr, uint32 size)
{
  uint32    word_addr, word_num, bit_num;
  uint32    bytes_written = 0, offset = addr;
  uint32    *words;
  uint8     *check_buf = NULL;

  MTLK_ASSERT(NULL != ccr_mem);
  MTLK_ASSERT(NULL != buf);

  ELOG_V("Gen6 EFUSE writing is unsupported");
  return 0;

  if (!_mtlk_pcieg6_efuse_access_check("write", buf, addr, size)) {
    goto FAIL;
  }

  /* check if eFuse target area is clean */
  check_buf = mtlk_osal_mem_alloc(PCIEG6_MAX_EFUSE_SIZE, MTLK_MEM_TAG_EEPROM);
  if (NULL == check_buf) {
    ELOG_V("Failed to allocate validation buffer");
    goto FAIL;
  }
  _mtlk_pcieg6_efuse_pre_write_check(ccr_mem, check_buf, offset, size);

  if (MTLK_ERR_OK != _mtlk_pcieg6_efuse_open(ccr_mem))
    goto FINISH;

  mtlk_dump(0, buf, size, "eFuse data to write:");

  words = (uint32 *)buf;
  word_addr = addr / sizeof(uint32);
  for (word_num = 0; word_num < (size / sizeof(uint32)); word_num++, word_addr++) {
    /* Write 1 word bitwise, BIT0 is first */
    for (bit_num = 0; bit_num < SOC_REGS_EFUSE_BITS_PER_WORD; bit_num++) {
      /* only 1s are burned */
      if (words[word_num] & (1u << bit_num)) {
        /* compose word and bit adress */
        _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_ADDRESS,
                              (MTLK_BFIELD_VALUE(SOC_REGS_EFUSE_ADDRESS_BIT_ADDR,  bit_num,   uint32) |
                               MTLK_BFIELD_VALUE(SOC_REGS_EFUSE_ADDRESS_WORD_ADDR, word_addr, uint32)));

        /* Write "Write Request" to the eFuse control register */
        _CCR_WRITE_REG_VALUE_(ccr_mem, SOC_REGS_EFUSE_CONTROL, SOC_REGS_EFUSE_CONTROL_EFUSE_WRITE_REQ);

        /* wait for writing is finished */
        if (FALSE == __mtlk_pcieg6_efuse_status_busy_cleared(ccr_mem)) {
          ELOG_DDD("eFuse write error! start_addr: 0x%04X, word:0x%02X, bit: %2u",
                    (int)addr, word_num, bit_num);
          goto FINISH;
        }
      }
    }
  }

  bytes_written = word_num * sizeof(uint32);

FINISH:
  _mtlk_pcieg6_efuse_close(ccr_mem);

  /* verify write */
  if (bytes_written) {
    _mtlk_pcieg6_efuse_post_write_check(ccr_mem, check_buf, buf, offset, size);
  }

  if (check_buf) {
    mtlk_osal_mem_free(check_buf);
  }

FAIL:
  return bytes_written;
}

/* Get eFuse size */
/* Return value - number of bytes in eFuse */
static uint32
_mtlk_pcieg6_efuse_get_size(void *ccr_mem)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  return PCIEG6_MAX_EFUSE_SIZE;
}

/* Power data per antenna are in 0.5 dB units i.e. in S8.1 format */
#define PW_FIXP_NBI     (8) /* number of bits of integer part */
#define PW_FIXP_NBF     (1) /* number of bits of fractional part */
#define PW_NOF_ANTS     (4) /* number of antennas */

static int
_mtlk_pcieg6_read_phy_inband_power(void *ccr_mem, unsigned radio_id, uint32 ant_mask,
                                   int32 *noise_estim, int32 *system_gain)
{
  uint32 ant_offs;
  uint32 reg_addr, value;
  int ant_num;

  ILOG2_DD("RadioID %u ant_mask 0x%02X", radio_id, ant_mask);

  for (ant_num = 0; ant_num < PW_NOF_ANTS; ant_num++) {
    if (0 == (ant_mask & (1 << ant_num))) {
      /* default values for unavailable antenna */
      noise_estim[ant_num] = FIXP_MIN_INT(PW_FIXP_NBI, PW_FIXP_NBF);
      system_gain[ant_num] = 0;
    } else {
      switch (ant_num) {
        default:
        case 0: ant_offs = 0; break;
        case 1: ant_offs = B0_PHY_AGC_ANT1_OFFSET_FROM_BAR0 - B0_PHY_AGC_ANT0_OFFSET_FROM_BAR0; break;
        case 2: ant_offs = B0_PHY_AGC_ANT2_OFFSET_FROM_BAR0 - B0_PHY_AGC_ANT0_OFFSET_FROM_BAR0; break;
        case 3: ant_offs = B0_PHY_AGC_ANT3_OFFSET_FROM_BAR0 - B0_PHY_AGC_ANT0_OFFSET_FROM_BAR0; break;
      }

      /* Register addresses depend on the Radio ID */
      reg_addr = (0 == radio_id) ? B0_PHY_AGC_ANT0_IB_POWER_REG
                                 : B1_PHY_AGC_ANT0_IB_POWER_REG;

      _CCR_READ_REG_VALUE_(ccr_mem, ant_offs + reg_addr, value);
      value = MTLK_BFIELD_GET(value, PHY_AGC_ANT0_IB_POWER_IBPOWER);
      noise_estim[ant_num] = FIXP_TO_INT(value, PW_FIXP_NBI, PW_FIXP_NBF);

      reg_addr = (0 == radio_id) ? B0_PHY_AGC_ANT0_INBAND_RSSI_SYSTEM_GAIN_REG
                                 : B1_PHY_AGC_ANT0_INBAND_RSSI_SYSTEM_GAIN_REG;

      _CCR_READ_REG_VALUE_(ccr_mem, ant_offs + reg_addr, value);
      value = MTLK_BFIELD_GET(value, PHY_AGC_ANT0_INBAND_RSSI_SYSTEM_GAIN_REG_IBGAIN);
      system_gain[ant_num] = FIXP_TO_INT(value, PW_FIXP_NBI, PW_FIXP_NBF);
    }
  }

  return MTLK_ERR_OK;
}

static const mtlk_ccr_api_t mtlk_ccr_api = {
  _mtlk_pcieg6_ccr_init,
  _mtlk_pcieg6_ccr_cleanup,
  _mtlk_pcieg6_ccr_read_chip_id,
  _mtlk_pcieg6_ccr_get_fw_info,
  _mtlk_pcieg6_ccr_get_fw_dump_info,
  _mtlk_pcieg6_ccr_get_hw_dump_info,
  _mtlk_pcieg6_enable_interrupts,
  _mtlk_pcieg6_disable_interrupts,
  _mtlk_pcieg6_clear_interrupts_if_pending,
  _mtlk_pcieg6_disable_interrupts_if_pending,
  _mtlk_pcieg6_initiate_doorbell_interrupt,
  _mtlk_pcieg6_is_interrupt_pending,
  _mtlk_pcieg6_recover_interrupts,
  _mtlk_pcieg6_release_ctl_from_reset,
  _mtlk_pcieg6_put_ctl_to_reset,
  _mtlk_pcieg6_boot_from_bus,
  _mtlk_pcieg6_clear_boot_from_bus,
  _mtlk_pcieg6_switch_to_iram_boot,
  _mtlk_pcieg6_exit_debug_mode,
  _mtlk_pcieg6_put_cpus_to_reset,
  _mtlk_pcieg6_reset_mac,
  _mtlk_pcieg6_power_on_cpus,
  _mtlk_pcieg6_get_afe_value,
  _mtlk_pcieg6_get_progmodel_version,
  _mtlk_pcieg6_bist_efuse,
  _mtlk_pcieg6_init_pll,
  _mtlk_pcieg6_release_cpus_reset,
  _mtlk_pcieg6_check_bist,
  _mtlk_pcieg6_eeprom_init,
  _mtlk_pcieg6_eeprom_override_write,
  _mtlk_pcieg6_eeprom_override_read,
  _mtlk_pcieg6_ccr_get_mips_freq,
  _mtlk_pcieg6_set_msi_intr_mode,
#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
  _mtlk_pcieg6_read_hw_timestamp,
#endif
  _mtlk_pcieg6_ccr_ctrl_ring_init,
  _mtlk_pcieg6_ccr_ring_get_hds_to_process,
  _mtlk_pcieg6_ccr_ring_clear_interrupt,
  _mtlk_pcieg6_ccr_ring_initiate_doorbell_interrupt,
  _mtlk_pcieg6_ccr_print_irq_regs,
  _mtlk_pcieg6_efuse_read_data,
  _mtlk_pcieg6_efuse_write_data,
  _mtlk_pcieg6_efuse_get_size,
  _mtlk_pcieg6_ccr_read_uram_version,
  _mtlk_pcieg6_read_phy_inband_power,
};

/* The following PCIEG6 general APIs should not be duplicated */
#ifdef _PCIEG6_GENERAL_API_

/* called as interface API only in case of Gen6 "main" or "aux" PCIe early detection */
uint32 __MTLK_IFUNC
wave_pcieg6_chip_id_read(void *bar1)
{
  uint32 system_info;

  __ccr_readl(bar1 + SYS_IF_UM_SYSTEM_INFO, system_info);
  system_info &=  PCIEG6_SYS_IF_UM_SYSTEM_INFO_CHIPID_MASK;
  system_info >>= PCIEG6_SYS_IF_UM_SYSTEM_INFO_CHIPID_OFFSET;
  mtlk_osal_emergency_print("PCIE bus ChipVersion:%s ChipID is:0x%04X address is:0x%px",
                            ((system_info == WAVE_WAVE600_A0_CHIP_ID) ? "A0" :
                            ((system_info == WAVE_WAVE600_B0_CHIP_ID) ? "A1" :
                            ((system_info == WAVE_WAVE600_D2_CHIP_ID) ? "D2" : "UNKNOWN"))),
                            system_info, bar1 + SYS_IF_UM_SYSTEM_INFO);

  return system_info;
}

#define PCIEG6_REG_SOC_REGS_RCU_BOOT_MSPS_STATUS_OFFSET  (0x00000008)
#define PCIEG6_REG_SOC_REGS_RCU_BOOT_MSPS_STATUS_MASK    (0x00000300)
#define MSPS_STATUS_MODE_DUAL_PCI                        (0x00)

BOOL __MTLK_IFUNC
wave_pcieg6_is_mode_dual_pci(void *bar1)
{
  void *addr = bar1 + SOC_REGS_RCU_BOOT;
  uint32 rcu_boot;
  uint32 msps_status;

  __ccr_readl(addr, rcu_boot);
  msps_status = rcu_boot & PCIEG6_REG_SOC_REGS_RCU_BOOT_MSPS_STATUS_MASK;
  msps_status >>= PCIEG6_REG_SOC_REGS_RCU_BOOT_MSPS_STATUS_OFFSET;
  mtlk_osal_emergency_print("RCU_BOOT: 0x%08X, mspsStatus: 0x%02X, address: 0x%px",
                            rcu_boot, msps_status, addr);
  return (msps_status == MSPS_STATUS_MODE_DUAL_PCI);
}

#ifdef CPTCFG_IWLWAV_DEBUG
int mtlk_pcieg6_ccr_all_regs_validate (void)
{
  return _mtlk_ccr_pcieg6_all_regs_validate();
}
#endif  /* CPTCFG_IWLWAV_DEBUG */

#endif /* _PCIEG6_GENERAL_API_ */
