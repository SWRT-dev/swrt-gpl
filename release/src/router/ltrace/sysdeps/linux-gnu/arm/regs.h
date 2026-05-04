/*
 * This file is part of ltrace.
 * Copyright (C) 2013 Petr Machata, Red Hat Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#define SUBMASK(x) ((1L << ((x) + 1)) - 1)
#define BIT(obj,st) (((obj) >> (st)) & 1)
#define BITS(obj,st,fn) (((obj) >> (st)) & SUBMASK((fn) - (st)))
#define SBITS(obj,st,fn) \
	((long) (BITS(obj,st,fn) | ((long) BIT(obj,fn) * ~ SUBMASK(fn - st))))

enum arm_register {
	ARM_REG_R7 = 7,
	ARM_REG_IP = 12,
	ARM_REG_SP = 13,
	ARM_REG_LR = 14,
	ARM_REG_PC = 15,
	ARM_REG_CPSR = 16,
};

/* Write value of register REG to *LP.  Return 0 on success or a
 * negative value on failure.  */
int arm_get_register(struct process *proc, enum arm_register reg, uint32_t *lp);

/* Same as above, but if REG==ARM_REG_PC, it returns the value +8.  */
int arm_get_register_offpc(struct process *proc, enum arm_register reg,
			   uint32_t *lp);

/* Same as arm_get_register, but shift is performed depending on
 * instruction INST.  */
int arm_get_shifted_register(struct process *proc, uint32_t inst, int carry,
			     arch_addr_t pc, uint32_t *lp);
