/* Generated automatically by the program `genflags'
   from the machine description file `md'.  */

#ifndef GCC_INSN_FLAGS_H
#define GCC_INSN_FLAGS_H

#define HAVE_ls2_alu1_turn_enabled_insn (TUNE_LOONGSON_2EF)
#define HAVE_ls2_alu2_turn_enabled_insn (TUNE_LOONGSON_2EF)
#define HAVE_ls2_falu1_turn_enabled_insn (TUNE_LOONGSON_2EF)
#define HAVE_ls2_falu2_turn_enabled_insn (TUNE_LOONGSON_2EF)
#define HAVE_trap 1
#define HAVE_addsf3 (TARGET_HARD_FLOAT)
#define HAVE_adddf3 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)
#define HAVE_addv2sf3 (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_subsf3 (TARGET_HARD_FLOAT)
#define HAVE_subdf3 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)
#define HAVE_subv2sf3 (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_subsi3 1
#define HAVE_subdi3 (TARGET_64BIT)
#define HAVE_mulv2sf3 (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mulsi3_mul3_nohilo (TARGET_LOONGSON_2EF || TARGET_LOONGSON_3A || ISA_HAS_R6MUL)
#define HAVE_muldi3_mul3_nohilo ((TARGET_LOONGSON_2EF || TARGET_LOONGSON_3A || ISA_HAS_R6DMUL) && (TARGET_64BIT))
#define HAVE_mulsi3_mul3 (ISA_HAS_MUL3)
#define HAVE_muldi3_mul3 ((ISA_HAS_DMUL3) && (TARGET_64BIT))
#define HAVE_mulsi3_internal (ISA_HAS_MULT && !TARGET_FIX_R4000)
#define HAVE_muldi3_internal ((ISA_HAS_DMULT && !TARGET_FIX_R4000) && (TARGET_64BIT))
#define HAVE_mulsi3_r4000 (ISA_HAS_MULT && TARGET_FIX_R4000)
#define HAVE_muldi3_r4000 ((ISA_HAS_DMULT && TARGET_FIX_R4000) && (TARGET_64BIT))
#define HAVE_mulsidi3_32bit (!TARGET_64BIT && (!TARGET_FIX_R4000 || ISA_HAS_DSP) && ISA_HAS_MULT)
#define HAVE_umulsidi3_32bit (!TARGET_64BIT && (!TARGET_FIX_R4000 || ISA_HAS_DSP) && ISA_HAS_MULT)
#define HAVE_mulsidi3_32bit_r4000 (!TARGET_64BIT && TARGET_FIX_R4000 && !ISA_HAS_DSP && ISA_HAS_MULT)
#define HAVE_umulsidi3_32bit_r4000 (!TARGET_64BIT && TARGET_FIX_R4000 && !ISA_HAS_DSP && ISA_HAS_MULT)
#define HAVE_mulsidi3_64bit (TARGET_64BIT && !TARGET_FIX_R4000 && !ISA_HAS_DMUL3 \
   && !TARGET_MIPS16 && ISA_HAS_MULT)
#define HAVE_umulsidi3_64bit (TARGET_64BIT && !TARGET_FIX_R4000 && !ISA_HAS_DMUL3 \
   && !TARGET_MIPS16 && ISA_HAS_MULT)
#define HAVE_mulsidi3_64bit_hilo (TARGET_64BIT && !TARGET_FIX_R4000)
#define HAVE_umulsidi3_64bit_hilo (TARGET_64BIT && !TARGET_FIX_R4000)
#define HAVE_mulsidi3_64bit_dmul (ISA_HAS_DMUL3)
#define HAVE_mulsidi3_64bit_r6dmul (ISA_HAS_R6DMUL)
#define HAVE_msubsidi4 (!TARGET_64BIT && (ISA_HAS_MSAC || GENERATE_MADD_MSUB || ISA_HAS_DSP))
#define HAVE_umsubsidi4 (!TARGET_64BIT && (ISA_HAS_MSAC || GENERATE_MADD_MSUB || ISA_HAS_DSP))
#define HAVE_smulsi3_highpart_r6 (ISA_HAS_R6MUL)
#define HAVE_umulsi3_highpart_r6 (ISA_HAS_R6MUL)
#define HAVE_smulsi3_highpart_internal (ISA_HAS_MULT && !ISA_HAS_MULHI && !TARGET_MIPS16)
#define HAVE_umulsi3_highpart_internal (ISA_HAS_MULT && !ISA_HAS_MULHI && !TARGET_MIPS16)
#define HAVE_smulsi3_highpart_mulhi_internal (ISA_HAS_MULHI)
#define HAVE_umulsi3_highpart_mulhi_internal (ISA_HAS_MULHI)
#define HAVE_smuldi3_highpart_r6 (ISA_HAS_R6DMUL)
#define HAVE_umuldi3_highpart_r6 (ISA_HAS_R6DMUL)
#define HAVE_smuldi3_highpart_internal (ISA_HAS_DMULT \
   && !TARGET_MIPS16 \
   && !(SIGN_EXTEND == ZERO_EXTEND && TARGET_FIX_VR4120))
#define HAVE_umuldi3_highpart_internal (ISA_HAS_DMULT \
   && !TARGET_MIPS16 \
   && !(ZERO_EXTEND == ZERO_EXTEND && TARGET_FIX_VR4120))
#define HAVE_mulditi3_internal (ISA_HAS_DMULT \
   && !TARGET_FIX_R4000 \
   && !(SIGN_EXTEND == ZERO_EXTEND && TARGET_FIX_VR4120))
#define HAVE_umulditi3_internal (ISA_HAS_DMULT \
   && !TARGET_FIX_R4000 \
   && !(ZERO_EXTEND == ZERO_EXTEND && TARGET_FIX_VR4120))
#define HAVE_mulditi3_r4000 (ISA_HAS_DMULT \
   && TARGET_FIX_R4000 \
   && !(SIGN_EXTEND == ZERO_EXTEND && TARGET_FIX_VR4120))
#define HAVE_umulditi3_r4000 (ISA_HAS_DMULT \
   && TARGET_FIX_R4000 \
   && !(ZERO_EXTEND == ZERO_EXTEND && TARGET_FIX_VR4120))
#define HAVE_madsi (TARGET_MAD)
#define HAVE_maddsidi4 ((TARGET_MAD || ISA_HAS_MACC || GENERATE_MADD_MSUB || ISA_HAS_DSP) \
   && !TARGET_64BIT)
#define HAVE_umaddsidi4 ((TARGET_MAD || ISA_HAS_MACC || GENERATE_MADD_MSUB || ISA_HAS_DSP) \
   && !TARGET_64BIT)
