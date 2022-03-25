//-----------------------------------------------------------------------------
// LSD Generator
//-----------------------------------------------------------------------------
// Perl Package        : LSD::generator::targetC (v1.1)
// LSD Source          : C:/Users/huchunfe/Perforce/huchunfe_huchunfe-MOBL1_dev.FalcONT/ipg_lsd/lsd_sys/source/xml/reg_files/buffer_manager.xml
// Register File Name  : BUFFER_MANAGER
// Register File Title : Buffer Manager Register Description
// Register Width      : 32
// Note                : Doxygen compliant comments
//-----------------------------------------------------------------------------

#ifndef _BUFFER_MANAGER_H
#define _BUFFER_MANAGER_H

//! \defgroup BUFFER_MANAGER Register File BUFFER_MANAGER - Buffer Manager Register Description
//! @{

//! Base Address of BUFFER_MANAGER
#define BUFFER_MANAGER_MODULE_BASE 0x18B00000u

//! \defgroup BMGR_RDL_BMNGR_CTRL Register BMGR_RDL_BMNGR_CTRL - bmngr_ctrl
//! @{

//! Register Offset (relative)
#define BMGR_RDL_BMNGR_CTRL 0x0
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_BMNGR_CTRL 0x18B00000u

//! Register Reset Value
#define BMGR_RDL_BMNGR_CTRL_RST 0x00000000u

//! Field BMNG_CLNT_EN - BMNG_CLNT_EN
#define BMGR_RDL_BMNGR_CTRL_BMNG_CLNT_EN_POS 0
//! Field BMNG_CLNT_EN - BMNG_CLNT_EN
#define BMGR_RDL_BMNGR_CTRL_BMNG_CLNT_EN_MASK 0x1u

//! Field POP_DISABLE - POP_DISABLE
#define BMGR_RDL_BMNGR_CTRL_POP_DISABLE_POS 1
//! Field POP_DISABLE - POP_DISABLE
#define BMGR_RDL_BMNGR_CTRL_POP_DISABLE_MASK 0x2u

//! Field PUSH_DISABLE - PUSH_DISABLE
#define BMGR_RDL_BMNGR_CTRL_PUSH_DISABLE_POS 2
//! Field PUSH_DISABLE - PUSH_DISABLE
#define BMGR_RDL_BMNGR_CTRL_PUSH_DISABLE_MASK 0x4u

//! Field PM_ARB_DISABLE - PM_ARB_DISABLE
#define BMGR_RDL_BMNGR_CTRL_PM_ARB_DISABLE_POS 3
//! Field PM_ARB_DISABLE - PM_ARB_DISABLE
#define BMGR_RDL_BMNGR_CTRL_PM_ARB_DISABLE_MASK 0x8u

//! @}

//! \defgroup BMGR_RDL_POOL_MIN_GRNT_MASK Register BMGR_RDL_POOL_MIN_GRNT_MASK - pool_min_grnt_mask
//! @{

//! Register Offset (relative)
#define BMGR_RDL_POOL_MIN_GRNT_MASK 0x4
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_POOL_MIN_GRNT_MASK 0x18B00004u

//! Register Reset Value
#define BMGR_RDL_POOL_MIN_GRNT_MASK_RST 0x00000000u

//! Field POOL_MIN_GRNT_MASK - POOL_MIN_GRNT_MASK
#define BMGR_RDL_POOL_MIN_GRNT_MASK_POOL_MIN_GRNT_MASK_POS 0
//! Field POOL_MIN_GRNT_MASK - POOL_MIN_GRNT_MASK
#define BMGR_RDL_POOL_MIN_GRNT_MASK_POOL_MIN_GRNT_MASK_MASK 0xFFFFu

//! @}

//! \defgroup BMGR_RDL_POOL_ENABLE Register BMGR_RDL_POOL_ENABLE - pool_enable
//! @{

//! Register Offset (relative)
#define BMGR_RDL_POOL_ENABLE 0x8
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_POOL_ENABLE 0x18B00008u

//! Register Reset Value
#define BMGR_RDL_POOL_ENABLE_RST 0x00000000u

//! Field PU_POOL_EN - PU_POOL_EN
#define BMGR_RDL_POOL_ENABLE_PU_POOL_EN_POS 0
//! Field PU_POOL_EN - PU_POOL_EN
#define BMGR_RDL_POOL_ENABLE_PU_POOL_EN_MASK 0xFFFFu

//! Field PFTCH_POOL_EN - PFTCH_POOL_EN
#define BMGR_RDL_POOL_ENABLE_PFTCH_POOL_EN_POS 16
//! Field PFTCH_POOL_EN - PFTCH_POOL_EN
#define BMGR_RDL_POOL_ENABLE_PFTCH_POOL_EN_MASK 0xFFFF0000u

