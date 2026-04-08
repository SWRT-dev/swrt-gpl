/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include "mtlkinc.h"
#include "hw_mmb.h"
#include "mtlk_ccr_pcieg5.h"
#include "mtlk_ccr_api.h"
#include "mtlk_ccr_common.h"
#include "HwHeaders/Wave500B/HwMemoryMap.h"
#include "HwRegs/Wave500B/HostIfAccRegs.h"


#ifdef CPTCFG_IWLWAV_LINDRV_HW_PCIEG5

#define LOG_LOCAL_GID   GID_CCR_PCIEG5
#define LOG_LOCAL_FID   0

struct pcieg5_pas_map {

  unsigned char shram_access[0x01A000];       /* 0x0 */
  struct {
    unsigned char   filler0[0x850];           /* 0x01A,000 */
    volatile uint32 pcie_cfg_area;            /* 0x01A,850 */
    unsigned char   filler01[0xE57AC];        /* 0x01A,854 */
  } PCIE_CFG;
  unsigned char circular_buffers[0x100000];   /* 0x100,000 */
  struct {
    unsigned char   pac_filler1[0x300];       /* 0x0,200,000 */
    volatile uint32 rx_control;               /* 0x0,200,300 */
  } PAC;
  unsigned char filler2[0x45C];               /* 0x0,200,304 */
  volatile uint32 tsf_timer_low;              /* 0x0,200,760 */
  volatile uint32 tsf_timer_high;             /* 0x0,200,764 */
  unsigned char filler3[0x1F898];             /* 0x0,200,768 */
  struct {
    volatile uint32 enable_upi_interrupt;     /* 0x0,220,000 */
    volatile uint32 upi_interrupt;            /* 0x0,220,004 */
    volatile uint32 upi_interrupt_set;        /* 0x0,220,008 */
    volatile uint32 upi_interrupt_clear;      /* 0x0,220,00C */
    volatile uint32 enable_lpi_interrupt;     /* 0x0,220,010 */
    volatile uint32 lpi_interrupt;            /* 0x0,220,014 */
    volatile uint32 lpi_interrupt_set;        /* 0x0,220,018 */
    volatile uint32 lpi_interrupt_clear;      /* 0x0,220,01C */
    volatile uint32 enable_phi_interrupt;     /* 0x0,220,020 */
    volatile uint32 phi_interrupt;            /* 0x0,220,024 */
    volatile uint32 phi_interrupt_set;        /* 0x0,220,028 */
    volatile uint32 phi_interrupt_clear;      /* 0x0,220,02C */
    volatile uint32 enable_npu2upi_interrupt; /* 0x0,220,030 */
    volatile uint32 npu2upi_interrupt;        /* 0x0,220,034 */
    volatile uint32 npu2upi_interrupt_set;    /* 0x0,220,038 */
    volatile uint32 npu2upi_interrupt_clear;  /* 0x0,220,03C */
    unsigned char   rab_filler2[0x4];         /* 0x0,220,040 */
    volatile uint32 secure_write_register;    /* 0x0,220,044 */
    unsigned char   rab_filler3[0x10];        /* 0x0,220,048 */
    volatile uint32 cpu_control_register;     /* 0x0,220,058 */
    volatile uint32 eeprom_override;          /* 0x0,220,05C */
  } RAB;
  unsigned char filler4[0xFFA0];              /* 0x0,220,060 */
  struct {
    volatile uint32 dynamic;                  /* 0x0,230,000 */
    volatile uint32 stop;                     /* 0x0,230,004 */
    volatile uint32 extend_regclk;            /* 0x0,230,008 */
    volatile uint32 extend_smcclk;            /* 0x0,230,00C */
    volatile uint32 follow_regclk;            /* 0x0,230,010 */
  } CLC;
  unsigned char filler5[0x6FFEC];             /* 0x0,230,014 */
  struct {
    unsigned char   soc_filler1a[0x04];       /* 0x0,2A0,000 */
    volatile uint32 hw_reset;                 /* 0x0,2A0,004 */
    unsigned char   soc_filler1b[0x08];       /* 0x0,2A0,008 */
    volatile uint32 gpio_pullup_en;           /* 0x0,2A0,010 */
    unsigned char   soc_filler2[0x14];        /* 0x0,2A0,014 */
    volatile uint32 eeprom_override;          /* 0x0,2A0,028 */
    unsigned char   soc_filler3[0x20];        /* 0x0,2A0,02C */
    volatile uint32 pcie_msi_intr_enable_mask;/* 0x0,2A0,04C */
    unsigned char   legacy_int;               /* 0x0,2A0,050 */
    unsigned char   soc_filler4[0x1c];        /* 0x0,2A0,054 */

    /* EFUSE registers */
    volatile uint32 efuse_data;               /* 0x0,2A0,070 */
    volatile uint32 efuse_addr;               /* 0x0,2A0,074 */
    volatile uint32 efuse_cntrl;              /* 0x0,2A0,078 */
    volatile uint32 efuse_status;             /* 0x0,2A0,07C */
    volatile uint32 efuse_timer_tpgm;         /* 0x0,2A0,080 */
    volatile uint32 efuse_timer_trd;          /* 0x0,2A0,084 */
    volatile uint32 efuse_control_override;   /* 0x0,2A0,088 */
    volatile uint32 efuse_power_down;         /* 0x0,2A0,08C */

    unsigned char   soc_filler5[0x1C];        /* 0x0,2A0,090 */
    volatile uint32 pcie_msi_intr_mode;       /* 0x0,2A0,0AC */
    unsigned char   soc_filler6[0x7F50];      /* 0x0,2A0,0B0 */
  } SOC_REGS;
  struct {
    volatile uint32 device_vendor_id;         /* 0x0,2A8,000 */
    volatile uint32 command;                  /* 0x0,2A8,004 */
    unsigned char   pcie_filler1[0x48];       /* 0x0,2A8,008 */
    volatile uint32 msi_capability;           /* 0x0,2A8,050 */
    unsigned char   pcie_filler2[0x37FAC];    /* 0x0,2A8,054 */
  } PCIE_CFG_REGS;
  struct {
    volatile uint32 gated_clock_reg;          /* 0x0,2E0,000 */
    volatile uint32 sw_reset;                 /* 0x0,2E0,004 */
    unsigned char   htext_filler1[0x0C];      /* 0x0,2E0,008 */
    volatile uint32 bist_result;              /* 0x0,2E0,014 */
    volatile uint32 door_bell;                /* 0x0,2E0,018 */
    unsigned char   htext_filler2[0x0DC];     /* 0x0,2E0,01C */
    volatile uint32 htext_offset_f8;          /* 0x0,2E0,0F8 */
    unsigned char   htext_filler3[0x064];     /* 0x0,2E0,0FC */
    volatile uint32 start_bist;               /* 0x0,2E0,160 */
    unsigned char   htext_filler4[0x8];       /* 0x0,2E0,164 */
    volatile uint32 endian_swap_control;      /* 0x0,2E0,16C */
    unsigned char   htext_filler5[0x050];     /* 0x0,2E0,170 */
    volatile uint32 ahb_arb_bbcpu_page_reg;   /* 0x0,2E0,1C0 */
    unsigned char   htext_filler6[0x00C];     /* 0x0,2E0,1C4 */
    volatile uint32 host_irq_status;          /* 0x0,2E0,1D0 */
    volatile uint32 host_irq_mask;            /* 0x0,2E0,1D4 */
    unsigned char   htext_filler7[0x08];      /* 0x0,2E0,1D8 */
    volatile uint32 host_irq;                 /* 0x0,2E0,1E0 */
  } HTEXT;
  unsigned char filler6[0x1FE1C];             /* 0x0,2E0,1E4 */
  unsigned char filler7[0x7C000];             /* 0x0,300,000 */  /* UPPER_BBCPU_IRAM */
  struct {
    unsigned char sys_if_filler0[0x40];       /* 0x0,37C,000 */
    volatile uint32 pll_div;                  /* 0x0,37C,040 */
    volatile uint32 pll_en;                   /* 0x0,37C,044 */
    volatile uint32 pll_ctl;                  /* 0x0,37C,048 */
    volatile uint32 sys_clr_divr;             /* 0x0,37C,04c */
    volatile uint32 pll_lock;                 /* 0x0,37C,050 */
    volatile uint32 cpu_enable;               /* 0x0,37C,054 */
    volatile uint32 clock_select;             /* 0x0,37C,058 */
    unsigned char   sys_if_filler1[0x4];      /* 0x0,37C,05c */
    volatile uint32 system_info;              /* 0x0,37C,060 */
    unsigned char   sys_if_filler2[0x04];     /* 0x0,37C,064 */
    volatile uint32 secure_write_register;    /* 0x0,37C,068 */
    unsigned char   sys_if_filler3[0x08];     /* 0x0,37C,06C */
    volatile uint32 mips_ctrl;                /* 0x0,37C,074 */
    unsigned char   sys_if_filler4[0x34];     /* 0x0,37C,078 */
    volatile uint32 ucpu_32k_blocks;          /* 0x0,37C,0AC */
    unsigned char   sys_if_filler5[0x24];     /* 0x0,37C,0B0 */
    volatile uint32 m4k_rams_rm;              /* 0x0,37C,0D4 */
    volatile uint32 iram_rm;                  /* 0x0,37C,0D8 */
    volatile uint32 eram_rm;                  /* 0x0,37C,0DC */
  } UPPER_SYS_IF;
  unsigned char filler8[0x7FF20];             /* 0x0,37C,0E0 */
  struct {
    unsigned char   sys_if_filler1[0x68];     /* 0x0,3FC,000 */
    volatile uint32 secure_write_register;    /* 0x0,3FC,068 */
    unsigned char   sys_if_filler3[0x08];     /* 0x0,3FC,06C */
    volatile uint32 mips_ctrl;                /* 0x0,3FC,074 */
    unsigned char   sys_if_filler2[0x5C];     /* 0x0,3FC,078 */
    volatile uint32 m4k_rams_rm;              /* 0x0,3FC,0D4 */
    volatile uint32 iram_rm;                  /* 0x0,3FC,0D8 */
    volatile uint32 eram_rm;                  /* 0x0,3FC,0DC */
  } LOWER_SYS_IF;
  unsigned char filler9[0x103F20];            /* 0x0,3FC,0E0 */
  struct {
    unsigned char td_filler1[0x8040];         /* 0x0,500,000 */
    volatile uint32 phy_progmodel_ver;        /* 0x0,508,040 */
  } TD;
  unsigned char filler10[0x2F7FBC];           /* 0x0,508,044 */

#define PCIEG5_PAS_MAP_SIZE  0x800000         /* IO mem size */
};