#define HAVE_divmodsi4_mips16 (ISA_HAS_DIV && !TARGET_FIX_VR4120 && TARGET_MIPS16)
#define HAVE_divmoddi4_mips16 ((ISA_HAS_DDIV && !TARGET_FIX_VR4120 && TARGET_MIPS16) && (TARGET_64BIT))
#define HAVE_udivmodsi4_mips16 (ISA_HAS_DIV && TARGET_MIPS16)
#define HAVE_udivmoddi4_mips16 ((ISA_HAS_DDIV && TARGET_MIPS16) && (TARGET_64BIT))
#define HAVE_divmodsi4_hilo_di ((ISA_HAS_DIV) && (!TARGET_64BIT))
#define HAVE_udivmodsi4_hilo_di ((ISA_HAS_DIV) && (!TARGET_64BIT))
#define HAVE_divmodsi4_hilo_ti ((ISA_HAS_DIV) && (TARGET_64BIT))
#define HAVE_udivmodsi4_hilo_ti ((ISA_HAS_DIV) && (TARGET_64BIT))
#define HAVE_divmoddi4_hilo_ti ((ISA_HAS_DDIV) && (TARGET_64BIT))
#define HAVE_udivmoddi4_hilo_ti ((ISA_HAS_DDIV) && (TARGET_64BIT))
#define HAVE_divsi3 (TARGET_LOONGSON_2EF || TARGET_LOONGSON_3A || ISA_HAS_R6DIV)
#define HAVE_udivsi3 (TARGET_LOONGSON_2EF || TARGET_LOONGSON_3A || ISA_HAS_R6DIV)
#define HAVE_divdi3 ((TARGET_LOONGSON_2EF || TARGET_LOONGSON_3A || ISA_HAS_R6DDIV) && (TARGET_64BIT))
#define HAVE_udivdi3 ((TARGET_LOONGSON_2EF || TARGET_LOONGSON_3A || ISA_HAS_R6DDIV) && (TARGET_64BIT))
#define HAVE_modsi3 (TARGET_LOONGSON_2EF || TARGET_LOONGSON_3A || ISA_HAS_R6DIV)
#define HAVE_umodsi3 (TARGET_LOONGSON_2EF || TARGET_LOONGSON_3A || ISA_HAS_R6DIV)
#define HAVE_moddi3 ((TARGET_LOONGSON_2EF || TARGET_LOONGSON_3A || ISA_HAS_R6DDIV) && (TARGET_64BIT))
#define HAVE_umoddi3 ((TARGET_LOONGSON_2EF || TARGET_LOONGSON_3A || ISA_HAS_R6DDIV) && (TARGET_64BIT))
#define HAVE_sqrtsf2 ((!ISA_MIPS1) && (TARGET_HARD_FLOAT))
#define HAVE_sqrtdf2 ((!ISA_MIPS1) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sqrtv2sf2 ((TARGET_SB1) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_abssf2 ((mips_abs == MIPS_IEEE_754_2008 || !HONOR_NANS (SFmode)) && (TARGET_HARD_FLOAT))
#define HAVE_absdf2 ((mips_abs == MIPS_IEEE_754_2008 || !HONOR_NANS (DFmode)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_absv2sf2 ((mips_abs == MIPS_IEEE_754_2008 || !HONOR_NANS (V2SFmode)) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_clzsi2 (ISA_HAS_CLZ_CLO)
#define HAVE_clzdi2 ((ISA_HAS_CLZ_CLO) && (TARGET_64BIT))
#define HAVE_popcountsi2 (ISA_HAS_POP)
#define HAVE_popcountdi2 ((ISA_HAS_POP) && (TARGET_64BIT))
#define HAVE_negsi2 1
#define HAVE_negdi2 (TARGET_64BIT && !TARGET_MIPS16)
#define HAVE_negsf2 ((mips_abs == MIPS_IEEE_754_2008 || !HONOR_NANS (SFmode)) && (TARGET_HARD_FLOAT))
#define HAVE_negdf2 ((mips_abs == MIPS_IEEE_754_2008 || !HONOR_NANS (DFmode)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_negv2sf2 ((mips_abs == MIPS_IEEE_754_2008 || !HONOR_NANS (V2SFmode)) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_one_cmplsi2 1
#define HAVE_one_cmpldi2 (TARGET_64BIT)
#define HAVE_truncdfsf2 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)
#define HAVE_truncdiqi2 (TARGET_64BIT)
#define HAVE_truncdihi2 (TARGET_64BIT)
#define HAVE_truncdisi2 (TARGET_64BIT)
#define HAVE_extendsidi2 (TARGET_64BIT)
#define HAVE_extendsfdf2 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)
#define HAVE_fix_truncdfsi2_insn (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT && ISA_HAS_TRUNC_W)
#define HAVE_fix_truncdfsi2_macro (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT && !ISA_HAS_TRUNC_W)
#define HAVE_fix_truncsfsi2_insn (TARGET_HARD_FLOAT && ISA_HAS_TRUNC_W)
#define HAVE_fix_truncsfsi2_macro (TARGET_HARD_FLOAT && !ISA_HAS_TRUNC_W)
#define HAVE_fix_truncdfdi2 (TARGET_HARD_FLOAT && TARGET_FLOAT64 && TARGET_DOUBLE_FLOAT)
#define HAVE_fix_truncsfdi2 (TARGET_HARD_FLOAT && TARGET_FLOAT64 && TARGET_DOUBLE_FLOAT)
#define HAVE_floatsidf2 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)
#define HAVE_floatdidf2 (TARGET_HARD_FLOAT && TARGET_FLOAT64 && TARGET_DOUBLE_FLOAT)
#define HAVE_floatsisf2 (TARGET_HARD_FLOAT)
#define HAVE_floatdisf2 (TARGET_HARD_FLOAT && TARGET_FLOAT64 && TARGET_DOUBLE_FLOAT)
#define HAVE_mov_lwl (!TARGET_MIPS16 && mips_mem_fits_mode_p (SImode, operands[1]))
#define HAVE_mov_ldl ((!TARGET_MIPS16 && mips_mem_fits_mode_p (DImode, operands[1])) && (TARGET_64BIT))
#define HAVE_mov_lwr (!TARGET_MIPS16 && mips_mem_fits_mode_p (SImode, operands[1]))
#define HAVE_mov_ldr ((!TARGET_MIPS16 && mips_mem_fits_mode_p (DImode, operands[1])) && (TARGET_64BIT))
#define HAVE_mov_swl (!TARGET_MIPS16 && mips_mem_fits_mode_p (SImode, operands[0]))
#define HAVE_mov_sdl ((!TARGET_MIPS16 && mips_mem_fits_mode_p (DImode, operands[0])) && (TARGET_64BIT))
#define HAVE_mov_swr (!TARGET_MIPS16 && mips_mem_fits_mode_p (SImode, operands[0]))
#define HAVE_mov_sdr ((!TARGET_MIPS16 && mips_mem_fits_mode_p (DImode, operands[0])) && (TARGET_64BIT))
#define HAVE_load_gotsi (Pmode == SImode)
#define HAVE_load_gotdi (Pmode == DImode)
#define HAVE_movccf (ISA_HAS_CCF)
#define HAVE_mfhisi_di (!TARGET_64BIT)
#define HAVE_mfhisi_ti (TARGET_64BIT)
#define HAVE_mfhidi_ti (TARGET_64BIT)
#define HAVE_mthisi_di (!TARGET_64BIT)
#define HAVE_mthisi_ti (TARGET_64BIT)
#define HAVE_mthidi_ti (TARGET_64BIT)
#define HAVE_load_lowdf ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_DOUBLE_FLOAT))
#define HAVE_load_lowdi ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_DOUBLE_FLOAT))
#define HAVE_load_lowv2sf ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_load_lowv2si ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_load_lowv4hi ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_load_lowv8qi ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_load_lowtf ((TARGET_HARD_FLOAT) && (TARGET_64BIT && TARGET_FLOAT64))
#define HAVE_load_highdf ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_DOUBLE_FLOAT))
#define HAVE_load_highdi ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_DOUBLE_FLOAT))
#define HAVE_load_highv2sf ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_load_highv2si ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_load_highv4hi ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_load_highv8qi ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_load_hightf ((TARGET_HARD_FLOAT) && (TARGET_64BIT && TARGET_FLOAT64))
#define HAVE_store_worddf ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_DOUBLE_FLOAT))
#define HAVE_store_worddi ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_DOUBLE_FLOAT))
#define HAVE_store_wordv2sf ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_store_wordv2si ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_store_wordv4hi ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_store_wordv8qi ((TARGET_HARD_FLOAT) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_store_wordtf ((TARGET_HARD_FLOAT) && (TARGET_64BIT && TARGET_FLOAT64))
#define HAVE_mthc1df ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_DOUBLE_FLOAT))
#define HAVE_mthc1di ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_DOUBLE_FLOAT))
#define HAVE_mthc1v2sf ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_mthc1v2si ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_mthc1v4hi ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_mthc1v8qi ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_mthc1tf ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (TARGET_64BIT && TARGET_FLOAT64))
#define HAVE_mfhc1df ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_DOUBLE_FLOAT))
#define HAVE_mfhc1di ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_DOUBLE_FLOAT))
#define HAVE_mfhc1v2sf ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_mfhc1v2si ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_mfhc1v4hi ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_mfhc1v8qi ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (!TARGET_64BIT && TARGET_LOONGSON_VECTORS))
#define HAVE_mfhc1tf ((TARGET_HARD_FLOAT && ISA_HAS_MXHC1) && (TARGET_64BIT && TARGET_FLOAT64))
#define HAVE_loadgp_newabi_si ((mips_current_loadgp_style () == LOADGP_NEWABI) && (Pmode == SImode))
#define HAVE_loadgp_newabi_di ((mips_current_loadgp_style () == LOADGP_NEWABI) && (Pmode == DImode))
#define HAVE_loadgp_absolute_si ((mips_current_loadgp_style () == LOADGP_ABSOLUTE) && (Pmode == SImode))
#define HAVE_loadgp_absolute_di ((mips_current_loadgp_style () == LOADGP_ABSOLUTE) && (Pmode == DImode))
#define HAVE_loadgp_blockage 1
#define HAVE_loadgp_rtp_si ((mips_current_loadgp_style () == LOADGP_RTP) && (Pmode == SImode))
#define HAVE_loadgp_rtp_di ((mips_current_loadgp_style () == LOADGP_RTP) && (Pmode == DImode))
#define HAVE_copygp_mips16_si ((TARGET_MIPS16) && (Pmode == SImode))
#define HAVE_copygp_mips16_di ((TARGET_MIPS16) && (Pmode == DImode))
#define HAVE_potential_cprestore_si ((!TARGET_CPRESTORE_DIRECTIVE || operands[2] == pic_offset_table_rtx) && (Pmode == SImode))
#define HAVE_potential_cprestore_di ((!TARGET_CPRESTORE_DIRECTIVE || operands[2] == pic_offset_table_rtx) && (Pmode == DImode))
#define HAVE_cprestore_si ((TARGET_CPRESTORE_DIRECTIVE) && (Pmode == SImode))
#define HAVE_cprestore_di ((TARGET_CPRESTORE_DIRECTIVE) && (Pmode == DImode))
#define HAVE_use_cprestore_si (Pmode == SImode)
#define HAVE_use_cprestore_di (Pmode == DImode)
#define HAVE_sync (GENERATE_SYNC)
#define HAVE_synci (TARGET_SYNCI)
#define HAVE_rdhwr_synci_step_si ((ISA_HAS_SYNCI) && (Pmode == SImode))
#define HAVE_rdhwr_synci_step_di ((ISA_HAS_SYNCI) && (Pmode == DImode))
#define HAVE_clear_hazard_si ((ISA_HAS_SYNCI) && (Pmode == SImode))
#define HAVE_clear_hazard_di ((ISA_HAS_SYNCI) && (Pmode == DImode))
#define HAVE_mips_cache (ISA_HAS_CACHE)
#define HAVE_r10k_cache_barrier (ISA_HAS_CACHE)
#define HAVE_lsa (ISA_HAS_LSA)
#define HAVE_dlsa ((ISA_HAS_DLSA) && (TARGET_64BIT))
#define HAVE_rotrsi3 (ISA_HAS_ROR)
#define HAVE_rotrdi3 ((ISA_HAS_ROR) && (TARGET_64BIT))
#define HAVE_bswaphi2 (ISA_HAS_WSBH)
#define HAVE_bswapsi2 (ISA_HAS_WSBH && ISA_HAS_ROR)
#define HAVE_bswapdi2 (TARGET_64BIT && ISA_HAS_WSBH)
#define HAVE_wsbh (ISA_HAS_WSBH)
#define HAVE_dsbh (TARGET_64BIT && ISA_HAS_WSBH)
#define HAVE_dshd (TARGET_64BIT && ISA_HAS_WSBH)
#define HAVE_sunordered_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_suneq_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sunlt_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sunle_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_seq_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_slt_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sle_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sunordered_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_suneq_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sunlt_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sunle_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_seq_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_slt_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sle_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sunordered_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_suneq_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sunlt_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sunle_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_seq_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_slt_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sle_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sordered_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sltgt_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sne_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sunordered_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_suneq_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sunlt_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sunle_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_seq_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_slt_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sle_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sordered_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sltgt_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sne_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sge_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sgt_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sunge_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sungt_sf_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sge_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sgt_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sunge_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sungt_df_using_cc (((!ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sge_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sgt_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sunge_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sungt_sf_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT)) && (TARGET_HARD_FLOAT))
#define HAVE_sge_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sgt_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sunge_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_sungt_df_using_ccf (((ISA_HAS_CCF) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_indirect_jump_si (Pmode == SImode)
#define HAVE_indirect_jump_di (Pmode == DImode)
#define HAVE_indirect_jump_and_restore_si (Pmode == SImode)
#define HAVE_indirect_jump_and_restore_di (Pmode == DImode)
#define HAVE_tablejump_si (Pmode == SImode)
#define HAVE_tablejump_di (Pmode == DImode)
#define HAVE_casesi_internal_mips16_si ((TARGET_MIPS16_SHORT_JUMP_TABLES) && (Pmode == SImode))
#define HAVE_casesi_internal_mips16_di ((TARGET_MIPS16_SHORT_JUMP_TABLES) && (Pmode == DImode))
#define HAVE_blockage 1
#define HAVE_probe_stack_range_si (Pmode == SImode)
#define HAVE_probe_stack_range_di (Pmode == DImode)
#define HAVE_return_internal 1
#define HAVE_simple_return_internal 1
#define HAVE_mips_eret 1
#define HAVE_mips_deret 1
#define HAVE_mips_di 1
#define HAVE_mips_ehb 1
#define HAVE_mips_rdpgpr_si (Pmode == SImode)
#define HAVE_mips_rdpgpr_di (Pmode == DImode)
#define HAVE_cop0_move 1
#define HAVE_eh_set_lr_si (! TARGET_64BIT)
#define HAVE_eh_set_lr_di (TARGET_64BIT)
#define HAVE_restore_gp_si ((TARGET_CALL_CLOBBERED_GP) && (Pmode == SImode))
#define HAVE_restore_gp_di ((TARGET_CALL_CLOBBERED_GP) && (Pmode == DImode))
#define HAVE_move_gpsi 1
#define HAVE_move_gpdi (TARGET_64BIT)
#define HAVE_load_callsi ((TARGET_USE_GOT) && (Pmode == SImode))
#define HAVE_load_calldi ((TARGET_USE_GOT) && (Pmode == DImode))
#define HAVE_set_got_version (TARGET_USE_GOT)
#define HAVE_update_got_version (TARGET_USE_GOT)
#define HAVE_sibcall_internal (TARGET_SIBCALLS && SIBLING_CALL_P (insn))
#define HAVE_sibcall_value_internal (TARGET_SIBCALLS && SIBLING_CALL_P (insn))
#define HAVE_sibcall_value_multiple_internal (TARGET_SIBCALLS && SIBLING_CALL_P (insn))
#define HAVE_call_internal 1
#define HAVE_call_split (TARGET_SPLIT_CALLS)
#define HAVE_call_internal_direct 1
#define HAVE_call_direct_split (TARGET_SPLIT_CALLS)
#define HAVE_call_value_internal 1
#define HAVE_call_value_split (TARGET_SPLIT_CALLS)
#define HAVE_call_value_internal_direct 1
#define HAVE_call_value_direct_split (TARGET_SPLIT_CALLS)
#define HAVE_call_value_multiple_internal 1
#define HAVE_call_value_multiple_split (TARGET_SPLIT_CALLS)
#define HAVE_prefetch (ISA_HAS_PREFETCH && TARGET_EXPLICIT_RELOCS)
#define HAVE_nop 1
#define HAVE_hazard_nop 1
#define HAVE_insn_pseudo 1
#define HAVE_consttable 1
#define HAVE_consttable_end 1
#define HAVE_consttable_tls_reloc (TARGET_MIPS16_PCREL_LOADS)
#define HAVE_consttable_int (TARGET_MIPS16)
#define HAVE_consttable_float (TARGET_MIPS16)
#define HAVE_align 1
#define HAVE_tls_get_tp_si ((HAVE_AS_TLS && !TARGET_MIPS16) && (Pmode == SImode))
#define HAVE_tls_get_tp_di ((HAVE_AS_TLS && !TARGET_MIPS16) && (Pmode == DImode))
#define HAVE_tls_get_tp_mips16_si ((HAVE_AS_TLS && TARGET_MIPS16) && (Pmode == SImode))
#define HAVE_tls_get_tp_mips16_di ((HAVE_AS_TLS && TARGET_MIPS16) && (Pmode == DImode))
#define HAVE_mips_get_fcsr_mips16_si ((TARGET_HARD_FLOAT_ABI && TARGET_MIPS16) && (Pmode == SImode))
#define HAVE_mips_get_fcsr_mips16_di ((TARGET_HARD_FLOAT_ABI && TARGET_MIPS16) && (Pmode == DImode))
#define HAVE_mips_set_fcsr_mips16_si ((TARGET_HARD_FLOAT_ABI && TARGET_MIPS16) && (Pmode == SImode))
#define HAVE_mips_set_fcsr_mips16_di ((TARGET_HARD_FLOAT_ABI && TARGET_MIPS16) && (Pmode == DImode))
#define HAVE_sync_compare_and_swapsi (GENERATE_LL_SC)
#define HAVE_sync_compare_and_swapdi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_compare_and_swap_12 (GENERATE_LL_SC)
#define HAVE_sync_addsi (GENERATE_LL_SC)
#define HAVE_sync_adddi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_add_12 (GENERATE_LL_SC)
#define HAVE_sync_sub_12 (GENERATE_LL_SC)
#define HAVE_sync_ior_12 (GENERATE_LL_SC)
#define HAVE_sync_xor_12 (GENERATE_LL_SC)
#define HAVE_sync_and_12 (GENERATE_LL_SC)
#define HAVE_sync_old_add_12 (GENERATE_LL_SC)
#define HAVE_sync_old_sub_12 (GENERATE_LL_SC)
#define HAVE_sync_old_ior_12 (GENERATE_LL_SC)
#define HAVE_sync_old_xor_12 (GENERATE_LL_SC)
#define HAVE_sync_old_and_12 (GENERATE_LL_SC)
#define HAVE_sync_new_add_12 (GENERATE_LL_SC)
#define HAVE_sync_new_sub_12 (GENERATE_LL_SC)
#define HAVE_sync_new_ior_12 (GENERATE_LL_SC)
#define HAVE_sync_new_xor_12 (GENERATE_LL_SC)
#define HAVE_sync_new_and_12 (GENERATE_LL_SC)
#define HAVE_sync_nand_12 (GENERATE_LL_SC)
#define HAVE_sync_old_nand_12 (GENERATE_LL_SC)
#define HAVE_sync_new_nand_12 (GENERATE_LL_SC)
#define HAVE_sync_subsi (GENERATE_LL_SC)
#define HAVE_sync_subdi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_old_addsi (GENERATE_LL_SC)
#define HAVE_sync_old_adddi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_old_subsi (GENERATE_LL_SC)
#define HAVE_sync_old_subdi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_new_addsi (GENERATE_LL_SC)
#define HAVE_sync_new_adddi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_new_subsi (GENERATE_LL_SC)
#define HAVE_sync_new_subdi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_iorsi (GENERATE_LL_SC)
#define HAVE_sync_xorsi (GENERATE_LL_SC)
#define HAVE_sync_andsi (GENERATE_LL_SC)
#define HAVE_sync_iordi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_xordi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_anddi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_old_iorsi (GENERATE_LL_SC)
#define HAVE_sync_old_xorsi (GENERATE_LL_SC)
#define HAVE_sync_old_andsi (GENERATE_LL_SC)
#define HAVE_sync_old_iordi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_old_xordi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_old_anddi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_new_iorsi (GENERATE_LL_SC)
#define HAVE_sync_new_xorsi (GENERATE_LL_SC)
#define HAVE_sync_new_andsi (GENERATE_LL_SC)
#define HAVE_sync_new_iordi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_new_xordi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_new_anddi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_nandsi (GENERATE_LL_SC)
#define HAVE_sync_nanddi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_old_nandsi (GENERATE_LL_SC)
#define HAVE_sync_old_nanddi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_new_nandsi (GENERATE_LL_SC)
#define HAVE_sync_new_nanddi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_sync_lock_test_and_setsi (GENERATE_LL_SC)
#define HAVE_sync_lock_test_and_setdi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_test_and_set_12 (GENERATE_LL_SC)
#define HAVE_atomic_compare_and_swapsi (GENERATE_LL_SC)
#define HAVE_atomic_compare_and_swapdi ((GENERATE_LL_SC) && (TARGET_64BIT))
#define HAVE_atomic_exchangesi_llsc (GENERATE_LL_SC && !ISA_HAS_SWAP)
#define HAVE_atomic_exchangedi_llsc ((GENERATE_LL_SC && !ISA_HAS_SWAP) && (TARGET_64BIT))
#define HAVE_atomic_exchangesi_swap (ISA_HAS_SWAP)
#define HAVE_atomic_exchangedi_swap ((ISA_HAS_SWAP) && (TARGET_64BIT))
#define HAVE_atomic_fetch_addsi_llsc (GENERATE_LL_SC && !ISA_HAS_LDADD)
#define HAVE_atomic_fetch_adddi_llsc ((GENERATE_LL_SC && !ISA_HAS_LDADD) && (TARGET_64BIT))
#define HAVE_atomic_fetch_addsi_ldadd (ISA_HAS_LDADD)
#define HAVE_atomic_fetch_adddi_ldadd ((ISA_HAS_LDADD) && (TARGET_64BIT))
#define HAVE_mips_cond_move_tf_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_vec_perm_const_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_vec_concatv2sf (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_vec_extractv2sfsf (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_alnv_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_addr_ps (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_mips_cvt_pw_ps (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_mips_cvt_ps_pw (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_mips_mulr_ps (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_mips_cabs_cond_s ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT))
#define HAVE_mips_cabs_cond_d ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_mips_c_cond_4s (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_cabs_cond_4s (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_mips_c_cond_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_cabs_cond_ps (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_sunordered_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_suneq_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_sunlt_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_sunle_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_seq_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_slt_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_sle_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_sordered_ps ((TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT) && (ISA_HAS_CCF))
#define HAVE_sltgt_ps ((TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT) && (ISA_HAS_CCF))
#define HAVE_sne_ps ((TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT) && (ISA_HAS_CCF))
#define HAVE_sge_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_sgt_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_sunge_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_sungt_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_bc1any4t (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_bc1any4f (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_bc1any2t (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_bc1any2f (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_mips_rsqrt1_s ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT))
#define HAVE_mips_rsqrt1_d ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_mips_rsqrt1_ps ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_mips_rsqrt2_s ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT))
#define HAVE_mips_rsqrt2_d ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_mips_rsqrt2_ps ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_mips_recip1_s ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT))
#define HAVE_mips_recip1_d ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_mips_recip1_ps ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_mips_recip2_s ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT))
#define HAVE_mips_recip2_d ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_mips_recip2_ps ((TARGET_HARD_FLOAT && TARGET_MIPS3D) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_addv2hi3 (ISA_HAS_DSP)
#define HAVE_addv4qi3 (ISA_HAS_DSP)
#define HAVE_mips_addq_s_w (ISA_HAS_DSP)
#define HAVE_mips_addq_s_ph (ISA_HAS_DSP)
#define HAVE_mips_addu_s_qb (ISA_HAS_DSP)
#define HAVE_subv2hi3 (ISA_HAS_DSP)
#define HAVE_subv4qi3 (ISA_HAS_DSP)
#define HAVE_mips_subq_s_w (ISA_HAS_DSP)
#define HAVE_mips_subq_s_ph (ISA_HAS_DSP)
#define HAVE_mips_subu_s_qb (ISA_HAS_DSP)
#define HAVE_mips_addsc (ISA_HAS_DSP)
#define HAVE_mips_addwc (ISA_HAS_DSP)
#define HAVE_mips_modsub (ISA_HAS_DSP)
#define HAVE_mips_raddu_w_qb (ISA_HAS_DSP)
#define HAVE_mips_absq_s_w (ISA_HAS_DSP)
#define HAVE_mips_absq_s_ph (ISA_HAS_DSP)
#define HAVE_mips_precrq_qb_ph (ISA_HAS_DSP)
#define HAVE_mips_precrq_ph_w (ISA_HAS_DSP)
#define HAVE_mips_precrq_rs_ph_w (ISA_HAS_DSP)
#define HAVE_mips_precrqu_s_qb_ph (ISA_HAS_DSP)
#define HAVE_mips_preceq_w_phl (ISA_HAS_DSP)
#define HAVE_mips_preceq_w_phr (ISA_HAS_DSP)
#define HAVE_mips_precequ_ph_qbl (ISA_HAS_DSP)
#define HAVE_mips_precequ_ph_qbr (ISA_HAS_DSP)
#define HAVE_mips_precequ_ph_qbla (ISA_HAS_DSP)
#define HAVE_mips_precequ_ph_qbra (ISA_HAS_DSP)
#define HAVE_mips_preceu_ph_qbl (ISA_HAS_DSP)
#define HAVE_mips_preceu_ph_qbr (ISA_HAS_DSP)
#define HAVE_mips_preceu_ph_qbla (ISA_HAS_DSP)
#define HAVE_mips_preceu_ph_qbra (ISA_HAS_DSP)
#define HAVE_mips_shll_ph (ISA_HAS_DSP)
#define HAVE_mips_shll_qb (ISA_HAS_DSP)
#define HAVE_mips_shll_s_w (ISA_HAS_DSP)
#define HAVE_mips_shll_s_ph (ISA_HAS_DSP)
#define HAVE_mips_shrl_qb (ISA_HAS_DSP)
#define HAVE_mips_shra_ph (ISA_HAS_DSP)
#define HAVE_mips_shra_r_w (ISA_HAS_DSP)
#define HAVE_mips_shra_r_ph (ISA_HAS_DSP)
#define HAVE_mips_muleu_s_ph_qbl (ISA_HAS_DSP)
#define HAVE_mips_muleu_s_ph_qbr (ISA_HAS_DSP)
#define HAVE_mips_mulq_rs_ph (ISA_HAS_DSP)
#define HAVE_mips_muleq_s_w_phl (ISA_HAS_DSP)
#define HAVE_mips_muleq_s_w_phr (ISA_HAS_DSP)
#define HAVE_mips_dpau_h_qbl (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_dpau_h_qbr (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_dpsu_h_qbl (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_dpsu_h_qbr (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_dpaq_s_w_ph (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_dpsq_s_w_ph (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_mulsaq_s_w_ph (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_dpaq_sa_l_w (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_dpsq_sa_l_w (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_maq_s_w_phl (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_maq_s_w_phr (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_maq_sa_w_phl (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_maq_sa_w_phr (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_bitrev (ISA_HAS_DSP)
#define HAVE_mips_insv (ISA_HAS_DSP)
#define HAVE_mips_repl_qb (ISA_HAS_DSP)
#define HAVE_mips_repl_ph (ISA_HAS_DSP)
#define HAVE_mips_cmp_eq_ph (ISA_HAS_DSP)
#define HAVE_mips_cmpu_eq_qb (ISA_HAS_DSP)
#define HAVE_mips_cmp_lt_ph (ISA_HAS_DSP)
#define HAVE_mips_cmpu_lt_qb (ISA_HAS_DSP)
#define HAVE_mips_cmp_le_ph (ISA_HAS_DSP)
#define HAVE_mips_cmpu_le_qb (ISA_HAS_DSP)
#define HAVE_mips_cmpgu_eq_qb (ISA_HAS_DSP)
#define HAVE_mips_cmpgu_lt_qb (ISA_HAS_DSP)
#define HAVE_mips_cmpgu_le_qb (ISA_HAS_DSP)
#define HAVE_mips_pick_ph (ISA_HAS_DSP)
#define HAVE_mips_pick_qb (ISA_HAS_DSP)
#define HAVE_mips_packrl_ph (ISA_HAS_DSP)
#define HAVE_mips_extr_w (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_extr_r_w (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_extr_rs_w (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_extr_s_h (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_extp (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_extpdp (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_shilo (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_mthlip (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_wrdsp (ISA_HAS_DSP)
#define HAVE_mips_rddsp (ISA_HAS_DSP)
#define HAVE_mips_lbx_extsi_si ((ISA_HAS_LBX) && (Pmode == SImode))
#define HAVE_mips_lbux_extsi_si ((ISA_HAS_LBUX) && (Pmode == SImode))
#define HAVE_mips_lbx_extdi_si ((ISA_HAS_LBX) && ((((TARGET_64BIT) && (Pmode == SImode)) && (Pmode == SImode)) && (Pmode == SImode)))
#define HAVE_mips_lbux_extdi_si ((ISA_HAS_LBUX) && ((((TARGET_64BIT) && (Pmode == SImode)) && (Pmode == SImode)) && (Pmode == SImode)))
#define HAVE_mips_lhx_extsi_si ((ISA_HAS_LHX) && (Pmode == SImode))
#define HAVE_mips_lhux_extsi_si ((ISA_HAS_LHUX) && (Pmode == SImode))
#define HAVE_mips_lhx_extdi_si ((ISA_HAS_LHX) && ((((TARGET_64BIT) && (Pmode == SImode)) && (Pmode == SImode)) && (Pmode == SImode)))
#define HAVE_mips_lhux_extdi_si ((ISA_HAS_LHUX) && ((((TARGET_64BIT) && (Pmode == SImode)) && (Pmode == SImode)) && (Pmode == SImode)))
#define HAVE_mips_lbx_extsi_di ((ISA_HAS_LBX) && (Pmode == DImode))
#define HAVE_mips_lbux_extsi_di ((ISA_HAS_LBUX) && (Pmode == DImode))
#define HAVE_mips_lbx_extdi_di ((ISA_HAS_LBX) && ((((TARGET_64BIT) && (Pmode == DImode)) && (Pmode == DImode)) && (Pmode == DImode)))
#define HAVE_mips_lbux_extdi_di ((ISA_HAS_LBUX) && ((((TARGET_64BIT) && (Pmode == DImode)) && (Pmode == DImode)) && (Pmode == DImode)))
#define HAVE_mips_lhx_extsi_di ((ISA_HAS_LHX) && (Pmode == DImode))
#define HAVE_mips_lhux_extsi_di ((ISA_HAS_LHUX) && (Pmode == DImode))
#define HAVE_mips_lhx_extdi_di ((ISA_HAS_LHX) && ((((TARGET_64BIT) && (Pmode == DImode)) && (Pmode == DImode)) && (Pmode == DImode)))
#define HAVE_mips_lhux_extdi_di ((ISA_HAS_LHUX) && ((((TARGET_64BIT) && (Pmode == DImode)) && (Pmode == DImode)) && (Pmode == DImode)))
#define HAVE_mips_lwx_si ((ISA_HAS_LWX) && (Pmode == SImode))
#define HAVE_mips_ldx_si ((ISA_HAS_LDX) && ((((TARGET_64BIT) && (Pmode == SImode)) && (Pmode == SImode)) && (Pmode == SImode)))
#define HAVE_mips_lwx_di ((ISA_HAS_LWX) && (Pmode == DImode))
#define HAVE_mips_ldx_di ((ISA_HAS_LDX) && ((((TARGET_64BIT) && (Pmode == DImode)) && (Pmode == DImode)) && (Pmode == DImode)))
#define HAVE_mips_bposge (ISA_HAS_DSP)
#define HAVE_mips_absq_s_qb (ISA_HAS_DSPR2)
#define HAVE_mips_addu_ph (ISA_HAS_DSPR2)
#define HAVE_mips_addu_s_ph (ISA_HAS_DSPR2)
#define HAVE_mips_adduh_qb (ISA_HAS_DSPR2)
#define HAVE_mips_adduh_r_qb (ISA_HAS_DSPR2)
#define HAVE_mips_append (ISA_HAS_DSPR2)
#define HAVE_mips_balign (ISA_HAS_DSPR2)
#define HAVE_mips_cmpgdu_eq_qb (ISA_HAS_DSPR2)
#define HAVE_mips_cmpgdu_lt_qb (ISA_HAS_DSPR2)
#define HAVE_mips_cmpgdu_le_qb (ISA_HAS_DSPR2)
#define HAVE_mips_dpa_w_ph (ISA_HAS_DSPR2 && !TARGET_64BIT)
#define HAVE_mips_dps_w_ph (ISA_HAS_DSPR2 && !TARGET_64BIT)
#define HAVE_mulv2hi3 (ISA_HAS_DSPR2)
#define HAVE_mips_mul_s_ph (ISA_HAS_DSPR2)
#define HAVE_mips_mulq_rs_w (ISA_HAS_DSPR2)
#define HAVE_mips_mulq_s_ph (ISA_HAS_DSPR2)
#define HAVE_mips_mulq_s_w (ISA_HAS_DSPR2)
#define HAVE_mips_mulsa_w_ph (ISA_HAS_DSPR2 && !TARGET_64BIT)
#define HAVE_mips_precr_qb_ph (ISA_HAS_DSPR2)
#define HAVE_mips_precr_sra_ph_w (ISA_HAS_DSPR2)
#define HAVE_mips_precr_sra_r_ph_w (ISA_HAS_DSPR2)
#define HAVE_mips_prepend (ISA_HAS_DSPR2)
#define HAVE_mips_shra_qb (ISA_HAS_DSPR2)
#define HAVE_mips_shra_r_qb (ISA_HAS_DSPR2)
#define HAVE_mips_shrl_ph (ISA_HAS_DSPR2)
#define HAVE_mips_subu_ph (ISA_HAS_DSPR2)
#define HAVE_mips_subu_s_ph (ISA_HAS_DSPR2)
#define HAVE_mips_subuh_qb (ISA_HAS_DSPR2)
#define HAVE_mips_subuh_r_qb (ISA_HAS_DSPR2)
#define HAVE_mips_addqh_ph (ISA_HAS_DSPR2)
#define HAVE_mips_addqh_r_ph (ISA_HAS_DSPR2)
#define HAVE_mips_addqh_w (ISA_HAS_DSPR2)
#define HAVE_mips_addqh_r_w (ISA_HAS_DSPR2)
#define HAVE_mips_subqh_ph (ISA_HAS_DSPR2)
#define HAVE_mips_subqh_r_ph (ISA_HAS_DSPR2)
#define HAVE_mips_subqh_w (ISA_HAS_DSPR2)
#define HAVE_mips_subqh_r_w (ISA_HAS_DSPR2)
#define HAVE_mips_dpax_w_ph (ISA_HAS_DSPR2 && !TARGET_64BIT)
#define HAVE_mips_dpsx_w_ph (ISA_HAS_DSPR2 && !TARGET_64BIT)
#define HAVE_mips_dpaqx_s_w_ph (ISA_HAS_DSPR2 && !TARGET_64BIT)
#define HAVE_mips_dpaqx_sa_w_ph (ISA_HAS_DSPR2 && !TARGET_64BIT)
#define HAVE_mips_dpsqx_s_w_ph (ISA_HAS_DSPR2 && !TARGET_64BIT)
#define HAVE_mips_dpsqx_sa_w_ph (ISA_HAS_DSPR2 && !TARGET_64BIT)
#define HAVE_addqq3 1
#define HAVE_addhq3 1
#define HAVE_addsq3 1
#define HAVE_adddq3 (TARGET_64BIT)
#define HAVE_adduqq3 1
#define HAVE_adduhq3 1
#define HAVE_addusq3 1
#define HAVE_addudq3 (TARGET_64BIT)
#define HAVE_addha3 1
#define HAVE_addsa3 1
#define HAVE_addda3 (TARGET_64BIT)
#define HAVE_adduha3 1
#define HAVE_addusa3 1
#define HAVE_adduda3 (TARGET_64BIT)
#define HAVE_usadduqq3 (ISA_HAS_DSP)
#define HAVE_usadduhq3 (ISA_HAS_DSPR2)
#define HAVE_usadduha3 (ISA_HAS_DSPR2)
#define HAVE_usaddv4uqq3 (ISA_HAS_DSP)
#define HAVE_usaddv2uhq3 (ISA_HAS_DSPR2)
#define HAVE_usaddv2uha3 (ISA_HAS_DSPR2)
#define HAVE_ssaddhq3 (ISA_HAS_DSP)
#define HAVE_ssaddsq3 (ISA_HAS_DSP)
#define HAVE_ssaddha3 (ISA_HAS_DSP)
#define HAVE_ssaddsa3 (ISA_HAS_DSP)
#define HAVE_ssaddv2hq3 (ISA_HAS_DSP)
#define HAVE_ssaddv2ha3 (ISA_HAS_DSP)
#define HAVE_subqq3 1
#define HAVE_subhq3 1
#define HAVE_subsq3 1
#define HAVE_subdq3 (TARGET_64BIT)
#define HAVE_subuqq3 1
#define HAVE_subuhq3 1
#define HAVE_subusq3 1
#define HAVE_subudq3 (TARGET_64BIT)
#define HAVE_subha3 1
#define HAVE_subsa3 1
#define HAVE_subda3 (TARGET_64BIT)
#define HAVE_subuha3 1
#define HAVE_subusa3 1
#define HAVE_subuda3 (TARGET_64BIT)
#define HAVE_ussubuqq3 (ISA_HAS_DSP)
#define HAVE_ussubuhq3 (ISA_HAS_DSPR2)
#define HAVE_ussubuha3 (ISA_HAS_DSPR2)
#define HAVE_ussubv4uqq3 (ISA_HAS_DSP)
#define HAVE_ussubv2uhq3 (ISA_HAS_DSPR2)
#define HAVE_ussubv2uha3 (ISA_HAS_DSPR2)
#define HAVE_sssubhq3 (ISA_HAS_DSP)
#define HAVE_sssubsq3 (ISA_HAS_DSP)
#define HAVE_sssubha3 (ISA_HAS_DSP)
#define HAVE_sssubsa3 (ISA_HAS_DSP)
#define HAVE_sssubv2hq3 (ISA_HAS_DSP)
#define HAVE_sssubv2ha3 (ISA_HAS_DSP)
#define HAVE_ssmulv2hq3 (ISA_HAS_DSP)
#define HAVE_ssmulhq3 (ISA_HAS_DSP)
#define HAVE_ssmulsq3 (ISA_HAS_DSPR2)
#define HAVE_ssmaddsqdq4 (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_ssmsubsqdq4 (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_jraddiusp (TARGET_MICROMIPS)
#define HAVE_movv2si_internal (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_movv4hi_internal (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_movv8qi_internal (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_vec_init1_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_vec_init1_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_pack_ssat_v2si (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_pack_ssat_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_pack_usat_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_addv2si3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_addv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_addv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_paddd (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_ssaddv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_ssaddv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_usaddv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_usaddv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pandn_w (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pandn_h (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pandn_b (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pandn_d (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_andv2si3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_andv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_andv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_iorv2si3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_iorv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_iorv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_xorv2si3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_xorv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_xorv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_one_cmplv2si2 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_one_cmplv4hi2 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_one_cmplv8qi2 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pavgh (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pavgb (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pcmpeqw (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pcmpeqh (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pcmpeqb (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pcmpgtw (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pcmpgth (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pcmpgtb (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pextrh (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pinsrh_0 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pinsrh_1 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pinsrh_2 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pinsrh_3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pmaddhw (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_smaxv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_umaxv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_sminv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_uminv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pmovmskb (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_umulv4hi3_highpart (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_smulv4hi3_highpart (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_mulv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pmuluw (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pasubub (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_biadd (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_uplus_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_psadbh (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_pshufh (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_ashlv2si3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_ashlv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_ashrv2si3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_ashrv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_lshrv2si3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_lshrv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_subv2si3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_subv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_subv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_psubd (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_sssubv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_sssubv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_ussubv4hi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_ussubv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_punpckhbh (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_punpckhhw (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_punpckhhw_qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_punpckhwd (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_punpckhwd_qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_punpckhwd_hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_punpcklbh (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_punpcklhw (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_loongson_punpcklwd (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_shl_v2si (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_shl_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_shl_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_shl_di (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_shr_v2si (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_shr_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_shr_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_shr_di (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_loongson_extract_lo_v2si (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_loongson_extract_lo_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_loongson_extract_lo_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_pack_trunc_v2di (ISA_HAS_MSA)
#define HAVE_vec_pack_trunc_v4si (ISA_HAS_MSA)
#define HAVE_vec_pack_trunc_v8hi (ISA_HAS_MSA)
#define HAVE_msa_vec_extract_d_f (ISA_HAS_MSA)
#define HAVE_msa_vec_extract_w_f (ISA_HAS_MSA)
#define HAVE_msa_insert_d_f (ISA_HAS_MSA)
#define HAVE_msa_insert_w_f (ISA_HAS_MSA)
#define HAVE_msa_insert_d (ISA_HAS_MSA)
#define HAVE_msa_insert_w (ISA_HAS_MSA)
#define HAVE_msa_insert_h (ISA_HAS_MSA)
#define HAVE_msa_insert_b (ISA_HAS_MSA)
#define HAVE_msa_insve_d_f (ISA_HAS_MSA)
#define HAVE_msa_insve_w_f (ISA_HAS_MSA)
#define HAVE_msa_insve_d (ISA_HAS_MSA)
#define HAVE_msa_insve_w (ISA_HAS_MSA)
#define HAVE_msa_insve_h (ISA_HAS_MSA)
#define HAVE_msa_insve_b (ISA_HAS_MSA)
#define HAVE_msa_insve_d_f_scalar (ISA_HAS_MSA)
#define HAVE_msa_insve_w_f_scalar (ISA_HAS_MSA)
#define HAVE_msa_copy_s_h (ISA_HAS_MSA)
#define HAVE_msa_copy_u_h (ISA_HAS_MSA)
#define HAVE_msa_copy_s_b (ISA_HAS_MSA)
#define HAVE_msa_copy_u_b (ISA_HAS_MSA)
#define HAVE_msa_copy_u_w (ISA_HAS_MSA && TARGET_64BIT)
#define HAVE_msa_copy_s_w_64bit (ISA_HAS_MSA && TARGET_64BIT)
#define HAVE_msa_copy_s_w_f_64bit (ISA_HAS_MSA && TARGET_64BIT)
#define HAVE_msa_copy_s_w (ISA_HAS_MSA)
#define HAVE_msa_copy_s_w_f (ISA_HAS_MSA)
#define HAVE_msa_copy_s_d (ISA_HAS_MSA)
#define HAVE_msa_copy_s_d_f (ISA_HAS_MSA)
#define HAVE_vec_permv2df (ISA_HAS_MSA)
#define HAVE_vec_permv4sf (ISA_HAS_MSA)
#define HAVE_vec_permv2di (ISA_HAS_MSA)
#define HAVE_vec_permv4si (ISA_HAS_MSA)
#define HAVE_vec_permv8hi (ISA_HAS_MSA)
#define HAVE_vec_permv16qi (ISA_HAS_MSA)
#define HAVE_movv2df_msa (ISA_HAS_MSA)
#define HAVE_movv4sf_msa (ISA_HAS_MSA)
#define HAVE_movv2di_msa (ISA_HAS_MSA)
#define HAVE_movv4si_msa (ISA_HAS_MSA)
#define HAVE_movv8hi_msa (ISA_HAS_MSA)
#define HAVE_movv16qi_msa (ISA_HAS_MSA)
#define HAVE_addv2di3 (ISA_HAS_MSA)
#define HAVE_addv4si3 (ISA_HAS_MSA)
#define HAVE_addv8hi3 (ISA_HAS_MSA)
#define HAVE_addv16qi3 (ISA_HAS_MSA)
#define HAVE_subv2di3 (ISA_HAS_MSA)
#define HAVE_subv4si3 (ISA_HAS_MSA)
#define HAVE_subv8hi3 (ISA_HAS_MSA)
#define HAVE_subv16qi3 (ISA_HAS_MSA)
#define HAVE_mulv2di3 (ISA_HAS_MSA)
#define HAVE_mulv4si3 (ISA_HAS_MSA)
#define HAVE_mulv8hi3 (ISA_HAS_MSA)
#define HAVE_mulv16qi3 (ISA_HAS_MSA)
#define HAVE_msa_maddv_d (ISA_HAS_MSA)
#define HAVE_msa_maddv_w (ISA_HAS_MSA)
#define HAVE_msa_maddv_h (ISA_HAS_MSA)
#define HAVE_msa_maddv_b (ISA_HAS_MSA)
#define HAVE_msa_msubv_d (ISA_HAS_MSA)
#define HAVE_msa_msubv_w (ISA_HAS_MSA)
#define HAVE_msa_msubv_h (ISA_HAS_MSA)
#define HAVE_msa_msubv_b (ISA_HAS_MSA)
#define HAVE_divv2di3 (ISA_HAS_MSA)
#define HAVE_divv4si3 (ISA_HAS_MSA)
#define HAVE_divv8hi3 (ISA_HAS_MSA)
#define HAVE_divv16qi3 (ISA_HAS_MSA)
#define HAVE_udivv2di3 (ISA_HAS_MSA)
#define HAVE_udivv4si3 (ISA_HAS_MSA)
#define HAVE_udivv8hi3 (ISA_HAS_MSA)
#define HAVE_udivv16qi3 (ISA_HAS_MSA)
#define HAVE_modv2di3 (ISA_HAS_MSA)
#define HAVE_modv4si3 (ISA_HAS_MSA)
#define HAVE_modv8hi3 (ISA_HAS_MSA)
#define HAVE_modv16qi3 (ISA_HAS_MSA)
#define HAVE_umodv2di3 (ISA_HAS_MSA)
#define HAVE_umodv4si3 (ISA_HAS_MSA)
#define HAVE_umodv8hi3 (ISA_HAS_MSA)
#define HAVE_umodv16qi3 (ISA_HAS_MSA)
#define HAVE_xorv2di3 (ISA_HAS_MSA)
#define HAVE_xorv4si3 (ISA_HAS_MSA)
#define HAVE_xorv8hi3 (ISA_HAS_MSA)
#define HAVE_xorv16qi3 (ISA_HAS_MSA)
#define HAVE_iorv2di3 (ISA_HAS_MSA)
#define HAVE_iorv4si3 (ISA_HAS_MSA)
#define HAVE_iorv8hi3 (ISA_HAS_MSA)
#define HAVE_iorv16qi3 (ISA_HAS_MSA)
#define HAVE_andv2di3 (ISA_HAS_MSA)
#define HAVE_andv4si3 (ISA_HAS_MSA)
#define HAVE_andv8hi3 (ISA_HAS_MSA)
#define HAVE_andv16qi3 (ISA_HAS_MSA)
#define HAVE_one_cmplv2di2 (ISA_HAS_MSA)
#define HAVE_one_cmplv4si2 (ISA_HAS_MSA)
#define HAVE_one_cmplv8hi2 (ISA_HAS_MSA)
#define HAVE_one_cmplv16qi2 (ISA_HAS_MSA)
#define HAVE_vlshrv2di3 (ISA_HAS_MSA)
#define HAVE_vlshrv4si3 (ISA_HAS_MSA)
#define HAVE_vlshrv8hi3 (ISA_HAS_MSA)
#define HAVE_vlshrv16qi3 (ISA_HAS_MSA)
#define HAVE_vashrv2di3 (ISA_HAS_MSA)
#define HAVE_vashrv4si3 (ISA_HAS_MSA)
#define HAVE_vashrv8hi3 (ISA_HAS_MSA)
#define HAVE_vashrv16qi3 (ISA_HAS_MSA)
#define HAVE_vashlv2di3 (ISA_HAS_MSA)
#define HAVE_vashlv4si3 (ISA_HAS_MSA)
#define HAVE_vashlv8hi3 (ISA_HAS_MSA)
#define HAVE_vashlv16qi3 (ISA_HAS_MSA)
#define HAVE_addv2df3 (ISA_HAS_MSA)
#define HAVE_addv4sf3 (ISA_HAS_MSA)
#define HAVE_subv2df3 (ISA_HAS_MSA)
#define HAVE_subv4sf3 (ISA_HAS_MSA)
#define HAVE_mulv2df3 (ISA_HAS_MSA)
#define HAVE_mulv4sf3 (ISA_HAS_MSA)
#define HAVE_divv2df3 (ISA_HAS_MSA)
#define HAVE_divv4sf3 (ISA_HAS_MSA)
#define HAVE_fmav2df4 (ISA_HAS_MSA)
#define HAVE_fmav4sf4 (ISA_HAS_MSA)
#define HAVE_fnmav2df4 (ISA_HAS_MSA)
#define HAVE_fnmav4sf4 (ISA_HAS_MSA)
#define HAVE_sqrtv2df2 (ISA_HAS_MSA)
#define HAVE_sqrtv4sf2 (ISA_HAS_MSA)
#define HAVE_msa_add_a_d (ISA_HAS_MSA)
#define HAVE_msa_add_a_w (ISA_HAS_MSA)
#define HAVE_msa_add_a_h (ISA_HAS_MSA)
#define HAVE_msa_add_a_b (ISA_HAS_MSA)
#define HAVE_msa_adds_a_d (ISA_HAS_MSA)
#define HAVE_msa_adds_a_w (ISA_HAS_MSA)
#define HAVE_msa_adds_a_h (ISA_HAS_MSA)
#define HAVE_msa_adds_a_b (ISA_HAS_MSA)
#define HAVE_ssaddv2di3 (ISA_HAS_MSA)
#define HAVE_ssaddv4si3 (ISA_HAS_MSA)
#define HAVE_ssaddv8hi3 (ISA_HAS_MSA)
#define HAVE_ssaddv16qi3 (ISA_HAS_MSA)
#define HAVE_usaddv2di3 (ISA_HAS_MSA)
#define HAVE_usaddv4si3 (ISA_HAS_MSA)
#define HAVE_usaddv8hi3 (ISA_HAS_MSA)
#define HAVE_usaddv16qi3 (ISA_HAS_MSA)
#define HAVE_msa_asub_s_d (ISA_HAS_MSA)
#define HAVE_msa_asub_s_w (ISA_HAS_MSA)
#define HAVE_msa_asub_s_h (ISA_HAS_MSA)
#define HAVE_msa_asub_s_b (ISA_HAS_MSA)
#define HAVE_msa_asub_u_d (ISA_HAS_MSA)
#define HAVE_msa_asub_u_w (ISA_HAS_MSA)
#define HAVE_msa_asub_u_h (ISA_HAS_MSA)
#define HAVE_msa_asub_u_b (ISA_HAS_MSA)
#define HAVE_msa_ave_s_d (ISA_HAS_MSA)
#define HAVE_msa_ave_s_w (ISA_HAS_MSA)
#define HAVE_msa_ave_s_h (ISA_HAS_MSA)
#define HAVE_msa_ave_s_b (ISA_HAS_MSA)
#define HAVE_msa_ave_u_d (ISA_HAS_MSA)
#define HAVE_msa_ave_u_w (ISA_HAS_MSA)
#define HAVE_msa_ave_u_h (ISA_HAS_MSA)
#define HAVE_msa_ave_u_b (ISA_HAS_MSA)
#define HAVE_msa_aver_s_d (ISA_HAS_MSA)
#define HAVE_msa_aver_s_w (ISA_HAS_MSA)
#define HAVE_msa_aver_s_h (ISA_HAS_MSA)
#define HAVE_msa_aver_s_b (ISA_HAS_MSA)
#define HAVE_msa_aver_u_d (ISA_HAS_MSA)
#define HAVE_msa_aver_u_w (ISA_HAS_MSA)
#define HAVE_msa_aver_u_h (ISA_HAS_MSA)
#define HAVE_msa_aver_u_b (ISA_HAS_MSA)
#define HAVE_msa_bclr_d (ISA_HAS_MSA)
#define HAVE_msa_bclr_w (ISA_HAS_MSA)
#define HAVE_msa_bclr_h (ISA_HAS_MSA)
#define HAVE_msa_bclr_b (ISA_HAS_MSA)
#define HAVE_msa_bclri_d (ISA_HAS_MSA)
#define HAVE_msa_bclri_w (ISA_HAS_MSA)
#define HAVE_msa_bclri_h (ISA_HAS_MSA)
#define HAVE_msa_bclri_b (ISA_HAS_MSA)
#define HAVE_msa_binsl_d (ISA_HAS_MSA)
#define HAVE_msa_binsl_w (ISA_HAS_MSA)
#define HAVE_msa_binsl_h (ISA_HAS_MSA)
#define HAVE_msa_binsl_b (ISA_HAS_MSA)
#define HAVE_msa_binsli_d (ISA_HAS_MSA)
#define HAVE_msa_binsli_w (ISA_HAS_MSA)
#define HAVE_msa_binsli_h (ISA_HAS_MSA)
#define HAVE_msa_binsli_b (ISA_HAS_MSA)
#define HAVE_msa_binsr_d (ISA_HAS_MSA)
#define HAVE_msa_binsr_w (ISA_HAS_MSA)
#define HAVE_msa_binsr_h (ISA_HAS_MSA)
#define HAVE_msa_binsr_b (ISA_HAS_MSA)
#define HAVE_msa_binsri_d (ISA_HAS_MSA)
#define HAVE_msa_binsri_w (ISA_HAS_MSA)
#define HAVE_msa_binsri_h (ISA_HAS_MSA)
#define HAVE_msa_binsri_b (ISA_HAS_MSA)
#define HAVE_msa_bmnz_d (ISA_HAS_MSA)
#define HAVE_msa_bmnz_w (ISA_HAS_MSA)
#define HAVE_msa_bmnz_h (ISA_HAS_MSA)
#define HAVE_msa_bmnz_b (ISA_HAS_MSA)
#define HAVE_msa_bmz_d (ISA_HAS_MSA)
#define HAVE_msa_bmz_w (ISA_HAS_MSA)
#define HAVE_msa_bmz_h (ISA_HAS_MSA)
#define HAVE_msa_bmz_b (ISA_HAS_MSA)
#define HAVE_msa_bneg_d (ISA_HAS_MSA)
#define HAVE_msa_bneg_w (ISA_HAS_MSA)
#define HAVE_msa_bneg_h (ISA_HAS_MSA)
#define HAVE_msa_bneg_b (ISA_HAS_MSA)
#define HAVE_msa_bnegi_d (ISA_HAS_MSA)
#define HAVE_msa_bnegi_w (ISA_HAS_MSA)
#define HAVE_msa_bnegi_h (ISA_HAS_MSA)
#define HAVE_msa_bnegi_b (ISA_HAS_MSA)
#define HAVE_msa_bsel_d (ISA_HAS_MSA)
#define HAVE_msa_bsel_w (ISA_HAS_MSA)
#define HAVE_msa_bsel_h (ISA_HAS_MSA)
#define HAVE_msa_bsel_b (ISA_HAS_MSA)
#define HAVE_msa_bset_d (ISA_HAS_MSA)
#define HAVE_msa_bset_w (ISA_HAS_MSA)
#define HAVE_msa_bset_h (ISA_HAS_MSA)
#define HAVE_msa_bset_b (ISA_HAS_MSA)
#define HAVE_msa_bseti_d (ISA_HAS_MSA)
#define HAVE_msa_bseti_w (ISA_HAS_MSA)
#define HAVE_msa_bseti_h (ISA_HAS_MSA)
#define HAVE_msa_bseti_b (ISA_HAS_MSA)
#define HAVE_msa_ceq_d (ISA_HAS_MSA)
#define HAVE_msa_cle_s_d (ISA_HAS_MSA)
#define HAVE_msa_cle_u_d (ISA_HAS_MSA)
#define HAVE_msa_clt_s_d (ISA_HAS_MSA)
#define HAVE_msa_clt_u_d (ISA_HAS_MSA)
#define HAVE_msa_ceq_w (ISA_HAS_MSA)
#define HAVE_msa_cle_s_w (ISA_HAS_MSA)
#define HAVE_msa_cle_u_w (ISA_HAS_MSA)
#define HAVE_msa_clt_s_w (ISA_HAS_MSA)
#define HAVE_msa_clt_u_w (ISA_HAS_MSA)
#define HAVE_msa_ceq_h (ISA_HAS_MSA)
#define HAVE_msa_cle_s_h (ISA_HAS_MSA)
#define HAVE_msa_cle_u_h (ISA_HAS_MSA)
#define HAVE_msa_clt_s_h (ISA_HAS_MSA)
#define HAVE_msa_clt_u_h (ISA_HAS_MSA)
#define HAVE_msa_ceq_b (ISA_HAS_MSA)
#define HAVE_msa_cle_s_b (ISA_HAS_MSA)
#define HAVE_msa_cle_u_b (ISA_HAS_MSA)
#define HAVE_msa_clt_s_b (ISA_HAS_MSA)
#define HAVE_msa_clt_u_b (ISA_HAS_MSA)
#define HAVE_msa_dotp_s_d (ISA_HAS_MSA)
#define HAVE_msa_dotp_u_d (ISA_HAS_MSA)
#define HAVE_msa_dotp_s_w (ISA_HAS_MSA)
#define HAVE_msa_dotp_u_w (ISA_HAS_MSA)
#define HAVE_msa_dotp_s_h (ISA_HAS_MSA)
#define HAVE_msa_dotp_u_h (ISA_HAS_MSA)
#define HAVE_msa_dpadd_s_d (ISA_HAS_MSA)
#define HAVE_msa_dpadd_u_d (ISA_HAS_MSA)
#define HAVE_msa_dpadd_s_w (ISA_HAS_MSA)
#define HAVE_msa_dpadd_u_w (ISA_HAS_MSA)
#define HAVE_msa_dpadd_s_h (ISA_HAS_MSA)
#define HAVE_msa_dpadd_u_h (ISA_HAS_MSA)
#define HAVE_msa_dpsub_s_d (ISA_HAS_MSA)
#define HAVE_msa_dpsub_u_d (ISA_HAS_MSA)
#define HAVE_msa_dpsub_s_w (ISA_HAS_MSA)
#define HAVE_msa_dpsub_u_w (ISA_HAS_MSA)
#define HAVE_msa_dpsub_s_h (ISA_HAS_MSA)
#define HAVE_msa_dpsub_u_h (ISA_HAS_MSA)
#define HAVE_msa_fclass_d (ISA_HAS_MSA)
#define HAVE_msa_fclass_w (ISA_HAS_MSA)
#define HAVE_msa_fcaf_d (ISA_HAS_MSA)
#define HAVE_msa_fcaf_w (ISA_HAS_MSA)
#define HAVE_msa_fcune_d (ISA_HAS_MSA)
#define HAVE_msa_fcune_w (ISA_HAS_MSA)
#define HAVE_msa_fcun_d (ISA_HAS_MSA)
#define HAVE_msa_fcor_d (ISA_HAS_MSA)
#define HAVE_msa_fceq_d (ISA_HAS_MSA)
#define HAVE_msa_fcne_d (ISA_HAS_MSA)
#define HAVE_msa_fcle_d (ISA_HAS_MSA)
#define HAVE_msa_fclt_d (ISA_HAS_MSA)
#define HAVE_msa_fcueq_d (ISA_HAS_MSA)
#define HAVE_msa_fcule_d (ISA_HAS_MSA)
#define HAVE_msa_fcult_d (ISA_HAS_MSA)
#define HAVE_msa_fcun_w (ISA_HAS_MSA)
#define HAVE_msa_fcor_w (ISA_HAS_MSA)
#define HAVE_msa_fceq_w (ISA_HAS_MSA)
#define HAVE_msa_fcne_w (ISA_HAS_MSA)
#define HAVE_msa_fcle_w (ISA_HAS_MSA)
#define HAVE_msa_fclt_w (ISA_HAS_MSA)
#define HAVE_msa_fcueq_w (ISA_HAS_MSA)
#define HAVE_msa_fcule_w (ISA_HAS_MSA)
#define HAVE_msa_fcult_w (ISA_HAS_MSA)
#define HAVE_msa_fsaf_d (ISA_HAS_MSA)
#define HAVE_msa_fsun_d (ISA_HAS_MSA)
#define HAVE_msa_fsor_d (ISA_HAS_MSA)
#define HAVE_msa_fseq_d (ISA_HAS_MSA)
#define HAVE_msa_fsne_d (ISA_HAS_MSA)
#define HAVE_msa_fsueq_d (ISA_HAS_MSA)
#define HAVE_msa_fsune_d (ISA_HAS_MSA)
#define HAVE_msa_fsule_d (ISA_HAS_MSA)
#define HAVE_msa_fsult_d (ISA_HAS_MSA)
#define HAVE_msa_fsle_d (ISA_HAS_MSA)
#define HAVE_msa_fslt_d (ISA_HAS_MSA)
#define HAVE_msa_fsaf_w (ISA_HAS_MSA)
#define HAVE_msa_fsun_w (ISA_HAS_MSA)
#define HAVE_msa_fsor_w (ISA_HAS_MSA)
#define HAVE_msa_fseq_w (ISA_HAS_MSA)
#define HAVE_msa_fsne_w (ISA_HAS_MSA)
#define HAVE_msa_fsueq_w (ISA_HAS_MSA)
#define HAVE_msa_fsune_w (ISA_HAS_MSA)
#define HAVE_msa_fsule_w (ISA_HAS_MSA)
#define HAVE_msa_fsult_w (ISA_HAS_MSA)
#define HAVE_msa_fsle_w (ISA_HAS_MSA)
#define HAVE_msa_fslt_w (ISA_HAS_MSA)
#define HAVE_msa_fexp2_d (ISA_HAS_MSA)
#define HAVE_msa_fexp2_w (ISA_HAS_MSA)
#define HAVE_floatv2div2df2 (ISA_HAS_MSA)
#define HAVE_floatv4siv4sf2 (ISA_HAS_MSA)
#define HAVE_floatunsv2div2df2 (ISA_HAS_MSA)
#define HAVE_floatunsv4siv4sf2 (ISA_HAS_MSA)
#define HAVE_msa_ffql_d (ISA_HAS_MSA)
#define HAVE_msa_ffql_w (ISA_HAS_MSA)
#define HAVE_msa_ffqr_d (ISA_HAS_MSA)
#define HAVE_msa_ffqr_w (ISA_HAS_MSA)
#define HAVE_msa_fill_d_f (ISA_HAS_MSA)
#define HAVE_msa_fill_w_f (ISA_HAS_MSA)
#define HAVE_msa_fill_d (ISA_HAS_MSA)
#define HAVE_msa_fill_w (ISA_HAS_MSA)
#define HAVE_msa_fill_h (ISA_HAS_MSA)
#define HAVE_msa_fill_b (ISA_HAS_MSA)
#define HAVE_msa_flog2_d (ISA_HAS_MSA)
#define HAVE_msa_flog2_w (ISA_HAS_MSA)
#define HAVE_smaxv2df3 (ISA_HAS_MSA)
#define HAVE_smaxv4sf3 (ISA_HAS_MSA)
#define HAVE_msa_fmax_a_d (ISA_HAS_MSA)
#define HAVE_msa_fmax_a_w (ISA_HAS_MSA)
#define HAVE_sminv2df3 (ISA_HAS_MSA)
#define HAVE_sminv4sf3 (ISA_HAS_MSA)
#define HAVE_msa_fmin_a_d (ISA_HAS_MSA)
#define HAVE_msa_fmin_a_w (ISA_HAS_MSA)
#define HAVE_msa_frcp_d (ISA_HAS_MSA)
#define HAVE_msa_frcp_w (ISA_HAS_MSA)
#define HAVE_msa_frint_d (ISA_HAS_MSA)
#define HAVE_msa_frint_w (ISA_HAS_MSA)
#define HAVE_msa_frsqrt_d (ISA_HAS_MSA)
#define HAVE_msa_frsqrt_w (ISA_HAS_MSA)
#define HAVE_msa_ftint_s_d (ISA_HAS_MSA)
#define HAVE_msa_ftint_s_w (ISA_HAS_MSA)
#define HAVE_msa_ftint_u_d (ISA_HAS_MSA)
#define HAVE_msa_ftint_u_w (ISA_HAS_MSA)
#define HAVE_fix_truncv2dfv2di2 (ISA_HAS_MSA)
#define HAVE_fix_truncv4sfv4si2 (ISA_HAS_MSA)
#define HAVE_fixuns_truncv2dfv2di2 (ISA_HAS_MSA)
#define HAVE_fixuns_truncv4sfv4si2 (ISA_HAS_MSA)
#define HAVE_msa_ftq_h (ISA_HAS_MSA)
#define HAVE_msa_ftq_w (ISA_HAS_MSA)
#define HAVE_msa_hadd_s_h (ISA_HAS_MSA)
#define HAVE_msa_hadd_u_h (ISA_HAS_MSA)
#define HAVE_msa_hsub_s_h (ISA_HAS_MSA)
#define HAVE_msa_hsub_u_h (ISA_HAS_MSA)
#define HAVE_msa_hadd_s_w (ISA_HAS_MSA)
#define HAVE_msa_hadd_u_w (ISA_HAS_MSA)
#define HAVE_msa_hsub_s_w (ISA_HAS_MSA)
#define HAVE_msa_hsub_u_w (ISA_HAS_MSA)
#define HAVE_msa_hadd_s_d (ISA_HAS_MSA)
#define HAVE_msa_hadd_u_d (ISA_HAS_MSA)
#define HAVE_msa_hsub_s_d (ISA_HAS_MSA)
#define HAVE_msa_hsub_u_d (ISA_HAS_MSA)
#define HAVE_msa_ilvev_b (ISA_HAS_MSA)
#define HAVE_msa_ilvev_h (ISA_HAS_MSA)
#define HAVE_msa_ilvev_w (ISA_HAS_MSA)
#define HAVE_msa_ilvev_w_f (ISA_HAS_MSA)
#define HAVE_msa_ilvl_b (ISA_HAS_MSA)
#define HAVE_msa_ilvl_h (ISA_HAS_MSA)
#define HAVE_msa_ilvl_w (ISA_HAS_MSA)
#define HAVE_msa_ilvl_w_f (ISA_HAS_MSA)
#define HAVE_msa_ilvl_d (ISA_HAS_MSA)
#define HAVE_msa_ilvl_d_f (ISA_HAS_MSA)
#define HAVE_msa_ilvod_b (ISA_HAS_MSA)
#define HAVE_msa_ilvod_h (ISA_HAS_MSA)
#define HAVE_msa_ilvod_w (ISA_HAS_MSA)
#define HAVE_msa_ilvod_w_f (ISA_HAS_MSA)
#define HAVE_msa_ilvr_b (ISA_HAS_MSA)
#define HAVE_msa_ilvr_h (ISA_HAS_MSA)
#define HAVE_msa_ilvr_w (ISA_HAS_MSA)
#define HAVE_msa_ilvr_w_f (ISA_HAS_MSA)
#define HAVE_msa_ilvr_d (ISA_HAS_MSA)
#define HAVE_msa_ilvr_d_f (ISA_HAS_MSA)
#define HAVE_msa_madd_q_w (ISA_HAS_MSA)
#define HAVE_msa_madd_q_h (ISA_HAS_MSA)
#define HAVE_msa_maddr_q_w (ISA_HAS_MSA)
#define HAVE_msa_maddr_q_h (ISA_HAS_MSA)
#define HAVE_msa_max_a_d (ISA_HAS_MSA)
#define HAVE_msa_max_a_w (ISA_HAS_MSA)
#define HAVE_msa_max_a_h (ISA_HAS_MSA)
#define HAVE_msa_max_a_b (ISA_HAS_MSA)
#define HAVE_smaxv2di3 (ISA_HAS_MSA)
#define HAVE_smaxv4si3 (ISA_HAS_MSA)
#define HAVE_smaxv8hi3 (ISA_HAS_MSA)
#define HAVE_smaxv16qi3 (ISA_HAS_MSA)
#define HAVE_umaxv2di3 (ISA_HAS_MSA)
#define HAVE_umaxv4si3 (ISA_HAS_MSA)
#define HAVE_umaxv8hi3 (ISA_HAS_MSA)
#define HAVE_umaxv16qi3 (ISA_HAS_MSA)
#define HAVE_msa_min_a_d (ISA_HAS_MSA)
#define HAVE_msa_min_a_w (ISA_HAS_MSA)
#define HAVE_msa_min_a_h (ISA_HAS_MSA)
#define HAVE_msa_min_a_b (ISA_HAS_MSA)
#define HAVE_sminv2di3 (ISA_HAS_MSA)
#define HAVE_sminv4si3 (ISA_HAS_MSA)
#define HAVE_sminv8hi3 (ISA_HAS_MSA)
#define HAVE_sminv16qi3 (ISA_HAS_MSA)
#define HAVE_uminv2di3 (ISA_HAS_MSA)
#define HAVE_uminv4si3 (ISA_HAS_MSA)
#define HAVE_uminv8hi3 (ISA_HAS_MSA)
#define HAVE_uminv16qi3 (ISA_HAS_MSA)
#define HAVE_msa_msub_q_w (ISA_HAS_MSA)
#define HAVE_msa_msub_q_h (ISA_HAS_MSA)
#define HAVE_msa_msubr_q_w (ISA_HAS_MSA)
#define HAVE_msa_msubr_q_h (ISA_HAS_MSA)
#define HAVE_msa_mul_q_w (ISA_HAS_MSA)
#define HAVE_msa_mul_q_h (ISA_HAS_MSA)
#define HAVE_msa_mulr_q_w (ISA_HAS_MSA)
#define HAVE_msa_mulr_q_h (ISA_HAS_MSA)
#define HAVE_msa_nloc_d (ISA_HAS_MSA)
#define HAVE_msa_nloc_w (ISA_HAS_MSA)
#define HAVE_msa_nloc_h (ISA_HAS_MSA)
#define HAVE_msa_nloc_b (ISA_HAS_MSA)
#define HAVE_clzv2di2 (ISA_HAS_MSA)
#define HAVE_clzv4si2 (ISA_HAS_MSA)
#define HAVE_clzv8hi2 (ISA_HAS_MSA)
#define HAVE_clzv16qi2 (ISA_HAS_MSA)
#define HAVE_msa_nor_d (ISA_HAS_MSA)
#define HAVE_msa_nor_w (ISA_HAS_MSA)
#define HAVE_msa_nor_h (ISA_HAS_MSA)
#define HAVE_msa_nor_b (ISA_HAS_MSA)
#define HAVE_msa_pckev_b (ISA_HAS_MSA)
#define HAVE_msa_pckev_h (ISA_HAS_MSA)
#define HAVE_msa_pckev_w (ISA_HAS_MSA)
#define HAVE_msa_pckev_w_f (ISA_HAS_MSA)
#define HAVE_msa_pckod_b (ISA_HAS_MSA)
#define HAVE_msa_pckod_h (ISA_HAS_MSA)
#define HAVE_msa_pckod_w (ISA_HAS_MSA)
#define HAVE_msa_pckod_w_f (ISA_HAS_MSA)
#define HAVE_popcountv2di2 (ISA_HAS_MSA)
#define HAVE_popcountv4si2 (ISA_HAS_MSA)
#define HAVE_popcountv8hi2 (ISA_HAS_MSA)
#define HAVE_popcountv16qi2 (ISA_HAS_MSA)
#define HAVE_msa_sat_s_d (ISA_HAS_MSA)
#define HAVE_msa_sat_s_w (ISA_HAS_MSA)
#define HAVE_msa_sat_s_h (ISA_HAS_MSA)
#define HAVE_msa_sat_s_b (ISA_HAS_MSA)
#define HAVE_msa_sat_u_d (ISA_HAS_MSA)
#define HAVE_msa_sat_u_w (ISA_HAS_MSA)
#define HAVE_msa_sat_u_h (ISA_HAS_MSA)
#define HAVE_msa_sat_u_b (ISA_HAS_MSA)
#define HAVE_msa_shf_w (ISA_HAS_MSA)
#define HAVE_msa_shf_h (ISA_HAS_MSA)
#define HAVE_msa_shf_b (ISA_HAS_MSA)
#define HAVE_msa_shf_w_f (ISA_HAS_MSA)
#define HAVE_msa_srar_d (ISA_HAS_MSA)
#define HAVE_msa_srar_w (ISA_HAS_MSA)
#define HAVE_msa_srar_h (ISA_HAS_MSA)
#define HAVE_msa_srar_b (ISA_HAS_MSA)
#define HAVE_msa_srari_d (ISA_HAS_MSA)
#define HAVE_msa_srari_w (ISA_HAS_MSA)
#define HAVE_msa_srari_h (ISA_HAS_MSA)
#define HAVE_msa_srari_b (ISA_HAS_MSA)
#define HAVE_msa_srlr_d (ISA_HAS_MSA)
#define HAVE_msa_srlr_w (ISA_HAS_MSA)
#define HAVE_msa_srlr_h (ISA_HAS_MSA)
#define HAVE_msa_srlr_b (ISA_HAS_MSA)
#define HAVE_msa_srlri_d (ISA_HAS_MSA)
#define HAVE_msa_srlri_w (ISA_HAS_MSA)
#define HAVE_msa_srlri_h (ISA_HAS_MSA)
#define HAVE_msa_srlri_b (ISA_HAS_MSA)
#define HAVE_msa_subs_s_d (ISA_HAS_MSA)
#define HAVE_msa_subs_s_w (ISA_HAS_MSA)
#define HAVE_msa_subs_s_h (ISA_HAS_MSA)
#define HAVE_msa_subs_s_b (ISA_HAS_MSA)
#define HAVE_msa_subs_u_d (ISA_HAS_MSA)
#define HAVE_msa_subs_u_w (ISA_HAS_MSA)
#define HAVE_msa_subs_u_h (ISA_HAS_MSA)
#define HAVE_msa_subs_u_b (ISA_HAS_MSA)
#define HAVE_msa_subsuu_s_d (ISA_HAS_MSA)
#define HAVE_msa_subsuu_s_w (ISA_HAS_MSA)
#define HAVE_msa_subsuu_s_h (ISA_HAS_MSA)
#define HAVE_msa_subsuu_s_b (ISA_HAS_MSA)
#define HAVE_msa_subsus_u_d (ISA_HAS_MSA)
#define HAVE_msa_subsus_u_w (ISA_HAS_MSA)
#define HAVE_msa_subsus_u_h (ISA_HAS_MSA)
#define HAVE_msa_subsus_u_b (ISA_HAS_MSA)
#define HAVE_msa_sld_d_f (ISA_HAS_MSA)
#define HAVE_msa_sld_w_f (ISA_HAS_MSA)
#define HAVE_msa_sld_d (ISA_HAS_MSA)
#define HAVE_msa_sld_w (ISA_HAS_MSA)
#define HAVE_msa_sld_h (ISA_HAS_MSA)
#define HAVE_msa_sld_b (ISA_HAS_MSA)
#define HAVE_msa_sldi_d_f (ISA_HAS_MSA)
#define HAVE_msa_sldi_w_f (ISA_HAS_MSA)
#define HAVE_msa_sldi_d (ISA_HAS_MSA)
#define HAVE_msa_sldi_w (ISA_HAS_MSA)
#define HAVE_msa_sldi_h (ISA_HAS_MSA)
#define HAVE_msa_sldi_b (ISA_HAS_MSA)
#define HAVE_msa_splat_d_f (ISA_HAS_MSA)
#define HAVE_msa_splat_w_f (ISA_HAS_MSA)
#define HAVE_msa_splat_d (ISA_HAS_MSA)
#define HAVE_msa_splat_w (ISA_HAS_MSA)
#define HAVE_msa_splat_h (ISA_HAS_MSA)
#define HAVE_msa_splat_b (ISA_HAS_MSA)
#define HAVE_msa_splati_d_f (ISA_HAS_MSA)
#define HAVE_msa_splati_w_f (ISA_HAS_MSA)
#define HAVE_msa_splati_d (ISA_HAS_MSA)
#define HAVE_msa_splati_w (ISA_HAS_MSA)
#define HAVE_msa_splati_h (ISA_HAS_MSA)
#define HAVE_msa_splati_b (ISA_HAS_MSA)
#define HAVE_msa_splati_d_f_scalar (ISA_HAS_MSA)
#define HAVE_msa_splati_w_f_scalar (ISA_HAS_MSA)
#define HAVE_msa_cfcmsa (ISA_HAS_MSA)
#define HAVE_msa_ctcmsa (ISA_HAS_MSA)
#define HAVE_msa_fexdo_h (ISA_HAS_MSA)
#define HAVE_vec_pack_trunc_v2df (ISA_HAS_MSA)
#define HAVE_msa_fexupl_w (ISA_HAS_MSA)
#define HAVE_msa_fexupl_d (ISA_HAS_MSA)
#define HAVE_msa_fexupr_w (ISA_HAS_MSA)
#define HAVE_msa_fexupr_d (ISA_HAS_MSA)
#define HAVE_msa_bz_d_f (ISA_HAS_MSA)
#define HAVE_msa_bnz_d_f (ISA_HAS_MSA)
#define HAVE_msa_bz_w_f (ISA_HAS_MSA)
#define HAVE_msa_bnz_w_f (ISA_HAS_MSA)
#define HAVE_msa_bz_d (ISA_HAS_MSA)
#define HAVE_msa_bnz_d (ISA_HAS_MSA)
#define HAVE_msa_bz_w (ISA_HAS_MSA)
#define HAVE_msa_bnz_w (ISA_HAS_MSA)
#define HAVE_msa_bz_h (ISA_HAS_MSA)
#define HAVE_msa_bnz_h (ISA_HAS_MSA)
#define HAVE_msa_bz_b (ISA_HAS_MSA)
#define HAVE_msa_bnz_b (ISA_HAS_MSA)
#define HAVE_msa_bz_v_d_f (ISA_HAS_MSA)
#define HAVE_msa_bnz_v_d_f (ISA_HAS_MSA)
#define HAVE_msa_bz_v_w_f (ISA_HAS_MSA)
#define HAVE_msa_bnz_v_w_f (ISA_HAS_MSA)
#define HAVE_msa_bz_v_d (ISA_HAS_MSA)
#define HAVE_msa_bnz_v_d (ISA_HAS_MSA)
#define HAVE_msa_bz_v_w (ISA_HAS_MSA)
#define HAVE_msa_bnz_v_w (ISA_HAS_MSA)
#define HAVE_msa_bz_v_h (ISA_HAS_MSA)
#define HAVE_msa_bnz_v_h (ISA_HAS_MSA)
#define HAVE_msa_bz_v_b (ISA_HAS_MSA)
#define HAVE_msa_bnz_v_b (ISA_HAS_MSA)
#define HAVE_ctrapsi4 (ISA_HAS_COND_TRAPI || ISA_HAS_COND_TRAP)
#define HAVE_ctrapdi4 ((ISA_HAS_COND_TRAPI || ISA_HAS_COND_TRAP) && (TARGET_64BIT))
#define HAVE_addsi3 1
#define HAVE_adddi3 (TARGET_64BIT)
#define HAVE_mulsf3 (TARGET_HARD_FLOAT)
#define HAVE_muldf3 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)
#define HAVE_mulsi3 1
#define HAVE_muldi3 ((ISA_HAS_DMULT || ISA_HAS_R6DMUL) && (TARGET_64BIT))
#define HAVE_mulsidi3 (mips_mulsidi3_gen_fn (SIGN_EXTEND) != NULL)
#define HAVE_umulsidi3 (mips_mulsidi3_gen_fn (ZERO_EXTEND) != NULL)
#define HAVE_mulsidi3_32bit_r6 (!TARGET_64BIT && ISA_HAS_R6MUL)
#define HAVE_umulsidi3_32bit_r6 (!TARGET_64BIT && ISA_HAS_R6MUL)
#define HAVE_mulsidi3_32bit_mips16 (!TARGET_64BIT && TARGET_MIPS16)
#define HAVE_umulsidi3_32bit_mips16 (!TARGET_64BIT && TARGET_MIPS16)
#define HAVE_mulsidi3_64bit_mips16 (TARGET_64BIT && TARGET_MIPS16)
#define HAVE_umulsidi3_64bit_mips16 (TARGET_64BIT && TARGET_MIPS16)
#define HAVE_mulsidi3_64bit_split 1
#define HAVE_umulsidi3_64bit_split 1
#define HAVE_smulsi3_highpart 1
#define HAVE_umulsi3_highpart 1
#define HAVE_smulsi3_highpart_split 1
#define HAVE_umulsi3_highpart_split 1
#define HAVE_smuldi3_highpart (ISA_HAS_R6DMUL \
   || (ISA_HAS_DMULT \
       && !(SIGN_EXTEND == ZERO_EXTEND && TARGET_FIX_VR4120)))
