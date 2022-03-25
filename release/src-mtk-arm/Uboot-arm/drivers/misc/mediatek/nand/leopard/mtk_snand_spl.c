/*
 * Driver for MediaTek SPI NAND Flash controller
 *
 * Copyright (C) 2018 MediaTek Inc.
 *
 * SPDX-License-Identifier:	 GPL-2.0+
 */

#include <common.h>
#include <linux/string.h>
#include <config.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <asm/arch/nand/mtk_nand.h>
#include <asm/arch/nand/mtk_snand_k.h>
#include <asm/arch/nand/snand_device_list.h>
#include <asm/arch/timer.h>

static struct mtd_info mtd;
u8 g_snand_spare_per_sector = 0;
static u32 g_value = 0;
bool g_bInitDone;
#define CACHE_ALIGNE_SIZE   (64)

__attribute__((aligned(CACHE_ALIGNE_SIZE))) unsigned char g_snand_spare[256];
__attribute__((aligned(CACHE_ALIGNE_SIZE))) unsigned char g_snand_temp[4096];
static u8 *local_buffer_16_align;   /* 16 byte aligned buffer, for HW issue */

#define NAND_SECTOR_SIZE    (512)
#define OOB_AVAI_PER_SECTOR (8)
#define GPIO_SNAND_BASE      0x10217000
#define GPIO_MODE(x)        (GPIO_SNAND_BASE + 0x300 + 0x10 * x)
#define GPIO_DRIV(x)        (GPIO_SNAND_BASE + 0x6600 + 0x10 * x)

void  bm_swap(struct mtd_info *mtd, u8 *fdmbuf, u8 *buf)
{
	u8 *psw1, *psw2, tmp;
	int sec_num = mtd->writesize / 512;

	/* the data */
	psw1 = buf + mtd->writesize - ((mtd->oobsize * (sec_num - 1) / sec_num));
	/* the first FDM byte of the last sector */
	psw2 = fdmbuf + (8 * (sec_num - 1));

	tmp = *psw1;
	*psw1 = *psw2;
	*psw2 = tmp;
}

void mtk_snand_clk_setting(void)
{
	int reg;

	/* NFI default clock is on */
	DRV_WriteReg32(0x1000205C, 1);
	/* Default is NFI mode, switch to SNAND mode */
	DRV_WriteReg32(RW_SNAND_CNFG, 1);

	reg = DRV_Reg32(SYS_CKGEN_BASE + SYS_CLK_CFG_1);
	reg = reg & ~0xf0000;
	reg = reg | 0x60000;
	DRV_WriteReg32(SYS_CKGEN_BASE + SYS_CLK_CFG_1, reg);
}

/**********************************************************
Description : SNAND_GPIO_config
***********************************************************/
void mtk_snand_gpio_config(int state)
{
	u32 reg;
	u32 snfi_gpio_backup[4] = {0};

	if (state) {
		/* pinmux config */
		snfi_gpio_backup[0] = DRV_Reg32(GPIO_MODE(7));
		snfi_gpio_backup[1] = DRV_Reg32(GPIO_MODE(8));
		reg = snfi_gpio_backup[0];
		reg &= ~(0xFF<<24);
		reg |= (0x22<<24);
		DRV_WriteReg32(GPIO_MODE(7), reg);
		reg = snfi_gpio_backup[1];
		reg &= ~(0xFFFF);
		reg |= (0x2222);
		DRV_WriteReg32(GPIO_MODE(8), reg);

		/* IO driving config */
		snfi_gpio_backup[2] = DRV_Reg32(GPIO_DRIV(1));
		snfi_gpio_backup[3] = DRV_Reg32(GPIO_DRIV(2));
		reg = snfi_gpio_backup[2];
		reg |= 0x66666000;
		DRV_WriteReg32(GPIO_DRIV(1), reg);
		reg = snfi_gpio_backup[3];
		reg |= 0x6;
		DRV_WriteReg32(GPIO_DRIV(2), reg);

	} else {
		DRV_WriteReg32(GPIO_MODE(7), snfi_gpio_backup[0]);
		DRV_WriteReg32(GPIO_MODE(8), snfi_gpio_backup[1]);
		DRV_WriteReg32(GPIO_DRIV(1), snfi_gpio_backup[2]);
		DRV_WriteReg32(GPIO_DRIV(2), snfi_gpio_backup[3]);
	}
}

/******************************************************************************
 * ECC_Config
 *
 * DESCRIPTION:
 *   Configure HW ECC!
 *
 * PARAMETERS:
 *   struct mtk_nand_host_hw *hw
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void ECC_Config(u32 ecc_bit)
{
	u32 encode_size;
	u32 decode_size;
	u32 ecc_bit_cfg = ECC_CNFG_ECC4;

	switch (ecc_bit) {
	case 4:
		ecc_bit_cfg = ECC_CNFG_ECC4;
		break;
	case 8:
		ecc_bit_cfg = ECC_CNFG_ECC8;
		break;
	case 10:
		ecc_bit_cfg = ECC_CNFG_ECC10;
		break;
	case 12:
		ecc_bit_cfg = ECC_CNFG_ECC12;
		break;
	default:
		break;

	}
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);

	do {;
	} while (!DRV_Reg16(ECC_DECIDLE_REG16));

	DRV_WriteReg16(ECC_ENCCON_REG16, ENC_DE);
	do {;
	} while (!DRV_Reg32(ECC_ENCIDLE_REG32));

	/* setup FDM register base */
	DRV_WriteReg32(ECC_FDMADDR_REG32, NFI_FDM0L_REG32);

	/* Sector + FDM */
	encode_size = (NAND_SECTOR_SIZE + 1) << 3;
	/* Sector + FDM + YAFFS2 meta data bits */
	decode_size = ((NAND_SECTOR_SIZE + 1) << 3) + ecc_bit * 13;

	/* configure ECC decoder && encoder */
	DRV_WriteReg32(ECC_DECCNFG_REG32, ecc_bit_cfg | DEC_CNFG_NFI | DEC_CNFG_EMPTY_EN | (decode_size << DEC_CNFG_CODE_SHIFT));
	DRV_WriteReg32(ECC_ENCCNFG_REG32, ecc_bit_cfg | ENC_CNFG_NFI | (encode_size << ENC_CNFG_MSG_SHIFT));
	NFI_SET_REG32(ECC_DECCNFG_REG32, DEC_CNFG_CORRECT);

}

