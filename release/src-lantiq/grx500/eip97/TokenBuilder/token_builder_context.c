/* token_builder.c
 *
 * This is the main implementation module for the EIP-96 Token Builder.
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
#include "token_builder.h"


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "c_token_builder.h"
#include "basic_defs.h"
#include "log.h"
#include "sa_builder_params.h"
#include "sa_builder_params_ipsec.h"

#include "token_builder_internal.h"
#include "sa_builder_params.h"
#ifdef TKB_ENABLE_PROTO_IPSEC
#include "sa_builder_params_ipsec.h"
#endif
#ifdef TKB_ENABLE_PROTO_SSLTLS
#include "sa_builder_params_ssltls.h"
#endif
#ifdef TKB_ENABLE_PROTO_BASIC
#include "sa_builder_params_basic.h"
#endif
#ifdef TKB_ENABLE_PROTO_SRTP
#include "sa_builder_params_srtp.h"
#endif


/*----------------------------------------------------------------------------
 * Definitions and macros
 */



/*----------------------------------------------------------------------------
 * TokenBuilder_GetContextSize
 *
 * Determine the size of the token context record in 32-bit words, which may
 * depend on the SA parameters.
 *
 * SAParams_p (input)
 *   Points to the SA parameters data structure
 * TokenContextWord32Count_p (output)
 *   Required size of the Token Context Record in 32-bit words.
 *
 * Note: This implementation has one Token Context Record format with a fixed
 *       size. Therefore this function does not look at the SA parameters.
 *
 * Return:
 * TKB_STATUS_OK if successful.
 * TKB_INVALID_PARAMETER if any of the pointer parameters is NULL or if
 *                      the contents of the input data structures are invalid.
 */
TokenBuilder_Status_t
TokenBuilder_GetContextSize(
    const SABuilder_Params_t * const SAParams_p,
    unsigned int * const TokenContextWord32Count_p
    )
{
#ifdef TKB_STRICT_ARGS_CHECK
    if (SAParams_p == NULL || TokenContextWord32Count_p == NULL)
    {
        LOG_CRIT("TokenBuilder_GetContextSize: NULL pointer supplied\n");
        return TKB_INVALID_PARAMETER;
    }
#endif
    *TokenContextWord32Count_p =
        ((unsigned int)sizeof(TokenBuilder_Context_t) + 3)/4;
    return TKB_STATUS_OK;
}

/*----------------------------------------------------------------------------
 * TokenBuilder_BuildContext
 *
 * Create a Token Context Record.
 *
 * SAParams_p (input)
 *   Points to the SA parameters data structure. It is important that
 *   SABuilder_GetSIze() and SABuilder_BuildSA() have already been called
 *   for this SA.
 * TokenContext_p (output)
 *   Buffer to hold the Token Context Record. It must point to a valid
 *   storage buffer that is large enough to hold the Token
 *   Context. Before calling this function, the application must call
 *   TokeBuilder_GetContextSize() with the same SA parameters and
 *   allocate a buffer of that size.
 *
 * Return:
 * TKB_STATUS_OK if successful.
 * TKB_INVALID_PARAMETER if any of the pointer parameters is NULL or if
 *                      the contents of the SA parameters are invalid.
 */
