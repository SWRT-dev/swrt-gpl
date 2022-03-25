/*
 * Intel XPCS SysFs
 *
 * Copyright (C) 2018 Intel, Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/device.h>
#include "xpcs.h"

static ssize_t
xpcs_linksts_show(struct device *dev,
		  struct device_attribute *attr, char *buf)
{
	struct xpcs_prv_data *pdata;
	int off = 0;
	u32 val;

	pdata = dev_get_drvdata(dev);

	off = sprintf(buf + off, "\n%s: LINK Status\n", pdata->name);
	off += sprintf(buf + off, "\n");

	off += sprintf(buf + off, "\nPMA Tx status\n");
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, PMA_TX_STS, TX_ACK_0);
	off += sprintf(buf + off, "\tTX_ACK_0:                            %s\n",
		       val ? "Tx Acknowledge on Lane 0" :
		       "Tx NOT Acknowledge on Lane 0");

	val = XPCS_RGRD_VAL(pdata, PMA_TX_STS, DETRX_RSLT_0);
	off += sprintf(buf + off, "\tRx Detection Result on lane 0:       %s\n",
		       val ? "Rx Detected on Lane 0" :
		       "Rx NOT Detected on Lane 0");

	val = XPCS_RGRD_VAL(pdata, PMA_TX_PS_CTRL, TX0_PSTATE);
	off += sprintf(buf + off,
		       "\tTX0_PSTATE:                          %08x\n",
		       val);

	off += sprintf(buf + off, "\nPMA Rx status\n");
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, PMA_STS1, RLU);
	off += sprintf(buf + off, "\tPMA_STS:                             %s\n",
		       val ? "LINK UP" : "LINK DOWN");

	val = XPCS_RGRD_VAL(pdata, PMA_STS2, RF);
	off += sprintf(buf + off, "\tPMA_STATUS2 RF:                      %s\n",
		       val ? "Rx Fault" : "Rx No Fault");

	val = XPCS_RGRD_VAL(pdata, PMA_KR_PMD_STS, RCV_STS);
	off += sprintf(buf + off, "\tPMA_KR_PMD_STS RCV_STS:              %s\n",
		       val ? "Received trained and ready to receive data" :
		       "Rx training");

	val = XPCS_RGRD_VAL(pdata, PMA_KR_PMD_STS, FRM_LCK);
	off += sprintf(buf + off, "\tPMA_KR_PMD_STS FRM_LCK:              %s\n",
		       val ? "Training frame delineation detected" :
		       "Training frame delineation not detected");

	val = XPCS_RGRD_VAL(pdata, PMA_KR_PMD_STS, SU_PR_DTD);
	off += sprintf(buf + off, "\tPMA_KR_PMD_STS SU_PR_DTD:            %s\n",
		       val ? "Start-up protocol in progress" :
		       "Start-up protocol complete");

	val = XPCS_RGRD_VAL(pdata, PMA_KR_PMD_STS, TR_FAIL);
	off += sprintf(buf + off, "\tPMA_KR_PMD_STS TR_FAIL:              %s\n",
		       val ? "Training is completed with failure" :
		       "Training is not yet complete");

	off += sprintf(buf + off, "\n10GBASE-KR LP coeff Update\n");
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, PMA_KR_LP_CEU, LP_INIT);
	off += sprintf(buf + off, "\tPMA_KR_PMD_STS LP_INIT:              %s\n",
		       val ? "Init the local device tx filter coeff" :
		       "Normal operation");

	val = XPCS_RGRD_VAL(pdata, PMA_KR_LP_CEU, LP_PRST);
	off += sprintf(buf + off, "\tPMA_KR_PMD_STS LP_PRST:              %s\n",
		       val ? "LD Pre-initialize coeffs" :
		       "Normal operation");

	off += sprintf(buf + off, "\n10GBASE-KR LP coeff Status\n");
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, PMA_KR_LP_CESTS, LP_RR);
	off += sprintf(buf + off, "\tPMA_KR_LP_CESTS LP_RR:               %s\n",
		       val ? "The LP rx has determined that the training is "
		       "complete, and it is ready to receive data." :
		       "The LP rx is requesting that the training "
		       "should be continued.");

	off += sprintf(buf + off, "\n10GBASE-KR LD coeff Update Local Device "
		       "requests to Link Partner.\n");
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, PMA_KR_LD_CEU, LD_INIT);
	off += sprintf(buf + off, "\tPMA_KR_LD_CEU LD_INIT:               %s\n",
		       val ? "Init the link partner tx filter coeff" :
		       "Normal operation");

	val = XPCS_RGRD_VAL(pdata, PMA_KR_LD_CEU, LD_PRST);
	off += sprintf(buf + off, "\tPMA_KR_LD_CEU PRST:                  %s\n",
		       val ? "LP Pre-initialize coeffs" :
		       "Normal operation");

	off += sprintf(buf + off, "\n10GBASE-KR LD coeff Status\n");
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, PMA_KR_LD_CESTS, LD_RR);
	off += sprintf(buf + off, "\tPMA_KR_LD_CESTS RR:                  %s\n",
		       val ? "The LD rx has determined that the training is "
		       "complete, and it is ready to receive data." :
		       "The LD rx is requesting that the training "
		       "should be continued.");

	off += sprintf(buf + off, "\nAutonegotiation Status Register\n");
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, AN_STS, LP_AN_ABL);
	off += sprintf(buf + off, "\tLP AN Ability:                       %s\n",
		       val ? "LP able to participate in AN" :
		       "LP unable to participate in AN");

	val = XPCS_RGRD_VAL(pdata, AN_STS, AN_LS);
	off += sprintf(buf + off, "\tAN Link Status:                      %s\n",
		       val ? "CL73 AN complete and determined a valid link" :
		       "CL73 not complete and no valid link");

	val = XPCS_RGRD_VAL(pdata, AN_STS, AN_ABL);
	off += sprintf(buf + off, "\tLD AN Ability:                       %s\n",
		       val ? "LD supports CL73 AN" :
		       "LD does not support CL73 AN");

	val = XPCS_RGRD_VAL(pdata, AN_STS, AN_RF);
	off += sprintf(buf + off, "\tAN Remote Fault:                     %s\n",
		       val ? "AN process detected a remote fault" :
		       "No fault detected");

	val = XPCS_RGRD_VAL(pdata, AN_STS, ANC);
	off += sprintf(buf + off, "\tAN Complete:                         %s\n",
		       val ? "AN completes" :
		       "AN not complete");

	val = XPCS_RGRD_VAL(pdata, AN_STS, PR);
	off += sprintf(buf + off, "\tPage Received:                       %s\n",
		       val ? "Page is received and the corresponding "
		       "Link Code Word is stored " : "No page received");

	val = XPCS_RGRD_VAL(pdata, AN_STS, PDF);
	off += sprintf(buf + off, "\tParallel Detection Fault in CL73 :   %s\n",
		       val ? "Parallel detection fault detected " :
		       "No fault detected");

	off += sprintf(buf + off, "\nPCS Rx Status\n");
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, PCS_STS1, RLU);
	off += sprintf(buf + off, "\tPCS_STS:                             %s\n",
		       val ? "LINK UP" : "LINK DOWN");

	val = XPCS_RGRD_VAL(pdata, PCS_STS2, RF);
	off += sprintf(buf + off, "\tPCS_STATUS2 RF:                      %s\n",
		       val ? "Rx Fault" : "Rx No Fault");

	val = XPCS_RGRD_VAL(pdata, PCS_STS2, TF);
	off += sprintf(buf + off, "\tPCS_STATUS2 TF:                      %s\n",
		       val ? "Tx Fault" : "Tx No Fault");

	val = XPCS_RGRD_VAL(pdata, PCS_STS2, CAP_EN);
	off += sprintf(buf + off, "\tCAP_EN:                              %s\n",
		       val ? "10GBASE-R Capable" : "10GBASE-R Not Capable");

	val = XPCS_RGRD_VAL(pdata, PCS_STS2, CAP_10_1GC);
	off += sprintf(buf + off, "\tCAP_10_1GC:                          %s\n",
		       val ? "10GBASE-X Capable" : "10GBASE-X Not Capable");

	val = XPCS_RGRD_VAL(pdata, PCS_STS2, CAP_10GBW);
	off += sprintf(buf + off, "\tCAP_10GBW:                           %s\n",
		       val ? "10GBASE-W Capable" : "10GBASE-W Not Capable");

	val = XPCS_RGRD_VAL(pdata, PCS_STS2, CAP_10GBT);
	off += sprintf(buf + off, "\tCAP_10GBT:                           %s\n",
		       val ? "10GBASE-T Capable" : "10GBASE-T Not Capable");

	val = XPCS_RGRD_VAL(pdata, PCS_STS2, DS);
	off += sprintf(buf + off, "\tDS:                                  %s\n",
		       (val == 2) ?
		       "MMD is present and responding to this reg addr" :
		       "MMD is not present or not functioning properly");

	val = XPCS_RGRD(pdata, PCS_KR_STS2);
	off += sprintf(buf + off,
		       "\tPCS_KR_STS2 :                        %08X\n", val);

	val = XPCS_RGRD_VAL(pdata, PCS_CTRL2, PCS_TYPE_SEL);

	if (val == 0) {
		off += sprintf(buf + off,
			       "\tPCS_TYPE_SEL:                        %s\n",
			       "10GBASE-R PCS Type");
	} else if (val == 1) {
		off += sprintf(buf + off,
			       "\tPCS_TYPE_SEL:                        %s\n",
			       "10GBASE-X PCS Type");
	} else if (val == 2) {
		off += sprintf(buf + off,
			       "\tPCS_TYPE_SEL:                        %s\n",
			       "10GBASE-W PCS Type");
	} else {
		off += sprintf(buf + off,
			       "\tPCS_TYPE_SEL:                        %s\n",
			       "Reserved");
	}

	off += sprintf(buf + off, "\nMII Status\n");
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, SR_MII_STS, LINK_STS);
	off += sprintf(buf + off, "\t1000BASE-X mode Rx:                  %s\n",
		       val ? "LINK UP" : "LINK DOWN");

	val = XPCS_RGRD_VAL(pdata, VR_MII_AN_CTRL, SGMII_LINK_STS);
	off += sprintf(buf + off, "\tSGMII/QSGMII/USXGMII mode Port0 Rx:  %s\n",
		       val ? "LINK UP" : "LINK DOWN");

	return off;
}

static DEVICE_ATTR_RO(xpcs_linksts);

static ssize_t
xpcs_cl73_info_show(struct device *dev,
		    struct device_attribute *attr, char *buf)
{
	struct xpcs_prv_data *pdata;
	int off = 0;
	u32 val;

	pdata = dev_get_drvdata(dev);

	off = sprintf(buf + off, "\n%s: CL 73 Status\n", pdata->name);
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, AN_INTR, AN_INT_CMPLT);
	off += sprintf(buf + off, "AN_INT_COMPL:                          %s\n",
		       val ? "SET" : "NOT_SET");

	val = XPCS_RGRD_VAL(pdata, AN_INTR, AN_INC_LINK);
	off += sprintf(buf + off, "AN_INC_LINK:                           %s\n",
		       val ? "SET" : "NOT_SET");

	val = XPCS_RGRD_VAL(pdata, AN_INTR, AN_PG_RCV);
	off += sprintf(buf + off, "AN_PG_RCV:                             %s\n",
		       val ? "SET" : "NOT_SET");

	val = XPCS_RGRD_VAL(pdata, AN_STS, LP_AN_ABL);
	off += sprintf(buf + off, "AN_STS:                                %s\n",
		       val ? "LP able to participate in AN" :
		       "LP unable to participate in AN");

	val = XPCS_RGRD_VAL(pdata, AN_STS, AN_LS);
	off += sprintf(buf + off, "AN_STS:                                %s\n",
		       val ? "CL73 AN complete and determined a valid link" :
		       "CL73 not complete and no valid link");

	val = XPCS_RGRD_VAL(pdata, AN_STS, AN_ABL);
	off += sprintf(buf + off, "AN_STS:                                %s\n",
		       val ? "LD supports CL73 AN" :
		       "LD does not support CL73 AN");

	val = XPCS_RGRD_VAL(pdata, AN_STS, AN_RF);
	off += sprintf(buf + off, "AN_STS:                                %s\n",
		       val ? "AN process detected a remote fault" :
		       "No fault detected");

	val = XPCS_RGRD_VAL(pdata, AN_STS, ANC);
	off += sprintf(buf + off, "AN_STS:                                %s\n",
		       val ? "AN completes" :
		       "AN not complete");

	val = XPCS_RGRD_VAL(pdata, AN_STS, PR);
	off += sprintf(buf + off, "AN_STS:                                %s\n",
		       val ? "Parallel detection fault detected " :
		       "No fault detected");

	val = XPCS_RGRD_VAL(pdata, AN_STS, PDF);
	off += sprintf(buf + off, "AN_STS:                                %s\n",
		       val ? "Parallel detection fault detected " :
		       "No fault detected");

	return off;
}

static DEVICE_ATTR_RO(xpcs_cl73_info);

static ssize_t
xpcs_table_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	struct xpcs_prv_data *pdata;
	int off = 0;
	u32 val;
	u32 mplla_ctrl2 = 0, afe_dfe_ctrl;
	u32 tx_eq0, tx_eq1;

	pdata = dev_get_drvdata(dev);

	off = sprintf(buf + off, "\n%s: Table Show\n", pdata->name);
	off += sprintf(buf + off, "\n");

	off += sprintf(buf + off, "ID:                            %d\n",
		       pdata->mode_cfg->id);
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, PMA_MPLLA_C0, MPLLA_MULTIPLIER);
	off += sprintf(buf + off, "MPLLA_MULTIPLIER:              %x\n", val);

	val = XPCS_RGRD_VAL(pdata, PMA_MPLLA_C3, MPPLA_BANDWIDTH);
	off += sprintf(buf + off, "MPPLA_BANDWIDTH:               %x\n", val);

	switch (pdata->mode_cfg->lane) {
	case LANE_4:
		val = XPCS_RGRD_VAL(pdata, PMA_VCO_CAL_LD3, VCO_LD_VAL_3);
		off += sprintf(buf + off, "VCO_LD_VAL_3:                  %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_VCO_CAL_REF1, VCO_REF_LD_3);
		off += sprintf(buf + off, "VCO_REF_LD_3:                  %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_TX_RATE_CTRL, TX_RATE_3);
		off += sprintf(buf + off, "TX_RATE_3:                     %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_RX_RATE_CTRL, RX_RATE_3);
		off += sprintf(buf + off, "RX_RATE_3:                     %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_TX_GENCTRL2, TX_WIDTH_3);
		off += sprintf(buf + off, "TX_WIDTH_3:                    %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_RX_GENCTRL2, RX_WIDTH_3);
		off += sprintf(buf + off, "RX_WIDTH_3:                    %x\n",
			       val);

	case LANE_3:
		val = XPCS_RGRD_VAL(pdata, PMA_VCO_CAL_LD2, VCO_LD_VAL_2);
		off += sprintf(buf + off, "VCO_LD_VAL_2:                  %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_VCO_CAL_REF1, VCO_REF_LD_2);
		off += sprintf(buf + off, "VCO_REF_LD_2:                  %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_TX_RATE_CTRL, TX_RATE_2);
		off += sprintf(buf + off, "TX_RATE_2:                     %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_RX_RATE_CTRL, RX_RATE_2);
		off += sprintf(buf + off, "RX_RATE_2:                     %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_TX_GENCTRL2, TX_WIDTH_2);
		off += sprintf(buf + off, "TX_WIDTH_2:                    %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_RX_GENCTRL2, RX_WIDTH_2);
		off += sprintf(buf + off, "RX_WIDTH_2:                    %x\n",
			       val);

	case LANE_2:
		val = XPCS_RGRD_VAL(pdata, PMA_VCO_CAL_LD1, VCO_LD_VAL_1);
		off += sprintf(buf + off, "VCO_LD_VAL_1:                  %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_VCO_CAL_REF0, VCO_REF_LD_1);
		off += sprintf(buf + off, "VCO_REF_LD_1:                  %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_TX_RATE_CTRL, TX_RATE_1);
		off += sprintf(buf + off, "TX_RATE_1:                     %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_RX_RATE_CTRL, RX_RATE_1);
		off += sprintf(buf + off, "RX_RATE_1:                     %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_TX_GENCTRL2, TX_WIDTH_1);
		off += sprintf(buf + off, "TX_WIDTH_1:                    %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_RX_GENCTRL2, RX_WIDTH_1);
		off += sprintf(buf + off, "RX_WIDTH_1:                    %x\n",
			       val);

	case LANE_1:
		val = XPCS_RGRD_VAL(pdata, PMA_VCO_CAL_LD0, VCO_LD_VAL_0);
		off += sprintf(buf + off, "VCO_LD_VAL_0:                  %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_VCO_CAL_REF0, VCO_REF_LD_0);
		off += sprintf(buf + off, "VCO_REF_LD_0:                  %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_TX_RATE_CTRL, TX_RATE_0);
		off += sprintf(buf + off, "TX_RATE_0:                     %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_RX_RATE_CTRL, RX_RATE_0);
		off += sprintf(buf + off, "RX_RATE_0:                     %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_TX_GENCTRL2, TX_WIDTH_0);
		off += sprintf(buf + off, "TX_WIDTH_0:                    %x\n",
			       val);

		val = XPCS_RGRD_VAL(pdata, PMA_RX_GENCTRL2, RX_WIDTH_0);
		off += sprintf(buf + off, "RX_WIDTH_0:                    %x\n",
			       val);

		break;

	default:
		break;
	}

	afe_dfe_ctrl = XPCS_RGRD(pdata, PMA_AFE_DFE_EN_CTRL);

	val = XPCS_GET_VAL(val, PMA_AFE_DFE_EN_CTRL, AFE_EN_0);
	off += sprintf(buf + off, "AFE_EN_0:                      %x\n", val);

	val = XPCS_GET_VAL(val, PMA_AFE_DFE_EN_CTRL, AFE_EN_3_1);
	off += sprintf(buf + off, "AFE_EN_3_1:                    %x\n", val);

	val = XPCS_GET_VAL(val, PMA_AFE_DFE_EN_CTRL, DFE_EN_0);
	off += sprintf(buf + off, "DFE_EN_0:                      %x\n", val);

	val = XPCS_GET_VAL(val, PMA_AFE_DFE_EN_CTRL, DFE_EN_3_1);
	off += sprintf(buf + off, "DFE_EN_3_1:                    %x\n", val);

	val = XPCS_RGRD_VAL(pdata, PMA_RX_EQ_CTRL4, CONT_ADAPT_0);
	off += sprintf(buf + off, "CONT_ADAPT_0:                  %x\n", val);

	val = XPCS_RGRD_VAL(pdata, PMA_RX_EQ_CTRL4, CONT_ADAPT_3_1);
	off += sprintf(buf + off, "CONT_ADAPT_3_1:                %x\n", val);

	mplla_ctrl2 = XPCS_RGRD(pdata, PMA_MPLLA_C2);

	val = XPCS_GET_VAL(mplla_ctrl2, PMA_MPLLA_C2, MPLLA_DIV16P5_CLK_EN);
	off += sprintf(buf + off, "MPLLA_DIV16P5_CLK_EN:          %x\n", val);

	val = XPCS_GET_VAL(mplla_ctrl2, PMA_MPLLA_C2, MPLLA_DIV10_CLK_EN);
	off += sprintf(buf + off, "MPLLA_DIV10_CLK_EN:            %x\n", val);

	val = XPCS_GET_VAL(mplla_ctrl2, PMA_MPLLA_C2, MPLLA_DIV8_CLK_EN);
	off += sprintf(buf + off, "MPLLA_DIV8_CLK_EN:             %x\n", val);

	tx_eq0 = XPCS_RGRD(pdata, PMA_TX_EQ_C0);

	val = XPCS_GET_VAL(tx_eq0, PMA_TX_EQ_C0, TX_EQ_MAIN);
	off += sprintf(buf + off, "TX_EQ_MAIN:                    %x\n", val);

	val = XPCS_GET_VAL(tx_eq0, PMA_TX_EQ_C0, TX_EQ_PRE);
	off += sprintf(buf + off, "TX_EQ_PRE:                     %x\n", val);

	tx_eq1 = XPCS_RGRD(pdata, PMA_TX_EQ_C1);

	val = XPCS_GET_VAL(tx_eq1, PMA_TX_EQ_C1, TX_EQ_POST);
	off += sprintf(buf + off, "TX_EQ_POST:                    %x\n", val);

	val = XPCS_GET_VAL(tx_eq1, PMA_TX_EQ_C1, TX_EQ_OVR_RIDE);
	off += sprintf(buf + off, "TX_EQ_OVR_RIDE:                %x\n", val);

	return off;
}

static DEVICE_ATTR_RO(xpcs_table);

static const char *xpcs_status_strings[] = {
	"Wait for Ack High 0",
	"Wait for Ack Low 0",
	"Wait for Ack High 1",
	"Wait for Ack Low 1",
	"Tx/Rx Stable (Power_Good state)",
	"Power Save state",
	"Power Down state",
};

static ssize_t xpcs_status_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct xpcs_prv_data *pdata;
	int off = 0;
	u32 val = 0;

	pdata = dev_get_drvdata(dev);

	off = sprintf(buf + off, "\n%s: RX/TX Stable Status\n", pdata->name);
	off += sprintf(buf + off, "\n");

	val = XPCS_RGRD_VAL(pdata, PCS_DIG_STS, PSEQ_STATE);

	if (val < ARRAY_SIZE(xpcs_status_strings))
		off += sprintf(buf + off, "\tStatus:		      %s\n",
			       xpcs_status_strings[val]);
	else
		off += sprintf(buf + off, "\tStatus:		      %s\n",
			       "Unknown/Invalid Status");

	return off;
}
static DEVICE_ATTR_RO(xpcs_status);

static struct attribute *xpcs_attrs[] = {
	&dev_attr_xpcs_cl73_info.attr,
	&dev_attr_xpcs_status.attr,
	&dev_attr_xpcs_table.attr,
	&dev_attr_xpcs_linksts.attr,
	NULL,
};

ATTRIBUTE_GROUPS(xpcs);

int xpcs_sysfs_init(struct xpcs_prv_data *priv)
{
	return sysfs_create_groups(&priv->dev->kobj, xpcs_groups);
}

