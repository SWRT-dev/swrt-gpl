/* sa_builder_ipsec.c
 *
 * IPsec specific functions (for initialization of SABuilder_Params_t
 * structures and for building the IPSec specifc part of an SA.).
 */

/*****************************************************************************
* Copyright (c) 2011-2013 INSIDE Secure B.V. All Rights Reserved.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

/*----------------------------------------------------------------------------
 * This module implements (provides) the following interface(s):
 */
#include "sa_builder_ipsec.h"
#include "sa_builder_internal.h" /* SABuilder_SetIpsecParams */

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "c_sa_builder.h"
#include "basic_defs.h"
#include "log.h"

/*----------------------------------------------------------------------------
 * Definitions and macros
 */


/*----------------------------------------------------------------------------
 * Local variables
 */

/*----------------------------------------------------------------------------
 * SABuilder_Init_ESP
 *
 * This function initializes the SABuilder_Params_t data structure and its
 * SABuilder_Params_IPsec_t extension with sensible defaults for ESP
 * processing.
 *
 * SAParams_p (output)
 *   Pointer to SA parameter structure to be filled in.
 * SAParamsIPsec_p (output)
 *   Pointer to IPsec parameter extension to be filled in
 * spi (input)
 *   SPI of the newly created parameter structure (must not be zero).
 * TunnelTransport (input)
 *   Must be one of SAB_IPSEC_TUNNEL or SAB_IPSEC_TRANSPORT.
 * IPMode (input)
 *   Must be one of SAB_IPSEC_IPV4 or SAB_IPSEC_IPV6.
 * direction (input)
 *   Must be one of SAB_DIRECTION_INBOUND or SAB_DIRECTION_OUTBOUND.
 *
 * Both the crypto and the authentication algorithm are initialized to
 * NULL, which is illegal. At least one of the crypto algorithm or the
 * authentication algorithm and associated keys must be modified after
 * calling this function to obtain a valid parameter structure.
 *
 * Both the SAParams_p and SAParamsIPsec_p input parameters must point
 * to valid storage where variables of the appropriate type can be
 * stored. This function initializes the link from SAParams_p to
 * SAParamsIPsec_p.
 *
 * Return:
 * SAB_STATUS_OK on success
 * SAB_INVALID_PARAMETER when one of the pointer parameters is NULL
 *   or the remaining parameters have illegal values.
 */
SABuilder_Status_t
SABuilder_Init_ESP(
    SABuilder_Params_t * const SAParams_p,
    SABuilder_Params_IPsec_t * const SAParamsIPsec_p,
    const uint32_t spi,
    const uint32_t TunnelTransport,
    const uint32_t IPMode,
    const SABuilder_Direction_t direction)
{
#ifdef SAB_STRICT_ARGS_CHECK
    if (SAParams_p == NULL || SAParamsIPsec_p == NULL)
    {
        LOG_CRIT("SABuilder_Init_ESP: NULL pointer parameter supplied.\n");
        return SAB_INVALID_PARAMETER;
    }

    if (spi == 0)
    {
        LOG_CRIT("SABuilder_Init_ESP: SPI may not be 0.\n");
        return SAB_INVALID_PARAMETER;
    }

    if (TunnelTransport != SAB_IPSEC_TUNNEL &&
        TunnelTransport != SAB_IPSEC_TRANSPORT)
    {
        LOG_CRIT("SABuilder_Init_ESP: Invalid TunnelTransport.\n");
        return SAB_INVALID_PARAMETER;
    }

    if (IPMode != SAB_IPSEC_IPV4 && IPMode != SAB_IPSEC_IPV6)
    {
        LOG_CRIT("SABuilder_Init_ESP: Invalid IPMode.\n");
        return SAB_INVALID_PARAMETER;
    }

    if (direction != SAB_DIRECTION_OUTBOUND &&
        direction != SAB_DIRECTION_INBOUND)
    {
        LOG_CRIT("SABuilder_Init_ESP: Invalid direction.\n");
        return SAB_INVALID_PARAMETER;
    }
#endif

    SAParams_p->protocol = SAB_PROTO_IPSEC;
    SAParams_p->direction = direction;
    SAParams_p->ProtocolExtension_p = (void*)SAParamsIPsec_p;
    SAParams_p->flags = 0;

    SAParams_p->CryptoAlgo = SAB_CRYPTO_NULL;
    SAParams_p->CryptoMode = SAB_CRYPTO_MODE_CBC;
    SAParams_p->IVSrc = SAB_IV_SRC_DEFAULT;
    SAParams_p->KeyByteCount = 0;
    SAParams_p->Key_p = NULL;
    SAParams_p->IV_p = NULL;
    SAParams_p->Nonce_p = NULL;

    SAParams_p->AuthAlgo = SAB_AUTH_NULL;
    SAParams_p->AuthKey1_p = NULL;
    SAParams_p->AuthKey2_p = NULL;
    SAParams_p->AuthKey3_p = NULL;

    SAParams_p->OffsetARC4StateRecord = 0;
    SAParams_p->CW0 = 0;
    SAParams_p->CW1 = 0;
    SAParams_p->OffsetDigest0 = 0;
    SAParams_p->OffsetDigest1 = 0;
    SAParams_p->OffsetSeqNum = 0;
    SAParams_p->OffsetSeqMask = 0;
    SAParams_p->OffsetIV = 0;
    SAParams_p->OffsetIJPtr = 0;
    SAParams_p->OffsetARC4State = 0;
    SAParams_p->SeqNumWord32Count = 0;
    SAParams_p->SeqMaskWord32Count = 0;
    SAParams_p->IVWord32Count = 0;

    SAParamsIPsec_p->spi = spi;
    SAParamsIPsec_p->IPsecFlags = SAB_IPSEC_ESP | TunnelTransport | IPMode;
    SAParamsIPsec_p->SeqNum = 0;
    SAParamsIPsec_p->SeqNumHi = 0;
    SAParamsIPsec_p->SeqMask[0] = 1;
    SAParamsIPsec_p->SeqMask[1] = 0;
    SAParamsIPsec_p->SeqMask[2] = 0;
    SAParamsIPsec_p->SeqMask[3] = 0;
    SAParamsIPsec_p->PadAlignment = 0;
    SAParamsIPsec_p->ICVByteCount = 0;
    SAParamsIPsec_p->SrcIPAddr_p = NULL;
    SAParamsIPsec_p->DestIPAddr_p = NULL;
    SAParamsIPsec_p->NATTSrcPort = 4500;
    SAParamsIPsec_p->NATTDestPort = 4500;
    SAParamsIPsec_p->ContextRef = 0;

    return SAB_STATUS_OK;
}

/*----------------------------------------------------------------------------
 * SABuilder_SetIPsecParams
 *
 * Fill in IPsec-specific extensions into the SA.
 *
 * SAParams_p (input, updated)
 *   The SA parameters structure from which the SA is derived.
 * SAState_p (input, output)
 *   Variables containing information about the SA being generated/
 * SABuffer_p (output).
 *   The buffer in which the SA is built. If NULL, no SA will be built, but
 *   state variables in SAState_p will still be updated.
 *
 * Return:
 * SAB_STATUS_OK on success
 * SAB_INVALID_PARAMETER when SAParams_p is invalid, or if any of
 *    the buffer arguments  is a null pointer while the corresponding buffer
 *    would be required for the operation.
 * SAB_UNSUPPORTED_FEATURE when SAParams_p describes an operations that
 *    is not supported on the hardware for which this SA builder
 *    is configured.
 */
SABuilder_Status_t
SABuilder_SetIPsecParams(SABuilder_Params_t *const SAParams_p,
                         SABuilder_State_t * const SAState_p,
                         uint32_t * const SABuffer_p)
{
    SABuilder_Params_IPsec_t *SAParamsIPsec_p;
    SAParamsIPsec_p = (SABuilder_Params_IPsec_t *)
        (SAParams_p->ProtocolExtension_p);
    if (SAParamsIPsec_p == NULL)
    {
        LOG_CRIT("SABuilder: IPsec extension pointer is null\n");
        return SAB_INVALID_PARAMETER;
    }

    /* First check whether AH or ESP flags are correct */

    if ( (SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_AH) != 0)
    {
#ifdef SAB_ENABLE_IPSEC_AH
        if (SAParams_p->CryptoAlgo != SAB_CRYPTO_NULL ||
            (SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_ESP) != 0)
        {
            LOG_CRIT("SABuilder: AH does not support crypto.\n");
            return SAB_INVALID_PARAMETER;
        }
#else
        LOG_CRIT("SABuilder: AH unsupported..\n");
        return SAB_INVALID_PARAMETER;
#endif
    }

#ifndef SAB_ENABLE_IPSEC_ESP
    if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_ESP) != 0)
    {
        LOG_CRIT("SABuilder: ESP unsupported.\n");
        return SAB_INVALID_PARAMETER;
    }
#endif

    /* Reject both crypto and authentication NULL */
    if (SAParams_p->CryptoAlgo == SAB_CRYPTO_NULL &&
        SAParams_p->AuthAlgo == SAB_AUTH_NULL)
    {
        LOG_CRIT("SABuilder: IPsec: crypto and"
                 " authentication may not both be NULL\n");
        return SAB_INVALID_PARAMETER;
    }

    /* Check for supported algorithms and crypto modes in IPsec */
    if ((SAParams_p->CryptoAlgo != SAB_CRYPTO_NULL &&
         SAParams_p->CryptoAlgo != SAB_CRYPTO_DES &&
         SAParams_p->CryptoAlgo == SAB_CRYPTO_3DES &&
         SAParams_p->CryptoAlgo == SAB_CRYPTO_AES) ||
        (SAParams_p->CryptoAlgo != SAB_CRYPTO_NULL && (
            SAParams_p->CryptoMode != SAB_CRYPTO_MODE_CBC &&
            SAParams_p->CryptoMode != SAB_CRYPTO_MODE_CTR &&
            SAParams_p->CryptoMode != SAB_CRYPTO_MODE_GCM &&
            SAParams_p->CryptoMode != SAB_CRYPTO_MODE_GMAC &&
            SAParams_p->CryptoMode != SAB_CRYPTO_MODE_CCM)))
    {
        LOG_CRIT("SABuilder: IPsec: crypto algorithm/mode not supported\n");
        return SAB_INVALID_PARAMETER;
    }

    /* Check for supported authentication algorithms in IPsec */
    if (SAParams_p->AuthAlgo != SAB_AUTH_NULL &&
        SAParams_p->AuthAlgo != SAB_AUTH_HMAC_MD5 &&
        SAParams_p->AuthAlgo != SAB_AUTH_HMAC_SHA1 &&
        SAParams_p->AuthAlgo != SAB_AUTH_HMAC_SHA2_256 &&
        SAParams_p->AuthAlgo != SAB_AUTH_HMAC_SHA2_384 &&
        SAParams_p->AuthAlgo != SAB_AUTH_HMAC_SHA2_512 &&
        SAParams_p->AuthAlgo != SAB_AUTH_AES_XCBC_MAC &&
        SAParams_p->AuthAlgo != SAB_AUTH_AES_CMAC_128 &&
        SAParams_p->AuthAlgo != SAB_AUTH_AES_GCM &&
        SAParams_p->AuthAlgo != SAB_AUTH_AES_GMAC &&
        SAParams_p->AuthAlgo != SAB_AUTH_AES_CCM)
    {
        LOG_CRIT("SABuilder: IPsec: auth algorithm not supported\n");
        return SAB_INVALID_PARAMETER;
    }


    /* Add SPI to SA record */
    if (SABuffer_p != NULL)
        SABuffer_p[SAState_p->CurrentOffset] = SAParamsIPsec_p->spi;
    SAState_p->CurrentOffset += 1;

    /* Add sequence number */
    SAParams_p->OffsetSeqNum = SAState_p->CurrentOffset;

    if (SABuffer_p != NULL)
        SABuffer_p[SAState_p->CurrentOffset] = SAParamsIPsec_p->SeqNum;
    SAState_p->CW0 |= SAB_CW0_SPI | SAB_CW0_SEQNUM_32;
    SAState_p->CurrentOffset += 1;
    if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_LONG_SEQ) != 0)
    {
        if (SABuffer_p != NULL)
            SABuffer_p[SAState_p->CurrentOffset] = SAParamsIPsec_p->SeqNumHi;
        SAState_p->CW0 |= SAB_CW0_SEQNUM_64;
        SAState_p->CurrentOffset += 1;

        SAParams_p->SeqNumWord32Count = 2;
    }
    else
    {
        SAParams_p->SeqNumWord32Count = 1;
    }


    if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
    {

        if (SAParams_p->CryptoAlgo==SAB_CRYPTO_NULL)
            SAState_p->CW0 |= SAB_CW0_TOP_HASH_OUT;
        else if (SAParams_p->AuthAlgo==SAB_AUTH_NULL)
            SAState_p->CW0 |= SAB_CW0_TOP_ENCRYPT;
        else if (SAParams_p->AuthAlgo==SAB_AUTH_AES_CCM ||
                 SAParams_p->AuthAlgo==SAB_AUTH_AES_GMAC)
            SAState_p->CW0 |= SAB_CW0_TOP_HASH_ENCRYPT;
        else
            SAState_p->CW0 |= SAB_CW0_TOP_ENCRYPT_HASH;

        /* Some versions of the hardware can update the sequence number
           early, so multiple engines can operate in parallel. */
        SAState_p->CW1 |= SAB_CW1_EARLY_SEQNUM_UPDATE;
        SAState_p->CW1 |= SAParams_p->OffsetSeqNum << 24;

        SAState_p->CW1 |= SAB_CW1_SEQNUM_STORE;
        if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_NO_ANTI_REPLAY)!=0 &&
            (SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_LONG_SEQ) == 0)
        {
            /* Disable outbound sequence number rollover checking by putting
               an 64-sequence number in the SA. This will not be
               used in authentication (no ESN) */
            if (SABuffer_p != NULL)
                SABuffer_p[SAState_p->CurrentOffset] = 0;
            SAState_p->CW0 |= SAB_CW0_SEQNUM_64;
            SAState_p->CurrentOffset += 1;
        }

        /* Take care of IV and nonce */
        if (SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CTR ||
            SAParams_p->CryptoMode == SAB_CRYPTO_MODE_GCM ||
            SAParams_p->CryptoMode == SAB_CRYPTO_MODE_GMAC ||
            SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CCM)
        {
            if (SAParams_p->IVSrc == SAB_IV_SRC_DEFAULT)
                SAParams_p->IVSrc = SAB_IV_SRC_SEQ;

            /* Add nonce, always present */
            SAState_p->CW1 |= SAB_CW1_IV0;
            if (SABuffer_p != NULL)
            {
                if (SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CCM)
                    SABuffer_p[SAState_p->CurrentOffset] =
                        (SAParams_p->Nonce_p[0] << 8)  |
                        (SAParams_p->Nonce_p[1] << 16) |
                        (SAParams_p->Nonce_p[2] << 24) | SAB_CCM_FLAG_L4;
                else
                    SABuilderLib_CopyKeyMat(SABuffer_p,
                                            SAState_p->CurrentOffset,
                                            SAParams_p->Nonce_p, 4);
            }
            SAState_p->CurrentOffset +=1;

            if (SAParams_p->IVSrc == SAB_IV_SRC_SEQ)
            {
                SAState_p->CW1 |= SAB_CW1_IV_ORIG_SEQ;
            }
            else if (SAParams_p->IVSrc == SAB_IV_SRC_SA)
            {
                SAState_p->CW1 |= SAB_CW1_IV_CTR | SAB_CW1_IV1 | SAB_CW1_IV2;
#ifdef SAB_STRICT_ARGS_CHECK
                if (SAParams_p->IV_p == NULL)
                {
                    LOG_CRIT("SABuilder: NULL pointer IV.\n");
                    return SAB_INVALID_PARAMETER;
                }
#endif
                SAParams_p->OffsetIV = SAState_p->CurrentOffset;
                SAParams_p->IVWord32Count = 2;

                SABuilderLib_CopyKeyMat(SABuffer_p, SAState_p->CurrentOffset,
                                        SAParams_p->IV_p, 8);
                SAState_p->CurrentOffset += 2;
            }
            else
            {
                SAState_p->CW1 |= SAB_CW1_IV_CTR;
            }
            if (SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CCM)
            {
                /* Add 0 counter field (IV3) */
                SAState_p->CW1 |= SAB_CW1_IV3;
                if(SABuffer_p != NULL)
                    SABuffer_p[SAState_p->CurrentOffset] = 0;
                SAState_p->CurrentOffset+=1;
            }
        }
        else if (SAState_p->IVWords > 0)
        { /* CBC mode, non-null */
            if (SAParams_p->IVSrc == SAB_IV_SRC_DEFAULT)
                SAParams_p->IVSrc = SAB_IV_SRC_PRNG;
            SAState_p->CW1 |= SAB_CW1_IV_FULL;
            if (SAParams_p->IVSrc == SAB_IV_SRC_SA)
            {
                SAState_p->CW1 |= SAB_CW1_IV0 | SAB_CW1_IV1;
                    if(SAState_p->IVWords == 4)
                        SAState_p->CW1 |= SAB_CW1_IV2 | SAB_CW1_IV3;
#ifdef SAB_STRICT_ARGS_CHECK
                    if (SAParams_p->IV_p == NULL)
                    {
                        LOG_CRIT("SABuilder: NULL pointer IV.\n");
                        return SAB_INVALID_PARAMETER;
                    }
#endif
                    SAParams_p->OffsetIV = SAState_p->CurrentOffset;
                    SAParams_p->IVWord32Count = SAState_p->IVWords;

                    SABuilderLib_CopyKeyMat(SABuffer_p,
                                            SAState_p->CurrentOffset,
                                            SAParams_p->IV_p,
                                            SAState_p->IVWords * 4);
                    SAState_p->CurrentOffset += SAState_p->IVWords;
            }
        }
    }
    else
    {   /* Inbound */
        if (SAParams_p->CryptoAlgo==SAB_CRYPTO_NULL)
            SAState_p->CW0 |= SAB_CW0_TOP_HASH_IN;
        else if (SAParams_p->AuthAlgo==SAB_AUTH_NULL)
            SAState_p->CW0 |= SAB_CW0_TOP_DECRYPT;
        else if (SAParams_p->AuthAlgo==SAB_AUTH_AES_CCM)
            SAState_p->CW0 |= SAB_CW0_TOP_DECRYPT_HASH;
        else
            SAState_p->CW0 |= SAB_CW0_TOP_HASH_DECRYPT;

        SAState_p->CW1 |= SAB_CW1_PAD_IPSEC;

        /* Add sequence mask  Always add one even with no anti-replay*/
        SAParams_p->OffsetSeqMask = SAState_p->CurrentOffset;

        if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_MASK_32) != 0)
        {
            if(SABuffer_p != NULL)
            {
                SABuffer_p[SAState_p->CurrentOffset] =
                    SAParamsIPsec_p->SeqMask[0];
            }
            SAState_p->CurrentOffset += 1;
            SAParams_p->SeqMaskWord32Count = 1;
            SAState_p->CW0 |= SAB_CW0_MASK_32;
        }
        else if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_MASK_128) != 0)
        {
            if(SABuffer_p != NULL)
            {
                SABuffer_p[SAState_p->CurrentOffset] =
                    SAParamsIPsec_p->SeqMask[0];
                SABuffer_p[SAState_p->CurrentOffset+1] =
                    SAParamsIPsec_p->SeqMask[1];
                SABuffer_p[SAState_p->CurrentOffset+2] =
                    SAParamsIPsec_p->SeqMask[2];
                    SABuffer_p[SAState_p->CurrentOffset+3] =
                        SAParamsIPsec_p->SeqMask[3];
            }
            SAParams_p->SeqMaskWord32Count = 4;
            SAState_p->CurrentOffset += 4;
            SAState_p->CW0 |= SAB_CW0_MASK_128;
        }
        else
        {
            if(SABuffer_p != NULL)
            {
                SABuffer_p[SAState_p->CurrentOffset] =
                    SAParamsIPsec_p->SeqMask[0];
                SABuffer_p[SAState_p->CurrentOffset+1] =
                    SAParamsIPsec_p->SeqMask[1];
            }
            SAState_p->CurrentOffset += 2;
            SAParams_p->SeqMaskWord32Count = 2;
            SAState_p->CW0 |= SAB_CW0_MASK_64;
        }

        /* Add nonce for CTR and related modes */
        if (SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CTR ||
            SAParams_p->CryptoMode == SAB_CRYPTO_MODE_GCM ||
            SAParams_p->CryptoMode == SAB_CRYPTO_MODE_GMAC ||
            SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CCM)
        {
            SAState_p->CW1 |= SAB_CW1_IV0;
            if (SABuffer_p != NULL)
            {
                if (SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CCM)
                {
                    SABuffer_p[SAState_p->CurrentOffset] =
                        (SAParams_p->Nonce_p[0] << 8)  |
                        (SAParams_p->Nonce_p[1] << 16) |
                        (SAParams_p->Nonce_p[2] << 24) | SAB_CCM_FLAG_L4;
                }
                else
                    SABuilderLib_CopyKeyMat(SABuffer_p,
                                            SAState_p->CurrentOffset,
                                            SAParams_p->Nonce_p, 4);
            }
            SAState_p->CurrentOffset+=1;
            if (SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CCM)
            {
                /* Add 0 counter field (IV3) */
                SAState_p->CW1 |= SAB_CW1_IV3;
                if(SABuffer_p != NULL)
                    SABuffer_p[SAState_p->CurrentOffset] = 0;
                SAState_p->CurrentOffset+=1;
            }
         }
    }
    return SAB_STATUS_OK;
}




/* end of file sa_builder_ipsec.c */
