
/* token_builder_macros.h
 *
 * Macros to be used inside the Token Builder. Main purpose is to evaluate
 * variables that are used inside generated code.
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


#ifndef TOKEN_BUILDER_MACROS_H_
#define TOKEN_BUILDER_MACROS_H_

#include "log.h"

/*----------------------------------------------------------------------------
 * Definitions and macros
 */


/* Values to be added to Context Control Word 0 for per-packet options */
#define TKB_PERPKT_HASH_FIRST     0x00000010
#define TKB_PERPKT_HASH_NO_FINAL  0x00000020
#define TKB_PERPKT_CTR_INIT       0x00000040
#define TKB_PERPKT_ARC4_INIT      0x00000080
#define TKB_PERPKT_XTS_INIT       0x00000080


#define EVAL_TokenHeaderWord() (EVAL_packetsize() | \
             TokenContext_Internal_p->TokenHeaderWord | \
             (EVAL_per_packet_options() != 0?TKB_HEADER_C:0))

#define EVAL_per_packet_options() PacketOptions(TokenContext_Internal_p,\
                                                TKBParams_p->PacketFlags)
#define EVAL_appendhash() ((unsigned)((TKBParams_p->PacketFlags &   \
                                       TKB_PACKET_FLAG_HASHAPPEND) != 0 || \
                               TokenContext_Internal_p->SeqOffset==0))

/* The following values are each a parameter, either in the Token Context,
   or in the Packet Parameters or a parameter to the token builder function */
#define EVAL_cw0() ((unsigned)TokenContext_Internal_p->u.generic.CW0)
#define EVAL_cw1() ((unsigned)TokenContext_Internal_p->u.generic.CW1)
#define EVAL_proto() ((unsigned)TokenContext_Internal_p->protocol)
#define EVAL_ivlen() ((unsigned)TokenContext_Internal_p->IVByteCount)
#define EVAL_icvlen() ((unsigned)TokenContext_Internal_p->ICVByteCount)
#define EVAL_hstatelen() ((unsigned)TokenContext_Internal_p->DigestWordCount)
#define EVAL_cipher_is_aes() ((unsigned)(TokenContext_Internal_p->PadBlockByteCount==16))
#define EVAL_pad_blocksize() \
    PadBlockSize(TokenContext_Internal_p->PadBlockByteCount, \
                 TKBParams_p->AdditionalValue)
#define EVAL_seq_offset() ((unsigned)TokenContext_Internal_p->SeqOffset)
#define EVAL_iv_offset() ((unsigned)TokenContext_Internal_p->IVOffset)
#define EVAL_packetsize() ((unsigned)PacketByteCount)
#define EVAL_nextheader() ((unsigned)TKBParams_p->PadByte)
#define EVAL_aadlen_pkt() (EVAL_aad()==0?(unsigned)TKBParams_p->AdditionalValue:0)
#define EVAL_aadlen_tkn() (EVAL_aad()==0?0:(unsigned)TKBParams_p->AdditionalValue)
#define EVAL_aadlen_out() (EVAL_extseq()==0?0:(unsigned)TKBParams_p->AdditionalValue)
#define EVAL_swap_j() ByteSwap32(TKBParams_p->AdditionalValue)
#define EVAL_paylen_ccm() (EVAL_packetsize()-EVAL_bypass() - \
                                        EVAL_ivlen()-EVAL_aadlen_pkt()- \
                   (EVAL_proto()==TKB_PROTO_BASIC_CCM_IN?EVAL_icvlen():0))
#define EVAL_basic_swaplen() ByteSwap32(EVAL_paylen_ccm())
#define EVAL_aadlen_swap() ByteSwap16(TKBParams_p->AdditionalValue)
#define EVAL_aadpad() (PaddedSize(TKBParams_p->AdditionalValue + 18 ,16) - \
                       TKBParams_p->AdditionalValue - 18)
#define EVAL_basic_hashpad() (PaddedSize( EVAL_paylen_ccm(), 16) - EVAL_paylen_ccm())

