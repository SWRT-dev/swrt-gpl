
/***********************************************************************************
File:				MacWepRegs.h
Module:				macWep
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_WEP_REGS_H_
#define _MAC_WEP_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_WEP_BASE_ADDRESS                             MEMORY_MAP_UNIT_17_BASE_ADDRESS
#define	REG_MAC_WEP_CONTROL                           (MAC_WEP_BASE_ADDRESS + 0x0)
#define	REG_MAC_WEP_STATUS                            (MAC_WEP_BASE_ADDRESS + 0x4)
#define	REG_MAC_WEP_SRC_ADDRESS                       (MAC_WEP_BASE_ADDRESS + 0x8)
#define	REG_MAC_WEP_DST_ADDRESS                       (MAC_WEP_BASE_ADDRESS + 0xC)
#define	REG_MAC_WEP_CUR_ADDRESS_OFFSET                (MAC_WEP_BASE_ADDRESS + 0x10)
#define	REG_MAC_WEP_LENGTH                            (MAC_WEP_BASE_ADDRESS + 0x14)
#define	REG_MAC_WEP_KEY_LENGTH                        (MAC_WEP_BASE_ADDRESS + 0x18)
#define	REG_MAC_WEP_KEY_31TO0                         (MAC_WEP_BASE_ADDRESS + 0x1C)
#define	REG_MAC_WEP_KEY_63TO32                        (MAC_WEP_BASE_ADDRESS + 0x20)
#define	REG_MAC_WEP_KEY_95TO64                        (MAC_WEP_BASE_ADDRESS + 0x24)
#define	REG_MAC_WEP_KEY_127TO96                       (MAC_WEP_BASE_ADDRESS + 0x28)
#define	REG_MAC_WEP_CRC_RESULT                        (MAC_WEP_BASE_ADDRESS + 0x2C)
#define	REG_MAC_WEP_TX_ABORT                          (MAC_WEP_BASE_ADDRESS + 0x30)
#define	REG_MAC_WEP_TX_ABORT_STATUS                   (MAC_WEP_BASE_ADDRESS + 0x34)
#define	REG_MAC_WEP_WRITE10                           (MAC_WEP_BASE_ADDRESS + 0x38)
#define	REG_MAC_WEP_WRITE11                           (MAC_WEP_BASE_ADDRESS + 0x3C)
#define	REG_MAC_WEP_MODE                              (MAC_WEP_BASE_ADDRESS + 0x40)
#define	REG_MAC_WEP_WRITE12                           (MAC_WEP_BASE_ADDRESS + 0x44)
#define	REG_MAC_WEP_WRITE13                           (MAC_WEP_BASE_ADDRESS + 0x48)
#define	REG_MAC_WEP_WRITE14                           (MAC_WEP_BASE_ADDRESS + 0x4C)
#define	REG_MAC_WEP_WRITE15                           (MAC_WEP_BASE_ADDRESS + 0x50)
#define	REG_MAC_WEP_WRITE16                           (MAC_WEP_BASE_ADDRESS + 0x54)
#define	REG_MAC_WEP_READ5                             (MAC_WEP_BASE_ADDRESS + 0x58)
#define	REG_MAC_WEP_READ6                             (MAC_WEP_BASE_ADDRESS + 0x5C)
#define	REG_MAC_WEP_READ7                             (MAC_WEP_BASE_ADDRESS + 0x60)
#define	REG_MAC_WEP_HARDWARE_ID_W0                    (MAC_WEP_BASE_ADDRESS + 0x64)
#define	REG_MAC_WEP_CCM_FC_MASK                       (MAC_WEP_BASE_ADDRESS + 0x68)
#define	REG_MAC_WEP_CCM_SC_MASK                       (MAC_WEP_BASE_ADDRESS + 0x6C)
#define	REG_MAC_WEP_CCM_QC_MASK                       (MAC_WEP_BASE_ADDRESS + 0x70)
#define	REG_MAC_WEP_CR_USER_ID                        (MAC_WEP_BASE_ADDRESS + 0x74)
#define	REG_MAC_WEP_FULL_HW_ENC                       (MAC_WEP_BASE_ADDRESS + 0x78)
#define	REG_MAC_WEP_START_ENTRIES_ADDRESS             (MAC_WEP_BASE_ADDRESS + 0x7C)
#define	REG_MAC_WEP_ENTRY_NUMBER                      (MAC_WEP_BASE_ADDRESS + 0x80)
#define	REG_MAC_WEP_LENGTH_OFFSET                     (MAC_WEP_BASE_ADDRESS + 0x84)
#define	REG_MAC_WEP_TKIP_CONTROL                      (MAC_WEP_BASE_ADDRESS + 0x88)
#define	REG_MAC_WEP_TKIP_MIC_CONTROL                  (MAC_WEP_BASE_ADDRESS + 0x8C)
#define	REG_MAC_WEP_WEP_CONTROL                       (MAC_WEP_BASE_ADDRESS + 0x90)
#define	REG_MAC_WEP_TKIP_FRAGMENT_CONTROL             (MAC_WEP_BASE_ADDRESS + 0x94)
#define	REG_MAC_WEP_SHRAM_MASK_WRITE                  (MAC_WEP_BASE_ADDRESS + 0x98)
#define	REG_MAC_WEP_MAX_MPDU_ADDRESS                  (MAC_WEP_BASE_ADDRESS + 0x9C)
#define	REG_MAC_WEP_IV_GENERATE_VALUE                 (MAC_WEP_BASE_ADDRESS + 0xA0)
#define	REG_MAC_WEP_SM_DEBUG_REGISTER                 (MAC_WEP_BASE_ADDRESS + 0xA4)
#define	REG_MAC_WEP_WIRELESS_HEADER_LENGTH            (MAC_WEP_BASE_ADDRESS + 0xA8)
#define	REG_MAC_WEP_FRAGMENT_DB_BASE_ADDR             (MAC_WEP_BASE_ADDRESS + 0xDC)
#define	REG_MAC_WEP_TKIP_FRAGMENT2_CONTROL            (MAC_WEP_BASE_ADDRESS + 0xE0)
#define	REG_MAC_WEP_FRAGMENT_ENTRY_NUM                (MAC_WEP_BASE_ADDRESS + 0xE4)
#define	REG_MAC_WEP_TKIP_FRAGMENT3_CONTROL            (MAC_WEP_BASE_ADDRESS + 0xE8)
#define	REG_MAC_WEP_DESC_ARBITER                      (MAC_WEP_BASE_ADDRESS + 0xEC)
#define	REG_MAC_WEP_TX_INTERRUPTS_EN                  (MAC_WEP_BASE_ADDRESS + 0xF0)
#define	REG_MAC_WEP_TX_INTERRUPTS_STATUS              (MAC_WEP_BASE_ADDRESS + 0xF4)
#define	REG_MAC_WEP_TX_INTERRUPTS_CLEAR               (MAC_WEP_BASE_ADDRESS + 0xF8)
#define	REG_MAC_WEP_TX_FAIL_STATUS                    (MAC_WEP_BASE_ADDRESS + 0xFC)
#define	REG_MAC_WEP_RX_INTERRUPTS_EN                  (MAC_WEP_BASE_ADDRESS + 0x100)
#define	REG_MAC_WEP_RX_INTERRUPTS_STATUS              (MAC_WEP_BASE_ADDRESS + 0x104)
#define	REG_MAC_WEP_RX_INTERRUPTS_CLEAR               (MAC_WEP_BASE_ADDRESS + 0x108)
#define	REG_MAC_WEP_RX_FAIL_STATUS                    (MAC_WEP_BASE_ADDRESS + 0x10C)
#define	REG_MAC_WEP_COMPLETE_STATUS                   (MAC_WEP_BASE_ADDRESS + 0x110)
#define	REG_MAC_WEP_CR_MIC_KEY128_31TO0               (MAC_WEP_BASE_ADDRESS + 0x114)
#define	REG_MAC_WEP_CR_MIC_KEY128_63TO32              (MAC_WEP_BASE_ADDRESS + 0x118)
#define	REG_MAC_WEP_CR_MIC_KEY128_95TO64              (MAC_WEP_BASE_ADDRESS + 0x11C)
#define	REG_MAC_WEP_CR_MIC_KEY128_127TO96             (MAC_WEP_BASE_ADDRESS + 0x120)
#define	REG_MAC_WEP_SEC_LOGGER                        (MAC_WEP_BASE_ADDRESS + 0x124)
#define	REG_MAC_WEP_SEC_LOGGER_ACTIVE                 (MAC_WEP_BASE_ADDRESS + 0x128)
#define	REG_MAC_WEP_AAD_ADDR                          (MAC_WEP_BASE_ADDRESS + 0x12C)
#define	REG_MAC_WEP_AAD_DATA                          (MAC_WEP_BASE_ADDRESS + 0x130)
#define	REG_MAC_WEP_DLM_MPDU_LIST_IDX_OUT             (MAC_WEP_BASE_ADDRESS + 0x134)
#define	REG_MAC_WEP_SEC_LOGGER_FILTER                 (MAC_WEP_BASE_ADDRESS + 0x138)
#define	REG_MAC_WEP_FIFOS_STATUS                      (MAC_WEP_BASE_ADDRESS + 0x13C)
#define	REG_MAC_WEP_MIC_INDICATION_15TO0              (MAC_WEP_BASE_ADDRESS + 0x140)
#define	REG_MAC_WEP_MIC_INDICATION_31TO16             (MAC_WEP_BASE_ADDRESS + 0x144)
#define	REG_MAC_WEP_MIC_INDICATION_35TO32             (MAC_WEP_BASE_ADDRESS + 0x148)
#define	REG_MAC_WEP_USER_ACTIVE_INDICATION_15TO0      (MAC_WEP_BASE_ADDRESS + 0x14C)
#define	REG_MAC_WEP_USER_ACTIVE_INDICATION_31TO16     (MAC_WEP_BASE_ADDRESS + 0x150)
#define	REG_MAC_WEP_USER_ACTIVE_INDICATION_47TO32     (MAC_WEP_BASE_ADDRESS + 0x154)
#define	REG_MAC_WEP_USER_ACTIVE_INDICATION_63TO48     (MAC_WEP_BASE_ADDRESS + 0x158)
#define	REG_MAC_WEP_USER_ACTIVE_INDICATION_71TO64     (MAC_WEP_BASE_ADDRESS + 0x15C)
#define	REG_MAC_WEP_TX_IDLE_INDICATION                (MAC_WEP_BASE_ADDRESS + 0x160)
#define	REG_MAC_WEP_TX_DLM_FC_CONTROL                 (MAC_WEP_BASE_ADDRESS + 0x164)
#define	REG_MAC_WEP_RX_DLM_FC_CONTROL                 (MAC_WEP_BASE_ADDRESS + 0x168)
#define	REG_MAC_WEP_WR_FC_STROBES                     (MAC_WEP_BASE_ADDRESS + 0x16C)
#define	REG_MAC_WEP_CR_MAX_TKIP_FRAG_LENGTH           (MAC_WEP_BASE_ADDRESS + 0x170)
#define	REG_MAC_WEP_CR_USER_BYPASS_HC_RD_FC_31TO0     (MAC_WEP_BASE_ADDRESS + 0x174)
#define	REG_MAC_WEP_CR_USER_BYPASS_HC_RD_FC_35TO32    (MAC_WEP_BASE_ADDRESS + 0x178)
#define	REG_MAC_WEP_BIP_FC_MASK                       (MAC_WEP_BASE_ADDRESS + 0x17C)
#define	REG_MAC_WEP_SPARE_REG                         (MAC_WEP_BASE_ADDRESS + 0x180)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_WEP_CONTROL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 start : 1; //start, reset value: 0x0, access type: WO
		uint32 controlReg : 15; //Control register, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacWepControl_u;

/*REG_MAC_WEP_STATUS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 active : 1; //WEP2  active, reset value: 0x0, access type: RO
		uint32 crcfail : 1; //WEP2  crcfail, reset value: 0x0, access type: RO
		uint32 micfail : 1; //WEP2  micfail, reset value: 0x0, access type: RO
		uint32 notKeyValid : 1; //not_key_valid, reset value: 0x0, access type: RO
		uint32 notProtectedFrame : 1; //not_protected_frame, reset value: 0x0, access type: RO
		uint32 lengthError : 1; //length_error, reset value: 0x0, access type: RO
		uint32 eivFieldError : 1; //eiv_field_error, reset value: 0x0, access type: RO
		uint32 fragLengthShortError : 1; //frag_length_short_error, reset value: 0x0, access type: RO
		uint32 fragNumNewEqualsPrevError : 1; //frag_num_new_equals_prev_error, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegMacWepStatus_u;

/*REG_MAC_WEP_SRC_ADDRESS 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcAddress : 25; //WEP2  src address, reset value: 0x0, access type: RW
		uint32 encModeConfig : 4; //enc_mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 sppAMsduCapable : 1; //spp_a_msdu_capable, reset value: 0x0, access type: RW
		uint32 encryptMode : 1; //encrypt, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepSrcAddress_u;

/*REG_MAC_WEP_DST_ADDRESS 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dstAddress : 25; //WEP2  dst address, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 keyId : 2; //key_id, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegMacWepDstAddress_u;

/*REG_MAC_WEP_CUR_ADDRESS_OFFSET 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 curAddressOffset : 16; //WEP2  cur address offset, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegMacWepCurAddressOffset_u;

/*REG_MAC_WEP_LENGTH 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 length : 16; //WEP2  length, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacWepLength_u;

/*REG_MAC_WEP_KEY_LENGTH 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 keyLength : 6; //WEP2  key length, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegMacWepKeyLength_u;

/*REG_MAC_WEP_KEY_31TO0 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 key31To0 : 32; //WEP2  key 31to0, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepKey31To0_u;

/*REG_MAC_WEP_KEY_63TO32 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 key63To32 : 32; //WEP2  key 63to32, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepKey63To32_u;

/*REG_MAC_WEP_KEY_95TO64 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 key95To64 : 32; //WEP2  key 95to64, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepKey95To64_u;

/*REG_MAC_WEP_KEY_127TO96 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 key127To96 : 32; //WEP2  key 127to96, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepKey127To96_u;

/*REG_MAC_WEP_CRC_RESULT 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcResult : 32; //WEP2  crc result, reset value: 0x0, access type: RO
	} bitFields;
} RegMacWepCrcResult_u;

/*REG_MAC_WEP_TX_ABORT 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txAbortPulse : 1; //Trigger to start Tx abort., reset value: 0x0, access type: WO
		uint32 txFlushPulse : 1; //Trigger to start Tx descriptors flush., reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacWepTxAbort_u;

/*REG_MAC_WEP_TX_ABORT_STATUS 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txAbortDone : 1; //Tx abort done indication., reset value: 0x1, access type: RO
		uint32 txFlushDone : 1; //Tx flush done indication., reset value: 0x1, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacWepTxAbortStatus_u;

/*REG_MAC_WEP_WRITE10 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write10 : 32; //write10_w0, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepWrite10_u;

/*REG_MAC_WEP_WRITE11 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write11 : 32; //write11_w0, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepWrite11_u;

/*REG_MAC_WEP_MODE 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mode : 4; //Security mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegMacWepMode_u;

/*REG_MAC_WEP_WRITE12 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write12 : 32; //write12_w0, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepWrite12_u;

/*REG_MAC_WEP_WRITE13 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write13 : 32; //write13_w0, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepWrite13_u;

/*REG_MAC_WEP_WRITE14 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write14 : 32; //write14_w0, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepWrite14_u;

/*REG_MAC_WEP_WRITE15 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write15 : 32; //write15_w0, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepWrite15_u;

/*REG_MAC_WEP_WRITE16 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write16 : 32; //write16_w0, reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepWrite16_u;

/*REG_MAC_WEP_READ5 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 read5 : 32; //read5_w0, reset value: 0x0, access type: RO
	} bitFields;
} RegMacWepRead5_u;

/*REG_MAC_WEP_READ6 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 read6 : 32; //read6_w0, reset value: 0x0, access type: RO
	} bitFields;
} RegMacWepRead6_u;

/*REG_MAC_WEP_READ7 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 read7 : 32; //read7_w0, reset value: 0x0, access type: RO
	} bitFields;
} RegMacWepRead7_u;

/*REG_MAC_WEP_HARDWARE_ID_W0 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hardwareIdAes : 1; //hardware id AES, reset value: 0x1, access type: RO
		uint32 hardwareIdOcb : 1; //hardware id OCB, reset value: 0x0, access type: RO
		uint32 hardwareIdCcm : 1; //hardware id CCM, reset value: 0x1, access type: RO
		uint32 hardwareIdWapi : 1; //hardware id WAPI, reset value: 0x1, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacWepHardwareIdW0_u;

/*REG_MAC_WEP_CCM_FC_MASK 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccmFcMask : 16; //ccm_fc_mask, reset value: 0xc78f, access type: RW
		uint32 ccmFcManagementMask : 16; //ccm_fc_management_mask, reset value: 0xc7ff, access type: RW
	} bitFields;
} RegMacWepCcmFcMask_u;

/*REG_MAC_WEP_CCM_SC_MASK 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccmScMask : 16; //ccm_sc_mask, reset value: 0xf, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacWepCcmScMask_u;

/*REG_MAC_WEP_CCM_QC_MASK 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccmQcMask : 16; //ccm_qc_mask, reset value: 0x8f, access type: RW
		uint32 managementNonceMask : 1; //management_nonce_mask, reset value: 0x1, access type: RW
		uint32 reserved0 : 15;
	} bitFields;
} RegMacWepCcmQcMask_u;

/*REG_MAC_WEP_CR_USER_ID 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crUserId : 8; //user_id, used when descriptor_list_en is set to '0'., reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacWepCrUserId_u;

/*REG_MAC_WEP_FULL_HW_ENC 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fullHwEnc : 1; //full_hw_enc, reset value: 0x1, access type: RW
		uint32 rc4PreInitHwEn : 1; //rc4_pre_init_hw_en, reset value: 0x1, access type: RW
		uint32 descriptorListEn : 1; //descriptor_list_en, reset value: 0x1, access type: RW
		uint32 txDescModeGenErrIrqEn : 1; //tx_desc_mode_gen_err_irq_en, reset value: 0x0, access type: RW
		uint32 rxDescModeGenErrIrqEn : 1; //rx_desc_mode_gen_err_irq_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegMacWepFullHwEnc_u;

/*REG_MAC_WEP_START_ENTRIES_ADDRESS 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startEntriesAddress : 22; //start_entries_address, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 dbEntrySize : 4; //entry_length, reset value: 0x4, access type: RW
		uint32 differentDest : 1; //different_dest, reset value: 0x1, access type: RW
		uint32 prepareIv : 1; //prepare_iv, reset value: 0x1, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegMacWepStartEntriesAddress_u;

/*REG_MAC_WEP_ENTRY_NUMBER 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entryNumber : 12; //entry_number, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 micEntryNumber : 12; //mic_entry_number, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
	} bitFields;
} RegMacWepEntryNumber_u;

/*REG_MAC_WEP_LENGTH_OFFSET 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lengthOffsetEncrypt : 3; //length_offset_encrypt, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 lengthOffsetDecrypt : 3; //length_offset_decrypt, reset value: 0x0, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegMacWepLengthOffset_u;

/*REG_MAC_WEP_TKIP_CONTROL 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tkipControlDec : 16; //tkip_control_dec, reset value: 0x580b, access type: RW
		uint32 tkipControlEnc : 16; //tkip_control_enc, reset value: 0x58af, access type: RW
	} bitFields;
} RegMacWepTkipControl_u;

/*REG_MAC_WEP_TKIP_MIC_CONTROL 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tkipMicControlHeader : 16; //tkip_mic_control_header, reset value: 0x4009, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacWepTkipMicControl_u;

/*REG_MAC_WEP_WEP_CONTROL 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wepControlDec : 16; //wep_control_dec, reset value: 0xb, access type: RW
		uint32 wepControlEnc : 16; //wep_control_enc, reset value: 0x3f, access type: RW
	} bitFields;
} RegMacWepWepControl_u;

/*REG_MAC_WEP_TKIP_FRAGMENT_CONTROL 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tkipControlFragmentDec : 16; //tkip_control_fragment_dec, reset value: 0x402b, access type: RW
		uint32 tkipControlFragmentEnc : 16; //tkip_control_fragment_enc, reset value: 0x402f, access type: RW
	} bitFields;
} RegMacWepTkipFragmentControl_u;

/*REG_MAC_WEP_SHRAM_MASK_WRITE 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramMaskWrite : 1; //shram_mask_write, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacWepShramMaskWrite_u;

/*REG_MAC_WEP_MAX_MPDU_ADDRESS 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduAddress : 25; //Maximum MPDU address, used in SW mode only., reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegMacWepMaxMpduAddress_u;

/*REG_MAC_WEP_IV_GENERATE_VALUE 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ivGenerateValue : 32; //iv_generate_value, reset value: 0x12345678, access type: RW
	} bitFields;
} RegMacWepIvGenerateValue_u;

/*REG_MAC_WEP_SM_DEBUG_REGISTER 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mutedHdrStateMachine : 4; //muted_hdr_state_machine, reset value: 0x0, access type: RO
		uint32 encStateMachine : 4; //enc_state_machine, reset value: 0x0, access type: RO
		uint32 wepStateMachine : 4; //wep_state_machine, reset value: 0x0, access type: RO
		uint32 ccmpStateMachine : 3; //ccmp_state_machine, reset value: 0x0, access type: RO
		uint32 gcmpStateMachine : 3; //gcmp_state_machine, reset value: 0x0, access type: RO
		uint32 bipCmacState : 2; //BIP-CMAC FSM state, reset value: 0x0, access type: RO
		uint32 bipGmacState : 3; //BIP-GMAC FSM state, reset value: 0x0, access type: RO
		uint32 keySmState : 2; //Read key FSM state, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegMacWepSmDebugRegister_u;

/*REG_MAC_WEP_WIRELESS_HEADER_LENGTH 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wirelessHeaderLength : 6; //wireless_header_length, reset value: 0x18, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegMacWepWirelessHeaderLength_u;

/*REG_MAC_WEP_FRAGMENT_DB_BASE_ADDR 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentDbBaseAddr : 22; //fragment_db_base_addr, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegMacWepFragmentDbBaseAddr_u;

/*REG_MAC_WEP_TKIP_FRAGMENT2_CONTROL 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tkipMicControlFirstFrag : 16; //tkip_mic_control_first_frag, reset value: 0x5009, access type: RW
		uint32 tkipMicControlLastFrag : 16; //tkip_mic_control_last_frag, reset value: 0x6089, access type: RW
	} bitFields;
} RegMacWepTkipFragment2Control_u;

/*REG_MAC_WEP_FRAGMENT_ENTRY_NUM 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentEntryNum : 9; //fragment_entry_num, reset value: 0x0, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegMacWepFragmentEntryNum_u;

/*REG_MAC_WEP_TKIP_FRAGMENT3_CONTROL 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tkipMicControlMiddleFrag : 16; //tkip_mic_control_middle_frag, reset value: 0x6009, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacWepTkipFragment3Control_u;

/*REG_MAC_WEP_DESC_ARBITER 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 arbiterResultValid : 1; //Valid descriptor arbiter result., reset value: 0x0, access type: RO
		uint32 arbiterResultTxRxN : 1; //When set descriptor arbiter selects Tx descriptor list, otherwise Rx descriptor list is selected., reset value: 0x0, access type: RO
		uint32 arbiterResult : 6; //The user selected by the descriptor arbiter., reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegMacWepDescArbiter_u;

/*REG_MAC_WEP_TX_INTERRUPTS_EN 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 txInterruptsEn : 8; //Tx Interrupts enable, reset value: 0xff, access type: RW
		uint32 reserved1 : 23;
	} bitFields;
} RegMacWepTxInterruptsEn_u;

/*REG_MAC_WEP_TX_INTERRUPTS_STATUS 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDoneIndicationIrq : 1; //tx_done_indication_irq, reset value: 0x0, access type: RO
		uint32 txFailureIndicationIrq : 1; //tx_failure_indication_irq, reset value: 0x0, access type: RO
		uint32 txMicFailureIrq : 1; //tx_mic_failure_irq, reset value: 0x0, access type: RO
		uint32 txNotKeyValidIrq : 1; //tx_not_key_valid_irq, reset value: 0x0, access type: RO
		uint32 txNotProtectedFrameIrq : 1; //tx_not_protected_frame_irq, reset value: 0x0, access type: RO
		uint32 txLengthErrorIrq : 1; //tx_length_error_irq, reset value: 0x0, access type: RO
		uint32 txEivFieldErrorIrq : 1; //tx_eiv_field_error_irq, reset value: 0x0, access type: RO
		uint32 txFragLengthShortErrorIrq : 1; //tx_frag_length_short_error_irq, reset value: 0x0, access type: RO
		uint32 txFragNumNewEqualsPrevIrq : 1; //tx_frag_num_new_equals_prev_irq, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegMacWepTxInterruptsStatus_u;

/*REG_MAC_WEP_TX_INTERRUPTS_CLEAR 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInterruptsClear : 9; //Tx Interrupts clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 23;
	} bitFields;
} RegMacWepTxInterruptsClear_u;

/*REG_MAC_WEP_TX_FAIL_STATUS 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srTxFailDescriptorPointer : 16; //Tx fail descriptor pointer., reset value: 0x0, access type: RO
		uint32 srTxFailUserId : 7; //Tx fail user ID., reset value: 0x0, access type: RO
		uint32 reserved0 : 9;
	} bitFields;
} RegMacWepTxFailStatus_u;

/*REG_MAC_WEP_RX_INTERRUPTS_EN 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 rxInterruptsEn : 9; //Rx Interrupts enable, reset value: 0x1ff, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegMacWepRxInterruptsEn_u;

/*REG_MAC_WEP_RX_INTERRUPTS_STATUS 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDoneIndicationIrq : 1; //rx_done_indication_irq, reset value: 0x0, access type: RO
		uint32 rxFailureIndicationIrq : 1; //rx_failure_indication_irq, reset value: 0x0, access type: RO
		uint32 rxMicFailureIrq : 1; //rx_mic_failure_irq, reset value: 0x0, access type: RO
		uint32 rxNotKeyValidIrq : 1; //rx_not_key_valid_irq, reset value: 0x0, access type: RO
		uint32 rxNotProtectedFrameIrq : 1; //rx_not_protected_frame_irq, reset value: 0x0, access type: RO
		uint32 rxLengthErrorIrq : 1; //rx_length_error_irq, reset value: 0x0, access type: RO
		uint32 rxEivFieldErrorIrq : 1; //rx_eiv_field_error_irq, reset value: 0x0, access type: RO
		uint32 rxFragLengthShortErrorIrq : 1; //rx_frag_length_short_error_irq, reset value: 0x0, access type: RO
		uint32 rxFragNumNewEqualsPrevIrq : 1; //rx_frag_num_new_equals_prev_irq, reset value: 0x0, access type: RO
		uint32 rxUnexpectedBipIrq : 1; //Rx unexpected BIP-CMAC/GMAC mode received interrupt. Frame is treated as Skip mode in this case., reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegMacWepRxInterruptsStatus_u;

/*REG_MAC_WEP_RX_INTERRUPTS_CLEAR 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInterruptsClear : 10; //Rx Interrupts clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 22;
	} bitFields;
} RegMacWepRxInterruptsClear_u;

/*REG_MAC_WEP_RX_FAIL_STATUS 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srRxFailDescriptorPointer : 16; //Rx fail descriptor pointer., reset value: 0x0, access type: RO
		uint32 srRxFailUserId : 7; //Rx fail user ID., reset value: 0x0, access type: RO
		uint32 reserved0 : 9;
	} bitFields;
} RegMacWepRxFailStatus_u;

/*REG_MAC_WEP_COMPLETE_STATUS 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srDescriptorPointer : 16; //Complete descriptor pointer., reset value: 0x0, access type: RO
		uint32 srUserId : 7; //Complete user ID., reset value: 0x0, access type: RO
		uint32 reserved0 : 9;
	} bitFields;
} RegMacWepCompleteStatus_u;

/*REG_MAC_WEP_CR_MIC_KEY128_31TO0 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crMicKey12831To0 : 32; //GCMP256 upper 128bit key, key[159:128], reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepCrMicKey12831To0_u;

/*REG_MAC_WEP_CR_MIC_KEY128_63TO32 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crMicKey12863To32 : 32; //GCMP256 upper 128bit key, key[191:160], reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepCrMicKey12863To32_u;

/*REG_MAC_WEP_CR_MIC_KEY128_95TO64 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crMicKey12895To64 : 32; //GCMP256 upper 128bit key, key[223:192], reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepCrMicKey12895To64_u;

/*REG_MAC_WEP_CR_MIC_KEY128_127TO96 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crMicKey128127To96 : 32; //GCMP256 upper 128bit key, key[255:224], reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepCrMicKey128127To96_u;

/*REG_MAC_WEP_SEC_LOGGER 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 secLoggerEn : 1; //Security engine logger enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
		uint32 secLoggerPriority : 2; //Security engine logger priority, reset value: 0x0, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegMacWepSecLogger_u;

/*REG_MAC_WEP_SEC_LOGGER_ACTIVE 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 secLoggerActive : 1; //Security engine logger active, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacWepSecLoggerActive_u;

/*REG_MAC_WEP_AAD_ADDR 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aadAddr : 4; //AAD buffer address to be loaded by aad_data in SW mode. , Address resolution is 4 bytes (32bits)., reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegMacWepAadAddr_u;

/*REG_MAC_WEP_AAD_DATA 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aadData : 32; //AAD buffer data in SW mode., reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepAadData_u;

/*REG_MAC_WEP_DLM_MPDU_LIST_IDX_OUT 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crDlmTxMpduListIdxOut : 8; //DLM Tx push descriptor pointer base list index., reset value: 0x50, access type: RW
		uint32 crDlmRxMpduListIdxOut : 8; //DLM Rx push descriptor pointer base list index., reset value: 0x2c, access type: RW
		uint32 crDlmFreeMpduListIdxOut : 8; //DLM Free push descriptor pointer base list index., reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacWepDlmMpduListIdxOut_u;

/*REG_MAC_WEP_SEC_LOGGER_FILTER 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crLogTx : 1; //Filter only encryption logs, reset value: 0x1, access type: RW
		uint32 crLogRx : 1; //Filter only decryption logs, reset value: 0x1, access type: RW
		uint32 crLogError : 1; //Filter only erroneous logs, reset value: 0x1, access type: RW
		uint32 crLogNoError : 1; //Filter only non erroneous logs, reset value: 0x1, access type: RW
		uint32 crLogUserId0 : 8; //Filter only users with user_id equals cr_log_user_id0, reset value: 0x0, access type: RW
		uint32 crLogUserId1 : 8; //Filter only users with user_id equals cr_log_user_id0, reset value: 0x24, access type: RW
		uint32 crLogAllUsers : 1; //Log all users, reset value: 0x1, access type: RW
		uint32 crLogEncMode : 4; //Filter only users with enc_mode equals cr_log_enc_mode, reset value: 0x0, access type: RW
		uint32 crLogAllEncMode : 1; //Log all encryption modes, reset value: 0x1, access type: RW
		uint32 reserved0 : 6;
	} bitFields;
} RegMacWepSecLoggerFilter_u;

/*REG_MAC_WEP_FIFOS_STATUS 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qOrderFifoBytes : 2; //Valid entries in Q order FIFO., reset value: 0x0, access type: RO
		uint32 qOrderFifoEmpty : 1; //Q order FIFO empty indication., reset value: 0x1, access type: RO
		uint32 qOrderFifoFull : 1; //Q order FIFO full indication., reset value: 0x0, access type: RO
		uint32 rdBlockFifoBytes : 3; //Valid entries in read block FIFO., reset value: 0x0, access type: RO
		uint32 rdBlockFifoEmpty : 1; //Read block FIFO empty indication., reset value: 0x1, access type: RO
		uint32 rdBlockFifoFull : 1; //Read block FIFO full indication., reset value: 0x0, access type: RO
		uint32 wrBlockFifoBytes : 4; //Valid entries in write block FIFO., reset value: 0x0, access type: RO
		uint32 wrBlockFifoEmpty : 1; //Write block FIFO empty indication., reset value: 0x1, access type: RO
		uint32 wrBlockFifoFull : 1; //Write block FIFO full indication., reset value: 0x0, access type: RO
		uint32 wrRxDescriptorStatusFifoBytes : 4; //Valid entries in write rx descriptor status FIFO., reset value: 0x0, access type: RO
		uint32 wrRxDescriptorStatusFifoEmpty : 1; //Write rx descriptor status FIFO empty indication., reset value: 0x1, access type: RO
		uint32 wrRxDescriptorStatusFifoFull : 1; //Write rx descriptor status FIFO full indication, reset value: 0x0, access type: RO
		uint32 secCmpDataArbFifoBytes : 2; //Valid entries in CMP Data arbiter FIFO., reset value: 0x0, access type: RO
		uint32 secCmpDataArbFifoEmpty : 1; //CMP Data arbiter FIFO empty indication., reset value: 0x1, access type: RO
		uint32 secCmpDataArbFifoFull : 1; //CMP Data arbiter FIFO full indication., reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegMacWepFifosStatus_u;

/*REG_MAC_WEP_MIC_INDICATION_15TO0 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 micValid15To0 : 16; //mic_valid[15:0], reset value: 0x0, access type: RO
		uint32 micIdx15To0 : 16; //mic_idx[15:0], reset value: 0x0, access type: RO
	} bitFields;
} RegMacWepMicIndication15To0_u;

/*REG_MAC_WEP_MIC_INDICATION_31TO16 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 micValid31To16 : 16; //mic_valid[31:16], reset value: 0x0, access type: RO
		uint32 micIdx31To16 : 16; //mic_idx[31:16], reset value: 0x0, access type: RO
	} bitFields;
} RegMacWepMicIndication31To16_u;

/*REG_MAC_WEP_MIC_INDICATION_35TO32 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 micValid35To32 : 4; //mic_valid[35:32], reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
		uint32 micIdx35To32 : 4; //mic_idx[35:32], reset value: 0x0, access type: RO
		uint32 reserved1 : 12;
	} bitFields;
} RegMacWepMicIndication35To32_u;

/*REG_MAC_WEP_USER_ACTIVE_INDICATION_15TO0 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 userActive15To0 : 16; //user_active[15:0], reset value: 0x0, access type: RO
		uint32 userActiveTxRxN15To0 : 16; //user_active_tx_rx_n[15:0], reset value: 0x0, access type: RO
	} bitFields;
} RegMacWepUserActiveIndication15To0_u;

/*REG_MAC_WEP_USER_ACTIVE_INDICATION_31TO16 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 userActive31To16 : 16; //user_active[31:16], reset value: 0x0, access type: RO
		uint32 userActiveTxRxN31To16 : 16; //user_active_tx_rx_n[31:16], reset value: 0x0, access type: RO
	} bitFields;
} RegMacWepUserActiveIndication31To16_u;

/*REG_MAC_WEP_USER_ACTIVE_INDICATION_47TO32 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 userActive47To32 : 16; //user_active[47:32], reset value: 0x0, access type: RO
		uint32 userActiveTxRxN47To32 : 16; //user_active_tx_rx_n[47:32], reset value: 0x0, access type: RO
	} bitFields;
} RegMacWepUserActiveIndication47To32_u;

/*REG_MAC_WEP_USER_ACTIVE_INDICATION_63TO48 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 userActive63To48 : 16; //user_active[63:48], reset value: 0x0, access type: RO
		uint32 userActiveTxRxN63To48 : 16; //user_active_tx_rx_n[63:48], reset value: 0x0, access type: RO
	} bitFields;
} RegMacWepUserActiveIndication63To48_u;

/*REG_MAC_WEP_USER_ACTIVE_INDICATION_71TO64 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 userActive71To64 : 8; //user_active[71:64], reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
		uint32 userActiveTxRxN71To64 : 8; //user_active_tx_rx_n[71:64], reset value: 0x0, access type: RO
		uint32 reserved1 : 8;
	} bitFields;
} RegMacWepUserActiveIndication71To64_u;

/*REG_MAC_WEP_TX_IDLE_INDICATION 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txUserCnt : 7; //Tx active user counter., reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 txSecurityPipeIdle : 1; //When set there are no pending Tx jobs in pipe., reset value: 0x1, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegMacWepTxIdleIndication_u;

/*REG_MAC_WEP_TX_DLM_FC_CONTROL 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crEnableTxDlm : 1; //Enable Tx DLM., reset value: 0x0, access type: RW
		uint32 crTxBypassRdFc : 1; //Tx bypass (ignore) read flow control., reset value: 0x0, access type: RW
		uint32 crTxDisableUpdateWrFc : 1; //Tx disable update write flow control., reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegMacWepTxDlmFcControl_u;

/*REG_MAC_WEP_RX_DLM_FC_CONTROL 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crEnableRxDlm : 1; //Enable Rx DLM., reset value: 0x0, access type: RW
		uint32 crRxBypassRdFc : 1; //Rx bypass (ignore) read flow control., reset value: 0x0, access type: RW
		uint32 crRxDisableUpdateWrFc : 1; //Rx disable update write flow control., reset value: 0x0, access type: RW
		uint32 crBlockRxIfTxDescNotEmpty : 1; //When set Rx descriptors are blocked if there is Tx descriptor which is not empty. , Still if unset Tx gets priority over Rx, but then if there are 2 outstanding Tx jobs for each of the nonempty Tx users (up to two pending jobs per user may enter the pipe), Rx descriptors may get service., reset value: 0x1, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegMacWepRxDlmFcControl_u;

/*REG_MAC_WEP_WR_FC_STROBES 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crClrRoofMinAddrPulse : 1; //Clear roof_min_addr flop for all users. Should be cleared on any new transmission by SW., reset value: 0x0, access type: WO
		uint32 crTxClearWrFcPulse : 1; //Tx clear write flow control., reset value: 0x0, access type: WO
		uint32 crRxClearWrFcPulse : 1; //Rx clear write flow control., reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegMacWepWrFcStrobes_u;

/*REG_MAC_WEP_CR_MAX_TKIP_FRAG_LENGTH 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crMaxTkipFragLength : 16; //Max TKIP fragment frame length. When exceeded an error of type length_error will be indicated., reset value: 0x800, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacWepCrMaxTkipFragLength_u;

/*REG_MAC_WEP_CR_USER_BYPASS_HC_RD_FC_31TO0 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crUserBypassHcRdFc31To0 : 32; //Per user bypass (ignore) header conversion read flow control., reset value: 0x0, access type: RW
	} bitFields;
} RegMacWepCrUserBypassHcRdFc31To0_u;

/*REG_MAC_WEP_CR_USER_BYPASS_HC_RD_FC_35TO32 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crUserBypassHcRdFc35To32 : 4; //Per user bypass (ignore) header conversion read flow control., reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegMacWepCrUserBypassHcRdFc35To32_u;

/*REG_MAC_WEP_BIP_FC_MASK 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crBipFcMask : 16; //BIP FC mask, reset value: 0xc7ff, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacWepBipFcMask_u;

/*REG_MAC_WEP_SPARE_REG 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareReg : 32; //Spare register, reset value: 0x88888888, access type: RW
	} bitFields;
} RegMacWepSpareReg_u;



#endif // _MAC_WEP_REGS_H_