//! @}

//! \defgroup BMGR_RDL_POOL_RESET Register BMGR_RDL_POOL_RESET - pool_reset
//! @{

//! Register Offset (relative)
#define BMGR_RDL_POOL_RESET 0xC
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_POOL_RESET 0x18B0000Cu

//! Register Reset Value
#define BMGR_RDL_POOL_RESET_RST 0x0000FFFFu

//! Field POOL_RST - POOL_RST
#define BMGR_RDL_POOL_RESET_POOL_RST_POS 0
//! Field POOL_RST - POOL_RST
#define BMGR_RDL_POOL_RESET_POOL_RST_MASK 0xFFFFu

//! @}

//! \defgroup BMGR_RDL_OCPM_BURST_SIZE Register BMGR_RDL_OCPM_BURST_SIZE - ocpm_burst_size
//! @{

//! Register Offset (relative)
#define BMGR_RDL_OCPM_BURST_SIZE 0x10
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_OCPM_BURST_SIZE 0x18B00010u

//! Register Reset Value
#define BMGR_RDL_OCPM_BURST_SIZE_RST 0x00000000u

//! Field OCPM_BRST_SIZE - OCPM_BRST_SIZE
#define BMGR_RDL_OCPM_BURST_SIZE_OCPM_BRST_SIZE_POS 0
//! Field OCPM_BRST_SIZE - OCPM_BRST_SIZE
#define BMGR_RDL_OCPM_BURST_SIZE_OCPM_BRST_SIZE_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BMGR_RDL_OCPM_NUM_OF_BURSTS Register BMGR_RDL_OCPM_NUM_OF_BURSTS - ocpm_num_of_bursts
//! @{

//! Register Offset (relative)
#define BMGR_RDL_OCPM_NUM_OF_BURSTS 0x14
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_OCPM_NUM_OF_BURSTS 0x18B00014u

//! Register Reset Value
#define BMGR_RDL_OCPM_NUM_OF_BURSTS_RST 0x00000000u

//! Field OCPM_NUM_OF_BRST - OCPM_NUM_OF_BRST
#define BMGR_RDL_OCPM_NUM_OF_BURSTS_OCPM_NUM_OF_BRST_POS 0
//! Field OCPM_NUM_OF_BRST - OCPM_NUM_OF_BRST
#define BMGR_RDL_OCPM_NUM_OF_BURSTS_OCPM_NUM_OF_BRST_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BMGR_RDL_POOL_SIZE Register BMGR_RDL_POOL_SIZE - pool_size
//! @{

//! Register Offset (relative)
#define BMGR_RDL_POOL_SIZE 0x20
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_POOL_SIZE 0x18B00020u

//! Register Reset Value
#define BMGR_RDL_POOL_SIZE_RST 0x00000000u

//! Field POOL_SIZE - POOL_SIZE
#define BMGR_RDL_POOL_SIZE_POOL_SIZE_POS 0
//! Field POOL_SIZE - POOL_SIZE
#define BMGR_RDL_POOL_SIZE_POOL_SIZE_MASK 0xFFFFFFu

//! @}

//! \defgroup BMGR_RDL_GRP_AVL Register BMGR_RDL_GRP_AVL - grp_avl
//! @{

//! Register Offset (relative)
#define BMGR_RDL_GRP_AVL 0x100
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_GRP_AVL 0x18B00100u

//! Register Reset Value
#define BMGR_RDL_GRP_AVL_RST 0x00000000u

//! Field GRP_AVAILABLE - GRP_AVAILABLE
#define BMGR_RDL_GRP_AVL_GRP_AVAILABLE_POS 0
//! Field GRP_AVAILABLE - GRP_AVAILABLE
#define BMGR_RDL_GRP_AVL_GRP_AVAILABLE_MASK 0xFFFFFFu

//! @}

//! \defgroup BMGR_RDL_GRP_RES Register BMGR_RDL_GRP_RES - grp_res
//! @{

//! Register Offset (relative)
#define BMGR_RDL_GRP_RES 0x200
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_GRP_RES 0x18B00200u

//! Register Reset Value
#define BMGR_RDL_GRP_RES_RST 0x00000000u

//! Field GRP_RESERVED - GRP_RESERVED
#define BMGR_RDL_GRP_RES_GRP_RESERVED_POS 0
//! Field GRP_RESERVED - GRP_RESERVED
#define BMGR_RDL_GRP_RES_GRP_RESERVED_MASK 0xFFFFFFu

