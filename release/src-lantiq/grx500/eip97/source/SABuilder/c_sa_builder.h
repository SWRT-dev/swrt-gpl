/* c_sa_builder.h
 *
 * Default configuration file for the SA Builder
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
 * Import the product specific configuration.
 */
#include "cs_sa_builder.h"

/* Specify a specific version of the EIP-96, specify exactly one */
//#define SAB_EIP96_VERSION_I
//#define SAB_EIP96_VERSION_IE
//#define SAB_EIP96_VERSION_IS
//#define SAB_EIP96_VERSION_IES
//#define SAB_EIP96_VERSION_IW
//#define SAB_EIP96_VERSION_IEW
//#define SAB_EIP96_VERSION_IESW
//#define SAB_EIP96_VERSION_IEX
//#define SAB_EIP96_VERSION_IESWX


/* Which protocol families are enabled? Enable the desired protocols, for the
   I and IE version, the SSLTLS protocol is automatically disabled.*/
//#define SAB_ENABLE_PROTO_BASIC
//#define SAB_ENABLE_PROTO_IPSEC
//#define SAB_ENABLE_PROTO_SSLTLS
//#define SAB_ENABLE_PROTO_MACSEC
//#define SAB_ENABLE_PROTO_SRTP

/* Which protocol-specific options are enabled? */
//#define SAB_ENABLE_IPSEC_ESP
//#define SAB_ENABLE_IPSEC_AH

/* Enable if the SA Builder must support extended use case for IPsec
   processing */
//#define SAB_ENABLE_IPSEC_EXTENDED

/* Enable if the SA Builder must support extended use case for DTLS
   processing */
//#define SAB_ENABLE_DTLS_EXTENDED

/* Enable if the SA Builder must support extended use case for MACsec
   processing */
//#define SAB_ENABLE_MACSEC_EXTENDED

/* Enable if the SA Builder must support an engine with fixed SA records
   (e.g. for a record cache) */
//#define SAB_ENABLE_FIXED_RECORD_SIZE

#ifdef SAB_ENABLE_IPSEC_EXTENDED
// In the extended use case, always use a fixed record size, but do
// not provide a value of that size here.
#define SAB_ENABLE_FIXED_RECORD_SIZE

// Set this if tunnel header fields are to be copied into the transform.
// for extended use case.
//#define SAB_ENABLE_EXTENDED_TUNNEL_HEADER

#else
// Look-aside use case.
#ifdef SAB_ENABLE_FIXED_RECORD_SIZE
#ifndef SAB_RECORD_WORD_COUNT
// Specify the number of words of an SA record when the record size is fixed.
#define SAB_RECORD_WORD_COUNT 64
#endif
#endif
#endif

// Set this if there are two fixed record sizes:
// SAB_RECORD_WORD_COUNT and SAB_RECORD_WORD_COUNT_LARGE,
// select the large one if the context size exceeds
// SAB_LARGE_RECORD_TRHESHOLD_WORD_COUNT.
//#define SAB_ENABLE_TWO_FIXED_RECORD_SIZES


/* Enable specific crypto and authentication algorithms. The correct ones
   are already set by the VERSION define.*/
//#define SAB_ENABLE_CRYPTO_AES
//#define SAB_ENABLE_CRYPTO_GCM
//#define SAB_ENABLE_CRYPTO_3DES
//#define SAB_ENABLE_CRYPTO_ARCFOUR
//#define SAB_ENABLE_CRYPTO_XTS
//#define SAB_ENABLE_CRYPTO_KASUMI
//#define SAB_ENABLE_CRYPTO_SNOW
//#define SAB_ENABLE_CRYPTO_ZUC
//#define SAB_ENABLE_AUTH_MD5
//#define SAB_ENABLE_AUTH_SHA1
//#define SAB_ENABLE_AUTH_SHA2_256
//#define SAB_ENABLE_AUTH_SHA2_512

