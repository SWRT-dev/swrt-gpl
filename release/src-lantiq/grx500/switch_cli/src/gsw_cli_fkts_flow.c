/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include "ltq_cli_lib.h"


#ifndef IOCTL_PCTOOL
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#endif

#define NUM_TC 16

int gsw_read_all_enabled_pce_rule(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_rule_t pce_rule;
	GSW_cap_t cap;
	int cnt = 0, i;
	int table_size_to_read = 0;
	int k = 0;
	GSW_register_t param;

	cnt += scanParamArg(argc, argv, "nTableSize", sizeof(table_size_to_read), &table_size_to_read);

	memset(&param, 0, sizeof(GSW_register_t));

	if (gswip_version == GSWIP_3_2 || gswip_version == GSWIP_3_1) {
		param.nRegAddr = 0x46f;

		if (cli_ioctl(fd, GSW_REGISTER_GET, &param) != 0) {
			printf("ioctl returned with ERROR!\n");
			return (-1);
		}
	} else {
		param.nRegAddr = 0xf;

		if (cli_ioctl(fd, GSW_REGISTER_GET, &param) != 0) {
			printf("ioctl returned with ERROR!\n");
			return (-1);
		}
	}

	if (cnt == 0 && table_size_to_read == 0) {
		if (gswip_version == GSWIP_3_2 || gswip_version == GSWIP_3_1)
			table_size_to_read = param.nData << 2;
		else
			table_size_to_read = param.nData;
	}

	printf("PCE table_size_to_read = %d\n", table_size_to_read);

	for (k = 0; k < table_size_to_read; k++) {
		memset(&pce_rule, 0x00, sizeof(pce_rule));
		pce_rule.pattern.nIndex = k;
		printf("\n");
		printf("PCE INDEX : %d\n", pce_rule.pattern.nIndex);

		if (cli_ioctl(fd, GSW_PCE_RULE_READ, &pce_rule) != 0)
			return (-3);

		if (pce_rule.pattern.bEnable) {
			printf("\n\tp.nIndex                                           = %u", pce_rule.pattern.nIndex);

			if (pce_rule.pattern.bMAC_DstEnable) {
				printf("\n\tp.bMAC_DstEnable                                   = %u", pce_rule.pattern.bMAC_DstEnable);
				printf("\n\tp.bDstMAC_Exclude                                  = %u", pce_rule.pattern.bDstMAC_Exclude);
				printf("\n\tp.nMAC_Dst                                         = ");

				for (i = 0; i < 6; i++) {
					printf("%2.2x", pce_rule.pattern.nMAC_Dst[i]);
				}

				printf("\n\tp.nMAC_DstMask                                     = 0x%x", pce_rule.pattern.nMAC_DstMask);
			}

			if (pce_rule.pattern.bMAC_SrcEnable) {
				printf("\n\tp.bMAC_SrcEnable                                   = %u", pce_rule.pattern.bMAC_SrcEnable);
				printf("\n\tp.bSrcMAC_Exclude                                  = %u", pce_rule.pattern.bSrcMAC_Exclude);
				printf("\n\tp.nMAC_Src                                         = ");

				for (i = 0; i < 6; i++) {
					printf("%2.2x", pce_rule.pattern.nMAC_Src[i]);
				}

				printf("\n\tp.nMAC_SrcMask                                     = 0x%x", pce_rule.pattern.nMAC_SrcMask);
			}

			if (pce_rule.pattern.eDstIP_Select) {
				printf("\n\tp.eDstIP_Select                                    = %u", pce_rule.pattern.eDstIP_Select);
				printf("\n\tp.bDstIP_Exclude                                   = %u", pce_rule.pattern.bDstIP_Exclude);

				if (pce_rule.pattern.eDstIP_Select == GSW_PCE_IP_V4) {
					printf("\n\tp.nDstIP                                           = 0x%x", pce_rule.pattern.nDstIP.nIPv4);
					printf("\n\tp.nDstIP_Mask                                      = 0x%x", pce_rule.pattern.nDstIP_Mask);
				} else if (pce_rule.pattern.eDstIP_Select == GSW_PCE_IP_V6) {
					printf("\n\tp.nDstIP                                           = ");

					for (i = 0; i < 8; i++) {
						if (i == 7)
							printf("%x", pce_rule.pattern.nDstIP.nIPv6[i]);
						else
							printf("%x:", pce_rule.pattern.nDstIP.nIPv6[i]);
					}

					printf("\n\tp.nDstIP_Mask                                      = 0x%x", pce_rule.pattern.nDstIP_Mask);
				}
			}

			if (pce_rule.pattern.eInnerDstIP_Select) {
				printf("\n\tp.eInnerDstIP_Select                               = %u", pce_rule.pattern.eInnerDstIP_Select);
				printf("\n\tp.bInnerDstIP_Exclude                              = %u", pce_rule.pattern.bInnerDstIP_Exclude);

				if (pce_rule.pattern.eInnerDstIP_Select == GSW_PCE_IP_V4) {
					printf("\n\tp.nInnerDstIP                                      = 0x%x", pce_rule.pattern.nInnerDstIP.nIPv4);
					printf("\n\tp.nInnerDstIP_Mask                                 = 0x%x", pce_rule.pattern.nInnerDstIP_Mask);
				} else if (pce_rule.pattern.eInnerDstIP_Select == GSW_PCE_IP_V6) {
					printf("\n\tp.nInnerDstIP                                      = ");

					for (i = 0; i < 8; i++) {
						if (i == 7)
							printf("%x", pce_rule.pattern.nInnerDstIP.nIPv6[i]);
						else
							printf("%x:", pce_rule.pattern.nInnerDstIP.nIPv6[i]);
					}

					printf("\n\tp.nInnerDstIP_Mask                                 = 0x%x", pce_rule.pattern.nInnerDstIP_Mask);
				}
			}

			if (pce_rule.pattern.eSrcIP_Select) {
				printf("\n\tp.eSrcIP_Select                                    = %u", pce_rule.pattern.eSrcIP_Select);
				printf("\n\tp.bSrcIP_Exclude                                   = %u", pce_rule.pattern.bSrcIP_Exclude);

				if (pce_rule.pattern.eSrcIP_Select == GSW_PCE_IP_V4) {
					printf("\n\tp.nSrcIP                                           = 0x%x", pce_rule.pattern.nSrcIP.nIPv4);
					printf("\n\tp.nSrcIP_Mask                                      = 0x%x", pce_rule.pattern.nSrcIP_Mask);
				} else if (pce_rule.pattern.eSrcIP_Select == GSW_PCE_IP_V6) {
					printf("\n\tp.nSrcIP                                           = ");

					for (i = 0; i < 8; i++) {
						if (i == 7)
							printf("%x", pce_rule.pattern.nSrcIP.nIPv6[i]);
						else
							printf("%x:", pce_rule.pattern.nSrcIP.nIPv6[i]);
					}

					printf("\n\tp.nSrcIP_Mask                                      = 0x%x", pce_rule.pattern.nSrcIP_Mask);
				}
			}

			if (pce_rule.pattern.eInnerSrcIP_Select) {
				printf("\n\tp.eInnerSrcIP_Select                               = %u", pce_rule.pattern.eInnerSrcIP_Select);
				printf("\n\tp.bInnerSrcIP_Exclude                              = %u", pce_rule.pattern.bInnerSrcIP_Exclude);

				if (pce_rule.pattern.eInnerSrcIP_Select == GSW_PCE_IP_V4) {
					printf("\n\tp.nInnerSrcIP                                      = 0x%x", pce_rule.pattern.nInnerSrcIP.nIPv4);
					printf("\n\tp.nInnerSrcIP_Mask                                 = 0x%x", pce_rule.pattern.nInnerSrcIP_Mask);
				} else if (pce_rule.pattern.eInnerSrcIP_Select == GSW_PCE_IP_V6) {
					printf("\n\tp.nInnerSrcIP                                      = ");

					for (i = 0; i < 8; i++) {
						if (i == 7)
							printf("%x", pce_rule.pattern.nInnerSrcIP.nIPv6[i]);
						else
							printf("%x:", pce_rule.pattern.nInnerSrcIP.nIPv6[i]);
					}

					printf("\n\tp.nInnerSrcIP_Mask                                 = 0x%x", pce_rule.pattern.nInnerSrcIP_Mask);
				}
			}

			if (pce_rule.pattern.bVid) {
				printf("\n\tp.bVid                                             = %u", pce_rule.pattern.bVid);
				printf("\n\tp.bVid_Exclude                                     = %u", pce_rule.pattern.bVid_Exclude);
				printf("\n\tp.nVid                                             = %u", pce_rule.pattern.nVid);

				if (pce_rule.pattern.bVidRange_Select)
					printf("\n\tp.bVidRange_Select                                 = %u (Range Key)", pce_rule.pattern.bVidRange_Select);
				else
					printf("\n\tp.bVidRange_Select                                 = %u (Mask Key)", pce_rule.pattern.bVidRange_Select);

				printf("\n\tp.nVidRange                                        = %u", pce_rule.pattern.nVidRange);
				printf("\n\tp.bVid_Original                                    = %u", pce_rule.pattern.bVid_Original);
			}

			if (pce_rule.pattern.bSLAN_Vid) {
				printf("\n\tp.bSLAN_Vid                                        = %u", pce_rule.pattern.bSLAN_Vid);
				printf("\n\tp.bSLANVid_Exclude                                 = %u", pce_rule.pattern.bSLANVid_Exclude);
				printf("\n\tp.nSLAN_Vid                                        = %u", pce_rule.pattern.nSLAN_Vid);

				if (pce_rule.pattern.bSVidRange_Select)
					printf("\n\tp.bSVidRange_Select                                = %u (Range Key)", pce_rule.pattern.bSVidRange_Select);
				else
					printf("\n\tp.bSVidRange_Select                                = %u (Mask Key)", pce_rule.pattern.bSVidRange_Select);

				printf("\n\tp.nOuterVidRange                                   = %u", pce_rule.pattern.nOuterVidRange);
				printf("\n\tp.bOuterVid_Original                               = %u", pce_rule.pattern.bOuterVid_Original);
			}

			if (pce_rule.pattern.bPortIdEnable) {
				printf("\n\tp.bPortIdEnable                                    = %u", pce_rule.pattern.bPortIdEnable);
				printf("\n\tp.bPortId_Exclude                                  = %u", pce_rule.pattern.bPortId_Exclude);
				printf("\n\tp.nPortId                                          = %u", pce_rule.pattern.nPortId);
			}

			if (pce_rule.pattern.bSubIfIdEnable) {
				printf("\n\tp.bSubIfIdEnable                                   = %u", pce_rule.pattern.bSubIfIdEnable);
				printf("\n\tp.bSubIfId_Exclude                                 = %u", pce_rule.pattern.bSubIfId_Exclude);
				printf("\n\tp.eSubIfIdType                                     = %u", pce_rule.pattern.eSubIfIdType);
				printf("\n\tp.nSubIfId                                         = %u", pce_rule.pattern.nSubIfId);
			}

			if (pce_rule.pattern.bPktLngEnable) {
				printf("\n\tp.bPktLngEnable                                    = %u", pce_rule.pattern.bPktLngEnable);
				printf("\n\tp.bPktLng_Exclude                                  = %u", pce_rule.pattern.bPktLng_Exclude);
				printf("\n\tp.nPktLng                                          = %u", pce_rule.pattern.nPktLng);
				printf("\n\tp.nPktLngRange                                     = %u", pce_rule.pattern.nPktLngRange);
			}

			if (pce_rule.pattern.bPayload1_SrcEnable) {
				printf("\n\tp.bPayload1_Exclude                                = %u", pce_rule.pattern.bPayload1_Exclude);
				printf("\n\tp.nPayload1                                        = 0x%x", pce_rule.pattern.nPayload1);
				printf("\n\tp.bPayload1MaskRange_Select                        = %u", pce_rule.pattern.bPayload1MaskRange_Select);
				printf("\n\tp.nPayload1_Mask                                   = 0x%x", pce_rule.pattern.nPayload1_Mask);
			}

			if (pce_rule.pattern.bPayload2_SrcEnable) {
				printf("\n\tp.bPayload2_Exclude                                = %u", pce_rule.pattern.bPayload2_Exclude);
				printf("\n\tp.nPayload2                                        = 0x%x", pce_rule.pattern.nPayload2);
				printf("\n\tp.bPayload2MaskRange_Select                        = %u", pce_rule.pattern.bPayload2MaskRange_Select);
				printf("\n\tp.nPayload2_Mask                                   = 0x%x", pce_rule.pattern.nPayload2_Mask);
			}

			if (pce_rule.pattern.bParserFlagLSB_Enable) {
				printf("\n\tp.bParserFlagLSB_Exclude                           = %u", pce_rule.pattern.bParserFlagLSB_Exclude);
				printf("\n\tp.nParserFlagLSB                                   = 0x%x", pce_rule.pattern.nParserFlagLSB);
				printf("\n\tp.nParserFlagLSB_Mask                              = 0x%x", pce_rule.pattern.nParserFlagLSB_Mask);
			}

			if (pce_rule.pattern.bParserFlagMSB_Enable) {
				printf("\n\tp.bParserFlagMSB_Exclude                           = %u", pce_rule.pattern.bParserFlagMSB_Exclude);
				printf("\n\tp.nParserFlagMSB                                   = 0x%x", pce_rule.pattern.nParserFlagMSB);
				printf("\n\tp.nParserFlagMSB_Mask                              = 0x%x", pce_rule.pattern.nParserFlagMSB_Mask);
			}

			if (pce_rule.pattern.bParserFlag1LSB_Enable) {
				printf("\n\tp.bParserFlag1LSB_Exclude                          = %u", pce_rule.pattern.bParserFlag1LSB_Exclude);
				printf("\n\tp.nParserFlag1LSB                                  = 0x%x", pce_rule.pattern.nParserFlag1LSB);
				printf("\n\tp.nParserFlag1LSB_Mask                             = 0x%x", pce_rule.pattern.nParserFlag1LSB_Mask);
			}

			if (pce_rule.pattern.bParserFlag1MSB_Enable) {
				printf("\n\tp.bParserFlag1MSB_Exclude                          = %u", pce_rule.pattern.bParserFlag1MSB_Exclude);
				printf("\n\tp.nParserFlag1MSB                                  = 0x%x", pce_rule.pattern.nParserFlag1MSB);
				printf("\n\tp.nParserFlag1MSB_Mask                             = 0x%x", pce_rule.pattern.nParserFlag1MSB_Mask);
			}

			if (pce_rule.action.eVLAN_Action) {
				printf("\n\ta.eVLAN_Action				= %u", pce_rule.action.eVLAN_Action);
				printf("\n\ta.nVLAN_Id					= %u", pce_rule.action.nVLAN_Id);
			}

			if (pce_rule.action.eSVLAN_Action) {
				printf("\n\ta.eSVLAN_Action 			= %u", pce_rule.action.eSVLAN_Action);
				printf("\n\ta.nSVLAN_Id 				= %u", pce_rule.action.nSVLAN_Id);
			}

			if (pce_rule.action.eVLAN_CrossAction)
				printf("\n\ta.eVLAN_CrossAction 		= %u", pce_rule.action.eVLAN_CrossAction);

			if (pce_rule.action.bPortBitMapMuxControl)
				printf("\n\ta.bPortBitMapMuxControl 	= %u", pce_rule.action.bPortBitMapMuxControl);

			if (pce_rule.action.bCVLAN_Ignore_Control)
				printf("\n\ta.bCVLAN_Ignore_Control 	= %u", pce_rule.action.bCVLAN_Ignore_Control);

			if (pce_rule.action.eLearningAction)
				printf("\n\ta.eLearningAction                                  = %u", pce_rule.action.eLearningAction);

			if (pce_rule.action.eSnoopingTypeAction)
				printf("\n\ta.eSnoopingTypeAction                              = %u", pce_rule.action.eSnoopingTypeAction);

			if (pce_rule.pattern.bEtherTypeEnable) {
				printf("\n\tp.bEtherType_Exclude                               = 0x%x", pce_rule.pattern.bEtherType_Exclude);
				printf("\n\tp.nEtherType                                       = 0x%x", pce_rule.pattern.nEtherType);
				printf("\n\tp.nEtherTypeMask                                   = 0x%x", pce_rule.pattern.nEtherTypeMask);
			}

			if (pce_rule.pattern.bProtocolEnable) {
				printf("\n\tp.bProtocol_Exclude                                = 0x%x", pce_rule.pattern.bProtocol_Exclude);
				printf("\n\tp.nProtocol                                        = 0x%x", pce_rule.pattern.nProtocol);
				printf("\n\tp.nProtocolMask                                    = 0x%x", pce_rule.pattern.nProtocolMask);
			}

			if (pce_rule.pattern.bInnerProtocolEnable) {
				printf("\n\tp.bInnerProtocol_Exclude                           = 0x%x", pce_rule.pattern.bInnerProtocol_Exclude);
				printf("\n\tp.nInnerProtocol                                   = 0x%x", pce_rule.pattern.nInnerProtocol);
				printf("\n\tp.nInnerProtocolMask                               = 0x%x", pce_rule.pattern.nInnerProtocolMask);
			}

			if (pce_rule.pattern.bSessionIdEnable) {
				printf("\n\tp.bSessionIdEnable                                 = 0x%x", pce_rule.pattern.bSessionIdEnable);
				printf("\n\tp.bSessionId_Exclude                               = 0x%x", pce_rule.pattern.bSessionId_Exclude);
				printf("\n\tp.nSessionId                                       = 0x%x", pce_rule.pattern.nSessionId);
			}

			if (pce_rule.pattern.bPPP_ProtocolEnable) {
				printf("\n\tp.bPPP_Protocol_Exclude                            = 0x%x", pce_rule.pattern.bPPP_Protocol_Exclude);
				printf("\n\tp.nPPP_Protocol                                    = 0x%x", pce_rule.pattern.nPPP_Protocol);
				printf("\n\tp.nPPP_ProtocolMask                                = 0x%x", pce_rule.pattern.nPPP_ProtocolMask);
			}

			if (pce_rule.pattern.bAppDataMSB_Enable) {
				printf("\n\tp.bAppMSB_Exclude                                  = 0x%x", pce_rule.pattern.bAppMSB_Exclude);
				printf("\n\tp.nAppDataMSB                                      = 0x%x", pce_rule.pattern.nAppDataMSB);
				printf("\n\tp.bAppMaskRangeMSB_Select                          = %u", pce_rule.pattern.bAppMaskRangeMSB_Select);
				printf("\n\tp.nAppMaskRangeMSB                                 = 0x%x", pce_rule.pattern.nAppMaskRangeMSB);
			}

			if (pce_rule.pattern.bAppDataLSB_Enable) {
				printf("\n\tp.bAppLSB_Exclude                                  = 0x%x", pce_rule.pattern.bAppLSB_Exclude);
				printf("\n\tp.nAppDataLSB                                      = 0x%x", pce_rule.pattern.nAppDataLSB);
				printf("\n\tp.bAppMaskRangeLSB_Select                          = %u", pce_rule.pattern.bAppMaskRangeLSB_Select);
				printf("\n\tp.nAppMaskRangeLSB                                 = 0x%x", pce_rule.pattern.nAppMaskRangeLSB);
			}

			if (pce_rule.pattern.bDSCP_Enable) {
				printf("\n\tp.bDSCP_Exclude                                    = %u", pce_rule.pattern.bDSCP_Exclude);
				printf("\n\tp.nDSCP                                            = %u", pce_rule.pattern.nDSCP);
			}

			if (pce_rule.pattern.bInner_DSCP_Enable) {
				printf("\n\tp.bInnerDSCP_Exclude                               = %u", pce_rule.pattern.bInnerDSCP_Exclude);
				printf("\n\tp.nInnerDSCP                                       = %u", pce_rule.pattern.nInnerDSCP);
			}

			if (pce_rule.action.bRemarkAction)
				printf("\n\ta.bRemarkAction                                    = Enabled  val = %u", pce_rule.action.bRemarkAction);

			if (pce_rule.action.bRemarkPCP)
				printf("\n\ta.bRemarkPCP                                       = Disabled val = %u", pce_rule.action.bRemarkPCP);

			if (pce_rule.action.bRemarkDSCP)
				printf("\n\ta.bRemarkDSCP                                      = Disabled val = %u", pce_rule.action.bRemarkDSCP);

			if (pce_rule.action.bRemarkClass)
				printf("\n\ta.bRemarkClass                                     = Disabled val = %u", pce_rule.action.bRemarkClass);

			if (pce_rule.action.bRemarkSTAG_PCP)
				printf("\n\ta.bRemarkSTAG_PCP                                  = Disabled val = %u", pce_rule.action.bRemarkSTAG_PCP);

			if (pce_rule.action.bRemarkSTAG_DEI)
				printf("\n\ta.bRemarkSTAG_DEI                                  = Disabled val = %u", pce_rule.action.bRemarkSTAG_DEI);

			if ((pce_rule.action.bRMON_Action) || (pce_rule.action.bFlowID_Action)) {
				printf("\n\ta.nFlowID/nRmon_ID                                 = %u", pce_rule.action.nFlowID);
			}

			if (pce_rule.pattern.bPCP_Enable) {
				printf("\n\tp.bPCP_Enable                                      = %u", pce_rule.pattern.bPCP_Enable);
				printf("\n\tp.bCTAG_PCP_DEI_Exclude                            = %u", pce_rule.pattern.bCTAG_PCP_DEI_Exclude);
				printf("\n\tp.nPCP                                             = %u", pce_rule.pattern.nPCP);
			}

			if (pce_rule.pattern.bSTAG_PCP_DEI_Enable) {
				printf("\n\tp.bSTAG_PCP_DEI_Enable                             = %u", pce_rule.pattern.bSTAG_PCP_DEI_Enable);
				printf("\n\tp.bSTAG_PCP_DEI_Exclude                            = %u", pce_rule.pattern.bSTAG_PCP_DEI_Exclude);
				printf("\n\tp.nSTAG_PCP_DEI                                    = %u", pce_rule.pattern.nSTAG_PCP_DEI);
			}

			if (pce_rule.action.ePortMapAction) {
				printf("\n\ta.ePortMapAction                                   = 0x%x", pce_rule.action.ePortMapAction);

				for (i = 0; i < 8; i++) {
					if (pce_rule.action.nForwardPortMap[i])
						printf("\n\ta.nForwardPortMap[%d]                              = 0x%x", i, pce_rule.action.nForwardPortMap[i]);
				}
			}

			if (pce_rule.action.eTrafficClassAction) {
				printf("\n\ta.eTrafficClassAction                              = %u", pce_rule.action.eTrafficClassAction);
				printf("\n\ta.nTrafficClassAlternate                           = %u", pce_rule.action.nTrafficClassAlternate);
			}

			if (pce_rule.action.bPortTrunkAction) {
				printf("\n\ta.bPortTrunkAction                                 = Enabled");
				printf("\n\ta.bPortLinkSelection                               = %u", pce_rule.action.bPortLinkSelection);
			}

			if (pce_rule.action.bExtendedVlanEnable) {
				printf("\n\ta.bExtendedVlanEnable                              = Enabled");
				printf("\n\ta.nExtendedVlanBlockId                             = %u", pce_rule.action.nExtendedVlanBlockId);
			}

			if (pce_rule.action.ePortFilterType_Action) {
				printf("\n\ta.ePortFilterType_Action                           = %u", pce_rule.action.ePortFilterType_Action);

				for (i = 0; i < 8; i++) {
					if (pce_rule.action.nForwardPortMap[i])
						printf("\n\ta.nForwardPortMap[%d]                              = 0x%x", i,  pce_rule.action.nForwardPortMap[i]);
				}
			}

			if (pce_rule.action.eProcessPath_Action)
				printf("\n\ta.eProcessPath_Action                              = %u", pce_rule.action.eProcessPath_Action);

			if (pce_rule.action.bOamEnable)
				printf("\n\ta.bOamEnable                                       = %u", pce_rule.action.bOamEnable);

			if (pce_rule.action.bExtractEnable)
				printf("\n\ta.bExtractEnable                                   = %u", pce_rule.action.bExtractEnable);

			if (pce_rule.action.bOamEnable || pce_rule.action.bExtractEnable)
				printf("\n\ta.nRecordId                                        = %u", pce_rule.action.nRecordId);

			if (pce_rule.action.eMeterAction) {
				printf("\n\ta.eMeterAction                                     = %u", pce_rule.action.eMeterAction);
				printf("\n\ta.nMeterId                                         = %u", pce_rule.action.nMeterId);
			}

			if (pce_rule.action.bFidEnable)
				printf("\n\ta.nFId                                             = %u", pce_rule.action.nFId);

			if (pce_rule.pattern.bInsertionFlag_Enable)
				printf("\n\tp.nInsertionFlag                                   = %u", pce_rule.pattern.nInsertionFlag);

			if (pce_rule.action.eCrossStateAction == GSW_PCE_ACTION_CROSS_STATE_CROSS)
				printf("\n\tp.eCrossStateAction                                = GSW_PCE_ACTION_CROSS_STATE_CROSS");
			else if (pce_rule.action.eCrossStateAction == GSW_PCE_ACTION_CROSS_STATE_REGULAR)
				printf("\n\tp.eCrossStateAction                                = GSW_PCE_ACTION_CROSS_STATE_REGULAR");
			else
				printf("\n\tp.eCrossStateAction                                = GSW_PCE_ACTION_CROSS_STATE_DISABLE");

			if (gswip_version == GSWIP_3_2) {
				/*Applicable onlt for GSWIP 3.2*/
				printf("\n\tp.bFlexibleField4_ExcludeEnable			= %u", pce_rule.pattern.bFlexibleField4_ExcludeEnable);
				printf("\n\tp.bFlexibleField4_RangeEnable			= %u", pce_rule.pattern.bFlexibleField4_RangeEnable);
				printf("\n\tp.nFlexibleField4_ParserIndex			= %u", pce_rule.pattern.nFlexibleField4_ParserIndex);
				printf("\n\tp.nFlexibleField4_Value                 = %u", pce_rule.pattern.nFlexibleField4_Value);
				printf("\n\tp.nFlexibleField4_MaskOrRange			= %u", pce_rule.pattern.nFlexibleField4_MaskOrRange);

				printf("\n\tp.bFlexibleField3_ExcludeEnable			= %u", pce_rule.pattern.bFlexibleField3_ExcludeEnable);
				printf("\n\tp.bFlexibleField3_RangeEnable			= %u", pce_rule.pattern.bFlexibleField3_RangeEnable);
				printf("\n\tp.nFlexibleField3_ParserIndex			= %u", pce_rule.pattern.nFlexibleField3_ParserIndex);
				printf("\n\tp.nFlexibleField3_Value                 = %u", pce_rule.pattern.nFlexibleField3_Value);
				printf("\n\tp.nFlexibleField3_MaskOrRange			= %u", pce_rule.pattern.nFlexibleField3_MaskOrRange);

				printf("\n\tp.bFlexibleField2_ExcludeEnable			= %u", pce_rule.pattern.bFlexibleField2_ExcludeEnable);
				printf("\n\tp.bFlexibleField2_RangeEnable			= %u", pce_rule.pattern.bFlexibleField2_RangeEnable);
				printf("\n\tp.nFlexibleField2_ParserIndex			= %u", pce_rule.pattern.nFlexibleField2_ParserIndex);
				printf("\n\tp.nFlexibleField2_Value                 = %u", pce_rule.pattern.nFlexibleField2_Value);
				printf("\n\tp.nFlexibleField2_MaskOrRange			= %u", pce_rule.pattern.nFlexibleField2_MaskOrRange);

				printf("\n\tp.bFlexibleField1_ExcludeEnable			= %u", pce_rule.pattern.bFlexibleField1_ExcludeEnable);
				printf("\n\tp.bFlexibleField1_RangeEnable			= %u", pce_rule.pattern.bFlexibleField1_RangeEnable);
				printf("\n\tp.nFlexibleField1_ParserIndex			= %u", pce_rule.pattern.nFlexibleField1_ParserIndex);
				printf("\n\tp.nFlexibleField1_Value                 = %u", pce_rule.pattern.nFlexibleField1_Value);
				printf("\n\tp.nFlexibleField1_MaskOrRange			= %u", pce_rule.pattern.nFlexibleField1_MaskOrRange);

				if (pce_rule.action.sPBB_Action.bIheaderActionEnable) {
					printf("\n\ta.sPBB_Action.bIheaderActionEnable			= %u", pce_rule.action.sPBB_Action.bIheaderActionEnable);

					switch (pce_rule.action.sPBB_Action.eIheaderOpMode) {
					case GSW_PCE_I_HEADER_OPERATION_INSERT :
						printf("\n\ta.sPBB_Action.eIheaderOpMode				= GSW_PCE_I_HEADER_OPERATION_INSERT");
						printf("\n\ta.sPBB_Action.nTunnelIdKnownTraffic			= %u", pce_rule.action.sPBB_Action.nTunnelIdKnownTraffic);
						printf("\n\ta.sPBB_Action.nTunnelIdUnKnownTraffic		= %u", pce_rule.action.sPBB_Action.nTunnelIdUnKnownTraffic);
						printf("\n\ta.sPBB_Action.bB_DstMac_FromMacTableEnable	= %u", pce_rule.action.sPBB_Action.bB_DstMac_FromMacTableEnable);

						break;

					case GSW_PCE_I_HEADER_OPERATION_REPLACE :
						printf("\n\ta.sPBB_Action.eIheaderOpMode				= GSW_PCE_I_HEADER_OPERATION_REPLACE");
						printf("\n\ta.sPBB_Action.nTunnelIdKnownTraffic			= %u", pce_rule.action.sPBB_Action.nTunnelIdKnownTraffic);
						printf("\n\ta.sPBB_Action.nTunnelIdUnKnownTraffic		= %u", pce_rule.action.sPBB_Action.nTunnelIdUnKnownTraffic);
						printf("\n\ta.sPBB_Action.bReplace_B_SrcMacEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_SrcMacEnable);

						printf("\n\ta.sPBB_Action.bReplace_B_DstMacEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_DstMacEnable);
						printf("\n\ta.sPBB_Action.bReplace_I_TAG_ResEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_ResEnable);
						printf("\n\ta.sPBB_Action.bReplace_I_TAG_UacEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_UacEnable);
						printf("\n\ta.sPBB_Action.bReplace_I_TAG_DeiEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_DeiEnable);
						printf("\n\ta.sPBB_Action.bReplace_I_TAG_PcpEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_PcpEnable);
						printf("\n\ta.sPBB_Action.bReplace_I_TAG_SidEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_SidEnable);
						printf("\n\ta.sPBB_Action.bReplace_I_TAG_TpidEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_TpidEnable);

						break;

					case GSW_PCE_I_HEADER_OPERATION_REMOVE :
						printf("\n\ta.sPBB_Action.eIheaderOpMode				= GSW_PCE_I_HEADER_OPERATION_REMOVE");

						break;

					case GSW_PCE_I_HEADER_OPERATION_NOCHANGE :
						printf("\n\ta.sPBB_Action.eIheaderOpMode				= GSW_PCE_I_HEADER_OPERATION_NOCHANGE");

						break;

					default:
						break;
					}
				}

				/*Applicable onlt for GSWIP 3.2*/
				if (pce_rule.action.sPBB_Action.bBtagActionEnable) {
					printf("\n\ta.sPBB_Action.bBtagActionEnable 			= %u", pce_rule.action.sPBB_Action.bBtagActionEnable);

					switch (pce_rule.action.sPBB_Action.eBtagOpMode) {
					case GSW_PCE_B_TAG_OPERATION_INSERT :
						printf("\n\ta.sPBB_Action.eBtagOpMode					= GSW_PCE_B_TAG_OPERATION_INSERT");
						printf("\n\ta.sPBB_Action.nProcessIdKnownTraffic		= %u", pce_rule.action.sPBB_Action.nProcessIdKnownTraffic);
						printf("\n\ta.sPBB_Action.nProcessIdUnKnownTraffic		= %u", pce_rule.action.sPBB_Action.nProcessIdUnKnownTraffic);
						break;

					case GSW_PCE_B_TAG_OPERATION_REPLACE :
						printf("\n\ta.sPBB_Action.eBtagOpMode					= GSW_PCE_B_TAG_OPERATION_REPLACE");
						printf("\n\ta.sPBB_Action.nProcessIdKnownTraffic		= %u", pce_rule.action.sPBB_Action.nProcessIdKnownTraffic);
						printf("\n\ta.sPBB_Action.nProcessIdUnKnownTraffic		= %u", pce_rule.action.sPBB_Action.nProcessIdUnKnownTraffic);
						printf("\n\ta.sPBB_Action.bReplace_B_TAG_DeiEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_TAG_DeiEnable);
						printf("\n\ta.sPBB_Action.bReplace_B_TAG_PcpEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_TAG_PcpEnable);
						printf("\n\ta.sPBB_Action.bReplace_B_TAG_VidEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_TAG_VidEnable);
						printf("\n\ta.sPBB_Action.bReplace_B_TAG_TpidEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_TAG_TpidEnable);
						break;

					case GSW_PCE_B_TAG_OPERATION_REMOVE :
						printf("\n\ta.sPBB_Action.eBtagOpMode					= GSW_PCE_B_TAG_OPERATION_REMOVE");
						break;

					case GSW_PCE_B_TAG_OPERATION_NOCHANGE :
						printf("\n\ta.sPBB_Action.eBtagOpMode					= GSW_PCE_B_TAG_OPERATION_NOCHANGE");
						break;

					default:
						break;
					}
				}

				/*Applicable onlt for GSWIP 3.2*/
				if (pce_rule.action.sPBB_Action.bMacTableMacinMacActionEnable) {
					printf("\n\ta.sPBB_Action.bMacTableMacinMacActionEnable 			= %u", pce_rule.action.sPBB_Action.bMacTableMacinMacActionEnable);

					switch (pce_rule.action.sPBB_Action.eMacTableMacinMacSelect) {
					case GSW_PCE_OUTER_MAC_SELECTED :
						printf("\n\ta.sPBB_Action.eMacTableMacinMacSelect					= GSW_PCE_OUTER_MAC_SELECTED");
						break;

					case GSW_PCE_INNER_MAC_SELECTED :
						printf("\n\ta.sPBB_Action.eMacTableMacinMacSelect					= GSW_PCE_INNER_MAC_SELECTED");
						break;

					default:
						break;
					}
				}

				/*Applicable onlt for GSWIP 3.2*/
				if (gswip_version == GSWIP_3_2) {
					printf("\n\tsDestSubIF_Action.bDestSubIFIDActionEnable 			= %u", pce_rule.action.sDestSubIF_Action.bDestSubIFIDActionEnable);
					printf("\n\tsDestSubIF_Action.bDestSubIFIDAssignmentEnable 		= %u", pce_rule.action.sDestSubIF_Action.bDestSubIFIDAssignmentEnable);
					printf("\n\tsDestSubIF_Action.nDestSubIFGrp_Field 			    = %u", pce_rule.action.sDestSubIF_Action.nDestSubIFGrp_Field);
				}



			}
		} else {
			printf("\n\tp.nIndex rule not set at	= %u", pce_rule.pattern.nIndex);
		}

		printf("\n\n");
		printf("-------------------------------------------------------");
	}

	printf("\n");
	return 0;
}

int gsw_pce_rule_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_rule_t pce_rule;
	int cnt = 0, i;

	memset(&pce_rule, 0x00, sizeof(pce_rule));
	cnt += scanParamArg(argc, argv, "pattern.nIndex", sizeof(pce_rule.pattern.nIndex), &pce_rule.pattern.nIndex);
	cnt += scanParamArg(argc, argv, "nRegion", sizeof(pce_rule.region), &pce_rule.region);
	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(pce_rule.logicalportid), &pce_rule.logicalportid);
	cnt += scanParamArg(argc, argv, "nSubIfIdGroup", sizeof(pce_rule.subifidgroup), &pce_rule.subifidgroup);

	if (cnt == 0)
		return (-2);

	if (cli_ioctl(fd, GSW_PCE_RULE_READ, &pce_rule) != 0)
		return (-3);

	if (pce_rule.pattern.bEnable) {
		printf("\n\tp.nIndex                                           = %u", pce_rule.pattern.nIndex);

		if (pce_rule.pattern.bMAC_DstEnable) {
			printf("\n\tp.bMAC_DstEnable                                   = %u", pce_rule.pattern.bMAC_DstEnable);
			printf("\n\tp.bDstMAC_Exclude                                  = %u", pce_rule.pattern.bDstMAC_Exclude);
			printf("\n\tp.nMAC_Dst                                         = ");

			for (i = 0; i < 6; i++) {
				printf("%2.2x", pce_rule.pattern.nMAC_Dst[i]);
			}

			printf("\n\tp.nMAC_DstMask                                     = 0x%x", pce_rule.pattern.nMAC_DstMask);
		}

		if (pce_rule.pattern.bMAC_SrcEnable) {
			printf("\n\tp.bMAC_SrcEnable                                   = %u", pce_rule.pattern.bMAC_SrcEnable);
			printf("\n\tp.bSrcMAC_Exclude                                  = %u", pce_rule.pattern.bSrcMAC_Exclude);
			printf("\n\tp.nMAC_Src                                         = ");

			for (i = 0; i < 6; i++) {
				printf("%2.2x", pce_rule.pattern.nMAC_Src[i]);
			}

			printf("\n\tp.nMAC_SrcMask                                     = 0x%x", pce_rule.pattern.nMAC_SrcMask);
		}

		if (pce_rule.pattern.eDstIP_Select) {
			printf("\n\tp.eDstIP_Select                                    = %u", pce_rule.pattern.eDstIP_Select);
			printf("\n\tp.bDstIP_Exclude                                   = %u", pce_rule.pattern.bDstIP_Exclude);

			if (pce_rule.pattern.eDstIP_Select == GSW_PCE_IP_V4) {
				printf("\n\tp.nDstIP                                           = 0x%x", pce_rule.pattern.nDstIP.nIPv4);
				printf("\n\tp.nDstIP_Mask                                      = 0x%x", pce_rule.pattern.nDstIP_Mask);
			} else if (pce_rule.pattern.eDstIP_Select == GSW_PCE_IP_V6) {
				printf("\n\tp.nDstIP                                           = ");

				for (i = 0; i < 8; i++) {
					if (i == 7)
						printf("%x", pce_rule.pattern.nDstIP.nIPv6[i]);
					else
						printf("%x:", pce_rule.pattern.nDstIP.nIPv6[i]);
				}

				printf("\n\tp.nDstIP_Mask                                      = 0x%x", pce_rule.pattern.nDstIP_Mask);
			}
		}

		if (pce_rule.pattern.eInnerDstIP_Select) {
			printf("\n\tp.eInnerDstIP_Select                               = %u", pce_rule.pattern.eInnerDstIP_Select);
			printf("\n\tp.bInnerDstIP_Exclude                              = %u", pce_rule.pattern.bInnerDstIP_Exclude);

			if (pce_rule.pattern.eInnerDstIP_Select == GSW_PCE_IP_V4) {
				printf("\n\tp.nInnerDstIP                                      = 0x%x", pce_rule.pattern.nInnerDstIP.nIPv4);
				printf("\n\tp.nInnerDstIP_Mask                                 = 0x%x", pce_rule.pattern.nInnerDstIP_Mask);
			} else if (pce_rule.pattern.eInnerDstIP_Select == GSW_PCE_IP_V6) {
				printf("\n\tp.nInnerDstIP                                      = ");

				for (i = 0; i < 8; i++) {
					if (i == 7)
						printf("%x", pce_rule.pattern.nInnerDstIP.nIPv6[i]);
					else
						printf("%x:", pce_rule.pattern.nInnerDstIP.nIPv6[i]);
				}

				printf("\n\tp.nInnerDstIP_Mask                                 = 0x%x", pce_rule.pattern.nInnerDstIP_Mask);
			}
		}

		if (pce_rule.pattern.eSrcIP_Select) {
			printf("\n\tp.eSrcIP_Select                                    = %u", pce_rule.pattern.eSrcIP_Select);
			printf("\n\tp.bSrcIP_Exclude                                   = %u", pce_rule.pattern.bSrcIP_Exclude);

			if (pce_rule.pattern.eSrcIP_Select == GSW_PCE_IP_V4) {
				printf("\n\tp.nSrcIP                                           = 0x%x", pce_rule.pattern.nSrcIP.nIPv4);
				printf("\n\tp.nSrcIP_Mask                                      = 0x%x", pce_rule.pattern.nSrcIP_Mask);
			} else if (pce_rule.pattern.eSrcIP_Select == GSW_PCE_IP_V6) {
				printf("\n\tp.nSrcIP                                           = ");

				for (i = 0; i < 8; i++) {
					if (i == 7)
						printf("%x", pce_rule.pattern.nSrcIP.nIPv6[i]);
					else
						printf("%x:", pce_rule.pattern.nSrcIP.nIPv6[i]);
				}

				printf("\n\tp.nSrcIP_Mask                                      = 0x%x", pce_rule.pattern.nSrcIP_Mask);
			}
		}

		if (pce_rule.pattern.eInnerSrcIP_Select) {
			printf("\n\tp.eInnerSrcIP_Select                               = %u", pce_rule.pattern.eInnerSrcIP_Select);
			printf("\n\tp.bInnerSrcIP_Exclude                              = %u", pce_rule.pattern.bInnerSrcIP_Exclude);

			if (pce_rule.pattern.eInnerSrcIP_Select == GSW_PCE_IP_V4) {
				printf("\n\tp.nInnerSrcIP                                      = 0x%x", pce_rule.pattern.nInnerSrcIP.nIPv4);
				printf("\n\tp.nInnerSrcIP_Mask                                 = 0x%x", pce_rule.pattern.nInnerSrcIP_Mask);
			} else if (pce_rule.pattern.eInnerSrcIP_Select == GSW_PCE_IP_V6) {
				printf("\n\tp.nInnerSrcIP                                      = ");

				for (i = 0; i < 8; i++) {
					if (i == 7)
						printf("%x", pce_rule.pattern.nInnerSrcIP.nIPv6[i]);
					else
						printf("%x:", pce_rule.pattern.nInnerSrcIP.nIPv6[i]);
				}

				printf("\n\tp.nInnerSrcIP_Mask                                 = 0x%x", pce_rule.pattern.nInnerSrcIP_Mask);
			}
		}

		if (pce_rule.pattern.bVid) {
			printf("\n\tp.bVid                                             = %u", pce_rule.pattern.bVid);
			printf("\n\tp.bVid_Exclude                                     = %u", pce_rule.pattern.bVid_Exclude);
			printf("\n\tp.nVid                                             = %u", pce_rule.pattern.nVid);

			if (pce_rule.pattern.bVidRange_Select)
				printf("\n\tp.bVidRange_Select                                 = %u (Range Key)", pce_rule.pattern.bVidRange_Select);
			else
				printf("\n\tp.bVidRange_Select                                 = %u (Mask Key)", pce_rule.pattern.bVidRange_Select);

			printf("\n\tp.nVidRange                                        = %u", pce_rule.pattern.nVidRange);
			printf("\n\tp.bVid_Original                                    = %u", pce_rule.pattern.bVid_Original);
		}

		if (pce_rule.pattern.bSLAN_Vid) {
			printf("\n\tp.bSLAN_Vid                                        = %u", pce_rule.pattern.bSLAN_Vid);
			printf("\n\tp.bSLANVid_Exclude                                 = %u", pce_rule.pattern.bSLANVid_Exclude);
			printf("\n\tp.nSLAN_Vid                                        = %u", pce_rule.pattern.nSLAN_Vid);

			if (pce_rule.pattern.bSVidRange_Select)
				printf("\n\tp.bSVidRange_Select                                = %u (Range Key)", pce_rule.pattern.bSVidRange_Select);
			else
				printf("\n\tp.bSVidRange_Select                                = %u (Mask Key)", pce_rule.pattern.bSVidRange_Select);

			printf("\n\tp.nOuterVidRange                                   = %u", pce_rule.pattern.nOuterVidRange);
			printf("\n\tp.bOuterVid_Original                               = %u", pce_rule.pattern.bOuterVid_Original);
		}

		if (pce_rule.pattern.bPortIdEnable) {
			printf("\n\tp.bPortIdEnable                                    = %u", pce_rule.pattern.bPortIdEnable);
			printf("\n\tp.bPortId_Exclude                                  = %u", pce_rule.pattern.bPortId_Exclude);
			printf("\n\tp.nPortId                                          = %u", pce_rule.pattern.nPortId);
		}

		if (pce_rule.pattern.bSubIfIdEnable) {
			printf("\n\tp.bSubIfIdEnable                                   = %u", pce_rule.pattern.bSubIfIdEnable);
			printf("\n\tp.bSubIfId_Exclude                                 = %u", pce_rule.pattern.bSubIfId_Exclude);
			printf("\n\tp.eSubIfIdType                                     = %u", pce_rule.pattern.eSubIfIdType);
			printf("\n\tp.nSubIfId                                         = %u", pce_rule.pattern.nSubIfId);
		}

		if (pce_rule.pattern.bPktLngEnable) {
			printf("\n\tp.bPktLngEnable                                    = %u", pce_rule.pattern.bPktLngEnable);
			printf("\n\tp.bPktLng_Exclude                                  = %u", pce_rule.pattern.bPktLng_Exclude);
			printf("\n\tp.nPktLng                                          = %u", pce_rule.pattern.nPktLng);
			printf("\n\tp.nPktLngRange                                     = %u", pce_rule.pattern.nPktLngRange);
		}

		if (pce_rule.pattern.bPayload1_SrcEnable) {
			printf("\n\tp.bPayload1_Exclude                                = %u", pce_rule.pattern.bPayload1_Exclude);
			printf("\n\tp.nPayload1                                        = 0x%x", pce_rule.pattern.nPayload1);
			printf("\n\tp.bPayload1MaskRange_Select                        = %u", pce_rule.pattern.bPayload1MaskRange_Select);
			printf("\n\tp.nPayload1_Mask                                   = 0x%x", pce_rule.pattern.nPayload1_Mask);
		}

		if (pce_rule.pattern.bPayload2_SrcEnable) {
			printf("\n\tp.bPayload2_Exclude                                = %u", pce_rule.pattern.bPayload2_Exclude);
			printf("\n\tp.nPayload2                                        = 0x%x", pce_rule.pattern.nPayload2);
			printf("\n\tp.bPayload2MaskRange_Select                        = %u", pce_rule.pattern.bPayload2MaskRange_Select);
			printf("\n\tp.nPayload2_Mask                                   = 0x%x", pce_rule.pattern.nPayload2_Mask);
		}

		if (pce_rule.pattern.bParserFlagLSB_Enable) {
			printf("\n\tp.bParserFlagLSB_Exclude                           = %u", pce_rule.pattern.bParserFlagLSB_Exclude);
			printf("\n\tp.nParserFlagLSB                                   = 0x%x", pce_rule.pattern.nParserFlagLSB);
			printf("\n\tp.nParserFlagLSB_Mask                              = 0x%x", pce_rule.pattern.nParserFlagLSB_Mask);
		}

		if (pce_rule.pattern.bParserFlagMSB_Enable) {
			printf("\n\tp.bParserFlagMSB_Exclude                           = %u", pce_rule.pattern.bParserFlagMSB_Exclude);
			printf("\n\tp.nParserFlagMSB                                   = 0x%x", pce_rule.pattern.nParserFlagMSB);
			printf("\n\tp.nParserFlagMSB_Mask                              = 0x%x", pce_rule.pattern.nParserFlagMSB_Mask);
		}

		if (pce_rule.pattern.bParserFlag1LSB_Enable) {
			printf("\n\tp.bParserFlag1LSB_Exclude                          = %u", pce_rule.pattern.bParserFlag1LSB_Exclude);
			printf("\n\tp.nParserFlag1LSB                                  = 0x%x", pce_rule.pattern.nParserFlag1LSB);
			printf("\n\tp.nParserFlag1LSB_Mask                             = 0x%x", pce_rule.pattern.nParserFlag1LSB_Mask);
		}

		if (pce_rule.pattern.bParserFlag1MSB_Enable) {
			printf("\n\tp.bParserFlag1MSB_Exclude                          = %u", pce_rule.pattern.bParserFlag1MSB_Exclude);
			printf("\n\tp.nParserFlag1MSB                                  = 0x%x", pce_rule.pattern.nParserFlag1MSB);
			printf("\n\tp.nParserFlag1MSB_Mask                             = 0x%x", pce_rule.pattern.nParserFlag1MSB_Mask);
		}

		if (pce_rule.action.eVLAN_Action) {
			printf("\n\ta.eVLAN_Action				= %u", pce_rule.action.eVLAN_Action);
			printf("\n\ta.nVLAN_Id					= %u", pce_rule.action.nVLAN_Id);
		}

		if (pce_rule.action.eSVLAN_Action) {
			printf("\n\ta.eSVLAN_Action 			= %u", pce_rule.action.eSVLAN_Action);
			printf("\n\ta.nSVLAN_Id 				= %u", pce_rule.action.nSVLAN_Id);
		}

		if (pce_rule.action.eVLAN_CrossAction)
			printf("\n\ta.eVLAN_CrossAction 		= %u", pce_rule.action.eVLAN_CrossAction);

		if (pce_rule.action.bPortBitMapMuxControl)
			printf("\n\ta.bPortBitMapMuxControl 	= %u", pce_rule.action.bPortBitMapMuxControl);

		if (pce_rule.action.bCVLAN_Ignore_Control)
			printf("\n\ta.bCVLAN_Ignore_Control 	= %u", pce_rule.action.bCVLAN_Ignore_Control);

		if (pce_rule.action.eLearningAction)
			printf("\n\ta.eLearningAction                                  = %u", pce_rule.action.eLearningAction);

		if (pce_rule.action.eSnoopingTypeAction)
			printf("\n\ta.eSnoopingTypeAction                              = %u", pce_rule.action.eSnoopingTypeAction);

		if (pce_rule.pattern.bEtherTypeEnable) {
			printf("\n\tp.bEtherType_Exclude                               = 0x%x", pce_rule.pattern.bEtherType_Exclude);
			printf("\n\tp.nEtherType                                       = 0x%x", pce_rule.pattern.nEtherType);
			printf("\n\tp.nEtherTypeMask                                   = 0x%x", pce_rule.pattern.nEtherTypeMask);
		}

		if (pce_rule.pattern.bProtocolEnable) {
			printf("\n\tp.bProtocol_Exclude                                = 0x%x", pce_rule.pattern.bProtocol_Exclude);
			printf("\n\tp.nProtocol                                        = 0x%x", pce_rule.pattern.nProtocol);
			printf("\n\tp.nProtocolMask                                    = 0x%x", pce_rule.pattern.nProtocolMask);
		}

		if (pce_rule.pattern.bInnerProtocolEnable) {
			printf("\n\tp.bInnerProtocol_Exclude                           = 0x%x", pce_rule.pattern.bInnerProtocol_Exclude);
			printf("\n\tp.nInnerProtocol                                   = 0x%x", pce_rule.pattern.nInnerProtocol);
			printf("\n\tp.nInnerProtocolMask                               = 0x%x", pce_rule.pattern.nInnerProtocolMask);
		}

		if (pce_rule.pattern.bSessionIdEnable) {
			printf("\n\tp.bSessionIdEnable                                 = 0x%x", pce_rule.pattern.bSessionIdEnable);
			printf("\n\tp.bSessionId_Exclude                               = 0x%x", pce_rule.pattern.bSessionId_Exclude);
			printf("\n\tp.nSessionId                                       = 0x%x", pce_rule.pattern.nSessionId);
		}

		if (pce_rule.pattern.bPPP_ProtocolEnable) {
			printf("\n\tp.bPPP_Protocol_Exclude                            = 0x%x", pce_rule.pattern.bPPP_Protocol_Exclude);
			printf("\n\tp.nPPP_Protocol                                    = 0x%x", pce_rule.pattern.nPPP_Protocol);
			printf("\n\tp.nPPP_ProtocolMask                                = 0x%x", pce_rule.pattern.nPPP_ProtocolMask);
		}

		if (pce_rule.pattern.bAppDataMSB_Enable) {
			printf("\n\tp.bAppMSB_Exclude                                  = 0x%x", pce_rule.pattern.bAppMSB_Exclude);
			printf("\n\tp.nAppDataMSB                                      = 0x%x", pce_rule.pattern.nAppDataMSB);
			printf("\n\tp.bAppMaskRangeMSB_Select                          = %u", pce_rule.pattern.bAppMaskRangeMSB_Select);
			printf("\n\tp.nAppMaskRangeMSB                                 = 0x%x", pce_rule.pattern.nAppMaskRangeMSB);
		}

		if (pce_rule.pattern.bAppDataLSB_Enable) {
			printf("\n\tp.bAppLSB_Exclude                                  = 0x%x", pce_rule.pattern.bAppLSB_Exclude);
			printf("\n\tp.nAppDataLSB                                      = 0x%x", pce_rule.pattern.nAppDataLSB);
			printf("\n\tp.bAppMaskRangeLSB_Select                          = %u", pce_rule.pattern.bAppMaskRangeLSB_Select);
			printf("\n\tp.nAppMaskRangeLSB                                 = 0x%x", pce_rule.pattern.nAppMaskRangeLSB);
		}

		if (pce_rule.pattern.bDSCP_Enable) {
			printf("\n\tp.bDSCP_Exclude                                    = %u", pce_rule.pattern.bDSCP_Exclude);
			printf("\n\tp.nDSCP                                            = %u", pce_rule.pattern.nDSCP);
		}

		if (pce_rule.pattern.bInner_DSCP_Enable) {
			printf("\n\tp.bInnerDSCP_Exclude                               = %u", pce_rule.pattern.bInnerDSCP_Exclude);
			printf("\n\tp.nInnerDSCP                                       = %u", pce_rule.pattern.nInnerDSCP);
		}

		if (pce_rule.action.bRemarkAction)
			printf("\n\ta.bRemarkAction                                    = Enabled  val = %u", pce_rule.action.bRemarkAction);

		if (pce_rule.action.bRemarkPCP)
			printf("\n\ta.bRemarkPCP                                       = Disabled val = %u", pce_rule.action.bRemarkPCP);

		if (pce_rule.action.bRemarkDSCP)
			printf("\n\ta.bRemarkDSCP                                      = Disabled val = %u", pce_rule.action.bRemarkDSCP);

		if (pce_rule.action.bRemarkClass)
			printf("\n\ta.bRemarkClass                                     = Disabled val = %u", pce_rule.action.bRemarkClass);

		if (pce_rule.action.bRemarkSTAG_PCP)
			printf("\n\ta.bRemarkSTAG_PCP                                  = Disabled val = %u", pce_rule.action.bRemarkSTAG_PCP);

		if (pce_rule.action.bRemarkSTAG_DEI)
			printf("\n\ta.bRemarkSTAG_DEI                                  = Disabled val = %u", pce_rule.action.bRemarkSTAG_DEI);

		if ((pce_rule.action.bRMON_Action) || (pce_rule.action.bFlowID_Action)) {
			printf("\n\ta.nFlowID/nRmon_ID                                 = %u", pce_rule.action.nFlowID);
		}

		if (pce_rule.pattern.bPCP_Enable) {
			printf("\n\tp.bPCP_Enable                                      = %u", pce_rule.pattern.bPCP_Enable);
			printf("\n\tp.bCTAG_PCP_DEI_Exclude                            = %u", pce_rule.pattern.bCTAG_PCP_DEI_Exclude);
			printf("\n\tp.nPCP                                             = %u", pce_rule.pattern.nPCP);
		}

		if (pce_rule.pattern.bSTAG_PCP_DEI_Enable) {
			printf("\n\tp.bSTAG_PCP_DEI_Enable                             = %u", pce_rule.pattern.bSTAG_PCP_DEI_Enable);
			printf("\n\tp.bSTAG_PCP_DEI_Exclude                            = %u", pce_rule.pattern.bSTAG_PCP_DEI_Exclude);
			printf("\n\tp.nSTAG_PCP_DEI                                    = %u", pce_rule.pattern.nSTAG_PCP_DEI);
		}

		if (pce_rule.action.ePortMapAction) {
			printf("\n\ta.ePortMapAction                                   = 0x%x", pce_rule.action.ePortMapAction);

			for (i = 0; i < 8; i++) {
				if (pce_rule.action.nForwardPortMap[i])
					printf("\n\ta.nForwardPortMap[%d]                              = 0x%x", i, pce_rule.action.nForwardPortMap[i]);
			}
		}

		if (pce_rule.action.eTrafficClassAction) {
			printf("\n\ta.eTrafficClassAction                              = %u", pce_rule.action.eTrafficClassAction);
			printf("\n\ta.nTrafficClassAlternate                           = %u", pce_rule.action.nTrafficClassAlternate);
		}

		if (pce_rule.action.bPortTrunkAction) {
			printf("\n\ta.bPortTrunkAction                                 = Enabled");
			printf("\n\ta.bPortLinkSelection                               = %u", pce_rule.action.bPortLinkSelection);
		}

		if (pce_rule.action.bExtendedVlanEnable) {
			printf("\n\ta.bExtendedVlanEnable                              = Enabled");
			printf("\n\ta.nExtendedVlanBlockId                             = %u", pce_rule.action.nExtendedVlanBlockId);
		}

		if (pce_rule.action.ePortFilterType_Action) {
			printf("\n\ta.ePortFilterType_Action                           = %u", pce_rule.action.ePortFilterType_Action);

			for (i = 0; i < 8; i++) {
				if (pce_rule.action.nForwardPortMap[i])
					printf("\n\ta.nForwardPortMap[%d]                              = 0x%x", i,  pce_rule.action.nForwardPortMap[i]);
			}
		}

		if (pce_rule.action.eProcessPath_Action)
			printf("\n\ta.eProcessPath_Action                              = %u", pce_rule.action.eProcessPath_Action);

		if (pce_rule.action.bOamEnable)
			printf("\n\ta.bOamEnable                                       = %u", pce_rule.action.bOamEnable);

		if (pce_rule.action.bExtractEnable)
			printf("\n\ta.bExtractEnable                                   = %u", pce_rule.action.bExtractEnable);

		if (pce_rule.action.bOamEnable || pce_rule.action.bExtractEnable)
			printf("\n\ta.nRecordId                                        = %u", pce_rule.action.nRecordId);

		if (pce_rule.action.eMeterAction) {
			printf("\n\ta.eMeterAction                                     = %u", pce_rule.action.eMeterAction);
			printf("\n\ta.nMeterId                                         = %u", pce_rule.action.nMeterId);
		}

		if (pce_rule.action.bFidEnable)
			printf("\n\ta.nFId                                             = %u", pce_rule.action.nFId);

		if (pce_rule.pattern.bInsertionFlag_Enable)
			printf("\n\tp.nInsertionFlag                                   = %u", pce_rule.pattern.nInsertionFlag);

		if (pce_rule.action.eCrossStateAction == GSW_PCE_ACTION_CROSS_STATE_CROSS)
			printf("\n\tp.eCrossStateAction                                = GSW_PCE_ACTION_CROSS_STATE_CROSS");
		else if (pce_rule.action.eCrossStateAction == GSW_PCE_ACTION_CROSS_STATE_REGULAR)
			printf("\n\tp.eCrossStateAction                                = GSW_PCE_ACTION_CROSS_STATE_REGULAR");
		else
			printf("\n\tp.eCrossStateAction                                = GSW_PCE_ACTION_CROSS_STATE_DISABLE");

		if (gswip_version == GSWIP_3_2) {
			/*Applicable onlt for GSWIP 3.2*/
			printf("\n\tp.bFlexibleField4_ExcludeEnable			= %u", pce_rule.pattern.bFlexibleField4_ExcludeEnable);
			printf("\n\tp.bFlexibleField4_RangeEnable			= %u", pce_rule.pattern.bFlexibleField4_RangeEnable);
			printf("\n\tp.nFlexibleField4_ParserIndex			= %u", pce_rule.pattern.nFlexibleField4_ParserIndex);
			printf("\n\tp.nFlexibleField4_Value                 = %u", pce_rule.pattern.nFlexibleField4_Value);
			printf("\n\tp.nFlexibleField4_MaskOrRange			= %u", pce_rule.pattern.nFlexibleField4_MaskOrRange);

			printf("\n\tp.bFlexibleField3_ExcludeEnable			= %u", pce_rule.pattern.bFlexibleField3_ExcludeEnable);
			printf("\n\tp.bFlexibleField3_RangeEnable			= %u", pce_rule.pattern.bFlexibleField3_RangeEnable);
			printf("\n\tp.nFlexibleField3_ParserIndex			= %u", pce_rule.pattern.nFlexibleField3_ParserIndex);
			printf("\n\tp.nFlexibleField3_Value                 = %u", pce_rule.pattern.nFlexibleField3_Value);
			printf("\n\tp.nFlexibleField3_MaskOrRange			= %u", pce_rule.pattern.nFlexibleField3_MaskOrRange);

			printf("\n\tp.bFlexibleField2_ExcludeEnable			= %u", pce_rule.pattern.bFlexibleField2_ExcludeEnable);
			printf("\n\tp.bFlexibleField2_RangeEnable			= %u", pce_rule.pattern.bFlexibleField2_RangeEnable);
			printf("\n\tp.nFlexibleField2_ParserIndex			= %u", pce_rule.pattern.nFlexibleField2_ParserIndex);
			printf("\n\tp.nFlexibleField2_Value                 = %u", pce_rule.pattern.nFlexibleField2_Value);
			printf("\n\tp.nFlexibleField2_MaskOrRange			= %u", pce_rule.pattern.nFlexibleField2_MaskOrRange);

			printf("\n\tp.bFlexibleField1_ExcludeEnable			= %u", pce_rule.pattern.bFlexibleField1_ExcludeEnable);
			printf("\n\tp.bFlexibleField1_RangeEnable			= %u", pce_rule.pattern.bFlexibleField1_RangeEnable);
			printf("\n\tp.nFlexibleField1_ParserIndex			= %u", pce_rule.pattern.nFlexibleField1_ParserIndex);
			printf("\n\tp.nFlexibleField1_Value                 = %u", pce_rule.pattern.nFlexibleField1_Value);
			printf("\n\tp.nFlexibleField1_MaskOrRange			= %u", pce_rule.pattern.nFlexibleField1_MaskOrRange);

			if (pce_rule.action.sPBB_Action.bIheaderActionEnable) {
				printf("\n\ta.sPBB_Action.bIheaderActionEnable			= %u", pce_rule.action.sPBB_Action.bIheaderActionEnable);

				switch (pce_rule.action.sPBB_Action.eIheaderOpMode) {
				case GSW_PCE_I_HEADER_OPERATION_INSERT :
					printf("\n\ta.sPBB_Action.eIheaderOpMode				= GSW_PCE_I_HEADER_OPERATION_INSERT");
					printf("\n\ta.sPBB_Action.nTunnelIdKnownTraffic			= %u", pce_rule.action.sPBB_Action.nTunnelIdKnownTraffic);
					printf("\n\ta.sPBB_Action.nTunnelIdUnKnownTraffic		= %u", pce_rule.action.sPBB_Action.nTunnelIdUnKnownTraffic);
					printf("\n\ta.sPBB_Action.bB_DstMac_FromMacTableEnable	= %u", pce_rule.action.sPBB_Action.bB_DstMac_FromMacTableEnable);

					break;

				case GSW_PCE_I_HEADER_OPERATION_REPLACE :
					printf("\n\ta.sPBB_Action.eIheaderOpMode				= GSW_PCE_I_HEADER_OPERATION_REPLACE");
					printf("\n\ta.sPBB_Action.nTunnelIdKnownTraffic			= %u", pce_rule.action.sPBB_Action.nTunnelIdKnownTraffic);
					printf("\n\ta.sPBB_Action.nTunnelIdUnKnownTraffic		= %u", pce_rule.action.sPBB_Action.nTunnelIdUnKnownTraffic);
					printf("\n\ta.sPBB_Action.bReplace_B_SrcMacEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_SrcMacEnable);

					printf("\n\ta.sPBB_Action.bReplace_B_DstMacEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_DstMacEnable);
					printf("\n\ta.sPBB_Action.bReplace_I_TAG_ResEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_ResEnable);
					printf("\n\ta.sPBB_Action.bReplace_I_TAG_UacEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_UacEnable);
					printf("\n\ta.sPBB_Action.bReplace_I_TAG_DeiEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_DeiEnable);
					printf("\n\ta.sPBB_Action.bReplace_I_TAG_PcpEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_PcpEnable);
					printf("\n\ta.sPBB_Action.bReplace_I_TAG_SidEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_SidEnable);
					printf("\n\ta.sPBB_Action.bReplace_I_TAG_TpidEnable		= %u", pce_rule.action.sPBB_Action.bReplace_I_TAG_TpidEnable);

					break;

				case GSW_PCE_I_HEADER_OPERATION_REMOVE :
					printf("\n\ta.sPBB_Action.eIheaderOpMode				= GSW_PCE_I_HEADER_OPERATION_REMOVE");

					break;

				case GSW_PCE_I_HEADER_OPERATION_NOCHANGE :
					printf("\n\ta.sPBB_Action.eIheaderOpMode				= GSW_PCE_I_HEADER_OPERATION_NOCHANGE");

					break;

				default:
					break;
				}
			}

			/*Applicable onlt for GSWIP 3.2*/
			if (pce_rule.action.sPBB_Action.bBtagActionEnable) {
				printf("\n\ta.sPBB_Action.bBtagActionEnable 			= %u", pce_rule.action.sPBB_Action.bBtagActionEnable);

				switch (pce_rule.action.sPBB_Action.eBtagOpMode) {
				case GSW_PCE_B_TAG_OPERATION_INSERT :
					printf("\n\ta.sPBB_Action.eBtagOpMode					= GSW_PCE_B_TAG_OPERATION_INSERT");
					printf("\n\ta.sPBB_Action.nProcessIdKnownTraffic		= %u", pce_rule.action.sPBB_Action.nProcessIdKnownTraffic);
					printf("\n\ta.sPBB_Action.nProcessIdUnKnownTraffic		= %u", pce_rule.action.sPBB_Action.nProcessIdUnKnownTraffic);
					break;

				case GSW_PCE_B_TAG_OPERATION_REPLACE :
					printf("\n\ta.sPBB_Action.eBtagOpMode					= GSW_PCE_B_TAG_OPERATION_REPLACE");
					printf("\n\ta.sPBB_Action.nProcessIdKnownTraffic		= %u", pce_rule.action.sPBB_Action.nProcessIdKnownTraffic);
					printf("\n\ta.sPBB_Action.nProcessIdUnKnownTraffic		= %u", pce_rule.action.sPBB_Action.nProcessIdUnKnownTraffic);
					printf("\n\ta.sPBB_Action.bReplace_B_TAG_DeiEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_TAG_DeiEnable);
					printf("\n\ta.sPBB_Action.bReplace_B_TAG_PcpEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_TAG_PcpEnable);
					printf("\n\ta.sPBB_Action.bReplace_B_TAG_VidEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_TAG_VidEnable);
					printf("\n\ta.sPBB_Action.bReplace_B_TAG_TpidEnable		= %u", pce_rule.action.sPBB_Action.bReplace_B_TAG_TpidEnable);
					break;

				case GSW_PCE_B_TAG_OPERATION_REMOVE :
					printf("\n\ta.sPBB_Action.eBtagOpMode					= GSW_PCE_B_TAG_OPERATION_REMOVE");
					break;

				case GSW_PCE_B_TAG_OPERATION_NOCHANGE :
					printf("\n\ta.sPBB_Action.eBtagOpMode					= GSW_PCE_B_TAG_OPERATION_NOCHANGE");
					break;

				default:
					break;
				}
			}

			/*Applicable onlt for GSWIP 3.2*/
			if (pce_rule.action.sPBB_Action.bMacTableMacinMacActionEnable) {
				printf("\n\ta.sPBB_Action.bMacTableMacinMacActionEnable 			= %u", pce_rule.action.sPBB_Action.bMacTableMacinMacActionEnable);

				switch (pce_rule.action.sPBB_Action.eMacTableMacinMacSelect) {
				case GSW_PCE_OUTER_MAC_SELECTED :
					printf("\n\ta.sPBB_Action.eMacTableMacinMacSelect					= GSW_PCE_OUTER_MAC_SELECTED");
					break;

				case GSW_PCE_INNER_MAC_SELECTED :
					printf("\n\ta.sPBB_Action.eMacTableMacinMacSelect					= GSW_PCE_INNER_MAC_SELECTED");
					break;

				default:
					break;
				}
			}

			/*Applicable onlt for GSWIP 3.2*/
			if (gswip_version == GSWIP_3_2) {
				printf("\n\tsDestSubIF_Action.bDestSubIFIDActionEnable 			= %u", pce_rule.action.sDestSubIF_Action.bDestSubIFIDActionEnable);
				printf("\n\tsDestSubIF_Action.bDestSubIFIDAssignmentEnable 		= %u", pce_rule.action.sDestSubIF_Action.bDestSubIFIDAssignmentEnable);
				printf("\n\tsDestSubIF_Action.nDestSubIFGrp_Field 			    = %u", pce_rule.action.sDestSubIF_Action.nDestSubIFGrp_Field);
			}



		}
	} else {
		printf("\n\tp.nIndex rule not set at	= %u", pce_rule.pattern.nIndex);
	}

	printf("\n");

	return 0;
}

int gsw_pce_rule_write(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_rule_t pce_rule;
	int cnt = 0;

	memset(&pce_rule, 0, sizeof(pce_rule));
	cnt += scanParamArg(argc, argv, "nRegion", sizeof(pce_rule.region), &pce_rule.region);
	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(pce_rule.logicalportid), &pce_rule.logicalportid);
	cnt += scanParamArg(argc, argv, "nSubIfIdGroup", sizeof(pce_rule.subifidgroup), &pce_rule.subifidgroup);
	cnt += scanParamArg(argc, argv, "pattern.nIndex", sizeof(pce_rule.pattern.nIndex), &pce_rule.pattern.nIndex);
	cnt += scanParamArg(argc, argv, "pattern.bEnable", sizeof(pce_rule.pattern.bEnable), &pce_rule.pattern.bEnable);
	cnt += scanParamArg(argc, argv, "pattern.bPortIdEnable", sizeof(pce_rule.pattern.bPortIdEnable), &pce_rule.pattern.bPortIdEnable);
	cnt += scanParamArg(argc, argv, "pattern.nPortId", sizeof(pce_rule.pattern.nPortId), &pce_rule.pattern.nPortId);
	cnt += scanParamArg(argc, argv, "pattern.bPortId_Exclude", sizeof(pce_rule.pattern.bPortId_Exclude), &pce_rule.pattern.bPortId_Exclude);
	cnt += scanParamArg(argc, argv, "pattern.bSubIfIdEnable", sizeof(pce_rule.pattern.bSubIfIdEnable), &pce_rule.pattern.bSubIfIdEnable);
	cnt += scanParamArg(argc, argv, "pattern.nSubIfId", sizeof(pce_rule.pattern.nSubIfId), &pce_rule.pattern.nSubIfId);
	cnt += scanParamArg(argc, argv, "pattern.eSubIfIdType", sizeof(pce_rule.pattern.eSubIfIdType), &pce_rule.pattern.eSubIfIdType);
	cnt += scanParamArg(argc, argv, "pattern.bSubIfId_Exclude", sizeof(pce_rule.pattern.bSubIfId_Exclude), &pce_rule.pattern.bSubIfId_Exclude);
	cnt += scanParamArg(argc, argv, "pattern.bInsertionFlag_Enable", sizeof(pce_rule.pattern.bInsertionFlag_Enable), &pce_rule.pattern.bInsertionFlag_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nInsertionFlag", sizeof(pce_rule.pattern.nInsertionFlag), &pce_rule.pattern.nInsertionFlag);
	cnt += scanParamArg(argc, argv, "pattern.bDSCP_Enable", sizeof(pce_rule.pattern.bDSCP_Enable), &pce_rule.pattern.bDSCP_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nDSCP", sizeof(pce_rule.pattern.nDSCP), &pce_rule.pattern.nDSCP);
	cnt += scanParamArg(argc, argv, "pattern.bDSCP_Exclude", sizeof(pce_rule.pattern.bDSCP_Exclude), &pce_rule.pattern.bDSCP_Exclude);
	cnt += scanParamArg(argc, argv, "pattern.bInner_DSCP_Enable", sizeof(pce_rule.pattern.bInner_DSCP_Enable), &pce_rule.pattern.bInner_DSCP_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nInnerDSCP", sizeof(pce_rule.pattern.nInnerDSCP), &pce_rule.pattern.nInnerDSCP);
	cnt += scanParamArg(argc, argv, "pattern.bInnerDSCP_Exclude", sizeof(pce_rule.pattern.bInnerDSCP_Exclude), &pce_rule.pattern.bInnerDSCP_Exclude);
	cnt += scanParamArg(argc, argv, "pattern.bPCP_Enable", sizeof(pce_rule.pattern.bPCP_Enable), &pce_rule.pattern.bPCP_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nPCP", sizeof(pce_rule.pattern.nPCP), &pce_rule.pattern.nPCP);
	cnt += scanParamArg(argc, argv, "pattern.bCTAG_PCP_DEI_Exclude", sizeof(pce_rule.pattern.bCTAG_PCP_DEI_Exclude), &pce_rule.pattern.bCTAG_PCP_DEI_Exclude);
	cnt += scanParamArg(argc, argv, "pattern.bSTAG_PCP_DEI_Enable", sizeof(pce_rule.pattern.bSTAG_PCP_DEI_Enable), &pce_rule.pattern.bSTAG_PCP_DEI_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nSTAG_PCP_DEI", sizeof(pce_rule.pattern.nSTAG_PCP_DEI), &pce_rule.pattern.nSTAG_PCP_DEI);
	cnt += scanParamArg(argc, argv, "pattern.bSTAG_PCP_DEI_Exclude", sizeof(pce_rule.pattern.bSTAG_PCP_DEI_Exclude), &pce_rule.pattern.bSTAG_PCP_DEI_Exclude);
	cnt += scanParamArg(argc, argv, "pattern.bPktLngEnable", sizeof(pce_rule.pattern.bPktLngEnable), &pce_rule.pattern.bPktLngEnable);
	cnt += scanParamArg(argc, argv, "pattern.nPktLng", sizeof(pce_rule.pattern.nPktLng), &pce_rule.pattern.nPktLng);
	cnt += scanParamArg(argc, argv, "pattern.nPktLngRange", sizeof(pce_rule.pattern.nPktLngRange), &pce_rule.pattern.nPktLngRange);
	cnt += scanParamArg(argc, argv, "pattern.bPktLng_Exclude", sizeof(pce_rule.pattern.bPktLng_Exclude), &pce_rule.pattern.bPktLng_Exclude);
	cnt += scanParamArg(argc, argv, "pattern.bMAC_DstEnable", sizeof(pce_rule.pattern.bMAC_DstEnable), &pce_rule.pattern.bMAC_DstEnable);
	cnt += scanMAC_Arg(argc, argv, "pattern.nMAC_Dst", pce_rule.pattern.nMAC_Dst);
	cnt += scanParamArg(argc, argv, "pattern.nMAC_DstMask", sizeof(pce_rule.pattern.nMAC_DstMask), &pce_rule.pattern.nMAC_DstMask);
	cnt += scanParamArg(argc, argv, "pattern.bDstMAC_Exclude", sizeof(pce_rule.pattern.bDstMAC_Exclude), &pce_rule.pattern.bDstMAC_Exclude);
	cnt += scanParamArg(argc, argv, "pattern.bMAC_SrcEnable", sizeof(pce_rule.pattern.bMAC_SrcEnable), &pce_rule.pattern.bMAC_SrcEnable);
	cnt += scanMAC_Arg(argc, argv, "pattern.nMAC_Src", pce_rule.pattern.nMAC_Src);
	cnt += scanParamArg(argc, argv, "pattern.nMAC_SrcMask", sizeof(pce_rule.pattern.nMAC_SrcMask), &pce_rule.pattern.nMAC_SrcMask);
	cnt += scanParamArg(argc, argv, "pattern.bSrcMAC_Exclude", sizeof(pce_rule.pattern.bSrcMAC_Exclude), &pce_rule.pattern.bSrcMAC_Exclude);
	cnt += scanParamArg(argc, argv, "pattern.bAppDataMSB_Enable", sizeof(pce_rule.pattern.bAppDataMSB_Enable), &pce_rule.pattern.bAppDataMSB_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nAppDataMSB", sizeof(pce_rule.pattern.nAppDataMSB), &pce_rule.pattern.nAppDataMSB);
	cnt += scanParamArg(argc, argv, "pattern.bAppMaskRangeMSB_Select", sizeof(pce_rule.pattern.bAppMaskRangeMSB_Select), &pce_rule.pattern.bAppMaskRangeMSB_Select);
	cnt += scanParamArg(argc, argv, "pattern.nAppMaskRangeMSB", sizeof(pce_rule.pattern.nAppMaskRangeMSB), &pce_rule.pattern.nAppMaskRangeMSB);
	cnt += scanParamArg(argc, argv, "pattern.bAppMSB_Exclude", sizeof(pce_rule.pattern.bAppMSB_Exclude), &pce_rule.pattern.bAppMSB_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bAppDataLSB_Enable", sizeof(pce_rule.pattern.bAppDataLSB_Enable), &pce_rule.pattern.bAppDataLSB_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nAppDataLSB", sizeof(pce_rule.pattern.nAppDataLSB), &pce_rule.pattern.nAppDataLSB);
	cnt += scanParamArg(argc, argv, "pattern.bAppMaskRangeLSB_Select", sizeof(pce_rule.pattern.bAppMaskRangeLSB_Select), &pce_rule.pattern.bAppMaskRangeLSB_Select);
	cnt += scanParamArg(argc, argv, "pattern.nAppMaskRangeLSB", sizeof(pce_rule.pattern.nAppMaskRangeLSB), &pce_rule.pattern.nAppMaskRangeLSB);
	cnt += scanParamArg(argc, argv, "pattern.bAppLSB_Exclude", sizeof(pce_rule.pattern.bAppLSB_Exclude), &pce_rule.pattern.bAppLSB_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.eDstIP_Select", sizeof(pce_rule.pattern.eDstIP_Select), &pce_rule.pattern.eDstIP_Select);

	if (pce_rule.pattern.eDstIP_Select == GSW_PCE_IP_V4)
		cnt += scanIPv4_Arg(argc, argv, "pattern.nDstIP", &pce_rule.pattern.nDstIP.nIPv4);
	else if (pce_rule.pattern.eDstIP_Select == GSW_PCE_IP_V6)
		cnt += scanIPv6_Arg(argc, argv, "pattern.nDstIP", pce_rule.pattern.nDstIP.nIPv6);

	cnt += scanParamArg(argc, argv, "pattern.nDstIP_Mask", sizeof(pce_rule.pattern.nDstIP_Mask), &pce_rule.pattern.nDstIP_Mask);
	cnt += scanParamArg(argc, argv, "pattern.bDstIP_Exclude", sizeof(pce_rule.pattern.bDstIP_Exclude), &pce_rule.pattern.bDstIP_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.eInnerDstIP_Select", sizeof(pce_rule.pattern.eInnerDstIP_Select), &pce_rule.pattern.eInnerDstIP_Select);

	if (pce_rule.pattern.eInnerDstIP_Select == GSW_PCE_IP_V4)
		cnt += scanIPv4_Arg(argc, argv, "pattern.nInnerDstIP", &pce_rule.pattern.nInnerDstIP.nIPv4);
	else if (pce_rule.pattern.eInnerDstIP_Select == GSW_PCE_IP_V6)
		cnt += scanIPv6_Arg(argc, argv, "pattern.nInnerDstIP", pce_rule.pattern.nInnerDstIP.nIPv6);

	cnt += scanParamArg(argc, argv, "pattern.nInnerDstIP_Mask", sizeof(pce_rule.pattern.nInnerDstIP_Mask), &pce_rule.pattern.nInnerDstIP_Mask);
	cnt += scanParamArg(argc, argv, "pattern.bInnerDstIP_Exclude", sizeof(pce_rule.pattern.bInnerDstIP_Exclude), &pce_rule.pattern.bInnerDstIP_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.eSrcIP_Select", sizeof(pce_rule.pattern.eSrcIP_Select), &pce_rule.pattern.eSrcIP_Select);

	if (pce_rule.pattern.eSrcIP_Select == GSW_PCE_IP_V4)
		cnt += scanIPv4_Arg(argc, argv, "pattern.nSrcIP", &pce_rule.pattern.nSrcIP.nIPv4);
	else if (pce_rule.pattern.eSrcIP_Select == GSW_PCE_IP_V6)
		cnt += scanIPv6_Arg(argc, argv, "pattern.nSrcIP", pce_rule.pattern.nSrcIP.nIPv6);

	cnt += scanParamArg(argc, argv, "pattern.nSrcIP_Mask", sizeof(pce_rule.pattern.nSrcIP_Mask), &pce_rule.pattern.nSrcIP_Mask);
	cnt += scanParamArg(argc, argv, "pattern.bSrcIP_Exclude", sizeof(pce_rule.pattern.bSrcIP_Exclude), &pce_rule.pattern.bSrcIP_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.eInnerSrcIP_Select", sizeof(pce_rule.pattern.eInnerSrcIP_Select), &pce_rule.pattern.eInnerSrcIP_Select);

	if (pce_rule.pattern.eInnerSrcIP_Select == GSW_PCE_IP_V4)
		cnt += scanIPv4_Arg(argc, argv, "pattern.nInnerSrcIP", &pce_rule.pattern.nInnerSrcIP.nIPv4);
	else if (pce_rule.pattern.eInnerSrcIP_Select == GSW_PCE_IP_V6)
		cnt += scanIPv6_Arg(argc, argv, "pattern.nInnerSrcIP", pce_rule.pattern.nInnerSrcIP.nIPv6);

	cnt += scanParamArg(argc, argv, "pattern.nInnerSrcIP_Mask", sizeof(pce_rule.pattern.nInnerSrcIP_Mask), &pce_rule.pattern.nInnerSrcIP_Mask);
	cnt += scanParamArg(argc, argv, "pattern.bInnerSrcIP_Exclude", sizeof(pce_rule.pattern.bInnerSrcIP_Exclude), &pce_rule.pattern.bInnerSrcIP_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bEtherTypeEnable", sizeof(pce_rule.pattern.bEtherTypeEnable), &pce_rule.pattern.bEtherTypeEnable);
	cnt += scanParamArg(argc, argv, "pattern.nEtherType", sizeof(pce_rule.pattern.nEtherType), &pce_rule.pattern.nEtherType);
	cnt += scanParamArg(argc, argv, "pattern.nEtherTypeMask", sizeof(pce_rule.pattern.nEtherTypeMask), &pce_rule.pattern.nEtherTypeMask);
	cnt += scanParamArg(argc, argv, "pattern.bEtherType_Exclude", sizeof(pce_rule.pattern.bEtherType_Exclude), &pce_rule.pattern.bEtherType_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bProtocolEnable", sizeof(pce_rule.pattern.bProtocolEnable), &pce_rule.pattern.bProtocolEnable);
	cnt += scanParamArg(argc, argv, "pattern.nProtocol", sizeof(pce_rule.pattern.nProtocol), &pce_rule.pattern.nProtocol);
	cnt += scanParamArg(argc, argv, "pattern.nProtocolMask", sizeof(pce_rule.pattern.nProtocolMask), &pce_rule.pattern.nProtocolMask);
	cnt += scanParamArg(argc, argv, "pattern.bProtocol_Exclude", sizeof(pce_rule.pattern.bProtocol_Exclude), &pce_rule.pattern.bProtocol_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bInnerProtocolEnable", sizeof(pce_rule.pattern.bInnerProtocolEnable), &pce_rule.pattern.bInnerProtocolEnable);
	cnt += scanParamArg(argc, argv, "pattern.nInnerProtocol", sizeof(pce_rule.pattern.nInnerProtocol), &pce_rule.pattern.nInnerProtocol);
	cnt += scanParamArg(argc, argv, "pattern.nInnerProtocolMask", sizeof(pce_rule.pattern.nInnerProtocolMask), &pce_rule.pattern.nInnerProtocolMask);
	cnt += scanParamArg(argc, argv, "pattern.bInnerProtocol_Exclude", sizeof(pce_rule.pattern.bInnerProtocol_Exclude), &pce_rule.pattern.bInnerProtocol_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bSessionIdEnable", sizeof(pce_rule.pattern.bSessionIdEnable), &pce_rule.pattern.bSessionIdEnable);
	cnt += scanParamArg(argc, argv, "pattern.nSessionId", sizeof(pce_rule.pattern.nSessionId), &pce_rule.pattern.nSessionId);
	cnt += scanParamArg(argc, argv, "pattern.bSessionId_Exclude", sizeof(pce_rule.pattern.bSessionId_Exclude), &pce_rule.pattern.bSessionId_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bPPP_ProtocolEnable", sizeof(pce_rule.pattern.bPPP_ProtocolEnable), &pce_rule.pattern.bPPP_ProtocolEnable);
	cnt += scanParamArg(argc, argv, "pattern.nPPP_Protocol", sizeof(pce_rule.pattern.nPPP_Protocol), &pce_rule.pattern.nPPP_Protocol);
	cnt += scanParamArg(argc, argv, "pattern.nPPP_ProtocolMask", sizeof(pce_rule.pattern.nPPP_ProtocolMask), &pce_rule.pattern.nPPP_ProtocolMask);
	cnt += scanParamArg(argc, argv, "pattern.bPPP_Protocol_Exclude", sizeof(pce_rule.pattern.bPPP_Protocol_Exclude), &pce_rule.pattern.bPPP_Protocol_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bVid", sizeof(pce_rule.pattern.bVid), &pce_rule.pattern.bVid);
	cnt += scanParamArg(argc, argv, "pattern.nVid", sizeof(pce_rule.pattern.nVid), &pce_rule.pattern.nVid);
	cnt += scanParamArg(argc, argv, "pattern.bVidRange_Select", sizeof(pce_rule.pattern.bVidRange_Select), &pce_rule.pattern.bVidRange_Select);
	cnt += scanParamArg(argc, argv, "pattern.nVidRange", sizeof(pce_rule.pattern.nVidRange), &pce_rule.pattern.nVidRange);
	cnt += scanParamArg(argc, argv, "pattern.bVid_Exclude", sizeof(pce_rule.pattern.bVid_Exclude), &pce_rule.pattern.bVid_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bSLAN_Vid", sizeof(pce_rule.pattern.bSLAN_Vid), &pce_rule.pattern.bSLAN_Vid);
	cnt += scanParamArg(argc, argv, "pattern.nSLAN_Vid", sizeof(pce_rule.pattern.nSLAN_Vid), &pce_rule.pattern.nSLAN_Vid);
	cnt += scanParamArg(argc, argv, "pattern.bSLANVid_Exclude", sizeof(pce_rule.pattern.bSLANVid_Exclude), &pce_rule.pattern.bSLANVid_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bPayload1_SrcEnable", sizeof(pce_rule.pattern.bPayload1_SrcEnable), &pce_rule.pattern.bPayload1_SrcEnable);
	cnt += scanParamArg(argc, argv, "pattern.nPayload1", sizeof(pce_rule.pattern.nPayload1), &pce_rule.pattern.nPayload1);
	cnt += scanParamArg(argc, argv, "pattern.bPayload1MaskRange_Select", sizeof(pce_rule.pattern.bPayload1MaskRange_Select), &pce_rule.pattern.bPayload1MaskRange_Select);
	cnt += scanParamArg(argc, argv, "pattern.nPayload1_Mask", sizeof(pce_rule.pattern.nPayload1_Mask), &pce_rule.pattern.nPayload1_Mask);
	cnt += scanParamArg(argc, argv, "pattern.bPayload1_Exclude", sizeof(pce_rule.pattern.bPayload1_Exclude), &pce_rule.pattern.bPayload1_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bPayload2_SrcEnable", sizeof(pce_rule.pattern.bPayload2_SrcEnable), &pce_rule.pattern.bPayload2_SrcEnable);
	cnt += scanParamArg(argc, argv, "pattern.nPayload2", sizeof(pce_rule.pattern.nPayload2), &pce_rule.pattern.nPayload2);
	cnt += scanParamArg(argc, argv, "pattern.bPayload2MaskRange_Select", sizeof(pce_rule.pattern.bPayload2MaskRange_Select), &pce_rule.pattern.bPayload2MaskRange_Select);
	cnt += scanParamArg(argc, argv, "pattern.nPayload2_Mask", sizeof(pce_rule.pattern.nPayload2_Mask), &pce_rule.pattern.nPayload2_Mask);
	cnt += scanParamArg(argc, argv, "pattern.bPayload2_Exclude", sizeof(pce_rule.pattern.bPayload2_Exclude), &pce_rule.pattern.bPayload2_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bParserFlagLSB_Enable", sizeof(pce_rule.pattern.bParserFlagLSB_Enable), &pce_rule.pattern.bParserFlagLSB_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nParserFlagLSB", sizeof(pce_rule.pattern.nParserFlagLSB), &pce_rule.pattern.nParserFlagLSB);
	cnt += scanParamArg(argc, argv, "pattern.nParserFlagLSB_Mask", sizeof(pce_rule.pattern.nParserFlagLSB_Mask), &pce_rule.pattern.nParserFlagLSB_Mask);
	cnt += scanParamArg(argc, argv, "pattern.bParserFlagLSB_Exclude", sizeof(pce_rule.pattern.bParserFlagLSB_Exclude), &pce_rule.pattern.bParserFlagLSB_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bParserFlagMSB_Enable", sizeof(pce_rule.pattern.bParserFlagMSB_Enable), &pce_rule.pattern.bParserFlagMSB_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nParserFlagMSB", sizeof(pce_rule.pattern.nParserFlagMSB), &pce_rule.pattern.nParserFlagMSB);
	cnt += scanParamArg(argc, argv, "pattern.nParserFlagMSB_Mask", sizeof(pce_rule.pattern.nParserFlagMSB_Mask), &pce_rule.pattern.nParserFlagMSB_Mask);
	cnt += scanParamArg(argc, argv, "pattern.bParserFlagMSB_Exclude", sizeof(pce_rule.pattern.bParserFlagMSB_Exclude), &pce_rule.pattern.bParserFlagMSB_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bParserFlag1LSB_Enable", sizeof(pce_rule.pattern.bParserFlag1LSB_Enable), &pce_rule.pattern.bParserFlag1LSB_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nParserFlag1LSB", sizeof(pce_rule.pattern.nParserFlag1LSB), &pce_rule.pattern.nParserFlag1LSB);
	cnt += scanParamArg(argc, argv, "pattern.nParserFlag1LSB_Mask", sizeof(pce_rule.pattern.nParserFlag1LSB_Mask), &pce_rule.pattern.nParserFlag1LSB_Mask);
	cnt += scanParamArg(argc, argv, "pattern.bParserFlag1LSB_Exclude", sizeof(pce_rule.pattern.bParserFlag1LSB_Exclude), &pce_rule.pattern.bParserFlag1LSB_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bParserFlag1MSB_Enable", sizeof(pce_rule.pattern.bParserFlag1MSB_Enable), &pce_rule.pattern.bParserFlag1MSB_Enable);
	cnt += scanParamArg(argc, argv, "pattern.nParserFlag1MSB", sizeof(pce_rule.pattern.nParserFlag1MSB), &pce_rule.pattern.nParserFlag1MSB);
	cnt += scanParamArg(argc, argv, "pattern.nParserFlag1MSB_Mask", sizeof(pce_rule.pattern.nParserFlag1MSB_Mask), &pce_rule.pattern.nParserFlag1MSB_Mask);
	cnt += scanParamArg(argc, argv, "pattern.bParserFlag1MSB_Exclude", sizeof(pce_rule.pattern.bParserFlag1MSB_Exclude), &pce_rule.pattern.bParserFlag1MSB_Exclude);

	cnt += scanParamArg(argc, argv, "pattern.bVid_Original",      sizeof(pce_rule.pattern.bVid_Original), &pce_rule.pattern.bVid_Original);
	cnt += scanParamArg(argc, argv, "pattern.nOuterVidRange",     sizeof(pce_rule.pattern.nOuterVidRange), &pce_rule.pattern.nOuterVidRange);
	cnt += scanParamArg(argc, argv, "pattern.bSVidRange_Select",  sizeof(pce_rule.pattern.bSVidRange_Select), &pce_rule.pattern.bSVidRange_Select);
	cnt += scanParamArg(argc, argv, "pattern.bOuterVid_Original", sizeof(pce_rule.pattern.bOuterVid_Original), &pce_rule.pattern.bOuterVid_Original);

	cnt += scanParamArg(argc, argv, "action.eTrafficClassAction", sizeof(pce_rule.action.eTrafficClassAction), &pce_rule.action.eTrafficClassAction);
	cnt += scanParamArg(argc, argv, "action.nTrafficClassAlternate", sizeof(pce_rule.action.nTrafficClassAlternate), &pce_rule.action.nTrafficClassAlternate);
	cnt += scanParamArg(argc, argv, "action.eSnoopingTypeAction", sizeof(pce_rule.action.eSnoopingTypeAction), &pce_rule.action.eSnoopingTypeAction);
	cnt += scanParamArg(argc, argv, "action.eLearningAction", sizeof(pce_rule.action.eLearningAction), &pce_rule.action.eLearningAction);
	cnt += scanParamArg(argc, argv, "action.eIrqAction", sizeof(pce_rule.action.eIrqAction), &pce_rule.action.eIrqAction);
	cnt += scanParamArg(argc, argv, "action.eCrossStateAction", sizeof(pce_rule.action.eCrossStateAction), &pce_rule.action.eCrossStateAction);
	cnt += scanParamArg(argc, argv, "action.eCritFrameAction", sizeof(pce_rule.action.eCritFrameAction), &pce_rule.action.eCritFrameAction);
	cnt += scanParamArg(argc, argv, "action.eTimestampAction", sizeof(pce_rule.action.eTimestampAction), &pce_rule.action.eTimestampAction);
	cnt += scanParamArg(argc, argv, "action.ePortMapAction", sizeof(pce_rule.action.ePortMapAction), &pce_rule.action.ePortMapAction);
	cnt += scanParamArg(argc, argv, "action.nForwardPortMap", sizeof(pce_rule.action.nForwardPortMap[0]), &pce_rule.action.nForwardPortMap[0]);
	cnt += scanParamArg(argc, argv, "action.nForwardPortMap[0]", sizeof(pce_rule.action.nForwardPortMap[0]), &pce_rule.action.nForwardPortMap[0]);
	cnt += scanParamArg(argc, argv, "action.nForwardPortMap[1]", sizeof(pce_rule.action.nForwardPortMap[1]), &pce_rule.action.nForwardPortMap[1]);
	cnt += scanParamArg(argc, argv, "action.nForwardPortMap[2]", sizeof(pce_rule.action.nForwardPortMap[2]), &pce_rule.action.nForwardPortMap[2]);
	cnt += scanParamArg(argc, argv, "action.nForwardPortMap[3]", sizeof(pce_rule.action.nForwardPortMap[3]), &pce_rule.action.nForwardPortMap[3]);
	cnt += scanParamArg(argc, argv, "action.nForwardPortMap[4]", sizeof(pce_rule.action.nForwardPortMap[4]), &pce_rule.action.nForwardPortMap[4]);
	cnt += scanParamArg(argc, argv, "action.nForwardPortMap[5]", sizeof(pce_rule.action.nForwardPortMap[5]), &pce_rule.action.nForwardPortMap[5]);
	cnt += scanParamArg(argc, argv, "action.nForwardPortMap[6]", sizeof(pce_rule.action.nForwardPortMap[6]), &pce_rule.action.nForwardPortMap[6]);
	cnt += scanParamArg(argc, argv, "action.nForwardPortMap[7]", sizeof(pce_rule.action.nForwardPortMap[7]), &pce_rule.action.nForwardPortMap[7]);
	cnt += scanParamArg(argc, argv, "action.nForwardSubIfId", sizeof(pce_rule.action.nForwardSubIfId), &pce_rule.action.nForwardSubIfId);
	cnt += scanParamArg(argc, argv, "action.bRemarkAction", sizeof(pce_rule.action.bRemarkAction), &pce_rule.action.bRemarkAction);
	cnt += scanParamArg(argc, argv, "action.bRemarkPCP", sizeof(pce_rule.action.bRemarkAction), &pce_rule.action.bRemarkPCP);
	cnt += scanParamArg(argc, argv, "action.bRemarkSTAG_PCP", sizeof(pce_rule.action.bRemarkSTAG_PCP), &pce_rule.action.bRemarkSTAG_PCP);
	cnt += scanParamArg(argc, argv, "action.bRemarkSTAG_DEI", sizeof(pce_rule.action.bRemarkSTAG_DEI), &pce_rule.action.bRemarkSTAG_DEI);
	cnt += scanParamArg(argc, argv, "action.bRemarkDSCP", sizeof(pce_rule.action.bRemarkDSCP), &pce_rule.action.bRemarkDSCP);
	cnt += scanParamArg(argc, argv, "action.bRemarkClass", sizeof(pce_rule.action.bRemarkClass), &pce_rule.action.bRemarkClass);
	cnt += scanParamArg(argc, argv, "action.eMeterAction", sizeof(pce_rule.action.eMeterAction), &pce_rule.action.eMeterAction);
	cnt += scanParamArg(argc, argv, "action.nMeterId", sizeof(pce_rule.action.nMeterId), &pce_rule.action.nMeterId);
	cnt += scanParamArg(argc, argv, "action.bRMON_Action", sizeof(pce_rule.action.bRMON_Action), &pce_rule.action.bRMON_Action);
	cnt += scanParamArg(argc, argv, "action.nRMON_Id", sizeof(pce_rule.action.nRMON_Id), &pce_rule.action.nRMON_Id);
	cnt += scanParamArg(argc, argv, "action.eVLAN_Action", sizeof(pce_rule.action.eVLAN_Action), &pce_rule.action.eVLAN_Action);
	cnt += scanParamArg(argc, argv, "action.nVLAN_Id", sizeof(pce_rule.action.nVLAN_Id), &pce_rule.action.nVLAN_Id);
	cnt += scanParamArg(argc, argv, "action.nFId", sizeof(pce_rule.action.nFId), &pce_rule.action.nFId);
	cnt += scanParamArg(argc, argv, "action.bFidEnable", sizeof(pce_rule.action.bFidEnable), &pce_rule.action.bFidEnable);

	cnt += scanParamArg(argc, argv, "action.eSVLAN_Action", sizeof(pce_rule.action.eSVLAN_Action), &pce_rule.action.eSVLAN_Action);
	cnt += scanParamArg(argc, argv, "action.nSVLAN_Id", sizeof(pce_rule.action.nSVLAN_Id), &pce_rule.action.nSVLAN_Id);
	cnt += scanParamArg(argc, argv, "action.eVLAN_CrossAction", sizeof(pce_rule.action.eVLAN_CrossAction), &pce_rule.action.eVLAN_CrossAction);
	cnt += scanParamArg(argc, argv, "action.bPortBitMapMuxControl", sizeof(pce_rule.action.bPortBitMapMuxControl), &pce_rule.action.bPortBitMapMuxControl);
	cnt += scanParamArg(argc, argv, "action.bCVLAN_Ignore_Control", sizeof(pce_rule.action.bCVLAN_Ignore_Control), &pce_rule.action.bCVLAN_Ignore_Control);
	cnt += scanParamArg(argc, argv, "action.bPortLinkSelection", sizeof(pce_rule.action.bPortLinkSelection), &pce_rule.action.bPortLinkSelection);
	cnt += scanParamArg(argc, argv, "action.bPortTrunkAction", sizeof(pce_rule.action.bPortTrunkAction), &pce_rule.action.bPortTrunkAction);

	cnt += scanParamArg(argc, argv, "action.bFlowID_Action", sizeof(pce_rule.action.bFlowID_Action), &pce_rule.action.bFlowID_Action);
	cnt += scanParamArg(argc, argv, "action.nFlowID", sizeof(pce_rule.action.nFlowID), &pce_rule.action.nFlowID);

	cnt += scanParamArg(argc, argv, "action.bRoutExtId_Action", sizeof(pce_rule.action.bRoutExtId_Action), &pce_rule.action.bRoutExtId_Action);
	cnt += scanParamArg(argc, argv, "action.nRoutExtId", sizeof(pce_rule.action.nRoutExtId), &pce_rule.action.nRoutExtId);

	cnt += scanParamArg(argc, argv, "action.bRtDstPortMaskCmp_Action", sizeof(pce_rule.action.bRtDstPortMaskCmp_Action), &pce_rule.action.bRtDstPortMaskCmp_Action);
	cnt += scanParamArg(argc, argv, "action.bRtSrcPortMaskCmp_Action", sizeof(pce_rule.action.bRtSrcPortMaskCmp_Action), &pce_rule.action.bRtSrcPortMaskCmp_Action);
	cnt += scanParamArg(argc, argv, "action.bRtDstIpMaskCmp_Action", sizeof(pce_rule.action.bRtDstIpMaskCmp_Action), &pce_rule.action.bRtDstIpMaskCmp_Action);
	cnt += scanParamArg(argc, argv, "action.bRtSrcIpMaskCmp_Action", sizeof(pce_rule.action.bRtSrcIpMaskCmp_Action), &pce_rule.action.bRtSrcIpMaskCmp_Action);
	cnt += scanParamArg(argc, argv, "action.bRtInnerIPasKey_Action", sizeof(pce_rule.action.bRtInnerIPasKey_Action), &pce_rule.action.bRtInnerIPasKey_Action);

	cnt += scanParamArg(argc, argv, "action.bRtAccelEna_Action", sizeof(pce_rule.action.bRtAccelEna_Action), &pce_rule.action.bRtAccelEna_Action);
	cnt += scanParamArg(argc, argv, "action.bRtCtrlEna_Action", sizeof(pce_rule.action.bRtCtrlEna_Action), &pce_rule.action.bRtCtrlEna_Action);
	cnt += scanParamArg(argc, argv, "action.eProcessPath_Action", sizeof(pce_rule.action.eProcessPath_Action), &pce_rule.action.eProcessPath_Action);
	cnt += scanParamArg(argc, argv, "action.ePortFilterType_Action", sizeof(pce_rule.action.ePortFilterType_Action), &pce_rule.action.ePortFilterType_Action);

	cnt += scanParamArg(argc, argv, "action.bOamEnable", 		   sizeof(pce_rule.action.bOamEnable), &pce_rule.action.bOamEnable);
	cnt += scanParamArg(argc, argv, "action.nRecordId",  		   sizeof(pce_rule.action.nRecordId), &pce_rule.action.nRecordId);
	cnt += scanParamArg(argc, argv, "action.bExtractEnable",  	   sizeof(pce_rule.action.bExtractEnable), &pce_rule.action.bExtractEnable);
	cnt += scanParamArg(argc, argv, "action.eColorFrameAction",    sizeof(pce_rule.action.eColorFrameAction), &pce_rule.action.eColorFrameAction);
	cnt += scanParamArg(argc, argv, "action.bExtendedVlanEnable",  sizeof(pce_rule.action.bExtendedVlanEnable), &pce_rule.action.bExtendedVlanEnable);
	cnt += scanParamArg(argc, argv, "action.nExtendedVlanBlockId", sizeof(pce_rule.action.nExtendedVlanBlockId), &pce_rule.action.nExtendedVlanBlockId);

	/*Aplicable for GSWIP 3.2*/

	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField4Enable",
			    sizeof(&pce_rule.pattern.bFlexibleField4Enable), &pce_rule.pattern.bFlexibleField4Enable);
	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField4_ExcludeEnable",
			    sizeof(&pce_rule.pattern.bFlexibleField4_ExcludeEnable), &pce_rule.pattern.bFlexibleField4_ExcludeEnable);
	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField4_RangeEnable",
			    sizeof(&pce_rule.pattern.bFlexibleField4_RangeEnable), &pce_rule.pattern.bFlexibleField4_RangeEnable);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField4_ParserIndex",
			    sizeof(&pce_rule.pattern.nFlexibleField4_ParserIndex), &pce_rule.pattern.nFlexibleField4_ParserIndex);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField4_Value",
			    sizeof(&pce_rule.pattern.nFlexibleField4_Value), &pce_rule.pattern.nFlexibleField4_Value);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField4_MaskOrRange",
			    sizeof(&pce_rule.pattern.nFlexibleField4_MaskOrRange), &pce_rule.pattern.nFlexibleField4_MaskOrRange);

	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField3Enable",
			    sizeof(&pce_rule.pattern.bFlexibleField3Enable), &pce_rule.pattern.bFlexibleField3Enable);
	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField3_ExcludeEnable",
			    sizeof(&pce_rule.pattern.bFlexibleField3_ExcludeEnable), &pce_rule.pattern.bFlexibleField3_ExcludeEnable);
	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField3_RangeEnable",
			    sizeof(&pce_rule.pattern.bFlexibleField3_RangeEnable), &pce_rule.pattern.bFlexibleField3_RangeEnable);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField3_ParserIndex",
			    sizeof(&pce_rule.pattern.nFlexibleField3_ParserIndex), &pce_rule.pattern.nFlexibleField3_ParserIndex);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField3_Value",
			    sizeof(&pce_rule.pattern.nFlexibleField3_Value), &pce_rule.pattern.nFlexibleField3_Value);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField3_MaskOrRange",
			    sizeof(&pce_rule.pattern.nFlexibleField3_MaskOrRange), &pce_rule.pattern.nFlexibleField3_MaskOrRange);

	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField2Enable",
			    sizeof(&pce_rule.pattern.bFlexibleField2Enable), &pce_rule.pattern.bFlexibleField2Enable);
	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField2_ExcludeEnable",
			    sizeof(&pce_rule.pattern.bFlexibleField2_ExcludeEnable), &pce_rule.pattern.bFlexibleField2_ExcludeEnable);
	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField2_RangeEnable",
			    sizeof(&pce_rule.pattern.bFlexibleField2_RangeEnable), &pce_rule.pattern.bFlexibleField2_RangeEnable);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField2_ParserIndex",
			    sizeof(&pce_rule.pattern.nFlexibleField2_ParserIndex), &pce_rule.pattern.nFlexibleField2_ParserIndex);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField2_Value",
			    sizeof(&pce_rule.pattern.nFlexibleField2_Value), &pce_rule.pattern.nFlexibleField2_Value);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField2_MaskOrRange",
			    sizeof(&pce_rule.pattern.nFlexibleField2_MaskOrRange), &pce_rule.pattern.nFlexibleField2_MaskOrRange);

	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField1Enable",
			    sizeof(&pce_rule.pattern.bFlexibleField1Enable), &pce_rule.pattern.bFlexibleField1Enable);
	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField1_ExcludeEnable",
			    sizeof(&pce_rule.pattern.bFlexibleField1_ExcludeEnable), &pce_rule.pattern.bFlexibleField1_ExcludeEnable);
	cnt += scanParamArg(argc, argv, "pattern.bFlexibleField1_RangeEnable",
			    sizeof(&pce_rule.pattern.bFlexibleField1_RangeEnable), &pce_rule.pattern.bFlexibleField1_RangeEnable);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField1_ParserIndex",
			    sizeof(&pce_rule.pattern.nFlexibleField1_ParserIndex), &pce_rule.pattern.nFlexibleField1_ParserIndex);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField1_Value",
			    sizeof(&pce_rule.pattern.nFlexibleField1_Value), &pce_rule.pattern.nFlexibleField1_Value);
	cnt += scanParamArg(argc, argv, "pattern.nFlexibleField1_MaskOrRange",
			    sizeof(&pce_rule.pattern.nFlexibleField1_MaskOrRange), &pce_rule.pattern.nFlexibleField1_MaskOrRange);

	cnt += scanParamArg(argc, argv, "action.bPBB_Action_Enable", sizeof(pce_rule.action.bPBB_Action_Enable), &pce_rule.action.bPBB_Action_Enable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bIheaderActionEnable",
			    sizeof(pce_rule.action.sPBB_Action.bIheaderActionEnable), &pce_rule.action.sPBB_Action.bIheaderActionEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.eIheaderOpMode",
			    sizeof(pce_rule.action.sPBB_Action.eIheaderOpMode), &pce_rule.action.sPBB_Action.eIheaderOpMode);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bTunnelIdKnownTrafficEnable",
			    sizeof(pce_rule.action.sPBB_Action.bTunnelIdKnownTrafficEnable), &pce_rule.action.sPBB_Action.bTunnelIdKnownTrafficEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.nTunnelIdKnownTraffic",
			    sizeof(pce_rule.action.sPBB_Action.nTunnelIdKnownTraffic), &pce_rule.action.sPBB_Action.nTunnelIdKnownTraffic);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bTunnelIdUnKnownTrafficEnable",
			    sizeof(pce_rule.action.sPBB_Action.bTunnelIdUnKnownTrafficEnable), &pce_rule.action.sPBB_Action.bTunnelIdUnKnownTrafficEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.nTunnelIdUnKnownTraffic",
			    sizeof(pce_rule.action.sPBB_Action.nTunnelIdUnKnownTraffic), &pce_rule.action.sPBB_Action.nTunnelIdUnKnownTraffic);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bB_DstMac_FromMacTableEnable",
			    sizeof(pce_rule.action.sPBB_Action.bB_DstMac_FromMacTableEnable), &pce_rule.action.sPBB_Action.bB_DstMac_FromMacTableEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_B_SrcMacEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_B_SrcMacEnable), &pce_rule.action.sPBB_Action.bReplace_B_SrcMacEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_B_DstMacEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_B_DstMacEnable), &pce_rule.action.sPBB_Action.bReplace_B_DstMacEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_I_TAG_ResEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_I_TAG_ResEnable), &pce_rule.action.sPBB_Action.bReplace_I_TAG_ResEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_I_TAG_UacEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_I_TAG_UacEnable), &pce_rule.action.sPBB_Action.bReplace_I_TAG_UacEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_I_TAG_DeiEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_I_TAG_DeiEnable), &pce_rule.action.sPBB_Action.bReplace_I_TAG_DeiEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_I_TAG_PcpEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_I_TAG_PcpEnable), &pce_rule.action.sPBB_Action.bReplace_I_TAG_PcpEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_I_TAG_SidEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_I_TAG_SidEnable), &pce_rule.action.sPBB_Action.bReplace_I_TAG_SidEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_I_TAG_TpidEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_I_TAG_TpidEnable), &pce_rule.action.sPBB_Action.bReplace_I_TAG_TpidEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bBtagActionEnable",
			    sizeof(pce_rule.action.sPBB_Action.bBtagActionEnable), &pce_rule.action.sPBB_Action.bBtagActionEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.eBtagOpMode",
			    sizeof(pce_rule.action.sPBB_Action.eBtagOpMode), &pce_rule.action.sPBB_Action.eBtagOpMode);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bProcessIdKnownTrafficEnable",
			    sizeof(pce_rule.action.sPBB_Action.bProcessIdKnownTrafficEnable), &pce_rule.action.sPBB_Action.bProcessIdKnownTrafficEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.nProcessIdKnownTraffic",
			    sizeof(pce_rule.action.sPBB_Action.nProcessIdKnownTraffic), &pce_rule.action.sPBB_Action.nProcessIdKnownTraffic);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bProcessIdUnKnownTrafficEnable",
			    sizeof(pce_rule.action.sPBB_Action.bProcessIdUnKnownTrafficEnable), &pce_rule.action.sPBB_Action.bProcessIdUnKnownTrafficEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.nProcessIdUnKnownTraffic",
			    sizeof(pce_rule.action.sPBB_Action.nProcessIdUnKnownTraffic), &pce_rule.action.sPBB_Action.nProcessIdUnKnownTraffic);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_B_TAG_DeiEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_B_TAG_DeiEnable), &pce_rule.action.sPBB_Action.bReplace_B_TAG_DeiEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_B_TAG_PcpEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_B_TAG_PcpEnable), &pce_rule.action.sPBB_Action.bReplace_B_TAG_PcpEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_B_TAG_VidEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_B_TAG_VidEnable), &pce_rule.action.sPBB_Action.bReplace_B_TAG_VidEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bReplace_B_TAG_TpidEnable",
			    sizeof(pce_rule.action.sPBB_Action.bReplace_B_TAG_TpidEnable), &pce_rule.action.sPBB_Action.bReplace_B_TAG_TpidEnable);

	cnt += scanParamArg(argc, argv, "action.sPBB_Action.bMacTableMacinMacActionEnable",
			    sizeof(pce_rule.action.sPBB_Action.bMacTableMacinMacActionEnable), &pce_rule.action.sPBB_Action.bMacTableMacinMacActionEnable);
	cnt += scanParamArg(argc, argv, "action.sPBB_Action.eMacTableMacinMacSelect",
			    sizeof(pce_rule.action.sPBB_Action.eMacTableMacinMacSelect), &pce_rule.action.sPBB_Action.eMacTableMacinMacSelect);

	/*Applicable only for GSWIP V32 */
	if (gswip_version == GSWIP_3_2) {
		cnt += scanParamArg(argc, argv, "action.bDestSubIf_Action_Enable",
				    sizeof(pce_rule.action.bDestSubIf_Action_Enable), &pce_rule.action.bDestSubIf_Action_Enable);
		cnt += scanParamArg(argc, argv, "action.sDestSubIF_Action.bDestSubIFIDActionEnable",
				    sizeof(pce_rule.action.sDestSubIF_Action.bDestSubIFIDActionEnable), &pce_rule.action.sDestSubIF_Action.bDestSubIFIDActionEnable);
		cnt += scanParamArg(argc, argv, "action.sDestSubIF_Action.bDestSubIFIDAssignmentEnable",
				    sizeof(pce_rule.action.sDestSubIF_Action.bDestSubIFIDAssignmentEnable), &pce_rule.action.sDestSubIF_Action.bDestSubIFIDAssignmentEnable);
		cnt += scanParamArg(argc, argv, "action.sDestSubIF_Action.nDestSubIFGrp_Field",
				    sizeof(pce_rule.action.sDestSubIF_Action.nDestSubIFGrp_Field), &pce_rule.action.sDestSubIF_Action.nDestSubIFGrp_Field);
	}

	//printf("cnt = %d numPar = %d................\n", cnt, numPar);

	if (cnt != numPar) return (-2);

	if (cnt == 0) return (-3);

	return cli_ioctl(fd, GSW_PCE_RULE_WRITE, &pce_rule);
}

int gsw_pce_rule_enable(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_rule_t pce_rule;
	int cnt = 0;

	memset(&pce_rule, 0x00, sizeof(pce_rule));
	cnt += scanParamArg(argc, argv, "pattern.nIndex", sizeof(pce_rule.pattern.nIndex), &pce_rule.pattern.nIndex);
	cnt += scanParamArg(argc, argv, "nRegion", sizeof(pce_rule.region), &pce_rule.region);
	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(pce_rule.logicalportid), &pce_rule.logicalportid);
	cnt += scanParamArg(argc, argv, "nSubIfIdGroup", sizeof(pce_rule.subifidgroup), &pce_rule.subifidgroup);

	if (cnt == 0)
		return (-2);

	return cli_ioctl(fd, GSW_PCE_RULE_ENABLE, &pce_rule);
}

int gsw_pce_rule_disable(int argc, char *argv[], void *fd, int numPar)
{
	GSW_PCE_rule_t pce_rule;
	int cnt = 0;

	memset(&pce_rule, 0x00, sizeof(pce_rule));
	cnt += scanParamArg(argc, argv, "pattern.nIndex", sizeof(pce_rule.pattern.nIndex), &pce_rule.pattern.nIndex);
	cnt += scanParamArg(argc, argv, "nRegion", sizeof(pce_rule.region), &pce_rule.region);
	cnt += scanParamArg(argc, argv, "nLogicalPortId", sizeof(pce_rule.logicalportid), &pce_rule.logicalportid);
	cnt += scanParamArg(argc, argv, "nSubIfIdGroup", sizeof(pce_rule.subifidgroup), &pce_rule.subifidgroup);

	if (cnt == 0)
		return (-2);

	return cli_ioctl(fd, GSW_PCE_RULE_DISABLE, &pce_rule);
}