#define EVAL_bypass() ((unsigned)TKBParams_p->BypassByteCount)
#define EVAL_ivhandling() ((unsigned)TokenContext_Internal_p->IVHandling)
#define EVAL_upd_handling() ((unsigned)TokenContext_Internal_p->u.generic.UpdateHandling)
#define EVAL_extseq() ((unsigned)TokenContext_Internal_p->ExtSeq)
#define EVAL_antireplay() ((unsigned)TokenContext_Internal_p->AntiReplay)
#define EVAL_salt() ((unsigned)TokenContext_Internal_p->u.generic.CCMSalt)
#define EVAL_basic_salt() (EVAL_salt() - (TKBParams_p->AdditionalValue==0?0x40:0))
#define EVAL_swaplen() (EVAL_proto()==TKB_PROTO_SSLTLS_OUT||    \
                        EVAL_proto()==TKB_PROTO_SSLTLS_IN||     \
                        EVAL_proto()==TKB_PROTO_SSLTLS_GCM_OUT||\
                        EVAL_proto()==TKB_PROTO_SSLTLS_GCM_IN?  \
                         ByteSwap16(EVAL_paylen()):             \
                         ByteSwap32(EVAL_paylen()))
#define EVAL_swap_fraglen() ByteSwap16(EVAL_packetsize() - EVAL_bypass() + \
       EVAL_ivlen() + EVAL_icvlen() + \
           (unsigned int)(EVAL_upd_handling() >= TKB_UPD_IV2) * \
                                       EVAL_pad_bytes())
#define EVAL_hashpad() (PaddedSize( EVAL_paylen(), 16) - EVAL_paylen())

#define EVAL_paylen() PayloadSize(TokenContext_Internal_p, \
                                  EVAL_packetsize() - EVAL_bypass(), \
                                  TKBParams_p->AdditionalValue)
#define EVAL_pad_bytes_in() (paylen == 0 ? \
PayloadSize(TokenContext_Internal_p, EVAL_packetsize() - EVAL_bypass(), 0)+1 :\
                             (unsigned int)TKBParams_p->AdditionalValue + 1)
#define EVAL_iv() TKBParams_p->IV_p
#define EVAL_aad() TKBParams_p->AAD_p


/* EVAL_pad_remainder() is used to check whether the packet payload
   on inbound packets is a multiple of the pad block size. If not,
   the packet is invalid and shall not be processed.
 */
#define EVAL_pad_remainder() PadRemainder(EVAL_packetsize() - 8 - \
                                           EVAL_ivlen() - EVAL_icvlen() - \
                                           EVAL_bypass(),               \
                                           EVAL_pad_blocksize())

/* EVAL_pad_bytes() is used to compute the number of bytes that must
   be added to outbound packets.

   For IPsec, two bytes (next header and the number of padded bytes)
   must be added in any case. It is the difference between the padded
   payload size and the unpadded payload size.

   For SSL and TLS, one byte (number of padded bytes) must be added in
   any case.  Padding has to be applied to the payload plus MAC.
*/
#define EVAL_pad_bytes() ComputePadBytes(TokenContext_Internal_p,\
                            EVAL_packetsize()-EVAL_bypass(),     \
                            EVAL_pad_blocksize())
#define EVAL_pad_bytes_basic() (PaddedSize(EVAL_packetsize() - EVAL_bypass() -EVAL_antireplay() - EVAL_aadlen_pkt(), \
                  TokenContext_Internal_p->PadBlockByteCount) -  \
                                EVAL_packetsize()+EVAL_bypass()+EVAL_antireplay()+EVAL_aadlen_pkt())


#define EVAL_srtp_iv0() (TokenContext_Internal_p->u.srtp.SaltKey0)
#define EVAL_srtp_iv1() SRTP_IV1(TokenContext_Internal_p,\
                                 Packet_p + EVAL_bypass())
#define EVAL_srtp_iv2() SRTP_IV2(TokenContext_Internal_p, \
                                 Packet_p + EVAL_bypass(), \
                                 PacketByteCount - EVAL_bypass(), \
                                 TKBParams_p->AdditionalValue)
#define EVAL_srtp_iv3() SRTP_IV3(TokenContext_Internal_p, \
                                 Packet_p + EVAL_bypass(), \
                                 PacketByteCount - EVAL_bypass(), \
                                 TKBParams_p->AdditionalValue)
#define EVAL_srtp_offset() SRTP_Offset(TokenContext_Internal_p, \
                                       Packet_p + EVAL_bypass(),  \
                                       PacketByteCount - EVAL_bypass(), \
                                       TKBParams_p->AdditionalValue)
#define EVAL_srtp_swaproc() ByteSwap32(TKBParams_p->AdditionalValue)


