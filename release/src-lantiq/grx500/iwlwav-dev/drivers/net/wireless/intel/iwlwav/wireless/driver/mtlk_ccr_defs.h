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

#if !defined (SAFE_PLACE_TO_INCLUDE_MTLK_CCR_DEFS)
#error "You shouldn't include this file directly!"
#endif /* SAFE_PLACE_TO_INCLUDE_MTLK_CCR_DEFS */

#undef SAFE_PLACE_TO_INCLUDE_MTLK_CCR_DEFS

#include "mtlk_card_selector.h"
#include "mtlk_ccr_api.h"

#define LOG_LOCAL_GID   GID_MTLK_CCR_DEFS
#define LOG_LOCAL_FID   0

/* Clear interrupts (unconditionally) by handling received msg */
 #define MTLK_CCR_CLEAR_INT_UNCONDIT
/* #undef MTLK_CCR_CLEAR_INT_UNCONDIT */

/* Print interrupt vars/regs in case of message timeout */
#define MTLK_CCR_DEBUG_PRINT_INTR_REGS  TRUE

/* First FPGA version with unified RAM 1.17.6.0 */
#define MTLK_GEN6_VERSION_UNIFIED_RAM  0x00005160

#define MTLK_IRQ_NONE       0
#define MTLK_IRQ_CONTROL    (1 << 5) /* bit 5 */
#define MTLK_IRQ_MAILBOX    (1 << 4) /* bit 4 (was 6 ?) */
#define MTLK_IRQ_BSS_IND    (1 << 3) /* bit 3 */
#define MTLK_IRQ_BSS_CFM    (1 << 2) /* bit 2 */
#define MTLK_IRQ_RX         (1 << 1) /* bit 1 */
#define MTLK_IRQ_TXOUT      (1 << 0) /* bit 0 */

#define MTLK_IRQ_LEGACY     MTLK_IRQ_CONTROL

#define MTLK_IRQ_ALL        (MTLK_IRQ_LEGACY | MTLK_IRQ_MAILBOX | MTLK_IRQ_TXOUT | MTLK_IRQ_RX | MTLK_IRQ_BSS_IND | MTLK_IRQ_BSS_CFM)
#define MTLK_IRQ_ALL_DISABLE (0)

#ifdef CPTCFG_IWLWAV_CCR_DEBUG
/* Helper BUS read/write macros */
#define __ccr_writel(val, addr) for(;;) { \
    mtlk_osal_emergency_print("write shram: 0x%08X  to  addr 0x%px", (val), (addr)); \
    mtlk_writel((val), (addr)); \
    break; \
  }
#define __ccr_readl(addr, val) for(;;) { \
    mtlk_osal_emergency_print("reading shram   data    from addr 0x%px", (addr)); \
    (val) = mtlk_readl((addr)); \
    mtlk_osal_emergency_print("read  shram: 0x%08X from addr 0x%px", (val), (addr)); \
    break; \
  }
#else
#define __ccr_writel(val, addr) for(;;) { \
    mtlk_writel((val), (addr)); \
    break; \
  }
#define __ccr_readl(addr, val) for(;;) { \
    (val) = mtlk_readl((addr)); \
    break; \
  }
#endif

#define __ccr_setl(addr, flag) for(;;) { \
    uint32 val; \
    __ccr_readl((addr), val); \
    __ccr_writel(val | (flag), (addr)); \
    break; \
  }

#define __ccr_resetl(addr, flag) for(;;) { \
    uint32 val; \
    __ccr_readl((addr), val); \
    __ccr_writel((val) & ~(flag), (addr)); \
    break; \
  }

#define __ccr_issetl(addr, flag, res) for(;;) { \
    uint32 val; \
    __ccr_readl((addr), val); \
    (res) = ( (val & (flag)) == (flag) ); \
    break; \
  }

static const __INLINE mtlk_ccr_api_t * _mtlk_ccr_api_init(mtlk_ccr_t *ccr, void *bar1)
{
  CARD_SELECTOR_START(ccr->hw_type)
    IF_CARD_PCIEG6(
      uint32 chip_id;
      if (MTLK_CARD_PCIEG6_A0_B0 == ccr->hw_type)
        chip_id = wave_pcieg6a0b0_chip_id_read(bar1);
      else
        chip_id = wave_pcieg6d2_chip_id_read(bar1);
      if (_chipid_is_gen6_b0(chip_id))
        return mtlk_ccr_pcieg6b0_api_get();
      else if (_chipid_is_gen6_d2(chip_id))
        return mtlk_ccr_pcieg6d2_api_get();
      else return mtlk_ccr_pcieg6a0_api_get();
    );
    IF_CARD_PCIEG7( return mtlk_ccr_pcieg7_api_get() );
  CARD_SELECTOR_END();

  return NULL;
}

