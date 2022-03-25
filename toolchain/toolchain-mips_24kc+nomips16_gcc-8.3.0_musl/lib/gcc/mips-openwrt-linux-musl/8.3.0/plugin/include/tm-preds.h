/* Generated automatically by the program 'build/genpreds'
   from the machine description file '/home/paldier/isdk/8.4/openwrt/build_dir/toolchain-mips_24kc+nomips16_gcc-8.3.0_musl/gcc-8.3.0/gcc/config/mips/mips.md'.  */

#ifndef GCC_TM_PREDS_H
#define GCC_TM_PREDS_H

#ifdef HAVE_MACHINE_MODES
extern int general_operand (rtx, machine_mode);
extern int address_operand (rtx, machine_mode);
extern int register_operand (rtx, machine_mode);
extern int pmode_register_operand (rtx, machine_mode);
extern int scratch_operand (rtx, machine_mode);
extern int immediate_operand (rtx, machine_mode);
extern int const_int_operand (rtx, machine_mode);
extern int const_double_operand (rtx, machine_mode);
extern int nonimmediate_operand (rtx, machine_mode);
extern int nonmemory_operand (rtx, machine_mode);
extern int push_operand (rtx, machine_mode);
extern int pop_operand (rtx, machine_mode);
extern int memory_operand (rtx, machine_mode);
extern int indirect_operand (rtx, machine_mode);
extern int ordered_comparison_operator (rtx, machine_mode);
extern int comparison_operator (rtx, machine_mode);
extern int const_uns_arith_operand (rtx, machine_mode);
extern int uns_arith_operand (rtx, machine_mode);
extern int const_arith_operand (rtx, machine_mode);
extern int arith_operand (rtx, machine_mode);
extern int const_immlsa_operand (rtx, machine_mode);
extern int const_msa_branch_operand (rtx, machine_mode);
extern int const_uimm3_operand (rtx, machine_mode);
extern int const_uimm4_operand (rtx, machine_mode);
extern int const_uimm5_operand (rtx, machine_mode);
extern int const_uimm6_operand (rtx, machine_mode);
extern int const_uimm8_operand (rtx, machine_mode);
extern int const_imm5_operand (rtx, machine_mode);
extern int const_imm10_operand (rtx, machine_mode);
extern int reg_imm10_operand (rtx, machine_mode);
extern int aq10b_operand (rtx, machine_mode);
extern int aq10h_operand (rtx, machine_mode);
extern int aq10w_operand (rtx, machine_mode);
extern int aq10d_operand (rtx, machine_mode);
extern int sle_operand (rtx, machine_mode);
extern int sleu_operand (rtx, machine_mode);
extern int const_0_operand (rtx, machine_mode);
extern int const_m1_operand (rtx, machine_mode);
extern int reg_or_m1_operand (rtx, machine_mode);
extern int reg_or_0_operand (rtx, machine_mode);
extern int const_1_operand (rtx, machine_mode);
extern int reg_or_1_operand (rtx, machine_mode);
extern int const_exp_2_operand (rtx, machine_mode);
extern int const_exp_4_operand (rtx, machine_mode);
extern int const_exp_8_operand (rtx, machine_mode);
extern int const_exp_16_operand (rtx, machine_mode);
extern int const_0_or_1_operand (rtx, machine_mode);
extern int const_2_or_3_operand (rtx, machine_mode);
extern int const_0_to_3_operand (rtx, machine_mode);
extern int qi_mask_operand (rtx, machine_mode);
extern int hi_mask_operand (rtx, machine_mode);
extern int si_mask_operand (rtx, machine_mode);
extern int and_load_operand (rtx, machine_mode);
extern int low_bitmask_operand (rtx, machine_mode);
extern int and_reg_operand (rtx, machine_mode);
extern int and_operand (rtx, machine_mode);
extern int d_operand (rtx, machine_mode);
extern int lwsp_swsp_operand (rtx, machine_mode);
extern int lw16_sw16_operand (rtx, machine_mode);
extern int lhu16_sh16_operand (rtx, machine_mode);
extern int lbu16_operand (rtx, machine_mode);
extern int sb16_operand (rtx, machine_mode);
extern int db4_operand (rtx, machine_mode);
extern int db7_operand (rtx, machine_mode);
extern int db8_operand (rtx, machine_mode);
extern int ib3_operand (rtx, machine_mode);
extern int sb4_operand (rtx, machine_mode);
extern int sb5_operand (rtx, machine_mode);
extern int sb8_operand (rtx, machine_mode);
extern int sd8_operand (rtx, machine_mode);
extern int ub4_operand (rtx, machine_mode);
extern int ub8_operand (rtx, machine_mode);
extern int uh4_operand (rtx, machine_mode);
extern int uw4_operand (rtx, machine_mode);
extern int uw5_operand (rtx, machine_mode);
extern int uw6_operand (rtx, machine_mode);
extern int uw8_operand (rtx, machine_mode);
extern int addiur2_operand (rtx, machine_mode);
extern int addiusp_operand (rtx, machine_mode);
extern int andi16_operand (rtx, machine_mode);
extern int movep_src_register (rtx, machine_mode);
extern int movep_src_operand (rtx, machine_mode);
extern int lo_operand (rtx, machine_mode);
extern int hilo_operand (rtx, machine_mode);
extern int fcc_reload_operand (rtx, machine_mode);
extern int muldiv_target_operand (rtx, machine_mode);
extern int const_call_insn_operand (rtx, machine_mode);
extern int call_insn_operand (rtx, machine_mode);
extern int splittable_const_int_operand (rtx, machine_mode);
extern int move_operand (rtx, machine_mode);
extern int cprestore_save_slot_operand (rtx, machine_mode);
extern int cprestore_load_slot_operand (rtx, machine_mode);
extern int consttable_operand (rtx, machine_mode);
extern int symbolic_operand (rtx, machine_mode);
extern int absolute_symbolic_operand (rtx, machine_mode);
extern int symbolic_operand_with_high (rtx, machine_mode);
extern int force_to_mem_operand (rtx, machine_mode);
extern int got_disp_operand (rtx, machine_mode);
extern int got_page_ofst_operand (rtx, machine_mode);
extern int tls_reloc_operand (rtx, machine_mode);
extern int symbol_ref_operand (rtx, machine_mode);
extern int stack_operand (rtx, machine_mode);
extern int macc_msac_operand (rtx, machine_mode);
extern int equality_operator (rtx, machine_mode);
extern int extend_operator (rtx, machine_mode);
extern int trap_comparison_operator (rtx, machine_mode);
extern int order_operator (rtx, machine_mode);
extern int mips_cstore_operator (rtx, machine_mode);
extern int small_data_pattern (rtx, machine_mode);
extern int mem_noofs_operand (rtx, machine_mode);
extern int non_volatile_mem_operand (rtx, machine_mode);
extern int const_vector_same_val_operand (rtx, machine_mode);
extern int const_vector_same_simm5_operand (rtx, machine_mode);
extern int const_vector_same_uimm5_operand (rtx, machine_mode);
extern int const_vector_same_ximm5_operand (rtx, machine_mode);
extern int const_vector_same_uimm6_operand (rtx, machine_mode);
extern int const_vector_same_uimm8_operand (rtx, machine_mode);
extern int par_const_vector_shf_set_operand (rtx, machine_mode);
extern int reg_or_vector_same_val_operand (rtx, machine_mode);
extern int reg_or_vector_same_simm5_operand (rtx, machine_mode);
extern int reg_or_vector_same_uimm5_operand (rtx, machine_mode);
extern int reg_or_vector_same_ximm5_operand (rtx, machine_mode);
extern int reg_or_vector_same_uimm6_operand (rtx, machine_mode);
#endif /* HAVE_MACHINE_MODES */