int gsw_rmon_extend_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_RMON_extendGet_t param;
	int cnt, i;
	memset(&param, 0, sizeof(GSW_RMON_extendGet_t));

	cnt = scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != 1)
		return (-2);

	if (cli_ioctl(fd, GSW_RMON_EXTEND_GET, &param)) {
		printf("ioctl returned with ERROR!\n");
		return (-3);
	}

	for (i = 0; i < GSW_RMON_EXTEND_NUM; i++) {
		printf("RMON Counter [%d] = %d\n", i, param.nTrafficFlowCnt[i]);
	}

	return 0;
}

int gsw_rmon_extend_set(int argc, char *argv[], void *fd, int numPar)
{
	return -1;
}

int gsw_timestamp_timer_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_TIMESTAMP_Timer_t param;
	memset(&param, 0, sizeof(GSW_TIMESTAMP_Timer_t));

	if (cli_ioctl(fd, GSW_TIMESTAMP_TIMER_GET, &param)) {
		printf("ioctl returned with ERROR!\n");
		return (-3);
	}

	printHex32Value("nSec", param.nSec, 0);
	printHex32Value("nNanoSec", param.nNanoSec, 0);
	printHex32Value("nFractionalNanoSec", param.nFractionalNanoSec, 0);
	return 0;
}