static __INLINE int
_mtlk_sub_ccr_init(mtlk_ccr_t *ccr, mtlk_hw_t *hw,
                   void *bar0, void *bar1, void *bar1_phy, void *dev_ctx, void **shram)
{
  int res = ccr->card_api->init(&ccr->mem, hw, bar0, bar1, bar1_phy, dev_ctx, shram);
  return res;
}

static __INLINE void
_mtlk_sub_ccr_cleanup(mtlk_ccr_t *ccr)
{
  ccr->card_api->cleanup(&ccr->mem);
}

#ifdef CPTCFG_IWLWAV_USE_INTERRUPT_POLLING

#define _MTLK_CCR_INT_POLL_PERIOD_MS (100)

static uint32 __MTLK_IFUNC
_mtlk_ccr_poll_interrupt (mtlk_osal_timer_t *timer, 
                          mtlk_handle_t      clb_usr_data)
{
  mtlk_ccr_t *ccr = (mtlk_ccr_t *)clb_usr_data;

  MTLK_UNREFERENCED_PARAM(timer);

  mtlk_hw_mmb_interrupt_handler_legacy(ccr->mem.common.hw);
  return _MTLK_CCR_INT_POLL_PERIOD_MS;
}

#endif

MTLK_INIT_STEPS_LIST_BEGIN(ccr)
#ifdef CPTCFG_IWLWAV_USE_INTERRUPT_POLLING
  MTLK_INIT_STEPS_LIST_ENTRY(ccr, INT_POLL_TIMER)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(ccr, EEPROM_MUTEX)
  MTLK_INIT_STEPS_LIST_ENTRY(ccr, CCR_API)
  MTLK_INIT_STEPS_LIST_ENTRY(ccr, SUB_CCR)
  MTLK_INIT_STEPS_LIST_ENTRY(ccr, CHIP_ID)
MTLK_INIT_INNER_STEPS_BEGIN(ccr)
MTLK_INIT_STEPS_LIST_END(ccr);

static __INLINE void
mtlk_ccr_cleanup(mtlk_ccr_t *ccr)
{
  MTLK_CLEANUP_BEGIN(ccr, MTLK_OBJ_PTR(ccr))
    MTLK_CLEANUP_STEP(ccr, CHIP_ID, MTLK_OBJ_PTR(ccr),
                      MTLK_NOACTION, ())
    MTLK_CLEANUP_STEP(ccr, SUB_CCR, MTLK_OBJ_PTR(ccr),
                      _mtlk_sub_ccr_cleanup, (ccr))
    MTLK_CLEANUP_STEP(ccr, CCR_API, MTLK_OBJ_PTR(ccr),
                      MTLK_NOACTION, ())
    MTLK_CLEANUP_STEP(ccr, EEPROM_MUTEX, MTLK_OBJ_PTR(ccr),
                      mtlk_osal_mutex_cleanup, (&ccr->eeprom_mutex));
#ifdef CPTCFG_IWLWAV_USE_INTERRUPT_POLLING
    MTLK_CLEANUP_STEP(ccr, INT_POLL_TIMER, MTLK_OBJ_PTR(ccr),
                      mtlk_osal_timer_cleanup, (&ccr->poll_interrupts))
#endif
  MTLK_CLEANUP_END(ccr, MTLK_OBJ_PTR(ccr));
}

static __INLINE uint32
_mtlk_ccr_read_chip_id(mtlk_ccr_t *ccr)
{
  return ccr->card_api->read_chip_id(&ccr->mem);
}

static __INLINE uint32
_mtlk_ccr_read_uram_version(mtlk_ccr_t *ccr)
{
  return ccr->card_api->read_uram_version(&ccr->mem);
}