//! @}

//! \defgroup BMGR_RDL_PCU_FIFO_BASE_ADDR Register BMGR_RDL_PCU_FIFO_BASE_ADDR - pcu_fifo_base_addr
//! @{

//! Register Offset (relative)
#define BMGR_RDL_PCU_FIFO_BASE_ADDR 0x400
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_PCU_FIFO_BASE_ADDR 0x18B00400u

//! Register Reset Value
#define BMGR_RDL_PCU_FIFO_BASE_ADDR_RST 0x00000000u

//! Field PCU_FIFO_BASE_ADDR - PCU_FIFO_BASE_ADDR
#define BMGR_RDL_PCU_FIFO_BASE_ADDR_PCU_FIFO_BASE_ADDR_POS 0
//! Field PCU_FIFO_BASE_ADDR - PCU_FIFO_BASE_ADDR
#define BMGR_RDL_PCU_FIFO_BASE_ADDR_PCU_FIFO_BASE_ADDR_MASK 0xFFFFu

//! @}

//! \defgroup BMGR_RDL_PCU_FIFO_SIZE Register BMGR_RDL_PCU_FIFO_SIZE - pcu_fifo_size
//! @{

//! Register Offset (relative)
#define BMGR_RDL_PCU_FIFO_SIZE 0x440
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_PCU_FIFO_SIZE 0x18B00440u

//! Register Reset Value
#define BMGR_RDL_PCU_FIFO_SIZE_RST 0x00000100u

//! Field PCU_FIFO_SIZE - PCU_FIFO_SIZE
#define BMGR_RDL_PCU_FIFO_SIZE_PCU_FIFO_SIZE_POS 0
//! Field PCU_FIFO_SIZE - PCU_FIFO_SIZE
#define BMGR_RDL_PCU_FIFO_SIZE_PCU_FIFO_SIZE_MASK 0xFFFFu

//! @}

//! \defgroup BMGR_RDL_PCU_FIFO_OCC Register BMGR_RDL_PCU_FIFO_OCC - pcu_fifo_occ
//! @{

//! Register Offset (relative)
#define BMGR_RDL_PCU_FIFO_OCC 0x480
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_PCU_FIFO_OCC 0x18B00480u

//! Register Reset Value
#define BMGR_RDL_PCU_FIFO_OCC_RST 0x00000000u

//! Field PCU_FIFO_OCC - PCU_FIFO_OCC
#define BMGR_RDL_PCU_FIFO_OCC_PCU_FIFO_OCC_POS 0
//! Field PCU_FIFO_OCC - PCU_FIFO_OCC
#define BMGR_RDL_PCU_FIFO_OCC_PCU_FIFO_OCC_MASK 0xFFFFu

//! @}

//! \defgroup BMGR_RDL_PCU_FIFO_PROG_EMPTY Register BMGR_RDL_PCU_FIFO_PROG_EMPTY - pcu_fifo_prog_empty
//! @{

//! Register Offset (relative)
#define BMGR_RDL_PCU_FIFO_PROG_EMPTY 0x4C0
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_PCU_FIFO_PROG_EMPTY 0x18B004C0u

//! Register Reset Value
#define BMGR_RDL_PCU_FIFO_PROG_EMPTY_RST 0x00000055u

//! Field PCU_FIFO_P_EMPTY - PCU_FIFO_P_EMPTY
#define BMGR_RDL_PCU_FIFO_PROG_EMPTY_PCU_FIFO_P_EMPTY_POS 0
//! Field PCU_FIFO_P_EMPTY - PCU_FIFO_P_EMPTY
#define BMGR_RDL_PCU_FIFO_PROG_EMPTY_PCU_FIFO_P_EMPTY_MASK 0xFFFFu

//! @}

//! \defgroup BMGR_RDL_PCU_FIFO_PROG_FULL Register BMGR_RDL_PCU_FIFO_PROG_FULL - pcu_fifo_prog_full
//! @{

//! Register Offset (relative)
#define BMGR_RDL_PCU_FIFO_PROG_FULL 0x500
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_PCU_FIFO_PROG_FULL 0x18B00500u

//! Register Reset Value
#define BMGR_RDL_PCU_FIFO_PROG_FULL_RST 0x000000AAu

//! Field PCU_FIFO_P_FULL - PCU_FIFO_P_FULL
#define BMGR_RDL_PCU_FIFO_PROG_FULL_PCU_FIFO_P_FULL_POS 0
//! Field PCU_FIFO_P_FULL - PCU_FIFO_P_FULL
#define BMGR_RDL_PCU_FIFO_PROG_FULL_PCU_FIFO_P_FULL_MASK 0xFFFFu

