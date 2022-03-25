#include <common.h>
#include <config.h>
#include <command.h>
#include <image.h>
#include <aes.h>
#include <asm/io.h>
#if (defined (LEOPARD_ASIC_BOARD)) || (defined (LEOPARD_FPGA_BOARD))
#include <asm/arch/gpio.h>
#endif
#if defined(ON_BOARD_NAND_FLASH_COMPONENT)
#include <asm/arch/nand/partition_define.h>
#endif

#ifndef CONFIG_SPL_BUILD

#define PART_MAGIC          0x58881688
#define PART_FDT_MAGIC      0xD00DFEED

#define ENCRPTED_IMG_TAG  "encrpted_img"
#define AES_KEY_256  "he9-4+M!)d6=m~we1,q2a3d1n&2*Z^%8$"
#define AES_NOR_IV  "J%1iQl8$=lm-;8AE@"

ulong ctp_load_addr = 0x40004400;


typedef union
{
	struct
	{
		unsigned int magic; 	/* partition magic */
		unsigned int dsize; 	/* partition data size */
		char name[32];			/* partition name */
		unsigned int maddr; 	/* partition memory address */
		unsigned int mode;		/* memory addressing mode */
	} info;
	unsigned char data[512];
} part_hdr_t;

typedef  struct
{
	char model[32]; 	   /*model name*/
	char region[32];	   /*region*/
	char version[64];	   /*version*/
	char dateTime[64];	   /*date*/
	char reserved[320];    /*reserved space, if add struct member, please adjust this reserved size to keep the head total size is 512 bytes*/
} __attribute__((__packed__)) image_head_t;

typedef  struct
{
	char checkSum[4];		/*checkSum*/
} __attribute__((__packed__)) image_tail_t;

typedef struct
{
    char name[32];
    char version[32];
    u32 size;
    u32 crc;
    char padding[56];
    char data[0];
} __attribute__((__packed__)) DAI_LANGUAGE_PARTITION_T;

typedef struct
{
    char checkSum[4];
    char tag[32];
    int partSize;
    char reserve[24];
}__attribute__((__packed__)) PARTITION_HEADER_T;

#define MTD_BOOT_TAG               "MTD_BOOT_TAG"
#define MTD_IMAGEA_TAG              "MTD_IMAGEA_TAG"
#define MTD_IMAGEB_TAG              "MTD_IMAGEB_TAG"

#if defined (CONFIG_CMD_NMRP)
int do_read_str_blks (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    DAI_LANGUAGE_PARTITION_T *str_hdr = (DAI_LANGUAGE_PARTITION_T *)load_addr;
    const unsigned char *str_addr = (const unsigned char *)load_addr;
    u32 str_size = CONFIG_STR_BLK_SIZE;
    u32 crc = str_hdr->crc;

    str_hdr->crc = 0;
    if (crc32(0,str_addr,str_size) != uimage_to_cpu(crc))
        setenv("str_blk_crc","error");
    else
        setenv("str_blk_crc","ok");
    str_hdr->crc = crc;

    return 0;
}
#endif