static __INLINE int
mtlk_ccr_init(mtlk_ccr_t *ccr, mtlk_card_type_t hw_type, mtlk_hw_t *hw,
              void *bar0, void *bar1, void *bar1_phy, void *dev_ctx, void **shram)
{
  MTLK_INIT_TRY(ccr, MTLK_OBJ_PTR(ccr))
    ccr->hw_type = hw_type;

#ifdef CPTCFG_IWLWAV_USE_INTERRUPT_POLLING
    MTLK_INIT_STEP(ccr, INT_POLL_TIMER, MTLK_OBJ_PTR(ccr),
                   mtlk_osal_timer_init, 
                   (&ccr->poll_interrupts, _mtlk_ccr_poll_interrupt, HANDLE_T(ccr)))
#endif
    MTLK_INIT_STEP(ccr, EEPROM_MUTEX, MTLK_OBJ_PTR(ccr),
                   mtlk_osal_mutex_init, (&ccr->eeprom_mutex));

    MTLK_INIT_STEP_EX(ccr, CCR_API, MTLK_OBJ_PTR(ccr),
                   _mtlk_ccr_api_init, (ccr, bar1),
                   ccr->card_api, NULL != ccr->card_api,
                   MTLK_ERR_NOT_SUPPORTED);

    MTLK_INIT_STEP(ccr, SUB_CCR, MTLK_OBJ_PTR(ccr),
                   _mtlk_sub_ccr_init, (ccr, hw, bar0, bar1, bar1_phy, dev_ctx, shram))

    MTLK_INIT_STEP_EX(ccr, CHIP_ID, MTLK_OBJ_PTR(ccr),
                      mtlk_chip_info_get, (_mtlk_ccr_read_chip_id(ccr)),
                      ccr->chip_info, NULL != ccr->chip_info,
                      MTLK_ERR_NOT_SUPPORTED);
  MTLK_INIT_FINALLY(ccr, MTLK_OBJ_PTR(ccr))
  MTLK_INIT_RETURN(ccr, MTLK_OBJ_PTR(ccr), mtlk_ccr_cleanup, (ccr));
}

static __INLINE struct device *
mtlk_ccr_get_device(mtlk_ccr_t *ccr)
{
  MTLK_ASSERT(NULL != ccr);

  return &((struct pci_dev*)ccr->mem.dev_ctx)->dev;
  /*return ccr->mem.dev_ctx;*/
}

static __INLINE int
mtlk_ccr_get_fw_info(mtlk_ccr_t *ccr, wave_fw_num_e fw_num, mtlk_fw_info_t *fw_info)
{
  MTLK_ASSERT(NULL != fw_info);
  memset(fw_info, 0, sizeof (*fw_info));

  /* prepare card dependent information */
  return ccr->card_api->get_fw_info(&ccr->mem, fw_num, fw_info);
}

static __INLINE int
mtlk_ccr_get_fw_dump_info(mtlk_ccr_t *ccr, wave_fw_dump_info_t **fw_info)
{
  MTLK_ASSERT(NULL != fw_info);
  return ccr->card_api->get_fw_dump_info(&ccr->mem, fw_info);
}

static __INLINE int
mtlk_ccr_get_hw_dump_info(mtlk_ccr_t *ccr, wave_hw_dump_info_t **hw_dump_info)
{
  MTLK_ASSERT(NULL != hw_dump_info);
  return ccr->card_api->get_hw_dump_info(&ccr->mem, hw_dump_info);
}

static __INLINE void *
mtlk_ccr_get_dev_ctx(mtlk_ccr_t *ccr)
{
    return ccr->mem.dev_ctx;
}

#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
static __INLINE void
mtlk_ccr_read_hw_timestamp(mtlk_ccr_t *ccr, uint32 *low, uint32 *high)
{
  ccr->card_api->read_hw_timestamp(&ccr->mem, low, high);
}
#endif /* CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED */

static __INLINE void
mtlk_ccr_pcie_set_msi_intr_mode(mtlk_ccr_t *ccr, uint32 mode)
{
  ccr->card_api->set_msi_intr_mode(&ccr->mem, mode);
}

static __INLINE void
mtlk_ccr_enable_interrupts(mtlk_ccr_t *ccr, uint32 mask)
{
  ccr->card_api->enable_interrupts(&ccr->mem, mask);

#ifdef CPTCFG_IWLWAV_USE_INTERRUPT_POLLING
  mtlk_osal_timer_set(&ccr->poll_interrupts,
                      _MTLK_CCR_INT_POLL_PERIOD_MS);
#endif
}

