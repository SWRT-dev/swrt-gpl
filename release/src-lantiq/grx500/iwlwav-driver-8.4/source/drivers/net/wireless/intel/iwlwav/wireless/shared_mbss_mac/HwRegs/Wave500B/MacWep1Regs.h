
/***********************************************************************************
File:				MacWep1Regs.h
Module:				macWep1
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_WEP1_REGS_H_
#define _MAC_WEP1_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_WEP1_BASE_ADDRESS                             MEMORY_MAP_UNIT_73_BASE_ADDRESS
#define	REG_MAC_WEP1_CONTROL                           (MAC_WEP1_BASE_ADDRESS + 0x0)
#define	REG_MAC_WEP1_STATUS                            (MAC_WEP1_BASE_ADDRESS + 0x4)
#define	REG_MAC_WEP1_SRC_ADDRESS                       (MAC_WEP1_BASE_ADDRESS + 0x8)
#define	REG_MAC_WEP1_DST_ADDRESS                       (MAC_WEP1_BASE_ADDRESS + 0xC)
#define	REG_MAC_WEP1_CUR_ADDRESS_OFFSET                (MAC_WEP1_BASE_ADDRESS + 0x10)
#define	REG_MAC_WEP1_LENGTH                            (MAC_WEP1_BASE_ADDRESS + 0x14)
#define	REG_MAC_WEP1_KEY_LENGTH                        (MAC_WEP1_BASE_ADDRESS + 0x18)
#define	REG_MAC_WEP1_KEY_31TO0                         (MAC_WEP1_BASE_ADDRESS + 0x1C)
#define	REG_MAC_WEP1_KEY_63TO32                        (MAC_WEP1_BASE_ADDRESS + 0x20)
#define	REG_MAC_WEP1_KEY_95TO64                        (MAC_WEP1_BASE_ADDRESS + 0x24)
#define	REG_MAC_WEP1_KEY_127TO96                       (MAC_WEP1_BASE_ADDRESS + 0x28)
#define	REG_MAC_WEP1_CRC_RESULT                        (MAC_WEP1_BASE_ADDRESS + 0x2C)
#define	REG_MAC_WEP1_DESC_CLEAR_STATE                  (MAC_WEP1_BASE_ADDRESS + 0x30)
#define	REG_MAC_WEP1_WRITE10                           (MAC_WEP1_BASE_ADDRESS + 0x34)
#define	REG_MAC_WEP1_WRITE11                           (MAC_WEP1_BASE_ADDRESS + 0x38)
#define	REG_MAC_WEP1_MODE                              (MAC_WEP1_BASE_ADDRESS + 0x40)
#define	REG_MAC_WEP1_WRITE12                           (MAC_WEP1_BASE_ADDRESS + 0x44)
#define	REG_MAC_WEP1_WRITE13                           (MAC_WEP1_BASE_ADDRESS + 0x48)
#define	REG_MAC_WEP1_WRITE14                           (MAC_WEP1_BASE_ADDRESS + 0x4C)
#define	REG_MAC_WEP1_WRITE15                           (MAC_WEP1_BASE_ADDRESS + 0x50)
#define	REG_MAC_WEP1_WRITE16                           (MAC_WEP1_BASE_ADDRESS + 0x54)
#define	REG_MAC_WEP1_READ5                             (MAC_WEP1_BASE_ADDRESS + 0x58)
#define	REG_MAC_WEP1_READ6                             (MAC_WEP1_BASE_ADDRESS + 0x5C)
#define	REG_MAC_WEP1_READ7                             (MAC_WEP1_BASE_ADDRESS + 0x60)
#define	REG_MAC_WEP1_HARDWARE_ID_W0                    (MAC_WEP1_BASE_ADDRESS + 0x64)
#define	REG_MAC_WEP1_CCM_FC_MASK                       (MAC_WEP1_BASE_ADDRESS + 0x68)
#define	REG_MAC_WEP1_CCM_SC_MASK                       (MAC_WEP1_BASE_ADDRESS + 0x6C)
#define	REG_MAC_WEP1_CCM_QC_MASK                       (MAC_WEP1_BASE_ADDRESS + 0x70)
#define	REG_MAC_WEP1_MUTED_HDR_ADDRESS                 (MAC_WEP1_BASE_ADDRESS + 0x74)
#define	REG_MAC_WEP1_FULL_HW_ENC                       (MAC_WEP1_BASE_ADDRESS + 0x78)
#define	REG_MAC_WEP1_START_ENTRIES_ADDRESS             (MAC_WEP1_BASE_ADDRESS + 0x7C)
#define	REG_MAC_WEP1_ENTRY_NUMBER                      (MAC_WEP1_BASE_ADDRESS + 0x80)
#define	REG_MAC_WEP1_LENGTH_OFFSET                     (MAC_WEP1_BASE_ADDRESS + 0x84)
#define	REG_MAC_WEP1_TKIP_CONTROL                      (MAC_WEP1_BASE_ADDRESS + 0x88)
#define	REG_MAC_WEP1_TKIP_MIC_CONTROL                  (MAC_WEP1_BASE_ADDRESS + 0x8C)
#define	REG_MAC_WEP1_WEP_CONTROL                       (MAC_WEP1_BASE_ADDRESS + 0x90)
#define	REG_MAC_WEP1_TKIP_FRAGMENT_CONTROL             (MAC_WEP1_BASE_ADDRESS + 0x94)
#define	REG_MAC_WEP1_SHRAM_MASK_WRITE                  (MAC_WEP1_BASE_ADDRESS + 0x98)
#define	REG_MAC_WEP1_DATA_ADDRESS_ENCRYPT              (MAC_WEP1_BASE_ADDRESS + 0x9C)
#define	REG_MAC_WEP1_IV_GENERATE_VALUE                 (MAC_WEP1_BASE_ADDRESS + 0xA0)
#define	REG_MAC_WEP1_SM_DEBUG_REGISTER                 (MAC_WEP1_BASE_ADDRESS + 0xA4)
#define	REG_MAC_WEP1_WIRELESS_HEADER_LENGTH            (MAC_WEP1_BASE_ADDRESS + 0xA8)
#define	REG_MAC_WEP1_WAPI_PROG_REGS_MODE_ENABLE        (MAC_WEP1_BASE_ADDRESS + 0xAC)
#define	REG_MAC_WEP1_WAPI_PROG_REGS_MODE_CONTROL_WO    (MAC_WEP1_BASE_ADDRESS + 0xB0)
#define	REG_MAC_WEP1_WAPI_PROG_REGS_MODE_CONTROL_RO    (MAC_WEP1_BASE_ADDRESS + 0xB4)
#define	REG_MAC_WEP1_WAPI_READ_RESULT_31TO0            (MAC_WEP1_BASE_ADDRESS + 0xB8)
#define	REG_MAC_WEP1_WAPI_READ_RESULT_63TO32           (MAC_WEP1_BASE_ADDRESS + 0xBC)
#define	REG_MAC_WEP1_WAPI_READ_RESULT_95TO64           (MAC_WEP1_BASE_ADDRESS + 0xC0)
#define	REG_MAC_WEP1_WAPI_READ_RESULT_127TO96          (MAC_WEP1_BASE_ADDRESS + 0xC4)
#define	REG_MAC_WEP1_WAPI_IV_95TO64                    (MAC_WEP1_BASE_ADDRESS + 0xC8)
#define	REG_MAC_WEP1_WAPI_IV_127TO96                   (MAC_WEP1_BASE_ADDRESS + 0xCC)
#define	REG_MAC_WEP1_WAPI_FC_MASK                      (MAC_WEP1_BASE_ADDRESS + 0xD0)
#define	REG_MAC_WEP1_WAPI_SC_MASK                      (MAC_WEP1_BASE_ADDRESS + 0xD4)
#define	REG_MAC_WEP1_WAPI_ENABLE                       (MAC_WEP1_BASE_ADDRESS + 0xD8)
#define	REG_MAC_WEP1_FRAGMENT_DB_BASE_ADDR             (MAC_WEP1_BASE_ADDRESS + 0xDC)
#define	REG_MAC_WEP1_TKIP_FRAGMENT2_CONTROL            (MAC_WEP1_BASE_ADDRESS + 0xE0)
#define	REG_MAC_WEP1_TX_DESC_LIST_BASE_ADDR            (MAC_WEP1_BASE_ADDRESS + 0xE4)
#define	REG_MAC_WEP1_TX_DESC_DEBUG                     (MAC_WEP1_BASE_ADDRESS + 0xE8)
#define	REG_MAC_WEP1_FRAGMENT_ENTRY_NUM                (MAC_WEP1_BASE_ADDRESS + 0xEC)
#define	REG_MAC_WEP1_TKIP_FRAGMENT3_CONTROL            (MAC_WEP1_BASE_ADDRESS + 0xF0)
#define	REG_MAC_WEP1_TX_INTERRUPTS_EN                  (MAC_WEP1_BASE_ADDRESS + 0xF4)
#define	REG_MAC_WEP1_TX_INTERRUPTS_STATUS              (MAC_WEP1_BASE_ADDRESS + 0xF8)
#define	REG_MAC_WEP1_TX_INTERRUPTS_CLEAR               (MAC_WEP1_BASE_ADDRESS + 0xFC)
#define	REG_MAC_WEP1_RX_DESC_LIST_BASE_ADDR            (MAC_WEP1_BASE_ADDRESS + 0x100)
#define	REG_MAC_WEP1_RX_DESC_DEBUG                     (MAC_WEP1_BASE_ADDRESS + 0x104)
#define	REG_MAC_WEP1_RX_INTERRUPTS_EN                  (MAC_WEP1_BASE_ADDRESS + 0x108)
#define	REG_MAC_WEP1_RX_INTERRUPTS_STATUS              (MAC_WEP1_BASE_ADDRESS + 0x10C)
#define	REG_MAC_WEP1_RX_INTERRUPTS_CLEAR               (MAC_WEP1_BASE_ADDRESS + 0x110)
#define	REG_MAC_WEP1_RX_NEW_DESC_REQ_STRB              (MAC_WEP1_BASE_ADDRESS + 0x114)
#define	REG_MAC_WEP1_RX_DESC_DONE_ACC_CLR_STRB         (MAC_WEP1_BASE_ADDRESS + 0x118)
#define	REG_MAC_WEP1_RX_DESC_DONE_ACC_SUB_STRB         (MAC_WEP1_BASE_ADDRESS + 0x11C)
#define	REG_MAC_WEP1_RX_DESC_DONE_ACC_RESULT           (MAC_WEP1_BASE_ADDRESS + 0x120)
#define	REG_MAC_WEP1_SEC_LOGGER                        (MAC_WEP1_BASE_ADDRESS + 0x124)
#define	REG_MAC_WEP1_SEC_LOGGER_ACTIVE                 (MAC_WEP1_BASE_ADDRESS + 0x128)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_WEP1_CONTROL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 start:1;	// start
		uint32 controlReg:15;	// Control register
		uint32 reserved0:16;
	} bitFields;
} RegMacWep1Control_u;

/*REG_MAC_WEP1_STATUS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 active:1;	// WEP2  active
		uint32 crcfail:1;	// WEP2  crcfail
		uint32 micfail:1;	// WEP2  micfail
		uint32 notKeyValid:1;	// not_key_valid
		uint32 notProtectedFrame:1;	// not_protected_frame
		uint32 lengthError:1;	// length_error
		uint32 eivFieldError:1;	// eiv_field_error
		uint32 fragLengthShortError:1;	// frag_length_short_error
		uint32 fragNumNewEqualsPrevError:1;	// frag_num_new_equals_prev_error
		uint32 reserved0:23;
	} bitFields;
} RegMacWep1Status_u;

/*REG_MAC_WEP1_SRC_ADDRESS 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 srcAddress:24;	// WEP2  src address
		uint32 encModeConfig:3;	// enc_mode
		uint32 reserved0:1;
		uint32 keyId:2;	// key_id
		uint32 sppAMsduCapable:1;	// spp_a_msdu_capable
		uint32 encryptMode:1;	// encrypt
	} bitFields;
} RegMacWep1SrcAddress_u;

/*REG_MAC_WEP1_DST_ADDRESS 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dstAddress:24;	// WEP2  dst address
		uint32 reserved0:8;
	} bitFields;
} RegMacWep1DstAddress_u;

/*REG_MAC_WEP1_CUR_ADDRESS_OFFSET 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 curAddressOffset:16;	// WEP2  cur address offset
		uint32 reserved0:16;
	} bitFields;
} RegMacWep1CurAddressOffset_u;

/*REG_MAC_WEP1_LENGTH 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 length:16;	// WEP2  length
		uint32 reserved0:16;
	} bitFields;
} RegMacWep1Length_u;

/*REG_MAC_WEP1_KEY_LENGTH 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 keyLength:6;	// WEP2  key length
		uint32 reserved0:26;
	} bitFields;
} RegMacWep1KeyLength_u;

/*REG_MAC_WEP1_KEY_31TO0 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 key31To0:32;	// WEP2  key 31to0
	} bitFields;
} RegMacWep1Key31To0_u;

/*REG_MAC_WEP1_KEY_63TO32 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 key63To32:32;	// WEP2  key 63to32
	} bitFields;
} RegMacWep1Key63To32_u;

/*REG_MAC_WEP1_KEY_95TO64 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 key95To64:32;	// WEP2  key 95to64
	} bitFields;
} RegMacWep1Key95To64_u;

/*REG_MAC_WEP1_KEY_127TO96 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 key127To96:32;	// WEP2  key 127to96
	} bitFields;
} RegMacWep1Key127To96_u;

/*REG_MAC_WEP1_CRC_RESULT 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcResult:32;	// WEP2  crc result
	} bitFields;
} RegMacWep1CrcResult_u;

/*REG_MAC_WEP1_DESC_CLEAR_STATE 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNumValidDescClearPulse:1;	// Tx num valid desc clear pulse
		uint32 txDescPointerClearPulse:1;	// Tx desc pointer clear pulse
		uint32 rxNumValidDescClearPulse:1;	// Rx num valid desc clear pulse
		uint32 rxDescPointerClearPulse:1;	// Rx desc pointer clear pulse
		uint32 txCcmpGcmpAbortPulse:1;	// Tx CCMP/GCMP abort operation pulse
		uint32 reserved0:27;
	} bitFields;
} RegMacWep1DescClearState_u;

/*REG_MAC_WEP1_WRITE10 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write10:32;	// write10_w0
	} bitFields;
} RegMacWep1Write10_u;

/*REG_MAC_WEP1_WRITE11 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write11:32;	// write11_w0
	} bitFields;
} RegMacWep1Write11_u;

/*REG_MAC_WEP1_MODE 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mode:3;	// Security mode
		uint32 reserved0:29;
	} bitFields;
} RegMacWep1Mode_u;

/*REG_MAC_WEP1_WRITE12 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write12:32;	// write12_w0
	} bitFields;
} RegMacWep1Write12_u;

/*REG_MAC_WEP1_WRITE13 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write13:32;	// write13_w0
	} bitFields;
} RegMacWep1Write13_u;

/*REG_MAC_WEP1_WRITE14 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write14:32;	// write14_w0
	} bitFields;
} RegMacWep1Write14_u;

/*REG_MAC_WEP1_WRITE15 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write15:32;	// write15_w0
	} bitFields;
} RegMacWep1Write15_u;

/*REG_MAC_WEP1_WRITE16 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 write16:32;	// write16_w0
	} bitFields;
} RegMacWep1Write16_u;

/*REG_MAC_WEP1_READ5 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 read5:32;	// read5_w0
	} bitFields;
} RegMacWep1Read5_u;

/*REG_MAC_WEP1_READ6 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 read6:32;	// read6_w0
	} bitFields;
} RegMacWep1Read6_u;

/*REG_MAC_WEP1_READ7 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 read7:32;	// read7_w0
	} bitFields;
} RegMacWep1Read7_u;

/*REG_MAC_WEP1_HARDWARE_ID_W0 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hardwareIdAes:1;	// hardware id AES
		uint32 hardwareIdOcb:1;	// hardware id OCB
		uint32 hardwareIdCcm:1;	// hardware id CCM
		uint32 hardwareIdWapi:1;	// hardware id WAPI
		uint32 reserved0:28;
	} bitFields;
} RegMacWep1HardwareIdW0_u;

/*REG_MAC_WEP1_CCM_FC_MASK 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccmFcMask:16;	// ccm_fc_mask
		uint32 ccmFcManagementMask:16;	// ccm_fc_management_mask
	} bitFields;
} RegMacWep1CcmFcMask_u;

/*REG_MAC_WEP1_CCM_SC_MASK 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccmScMask:16;	// ccm_sc_mask
		uint32 reserved0:16;
	} bitFields;
} RegMacWep1CcmScMask_u;

/*REG_MAC_WEP1_CCM_QC_MASK 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ccmQcMask:16;	// ccm_qc_mask
		uint32 managementNonceMask:1;	// management_nonce_mask
		uint32 reserved0:15;
	} bitFields;
} RegMacWep1CcmQcMask_u;

/*REG_MAC_WEP1_MUTED_HDR_ADDRESS 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mutedHdrAddress:24;	// muted_hdr_address
		uint32 reserved0:8;
	} bitFields;
} RegMacWep1MutedHdrAddress_u;

/*REG_MAC_WEP1_FULL_HW_ENC 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fullHwEnc:1;	// full_hw_enc
		uint32 rc4PreInitHwEn:1;	// rc4_pre_init_hw_en
		uint32 descriptorListEn:1;	// descriptor_list_en
		uint32 txDescModeGenErrIrqEn:1;	// tx_desc_mode_gen_err_irq_en
		uint32 rxDescModeGenErrIrqEn:1;	// rx_desc_mode_gen_err_irq_en
		uint32 reserved0:27;
	} bitFields;
} RegMacWep1FullHwEnc_u;

/*REG_MAC_WEP1_START_ENTRIES_ADDRESS 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startEntriesAddress:24;	// start_entries_address
		uint32 dbEntrySize:4;	// entry_length
		uint32 differentDest:1;	// different_dest
		uint32 prepareIv:1;	// prepare_iv
		uint32 reserved0:2;
	} bitFields;
} RegMacWep1StartEntriesAddress_u;

/*REG_MAC_WEP1_ENTRY_NUMBER 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entryNumber:11;	// entry_number
		uint32 reserved0:5;
		uint32 micEntryNumber:11;	// mic_entry_number
		uint32 reserved1:5;
	} bitFields;
} RegMacWep1EntryNumber_u;

/*REG_MAC_WEP1_LENGTH_OFFSET 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lengthOffsetEncrypt:3;	// length_offset_encrypt
		uint32 reserved0:1;
		uint32 lengthOffsetDecrypt:3;	// length_offset_decrypt
		uint32 reserved1:25;
	} bitFields;
} RegMacWep1LengthOffset_u;

/*REG_MAC_WEP1_TKIP_CONTROL 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tkipControlDec:16;	// tkip_control_dec
		uint32 tkipControlEnc:16;	// tkip_control_enc
	} bitFields;
} RegMacWep1TkipControl_u;

/*REG_MAC_WEP1_TKIP_MIC_CONTROL 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tkipMicControlHeader:16;	// tkip_mic_control_header
		uint32 reserved0:16;
	} bitFields;
} RegMacWep1TkipMicControl_u;

/*REG_MAC_WEP1_WEP_CONTROL 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wepControlDec:16;	// wep_control_dec
		uint32 wepControlEnc:16;	// wep_control_enc
	} bitFields;
} RegMacWep1WepControl_u;

/*REG_MAC_WEP1_TKIP_FRAGMENT_CONTROL 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tkipControlFragmentDec:16;	// tkip_control_fragment_dec
		uint32 tkipControlFragmentEnc:16;	// tkip_control_fragment_enc
	} bitFields;
} RegMacWep1TkipFragmentControl_u;

/*REG_MAC_WEP1_SHRAM_MASK_WRITE 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramMaskWrite:1;	// shram_mask_write
		uint32 reserved0:31;
	} bitFields;
} RegMacWep1ShramMaskWrite_u;

/*REG_MAC_WEP1_DATA_ADDRESS_ENCRYPT 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dataAddressEncrypt:24;	// data_address_encrypt
		uint32 reserved0:8;
	} bitFields;
} RegMacWep1DataAddressEncrypt_u;

/*REG_MAC_WEP1_IV_GENERATE_VALUE 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ivGenerateValue:32;	// iv_generate_value
	} bitFields;
} RegMacWep1IvGenerateValue_u;

/*REG_MAC_WEP1_SM_DEBUG_REGISTER 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mutedHdrStateMachine:4;	// muted_hdr_state_machine
		uint32 encStateMachine:5;	// enc_state_machine
		uint32 wepStateMachine:4;	// wep_state_machine
		uint32 ccmpStateMachine:4;	// ccmp_state_machine
		uint32 wapiMainStateMachine:3;	// wapi_main_state_machine
		uint32 wapiPart2StateMachine:4;	// wapi_part2_state_machine
		uint32 gcmpStateMachine:3;	// gcmp_state_machine
		uint32 descriptorStateMachine:3;	// descriptor_state_machine
		uint32 descriptorArbStateMachine:2;	// descriptor_arb_state_machine
	} bitFields;
} RegMacWep1SmDebugRegister_u;

/*REG_MAC_WEP1_WIRELESS_HEADER_LENGTH 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wirelessHeaderLength:6;	// wireless_header_length
		uint32 reserved0:26;
	} bitFields;
} RegMacWep1WirelessHeaderLength_u;

/*REG_MAC_WEP1_WAPI_PROG_REGS_MODE_ENABLE 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiProgRegsModeEnable:1;	// wapi_prog_regs_mode_enable
		uint32 reserved0:31;
	} bitFields;
} RegMacWep1WapiProgRegsModeEnable_u;

/*REG_MAC_WEP1_WAPI_PROG_REGS_MODE_CONTROL_WO 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiProgRegsModeWren:1;	// wapi_prog_regs_mode_wren
		uint32 wapiProgRegsModeRden:1;	// wapi_prog_regs_mode_rden
		uint32 reserved0:30;
	} bitFields;
} RegMacWep1WapiProgRegsModeControlWo_u;

/*REG_MAC_WEP1_WAPI_PROG_REGS_MODE_CONTROL_RO 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiProgRegsModeRdyin:1;	// wapi_prog_regs_mode_rdyin
		uint32 wapiProgRegsModeRdyout:1;	// wapi_prog_regs_mode_rdyout
		uint32 reserved0:30;
	} bitFields;
} RegMacWep1WapiProgRegsModeControlRo_u;

/*REG_MAC_WEP1_WAPI_READ_RESULT_31TO0 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiReadResult31To0:32;	// wapi_read_result_31to0
	} bitFields;
} RegMacWep1WapiReadResult31To0_u;

/*REG_MAC_WEP1_WAPI_READ_RESULT_63TO32 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiReadResult63To32:32;	// wapi_read_result_63to32
	} bitFields;
} RegMacWep1WapiReadResult63To32_u;

/*REG_MAC_WEP1_WAPI_READ_RESULT_95TO64 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiReadResult95To64:32;	// wapi_read_result_95to64
	} bitFields;
} RegMacWep1WapiReadResult95To64_u;

/*REG_MAC_WEP1_WAPI_READ_RESULT_127TO96 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiReadResult127To96:32;	// wapi_read_result_127to96
	} bitFields;
} RegMacWep1WapiReadResult127To96_u;

/*REG_MAC_WEP1_WAPI_IV_95TO64 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiIv95To64:32;	// wapi_iv_95to64
	} bitFields;
} RegMacWep1WapiIv95To64_u;

/*REG_MAC_WEP1_WAPI_IV_127TO96 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiIv127To96:32;	// wapi_iv_127to96
	} bitFields;
} RegMacWep1WapiIv127To96_u;

/*REG_MAC_WEP1_WAPI_FC_MASK 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiFcMask:16;	// wapi_fc_mask
		uint32 reserved0:16;
	} bitFields;
} RegMacWep1WapiFcMask_u;

/*REG_MAC_WEP1_WAPI_SC_MASK 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiScMask:16;	// wapi_sc_mask
		uint32 reserved0:16;
	} bitFields;
} RegMacWep1WapiScMask_u;

/*REG_MAC_WEP1_WAPI_ENABLE 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wapiEnable:1;	// wapi_enable
		uint32 reserved0:31;
	} bitFields;
} RegMacWep1WapiEnable_u;

/*REG_MAC_WEP1_FRAGMENT_DB_BASE_ADDR 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentDbBaseAddr:24;	// fragment_db_base_addr
		uint32 reserved0:8;
	} bitFields;
} RegMacWep1FragmentDbBaseAddr_u;

/*REG_MAC_WEP1_TKIP_FRAGMENT2_CONTROL 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tkipMicControlFirstFrag:16;	// tkip_mic_control_first_frag
		uint32 tkipMicControlLastFrag:16;	// tkip_mic_control_last_frag
	} bitFields;
} RegMacWep1TkipFragment2Control_u;

/*REG_MAC_WEP1_TX_DESC_LIST_BASE_ADDR 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDescListBaseAddr:24;	// tx_desc_list_base_addr
		uint32 reserved0:8;
	} bitFields;
} RegMacWep1TxDescListBaseAddr_u;

/*REG_MAC_WEP1_TX_DESC_DEBUG 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNumValidDescriptors:8;	// tx_num_valid_descriptors
		uint32 txDescriptorPointer:8;	// tx_descriptor_pointer
		uint32 txCcmpGcmpAbortReq:1;	// Tx CCMP/GCMP abort operation request
		uint32 reserved0:15;
	} bitFields;
} RegMacWep1TxDescDebug_u;

/*REG_MAC_WEP1_FRAGMENT_ENTRY_NUM 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fragmentEntryNum:11;	// fragment_entry_num
		uint32 reserved0:21;
	} bitFields;
} RegMacWep1FragmentEntryNum_u;

/*REG_MAC_WEP1_TKIP_FRAGMENT3_CONTROL 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tkipMicControlMiddleFrag:16;	// tkip_mic_control_middle_frag
		uint32 reserved0:16;
	} bitFields;
} RegMacWep1TkipFragment3Control_u;

/*REG_MAC_WEP1_TX_INTERRUPTS_EN 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 txInterruptsEn:8;	// Tx Interrupts enable
		uint32 reserved1:23;
	} bitFields;
} RegMacWep1TxInterruptsEn_u;

/*REG_MAC_WEP1_TX_INTERRUPTS_STATUS 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDoneIndicationIrq:1;	// tx_done_indication_irq
		uint32 txFailureIndicationIrq:1;	// tx_failure_indication_irq
		uint32 txMicFailureIrq:1;	// tx_mic_failure_irq
		uint32 txNotKeyValidIrq:1;	// tx_not_key_valid_irq
		uint32 txNotProtectedFrameIrq:1;	// tx_not_protected_frame_irq
		uint32 txLengthErrorIrq:1;	// tx_length_error_irq
		uint32 txEivFieldErrorIrq:1;	// tx_eiv_field_error_irq
		uint32 txFragLengthShortErrorIrq:1;	// tx_frag_length_short_error_irq
		uint32 txFragNumNewEqualsPrevIrq:1;	// tx_frag_num_new_equals_prev_irq
		uint32 reserved0:23;
	} bitFields;
} RegMacWep1TxInterruptsStatus_u;

/*REG_MAC_WEP1_TX_INTERRUPTS_CLEAR 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txInterruptsClear:9;	// Tx Interrupts clear
		uint32 reserved0:23;
	} bitFields;
} RegMacWep1TxInterruptsClear_u;

/*REG_MAC_WEP1_RX_DESC_LIST_BASE_ADDR 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDescListBaseAddr:24;	// rx_desc_list_base_addr
		uint32 reserved0:8;
	} bitFields;
} RegMacWep1RxDescListBaseAddr_u;

/*REG_MAC_WEP1_RX_DESC_DEBUG 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxNumValidDescriptors:8;	// rx_num_valid_descriptors
		uint32 rxDescriptorPointer:8;	// rx_descriptor_pointer
		uint32 reserved0:16;
	} bitFields;
} RegMacWep1RxDescDebug_u;

/*REG_MAC_WEP1_RX_INTERRUPTS_EN 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 rxInterruptsEn:8;	// Rx Interrupts enable
		uint32 reserved1:23;
	} bitFields;
} RegMacWep1RxInterruptsEn_u;

/*REG_MAC_WEP1_RX_INTERRUPTS_STATUS 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDoneIndicationIrq:1;	// rx_done_indication_irq
		uint32 rxFailureIndicationIrq:1;	// rx_failure_indication_irq
		uint32 rxMicFailureIrq:1;	// rx_mic_failure_irq
		uint32 rxNotKeyValidIrq:1;	// rx_not_key_valid_irq
		uint32 rxNotProtectedFrameIrq:1;	// rx_not_protected_frame_irq
		uint32 rxLengthErrorIrq:1;	// rx_length_error_irq
		uint32 rxEivFieldErrorIrq:1;	// rx_eiv_field_error_irq
		uint32 rxFragLengthShortErrorIrq:1;	// rx_frag_length_short_error_irq
		uint32 rxFragNumNewEqualsPrevIrq:1;	// rx_frag_num_new_equals_prev_irq
		uint32 reserved0:23;
	} bitFields;
} RegMacWep1RxInterruptsStatus_u;

/*REG_MAC_WEP1_RX_INTERRUPTS_CLEAR 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxInterruptsClear:9;	// Rx Interrupts clear
		uint32 reserved0:23;
	} bitFields;
} RegMacWep1RxInterruptsClear_u;

/*REG_MAC_WEP1_RX_NEW_DESC_REQ_STRB 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxNewDescReqStrb:1;	// Add new Rx descriptor
		uint32 reserved0:31;
	} bitFields;
} RegMacWep1RxNewDescReqStrb_u;

/*REG_MAC_WEP1_RX_DESC_DONE_ACC_CLR_STRB 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDescDoneAccClrStrb:1;	// Rx descriptor done accumulator clear
		uint32 reserved0:31;
	} bitFields;
} RegMacWep1RxDescDoneAccClrStrb_u;

/*REG_MAC_WEP1_RX_DESC_DONE_ACC_SUB_STRB 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDescDoneAccSubStrb:1;	// Rx descriptor done accumulator subtract
		uint32 reserved0:31;
	} bitFields;
} RegMacWep1RxDescDoneAccSubStrb_u;

/*REG_MAC_WEP1_RX_DESC_DONE_ACC_RESULT 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDescDoneAccResult:8;	// Rx descriptor done accumulator result
		uint32 reserved0:24;
	} bitFields;
} RegMacWep1RxDescDoneAccResult_u;

/*REG_MAC_WEP1_SEC_LOGGER 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 secLoggerEn:1;	// Security engine logger enable
		uint32 reserved0:7;
		uint32 secLoggerPriority:2;	// Security engine logger priority
		uint32 reserved1:22;
	} bitFields;
} RegMacWep1SecLogger_u;

/*REG_MAC_WEP1_SEC_LOGGER_ACTIVE 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 secLoggerActive:1;	// Security engine logger active
		uint32 reserved0:31;
	} bitFields;
} RegMacWep1SecLoggerActive_u;



#endif // _MAC_WEP1_REGS_H_