int do_read_image_blks (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned long blk_size;
    unsigned long img_blks;
	char *endp;
    image_header_t *img_hdr = (image_header_t *)load_addr;
	unsigned long img_addr = (unsigned long)load_addr;
    unsigned long img_size = 0;
    char s[32];

    if ((argc < 2) || (argc > 3))
    goto usage;

    blk_size = simple_strtoul(argv[1], &endp, 10);
    if (blk_size < 1)
    {
        printf("Block Size should be positive numble.\n");
        return 0;
    }
    if (argc == 3)
    {
        img_size = simple_strtoul(argv[2], &endp, 16);

        if (img_size < 1)
        {
            printf("Image Size should be positive numble.\n");
            return 0;
        }
    }

    if(PART_FDT_MAGIC == (unsigned long) fdt_magic(img_addr)) //This is fit image
	{
		/*
	     * If img_size is large than 0, it specifies that user assigns a image size.
	     * Or we read image size from image header.
	     */
	    if (img_size == 0)
	    {
		    img_size = (unsigned long) fdt_totalsize(img_addr);
	    }
		printf("[%s] This is a FIT image,img_size = 0x%lx\n", __func__, img_size);
   }
   else if(IH_MAGIC != uimage_to_cpu(img_hdr->ih_magic))
	{
#if defined (CONFIG_MTK_MTD_NAND)
	 	if (*((unsigned long*)load_addr) != PART_MAGIC)
#endif
		{
            printf("[%s] Image format error,neither FIT image nor old image.\n",__func__);
			printf("Bad Magic Number.\n");
			sprintf((char*) s,"%lX", (unsigned long)0);
			setenv("img_blks", (const char *)s);
#if defined (CONFIG_MTK_MTD_NAND)
			sprintf((char*) s,"%lX", (unsigned long)0);
			setenv("img_align_size", (const char *)s);
#endif

			return 0;
		}
#if defined (CONFIG_MTK_MTD_NAND)
		else
        {
			img_size = getenv_ulong("filesize", 16, 0);
        }
#endif
    }
	else
    {
        /*
         * If img_size is large than 0, it specifies that user assigns a image size.
         * Or we read image size from image header.
         */
        if (img_size == 0)
        {
            img_size = uimage_to_cpu(img_hdr->ih_size)+sizeof(image_header_t);
        }

    }


	img_blks = (img_size + (blk_size - 1))/blk_size;

    sprintf((char*) s,"%lX", img_blks);
    setenv("img_blks", (const char *)s);
#if defined (CONFIG_MTK_MTD_NAND) || defined(ON_BOARD_SPI_NOR_FLASH_COMPONENT)
	sprintf((char*) s,"%lX", img_blks*blk_size);
	setenv("img_align_size", (const char *)s);
	printf("[%s] img_blks = 0x%lx\n", __func__, img_blks);
	printf("[%s] img_align_size = 0x%lx\n", __func__, img_blks*blk_size);
#endif

    return 0;

usage:
	return CMD_RET_USAGE;
}

int do_read_mtk_image_blks (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned long blk_size;
    unsigned long img_blks;
    char *endp;

    unsigned long img_size;
    char s[32];
    part_hdr_t* img_addr = (part_hdr_t*)load_addr;

    int i = 0;

    if (argc != 2)
    	goto usage;
    
    blk_size = simple_strtoul(argv[1], &endp, 10);
    if (*argv[1] == 0 || *endp != 0)
    	goto usage;

    if(blk_size < 1)
    {
        printf("Block Size should be positive numble.\n");
        return 0;
    }

    setenv("check_boot_error", "1");
    if (PART_MAGIC != (unsigned long) img_addr->info.magic)
    {
    	printf("[%s]Bad Magic Number = 0x%lx\n", 
    		__func__, (unsigned long) img_addr->info.magic);
    	
    	return 0;
    }
    setenv("check_boot_error", "0");
    printf("[%s]Good data[] = ", __func__);
    for(i=0; i<64; i++)
    	printf("0x%x ", (unsigned char) img_addr->data[i]);
    printf("\n");
    
    printf("[%s]Good Magic Number = 0x%lx\n", 
    		__func__, (unsigned long) img_addr->info.magic);
    printf("[%s]Good Data size = 0x%lx\n", 
    		__func__, (unsigned long) img_addr->info.dsize);
    printf("[%s]Good name = %s\n", 
    		__func__, img_addr->info.name);
    img_size = (unsigned long) img_addr->info.dsize;
    img_size += 0x200;	/* NOTE(Nelson): Add FIT header size */
    printf("[%s]img_size = 0x%lx\n", __func__, img_size);
    
	img_blks = (img_size + (blk_size - 1))/blk_size;

    sprintf((char*) s,"%lX", img_blks);
    setenv("img_blks", (const char *)s);
#if defined (CONFIG_MTK_MTD_NAND) || defined(ON_BOARD_SPI_NOR_FLASH_COMPONENT)
	sprintf((char*) s,"%lX", img_blks*blk_size);
	setenv("img_align_size", (const char *)s);
	printf("[%s] img_align_size = %lX\n", __func__, img_blks*blk_size);
    img_size = (unsigned long) img_addr->info.dsize;
#endif

    return 0;

usage:
	return CMD_RET_USAGE;    
}

int do_image_check (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    image_header_t *img_hdr = (image_header_t *)load_addr;
    char s[32];
    
    if ( uimage_to_cpu(img_hdr->ih_magic) != IH_MAGIC )
    {
        printf("Image_Check Bad Magic Number.\n");        
        sprintf((char*) s, "%s", "bad");        
        setenv("img_result", (const char *)s);

        return 0;
    }

    sprintf((char*) s, "%s", "good");        
    setenv("img_result", (const char *)s);    

    return 0;
}