/******************************************************************************
 * mtk_nand_configure_fdm
 *
 * DESCRIPTION:
 *   Configure the FDM data size !
 *
 * PARAMETERS:
 *   u16 fdmsize
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_nand_configure_fdm(u16 fdmsize, u16 fdmeccsize)
{
	NFI_CLN_REG16(NFI_PAGEFMT_REG16, PAGEFMT_FDM_MASK | PAGEFMT_FDM_ECC_MASK);
	NFI_SET_REG16(NFI_PAGEFMT_REG16, fdmsize << PAGEFMT_FDM_SHIFT);
	NFI_SET_REG16(NFI_PAGEFMT_REG16, fdmeccsize << PAGEFMT_FDM_ECC_SHIFT);
}

/******************************************************************************
 * mtk_snand_status_ready
 *
 * DESCRIPTION:
 *   Indicate the NAND device is ready or not !
 *
 * PARAMETERS:
 *   u32 status
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_status_ready(u32 status)
{
	u32 timeout = 0xFFFF;

	status &= ~STA_NAND_BUSY;

	while ((DRV_Reg32(NFI_STA_REG32) & status) != 0) {
		timeout--;

		if (0 == timeout)
			return false;
	}

	return true;
}

/******************************************************************************
 * mtk_snand_reset_con
 *
 * DESCRIPTION:
 *   Reset the NAND device hardware component !
 *
 * PARAMETERS:
 *   struct mtk_nand_host *host (Initial setting data)
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_reset_con(void)
{
	u32 reg32;

	/* HW recommended reset flow */
	int timeout = 0xFFFF;

	/* part 1. SNF */

	reg32 = DRV_Reg32(RW_SNAND_MISC_CTL);
	reg32 |= SNAND_SW_RST;
	DRV_WriteReg32(RW_SNAND_MISC_CTL, reg32);
	reg32 &= ~SNAND_SW_RST;
	DRV_WriteReg32(RW_SNAND_MISC_CTL, reg32);

	/* part 2. NFI */

	for (; ;) {
		if (0 == DRV_Reg16(NFI_MASTERSTA_REG16))
			break;

		timeout--;
		if (!timeout)
			MSG(INIT, "[%s] NFI_MASTERSTA_REG16 timeout!!\n", __func__);
	}

	DRV_WriteReg16(NFI_CON_REG16, CON_FIFO_FLUSH | CON_NFI_RST);

	for (; ;) {
		if (0 == (DRV_Reg16(NFI_STA_REG32) & (STA_NFI_FSM_MASK | STA_NAND_FSM_MASK)))
			break;

		timeout--;
		if (!timeout)
			MSG(INIT, "[%s] NFI_STA_REG32 timeout!!\n", __func__);
	}

	/* issue reset operation */

	return mtk_snand_status_ready(STA_NFI_FSM_MASK | STA_NAND_BUSY);
}

/******************************************************************************
 * mtk_nand_init_hw
 *
 * DESCRIPTION:
 *   Initial NAND device hardware component !
 *
 * PARAMETERS:
 *   struct mtk_nand_host *host (Initial setting data)
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_nand_init_hw(void)
{
	g_bInitDone = false;
	/* Set default NFI access timing control */
	DRV_WriteReg32(NFI_ACCCON_REG32, 0x30c77fff);
	DRV_WriteReg16(NFI_CNFG_REG16, 0);
	DRV_WriteReg16(NFI_PAGEFMT_REG16, 0);

	/* Reset the state machine and data FIFO, because flushing FIFO */
	mtk_snand_reset_con();

	NFI_SET_REG32(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
	ECC_Config(4);
	mtk_nand_configure_fdm(8, 1);

	/* Initilize interrupt. Clear interrupt, read clear. */
	DRV_Reg16(NFI_INTR_REG16);

	/* Interrupt arise when read data or program data to/from AHB is done. */
	DRV_WriteReg16(NFI_INTR_EN_REG16, 0);

	/* Set sample delay */
	DRV_WriteReg32(RW_SNAND_DLY_CTL3, 40);

	/* Enable automatic disable ECC clock when NFI is busy state */
	DRV_WriteReg16(NFI_DEBUG_CON1_REG16, (WBUF_EN|HWDCM_SWCON_ON));

}
static void mtk_snand_dev_mac_enable(SNAND_Mode mode)
{
	u32 mac;

	mac = DRV_Reg32(RW_SNAND_MAC_CTL);

	/* SPI */
	if (mode == SPI) {
		mac &= ~SNAND_MAC_SIO_SEL;   /* Clear SIO_SEL to send command in SPI style */
		mac |= SNAND_MAC_EN;         /* Enable Macro Mode */
	}
	/* QPI */
	else {
		/*
		 * SFI V2: QPI_EN only effects direct read mode, and it is moved into DIRECT_CTL in V2
		 *         There's no need to clear the bit again.
		 */
		mac |= (SNAND_MAC_SIO_SEL | SNAND_MAC_EN);  /* Set SIO_SEL to send command in QPI style, and enable Macro Mode */
	}

	DRV_WriteReg32(RW_SNAND_MAC_CTL, mac);
}

