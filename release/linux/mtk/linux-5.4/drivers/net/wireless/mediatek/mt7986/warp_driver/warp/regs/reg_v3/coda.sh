#!/bin/sh
# translate coda for warp driver usage
################################################
# wed0_c_header.h [module: wed0, H link]
################################################
# add some macro
sed -i '12 a #define REG_FLD(width, shift) (shift)' wed_hw.h

# change offset
sed -i 's/INREG32(.*\ /0x0000/g' wed_hw.h

# remove some line
sed -i '/wed0_BASE\|REG_FLD_GET\|REG_FLD_SET\|REG_FLD_VAL/d' wed_hw.h

################################################
# pao2_reg.h [module: pao0, sysc_reg link]
################################################
sed -i 's/PAO2_//g' pao_hw.h
sed -i 's/(.*\ 5/0x00001/g' pao_hw.h

################################################
# wo2_mcu_cfg_ls_reg.h [module: wo0_mcu_cfg_ls, sysc_reg link]
################################################
sed -i 's/WO2_MCU/WOX_MCU/g' wox_mcu_cfg_ls.h

################################################
# wo2_mcu_cfg_hs_reg.h [module: wo0_mcu_cfg_hs, sysc_reg link]
################################################
sed -i 's/WO2_MCU/WOX_MCU/g' wox_mcu_cfg_hs.h

################################################
# WO2_AP2WO_MCU_CCIF4_reg.h [module: WO0_WO2AP_MCU_CCIF4, sysc_reg link]
################################################
sed -i 's/WO2_AP2WO/WOX_AP2WO/g' wox_ap2wo_mcu_ccif4.h
