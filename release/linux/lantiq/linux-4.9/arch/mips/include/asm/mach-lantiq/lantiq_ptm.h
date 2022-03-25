/******************************************************************************
**
** FILE NAME    : ifx_ptm.h
** PROJECT      : UEIP
** MODULES      : PTM
**
** DATE         : 17 Jun 2009
** AUTHOR       : Xu Liang
** DESCRIPTION  : Global PTM driver header file
** COPYRIGHT    :       Copyright (c) 2006
**                      Infineon Technologies AG
**                      Am Campeon 1-12, 85579 Neubiberg, Germany
**
**    This program is free software; you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation; either version 2 of the License, or
**    (at your option) any later version.
**
** HISTORY
** $Date        $Author         $Comment
** 07 JUL 2009  Xu Liang        Init Version
*******************************************************************************/

#ifndef IFX_PTM_H
#define IFX_PTM_H



/*!
  \defgroup IFX_PTM UEIP Project - PTM driver module
  \brief UEIP Project - PTM driver module, support Danube, Amazon-SE, AR9, VR9.
 */

/*!
  \defgroup IFX_PTM_IOCTL IOCTL Commands
  \ingroup IFX_PTM
  \brief IOCTL Commands used by user application.
 */

/*!
  \defgroup IFX_PTM_STRUCT Structures
  \ingroup IFX_PTM
  \brief Structures used by user application.
 */

/*!
  \file ifx_ptm.h
  \ingroup IFX_PTM
  \brief PTM driver header file
 */



/*
 * ####################################
 *              Definition
 * ####################################
 */



/*
 * ####################################
 *                IOCTL
 * ####################################
 */

/*!
  \addtogroup IFX_PTM_IOCTL
 */
/*@{*/

/*
 *  ioctl Command
 */
/*!
  \brief PTM IOCTL Command - Get codeword MIB counters.

  This command uses structure "PTM_CW_IF_ENTRY_T" to get codeword level MIB counters.
 */
#define IFX_PTM_MIB_CW_GET              SIOCDEVPRIVATE + 1
/*!
  \brief PTM IOCTL Command - Get packet MIB counters.

  This command uses structure "PTM_FRAME_MIB_T" to get packet level MIB counters.
 */
#define IFX_PTM_MIB_FRAME_GET           SIOCDEVPRIVATE + 2
/*!
  \brief PTM IOCTL Command - Get firmware configuration (CRC).

  This command uses structure "IFX_PTM_CFG_T" to get firmware configuration (CRC).
 */
#define IFX_PTM_CFG_GET                 SIOCDEVPRIVATE + 3
/*!
  \brief PTM IOCTL Command - Set firmware configuration (CRC).

  This command uses structure "IFX_PTM_CFG_T" to set firmware configuration (CRC).
 */
#define IFX_PTM_CFG_SET                 SIOCDEVPRIVATE + 4
/*!
  \brief PTM IOCTL Command - Program priority value to TX queue mapping.

  This command uses structure "IFX_PTM_PRIO_Q_MAP_T" to program priority value to TX queue mapping.
 */
#define IFX_PTM_MAP_PKT_PRIO_TO_Q       SIOCDEVPRIVATE + 14

/*@}*/


/*!
  \addtogroup IFX_PTM_STRUCT
 */
/*@{*/

/*
 *  ioctl Data Type
 */

/*!
  \typedef PTM_CW_IF_ENTRY_T
  \brief Wrapping of structure "ptm_cw_ifEntry_t".
 */
/*!
  \struct ptm_cw_ifEntry_t
  \brief Structure used for CodeWord level MIB counters.
 */
typedef struct ptm_cw_ifEntry_t {
    uint32_t    ifRxNoIdleCodewords;    /*!< output, number of ingress user codeword */
    uint32_t    ifRxIdleCodewords;      /*!< output, number of ingress idle codeword */
    uint32_t    ifRxCodingViolation;    /*!< output, number of error ingress codeword */
    uint32_t    ifTxNoIdleCodewords;    /*!< output, number of egress user codeword */
    uint32_t    ifTxIdleCodewords;      /*!< output, number of egress idle codeword */
} PTM_CW_IF_ENTRY_T;

/*!
  \typedef PTM_FRAME_MIB_T
  \brief Wrapping of structure "ptm_frame_mib_t".
 */
/*!
  \struct ptm_frame_mib_t
  \brief Structure used for packet level MIB counters.
 */
typedef struct ptm_frame_mib_t {
    uint32_t    RxCorrect;      /*!< output, number of ingress packet */
    uint32_t    TC_CrcError;    /*!< output, number of egress packet with CRC error */
    uint32_t    RxDropped;      /*!< output, number of dropped ingress packet */
    uint32_t    TxSend;         /*!< output, number of egress packet */
} PTM_FRAME_MIB_T;

/*!
  \typedef IFX_PTM_CFG_T
  \brief Wrapping of structure "ptm_cfg_t".
 */
/*!
  \struct ptm_cfg_t
  \brief Structure used for ETH/TC CRC configuration.
 */
typedef struct ptm_cfg_t {
    uint32_t    RxEthCrcPresent;    /*!< input/output, ingress packet has ETH CRC */
    uint32_t    RxEthCrcCheck;      /*!< input/output, check ETH CRC of ingress packet */
    uint32_t    RxTcCrcCheck;       /*!< input/output, check TC CRC of ingress codeword */
    uint32_t    RxTcCrcLen;         /*!< input/output, length of TC CRC of ingress codeword */
    uint32_t    TxEthCrcGen;        /*!< input/output, generate ETH CRC for egress packet */
    uint32_t    TxTcCrcGen;         /*!< input/output, generate TC CRC for egress codeword */
    uint32_t    TxTcCrcLen;         /*!< input/output, length of TC CRC of egress codeword */
} IFX_PTM_CFG_T;

/*!
  \typedef IFX_PTM_PRIO_Q_MAP_T
  \brief Wrapping of structure "ppe_prio_q_map".
 */
/*!
  \struct ppe_prio_q_map
  \brief Structure used for Priority Value to TX Queue mapping.
 */
typedef struct ppe_prio_q_map {
    int             pkt_prio;
    int             qid;
    int             vpi;    //  ignored in eth interface
    int             vci;    //  ignored in eth interface
} IFX_PTM_PRIO_Q_MAP_T;

/*@}*/



/*
 * ####################################
 *                 API
 * ####################################
 */

#ifdef __KERNEL__
struct port_cell_info {
    unsigned int    port_num;
    unsigned int    tx_link_rate[2];
};
#endif



#endif  //  IFX_PTM_H