/**
 * @brief This funciton triggers SFI to issue command to serial flash, wait SFI until ready.
 *
 * @remarks: !NOTE! This function must be used with mtk_snand_dev_mac_enable in pair!
 */
static void mtk_snand_dev_mac_trigger(void)
{
	u32 mac;

	mac = DRV_Reg32(RW_SNAND_MAC_CTL);

	/* Trigger SFI: Set TRIG and enable Macro mode */
	mac |= (SNAND_TRIG | SNAND_MAC_EN);
	DRV_WriteReg32(RW_SNAND_MAC_CTL, mac);

	/*
	 * Wait for SFI ready
	 * Step 1. Wait for WIP_READY becoming 1 (WIP register is ready)
	 */
	while (!(DRV_Reg32(RW_SNAND_MAC_CTL) & SNAND_WIP_READY));

	/*
	 * Step 2. Wait for WIP becoming 0 (Controller finishes command write process)
	 */
	while ((DRV_Reg32(RW_SNAND_MAC_CTL) & SNAND_WIP));

}

/**
 * @brief This funciton leaves Macro mode and enters Direct Read mode
 *
 * @remarks: !NOTE! This function must be used after mtk_snand_dev_mac_trigger
 */
static void mtk_snand_dev_mac_leave(void)
{
	u32 mac;

	/* clear SF_TRIG and leave mac mode */
	mac = DRV_Reg32(RW_SNAND_MAC_CTL);

	/*
	 * Clear following bits
	 * SF_TRIG: Confirm the macro command sequence is completed
	 * SNAND_MAC_EN: Leaves macro mode, and enters direct read mode
	 * SNAND_MAC_SIO_SEL: Always reset quad macro control bit at the end
	 */
	mac &= ~(SNAND_TRIG | SNAND_MAC_EN | SNAND_MAC_SIO_SEL);
	DRV_WriteReg32(RW_SNAND_MAC_CTL, mac);
}

static void mtk_snand_dev_mac_op(SNAND_Mode mode)
{
	mtk_snand_dev_mac_enable(mode);
	mtk_snand_dev_mac_trigger();
	mtk_snand_dev_mac_leave();
}

static void mtk_snand_dev_command_ext(SNAND_Mode mode, const u8 cmd[], u8 data[], const u32 outl, const u32 inl)
{
	u32   tmp;
	u32   i, j, p_data;
	volatile u8 *p_tmp;

	p_tmp = (u8 *)(&tmp);

	/* Moving commands into SFI GPRAM */
	for (i = 0, p_data = RW_SNAND_GPRAM_DATA; i < outl; p_data += 4) {
		/* Using 4 bytes aligned copy, by moving the data into the temp buffer and then write to GPRAM */
		for (j = 0, tmp = 0; i < outl && j < 4; i++, j++)
			p_tmp[j] = cmd[i];

		DRV_WriteReg32(p_data, tmp);
	}

	DRV_WriteReg32(RW_SNAND_MAC_OUTL, outl);
	DRV_WriteReg32(RW_SNAND_MAC_INL, inl);
	mtk_snand_dev_mac_op(mode);

	/* for NULL data, this loop will be skipped */
	for (i = 0, p_data = RW_SNAND_GPRAM_DATA + outl; i < inl; ++i, ++data, ++p_data)
		*data = DRV_Reg8(p_data);

	return;
}

static void mtk_snand_reset_dev(void)
{
	u8 cmd = SNAND_CMD_SW_RESET;

	/* issue SW RESET command to device */
	mtk_snand_dev_command_ext(SPI, &cmd, NULL, 1, 0);

	/* wait for awhile, then polling status register (required by spec) */
	udelay(SNAND_DEV_RESET_LATENCY_US);

	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_STATUS << 8));
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

	/* polling status register */

	for (; ;) {
		mtk_snand_dev_mac_op(SPI);

		cmd = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);

		if (0 == (cmd & SNAND_STATUS_OIP))
			break;
	}
}


static void mtk_snand_dev_read_id(u8 id[])
{
	u8 cmd[] = {SNAND_CMD_READ_ID, 0};

	mtk_snand_dev_command_ext(SPI, cmd, id, 2, SNAND_MAX_ID);
}