#ifdef SAB_EIP96_VERSION_I
#undef SAB_ENABLE_PROTO_SSLTLS
#define SAB_ENABLE_CRYPTO_AES
#define SAB_ENABLE_CRYPTO_GCM
#define SAB_ENABLE_CRYPTO_3DES
#undef SAB_ENABLE_CRYPTO_ARCFOUR
#define SAB_ENABLE_AUTH_MD5
#define SAB_ENABLE_AUTH_SHA1
#define SAB_ENABLE_AUTH_SHA2_256
#undef SAB_ENABLE_AUTH_SHA2_512
#undef SAB_ENABLE_CRYPTO_XTS
#undef SAB_ENABLE_CRYPTO_KASUMI
#undef SAB_ENABLE_CRYPTO_SNOW
#undef SAB_ENABLE_CRYPTO_ZUC
#endif

#ifdef SAB_EIP96_VERSION_IE
#undef SAB_ENABLE_PROTO_SSLTLS
#define SAB_ENABLE_CRYPTO_AES
#define SAB_ENABLE_CRYPTO_GCM
#define SAB_ENABLE_CRYPTO_3DES
#undef SAB_ENABLE_CRYPTO_ARCFOUR
#define SAB_ENABLE_AUTH_MD5
#define SAB_ENABLE_AUTH_SHA1
#define SAB_ENABLE_AUTH_SHA2_256
#define SAB_ENABLE_AUTH_SHA2_512
#undef SAB_ENABLE_CRYPTO_XTS
#undef SAB_ENABLE_CRYPTO_KASUMI
#undef SAB_ENABLE_CRYPTO_SNOW
#undef SAB_ENABLE_CRYPTO_ZUC
#endif

#ifdef SAB_EIP96_VERSION_IS
#define SAB_ENABLE_CRYPTO_AES
#define SAB_ENABLE_CRYPTO_GCM
#define SAB_ENABLE_CRYPTO_3DES
#define SAB_ENABLE_CRYPTO_ARCFOUR
#define SAB_ENABLE_AUTH_MD5
#define SAB_ENABLE_AUTH_SHA1
#define SAB_ENABLE_AUTH_SHA2_256
#undef SAB_ENABLE_AUTH_SHA2_512
#undef SAB_ENABLE_CRYPTO_XTS
#undef SAB_ENABLE_CRYPTO_KASUMI
#undef SAB_ENABLE_CRYPTO_SNOW
#undef SAB_ENABLE_CRYPTO_ZUC
#endif

#ifdef SAB_EIP96_VERSION_IES
#define SAB_ENABLE_CRYPTO_AES
#define SAB_ENABLE_CRYPTO_GCM
#define SAB_ENABLE_CRYPTO_3DES
#define SAB_ENABLE_CRYPTO_ARCFOUR
#define SAB_ENABLE_AUTH_MD5
#define SAB_ENABLE_AUTH_SHA1
#define SAB_ENABLE_AUTH_SHA2_256
#define SAB_ENABLE_AUTH_SHA2_512
#undef SAB_ENABLE_CRYPTO_XTS
#undef SAB_ENABLE_CRYPTO_KASUMI
#undef SAB_ENABLE_CRYPTO_SNOW
#undef SAB_ENABLE_CRYPTO_ZUC
#endif


#ifdef SAB_EIP96_VERSION_IW
#undef SAB_ENABLE_PROTO_SSLTLS
#define SAB_ENABLE_CRYPTO_AES
#define SAB_ENABLE_CRYPTO_GCM
#define SAB_ENABLE_CRYPTO_3DES
#undef SAB_ENABLE_CRYPTO_ARCFOUR
#define SAB_ENABLE_AUTH_MD5
#define SAB_ENABLE_AUTH_SHA1
#define SAB_ENABLE_AUTH_SHA2_256
#undef SAB_ENABLE_AUTH_SHA2_512
#undef SAB_ENABLE_CRYPTO_XTS
#define SAB_ENABLE_CRYPTO_KASUMI
#define SAB_ENABLE_CRYPTO_SNOW
#define SAB_ENABLE_CRYPTO_ZUC
#endif