//! @}

//! \defgroup BMGR_RDL_EXT_FIFO_BASE_ADDR_LOW Register BMGR_RDL_EXT_FIFO_BASE_ADDR_LOW - ext_fifo_base_addr_low
//! @{

//! Register Offset (relative)
#define BMGR_RDL_EXT_FIFO_BASE_ADDR_LOW 0x540
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_EXT_FIFO_BASE_ADDR_LOW 0x18B00540u

//! Register Reset Value
#define BMGR_RDL_EXT_FIFO_BASE_ADDR_LOW_RST 0x00000000u

//! Field EXT_FIFO_BASE_ADDR_LOW - EXT_FIFO_BASE_ADDR_LOW
#define BMGR_RDL_EXT_FIFO_BASE_ADDR_LOW_EXT_FIFO_BASE_ADDR_LOW_POS 0
//! Field EXT_FIFO_BASE_ADDR_LOW - EXT_FIFO_BASE_ADDR_LOW
#define BMGR_RDL_EXT_FIFO_BASE_ADDR_LOW_EXT_FIFO_BASE_ADDR_LOW_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BMGR_RDL_EXT_FIFO_BASE_ADDR_HIGH Register BMGR_RDL_EXT_FIFO_BASE_ADDR_HIGH - ext_fifo_base_addr_high
//! @{

//! Register Offset (relative)
#define BMGR_RDL_EXT_FIFO_BASE_ADDR_HIGH 0x580
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_EXT_FIFO_BASE_ADDR_HIGH 0x18B00580u

//! Register Reset Value
#define BMGR_RDL_EXT_FIFO_BASE_ADDR_HIGH_RST 0x00000000u

//! Field EXT_FIFO_BASE_ADDR_HIGH - EXT_FIFO_BASE_ADDR_HIGH
#define BMGR_RDL_EXT_FIFO_BASE_ADDR_HIGH_EXT_FIFO_BASE_ADDR_HIGH_POS 0
//! Field EXT_FIFO_BASE_ADDR_HIGH - EXT_FIFO_BASE_ADDR_HIGH
#define BMGR_RDL_EXT_FIFO_BASE_ADDR_HIGH_EXT_FIFO_BASE_ADDR_HIGH_MASK 0xFFu

//! @}

//! \defgroup BMGR_RDL_EXT_FIFO_OCC Register BMGR_RDL_EXT_FIFO_OCC - ext_fifo_occ
//! @{

//! Register Offset (relative)
#define BMGR_RDL_EXT_FIFO_OCC 0x5C0
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_EXT_FIFO_OCC 0x18B005C0u

//! Register Reset Value
#define BMGR_RDL_EXT_FIFO_OCC_RST 0x00000000u

//! Field EXT_FIFO_OCC - EXT_FIFO_OCC
#define BMGR_RDL_EXT_FIFO_OCC_EXT_FIFO_OCC_POS 0
//! Field EXT_FIFO_OCC - EXT_FIFO_OCC
#define BMGR_RDL_EXT_FIFO_OCC_EXT_FIFO_OCC_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BMGR_RDL_POOL_CNT Register BMGR_RDL_POOL_CNT - pool_cnt
//! @{

//! Register Offset (relative)
#define BMGR_RDL_POOL_CNT 0x5D0
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_POOL_CNT 0x18B005D0u

//! Register Reset Value
#define BMGR_RDL_POOL_CNT_RST 0x00000000u

//! Field POOL_ALLOC_CNT - POOL_ALLOC_CNT
#define BMGR_RDL_POOL_CNT_POOL_ALLOC_CNT_POS 0
//! Field POOL_ALLOC_CNT - POOL_ALLOC_CNT
#define BMGR_RDL_POOL_CNT_POOL_ALLOC_CNT_MASK 0xFFFFFFu

//! @}

//! \defgroup BMGR_RDL_NULL_CNT Register BMGR_RDL_NULL_CNT - null_cnt
//! @{

//! Register Offset (relative)
#define BMGR_RDL_NULL_CNT 0x5E0
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_NULL_CNT 0x18B005E0u

//! Register Reset Value
#define BMGR_RDL_NULL_CNT_RST 0x00000000u

//! Field NULL_CNT - NULL_CNT
#define BMGR_RDL_NULL_CNT_NULL_CNT_POS 0
//! Field NULL_CNT - NULL_CNT
#define BMGR_RDL_NULL_CNT_NULL_CNT_MASK 0xFFFFFFu

//! @}

//! \defgroup BMGR_RDL_POP_CNT Register BMGR_RDL_POP_CNT - pop_cnt
//! @{