#define EVAL_ssltls_lastblock() (Packet_p + EVAL_packetsize() - 16 - 16*EVAL_cipher_is_aes())

#define EVAL_ssltls_lastword() (Packet_p + EVAL_packetsize() - 4)

#define EVAL_count() TKBParams_p->AdditionalValue
#define EVAL_bearer_dir_fresh() TokenContext_Internal_p->u.generic.CCMSalt

#if TKB_HAVE_PROTO_IPSEC==1|TKB_HAVE_PROTO_SSLTLS==1
/*----------------------------------------------------------------------------
 * PadRemainder
 *
 * Compute the remainder of ByteCount when devided by BlockByteCount..
 * BlockByteCount must be a power of 2.
 */
static unsigned int
PadRemainder(unsigned int ByteCount,
             unsigned int BlockByteCount)
{
    unsigned int val;
    val = ByteCount & (BlockByteCount - 1);
    LOG_INFO("TokenBuilder_BuildToken: Input message %u pad align=%u "
             "remainder=%u\n",
             ByteCount, BlockByteCount,val);
    return val;
}
#endif

#if TKB_HAVE_PROTO_IPSEC==1|TKB_HAVE_PROTO_SSLTLS==1||TKB_HAVE_PROTO_BASIC==1
/*----------------------------------------------------------------------------
 * PaddedSize
 *
 * Compute the size of a packet of length ByteCount when padded to a multiple
 * of BlockByteCount.
 *
 * BlockByteCount must be a power of 2.
 */
static unsigned int
PaddedSize(unsigned int ByteCount,
           unsigned int BlockByteCount)
{
    return (ByteCount + BlockByteCount - 1) & ~(BlockByteCount - 1);
}
#endif

#if TKB_HAVE_PROTO_IPSEC==1|TKB_HAVE_PROTO_SSLTLS==1
/*----------------------------------------------------------------------------
 * PadBlockSize
 *
 * Select the pad block size: select the override value if it is appropriate,
 * else select the default value.
 *
 * The override value is appropriate if it is a power of two and if it is
 * greater than the default value and at most 256.
 */
static unsigned int
PadBlockSize(unsigned int DefaultVal,
             unsigned int OverrideVal)
{
    if (OverrideVal > DefaultVal  &&
        OverrideVal <= 256 &&
        (OverrideVal  & (OverrideVal - 1)) == 0) // Check power of two.
    {
        LOG_INFO("TokenBuilder_Buildtoken: Override pad alignment from %u to %u\n",
                 DefaultVal, OverrideVal);
        return OverrideVal;
    }
    else
    {
        return DefaultVal;
    }
}
#endif

/*----------------------------------------------------------------------------
 * ByteSwap32
 *
 * Return a 32-bit value byte-swapped.
 */
static unsigned int
ByteSwap32(unsigned int val)
{
    return ((val << 24) & 0xff000000) |
           ((val << 8)  & 0x00ff0000) |
           ((val >> 8)  & 0x0000ff00) |
           ((val >> 24) & 0x000000ff);
}

#if TKB_HAVE_PROTO_IPSEC==1||TKB_HAVE_PROTO_SSLTLS==1||TKB_HAVE_PROTO_BASIC==1
/*----------------------------------------------------------------------------
 * ByteSwap16
 *
 * Return a 16-bit value byte-swapped.
 */
static unsigned int
ByteSwap16(unsigned int val)
{
    return ((val << 8) & 0xff00) |
           ((val >> 8) & 0x00ff);
}
#endif

/*----------------------------------------------------------------------------
 * PacketOptions
 *
 * Return the per-packet options depending on the packet flags and the protocol.
 * In this implementaiton only the TKB_PACKET_FLAG_ARC4_INIT flag is
 * honored for the ARC4 SSL/TLS case.
 */
