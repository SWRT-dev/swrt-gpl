/******************************************************************************
 *                Copyright (c) 2016, 2017 Intel Corporation
 *
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 ******************************************************************************/

#ifndef _ADAP_OPS_H_
#define _ADAP_OPS_H_

#include "gsw_types.h"
#include "lantiq_gsw.h"


typedef enum {
	GSWSS_REG_WR = 0,
	GSWSS_REG_RD,
	GSWSS_MAC_RESET,
	GSWSS_MAC_EN,
	GSWSS_MAC_IF,
	GSWSS_MAC_OP,
	GSWSS_MAC_MTU,
	GSWSS_MAC_TXTSTAMP_FIFO,
	GSWSS_MAC_PHY2MODE,
	GSWSS_MAX_MAC,
	GSWSS_ADAP_INT,
	GSWSS_ADAP_CFG_1588,
	GSWSS_ADAP_NCO,
	GSWSS_ADAP_MACSEC_RST,
	GSWSS_ADAP_SS_RST,
	GSWSS_ADAP_MACSEC_TO_MAC,
	GSWSS_ADAP_CORESE,
	GSWSS_ADAP_CLK_MD,
	GSWSS_ADAP_1588_CFG1,
	GSWSS_MAX_ADAP,
} GSWSS_CLI_CMDS;