#define MTLK_GEN5_DISABLE_INTERRUPT     (0x0)
#define MTLK_GEN5_ENABLE_INTERRUPT      (MTLK_IRQ_ALL)


#define PCIEG5_MIPS_FREQ_DEFAULT  240

#define PCIEG5_CPU_Control_BIST_Passed    ((1 << 31) | (1 << 15))

#define PCIEG5_UPPER_SYS_IF_SYSTEM_INFO_CHIPID_OFFSET (0x00000000)
#define PCIEG5_UPPER_SYS_IF_SYSTEM_INFO_CHIPID_MASK   (0x0000FFFF)

#define MTLK_G5_CLOCK_ENABLE_TXSENDER                     (1 << 6)
#define MTLK_G5_CLOCK_ENABLE_HOSTIF                       (1 << 14)
#define MTLK_G5_CLOCK_ENABLE_EVERYTHING                   0xFFFFFFFFu

#define MTLK_G5_SWRESET_TXSENDER                          (1 << 6)
#define MTLK_G5_SWRESET_HOSTIF                            (1 << 14)

/* ======= B11 ================================== */

#define MTLK_B11_UCPU_IRAM_START (0x300000)
#define MTLK_B11_UCPU_IRAM_SIZE  (0x58000)

#define MTLK_B11_LCPU_IRAM_START (0x380000)
#define MTLK_B11_LCPU_IRAM_SIZE  (0x28000)

#define MTLK_B11_TXCPU_IRAM_START (0x2F0000)
#define MTLK_B11_TXCPU_IRAM_SIZE  (0x9000)

#define MTLK_B11_IFCPU_IRAM_START (0x2EC000)
#define MTLK_B11_IFCPU_IRAM_SIZE  (0x1800)

#define MTLK_B11_RXCPU_IRAM_START (0x2E8000)
#define MTLK_B11_RXCPU_IRAM_SIZE  (0x2000)

/* ============================================== */

#define REG_PCIE5_PLL_CTL_LCPLL_PUP_D2A_MASK         0x00000080
#define REG_PCIE5_PLL_CTL_LCPLL_RESETN_D2A_MASK      0x00000100
#define REG_PCIE5_PLL_LOCK_PLL_LOCK_MASK             0x00000001

/* PLL divider */
#define REG_PCIE5_PLL_DIV_TEST_CLK_DIV_D2A_SHIFT     0
#define REG_PCIE5_PLL_DIV_WLAN_CLK_DIV_D2A_SHIFT     2
#define REG_PCIE5_PLL_DIV_ADC_CLK_DIV_D2A_SHIFT      4
#define REG_PCIE5_PLL_DIV_DAC_CLK_DIV_D2A_SHIFT      6
#define REG_PCIE5_PLL_DIV_SSI_CLK_DIV_D2A_SHIFT      8

#define REG_PCIE5_PLL_DIV_TEST_CLK_DIV_D2A_DEFAULT   0x00
#define REG_PCIE5_PLL_DIV_WLAN_CLK_DIV_D2A_DEFAULT   0x01
#define REG_PCIE5_PLL_DIV_ADC_CLK_DIV_D2A_DEFAULT    0x00
#define REG_PCIE5_PLL_DIV_DAC_CLK_DIV_D2A_DEFAULT    0x00
#define REG_PCIE5_PLL_DIV_SSI_CLK_DIV_D2A_DEFAULT    0x01

/* PLL enable clocks */
#define REG_PCIE5_PLL_EN_TEST_CLK_PDN_D2A_SHIFT      0
#define REG_PCIE5_PLL_EN_WLAN_CLK_PDN_D2A_SHIFT      1
#define REG_PCIE5_PLL_EN_ADC_CLK_PDN_D2A_SHIFT       2
#define REG_PCIE5_PLL_EN_DAC_CLK_PDN_D2A_SHIFT       3
#define REG_PCIE5_PLL_EN_SSI_CLK_PDN_D2A_SHIFT       4

#define REG_PCIE5_PLL_EN_TEST_CLK_PDN_D2A_DEFAULT    0x01
#define REG_PCIE5_PLL_EN_WLAN_CLK_PDN_D2A_DEFAULT    0x00
#define REG_PCIE5_PLL_EN_ADC_CLK_PDN_D2A_DEFAULT     0x00
#define REG_PCIE5_PLL_EN_DAC_CLK_PDN_D2A_DEFAULT     0x00
#define REG_PCIE5_PLL_EN_SSI_CLK_PDN_D2A_DEFAULT     0x01

/* Reset/Active bit */
#define G5_CPU_RAB_Reset              (0 << 1)
#define G5_CPU_RAB_Active             (1 << 1)

/* Boot mode bits */
#define G5_CPU_RAB_IRAM               (0 << 2)
#define G5_CPU_RAB_SHRAM              (2 << 2)
#define G5_CPU_RAB_DEBUG              (3 << 2)

/* Override bit */
#define PCIEG5_CPU_RAB_Override       (1 << 4)

#define G5_MASK_RX_ENABLED            (1)

#define PCIEG5_B11_UCPU_32K_BLOCKS    (1)

/*--------- Table of FW dump files ---------------*/
/* All IO base addresses are offsets from BAR0
*/

#define PHY_TEST_BUS_IO_BASE       (0x00530000)
#define PHY_TEST_BUS_SIZE          (0x00004000)

#define PHY_TEST_BUS_OFFS_IO_BASE  (0x00510204)
#define PHY_TEST_BUS_OFFS_SIZE     (0x00000004)

