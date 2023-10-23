/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __QOS_AL_FLOW_IF_H
#define __QOS_AL_FLOW_IF_H
#include <qosal_flow.h>
#define FLOWS_BW_IFS 5 //Number of possible datapaths between pair of interfaces

/* Order in this enum should be as per datapath-type ids in platform.xml file */
typedef enum qos_dp_type{
	QOS_DATAPATH_SLOW,
	QOS_DATAPATH_SWFP,
	QOS_DATAPATH_PPEFP,
	QOS_DATAPATH_PPEDP,
	QOS_DATAPATH_PPEDL,
	QOS_DATAPATH_BRIDGE,
	QOS_DATAPATH_MAX
} qos_dp_type_t;

/* Order in this enum should be as per engine's ids in platform.xml file */
typedef enum qos_engines {
	QOS_ENGINE_PPA, 
	QOS_ENGINE_SWITCH,
	QOS_ENGINE_PPE_I, 
	QOS_ENGINE_PPE_R, 
	QOS_ENGINE_PPE_E,
	QOS_ENGINE_CPU_I,
	QOS_ENGINE_CPU,
	QOS_ENGINE_CPU_DP,
	QOS_ENGINE_MAX
} qos_engines_t;

/* Used to define static table of datapaths across the interfaces*/
typedef struct qos_dp_info {
	int32_t datapath_id;
	int32_t number_of_engines;
	int32_t datapath[2*QOS_ENGINE_MAX];
	qos_dp_type_t datapath_type;
} qos_dp_info_t;

/* Used to maintain dynamic list of datapaths for a pair of interfaces*/
typedef struct qos_dp_entry {
	int32_t datapath_id;
	int32_t flag; // To indicate if a datapath is active, disable etc.
} qos_dp_entry_t;

#ifdef STATIC_TABLE
/* Static List of the possible datapaths available in the system across various interfaces*/
const qos_dp_info_t st_sys_datapaths[] = {	
																		{1, {SWITCH, PPE, CPU, PPE, SWITCH}, QOS_DATAPATH_CONTROL},  //Id may not be needed, we can use array index as id.
																		{2, {SWITCH, PPE, CPU, SWITCH}, 1}, //invalid, ppe can be bypassed in egress leg when pkts are going to virtual ports of the switch.
																		{3, {SWITCH, PPE, SWITCH}, 0},
																		{4, {SWITCH}, 0},
																		{5, {SWITCH, PPE, CPU}, 0},
																		{6, {SWITCH, PPE, CPU, PPE}, 1},
																		{7, {SWITCH, PPE}, 0},
																		{2, {SWITCH, PPE, CPU, SWITCH}, 1},
};

/* Static Flow table between each pair of interfaces*/
const int32_t st_datapath_tbl[MAX_INTERFACES][MAX_INTERFACES][FLOWS_BW_IFS]= {	
																																			{	{0}/*LAN -> LAN*/, {5}/*LAN -> WLAN*/, {2, 3, 4}/*LAN -> EWAN*/, {6, 7}/*LAN -> DWAN*/ },  //LAN -> Others
																																			{	{0}/*LAN -> LAN*/, {5}/*LAN -> WLAN*/, {2, 3, 4}/*LAN -> EWAN*/, {6, 7}/*LAN -> DWAN*/ },  //WLAN -> Others
																																			{1,3,2},  //EWAN to others
																																			{1,3,2},  //DWAN to others
};
#endif
#endif