#define HAVE_umuldi3_highpart (ISA_HAS_R6DMUL \
   || (ISA_HAS_DMULT \
       && !(ZERO_EXTEND == ZERO_EXTEND && TARGET_FIX_VR4120)))
#define HAVE_smuldi3_highpart_split 1
#define HAVE_umuldi3_highpart_split 1
#define HAVE_mulditi3 (ISA_HAS_DMULT && !(SIGN_EXTEND == ZERO_EXTEND && TARGET_FIX_VR4120))
#define HAVE_umulditi3 (ISA_HAS_DMULT && !(ZERO_EXTEND == ZERO_EXTEND && TARGET_FIX_VR4120))
#define HAVE_fmasf4 ((ISA_HAS_FUSED_MADDF || ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4) && (TARGET_HARD_FLOAT))
#define HAVE_fmadf4 ((ISA_HAS_FUSED_MADDF || ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_fmav2sf4 ((ISA_HAS_FUSED_MADDF || ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_fmssf4 (((ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4)) && (TARGET_HARD_FLOAT))
#define HAVE_fmsdf4 (((ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_fmsv2sf4 (((ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4)) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_fnmasf4 (((ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4) \
   && !HONOR_SIGNED_ZEROS (SFmode)) && (TARGET_HARD_FLOAT))
#define HAVE_fnmadf4 (((ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4) \
   && !HONOR_SIGNED_ZEROS (DFmode)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_fnmav2sf4 (((ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4) \
   && !HONOR_SIGNED_ZEROS (V2SFmode)) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_fnmssf4 (((ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4) \
   && !HONOR_SIGNED_ZEROS (SFmode)) && (TARGET_HARD_FLOAT))
#define HAVE_fnmsdf4 (((ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4) \
   && !HONOR_SIGNED_ZEROS (DFmode)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_fnmsv2sf4 (((ISA_HAS_FUSED_MADD3 || ISA_HAS_FUSED_MADD4) \
   && !HONOR_SIGNED_ZEROS (V2SFmode)) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_divsf3 ((!TARGET_FIX_SB1 || flag_unsafe_math_optimizations) && (TARGET_HARD_FLOAT))
#define HAVE_divdf3 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)
#define HAVE_divv2sf3 ((TARGET_SB1 && (!TARGET_FIX_SB1 || flag_unsafe_math_optimizations)) && (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT))
#define HAVE_divmodsi4 (ISA_HAS_DIV && !TARGET_FIX_VR4120)
#define HAVE_divmoddi4 ((ISA_HAS_DDIV && !TARGET_FIX_VR4120) && (TARGET_64BIT))
#define HAVE_udivmodsi4 (ISA_HAS_DIV && !TARGET_FIX_VR4120)
#define HAVE_udivmoddi4 ((ISA_HAS_DDIV && !TARGET_FIX_VR4120) && (TARGET_64BIT))
#define HAVE_divmodsi4_split 1
#define HAVE_udivmodsi4_split 1
#define HAVE_divmoddi4_split (TARGET_64BIT)
#define HAVE_udivmoddi4_split (TARGET_64BIT)
#define HAVE_andsi3 1
#define HAVE_anddi3 (TARGET_64BIT)
#define HAVE_iorsi3 1
#define HAVE_iordi3 (TARGET_64BIT)
#define HAVE_xorsi3 1
#define HAVE_xordi3 (TARGET_64BIT)
#define HAVE_zero_extendsidi2 (TARGET_64BIT)
#define HAVE_zero_extendqisi2 1
#define HAVE_zero_extendqidi2 (TARGET_64BIT)
#define HAVE_zero_extendhisi2 1
#define HAVE_zero_extendhidi2 (TARGET_64BIT)
#define HAVE_zero_extendqihi2 1
#define HAVE_extendqisi2 1
#define HAVE_extendqidi2 (TARGET_64BIT)
#define HAVE_extendhisi2 1
#define HAVE_extendhidi2 (TARGET_64BIT)
#define HAVE_extendqihi2 1
#define HAVE_fix_truncdfsi2 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)
#define HAVE_fix_truncsfsi2 (TARGET_HARD_FLOAT)
#define HAVE_fixuns_truncdfsi2 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)
#define HAVE_fixuns_truncdfdi2 (TARGET_HARD_FLOAT && TARGET_64BIT && TARGET_DOUBLE_FLOAT)
#define HAVE_fixuns_truncsfsi2 (TARGET_HARD_FLOAT)
#define HAVE_fixuns_truncsfdi2 (TARGET_HARD_FLOAT && TARGET_64BIT && TARGET_DOUBLE_FLOAT)
#define HAVE_extvmisalignsi (ISA_HAS_LWL_LWR)
#define HAVE_extvmisaligndi ((ISA_HAS_LWL_LWR) && (TARGET_64BIT))
#define HAVE_extvsi (ISA_HAS_EXTS)
#define HAVE_extvdi ((ISA_HAS_EXTS) && (TARGET_64BIT))
#define HAVE_extzvmisalignsi (ISA_HAS_LWL_LWR)
#define HAVE_extzvmisaligndi ((ISA_HAS_LWL_LWR) && (TARGET_64BIT))
#define HAVE_extzvsi 1
#define HAVE_extzvdi (TARGET_64BIT)
#define HAVE_insvmisalignsi (ISA_HAS_LWL_LWR)
#define HAVE_insvmisaligndi ((ISA_HAS_LWL_LWR) && (TARGET_64BIT))
#define HAVE_insvsi 1
#define HAVE_insvdi (TARGET_64BIT)
#define HAVE_unspec_got_si (Pmode == SImode)
#define HAVE_unspec_got_di (Pmode == DImode)
#define HAVE_movdi 1
#define HAVE_movsi 1
#define HAVE_movv2hi (TARGET_DSP)
#define HAVE_movv4qi (TARGET_DSP)
#define HAVE_movv2hq (TARGET_DSP)
#define HAVE_movv2uhq (TARGET_DSP)
#define HAVE_movv2ha (TARGET_DSP)
#define HAVE_movv2uha (TARGET_DSP)
#define HAVE_movv4qq (TARGET_DSP)
#define HAVE_movv4uqq (TARGET_DSP)
#define HAVE_movhi 1
#define HAVE_movqi 1
#define HAVE_movsf 1
#define HAVE_movdf 1
#define HAVE_movti (TARGET_64BIT)
#define HAVE_movtf (TARGET_64BIT)
#define HAVE_movv2sf (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_move_doubleword_fprdf (!TARGET_64BIT && TARGET_DOUBLE_FLOAT)
#define HAVE_move_doubleword_fprdi (!TARGET_64BIT && TARGET_DOUBLE_FLOAT)
#define HAVE_move_doubleword_fprv2sf (!TARGET_64BIT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_move_doubleword_fprv2si (!TARGET_64BIT && TARGET_LOONGSON_VECTORS)
#define HAVE_move_doubleword_fprv4hi (!TARGET_64BIT && TARGET_LOONGSON_VECTORS)
#define HAVE_move_doubleword_fprv8qi (!TARGET_64BIT && TARGET_LOONGSON_VECTORS)
#define HAVE_move_doubleword_fprtf (TARGET_64BIT && TARGET_FLOAT64)
#define HAVE_load_const_gp_si (Pmode == SImode)
#define HAVE_load_const_gp_di (Pmode == DImode)
#define HAVE_clear_cache 1
#define HAVE_movmemsi (!TARGET_MIPS16 && !TARGET_MEMCPY)
#define HAVE_ashlsi3 1
#define HAVE_ashrsi3 1
#define HAVE_lshrsi3 1
#define HAVE_ashldi3 (TARGET_64BIT)
#define HAVE_ashrdi3 (TARGET_64BIT)
#define HAVE_lshrdi3 (TARGET_64BIT)
#define HAVE_cbranchsi4 1
#define HAVE_cbranchdi4 (TARGET_64BIT)
#define HAVE_cbranchsf4 (TARGET_HARD_FLOAT)
#define HAVE_cbranchdf4 (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT)
#define HAVE_condjump 1
#define HAVE_cstoresi4 1
#define HAVE_cstoredi4 (TARGET_64BIT)
#define HAVE_jump 1
#define HAVE_indirect_jump 1
#define HAVE_tablejump (!TARGET_MIPS16_SHORT_JUMP_TABLES)
#define HAVE_casesi (TARGET_MIPS16_SHORT_JUMP_TABLES)
#define HAVE_builtin_setjmp_setup (TARGET_USE_GOT)
#define HAVE_builtin_longjmp (TARGET_USE_GOT)
#define HAVE_prologue 1
#define HAVE_epilogue 1
#define HAVE_sibcall_epilogue 1
#define HAVE_return (mips_can_use_return_insn ())
#define HAVE_simple_return 1
#define HAVE_eh_return 1
#define HAVE_exception_receiver (TARGET_USE_GOT)
#define HAVE_nonlocal_goto_receiver (TARGET_USE_GOT)
#define HAVE_sibcall (TARGET_SIBCALLS)
#define HAVE_sibcall_value (TARGET_SIBCALLS)
#define HAVE_call 1
#define HAVE_call_value 1
#define HAVE_untyped_call 1
#define HAVE_movsicc (ISA_HAS_CONDMOVE || ISA_HAS_SEL)
#define HAVE_movdicc ((ISA_HAS_CONDMOVE || ISA_HAS_SEL) && (TARGET_64BIT))
#define HAVE_movsfcc ((ISA_HAS_FP_CONDMOVE \
   || (ISA_HAS_SEL && ISA_HAS_CCF)) && (TARGET_HARD_FLOAT))
