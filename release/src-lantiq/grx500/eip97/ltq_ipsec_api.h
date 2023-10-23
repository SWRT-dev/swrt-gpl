/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/******************************************************************************
**
** FILE NAME    : ltq_ipsec_api.h
** PROJECT      : GRX500
** MODULES      : crypto hardware
**
** DATE         : 10 May 2016
** AUTHOR       : Mohammad Firdaus B Alias Thani
** DESCRIPTION  : Hardware accelerated Crypto IPsec Drivers
**
*******************************************************************************/

#ifdef CONFIG_LTQ_MPE_IPSEC_SUPPORT 
#include <linux/crypto.h>
#include <linux/completion.h>
#include <linux/kernel.h>

#define CDR_RING_BASE_ADDR_LO_OFFSET(i)		(0x00000000 | i << 12)
#define CDR_RING_BASE_ADDR_HI_OFFSET(i)		(0x00000004 | i << 12)
#define CDR_DATA_BASE_ADDR_LO_OFFSET(i)		(0x00000008 | i << 12)
#define CDR_DATA_BASE_ADDR_HI_OFFSET(i)		(0x0000000C | i << 12)
#define CDR_ACD_BASE_ADDR_LO_OFFSET(i)		(0x00000010 | i << 12)
#define CDR_ACD_BASE_ADDR_HI_OFFSET(i)		(0x00000014 | i << 12)
#define CDR_RING_SIZE_OFFSET(i)				(0x00000018 | i << 12)
#define CDR_DESC_SIZE_OFFSET(i)				(0x0000001C | i << 12)
#define CDR_CFG_OFFSET(i)					(0x00000020 | i << 12)
#define CDR_DMA_CFG_OFFSET(i)				(0x00000024 | i << 12)
#define CDR_THRESH_OFFSET(i)				(0x00000028 | i << 12)
#define CDR_PREP_COUNT_OFFSET(i)			(0x0000002C | i << 12)
#define CDR_PROC_COUNT_OFFSET(i)			(0x00000030 | i << 12)
#define CDR_PREP_PNTR_OFFSET(i)				(0x00000034 | i << 12)
#define CDR_PROC_PNTR_OFFSET(i)				(0x00000038 | i << 12)
#define CDR_STAT_OFFSET(i)					(0x0000003C | i << 12)

#define RDR_RING_BASE_ADDR_LO_OFFSET(i)		(0x00000800 | i << 12)
#define RDR_RING_BASE_ADDR_HI_OFFSET(i)		(0x00000804 | i << 12)
#define RDR_DATA_BASE_ADDR_LO_OFFSET(i)		(0x00000808 | i << 12)
#define RDR_DATA_BASE_ADDR_HI_OFFSET(i)		(0x0000080C | i << 12)
#define RDR_RING_SIZE_OFFSET(i)				(0x00000818 | i << 12)
#define RDR_DESC_SIZE_OFFSET(i)				(0x0000081C | i << 12)
#define RDR_CFG_OFFSET(i)					(0x00000820 | i << 12)
#define RDR_DMA_CFG_OFFSET(i)				(0x00000824 | i << 12)
#define RDR_THRESH_OFFSET(i)				(0x00000828 | i << 12)
#define RDR_PREP_COUNT_OFFSET(i)			(0x0000082C | i << 12)
#define RDR_PROC_COUNT_OFFSET(i)			(0x00000830 | i << 12)
#define RDR_PREP_PNTR_OFFSET(i)				(0x00000834 | i << 12)
#define RDR_PROC_PNTR_OFFSET(i)				(0x00000838 | i << 12)
#define RDR_STAT_OFFSET(i)					(0x0000083C | i << 12)

#define HIA_DFE_CFG_OFFSET					0x0000f000
#define HIA_DFE_PRIO_0_OFFSET				0x0000f010
#define HIA_DFE_PRIO_1_OFFSET				0x0000f014
#define HIA_DFE_PRIO_2_OFFSET				0x0000f018
#define HIA_DFE_THR_CTRL_OFFSET				0x0000f200
//#define HIA_DFE_THR_CTRL_OFFSET(i)			(0x0000f200 + (i*64))