#define CONSTRAINT_NUM_DEFINED_P 1
enum constraint_num
{
  CONSTRAINT__UNKNOWN = 0,
  CONSTRAINT_r,
  CONSTRAINT_d,
  CONSTRAINT_t,
  CONSTRAINT_f,
  CONSTRAINT_h,
  CONSTRAINT_l,
  CONSTRAINT_x,
  CONSTRAINT_b,
  CONSTRAINT_u,
  CONSTRAINT_c,
  CONSTRAINT_e,
  CONSTRAINT_j,
  CONSTRAINT_v,
  CONSTRAINT_y,
  CONSTRAINT_z,
  CONSTRAINT_A,
  CONSTRAINT_a,
  CONSTRAINT_B,
  CONSTRAINT_C,
  CONSTRAINT_D,
  CONSTRAINT_ka,
  CONSTRAINT_kb,
  CONSTRAINT_I,
  CONSTRAINT_J,
  CONSTRAINT_K,
  CONSTRAINT_L,
  CONSTRAINT_M,
  CONSTRAINT_N,
  CONSTRAINT_O,
  CONSTRAINT_P,
  CONSTRAINT_m,
  CONSTRAINT_o,
  CONSTRAINT_R,
  CONSTRAINT_W,
  CONSTRAINT_ZC,
  CONSTRAINT_ZR,
  CONSTRAINT_ZS,
  CONSTRAINT_ZT,
  CONSTRAINT_ZU,
  CONSTRAINT_ZV,
  CONSTRAINT_ZW,
  CONSTRAINT_p,
  CONSTRAINT_ZD,
  CONSTRAINT_kf,
  CONSTRAINT_G,
  CONSTRAINT_Q,
  CONSTRAINT_Udb7,
  CONSTRAINT_Udb8,
  CONSTRAINT_Uead,
  CONSTRAINT_Uean,
  CONSTRAINT_Uesp,
  CONSTRAINT_Uib3,
  CONSTRAINT_Usb4,
  CONSTRAINT_Usb5,
  CONSTRAINT_Usb8,
  CONSTRAINT_Usd8,
  CONSTRAINT_Uub8,
  CONSTRAINT_Uuw5,
  CONSTRAINT_Uuw6,
  CONSTRAINT_Uuw8,
  CONSTRAINT_YG,
  CONSTRAINT_YA,
  CONSTRAINT_YB,
  CONSTRAINT_Yb,
  CONSTRAINT_Yh,
  CONSTRAINT_Yw,
  CONSTRAINT_Yx,
  CONSTRAINT_YI,
  CONSTRAINT_YC,
  CONSTRAINT_YZ,
  CONSTRAINT_Unv5,
  CONSTRAINT_Uuv5,
  CONSTRAINT_Usv5,
  CONSTRAINT_Uuv6,
  CONSTRAINT_Urv8,
  CONSTRAINT_ks,
  CONSTRAINT_S,
  CONSTRAINT_V,
  CONSTRAINT__l,
  CONSTRAINT__g,
  CONSTRAINT_i,
  CONSTRAINT_s,
  CONSTRAINT_n,
  CONSTRAINT_E,
  CONSTRAINT_F,
  CONSTRAINT_X,
  CONSTRAINT_Yd,
  CONSTRAINT_Yf,
  CONSTRAINT__LIMIT
};