//! Register Offset (relative)
#define BMGR_RDL_POP_CNT 0x660
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_POP_CNT 0x18B00660u

//! Register Reset Value
#define BMGR_RDL_POP_CNT_RST 0x00000000u

//! Field POP_CNT - POP_CNT
#define BMGR_RDL_POP_CNT_POP_CNT_POS 0
//! Field POP_CNT - POP_CNT
#define BMGR_RDL_POP_CNT_POP_CNT_MASK 0xFFFFFFu

//! @}

//! \defgroup BMGR_RDL_PUSH_CNT Register BMGR_RDL_PUSH_CNT - push_cnt
//! @{

//! Register Offset (relative)
#define BMGR_RDL_PUSH_CNT 0x670
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_PUSH_CNT 0x18B00670u

//! Register Reset Value
#define BMGR_RDL_PUSH_CNT_RST 0x00000000u

//! Field PUSH_CNT - PUSH_CNT
#define BMGR_RDL_PUSH_CNT_PUSH_CNT_POS 0
//! Field PUSH_CNT - PUSH_CNT
#define BMGR_RDL_PUSH_CNT_PUSH_CNT_MASK 0xFFFFFFu

//! @}

//! \defgroup BMGR_RDL_BURST_WR_CNT Register BMGR_RDL_BURST_WR_CNT - burst_wr_cnt
//! @{

//! Register Offset (relative)
#define BMGR_RDL_BURST_WR_CNT 0x680
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_BURST_WR_CNT 0x18B00680u

//! Register Reset Value
#define BMGR_RDL_BURST_WR_CNT_RST 0x00000000u

//! Field DDR_BRST_WR_CNT - DDR_BRST_WR_CNT
#define BMGR_RDL_BURST_WR_CNT_DDR_BRST_WR_CNT_POS 0
//! Field DDR_BRST_WR_CNT - DDR_BRST_WR_CNT
#define BMGR_RDL_BURST_WR_CNT_DDR_BRST_WR_CNT_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BMGR_RDL_BURST_RD_CNT Register BMGR_RDL_BURST_RD_CNT - burst_rd_cnt
//! @{

//! Register Offset (relative)
#define BMGR_RDL_BURST_RD_CNT 0x690
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_BURST_RD_CNT 0x18B00690u

//! Register Reset Value
#define BMGR_RDL_BURST_RD_CNT_RST 0x00000000u

//! Field DDR_BRST_RD_CNT - DDR_BRST_RD_CNT
#define BMGR_RDL_BURST_RD_CNT_DDR_BRST_RD_CNT_POS 0
//! Field DDR_BRST_RD_CNT - DDR_BRST_RD_CNT
#define BMGR_RDL_BURST_RD_CNT_DDR_BRST_RD_CNT_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BMGR_RDL_WATERMARK_LOW_THRSH Register BMGR_RDL_WATERMARK_LOW_THRSH - watermark_low_thrsh
//! @{

//! Register Offset (relative)
#define BMGR_RDL_WATERMARK_LOW_THRSH 0x6A0
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_WATERMARK_LOW_THRSH 0x18B006A0u

//! Register Reset Value
#define BMGR_RDL_WATERMARK_LOW_THRSH_RST 0x00000000u

//! Field WM_LOW_TRSH - WM_LOW_TRSH
#define BMGR_RDL_WATERMARK_LOW_THRSH_WM_LOW_TRSH_POS 0
//! Field WM_LOW_TRSH - WM_LOW_TRSH
#define BMGR_RDL_WATERMARK_LOW_THRSH_WM_LOW_TRSH_MASK 0xFFFFFFFFu

//! @}

//! \defgroup BMGR_RDL_WATERMARK_LOW_CNT Register BMGR_RDL_WATERMARK_LOW_CNT - watermark_low_cnt
//! @{

//! Register Offset (relative)
#define BMGR_RDL_WATERMARK_LOW_CNT 0x6B0
//! Register Offset (absolute) for 1st Instance BUFFER_MANAGER
#define BUFFER_MANAGER_BMGR_RDL_WATERMARK_LOW_CNT 0x18B006B0u

//! Register Reset Value
#define BMGR_RDL_WATERMARK_LOW_CNT_RST 0x00000000u

//! Field WM_LOW_CNT - WM_LOW_CNT
#define BMGR_RDL_WATERMARK_LOW_CNT_WM_LOW_CNT_POS 0
//! Field WM_LOW_CNT - WM_LOW_CNT
#define BMGR_RDL_WATERMARK_LOW_CNT_WM_LOW_CNT_MASK 0xFFFFFFFFu

//! @}

//! @}

#endif
