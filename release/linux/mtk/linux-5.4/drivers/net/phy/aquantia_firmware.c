// SPDX-License-Identifier: GPL-2.0
/* FW download driver for Aquantia PHY
 */

#include <linux/phy.h>
#include <linux/of.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/kthread.h>

#include "aquantia.h"

#undef AQ_VERBOSE

#ifdef CONFIG_AQUANTIA_PHY_FW_DOWNLOAD_SINGLE
#define MAX_GANGLOAD_DEVICES				1
#elif CONFIG_AQUANTIA_PHY_FW_DOWNLOAD_GANG
#define MAX_GANGLOAD_DEVICES				2
#endif

#define AQR_FIRMWARE					CONFIG_AQUANTIA_PHY_FW_FILE

/* Vendor specific 1, MDIO_MMD_VEND1 */
#define VEND1_STD_CONTROL1				0x0000
#define VEND1_STD_CONTROL1_SOFT_RESET			BIT(15)

#define VEND1_MAILBOX_INTERFACE1			0x0200
#define VEND1_MAILBOX_INTERFACE1_START			BIT(15)
#define VEND1_MAILBOX_INTERFACE1_WRITE			FIELD_PREP(BIT(14), 1)
#define VEND1_MAILBOX_INTERFACE1_READ			FIELD_PREP(BIT(14), 0)
#define VEND1_MAILBOX_INTERFACE1_RESET_CRC		BIT(12)

#define VEND1_MAILBOX_INTERFACE2			0x0201
#define VEND1_MAILBOX_INTERFACE2_CRC			GENMASK(15, 0)

#define VEND1_MAILBOX_INTERFACE3			0x0202
#define VEND1_MAILBOX_INTERFACE3_ADDR_MSW		GENMASK(15, 0)

#define VEND1_MAILBOX_INTERFACE4			0x0203
#define VEND1_MAILBOX_INTERFACE4_ADDR_LSW		GENMASK(15, 2)

#define VEND1_MAILBOX_INTERFACE5			0x0204
#define VEND1_MAILBOX_INTERFACE5_DATA_MSW		GENMASK(15, 0)

#define VEND1_MAILBOX_INTERFACE6			0x0205
#define VEND1_MAILBOX_INTERFACE6_DATA_LSW		GENMASK(15, 0)

#define VEND1_CONTROL2					0xc001
#define VEND1_CONTROL2_UP_RESET				BIT(15)
#define VEND1_CONTROL2_UP_RUNSTALL_OVERRIDE		BIT(6)
#define VEND1_CONTROL2_UP_RUNSTALL			BIT(0)

#define VEND1_RESET_CONTROL				0xc006
#define VEND1_RESET_CONTROL_MMD_RESET_DISABLE		BIT(14)

#define VEND1_GENERAL_PROV2				0xc441
#define VEND1_GENERAL_PROV2_MDIO_BROADCAST_ENABLE	BIT(14)

#define VEND1_GENERAL_PROV8				0xc447
#define VEND1_GENERAL_PROV8_MDIO_BROADCAST_ADDRESS	GENMASK(4, 0)

#define VEND1_NVR_PROV3					0xc452
#define VEND1_NVR_PROV3_DAISYCHAIN_DISABLE		BIT(0)

#define VEND1_RSVD_PROV2				0xc471
#define VEND1_RSVD_PROV2_DAISYCHAIN_HOPCOUNT		GENMASK(5, 0)
#define VEND1_RSVD_PROV2_DAISYCHAIN_HOPCOUNT_OVERRIDE	BIT(6)

/*! The byte address, in processor memory, of the start of the IRAM segment. */
#define AQ_IRAM_BASE_ADDRESS				0x40000000

/*! The byte address, in processor memory, of the start of the DRAM segment. */
#define AQ_DRAM_BASE_ADDRESS				0x3FFE0000

/*! The byte offset from the top of the PHY image to the header content (HHD & EUR devices). */
#define AQ_PHY_IMAGE_HEADER_CONTENT_OFFSET_HHD		0x300

/*! The offset, from the start of DRAM, where the provisioning block begins. */
#define AQ_PHY_IMAGE_PROVTABLE_OFFSET			0x680

/*! The offset, from the start of DRAM, where the provisioning block's ending address is recorded. */
#define AQ_PHY_IMAGE_PROVTABLE_TERM_OFFSET		0x028C

/*! The size of the space alloted within the PHY image for the provisioning table. */
#define AQ_PHY_IMAGE_PROVTABLE_MAXSIZE			0x800

/*! The maximum number of ports that can be MDIO bootloaded at once. */
#define AQ_MAX_NUM_PHY_IDS				48

/*! This enumeration is used to describe the different types of
    Aquantia PHY.*/
typedef enum
{
	/*! 1/2/4-port package, 40nm architechture.*/
	AQ_DEVICE_APPIA,
	/*! 1/2/4-port package, first-generation 28nm architechture.*/
	AQ_DEVICE_HHD,
	/*! 1/2/4-port package, second-generation 28nm architechture.*/
	AQ_DEVICE_EUR,
	/*! 1/2/4-port package, third-generation 28nm architechture.*/
	AQ_DEVICE_CAL,
	/*! 1/2/4/8-port package, forth-generation 14nm architechture.*/
	AQ_DEVICE_RHEA,
	/*! 8-port package, fifth-generation 14nm architechture.*/
	AQ_DEVICE_DIONE
} AQ_API_Device;

/*! The table used to compute CRC's within the PHY. */
const uint16_t AQ_CRC16Table[256] = {0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
				     0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
				     0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
				     0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
				     0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
				     0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
				     0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
				     0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
				     0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
				     0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
				     0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
				     0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
				     0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
				     0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
				     0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
				     0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
				     0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
				     0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
				     0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
				     0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
				     0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
				     0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
				     0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
				     0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
				     0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
				     0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
				     0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
				     0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
				     0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
				     0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
				     0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
				     0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0};

struct task_struct *gangload_kthread = NULL;
struct phy_device *gangload_phydevs[MAX_GANGLOAD_DEVICES];
static int gangload = 0;

void AQ_API_EnableMDIO_BootLoadMode
(
	/*! The target PHY port.*/
	struct phy_device *phydev,
	/*! The provisioning address to use when the FW starts and applies the
	* bootloaded image's provisioned values. */
	unsigned int provisioningAddress
)
{
	uint16_t globalNvrProvisioning;
	uint16_t globalReservedProvisioning;

	/* disable the daisy-chain */
	globalNvrProvisioning = phy_read_mmd(phydev, MDIO_MMD_VEND1, VEND1_NVR_PROV3);
	globalNvrProvisioning |= VEND1_NVR_PROV3_DAISYCHAIN_DISABLE;
	phy_write_mmd(phydev, MDIO_MMD_VEND1, VEND1_NVR_PROV3, globalNvrProvisioning);

	/* override the hop-count */
	globalReservedProvisioning = phy_read_mmd(phydev, MDIO_MMD_VEND1, VEND1_RSVD_PROV2);
	globalReservedProvisioning &= ~VEND1_RSVD_PROV2_DAISYCHAIN_HOPCOUNT;
	globalReservedProvisioning |= FIELD_PREP(VEND1_RSVD_PROV2_DAISYCHAIN_HOPCOUNT,
						 provisioningAddress);
	globalReservedProvisioning |= VEND1_RSVD_PROV2_DAISYCHAIN_HOPCOUNT_OVERRIDE;
	phy_write_mmd(phydev, MDIO_MMD_VEND1, VEND1_RSVD_PROV2, globalReservedProvisioning);

	return;
}

/*! Prepare the specified port for MDIO bootloading, and set the temporary MDIO
 * address to be used during the bootload process.  Disables the daisy-chain,
 * and explicitly sets the port's provisioningAddress. */
void AQ_API_EnableGangLoadMode
(
	/*! The target PHY port.*/
	struct phy_device *phydev,
	/*! The PHY's MDIO address will be changed to this value during the
	* bootload process. */
	unsigned int gangLoadAddress
)
{
	uint16_t globalGeneralProvisioning;

	/* Enable gangload mode.  After doing this, the PHY will be
	 * addressable at the MDIO address indicated by gangLoadAddress.
	 * Now that the PHY is in gangload mode, MDIO reads are prohibited
	 * until AQ_API_DisableGangLoadMode is called. */
	globalGeneralProvisioning = phy_read_mmd(phydev, MDIO_MMD_VEND1, VEND1_GENERAL_PROV8);
	globalGeneralProvisioning &= ~VEND1_GENERAL_PROV8_MDIO_BROADCAST_ADDRESS;
	globalGeneralProvisioning |= FIELD_PREP(VEND1_GENERAL_PROV8_MDIO_BROADCAST_ADDRESS,
						gangLoadAddress);
	phy_write_mmd(phydev, MDIO_MMD_VEND1, VEND1_GENERAL_PROV8, globalGeneralProvisioning);

	globalGeneralProvisioning = phy_read_mmd(phydev, MDIO_MMD_VEND1, VEND1_GENERAL_PROV2);
	globalGeneralProvisioning |= VEND1_GENERAL_PROV2_MDIO_BROADCAST_ENABLE;
	phy_write_mmd(phydev, MDIO_MMD_VEND1, VEND1_GENERAL_PROV2, globalGeneralProvisioning);

	return;
}

/*! Restore the PHY's MDIO address to the pin-specified value.  Should be
 * called when MDIO bootloading is complete, to return to normal MDIO
 * addressing.
 * <b>This is a gang-load function, hence write-only!</b> */
void AQ_API_DisableGangLoadMode
(
	/*! The target PHY port.*/
	struct phy_device *phydev,
	/*! The value to write to of AQ_GlobalGeneralProvisioning.u1.word_1. */
	uint16_t origVal_GGP1
)
{
	uint16_t globalGeneralProvisioning;

	/* Restore the original value of globalGeneralProvisioning.u1, and set
	 * the MDIO address reset bit.  This will cause the MDIO address to be
	 * reset to the value indicated by the pins. */
	globalGeneralProvisioning = origVal_GGP1;
	globalGeneralProvisioning &= ~VEND1_GENERAL_PROV2_MDIO_BROADCAST_ENABLE;
	phy_write_mmd(phydev, MDIO_MMD_VEND1, VEND1_GENERAL_PROV2, globalGeneralProvisioning);

	/* The PHY has now exited gang-load mode. */
	return;
}

/* Common implementation of MDIO bootload routine, for the entry points:
 * AQ_API_WriteBootLoadImage
 * AQ_API_WriteBootLoadImageWithProvTable
 * AQ_API_WriteBootLoadImageDRAMOnly
 * AQ_API_WriteBootLoadImageWithProvTableDRAMOnly */
static int AQ_API_WriteBootLoadImage_impl
(
	struct phy_device **phydevs,
	int num_phydevs,
	struct phy_device *gandload_phydev,
	int *result,
	const uint32_t* imageSizePointer,
	const uint8_t* image,
	const uint32_t* provTableSizePointer,
	const uint8_t* provTableImage,
	bool dramOnly
)
{
	uint32_t primaryHeaderPtr = 0x00000000;
	uint32_t primaryIramPtr = 0x00000000;
	uint32_t primaryDramPtr = 0x00000000;
	uint32_t primaryIramSize = 0x00000000;
	uint32_t primaryDramSize = 0x00000000;
	uint32_t terminatorPtr = 0x00000000;
	uint32_t phyImageHeaderContentOffset = 0x00000000;
	uint32_t i, j;
	uint32_t imageSize;
	uint32_t provTableImageSize = 0;
	uint32_t bytePointer;
	uint32_t byteSize;
	uint32_t dWordSize;
#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE
	uint32_t countPendingOps;				/* A count of block MDIO operation pending... necessary to keep a count
								   in order to ensure we don't exceed the maximum pending operations. */
#endif
	uint16_t globalControl;
	uint16_t msw;
	uint16_t lsw;
	uint16_t crc16Calculated;
	uint16_t provTableCrc16Calculated;
	uint16_t fileCRC;
	uint16_t provTableFileCRC;
	uint16_t mailboxCRC;
	uint16_t mailboxWrite;
	uint16_t recordedGGP1Values[AQ_MAX_NUM_PHY_IDS];	/* When entering/exiting gangload mode, we record and restore
								   the AQ_GlobalGeneralProvisioning.u1 register values. */

	/* store the CRC-16 for the image, which is the last two bytes */
	imageSize = *imageSizePointer;

	/*
	 * If the imageSize is less than 2, we don't do anything
	 */
	if (imageSize < 2) {
		result[0] = -EINVAL;
		return -EINVAL;
	}

	fileCRC = image[imageSize-2] << 8 | image[imageSize-1];

	/*------------------------------------- Check the image integrity ------------------------------------------------*/
	crc16Calculated = 0x0000;
	for (i = 0; i < imageSize-2; i++)
	{
		crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ image[i]];
	}

	if (crc16Calculated != fileCRC)
	{
		phydev_err(phydevs[0], "CRC check failed on image file (expected 0x%X, found 0x%X)\n",
			   fileCRC, crc16Calculated);
		result[0] = -EINVAL;
		return -EINVAL;
	}
	else
	{
		phydev_info(phydevs[0], "CRC check good on image file (0x%04X)\n", crc16Calculated);
	}

	/*-------------------------------- Check the provisioning table image integrity ----------------------------------*/
	if (provTableSizePointer != NULL && provTableImage != NULL)
	{
		provTableImageSize = (*provTableSizePointer) - 2;
		provTableFileCRC = provTableImage[provTableImageSize + 1] << 8 |
				   provTableImage[provTableImageSize];

		provTableCrc16Calculated = 0x0000;
		for (i = 0; i < provTableImageSize; i++)
		{
			provTableCrc16Calculated = ((provTableCrc16Calculated & 0xFF) << 8) ^
						   AQ_CRC16Table[(provTableCrc16Calculated >> 8) ^ provTableImage[i]];
		}

		if (provTableCrc16Calculated != provTableFileCRC)
		{
			phydev_err(phydevs[0], "CRC check failed on provisioning table file (expected 0x%X, found 0x%X)\n",
				   provTableFileCRC, provTableCrc16Calculated);
			result[0] = -EINVAL;
			return -EINVAL;
		}
		else
		{
			phydev_info(phydevs[0], "CRC check good on provisioning table file (0x%04X)\n",
				    provTableCrc16Calculated);
		}
	}

	/*--------------------------- Store 1E.C441 values for later use.  Enforce uniformity. ---------------------------*/
	for (j = 0; j < num_phydevs; j++)
	{
		/* Record the original value of AQ_GlobalGeneralProvisioning.u1.word_1,
		 * so that we can restore it later after exiting gangload mode. */
		recordedGGP1Values[j] = phy_read_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_GENERAL_PROV2);

		/* If any of the PHYs' GGP1 values don't match the others, set the appropriate
		 * error code and return. */
		if (j > 0 && recordedGGP1Values[j] != recordedGGP1Values[0])
		{
			phydev_err(phydevs[j], "Non-uniform value of 1E.C441 found (expected 0x%X, found 0x%X)\n",
				   recordedGGP1Values[0], recordedGGP1Values[j]);
			result[j] = -EINVAL;
			return -EINVAL;
		}
	}

	/*--------------------------- Put each PHY into gangload mode at the specified address ---------------------------*/
	for (j = 0; j < num_phydevs; j++) {
		AQ_API_EnableMDIO_BootLoadMode(phydevs[j], 0);
		AQ_API_EnableGangLoadMode(phydevs[j], gandload_phydev->mdio.addr);
	}

	/*------------------------------------- Stall the uP  ------------------------------------------------------------*/
	globalControl = 0x0000;
	globalControl |= VEND1_CONTROL2_UP_RUNSTALL_OVERRIDE;
	globalControl |= VEND1_CONTROL2_UP_RUNSTALL;
	phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_CONTROL2, globalControl);

	/*------------------------------------- Initialize the mailbox write command -------------------------------------*/
	mailboxWrite = 0x0000;
	mailboxWrite |= VEND1_MAILBOX_INTERFACE1_WRITE;
	mailboxWrite |= VEND1_MAILBOX_INTERFACE1_START;

	/*------------------------------------- Read the segment addresses and sizes -------------------------------------*/
	primaryHeaderPtr = (((image[0x9] & 0x0F) << 8) | image[0x8]) << 12;

	/* setup image header content offset for HHD/EUR/CAL/RHEA */
	phyImageHeaderContentOffset = AQ_PHY_IMAGE_HEADER_CONTENT_OFFSET_HHD;

	primaryIramPtr =  (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x4 + 2] << 16) |
			  (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x4 + 1] << 8)  |
			   image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x4];
	primaryIramSize = (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x7 + 2] << 16) |
			  (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x7 + 1] << 8)  |
			   image[primaryHeaderPtr + phyImageHeaderContentOffset + 0x7];
	primaryDramPtr =  (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xA + 2] << 16) |
			  (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xA + 1] << 8)  |
			   image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xA];
	primaryDramSize = (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xD + 2] << 16) |
			  (image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xD + 1] << 8)  |
			   image[primaryHeaderPtr + phyImageHeaderContentOffset + 0xD];

	/* setup primary image pointer for HHD/EUR/CAL/RHEA */
	primaryIramPtr += primaryHeaderPtr;
	primaryDramPtr += primaryHeaderPtr;

	phydev_info(gandload_phydev, "Segment Addresses and Sizes as read from the PHY ROM image header:\n");
	phydev_info(gandload_phydev, "Primary Iram Address: 0x%x\n", primaryIramPtr);
	phydev_info(gandload_phydev, "Primary Iram Size: 0x%x\n", primaryIramSize);
	phydev_info(gandload_phydev, "Primary Dram Address: 0x%x\n", primaryDramPtr);
	phydev_info(gandload_phydev, "Primary Dram Size: 0x%x\n", primaryDramSize);

	/*------------------ Prepare to merge the provisioning table into the main image ---------------------------------*/
	if (provTableSizePointer != NULL && provTableImage != NULL)
	{
		/* Locate the terminator of the built-in provisioning table */
		terminatorPtr = primaryDramPtr +
			       ((image[primaryDramPtr + AQ_PHY_IMAGE_PROVTABLE_TERM_OFFSET + 1] << 8) |
				 image[primaryDramPtr + AQ_PHY_IMAGE_PROVTABLE_TERM_OFFSET]);

		phydev_info(gandload_phydev, "Supplied Provisioning Table At Address: 0x%x\n\n", terminatorPtr);

		/* Check that the supplied provisioning table will fit within the alloted
		 * space. */
		if (terminatorPtr - (primaryDramPtr + AQ_PHY_IMAGE_PROVTABLE_OFFSET) +
		    provTableImageSize > AQ_PHY_IMAGE_PROVTABLE_MAXSIZE)
		{
			result[0] = -EINVAL;
			return -EINVAL;
		}
	}

	/*------------------------------------- Load IRAM and DRAM -------------------------------------------------------*/
	/* clear the mailbox CRC */
	phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, VEND1_MAILBOX_INTERFACE1_RESET_CRC);
	phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, 0x0000);

	crc16Calculated = 0;         /* This is to calculate what was written through the mailbox */

	if (!dramOnly)
	{
		/* load the IRAM */
		phydev_info(gandload_phydev, "Loading IRAM:\n");

		/* dWord align the address: note the image addressing is byte based, but is properly aligned on dWord
		boundaries, so the 2 LSbits of the block start are always zero. */
		msw = (uint16_t) (AQ_IRAM_BASE_ADDRESS >> 16);
		lsw = (AQ_IRAM_BASE_ADDRESS & 0xFFFF) >> 2;
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE3, msw);
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE4, lsw);

		/* set block size so that there are from 0-3 bytes remaining */
		byteSize = primaryIramSize;
		dWordSize = byteSize >> 2;

		bytePointer = primaryIramPtr;
		#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE
		countPendingOps = 0;
		#endif
		for (i = 0; i < dWordSize; i++)
		{
			/* write 4 bytes of data */
			if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
				lsw = provTableImage[bytePointer - terminatorPtr];
			else
				lsw = image[bytePointer];

			if (terminatorPtr && ((bytePointer+1) >= terminatorPtr) && ((bytePointer+1) < terminatorPtr + provTableImageSize))
				lsw |= provTableImage[bytePointer + 1 - terminatorPtr] << 8;
			else
				lsw |= image[bytePointer+1] << 8;

			bytePointer += 2;

			if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
				msw = provTableImage[bytePointer - terminatorPtr];
			else
				msw = image[bytePointer];

			if (terminatorPtr && ((bytePointer+1) >= terminatorPtr) && ((bytePointer+1) < terminatorPtr + provTableImageSize))
				msw |= provTableImage[bytePointer + 1 - terminatorPtr] << 8;
			else
				msw |= image[bytePointer+1] << 8;

			bytePointer += 2;

			#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE5, msw);
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE6, lsw);
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, mailboxWrite);

			countPendingOps += 3;
			/* Check if we've filled our output buffer, and if so, flush. */
			#ifdef AQ_EXTRA_FLAGS
			if (countPendingOps >= AQ_API_MDIO_MaxBlockOperations(0) - 3)
			#else
			if (countPendingOps >= AQ_API_MDIO_MaxBlockOperations() - 3)
			#endif
			{
				AQ_API_MDIO_BlockOperationExecute(gandload_phydev);
				countPendingOps = 0;
			}
			#else
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE5, msw);
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE6, lsw);
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, mailboxWrite);
			#endif

			/* update the calculated CRC */
			crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw >> 8)];
			crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw & 0xFF)];
			crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw >> 8)];
			crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw & 0xFF)];

			#ifdef AQ_VERBOSE
			if (i && ((i % 512) == 0)) phydev_info(gandload_phydev, "    Byte: %X:\n", i << 2);
			#endif
		}

		#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE
		/* flush the output buffer one last time. */
		AQ_API_MDIO_BlockOperationExecute(gandload_phydev);
		countPendingOps = 0;
		#endif

		/* Note: this final write right-justifies non-dWord data in the final dWord */
		switch (byteSize & 0x3)
		{
		case 0x1:
			/* write 1 byte of data */
			if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
				lsw = provTableImage[bytePointer - terminatorPtr];
			else
				lsw = image[bytePointer];

			bytePointer += 1;

			msw = 0x0000;

			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE5, msw);
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE6, lsw);

			/* no polling */
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, mailboxWrite);
			break;

		case 0x2:
			/* write 2 bytes of data */
			if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
				lsw = provTableImage[bytePointer - terminatorPtr];
			else
				lsw = image[bytePointer];

			if (terminatorPtr && ((bytePointer+1) >= terminatorPtr) && ((bytePointer+1) < terminatorPtr + provTableImageSize))
				lsw |= provTableImage[bytePointer + 1 - terminatorPtr] << 8;
			else
				lsw |= image[bytePointer+1] << 8;

			bytePointer += 2;

			msw = 0x0000;

			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE5, msw);
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE6, lsw);

			/* no polling */
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, mailboxWrite);
			break;

		case 0x3:
			/* write 3 bytes of data */
			if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
				lsw = provTableImage[bytePointer - terminatorPtr];
			else
				lsw = image[bytePointer];

			if (terminatorPtr && ((bytePointer+1) >= terminatorPtr) && ((bytePointer+1) < terminatorPtr + provTableImageSize))
				lsw |= provTableImage[bytePointer + 1 - terminatorPtr] << 8;
			else
				lsw |= image[bytePointer+1] << 8;

			bytePointer += 2;

			if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
				msw = provTableImage[bytePointer - terminatorPtr];
			else
				msw = image[bytePointer];

			bytePointer += 1;

			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE5, msw);
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE6, lsw);

			/* no polling */
			phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, mailboxWrite);
			break;
		}

		if (byteSize & 0x3)
		{
			/* update the calculated CRC */
			crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw >> 8)];
			crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw & 0xFF)];
			crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw >> 8)];
			crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw & 0xFF)];
		}

		phydev_info(gandload_phydev, "CRC-16 after loading IRAM: 0x%X\n", crc16Calculated);
	}

	/* load the DRAM */
	phydev_info(gandload_phydev, "Loading DRAM:\n");

	/* dWord align the address: note the image addressing is byte based, but is properly aligned on dWord
	   boundaries, so the 2 LSbits of the block start are always zero. */
	msw = (uint16_t) (AQ_DRAM_BASE_ADDRESS >> 16);
	lsw = (AQ_DRAM_BASE_ADDRESS & 0xFFFF) >> 2;
	phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE3, msw);
	phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE4, lsw);

	/* set block size so that there are from 0-3 bytes remaining */
	byteSize = primaryDramSize;
	dWordSize = byteSize >> 2;

	bytePointer = primaryDramPtr;
#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE
	countPendingOps = 0;
#endif
	for (i = 0; i < dWordSize; i++)
	{
		/* write 4 bytes of data */
		if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
			lsw = provTableImage[bytePointer - terminatorPtr];
		else
			lsw = image[bytePointer];

		if (terminatorPtr && ((bytePointer+1) >= terminatorPtr) && ((bytePointer+1) < terminatorPtr + provTableImageSize))
			lsw |= provTableImage[bytePointer + 1 - terminatorPtr] << 8;
		else
			lsw |= image[bytePointer+1] << 8;

		bytePointer += 2;

		if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
			msw = provTableImage[bytePointer - terminatorPtr];
		else
			msw = image[bytePointer];

		if (terminatorPtr && ((bytePointer+1) >= terminatorPtr) && ((bytePointer+1) < terminatorPtr + provTableImageSize))
			msw |= provTableImage[bytePointer + 1 - terminatorPtr] << 8;
		else
			msw |= image[bytePointer+1] << 8;

		bytePointer += 2;

		#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE5, msw);
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE6, lsw);
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, mailboxWrite);

		countPendingOps += 3;
		/* Check if we've filled our output buffer, and if so, flush. */
		#ifdef AQ_EXTRA_FLAGS
		if (countPendingOps >= AQ_API_MDIO_MaxBlockOperations(0) - 3)
		#else
		if (countPendingOps >= AQ_API_MDIO_MaxBlockOperations() - 3)
		#endif
		{
			AQ_API_MDIO_BlockOperationExecute(gandload_phydev);
			countPendingOps = 0;
		}
		#else
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE5, msw);
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE6, lsw);
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, mailboxWrite);
		#endif

		/* update the calculated CRC */
		crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw >> 8)];
		crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw & 0xFF)];
		crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw >> 8)];
		crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw & 0xFF)];

		#ifdef AQ_VERBOSE
		if (i && ((i % 512) == 0)) phydev_info(gandload_phydev, "    Byte: %X:\n", i << 2);
		#endif
	}

	#ifdef AQ_PHY_SUPPORTS_BLOCK_READ_WRITE
	/* flush the output buffer one last time. */
	AQ_API_MDIO_BlockOperationExecute(gandload_phydev);
	countPendingOps = 0;
	#endif

	/* Note: this final write right-justifies non-dWord data in the final dWord */
	switch (byteSize & 0x3)
	{
	case 0x1:
		/* write 1 byte of data */
		if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
			lsw = provTableImage[bytePointer - terminatorPtr];
		else
			lsw = image[bytePointer];

		bytePointer += 1;

		msw = 0x0000;

		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE5, msw);
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE6, lsw);

		/* no polling */
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, mailboxWrite);
		break;

	case 0x2:
		/* write 2 bytes of data */
		if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
			lsw = provTableImage[bytePointer - terminatorPtr];
		else
			lsw = image[bytePointer];

		if (terminatorPtr && ((bytePointer+1) >= terminatorPtr) && ((bytePointer+1) < terminatorPtr + provTableImageSize))
			lsw |= provTableImage[bytePointer + 1 - terminatorPtr] << 8;
		else
			lsw |= image[bytePointer+1] << 8;

		bytePointer += 2;

		msw = 0x0000;

		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE5, msw);
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE6, lsw);

		/* no polling */
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, mailboxWrite);
		break;

	case 0x3:
		/* write 3 bytes of data */
		if (terminatorPtr && (bytePointer >= terminatorPtr) && (bytePointer < terminatorPtr + provTableImageSize))
			lsw = provTableImage[bytePointer - terminatorPtr];
		else
			lsw = image[bytePointer];

		if (terminatorPtr && ((bytePointer+1) >= terminatorPtr) && ((bytePointer+1) < terminatorPtr + provTableImageSize))
			lsw |= provTableImage[bytePointer + 1 - terminatorPtr] << 8;
		else
			lsw |= image[bytePointer+1] << 8;

		bytePointer += 2;

		msw = image[bytePointer];
		bytePointer += 1;

		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE5, msw);
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE6, lsw);

		/* no polling */
		phy_write_mmd(gandload_phydev, MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE1, mailboxWrite);
		break;
	}

	if (byteSize & 0x3)
	{
		/* update the calculated CRC */
		crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw >> 8)];
		crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (msw & 0xFF)];
		crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw >> 8)];
		crc16Calculated = ((crc16Calculated & 0xFF) << 8) ^ AQ_CRC16Table[(crc16Calculated >> 8) ^ (lsw & 0xFF)];
	}

	/*------------------------------------- Exit gangload mode -------------------------------------------------------*/
	AQ_API_DisableGangLoadMode(gandload_phydev, recordedGGP1Values[0]);

	/*------------------------------------- Check mailbox CRCs -------------------------------------------------------*/
	/* check to make sure the mailbox CRC matches the calculated CRC */
	for (j = 0; j < num_phydevs; j++) {
		mailboxCRC = phy_read_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_MAILBOX_INTERFACE2);
		if (mailboxCRC != crc16Calculated)
		{
			phydev_err(phydevs[j], "%uth port: Mailbox CRC-16 (0x%X) does not match calculated CRC-16 (0x%X)\n",
				j, mailboxCRC, crc16Calculated);
			result[j] = -EIO;
		}
		else
		{
			phydev_info(phydevs[j], "%uth port: Image load good - mailbox CRC-16 matches (0x%X)\n",
				j, mailboxCRC);
		}
	}

	/*------------------------------------- Clear any resets ---------------------------------------------------------*/
	for (j = 0; j < num_phydevs; j++) {
		phy_write_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_STD_CONTROL1, 0x0000);
	}

	/*------------------------------------- Release the uP -----------------------------------------------------------*/
	globalControl = 0x0000;
	globalControl |= VEND1_CONTROL2_UP_RUNSTALL_OVERRIDE;
	globalControl |= VEND1_CONTROL2_UP_RUNSTALL;
	for (j = 0; j < num_phydevs; j++) {
		globalControl &= ~VEND1_CONTROL2_UP_RESET;
		phy_write_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_CONTROL2, globalControl);
		globalControl |= VEND1_CONTROL2_UP_RESET;
		phy_write_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_CONTROL2, globalControl);
	}

	/* Need to wait at least 100us. */
	udelay(100);

	globalControl &= ~VEND1_CONTROL2_UP_RESET;
	globalControl &= ~VEND1_CONTROL2_UP_RUNSTALL;
	/* For post-APPIA devices, always set the uP stall override bit to
	 * smooth over any packaging differences WRT the boot load pin. */
	/* REGDOC: Assign to local representation of bitfield (HHD/APPIA/EUR/CAL/RHEA: 1E.C001.6) */
	globalControl |= VEND1_CONTROL2_UP_RUNSTALL_OVERRIDE;

	for (j = 0; j < num_phydevs; j++) {
		phy_write_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_CONTROL2, globalControl);
	}

	/* NOTE!!! We can't re-enable the daisy-chain here, as this will overwrite the IRAM and DRAM with the FLASH contents*/

	/* If any of the ports was not bootloaded successfully, return AQ_RET_ERROR */
	for (j = 0; j < num_phydevs; j++) {
		if (result[j] != 0)
			return -EIO;
	}

	/* All ports were bootloaded successfully. */
	return 0;
}

static int AQ_API_WriteBootLoadImage(
	struct phy_device **phydevs,
	int num_phydevs,
	struct phy_device *gandload_phydev,
	int *result,
	const uint8_t* data,
	size_t size)
{
	unsigned int val;
	int j;

	for (j = 0; j < num_phydevs; j++) {
		/* disable the S/W reset to the Global MMD registers */
		val = phy_read_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_RESET_CONTROL);
		val |= VEND1_RESET_CONTROL_MMD_RESET_DISABLE;
		phy_write_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_RESET_CONTROL, val);

		/* assert Global S/W reset */
		val = phy_read_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_STD_CONTROL1);
		val |= VEND1_STD_CONTROL1_SOFT_RESET;
		phy_write_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_STD_CONTROL1, val);

		/* de-assert Global S/W reset */
		val = phy_read_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_STD_CONTROL1);
		val &= ~VEND1_STD_CONTROL1_SOFT_RESET;
		phy_write_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_STD_CONTROL1, val);

		/* wait 100ms */
		mdelay(100);

		/* enable the S/W reset to the Global MMD registers */
		val = phy_read_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_RESET_CONTROL);
		val &= ~VEND1_RESET_CONTROL_MMD_RESET_DISABLE;
		phy_write_mmd(phydevs[j], MDIO_MMD_VEND1, VEND1_RESET_CONTROL, val);
	}

	return AQ_API_WriteBootLoadImage_impl(phydevs, num_phydevs, gandload_phydev,
					      result, (const uint32_t *)&size, data,
					      NULL, NULL, 0);
}

static void aqr_firmware_download_cb(const struct firmware *fw, void *context)
{
	struct phy_device **phydevs = context;
	struct phy_device *gandload_phydev = phydevs[0];
	struct device *dev;
	struct aqr107_priv *priv;
	int result[MAX_GANGLOAD_DEVICES];
	int i, ret = 0;

	if (!fw)
		return;

retry:
	memset(result, 0, sizeof(result));

	ret = AQ_API_WriteBootLoadImage(phydevs, MAX_GANGLOAD_DEVICES, gandload_phydev,
					result, fw->data, fw->size);
	if (ret) {
		for (i = 0; i < MAX_GANGLOAD_DEVICES; i++) {
			if (result[i] == 0)
				continue;

			dev = &phydevs[i]->mdio.dev;
			dev_err(dev, "failed to download firmware %s, ret: %d\n",
				AQR_FIRMWARE, ret);
			goto retry;
		}
	}

#ifdef CONFIG_AQUANTIA_PHY_MDI_SWAP
	mdelay(250);
#endif
	for (i = 0; i < MAX_GANGLOAD_DEVICES; i++) {
		if (result[i] == 0) {
			priv = phydevs[i]->priv;
			priv->fw_initialized = true;
#ifdef CONFIG_AQUANTIA_PHY_MDI_SWAP
			aqr107_config_mdi(phydevs[i]);
#endif
		}
	}

	release_firmware(fw);
}