static __INLINE void
mtlk_ccr_disable_interrupts(mtlk_ccr_t *ccr, uint32 mask)
{
#ifdef CPTCFG_IWLWAV_USE_INTERRUPT_POLLING
  mtlk_osal_timer_cancel(&ccr->poll_interrupts);
#endif

  ccr->card_api->disable_interrupts(&ccr->mem, mask);
}

static __INLINE uint32
mtlk_ccr_clear_interrupts_if_pending(mtlk_ccr_t *ccr, uint32 clear_mask)
{
  return ccr->card_api->clear_interrupts_if_pending(&ccr->mem, clear_mask);
}

/* Clear interrupts unconditionally */
#ifdef MTLK_CCR_CLEAR_INT_UNCONDIT

static __INLINE uint32
mtlk_ccr_clear_interrupts(mtlk_ccr_t *ccr, uint32 clear_mask)
{
  return ccr->card_api->clear_interrupts_if_pending(&ccr->mem, clear_mask);
}
#endif /* MTLK_CCR_CLEAR_INT_UNCONDIT */

static __INLINE BOOL
mtlk_ccr_disable_interrupts_if_pending(mtlk_ccr_t *ccr, uint32 mask)
{
  BOOL res = ccr->card_api->disable_interrupts_if_pending(&ccr->mem, mask);

#ifdef CPTCFG_IWLWAV_USE_INTERRUPT_POLLING
  if(res)
    mtlk_osal_timer_cancel(&ccr->poll_interrupts);
#endif

  return res;
}

static __INLINE void
mtlk_ccr_initiate_doorbell_inerrupt(mtlk_ccr_t *ccr)
{
  ccr->card_api->initiate_doorbell_inerrupt(&ccr->mem);
}


static __INLINE uint32
mtlk_ccr_is_interrupt_pending(mtlk_ccr_t *ccr)
{
    return ccr->card_api->is_interrupt_pending(&ccr->mem);
}

static __INLINE void
mtlk_ccr_recover_interrupts(mtlk_ccr_t *ccr)
{
  ccr->card_api->recover_interrupts(&ccr->mem);
}

static __INLINE void
mtlk_ccr_release_ctl_from_reset(mtlk_ccr_t *ccr)
{
  ccr->card_api->release_ctl_from_reset(&ccr->mem, ccr->chip_info->id);
}

static __INLINE void
mtlk_ccr_put_ctl_to_reset(mtlk_ccr_t *ccr)
{
  ccr->card_api->put_ctl_to_reset(&ccr->mem);
}

static __INLINE void
mtlk_ccr_exit_debug_mode(mtlk_ccr_t *ccr)
{
  ccr->card_api->exit_debug_mode(&ccr->mem);
}

static __INLINE void
mtlk_ccr_put_cpus_to_reset(mtlk_ccr_t *ccr)
{
  ccr->card_api->put_cpus_to_reset(&ccr->mem);
}

static __INLINE void
mtlk_ccr_reset_mac(mtlk_ccr_t *ccr)
{
  ccr->card_api->reset_mac(&ccr->mem);
}

static __INLINE void
mtlk_ccr_power_on_cpus(mtlk_ccr_t *ccr)
{
  ccr->card_api->power_on_cpus(&ccr->mem);
}

static __INLINE void
mtlk_ccr_bist_efuse(mtlk_ccr_t *ccr)
{
  return ccr->card_api->bist_efuse(&ccr->mem, ccr->chip_info->id);
}

static __INLINE int
mtlk_ccr_init_pll(mtlk_ccr_t *ccr)
{
  return ccr->card_api->init_pll(&ccr->mem, ccr->chip_info->id);
}

static __INLINE void
mtlk_ccr_release_cpus_reset(mtlk_ccr_t *ccr)
{
  ccr->card_api->release_cpus_reset(&ccr->mem);
}

static __INLINE BOOL
mtlk_ccr_check_bist(mtlk_ccr_t *ccr, uint32 *bist_result)
{
  return ccr->card_api->check_bist(&ccr->mem, bist_result);
}

static __INLINE void
mtlk_ccr_eeprom_protect (mtlk_ccr_t *ccr)
{
  mtlk_osal_mutex_acquire(&ccr->eeprom_mutex);
}