static unsigned int
PacketOptions(const TokenBuilder_Context_t * TokenContext_p,
              unsigned int flags)
{
    unsigned int val = 0;
    unsigned int protocol = TokenContext_p->protocol;

    if ((protocol == TKB_PROTO_SSLTLS_OUT || protocol == TKB_PROTO_SSLTLS_IN ||
            protocol == TKB_PROTO_BASIC_CRYPTO) &&
        (flags & TKB_PACKET_FLAG_ARC4_INIT) != 0)
        val |= TKB_PERPKT_ARC4_INIT;

    if (protocol == TKB_PROTO_BASIC_XTS_CRYPTO &&
        (flags & TKB_PACKET_FLAG_XTS_INIT) != 0)
        val |= TKB_PERPKT_XTS_INIT;

    if (protocol == TKB_PROTO_BASIC_HASH &&
        TokenContext_p->SeqOffset != 0 &&
        TokenContext_p->DigestWordCount >= 16)
    {
        if ((flags & TKB_PACKET_FLAG_HASHFIRST) != 0)
            val |= TKB_PERPKT_HASH_FIRST;
        if ((flags & TKB_PACKET_FLAG_HASHFINAL) == 0)
            val |= TKB_PERPKT_HASH_NO_FINAL;
    }

    if (protocol == TKB_PROTO_BASIC_SNOW_HASH ||
        protocol == TKB_PROTO_BASIC_ZUC_HASH)
    {
        val |= TKB_PERPKT_HASH_FIRST;
    }

    if (val != 0)
    {
        LOG_INFO("TokenBuilder_BuildToken: non-zero per-packet options: %x\n",
                 val);
    }

    return val;
}

#if TKB_HAVE_PROTO_IPSEC==1||TKB_HAVE_PROTO_SSLTLS==1
/*----------------------------------------------------------------------------
 * ComputePadBytes
 *
 * PayloadByteCount (input)
 *    Size of message to be padded in bytes.
 * PadBlockByteCount (input)
 *    Block size to which the payload must be padded.
 * TokenContext_p (input)
 *     Token context.
 */
static unsigned int
ComputePadBytes(const TokenBuilder_Context_t *TokenContext_p,
                unsigned int PayloadByteCount,
                unsigned int PadBlockByteCount)
{
    unsigned int PadByteCount;
    if(TokenContext_p->protocol == TKB_PROTO_SSLTLS_OUT)
    {
        PadByteCount = PaddedSize(PayloadByteCount + 1 +
                                  TokenContext_p->IVByteCount +
                                  TokenContext_p->ICVByteCount,
                                  PadBlockByteCount) -
          PayloadByteCount - TokenContext_p->ICVByteCount -
          TokenContext_p->IVByteCount;
        LOG_INFO("TokenBuilder_BuildToken: SSL/TLS padding message %u\n"
                 "  with mac %u pad bytes=%u align=%u\n",
                 PayloadByteCount,
                 TokenContext_p->ICVByteCount,
                 PadByteCount, PadBlockByteCount);
    }
    else
    {
        PadByteCount =  PaddedSize(PayloadByteCount + 2, PadBlockByteCount) -
            PayloadByteCount;
        LOG_INFO("TokenBuilder_BuildToken: IPsec padding message %u "
                 "pad bytes=%u align=%u\n",
                 PayloadByteCount, PadByteCount, PadBlockByteCount);
    }
    return PadByteCount;
}
#endif

#if TKB_HAVE_PROTO_IPSEC==1||TKB_HAVE_PROTO_SSLTLS==1
/*----------------------------------------------------------------------------
 * PayloadSize
 *
 * TokenContext_p (input)
 *     Token context.
 * MessageByteCount (input)
 *     Size of the input packet, excluding bypass.
 * AdditionalValue
 *     Additional value supplied in pacekt parameters. This may be a pad
 *     block size or the number of pad bytes.
 *
 * Compute the payload length in bytes for all SSL/TLS and for ESP with AES-CCM.
 * This is not used for other ESP modes.
 */