wave_fw_dump_info_t  g5_proc_fw_dump_files[] = {
  { "um",               PROC_FW_IO,     UPPER_BBCPU_IRAM_OFFSET_FROM_BAR0,       UPPER_BBCPU_IRAM_SIZE       },
  { "lm",               PROC_FW_IO,     LOWER_BBCPU_IRAM_OFFSET_FROM_BAR0,       LOWER_BBCPU_IRAM_SIZE       },
  { "shram",            PROC_FW_IO,     SHARED_RAM_OFFSET_FROM_BAR0,             SHARED_RAM_SIZE             },
  { "host_if_ram",      PROC_FW_IO,     MAC_GENRISC_HOST_IRAM_OFFSET_FROM_BAR0,  MAC_GENRISC_HOST_IRAM_SIZE  },
  { "tx_sender_ram",    PROC_FW_IO,     MAC_GENRISC_TX_IRAM_OFFSET_FROM_BAR0,    MAC_GENRISC_TX_IRAM_SIZE    },
  { "rx_handler_ram",   PROC_FW_IO,     MAC_GENRISC_RX_IRAM_OFFSET_FROM_BAR0,    MAC_GENRISC_RX_IRAM_SIZE    },
  { "host_if_scpad",    PROC_FW_IO,     MAC_GENRISC_HOST_SPRAM_OFFSET_FROM_BAR0, MAC_GENRISC_HOST_SPRAM_SIZE },
  { "tx_sender_scpad",  PROC_FW_IO,     MAC_GENRISC_TX_SPRAM_OFFSET_FROM_BAR0,   MAC_GENRISC_TX_SPRAM_SIZE   },
  { "rx_handler_scpad", PROC_FW_IO,     MAC_GENRISC_RX_SPRAM_OFFSET_FROM_BAR0,   MAC_GENRISC_RX_SPRAM_SIZE   },
  { "test_bus",         PROC_FW_IO,     PHY_TEST_BUS_IO_BASE,                    PHY_TEST_BUS_SIZE           },
  { "test_bus_offset",  PROC_FW_IO,     PHY_TEST_BUS_OFFS_IO_BASE,               PHY_TEST_BUS_OFFS_SIZE      },
};

/* Below entires should be HEX-dumped as text in one file */
#define MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(name,  base, size) {#name, base, size}

wave_hw_dump_info_t g5_proc_hw_dump_table[] = {
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(PAC_RXF,                    0x00200000, 0x00000200),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(PAC_RXC,                    0x00200300, 0x00000100),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(PAC_DUR,                    0x00200400, 0x00000100),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(PAC_EXTRAPOLATOR,           0x00200600, 0x00000100),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(PAC_TIM,                    0x00200700, 0x00000100),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_BF,                     0x00200800, 0x00000200),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_BEACON,                 0x00200B00, 0x00000100),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(PAC_RXD,                    0x00200C00, 0x00000200),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(TXH_TXC,                    0x00200E00, 0x00000100),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(TXH_NTD,                    0x00200F00, 0x00000100),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(PS_SETTING,                 0x00201000, 0x00000100),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(RXF_MPDU_TYPES_MEM,         0x00201300, 0x00000100),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(TRAFFIC_INDICATOR,          0x00201400, 0x00000400),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(DUR_AUTO_RATE_MEM,          0x00201800, 0x00000200),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(PAC_DEL,                    0x00201C00, 0x00000400),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(WEP,                        0x00210000, 0x00000800),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(RAB,                        0x00220000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(TX_SEQUENCER,               0x0022E000, 0x00000800),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(TX_PD_ACC,                  0x0022E800, 0x00000800),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_COUNTERS,               0x0022F000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(CLC,                        0x00230000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(TX_SELECTOR,                0x00231000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(AGG_BUILDER,                0x00232000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_ADDR2INDEX_REG,         0x00233000, 0x00000200),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(PROT_DB,                    0x00233200, 0x00000200),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(PROT_DB_SHADOW_MEM,         0x00233400, 0x00000400),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_ADDR2INDEX_MEM,         0x00233800, 0x00000800),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(TX_HANDLER,                 0x00234000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(BA_ANALYZER,                0x00235000, 0x00000800),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(RX_PP,                      0x00236000, 0x00005000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(RX_CLASSIFIER,              0x0023B000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(BA_ANALYZER_COUNTER,        0x0023C000, 0x00000E00),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(RX_PP_COUNTERS,             0x0023D000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_HT_EXTENSIONS,          0x002E0000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_BACK,                   0x002E1000, 0x00000800),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_TRAINER,                0x002E1800, 0x00000400),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_GENRISC_HOST_REG,       0x002E1C00, 0x00000400),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_GENRISC_TX_REG,         0x002E2000, 0x00000400),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_GENRISC_RX_REG,         0x002E2400, 0x00000400),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_GENRISC_HOST_SPRAM,     0x002E3000, 0x00000600),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_GENRISC_TX_SPRAM,       0x002E4000, 0x00000A00),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_GENRISC_RX_SPRAM,       0x002E5000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_GENRISC_TX_STD,         0x002E6000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_GENRISC_RX_IRAM,        0x002E8000, 0x00002000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_GENRISC_HOST_IRAM,      0x002EC000, 0x00001800),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(MAC_GENRISC_TX_IRAM,        0x002F0000, 0x00009000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(WLAN_ARM_DMA,               0x002B1000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(DATA_LOGGER,                0x002B0000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(ARM_DMA_WRAPPER,            0x002B2000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(HOST_IF_COUNTERS,           0x002B3000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(HOST_IF_ACC,                0x002B7000, 0x00001000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(DLM,                        0x002B8000, 0x00004000),
  MTLK_DEF_IO_FW_DUMP_TEXT_ENTRY(QUEUE_MANAGER,              0x002BC000, 0x00001000),
};

/*------------------------------------------------*/

#define _mtlk_pcieg5_get_afe_value mtlk_ccr_dummy_ret_void_array
#define _mtlk_pcieg5_bist_efuse mtlk_ccr_dummy_ret_void_param
#define _mtlk_pcieg5_exit_debug_mode mtlk_ccr_dummy_ret_void
#define _mtlk_pcieg5_release_ctl_from_reset mtlk_ccr_dummy_ret_void_param
#define _mtlk_pcieg5_read_uram_version mtlk_ccr_dummy_ret_uzero

/* Put interrupt-related functions into single section */
static void   _mtlk_pcieg5_enable_interrupts(void *ccr_mem, uint32 mask)                    __MTLK_INT_HANDLER_SECTION;
static void   _mtlk_pcieg5_disable_interrupts(void *ccr_mem, uint32 mask)                   __MTLK_INT_HANDLER_SECTION;
static uint32 _mtlk_pcieg5_clear_interrupts_if_pending(void *ccr_mem, uint32 clear_mask)    __MTLK_INT_HANDLER_SECTION;
static BOOL   _mtlk_pcieg5_disable_interrupts_if_pending(void *ccr_mem, uint32 mask)        __MTLK_INT_HANDLER_SECTION;
static void   _mtlk_pcieg5_initiate_doorbell_interrupt(void *ccr_mem)                       __MTLK_INT_HANDLER_SECTION;
static void   _mtlk_pcieg5_ccr_ctrl_ring_init(void *ccr_mem, void *ring_regs, unsigned char *mmb_base) __MTLK_INT_HANDLER_SECTION;
static uint32 _mtlk_pcieg5_ccr_ring_get_hds_to_process(void *ccr_mem, void *ring_regs)      __MTLK_INT_HANDLER_SECTION;
static uint32 _mtlk_pcieg5_is_interrupt_pending(void *ccr_mem)                              __MTLK_INT_HANDLER_SECTION;
static void   _mtlk_pcieg5_recover_interrupts(void *ccr_mem)                                __MTLK_INT_HANDLER_SECTION;


static uint32
_mtlk_pcieg5_is_interrupt_pending(void *ccr_mem)
{
  uint32 status;
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;

   MTLK_ASSERT(NULL != g5_mem);

   __ccr_readl(&g5_mem->pas->HTEXT.host_irq_status, status);
   return status;
}

static void
_mtlk_pcieg5_recover_interrupts(void *ccr_mem)
{
    uint32 irq_mask_dummy;
    uint32 irq_mask_cur;
    _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;

    MTLK_ASSERT(NULL != g5_mem);

    /* Read current status of interrupt mask */
    __ccr_readl(&g5_mem->pas->HTEXT.host_irq_mask, irq_mask_cur);

    /* Disable interrupt */
    __ccr_writel(MTLK_GEN5_DISABLE_INTERRUPT, &g5_mem->pas->HTEXT.host_irq_mask);
    mmiowb();
    /* Read back to ensure transaction is reached end point */
    __ccr_readl(&g5_mem->pas->HTEXT.host_irq_mask, irq_mask_dummy);
    /* Enable Interrupts even they were disabled side effect: one dummy interrupt */
    __ccr_writel(MTLK_IRQ_ALL, &g5_mem->pas->HTEXT.host_irq_mask);
}

static uint32
_mtlk_pcieg5_ccr_read_chip_id(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  uint32 system_info;

  __ccr_readl(&pcieg5_mem->pas->UPPER_SYS_IF.system_info, system_info);

  system_info &= PCIEG5_UPPER_SYS_IF_SYSTEM_INFO_CHIPID_MASK;
  system_info >>= PCIEG5_UPPER_SYS_IF_SYSTEM_INFO_CHIPID_OFFSET;

  mtlk_osal_emergency_print("PCIE bus ChipID is:0x%04X address is:0x%px", system_info, &pcieg5_mem->pas->UPPER_SYS_IF.system_info);

  return system_info;
}

static int
_mtlk_pcieg5_ccr_get_hw_dump_info (void *ccr_mem, wave_hw_dump_info_t **hw_dump_info)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_ASSERT(NULL != hw_dump_info);
  *hw_dump_info = &g5_proc_hw_dump_table[0];
  return ARRAY_SIZE(g5_proc_hw_dump_table);
}

static int
_mtlk_pcieg5_ccr_get_fw_dump_info (void *ccr_mem, wave_fw_dump_info_t **fw_info)
{
  size_t alloc_size;

  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_ASSERT(NULL != fw_info);
  MTLK_ASSERT(NULL == fw_info);

  alloc_size = sizeof(g5_proc_fw_dump_files);
  *fw_info = mtlk_osal_mem_alloc(alloc_size, LQLA_MEM_TAG_FW_RECOVERY);
  if (*fw_info == NULL) {
    ELOG_V("Failed to alloc memory for fw_info");
    return MTLK_ERR_NO_MEM;
  }

  wave_memcpy(*fw_info, alloc_size, &g5_proc_fw_dump_files[0], sizeof(g5_proc_fw_dump_files));

  return ARRAY_SIZE(g5_proc_fw_dump_files);
}

static int
_mtlk_pcieg5_ccr_get_fw_info (void *ccr_mem, uint8 cpu, mtlk_fw_info_t *fw_info)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  MTLK_ASSERT(ARRAY_SIZE(fw_info->mem) >= 3);

  switch (cpu)
  {
    case CHI_CPU_NUM_UM:
      /* initialize memory chunks for upper-mac CPU */
      fw_info->mem[0].start   = ((unsigned char*)pcieg5_mem->pas) + MTLK_B11_UCPU_IRAM_START;
      fw_info->mem[0].length  = MTLK_B11_UCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case CHI_CPU_NUM_LM:
      /* initialize memory chunks for lower-mac CPU */
      fw_info->mem[0].start   = ((unsigned char*)pcieg5_mem->pas) + MTLK_B11_LCPU_IRAM_START;
      fw_info->mem[0].length  = MTLK_B11_LCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case CHI_CPU_NUM_TX:
      /* initialize memory chunks for TxSender CPU */
      fw_info->mem[0].start   = ((unsigned char*)pcieg5_mem->pas) + MTLK_B11_TXCPU_IRAM_START;
      fw_info->mem[0].length  = MTLK_B11_TXCPU_IRAM_SIZE;
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
      fw_info->mem[0].start   = ((unsigned char*)pcieg5_mem->pas) + MTLK_B11_IFCPU_IRAM_START;
      fw_info->mem[0].length  = MTLK_B11_IFCPU_IRAM_SIZE;
      fw_info->mem[0].swap    = FALSE;
      fw_info->mem[1].start   = 0;
      fw_info->mem[1].length  = 0;
      fw_info->mem[1].swap    = FALSE;
      fw_info->mem[2].start   = 0;
      fw_info->mem[2].length  = 0;
      fw_info->mem[2].swap    = FALSE;
      break;
    case CHI_CPU_NUM_RX:
      /* initialize memory chunks for RX handler GenRISC CPU */
      fw_info->mem[0].start   = ((unsigned char*)pcieg5_mem->pas) + MTLK_B11_RXCPU_IRAM_START;
      fw_info->mem[0].length  = MTLK_B11_RXCPU_IRAM_SIZE;
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

#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
static void
_mtlk_pcieg5_read_hw_timestamp(void *ccr_mem, uint32 *low, uint32 *high)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != pcieg5_mem);

  __ccr_readl(&pcieg5_mem->pas->tsf_timer_low, *low);
  __ccr_readl(&pcieg5_mem->pas->tsf_timer_high, *high);
}
#endif /* CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED */


#if (!MTLK_CCR_DEBUG_PRINT_INTR_REGS)
#define _mtlk_pcieg5_ccr_print_irq_regs mtlk_ccr_dummy_ret_void
#else
static void
_mtlk_pcieg5_ccr_print_irq_regs(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  uint32 host_irq_mask, host_irq_status;
  uint32 pcie_msi_intr_enable_mask, legacy_int, pcie_msi_intr_mode, msi_capability, phi_interrupt_clear;

  MTLK_ASSERT(NULL != g5_mem);

  __ccr_readl(&g5_mem->pas->HTEXT.host_irq_mask, host_irq_mask);
  __ccr_readl(&g5_mem->pas->HTEXT.host_irq_status, host_irq_status);
  __ccr_readl(&g5_mem->pas->SOC_REGS.pcie_msi_intr_enable_mask, pcie_msi_intr_enable_mask);
  __ccr_readl(&g5_mem->pas->SOC_REGS.legacy_int, legacy_int);
  __ccr_readl(&g5_mem->pas->SOC_REGS.pcie_msi_intr_mode, pcie_msi_intr_mode);
  __ccr_readl(&g5_mem->pas->PCIE_CFG_REGS.msi_capability, msi_capability);
  __ccr_readl(&g5_mem->pas->RAB.phi_interrupt_clear, phi_interrupt_clear);

  WLOG_P  ("Gen5 pas=%p\n", g5_mem->pas);
  WLOG_DD ("Gen5 IRQ regs: HTEXT.host_irq_mask=%08X, HTEXT.host_irq_status=%08X",
          host_irq_mask, host_irq_status);
  WLOG_DDD("  SOC_REGS.pcie_msi_intr_enable_mask=%08X, SOC_REGS.legacy_int=%08X, SOC_REGS.pcie_msi_intr_mode=%08X",
          pcie_msi_intr_enable_mask, legacy_int, pcie_msi_intr_mode);
  WLOG_DD ("  PCIE_CFG_REGS.msi_capability=%08X, RAB.phi_interrupt_clear=%08X",
          msi_capability, phi_interrupt_clear);

  mtlk_hw_debug_print_all_ring_regs(g5_mem->hw);
}
#endif /* MTLK_CCR_DEBUG_PRINT_INTR_REGS */

static void
_mtlk_pcieg5_enable_interrupts(void *ccr_mem, uint32 mask)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != g5_mem);

  __ccr_writel(mask/*MTLK_GEN5_ENABLE_INTERRUPT*/, &g5_mem->pas->HTEXT.host_irq_mask);
}

static void
_mtlk_pcieg5_disable_interrupts(void *ccr_mem, uint32 mask)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != g5_mem);

  __ccr_writel(MTLK_GEN5_DISABLE_INTERRUPT, &g5_mem->pas->HTEXT.host_irq_mask);

}