bool mtk_snand_get_device_info(u8 *id, snand_flashdev_info *devinfo)
{
	u32 i, m, n, mismatch;
	int target = -1;
	u8 target_id_len = 0;

	for (i = 0; i < ARRAY_SIZE(gen_snand_FlashTable); i++) {
		mismatch = 0;

		for (m = 0; m < gen_snand_FlashTable[i].id_length; m++) {
			if (id[m] != gen_snand_FlashTable[i].id[m]) {
				mismatch = 1;
				break;
			}
		}

		if (mismatch == 0 && gen_snand_FlashTable[i].id_length > target_id_len) {
				target = i;
				target_id_len = gen_snand_FlashTable[i].id_length;
		}
	}

	if (target != -1) {
		MSG(INIT, "Recognize SNAND: ID [");

		for (n = 0; n < gen_snand_FlashTable[target].id_length; n++) {
			devinfo->id[n] = gen_snand_FlashTable[target].id[n];
			MSG(INIT, "%x ", devinfo->id[n]);
		}

		MSG(INIT, "], Device Name [%s], Page Size [%d]B Spare Size [%d]B Total Size [%d]MB\n", gen_snand_FlashTable[target].devicename, gen_snand_FlashTable[target].pagesize, gen_snand_FlashTable[target].sparesize, gen_snand_FlashTable[target].totalsize);
		devinfo->id_length = gen_snand_FlashTable[target].id_length;
		devinfo->blocksize = gen_snand_FlashTable[target].blocksize;    /* KB */
		devinfo->advancedmode = gen_snand_FlashTable[target].advancedmode;
		devinfo->pagesize = gen_snand_FlashTable[target].pagesize;
		devinfo->sparesize = gen_snand_FlashTable[target].sparesize;
		devinfo->totalsize = gen_snand_FlashTable[target].totalsize;

		memcpy(devinfo->devicename, gen_snand_FlashTable[target].devicename, sizeof(devinfo->devicename));

		devinfo->SNF_DLY_CTL1 = gen_snand_FlashTable[target].SNF_DLY_CTL1;
		devinfo->SNF_DLY_CTL2 = gen_snand_FlashTable[target].SNF_DLY_CTL2;
		devinfo->SNF_DLY_CTL3 = gen_snand_FlashTable[target].SNF_DLY_CTL3;
		devinfo->SNF_DLY_CTL4 = gen_snand_FlashTable[target].SNF_DLY_CTL4;
		devinfo->SNF_MISC_CTL = gen_snand_FlashTable[target].SNF_MISC_CTL;
		devinfo->SNF_DRIVING = gen_snand_FlashTable[target].SNF_DRIVING;
		g_snand_spare_per_sector = devinfo->sparesize / (devinfo->pagesize / NAND_SECTOR_SIZE);

		return true;
	} else {
		MSG(INIT, "Not Found NAND: ID [");
		for (n = 0; n < SNAND_MAX_ID; n++)
			MSG(INIT, "%x ", id[n]);
		MSG(INIT, "]\n");
		return false;
	}
}

static u32 mtk_snand_reverse_byte_order(u32 num)
{
	u32 ret = 0;

	ret |= ((num >> 24) & 0x000000FF);
	ret |= ((num >> 8) & 0x0000FF00);
	ret |= ((num << 8) & 0x00FF0000);
	ret |= ((num << 24) & 0xFF000000);

	return ret;
}

static u32 mtk_snand_gen_c1a3(const u32 cmd, const u32 address)
{
	return (mtk_snand_reverse_byte_order(address) & 0xFFFFFF00) | (cmd & 0xFF);
}


static void mtk_snand_dev_enable_spiq(bool enable)
{
	u8   regval;
	u32  cmd;

	/* read QE in status register */
	cmd = SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_OTP << 8);
	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

	mtk_snand_dev_mac_op(SPI);

	regval = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);
	/* MSG(INIT, [SNF] [Cheng] read status register[configuration register]:0x%x \n\r", regval); */

	if (false == enable) {
		if ((regval & SNAND_OTP_QE) == 0)
			return;
		else
			regval = regval & ~SNAND_OTP_QE;
	} else {
		if ((regval & SNAND_OTP_QE) == 1)
			return;
		else
			regval = regval | SNAND_OTP_QE;
	}

	/* if goes here, it means QE needs to be set as new different value */

	/* write status register */
	cmd = SNAND_CMD_SET_FEATURES | (SNAND_CMD_FEATURES_OTP << 8) | (regval << 16);
	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 3);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 0);

	mtk_snand_dev_mac_op(SPI);
}


/******************************************************************************
 * mtk_snand_set_mode
 *
 * DESCRIPTION:
 *    Set the oepration mode !
 *
 * PARAMETERS:
 *   u16 opmode (read/write)
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_set_mode(u16 opmode)
{
	u16 mode = DRV_Reg16(NFI_CNFG_REG16);
	mode &= ~CNFG_OP_MODE_MASK;
	mode |= opmode;
	DRV_WriteReg16(NFI_CNFG_REG16, mode);
}

/******************************************************************************
 * mtk_snand_set_autoformat
 *
 * DESCRIPTION:
 *    Enable/Disable hardware autoformat !
 *
 * PARAMETERS:
 *   bool enable (Enable/Disable)
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_set_autoformat(bool enable)
{
	if (enable)
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
	else
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AUTO_FMT_EN);
}
/******************************************************************************
 * mtk_snand_ecc_decode_start
 *
 * DESCRIPTION:
 *   HW ECC Decode Start !
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_ecc_decode_start(void)
{
	/* wait for device returning idle */
	while (!(DRV_Reg16(ECC_DECIDLE_REG16) & DEC_IDLE)) ;
	DRV_WriteReg16(ECC_DECCON_REG16, DEC_EN);
}