extern enum constraint_num lookup_constraint_1 (const char *);
extern const unsigned char lookup_constraint_array[];

/* Return the constraint at the beginning of P, or CONSTRAINT__UNKNOWN if it
   isn't recognized.  */

static inline enum constraint_num
lookup_constraint (const char *p)
{
  unsigned int index = lookup_constraint_array[(unsigned char) *p];
  return (index == UCHAR_MAX
          ? lookup_constraint_1 (p)
          : (enum constraint_num) index);
}

extern bool (*constraint_satisfied_p_array[]) (rtx);

/* Return true if X satisfies constraint C.  */

static inline bool
constraint_satisfied_p (rtx x, enum constraint_num c)
{
  int i = (int) c - (int) CONSTRAINT_I;
  return i >= 0 && constraint_satisfied_p_array[i] (x);
}

static inline bool
insn_extra_register_constraint (enum constraint_num c)
{
  return c >= CONSTRAINT_r && c <= CONSTRAINT_kb;
}

static inline bool
insn_extra_memory_constraint (enum constraint_num c)
{
  return c >= CONSTRAINT_m && c <= CONSTRAINT_ZW;
}

static inline bool
insn_extra_special_memory_constraint (enum constraint_num)
{
  return false;
}

static inline bool
insn_extra_address_constraint (enum constraint_num c)
{
  return c >= CONSTRAINT_p && c <= CONSTRAINT_ZD;
}

static inline void
insn_extra_constraint_allows_reg_mem (enum constraint_num c,
				      bool *allows_reg, bool *allows_mem)
{
  if (c >= CONSTRAINT_kf && c <= CONSTRAINT_Urv8)
    return;
  if (c >= CONSTRAINT_ks && c <= CONSTRAINT_S)
    {
      *allows_reg = true;
      return;
    }
  if (c >= CONSTRAINT_V && c <= CONSTRAINT__g)
    {
      *allows_mem = true;
      return;
    }
  (void) c;
  *allows_reg = true;
  *allows_mem = true;
}

static inline size_t
insn_constraint_len (char fc, const char *str ATTRIBUTE_UNUSED)
{
  switch (fc)
    {
    case 'U': return 4;
    case 'Y': return 2;
    case 'Z': return 2;
    case 'k': return 2;
    default: break;
    }
  return 1;
}

#define CONSTRAINT_LEN(c_,s_) insn_constraint_len (c_,s_)

extern enum reg_class reg_class_for_constraint_1 (enum constraint_num);

static inline enum reg_class
reg_class_for_constraint (enum constraint_num c)
{
  if (insn_extra_register_constraint (c))
    return reg_class_for_constraint_1 (c);
  return NO_REGS;
}

extern bool insn_const_int_ok_for_constraint (HOST_WIDE_INT, enum constraint_num);
#define CONST_OK_FOR_CONSTRAINT_P(v_,c_,s_) \
    insn_const_int_ok_for_constraint (v_, lookup_constraint (s_))

enum constraint_type
{
  CT_REGISTER,
  CT_CONST_INT,
  CT_MEMORY,
  CT_SPECIAL_MEMORY,
  CT_ADDRESS,
  CT_FIXED_FORM
};

static inline enum constraint_type
get_constraint_type (enum constraint_num c)
{
  if (c >= CONSTRAINT_p)
    {
      if (c >= CONSTRAINT_kf)
        return CT_FIXED_FORM;
      return CT_ADDRESS;
    }
  if (c >= CONSTRAINT_m)
    return CT_MEMORY;
  if (c >= CONSTRAINT_I)
    return CT_CONST_INT;
  return CT_REGISTER;
}
#endif /* tm-preds.h */