int gsw_timestamp_timer_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_TIMESTAMP_Timer_t param;
	int cnt = 0;
	memset(&param, 0, sizeof(GSW_TIMESTAMP_Timer_t));

	if (cli_ioctl(fd, GSW_TIMESTAMP_TIMER_GET, &param)) {
		printf("ioctl returned with ERROR!\n");
		return (-3);
	}

	cnt += scanParamArg(argc, argv, "nSec", sizeof(param.nSec), &param.nSec);
	cnt += scanParamArg(argc, argv, "nNanoSec", sizeof(param.nNanoSec), &param.nNanoSec);
	cnt += scanParamArg(argc, argv, "nFractionalNanoSec", sizeof(param.nFractionalNanoSec), &param.nFractionalNanoSec);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_TIMESTAMP_TIMER_SET, &param)) {
		printf("ioctl returned with ERROR!\n");
		return (-3);
	}

	return 0;
}

int gsw_timestamp_port_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_TIMESTAMP_PortRead_t param;
	int cnt;
	memset(&param, 0, sizeof(GSW_TIMESTAMP_PortRead_t));
	cnt = scanParamArg(argc, argv, "nPortId", sizeof(param.nPortId), &param.nPortId);

	if (cnt != numPar)
		return (-2);

	if (cli_ioctl(fd, GSW_TIMESTAMP_PORT_READ, &param)) {
		printf("ioctl returned with ERROR!\n");
		return (-3);
	}

	printHex32Value("nPortId", param.nPortId, 0);
	printHex32Value("nIngressSec", param.nIngressSec, 0);
	printHex32Value("nIngressNanoSec", param.nIngressNanoSec, 0);
	printHex32Value("nEgressSec", param.nEgressSec, 0);
	printHex32Value("nEgressNanoSec", param.nEgressNanoSec, 0);
	return 0;
}
int gsw_boot_get(int argc, char *argv[], void *fd, int numPar)
{
	return -1;
}