/******************************************************************************
 * mtk_snand_ecc_decode_end
 *
 * DESCRIPTION:
 *   HW ECC Decode End !
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_ecc_decode_end(void)
{
	u32 timeout = 0xFFFF;

	/* wait for device returning idle */
	while (!(DRV_Reg16(ECC_DECIDLE_REG16) & DEC_IDLE)) {
		timeout--;

		if (timeout == 0)
			break;
	}

	DRV_WriteReg16(ECC_DECCON_REG16, DEC_DE);
}

/******************************************************************************
 * mtk_snand_ready_for_read
 *
 * DESCRIPTION:
 *    Prepare hardware environment for read !
 *
 * PARAMETERS:
 *   struct nand_chip *nand, u32 rowaddr
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_ready_for_read(struct nand_chip *nand, u32 rowaddr, u32 coladdr, u32 sectors, bool full, u8 *buf)
{
	/* Reset NFI HW internal state machine and flush NFI in/out FIFO */
	bool ret = false;
	u32 cmd, reg;
	u32 col_addr = coladdr;
	SNAND_Mode mode = SPIQ;
	unsigned long phys = 0;
	u32 timeout = 0xFFFF;

	if (!mtk_snand_reset_con())
		goto cleanup;

	/* 1. Read page to cache */

	cmd = mtk_snand_gen_c1a3(SNAND_CMD_PAGE_READ, rowaddr); /* PAGE_READ command + 3-byte address */

	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 1 + 3);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 0);

	mtk_snand_dev_mac_op(SPI);

	/* 2. Get features (status polling) */

	cmd = SNAND_CMD_GET_FEATURES | (SNAND_CMD_FEATURES_STATUS << 8);

	DRV_WriteReg32(RW_SNAND_GPRAM_DATA, cmd);
	DRV_WriteReg32(RW_SNAND_MAC_OUTL, 2);
	DRV_WriteReg32(RW_SNAND_MAC_INL, 1);

	for (; ;) {
		mtk_snand_dev_mac_op(SPI);

		cmd = DRV_Reg8(RW_SNAND_GPRAM_DATA + 2);

		if ((cmd & SNAND_STATUS_OIP) == 0)
			break;

		timeout--;
		if (timeout == 0) {
			MSG(INIT, "Polling SNAND_STATUS_OIP Fail \r\n");
			break;
		}
	}

	/* ------ SNF Part ------ */
	/* set PAGE READ command & address */
	reg = (SNAND_CMD_PAGE_READ << SNAND_PAGE_READ_CMD_OFFSET) | (rowaddr & SNAND_PAGE_READ_ADDRESS_MASK);
	DRV_WriteReg32(RW_SNAND_RD_CTL1, reg);

	/* set DATA READ dummy cycle and command (use default value, ignored) */
	if (mode == SPI) {
		reg = DRV_Reg32(RW_SNAND_RD_CTL2);
		reg &= ~SNAND_DATA_READ_CMD_MASK;
		reg |= SNAND_CMD_RANDOM_READ & SNAND_DATA_READ_CMD_MASK;
		DRV_WriteReg32(RW_SNAND_RD_CTL2, reg);
	} else if (mode == SPIQ) {
		mtk_snand_dev_enable_spiq(true);

		reg = DRV_Reg32(RW_SNAND_RD_CTL2);
		reg &= ~SNAND_DATA_READ_CMD_MASK;
		reg |= SNAND_CMD_RANDOM_READ_SPIQ & SNAND_DATA_READ_CMD_MASK;
		DRV_WriteReg32(RW_SNAND_RD_CTL2, reg);
	}

	/* set DATA READ address */
	DRV_WriteReg32(RW_SNAND_RD_CTL3, (col_addr & SNAND_DATA_READ_ADDRESS_MASK));

	/* set SNF timing */
	reg = DRV_Reg32(RW_SNAND_MISC_CTL);

	reg |= SNAND_DATARD_CUSTOM_EN;

	if (mode == SPI) {
		reg &= ~SNAND_DATA_READ_MODE_MASK;
	} else if (mode == SPIQ) {
		reg &= ~SNAND_DATA_READ_MODE_MASK;
		reg |= ((SNAND_DATA_READ_MODE_X4 << SNAND_DATA_READ_MODE_OFFSET) & SNAND_DATA_READ_MODE_MASK);
	}

	DRV_WriteReg32(RW_SNAND_MISC_CTL, reg);

	/* set SNF data length */

	reg = sectors * (NAND_SECTOR_SIZE + g_snand_spare_per_sector);

	DRV_WriteReg32(RW_SNAND_MISC_CTL2, (reg | (reg << SNAND_PROGRAM_LOAD_BYTE_LEN_OFFSET)));

	/* ------ NFI Part ------ */

	mtk_snand_set_mode(CNFG_OP_CUST);
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_READ_EN);
	DRV_WriteReg16(NFI_CON_REG16, sectors << CON_NFI_SEC_SHIFT);

	/* Enable HW ECC */
	NFI_SET_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);

	if (full) {

		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_AHB);
		phys = (u32)buf;

		if (!phys) {
			MSG(INIT, "[mtk_snand_ready_for_read]convert virt addr (%x) to phys add (%x)fail!!!", (u32)buf, (u32)phys);
			return false;
		} else {
			DRV_WriteReg32(NFI_STRADDR_REG32, phys);
		}

	} else {
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_HW_ECC_EN);
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_AHB);
	}

	mtk_snand_set_autoformat(full);

	if (full)
		mtk_snand_ecc_decode_start();

	ret = true;