/* WARNING                                                      */
/* Currently we do not utilize (and have no plans to utilize)   */
/* interrupt sharing on Gen 5 platform. However, Gen 5          */
/* hardware supports this ability.                              */
/* For now, in all *_if_pending functions we assume there is no */
/* interrupt sharing, so we may not check whether arrived       */
/* interrupt is our. This save us CPU cycles and code lines.    */
/* In case interrupt sharing will be used, additional checks    */
/* for interrupt appurtenance to be added into these functions. */

static uint32
_mtlk_pcieg5_clear_interrupts_if_pending(void *ccr_mem, uint32 clear_mask)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  uint32 status;

  MTLK_ASSERT(NULL != g5_mem);

#define MTLK_GEN5_CLEAR_RAB_INTERRUPT       (0x1)

  __ccr_readl(&g5_mem->pas->HTEXT.host_irq_status, status);

  if (status & MTLK_IRQ_LEGACY & clear_mask) {
      __ccr_writel(MTLK_GEN5_CLEAR_RAB_INTERRUPT,
               &g5_mem->pas->RAB.phi_interrupt_clear);
  }

  return status;
}

static BOOL
_mtlk_pcieg5_disable_interrupts_if_pending(void *ccr_mem, uint32 mask)
{
  _mtlk_pcieg5_disable_interrupts(ccr_mem, mask);
  return TRUE;
}

static void
_mtlk_pcieg5_initiate_doorbell_interrupt(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
#define MTLK_GEN5_GENERATE_DOOR_BELL    (0x1)

  MTLK_ASSERT(NULL != g5_mem);

  __ccr_writel(MTLK_GEN5_GENERATE_DOOR_BELL,
               &g5_mem->pas->RAB.npu2upi_interrupt_set);
}

