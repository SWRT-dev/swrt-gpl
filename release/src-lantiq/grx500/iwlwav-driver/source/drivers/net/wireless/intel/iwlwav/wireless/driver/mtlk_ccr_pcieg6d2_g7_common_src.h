/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#define LOG_LOCAL_GID   GID_CCR_PCIE
#define LOG_LOCAL_FID   6

/* Secure Boot: FW binary images loading address and sizes */
#define WAVE_FW_FC_IRAM_START       (0x00400000)
#define WAVE_FW_FC_IRAM_SIZE        (0x000001E8)
#define WAVE_FW_DC_IRAM_SIZE        (0x000001E8)
#define WAVE_FW_FW_HDR_IRAM_SIZE    (0x00000188)
#define WAVE_FW_PSD_HDR_IRAM_SIZE   (0x00000188)
#define WAVE_FW_CAL_HDR_IRAM_SIZE   (0x00000188)
#define WAVE_FW_TXCPU_IRAM_SIZE     (0x00010000)

/* Table with sizes of FW binary images.
   Only exactly defined sizes are filled in.
   Unfilled items suppose actual size of FW.
 */
static const uint32 _wave_fw_size[WAVE_FW_NUM_CNT] =
{
  [WAVE_FW_NUM_UM]            = WAVE_PCIE_URAM_IRAM_SIZE,     /* Unified RAM */
  [WAVE_FW_NUM_SECB_FC]       = WAVE_FW_FC_IRAM_SIZE,         /* FW certificate */
  [WAVE_FW_NUM_SECB_DC]       = WAVE_FW_DC_IRAM_SIZE,         /* DUT certificate */
  [WAVE_FW_NUM_SECB_FW_HDR]   = WAVE_FW_FW_HDR_IRAM_SIZE,     /* Signed FW header */
  [WAVE_FW_NUM_SECB_PSD_HDR]  = WAVE_FW_PSD_HDR_IRAM_SIZE,    /* Signed PSD header */
  [WAVE_FW_NUM_SECB_CAL_HDR]  = WAVE_FW_CAL_HDR_IRAM_SIZE,    /* Signed Cal file header */
  [WAVE_FW_NUM_TX]            = WAVE_PCIE_B0_TXCPU_IRAM_SIZE, /* Tx Sender CPUs */
  [WAVE_FW_NUM_RX]            = WAVE_PCIE_B0_RXCPU_IRAM_SIZE, /* Rx Handler CPUs */
  [WAVE_FW_NUM_IF]            = WAVE_PCIE_IFCPU_IRAM_SIZE,    /* Host Interface CPUs */
  [WAVE_FW_NUM_SECB_IF_1]     = WAVE_PCIE_IFCPU_IRAM_SIZE,
  [WAVE_FW_NUM_SECB_IF_2]     = WAVE_PCIE_IFCPU_IRAM_SIZE,
  [WAVE_FW_NUM_SECB_IF_3]     = WAVE_PCIE_IFCPU_IRAM_SIZE,
  [WAVE_FW_NUM_SECB_IF_4]     = WAVE_PCIE_IFCPU_IRAM_SIZE,
};

