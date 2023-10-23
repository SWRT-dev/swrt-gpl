/* token_builder_interna.h
 *
 * Internal APIs for the Token Builder implementation.
 * This includes thee actual definition of the Token Context Record.
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


#ifndef TOKEN_BUILDER_INTERNAL_H_
#define TOKEN_BUILDER_INTERNAL_H_


/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */
#include "token_builder.h"
#include "basic_defs.h"

/*----------------------------------------------------------------------------
 * Definitions and macros
 */

/* Various fields in the token header word.
 */
#define TKB_HEADER_RC_NO_REUSE       0x00000000
#define TKB_HEADER_RC_REUSE          0x00100000
#define TKB_HEADER_RC_AUTO_REUSE     0x00200000

#define TKB_HEADER_IP                0x00020000

#ifdef TKB_NO_INPUT_PACKET_POINTER
#define TKB_HEADER_IP_OPTION TKB_HEADER_IP
#else
#define TKB_HEADER_IP_OPTION 0
#endif

#ifdef TKB_AUTODETECT_CONTEXT_REUSE
#define TKB_HEADER_RC_OPTION TKB_HEADER_RC_AUTO_REUSE
#else
#define TKB_HEADER_RC_OPTION TKB_HEADER_RC_NO_REUSE
#endif

#define TKB_HEADER_DEFAULT           (TKB_HEADER_RC_OPTION | TKB_HEADER_IP_OPTION)

#define TKB_HEADER_C                 0x02000000

#define TKB_HEADER_PAD_VERIFY        0x01000000
#define TKB_HEADER_IV_DEFAULT        0x00000000
#define TKB_HEADER_IV_PRNG           0x04000000
#define TKB_HEADER_IV_TOKEN_2WORDS   0x18000000
#define TKB_HEADER_IV_TOKEN_4WORDS   0x1c000000

#define TKB_HEADER_U                 0x20000000

/* CCM flag byte, includes Adata and L=4, M has to be filled in */
#define TKB_CCM_FLAG_ADATA_L4        0x43

/* The protocol values have to agree with those used in token_builder_core.c
 */
typedef enum
{
    TKB_PROTO_ESP_OUT = 0,
    TKB_PROTO_ESP_IN = 1,
    TKB_PROTO_ESP_CCM_OUT = 2,
    TKB_PROTO_ESP_CCM_IN = 3,
    TKB_PROTO_ESP_GCM_OUT = 4,
    TKB_PROTO_ESP_GCM_IN = 5,
    TKB_PROTO_ESP_GMAC_OUT = 6,
    TKB_PROTO_ESP_GMAC_IN = 7,
    TKB_PROTO_SSLTLS_OUT = 8,
    TKB_PROTO_SSLTLS_IN = 9,
    TKB_PROTO_BASIC_CRYPTO = 10,
    TKB_PROTO_BASIC_HASH = 11,
    TKB_PROTO_SRTP_OUT = 12,
    TKB_PROTO_SRTP_IN = 13,
    TKB_PROTO_BASIC_CRYPTHASH = 14,
    TKB_PROTO_BASIC_CCM_OUT = 15,
    TKB_PROTO_BASIC_CCM_IN = 16,
    TKB_PROTO_BASIC_GCM_OUT = 17,
    TKB_PROTO_BASIC_GCM_IN = 18,
    TKB_PROTO_BASIC_GMAC_OUT = 19,
    TKB_PROTO_BASIC_GMAC_IN = 20,
    TKB_PROTO_SSLTLS_GCM_OUT = 21,
    TKB_PROTO_SSLTLS_GCM_IN = 22,
    TKB_PROTO_BASIC_XTS_CRYPTO = 23,
    TKB_PROTO_BASIC_KASUMI_HASH = 24,
    TKB_PROTO_BASIC_SNOW_HASH = 25,
    TKB_PROTO_BASIC_ZUC_HASH = 26,
} TokenBuilder_Protocol_t;

/* The IV handling values have to agree with those used in token_builder_core.c
 */
typedef enum
{
    TKB_IV_INBOUND_CTR = 0,
    TKB_IV_INBOUND_CBC = 1,
    TKB_IV_OUTBOUND_CTR = 2,
    TKB_IV_OUTBOUND_CBC = 3,
    TKB_IV_COPY_CBC = 4,
    TKB_IV_COPY_CTR = 5,
    TKB_IV_OUTBOUND_2WORDS = 6,
    TKB_IV_OUTBOUND_4WORDS = 7,
    TKB_IV_OUTBOUND_TOKEN_2WORDS = 8,
    TKB_IV_COPY_TOKEN_2WORDS = 9,
    TKB_IV_OUTBOUND_TOKEN_SRTP = 10,
    TKB_IV_KASUMI_F8 = 11,
    TKB_IV_SNOW_UEA2 = 12,
    TKB_IV_ZUC_EEA3 = 13,
    TKB_IV_OUTBOUND_TOKEN_4WORDS = 14,
    TKB_IV_COPY_TOKEN_4WORDS = 15,
} TokenBuilder_IVHandling_t;


/* Context update handling for SSL/TLS. The values have to agree with
   those used in token_builder_core.c
*/
typedef enum
{
    TKB_UPD_NULL = 0,
    TKB_UPD_ARC4 = 1,
    TKB_UPD_IV2 = 2,
    TKB_UPD_IV4 = 3,
    TKB_UPD_BLK = 4,
} TokenBuilder_UpdateHandling_t;


/* The TokenBuilder_Context_t (Token Context Record) contains all
   information that the Token Builder requires for each SA.
*/
typedef struct
{
    uint32_t TokenHeaderWord;
    TokenBuilder_Protocol_t protocol;
    uint8_t IVByteCount;
    uint8_t ICVByteCount;
    uint8_t PadBlockByteCount;
    uint8_t ExtSeq;
    uint8_t AntiReplay;
    uint8_t SeqOffset;
    uint8_t IVOffset;
    uint8_t DigestWordCount;
    TokenBuilder_IVHandling_t IVHandling;
    union {
        struct {
            TokenBuilder_UpdateHandling_t UpdateHandling;
            uint32_t CCMSalt;
            uint32_t CW0,CW1; /* Context control words */
        } generic;
        struct { // Make the SRTP salt key overlap with fields not used in SRTP.
            uint32_t SaltKey0,SaltKey1,SaltKey2,SaltKey3;
        } srtp;
    } u;
} TokenBuilder_Context_t;


#endif /* TOKEN_BUILDER_INTERNAL_H_ */

/* end of file token_builder_internal.h */
