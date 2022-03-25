/*
 * GPL LICENSE SUMMARY
 *
 *  Copyright(c) 2017 Intel Corporation.
 */

#include <linux/printk.h>
#include <linux/string.h>
#include <linux/firmware.h>
#include <asm/byteorder.h>
#include "pp_qos_elf.h"
#include "elf.h"

#define ELF_FLD_RD(type, field, end, cls)				\
	({								\
		u64 ret = 0;						\
		int __size;						\
		if (cls == ELFCLASS32)					\
			__size = FIELD_SIZEOF(struct elf32_##type, field); \
		else							\
			__size = FIELD_SIZEOF(struct elf64_##type, field); \
		switch (__size) {					\
		case 2:							\
			ret = (end == ELFDATA2LSB) ?			\
			le16_to_cpu(*(u16 *)data) :			\
			be16_to_cpu(*(u16 *)data);			\
			break;						\
		case 4:							\
			ret = (end == ELFDATA2LSB) ?			\
			le32_to_cpu(*(u32 *)data) :			\
			be32_to_cpu(*(u32 *)data);			\
			break;						\
		case 8:							\
			ret = (end == ELFDATA2LSB) ?			\
			le64_to_cpu(*(u64 *)data) :			\
			be64_to_cpu(*(u64 *)data);			\
			break;						\
		default:						\
			pr_err("ELF Field read: Illegal size\n");	\
			break;						\
		}							\
		data += __size;						\
		ret;							\
	})

static void parse_elf_sec(const unsigned char *data, struct elf_sec *sh,
			  u8 endian, u8 cls)
{
	sh->name_idx = ELF_FLD_RD(shdr, sh_name, endian, cls);
	sh->type = ELF_FLD_RD(shdr, sh_type, endian, cls);
	sh->flags = ELF_FLD_RD(shdr, sh_flags, endian, cls);
	sh->addr = ELF_FLD_RD(shdr, sh_addr, endian, cls);
	sh->offset = ELF_FLD_RD(shdr, sh_offset, endian, cls);
	sh->size = ELF_FLD_RD(shdr, sh_size, endian, cls);
}

static s32 parse_elf_identification(const unsigned char *data,
				    u32 len, u8 *endian, u8 *cls)
{
	if (unlikely(len < 64)) {
		pr_err("ELF file too small %d\n", len);
		return -EIO;
	}

	if (unlikely(strncmp((const char *)data, ELFMAG, 4) != 0)) {
		pr_err("Could not find ELF magic\n");
		return -EIO;
	}

	*cls = data[EI_CLASS];
	pr_debug("CLASS --> %d\n", data[EI_CLASS]);
	if (unlikely(data[EI_CLASS] != ELFCLASS32 &&
		     data[EI_CLASS] != ELFCLASS64)) {
		pr_err("Unsupported ELF class %d\n", data[EI_CLASS]);
		return -EIO;
	}

	*endian = data[EI_DATA];
	pr_debug("ENDIANNESS --> %d\n", data[EI_DATA]);
	if (unlikely(data[EI_DATA] == ELFDATANONE)) {
		pr_err("Invalid endianness %d\n", data[EI_DATA]);
		return -EIO;
	}

	return 0;
}

s32 elf_parse(const unsigned char *elf, u32 len, struct elf_sec secs[],
	      u16 max_secs, u16 *num_secs)
{
	s32 ret;
	const unsigned char *data = elf;
	u32 file_offset;
	struct elf_sec strtab;
	unsigned long soff;
	const unsigned char *s;
	int i;
	u8 endian;
	u8 cls;
	struct elf64_hdr hdr; /* Using hdr 64b version. Suitable also for 32b */

	pr_debug("Start parsing elf size %d\n", len);

	if (!num_secs) {
		pr_err("num_secs is NULL\n");
		return -EIO;
	}

	*num_secs = 0;

	ret = parse_elf_identification(data, len, &endian, &cls);
	if (unlikely(ret))
		return ret;

	/* Skip e_ident*/
	data += EI_NIDENT;

	hdr.e_type = ELF_FLD_RD(hdr, e_type, endian, cls);
	hdr.e_machine = ELF_FLD_RD(hdr, e_machine, endian, cls);
	hdr.e_version = ELF_FLD_RD(hdr, e_version, endian, cls);
	hdr.e_entry = ELF_FLD_RD(hdr, e_entry, endian, cls);
	hdr.e_phoff = ELF_FLD_RD(hdr, e_phoff, endian, cls);
	hdr.e_shoff = ELF_FLD_RD(hdr, e_shoff, endian, cls);
	hdr.e_flags = ELF_FLD_RD(hdr, e_flags, endian, cls);
	hdr.e_ehsize = ELF_FLD_RD(hdr, e_ehsize, endian, cls);

	pr_debug("Sec header table file offset: %#llx bytes\n", hdr.e_shoff);
	pr_debug("ELF header size: %#x bytes\n", hdr.e_ehsize);

	if (unlikely(hdr.e_ehsize > len)) {
		pr_err("Section header size is bigger than ELF file size\n");
		return -EIO;
	}

	hdr.e_phentsize = ELF_FLD_RD(hdr, e_phentsize, endian, cls);
	hdr.e_phnum = ELF_FLD_RD(hdr, e_phnum, endian, cls);
	hdr.e_shentsize = ELF_FLD_RD(hdr, e_shentsize, endian, cls);

	if (unlikely(hdr.e_shentsize % 4)) {
		pr_err("ELF wrong sh entry size %d\n", hdr.e_shentsize);
		return -EIO;
	}

	hdr.e_shnum = ELF_FLD_RD(hdr, e_shnum, endian, cls);

	if (unlikely(hdr.e_shnum > max_secs)) {
		pr_err("ELF No room for %u sections\n", hdr.e_shnum);
		return -EIO;
	}

	hdr.e_shstrndx = ELF_FLD_RD(hdr, e_shstrndx, endian, cls);

	if ((hdr.e_shnum * hdr.e_shentsize) > len) {
		pr_err("ELF Exceed file size\n");
		return -EIO;
	}

	soff = hdr.e_shoff + (hdr.e_shentsize * hdr.e_shstrndx);

	if ((soff + hdr.e_shentsize) > len || soff % 4 || soff == 0) {
		pr_err("Wrong ELF format\n");
		return -EIO;
	}

	parse_elf_sec(elf + soff, &strtab, endian, cls);
	file_offset = strtab.offset;

	if ((u32)(file_offset + hdr.e_shentsize) >= len || file_offset == 0) {
		pr_err("Wrong ELF offset to name\n");
		return -EIO;
	}

	s = (const unsigned char *)(elf + hdr.e_shoff);

	for (i = 0; i < hdr.e_shnum; ++i) {
		parse_elf_sec(s, &secs[*num_secs], endian, cls);
		s += hdr.e_shentsize;

		if (secs[*num_secs].name_idx == 0)
			continue;

		secs[*num_secs].name = (char *)(elf + file_offset +
				secs[*num_secs].name_idx);

		if (file_offset + secs[*num_secs].name_idx > len) {
			pr_err("Name offset exceeds file size\n");
			return 1;
		}

		secs[*num_secs].need_copy =
				((secs[*num_secs].flags & SHF_ALLOC) &&
				(secs[*num_secs].type & SHT_PROGBITS))
				? 1 : 0;

		secs[*num_secs].data = (void *)(elf + secs[*num_secs].offset);

		(*num_secs)++;
	}

	return 0;
}

s32 elf_request_fw_and_parse(struct device *dev, const unsigned char *name,
			     const struct firmware **fw, struct elf_sec secs[],
			     u16 max_secs, u16 *num_secs)
{
	s32 ret;

	ret = request_firmware(fw, name, dev);
	if (ret < 0) {
		pr_err("Failed loading firmware ret is %d\n", ret);
		return ret;
	}

	return elf_parse((*fw)->data, (*fw)->size, secs, max_secs, num_secs);
}

void elf_release_fw(const struct firmware *fw)
{
	release_firmware(fw);
}
