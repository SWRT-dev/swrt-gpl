/*
 * This file is part of ltrace.
 * Copyright (C) 2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2010 Zach Welch, CodeSourcery
 * Copyright (C) 2004,2008,2009 Juan Cespedes
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

#include <gelf.h>
#include <stdio.h>
#include <string.h>

#include "proc.h"
#include "library.h"
#include "ltrace-elf.h"

static int
get_hardfp(uint64_t abi_vfp_args)
{
	if (abi_vfp_args == 2)
		fprintf(stderr,
			"Tag_ABI_VFP_args value 2 (tool chain-specific "
			"conventions) not supported.\n");
	return abi_vfp_args == 1;
}

int
arch_elf_init(struct ltelf *lte, struct library *lib)
{
	GElf_Addr jmprel_addr;
	Elf_Scn *jmprel_sec;
	GElf_Shdr jmprel_shdr;
	if (elf_load_dynamic_entry(lte, DT_JMPREL, &jmprel_addr) < 0
	    || elf_get_section_covering(lte, jmprel_addr,
					&jmprel_sec, &jmprel_shdr) < 0
	    || jmprel_sec == NULL)
		return -1;

	lte->arch.jmprel_data = elf_loaddata(jmprel_sec, &jmprel_shdr);
	if (lte->arch.jmprel_data == NULL)
		return -1;

	/* Nothing in this section is strictly critical.  It's not
	 * that much of a deal if we fail to guess right whether the
	 * ABI is softfp or hardfp.  */
	unsigned hardfp = 0;

	Elf_Scn *scn;
	Elf_Data *data;
	GElf_Shdr shdr;
	if (elf_get_section_type(lte, SHT_ARM_ATTRIBUTES, &scn, &shdr) < 0
	    || (scn != NULL && (data = elf_loaddata(scn, &shdr)) == NULL)) {
		fprintf(stderr,
			"Error when obtaining ARM attribute section: %s\n",
			elf_errmsg(-1));
		goto done;

	} else if (scn != NULL && data != NULL) {
		GElf_Xword offset = 0;
		uint8_t version;
		if (elf_read_next_u8(data, &offset, &version) < 0) {
			goto done;
		} else if (version != 'A') {
			fprintf(stderr, "Unsupported ARM attribute section "
				"version %d ('%c').\n", version, version);
			goto done;
		}

		do {
			const char signature[] = "aeabi";
			/* N.B. LEN is including the length field
			 * itself.  */
			uint32_t sec_len;
			if (elf_read_u32(data, offset, &sec_len) < 0
			    || !elf_can_read_next(data, offset, sec_len)) {
				goto done;
			}
			const GElf_Xword next_offset = offset + sec_len;
			offset += 4;

			if (sec_len < 4 + sizeof signature
			    || strcmp(signature, data->d_buf + offset) != 0)
				goto skip;
			offset += sizeof signature;

			const GElf_Xword offset0 = offset;
			uint64_t tag;
			uint32_t sub_len;
			if (elf_read_next_uleb128(data, &offset, &tag) < 0
			    || elf_read_next_u32(data, &offset, &sub_len) < 0
			    || !elf_can_read_next(data, offset0, sub_len))
				goto done;

			if (tag != 1)
				/* IHI0045D_ABI_addenda: "section and
				 * symbol attributes are deprecated
				 * [...] consumers are permitted to
				 * ignore them."  */
				goto skip;

			while (offset < offset0 + sub_len) {
				if (elf_read_next_uleb128(data,
							  &offset, &tag) < 0)
					goto done;

				switch (tag) {
					uint64_t v;
				case 6: /* Tag_CPU_arch */
				case 7: /* Tag_CPU_arch_profile */
				case 8: /* Tag_ARM_ISA_use */
				case 9: /* Tag_THUMB_ISA_use */
				case 10: /* Tag_FP_arch */
				case 11: /* Tag_WMMX_arch */
				case 12: /* Tag_Advanced_SIMD_arch */
				case 13: /* Tag_PCS_config */
				case 14: /* Tag_ABI_PCS_R9_use */
				case 15: /* Tag_ABI_PCS_RW_data */
				case 16: /* Tag_ABI_PCS_RO_data */
				case 17: /* Tag_ABI_PCS_GOT_use */
				case 18: /* Tag_ABI_PCS_wchar_t */
				case 19: /* Tag_ABI_FP_rounding */
				case 20: /* Tag_ABI_FP_denormal */
				case 21: /* Tag_ABI_FP_exceptions */
				case 22: /* Tag_ABI_FP_user_exceptions */
				case 23: /* Tag_ABI_FP_number_model */
				case 24: /* Tag_ABI_align_needed */
				case 25: /* Tag_ABI_align_preserved */
				case 26: /* Tag_ABI_enum_size */
				case 27: /* Tag_ABI_HardFP_use */
				case 28: /* Tag_ABI_VFP_args */
				case 29: /* Tag_ABI_WMMX_args */
				case 30: /* Tag_ABI_optimization_goals */
				case 31: /* Tag_ABI_FP_optimization_goals */
				case 32: /* Tag_compatibility */
				case 34: /* Tag_CPU_unaligned_access */
				case 36: /* Tag_FP_HP_extension */
				case 38: /* Tag_ABI_FP_16bit_format */
				case 42: /* Tag_MPextension_use */
				case 70: /* Tag_MPextension_use as well */
				case 44: /* Tag_DIV_use */
				case 64: /* Tag_nodefaults */
				case 66: /* Tag_T2EE_use */
				case 68: /* Tag_Virtualization_use */
				uleb128:
					if (elf_read_next_uleb128
						(data, &offset, &v) < 0)
						goto done;
					if (tag == 28)
						hardfp = get_hardfp(v);
					if (tag != 32)
						continue;

					/* Tag 32 has two arguments,
					 * fall through.  */

				case 4:	/* Tag_CPU_raw_name */
				case 5:	/* Tag_CPU_name */
				case 65: /* Tag_also_compatible_with */
				case 67: /* Tag_conformance */
				ntbs:
					offset += strlen(data->d_buf
							 + offset) + 1;
					continue;
				}

				/* Handle unknown tags in a generic
				 * manner, if possible.  */
				if (tag <= 32) {
					fprintf(stderr,
						"Unknown tag %lld "
						"at offset %#llx "
						"of ARM attribute section.",
						tag, offset);
					goto skip;
				} else if (tag % 2 == 0) {
					goto uleb128;
				} else {
					goto ntbs;
				}
			}

		skip:
			offset = next_offset;

		} while (elf_can_read_next(data, offset, 1));

	}

done:
	lib->arch.hardfp = hardfp;
	return 0;
}

void
arch_elf_destroy(struct ltelf *lte)
{
}

static int
arch_plt_entry_has_stub(struct ltelf *lte, size_t off) {
	char *buf = (char *) lte->arch.jmprel_data->d_buf;
	uint16_t op = *(uint16_t *) (buf + off);
	return op == 0x4778;
}

GElf_Addr
arch_plt_sym_val(struct ltelf *lte, size_t ndx, GElf_Rela * rela) {
	size_t start = lte->arch.jmprel_data->d_size + 12;
	size_t off = start + 20, i;
	for (i = 0; i < ndx; i++)
		off += arch_plt_entry_has_stub(lte, off) ? 16 : 12;
	if (arch_plt_entry_has_stub(lte, off))
		off += 4;
	return lte->plt_addr + off - start;
}

void *
sym2addr(struct process *proc, struct library_symbol *sym)
{
	return sym->enter_addr;
}

int
arch_library_init(struct library *lib)
{
	return 0;
}

void
arch_library_destroy(struct library *lib)
{
}

int
arch_library_clone(struct library *retp, struct library *lib)
{
	retp->arch = lib->arch;
	return 0;
}