#define HAVE_movdfcc ((ISA_HAS_FP_CONDMOVE \
   || (ISA_HAS_SEL && ISA_HAS_CCF)) && (TARGET_HARD_FLOAT && TARGET_DOUBLE_FLOAT))
#define HAVE_get_thread_pointersi ((HAVE_AS_TLS) && (Pmode == SImode))
#define HAVE_get_thread_pointerdi ((HAVE_AS_TLS) && (Pmode == DImode))
#define HAVE_mips_get_fcsr (TARGET_HARD_FLOAT_ABI)
#define HAVE_mips_set_fcsr (TARGET_HARD_FLOAT_ABI)
#define HAVE_memory_barrier (GENERATE_SYNC)
#define HAVE_sync_compare_and_swapqi (GENERATE_LL_SC)
#define HAVE_sync_compare_and_swaphi (GENERATE_LL_SC)
#define HAVE_sync_addqi (GENERATE_LL_SC)
#define HAVE_sync_subqi (GENERATE_LL_SC)
#define HAVE_sync_iorqi (GENERATE_LL_SC)
#define HAVE_sync_xorqi (GENERATE_LL_SC)
#define HAVE_sync_andqi (GENERATE_LL_SC)
#define HAVE_sync_addhi (GENERATE_LL_SC)
#define HAVE_sync_subhi (GENERATE_LL_SC)
#define HAVE_sync_iorhi (GENERATE_LL_SC)
#define HAVE_sync_xorhi (GENERATE_LL_SC)
#define HAVE_sync_andhi (GENERATE_LL_SC)
#define HAVE_sync_old_addqi (GENERATE_LL_SC)
#define HAVE_sync_old_subqi (GENERATE_LL_SC)
#define HAVE_sync_old_iorqi (GENERATE_LL_SC)
#define HAVE_sync_old_xorqi (GENERATE_LL_SC)
#define HAVE_sync_old_andqi (GENERATE_LL_SC)
#define HAVE_sync_old_addhi (GENERATE_LL_SC)
#define HAVE_sync_old_subhi (GENERATE_LL_SC)
#define HAVE_sync_old_iorhi (GENERATE_LL_SC)
#define HAVE_sync_old_xorhi (GENERATE_LL_SC)
#define HAVE_sync_old_andhi (GENERATE_LL_SC)
#define HAVE_sync_new_addqi (GENERATE_LL_SC)
#define HAVE_sync_new_subqi (GENERATE_LL_SC)
#define HAVE_sync_new_iorqi (GENERATE_LL_SC)
#define HAVE_sync_new_xorqi (GENERATE_LL_SC)
#define HAVE_sync_new_andqi (GENERATE_LL_SC)
#define HAVE_sync_new_addhi (GENERATE_LL_SC)
#define HAVE_sync_new_subhi (GENERATE_LL_SC)
#define HAVE_sync_new_iorhi (GENERATE_LL_SC)
#define HAVE_sync_new_xorhi (GENERATE_LL_SC)
#define HAVE_sync_new_andhi (GENERATE_LL_SC)
#define HAVE_sync_nandqi (GENERATE_LL_SC)
#define HAVE_sync_nandhi (GENERATE_LL_SC)
#define HAVE_sync_old_nandqi (GENERATE_LL_SC)
#define HAVE_sync_old_nandhi (GENERATE_LL_SC)
#define HAVE_sync_new_nandqi (GENERATE_LL_SC)
#define HAVE_sync_new_nandhi (GENERATE_LL_SC)
#define HAVE_sync_lock_test_and_setqi (GENERATE_LL_SC)
#define HAVE_sync_lock_test_and_sethi (GENERATE_LL_SC)
#define HAVE_atomic_exchangesi (GENERATE_LL_SC || ISA_HAS_SWAP)
#define HAVE_atomic_exchangedi ((GENERATE_LL_SC || ISA_HAS_SWAP) && (TARGET_64BIT))
#define HAVE_atomic_fetch_addsi (GENERATE_LL_SC || ISA_HAS_LDADD)
#define HAVE_atomic_fetch_adddi ((GENERATE_LL_SC || ISA_HAS_LDADD) && (TARGET_64BIT))
#define HAVE_movv2sfcc (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_puu_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_pul_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_plu_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_pll_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_vec_initv2sfsf (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_vec_setv2sf (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_cvt_ps_s (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_cvt_s_pl (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_cvt_s_pu (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_reduc_plus_scal_v2sf (TARGET_HARD_FLOAT && TARGET_MIPS3D)
#define HAVE_mips_abs_ps (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_scc_ps 1
#define HAVE_single_cc 1
#define HAVE_vcondv2sfv2sf (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_sminv2sf3 (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_smaxv2sf3 (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_reduc_smin_scal_v2sf (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_reduc_smax_scal_v2sf (TARGET_HARD_FLOAT && TARGET_PAIRED_SINGLE_FLOAT)
#define HAVE_mips_lbux (ISA_HAS_DSP)
#define HAVE_mips_lhx (ISA_HAS_DSP)
#define HAVE_mips_lwx (ISA_HAS_DSP)
#define HAVE_mips_ldx ((ISA_HAS_DSP) && (TARGET_64BIT))
#define HAVE_mips_madd (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_maddu (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_msub (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_mips_msubu (ISA_HAS_DSP && !TARGET_64BIT)
#define HAVE_movv2si (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_movv4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_movv8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_initv2sisi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_initv4hihi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_initv8qiqi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_setv4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_sdot_prodv4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_smaxv2si3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_smaxv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_sminv2si3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_sminv8qi3 (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_unpacks_lo_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_unpacks_lo_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_unpacks_hi_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_unpacks_hi_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_unpacku_lo_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_unpacku_lo_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_unpacku_hi_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_unpacku_hi_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_plus_scal_v2si (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_plus_scal_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_plus_scal_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_smax_scal_v2si (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_smax_scal_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_smax_scal_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_smin_scal_v2si (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_smin_scal_v4hi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_smin_scal_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_umax_scal_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_reduc_umin_scal_v8qi (TARGET_HARD_FLOAT && TARGET_LOONGSON_VECTORS)
#define HAVE_vec_initv2dfdf (ISA_HAS_MSA)
#define HAVE_vec_initv4sfsf (ISA_HAS_MSA)
#define HAVE_vec_initv2didi (ISA_HAS_MSA)
#define HAVE_vec_initv4sisi (ISA_HAS_MSA)
#define HAVE_vec_initv8hihi (ISA_HAS_MSA)
#define HAVE_vec_initv16qiqi (ISA_HAS_MSA)
#define HAVE_vec_unpacks_hi_v4sf (ISA_HAS_MSA)
#define HAVE_vec_unpacks_lo_v4sf (ISA_HAS_MSA)
#define HAVE_vec_unpacks_hi_v4si (ISA_HAS_MSA)
#define HAVE_vec_unpacks_hi_v8hi (ISA_HAS_MSA)
#define HAVE_vec_unpacks_hi_v16qi (ISA_HAS_MSA)
#define HAVE_vec_unpacks_lo_v4si (ISA_HAS_MSA)
#define HAVE_vec_unpacks_lo_v8hi (ISA_HAS_MSA)
#define HAVE_vec_unpacks_lo_v16qi (ISA_HAS_MSA)
#define HAVE_vec_unpacku_hi_v4si (ISA_HAS_MSA)
#define HAVE_vec_unpacku_hi_v8hi (ISA_HAS_MSA)
#define HAVE_vec_unpacku_hi_v16qi (ISA_HAS_MSA)
#define HAVE_vec_unpacku_lo_v4si (ISA_HAS_MSA)
#define HAVE_vec_unpacku_lo_v8hi (ISA_HAS_MSA)
#define HAVE_vec_unpacku_lo_v16qi (ISA_HAS_MSA)
#define HAVE_vec_extractv2didi (ISA_HAS_MSA)
#define HAVE_vec_extractv4sisi (ISA_HAS_MSA)
#define HAVE_vec_extractv8hihi (ISA_HAS_MSA)
#define HAVE_vec_extractv16qiqi (ISA_HAS_MSA)
#define HAVE_vec_extractv2dfdf (ISA_HAS_MSA)
#define HAVE_vec_extractv4sfsf (ISA_HAS_MSA)
#define HAVE_vec_setv2di (ISA_HAS_MSA)
#define HAVE_vec_setv4si (ISA_HAS_MSA)
#define HAVE_vec_setv8hi (ISA_HAS_MSA)
#define HAVE_vec_setv16qi (ISA_HAS_MSA)
#define HAVE_vec_setv2df (ISA_HAS_MSA)
#define HAVE_vec_setv4sf (ISA_HAS_MSA)
#define HAVE_vconduv2dfv2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DFmode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vconduv2dfv4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DFmode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vconduv2dfv8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DFmode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vconduv2dfv16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DFmode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vconduv4sfv2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SFmode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vconduv4sfv4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SFmode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vconduv4sfv8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SFmode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vconduv4sfv16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SFmode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vconduv2div2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DImode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vconduv2div4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DImode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vconduv2div8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DImode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vconduv2div16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DImode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vconduv4siv2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SImode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vconduv4siv4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SImode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vconduv4siv8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SImode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vconduv4siv16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SImode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vconduv8hiv2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V8HImode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vconduv8hiv4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V8HImode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vconduv8hiv8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V8HImode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vconduv8hiv16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V8HImode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vconduv16qiv2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V16QImode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vconduv16qiv4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V16QImode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vconduv16qiv8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V16QImode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vconduv16qiv16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V16QImode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vcondv2dfv2df (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DFmode) == GET_MODE_NUNITS (V2DFmode)))
#define HAVE_vcondv4sfv2df (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SFmode) == GET_MODE_NUNITS (V2DFmode)))
#define HAVE_vcondv2div2df (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DImode) == GET_MODE_NUNITS (V2DFmode)))
#define HAVE_vcondv4siv2df (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SImode) == GET_MODE_NUNITS (V2DFmode)))
#define HAVE_vcondv8hiv2df (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V8HImode) == GET_MODE_NUNITS (V2DFmode)))
#define HAVE_vcondv16qiv2df (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V16QImode) == GET_MODE_NUNITS (V2DFmode)))
#define HAVE_vcondv2dfv4sf (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DFmode) == GET_MODE_NUNITS (V4SFmode)))
#define HAVE_vcondv4sfv4sf (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SFmode) == GET_MODE_NUNITS (V4SFmode)))
#define HAVE_vcondv2div4sf (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DImode) == GET_MODE_NUNITS (V4SFmode)))
#define HAVE_vcondv4siv4sf (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SImode) == GET_MODE_NUNITS (V4SFmode)))
#define HAVE_vcondv8hiv4sf (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V8HImode) == GET_MODE_NUNITS (V4SFmode)))
#define HAVE_vcondv16qiv4sf (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V16QImode) == GET_MODE_NUNITS (V4SFmode)))
#define HAVE_vcondv2dfv2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DFmode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vcondv4sfv2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SFmode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vcondv2div2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DImode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vcondv4siv2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SImode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vcondv8hiv2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V8HImode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vcondv16qiv2di (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V16QImode) == GET_MODE_NUNITS (V2DImode)))
#define HAVE_vcondv2dfv4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DFmode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vcondv4sfv4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SFmode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vcondv2div4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DImode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vcondv4siv4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SImode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vcondv8hiv4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V8HImode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vcondv16qiv4si (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V16QImode) == GET_MODE_NUNITS (V4SImode)))
#define HAVE_vcondv2dfv8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DFmode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vcondv4sfv8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SFmode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vcondv2div8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DImode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vcondv4siv8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SImode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vcondv8hiv8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V8HImode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vcondv16qiv8hi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V16QImode) == GET_MODE_NUNITS (V8HImode)))
#define HAVE_vcondv2dfv16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DFmode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vcondv4sfv16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SFmode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vcondv2div16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V2DImode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vcondv4siv16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V4SImode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vcondv8hiv16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V8HImode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_vcondv16qiv16qi (ISA_HAS_MSA \
   && (GET_MODE_NUNITS (V16QImode) == GET_MODE_NUNITS (V16QImode)))