static void
_mtlk_pcieg5_switch_to_iram_boot(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != pcieg5_mem);

  pcieg5_mem->next_boot_mode = G5_CPU_RAB_IRAM;
}

static void
_mtlk_pcieg5_boot_from_bus(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != pcieg5_mem);

  pcieg5_mem->next_boot_mode = G5_CPU_RAB_SHRAM;
}

static void
_mtlk_pcieg5_clear_boot_from_bus(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != pcieg5_mem);

  pcieg5_mem->next_boot_mode = G5_CPU_RAB_DEBUG;
}

static __INLINE void
__pcieg5_open_secure_write_register_upper(_mtlk_pcieg5_ccr_t *g5_mem)
{
  MTLK_ASSERT(NULL != g5_mem);

  __ccr_writel(0xAAAA,
               &g5_mem->pas->UPPER_SYS_IF.secure_write_register);
  __ccr_writel(0x5555,
               &g5_mem->pas->UPPER_SYS_IF.secure_write_register);

}

static __INLINE void
__pcieg5_open_secure_write_register_lower(_mtlk_pcieg5_ccr_t *g5_mem)
{
  MTLK_ASSERT(NULL != g5_mem);

  __ccr_writel(0xAAAA,
               &g5_mem->pas->LOWER_SYS_IF.secure_write_register);
  __ccr_writel(0x5555,
               &g5_mem->pas->LOWER_SYS_IF.secure_write_register);
}

static __INLINE void
__mtlk_pcieg5_set_ucpu_32k_blocks (_mtlk_pcieg5_ccr_t *g5_mem)
{
  MTLK_ASSERT(NULL != g5_mem);

  __pcieg5_open_secure_write_register_upper(g5_mem);
  __ccr_writel(PCIEG5_B11_UCPU_32K_BLOCKS, &g5_mem->pas->UPPER_SYS_IF.ucpu_32k_blocks);
}

/* #define DEBUG_CCR_CHECK_REGISTERS */
#ifdef  DEBUG_CCR_CHECK_REGISTERS
static void debug_read_registers(_mtlk_pcieg5_ccr_t *g5_mem, char *msg)
{
  uint32 val;

  MTLK_ASSERT(NULL != g5_mem);

  __ccr_readl(&g5_mem->pas->SOC_REGS.hw_reset, val);
  ILOG0_SSPD("DEBUG(%s): %s:address is 0x%p, value=0x%08x", msg, "hw_reset", &g5_mem->pas->SOC_REGS.hw_reset, val);

  __ccr_readl(&g5_mem->pas->SOC_REGS.pcie_msi_intr_enable_mask, val);
  ILOG0_SSPD("DEBUG(%s): %s:address is 0x%p, value=0x%08x", msg, "pcie_msi_intr_enable_mask", &g5_mem->pas->SOC_REGS.pcie_msi_intr_enable_mask, val);

  __ccr_readl(&g5_mem->pas->SOC_REGS.legacy_int, val);
  ILOG0_SSPD("DEBUG(%s): %s:address is 0x%p, value=0x%08x", msg, "legacy_int", &g5_mem->pas->SOC_REGS.legacy_int, val);

  __ccr_readl(&g5_mem->pas->SOC_REGS.pcie_msi_intr_mode, val);
  ILOG0_SSPD("DEBUG(%s): %s:address is 0x%p, value=0x%08x", msg, "pcie_msi_intr_mode", &g5_mem->pas->SOC_REGS.pcie_msi_intr_mode, val);

  __ccr_readl(&g5_mem->pas->PCIE_CFG_REGS.msi_capability, val);
  ILOG0_SSPD("DEBUG(%s): %s:address is 0x%p, value=0x%08x", msg, "msi_capability", &g5_mem->pas->PCIE_CFG_REGS.msi_capability, val);

}
#endif  /* DEBUG_CCR_CHECK_REGISTERS */

static void _mtlk_pcieg5_reset_mac(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;

  MTLK_ASSERT(NULL != g5_mem);

#define G5_WLAN_SW_RESET    0x1u

#ifdef  DEBUG_CCR_CHECK_REGISTERS
  debug_read_registers(g5_mem, "BEFORE_RST");
#endif  /* DEBUG_CCR_CHECK_REGISTERS */

  ILOG1_PD("Reset WLAN SOC: address is 0x%p, value=0x%08x", &g5_mem->pas->SOC_REGS.hw_reset, G5_WLAN_SW_RESET);
  __ccr_resetl(&g5_mem->pas->SOC_REGS.hw_reset, G5_WLAN_SW_RESET);
  mtlk_osal_msleep(10);

#ifdef  DEBUG_CCR_CHECK_REGISTERS
  debug_read_registers(g5_mem, "AFTER_RST");
#endif  /* DEBUG_CCR_CHECK_REGISTERS */
}

static void
_mtlk_pcieg5_put_cpus_to_reset(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;

  MTLK_ASSERT(NULL != g5_mem);

#define G5_MIPS_RESET   0

  /* FIXME: not used on anymore */
  g5_mem->current_ucpu_state =
    g5_mem->current_lcpu_state =
      G5_CPU_RAB_IRAM;

  __pcieg5_open_secure_write_register_lower(g5_mem);
  __ccr_writel(G5_MIPS_RESET, &g5_mem->pas->LOWER_SYS_IF.mips_ctrl);

  /* CPU requires time to go to  reset, so we       */
  /* MUST wait here before writing something else   */
  /* to CPU control register. In other case this    */
  /* may lead to unpredictable results.             */
  mtlk_osal_msleep(20);

  __pcieg5_open_secure_write_register_upper(g5_mem);
  __ccr_writel(G5_MIPS_RESET, &g5_mem->pas->UPPER_SYS_IF.mips_ctrl);
  mtlk_osal_msleep(20);
}

static __INLINE uint32
_mtlk_pcieg5_get_progmodel_version(void *ccr_mem)
{
  uint32 val;
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;

  MTLK_ASSERT(NULL != g5_mem);

  __ccr_readl(&g5_mem->pas->TD.phy_progmodel_ver, val);

  return val;
}

static int
_mtlk_pcieg5_init_pll(void *ccr_mem, uint32 chip_id)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  uint32 reg_val, counter = 100;
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != g5_mem);
  MTLK_STATIC_ASSERT(PCIEG5_PAS_MAP_SIZE == sizeof(*(g5_mem->pas)));

  if (!wave_hw_mmb_card_is_asic(g5_mem->hw)) {
    WLOG_V("Skip PLL init due to non ASIC");
    return MTLK_ERR_OK;
  }

   /* Start pll */
  __ccr_readl(&g5_mem->pas->UPPER_SYS_IF.pll_ctl, reg_val);
  reg_val |= REG_PCIE5_PLL_CTL_LCPLL_PUP_D2A_MASK;
  __pcieg5_open_secure_write_register_upper(g5_mem);
  __ccr_writel(reg_val, &g5_mem->pas->UPPER_SYS_IF.pll_ctl);
  mtlk_osal_msleep(50);

  /* Reset pll */
  __ccr_readl(&g5_mem->pas->UPPER_SYS_IF.pll_ctl, reg_val);
  reg_val |= (REG_PCIE5_PLL_CTL_LCPLL_RESETN_D2A_MASK | REG_PCIE5_PLL_CTL_LCPLL_PUP_D2A_MASK);
  __pcieg5_open_secure_write_register_upper(g5_mem);
  __ccr_writel(reg_val, &g5_mem->pas->UPPER_SYS_IF.pll_ctl);

  /* Wait for PLL lock */
  do
  {
    mtlk_osal_msleep(1);
   __ccr_readl(&g5_mem->pas->UPPER_SYS_IF.pll_lock, reg_val);
   reg_val &= REG_PCIE5_PLL_LOCK_PLL_LOCK_MASK;
  }
  while((counter--) && (!reg_val));

  if(!reg_val)
  {
    res = MTLK_ERR_HW;
    goto finish;
  }

  /* Configure PLL clocks */
  reg_val = ((REG_PCIE5_PLL_DIV_WLAN_CLK_DIV_D2A_DEFAULT << REG_PCIE5_PLL_DIV_WLAN_CLK_DIV_D2A_SHIFT)|
             (REG_PCIE5_PLL_DIV_TEST_CLK_DIV_D2A_DEFAULT << REG_PCIE5_PLL_DIV_TEST_CLK_DIV_D2A_SHIFT)|
             (REG_PCIE5_PLL_DIV_ADC_CLK_DIV_D2A_DEFAULT  << REG_PCIE5_PLL_DIV_ADC_CLK_DIV_D2A_SHIFT)|
             (REG_PCIE5_PLL_DIV_DAC_CLK_DIV_D2A_DEFAULT  << REG_PCIE5_PLL_DIV_DAC_CLK_DIV_D2A_SHIFT)|
             (REG_PCIE5_PLL_DIV_SSI_CLK_DIV_D2A_DEFAULT  << REG_PCIE5_PLL_DIV_SSI_CLK_DIV_D2A_SHIFT));
  __pcieg5_open_secure_write_register_upper(g5_mem);
  __ccr_writel(reg_val, &g5_mem->pas->UPPER_SYS_IF.pll_div);

  /* Enable clocks from PLL */
  reg_val = ((REG_PCIE5_PLL_EN_TEST_CLK_PDN_D2A_DEFAULT << REG_PCIE5_PLL_EN_TEST_CLK_PDN_D2A_SHIFT)|
             (REG_PCIE5_PLL_EN_WLAN_CLK_PDN_D2A_DEFAULT << REG_PCIE5_PLL_EN_WLAN_CLK_PDN_D2A_SHIFT)|
             (REG_PCIE5_PLL_EN_ADC_CLK_PDN_D2A_DEFAULT  << REG_PCIE5_PLL_EN_ADC_CLK_PDN_D2A_SHIFT)|
             (REG_PCIE5_PLL_EN_DAC_CLK_PDN_D2A_DEFAULT  << REG_PCIE5_PLL_EN_DAC_CLK_PDN_D2A_SHIFT)|
             (REG_PCIE5_PLL_EN_SSI_CLK_PDN_D2A_DEFAULT  << REG_PCIE5_PLL_EN_SSI_CLK_PDN_D2A_SHIFT));
  __pcieg5_open_secure_write_register_upper(g5_mem);
  __ccr_writel(reg_val, &g5_mem->pas->UPPER_SYS_IF.pll_en);

  /* Switch to PLL, Select fast clock (sel=2) */
  __pcieg5_open_secure_write_register_upper(g5_mem);
  __ccr_writel(2, &g5_mem->pas->UPPER_SYS_IF.clock_select);