static unsigned int
PayloadSize(const TokenBuilder_Context_t *TokenContext_p,
            unsigned int MessageByteCount,
            unsigned int AdditionalValue)
{
    int size;
    switch(TokenContext_p->protocol)
    {
    case TKB_PROTO_ESP_CCM_OUT:
        /* Need paddded payload size, pad message */
        return PaddedSize(MessageByteCount + 2,
           PadBlockSize(TokenContext_p->PadBlockByteCount, AdditionalValue));
    case TKB_PROTO_ESP_CCM_IN:
        /* Need paddded payload size, derive from message length, subtract
           headers and trailers.*/
        return MessageByteCount - TokenContext_p->IVByteCount -
            TokenContext_p->ICVByteCount - 8;
    case TKB_PROTO_SSLTLS_OUT:
    case TKB_PROTO_SSLTLS_GCM_OUT:
        /* Need fragment length */
        return MessageByteCount;
    case TKB_PROTO_SSLTLS_IN:
    case TKB_PROTO_SSLTLS_GCM_IN:
        /* Need fragment length. Must remove any headers, padding and trailers
         */
        size = (int)MessageByteCount;
        if (TokenContext_p->DigestWordCount != 0)
            size -= 4;
        // Deduce CAPWAP/DTLS header.
        size -= TokenContext_p->ICVByteCount;
        size -= TokenContext_p->IVByteCount;
        if (TokenContext_p->ExtSeq != 0)
            size -= 8; /* Deduce epoch and sequence number for DTLS */
        size -= 5;   /* Deduce type, version and fragment length. */
        if (size < 0)
            return 0xffffffffu;
        if(TokenContext_p->u.generic.UpdateHandling != TKB_UPD_NULL &&
           TokenContext_p->u.generic.UpdateHandling != TKB_UPD_ARC4)
        {
            if (PadRemainder((unsigned int)size + TokenContext_p->ICVByteCount,
                             TokenContext_p->PadBlockByteCount) != 0)
                /* Padded payload + ICV must be a multiple of block size */
                return 0xffffffffu;
#if TKB_SINGLE_PASS_SSLTLS==0
            size -= 1 + AdditionalValue; /* Deduce paddding */
            if (size < 0)
                return 0;
#endif
/* Report negative payload size due to subtraction of pad bytes as 0
 * rather than 0xffffffff, so the the operation will still be
 * execcuted. This to avoid timing attacks. See RFC5246, section
 * 6.2.3.2. */
        }
        return (unsigned int)size;
    default:
        LOG_CRIT("Token Builder: PayloadSize used with unsupported protocol\n");
        return 0;
    }
}
#endif

/* SRTP_IV1

   Form second word of IV.  (Exclusive or of salt key and SSRC).
*/
static unsigned int
SRTP_IV1(
        const TokenBuilder_Context_t *TokenContext_p,
        const uint8_t *Packet_p)
{
    unsigned int SSRCOffset;
    if (TokenContext_p->ExtSeq != 0)
    { // SRTCP
        SSRCOffset = 4;
    }
    else
    { // SRTP
        SSRCOffset = 8;
    }

    return TokenContext_p->u.srtp.SaltKey1 ^ ((Packet_p[SSRCOffset]) |
                                              (Packet_p[SSRCOffset+1]<<8) |
                                              (Packet_p[SSRCOffset+2]<<16) |
                                              (Packet_p[SSRCOffset+3]<<24));
}

/* SRTP_IV2

   Form third word of IV (SRTCP: exclusive or with MSB of SRTCP index.
   SRTP: Exclusive or with ROC).
*/
static unsigned int
SRTP_IV2(
        const TokenBuilder_Context_t *TokenContext_p,
        const uint8_t *Packet_p,
        unsigned int MessageByteCount,
        unsigned int AdditionalValue)
{
    unsigned int ByteOffset;
    if (TokenContext_p->ExtSeq != 0)
    { // SRTCP
        uint32_t SRTCP_Index;
        if (TokenContext_p->protocol == TKB_PROTO_SRTP_OUT)
        {   // For outbound packet, SRTCP index is supplied parameter.
            SRTCP_Index = AdditionalValue & 0x7fffffff;
        }
        else
        {   // For inbound, extract it from the packet.
            ByteOffset = MessageByteCount - 4 - TokenContext_p->AntiReplay -
                TokenContext_p->ICVByteCount;
            SRTCP_Index = ((Packet_p[ByteOffset]&0x7f)<<24) |
                (Packet_p[ByteOffset+1]<<16) ;
        }
        return TokenContext_p->u.srtp.SaltKey2 ^ ByteSwap32(SRTCP_Index>>16);
    }
    else
    { // SRTP
        return TokenContext_p->u.srtp.SaltKey2 ^ ByteSwap32(AdditionalValue);
    }
}