struct adap_ops {
	/* This function does the whole GSWIP Subsystem Reset.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * return		OUT  0:	Reset Succesfully
	 * return		OUT  !0:	GSWIP Subsystem Reset Error
	 */
	int(*ss_hwreset)(void *);
	/* This function Sets the Master Time Source for REF_TIME, DIG_TIME,
	 * BIN_TIME, PPS_SEL.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * param[in/out]	IN:	ref_time	Selects the Master
	 *  0 -	PON_PCS is the MASTER, 1 - PCIE0 is the master,
	 *  2 - PCIE1 is the master 3 - XGMAC2 is the master,
	 *  4 - XGMAC3 is the master, 5 - XGMAC4 is the master
	 * param[in/out]	IN:	dig_time
	 *  0 -	PON_PCS is the MASTER, 1 - PCIE0 is the master,
	 *  2 - PCIE1 is the master 3 - XGMAC2 is the master,
	 *  4 - XGMAC3 is the master, 5 - XGMAC4 is the master
	 * param[in/out]	IN:	bin_time
	 *  0 -	PON_PCS is the MASTER, 1 - PCIE0 is the master,
	 *  2 - PCIE1 is the master 3 - XGMAC2 is the master,
	 *  4 - XGMAC3 is the master, 5 - XGMAC4 is the master
	 * param[in/out]	IN:	pps_sel
	 *  0 -	PON_PCS is the MASTER, 1 - PCIE0 is the master,
	 *  2 - PCIE1 is the master 3 - XGMAC2 is the master,
	 *  4 - XGMAC3 is the master, 5 - XGMAC4 is the master
	 * return		OUT  0:	Master Set Succesfully
	 * return		OUT  !0:Error in Setting Master Time Source
	 */
	int(*ss_set_cfg0_1588)(void *, u32, u32, u32, u32);
	/* This function Sets the Master Time Source for REF_TIME, DIG_TIME,
	 * BIN_TIME, PPS_SEL.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * param[in/out]  OUT:	ref_time	Gets the Master for REF_TIME.
	 *  0 -	PON_PCS is the MASTER, 1 - PCIE0 is the master,
	 *  2 - PCIE1 is the master 3 - XGMAC2 is the master,
	 *  4 - XGMAC3 is the master, 5 - XGMAC4 is the master
	 * param[in/out]  OUT:	dig_time	Gets the Master for DIG_TIME.
	 *  0 -	PON_PCS is the MASTER, 1 - PCIE0 is the master,
	 *  2 - PCIE1 is the master 3 - XGMAC2 is the master,
	 *  4 - XGMAC3 is the master, 5 - XGMAC4 is the master
	 * param[in/out]  OUT:	bin_time	Gets the Master for BIN_TIME.
	 *  0 -	PON_PCS is the MASTER, 1 - PCIE0 is the master,
	 *  2 - PCIE1 is the master 3 - XGMAC2 is the master,
	 *  4 - XGMAC3 is the master, 5 - XGMAC4 is the master
	 * param[in/out]  OUT:	pps_sel		Gets the Master for PPS_SEL.
	 *  0 -	PON_PCS is the MASTER, 1 - PCIE0 is the master,
	 *  2 - PCIE1 is the master 3 - XGMAC2 is the master,
	 *  4 - XGMAC3 is the master, 5 - XGMAC4 is the master
	 * return		OUT  0:	Master Got Succesfully
	 * return		OUT  !0:Error in Getting Master Time Source
	 */
	int(*ss_get_cfg0_1588)(void *, u32 *, u32 *, u32 *, u32 *);
	/* This function Sets the GSWIP Clock Mode to NC01, NCO2 or Auto Mode.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * param[in/out]  IN:	Clk Mode	Selects the Clock Mode.
	 *  0 -	NCO1 - default 666 Mhz, 1 - NCO2 - default 450 Mhz,
	 *  2 - Auto Mode (666/450) Mhz, 3 - Auto Mode (666/450) Mhz
	 * return		   OUT  0:	Clock Mode Set Succesfully
	 * return		   OUT  !0:	Error in Setting Clock Mode
	 */
	int(*ss_set_clkmode)(void *, u32);
	/* This function Gets the GSWIP Clock Mode.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * return		OUT:	Clk Mode	Gets the Clock Mode.
	 *  0 -	NCO1 - default 666 Mhz, 1 - NCO2 - default 450 Mhz,
	 *  2 - Auto Mode (666/450) Mhz, 3 - Auto Mode (666/450) Mhz
	 * return		OUT  -1:	Error in Getting Clock Mode
	 */
	int(*ss_get_clkmode)(void *);
	/* This function does the Switch Core Enable/Disable.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * param[in/out]	IN:	Core Enable	Selects the Core Enable
	 *				1 - Enable, 0 - Disable.
	 * return		OUT  0:	Switch Core Enable/Disable Successfully
	 * return		OUT  !0:Switch Core Enable/Disable Error
	 */
	int(*ss_core_en)(void *, u32);
	/* This function Gets the Switch Core Enable/Disable.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * return		OUT:	core_se	Gets the CORE_SE val.
	 *  			0:	Switch Core Disable,
	 *  			1:	Switch Core Enable
	 */
	int(*ss_get_core_en)(void *);
	/* This function Sets MACSEC to a Mac Module Attachment.
	 *  param[in/out]IN:	ops Adaption ops Struct registered.
	 * param[in/out]IN:	Mac Index	0 - MAC2 is attached to MACSEC,
	 * 	 				1 - MAC3 is attached to MACSEC,
	 * 					2 - MAC4 is attached to MACSEC.
	 * param[in/out]IN:	Enable		1 - Enable MACSEC to MAC Mapping
	 * 	 			0 - Disable MACSEC to MAC Mapping,
	 * return	OUT  0:	MACSEC to Mac Module Attachment done
	 * return	OUT  !0:Error in MACSEC to Mac Module Attachment
	 */
	int(*ss_set_macsec_mac)(void *, u32, u32);
	/* This function Gets MACSEC to a Mac Module mapping.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * \return		OUT:	0 - Disabled MACSEC to MAC mapping,
	 * 	 			2 - MAC2 is attached to MACSEC,
	 * 	 			3 - MAC3 is attached to MACSEC,
	 * 	 			4 - MAC4 is attached to MACSEC
	 * return		OUT  -1:Error in MACSEC to Mac Module Attachment
	 */
	int(*ss_get_macsec_mac)(void *);
	/* This function does MACsec Hardware Reset.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * param[in/out]	IN:  	0	OFF reset is off
	 *				1	ON reset is active
	 * return		OUT  0:		MACsec Hardware Reset done
	 * return		OUT  !0:	MACsec Hardware Reset Error
	 */
	int(*ss_reset_macsec)(void *, u32);
	/* This function Sets NCO value.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * param[in/out]	IN:  val	NCO value to set
	 * 			IN:  nco idx	NCO Index to set (0/1/2/3/4)
	 * return		OUT  0:		NCO value Set Successfully
	 * return		OUT  !0:	NCO value Set Error
	 */
	int(*ss_set_nco)(void *, u32, u32);
	/* This function Gets NCO value.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * param[in/out]	IN:	nco idx	NCO Value to Get from which
	 *				Index (0/1/2/3/4)
	 * return		OUT  0:	NCO value Configured
	 * return		OUT  -1:NCO value Get Error
	 */
	int(*ss_get_nco)(void *, u32);
	/* This function Enables/Disbales Interrupt Enable Register.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * param[in/out]	IN:	module	0 - XGMAC
	 *					4 - LINK
	 * param[in/out]	IN:	idx	XGMAC Index enabled/disabled
	 * param[in/out]	IN:	enable	1 - Enable, 0 - Disable
	 * return		OUT	0:	Interrupt Enable Register Set
	 *					Successfully
	 * return		OUT	!0:	Interrupt Enable Register Set
	 *					Error
	 */
	int(*ss_set_inten)(void *, u32, u32, u32);
	/* This function Gets the Interrupt Sts Register.
	 * param[in/out]	IN:	ops Adaption ops Struct registered.
	 * param[in/out]	IN:	module	0 - XGMAC
	 *					4 - LINK
	 * param[in/out]	IN:	idx	XGMAC Index to Get Sts
	 * return		OUT	val:	Interrupt Sts
	 *					0 - no interrupt pending
	 *					1 - interrupt is pending
	 * return		OUT	!0:	Interrupt Sts Register Get Error
	 */
	int(*ss_get_intstat)(void *, u32);
	/* This sequence is Read Adaption register
	 * param[in/out]IN:	ops	Adap ops Struct registered
	 * param[in/out]IN:	off	Adap Register offset.
	 * return	OUT	reg_val:Register value will be returned
	 */
	int(*ss_rg_rd)(void *, u32);
	/* This sequence is Write Adaption register
	 * param[in/out]IN:	ops	Adap ops Struct registered.
	 * param[in/out]IN:	off	Adap Register offset.
	 * param[in/out]IN:	val	Adap Register Value to be written.
	 */
	int(*ss_rg_wr)(void *, u32, u32);
	/* This sequence is used for GSWSS Cli implementation
	 * param[in/out]IN:	argc - Number of args.
	 * param[in/out]IN:	argv - Argument value.
	 * return	OUT	-1: 	Exit GSWSS Error
	 */
	int(*ss_cli)(GSW_MAC_Cli_t *);
};

#endif