finish:

  return res;
}

static void
_mtlk_pcieg5_put_ctl_to_reset(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != g5_mem);

  /* Disable RX */
  __ccr_resetl(&g5_mem->pas->PAC.rx_control, G5_MASK_RX_ENABLED);
}

static void
_mtlk_pcieg5_power_on_cpus(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;

  MTLK_ASSERT(NULL != g5_mem);

  /* SW reset for TX sender and Host Interface GenRisc */
  __ccr_setl(&g5_mem->pas->HTEXT.sw_reset, MTLK_G5_CLOCK_ENABLE_EVERYTHING);

  /* Enable TX sender and Host Interface GenRisc CPU clocks */
  __ccr_setl(&g5_mem->pas->HTEXT.gated_clock_reg, MTLK_G5_CLOCK_ENABLE_EVERYTHING);

  /* set IRAM memory configuration */
  __mtlk_pcieg5_set_ucpu_32k_blocks(g5_mem);
}

static void
_mtlk_pcieg5_release_cpus_reset(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;

#define G5_RELEASE_FROM_RESET     0x10

  MTLK_ASSERT(NULL != g5_mem);
  MTLK_ASSERT(G5_CPU_RAB_IRAM == g5_mem->next_boot_mode);

  /* FIXME: not used anymore */
  g5_mem->current_ucpu_state =
    g5_mem->current_lcpu_state =
      G5_CPU_RAB_Active | g5_mem->next_boot_mode;

  __pcieg5_open_secure_write_register_lower(g5_mem);
  __ccr_writel(G5_RELEASE_FROM_RESET, &g5_mem->pas->LOWER_SYS_IF.mips_ctrl);

  /* CPU requires time to change its state, so we   */
  /* MUST wait here before writing something else   */
  /* to CPU control register. In other case this    */
  /* may lead to unpredictable results.             */
  mtlk_osal_msleep(10);

  __pcieg5_open_secure_write_register_upper(g5_mem);
  __ccr_writel(G5_RELEASE_FROM_RESET, &g5_mem->pas->UPPER_SYS_IF.mips_ctrl);
  mtlk_osal_msleep(10);
}

static BOOL
_mtlk_pcieg5_check_bist(void *ccr_mem, uint32 *bist_result)
{
  MTLK_ASSERT(NULL != ccr_mem);
  /* Not needed on G5 */
  MTLK_UNREFERENCED_PARAM(ccr_mem);

  *bist_result = 0;
  return TRUE;
}

static int
_mtlk_pcieg5_eeprom_override_write (void *ccr_mem, uint32 data, uint32 mask)
{
  uint32 val;
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;

  MTLK_ASSERT(NULL != pcieg5_mem);

  __ccr_readl(&pcieg5_mem->pas->SOC_REGS.eeprom_override, val);

  val = (val & (~mask))| data;

  __ccr_writel(val, &pcieg5_mem->pas->SOC_REGS.eeprom_override);

  return MTLK_ERR_OK;
}

static uint32
_mtlk_pcieg5_eeprom_override_read (void *ccr_mem)
{
  uint32 val;
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;

  MTLK_ASSERT(NULL != pcieg5_mem);

  __ccr_readl(&pcieg5_mem->pas->SOC_REGS.eeprom_override, val);

  return val;
}

static uint32
_mtlk_pcieg5_ccr_get_mips_freq(void *ccr_mem, uint32 chip_id)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  return PCIEG5_MIPS_FREQ_DEFAULT;
}

static int _mtlk_pcieg5_ccr_init(void *ccr_mem, mtlk_hw_t *hw, void *bar0, void *bar1, void *bar1_phy, void *dev_ctx, void **shram)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != ccr_mem);

  pcieg5_mem->hw = hw;
  pcieg5_mem->pas = bar1;
  pcieg5_mem->pas_physical = bar1_phy;
  pcieg5_mem->dev_ctx = dev_ctx;

  /* This is a state of cpu on power-on */
  pcieg5_mem->current_ucpu_state =
  pcieg5_mem->current_lcpu_state = G5_CPU_RAB_Active | G5_CPU_RAB_DEBUG;

  pcieg5_mem->next_boot_mode = G5_CPU_RAB_DEBUG;

  *shram = bar1 + SHARED_RAM_OFFSET_FROM_BAR0;

  return MTLK_ERR_OK;
}

static void _mtlk_pcieg5_ccr_cleanup(void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  MTLK_ASSERT(NULL != ccr_mem);

  /* cleanup all values at once */
  memset(pcieg5_mem, 0, sizeof(_mtlk_pcieg5_ccr_t));
}

/* MSI Capability and Control common register */
#define MSI_CAPS_CTRL_CAPS_ID_MASK                              (0x000FF) /* Bits 0.. 7 */
#define MSI_CAPS_CTRL_NEXT_PTR_MASK                             (0x0FF00) /* Bits 8..16 */
#define MSI_CAPS_CTRL_MSI_ENABLE_MASK                           (0x10000) /* Bit     16 */

/* SOC_REGS_PCIE_LEGACY_INT_ENA     - single field register */
#define PCIE_LEGACY_INT_ENABLED                                 (0x01)    /* Bit  0    */
#define PCIE_LEGACY_INT_DISABLED                                (0x00)

/* SOC_REGS_PCIE_MSI_ENABLE         - single field register */
#define PCIE_MSI_ENABLED                                        (0xFF)    /* Bits 0..7 */
#define PCIE_MSI_DISABLED                                       (0x00)

/* SOC_REGS_PCIE_MSI_INTR_MODE      - single field register */
#define SOC_REGS_PCIE_MSI_INTR_MODE_MASK                        (0x03)    /* Bits 0..1 */