#ifdef SAB_EIP96_VERSION_IEW
#undef SAB_ENABLE_PROTO_SSLTLS
#define SAB_ENABLE_CRYPTO_AES
#define SAB_ENABLE_CRYPTO_GCM
#define SAB_ENABLE_CRYPTO_3DES
#undef SAB_ENABLE_CRYPTO_ARCFOUR
#define SAB_ENABLE_AUTH_MD5
#define SAB_ENABLE_AUTH_SHA1
#define SAB_ENABLE_AUTH_SHA2_256
#define SAB_ENABLE_AUTH_SHA2_512
#undef SAB_ENABLE_CRYPTO_XTS
#define SAB_ENABLE_CRYPTO_KASUMI
#define SAB_ENABLE_CRYPTO_SNOW
#define SAB_ENABLE_CRYPTO_ZUC
#endif

#ifdef SAB_EIP96_VERSION_IESW
#define SAB_ENABLE_CRYPTO_AES
#define SAB_ENABLE_CRYPTO_GCM
#define SAB_ENABLE_CRYPTO_3DES
#define SAB_ENABLE_CRYPTO_ARCFOUR
#define SAB_ENABLE_AUTH_MD5
#define SAB_ENABLE_AUTH_SHA1
#define SAB_ENABLE_AUTH_SHA2_256
#define SAB_ENABLE_AUTH_SHA2_512
#undef SAB_ENABLE_CRYPTO_XTS
#define SAB_ENABLE_CRYPTO_KASUMI
#define SAB_ENABLE_CRYPTO_SNOW
#define SAB_ENABLE_CRYPTO_ZUC
#endif

#ifdef SAB_EIP96_VERSION_IEX
#undef SAB_ENABLE_PROTO_SSLTLS
#define SAB_ENABLE_CRYPTO_AES
#define SAB_ENABLE_CRYPTO_GCM
#define SAB_ENABLE_CRYPTO_3DES
#undef SAB_ENABLE_CRYPTO_ARCFOUR
#define SAB_ENABLE_AUTH_MD5
#define SAB_ENABLE_AUTH_SHA1
#define SAB_ENABLE_AUTH_SHA2_256
#define SAB_ENABLE_AUTH_SHA2_512
#define SAB_ENABLE_CRYPTO_XTS
#undef SAB_ENABLE_CRYPTO_KASUMI
#undef SAB_ENABLE_CRYPTO_SNOW
#undef SAB_ENABLE_CRYPTO_ZUC
#endif

#ifdef SAB_EIP96_VERSION_IESWX
#define SAB_ENABLE_CRYPTO_AES
#define SAB_ENABLE_CRYPTO_GCM
#define SAB_ENABLE_CRYPTO_3DES
#define SAB_ENABLE_CRYPTO_ARCFOUR
#define SAB_ENABLE_AUTH_MD5
#define SAB_ENABLE_AUTH_SHA1
#define SAB_ENABLE_AUTH_SHA2_256
#define SAB_ENABLE_AUTH_SHA2_512
#define SAB_ENABLE_CRYPTO_XTS
#define SAB_ENABLE_CRYPTO_KASUMI
#define SAB_ENABLE_CRYPTO_SNOW
#define SAB_ENABLE_CRYPTO_ZUC
#endif



/* Enable this if you desire to include the ARC4 state in the SA
   record. This requires the hardware to be configured for relative
   ARC4 state offsets */
//#define SAB_ARC4_STATE_IN_SA


/* When the ARC4 state is included in the SA record, specify the
   alignment requirements for this state.
 */
#ifndef SAB_ARC4_STATE_ALIGN_BYTE_COUNT
#define SAB_ARC4_STATE_ALIGN_BYTE_COUNT 4
#endif


/* Enable single-pass inbound SSL/TLS, only for HW2.2 */
//#define SAB_ENABLE_SINGLE_PASS_SSLTLS

/* Strict checking of function arguments if enabled */
//#define SAB_STRICT_ARGS_CHECK


/* log level for the SA builder.
   choose from LOG_SEVERITY_INFO, LOG_SEVERITY_WARN, LOG_SEVERITY_CRIT */
#ifndef LOG_SEVERITY_MAX
#define LOG_SEVERITY_MAX LOG_SEVERITY_CRIT
#endif


/* end of file c_sa_builder.h */
