/*
 * This file is part of ltrace.
 * Copyright (C) 2006,2010,2011,2012,2013 Petr Machata, Red Hat Inc.
 * Copyright (C) 2010 Zachary T Welch, CodeSourcery
 * Copyright (C) 2010 Joe Damato
 * Copyright (C) 1997,1998,2001,2004,2007,2008,2009 Juan Cespedes
 * Copyright (C) 2006 Olaf Hering, SUSE Linux GmbH
 * Copyright (C) 2006 Eric Vaitl, Cisco Systems, Inc.
 * Copyright (C) 2006 Paul Gilliam, IBM Corporation
 * Copyright (C) 2006 Ian Wienand
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

#include "config.h"

#include <assert.h>
#ifdef	__linux__
#include <endian.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <inttypes.h>
#include <search.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "backend.h"
#include "filter.h"
#include "library.h"
#include "ltrace-elf.h"
#include "proc.h"
#include "debug.h"
#include "options.h"

#ifndef ARCH_HAVE_LTELF_DATA
int
arch_elf_init(struct ltelf *lte, struct library *lib)
{
	return 0;
}

void
arch_elf_destroy(struct ltelf *lte)
{
}
#endif

#ifndef OS_HAVE_ADD_PLT_ENTRY
enum plt_status
os_elf_add_plt_entry(struct process *proc, struct ltelf *lte,
		     const char *a_name, GElf_Rela *rela, size_t ndx,
		     struct library_symbol **ret)
{
	return PLT_DEFAULT;
}
#endif

#ifndef ARCH_HAVE_ADD_PLT_ENTRY
enum plt_status
arch_elf_add_plt_entry(struct process *proc, struct ltelf *lte,
		       const char *a_name, GElf_Rela *rela, size_t ndx,
		       struct library_symbol **ret)
{
	return PLT_DEFAULT;
}
#endif

#ifndef OS_HAVE_ADD_FUNC_ENTRY
enum plt_status
os_elf_add_func_entry(struct process *proc, struct ltelf *lte,
		      const GElf_Sym *sym,
		      arch_addr_t addr, const char *name,
		      struct library_symbol **ret)
{
	if (GELF_ST_TYPE(sym->st_info) != STT_FUNC) {
		*ret = NULL;
		return PLT_OK;
	} else {
		return PLT_DEFAULT;
	}
}
#endif

#ifndef ARCH_HAVE_ADD_FUNC_ENTRY
enum plt_status
arch_elf_add_func_entry(struct process *proc, struct ltelf *lte,
			const GElf_Sym *sym,
			arch_addr_t addr, const char *name,
			struct library_symbol **ret)
{
	return PLT_DEFAULT;
}
#endif

Elf_Data *
elf_loaddata(Elf_Scn *scn, GElf_Shdr *shdr)
{
	Elf_Data *data = elf_getdata(scn, NULL);
	if (data == NULL || elf_getdata(scn, data) != NULL
	    || data->d_off || data->d_size != shdr->sh_size)
		return NULL;
	return data;
}

static int
elf_get_section_if(struct ltelf *lte, Elf_Scn **tgt_sec, GElf_Shdr *tgt_shdr,
		   int (*predicate)(Elf_Scn *, GElf_Shdr *, void *data),
		   void *data)
{
	int i;
	for (i = 1; i < lte->ehdr.e_shnum; ++i) {
		Elf_Scn *scn;
		GElf_Shdr shdr;

		scn = elf_getscn(lte->elf, i);
		if (scn == NULL || gelf_getshdr(scn, &shdr) == NULL) {
			debug(1, "Couldn't read section or header.");
			return -1;
		}
		if (predicate(scn, &shdr, data)) {
			*tgt_sec = scn;
			*tgt_shdr = shdr;
			return 0;
		}
	}

	*tgt_sec = NULL;
	return 0;
}

static int
inside_p(Elf_Scn *scn, GElf_Shdr *shdr, void *data)
{
	GElf_Addr addr = *(GElf_Addr *)data;
	return addr >= shdr->sh_addr
		&& addr < shdr->sh_addr + shdr->sh_size;
}

int
elf_get_section_covering(struct ltelf *lte, GElf_Addr addr,
			 Elf_Scn **tgt_sec, GElf_Shdr *tgt_shdr)
{
	return elf_get_section_if(lte, tgt_sec, tgt_shdr,
				  &inside_p, &addr);
}

static int
type_p(Elf_Scn *scn, GElf_Shdr *shdr, void *data)
{
	GElf_Word type = *(GElf_Word *)data;
	return shdr->sh_type == type;
}

int
elf_get_section_type(struct ltelf *lte, GElf_Word type,
		     Elf_Scn **tgt_sec, GElf_Shdr *tgt_shdr)
{
	return elf_get_section_if(lte, tgt_sec, tgt_shdr,
				  &type_p, &type);
}

struct section_named_data {
	struct ltelf *lte;
	const char *name;
};

static int
name_p(Elf_Scn *scn, GElf_Shdr *shdr, void *d)
{
	struct section_named_data *data = d;
	const char *name = elf_strptr(data->lte->elf,
				      data->lte->ehdr.e_shstrndx,
				      shdr->sh_name);
	return strcmp(name, data->name) == 0;
}

int
elf_get_section_named(struct ltelf *lte, const char *name,
		     Elf_Scn **tgt_sec, GElf_Shdr *tgt_shdr)
{
	struct section_named_data data = {
		.lte = lte,
		.name = name,
	};
	return elf_get_section_if(lte, tgt_sec, tgt_shdr,
				  &name_p, &data);
}

static struct elf_each_symbol_t
each_symbol_in(Elf_Data *symtab, const char *strtab, size_t count,
	       unsigned i,
	       enum callback_status (*cb)(GElf_Sym *symbol,
					  const char *name, void *data),
	       void *data)
{
	for (; i < count; ++i) {
		GElf_Sym sym;
		if (gelf_getsym(symtab, i, &sym) == NULL)
			return (struct elf_each_symbol_t){ i, -2 };

		switch (cb(&sym, strtab + sym.st_name, data)) {
		case CBS_FAIL:
			return (struct elf_each_symbol_t){ i, -1 };
		case CBS_STOP:
			return (struct elf_each_symbol_t){ i + 1, 0 };
		case CBS_CONT:
			break;
		}
	}

	return (struct elf_each_symbol_t){ 0, 0 };
}

/* N.B.: gelf_getsym takes integer argument.  Since negative values
 * are invalid as indices, we can use the extra bit to encode which
 * symbol table we are looking into.  ltrace currently doesn't handle
 * more than two symbol tables anyway, nor does it handle the xindex
 * stuff.  */