static void _mtlk_pcieg5_set_msi_intr_mode (void *ccr_mem, uint32 irq_mode)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;

  MTLK_ASSERT(NULL != pcieg5_mem);

  if (MTLK_IRQ_MODE_LEGACY == irq_mode) {
    /* Legacy IRQ mode */
    __ccr_writel(PCIE_LEGACY_INT_ENABLED, &pcieg5_mem->pas->SOC_REGS.legacy_int);
    __ccr_writel(PCIE_MSI_DISABLED, &pcieg5_mem->pas->SOC_REGS.pcie_msi_intr_enable_mask);
    __ccr_resetl(&pcieg5_mem->pas->PCIE_CFG_REGS.msi_capability, MSI_CAPS_CTRL_MSI_ENABLE_MASK);
  } else {
    /* MSI IRQ mode */
    __ccr_writel(PCIE_LEGACY_INT_DISABLED, &pcieg5_mem->pas->SOC_REGS.legacy_int);
    __ccr_writel(PCIE_MSI_ENABLED, &pcieg5_mem->pas->SOC_REGS.pcie_msi_intr_enable_mask);
    __ccr_setl(&pcieg5_mem->pas->PCIE_CFG_REGS.msi_capability, MSI_CAPS_CTRL_MSI_ENABLE_MASK);
    MTLK_ASSERT(irq_mode <= SOC_REGS_PCIE_MSI_INTR_MODE_MASK);
    __ccr_writel(irq_mode, &pcieg5_mem->pas->SOC_REGS.pcie_msi_intr_mode);
  }
}

#define REG_OFFS(x) (x - HOST_IF_ACC_BASE_ADDR + HOST_IF_ACC_OFFSET_FROM_BAR0)

static void
_mtlk_pcieg5_ccr_ctrl_ring_init(void *ccr_mem, void *ring_regs, unsigned char *mmb_base)
{
  mtlk_ring_regs *regs = (mtlk_ring_regs *)ring_regs;
  
  MTLK_ASSERT(NULL != regs);
  
  regs->p_IN_cntr    = (uint32*)(mmb_base + REG_OFFS(REG_HOST_IF_ACC_HOST_MAC_MAILBOX_ACCUM_ADD));
  regs->p_OUT_status = (uint32*)(mmb_base + REG_OFFS(REG_HOST_IF_ACC_MAC_HOST_MAILBOX_ACCUM_COUNT));
  regs->p_OUT_cntr   = (uint32*)(mmb_base + REG_OFFS(REG_HOST_IF_ACC_MAC_HOST_MAILBOX_ACCUM_SUB));
  regs->int_OUT_cntr = mtlk_raw_readl(regs->p_OUT_status);
  
  ILOG0_PPP("[Ring Regs] p_IN_cntr:0x%p, p_OUT_status:0x%p, p_OUT_cntr:0x%p",
  	regs->p_IN_cntr, regs->p_OUT_status, regs->p_OUT_cntr);
  
  MTLK_ASSERT(0 == regs->int_OUT_cntr);
  
  return;
}

static uint32
_mtlk_pcieg5_ccr_ring_get_hds_to_process (void *ccr_mem, void *ring_regs)
{
  mtlk_ring_regs *regs = (mtlk_ring_regs *)ring_regs;

  MTLK_ASSERT(NULL != regs);

  /* p_tx_OUT_status already contains number of HDs */
  return LE32_TO_CPU(mtlk_raw_readl(regs->p_OUT_status));
}

static void
_mtlk_pcieg5_ccr_ring_clear_interrupt (void *ring_regs, uint32 hds_processed)
{
  mtlk_ring_regs *regs = (mtlk_ring_regs *)ring_regs;

  MTLK_ASSERT(NULL != regs);

  regs->int_OUT_cntr += hds_processed;

  /* clear interrupt */
  mtlk_raw_writel(CPU_TO_LE32(hds_processed), regs->p_OUT_cntr);
}

static void
_mtlk_pcieg5_ccr_ring_initiate_doorbell_interrupt (void *ccr_mem, void *ring_regs)
{
  mtlk_ring_regs *regs = (mtlk_ring_regs*)ring_regs;

  MTLK_ASSERT(NULL != regs);

  /* just one HD added, accumulator register */
  mtlk_raw_writel(CPU_TO_LE32(1), regs->p_IN_cntr);
}

static void
_mtlk_pcieg5_eeprom_init (void *ccr_mem)
{
  _mtlk_pcieg5_ccr_t *g5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  uint32 reg_val;

#define EEPROM_PULLUP_GPIO_SCL (1<<4)
#define EEPROM_PULLUP_GPIO_SDA (1<<5)

  MTLK_ASSERT(NULL != g5_mem);

  __ccr_readl(&g5_mem->pas->SOC_REGS.gpio_pullup_en, reg_val);
  reg_val |= EEPROM_PULLUP_GPIO_SCL;
  reg_val |= EEPROM_PULLUP_GPIO_SDA;
  __ccr_writel(reg_val, &g5_mem->pas->SOC_REGS.gpio_pullup_en);
}

#define PCIEG5_MAX_EFUSE_SIZE               (512)
#define PCIEG5_EFUSE_PD_DISABLE             (0)
#define PCIEG5_EFUSE_PD_ENABLE              (1)
#define PCIEG5_EFUSE_CNTRL_READ             (0)
#define PCIEG5_EFUSE_CNTRL_WRITE            (1)
#define PCIEG5_EFUSE_STATUS_BUSY_MASK       (0x01)
#define PCIEG5_EFUSE_STATUS_BUSY_CLEARED    (0 * PCIEG5_EFUSE_STATUS_BUSY_MASK)
#define PCIEG5_EFUSE_STATUS_INIT_DONE_MASK  (0x02)
#define PCIEG5_EFUSE_STATUS_INIT_DONE_VALUE (1 * PCIEG5_EFUSE_STATUS_INIT_DONE_MASK)
#define PCIEG5_EFUSE_STATUS_READ_MAX        (1000)  /* max number of attempts to read */
#define PCIEG5_EFUSE_ADDR_SHIFT             (3)     /* byte number to bit number */
#define PCIEG5_EFUSE_ADDR_BIT_MASK          (0x07)  /* mask for the bit number within a byte */

static uint32
_mtlk_pcieg5_efuse_open(_mtlk_pcieg5_ccr_t *pcieg5_mem, uint32 addr, uint32 size)
{
  int val, n;

  if (PCIEG5_MAX_EFUSE_SIZE <= addr ||
      PCIEG5_MAX_EFUSE_SIZE <  size ||
      PCIEG5_MAX_EFUSE_SIZE <  (addr + size))
  {
    ELOG_DD("eFuse open: wrong start addr and/or size: %d, %d", addr, size);
    return MTLK_ERR_PARAMS;
  }
  ILOG1_P("eFuse_data address: 0x%p", &pcieg5_mem->pas->SOC_REGS.efuse_data);

  /* 1. Disable PD bit in efuse_PD register. */
  __ccr_writel(PCIEG5_EFUSE_PD_DISABLE, &pcieg5_mem->pas->SOC_REGS.efuse_power_down);
  __ccr_writel(1, &pcieg5_mem->pas->SOC_REGS.efuse_control_override);

  /* 2. Check efuse_control_busy (bit 0) bit is cleared and check efuse_init_done (bit 1) bit is set */
  for (n = 0; n < PCIEG5_EFUSE_STATUS_READ_MAX; n++) {
    __ccr_readl(&pcieg5_mem->pas->SOC_REGS.efuse_status, val);
    val &= (PCIEG5_EFUSE_STATUS_BUSY_MASK | PCIEG5_EFUSE_STATUS_INIT_DONE_MASK);
    if ((PCIEG5_EFUSE_STATUS_BUSY_CLEARED | PCIEG5_EFUSE_STATUS_INIT_DONE_VALUE) == val) {
      return MTLK_ERR_OK;
    }
  }
  ELOG_D("eFuse open: status not ready: 0x%x", val);
  return MTLK_ERR_NOT_READY;
}

static void
_mtlk_pcieg5_efuse_close(_mtlk_pcieg5_ccr_t *pcieg5_mem)
{
  /* 7. Enable PD bit in efuse_PD register. */
  __ccr_writel(0, &pcieg5_mem->pas->SOC_REGS.efuse_control_override);
  __ccr_writel(PCIEG5_EFUSE_PD_ENABLE, &pcieg5_mem->pas->SOC_REGS.efuse_power_down);
}

static __INLINE BOOL
__mtlk_pcieg5_efuse_status_busy_cleared(_mtlk_pcieg5_ccr_t *pcieg5_mem)
{
  int n, val;

  /* 5. Poll the efuse_status register until the efuse_control_busy bit 0 is cleared */
  for (n = 0; n < PCIEG5_EFUSE_STATUS_READ_MAX; n++) {
    __ccr_readl(&pcieg5_mem->pas->SOC_REGS.efuse_status, val);
    if (PCIEG5_EFUSE_STATUS_BUSY_CLEARED == (PCIEG5_EFUSE_STATUS_BUSY_MASK & val)) {
      return TRUE;
    }
  }
  ELOG_V("eFuse busy timeout");
  return FALSE;
}