int gsw_boot_set(int argc, char *argv[], void *fd, int numPar)
{
	return -1;
}

int gsw_rmon_extend_clear(int argc, char *argv[], void *fd, int numPar)
{
	return -1;
}

int gsw_register_set(int argc, char *argv[], void *fd, int numPar)
{
	GSW_register_t param;
	u16 nSetBits = 0, nClearBits = 0;
	int bitmask_used = 0;

	memset(&param, 0, sizeof(GSW_register_t));

	if (scanParamArg(argc, argv, "nRegAddr", sizeof(param.nRegAddr), &param.nRegAddr) == 0)
		return (-2);


	if (scanParamArg(argc, argv, "nSetBits", sizeof(nSetBits), &nSetBits) != 0)
		bitmask_used = 1;

	if (scanParamArg(argc, argv, "nClearBits", sizeof(nClearBits), &nClearBits) != 0)
		bitmask_used = 1;

	if (bitmask_used == 0) {
		/* Scan 'nData' and write the hardware register. */
		if (scanParamArg(argc, argv, "nData", sizeof(param.nData), &param.nData) == 0)
			return (-3);

	} else {
		/* Read the register, set 'nSetBits', clear 'nCleartBits' and
		   write hardware register. */
		if (cli_ioctl(fd, GSW_REGISTER_GET, &param) != 0)
			return (-4);

		param.nData = param.nData & ~nClearBits;
		param.nData = param.nData | nSetBits;
	}

	return cli_ioctl(fd, GSW_REGISTER_SET, &param);
}

static int print_queues_gswip30(void *fd, u16 port)
{
	GSW_QoS_queuePort_t queuePortParam = {0};
	int tc;

	queuePortParam.nPortId = port;

	for (tc = 0; tc < NUM_TC * 2; tc++) {
		queuePortParam.nTrafficClassId = tc & GENMASK(3, 0);
		queuePortParam.bRedirectionBypass = (tc & BIT(4)) >> 4;

		if (queuePortParam.nTrafficClassId == 0) {
			printf("\n");
			printf(" Port | Traffic Class | bRedirectionBypass | Egress Queue | nRedirectPortId\n");
			printf("---------------------------------------------------------------------------\n");
		}

		if (cli_ioctl(fd, GSW_QOS_QUEUE_PORT_GET, &queuePortParam)) {
			printf("ioctl returned with ERROR!\n");
			return -1;
		}

		printf(" %4d | %13d | %18d | %12d | %15d\n",
		       queuePortParam.nPortId,
		       queuePortParam.nTrafficClassId,
		       queuePortParam.bRedirectionBypass,
		       queuePortParam.nQueueId,
		       queuePortParam.nRedirectPortId);

		if (queuePortParam.nTrafficClassId == NUM_TC - 1)
			printf("---------------------------------------------------------------------------\n");
	}

	return 0;
}