int do_serious_image_check (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	image_header_t *img_hdr = (image_header_t *)load_addr;
	char s[32];

	printf("serious check\n"); 
	if ( uimage_to_cpu(img_hdr->ih_magic) != IH_MAGIC )
	{
		printf("Bad Magic Number.\n");        
		goto error; 

	} 
	/////////////////////////
	if (!image_check_hcrc(img_hdr)) {
		printf("Bad Header Checksum\n");
		goto error; 
	}
	//////////////////
	if (!image_check_dcrc(img_hdr)) {
		printf("Bad Data CRC\n");
		goto error; 
	}	


	sprintf((char*) s, "%s", "good");        
	setenv("img_result", (const char *)s);    

	return 0;

error:
	
	sprintf((char*) s, "%s", "bad");       
	setenv("img_result", (const char *)s);
	return 1;
}

int do_backup_message (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	printf("Recovery partition is empty. Copy kernel image to it!!!\n");
	return 0;
}

int do_reco_message (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{

	printf("Main kernel image is damaged. Recover kernel image!!!\n");
	return 0;
}

int do_invaild_env (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	setenv("invaild_env","yes");
	saveenv();
	printf("invaild env!!!\n");
	return 0;
}


int do_uboot_check (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned long magic_no;    
    char s[32];
    
    magic_no = *(unsigned long*)load_addr;
    if ( magic_no != PART_MAGIC )
    {
        printf("Bad Magic Number.\n");        
        sprintf((char*) s, "%s", "bad");        
        setenv("uboot_result", (const char *)s);

        return 0;
    }

    sprintf((char*) s, "%s", "good");        
    setenv("uboot_result", (const char *)s);

    return 0;
}


int do_filesize_check (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    unsigned long partition_size, filesize;
    unsigned long file_align_size;
    char s[32];
	char *endp;
    
	if (argc != 2)
    	goto usage;
    
	partition_size = simple_strtoul(argv[1], &endp, 16);
	if (*argv[1] == 0 || *endp != 0)
			goto usage;

	
	filesize = getenv_hex("filesize",0);
	printf("[%s] partition_size: 0x%lx , filesize: 0x%lx\n", __func__, partition_size, filesize);
	if (filesize == 0)
	{
		printf("filesize == 0 ! please check the image is ok or not !!!.\n"); 
		sprintf((char*) s, "%s", "bad");        
        setenv("filesize_result", (const char *)s);
		return 0;
		
	}
    if ( partition_size < filesize )
    {
    	printf("Bad file size ! please check the image is ok or not !!! max_partition_size:0x%lx .\n",partition_size);
        sprintf((char*) s, "%s", "bad");        
        setenv("filesize_result", (const char *)s);

        return 0;
    }

    sprintf((char*) s, "%s", "good");        
    setenv("filesize_result", (const char *)s);
    uninitialized_var(file_align_size);
#if defined(ON_BOARD_SPI_NOR_FLASH_COMPONENT)
    file_align_size = ROUND(filesize, 0x1000); /* align 4K size */
    sprintf((char*) s,"%lx", file_align_size);
    setenv("file_align_size", (const char *)s);
    printf("[%s] file_align_size = 0x%lx\n", __func__, file_align_size);
#endif
    return 0;
	
usage:
	return CMD_RET_USAGE;	
}


void input_value(char *str)
{
    if (str)
        strcpy(console_buffer, str);
    else
        console_buffer[0] = '\0';
	while(1)
    {
        if (readline ("==:") > 0)
        {
            strcpy (str, console_buffer);
            break;
        }
        else
            break;
    }
}

void get_filetype(char *dst, const char *src, int size)
{
	if (*src && (*src == '"')) {
		++src;
		--size;
	}

	while ((--size > 0) && *src && (*src != '"'))
		*dst++ = *src++;
	*dst = '\0';
}


int do_download_setting (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int modifies = 0;
	char *s;
	char filetype[16],filename[128],defaultfilename[128],srvip[32],defaultsrvip[32],devip[32],defaultdevip[32];


	if (argc != 2)
		goto usage;

	printf(" Please Input new setting /or enter to choose the default setting\n");

	memset(filetype, 0, 16);
	memset(filename, 0, 128);
	memset(defaultfilename, 0, 128);
	memset(srvip, 0, 32);
	memset(defaultsrvip, 0, 32);
	memset(devip, 0, 32);
	memset(defaultdevip, 0, 32);

	get_filetype(filetype, argv[1], sizeof(filetype));
	if (strcmp(filetype, "kernel") == 0)
	{
        printf("\tInput kernel file name ");
        s = getenv("kernel_filename");
		memcpy(filename, s, strlen(s));
		memcpy(defaultfilename, s, strlen(s));
        printf("(%s) ", filename);
        input_value(filename);
        setenv("kernel_filename", filename);
		if (strcmp(defaultfilename, filename) != 0)
			modifies++;
	}
	else if (strcmp(filetype, "uboot") == 0)
	{
		printf("\tInput uboot file name ");
        s = getenv("uboot_filename");
		memcpy(filename, s, strlen(s));
		memcpy(defaultfilename, s, strlen(s));
        printf("(%s) ", filename);
        input_value(filename);
        setenv("uboot_filename", filename);
		if (strcmp(defaultfilename, filename) != 0)
			modifies++;
	}
	else if (strcmp(filetype, "atf") == 0)
	{
		printf("\tInput atf file name ");
        s = getenv("atf_filename");
		memcpy(filename, s, strlen(s));
		memcpy(defaultfilename, s, strlen(s));
        printf("(%s) ", filename);
        input_value(filename);
        setenv("atf_filename", filename);
		if (strcmp(defaultfilename, filename) != 0)
			modifies++;
	}
	else if (strcmp(filetype, "preloader") == 0)
	{
		printf("\tInput preloader file name ");
        s = getenv("preloader_filename");
		memcpy(filename, s, strlen(s));
		memcpy(defaultfilename, s, strlen(s));
        printf("(%s) ", filename);
        input_value(filename);
        setenv("preloader_filename", filename);
		if (strcmp(defaultfilename, filename) != 0)
			modifies++;
	}
	else if (strcmp(filetype, "hdr") == 0)
	{
		printf("\tInput hdr file name ");
        s = getenv("hdr_filename");
		memcpy(filename, s, strlen(s));
		memcpy(defaultfilename, s, strlen(s));
        printf("(%s) ", filename);
        input_value(filename);
        setenv("hdr_filename", filename);
		if (strcmp(defaultfilename, filename) != 0)
			modifies++;
	}
	else if (strcmp(filetype, "ctp") == 0)
	{
		printf("\tInput ctp file name ");
        s = getenv("ctp_filename");
		memcpy(filename, s, strlen(s));
		memcpy(defaultfilename, s, strlen(s));
        printf("(%s) ", filename);
        input_value(filename);
        setenv("ctp_filename", filename);
		if (strcmp(defaultfilename, filename) != 0)
			modifies++;
	}
	else if (strcmp(filetype, "flashimage") == 0)
	{
		printf("\tInput flashimage file name ");
        s = getenv("flashimage_filename");
		memcpy(filename, s, strlen(s));
		memcpy(defaultfilename, s, strlen(s));
        printf("(%s) ", filename);
        input_value(filename);
        setenv("flashimage_filename", filename);
		if (strcmp(defaultfilename, filename) != 0)
			modifies++;
	}
	else if (strcmp(filetype, "gpt") == 0)
	{
		printf("\tInput partition table file name ");
		s = getenv("gpt_filename");
		memcpy(filename, s, strlen(s));
		memcpy(defaultfilename, s, strlen(s));
		printf("(%s) ", filename);
		input_value(filename);
		setenv("gpt_filename", filename);
		if (strcmp(defaultfilename, filename) != 0)
			modifies++;
	}
	else
	{
		printf("\tbad filetype ! please check\n ");
		goto usage;
	}


	printf("\tInput server IP ");
	s = getenv("serverip");
	memcpy(srvip, s, strlen(s));
	memcpy(defaultsrvip, s, strlen(s));
	printf("(%s) ", srvip);
	input_value(srvip);
	setenv("serverip", srvip);
	if (strcmp(defaultsrvip, srvip) != 0)
		modifies++;

	printf("\tInput device IP ");
	s = getenv("ipaddr");
	memcpy(devip, s, strlen(s));
	memcpy(defaultdevip, s, strlen(s));
	printf("(%s) ", devip);
	input_value(devip);
	setenv("ipaddr", devip);
	if (strcmp(defaultdevip, devip) != 0)
		modifies++;

	if(modifies)
	{
		printf("setting changed , saveenv\n");
		saveenv();
	}
	return 0;

usage:
	return CMD_RET_USAGE;
}

#if (defined (LEOPARD_ASIC_BOARD)) || (defined (LEOPARD_FPGA_BOARD))
int do_custom_image_check(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    u32 *img_hdr = (u32 *)CONFIG_SYS_SDRAM_BASE;
    u32 first_val, img_hdr_magic;
    char s[32];

#if defined (CONFIG_CMD_NAND)
    img_hdr_magic = 0x424F4F54;
#elif defined (CONFIG_CMD_NOR)
    img_hdr_magic = 0x53465F42;
#else
    img_hdr_magic = 0x12345678;
#endif
    first_val = uimage_to_cpu((u32)(*img_hdr));
    if (first_val != img_hdr_magic) {
        printf("Image_Check Bad Magic Number - 0x%x\n", first_val);
        sprintf((char*) s, "%s", "bad");
        setenv("img_result", (const char *)s);
        return 0;
    }

    sprintf((char*) s, "%s", "good");
    setenv("img_result", (const char *)s);
    printf("[%s] start to upgrade with ICE load...\n", __func__);
    return 0;
}

int do_utif_setting(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
#if 1  /* This settings must be checked by DE */
	mt_set_gpio_mode(GPIO11, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO18, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO21, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO22, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO23, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO24, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO33, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO34, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO35, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO36, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO37, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO38, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO39, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO40, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO41, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO42, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO43, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO44, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO45, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO46, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO47, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO48, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO49, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO50, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO53, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO54, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO55, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO56, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO57, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO58, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO59, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO60, GPIO_MODE_07);
	mt_set_gpio_mode(GPIO61, GPIO_MODE_07);
#endif
	printf("Set UTIF_debug mode successfully!\n");
	return 0;
}

int do_switch_for_N9_jtag(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int port;
	char *endp;

	if(argc > 2) {
		printf("Invalid input parameter!\n");
		return 1;
	}

	port = simple_strtoul(argv[1], &endp, 10);
	if(port > 2) {
		printf("Invalid Error Port %d!\n", port);
		return 1;
	}
	if(port == 2) {
		mt_set_gpio_mode(GPIO53, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO54, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO55, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO56, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO57, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO58, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO59, GPIO_MODE_06);
		printf("Switch GPIO53~GPIO59 to AUX6 for N9 JTAG successfully!\n");
	} else {
		mt_set_gpio_mode(GPIO11, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO12, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO13, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO14, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO15, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO16, GPIO_MODE_06);
		mt_set_gpio_mode(GPIO17, GPIO_MODE_06);
		printf("Switch GPIO11~GPIO17 to AUX6 for CA7/N9 JTAG successfully!\n");
	}

	return 0;
}

int do_switch_for_N9_uart(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	mt_set_gpio_mode(GPIO60, GPIO_MODE_06);
	mt_set_gpio_mode(GPIO61, GPIO_MODE_06);

	printf("Switch GPIO60~GPIO61 to AUX6 for N9 UART successfully!\n");
	return 0;
}

int do_gpio_dump(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	mt_get_gpio_status_dump();
	return 0;
}
#endif

#if (defined (MT7622_FPGA_BOARD)) || (defined (MT7622_ASIC_BOARD))
int do_ctp_check (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    char *s;
    int i = 0;
	s = (char*)ctp_load_addr;
	for(i=0; i<=31; i++)
    {
    	printf("%2x\t",s[i]);
		if((i+1)%8 == 0)
		{
			printf("\n");
		}
    }
    setenv("ctp_result", "good");

    return 0;
}

int do_boot_to_ctp (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    void (*func)(void);
	int i;
	char *t;
	t = (char*)ctp_load_addr;
    for(i=0; i<=31; i++)
    {
    	printf("%2x\t",t[i]);
		if((i+1)%8 == 0)
		{
			printf("\n");
		}
    }
	func = (void*)(ctp_load_addr);
	func();
    return 0;
}
#endif

//extern void led_time_tick(ulong times);

int do_decrypt_image (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	u32 size = 0;
	u32 file_size = 0;
	unsigned char *src_addr = NULL;
	unsigned char *dst_addr = NULL;
	image_head_t *image_head = NULL;
	char *image_tag = NULL;
	//char *fenv_model = NULL;
	//char *fenv_region = NULL;
	u32 image_size = 0;
	u32 encrypted_size = 0;
	u32 block_size = 0;
	u32 checksum = 0;
	u32 curr_checksum = 0;
	unsigned char key_exp[AES256_EXPAND_KEY_LENGTH] = {0};
	unsigned int aes_blocks = 0;

	setenv("decrypt_result", "bad");

	size = sizeof(image_head_t) + strlen(ENCRPTED_IMG_TAG) + 4 * 2;

	file_size = getenv_hex("filesize", 0);
	if (file_size < size)
	{
		printf("Image head not found!\n");
		goto good;
	}

	src_addr = (unsigned char *)load_addr;
	dst_addr = (unsigned char *)load_addr;

	image_head = (image_head_t *)src_addr;
	src_addr += sizeof(image_head_t);

	image_tag = (char *)src_addr;
	if (strncmp(image_tag, ENCRPTED_IMG_TAG, strlen(ENCRPTED_IMG_TAG)))
	{
		printf("Encrpted tag not found!\n");
		goto good;
	}
	src_addr += strlen(ENCRPTED_IMG_TAG);

	printf("Image is encrypted\n");
	printf("model: %s\n", image_head->model);
	printf("region: %s\n", image_head->region);
	printf("version: %s\n", image_head->version);
	printf("dateTime: %s\n", image_head->dateTime);

#if 0
	fenv_model = getenv("fenv_model");
	if (!fenv_model || strcmp(fenv_model, image_head->model))
	{
		printf("Image model not match!\n");
		return 1;
	}

	fenv_region = getenv("fenv_region");
	if (!fenv_region || strcmp(fenv_region, image_head->region))
	{
		printf("Image region not match!\n");
		return 1;
	}
#endif

	image_size = ntohl(*(u32 *)src_addr);
	src_addr += sizeof(u32);
	printf("size: 0x%lx\n", image_size);

	encrypted_size = DIV_ROUND_UP(image_size, AES_BLOCK_LENGTH) * AES_BLOCK_LENGTH;

	block_size = ntohl(*(u32 *)src_addr);
	src_addr += sizeof(u32);
	printf("block size: 0x%lx\n", block_size);
	if (block_size % AES_BLOCK_LENGTH)
	{
		printf("Image block size not times of AES_BLOCK_LENGTH!\n");
		return 1;
	}

	if (file_size < (size + encrypted_size + sizeof(image_tail_t)))
	{
		printf("Image incomplete!\n");
		return 1;
	}

	checksum = ntohl(*(u32 *)(src_addr + encrypted_size));
	printf("checksum: 0x%lx\n", checksum);

	curr_checksum = crc32_no_comp(0, (uchar *)load_addr, size + encrypted_size);
	if (curr_checksum != checksum)
	{
		printf("Image checksum error!\n");
		return 1;
	}

	printf("Decrypt image...\n");

	aes_expand_key((u8 *)AES_KEY_256, AES256_KEY_LENGTH, key_exp);
	for (size = 0; size < encrypted_size; size += block_size)
	{
		if (size + block_size > encrypted_size)
		{
			block_size = encrypted_size - size;
		}

		aes_blocks = DIV_ROUND_UP(block_size, AES_BLOCK_LENGTH);
		aes_cbc_decrypt_blocks(AES256_KEY_LENGTH, key_exp, (u8 *)AES_NOR_IV, (u8 *)src_addr, (u8 *)dst_addr, aes_blocks);

		src_addr += block_size;
		dst_addr += block_size;
//		led_time_tick(get_timer(0)); 
	}
	printf("Decrypt finish\n");

	setenv_hex("filesize", image_size);

good:
	setenv("decrypt_result", "good");
	return 0;
}

int do_check_image (cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
    PARTITION_HEADER_T *image_hdr = (PARTITION_HEADER_T *)load_addr;
    const unsigned char *image_addr = (const unsigned char *)(load_addr + CONFIG_MAX_NAND_BLOCK_SIZE);
    u32 image_size = image_hdr->partSize;
    u32 image_crc = ntohl(*(u32 *)image_hdr->checkSum);
    char image_tag[32] = "";
    u32 crc = 0;
    u32 len = sizeof(image_tag)-1;

    strncpy(image_tag,image_hdr->tag,len);
    printf("image tag:    %s\n",image_tag);
    setenv("checkimg_result", "bad");
    setenv("have_tag", "yes");
    if (strncmp(image_tag, MTD_IMAGEA_TAG, strlen(MTD_IMAGEA_TAG)) && strncmp(image_tag, MTD_IMAGEB_TAG, strlen(MTD_IMAGEB_TAG)))
    {
        printf("tag error!try no tag mode...\n");
        setenv("have_tag", "no");
        return 1;
    }

    crc = crc32_no_comp(0,image_addr,image_size);
    printf("image size:   0x%lx\n",image_size);
    printf("checksum:     0x%lx\n",crc);
    printf("image crc:    0x%lx\n",image_crc);
    if (crc != image_crc)
    {
        printf("error checkSum\n");
        return 1;
    }
    setenv("checkimg_result","ok");

    return 0;
}

#if defined (CONFIG_CMD_NMRP)
U_BOOT_CMD(
	str_blks, 1,	1, do_read_str_blks,
	"check str table blk from load_addr",
	""
);
#endif

U_BOOT_CMD(
	image_blks, 3,	1, do_read_image_blks,
	"read image size from img_size or image header if no specifying img_size, and divided by blk_size and save image blocks in image_blks variable.",
	"[blk_size]\n"
	"   - specify block size in decimal byte.\n"
	"[img_size]\n"
	"   - specify image size in hex byte."
);

U_BOOT_CMD(
	mtk_image_blks, 2, 1, do_read_mtk_image_blks,
	"read image size from image header (MTK format) located at load_addr, divided by blk_size and save image blocks in image_blks variable.",
	"[blk_size]\n"
	"   - specify block size in byte."
);

U_BOOT_CMD(
	image_check, 2,	1, do_image_check,
	"check if image in load_addr is normal.",
	""
);
U_BOOT_CMD(
	serious_image_check, 2,	1, do_serious_image_check,
	"seriously check if image in load_addr is normal.",
	""
);

U_BOOT_CMD(
	backup_message, 2,	1, do_backup_message,
	"print backup message.",
	""
);

U_BOOT_CMD(
	reco_message, 2,	1, do_reco_message,
	"print recovery message.",
	""
);

U_BOOT_CMD(
	uboot_check, 2,	1, do_uboot_check,
	"check if uboot in load_addr is normal.",
	""
);

U_BOOT_CMD(
	filesize_check, 2,	1, do_filesize_check,
	"check if filesize of the image that you want to upgrade is normal.",
	""
);

U_BOOT_CMD(
	download_setting, 2,	1, do_download_setting,
	"set download image file name , and device IP , server IP before upgrade",
	""
);

U_BOOT_CMD(
	invaild_env, 2,	1, do_invaild_env,
	"need to invaild env.",
	""
);

#if (defined (LEOPARD_ASIC_BOARD)) || (defined (LEOPARD_FPGA_BOARD))
U_BOOT_CMD(
	custom_image_check, 2,	1, do_custom_image_check,
	"check if image in load_addr is normal.",
	""
);

U_BOOT_CMD(
	utif_debug, 2,	1, do_utif_setting,
	"enable utif debug function.",
	""
);
U_BOOT_CMD(
	n9_jtag, 2,	1, do_switch_for_N9_jtag,
	"switch GPIO to AUX6 for N9 jtag debug.",
	""
);
U_BOOT_CMD(
	n9_uart, 2,	1, do_switch_for_N9_uart,
	"switch GPIO to AUX6 for N9 UART output.",
	""
);
U_BOOT_CMD(
	gpio_dump, 2,	1, do_gpio_dump,
	"enable utif debug function.",
	""
);
#endif

#if (defined (MT7622_FPGA_BOARD)) || (defined (MT7622_ASIC_BOARD))
U_BOOT_CMD(
	ctp_check, 2,	1, do_ctp_check,
	"check if ctp in load_addr is normal.",
	""
);

U_BOOT_CMD(
	boot_to_ctp,	CONFIG_SYS_MAXARGS,	1,	do_boot_to_ctp,
	"boot to ctp",
	""
);
#endif

U_BOOT_CMD(
	decrypt_image,	1,	1,	do_decrypt_image,
	"decrypt image in load_addr.",
	""
);

U_BOOT_CMD(
	check_image,	1,	1,	do_check_image,
	"check image in load_addr.",
	""
);

#endif /* !defined(CONFIG_SPL_BUILD) */