#define HIA_DSE_CFG_OFFSET					0x0000f400
#define HIA_DSE_PRIO_0_OFFSET				0x0000f410
#define HIA_DSE_PRIO_1_OFFSET				0x0000f414
#define HIA_DSE_PRIO_2_OFFSET				0x0000f418
#define HIA_DSE_THR_CTRL_OFFSET				0x0000f600
//#define HIA_DSE_THR_CTRL_OFFSET(i)			(0x0000f600 + (i*64))
#define HIA_AIC_GLOBAL_CTRL					0x0000f808
#define HIA_AIC_GLOBAL_ACK					0x0000f810
#define HIA_AIC_CLEAR_REG					0x0000f814
#define HIA_AIC_RING_CTRL(i)				(0x0000e008 - (0x1000 * i))
#define HIA_AIC_RING_ACK(i)					(0x0000e010 - (0x1000 * i))
#define HIA_AIC_RING_CLEAR(i)				(0x0000e014 - (0x1000 * i))


#define PE_IN_DBUF_THRESH					0x10000
#define PE_IN_TBUF_THRESH					0x10100
#define PE_EIP96_TOKEN_CTRL_STAT			0x11000
#define PE_EIP96_CONTEXT_CTRL				0x11008
#define PE_OUT_DBUF_THRESH					0x11c00

#define INSERT_LEN_MASK 					0x000001FF
#define INSERT_LEN_OFFSET					0
#define DIR_LEN_MASK						0x0001FFFF
#define DIR_LEN_OFFSET						0

#define ESP_NH_IP 							4
#define ESP_NH_IP6 							41
#define ESP_HDR_LEN							8
#define INSERT_XLIV_MASK					0x0001FE00
#define INSERT_XLIV_OFFSET					9
#define RDR_ERR_E0_BIT						17
#define E15_MASK							0x00000010


#define CDR_DW0_TEMPL						0x00c00000
#define RDR_DW0_TEMPL						0x04c00000
#define RDR_DW4_TEMPL						0x00201601

#define GCR_CUS_REG_CCA_IC_MREQ_0 0xb23F0018u
#define GCR_CUS_REG_CCA_IC_MREQ_1 0xb23F001Cu

#define GCR_CCA_IC_MREQ(id)		(void __iomem *)(GCR_CUS_REG_CCA_IC_MREQ_0 + (id * (GCR_CUS_REG_CCA_IC_MREQ_1 - GCR_CUS_REG_CCA_IC_MREQ_0)))
#define VIRT_TO_IOCU(addr) (((addr) & 0x1fffffff)|0xC0000000) // virtual to IOCU i.e 2 and 8 convert to C-D

#define set_val(reg, val, mask, offset) do {(reg) &= ~(mask); (reg) |= (((val) << (offset)) & (mask)); } while(0)

struct cdr_dw0 {
    uint32_t buf_size: 17;  /*!< size of input data buffer in bytes */
    uint32_t res: 5;            /*!< reserved */
    uint32_t last_seg: 1;   /*!< last segment indicator */
    uint32_t first_seg: 1;  /*!< first segment indicator */
    uint32_t acd_size: 8;   /*!< additional control data size */
};

struct cdr_dw1 {
    uint32_t buf_ptr;   /*!< data buffer pointer */
};

struct cdr_dw2 {
	uint32_t token_ptr;
};

struct cdr_dw3 {
    uint32_t len: 17; /*!< input packet length */
    uint32_t ip: 1; /*!< EIP97 Mode */
    uint32_t cp: 1; /*!< context pointer 64bit */
    uint32_t ct: 1; /*!< context type, reserved */
    uint32_t rc: 2; /*!< reuse context */
    uint32_t too: 3;    /*!< type of output */
    uint32_t c: 1;      /*!< context control words present in token */
    uint32_t iv: 3; /*!< IV??? */
    uint32_t u: 1;      /*!< upper layer header from token */
    uint32_t type: 2;   /*!< type of token, reserved*/
};