static int
_mtlk_pcie_ccr_get_sec_boot_fw_info (void *ccr_mem, wave_fw_num_e fw_num, mtlk_fw_info_t *fw_info)
{
  mtlk_pcie_ccr_t *pcie_mem = (mtlk_pcie_ccr_t *)ccr_mem;
  MTLK_ASSERT(ARRAY_SIZE(fw_info->mem) >= 3);

  /* Default memory chunks values for any FW bin */
  fw_info->mem[0].start   = 0;
  fw_info->mem[0].length  = 0;
  fw_info->mem[0].swap    = FALSE;
  fw_info->mem[1].start   = 0;
  fw_info->mem[1].length  = 0;
  fw_info->mem[1].swap    = FALSE;
  fw_info->mem[2].start   = 0;
  fw_info->mem[2].length  = 0;
  fw_info->mem[2].swap    = FALSE;

  /* FW specific */
  switch (fw_num)
  {
    /* Fixed loading address */
    case WAVE_FW_NUM_UM: /* URAM */
      fw_info->mem[0].start   = ((unsigned char*)pcie_mem->pas) + WAVE_PCIE_URAM_IRAM_START;
      break;
    case WAVE_FW_NUM_SECB_FC: /* FW certificate */
      fw_info->mem[0].start   = ((unsigned char*)pcie_mem->pas) + WAVE_FW_FC_IRAM_START;
      break;

    /* All below have variable loading address */
    case WAVE_FW_NUM_SECB_DC:       /* DUT certificate */
    case WAVE_FW_NUM_SECB_FW_HDR:   /* FW header */
    case WAVE_FW_NUM_SECB_PSD_HDR:  /* PSD header */
    case WAVE_FW_NUM_SECB_CAL_HDR:  /* Cal header */
    case WAVE_FW_NUM_TX:            /* Tx Sender CPUs */
    case WAVE_FW_NUM_RX:            /* Rx Handler CPUs */
    case WAVE_FW_NUM_IF:            /* Host Interface CPUs */
    case WAVE_FW_NUM_SECB_IF_1:
    case WAVE_FW_NUM_SECB_IF_2:
    case WAVE_FW_NUM_SECB_IF_3:
    case WAVE_FW_NUM_SECB_IF_4:
    case WAVE_FW_NUM_SECB_PM_CMN:    /* ProgModel Common */
    case WAVE_FW_NUM_SECB_PM_ANT_RX: /* ProgModel Rx Ant */
    case WAVE_FW_NUM_SECB_PM_ANT_TX: /* ProgModel Tx Ant */
    case WAVE_FW_NUM_SECB_PSD_FILE:  /* PSD file */
    case WAVE_FW_NUM_SECB_CAL_FILE:  /* Cal file */
      break;
    /* Not supported */
    default:
      return MTLK_ERR_NOT_SUPPORTED;
  }

  fw_info->mem[0].length = _wave_fw_size[fw_num];

  return MTLK_ERR_OK;
}

#if (MTLK_CCR_DEBUG_PRINT_INTR_REGS)
static void
_mtlk_pcie_ccr_print_irq_regs(void *ccr_mem)
{
  mtlk_pcie_ccr_t *pci_mem = (mtlk_pcie_ccr_t *)ccr_mem;
  uint32 host_irq_mask, host_irq_status;
  uint32 phi_interrupt_clear;

  MTLK_ASSERT(NULL != pci_mem);

  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_HOST_IRQ_MASK, host_irq_mask);
  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_HOST_IRQ_STATUS, host_irq_status);
  __ccr_mem_readl(ccr_mem, HOST_IF_IRQS_PHI_INTERRUPT_CLEAR, phi_interrupt_clear);

  ILOG0_P  ("PCIE pas=%p\n", pci_mem->pas);
  ILOG0_DDD("PCIE IRQ regs: host_irq_mask=%08X, host_irq_status=%08X, phi_interrupt_clear=%08X",
            host_irq_mask, host_irq_status, phi_interrupt_clear);

  mtlk_hw_debug_print_all_ring_regs(pci_mem->hw);
}
#endif /* MTLK_CCR_DEBUG_PRINT_INTR_REGS */

static void
_mtlk_pcie_secure_release_cpus_reset (void *ccr_mem)
{
  MTLK_ASSERT(NULL != ccr_mem);

  ILOG0_V("Release UMAC");
  __ccr_mem_writel(ccr_mem, HOST_IF_UCPU_STAT_HOST_START_UCPU, HOST_IF_UCPU_STAT);

  mtlk_osal_msleep(10);
}

static uint32
_wave_pcie_read_hw_type (void *ccr_mem)
{
  uint32 system_info;

  MTLK_ASSERT(NULL != ccr_mem);
  __ccr_mem_readl(ccr_mem, HOST_IF_WLAN_EFUSE_INDICATION, system_info);
  return MTLK_BFIELD_GET(system_info, HOST_IF_WLAN_EFUSE_INDICATION_HW_TYPE);
}

static uint32
_wave_pcie_read_hw_rev (void *ccr_mem)
{
  uint32 system_info;

  MTLK_ASSERT(NULL != ccr_mem);
  __ccr_mem_readl(ccr_mem, HOST_IF_WLAN_EFUSE_INDICATION, system_info);
  return MTLK_BFIELD_GET(system_info, HOST_IF_WLAN_EFUSE_INDICATION_HW_REV);
}

/* Unsupported APIs */
#define _mtlk_pcie_init_pll           mtlk_ccr_dummy_ret_ok_param
#define _mtlk_pcie_power_on_cpus      mtlk_ccr_dummy_ret_void
#define _mtlk_pcie_put_cpus_to_reset  mtlk_ccr_dummy_ret_void
#define _mtlk_pcie_put_ctl_to_reset   mtlk_ccr_dummy_ret_void
#define _mtlk_pcie_reset_mac          mtlk_ccr_dummy_ret_void
#define _mtlk_pcie_set_msi_intr_mode  mtlk_ccr_dummy_ret_void_param

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID
