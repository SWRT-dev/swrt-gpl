/*
 * (C) Copyright 2008 Semihalf
 *
 * (C) Copyright 2000-2004
 * DENX Software Engineering
 * Wolfgang Denk, wd@denx.de
 *
 * Updated-by: Prafulla Wadaskar <prafulla@marvell.com>
 *		default_image specific code abstracted from mkimage.c
 *		some functions added to address abstraction
 *
 * All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include "mkimage.h"
#include <image.h>
#include <u-boot/crc.h>

static image_header_t header;

static int image_check_image_types(uint8_t type)
{
	if (((type > IH_TYPE_INVALID) && (type < IH_TYPE_FLATDT)) ||
	    (type == IH_TYPE_KERNEL_NOLOAD))
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

static int image_check_params(struct mkimage_params *params)
{
	return	((params->dflag && (params->fflag || params->lflag)) ||
		(params->fflag && (params->dflag || params->lflag)) ||
		(params->lflag && (params->dflag || params->fflag)));
}

static int image_verify_header(unsigned char *ptr, int image_size,
			struct mkimage_params *params)
{
	uint32_t len;
	const unsigned char *data;
	uint32_t checksum;
	image_header_t header;
	image_header_t *hdr = &header;

	/*
	 * create copy of header so that we can blank out the
	 * checksum field for checking - this can't be done
	 * on the PROT_READ mapped data.
	 */
	memcpy(hdr, ptr, sizeof(image_header_t));

	if (be32_to_cpu(hdr->ih_magic) != IH_MAGIC) {
		fprintf(stderr,
			"%s: Bad Magic Number: \"%s\" is no valid image\n",
			params->cmdname, params->imagefile);
		return -FDT_ERR_BADMAGIC;
	}

	data = (const unsigned char *)hdr;
	len  = sizeof(image_header_t);

	checksum = be32_to_cpu(hdr->ih_hcrc);
	hdr->ih_hcrc = cpu_to_be32(0);	/* clear for re-calculation */

	if (crc32(0, data, len) != checksum) {
		fprintf(stderr,
			"%s: ERROR: \"%s\" has bad header checksum!\n",
			params->cmdname, params->imagefile);
		return -FDT_ERR_BADSTATE;
	}

	data = (const unsigned char *)ptr + sizeof(image_header_t);
	len  = image_size - sizeof(image_header_t) ;

	checksum = be32_to_cpu(hdr->ih_dcrc);
	if (crc32(0, data, len) != checksum) {
		fprintf(stderr,
			"%s: ERROR: \"%s\" has corrupted data!\n",
			params->cmdname, params->imagefile);
		return -FDT_ERR_BADSTRUCTURE;
	}
	return 0;
}

static void image_set_header(void *ptr, struct stat *sbuf, int ifd,
				struct mkimage_params *params)
{
	uint32_t checksum;
#ifdef TRX_NEW
	uint8_t *l1,*r1;
        uint8_t lrand = 0;
        uint8_t rrand = 0;
        uint32_t linux_offset = 0;
        uint32_t rootfs_offset = 0;
#endif
	image_header_t * hdr = (image_header_t *)ptr;

	checksum = crc32(0,
			(const unsigned char *)(ptr +
				sizeof(image_header_t)),
			sbuf->st_size - sizeof(image_header_t));

	/* Build new header */
	image_set_magic(hdr, params->magic);
	image_set_time(hdr, sbuf->st_mtime);
	image_set_size(hdr, sbuf->st_size - sizeof(image_header_t));
	image_set_load(hdr, params->addr);
	image_set_ep(hdr, params->ep);
	image_set_dcrc(hdr, checksum);
	image_set_os(hdr, params->os);
	image_set_arch(hdr, params->arch);
	image_set_type(hdr, params->type);
	image_set_comp(hdr, params->comp);

	if(params->vargv==0)
	{
		image_set_name(hdr, params->imagename);
	}
	else
	{
		strncpy((char *)params->tail_pre.productid, params->imagename, MAX_STRING);
#ifdef TRX_NEW
                linux_offset = (params->rfs_offset) / 2 ;
                l1=(ptr + linux_offset); //get kernel data
		lrand=*l1;
        //printf("rfs_offset = %02x  lrand = %02x linux_offset=%02x\n", params->rfs_offset, lrand, linux_offset);
                rootfs_offset =  (params->rfs_offset) + ((sbuf->st_size  - (params->rfs_offset)) / 2 );
		r1= (ptr + rootfs_offset); //get rootfs data
		rrand = *r1;
        //printf("rfs_offset = %02x  rrand = %02x  hdr->ih_size = %02x rootfs_offset=%02x\n", params->rfs_offset, rrand, sbuf->st_size - sizeof(image_header_t), rootfs_offset);
 
        params->tail_pre.pkey = lrand;
 
        if (rrand== 0x0)
                params->tail_pre.key = 0xfd + lrand % 3;
        else
                params->tail_pre.key = 0xff - rrand + lrand;
 
        printf ("Pkey:         %02x\n", params->tail_pre.pkey);
        printf ("Key:          %02x\n", params->tail_pre.key);
#endif
		memcpy(&hdr->u.tail, &params->tail_pre, sizeof(TAIL));

		if (params->rfs_offset) {
#ifdef TRX_NEW
                        version_t *v1 = &hdr->u.tail.hw[MAX_VER - 2], *v2 = v1+1;
#else
                 	version_t *v1 = &hdr->u.tail.hw[MAX_VER*2 - 2], *v2 = v1+1;
#endif
			union {
				uint32_t rfs_offset_net_endian;
				uint8_t p[4];
			} u;
			u.rfs_offset_net_endian = htonl(params->rfs_offset);

			if (v1->major || v1->minor || v2->major || v2->minor)
				printf("Hardware Compatible List: %d.%d and %d.%d "
					"are override by rootfilesystem offset.\n",
					v1->major, v1->minor, v2->major, v2->minor);
			v1->major = ROOTFS_OFFSET_MAGIC;
			v1->minor = u.p[1];
			v2->major = u.p[2];
			v2->minor = u.p[3];
		}
	}

	checksum = crc32(0, (const unsigned char *)hdr,
				sizeof(image_header_t));

	image_set_hcrc(hdr, checksum);
}

/*
 * Default image type parameters definition
 */
static struct image_type_params defimage_params = {
	.name = "Default Image support",
	.header_size = sizeof(image_header_t),
	.hdr = (void*)&header,
	.check_image_type = image_check_image_types,
	.verify_header = image_verify_header,
	.print_header = image_print_contents,
	.set_header = image_set_header,
	.check_params = image_check_params,
};

void init_default_image_type(void)
{
	mkimage_register(&defimage_params);
}