struct cdr_dw4 {
    uint32_t app_id: 24;    /*!< application id */
    uint32_t res: 8;        /*!< reserved */	
};

struct cdr_dw5 {
	uint32_t context_ptr;
};

struct cdr_desc {
	struct cdr_dw0 dw0;
	struct cdr_dw1 dw1;
	struct cdr_dw2 dw2;
	struct cdr_dw3 dw3;
	struct cdr_dw4 dw4;
	struct cdr_dw5 dw5;
};


struct rdr_dw0 {
    uint32_t buf_size: 17;  /*!< size of the output data segment buffer in bytes */
    uint32_t res: 3;            /*!< reserved */
    uint32_t desc_ovf: 1;       /*!< buffer overflow error indicator */
    uint32_t buf_ovf: 1;        /*!< buffer overflow error indicator */
    uint32_t last_seg: 1;   /*!< last segment indicator */
    uint32_t first_seg: 1;  /*!< first segment indicator */
    uint32_t result_size: 8;/*!< number of result data words written to this result descriptor */
};

struct rdr_dw1 {
	uint32_t buf_ptr;
};

struct rdr_dw2 {
    uint32_t pkt_len: 17;       /*!<  the length of the output packet in bytes; \n
                                        excluding the appended result fields */
    uint32_t err_code: 15;  /*!<  the error code (E14..E0) that is returned by the EIP-96 Packet Engine */

};

struct rdr_dw3 {
    uint32_t bypass_len: 4; /*!< the number of DWORDs of Bypass_Token_Words in the Result Descriptor */
    uint32_t E15: 1;            /*!< the error code (E15) that is returned by the EIP-96 Packet Engine */
    uint32_t res: 16;           /*!< reserved */
    uint32_t H: 1;              /*!< Hash Byte field appended at the end of the packet data flag */
    uint32_t hash_len: 6;       /*!< The number of appended hash bytes at the end of the packet data. \n
                                        Valid when H = 1 */
    uint32_t B: 1;  /*!< Generic Bytes field appended at the end of the packet data flag */
    uint32_t C: 1; /*!< Checksum field appended at the end of the packet data flag */
    uint32_t N: 1;  /*!< Next Header field appended at the end of the packet data flag */
    uint32_t L: 1;  /*!< Length field appended at the end of the packet data flag */
};

struct rdr_dw4 {
    uint32_t app_id: 24;    /*!< application id */
    uint32_t res: 8;        /*!< reserved */
};

struct rdr_dw5 {
    uint32_t next_hdr: 8;   /*!< next header result value from IPSec trailer */
    uint32_t pad_len: 8;        /*!<  number of detected (and removed) padding bytes */
    uint32_t res: 16;           /*!< reserved*/
};

struct rdr_desc {
	struct rdr_dw0 dw0;
	struct rdr_dw1 dw1;
	struct rdr_dw2 dw2;
	struct rdr_dw3 dw3;
	struct rdr_dw4 dw4;
	struct rdr_dw5 dw5;
};

extern int (*ltq_ipsec_enc_hook)(u32 spi, u16 ip_prot, u8 *in, u8 *out, uint16_t outBufLen, void (*callback)(struct ltq_ipsec_complete *done),
                         unsigned int buflen, void *ip_data);
extern int (*ltq_get_len_param_hook)(u32 spi, unsigned int *ivsize, unsigned int *ICV_length,
                         unsigned int *blksize);
extern int (*ltq_ipsec_dec_hook)(u32 spi, u8 *in, u8 *out, void (*callback)(struct ltq_ipsec_complete *done),
		unsigned int buflen, void *ip_data);
extern struct ltq_crypto_ipsec_params *(*ltq_ipsec_get_param_hook)(u32 spi);
extern void (*ltq_destroy_ipsec_sa_hook)(struct ltq_crypto_ipsec_params *req);
#endif /* CONFIG_LTQ_MPE_IPSEC_SUPPORT */