/* SRTP_IV3

   Form fourth word of IV (SRTCP: exclusive or with LSB of SRTCP index.
   SRTP: Exclusive or with seq no in packet).
*/
static unsigned int
SRTP_IV3(
        const TokenBuilder_Context_t *TokenContext_p,
        const uint8_t *Packet_p,
        unsigned int MessageByteCount,
        unsigned int AdditionalValue)
{
    unsigned int ByteOffset;
    if (TokenContext_p->ExtSeq != 0)
    { // SRTCP
        uint32_t SRTCP_Index;
        if (TokenContext_p->protocol == TKB_PROTO_SRTP_OUT)
        {   // For outbound packet, SRTCP index is supplied parameter.
            SRTCP_Index = AdditionalValue;
        }
        else
        {   // For inbound, extract it from the packet.
            ByteOffset = MessageByteCount - 4 - TokenContext_p->AntiReplay -
                TokenContext_p->ICVByteCount;
            SRTCP_Index = (Packet_p[ByteOffset+2]<<8) |
                (Packet_p[ByteOffset+3]) ;
        }
        return TokenContext_p->u.srtp.SaltKey3 ^ ByteSwap32(SRTCP_Index<<16);
    }
    else
    {
        return TokenContext_p->u.srtp.SaltKey3 ^ (Packet_p[2] |
                                                  (Packet_p[3]<<8));
    }
}

#if TKB_HAVE_PROTO_SRTP==1
/* SRTP_Offset

   Compute the crypto offset (in bytes) of an SRTP packet.

   If no crypto used: return 0.
   If packet is invalid: return offset larger than message size.
*/
static unsigned int
SRTP_Offset(
        const TokenBuilder_Context_t *TokenContext_p,
        const uint8_t *Packet_p,
        unsigned int MessageByteCount,
        unsigned int AdditionalValue)
{
    unsigned int ByteOffset;
    if (TokenContext_p->IVByteCount == 0)
        return 0;

    if (TokenContext_p->ExtSeq != 0)
    { // SRTCP
        uint32_t SRTCP_Index;
        if (TokenContext_p->protocol == TKB_PROTO_SRTP_OUT)
        {   // For outbound packet, SRTCP index is supplied parameter.
            SRTCP_Index = AdditionalValue;
        }
        else
        {   // For inbound, extract it from the packet.
            ByteOffset = MessageByteCount - 4 - TokenContext_p->AntiReplay -
                TokenContext_p->ICVByteCount;
            if (ByteOffset > MessageByteCount)
            {
                LOG_INFO("TokenBuilder_BuildToken: Short packet\n");
                return MessageByteCount + 1;

            }
            SRTCP_Index = Packet_p[ByteOffset]<<24; // Only get MSB.
        }
        if ((SRTCP_Index & BIT_31) != 0) // Test the E bit.
            return 8; // SRTCP always has crypto offset 8.
        else
            return 0; // Return 0 if no encryption is used.
    }
    else
    { // SRTP
        unsigned int NCSRC = Packet_p[0] & 0xf; // Number of CSRC fields.
        ByteOffset = 12 + 4*NCSRC;
        if ( (Packet_p[0] & BIT_4) != 0) // Extension header present.
        {
            if (ByteOffset + 4 > MessageByteCount)
            {
                LOG_INFO("TokenBuilder_BuildToken: Short packet\n");
                return MessageByteCount + 1;
            }
            ByteOffset += 4 + 4*(Packet_p[ByteOffset+2]<<8) +
                4*Packet_p[ByteOffset+3]; // Add length field from extension.
        }
        return ByteOffset;
    }
}
#endif



/*-----------------------------------------------------------------------------
 * TokenBuilder_CopyBytes
 *
 * Copy a number of bytes from src to dst. src is a byte pointer, which is
 * not necessarily byte aligned. Always write a whole number of words,
 * filling the last word with null-bytes if necessary.
 */
static void
TokenBuilder_CopyBytes(uint32_t *dest,
                       const uint8_t *src,
                       unsigned int ByteCount)
{
    unsigned int i,j;
    uint32_t w;
    if (src == NULL)
    {
        LOG_CRIT("TokenBuilder trying copy data from NULL pointer\n");
        return;
    }
    for (i=0; i<ByteCount/4; i++)
    {
        w=0;
        for (j=0; j<4; j++)
        {
            w = (w>>8) | (*src++ << 24);
        }
        *dest++ = w;
    }
    if (ByteCount % 4 != 0)
    {
        w=0;
        for (j=0; j<ByteCount % 4; j++)
        {
            w = w | (*src++ << (8*j));
        }
        *dest++ = w;
    }
}

#endif /* TOKEN_BUILDER_MACROS_H_ */

/* end of file token_builder_macros.h */