cleanup:

	return ret;
}

/******************************************************************************
 * mtk_snand_read_page_data
 *
 * DESCRIPTION:
 *   Fill the page data into buffer !
 *
 * PARAMETERS:
 *   u8* databuf, u32 u4Size
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_dma_read_data(struct mtd_info *mtd, u8 *buf, u32 num_sec, u8 full)
{
	int timeout = 0xffff;

	NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_BYTE_RW);

	if ((unsigned int)buf % 16) {
		MSG(INIT, "Un-16-aligned address\n");
		NFI_CLN_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
	} else {
		NFI_SET_REG16(NFI_CNFG_REG16, CNFG_DMA_BURST_EN);
	}

	/* set dummy command to trigger NFI enter custom mode */
	DRV_WriteReg16(NFI_CMD_REG16, NAND_CMD_DUMMYREAD);

	DRV_Reg16(NFI_INTR_REG16);  /* read clear */

	DRV_WriteReg16(NFI_INTR_EN_REG16, INTR_AHB_DONE_EN);

	NFI_SET_REG16(NFI_CON_REG16, CON_NFI_BRD);

	while (!DRV_Reg16(NFI_INTR_REG16)) {
		timeout--;

		if (0 == timeout) {
			MSG(INIT, "[%s] poll nfi_intr error\n", __func__);
			return false;   /* AHB Mode Time Out! */
		}
	}

	while (num_sec > ((DRV_Reg16(NFI_BYTELEN_REG16) & 0xf000) >> 12)) {
		timeout--;

		if (0 == timeout) {
			MSG(INIT, "[%s] Poll BYTELEN error(num_sec=%d)\n", __func__, num_sec);
			return false;   /* AHB Mode Time Out! */
		}
	}

	return true;
}

static bool mtk_snand_read_page_data(struct mtd_info *mtd, u8 *databuf, u32 sectors, u8 full)
{
	return mtk_snand_dma_read_data(mtd, databuf, sectors, full);
}

static bool mtk_snand_check_dececc_done(u32 sectors)
{
	u32 timeout, dec_mask;
	timeout = 0xffff;
	dec_mask = (1 << sectors) - 1;
	while ((dec_mask != DRV_Reg16(ECC_DECDONE_REG16)) && timeout > 0)
		timeout--;
	if (timeout == 0) {
		MSG(VERIFY, "ECC_DECDONE: timeout\n");
		return false;
	}
	return true;
}

/******************************************************************************
 * mtk_snand_read_fdm_data
 *
 * DESCRIPTION:
 *   Read a fdm data !
 *
 * PARAMETERS:
 *   u8* databuf, u32 sectors
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_read_fdm_data(u8 *databuf, u32 sectors)
{
	u32 i;
	u32 *buf32 = (u32 *) databuf;

	if (buf32) {
		for (i = 0; i < sectors; ++i) {
			*buf32++ = DRV_Reg32(NFI_FDM0L_REG32 + (i * 8));
			*buf32++ = DRV_Reg32(NFI_FDM0M_REG32 + (i * 8));
		}
	}
}

/******************************************************************************
 * mtk_snand_check_bch_error
 *
 * DESCRIPTION:
 *   Check BCH error or not !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd
 *	 u8* databuf
 *	 u32 u4SecIndex
 *	 u32 u4PageAddr
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static bool mtk_snand_check_bch_error(struct mtd_info *mtd, u8 *databuf, u8 *spareBuf, u32 u4SecIndex, u32 u4PageAddr)
{
	bool ret = true;
	u16 sectordonemask = 1 << u4SecIndex;
	u32 errornumdebug0, errornumdebug1, i, errnum;
	u32 timeout = 0xFFFF;
	u32 correct_count = 0;
	u16 failed_sec = 0;

	while (0 == (sectordonemask & DRV_Reg16(ECC_DECDONE_REG16))) {
		timeout--;
		if (0 == timeout)
			return false;
	}

	errornumdebug0 = DRV_Reg32(ECC_DECENUM0_REG32);
	errornumdebug1 = DRV_Reg32(ECC_DECENUM1_REG32);
	if (0 != (errornumdebug0 & 0xFFFFF) || 0 != (errornumdebug1 & 0xFFFFF)) {
		for (i = 0; i <= u4SecIndex; ++i) {
			if (i < 4)
				errnum = DRV_Reg32(ECC_DECENUM0_REG32) >> (i * 5);
			else
				errnum = DRV_Reg32(ECC_DECENUM1_REG32) >> ((i - 4) * 5);

			errnum &= 0x1F;
		failed_sec = 0;

			if (0x1F == errnum) {
				failed_sec++;
				ret = false;
				MSG(INIT, "[%s] ECC-U, PA=%d, S=%d\n", __func__, u4PageAddr, i);
			} else {
				if (errnum)
					correct_count += errnum;
			}
		}

	mtd->ecc_stats.failed += failed_sec;
		if (correct_count > 2 && ret)
			mtd->ecc_stats.corrected++;
	}
	return ret;
}

/******************************************************************************
 * mtk_snand_stop_read
 *
 * DESCRIPTION:
 *   Stop read operation !
 *
 * PARAMETERS:
 *   None
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_stop_read(void)
{
	/* ------ NFI Part */
	NFI_CLN_REG16(NFI_CON_REG16, CON_NFI_BRD);

	/* ------ SNF Part */
	/* set 1 then set 0 to clear done flag */
	DRV_WriteReg32(RW_SNAND_STA_CTL1, SNAND_CUSTOM_PROGRAM);
	DRV_WriteReg32(RW_SNAND_STA_CTL1, 0);

	/* clear essential SNF setting */
	NFI_CLN_REG32(RW_SNAND_MISC_CTL, SNAND_PG_LOAD_CUSTOM_EN);
	mtk_snand_ecc_decode_end();
	DRV_WriteReg16(NFI_INTR_EN_REG16, 0);

	mtk_snand_reset_con();
}