static int print_queues_gswip31(void *fd, u16 port)
{
	GSW_QoS_queuePort_t queuePortParam = {0};
	int tc;

	queuePortParam.nPortId = port;

	for (tc = 0; tc < NUM_TC * 4; tc++) {
		queuePortParam.nTrafficClassId = tc & GENMASK(3, 0);
		queuePortParam.bRedirectionBypass = (tc & BIT(4)) >> 4;
		queuePortParam.bExtrationEnable = (tc & BIT(5)) >> 5;

		if (queuePortParam.nTrafficClassId == 0) {
			printf("\n");
			printf(" Port | Traffic Class | bRedirectionBypass | bExtrationEnable | Egress Queue | nRedirectPortId\n");
			printf("----------------------------------------------------------------------------------------------\n");
		}

		if (cli_ioctl(fd, GSW_QOS_QUEUE_PORT_GET, &queuePortParam)) {
			printf("ioctl returned with ERROR!\n");
			return -1;
		}

		printf(" %4d | %13d | %18d | %16d | %12d | %15d\n",
		       queuePortParam.nPortId,
		       queuePortParam.nTrafficClassId,
		       queuePortParam.bRedirectionBypass,
		       queuePortParam.bExtrationEnable,
		       queuePortParam.nQueueId,
		       queuePortParam.nRedirectPortId);

		if (queuePortParam.nTrafficClassId == NUM_TC - 1)
			printf("----------------------------------------------------------------------------------------------\n");
	}

	return 0;
}

static int print_queues_gswip32(void *fd, u16 port)
{
	GSW_QoS_queuePort_t queuePortParam = {0};
	int tc;

	queuePortParam.nPortId = port;

	for (tc = 0; tc < NUM_TC * 4; tc++) {
		queuePortParam.nTrafficClassId = tc & GENMASK(3, 0);
		queuePortParam.bRedirectionBypass = (tc & BIT(4)) >> 4;
		queuePortParam.bExtrationEnable = (tc & BIT(5)) >> 5;

		if (queuePortParam.nTrafficClassId == 0) {
			printf("\n");
			printf(" Port | Traffic Class | bRedirectionBypass | bPceIngressBypass | bExtrationEnable | Egress Queue | nRedirectPortId\n");
			printf("------------------------------------------------------------------------------------------------------------------\n");
		}

		if (cli_ioctl(fd, GSW_QOS_QUEUE_PORT_GET, &queuePortParam)) {
			printf("ioctl returned with ERROR!\n");
			return -1;
		}

		printf(" %4d | %13d | %18d ",
		       queuePortParam.nPortId,
		       queuePortParam.nTrafficClassId,
		       queuePortParam.bRedirectionBypass);

		if (queuePortParam.bRedirectionBypass)
			printf("| %17s ", "n/a");
		else
			printf("| %17d ", queuePortParam.bEnableIngressPceBypass);

		if (queuePortParam.bRedirectionBypass
		    || !queuePortParam.bEnableIngressPceBypass)
			printf("| %16d ", queuePortParam.bExtrationEnable);
		else
			printf("| %16s ", "n/a");

		printf("| %12d | %15d\n",
		       queuePortParam.nQueueId,
		       queuePortParam.nRedirectPortId);

		if (queuePortParam.nTrafficClassId == NUM_TC - 1)
			printf("------------------------------------------------------------------------------------------------------------------\n");
	}

	return 0;
}

int gsw_qos_queue_port_get(int argc, char *argv[], void *fd, int numPar)
{
	GSW_QoS_queuePort_t queuePortParam;
	int tc;

	if (findStringParam(argc, argv, "listunused")) {
		/* printout the unused Queues of the whole switch.
		   First read out the capability retreaving the total amount of queues.
		   Then read the queue assignment of all traffic classes of all ports.
		   Then printout the list of all unused queues. */
		GSW_cap_t capParam;
		int numQueues, numPorts, port, i;
		char *ptrQueues;

		memset(&capParam, 0, sizeof(GSW_cap_t));
		capParam.nCapType = GSW_CAP_TYPE_PRIORITY_QUEUE;

		if (cli_ioctl(fd, GSW_CAP_GET, &capParam) != 0) {
			printf("ioctl returned with ERROR!\n");
			return (-1);
		}

		numQueues = capParam.nCap;

		memset(&capParam, 0, sizeof(GSW_cap_t));
		capParam.nCapType = GSW_CAP_TYPE_PORT;

		if (cli_ioctl(fd, GSW_CAP_GET, &capParam) != 0) {
			printf("ioctl returned with ERROR!\n");
			return (-1);
		}

		numPorts = capParam.nCap;

		memset(&capParam, 0, sizeof(GSW_cap_t));
		capParam.nCapType = GSW_CAP_TYPE_VIRTUAL_PORT;

		if (cli_ioctl(fd, GSW_CAP_GET, &capParam) != 0) {
			printf("ioctl returned with ERROR!\n");
			return (-1);
		}

		numPorts += capParam.nCap;

		/* allocate temporary buffer which will be freed later again */
		ptrQueues = malloc(numQueues);

		if (ptrQueues == NULL) {
			printf("Could not allocate memory!\n");
			return (-1);
		}

		memset(ptrQueues, 0, numQueues);

		for (port = 0; port < numPorts; port++) {
			for (tc = 0; tc < NUM_TC * 4; tc++) {
				if (VRSN_CMP(gswip_version, GSWIP_3_0) < 0
				    && (tc & GENMASK(5, 4)))
					break;
				else if (VRSN_CMP(gswip_version, GSWIP_3_1) < 0
					 && (tc & BIT(5)))
					break;

				memset(&queuePortParam, 0, sizeof(GSW_QoS_queuePort_t));
				queuePortParam.nPortId = port;
				queuePortParam.nTrafficClassId = tc & GENMASK(3, 0);
				queuePortParam.bRedirectionBypass = (tc & BIT(4)) >> 4;
				queuePortParam.bExtrationEnable = (tc & BIT(5)) >> 5;

				if (cli_ioctl(fd, GSW_QOS_QUEUE_PORT_GET, &queuePortParam) != 0) {
					printf("ioctl returned with ERROR!\n");
					/* free temporary buffer */
					free(ptrQueues);
					return (-1);
				}

				if (queuePortParam.nQueueId < numQueues) {
					/* mark the queue as used */
					ptrQueues[queuePortParam.nQueueId] = 1;
				}
			}
		}

		printf("\nList of unused Egress Traffic Class Queue Indices:\n"
		       "--------------------------------------------------\n");

		for (i = 0; i < numQueues; i++) {
			if (ptrQueues[i] == 0) {
				printf("Queue %2d unused\n", i);
			}
		}

		printf("\n");

		/* free temporary buffer */
		free(ptrQueues);
		return 0;
	}

	memset(&queuePortParam, 0, sizeof(GSW_QoS_queuePort_t));

	if (scanParamArg(argc, argv, "nPortId", sizeof(queuePortParam.nPortId), &queuePortParam.nPortId)) {
		if (scanParamArg(argc, argv, "nTrafficClassId", sizeof(queuePortParam.nTrafficClassId), &queuePortParam.nTrafficClassId)) {
			scanParamArg(argc, argv, "bRedirectionBypass", sizeof(queuePortParam.bRedirectionBypass), &queuePortParam.bRedirectionBypass);

			if (VRSN_CMP(gswip_version, GSWIP_3_1) >= 0)
				scanParamArg(argc, argv, "bExtrationEnable", sizeof(queuePortParam.bExtrationEnable), &queuePortParam.bExtrationEnable);

			if (cli_ioctl(fd, GSW_QOS_QUEUE_PORT_GET, &queuePortParam) != 0) {
				printf("ioctl returned with ERROR!\n");
				return (-1);
			}

			printf("Returned values:\n----------------\n");
			printHex32Value("nPortId", queuePortParam.nPortId, 0);
			printHex32Value("nTrafficClassId", queuePortParam.nTrafficClassId, 0);
			printHex32Value("bRedirectionBypass", queuePortParam.bRedirectionBypass, 0);

			if (VRSN_CMP(gswip_version, GSWIP_3_2) >= 0)
				printHex32Value("bPceIngressBypass", queuePortParam.bEnableIngressPceBypass, 0);

			if (VRSN_CMP(gswip_version, GSWIP_3_1) >= 0) {
				printHex32Value("bExtrationEnable", queuePortParam.bExtrationEnable, 0);

				if (queuePortParam.eQMapMode == GSW_QOS_QMAP_SINGLE_MODE)
					printf("\t%40s:\tSingle\n", "eQMapMode");
				else
					printf("\t%40s:\tSubifid\n", "eQMapMode");
			}

			printHex32Value("nQueueId", queuePortParam.nQueueId, 0);
			printHex32Value("nRedirectPortId", queuePortParam.nRedirectPortId, 0);
			return 0;
		} else {
			if (VRSN_CMP(gswip_version, GSWIP_3_0) <= 0)
				return print_queues_gswip30(fd, queuePortParam.nPortId);
			else if (VRSN_CMP(gswip_version, GSWIP_3_1) <= 0)
				return print_queues_gswip31(fd, queuePortParam.nPortId);
			else
				return print_queues_gswip32(fd, queuePortParam.nPortId);
		}
	}

	printf("Parameter \"nPortId\" is missing.\n");
	return -1;
}

#if defined(CONFIG_LTQ_TEST) && CONFIG_LTQ_TEST

int gsw_route_entry_add(int argc, char *argv[], void *fd, int numPar)
{
	GSW_ROUTE_Entry_t rt_entry;
	int cnt = 0;

	memset(&rt_entry, 0, sizeof(rt_entry));

	cnt += scanParamArg(argc, argv, "nHashVal", sizeof(rt_entry.nHashVal), &rt_entry.nHashVal);
	cnt += scanParamArg(argc, argv, "nRtIndex", sizeof(rt_entry.nRtIndex), &rt_entry.nRtIndex);
	cnt += scanParamArg(argc, argv, "bPrio", sizeof(rt_entry.bPrio), &rt_entry.bPrio);
	cnt += scanParamArg(argc, argv, "nFlags", sizeof(rt_entry.nFlags), &rt_entry.nFlags);

	cnt += scanParamArg(argc, argv, "routeEntry.pattern.eIpType", sizeof(rt_entry.routeEntry.pattern.eIpType), &rt_entry.routeEntry.pattern.eIpType);

	if (rt_entry.routeEntry.pattern.eIpType == GSW_RT_IP_V4)
		cnt += scanIPv4_Arg(argc, argv, "routeEntry.pattern.nDstIP", &rt_entry.routeEntry.pattern.nDstIP.nIPv4);
	else if (rt_entry.routeEntry.pattern.eIpType == GSW_RT_IP_V6)
		cnt += scanIPv6_Arg(argc, argv, "routeEntry.pattern.nDstIP", rt_entry.routeEntry.pattern.nDstIP.nIPv6);

	if (rt_entry.routeEntry.pattern.eIpType == GSW_RT_IP_V4)
		cnt += scanIPv4_Arg(argc, argv, "routeEntry.pattern.nSrcIP", &rt_entry.routeEntry.pattern.nSrcIP.nIPv4);
	else if (rt_entry.routeEntry.pattern.eIpType == GSW_RT_IP_V6)
		cnt += scanIPv6_Arg(argc, argv, "routeEntry.pattern.nSrcIP", rt_entry.routeEntry.pattern.nSrcIP.nIPv6);

	cnt += scanParamArg(argc, argv, "routeEntry.pattern.nSrcPort", sizeof(rt_entry.routeEntry.pattern.nSrcPort), &rt_entry.routeEntry.pattern.nSrcPort);
	cnt += scanParamArg(argc, argv, "routeEntry.pattern.nDstPort", sizeof(rt_entry.routeEntry.pattern.nDstPort), &rt_entry.routeEntry.pattern.nDstPort);
	cnt += scanParamArg(argc, argv, "routeEntry.pattern.nRoutExtId", sizeof(rt_entry.routeEntry.pattern.nRoutExtId), &rt_entry.routeEntry.pattern.nRoutExtId);
	cnt += scanParamArg(argc, argv, "routeEntry.pattern.bValid", sizeof(rt_entry.routeEntry.pattern.bValid), &rt_entry.routeEntry.pattern.bValid);


	cnt += scanParamArg(argc, argv, "routeEntry.action.nDstPortMap", sizeof(rt_entry.routeEntry.action.nDstPortMap), &rt_entry.routeEntry.action.nDstPortMap);
	cnt += scanParamArg(argc, argv, "routeEntry.action.nDstSubIfId", sizeof(rt_entry.routeEntry.action.nDstSubIfId), &rt_entry.routeEntry.action.nDstSubIfId);
	cnt += scanParamArg(argc, argv, "routeEntry.action.eIpType", sizeof(rt_entry.routeEntry.action.eIpType), &rt_entry.routeEntry.action.eIpType);

	if (rt_entry.routeEntry.action.eIpType == GSW_RT_IP_V4)
		cnt += scanIPv4_Arg(argc, argv, "routeEntry.action.nNATIPaddr", &rt_entry.routeEntry.action.nNATIPaddr.nIPv4);
	else if (rt_entry.routeEntry.action.eIpType == GSW_RT_IP_V6)
		cnt += scanIPv6_Arg(argc, argv, "routeEntry.action.nNATIPaddr", rt_entry.routeEntry.action.nNATIPaddr.nIPv6);

	cnt += scanParamArg(argc, argv, "routeEntry.action.nTcpUdpPort", sizeof(rt_entry.routeEntry.action.nTcpUdpPort), &rt_entry.routeEntry.action.nTcpUdpPort);
	cnt += scanParamArg(argc, argv, "routeEntry.action.nMTUvalue", sizeof(rt_entry.routeEntry.action.nMTUvalue), &rt_entry.routeEntry.action.nMTUvalue);

	cnt += scanParamArg(argc, argv, "routeEntry.action.bMAC_SrcEnable", sizeof(rt_entry.routeEntry.action.bMAC_SrcEnable), &rt_entry.routeEntry.action.bMAC_SrcEnable);
	cnt += scanMAC_Arg(argc, argv, "routeEntry.action.nSrcMAC", rt_entry.routeEntry.action.nSrcMAC);
	cnt += scanParamArg(argc, argv, "routeEntry.action.bMAC_DstEnable", sizeof(rt_entry.routeEntry.action.bMAC_DstEnable), &rt_entry.routeEntry.action.bMAC_DstEnable);
	cnt += scanMAC_Arg(argc, argv, "routeEntry.action.nDstMAC", rt_entry.routeEntry.action.nDstMAC);

	cnt += scanParamArg(argc, argv, "routeEntry.action.bPPPoEmode", sizeof(rt_entry.routeEntry.action.bPPPoEmode), &rt_entry.routeEntry.action.bPPPoEmode);
	cnt += scanParamArg(argc, argv, "routeEntry.action.nPPPoESessId", sizeof(rt_entry.routeEntry.action.nPPPoESessId), &rt_entry.routeEntry.action.nPPPoESessId);

	cnt += scanParamArg(argc, argv, "routeEntry.action.bTunnel_Enable", sizeof(rt_entry.routeEntry.action.bTunnel_Enable), &rt_entry.routeEntry.action.bTunnel_Enable);
	cnt += scanParamArg(argc, argv, "routeEntry.action.eTunType", sizeof(rt_entry.routeEntry.action.eTunType), &rt_entry.routeEntry.action.eTunType);
	cnt += scanParamArg(argc, argv, "routeEntry.action.nTunnelIndex", sizeof(rt_entry.routeEntry.action.nTunnelIndex), &rt_entry.routeEntry.action.nTunnelIndex);

	cnt += scanParamArg(argc, argv, "routeEntry.action.bMeterAssign", sizeof(rt_entry.routeEntry.action.bMeterAssign), &rt_entry.routeEntry.action.bMeterAssign);
	cnt += scanParamArg(argc, argv, "routeEntry.action.nMeterId", sizeof(rt_entry.routeEntry.action.nMeterId), &rt_entry.routeEntry.action.nMeterId);

	cnt += scanParamArg(argc, argv, "routeEntry.action.bRTPMeasEna", sizeof(rt_entry.routeEntry.action.bRTPMeasEna), &rt_entry.routeEntry.action.bRTPMeasEna);
	cnt += scanParamArg(argc, argv, "routeEntry.action.nRTPSeqNumber", sizeof(rt_entry.routeEntry.action.nRTPSeqNumber), &rt_entry.routeEntry.action.nRTPSeqNumber);
	cnt += scanParamArg(argc, argv, "routeEntry.action.nRTPSessionPktCnt", sizeof(rt_entry.routeEntry.action.nRTPSessionPktCnt), &rt_entry.routeEntry.action.nRTPSessionPktCnt);

	cnt += scanParamArg(argc, argv, "routeEntry.action.nFID", sizeof(rt_entry.routeEntry.action.nFID), &rt_entry.routeEntry.action.nFID);
	cnt += scanParamArg(argc, argv, "routeEntry.action.nFlowId", sizeof(rt_entry.routeEntry.action.nFlowId), &rt_entry.routeEntry.action.nFlowId);

	cnt += scanParamArg(argc, argv, "routeEntry.action.eOutDSCPAction", sizeof(rt_entry.routeEntry.action.eOutDSCPAction), &rt_entry.routeEntry.action.eOutDSCPAction);
	cnt += scanParamArg(argc, argv, "routeEntry.action.bInnerDSCPRemark", sizeof(rt_entry.routeEntry.action.bInnerDSCPRemark), &rt_entry.routeEntry.action.bInnerDSCPRemark);
	cnt += scanParamArg(argc, argv, "routeEntry.action.nDSCP", sizeof(rt_entry.routeEntry.action.nDSCP), &rt_entry.routeEntry.action.nDSCP);

	cnt += scanParamArg(argc, argv, "routeEntry.action.bTCremarking", sizeof(rt_entry.routeEntry.action.bTCremarking), &rt_entry.routeEntry.action.bTCremarking);
	cnt += scanParamArg(argc, argv, "routeEntry.action.nTrafficClass", sizeof(rt_entry.routeEntry.action.nTrafficClass), &rt_entry.routeEntry.action.nTrafficClass);

	cnt += scanParamArg(argc, argv, "routeEntry.action.nSessionCtrs", sizeof(rt_entry.routeEntry.action.nSessionCtrs), &rt_entry.routeEntry.action.nSessionCtrs);

	cnt += scanParamArg(argc, argv, "routeEntry.action.eSessDirection", sizeof(rt_entry.routeEntry.action.eSessDirection), &rt_entry.routeEntry.action.eSessDirection);
	cnt += scanParamArg(argc, argv, "routeEntry.action.eSessRoutingMode", sizeof(rt_entry.routeEntry.action.eSessRoutingMode), &rt_entry.routeEntry.action.eSessRoutingMode);

	cnt += scanParamArg(argc, argv, "routeEntry.action.bTTLDecrement", sizeof(rt_entry.routeEntry.action.bTTLDecrement), &rt_entry.routeEntry.action.bTTLDecrement);
	cnt += scanParamArg(argc, argv, "routeEntry.action.bHitStatus", sizeof(rt_entry.routeEntry.action.bHitStatus), &rt_entry.routeEntry.action.bHitStatus);

	if (cnt != numPar) return (-2);

	if (cnt == 0) return (-2);

//	if(cli_ioctl(fd, GSW_ROUTE_ENTRY_ADD, &rt_entry) != 0)
//		return (-3);
	cnt = cli_ioctl(fd, GSW_ROUTE_ENTRY_ADD, &rt_entry);
	printf("\treturn:    %d\n", cnt);
	printf("\tnRtIndex:  %d\n", rt_entry.nRtIndex);
	return 0;
}