/* Read data from eFuse */
/* Return value - number of bytes was read */
static uint32
_mtlk_pcieg5_efuse_read_data(void *ccr_mem, void *buf, uint32 addr, uint32 size)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  uint32    i, val, bytes_read = 0;
  uint8    *bytes = buf;

  MTLK_ASSERT(NULL != pcieg5_mem);
  MTLK_ASSERT(NULL != buf);

  if (MTLK_ERR_OK != _mtlk_pcieg5_efuse_open(pcieg5_mem, addr, size))
    goto FINISH;

  for (i = 0; i < size; i++, addr++) {
    /* 3. Write the Byte Address (Bit Number) */
    __ccr_writel((addr << PCIEG5_EFUSE_ADDR_SHIFT), &pcieg5_mem->pas->SOC_REGS.efuse_addr); /* bit number */

    /* 4. Write "Read Request" to the efuse_control register. */
    __ccr_writel(PCIEG5_EFUSE_CNTRL_READ, &pcieg5_mem->pas->SOC_REGS.efuse_cntrl);

    if (FALSE == __mtlk_pcieg5_efuse_status_busy_cleared(pcieg5_mem))
      goto FINISH;

    /* 6. Read the value from the efuse_data register. */
    __ccr_readl(&pcieg5_mem->pas->SOC_REGS.efuse_data, val);
    bytes[i] = (uint8)val; /* store one byte */
  }

  bytes_read = i; /* all data complete */

FINISH:
  _mtlk_pcieg5_efuse_close(pcieg5_mem);

  return bytes_read;
}

static void
_mtlk_pcieg5_efuse_pre_write_check(void *ccr_mem, uint8 *check_buf, uint32 addr, uint32 size)
{
#define EFUSE_CLEAN_PATTERN 0x00
  int i;

  if (!_mtlk_pcieg5_efuse_read_data(ccr_mem, check_buf, addr, size)) {
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
_mtlk_pcieg5_efuse_post_write_check(void *ccr_mem, uint8 *check_buf, uint8 *write_buf, uint32 addr, uint32 size)
{
  int i;
  BOOL write_err = FALSE;

  if (!_mtlk_pcieg5_efuse_read_data(ccr_mem, check_buf, addr, size)) {
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
/* Return value - number of bytes was written */
static uint32
_mtlk_pcieg5_efuse_write_data(void *ccr_mem, void *buf, uint32 addr, uint32 size)
{
  _mtlk_pcieg5_ccr_t *pcieg5_mem = (_mtlk_pcieg5_ccr_t *)ccr_mem;
  uint32 i, n, bytes_written = 0, offset = addr;
  uint8 *bytes = buf;
  uint8 *check_buf = NULL;

  MTLK_ASSERT(NULL != pcieg5_mem);
  MTLK_ASSERT(NULL != buf);

  /* check if eFuse target area is clean */
  check_buf = mtlk_osal_mem_alloc(PCIEG5_MAX_EFUSE_SIZE, MTLK_MEM_TAG_EEPROM);
  if (NULL == check_buf) {
    ELOG_V("Failed to allocate validation buffer");
    goto FAIL;
  }
  _mtlk_pcieg5_efuse_pre_write_check(ccr_mem, check_buf, offset, size);

  if (MTLK_ERR_OK != _mtlk_pcieg5_efuse_open(pcieg5_mem, addr, size))
    goto FINISH;

  mtlk_dump(1, bytes, size, "eFuse data to write:");

  for (i = 0; i < size; i++, addr++) {
    /* Write 1 byte bitwise, BIT0 is first */
    for (n = 0; n < 8; n++) {
      /* only 1s are burned */
      if (bytes[i] & (1 << n)) {
        /* compose bit adressing */
        uint32 bit_addr = (addr << PCIEG5_EFUSE_ADDR_SHIFT) | (n & PCIEG5_EFUSE_ADDR_BIT_MASK);
        __ccr_writel(bit_addr, &pcieg5_mem->pas->SOC_REGS.efuse_addr);
        /* write "Write Request" to the efuse_control register. */
        __ccr_writel(PCIEG5_EFUSE_CNTRL_WRITE, &pcieg5_mem->pas->SOC_REGS.efuse_cntrl);
        /* wait for writing is finished */
        if (FALSE == __mtlk_pcieg5_efuse_status_busy_cleared(pcieg5_mem)) {
          ELOG_DDD("eFuse write error! addr: 0x%x, byte: %d, bit: %d", addr, i, n);
          goto FINISH;
        }
      }
    }
  }
  bytes_written = i;

FINISH:
  _mtlk_pcieg5_efuse_close(pcieg5_mem);

  /* verify write */
  if (bytes_written) {
    _mtlk_pcieg5_efuse_post_write_check(ccr_mem, check_buf, buf, offset, size);
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
_mtlk_pcieg5_efuse_get_size(void *ccr_mem)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  return PCIEG5_MAX_EFUSE_SIZE;
}

static int
_mtlk_pcieg5_read_phy_inband_power(void *ccr_mem, unsigned radio_id, uint32 ant_mask,
                                   int32 *noise_estim, int32 *system_gain)
{
  MTLK_UNREFERENCED_PARAM(ccr_mem);
  MTLK_UNREFERENCED_PARAM(radio_id);
  MTLK_UNREFERENCED_PARAM(ant_mask);
  MTLK_UNREFERENCED_PARAM(noise_estim);
  MTLK_UNREFERENCED_PARAM(system_gain);
  return MTLK_ERR_NOT_SUPPORTED;
}

static const mtlk_ccr_api_t mtlk_ccr_api = {
  _mtlk_pcieg5_ccr_init,
  _mtlk_pcieg5_ccr_cleanup,
  _mtlk_pcieg5_ccr_read_chip_id,
  _mtlk_pcieg5_ccr_get_fw_info,
  _mtlk_pcieg5_ccr_get_fw_dump_info,
  _mtlk_pcieg5_ccr_get_hw_dump_info,
  _mtlk_pcieg5_enable_interrupts,
  _mtlk_pcieg5_disable_interrupts,
  _mtlk_pcieg5_clear_interrupts_if_pending,
  _mtlk_pcieg5_disable_interrupts_if_pending,
  _mtlk_pcieg5_initiate_doorbell_interrupt,
  _mtlk_pcieg5_is_interrupt_pending,
  _mtlk_pcieg5_recover_interrupts,
  _mtlk_pcieg5_release_ctl_from_reset,
  _mtlk_pcieg5_put_ctl_to_reset,
  _mtlk_pcieg5_boot_from_bus,
  _mtlk_pcieg5_clear_boot_from_bus,
  _mtlk_pcieg5_switch_to_iram_boot,
  _mtlk_pcieg5_exit_debug_mode,
  _mtlk_pcieg5_put_cpus_to_reset,
  _mtlk_pcieg5_reset_mac,
  _mtlk_pcieg5_power_on_cpus,
  _mtlk_pcieg5_get_afe_value,
  _mtlk_pcieg5_get_progmodel_version,
  _mtlk_pcieg5_bist_efuse,
  _mtlk_pcieg5_init_pll,
  _mtlk_pcieg5_release_cpus_reset,
  _mtlk_pcieg5_check_bist,
  _mtlk_pcieg5_eeprom_init,
  _mtlk_pcieg5_eeprom_override_write,
  _mtlk_pcieg5_eeprom_override_read,
  _mtlk_pcieg5_ccr_get_mips_freq,
  _mtlk_pcieg5_set_msi_intr_mode,
#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
  _mtlk_pcieg5_read_hw_timestamp,
#endif
  _mtlk_pcieg5_ccr_ctrl_ring_init,
  _mtlk_pcieg5_ccr_ring_get_hds_to_process,
  _mtlk_pcieg5_ccr_ring_clear_interrupt,
  _mtlk_pcieg5_ccr_ring_initiate_doorbell_interrupt,
  _mtlk_pcieg5_ccr_print_irq_regs,
  _mtlk_pcieg5_efuse_read_data,
  _mtlk_pcieg5_efuse_write_data,
  _mtlk_pcieg5_efuse_get_size,
  _mtlk_pcieg5_read_uram_version,
  _mtlk_pcieg5_read_phy_inband_power,
};

const mtlk_ccr_api_t *__MTLK_IFUNC mtlk_ccr_pcieg5_api_get(void)
{
  return &mtlk_ccr_api;
}

#endif /* CPTCFG_IWLWAV_LINDRV_HW_PCIEG5 */