/******************************************************************************
 * mtk_nand_exec_read_page
 *
 * DESCRIPTION:
 *   Read a page data !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, u32 rowaddr, u32 pagesize,
 *   u8* pagebuf, u8* fdmbuf
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/

int mtk_nand_exec_read_page(struct mtd_info *mtd, u32 rowaddr, u32 pagesize, u8 *pagebuf, u8 *fdmbuf)
{
	u8 *buf;
	bool ret = true;
	struct nand_chip *nand = mtd->priv;
	u32 sectors = pagesize >> 9;

	memset(local_buffer_16_align, 0x5a, pagesize);
#if !defined (CONFIG_SYS_DCACHE_OFF)
		if (((u32) pagebuf % 64) && local_buffer_16_align)
#else
		if (((unsigned long) pagebuf % 16) && local_buffer_16_align)
#endif
		{
			MSG(INIT, "<READ> Data buffer not 16 bytes aligned %d:, address %p\n", pagesize, pagebuf);
			buf = local_buffer_16_align;
		} else {
			buf = pagebuf;
		}

	if (mtk_snand_ready_for_read(nand, rowaddr, 0, sectors, true, buf)) {
		if (!mtk_snand_read_page_data(mtd, buf, sectors, true)) {
			MSG(INIT, "[%s]mtk_snand_read_page_data() FAIL!!!\n", __func__);
			ret = false;
		}

		if (!mtk_snand_status_ready(STA_NAND_BUSY)) {
			ret = false;
			MSG(INIT, "mtk_snand_status_ready FAIL!!!\n");
		}

		if (!mtk_snand_check_dececc_done(sectors)) {
			MSG(INIT, "[%s]mtk_snand_check_dececc_done() FAIL!!!\n", __func__);
			ret = false;
		}

		mtk_snand_read_fdm_data(fdmbuf, sectors);
		if (!mtk_snand_check_bch_error(mtd, buf, fdmbuf, sectors - 1, rowaddr)) {
			MSG(INIT, "[%s]mtk_snand_check_bch_error() FAIL!!!\n", __func__);
			ret = false;
		}
		bm_swap(mtd, fdmbuf, buf);
		mtk_snand_stop_read();
	}

	mtk_snand_dev_enable_spiq(false);
	if (buf == local_buffer_16_align)
		memcpy(pagebuf, buf, pagesize);

	return ret;
}

/******************************************************************************
 *
 * Read a page to a logical address
 *
 *****************************************************************************/
static int mtk_snand_read_page(struct mtd_info *mtd, struct nand_chip *chip, u8 *buf, int page)
{
	if (mtk_nand_exec_read_page(mtd, page, mtd->writesize, buf, g_snand_spare))
		return 0;

	return -EIO;
}

/******************************************************************************
 * mtk_snand_read_oob_raw
 *
 * DESCRIPTION:
 *   Read oob data
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, const uint8_t *buf, int addr, int len
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   this function read raw oob data out of flash, so need to re-organise
 *   data format before using.
 *   len should be times of 8, call this after nand_get_device.
 *   Should notice, this function read data without ECC protection.
 *
 *****************************************************************************/
static int mtk_snand_read_oob_raw(struct mtd_info *mtd, uint8_t *buf, int page_addr, int len)
{
	struct nand_chip *chip = (struct nand_chip *)mtd->priv;
	int ret = true;
	int num_sec;

	num_sec = len / OOB_AVAI_PER_SECTOR;

	if (mtk_snand_ready_for_read(chip, page_addr, 0, num_sec, true, g_snand_temp)) {

		if (!mtk_snand_read_page_data(mtd, g_snand_temp, num_sec, true))
			ret = false;

		if (!mtk_snand_status_ready(STA_NAND_BUSY))
			ret = false;

		if (!mtk_snand_check_dececc_done(num_sec))
			ret = false;

		mtk_snand_read_fdm_data(g_snand_spare, num_sec);
		bm_swap(mtd, g_snand_spare, g_snand_temp);
		mtk_snand_stop_read();
	}

	mtk_snand_dev_enable_spiq(false);

	if (true == ret)
		return 0;
	else
		return 1;
}

int mtk_nand_block_bad_hw(struct mtd_info *mtd, loff_t ofs)
{
	struct nand_chip *chip = (struct nand_chip *)mtd->priv;
	int page_addr = (int)(ofs >> chip->page_shift);
	unsigned int page_per_block = 1 << (chip->phys_erase_shift - chip->page_shift);
	unsigned int pagesize = 1 << chip->page_shift;
	page_addr &= ~(page_per_block - 1);
	u32 sectors = pagesize >> 9;
	unsigned int offset = (sectors - 1) * OOB_AVAI_PER_SECTOR;

	if (mtk_snand_read_oob_raw(mtd, g_snand_spare, page_addr, sectors * OOB_AVAI_PER_SECTOR)) {
		MSG(INIT, "mtk_snand_read_oob_raw return error\n");
		return 1;
	}

	if (g_snand_spare[offset] != 0xff) {
		MSG(INIT, "Bad block detected at 0x%x, Badmark is 0x%x\n", page_addr, g_snand_spare[offset]);
		return 1;
	}

	return 0;                   /* everything is OK, good block */
}