int gsw_route_entry_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_ROUTE_Entry_t rt_entry;
	int cnt = 0;

	memset(&rt_entry, 0, sizeof(rt_entry));

	cnt += scanParamArg(argc, argv, "nRtIndex", sizeof(rt_entry.nRtIndex), &rt_entry.nRtIndex);

	if (cnt == 0)
		return (-2);

	if (cli_ioctl(fd, GSW_ROUTE_ENTRY_READ, &rt_entry) != 0)
		return (-3);

	printf("\tnHashVal:              %d\n", rt_entry.nHashVal);
	printf("\tnRtIndex:              %d\n", rt_entry.nRtIndex);
	printf("\tbPrio:                 %d\n", rt_entry.bPrio);
	printf("\tnFlags:                %d\n", rt_entry.nFlags);

	if (rt_entry.routeEntry.pattern.eIpType == GSW_RT_IP_V4) {
		printf("\trouteEntry.pattern.nDstIP: ");
		printIPv4_Address(rt_entry.routeEntry.pattern.nDstIP.nIPv4);
		printf("\n");
		printf("\trouteEntry.pattern.nSrcIP: ");
		printIPv4_Address(rt_entry.routeEntry.pattern.nSrcIP.nIPv4);
		printf("\n");
	} else if (rt_entry.routeEntry.pattern.eIpType == GSW_RT_IP_V6) {
		printf("\trouteEntry.pattern.nDstIP: ");
		printIPv6_Address(rt_entry.routeEntry.pattern.nDstIP.nIPv6);
		printf("\n");
		printf("\trouteEntry.pattern.nSrcIP: ");
		printIPv6_Address(rt_entry.routeEntry.pattern.nSrcIP.nIPv6);
		printf("\n");
	}

	printf("\trouteEntry.pattern.nSrcPort:0x%08x\n", rt_entry.routeEntry.pattern.nSrcPort);
	printf("\trouteEntry.pattern.nDstPort:0x%08x\n", rt_entry.routeEntry.pattern.nDstPort);
	printf("\trouteEntry.pattern.nRoutExtId:%d\n", rt_entry.routeEntry.pattern.nRoutExtId);
	printf("\trouteEntry.pattern.bValid:%d\n", rt_entry.routeEntry.pattern.bValid);


	printf("\trouteEntry.action.nDstPortMap: 0x%08x\n", rt_entry.routeEntry.action.nDstPortMap);
	printf("\trouteEntry.action.nDstSubIfId: 0x%08x\n", rt_entry.routeEntry.action.nDstSubIfId);

	if (rt_entry.routeEntry.action.eIpType == GSW_RT_IP_V4) {
		printf("\trouteEntry.action.nNATIPaddr: ");
		printIPv4_Address(rt_entry.routeEntry.action.nNATIPaddr.nIPv4);
		printf("\n");
	} else if (rt_entry.routeEntry.action.eIpType == GSW_RT_IP_V6) {
		printf("\trouteEntry.action.nNATIPaddr: ");
		printIPv6_Address(rt_entry.routeEntry.action.nNATIPaddr.nIPv6);
		printf("\n");
	}

	printf("\trouteEntry.action.nTcpUdpPort: 0x%08x\n", rt_entry.routeEntry.action.nTcpUdpPort);
	printf("\trouteEntry.action.nMTUvalue: 0x%08x\n", rt_entry.routeEntry.action.nMTUvalue);


	printf("\trouteEntry.action.bMAC_SrcEnable: %d\n", rt_entry.routeEntry.action.bMAC_SrcEnable);
	printf("\trouteEntry.action.nSrcMAC[6]               ");
	printMAC_Address(rt_entry.routeEntry.action.nSrcMAC);
	printf("\n");
	printf("\trouteEntry.action.bMAC_SrcEnable: %d\n", rt_entry.routeEntry.action.bMAC_DstEnable);
	printf("\trouteEntry.action.nDstMAC[6]               ");
	printMAC_Address(rt_entry.routeEntry.action.nDstMAC);
	printf("\n");

	printf("\trouteEntry.action.bPPPoEmode :%d\n", rt_entry.routeEntry.action.bPPPoEmode);
	printf("\trouteEntry.action.nPPPoESessId:%d\n",  rt_entry.routeEntry.action.nPPPoESessId);

	printf("\trouteEntry.action.bTunnel_Enable: %d\n",  rt_entry.routeEntry.action.bTunnel_Enable);
	printf("\trouteEntry.action.eTunType: %d\n", rt_entry.routeEntry.action.eTunType);
	printf("\trouteEntry.action.nTunnelIndex: %d\n", rt_entry.routeEntry.action.nTunnelIndex);

	printf("\trouteEntry.action.bMeterAssign: %d\n", rt_entry.routeEntry.action.bMeterAssign);
	printf("\trouteEntry.action.nMeterId: %d\n", rt_entry.routeEntry.action.nMeterId);

	printf("\trouteEntry.action.bRTPMeasEna:%d\n", rt_entry.routeEntry.action.bRTPMeasEna);
	printf("\trouteEntry.action.nRTPSeqNumber:%d\n",  rt_entry.routeEntry.action.nRTPSeqNumber);
	printf("\trouteEntry.action.nRTPSessionPktCnt: %d\n", rt_entry.routeEntry.action.nRTPSessionPktCnt);

	printf("\trouteEntry.action.nFID: %d\n", rt_entry.routeEntry.action.nFID);
	printf("\trouteEntry.action.nFlowId: %d\n", rt_entry.routeEntry.action.nFlowId);

	printf("\trouteEntry.action.eOutDSCPAction:%d\n", rt_entry.routeEntry.action.eOutDSCPAction);
	printf("\trouteEntry.action.bInnerDSCPRemark:%d\n", rt_entry.routeEntry.action.bInnerDSCPRemark);
	printf("\trouteEntry.action.nDSCP:%d\n", rt_entry.routeEntry.action.nDSCP);

	printf("\trouteEntry.action.bTCremarking:%d\n", rt_entry.routeEntry.action.bTCremarking);
	printf("\trouteEntry.action.nTrafficClass:%d\n", rt_entry.routeEntry.action.nTrafficClass);

	printf("\trouteEntry.action.nSessionCtrs: %d\n", rt_entry.routeEntry.action.nSessionCtrs);

	printf("\trouteEntry.action.eSessDirection: %d\n", rt_entry.routeEntry.action.eSessDirection);
	printf("\trouteEntry.action.eSessRoutingMode: %d\n", rt_entry.routeEntry.action.eSessRoutingMode);

	printf("\trouteEntry.action.bTTLDecrement: %d\n", rt_entry.routeEntry.action.bTTLDecrement);
	printf("\trouteEntry.action.bHitStatus: %d\n", rt_entry.routeEntry.action.bHitStatus);

	return 0;
}

int gsw_route_entry_delete(int argc, char *argv[], void *fd, int numPar)
{
	GSW_ROUTE_Entry_t rt_entry;
	int cnt = 0;

	memset(&rt_entry, 0, sizeof(rt_entry));

	cnt += scanParamArg(argc, argv, "nRtIndex", sizeof(rt_entry.nRtIndex), &rt_entry.nRtIndex);
	cnt += scanParamArg(argc, argv, "nHashVal", sizeof(rt_entry.nHashVal), &rt_entry.nHashVal);

	if (cnt == 0)
		return (-2);

	if (cli_ioctl(fd, GSW_ROUTE_ENTRY_DELETE, &rt_entry) != 0)
		return (-3);

	return 0;
}

int gsw_route_tunnel_entry_add(int argc, char *argv[], void *fd, int numPar)
{
	GSW_ROUTE_Tunnel_Entry_t tn_entry;
	int cnt = 0;

	memset(&tn_entry, 0, sizeof(tn_entry));

	cnt += scanParamArg(argc, argv, "nTunIndex", sizeof(tn_entry.nTunIndex), &tn_entry.nTunIndex);
	cnt += scanParamArg(argc, argv, "tunnelEntry.eTunnelType", sizeof(tn_entry.tunnelEntry.eTunnelType), &tn_entry.tunnelEntry.eTunnelType);

	if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_6RD) {
		cnt += scanIPv4_Arg(argc, argv, "tunnelEntry.t.tun6RD.nSrcIP4Addr", &tn_entry.tunnelEntry.t.tun6RD.nSrcIP4Addr.nIPv4);
		cnt += scanIPv4_Arg(argc, argv, "tunnelEntry.t.tun6RD.nDstIP4Addr", &tn_entry.tunnelEntry.t.tun6RD.nDstIP4Addr.nIPv4);
	} else if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_L2TP) {
		cnt += scanParamArg(argc, argv, "tunnelEntry.t.nTunL2TP", sizeof(tn_entry.tunnelEntry.t.nTunL2TP), &tn_entry.tunnelEntry.t.nTunL2TP);
	} else if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_IPSEC) {
		cnt += scanParamArg(argc, argv, "tunnelEntry.t.nTunIPsec", sizeof(tn_entry.tunnelEntry.t.nTunIPsec), &tn_entry.tunnelEntry.t.nTunIPsec);
	} else if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_DSLITE) {
		cnt += scanIPv6_Arg(argc, argv, "tunnelEntry.t.tunDSlite.nSrcIP6Addr", tn_entry.tunnelEntry.t.tunDSlite.nSrcIP6Addr.nIPv6);
		cnt += scanIPv6_Arg(argc, argv, "tunnelEntry.t.tunDSlite.nDstIP6Addr", tn_entry.tunnelEntry.t.tunDSlite.nDstIP6Addr.nIPv6);
	}

	if (cnt != numPar) return (-2);

	if (cnt == 0) return (-2);

	if (cli_ioctl(fd, GSW_ROUTE_TUNNEL_ENTRY_ADD, &tn_entry) != 0)
		return (-3);

	return 0;
}

int gsw_route_tunnel_entry_delete(int argc, char *argv[], void *fd, int numPar)
{
	GSW_ROUTE_Tunnel_Entry_t tn_entry;
	int cnt = 0;

	memset(&tn_entry, 0, sizeof(tn_entry));

	cnt += scanParamArg(argc, argv, "nTunIndex", sizeof(tn_entry.nTunIndex), &tn_entry.nTunIndex);
	cnt += scanParamArg(argc, argv, "tunnelEntry.eTunnelType", sizeof(tn_entry.tunnelEntry.eTunnelType), &tn_entry.tunnelEntry.eTunnelType);

	if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_6RD) {
		cnt += scanIPv4_Arg(argc, argv, "tunnelEntry.t.tun6RD.nSrcIP4Addr", &tn_entry.tunnelEntry.t.tun6RD.nSrcIP4Addr.nIPv4);
		cnt += scanIPv4_Arg(argc, argv, "tunnelEntry.t.tun6RD.nDstIP4Addr", &tn_entry.tunnelEntry.t.tun6RD.nDstIP4Addr.nIPv4);
	} else if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_L2TP) {
		cnt += scanParamArg(argc, argv, "tunnelEntry.t.nTunL2TP", sizeof(tn_entry.tunnelEntry.t.nTunL2TP), &tn_entry.tunnelEntry.t.nTunL2TP);
	} else if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_IPSEC) {
		cnt += scanParamArg(argc, argv, "tunnelEntry.t.nTunIPsec", sizeof(tn_entry.tunnelEntry.t.nTunIPsec), &tn_entry.tunnelEntry.t.nTunIPsec);
	} else if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_DSLITE) {
		cnt += scanIPv6_Arg(argc, argv, "tunnelEntry.t.tunDSlite.nSrcIP6Addr", tn_entry.tunnelEntry.t.tunDSlite.nSrcIP6Addr.nIPv6);
		cnt += scanIPv6_Arg(argc, argv, "tunnelEntry.t.tunDSlite.nDstIP6Addr", tn_entry.tunnelEntry.t.tunDSlite.nDstIP6Addr.nIPv6);
	}

	if (cnt != numPar) return (-2);

	if (cnt == 0) return (-2);

	if (cli_ioctl(fd, GSW_ROUTE_TUNNEL_ENTRY_DELETE, &tn_entry) != 0)
		return (-3);

	return 0;
}

int gsw_route_tunnel_entry_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_ROUTE_Tunnel_Entry_t tn_entry;
	int cnt = 0;

	memset(&tn_entry, 0, sizeof(tn_entry));

	cnt += scanParamArg(argc, argv, "nTunIndex", sizeof(tn_entry.nTunIndex), &tn_entry.nTunIndex);
	cnt += scanParamArg(argc, argv, "tunnelEntry.eTunnelType", sizeof(tn_entry.tunnelEntry.eTunnelType), &tn_entry.tunnelEntry.eTunnelType);

	if (cnt != numPar) return (-2);

	if (cnt != 2) return (-2);

	if (cli_ioctl(fd, GSW_ROUTE_TUNNEL_ENTRY_READ, &tn_entry) != 0)
		return (-3);

	printf("\n\tTunIndex      :          %d\n", tn_entry.nTunIndex);
	printf("\ttunnelEntry.eTunnelType:              %d\n", tn_entry.tunnelEntry.eTunnelType);

	if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_6RD) {
		printf("\ttunnelEntry.t.tun6RD.nSrcIP4Addr: ");
		printIPv4_Address(tn_entry.tunnelEntry.t.tun6RD.nSrcIP4Addr.nIPv4);
		printf("\n");
		printf("\ttunnelEntry.t.tun6RD.nDstIP4Addr: ");
		printIPv4_Address(tn_entry.tunnelEntry.t.tun6RD.nDstIP4Addr.nIPv4);
		printf("\n");
	}

	if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_L2TP)
		printf("\ttunnelEntry.t.nTunL2TP:0x%08x\n", tn_entry.tunnelEntry.t.nTunL2TP);

	if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_IPSEC)
		printf("\ttunnelEntry.t.nTunIPsec:0x%08x\n", tn_entry.tunnelEntry.t.nTunIPsec);

	if (tn_entry.tunnelEntry.eTunnelType == GSW_ROUTE_TUNL_DSLITE) {
		printf("\ttunnelEntry.t.tunDSlite.nSrcIP6Addr: ");
		printIPv6_Address(tn_entry.tunnelEntry.t.tunDSlite.nSrcIP6Addr.nIPv6);
		printf("\n");
		printf("\ttunnelEntry.t.tunDSlite.nDstIP6Addr: ");
		printIPv6_Address(tn_entry.tunnelEntry.t.tunDSlite.nDstIP6Addr.nIPv6);
		printf("\n");
	}

	return 0;
}

int gsw_route_l2nat_cfg_read(int argc, char *argv[], void *fd, int numPar)
{
	GSW_ROUTE_EgPort_L2NAT_Cfg_t l2nat_cfg;
	int cnt = 0;

	memset(&l2nat_cfg, 0, sizeof(l2nat_cfg));

	cnt += scanParamArg(argc, argv, "nEgPortId", sizeof(l2nat_cfg.nEgPortId), &l2nat_cfg.nEgPortId);

	if (cnt != numPar) return (-2);

	if (cli_ioctl(fd, GSW_ROUTE_L2NAT_CFG_READ, &l2nat_cfg) != 0)
		return (-3);

	printf("\n\tbL2NATEna : 0x%08x", l2nat_cfg.bL2NATEna);
	printf("\n\tEgPortId  : 0x%08x", l2nat_cfg.nEgPortId);
	printf("\n\tNatMAC[6] : ");
	printMAC_Address(l2nat_cfg.nNatMAC);
	printf("\n");
	return 0;
}

int gsw_route_l2nat_cfg_write(int argc, char *argv[], void *fd, int numPar)
{
	GSW_ROUTE_EgPort_L2NAT_Cfg_t l2nat_cfg;
	int cnt = 0;

	memset(&l2nat_cfg, 0, sizeof(l2nat_cfg));
	cnt += scanParamArg(argc, argv, "bL2NATEna", sizeof(l2nat_cfg.bL2NATEna), &l2nat_cfg.bL2NATEna);
	cnt += scanParamArg(argc, argv, "nEgPortId", sizeof(l2nat_cfg.nEgPortId), &l2nat_cfg.nEgPortId);
	cnt += scanMAC_Arg(argc, argv, "nNatMAC", l2nat_cfg.nNatMAC);

	if ((cnt != numPar)) return (-2);

	if (cli_ioctl(fd, GSW_ROUTE_L2NAT_CFG_WRITE, &l2nat_cfg) != 0)
		return (-3);

	return 0;
}


int gsw_route_session_hit_op(int argc, char *argv[], void *fd, int numPar)
{
	GSW_ROUTE_Session_Hit_t session_hit;

	int cnt = 0;
	int ret;

	memset(&session_hit, 0, sizeof(session_hit));

	cnt += scanParamArg(argc, argv, "nRtIndex", sizeof(session_hit.nRtIndex), &session_hit.nRtIndex);
	cnt += scanParamArg(argc, argv, "eHitOper", sizeof(session_hit.eHitOper), &session_hit.eHitOper);

	if (cnt != numPar) return (-2);

//	if(cli_ioctl(fd, GSW_ROUTE_SESSION_HIT_OP, &session_hit) != 0)
//		return (-3);
	ret = cli_ioctl(fd, GSW_ROUTE_SESSION_HIT_OP, &session_hit);

	if (session_hit.eHitOper == GSW_ROUTE_HIT_READ) {
		printf("\n\tnRtIndex   :0x%08x", session_hit.nRtIndex);
		printf("\n\teHitOper   :0x%08x", session_hit.eHitOper);
		printf("\n\tHIt Status : %d", ret);
		printf("\n");
	}

	return 0;
}

int gsw_route_session_dest_mod(int argc, char *argv[], void *fd, int numPar)
{
	GSW_ROUTE_Session_Dest_t dest_mod;
	int cnt = 0;

	memset(&dest_mod, 0, sizeof(dest_mod));

	cnt += scanParamArg(argc, argv, "nRtIdx", sizeof(dest_mod.nRtIdx), &dest_mod.nRtIdx);
	cnt += scanParamArg(argc, argv, "nDstSubIfId", sizeof(dest_mod.nDstSubIfId), &dest_mod.nDstSubIfId);
	cnt += scanParamArg(argc, argv, "nDstPortMap", sizeof(dest_mod.nDstPortMap), &dest_mod.nDstPortMap);

	if (cnt != numPar) return (-2);

	if (cli_ioctl(fd, GSW_ROUTE_SESSION_DEST_MOD, &dest_mod) != 0)
		return (-3);

	printf("\n\t nRtIdx   :0x%08x", dest_mod.nRtIdx);
	printf("\n\t nDstSubIfId   :0x%08x", dest_mod.nDstSubIfId);
	printf("\n\t nDstPortMap   :0x%08x", dest_mod.nDstPortMap);
	return 0;
}
#endif