static __INLINE void
mtlk_ccr_eeprom_release (mtlk_ccr_t *ccr)
{
  mtlk_osal_mutex_release(&ccr->eeprom_mutex);
}

static __INLINE int
mtlk_ccr_eeprom_override_write (mtlk_ccr_t *ccr, uint32 data, uint32 mask)
{
  return ccr->card_api->eeprom_override_write(&ccr->mem, data, mask);
}

static __INLINE uint32
mtlk_ccr_eeprom_override_read (mtlk_ccr_t *ccr)
{
  return ccr->card_api->eeprom_override_read(&ccr->mem);
}

static __INLINE uint32
mtlk_ccr_get_mips_freq (mtlk_ccr_t *ccr)
{
  return ccr->card_api->get_mips_freq(&ccr->mem, ccr->chip_info->id);
}

static __INLINE void
mtlk_ccr_get_afe_value (mtlk_ccr_t *ccr, uint32 *values)
{
  /* return ccr->card_api->get_afe_value(&ccr->mem, values); */
  return ccr->card_api->get_afe_value(ccr, values); /* pass ccr */
}

static __INLINE uint32
mtlk_ccr_get_progmodel_version (mtlk_ccr_t *ccr)
{
  return ccr->card_api->get_progmodel_version(&ccr->mem);
}

static __INLINE void
mtlk_ccr_ctrl_ring_init(mtlk_ccr_t *ccr, mtlk_ring_regs *regs, unsigned char *mmb_base)
{
  return ccr->card_api->ctrl_ring_init(&ccr->mem, regs, mmb_base);
}

static __INLINE uint32
mtlk_ccr_ring_get_hds_to_process (mtlk_ccr_t *ccr, mtlk_ring_regs *regs)
{
  return ccr->card_api->ring_get_hds_to_process(&ccr->mem, regs);
}

static __INLINE void
mtlk_ccr_ring_clear_interrupt (mtlk_ccr_t *ccr, mtlk_ring_regs *regs, uint32 hds_processed)
{
  MTLK_ASSERT(mtlk_ccr_ring_get_hds_to_process(ccr, regs) >= hds_processed);
  return ccr->card_api->ring_clear_interrupt(regs, hds_processed);
}

static __INLINE void
mtlk_ccr_ring_initiate_doorbell_interrupt (mtlk_ccr_t *ccr, mtlk_ring_regs *regs)
{
  return ccr->card_api->ring_initiate_doorbell_interrupt(&ccr->mem, regs);
}

static __INLINE void
mtlk_ccr_print_irq_regs(mtlk_ccr_t *ccr)
{
  return ccr->card_api->print_irq_regs(&ccr->mem);
}

static __INLINE void
mtlk_ccr_eeprom_init(mtlk_ccr_t *ccr)
{
  return ccr->card_api->eeprom_init(&ccr->mem);
}

static __INLINE uint32
__mtlk_ccr_efuse_get_size(mtlk_ccr_t *ccr)
{
  return ccr->card_api->efuse_get_size(&ccr->mem);
}

static __INLINE uint32
__mtlk_ccr_efuse_read(mtlk_ccr_t *ccr, void *buf, uint32 efuse_offset, uint32 size)
{
  return ccr->card_api->efuse_read(&ccr->mem, buf, efuse_offset, size);
}

static __INLINE uint32
__mtlk_ccr_efuse_write(mtlk_ccr_t *ccr, void *buf, uint32 efuse_offset, uint32 size)
{
  return ccr->card_api->efuse_write(&ccr->mem, buf, efuse_offset, size);
}

static __INLINE uint32
mtlk_ccr_read_phy_inband_power(mtlk_ccr_t *ccr, unsigned radio_id, uint32 ant_mask,
                               uint32 *noise_estim, uint32 *system_gain)
{
  return ccr->card_api->read_phy_inband_power(&ccr->mem, radio_id, ant_mask, noise_estim, system_gain);
}

static __INLINE uint32
mtlk_ccr_read_hw_type (mtlk_ccr_t *ccr)
{
  return ccr->card_api->read_hw_type(&ccr->mem);
}

static __INLINE uint32
mtlk_ccr_read_hw_rev (mtlk_ccr_t *ccr)
{
  return ccr->card_api->read_hw_rev(&ccr->mem);
}

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID
