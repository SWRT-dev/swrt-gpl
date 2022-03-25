/*
 *
 * FILE NAME	: ltq_system_reset.h
 * PROJECT		: grx500
 * MODULES		: SYSRST (System Reset Driver)
 *
 * DESCRIPTION	: Global System Reset Driver header file
 * COPYRIGHT	:		Copyright (c) 2014
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * HISTORY
 */

#ifndef SYSTEM_RESET_H
#define SYSTEM_RESET_H

#define SYSRST_AFFTECTED_DOMAIN_MAX 4
#define SYSRST_RCU_AFFTECTED_DOMAIN_MAX 8
#define SYSRST_NGI_AFFTECTED_DOMAIN_MAX 8

#define SYSRST_DOMAIN_INVALID_ID 0xff

enum {
	/* TOE */
	LTQ_NGI_DOMAIN_TA_TOE,
	/* MPE */
	LTQ_NGI_DOMAIN_TA_MPE,
	/* MemMax */
	LTQ_NGI_DOMAIN_TA_DDR,
	/* CBM */
	LTQ_NGI_DOMAIN_TA_CMB1,
	LTQ_NGI_DOMAIN_TA_CMB2,
	/* DMA4 */
	LTQ_NGI_DOMAIN_TA_DMA4,
	LTQ_NGI_DOMAIN_IA_DMA4,
	/* DMA3 */
	LTQ_NGI_DOMAIN_TA_DMA3,
	LTQ_NGI_DOMAIN_IA_DMA3,
	/* EIP-123 */
	LTQ_NGI_DOMAIN_TA_EIP_123,
	/* EIP-97 */
	LTQ_NGI_DOMAIN_TA_EIP_97,
	/* EX05 */
	LTQ_NGI_DOMAIN_TA_EX05,
	/* OCP2OTP */
	LTQ_NGI_DOMAIN_TA_OCP2OTP,
	/* I2C */
	LTQ_NGI_DOMAIN_TA_I2C,
	/* UART0 */
	LTQ_NGI_DOMAIN_TA_UART0,
	/* UART0 */
	LTQ_NGI_DOMAIN_TA_UART1,
	/* GPTC0 */
	LTQ_NGI_DOMAIN_TA_GPTC0,
	LTQ_NGI_DOMAIN_TA_GPTC1,
	LTQ_NGI_DOMAIN_TA_GPTC2,
	/* SPI0 */
	LTQ_NGI_DOMAIN_TA_SPI0,
	/* SPI1 */
	LTQ_NGI_DOMAIN_TA_SPI1,
	/* DMA0 */
	LTQ_NGI_DOMAIN_TA_DMA0,
	LTQ_NGI_DOMAIN_IA_DMA0,
	/* EBU */
	LTQ_NGI_DOMAIN_TA_EBU,
	/* HSNAND */
	LTQ_NGI_DOMAIN_TA_HSNAND,
	/* DMA2 TX */
	LTQ_NGI_DOMAIN_TA_DMA2_TX,
	LTQ_NGI_DOMAIN_IA_DMA2_TX,
	/* DMA2 RX */
	LTQ_NGI_DOMAIN_TA_DMA2_RX,
	LTQ_NGI_DOMAIN_IA_DMA2_RX,
	/* GSWIP L */
	LTQ_NGI_DOMAIN_TA_GSWIP_L,
	/* DMA1 TX */
	LTQ_NGI_DOMAIN_TA_DMA1_TX,
	LTQ_NGI_DOMAIN_IA_DMA1_TX,
	/* DMA1 RX */
	LTQ_NGI_DOMAIN_TA_DMA1_RX,
	LTQ_NGI_DOMAIN_IA_DMA1_RX,
	/* GSWIP R */
	LTQ_NGI_DOMAIN_TA_GSWIP_R,
	/* PCIE1 */
	LTQ_NGI_DOMAIN_TA_PCIE1,
	LTQ_NGI_DOMAIN_TA_PCIE1_A,
	LTQ_NGI_DOMAIN_TA_PCIE1_C,
	LTQ_NGI_DOMAIN_IA_PCIE1_W,
	LTQ_NGI_DOMAIN_IA_PCIE1_R,
	/* PCIE2 */
	LTQ_NGI_DOMAIN_TA_PCIE2,
	LTQ_NGI_DOMAIN_TA_PCIE2_A,
	LTQ_NGI_DOMAIN_TA_PCIE2_C,
	LTQ_NGI_DOMAIN_IA_PCIE2_W,
	LTQ_NGI_DOMAIN_IA_PCIE2_R,
	/* PCIE3 */
	LTQ_NGI_DOMAIN_TA_PCIE3,
	LTQ_NGI_DOMAIN_TA_PCIE3_A,
	LTQ_NGI_DOMAIN_TA_PCIE3_C,
	LTQ_NGI_DOMAIN_IA_PCIE3_W,
	LTQ_NGI_DOMAIN_IA_PCIE3_R,
	LTQ_NGI_MAX
};

#define LTQ_NGI_DECLARE_NAME(var)\
	char *(var)[] = {\
	"TA_TOE",\
	"TA_MPE",\
	"TA_DDR",\
	"TA_CMB1",\
	"TA_CBM2",\
	"TA_DMA4",\
	"IA_DMA4",\
	"TA_DMA3",\
	"IA_DMA3",\
	"EIP-123",\
	"EIP-97",\
	"EX05",\
	"OCP2OTP",\
	"TA_I2C",\
	"TA_UART0",\
	"TA_UART1",\
	"TA_GPTC0",\
	"TA_GPTC1",\
	"TA_GPTC2",\
	"TA_SPI0",\
	"TA_SPI1",\
	"TA_DMA0",\
	"IA_DMA0",\
	"TA_EBU",\
	"TA_HSNAND",\
	"TA_DMA2_TX",\
	"IA_DMA2_TX",\
	"TA_DMA2_RX",\
	"IA_DMA2_RX",\
	"TA_GSWIP_L",\
	"TA_DMA1_TX",\
	"IA_DMA1_TX",\
	"TA_DMA1_RX",\
	"IA_DMA1_RX",\
	"TA_GSWIP_R",\
	"TA_PCIE1",\
	"TA_PCIE1_A",\
	"TA_PCIE1_C",\
	"IA_PCIE1_W",\
	"IA_PCIE1_R",\
	"TA_PCIE2",\
	"TA_PCIE2_A",\
	"TA_PCIE2_C",\
	"IA_PCIE2_W",\
	"IA_PCIE2_R",\
	"TA_PCIE3",\
	"TA_PCIE3_A",\
	"TA_PCIE3_C",\
	"IA_PCIE3_W",\
	"IA_PCIE3_R",\
}

enum {
	SYSRST_DOMAIN_ONBOARD_PERIPHERAL = 0,
	SYSRST_DOMAIN_TEP,	/* Boot core, MIPS4kec */
	SYSRST_DOMAIN_CPS_SUBSYSTEM, /* CPS Sub System */
	SYSRST_DOMAIN_TOE,/* TCP Offload Engine */
	SYSRST_DOMAIN_TMU,	/* Traffic Management Unit */
	SYSRST_DOMAIN_MPE,	/* Multi-core Processing Engine */
	SYSRST_DOMAIN_PCIE_PHY3,/* PCIe PHY3 */
	SYSRST_DOMAIN_PCIE_PHY1,/* PCIe PHY1 */
	SYSRST_DOMAIN_PCIE_PHY2,/* PCIe PHY2 */
	SYSRST_DOMAIN_XBAR6,	/* Crossbar 6 */
	SYSRST_DOMAIN_GSWIP_L,	/* GSWIP_L */
	SYSRST_DOMAIN_WLAN,		/* WLAN */
	SYSRST_DOMAIN_PHY5,		/* PHY5 reset when set 1 */
	SYSRST_DOMAIN_PHY4,		/* PHY4 reset when set 1 */
	SYSRST_DOMAIN_PHY3,		/* PHY3 reset when set 1 */
	SYSRST_DOMAIN_PHY2,		/* PHY2 reset when set 1 */
	SYSRST_DOMAIN_GLB_SW,	/* Global software reset */
	SYSRST_DOMAIN_PHY6F,	/* GPHY6 reset when set 1 */
	SYSRST_DOMAIN_VCODEC,	/* Voice Codec */
	SYSRST_DOMAIN_OCP2SRAM,	/* OCP2SRAM Bridge */
	SYSRST_DOMAIN_XBAR7,	/* Crossbar 7 */
	SYSRST_DOMAIN_SL40,		/* SL40 */
	SYSRST_DOMAIN_USB_PHY0,	/* USB_PHY0 */
	SYSRST_DOMAIN_USB_PHY1,	/* USB_PHY1 */
	SYSRST_DOMAIN_SR_PHYF,	/* Soft GPHY6 */
	SYSRST_DOMAIN_SR_PHY2,	/* Soft PHY2 */
	SYSRST_DOMAIN_SR_PHY3,	/* Soft PHY3 */
	SYSRST_DOMAIN_TEMPS,	/* Temperature sensor reset */
	SYSRST_DOMAIN_SR_PHY5,	/* Soft PHY5 */
	SYSRST_DOMAIN_SR_PHY4,	/* Soft PHY4 */
	SYSRST_DOMAIN_XBAR3,	/* Crossbar 3 */
	SYSRST_DOMAIN_XBAR2,	/* Crossbar 2 */
	SYSRST_DOMAIN_XBAR1,	/* Crossbar 1 */
	SYSRST_DOMAIN_XBAR0,	/* Crossbar 0 */
	SYSRST_DOMAIN_GSWIP_R,	/* GSWIP_R */
	SYSRST_DOMAIN_E123_ABORT_REQ,/* EIP123 reset/abort request */
	SYSRST_DOMAIN_E123_ABORT_ACK,/* EIP123 to reset/abort */
	SYSRST_DOMAIN_GPHY_CDB,	/* GPHY CDB */
	SYSRST_DOMAIN_DDR_CTL,	/* DDR Controller */
	SYSRST_DOMAIN_DDR_PUB,	/* DDR PUB */
	SYSRST_DOMAIN_DDR_PHY,	/* DDR PHY */
	SYSRST_DOMAIN_MEMMAX,	/* Memory max */
	SYSRST_DOMAIN_CBM,	/* Central Buffer Management */
	SYSRST_DOMAIN_DMA4,
	SYSRST_DOMAIN_DMA3,
	SYSRST_DOMAIN_EIP123,
	SYSRST_DOMAIN_EIP97,
	SYSRST_DOMAIN_EX05,
	SYSRST_DOMAIN_OCP2OTP,
	SYSRST_DOMAIN_I2C,
	SYSRST_DOMAIN_UART0,
	SYSRST_DOMAIN_UART1,
	SYSRST_DOMAIN_GPTC0,
	SYSRST_DOMAIN_GPTC1,
	SYSRST_DOMAIN_GPTC2,
	SYSRST_DOMAIN_SPI0,
	SYSRST_DOMAIN_SPI1,
	SYSRST_DOMAIN_DMA0,
	SYSRST_DOMAIN_EBU,
	SYSRST_DOMAIN_HSNAND,
	SYSRST_DOMAIN_DMA2TX,
	SYSRST_DOMAIN_DMA2RX,
	SYSRST_DOMAIN_DMA1TX,
	SYSRST_DOMAIN_DMA1RX,
	SYSRST_DOMAIN_PCIE0,
	SYSRST_DOMAIN_PCIE1,
	SYSRST_DOMAIN_PCIE2,
	SYSRST_DOMAIN_CPU_CLUSTER_RST,
	SYSRST_DOMAIN_CPU_CLUSTER_RLS,
	/* extension for prx300 */
	SYSRST_DOMAIN_GPHY,
	SYSRST_DOMAIN_PONIP,
	SYSRST_DOMAIN_HYST0,
	SYSRST_DOMAIN_PCIE_CTRL1,
	SYSRST_DOMAIN_PCIE_CTRL0,
	SYSRST_DOMAIN_GSWIP,
	SYSRST_DOMAIN_CPHYSS,
	SYSRST_DOMAIN_WANSS,
	SYSRST_DOMAIN_XPCS2,
	SYSRST_DOMAIN_XPCS1,
	SYSRST_DOMAIN_XPCS0,
	SYSRST_DOMAIN_PHY1,
	SYSRST_DOMAIN_PHY0,
	SYSRST_DOMAIN_QSPI,
	SYSRST_DOMAIN_EPMSI,
	SYSRST_DOMAIN_DDR_APB,
	SYSRST_DOMAIN_GPHY_PWR_DOWN,
	SYSRST_DOMAIN_ACASL,
	SYSRST_DOMAIN_HOSTIF,
	SYSRST_DOMAIN_ACADMA,
	SYSRST_DOMAIN_SPIDBG,
	SYSRST_DOMAIN_SR_GPHY,
	SYSRST_DOMAIN_SSX4,
	SYSRST_DOMAIN_OTP,

	/* add more component in the future */
	SYSRST_DOMAIN_MAX,
};

#define SYSRST_DECLARE_DOMAIN_NAME(var)\
	char *(var)[] = {\
	"HRST",\
	"TEP",\
	"CPS_SYSTEM",\
	"TOE",\
	"TMU",\
	"MPE",\
	"PCIE_PHY3",\
	"PCIE_PHY1",\
	"PCIE_PHY2",\
	"XBAR6",\
	"GSWIP_L",\
	"WLAN",\
	"PHY5",\
	"PHY4",\
	"PHY3",\
	"PHY2",\
	"GLB_SW",\
	"PHY6F",\
	"VCODEC",\
	"OCP2SRAM",\
	"XBAR7",\
	"SL40",\
	"USB_PHY0",\
	"USB_PHY1",\
	"SR_PHYF",\
	"SR_PHY2",\
	"SR_PHY3",\
	"TEMPS",\
	"SR_PHY5",\
	"SR_PHY4",\
	"XBAR3",\
	"XBAR2",\
	"XBAR1",\
	"XBAR0",\
	"GSWIP_R",\
	"E123_ABORT_REQ",\
	"E123_ABORT_ACK",\
	"GPHY_CDB",\
	"DDR_CTL",\
	"DDR_PUB",\
	"DDR_PHY",\
	"MemMax",\
	"CBM",\
	"DMA4",\
	"DMA3",\
	"EIP_123",\
	"EIP_97",\
	"EX05",\
	"OCP2OTP",\
	"I2C",\
	"UART0",\
	"UART1",\
	"GPTC0",\
	"GPTC1",\
	"GPTC2",\
	"SPI0",\
	"SPI1",\
	"DMA0",\
	"EBU",\
	"HSNAND",\
	"DMA2TX",\
	"DMA2RX",\
	"DMA1TX",\
	"DMA1RX",\
	"PCIE0",\
	"PCIE1",\
	"PCIE2",\
	"CPU_CLUSTER_RST",\
	"CPU_CLUSTER_RLS",\
	"GPHY",\
	"PONIP",\
	"HYST0",\
	"PCIE_CTRL1",\
	"PCIE_CTRL0",\
	"GSWIP",\
	"CPHYSS",\
	"WANSS",\
	"XPCS2",\
	"XPCS1",\
	"XPCS0",\
	"PHY1",\
	"PHY0",\
	"QSPI",\
	"EPMSI",\
	"DDR_APB",\
	"GPHY_PWR_DOWN",\
	"ACASL",\
	"HOSTIF",\
	"ACADMA",\
	"SPIDBG",\
	"SR_GPHY",\
	"SSX4",\
	"OTP",\
}

enum sysrst_rst_event_t {
	SYSRST_EVENT_PRE_RESET = 0,
	SYSRST_EVENT_POST_RESET = 1,
};

enum {
	SYSRST_MODULE_USB,
	SYSRST_MODULE_ETH,
	SYSRST_MODULE_ATM,
	SYSRST_MODULE_PTM,
	SYSRST_MODULE_PPA,
	SYSRST_MODULE_DMA,
	SYSRST_MODULE_SDIO,
	SYSRST_MODULE_MEI,
	SYSRST_MODULE_TAPI,
	SYSRST_MODULE_PCI,
	SYSRST_MODULE_NAND,
	/* add more component in the future */
	SYSRST_MODULE_MAX,
};

#define SYSRST_DECLARE_MODULE_NAME(var)\
	char *(var)[] = {\
	"USB",\
	"ETH",\
	"ATM",\
	"PTM",\
	"PPA",\
	"DMA",\
	"SDIO",\
	"MEI",\
	"TAPI",\
	"PCI",\
	"NAND",\
	}

typedef int32_t (*sysrst_rst_handler_t)(
	unsigned int reset_domain_id,
	unsigned int module_id,
	enum sysrst_rst_event_t event,
	unsigned int arg
	);
typedef int32_t (*sysrst_rst_async_handler_t)(
	int32_t rst_status,
	uint32_t arg
	);
/*
 * ####################################
 *				Data Type
 * ####################################
 */
struct sysrst_handler_t {
	struct sysrst_handler_t *next;
	sysrst_rst_handler_t	   fn;
	enum sysrst_rst_event_t		   event;
	unsigned int			   arg;
	unsigned int			   module_id;
};

struct sysrst_domain_t {
/* domains affected by reset to this domain (inclusive) */
unsigned char affected_domains[SYSRST_AFFTECTED_DOMAIN_MAX];
unsigned char affected_ngi_domains[SYSRST_NGI_AFFTECTED_DOMAIN_MAX];
unsigned char affected_rcu_domains[SYSRST_RCU_AFFTECTED_DOMAIN_MAX];
struct sysrst_handler_t *handlers;
};

/*
 * ####################################
 *				  IOCTL
 * ####################################
 */
struct sysrst_ioctl_version {
	unsigned int	major;	/*!< output, major number of driver */
	unsigned int	mid;	/*!< output, mid number of driver */
	unsigned int	minor;	/*!< output, minor number of driver */
};

/*!
  \struct sysrst_ioctl_query_rst_domain
  \brief Structure used to get reset status of given domain module ID.
 */
struct sysrst_ioctl_query_rst_domain {
	unsigned int	domain_id;	/*!< input, domain ID */
	unsigned int	module_id;	/*!< input, module ID */
	unsigned int	flags;		/*!< input, flags */
	int	f_reset;	/*!< output, reset status */
};

#define SYSRST_IOC_MAGIC			   0xe0
/*!
  \def SYSRST_IOC_VERSION
  \brief RCU IOCTL Command - Get driver version number.

   This command uses struct "sysrst_ioctl_version"
   as parameter to RCU driver version number.
 */
#define SYSRST_IOC_VERSION _IOR(\
	SYSRST_IOC_MAGIC,\
	0,\
	struct sysrst_ioctl_version)
/*
  \def SYSRST_IOC_QUERY_RST_DOMAIN
  \brief RCU IOCTL Command - Get reset status of given hardware module.

   This command uses struct "sysrst_ioctl_query_rst_domain"
   as parameter to get reset status of given hardware module.
 */
#define SYSRST_IOC_QUERY_RST_DOMAIN _IOWR(\
	SYSRST_IOC_MAGIC,\
	1,\
	struct sysrst_ioctl_query_rst_domain)

/*!
  \def SYSRST_IOC_DO_RST_DOMAIN
  \brief RCU IOCTL Command - Get reset status of given domain module.

   This command uses struct "sysrst_ioctl_query_rst_domain"
   as parameter to get reset status of given hardware module.
 */
#define SYSRST_IOC_DO_RST_DOMAIN _IOWR(\
	SYSRST_IOC_MAGIC,\
	2,\
	struct sysrst_ioctl_query_rst_domain)
/*
 * ####################################
 *				   Exported API
 * ####################################
 */

#ifdef __KERNEL__
/*
 *	Reset Operations
 */
int sysrst_register(unsigned int reset_domain_id, unsigned int module_id,
		    sysrst_rst_handler_t callbackfn, unsigned long arg);
int sysrst_free(unsigned int reset_domain_id, unsigned int module_id);
int sysrst_rst(unsigned int reset_domain_id, unsigned int module_id,
	       unsigned int flags);
void sysrst_clrstatus(unsigned int reset_domain_id);
int sysrst_rststatus(unsigned int reset_domain_id);
int sysrst_rst_async(unsigned int reset_domain_id, unsigned int module_id,
		     sysrst_rst_async_handler_t rst_done, unsigned int arg,
		     unsigned int flags);
#endif

#endif	/* SYSTEM_RESET_H */