#define HAVE_absv2di2 (ISA_HAS_MSA)
#define HAVE_absv4si2 (ISA_HAS_MSA)
#define HAVE_absv8hi2 (ISA_HAS_MSA)
#define HAVE_absv16qi2 (ISA_HAS_MSA)
#define HAVE_negv2df2 (ISA_HAS_MSA)
#define HAVE_negv4sf2 (ISA_HAS_MSA)
#define HAVE_negv2di2 (ISA_HAS_MSA)
#define HAVE_negv4si2 (ISA_HAS_MSA)
#define HAVE_negv8hi2 (ISA_HAS_MSA)
#define HAVE_negv16qi2 (ISA_HAS_MSA)
#define HAVE_msa_ldiv2di (ISA_HAS_MSA)
#define HAVE_msa_ldiv4si (ISA_HAS_MSA)
#define HAVE_msa_ldiv8hi (ISA_HAS_MSA)
#define HAVE_msa_ldiv16qi (ISA_HAS_MSA)
#define HAVE_movv2df (ISA_HAS_MSA)
#define HAVE_movv4sf (ISA_HAS_MSA)
#define HAVE_movv2di (ISA_HAS_MSA)
#define HAVE_movv4si (ISA_HAS_MSA)
#define HAVE_movv8hi (ISA_HAS_MSA)
#define HAVE_movv16qi (ISA_HAS_MSA)
#define HAVE_movmisalignv2df (ISA_HAS_MSA)
#define HAVE_movmisalignv4sf (ISA_HAS_MSA)
#define HAVE_movmisalignv2di (ISA_HAS_MSA)
#define HAVE_movmisalignv4si (ISA_HAS_MSA)
#define HAVE_movmisalignv8hi (ISA_HAS_MSA)
#define HAVE_movmisalignv16qi (ISA_HAS_MSA)
#define HAVE_msa_ld_d_f (ISA_HAS_MSA)
#define HAVE_msa_ld_w_f (ISA_HAS_MSA)
#define HAVE_msa_ld_d (ISA_HAS_MSA)
#define HAVE_msa_ld_w (ISA_HAS_MSA)
#define HAVE_msa_ld_h (ISA_HAS_MSA)
#define HAVE_msa_ld_b (ISA_HAS_MSA)
#define HAVE_msa_st_d_f (ISA_HAS_MSA)
#define HAVE_msa_st_w_f (ISA_HAS_MSA)
#define HAVE_msa_st_d (ISA_HAS_MSA)
#define HAVE_msa_st_w (ISA_HAS_MSA)
#define HAVE_msa_st_h (ISA_HAS_MSA)
#define HAVE_msa_st_b (ISA_HAS_MSA)
extern rtx        gen_ls2_alu1_turn_enabled_insn      (void);
extern rtx        gen_ls2_alu2_turn_enabled_insn      (void);
extern rtx        gen_ls2_falu1_turn_enabled_insn     (void);
extern rtx        gen_ls2_falu2_turn_enabled_insn     (void);
extern rtx        gen_trap                            (void);
extern rtx        gen_addsf3                          (rtx, rtx, rtx);
extern rtx        gen_adddf3                          (rtx, rtx, rtx);
extern rtx        gen_addv2sf3                        (rtx, rtx, rtx);
extern rtx        gen_subsf3                          (rtx, rtx, rtx);
extern rtx        gen_subdf3                          (rtx, rtx, rtx);
extern rtx        gen_subv2sf3                        (rtx, rtx, rtx);
extern rtx        gen_subsi3                          (rtx, rtx, rtx);
extern rtx        gen_subdi3                          (rtx, rtx, rtx);
extern rtx        gen_mulv2sf3                        (rtx, rtx, rtx);
extern rtx        gen_mulsi3_mul3_nohilo              (rtx, rtx, rtx);
extern rtx        gen_muldi3_mul3_nohilo              (rtx, rtx, rtx);
extern rtx        gen_mulsi3_mul3                     (rtx, rtx, rtx);
extern rtx        gen_muldi3_mul3                     (rtx, rtx, rtx);
extern rtx        gen_mulsi3_internal                 (rtx, rtx, rtx);
extern rtx        gen_muldi3_internal                 (rtx, rtx, rtx);
extern rtx        gen_mulsi3_r4000                    (rtx, rtx, rtx);
extern rtx        gen_muldi3_r4000                    (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_32bit                  (rtx, rtx, rtx);
extern rtx        gen_umulsidi3_32bit                 (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_32bit_r4000            (rtx, rtx, rtx);
extern rtx        gen_umulsidi3_32bit_r4000           (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_64bit                  (rtx, rtx, rtx);
extern rtx        gen_umulsidi3_64bit                 (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_64bit_hilo             (rtx, rtx, rtx);
extern rtx        gen_umulsidi3_64bit_hilo            (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_64bit_dmul             (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_64bit_r6dmul           (rtx, rtx, rtx);
extern rtx        gen_msubsidi4                       (rtx, rtx, rtx, rtx);
extern rtx        gen_umsubsidi4                      (rtx, rtx, rtx, rtx);
extern rtx        gen_smulsi3_highpart_r6             (rtx, rtx, rtx);
extern rtx        gen_umulsi3_highpart_r6             (rtx, rtx, rtx);
extern rtx        gen_smulsi3_highpart_internal       (rtx, rtx, rtx);
extern rtx        gen_umulsi3_highpart_internal       (rtx, rtx, rtx);
extern rtx        gen_smulsi3_highpart_mulhi_internal (rtx, rtx, rtx);
extern rtx        gen_umulsi3_highpart_mulhi_internal (rtx, rtx, rtx);
extern rtx        gen_smuldi3_highpart_r6             (rtx, rtx, rtx);
extern rtx        gen_umuldi3_highpart_r6             (rtx, rtx, rtx);
extern rtx        gen_smuldi3_highpart_internal       (rtx, rtx, rtx);
extern rtx        gen_umuldi3_highpart_internal       (rtx, rtx, rtx);
extern rtx        gen_mulditi3_internal               (rtx, rtx, rtx);
extern rtx        gen_umulditi3_internal              (rtx, rtx, rtx);
extern rtx        gen_mulditi3_r4000                  (rtx, rtx, rtx);
extern rtx        gen_umulditi3_r4000                 (rtx, rtx, rtx);
extern rtx        gen_madsi                           (rtx, rtx, rtx);
extern rtx        gen_maddsidi4                       (rtx, rtx, rtx, rtx);
extern rtx        gen_umaddsidi4                      (rtx, rtx, rtx, rtx);
extern rtx        gen_divmodsi4_mips16                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_divmoddi4_mips16                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_udivmodsi4_mips16               (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_udivmoddi4_mips16               (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_divmodsi4_hilo_di               (rtx, rtx, rtx);
extern rtx        gen_udivmodsi4_hilo_di              (rtx, rtx, rtx);
static inline rtx gen_divmoddi4_hilo_di               (rtx, rtx, rtx);
static inline rtx
gen_divmoddi4_hilo_di(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_udivmoddi4_hilo_di              (rtx, rtx, rtx);
static inline rtx
gen_udivmoddi4_hilo_di(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_divmodsi4_hilo_ti               (rtx, rtx, rtx);
extern rtx        gen_udivmodsi4_hilo_ti              (rtx, rtx, rtx);
extern rtx        gen_divmoddi4_hilo_ti               (rtx, rtx, rtx);
extern rtx        gen_udivmoddi4_hilo_ti              (rtx, rtx, rtx);
extern rtx        gen_divsi3                          (rtx, rtx, rtx);
extern rtx        gen_udivsi3                         (rtx, rtx, rtx);
extern rtx        gen_divdi3                          (rtx, rtx, rtx);
extern rtx        gen_udivdi3                         (rtx, rtx, rtx);
extern rtx        gen_modsi3                          (rtx, rtx, rtx);
extern rtx        gen_umodsi3                         (rtx, rtx, rtx);
extern rtx        gen_moddi3                          (rtx, rtx, rtx);
extern rtx        gen_umoddi3                         (rtx, rtx, rtx);
extern rtx        gen_sqrtsf2                         (rtx, rtx);
extern rtx        gen_sqrtdf2                         (rtx, rtx);
extern rtx        gen_sqrtv2sf2                       (rtx, rtx);
extern rtx        gen_abssf2                          (rtx, rtx);
extern rtx        gen_absdf2                          (rtx, rtx);
extern rtx        gen_absv2sf2                        (rtx, rtx);
extern rtx        gen_clzsi2                          (rtx, rtx);
extern rtx        gen_clzdi2                          (rtx, rtx);
extern rtx        gen_popcountsi2                     (rtx, rtx);
extern rtx        gen_popcountdi2                     (rtx, rtx);
extern rtx        gen_negsi2                          (rtx, rtx);
extern rtx        gen_negdi2                          (rtx, rtx);
extern rtx        gen_negsf2                          (rtx, rtx);
extern rtx        gen_negdf2                          (rtx, rtx);
extern rtx        gen_negv2sf2                        (rtx, rtx);
extern rtx        gen_one_cmplsi2                     (rtx, rtx);
extern rtx        gen_one_cmpldi2                     (rtx, rtx);
extern rtx        gen_truncdfsf2                      (rtx, rtx);
extern rtx        gen_truncdiqi2                      (rtx, rtx);
extern rtx        gen_truncdihi2                      (rtx, rtx);
extern rtx        gen_truncdisi2                      (rtx, rtx);
extern rtx        gen_extendsidi2                     (rtx, rtx);
extern rtx        gen_extendsfdf2                     (rtx, rtx);
extern rtx        gen_fix_truncdfsi2_insn             (rtx, rtx);
extern rtx        gen_fix_truncdfsi2_macro            (rtx, rtx);
extern rtx        gen_fix_truncsfsi2_insn             (rtx, rtx);
extern rtx        gen_fix_truncsfsi2_macro            (rtx, rtx);
extern rtx        gen_fix_truncdfdi2                  (rtx, rtx);
extern rtx        gen_fix_truncsfdi2                  (rtx, rtx);
extern rtx        gen_floatsidf2                      (rtx, rtx);
extern rtx        gen_floatdidf2                      (rtx, rtx);
extern rtx        gen_floatsisf2                      (rtx, rtx);
extern rtx        gen_floatdisf2                      (rtx, rtx);
extern rtx        gen_mov_lwl                         (rtx, rtx, rtx);
extern rtx        gen_mov_ldl                         (rtx, rtx, rtx);
extern rtx        gen_mov_lwr                         (rtx, rtx, rtx, rtx);
extern rtx        gen_mov_ldr                         (rtx, rtx, rtx, rtx);
extern rtx        gen_mov_swl                         (rtx, rtx, rtx);
extern rtx        gen_mov_sdl                         (rtx, rtx, rtx);
extern rtx        gen_mov_swr                         (rtx, rtx, rtx);
extern rtx        gen_mov_sdr                         (rtx, rtx, rtx);
extern rtx        gen_load_gotsi                      (rtx, rtx, rtx);
extern rtx        gen_load_gotdi                      (rtx, rtx, rtx);
extern rtx        gen_movccf                          (rtx, rtx);
extern rtx        gen_mfhisi_di                       (rtx, rtx);
static inline rtx gen_mfhidi_di                       (rtx, rtx);
static inline rtx
gen_mfhidi_di(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b))
{
  return 0;
}
extern rtx        gen_mfhisi_ti                       (rtx, rtx);
extern rtx        gen_mfhidi_ti                       (rtx, rtx);
extern rtx        gen_mthisi_di                       (rtx, rtx, rtx);
static inline rtx gen_mthidi_di                       (rtx, rtx, rtx);
static inline rtx
gen_mthidi_di(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_mthisi_ti                       (rtx, rtx, rtx);
extern rtx        gen_mthidi_ti                       (rtx, rtx, rtx);
extern rtx        gen_load_lowdf                      (rtx, rtx);
extern rtx        gen_load_lowdi                      (rtx, rtx);
extern rtx        gen_load_lowv2sf                    (rtx, rtx);
extern rtx        gen_load_lowv2si                    (rtx, rtx);
extern rtx        gen_load_lowv4hi                    (rtx, rtx);
extern rtx        gen_load_lowv8qi                    (rtx, rtx);
extern rtx        gen_load_lowtf                      (rtx, rtx);
extern rtx        gen_load_highdf                     (rtx, rtx, rtx);
extern rtx        gen_load_highdi                     (rtx, rtx, rtx);
extern rtx        gen_load_highv2sf                   (rtx, rtx, rtx);
extern rtx        gen_load_highv2si                   (rtx, rtx, rtx);
extern rtx        gen_load_highv4hi                   (rtx, rtx, rtx);
extern rtx        gen_load_highv8qi                   (rtx, rtx, rtx);
extern rtx        gen_load_hightf                     (rtx, rtx, rtx);
extern rtx        gen_store_worddf                    (rtx, rtx, rtx);
extern rtx        gen_store_worddi                    (rtx, rtx, rtx);
extern rtx        gen_store_wordv2sf                  (rtx, rtx, rtx);
extern rtx        gen_store_wordv2si                  (rtx, rtx, rtx);
extern rtx        gen_store_wordv4hi                  (rtx, rtx, rtx);
extern rtx        gen_store_wordv8qi                  (rtx, rtx, rtx);
extern rtx        gen_store_wordtf                    (rtx, rtx, rtx);
extern rtx        gen_mthc1df                         (rtx, rtx, rtx);
extern rtx        gen_mthc1di                         (rtx, rtx, rtx);
extern rtx        gen_mthc1v2sf                       (rtx, rtx, rtx);
extern rtx        gen_mthc1v2si                       (rtx, rtx, rtx);
extern rtx        gen_mthc1v4hi                       (rtx, rtx, rtx);
extern rtx        gen_mthc1v8qi                       (rtx, rtx, rtx);
extern rtx        gen_mthc1tf                         (rtx, rtx, rtx);
extern rtx        gen_mfhc1df                         (rtx, rtx);
extern rtx        gen_mfhc1di                         (rtx, rtx);
extern rtx        gen_mfhc1v2sf                       (rtx, rtx);
extern rtx        gen_mfhc1v2si                       (rtx, rtx);
extern rtx        gen_mfhc1v4hi                       (rtx, rtx);
extern rtx        gen_mfhc1v8qi                       (rtx, rtx);
extern rtx        gen_mfhc1tf                         (rtx, rtx);
extern rtx        gen_loadgp_newabi_si                (rtx, rtx, rtx);
extern rtx        gen_loadgp_newabi_di                (rtx, rtx, rtx);
extern rtx        gen_loadgp_absolute_si              (rtx, rtx);
extern rtx        gen_loadgp_absolute_di              (rtx, rtx);
extern rtx        gen_loadgp_blockage                 (void);
extern rtx        gen_loadgp_rtp_si                   (rtx, rtx, rtx);
extern rtx        gen_loadgp_rtp_di                   (rtx, rtx, rtx);
extern rtx        gen_copygp_mips16_si                (rtx, rtx);
extern rtx        gen_copygp_mips16_di                (rtx, rtx);
extern rtx        gen_potential_cprestore_si          (rtx, rtx, rtx, rtx);
extern rtx        gen_potential_cprestore_di          (rtx, rtx, rtx, rtx);
extern rtx        gen_cprestore_si                    (rtx, rtx);
extern rtx        gen_cprestore_di                    (rtx, rtx);
extern rtx        gen_use_cprestore_si                (rtx);
extern rtx        gen_use_cprestore_di                (rtx);
extern rtx        gen_sync                            (void);
extern rtx        gen_synci                           (rtx);
extern rtx        gen_rdhwr_synci_step_si             (rtx);
extern rtx        gen_rdhwr_synci_step_di             (rtx);
extern rtx        gen_clear_hazard_si                 (void);
extern rtx        gen_clear_hazard_di                 (void);
extern rtx        gen_mips_cache                      (rtx, rtx);
extern rtx        gen_r10k_cache_barrier              (void);
extern rtx        gen_lsa                             (rtx, rtx, rtx, rtx);
extern rtx        gen_dlsa                            (rtx, rtx, rtx, rtx);
extern rtx        gen_rotrsi3                         (rtx, rtx, rtx);
extern rtx        gen_rotrdi3                         (rtx, rtx, rtx);
extern rtx        gen_bswaphi2                        (rtx, rtx);
extern rtx        gen_bswapsi2                        (rtx, rtx);
extern rtx        gen_bswapdi2                        (rtx, rtx);
extern rtx        gen_wsbh                            (rtx, rtx);
extern rtx        gen_dsbh                            (rtx, rtx);
extern rtx        gen_dshd                            (rtx, rtx);
extern rtx        gen_sunordered_sf_using_cc          (rtx, rtx, rtx);
extern rtx        gen_suneq_sf_using_cc               (rtx, rtx, rtx);
extern rtx        gen_sunlt_sf_using_cc               (rtx, rtx, rtx);
extern rtx        gen_sunle_sf_using_cc               (rtx, rtx, rtx);
extern rtx        gen_seq_sf_using_cc                 (rtx, rtx, rtx);
extern rtx        gen_slt_sf_using_cc                 (rtx, rtx, rtx);
extern rtx        gen_sle_sf_using_cc                 (rtx, rtx, rtx);
static inline rtx gen_sordered_sf_using_cc            (rtx, rtx, rtx);
static inline rtx
gen_sordered_sf_using_cc(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_sltgt_sf_using_cc               (rtx, rtx, rtx);
static inline rtx
gen_sltgt_sf_using_cc(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_sne_sf_using_cc                 (rtx, rtx, rtx);
static inline rtx
gen_sne_sf_using_cc(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_sunordered_df_using_cc          (rtx, rtx, rtx);
extern rtx        gen_suneq_df_using_cc               (rtx, rtx, rtx);
extern rtx        gen_sunlt_df_using_cc               (rtx, rtx, rtx);
extern rtx        gen_sunle_df_using_cc               (rtx, rtx, rtx);
extern rtx        gen_seq_df_using_cc                 (rtx, rtx, rtx);
extern rtx        gen_slt_df_using_cc                 (rtx, rtx, rtx);
extern rtx        gen_sle_df_using_cc                 (rtx, rtx, rtx);
static inline rtx gen_sordered_df_using_cc            (rtx, rtx, rtx);
static inline rtx
gen_sordered_df_using_cc(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_sltgt_df_using_cc               (rtx, rtx, rtx);
static inline rtx
gen_sltgt_df_using_cc(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
static inline rtx gen_sne_df_using_cc                 (rtx, rtx, rtx);
static inline rtx
gen_sne_df_using_cc(rtx ARG_UNUSED (a), rtx ARG_UNUSED (b), rtx ARG_UNUSED (c))
{
  return 0;
}
extern rtx        gen_sunordered_sf_using_ccf         (rtx, rtx, rtx);
extern rtx        gen_suneq_sf_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_sunlt_sf_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_sunle_sf_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_seq_sf_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_slt_sf_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_sle_sf_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_sordered_sf_using_ccf           (rtx, rtx, rtx);
extern rtx        gen_sltgt_sf_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_sne_sf_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_sunordered_df_using_ccf         (rtx, rtx, rtx);
extern rtx        gen_suneq_df_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_sunlt_df_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_sunle_df_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_seq_df_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_slt_df_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_sle_df_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_sordered_df_using_ccf           (rtx, rtx, rtx);
extern rtx        gen_sltgt_df_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_sne_df_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_sge_sf_using_cc                 (rtx, rtx, rtx);
extern rtx        gen_sgt_sf_using_cc                 (rtx, rtx, rtx);
extern rtx        gen_sunge_sf_using_cc               (rtx, rtx, rtx);
extern rtx        gen_sungt_sf_using_cc               (rtx, rtx, rtx);
extern rtx        gen_sge_df_using_cc                 (rtx, rtx, rtx);
extern rtx        gen_sgt_df_using_cc                 (rtx, rtx, rtx);
extern rtx        gen_sunge_df_using_cc               (rtx, rtx, rtx);
extern rtx        gen_sungt_df_using_cc               (rtx, rtx, rtx);
extern rtx        gen_sge_sf_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_sgt_sf_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_sunge_sf_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_sungt_sf_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_sge_df_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_sgt_df_using_ccf                (rtx, rtx, rtx);
extern rtx        gen_sunge_df_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_sungt_df_using_ccf              (rtx, rtx, rtx);
extern rtx        gen_indirect_jump_si                (rtx);
extern rtx        gen_indirect_jump_di                (rtx);
extern rtx        gen_indirect_jump_and_restore_si    (rtx, rtx, rtx);
extern rtx        gen_indirect_jump_and_restore_di    (rtx, rtx, rtx);
extern rtx        gen_tablejump_si                    (rtx, rtx);
extern rtx        gen_tablejump_di                    (rtx, rtx);
extern rtx        gen_casesi_internal_mips16_si       (rtx, rtx);
extern rtx        gen_casesi_internal_mips16_di       (rtx, rtx);
extern rtx        gen_blockage                        (void);
extern rtx        gen_probe_stack_range_si            (rtx, rtx, rtx);
extern rtx        gen_probe_stack_range_di            (rtx, rtx, rtx);
extern rtx        gen_return_internal                 (rtx);
extern rtx        gen_simple_return_internal          (rtx);
extern rtx        gen_mips_eret                       (void);
extern rtx        gen_mips_deret                      (void);
extern rtx        gen_mips_di                         (void);
extern rtx        gen_mips_ehb                        (void);
extern rtx        gen_mips_rdpgpr_si                  (rtx, rtx);
extern rtx        gen_mips_rdpgpr_di                  (rtx, rtx);
extern rtx        gen_cop0_move                       (rtx, rtx);
extern rtx        gen_eh_set_lr_si                    (rtx);
extern rtx        gen_eh_set_lr_di                    (rtx);
extern rtx        gen_restore_gp_si                   (void);
extern rtx        gen_restore_gp_di                   (void);
extern rtx        gen_move_gpsi                       (rtx, rtx);
extern rtx        gen_move_gpdi                       (rtx, rtx);
extern rtx        gen_load_callsi                     (rtx, rtx, rtx);
extern rtx        gen_load_calldi                     (rtx, rtx, rtx);
extern rtx        gen_set_got_version                 (void);
extern rtx        gen_update_got_version              (void);
extern rtx        gen_sibcall_internal                (rtx, rtx);
extern rtx        gen_sibcall_value_internal          (rtx, rtx, rtx);
extern rtx        gen_sibcall_value_multiple_internal (rtx, rtx, rtx, rtx);
extern rtx        gen_call_internal                   (rtx, rtx);
extern rtx        gen_call_split                      (rtx, rtx);
extern rtx        gen_call_internal_direct            (rtx, rtx);
extern rtx        gen_call_direct_split               (rtx, rtx);
extern rtx        gen_call_value_internal             (rtx, rtx, rtx);
extern rtx        gen_call_value_split                (rtx, rtx, rtx);
extern rtx        gen_call_value_internal_direct      (rtx, rtx, rtx);
extern rtx        gen_call_value_direct_split         (rtx, rtx, rtx);
extern rtx        gen_call_value_multiple_internal    (rtx, rtx, rtx, rtx);
extern rtx        gen_call_value_multiple_split       (rtx, rtx, rtx, rtx);
extern rtx        gen_prefetch                        (rtx, rtx, rtx);
extern rtx        gen_nop                             (void);
extern rtx        gen_hazard_nop                      (void);
extern rtx        gen_insn_pseudo                     (void);
extern rtx        gen_consttable                      (rtx);
extern rtx        gen_consttable_end                  (rtx);
extern rtx        gen_consttable_tls_reloc            (rtx, rtx);
extern rtx        gen_consttable_int                  (rtx, rtx);
extern rtx        gen_consttable_float                (rtx);
extern rtx        gen_align                           (rtx);
extern rtx        gen_tls_get_tp_si                   (rtx);
extern rtx        gen_tls_get_tp_di                   (rtx);
extern rtx        gen_tls_get_tp_mips16_si            (rtx, rtx);
extern rtx        gen_tls_get_tp_mips16_di            (rtx, rtx);
extern rtx        gen_mips_get_fcsr_mips16_si         (rtx);
extern rtx        gen_mips_get_fcsr_mips16_di         (rtx);
extern rtx        gen_mips_set_fcsr_mips16_si         (rtx);
extern rtx        gen_mips_set_fcsr_mips16_di         (rtx);
extern rtx        gen_sync_compare_and_swapsi         (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_compare_and_swapdi         (rtx, rtx, rtx, rtx);
extern rtx        gen_compare_and_swap_12             (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_addsi                      (rtx, rtx);
extern rtx        gen_sync_adddi                      (rtx, rtx);
extern rtx        gen_sync_add_12                     (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_sub_12                     (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_ior_12                     (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_xor_12                     (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_and_12                     (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_old_add_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_old_sub_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_old_ior_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_old_xor_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_old_and_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_new_add_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_new_sub_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_new_ior_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_new_xor_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_new_and_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_nand_12                    (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_old_nand_12                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_new_nand_12                (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sync_subsi                      (rtx, rtx);
extern rtx        gen_sync_subdi                      (rtx, rtx);
extern rtx        gen_sync_old_addsi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_adddi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_subsi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_subdi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_addsi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_adddi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_subsi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_subdi                  (rtx, rtx, rtx);
extern rtx        gen_sync_iorsi                      (rtx, rtx);
extern rtx        gen_sync_xorsi                      (rtx, rtx);
extern rtx        gen_sync_andsi                      (rtx, rtx);
extern rtx        gen_sync_iordi                      (rtx, rtx);
extern rtx        gen_sync_xordi                      (rtx, rtx);
extern rtx        gen_sync_anddi                      (rtx, rtx);
extern rtx        gen_sync_old_iorsi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_xorsi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_andsi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_iordi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_xordi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_anddi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_iorsi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_xorsi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_andsi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_iordi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_xordi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_anddi                  (rtx, rtx, rtx);
extern rtx        gen_sync_nandsi                     (rtx, rtx);
extern rtx        gen_sync_nanddi                     (rtx, rtx);
extern rtx        gen_sync_old_nandsi                 (rtx, rtx, rtx);
extern rtx        gen_sync_old_nanddi                 (rtx, rtx, rtx);
extern rtx        gen_sync_new_nandsi                 (rtx, rtx, rtx);
extern rtx        gen_sync_new_nanddi                 (rtx, rtx, rtx);
extern rtx        gen_sync_lock_test_and_setsi        (rtx, rtx, rtx);
extern rtx        gen_sync_lock_test_and_setdi        (rtx, rtx, rtx);
extern rtx        gen_test_and_set_12                 (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_compare_and_swapsi       (rtx, rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_compare_and_swapdi       (rtx, rtx, rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_exchangesi_llsc          (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_exchangedi_llsc          (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_exchangesi_swap          (rtx, rtx, rtx);
extern rtx        gen_atomic_exchangedi_swap          (rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_addsi_llsc         (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_adddi_llsc         (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_addsi_ldadd        (rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_adddi_ldadd        (rtx, rtx, rtx);
extern rtx        gen_mips_cond_move_tf_ps            (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_perm_const_ps               (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vec_concatv2sf                  (rtx, rtx, rtx);
extern rtx        gen_vec_extractv2sfsf               (rtx, rtx, rtx);
extern rtx        gen_mips_alnv_ps                    (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_addr_ps                    (rtx, rtx, rtx);
extern rtx        gen_mips_cvt_pw_ps                  (rtx, rtx);
extern rtx        gen_mips_cvt_ps_pw                  (rtx, rtx);
extern rtx        gen_mips_mulr_ps                    (rtx, rtx, rtx);
extern rtx        gen_mips_cabs_cond_s                (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_cabs_cond_d                (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_c_cond_4s                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_mips_cabs_cond_4s               (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_mips_c_cond_ps                  (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_cabs_cond_ps               (rtx, rtx, rtx, rtx);
extern rtx        gen_sunordered_ps                   (rtx, rtx, rtx);
extern rtx        gen_suneq_ps                        (rtx, rtx, rtx);
extern rtx        gen_sunlt_ps                        (rtx, rtx, rtx);
extern rtx        gen_sunle_ps                        (rtx, rtx, rtx);
extern rtx        gen_seq_ps                          (rtx, rtx, rtx);
extern rtx        gen_slt_ps                          (rtx, rtx, rtx);
extern rtx        gen_sle_ps                          (rtx, rtx, rtx);
extern rtx        gen_sordered_ps                     (rtx, rtx, rtx);
extern rtx        gen_sltgt_ps                        (rtx, rtx, rtx);
extern rtx        gen_sne_ps                          (rtx, rtx, rtx);
extern rtx        gen_sge_ps                          (rtx, rtx, rtx);
extern rtx        gen_sgt_ps                          (rtx, rtx, rtx);
extern rtx        gen_sunge_ps                        (rtx, rtx, rtx);
extern rtx        gen_sungt_ps                        (rtx, rtx, rtx);
extern rtx        gen_bc1any4t                        (rtx, rtx);
extern rtx        gen_bc1any4f                        (rtx, rtx);
extern rtx        gen_bc1any2t                        (rtx, rtx);
extern rtx        gen_bc1any2f                        (rtx, rtx);
extern rtx        gen_mips_rsqrt1_s                   (rtx, rtx);
extern rtx        gen_mips_rsqrt1_d                   (rtx, rtx);
extern rtx        gen_mips_rsqrt1_ps                  (rtx, rtx);
extern rtx        gen_mips_rsqrt2_s                   (rtx, rtx, rtx);
extern rtx        gen_mips_rsqrt2_d                   (rtx, rtx, rtx);
extern rtx        gen_mips_rsqrt2_ps                  (rtx, rtx, rtx);
extern rtx        gen_mips_recip1_s                   (rtx, rtx);
extern rtx        gen_mips_recip1_d                   (rtx, rtx);
extern rtx        gen_mips_recip1_ps                  (rtx, rtx);
extern rtx        gen_mips_recip2_s                   (rtx, rtx, rtx);
extern rtx        gen_mips_recip2_d                   (rtx, rtx, rtx);
extern rtx        gen_mips_recip2_ps                  (rtx, rtx, rtx);
extern rtx        gen_addv2hi3                        (rtx, rtx, rtx);
extern rtx        gen_addv4qi3                        (rtx, rtx, rtx);
extern rtx        gen_mips_addq_s_w                   (rtx, rtx, rtx);
extern rtx        gen_mips_addq_s_ph                  (rtx, rtx, rtx);
extern rtx        gen_mips_addu_s_qb                  (rtx, rtx, rtx);
extern rtx        gen_subv2hi3                        (rtx, rtx, rtx);
extern rtx        gen_subv4qi3                        (rtx, rtx, rtx);
extern rtx        gen_mips_subq_s_w                   (rtx, rtx, rtx);
extern rtx        gen_mips_subq_s_ph                  (rtx, rtx, rtx);
extern rtx        gen_mips_subu_s_qb                  (rtx, rtx, rtx);
extern rtx        gen_mips_addsc                      (rtx, rtx, rtx);
extern rtx        gen_mips_addwc                      (rtx, rtx, rtx);
extern rtx        gen_mips_modsub                     (rtx, rtx, rtx);
extern rtx        gen_mips_raddu_w_qb                 (rtx, rtx);
extern rtx        gen_mips_absq_s_w                   (rtx, rtx);
extern rtx        gen_mips_absq_s_ph                  (rtx, rtx);
extern rtx        gen_mips_precrq_qb_ph               (rtx, rtx, rtx);
extern rtx        gen_mips_precrq_ph_w                (rtx, rtx, rtx);
extern rtx        gen_mips_precrq_rs_ph_w             (rtx, rtx, rtx);
extern rtx        gen_mips_precrqu_s_qb_ph            (rtx, rtx, rtx);
extern rtx        gen_mips_preceq_w_phl               (rtx, rtx);
extern rtx        gen_mips_preceq_w_phr               (rtx, rtx);
extern rtx        gen_mips_precequ_ph_qbl             (rtx, rtx);
extern rtx        gen_mips_precequ_ph_qbr             (rtx, rtx);
extern rtx        gen_mips_precequ_ph_qbla            (rtx, rtx);
extern rtx        gen_mips_precequ_ph_qbra            (rtx, rtx);
extern rtx        gen_mips_preceu_ph_qbl              (rtx, rtx);
extern rtx        gen_mips_preceu_ph_qbr              (rtx, rtx);
extern rtx        gen_mips_preceu_ph_qbla             (rtx, rtx);
extern rtx        gen_mips_preceu_ph_qbra             (rtx, rtx);
extern rtx        gen_mips_shll_ph                    (rtx, rtx, rtx);
extern rtx        gen_mips_shll_qb                    (rtx, rtx, rtx);
extern rtx        gen_mips_shll_s_w                   (rtx, rtx, rtx);
extern rtx        gen_mips_shll_s_ph                  (rtx, rtx, rtx);
extern rtx        gen_mips_shrl_qb                    (rtx, rtx, rtx);
extern rtx        gen_mips_shra_ph                    (rtx, rtx, rtx);
extern rtx        gen_mips_shra_r_w                   (rtx, rtx, rtx);
extern rtx        gen_mips_shra_r_ph                  (rtx, rtx, rtx);
extern rtx        gen_mips_muleu_s_ph_qbl             (rtx, rtx, rtx);
extern rtx        gen_mips_muleu_s_ph_qbr             (rtx, rtx, rtx);
extern rtx        gen_mips_mulq_rs_ph                 (rtx, rtx, rtx);
extern rtx        gen_mips_muleq_s_w_phl              (rtx, rtx, rtx);
extern rtx        gen_mips_muleq_s_w_phr              (rtx, rtx, rtx);
extern rtx        gen_mips_dpau_h_qbl                 (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpau_h_qbr                 (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpsu_h_qbl                 (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpsu_h_qbr                 (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpaq_s_w_ph                (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpsq_s_w_ph                (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_mulsaq_s_w_ph              (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpaq_sa_l_w                (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpsq_sa_l_w                (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_maq_s_w_phl                (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_maq_s_w_phr                (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_maq_sa_w_phl               (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_maq_sa_w_phr               (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_bitrev                     (rtx, rtx);
extern rtx        gen_mips_insv                       (rtx, rtx, rtx);
extern rtx        gen_mips_repl_qb                    (rtx, rtx);
extern rtx        gen_mips_repl_ph                    (rtx, rtx);
extern rtx        gen_mips_cmp_eq_ph                  (rtx, rtx);
extern rtx        gen_mips_cmpu_eq_qb                 (rtx, rtx);
extern rtx        gen_mips_cmp_lt_ph                  (rtx, rtx);
extern rtx        gen_mips_cmpu_lt_qb                 (rtx, rtx);
extern rtx        gen_mips_cmp_le_ph                  (rtx, rtx);
extern rtx        gen_mips_cmpu_le_qb                 (rtx, rtx);
extern rtx        gen_mips_cmpgu_eq_qb                (rtx, rtx, rtx);
extern rtx        gen_mips_cmpgu_lt_qb                (rtx, rtx, rtx);
extern rtx        gen_mips_cmpgu_le_qb                (rtx, rtx, rtx);
extern rtx        gen_mips_pick_ph                    (rtx, rtx, rtx);
extern rtx        gen_mips_pick_qb                    (rtx, rtx, rtx);
extern rtx        gen_mips_packrl_ph                  (rtx, rtx, rtx);
extern rtx        gen_mips_extr_w                     (rtx, rtx, rtx);
extern rtx        gen_mips_extr_r_w                   (rtx, rtx, rtx);
extern rtx        gen_mips_extr_rs_w                  (rtx, rtx, rtx);
extern rtx        gen_mips_extr_s_h                   (rtx, rtx, rtx);
extern rtx        gen_mips_extp                       (rtx, rtx, rtx);
extern rtx        gen_mips_extpdp                     (rtx, rtx, rtx);
extern rtx        gen_mips_shilo                      (rtx, rtx, rtx);
extern rtx        gen_mips_mthlip                     (rtx, rtx, rtx);
extern rtx        gen_mips_wrdsp                      (rtx, rtx);
extern rtx        gen_mips_rddsp                      (rtx, rtx);
extern rtx        gen_mips_lbx_extsi_si               (rtx, rtx, rtx);
extern rtx        gen_mips_lbux_extsi_si              (rtx, rtx, rtx);
extern rtx        gen_mips_lbx_extdi_si               (rtx, rtx, rtx);
extern rtx        gen_mips_lbux_extdi_si              (rtx, rtx, rtx);
extern rtx        gen_mips_lhx_extsi_si               (rtx, rtx, rtx);
extern rtx        gen_mips_lhux_extsi_si              (rtx, rtx, rtx);
extern rtx        gen_mips_lhx_extdi_si               (rtx, rtx, rtx);
extern rtx        gen_mips_lhux_extdi_si              (rtx, rtx, rtx);
extern rtx        gen_mips_lbx_extsi_di               (rtx, rtx, rtx);
extern rtx        gen_mips_lbux_extsi_di              (rtx, rtx, rtx);
extern rtx        gen_mips_lbx_extdi_di               (rtx, rtx, rtx);
extern rtx        gen_mips_lbux_extdi_di              (rtx, rtx, rtx);
extern rtx        gen_mips_lhx_extsi_di               (rtx, rtx, rtx);
extern rtx        gen_mips_lhux_extsi_di              (rtx, rtx, rtx);
extern rtx        gen_mips_lhx_extdi_di               (rtx, rtx, rtx);
extern rtx        gen_mips_lhux_extdi_di              (rtx, rtx, rtx);
extern rtx        gen_mips_lwx_si                     (rtx, rtx, rtx);
extern rtx        gen_mips_ldx_si                     (rtx, rtx, rtx);
extern rtx        gen_mips_lwx_di                     (rtx, rtx, rtx);
extern rtx        gen_mips_ldx_di                     (rtx, rtx, rtx);
extern rtx        gen_mips_bposge                     (rtx, rtx);
extern rtx        gen_mips_absq_s_qb                  (rtx, rtx);
extern rtx        gen_mips_addu_ph                    (rtx, rtx, rtx);
extern rtx        gen_mips_addu_s_ph                  (rtx, rtx, rtx);
extern rtx        gen_mips_adduh_qb                   (rtx, rtx, rtx);
extern rtx        gen_mips_adduh_r_qb                 (rtx, rtx, rtx);
extern rtx        gen_mips_append                     (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_balign                     (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_cmpgdu_eq_qb               (rtx, rtx, rtx);
extern rtx        gen_mips_cmpgdu_lt_qb               (rtx, rtx, rtx);
extern rtx        gen_mips_cmpgdu_le_qb               (rtx, rtx, rtx);
extern rtx        gen_mips_dpa_w_ph                   (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dps_w_ph                   (rtx, rtx, rtx, rtx);
extern rtx        gen_mulv2hi3                        (rtx, rtx, rtx);
extern rtx        gen_mips_mul_s_ph                   (rtx, rtx, rtx);
extern rtx        gen_mips_mulq_rs_w                  (rtx, rtx, rtx);
extern rtx        gen_mips_mulq_s_ph                  (rtx, rtx, rtx);
extern rtx        gen_mips_mulq_s_w                   (rtx, rtx, rtx);
extern rtx        gen_mips_mulsa_w_ph                 (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_precr_qb_ph                (rtx, rtx, rtx);
extern rtx        gen_mips_precr_sra_ph_w             (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_precr_sra_r_ph_w           (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_prepend                    (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_shra_qb                    (rtx, rtx, rtx);
extern rtx        gen_mips_shra_r_qb                  (rtx, rtx, rtx);
extern rtx        gen_mips_shrl_ph                    (rtx, rtx, rtx);
extern rtx        gen_mips_subu_ph                    (rtx, rtx, rtx);
extern rtx        gen_mips_subu_s_ph                  (rtx, rtx, rtx);
extern rtx        gen_mips_subuh_qb                   (rtx, rtx, rtx);
extern rtx        gen_mips_subuh_r_qb                 (rtx, rtx, rtx);
extern rtx        gen_mips_addqh_ph                   (rtx, rtx, rtx);
extern rtx        gen_mips_addqh_r_ph                 (rtx, rtx, rtx);
extern rtx        gen_mips_addqh_w                    (rtx, rtx, rtx);
extern rtx        gen_mips_addqh_r_w                  (rtx, rtx, rtx);
extern rtx        gen_mips_subqh_ph                   (rtx, rtx, rtx);
extern rtx        gen_mips_subqh_r_ph                 (rtx, rtx, rtx);
extern rtx        gen_mips_subqh_w                    (rtx, rtx, rtx);
extern rtx        gen_mips_subqh_r_w                  (rtx, rtx, rtx);
extern rtx        gen_mips_dpax_w_ph                  (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpsx_w_ph                  (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpaqx_s_w_ph               (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpaqx_sa_w_ph              (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpsqx_s_w_ph               (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_dpsqx_sa_w_ph              (rtx, rtx, rtx, rtx);
extern rtx        gen_addqq3                          (rtx, rtx, rtx);
extern rtx        gen_addhq3                          (rtx, rtx, rtx);
extern rtx        gen_addsq3                          (rtx, rtx, rtx);
extern rtx        gen_adddq3                          (rtx, rtx, rtx);
extern rtx        gen_adduqq3                         (rtx, rtx, rtx);
extern rtx        gen_adduhq3                         (rtx, rtx, rtx);
extern rtx        gen_addusq3                         (rtx, rtx, rtx);
extern rtx        gen_addudq3                         (rtx, rtx, rtx);
extern rtx        gen_addha3                          (rtx, rtx, rtx);
extern rtx        gen_addsa3                          (rtx, rtx, rtx);
extern rtx        gen_addda3                          (rtx, rtx, rtx);
extern rtx        gen_adduha3                         (rtx, rtx, rtx);
extern rtx        gen_addusa3                         (rtx, rtx, rtx);
extern rtx        gen_adduda3                         (rtx, rtx, rtx);
extern rtx        gen_usadduqq3                       (rtx, rtx, rtx);
extern rtx        gen_usadduhq3                       (rtx, rtx, rtx);
extern rtx        gen_usadduha3                       (rtx, rtx, rtx);
extern rtx        gen_usaddv4uqq3                     (rtx, rtx, rtx);
extern rtx        gen_usaddv2uhq3                     (rtx, rtx, rtx);
extern rtx        gen_usaddv2uha3                     (rtx, rtx, rtx);
extern rtx        gen_ssaddhq3                        (rtx, rtx, rtx);
extern rtx        gen_ssaddsq3                        (rtx, rtx, rtx);
extern rtx        gen_ssaddha3                        (rtx, rtx, rtx);
extern rtx        gen_ssaddsa3                        (rtx, rtx, rtx);
extern rtx        gen_ssaddv2hq3                      (rtx, rtx, rtx);
extern rtx        gen_ssaddv2ha3                      (rtx, rtx, rtx);
extern rtx        gen_subqq3                          (rtx, rtx, rtx);
extern rtx        gen_subhq3                          (rtx, rtx, rtx);
extern rtx        gen_subsq3                          (rtx, rtx, rtx);
extern rtx        gen_subdq3                          (rtx, rtx, rtx);
extern rtx        gen_subuqq3                         (rtx, rtx, rtx);
extern rtx        gen_subuhq3                         (rtx, rtx, rtx);
extern rtx        gen_subusq3                         (rtx, rtx, rtx);
extern rtx        gen_subudq3                         (rtx, rtx, rtx);
extern rtx        gen_subha3                          (rtx, rtx, rtx);
extern rtx        gen_subsa3                          (rtx, rtx, rtx);
extern rtx        gen_subda3                          (rtx, rtx, rtx);
extern rtx        gen_subuha3                         (rtx, rtx, rtx);
extern rtx        gen_subusa3                         (rtx, rtx, rtx);
extern rtx        gen_subuda3                         (rtx, rtx, rtx);
extern rtx        gen_ussubuqq3                       (rtx, rtx, rtx);
extern rtx        gen_ussubuhq3                       (rtx, rtx, rtx);
extern rtx        gen_ussubuha3                       (rtx, rtx, rtx);
extern rtx        gen_ussubv4uqq3                     (rtx, rtx, rtx);
extern rtx        gen_ussubv2uhq3                     (rtx, rtx, rtx);
extern rtx        gen_ussubv2uha3                     (rtx, rtx, rtx);
extern rtx        gen_sssubhq3                        (rtx, rtx, rtx);
extern rtx        gen_sssubsq3                        (rtx, rtx, rtx);
extern rtx        gen_sssubha3                        (rtx, rtx, rtx);
extern rtx        gen_sssubsa3                        (rtx, rtx, rtx);
extern rtx        gen_sssubv2hq3                      (rtx, rtx, rtx);
extern rtx        gen_sssubv2ha3                      (rtx, rtx, rtx);
extern rtx        gen_ssmulv2hq3                      (rtx, rtx, rtx);
extern rtx        gen_ssmulhq3                        (rtx, rtx, rtx);
extern rtx        gen_ssmulsq3                        (rtx, rtx, rtx);
extern rtx        gen_ssmaddsqdq4                     (rtx, rtx, rtx, rtx);
extern rtx        gen_ssmsubsqdq4                     (rtx, rtx, rtx, rtx);
extern rtx        gen_jraddiusp                       (rtx);
extern rtx        gen_movv2si_internal                (rtx, rtx);
extern rtx        gen_movv4hi_internal                (rtx, rtx);
extern rtx        gen_movv8qi_internal                (rtx, rtx);
extern rtx        gen_loongson_vec_init1_v4hi         (rtx, rtx);
extern rtx        gen_loongson_vec_init1_v8qi         (rtx, rtx);
extern rtx        gen_vec_pack_ssat_v2si              (rtx, rtx, rtx);
extern rtx        gen_vec_pack_ssat_v4hi              (rtx, rtx, rtx);
extern rtx        gen_vec_pack_usat_v4hi              (rtx, rtx, rtx);
extern rtx        gen_addv2si3                        (rtx, rtx, rtx);
extern rtx        gen_addv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_addv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_loongson_paddd                  (rtx, rtx, rtx);
extern rtx        gen_ssaddv4hi3                      (rtx, rtx, rtx);
extern rtx        gen_ssaddv8qi3                      (rtx, rtx, rtx);
extern rtx        gen_usaddv4hi3                      (rtx, rtx, rtx);
extern rtx        gen_usaddv8qi3                      (rtx, rtx, rtx);
extern rtx        gen_loongson_pandn_w                (rtx, rtx, rtx);
extern rtx        gen_loongson_pandn_h                (rtx, rtx, rtx);
extern rtx        gen_loongson_pandn_b                (rtx, rtx, rtx);
extern rtx        gen_loongson_pandn_d                (rtx, rtx, rtx);
extern rtx        gen_andv2si3                        (rtx, rtx, rtx);
extern rtx        gen_andv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_andv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_iorv2si3                        (rtx, rtx, rtx);
extern rtx        gen_iorv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_iorv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_xorv2si3                        (rtx, rtx, rtx);
extern rtx        gen_xorv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_xorv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_one_cmplv2si2                   (rtx, rtx);
extern rtx        gen_one_cmplv4hi2                   (rtx, rtx);
extern rtx        gen_one_cmplv8qi2                   (rtx, rtx);
extern rtx        gen_loongson_pavgh                  (rtx, rtx, rtx);
extern rtx        gen_loongson_pavgb                  (rtx, rtx, rtx);
extern rtx        gen_loongson_pcmpeqw                (rtx, rtx, rtx);
extern rtx        gen_loongson_pcmpeqh                (rtx, rtx, rtx);
extern rtx        gen_loongson_pcmpeqb                (rtx, rtx, rtx);
extern rtx        gen_loongson_pcmpgtw                (rtx, rtx, rtx);
extern rtx        gen_loongson_pcmpgth                (rtx, rtx, rtx);
extern rtx        gen_loongson_pcmpgtb                (rtx, rtx, rtx);
extern rtx        gen_loongson_pextrh                 (rtx, rtx, rtx);
extern rtx        gen_loongson_pinsrh_0               (rtx, rtx, rtx);
extern rtx        gen_loongson_pinsrh_1               (rtx, rtx, rtx);
extern rtx        gen_loongson_pinsrh_2               (rtx, rtx, rtx);
extern rtx        gen_loongson_pinsrh_3               (rtx, rtx, rtx);
extern rtx        gen_loongson_pmaddhw                (rtx, rtx, rtx);
extern rtx        gen_smaxv4hi3                       (rtx, rtx, rtx);
extern rtx        gen_umaxv8qi3                       (rtx, rtx, rtx);
extern rtx        gen_sminv4hi3                       (rtx, rtx, rtx);
extern rtx        gen_uminv8qi3                       (rtx, rtx, rtx);
extern rtx        gen_loongson_pmovmskb               (rtx, rtx);
extern rtx        gen_umulv4hi3_highpart              (rtx, rtx, rtx);
extern rtx        gen_smulv4hi3_highpart              (rtx, rtx, rtx);
extern rtx        gen_mulv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_loongson_pmuluw                 (rtx, rtx, rtx);
extern rtx        gen_loongson_pasubub                (rtx, rtx, rtx);
extern rtx        gen_loongson_biadd                  (rtx, rtx);
extern rtx        gen_reduc_uplus_v8qi                (rtx, rtx);
extern rtx        gen_loongson_psadbh                 (rtx, rtx, rtx);
extern rtx        gen_loongson_pshufh                 (rtx, rtx, rtx);
extern rtx        gen_ashlv2si3                       (rtx, rtx, rtx);
extern rtx        gen_ashlv4hi3                       (rtx, rtx, rtx);
extern rtx        gen_ashrv2si3                       (rtx, rtx, rtx);
extern rtx        gen_ashrv4hi3                       (rtx, rtx, rtx);
extern rtx        gen_lshrv2si3                       (rtx, rtx, rtx);
extern rtx        gen_lshrv4hi3                       (rtx, rtx, rtx);
extern rtx        gen_subv2si3                        (rtx, rtx, rtx);
extern rtx        gen_subv4hi3                        (rtx, rtx, rtx);
extern rtx        gen_subv8qi3                        (rtx, rtx, rtx);
extern rtx        gen_loongson_psubd                  (rtx, rtx, rtx);
extern rtx        gen_sssubv4hi3                      (rtx, rtx, rtx);
extern rtx        gen_sssubv8qi3                      (rtx, rtx, rtx);
extern rtx        gen_ussubv4hi3                      (rtx, rtx, rtx);
extern rtx        gen_ussubv8qi3                      (rtx, rtx, rtx);
extern rtx        gen_loongson_punpckhbh              (rtx, rtx, rtx);
extern rtx        gen_loongson_punpckhhw              (rtx, rtx, rtx);
extern rtx        gen_loongson_punpckhhw_qi           (rtx, rtx, rtx);
extern rtx        gen_loongson_punpckhwd              (rtx, rtx, rtx);
extern rtx        gen_loongson_punpckhwd_qi           (rtx, rtx, rtx);
extern rtx        gen_loongson_punpckhwd_hi           (rtx, rtx, rtx);
extern rtx        gen_loongson_punpcklbh              (rtx, rtx, rtx);
extern rtx        gen_loongson_punpcklhw              (rtx, rtx, rtx);
extern rtx        gen_loongson_punpcklwd              (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v2si                    (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v4hi                    (rtx, rtx, rtx);
extern rtx        gen_vec_shl_v8qi                    (rtx, rtx, rtx);
extern rtx        gen_vec_shl_di                      (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v2si                    (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v4hi                    (rtx, rtx, rtx);
extern rtx        gen_vec_shr_v8qi                    (rtx, rtx, rtx);
extern rtx        gen_vec_shr_di                      (rtx, rtx, rtx);
extern rtx        gen_vec_loongson_extract_lo_v2si    (rtx, rtx);
extern rtx        gen_vec_loongson_extract_lo_v4hi    (rtx, rtx);
extern rtx        gen_vec_loongson_extract_lo_v8qi    (rtx, rtx);
extern rtx        gen_vec_pack_trunc_v2di             (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v4si             (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v8hi             (rtx, rtx, rtx);
extern rtx        gen_msa_vec_extract_d_f             (rtx, rtx);
extern rtx        gen_msa_vec_extract_w_f             (rtx, rtx);
extern rtx        gen_msa_insert_d_f                  (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insert_w_f                  (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insert_d                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insert_w                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insert_h                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insert_b                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insve_d_f                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insve_w_f                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insve_d                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insve_w                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insve_h                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insve_b                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insve_d_f_scalar            (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_insve_w_f_scalar            (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_copy_s_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_copy_u_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_copy_s_b                    (rtx, rtx, rtx);
extern rtx        gen_msa_copy_u_b                    (rtx, rtx, rtx);
extern rtx        gen_msa_copy_u_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_copy_s_w_64bit              (rtx, rtx, rtx);
extern rtx        gen_msa_copy_s_w_f_64bit            (rtx, rtx, rtx);
extern rtx        gen_msa_copy_s_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_copy_s_w_f                  (rtx, rtx, rtx);
extern rtx        gen_msa_copy_s_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_copy_s_d_f                  (rtx, rtx, rtx);
extern rtx        gen_vec_permv2df                    (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_permv4sf                    (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_permv2di                    (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_permv4si                    (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_permv8hi                    (rtx, rtx, rtx, rtx);
extern rtx        gen_vec_permv16qi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_movv2df_msa                     (rtx, rtx);
extern rtx        gen_movv4sf_msa                     (rtx, rtx);
extern rtx        gen_movv2di_msa                     (rtx, rtx);
extern rtx        gen_movv4si_msa                     (rtx, rtx);
extern rtx        gen_movv8hi_msa                     (rtx, rtx);
extern rtx        gen_movv16qi_msa                    (rtx, rtx);
extern rtx        gen_addv2di3                        (rtx, rtx, rtx);
extern rtx        gen_addv4si3                        (rtx, rtx, rtx);
extern rtx        gen_addv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_addv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_subv2di3                        (rtx, rtx, rtx);
extern rtx        gen_subv4si3                        (rtx, rtx, rtx);
extern rtx        gen_subv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_subv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_mulv2di3                        (rtx, rtx, rtx);
extern rtx        gen_mulv4si3                        (rtx, rtx, rtx);
extern rtx        gen_mulv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_mulv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_msa_maddv_d                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_maddv_w                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_maddv_h                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_maddv_b                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_msubv_d                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_msubv_w                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_msubv_h                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_msubv_b                     (rtx, rtx, rtx, rtx);
extern rtx        gen_divv2di3                        (rtx, rtx, rtx);
extern rtx        gen_divv4si3                        (rtx, rtx, rtx);
extern rtx        gen_divv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_divv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_udivv2di3                       (rtx, rtx, rtx);
extern rtx        gen_udivv4si3                       (rtx, rtx, rtx);
extern rtx        gen_udivv8hi3                       (rtx, rtx, rtx);
extern rtx        gen_udivv16qi3                      (rtx, rtx, rtx);
extern rtx        gen_modv2di3                        (rtx, rtx, rtx);
extern rtx        gen_modv4si3                        (rtx, rtx, rtx);
extern rtx        gen_modv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_modv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_umodv2di3                       (rtx, rtx, rtx);
extern rtx        gen_umodv4si3                       (rtx, rtx, rtx);
extern rtx        gen_umodv8hi3                       (rtx, rtx, rtx);
extern rtx        gen_umodv16qi3                      (rtx, rtx, rtx);
extern rtx        gen_xorv2di3                        (rtx, rtx, rtx);
extern rtx        gen_xorv4si3                        (rtx, rtx, rtx);
extern rtx        gen_xorv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_xorv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_iorv2di3                        (rtx, rtx, rtx);
extern rtx        gen_iorv4si3                        (rtx, rtx, rtx);
extern rtx        gen_iorv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_iorv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_andv2di3                        (rtx, rtx, rtx);
extern rtx        gen_andv4si3                        (rtx, rtx, rtx);
extern rtx        gen_andv8hi3                        (rtx, rtx, rtx);
extern rtx        gen_andv16qi3                       (rtx, rtx, rtx);
extern rtx        gen_one_cmplv2di2                   (rtx, rtx);
extern rtx        gen_one_cmplv4si2                   (rtx, rtx);
extern rtx        gen_one_cmplv8hi2                   (rtx, rtx);
extern rtx        gen_one_cmplv16qi2                  (rtx, rtx);
extern rtx        gen_vlshrv2di3                      (rtx, rtx, rtx);
extern rtx        gen_vlshrv4si3                      (rtx, rtx, rtx);
extern rtx        gen_vlshrv8hi3                      (rtx, rtx, rtx);
extern rtx        gen_vlshrv16qi3                     (rtx, rtx, rtx);
extern rtx        gen_vashrv2di3                      (rtx, rtx, rtx);
extern rtx        gen_vashrv4si3                      (rtx, rtx, rtx);
extern rtx        gen_vashrv8hi3                      (rtx, rtx, rtx);
extern rtx        gen_vashrv16qi3                     (rtx, rtx, rtx);
extern rtx        gen_vashlv2di3                      (rtx, rtx, rtx);
extern rtx        gen_vashlv4si3                      (rtx, rtx, rtx);
extern rtx        gen_vashlv8hi3                      (rtx, rtx, rtx);
extern rtx        gen_vashlv16qi3                     (rtx, rtx, rtx);
extern rtx        gen_addv2df3                        (rtx, rtx, rtx);
extern rtx        gen_addv4sf3                        (rtx, rtx, rtx);
extern rtx        gen_subv2df3                        (rtx, rtx, rtx);
extern rtx        gen_subv4sf3                        (rtx, rtx, rtx);
extern rtx        gen_mulv2df3                        (rtx, rtx, rtx);
extern rtx        gen_mulv4sf3                        (rtx, rtx, rtx);
extern rtx        gen_divv2df3                        (rtx, rtx, rtx);
extern rtx        gen_divv4sf3                        (rtx, rtx, rtx);
extern rtx        gen_fmav2df4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_fmav4sf4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmav2df4                       (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmav4sf4                       (rtx, rtx, rtx, rtx);
extern rtx        gen_sqrtv2df2                       (rtx, rtx);
extern rtx        gen_sqrtv4sf2                       (rtx, rtx);
extern rtx        gen_msa_add_a_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_add_a_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_add_a_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_add_a_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_adds_a_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_adds_a_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_adds_a_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_adds_a_b                    (rtx, rtx, rtx);
extern rtx        gen_ssaddv2di3                      (rtx, rtx, rtx);
extern rtx        gen_ssaddv4si3                      (rtx, rtx, rtx);
extern rtx        gen_ssaddv8hi3                      (rtx, rtx, rtx);
extern rtx        gen_ssaddv16qi3                     (rtx, rtx, rtx);
extern rtx        gen_usaddv2di3                      (rtx, rtx, rtx);
extern rtx        gen_usaddv4si3                      (rtx, rtx, rtx);
extern rtx        gen_usaddv8hi3                      (rtx, rtx, rtx);
extern rtx        gen_usaddv16qi3                     (rtx, rtx, rtx);
extern rtx        gen_msa_asub_s_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_asub_s_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_asub_s_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_asub_s_b                    (rtx, rtx, rtx);
extern rtx        gen_msa_asub_u_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_asub_u_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_asub_u_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_asub_u_b                    (rtx, rtx, rtx);
extern rtx        gen_msa_ave_s_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_ave_s_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_ave_s_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_ave_s_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_ave_u_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_ave_u_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_ave_u_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_ave_u_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_aver_s_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_aver_s_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_aver_s_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_aver_s_b                    (rtx, rtx, rtx);
extern rtx        gen_msa_aver_u_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_aver_u_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_aver_u_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_aver_u_b                    (rtx, rtx, rtx);
extern rtx        gen_msa_bclr_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_bclr_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_bclr_h                      (rtx, rtx, rtx);
extern rtx        gen_msa_bclr_b                      (rtx, rtx, rtx);
extern rtx        gen_msa_bclri_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_bclri_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_bclri_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_bclri_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_binsl_d                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsl_w                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsl_h                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsl_b                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsli_d                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsli_w                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsli_h                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsli_b                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsr_d                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsr_w                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsr_h                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsr_b                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsri_d                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsri_w                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsri_h                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_binsri_b                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bmnz_d                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bmnz_w                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bmnz_h                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bmnz_b                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bmz_d                       (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bmz_w                       (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bmz_h                       (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bmz_b                       (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bneg_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_bneg_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_bneg_h                      (rtx, rtx, rtx);
extern rtx        gen_msa_bneg_b                      (rtx, rtx, rtx);
extern rtx        gen_msa_bnegi_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_bnegi_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_bnegi_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_bnegi_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_bsel_d                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bsel_w                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bsel_h                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bsel_b                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_bset_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_bset_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_bset_h                      (rtx, rtx, rtx);
extern rtx        gen_msa_bset_b                      (rtx, rtx, rtx);
extern rtx        gen_msa_bseti_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_bseti_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_bseti_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_bseti_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_ceq_d                       (rtx, rtx, rtx);
extern rtx        gen_msa_cle_s_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_cle_u_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_clt_s_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_clt_u_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_ceq_w                       (rtx, rtx, rtx);
extern rtx        gen_msa_cle_s_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_cle_u_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_clt_s_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_clt_u_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_ceq_h                       (rtx, rtx, rtx);
extern rtx        gen_msa_cle_s_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_cle_u_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_clt_s_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_clt_u_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_ceq_b                       (rtx, rtx, rtx);
extern rtx        gen_msa_cle_s_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_cle_u_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_clt_s_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_clt_u_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_dotp_s_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_dotp_u_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_dotp_s_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_dotp_u_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_dotp_s_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_dotp_u_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_dpadd_s_d                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpadd_u_d                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpadd_s_w                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpadd_u_w                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpadd_s_h                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpadd_u_h                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpsub_s_d                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpsub_u_d                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpsub_s_w                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpsub_u_w                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpsub_s_h                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_dpsub_u_h                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_fclass_d                    (rtx, rtx);
extern rtx        gen_msa_fclass_w                    (rtx, rtx);
extern rtx        gen_msa_fcaf_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fcaf_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fcune_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_fcune_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_fcun_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fcor_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fceq_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fcne_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fcle_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fclt_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fcueq_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_fcule_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_fcult_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_fcun_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fcor_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fceq_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fcne_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fcle_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fclt_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fcueq_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_fcule_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_fcult_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_fsaf_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fsun_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fsor_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fseq_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fsne_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fsueq_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_fsune_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_fsule_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_fsult_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_fsle_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fslt_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_fsaf_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fsun_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fsor_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fseq_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fsne_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fsueq_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_fsune_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_fsule_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_fsult_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_fsle_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fslt_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_fexp2_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_fexp2_w                     (rtx, rtx, rtx);
extern rtx        gen_floatv2div2df2                  (rtx, rtx);
extern rtx        gen_floatv4siv4sf2                  (rtx, rtx);
extern rtx        gen_floatunsv2div2df2               (rtx, rtx);
extern rtx        gen_floatunsv4siv4sf2               (rtx, rtx);
extern rtx        gen_msa_ffql_d                      (rtx, rtx);
extern rtx        gen_msa_ffql_w                      (rtx, rtx);
extern rtx        gen_msa_ffqr_d                      (rtx, rtx);
extern rtx        gen_msa_ffqr_w                      (rtx, rtx);
extern rtx        gen_msa_fill_d_f                    (rtx, rtx);
extern rtx        gen_msa_fill_w_f                    (rtx, rtx);
extern rtx        gen_msa_fill_d                      (rtx, rtx);
extern rtx        gen_msa_fill_w                      (rtx, rtx);
extern rtx        gen_msa_fill_h                      (rtx, rtx);
extern rtx        gen_msa_fill_b                      (rtx, rtx);
extern rtx        gen_msa_flog2_d                     (rtx, rtx);
extern rtx        gen_msa_flog2_w                     (rtx, rtx);
extern rtx        gen_smaxv2df3                       (rtx, rtx, rtx);
extern rtx        gen_smaxv4sf3                       (rtx, rtx, rtx);
extern rtx        gen_msa_fmax_a_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_fmax_a_w                    (rtx, rtx, rtx);
extern rtx        gen_sminv2df3                       (rtx, rtx, rtx);
extern rtx        gen_sminv4sf3                       (rtx, rtx, rtx);
extern rtx        gen_msa_fmin_a_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_fmin_a_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_frcp_d                      (rtx, rtx);
extern rtx        gen_msa_frcp_w                      (rtx, rtx);
extern rtx        gen_msa_frint_d                     (rtx, rtx);
extern rtx        gen_msa_frint_w                     (rtx, rtx);
extern rtx        gen_msa_frsqrt_d                    (rtx, rtx);
extern rtx        gen_msa_frsqrt_w                    (rtx, rtx);
extern rtx        gen_msa_ftint_s_d                   (rtx, rtx);
extern rtx        gen_msa_ftint_s_w                   (rtx, rtx);
extern rtx        gen_msa_ftint_u_d                   (rtx, rtx);
extern rtx        gen_msa_ftint_u_w                   (rtx, rtx);
extern rtx        gen_fix_truncv2dfv2di2              (rtx, rtx);
extern rtx        gen_fix_truncv4sfv4si2              (rtx, rtx);
extern rtx        gen_fixuns_truncv2dfv2di2           (rtx, rtx);
extern rtx        gen_fixuns_truncv4sfv4si2           (rtx, rtx);
extern rtx        gen_msa_ftq_h                       (rtx, rtx, rtx);
extern rtx        gen_msa_ftq_w                       (rtx, rtx, rtx);
extern rtx        gen_msa_hadd_s_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_hadd_u_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_hsub_s_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_hsub_u_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_hadd_s_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_hadd_u_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_hsub_s_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_hsub_u_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_hadd_s_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_hadd_u_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_hsub_s_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_hsub_u_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_ilvev_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_ilvev_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_ilvev_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_ilvev_w_f                   (rtx, rtx, rtx);
extern rtx        gen_msa_ilvl_b                      (rtx, rtx, rtx);
extern rtx        gen_msa_ilvl_h                      (rtx, rtx, rtx);
extern rtx        gen_msa_ilvl_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_ilvl_w_f                    (rtx, rtx, rtx);
extern rtx        gen_msa_ilvl_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_ilvl_d_f                    (rtx, rtx, rtx);
extern rtx        gen_msa_ilvod_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_ilvod_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_ilvod_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_ilvod_w_f                   (rtx, rtx, rtx);
extern rtx        gen_msa_ilvr_b                      (rtx, rtx, rtx);
extern rtx        gen_msa_ilvr_h                      (rtx, rtx, rtx);
extern rtx        gen_msa_ilvr_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_ilvr_w_f                    (rtx, rtx, rtx);
extern rtx        gen_msa_ilvr_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_ilvr_d_f                    (rtx, rtx, rtx);
extern rtx        gen_msa_madd_q_w                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_madd_q_h                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_maddr_q_w                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_maddr_q_h                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_max_a_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_max_a_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_max_a_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_max_a_b                     (rtx, rtx, rtx);
extern rtx        gen_smaxv2di3                       (rtx, rtx, rtx);
extern rtx        gen_smaxv4si3                       (rtx, rtx, rtx);
extern rtx        gen_smaxv8hi3                       (rtx, rtx, rtx);
extern rtx        gen_smaxv16qi3                      (rtx, rtx, rtx);
extern rtx        gen_umaxv2di3                       (rtx, rtx, rtx);
extern rtx        gen_umaxv4si3                       (rtx, rtx, rtx);
extern rtx        gen_umaxv8hi3                       (rtx, rtx, rtx);
extern rtx        gen_umaxv16qi3                      (rtx, rtx, rtx);
extern rtx        gen_msa_min_a_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_min_a_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_min_a_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_min_a_b                     (rtx, rtx, rtx);
extern rtx        gen_sminv2di3                       (rtx, rtx, rtx);
extern rtx        gen_sminv4si3                       (rtx, rtx, rtx);
extern rtx        gen_sminv8hi3                       (rtx, rtx, rtx);
extern rtx        gen_sminv16qi3                      (rtx, rtx, rtx);
extern rtx        gen_uminv2di3                       (rtx, rtx, rtx);
extern rtx        gen_uminv4si3                       (rtx, rtx, rtx);
extern rtx        gen_uminv8hi3                       (rtx, rtx, rtx);
extern rtx        gen_uminv16qi3                      (rtx, rtx, rtx);
extern rtx        gen_msa_msub_q_w                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_msub_q_h                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_msubr_q_w                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_msubr_q_h                   (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_mul_q_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_mul_q_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_mulr_q_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_mulr_q_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_nloc_d                      (rtx, rtx);
extern rtx        gen_msa_nloc_w                      (rtx, rtx);
extern rtx        gen_msa_nloc_h                      (rtx, rtx);
extern rtx        gen_msa_nloc_b                      (rtx, rtx);
extern rtx        gen_clzv2di2                        (rtx, rtx);
extern rtx        gen_clzv4si2                        (rtx, rtx);
extern rtx        gen_clzv8hi2                        (rtx, rtx);
extern rtx        gen_clzv16qi2                       (rtx, rtx);
extern rtx        gen_msa_nor_d                       (rtx, rtx, rtx);
extern rtx        gen_msa_nor_w                       (rtx, rtx, rtx);
extern rtx        gen_msa_nor_h                       (rtx, rtx, rtx);
extern rtx        gen_msa_nor_b                       (rtx, rtx, rtx);
extern rtx        gen_msa_pckev_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_pckev_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_pckev_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_pckev_w_f                   (rtx, rtx, rtx);
extern rtx        gen_msa_pckod_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_pckod_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_pckod_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_pckod_w_f                   (rtx, rtx, rtx);
extern rtx        gen_popcountv2di2                   (rtx, rtx);
extern rtx        gen_popcountv4si2                   (rtx, rtx);
extern rtx        gen_popcountv8hi2                   (rtx, rtx);
extern rtx        gen_popcountv16qi2                  (rtx, rtx);
extern rtx        gen_msa_sat_s_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_sat_s_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_sat_s_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_sat_s_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_sat_u_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_sat_u_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_sat_u_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_sat_u_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_shf_w                       (rtx, rtx, rtx);
extern rtx        gen_msa_shf_h                       (rtx, rtx, rtx);
extern rtx        gen_msa_shf_b                       (rtx, rtx, rtx);
extern rtx        gen_msa_shf_w_f                     (rtx, rtx, rtx);
extern rtx        gen_msa_srar_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_srar_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_srar_h                      (rtx, rtx, rtx);
extern rtx        gen_msa_srar_b                      (rtx, rtx, rtx);
extern rtx        gen_msa_srari_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_srari_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_srari_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_srari_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_srlr_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_srlr_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_srlr_h                      (rtx, rtx, rtx);
extern rtx        gen_msa_srlr_b                      (rtx, rtx, rtx);
extern rtx        gen_msa_srlri_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_srlri_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_srlri_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_srlri_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_subs_s_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_subs_s_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_subs_s_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_subs_s_b                    (rtx, rtx, rtx);
extern rtx        gen_msa_subs_u_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_subs_u_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_subs_u_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_subs_u_b                    (rtx, rtx, rtx);
extern rtx        gen_msa_subsuu_s_d                  (rtx, rtx, rtx);
extern rtx        gen_msa_subsuu_s_w                  (rtx, rtx, rtx);
extern rtx        gen_msa_subsuu_s_h                  (rtx, rtx, rtx);
extern rtx        gen_msa_subsuu_s_b                  (rtx, rtx, rtx);
extern rtx        gen_msa_subsus_u_d                  (rtx, rtx, rtx);
extern rtx        gen_msa_subsus_u_w                  (rtx, rtx, rtx);
extern rtx        gen_msa_subsus_u_h                  (rtx, rtx, rtx);
extern rtx        gen_msa_subsus_u_b                  (rtx, rtx, rtx);
extern rtx        gen_msa_sld_d_f                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sld_w_f                     (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sld_d                       (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sld_w                       (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sld_h                       (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sld_b                       (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sldi_d_f                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sldi_w_f                    (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sldi_d                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sldi_w                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sldi_h                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_sldi_b                      (rtx, rtx, rtx, rtx);
extern rtx        gen_msa_splat_d_f                   (rtx, rtx, rtx);
extern rtx        gen_msa_splat_w_f                   (rtx, rtx, rtx);
extern rtx        gen_msa_splat_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_splat_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_splat_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_splat_b                     (rtx, rtx, rtx);
extern rtx        gen_msa_splati_d_f                  (rtx, rtx, rtx);
extern rtx        gen_msa_splati_w_f                  (rtx, rtx, rtx);
extern rtx        gen_msa_splati_d                    (rtx, rtx, rtx);
extern rtx        gen_msa_splati_w                    (rtx, rtx, rtx);
extern rtx        gen_msa_splati_h                    (rtx, rtx, rtx);
extern rtx        gen_msa_splati_b                    (rtx, rtx, rtx);
extern rtx        gen_msa_splati_d_f_scalar           (rtx, rtx);
extern rtx        gen_msa_splati_w_f_scalar           (rtx, rtx);
extern rtx        gen_msa_cfcmsa                      (rtx, rtx);
extern rtx        gen_msa_ctcmsa                      (rtx, rtx);
extern rtx        gen_msa_fexdo_h                     (rtx, rtx, rtx);
extern rtx        gen_vec_pack_trunc_v2df             (rtx, rtx, rtx);
extern rtx        gen_msa_fexupl_w                    (rtx, rtx);
extern rtx        gen_msa_fexupl_d                    (rtx, rtx);
extern rtx        gen_msa_fexupr_w                    (rtx, rtx);
extern rtx        gen_msa_fexupr_d                    (rtx, rtx);
extern rtx        gen_msa_bz_d_f                      (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_d_f                     (rtx, rtx, rtx);
extern rtx        gen_msa_bz_w_f                      (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_w_f                     (rtx, rtx, rtx);
extern rtx        gen_msa_bz_d                        (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_d                       (rtx, rtx, rtx);
extern rtx        gen_msa_bz_w                        (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_w                       (rtx, rtx, rtx);
extern rtx        gen_msa_bz_h                        (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_h                       (rtx, rtx, rtx);
extern rtx        gen_msa_bz_b                        (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_b                       (rtx, rtx, rtx);
extern rtx        gen_msa_bz_v_d_f                    (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_v_d_f                   (rtx, rtx, rtx);
extern rtx        gen_msa_bz_v_w_f                    (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_v_w_f                   (rtx, rtx, rtx);
extern rtx        gen_msa_bz_v_d                      (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_v_d                     (rtx, rtx, rtx);
extern rtx        gen_msa_bz_v_w                      (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_v_w                     (rtx, rtx, rtx);
extern rtx        gen_msa_bz_v_h                      (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_v_h                     (rtx, rtx, rtx);
extern rtx        gen_msa_bz_v_b                      (rtx, rtx, rtx);
extern rtx        gen_msa_bnz_v_b                     (rtx, rtx, rtx);
extern rtx        gen_ctrapsi4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_ctrapdi4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_addsi3                          (rtx, rtx, rtx);
extern rtx        gen_adddi3                          (rtx, rtx, rtx);
extern rtx        gen_mulsf3                          (rtx, rtx, rtx);
extern rtx        gen_muldf3                          (rtx, rtx, rtx);
extern rtx        gen_mulsi3                          (rtx, rtx, rtx);
extern rtx        gen_muldi3                          (rtx, rtx, rtx);
extern rtx        gen_mulsidi3                        (rtx, rtx, rtx);
extern rtx        gen_umulsidi3                       (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_32bit_r6               (rtx, rtx, rtx);
extern rtx        gen_umulsidi3_32bit_r6              (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_32bit_mips16           (rtx, rtx, rtx);
extern rtx        gen_umulsidi3_32bit_mips16          (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_64bit_mips16           (rtx, rtx, rtx);
extern rtx        gen_umulsidi3_64bit_mips16          (rtx, rtx, rtx);
extern rtx        gen_mulsidi3_64bit_split            (rtx, rtx, rtx, rtx);
extern rtx        gen_umulsidi3_64bit_split           (rtx, rtx, rtx, rtx);
extern rtx        gen_smulsi3_highpart                (rtx, rtx, rtx);
extern rtx        gen_umulsi3_highpart                (rtx, rtx, rtx);
extern rtx        gen_smulsi3_highpart_split          (rtx, rtx, rtx);
extern rtx        gen_umulsi3_highpart_split          (rtx, rtx, rtx);
extern rtx        gen_smuldi3_highpart                (rtx, rtx, rtx);
extern rtx        gen_umuldi3_highpart                (rtx, rtx, rtx);
extern rtx        gen_smuldi3_highpart_split          (rtx, rtx, rtx);
extern rtx        gen_umuldi3_highpart_split          (rtx, rtx, rtx);
extern rtx        gen_mulditi3                        (rtx, rtx, rtx);
extern rtx        gen_umulditi3                       (rtx, rtx, rtx);
extern rtx        gen_fmasf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_fmadf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_fmav2sf4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_fmssf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_fmsdf4                          (rtx, rtx, rtx, rtx);
extern rtx        gen_fmsv2sf4                        (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmasf4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmadf4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmav2sf4                       (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmssf4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmsdf4                         (rtx, rtx, rtx, rtx);
extern rtx        gen_fnmsv2sf4                       (rtx, rtx, rtx, rtx);
extern rtx        gen_divsf3                          (rtx, rtx, rtx);
extern rtx        gen_divdf3                          (rtx, rtx, rtx);
extern rtx        gen_divv2sf3                        (rtx, rtx, rtx);
extern rtx        gen_divmodsi4                       (rtx, rtx, rtx, rtx);
extern rtx        gen_divmoddi4                       (rtx, rtx, rtx, rtx);
extern rtx        gen_udivmodsi4                      (rtx, rtx, rtx, rtx);
extern rtx        gen_udivmoddi4                      (rtx, rtx, rtx, rtx);
extern rtx        gen_divmodsi4_split                 (rtx, rtx, rtx);
extern rtx        gen_udivmodsi4_split                (rtx, rtx, rtx);
extern rtx        gen_divmoddi4_split                 (rtx, rtx, rtx);
extern rtx        gen_udivmoddi4_split                (rtx, rtx, rtx);
extern rtx        gen_andsi3                          (rtx, rtx, rtx);
extern rtx        gen_anddi3                          (rtx, rtx, rtx);
extern rtx        gen_iorsi3                          (rtx, rtx, rtx);
extern rtx        gen_iordi3                          (rtx, rtx, rtx);
extern rtx        gen_xorsi3                          (rtx, rtx, rtx);
extern rtx        gen_xordi3                          (rtx, rtx, rtx);
extern rtx        gen_zero_extendsidi2                (rtx, rtx);
extern rtx        gen_zero_extendqisi2                (rtx, rtx);
extern rtx        gen_zero_extendqidi2                (rtx, rtx);
extern rtx        gen_zero_extendhisi2                (rtx, rtx);
extern rtx        gen_zero_extendhidi2                (rtx, rtx);
extern rtx        gen_zero_extendqihi2                (rtx, rtx);
extern rtx        gen_extendqisi2                     (rtx, rtx);
extern rtx        gen_extendqidi2                     (rtx, rtx);
extern rtx        gen_extendhisi2                     (rtx, rtx);
extern rtx        gen_extendhidi2                     (rtx, rtx);
extern rtx        gen_extendqihi2                     (rtx, rtx);
extern rtx        gen_fix_truncdfsi2                  (rtx, rtx);
extern rtx        gen_fix_truncsfsi2                  (rtx, rtx);
extern rtx        gen_fixuns_truncdfsi2               (rtx, rtx);
extern rtx        gen_fixuns_truncdfdi2               (rtx, rtx);
extern rtx        gen_fixuns_truncsfsi2               (rtx, rtx);
extern rtx        gen_fixuns_truncsfdi2               (rtx, rtx);
extern rtx        gen_extvmisalignsi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_extvmisaligndi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_extvsi                          (rtx, rtx, rtx, rtx);
extern rtx        gen_extvdi                          (rtx, rtx, rtx, rtx);
extern rtx        gen_extzvmisalignsi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_extzvmisaligndi                 (rtx, rtx, rtx, rtx);
extern rtx        gen_extzvsi                         (rtx, rtx, rtx, rtx);
extern rtx        gen_extzvdi                         (rtx, rtx, rtx, rtx);
extern rtx        gen_insvmisalignsi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_insvmisaligndi                  (rtx, rtx, rtx, rtx);
extern rtx        gen_insvsi                          (rtx, rtx, rtx, rtx);
extern rtx        gen_insvdi                          (rtx, rtx, rtx, rtx);
extern rtx        gen_unspec_got_si                   (rtx, rtx);
extern rtx        gen_unspec_got_di                   (rtx, rtx);
extern rtx        gen_movdi                           (rtx, rtx);
extern rtx        gen_movsi                           (rtx, rtx);
extern rtx        gen_movv2hi                         (rtx, rtx);
extern rtx        gen_movv4qi                         (rtx, rtx);
extern rtx        gen_movv2hq                         (rtx, rtx);
extern rtx        gen_movv2uhq                        (rtx, rtx);
extern rtx        gen_movv2ha                         (rtx, rtx);
extern rtx        gen_movv2uha                        (rtx, rtx);
extern rtx        gen_movv4qq                         (rtx, rtx);
extern rtx        gen_movv4uqq                        (rtx, rtx);
extern rtx        gen_movhi                           (rtx, rtx);
extern rtx        gen_movqi                           (rtx, rtx);
extern rtx        gen_movsf                           (rtx, rtx);
extern rtx        gen_movdf                           (rtx, rtx);
extern rtx        gen_movti                           (rtx, rtx);
extern rtx        gen_movtf                           (rtx, rtx);
extern rtx        gen_movv2sf                         (rtx, rtx);
extern rtx        gen_move_doubleword_fprdf           (rtx, rtx);
extern rtx        gen_move_doubleword_fprdi           (rtx, rtx);
extern rtx        gen_move_doubleword_fprv2sf         (rtx, rtx);
extern rtx        gen_move_doubleword_fprv2si         (rtx, rtx);
extern rtx        gen_move_doubleword_fprv4hi         (rtx, rtx);
extern rtx        gen_move_doubleword_fprv8qi         (rtx, rtx);
extern rtx        gen_move_doubleword_fprtf           (rtx, rtx);
extern rtx        gen_load_const_gp_si                (rtx);
extern rtx        gen_load_const_gp_di                (rtx);
extern rtx        gen_clear_cache                     (rtx, rtx);
extern rtx        gen_movmemsi                        (rtx, rtx, rtx, rtx);
extern rtx        gen_ashlsi3                         (rtx, rtx, rtx);
extern rtx        gen_ashrsi3                         (rtx, rtx, rtx);
extern rtx        gen_lshrsi3                         (rtx, rtx, rtx);
extern rtx        gen_ashldi3                         (rtx, rtx, rtx);
extern rtx        gen_ashrdi3                         (rtx, rtx, rtx);
extern rtx        gen_lshrdi3                         (rtx, rtx, rtx);
extern rtx        gen_cbranchsi4                      (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchdi4                      (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchsf4                      (rtx, rtx, rtx, rtx);
extern rtx        gen_cbranchdf4                      (rtx, rtx, rtx, rtx);
extern rtx        gen_condjump                        (rtx, rtx);
extern rtx        gen_cstoresi4                       (rtx, rtx, rtx, rtx);
extern rtx        gen_cstoredi4                       (rtx, rtx, rtx, rtx);
extern rtx        gen_jump                            (rtx);
extern rtx        gen_indirect_jump                   (rtx);
extern rtx        gen_tablejump                       (rtx, rtx);
extern rtx        gen_casesi                          (rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_builtin_setjmp_setup            (rtx);
extern rtx        gen_builtin_longjmp                 (rtx);
extern rtx        gen_prologue                        (void);
extern rtx        gen_epilogue                        (void);
extern rtx        gen_sibcall_epilogue                (void);
extern rtx        gen_return                          (void);
extern rtx        gen_simple_return                   (void);
extern rtx        gen_eh_return                       (rtx);
extern rtx        gen_exception_receiver              (void);
extern rtx        gen_nonlocal_goto_receiver          (void);
extern rtx        gen_sibcall                         (rtx, rtx, rtx, rtx);
extern rtx        gen_sibcall_value                   (rtx, rtx, rtx, rtx);
extern rtx        gen_call                            (rtx, rtx, rtx, rtx);
extern rtx        gen_call_value                      (rtx, rtx, rtx, rtx);
extern rtx        gen_untyped_call                    (rtx, rtx, rtx);
extern rtx        gen_movsicc                         (rtx, rtx, rtx, rtx);
extern rtx        gen_movdicc                         (rtx, rtx, rtx, rtx);
extern rtx        gen_movsfcc                         (rtx, rtx, rtx, rtx);
extern rtx        gen_movdfcc                         (rtx, rtx, rtx, rtx);
extern rtx        gen_get_thread_pointersi            (rtx);
extern rtx        gen_get_thread_pointerdi            (rtx);
extern rtx        gen_mips_get_fcsr                   (rtx);
extern rtx        gen_mips_set_fcsr                   (rtx);
extern rtx        gen_memory_barrier                  (void);
extern rtx        gen_sync_compare_and_swapqi         (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_compare_and_swaphi         (rtx, rtx, rtx, rtx);
extern rtx        gen_sync_addqi                      (rtx, rtx);
extern rtx        gen_sync_subqi                      (rtx, rtx);
extern rtx        gen_sync_iorqi                      (rtx, rtx);
extern rtx        gen_sync_xorqi                      (rtx, rtx);
extern rtx        gen_sync_andqi                      (rtx, rtx);
extern rtx        gen_sync_addhi                      (rtx, rtx);
extern rtx        gen_sync_subhi                      (rtx, rtx);
extern rtx        gen_sync_iorhi                      (rtx, rtx);
extern rtx        gen_sync_xorhi                      (rtx, rtx);
extern rtx        gen_sync_andhi                      (rtx, rtx);
extern rtx        gen_sync_old_addqi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_subqi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_iorqi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_xorqi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_andqi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_addhi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_subhi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_iorhi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_xorhi                  (rtx, rtx, rtx);
extern rtx        gen_sync_old_andhi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_addqi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_subqi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_iorqi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_xorqi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_andqi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_addhi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_subhi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_iorhi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_xorhi                  (rtx, rtx, rtx);
extern rtx        gen_sync_new_andhi                  (rtx, rtx, rtx);
extern rtx        gen_sync_nandqi                     (rtx, rtx);
extern rtx        gen_sync_nandhi                     (rtx, rtx);
extern rtx        gen_sync_old_nandqi                 (rtx, rtx, rtx);
extern rtx        gen_sync_old_nandhi                 (rtx, rtx, rtx);
extern rtx        gen_sync_new_nandqi                 (rtx, rtx, rtx);
extern rtx        gen_sync_new_nandhi                 (rtx, rtx, rtx);
extern rtx        gen_sync_lock_test_and_setqi        (rtx, rtx, rtx);
extern rtx        gen_sync_lock_test_and_sethi        (rtx, rtx, rtx);
extern rtx        gen_atomic_exchangesi               (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_exchangedi               (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_addsi              (rtx, rtx, rtx, rtx);
extern rtx        gen_atomic_fetch_adddi              (rtx, rtx, rtx, rtx);
extern rtx        gen_movv2sfcc                       (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_puu_ps                     (rtx, rtx, rtx);
extern rtx        gen_mips_pul_ps                     (rtx, rtx, rtx);
extern rtx        gen_mips_plu_ps                     (rtx, rtx, rtx);
extern rtx        gen_mips_pll_ps                     (rtx, rtx, rtx);
extern rtx        gen_vec_initv2sfsf                  (rtx, rtx);
extern rtx        gen_vec_setv2sf                     (rtx, rtx, rtx);
extern rtx        gen_mips_cvt_ps_s                   (rtx, rtx, rtx);
extern rtx        gen_mips_cvt_s_pl                   (rtx, rtx);
extern rtx        gen_mips_cvt_s_pu                   (rtx, rtx);
extern rtx        gen_reduc_plus_scal_v2sf            (rtx, rtx);
extern rtx        gen_mips_abs_ps                     (rtx, rtx);
extern rtx        gen_scc_ps                          (rtx, rtx);
extern rtx        gen_single_cc                       (rtx, rtx);
extern rtx        gen_vcondv2sfv2sf                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_sminv2sf3                       (rtx, rtx, rtx);
extern rtx        gen_smaxv2sf3                       (rtx, rtx, rtx);
extern rtx        gen_reduc_smin_scal_v2sf            (rtx, rtx);
extern rtx        gen_reduc_smax_scal_v2sf            (rtx, rtx);
extern rtx        gen_mips_lbux                       (rtx, rtx, rtx);
extern rtx        gen_mips_lhx                        (rtx, rtx, rtx);
extern rtx        gen_mips_lwx                        (rtx, rtx, rtx);
extern rtx        gen_mips_ldx                        (rtx, rtx, rtx);
extern rtx        gen_mips_madd                       (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_maddu                      (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_msub                       (rtx, rtx, rtx, rtx);
extern rtx        gen_mips_msubu                      (rtx, rtx, rtx, rtx);
extern rtx        gen_movv2si                         (rtx, rtx);
extern rtx        gen_movv4hi                         (rtx, rtx);
extern rtx        gen_movv8qi                         (rtx, rtx);
extern rtx        gen_vec_initv2sisi                  (rtx, rtx);
extern rtx        gen_vec_initv4hihi                  (rtx, rtx);
extern rtx        gen_vec_initv8qiqi                  (rtx, rtx);
extern rtx        gen_vec_setv4hi                     (rtx, rtx, rtx, rtx);
extern rtx        gen_sdot_prodv4hi                   (rtx, rtx, rtx, rtx);
extern rtx        gen_smaxv2si3                       (rtx, rtx, rtx);
extern rtx        gen_smaxv8qi3                       (rtx, rtx, rtx);
extern rtx        gen_sminv2si3                       (rtx, rtx, rtx);
extern rtx        gen_sminv8qi3                       (rtx, rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v4hi             (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v8qi             (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v4hi             (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v8qi             (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v4hi             (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v8qi             (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v4hi             (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v8qi             (rtx, rtx);
extern rtx        gen_reduc_plus_scal_v2si            (rtx, rtx);
extern rtx        gen_reduc_plus_scal_v4hi            (rtx, rtx);
extern rtx        gen_reduc_plus_scal_v8qi            (rtx, rtx);
extern rtx        gen_reduc_smax_scal_v2si            (rtx, rtx);
extern rtx        gen_reduc_smax_scal_v4hi            (rtx, rtx);
extern rtx        gen_reduc_smax_scal_v8qi            (rtx, rtx);
extern rtx        gen_reduc_smin_scal_v2si            (rtx, rtx);
extern rtx        gen_reduc_smin_scal_v4hi            (rtx, rtx);
extern rtx        gen_reduc_smin_scal_v8qi            (rtx, rtx);
extern rtx        gen_reduc_umax_scal_v8qi            (rtx, rtx);
extern rtx        gen_reduc_umin_scal_v8qi            (rtx, rtx);
extern rtx        gen_vec_initv2dfdf                  (rtx, rtx);
extern rtx        gen_vec_initv4sfsf                  (rtx, rtx);
extern rtx        gen_vec_initv2didi                  (rtx, rtx);
extern rtx        gen_vec_initv4sisi                  (rtx, rtx);
extern rtx        gen_vec_initv8hihi                  (rtx, rtx);
extern rtx        gen_vec_initv16qiqi                 (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v4sf             (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v4sf             (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v4si             (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v8hi             (rtx, rtx);
extern rtx        gen_vec_unpacks_hi_v16qi            (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v4si             (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v8hi             (rtx, rtx);
extern rtx        gen_vec_unpacks_lo_v16qi            (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v4si             (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v8hi             (rtx, rtx);
extern rtx        gen_vec_unpacku_hi_v16qi            (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v4si             (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v8hi             (rtx, rtx);
extern rtx        gen_vec_unpacku_lo_v16qi            (rtx, rtx);
extern rtx        gen_vec_extractv2didi               (rtx, rtx, rtx);
extern rtx        gen_vec_extractv4sisi               (rtx, rtx, rtx);
extern rtx        gen_vec_extractv8hihi               (rtx, rtx, rtx);
extern rtx        gen_vec_extractv16qiqi              (rtx, rtx, rtx);
extern rtx        gen_vec_extractv2dfdf               (rtx, rtx, rtx);
extern rtx        gen_vec_extractv4sfsf               (rtx, rtx, rtx);
extern rtx        gen_vec_setv2di                     (rtx, rtx, rtx);
extern rtx        gen_vec_setv4si                     (rtx, rtx, rtx);
extern rtx        gen_vec_setv8hi                     (rtx, rtx, rtx);
extern rtx        gen_vec_setv16qi                    (rtx, rtx, rtx);
extern rtx        gen_vec_setv2df                     (rtx, rtx, rtx);
extern rtx        gen_vec_setv4sf                     (rtx, rtx, rtx);
extern rtx        gen_vconduv2dfv2di                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv2dfv4si                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv2dfv8hi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv2dfv16qi                 (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4sfv2di                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4sfv4si                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4sfv8hi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4sfv16qi                 (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv2div2di                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv2div4si                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv2div8hi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv2div16qi                 (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4siv2di                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4siv4si                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4siv8hi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv4siv16qi                 (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv8hiv2di                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv8hiv4si                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv8hiv8hi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv8hiv16qi                 (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv16qiv2di                 (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv16qiv4si                 (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv16qiv8hi                 (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vconduv16qiv16qi                (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2dfv2df                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4sfv2df                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2div2df                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4siv2df                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv8hiv2df                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv16qiv2df                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2dfv4sf                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4sfv4sf                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2div4sf                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4siv4sf                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv8hiv4sf                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv16qiv4sf                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2dfv2di                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4sfv2di                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2div2di                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4siv2di                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv8hiv2di                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv16qiv2di                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2dfv4si                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4sfv4si                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2div4si                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4siv4si                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv8hiv4si                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv16qiv4si                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2dfv8hi                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4sfv8hi                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2div8hi                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4siv8hi                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv8hiv8hi                   (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv16qiv8hi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2dfv16qi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4sfv16qi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv2div16qi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv4siv16qi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv8hiv16qi                  (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_vcondv16qiv16qi                 (rtx, rtx, rtx, rtx, rtx, rtx);
extern rtx        gen_absv2di2                        (rtx, rtx);
extern rtx        gen_absv4si2                        (rtx, rtx);
extern rtx        gen_absv8hi2                        (rtx, rtx);
extern rtx        gen_absv16qi2                       (rtx, rtx);
extern rtx        gen_negv2df2                        (rtx, rtx);
extern rtx        gen_negv4sf2                        (rtx, rtx);
extern rtx        gen_negv2di2                        (rtx, rtx);
extern rtx        gen_negv4si2                        (rtx, rtx);
extern rtx        gen_negv8hi2                        (rtx, rtx);
extern rtx        gen_negv16qi2                       (rtx, rtx);
extern rtx        gen_msa_ldiv2di                     (rtx, rtx);
extern rtx        gen_msa_ldiv4si                     (rtx, rtx);
extern rtx        gen_msa_ldiv8hi                     (rtx, rtx);
extern rtx        gen_msa_ldiv16qi                    (rtx, rtx);
extern rtx        gen_movv2df                         (rtx, rtx);
extern rtx        gen_movv4sf                         (rtx, rtx);
extern rtx        gen_movv2di                         (rtx, rtx);
extern rtx        gen_movv4si                         (rtx, rtx);
extern rtx        gen_movv8hi                         (rtx, rtx);
extern rtx        gen_movv16qi                        (rtx, rtx);
extern rtx        gen_movmisalignv2df                 (rtx, rtx);
extern rtx        gen_movmisalignv4sf                 (rtx, rtx);
extern rtx        gen_movmisalignv2di                 (rtx, rtx);
extern rtx        gen_movmisalignv4si                 (rtx, rtx);
extern rtx        gen_movmisalignv8hi                 (rtx, rtx);
extern rtx        gen_movmisalignv16qi                (rtx, rtx);
extern rtx        gen_msa_ld_d_f                      (rtx, rtx, rtx);
extern rtx        gen_msa_ld_w_f                      (rtx, rtx, rtx);
extern rtx        gen_msa_ld_d                        (rtx, rtx, rtx);
extern rtx        gen_msa_ld_w                        (rtx, rtx, rtx);
extern rtx        gen_msa_ld_h                        (rtx, rtx, rtx);
extern rtx        gen_msa_ld_b                        (rtx, rtx, rtx);
extern rtx        gen_msa_st_d_f                      (rtx, rtx, rtx);
extern rtx        gen_msa_st_w_f                      (rtx, rtx, rtx);
extern rtx        gen_msa_st_d                        (rtx, rtx, rtx);
extern rtx        gen_msa_st_w                        (rtx, rtx, rtx);
extern rtx        gen_msa_st_h                        (rtx, rtx, rtx);
extern rtx        gen_msa_st_b                        (rtx, rtx, rtx);

#endif /* GCC_INSN_FLAGS_H */