/******************************************************************************
 * mtk_snand_select_chip
 *
 * DESCRIPTION:
 *   Select a chip !
 *
 * PARAMETERS:
 *   struct mtd_info *mtd, int chip
 *
 * RETURNS:
 *   None
 *
 * NOTES:
 *   None
 *
 ******************************************************************************/
static void mtk_snand_select_chip(struct mtd_info *mtd, int chip)
{
	if (chip == -1 && false == g_bInitDone) {

		u32 spare_per_sector = mtd->oobsize/(mtd->writesize/512);
		u32 ecc_bit = 4;
		u32 spare_bit = PAGEFMT_SPARE_16;

		if (spare_per_sector >= 28) {
			spare_bit = PAGEFMT_SPARE_28;
			ecc_bit = 12;
			spare_per_sector = 28;
		} else if (spare_per_sector >= 27) {
			spare_bit = PAGEFMT_SPARE_27;
			ecc_bit = 8;
			spare_per_sector = 27;
		} else if (spare_per_sector >= 26) {
			spare_bit = PAGEFMT_SPARE_26;
			ecc_bit = 8;
			spare_per_sector = 26;
		} else if (spare_per_sector >= 16) {
			spare_bit = PAGEFMT_SPARE_16;
			ecc_bit = 4;
			spare_per_sector = 16;
		} else {
			MSG(INIT, "[NAND]: NFI not support oobsize: %x\n", spare_per_sector);
		}
		mtd->oobsize = spare_per_sector * (mtd->writesize/512);

		/* Setup PageFormat */
		if (4096 == mtd->writesize)
			NFI_SET_REG16(NFI_PAGEFMT_REG16, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_4K);
		else if (2048 == mtd->writesize)
			NFI_SET_REG16(NFI_PAGEFMT_REG16, (spare_bit << PAGEFMT_SPARE_SHIFT) | PAGEFMT_2K);

		ECC_Config(ecc_bit);
		g_snand_spare_per_sector = spare_per_sector;

		g_bInitDone = true;
	}
	switch (chip) {
		case -1:
			break;
		case 0:
#if defined(CONFIG_EARLY_LINUX_PORTING)		/* FPGA NAND is placed at CS1 not CS0 */
			/* DRV_WriteReg16(NFI_CSEL_REG16, 1); */
			break;
#endif
		case 1:
			/* DRV_WriteReg16(NFI_CSEL_REG16, chip); */
			break;
	}
}

int nand_spl_load_image(uint32_t offs, unsigned int size, void *dst)
{
	unsigned int block, lastblock;
	unsigned int page, page_addr, pagesize, pagecount;
	struct nand_chip *chip;
	uint64_t blocksize;

	chip = mtd.priv;
	blocksize = 1 << chip->phys_erase_shift;
	pagesize = 1 << chip->page_shift;
	pagecount = 1 << (chip->phys_erase_shift - chip->page_shift);

	/*
	 * offs has to be aligned to a page address!
	 */
	block = offs / blocksize;
	lastblock = (offs + size - 1) / blocksize;
	page = (offs % blocksize) / pagesize;

	while (block <= lastblock) {
		if (!mtk_nand_block_bad_hw(&mtd, block << chip->phys_erase_shift)) {
			page_addr = page + block * pagecount;
			while (page < pagecount) {
				if (!mtk_snand_read_page(&mtd, chip, dst, page_addr)) {
					dst += pagesize;
				}
				page_addr++;
				page++;
			}
			page = 0;
		} else {
			lastblock++;
		}
		block++;
	}

	return 0;
}

/* nand_init() - initialize data to make nand usable by SPL */
void nand_init(void)
{
	u8 id[SNAND_MAX_ID];
	snand_flashdev_info devinfo;
	struct nand_chip chip;

	mtd.priv = &chip;
	mtd.name = "MTK-SNand";

	/* Select the device */
	mtk_snand_select_chip(&mtd, 0);

	mtk_snand_clk_setting();
#if !defined(LEOPARD_FPGA_BOARD)
	mtk_snand_gpio_config(true);
#endif
	mtk_nand_init_hw();
	mtk_snand_reset_dev();

	mtk_snand_dev_read_id(id);
	if (!mtk_snand_get_device_info(id, &devinfo))
		MSG(INIT, "Not Support this Device! \r\n");

	mtd.oobsize = devinfo.sparesize;
	mtd.writesize = devinfo.pagesize;
	mtd.erasesize = devinfo.blocksize * 1024;

	chip.chipsize = (((uint64_t)devinfo.totalsize) << 20);
	chip.page_shift = ffs(mtd.writesize) - 1;
	chip.phys_erase_shift = ffs(devinfo.blocksize * 1024)-1;
	chip.numchips = 1;
	chip.pagemask = (chip.chipsize >> chip.page_shift) - 1;
	mtk_snand_select_chip(&mtd, -1);
	MSG(INIT, "MTK SPI NAND: nand init Success!\n");
}

/* Unselect after operation */
void nand_deselect(void)
{
	mtk_snand_select_chip(&mtd, -1);
}