TokenBuilder_Status_t
TokenBuilder_BuildContext(
    const SABuilder_Params_t * const SAParams_p,
    void * const TokenContext_p
    )
{
    TokenBuilder_Context_t *TokenContext_Internal_p =
        (TokenBuilder_Context_t *)TokenContext_p;
#ifdef TKB_STRICT_ARGS_CHECK
    if (SAParams_p == NULL || TokenContext_p == NULL)
    {
        LOG_CRIT("TokenBuilder_BuildContext: NULL pointer supplied\n");
        return TKB_INVALID_PARAMETER;
    }
#endif

    TokenContext_Internal_p->TokenHeaderWord = TKB_HEADER_DEFAULT;

    TokenContext_Internal_p->u.generic.CW0 = SAParams_p->CW0;
    TokenContext_Internal_p->u.generic.CW1 = SAParams_p->CW1;

    switch (SAParams_p->protocol)
    {
#ifdef TKB_ENABLE_PROTO_IPSEC
    case SAB_PROTO_IPSEC:
    {
        SABuilder_Params_IPsec_t *SAParamsIPsec_p;
        SAParamsIPsec_p = (SABuilder_Params_IPsec_t *)
            (SAParams_p->ProtocolExtension_p);
        if (SAParamsIPsec_p == NULL)
        {
            LOG_CRIT("TokenBuilder_BuildContext:"
                     " IPsec extension pointer is NULL\n");
            return TKB_INVALID_PARAMETER;
        }
        if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_ESP) != 0)
        {
#ifdef TKB_ENABLE_IPSEC_ESP
            TokenContext_Internal_p->ExtSeq = 0;
            if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_NO_ANTI_REPLAY) != 0)
                TokenContext_Internal_p->AntiReplay = 0;
            else
                TokenContext_Internal_p->AntiReplay = 1;

            if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
            {
                TokenContext_Internal_p->protocol = TKB_PROTO_ESP_OUT;
                TokenContext_Internal_p->PadBlockByteCount = 4;
                TokenContext_Internal_p->IVHandling = TKB_IV_OUTBOUND_CBC;
                if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_LONG_SEQ) != 0)
                    TokenContext_Internal_p->ExtSeq = 1;
            }
            else
            {
                TokenContext_Internal_p->protocol = TKB_PROTO_ESP_IN;
                TokenContext_Internal_p->PadBlockByteCount = 4;
                TokenContext_Internal_p->TokenHeaderWord |=
                    TKB_HEADER_PAD_VERIFY;
                TokenContext_Internal_p->IVHandling = TKB_IV_INBOUND_CBC;
                if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_LONG_SEQ) != 0)
                    TokenContext_Internal_p->ExtSeq = 1;
                if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_MASK_128) != 0)
                    TokenContext_Internal_p->AntiReplay *= 4;
                else if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_MASK_32) == 0)
                    TokenContext_Internal_p->AntiReplay *= 2;
            }
            TokenContext_Internal_p->SeqOffset = SAParams_p->OffsetSeqNum;

            switch (SAParams_p->CryptoAlgo)
            {
            case SAB_CRYPTO_NULL:
                TokenContext_Internal_p->IVByteCount = 0;
                break;
            case SAB_CRYPTO_DES:
            case SAB_CRYPTO_3DES:
                TokenContext_Internal_p->IVByteCount = 8;
                TokenContext_Internal_p->PadBlockByteCount = 8;
                break;
            case SAB_CRYPTO_AES:
                if (SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CBC)
                {
                    TokenContext_Internal_p->IVByteCount = 16;
                    TokenContext_Internal_p->PadBlockByteCount = 16;
                }
                else
                {
                    if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
                        TokenContext_Internal_p->IVHandling =
                            TKB_IV_OUTBOUND_CTR;
                    else
                        TokenContext_Internal_p->IVHandling =
                            TKB_IV_INBOUND_CTR;

                    TokenContext_Internal_p->IVByteCount = 8;
                }
                break;
            default:
                LOG_CRIT("TokenBuilder_BuildContext:"
                         " unsupported crypto algorithm.\n");
                return TKB_INVALID_PARAMETER;
            }

            /* For all inbound and CTR mode outbound packets there is
               only one supported way to obtain the IV, which is already
               taken care of. Now handle outbound CBC. */
            if(SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CBC &&
               SAParams_p->direction == SAB_DIRECTION_OUTBOUND &&
               SAParams_p->CryptoAlgo != SAB_CRYPTO_NULL)
            {
                switch (SAParams_p->IVSrc)
                {
                case SAB_IV_SRC_DEFAULT:
                case SAB_IV_SRC_PRNG:
                    TokenContext_Internal_p->TokenHeaderWord |=
                        TKB_HEADER_IV_PRNG;
                    break;
                case SAB_IV_SRC_SA: /* No action required */
                    break;
                case SAB_IV_SRC_TOKEN:
                    if (TokenContext_Internal_p->IVByteCount == 8)
                    {
                        TokenContext_Internal_p->IVHandling =
                            TKB_IV_OUTBOUND_TOKEN_2WORDS;
                        TokenContext_Internal_p->TokenHeaderWord |=
                            TKB_HEADER_IV_TOKEN_2WORDS;
                    }
                    else
                    {
                        TokenContext_Internal_p->IVHandling =
                            TKB_IV_OUTBOUND_TOKEN_4WORDS;
                        TokenContext_Internal_p->TokenHeaderWord |=
                            TKB_HEADER_IV_TOKEN_4WORDS;
                    }
                    break;
                default:
                    LOG_CRIT("TokenBuilder_BuildContext:"
                             "Unsupported IV source\n");
                    return TKB_INVALID_PARAMETER;
                }
            }

            if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND &&
                SAParamsIPsec_p->PadAlignment >
                TokenContext_Internal_p->PadBlockByteCount &&
                SAParamsIPsec_p->PadAlignment <= 256)
                TokenContext_Internal_p->PadBlockByteCount =
                    SAParamsIPsec_p->PadAlignment;

            switch(SAParams_p->AuthAlgo)
            {
            case SAB_AUTH_NULL:
                TokenContext_Internal_p->ICVByteCount = 0;
                TokenContext_Internal_p->ExtSeq = 0;
                break;
            case SAB_AUTH_HMAC_MD5:
            case SAB_AUTH_HMAC_SHA1:
            case SAB_AUTH_AES_XCBC_MAC:
            case SAB_AUTH_AES_CMAC_128:
                TokenContext_Internal_p->ICVByteCount = 12;
                break;
            case SAB_AUTH_HMAC_SHA2_224:
            case SAB_AUTH_HMAC_SHA2_256:
                TokenContext_Internal_p->ICVByteCount = 16;
                break;
            case SAB_AUTH_HMAC_SHA2_384:
                TokenContext_Internal_p->ICVByteCount = 24;
                break;
            case SAB_AUTH_HMAC_SHA2_512:
                TokenContext_Internal_p->ICVByteCount = 32;
                break;
            case SAB_AUTH_AES_CCM:
            case SAB_AUTH_AES_GCM:
            case SAB_AUTH_AES_GMAC:
                // All these protocols have a selectable ICV length.
                if (SAParamsIPsec_p->ICVByteCount == 8 ||
                    SAParamsIPsec_p->ICVByteCount == 12 ||
                    SAParamsIPsec_p->ICVByteCount == 16)
                {
                    TokenContext_Internal_p->ICVByteCount =
                        SAParamsIPsec_p->ICVByteCount;
                }
                else
                {
                    TokenContext_Internal_p->ICVByteCount = 16;
                }
                switch (SAParams_p->AuthAlgo)
                {
                /* These protocols need specialized protocol codes
                   for the token generator.*/
                case SAB_AUTH_AES_CCM:
                    if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
                    {
                        TokenContext_Internal_p->protocol = TKB_PROTO_ESP_CCM_OUT;
                    }
                    else
                    {
                        TokenContext_Internal_p->protocol = TKB_PROTO_ESP_CCM_IN;
                    }
                    TokenContext_Internal_p->u.generic.CCMSalt =
                        (SAParams_p->Nonce_p[0] << 8) |
                        (SAParams_p->Nonce_p[1] << 16) |
                        (SAParams_p->Nonce_p[2] << 24) |
                        TKB_CCM_FLAG_ADATA_L4 |
                        ((TokenContext_Internal_p->ICVByteCount-2)*4);
                    break;
                case SAB_AUTH_AES_GCM:
                    if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
                    {
                        TokenContext_Internal_p->protocol = TKB_PROTO_ESP_GCM_OUT;
                    }
                    else
                    {
                        TokenContext_Internal_p->protocol = TKB_PROTO_ESP_GCM_IN;
                    }
                    break;
                case SAB_AUTH_AES_GMAC:
                    if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
                    {
                        TokenContext_Internal_p->protocol = TKB_PROTO_ESP_GMAC_OUT;
                    }
                    else
                    {
                        TokenContext_Internal_p->protocol = TKB_PROTO_ESP_GMAC_IN;
                    }
                    break;
                default:
                    ;
                }
                break;
            default:
                LOG_CRIT("TokenBuilder_BuildContext:"
                         " unsupported authentication algorithm.\n");
                return TKB_INVALID_PARAMETER;
            }
#else /* TKB_ENABLE_IPSEC_ESP */
            LOG_CRIT("TokenBuilder_BuildContext: ESP not supported\n");
            return TKB_INVALID_PARAMETER;
#endif /* TKB_ENABLE_IPSEC_ESP */
        }
        else if ((SAParamsIPsec_p->IPsecFlags & SAB_IPSEC_AH) != 0)
        {
#ifdef TKB_ENABLE_IPSEC_AH

#else /* TKB_ENABLE_IPSEC_AH */
            LOG_CRIT("TokenBuilder_BuildContext: AH not supported\n");
            return TKB_INVALID_PARAMETER;
#endif /* TKB_ENABLE_IPSEC_AH */
        }
        else
        {
            LOG_CRIT("TokenBuilder_BuildContext: Neither ESP nor AH set\n");
            return TKB_INVALID_PARAMETER;
        }
    }
    break;
#endif  /* TKB_ENABLE_PROTO_IPSEC */
#ifdef TKB_ENABLE_PROTO_SSLTLS
    case SAB_PROTO_SSLTLS:
    {
        SABuilder_Params_SSLTLS_t *SAParamsSSLTLS_p;
        SAParamsSSLTLS_p = (SABuilder_Params_SSLTLS_t *)
            (SAParams_p->ProtocolExtension_p);
        if (SAParamsSSLTLS_p == NULL)
        {
            LOG_CRIT("TokenBuilder_BuildContext:"
                     " SSL/TLS extension pointer is NULL\n");
            return TKB_INVALID_PARAMETER;
        }

        TokenContext_Internal_p->ExtSeq = 0;
        TokenContext_Internal_p->AntiReplay = 1;
        TokenContext_Internal_p->IVOffset = 0;
        TokenContext_Internal_p->DigestWordCount = 0;
        if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
        {
            TokenContext_Internal_p->protocol = TKB_PROTO_SSLTLS_OUT;
            TokenContext_Internal_p->PadBlockByteCount = 1;
            TokenContext_Internal_p->IVHandling = TKB_IV_OUTBOUND_CBC;
        }
        else
        {
            TokenContext_Internal_p->protocol = TKB_PROTO_SSLTLS_IN;
            TokenContext_Internal_p->PadBlockByteCount = 1;
            if (SAParams_p->CryptoAlgo != SAB_CRYPTO_NULL &&
                SAParams_p->CryptoAlgo != SAB_CRYPTO_ARCFOUR)
                TokenContext_Internal_p->TokenHeaderWord |=
                    TKB_HEADER_PAD_VERIFY;
            TokenContext_Internal_p->IVHandling = TKB_IV_INBOUND_CBC;
        }
        TokenContext_Internal_p->SeqOffset = SAParams_p->OffsetSeqNum;

        switch (SAParams_p->CryptoAlgo)
        {
        case SAB_CRYPTO_NULL:
            TokenContext_Internal_p->IVByteCount = 0;
            TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;
            break;
        case SAB_CRYPTO_ARCFOUR:
            TokenContext_Internal_p->IVByteCount = 0;
            TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_ARC4;
            TokenContext_Internal_p->IVOffset = SAParams_p->OffsetIJPtr;
            break;
        case SAB_CRYPTO_DES:
        case SAB_CRYPTO_3DES:
            TokenContext_Internal_p->IVByteCount = 8;
            TokenContext_Internal_p->PadBlockByteCount = 8;
            TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_IV2;
            TokenContext_Internal_p->IVOffset = SAParams_p->OffsetIV;
            break;
        case SAB_CRYPTO_AES:
            if (SAParams_p->CryptoMode == SAB_CRYPTO_MODE_GCM)
            {
                TokenContext_Internal_p->IVByteCount = 8;
                TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;
                TokenContext_Internal_p->IVOffset = SAParams_p->OffsetIV;
                if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
                {
                    TokenContext_Internal_p->IVHandling = TKB_IV_OUTBOUND_CTR;
                    TokenContext_Internal_p->protocol =
                        TKB_PROTO_SSLTLS_GCM_OUT;
                }
                else
                {
                    TokenContext_Internal_p->IVHandling = TKB_IV_INBOUND_CTR;
                    TokenContext_Internal_p->protocol = TKB_PROTO_SSLTLS_GCM_IN;
                }
            }
            else
            {
                TokenContext_Internal_p->IVByteCount = 16;
                TokenContext_Internal_p->PadBlockByteCount = 16;
                TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_IV4;
                TokenContext_Internal_p->IVOffset = SAParams_p->OffsetIV;
            }
            break;
        default:
            LOG_CRIT("TokenBuilder_BuildContext:"
                     " unsupported crypto algorithm.\n");
            return TKB_INVALID_PARAMETER;
        }

        /* For all inbound and ARCFOUR outbound packets there is
           only one supported way to obtain the IV, which is already
           taken care of. Now handle outbound CBC. */
        if(SAParams_p->CryptoMode == SAB_CRYPTO_MODE_CBC &&
           SAParams_p->direction == SAB_DIRECTION_OUTBOUND &&
           SAParams_p->CryptoAlgo != SAB_CRYPTO_NULL)
        {
            switch (SAParams_p->IVSrc)
            {
            case SAB_IV_SRC_DEFAULT:
            case SAB_IV_SRC_PRNG:
                TokenContext_Internal_p->TokenHeaderWord |=
                    TKB_HEADER_IV_PRNG;
                break;
            case SAB_IV_SRC_SA: /* No action required */
                break;
            case SAB_IV_SRC_TOKEN:
                if (TokenContext_Internal_p->IVByteCount == 8)
                {
                    TokenContext_Internal_p->IVHandling =
                        TKB_IV_OUTBOUND_TOKEN_2WORDS;
                    TokenContext_Internal_p->TokenHeaderWord |=
                        TKB_HEADER_IV_TOKEN_2WORDS;
                }
                else
                {
                    TokenContext_Internal_p->IVHandling =
                        TKB_IV_OUTBOUND_TOKEN_4WORDS;
                    TokenContext_Internal_p->TokenHeaderWord |=
                        TKB_HEADER_IV_TOKEN_4WORDS;
                }
                break;
            default:
                LOG_CRIT("TokenBuilder_BuildContext:"
                         "Unsupported IV source\n");
                return TKB_INVALID_PARAMETER;
            }
        }

        /* In SSL and TLS1.0 the IV does not appear explicitly in the packet */
        if (SAParamsSSLTLS_p->version == SAB_SSL_VERSION_3_0 ||
            SAParamsSSLTLS_p->version == SAB_TLS_VERSION_1_0)
            TokenContext_Internal_p->IVByteCount = 0;
        else if (TokenContext_Internal_p->u.generic.UpdateHandling == TKB_UPD_IV2 ||
                 TokenContext_Internal_p->u.generic.UpdateHandling == TKB_UPD_IV4)
            TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_BLK;

        /* Sequence numbers appear explicitly in the packet in DTLS */
        if (SAParamsSSLTLS_p->version == SAB_DTLS_VERSION_1_0 ||
            SAParamsSSLTLS_p->version == SAB_DTLS_VERSION_1_2)
        {
            if ((SAParamsSSLTLS_p->SSLTLSFlags & SAB_DTLS_CAPWAP) != 0)
                TokenContext_Internal_p->DigestWordCount = 1;

            if ((SAParamsSSLTLS_p->SSLTLSFlags & SAB_DTLS_NO_ANTI_REPLAY) != 0)
                TokenContext_Internal_p->ExtSeq = 1;
            else if ((SAParamsSSLTLS_p->SSLTLSFlags & SAB_DTLS_MASK_128) != 0)
                TokenContext_Internal_p->ExtSeq = 5;
            else if ((SAParamsSSLTLS_p->SSLTLSFlags & SAB_DTLS_MASK_32) != 0)
                TokenContext_Internal_p->ExtSeq = 2;
            else
                TokenContext_Internal_p->ExtSeq = 3;
        }

        /* Version field is not hashed in SSL 3.0. */
        if (SAParamsSSLTLS_p->version == SAB_SSL_VERSION_3_0)
            TokenContext_Internal_p->AntiReplay = 0;


        switch(SAParams_p->AuthAlgo)
        {
        case SAB_AUTH_HMAC_MD5:
        case SAB_AUTH_SSLMAC_MD5:
            TokenContext_Internal_p->ICVByteCount = 16;
            break;
        case SAB_AUTH_HMAC_SHA1:
        case SAB_AUTH_SSLMAC_SHA1:
            TokenContext_Internal_p->ICVByteCount = 20;
            break;
        case SAB_AUTH_HMAC_SHA2_224:
            TokenContext_Internal_p->ICVByteCount = 28;
            break;
        case SAB_AUTH_HMAC_SHA2_256:
            TokenContext_Internal_p->ICVByteCount = 32;
            break;
        case SAB_AUTH_HMAC_SHA2_384:
            TokenContext_Internal_p->ICVByteCount = 48;
            break;
        case SAB_AUTH_HMAC_SHA2_512:
            TokenContext_Internal_p->ICVByteCount = 64;
            break;
        case SAB_AUTH_AES_GCM:
            TokenContext_Internal_p->ICVByteCount = 16;
            break;
        default:
            LOG_CRIT("TokenBuilder_BuildContext:"
                     " unsupported authentication algorithm.\n");
            return TKB_INVALID_PARAMETER;
        }

    }
    break;
#endif
#ifdef TKB_ENABLE_PROTO_BASIC
    case SAB_PROTO_BASIC:
    {
        SABuilder_Params_Basic_t *SAParamsBasic_p;
        SAParamsBasic_p = (SABuilder_Params_Basic_t *)
            (SAParams_p->ProtocolExtension_p);
        if (SAParamsBasic_p == NULL)
        {
            LOG_CRIT("TokenBuilder_BuildContext:"
                     " Basic extension pointer is NULL\n");
            return TKB_INVALID_PARAMETER;
        }

        TokenContext_Internal_p->ExtSeq = 0;
        TokenContext_Internal_p->AntiReplay = 0;
        TokenContext_Internal_p->IVOffset = 0;
        TokenContext_Internal_p->SeqOffset = 0;
        TokenContext_Internal_p->PadBlockByteCount = 1;
        TokenContext_Internal_p->IVByteCount = 0;
        TokenContext_Internal_p->ICVByteCount = 0;
        TokenContext_Internal_p->DigestWordCount = 0;
        TokenContext_Internal_p->IVHandling = TKB_IV_OUTBOUND_CBC;
        TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;

        if (SAParams_p->CryptoAlgo != SAB_CRYPTO_NULL)
        {   /* Basic crypto */
            if (SAParams_p->AuthAlgo == SAB_AUTH_NULL)
                TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_CRYPTO;
            else
                TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_CRYPTHASH;
            switch (SAParams_p->CryptoAlgo)
            {
            case SAB_CRYPTO_ARCFOUR:
                TokenContext_Internal_p->IVByteCount = 0;
                if (SAParams_p->CryptoMode != SAB_CRYPTO_MODE_STATELESS)
                    TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_ARC4;
                TokenContext_Internal_p->IVOffset = SAParams_p->OffsetIJPtr;
                break;
            case SAB_CRYPTO_DES:
            case SAB_CRYPTO_3DES:
                TokenContext_Internal_p->IVByteCount = 8;
                TokenContext_Internal_p->PadBlockByteCount = 8;
                TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_IV2;
                TokenContext_Internal_p->IVOffset = SAParams_p->OffsetIV;
            break;
            case SAB_CRYPTO_AES:
                TokenContext_Internal_p->IVByteCount = 16;
                TokenContext_Internal_p->PadBlockByteCount = 16;
                TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_IV4;
                TokenContext_Internal_p->IVOffset = SAParams_p->OffsetIV;
                break;
#ifdef TKB_ENABLE_CRYPTO_WIRELESS
            case SAB_CRYPTO_KASUMI:
                TokenContext_Internal_p->IVByteCount = 8;
                TokenContext_Internal_p->PadBlockByteCount = 8;
                TokenContext_Internal_p->IVHandling = TKB_IV_KASUMI_F8;
                TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;
                TokenContext_Internal_p->IVOffset = SAParams_p->OffsetIV;
                break;
            case SAB_CRYPTO_SNOW:
                TokenContext_Internal_p->IVByteCount = 16;
                TokenContext_Internal_p->PadBlockByteCount = 1;
                TokenContext_Internal_p->IVHandling = TKB_IV_SNOW_UEA2;
                TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;
                TokenContext_Internal_p->IVOffset = SAParams_p->OffsetIV;
                break;
            case SAB_CRYPTO_ZUC:
                TokenContext_Internal_p->IVByteCount = 16;
                TokenContext_Internal_p->PadBlockByteCount = 1;
                TokenContext_Internal_p->IVHandling = TKB_IV_ZUC_EEA3;
                TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;
                TokenContext_Internal_p->IVOffset = SAParams_p->OffsetIV;
                break;
#endif
            default:
                LOG_CRIT("TokenBuilder_BuildContext:"
                         " unsupported crypto algorithm.\n");
                return TKB_INVALID_PARAMETER;
            }

            switch (SAParams_p->CryptoMode)
            {
            case SAB_CRYPTO_MODE_ECB:
                TokenContext_Internal_p->IVByteCount = 0;
                TokenContext_Internal_p->IVHandling = TKB_IV_OUTBOUND_CBC;
                TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;
                break;
            case SAB_CRYPTO_MODE_BASIC:
                TokenContext_Internal_p->IVHandling = TKB_IV_OUTBOUND_CBC;
                if (SAParams_p->CryptoAlgo == SAB_CRYPTO_KASUMI)
                {
                    TokenContext_Internal_p->IVByteCount = 0;
                    TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;
                    break;
                }
                else
                {
                    TokenContext_Internal_p->PadBlockByteCount = 1;
                }
                /* FALLTHROUGH intended */
            case SAB_CRYPTO_MODE_CBC:
            case SAB_CRYPTO_MODE_ICM:
            case SAB_CRYPTO_MODE_CFB:
            case SAB_CRYPTO_MODE_OFB:
            case SAB_CRYPTO_MODE_XTS:
            case SAB_CRYPTO_MODE_XTS_STATEFUL:
                if (SAParams_p->CryptoMode == SAB_CRYPTO_MODE_ICM)
                {
                    TokenContext_Internal_p->PadBlockByteCount = 1;
                }
                else if (SAParams_p->CryptoMode == SAB_CRYPTO_MODE_XTS ||
                         SAParams_p->CryptoMode == SAB_CRYPTO_MODE_XTS_STATEFUL)
                {
                    TokenContext_Internal_p->PadBlockByteCount = 1;
                    TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_XTS_CRYPTO;
                }

                if (SAParams_p->IVSrc == SAB_IV_SRC_TOKEN)
                {
                    TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;
                    if (TokenContext_Internal_p->IVByteCount == 8)
                    {
                        TokenContext_Internal_p->IVByteCount = 0;
                        if ((SAParams_p->flags & SAB_FLAG_COPY_IV) != 0)
                            TokenContext_Internal_p->IVHandling =
                                TKB_IV_COPY_TOKEN_2WORDS;
                        else
                            TokenContext_Internal_p->IVHandling =
                                TKB_IV_OUTBOUND_TOKEN_2WORDS;
                        TokenContext_Internal_p->TokenHeaderWord |=
                            TKB_HEADER_IV_TOKEN_2WORDS;
                    }
                    else
                    {
                        TokenContext_Internal_p->IVByteCount = 0;
                        if ((SAParams_p->flags & SAB_FLAG_COPY_IV) != 0)
                            TokenContext_Internal_p->IVHandling =
                                TKB_IV_COPY_TOKEN_4WORDS;
                        else
                            TokenContext_Internal_p->IVHandling =
                                TKB_IV_OUTBOUND_TOKEN_4WORDS;
                        TokenContext_Internal_p->TokenHeaderWord |=
                            TKB_HEADER_IV_TOKEN_4WORDS;
                    }
                }
                else if (SAParams_p->IVSrc == SAB_IV_SRC_INPUT)
                {
                    TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;
                    if ((SAParams_p->flags & SAB_FLAG_COPY_IV) != 0)
                        TokenContext_Internal_p->IVHandling = TKB_IV_COPY_CBC;
                    else
                        TokenContext_Internal_p->IVHandling = TKB_IV_INBOUND_CBC;
                }
                else
                {
                    if(SAParams_p->IVSrc ==  SAB_IV_SRC_PRNG)
                    {
                        TokenContext_Internal_p->TokenHeaderWord |=
                            TKB_HEADER_IV_PRNG;
                        TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;
                    }
                    if ((SAParams_p->flags & SAB_FLAG_COPY_IV) != 0)
                    {
                        if (TokenContext_Internal_p->IVByteCount==8)
                            TokenContext_Internal_p->IVHandling =
                                TKB_IV_OUTBOUND_2WORDS;
                        else
                            TokenContext_Internal_p->IVHandling =
                                TKB_IV_OUTBOUND_4WORDS;
                    }
                    TokenContext_Internal_p->IVByteCount = 0;
                }

                break;
            case SAB_CRYPTO_MODE_CTR:
            case SAB_CRYPTO_MODE_CCM:
            case SAB_CRYPTO_MODE_GCM:
            case SAB_CRYPTO_MODE_GMAC:
                TokenContext_Internal_p->PadBlockByteCount = 1;
                TokenContext_Internal_p->u.generic.UpdateHandling = TKB_UPD_NULL;

                if (SAParams_p->IVSrc == SAB_IV_SRC_TOKEN)
                {
                    TokenContext_Internal_p->IVByteCount = 0;
                        if ((SAParams_p->flags & SAB_FLAG_COPY_IV) != 0)
                            TokenContext_Internal_p->IVHandling =
                                TKB_IV_COPY_TOKEN_4WORDS;
                        else
                            TokenContext_Internal_p->IVHandling =
                                TKB_IV_OUTBOUND_TOKEN_4WORDS;
                    TokenContext_Internal_p->TokenHeaderWord |=
                        TKB_HEADER_IV_TOKEN_4WORDS;
                }
                else if (SAParams_p->IVSrc == SAB_IV_SRC_INPUT)
                {
                    TokenContext_Internal_p->IVByteCount = 8;
                    if ((SAParams_p->flags & SAB_FLAG_COPY_IV) != 0)
                        TokenContext_Internal_p->IVHandling = TKB_IV_COPY_CTR;
                    else
                        TokenContext_Internal_p->IVHandling = TKB_IV_INBOUND_CTR;
                }
                else
                {
                    TokenContext_Internal_p->IVByteCount = 0;
                    if ((SAParams_p->flags & SAB_FLAG_COPY_IV) != 0)
                        TokenContext_Internal_p->IVHandling = TKB_IV_OUTBOUND_CTR;
                }
                break;
#ifdef TKB_ENABLE_CRYPTO_WIRELESS
            case SAB_CRYPTO_MODE_F8:
            case SAB_CRYPTO_MODE_UEA2:
            case SAB_CRYPTO_MODE_EEA3:
                TokenContext_Internal_p->PadBlockByteCount = 1;
                TokenContext_Internal_p->u.generic.CCMSalt =
                    (SAParamsBasic_p->bearer << 3) |
                    (SAParamsBasic_p->direction<<2);
                if (TokenContext_Internal_p->IVByteCount == 8)
                    TokenContext_Internal_p->TokenHeaderWord |=
                        TKB_HEADER_IV_TOKEN_2WORDS;
                else
                    TokenContext_Internal_p->TokenHeaderWord |=
                        TKB_HEADER_IV_TOKEN_4WORDS;
                TokenContext_Internal_p->IVByteCount = 0;
                break;
#endif
            default:
                ;
            }
        }
        else
        {
            TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_HASH;
        }
        if (SAParams_p->AuthAlgo != SAB_AUTH_NULL)
        {   /* Basic hash */
            if ((SAParams_p->flags & (SAB_FLAG_HASH_SAVE|
                                      SAB_FLAG_HASH_INTERMEDIATE)) != 0)
            {
                /* We intend to store the hash state.*/
                TokenContext_Internal_p->SeqOffset = SAParams_p->OffsetDigest0;
            }
            if(SAParams_p->CryptoAlgo == SAB_CRYPTO_NULL)
            {
                /* Use ExtSeq variable to specify that payload will be copied */
                if ((SAParams_p->flags & SAB_FLAG_SUPPRESS_PAYLOAD) == 0)
                    TokenContext_Internal_p->ExtSeq = 1;
            }
            else
            {
                /* Use ExtSeq variable to specify that header
                 * (authenticated data( will be copied */
                if ((SAParams_p->flags & SAB_FLAG_SUPPRESS_HEADER) == 0)
                    TokenContext_Internal_p->ExtSeq = 1;
            }
            switch(SAParams_p->AuthAlgo)
            {
            case SAB_AUTH_HASH_MD5:
                TokenContext_Internal_p->ICVByteCount = 16;
                TokenContext_Internal_p->DigestWordCount = 16 + 4;
                break;
            case SAB_AUTH_HMAC_MD5:
                TokenContext_Internal_p->ICVByteCount = 16;
                TokenContext_Internal_p->DigestWordCount = 4;
                break;
            case SAB_AUTH_HASH_SHA1:
                TokenContext_Internal_p->ICVByteCount = 20;
                TokenContext_Internal_p->DigestWordCount = 16 + 5;
                break;
            case SAB_AUTH_HMAC_SHA1:
                TokenContext_Internal_p->ICVByteCount = 20;
                TokenContext_Internal_p->DigestWordCount = 5;
                break;
            case SAB_AUTH_HASH_SHA2_224:
                TokenContext_Internal_p->ICVByteCount = 32;
                TokenContext_Internal_p->DigestWordCount = 16 + 8;
                break;
            case SAB_AUTH_HMAC_SHA2_224:
                TokenContext_Internal_p->ICVByteCount = 32;
                TokenContext_Internal_p->DigestWordCount = 8;
                break;
            case SAB_AUTH_HASH_SHA2_256:
                TokenContext_Internal_p->ICVByteCount = 32;
                TokenContext_Internal_p->DigestWordCount = 16 + 8;
                break;
            case SAB_AUTH_HMAC_SHA2_256:
                TokenContext_Internal_p->ICVByteCount = 32;
                TokenContext_Internal_p->DigestWordCount = 8;
                break;
            case SAB_AUTH_HASH_SHA2_384:
                TokenContext_Internal_p->ICVByteCount = 64;
                TokenContext_Internal_p->DigestWordCount = 16 + 16;
                break;
            case SAB_AUTH_HMAC_SHA2_384:
                TokenContext_Internal_p->ICVByteCount = 64;
                TokenContext_Internal_p->DigestWordCount = 16;
                break;
            case SAB_AUTH_HASH_SHA2_512:
                TokenContext_Internal_p->ICVByteCount = 64;
                TokenContext_Internal_p->DigestWordCount = 16 + 16;
                break;
            case SAB_AUTH_HMAC_SHA2_512:
                TokenContext_Internal_p->ICVByteCount = 64;
                TokenContext_Internal_p->DigestWordCount = 16;
                break;
            case SAB_AUTH_AES_XCBC_MAC:
            case SAB_AUTH_AES_CMAC_128:
            case SAB_AUTH_AES_CMAC_192:
            case SAB_AUTH_AES_CMAC_256:
                TokenContext_Internal_p->ICVByteCount = 16;
                TokenContext_Internal_p->DigestWordCount = 4;
                break;
            case SAB_AUTH_AES_CCM:
                if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
                    TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_CCM_OUT;
                else
                    TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_CCM_IN;
                TokenContext_Internal_p->ICVByteCount = 16;
                TokenContext_Internal_p->DigestWordCount = 4;
                // Adjust byte count now as it influences the SALT value.
                if (SAParamsBasic_p->ICVByteCount != 0 &&
                    SAParamsBasic_p->ICVByteCount <
                    TokenContext_Internal_p->ICVByteCount)
                    TokenContext_Internal_p->ICVByteCount =
                        SAParamsBasic_p->ICVByteCount;
                TokenContext_Internal_p->u.generic.CCMSalt =
                    (SAParams_p->Nonce_p[0] << 8) |
                    (SAParams_p->Nonce_p[1] << 16) |
                    (SAParams_p->Nonce_p[2] << 24) |
                    TKB_CCM_FLAG_ADATA_L4 |
                    ((TokenContext_Internal_p->ICVByteCount-2)*4);
                break;
            case SAB_AUTH_AES_GCM:
                if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
                    TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_GCM_OUT;
                else
                    TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_GCM_IN;
                TokenContext_Internal_p->ICVByteCount = 16;
                TokenContext_Internal_p->DigestWordCount = 4;
                break;
            case SAB_AUTH_AES_GMAC:
                if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
                    TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_GMAC_OUT;
                else
                    TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_GMAC_IN;
                TokenContext_Internal_p->ICVByteCount = 16;
                TokenContext_Internal_p->DigestWordCount = 4;
                break;
#ifdef TKB_ENABLE_CRYPTO_WIRELESS
            case SAB_AUTH_KASUMI_F9:
                TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_KASUMI_HASH;
                TokenContext_Internal_p->ICVByteCount = 4;
                TokenContext_Internal_p->DigestWordCount = 4;
                TokenContext_Internal_p->u.generic.CCMSalt =
                    SAParamsBasic_p->fresh;
                break;
            case SAB_AUTH_SNOW_UIA2:
                TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_SNOW_HASH;
                TokenContext_Internal_p->TokenHeaderWord |= TKB_HEADER_U;
                TokenContext_Internal_p->ICVByteCount = 4;
                TokenContext_Internal_p->DigestWordCount = 4;
                TokenContext_Internal_p->u.generic.CCMSalt =
                    SAParamsBasic_p->fresh;
                break;
            case SAB_AUTH_ZUC_EIA3:
                TokenContext_Internal_p->protocol = TKB_PROTO_BASIC_ZUC_HASH;
                TokenContext_Internal_p->TokenHeaderWord |= TKB_HEADER_U;
                TokenContext_Internal_p->ICVByteCount = 4;
                TokenContext_Internal_p->DigestWordCount = 4;
                TokenContext_Internal_p->u.generic.CCMSalt =
                    (SAParamsBasic_p->bearer << 3);
                break;
#endif
            default:
                LOG_CRIT("TokenBuilder_BuildContext:"
                         " unsupported authentication algorithm.\n");
                return TKB_INVALID_PARAMETER;
            }

		/* Workaround for the ICV issue */
		SAParamsBasic_p->BasicFlags = 0;
		/* ---------------------------- */

	    if (SAParamsBasic_p->ICVByteCount != 0 &&
                SAParamsBasic_p->ICVByteCount <
                TokenContext_Internal_p->ICVByteCount)
                TokenContext_Internal_p->ICVByteCount =
                    SAParamsBasic_p->ICVByteCount;
            if ((SAParamsBasic_p->BasicFlags & SAB_BASIC_FLAG_EXTRACT_ICV) != 0 ||
                ((SAParams_p->AuthAlgo == SAB_AUTH_AES_CCM ||
                 SAParams_p->AuthAlgo == SAB_AUTH_AES_GCM ||
                 SAParams_p->AuthAlgo == SAB_AUTH_AES_GMAC) &&
                 TokenContext_Internal_p->SeqOffset == 0))
                TokenContext_Internal_p->AntiReplay =
                    TokenContext_Internal_p->ICVByteCount;
        }
    }
    break;
#endif
#ifdef TKB_ENABLE_PROTO_SRTP
    case SAB_PROTO_SRTP:
    {
        // Note: SRTP may not make use of the Token Context fields
        // CW0, CW1 CCMAalt and UpdateHandling as they occupy the same
        // space as the SaltKey fields.
        SABuilder_Params_SRTP_t *SAParamsSRTP_p;
        SAParamsSRTP_p = (SABuilder_Params_SRTP_t *)
            (SAParams_p->ProtocolExtension_p);
        if (SAParamsSRTP_p == NULL)
        {
            LOG_CRIT("TokenBuilder_BuildContext:"
                     " SRTP extension pointer is NULL\n");
            return TKB_INVALID_PARAMETER;
        }

        if (SAParams_p->direction == SAB_DIRECTION_OUTBOUND)
            TokenContext_Internal_p->protocol = TKB_PROTO_SRTP_OUT;
        else
            TokenContext_Internal_p->protocol = TKB_PROTO_SRTP_IN;

        TokenContext_Internal_p->ICVByteCount = SAParamsSRTP_p->ICVByteCount;

        if (SAParams_p->CryptoAlgo != SAB_CRYPTO_NULL)
        {
            TokenContext_Internal_p->IVHandling = TKB_IV_OUTBOUND_TOKEN_SRTP;

            TokenContext_Internal_p->IVByteCount = 16;
            TokenContext_Internal_p->TokenHeaderWord |=
                TKB_HEADER_IV_TOKEN_4WORDS;
            if (SAParams_p->Nonce_p == NULL)
            {
                LOG_CRIT("TokenBuilder_BuildContext: NULL nonce pointer.\n");
                return TKB_INVALID_PARAMETER;
            }

            TokenContext_Internal_p->u.srtp.SaltKey0 =
                (SAParams_p->Nonce_p[0]) |
                (SAParams_p->Nonce_p[1] << 8) |
                (SAParams_p->Nonce_p[2] << 16) |
                (SAParams_p->Nonce_p[3] << 24);
            TokenContext_Internal_p->u.srtp.SaltKey1 =
                (SAParams_p->Nonce_p[4]) |
                (SAParams_p->Nonce_p[5] << 8) |
                (SAParams_p->Nonce_p[6] << 16) |
                (SAParams_p->Nonce_p[7] << 24);
            TokenContext_Internal_p->u.srtp.SaltKey2 =
                (SAParams_p->Nonce_p[8]) |
                (SAParams_p->Nonce_p[9] << 8) |
                (SAParams_p->Nonce_p[10] << 16) |
                (SAParams_p->Nonce_p[11] << 24);
            TokenContext_Internal_p->u.srtp.SaltKey3 =
                (SAParams_p->Nonce_p[12]) |
                (SAParams_p->Nonce_p[13] << 8);
        }
        else
        {
            TokenContext_Internal_p->IVHandling = TKB_IV_OUTBOUND_CBC;
            TokenContext_Internal_p->IVByteCount = 0;
        }

        if ((SAParamsSRTP_p->SRTPFlags & SAB_SRTP_FLAG_SRTCP) != 0)
        {
            TokenContext_Internal_p->ExtSeq = 4;
        }
        else
        {
            TokenContext_Internal_p->ExtSeq = 0;
        }

        if ((SAParamsSRTP_p->SRTPFlags & SAB_SRTP_FLAG_INCLUDE_MKI) != 0)
        {
            TokenContext_Internal_p->AntiReplay = 4;
        }
        else
        {
            TokenContext_Internal_p->AntiReplay = 0;
        }
    }
    break;
#endif
    default:
        LOG_CRIT("TokenBuilder_BuildContext: unsupported protocol.\n");
        return TKB_INVALID_PARAMETER;
    }


    LOG_INFO("TokenBuilderBuildContext: created context, header word=%08x\n"
             "  proto=%d padalign=%d ivsize=%d icvsize=%d\n"
             "  seqoffset=%d extseq=%d antireplay=%d ivhandling=%d\n",
             TokenContext_Internal_p->TokenHeaderWord,
             TokenContext_Internal_p->protocol,
             TokenContext_Internal_p->PadBlockByteCount,
             TokenContext_Internal_p->IVByteCount,
             TokenContext_Internal_p->ICVByteCount,
             TokenContext_Internal_p->SeqOffset,
             TokenContext_Internal_p->ExtSeq,
             TokenContext_Internal_p->AntiReplay,
             TokenContext_Internal_p->IVHandling);

    return TKB_STATUS_OK;
}




/* end of file token_builder.c */