struct elf_each_symbol_t
elf_each_symbol(struct ltelf *lte, unsigned start_after,
		enum callback_status (*cb)(GElf_Sym *symbol,
					   const char *name, void *data),
		void *data)
{
	unsigned index = start_after == 0 ? 0 : start_after >> 1;

	/* Go through static symbol table first.  */
	if ((start_after & 0x1) == 0) {
		struct elf_each_symbol_t st
			= each_symbol_in(lte->symtab, lte->strtab,
					 lte->symtab_count, index, cb, data);

		/* If the iteration stopped prematurely, bail out.  */
		if (st.restart != 0)
			return ((struct elf_each_symbol_t)
				{ st.restart << 1, st.status });
	}

	struct elf_each_symbol_t st
		= each_symbol_in(lte->dynsym, lte->dynstr, lte->dynsym_count,
				 index, cb, data);
	if (st.restart != 0)
		return ((struct elf_each_symbol_t)
			{ st.restart << 1 | 0x1, st.status });

	return (struct elf_each_symbol_t){ 0, 0 };
}

int
elf_can_read_next(Elf_Data *data, GElf_Xword offset, GElf_Xword size)
{
	assert(data != NULL);
	if (data->d_size < size || offset > data->d_size - size) {
		debug(1, "Not enough data to read %"PRId64"-byte value"
		      " at offset %"PRId64".", size, offset);
		return 0;
	}
	return 1;
}

