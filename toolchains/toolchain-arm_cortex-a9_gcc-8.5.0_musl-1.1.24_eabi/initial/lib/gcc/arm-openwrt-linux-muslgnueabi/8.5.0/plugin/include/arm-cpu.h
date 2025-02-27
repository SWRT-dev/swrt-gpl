/* -*- buffer-read-only: t -*-
   Generated automatically by parsecpu.awk from arm-cpus.in.
   Do not edit.

   Copyright (C) 2011-2018 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 3,
   or (at your option) any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public
   License along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

enum processor_type
{
  TARGET_CPU_arm2,
  TARGET_CPU_arm250,
  TARGET_CPU_arm3,
  TARGET_CPU_arm6,
  TARGET_CPU_arm60,
  TARGET_CPU_arm600,
  TARGET_CPU_arm610,
  TARGET_CPU_arm620,
  TARGET_CPU_arm7,
  TARGET_CPU_arm7d,
  TARGET_CPU_arm7di,
  TARGET_CPU_arm70,
  TARGET_CPU_arm700,
  TARGET_CPU_arm700i,
  TARGET_CPU_arm710,
  TARGET_CPU_arm720,
  TARGET_CPU_arm710c,
  TARGET_CPU_arm7100,
  TARGET_CPU_arm7500,
  TARGET_CPU_arm7500fe,
  TARGET_CPU_arm7m,
  TARGET_CPU_arm7dm,
  TARGET_CPU_arm7dmi,
  TARGET_CPU_arm8,
  TARGET_CPU_arm810,
  TARGET_CPU_strongarm,
  TARGET_CPU_strongarm110,
  TARGET_CPU_strongarm1100,
  TARGET_CPU_strongarm1110,
  TARGET_CPU_fa526,
  TARGET_CPU_fa626,
  TARGET_CPU_arm7tdmi,
  TARGET_CPU_arm7tdmis,
  TARGET_CPU_arm710t,
  TARGET_CPU_arm720t,
  TARGET_CPU_arm740t,
  TARGET_CPU_arm9,
  TARGET_CPU_arm9tdmi,
  TARGET_CPU_arm920,
  TARGET_CPU_arm920t,
  TARGET_CPU_arm922t,
  TARGET_CPU_arm940t,
  TARGET_CPU_ep9312,
  TARGET_CPU_arm10tdmi,
  TARGET_CPU_arm1020t,
  TARGET_CPU_arm9e,
  TARGET_CPU_arm946es,
  TARGET_CPU_arm966es,
  TARGET_CPU_arm968es,
  TARGET_CPU_arm10e,
  TARGET_CPU_arm1020e,
  TARGET_CPU_arm1022e,
  TARGET_CPU_xscale,
  TARGET_CPU_iwmmxt,
  TARGET_CPU_iwmmxt2,
  TARGET_CPU_fa606te,
  TARGET_CPU_fa626te,
  TARGET_CPU_fmp626,
  TARGET_CPU_fa726te,
  TARGET_CPU_arm926ejs,
  TARGET_CPU_arm1026ejs,
  TARGET_CPU_arm1136js,
  TARGET_CPU_arm1136jfs,
  TARGET_CPU_arm1176jzs,
  TARGET_CPU_arm1176jzfs,
  TARGET_CPU_mpcorenovfp,
  TARGET_CPU_mpcore,
  TARGET_CPU_arm1156t2s,
  TARGET_CPU_arm1156t2fs,
  TARGET_CPU_cortexm1,
  TARGET_CPU_cortexm0,
  TARGET_CPU_cortexm0plus,
  TARGET_CPU_cortexm1smallmultiply,
  TARGET_CPU_cortexm0smallmultiply,
  TARGET_CPU_cortexm0plussmallmultiply,
  TARGET_CPU_genericv7a,
  TARGET_CPU_cortexa5,
  TARGET_CPU_cortexa7,
  TARGET_CPU_cortexa8,
  TARGET_CPU_cortexa9,
  TARGET_CPU_cortexa12,
  TARGET_CPU_cortexa15,
  TARGET_CPU_cortexa17,
  TARGET_CPU_cortexr4,
  TARGET_CPU_cortexr4f,
  TARGET_CPU_cortexr5,
  TARGET_CPU_cortexr7,
  TARGET_CPU_cortexr8,
  TARGET_CPU_cortexm7,
  TARGET_CPU_cortexm4,
  TARGET_CPU_cortexm3,
  TARGET_CPU_marvell_pj4,
  TARGET_CPU_cortexa15cortexa7,
  TARGET_CPU_cortexa17cortexa7,
  TARGET_CPU_cortexa32,
  TARGET_CPU_cortexa35,
  TARGET_CPU_cortexa53,
  TARGET_CPU_cortexa57,
  TARGET_CPU_cortexa72,
  TARGET_CPU_cortexa73,
  TARGET_CPU_exynosm1,
  TARGET_CPU_xgene1,
  TARGET_CPU_cortexa57cortexa53,
  TARGET_CPU_cortexa72cortexa53,
  TARGET_CPU_cortexa73cortexa35,
  TARGET_CPU_cortexa73cortexa53,
  TARGET_CPU_cortexa55,
  TARGET_CPU_cortexa75,
  TARGET_CPU_cortexa75cortexa55,
  TARGET_CPU_neoversev1,
  TARGET_CPU_neoversen2,
  TARGET_CPU_cortexm23,
  TARGET_CPU_cortexm33,
  TARGET_CPU_cortexr52,
  TARGET_CPU_arm_none
};

enum arch_type
{
  TARGET_ARCH_armv2,
  TARGET_ARCH_armv2a,
  TARGET_ARCH_armv3,
  TARGET_ARCH_armv3m,
  TARGET_ARCH_armv4,
  TARGET_ARCH_armv4t,
  TARGET_ARCH_armv5,
  TARGET_ARCH_armv5t,
  TARGET_ARCH_armv5e,
  TARGET_ARCH_armv5te,
  TARGET_ARCH_armv5tej,
  TARGET_ARCH_armv6,
  TARGET_ARCH_armv6j,
  TARGET_ARCH_armv6k,
  TARGET_ARCH_armv6z,
  TARGET_ARCH_armv6kz,
  TARGET_ARCH_armv6zk,
  TARGET_ARCH_armv6t2,
  TARGET_ARCH_armv6_m,
  TARGET_ARCH_armv6s_m,
  TARGET_ARCH_armv7,
  TARGET_ARCH_armv7_a,
  TARGET_ARCH_armv7ve,
  TARGET_ARCH_armv7_r,
  TARGET_ARCH_armv7_m,
  TARGET_ARCH_armv7e_m,
  TARGET_ARCH_armv8_a,
  TARGET_ARCH_armv8_1_a,
  TARGET_ARCH_armv8_2_a,
  TARGET_ARCH_armv8_3_a,
  TARGET_ARCH_armv8_4_a,
  TARGET_ARCH_armv8_m_base,
  TARGET_ARCH_armv8_m_main,
  TARGET_ARCH_armv8_r,
  TARGET_ARCH_iwmmxt,
  TARGET_ARCH_iwmmxt2,
  TARGET_ARCH_arm_none
};

enum fpu_type
{
  TARGET_FPU_vfp,
  TARGET_FPU_vfpv2,
  TARGET_FPU_vfpv3,
  TARGET_FPU_vfpv3_fp16,
  TARGET_FPU_vfpv3_d16,
  TARGET_FPU_vfpv3_d16_fp16,
  TARGET_FPU_vfpv3xd,
  TARGET_FPU_vfpv3xd_fp16,
  TARGET_FPU_neon,
  TARGET_FPU_neon_vfpv3,
  TARGET_FPU_neon_fp16,
  TARGET_FPU_vfpv4,
  TARGET_FPU_neon_vfpv4,
  TARGET_FPU_vfpv4_d16,
  TARGET_FPU_fpv4_sp_d16,
  TARGET_FPU_fpv5_sp_d16,
  TARGET_FPU_fpv5_d16,
  TARGET_FPU_fp_armv8,
  TARGET_FPU_neon_fp_armv8,
  TARGET_FPU_crypto_neon_fp_armv8,
  TARGET_FPU_vfp3,
  TARGET_FPU_auto
};
