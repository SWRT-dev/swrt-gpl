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
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include "imagetool.h"
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

static int image_check_params(struct image_tool_params *params)
{
	return	((params->dflag && (params->fflag || params->lflag)) ||
		(params->fflag && (params->dflag || params->lflag)) ||
		(params->lflag && (params->dflag || params->fflag)));
}

static int image_verify_header(unsigned char *ptr, int image_size,
			struct image_tool_params *params)
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

TAIL tail_pre;
uint32_t rfs_offset;
int vargv;
static void image_set_header(char *ptr, struct stat *sbuf, int ifd,
				struct image_tool_params *params)
{
	uint32_t checksum;
	char *source_date_epoch;
	struct tm *time_universal;
	time_t time;

	// ASUS_EXT
#ifdef TRX_NEW
	//char *sn = NULL, *en = NULL;
	//char tmp[10];
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

	source_date_epoch = getenv("SOURCE_DATE_EPOCH");
	if (source_date_epoch != NULL) {
		time = (time_t) strtol(source_date_epoch, NULL, 10);

		time_universal = gmtime(&time);
		if (time_universal == NULL) {
			fprintf(stderr, "%s: SOURCE_DATE_EPOCH is not valid\n",
				__func__);
			time = 0;
		} else {
			time = mktime(time_universal);
		}
	} else {
		time = sbuf->st_mtime;
	}

	/* Build new header */
	image_set_magic(hdr, params->magic);
	image_set_time(hdr, time);
	image_set_size(hdr, sbuf->st_size - sizeof(image_header_t));
	image_set_load(hdr, params->addr);
	image_set_ep(hdr, params->ep);
	image_set_dcrc(hdr, checksum);
	image_set_os(hdr, params->os);
	image_set_arch(hdr, params->arch);
	image_set_type(hdr, params->type);
	image_set_comp(hdr, params->comp);
	//image_set_name(hdr, params->imagename);

	if(vargv==0)
	{
		image_set_name(hdr, params->imagename);
	}
	else
	{
		strncpy((char *)tail_pre.productid, params->imagename, MAX_STRING);

#ifdef TRX_NEW
		linux_offset = rfs_offset / 2 ;  
		lrand = *(ptr + linux_offset); //get kernel data
	//printf("rfs_offset = %02x  lrand = %02x linux_offset=%02x\n", rfs_offset, lrand, linux_offset);	
		rootfs_offset =  rfs_offset + ((sbuf->st_size  - rfs_offset) / 2 );
		rrand = *(ptr + rootfs_offset);	//get rootfs data
	//printf("rfs_offset = %02x  rrand = %02x  hdr->ih_size = %02x rootfs_offset=%02x\n", rfs_offset, rrand, sbuf->st_size - sizeof(image_header_t), rootfs_offset);		
	
	tail_pre.pkey = lrand; 

	if (rrand== 0x0)
		tail_pre.key = 0xfd + lrand % 3;
	else
		tail_pre.key = 0xff - rrand + lrand;

	//printf ("Pkey:         %02x\n", tail_pre.pkey);
	//printf ("Key:          %02x\n", tail_pre.key);     
#endif	
		tail_pre.hw[0].major = lrand + rrand;
		tail_pre.hw[0].minor = lrand + 0xa1;
		tail_pre.hw[1].major = 2 * lrand + rrand;
		tail_pre.hw[1].minor = lrand + 0xb2;
		tail_pre.hw[2].major = 3 * lrand + rrand;
		tail_pre.hw[2].minor = lrand + 0xc3;

		memcpy(&hdr->u.tail, &tail_pre, sizeof(TAIL));

		if (rfs_offset) {
#ifdef TRX_NEW	

			version_t *v1 = &hdr->u.tail.hw[MAX_VER - 2], *v2 = v1+1;
#else
			version_t *v1 = &hdr->u.tail.hw[MAX_VER*2 - 2], *v2 = v1+1;
#endif			
			union {
				uint32_t rfs_offset_net_endian;
				uint8_t p[4];
			} u;
			u.rfs_offset_net_endian = htonl(rfs_offset);

			if (v1->major || v1->minor || v2->major || v2->minor)
				printf("Hardware Compatible List: %d.%d and %d.%d are override by rootfilesystem offset.\n",
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

static int image_save_datafile(struct image_tool_params *params,
			       ulong file_data, ulong file_len)
{
	int dfd;
	const char *datafile = params->outfile;

	dfd = open(datafile, O_RDWR | O_CREAT | O_TRUNC | O_BINARY,
		   S_IRUSR | S_IWUSR);
	if (dfd < 0) {
		fprintf(stderr, "%s: Can't open \"%s\": %s\n",
			params->cmdname, datafile, strerror(errno));
		return -1;
	}

	if (write(dfd, (void *)file_data, file_len) != (ssize_t)file_len) {
		fprintf(stderr, "%s: Write error on \"%s\": %s\n",
			params->cmdname, datafile, strerror(errno));
		close(dfd);
		return -1;
	}

	close(dfd);

	return 0;
}

static int image_extract_datafile(void *ptr, struct image_tool_params *params)
{
	const image_header_t *hdr = (const image_header_t *)ptr;
	ulong file_data;
	ulong file_len;

	if (image_check_type(hdr, IH_TYPE_MULTI)) {
		ulong idx = params->pflag;
		ulong count;

		/* get the number of data files present in the image */
		count = image_multi_count(hdr);

		/* retrieve the "data file" at the idx position */
		image_multi_getimg(hdr, idx, &file_data, &file_len);

		if ((file_len == 0) || (idx >= count)) {
			fprintf(stderr, "%s: No such data file %ld in \"%s\"\n",
				params->cmdname, idx, params->imagefile);
			return -1;
		}
	} else {
		file_data = image_get_data(hdr);
		file_len = image_get_size(hdr);
	}

	/* save the "data file" into the file system */
	return image_save_datafile(params, file_data, file_len);
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
	.extract_datafile = image_extract_datafile,
	.check_params = image_check_params,
};

void init_default_image_type(void)
{
	register_image_type(&defimage_params);
}