int aqr_firmware_download_single(struct phy_device *phydev)
{
	struct aqr107_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;
	int ret = 0;

	if (priv->fw_initialized == true)
		return 0;

	priv->phydevs[0] = phydev;

	ret = request_firmware_nowait(THIS_MODULE, true, AQR_FIRMWARE, dev,
				      GFP_KERNEL, priv->phydevs, aqr_firmware_download_cb);
	if (ret) {
		dev_err(dev, "failed to load firmware %s, ret: %d\n",
			AQR_FIRMWARE, ret);
	}

	return ret;
}

static int aqr_firmware_gandload_thread(void *data)
{
	struct phy_device **phydevs = data;
	struct device *dev = &phydevs[0]->mdio.dev;
	int ret = 0;

	for (;;) {
		if (kthread_should_stop())
			break;

		/* reach maximum gangload phy devices */
		if (gangload == MAX_GANGLOAD_DEVICES) {
			ret = request_firmware_nowait(THIS_MODULE, true, AQR_FIRMWARE, dev,
						      GFP_KERNEL, phydevs, aqr_firmware_download_cb);
			if (ret) {
				dev_err(dev, "failed to load firmware %s, ret: %d\n",
					AQR_FIRMWARE, ret);
			}
			break;
		}

		set_current_state(TASK_INTERRUPTIBLE);
		msleep(1);
	}

	return ret;
}

static int aqr_firmware_download_gang(struct phy_device *phydev)
{
	struct aqr107_priv *priv = phydev->priv;
	struct device *dev = &phydev->mdio.dev;

	if (priv->fw_initialized == true)
		return 0;

	if (!gangload_kthread) {
		/* create a thread for monitor gangload devices */
		gangload_kthread = kthread_create(aqr_firmware_gandload_thread,
						  gangload_phydevs,
						  "aqr_firmware_gandload_thread");
		if (IS_ERR(gangload_kthread)) {
			dev_err(dev,
				"%s Failed to create thread for aqr_firmware_gandload_thread\n",
				__func__);
			return PTR_ERR(gangload_kthread);
		}
		wake_up_process(gangload_kthread);
	}

	gangload_phydevs[gangload] = phydev;
	gangload++;

	return 0;
}

int aqr_firmware_download(struct phy_device *phydev)
{
	int ret = 0;
#ifdef CONFIG_AQUANTIA_PHY_FW_DOWNLOAD_SINGLE
	ret = aqr_firmware_download_single(phydev);
#elif CONFIG_AQUANTIA_PHY_FW_DOWNLOAD_GANG
	ret = aqr_firmware_download_gang(phydev);
#endif
	return ret;
}