#define DEF_READER(NAME, SIZE)						\
	int								\
	NAME(Elf_Data *data, GElf_Xword offset, uint##SIZE##_t *retp)	\
	{								\
		if (!elf_can_read_next(data, offset, SIZE / 8))		\
			return -1;					\
									\
		if (data->d_buf == NULL) /* NODATA section */ {		\
			*retp = 0;					\
			return 0;					\
		}							\
									\
		union {							\
			uint##SIZE##_t dst;				\
			char buf[0];					\
		} u;							\
		memcpy(u.buf, data->d_buf + offset, sizeof(u.dst));	\
		*retp = u.dst;						\
		return 0;						\
	}

DEF_READER(elf_read_u8, 8)
DEF_READER(elf_read_u16, 16)
DEF_READER(elf_read_u32, 32)
DEF_READER(elf_read_u64, 64)

#undef DEF_READER

#define DEF_READER(NAME, SIZE)						\
	int								\
	NAME(Elf_Data *data, GElf_Xword *offset, uint##SIZE##_t *retp)	\
	{								\
		int rc = elf_read_u##SIZE(data, *offset, retp);		\
		if (rc < 0)						\
			return rc;					\
		*offset += SIZE / 8;					\
		return 0;						\
	}

DEF_READER(elf_read_next_u8, 8)
DEF_READER(elf_read_next_u16, 16)
DEF_READER(elf_read_next_u32, 32)
DEF_READER(elf_read_next_u64, 64)

#undef DEF_READER

int
elf_read_next_uleb128(Elf_Data *data, GElf_Xword *offset, uint64_t *retp)
{
	uint64_t result = 0;
	int shift = 0;
	int size = 8 * sizeof result;

	while (1) {
		uint8_t byte;
		if (elf_read_next_u8(data, offset, &byte) < 0)
			return -1;

		uint8_t payload = byte & 0x7f;
		result |= (uint64_t)payload << shift;
		shift += 7;
		if (shift > size && byte != 0x1)
			return -1;
		if ((byte & 0x80) == 0)
			break;
	}

	if (retp != NULL)
		*retp = result;
	return 0;
}

int
elf_read_uleb128(Elf_Data *data, GElf_Xword offset, uint64_t *retp)
{
	return elf_read_next_uleb128(data, &offset, retp);
}

int
ltelf_init(struct ltelf *lte, const char *filename)
{
	memset(lte, 0, sizeof *lte);
	lte->fd = open(filename, O_RDONLY);
	if (lte->fd == -1) {
		fprintf(stderr, "Can't open %s: %s\n", filename,
			strerror(errno));
		return 1;
	}

	elf_version(EV_CURRENT);

#ifdef HAVE_ELF_C_READ_MMAP
	lte->elf = elf_begin(lte->fd, ELF_C_READ_MMAP, NULL);
#else
	lte->elf = elf_begin(lte->fd, ELF_C_READ, NULL);
#endif

	if (lte->elf == NULL || elf_kind(lte->elf) != ELF_K_ELF) {
		fprintf(stderr, "\"%s\" is not an ELF file\n", filename);
		exit(EXIT_FAILURE);
	}

	if (gelf_getehdr(lte->elf, &lte->ehdr) == NULL) {
		fprintf(stderr, "can't read ELF header of \"%s\": %s\n",
			filename, elf_errmsg(-1));
		exit(EXIT_FAILURE);
	}

	if (lte->ehdr.e_type != ET_EXEC && lte->ehdr.e_type != ET_DYN) {
		fprintf(stderr, "\"%s\" is neither an ELF executable"
			" nor a shared library\n", filename);
		exit(EXIT_FAILURE);
	}

	if (1
#ifdef LT_ELF_MACHINE
	    && (lte->ehdr.e_ident[EI_CLASS] != LT_ELFCLASS
		|| lte->ehdr.e_machine != LT_ELF_MACHINE)
#endif
#ifdef LT_ELF_MACHINE2
	    && (lte->ehdr.e_ident[EI_CLASS] != LT_ELFCLASS2
		|| lte->ehdr.e_machine != LT_ELF_MACHINE2)
#endif
#ifdef LT_ELF_MACHINE3
	    && (lte->ehdr.e_ident[EI_CLASS] != LT_ELFCLASS3
		|| lte->ehdr.e_machine != LT_ELF_MACHINE3)
#endif
		) {
		fprintf(stderr,
			"\"%s\" is ELF from incompatible architecture\n",
			filename);
		exit(EXIT_FAILURE);
	}

	VECT_INIT(&lte->plt_relocs, GElf_Rela);

	return 0;
}

void
ltelf_destroy(struct ltelf *lte)
{
	debug(DEBUG_FUNCTION, "close_elf()");
	elf_end(lte->elf);
	close(lte->fd);
	VECT_DESTROY(&lte->plt_relocs, GElf_Rela, NULL, NULL);
}

static void
read_symbol_table(struct ltelf *lte, const char *filename,
		  Elf_Scn *scn, GElf_Shdr *shdr, const char *name,
		  Elf_Data **datap, size_t *countp, const char **strsp)
{
	*datap = elf_getdata(scn, NULL);
	*countp = shdr->sh_size / shdr->sh_entsize;
	if ((*datap == NULL || elf_getdata(scn, *datap) != NULL)
	    && options.static_filter != NULL) {
		fprintf(stderr, "Couldn't get data of section"
			" %s from \"%s\": %s\n",
			name, filename, elf_errmsg(-1));
		exit(EXIT_FAILURE);
	}

	scn = elf_getscn(lte->elf, shdr->sh_link);
	GElf_Shdr shdr2;
	if (scn == NULL || gelf_getshdr(scn, &shdr2) == NULL) {
		fprintf(stderr, "Couldn't get header of section"
			" #%d from \"%s\": %s\n",
			shdr->sh_link, filename, elf_errmsg(-1));
		exit(EXIT_FAILURE);
	}

	Elf_Data *data = elf_getdata(scn, NULL);
	if (data == NULL || elf_getdata(scn, data) != NULL
	    || shdr2.sh_size != data->d_size || data->d_off) {
		fprintf(stderr, "Couldn't get data of section"
			" #%d from \"%s\": %s\n",
			shdr2.sh_link, filename, elf_errmsg(-1));
		exit(EXIT_FAILURE);
	}

	*strsp = data->d_buf;
}

static int
rel_to_rela(struct ltelf *lte, const GElf_Rel *rel, GElf_Rela *rela)
{
	rela->r_offset = rel->r_offset;
	rela->r_info = rel->r_info;

	Elf_Scn *sec;
	GElf_Shdr shdr;
	if (elf_get_section_covering(lte, rel->r_offset, &sec, &shdr) < 0
	    || sec == NULL)
		return -1;

	Elf_Data *data = elf_loaddata(sec, &shdr);
	if (data == NULL)
		return -1;

	GElf_Xword offset = rel->r_offset - shdr.sh_addr - data->d_off;
	uint64_t value;
	if (lte->ehdr.e_ident[EI_CLASS] == ELFCLASS32) {
		uint32_t tmp;
		if (elf_read_u32(data, offset, &tmp) < 0)
			return -1;
		value = tmp;
	} else if (elf_read_u64(data, offset, &value) < 0) {
		return -1;
	}

	rela->r_addend = value;
	return 0;
}

int
elf_read_relocs(struct ltelf *lte, Elf_Scn *scn, GElf_Shdr *shdr,
		struct vect *rela_vec)
{
	if (vect_reserve_additional(rela_vec, lte->ehdr.e_shnum) < 0)
		return -1;

	Elf_Data *relplt = elf_loaddata(scn, shdr);
	if (relplt == NULL) {
		fprintf(stderr, "Couldn't load .rel*.plt data.\n");
		return -1;
	}

	if ((shdr->sh_size % shdr->sh_entsize) != 0) {
		fprintf(stderr, ".rel*.plt size (%" PRIx64 "d) not a multiple "
			"of its sh_entsize (%" PRIx64 "d).\n",
			shdr->sh_size, shdr->sh_entsize);
		return -1;
	}

	GElf_Xword relplt_count = shdr->sh_size / shdr->sh_entsize;
	GElf_Xword i;
	for (i = 0; i < relplt_count; ++i) {
		GElf_Rela rela;
		if (relplt->d_type == ELF_T_REL) {
			GElf_Rel rel;
			if (gelf_getrel(relplt, i, &rel) == NULL
			    || rel_to_rela(lte, &rel, &rela) < 0)
				return -1;

		} else if (gelf_getrela(relplt, i, &rela) == NULL) {
			return -1;
		}

		if (VECT_PUSHBACK(rela_vec, &rela) < 0)
			return -1;
	}

	return 0;
}

int
elf_load_dynamic_entry(struct ltelf *lte, int tag, GElf_Addr *valuep)
{
	Elf_Scn *scn;
	GElf_Shdr shdr;
	if (elf_get_section_type(lte, SHT_DYNAMIC, &scn, &shdr) < 0
	    || scn == NULL) {
	fail:
		fprintf(stderr, "Couldn't get SHT_DYNAMIC: %s\n",
			elf_errmsg(-1));
		return -1;
	}

	Elf_Data *data = elf_loaddata(scn, &shdr);
	if (data == NULL)
		goto fail;

	size_t j;
	for (j = 0; j < shdr.sh_size / shdr.sh_entsize; ++j) {
		GElf_Dyn dyn;
		if (gelf_getdyn(data, j, &dyn) == NULL)
			goto fail;

		if(dyn.d_tag == tag) {
			*valuep = dyn.d_un.d_ptr;
			return 0;
		}
	}

	return -1;
}

static int
ltelf_read_elf(struct ltelf *lte, const char *filename)
{
	int i;
	GElf_Addr relplt_addr = 0;
	GElf_Addr soname_offset = 0;
	GElf_Xword relplt_size = 0;

	debug(DEBUG_FUNCTION, "ltelf_read_elf(filename=%s)", filename);
	debug(1, "Reading ELF from %s...", filename);

	for (i = 1; i < lte->ehdr.e_shnum; ++i) {
		Elf_Scn *scn;
		GElf_Shdr shdr;
		const char *name;

		scn = elf_getscn(lte->elf, i);
		if (scn == NULL || gelf_getshdr(scn, &shdr) == NULL) {
			fprintf(stderr,	"Couldn't get section #%d from"
				" \"%s\": %s\n", i, filename, elf_errmsg(-1));
			exit(EXIT_FAILURE);
		}

		name = elf_strptr(lte->elf, lte->ehdr.e_shstrndx, shdr.sh_name);
		if (name == NULL) {
			fprintf(stderr,	"Couldn't get name of section #%d from"
				" \"%s\": %s\n", i, filename, elf_errmsg(-1));
			exit(EXIT_FAILURE);
		}

		if (shdr.sh_type == SHT_SYMTAB) {
			read_symbol_table(lte, filename,
					  scn, &shdr, name, &lte->symtab,
					  &lte->symtab_count, &lte->strtab);

		} else if (shdr.sh_type == SHT_DYNSYM) {
			read_symbol_table(lte, filename,
					  scn, &shdr, name, &lte->dynsym,
					  &lte->dynsym_count, &lte->dynstr);

		} else if (shdr.sh_type == SHT_DYNAMIC) {
			Elf_Data *data;
			size_t j;

			lte->dyn_addr = shdr.sh_addr + lte->bias;
			lte->dyn_sz = shdr.sh_size;

			data = elf_getdata(scn, NULL);
			if (data == NULL || elf_getdata(scn, data) != NULL) {
				fprintf(stderr, "Couldn't get .dynamic data"
					" from \"%s\": %s\n",
					filename, strerror(errno));
				exit(EXIT_FAILURE);
			}

			for (j = 0; j < shdr.sh_size / shdr.sh_entsize; ++j) {
				GElf_Dyn dyn;

				if (gelf_getdyn(data, j, &dyn) == NULL) {
					fprintf(stderr, "Couldn't get .dynamic"
						" data from \"%s\": %s\n",
						filename, strerror(errno));
					exit(EXIT_FAILURE);
				}
				if (dyn.d_tag == DT_JMPREL)
					relplt_addr = dyn.d_un.d_ptr;
				else if (dyn.d_tag == DT_PLTRELSZ)
					relplt_size = dyn.d_un.d_val;
				else if (dyn.d_tag == DT_SONAME)
					soname_offset = dyn.d_un.d_val;
			}
		} else if (shdr.sh_type == SHT_PROGBITS
			   || shdr.sh_type == SHT_NOBITS) {
			if (strcmp(name, ".plt") == 0) {
				lte->plt_addr = shdr.sh_addr;
				lte->plt_size = shdr.sh_size;
				lte->plt_data = elf_loaddata(scn, &shdr);
				if (lte->plt_data == NULL)
					fprintf(stderr,
						"Can't load .plt data\n");
				lte->plt_flags = shdr.sh_flags;
			}
#ifdef ARCH_SUPPORTS_OPD
			else if (strcmp(name, ".opd") == 0) {
				lte->opd_addr = (GElf_Addr *) (long) shdr.sh_addr;
				lte->opd_size = shdr.sh_size;
				lte->opd = elf_rawdata(scn, NULL);
			}
#endif
		}
	}

	if (lte->dynsym == NULL || lte->dynstr == NULL) {
		fprintf(stderr, "Couldn't find .dynsym or .dynstr in \"%s\"\n",
			filename);
		exit(EXIT_FAILURE);
	}

	if (!relplt_addr || !lte->plt_addr) {
		debug(1, "%s has no PLT relocations", filename);
	} else if (relplt_size == 0) {
		debug(1, "%s has unknown PLT size", filename);
	} else {
		for (i = 1; i < lte->ehdr.e_shnum; ++i) {
			Elf_Scn *scn;
			GElf_Shdr shdr;

			scn = elf_getscn(lte->elf, i);
			if (scn == NULL || gelf_getshdr(scn, &shdr) == NULL) {
				fprintf(stderr, "Couldn't get section header"
					" from \"%s\": %s\n",
					filename, elf_errmsg(-1));
				exit(EXIT_FAILURE);
			}
			if (shdr.sh_addr == relplt_addr
			    && shdr.sh_size == relplt_size) {
				if (elf_read_relocs(lte, scn, &shdr,
						    &lte->plt_relocs) < 0) {
					fprintf(stderr, "Couldn't get .rel*.plt"
						" data from \"%s\": %s\n",
						filename, elf_errmsg(-1));
					exit(EXIT_FAILURE);
				}
				break;
			}
		}

		if (i == lte->ehdr.e_shnum) {
			fprintf(stderr,
				"Couldn't find .rel*.plt section in \"%s\"\n",
				filename);
			exit(EXIT_FAILURE);
		}
	}
	debug(1, "%s %zd PLT relocations", filename,
	      vect_size(&lte->plt_relocs));

	if (soname_offset != 0)
		lte->soname = lte->dynstr + soname_offset;

	return 0;
}

#ifndef ARCH_HAVE_GET_SYMINFO
int
arch_get_sym_info(struct ltelf *lte, const char *filename,
		  size_t sym_index, GElf_Rela *rela, GElf_Sym *sym)
{
	return gelf_getsym(lte->dynsym,
			   ELF64_R_SYM(rela->r_info), sym) != NULL ? 0 : -1;
}
#endif

int
default_elf_add_plt_entry(struct process *proc, struct ltelf *lte,
			  const char *a_name, GElf_Rela *rela, size_t ndx,
			  struct library_symbol **ret)
{
	char *name = strdup(a_name);
	if (name == NULL) {
	fail_message:
		fprintf(stderr, "Couldn't create symbol for PLT entry: %s\n",
			strerror(errno));
	fail:
		free(name);
		return -1;
	}

	GElf_Addr addr = arch_plt_sym_val(lte, ndx, rela);

	struct library_symbol *libsym = malloc(sizeof(*libsym));
	if (libsym == NULL)
		goto fail_message;

	/* XXX The double cast should be removed when
	 * arch_addr_t becomes integral type.  */
	arch_addr_t taddr = (arch_addr_t)
		(uintptr_t)(addr + lte->bias);

	if (library_symbol_init(libsym, taddr, name, 1, LS_TOPLT_EXEC) < 0) {
		free(libsym);
		goto fail;
	}

	libsym->next = *ret;
	*ret = libsym;
	return 0;
}

int
elf_add_plt_entry(struct process *proc, struct ltelf *lte,
		  const char *name, GElf_Rela *rela, size_t idx,
		  struct library_symbol **ret)
{
	enum plt_status plts
		= arch_elf_add_plt_entry(proc, lte, name, rela, idx, ret);

	if (plts == PLT_DEFAULT)
		plts = os_elf_add_plt_entry(proc, lte, name, rela, idx, ret);

	switch (plts) {
	case PLT_DEFAULT:
		return default_elf_add_plt_entry(proc, lte, name,
						 rela, idx, ret);
	case PLT_FAIL:
		return -1;
	case PLT_OK:
		return 0;
	}

	assert(! "Invalid return from X_elf_add_plt_entry!");
	abort();
}

static void
mark_chain_latent(struct library_symbol *libsym)
{
	for (; libsym != NULL; libsym = libsym->next) {
		debug(DEBUG_FUNCTION, "marking %s latent", libsym->name);
		libsym->latent = 1;
	}
}

static void
filter_symbol_chain(struct filter *filter,
		    struct library_symbol **libsymp, struct library *lib)
{
	assert(libsymp != NULL);
	struct library_symbol **ptr = libsymp;
	while (*ptr != NULL) {
		if (filter_matches_symbol(filter, (*ptr)->name, lib)) {
			ptr = &(*ptr)->next;
		} else {
			struct library_symbol *sym = *ptr;
			*ptr = (*ptr)->next;
			library_symbol_destroy(sym);
			free(sym);
		}
	}
}

static int
populate_plt(struct process *proc, const char *filename,
	     struct ltelf *lte, struct library *lib)
{
	const bool latent_plts = options.export_filter != NULL;
	const size_t count = vect_size(&lte->plt_relocs);

	size_t i;
	for (i = 0; i < count; ++i) {
		GElf_Rela *rela = VECT_ELEMENT(&lte->plt_relocs, GElf_Rela, i);
		GElf_Sym sym;

		switch (arch_get_sym_info(lte, filename, i, rela, &sym)) {
		default:
			fprintf(stderr,
				"Couldn't get relocation for symbol #%zd"
				" from \"%s\": %s\n",
				i, filename, elf_errmsg(-1));
			/* Fall through.  */
		case 1:
			continue; /* Skip this entry.  */
		case 0:
			break;
		}

		char const *name = lte->dynstr + sym.st_name;
		int matched = filter_matches_symbol(options.plt_filter,
						    name, lib);

		struct library_symbol *libsym = NULL;
		if (elf_add_plt_entry(proc, lte, name, rela, i, &libsym) < 0)
			return -1;

		/* If we didn't match the PLT entry, filter the chain
		 * to only include the matching symbols (but include
		 * all if we are adding latent symbols) to allow
		 * backends to override the PLT symbol's name.  */

		if (! matched && ! latent_plts)
			filter_symbol_chain(options.plt_filter, &libsym, lib);

		if (libsym != NULL) {
			/* If we are adding those symbols just for
			 * tracing exports, mark them all latent.  */
			if (! matched && latent_plts)
				mark_chain_latent(libsym);
			library_add_symbol(lib, libsym);
		}
	}
	return 0;
}

void
delete_symbol_chain(struct library_symbol *libsym)
{
	while (libsym != NULL) {
		struct library_symbol *tmp = libsym->next;
		library_symbol_destroy(libsym);
		free(libsym);
		libsym = tmp;
	}
}

/* When -x rules result in request to trace several aliases, we only
 * want to add such symbol once.  The only way that those symbols
 * differ in is their name, e.g. in glibc you have __GI___libc_free,
 * __cfree, __free, __libc_free, cfree and free all defined on the
 * same address.  So instead we keep this unique symbol struct for
 * each address, and replace name in libsym with a shorter variant if
 * we find it.  */
struct unique_symbol {
	arch_addr_t addr;
	struct library_symbol *libsym;
};

static int
unique_symbol_cmp(const void *key, const void *val)
{
	const struct unique_symbol *sym_key = key;
	const struct unique_symbol *sym_val = val;
	return sym_key->addr != sym_val->addr;
}

static enum callback_status
symbol_with_address(struct library_symbol *sym, void *addrptr)
{
	return sym->enter_addr == *(arch_addr_t *)addrptr
		? CBS_STOP : CBS_CONT;
}

static int
populate_this_symtab(struct process *proc, const char *filename,
		     struct ltelf *lte, struct library *lib,
		     Elf_Data *symtab, const char *strtab, size_t count,
		     struct library_exported_names *names,
		     bool only_exported_names)
{
	/* Using sorted array would be arguably better, but this
	 * should be well enough for the number of symbols that we
	 * typically deal with.  */
	size_t num_symbols = 0;
	struct unique_symbol *symbols = NULL;

	if (!only_exported_names) {
		symbols = malloc(sizeof(*symbols) * count);
		if (symbols == NULL) {
			fprintf(stderr, "couldn't insert symbols for -x: %s\n",
				strerror(errno));
			return -1;
		}
	}

	GElf_Word secflags[lte->ehdr.e_shnum];
	size_t i;
	for (i = 1; i < lte->ehdr.e_shnum; ++i) {
		Elf_Scn *scn = elf_getscn(lte->elf, i);
		GElf_Shdr shdr;
		if (scn == NULL || gelf_getshdr(scn, &shdr) == NULL)
			secflags[i] = 0;
		else
			secflags[i] = shdr.sh_flags;
	}

	for (i = 0; i < count; ++i) {
		GElf_Sym sym;
		if (gelf_getsym(symtab, i, &sym) == NULL) {
			fprintf(stderr,
				"couldn't get symbol #%zd from %s: %s\n",
				i, filename, elf_errmsg(-1));
			continue;
		}

		if (sym.st_value == 0 || sym.st_shndx == STN_UNDEF
		    /* Also ignore any special values besides direct
		     * section references.  */
		    || sym.st_shndx >= lte->ehdr.e_shnum)
			continue;

		/* Find symbol name and snip version.  */
		const char *orig_name = strtab + sym.st_name;
		const char *version = strchr(orig_name, '@');
		size_t len = version != NULL ? (assert(version > orig_name),
						(size_t)(version - orig_name))
			: strlen(orig_name);
		char name[len + 1];
		memcpy(name, orig_name, len);
		name[len] = 0;

		/* If we are interested in exports, store this name.  */
		if (names != NULL) {
			char *name_copy = strdup(name);
			if (name_copy == NULL ||
			    library_exported_names_push(names,
							sym.st_value,
							name_copy, 1) != 0)
			{
				fprintf(stderr, "Couldn't store symbol %s.  "
					"Tracing may be incomplete.\n", name);
			}
		}

		/* If we're only dealing with the exported names list, there's
		 * nothing left to do with this symbol */
		if (only_exported_names)
			continue;

		/* If the symbol is not matched, skip it.  We already
		 * stored it to export list above.  */
		if (!filter_matches_symbol(options.static_filter, name, lib))
			continue;

		arch_addr_t addr = (arch_addr_t)
			(uintptr_t)(sym.st_value + lte->bias);
		arch_addr_t naddr;

		/* On arches that support OPD, the value of typical
		 * function symbol will be a pointer to .opd, but some
		 * will point directly to .text.  We don't want to
		 * translate those.  */
		if (secflags[sym.st_shndx] & SHF_EXECINSTR) {
			naddr = addr;
		} else if (arch_translate_address(lte, addr, &naddr) < 0) {
			fprintf(stderr,
				"couldn't translate address of %s@%s: %s\n",
				name, lib->soname, strerror(errno));
			continue;
		}

		char *full_name = strdup(name);
		if (full_name == NULL) {
			fprintf(stderr, "couldn't copy name of %s@%s: %s\n",
				name, lib->soname, strerror(errno));
			continue;
		}

		struct library_symbol *libsym = NULL;
		enum plt_status plts
			= arch_elf_add_func_entry(proc, lte, &sym,
						  naddr, full_name, &libsym);
		if (plts == PLT_DEFAULT)
			plts = os_elf_add_func_entry(proc, lte, &sym,
						     naddr, full_name, &libsym);

		switch (plts) {
		case PLT_DEFAULT:;
			/* Put the default symbol to the chain.  */
			struct library_symbol *tmp = malloc(sizeof *tmp);
			if (tmp == NULL
			    || library_symbol_init(tmp, naddr, full_name, 1,
						   LS_TOPLT_NONE) < 0) {
				free(tmp);

				/* Either add the whole bunch, or none
				 * of it.  Note that for PLT_FAIL we
				 * don't do this--it's the callee's
				 * job to clean up after itself before
				 * it bails out.  */
				delete_symbol_chain(libsym);
				libsym = NULL;

		case PLT_FAIL:
				fprintf(stderr, "Couldn't add symbol %s@%s "
					"for tracing.\n", name, lib->soname);

				break;
			}

			full_name = NULL;
			tmp->next = libsym;
			libsym = tmp;
			break;

		case PLT_OK:
			break;
		}

		free(full_name);

		struct library_symbol *tmp;
		for (tmp = libsym; tmp != NULL; ) {
			/* Look whether we already have a symbol for
			 * this address.  If not, add this one.  If
			 * yes, look if we should pick the new symbol
			 * name.  */

			struct unique_symbol key = { tmp->enter_addr, NULL };
			struct unique_symbol *unique
				= lsearch(&key, symbols, &num_symbols,
					  sizeof *symbols, &unique_symbol_cmp);

			if (unique->libsym == NULL) {
				unique->libsym = tmp;
				unique->addr = tmp->enter_addr;
				tmp = tmp->next;
				unique->libsym->next = NULL;
			} else {
				if (strlen(tmp->name)
				    < strlen(unique->libsym->name)) {
					library_symbol_set_name
						(unique->libsym, tmp->name, 1);
					tmp->name = NULL;
				}
				struct library_symbol *next = tmp->next;
				library_symbol_destroy(tmp);
				free(tmp);
				tmp = next;
			}
		}
	}

	/* If we're only dealing with the exported names list, there's nothing
	 * left to do */
	if (only_exported_names)
		return 0;


	/* Now we do the union of this set of unique symbols with
	 * what's already in the library.  */
	for (i = 0; i < num_symbols; ++i) {
		struct library_symbol *this_sym = symbols[i].libsym;
		assert(this_sym != NULL);
		struct library_symbol *other
			= library_each_symbol(lib, NULL, symbol_with_address,
					      &this_sym->enter_addr);
		if (other != NULL) {
			library_symbol_destroy(this_sym);
			free(this_sym);
			symbols[i].libsym = NULL;
		}
	}

	for (i = 0; i < num_symbols; ++i)
		if (symbols[i].libsym != NULL)
			library_add_symbol(lib, symbols[i].libsym);

	free(symbols);
	return 0;
}

static int
populate_symtab(struct process *proc, const char *filename,
		struct ltelf *lte, struct library *lib,
		int symtabs, int exports)
{
	int status;
	if (symtabs && lte->symtab != NULL && lte->strtab != NULL
	    && (status = populate_this_symtab(proc, filename, lte, lib,
					      lte->symtab, lte->strtab,
					      lte->symtab_count, NULL,
					      false)) < 0)
		return status;

	/* Check whether we want to trace symbols implemented by this
	 * library (-l).  */
	struct library_exported_names *names = &lib->exported_names;
	lib->should_activate_latent = exports != 0;

	bool only_exported_names = symtabs == 0 && exports == 0;
	return populate_this_symtab(proc, filename, lte, lib,
				    lte->dynsym, lte->dynstr,
				    lte->dynsym_count, names,
				    only_exported_names);
}

static int
read_module(struct library *lib, struct process *proc,
	    const char *filename, GElf_Addr bias, int main)
{
	struct ltelf lte;
	if (ltelf_init(&lte, filename) < 0)
		return -1;

	/* XXX When we abstract ABI into a module, this should instead
	 * become something like
	 *
	 *    proc->abi = arch_get_abi(lte.ehdr);
	 *
	 * The code in ltelf_init needs to be replaced by this logic.
	 * Be warned that libltrace.c calls ltelf_init as well to
	 * determine whether ABI is supported.  This is to get
	 * reasonable error messages when trying to run 64-bit binary
	 * with 32-bit ltrace.  It is desirable to preserve this.  */
	proc->e_machine = lte.ehdr.e_machine;
	proc->e_class = lte.ehdr.e_ident[EI_CLASS];
	get_arch_dep(proc);

	/* Find out the base address.  For PIE main binaries we look
	 * into auxv, otherwise we scan phdrs.  */
	if (main && lte.ehdr.e_type == ET_DYN) {
		arch_addr_t entry;
		if (process_get_entry(proc, &entry, NULL) < 0) {
			fprintf(stderr, "Couldn't find entry of PIE %s\n",
				filename);
		fail:
			ltelf_destroy(&lte);
			return -1;
		}
		/* XXX The double cast should be removed when
		 * arch_addr_t becomes integral type.  */
		lte.entry_addr = (GElf_Addr)(uintptr_t)entry;
		lte.bias = (GElf_Addr)(uintptr_t)entry - lte.ehdr.e_entry;

	} else {
		GElf_Phdr phdr;
		size_t i;
		for (i = 0; gelf_getphdr (lte.elf, i, &phdr) != NULL; ++i) {
			if (phdr.p_type == PT_LOAD) {
				lte.base_addr = phdr.p_vaddr + bias;
				break;
			}
		}

		lte.bias = bias;
		lte.entry_addr = lte.ehdr.e_entry + lte.bias;

		if (lte.base_addr == 0) {
			fprintf(stderr,
				"Couldn't determine base address of %s\n",
				filename);
			goto fail;
		}
	}

	if (ltelf_read_elf(&lte, filename) < 0)
		goto fail;

	if (arch_elf_init(&lte, lib) < 0) {
		fprintf(stderr, "Backend initialization failed.\n");
		goto fail;
	}

	if (lib == NULL)
		goto fail;

	/* Note that we set soname and pathname as soon as they are
	 * allocated, so in case of further errors, this get released
	 * when LIB is released, which should happen in the caller
	 * when we return error.  */

	if (lib->pathname == NULL) {
		char *pathname = strdup(filename);
		if (pathname == NULL)
			goto fail;
		library_set_pathname(lib, pathname, 1);
	}

	if (lte.soname != NULL) {
		char *soname = strdup(lte.soname);
		if (soname == NULL)
			goto fail;
		library_set_soname(lib, soname, 1);
	} else {
		const char *soname = strrchr(lib->pathname, '/');
		if (soname != NULL)
			soname += 1;
		else
			soname = lib->pathname;
		library_set_soname(lib, soname, 0);
	}

	/* XXX The double cast should be removed when
	 * arch_addr_t becomes integral type.  */
	arch_addr_t entry = (arch_addr_t)(uintptr_t)lte.entry_addr;
	if (arch_translate_address(&lte, entry, &entry) < 0)
		goto fail;

	/* XXX The double cast should be removed when
	 * arch_addr_t becomes integral type.  */
	lib->base = (arch_addr_t)(uintptr_t)lte.base_addr;
	lib->entry = entry;
	/* XXX The double cast should be removed when
	 * arch_addr_t becomes integral type.  */
	lib->dyn_addr = (arch_addr_t)(uintptr_t)lte.dyn_addr;

	/* There are several reasons that we need to inspect symbol tables or
	 * populate PLT entries. The user may have requested corresponding
	 * tracing features (respectively -x and -e), or they requested tracing
	 * exported symbols (-l). We also do this to resolve symbol aliases
	 *
	 * In the case of -l, we need to keep even those PLT slots that are not
	 * requested by -e (but we keep them latent). We also need to inspect
	 * .dynsym to find what exports this library provide, to turn on
	 * existing latent PLT entries. */

	int plts = filter_matches_library(options.plt_filter, lib);
	if ((plts || options.export_filter != NULL)
	    && populate_plt(proc, filename, &lte, lib) < 0)
		goto fail;

	int exports = filter_matches_library(options.export_filter, lib);
	int symtabs = filter_matches_library(options.static_filter, lib);
	if (populate_symtab(proc, filename, &lte, lib,
			    symtabs, exports) < 0)
		goto fail;

	arch_elf_destroy(&lte);
	ltelf_destroy(&lte);
	return 0;
}

int
ltelf_read_library(struct library *lib, struct process *proc,
		   const char *filename, GElf_Addr bias)
{
	return read_module(lib, proc, filename, bias, 0);
}


struct library *
ltelf_read_main_binary(struct process *proc, const char *path)
{
	struct library *lib = malloc(sizeof(*lib));
	if (lib == NULL || library_init(lib, LT_LIBTYPE_MAIN) < 0) {
		free(lib);
		return NULL;
	}
	library_set_pathname(lib, path, 0);

	/* There is a race between running the process and reading its
	 * binary for internal consumption.  So open the binary from
	 * the /proc filesystem.  XXX Note that there is similar race
	 * for libraries, but there we don't have a nice answer like
	 * that.  Presumably we could read the DSOs from the process
	 * memory image, but that's not currently done.  */
	char *fname = pid2name(proc->pid);
	if (fname == NULL
	    || read_module(lib, proc, fname, 0, 1) < 0) {
		library_destroy(lib);
		free(lib);
		lib = NULL;
	}

	free(fname);
	return lib;
}
